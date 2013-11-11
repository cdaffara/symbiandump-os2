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
//  Name        : tlargefileeserver.cpp


#include <c32comm.h>

#if defined (__WINS__)
#define PDD_NAME		_L("ECDRV")
#else
#define PDD_NAME		_L("EUART1")
#define PDD2_NAME		_L("EUART2")
#define PDD3_NAME		_L("EUART3")
#define PDD4_NAME		_L("EUART4")
#endif

#define LDD_NAME		_L("ECOMM")


#include "tlargefileserver.h"
#include "tlargefile.h"


_LIT(KServerName, "tlargefile");

CLargefileTestServer* CLargefileTestServer::NewL()
	{
	CLargefileTestServer *server = new(ELeave) CLargefileTestServer();
	CleanupStack::PushL(server);
	server->ConstructL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}

static void InitCommsL()
    {
    TInt ret = User::LoadPhysicalDevice(PDD_NAME);
    User::LeaveIfError(ret == KErrAlreadyExists?KErrNone:ret);

#ifndef __WINS__
    ret = User::LoadPhysicalDevice(PDD2_NAME);
    ret = User::LoadPhysicalDevice(PDD3_NAME);
    ret = User::LoadPhysicalDevice(PDD4_NAME);
#endif

    ret = User::LoadLogicalDevice(LDD_NAME);
    User::LeaveIfError(ret == KErrAlreadyExists?KErrNone:ret);
    ret = StartC32();
    User::LeaveIfError(ret == KErrAlreadyExists?KErrNone:ret);
    }

LOCAL_C void MainL()
	{
	// Leave the hooks in for platform security
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().SecureApi(RProcess::ESecureApiOn);
#endif
	//InitCommsL();
	
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CLargefileTestServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err, server = CLargefileTestServer::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;
	}

/**
 * Server entry point
 * @return Standard Epoc error code on exit
 */
TInt main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL) 
		{
		return KErrNoMemory;  
		}
	TRAP_IGNORE(MainL());
	delete cleanup;
	__UHEAP_MARKEND;
	
	return KErrNone;
	}

CTestStep* CLargefileTestServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;
    if(aStepName == KTestLargeFileTruncate)
		{
		testStep = new CTestLargefile(aStepName);
		}
    else if(aStepName == KTestLargeFileFtruncate)
		{
		testStep = new CTestLargefile(aStepName);
		}
    else if(aStepName == KTestLargeFileReadWrite)
		{
		testStep = new CTestLargefile(aStepName);
		}
    else if(aStepName == KTestLargeFileFseeknTell)
		{
		testStep = new CTestLargefile(aStepName);
		}
    else if(aStepName == KTestLargeFileLseeknTell)
		{
		testStep = new CTestLargefile(aStepName);
		}
    else if(aStepName == KTestCreatenFseeknPos)
		{
		testStep = new CTestLargefile(aStepName);
		}
    else if(aStepName == KTestCreatenLseeknPos)
		{
		testStep = new CTestLargefile(aStepName);
		}
    else if(aStepName == KTestChildFileTransfer)
		{
		testStep = new CTestLargefile(aStepName);
		}
    else if(aStepName == KTestLargeFileFtw)
		{
		testStep = new CTestLargefile(aStepName);
		}
    else if(aStepName == KTestLargeFileReaddir)
		{
		testStep = new CTestLargefile(aStepName);
		}
    else if(aStepName == KTestLargeFileWreaddir)
		{
		testStep = new CTestLargefile(aStepName);
		}
    else if(aStepName == KTestLargeFileAlphasort)
		{
		testStep = new CTestLargefile(aStepName);
		}
    else if(aStepName == KTestNonsortScandir)
		{
		testStep = new CTestLargefile(aStepName);
		}
    else if(aStepName == KTestSortScandir)
		{
		testStep = new CTestLargefile(aStepName);
		}
    else if(aStepName == KTestFcntlwithSinglefd)
		{
		testStep = new CTestLargefile(aStepName);
		}
    else if(aStepName == KTestFcntlwithDoublefds)
		{
		testStep = new CTestLargefile(aStepName);
		}
    else if(aStepName == KTestLargeFileSeek)
		{
		testStep = new CTestLargefile(aStepName);
		}
    else if(aStepName == KTestLargeFileGlob)
		{
		testStep = new CTestLargefile(aStepName);
		}
    else if(aStepName == KTestLargeFileMmapWrite)
		{
		testStep = new CTestLargefile(aStepName);
		}
    else if(aStepName == KTestLargeFileWriteMmap)
		{
		testStep = new CTestLargefile(aStepName);
		}
    else if(aStepName == KTestNegSmallOffsetValues)
		{
		testStep = new CTestLargefile(aStepName);
		}
    else if(aStepName == KTestNegLargeOffsetValues)
		{
		testStep = new CTestLargefile(aStepName);
		}
    else if(aStepName == KTestLargeFileFreopen)
		{
		testStep = new CTestLargefile(aStepName);
		}
    else if(aStepName == KTestLargeFileWfreopen)
		{
		testStep = new CTestLargefile(aStepName);
		}
    else if(aStepName == KTestFcntlWriteLockwithMaxSize)
        {
        testStep = new CTestLargefile(aStepName);
        }

	return testStep;
	}

