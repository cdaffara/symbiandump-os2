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



 /************************************************************************* 
 *
 * System Includes
 *
 *************************************************************************/

/*************************************************************************
 *
 * Local Includes
 *
 *************************************************************************/
#include "assert.h"
#include "stat.h"
#include "stat_interfaces.h"
#include "stat_controller.h"
#include "stat_engine.h"


/*************************************************************************
 *
 * Definitions
 *
 *************************************************************************/

/*************************************************************************
 *
 * CStatController - Construction
 *
 *************************************************************************/
CStatController *CStatController::NewL()
{
    CStatController *self = new (ELeave) CStatController();
    CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
    return self;
}

CStatController::CStatController() : CActive(EPriorityHigh), iFs( NULL ), iMsg( NULL )
{
}

void CStatController::ConstructL( void ) 
{
	// add me to the active scheduler
	CActiveScheduler::Add(this); 

	// params
	iEngine = NULL;
	iSessionStatus = EIdle;
	iTransport = NULL;
	iUI = NULL;
	iConnectedSuccessfully = EFalse;
	iUserInitiatedExitRequest = EFalse;
	iFs =	NULL;
	iMsg = NULL;

	// wait for notifications
	iStatus = KRequestPending;
	SetActive();
}

CStatController::~CStatController()
{
	// clear notification -- deque (and CActive destructor) will fail otherwise
	Notify( KErrCancel );
	Deque();

	// make sure the session is closed
	asserte( iSessionStatus == EIdle );
	asserte( iEngine == NULL );
	asserte( iTransport == NULL );
}

/*************************************************************************
 *
 * CStatController - public interface
 *
 *************************************************************************/
TInt CStatController::StartSession( TStatConnectType aConnectType, TDesC *aConnectParams, MNotifyUI *aUI, RFs *const aSession, MNotifyLogMessage *const aMsg )
{
	TInt exp =	KErrNone;

	iFs =	aSession;
	iMsg =	aMsg;

    if(!aConnectParams) {
        return -1;
    }

	// this is the notification sink for this session
	iUI = aUI;
	iConnectType = aConnectType;
	asserte( aConnectParams->Length() <= KAddressTextLimit );
    iConnectParams.Copy( *aConnectParams );
		
	// create the engine with a callback interface to the controller, create the 
	// appropriate transport, and start the engine
	iConnectedSuccessfully = EFalse;
	TRAP( exp, (iEngine = CStatEngine::NewL(this, iFs, iMsg)) );
	if( exp != KErrNone ) {
		return -1;
	}

	// if the start transport leaves then its because it failed to create an object. This
	// is either because the network or transport creation failed left. If it was the
	// transport then the network may still have memory allocated.
	TRAP( exp, (iTransport = StartTransportL(iConnectType)) );
	if( exp != KErrNone ) {
		if( iSerialNetwork != NULL ) {
			delete iSerialNetwork;
			iSerialNetwork = 0;
		}
		return -1;
	}

	iEngine->StartEngine( iTransport, iConnectType, &iConnectParams );

	// this controller only supports one session which always has ID = 1
	return 1;
}

TInt CStatController::StopSession( TInt aSessionId )
{
	// make sure this is a valid, active, session
	if( (aSessionId != 1) || (iSessionStatus == EIdle) )
	{
		// If the session is valid and idle then it is not
		// initialised and connected correctly.
		// Call Notify to cancel the outstanding asynchronous events
		// we are registered for.
		// This fixes a problem that appears when the engine fails to
		// start correctly (due to COM port failures for example) and
		// caused the CActiveScheduler never to stop as this object
		// never received a callback when it needs one to remove
		// itself from the active state.
		// If the CActiveScheduler does not shut down then the
		// program never exits and blocks.
		Notify( KErrNone );
		//
		return KErrBadHandle;
	}

	iMsg->Msg( _L("CONTROLLER: Stopping session.") );

	// tell the engine to stop and then wait for an asynchronous response
	iUserInitiatedExitRequest = ETrue;
	iEngine->StopEngine();
	return KErrNone;
}

TInt CStatController::SessionStatus( TInt aSessionId )
{
	if( aSessionId != 1 )
		return KErrBadHandle;
	return iSessionStatus;
}

void CStatController::SetStatus( TCommStatus aNewStatus )
{
	iSessionStatus = aNewStatus;
	if( iUI != NULL )
		iUI->HandleStatusChange( 1, iSessionStatus );
	if( aNewStatus == EConnected ) 
		iConnectedSuccessfully = ETrue;
}

/*************************************************************************
 *
 * CStatController - MNotifyStatController
 *
 *************************************************************************/
void CStatController::HandleStatusChange( TCommStatus aNewStatus )
{
	// If the engine has gone idle then everything is done and we can kill
	// everything off. But, this function was called by the engine, so we
	// can't kill it here -- send self an event to clean things up.
	if( aNewStatus == EIdle ) {
		Notify( KErrNone );
		return;
	}
	
	// update the status
	SetStatus( aNewStatus );
}

// The engine deals with the transports in the face of errors itself, and the 
// interaction with the engine is really handled via HandleStatusChange. This
// function is mainly here to be able to pass information up from the transport
// and the engine to the UI / user
void CStatController::HandleError( TInt aError, void *aErrorData )
{
	if( iUI != NULL ) 
		iUI->HandleError( aError, aErrorData );
}

// Info is for end-user consumption only and is sent directly to the UI.
void CStatController::HandleInfo( const TDesC *aInfo )
{
	if( iUI != NULL )
		iUI->HandleInfo( aInfo );
}

/*************************************************************************
 *
 * CStatController - CActive
 *
 *************************************************************************/
void CStatController::RunL()
{
	// delete the engine and the transport
	delete iEngine;
	iEngine = NULL;
	KillTransport();

	// now change the status to idle
	SetStatus( EIdle );

	// wait again
	iStatus = KRequestPending;
	SetActive();

	// But if this wasn't a user requested exit -- and the last connection
	// did connect successfully, then restart. Otherwise clear the flags.
	if( (iConnectedSuccessfully) && (!iUserInitiatedExitRequest) ) {
		iConnectedSuccessfully = EFalse;
		iEngine = CStatEngine::NewL( this, iFs, iMsg );
		iTransport = StartTransportL( iConnectType );
		iEngine->StartEngine( iTransport, iConnectType, &iConnectParams );
	} else {
		iUserInitiatedExitRequest = EFalse;
		iConnectedSuccessfully = EFalse;
	}
}		

void CStatController::DoCancel()
{
}

void CStatController::Notify( TInt aErrorStatus )
{
	TRequestStatus *pstatus;
	pstatus = &iStatus;
	TRequestStatus *&rstatus = pstatus;
	User::RequestComplete( rstatus, aErrorStatus );
}

/*************************************************************************
 *
 * CStatController - Private
 *
 *************************************************************************/
MStatApiTransport *CStatController::StartTransportL( TStatConnectType aConnectType )
{
	switch( aConnectType ) {

#ifndef LIGHT_MODE
	case ETCPIP:
		iTcpipTransport = CStatTransportTCPIP::NewL( );
		iPacketisationTransport = CStatTransportPacketisation::NewL( iEngine, iTcpipTransport, iTcpipTransport->GetPacketSize() );
		return iPacketisationTransport;

	case EInfrared:
		iSerialNetwork = CStatApiSerial::NewL( _L("IrCOMM") );
		iPacketisationTransport = CStatTransportPacketisation::NewL( iEngine, iSerialNetwork, iSerialNetwork->GetPacketSize() );
		return iPacketisationTransport;

	case EBluetooth:
		iBluetoothTransport = CStatTransportBT::NewL();
		iPacketisationTransport = CStatTransportPacketisation::NewL( iEngine, iBluetoothTransport, iBluetoothTransport->GetPacketSize() );
		return iPacketisationTransport;
	
	case EUsb:
		iUsbNetwork = CStatApiUsb::NewL();
		iPacketisationTransport = CStatTransportPacketisation::NewL( iEngine, iUsbNetwork, iUsbNetwork->GetPacketSize() );
		return iPacketisationTransport;

		
		
#endif // ifndef LIGHT_MODE

	case ESerial:
		iSerialNetwork = CStatApiSerial::NewL( _L("ECUART") );
		iPacketisationTransport = CStatTransportPacketisation::NewL( iEngine, iSerialNetwork, iSerialNetwork->GetPacketSize() );
		return iPacketisationTransport;

	default:
		;
	}
	return NULL;
}

void CStatController::KillTransport()
{
	switch( iConnectType ) {

#ifndef LIGHT_MODE
	case ETCPIP:
		delete iTcpipTransport;
		delete iPacketisationTransport;
		iPacketisationTransport = NULL;
		iTcpipTransport = NULL;
		break;

	case EBluetooth:
		delete iBluetoothTransport;
		delete iPacketisationTransport;
		iBluetoothTransport = NULL;
		iPacketisationTransport = NULL;
		break;

	case EInfrared:
		delete iSerialNetwork;
		delete iPacketisationTransport;
		iSerialNetwork = NULL;
		iPacketisationTransport = NULL;
		break;
#endif // ifndef LIGHT_MODE

	case ESerial:
		delete iSerialNetwork;
		delete iPacketisationTransport;
		iSerialNetwork = NULL;
		iPacketisationTransport = NULL;
		break;

	default:
		;
	}
	iTransport = NULL;	
}
