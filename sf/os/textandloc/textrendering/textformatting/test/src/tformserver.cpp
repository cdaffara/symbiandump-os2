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
* @file
* @internalComponent 
*
*/

#include "tformserver.h"
#include "tcustomcharmapping.h"
#include "ttagmaimp.h"
#include "ttmsource.h"
#include "tbidicursorpos.h"
#include "ttmcode.h"
#include "tunique.h"
#include "tundo.h"
#include "tinterpreter.h"
#include "tformat.h"
#include "tinlinetext.h"
#include "tgraphemeiterator.h"
#include "tformbenchmark.h"
#include "tlinepag.h"
#include "tformhindi.h"

_LIT(KServerName,"TFormServer");

CTFormServer* CTFormServer::NewL()
    {
    CTFormServer * server = new (ELeave) CTFormServer();
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
    CTFormServer *server = NULL;
    // Create the CTestServer derived server
    TRAPD(err,server = CTFormServer::NewL());
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

CTestStep *CTFormServer::CreateTestStep(const TDesC& aStepName)
/**
   @return - A CTestStep derived instance
   Secure and non-secure variants
   Implementation of CTestServer pure virtual
 */
    {
    if(aStepName == KTestStep_T_CustomCharMapping)
        {
        return new CTCustomCharMappingStep();
        }
    else if(aStepName == KTestStep_T_TagmaImp)
        {
        return new CTTagmaImpStep();
        }
    else if(aStepName == KTestStep_T_TmSource)
        {
        return new CTTmSourceStep();
        }
    else if(aStepName == KTestStep_T_BidiCursorPos)
        {
        return new CTBidiCursorPosStep();
        }
    else if(aStepName == KTestStep_T_TmCode)
        {
        return new CTTmCodeStep();
        }
    else if(aStepName == KTestStep_T_Unique)
        {
        return new CTUniqueStep();
        }
    else if(aStepName == KTestStep_T_Undo)
        {
        return new CTUndoStep();
        }
    else if(aStepName == KTestStep_T_Interpreter)
        {
        return new CTInterpreterStep();
        }
    else if(aStepName == KTestStep_T_Format)
        {
        return new CTFormatStep();
        }
    else if(aStepName == KTestStep_T_InLineText)
        {
        return new CTInLineTextStep();
        }
    else if(aStepName == KTestStep_T_GraphemeIterator)
        {
        return new CTGraphemeIteratorStep();
        }
    else if(aStepName == KTestStep_T_FormBenchmark)
        {
        return new CTFormBenchmarkStep();
        }
    else if(aStepName == KTestStep_T_LinePag)
        {
        return new CTLinePagStep();
        }
    else if(aStepName == KTestStep_T_FormHindi)
        {
        return new CTFormHindiStep();
        }
        
    
    return NULL;
    }



