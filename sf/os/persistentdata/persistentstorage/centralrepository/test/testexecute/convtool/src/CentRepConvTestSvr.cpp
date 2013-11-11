// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "CentRepConvTestSvr.h"
#include "ConvertStep.h"
#include "CmdLineStep.h"
#include "ReadModemRecordStep.h"

_LIT(KServerName, "CentRepConvToolTestSvr");

CCentRepConvTestSvr* CCentRepConvTestSvr::NewL()
	{
	CCentRepConvTestSvr* server = new (ELeave) CCentRepConvTestSvr();
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
	CCentRepConvTestSvr* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CCentRepConvTestSvr::NewL());
	if (KErrNone == err)
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

CTestStep* CCentRepConvTestSvr::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;

	if (aStepName == KConvertToBinStep)
		{
		testStep = new CConvertToBinaryStep();
		}
	else if (aStepName == KConvertToTextStep)
		{
		testStep = new CConvertToTextStep();
		}
	else if (aStepName == KRoundTripStep)
		{
		testStep = new CRoundTripStep();
		}
	else if (aStepName == KCmdLineTestStep)
		{
		testStep = new CCmdLineTestStep();
		}
	else if (aStepName == KReadModemRecordStep)
		{
		testStep = new CReadModemRecordStep();
		}
	else if (aStepName == KGenerateCreStep)
		{
		testStep = new CGenerateCreStep();
		}
		
	return testStep;
	}
	
