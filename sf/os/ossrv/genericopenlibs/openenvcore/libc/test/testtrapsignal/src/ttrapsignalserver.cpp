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
// Name        : ttrapsignalserver.cpp
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
#include "ttrapsignalserver.h"
#include "ttrapsignal.h"


_LIT(KServerName, "ttrapsignal");

CSendsignalTestServer* CSendsignalTestServer::NewL()
	{
	CSendsignalTestServer *server = new(ELeave) CSendsignalTestServer();
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
	CSendsignalTestServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err, server = CSendsignalTestServer::NewL());
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

CTestStep* CSendsignalTestServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;

	if(aStepName == KTestsigactionapi)
		{
			testStep = new CTestTrapsignal(aStepName);
		}
	if(aStepName == KTestcustomsignalapi)
		{
			testStep = new CTestTrapsignal(aStepName);
		}
	if(aStepName == KTestcustomsigsetapi1)
		{
			testStep = new CTestTrapsignal(aStepName);
		}
	if(aStepName == KTestcustomsigsetapi2)
		{
			testStep = new CTestTrapsignal(aStepName);
		}
	if(aStepName == KTestcustombsd_signalapi)
		{
			testStep = new CTestTrapsignal(aStepName);
		}
	if(aStepName == KTestcustomsigactionapi)
		{
			testStep = new CTestTrapsignal(aStepName);
		}
		
	if(aStepName == KTestnegcustomsignalapi)
		{
			testStep = new CTestTrapsignal(aStepName);
		}
	if(aStepName == KTestnegcustomsigsetapi)
		{
			testStep = new CTestTrapsignal(aStepName);
		}
	if(aStepName == KTestnegcustombsd_signalapi)
		{
			testStep = new CTestTrapsignal(aStepName);
		}
	if(aStepName == KTestnegcustomsigactionapi)
		{
			testStep = new CTestTrapsignal(aStepName);
		}
	
	if(aStepName == KTestignoresignalapi)
		{
			testStep = new CTestTrapsignal(aStepName);
		}
	if(aStepName == KTestignoresigsetapi1)
		{
			testStep = new CTestTrapsignal(aStepName);
		}
	if(aStepName == KTestignoresigsetapi2)
		{
			testStep = new CTestTrapsignal(aStepName);
		}
	if(aStepName == KTestignorebsd_signalapi)
		{
			testStep = new CTestTrapsignal(aStepName);
		}
	if(aStepName == KTestignoresigactionapi)
		{
			testStep = new CTestTrapsignal(aStepName);
		}
	if(aStepName == KTestsigignoreapi)
		{
			testStep = new CTestTrapsignal(aStepName);
		}
	
	if(aStepName == KTestnegignoresignalapi)
		{
			testStep = new CTestTrapsignal(aStepName);
		}
	if(aStepName == KTestnegignoresigsetapi)
		{
			testStep = new CTestTrapsignal(aStepName);
		}
	if(aStepName == KTestnegignorebsd_signalapi)
		{
			testStep = new CTestTrapsignal(aStepName);
		}
	if(aStepName == KTestnegignoresigactionapi)
		{
			testStep = new CTestTrapsignal(aStepName);
		}
	if(aStepName == KTestnegsigignoreapi)
		{
			testStep = new CTestTrapsignal(aStepName);
		}

	if(aStepName == KTestnegdefaultsignalapi)
		{
			testStep = new CTestTrapsignal(aStepName);
		}
	if(aStepName == KTestnegdefaultsigsetapi)
		{
			testStep = new CTestTrapsignal(aStepName);
		}
	if(aStepName == KTestnegdefaultbsd_signalapi)
		{
			testStep = new CTestTrapsignal(aStepName);
		}
	if(aStepName == KTestnegdefaultsigactionapi)
		{
			testStep = new CTestTrapsignal(aStepName);
		}
	
	if(aStepName == KTestsigsethold1)
		{
			testStep = new CTestTrapsignal(aStepName);
		}
	
	if(aStepName == KTestsigactionapi1)
		{
			testStep = new CTestTrapsignal(aStepName);
		}
	if(aStepName == KTestsigactionapi2)
		{
			testStep = new CTestTrapsignal(aStepName);
		}
	if(aStepName == KTestsigactionapi3)
		{
			testStep = new CTestTrapsignal(aStepName);
		}
	if(aStepName == KTestsigactionapi4)
		{
			testStep = new CTestTrapsignal(aStepName);
		}
	if(aStepName == KTestsigactionapi5)
		{
			testStep = new CTestTrapsignal(aStepName);
		}
	if(aStepName == KTestsigactionapi6)
		{
			testStep = new CTestTrapsignal(aStepName);
		}
	return testStep;
	 }

