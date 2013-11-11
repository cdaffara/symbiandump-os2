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

#include "TETextServer.h"
#include "T_FIELD1.h"
#include "T_ROMAN.h"
#include "T_STREAM.h"
#include "T_RICHOOM.h"
#include "TRTCOPY.h"
#include "TRTCUSTM.h"
#include "TTIMES1.h"
#include "T_CONVRT.h"
#include "T_CONVS.h"
#include "T_CONVS1.h"
#include "T_CPLAIN.h"
#include "T_CUTPST.h"
#include "T_FMT.h"
#include "T_FMT1.h"
#include "T_IMPORT.h"
#include "T_INDTER.h"
#include "T_LAYDOC.h"
#include "T_PARSE_for_TEF.h"
#include "T_PICRES.h"
#include "T_RICH1A.h"
#include "T_RTCLIP.h"
#include "T_SCANDC.h"
#include "T_STYLE.h"
#include "T_TIMES.h"
#include "T_TRAN.h"
#include "TEtextDefect.h"

_LIT(KServerName, "TETextServer");

CTETextServer *CTETextServer::NewL()
/**
   @return - Instance of the test server
   Same code for Secure and non-secure variants
   Called inside the MainL() function to create and start the
   CTestServer derived server.
 */
    {
	CTETextServer * server = new (ELeave) CTETextServer();
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
    CTETextServer *server = NULL;
    // Create the CTestServer derived server
    TRAPD(err,server = CTETextServer::NewL());
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

CTestStep *CTETextServer::CreateTestStep(const TDesC& aStepName)
/**
   @return - A CTestStep derived instance
   Secure and non-secure variants
   Implementation of CTestServer pure virtual
 */
	{
	if (aStepName == KTestStep_T_FIELD1)
		return new CT_FIELD1();
	else if (aStepName == KTestStep_T_ROMAN)
		return new CT_ROMAN();
	else if (aStepName == KTestStep_T_STREAM)
		return new CT_STREAM();
	else if (aStepName == KTestStep_T_RICHOOM)
		return new CT_RICHOOM();
    else if (aStepName == KTestStep_T_TRTCOPY)
        return new CT_TRTCOPY();
    else if (aStepName == KTestStep_T_TRTCUSTM)
        return new CT_TRTCUSTM();
    else if (aStepName == KTestStep_T_TTIMES1)
        return new CT_TTIMES1();
    else if (aStepName == KTestStep_T_CONVRT)
        return new CT_CONVRT();
    else if (aStepName == KTestStep_T_CONVS)
        return new CT_CONVS();
    else if (aStepName == KTestStep_T_CONVS1)
        return new CT_CONVS1();
    else if (aStepName == KTestStep_T_CPLAIN)
        return new CT_CPLAIN();
    else if (aStepName == KTestStep_T_CUTPST)
        return new CT_CUTPST();
    else if (aStepName == KTestStep_T_FMT)
        return new CT_FMT();
    else if (aStepName == KTestStep_T_FMT1)
        return new CT_FMT1();
    else if (aStepName == KTestStep_T_IMPORT)
        return new CT_IMPORT();
    else if (aStepName == KTestStep_T_INDTER)
        return new CT_INDTER();
    else if (aStepName == KTestStep_T_LAYDOC)
        return new CT_LAYDOC();
    else if (aStepName == KTestStep_T_PARSE)
        return new CT_PARSE();
    else if (aStepName == KTestStep_T_PICRES)
        return new CT_PICRES();
    else if (aStepName == KTestStep_T_RICH1A)
        return new CT_RICH1A();
    else if (aStepName == KTestStep_T_RTCLIP)
        return new CT_RTCLIP();
    else if (aStepName == KTestStep_T_SCANDC)
        return new CT_SCANDC();
    else if (aStepName == KTestStep_T_STYLE)
        return new CT_STYLE();
    else if (aStepName == KTestStep_T_TIMES)
        return new CT_TIMES();
    else if (aStepName == KTestStep_T_TRAN)
        return new CT_TRAN();
    else if (aStepName == KTestStep_TEtextDefect)
        return new CTEtextDefect();
	return NULL;
	}
