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




#include "stdafx.h"
#include "statserialbluetooth.h"
#include "assert.h"

#define SP_COMMANDSIZE	4
#define SP_LENGTHSIZE	4
#define SP_BUFFERSIZE	STAT_BUFFERSIZE

////////////////////////////////////////////////////////////////////////////////////////
// Constructor
CSTATSerialBluetooth::CSTATSerialBluetooth()
: hComPort((HANDLE)0), 	pTemp(NULL)
{
}


////////////////////////////////////////////////////////////////////////////////////////
// Destructor
CSTATSerialBluetooth::~CSTATSerialBluetooth()
{
	Disconnect();
	Release();
}


////////////////////////////////////////////////////////////////////////////////////////
// Initialisation stuff
int CSTATSerialBluetooth::Initialise(void)
{
	return ITS_OK;
}


////////////////////////////////////////////////////////////////////////////////////////
// Open a connection
int CSTATSerialBluetooth::Connect(const char *pAddress)
{
	// already connected
	if (hComPort)
		return ITS_OK;

	if (OpenComPort(pAddress))
	{
		SetError("Connected to port %s Speed %d Bytes %d Parity %d Stop %d",
			pAddress, STATBT_BAUDRATE, STATBT_BYTESIZE, STATBT_PARITY, STATBT_STOPBITS);
		return ITS_OK;
	}

	return E_CONNECTIONFAILED;
}


////////////////////////////////////////////////////////////////////////////////////////
// Send a command and associated data
////////////////////////////////////////////////////////////////////////////////////////
int CSTATSerialBluetooth::Send(const char cIdentifier, const char *pCommand, const unsigned long ulLength)
{
	DWORD dwBytes = 0;
	int ret = E_WRITEFAILED;

	// prepare command
	STATCOMMAND STATSendCommand;
	memset(&STATSendCommand, 0, sizeof(STATCOMMAND));
	STATSendCommand.cIdentifier = cIdentifier;
	STATSendCommand.ulLength = ulLength;

	if (WriteFile(hComPort, (LPVOID)&STATSendCommand, sizeof(STATCOMMAND), &dwBytes, NULL))
	{
		if (dwBytes == sizeof(STATCOMMAND))
		{
			// only send data if it will fit into our buffer
		//	if (pCommand && STATSendCommand.ulLength && STATSendCommand.ulLength <= STAT_BUFFERSIZE)
		//	{
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
		///	}
		//	else
		//	{
		//		SetError("Sent successfully");
		//		ret = ITS_OK;
		//	}
		}
		else
		{
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
int CSTATSerialBluetooth::ReceiveBytes( char *buff, unsigned long size )
{
	int ret;
	int err;
	unsigned long bytes_read = 0;
	unsigned long bytes_read_this_iteration = 0;

	// make sure size > 0 otherwise the following semantics fail
	assert( size > 0 );

	// loop until either all bytes have been received or an error occurs
	for( bytes_read = 0; bytes_read < size; bytes_read += bytes_read_this_iteration ) 
	{ 
		ret = ReadFile( hComPort, &(buff[bytes_read]), (size - bytes_read), &bytes_read_this_iteration, NULL );		
		
		if( bytes_read_this_iteration == 0 ) 
		{
			return NO_DATA_AT_PORT;
		}

		if( ret == 0 ) 
		{
			err = GetLastError();
			SetError( "Error while receiving command - %ld", err );
			return err;
		}
	}

	// everything is OK
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////
int CSTATSerialBluetooth::Receive( char *cIdentifier, char **ppData, unsigned long *pLength )
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
/*	if( data_length > SP_BUFFERSIZE ) {
		SetError( "Received command ID: %c successfully length %d", (*cIdentifier), (*pLength) );
		return ITS_OK;
	}
*/

	SetError( "Received command ID: %c successfully length %d", (*cIdentifier), (*pLength) );
	
	// Otherwise we are about to read some data. We first have to setup the memory management
	// which is done in a pretty ugly way IMAO.
	if( pTemp ) 
	{
		delete [] pTemp;
		pTemp = NULL;
	}
	
	pTemp = new char [data_length];
	
	if( pTemp == NULL ) 
	{
		SetError( "Unable to allocate %ld bytes of memory to hold data", data_length );
		return E_OUTOFMEM;
	}

	// now read the data
	ret = ReceiveBytes( pTemp, data_length );
	
	if( ret != 0 ) 
	{
		return ret;
	}

	*ppData = pTemp;

	return ITS_OK;
}


////////////////////////////////////////////////////////////////////////////////////////
// Disconnect from the port
int CSTATSerialBluetooth::Disconnect(void)
{
	// release previous resources
	if (pTemp)
	{
		delete [] pTemp;
		pTemp = NULL;
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
int CSTATSerialBluetooth::Release(void)
{
	Disconnect();
	return ITS_OK;
}


////////////////////////////////////////////////////////////////////////////////////////
//	PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////
// 
bool CSTATSerialBluetooth::OpenComPort(const char *pAddress)
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

	if (INVALID_HANDLE_VALUE == (hComPort = CreateFile(szAddress,
					 GENERIC_READ | GENERIC_WRITE,
					 0,
					 NULL,
					 OPEN_EXISTING,
					 FILE_FLAG_WRITE_THROUGH,
					 NULL)))
	{
		SetError("Port [%s] could not be opened", pAddress);
		return false;
	}

	COMMTIMEOUTS CommTimeOuts;
	CommTimeOuts.WriteTotalTimeoutMultiplier	= STATBT_WRITETOTALTIMEOUTMULTIPLIER; 
	CommTimeOuts.WriteTotalTimeoutConstant		= STATBT_WRITETOTALTIMEOUTCONSTANT;
	CommTimeOuts.ReadIntervalTimeout			= STATBT_READINTERVALTIMEOUT;          
    CommTimeOuts.ReadTotalTimeoutMultiplier		= STATBT_READTOTALTIMEOUTMULTIPLIER;   
    CommTimeOuts.ReadTotalTimeoutConstant		= STATBT_READTOTALTIMEOUTCONSTANT;
	
	if (!SetCommTimeouts(hComPort, &CommTimeOuts))
	{
		SetError("Comm port timeouts could not be set");
		CloseHandle(hComPort);
		return false;
	} 

	// Configure the COM port 
	DCB dcb;
	GetCommState(hComPort, &dcb);
	dcb.DCBlength			= sizeof(dcb); 
	dcb.BaudRate			= STATBT_BAUDRATE;
	dcb.fBinary				= TRUE;
	dcb.fParity				= STATBT_PARITY;
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
	dcb.ByteSize			= STATBT_BYTESIZE;
	dcb.Parity				= STATBT_PARITY;
	dcb.StopBits			= STATBT_STOPBITS;
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
		return false;
	} 

	return true;
}
