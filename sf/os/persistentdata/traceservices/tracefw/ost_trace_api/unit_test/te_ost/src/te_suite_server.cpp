// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Example file/test code to demonstrate how to develop a TestExecute Server
// Developers should take this project as a template and substitute their own
// for (WINS && !EKA2) versions will be xxxServer.Dll and require a thread to be started
// in the process of the client. The client initialises the server by calling the
// one and only ordinal.
//



/**
 @file Te_UTraceSuiteServer.cpp
 @internalTechnology
*/


#include "te_suite_server.h"
#include "te_suite_step_base.h"
#include "te_check_build_variability_step.h"
#include "te_check_record_content_step.h"
#include "te_check_performance_step.h"
#include "te_multiparttrace.h"
#include "te_macrostest.h"
#include "te_macrostest_disabled.h"

#ifdef TE_TRACE_ENABLED
_LIT(KServerName,"te_ost_enabled");
#endif
#ifdef TE_TRACE_DISABLED
_LIT(KServerName,"te_ost_disabled");
#endif
#ifdef TE_SYSTEM_TRACE_ENABLED
_LIT(KServerName,"te_ost_system_enabled");
#endif
#ifdef TE_SYSTEM_TRACE_DISABLED
_LIT(KServerName,"te_ost_system_disabled");
#endif

#ifdef TE_RECORD_CONTENT
	_LIT(KServerName,"te_ost_record_content");
#endif
	
#ifdef TE_MULTIPART_TRACE
	_LIT(KServerName,"te_ost_multipart_trace");
#endif

#ifdef TE_OST_MACROS_ENABLED
    _LIT(KServerName,"te_ost_macros_enabled");
#endif

#ifdef TE_OST_MACROS_DISABLED
    _LIT(KServerName,"te_ost_macros_disabled");
#endif

CTe_UTraceSuite* CTe_UTraceSuite::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CTe_UTraceSuite * server = new (ELeave) CTe_UTraceSuite();
	CleanupStack::PushL(server);
	
	server->ConstructL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}


// Secure variants much simpler
// For EKA2, just an E32Main and a MainL()
LOCAL_C void MainL()
/**
 * Secure variant
 * Much simpler, uses the new Rendezvous() call to sync with the client
 */
	{
	// Leave the hooks in for platform security
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().DataCaging(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CTe_UTraceSuite* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTe_UTraceSuite::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;
	}



GLDEF_C TInt E32Main()
/**
 * @return - Standard Epoc error code on process exit
 * Secure variant only
 * Process entry point. Called by client using RProcess API
 */
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAPD(err,MainL());
	delete cleanup;
	__UHEAP_MARKEND;
	return err;
    }


CTestStep* CTe_UTraceSuite::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;
	if(aStepName == KCheckBuildVariabilityStep)
    	testStep = new CCheckBuildVariabilityStep();
	if(aStepName == KCheckRecordContentStep)
    	testStep = new CCheckRecordContentStep();
	if(aStepName == KCheckPerformanceStep)
    	testStep = new CCheckPerformanceStep();
	if(aStepName == KMultiTraceTestStep)
		testStep = new CCTe_multiparttrace();
	if(aStepName == KOstMacrosEnabledStep)
	    testStep = new COstMacrosEnabledStep();
	if(aStepName == KOstMacrosDisabledStep)
	    testStep = new COstMacrosDisabledStep();
	return testStep;
	}
