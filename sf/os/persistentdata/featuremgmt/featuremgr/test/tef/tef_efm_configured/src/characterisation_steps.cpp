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
// EFM test steps for characterising feature manager performance.
// 
//

/**
 @file
 @internalComponent
 @test
*/

#include <featdiscovery.h>
#include <featmgr/featurecontrol.h>
#include <hal.h>
#include "characterisation_steps.h"
#include "efm_teststepbase.h"


TBool SkipPerformanceTest = EFalse;
const TInt KPlatSimUid = 0x20029a73; //0x20029a73 will be changed soon once PlatSim get its own uid.

// This is the base class for some of our characterisation tests.
// Creates an RFeatureControl object, finds out what the fast counter
// frequency is and gets the timeout for this test.
TVerdict CCharacteriseBaseStep :: doTestStepPreambleL()
	{


	// Get the timeout from the .ini file. The name of the timeout entry
	// varies depending on what type of build we have.
	TBuf<20> timeoutini(_L("Timeout"));
#ifdef __WINSCW__
	timeoutini.Append(_L("_WINSCW"));
#endif
#ifdef __ARMCC__
	timeoutini.Append(_L("_ARM"));
	TInt muid = 0;   
   	HAL::Get(HAL::EMachineUid, muid);
    switch (muid)
	   {
	   case HAL::EMachineUid_OmapH2:
	   		timeoutini.Append(_L("_H2"));
	   		break;
	   case HAL::EMachineUid_OmapH4:
	   		timeoutini.Append(_L("_H4"));
	   		break;
	   case HAL::EMachineUid_OmapH6:
	   		timeoutini.Append(_L("_H6"));
	   		break;
	   case HAL::EMachineUid_NE1_TB:
	   		timeoutini.Append(_L("_NE1"));
	   		break;
	   case KPlatSimUid:
            ERR_PRINTF1(_L("Not running performance test in PlatSim"));
            SkipPerformanceTest = ETrue; //Skip performance test on PlatSim
            break;
	   //add any new hardware IDs here		
	   default:
	   		break;
	   	}
#endif
#ifdef _DEBUG
	timeoutini.Append(_L("_UDEB"));
#else
	timeoutini.Append(_L("_UREL"));
#endif
    if (!SkipPerformanceTest)
        {
        // First find out the granularity of the system clock..
        HAL::Get(HALData::EFastCounterFrequency, icountfreq);
        INFO_PRINTF2(_L("Counts per sec=%d"), icountfreq);
        icountperusec = icountfreq/1000000.0;
        INFO_PRINTF2(_L("Counts per usec=%f"), icountperusec);
        TInt err = icontrol.Open();
        TESTDIAGNOSTICERROR(err==KErrNone,
            _L("RFeatureControl::Open failed: error = %d"), err);
        if(err) { return TestStepResult(); }
        CleanupClosePushL(icontrol);
        // Get the timeout, which is measured in microseconds.
        if( !GetIntFromConfig(ConfigSection(), timeoutini, itimeout ) )
            {
            ERR_PRINTF2(_L("Can't find '%S' in .ini file"),
                            &timeoutini );
            CleanupStack::PopAndDestroy(&icontrol);
            SetTestStepResult(EFail);
            return TestStepResult();
            }
        }
	return TestStepResult();
	}

TVerdict CCharacteriseBaseStep :: doTestStepPostambleL()
	{
    if (SkipPerformanceTest)
		{
        return TestStepResult();
		}
	CleanupStack::PopAndDestroy(&icontrol);
	return TestStepResult();
	}

CCharacteriseSupportedStep :: CCharacteriseSupportedStep()
	{
	SetTestStepName(KCharacteriseSupported);
	}

TVerdict CCharacteriseSupportedStep::doTestStepL()
	{
    if (SkipPerformanceTest)
		{
		return TestStepResult();
		}
	// The base class 'CCharacteriseBaseStep' has already opened the
	// RFeatureControl object and read in the timeout number.

	// Find out how many features we're going to attempt to read, the
	// minimum uid (that we start on) and the number of supported UIDs
	// we expect to find in this range.
	TInt nfeats, minuid, expsup;
	if(!GetIntFromConfig(ConfigSection(), _L("NumFeaturesToRead"), nfeats) ||
	   !GetIntFromConfig(ConfigSection(), _L("MinFeatureUID"), minuid) ||
	   !GetIntFromConfig(ConfigSection(), _L("NumSupported"), expsup))
		{
		ERR_PRINTF1(_L("Can't find 'NumFeaturesToRead', 'MinFeatureUID' and 'NumSupported' in .ini file"));
		SetTestStepResult(EFail);
		return TestStepResult();
		}

	TInt mintime = 0x7fffffff;
	TInt maxtime = 0;
	TInt totaltime = User::FastCounter();
	for(TInt i=0 ; i<10 ; i++)
		{
		// This is the main part of the test where we perform timing..
		TInt nsup=0;
		TUint prevtime = User::FastCounter();
		for(TInt uid=minuid ; uid < minuid + nfeats ; uid++)
			{
			TUid tuid = { uid };
			if(icontrol.FeatureSupported(tuid) == KFeatureSupported) nsup++;
			}
		TUint elapsedtime = User::FastCounter() - prevtime;
		if(elapsedtime < mintime) mintime = elapsedtime;
		if(elapsedtime > maxtime) maxtime = elapsedtime;
		if(nsup != expsup)
			{
			ERR_PRINTF3(_L("Expected %d supported features, got %d"),
					expsup, nsup);
			SetTestStepResult(EFail);
			}
		}
	totaltime = User::FastCounter() - totaltime;
	TInt avetime = totaltime/10;

	TReal us = mintime / icountperusec;
	INFO_PRINTF2(_L("Minimum time %f us"), us);
	// This test has been changed to use the minimum timing to check against
	// the wanted because it suffers from lots of noise - I've seen
	// max/min ratios of > 10!
	if(us > itimeout)
		{
		ERR_PRINTF3(_L("Test failed timeout. Got '%f' minimum, wanted < '%d'"),
					us, itimeout);
		SetTestStepResult(EFail);
		}
	us = maxtime / icountperusec;
	INFO_PRINTF2(_L("Maximum time %f us"), us);
	us = avetime / icountperusec;
	INFO_PRINTF2(_L("Average time %f us"), us);
	return TestStepResult();
	}

CCharacteriseSupportedArrayStep :: CCharacteriseSupportedArrayStep()
	{
	SetTestStepName(KCharacteriseSupportedArray);
	}

TVerdict CCharacteriseSupportedArrayStep::doTestStepL()
	{
    if (SkipPerformanceTest)
		{
		return TestStepResult();
		}
	// Find out how many features we're going to attempt to read..
	TInt nfeats, minuid, expsup;
	if(!GetIntFromConfig(ConfigSection(), _L("NumFeaturesToRead"), nfeats) ||
	   !GetIntFromConfig(ConfigSection(), _L("MinFeatureUID"), minuid) ||
	   !GetIntFromConfig(ConfigSection(), _L("NumSupported"), expsup))
		{
		ERR_PRINTF1(_L("Can't find 'NumFeaturesToRead', 'MinFeatureUID' and 'NumSupported' in .ini file"));
		SetTestStepResult(EFail);
		return TestStepResult();
		}


	// Set up a loop so we can get average timings.
	TInt mintime = 0x7fffffff;
	TInt maxtime = 0;
	TInt totaltime = User::FastCounter();
	for(TInt i=0 ; i<10 ; i++)
		{
		// First generate an RFeatureArray containing the features we're interested
		// in.
		RFeatureArray rfa;
		CleanupClosePushL(rfa);
		for(TInt uid=minuid ; uid < minuid + nfeats ; uid++)
			{
			TUid tuid = { uid };
			rfa.AppendL(tuid);
			}
		// This is the main part of the test where we perform timing..
		TUint prevtime = User::FastCounter();
		TInt err = icontrol.FeaturesSupported(rfa);
		TUint elapsedtime = User::FastCounter() - prevtime;
		TInt nsup = rfa.Count(); // The array may have been pared down..
		CleanupStack::PopAndDestroy(&rfa);

		if(err)
			{
			ERR_PRINTF2(_L("FeaturesSupported gave %d, expected KErrNone"), err);
			SetTestStepResult(EFail);
			}
		if(nsup != expsup)
			{
			ERR_PRINTF3(_L("Expected %d supported features, got %d"),
					expsup, nsup);
			SetTestStepResult(EFail);
			}
		if(elapsedtime < mintime) mintime = elapsedtime;
		if(elapsedtime > maxtime) maxtime = elapsedtime;
		}
	totaltime = User::FastCounter() - totaltime;
	TInt avetime = totaltime/10;
	TReal us = mintime / icountperusec;
	INFO_PRINTF2(_L("Minimum time %f us"), us);
	// This test has been changed to use the minimum timing to check against
	// the wanted because it suffers from lots of noise - I've seen
	// max/min ratios of > 10!
	if(us > itimeout)
		{
		ERR_PRINTF3(_L("Test failed timeout. Got '%f' minimum, wanted < '%d'"),
					us, itimeout);
		SetTestStepResult(EFail);
		}
	us = maxtime / icountperusec;
	INFO_PRINTF2(_L("Maximum time %f us"), us);
	us = avetime / icountperusec;
	INFO_PRINTF2(_L("Average time %f us"), us);

	return TestStepResult();
	}
