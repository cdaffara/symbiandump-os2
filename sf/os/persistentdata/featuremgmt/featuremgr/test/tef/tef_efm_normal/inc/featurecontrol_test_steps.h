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
// featurecontrol_teststeps.cpp
// Feature Control component test steps
// 
//

/**
 @file
 @internalComponent
 @test
*/

#ifndef __FEATURECONTROLTESTSTEPS__
#define __FEATURECONTROLTESTSTEPS__

#include "efm_teststepbase.h"

/** Test case for CFeatureControl
instantation.
*/
_LIT(KFeatureControlConnetction, "FeatureControlConnetction");

class CFeatureControlConnetctionStep : public CTestStep
   {
public:
   CFeatureControlConnetctionStep ();
   virtual TVerdict doTestStepL(void);
   };
     
_LIT(KFeatureControlSingleFeatureRequest, "FeatureControlSingleFeatureRequest");
class CFeatureControlSingleFeatureRequestStep : public CTestStep
   {
public:
   CFeatureControlSingleFeatureRequestStep();
   virtual TVerdict doTestStepL(void);
   };

_LIT(KFeatureControlMultipleFeatureRequest, "FeatureControlMultipleFeatureRequest");
class CFeatureControlMultipleFeatureRequestStep: public CTestStep
   {
public:
   CFeatureControlMultipleFeatureRequestStep();
   virtual TVerdict doTestStepL(void);
   };     

_LIT(KFeatureControlDataAndStatusRequest, "FeatureControlDataAndStatusRequest");
class CFeatureControlDataAndStatusRequestStep : public CEFMConfigurableTestStepBase
   {
public:
   CFeatureControlDataAndStatusRequestStep();
   virtual TVerdict doTestStepL(void);
   }; 
        
_LIT(KFeatureControlListSupportedFeatures, "FeatureControlListSupportedFeatures");
class CFeatureControlListSupportedFeaturesStep : public CTestStep
   {
public:
   CFeatureControlListSupportedFeaturesStep();
   virtual TVerdict doTestStepL(void);
   };    

_LIT(KFeatureControlFeatureEnabling, "FeatureControlFeatureEnabling");
class CFeatureControlFeatureEnablingStep: public CEFMConfigurableTestStepBase
   {
public:
   CFeatureControlFeatureEnablingStep();
   virtual TVerdict doTestStepL(void);
   };     
        
_LIT(KFeatureControlFeatureDisabling, "FeatureControlFeatureDisabling");
class CFeatureControlFeatureDisablingStep: public CEFMConfigurableTestStepBase
   {
public:
   CFeatureControlFeatureDisablingStep();
   virtual TVerdict doTestStepL(void);
   };     

_LIT(KFeatureControlFeatureDataSetting, "FeatureControlFeatureDataSetting");
class CFeatureControlFeatureDataSettingStep: public CEFMConfigurableTestStepBase
   {
public:
   CFeatureControlFeatureDataSettingStep();
   virtual TVerdict doTestStepL(void);
   };

_LIT(KFeatureControlFeatureDataAndStatusSetting, "FeatureControlFeatureDataAndStatusSetting");
class CFeatureControlFeatureDataAndStatusSettingStep: public CEFMConfigurableTestStepBase
   {
public:
   CFeatureControlFeatureDataAndStatusSettingStep();
   virtual TVerdict doTestStepL(void);
   };        

_LIT(KFeatureControlFeatureAdding, "FeatureControlFeatureAdding");
class CFeatureControlFeatureAddingStep: public CEFMConfigurableTestStepBase
   {
public:
   CFeatureControlFeatureAddingStep();
   virtual TVerdict doTestStepL(void);
   }; 
   
_LIT(KFeatureControlInvalidUsePanicTest, "FeatureControlInvalidUsePanicTest");     
class CFeatureControlInvalidUsePanicTestStep: public CTestStep
   {
public:
   CFeatureControlInvalidUsePanicTestStep();
   virtual TVerdict doTestStepL(void);   
   };
   
#endif
