// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : twcharserver.cpp
// This material, including documentation and any related 
// computer programs, is protected by copyright controlled by 
// Nokia Corporation. All rights are reserved. Copying, 
// including reproducing, storing,  adapting or translating, any 
// or all of this material requires the prior written consent of 
// Nokia Corporation. This material also contains confidential 
// information which may not be disclosed to others without the 
// prior written consent of Nokia Corporation.
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
#include "twcharserver.h"
#include "twchar.h"


_LIT(KServerName, "twchar");

CWCharTestServer* CWCharTestServer::NewL()
	{
	CWCharTestServer *server = new(ELeave) CWCharTestServer();
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
	CWCharTestServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err, server = CWCharTestServer::NewL());
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


CTestStep* CWCharTestServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;

	// This server creates just one step but create as many as you want
	// They are created "just in time" when the worker thread is created
	// install steps
	if(aStepName == Kwcsupr1)
		{
		testStep = new CTestWChar(aStepName);
		}
	else if(aStepName == Kwcsupr2)
		{
		testStep = new CTestWChar(aStepName);
		}
	else if(aStepName == Kwcsupr3)
		{
		testStep = new CTestWChar(aStepName);
		}
	else if(aStepName == Kwcsupr4)
		{
		testStep = new CTestWChar(aStepName);
		}
	else if(aStepName == Kwcsupr5)
		{
		testStep = new CTestWChar(aStepName);
		}
	else if(aStepName == Kwcsupr6)
		{
		testStep = new CTestWChar(aStepName);
		}
	else if(aStepName == Kwcslwr1)
		{
		testStep = new CTestWChar(aStepName);
		}
	else if(aStepName == Kwcslwr2)
		{
		testStep = new CTestWChar(aStepName);
		}
	else if(aStepName == Kwcslwr3)
		{
		testStep = new CTestWChar(aStepName);
		}
	else if(aStepName == Kwcslwr4)
		{
		testStep = new CTestWChar(aStepName);
		}
	else if(aStepName == Kwcslwr5)
		{
		testStep = new CTestWChar(aStepName);
		}
	else if(aStepName == Kwcslwr6)
		{
		testStep = new CTestWChar(aStepName);
		}
	else if(aStepName == Kwcsrev1)
		{
		testStep = new CTestWChar(aStepName);
		}
	else if(aStepName == Kwcsrev2)
		{
		testStep = new CTestWChar(aStepName);
		}
	else if(aStepName == Kwcsrev3)
		{
		testStep = new CTestWChar(aStepName);
		}
	else if(aStepName == Kwcsrev4)
		{
		testStep = new CTestWChar(aStepName);
		}
	else if(aStepName == Kwcsrev5)
		{
		testStep = new CTestWChar(aStepName);
		}
	else if(aStepName == Kwcsrev6)
		{
		testStep = new CTestWChar(aStepName);
		}
	else if(aStepName == Kwcsicmp1)
		{
		testStep = new CTestWChar(aStepName);
		}
	else if(aStepName == Kwcsicmp2)
		{
		testStep = new CTestWChar(aStepName);
		}
	else if(aStepName == Kwcsicmp3)
		{
		testStep = new CTestWChar(aStepName);
		}
	else if(aStepName == Kwcsicmp4)
		{
		testStep = new CTestWChar(aStepName);
		}
	else if(aStepName == Kwcsicmp5)
		{
		testStep = new CTestWChar(aStepName);
		}
	else if(aStepName == Kwcsicmp6)
		{
		testStep = new CTestWChar(aStepName);
		}
	else if(aStepName == Kwcsicmp7)
		{
		testStep = new CTestWChar(aStepName);
		}
	else if(aStepName == Kwcsicmp8)
		{
		testStep = new CTestWChar(aStepName);
		}
	else if(aStepName == Kwcsicmp9)
		{
		testStep = new CTestWChar(aStepName);
		}
	else if(aStepName == Kwcsset1)
		{
		testStep = new CTestWChar(aStepName);
		}
	else if(aStepName == Kwcsset2)
		{
		testStep = new CTestWChar(aStepName);
		}
	else if(aStepName == Kwcsset3)
		{
		testStep = new CTestWChar(aStepName);
		}
	else if(aStepName == Kwcsset4)
		{
		testStep = new CTestWChar(aStepName);
		}
	else if(aStepName == Kwcsnset1)
		{
		testStep = new CTestWChar(aStepName);
		}
	else if(aStepName == Kwcsnset2)
		{
		testStep = new CTestWChar(aStepName);
		}
	else if(aStepName == Kwcsnset3)
		{
		testStep = new CTestWChar(aStepName);
		}
	else if(aStepName == Kwcsnset4)
		{
		testStep = new CTestWChar(aStepName);
		}
	else if(aStepName == Kwcsnset5)
		{
		testStep = new CTestWChar(aStepName);
		}
	else if(aStepName == Kwcsnset6)
		{
		testStep = new CTestWChar(aStepName);
		}		
	else if(aStepName == Kwstrtime1)
		{
		testStep = new CTestWChar(aStepName);
		}
	else if(aStepName == Kwstrtime2)
		{
		testStep = new CTestWChar(aStepName);
		}
	else if(aStepName == Kwstrdate1)
		{
		testStep = new CTestWChar(aStepName);
		}
	else if(aStepName == Kwstrdate2)
		{
		testStep = new CTestWChar(aStepName);
		}
	else if(aStepName == Kwcsnicmp1)
		{
		testStep = new CTestWChar(aStepName);
		}
	else if(aStepName == Kwcsnicmp2)
		{
		testStep = new CTestWChar(aStepName);
		}
	else if(aStepName == Kwcsnicmp3)
		{
		testStep = new CTestWChar(aStepName);
		}
	else if(aStepName == Kwcsnicmp4)
		{
		testStep = new CTestWChar(aStepName);
		}
	else if(aStepName == Kwcsnicmp5)
		{
		testStep = new CTestWChar(aStepName);
		}
	else if(aStepName == Kwcsnicmp6)
		{
		testStep = new CTestWChar(aStepName);
		}
	else if(aStepName == Kwcsnicmp7)
		{
		testStep = new CTestWChar(aStepName);
		}
	else if(aStepName == Kwcsnicmp8)
		{
		testStep = new CTestWChar(aStepName);
		}
	else if(aStepName == Kwcsnicmp9)
		{
		testStep = new CTestWChar(aStepName);
		}		
	else if(aStepName == Kwcsicoll1)
		{
		testStep = new CTestWChar(aStepName);
		}
	else if(aStepName == Kwcsicoll2)
		{
		testStep = new CTestWChar(aStepName);
		}
	else if(aStepName == Kwcsicoll3)
		{
		testStep = new CTestWChar(aStepName);
		}
	else if(aStepName == Kwcsncoll1)
		{
		testStep = new CTestWChar(aStepName);
		}
	else if(aStepName == Kwcsncoll2)
		{
		testStep = new CTestWChar(aStepName);
		}
	else if(aStepName == Kwcsncoll3)
		{
		testStep = new CTestWChar(aStepName);
		}
	else if(aStepName == Kwcsnicoll1)
		{
		testStep = new CTestWChar(aStepName);
		}
	else if(aStepName == Kwcsnicoll2)
		{
		testStep = new CTestWChar(aStepName);
		}
	else if(aStepName == Kwcsnicoll3)
		{
		testStep = new CTestWChar(aStepName);
		}
	else if(aStepName == Kwtmpnam1)
		{
		testStep = new CTestWChar(aStepName);
		}
	else if(aStepName == Kwtmpnam2)
		{
		testStep = new CTestWChar(aStepName);
		}

	return testStep;
	}

