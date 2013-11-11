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
// Feature Discovery component test steps
// 
//

/**
 @file
 @internalComponent
 @test
*/

#ifndef __FEATUREDISCOVERYTESTSTEP__
#define __FEATUREDISCOVERYTESTSTEP__

#include "efm_teststepbase.h"

/** Test case for CFeatureDiscovery 
instantation.
*/
_LIT(KFeatureDiscoverySingleFeatureStaticRequest, "FeatureDiscoverySingleFeatureStaticRequest");     
class CFeatureDiscoverySingleFeatureStaticRequestStep: public CTestStep
	{
public:
   CFeatureDiscoverySingleFeatureStaticRequestStep();
   virtual TVerdict doTestStepL(void);	
	};

_LIT(KFeatureDiscoverySingleFeatureDynamicRequest, "FeatureDiscoverySingleFeatureDynamicRequest");     
class CFeatureDiscoverySingleFeatureDynamicRequestStep: public CTestStep
	{
public:
   CFeatureDiscoverySingleFeatureDynamicRequestStep();
   virtual TVerdict doTestStepL(void);	
	};

_LIT(KFeatureDiscoveryMultipleFeatureStaticRequest, "FeatureDiscoveryMultipleFeatureStaticRequest");     
class CFeatureDiscoveryMultipleFeatureStaticRequestStep: public CTestStep
	{
public:
   CFeatureDiscoveryMultipleFeatureStaticRequestStep();
   virtual TVerdict doTestStepL(void);	
	};

_LIT(KFeatureDiscoveryMultipleFeatureDynamicRequest, "FeatureDiscoveryMultipleFeatureDynamicRequest");     
class CFeatureDiscoveryMultipleFeatureDynamicRequestStep: public CTestStep
	{
public:
   CFeatureDiscoveryMultipleFeatureDynamicRequestStep();
   virtual TVerdict doTestStepL(void);	
	};
	
#endif
