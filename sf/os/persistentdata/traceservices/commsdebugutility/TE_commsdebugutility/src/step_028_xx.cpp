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
// See the test specification for details of what these test cases do.

#include "teststepcomsdbg.h"
#include "TestMessage.h"
#include "step_028_xx.h"


#include <comms-infras/commsdebugutility.h>

const TInt KWaitForFloggerShutdown = 2000000;
const TInt KMultipleWriteStressTimes = 100;

/**
* 028_Sync_Setup - creates flogger.ini to include token "synchronous"
* doTestStep returns whether test case passed or failed.
*/ 


CFloggerTest028_Sync_Setup::CFloggerTest028_Sync_Setup()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_028_Sync_Setup"));
	}


CFloggerTest028_Sync_Setup::~CFloggerTest028_Sync_Setup()
	{
	}



TVerdict CFloggerTest028_Sync_Setup::doTestStepL( )
	{
	if ( executeStepL() == KErrNone  )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	
	return TestStepResult();
	}


TInt CFloggerTest028_Sync_Setup::executeStepL(TBool)
	{
	return KErrGeneral;
	}

TInt CFloggerTest028_Sync_Setup::executeStepL()
	{
	TInt ret;
	RFileLogger flogger;
	flogger.Connect();
	flogger.__DbgShutDownServer();
	flogger.Close();
	User::After(KWaitForFloggerShutdown);
	TRAP(ret,constructFloggerIniL(KSyncTestsIniFileSettings));
	flogger.Connect();
	flogger.SetLogTags(KStdSubsysTag8, KStdCompTag8);
	flogger.Close();
	return ret;
	}



/**
* 028_Sync_ConWriteUni - test sync with connected unicode write
* doTestStep returns whether test case passed or failed.
*/ 

CFloggerTest028_Sync_ConWriteUni::CFloggerTest028_Sync_ConWriteUni()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_028_Sync_ConWriteUni"));
	}




CFloggerTest028_Sync_ConWriteUni::~CFloggerTest028_Sync_ConWriteUni()
	{
	}




TVerdict CFloggerTest028_Sync_ConWriteUni::doTestStepL( )
	{
	if ( executeStepL() == KErrNone  )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	
	return TestStepResult();
	}



TInt CFloggerTest028_Sync_ConWriteUni::executeStepL(TBool)
	{
	return KErrGeneral;
	}

TInt CFloggerTest028_Sync_ConWriteUni::executeStepL()
	{
	TInt ret;
	
	RFileLogger flogger;
	ret = flogger.Connect();
	
	if ( ret == KErrNone )
		{
		flogger.SetLogTags(KStdSubsysTag8, KStdCompTag8);
		flogger.ClearLog();
		flogger.Write(KTestMessage);
		flogger.Close();
		TRAPD(r, ret = DoTestCheckWriteL());
		if (r != KErrNone)
			ret = r;
		}
	
	return ret;		

	}



TInt CFloggerTest028_Sync_ConWriteUni::DoTestCheckWriteL()
	{
	RFile logFile;
	HBufC8* hBuffer;
	TInt listfilesize,returnCode;
	RFs fileSystem; //For file operation create a file system
	TBuf8<KTestMessageSize> testData; //To hold the test descriptor

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
	
	testData.Copy(KTestMessage); //Copy the test descriptor
	returnCode = ptrString.Find(testData); //find the test descriptor in the buffer read
											//from the file

	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy();	//logFile
	if (returnCode > 0)
		return KErrNone;
	else
		return KErrNotFound;
	}



/**
* 028_Sync_ConWriteUniBound - test sync with connected unicode write and long string
* doTestStep returns whether test case passed or failed.
*/ 

CFloggerTest028_Sync_ConWriteUniBound::CFloggerTest028_Sync_ConWriteUniBound()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_028_Sync_ConWriteUniBound"));
	}




CFloggerTest028_Sync_ConWriteUniBound::~CFloggerTest028_Sync_ConWriteUniBound()
	{
	}




TVerdict CFloggerTest028_Sync_ConWriteUniBound::doTestStepL( )
	{
	if ( executeStepL() == KErrNone  )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	
	return TestStepResult();
	}


TInt CFloggerTest028_Sync_ConWriteUniBound::executeStepL(TBool)
	{
	return KErrGeneral;
	}


TInt CFloggerTest028_Sync_ConWriteUniBound::executeStepL()
	{
	TInt ret;
	
	RFileLogger flogger;
	ret = flogger.Connect();
	
	if ( ret == KErrNone )
		{
		flogger.SetLogTags(KStdSubsysTag8, KStdCompTag8);
		flogger.ClearLog();
		flogger.Write(KTestTooLongMessage16);
		flogger.Close();
		TRAPD(r, ret = DoTestCheckWriteL());
		if (r != KErrNone)
			ret = r;
		}
	
	return ret;		

	}



TInt CFloggerTest028_Sync_ConWriteUniBound::DoTestCheckWriteL()
	{
	RFile logFile;
	HBufC8* hBuffer;
	TInt listfilesize,returnCode;
	RFs fileSystem; //For file operation create a file system
	TBuf8<KLogBufferSize> testData; //We expect the maximum chars this time

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
	
	testData.Copy(KTestTooLongExpectedMessage); //Copy the test descriptor
	returnCode = ptrString.Find(testData); //find the test descriptor in the buffer read
											//from the file

	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy();	//logFile
	if (returnCode > 0)
		return KErrNone;
	else
		return KErrNotFound;
	}


/**
* 028_Sync_ConWriteFormatEUni - test sync with connected unicode write
* doTestStep returns whether test case passed or failed.
*/ 

CFloggerTest028_Sync_ConWriteFormatEUni::CFloggerTest028_Sync_ConWriteFormatEUni()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_028_Sync_ConWriteFormatEUni"));
	}




CFloggerTest028_Sync_ConWriteFormatEUni::~CFloggerTest028_Sync_ConWriteFormatEUni()
	{
	}




TVerdict CFloggerTest028_Sync_ConWriteFormatEUni::doTestStepL( )
	{
	if ( executeStepL() == KErrNone  )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	
	return TestStepResult();
	}

TInt CFloggerTest028_Sync_ConWriteFormatEUni::executeStepL(TBool)
	{
	return KErrGeneral;
	}


TInt CFloggerTest028_Sync_ConWriteFormatEUni::executeStepL()
	{
	TInt ret;
	
	RFileLogger flogger;
	ret = flogger.Connect();
	
	if ( ret == KErrNone )
		{
		flogger.SetLogTags(KStdSubsysTag8, KStdCompTag8);
		flogger.ClearLog();
		flogger.WriteFormat(KTestMessageOneParam16,KTestMessageOneParamValue);
		flogger.Close();
		TRAPD(r, ret = DoTestCheckWriteL());
		if (r != KErrNone)
			ret = r;
		}
	
	return ret;		

	}



TInt CFloggerTest028_Sync_ConWriteFormatEUni::DoTestCheckWriteL()
	{
	RFile logFile;
	HBufC8* hBuffer;
	TInt listfilesize,returnCode;
	RFs fileSystem; //For file operation create a file system
	TBuf8<KTestMessageSize> testData; //To hold the test descriptor

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
	
	testData.Copy(KTestMessageOneParamExpected); //Copy the test descriptor
	returnCode = ptrString.Find(testData); //find the test descriptor in the buffer read
											//from the file

	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy();	//logFile
	if (returnCode > 0)
		return KErrNone;
	else
		return KErrNotFound;
	}



/**
* 028_Sync_ConWriteFormatEUniBound - test sync with connected unicode write and long string
* doTestStep returns whether test case passed or failed.
*/ 

CFloggerTest028_Sync_ConWriteFormatEUniBound::CFloggerTest028_Sync_ConWriteFormatEUniBound()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_028_Sync_ConWriteFormatEUniBound"));
	}




CFloggerTest028_Sync_ConWriteFormatEUniBound::~CFloggerTest028_Sync_ConWriteFormatEUniBound()
	{
	}




TVerdict CFloggerTest028_Sync_ConWriteFormatEUniBound::doTestStepL( )
	{
	if ( executeStepL() == KErrNone  )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	
	return TestStepResult();
	}


TInt CFloggerTest028_Sync_ConWriteFormatEUniBound::executeStepL(TBool)
	{
	return KErrGeneral;
	}

TInt CFloggerTest028_Sync_ConWriteFormatEUniBound::executeStepL()
	{
	TInt ret;
	
	RFileLogger flogger;
	ret = flogger.Connect();
	
	if ( ret == KErrNone )
		{
		flogger.SetLogTags(KStdSubsysTag8, KStdCompTag8);
		flogger.ClearLog();
		flogger.WriteFormat(KTestTooLongMessageOneParam16,KTestTooLongMessageOneParamValue);
		flogger.Close();
		TRAPD(r, ret = DoTestCheckWriteL());
		if (r != KErrNone)
			ret = r;
		}
	
	return ret;		

	}



TInt CFloggerTest028_Sync_ConWriteFormatEUniBound::DoTestCheckWriteL()
	{
	RFile logFile;
	HBufC8* hBuffer;
	TInt listfilesize,returnCode;
	RFs fileSystem; //For file operation create a file system
	TBuf8<KLogBufferSize> testData; //We expect the maximum chars for this case

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
	
	testData.Copy(KTestTooLongMessageOneParamExpected); //Copy the test descriptor
	returnCode = ptrString.Find(testData); //find the test descriptor in the buffer read
											//from the file

	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy();	//logFile
	if (returnCode > 0)
		return KErrNone;
	else
		return KErrNotFound;
	}



/**
* 028_Sync_ConWriteFormatV8Bit - test sync with connected unicode write
* doTestStep returns whether test case passed or failed.
*/ 

CFloggerTest028_Sync_ConWriteFormatV8Bit::CFloggerTest028_Sync_ConWriteFormatV8Bit()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_028_Sync_ConWriteFormatV8Bit"));
	}




CFloggerTest028_Sync_ConWriteFormatV8Bit::~CFloggerTest028_Sync_ConWriteFormatV8Bit()
	{
	}




TVerdict CFloggerTest028_Sync_ConWriteFormatV8Bit::doTestStepL( )
	{
	if ( executeStepL() == KErrNone  )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	
	return TestStepResult();
	}


TInt CFloggerTest028_Sync_ConWriteFormatV8Bit::executeStepL(TBool)
	{
	return KErrGeneral;
	}

TInt CFloggerTest028_Sync_ConWriteFormatV8Bit::executeStepL()
	{
	TInt ret;
	
	ret = flogger.Connect();
	
	if ( ret == KErrNone )
		{
		flogger.SetLogTags(KStdSubsysTag8, KStdCompTag8);
		flogger.ClearLog();
		DoTestWriteFormat(KTestMessageOneParam8,KTestMessageOneParamValue);
		flogger.Close();
		TRAPD(r, ret = DoTestCheckWriteL());
		if (r != KErrNone)
			ret = r;
		}
	
	return ret;		

	}


void CFloggerTest028_Sync_ConWriteFormatV8Bit::DoTestWriteFormat(TRefByValue<const TDesC8> aFmt,...)
	{
	VA_LIST list;
	VA_START(list,aFmt);
	flogger.WriteFormat(aFmt,list);
	}



TInt CFloggerTest028_Sync_ConWriteFormatV8Bit::DoTestCheckWriteL()
	{
	RFile logFile;
	HBufC8* hBuffer;
	TInt listfilesize,returnCode;
	RFs fileSystem; //For file operation create a file system
	TBuf8<KTestMessageSize> testData; //To hold the test descriptor

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
	
	testData.Copy(KTestMessageOneParamExpected); //Copy the test descriptor
	returnCode = ptrString.Find(testData); //find the test descriptor in the buffer read
											//from the file

	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy();	//logFile
	if (returnCode > 0)
		return KErrNone;
	else
		return KErrNotFound;
	}



/**
* 028_Sync_ConWriteFormatV8BitBound - test sync with connected unicode write and long string
* doTestStep returns whether test case passed or failed.
*/ 

CFloggerTest028_Sync_ConWriteFormatV8BitBound::CFloggerTest028_Sync_ConWriteFormatV8BitBound()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_028_Sync_ConWriteFormatV8BitBound"));
	}




CFloggerTest028_Sync_ConWriteFormatV8BitBound::~CFloggerTest028_Sync_ConWriteFormatV8BitBound()
	{
	}




TVerdict CFloggerTest028_Sync_ConWriteFormatV8BitBound::doTestStepL( )
	{
	if ( executeStepL() == KErrNone  )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	
	return TestStepResult();
	}


TInt CFloggerTest028_Sync_ConWriteFormatV8BitBound::executeStepL(TBool)
	{
	return KErrGeneral;
	}

TInt CFloggerTest028_Sync_ConWriteFormatV8BitBound::executeStepL()
	{
	TInt ret;
	
	ret = flogger.Connect();
	
	if ( ret == KErrNone )
		{
		flogger.SetLogTags(KStdSubsysTag8, KStdCompTag8);
		flogger.ClearLog();
		DoTestWriteFormat(KTestTooLongMessageOneParam8,KTestTooLongMessageOneParamValue);
		flogger.Close();
		TRAPD(r, ret = DoTestCheckWriteL());
		if (r != KErrNone)
			ret = r;
		}
	
	return ret;		

	}


void CFloggerTest028_Sync_ConWriteFormatV8BitBound::DoTestWriteFormat(TRefByValue<const TDesC8> aFmt,...)
	{
	VA_LIST list;
	VA_START(list,aFmt);
	flogger.WriteFormat(aFmt,list);
	}


TInt CFloggerTest028_Sync_ConWriteFormatV8BitBound::DoTestCheckWriteL()
	{
	RFile logFile;
	HBufC8* hBuffer;
	TInt listfilesize,returnCode;
	RFs fileSystem; //For file operation create a file system
	TBuf8<KLogBufferSize> testData; //We expect the maximum chars for this case

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
	
	testData.Copy(KTestTooLongMessageOneParamExpected); //Copy the test descriptor
	returnCode = ptrString.Find(testData); //find the test descriptor in the buffer read
											//from the file

	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy();	//logFile
	if (returnCode > 0)
		return KErrNone;
	else
		return KErrNotFound;
	}




/**
* 028_Sync_Static_WriteUni - test sync with connected unicode write
* doTestStep returns whether test case passed or failed.
*/ 

CFloggerTest028_Sync_Static_WriteUni::CFloggerTest028_Sync_Static_WriteUni()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_028_Sync_Static_WriteUni"));
	}




CFloggerTest028_Sync_Static_WriteUni::~CFloggerTest028_Sync_Static_WriteUni()
	{
	}




TVerdict CFloggerTest028_Sync_Static_WriteUni::doTestStepL( )
	{
	if ( executeStepL() == KErrNone  )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	
	return TestStepResult();
	}


TInt CFloggerTest028_Sync_Static_WriteUni::executeStepL(TBool)
	{
	return KErrGeneral;
	}

TInt CFloggerTest028_Sync_Static_WriteUni::executeStepL()
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
		
		RFileLogger::Write(KStdSubsysTag8, KStdCompTag8, KTestMessage);
		
		TRAPD(r, ret = DoTestCheckWriteL());
		if (r != KErrNone)
			ret = r;
		}
		
	
	return ret;		

	}


TInt CFloggerTest028_Sync_Static_WriteUni::DoTestCheckWriteL()
	{
	RFile logFile;
	HBufC8* hBuffer;
	TInt listfilesize,returnCode;
	RFs fileSystem; //For file operation create a file system
	TBuf8<KTestMessageSize> testData; //To hold the test descriptor

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
	
	testData.Copy(KTestMessage); //Copy the test descriptor
	returnCode = ptrString.Find(testData); //find the test descriptor in the buffer read
											//from the file

	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy();	//logFile
	if (returnCode > 0)
		return KErrNone;
	else
		return KErrNotFound;
	}



/**
* 028_Sync_Static_WriteUniBound - test sync with connected unicode write and long string
* doTestStep returns whether test case passed or failed.
*/ 

CFloggerTest028_Sync_Static_WriteUniBound::CFloggerTest028_Sync_Static_WriteUniBound()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_028_Sync_Static_WriteUniBound"));
	}




CFloggerTest028_Sync_Static_WriteUniBound::~CFloggerTest028_Sync_Static_WriteUniBound()
	{
	}




TVerdict CFloggerTest028_Sync_Static_WriteUniBound::doTestStepL( )
	{
	if ( executeStepL() == KErrNone  )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	
	return TestStepResult();
	}


TInt CFloggerTest028_Sync_Static_WriteUniBound::executeStepL(TBool)
	{
	return KErrGeneral;
	}

TInt CFloggerTest028_Sync_Static_WriteUniBound::executeStepL()
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
		
		RFileLogger::Write(KStdSubsysTag8, KStdCompTag8, KTestTooLongMessage16);
		
		TRAPD(r, ret = DoTestCheckWriteL());
		if (r != KErrNone)
			ret = r;
		}
	
	return ret;		
	

	}


TInt CFloggerTest028_Sync_Static_WriteUniBound::DoTestCheckWriteL()
	{
	RFile logFile;
	HBufC8* hBuffer;
	TInt listfilesize,returnCode;
	RFs fileSystem; //For file operation create a file system
	TBuf8<KLogBufferSize> testData; //We expect the maximum chars for this case

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
	
	testData.Copy(KTestTooLongExpectedMessage); //Copy the test descriptor
	returnCode = ptrString.Find(testData); //find the test descriptor in the buffer read
											//from the file

	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy();	//logFile
	if (returnCode > 0)
		return KErrNone;
	else
		return KErrNotFound;
	}





/**
* 028_Sync_Static_WriteFormatVUni - test sync with static formatted VA_LIST unicode write
* doTestStep returns whether test case passed or failed.
*/ 

CFloggerTest028_Sync_Static_WriteFormatVUni::CFloggerTest028_Sync_Static_WriteFormatVUni()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_028_Sync_Static_WriteFormatVUni"));
	}




CFloggerTest028_Sync_Static_WriteFormatVUni::~CFloggerTest028_Sync_Static_WriteFormatVUni()
	{
	}




TVerdict CFloggerTest028_Sync_Static_WriteFormatVUni::doTestStepL( )
	{
	if ( executeStepL() == KErrNone  )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	
	return TestStepResult();
	}


TInt CFloggerTest028_Sync_Static_WriteFormatVUni::executeStepL(TBool)
	{
	return KErrGeneral;
	}

TInt CFloggerTest028_Sync_Static_WriteFormatVUni::executeStepL()
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
		
		DoTestWriteFormat(KTestMessageOneParam16,KTestMessageOneParamValue);
		
		TRAPD(r, ret = DoTestCheckWriteL());
		if (r != KErrNone)
			ret = r;
		}
		
	
	return ret;		

	}


void CFloggerTest028_Sync_Static_WriteFormatVUni::DoTestWriteFormat(TRefByValue<const TDesC16> aFmt,...)
	{
	VA_LIST list;
	VA_START(list,aFmt);
	RFileLogger::WriteFormat(KStdSubsysTag8,KStdCompTag8,aFmt,list);
	}



TInt CFloggerTest028_Sync_Static_WriteFormatVUni::DoTestCheckWriteL()
	{
	RFile logFile;
	HBufC8* hBuffer;
	TInt listfilesize,returnCode;
	RFs fileSystem; //For file operation create a file system
	TBuf8<KTestMessageSize> testData; //To hold the test descriptor

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
	
	testData.Copy(KTestMessageOneParamExpected); //Copy the test descriptor
	returnCode = ptrString.Find(testData); //find the test descriptor in the buffer read
											//from the file

	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy();	//logFile
	if (returnCode > 0)
		return KErrNone;
	else
		return KErrNotFound;
	}



/**
* 028_Sync_Static_WriteFormatVUniBound - test sync with static formatted VA_LIST unicode write and long string
* doTestStep returns whether test case passed or failed.
*/ 

CFloggerTest028_Sync_Static_WriteFormatVUniBound::CFloggerTest028_Sync_Static_WriteFormatVUniBound()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_028_Sync_Static_WriteFormatVUniBound"));
	}




CFloggerTest028_Sync_Static_WriteFormatVUniBound::~CFloggerTest028_Sync_Static_WriteFormatVUniBound()
	{
	}




TVerdict CFloggerTest028_Sync_Static_WriteFormatVUniBound::doTestStepL( )
	{
	if ( executeStepL() == KErrNone  )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	
	return TestStepResult();
	}


TInt CFloggerTest028_Sync_Static_WriteFormatVUniBound::executeStepL(TBool)
	{
	return KErrGeneral;
	}

TInt CFloggerTest028_Sync_Static_WriteFormatVUniBound::executeStepL()
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
		
		DoTestWriteFormat(KTestTooLongMessageOneParam16,KTestTooLongMessageOneParamValue);
		
		TRAPD(r, ret = DoTestCheckWriteL());
		if (r != KErrNone)
			ret = r;
		}
	
	return ret;		
	}



void CFloggerTest028_Sync_Static_WriteFormatVUniBound::DoTestWriteFormat(TRefByValue<const TDesC16> aFmt,...)
	{
	VA_LIST list;
	VA_START(list,aFmt);
	RFileLogger::WriteFormat(KStdSubsysTag8, KStdCompTag8, aFmt,list);
	}
	
	

TInt CFloggerTest028_Sync_Static_WriteFormatVUniBound::DoTestCheckWriteL()
	{
	RFile logFile;
	HBufC8* hBuffer;
	TInt listfilesize,returnCode;
	RFs fileSystem; //For file operation create a file system
	TBuf8<KLogBufferSize> testData; //We expect the maximum chars for this case

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
	
	testData.Copy(KTestTooLongMessageOneParamExpected); //Copy the test descriptor
	returnCode = ptrString.Find(testData); //find the test descriptor in the buffer read
											//from the file

	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy();	//logFile
	if (returnCode > 0)
		return KErrNone;
	else
		return KErrNotFound;
	}




/**
* 028_Sync_Static_HexDump - test sync with connected unicode write
* doTestStep returns whether test case passed or failed.
*/ 

CFloggerTest028_Sync_Static_HexDump::CFloggerTest028_Sync_Static_HexDump()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_028_Sync_Static_HexDump"));
	}




CFloggerTest028_Sync_Static_HexDump::~CFloggerTest028_Sync_Static_HexDump()
	{
	}




TVerdict CFloggerTest028_Sync_Static_HexDump::doTestStepL( )
	{
	if ( executeStepL() == KErrNone  )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	
	return TestStepResult();
	}


TInt CFloggerTest028_Sync_Static_HexDump::executeStepL(TBool)
	{
	return KErrGeneral;
	}

TInt CFloggerTest028_Sync_Static_HexDump::executeStepL()
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
		
		RFileLogger::HexDump(KStdSubsysTag8, KStdCompTag8, KTestMessage8);
		
		TRAPD(r, ret = DoTestCheckWriteL());
		if (r != KErrNone)
			ret = r;
		}
		
	
	return ret;		

	}


TInt CFloggerTest028_Sync_Static_HexDump::DoTestCheckWriteL()
	{
	RFile logFile;
	HBufC8* hBuffer;
	TInt listfilesize,returnCode;
	RFs fileSystem; //For file operation create a file system

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
	
	returnCode = ptrString.Find(KTestMessageAsHex8); //find the test descriptor in the buffer read
											//from the file

	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy();	//logFile
	if (returnCode > 0)
		return KErrNone;
	else
		return KErrNotFound;
	}
	
	
	
/**
* 028_Sync_ClearLog - test sync with clearlog
* doTestStep returns whether test case passed or failed.
*/ 

CFloggerTest028_Sync_ClearLog::CFloggerTest028_Sync_ClearLog()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_028_Sync_ClearLog"));
	}




CFloggerTest028_Sync_ClearLog::~CFloggerTest028_Sync_ClearLog()
	{
	}




TVerdict CFloggerTest028_Sync_ClearLog::doTestStepL( )
	{
	if ( executeStepL() == KErrNone  )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	
	return TestStepResult();
	}


TInt CFloggerTest028_Sync_ClearLog::executeStepL(TBool)
	{
	return KErrGeneral;
	}

TInt CFloggerTest028_Sync_ClearLog::executeStepL()
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
		
		TRAPD(r, ret = DoTestCheckWriteL());
		if (r != KErrNone)
			ret = r;
		}
		
	
	return ret;		

	}


TInt CFloggerTest028_Sync_ClearLog::DoTestCheckWriteL()
	{
	RFile logFile;
	HBufC8* hBuffer;
	TInt listfilesize,returnCode;
	RFs fileSystem; //For file operation create a file system
	TBuf8<KTestMessageSize> testData; //To hold the test descriptor

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
	
	testData.Copy(KTestMessage); //Copy the test descriptor
	returnCode = ptrString.Find(testData); //find the test descriptor in the buffer read
											//from the file

	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy();	//logFile
	if (returnCode == KErrNotFound)
		return KErrNone;
	else
		return KErrUnknown;
	}
	
	
	
	
/**
* 028_Sync_Binary - test sync with write binary
* doTestStep returns whether test case passed or failed.
*/ 

CFloggerTest028_Sync_Binary::CFloggerTest028_Sync_Binary()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_028_Sync_Binary"));
	}




CFloggerTest028_Sync_Binary::~CFloggerTest028_Sync_Binary()
	{
	}




TVerdict CFloggerTest028_Sync_Binary::doTestStepL( )
	{
	if ( executeStepL() == KErrNone  )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	
	return TestStepResult();
	}


TInt CFloggerTest028_Sync_Binary::executeStepL(TBool)
	{
	return KErrGeneral;
	}	

TInt CFloggerTest028_Sync_Binary::executeStepL()
	{
	TInt ret;
	
	// clear the old log messages
	ret = flogger.Connect();
	
	if ( ret == KErrNone )
		{
		flogger.SetLogTags(KStdSubsysTag8, KStdCompTag8);
		flogger.ClearLog();
		
		flogger.WriteBinary(KTestMessage8);
		
		flogger.Close();
		
		TRAPD(r, ret = DoTestCheckWriteL());
		if (r != KErrNone)
			ret = r;
		}
		
	
	return ret;		

	}


TInt CFloggerTest028_Sync_Binary::DoTestCheckWriteL()
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
	
	returnCode = ptrString.Find(KTestMessage8); //find the test descriptor in the buffer read
											//from the file
	if (returnCode > 0)
		{
		numSuccessful++;
		}

	returnCode = ptrString.Find(KStdSubsysTag8); 
	if (returnCode > 0)
		{
		numSuccessful++;
		}

	returnCode = ptrString.Find(KStdCompTag8); 
	if (returnCode > 0)
		{
		numSuccessful++;
		}
		


	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy();	//logFile
	if (numSuccessful ==3)
		return KErrNone;
	else
		return KErrUnknown;
	}
	
	
	
/**
* 028_Sync_ConMultiple - test connect and multiple writes
* doTestStep returns whether test case passed or failed.
*/ 

CFloggerTest028_Sync_ConMultiple::CFloggerTest028_Sync_ConMultiple()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_028_Sync_ConMultiple"));
	}




CFloggerTest028_Sync_ConMultiple::~CFloggerTest028_Sync_ConMultiple()
	{
	}




TVerdict CFloggerTest028_Sync_ConMultiple::doTestStepL( )
	{
	if ( executeStepL() == KErrNone  )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	
	return TestStepResult();
	}

TInt CFloggerTest028_Sync_ConMultiple::executeStepL(TBool)
	{
	return KErrGeneral;
	}


TInt CFloggerTest028_Sync_ConMultiple::executeStepL()
	{
	TInt ret;
	
	// clear the old log messages
	ret = flogger.Connect();
	
	if ( ret == KErrNone )
		{
		flogger.SetLogTags(KStdSubsysTag8, KStdCompTag8);
		flogger.ClearLog();
		
		flogger.Write(KTestMessage);
		flogger.WriteFormat(KTestMessageOneParam16,KTestMessageOneParamValue);
		flogger.Write(KTestTooLongMessage16);
		flogger.WriteFormat(KTestTooLongMessageOneParam16,KTestTooLongMessageOneParamValue);
		DoTestWriteFormat(KTestMessageOneParam8,KTestMessageOneParamValue);
		
		TPtrC8 dataSeg;
		dataSeg.Set(K1KilobyteOfData,K1KilobyteOfDataSize);
		flogger.WriteBinary(dataSeg);
		
		flogger.Close();
		
		TRAPD(r, ret = DoTestCheckWriteL());
		if (r != KErrNone)
			ret = r;
		}
		
	
	return ret;		

	}

void CFloggerTest028_Sync_ConMultiple::DoTestWriteFormat(TRefByValue<const TDesC8> aFmt,...)
	{
	VA_LIST list;
	VA_START(list,aFmt);
	flogger.WriteFormat(aFmt,list);
	}

TInt CFloggerTest028_Sync_ConMultiple::DoTestCheckWriteL()
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

	TPtr8 ptrString = hBuffer->Des();  //To access the buffer

	// Read from position 0: start of file
	User::LeaveIfError(returnCode = logFile.Read(ptrString));
	
	returnCode = ptrString.Find(KTestMessage8); //find the test descriptor in the buffer read
											//from the file
	if (returnCode > 0)
		{
		numSuccessful++;
		}


	returnCode = ptrString.Find(KTestMessageOneParamExpected); 
	if (returnCode > 0)
		{
		numSuccessful++;
		}

	TPtrC8 ptrStringOffsetForSearching = ptrString.Right((ptrString.Length()-returnCode)-KTestMessageOneParamExpected().Length());

	returnCode = ptrStringOffsetForSearching.Find(KTestMessageOneParamExpected); //find the next occurance
	if (returnCode > 0)
		{
		numSuccessful++;
		}

	returnCode = ptrString.Find(KTestTooLongExpectedMessage); 
	if (returnCode > 0)
		{
		numSuccessful++;
		}
		
	returnCode = ptrString.Find(KTestTooLongMessageOneParamExpected); 
	if (returnCode > 0)
		{
		numSuccessful++;
		}

	// we don't check that all the binary data is there - just that the file is nice and long		
	if ( listfilesize > K1KilobyteOfDataSize)
		{
		numSuccessful++;
		}



	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy();	//logFile
	if ( numSuccessful == 6 )
		return KErrNone;
	else
		return KErrUnknown;
	}
	
	
	
	
	
	
/**
* 028_Sync_Static_Multiple - test static multiple writes
* doTestStep returns whether test case passed or failed.
*/ 

CFloggerTest028_Sync_Static_Multiple::CFloggerTest028_Sync_Static_Multiple()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_028_Sync_Static_Multiple"));
	}




CFloggerTest028_Sync_Static_Multiple::~CFloggerTest028_Sync_Static_Multiple()
	{
	}




TVerdict CFloggerTest028_Sync_Static_Multiple::doTestStepL( )
	{
	if ( executeStepL() == KErrNone  )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	
	return TestStepResult();
	}

TInt CFloggerTest028_Sync_Static_Multiple::executeStepL(TBool)
	{
	return KErrGeneral;
	}


TInt CFloggerTest028_Sync_Static_Multiple::executeStepL()
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
		
		RFileLogger::Write(KStdSubsysTag8, KStdCompTag8,KTestMessage);
		RFileLogger::WriteFormat(KStdSubsysTag8, KStdCompTag8,KTestTooLongMessageOneParam16,KTestTooLongMessageOneParamValue);
		DoTestWriteFormat(KTestMessageOneParam16,KTestMessageOneParamValue);
		DoTestWriteFormat(KTestTooLongMessageOneParam16,KTestTooLongMessageOneParamValue);
		
		RFileLogger::HexDump(KStdSubsysTag8, KStdCompTag8,KTestMessage8);
		
		TRAPD(r, ret = DoTestCheckWriteL());
		if (r != KErrNone)
			ret = r;
		}
		
	
	return ret;		

	}

void CFloggerTest028_Sync_Static_Multiple::DoTestWriteFormat(TRefByValue<const TDesC16> aFmt,...)
	{
	VA_LIST list;
	VA_START(list,aFmt);
	RFileLogger::WriteFormat(KStdSubsysTag8, KStdCompTag8,aFmt,list);
	}



TInt CFloggerTest028_Sync_Static_Multiple::DoTestCheckWriteL()
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

	TPtr8 ptrString = hBuffer->Des();  //To access the buffer

	// Read from position 0: start of file
	User::LeaveIfError(returnCode = logFile.Read(ptrString));
	
	returnCode = ptrString.Find(KTestMessage8); //find the test descriptor in the buffer read
											//from the file
	if (returnCode > 0)
		{
		numSuccessful++;
		}

	returnCode = ptrString.Find(KTestTooLongMessageOneParamExpected); 
	if (returnCode > 0)
		{
		numSuccessful++;
		}

	// search for the occurance that the VA_LIST put in
	// when testing serial, the length will be negative, so skip
	TInt len;
	len = (ptrString.Length()-returnCode)-KTestTooLongMessageOneParamExpected().Length();
	if (len > 0)
		{
		TPtrC8 ptrStringOffsetForSearching = ptrString.Right(len);
		
		returnCode = ptrStringOffsetForSearching.Find(KTestTooLongMessageOneParamExpected); //find the next occurance
		if (returnCode > 0)
			{
			numSuccessful++;
			}
		}


	returnCode = ptrString.Find(KTestMessageOneParamExpected); 
	if (returnCode > 0)
		{
		numSuccessful++;
		}

	returnCode = ptrString.Find(KTestMessageAsHex8); 
	if (returnCode > 0)
		{
		numSuccessful++;
		}

	
	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy();	//logFile
	if ( numSuccessful == 5 )
		return KErrNone;
	else
		return KErrUnknown;
	}
	
	
	
/**
* 028_Sync_ConMultiple2 - test connect and multiple2 writes
* doTestStep returns whether test case passed or failed.
*/ 

CFloggerTest028_Sync_ConMultiple2::CFloggerTest028_Sync_ConMultiple2()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_028_Sync_ConMultiple2"));
	}




CFloggerTest028_Sync_ConMultiple2::~CFloggerTest028_Sync_ConMultiple2()
	{
	}




TVerdict CFloggerTest028_Sync_ConMultiple2::doTestStepL( )
	{
	if ( executeStepL() == KErrNone  )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	
	return TestStepResult();
	}

TInt CFloggerTest028_Sync_ConMultiple2::executeStepL(TBool)
	{
	return KErrGeneral;
	}


TInt CFloggerTest028_Sync_ConMultiple2::executeStepL()
	{
	TInt ret;
	
	// clear the old log messages
	ret = flogger.Connect();
	
	if ( ret == KErrNone )
		{
		flogger.SetLogTags(KStdSubsysTag8, KStdCompTag8);
		flogger.ClearLog();
		
		for (TInt loop = 1; loop <= KMultipleWriteStressTimes; loop++)
			{
			flogger.Write(KTestMessage);
			flogger.WriteFormat(KTestMessageOneParam16,KTestMessageOneParamValue);
			flogger.Write(KTestTooLongMessage16);
			flogger.WriteFormat(KTestTooLongMessageOneParam16,KTestTooLongMessageOneParamValue);
			DoTestWriteFormat(KTestTooLongMessageOneParam8,KTestTooLongMessageOneParamValue);
			
			TPtrC8 dataSeg;
			dataSeg.Set(K1KilobyteOfData,K1KilobyteOfDataSize);
			flogger.WriteBinary(dataSeg);
			}
		flogger.Write(KTestEndMessage8);
		
		flogger.Close();
		
		TRAPD(r, ret = DoTestCheckWriteL());
		if (r != KErrNone)
			ret = r;
		}
		
	
	return ret;		

	}

void CFloggerTest028_Sync_ConMultiple2::DoTestWriteFormat(TRefByValue<const TDesC8> aFmt,...)
	{
	VA_LIST list;
	VA_START(list,aFmt);
	flogger.WriteFormat(aFmt,list);
	}

TInt CFloggerTest028_Sync_ConMultiple2::DoTestCheckWriteL()
	{
	RFile logFile;
	HBufC8* hBuffer;
	TInt listfilesize,returnCode;
	RFs fileSystem; //For file operation create a file system
	TInt numSuccessful = 0;
	TInt loop = 0;

	User::LeaveIfError(fileSystem.Connect());
	
	//Open the file in the read mode
	User::LeaveIfError(logFile.Open(fileSystem,KFloggerOutputFile,EFileRead));

	CleanupClosePushL(logFile);

	User::LeaveIfError(logFile.Size(listfilesize)); //Size of the file
	
	hBuffer = HBufC8::New(listfilesize); //Allocate the buffer. This is about 176K in size
	CleanupStack::PushL(hBuffer);

	TPtr8 ptrString = hBuffer->Des();  //To access the buffer

	// Read from position 0: start of file
	User::LeaveIfError(returnCode = logFile.Read(ptrString));
	
	// the test case writes 6 elements 100 times, but we only check these things:
	// 1. that the test message with one parameter is written 100 times
	// 2. that the long message is written 100 times
	// 3. that the end of test message has been written
	// 4. that the file is suitably big enough to be expected to contain the
	//    1K data segment 100 times in it.
	
	TPtrC8 ptrStringOffsetForSearching;
	returnCode = ptrString.Find(KTestMessageOneParamExpected);

	if (returnCode > 0)
		{
		numSuccessful++;
		}
		
	TInt newLength;
	ptrStringOffsetForSearching.Set(ptrString.Right((ptrString.Length()-returnCode)-KTestMessageOneParamExpected().Length()));
	for (loop=0; loop < KMultipleWriteStressTimes; loop++)
		{
		returnCode = ptrStringOffsetForSearching.Find(KTestMessageOneParamExpected); //find the next occurance
		if (returnCode > 0)
			{
			numSuccessful++;
			}
			
		newLength = ptrStringOffsetForSearching.Length() - returnCode - KTestMessageOneParamExpected().Length();
		if (newLength < 0)
			{
			User::Leave(KErrUnknown);
			}
		ptrStringOffsetForSearching.Set(ptrStringOffsetForSearching.Right(newLength));
		
		}

	returnCode = ptrString.Find(KTestTooLongExpectedMessage); 
	
	if (returnCode > 0)
		{
		numSuccessful++;
		}
		
	ptrStringOffsetForSearching.Set(ptrString.Right((ptrString.Length()-returnCode)-KTestTooLongExpectedMessage().Length()));

	for (loop=0; loop < KMultipleWriteStressTimes; loop++)
		{

		returnCode = ptrStringOffsetForSearching.Find(KTestTooLongExpectedMessage); //find the next occurance
		if (returnCode > 0)
			{
			numSuccessful++;
			}
		newLength = ptrStringOffsetForSearching.Length() - returnCode - KTestTooLongExpectedMessage().Length();
		if (newLength <0)
			{
			User::Leave(KErrUnknown);
			}
		ptrStringOffsetForSearching.Set(ptrStringOffsetForSearching.Right(newLength));
			
		}


	returnCode = ptrString.Find(KTestEndMessage8); 
	if (returnCode > 0)
		{
		numSuccessful++;
		}
		

	// we don't check that all the binary data is there - just that the file is nice and long		
	if ( listfilesize > (K1KilobyteOfDataSize * KMultipleWriteStressTimes))
		{
		numSuccessful++;
		}
	


	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy();	//logFile
	if ( numSuccessful == 202 )
		return KErrNone;
	else
		return KErrUnknown;
	}
	
	
	
	
	
	
/**
* 028_Sync_Static_Multiple2 - test static multiple2 writes
* doTestStep returns whether test case passed or failed.
*/ 

CFloggerTest028_Sync_Static_Multiple2::CFloggerTest028_Sync_Static_Multiple2()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_028_Sync_Static_Multiple2"));
	}




CFloggerTest028_Sync_Static_Multiple2::~CFloggerTest028_Sync_Static_Multiple2()
	{
	}




TVerdict CFloggerTest028_Sync_Static_Multiple2::doTestStepL( )
	{
	if ( executeStepL() == KErrNone  )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	
	return TestStepResult();
	}


TInt CFloggerTest028_Sync_Static_Multiple2::executeStepL(TBool)
	{
	return KErrGeneral;
	}

TInt CFloggerTest028_Sync_Static_Multiple2::executeStepL()
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
		
		for (TInt loop = 1; loop <= KMultipleWriteStressTimes; loop++)
			{		
			RFileLogger::Write(KStdSubsysTag8, KStdCompTag8,KTestMessage);
			RFileLogger::Write(KStdSubsysTag8, KStdCompTag8,KTestTooLongMessage16);
			DoTestWriteFormat(KTestMessageOneParam16,KTestMessageOneParamValue);
			DoTestWriteFormat(KTestTooLongMessageOneParam16,KTestTooLongMessageOneParamValue);
			
			RFileLogger::HexDump(KStdSubsysTag8, KStdCompTag8,KTestMessage8);
			}
		RFileLogger::Write(KStdSubsysTag8, KStdCompTag8,KTestEndMessage8);
		
		TRAPD(r, ret = DoTestCheckWriteL());
		if (r != KErrNone)
			ret = r;
		}
		
	
	return ret;		

	}

void CFloggerTest028_Sync_Static_Multiple2::DoTestWriteFormat(TRefByValue<const TDesC16> aFmt,...)
	{
	VA_LIST list;
	VA_START(list,aFmt);
	RFileLogger::WriteFormat(KStdSubsysTag8, KStdCompTag8,aFmt,list);
	}



TInt CFloggerTest028_Sync_Static_Multiple2::DoTestCheckWriteL()
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

	TPtr8 ptrString = hBuffer->Des();  //To access the buffer

	// Read from position 0: start of file
	User::LeaveIfError(returnCode = logFile.Read(ptrString));
	
	
	// the test case writes 5 elements 100 times, but we only check these things:
	// 1. that the long test message with one parameter is written 100 times
	// 2. that the test message with one param is written 100 times
	// 3. that the end of test message has been written
	
	TPtrC8 ptrStringOffsetForSearching;
	returnCode = ptrString.Find(KTestMessageOneParamExpected);

	if (returnCode > 0)
		{
		numSuccessful++;
		}
		
	TInt newLength;   // allow length to be watched during debugging
	TInt loop;
	
	ptrStringOffsetForSearching.Set(ptrString.Right((ptrString.Length()-returnCode)-KTestMessageOneParamExpected().Length()));
	for (loop=0; loop < KMultipleWriteStressTimes; loop++)
		{
		returnCode = ptrStringOffsetForSearching.Find(KTestMessageOneParamExpected); //find the next occurance
		if (returnCode > 0)
			{
			numSuccessful++;
			}
			
		newLength = ptrStringOffsetForSearching.Length() - returnCode - KTestMessageOneParamExpected().Length();
	 	if (newLength < 0)
	 		{
	 		User::Leave(KErrUnknown);
	 		}

		ptrStringOffsetForSearching.Set(ptrStringOffsetForSearching.Right(newLength));
		
		}


	returnCode = ptrString.Find(KTestTooLongMessageOneParamExpected);

	if (returnCode > 0)
		{
		numSuccessful++;
		}
		
	newLength = ptrString.Length()-returnCode-KTestTooLongMessageOneParamExpected().Length();
	ptrStringOffsetForSearching.Set(ptrString.Right(newLength));
	for (loop=0; loop < KMultipleWriteStressTimes; loop++)
		{
		returnCode = ptrStringOffsetForSearching.Find(KTestTooLongMessageOneParamExpected); //find the next occurance
		if (returnCode > 0)
			{
			numSuccessful++;
			}
			
		if (loop < (KMultipleWriteStressTimes - 1))
			{
			newLength = ptrStringOffsetForSearching.Length() - returnCode - KTestTooLongMessageOneParamExpected().Length();
	 		if (newLength < 0)
	 			{
	 			User::Leave(KErrUnknown);
	 			}
			ptrStringOffsetForSearching.Set(ptrStringOffsetForSearching.Right(newLength));
			}
		
		}

	returnCode = ptrString.Find(KTestEndMessage8); 
	if (returnCode > 0)
		{
		numSuccessful++;
		}

	
	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy();	//logFile
	if ( numSuccessful == 201 )
		return KErrNone;
	else
		return KErrUnknown;
	}
