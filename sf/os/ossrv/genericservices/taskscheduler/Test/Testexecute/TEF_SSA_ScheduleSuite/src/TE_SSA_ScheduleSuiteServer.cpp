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

#include "TE_SSA_ScheduleSuiteServer.h"
#include "INC072424_step.h"

_LIT(KServerName, "TEF_SSA_ScheduleSuite");

CSSA_ScheduleSuiteSvr* CSSA_ScheduleSuiteSvr::NewL()
	{
	CSSA_ScheduleSuiteSvr* server = new (ELeave) CSSA_ScheduleSuiteSvr();
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
	CSSA_ScheduleSuiteSvr* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CSSA_ScheduleSuiteSvr::NewL());
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

CTestStep* CSSA_ScheduleSuiteSvr::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;

	if (aStepName == KSSA_StateTransitionStep)
		{
		testStep = new CSSA_StateTransitionStep();
		}
	else if (aStepName == KSSA_PersistentScheduleStep)
		{
		testStep = new CSSA_PersistentScheduleStep();
		}
	else if (aStepName == KSSA_ConditionScheduleStep)
		{
		testStep = new CSSA_ConditionScheduleStep();
		}
	else if (aStepName == KSSA_EnvironmentChangeStep)
		{
		testStep = new CSSA_EnvironmentChangeStep();
		}
	else if (aStepName == KSSA_EditTransientStep)
		{
		testStep = new CSSA_EditTransientStep();
		}
	else if (aStepName == KSSA_EditPersistentStep)
		{
		testStep = new CSSA_EditPersistentStep();
		}
	else if (aStepName == KSSA_EditConditionStep)
		{
		testStep = new CSSA_EditConditionStep();
		}

	return testStep;
	}
