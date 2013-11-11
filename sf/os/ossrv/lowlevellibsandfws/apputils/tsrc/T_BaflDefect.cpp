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
//

#include <f32file.h>
#include <e32test.h>
#include <bautils.h>
#include <hal.h>
#include <e32std.h>
#include <e32math.h>

#include "backup_std.h"

LOCAL_D CTrapCleanup*		TheTrapCleanup=NULL;
LOCAL_D CActiveScheduler*	TheActiveScheduler=NULL;

LOCAL_D RTest				TheTest (_L("T_BaflDefect"));
LOCAL_D RFs					TheFs;

_LIT(KServerLauncherProcess, "T_BackupServerLauncher");
_LIT(KServerName, "!BackupServer");
_LIT(KBURServerName,"baksrvs");

class RIpcServerCloseTest : public RSessionBase
{
public: // Constructors and destructor

	/**
	* Constructor for performing 1st stage construction
	*/
	RIpcServerCloseTest();

	/**
	* Destructor.
	*/
	~RIpcServerCloseTest();

	/**
	* Performs test steps
	*/
	void RunTestL(const TDesC& aTargetSrvName, TInt aNumber, TInt aArgCount);
};

RIpcServerCloseTest::RIpcServerCloseTest()
	{
	// No implementation required
	}

RIpcServerCloseTest::~RIpcServerCloseTest()
	{
	Close();	
	}

void RIpcServerCloseTest::RunTestL(const TDesC& aTargetSrvName, TInt aNumber, TInt aArgCount)
	{
	TVersion version(0,0,0);
	TInt err = KErrNotFound;
	TInt numberOfAttempts = 3;
	
	while(err!=KErrNone && numberOfAttempts>0)
		{
		err = CreateSession(aTargetSrvName, version, 200);

		if(err!=KErrNone)
			{
			// wait then try again if err!=0
			TheTest.Printf(_L("CreateSession returned: %d"), err);
			User::After(1000000);
			numberOfAttempts--;
			}
		}
	
	TheTest(err == KErrNone);
	
	HBufC8* buf = HBufC8::NewLC(255);
	TPtr8 ptr = buf->Des();
	ptr.Fill(Math::Random(),255);
		
	TIpcArgs args;
		
	for(TInt i = 0; i < aArgCount ;i++)
		{
		args.Set(i,&ptr);
		}
	
	TheTest.Printf(_L("Sending request to: %d with %d args"), aNumber, aArgCount);
	
	TRequestStatus status;
	
	SendReceive(aNumber, args, status);
	
	User::WaitForRequest(status);
	
	TheTest.Printf(_L("Status value from sending request: %d with %d args"), status.Int(), aArgCount);
	
	TheTest(status.Int() == KErrNotSupported);

	CleanupStack::PopAndDestroy(buf);
	}

//This function is used in the test code to kill backupSrv or the backuplauncher process.
// It creates a separate process to do this as killing a process requires
// PwrMgmt capability which we don't want to grant to all test exes.
TInt KillProcess(const TDesC& aProcessName)
	{
	_LIT(KProcessKiller, "t_processkiller");
	TRequestStatus stat;
	RProcess p;
	TInt result = p.Create(KProcessKiller, aProcessName);
	
	if(result == KErrNone)
		{
		// Asynchronous logon: completes when process terminates with process exit code
		p.Logon(stat);
		p.Resume();

		User::WaitForRequest(stat);
		result = p.ExitReason();
		p.Close();			
		}

	return result;
	}

TInt LaunchServer(RProcess& aServer)
	{	
	TheTest.Printf(_L("Launching BackupServer...\n"));
	
	TInt err = aServer.Create(KServerLauncherProcess, _L(""));
	TheTest(err == KErrNone);
	     
	//Start server and wait until it is running
	TRequestStatus serverStat;
	aServer.SetJustInTime(false);   
	aServer.Resume(); 
		   
	aServer.Rendezvous(serverStat);
	User::WaitForRequest(serverStat);
		
	err = serverStat.Int();
		 
	return err;
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-UT-4052
@SYMTestCaseDesc        Tests BackupServer crashes under IPC fuzzing and freezes phone 
@SYMTestPriority        High
@SYMTestActions         Calls BackupServer with EBakOpCodeCloseServer with 0-4 args. Verifies that server returns KErrNotSupported.
@SYMTestExpectedResults Server should not process the  CloseServer message and should return KErrNotSupported
*/
LOCAL_C void Defect_INC120743L()
    {
	TheTest.Next (_L ("Defect_INC120743L"));
    
	__UHEAP_MARK;
	
    RProcess server;
    TInt messageToTest = EBakOpCodeCloseServer;

    //Clean up any chance of launcher or baksrvs still running
    TInt err = KillProcess(KServerLauncherProcess);
    if((err != KErrNotFound)&&(err != KErrDied))
    	{
    	User::LeaveIfError(err);	
    	}

    err = KillProcess(KBURServerName);
    if((err != KErrNotFound)&&(err != KErrDied))
    	{
    	User::LeaveIfError(err);	
    	}
    
    TInt startedFlag = LaunchServer(server);
    
    TheTest.Printf(_L("LaunchServer has returned: %d"), startedFlag);
    
    TheTest(startedFlag == 0 || startedFlag == KErrAlreadyExists);
    
 	CTrapCleanup* cleanup=CTrapCleanup::New();
   	err=KErrNoMemory;

   	if (cleanup)
   		{
		//Carry out each test with number of arguments 1 - 4
		for(TInt argCount = 0; argCount <= 4; argCount++)
			{
			RIpcServerCloseTest closeTest;
		
			TRAP(err,closeTest.RunTestL(KServerName, messageToTest, argCount));
		
			closeTest.Close();
			}

   		delete cleanup;
   		}
        
	__UHEAP_MARKEND;
    }

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-1390
@SYMTestCaseDesc        Tests BaflUtils::PersistScreenCalibration
@SYMTestPriority        Medium
@SYMTestActions         Deletes screen calibration file, calls PersistScreenCalibration()
and checks that file now exists
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void Defect_DEF068052L()
	{
	TheTest.Next (_L (" @SYMTestCaseID:SYSLIB-BAFL-CT-1390 Defect_DEF068052L "));

    __UHEAP_MARK;

	User::LeaveIfError (TheFs.Connect ());
	::CleanupClosePushL (TheFs);

	const TInt KDriveLength = 2;
	const TInt KScreenPathLength = 26;
	const TInt KScreenPathLengthTemp = 30;

	_LIT (KDriveColon, ":");
	_LIT (KScreen, "\\System\\Data\\Screen.DAT");
	_LIT (KScreenTemp, "\\System\\Data\\ScreenTemp.DAT");

	// Find the system drive
	TDriveNumber systemDrive = TheFs.GetSystemDrive();

	// Set up a full file path name incorporating the system drive
	// (for example if system drive is EDriveC pathname will be:
	// C:\System\Data\Screen.DAT)
	TBuf<KDriveLength+KScreenPathLength> screenFilePath;
	screenFilePath.Append(systemDrive + 'A');
	screenFilePath.Append(KDriveColon);
	screenFilePath.Append(KScreen);

	// Set up a full file path name to make a copy of the screen.dat
	// file.
	TBuf<KDriveLength+KScreenPathLengthTemp> screenFilePathTemp;
	screenFilePathTemp.Append(systemDrive + 'A');
	screenFilePathTemp.Append(KDriveColon);
	screenFilePathTemp.Append(KScreenTemp);

	// Make a copy of the screen.dat file (if it exists)
	BaflUtils::CopyFile(TheFs, screenFilePath, screenFilePathTemp);

	// Delete the Screen.DAT file (if it exists)
	BaflUtils::DeleteFile(TheFs, screenFilePath);

	// Saving Screen settings - call API to write the screen
	// calibration to file
	TDigitizerCalibration calib;
	BaflUtils::PersistScreenCalibration(calib);

	// Check if screen.dat file now exists
	TBool exists = BaflUtils::FileExists(TheFs, screenFilePath);
	TheTest(exists);

	TheTest.Printf(_L("Screen settings were saved in %S\n"), &screenFilePath);

	// Cleaning up, restore files to original state
	User::LeaveIfError(BaflUtils::DeleteFile(TheFs, screenFilePath));
	BaflUtils::CopyFile(TheFs, screenFilePathTemp, screenFilePath);
	BaflUtils::DeleteFile(TheFs, screenFilePathTemp);
	CleanupStack::PopAndDestroy (&::TheFs);

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0487
@SYMTestCaseDesc        Tests for defect number INC045169L
@SYMTestPriority        High
@SYMTestActions         Tests for creation of files
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
void Defect_INC045169L()
	{
	TheTest.Next (_L (" @SYMTestCaseID:SYSLIB-BAFL-CT-0487 Defect_INC045169L "));

	__UHEAP_MARK;

  	// find out the number of open handles
	TInt startProcessHandleCount;
	TInt startThreadHandleCount;
	RThread ().HandleCount (startProcessHandleCount, startThreadHandleCount);

	//
	// The Test Setup

	User::LeaveIfError (TheFs.Connect ());
	::CleanupClosePushL (TheFs);

	//
	// The Test Begins...


	// Our test diectory and files...
	_LIT (KDirectory, "C:\\System\\Data\\");

	// ++++++++++++++

	// When creating test file names be careful to use different
	// combinations of charaters as you may get ambiguity!
	// i.e. '.aaa' will find files '.a' or 'a' if on disk.
	// similarly, 'abc158' will find files 'abc' or 'abc.'.
	// Also, file name ending in a period will have them
	// stripped off by the Fs, so 'abcd.' will give you 'abcd'
	// Finally, don't have like names like 'abc.rsc' and 'abc.r'

	// Failure cases
	_LIT (KFilename1, "c:"); // not supported
	_LIT (KFilename2, "x"); // not supported
	_LIT (KFilename3, "C:\\System\\Data\\a"); // not supported
	_LIT (KFilename4, "C:\\System\\Data\\ab"); // not supported
	_LIT (KFilename5, "C:\\System\\Data\\i.j"); // no 'i.01' available
	_LIT (KFilename6, "C:\\System\\Data\\abc.r12345678901234567890"); // suffix too large
	_LIT (KFilename7, "C:\\System\\Data\\"); // not supported

	// Successful cases
	_LIT (KFilename10, "C:\\System\\Data\\cde");
	_LIT (KFilename11, "C:\\System\\Data\\abc158");
	_LIT (KFilename12, "C:\\System\\Data\\C01abc001");
	_LIT (KFilename13, "C:\\System\\Data\\0123456789.012");
	_LIT (KFilename14, "C:\\System\\Data\\0123");
	_LIT (KFilename15, "C:\\System\\Data\\0123456789012");
	_LIT (KFilename16, "C:\\System\\Data\\C.01a1");
	_LIT (KFilename17, "C:\\System\\Data\\C.a0a1");
	_LIT (KFilename18, "C:\\System\\Data\\.b");
	_LIT (KFilename19, "C:\\System\\Data\\.ccc");
	_LIT (KFilename20, "C:\\System\\Data\\rfg.3a");
	_LIT (KFilename21, "C:\\System\\Data\\abc.r158");
	_LIT (KFilename22, "C:\\System\\Data\\abc.rsc");
	_LIT (KFilename23, "C:\\System\\Data\\efg.r");
	_LIT (KFilename24, "C:\\System\\Data\\klmn.");
	_LIT (KFilename25, "C:\\System\\Data\\jhgdfgd.123456789abc");
	_LIT (KFilename26, "\\abc.r158");
	_LIT (KFilename27, "abc.r158");
	_LIT (KFilename28, "c:abc.r158");

	// ++++++++++++++

	// ELangAmerican
	_LIT (KLocale, "ELOCL.10");
	_LIT (KNearestLang, "C:\\System\\Data\\abc.r10");

	// Tidy the files if there...
	// KFilename1 can't be created
	BaflUtils::DeleteFile (TheFs, KFilename2);
	BaflUtils::DeleteFile (TheFs, KFilename3);
	BaflUtils::DeleteFile (TheFs, KFilename4);
	BaflUtils::DeleteFile (TheFs, KFilename5);
	BaflUtils::DeleteFile (TheFs, KFilename6);

 	// Do not delete KFileName7, there may be other
 	// files in this directory unrelated to this test
 	// which are required by other tests.
 	// See defect DEF108808
 	// BaflUtils::DeleteFile (TheFs, KFilename7);

	BaflUtils::DeleteFile (TheFs, KFilename10);
	BaflUtils::DeleteFile (TheFs, KFilename11);
	BaflUtils::DeleteFile (TheFs, KFilename12);
	BaflUtils::DeleteFile (TheFs, KFilename13);
	BaflUtils::DeleteFile (TheFs, KFilename14);
	BaflUtils::DeleteFile (TheFs, KFilename15);
	BaflUtils::DeleteFile (TheFs, KFilename16);
	BaflUtils::DeleteFile (TheFs, KFilename17);
	BaflUtils::DeleteFile (TheFs, KFilename18);
	BaflUtils::DeleteFile (TheFs, KFilename19);
	BaflUtils::DeleteFile (TheFs, KFilename20);
	BaflUtils::DeleteFile (TheFs, KFilename21);
	BaflUtils::DeleteFile (TheFs, KFilename22);
	BaflUtils::DeleteFile (TheFs, KFilename23);
#ifdef __EPOC32__
	BaflUtils::DeleteFile (TheFs, KFilename24);
#else
	// Windows strips off trailing periods
	BaflUtils::DeleteFile (TheFs, _L("C:\\System\\Data\\klmn"));
#endif
	BaflUtils::DeleteFile (TheFs, KFilename25);
	BaflUtils::DeleteFile (TheFs, KFilename26);
	BaflUtils::DeleteFile (TheFs, KFilename27);
	BaflUtils::DeleteFile (TheFs, KFilename28);
	BaflUtils::DeleteFile (TheFs, KNearestLang);

	// Confirm the prerequisites for this test...
	if (!BaflUtils::PathExists (TheFs, KDirectory))
		{
		BaflUtils::EnsurePathExistsL(TheFs, KDirectory);
		}

	// KFilename1 can't be created
	TheTest (BaflUtils::FileExists (TheFs, KFilename2) == EFalse);
	TheTest (BaflUtils::FileExists (TheFs, KFilename3) == EFalse);
	TheTest (BaflUtils::FileExists (TheFs, KFilename4) == EFalse);
	TheTest (BaflUtils::FileExists (TheFs, KFilename5) == EFalse);
	TheTest (BaflUtils::FileExists (TheFs, KFilename6) == EFalse);
	// KFilename7 can't be created, not a file

	TheTest (BaflUtils::FileExists (TheFs, KFilename10) == EFalse);
	TheTest (BaflUtils::FileExists (TheFs, KFilename11) == EFalse);
	TheTest (BaflUtils::FileExists (TheFs, KFilename12) == EFalse);
	TheTest (BaflUtils::FileExists (TheFs, KFilename13) == EFalse);
	TheTest (BaflUtils::FileExists (TheFs, KFilename14) == EFalse);
	TheTest (BaflUtils::FileExists (TheFs, KFilename15) == EFalse);
	TheTest (BaflUtils::FileExists (TheFs, KFilename16) == EFalse);
	TheTest (BaflUtils::FileExists (TheFs, KFilename17) == EFalse);
	TheTest (BaflUtils::FileExists (TheFs, KFilename18) == EFalse);
	TheTest (BaflUtils::FileExists (TheFs, KFilename19) == EFalse);
	TheTest (BaflUtils::FileExists (TheFs, KFilename20) == EFalse);
	TheTest (BaflUtils::FileExists (TheFs, KFilename21) == EFalse);
	TheTest (BaflUtils::FileExists (TheFs, KFilename22) == EFalse);
	TheTest (BaflUtils::FileExists (TheFs, KFilename23) == EFalse);
	TheTest (BaflUtils::FileExists (TheFs, KFilename24) == EFalse);
	TheTest (BaflUtils::FileExists (TheFs, KFilename25) == EFalse);
	TheTest (BaflUtils::FileExists (TheFs, KFilename26) == EFalse);
	TheTest (BaflUtils::FileExists (TheFs, KFilename27) == EFalse);
	TheTest (BaflUtils::FileExists (TheFs, KFilename28) == EFalse);
	TheTest (BaflUtils::FileExists (TheFs, KNearestLang) == EFalse);

	// Create the files...
	RFile rFile;
	// KFilename1 can't be created
	rFile.Create (TheFs, KFilename2, EFileRead);
	rFile.Close ();
	rFile.Create (TheFs, KFilename3, EFileRead);
	rFile.Close ();
	rFile.Create (TheFs, KFilename4, EFileRead);
	rFile.Close ();
	rFile.Create (TheFs, KFilename5, EFileRead);
	rFile.Close ();
	rFile.Create (TheFs, KFilename6, EFileRead);
	rFile.Close ();
	rFile.Create (TheFs, KFilename7, EFileRead);
	rFile.Close ();

	rFile.Create (TheFs, KFilename10, EFileRead);
	rFile.Close ();
	rFile.Create (TheFs, KFilename11, EFileRead);
	rFile.Close ();
	rFile.Create (TheFs, KFilename12, EFileRead);
	rFile.Close ();
	rFile.Create (TheFs, KFilename13, EFileRead);
	rFile.Close ();
	rFile.Create (TheFs, KFilename14, EFileRead);
	rFile.Close ();
	rFile.Create (TheFs, KFilename15, EFileRead);
	rFile.Close ();
	rFile.Create (TheFs, KFilename16, EFileRead);
	rFile.Close ();
	rFile.Create (TheFs, KFilename17, EFileRead);
	rFile.Close ();
	rFile.Create (TheFs, KFilename18, EFileRead);
	rFile.Close ();
	rFile.Create (TheFs, KFilename19, EFileRead);
	rFile.Close ();
	rFile.Create (TheFs, KFilename20, EFileRead);
	rFile.Close ();
	rFile.Create (TheFs, KFilename21, EFileRead);
	rFile.Close ();
	rFile.Create (TheFs, KFilename22, EFileRead);
	rFile.Close ();
	rFile.Create (TheFs, KFilename23, EFileRead);
	rFile.Close ();
	rFile.Create (TheFs, KFilename24, EFileRead);
	rFile.Close ();
	rFile.Create (TheFs, KFilename25, EFileRead);
	rFile.Close ();
	rFile.Create (TheFs, KFilename26, EFileRead);
	rFile.Close ();
	rFile.Create (TheFs, KFilename27, EFileRead);
	rFile.Close ();
	rFile.Create (TheFs, KFilename28, EFileRead);
	rFile.Close ();
	rFile.Create (TheFs, KNearestLang, EFileRead);
	rFile.Close ();

	TBuf <256> filename (KFilename1);

	// Test the defect...
	BaflUtils::NearestLanguageFile (TheFs, filename);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KFilename1, &filename);
	TheTest(filename == KFilename1);

	filename.Copy (KFilename2);
	BaflUtils::NearestLanguageFile (TheFs, filename);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KFilename2, &filename);
	TheTest(filename == _L("x"));

	filename.Copy (KFilename3);
	BaflUtils::NearestLanguageFile (TheFs, filename);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KFilename3, &filename);
	TheTest(filename == KFilename3);

	filename.Copy (KFilename4);
	BaflUtils::NearestLanguageFile (TheFs, filename);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KFilename4, &filename);
	TheTest(filename == KFilename4);

	filename.Copy (KFilename5);
	BaflUtils::NearestLanguageFile (TheFs, filename);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KFilename5, &filename);
	TheTest(filename == KFilename5);

	filename.Copy (KFilename6);
	BaflUtils::NearestLanguageFile (TheFs, filename);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KFilename6, &filename);
	TheTest(filename == KFilename6);

	filename.Copy (KFilename7);
	BaflUtils::NearestLanguageFile (TheFs, filename);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KFilename7, &filename);
	TheTest(filename == KFilename7);



	filename.Copy (KFilename10);
	BaflUtils::NearestLanguageFile (TheFs, filename);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KFilename10, &filename);
	TheTest(filename == KFilename10);

	filename.Copy (KFilename11);
	BaflUtils::NearestLanguageFile (TheFs, filename);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KFilename11, &filename);
	TheTest(filename == KFilename11);

	filename.Copy (KFilename12);
	BaflUtils::NearestLanguageFile (TheFs, filename);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KFilename12, &filename);
	TheTest(filename == KFilename12);

	filename.Copy (KFilename13);
	BaflUtils::NearestLanguageFile (TheFs, filename);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KFilename13, &filename);
	TheTest(filename == KFilename13);

	filename.Copy (KFilename14);
	BaflUtils::NearestLanguageFile (TheFs, filename);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KFilename14, &filename);
	TheTest(filename == KFilename14);

	filename.Copy (KFilename15);
	BaflUtils::NearestLanguageFile (TheFs, filename);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KFilename15, &filename);
	TheTest(filename == KFilename15);

	filename.Copy (KFilename16);
	BaflUtils::NearestLanguageFile (TheFs, filename);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KFilename16, &filename);
	TheTest(filename == KFilename16);

	filename.Copy (KFilename17);
	BaflUtils::NearestLanguageFile (TheFs, filename);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KFilename17, &filename);
	TheTest(filename == KFilename17);

	filename.Copy (KFilename18);
	BaflUtils::NearestLanguageFile (TheFs, filename);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KFilename18, &filename);
	TheTest(filename == KFilename18);

	filename.Copy (KFilename19);
	BaflUtils::NearestLanguageFile (TheFs, filename);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KFilename19, &filename);
	TheTest(filename == KFilename19);

	filename.Copy (KFilename20);
	BaflUtils::NearestLanguageFile (TheFs, filename);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KFilename20, &filename);
	TheTest(filename == KFilename20);

	filename.Copy (KFilename21);
	BaflUtils::NearestLanguageFile (TheFs, filename);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KFilename21, &filename);
	TheTest(filename == KFilename21);

	filename.Copy (KFilename22);
	BaflUtils::NearestLanguageFile (TheFs, filename);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KFilename22, &filename);
	TheTest(filename == KFilename22);

	filename.Copy (KFilename23);
	BaflUtils::NearestLanguageFile (TheFs, filename);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KFilename23, &filename);
	TheTest(filename == KFilename23);

	filename.Copy (KFilename24);
	BaflUtils::NearestLanguageFile (TheFs, filename);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KFilename24, &filename);
	TheTest(filename == KFilename24);

	filename.Copy (KFilename25);
	BaflUtils::NearestLanguageFile (TheFs, filename);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KFilename25, &filename);
	TheTest(filename == KFilename25);

	filename.Copy (KFilename26);
	BaflUtils::NearestLanguageFile (TheFs, filename);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KFilename26, &filename);
	TheTest(filename == KFilename26);

	filename.Copy (KFilename27);
	BaflUtils::NearestLanguageFile (TheFs, filename);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KFilename27, &filename);
	TheTest(filename == KFilename27);

	filename.Copy (KFilename28);
	BaflUtils::NearestLanguageFile (TheFs, filename);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KFilename28, &filename);
	TheTest(filename == KFilename28);


	// Store the original locale settings so they can be restored at the
	// end of the test.

	TExtendedLocale originalLocale;
	originalLocale.LoadSystemSettings();

	// Change locale
	TExtendedLocale newLocale;
	TBuf<50> libraryName;

	libraryName.Format(KLocale);

	TInt err = newLocale.LoadLocale(libraryName);
	User::LeaveIfError (err);

	newLocale.SaveSystemSettings();
	User::After (5000000);

	filename.Copy (KFilename1);
	BaflUtils::NearestLanguageFile (TheFs, filename);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KFilename1, &filename);
	TheTest(filename == KFilename1);

	filename.Copy (KFilename2);
	BaflUtils::NearestLanguageFile (TheFs, filename);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KFilename2, &filename);
	TheTest(filename == _L("x"));

	filename.Copy (KFilename3);
	BaflUtils::NearestLanguageFile (TheFs, filename);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KFilename3, &filename);
	TheTest(filename == KFilename3);

	filename.Copy (KFilename4);
	BaflUtils::NearestLanguageFile (TheFs, filename);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KFilename4, &filename);
	TheTest(filename == KFilename4);

	filename.Copy (KFilename5);
	BaflUtils::NearestLanguageFile (TheFs, filename);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KFilename5, &filename);
	TheTest(filename == KFilename5);

	filename.Copy (KFilename6);
	BaflUtils::NearestLanguageFile (TheFs, filename);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KFilename6, &filename);
	TheTest(filename == KFilename6);

	filename.Copy (KFilename7);
	BaflUtils::NearestLanguageFile (TheFs, filename);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KFilename7, &filename);
	TheTest(filename == KFilename7);



	filename.Copy (KFilename10);
	BaflUtils::NearestLanguageFile (TheFs, filename);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KFilename10, &filename);
	TheTest(filename == KFilename10);

	filename.Copy (KFilename11);
	BaflUtils::NearestLanguageFile (TheFs, filename);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KFilename11, &filename);
	TheTest(filename == KFilename11);

	filename.Copy (KFilename12);
	BaflUtils::NearestLanguageFile (TheFs, filename);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KFilename12, &filename);
	TheTest(filename == KFilename12);

	filename.Copy (KFilename13);
	BaflUtils::NearestLanguageFile (TheFs, filename);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KFilename13, &filename);
	TheTest(filename == KFilename13);

	filename.Copy (KFilename14);
	BaflUtils::NearestLanguageFile (TheFs, filename);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KFilename14, &filename);
	TheTest(filename == KFilename14);

	filename.Copy (KFilename15);
	BaflUtils::NearestLanguageFile (TheFs, filename);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KFilename15, &filename);
	TheTest(filename == KFilename15);

	filename.Copy (KFilename16);
	BaflUtils::NearestLanguageFile (TheFs, filename);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KFilename16, &filename);
	TheTest(filename == KFilename16);

	filename.Copy (KFilename17);
	BaflUtils::NearestLanguageFile (TheFs, filename);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KFilename17, &filename);
	TheTest(filename == KFilename17);

	filename.Copy (KFilename18);
	BaflUtils::NearestLanguageFile (TheFs, filename);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KFilename18, &filename);
	TheTest(filename == KFilename18);

	filename.Copy (KFilename19);
	BaflUtils::NearestLanguageFile (TheFs, filename);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KFilename19, &filename);
	TheTest(filename == KFilename19);

	filename.Copy (KFilename20);
	BaflUtils::NearestLanguageFile (TheFs, filename);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KFilename20, &filename);
	TheTest(filename == KFilename20);

	filename.Copy (KFilename21);
	BaflUtils::NearestLanguageFile (TheFs, filename);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KFilename21, &filename);
	TheTest(filename == KNearestLang);

	filename.Copy (KFilename22);
	BaflUtils::NearestLanguageFile (TheFs, filename);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KFilename22, &filename);
	TheTest(filename == KNearestLang);

	filename.Copy (KFilename23);
	BaflUtils::NearestLanguageFile (TheFs, filename);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KFilename23, &filename);
	TheTest(filename == KFilename23);

	filename.Copy (KFilename24);
	BaflUtils::NearestLanguageFile (TheFs, filename);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KFilename24, &filename);
	TheTest(filename == KFilename24);

	filename.Copy (KFilename25);
	BaflUtils::NearestLanguageFile (TheFs, filename);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KFilename25, &filename);
	TheTest(filename == KFilename25);

	filename.Copy (KFilename26);
	BaflUtils::NearestLanguageFile (TheFs, filename);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KFilename26, &filename);
	TheTest(filename == KFilename26);

	filename.Copy (KFilename27);
	BaflUtils::NearestLanguageFile (TheFs, filename);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KFilename27, &filename);
	TheTest(filename == KFilename27);

	filename.Copy (KFilename28);
	BaflUtils::NearestLanguageFile (TheFs, filename);
	TheTest.Printf (_L ("The NearestLanguageFile for %S is %S\n"), &KFilename28, &filename);
	TheTest(filename == KFilename28);

	// Tidy the files...
	// KFilename1 can't be created
	BaflUtils::DeleteFile (TheFs, KFilename2);
	BaflUtils::DeleteFile (TheFs, KFilename3);
	BaflUtils::DeleteFile (TheFs, KFilename4);
	BaflUtils::DeleteFile (TheFs, KFilename5);
	BaflUtils::DeleteFile (TheFs, KFilename6);

	// Do not delete KFileName7, there may be other
 	// files in this directory unrelated to this test
 	// which are required by other tests.
 	// See defect DEF108808
 	// BaflUtils::DeleteFile (TheFs, KFilename7);

	BaflUtils::DeleteFile (TheFs, KFilename10);
	BaflUtils::DeleteFile (TheFs, KFilename11);
	BaflUtils::DeleteFile (TheFs, KFilename12);
	BaflUtils::DeleteFile (TheFs, KFilename13);
	BaflUtils::DeleteFile (TheFs, KFilename14);
	BaflUtils::DeleteFile (TheFs, KFilename15);
	BaflUtils::DeleteFile (TheFs, KFilename16);
	BaflUtils::DeleteFile (TheFs, KFilename17);
	BaflUtils::DeleteFile (TheFs, KFilename18);
	BaflUtils::DeleteFile (TheFs, KFilename19);
	BaflUtils::DeleteFile (TheFs, KFilename20);
	BaflUtils::DeleteFile (TheFs, KFilename21);
	BaflUtils::DeleteFile (TheFs, KFilename22);
	BaflUtils::DeleteFile (TheFs, KFilename23);
#ifdef __EPOC32__
	BaflUtils::DeleteFile (TheFs, KFilename24);
#else
	// Windows strips off trailing periods
	BaflUtils::DeleteFile (TheFs, _L("C:\\System\\Data\\klmn"));
#endif
	BaflUtils::DeleteFile (TheFs, KFilename25);
	BaflUtils::DeleteFile (TheFs, KFilename26);
	BaflUtils::DeleteFile (TheFs, KFilename27);
	BaflUtils::DeleteFile (TheFs, KFilename28);
	BaflUtils::DeleteFile (TheFs, KNearestLang);

	// Restore the original locale settings.
  	originalLocale.SaveSystemSettings();

	CleanupStack::PopAndDestroy (&::TheFs);

	// The Test Ends...
	//

	// check that no handles have leaked
	TInt endProcessHandleCount;
	TInt endThreadHandleCount;
	RThread ().HandleCount (endProcessHandleCount, endThreadHandleCount);

	TheTest (startThreadHandleCount == endThreadHandleCount);

	__UHEAP_MARKEND;
	}

//===============================================================================

/**
Initialise the cleanup stack and active scheduler
*/
LOCAL_C void SetupL ()
    {
	TheTrapCleanup = CTrapCleanup::New ();
	User::LeaveIfNull (TheTrapCleanup);

	// Construct and install the active scheduler
	TheActiveScheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install (TheActiveScheduler);
	}

/**
Cleanup
*/
LOCAL_C void CleanupL ()
    {
	delete TheActiveScheduler;
	delete TheTrapCleanup;
	}


/**
Invoke the tests
*/
LOCAL_C void DoTestsL ()
    {
	Defect_INC045169L ();
	Defect_DEF068052L ();
	
	Defect_INC120743L ();
	}

GLDEF_C TInt E32Main ()
	{
	__UHEAP_MARK;

	TheTest.Printf (_L ("\n"));
	TheTest.Title ();
	TheTest.Start (_L("Defect Tests "));

	TRAPD (err, SetupL ());
	TheTest (err == KErrNone);

	TRAP (err, DoTestsL ());
	TheTest (err == KErrNone);

	CleanupL ();

	TheTest.End ();
	TheTest.Close ();

	__UHEAP_MARKEND;
	return (KErrNone);
	}
