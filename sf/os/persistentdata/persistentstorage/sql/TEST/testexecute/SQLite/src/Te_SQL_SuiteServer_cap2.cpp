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
// te_sql_suitserver.cpp
// 
//

#include "Te_SQL_SuiteServer.h"
#include "ddtest.h"
#include "cdtest.h"

_LIT(KServerName,"Te_SQL_Suite_cap2");
CTe_SQL_Suite* CTe_SQL_Suite::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
    {
    CTe_SQL_Suite * server = new (ELeave) CTe_SQL_Suite();
    CleanupStack::PushL(server);
    // CServer base class call which can be either StartL or ConstructL,
    // the later will permit Server Logging.

    server->StartL(KServerName);
    CleanupStack::Pop(server);
    return server;
    }


// Secure variants much simpler
// For EKA2, just an E32Main and a MainL()
LOCAL_C void MainL()
/**
 * Secure variant
 * Much simpler, uses the new Rendezvous() call to sync with the client
 */
    {
    // Leave the hooks in for platform security
#if (defined __DATA_CAGING__)
    RProcess().DataCaging(RProcess::EDataCagingOn);
    RProcess().DataCaging(RProcess::ESecureApiOn);
#endif
    CActiveScheduler* sched=NULL;
    sched=new(ELeave) CActiveScheduler;
    CActiveScheduler::Install(sched);
    CTe_SQL_Suite* server = NULL;
    // Create the CTestServer derived server
    TRAPD(err,server = CTe_SQL_Suite::NewL());
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
/**
 * @return - Standard Epoc error code on process exit
 * Secure variant only
 * Process entry point. Called by client using RProcess API
 */
    {
    __UHEAP_MARK;
    CTrapCleanup* cleanup = CTrapCleanup::New();
    if(cleanup == NULL)
        {
        return KErrNoMemory;
        }
    TRAPD(err,MainL());
        
    delete cleanup;
    __UHEAP_MARKEND;
    
    return err;
    }


CTestStep* CTe_SQL_Suite::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
    {
    CTestStep* testStep = NULL;
    // They are created "just in time" when the worker thread is created
    // More test steps can be added below 
    if(aStepName == KSQLDDT)
        testStep = new CSQLDDT();
    else if(aStepName == KSQLCDT)
        testStep = new CSQLCDT();

    return testStep;
    }
