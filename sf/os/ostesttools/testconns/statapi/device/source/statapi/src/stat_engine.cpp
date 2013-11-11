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
 ********************************************************************************/
#include <e32std.h>
#include <e32base.h>

/********************************************************************************
 *
 * Local Includes
 *
 ********************************************************************************/
#include "assert.h"
#include "stat.h"
#include "stat_engine.h"

/********************************************************************************
 *
 * Macro functions
 *
 ********************************************************************************/

/********************************************************************************
 *
 * Definitions
 *
 ********************************************************************************/
const TUint	KSyncMax		= 5;

/********************************************************************************
 *
 * CStatEngine -- construction
 *
 ********************************************************************************/
CStatEngine *CStatEngine::NewL( MNotifyStatController *aController, RFs *const aSession, MNotifyLogMessage *const aMsg )
{
	CStatEngine *self = new (ELeave) CStatEngine();
	CleanupStack::PushL( self );
	self->ConstructL( aController, aSession, aMsg );
	CleanupStack::Pop();
	return self;
}

void CStatEngine::ConstructL( MNotifyStatController *aController, RFs *const aSession, MNotifyLogMessage *const aMsg )
{	
	// check params
	asserte( aController != NULL );

	// setup all params
	iController = aController;
	iEngineStatus = EIdle;
	iRemoteHost = NULL;
	iResyncCount = 0;
	iFs =	aSession;
	iMsg = aMsg;
	iCommandDecoder = CStatApiCommandDecoder::NewL( iFs, iMsg );
}

CStatEngine::CStatEngine() : iDataSupplier( NULL ), iMsg( NULL ), iFs( NULL ), iDeleteLastFile(EFalse)
{
}

CStatEngine::~CStatEngine()
{
	// clean up all params
	if( iRemoteHost != NULL ) {
		delete iRemoteHost;
	}
	delete iCommandDecoder;

	if(iDataSupplier)
		{
		iDataSupplier->Delete( );
		iDataSupplier = NULL;
		}
}

/********************************************************************************
 *
 * CStatEngine -- MStatEngine implementation
 *
 ********************************************************************************/
void CStatEngine::StartEngine( MStatApiTransport *aStatTransport, 
	TStatConnectType aConnectType, TDesC *aRemoteHost )
{
	TInt exception, ret = KErrNone;
	iTransport = aStatTransport;

	// save the remote host address string
	iRemoteHost = new TPtrC( *aRemoteHost );

	// create the message log 

	// start the initialisation procedure the ugly second line means that if an
	// exception is thrown the ret will have the exception, otherwise it will have
	// the return code. Exceptions are always -ve and return values are always +ve
	// so the two number spaces don't overlap (as zero means success in both).
	iMsg->Msg( _L("ENGINE: Calling InitialiseL, %d (%S)."), aConnectType, aRemoteHost );
	SetState( EInitialising );
	TRAP( exception, (ret = iTransport->InitialiseL()) );
	ret = ((exception == KErrNone) ? ret : exception);
	HandleStateChange( KOpInitialise, ret );
}

// the controller calls stop whenever he (yes the controller is male) wants the engine
// to arrange itself and the transport into a state where he can safetly shut it down. It
// can be called at any time -- i.e. when the engine is in any state.
void CStatEngine::StopEngine( void )
{
	TInt exception, ret = KErrNone;

	switch( iEngineStatus ) {
		
	case EIdle:
		;
		break;

	case EConnecting:
	case EConnected:
	case EReceivePending:
	case ESendPending:
		iMsg->Msg( _L("ENGINE: Calling Disconnect.") );
		SetState( EDisconnecting );
		TRAP( exception, (ret = iTransport->Disconnect()) );
		ret = ((exception == KErrNone) ? ret : exception);
		HandleStateChange( KOpDisconnect, ret );
		break;

	case EDisconnecting:
	case EDisconnected:
	case EReleasing:
		// nothing I can really do -- there is already a disconnect call
		// pending -- just return and wait for notification -- or force
		// disconnect by just deleting the object
		break;

	case EInitialising:
	case EInitialised:
		;
		break;
	case ELast:
		;
		break;
	}
}

/********************************************************************************
 *
 * CStatEngine - Higher level transport handling code. All errors and startup
 * shutdown issues are handled by the MNotifyStatEngine implementation.
 *
 ********************************************************************************/
void CStatEngine::OnConnect( void )
{	
	iMsg->Msg( _L("ENGINE: Waiting for next command.") );
	SetState( EReceivePending );
	TInt err = iTransport->RequestReceive();
	asserte( err == KSTErrAsynchronous );
}

void CStatEngine::OnRecvCommand( TUint aCommand, MDataConsumer *const aDataConsumer )
{
	TInt err = KErrNone;
	TInt dataLength = 0;

	if (aCommand == 'B')
		iDeleteLastFile = EFalse;
	else if (aCommand == 'S' || aCommand == 'X')
		iDeleteLastFile = ETrue;
	
	// Clean up the old data supplier.
	// If we are getting a command then it is safe to assume
	// that everyone has finished with the old data supplier as
	// they are valid only for the context of the current
	// command.
	if(iDataSupplier)
		{
		iDataSupplier->Delete( );
		iDataSupplier = NULL;
		}
	
	// if this is a resync command then we have already done all we need to do so
	// just return and wait for the next command. If we reach the maximum of 
	// resync commands disconnect and reset.
	if( aCommand == RESYNC_ID ) {
		iResyncCount++;
		if( iResyncCount > KSyncMax ) {
			iMsg->Msg( _L("ENGINE: Resync limit reached, calling Disconnect().") );
			SetState( EDisconnecting );
			err = iTransport->Disconnect();
			iMsg->Msg( _L("ENGINE: Synchronous response from Disconnect(%d)."), err );
			HandleStateChange( KOpDisconnect, err );
		}
		SetState( ESendPending );
		return;
	}
	iResyncCount = 0;

	if(aCommand == REFRESH_ID)
	{
		SetState( EDisconnecting );
		err = iTransport->Disconnect();
		HandleStateChange( KOpDisconnect, err );
		SetState( ESendPending );

		return;
	}

	// otherwise execute the command
	err = iCommandDecoder->ExecuteCommand( aCommand, aDataConsumer, &iDataSupplier );

	if( err == KErrNone )
		{
		if(iDataSupplier)
			{
			err = iDataSupplier->GetTotalSize( dataLength );
			}
		}

	// if there was an error then the result code is FAILED_ID
	if( err != KErrNone )
		{
		aCommand = FAILED_ID;
		}

	// now send the response and return
	iMsg->Msg( _L("ENGINE: Sending reply (%c, %d)."), (char)aCommand, dataLength );
	SetState( ESendPending );
	err = iTransport->RequestSend( aCommand, iDataSupplier );
	asserte( err == KSTErrAsynchronous );

	return;
}

/********************************************************************************
 *
 * CStatEngine -- MNotifyStatEngine implementation -- this is the lower level 
 * which maintains the startup / shutdown protocol with the transport to keep 
 * them happy and deal with the asynchronous behaviour. 
 *
 ********************************************************************************/
void CStatEngine::HandleStateChange( TStateOp aOperation, TInt aResult ) 
{
	TPtrC opname[] = {	_L("ENGINE: <null> - response (%d)."), 
						_L("ENGINE: InitialiseL - response (%d)."),
						_L("ENGINE: ConnectL - response (%d)."),
						_L("ENGINE: Disconnect - response (%d)."),
						_L("ENGINE: Release - response (%d).") };

	iMsg->Msg( opname[aOperation], aResult );
	while( (aResult != KSTErrAsynchronous) ) {
		if( aResult != KSTErrSuccess )
			iController->HandleError( aResult, NULL );
		HandleSingleStateChange( &aOperation, &aResult );
		iMsg->Msg( opname[aOperation], aResult );
	}
}

void CStatEngine::HandleSingleStateChange( TStateOp *aOperation, TInt *aResult )
{
	TStateOp thisOperation = *aOperation;
	TInt exception, ret = KErrNone, thisResult = *aResult;

	// should never get in here with a result of async
	asserte( *aResult != KSTErrAsynchronous );

	// set the operation to none by default and asynchronous which will break out of the above loop
	*aOperation = KOpNone;
	*aResult = KSTErrAsynchronous;

	// handle each operation
	switch( thisOperation ) {

	// if initialise failed then call release, otherwise call connect
	case KOpInitialise:			
		if( thisResult == KSTErrSuccess ) {
			SetState( EInitialised );
			*aOperation = KOpConnect;
			SetState( EConnecting );
			TRAP( exception, (ret = iTransport->ConnectL(iRemoteHost)) );
			*aResult = ((exception == KErrNone) ? ret : exception);
		} else {
			*aOperation = KOpRelease;
			SetState( EReleasing );
			*aResult = iTransport->Release();
		}
		break;

	// if connect failed call disconnect, otherwise call the internal OnConnect function
	case KOpConnect:
		if( thisResult != KSTErrSuccess ) {
			SetState( EDisconnecting );
			*aOperation = KOpDisconnect;
			*aResult = iTransport->Disconnect();
		} else { 
			SetState( EConnected );
			OnConnect();
		}
		break;

	// call release after disconnect regardless of result
	case KOpDisconnect:
		*aOperation = KOpRelease;
		SetState( EReleasing );
		*aResult = iTransport->Release();
		break;

	// exit after release
	case KOpRelease:
		SetState( EIdle );
		break;

	case KOpNone:
		;
		break;
	}
}

void CStatEngine::HandleInitialise( TInt aResult )
{
	HandleStateChange( KOpInitialise, aResult );
}

void CStatEngine::HandleConnect( TInt aResult )
{
	HandleStateChange( KOpConnect, aResult );
}

void CStatEngine::HandleDisconnect( TInt aResult )
{
	HandleStateChange( KOpDisconnect, aResult );
}

void CStatEngine::HandleRelease( TInt aResult )
{
	HandleStateChange( KOpRelease, aResult );
}

void CStatEngine::HandleSend( TInt aResult )
{
	TInt err;

	// we must either be sending an ack or a response 
	asserte( iEngineStatus == ESendPending );
	
	if (iDeleteLastFile)
		{
		iCommandDecoder->DeleteLastFile();
		iDeleteLastFile = EFalse;
		}
	// now that the reply has been sent wait for the next command
	iMsg->Msg( _L("ENGINE: Reply sent (%d)."), aResult );
	iMsg->Msg( _L("ENGINE: Waiting for next command.") );
	asserte( iEngineStatus == ESendPending );
	SetState( EReceivePending );
	err = iTransport->RequestReceive();
	asserte( err == KSTErrAsynchronous );
}

void CStatEngine::HandleReceive( TInt aResult, const TUint aCommand,
		MDataConsumer *const aDataConsumer )
{
	// log the receipt
	TInt dataLength = 0;
	
	if(aDataConsumer)
		{
		aDataConsumer->GetTotalSize( dataLength );
		}
	iMsg->Msg( _L("ENGINE: Command Received (%d, %c, %d)."), aResult, (char)(aCommand), dataLength );

	// verify that the engine state is as expected
	asserte( iEngineStatus == EReceivePending );

	// process the command
	iMsg->Msg( _L("ENGINE: Processing command (%c)."), (char)(aCommand) );
	OnRecvCommand( aCommand, aDataConsumer );
}

void CStatEngine::HandleError( TInt aError, void* aErrorData )
{
	// log the error
	iMsg->Msg( _L("ENGINE: Error encountered (%d)."), aError );

	// call the upper layers so that the UI can notify the user that an error has
	// occured.
	iController->HandleError( aError, aErrorData );
	
	// on an error the engine takes the responsibility of closing itself down -- and 
	// then informing the controller than it can clean up the objects.
	SetState( EDisconnecting );
	TInt err = iTransport->Disconnect();
	HandleStateChange( KOpDisconnect, err );
}

void CStatEngine::HandleInfo( const TDesC *aInfo )
{
	iController->HandleInfo( aInfo );
}

void CStatEngine::SetState( TCommStatus aNewStatus )
{
	// change the internal status and notify the controller of the change
	iEngineStatus = aNewStatus;
	asserte( iController != NULL );
	iController->HandleStatusChange( iEngineStatus );
}

