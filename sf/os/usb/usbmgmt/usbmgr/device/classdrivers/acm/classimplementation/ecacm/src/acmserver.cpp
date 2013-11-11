/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* ACM server.
*
*/

/**
 @file
 @internalComponent
*/

#include "acmserver.h"
#include "acmsession.h"
#include "acmserversecuritypolicy.h"
#include "acmserverconsts.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "acmserverTraces.h"
#endif


CAcmServer* CAcmServer::NewL(MAcmController& aAcmController)
	{
	OstTraceFunctionEntry0( CACMSERVER_NEWL_ENTRY );

	CAcmServer* self = new(ELeave) CAcmServer(aAcmController);
	CleanupStack::PushL(self);
	TInt err = self->Start(KAcmServerName);
	// KErrAlreadyExists is a success case (c.f. transient server boilerplate
	// code).
	if ( err != KErrAlreadyExists )
		{
		if (err < 0)
			{
			OstTrace1( TRACE_ERROR, CACMSERVER_NEWL, "CAcmServer::NewL;err=%d", err );
			User::Leave(err);
			}
		}
	CleanupStack::Pop(self);
	OstTraceFunctionExit0( CACMSERVER_NEWL_EXIT );
	return self;
	}

CAcmServer::~CAcmServer()
	{
	OstTraceFunctionEntry0( CACMSERVER_CACMSERVER_DES_ENTRY );
	OstTraceFunctionExit0( CACMSERVER_CACMSERVER_DES_EXIT );
	}

CAcmServer::CAcmServer(MAcmController& aAcmController)
 :	CPolicyServer(CActive::EPriorityStandard, KAcmServerPolicy),
 	iAcmController(aAcmController)
 	{
	OstTraceFunctionEntry0( CACMSERVER_CACMSERVER_CONS_ENTRY );
	OstTraceFunctionExit0( CACMSERVER_CACMSERVER_CONS_EXIT );
	}

CSession2* CAcmServer::NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const
	{
	OstTraceFunctionEntry0( CACMSERVER_NEWSESSIONL_ENTRY );
	//Validate session as coming from UsbSvr
	static _LIT_SECURITY_POLICY_S0(KSidPolicy, 0x101fe1db);
	TBool auth = KSidPolicy.CheckPolicy(aMessage);
	if(!auth)
		{
		OstTrace1( TRACE_ERROR, CACMSERVER_NEWSESSIONL_DUP1, 
							"CAcmServer::NewSessionL;KErrPermissionDenied=%d", 
							KErrPermissionDenied );
		User::Leave(KErrPermissionDenied);
		}

	// Version number check...
	TVersion v(	KAcmSrvMajorVersionNumber,
				KAcmSrvMinorVersionNumber,
				KAcmSrvBuildNumber);

	if ( !User::QueryVersionSupported(v, aVersion) )
		{
		OstTrace1( TRACE_ERROR, CACMSERVER_NEWSESSIONL_DUP2, 
							"CAcmServer::NewSessionL;KErrNotSupported=%d", 
							KErrNotSupported );
		User::Leave(KErrNotSupported);
		}

	CAcmSession* sess = CAcmSession::NewL(iAcmController);
	OstTraceExt1( TRACE_NORMAL, CACMSERVER_NEWSESSIONL, "CAcmServer::NewSessionL;sess=%p", sess );
	OstTraceFunctionExit0( CACMSERVER_NEWSESSIONL_EXIT );
	return sess;
	}
