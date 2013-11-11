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



 /**************************************************************************************
 *
 * Packetisation transport for STAT -- for packet based network layers
 *
 *************************************************************************************/
#include <e32std.h>
#include <e32base.h>

/**************************************************************************************
 *
 * Local Includes
 *
 *************************************************************************************/
#include "assert.h"
#include "stat.h"
#include "stat_packetisation.h"
#include "dataconsumer_memory.h"
#include "dataconsumer_file.h"

/*************************************************************************
 *
 * Definitions
 *
 *************************************************************************/
#define KMinDataSize		64
#define KMinPacketSize		KHeaderSize + KMinDataSize

 /**************************************************************************************
 *
 * CStatTransportPacketisation - Construction
 *
 *************************************************************************************/
CStatTransportPacketisation *CStatTransportPacketisation::NewL( MNotifyStatEngine *aStatEngine, MStatNetwork *aStatNetwork, TUint aPacketSize )
{
    CStatTransportPacketisation *self = new (ELeave) CStatTransportPacketisation();
    CleanupStack::PushL(self);
	self->ConstructL( aStatEngine, aStatNetwork, aPacketSize );
	CleanupStack::Pop();
    return self;
}

CStatTransportPacketisation::CStatTransportPacketisation() :
	iDataSupplier( NULL )
{
}

void CStatTransportPacketisation::ConstructL( MNotifyStatEngine *aStatEngine, MStatNetwork *aStatNetwork, TUint aPacketSize )
{
	// check params
	asserte( aStatEngine != NULL );
	asserte( aStatNetwork != NULL );
	asserte( aPacketSize >= KMinPacketSize );

	// setup general members
	iStatEngine = aStatEngine;
	iStatNetwork = aStatNetwork;
	iTransportStatus = EIdle;
	iSendStatus = ESendIdle;
	iRecvStatus = EReceiveIdle;
	iDataPacketSize = aPacketSize;

	// setup send buffer members
	iSendFragment = NULL;
	iSendCommand = 0;
	iSendDataLength = 0;
	iSendDataWritten = 0;
	iSendDataAcknowledged = 0;

	// setup recv buffer members
	iRecvCommand = 0;
	iRecvDataLength = 0;
	iRecvDataReceived = 0;
}

CStatTransportPacketisation::~CStatTransportPacketisation()
{
	if( iSendFragment != NULL )
		{
		delete iSendFragment;
		iSendFragment = NULL;
		}
	if( iDataConsumer )
		{
		iDataConsumer->Delete( );
		iDataConsumer = NULL;
		}
}

/**************************************************************************************
 *
 * CStatTransportPacketisation - MStatApiTransport - all calls except for send are 
 * passed straight through to the network layer (a flag is recorded for receive as
 * well). See the STAT design spec doc for the packetisation protocol.
 *
 *************************************************************************************/
TInt CStatTransportPacketisation::InitialiseL( void )
{
	return iStatNetwork->InitialiseL( this );
}

TInt CStatTransportPacketisation::ConnectL( TDesC *aRemoteHost )
{
	return iStatNetwork->ConnectL( aRemoteHost );
}

TInt CStatTransportPacketisation::RequestReceive( void )
{
	// set the state and wait for the header
	iTransportStatus = EReceivePending;
	iRecvStatus = EReceivingInitialHeader;
	return iStatNetwork->RequestReceive( KHeaderSize );
}

TInt CStatTransportPacketisation::Disconnect( void )
{
	return iStatNetwork->Disconnect();
}

TInt CStatTransportPacketisation::Release( void )
{
	return iStatNetwork->Release();
}

TText8 *CStatTransportPacketisation::Error( void )
{
	return iStatNetwork->Error();
}


TInt CStatTransportPacketisation::RequestSend( const TUint aCommand, MDataSupplier *const aDataSupplier)
	{
	TInt err = KErrNone;

	// make sure a packet can fit the header and at least some data -- otherwise it's useless
	asserte( iDataPacketSize >= KMinPacketSize );

	// check the state is as expected
	asserte( iTransportStatus == EIdle );
	asserte( iSendStatus == ESendIdle );

	// Take a copy of the pointer to the data supplier.
	iDataSupplier = aDataSupplier;

	// save the command and its data
	iSendCommand = aCommand;
	iSendDataLength = 0;
	if(iDataSupplier)
		{
		TInt dataLength = 0;
		err = iDataSupplier->GetTotalSize( dataLength );

		if( err == KErrNone )
			{
				iSendDataLength = static_cast<TUint>(dataLength);
			}
		}
	iSendDataWritten = 0;

	if( err == KErrNone )
	{
		// create and send the initial header
		SendHeaderPacket( iSendCommand, iSendDataLength );
		err = KSTErrAsynchronous;
	}

	return (err);
}



/**************************************************************************************
 *
 * CStatTransportPacketisation - MNotifyStatTransport - all events except HandleSend
 * and HandleReceive are sent straight through to the engine.  
 *
 *************************************************************************************/
void CStatTransportPacketisation::HandleInitialise( TInt aResult )
{
	iStatEngine->HandleInitialise( aResult );
}

void CStatTransportPacketisation::HandleConnect( TInt aResult )
{
	iStatEngine->HandleConnect( aResult );
}

void CStatTransportPacketisation::HandleDisconnect( TInt aResult )
{
	iStatEngine->HandleDisconnect( aResult );
}

void CStatTransportPacketisation::HandleRelease( TInt aResult )
{
	iStatEngine->HandleRelease( aResult );
}

void CStatTransportPacketisation::HandleError( TInt aError, void *aErrorData )
{
	iStatEngine->HandleError( aError, aErrorData );
}

void CStatTransportPacketisation::HandleInfo( const TDesC *aInfo )
{
	iStatEngine->HandleInfo( aInfo );
}

/**************************************************************************************
 *
 * CStatTransportPacketisation - HandleSend
 *
 *************************************************************************************/
void CStatTransportPacketisation::HandleSend( TInt aResult )
{
	TUint remainingBytes, packetSize, err;
	
	// handle each state
	switch( iSendStatus ) {

	case ESendingInitialHeader:

		// wait for an acknowledgement for the header
		iTransportStatus = EReceivePending;
		iRecvStatus = EReceivingFragmentAck;
		iSendStatus = ESendIdle;
		err = iStatNetwork->RequestReceive( KHeaderSize );
		asserte( err == KSTErrAsynchronous );                   
		break;

	case ESendingFragmentHeader:

		// if there is data left to send then send it -- otherwise we have completed a send
		if( iSendDataWritten == iSendDataLength ) {
			NotifyEngineSend( aResult );
		} else {
			remainingBytes = iSendDataLength - iSendDataWritten;
			packetSize = ((remainingBytes < iDataPacketSize) ? remainingBytes : iDataPacketSize);

			asserte( packetSize <= iDataPacketSize );

			iSendFragment = HBufC8::NewMax(packetSize);
			if(iSendFragment)
			{
				TInt dataCopied = 0;
				iDataSupplier->GetData( *iSendFragment, packetSize, dataCopied );

				// When we do the sending the data we send
				// is of size 'bufferSize' but we must only
				// add 'packetSize' to the running total of
				// data written.
				iTransportStatus = ESendPending;
				iSendStatus = ESendingFragmentData;
				iSendDataWritten += packetSize;
				err = iStatNetwork->RequestSend( iSendFragment, packetSize );
				asserte( err == KSTErrAsynchronous );
			}
		}
		break;

	case ESendingFragmentData:

		// if we have just sent some data then free the buffer
		asserte( iSendFragment != NULL );
		delete iSendFragment;
		iSendFragment = NULL;

		// if we are not packetising data then we have completed the send otherwise we are 
		// packetising data then the remote host will send an acknowledgement
		if( iSendDataLength <= iDataPacketSize ) {
			NotifyEngineSend( aResult );
		} else {
			iTransportStatus = EReceivePending;
			iRecvStatus = EReceivingFragmentAck;
			iSendStatus = ESendIdle;
			err = iStatNetwork->RequestReceive( KHeaderSize );
			asserte( err == KSTErrAsynchronous );
		}
		break;
		
	case ESendingFragmentAck:

		// if we have just sent a fragment ack then wait for the next fragment header
		iTransportStatus = EReceivePending;
		iRecvStatus = EReceivingFragmentHeader;
		iSendStatus = ESendIdle;
		err = iStatNetwork->RequestReceive( KHeaderSize );
		asserte( err == KSTErrAsynchronous );
		break;

	case ESendIdle:
		;
		break;
	}
}

/**************************************************************************************
 *
 * CStatTransportPacketisation - HandleReceive
 *
 *************************************************************************************/
void CStatTransportPacketisation::HandleReceive( TInt aResult, TDesC8 *aData, TUint aDataLength )
{
	TUint *pData, packetSize, remainingBytes, recvCommand, recvLength = 0;
	TInt err;

	
	
	// make sure that we thought we were waiting to receive
	asserte( aResult == KErrNone );
	asserte( iTransportStatus == EReceivePending );
	asserte( iRecvStatus != EReceiveIdle );
	
	
	// Handle a special case where we receive a modem query
	// from the (Windows) system.  There is a chance that we
	// will recieve a series of modem query strings of three
	// lots of 'AT'.  When that happens we need to ignore it.
	// Usually we use the 'L' macro and let the preprocessor
	// sort out the length of the character but we have to
	// force it here because in a UNICODE build the serial
	// port still delivers 8 bit characters.
	static const TInt modemQueryLength = 8;
	static const TLitC8<modemQueryLength + 1> modemQuery = { modemQueryLength, "AT\rAT\rAT" };

	if( (static_cast<TInt>(aDataLength) == modemQueryLength) && 
			(0 == aData->Compare(modemQuery.operator const TDesC8&()) ))
	{
		return;
	}
	// End special case of modem query.
	
	// STEP 1: process the incoming message
	switch( iRecvStatus ) {

	case EReceivingInitialHeader:

		// extract the header info and setup the local receive state 
		pData = (TUint*)aData->Ptr();
		iRecvCommand = iStatNetwork->NtoHl( pData[0] );
		iRecvDataLength = iStatNetwork->NtoHl( pData[1] );

		// if this is a resync then abort the connection
		if( iRecvCommand == RESYNC_ID ) {
			iStatEngine->HandleError( KSTErrResyncCommand, (void*)0 );
			iSendStatus = ESendIdle;
			iRecvStatus = EReceiveIdle;
			iTransportStatus = EIdle;
			return;
		}

		// allocate the data buffers 
		asserte( iRecvDataReceived == 0 );
		if( iRecvDataLength > 0 )
			{
			// Determine if we use a memory receive buffer
			// or a temporary file.  Purely arbitary size.
			static const unsigned int maxSizeOfMemoryConsumer = (1024 * 4);

			if( iRecvDataLength < maxSizeOfMemoryConsumer )
				{
				iDataConsumer = CDataConsumerMemory::NewL( );
				}
			else
				{
				iDataConsumer = CDataConsumerFile::NewL( );
				}
			}

		// If the data length is greater that iDataPacketSize then the desktop will
		// packetise the data into fragments each with a header. If the data is not
		// packetised then the initial header is equivalent to a fragment header.
		if( iRecvDataLength <= iDataPacketSize ) {
			iRecvStatus = EReceivingFragmentHeader;
			recvCommand = iRecvCommand;
			recvLength = iRecvDataLength;
		}
		break;

	case EReceivingFragmentHeader:
		
		// check that this is a fragment header as expected
		asserte( aDataLength == KHeaderSize );
		pData = (TUint*)aData->Ptr();
		recvCommand = iStatNetwork->NtoHl( pData[0] );
		recvLength = iStatNetwork->NtoHl( pData[1] );

		// if this is not the expected command (e.g. a resync) then throw an error 
		if( (recvCommand != iRecvCommand) ) {
			iStatEngine->HandleError( KSTErrUnexpectedFragmentCommand, (void*)recvCommand );
			iSendStatus = ESendIdle;
			iRecvStatus = EReceiveIdle;
			iTransportStatus = EIdle;
			return;
		}
		break;

	case EReceivingData:
		{
		// received data is placed into the receive buffer
		asserte( (iRecvDataReceived + aDataLength) <= iRecvDataLength );
		iDataConsumer->AddData( *aData );
		iRecvDataReceived += aDataLength;
		break;
		}
	case EReceivingFragmentAck:
		
		// check that this is a fragment ack as expected
		asserte( aDataLength == KHeaderSize );
		pData = (TUint*)aData->Ptr();
		recvCommand = iStatNetwork->NtoHl( pData[0] );
		recvLength = iStatNetwork->NtoHl( pData[1] );

		// if this is not the expected command (e.g. a resync) then throw an error 
		if( (recvCommand != iSendCommand) ) {
			iStatEngine->HandleError( KSTErrUnexpectedFragmentCommand, (void*)recvCommand );
			iSendStatus = ESendIdle;
			iRecvStatus = EReceiveIdle;
			iTransportStatus = EIdle;
			return;
		}
		asserte( recvLength == 0 );

		// update the number of bytes acknowledged
		iSendDataAcknowledged = iSendDataWritten;
		break;

	case EReceiveIdle:
		;
		break;
	}

	// STEP 2: decide what to do next
	switch( iRecvStatus ) {

	case EReceivingInitialHeader:

		// send an acknowledgement for the initial header
		SendAckPacket( iRecvCommand );
		break;

	case EReceivingFragmentHeader:

		// if there is more data then wait for it -- otherwise the receive is complete
		if( recvLength > 0 ) {
			iTransportStatus = EReceivePending;
			iRecvStatus = EReceivingData;
			err = iStatNetwork->RequestReceive( recvLength );	
			asserte( err == KSTErrAsynchronous );
		} else {
			NotifyEngineReceive( aResult );
		}
		break;

	case EReceivingData:

		// If we have received some packetised data then we have to acknowledge it. Otherwise
		// we have completed a receive and should notify the engine and clean up.
		if( iRecvDataLength > iDataPacketSize)
			SendAckPacket( iRecvCommand );
		else
			NotifyEngineReceive( aResult );
		break;

	case EReceivingFragmentAck:

		// if the desktop has just acknowledged a fragment then we send the next fragment header,
		// even if there are no more bytes to send.	
		remainingBytes = iSendDataLength - iSendDataWritten;
		packetSize = ((remainingBytes < iDataPacketSize) ? remainingBytes : iDataPacketSize);
		SendHeaderPacket( iSendCommand, packetSize );
		break;

	case EReceiveIdle:
		;
		break;
	}
}

/**************************************************************************************
 *
 * Helper functions
 *
 *************************************************************************************/
void CStatTransportPacketisation::SendHeaderPacket( TUint aCommand, TUint aDataLength )
{
	TUint err;

	// construct the header in iSendHeader
	BuildHeader( aCommand, aDataLength );

	// set the state
	iTransportStatus = ESendPending;
	iRecvStatus = EReceiveIdle;
	iSendStatus = ((aDataLength > iDataPacketSize) ? ESendingInitialHeader : ESendingFragmentHeader);

	// send it
	err = iStatNetwork->RequestSend( &iSendHeader, KHeaderSize );
	asserte( err == KSTErrAsynchronous );
}

void CStatTransportPacketisation::SendAckPacket( TUint aCommand )
{
	TUint err;

	// construct the header in iSendHeader
	BuildHeader( aCommand, 0 );

	// set the state
	iTransportStatus = ESendPending;
	iRecvStatus = EReceiveIdle;
	iSendStatus = ESendingFragmentAck;

	// send it
	err = iStatNetwork->RequestSend( &iSendHeader, KHeaderSize );
	asserte( err == KSTErrAsynchronous );
}

void CStatTransportPacketisation::BuildHeader( TUint aCommand, TUint aDataLength )
{
	TInt nbCommand, nbLength;
	TUint *buff;

	// create the header
	nbCommand = iStatNetwork->HtoNl( aCommand );
	nbLength = iStatNetwork->HtoNl( aDataLength );
	iSendHeader.SetLength( 0 );
	buff = (TUint*)iSendHeader.Ptr();
	buff[0] = nbCommand;
	buff[1] = nbLength;
	iSendHeader.SetLength( KHeaderSize );
}

void CStatTransportPacketisation::NotifyEngineSend( TUint aResult )
{
	// check the state is as expected
	asserte( iSendDataWritten == iSendDataLength );
	asserte( (iSendDataLength <= iDataPacketSize) || (iSendDataAcknowledged == iSendDataLength) );
	
	// clear the state
	iSendCommand = 0;
	iSendDataLength = 0;
	iSendDataWritten = 0;
	iSendDataAcknowledged = 0;

	iTransportStatus = EIdle;
	iSendStatus = ESendIdle;

	// notify the engine
	iStatEngine->HandleSend( aResult );
}

void CStatTransportPacketisation::NotifyEngineReceive( TUint aResult )
{
	// check the state is as expected
	asserte( iRecvDataLength == iRecvDataReceived || iRecvDataLength==(unsigned long)-1);
	asserte( (iRecvDataLength == 0) || (iDataConsumer != NULL) );
	
	// clear the send state (allows sending from the receive handler) but don't clear
	// the recvstatus since the buffers are still in use. This could be solved by 
	// creating multiple buffers but there is no need now and this would be slow
	iTransportStatus = EIdle;
	iSendStatus = ESendIdle;

	// notify the engine 
	iStatEngine->HandleReceive( aResult, iRecvCommand, iDataConsumer );

	// clear the status state
	iRecvStatus = EReceiveIdle;

	// clear the recv buffer state
	iRecvCommand = 0;
	iRecvDataLength = 0;
	iRecvDataReceived = 0;
	iRecvDataAcknowledged = 0;

	if(iDataConsumer)
		{
		iDataConsumer->Delete( );
		iDataConsumer = NULL;
		}
}

