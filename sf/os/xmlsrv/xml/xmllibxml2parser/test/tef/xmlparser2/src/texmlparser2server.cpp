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
 @file texmlparser2server.cpp
 @internalTechnology
*/

#include "texmlparser2server.h"
#include "texmlparser2step.h"
/**
 @return - Instance of the test server
 */
CXmlParser2TestServer* CXmlParser2TestServer::NewL()
	{
	CXmlParser2TestServer * server = new (ELeave) CXmlParser2TestServer();
	CleanupStack::PushL(server);
	server->ConstructL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}
/**
 * Standard Symbian two phase constructor
 */
void CXmlParser2TestServer::ConstructL(const TDesC& aName)
	{
	CTestServer::ConstructL(aName);
	}

LOCAL_C void MainL()
	{

	CActiveScheduler* sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CXmlParser2TestServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CXmlParser2TestServer::NewL());
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
 * Entry point
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
	TRAPD(err,MainL())
	;
	delete cleanup;

	__UHEAP_MARKEND;
	return err;
	}

CTestStep* CXmlParser2TestServer::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep= NULL;

	if (aStepName == KXmlParser2SetContentSink || aStepName
			== KXmlParser2OOMTests || aStepName == KXmlParser2OOMTests2
			|| aStepName == KXmlParser2OOMTests3 ||aStepName == KXmlParser2OOMTests4)
		{
		testStep = new CXmlParser2Step(aStepName);
		}
	return testStep;
	}
