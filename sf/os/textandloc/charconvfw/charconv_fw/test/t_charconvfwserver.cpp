/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "t_charconvfwserver.h"
#include "t_memory.h"
#include "t_simple7.h"
#include "t_simple8.h"
#include "t_rfc1642.h"
#include "t_comp7.h"
#include "t_comp8.h"
#include "t_realut8.h"
#include "t_realuni.h"
#include "t_error.h"
#include "t_newlin7.h"
#include "t_java.h"
#include "t_partial.h"
#include "t_imap.h"


/* Path to the script

c:\z:\t_TEF\t_TEF_All.script

*/

_LIT(KServerName,"T_CharConvFWServer");

CT_CharConvFwServer *CT_CharConvFwServer::NewL()
/**
   @return - Instance of the test server
   Same code for Secure and non-secure variants
   Called inside the MainL() function to create and start the
   CTestServer derived server.
 */
    {
    CT_CharConvFwServer * server = new (ELeave) CT_CharConvFwServer();
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
    CT_CharConvFwServer *server = NULL;
    // Create the CTestServer derived server
    TRAPD(err,server = CT_CharConvFwServer::NewL());
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

CTestStep *CT_CharConvFwServer::CreateTestStep(const TDesC& aStepName)
/**
   @return - A CTestStep derived instance
   Secure and non-secure variants
   Implementation of CTestServer pure virtual
 */
    {
    if(aStepName == KTestStep_T_MEMORY)
        {
        return new CT_MEMORY();
        }
    else if(aStepName == KTestStep_T_SIMPLE7)
        {
        return new CT_SIMPLE7();
        }
    else if(aStepName == KTestStep_T_SIMPLE8)
        {
        return new CT_SIMPLE8();
        }
    else if(aStepName == KTestStep_T_RFC1642)
        {
        return new CT_RFC1642();
        }
    else if(aStepName == KTestStep_T_COMP7)
        {
        return new CT_COMP7();
        }
    else if(aStepName == KTestStep_T_COMP8)
        {
        return new CT_COMP8();
        }
    else if(aStepName == KTestStep_T_REALUT8)
        {
        return new CT_REALUT8();
        }
    else if(aStepName == KTestStep_T_REALUNI)
        {
        return new CT_REALUNI();
        }
    else if(aStepName == KTestStep_T_ERROR)
        {
        return new CT_ERROR();
        }
    else if(aStepName == KTestStep_T_NEWLIN7)
        {
        return new CT_NEWLIN7();
        }
    else if(aStepName == KTestStep_T_JAVA)
        {
        return new CT_JAVA();
        }
    else if(aStepName == KTestStep_T_PARTIAL)
        {
        return new CT_PARTIAL();
        }
    else if(aStepName == KTestStep_T_IMAP)
        {
        return new CT_IMAP();
        }
    
    return NULL;
    }

