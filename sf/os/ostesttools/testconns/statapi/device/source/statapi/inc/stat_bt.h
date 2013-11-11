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
 * Stat Bletooth Transport
 *
 *******************************************************************************/
#ifndef __STATBT_H__
#define __STATBT_H__

/********************************************************************************
 *
 * System Includes
 *
 *******************************************************************************/
#include <es_sock.h>

#include <btmanclient.h>
#include <btsdp.h>
#include <bt_sock.h>
#include <btextnotifiers.h>

/********************************************************************************
 *
 * Local Includes
 *
 *******************************************************************************/
#include "stat_interfaces.h"

/********************************************************************************
 *
 * Definitions
 *
 *******************************************************************************/
const TUint KStatBTHeaderSize	= 8;
const TUint KLittleStatBTListenQueue = 1;
const TUint KLittleStatBTPort = 3;

/********************************************************************************
 *
 * CStatTransportBT - Top-level of the transport. Creates and waits on the 
 * listening socket. When a new data socket is created it creates the reader
 * and writer passing the socket to them. After that, the object simply routes
 * read/write calls/cancels to the appopriate objects.
 *
 *******************************************************************************/
class CStatTransportBT : public CActive, public MStatNetwork
{
public:
	// construction
	static CStatTransportBT *NewL( void );
	CStatTransportBT();
	~CStatTransportBT();

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
	void DoCancel();

private:
	void ConstructL( void );
	
	TInt RegWithSDPDatabaseL( void );
	TInt StartSocketL( void );
	void HandleAsyncDisconnect( void );

public:
	enum TStatTransportBTStatus { EIdle, EError, EInitialised, EConnectingSockets, EConnected, 
								EConnectingRegisterMgr, EConnectingRegisterSDP, 
								EDisconnectingData, EDisconnectingListen, 
								EDisconnected, EDisconnectingUnregister };

private:
	TStatTransportBTStatus iBTTransportStatus;
	TStatTransportBTStatus iBTTransportDisconnectStatusBeforeUnregister;
	TReadWriteStatus iRWStatus;
	MNotifyStatTransport *iTransport;
	TInt iMaxPacketSize;

	HBufC8 *iRecvBuffer;
	TPtr8 *iRecvBufferPtr;

 	RSocket iListenSocket;
	RSocket iDataSocket;
 	RSocketServ iSocketServ;
 	HBufC8 *iWrCommandData;
 	TUint iPort;							//	Assigned port number

 	TBTServiceSecurity iServiceSecurity;	//Service security object
 
 
 	RSdp iSdpSession;					//Service Discovery Session
 	RSdpDatabase iSdpDatabaseSession;	//Subsession to the SDP through which service record and their attributes can be added, deleted, and updated.
 	TSdpServRecordHandle iRecHandle;		//Session record handle
};

inline TUint CStatTransportBT::NtoHl( TUint aValue ) const
{
	return ( aValue );
}

inline TUint CStatTransportBT::HtoNl( TUint aValue ) const
{
	return ( aValue );
}

inline TBool CStatTransportBT::PadPacket( TUint ) const
{
	return ( EFalse );
}

#endif // __STATBT_H__
