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
// feature_oom_test_steps.cpp
// Similar to Feature Control component test steps, but we're expecting in the
// 'doTestStepL' method, for out of memory conditions (set with the TEF '!OOM'
// option). We therefore create required resources in the Preamble step and
// delete them in the Postamble. The doTestStepL method then exercises the
// methods we are testing for memory leaks in an OOM environment.
// 
//

/**
 @file
 @internalComponent
 @test
*/

#ifndef __FEATUREOOMTESTSTEPS__
#define __FEATUREOOMTESTSTEPS__

#include "efm_teststepbase.h"

#include <featmgr/featurecontrol.h>
#include <featmgr/featmgr.h>
#include <featdiscovery.h>

_LIT(KFeatureOpenOOM, "FeatureOpenOOM");
class CFeatureOpenOOMTestStep : public CTestStep
   {
public:
   CFeatureOpenOOMTestStep() { SetTestStepName(KFeatureOpenOOM); };
   virtual TVerdict doTestStepL(void);
   };

_LIT(KFeatureSupportedOOM, "FeatureSupportedOOM");
class CFeatureSupportedOOMTestStep : public CTestStep
   {
public:
   CFeatureSupportedOOMTestStep() { SetTestStepName(KFeatureSupportedOOM); };
   virtual TVerdict doTestStepL(void);
   virtual TVerdict doTestStepPreambleL(void);
   virtual TVerdict doTestStepPostambleL(void);
protected:
   RFeatureControl icontrol;
   };

// derive from the above so we don't repeat the creation and destruction of
// the RFeatureControl member variable.
_LIT(KFeatureEnableOOM, "FeatureEnableOOM");
class CFeatureEnableOOMTestStep : public CFeatureSupportedOOMTestStep
   {
public:
   CFeatureEnableOOMTestStep() { SetTestStepName(KFeatureEnableOOM); };
   virtual TVerdict doTestStepL(void);
   };

// derive from the above so we don't repeat the creation and destruction of
// the RFeatureControl member variable.
_LIT(KFeatureDisableOOM, "FeatureDisableOOM");
class CFeatureDisableOOMTestStep : public CFeatureSupportedOOMTestStep
   {
public:
   CFeatureDisableOOMTestStep() { SetTestStepName(KFeatureDisableOOM); };
   virtual TVerdict doTestStepL(void);
   };

// derive from the above so we don't repeat the creation and destruction of
// the RFeatureControl member variable.
_LIT(KFeatureSetFeatureOOM, "FeatureSetFeatureOOM");
class CFeatureSetFeatureOOMTestStep : public CFeatureSupportedOOMTestStep
   {
public:
   CFeatureSetFeatureOOMTestStep() { SetTestStepName(KFeatureSetFeatureOOM); };
   virtual TVerdict doTestStepL(void);
   };

// derive from the above so we don't repeat the creation and destruction of
// the RFeatureControl member variable.
_LIT(KFeatureAddFeatureOOM, "FeatureAddFeatureOOM");
class CFeatureAddFeatureOOMTestStep : public CFeatureSupportedOOMTestStep
   {
public:
   CFeatureAddFeatureOOMTestStep() { SetTestStepName(KFeatureAddFeatureOOM); };
   virtual TVerdict doTestStepL(void);
   };

// derive from the above so we don't repeat the creation and destruction of
// the RFeatureControl member variable.
_LIT(KFeatureDelFeatureOOM, "FeatureDelFeatureOOM");
class CFeatureDelFeatureOOMTestStep : public CFeatureSupportedOOMTestStep
   {
public:
   CFeatureDelFeatureOOMTestStep() { SetTestStepName(KFeatureDelFeatureOOM); };
   virtual TVerdict doTestStepPreambleL(void);
   virtual TVerdict doTestStepL(void);
   virtual TVerdict doTestStepPostambleL(void);
   };

// derive from the above so we don't repeat the creation and destruction of
// the RFeatureControl member variable.
_LIT(KListSupportedFeaturesOOM, "ListSupportedFeaturesOOM");
class CListSupportedFeaturesOOM : public CFeatureSupportedOOMTestStep
   {
public:
   CListSupportedFeaturesOOM() { SetTestStepName(KListSupportedFeaturesOOM); };
   virtual TVerdict doTestStepL(void);
   };

#endif  // __FEATUREOOMTESTSTEPS__
