/*
 ============================================================================
 Name		: t_TEF.cpp
 Author	  : Yanchun Li
 Copyright   : Your copyright notice
 Description : Exe source file
 ============================================================================
 */

//  Include Files  

#include "t_langutilsserver.h"
#include "t_langutils.h"
#include "t_langutilsimp.h"
#include "t_langutilslang.h"


/* Path to the script

c:\z:\t_TEF\t_TEF_All.script

*/

_LIT(KServerName,"T_LangUtilsServer");

CT_LangUtilsServer *CT_LangUtilsServer::NewL()
/**
   @return - Instance of the test server
   Same code for Secure and non-secure variants
   Called inside the MainL() function to create and start the
   CTestServer derived server.
 */
    {
    CT_LangUtilsServer * server = new (ELeave) CT_LangUtilsServer();
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
    CT_LangUtilsServer *server = NULL;
    // Create the CTestServer derived server
    TRAPD(err,server = CT_LangUtilsServer::NewL());
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

CTestStep *CT_LangUtilsServer::CreateTestStep(const TDesC& aStepName)
/**
   @return - A CTestStep derived instance
   Secure and non-secure variants
   Implementation of CTestServer pure virtual
 */
    {
    
    if(aStepName == KTestStep_T_LANGUTILS)
        {
        return new CT_LANGUTILS();
        }
    else if(aStepName == KTestStep_T_LANGUTILSIMP)
        {
        return new CT_LANGUTILSIMP();
        }
    else if(aStepName == KTestStep_T_LANGUTILSLANG)
        {
        return new CT_LANGUTILSLANG();
        }
        

    
    return NULL;
    }

