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
#include "tdouble_blrserver.h"
#include "tdouble_blr.h"


_LIT(KServerName, "tdouble_blr");

CTDouble_blrTestServer* CTDouble_blrTestServer::NewL()
	{
	CTDouble_blrTestServer *server = new(ELeave) CTDouble_blrTestServer();
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
	CTDouble_blrTestServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err, server = CTDouble_blrTestServer::NewL());
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

CTestStep* CTDouble_blrTestServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;

	// This server creates just one step but create as many as you want
	// They are created "just in time" when the worker thread is created
	// install steps
	if(aStepName == Kcbrt_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Kceil_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Kerf_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Kerfc_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Kexp_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Kexp2_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Kexpm1_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Kfabs_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Kilogb_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Kj0_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Kj1_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Klrint_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Kllrint_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Klog_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Klog10_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Klog1p_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Klogb_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Klround_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Kllround_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Krint_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Klog_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Ksqrt_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Ktrunc_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Ky0_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Ky1_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Kfloor_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Ksignificand_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Knearbyint_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Kisinf_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Kisnan_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Kisnan_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Kfdim_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Kfmax_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Kfmin_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Kfmod_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Khypot_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Kremainder_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Knexttoward_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Knextafter_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Kcopysign_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Kfjn_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Kfyn_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Kscalb_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Kscalbn_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Kpow_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Kacos_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Kacosh_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Kasin_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Kasinh_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Katan_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Katanh_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == KCos_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Kfpclassify_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Kcosh_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Ksin_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Ksinh_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Ktan_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Ktanh_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Katan2_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Kfma_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Kisfinite_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Kisnormal_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Ksignbit_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Kscalbln_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Kfinite_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Kmodf_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Kround_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Kldexp_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Kdrem_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Kfrexp_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Kremquo_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Kremquo_remcheck_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Kgamma_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	else if(aStepName == Klgamma_test)
		{
		testStep = new CTestDouble_blr(aStepName);
		}
	
	return testStep;
	}

