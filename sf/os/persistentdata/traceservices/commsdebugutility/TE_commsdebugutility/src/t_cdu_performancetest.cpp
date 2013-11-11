// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This test is to test the performace of CDU by logging information 
// continuosly in a loop and find the difference between the time 
// before the loop and the time after the loop.

// This test is run using the script file ts_comsdbg_performance.script

// This test is not run as part of nightly builds and could only be used 
// when CDU performance testing is necessary.

#include <e32test.h>
#include <comms-infras/commsdebugutility.h>

#ifndef __WINS__
#include <f32file.h>
#endif // __WINS__

#include "t_cdu_performancetest.h"

_LIT8(KCDUProfileLogFolder, "subsystem");
_LIT8(KCDUProfileLogFile,"component");
_LIT8(KTestMessage,"*** Logging test message for CDU performace test***");
_LIT(KLogDefaultFile,"C:\\logs\\log.txt");
_LIT8(KCDUTagTestsIniFileSettings, "\r\nMEDIA FILE\r\nLOGPATH c:\\logs\r\nSynchronous\r\nLOG subsystem component\r\n");

// The value is chosen such that the test runs for more than a minute.
const TInt KNoOfTimesToLog = 10000;

/**
* Function  Name : CCDUPerformanceTest
* Input parameters : None
* Output parameters : None
* Description : Constructor of the class
*/

CCDUPerformanceTest::CCDUPerformanceTest()
	{
	// store the name of this test case
	SetTestStepName(_L("cdu_performance_test"));
	}

/**
* Function  Name :~ CCDUPerformanceTest
* Input parameters : None
* Output parameters : None
* Description : Destructor of the class
*/

CCDUPerformanceTest::~CCDUPerformanceTest()
	{
	}

/**
* Function  Name : doTestStepL
* Input parameters : None
* Output parameters : TVerdict 
* Description : Returns weather the CDU performance test  has passed or failed
*/

TVerdict CCDUPerformanceTest::doTestStepL( )
	{
	INFO_PRINTF1(_L("CDU Performance Test called "));
	
	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	
	User::After(KTimeForDisplay);
	return TestStepResult();
	}

/**
* Function  Name		: executeStepL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: Invokes DoCDUPerformanceTestL which does the performance test for CDU
*/

TInt CCDUPerformanceTest::executeStepL()
	{
	TInt ret = KErrNone;
	ret = DoCDUPerformanceTestL();
	return ret;
	}

/**
* Function  Name		: DoCDUPerformanceTestL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: Uses the static logging to log test data to 
*						log.txt file in a continuous loop. And measures the performance of the
*						logging using the difference of User::Factor() before and after the loop.
*/

TInt CCDUPerformanceTest::DoCDUPerformanceTestL()
	{
#ifndef __WINS__
	// delete log.txt before the start of test
	DeleteLogFileL();
#endif //	__WINS__
	replaceFloggerIniL(KCDUTagTestsIniFileSettings);
// 	Write one test message to get CDU to start up if not already running
	__FLOG_STATIC0(KCDUProfileLogFolder,KCDUProfileLogFile,KTestMessage ); 
	
// 	TTime could be replaced with User::FastCounter later 
//  (User::FastCounter() dont seem to be working at this moment 
	TTime startTime;
	startTime.UniversalTime();
	for (TInt i=0; i<KNoOfTimesToLog; i++)
		{
		__FLOG_STATIC0(KCDUProfileLogFolder,KCDUProfileLogFile,KTestMessage ); 	
		}
	TTime stopTime;
	stopTime.UniversalTime();
	
	TInt64 timeDiff = stopTime.MicroSecondsFrom(startTime).Int64();
	
	INFO_PRINTF2(_L("Time at Test Start = %U microseconds"),startTime.Int64());
	INFO_PRINTF2(_L("Time at Test End = %U microseconds"),stopTime.Int64());	
	INFO_PRINTF2(_L("Time difference from Test start to end = %U microseconds"),timeDiff);
	
#ifndef __WINS__
	// get the file size and display in logs 
	TInt fileSize = GetLogFileSizeL();
	INFO_PRINTF2(_L("Size of file - log.txt = %d bytes"),fileSize);
	// delete log.txt file to create space for next tests
	DeleteLogFileL();
#endif //	__WINS__	
        SetTestStepResult(EPass);

	return TestStepResult();
	}

#ifndef __WINS__
/**
* Function  Name		: GetLogFileSizeL
* Input parameters		: None
* Output parameters		: TUint32 - file size 
* Description 			: Gets the size of the log.txt file
*/
TInt CCDUPerformanceTest::GetLogFileSizeL()
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	
	// To find if the file existed, then only open
	TBool isFileOpen = EFalse;
	TInt err = fs.IsFileOpen(KLogDefaultFile,isFileOpen);

	TInt fileSize = 0;
	if(err == KErrNone)
		{
		RFile file;
		err = file.Open(fs,KLogDefaultFile,EFileRead);
		
		if(err == KErrNone)
			file.Size(fileSize);
		file.Close();	
		}

	CleanupStack::PopAndDestroy(&fs);	
	return fileSize;
	}

/**
* Function  Name		: DeleteLogFileL()
* Input parameters		: None
* Output parameters		: None 
* Description 			: Delets log.txt after the test taking care that
* 						further tests run without stopping aruptly due to 
*						insufficient memory as the log.txt file has grown in size.
*/
void CCDUPerformanceTest::DeleteLogFileL()
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	// ignore error
	fs.Delete(KLogDefaultFile); 
	fs.Close();
	}
#endif // __WINS__

