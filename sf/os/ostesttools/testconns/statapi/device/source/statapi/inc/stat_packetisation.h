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
 * Stat Packetisation Transport
 *
 *******************************************************************************/
#ifndef __STATPACKETISATION_H__
#define __STATPACKETISATION_H__

/********************************************************************************
 *
 * System Includes
 *
 *******************************************************************************/

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
#define KIDSize				4
#define KLengthSize			4
#define KHeaderSize			(KIDSize + KLengthSize)

/********************************************************************************
 *
 * Classes
 *
 *******************************************************************************/
class CStatTransportPacketisation : public CBase, public MStatApiTransport, public MNotifyStatTransport
{
public:
	// construction
	static CStatTransportPacketisation *NewL( MNotifyStatEngine *aStatEngine, MStatNetwork *aStatNetwork, TUint aPacketSize );
	CStatTransportPacketisation();
	virtual ~CStatTransportPacketisation();

	// from MStatApiTransport
	TInt InitialiseL( void );
	TInt ConnectL( TDesC *aRemoteHost );
	TInt RequestSend( const TUint aCommand, MDataSupplier *const aDataSupplier);
	TInt RequestReceive( void );
	TInt Disconnect( void );
	TInt Release( void );
	TText8 *Error( void );

	// from MNotifyStatTransport
	void HandleInitialise( TInt aResult );
	void HandleConnect( TInt aResult );
	void HandleSend( TInt aResult );
	void HandleReceive( TInt aResult, TDesC8 *aData, TUint aDataLength );
	void HandleDisconnect( TInt aResult );
	void HandleRelease( TInt aResult );
	void HandleError( TInt aError, void *aErrorData );
	void HandleInfo( const TDesC *aInfo );

private:
	// private helper functions
	void ConstructL( MNotifyStatEngine *aStatEngine, MStatNetwork *aStatNetwork, TUint aPacketSize );
	void SendHeaderPacket( TUint aCommand, TUint aDataLength );
	void SendAckPacket( TUint aCommand );
	void BuildHeader( TUint aCommand, TUint aDataLength );
	void NotifyEngineSend( TUint aResult );
	void NotifyEngineReceive( TUint aResult );

private:
	// status enums
	enum TSendStatus { ESendIdle, ESendingInitialHeader, ESendingFragmentHeader, ESendingFragmentData, ESendingFragmentAck };
	enum TReceiveStatus { EReceiveIdle, EReceivingInitialHeader, EReceivingFragmentHeader, EReceivingData, EReceivingFragmentAck };

private:	
	// general members
	MNotifyStatEngine *iStatEngine;
	MStatNetwork *iStatNetwork;
	TCommStatus iTransportStatus;
	TSendStatus iSendStatus;
	TReceiveStatus iRecvStatus;
	TUint iDataPacketSize;

	// send buffers
	TBuf8<KHeaderSize> iSendHeader;
	HBufC8 *iSendFragment;
	TUint iSendCommand;
	TUint iSendDataLength;
	TUint iSendDataWritten;
	TUint iSendDataAcknowledged;

	// receive buffers
	TUint iRecvCommand;
	TUint iRecvDataLength;
	TUint iRecvDataReceived;
	TUint iRecvDataAcknowledged;

	MDataConsumer *iDataConsumer;	// The data consumer is 
									// use to pass data from the
									// transport layer to the 
									// command decoder.
									// Each specific object is
									// valid for the context of a 
									// single command (and 
									// destroyed).

	MDataSupplier *iDataSupplier;	// This is owned by other
									// objects and we are passed
									// a pointer for our own use
									// but we do not maintain this.
};

#endif // __STATTCPIP_H__
