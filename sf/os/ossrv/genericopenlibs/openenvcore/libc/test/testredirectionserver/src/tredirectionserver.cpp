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
// Name        : tredirectionserver.cpp
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
 * tredirectionserver implementation
 */
#include "tredirectionserver.h"
#include "tdevnull.h"


_LIT(KSerName, "tredirectionserver");

CRedirTestServer* CRedirTestServer::NewL()
	{
	CRedirTestServer *server = new(ELeave) CRedirTestServer();
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
	CRedirTestServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err, server = CRedirTestServer::NewL());
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

CTestStep* CRedirTestServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;

	if(aStepName == Ktest_devnullread)
		{
		testStep = new CTestDevNull(aStepName);
		}
	if(aStepName == Ktest_devnullwrite)
		{
		testStep = new CTestDevNull(aStepName);
		}
	return testStep;
	}

