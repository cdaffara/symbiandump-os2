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
 * Switches
 *
 ************************************************************************/

/*************************************************************************
 *
 * System Includes
 *
 ************************************************************************/

/*************************************************************************
 *
 * Local Includes
 *
 ************************************************************************/
#include "assert.h"
#include "stat_console.h"
#include "stat.h"

/*************************************************************************
 *
 * Definitions
 *
 ************************************************************************/
#define KConnectingStatusStr		_L("STAT is connecting.")
#define KWaitingForDataStatusStr	_L("STAT is waiting for data.")

/*************************************************************************
 *
 * CStatConsole
 *
 ************************************************************************/
CStatConsole *CStatConsole::NewL( CConsoleBase *aConsole, MNotifyLogMessage *const aMsg )
{
    CStatConsole *self = new (ELeave) CStatConsole();
    CleanupStack::PushL(self);
	self->ConstructL( aConsole, aMsg );
	CleanupStack::Pop();
    return self;
}

void CStatConsole::ConstructL( CConsoleBase *aConsole, MNotifyLogMessage *const aMsg )
{
	asserte( aConsole != NULL );

	iConsole = aConsole;
	iUserExitRequest=0;
	iMsg = aMsg;
}

CStatConsole::~CStatConsole( void )
{
}

void CStatConsole::HandleStatusChange( TInt /*aSessionId*/, TCommStatus aNewStatus )
{
	TPtrC status[] = {
		_L( "STAT is not running." ),
		_L( "STAT is initialising." ),
		_L( "STAT is initialised." ),
		KConnectingStatusStr,
		_L( "STAT is connected." ),
		_L( "STAT is disconnecting." ),
		_L( "STAT is disconnected." ),
		_L( "STAT is releasing." ),
		_L( "STAT is sending data."),
		KWaitingForDataStatusStr,
		_L( "<error>" )
	};
	if(aNewStatus==0 && iUserExitRequest==1){  //only closes down if user requested to.
		CActiveScheduler::Stop();
	}

	if( aNewStatus <= 9 ) {
//		iConsole->Printf( _L("%S\n"), status[aNewStatus] );
		iConsole->Printf( status[aNewStatus] );
		iConsole->Printf( _L("\n") );
	} else {
		iConsole->Printf( _L("Unknown status returned.\n") );
	}
}

void CStatConsole::HandleError( TInt aError, void * /*aErrorData*/ )
{
	// If aError is < 0 then it is an EPOC error code, if it is > 0 then
	// it is one of my error codes. aErrorData is defined for each code
	switch( aError ) 
	{
		case KErrAccessDenied:
			iConsole->Printf( _L("Comm port access denied.\n") );
			break;
	
		case KErrNotSupported:
			iConsole->Printf( _L("Unsupported comm port.\n") );
			break;

		case KErrCancel:
			iConsole->Printf( _L("Operation cancelled.\n") );
			break;
			
		case KErrDisconnected:
			break;

		default:
			iConsole->Printf( _L("Error occurred - %d.\n"), aError );
			break;
	}
}

void CStatConsole::HandleInfo( const TDesC *aInfo )
{
	if( aInfo != NULL ) {
		iConsole->Printf( _L("%S\n"), aInfo );
	}
}

void CStatConsole::UserExitRequest( )
{
	iUserExitRequest=1;
}
