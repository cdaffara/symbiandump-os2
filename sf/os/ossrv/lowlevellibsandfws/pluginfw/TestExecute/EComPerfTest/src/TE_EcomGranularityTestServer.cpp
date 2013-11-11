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
//

#include <test/testexecuteserverbase.h>
#include "TE_EcomGranularityTestStep.h" 

_LIT(KServerName, "TE_EcomGranularityTestServer");

class CEcomGranularityTestServer : public CTestServer
	{
public:
	static CEcomGranularityTestServer* NewL();
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	};

/**
Called inside the MainL() function to create and start the
CTestServer derived server.
*/
CEcomGranularityTestServer* CEcomGranularityTestServer::NewL()
	{
	CEcomGranularityTestServer* server = new (ELeave) CEcomGranularityTestServer();
	CleanupStack::PushL(server);
	// CServer base class call
	server->StartL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}
	
LOCAL_C void MainL()
	{
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().SecureApi(RProcess::ESecureApiOn);
	
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CEcomGranularityTestServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CEcomGranularityTestServer::NewL());
	if(KErrNone == err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;
	}

GLDEF_C TInt E32Main()
	{
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAPD(err,MainL());
	delete cleanup;
	return err;
    }

/**
@return - A CTestStep derived instance
Implementation of CTestServer pure virtual
*/
CTestStep* CEcomGranularityTestServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;

	if (aStepName == KEComImplIndexPerfTest)
		{
		testStep = new CEComImplIndexPerfTest();
		}
	return testStep;
	}
