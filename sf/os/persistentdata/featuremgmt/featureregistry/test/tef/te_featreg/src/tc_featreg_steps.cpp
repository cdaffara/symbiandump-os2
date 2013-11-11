// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Feature Registry component test steps
// 
//

/**
 @file
 @internalComponent
 @test
*/

#include <hal.h>
#include <test/testexecutelog.h>
#include <featreg.h>
#include <featregpan.h>
#include "tc_featreg_steps.h"
// following needed for constants used in test config files:
#include "maketestconfig.h"
#include "featregcmn.h"
// following needed for constants used in open and close test
#include "featureuids.h"

/**
 * Queries a single feature (actually the NULL UID, but any would do), passing if
 * it returns a zero (not supported) or positive (supported) value.
 * Fails if the query returns an error code.
 * Used to check whether the feature configuration info is not corrupt.
 */
CQueryAnyFeatureStep::CQueryAnyFeatureStep()
	{
	// MANDATORY call to base class method to set up the human readable name for logging.
	SetTestStepName(KQueryAnyFeatureStep);
	}

TVerdict CQueryAnyFeatureStep::doTestStepL()
	{
	SetTestStepResult(EFail);	

	TInt result = RFeatureRegistry::QuerySupportS(TUid::Null());

	if (result >= 0)
		{
		SetTestStepResult(EPass);
		}
	else
		{
		SetTestStepError(result);
		}
	return TestStepResult();
	}

/**
 * Run setup executable sourcing config file from C-data cage.
 * Expects corrupt config response: panic "EFeatRegBadConfigFile" in debug, error KErrCorrupt in release.
 */
CRunFeatRegSetupCorruptStep::CRunFeatRegSetupCorruptStep()
	{
	// MANDATORY call to base class method to set up the human readable name for logging.
	SetTestStepName(KRunFeatRegSetupCorruptStep);
	}

TVerdict CRunFeatRegSetupCorruptStep::doTestStepL()
	{
	SetTestStepResult(EFail);	
	TInt result = RunFeatRegSetup();
#ifdef _DEBUG
	// in debug only, a corrupt or missing config file should cause setup exe to panic
	if (result == EFeatRegBadConfig)
		{
		SetTestStepResult(EPass);
		}
#else // release
	if (result == KErrCorrupt)
		{
		SetTestStepResult(EPass);
		}
#endif
	return TestStepResult();
	}

/**
 * Queries a single feature (actually the NULL UID, but any would do).
 * Call is expected to panic in debug with EFeatRegBadConfig. In release it
 * should simply return KErrCorrupt, which is made into the above panic for
 * consistent treatment by TEF. Fails on any other return.
 */
CQueryAnyFeatureCorruptStep::CQueryAnyFeatureCorruptStep()
	{
	// MANDATORY call to base class method to set up the human readable name for logging.
	SetTestStepName(KQueryAnyFeatureCorruptStep);
	}

TVerdict CQueryAnyFeatureCorruptStep::doTestStepL()
	{
	SetTestStepResult(EFail);	
	TInt result = RFeatureRegistry::QuerySupportS(TUid::Null());
	if (result == KErrCorrupt)
		{
		Panic(EFeatRegBadConfig);
		}
	return TestStepResult();
	}

/**
 * Queries a number of feature UIDs and passes if all return negative - feature not supported.
 */
CQueryFeaturesNotSupportedStep::CQueryFeaturesNotSupportedStep()
	{
	// MANDATORY call to base class method to set up the human readable name for logging.
	SetTestStepName(KQueryFeaturesNotSupportedStep);
	}

TVerdict CQueryFeaturesNotSupportedStep::doTestStepL()
	{
	SetTestStepResult(EFail);	
	RFeatureRegistry featReg;
	TInt result = featReg.Open();
	User::LeaveIfError(result);
	CleanupClosePushL(featReg);
	SetTestStepResult(EPass);
	User::LeaveIfError(result = featReg.QuerySupport(TUid::Null()));
	TESTL(result == 0);
	TUid uid;
	User::LeaveIfError(result = featReg.QuerySupport(uid.Uid(KFeatRegTest1_01_UNFeatId)));
	TESTL(result == 0);
	User::LeaveIfError(result = featReg.QuerySupport(uid.Uid(KFeatRegTest1_02_SNFeatId)));
	TESTL(result == 0);
	User::LeaveIfError(result = featReg.QuerySupport(uid.Uid(KFeatRegTest1_03_LoFeatId)));
	TESTL(result == 0);
	User::LeaveIfError(result = featReg.QuerySupport(uid.Uid(KFeatRegTest1_04_HiFeatId)));
	TESTL(result == 0);
	User::LeaveIfError(result = featReg.QuerySupport(uid.Uid(KFeatRegTest1_05_URFeatId)));
	TESTL(result == 0);
	User::LeaveIfError(result = featReg.QuerySupport(uid.Uid(KFeatRegTest1_06_SRFeatId)));
	TESTL(result == 0);
	CleanupStack::PopAndDestroy(&featReg);
	return TestStepResult();
	}

/**
 * Checks correct API response for a small, valid config file (hand-crafted)
 */
CQueryFeaturesValidSmallStep::CQueryFeaturesValidSmallStep()
	{
	// MANDATORY call to base class method to set up the human readable name for logging.
	SetTestStepName(KQueryFeaturesValidSmallStep);
	}

TVerdict CQueryFeaturesValidSmallStep::doTestStepL()
	{
	SetTestStepResult(EFail);
	RFeatureRegistry featReg;
	TInt result = featReg.Open();
	User::LeaveIfError(result);
	CleanupClosePushL(featReg);
	SetTestStepResult(EPass);
	User::LeaveIfError(result = featReg.QuerySupport(TUid::Null()));
	TESTL(result == 0);
	TUid uid;
	TUint32 status;
	User::LeaveIfError(result = featReg.QuerySupport(uid.Uid(KFeatRegTest1_01_UNFeatId), status));
	TESTL(result == 0);
	TESTL(status == 0);
	User::LeaveIfError(result = featReg.QuerySupport(uid.Uid(KFeatRegTest1_02_SNFeatId), status));
	TESTL(result == 1);
#ifndef SYMBIAN_FEATURE_MANAGER	
	TESTL(status == KFeatRegTest1_02_SNFeatSt);
#endif	
	User::LeaveIfError(result = featReg.QuerySupport(uid.Uid(KFeatRegTest1_03_LoFeatId - 1)));
	TESTL(result == 0);
	User::LeaveIfError(result = featReg.QuerySupport(uid.Uid(KFeatRegTest1_03_LoFeatId)));
	TESTL(result == 1);
	User::LeaveIfError(result = featReg.QuerySupport(uid.Uid(KFeatRegTest1_04_HiFeatId)));
	TESTL(result == 1);
	User::LeaveIfError(result = featReg.QuerySupport(uid.Uid(KFeatRegTest1_04_HiFeatId + 1)));
	TESTL(result == 0);
	User::LeaveIfError(result = featReg.QuerySupport(uid.Uid(KFeatRegTest1_05_URFeatId), status));
	TESTL(result == 0);	
#ifndef SYMBIAN_FEATURE_MANAGER
	TESTL(status == KFeatRegTest1_05_URFeatSt);
#endif	
	User::LeaveIfError(result = featReg.QuerySupport(uid.Uid(KFeatRegTest1_06_SRFeatId), status));
	TESTL(result == 1);
	TESTL(status == RFeatureRegistry::EStatusSupportBit);
	CleanupStack::PopAndDestroy(&featReg);
	// extra tests on static query methods
	User::LeaveIfError(result = RFeatureRegistry::QuerySupportS(uid.Uid(KFeatRegTest1_03_LoFeatId)));
	TESTL(result == 1);
	User::LeaveIfError(result = RFeatureRegistry::QuerySupportS(uid.Uid(KFeatRegTest1_05_URFeatId), status));
	TESTL(result == 0);
#ifndef SYMBIAN_FEATURE_MANAGER
	TESTL(status == KFeatRegTest1_05_URFeatSt);
#endif
	return TestStepResult();
	}

/**
 * Checks correct API response for a large, valid config file (generated)
 */
CQueryFeaturesValidLargeStep::CQueryFeaturesValidLargeStep()
	{
	// MANDATORY call to base class method to set up the human readable name for logging.
	SetTestStepName(KQueryFeaturesValidLargeStep);
	}

TVerdict CQueryFeaturesValidLargeStep::doTestStepL()
	{
	SetTestStepResult(EFail);	
	RFeatureRegistry featReg;
	TInt result = featReg.Open();
	User::LeaveIfError(result);
	CleanupClosePushL(featReg);
	SetTestStepResult(EPass);
	TUid uid;
	// test the feature ids targeted in the config file
	TUint32 id = KFeatRegTest2_01_FirstId;
	TInt e;
	TBool expIsSupported;
	TBool defIsSupported;
	TBool actIsSupported;
	for (e = KFeatRegTest2_04_numTestFeatures; e > 0; e--)
		{
		expIsSupported = !(id & KFeatRegTest2_05_NotSupportedBit);
		User::LeaveIfError(result = featReg.QuerySupport(uid.Uid(id)));
		actIsSupported = result > 0;
		TESTL((actIsSupported && expIsSupported) || ((!actIsSupported) && (!expIsSupported)));
		id += KFeatRegTest2_03_IncrId;
		}
	// test the feature ids targeted in the config file PLUS 1 - should give default support
	id = KFeatRegTest2_01_FirstId + 1;
	for (e = KFeatRegTest2_04_numTestFeatures; e > 0; e--)
		{
		defIsSupported = id & KFeatRegTest2_06_DefSupportedBit;
		User::LeaveIfError(result = featReg.QuerySupport(uid.Uid(id)));
		actIsSupported = result > 0;
		TESTL((actIsSupported && defIsSupported) || ((!actIsSupported) && (!defIsSupported)));
		id += KFeatRegTest2_03_IncrId;
		}
	CleanupStack::PopAndDestroy(&featReg);
	// test again using static method
	id = KFeatRegTest2_01_FirstId;
	for (e = KFeatRegTest2_04_numTestFeatures; e > 0; e--)
		{
		expIsSupported = !(id & KFeatRegTest2_05_NotSupportedBit);
		User::LeaveIfError(result = RFeatureRegistry::QuerySupportS(uid.Uid(id)));
		actIsSupported = result > 0;
		TESTL(actIsSupported == expIsSupported);
		id += KFeatRegTest2_03_IncrId;
		}
	return TestStepResult();
	}

/**
 * Checks panic from calling non-static query on unopened RFeatureRegistry instance
 */
CInvalidUseQueryNotOpenStep::CInvalidUseQueryNotOpenStep()
	{
	// MANDATORY call to base class method to set up the human readable name for logging.
	SetTestStepName(KInvalidUseQueryNotOpenStep);
	}

TVerdict CInvalidUseQueryNotOpenStep::doTestStepL()
	{
	SetTestStepResult(EFail);	
	RFeatureRegistry featReg;
	TInt result = featReg.QuerySupport(TUid::Null()); // should panic - not open
	// following code should not be reachable
	TESTL(result >= 0);
	return TestStepResult();
	}

/**
 * Times FeatReg Query / Setup operations to measure performance.
 */
CFeatRegMeasPerformanceStep::CFeatRegMeasPerformanceStep()
	{
	// MANDATORY call to base class method to set up the human readable name for logging.
	SetTestStepName(KFeatRegMeasPerformanceStep);
	}

TVerdict CFeatRegMeasPerformanceStep::doTestStepL()
	{
	SetTestStepResult(EFail);

	// work out fast-timer frequency to convert ticks to microseconds
	TInt fastTimerFreq;
	HAL::Get(HALData::EFastCounterFrequency, fastTimerFreq);
	TReal ticksPerMicroSec = 1.0E-6 * fastTimerFreq;
	_LIT8(KFmtFreq, "                      Fast counter frequency = %10d Hz");
	Logger().WriteFormat(KFmtFreq, fastTimerFreq);

	// parameters for performance test, so a variety of UIDs are sampled
	const TUint32 KFeatRegPerf1_01_FirstId	= 0x00000007;
	const TUint32 KFeatRegPerf1_02_IncrId	= 0x00FEDCBA;
	const TUint32 KFeatRegPerf1_03_Count	= 100;
	// re-used variables
	TUint prevTime;
	TUint timeDiff;
	TUid uid;
	TInt result;
	TInt i;

	// measure time taken to establish session with file server, for comparison with setup costs
	prevTime = User::FastCounter();
	RFs fs;
	result = fs.Connect();
	TESTL(result == KErrNone);
	fs.Close();
	timeDiff = User::FastCounter() - prevTime;
	TReal64 fsSessionMicroSecs = timeDiff / ticksPerMicroSec;
	_LIT8(KFmtFsSession, "       Time to establish file server session = %10.2lf microseconds");
	Logger().WriteFormat(KFmtFsSession, fsSessionMicroSecs);

	// measure min, max and average time to perform first query with featregsetup.exe run
	const TUint setupCount = 10;
	uid = TUid::Null();
	TUint min = KMaxTUint;
	TUint max = 0;
	TUint64 sum = 0;
	_LIT(KForceSetupConfigArg, "force_setup");
	for (i = setupCount; i > 0; i--)
		{
		RunMakeTestConfig(KForceSetupConfigArg);
		prevTime = User::FastCounter();
		result = RFeatureRegistry::QuerySupportS(uid);
		timeDiff = User::FastCounter() - prevTime;
		TESTL(result >= 0);
		sum += timeDiff;
		if (timeDiff < min)
			{
			min = timeDiff;
			}
		if (timeDiff > max)
			{
			max = timeDiff;
			}
		}
	TReal64 firstQueryMinMicroSecs = min / ticksPerMicroSec;
	_LIT8(KFmtFirstQueryMin, "First query incl. setup, min of %4d samples = %10.2lf microseconds");
	Logger().WriteFormat(KFmtFirstQueryMin, setupCount, firstQueryMinMicroSecs);
	TReal64 firstQueryMaxMicroSecs = max / ticksPerMicroSec;
	_LIT8(KFmtFirstQueryMax, "First query incl. setup, max of %4d samples = %10.2lf microseconds");
	Logger().WriteFormat(KFmtFirstQueryMax, setupCount, firstQueryMaxMicroSecs);
	TReal64 firstQueryAvgMicroSecs = sum / (ticksPerMicroSec * setupCount);
	_LIT8(KFmtFirstQueryAvg, "First query incl. setup, avg of %4d samples = %10.2lf microseconds");
	Logger().WriteFormat(KFmtFirstQueryAvg, setupCount, firstQueryAvgMicroSecs);

	// measure average time to make a static query
	TUint32 id = KFeatRegPerf1_01_FirstId;
	prevTime = User::FastCounter();
	for (i = KFeatRegPerf1_03_Count; i > 0; i--)
		{
		result = RFeatureRegistry::QuerySupportS(uid.Uid(id));
		id += KFeatRegPerf1_02_IncrId;
		}
	timeDiff = User::FastCounter() - prevTime;
	TESTL(result >= 0);
	TReal64 staticQueryMicroSecs = timeDiff / (ticksPerMicroSec * KFeatRegPerf1_03_Count);
	_LIT8(KFmtStaticQuery,   "           Static query, avg of %4d samples = %10.2lf microseconds");
	Logger().WriteFormat(KFmtStaticQuery, KFeatRegPerf1_03_Count, staticQueryMicroSecs);

	// Measure time to make non-static queries, after featReg is opened
	// Obtain the number of reads to perform from the script file
	TLex lex1(this->ConfigSection());
	TInt nreads = -1;
    lex1.Val(nreads);
    TESTL(nreads>=0);
	// Open the P&S featreg property
	RFeatureRegistry featReg;
	result = featReg.Open();
	User::LeaveIfError(result);
	CleanupClosePushL(featReg);
	// Perform the reads	
	prevTime = User::FastCounter();
	id = KFeatRegTest2_01_FirstId;
	for (i = 0; i < nreads; i++)
		{
		result = featReg.QuerySupport(uid.Uid(id));
		id += KFeatRegTest2_03_IncrId;
		}
	timeDiff = User::FastCounter() - prevTime;
	// Report the time taken
	CleanupStack::PopAndDestroy(&featReg);
	TReal64 nonStaticQueryMicroSecs = timeDiff / (ticksPerMicroSec * nreads);
	_LIT8(KFmtNonStaticQuery,"       Non-static query, avg of %4d samples = %10.2lf microseconds");
	Logger().WriteFormat(KFmtNonStaticQuery, nreads, nonStaticQueryMicroSecs);

	SetTestStepResult(EPass);
	return TestStepResult();
	}

//Test cases not applicable for Feature manager wrapper
#ifndef SYMBIAN_FEATURE_MANAGER
/**
 * Checks correct RFeatureRegistryNotify API behaviour.
 * Note the feature property should be deleted before first running this test -
 * needed to prove false notification will not occur if subscribing for notification
 * before feature property is set up. Test configuration must be a valid file.
 */
CFeatRegNotifyStep::CFeatRegNotifyStep()
	{
	// MANDATORY call to base class method to set up the human readable name for logging.
	SetTestStepName(KFeatRegNotifyStep);
	}

TVerdict CFeatRegNotifyStep::doTestStepL()
	{
	SetTestStepResult(EFail);

	// test notify
	RFeatureRegistryNotify featRegNotify;
	TInt result = featRegNotify.Open();
	User::LeaveIfError(result);
	CleanupClosePushL(featRegNotify);
	TRequestStatus notifyStatus;
	featRegNotify.Subscribe(notifyStatus);
	TESTL(notifyStatus == KRequestPending);
	result = RFeatureRegistry::QuerySupportS(TUid::Null());
	TESTL(result >= 0);
	// check above query did not cause notification by invoking setup exe
	// (RFeatureRegistryNotify must ensure feature property is defined).
	TESTL(notifyStatus.Int() == KRequestPending);
	// run the setup exe to force notification
	result = RunFeatRegSetup();
	TESTL(0 == result);
	// should have notified:
	TESTL(notifyStatus.Int() == KErrNone);
	CleanupStack::PopAndDestroy(&featRegNotify);

	// test notify cancel
	result = featRegNotify.Open();
	User::LeaveIfError(result);
	CleanupClosePushL(featRegNotify);
	featRegNotify.Subscribe(notifyStatus);
	TESTL(notifyStatus == KRequestPending);
	featRegNotify.Cancel();
	TESTL(notifyStatus == KErrCancel);
	// run the setup exe to force notification
	result = RunFeatRegSetup();
	TESTL(0 == result);
	// since cancelled, should not have notified:
	TESTL(notifyStatus == KErrCancel);
	CleanupStack::PopAndDestroy(&featRegNotify);

	// test notify cancelled by Close()
	result = featRegNotify.Open();
	User::LeaveIfError(result);
	CleanupClosePushL(featRegNotify);
	featRegNotify.Subscribe(notifyStatus);
	TESTL(notifyStatus == KRequestPending);
	CleanupStack::PopAndDestroy(&featRegNotify);
	TESTL(notifyStatus == KErrCancel);

	SetTestStepResult(EPass);
	return TestStepResult();
	}

/**
 * Checks panic from calling Subscribe on unopened RFeatureRegistryNotify instance
 */
CInvalidUseSubscribeNotOpenStep::CInvalidUseSubscribeNotOpenStep()
	{
	// MANDATORY call to base class method to set up the human readable name for logging.
	SetTestStepName(KInvalidUseSubscribeNotOpenStep);
	}

TVerdict CInvalidUseSubscribeNotOpenStep::doTestStepL()
	{
	SetTestStepResult(EFail);	
	RFeatureRegistryNotify featRegNotify;
	TRequestStatus notifyStatus;
	featRegNotify.Subscribe(notifyStatus); // should panic - not open
	// following code should not be reachable
	return TestStepResult();
	}

/**
 * Checks panic from calling Cancel on unopened RFeatureRegistryNotify instance
 */
CInvalidUseCancelNotOpenStep::CInvalidUseCancelNotOpenStep()
	{
	// MANDATORY call to base class method to set up the human readable name for logging.
	SetTestStepName(KInvalidUseCancelNotOpenStep);
	}

TVerdict CInvalidUseCancelNotOpenStep::doTestStepL()
	{
	SetTestStepResult(EFail);	
	RFeatureRegistryNotify featRegNotify;
	featRegNotify.Cancel(); // should panic - not open
	// following code should not be reachable
	return TestStepResult();
	}


/**
 * Open and Close RFeatureRegistry object //wrapped around __UHEAP_MARK and __UHEAP_MARKEND.
 * //Should not panic.
 * //Panic if check fails for a user heap (memory leak). ALLOC: nnnnnnnn panic.
 */
CFeatRegOpenCloseStep::CFeatRegOpenCloseStep()
	{
	// MANDATORY call to base class method to set up the human readable name for logging.
	SetTestStepName(KFeatRegOpenCloseStep);
	}

TVerdict CFeatRegOpenCloseStep::doTestStepL()
	{
	SetTestStepResult(EFail);
	__UHEAP_MARK;
	RFeatureRegistry featReg;
 	const TBool opened = (featReg.Open() == KErrNone);
 	TBool haveUsb = opened && (featReg.QuerySupport(NFeature::KUsb) > 0);
 	TBool haveBluetooth = opened && (featReg.QuerySupport(NFeature::KBluetooth) > 0);
 	featReg.Close(); // can always call Close(), even if Open() failed:
 	__UHEAP_MARKEND; 
 	SetTestStepResult(EPass);
	return TestStepResult();
	}
#endif
