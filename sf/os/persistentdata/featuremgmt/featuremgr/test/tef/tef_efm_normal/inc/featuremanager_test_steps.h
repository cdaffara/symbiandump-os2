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
// Feature Manager component test steps
// 
//

/**
 @file
 @internalComponent
 @test
*/

#ifndef __FEATUREMANAGERTESTSTEP__
#define __FEATUREMANAGERTESTSTEP__

#include "efm_teststepbase.h"

_LIT(KFeatureManager_InitializeLibL, "FeatureManager_InitializeLibL");
class CFeatureManager_InitializeLibLStep : public CTestStep
   {
public:
   CFeatureManager_InitializeLibLStep();
   virtual TVerdict doTestStepL(void);
   };

_LIT(KFeatureManager_InvalidUseTest, "FeatureManager_InvalidUseTest");
class CFeatureManager_InvalidUseTestStep : public CTestStep
   {
public:
   CFeatureManager_InvalidUseTestStep();
   virtual TVerdict doTestStepL(void);
   };            
   
_LIT(KFeatureManager_FeatureSupported, "FeatureManager_Supproted");
class CFeatureManager_FeatureSupportedStep : public CTestStep
   {
public:
   CFeatureManager_FeatureSupportedStep();
   virtual TVerdict doTestStepL(void);
   };            
#endif


                                        
