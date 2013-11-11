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
// file te_ezlibezipserver.cpp
// 
//

#include "te_ezlibezipserver.h"
#include "te_performancetests.h"
#include "te_backwardscompatibilitytests.h"

_LIT(KServerName, "Te_EZlibEZip");

CEZipEZipServer *CEZipEZipServer::NewL()
	{
	CEZipEZipServer *server = new(ELeave) CEZipEZipServer();
	CleanupStack::PushL(server);

	server->ConstructL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}

LOCAL_C void MainL()
	{
	// Leave the hooks in for platform security
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().DataCaging(RProcess::ESecureApiOn);
#endif
	CActiveScheduler *sched = NULL;
	sched = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CEZipEZipServer *server = NULL;
	
	// Create the CTestServer derived server
	TRAPD(err, server = CEZipEZipServer::NewL());
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
	CTrapCleanup *cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAPD(err,MainL());
	delete cleanup;
	__UHEAP_MARKEND;
	return err;
    }

CTestStep *CEZipEZipServer::CreateTestStep(const TDesC &aStepName)
	{
	CTestStep *testStep = NULL;
	
	if(aStepName == KTestDeflatePerformanceL)
		{
		testStep = new CPerformanceTests(aStepName);
		}
	else if(aStepName == KTestInflatePerformanceL)
		{
		testStep = new CPerformanceTests(aStepName);
		}
	else if(aStepName == KTestGZipDeflatePerformanceL)
		{
		testStep = new CPerformanceTests(aStepName);
		}
	else if(aStepName == KTestGZipInflatePerformanceL)
		{
		testStep = new CPerformanceTests(aStepName);
		}
	else if(aStepName == KTestZipArchiveDecompressPerformanceL)
		{
		testStep = new CPerformanceTests(aStepName);
		}
	else if(aStepName == KTestDefOldInfNewCompatibilityL)
		{
		testStep = new CBackwardsCompatibilityTests(aStepName);
		}
	else if(aStepName == KTestDefNewInfOldCompatibilityL)
		{
		testStep = new CBackwardsCompatibilityTests(aStepName);
		}
	else if(aStepName == KTestGZipDefOldInfNewCompatibilityL)
		{
		testStep = new CBackwardsCompatibilityTests(aStepName);
		}
	else if(aStepName == KTestGZipDefNewInfOldCompatibilityL)
		{
		testStep = new CBackwardsCompatibilityTests(aStepName);
		}
	else if(aStepName == KTestZipArchiveDecompressCompatibilityL)
		{
		testStep = new CBackwardsCompatibilityTests(aStepName);
		}
	else if(aStepName == KTestOSCompatibilityL)
		{
		testStep = new CBackwardsCompatibilityTests(aStepName);
		}
	else if(aStepName == KTestOSGZipCompatibilityL)
			{
			testStep = new CBackwardsCompatibilityTests(aStepName);
			}

	return testStep;
	}
