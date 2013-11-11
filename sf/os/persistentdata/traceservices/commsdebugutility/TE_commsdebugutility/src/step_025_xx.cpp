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
// This contains  Unit Test Case 025.01

// EPOC includes
#include <e32base.h>

// Test system includes
#include "teststepcomsdbg.h"
#include "TestMessage.h"
#include "step_025_xx.h"
#include "tlog1.h"
#include "tlog2.h"
#include "tlog3.h"
#include "tlog4.h"


#include <comms-infras/commsdebugutility.h>


/**
* 025_BuildTestUdeb1: Test that flogger builds clients correctly
*/ 

CFloggerTest025_BuildTestUdeb1::CFloggerTest025_BuildTestUdeb1()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_025_BuildTestUdeb1"));
	}




CFloggerTest025_BuildTestUdeb1::~CFloggerTest025_BuildTestUdeb1()
	{
	}




TVerdict CFloggerTest025_BuildTestUdeb1::doTestStepL( )
	{
	if ( executeStepL() == KErrNone  )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);


	
	return TestStepResult();
	}


TInt CFloggerTest025_BuildTestUdeb1::executeStepL(TBool)
	{
	return KErrGeneral;
	}

TInt CFloggerTest025_BuildTestUdeb1::executeStepL()
	{
	
	TInt ret;
	RFileLogger theFlogger;
	
	ret = theFlogger.Connect();
	
	if ( ret == KErrNone)
		{
		theFlogger.SetLogTags(KStdSubsysTag8, KStdCompTag8);
		theFlogger.ClearLog();
		theFlogger.Close();
	
		TestLog1::Test();
		ret = DoTestCheckWriteL();
		}

	return ret;		

	}
	
	
	
TInt CFloggerTest025_BuildTestUdeb1::DoTestCheckWriteL()
	{
	RFile theFile;
	HBufC8 * hBuffer;
	TInt listfilesize;
	TInt returnCode;
	RFs fileSystem; //For file operation create a file system	

	User::After(KTimeToLog);
	
	User::LeaveIfError(fileSystem.Connect());
	
	//Open the file in the read mode
	User::LeaveIfError(theFile.Open(fileSystem,KFloggerOutputFile,EFileRead)); 

	CleanupClosePushL(theFile);	
	
	User::LeaveIfError(returnCode = theFile.Size(listfilesize)); //Size of the file
	hBuffer = HBufC8::New(listfilesize); //Allocate the buffer
	CleanupStack::PushL(hBuffer);
	TPtr8 ptrString = hBuffer->Des();  ; //To hold the buffer

	// Read from position 0: start of file
	returnCode = theFile.Read(ptrString);
	if (returnCode == KErrNone) 
		returnCode = ptrString.Find(KTestMessage8); //find the test descriptor in the buffer read
											//from the file

	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy(); // For theFile object
	if (returnCode > 0)
		return KErrNone;
	else 
		return KErrGeneral;
	}



/**
* 025_BuildTestUdeb2: Test that flogger builds clients correctly
*/ 

// We've had to disable this test case since the building of the class TestLog2 library tlog2.lib causes a linkage
// warning on WINS due to flogger.lib not being used in the debug build, but needed for the release build.

/*
CFloggerTest025_BuildTestUdeb2::CFloggerTest025_BuildTestUdeb2()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_025_BuildTestUdeb2"));
	}




CFloggerTest025_BuildTestUdeb2::~CFloggerTest025_BuildTestUdeb2()
	{
	}




TVerdict CFloggerTest025_BuildTestUdeb2::doTestStepL( )
	{
	if ( executeStepL() == KErrNone  )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);


	
	return TestStepResult();
	}




TInt CFloggerTest025_BuildTestUdeb2::executeStepL()
	{
	
	TInt ret;
	RFileLogger theFlogger;
	
	ret = theFlogger.Connect();
	
	if ( ret == KErrNone)
		{
		theFlogger.SetLogTags(KStdSubsysTag8, KStdCompTag8);
		theFlogger.ClearLog();
		theFlogger.Close();
	
		TestLog2::Test();
		ret = DoTestCheckWriteL();
		}

	return ret;		

	}
	
	
	
TInt CFloggerTest025_BuildTestUdeb2::DoTestCheckWriteL()
	{
	RFile theFile;
	HBufC8 * hBuffer;
	TInt listfilesize;
	TInt returnCode;
	RFs fileSystem; //For file operation create a file system	
	TInt numSuccessful = 0;

	User::After(KTimeToLog);
	
	User::LeaveIfError(fileSystem.Connect());
	
	//Open the file in the read mode
	User::LeaveIfError(theFile.Open(fileSystem,KFloggerOutputFile,EFileRead)); 

	CleanupClosePushL(theFile);	
	
	User::LeaveIfError(returnCode = theFile.Size(listfilesize)); //Size of the file
	hBuffer = HBufC8::New(listfilesize); //Allocate the buffer
	CleanupStack::PushL(hBuffer);
	TPtr8 ptrString = hBuffer->Des();  ; //To hold the buffer

	// Read from position 0: start of file
	returnCode = theFile.Read(ptrString);
	
	// we should not find any log string
	if (returnCode == KErrNone)
		{
		returnCode = ptrString.Find(KTestMessage8);
		if (returnCode == KErrNotFound)
			{
			numSuccessful++;
			}
		}

	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy(); // For theFile object
	if (numSuccessful > 0)
		return KErrNone;
	else 
		return KErrGeneral;
	}

*/

/**
* 025_BuildTestUdeb3: Test that flogger builds clients correctly
*/ 

CFloggerTest025_BuildTestUdeb3::CFloggerTest025_BuildTestUdeb3()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_025_BuildTestUdeb3"));
	}




CFloggerTest025_BuildTestUdeb3::~CFloggerTest025_BuildTestUdeb3()
	{
	}




TVerdict CFloggerTest025_BuildTestUdeb3::doTestStepL( )
	{
	if ( executeStepL() == KErrNone  )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);


	
	return TestStepResult();
	}


TInt CFloggerTest025_BuildTestUdeb3::executeStepL(TBool)
	{
	return KErrGeneral;
	}

TInt CFloggerTest025_BuildTestUdeb3::executeStepL()
	{
	
	TInt ret;
	RFileLogger theFlogger;
	
	ret = theFlogger.Connect();
	
	if ( ret == KErrNone)
		{
		theFlogger.SetLogTags(KStdSubsysTag8, KStdCompTag8);
		theFlogger.ClearLog();
		theFlogger.Close();
	
		TestLog3::Test();
		ret = DoTestCheckWriteL();
		}

	return ret;		

	}
	
	
	
TInt CFloggerTest025_BuildTestUdeb3::DoTestCheckWriteL()
	{
	RFile theFile;
	HBufC8 * hBuffer;
	TInt listfilesize;
	TInt returnCode;
	RFs fileSystem; //For file operation create a file system	
	TInt numSuccessful = 0;

	User::After(KTimeToLog);
	
	User::LeaveIfError(fileSystem.Connect());
	
	//Open the file in the read mode
	User::LeaveIfError(theFile.Open(fileSystem,KFloggerOutputFile,EFileRead)); 

	CleanupClosePushL(theFile);	
	
	User::LeaveIfError(returnCode = theFile.Size(listfilesize)); //Size of the file
	hBuffer = HBufC8::New(listfilesize); //Allocate the buffer
	CleanupStack::PushL(hBuffer);
	TPtr8 ptrString = hBuffer->Des();  ; //To hold the buffer

	// Read from position 0: start of file
	returnCode = theFile.Read(ptrString);
	
	// we should not find any log string
	if (returnCode == KErrNone)
		{
		returnCode = ptrString.Find(KTestMessage8);
		if (returnCode == KErrNotFound)
			{
			numSuccessful++;
			}
		}

	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy(); // For theFile object
	if (numSuccessful > 0)
		return KErrNone;
	else 
		return KErrGeneral;
	}
	
	
	
	
	
	
/**
* 025_BuildTestUdeb4: Test that flogger builds clients correctly
*/ 

CFloggerTest025_BuildTestUdeb4::CFloggerTest025_BuildTestUdeb4()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_025_BuildTestUdeb4"));
	}




CFloggerTest025_BuildTestUdeb4::~CFloggerTest025_BuildTestUdeb4()
	{
	}




TVerdict CFloggerTest025_BuildTestUdeb4::doTestStepL( )
	{
	if ( executeStepL() == KErrNone  )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);


	
	return TestStepResult();
	}


TInt CFloggerTest025_BuildTestUdeb4::executeStepL(TBool)
	{
	return KErrGeneral;
	}

TInt CFloggerTest025_BuildTestUdeb4::executeStepL()
	{
	
	TInt ret;
	RFileLogger theFlogger;
	
	ret = theFlogger.Connect();
	
	if ( ret == KErrNone)
		{
		theFlogger.SetLogTags(KStdSubsysTag8, KStdCompTag8);
		theFlogger.ClearLog();
		theFlogger.Close();
	
		TestLog4::Test();
		ret = DoTestCheckWriteL();
		}

	return ret;		

	}
	
	
	
TInt CFloggerTest025_BuildTestUdeb4::DoTestCheckWriteL()
	{
	RFile theFile;
	HBufC8 * hBuffer;
	TInt listfilesize;
	TInt returnCode;
	RFs fileSystem; //For file operation create a file system	

	User::After(KTimeToLog);
	
	User::LeaveIfError(fileSystem.Connect());
	
	//Open the file in the read mode
	User::LeaveIfError(theFile.Open(fileSystem,KFloggerOutputFile,EFileRead)); 

	CleanupClosePushL(theFile);	
	
	User::LeaveIfError(returnCode = theFile.Size(listfilesize)); //Size of the file
	hBuffer = HBufC8::New(listfilesize); //Allocate the buffer
	CleanupStack::PushL(hBuffer);
	TPtr8 ptrString = hBuffer->Des();  ; //To hold the buffer

	// Read from position 0: start of file
	returnCode = theFile.Read(ptrString);
	if (returnCode == KErrNone) 
		returnCode = ptrString.Find(KTestMessage8); //find the test descriptor in the buffer read
											//from the file

	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy(); // For theFile object
	if (returnCode > 0)
		return KErrNone;
	else 
		return KErrGeneral;
	}




/**
* 025_BuildTestUrel1: Test that flogger builds clients correctly
*/ 

CFloggerTest025_BuildTestUrel1::CFloggerTest025_BuildTestUrel1()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_025_BuildTestUrel1"));
	}




CFloggerTest025_BuildTestUrel1::~CFloggerTest025_BuildTestUrel1()
	{
	}




TVerdict CFloggerTest025_BuildTestUrel1::doTestStepL( )
	{
	if ( executeStepL() == KErrNone  )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);


	
	return TestStepResult();
	}


TInt CFloggerTest025_BuildTestUrel1::executeStepL(TBool)
	{
	return KErrGeneral;
	}

TInt CFloggerTest025_BuildTestUrel1::executeStepL()
	{
	
	TInt ret;
	RFileLogger theFlogger;
	
	ret = theFlogger.Connect();
	
	if ( ret == KErrNone)
		{
		theFlogger.SetLogTags(KStdSubsysTag8, KStdCompTag8);
		theFlogger.ClearLog();
		theFlogger.Close();
	
		TestLog1::Test();
		ret = DoTestCheckWriteL();
		}

	return ret;		

	}
	
	
	
TInt CFloggerTest025_BuildTestUrel1::DoTestCheckWriteL()
	{
	RFile theFile;
	HBufC8 * hBuffer;
	TInt listfilesize;
	TInt returnCode;
	RFs fileSystem; //For file operation create a file system	
	TInt numSuccessful = 0;

	User::After(KTimeToLog);
	
	User::LeaveIfError(fileSystem.Connect());
	
	//Open the file in the read mode
	User::LeaveIfError(theFile.Open(fileSystem,KFloggerOutputFile,EFileRead)); 

	CleanupClosePushL(theFile);	
	
	User::LeaveIfError(returnCode = theFile.Size(listfilesize)); //Size of the file
	hBuffer = HBufC8::New(listfilesize); //Allocate the buffer
	CleanupStack::PushL(hBuffer);
	TPtr8 ptrString = hBuffer->Des();  ; //To hold the buffer

	// Read from position 0: start of file
	returnCode = theFile.Read(ptrString);
	
	// we should not find any log string
	if (returnCode == KErrNone)
		{
		returnCode = ptrString.Find(KTestMessage8);
		if (returnCode == KErrNotFound)
			{
			numSuccessful++;
			}
		}

	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy(); // For theFile object
	if (numSuccessful > 0)
		return KErrNone;
	else 
		return KErrGeneral;
	}


/**
* 025_BuildTestUrel2: Test that flogger builds clients correctly
*/ 

// We've had to disable this test case since the building of the class TestLog2 library tlog2.lib causes a linkage
// warning on WINS due to flogger.lib not being used in the debug build, but needed for the release build.

/*
CFloggerTest025_BuildTestUrel2::CFloggerTest025_BuildTestUrel2()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_025_BuildTestUrel2"));
	}




CFloggerTest025_BuildTestUrel2::~CFloggerTest025_BuildTestUrel2()
	{
	}




TVerdict CFloggerTest025_BuildTestUrel2::doTestStepL( )
	{
	if ( executeStepL() == KErrNone  )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);


	
	return TestStepResult();
	}




TInt CFloggerTest025_BuildTestUrel2::executeStepL()
	{
	
	TInt ret;
	RFileLogger theFlogger;
	
	ret = theFlogger.Connect();
	
	if ( ret == KErrNone)
		{
		theFlogger.SetLogTags(KStdSubsysTag8, KStdCompTag8);
		theFlogger.ClearLog();
		theFlogger.Close();
	
		TestLog2::Test();
		ret = DoTestCheckWriteL();
		}

	return ret;		

	}
	
	
	
TInt CFloggerTest025_BuildTestUrel2::DoTestCheckWriteL()
	{
	RFile theFile;
	HBufC8 * hBuffer;
	TInt listfilesize;
	TInt returnCode;
	RFs fileSystem; //For file operation create a file system	

	User::After(KTimeToLog);
	
	User::LeaveIfError(fileSystem.Connect());
	
	//Open the file in the read mode
	User::LeaveIfError(theFile.Open(fileSystem,KFloggerOutputFile,EFileRead)); 

	CleanupClosePushL(theFile);	
	
	User::LeaveIfError(returnCode = theFile.Size(listfilesize)); //Size of the file
	hBuffer = HBufC8::New(listfilesize); //Allocate the buffer
	CleanupStack::PushL(hBuffer);
	TPtr8 ptrString = hBuffer->Des();  ; //To hold the buffer

	// Read from position 0: start of file
	returnCode = theFile.Read(ptrString);
	if (returnCode == KErrNone) 
		returnCode = ptrString.Find(KTestMessage8); //find the test descriptor in the buffer read
											//from the file

	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy(); // For theFile object
	if (returnCode > 0)
		return KErrNone;
	else 
		return KErrGeneral;
	}

*/

/**
* 025_BuildTestUrel3: Test that flogger builds clients correctly
*/ 

CFloggerTest025_BuildTestUrel3::CFloggerTest025_BuildTestUrel3()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_025_BuildTestUrel3"));
	}




CFloggerTest025_BuildTestUrel3::~CFloggerTest025_BuildTestUrel3()
	{
	}




TVerdict CFloggerTest025_BuildTestUrel3::doTestStepL( )
	{
	if ( executeStepL() == KErrNone  )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);


	
	return TestStepResult();
	}


TInt CFloggerTest025_BuildTestUrel3::executeStepL(TBool)
	{
	return KErrGeneral;
	}

TInt CFloggerTest025_BuildTestUrel3::executeStepL()
	{
	
	TInt ret;
	RFileLogger theFlogger;
	
	ret = theFlogger.Connect();
	
	if ( ret == KErrNone)
		{
		theFlogger.SetLogTags(KStdSubsysTag8, KStdCompTag8);
		theFlogger.ClearLog();
		theFlogger.Close();
	
		TestLog3::Test();
		ret = DoTestCheckWriteL();
		}

	return ret;		

	}
	
	
	
TInt CFloggerTest025_BuildTestUrel3::DoTestCheckWriteL()
	{
	RFile theFile;
	HBufC8 * hBuffer;
	TInt listfilesize;
	TInt returnCode;
	RFs fileSystem; //For file operation create a file system	
	TInt numSuccessful = 0;

	User::After(KTimeToLog);
	
	User::LeaveIfError(fileSystem.Connect());
	
	//Open the file in the read mode
	User::LeaveIfError(theFile.Open(fileSystem,KFloggerOutputFile,EFileRead)); 

	CleanupClosePushL(theFile);	
	
	User::LeaveIfError(returnCode = theFile.Size(listfilesize)); //Size of the file
	hBuffer = HBufC8::New(listfilesize); //Allocate the buffer
	CleanupStack::PushL(hBuffer);
	TPtr8 ptrString = hBuffer->Des();  ; //To hold the buffer

	// Read from position 0: start of file
	returnCode = theFile.Read(ptrString);
	
	// we should not find any log string
	if (returnCode == KErrNone)
		{
		returnCode = ptrString.Find(KTestMessage8);
		if (returnCode == KErrNotFound)
			{
			numSuccessful++;
			}
		}

	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy(); // For theFile object
	if (numSuccessful > 0)
		return KErrNone;
	else 
		return KErrGeneral;
	}
	
	
	
	
	
	
/**
* 025_BuildTestUrel4: Test that flogger builds clients correctly
*/ 

CFloggerTest025_BuildTestUrel4::CFloggerTest025_BuildTestUrel4()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_025_BuildTestUrel4"));
	}




CFloggerTest025_BuildTestUrel4::~CFloggerTest025_BuildTestUrel4()
	{
	}




TVerdict CFloggerTest025_BuildTestUrel4::doTestStepL( )
	{
	if ( executeStepL() == KErrNone  )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);


	
	return TestStepResult();
	}


TInt CFloggerTest025_BuildTestUrel4::executeStepL(TBool)
	{
	return KErrGeneral;
	}

TInt CFloggerTest025_BuildTestUrel4::executeStepL()
	{
	
	TInt ret;
	RFileLogger theFlogger;
	
	ret = theFlogger.Connect();
	
	if ( ret == KErrNone)
		{
		theFlogger.SetLogTags(KStdSubsysTag8, KStdCompTag8);
		theFlogger.ClearLog();
		theFlogger.Close();
	
		TestLog4::Test();
		ret = DoTestCheckWriteL();
		}

	return ret;		

	}
	
	
	
TInt CFloggerTest025_BuildTestUrel4::DoTestCheckWriteL()
	{
	RFile theFile;
	HBufC8 * hBuffer;
	TInt listfilesize;
	TInt returnCode;
	RFs fileSystem; //For file operation create a file system	

	User::After(KTimeToLog);
	
	User::LeaveIfError(fileSystem.Connect());
	
	//Open the file in the read mode
	User::LeaveIfError(theFile.Open(fileSystem,KFloggerOutputFile,EFileRead)); 

	CleanupClosePushL(theFile);	
	
	User::LeaveIfError(returnCode = theFile.Size(listfilesize)); //Size of the file
	hBuffer = HBufC8::New(listfilesize); //Allocate the buffer
	CleanupStack::PushL(hBuffer);
	TPtr8 ptrString = hBuffer->Des();  ; //To hold the buffer

	// Read from position 0: start of file
	returnCode = theFile.Read(ptrString);
	if (returnCode == KErrNone) 
		returnCode = ptrString.Find(KTestMessage8); //find the test descriptor in the buffer read
											//from the file

	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy(); // For theFile object
	if (returnCode > 0)
		return KErrNone;
	else 
		return KErrGeneral;
	}

