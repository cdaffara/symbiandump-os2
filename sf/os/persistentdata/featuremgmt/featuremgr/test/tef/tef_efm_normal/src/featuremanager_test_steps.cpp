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

#include <featmgr/featmgr.h>
#include <featmgr/featurecontrol.h>
#include <featmgr/featurenotifier.h>
#include "featuremanager_test_steps.h"

CFeatureManager_InitializeLibLStep::CFeatureManager_InitializeLibLStep()
   {
     SetTestStepName(KFeatureManager_InitializeLibL);
   }

TVerdict CFeatureManager_InitializeLibLStep::doTestStepL()
	{
	TInt err = KErrNone;
	__UHEAP_MARK;
	TRAP(err, FeatureManager::InitializeLibL());
	TESTDIAGNOSTICERROR(err == KErrNone,
		 _L("FeatureManager::InitializeLibL failed: err = %d"),err);
	
	TRAP(err, FeatureManager::InitializeLibL());
	TESTDIAGNOSTICERROR(err == KErrNone,
		 _L("Call to FeatureManager::InitializeLibL failed when FeatureManager has already been initialised: err = %d"),err);

	FeatureManager::UnInitializeLib();
	FeatureManager::UnInitializeLib();
	__UHEAP_MARKEND;
	return TestStepResult();
	}
 
CFeatureManager_FeatureSupportedStep :: CFeatureManager_FeatureSupportedStep()
    {
    SetTestStepName(KFeatureManager_FeatureSupported);
    }
    
TVerdict CFeatureManager_FeatureSupportedStep::doTestStepL()
	{
	TBool supported; 
	FeatureManager::InitializeLibL();
	supported = FeatureManager::FeatureSupported( KDefaultSupportedUid.iUid );
	TESTDIAGNOSTIC(supported,
		 _L("FeatureManager::FeatureSupported failed: the feature that is supposed to be supported is reported as unsupported"));

	supported = FeatureManager::FeatureSupported( KDefaultUnsupportedUid.iUid );
	TESTDIAGNOSTIC(!supported,
		 _L("FeatureManager::FeatureSupported failed: the feature that is supposed to be unsupported is reported as supported"));

	supported = FeatureManager::FeatureSupported( KUnknownUid.iUid );
	TESTDIAGNOSTIC(!supported,
		 _L("FeatureManager::FeatureSupported failed: the feature that is supposed to be unknown is reported as supported"));

	FeatureManager::UnInitializeLib();
	return TestStepResult();   
	}

CFeatureManager_InvalidUseTestStep :: CFeatureManager_InvalidUseTestStep()
	{
	SetTestStepName(KFeatureManager_InvalidUseTest);
	}

TVerdict CFeatureManager_InvalidUseTestStep :: doTestStepL()
	{
	SetTestStepResult(EFail);
	FeatureManager::FeatureSupported( KDefaultSupportedUid.iUid );
    return TestStepResult();   	
	}
