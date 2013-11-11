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
// Description:
// Name        : tlocaleserver.cpp
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
#include "tlocaleserver.h"
#include "tlocale.h"


_LIT(KServerName, "tlocale");

CLocaleTestServer* CLocaleTestServer::NewL()
	{
	CLocaleTestServer *server = new(ELeave) CLocaleTestServer();
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
	CLocaleTestServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err, server = CLocaleTestServer::NewL());
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

CTestStep* CLocaleTestServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;
    if(aStepName == Ksynclocale)
		{
		testStep = new CTestLocale(aStepName);
		}
    else if(aStepName == Ksynclocalehybrid)
		{
		testStep = new CTestLocale(aStepName);
		}
	else if(aStepName == Kstrcoll_locale)
		{
		testStep = new CTestLocale(aStepName);
		}
	else if(aStepName == Kstrcoll_synclocale)
		{
		testStep = new CTestLocale(aStepName);
		}
	else if(aStepName == Kclocale)
		{
		testStep = new CTestLocale(aStepName);
		}
 	else if(aStepName == KTestisascii)
		{
		testStep = new CTestLocale(aStepName);
		}
	else if(aStepName == KTesttoascii)
		{
		testStep = new CTestLocale(aStepName);
		}
	else if(aStepName == KTestmblen0)
		{
		testStep = new CTestLocale(aStepName);
		}
	else if(aStepName == KTestmblen1)
		{
		testStep = new CTestLocale(aStepName);
		}
	else if(aStepName == KTestmblen2)
		{
		testStep = new CTestLocale(aStepName);
		}
	else if(aStepName == KTestmblen3)
		{
		testStep = new CTestLocale(aStepName);
		}
	else if(aStepName == KTestmblen4)
		{
		testStep = new CTestLocale(aStepName);
		}
	else if(aStepName == KTestwctomb0)
		{
		testStep = new CTestLocale(aStepName);
		}
	else if(aStepName == KTestwctomb1)
		{
		testStep = new CTestLocale(aStepName);
		}
	else if(aStepName == KTestwctomb2)
		{
		testStep = new CTestLocale(aStepName);
		}
	else if(aStepName == KTestwctomb3)
		{
		testStep = new CTestLocale(aStepName);
		}
	else if(aStepName == KTestmbtowc0)
		{
		testStep = new CTestLocale(aStepName);
		}
	else if(aStepName == KTestmbtowc1)
		{
		testStep = new CTestLocale(aStepName);
		}
	else if(aStepName == KTestmbstowcs)
		{
		testStep = new CTestLocale(aStepName);
		}
	else if(aStepName == KTestwcstombs)
		{
		testStep = new CTestLocale(aStepName);
		}
	else if(aStepName == KTestwcrtomb0)
		{
		testStep = new CTestLocale(aStepName);
		}
	else if(aStepName == KTestwcrtomb1)
		{
		testStep = new CTestLocale(aStepName);
		}
	else if(aStepName == KTestwcrtomb_NULL)
		{
		testStep = new CTestLocale(aStepName);
		}
	else if(aStepName == KTestwcrtomb_Neg1)
		{
		testStep = new CTestLocale(aStepName);
		}
	else if(aStepName == KTestwcrtomb_Neg2)
		{
		testStep = new CTestLocale(aStepName);
		}
	else if(aStepName == KTestwcscoll)
		{
		testStep = new CTestLocale(aStepName);
		}
	else if(aStepName == KTestwcsicoll)
		{
		testStep = new CTestLocale(aStepName);
		}
	else if(aStepName == KTestwcslwr)
		{
		testStep = new CTestLocale(aStepName);
		}
	else if(aStepName == KTestwcsupr)
		{
		testStep = new CTestLocale(aStepName);
		}
	else if(aStepName == KTestwctrans)
		{
		testStep = new CTestLocale(aStepName);
		}
	else if(aStepName == KTestwctrans_Neg)
		{
		testStep = new CTestLocale(aStepName);
		}
	else if(aStepName == KTestwctype)
		{
		testStep = new CTestLocale(aStepName);
		}
	else if(aStepName == KTestwctype_Neg)
		{
		testStep = new CTestLocale(aStepName);
		}
	else if(aStepName == KTestwcstod)
		{
		testStep = new CTestLocale(aStepName);
		}
	else if(aStepName == KTestwcstod_Neg)
		{
		testStep = new CTestLocale(aStepName);
		}
	else if(aStepName == KTestwctob)
		{
		testStep = new CTestLocale(aStepName);
		}
	else if(aStepName == KTestwctob_Neg)
		{
		testStep = new CTestLocale(aStepName);
		}
	else if(aStepName == KTestwcswidth)
		{
		testStep = new CTestLocale(aStepName);
		}
	else if(aStepName == KTestwcswidth_Neg)
		{
		testStep = new CTestLocale(aStepName);
		}
	else if(aStepName == KTestwcwidth)
		{
		testStep = new CTestLocale(aStepName);
		}
	else if(aStepName == KTestmbrlen0)
		{
		testStep = new CTestLocale(aStepName);
		}
	else if(aStepName == KTestmbrlen1)
		{
		testStep = new CTestLocale(aStepName);
		}
	else if(aStepName == KTestmbrtowc)
		{
		testStep = new CTestLocale(aStepName);
		}
	else if(aStepName == KTestmbrtowc_Neg)
		{
		testStep = new CTestLocale(aStepName);
		}
	else if(aStepName == KTestmbsrtowcs)
		{
		testStep = new CTestLocale(aStepName);
		}
	else if(aStepName == KTestmbsrtowcs1)
		{
		testStep = new CTestLocale(aStepName);
		}
	else if(aStepName == KTestmbsinit)
		{
		testStep = new CTestLocale(aStepName);
		}
	else if(aStepName == KTestwcsrtombs)
		{
		testStep = new CTestLocale(aStepName);
		}
	else if(aStepName == KTestsetlocale)
		{
		testStep = new CTestLocale(aStepName);
		}
	else if(aStepName == KTestctypeApi)
		{
		testStep = new CTestLocale(aStepName);
		}
	else if(aStepName == Ktestnewlocale )
	    {
	    testStep = new CTestLocale(aStepName);
	    }
	return testStep;
	}

