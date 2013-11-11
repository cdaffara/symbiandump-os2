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
// Name        : tmiscserver.cpp
// 
//

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


/**
 * @file
 *
 * random test server implementation
 */
#include "tmiscserver.h"
#include "tmisc.h"


_LIT(KSerName, "tmisc");

CMiscTestServer* CMiscTestServer::NewL()
	{
	CMiscTestServer *server = new(ELeave) CMiscTestServer();
	CleanupStack::PushL(server);
	server->ConstructL(KSerName);
	CleanupStack::Pop(server);
	return server;
	}

static void InitCommsL()
    {
    
   TInt err=0;
    err = User::LoadPhysicalDevice (PDD_NAME); 
    if ( err != KErrAlreadyExists && err)
    	User::Leave(err); 
  	err = User::LoadLogicalDevice (LDD_NAME);
  	if ( err != KErrAlreadyExists && err)
    	User::Leave(err);
  	StartC32();
  	
  	RCommServ ComSer;
  	ComSer.Connect();
  		
  	err=ComSer.LoadCommModule(_L("ECUART"));	 	
    if ( err != KErrAlreadyExists && err)
    	User::Leave(err);  
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
	CMiscTestServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err, server = CMiscTestServer::NewL());
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
	
	TRAP_IGNORE(MainL());
	
	__UHEAP_MARKEND;
	return KErrNone;
	}

CTestStep* CMiscTestServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;

	// This server creates just one step but create as many as you want
	// They are created "just in time" when the worker thread is created
	// install steps
	if(aStepName == Ktest_random)
		{
		testStep = new CTestMisc(aStepName);
		}
	else if(aStepName == Ktest_nanosleep)
		{
		testStep = new CTestMisc(aStepName);
		}
	else if(aStepName == Ktest_fileoper)
		{
		testStep = new CTestMisc(aStepName);
		}
	else if(aStepName == Ktest_realloc)
		{
		testStep = new CTestMisc(aStepName);
		}
	else if(aStepName == Ktest_stdio)
		{
		testStep = new CTestMisc(aStepName);
		}
	else if(aStepName == KTestSysConf)
		{
		testStep = new CTestMisc(aStepName);
		}
	else if(aStepName == KTestScandir)
		{
		testStep = new CTestMisc(aStepName);
		}
	else if(aStepName == KTestReaddir)
		{
		testStep = new CTestMisc(aStepName);
		}
	else if(aStepName == KTestDiropr)
		{
		testStep = new CTestMisc(aStepName);
		}
	else if(aStepName == KTestWOpendir)
		{
		testStep = new CTestMisc(aStepName);
		}
	else if(aStepName == KTestWDiropr)
		{
		testStep = new CTestMisc(aStepName);
		}
	else if(aStepName == KTestFMScalls)
		{
		testStep = new CTestMisc(aStepName);
		}
	else if(aStepName == KTestGetDirEntries)
		{
		testStep = new CTestMisc(aStepName);
		}
	else if(aStepName == KTestAlphasort)
		{
		testStep = new CTestMisc(aStepName);
		}
	else if(aStepName == KTestPosixSpawn)
		{
		testStep = new CTestMisc(aStepName);
		}
	else if(aStepName == KTestPosixSpawn1)
		{
		testStep = new CTestMisc(aStepName);
		}			
	else if(aStepName == KTestSystem0)
		{
		testStep = new CTestMisc(aStepName);
		}
	else if(aStepName == KTestSystem1)
		{
		testStep = new CTestMisc(aStepName);
		}
	else if(aStepName == KTestSystem2)
		{
		testStep = new CTestMisc(aStepName);
		}		
	else if(aStepName == KTestAbort1)
		{
		testStep = new CTestMisc(aStepName);
		}
	else if(aStepName == KTestExit1)
		{
		testStep = new CTestMisc(aStepName);
		}
	else if(aStepName == KTestExit2)
		{
		testStep = new CTestMisc(aStepName);
		}
	else if(aStepName == KTestExitE1)
		{
		testStep = new CTestMisc(aStepName);
		}	
	else if(aStepName == KTestExit3)
		{
		testStep = new CTestMisc(aStepName);
		}
	else if(aStepName == KTestExit4)
		{
		testStep = new CTestMisc(aStepName);
		}
	else if(aStepName == KTestExit5)
		{
		testStep = new CTestMisc(aStepName);
		}
	else if(aStepName == KTestExit6)
		{
		testStep = new CTestMisc(aStepName);
		}		
	else if(aStepName == KTestWaitpid1)
		{
		testStep = new CTestMisc(aStepName);
		}
	else if(aStepName == KTestWaitpid2)
		{
		testStep = new CTestMisc(aStepName);
		}	
	else if(aStepName == KTestWaitpid3)
		{
		testStep = new CTestMisc(aStepName);
		}
	else if(aStepName == Ksemoptest16)
		{
		testStep = new CTestMisc(aStepName);
		}
	else if(aStepName == Ksemoptest17)
		{
		testStep = new CTestMisc(aStepName);
		}
	else if(aStepName == Ksemoptest18)
		{
		testStep = new CTestMisc(aStepName);
		}
	else if(aStepName == Ksemoptest19)
		{
		testStep = new CTestMisc(aStepName);
		}
	else if(aStepName == Ksemoptest20)
		{
		testStep = new CTestMisc(aStepName);
		}
	else if(aStepName == KTestMaxFopen)
		{
		testStep = new CTestMisc(aStepName);
		}
	else if(aStepName == KTestPopen3FdTransfer)
		{
			testStep = new CTestMisc(aStepName);
		}
	else if(aStepName == KTestPopenFdTransfer)
		{
			testStep = new CTestMisc(aStepName);
		}
	else if(aStepName == KTestSystemFdTransfer)
		{
			testStep = new CTestMisc(aStepName);
		}
	else if(aStepName == KTestPosixSpawnFdTransfer)
		{
			testStep = new CTestMisc(aStepName);
		}
	else if(aStepName == KTestPopen3PipeReadTransfer)
		{
			testStep = new CTestMisc(aStepName);
		}
	else if(aStepName == KTestPopen3PipeWriteTransfer)
		{
			testStep = new CTestMisc(aStepName);
		}
	else if(aStepName == KTestPopenPipeReadTransfer)
		{
		testStep = new CTestMisc(aStepName);
		}	
	else if(aStepName == KTestPopenPipeWriteTransfer)
		{
			testStep = new CTestMisc(aStepName);
		}
	else if(aStepName == KTestFileReadWrite)
		{
		testStep = new CTestMisc(aStepName);
		}
	else if(aStepName == KTestPipeReadWrite)
		{
		testStep = new CTestMisc(aStepName);
		}
	else if(aStepName == KTestTzname)
		{
		testStep = new CTestMisc(aStepName);
		}
  	else if(aStepName == KSysConfTest)
  		{
  		testStep = new CTestMisc(aStepName);
  		}
  	else if (aStepName == Ktestclock)
  		{
  		testStep = new CTestMisc(aStepName);
  		}
	else if(aStepName == KTestSysConf_Negative)
		{
		testStep = new CTestMisc(aStepName);
		}
	else if(aStepName == KTestOpenWithExclusiveLock)
		{
		testStep = new CTestMisc(aStepName);
		}	
	return testStep;
	}

