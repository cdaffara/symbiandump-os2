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

#include "te_createconfigserver.h"
#include "te_createconfigemptystep.h"
#include "te_createconfigfullstep.h"

_LIT(KServerName,"te_createconfigsuite");


/**
 * Creates an instance of CTestCreateConfigServer object
 * @return a pointer to the new created CTestCreateConfigServer Object
 */
 
CTestCreateConfigServer* CTestCreateConfigServer::NewL()
	{
	CTestCreateConfigServer* tracer = new (ELeave) CTestCreateConfigServer();
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

CTestStep* CTestCreateConfigServer::CreateTestStep(const TDesC& aStepName)
{
	CTestStep* testStep = NULL;
	if(aStepName == KCreateConfigEmptyStep)
		testStep = new CCreateConfigEmptyTestStep();
	else if(aStepName == KCreateConfigFullStep)
		testStep = new CCreateConfigFUllTestStep();
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
	
	CTestCreateConfigServer* server = NULL;
	TRAPD(err,server = CTestCreateConfigServer::NewL());
	
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

