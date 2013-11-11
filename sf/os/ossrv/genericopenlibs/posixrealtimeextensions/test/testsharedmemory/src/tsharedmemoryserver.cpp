// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : tsharedmemoryserver.cpp
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
 * Pipe test server implementation
 */
#include "tsharedmemoryserver.h"
#include "tsharedmemory.h"


_LIT(KServerName, "tsharedmemory");

CSharedmemoryTestServer* CSharedmemoryTestServer::NewL()
	{
	CSharedmemoryTestServer *server = new(ELeave) CSharedmemoryTestServer();
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
	CSharedmemoryTestServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err, server = CSharedmemoryTestServer::NewL());
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

CTestStep* CSharedmemoryTestServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;

	if(aStepName == KTestsharedmemory1)
		{
			testStep = new CTestsharedmemory(aStepName);
		}
	if(aStepName == KTestsharedmemory2)
		{
			testStep = new CTestsharedmemory(aStepName);
		}
	if(aStepName == KTestsharedmemory3)
		{
			testStep = new CTestsharedmemory(aStepName);
		}
	if(aStepName == KTestsharedmemory4)
		{
			testStep = new CTestsharedmemory(aStepName);
		}
	if(aStepName == KTestsharedmemory5)
		{
			testStep = new CTestsharedmemory(aStepName);
		}
	if(aStepName == KTestsharedmemory6)
		{
			testStep = new CTestsharedmemory(aStepName);
		}
	if(aStepName == KTestsharedmemory7)
		{
			testStep = new CTestsharedmemory(aStepName);
		}
	if(aStepName == KTestsharedmemory8)
		{
			testStep = new CTestsharedmemory(aStepName);
		}
	if(aStepName == KTestsharedmemory9)
		{
			testStep = new CTestsharedmemory(aStepName);
		}
	if(aStepName == KTestsharedmemory10)
		{
			testStep = new CTestsharedmemory(aStepName);
		}
	if(aStepName == KTestsharedmemory11)
		{
			testStep = new CTestsharedmemory(aStepName);
		}
	if(aStepName == KTestsharedmemory12)
		{
			testStep = new CTestsharedmemory(aStepName);
		}
	if(aStepName == KTestsharedmemory13)
		{
			testStep = new CTestsharedmemory(aStepName);
		}
	if(aStepName == KTestsharedmemory14)
		{
			testStep = new CTestsharedmemory(aStepName);
		}
	if(aStepName == KTestsharedmemory15)
		{
			testStep = new CTestsharedmemory(aStepName);
		}
	if(aStepName == KTestsharedmemory16)
		{
			testStep = new CTestsharedmemory(aStepName);
		}
	if(aStepName == KTestsharedmemory17)
		{
			testStep = new CTestsharedmemory(aStepName);
		}
	if(aStepName == KTestsharedmemory18)
		{
			testStep = new CTestsharedmemory(aStepName);
		}
	if(aStepName == KTestsharedmemory19)
		{
			testStep = new CTestsharedmemory(aStepName);
		}
	if(aStepName == KTestsharedmemory20)
		{
			testStep = new CTestsharedmemory(aStepName);
		}
	if(aStepName == KTestsharedmemory21)
		{
			testStep = new CTestsharedmemory(aStepName);
		}
	if(aStepName == KTestsharedmemory22)
		{
			testStep = new CTestsharedmemory(aStepName);
		}
	if(aStepName == KTestsharedmemory23)
		{
			testStep = new CTestsharedmemory(aStepName);
		}
	if(aStepName == KTestsharedmemory24)
		{
			testStep = new CTestsharedmemory(aStepName);
		}
	if(aStepName == KTestsharedmemory25)
		{
			testStep = new CTestsharedmemory(aStepName);
		}
	if(aStepName == KTestsharedmemory26)
		{
			testStep = new CTestsharedmemory(aStepName);
		}
	if(aStepName == KTestsharedmemory27)
		{
			testStep = new CTestsharedmemory(aStepName);
		}
	if(aStepName == KTestsharedmemory28)
		{
			testStep = new CTestsharedmemory(aStepName);
		}
	if(aStepName == KTestsharedmemory29)
		{
			testStep = new CTestsharedmemory(aStepName);
		}
	if(aStepName == KTestsharedmemory30)
		{
			testStep = new CTestsharedmemory(aStepName);
		}
	return testStep;
	 }

