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




#include "T_FatCharsetConvServer.h"
#include "T_CP874.h"
#include "T_CP932.h"
#include "T_CP936.h"
#include "T_CP949.h"
#include "T_CP950.h"
#include "T_CP1250.h"
#include "T_CP1251.h"
#include "T_CP1252.h"
#include "T_CP1253.h"
#include "T_CP1254.h"
#include "T_CP1255.h"
#include "T_CP1256.h"
#include "T_CP1257.h"
#include "T_CP1258.h"
#include "T_CP54936.h"

/* Path to the script

c:\z:\t_TEF\t_TEF_All.script

*/

_LIT(KServerName,"T_FatCharsetConvServer");

CT_FatCharsetConvServer *CT_FatCharsetConvServer::NewL()
/**
   @return - Instance of the test server
   Same code for Secure and non-secure variants
   Called inside the MainL() function to create and start the
   CTestServer derived server.
 */
    {
	CT_FatCharsetConvServer * server = new (ELeave) CT_FatCharsetConvServer();
    CleanupStack::PushL(server);
    // CServer base class call
    server->StartL(KServerName);
    CleanupStack::Pop(server);
    return server;
    }


LOCAL_C void MainL()
//
// Secure variant
// Much simpler, uses the new Rendezvous() call to sync with the client
//
    {
#if (defined __DATA_CAGING__)
    RProcess().DataCaging(RProcess::EDataCagingOn);
    RProcess().SecureApi(RProcess::ESecureApiOn);
#endif
    CActiveScheduler* sched=NULL;
    sched=new(ELeave) CActiveScheduler;
    CActiveScheduler::Install(sched);
    CT_FatCharsetConvServer *server = NULL;
    // Create the CTestServer derived server
    TRAPD(err,server = CT_FatCharsetConvServer::NewL());
    if(!err)
        {
        // Sync with the client and enter the active scheduler
        RProcess::Rendezvous(KErrNone);
        sched->Start();
        }
    delete server;
    delete sched;
    }

/** @return - Standard Epoc error code on process exit
    Secure variant only
    Process entry point. Called by client using RProcess API
*/
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

CTestStep *CT_FatCharsetConvServer::CreateTestStep(const TDesC& aStepName)
/**
   @return - A CTestStep derived instance
   Secure and non-secure variants
   Implementation of CTestServer pure virtual
 */
    {
    if(aStepName == KTestStep_T_CP874)
        {
        return new CT_CP874();
        }
    else if(aStepName == KTestStep_T_CP932)
        {
        return new CT_CP932();
        }
    else if(aStepName == KTestStep_T_CP936)
        {
        return new CT_CP936();
        }
    else if(aStepName == KTestStep_T_CP949)
        {
        return new CT_CP949();
        }
    else if(aStepName == KTestStep_T_CP950)
        {
        return new CT_CP950();
        }
    else if(aStepName == KTestStep_T_CP1250)
        {
        return new CT_CP1250();
        }
    else if(aStepName == KTestStep_T_CP1251)
        {
        return new CT_CP1251();
        }
    else if(aStepName == KTestStep_T_CP1252)
        {
        return new CT_CP1252();
        }
    else if(aStepName == KTestStep_T_CP1253)
        {
        return new CT_CP1253();
        }
    else if(aStepName == KTestStep_T_CP1254)
        {
        return new CT_CP1254();
        }
    else if(aStepName == KTestStep_T_CP1255)
        {
        return new CT_CP1255();
        }
    else if(aStepName == KTestStep_T_CP1256)
        {
        return new CT_CP1256();
        }
    else if(aStepName == KTestStep_T_CP1257)
        {
        return new CT_CP1257();
        }
    else if(aStepName == KTestStep_T_CP1258)
        {
        return new CT_CP1258();
        }
    else if(aStepName == KTestStep_T_CP54936)
        {
        return new CT_CP54936();
        }
    return NULL;
    }

