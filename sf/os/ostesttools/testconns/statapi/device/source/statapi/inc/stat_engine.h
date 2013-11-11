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
 * CStatEngine - central hub of stat
 *
 ********************************************************************************/
#ifndef __CSTATENGINE_H__
#define __CSTATENGINE_H__

/********************************************************************************
 *
 * Local Includes
 *
 ********************************************************************************/
#include "stat_interfaces.h"
#include "statapi_commanddecoder.h"
#include "filedump.h"

/********************************************************************************
 *
 * Definitions
 *
 ********************************************************************************/
//const char	RESYNC_ID		= '?';
//const char	FAILED_ID		= '*';
//const TUint	KSyncMax		= 5;

typedef enum { KOpNone, KOpInitialise, KOpConnect, KOpDisconnect, KOpRelease } TStateOp;

/********************************************************************************
 *
 * Types
 *
 ********************************************************************************/
class CStatEngine :  public CBase, public MNotifyStatEngine, public MStatEngine
{
public:
	// construction
	static CStatEngine *NewL( MNotifyStatController *aController, RFs *const aSession, MNotifyLogMessage *const aMsg );
	CStatEngine();
	virtual ~CStatEngine();

	// from MNotifyStatEngine
	void HandleInitialise( TInt aResult );
	void HandleConnect( TInt aResult );
	void HandleSend( TInt aResult );
	void HandleReceive( TInt aResult, const TUint aCommand,
							MDataConsumer *const aDataConsumer );
	void HandleDisconnect( TInt aResult );
	void HandleRelease( TInt aResult );
	void HandleError( TInt aError, void *aErrorData );
	void HandleInfo( const TDesC *aInfo );

	// from MStatEngine
	void StartEngine( MStatApiTransport *aStatTransport, TStatConnectType aConnectType, TDesC *aRemoteHost );
	void StopEngine( void );

private:
	// higher level transport interface functions
	void OnConnect( void );
	void OnRecvCommand( TUint aCommand, MDataConsumer *const aDataConsumer );

	// internal
	void ConstructL( MNotifyStatController *aController, RFs *const aSession, MNotifyLogMessage *const aMsg  );
	void HandleStateChange( TStateOp aOperation, TInt aResult );
	void HandleSingleStateChange( TStateOp *aOperation, TInt *aResult );
	void SetState( TCommStatus aNewStatus );

private:
	TUint iResyncCount;
	TCommStatus iEngineStatus;
	TPtrC *iRemoteHost;
	MStatApiTransport *iTransport;
	MNotifyStatController *iController;
	CStatApiCommandDecoder *iCommandDecoder;

	MDataSupplier *iDataSupplier;	// The data supplier is 
									// use to pass data from the
									// command decoder to the
									// transport layers.
									// Each specific object is
									// valid for the context of a 
									// single command (and 
									// destroyed).

	MNotifyLogMessage *iMsg;
	RFs	*iFs;
	TBool iDeleteLastFile;  
};

#endif //__CSTATENGINE_H__
