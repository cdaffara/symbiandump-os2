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
 * System Includes
 *
 *******************************************************************************/
#include <es_sock.h>
#include <in_sock.h>

/********************************************************************************
 *
 * Local Includes
 *
 *******************************************************************************/
#include "assert.h"
#include "ntoh.h"
#include "stat_tcpip.h"
#include "../../../../common/inc/serialpacketsize.h"

/********************************************************************************
 *
 * Macro functions
 *
 ********************************************************************************/

/********************************************************************************
 *
 * CStatTransportTCPIP -- Constructor
 *
 *******************************************************************************/
CStatTransportTCPIP *CStatTransportTCPIP::NewL( void )
{
	CStatTransportTCPIP *self = new (ELeave) CStatTransportTCPIP();
    CleanupStack::PushL(self);
	self->ConstructL( );
	CleanupStack::Pop();
    return self;
}

CStatTransportTCPIP::CStatTransportTCPIP() : CActive(EPriorityStandard)
{
}

void CStatTransportTCPIP::ConstructL( void )
{
	// add this to active scheduler
	CActiveScheduler::Add(this); 

	// initialise all params	

	iBufferPtr = NULL;
	iTransport = NULL;
	iTransportStatus = EIdle;
	iRWStatus = ENoRW;

	iMaxPacketSize = KMaxTCPIPPacketSize;
	
	iBuffer = HBufC8::New( iMaxPacketSize );
	
}

CStatTransportTCPIP::~CStatTransportTCPIP()
{
	// this will call cancel and remove the active object -- this will call cancel
	Deque(); 

	// cleanup the sockets
	switch( iTransportStatus ) {

	case EIdle:
	case EInitialised:
	case EDisconnected:
	case EError:
		break;

	case EConnected:
		iDataSocket.Shutdown( RSocket::EImmediate, iStatus );
		/* fall through */

	case EConnecting:
		iDataSocket.Close();
		iListenSocket.Shutdown( RSocket::EImmediate, iStatus );
		iListenSocket.Close();
		iSocketServ.Close();
		break;

	case EDisconnectingData:
	case EDisconnectingListen:
		;
		break;
	}

	if( iBuffer )
		{
			delete iBuffer;
			iBuffer = NULL;
		}

	if( iBufferPtr )
		{
			delete iBufferPtr;
			iBufferPtr = NULL;
		}
}

/********************************************************************************
 *
 * CStatTransportTCPIP -- MStatApiTransport
 *
 *******************************************************************************/
TInt CStatTransportTCPIP::InitialiseL( MNotifyStatTransport *aTransport )
{
	// save the transport interface
	iTransport = aTransport;

	// everything here is done in connect
	iTransportStatus = EInitialised;
	return KSTErrSuccess;
}

TInt CStatTransportTCPIP::Release( void )
{
	// release has nothing to do
	asserte( (iTransportStatus == EDisconnected) || (iTransportStatus == EInitialised) );
	iTransportStatus = EIdle;
	return KSTErrSuccess;
}

TInt CStatTransportTCPIP::ConnectL( TDesC* /*aRemoteHost*/ )
{
	// make sure we are in the correct state
	asserte( iTransportStatus == EInitialised );

	// connect to the socket server, create a socket, bind, listen, accept
	User::LeaveIfError( iSocketServ.Connect() );
	User::LeaveIfError( iListenSocket.Open(iSocketServ, KAfInet, KSockStream, KProtocolInetTcp) );
	User::LeaveIfError( iListenSocket.SetLocalPort( KLittleStatPort) );
	User::LeaveIfError( iListenSocket.Listen(KLittleStatListenQueue) );

	// create a blank socket which is used as the data socket
	User::LeaveIfError( iDataSocket.Open(iSocketServ) );

	// everything should now be set up, we just wait for a stat connection
	asserte( !IsActive() );
	iListenSocket.Accept( iDataSocket, iStatus );
	SetActive();
	iTransportStatus = EConnecting;

	// tell the client to wait for an asynchronous response
	return KSTErrAsynchronous;
}

TInt CStatTransportTCPIP::Disconnect( void )
{
	// must be connected 
	asserte( (iTransportStatus == EInitialised) || 
			(iTransportStatus == EConnected)   || 
			(iTransportStatus == EConnecting)  ||
			(iTransportStatus == EDisconnectingData) ||
			(iTransportStatus == EDisconnectingListen) );

	// cancel any pending ops 
	Cancel();

	// clean up the sockets depending on the state
	switch( iTransportStatus ) {

	case EConnected:
		iDataSocket.Shutdown( RSocket::ENormal, iStatus );
		SetActive();
		iTransportStatus = EDisconnectingData;
		return KSTErrAsynchronous;

	case EConnecting:
	case EDisconnectingData:
		iDataSocket.Close();
		iListenSocket.Shutdown( RSocket::ENormal, iStatus );
		SetActive();
		iTransportStatus = EDisconnectingListen;
		return KSTErrAsynchronous;

	case EInitialised:
	case EDisconnectingListen:
		// initialised may mean that ConnectL threw an error -- so close the resources
		iDataSocket.Close();
		iListenSocket.Close();
		iSocketServ.Close();
		return KSTErrSuccess;

	default:
		;
	}
	return KSTErrSuccess;
}

TInt CStatTransportTCPIP::RequestSend( TDesC8 *aData, const TUint /*aDataLength*/ )
{

	// make sure the state is correct
	asserte( iTransportStatus == EConnected );
	asserte( iRWStatus == ENoRW );
	iRWStatus = EWritePending;
	
	iDataSocket.Write( *aData, iStatus );
	SetActive();
	
	// tell the caller to wait for an asynchronous response
	return KSTErrAsynchronous;


}

TInt CStatTransportTCPIP::RequestReceive( TUint aByteCount )
{

	// ensure that there are no reads in progress
	asserte( iTransportStatus == EConnected );
	asserte( !IsActive() );
	asserte( iRWStatus == ENoRW );
	iRWStatus = EReadPending;

	asserte( aByteCount <= static_cast<TUint>(iMaxPacketSize) );
	asserte( !IsActive() );
	
	if(!iBufferPtr)
		{
		iBufferPtr = new TPtr8(const_cast<unsigned char*>(iBuffer->Ptr( )),aByteCount);	
		}
	else
		if(iBufferPtr->MaxLength()!=aByteCount)
			{
			delete iBufferPtr;
			iBufferPtr = new TPtr8(const_cast<unsigned char*>(iBuffer->Ptr( )),aByteCount);
			}
	
	iDataSocket.Read( *iBufferPtr, iStatus );
	SetActive();

	// return to the caller
	return KSTErrAsynchronous;
	
}

TText8 *CStatTransportTCPIP::Error( void )
{
	return NULL;
}

TInt CStatTransportTCPIP::GetPacketSize()
{
	// The packet size is configured when we initialise the port.
	return iMaxPacketSize;
}

/********************************************************************************
 *
 * CStatTransportTCPIP -- Active Object
 *
 *******************************************************************************/
void CStatTransportTCPIP::RunL( void )
{
	// if there was an error during connectiong then tell the engine this
	if( (iTransportStatus == EConnecting) && (iStatus != KErrNone) ) {
		iTransport->HandleError( KSTErrConnectFailure, (void*)iStatus.Int() );
		return;
	}

	//reconnect without restarting STAT
	if(iStatus != KErrNone)
		{
		//if no error due to end of socket connection
		if(iStatus != KErrDisconnected && iStatus != KErrEof)
			{
			_LIT(KFormat,"Error during TCPIP: %d\n");
			TBuf<50> lBuf;
			lBuf.Format(KFormat,iStatus.Int());
			iTransport->HandleInfo(&lBuf);	
			}
		
		// close and reopen the socket
		iDataSocket.Close();  
		User::LeaveIfError(iDataSocket.Open(iSocketServ));

		// wait for a new connection
		iTransportStatus = EConnecting;
		iRWStatus = ENoRW;
		iListenSocket.Accept( iDataSocket, iStatus );
		SetActive();
		return;
		}

	// handle connection response 
	if( iTransportStatus == EConnecting ) {
		asserte( iStatus == KErrNone );
		iTransportStatus = EConnected;
		iTransport->HandleConnect( KErrNone );
		return;
	}

	// handle shutdown data socket
	if( iTransportStatus == EDisconnectingData ) {
		iDataSocket.Close();
		iListenSocket.Shutdown( RSocket::ENormal, iStatus );
		SetActive();
		iTransportStatus = EDisconnectingListen;
		return;
	}

	// handle shutdown listen
	if( iTransportStatus == EDisconnectingListen ) {
		iListenSocket.Close();
		iTransportStatus = EDisconnected;
		iSocketServ.Close();
		iTransport->HandleDisconnect( KErrNone );
		return;
	}

	// if we are writing then notify of the write
	if( iRWStatus == EWritePending ) {
		iRWStatus = ENoRW;
		iTransport->HandleSend( KErrNone );
		return;
	}

	// if we are reading then notify of the read
	if( iRWStatus == EReadPending ) {
		iRWStatus = ENoRW;
		iTransport->HandleReceive( KErrNone, iBufferPtr, iBufferPtr->Length( ) );
		return;
	}
}

void CStatTransportTCPIP::DoCancel( void )
{
	if( iTransportStatus == EConnecting )
		{
		iListenSocket.CancelAccept();
		}

	if( iRWStatus == EReadPending )
		{
		iDataSocket.CancelRead();
		}

	if( iRWStatus == EWritePending )
		{
		iDataSocket.CancelWrite();
		}

	iRWStatus = ENoRW;
}

