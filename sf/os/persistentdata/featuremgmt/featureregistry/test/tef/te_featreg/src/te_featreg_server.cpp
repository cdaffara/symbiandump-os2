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
// Feature Registry TEF test server.
// 
//

/**
 @file
 @internalComponent
 @test
*/

#include "te_featreg_server.h"
#include "tc_featreg_steps.h"

_LIT(KServerName,"te_featreg");

CTe_FeatReg_Server* CTe_FeatReg_Server::NewL()
	{
	CTe_FeatReg_Server * server = new (ELeave) CTe_FeatReg_Server();
	CleanupStack::PushL(server);

	// CServer base class call which can be either StartL or ConstructL,
	// the later will permit Server Logging.
	server->ConstructL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}

/**
 * @return - A CTestStep derived instance
 * Implementation of CTestServer pure virtual
 */
CTestStep* CTe_FeatReg_Server::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;
	// They are created "just in time" when the worker thread is created
	// More test steps can be added below 
	if (aStepName == KQueryAnyFeatureStep)
		{
		testStep = new CQueryAnyFeatureStep();
		}
	else if (aStepName == KRunFeatRegSetupCorruptStep)
		{
		testStep = new CRunFeatRegSetupCorruptStep();
		}
	else if (aStepName == KQueryAnyFeatureCorruptStep)
		{
		testStep = new CQueryAnyFeatureCorruptStep();
		}
	else if (aStepName == KQueryFeaturesNotSupportedStep)
		{
		testStep = new CQueryFeaturesNotSupportedStep();
		}
	else if (aStepName == KQueryFeaturesValidSmallStep)
		{
		testStep = new CQueryFeaturesValidSmallStep();
		}
	else if (aStepName == KQueryFeaturesValidLargeStep)
		{
		testStep = new CQueryFeaturesValidLargeStep();
		}
	else if (aStepName == KInvalidUseQueryNotOpenStep)
		{
		testStep = new CInvalidUseQueryNotOpenStep();
		}
	else if (aStepName == KFeatRegMeasPerformanceStep)
		{
		testStep = new CFeatRegMeasPerformanceStep();
		}
#ifndef SYMBIAN_FEATURE_MANAGER       
//test steps not applicable for feature manager wrapper
	else if (aStepName == KFeatRegNotifyStep)
		{
		testStep = new CFeatRegNotifyStep();
		}
	else if (aStepName == KInvalidUseSubscribeNotOpenStep)
		{
		testStep = new CInvalidUseSubscribeNotOpenStep();
		}
	else if (aStepName == KInvalidUseCancelNotOpenStep)
		{
		testStep = new CInvalidUseCancelNotOpenStep();
		}
	else if (aStepName == KFeatRegOpenCloseStep)
		{
		testStep = new CFeatRegOpenCloseStep();
		}
#endif// SYMBIAN_FEATURE_MANAGER       
	// else if (aStepName == ...) { }

	return testStep;
	}

static void MainL()
	{
#if (defined __DATA_CAGING__)
	// Leave the hooks in for platform security
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().DataCaging(RProcess::ESecureApiOn);
#endif

	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CTe_FeatReg_Server* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTe_FeatReg_Server::NewL());
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
	{
	__UHEAP_MARK;

	CTrapCleanup* cleanup = CTrapCleanup::New();
	TInt result = KErrNoMemory;
	if (cleanup != NULL)
		{
		TRAP(result, MainL());
		delete cleanup;
		}

	__UHEAP_MARKEND;

	return result;
	}
