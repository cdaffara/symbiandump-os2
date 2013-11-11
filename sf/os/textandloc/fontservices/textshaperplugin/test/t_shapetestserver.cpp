/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <test/testexecuteserverbase.h>
#include "t_shapeteststep.h"


class CShapeTestServer : public CTestServer
    {
public:
    static CShapeTestServer* NewL();
    virtual CTestStep* CreateTestStep(const TDesC& aStepName);
    };


_LIT(KServerName,"t_shapetestserver");

CShapeTestServer *CShapeTestServer::NewL()
    {
    CShapeTestServer * server = new (ELeave) CShapeTestServer();
    CleanupStack::PushL(server);
    server->StartL(KServerName);
    CleanupStack::Pop(server);
    return server;
    }

CTestStep *CShapeTestServer::CreateTestStep(const TDesC& aStepName)
    {
    return CShapeTestStep::CreateShapeTestStep(aStepName);
    }


LOCAL_C void MainL()
    {
#if (defined __DATA_CAGING__)
    RProcess().DataCaging(RProcess::EDataCagingOn);
    RProcess().SecureApi(RProcess::ESecureApiOn);
#endif
    CActiveScheduler* sched=NULL;
    sched=new(ELeave) CActiveScheduler;
    CActiveScheduler::Install(sched);
    CShapeTestServer *server = NULL;
    TRAPD(err,server = CShapeTestServer::NewL());
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
    TRAPD(err,MainL());
    // This if statement is here just to shut up RVCT, which would otherwise warn
    // that err was set but never used
    if (err)
        {
        err = KErrNone;
        }
    delete cleanup;    
    __UHEAP_MARKEND;
    return KErrNone;
    }
