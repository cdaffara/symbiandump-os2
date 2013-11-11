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
// Te_EComPerfTestStep.cpp
// Implements the performance loggr for ECOM
// 
//

/**
 @file
 @internalComponent
*/

#include "Te_EComStartupStatePerfTestStep.h"
#include <e32test.h>
#include <f32file.h>
#include <bautils.h>
#include <startup.hrh>
#include <ecom/ecom.h>
#include "Interface.h"
#include "EcomTestUtils.h"

#ifdef __ECOM_SERVER_PERFORMANCE__
// Start-up urel/armv5 test limits for CS and NC are calculated using the number of DLL's added to the
// registry for Ro Internal Drives and Non RO Internal Drives respectively.
// The test limit formula used for the upper limit is:
// 50mS+(1.75mS*number of DLLs discovered) for H2 builds 
// 15mS+(1mS*number of DLLs discovered) for H4 RAM (non-NAND) builds
// 125mS+(1.5mS*number of DLLs discovered) for H4 NAND builds 
// 5mS+(1mS*number of DLLs discovered) for H6 RAM (non-NAND) builds  
// 40mS+(1.5mS*number of DLLs discovered) for H6 NAND builds   
// These figures were determined using benchmarks from the ONB&TR.
// Although these limits are passed to CheckPerformance() for non urel/armv5 testing this method
// does not test against them. It is disabled.

const TReal KStartupLimitBaseH2RAM = 50;
const TReal KStartupLimitMultiplierH2RAM = 2.05;

const TReal KStartupLimitBaseH4RAM = 25;
const TReal KStartupLimitMultiplierH4RAM = 1;

const TReal KStartupLimitBaseH6RAM = 5;
const TReal KStartupLimitMultiplierH6RAM = 0.5;

const TReal KStartupLimitBaseH4NAND = 140;
const TReal KStartupLimitMultiplierH4NAND = 1.5;

const TReal KStartupLimitBaseH6NAND = 40;
const TReal KStartupLimitMultiplierH6NAND = 1;

// There is no discovery at CD therefore limit is fixed.
const TReal KCriticalDynamicLimitH2RAM = 10;
const TReal KCriticalDynamicLimitH4RAM = 2;
const TReal KCriticalDynamicLimitH4NAND = 5;
const TReal KCriticalDynamicLimitH6RAM = 1;
const TReal KCriticalDynamicLimitH6NAND = 2.5;

// Initialisation limit is independant of DLL count and is also fixed
const TReal KInitialisationLimitH2RAM = 50;
const TReal KInitialisationLimitH4RAM = 25;
const TReal KInitialisationLimitH4NAND = 25;
const TReal KInitialisationLimitH6RAM = 5;
const TReal KInitialisationLimitH6NAND = 5;

#endif //__ECOM_SERVER_PERFORMANCE__

CEComStartupStatePerfTest::CEComStartupStatePerfTest() : CEComPerfTestBase(KEComStartupStatePerfTest)
/**
 * Constructor
 */
	{
	// does nothing here.
	}

CEComStartupStatePerfTest::~CEComStartupStatePerfTest()
/**
 * Destructor
 */
	{
	// does nothing here.
	}

#ifdef __ECOM_SERVER_PERFORMANCE__
void CEComStartupStatePerfTest::RetrieveRegistryCountL(
		RegistryCounts::TRegistryCounts::TRegistryCountDriveType aDriveType,
		RegistryCounts::TRegistryCounts& aCounts,
		TPtrC aMessage)
	{
	RegistryCounts::GetRegistryCountsL(aDriveType, aCounts);
	INFO_PRINTF3(_L("Drive count for %S: [%d]"), &aMessage, aCounts.iDrives);
	INFO_PRINTF3(_L("DLL count for %S: [%d]"), &aMessage, aCounts.iDlls);
	INFO_PRINTF3(_L("Interface count for %S: [%d]"), &aMessage, aCounts.iInterfaces);
	INFO_PRINTF3(_L("Implementation count for %S: [%d]\n"), &aMessage, aCounts.iImplementations);
	}

TVerdict CEComStartupStatePerfTest::ProcessState(RStartupStateTimerResults& aTimerResults, 
		TInt aTimerState, 
		TReal& aStateStartTime, 
		TReal& aStateComplTime)
	{
	TInt idx = aTimerResults.FindInOrderTimerResult(aTimerState);

	if(idx == KErrNotFound)
	 	{
 		INFO_PRINTF1(_L("Something went wrong......whoops\n"));
	 	SetTestStepResult(EFail);
	 	return EFail;
	 	}
	
	const TStartupStateTimerResult& timerResult = aTimerResults.At(idx);
	aStateStartTime = timerResult.iStartTime;
	aStateComplTime = timerResult.iEndTime;
	
	INFO_PRINTF3(_L("State: %d elapsed time: %f mSecs\n"), aTimerState, aStateComplTime - aStateStartTime);
	
	return EPass;
	}
	
TVerdict CEComStartupStatePerfTest::ProcessState(REComHeapUsageRecords& aHeapResults, TInt aState,TInt& aHeapUsage)
	{
	TInt heap=aHeapResults.GetHeapUsageAtState(aState);
	if (heap==KErrNotFound)
		{
 		INFO_PRINTF1(_L("Something went wrong......whoops\n"));
	 	SetTestStepResult(EFail);
	 	return EFail;		
		}
	aHeapUsage=heap;
	INFO_PRINTF3(_L("State: %d heap usage: %d bytes\n"), aState,aHeapUsage);	
	return EPass;
	}
	
#endif //__ECOM_SERVER_PERFORMANCE__
	
/*
 Retrieves startup state timing from the ECom server and tests that the time spent during the different startup states is within a set of maximum bounds
 @return EPass if the test succeeds and the measured times are within the maximumum, EFail otherwise
 */
TVerdict CEComStartupStatePerfTest::doTestStepL()
	{
#ifdef __ECOM_SERVER_PERFORMANCE__

	// get the registry counts from the server
	// the DLL count is used to set the test limits for the startup tests
	INFO_PRINTF1(_L("RO Internal Registry Counts"));
	RegistryCounts::TRegistryCounts roIntCounts;
	RetrieveRegistryCountL(RegistryCounts::TRegistryCounts::ERoInternal, roIntCounts, _L("RO internal drives"));

	INFO_PRINTF1(_L("Non RO Internal Registry Counts"));
	RegistryCounts::TRegistryCounts nonRoIntCounts;
	RetrieveRegistryCountL(RegistryCounts::TRegistryCounts::ENonRoInternal, nonRoIntCounts, _L("Non RO internal drives"));

	INFO_PRINTF1(_L("All Registry Counts"));
	RegistryCounts::TRegistryCounts allCounts;
	RetrieveRegistryCountL(RegistryCounts::TRegistryCounts::EAll, allCounts, _L("All drives"));
   
	//set configuration-dependent timing limits
	TReal CriticalStaticLimit = 0;
	TReal NonCriticalLimit = 0;
	TReal CriticalDynamicLimit = 0;
	TReal InitialisationLimit = 0;

	THardwareConfiguration hardware_configuration = EComTestUtils::GetHardwareConfiguration();
	switch (hardware_configuration)
		{
		case EPlatformH2RAM:
	    	CriticalStaticLimit = KStartupLimitBaseH2RAM + (KStartupLimitMultiplierH2RAM * roIntCounts.iDlls);
			NonCriticalLimit = KStartupLimitBaseH2RAM + (KStartupLimitMultiplierH2RAM * nonRoIntCounts.iDlls);
			CriticalDynamicLimit = KCriticalDynamicLimitH2RAM;
			InitialisationLimit = KInitialisationLimitH2RAM;
			INFO_PRINTF1(_L("Hardware configuration: H2 RAM"));
			break;
		
		case EPlatformH2NAND:		
			INFO_PRINTF1(_L("Hardware configuration: H2 NAND"));
			INFO_PRINTF1(_L("***Performance testing on H2 NAND is not supported!***"));
			SetTestStepResult(EFail);			
			break;
			
		case EPlatformH2NANDDP:
			INFO_PRINTF1(_L("Hardware configuration: H2 NAND DP"));
			INFO_PRINTF1(_L("***Performance testing on H2 NAND DP is not supported!***"));
			SetTestStepResult(EFail);			
			break;
		
		case EPlatformH4RAM:
			CriticalStaticLimit = KStartupLimitBaseH4RAM + (KStartupLimitMultiplierH4RAM * roIntCounts.iDlls);
			NonCriticalLimit = KStartupLimitBaseH4RAM + (KStartupLimitMultiplierH4RAM * nonRoIntCounts.iDlls);
			CriticalDynamicLimit = KCriticalDynamicLimitH4RAM;
			InitialisationLimit = KInitialisationLimitH4RAM;
			INFO_PRINTF1(_L("Hardware configuration: H4 RAM"));
			break;
							
		case EPlatformH4NAND:		
			CriticalStaticLimit = KStartupLimitBaseH4NAND + (KStartupLimitMultiplierH4NAND * roIntCounts.iDlls);
			NonCriticalLimit = KStartupLimitBaseH4NAND + (KStartupLimitMultiplierH4NAND * nonRoIntCounts.iDlls);
			CriticalDynamicLimit = KCriticalDynamicLimitH4NAND;
			InitialisationLimit = KInitialisationLimitH4NAND;
			INFO_PRINTF1(_L("Hardware configuration: H4 NAND"));
			break;
		
		case EPlatformH4NANDDP:	
			INFO_PRINTF1(_L("Hardware configuration: H4 NAND DP"));
			break;

		case EPlatformH6RAM:
			CriticalStaticLimit = KStartupLimitBaseH6RAM + (KStartupLimitMultiplierH6RAM * roIntCounts.iDlls);
			NonCriticalLimit = KStartupLimitBaseH6RAM + (KStartupLimitMultiplierH6RAM * nonRoIntCounts.iDlls);
			CriticalDynamicLimit = KCriticalDynamicLimitH6RAM;
			InitialisationLimit = KInitialisationLimitH6RAM;
			INFO_PRINTF1(_L("Hardware configuration: H6 RAM"));
			break;
							
		case EPlatformH6NAND:		
			CriticalStaticLimit = KStartupLimitBaseH6NAND + (KStartupLimitMultiplierH6NAND * roIntCounts.iDlls);
			NonCriticalLimit = KStartupLimitBaseH6NAND + (KStartupLimitMultiplierH6NAND * nonRoIntCounts.iDlls);
			CriticalDynamicLimit = KCriticalDynamicLimitH6NAND;
			InitialisationLimit = KInitialisationLimitH6NAND;
			INFO_PRINTF1(_L("Hardware configuration: H6 NAND"));
			break;
		
		case EPlatformH6NANDDP:	
			INFO_PRINTF1(_L("Hardware configuration: H6 NAND DP"));
			break;
					
		case EPlatformWINSCW:
			INFO_PRINTF1(_L("Hardware configuration: WINSCW"));
			break;
		
		default:		
			SetTestStepResult(EFail);
			INFO_PRINTF1(_L("***Unrecognized platform!***"));
			break;		
		}
		
	// get all the timer results from the server
	RStartupStateTimerResults timerResults;
	timerResults.GetAllTimerResults();
	
	// Get ecom start time and state. The state is a test state and should be -1 
	TInt idx = timerResults.FindInOrderTimerResult(-1);
	if(idx == KErrNotFound)
	 	{
 		INFO_PRINTF1(_L("Something went wrong......whoops\n"));
	 	SetTestStepResult(EFail);
	 	return TestStepResult();
	 	}

	TStartupStateTimerResult timerResult = timerResults.At(idx);
	TReal ecomStartTime = timerResult.iStartTime;
	TReal ecomComplTime = timerResult.iEndTime;
	TReal stateStartTime = 0.0;
	TReal stateComplTime = 0.0;
	TReal accumulatedTime = 0.0;
	TReal initialisationTime = 0.0;
	
	//Test for Critical Static state
	INFO_PRINTF1(_L("Critical Static"));
	TVerdict result = ProcessState(timerResults, EStartupStateCriticalStatic, stateStartTime, stateComplTime);
	if(result != EPass && result != EIgnore)
		{
		return TestStepResult();
		}
	accumulatedTime += (stateComplTime - stateStartTime);
	CheckPerformance(stateComplTime - stateStartTime, CriticalStaticLimit, _L("Critical static state"));
	
	//Test for initialisation
	initialisationTime = ecomComplTime - ecomStartTime - accumulatedTime;
	INFO_PRINTF2(_L("Initialisation Elapsed: %f mSecs\n"), ecomComplTime - ecomStartTime);
	accumulatedTime += initialisationTime;
	CheckPerformance(initialisationTime, InitialisationLimit, _L("Initialisation Time"));

	//Test for Critical Dynamic state
	result = ProcessState(timerResults, EStartupStateCriticalDynamic, stateStartTime, stateComplTime);
	if(result != EPass && result != EIgnore)
		{
		return TestStepResult();
		}
	accumulatedTime += (stateComplTime - stateStartTime);
	CheckPerformance(stateComplTime - stateStartTime, CriticalDynamicLimit, _L("Critical dynamic state"));

	//Test for Non Critical state
	result = ProcessState(timerResults, EStartupStateNonCritical, stateStartTime, stateComplTime);
	if(result != EPass && result != EIgnore)
		{
		return TestStepResult();
		}
	accumulatedTime += (stateComplTime - stateStartTime);
	CheckPerformance(stateComplTime - stateStartTime, NonCriticalLimit, _L("Non critical state"));

	//Test for overall time
	INFO_PRINTF2(_L("Ecom Stop Time: %f mSecs\n"), stateComplTime);
	INFO_PRINTF2(_L("Total Elapsed Time: %f mSecs\n"), stateComplTime - ecomStartTime);
	INFO_PRINTF2(_L("Total Accumulated Time: %f mSecs\n"), 	accumulatedTime);
	// CheckPerformance(accumulatedTime, 1000); Test unecessary as individual times that make up accumulated are already tested
	
	// Now get all the heap results from the server
	REComHeapUsageRecords heapResults;
	heapResults.OpenL();	

	//Test for Critical Static state
	TInt CSUsage=0;
	INFO_PRINTF1(_L("Critical Static state"));
	result=ProcessState(heapResults,EStartupStateCriticalStatic,CSUsage);
	if(result != EPass && result != EIgnore)
		{
		return TestStepResult();
		}
	//Test for initialisation
	INFO_PRINTF1(_L("Initialisation state(including Critical Static measurement)"));
	TInt heapUsage=0;
	result=ProcessState(heapResults,-1,heapUsage);
	TInt initialisationHeapUsage=heapUsage-CSUsage;
	if(result != EPass && result != EIgnore)
		{
		return TestStepResult();
		}
	INFO_PRINTF1(_L("Initialisation state(excluding Critical Static measurement)"));	
	INFO_PRINTF3(_L("State: %d heap usage: %d bytes\n"), -1,initialisationHeapUsage);			
					
	//Test for Critical Dynamic state
	INFO_PRINTF1(_L("Critical Dynamic state"));	
	result=ProcessState(heapResults,EStartupStateCriticalDynamic,heapUsage);
	if(result != EPass && result != EIgnore)
		{
		return TestStepResult();
		}	
	
	//Test for Non Critical State
	INFO_PRINTF1(_L("Non Critical sate"));
	result=ProcessState(heapResults,EStartupStateNonCritical,heapUsage);
	if(result != EPass && result != EIgnore)
		{
		return TestStepResult();
		}
				
	heapResults.Close();	
#else
	MacroNotDefinedError();
#endif // __ECOM_SERVER_PERFORMANCE__
	return TestStepResult();
	}
