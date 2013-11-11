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
#include "tfloat_blrserver.h"
#include "tfloat_blr.h"


_LIT(KServerName, "tfloat_blr");

CFloat_blrTestServer* CFloat_blrTestServer::NewL()
	{
	CFloat_blrTestServer *server = new(ELeave) CFloat_blrTestServer();
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
	CFloat_blrTestServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err, server = CFloat_blrTestServer::NewL());
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
	
	
CTestStep* CFloat_blrTestServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;

	// This server creates just one step but create as many as you want
	// They are created "just in time" when the worker thread is created
	// install steps
	
	if(aStepName == Kcbrt_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	else if(aStepName == Kceil_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	else if(aStepName == Kerf_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	else if(aStepName == Kerfc_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	else if(aStepName == Kexp_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	else if(aStepName == Kexp2_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	else if(aStepName == Kexpm1_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	else if(aStepName == Kfabs_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	else if(aStepName == Kilogb_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	else if(aStepName == Kj0_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	else if(aStepName == Kj1_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	else if(aStepName == Klrint_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	else if(aStepName == Kllrint_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	else if(aStepName == Klog_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	else if(aStepName == Klog10_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	else if(aStepName == Klog1pf_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	else if(aStepName == Klogb_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	else if(aStepName == Klroundf_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	else if(aStepName == Kllroundf_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	else if(aStepName == Krintf_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	else if(aStepName == Klog_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	else if(aStepName == Ksqrtf_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	else if(aStepName == Ktruncf_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	else if(aStepName == Ky0f_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	else if(aStepName == Ky1f_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	else if(aStepName == Kfloorf_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	else if(aStepName == Ksignificandf_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	else if(aStepName == Knearbyintf_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	else if(aStepName == Kisinf_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	else if(aStepName == Kisnanf_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	else if(aStepName == Kfdimf_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	else if(aStepName == Kfmax_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	else if(aStepName == Kfmin_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	else if(aStepName == Kfmod_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	else if(aStepName == Khypot_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	else if(aStepName == Knexttoward_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	else if(aStepName == Knextafter_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	else if(aStepName == Kremainder_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	else if(aStepName == Kcopysign_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	else if(aStepName == Kfjn_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	else if(aStepName == Kfyn_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	else if(aStepName == Kscalbf_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	else if(aStepName == Kscalbnf_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	else if(aStepName == Kpow_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	else if(aStepName == Kacosf_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	else if(aStepName == Kacoshf_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	else if(aStepName == Kasinf_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	else if(aStepName == Kasinh_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	else if(aStepName == Katan_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	else if(aStepName == Katanh_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	else if(aStepName == KCosf_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	else if(aStepName == Kfpclassify_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	else if(aStepName == Kcosh_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	else if(aStepName == Ksinf_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	else if(aStepName == Ksinh_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	else if(aStepName == Ktan_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	else if(aStepName == Ktanh_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	else if(aStepName == Katan2_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	else if(aStepName == Kfma_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	else if(aStepName == Kisfinitef_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	else if(aStepName == Kisnormal_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	else if(aStepName == Ksignbit_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	else if(aStepName == Kscalbln_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	else if(aStepName == Kfinitef_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	else if(aStepName == Kmodf_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	else if(aStepName == Kroundf_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	else if(aStepName == Kldexpf_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	else if(aStepName == Kfrexp_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	else if(aStepName == Kremquo_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	else if(aStepName == Kremquo_remcheckf_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	else if(aStepName == Kgamma_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	else if(aStepName == Klgamma_test)
		{
		testStep = new CTestFloat_blr(aStepName);
		}
	return testStep;
	}

