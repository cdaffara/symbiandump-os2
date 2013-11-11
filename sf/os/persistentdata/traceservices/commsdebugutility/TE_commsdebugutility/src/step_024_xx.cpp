// Copyright (c) 2003-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// This contains Flogger Unit Test Case 024.xx

// EPOC includes
#include <e32base.h>


// Test system includes
#include "teststepcomsdbg.h"
#include "TestMessage.h"
#include "step_024_xx.h"

#include <comms-infras/commsdebugutility.h>


const TInt KFiveSeconds = 5000000;
const TInt KMaxConnection = 500; ///< specify a lot of connections for the connection stress test 
const TInt KTimeBetConnection = 100000; //Some time between connection
const TInt KDelayToCheckWrite = 10000000; //Give some time for the flogger to write the message before checking. Replaces KTimeToLog which is normally used.


/**
* Function  Name : CFloggerTest024_01
* Input parameters : None
* Output parameters : None
* Description : This is the constructor
*/


CFloggerTest024_01::CFloggerTest024_01()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_024_01"));
	}


/**
* Function  Name :~ CFloggerTest024_01
* Input parameters : None
* Output parameters : None
* Description : This is the Destructor
*/


CFloggerTest024_01::~CFloggerTest024_01()
	{
	}


/**
* Function  Name	: doTestStepL
* Input parameters	: None
* Output parameters : TVerdict 
* Description		: This function returns weather the test case 024_01 has 
* 					  passed or failed

*/


TVerdict CFloggerTest024_01::doTestStepL( )
	{

	INFO_PRINTF1(_L("Step 024.01 called "));
	if ( executeStepL() == KErrNone  )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	
	return TestStepResult();
	}


/**
* Function  Name		: executeStepL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function sets the subsystem and component name
* 						  checks weather they are set in the log file correctly

					  
*/


TInt CFloggerTest024_01::executeStepL()
	{
	return DoTestWrite();
	}
	
/**
* Function  Name		: executeStepL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function sets the subsystem and component name
* 						  checks weather they are set in the log file correctly

					  
*/


TInt CFloggerTest024_01::executeStepL(TBool)
	{
	return KErrGeneral;
	}
/**
* Function  Name		: DoTestWriteL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function checks the weather test data was written
* 						  in to the log file by DoTestWriteL() or not.
					  
*/


TInt CFloggerTest024_01::DoTestWrite()
	{
	TPtrC8 ptrSubSystem;
	TPtrC8 ptrComponent;
	ptrSubSystem.Set(_L8("SubSystem"));
	ptrComponent.Set(_L8("Component"));

	_LIT8(KTestMessage1 ,"TC NO 24: This is test message before sleep");
	_LIT8(KTestMessage2 ,"TC NO 24: This is test message After sleep");

	TInt ret = KErrGeneral;
	RFileLogger theFlogger;

	ret = theFlogger.Connect();
	if (ret == KErrNone)
		{
		ret = theFlogger.SetLogTags(ptrSubSystem, ptrComponent);
		if (ret == KErrNone)
			{
			ret = theFlogger.ClearLog();
			if (ret == KErrNone)
				{
				theFlogger.Write(KTestMessage1); //Write the test descriptor before sleep
				User::After(KFiveSeconds); // Call After() function
				theFlogger.Write(KTestMessage2); //Write the test descriptor after sleep
				User::After(KFiveSeconds);

				TRAPD(r, ret = DoTestCheckWriteL()); // Check whether the message is present in log
			
				
				if (r != KErrNone)
					ret = r;
				}
			}
		}
	if (ret == KErrNone)
		theFlogger.__DbgShutDownServer();
	theFlogger.Close();
	return ret;
	}


/**
* Function  Name		: DoTestCheckWriteL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function checks the weather test data was written
* 						  in to the log file by DoTestWriteL() or not.
					  
*/


TInt CFloggerTest024_01::DoTestCheckWriteL()
	{
	RFile theFile;

	_LIT(KTestString,"Time");

	HBufC8 * hBuffer;
	TInt listfilesize;
	RFs iFileSystem; //For file operation create a file system
	TBuf8<256> testData; //To hold the test descriptor

	User::LeaveIfError(iFileSystem.Connect());
	
	//Open the file in the read mode
	User::LeaveIfError(theFile.Open(iFileSystem, KFloggerOutputFile, EFileWrite|EFileShareAny)); 
	CleanupClosePushL(theFile);
	
	User::LeaveIfError(theFile.Size(listfilesize)); //Size of the file
	
	hBuffer = HBufC8::New(listfilesize); //Allocate the buffer
	CleanupStack::PushL(hBuffer);
	TPtr8 ptrString = hBuffer->Des();  ; //To hold the buffer

	// Read from position 0: start of file
	User::LeaveIfError(theFile.Read(ptrString));
	
	testData.Copy(KTestString); //Copy the test string to be tested

	TPtrC8 ptrTmpBuffer = ptrString.Right(listfilesize); //Copy of the orginal buffer read from the file
	TInt charCount1 = 0;
	TInt charCount2 = 0;
	TInt lineCount = 0;
	TInt tmpCount = 0;
	
	charCount1 = ptrString.Find(testData);
	while(charCount1 != KErrNotFound)
		{
		lineCount++; //Increment the line count since we just found another one
		charCount2 = charCount2 + charCount1 + KSampleTimeLine().Length(); //the the number of char's present in the line which
										// is constant for the line starting with "#Time" in the log file

		tmpCount = listfilesize - charCount2; // Remaining string after the "Time" is read
		TPtrC8 strTmp = ptrString.Right(tmpCount);
		charCount1 = strTmp.Find(testData);
		if (charCount1 == 0) //Since the Time is found at the zeroth position count1 will contain 0
			charCount1 = charCount1 + 4; // Charecter count of Time is 4
		ptrString = ptrTmpBuffer.Right(listfilesize); // Get back the orginal buffer 
		}

	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy();	//theFile

	// we expect to find the word "TIME" 10 times (or more) - 5 during each delay.
	if ((lineCount >= 10) && (lineCount <= 12))
		return KErrNone;
	else
		return KErrGeneral;



	}






_LIT8(KTestStringWithPunctuation1,"                                                 /===-_---~~~~~~~~~------____"); _LIT8(KTestStringWithPunctuation26,"    `~/  )` ) ,/|                 ~-_~>--<_/-__       __-~ _/ ");
_LIT8(KTestStringWithPunctuation2,"                 -==\\\\                         `//~\\\\   ~~~~`---.___.-~~"); _LIT8(KTestStringWithPunctuation27,"   ;'( ')/ ,)(                              ~~~~~~~~~~ ");
_LIT8(KTestStringWithPunctuation3,"       __--~~~  ,-/-==\\\\                        | |   `\\        ,'"); _LIT8(KTestStringWithPunctuation19,"                  \\_|      /        _)   ;  ),   __--~~");
_LIT8(KTestStringWithPunctuation4,"  .'        /       |   \\\\                   /' /        \\   /'");              _LIT8(KTestStringWithPunctuation21,"                  |0  0 _/) )-~     | |__>--<__|      |");
_LIT8(KTestStringWithPunctuation5,"/-'~    ~~~~~---__  |     ~-/~         ( )   /'        _--~`");                    _LIT8(KTestStringWithPunctuation20,"                   {\\__--_/}    / \\\\_>- )<__\\      \\");
_LIT8(KTestStringWithPunctuation6,"                    '~~--_/      _-~/-  / \\   '-~ \\");                           _LIT8(KTestStringWithPunctuation22,"                 o o _//        /-~_>---<__-~      /");
_LIT8(KTestStringWithPunctuation15,"                                                |===-~___                _,-'");  _LIT8(KTestStringWithPunctuation24,"            ( ( '))          |__>--<__|    |                 /' _---_~\\");
_LIT8(KTestStringWithPunctuation16,"             ______-==|                         | |  \\\\           _-~`");       _LIT8(KTestStringWithPunctuation23,"                ,/|           /__>--<__/     _-~");
_LIT8(KTestStringWithPunctuation17,"    _-~       /'    |  \\\\                      / /      \\      /");           _LIT8(KTestStringWithPunctuation25,"        ,/,'//( (             \\__>--<__\\    \\            /'  //        ||");
_LIT8(KTestStringWithPunctuation18," /  ____  /         |    \\`\\.__/-~~ ~ \\ _ _/'  /          \\/'");
_LIT8(KTestStringWithPunctuationExpected1,"         `-)) )) (           |__>--<__|    |               /'  /     ~\\`\\");
_LIT8(KTestStringWithPunctuationExpected2,"                 o o _//        /-~_>---<__-~      /");
_LIT8(KTestStringWithPunctuationExpected3,"    `~/  )` ) ,/|                 ~-_~>--<_/-__       __-~ _/ ");



/**
* CFloggerTest024_02 - test static write with string with beeps, tabs, nulls, CR's, LFs in the tags
* doTestStep returns whether test case passed or failed.
*/ 




CFloggerTest024_02::CFloggerTest024_02()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_024_02"));
	}




CFloggerTest024_02::~CFloggerTest024_02()
	{
	}




TVerdict CFloggerTest024_02::doTestStepL( )
	{
	if ( executeStepL() == KErrNone  )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	
	return TestStepResult();
	}


TInt CFloggerTest024_02::executeStepL(TBool)
	{
	return KErrGeneral;
	}

TInt CFloggerTest024_02::executeStepL()
	{
	TInt ret;
	
	// clear the old log messages
	RFileLogger flogger;
	ret = flogger.Connect();
	
	if ( ret == KErrNone )
		{
		flogger.SetLogTags(KStdSubsysTag8, KStdCompTag8);
		flogger.ClearLog();
		flogger.Close();
		
		RFileLogger::Write(KSubsysTagWithEscapeChars8, KCompTagWithEscapeChars8,KTestMessage8);
		
		User::After(KTimeToLog);
		
		TRAPD(r, ret = DoTestCheckWriteL());
		if (r != KErrNone)
			ret = r;
		}
		
	
	return ret;		

	}


TInt CFloggerTest024_02::DoTestCheckWriteL()
	{
	RFile logFile;
	HBufC8* hBuffer;
	TInt listfilesize,returnCode;
	RFs fileSystem; //For file operation create a file system
	TInt numSuccessful = 0;

	User::LeaveIfError(fileSystem.Connect());
	
	//Open the file in the read mode
	User::LeaveIfError(logFile.Open(fileSystem,KFloggerOutputFile,EFileRead));

	CleanupClosePushL(logFile);

	User::LeaveIfError(logFile.Size(listfilesize)); //Size of the file
	
	hBuffer = HBufC8::New(listfilesize); //Allocate the buffer
	CleanupStack::PushL(hBuffer);

	TPtr8 ptrString = hBuffer->Des();  ; //To access the buffer

	// Read from position 0: start of file
	User::LeaveIfError(returnCode = logFile.Read(ptrString));
	
	// flogger should not have logged
	returnCode = ptrString.Find(KTestMessage8);
	
	if (returnCode == KErrNotFound)
		{
		numSuccessful++;
		}


	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy();	//logFile
	if (numSuccessful == 1)
		return KErrNone;
	else
		return KErrUnknown;
	}




/**
* CFloggerTest024_03 - test static write with string with punctuation and spaces in the data
* doTestStep returns whether test case passed or failed.
*/ 

_LIT8(KTestStringWithPunctuation8,"                  / /~ ,_/       / /__>---<__/      |  ");
_LIT8(KTestStringWithPunctuation14,"  ' ') '( (/");
_LIT8(KTestStringWithPunctuation9,"                 (^(~          /~_>---<__-      _-~");
_LIT8(KTestStringWithPunctuation7,"                   /'   (_/  _-~  | |__>--<__|      | ");
_LIT8(KTestStringWithPunctuation11,"         `-)) )) (           |__>--<__|    |               /'  /     ~\\`\\");
_LIT8(KTestStringWithPunctuation12,"      ,( ( ((, ))              ~-__>--<_~-_  ~--____---~' _/'/        /'");
_LIT8(KTestStringWithPunctuation10,"             ,//('(          |__>--<__|     /                  .----_ ");
_LIT8(KTestStringWithPunctuation13,"  ._-~//( )/ )) `                    ~~-'_/_/ /~~~~~~~__--~ ");


CFloggerTest024_03::CFloggerTest024_03()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_024_03"));
	}




CFloggerTest024_03::~CFloggerTest024_03()
	{
	}




TVerdict CFloggerTest024_03::doTestStepL( )
	{
	if ( executeStepL() == KErrNone  )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	
	return TestStepResult();
	}


TInt CFloggerTest024_03::executeStepL(TBool)
	{
	return KErrGeneral;
	}

TInt CFloggerTest024_03::executeStepL()
	{
	TInt ret;
	
	// clear the old log messages
	RFileLogger flogger;
	ret = flogger.Connect();
	
	if ( ret == KErrNone )
		{
		flogger.SetLogTags(KStdSubsysTag8, KStdCompTag8);
		flogger.ClearLog();
		flogger.Close();
		
		RFileLogger::Write(KStdSubsysTag8, KStdCompTag8,KTestStringWithPunctuation1);
		RFileLogger::Write(KStdSubsysTag8, KStdCompTag8,KTestStringWithPunctuation15);
		RFileLogger::Write(KStdSubsysTag8, KStdCompTag8,KTestStringWithPunctuation2);
		RFileLogger::Write(KStdSubsysTag8, KStdCompTag8,KTestStringWithPunctuation16);
		RFileLogger::Write(KStdSubsysTag8, KStdCompTag8,KTestStringWithPunctuation3);
		RFileLogger::Write(KStdSubsysTag8, KStdCompTag8,KTestStringWithPunctuation17);
		RFileLogger::Write(KStdSubsysTag8, KStdCompTag8,KTestStringWithPunctuation4);
		RFileLogger::Write(KStdSubsysTag8, KStdCompTag8,KTestStringWithPunctuation18);
		RFileLogger::Write(KStdSubsysTag8, KStdCompTag8,KTestStringWithPunctuation5);
		RFileLogger::Write(KStdSubsysTag8, KStdCompTag8,KTestStringWithPunctuation19);
		RFileLogger::Write(KStdSubsysTag8, KStdCompTag8,KTestStringWithPunctuation6);
		RFileLogger::Write(KStdSubsysTag8, KStdCompTag8,KTestStringWithPunctuation20);
		RFileLogger::Write(KStdSubsysTag8, KStdCompTag8,KTestStringWithPunctuation7);
		RFileLogger::Write(KStdSubsysTag8, KStdCompTag8,KTestStringWithPunctuation21);
		RFileLogger::Write(KStdSubsysTag8, KStdCompTag8,KTestStringWithPunctuation8);
		RFileLogger::Write(KStdSubsysTag8, KStdCompTag8,KTestStringWithPunctuation22);
		RFileLogger::Write(KStdSubsysTag8, KStdCompTag8,KTestStringWithPunctuation9);
		RFileLogger::Write(KStdSubsysTag8, KStdCompTag8,KTestStringWithPunctuation23);
		RFileLogger::Write(KStdSubsysTag8, KStdCompTag8,KTestStringWithPunctuation10);
		RFileLogger::Write(KStdSubsysTag8, KStdCompTag8,KTestStringWithPunctuation24);
		RFileLogger::Write(KStdSubsysTag8, KStdCompTag8,KTestStringWithPunctuation11);
		RFileLogger::Write(KStdSubsysTag8, KStdCompTag8,KTestStringWithPunctuation25);
		RFileLogger::Write(KStdSubsysTag8, KStdCompTag8,KTestStringWithPunctuation12);
		RFileLogger::Write(KStdSubsysTag8, KStdCompTag8,KTestStringWithPunctuation26);
		RFileLogger::Write(KStdSubsysTag8, KStdCompTag8,KTestStringWithPunctuation13);
		RFileLogger::Write(KStdSubsysTag8, KStdCompTag8,KTestStringWithPunctuation27);
		RFileLogger::Write(KStdSubsysTag8, KStdCompTag8,KTestStringWithPunctuation14);
		
		User::After(KTimeToLog);
		
		TRAPD(r, ret = DoTestCheckWriteL());
		if (r != KErrNone)
			ret = r;
		}
		
	
	return ret;		

	}


TInt CFloggerTest024_03::DoTestCheckWriteL()
	{
	RFile logFile;
	HBufC8* hBuffer;
	TInt listfilesize,returnCode;
	RFs fileSystem; //For file operation create a file system
	TInt numSuccessful = 0;

	User::LeaveIfError(fileSystem.Connect());
	
	//Open the file in the read mode
	User::LeaveIfError(logFile.Open(fileSystem,KFloggerOutputFile,EFileRead));

	CleanupClosePushL(logFile);

	User::LeaveIfError(logFile.Size(listfilesize)); //Size of the file
	
	hBuffer = HBufC8::New(listfilesize); //Allocate the buffer
	CleanupStack::PushL(hBuffer);

	TPtr8 ptrString = hBuffer->Des();  ; //To access the buffer

	// Read from position 0: start of file
	User::LeaveIfError(returnCode = logFile.Read(ptrString));
	
	// check that flogger logged the punctuation/spaces correctly by checking three samples
	returnCode = ptrString.Find(KTestStringWithPunctuationExpected1);
	
	if (returnCode > 0)
		{
		numSuccessful++;
		}
		
	returnCode = ptrString.Find(KTestStringWithPunctuationExpected2);
	
	if (returnCode > 0)
		{
		numSuccessful++;
		}
		
	returnCode = ptrString.Find(KTestStringWithPunctuationExpected3);
	
	if (returnCode > 0)
		{
		numSuccessful++;
		}


	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy();	//logFile
	if (numSuccessful == 3)
		return KErrNone;
	else
		return KErrUnknown;
	}




/**
* CFloggerTest024_04 - test static write with data string with beeps, tabs, nulls, CR's, LFs in it
* doTestStep returns whether test case passed or failed.
*/ 

CFloggerTest024_04::CFloggerTest024_04()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_024_04"));
	}




CFloggerTest024_04::~CFloggerTest024_04()
	{
	}




TVerdict CFloggerTest024_04::doTestStepL( )
	{
	if ( executeStepL() == KErrNone  )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	
	return TestStepResult();
	}


TInt CFloggerTest024_04::executeStepL(TBool)
	{
	return KErrGeneral;
	}

TInt CFloggerTest024_04::executeStepL()
	{
	TInt ret;
	
	// clear the old log messages
	RFileLogger flogger;
	ret = flogger.Connect();
	
	if ( ret == KErrNone )
		{
		flogger.SetLogTags(KStdSubsysTag8, KStdCompTag8);
		flogger.ClearLog();
		flogger.Close();
		
		RFileLogger::Write(KStdSubsysTag8, KStdCompTag8,KTestMessageWithEscapeChars8);
		
		User::After(KTimeToLog);
		
		TRAPD(r, ret = DoTestCheckWriteL());
		if (r != KErrNone)
			ret = r;
		}
		
	
	return ret;		

	}


TInt CFloggerTest024_04::DoTestCheckWriteL()
	{
	RFile logFile;
	HBufC8* hBuffer;
	TInt listfilesize,returnCode;
	RFs fileSystem; //For file operation create a file system
	TInt numSuccessful = 0;

	User::LeaveIfError(fileSystem.Connect());
	
	//Open the file in the read mode
	User::LeaveIfError(logFile.Open(fileSystem,KFloggerOutputFile,EFileRead));

	CleanupClosePushL(logFile);

	User::LeaveIfError(logFile.Size(listfilesize)); //Size of the file
	
	hBuffer = HBufC8::New(listfilesize); //Allocate the buffer
	CleanupStack::PushL(hBuffer);

	TPtr8 ptrString = hBuffer->Des();  ; //To access the buffer

	// Read from position 0: start of file
	User::LeaveIfError(returnCode = logFile.Read(ptrString));
	
	// flogger will have logged the string exactly as it was sent. The CR/LF will not
	// cause a line feed because they are not adjacent
	returnCode = ptrString.Find(KTestMessageWithEscapeChars8);
	
	if (returnCode > 0)
		{
		numSuccessful++;
		}


	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy();	//logFile
	if (numSuccessful == 1)
		return KErrNone;
	else
		return KErrUnknown;
	}





/**
* CFloggerTest024_05 - test static write with data string with char 255, nulls, and escapes in it
* doTestStep returns whether test case passed or failed.
*/ 

CFloggerTest024_05::CFloggerTest024_05()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_024_05"));
	}




CFloggerTest024_05::~CFloggerTest024_05()
	{
	}




TVerdict CFloggerTest024_05::doTestStepL( )
	{
	if ( executeStepL() == KErrNone  )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	
	return TestStepResult();
	}

TInt CFloggerTest024_05::executeStepL(TBool)
	{
	return KErrGeneral;
	}


TInt CFloggerTest024_05::executeStepL()
	{
	TInt ret;
	
	// clear the old log messages
	RFileLogger flogger;
	ret = flogger.Connect();
	
	if ( ret == KErrNone )
		{
		flogger.SetLogTags(KStdSubsysTag8, KStdCompTag8);
		flogger.ClearLog();
		flogger.Close();
		
		RFileLogger::Write(KStdSubsysTag8, KStdCompTag8,K2ndTestMessageWithEscapeChars8);
		
		User::After(KTimeToLog);
		
		TRAPD(r, ret = DoTestCheckWriteL());
		if (r != KErrNone)
			ret = r;
		}
		
	
	return ret;		

	}


TInt CFloggerTest024_05::DoTestCheckWriteL()
	{
	RFile logFile;
	HBufC8* hBuffer;
	TInt listfilesize,returnCode;
	RFs fileSystem; //For file operation create a file system
	TInt numSuccessful = 0;

	User::LeaveIfError(fileSystem.Connect());
	
	//Open the file in the read mode
	User::LeaveIfError(logFile.Open(fileSystem,KFloggerOutputFile,EFileRead));

	CleanupClosePushL(logFile);

	User::LeaveIfError(logFile.Size(listfilesize)); //Size of the file
	
	hBuffer = HBufC8::New(listfilesize); //Allocate the buffer
	CleanupStack::PushL(hBuffer);

	TPtr8 ptrString = hBuffer->Des();  ; //To access the buffer

	// Read from position 0: start of file
	User::LeaveIfError(returnCode = logFile.Read(ptrString));
	
	// flogger will have logged the string exactly as it was sent. The CR/LF will not
	// cause a line feed because they are not adjacent
	returnCode = ptrString.Find(K2ndTestMessageWithEscapeChars8);
	
	if (returnCode > 0)
		{
		numSuccessful++;
		}


	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy();	//logFile
	if (numSuccessful == 1)
		return KErrNone;
	else
		return KErrUnknown;
	}



/**
* CFloggerTest024_06 - test static write with empty data string
* doTestStep returns whether test case passed or failed.
*/ 

CFloggerTest024_06::CFloggerTest024_06()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_024_06"));
	}




CFloggerTest024_06::~CFloggerTest024_06()
	{
	}




TVerdict CFloggerTest024_06::doTestStepL( )
	{
	if ( executeStepL() == KErrNone  )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	
	return TestStepResult();
	}


TInt CFloggerTest024_06::executeStepL(TBool)
	{
	return KErrGeneral;
	}

TInt CFloggerTest024_06::executeStepL()
	{
	TInt ret;
	
	// clear the old log messages
	RFileLogger flogger;
	ret = flogger.Connect();
	
	if ( ret == KErrNone )
		{
		flogger.SetLogTags(KStdSubsysTag8, KStdCompTag8);
		flogger.ClearLog();
		flogger.Close();
		
		RFileLogger::Write(KStdSubsysTag8, KStdCompTag8,KNullDesC8);
		
		User::After(KTimeToLog);
		
		TRAPD(r, ret = DoTestCheckWriteL());
		if (r != KErrNone)
			ret = r;
		}
		
	
	return ret;		

	}


TInt CFloggerTest024_06::DoTestCheckWriteL()
	{
	RFile logFile;
	HBufC8* hBuffer;
	TInt listfilesize,returnCode;
	RFs fileSystem; //For file operation create a file system
	TInt numSuccessful = 0;

	User::LeaveIfError(fileSystem.Connect());
	
	//Open the file in the read mode
	User::LeaveIfError(logFile.Open(fileSystem,KFloggerOutputFile,EFileRead));

	CleanupClosePushL(logFile);

	User::LeaveIfError(logFile.Size(listfilesize)); //Size of the file
	
	hBuffer = HBufC8::New(listfilesize); //Allocate the buffer
	CleanupStack::PushL(hBuffer);

	TPtr8 ptrString = hBuffer->Des();  ; //To access the buffer

	// Read from position 0: start of file
	User::LeaveIfError(returnCode = logFile.Read(ptrString));
	
	// not much to check since flogger won't have written much other than
	// the tags
	returnCode = ptrString.Find(KStdCompTag8);
	
	if (returnCode > 0)
		{
		numSuccessful++;
		}


	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy();	//logFile
	if (numSuccessful == 1)
		return KErrNone;
	else
		return KErrUnknown;
	}


CFloggerTest024_07::CFloggerTest024_07()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_024_07"));
	}


/**
* Function  Name :~ CFloggerTest024_07
* Input parameters : None
* Output parameters : None
* Description : This is the Destructor
*/ 


CFloggerTest024_07::~CFloggerTest024_07()
	{
	}


/**
* Function  Name	: doTestStepL
* Input parameters	: None
* Output parameters : TVerdict 
* Description		: This function returns whether the test case 024_07 has 
* 					  passed or failed

*/


TVerdict CFloggerTest024_07::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 027.13 called "));
	if ( executeStepL() == KErrNone  )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	INFO_PRINTF1(_L("leaving Step 027.13"));
	User::After(KTimeForDisplay);

	
	return TestStepResult();
	}

/**
* Function  Name		: executeStepL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function checks for the flogger connection for 500 times

*/


TInt CFloggerTest024_07::executeStepL(TBool)
	{
	return KErrGeneral;
	}

/**
* Function  Name		: executeStepL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function checks for the flogger connection for 500 times

*/


TInt CFloggerTest024_07::executeStepL()
	{
	TInt ret = KErrNone;
	ret = connectionTest();
	if (ret == KErrNone)
		{
		TRAPD(r, ret = DoTestCheckWriteL());
		if ( r != KErrNone)
				ret = r;
		}
	return ret;
	}


TInt CFloggerTest024_07::connectionTest()
	{
	
	_LIT8(KTestMessage,"TC 24_15: The flogger connection has been connected %d");
	TPtrC8 ptrSubSystem;
	TPtrC8 ptrComponent;
	ptrSubSystem.Set(_L8("SubSystem"));
	ptrComponent.Set(_L8("Component"));
	TInt res = KErrNone;
	RFileLogger flogger[KMaxConnection];

	for(TInt i= 0; i<KMaxConnection; i++)
		{
		res = flogger[i].Connect();
		if (res == KErrNone)
			{
			res = flogger[i].SetLogTags(ptrSubSystem, ptrComponent);
			if (res == KErrNone)	
				{
				if (i == 0)
					flogger[0].ClearLog();	// Clear previous test cases messages
				flogger[i].WriteFormat(KTestMessage, i);
				}
			else
				return KErrGeneral;
			}
		
		else
			{
			INFO_PRINTF2(_L("Flogger connection failed for connection at -> %d "), i);
			return KErrGeneral;
			}
	User::After(KTimeBetConnection);
		}

	for(TInt j= 0; j<KMaxConnection; j++)
		flogger[j].Close();

	User::After(KDelayToCheckWrite);
	
	return KErrNone;

	}

TInt CFloggerTest024_07::DoTestCheckWriteL()
	{
	User::After(KTimeToLog);
	RFile theFile;
	HBufC8 * hBuffer;
	TInt listfilesize;
	TInt returnCode;
	RFs fileSystem; //For file operation create a file system	
	_LIT(KLogFile,"c:\\logs\\log.txt"); // the log file path

	TBuf8<256> testData; //To hold the test descriptor

	_LIT8(KTestMessage,"TC 24_15: The flogger connection has been connected %d");
	

	User::LeaveIfError(fileSystem.Connect());
	
	//Open the file in the read mode
	User::LeaveIfError(theFile.Open(fileSystem,KLogFile,EFileRead)); 

	CleanupClosePushL(theFile);	
	
	User::LeaveIfError(returnCode = theFile.Size(listfilesize)); //Size of the file
	hBuffer = HBufC8::New(listfilesize); //Allocate the buffer
	CleanupStack::PushL(hBuffer);
	TPtr8 ptrString = hBuffer->Des();  ; //To hold the buffer

	// Read from position 0: start of file
	returnCode = theFile.Read(ptrString);
	
	for(TInt i =0; i<KMaxConnection; i++)
	{
	testData.Format(KTestMessage, i);
	returnCode = ptrString.Find(testData); //find the test descriptor in the buffer read
												//from the file
	if (returnCode > 0)
		continue;
	else
		User::Leave(KErrGeneral);
	}

	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy(); // For theFile object
	if (returnCode > 0)
		return KErrNone;
	else 
		return KErrGeneral;
	}


/**
* 024_08: Test that flogger handles date changes
*/ 


CFloggerTest024_08::CFloggerTest024_08()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_024_08"));
	}




CFloggerTest024_08::~CFloggerTest024_08()
	{
	}




TVerdict CFloggerTest024_08::doTestStepL( )
	{
	if ( executeStepL() == KErrNone  )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	User::After(KTimeForDisplay);

	
	return TestStepResult();
	}


TInt CFloggerTest024_08::executeStepL(TBool)
	{
	return KErrGeneral;
	}

TInt CFloggerTest024_08::executeStepL()
	{
	TInt ret ;
	RFileLogger theFlogger; 
	
	//Just to clear the old log message
	ret = theFlogger.Connect(); 
	if ( ret == KErrNone)
		ret = theFlogger.SetLogTags(KStdSubsysTag8, KStdCompTag8);
		if ( ret == KErrNone)
			ret = theFlogger.ClearLog();
			if ( ret == KErrNone)
				theFlogger.Close();

	// perform test. write some data, roll the date to 0 the following day. write more data.
	if ( ret == KErrNone)
		{
		RFileLogger ::Write(KStdSubsysTag16, KStdCompTag16,EFileLoggingModeAppend, KTestMessage);
		
		TTime time;
		time.HomeTime();
		TInt item;
		TDateTime dateTime(time.DateTime());
		dateTime.SetHour(0);
		dateTime.SetMinute(0);
		dateTime.SetSecond(0);
		item = dateTime.Day();
		if ((dateTime.SetDay(++item) != KErrNone))
			{
			dateTime.SetDay(0);
			item = dateTime.Month();
			if (item == EDecember)
				{
				dateTime.SetMonth(EJanuary);
				item = dateTime.Year();
				dateTime.SetYear(item++);
				}
			else
				{
				dateTime.SetMonth(TMonth(item++));
				}
			}
		time = TTime(dateTime);
	
		User::SetHomeTime(time);
		TTime currentMicrosecs;
		currentMicrosecs.HomeTime();

		RFileLogger ::Write(KStdSubsysTag16, KStdCompTag16,EFileLoggingModeAppend, KTestMessage);
		User::After(KTimeToLog);
		TRAPD(r, ret = DoTestCheckWriteL());
		if (r != KErrNone)
			ret = r;

		}
	return ret;

	}



/**
* This function checks whether test data was written
* 						  in to the log file .				  
*/


TInt CFloggerTest024_08::DoTestCheckWriteL()
	{
	RFile theFile;
	HBufC8 * hBuffer;
	TInt listfilesize,returnCode;
	RFs fileSystem; //For file operation create a file system
	TBuf8<256> testData; //To hold the test descriptor
	TInt numSuccessful = 0;


	_LIT8(KOOMError, "#Logs may be lost out of memory!!");
	
	User::LeaveIfError(fileSystem.Connect());
	
	//Open the file in the read mode
	User::LeaveIfError(theFile.Open(fileSystem,KFloggerOutputFile,EFileRead)); 

	CleanupClosePushL(theFile);

	User::LeaveIfError(theFile.Size(listfilesize)); //Size of the file
	
	hBuffer = HBufC8::New(listfilesize); //Allocate the buffer
	CleanupStack::PushL(hBuffer);

	TPtr8 ptrString = hBuffer->Des();  ; //To hold the buffer

	// Read from position 0: start of file
	User::LeaveIfError(returnCode = theFile.Read(ptrString));
	
	testData.Copy(KTestMessage); //Copy the test descriptor
	returnCode = ptrString.Find(testData); //find the test descriptor in the buffer read
											//from the file
	if (returnCode > 0)
		{
		numSuccessful++;
		}
											
	// We expect to see the string "date change"
	testData.Copy(KDateChangeMessage); //Copy the test descriptor
	returnCode = ptrString.Find(testData); //find the test descriptor in the buffer read

	if (returnCode > 0)
		{
		numSuccessful++;
		}


	if (returnCode == KErrNotFound)
		{
		returnCode = ptrString.Find(KOOMError);
		if (returnCode > 0)
			User::Leave(KErrNoMemory);
		}


	
	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy();	//theFile
	if (numSuccessful == 2)
		return KErrNone;
	else 
		return KErrNotFound;
	}




