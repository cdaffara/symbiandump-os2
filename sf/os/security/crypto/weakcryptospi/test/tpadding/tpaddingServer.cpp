/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* paddingServer.cpp
*
*/



#include "tpaddingServer.h"
#include "tpaddingTestSteps.h"
#include "tpaddingNone.h"
#include "tpaddingSSLv3.h"
#include "tpaddingPKCS1.h"
#include "tpaddingPKCS7.h"

// name of test server 
_LIT(KServerName, "tpaddingserver");

// server class name
CPaddingServer* CPaddingServer::NewL()
/**
 * @return - Instance of the test server
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
{
   // new server class 
   CPaddingServer * server = new (ELeave) CPaddingServer();
   CleanupStack::PushL(server);
	
   // Either use a StartL or ConstructL, the latter will permit
   // Server Logging.

   //server->StartL(KServerName); 
   server->ConstructL(KServerName);
   CleanupStack::Pop(server);
   return server;
}

// EKA2 much simpler
// Just an E32Main and a MainL()
LOCAL_C void MainL()
/**
 * Much simpler, uses the new Rendezvous() call to sync with the client
 */
{
   // Leave the hooks in for platform security
#if (defined __DATA_CAGING__)
   RProcess().DataCaging(RProcess::EDataCagingOn);
   RProcess().SecureApi(RProcess::ESecureApiOn);
#endif
   CActiveScheduler* sched=NULL;
   sched=new(ELeave) CActiveScheduler;
   CActiveScheduler::Install(sched);
   // server name
   CPaddingServer* server = NULL;
   // Create the CTestServer derived server
   // server name
   TRAPD(err,server = CPaddingServer::NewL());
   if(!err)
   {
      // Sync with the client and enter the active scheduler
      RProcess::Rendezvous(KErrNone);
      sched->Start();
   }
   delete server;
   delete sched;
}

// Only a DLL on emulator for typhoon and earlier

GLDEF_C TInt E32Main()
/**
 * @return - Standard Epoc error code on exit
 */
{
   CTrapCleanup* cleanup = CTrapCleanup::New();
   if(cleanup == NULL)
   {
      return KErrNoMemory;
   }
   TRAP_IGNORE(MainL());
   delete cleanup;
   return KErrNone;
}

// Create a thread in the calling process
// Emulator typhoon and earlier

CTestStep* CPaddingServer::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Implementation of CTestServer pure virtual
 */
{
   CTestStep* testStep = NULL;
   // __EDIT_ME__ - Create your own test steps here
   // This server creates just one step but create as many as you want
   // They are created "just in time" when the worker thread is created

   if (aStepName == KPadNone)
      testStep = new (ELeave) CTestPadNone();
   else if (aStepName == KUnpadNone)
      testStep = new (ELeave) CTestUnpadNone();
   else if (aStepName == KPadSSLv3)
      testStep = new (ELeave) CTestPadSSLv3();
   else if (aStepName == KUnpadSSLv3)
      testStep = new (ELeave) CTestUnpadSSLv3();
   else if (aStepName == KPadPKCS1)
      testStep = new (ELeave) CTestPadPKCS1();
   else if (aStepName == KUnpadPKCS1)
      testStep = new (ELeave) CTestUnpadPKCS1();
   else if (aStepName == KPadPKCS7)
      testStep = new (ELeave) CTestPadPKCS7();
   else if (aStepName == KUnpadPKCS7)
      testStep = new (ELeave) CTestUnpadPKCS7();
   else if (aStepName == KUnpadCorruptPKCS7)
      testStep = new (ELeave) CTestUnpadCorruptPKCS7(); 
   else if (aStepName == KPaddingCorruptPKCS7)
   	  testStep = new (ELeave) CTestPaddingCorruptPKCS7();
   return testStep;
}
