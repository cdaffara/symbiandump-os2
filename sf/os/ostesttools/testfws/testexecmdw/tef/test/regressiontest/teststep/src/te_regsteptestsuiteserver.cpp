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
* Example file/test code to demonstrate how to develop a TestExecute Server
* Developers should take this project as a template and substitute their own
* for (WINS && !EKA2) versions will be xxxServer.Dll and require a thread to be started
* in the process of the client. The client initialises the server by calling the
* one and only ordinal.
*
*/



/**
 @file Te_RegressionSuiteServer.cpp
*/

#include "te_regsteptestsuiteserver.h"
#include "readwriteconfigstep.h"
#include "readwriteconfignegstep.h"
#include "seterrorstep.h"
#include "writeboolstep.h"
#include "writehexstep.h"
#include "writeintstep.h"
#include "writestringstep.h"
#include "abortstep.h"
#include "oomstep.h"
#include "shortfunsstep.h"
#include "testmacrosstep.h"
#include "panicstep.h"
#include "leaveforever.h"
#include "te_regsteptestsuitedefs.h"

CTe_RegStepTespSuiteServer* CTe_RegStepTespSuiteServer::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CTe_RegStepTespSuiteServer * server = new (ELeave) CTe_RegStepTespSuiteServer();
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
	CTe_RegStepTespSuiteServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTe_RegStepTespSuiteServer::NewL());
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


CTestStep* CTe_RegStepTespSuiteServer::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;
	if(aStepName == KReadWriteConfigStep)
		{
		testStep = new CReadWriteConfigStep();
		}
	else if(aStepName == KReadWriteConfigNegStep)
		{
		testStep = new CReadWriteConfigNegStep();
		}
	else if(aStepName == KSetErrorStep)
		{
		testStep = new CSetErrorStep();
		}
	else if(aStepName == KWriteBoolStep)
		{
		testStep = new CWriteBoolStep();
		}
	else if(aStepName == KWriteHexStep)
		{
		testStep = new CWriteHexStep();
		}
	else if(aStepName == KWriteIntStep)
		{
		testStep = new CWriteIntStep();
		}
	else if(aStepName == KWriteStringStep)
		{
		testStep = new CWriteStringStep();
		}
	else if (aStepName == KAbortTestStep)
		{
		testStep = new CAbortTestStep();
		}
	else if (aStepName == KOOMTestStep)
		{
		testStep = new COOMTestStep();
		}
	else if (aStepName == KShortFunsTestStep)
		{
		testStep = new CShortFunsTestStep();
		}
	else if (aStepName == KMacrosTestStep)
		{
		testStep = new CTestMacrosTestStep();
		}
	else if (aStepName == KPanicTestStep)
		{
		testStep = new CPanicTestStep();
		}
	else if(aStepName == KLeaveForever)
		{
		testStep = new CLeaveForever();
		}

	return testStep;
	}
