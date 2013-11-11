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



/********************************************************************************
 *
 * Stat TCP/IP Transport
 *
 *******************************************************************************/
#ifndef __STATTCPIP_H__
#define __STATTCPIP_H__

/********************************************************************************
 *
 * System Includes
 *
 *******************************************************************************/
#include <es_sock.h>

/********************************************************************************
 *
 * Local Includes
 *
 *******************************************************************************/
#include "stat_interfaces.h"
#include "ntoh.h"

/********************************************************************************
 *
 * Definitions
 *
 *******************************************************************************/
const TUint KStatHeaderSize	= 8;
const TUint KLittleStatPort = 3000;
const TUint KLittleStatListenQueue = 1;

/********************************************************************************
 *
 * CStatTransportTCPIP - Top-level of the transport. Creates and waits on the 
 * listening socket. When a new data socket is created it creates the reader
 * and writer passing the socket to them. After that, the object simply routes
 * read/write calls/cancels to the appopriate objects.
 *
 *******************************************************************************/
class CStatTransportTCPIP : public CActive, public MStatNetwork
{
public:
	// construction
	static CStatTransportTCPIP *NewL( void );
	CStatTransportTCPIP();
	~CStatTransportTCPIP();

	// from MStatNetwork
	TInt GetPacketSize();
	TInt InitialiseL( MNotifyStatTransport *iTransport );
	TInt ConnectL( TDesC *aRemoteHost );
	TInt RequestSend( TDesC8 *aCommandData, const TUint aDataLength );
	TInt RequestReceive( TUint aByteCount );
	TInt Disconnect( void );
	TInt Release( void );
	TText8 *Error( void );
	TUint NtoHl( TUint aValue ) const;
	TUint HtoNl( TUint aValue ) const;
	TBool PadPacket( TUint size ) const;

	// from CActive 
	void RunL( void );
	void DoCancel( void );

private:
	void ConstructL( void );

private:
	enum TStatTransportStatus { EIdle, EError, EInitialised, EConnecting, EConnected, 
			EDisconnectingData, EDisconnectingListen, EDisconnected };

private:
	TStatTransportStatus iTransportStatus;
	TReadWriteStatus iRWStatus;
	MNotifyStatTransport *iTransport;
	TInt iMaxPacketSize;

	HBufC8 *iBuffer;
	TPtr8 *iBufferPtr;

 	RSocket iListenSocket;
 	RSocket iDataSocket;
 	RSocketServ iSocketServ;
	
};

inline TUint CStatTransportTCPIP::NtoHl( TUint aValue ) const
{
	return ( ntohl( aValue ) );
}

inline TUint CStatTransportTCPIP::HtoNl( TUint aValue ) const
{
	return ( htonl( aValue ) );
}

inline TBool CStatTransportTCPIP::PadPacket( TUint ) const
{
	return ( EFalse );
}

#endif // __STATTCPIP_H__
