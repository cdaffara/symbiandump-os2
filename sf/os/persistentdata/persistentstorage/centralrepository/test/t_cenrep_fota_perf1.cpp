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
//

#include "t_cenrep_helper.h"
#include <centralrepository.h>
#include <e32property.h>
#include <e32test.h>
#include <f32file.h>
#include "testexecute/SWI/src/tui.h"
#include <e32std.h>
#include <swi/launcher.h>
#include <bautils.h>
#include "swi/sisregistrysession.h"
#include "swi/sisregistrypackage.h"
#include "swi/sisregistryentry.h"

#include "e32math.h"
#include "../cenrepsrv/srvparams.h"
#include "../cenrepsrv/srvreqs.h"
#include "../common/inc/srvdefs.h"
#include "../cenrepsrv/transstate.h"

RTest TheTest(_L("Central Repository Rom Flash Startup Test"));

const TUid KUidRomRepositories[] = { {0x90011116}, {0x90011117}, {0x90011118}, {0x90011119}, {0x90011120}, {0x90011121}, {0x90011122}, {0x90011123}, {0x90011124}, {0x90011125} , {0x90011126} , {0x90011127} , {0x90011128} ,{0x90011129}, {0x90011130} ,{0x90011131},{0x90011132},{0x90011133},{0x90011134}, {0x90011135}, {0x90011136}, {0x90011137}, {0x90011138}, {0x90011139}, {0x90011140} };

_LIT(KSisFile,"z:\\private\\10202be9\\RF2.sis");

///////////////////////////////////////////////////////////////////////////////////////
//Test macroses and functions

LOCAL_C void CheckL(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		RDebug::Print(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		CleanupCDriveL();
		TheTest(EFalse, aLine);
		}
	}
	
#define TEST2(aValue, aExpected) ::CheckL(aValue, aExpected, __LINE__)

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

/**
@SYMTestCaseID			SYSLIB-CENTRALREPOSITORY-CT-4016
@SYMTestCaseDesc		Time the startup impact for 30 repositories changes and updates. 
@SYMTestActions		  	Part 1 of Server startup timing test, sets up the test environment
@SYMTestExpectedResults 	The server Intialisation time is retrieved.
@SYMCR				CR1198
*/	
///////////////////////////////////////////////////////////////////////////////////////	
/**
@SYMTestCaseID			SYSLIB-CENTRALREPOSITORY-CT-4017
@SYMTestCaseDesc		Time the startup impact for 1 repository change and update. 
@SYMTestActions		  	Part 1 of Server startup timing test, sets up the test environment
@SYMTestExpectedResults 	The server Intialisation time is retrieved.
@SYMTestPriority    Medium
@SYMCR				CR1198
*/	
///////////////////////////////////////////////////////////////////////////////////////	
/**
@SYMTestCaseID			SYSLIB-CENTRALREPOSITORY-CT-4018
@SYMTestCaseDesc		Time the startup impact for 10 repositories changes and updates. 
@SYMTestActions		  	Part 1 of Server startup timing test, sets up the test environment
@SYMTestExpectedResults 	The server Intialisation time is retrieved.
@SYMTestPriority    Medium
@SYMCR				CR1198
*/	
///////////////////////////////////////////////////////////////////////////////////////	
/**
@SYMTestCaseID			SYSLIB-CENTRALREPOSITORY-CT-4019
@SYMTestCaseDesc		Time the startup impact for 20 repositories changes and updates. 
@SYMTestActions		  	Part 1 of Server startup timing test, sets up the test environment
@SYMTestExpectedResults 	The server Intialisation time is retrieved.
@SYMCR				CR1198
*/
LOCAL_C void Test_StartupPerformanceL(TInt aTestID, TInt aNumberOfReps)
	{
	const TUint32 KNewSetting1 = 0x1000100;
	const TUint32 KModifiedSetting1 = 0x02010100;
	const TUint32 KModifiedSetting2 = 0x02010400;
	const TUint32 KDeleteSetting1 = 0x02010200;
		
	const TInt KIntValue = 1234;
	const TInt KInt_UpdatedValue1 = 100;
   
//********************************************************
//Requires stub file to be set up before it will work
//requires techview
//********************************************************
 	// launch the installation
    TFileName SisFileName;
    SisFileName.Copy(KSisFile);
  
   if(aTestID == '0') 
	{
    	using namespace Swi;
    	TUI* Ui = new(ELeave) TUI();
    	CInstallPrefs* prefs = CInstallPrefs::NewLC();
    	TInt err = Launcher::Install(*Ui, SisFileName, *prefs);
    	TEST2(err, KErrNone);
    	delete Ui;
    	CleanupStack::PopAndDestroy();//prefs
    }

	//Open each of the rom repositories and modify them	
	TInt ii = 0;
	for(;ii<aNumberOfReps;++ii)
		{
    	CRepository* repository = CRepository::NewLC(KUidRomRepositories[ii]);
		TInt err=repository->StartTransaction(CRepository::EReadWriteTransaction);
 		TEST2(err,KErrNone);

		//Add a setting
		err=repository->Create(KNewSetting1,KIntValue);
 		TEST2(err,KErrNone);
 		
 		//Modify settings
 		err=repository->Set(KModifiedSetting1,KInt_UpdatedValue1);
 		TEST2(err,KErrNone);
	 
  		err=repository->Set(KModifiedSetting2,KInt_UpdatedValue1);
 		TEST2(err,KErrNone);
 		
 		//ensure deleted value is actually there
 		TInt settingValue = 0;
		TInt r = repository->Get(KDeleteSetting1, settingValue);
		TEST2(r, KErrNone);
	
		err=repository->Delete(KDeleteSetting1);
 		TEST2(err,KErrNone);
		
 	
 		//--------------Commit Transaction------------------------------------
   		TUint32 errId;
   		err=repository->CommitTransaction(errId);
   		TEST2(err,KErrNone);
		CleanupStack::PopAndDestroy();//repository	
		
   		}

	}

LOCAL_C void FuncTestsL()
	{ 
    TheTest.Start(_L("Central repository ->Test StartupPerformance"));	
    TBuf<0x100> cmd;
    User::CommandLine(cmd);
    TLex lex(cmd);
    lex.SkipSpace();
    TPtrC token = lex.NextToken();

    if(token.Length() == 1) 
    	{
    	TChar c = token[0];
    	if(c.IsDigit() && (c == '0')) 
    		Test_StartupPerformanceL(c,25);
    	else if(c.IsDigit() && (c == '1'))
    		Test_StartupPerformanceL(c,1);
    	else if(c.IsDigit() && (c == '2'))
    		Test_StartupPerformanceL(c,10);
    	else if(c.IsDigit() && (c == '3'))
    		Test_StartupPerformanceL(c,20);
    	else
    		RDebug::Print(_L("Unknown option, requires either argument 0,1,2 or 3"));	
    	}
	TheTest.End();
	}

LOCAL_C void MainL()
	{
	TheTest.Start(_L("CentralRepository ROM Flash Tests"));
	CleanupCDriveL(EFalse);

	FuncTestsL();

	TheTest.End();
	TheTest.Close();
	}

TInt E32Main()
	{	
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(!cleanup)
		return KErrNoMemory;

	TRAPD(err, MainL());
	if (err != KErrNone)
		User::Panic(_L("Testing failed: "), err);
	
	delete cleanup;
	__UHEAP_MARKEND;

	return 0;
	}

