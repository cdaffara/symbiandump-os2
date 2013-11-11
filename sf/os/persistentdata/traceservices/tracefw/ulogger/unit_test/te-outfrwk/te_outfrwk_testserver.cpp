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
// User includes
// 
//

#include "te_outfrwk_testserver.h"
#include "te_outfrwk_teststep.h"

_LIT(KServerName,"UloggerOutfrmwkServer");


/**
 * Creates an instance of CT_OutfrwkServer object
 * @return a pointer to the new created CT_OutfrwkServer Object
 */

CT_OutfrwkServer* CT_OutfrwkServer::NewL()
	{
	CT_OutfrwkServer* tracer = new (ELeave) CT_OutfrwkServer();
	CleanupStack::PushL(tracer);
	tracer->ConstructL(KServerName);
	CleanupStack::Pop(tracer); 
	return tracer;
	}

/**
 * Creates an instance of CTestStep object
 * @return a CTestStep derived instance
 * @param Test step name
 */
 
CTestStep* CT_OutfrwkServer::CreateTestStep(const TDesC& aStepName)
{
	CTestStep* testStep = NULL;
	if(aStepName == KTextMessageTeststep)
		testStep = new CTextMessageTest();
	return testStep;	
	
}

/**
 * Creates installs and starts the active scheduler 
 */
 
LOCAL_C void MainL()
	{
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	
	CT_OutfrwkServer* server = NULL;
	TRAPD(err,server = CT_OutfrwkServer::NewL());
	
	if(!err)
		{
 	  	RProcess::Rendezvous(KErrNone);
	  	sched->Start(); 
		}
		
	delete server;
	delete sched;
	}

/**
 *	Entrypoint method, creates a cleanup trap and calls
 *  the application start method.
 *	@return Standard error code on exit.
 */ 
 
GLDEF_C TInt E32Main()
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

