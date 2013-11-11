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
// EFM test steps for reconciliation testing
// 
//

/**
 @file
 @internalComponent
 @test
*/

#ifndef __RECONCILIATIONTESTSTEPS__
#define __RECONCILIATIONTESTSTEPS__

#include <test/testexecutestepbase.h>

_LIT(KReconciliationTest, "ReconciliationTest");
class CReconciliationTestStep : public CTestStep
   {
public:
   CReconciliationTestStep();
   virtual TVerdict doTestStepL(void);
   };
     
_LIT(KReconciliationROMSections, "ReconciliationROMSections");
class CReconciliationROMSections : public CTestStep
   {
public:
   CReconciliationROMSections();
   virtual TVerdict doTestStepL(void);
   };
   
   _LIT(KFeatureManagerPanicswithInvaliedFlagsets, "FeatureManagerPanicswithInvaliedFlagsets");
class CFeatureManagerPanicswithInvaliedFlagsetsStep: public CTestStep
   {
public:
   CFeatureManagerPanicswithInvaliedFlagsetsStep();
   virtual TVerdict doTestStepL(void);
   };   
   

 _LIT(KFeatureManagerInvalidFlagsetsRuntime, "FeatureManagerInvalidFlagsetsRuntime");
 class CFeatureManagerInvalidFlagsetsRuntimeStep: public CTestStep
      {
   public:
      CFeatureManagerInvalidFlagsetsRuntimeStep();
      virtual TVerdict doTestStepL(void);
      }; 
      
   _LIT(KFeatureControlDelete, "FeatureControlDelete");
class CFeatureControlDeleteStep: public CTestStep
   {
public:
   CFeatureControlDeleteStep();
   virtual TVerdict doTestStepL(void);
   };            
   
  
          
#endif


                                        
