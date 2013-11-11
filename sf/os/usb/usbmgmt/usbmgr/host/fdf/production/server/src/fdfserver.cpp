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
*
*/

/**
 @file
 @internalComponent
*/

#include "fdfserver.h"
#include "fdfsession.h"
#include <usb/usblogger.h>
#include "utils.h"
#include "fdfapi.h"
#include "fdf.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "fdfserverTraces.h"
#endif

#ifdef _DEBUG
_LIT(KPanicCategory, "fdfsrv");
#endif


void CFdfServer::NewLC()
	{
	OstTraceFunctionEntry0( CFDFSERVER_NEWLC_ENTRY );
	
	CFdfServer* self = new(ELeave) CFdfServer;
	CleanupStack::PushL(self);
	// StartL is where the kernel checks that there isn't already an instance
	// of the same server running, so do it before ConstructL.
	self->StartL(KUsbFdfServerName);
	self->ConstructL();
	OstTraceFunctionExit0( CFDFSERVER_NEWLC_EXIT );
	}

CFdfServer::~CFdfServer()
	{
	OstTraceFunctionEntry0( CFDFSERVER_CFDFSERVER_DES_ENTRY );
	

	delete iFdf;
	OstTraceFunctionExit0( CFDFSERVER_CFDFSERVER_DES_EXIT );
	}

CFdfServer::CFdfServer()
 :	CServer2(CActive::EPriorityHigh)
	{
	}

void CFdfServer::ConstructL()
	{
	OstTraceFunctionEntry0( CFDFSERVER_CONSTRUCTL_ENTRY );
	
	iFdf = CFdf::NewL();
	OstTraceFunctionExit0( CFDFSERVER_CONSTRUCTL_EXIT );
	}

CSession2* CFdfServer::NewSessionL(const TVersion& aVersion,
	const RMessage2& aMessage) const
	{
	OstTraceFunctionEntry0( CFDFSERVER_NEWSESSIONL_ENTRY );
	OstTraceExt3( TRACE_NORMAL, CFDFSERVER_NEWSESSIONL, "aVersion = (%d,%d,%d)", aVersion.iMajor, aVersion.iMinor, aVersion.iBuild );
	(void)aMessage;

	// Check if we already have a session open.
	if ( iSession )
		{
		OstTrace0( TRACE_NORMAL, CFDFSERVER_NEWSESSIONL_DUP1, 
		        "Session in use");
		User::Leave(KErrInUse);
		}

	// In the production system, check the secure ID of the prospective
	// client. It should be that of USBSVR.
	// For unit testing, don't check the SID of the connecting client (it will
	// not be USBSVR but the FDF Unit Test server).
#ifndef __OVER_DUMMYUSBDI__

#ifndef __TEST_FDF__
	// NB When using t_fdf, this SID check needs disabling- t_fdf has yet
	// another SID.
	_LIT_SECURE_ID(KUsbsvrSecureId, 0x101fe1db);
	// Contrary to what the sysdoc says on SecureId, we specifically *don't*
	// use a _LIT_SECURITY_POLICY_S0 here. This is because (a) we emit our own
	// diagnostic messages, and (b) we don't want configuring this security
	// check OFF to allow any client to pass and thereby break our
	// architecture.
	TInt error = ( aMessage.SecureId() == KUsbsvrSecureId ) ? KErrNone : KErrPermissionDenied;
	LEAVEIFERRORL(error,OstTrace0( TRACE_NORMAL, CFDFSERVER_NEWSESSIONL_DUP2, 
            "SecureId error"););
#endif // __TEST_FDF__

#endif // __OVER_DUMMYUSBDI__

	// Version number check...
	TVersion v(KUsbFdfSrvMajorVersionNumber,
		KUsbFdfSrvMinorVersionNumber,
		KUsbFdfSrvBuildNumber);

	if ( !User::QueryVersionSupported(v, aVersion) )
		{
		OstTrace0( TRACE_NORMAL, CFDFSERVER_NEWSESSIONL_DUP3, 
                "Version not supported");
		User::Leave(KErrNotSupported);
		}

	CFdfServer* ncThis = const_cast<CFdfServer*>(this);
	ncThis->iSession = new(ELeave) CFdfSession(*iFdf, *ncThis);
	if(!ncThis->iFdf)
	    {
        OstTrace0( TRACE_FATAL, CFDFSERVER_NEWSESSIONL_DUP4,"ncThis->iFdf is empty"); 
        __ASSERT_DEBUG(EFalse,User::Panic(KPanicCategory,__LINE__));
	    }
            
	ncThis->iFdf->SetSession(iSession);

	OstTrace1( TRACE_NORMAL, CFDFSERVER_NEWSESSIONL_DUP5, "iSession = 0x%08x", iSession );
	OstTraceFunctionExit0( CFDFSERVER_NEWSESSIONL_EXIT );
	return iSession;
	}

void CFdfServer::SessionClosed()
	{
    OstTraceFunctionEntry0( CFDFSERVER_SESSIONCLOSED_ENTRY );

	if(!iSession)
	    {
        OstTrace0( TRACE_FATAL, CFDFSERVER_SESSIONCLOSED,"Empty iSession" ); 
        __ASSERT_DEBUG(EFalse,User::Panic(KPanicCategory,__LINE__));
	    }
	        
	iSession = NULL;
	iFdf->SetSession(NULL);

	OstTrace0( TRACE_NORMAL, CFDFSERVER_SESSIONCLOSED_DUP1, "no remaining sessions- shutting down" );
	
	// This returns control to the server boilerplate in main.cpp. This
	// destroys all the objects, which includes signalling device detachment
	// to any extant FDCs.
	// The session object could perfectly well do this in its destructor but
	// it's arguably more clear for the server to do it as it's the server
	// that's created immediately before calling CActiveScheduler::Start in
	// main.cpp.
	CActiveScheduler::Stop();
	OstTraceFunctionExit0( CFDFSERVER_SESSIONCLOSED_EXIT );
	}
