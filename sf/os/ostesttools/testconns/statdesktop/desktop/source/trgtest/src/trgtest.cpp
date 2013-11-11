/*
* Copyright (c) 1994-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* e32utils\trgtest\trgtesth.cpp
*
*/


#include <e32err.h>
#include <windows.h>
#include <shlwapi.h>
#include <stdio.h>
#include <time.h>

#pragma comment(lib, "shlwapi.lib")

const TUint8 SOH=0x01;
const TUint8 STX=0x02;
const TUint8 EOT=0x04;
const TUint8 ACK=0x06;
const TUint8 NAK=0x15;
const TUint8 CAN=0x18;
const TUint8 SUB=0x1A;
const TUint8 BIGC=0x43;
const TUint8 BIGG=0x47;

// Try 115200bps first since it is the most commonly supported baud rate.  Newer platforms like the
// NaviEngine support 230400bps and above.
const TUint KDefaultBaudRate = 115200;
const TUint KBaudRateSearchList[] = {KDefaultBaudRate, 230400, 460800};

#define READ_BUF_SIZE	32768
#define WRITE_BUF_SIZE	32768
#define MAX_LINE		32768
#define PACKET_SIZE		1024

#define MIN(a,b)		((a)<(b)?(a):(b))
#define OFFSET(p,off)	((void*)((char*)p+off))

#define RESET_COMM()	PurgeComm(Comm, PURGE_RXCLEAR|PURGE_TXCLEAR)

const TInt	KExitCodeOK							= 0;		// no failure
const TInt	KExitCodeUsage						= 1;		// command like usage
const TInt	KExitCodeLog						= 2;		// can't open log file
const TInt	KExitCodeImage						= 3;		// can't open image file
const TInt	KExitCodeSerial						= 4;		// can't open serial port
const TInt	KExitCodeDownload					= 5;		// download started, but failed
const TInt	KExitCodeFaulted					= 6;		// entered debug monitor abnormally
const TInt	KExitCodeHung						= 7;		// target hung
const TInt	KExitCodeDOA						= 8;		// board was in debug monitor at start
const TInt	KExitCodeUnsupportedBaudRate		= 9;		// the baud rate set by the -b parameter is not supported by the PC's COM port
const TInt	KExitCodeAssert						= 99;		// trgtest assertion failed

FILE* LogFile = NULL;
HANDLE Comm = INVALID_HANDLE_VALUE;

char ImgFileName[MAX_PATH] = "";
TInt ImgFileSize = 0;
char LineBuf[MAX_LINE+1];
void* ImgFileChunkBase = NULL;
TBool use_stdout = false;

enum EDebugMonitorOptions
{
	kDebugDumpCodeSegs = 1
};

TUint DebugMonitorOptions = kDebugDumpCodeSegs;

#define TT_ASSERT(x)	__ASSERT_ALWAYS(x, __Panic(__LINE__))

const char* GetWinErrMsg(DWORD errorCode)
	{
	static char lastErrorBuffer[512]; // not thread safe or reentrant (but shouldn't need to be)
	strcpy(lastErrorBuffer, "Unknown error");
	
	if(errorCode != ERROR_SUCCESS)
		{
		if(FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, errorCode, 0, lastErrorBuffer, 512, NULL) != 0)
			{
			size_t size = strlen(lastErrorBuffer);
			
			// Strip any trailing carriage return.
			if(StrCmpNI(lastErrorBuffer + size - 2, "\r\n", 2) == 0)
				{
				lastErrorBuffer[size - 2] = '\0';
				size -= 2;
				}

			// Strip any trailing period.
			if(lastErrorBuffer[size - 1] == '.')
				{
				lastErrorBuffer[size - 1] = '\0';
				}
			}
		}
	
	return lastErrorBuffer;
	}

const char* GetSymOrWinErrMsg(TInt errorCode)
	{
	static char lastErrorBuffer[512]; // not thread safe or reentrant (but shouldn't need to be)
	strcpy(lastErrorBuffer, "Unknown error");
	
	switch(errorCode)
		{
		case KErrNone:
			{
			// Unknown error
			}
		break;
		case KErrEof:
			{
			strcpy(lastErrorBuffer, "Unexpected end of file");
			}
		break;
		case KErrTimedOut:
			{
			strcpy(lastErrorBuffer, "Timed-out");
			}
		break;
		default:
			{
			if(errorCode > 0)
				{
				strcpy(lastErrorBuffer, GetWinErrMsg(errorCode));
				}
			}
		break;
		}
	
	return lastErrorBuffer;
	}

void __Panic(TInt aLine)
	{
	fprintf(stderr, "Assertion failed at line %d\n", aLine);
	exit(KExitCodeAssert);
	}

void WriteLog(const void* aBuf, TInt aLength)
	{
	if (LogFile)
		fwrite(aBuf, 1, aLength, LogFile);
	}

void WriteLogS(const char* aString)
	{
	WriteLog( aString, strlen(aString) );
	}

void TraceLog(const char* aFmt, ...)
	{
	char buf[512];
	va_list list;
	va_start(list, aFmt);
	if (!LogFile)
		return;
	sprintf(buf, "TRGTEST: ");
	vsprintf(buf, aFmt, list);
	strcat(buf, "\r\n");
	WriteLogS(buf);
	}

/*
YModem packet structure:
Byte 0 = STX
Byte 1 = sequence number (first user data packet is 1)
Byte 2 = complement of sequence number
Bytes 3-1026 = data (1K per packet)
Bytes 1027, 1028 = 16-bit CRC (big-endian)

A herald packet is sent first, with sequence number 0
The data field consists of the null-terminated file name
followed by the null-terminated file size in ASCII decimal
digits.
*/
struct SPacket
	{
	TUint8 iPTI;
	TUint8 iSeq;
	TUint8 iSeqBar;
	TUint8 iData[PACKET_SIZE];
	TUint8 iCRC1;
	TUint8 iCRC0;
	};

static const TUint16 crcTab[256] =
    {
	0x0000,0x1021,0x2042,0x3063,0x4084,0x50a5,0x60c6,0x70e7,0x8108,0x9129,0xa14a,
	0xb16b,0xc18c,0xd1ad,0xe1ce,0xf1ef,0x1231,0x0210,0x3273,0x2252,0x52b5,0x4294,
	0x72f7,0x62d6,0x9339,0x8318,0xb37b,0xa35a,0xd3bd,0xc39c,0xf3ff,0xe3de,0x2462,
	0x3443,0x0420,0x1401,0x64e6,0x74c7,0x44a4,0x5485,0xa56a,0xb54b,0x8528,0x9509,
	0xe5ee,0xf5cf,0xc5ac,0xd58d,0x3653,0x2672,0x1611,0x0630,0x76d7,0x66f6,0x5695,
	0x46b4,0xb75b,0xa77a,0x9719,0x8738,0xf7df,0xe7fe,0xd79d,0xc7bc,0x48c4,0x58e5,
	0x6886,0x78a7,0x0840,0x1861,0x2802,0x3823,0xc9cc,0xd9ed,0xe98e,0xf9af,0x8948,
	0x9969,0xa90a,0xb92b,0x5af5,0x4ad4,0x7ab7,0x6a96,0x1a71,0x0a50,0x3a33,0x2a12,
	0xdbfd,0xcbdc,0xfbbf,0xeb9e,0x9b79,0x8b58,0xbb3b,0xab1a,0x6ca6,0x7c87,0x4ce4,
	0x5cc5,0x2c22,0x3c03,0x0c60,0x1c41,0xedae,0xfd8f,0xcdec,0xddcd,0xad2a,0xbd0b,
	0x8d68,0x9d49,0x7e97,0x6eb6,0x5ed5,0x4ef4,0x3e13,0x2e32,0x1e51,0x0e70,0xff9f,
	0xefbe,0xdfdd,0xcffc,0xbf1b,0xaf3a,0x9f59,0x8f78,0x9188,0x81a9,0xb1ca,0xa1eb,
	0xd10c,0xc12d,0xf14e,0xe16f,0x1080,0x00a1,0x30c2,0x20e3,0x5004,0x4025,0x7046,
	0x6067,0x83b9,0x9398,0xa3fb,0xb3da,0xc33d,0xd31c,0xe37f,0xf35e,0x02b1,0x1290,
	0x22f3,0x32d2,0x4235,0x5214,0x6277,0x7256,0xb5ea,0xa5cb,0x95a8,0x8589,0xf56e,
	0xe54f,0xd52c,0xc50d,0x34e2,0x24c3,0x14a0,0x0481,0x7466,0x6447,0x5424,0x4405,
	0xa7db,0xb7fa,0x8799,0x97b8,0xe75f,0xf77e,0xc71d,0xd73c,0x26d3,0x36f2,0x0691,
	0x16b0,0x6657,0x7676,0x4615,0x5634,0xd94c,0xc96d,0xf90e,0xe92f,0x99c8,0x89e9,
	0xb98a,0xa9ab,0x5844,0x4865,0x7806,0x6827,0x18c0,0x08e1,0x3882,0x28a3,0xcb7d,
	0xdb5c,0xeb3f,0xfb1e,0x8bf9,0x9bd8,0xabbb,0xbb9a,0x4a75,0x5a54,0x6a37,0x7a16,
	0x0af1,0x1ad0,0x2ab3,0x3a92,0xfd2e,0xed0f,0xdd6c,0xcd4d,0xbdaa,0xad8b,0x9de8,
	0x8dc9,0x7c26,0x6c07,0x5c64,0x4c45,0x3ca2,0x2c83,0x1ce0,0x0cc1,0xef1f,0xff3e,
	0xcf5d,0xdf7c,0xaf9b,0xbfba,0x8fd9,0x9ff8,0x6e17,0x7e36,0x4e55,0x5e74,0x2e93,
	0x3eb2,0x0ed1,0x1ef0
    };

void UpdateCrc(const void* aPtr, TInt aLength, TUint16& aCrc)
//
// Perform a CCITT CRC checksum.
//
	{

	register const TUint8* pB = (const TUint8*)aPtr;
	register TUint16 crc=aCrc;
    while (aLength--)
		crc=TUint16((crc<<8)^crcTab[(crc>>8)^*pB++]);
	aCrc=crc;
	}

void ClearCommError()
	{
	DWORD err;
	COMSTAT s;
	TT_ASSERT(ClearCommError(Comm,&err,&s));
	}
	
void OpenCommPort(TInt aPort)
	{
	char buf[20];

	sprintf(buf, "\\\\.\\COM%d", aPort); // COM ports with indexes higher than 9 can only be referenced by prefixing the device name with "\\.\\"
	Comm = CreateFile(buf, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (Comm == INVALID_HANDLE_VALUE)
		{
		if(GetLastError() == ERROR_ACCESS_DENIED)
			{
			fprintf(stderr, "Can't open COM%d: Error %d (Access denied) - a terminal application has probably been left running with the port still open\n", aPort, ERROR_ACCESS_DENIED);
			}
		else
			{
			fprintf(stderr, "Can't open COM%d: Error %d (%s)\n", aPort, GetLastError(), GetWinErrMsg(GetLastError()));
			}
		exit(KExitCodeSerial);
		}

	// Raise our thread priority to avoid being starved if builds are continuing in parallel while we are communicating with the target
	// at high speeds.  TrgTest does not use serial flow control so if the buffers fill up, data will be lost.  We do not try to use the
	// real-time process priority class because it requires administrator privileges.
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
	}
	
TInt SetupCommPort(TInt aPort, TUint aBaudRate, TBool aIgnoreInvalidBaudRateError = false)
	{
	TInt r = KErrNone;
	const char* errFuncName = NULL;

	DCB dcb;

	dcb.DCBlength = sizeof(dcb);
	if(!GetCommState(Comm, &dcb))
		{
		r = GetLastError();
		errFuncName = "GetCommState";
		}
	else if(!SetCommMask(Comm, EV_ERR|EV_RXCHAR))
		{
		r = GetLastError();
		errFuncName = "SetCommMask";
		}
	else if(!SetupComm(Comm, READ_BUF_SIZE, WRITE_BUF_SIZE))
		{
		r = GetLastError();
		errFuncName = "SetupComm";
		}
	if(r != KErrNone)
		{
		TT_ASSERT(errFuncName);
		
		fprintf(stderr, "Error in SetupComm: Error %d (%s) calling %s\n", r, GetWinErrMsg(r), errFuncName);
		TraceLog("Error in SetupComm: Error %d (%s) calling %s", r, GetWinErrMsg(r), errFuncName);
		fprintf(stderr, "Trying to proceed anyway, be suspicious of any problems!\n");
		TraceLog("Trying to proceed anyway, be suspicious of any problems!");
		}
	ClearCommError();
	r = KErrNone;

	dcb.fAbortOnError = TRUE;
	dcb.BaudRate=aBaudRate;
	dcb.Parity=NOPARITY;
	dcb.fParity = FALSE;
	dcb.fErrorChar = FALSE;
	dcb.ByteSize = 8;
	dcb.StopBits=ONESTOPBIT;
	dcb.fInX  = FALSE;
	dcb.fOutX = FALSE;
	dcb.XonChar = 0;
	dcb.XoffChar = 0;
	dcb.ErrorChar = 0;
	dcb.fOutxDsrFlow = FALSE;
	dcb.fOutxCtsFlow = FALSE;
	dcb.fDsrSensitivity =  FALSE;
	dcb.fRtsControl = RTS_CONTROL_DISABLE;
	dcb.fDtrControl = DTR_CONTROL_DISABLE;
	if(!SetCommState(Comm, &dcb))
		{
		if(GetLastError() == ERROR_INVALID_PARAMETER)
			{
			r = KExitCodeUnsupportedBaudRate;

			if(aIgnoreInvalidBaudRateError)
				{
				return r;
				}
			else
				{
				fprintf(stderr, "Error in SetCommState : Error %d (%s) - a baud rate of %dbps is not supported by COM%d - try a different port\n", GetLastError(), GetWinErrMsg(GetLastError()), aBaudRate, aPort);
				TraceLog("Error in SetCommState : Error %d (%s) - a baud rate of %dbps is not supported by COM%d - try a different port", GetLastError(), GetWinErrMsg(GetLastError()), aBaudRate, aPort);
				}
			}
		else
			{
			fprintf(stderr, "Error in SetCommState : Error %d calling (%s) SetCommState\n", GetLastError(), GetWinErrMsg(GetLastError()), aBaudRate);
			TraceLog("Error in SetCommState : Error %d (%s) SetCommState", GetLastError(), GetWinErrMsg(GetLastError()), aBaudRate);
			r = KExitCodeSerial;
			}

		exit(r);
		}
	EscapeCommFunction(Comm, SETDTR);
	EscapeCommFunction(Comm, SETRTS);
	
	return KErrNone;
	}

void SetupTimeout(TInt aInt, TInt aTot)
	{
	COMMTIMEOUTS ct;
	GetCommTimeouts(Comm, &ct);
	ct.ReadIntervalTimeout = aInt;		// ms
	ct.ReadTotalTimeoutMultiplier = 0;
	ct.ReadTotalTimeoutConstant = aTot;	// ms
	ct.WriteTotalTimeoutConstant = 0;
	ct.WriteTotalTimeoutMultiplier = 0;
	SetCommTimeouts(Comm, &ct);
	}

TInt CommRead1(TInt aTimeout)
	{
	unsigned char c;
	DWORD n=0;
	BOOL ok;

	SetupTimeout(0, aTimeout);
	ok = ReadFile(Comm, &c, 1, &n, NULL);
	if (!ok)
		ClearCommError();
	if (ok && n)
		return c;
	return KErrTimedOut;
	}

TInt CommRead(void* aBuf, TInt aMax, TInt aTimeout)
	{
	DWORD n=0;
	BOOL ok;

	SetupTimeout(100, aTimeout);
	ok = ReadFile(Comm, aBuf, aMax, &n, NULL);
	if (!ok)
		ClearCommError();
	if (n==0)
		return KErrTimedOut;
	return n;
	}

void CommWrite(const void* aBuf, TInt aLen)
	{
	DWORD n = 0;
	BOOL ok = WriteFile(Comm, aBuf, aLen, &n, NULL);
	if (!ok)
		ClearCommError();
	}

void CommWriteC(TUint aChar)
	{
	unsigned char c = (unsigned char)aChar;
	CommWrite(&c, 1);
	}

void CommWriteS(const char* aString)
	{
	CommWrite( aString, strlen(aString) );
	}

TInt PreparePacket(SPacket& pkt, TInt aSeq, TInt& aOutBytesUploaded)
	{
	TInt r = KErrNone;
	TUint16 crc = 0;

	pkt.iPTI = STX;
	pkt.iSeq = (TUint8)(aSeq>=0 ? aSeq : 0);
	pkt.iSeqBar = (TUint8)~pkt.iSeq;
	if (aSeq>0)
		{
		TInt l;
		aOutBytesUploaded = (aSeq-1)*PACKET_SIZE;	// file position of packet
		if ( aOutBytesUploaded >= ImgFileSize )
			return KErrEof;
		l = MIN(PACKET_SIZE, ImgFileSize-aOutBytesUploaded);
		memcpy(pkt.iData, OFFSET(ImgFileChunkBase,aOutBytesUploaded), l);
		if (l<PACKET_SIZE)
			memset(pkt.iData+l, 0, PACKET_SIZE-l);
		aOutBytesUploaded = max( aOutBytesUploaded, 0L );
		}
	else
		{
		aOutBytesUploaded = 0;
		memset(pkt.iData, 0, PACKET_SIZE);
		if (aSeq==0)
			{
			const char* p = ImgFileName;
			const char* q = p + strlen(p);
			while (--q>=p && *q!='\\') {}
			sprintf( (char*)pkt.iData, "%s%c%d", q+1, 0, ImgFileSize);
			}
		}
	UpdateCrc(pkt.iData, PACKET_SIZE, crc);
	pkt.iCRC1 = (TUint8)(crc>>8);
	pkt.iCRC0 = (TUint8)crc;
	return r;
	}

TInt SendPacket(TInt& aSeq, TBool aStream, clock_t startTime)
	{
	TBool ackPacket = aSeq == -1;
	TInt c;
	SPacket pkt;
	TInt retries = 10;
	TInt tmout = (aSeq>=0) ? 2000 : 500;
	TInt bytesUploaded;
	
	// Prepare the next packet.
	TInt r = PreparePacket(pkt, aSeq, bytesUploaded);
		
	if (r==KErrNone)
		{
		// Keep trying to send the packet until it is acknowledged.
		for(;;)
			{
			RESET_COMM();
			CommWrite(&pkt, sizeof(pkt));
			if (aStream)
				break;
			c = CommRead1(tmout);
			if (c==KErrTimedOut && aSeq<0)
				{
				return KErrNone;
				}
			if (c>=0)
				{
				if (c==ACK)
					break;
				}
			if (--retries==0)
				{
				r = KErrTimedOut;
				break;
				}
			}
	
		// Check to see if the send operation was successful.	
		if (r == KErrNone || (r == KErrEof))
			{
			// Verify the stream type for the first packet.
			if(aSeq==0)
				{
				c = CommRead1(100);
				if (c==KErrTimedOut)
					{
					r = KErrNone;
					}
				else if (aStream && c!=BIGG)
					r = KErrTimedOut;
				else if (!aStream && c!=BIGC)
					r = KErrTimedOut;
				}
			
			if(r == KErrNone)
				{
				++aSeq;
				}
			}
		}
		
	// Print the progress info.
	if (((r == KErrNone) && ((aSeq & 7) == 0)) || (r != KErrNone) || ackPacket)
		{
		// Calculate total packet count.
		TInt totalPacketCount = (ImgFileSize / PACKET_SIZE) + 2; // one extra packet for handshake and one extra packet for acknowledgement
		TInt roundedImageFileSize = ImgFileSize;
		TInt remainderSize = ImgFileSize % PACKET_SIZE;
		if(remainderSize != 0)
			{
			totalPacketCount ++;
			roundedImageFileSize += (PACKET_SIZE - remainderSize);
			}
		
		// Calculate the percentage complete.
		TInt percentComplete = (bytesUploaded * 100) / roundedImageFileSize;
		
		// Calculate the ETA.
		clock_t elapsedTime = clock() - startTime;
		clock_t etaM = -1L;
		clock_t etaS = -1L;
		if(aSeq > 0)
			{
			double bytesPerSec = double(bytesUploaded) / (double(elapsedTime) / double(CLOCKS_PER_SEC));
			clock_t etaTotalS = clock_t((double(roundedImageFileSize) - double(bytesUploaded)) / bytesPerSec);
			etaM = max(etaTotalS / 60, 0L);
			etaS = max(etaTotalS % 60, 0L);
			}

		// Ensure upload always halts at 100%.
		if(ackPacket)
			{
			aSeq = totalPacketCount;
			bytesUploaded = roundedImageFileSize;
			percentComplete = 100;
			etaM = 0L;
			etaS = 0L;
			}

		// Format uploaded bytes and ROM size.
		char sizeBuffer[256];
		StrFormatByteSize(bytesUploaded, sizeBuffer, sizeof(sizeBuffer));
		char imageSizeBuffer[256];
		StrFormatByteSize(roundedImageFileSize, imageSizeBuffer, sizeof(imageSizeBuffer));

		// Update the currently uploaded bytes, percentage complete and ETA in-place.
		// We add additional spacing to overwrite the previous line if the new line
		// contracts because the byte sizes have switched to larger units.
		if(ackPacket)
			{
			elapsedTime /= CLOCKS_PER_SEC;
			clock_t elapsedM = max(elapsedTime / 60, 0L);
			clock_t elapsedS = max(elapsedTime % 60, 0L);
			printf("\r%05d Packets Sent: %s / %s (%02d %%), Time Taken: %02d:%02d      ", aSeq, sizeBuffer, imageSizeBuffer, percentComplete, elapsedM, elapsedS);
			}
		else if((r == KErrNone) || (r == KErrEof))
			{
			if(etaM >= 0L)
				{
				printf("\r%05d Packets Sent: %s / %s (%02d %%), ETA: %02d:%02d      ", aSeq, sizeBuffer, imageSizeBuffer, percentComplete, etaM, etaS);
				}
			else
				{
				printf("\r%05d Packets Sent: %s / %s (%02d %%), ETA: ??:??      ", aSeq, sizeBuffer, imageSizeBuffer, percentComplete);
				}
			}
		else
			{
			fprintf(stderr, "\rFailed to send packet %d: %s / %s (%02d %%), Error: %d (%s)      ", ackPacket ? (totalPacketCount - 1) : aSeq, sizeBuffer, imageSizeBuffer, percentComplete, r, GetSymOrWinErrMsg(r));
			}
		}
		
	return r;
	}

TInt GetMonitorInfo();

TInt Handshake(TBool& aOutStream, TBool aIgnoreHandshakeFailure = false)
	{
	TInt r = 0;
	TInt r2 = 0;
	TUint8 b2[256];

	Sleep(2000); // wait 2 seconds
	RESET_COMM();
	Sleep(2000); // wait 2 seconds
flush:
	r = CommRead(b2, sizeof(b2), 60000);	// 60 second timeout
	if(r==sizeof(b2))
		{
		printf("Flushing data from port\n");
		TraceLog("Flushing data from port");
		goto flush;
		}

	if (r<0)
		{
		TraceLog("ERROR Handshake 1: r=%d (%s)", r, GetSymOrWinErrMsg(r));
		// Send CR to see if it elicits monitor prompt
		CommWriteC(0x0d);
		r2 = CommRead(LineBuf, 12, 1000);		// 1 second timeout
		if (r2==12 && memcmp(LineBuf, "\xd\xaPassword: ", 12)==0)
			{
			printf("ERROR: target is in debug monitor\n");
			TraceLog("ERROR: target is in debug monitor");
			if (LogFile)
				{
				printf("Gathering information...\n");
				TraceLog("Gathering information...");
				r2 = GetMonitorInfo();
				printf("Rebooting...\n");
				TraceLog("Rebooting...");
				CommWriteS("X\xd");
				}
			else
				{
				printf("No logfile - preserving state (not rebooting)\n");
				TraceLog("No logfile - preserving state (not rebooting)");
				}
			return KExitCodeDOA;
			}
		return r;
		}
	if (b2[r-1]!=BIGG && b2[r-1]!=BIGC)
		{
		if(!aIgnoreHandshakeFailure)
			{
			TraceLog("ERROR Handshake 2: r=%d (%s)", KErrTimedOut, GetSymOrWinErrMsg(KErrTimedOut));
			}
		return KErrTimedOut;
		}
		
	aOutStream = (b2[r-1]==BIGG);
	
	return KErrNone;
	}
	
TInt SendImageFile(TUint aPort, TUint aBaudRate, TBool aStream)
	{
	printf("Sending ROM image %s on COM%d at %dbps\n",ImgFileName,aPort,aBaudRate);
	TraceLog("Sending ROM image %s on COM%d at %dbps",ImgFileName,aPort,aBaudRate);

	TInt r = KErrNone;

	// Keep sending packets until we reach the end of the file or an error occurs.
	TInt seq = 0;
	clock_t startTime = clock();
	while(r==KErrNone)
		{
		r = SendPacket(seq, aStream, startTime);
		}
	
	// If we reached the end of the file, acknowledge that the transfer is complete.
	if(r==KErrEof)
		{
		RESET_COMM();
		CommWriteC(EOT);
		TInt b1 = CommRead1(500);
		if (b1==KErrTimedOut)
			{
			r = KErrNone;
			}
		else if (b1!=ACK)
			{
			r = KErrNone;
			}
		
		if(r == KErrNone)
			{
			printf("\n");
			}
		else
			{
			// Wait for acknowledgement from the target.
			TBool noAck = false;
			b1 = CommRead1(1000);
			if (b1==KErrTimedOut)
				{
				r = KErrNone;
				noAck = true;
				}
			else if (aStream && b1!=BIGG)
				{
				r = KErrNone;
				noAck = true;
				}
			else if (!aStream && b1!=BIGC)
				{
				r = KErrNone;
				noAck = true;
				}
			
			// If there was no acknowledgement from the target, ignore the problem at this point and print the newline.
			if(noAck)
				{
				printf("\n");
				}

			// Else, send our acknowledgement packet to complete the transfer.
			else
				{
				seq = -1;
				TInt bytesUploaded = 0;
				r = SendPacket(seq, aStream, bytesUploaded);
				printf("\n");
				if(r)
					{
					TraceLog("ERROR: SendImageFile r=%d (%s)", r, GetSymOrWinErrMsg(r));
					}
				}
			}
		}
		
	// Else, print any error.
	else
		{
		printf("\n<SendPacket %d -> %d (%s)\n", seq, r, GetSymOrWinErrMsg(r));
		TraceLog("ERROR: <SendPacket seq=%d r=%d (%s)", seq, r, GetSymOrWinErrMsg(r));
		}

	return r;
	}

void GetResponse()
	{
	TInt r;
	do	{
		r = CommRead(LineBuf, MAX_LINE, 1000);
		if (r>=0)
			if(use_stdout)
				{
				LineBuf[r]=0;
				printf(LineBuf);
				}
			else
				{
				WriteLog(LineBuf, r);
				}
		} while (r>=0);
	}

void IssueCommandAndGetResponse(const char* aCmd)
	{
	CommWriteS(aCmd);
	GetResponse();
	}

int token_length(const char* s)
	{
	const char* p = s;
	for (; *p && !isspace(*p); ++p) {}
	return p - s;
	}

const char* skip_space(const char* s)
	{
	for (; *s && isspace(*s); ++s) {}
	return s;
	}

TInt GetMonitorInfo()
/*
 return 0 on success, -1 on failure
 */
	{
	BOOL ok = FALSE;
	int l;
	int type;
	RESET_COMM();
	IssueCommandAndGetResponse("replacement\xd");
	IssueCommandAndGetResponse("f\xd");
	const char* p = LineBuf;
	const char* q = LineBuf + strlen(LineBuf);
	while (p<q)
		{
		p = skip_space(p);
		if (p+16>q || memcmp(p, "Fault Category: ", 16))
			{
			p += token_length(p);
			continue;
			}
		p = skip_space(p+16);
		l = token_length(p);
		if (l!=4 || memcmp(p, "KERN", 4))
			{
			p += l;
			continue;
			}
		p = skip_space(p+4);
		if (p+14>q || memcmp(p, "Fault Reason: ", 14))
			{
			p += token_length(p);
			continue;
			}
		p = skip_space(p+14);
		if (p+8<=q && memcmp(p, "00000050", 8)==0)
			{
			ok = true;
			break;
			}
		p+=8;
		continue;
		}
	if (ok)
		{
		// Add a line after the "." debug monitor prompt so the next log line won't appear next to it.
		TraceLog("");
		
		return 0;
		}

	IssueCommandAndGetResponse("i\xd");
	for (type=0; type<14; ++type)
		{
		char b[8];
		sprintf(b, "c%x\xd", type);
		IssueCommandAndGetResponse(b);
		}

	IssueCommandAndGetResponse("S\xd");
	
	// Optionally, dump the code segs to make it clear where non-XIP code is loaded
	// and make debugging easier.
	if(DebugMonitorOptions & kDebugDumpCodeSegs)
		{
		IssueCommandAndGetResponse("p all\xd");
		}
		
	// Add a line after the "." debug monitor prompt so the next log line won't appear next to it.
	TraceLog("");

	return -1;
	}
	
TInt ReceiveTestLog(TInt aOverallTimeOutS)
	{
	TInt r=0;
	TInt r2;
	clock_t iterationStartTime = clock();
	
	for(;;)
		{
		TBool checkDebugMonitor = false;
		
		r = CommRead(LineBuf, MAX_LINE, aOverallTimeOutS * 1000);
		
		// If a time-out occured, print an error message.
		if (r==KErrTimedOut)
			{
			clock_t elapsedTimeC = clock() - iterationStartTime;
			double elapsedTimeS = double(elapsedTimeC) / double(CLOCKS_PER_SEC);

			if(elapsedTimeS < 10.0)
				{
				if(!use_stdout)
					{
					fprintf(stderr, "Nothing received for past %.02f seconds\n", elapsedTimeS);
					}
				TraceLog("Nothing received for past %.02f seconds", elapsedTimeS);
				}
			else
				{
				elapsedTimeC /= CLOCKS_PER_SEC;
				clock_t elapsedM = max(elapsedTimeC / 60, 0L);
				clock_t elapsedS = max(elapsedTimeC % 60, 0L);

				if(!use_stdout)
					{
					fprintf(stderr, "Nothing received for past %02d:%02d minutes\n", elapsedM, elapsedS);
					}
				TraceLog("Nothing received for past %02d:%02d minutes", elapsedM, elapsedS);
				}

			checkDebugMonitor = true;
			}
			
		// If the string "Password: " was read, this probably indicates the kernel
		// has jumped into the debug monitor.
		if (r>=10 && memcmp(LineBuf+r-10, "Password: ", 10)==0)
			{
			checkDebugMonitor = true;
			}
			
		// If real data was received, print it to the selected output.
		if (r>0)
			{
			if (use_stdout)
				{
				LineBuf[r]=0;		// terminate string
				printf(LineBuf);
				}
			else
				{
				WriteLog(LineBuf, r);
				}

			iterationStartTime = clock();
			}
		
		// If an error occured, check to see if the debug monitor is running.  If it is,
		// we can extract information about the fault and use it to reboot the board.
		if (checkDebugMonitor || (r==KErrTimedOut))
			{
			// Send CR to see if it elicits monitor prompt.
			CommWriteC(0x0d);
			r2 = CommRead(LineBuf, 12, 1000);
			if (r2>0)
				{
				if(use_stdout)
					{
					LineBuf[r2]=0;
					printf(LineBuf);
					}
				else
					{
					WriteLog(LineBuf, r2);
					}
				}
			if (r2==12 && memcmp(LineBuf, "\xd\xaPassword: ", 12)==0)
				{
				break;
				}
			}
			
		// If an error occured and the debug monitor is not running, stop trying to receive the session log.
		if (r<0)
			{
			break;
			}
		}

	// Check to see if the debug monitor was successfully prompted.
	TT_ASSERT((r>0) || (r==KErrTimedOut));
	if (r>0)
		{
		// The debug monitor is accessible so extract debugging information.
		r = GetMonitorInfo();
		
		// Issue the command to try to reboot the board.
		CommWriteS("X\xd");
		if (r)
			{
			r = KExitCodeFaulted;
			}
		else
			{
			r = KErrNone;
			}
		}
	else if(r<0)
		{
		// The board debug monitor is not accessible or the board has locked up.
		printf("Target hung\n");
		TraceLog("Target hung");
		r = KExitCodeHung;
		}
		
	return r;	
	}

TInt DetermineBaudRate(TInt aPort, TUint& outBaudRate, TBool& aOutStream)
	{
	TInt r = KErrNone;
	
	for( TUint i = 0; i < sizeof(KBaudRateSearchList) / sizeof(TUint); i ++ )
		{
		// Set the COM port to use the next baud rate to try.
		if(i != 0)	// hide the first test so that the UI for boards supporting 115200bps-only remains the same as it
			{		// was before.
			printf("Testing COM%d at %dbps...", aPort, KBaudRateSearchList[i]);
			}
		TraceLog("Testing COM%d at %dbps", aPort, KBaudRateSearchList[i]);
		r = SetupCommPort(aPort, KBaudRateSearchList[i], true);

		if(r == KErrNone)
			{
			// Try to Y-modem handshake with the board.  If we succeed then we have found the correct speed.
			r = Handshake(aOutStream, true);
			if(r == KErrNone)
				{
				if(i != 0)
					{
					// If we finished the first test (115200bps) successfully, we do not reveal the fact that we are
					// prepared to test multiple baud rates so that the UI for boards supporting 115200bps only remains
					// the same as it was before.
					printf(" Succeeded\n", aPort, KBaudRateSearchList[i]);
					}
				TraceLog("Testing of COM%d at %dbps Succeeded", aPort, KBaudRateSearchList[i]);
				outBaudRate = KBaudRateSearchList[i];
				
				break;
				}
			else if(r == KExitCodeDOA)
				{
				// The board is stuck in the crash debugger and is unusable.
				break;
				}
			else
				{
				if(i == 0)
					{
					// If we finished the first test (115200bps) unsuccessfully, at this point we reveal that we are
					// testing multiple baud rates and report the results of the failure.
					printf("Testing COM%d at %dbps...", aPort, KBaudRateSearchList[i]);
					}
				printf(" Failed (%d) %s\n", r, GetSymOrWinErrMsg(r));
				TraceLog("Testing of COM%d at %dbps Failed (%d) %s", aPort, KBaudRateSearchList[i], r, GetSymOrWinErrMsg(r));
				}
			}
		else if(r == KExitCodeUnsupportedBaudRate)
			{
			if(i == 0)
				{
				// If we finished the first test (115200bps) and the baud rate setting is unsupported, at this point
				// we reveal that we are testing multiple baud rates and report the results of the failure.
				printf("Testing COM%d at %dbps...", aPort, KBaudRateSearchList[i]);
				}
			printf(" COM%d port doesn't support this baud rate setting\n", aPort);
			TraceLog("COM%d port doesn't support %dbps setting", aPort, KBaudRateSearchList[i]);
			}
		}
		
	return r;
	}

void UploadROM(TInt aPort, TUint& aInOutBaudRate)
	{
	// Open the ROM image file and copy it into memory.
	FILE* img = fopen(ImgFileName, "rb");
	if (!img)
		{
		fprintf(stderr, "Can't open %s for read (%d) %s\n", ImgFileName, errno, strerror(errno));
		exit(KExitCodeImage);
		}
	fseek(img,0,SEEK_END);
	ImgFileSize=ftell(img);
	fseek(img,0,SEEK_SET);
	TraceLog("ROM Image File Size   = %d bytes\n", ImgFileSize);
	ImgFileChunkBase = malloc(ImgFileSize);
	TT_ASSERT(ImgFileChunkBase != NULL);
	int n = fread(ImgFileChunkBase, 1, ImgFileSize, img);
	TT_ASSERT(n == ImgFileSize);
	fclose(img);

	// Check to see if we should flash the ROM image or the boot loader based
	// on the filename of the tool.
	if (strcmpi("flashimg", __argv[0])==0)
		{
		printf("Uploading image %s to flash ", ImgFileName);
		if (strstr(ImgFileName, ".zip") || strstr(ImgFileName, ".zip"))
			strcpy(ImgFileName, "flashimg.zip");
		else
			strcpy(ImgFileName, "flashimg.bin");
		printf("[%s]\n", ImgFileName);
		}
	else if (strcmpi("flashldr", __argv[0])==0)
		{
		printf("Uploading image %s to flash as bootloader ", ImgFileName);
		if (strstr(ImgFileName, ".zip") || strstr(ImgFileName, ".zip"))
			strcpy(ImgFileName, "flashldr.zip");
		else
			strcpy(ImgFileName, "flashldr.bin");
		printf("[%s]\n", ImgFileName);
		}

	// Open the COM port.
	OpenCommPort(aPort);
	
	// Check to see if we should try to automatically determine the baud rate.
	TInt r;
	TBool stream = true;
	if(aInOutBaudRate == 0)
		{
		r = DetermineBaudRate(aPort, aInOutBaudRate, stream);
		if(r != KErrNone)
			{
			if(r != KExitCodeDOA) // DetermineBaudRate prints its own error message for KExitCodeDOA
				{
				fprintf(stderr, "Unable to determine baud rate or the target is hung/unplugged\n");
				TraceLog("Unable to determine baud rate or the target is hung/unplugged");
				}
			exit(r);
			}
		}
	else
		{
		r = SetupCommPort(aPort, aInOutBaudRate);
		if(r == KErrNone)
			{
			r = Handshake(stream);
			}
		if(r != KErrNone)
			{
			fprintf(stderr, "Unable to connect to target on COM%d at %dbps (%d) %s\n", aPort, aInOutBaudRate, r, GetSymOrWinErrMsg(r));
			TraceLog("Unable to connect to target on COM%d at %dbps (%d) %s", aPort, aInOutBaudRate, r, GetSymOrWinErrMsg(r));
			exit(r);
			}
		}

	// Send the image.
	r = SendImageFile(aPort, aInOutBaudRate, stream);
	
	free(ImgFileChunkBase);
	}
	
TInt LogTargetOutput(const char* aLogFileName, TBool aUseStdOut, TInt aPort, TUint& aInOutBaudRate, TUint& aInOutLogBaudRate, TInt aOverallTimeOutS)
	{
	TInt r;
	
	// Switch to the session log baud rate (default 115200bps) - higher speeds are usually only used for serial
	// upload and not for the log download.  It is possible to utilise higher log baud rates by manually modifying
	// the debugport code in the board support package.
	if(aInOutBaudRate != aInOutLogBaudRate)
		{
		r = SetupCommPort(aPort, aInOutLogBaudRate);

		if(r != KErrNone)
			{
			fprintf(stderr, "Unable to switch to session log baud rate (%dbps) or the target is hung/unplugged\n", aInOutLogBaudRate);
			TraceLog("Unable to switch to session log baud rate (%dbps) or the target is hung/unplugged", aInOutLogBaudRate);
			exit(r);
			}
		
		aInOutBaudRate = aInOutLogBaudRate;
		}

	// Check to see if we are logging to STDOUT or to a file.
	if(!aUseStdOut)
		{
		printf("Logging target output from COM%d at %dbps to %s...\n", aPort, aInOutLogBaudRate, aLogFileName);
		TraceLog("BEGIN TARGET LOG", aLogFileName);
		}
	
	// Receive the session log.
	clock_t startTime = clock();
	r = ReceiveTestLog(aOverallTimeOutS);
	
	// Print the duration of the session.
	clock_t elapsedTimeC = clock() - startTime;
	double elapsedTimeS = double(elapsedTimeC) / double(CLOCKS_PER_SEC);
	if(elapsedTimeS < 10.0)
		{
		if(!use_stdout)
			{
			printf("Target output log ended after %.02f seconds\n", elapsedTimeS);
			}
		TraceLog("END TARGET LOG after %.02f seconds", elapsedTimeS);
		}
	else
		{
		elapsedTimeC /= CLOCKS_PER_SEC;
		clock_t elapsedM = max(elapsedTimeC / 60, 0L);
		clock_t elapsedS = max(elapsedTimeC % 60, 0L);

		if(!use_stdout)
			{
			printf("Target output log ended after %02d:%02d minutes\n", elapsedM, elapsedS);
			}
		TraceLog("END TARGET LOG after %02d:%02d minutes", elapsedM, elapsedS);
		}
	
	return r;
	}
	
void CloseSession()
	{
	// Perform cleanup...
	
	WriteLogS("\n\n");
	TraceLog("DONE");

	EscapeCommFunction(Comm, CLRRTS);
	EscapeCommFunction(Comm, CLRDTR);

	if(Comm != INVALID_HANDLE_VALUE)
		{
		CloseHandle(Comm);
		Comm = INVALID_HANDLE_VALUE;
		}

	if (LogFile)
		{
		fclose(LogFile);
		LogFile = NULL;
		}
	}

void PrintHelp()
	{
	fprintf(stderr, "\n");
	fprintf(stderr, "TrgTest Serial Y-Modem Upload Test Utility\n");
	fprintf(stderr, "==========================================\n\n");
	fprintf(stderr, "trgtest port# [-b<baudrate> default: 0 (auto)] [-lb<baudrate> default: 115200]");
	fprintf(stderr, "        imagefilename [logfilename] [timeout]\n\n");
	fprintf(stderr, "  port#              = COM port e.g., \"1\"\n");
	fprintf(stderr, "  -b (optional)      = ROM image serial upload baud rate e.g., \"-b115200\"\n");
	fprintf(stderr, "  -lb (opt.)         = session log download baud rate e.g., \"-lb115200\"\n");
	fprintf(stderr, "  imagefilename      = path to ROM image e.g., \"rom.img\" or \"nul\" for no image\n");
	fprintf(stderr, "  logfilename (opt.) = path to session log e.g., \"test.log\" or \"-\" for STDOUT\n");
	fprintf(stderr, "  timeout (opt.)     = timeout after inactivity in seconds e.g., \"1800\"\n\n");
	fprintf(stderr, "Example Usage:\n\n");
	fprintf(stderr, "  trgtest 1 rom.img log.txt\n");
	}


int main(int argc, char** argv)
	{
	TInt r = KErrNone;
	TInt port = 0;
	TUint baudRate = 0;
	TUint logBaudRate = 0;
	TInt overallTimeOutS = 1800;  // default time to wait for output (in seconds)
	const char* logfilename = NULL;
	int paramIndex = 1;
	
	// Disable stream buffering.
	setvbuf(stdout,NULL,_IONBF,0);
	
	// Print help if the parameter count is incorrect.
	if (argc<3 || argc>7)
		{
		PrintHelp();
		exit(KExitCodeUsage);
		}
		
	// Get the COM port.
	if(isdigit(argv[paramIndex][0]))
		{
		port = atoi(argv[paramIndex]);
		if(port == 0)
			{
			fprintf(stderr, "\"0\" is not valid - specify a serial COM port index number >= 1");
			exit(KExitCodeUsage);
			}
		paramIndex ++;
		}
	else
		{
		fprintf(stderr, "\"%s\" is not valid - specify a serial COM port index number >= 1", argv[paramIndex]);
		exit(KExitCodeUsage);
		}
	
	// Parse the optional parameters - order is not important.	
	while(paramIndex < argc)
		{
		// Get the serial upload baud rate.
		if(StrCmpNI(argv[paramIndex], "-b", 2) == 0)
			{
			baudRate = atoi(argv[paramIndex] + 2);
			paramIndex ++;
			}
			
		// Get the session log baud rate.
		else if(StrCmpNI(argv[paramIndex], "-lb", 3) == 0)
			{
			logBaudRate = atoi(argv[paramIndex] + 3);
			paramIndex ++;
			}
		
		// Stop if we parse an unknown option - it is probably the ROM image file path.	
		else
			{
			break;
			}
		}
		
	// If no session log baud rate was specified, set the default rate of 115200bps.
	if(logBaudRate == 0)
		{
		logBaudRate = KDefaultBaudRate;
		}
		
	// Get the ROM image file path.
	if(paramIndex >= argc)
		{
		fprintf(stderr, "No ROM image file specified\n");
		exit(KExitCodeUsage);
		}
	strcpy(ImgFileName, argv[paramIndex]);
	paramIndex ++;

	// Get the session log options.
	if (paramIndex < argc)
		{
		// Check to see if TrgTest should log the target output and to what.
		logfilename = argv[paramIndex];
		if (!strcmp(logfilename, "-"))
			{
			printf("After the upload has completed, TrgTest will read from the serial port and write to STDOUT.\n");
			use_stdout = true;
			}
		else
			{
			LogFile = fopen(logfilename, "wb");
			
			if (!LogFile)
				{
				fprintf(stderr, "Can't open %s for write (%d) %s\n", logfilename, errno, strerror(errno));
				exit(KExitCodeLog);
				}
			
			// Disable stream buffering.
			setvbuf(LogFile,NULL,_IONBF,0);
			}
		paramIndex ++;

		// Check to see if a non-default inactivity timeout has been specified.
		if (paramIndex < argc)
			{
			overallTimeOutS = atoi(argv[paramIndex]);
			}
		}

	TBool sendImage = strcmpi(ImgFileName, "nul") != 0;

	// Log the trgtest parameters.
	TraceLog("Port                  = COM%d", port);
	if(sendImage)
		{
		if(baudRate == 0)
			{
			TraceLog("Baud Rate             = Auto");
			}
		else
			{
			TraceLog("Baud Rate             = %dbps", baudRate);
			}
		TraceLog("ROM Image             = %s", ImgFileName);
		}
	else
		{
		TraceLog("ROM Image             = None");
		}
	TraceLog("Log Baud Rate         = %dbps", logBaudRate);
	if(use_stdout)
		{
		TraceLog("Log                   = STDOUT");
		}
	else if(logfilename)
		{
		TraceLog("Log                   = %s", logfilename);
		}
	else
		{
		TraceLog("Log                   = None", logfilename);
		}
	TraceLog("Inactivity Timeout    = %d secs.", overallTimeOutS);
	if(DebugMonitorOptions & kDebugDumpCodeSegs)
		{
		TraceLog("Debug Monitor Options = Dump Code Segs");
		}
	else
		{
		TraceLog("Debug Monitor Options = None");
		}

	// Initiate serial upload if a ROM image was specified.
	if(sendImage)
		{
		UploadROM(port, baudRate);
		}

	if(r==KErrNone)
		{
		// Check to see if the ROM upload (if any) succeeded.
		if(sendImage)
			{
			printf("Sent image file OK\n");
			TraceLog("Sent image file OK");
			}
		
		// Check to see if we should log the target output.
		if (LogFile || use_stdout)
			{
			// If no serial upload was performed the COM port is not yet open.
			if(!sendImage)
				{
				// Open the COM port.
				OpenCommPort(port);
				}

			// Log the target output if a log file was specified.
			r = LogTargetOutput(logfilename, use_stdout, port, baudRate, logBaudRate, overallTimeOutS);
			}
		}
	else
		{
		printf("SendImageFile upload failed -> (%d) %s\n", r, GetSymOrWinErrMsg(r));
		TraceLog("SendImageFile upload failed -> (%d) %s", r, GetSymOrWinErrMsg(r));
		r = KExitCodeDownload;
		}

	// Close the session
	CloseSession();

	// Print any exit error. 
	if (r!=KErrNone)
		{
		fprintf(stderr, "Exiting with error %d\n", r);
		}

	return r;
	}
