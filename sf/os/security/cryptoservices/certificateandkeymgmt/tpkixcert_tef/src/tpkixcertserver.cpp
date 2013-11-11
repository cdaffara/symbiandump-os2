/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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


#include "tpkixcertserver.h"
#include "apiteststeps.h"
#include "validateteststep.h"
#include "datetimefatalteststep.h"

_LIT(KServerName,"tpkixcert_tefserver");


CPkixCertTestServer* CPkixCertTestServer::NewL()
	{
	CPkixCertTestServer* server = new (ELeave) CPkixCertTestServer();
	CleanupStack::PushL(server);
	server->ConstructL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}

	
CTestStep* CPkixCertTestServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;

	if (aStepName == KAddSupportedOidsTestStep)
		{
		testStep = new CPkixCertAddSupportedOidsStep;
		}
	else if (aStepName == KRemoveSupportedOidsTestStep)
		{
		testStep = new CPkixCertRemoveSupportedOidsStep;
		}
	else if (aStepName == KSetSupportedOidsTestStep)
		{
		testStep = new CPkixCertSetSupportedOidsStep;
		}
	else if (aStepName == KResetSupportedOidsTestStep)
		{
		testStep = new CPkixCertResetSupportedOidsStep;
		}
	else if (aStepName == KValidateCertChainStep)
		{
		testStep = new CValidateCertChainStep;
		}
	else if (aStepName == KDateTimeFatalTestStep)
		{
		testStep = new CDateTimeFatalValidateStep;
		}
					
	return testStep;	
	}



LOCAL_C void MainL()
    {
    // Leave the hooks in for platform security
#if (defined __DATA_CAGING__)
    RProcess().DataCaging(RProcess::EDataCagingOn);
    RProcess().SecureApi(RProcess::ESecureApiOn);
#endif
    CActiveScheduler* sched=NULL;
    sched=new(ELeave) CActiveScheduler;
    CActiveScheduler::Install(sched);
    CPkixCertTestServer* server = NULL;
    TRAPD(err,server = CPkixCertTestServer::NewL());
    if(!err)
        {
        // Sync with the client and enter the active scheduler
        RProcess::Rendezvous(KErrNone);
        sched->Start();
        }
    delete server;
    delete sched;
    }


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
    
