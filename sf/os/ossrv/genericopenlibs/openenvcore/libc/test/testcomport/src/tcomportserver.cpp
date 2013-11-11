// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : tcomportserver.cpp
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
 * random test server implementation
 */
#include "tcomportserver.h"
#include "tcomport.h"


_LIT(KSerName, "tcomport");

CComportTestServer* CComportTestServer::NewL()
	{
	CComportTestServer *server = new(ELeave) CComportTestServer();
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
	CComportTestServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err, server = CComportTestServer::NewL());
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

CTestStep* CComportTestServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;

	// This server creates just one step but create as many as you want
	// They are created "just in time" when the worker thread is created
	// install steps
	if(aStepName == Ktest_printf_COM)
		{
		testStep = new CTestcomport(aStepName);
		}
	else if(aStepName == Ktest_scanf_COM)
		{
		testStep = new CTestcomport(aStepName);
		}
	return testStep;
	}

