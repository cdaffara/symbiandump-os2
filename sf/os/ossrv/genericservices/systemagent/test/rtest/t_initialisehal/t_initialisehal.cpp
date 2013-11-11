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
// For testing initialise HAL :
// 
//

/**
 @file
 @internalComponent
 @test
*/

#include <e32base.h>
#include <hal.h>
#include <f32file.h>
#include <bautils.h>
#include <e32test.h>

_LIT(KHALSettingsExeName, "HALSettings.exe");
_LIT(KInitialiseLocaleExeName, "InitialiseLocale.exe");

// Symbian VendorID as specified in 
// "Platform security engineering guide" Section 2.1.3.2 
const TVendorId KSymbianVendorId(0x70000001);

#define TEST(arg) ::Check((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check(aValue, aExpected, __LINE__)

//Number of sample used
TInt noSample = 25;
LOCAL_D RTest 			TheTest (_L ("T_InitialiseHAL"));

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

/**
 * This function will start HALSettings.exe with invalid argument and wait for finish
 * It is used for base line performance overhead
 */
TInt StartEmptyExeL()
	{
	RProcess process;
	//With command line length more than 10 HALSettings.exe returns
	//immediately with error KErrArgument, Used to baseline
	//performance overhead to run an empty exe
	TEST2(process.Create(KHALSettingsExeName,_L("Length is more than ten")),KErrNone);
	TRequestStatus status;
	process.Logon(status);
	process.Resume();
	User::WaitForRequest(status);
	TInt exitReason = process.ExitReason();
	process.Close();
	return exitReason;
	}

/**
 * Initialise HAL settings from HAL.DAT
 * This test will start the HALSettings.exe and wait to finish
 */
TInt TestInitialiseHALWaitL()
	{
	RProcess process;
	TEST2(process.Create(KHALSettingsExeName, _L("INITIALISE")),KErrNone);
	TRequestStatus status;
	process.Logon(status);
	process.Resume();
	User::WaitForRequest(status);
	TInt exitReason = process.ExitReason();
	process.Close();
	return exitReason;
	}

/**
 * Persist HAL settings to HAL.DAT
 * It will create a new HAL.DAT
 */
TInt TestPersistHALWaitL()
	{
	RProcess process;
	TEST2(process.Create(KHALSettingsExeName, _L("PERSIST")),KErrNone);
	TRequestStatus status;
	process.Logon(status);
	process.Resume();
	User::WaitForRequest(status);
	TInt exitReason = process.ExitReason();
	process.Close();
	return exitReason;
	}

//Tests
//===================================================================
/**
@SYMTestCaseID          SYSLIB-HALSETTINGS-CT-1712
@SYMTestCaseDesc        Initialise HAL settings With wrong commandline
@SYMTestPriority        Medium
@SYMTestActions         Start HALSettings.exe and returns KErrArgument
@SYMTestExpectedResults Tests must not fail
@SYMDEF                 DEF083235: Prop: HAL Attributes (eg screen calibration) lost if the battery is pulled out
*/
void TestInitialiseHALWrongParameterL()
	{
	TheTest.Next (_L (" @SYMTestCaseID:SYSLIB-HALSETTINGS-CT-1712 Try to Initialise With wrong commandline "));
	RProcess process;
	TEST2(process.Create(KHALSettingsExeName, _L("ABCD")),KErrNone);
	TRequestStatus status;
	process.Logon(status);
	process.Resume();
	User::WaitForRequest(status);
	TInt exitReason = process.ExitReason();
	process.Close();
	TEST2(exitReason, KErrArgument);
	}

/**
@SYMTestCaseID          SYSLIB-HALSETTINGS-CT-1713
@SYMTestCaseDesc        Missing HAL.DAT File
@SYMTestPriority        Medium
@SYMTestActions         This function first delete hal.dat and then try to initialise HAL
@SYMTestExpectedResults Tests must not fail
@SYMDEF                 DEF083235: Prop: HAL Attributes (eg screen calibration) lost if the battery is pulled out
*/
void TestHALFileMissingL()
	{
	TheTest.Next (_L (" @SYMTestCaseID:SYSLIB-HALSETTINGS-CT-1713 HAL.DAT file missing "));
	_LIT(KFileName,"c:\\private\\102825B1\\hal.dat");
	RFs fs;
	fs.Connect();
	TInt err = BaflUtils::DeleteFile(fs,KFileName);
	if(err == KErrNone || err == KErrNotFound )
		{
		TEST2(TestInitialiseHALWaitL(),KErrNotFound);
		}
	fs.Close();
	}

/**
@SYMTestCaseID          SYSLIB-HALSETTINGS-CT-1724
@SYMTestCaseDesc        No Access HAL.DAT File
@SYMTestPriority        Medium
@SYMTestActions         This function first open hal.dat and then try to initialise HAL
@SYMTestExpectedResults Tests must not fail
@SYMDEF                 DEF083235: Prop: HAL Attributes (eg screen calibration) lost if the battery is pulled out
*/
void TestHALFileNoAccessL()
	{
	TheTest.Next (_L (" @SYMTestCaseID:SYSLIB-HALSETTINGS-CT-1724 HAL.DAT No Access "));
	//To create hal.dat
 	TEST2(TestPersistHALWaitL(),KErrNone);
	_LIT(KFileName,"c:\\private\\102825B1\\hal.dat");
	RFs fs;
	fs.Connect();
	RFile file;
	TInt err = file.Open(fs,KFileName,EFileShareExclusive);
	if(err == KErrNone )
		{
		TEST2(TestInitialiseHALWaitL(),KErrInUse);
		}
	file.Close();
	fs.Close();
	}

/**
@SYMTestCaseID          SYSLIB-HALSETTINGS-CT-1714
@SYMTestCaseDesc        HAL.DAT File is Empty
@SYMTestPriority        Medium
@SYMTestActions         This function first create an empty hal.dat and then try to initialise HAL
@SYMTestExpectedResults Tests must not fail
@SYMDEF                 DEF083235: Prop: HAL Attributes (eg screen calibration) lost if the battery is pulled out
*/
void TestHALFileEmptyL()
	{
	TheTest.Next (_L (" @SYMTestCaseID:SYSLIB-HALSETTINGS-CT-1714 HAL.DAT file Empty "));
	_LIT(KFileName,"c:\\private\\102825B1\\hal.dat");
	RFs fs;
	fs.Connect();
	RFile file;
	TInt err = file.Replace(fs,KFileName,EFileRead);
	file.Close();

	if(err == KErrNone )
		{
		TEST2(TestInitialiseHALWaitL(), KErrCorrupt);
		}
	fs.Close();
	}

/**
@SYMTestCaseID          SYSLIB-HALSETTINGS-CT-1715
@SYMTestCaseDesc        Wrong Header
@SYMTestPriority        Medium
@SYMTestActions         This function first corrupt hal.dat by adding wrong header and then
 						try to initialise HAL
@SYMTestExpectedResults Tests must not fail
@SYMDEF                 DEF083235: Prop: HAL Attributes (eg screen calibration) lost if the battery is pulled out
*/
void TestHALFileWrongHeaderL()
	{
	TheTest.Next (_L (" @SYMTestCaseID:SYSLIB-HALSETTINGS-CT-1715 HAL.DAT wrong header "));
	_LIT(KFileName,"c:\\private\\102825B1\\hal.dat");
	RFs fs;
	fs.Connect();
	RFile file;
	TInt err = file.Replace(fs,KFileName,EFileRead| EFileWrite);

	//write data in multiples of 8 otherwise it will not pass the check (size&7) == 0
	file.Write(_L8("abcdefghijklmnop"));

	file.Close();
	fs.Close();

	if(err == KErrNone )
		{
		TEST2(TestInitialiseHALWaitL(), KErrCorrupt);
		}
	}

//Performance Tests
//======================================================================================
/**
@SYMTestCaseID          SYSLIB-HALSETTINGS-CT-1718
@SYMTestCaseDesc        Performance Test Initialise with file
@SYMTestPriority        Medium
@SYMTestActions         Initialise HAL settings with  HAL.DAT present
@SYMTestExpectedResults Tests must not fail
@SYMDEF                 DEF083235: Prop: HAL Attributes (eg screen calibration) lost if the battery is pulled out
*/
void TestPerformanceInitialiseWithFileL()
	{
	TheTest.Next (_L (" @SYMTestCaseID:SYSLIB-HALSETTINGS-CT-1718 Performance Test for initialise hal.dat present "));

	_LIT(KFileName,"c:\\private\\102825B1\\hal.dat");
	RFs fs;
	fs.Connect();
	//to create hal.dat if it is not there
	TEST2(TestPersistHALWaitL(),KErrNone);
	TEST(BaflUtils::FileExists(fs,KFileName));
	TInt fastTimerFreq;
	HAL::Get(HALData::EFastCounterFrequency, fastTimerFreq);
	TReal ticksPerMicroSec = 1.0E-6 * fastTimerFreq;
	TUint prevTime;
	TUint timeDiff;

	prevTime = User::FastCounter();
	TInt noSuccessfulSamples=0;
	for(;noSuccessfulSamples < noSample; ++noSuccessfulSamples)
		{
		if(TestInitialiseHALWaitL() != KErrNone)
			{
		  	break; //Stop performing the tests, calculate average with no of successful Samples.
		  	}
		//else continue;
		}
	TEST(noSuccessfulSamples >0);
	timeDiff = User::FastCounter() - prevTime;
	TReal64 fsSessionMicroSecs = timeDiff / (noSuccessfulSamples * ticksPerMicroSec);
	TheTest.Printf(_L("Time to Initialise HAL (with file) = %10.2lf microseconds\n"), fsSessionMicroSecs);
	fs.Close();
	}
/**
@SYMTestCaseID          SYSLIB-HALSETTINGS-CT-1719
@SYMTestCaseDesc        Performance Test Initialise with no file
@SYMTestPriority        Medium
@SYMTestActions         Initialise HAL settings with no HAL.DAT present
@SYMTestExpectedResults Tests must not fail
@SYMDEF                 DEF083235: Prop: HAL Attributes (eg screen calibration) lost if the battery is pulled out
*/
void TestPerformanceInitialiseNoFileL()
	{
	TheTest.Next (_L (" @SYMTestCaseID:SYSLIB-HALSETTINGS-CT-1719 Performance Test for initialise with out hal.dat "));

	_LIT(KFileName,"c:\\private\\102825B1\\hal.dat");
	RFs fs;
	fs.Connect();

	//Delete the File if it exists
	if(BaflUtils::FileExists(fs,KFileName))
		{
		TEST2(BaflUtils::DeleteFile(fs,KFileName),KErrNone);
		}
	TInt fastTimerFreq;
	HAL::Get(HALData::EFastCounterFrequency, fastTimerFreq);
	TReal ticksPerMicroSec = 1.0E-6 * fastTimerFreq;
	TUint prevTime;
	TUint timeDiff;
	prevTime = User::FastCounter();
	TInt noSuccessfulSamples=0;
	for(;noSuccessfulSamples < noSample; ++noSuccessfulSamples)
		{
		if ( TestInitialiseHALWaitL() !=KErrNotFound)
			{
	 	    break;
			}
		//else continue;
		}
	TEST(noSuccessfulSamples >0);
	timeDiff = User::FastCounter() - prevTime;
	TReal64 fsSessionMicroSecs = timeDiff / (noSuccessfulSamples * ticksPerMicroSec);
	TheTest.Printf(_L("Time to Initialise HAL  (no file) = %10.2lf microseconds\n"), fsSessionMicroSecs);
	fs.Close();
	}

/**
@SYMTestCaseID          SYSLIB-HALSETTINGS-CT-1720
@SYMTestCaseDesc        Performance Test Base Line
@SYMTestPriority        Medium
@SYMTestActions         Measure the performance to run empty exe
@SYMTestExpectedResults Tests must not fail
@SYMDEF                 DEF083235: Prop: HAL Attributes (eg screen calibration) lost if the battery is pulled out
*/
void PerformanceBaseLineL()
	{
	TheTest.Next (_L (" @SYMTestCaseID:SYSLIB-HALSETTINGS-CT-1720 Performance Test of empty EXE "));

	TInt fastTimerFreq;
	HAL::Get(HALData::EFastCounterFrequency, fastTimerFreq);
	TReal ticksPerMicroSec = 1.0E-6 * fastTimerFreq;
	TUint prevTime;
	TUint timeDiff;

	prevTime = User::FastCounter();
	TInt noSuccessfulSamples=0;
	for(;noSuccessfulSamples<noSample; ++noSuccessfulSamples)
		{
		if (StartEmptyExeL() != KErrArgument)
			{
			break;
			}
		//else continue;
		}
		TEST(noSuccessfulSamples >0);
		timeDiff = User::FastCounter() - prevTime;
		TReal64 fsSessionMicroSecs = timeDiff / (noSuccessfulSamples * ticksPerMicroSec);
		TheTest.Printf(_L("Time to run Empty EXE = %10.2lf microseconds\n"), fsSessionMicroSecs);
	}



/**
@SYMTestCaseID          SYSLIB-SYSAGENT2-UT-3385
@SYMTestCaseDesc        Checking the HAL.DAT filesize is not too big
@SYMTestPriority        Medium
@SYMTestActions         Initialise HAL settings with HAL.DAT padded past the reasonable threshold
@SYMTestExpectedResults Call to initialise HAL settings should return with KErrCorrupt
@SYMDEF                 PDEF101743: A small possibility for halsettings.exe to go infinite loop while initializing
*/

void TestHALDataSizeMismatchL()
	{
	TheTest.Next (_L (" @SYMTestCaseID:SYSLIB-SYSAGENT2-UT-3385 HAL.DAT size mismatch "));
	TInt err = BaflUtils::PersistHAL();
	_LIT(KFileName,"c:\\private\\102825B1\\hal.dat");
	RFs fs;
	if (err == KErrNone)
		{
		fs.Connect();
		RFile file;
		TInt pos = 0;
		err = file.Open(fs,KFileName,EFileRead | EFileWrite);
		// Want to add junk onto the end of the file, preserving the valid header
		err = file.Seek(ESeekEnd, pos);

		//write data in multiples of 8 otherwise it will not pass the check (size&7) == 0
		for (TInt i = 0; i < 400; i++)
			{
			file.Write(_L8("junkjunkjunkjunk"));
			}
		file.Close();
		}

	if(err == KErrNone)
		{
		TInt result = TestInitialiseHALWaitL();
		TEST2(result, KErrCorrupt);
		}
	err = fs.Delete(KFileName);
	fs.Close();
	}

/**
@SYMTestCaseID				SYSLIB-SYSAGENT2-UT-4001
@SYMTestCaseDesc			system clock change is persisted to hal.dat
@SYMTestPriority			Medium
@SYMTestActions				1. delete the hal file. 
							2. change system time 
							3. pause a few seconds and check if the hal file get created.
@SYMTestExpectedResults		At step 1 hal.dat is deleted (or KErrNotFound). 
							At step 3 hal.dat is created.
@SYMDEF						DEF115375
*/
void TestTimeChangesDEF115375()
	{
	TTime time;
	RProcess process;
	
	TheTest.Next (_L ("TestTimeChangesDEF115375"));
	_LIT(KFileName,"c:\\private\\102825B1\\hal.dat");
	
	TEST2(process.Create(KInitialiseLocaleExeName, KNullDesC),KErrNone);
	
	TRequestStatus stat;
	process.Rendezvous(stat);
	process.Resume(); // Start the process going
	//wait for the locale initialisation to complete first before testing
	User::WaitForRequest(stat);
	TEST2((stat.Int()==KErrNone)||(stat.Int()==KErrAlreadyExists),ETrue);
	
	RFs fs;
	fs.Connect(); 
	
	//Delete the File if it exists
	if(BaflUtils::FileExists(fs,KFileName))
		{
		TEST2(BaflUtils::DeleteFile(fs,KFileName),KErrNone);
		}
	
	time.UniversalTime();
	time+=TTimeIntervalMinutes(30);
	TInt r=User::SetUTCTime(time);
	TEST2(r, KErrNone);
	
	User::After(1000000);
	TEST(BaflUtils::FileExists(fs,KFileName));
	fs.Close();
	
	process.Close();
	User::After(1000000);
	}

/**
@SYMTestCaseID  			SYSLIB-SYSAGENT2-UT-4002
@SYMTestCaseDesc  			Check that the VendorID is set to 0x70000001 (Symbian's VendorID)
@SYMTestPriority   			Medium
@SYMTestActions 			1. Create a process for HALSettings.exe
							2. Compare the processes VID against the expected VID (0x70000001)
@SYMTestExpectedResults  	The VIDs should be identical and so the test will pass.
@SYMDEF 					INC121402
*/
void TestVendorIdINC121402()
	{
	TheTest.Next (_L ("TestVendorIDINC121402"));
	
	RProcess process;
	TEST2(process.Create(KHALSettingsExeName, _L("INITIALISE")), KErrNone);
	
	// Test that the VendorID of HALSettings.exe is set to the Symbian VendorID
	TEST2(KSymbianVendorId.iId, process.VendorId().iId);
	
	process.Close();
	}

LOCAL_C void RunTestsL()
	{

	//To create hal.dat
	TEST2(TestPersistHALWaitL(),KErrNone);
	//Initialise HAL
	TEST2(TestInitialiseHALWaitL(),KErrNone);
	TestHALFileWrongHeaderL();
	TestHALFileEmptyL();
	TestHALFileMissingL();
	TestInitialiseHALWrongParameterL();
	TestHALFileNoAccessL();
	TestTimeChangesDEF115375();
	TestVendorIdINC121402();
	//Performance Test
	PerformanceBaseLineL();
	TestPerformanceInitialiseWithFileL();
	TestPerformanceInitialiseNoFileL();
	TestHALDataSizeMismatchL();
	}

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	TheTest.Title ();
	TheTest.Start (_L ("HAL Initialise test"));
	CTrapCleanup* tc = CTrapCleanup::New();

	TRAPD(err, ::RunTestsL());

	delete tc;

	TheTest.End ();
	TheTest.Close ();

	__UHEAP_MARKEND;
	return err;
	}
