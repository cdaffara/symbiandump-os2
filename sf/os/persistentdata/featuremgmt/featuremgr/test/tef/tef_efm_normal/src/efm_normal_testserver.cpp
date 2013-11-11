// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// TEF test server for the EFM test steps that require unconfigured EFM server
// 
//

/**
 @file
 @internalComponent
 @test
*/

#include "efm_normal_testserver.h"
#include "featurediscovery_test_steps.h"
#include "featuremanager_test_steps.h"
#include "featurecontrol_test_steps.h"
#include "featurenotifier_test_steps.h"
#include "component_test_steps.h"
#include "feature_oom_test_steps.h"
#include "feature_ood_test_steps.h"
 
CTEFMServer* CTEFMServer::NewL()
/**
   @return - Instance of the test server
   Same code for Secure and non-secure variants
   Called inside the MainL() function to create and start the
   CTestServer derived server.
 */
   {
   CTEFMServer * server = new (ELeave) CTEFMServer();
   CleanupStack::PushL(server);
   
   // This test server uses different name after setcap
   TFileName exename = RProcess().FileName();
   TParsePtr pathparser(exename);
   server->StartL(pathparser.Name());
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
   CTEFMServer* server = NULL;
   // Create the CTestServer derived server
   TRAPD(err,server = CTEFMServer::NewL());
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

/** @return - Standard Epoc error code on process exit
    Secure variant only
    Process entry point. Called by client using RProcess API
*/
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

CTestStep* CTEFMServer::CreateTestStep(const TDesC& aStepName)
/**
   @return - A CTestStep derived instance
   Secure and non-secure variants
   Implementation of CTestServer pure virtual
 */
   {
   CTestStep* testStep = NULL;
   
   if (aStepName == KEFMTestStepPublishFeatures)// Test step that publishes features needed by later test steps
      {
      testStep = new CEFMTestStepPublishFeatures();
      }         
//Test steps for CFeatureDiscovery
   else if (aStepName == KFeatureDiscoverySingleFeatureStaticRequest)// Test case for CFeatureDiscovery instantation.
      {
      testStep = new CFeatureDiscoverySingleFeatureStaticRequestStep();
      }
   else if (aStepName == KFeatureDiscoverySingleFeatureDynamicRequest)// Test case for CFeatureDiscovery instantation.
      {
      testStep = new CFeatureDiscoverySingleFeatureDynamicRequestStep();
      }
   else if (aStepName == KFeatureDiscoveryMultipleFeatureStaticRequest)// Test case for CFeatureDiscovery instantation.
      {
      testStep = new CFeatureDiscoveryMultipleFeatureStaticRequestStep();
      }
   else if (aStepName == KFeatureDiscoveryMultipleFeatureDynamicRequest)// Test case for CFeatureDiscovery instantation.
      {
      testStep = new CFeatureDiscoveryMultipleFeatureDynamicRequestStep();
      }      
 
//test steps for FeatureManager        
       else if (aStepName == KFeatureManager_InitializeLibL)
      {
      testStep = new CFeatureManager_InitializeLibLStep();
      }
      else if (aStepName == KFeatureManager_FeatureSupported)
      {
      testStep = new CFeatureManager_FeatureSupportedStep();
      }
	  else if (aStepName == KFeatureManager_InvalidUseTest)
      {
      testStep = new CFeatureManager_InvalidUseTestStep();
      }
      
//test steps for RFeatureControl
   else if (aStepName == KFeatureControlConnetction)
      {
      testStep = new CFeatureControlConnetctionStep();
      }
    else if (aStepName == KFeatureControlSingleFeatureRequest)
      {
      testStep = new CFeatureControlSingleFeatureRequestStep();
      }
    else if (aStepName == KFeatureControlDataAndStatusRequest)
      {
      testStep = new CFeatureControlDataAndStatusRequestStep();
      }
    else if (aStepName == KFeatureControlMultipleFeatureRequest)
      {
      testStep = new CFeatureControlMultipleFeatureRequestStep();
      }        
    else if (aStepName == KFeatureControlFeatureEnabling)
      {
      testStep = new CFeatureControlFeatureEnablingStep();
      }
    else if (aStepName == KFeatureControlFeatureDisabling)
      {
      testStep = new CFeatureControlFeatureDisablingStep();
      }
    else if (aStepName == KFeatureControlFeatureDataSetting)
      {
      testStep = new CFeatureControlFeatureDataSettingStep();              
      }
    else if (aStepName == KFeatureControlFeatureDataAndStatusSetting)
      {
      testStep = new CFeatureControlFeatureDataAndStatusSettingStep();
      }
    else if (aStepName == KFeatureControlFeatureAdding)
      {
      testStep = new CFeatureControlFeatureAddingStep();
      }             
    else if (aStepName == KFeatureControlListSupportedFeatures)
      {
      testStep = new CFeatureControlListSupportedFeaturesStep();
      }
    else if (aStepName == KFeatureControlInvalidUsePanicTest)
        {
        testStep = new CFeatureControlInvalidUsePanicTestStep();
        }
   
      
//test steps for CFeatureNotifier         
   else if (aStepName == KFeatureNotifier_NewL)
      {
      testStep = new CFeatureNotifier_NewLStep();
      }
   else if (aStepName == KFeatureNotifier_NotifyRequestL)
      {
      testStep = new CFeatureNotifier_NotifyRequestLStep();
      }   
   else if (aStepName == KFeatureNotifier_NotifyRequestArrayL)
      {
      testStep = new CFeatureNotifier_NotifyRequestArrayLStep();
      }
   else if (aStepName == KFeatureNotifier_NotifyCancelL)
	  {
	  testStep = new CFeatureNotifier_NotifyCancelLStep();
	  }
   else if (aStepName == KFeatureNotifier_NotifyMiltipleCancelL)
	  {
	  testStep = new CFeatureNotifier_NotifyMiltipleCancel();
	  }
   else if (aStepName == KFeatureNotifier_UnchangedFeatureTest)
	  {
	  testStep = new CFeatureNotifier_UnchangedFeatureTestStep();
	  }
   else if (aStepName == KFeatureNotifier_CancelFeatureTwiceTest)
	  {
	  testStep = new CFeatureNotifier_CancelFeatureTwiceTestStep();
	  }
   else if (aStepName == KFeatureNotifier_NonExistingFeatureNotificationTest)
	  {
	  testStep = new CFeatureNotifier_NonExistingFeatureNotificationTestStep();
	  }
   else if (aStepName == KFeatureNotifier_NonExistingFeatureCancellationTest)
	  {
	  testStep = new CFeatureNotifier_NonExistingFeatureCancellationTestStep();
	  }	    	  
   
   //Multiple connections test steps
   else if (aStepName == KMulticonnectionTest1)
      {
      testStep = new CMulticonnectionTestStep1();
      }
   else if (aStepName == KMulticonnectionTest2)
      {
      testStep = new CMulticonnectionTestStep2();
      }         

   // OOM test steps.
   else if (aStepName == KFeatureOpenOOM)// case for RFeatureControl OOM Open.
      {
      testStep = new CFeatureOpenOOMTestStep();
      }
   else if (aStepName == KFeatureSupportedOOM)// RFeatureControl OOM Supported.
      {
      testStep = new CFeatureSupportedOOMTestStep();
      }
   else if (aStepName == KFeatureEnableOOM)// RFeatureControl OOM Enable.
      {
      testStep = new CFeatureEnableOOMTestStep();
      }
   else if (aStepName == KFeatureDisableOOM)// RFeatureControl OOM Disable.
      {
      testStep = new CFeatureDisableOOMTestStep();
      }
   else if (aStepName == KFeatureSetFeatureOOM)// RFeatureControl OOM SetFeat.
      {
      testStep = new CFeatureSetFeatureOOMTestStep();
      }
   else if (aStepName == KFeatureAddFeatureOOM)// RFeatureControl OOM AddFeat.
      {
      testStep = new CFeatureAddFeatureOOMTestStep();
      }
   else if (aStepName == KFeatureDelFeatureOOM)// RFeatureControl OOM DelFeat.
      {
      testStep = new CFeatureDelFeatureOOMTestStep();
      }
   else if (aStepName == KListSupportedFeaturesOOM)// ListSupportedFeatures
      {
      testStep = new CListSupportedFeaturesOOM();
      }   
   //OOD test
   else if (aStepName == KOODTest)
	  {
	  testStep = new COODTestStep();
	  }
   //Plugin ignoring test
   else if (aStepName == KPluginIgnoringTest)
	   {
	   testStep = new CPluginIgnoringTestStep();
	   }
      
   else if (aStepName == KFeatureNotifier_Thread_2_TestStep)
      {
      testStep = new CFeatureNotifier_Thread_2_TestStep();
      }
   else if (aStepName == KFeatureNotifier_Thread_3_TestStep)
      {
      testStep = new CFeatureNotifier_Thread_3_TestStep();
      }
   else if (aStepName == KFeatureNotifier_Thread_1_TestStep)
      {
      testStep = new CFeatureNotifier_Thread_1_TestStep();
      }
   else if (aStepName == KMultipleNotificationRequests)
	  {
	  testStep = new CMultipleNotificationRequestsStep();
	  }
   else if (aStepName == KFeatMgrStressTest)
   	  {
   	  testStep = new CFeatMgrStressTest();
   	  }
   else if (aStepName == KFeatmgrStartEndInstall )
	  {
	  testStep = new CFeatmgrStartEndInstall(); 
	  }

   return testStep;
   }
