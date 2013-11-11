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
// Te_EComPluginReDiscoverPerfTest.cpp
// Implements the performance loggr for ECOM
// 
//

/**
 @file
 @internalComponent
*/

#include "Te_EComPluginReDiscoverPerfTestStep.h"
#include <e32test.h>
#include <bautils.h>
#include <startup.hrh>
#include <ecom/ecom.h>
#include "Interface.h"
#include "EcomTestUtils.h"

#ifdef __ECOM_SERVER_PERFORMANCE__
const TInt KOneSecond = 1000000;

const TReal KNotifierRunLTimeLimitH2RAM = 2;					// mSec
const TReal KTimerRunLScanTwoRSCsOnOneDriveLimitH2RAM = 80;	// mSec
const TReal KTimerRunLScanZeroRSCOnOneDriveLimitH2RAM = 40;	// mSec

const TReal KNotifierRunLTimeLimitH4RAM = 1;               // mSec
const TReal KTimerRunLScanTwoRSCsOnOneDriveLimitH4RAM = 40;   // mSec
const TReal KTimerRunLScanZeroRSCOnOneDriveLimitH4RAM = 20;   // mSec 
		
const TReal KNotifierRunLTimeLimitH4NAND = 6;               // mSec
const TReal KTimerRunLScanTwoRSCsOnOneDriveLimitH4NAND = 50;   // mSec
const TReal KTimerRunLScanZeroRSCOnOneDriveLimitH4NAND = 20;   // mSec 

const TReal KNotifierRunLTimeLimitH6RAM = 1;               // mSec
const TReal KTimerRunLScanTwoRSCsOnOneDriveLimitH6RAM = 20;   // mSec
const TReal KTimerRunLScanZeroRSCOnOneDriveLimitH6RAM = 8;   // mSec 
		
const TReal KNotifierRunLTimeLimitH6NAND = 4;               // mSec
const TReal KTimerRunLScanTwoRSCsOnOneDriveLimitH6NAND = 25;   // mSec
const TReal KTimerRunLScanZeroRSCOnOneDriveLimitH6NAND = 8;   // mSec 

// The default benchmark checking threshold are defined as zero, for emulator platform.
// The values will be changed for H2 or H4 OR H6 boards.
// See CEComPluginReDiscoverPerfTest::SetBenchmarks()
static TReal NotifierRunLTimeLimit = 0;				
static TReal TimerRunLScanTwoRSCsOnOneDriveLimit = 0;	
static TReal TimerRunLScanZeroRSCOnOneDriveLimit = 0;	

_LIT(KRamOnlyEComCR629Example2RSC,			"Z:\\RAMOnly\\EComCR629Example2.rsc");
_LIT(KEComExample12RSCOnZ,						"Z:\\RAMOnly\\EComExample12.rsc");
_LIT(KEComExample12RSC1OnC,						"C:\\Resource\\Plugins\\EComExample12_1.rsc");
_LIT(KEComExample12RSC2OnC,						"C:\\Resource\\Plugins\\EComExample12_2.rsc");
_LIT(KEComRscDirName,						"\\Resource\\Plugins");
_LIT(KEComCR629RSCFileName,					"\\Resource\\Plugins\\EComCR629Example1_");
_LIT(KEComRSCFileExt,						".rsc");
_LIT(KEComAllRSCFilesName,					"\\Resource\\Plugins\\*.rsc");
#endif //__ECOM_SERVER_PERFORMANCE__

CEComPluginReDiscoverPerfTest::CEComPluginReDiscoverPerfTest() : CEComPerfTestBase(KEComPluginReDiscoverPerfTest)
	{
	}

CEComPluginReDiscoverPerfTest::~CEComPluginReDiscoverPerfTest()
	{
	}

#ifdef __ECOM_SERVER_PERFORMANCE__
/*
	Set different benchmark thresholds for H2 or H4 or H6 boards. 
	Emulator tests will not disable benchmark checking, so don't need to set values for it.
**/
void CEComPluginReDiscoverPerfTest::SetBenchmarksL()
	{
	THardwareConfiguration hardware_configuration = EComTestUtils::GetHardwareConfiguration();
	switch (hardware_configuration)
		{
		case EPlatformH2RAM:
	    	NotifierRunLTimeLimit = KNotifierRunLTimeLimitH2RAM;
			TimerRunLScanTwoRSCsOnOneDriveLimit = KTimerRunLScanTwoRSCsOnOneDriveLimitH2RAM;
			TimerRunLScanZeroRSCOnOneDriveLimit = KTimerRunLScanZeroRSCOnOneDriveLimitH2RAM;
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
			NotifierRunLTimeLimit = KNotifierRunLTimeLimitH4RAM;
			TimerRunLScanTwoRSCsOnOneDriveLimit = KTimerRunLScanTwoRSCsOnOneDriveLimitH4RAM;
			TimerRunLScanZeroRSCOnOneDriveLimit = KTimerRunLScanZeroRSCOnOneDriveLimitH4RAM;
			INFO_PRINTF1(_L("Hardware configuration: H4 RAM"));
			break;
		
		case EPlatformH4NAND:		
			NotifierRunLTimeLimit = KNotifierRunLTimeLimitH4NAND;
			TimerRunLScanTwoRSCsOnOneDriveLimit = KTimerRunLScanTwoRSCsOnOneDriveLimitH4NAND;
			TimerRunLScanZeroRSCOnOneDriveLimit = KTimerRunLScanZeroRSCOnOneDriveLimitH4NAND;
			INFO_PRINTF1(_L("Hardware configuration: H4 NAND"));
			break;
		
		case EPlatformH4NANDDP:	
			INFO_PRINTF1(_L("Hardware configuration: H4 NAND DP"));
			break;

		case EPlatformH6RAM:
			NotifierRunLTimeLimit = KNotifierRunLTimeLimitH6RAM;
			TimerRunLScanTwoRSCsOnOneDriveLimit = KTimerRunLScanTwoRSCsOnOneDriveLimitH6RAM;
			TimerRunLScanZeroRSCOnOneDriveLimit = KTimerRunLScanZeroRSCOnOneDriveLimitH6RAM;
			INFO_PRINTF1(_L("Hardware configuration: H6 RAM"));
			break;
		
		case EPlatformH6NAND:		
			NotifierRunLTimeLimit = KNotifierRunLTimeLimitH6NAND;
			TimerRunLScanTwoRSCsOnOneDriveLimit = KTimerRunLScanTwoRSCsOnOneDriveLimitH6NAND;
			TimerRunLScanZeroRSCOnOneDriveLimit = KTimerRunLScanZeroRSCOnOneDriveLimitH6NAND;
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
	}

TBool CEComPluginReDiscoverPerfTest::CheckTimeResults(TReal aTimeLimit, RArray<TEComPerfRealTimeResult>& aTimeResults)
	{
	TInt count = aTimeResults.Count();
	
	if(count)
		{
		switch(aTimeResults[0].iType)
			{
			case ECDiscovererNotifierRunL:
				{
				for(TInt idx = 0; idx < count; ++idx)
					{
					TReal elapsedTime = aTimeResults[idx].iEndTime - aTimeResults[idx].iStartTime;
					INFO_PRINTF2(_L("Notifier RunL gets called for drive: %d"), aTimeResults[idx].iInfo);
					CheckPerformance(elapsedTime, aTimeLimit, _L("Notifier RunL"));
					}
				break;
				}
			case ECDiscovererTimerRunL:
				{
				// Check timer runl only ran once
				if(count>1)
					{
					INFO_PRINTF1(_L("Timer RunL ran more than once!\n"));
					return EFalse;
					}
				else
					{
					const TEComPerfRealTimeResult& timeResult = aTimeResults[0];
					TReal elapsedTime = aTimeResults[0].iEndTime - aTimeResults[0].iStartTime;
					CheckPerformance(elapsedTime, aTimeLimit, _L("Timer RunL"));
					}
				break;
				}
			default:
				{
				INFO_PRINTF1(_L("Unrecognizable type for checking performance!\n"));
				return EFalse;
				}
			}
		return ETrue;
		}
	else // No time result retrieved
		{
		INFO_PRINTF1(_L("No time result Retrieved!\n"));
		return EFalse;
		}
	}

void CEComPluginReDiscoverPerfTest::CopyPluginCR629TestRSCOnDrive(TUint aDriveNum, TInt aNumOfCopies)
	{
	if(aNumOfCopies > 0)
		{
		TInt err=KErrNone;
		TDriveUnit aDrive(aDriveNum);
		TBuf<256> resourceFileName;
		for(TInt num = 1; num<= aNumOfCopies; num++ )
			{
			resourceFileName.Zero();
			resourceFileName.Append(aDrive.Name());
			resourceFileName.Append(KEComCR629RSCFileName);
			resourceFileName.AppendNum(num);
			resourceFileName.Append(KEComRSCFileExt);
			TRAP(err, EComTestUtils::FileManCopyFileL(KRamOnlyEComCR629Example2RSC, resourceFileName));
			TEST(err ==KErrNone);
			}
		}
	else
		{
		INFO_PRINTF1(_L("Number of file copies should be greater than 0!\n"));
		}
	}

void CEComPluginReDiscoverPerfTest::DeleteRSCFilesOnDrive(TUint aDriveNum)
	{
	TInt err=KErrNone;
	TDriveUnit aDrive(aDriveNum);

	TBuf<256> resourceFileName;
	resourceFileName.Append(aDrive.Name());
	resourceFileName.Append(KEComAllRSCFilesName);
	TRAP(err, EComTestUtils::FileManDeleteFileL(resourceFileName));
	}

void CEComPluginReDiscoverPerfTest::DeleteRSCFolderOnDrive(TUint aDriveNum)
	{
	TInt err=KErrNone;
	TDriveUnit aDrive(aDriveNum);

	TBuf<256> resourceFileName;
	resourceFileName.Append(aDrive.Name());
	resourceFileName.Append(KEComAllRSCFilesName);
	TRAP(err, EComTestUtils::FileManDeleteFileL(resourceFileName));

	TBuf<256> resourceDirName;
	resourceDirName.Append(aDrive.Name());
	resourceDirName.Append(KEComRscDirName);
	TRAP(err, EComTestUtils::FileManDeleteDirL(resourceDirName));
	}

#endif //__ECOM_SERVER_PERFORMANCE__

/*
	Test cases covered in the test step:
	1. Multiple notification processing on single drive (C:)

		1.1 Rediscovery on adding RSC files
				Performance check on Notifier RunL
				Functionality check on Notifier RunL
				Performance check on Timer RunL
				Functionality check on RediscoveryScanDirectoryL

		1.2 Rediscovery on deleting RSC files
				As above.
		1.3 Rediscovery on adding RSC PlugIn3 files. It does the performance test 
			for parsing new registry data.
				As above.
				
	2. Multiple notification processing on multiple drives (C: and X: or E:)

		2.1 Rediscovery on adding RSC files
				As above.
				
		2.2 Rediscovery on deleting RSC filles
				As above.
				
	3. MMC Remounting tests on drive E:

		3.1 Before: has plugins
			After: 	has plugins
				As above.
				
		3.2 Before: has plugins
			After: 	has no plugins
				As above.
				
		3.3 Before: has no plugins
			After: 	has plugins
				As above.
				
		3.4 Before: has no plugins
			After: 	has no plugins
				As above.

@pre For hardware test, MMC (E:) drive should be available.
@return EPass if the test succeeds and the measured times pass the checks
		EFail otherwise.
 */
TVerdict CEComPluginReDiscoverPerfTest::doTestStepL()
	{
#ifdef __ECOM_SERVER_PERFORMANCE__
	// Set benchmark threshold for different hardware platform, default values are 0s, for emulator.
	SetBenchmarksL();
	
	//1. Multiple notification processing on single drive (C:)
	INFO_PRINTF1(_L("1. Multiple notification processing on single drive (C:)"));		
	
	//1.1 Testing rediscovery performance on adding RSC files on C:
		INFO_PRINTF1(_L("1.1 Testing rediscovery on adding RSC files onto drive C:"));
		// Clean up Plugins
		DeleteRSCFilesOnDrive(EDriveC);
		User::After(KOneSecond * 3);
		REComPerfTimeRecords timeRecords;

		// Clean up time records on server.
		TRAPD(err, timeRecords.ResetRecordsOnServerL());
		TEST(err==KErrNone);
		timeRecords.Reset();
		CopyPluginCR629TestRSCOnDrive(EDriveC, 2);
		User::After(KOneSecond * 3);
		
		// Now get all time records from server
		TRAP(err, timeRecords.OpenL());
		TEST(err==KErrNone);
		TEST(timeRecords.Count() > 0);
		
		RArray<TEComPerfRealTimeResult> timeResults;
		TRAP(err, timeRecords.RetrieveResultsByTypeL(ECDiscovererNotifierRunL, timeResults));
		TEST(err==KErrNone);
		// Test notifications were triggered and check performance of RunL of Notifiers
		TEST(CheckTimeResults(NotifierRunLTimeLimit, timeResults));
		timeResults.Reset();
		
		RArray<TInt> timeResultsInfos;
		TRAP(err, timeRecords.RetrieveResultsInfoByTypeL(ECDiscovererNotifierRunL, timeResultsInfos));
		TEST(err==KErrNone);
		// Test notifications ONLY triggered on C: drive
		TEST(timeResultsInfos.Count()==1);
		TEST(timeResultsInfos[0] == EDriveC);
		timeResultsInfos.Reset();
		
		TRAP(err, timeRecords.RetrieveResultsByTypeL(ECDiscovererTimerRunL, timeResults));
		TEST(err==KErrNone);
		// Test Timer RunL ran only once and check performance.
		TEST(CheckTimeResults(TimerRunLScanTwoRSCsOnOneDriveLimit, timeResults));
		timeResults.Reset();
		
		TRAP(err, timeRecords.RetrieveResultsInfoByTypeL(ECDiscovererRediscoveryScanDirectoryL, timeResultsInfos));
		TEST(err==KErrNone);
		// Test directory scanning has and ONLY performed on C: drive
		TEST(timeResultsInfos.Count()==1);
		TEST(timeResultsInfos[0] == EDriveC);
		timeResultsInfos.Reset();

	//1.2 Testing rediscovery performance on deleting RSC files
		INFO_PRINTF1(_L("1.2 Testing rediscovery on removing RSC files from drive C:"));
		// Clean up time records on server.
		TRAP(err, timeRecords.ResetRecordsOnServerL());
		TEST(err==KErrNone);
		timeRecords.Reset();
		DeleteRSCFilesOnDrive(EDriveC);
		User::After(KOneSecond * 3);
		
		// Now get all time records from server.
		TRAP(err, timeRecords.OpenL());
		TEST(err==KErrNone);
		TEST(timeRecords.Count() > 0);
		
		TRAP(err, timeRecords.RetrieveResultsByTypeL(ECDiscovererNotifierRunL, timeResults));
		TEST(err==KErrNone);
		// Test notifications were triggered and check performance of RunL of Notifiers
		TEST(CheckTimeResults(NotifierRunLTimeLimit, timeResults));
		timeResults.Reset();
		
		TRAP(err, timeRecords.RetrieveResultsInfoByTypeL(ECDiscovererNotifierRunL, timeResultsInfos));
		TEST(err==KErrNone);
		// Test notifications ONLY triggered on C: drive
		TEST(timeResultsInfos.Count()==1);
		TEST(timeResultsInfos[0] == EDriveC);
		timeResultsInfos.Reset();
		
		TRAP(err, timeRecords.RetrieveResultsByTypeL(ECDiscovererTimerRunL, timeResults));
		TEST(err==KErrNone);
		// Test Timer RunL ran only once and check performance.
		TEST(CheckTimeResults(TimerRunLScanZeroRSCOnOneDriveLimit, timeResults));
		timeResults.Reset();
		
		TRAP(err, timeRecords.RetrieveResultsInfoByTypeL(ECDiscovererRediscoveryScanDirectoryL, timeResultsInfos));
		TEST(err==KErrNone);
		// Test directory scanning has and ONLY performed on C: drive
		TEST(timeResultsInfos.Count()==1);
		TEST(timeResultsInfos[0] == EDriveC);
		timeResultsInfos.Reset();

	//1.3 Testing rediscovery performance on adding RSC PLUGIN3 files on C:
		INFO_PRINTF1(_L("1.3 Testing rediscovery on adding RSC PLUGIN3 files onto drive C:"));
		// Clean up Plugins
		DeleteRSCFilesOnDrive(EDriveC);
		User::After(KOneSecond * 3);
		
		// Clean up time records on server.
		TRAP(err, timeRecords.ResetRecordsOnServerL());
		TEST(err==KErrNone);
		timeRecords.Reset();
		// Adding RSC PLUGIN3 file twice
		TRAP(err, EComTestUtils::FileManCopyFileL(KEComExample12RSCOnZ, KEComExample12RSC1OnC));
		TEST(err==KErrNone);
		TRAP(err, EComTestUtils::FileManCopyFileL(KEComExample12RSCOnZ, KEComExample12RSC2OnC));
		TEST(err==KErrNone);
		User::After(KOneSecond * 3);
		
		// Now get all time records from server
		TRAP(err, timeRecords.OpenL());
		TEST(err==KErrNone);
		TEST(timeRecords.Count() > 0);
		
		TRAP(err, timeRecords.RetrieveResultsByTypeL(ECDiscovererNotifierRunL, timeResults));
		TEST(err==KErrNone);
		// Test notifications were triggered and check performance of RunL of Notifiers
		TEST(CheckTimeResults(NotifierRunLTimeLimit, timeResults));
		timeResults.Reset();
		
		TRAP(err, timeRecords.RetrieveResultsInfoByTypeL(ECDiscovererNotifierRunL, timeResultsInfos));
		TEST(err==KErrNone);
		// Test notifications ONLY triggered on C: drive
		TEST(timeResultsInfos.Count()==1);
		TEST(timeResultsInfos[0] == EDriveC);
		timeResultsInfos.Reset();
		
		TRAP(err, timeRecords.RetrieveResultsByTypeL(ECDiscovererTimerRunL, timeResults));
		TEST(err==KErrNone);
		// Test Timer RunL ran only once and check performance.
		TEST(CheckTimeResults(TimerRunLScanTwoRSCsOnOneDriveLimit, timeResults));
		timeResults.Reset();
		
		TRAP(err, timeRecords.RetrieveResultsInfoByTypeL(ECDiscovererRediscoveryScanDirectoryL, timeResultsInfos));
		TEST(err==KErrNone);
		// Test directory scanning has and ONLY performed on C: drive
		TEST(timeResultsInfos.Count()==1);
		TEST(timeResultsInfos[0] == EDriveC);
		timeResultsInfos.Reset();
		// Delete the RSC files
		TRAP(err, EComTestUtils::FileManDeleteFileL(KEComExample12RSC1OnC));
		TRAP(err, EComTestUtils::FileManDeleteFileL(KEComExample12RSC2OnC));
		User::After(KOneSecond * 3);
		
	//2. Multiple notification processing on multiple drives (C: and X: or E:)
		RFs fs;
		TEST(fs.Connect()==KErrNone);

	//2.1 Testing rediscovery performance on adding RSC files on multiple drives
		// Clean up time records on server.
		TRAP(err, timeRecords.ResetRecordsOnServerL());
		TEST(err==KErrNone);
		timeRecords.Reset();
		CopyPluginCR629TestRSCOnDrive(EDriveC, 2);
#if defined(__WINSCW__) // C: and X: on emulator
		INFO_PRINTF1(_L("2. Multiple notification processing on multiple drives (C: and X:)"));
		INFO_PRINTF1(_L("2.1 Testing rediscovery on adding RSC files onto drive C: and X:"));
		CopyPluginCR629TestRSCOnDrive(EDriveX, 2);
#else // C: and E: on hardware
		INFO_PRINTF1(_L("2. Multiple notification processing on multiple drives (C: and E:)"));
		INFO_PRINTF1(_L("2.1 Testing rediscovery on adding RSC files onto drive C: and E:"));
		
		// Check the availability of E: drive and MMC card.
		TDriveList drivelist;
		TEST(fs.DriveList(drivelist)==KErrNone);
		if (!(drivelist[EDriveE]))
			{
			SetTestStepResult(EFail);
			INFO_PRINTF1(_L("*** Drive E: is not available for hardware tests! ***"));
			DeleteRSCFilesOnDrive(EDriveC);
			return TestStepResult();
			}
		
		TDriveInfo aDriveInfo;
		TEST(fs.Drive(aDriveInfo, EDriveE)==KErrNone);
		if (aDriveInfo.iType != EMediaHardDisk)
			{
			SetTestStepResult(EFail);
			INFO_PRINTF1(_L("*** MMC card is not available for hardware tests! ***"));
			DeleteRSCFilesOnDrive(EDriveC);
			return TestStepResult();
			}		
		
		// Now copy files
		CopyPluginCR629TestRSCOnDrive(EDriveE, 2);
#endif // __WINSCW__
		User::After(KOneSecond * 3);
		
		// Now get all time records from server.
		TRAP(err, timeRecords.OpenL());
		TEST(err==KErrNone);
		TEST(timeRecords.Count() > 0);
		
		TRAP(err, timeRecords.RetrieveResultsByTypeL(ECDiscovererNotifierRunL, timeResults));
		TEST(err==KErrNone);
		// Test notifications were triggered and check performance of RunL of Notifiers
		TEST(CheckTimeResults(NotifierRunLTimeLimit, timeResults));
		timeResults.Reset();
		
		TRAP(err, timeRecords.RetrieveResultsInfoByTypeL(ECDiscovererNotifierRunL, timeResultsInfos));
		TEST(err==KErrNone);
		// Test notifications ONLY triggered on C: and X: (or E:) drive
		TEST(timeResultsInfos.Count()==2);
		TEST(timeResultsInfos.Find(EDriveC)!=KErrNotFound);
#if defined(__WINSCW__) 
		TEST(timeResultsInfos.Find(EDriveX)!=KErrNotFound);
#else 
		TEST(timeResultsInfos.Find(EDriveE)!=KErrNotFound);
#endif // __WINSCW__
		timeResultsInfos.Reset();
		
		TRAP(err, timeRecords.RetrieveResultsByTypeL(ECDiscovererTimerRunL, timeResults));
		TEST(err==KErrNone);
		// Test Timer RunL ran only once and check performance.
		TEST(CheckTimeResults(TimerRunLScanTwoRSCsOnOneDriveLimit * 2, timeResults));
		timeResults.Reset();
		
		TRAP(err, timeRecords.RetrieveResultsInfoByTypeL(ECDiscovererRediscoveryScanDirectoryL, timeResultsInfos));
		TEST(err==KErrNone);
		// Test directory scanning has and ONLY performed on C: and X: (or E:) drive
		TEST(timeResultsInfos.Count()==2);
		TEST(timeResultsInfos.Find(EDriveC)!=KErrNotFound);
#if defined(__WINSCW__) 
		TEST(timeResultsInfos.Find(EDriveX)!=KErrNotFound);
#else 
		TEST(timeResultsInfos.Find(EDriveE)!=KErrNotFound);
#endif // __WINSCW__
		timeResultsInfos.Reset();
		
		
	//2.2 Testing rediscovery performance on deleting RSC files on multiple drives
		// Clean up time records on server.
		TRAP(err, timeRecords.ResetRecordsOnServerL());
		TEST(err==KErrNone);
		timeRecords.Reset();
		DeleteRSCFilesOnDrive(EDriveC);
#if defined(__WINSCW__) // C: and X: on emulator
		INFO_PRINTF1(_L("2.2 Testing rediscovery on removing RSC files from drive C: and X:"));
		DeleteRSCFolderOnDrive(EDriveX);
#else // C: and E: on hardware
		INFO_PRINTF1(_L("2.2 Testing rediscovery on removing RSC files from drive C: and E:"));
		DeleteRSCFolderOnDrive(EDriveE);
#endif // __WINSCW__
		User::After(KOneSecond * 3);
		
		// Now get all time records from server.
		TRAP(err, timeRecords.OpenL());
		TEST(err==KErrNone);
		TEST(timeRecords.Count() > 0);
		
		TRAP(err, timeRecords.RetrieveResultsByTypeL(ECDiscovererNotifierRunL, timeResults));
		TEST(err==KErrNone);
		// Test notifications were triggered and check performance of RunL of Notifiers
		TEST(CheckTimeResults(NotifierRunLTimeLimit, timeResults));
		timeResults.Reset();
		
		TRAP(err, timeRecords.RetrieveResultsInfoByTypeL(ECDiscovererNotifierRunL, timeResultsInfos));
		TEST(err==KErrNone);
		// Test notifications ONLY triggered on C: and X: (or E:) drive
		TEST(timeResultsInfos.Count()==2);
		TEST(timeResultsInfos.Find(EDriveC)!=KErrNotFound);
#if defined(__WINSCW__) 
		TEST(timeResultsInfos.Find(EDriveX)!=KErrNotFound);
#else 
		TEST(timeResultsInfos.Find(EDriveE)!=KErrNotFound);
#endif // __WINSCW__
		timeResultsInfos.Reset();
		
		TRAP(err, timeRecords.RetrieveResultsByTypeL(ECDiscovererTimerRunL, timeResults));
		TEST(err==KErrNone);
		// Test Timer RunL ran only once and check performance.
		TEST(CheckTimeResults(TimerRunLScanZeroRSCOnOneDriveLimit * 2, timeResults));
		timeResults.Reset();
		
		TRAP(err, timeRecords.RetrieveResultsInfoByTypeL(ECDiscovererRediscoveryScanDirectoryL, timeResultsInfos));
		TEST(err==KErrNone);
		// Test directory scanning has and ONLY performed on C: and X: (or E:) drive
		TEST(timeResultsInfos.Count()==2);
		TEST(timeResultsInfos.Find(EDriveC)!=KErrNotFound);
#if defined(__WINSCW__) 
		TEST(timeResultsInfos.Find(EDriveX)!=KErrNotFound);
#else 
		TEST(timeResultsInfos.Find(EDriveE)!=KErrNotFound);
#endif // __WINSCW__
		timeResultsInfos.Reset();


	//3. MMC Remounting tests on drive E:
	INFO_PRINTF1(_L("3. MMC Remounting tests on drive E:"));
#if defined(__WINSCW__)
	// Tests only run on hardware.
	INFO_PRINTF1(_L("This test can only run on hardware, please see test results of hardware for further information."));
#else
		CopyPluginCR629TestRSCOnDrive(EDriveE, 2);
		User::After(KOneSecond * 3);
		
	//3.1 before remounting: 	has plugins
	//    after remounting: 	has plugins
		INFO_PRINTF1(_L("3.1 Before Remount: has plugins; 	After Remount: has plugins"));
		// Clean up time records on server.
		TRAP(err, timeRecords.ResetRecordsOnServerL());
		TEST(err==KErrNone);
		timeRecords.Reset();
		TEST(fs.RemountDrive(EDriveE)==KErrNone);
		User::After(KOneSecond * 3);
		
		// Now get all time records from server.
		TRAP(err, timeRecords.OpenL());
		TEST(err==KErrNone);
		TEST(timeRecords.Count() > 0);

		TRAP(err, timeRecords.RetrieveResultsByTypeL(ECDiscovererNotifierRunL, timeResults));
		TEST(err==KErrNone);
		// Test notifications were triggered and check performance of RunL of Notifiers
		TEST(CheckTimeResults(NotifierRunLTimeLimit, timeResults));
		timeResults.Reset();

		TRAP(err, timeRecords.RetrieveResultsInfoByTypeL(ECDiscovererNotifierRunL, timeResultsInfos));
		// Test notifications ONLY triggered on E: drive
		TEST(timeResultsInfos.Count()==1);
		TEST(timeResultsInfos[0] == EDriveE);
		timeResultsInfos.Reset();
		
		TRAP(err, timeRecords.RetrieveResultsByTypeL(ECDiscovererTimerRunL, timeResults));
		TEST(err==KErrNone);
		// Test Timer RunL ran only once and check performance.
		TEST(CheckTimeResults(TimerRunLScanTwoRSCsOnOneDriveLimit, timeResults));
		timeResults.Reset();
		
		TRAP(err, timeRecords.RetrieveResultsInfoByTypeL(ECDiscovererRediscoveryScanDirectoryL, timeResultsInfos));
		TEST(err==KErrNone);
		// Test directory scanning has and ONLY performed on E: drive
		TEST(timeResultsInfos.Count()==1);
		TEST(timeResultsInfos[0] == EDriveE);
		timeResultsInfos.Reset();

	//3.2 before remounting: 	has plugins
	//    after remounting: 	has no plugins
		INFO_PRINTF1(_L("3.2 Before Remount: has plugins; 	After Remount: has no plugins"));
		// Clean up time records on server.
		TRAP(err, timeRecords.ResetRecordsOnServerL());
		TEST(err==KErrNone);
		timeRecords.Reset();
		DeleteRSCFolderOnDrive(EDriveE);
		TEST(fs.RemountDrive(EDriveE)==KErrNone);
		User::After(KOneSecond * 3);
		
		// Now get all time records from server.
		TRAP(err, timeRecords.OpenL());
		TEST(err==KErrNone);
		TEST(timeRecords.Count() > 0);
		
		TRAP(err, timeRecords.RetrieveResultsByTypeL(ECDiscovererNotifierRunL, timeResults));
		TEST(err==KErrNone);
		// Test notifications were triggered and check performance of RunL of Notifiers
		TEST(CheckTimeResults(NotifierRunLTimeLimit, timeResults));
		timeResults.Reset();
		
		TRAP(err, timeRecords.RetrieveResultsInfoByTypeL(ECDiscovererNotifierRunL, timeResultsInfos));
		TEST(err==KErrNone);
		// Test notifications ONLY triggered for E: drive
		TEST(timeResultsInfos.Count()==1);
		TEST(timeResultsInfos[0] == EDriveE);
		timeResultsInfos.Reset();
		
		TRAP(err, timeRecords.RetrieveResultsByTypeL(ECDiscovererTimerRunL, timeResults));
		TEST(err==KErrNone);
		// Test Timer RunL ran only once and check performance.
		TEST(CheckTimeResults(TimerRunLScanZeroRSCOnOneDriveLimit, timeResults));
		timeResults.Reset();
		
		TRAP(err, timeRecords.RetrieveResultsInfoByTypeL(ECDiscovererRediscoveryScanDirectoryL, timeResultsInfos));
		TEST(err==KErrNone);
		// Test directory scanning has and ONLY performed on E: drive
		TEST(timeResultsInfos.Count()==1);
		TEST(timeResultsInfos[0] == EDriveE);
		timeResultsInfos.Reset();

	//3.3 before remounting: 	has no plugins
	//    after remounting: 	has plugins
		INFO_PRINTF1(_L("3.3 Before Remount: has no plugins; 	After Remount: has plugins"));
		// Clean up time records on server.
		TRAP(err, timeRecords.ResetRecordsOnServerL());
		TEST(err==KErrNone);
		timeRecords.Reset();
		CopyPluginCR629TestRSCOnDrive(EDriveE, 2);
		TEST(fs.RemountDrive(EDriveE)==KErrNone);
		User::After(KOneSecond * 3);
		
		// Now get all time records from server.
		TRAP(err, timeRecords.OpenL());
		TEST(err==KErrNone);
		TEST(timeRecords.Count() > 0);
		
		TRAP(err, timeRecords.RetrieveResultsByTypeL(ECDiscovererNotifierRunL, timeResults));
		TEST(err==KErrNone);
		// Test notifications were triggered and check performance of RunL of Notifiers
		TEST(CheckTimeResults(NotifierRunLTimeLimit, timeResults));
		timeResults.Reset();
		
		TRAP(err, timeRecords.RetrieveResultsInfoByTypeL(ECDiscovererNotifierRunL, timeResultsInfos));
		TEST(err==KErrNone);
		// Test notifications ONLY triggered for E: drive
		TEST(timeResultsInfos.Count()==1);
		TEST(timeResultsInfos[0] == EDriveE);
		timeResultsInfos.Reset();
		
		TRAP(err, timeRecords.RetrieveResultsByTypeL(ECDiscovererTimerRunL, timeResults));
		TEST(err==KErrNone);
		// Test Timer RunL ran only once and check performance.
		TEST(CheckTimeResults(TimerRunLScanTwoRSCsOnOneDriveLimit, timeResults));
		timeResults.Reset();
		
		TRAP(err, timeRecords.RetrieveResultsInfoByTypeL(ECDiscovererRediscoveryScanDirectoryL, timeResultsInfos));
		TEST(err==KErrNone);
		// Test directory scanning has and ONLY performed on E: drive
		TEST(timeResultsInfos.Count()==1);
		TEST(timeResultsInfos[0] == EDriveE);
		timeResultsInfos.Reset();

		DeleteRSCFolderOnDrive(EDriveE);
		User::After(KOneSecond * 3);

	//3.4 before remounting: 	has no plugin
	//    after remounting: 	has no plugin
		INFO_PRINTF1(_L("3.4 Before Remount: has no plugins; 	After Remount: has no plugins"));
		// Clean up time records on server.
		TRAP(err, timeRecords.ResetRecordsOnServerL());
		TEST(err==KErrNone);
		timeRecords.Reset();
		TEST(fs.RemountDrive(EDriveE)==KErrNone);
		User::After(KOneSecond * 3);
		
		// Now get all time records from server.
		TRAP(err, timeRecords.OpenL());
		TEST(err==KErrNone);
		TEST(timeRecords.Count() > 0);
		
		TRAP(err, timeRecords.RetrieveResultsByTypeL(ECDiscovererNotifierRunL, timeResults));
		TEST(err==KErrNone);
		// Test notifications were triggered and check performance of RunL of Notifiers
		TEST(CheckTimeResults(NotifierRunLTimeLimit, timeResults));
		timeResults.Reset();
		
		TRAP(err, timeRecords.RetrieveResultsInfoByTypeL(ECDiscovererNotifierRunL, timeResultsInfos));
		TEST(err==KErrNone);
		// Test notifications ONLY triggered for E: drive
		TEST(timeResultsInfos.Count()==1);
		TEST(timeResultsInfos[0] == EDriveE);
		timeResultsInfos.Reset();
		
		TRAP(err, timeRecords.RetrieveResultsByTypeL(ECDiscovererTimerRunL, timeResults));
		TEST(err==KErrNone);
		// Test Timer RunL ran only once and check performance.
		TEST(CheckTimeResults(TimerRunLScanZeroRSCOnOneDriveLimit, timeResults));
		timeResults.Reset();
		
		TRAP(err, timeRecords.RetrieveResultsInfoByTypeL(ECDiscovererRediscoveryScanDirectoryL, timeResultsInfos));
		TEST(err==KErrNone);
		// Test directory scanning has NOT performed
		TEST(timeResultsInfos.Count()==0);
		timeResultsInfos.Reset();
		
#endif // __WINSCW__
		
	// Final cleanup
	DeleteRSCFilesOnDrive(EDriveC);
	DeleteRSCFolderOnDrive(EDriveX);
	DeleteRSCFolderOnDrive(EDriveE);
#else
	MacroNotDefinedError();
#endif // __ECOM_SERVER_PERFORMANCE__

	return TestStepResult();
	}
