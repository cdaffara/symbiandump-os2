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
// statserial.h
// Serial transport class definition
////////////////////////////////////////////////////////////////////////////////////////

#ifndef STATSERIAL_H
#define STATSERIAL_H

#include "statcommon.h"
#include "stattransport.h"

// COM port settings
// There is a problem with the use of ::ReadFile as this is a blocking
// operation called in a critical section.  If the call blocks for too
// long and the user closes the thread then the section is lost for ever.
// Reduce the timeouts to prevent the threads locking when there is
// a link error.  This is the wrong thing to do as we should really
// update the reading to check the port first and query if data
// is available.
#define STAT_WRITETOTALTIMEOUTMULTIPLIER	0
#define STAT_WRITETOTALTIMEOUTCONSTANT		1000
#define STAT_READINTERVALTIMEOUT			0
#define STAT_READTOTALTIMEOUTMULTIPLIER		0
#if defined _DEBUG
	// In a release build wait a few seconds.
	// In a debug build wait for longer as we may be debugging and 
	// pausing at break-points and generally slowing things up a bit.
	#define STAT_READTOTALTIMEOUTCONSTANT		(10 * 1000)
#else // defined _DEBUG
	#define STAT_READTOTALTIMEOUTCONSTANT		1000
#endif // defined _DEBUG

#define STAT_BAUDRATE						CBR_115200
#define STAT_BYTESIZE						8
#define STAT_PARITY							NOPARITY
#define STAT_STOPBITS						ONESTOPBIT


class CSTATSerial : public CSTATTransport
{
public:
	CSTATSerial(STATCONNECTTYPE eConnect);	// Request that when this object
											// is created then it knows what
											// implementation of serial it is.
											// We could use sub-classes but there
											// is so little difference we handle
											// all serial transports as one case.
											// The only difference is in the packet
											// size which we store as an object
											// data field.
	~CSTATSerial();
	int Initialise(void);
	int Connect(const char *pAddress = NULL);
	int Send(const char cIdentifier, const char *pData = NULL, const unsigned long ulLength = 0);
	int Receive(char *cIdentifier, char **ppData = NULL, unsigned long *pLength = NULL);
	int Disconnect(void);
	int Release(void);
	unsigned int GetMaxPacketSize(void) const;	// Override base class method
												// to specify that we have
												// a limit to the pakcet size
												// and the specific limit
												// depends upon the serial
												// transport we represent.

private:
	bool OpenComPort(const char *pAddress);
	int ReceiveBytes( char *buff, unsigned long size );

	HANDLE hComPort;				// com port to work with
	unsigned long int iMaxPacketSize;
									// Holds the maximum size of the
									// packets we send.
	char *pBuffer;					// Holds received data
	unsigned long int iBufferLength;
									// Holds the length of the receive data 
									// buffer
};

inline unsigned int CSTATSerial::GetMaxPacketSize(void) const
{
	return (iMaxPacketSize);
}

#endif
