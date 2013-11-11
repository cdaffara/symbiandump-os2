// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

/**
 @file
 @internalComponent
*/
#include <startup.hrh>
#include <hal.h>
#include "RegistryData.h"
#include "Registrar.h"
#include "RegistrarObserver.h"
#include "DriveInfo.h"
#include "TE_EcomGranularityTestStep.h"

_LIT (KMinGranInfUidIndex, "MinGranInfUidIndex");
_LIT (KMinGranImplUidIndex, "MinGranImplUidIndex");
_LIT (KGranStep, "GranStep");
_LIT (KMaxGranInfUidIndex, "MaxGranInfUidIndex");
_LIT (KMaxGranImplUidIndex, "MaxGranImplUidIndex");


/** Stub class needed to instantiate CRegistrar object. */
class CTestRegistrarObserver : public MRegistrarObserver // codescanner::missingcclass
	{
public:
	// This is a stub. Hence do nothing in notification.
	void Notification( TInt /*aNotification*/ ) {}
	};

/** Constructor of CEComImplIndexPerfTest */
CEComImplIndexPerfTest::CEComImplIndexPerfTest()
	{
	SetTestStepName(KEComImplIndexPerfTest);
	}

/** destructor of CEComImplIndexPerfTest */
CEComImplIndexPerfTest::~CEComImplIndexPerfTest()
	{
	iFs.Close();
	delete iScheduler;
	}

/** Run the test */
TVerdict CEComImplIndexPerfTest::doTestStepL()
	{
	iScheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(iScheduler);
	User::LeaveIfError(iFs.Connect());

	if (ReadConfigParameters() != EPass)
		{
		return TestStepResult();
		}

	TInt fastCounterFreq;
	HAL::Get(HAL::EFastCounterFrequency, fastCounterFreq);
	TUint numTicksInHalfMilliSec = fastCounterFreq / 2000;

	const TInt KNumberOfDataPts = 
		(iMaxInfUidIndexGranularity - iMinInfUidIndexGranularity) / iGranStep + 1;
	HBufC* hbufc = HBufC::NewLC(10 * KNumberOfDataPts);
	TPtr   buf = hbufc->Des();

	// If a sample has the same granularities as the default values
	// in RegistryData.h, save the result in defaultGranularityTime.
	TUint32 defaultGranularityTime = 0x7fffffff;

	TUint32 besttime = 0x7fffffff;
	TUint32 worsttime = 0;
	TInt bestAllImplGranularity = 0;
	TInt bestImplUidGranularity = 0;

	TUint32 startTicks, endTicks;

	// Print header for table
	_LIT(KTableLegend, "Columns = iInterfaceImplIndex granularity, rows = iImplIndex granularity");
	Logger().Write(KTableLegend);

	_LIT(KCommaNumber, ",%d");
	_LIT(KCommaNumber3Wide, ",%3d");
	_LIT(KNumber2Wide, "%2d");

	_LIT(KColumnHeadSpace, "--");
	buf.Copy(KColumnHeadSpace);
	TInt j;
	for (j = iMinInfUidIndexGranularity; j <= iMaxInfUidIndexGranularity; j += iGranStep)
		{
		buf.AppendFormat(KCommaNumber3Wide, j);
		}
	Logger().Write(buf);


	CTestRegistrarObserver* registrarObserver = new (ELeave) CTestRegistrarObserver; 
	CleanupStack::PushL(registrarObserver);

	// The two nexting for loops below generate a 2-D table of
	// discovery time at different combinations of granularities.
	for (TInt implIndexGranularity = iMinIimplUidIndexGranularity;
		 implIndexGranularity <= iMaxIimplUidIndexGranularity;
		 implIndexGranularity += iGranStep)
		{
		buf.Format(KNumber2Wide, implIndexGranularity);

		for (TInt infIndexGranularity = iMinInfUidIndexGranularity;
			infIndexGranularity <= iMaxInfUidIndexGranularity; 
			infIndexGranularity += iGranStep)
			{
			CRegistryData* registryData = CRegistryData::NewL(iFs, infIndexGranularity, implIndexGranularity); 
			CleanupStack::PushL(registryData);

			CRegistrar* registrar = CRegistrar::NewL(*registryData, *registrarObserver, iFs); 
			CleanupStack::PushL(registrar);

			startTicks = User::FastCounter();
			registrar->ProcessSSAEventL(EStartupStateCriticalStatic);
			endTicks = User::FastCounter();

			TUint32 elapsedMilliSec = (endTicks - startTicks + numTicksInHalfMilliSec) * 1000 / fastCounterFreq;
			buf.AppendFormat(KCommaNumber, elapsedMilliSec);

			// Look for the best, worst and default granularity times.
			if ((KDefaultInterfaceImplIndexGranularity == infIndexGranularity) &&
			 	(KDefaultImplIndexGranularity == implIndexGranularity))
				{
				defaultGranularityTime = elapsedMilliSec;
				}

			if (besttime > elapsedMilliSec)
				{
				bestAllImplGranularity = infIndexGranularity;
				bestImplUidGranularity = implIndexGranularity;
				besttime = elapsedMilliSec;
				}
			else if (elapsedMilliSec > worsttime)
				{
				worsttime = elapsedMilliSec;
				}

			CleanupStack::PopAndDestroy(registrar);
			CleanupStack::PopAndDestroy(registryData);
			} // for AllImplementations index granularity

		// Show one row of result.
		Logger().Write(buf);
		} // for impl. UID index granularity

	CleanupStack::PopAndDestroy(registrarObserver);

	_LIT(KBest, "Best time is %d milli sec at interface idx granularity %d, Impl UID index granularity %d");
	INFO_PRINTF4(KBest, besttime, bestAllImplGranularity, bestImplUidGranularity);

	_LIT(KWorst, "Worst time is %d milli sec.");
	INFO_PRINTF2(KWorst, worsttime);

	if (0x7fffffff != defaultGranularityTime)
		{
		_LIT(KDefGran, "Default granularities (%d,%d) time is %d.");
		INFO_PRINTF4(KDefGran, KDefaultInterfaceImplIndexGranularity, KDefaultImplIndexGranularity, defaultGranularityTime);
		}

	CleanupStack::PopAndDestroy(hbufc);
	return TestStepResult();
	}

/** Read test configuration from ini file.
*/
TVerdict CEComImplIndexPerfTest::ReadConfigParameters()
	{
	_LIT (KMissingParamMsg, "Missing %S in config file");

	if (!GetIntFromConfig(ConfigSection(), KMinGranInfUidIndex, iMinInfUidIndexGranularity))
		{
		ERR_PRINTF2(KMissingParamMsg, &KMinGranInfUidIndex);
		SetTestStepResult(EFail);
		}

	if (!GetIntFromConfig(ConfigSection(), KMaxGranInfUidIndex, iMaxInfUidIndexGranularity))
		{
		ERR_PRINTF2(KMissingParamMsg, &KMaxGranInfUidIndex);
		SetTestStepResult(EFail);
		}

	if (!GetIntFromConfig(ConfigSection(), KMinGranImplUidIndex, iMinIimplUidIndexGranularity))
		{
		ERR_PRINTF2(KMissingParamMsg, &KMinGranImplUidIndex);
		SetTestStepResult(EFail);
		}

	if (!GetIntFromConfig(ConfigSection(), KMaxGranImplUidIndex, iMaxIimplUidIndexGranularity))
		{
		ERR_PRINTF2(KMissingParamMsg, &KMaxGranImplUidIndex);
		SetTestStepResult(EFail);
		}

	if (!GetIntFromConfig(ConfigSection(), KGranStep, iGranStep))
		{
		ERR_PRINTF2(KMissingParamMsg, &KGranStep);
		SetTestStepResult(EFail);
		}

#if defined(__WINS__) || defined(__WINSCW__)
	// On winscw, each data point in the plot takes 3 to 4 seconds (instead
	// of 0.3 s).  A table of 12 columns x 20 rows needs 1000 s.
	// It is not worthwhile to scan the full range because performance results
	// on winscw is meaningless anyway. Hence adjust the range to reduce the
	// stress on over night build.

	// reduce number of columns to plot
	iMinInfUidIndexGranularity += (iGranStep * 3);
	iMaxInfUidIndexGranularity -= iGranStep;

	iMinIimplUidIndexGranularity += (iGranStep * 4);
	iMaxIimplUidIndexGranularity -= (iGranStep * 2);

	INFO_PRINTF1(_L("This test takes too long to run on winscw."));
	INFO_PRINTF5(_L("Range of I/F idx granularities narrowed to %d,%d, Impl. idx granularity narrowed to %d,%d"), iMinInfUidIndexGranularity, iMaxInfUidIndexGranularity, iMinIimplUidIndexGranularity, iMaxIimplUidIndexGranularity);
#endif

	return TestStepResult();
	}
