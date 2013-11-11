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
* This is the test class for TEF (PREQ10437).
*
*/



#include "tefpanictest.h"

CTEFPanicSomeThingStep::CTEFPanicSomeThingStep()
	{
	SetTestStepName(KTEFPanicSomeThingStep);
	}
TVerdict CTEFPanicSomeThingStep::doTestStepL()
	{
	User::Panic( _L("SomeThing"), 110);
	return TestStepResult();
	}
CTEFPanicEmptyStep::CTEFPanicEmptyStep()
	{
	SetTestStepName(KTEFPanicEmptyStep);
	}

TVerdict CTEFPanicEmptyStep::doTestStepL(void)
	{
	User::Panic( _L(""), 110);
	return TestStepResult();
	}

CTEFPanicDblSpaceStep::CTEFPanicDblSpaceStep()
	{
	SetTestStepName(KTEFPanicDblSpaceStep);
	}
TVerdict CTEFPanicDblSpaceStep::doTestStepL(void)
	{
	User::Panic( _L("a b"), 110);
	return TestStepResult();
	}
CTEFIgnoreStep::CTEFIgnoreStep()
	{
	SetTestStepName(KTEFIgnoreStep);
	}
TVerdict CTEFIgnoreStep::doTestStepL(void)
	{
	SetTestStepResult(EIgnore);
	return TestStepResult();
	}

CTEFPassStep::CTEFPassStep()
	{
	SetTestStepName(KTEFPassStep);
	}
TVerdict CTEFPassStep::doTestStepL(void)
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

CTEFPanicBugServer* CTEFPanicBugServer::NewL()
/**
 @return - Instance of the test server
 Same code for Secure and non-secure variants
 Called inside the MainL() function to create and start the
 CTestServer derived server.
 */
	{
	CTEFPanicBugServer * server = new (ELeave) CTEFPanicBugServer();
	CleanupStack::PushL(server);

	server->StartL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}

CTEFPanicBugStep::CTEFPanicBugStep()
	{
	SetTestStepName(KTEFPanicBugStep);
	}
TVerdict CTEFPanicBugStep::doTestStepL()
	{
	User::Panic( _L("123"), 110);
	return TestStepResult();
	}


LOCAL_C void MainL()
//
// Secure variant
// Much simpler, uses the new Rendezvous() call to sync with the client
//

	{
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().SecureApi(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CTEFPanicBugServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTEFPanicBugServer::NewL());
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

/** @return - Standard Epoc error code on process exit
 Secure variant only
 Process entry point. Called by client using RProcess API
 */
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if (cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAPD(err,MainL())
	;
	// This if statement is here just to shut up RVCT, which would otherwise warn
	// that err was set but never used
	if (err)
		{
		err = KErrNone;
		}
	delete cleanup;
	__UHEAP_MARKEND;
	return KErrNone;
	}

CTestStep* CTEFPanicBugServer::CreateTestStep(const TDesC& aStepName)
/**
 @return - A CTestStep derived instance
 Secure and non-secure variants
 Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep= NULL;
	if (aStepName == KTEFPanicBugStep)
		{
		testStep = new CTEFPanicBugStep();
		}
	else if (aStepName == KTEFIgnoreStep)
		{
		testStep = new CTEFIgnoreStep();
		}
	else if (aStepName == KTEFPassStep)
		{
		testStep = new CTEFPassStep();
		}
	else if (aStepName == KTEFPanicSomeThingStep)
		{
		testStep = new CTEFPanicSomeThingStep();
		}
	else if (aStepName == KTEFPanicEmptyStep)
		{
		testStep = new CTEFPanicEmptyStep();
		}
	else  if (aStepName == KTEFPanicDblSpaceStep)
		{
		testStep = new CTEFPanicDblSpaceStep();
		}
	return testStep;
	}

