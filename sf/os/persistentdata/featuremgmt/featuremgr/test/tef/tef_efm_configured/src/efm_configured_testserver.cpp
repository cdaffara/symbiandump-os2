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
// TEF test server for the test steps that require configured EFM server
// 
//

/**
 @file
 @internalComponent
 @test
*/

#include "efm_configured_testserver.h"
#include "reconciliation_test_steps.h"
#include "startup_performance_test_steps.h"
#include "persistence_test_steps.h"
#include "dsr_test_steps.h"
#include "plugin_test_steps.h"
#include "corruptdat_test_steps.h"
#include "characterisation_steps.h"
#include "memoryleak_test_steps.h"
 
_LIT(KServerName,"efm_configured_testserver");

CConfiguredEFMServer* CConfiguredEFMServer::NewL()
/**
   @return - Instance of the test server
   Same code for Secure and non-secure variants
   Called inside the MainL() function to create and start the
   CTestServer derived server.
 */
   {
   CConfiguredEFMServer* server = new (ELeave) CConfiguredEFMServer();
   CleanupStack::PushL(server);
   server->StartL(KServerName);
   CleanupStack::Pop(server);
   return server;
   }

// DEFECT 112153 - This function disables Lazy library unloading which keeps 
//   dlls in cache for two minutes from system boot-up.  This was casuing this 
//   test to fail
//as a plugin was being upgraded but ecom was returning the old plugin which was still in the cache. See defect for more details
LOCAL_C void CancelLazyDllUnloadingL()
	{
	RLoader loaderObj;
	User::LeaveIfError(loaderObj.Connect());
	CleanupClosePushL(loaderObj);
	User::LeaveIfError(loaderObj.CancelLazyDllUnload());
	RDebug::Print(_L("Cancelled Lazy Unloading of dlls"));
	CleanupStack::PopAndDestroy();
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
   //cancel lazy dll unloading to be able to delete plugin files
   TRAPD(LeaveError, CancelLazyDllUnloadingL())
   if (LeaveError)
      {
      RDebug::Print(_L("Unable to cancel Lazy Unloading of dlls, error may occur deleting dlls."));    	  
      }
   CActiveScheduler* sched=NULL;
   sched=new(ELeave) CActiveScheduler;
   CActiveScheduler::Install(sched);
   CConfiguredEFMServer* server = NULL;
   // Create the CTestServer derived server
   TRAPD(err,server = CConfiguredEFMServer::NewL());
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

CTestStep* CConfiguredEFMServer::CreateTestStep(const TDesC& aStepName)
/**
   @return - A CTestStep derived instance
   Secure and non-secure variants
   Implementation of CTestServer pure virtual
 */
   {
   CTestStep* testStep = NULL;
   if (aStepName == KPersistenceStoreSettingsTest)
      {
	  testStep = new CPersistenceStoreSettingsTestStep();
	  }
   else if (aStepName == KPersistenceLoadSettingsTest)
	  {
	  testStep = new CPersistenceLoadSettingsTestStep();
	  }
   else if (aStepName == KReconciliationTest)
      {
      testStep = new CReconciliationTestStep();
      }
   else if (aStepName == KStartupPerformanceTest)
	  {
	  testStep = new CStartupPerformanceTestStep();
	  }
   else if (aStepName == KDSRTest)
	  {
	  testStep = new CDSRTestStep();
	  }
   else if (aStepName == KDSRBoundsTest)
	  {
	  testStep = new CDSRBoundsTestStep();
	  }
   else if (aStepName == KFeatureManagerPanicswithInvaliedFlagsets)
      {
      testStep = new CFeatureManagerPanicswithInvaliedFlagsetsStep();
      }
   else if (aStepName == KFeatureManagerInvalidFlagsetsRuntime)
	  {
	  testStep = new CFeatureManagerInvalidFlagsetsRuntimeStep();
	  }         
   else if (aStepName == KFeatureControlDelete)
      {
      testStep = new CFeatureControlDeleteStep();
      }                               
   else if (aStepName == KNormalPluginTest)
	  {
	  testStep = new CNormalPluginTestStep();
	  }
   else if (aStepName == KCorruptPluginTest)
	  {
	  testStep = new CCorruptPluginTestStep();
	  }
   else if (aStepName == KHangingPluginTest)
	  {
	  testStep = new CHangingPluginTestStep();
	  }
   else if (aStepName == KSimpleEnhancedPluginTest)
	  {
	  testStep = new CSimpleEnhancedPluginTestStep();
	  }
   else if (aStepName == KCombinationNHPluginTest)
	  {
	  testStep = new CCombinationNHPluginTestStep();
	  }   
   else if (aStepName == KCombinationNCPluginTest)
	  {
	  testStep = new CCombinationNCPluginTestStep();
	  }   
   else if (aStepName == KSlowStartPluginTest)
	  {
	  testStep = new CSlowStartPluginTestStep();
	  }
   else if (aStepName == KCorruptFeatDatTest)
	  {
	  testStep = new CCorruptFeatDatFileStep();
	  }
   else if (aStepName == KMissingDSRDatFileStep)
	  {
	  testStep = new CMissingDSRDatFileStep();
	  }
   else if (aStepName == KMissingDatFileStep)
	  {
	  testStep = new CMissingDatFileStep();
	  }
   else if (aStepName == KOverlappingDSRFeatDatTest)
	  {
	  testStep = new COverlappingDSRFeatDatFileStep();
	  }
   else if (aStepName == KInvalidFlagsPluginTest)
	  {
	  testStep = new CInvalidFlagsPluginTest();
	  } 
   else if (aStepName == KCharacteriseSupported)
	  {
	  testStep = new CCharacteriseSupportedStep();
	  } 
   else if (aStepName == KCharacteriseSupportedArray)
	  {
	  testStep = new CCharacteriseSupportedArrayStep();
	  }
   else if (aStepName == KPluginCapabilitiesTest)
	  {
	  testStep = new CPluginCapabilitiesTest();
	  } 
   else if (aStepName == KMemoryLeak_ReNotifyRequestL)
	  {
	  testStep = new CMemoryLeak_ReNotifyRequestLStep();
	  }
   else if (aStepName == KDeleteFeatureStep)
	  {
	  testStep = new CDeleteFeatStep();
	  }
   else if(aStepName == KRefPluginsTest)
	  {
	  testStep = new CRefPluginsTest();
	  }
   else if(aStepName == KInvalidPluginTest)
	  {
	  testStep = new CInvalidPluginTest();
	  }
   else if(aStepName == KCorruptRuntimeFeaturesStep)
	  {
	  testStep = new CCorruptRuntimeFeaturesStep();
	  }
   else if(aStepName == KSimpleMergePluginTestStep)
	  {
	  testStep = new CSimpleMergePluginTestStep();
	  }
   
   return testStep;
   }
