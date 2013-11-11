// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

/**
 @file texml2server.cpp
 @internalTechnology
*/

#include "texml2server.h"
#include "texml2teststep.h"
/**
 @return - Instance of the test server
 */
CXml2TestServer* CXml2TestServer::NewL()
	{
	CXml2TestServer * server = new (ELeave) CXml2TestServer();
	CleanupStack::PushL(server);
	server->ConstructL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}
/**
 * Standard 2 step constructor
 */
void CXml2TestServer::ConstructL(const TDesC& aName)
	{
	CTestServer::ConstructL(aName);
	}

LOCAL_C void MainL()
	{

	CActiveScheduler* sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CXml2TestServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CXml2TestServer::NewL());
	if (KErrNone == err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;
	}

/**
 * Entry point for the executable
 */
GLDEF_C TInt E32Main()
/**
 * @return - Standard Epoc error code on exit
 */
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if (cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAPD(err,MainL());
	delete cleanup;

	__UHEAP_MARKEND;
	return err;
	}

CTestStep* CXml2TestServer::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Implementation of CTestServer pure virtual.
 * Based on the test step, instantiates appropriate CTestStep dervied class
 */
	{
	CTestStep* testStep= NULL;

	if (aStepName == KXmlEngLeaveOOMStep1 || aStepName == KXmlEngLeaveOOMStep2)
		{
		testStep = new CXmlEngMemStep(aStepName);
		}
	else if (aStepName == KXmlEngXmlCharFromDes)
		{
		testStep = new CXmlUtilStep(aStepName);
		}
	else if (aStepName == KXmlEngEscapeForXmlValueStep1 || 
		 aStepName== KXmlEngEscapeForXmlValueStep2)
		{
		testStep = new CXmlUtilStep(aStepName);
		}
	else if (aStepName == KXmlEnginePushAndPop)
		{
		testStep = new CXmlEngineStep(aStepName);
		}
	else if (aStepName == KXmlStringCopy || 
			aStepName == KXmlStringSetStep1 || 
		 	aStepName == KXmlStringSetStep2 || 
			aStepName == KXmlStringAlloc1 || 
			aStepName == KXmlStringAlloc2 || 
			aStepName == KXmlStringAllocAndFree1 || 
			aStepName == KXmlStringAllocAndFree2 || 
			aStepName == KXmlStringAppend || 
			aStepName == KXmlStringCompare)
		{
		testStep = new CXmlStringStep(aStepName);
		}

	return testStep;
	}
