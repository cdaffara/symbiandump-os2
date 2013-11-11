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
#include "ttime_blrserver.h"
#include "ttime_blr.h"


_LIT(KServerName, "ttime_blr");

CTimeBlrTestServer* CTimeBlrTestServer::NewL()
	{
	CTimeBlrTestServer *server = new(ELeave) CTimeBlrTestServer();
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
	CTimeBlrTestServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err, server = CTimeBlrTestServer::NewL());
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

CTestStep* CTimeBlrTestServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;

	// This server creates just one step but create as many as you want
	// They are created "just in time" when the worker thread is created
	// install steps
	if(aStepName == Kasctime_test_all)
		{
		testStep = new CTestTimeBlr(aStepName);
		}
	if(aStepName == Kasctime_test_null)
		{
		testStep = new CTestTimeBlr(aStepName);
		}
	if(aStepName == Klocaltime_test_all)
		{
		testStep = new CTestTimeBlr(aStepName);
		}
    if(aStepName == Kgmtime_test_all)
		{
		testStep = new CTestTimeBlr(aStepName);
		}
    if(aStepName == Kmktime_test_null)
		{
		testStep = new CTestTimeBlr(aStepName);
		}
    if(aStepName == Kmktime_test_epoch)
		{
		testStep = new CTestTimeBlr(aStepName);
		}
    if(aStepName == Kmktime_test_extreme)
		{
		testStep = new CTestTimeBlr(aStepName);
		}
    if(aStepName == Kctime_test_all)
		{
		testStep = new CTestTimeBlr(aStepName);
		}
    if(aStepName == Kdifftime_test_all)
		{
		testStep = new CTestTimeBlr(aStepName);
		}
    if(aStepName == Kdifftime_test_extreme)
		{
		testStep = new CTestTimeBlr(aStepName);
		}
    if(aStepName == Ktzset_test)
		{
		testStep = new CTestTimeBlr(aStepName);
		}
    if(aStepName == Kasctime_r_all)
		{
		testStep = new CTestTimeBlr(aStepName);
		}
    if(aStepName == Kasctime_r_null)
		{
		testStep = new CTestTimeBlr(aStepName);
		}
    if(aStepName == Klocaltime_r_all)
		{
		testStep = new CTestTimeBlr(aStepName);
		}
    if(aStepName == Kgmtime_r_all)
		{
		testStep = new CTestTimeBlr(aStepName);
		}
    if(aStepName == Kctime_r_all)
		{
		testStep = new CTestTimeBlr(aStepName);
		}
    if(aStepName == Kmultithread_asctime_r)
		{
		testStep = new CTestTimeBlr(aStepName);
		}
    if(aStepName == Kmultithread_localtime_r)
		{
		testStep = new CTestTimeBlr(aStepName);
		}
	if(aStepName == Kmultithread_ctime_r)
		{
		testStep = new CTestTimeBlr(aStepName);
		}      
    if(aStepName == Kmultithread_gmtime_r)
		{
		testStep = new CTestTimeBlr(aStepName);
		}
    if(aStepName == Kclock_getres_pos_test)
		{
		testStep = new CTestTimeBlr(aStepName);
		}
    if(aStepName == Kclock_gettime_pos_test)
		{
		testStep = new CTestTimeBlr(aStepName);
		}
    if(aStepName == Kclock_settime_pos_test)
		{
		testStep = new CTestTimeBlr(aStepName);
		}
    if(aStepName == Kclock_getcpuid_pos_test)
		{
		testStep = new CTestTimeBlr(aStepName);
		}
    if(aStepName == Kclock_getres_neg_test)
		{
		testStep = new CTestTimeBlr(aStepName);
		}
    if(aStepName == Kclock_gettime_neg_test)
		{
		testStep = new CTestTimeBlr(aStepName);
		}
    if(aStepName == Kclock_settime_neg_test)
		{
		testStep = new CTestTimeBlr(aStepName);
		}
    if(aStepName == Kclock_getcpuid_neg_test)
		{
		testStep = new CTestTimeBlr(aStepName);
		}
	if(aStepName == Kadjtime_pos_test)
		{
		testStep = new CTestTimeBlr(aStepName);
		}
	if(aStepName == Kadjtime_neg_test)
		{
		testStep = new CTestTimeBlr(aStepName);
		}
	if(aStepName == Kgetoffset_test)
		{
		testStep = new CTestTimeBlr(aStepName);
		}
	if(aStepName == Kgetrule_test)
		{
		testStep = new CTestTimeBlr(aStepName);
		}
	if(aStepName == Ktzset1)
		{
		testStep = new CTestTimeBlr(aStepName);
		}
	if(aStepName == Ktimegm_Test)
		{
		testStep = new CTestTimeBlr(aStepName);
		}
	if(aStepName == Kclock_getres_null_test)
		{
		testStep = new CTestTimeBlr(aStepName);
		}
	if(aStepName == Kadjtime_null_test)
		{
		testStep = new CTestTimeBlr(aStepName);
		}
	if(aStepName == Kadjtime_range_test)
		{
		testStep = new CTestTimeBlr(aStepName);
		}
	if(aStepName == Kclock_gettime_null_test)
		{
		testStep = new CTestTimeBlr(aStepName);
		}
	if(aStepName == Kclock_settime_null_test)
		{
		testStep = new CTestTimeBlr(aStepName);
		}
	if(aStepName == Kclock_settime_range1_test)
		{
		testStep = new CTestTimeBlr(aStepName);
		}
	if(aStepName == Kclock_settime_range2_test)
		{
		testStep = new CTestTimeBlr(aStepName);
		}
	if(aStepName == Kmktime_test_extreme1)
		{
		testStep = new CTestTimeBlr(aStepName);
		}
	if(aStepName == KtimeTestWithTzset)
	    {
	    testStep = new CTestTimeBlr(aStepName);
	    }
	if(aStepName == Klocal_and_mktime)
		{
		testStep = new CTestTimeBlr(aStepName);
		}
	return testStep;
	}



