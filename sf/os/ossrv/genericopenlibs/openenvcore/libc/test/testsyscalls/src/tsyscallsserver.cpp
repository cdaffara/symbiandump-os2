/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/




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
 * Pipe test server implementation
 */
#include "tsyscallsserver.h"
#include "tsyscalls.h"


_LIT(KServerName, "tsyscalls");

CSyscallsTestServer* CSyscallsTestServer::NewL()
	{
	CSyscallsTestServer *server = new(ELeave) CSyscallsTestServer();
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
	CSyscallsTestServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err, server = CSyscallsTestServer::NewL());
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

CTestStep* CSyscallsTestServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;

	// This server creates just one step but create as many as you want
	// They are created "just in time" when the worker thread is created
	// install steps

	if(aStepName == KCreat)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
   	if(aStepName == Kopen1)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
   	if(aStepName == Kopen2)
		{
		testStep = new CTestSyscalls(aStepName); 
		} 
   	if(aStepName == Kopen3)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
   	if(aStepName == Kopen4)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
   	if(aStepName == Kopen5)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
   	if(aStepName == Kopen6)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
   	if(aStepName == KOpenTruncate1)
   		{
   		testStep = new CTestSyscalls(aStepName); 
   		}
   	if(aStepName == KOpenTruncate2)
   	   	{
   	   	testStep = new CTestSyscalls(aStepName); 
   	   	}
   	if(aStepName == Kopen7)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
	if(aStepName == KOpenInAppendMode)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
 
   	if(aStepName == Kwrite1)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
   	if(aStepName == Kwrite2)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
   	if(aStepName == Kwrite3)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
   	if(aStepName == Kwrite5)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
 
   	if(aStepName == Kread1)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
   	if(aStepName == Kread2)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
   	if(aStepName == Kread3)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
   	if(aStepName == Kread4)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
 
   	if(aStepName == KOpendir)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
   	if(aStepName == KClosedir)
		{
		testStep = new CTestSyscalls(aStepName); 
		}

   	if(aStepName == KReaddir)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
   	if(aStepName == KLseek)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
   	if(aStepName == KLseek1)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
   	if(aStepName == KAccess)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
   	if(aStepName == KAccess1)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
   	if(aStepName == KDup)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
   	if(aStepName == KDup2)
		{
		testStep = new CTestSyscalls(aStepName); 
		}

   
   	if(aStepName == KRename)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
   	if(aStepName == KRename1)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
   
   	if(aStepName == KChmod)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
   	if(aStepName == KChmod1)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
   	if(aStepName == KChmod_dir)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
   	if(aStepName == KFChmod)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
   	if(aStepName == KFChmod_dir)
		{
		testStep = new CTestSyscalls(aStepName); 
		}

   	if(aStepName == KExit)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
   	if(aStepName == KClose)
		{
		testStep = new CTestSyscalls(aStepName); 
		}

   	if(aStepName == KMkdir)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
   	if(aStepName == KMk_dir)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
		
   	if(aStepName == KRmdir)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
   	if(aStepName == KRm_dir)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
   	if(aStepName == KRmdir1)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
   	if(aStepName == KRmdir_Chdir)
		{
		testStep = new CTestSyscalls(aStepName); 
		}

   	if(aStepName == KFsync)
		{
		testStep = new CTestSyscalls(aStepName); 
		}

   	if(aStepName == KUtimes)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
   	if(aStepName == KUtime)
		{
		testStep = new CTestSyscalls(aStepName); 
		}

   	if(aStepName == KChdir)
		{
		testStep = new CTestSyscalls(aStepName); 
		}

   	if(aStepName == KFcntl)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
   	if(aStepName == KIoctl)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
   	if(aStepName == KFstat)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
   	if(aStepName == KStat)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
   	if(aStepName == KStat1)
		{
		testStep = new CTestSyscalls(aStepName); 
		} 	
   	if(aStepName == KStat2)
		{
		testStep = new CTestSyscalls(aStepName); 
		} 
	if(aStepName == KStat3)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
   	if(aStepName == KGetpid)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
   	if(aStepName == KClock)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
   	if(aStepName == KTime)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
   	if(aStepName == KWaitPid)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
   	if(aStepName == KReadV)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
   	if(aStepName == KWriteV)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
   	if(aStepName == KSleep)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
   	if(aStepName == KSeekDir)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
   	if(aStepName == KRewindDir)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
   	if(aStepName == KTelldir)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
	if(aStepName == KTestClock)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
	if(aStepName == KCreat2)	
		{
		testStep = new CTestSyscalls(aStepName); 
		}
	if(aStepName == Kopen8)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
	if (aStepName == KTestStat)
		{
		testStep = new CTestSyscalls(aStepName);
		}
	if(aStepName == KLseekttytest1)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
	if(aStepName == KLseekttytest2)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
	if(aStepName == KWaitPidtest)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
	if(aStepName == KWaittest)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
	if(aStepName == KOpen_FileDes_Test)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
	if(aStepName == Kopenuid)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
	if(aStepName == KMkdir1)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
	if(aStepName == KMkdir2)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
	if(aStepName == KRename2)
		{	
		testStep = new CTestSyscalls(aStepName); 
		}
	if(aStepName == Ktestfsync)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
	if(aStepName == Ktestrename)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
	if(aStepName == Ktestopenvalidate)
		{
		testStep = new CTestSyscalls(aStepName); 
		}	
	if(aStepName == Ksync_safe)
		{
		testStep = new CTestSyscalls(aStepName); 
		}	
	if(aStepName == KFstat1)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
	if(aStepName == KUtimes1)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
	if(aStepName == KMkdir_test1)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
	if(aStepName == KChmod_test)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
	if(aStepName == KChdir1)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
	if(aStepName == KRmdir2)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
	if(aStepName == KRename_test)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
   	if(aStepName == KRename3)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
	if(aStepName == KCreat1)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
	if(aStepName == KReadV1)
		{
		testStep = new CTestSyscalls(aStepName); 
		}
 	if(aStepName == KUtimes2)
 		{
 		testStep = new CTestSyscalls(aStepName); 
 		}
 	if(aStepName == KStat_test)
 		{
 		testStep = new CTestSyscalls(aStepName); 
 		}
 	if(aStepName == KMkdir_test2)
 		{
 		testStep = new CTestSyscalls(aStepName); 
 		}
 	if(aStepName == KChmod2)
 		{
 		testStep = new CTestSyscalls(aStepName); 
 		}
 	if(aStepName == KChdir2)
 		{
 		testStep = new CTestSyscalls(aStepName); 
 		}
 	if(aStepName == KRename4)
 		{
 		testStep = new CTestSyscalls(aStepName); 
 		}
 	if(aStepName == KRename5)
 		{
 		testStep = new CTestSyscalls(aStepName); 
 		}
 	if(aStepName == KRmdir3)
 		{
 		testStep = new CTestSyscalls(aStepName); 
 		}
 	if(aStepName == Kread5)
 		{
 		testStep = new CTestSyscalls(aStepName); 
 		}
	return testStep;
	}

