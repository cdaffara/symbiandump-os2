// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// for (WINS && !EKA2) versions will be xxxServer.Dll and require a thread to be started
// in the process of the client. The client initialises the server by calling the
// one and only ordinal.
// 
//

/**
 @file
*/

#include "TE_EComSWITestSuiteServer.h"
#include <e32ldr.h>
#include "installStep.h"
#include "checkPlugin.h"
#include <e32debug.h>

// This TEF server uses setcap to create server with different names.
// So cannot use the hardcoded KServerName.

// _LIT(KServerName,"TE_EComSWITestSuite");

CTe_EComSWISuiteServer* CTe_EComSWISuiteServer::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CTe_EComSWISuiteServer * server = new (ELeave) CTe_EComSWISuiteServer();
	CleanupStack::PushL(server);

	// This test server uses different name after setcap
	TFileName exename = RProcess().FileName();
	TParsePtr pathparser(exename);

	// CServer base class call which can be either StartL or ConstructL,
	// the later will permit Server Logging.

	server->StartL(pathparser.Name());
	CleanupStack::Pop(server);
	return server;
	}

// DEFECT 097099 - This function disables Lazy library unloading
// which keeps dlls in cache for two minutes from system boot-up.  This was casuing this test  to fail
//as a plugin was being upgraded but ecom was returning the old plugin which was still in the cache. See defect for more details
LOCAL_C void CancelLazyDllUnloadingL()
	{
	RLoader loaderObj;
	User::LeaveIfError(loaderObj.Connect());
	CleanupClosePushL(loaderObj);
	User::LeaveIfError(loaderObj.CancelLazyDllUnload());
	RDebug::Print(_L("Cancelled Lazy Unloading of dlls"));
	CleanupStack::PopAndDestroy();
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
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CTe_EComSWISuiteServer* server = NULL;
	
	// Stoping Lazy Unloading of Dlls  - This is Not standard TEF code.
	CancelLazyDllUnloadingL();	
		
	// Create the CTestServer derived server
	TRAPD(err,server = CTe_EComSWISuiteServer::NewL());
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


CTestStep* CTe_EComSWISuiteServer::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;
	// They are created "just in time" when the worker thread is created
	// More test steps can be added below 
	
			if(aStepName == KInstallStep)
				{
				testStep = new CinstallStep(CinstallStep::EUseFileName);
				}
			else if(aStepName == KUninstallStep)
				{
				testStep = new CuninstallStep(CuninstallStep::EByUid);     			
				}
			else if(aStepName == KUninstallStepPackageName)
				{
				testStep = new CuninstallStep(CuninstallStep::EByPackage);     			
				}
			else if(aStepName == KListImplUidStep)
				{
				testStep = new CCheckListImplementation(CCheckListImplementation::EUseListImplUid);     			
				}
			else if(aStepName == KCreateImpl_ImpId_DtorId_Step)
				{
				testStep = new CCheckCreateImplementation(CCheckCreateImplementation::EUseImplUidAndDtorID);     			
				}
			else if(aStepName == KResolverInstallStep)
				{
				testStep = new CResolverInstallStep(CinstallStep::EUseFileName);
				}
			else if(aStepName == KUseCustomResolverStep)
				{
				testStep = new CUseCustomResolver();
				}
			else if(aStepName == KResolverUninstallStep)
				{
				testStep = new CResolverUninstallStep(CuninstallStep::EByUid);
				}
        		
	return testStep;
	}
