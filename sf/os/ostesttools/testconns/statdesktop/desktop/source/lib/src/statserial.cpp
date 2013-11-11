/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/




////////////////////////////////////////////////////////////////////////////////////////
// STATSerial.cpp
// Implementation of the serial comms transport
////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "statserial.h"
#include "assert.h"
#include "../../../../Common/inc/SerialPacketSize.h"

#define SP_COMMANDSIZE	4
#define SP_LENGTHSIZE	4

////////////////////////////////////////////////////////////////////////////////////////
// Constructor
CSTATSerial::CSTATSerial(STATCONNECTTYPE eConnect)
: hComPort((HANDLE)0), 	pBuffer(NULL), iBufferLength(0), iMaxPacketSize(0)
{
	switch(eConnect)
	{
		case SymbianSerial:
		case SymbianInfrared:
			iMaxPacketSize = KMaxPacketSize;
			break;
		case SymbianBluetooth:
			iMaxPacketSize = KMaxBluetoothPacketSize;
			break;
		default:
			assert(0);
			break;
	};

	assert(0 != iMaxPacketSize);
}


////////////////////////////////////////////////////////////////////////////////////////
// Destructor
CSTATSerial::~CSTATSerial()
{
	Disconnect();
	Release();
}


////////////////////////////////////////////////////////////////////////////////////////
// Initialisation stuff
int CSTATSerial::Initialise(void)
{
	return ITS_OK;
}


////////////////////////////////////////////////////////////////////////////////////////
// Open a connection
int CSTATSerial::Connect(const char *pAddress)
{
	// already connected
	if (hComPort)
		return ITS_OK;

	if (OpenComPort(pAddress))
	{
		SetError("Connected to port %s Speed %d Bytes %d Parity %d Stop %d",
			pAddress, STAT_BAUDRATE, STAT_BYTESIZE, STAT_PARITY, STAT_STOPBITS);
		return ITS_OK;
	}

	return E_CONNECTIONFAILED;
}


////////////////////////////////////////////////////////////////////////////////////////
// Send a command and associated data
////////////////////////////////////////////////////////////////////////////////////////
int CSTATSerial::Send(const char cIdentifier, const char *pCommand, const unsigned long ulLength)
{
	DWORD dwBytes = 0;
	int ret = E_WRITEFAILED;

	// prepare command
	STATCOMMAND STATSendCommand;
	memset(&STATSendCommand, 0, sizeof(STATCOMMAND));
	STATSendCommand.cIdentifier = cIdentifier;
	STATSendCommand.ulLength = ulLength;

	int result = 0;
	if (result = WriteFile(hComPort, (LPVOID)&STATSendCommand, sizeof(STATCOMMAND), &dwBytes, NULL))
	{
		if (dwBytes == sizeof(STATCOMMAND))
		{
			// only send data if it will fit into our buffer
			if (pCommand && STATSendCommand.ulLength && STATSendCommand.ulLength <= iMaxPacketSize)
			{
				if (WriteFile(hComPort, (LPVOID)pCommand, STATSendCommand.ulLength, &dwBytes, NULL))
				{
					if (dwBytes == STATSendCommand.ulLength)
					{
						SetError("Sent successfully %ld bytes", STATSendCommand.ulLength);
						ret = ITS_OK;
					}
					else
					{
						SetError("Incorrect number of bytes written - expected %ld sent %ld", STATSendCommand.ulLength, dwBytes);
						ret = E_BADNUMBERBYTES;
					}
				}
				else
					SetError("Write data to port failed");
			}
			else
			{
				SetError("Sent successfully");
				ret = ITS_OK;
			}
		}
		else
		{
			int err = ::GetLastError();
			SetError("Incorrect number of bytes written - expected %ld sent %ld", sizeof(STATCOMMAND), dwBytes);
			ret = E_BADNUMBERBYTES;
		}
	}
	else
		SetError("Write command to port failed");

	return ret;
}


////////////////////////////////////////////////////////////////////////////////////////
// Receive a command and associated data
//
// Tries to read a STATCOMMAND structure from the port.  If successful, the length
// of any data following will be specified in the structure.  If extra data, try to
// read this.  If all successful, set the pointers to point to it
//
////////////////////////////////////////////////////////////////////////////////////////
int CSTATSerial::ReceiveBytes( char *buff, unsigned long size )
{
	int ret;
	int err;
	unsigned long bytes_read = 0;
	unsigned long bytes_read_this_iteration = 0;

	// make sure size > 0 otherwise the following semantics fail
	assert( size > 0 );

	// loop until either all bytes have been received or an error occurs
	for( bytes_read = 0; bytes_read < size; bytes_read += bytes_read_this_iteration ) { 
		ret = ReadFile( hComPort, &(buff[bytes_read]), (size - bytes_read), &bytes_read_this_iteration, NULL );
		if( bytes_read_this_iteration == 0 ) {
			return NO_DATA_AT_PORT;
		}
		if( ret == 0 ) {
			err = ::GetLastError();
			SetError( "Error while receiving command - %ld", err );
			return err;
		}
	}

	// everything is OK
	return 0;
}

int CSTATSerial::Receive( char *cIdentifier, char **ppData, unsigned long *pLength )
{
	int ret;
	unsigned long command;
	unsigned long data_length;

	// initialise parameters
	*cIdentifier = 0;
	*ppData = NULL;
	*pLength = 0;

	// get command
	ret = ReceiveBytes( (char*)&command, SP_COMMANDSIZE );
	if( ret != 0 ) {
		return ret;
	}

	static const int MAX_COMMAND = 127;
	if( command > MAX_COMMAND ){
		return INVALID_COMMAND_FORMAT;
	}

	// get length
	ret = ReceiveBytes( (char*)&data_length, SP_LENGTHSIZE );
	if( ret != 0 ) {
		return ret;
	}

	// put these values into the return slots
	*cIdentifier = (char)command;
	*pLength = data_length;

	// if the length is zero then there is no more to do
	if( data_length == 0 ) {
		return ITS_OK;
	}

	// if the length is greater than the buffer size then this is a header packet and we are not
	// supposed to actually read any data from it.
	// if( data_length > SP_BUFFERSIZE ) {
	if( data_length > iMaxPacketSize ) {
		SetError( "Received command ID: %c successfully length %d", (*cIdentifier), (*pLength) );
		return ITS_OK;
	}

	// Otherwise we are about to read some data. We first have to setup the memory management
	// which is done in a pretty ugly way IMAO.
	if( data_length > iBufferLength )
	{
		if( pBuffer ) {
			delete [] pBuffer;
			pBuffer = NULL;
			iBufferLength = 0;
		}
		pBuffer = new char [data_length];
		if( pBuffer == NULL ) {
			SetError( "Unable to allocate %ld bytes of memory to hold data", data_length );
			return E_OUTOFMEM;
		}
		iBufferLength = data_length;
	}

	// now read the data
	ret = ReceiveBytes( pBuffer, data_length );
	if( ret != 0 ) {
		return ret;
	}
	*ppData = pBuffer;

	return ITS_OK;
}


////////////////////////////////////////////////////////////////////////////////////////
// Disconnect from the port
int CSTATSerial::Disconnect(void)
{
	// release previous resources
	if (pBuffer)
	{
		delete [] pBuffer;
		pBuffer = NULL;
		iBufferLength = 0;
	}

	// disconnect
	if (hComPort)
	{
		CloseHandle(hComPort);
		hComPort = (HANDLE)0;
	}

	SetError("Disconnected successfully");
	return ITS_OK;
}


////////////////////////////////////////////////////////////////////////////////////////
// Release resources
int CSTATSerial::Release(void)
{
	Disconnect();
	return ITS_OK;
}


////////////////////////////////////////////////////////////////////////////////////////
//	PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////
// 
bool CSTATSerial::OpenComPort(const char *pAddress)
{
	TCHAR szAddress[256] = {0};

#ifdef UNICODE
	szAddress[0] = (TCHAR)0;

    // Convert to UNICODE.
    MultiByteToWideChar(CP_ACP,					// conversion type
						 0,							// flags
						 pAddress,					// source
						 -1,						// length
						 szAddress,					// dest
						 256);						// length
#else
	strcpy(szAddress, pAddress);
#endif

	// Use device naming for COM port.
	CString port;
	CString address(pAddress);	// Local variable to cope with 
								// wide UNICODE characters.
	port.Format(_T("\\\\.\\%s"), address.operator LPCTSTR());

	if (INVALID_HANDLE_VALUE == (hComPort = CreateFile(port,
					 GENERIC_READ | GENERIC_WRITE,
					 0,
					 NULL,
					 OPEN_EXISTING,
					 FILE_FLAG_WRITE_THROUGH,
					 NULL)))
	{
		SetError("Port [%s] could not be opened", pAddress);
		int err = ::GetLastError();
		hComPort = (HANDLE)0;
		return false;
	}

	COMMTIMEOUTS CommTimeOuts;
	CommTimeOuts.WriteTotalTimeoutMultiplier	= STAT_WRITETOTALTIMEOUTMULTIPLIER; 
	CommTimeOuts.WriteTotalTimeoutConstant		= STAT_WRITETOTALTIMEOUTCONSTANT;
	CommTimeOuts.ReadIntervalTimeout			= STAT_READINTERVALTIMEOUT;
	CommTimeOuts.ReadTotalTimeoutMultiplier		= STAT_READTOTALTIMEOUTMULTIPLIER;
	CommTimeOuts.ReadTotalTimeoutConstant		= STAT_READTOTALTIMEOUTCONSTANT;

	if (!SetCommTimeouts(hComPort, &CommTimeOuts))
	{
		SetError("Comm port timeouts could not be set");
		CloseHandle(hComPort);
		hComPort = (HANDLE)0;
		return false;
	} 

	// Configure the COM port 
	DCB dcb;
	GetCommState(hComPort, &dcb);
	dcb.DCBlength			= sizeof(dcb); 
	dcb.BaudRate			= STAT_BAUDRATE;
	dcb.fBinary				= TRUE;
	dcb.fParity				= STAT_PARITY;
	dcb.fOutxCtsFlow		= TRUE;
	dcb.fOutxDsrFlow		= FALSE;
	dcb.fDtrControl			= DTR_CONTROL_ENABLE;	
	dcb.fDsrSensitivity		= FALSE;
	dcb.fTXContinueOnXoff	= TRUE;
	dcb.fOutX				= FALSE;
	dcb.fInX				= FALSE;
	dcb.fErrorChar			= FALSE;
	dcb.fNull				= FALSE;
	dcb.fRtsControl			= RTS_CONTROL_HANDSHAKE;
	dcb.fAbortOnError		= TRUE;
	dcb.XonLim				= 4096;
	dcb.XoffLim				= 1024;
	dcb.ByteSize			= STAT_BYTESIZE;
	dcb.Parity				= STAT_PARITY;
	dcb.StopBits			= STAT_STOPBITS;
	dcb.XonChar				= 17;
	dcb.XoffChar			= 19;

//	dcb.fRtsControl		= RTS_CONTROL_ENABLE;
//	dcb.BaudRate		= STAT_BAUDRATE;
//	dcb.fOutxCtsFlow	= FALSE;
//	dcb.XonLim			= STAT_XONLIMIT;
//	dcb.XoffLim			= STAT_XOFFLIMIT;
	
	if (!SetCommState(hComPort, &dcb))
	{
		SetError("Comm port state could not be set");
		CloseHandle(hComPort);
		hComPort = (HANDLE)0;
		return false;
	} 

	return true;
}
