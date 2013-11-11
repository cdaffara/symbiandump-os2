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
// EFM test steps for startup performance testing
// 
//

/**
 @file
 @internalComponent
 @test
*/

#include <featmgr/featurecontrol.h>
#include "efm_configured_testserver.h"
#include "startup_performance_test_steps.h"
#include "efm_teststepbase.h"
#include <hal.h>

#if !(defined(__WINSCW__) || defined (_DEBUG))
const TInt KTimingLimitH2 = 450;
const TInt KTimingLimitH4 = 350;
const TInt KTimingLimitH6 = 150;
#endif

TReal FastCountToMilliseconds(TInt aFastCount)
   {
   TInt freqInHz;
   HAL::Get(HAL::EFastCounterFrequency, freqInHz);
   TReal freqInkHz = freqInHz / 1000;
   return (TReal)aFastCount / freqInkHz;
   }

CStartupPerformanceTestStep::CStartupPerformanceTestStep()
   {
   SetTestStepName(KStartupPerformanceTest );
   }

TVerdict CStartupPerformanceTestStep::doTestStepL()
	{
	TInt startTime = User::FastCounter();
	RFeatureControl control;
	TInt err = control.Open();
	TESTDIAGNOSTICERROR(err==KErrNone,
			_L("RFeatureControl::Open failed: error = %d"),err);
	TInt endTime = User::FastCounter();
	control.Close();
	TReal netTime = FastCountToMilliseconds(endTime-startTime); 
	INFO_PRINTF2(_L("EFM server startup time = %f ms\n"),netTime);
	
#if !(defined(__WINSCW__) || defined (_DEBUG))
	//we only check timing results against timing limits on UREL hardware builds, RAM configuration
	TInt timingLimit = 0;
	RFs rfs;
	rfs.Connect();
	CleanupClosePushL(rfs);   
	   
	// determine media type of C drive
	TDriveInfo driveInf;
	rfs.Drive(driveInf, EDriveC);   
	  
	if (driveInf.iType == EMediaRam)
	      {
	      TInt muid = 0;   
	      HAL::Get(HAL::EMachineUid, muid);	      
	       switch (muid)
	            {
	            case HAL::EMachineUid_OmapH2:
	               timingLimit = KTimingLimitH2;
	               break;
	            case HAL::EMachineUid_OmapH4:
	               timingLimit = KTimingLimitH4;
	               break;
	            case HAL::EMachineUid_OmapH6:
	               timingLimit = KTimingLimitH6;
	               break;
	            default:
	               break;
	            }
	         }
	CleanupStack::PopAndDestroy(&rfs);
	if (timingLimit)
		{
		TESTDIAGNOSTICERROR(netTime<timingLimit,
		          _L("EFM server startup performance test failed: startup time should be less than %d ms"),timingLimit);
		}
#else
	INFO_PRINTF1(_L("Timings results are only checked on the RAM configuration of H2, H4 and H6 HRP\n"));
#endif	
	
	return TestStepResult();   
	}








