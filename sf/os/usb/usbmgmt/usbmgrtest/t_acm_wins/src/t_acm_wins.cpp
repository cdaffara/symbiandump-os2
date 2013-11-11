/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* This program executes all the tests in the ACM CSY Test Specification v 0.1
* on the host, and is to be used in conjunction with the device-side application (t_acm).
*
*/

// Includes ////////////////////////////////////////////////////////////////////

/* Symbian OS includes */
#include <e32base.h>
#include <e32cons.h>
#include <e32math.h>

#define WIN32_LEAN_AND_MEAN

/* Windows i/o for serial device */
#include <windows.h>
#include <wchar.h>

// Defines and enumerations /////////////////////////////////////////////////////

#define _printf console->Printf
#define _getch console->Getch
#define LEAVE(_x) VerboseLeaveL(_x, __LINE__, TPtrC8((const TUint8*)__FILE__), _L8(#_x))
#define LEAVEIFERROR(_x) VerboseLeaveIfErrorL(_x, __LINE__, TPtrC8((const TUint8*)__FILE__), _L8(#_x))
#define CHECK(_x) if (! (_x)) VerboseLeaveL(KErrGeneral, __LINE__, TPtrC8((const TUint8*)__FILE__), _L8(#_x))

#define MAX_BUFFER_SIZE 1024

// Flow control constants
enum	FlowControl_Modes {KFLOW_NONE, KFLOW_RTSCTS, KFLOW_DTRDSR};

// Global Variables /////////////////////////////////////////////////////////////

HANDLE	hSerial;	// An MS Windows handle for the serial port
DCB		OldDcb;		// for storing the original serial device context
DCB		newDcb;

// For holding our new serial port parameteres
struct SerialParameters
	{
	char			*comport;					// which port
	unsigned long	pktsize, pktcount, maxwait;	// packet config
	unsigned long	baud, bits, stop;			// serial parameters
	unsigned char	parity;
	enum	FlowControl_Modes	flowcontrol;	// flow control handshaking
} SerialParams;

LOCAL_D CConsoleBase* console;	// A Symbian command console

_LIT(KPortName, "COM3");		// Windows serial port name

RTimer timer;	// A timer for use by several of the tests

// Some buffers allocated on the heap for functions with large
// stack usage so we don't get a __chkstk error.
char writeBufbig[MAX_BUFFER_SIZE];
char readBufbig[MAX_BUFFER_SIZE];

// Functions //////////////////////////////////////////////////////////////////

void VerboseLeaveL(TInt aError, TInt aLineNum, const TDesC8& aFileName, const TDesC8& aCode)
/**
 * For bombing out usefully.
 */
	{
	TInt filenameOffset = aFileName.LocateReverse('\\') + 1;
	if (filenameOffset < 0) filenameOffset = 1;
	TPtrC8 shortFileName = aFileName.Mid(filenameOffset);
	TBuf<64> fName, code;
	fName.Copy(shortFileName.Left(64));
	code.Copy(aCode.Left(64));
	_printf(_L("ERROR (%d) on line %d of file %S\n"), aError, aLineNum, &fName);
	_printf(_L("Code: %S\n\n"), &code);
	_printf(_L("[ press any key ]"));
	_getch();
	User::Leave(aError);
	}

void VerboseLeaveIfErrorL(TInt aError, TInt aLineNum, const TDesC8& aFileName, const TDesC8& aCode)
/**
 * For bombing out usefully if there's an error.
 */
	{
	if (aError) 
		VerboseLeaveL(aError, aLineNum, aFileName, aCode);
	}

void SetWindowsSerialParams()
/**
 * Set up the new serial parameters with which to run the tests.
 */
{
	// parameter 1 is port name
	// read in COM port name (this parameter must be specified!)
	SerialParams.comport = new char[10];
	strcpy(SerialParams.comport, "COM3");

	// default packet size
	SerialParams.pktsize = 1024;

	// default no of packets
	SerialParams.pktcount = 1;

	// default wait for data return
	SerialParams.maxwait = 30;

	// default flow control
	SerialParams.flowcontrol = KFLOW_RTSCTS;

	// default data rate
	// baud rate 0 (port's default) standard rates 300 -> 115200
	SerialParams.baud = 0;

	// default bits per char
	SerialParams.bits = 8;

	// default parity
	SerialParams.parity = 'N';

	// default 9 is stop bits
	SerialParams.stop = 1;
}

static int	InitializePort()
/**
 * Store the old state of the serial port and set it up for our tests.
 */
{
	DCB				dcb;
	COMMTIMEOUTS	CommsTimeouts;

	// Setup comms device receive & transmit buffers
	if(SetupComm(hSerial, 16384, 16384) == 0)
	{
		LEAVE(-1);
	}

	// Purge all characters from the output and input buffers
	// and terminate any pending read or write operations 
	if(PurgeComm(hSerial, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR) == 0)
	{
		LEAVE(-1);
	}

	dcb.DCBlength = sizeof(DCB);
	if(GetCommState(hSerial, &dcb) == 0)
	{
		// not a serial device
		LEAVE(-1);
	}

    memcpy(&OldDcb, &dcb, sizeof(DCB));

	// General initialization for both transmitted and received bytes
	// using port's default baud rate etc..., so read it
	SerialParams.baud = dcb.BaudRate;		// Get baud rate 
	SerialParams.bits = dcb.ByteSize;		// Get number of bits 
	if(dcb.StopBits == 2)					// Get number of stop bits to be used
		SerialParams.stop = 2;
	else
		SerialParams.stop = 1;

	// Get parity scheme
	if((dcb.fParity == FALSE) || (dcb.Parity == 0))
	{
		SerialParams.parity = 'N';
	}
	else
	{
		switch(dcb.Parity)
		{
		case 1:	SerialParams.parity = 'O'; break;
		case 2:	SerialParams.parity = 'E'; break;
		case 3:	SerialParams.parity = 'M'; break;
		case 4:	SerialParams.parity = 'S'; break;
		default:	SerialParams.parity = 'N'; break;  // shouldn't happen
		}
	}
	dcb.fBinary = TRUE;				// Set binary mode 

	// Setup hardware flow control
	if(SerialParams.flowcontrol == KFLOW_RTSCTS)
	{
		dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;	// Specify the type of RTS flow control
		dcb.fOutxCtsFlow = 1;						// Specify whether the CTS signal is monitored

		dcb.fDtrControl = DTR_CONTROL_ENABLE;	// Specify the type of DTR flow control
		dcb.fOutxDsrFlow = 0;					// Specify whether the DSR signal is monitored
	}
	else if(SerialParams.flowcontrol == KFLOW_DTRDSR)
	{
		dcb.fRtsControl = RTS_CONTROL_ENABLE;	// Specify the type of RTS flow control
		dcb.fOutxCtsFlow = 0;						// Specify whether the CTS signal is monitored

		dcb.fDtrControl = DTR_CONTROL_HANDSHAKE;// Specify the type of DTR flow control
		dcb.fOutxDsrFlow = 1;					// Specify whether the DSR signal is monitored
	}
	else // KFLOW_NONE
	{
		dcb.fRtsControl = RTS_CONTROL_ENABLE;	// Specify the type of RTS flow control
		dcb.fOutxCtsFlow = 0;					// Specify whether the CTS signal is monitored

		dcb.fDtrControl = DTR_CONTROL_ENABLE;	// Specify the type of DTR flow control
		dcb.fOutxDsrFlow = 0;					// Specify whether the DSR signal is monitored
	}

	dcb.fInX = dcb.fOutX = 0;
	dcb.fDsrSensitivity = 0;
	dcb.fErrorChar = 0;
	dcb.fNull = 0;
	dcb.fAbortOnError = 1;

	// Configure the communications device according 
	// to the specifications in the device-control block
	// The function reinitializes all hardware and control settings, 
	// but does not empty output or input queues
	if(SetCommState(hSerial, &dcb) == 0)
	{
		LEAVE(-1);
	}

	// Set timeout parameters for comms device
	/* A value of MAXDWORD, combined with zero values for both the 
	 * ReadTotalTimeoutConstant and ReadTotalTimeoutMultiplier members, 
	 * specifies that the read operation is to return immediately with 
	 * the characters that have already been received, even if no characters 
	 * have been received.
	 */

	CommsTimeouts.ReadIntervalTimeout = 0xFFFFFFFF;
	CommsTimeouts.ReadTotalTimeoutMultiplier = 0;
	CommsTimeouts.ReadTotalTimeoutConstant = 1000;

	// CBR_9600 is approximately 1byte/ms. For our purposes be generous 
	// & allow double the expected time per character
	if(SerialParams.maxwait > 1)
	{
		CommsTimeouts.WriteTotalTimeoutMultiplier = 0;
		CommsTimeouts.WriteTotalTimeoutConstant = (SerialParams.maxwait - 1) * 1000;
	}
	else
	{
		CommsTimeouts.WriteTotalTimeoutMultiplier = 2*CBR_9600/dcb.BaudRate;
		CommsTimeouts.WriteTotalTimeoutConstant = 1000;
	}

	if(0 == SetCommTimeouts(hSerial, &CommsTimeouts))
	{
		LEAVE(-1);
	}

	if(0 == SetCommMask(hSerial, EV_RXCHAR | EV_ERR /* | EV_TXEMPTY */))
	{
		LEAVE(-1);
	}

	return 0;
}

void OpenComPort()
/**
 * Create a Windows com port for our serial device.
 */
{
	hSerial = INVALID_HANDLE_VALUE;
	wchar_t  *fullname = new wchar_t[50];
    
	// Open comms device
	wcscpy(fullname, L"\\\\.\\com3");
	hSerial = CreateFile(fullname,
						 GENERIC_READ | GENERIC_WRITE,
						 0,                    // exclusive access
						 NULL,                 // no security attrs
						 OPEN_EXISTING,
						 FILE_ATTRIBUTE_NORMAL,
						 NULL ) ;

	// return if handle for comms device is invalid
	if (hSerial == INVALID_HANDLE_VALUE)
	{
		delete[] fullname;
		LEAVE(GetLastError());
	}
	else 
	{
		if(InitializePort())
		{
			// Close comms device and event handles for overlapped read and write
			CloseHandle(hSerial);
			hSerial = INVALID_HANDLE_VALUE;
		}
	}
	delete[] fullname;
}

void FillBuffer(char *buffer, int size)
/**
 * Fills the buffer with incrementing data
 */
	{
	for (TInt i=0; i<size; i++)
		{
		// Need the & 0xff to remove the truncation warning
		buffer[i] = (char)((i%256) & 0xff);
		}
	}

bool CheckBuffer(char *buffer, int size)
/**
 * Checks the buffer is filled with incrementing data
 */
	{
	for (TInt i=0; i<size; i++)
		{
		if (buffer[i] != i%256)
		return FALSE;
		}

	return TRUE;
	}

void ReadData(char *RxBuffer, int size)
/**
 * Read the given amount of data into a buffer from the serial port.
 */
{
	unsigned long Count;
	long total = 0;

	while (total < size) {
	  if(ReadFile(hSerial, RxBuffer, size, &Count, NULL) == 0)
	  {
		_printf(_L("ReadFile() returned error code: %d.\n"), GetLastError());
	  }
	  else
	  {
		total += Count;
	  }
	}

}

void WriteData(char *TxBuffer, int size)
/**
 * Write the given data over the serial port.
 */
{
	unsigned long TxCount;

	if(WriteFile(hSerial, TxBuffer, size, &TxCount, NULL) == 0)
	{
		_printf(_L("WriteFile() returned error code: %d.\n"), GetLastError());
	}
}


////////////////////////////////////////////////////////////////////////////////

void DataStress_SizeVary_TestL()
/**
 * Perform a single test.
 */
	{
	_printf(_L("\n----------------------------------\n"));
	_printf(_L("This test performs varying size read\n"));
	_printf(_L("and writes to and from the device.\n"));
	_printf(_L("------------------------------------\n\n"));

	// Purge all characters from the output and input buffers
	// and terminate any pending read or write operations 
	if(PurgeComm(hSerial, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR) == 0)
		{
		CloseHandle(hSerial);
		LEAVE(-1);
		}

	_printf(_L("Writing data\n"));
	for (int i = 1; i<MAX_BUFFER_SIZE; i*=2)
		{
		FillBuffer(writeBufbig, i);
		WriteData(writeBufbig, i);
		}

	_printf(_L("Writing more data\n"));
	for (int i = 1; i<MAX_BUFFER_SIZE; i*=2)
		{	
		FillBuffer(writeBufbig, i);
		WriteData(writeBufbig, i);
		}

	_printf(_L("Reading data\n"));
	for (int i = 1; i<MAX_BUFFER_SIZE; i*=2)
		{
		memset(readBufbig, 0, MAX_BUFFER_SIZE);
		ReadData(readBufbig, i);
		_printf(_L("%d "), i);
		}

	_printf(_L("\nTest complete.\n"));
	}


void DataStress_RateVary_TestL()
/**
 * Perform a single test.
 */
{
	TInt i;

	_printf(_L("\n----------------------------------\n"));
	_printf(_L("This test performs varying speed read\n"));
	_printf(_L("and writes to and from the host.\n"));
	_printf(_L("Press any key to quit.\n"));
	_printf(_L("------------------------------------\n\n"));

	_printf(_L("Setting handshaking & receive buffer length\n"));

	_printf(_L("Writing data\n"));
	for (i = 1; i<100; i++)
		{
		FillBuffer(writeBufbig, MAX_BUFFER_SIZE);
		WriteData(writeBufbig, MAX_BUFFER_SIZE);
		}

	_printf(_L("Writing more data\n"));
	for (i = 1; i<20; i++)
		{
		FillBuffer(writeBufbig, MAX_BUFFER_SIZE);
		WriteData(writeBufbig, MAX_BUFFER_SIZE);
		}

	_printf(_L("Reading data\n"));
	for (i = 1; i<100; i++)
		{
		memset(readBufbig, 0, MAX_BUFFER_SIZE);
		ReadData(readBufbig, MAX_BUFFER_SIZE);
		CheckBuffer(readBufbig, MAX_BUFFER_SIZE);

		}

	_printf(_L("\nTest complete\n"));
}

void TimeOut_TestL()
/**
 * Perform a single test.
 */
{
	_printf(_L("\n---------------------------\n"));
	_printf(_L("This test exercises the read\n"));
	_printf(_L("and write timeouts on the device.\n"));
	_printf(_L("Press any key to quit.\n"));
	_printf(_L("-----------------------------\n\n"));


	_printf(_L("In this test the host sits around while the device times out.\n"));
	_printf(_L("Press a key when finished.\n"));
	_getch();

	_printf(_L("\nTest complete\n"));
}

void CancelTx_TestL()
/**
 * Perform a single test.
 */
{
	_printf(_L("\n----------------------------------------\n"));
	_printf(_L("This tests the read/write cancel feature\n"));
	_printf(_L("Press any key to quit.\n"));
	_printf(_L("------------------------------------------\n\n"));

	_printf(_L("In this test the host sits around while the device cancels.\n"));
	_printf(_L("Press a key when finished.\n"));
	_getch();

	_printf(_L("\nTest complete\n"));
}

void InterruptTx_TestL()
/**
 * Perform a single test.
 */
{
	_printf(_L("\n--------------------------------\n"));
	_printf(_L("This tests the read/write cancel\n"));
	_printf(_L("when the USB cable is pulled\n"));
	_printf(_L("----------------------------------\n\n"));

	// Close the handle. If we don't do this before the device operator starts 
	// pulling the cable out and plugging it in again then we'll never be able 
	// to open it again afterwards. (This is also seen in HyperTerminal- 
	// solution: reboot HyperTerminal.)
	CloseHandle(hSerial);

	// The device will attempt to read, then the cable will be pulled.
	_printf(_L("Press any key when the device reports\n"));
	_printf(_L("that the test is complete.\n"));
	_getch();

	// restart the serial device
	OpenComPort();

	_printf(_L("\nTest complete\n"));
}

void Shutdown_TestL()
/**
 * Perform a single test.
 */
{
	_printf(_L("\n-----------------------------------\n"));
	_printf(_L("This tests the USB Manager shutdown\n"));
	_printf(_L("during reads and writes.\n"));
	_printf(_L("Press any key to quit.\n"));
	_printf(_L("-------------------------------------\n\n"));

	_printf(_L("In this test the host sits around while\n"));
	_printf(_L(" the device shuts down the USB manager.\n"));
	_printf(_L("Press a key when finished.\n"));
	_getch();

	_printf(_L("\nTest complete\n"));
}

void BufferOverrun_TestL()
/**
 * Test updated from that in the ACM unit test specification to
 * read/write messages bigger than the receive and transmit buffers.
 * Changed as previous test was no longer valid.
 */
{
	#define MAX_BIG_BUFFER_SIZE	(1024*8)
	char bigBuf[MAX_BIG_BUFFER_SIZE];

	_printf(_L("\n--------------------------------\n"));
	_printf(_L("This tests read/writes which are\n"));
	_printf(_L("bigger than the buffer length.\n"));
	_printf(_L("----------------------------------\n\n"));

	_printf(_L("Writing data.\nBuffer length: %d"), MAX_BIG_BUFFER_SIZE);
	FillBuffer(bigBuf, MAX_BIG_BUFFER_SIZE);
	WriteData(bigBuf, MAX_BIG_BUFFER_SIZE);

	_printf(_L("done.\nReading data\nBuffer length: %d"), MAX_BIG_BUFFER_SIZE);
	ReadData(bigBuf, MAX_BIG_BUFFER_SIZE);

	_printf(_L(" done.\nTest complete\n"));
}

void Break_TestL()
/**
 * Perform a single test.
 */
{
	_printf(_L("\n--------------------------------\n"));
	_printf(_L("This tests break and break cancel.\n"));
	_printf(_L("----------------------------------\n\n"));

	_printf(_L("In this test the host sits around while\n"));
	_printf(_L("the device issues some breaks.\n"));
	_printf(_L("Press a key when finished.\n"));
	_getch();

	// TODO: should also do host-requested breaks. And NotifyBreaks.
	_printf(_L("\nTest complete\n"));
}

void SignalChange_TestL()
/**
 * Perform a single test.
 */
{
	_printf(_L("\n---------------------------------------\n"));
	_printf(_L("This tests signal change notifications.\n"));
	_printf(_L("---------------------------------------\n\n"));


	_printf(_L("Press any key to send a signal change.\n"));
	_getch();

	newDcb.DCBlength = sizeof(DCB);
    memcpy(&OldDcb, &newDcb, sizeof(DCB));

	newDcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
	
	(void) SetCommState(hSerial, &newDcb);

	_printf(_L("\nTest complete\n"));
}

void FlowControl_TestL()
/**
 * Perform a single test.
 */
{
	_printf(_L("\n-------------------------------------------\n"));
	_printf(_L("This tests flow control change notifications.\n"));
	_printf(_L("---------------------------------------------\n\n"));

	_printf(_L("Host side does nothing in this test. Press any key.\n"));
	_getch();

	_printf(_L("\nTest complete\n"));
}

void ConfigChange_TestL()
/**
 * Perform a single test.
 */
{
	DCB		dcb;

	_printf(_L("\n------------------------------- -----\n"));
	_printf(_L("This tests config change notifications.\n"));
	_printf(_L("---------------------------------------\n\n"));

	
	_printf(_L("Press any key to send a new configuration.\n"));
	_getch();

	dcb.DCBlength = sizeof(DCB);
    memcpy(&OldDcb, &dcb, sizeof(DCB));
	dcb.BaudRate=38400;

	(void) SetCommState(hSerial, &dcb);
	
	_printf(_L("\nTest complete\n"));
}


void SecondClient_TestL()
/**
 * Perform a single test.
 */
{
	_printf(_L("\n---------------------------------------\n"));
	_printf(_L("This tests that we can have a second\n"));
	_printf(_L("client with non-exclusive access.\n"));
	_printf(_L("Press any key to quit.\n"));
	_printf(_L("---------------------------------------\n\n"));

	_printf(_L("Writing data\n"));
	FillBuffer(writeBufbig, MAX_BUFFER_SIZE);
	WriteData(writeBufbig, MAX_BUFFER_SIZE);
	WriteData(writeBufbig, MAX_BUFFER_SIZE);

	_printf(_L("Reading data\n"));
	memset(readBufbig, 0, 256);
	ReadData(readBufbig, 256);
	CheckBuffer(readBufbig, 256);

	_printf(_L("\nTest complete\n"));
}


// ******************************************************************
// The following are placholders for the ACMRq tests.
// These tests need no support on the platform end, as yet.
// The functions are left here incase someday some support is needed.
// ******************************************************************

void ACMRq_EncapCommand_TestL()
{
	 // Not needed to support this here
}

void ACMRq_Break_TestL()
{
	 // TODO: can we check for break status here ?
}

void ACMRq_SetFeature_TestL()
{
	 // TODO: Test doesn't make sense.
}

void ACMRq_ClearFeature_TestL()
{
	 // TODO: Test doesn't make sense.
}

void ACMRq_SetCoding_TestL()
{
	 // TODO: Can we check the line codeing here ?
}

void ACMRq_CtrlState_TestL()
{
	 // TODO: Test doesn't make sense.
}

void ACMRq_EncapResp_TestL()
{
	 // Not needed to support this here.
}

void ACMRq_CommsFeature_TestL()
{
	 // Not needed to support this here.
}

void ACMRq_GetCoding_TestL()
{
	 // Not needed to support this here.
}


void ACMNtf_SendState_TestL()
{
	 // TODO: SendSerialState() ???
}

void ACMNtf_Status_TestL()
{
	 // TODO: SendNetworkConnection() ???
}

void ACMNtf_RespAvail_TestL()
{
	 // Test not supported.
}

void Loopback_TestL()
/**
 * Perform a single test.
 */
	{
	_printf(_L("\n---------------------------------------------\n"));
	_printf(_L("This test writes data out of the USB ACM port\n"));
	_printf(_L("and check that it is correctly echoed back.\n"));
	_printf(_L("---------------------------------------------\n\n"));

	// Purge all characters from the output and input buffers
	// and terminate any pending read or write operations 
	if(PurgeComm(hSerial, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR) == 0)
		{
		LEAVE(GetLastError());
		}

	for (char i=0; i<100; i++)
		{
		memset(readBufbig, 0, MAX_BUFFER_SIZE);	// clear the read buffer
		memset(writeBufbig, i, MAX_BUFFER_SIZE);	// fill the write buffer with known data
		_printf(_L("%d: "), (TInt)i);
		WriteData(writeBufbig, 64);
		ReadData(readBufbig, 64);
		for (int j=0; j<64; j++)
			{
			if (readBufbig[j] != i)
				{
				_printf(_L("Buffer contents error.\n"));
				for (int k=0; k<64; k++)
					{
					_printf(_L("%d "), readBufbig[k]);
					}
				_printf(_L("\n"));
				return;
				}
			}
		}
	}

void mainL()
/**
 * This function controls test execution as directed by the user.
 */
	{
	TRequestStatus status;

	memset(&SerialParams, 0, sizeof(struct SerialParameters));
	SetWindowsSerialParams();

	OpenComPort();

	_printf(_L("Opened com port. [press any key]")); _getch();

	TBool noExit = ETrue;
	while (noExit)
		{
			_printf(_L("\nAvailable tests:\n\n"));
			_printf(_L("1. Data stress, size varies (test 2.1.1)\n"));
			_printf(_L("2. Data stress, rate varies (test 2.1.2)\n"));
			_printf(_L("3. Timeout (test 2.2)\n"));
			_printf(_L("4. Cancel Transfer (test 2.3)\n"));
			_printf(_L("5. Interrupt Transfer (test 2.4)\n"));
			_printf(_L("6. Shutdown (test 2.5)\n"));
			_printf(_L("7. Buffer overrun (test 2.6)\n"));
			_printf(_L("8. Break (test 2.7)\n"));
			_printf(_L("9. Event notification, signal change (test 2.8.1)\n"));
			_printf(_L("a. Event notification, flow control (test 2.8.2)\n"));
			_printf(_L("b. Event notification, config change (test 2.8.3)\n"));
			_printf(_L("c. Second client (test 2.9)\n"));
			_printf(_L("p. Loopback test (test 2.12)\n"));
			_printf(_L("\n"));
			_printf(_L("\nSelection (x to exit): "));

			char ch = (char) _getch();
			_printf(_L("\n"));
			switch (ch)
				{
			case '1': DataStress_SizeVary_TestL();	break;
			case '2': DataStress_RateVary_TestL();	break;
			case '3': TimeOut_TestL();	break;
			case '4': CancelTx_TestL();	break;
			case '5': InterruptTx_TestL();	break;
			case '6': Shutdown_TestL();	break;
			case '7': BufferOverrun_TestL();	break;
			case '8': Break_TestL();	break;
			case '9': SignalChange_TestL();	break;
			case 'A':
			case 'a': FlowControl_TestL();	break;
			case 'B':
			case 'b': ConfigChange_TestL();	break;
			case 'C':
			case 'c': SecondClient_TestL();	break;
			case 'D':
			case 'd': ACMRq_EncapCommand_TestL();	break;
			case 'E':
			case 'e': ACMRq_Break_TestL();	break;
			case 'F':
			case 'f': ACMRq_SetFeature_TestL();	break;
			case 'G':
			case 'g': ACMRq_ClearFeature_TestL();	break;
			case 'H':
			case 'h': ACMRq_SetCoding_TestL();	break;
			case 'I':
			case 'i': ACMRq_CtrlState_TestL();	break;
			case 'J':
			case 'j': ACMRq_EncapResp_TestL();	break;
			case 'K':
			case 'k': ACMRq_CommsFeature_TestL();	break;
			case 'L':
			case 'l': ACMRq_GetCoding_TestL();	break;
			case 'M':
			case 'm': ACMNtf_SendState_TestL();	break;
			case 'N':
			case 'n': ACMNtf_Status_TestL();	break;
			case 'O':
			case 'o': ACMNtf_RespAvail_TestL();	break;
			case 'P':
			case 'p': Loopback_TestL(); break;
			case 'x':
			case 'X':
				noExit = EFalse;
				break;
			default:
				_printf(_L("\nInvalid key\n"));
				break;
				}

			if (noExit)
				{
				_printf(_L("Test Complete. Press a key.\n"));
				_getch();
				}
		}

		(void) SetCommState(hSerial, &OldDcb);
		CloseHandle(hSerial);
		delete[] SerialParams.comport;
	}

void consoleMainL()
/**
 * Create a console and run mainL().
 */
	{
	console=Console::NewL(_L("T_ACM"),TSize(KConsFullScreen,KConsFullScreen));
	CleanupStack::PushL(console);
	mainL();
	CleanupStack::PopAndDestroy();
	}

GLDEF_C TInt E32Main()
/**
 * Standard Symbian entry point. Sets stuff up and deals with the cleanup stack.
 */
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanupStack=CTrapCleanup::New();

	// create the timer for use during some of the tests
	timer.CreateLocal();

	TRAP_IGNORE(consoleMainL());
	delete cleanupStack;
	timer.Close();
	__UHEAP_MARKEND;
	return 0;
	}


