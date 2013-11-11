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
// For testing Persist HAL :
// 
//

/**
 @file
 @internalComponent
 @test
*/

#include <e32base.h>
#include <hal.h>
#include <bautils.h>
#include <e32test.h>

#define TEST(arg) ::Check((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check(aValue, aExpected, __LINE__)

LOCAL_D RTest 			TheTest (_L ("T_PersistHAL"));

TInt noSample = 25;

LOCAL_C  void Check(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		TheTest(EFalse, aLine);
		}
	}

LOCAL_C void Check(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		RDebug::Print(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		TheTest(EFalse, aLine);
		}
	}

TInt TestPersistHALWaitDirectL()
	{
	RProcess process;
	TEST2(process.Create(_L("HALSettings.exe"), _L("PERSIST")),KErrNone);
	TRequestStatus status;
	process.Logon(status);
	process.Resume();
	User::WaitForRequest(status);
	TInt exitReason = process.ExitReason();
	process.Close();
	return exitReason;
	}

//Tests
//====================================================================

/**
@SYMTestCaseID          SYSLIB-HALSETTINGS-CT-1721
@SYMTestCaseDesc        Performance Test Persist HAL
@SYMTestPriority        Medium
@SYMTestActions         This test trys to persist HAL settings by starting HALSetting.exe
						directly
@SYMTestExpectedResults Tests must not fail
@SYMDEF                 DEF083235: Prop: HAL Attributes (eg screen calibration) lost if the battery is pulled out
*/
void TestPerformancePersistDirectL()
	{
	TheTest.Next (_L (" @SYMTestCaseID:SYSLIB-HALSETTINGS-CT-1721 Performance Persist test directly calling EXE "));
	TInt fastTimerFreq;
	HAL::Get(HALData::EFastCounterFrequency, fastTimerFreq);
	TReal ticksPerMicroSec = 1.0E-6 * fastTimerFreq;

	TUint prevTime;
	TUint timeDiff;

	prevTime = User::FastCounter();
	TInt noSuccessfulSamples=0;
	for(; noSuccessfulSamples < noSample; ++noSuccessfulSamples)
		{
		if(TestPersistHALWaitDirectL() !=KErrNone )
			{
		  	break;//Stop performing the tests, calculate average with no of successful Samples.
		  	}
		}
	TEST(noSuccessfulSamples >0);
	timeDiff = User::FastCounter() - prevTime;
	TReal64 fsSessionMicroSecs = timeDiff / (noSuccessfulSamples * ticksPerMicroSec);
	TheTest.Printf(_L("Time to Persist HAL directly calling exe = %10.2lf microseconds\n"), fsSessionMicroSecs);
	}

/**
@SYMTestCaseID          SYSLIB-HALSETTINGS-CT-1722
@SYMTestCaseDesc        Performance Test Persist HAL through BAFL DLL
@SYMTestPriority        Medium
@SYMTestActions         This test trys to persist HAL settings by calling BAFL
						API
@SYMTestExpectedResults Tests must not fail
@SYMDEF                 DEF083235: Prop: HAL Attributes (eg screen calibration) lost if the battery is pulled out
*/
void TestPerformancePersistThroughBAFL()
	{

	TheTest.Next (_L (" @SYMTestCaseID:SYSLIB-HALSETTINGS-CT-1722 Performance Persist test Through BAFL DLL "));
	TInt fastTimerFreq;
	HAL::Get(HALData::EFastCounterFrequency, fastTimerFreq);
	TReal ticksPerMicroSec = 1.0E-6 * fastTimerFreq;

	TUint prevTime;
	TUint timeDiff;

	prevTime = User::FastCounter();
	TInt noSuccessfulSamples=0;
	for(;noSuccessfulSamples< noSample; ++noSuccessfulSamples)
		{
		if(BaflUtils::PersistHAL()!=KErrNone)
			{
			break;//Stop performing the tests, calculate average with no of successful Samples.
			}
		}
	TEST(noSuccessfulSamples >0);
	timeDiff = User::FastCounter() - prevTime;
	TReal64 fsSessionMicroSecs = timeDiff / (noSuccessfulSamples * ticksPerMicroSec);
	TheTest.Printf(_L("Time to Persist HAL through BAFL DLL = %10.2lf microseconds\n"), fsSessionMicroSecs);
	}

/**
@SYMTestCaseID          SYSLIB-HALSETTINGS-CT-1723
@SYMTestCaseDesc        Capability Test
@SYMTestPriority        Medium
@SYMTestActions         This test exe do not have the capability to initialise HAL settings
						try to initialise HAL setting by starting HALSetting.exe
@SYMTestExpectedResults Tests must not fail
@SYMDEF                 DEF083235: Prop: HAL Attributes (eg screen calibration) lost if the battery is pulled out
*/
void CapabilityTest()
	{
	TheTest.Next (_L (" @SYMTestCaseID:SYSLIB-HALSETTINGS-CT-1723 Capability Test "));
	RProcess process;
	//To initialise processes should have SID == SID of EStart, This process's SID is '0'
	TEST2(process.Create(_L("HALSettings.exe"), _L("INITIALISE")),KErrNone);
	TRequestStatus status;
	process.Logon(status);
	process.Resume();
	User::WaitForRequest(status);
	TInt exitReason = process.ExitReason();
	process.Close();
	TEST2(exitReason,KErrPermissionDenied);
	}

LOCAL_C void RunTestsL()
	{
	TEST2(TestPersistHALWaitDirectL (),KErrNone);
	TEST2(BaflUtils::PersistHAL(),KErrNone);

	CapabilityTest();
	TestPerformancePersistDirectL ();
	TestPerformancePersistThroughBAFL ();
	}

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	TheTest.Title ();
	TheTest.Start (_L ("HAL Persist test"));
	CTrapCleanup* tc = CTrapCleanup::New();

	TRAPD(err, ::RunTestsL());

	delete tc;
	TheTest.End ();
	TheTest.Close ();
	__UHEAP_MARKEND;
	return err;
	}
