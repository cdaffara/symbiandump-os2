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
#include "tctypeserver.h"
#include "tctype.h"


_LIT(KServerName, "tctype");

CCtypeTestServer* CCtypeTestServer::NewL()
	{
	CCtypeTestServer *server = new(ELeave) CCtypeTestServer();
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
	CCtypeTestServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err, server = CCtypeTestServer::NewL());
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

CTestStep* CCtypeTestServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;

	// This server creates just one step but create as many as you want
	// They are created "just in time" when the worker thread is created
	// install steps
	if(aStepName == Kisalpha_atoz)
		{
		testStep = new CTestCtype(aStepName);
		}
	if(aStepName == Kisalpha_num)
		{
		testStep = new CTestCtype(aStepName);
		}
	if(aStepName == Kisalpha_punc)
		{
		testStep = new CTestCtype(aStepName);
		}
	if(aStepName == Kisalpha_escape)
		{
		testStep = new CTestCtype(aStepName);
		}
	        
	if(aStepName == Kisdigit_digit)
		{
		testStep = new CTestCtype(aStepName);
		}
	if(aStepName == Kisdigit_punc)
		{
		testStep = new CTestCtype(aStepName);
		}
	if(aStepName == Kisdigit_escape)
		{
		testStep = new CTestCtype(aStepName);
		}
	if(aStepName == Kisdigit_alpha)
		{
		testStep = new CTestCtype(aStepName);
		}
	        
	if(aStepName == Kisalnum_alpha)
		{
		testStep = new CTestCtype(aStepName);
		}
	if(aStepName == Kisalnum_num)
		{
		testStep = new CTestCtype(aStepName);
		}
	if(aStepName == Kisalnum_punc)
		{
		testStep = new CTestCtype(aStepName);
		}
	if(aStepName == Kisalnum_escape)
		{
		testStep = new CTestCtype(aStepName);
		}

	if(aStepName == Kispunct_punct)
		{
		testStep = new CTestCtype(aStepName);
		}
	if(aStepName == Kispunct_alpha_num)
		{
		testStep = new CTestCtype(aStepName);
		}
	if(aStepName == Kispunct_cntrl)
		{
		testStep = new CTestCtype(aStepName);
		}

	if(aStepName == Kisupper_upper)
		{
		testStep = new CTestCtype(aStepName);
		}
	if(aStepName == Kisupper_lower)
		{
		testStep = new CTestCtype(aStepName);
		}
	if(aStepName == Kisupper_punc)
		{
		testStep = new CTestCtype(aStepName);
		}
	if(aStepName == Kisupper_escape)
		{
		testStep = new CTestCtype(aStepName);
		}
			
	if(aStepName == Kislower_lower)
		{
		testStep = new CTestCtype(aStepName);
		}
	if(aStepName == Kislower_upper)
		{
		testStep = new CTestCtype(aStepName);
		}
	if(aStepName == Kislower_punc)
		{
		testStep = new CTestCtype(aStepName);
		}
	if(aStepName == Kislower_escape)
		{
		testStep = new CTestCtype(aStepName);
		}
			
	if(aStepName == Kisxdigit_xdigit)
		{
		testStep = new CTestCtype(aStepName);
		}
	if(aStepName == Kisxdigit_escape)
		{
		testStep = new CTestCtype(aStepName);
		}
	if(aStepName == Kisxdigit_punc)
		{
		testStep = new CTestCtype(aStepName);
		}
	if(aStepName == Kisxdigit_other_alpha)
		{
		testStep = new CTestCtype(aStepName);
		}
			
	if(aStepName == Ktoupper_lower)
		{
		testStep = new CTestCtype(aStepName);
		}
	if(aStepName == Ktoupper_upper)
		{
		testStep = new CTestCtype(aStepName);
		}
	if(aStepName == Ktoupper_punc)
		{
		testStep = new CTestCtype(aStepName);
		}
	if(aStepName == Ktoupper_escape)
		{
		testStep = new CTestCtype(aStepName);
		}

	if(aStepName == Ktolower_upper)
		{
		testStep = new CTestCtype(aStepName);
		}
	if(aStepName == Ktolower_lower)
		{
		testStep = new CTestCtype(aStepName);
		}
	if(aStepName == Ktolower_punc)
		{
		testStep = new CTestCtype(aStepName);
		}
	if(aStepName == Ktolower_escape)
		{
		testStep = new CTestCtype(aStepName);
		}

	if(aStepName == Kisspace_space)
		{
		testStep = new CTestCtype(aStepName);
		}
	if(aStepName == Kisspace_punc)
		{
		testStep = new CTestCtype(aStepName);
		}
	if(aStepName == Kisspace_alpha)
		{
		testStep = new CTestCtype(aStepName);
		}
	if(aStepName == Kisspace_num)
		{
		testStep = new CTestCtype(aStepName);
		}
			
	if(aStepName == Kiscntrl_cntrl)
		{
		testStep = new CTestCtype(aStepName);
		}
	if(aStepName == Kiscntrl_other_escape)
		{
		testStep = new CTestCtype(aStepName);
		}
	if(aStepName == Kiscntrl_alpha_num)
		{
		testStep = new CTestCtype(aStepName);
		}
			
	if(aStepName == Kisprint_cntrl)
		{
		testStep = new CTestCtype(aStepName);
		}
	if(aStepName == Kisprint_alpha_num)
		{
		testStep = new CTestCtype(aStepName);
		}
	if(aStepName == Kisprint_punc)
		{
		testStep = new CTestCtype(aStepName);
		}
	if(aStepName == Kisprint_xdigit)
		{
		testStep = new CTestCtype(aStepName);
		}
		
	if(aStepName == Kisgraph_escape)
		{
		testStep = new CTestCtype(aStepName);
		}
	if(aStepName == Kisgraph_alpha_num)
		{
		testStep = new CTestCtype(aStepName);
		}
	if(aStepName == Kisgraph_punc)
		{
		testStep = new CTestCtype(aStepName);
		}
	if(aStepName == Kisgraph_xdigit)
		{
		testStep = new CTestCtype(aStepName);
		}
	if(aStepName == Kisgraph_space)
		{
		testStep = new CTestCtype(aStepName);
		}
			
	if(aStepName == Kisascii_ascii)
		{
		testStep = new CTestCtype(aStepName);
		}
	if(aStepName == Kisascii_bad_param)
		{
		testStep = new CTestCtype(aStepName);
		}

	if(aStepName == Ktoascii_ascii)
		{
		testStep = new CTestCtype(aStepName);
		}
	if(aStepName == Ktoascii_bad)
		{
		testStep = new CTestCtype(aStepName);
		}
	
	return testStep;
	}

