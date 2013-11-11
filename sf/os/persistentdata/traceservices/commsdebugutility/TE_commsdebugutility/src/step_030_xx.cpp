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
#include "step_030_xx.h"


#include <comms-infras/commsdebugutility.h>



/**
* This is the Constructor
*/ 


CFloggerTest030_Tag_Setup::CFloggerTest030_Tag_Setup()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_030_Tag_Setup"));
	}


/**
* This is the Destructor
*/ 


CFloggerTest030_Tag_Setup::~CFloggerTest030_Tag_Setup()
	{
	}


/**
* This function returns whether the test case has 
* 					  passed or failed
*/


TVerdict CFloggerTest030_Tag_Setup::doTestStepL( )
	{
	if ( executeStepL() == KErrNone  )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	//INFO_PRINTF1(_L("leaving Step 030_Tag_Setup "));
	User::After(KTimeForDisplay);

	
	return TestStepResult();
	}


TInt CFloggerTest030_Tag_Setup::executeStepL(TBool)
	{
	return KErrGeneral;
	}

TInt CFloggerTest030_Tag_Setup::executeStepL()
	{
	return constructFloggerIniL(KTagTestsIniFileSettings);
	}





/**
* 030_TagSetLogTag01: Test that flogger handles empty tags
*/ 

CFloggerTest030_TagSetLogTag01::CFloggerTest030_TagSetLogTag01()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_030_TagSetLogTag01"));
	}




CFloggerTest030_TagSetLogTag01::~CFloggerTest030_TagSetLogTag01()
	{
	}




TVerdict CFloggerTest030_TagSetLogTag01::doTestStepL( )
	{
	if ( executeStepL() == KErrNone  )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	User::After(KTimeForDisplay);

	
	return TestStepResult();
	}

TInt CFloggerTest030_TagSetLogTag01::executeStepL(TBool)
	{
	return KErrGeneral;
	}


TInt CFloggerTest030_TagSetLogTag01::executeStepL()
	{
	
	TInt ret ;
	RFileLogger theFlogger; 
	ret = theFlogger.Connect();
	
	if ( ret == KErrNone)
		{
		theFlogger.SetLogTags(KStdSubsysTag8, KStdCompTag8);
		theFlogger.ClearLog();
		theFlogger.SetLogTags(KNullDesC8, KNullDesC8);
		theFlogger.Write(KTestMessage);
		theFlogger.Close();
		User::After(KTimeToLog);
		TRAPD(r, ret = DoTestCheckWriteL());
		if (r != KErrNone)
			ret = r;
		}

	return ret;		

	}




TInt CFloggerTest030_TagSetLogTag01::DoTestCheckWriteL()
	{
	RFile theFile;
	HBufC8 * hBuffer;
	TInt listfilesize,returnCode;
	RFs fileSystem; //For file operation create a file system
	TUint numSuccessful = 0;

	
	User::LeaveIfError(fileSystem.Connect());
	CleanupClosePushL(fileSystem);
	
	//Open the file in the read mode
	User::LeaveIfError(theFile.Open(fileSystem,KFloggerOutputFile,EFileRead)); 

	CleanupClosePushL(theFile);

	User::LeaveIfError(theFile.Size(listfilesize)); //Size of the file
	
	hBuffer = HBufC8::New(listfilesize); //Allocate the buffer
	CleanupStack::PushL(hBuffer);

	TPtr8 ptrString = hBuffer->Des();  ; //To hold the buffer

	// Read from position 0: start of file
	User::LeaveIfError(returnCode = theFile.Read(ptrString));
	
	returnCode = ptrString.Find(KTestMessage8); //find the test descriptor in the buffer read
											//from the file

	if (returnCode == KErrNotFound)
		{
		numSuccessful++;
		returnCode = ptrString.Find(KErrOOM);
		if (returnCode > 0)
			User::Leave(KErrNoMemory);
		}

	
	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy();	//theFile
	CleanupStack::PopAndDestroy();  //fileSystem
	
	if (numSuccessful == 1)
		return KErrNone;
	else 
		return KErrNotFound;
	}




/**
* 030_TagSetLogTag02: Test that flogger handles empty subsys tag
*/ 

CFloggerTest030_TagSetLogTag02::CFloggerTest030_TagSetLogTag02()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_030_TagSetLogTag02"));
	}




CFloggerTest030_TagSetLogTag02::~CFloggerTest030_TagSetLogTag02()
	{
	}




TVerdict CFloggerTest030_TagSetLogTag02::doTestStepL( )
	{
	if ( executeStepL() == KErrNone  )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	User::After(KTimeForDisplay);

	
	return TestStepResult();
	}


TInt CFloggerTest030_TagSetLogTag02::executeStepL(TBool)
	{
	return KErrGeneral;
	}

TInt CFloggerTest030_TagSetLogTag02::executeStepL()
	{
	
	TInt ret ;
	RFileLogger theFlogger; 
	ret = theFlogger.Connect();
	
	if ( ret == KErrNone)
		{
		//Create Log by passing the subsystem and component name as parameters
		theFlogger.SetLogTags(KStdSubsysTag8, KStdCompTag8);
		theFlogger.ClearLog();
		theFlogger.SetLogTags(KNullDesC8, KStdCompTag8);
		theFlogger.Write(KTestMessage);
		theFlogger.Close();
		User::After(KTimeToLog);
		TRAPD(r, ret = DoTestCheckWriteL());
		if (r != KErrNone)
			ret = r;
		}

	return ret;		

	}




TInt CFloggerTest030_TagSetLogTag02::DoTestCheckWriteL()
	{
	RFile theFile;
	HBufC8 * hBuffer;
	TInt listfilesize,returnCode;
	RFs fileSystem; //For file operation create a file system
	TUint numSuccessful = 0;

	
	User::LeaveIfError(fileSystem.Connect());
	CleanupClosePushL(fileSystem);
	
	//Open the file in the read mode
	User::LeaveIfError(theFile.Open(fileSystem,KFloggerOutputFile,EFileRead)); 

	CleanupClosePushL(theFile);

	User::LeaveIfError(theFile.Size(listfilesize)); //Size of the file
	
	hBuffer = HBufC8::New(listfilesize); //Allocate the buffer
	CleanupStack::PushL(hBuffer);

	TPtr8 ptrString = hBuffer->Des();  ; //To hold the buffer

	// Read from position 0: start of file
	User::LeaveIfError(returnCode = theFile.Read(ptrString));
	
	returnCode = ptrString.Find(KTestMessage8); //find the test descriptor in the buffer read
											//from the file

	if (returnCode == KErrNotFound)
		{
		numSuccessful++;
		returnCode = ptrString.Find(KErrOOM);
		if (returnCode > 0)
			User::Leave(KErrNoMemory);
		}

	
	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy();	//theFile
	CleanupStack::PopAndDestroy();  //fileSystem
	
	if (numSuccessful == 1)
		return KErrNone;
	else 
		return KErrNotFound;
	}



/**
* 030_TagSetLogTag03: Test that flogger handles empty component tag
*/ 

CFloggerTest030_TagSetLogTag03::CFloggerTest030_TagSetLogTag03()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_030_TagSetLogTag03"));
	}




CFloggerTest030_TagSetLogTag03::~CFloggerTest030_TagSetLogTag03()
	{
	}




TVerdict CFloggerTest030_TagSetLogTag03::doTestStepL( )
	{
	if ( executeStepL() == KErrNone  )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	User::After(KTimeForDisplay);

	
	return TestStepResult();
	}

TInt CFloggerTest030_TagSetLogTag03::executeStepL(TBool)
	{
	return KErrGeneral;
	}


TInt CFloggerTest030_TagSetLogTag03::executeStepL()
	{
	
	TInt ret ;
	RFileLogger theFlogger; 
	ret = theFlogger.Connect();
	
	if ( ret == KErrNone)
		{
		//Create Log by passing the subsystem and component name as parameters
		theFlogger.SetLogTags(KStdSubsysTag8, KStdCompTag8);
		theFlogger.ClearLog();
		theFlogger.SetLogTags(KStdSubsysTag8, KNullDesC8);
		theFlogger.Write(KTestMessage);
		theFlogger.Close();
		User::After(KTimeToLog);
		TRAPD(r, ret = DoTestCheckWriteL());
		if (r != KErrNone)
			ret = r;
		}

	return ret;		

	}




TInt CFloggerTest030_TagSetLogTag03::DoTestCheckWriteL()
	{
	RFile theFile;
	HBufC8 * hBuffer;
	TInt listfilesize,returnCode;
	RFs fileSystem; //For file operation create a file system
	TUint numSuccessful = 0;

	
	User::LeaveIfError(fileSystem.Connect());
	CleanupClosePushL(fileSystem);
	
	//Open the file in the read mode
	User::LeaveIfError(theFile.Open(fileSystem,KFloggerOutputFile,EFileRead)); 

	CleanupClosePushL(theFile);

	User::LeaveIfError(theFile.Size(listfilesize)); //Size of the file
	
	hBuffer = HBufC8::New(listfilesize); //Allocate the buffer
	CleanupStack::PushL(hBuffer);

	TPtr8 ptrString = hBuffer->Des();  ; //To hold the buffer

	// Read from position 0: start of file
	User::LeaveIfError(returnCode = theFile.Read(ptrString));
	
	returnCode = ptrString.Find(KTestMessage8); //find the test descriptor in the buffer read
											//from the file

	if (returnCode == KErrNotFound)
		{
		numSuccessful++;
		returnCode = ptrString.Find(KErrOOM);
		if (returnCode > 0)
			User::Leave(KErrNoMemory);
		}

	
	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy();	//theFile
	CleanupStack::PopAndDestroy();  //fileSystem
	
	if (numSuccessful == 1)
		return KErrNone;
	else 
		return KErrNotFound;
	}




/**
* 030_TagSetLogTag04: Test that flogger handles spaces in tags
*/ 

CFloggerTest030_TagSetLogTag04::CFloggerTest030_TagSetLogTag04()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_030_TagSetLogTag04"));
	}




CFloggerTest030_TagSetLogTag04::~CFloggerTest030_TagSetLogTag04()
	{
	}




TVerdict CFloggerTest030_TagSetLogTag04::doTestStepL( )
	{
	if ( executeStepL() == KErrNone  )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	User::After(KTimeForDisplay);

	
	return TestStepResult();
	}


TInt CFloggerTest030_TagSetLogTag04::executeStepL(TBool)
	{
	return KErrGeneral;
	}

TInt CFloggerTest030_TagSetLogTag04::executeStepL()
	{
	
	TInt ret ;
	RFileLogger theFlogger; 
	ret = theFlogger.Connect();
	
	if ( ret == KErrNone)
		{
		//Create Log by passing the subsystem and component name as parameters
		theFlogger.SetLogTags(KStdSubsysTag8, KStdCompTag8);
		theFlogger.ClearLog();
		theFlogger.SetLogTags(KStdSubsysTag8, KCompTagWithSpaceChars8);
		theFlogger.Write(KTestMessage);
		theFlogger.Close();
		User::After(KTimeToLog);
		TRAPD(r, ret = DoTestCheckWriteL());
		if (r != KErrNone)
			ret = r;
		}

	return ret;		

	}




TInt CFloggerTest030_TagSetLogTag04::DoTestCheckWriteL()
	{
	RFile theFile;
	HBufC8 * hBuffer;
	TInt listfilesize,returnCode;
	RFs fileSystem; //For file operation create a file system
	TUint numSuccessful = 0;

	
	User::LeaveIfError(fileSystem.Connect());
	CleanupClosePushL(fileSystem);
	
	//Open the file in the read mode
	User::LeaveIfError(theFile.Open(fileSystem,KFloggerOutputFile,EFileRead)); 

	CleanupClosePushL(theFile);

	User::LeaveIfError(theFile.Size(listfilesize)); //Size of the file
	
	hBuffer = HBufC8::New(listfilesize); //Allocate the buffer
	CleanupStack::PushL(hBuffer);

	TPtr8 ptrString = hBuffer->Des();  ; //To hold the buffer

	// Read from position 0: start of file
	User::LeaveIfError(returnCode = theFile.Read(ptrString));
	
	returnCode = ptrString.Find(KTestMessage8); //find the test descriptor in the buffer read
											//from the file

	if (returnCode == KErrNotFound)
		{
		numSuccessful++;
		returnCode = ptrString.Find(KErrOOM);
		if (returnCode > 0)
			User::Leave(KErrNoMemory);
		}

	
	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy();	//theFile
	CleanupStack::PopAndDestroy();  //fileSystem
	
	if (numSuccessful == 1)
		return KErrNone;
	else 
		return KErrNotFound;
	}





/**
* 030_TagSetLogTag05: Test that flogger handles punctuation in tags
*/ 

CFloggerTest030_TagSetLogTag05::CFloggerTest030_TagSetLogTag05()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_030_TagSetLogTag05"));
	}




CFloggerTest030_TagSetLogTag05::~CFloggerTest030_TagSetLogTag05()
	{
	}




TVerdict CFloggerTest030_TagSetLogTag05::doTestStepL( )
	{
	if ( executeStepL() == KErrNone  )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	User::After(KTimeForDisplay);

	
	return TestStepResult();
	}


TInt CFloggerTest030_TagSetLogTag05::executeStepL(TBool)
	{
	return KErrGeneral;
	}

TInt CFloggerTest030_TagSetLogTag05::executeStepL()
	{
	
	TInt ret ;
	RFileLogger theFlogger; 
	ret = theFlogger.Connect();
	
	if ( ret == KErrNone)
		{
		//Create Log by passing the subsystem and component name as parameters
		theFlogger.SetLogTags(KStdSubsysTag8, KStdCompTag8);
		theFlogger.ClearLog();
		theFlogger.SetLogTags(KSubsysTagWithPunctuationChars8, KCompTagWithPunctuationChars8);
		theFlogger.Write(KTestMessage);
		theFlogger.Close();
		User::After(KTimeToLog);
		TRAPD(r, ret = DoTestCheckWriteL());
		if (r != KErrNone)
			ret = r;
		}

	return ret;		

	}




TInt CFloggerTest030_TagSetLogTag05::DoTestCheckWriteL()
	{
	RFile theFile;
	HBufC8 * hBuffer;
	TInt listfilesize,returnCode;
	RFs fileSystem; //For file operation create a file system
	TUint numSuccessful = 0;

	
	User::LeaveIfError(fileSystem.Connect());
	CleanupClosePushL(fileSystem);
	
	//Open the file in the read mode
	User::LeaveIfError(theFile.Open(fileSystem,KFloggerOutputFile,EFileRead)); 

	CleanupClosePushL(theFile);

	User::LeaveIfError(theFile.Size(listfilesize)); //Size of the file
	
	hBuffer = HBufC8::New(listfilesize); //Allocate the buffer
	CleanupStack::PushL(hBuffer);

	TPtr8 ptrString = hBuffer->Des();  ; //To hold the buffer

	// Read from position 0: start of file
	User::LeaveIfError(returnCode = theFile.Read(ptrString));
	
	returnCode = ptrString.Find(KTestMessage8); //find the test descriptor in the buffer read
											//from the file

	if (returnCode == KErrNotFound)
		{
		numSuccessful++;
		returnCode = ptrString.Find(KErrOOM);
		if (returnCode > 0)
			User::Leave(KErrNoMemory);
		}

	
	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy();	//theFile
	CleanupStack::PopAndDestroy();  //fileSystem
	
	if (numSuccessful == 1)
		return KErrNone;
	else 
		return KErrNotFound;
	}


/**
* 030_TagSetLogTag06: Test that flogger handles long tags
*/ 

CFloggerTest030_TagSetLogTag06::CFloggerTest030_TagSetLogTag06()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_030_TagSetLogTag06"));
	}




CFloggerTest030_TagSetLogTag06::~CFloggerTest030_TagSetLogTag06()
	{
	}




TVerdict CFloggerTest030_TagSetLogTag06::doTestStepL( )
	{
	if ( executeStepL() == KErrNone  )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	User::After(KTimeForDisplay);

	
	return TestStepResult();
	}


TInt CFloggerTest030_TagSetLogTag06::executeStepL(TBool)
	{
	return KErrGeneral;
	}

TInt CFloggerTest030_TagSetLogTag06::executeStepL()
	{
	
	TInt ret ;
	RFileLogger theFlogger; 
	ret = theFlogger.Connect();
	
	if ( ret == KErrNone)
		{
		//Create Log by passing the subsystem and component name as parameters
		theFlogger.SetLogTags(KStdSubsysTag8, KStdCompTag8);
		theFlogger.ClearLog();
		theFlogger.SetLogTags(KSubsysTag16char8, KCompTag16char8);
		theFlogger.Write(KTestMessage8);
		theFlogger.Close();
		User::After(KTimeToLog);
		TRAPD(r, ret = DoTestCheckWriteL());
		if (r != KErrNone)
			ret = r;
		}

	return ret;		

	}




TInt CFloggerTest030_TagSetLogTag06::DoTestCheckWriteL()
	{
	RFile theFile;
	HBufC8 * hBuffer;
	TInt listfilesize,returnCode;
	RFs fileSystem; //For file operation create a file system
	TUint numSuccessful = 0;

	
	User::LeaveIfError(fileSystem.Connect());
	CleanupClosePushL(fileSystem);
	
	//Open the file in the read mode
	User::LeaveIfError(theFile.Open(fileSystem,KFloggerOutputFile,EFileRead)); 

	CleanupClosePushL(theFile);

	User::LeaveIfError(theFile.Size(listfilesize)); //Size of the file
	
	hBuffer = HBufC8::New(listfilesize); //Allocate the buffer
	CleanupStack::PushL(hBuffer);

	TPtr8 ptrString = hBuffer->Des();  ; //To hold the buffer

	// Read from position 0: start of file
	User::LeaveIfError(returnCode = theFile.Read(ptrString));
	
	returnCode = ptrString.Find(KTestMessage8); //find the test descriptor in the buffer read
											//from the file
	
	if (returnCode > 0)
		{
		numSuccessful++;
		}
	else if (returnCode == KErrNotFound)
		{
		returnCode = ptrString.Find(KErrOOM);
		if (returnCode > 0)
			User::Leave(KErrNoMemory);
		}

	
	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy();	//theFile
	CleanupStack::PopAndDestroy();  //fileSystem
	
	if (numSuccessful == 1)
		return KErrNone;
	else 
		return KErrNotFound;
	}




/**
* 030_TagSetLogTag07: Test that flogger handles long tags
*/ 

CFloggerTest030_TagSetLogTag07::CFloggerTest030_TagSetLogTag07()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_030_TagSetLogTag07"));
	}




CFloggerTest030_TagSetLogTag07::~CFloggerTest030_TagSetLogTag07()
	{
	}




TVerdict CFloggerTest030_TagSetLogTag07::doTestStepL( )
	{
	if ( executeStepL() == KErrNone  )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	User::After(KTimeForDisplay);

	
	return TestStepResult();
	}


TInt CFloggerTest030_TagSetLogTag07::executeStepL(TBool)
	{
	return KErrGeneral;
	}

TInt CFloggerTest030_TagSetLogTag07::executeStepL()
	{
	
	TInt ret ;
	RFileLogger theFlogger; 
	ret = theFlogger.Connect();
	
	if ( ret == KErrNone)
		{
		//Create Log by passing the subsystem and component name as parameters
		theFlogger.SetLogTags(KStdSubsysTag8, KStdCompTag8);
		theFlogger.ClearLog();
		theFlogger.SetLogTags(KSubsysTag17char8, KCompTag17char8);
		theFlogger.Write(KTestMessage8);
		theFlogger.Close();
		User::After(KTimeToLog);
		TRAPD(r, ret = DoTestCheckWriteL());
		if (r != KErrNone)
			ret = r;
		}

	return ret;		

	}




TInt CFloggerTest030_TagSetLogTag07::DoTestCheckWriteL()
	{
	RFile theFile;
	HBufC8 * hBuffer;
	TInt listfilesize,returnCode;
	RFs fileSystem; //For file operation create a file system
	TUint numSuccessful = 0;

	
	User::LeaveIfError(fileSystem.Connect());
	CleanupClosePushL(fileSystem);
	
	//Open the file in the read mode
	User::LeaveIfError(theFile.Open(fileSystem,KFloggerOutputFile,EFileRead)); 

	CleanupClosePushL(theFile);

	User::LeaveIfError(theFile.Size(listfilesize)); //Size of the file
	
	hBuffer = HBufC8::New(listfilesize); //Allocate the buffer
	CleanupStack::PushL(hBuffer);

	TPtr8 ptrString = hBuffer->Des();  ; //To hold the buffer

	// Read from position 0: start of file
	User::LeaveIfError(returnCode = theFile.Read(ptrString));
	
	returnCode = ptrString.Find(KTestMessage8); //find the test descriptor in the buffer read
											//from the file
	
	if (returnCode > 0)
		{
		numSuccessful++;
		}
	else if (returnCode == KErrNotFound)
		{
		returnCode = ptrString.Find(KErrOOM);
		if (returnCode > 0)
			User::Leave(KErrNoMemory);
		}

	// We expect to see the string "subsystem0123456" but not "subsystem01234567"
	returnCode = ptrString.Find(KSubsysTag16char8); //find the test descriptor in the buffer read

	if (returnCode > 0)
		{
		numSuccessful++;
		}

	returnCode = ptrString.Find(KSubsysTag17char8); //find the test descriptor in the buffer read
	
	if (returnCode == KErrNotFound)
		{
		numSuccessful++;
		}
	
	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy();	//theFile
	CleanupStack::PopAndDestroy();  //fileSystem
	
	if (numSuccessful == 3)
		return KErrNone;
	else 
		return KErrNotFound;
	}




/**
* 030_TagSetLogTag08: Test that flogger handles very long tags
*/ 

CFloggerTest030_TagSetLogTag08::CFloggerTest030_TagSetLogTag08()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_030_TagSetLogTag08"));
	}




CFloggerTest030_TagSetLogTag08::~CFloggerTest030_TagSetLogTag08()
	{
	}




TVerdict CFloggerTest030_TagSetLogTag08::doTestStepL( )
	{
	if ( executeStepL() == KErrNone  )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	User::After(KTimeForDisplay);

	
	return TestStepResult();
	}


TInt CFloggerTest030_TagSetLogTag08::executeStepL(TBool)
	{
	return KErrGeneral;
	}

TInt CFloggerTest030_TagSetLogTag08::executeStepL()
	{
	
	TInt ret ;
	RFileLogger theFlogger; 
	ret = theFlogger.Connect();
	
	if ( ret == KErrNone)
		{
		theFlogger.SetLogTags(KStdSubsysTag8, KStdCompTag8);
		theFlogger.ClearLog();
		theFlogger.SetLogTags(KSubsysTag66char8, KCompTag66char8);
		theFlogger.Write(KTestMessage);
		theFlogger.Close();
		User::After(KTimeToLog);
		TRAPD(r, ret = DoTestCheckWriteL());
		if (r != KErrNone)
			ret = r;
		}

	return ret;		

	}




TInt CFloggerTest030_TagSetLogTag08::DoTestCheckWriteL()
	{
	RFile theFile;
	HBufC8 * hBuffer;
	TInt listfilesize,returnCode;
	RFs fileSystem; //For file operation create a file system
	TUint numSuccessful = 0;

	
	User::LeaveIfError(fileSystem.Connect());
	CleanupClosePushL(fileSystem);
	
	//Open the file in the read mode
	User::LeaveIfError(theFile.Open(fileSystem,KFloggerOutputFile,EFileRead)); 

	CleanupClosePushL(theFile);

	User::LeaveIfError(theFile.Size(listfilesize)); //Size of the file
	
	hBuffer = HBufC8::New(listfilesize); //Allocate the buffer
	CleanupStack::PushL(hBuffer);

	TPtr8 ptrString = hBuffer->Des();  ; //To hold the buffer

	// Read from position 0: start of file
	User::LeaveIfError(returnCode = theFile.Read(ptrString));
	
	returnCode = ptrString.Find(KTestMessage8); //find the test descriptor in the buffer read
											//from the file
	
	if (returnCode > 0)
		{
		numSuccessful++;
		}
	else if (returnCode == KErrNotFound)
		{
		returnCode = ptrString.Find(KErrOOM);
		if (returnCode > 0)
			User::Leave(KErrNoMemory);
		}

	// We expect to see the string "subsystem0123456" but not "subsystem01234567"
	returnCode = ptrString.Find(KSubsysTag16char8); //find the test descriptor in the buffer read

	if (returnCode > 0)
		{
		numSuccessful++;
		}
	
	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy();	//theFile
	CleanupStack::PopAndDestroy();  //fileSystem
	
	if (numSuccessful == 2)
		return KErrNone;
	else 
		return KErrNotFound;
	}




/***************************************************************************************/

/**
* 030_TagCreateLog01: Test that flogger handles empty tags
*/ 

CFloggerTest030_TagCreateLog01::CFloggerTest030_TagCreateLog01()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_030_TagCreateLog01"));
	}




CFloggerTest030_TagCreateLog01::~CFloggerTest030_TagCreateLog01()
	{
	}




TVerdict CFloggerTest030_TagCreateLog01::doTestStepL( )
	{
	if ( executeStepL() == KErrNone  )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	User::After(KTimeForDisplay);

	
	return TestStepResult();
	}


TInt CFloggerTest030_TagCreateLog01::executeStepL(TBool)
	{
	return KErrGeneral;
	}

TInt CFloggerTest030_TagCreateLog01::executeStepL()
	{
	
	TInt ret ;
	RFileLogger theFlogger; 
	ret = theFlogger.Connect();
	
	if ( ret == KErrNone)
		{
		theFlogger.SetLogTags(KStdSubsysTag8, KStdCompTag8);
		theFlogger.ClearLog();
		theFlogger.CreateLog(KNullDesC16, KNullDesC16, EFileLoggingModeOverwrite);
		theFlogger.Write(KTestMessage);
		theFlogger.Close();
		User::After(KTimeToLog);
		TRAPD(r, ret = DoTestCheckWriteL());
		if (r != KErrNone)
			ret = r;
		}

	return ret;		

	}




TInt CFloggerTest030_TagCreateLog01::DoTestCheckWriteL()
	{
	RFile theFile;
	HBufC8 * hBuffer;
	TInt listfilesize,returnCode;
	RFs fileSystem; //For file operation create a file system
	TUint numSuccessful = 0;

	
	User::LeaveIfError(fileSystem.Connect());
	CleanupClosePushL(fileSystem);
	
	//Open the file in the read mode
	User::LeaveIfError(theFile.Open(fileSystem,KFloggerOutputFile,EFileRead)); 

	CleanupClosePushL(theFile);

	User::LeaveIfError(theFile.Size(listfilesize)); //Size of the file
	
	hBuffer = HBufC8::New(listfilesize); //Allocate the buffer
	CleanupStack::PushL(hBuffer);

	TPtr8 ptrString = hBuffer->Des();  ; //To hold the buffer

	// Read from position 0: start of file
	User::LeaveIfError(returnCode = theFile.Read(ptrString));
	
	returnCode = ptrString.Find(KTestMessage8); //find the test descriptor in the buffer read
											//from the file

	if (returnCode == KErrNotFound)
		{
		numSuccessful++;
		returnCode = ptrString.Find(KErrOOM);
		if (returnCode > 0)
			User::Leave(KErrNoMemory);
		}

	
	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy();	//theFile
	CleanupStack::PopAndDestroy();  //fileSystem
	
	if (numSuccessful == 1)
		return KErrNone;
	else 
		return KErrNotFound;
	}




/**
* 030_TagCreateLog02: Test that flogger handles empty subsys tag
*/ 

CFloggerTest030_TagCreateLog02::CFloggerTest030_TagCreateLog02()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_030_TagCreateLog02"));
	}




CFloggerTest030_TagCreateLog02::~CFloggerTest030_TagCreateLog02()
	{
	}




TVerdict CFloggerTest030_TagCreateLog02::doTestStepL( )
	{
	if ( executeStepL() == KErrNone  )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	User::After(KTimeForDisplay);

	
	return TestStepResult();
	}

TInt CFloggerTest030_TagCreateLog02::executeStepL(TBool)
	{
	return KErrGeneral;
	}


TInt CFloggerTest030_TagCreateLog02::executeStepL()
	{
	
	TInt ret ;
	RFileLogger theFlogger; 
	ret = theFlogger.Connect();
	
	if ( ret == KErrNone)
		{
		//Create Log by passing the subsystem and component name as parameters
		theFlogger.SetLogTags(KStdSubsysTag8, KStdCompTag8);
		theFlogger.ClearLog();
		theFlogger.CreateLog(KNullDesC16, KStdCompTag16, EFileLoggingModeOverwrite);
		theFlogger.Write(KTestMessage);
		theFlogger.Close();
		User::After(KTimeToLog);
		TRAPD(r, ret = DoTestCheckWriteL());
		if (r != KErrNone)
			ret = r;
		}

	return ret;		

	}




TInt CFloggerTest030_TagCreateLog02::DoTestCheckWriteL()
	{
	RFile theFile;
	HBufC8 * hBuffer;
	TInt listfilesize,returnCode;
	RFs fileSystem; //For file operation create a file system
	TUint numSuccessful = 0;

	
	User::LeaveIfError(fileSystem.Connect());
	CleanupClosePushL(fileSystem);
	
	//Open the file in the read mode
	User::LeaveIfError(theFile.Open(fileSystem,KFloggerOutputFile,EFileRead)); 

	CleanupClosePushL(theFile);

	User::LeaveIfError(theFile.Size(listfilesize)); //Size of the file
	
	hBuffer = HBufC8::New(listfilesize); //Allocate the buffer
	CleanupStack::PushL(hBuffer);

	TPtr8 ptrString = hBuffer->Des();  ; //To hold the buffer

	// Read from position 0: start of file
	User::LeaveIfError(returnCode = theFile.Read(ptrString));
	
	returnCode = ptrString.Find(KTestMessage8); //find the test descriptor in the buffer read
											//from the file

	if (returnCode == KErrNotFound)
		{
		numSuccessful++;
		returnCode = ptrString.Find(KErrOOM);
		if (returnCode > 0)
			User::Leave(KErrNoMemory);
		}

	
	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy();	//theFile
	CleanupStack::PopAndDestroy();  //fileSystem
	
	if (numSuccessful == 1)
		return KErrNone;
	else 
		return KErrNotFound;
	}



/**
* 030_TagCreateLog03: Test that flogger handles empty component tag
*/ 

CFloggerTest030_TagCreateLog03::CFloggerTest030_TagCreateLog03()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_030_TagCreateLog03"));
	}




CFloggerTest030_TagCreateLog03::~CFloggerTest030_TagCreateLog03()
	{
	}




TVerdict CFloggerTest030_TagCreateLog03::doTestStepL( )
	{
	if ( executeStepL() == KErrNone  )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	User::After(KTimeForDisplay);

	
	return TestStepResult();
	}


TInt CFloggerTest030_TagCreateLog03::executeStepL(TBool)
	{
	return KErrGeneral;
	}

TInt CFloggerTest030_TagCreateLog03::executeStepL()
	{
	
	TInt ret ;
	RFileLogger theFlogger; 
	ret = theFlogger.Connect();
	
	if ( ret == KErrNone)
		{
		//Create Log by passing the subsystem and component name as parameters
		theFlogger.SetLogTags(KStdSubsysTag8, KStdCompTag8);
		theFlogger.ClearLog();
		theFlogger.CreateLog(KStdSubsysTag16, KNullDesC16, EFileLoggingModeOverwrite);
		theFlogger.Write(KTestMessage);
		theFlogger.Close();
		User::After(KTimeToLog);
		TRAPD(r, ret = DoTestCheckWriteL());
		if (r != KErrNone)
			ret = r;
		}

	return ret;		

	}




TInt CFloggerTest030_TagCreateLog03::DoTestCheckWriteL()
	{
	RFile theFile;
	HBufC8 * hBuffer;
	TInt listfilesize,returnCode;
	RFs fileSystem; //For file operation create a file system
	TUint numSuccessful = 0;

	
	User::LeaveIfError(fileSystem.Connect());
	CleanupClosePushL(fileSystem);
	
	//Open the file in the read mode
	User::LeaveIfError(theFile.Open(fileSystem,KFloggerOutputFile,EFileRead)); 

	CleanupClosePushL(theFile);

	User::LeaveIfError(theFile.Size(listfilesize)); //Size of the file
	
	hBuffer = HBufC8::New(listfilesize); //Allocate the buffer
	CleanupStack::PushL(hBuffer);

	TPtr8 ptrString = hBuffer->Des();  ; //To hold the buffer

	// Read from position 0: start of file
	User::LeaveIfError(returnCode = theFile.Read(ptrString));
	
	returnCode = ptrString.Find(KTestMessage8); //find the test descriptor in the buffer read
											//from the file

	if (returnCode == KErrNotFound)
		{
		numSuccessful++;
		returnCode = ptrString.Find(KErrOOM);
		if (returnCode > 0)
			User::Leave(KErrNoMemory);
		}

	
	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy();	//theFile
	CleanupStack::PopAndDestroy();  //fileSystem
	
	if (numSuccessful == 1)
		return KErrNone;
	else 
		return KErrNotFound;
	}




/**
* 030_TagCreateLog04: Test that flogger handles spaces in tags
*/ 

CFloggerTest030_TagCreateLog04::CFloggerTest030_TagCreateLog04()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_030_TagCreateLog04"));
	}




CFloggerTest030_TagCreateLog04::~CFloggerTest030_TagCreateLog04()
	{
	}




TVerdict CFloggerTest030_TagCreateLog04::doTestStepL( )
	{
	if ( executeStepL() == KErrNone  )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	User::After(KTimeForDisplay);

	
	return TestStepResult();
	}


TInt CFloggerTest030_TagCreateLog04::executeStepL(TBool)
	{
	return KErrGeneral;
	}

TInt CFloggerTest030_TagCreateLog04::executeStepL()
	{
	
	TInt ret ;
	RFileLogger theFlogger; 
	ret = theFlogger.Connect();
	
	if ( ret == KErrNone)
		{
		//Create Log by passing the subsystem and component name as parameters
		theFlogger.SetLogTags(KStdSubsysTag8, KStdCompTag8);
		theFlogger.ClearLog();
		theFlogger.CreateLog(KStdSubsysTag16, KCompTagWithSpaceChars16, EFileLoggingModeOverwrite);
		theFlogger.Write(KTestMessage);
		theFlogger.Close();
		User::After(KTimeToLog);
		TRAPD(r, ret = DoTestCheckWriteL());
		if (r != KErrNone)
			ret = r;
		}

	return ret;		

	}




TInt CFloggerTest030_TagCreateLog04::DoTestCheckWriteL()
	{
	RFile theFile;
	HBufC8 * hBuffer;
	TInt listfilesize,returnCode;
	RFs fileSystem; //For file operation create a file system
	TUint numSuccessful = 0;

	
	User::LeaveIfError(fileSystem.Connect());
	CleanupClosePushL(fileSystem);
	
	//Open the file in the read mode
	User::LeaveIfError(theFile.Open(fileSystem,KFloggerOutputFile,EFileRead)); 

	CleanupClosePushL(theFile);

	User::LeaveIfError(theFile.Size(listfilesize)); //Size of the file
	
	hBuffer = HBufC8::New(listfilesize); //Allocate the buffer
	CleanupStack::PushL(hBuffer);

	TPtr8 ptrString = hBuffer->Des();  ; //To hold the buffer

	// Read from position 0: start of file
	User::LeaveIfError(returnCode = theFile.Read(ptrString));
	
	returnCode = ptrString.Find(KTestMessage8); //find the test descriptor in the buffer read
											//from the file

	if (returnCode == KErrNotFound)
		{
		numSuccessful++;
		returnCode = ptrString.Find(KErrOOM);
		if (returnCode > 0)
			User::Leave(KErrNoMemory);
		}

	
	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy();	//theFile
	CleanupStack::PopAndDestroy();  //fileSystem
	
	if (numSuccessful == 1)
		return KErrNone;
	else 
		return KErrNotFound;
	}





/**
* 030_TagCreateLog05: Test that flogger handles punctuation in tags
*/ 

CFloggerTest030_TagCreateLog05::CFloggerTest030_TagCreateLog05()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_030_TagCreateLog05"));
	}




CFloggerTest030_TagCreateLog05::~CFloggerTest030_TagCreateLog05()
	{
	}




TVerdict CFloggerTest030_TagCreateLog05::doTestStepL( )
	{
	if ( executeStepL() == KErrNone  )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	User::After(KTimeForDisplay);

	
	return TestStepResult();
	}


TInt CFloggerTest030_TagCreateLog05::executeStepL(TBool)
{
return KErrGeneral;
}

TInt CFloggerTest030_TagCreateLog05::executeStepL()
	{
	
	TInt ret ;
	RFileLogger theFlogger; 
	ret = theFlogger.Connect();
	
	if ( ret == KErrNone)
		{
		//Create Log by passing the subsystem and component name as parameters
		theFlogger.SetLogTags(KStdSubsysTag8, KStdCompTag8);
		theFlogger.ClearLog();
		theFlogger.CreateLog(KSubsysTagWithPunctuationChars16, KCompTagWithPunctuationChars16, EFileLoggingModeOverwrite);
		theFlogger.Write(KTestMessage);
		theFlogger.Close();
		User::After(KTimeToLog);
		TRAPD(r, ret = DoTestCheckWriteL());
		if (r != KErrNone)
			ret = r;
		}

	return ret;		

	}




TInt CFloggerTest030_TagCreateLog05::DoTestCheckWriteL()
	{
	RFile theFile;
	HBufC8 * hBuffer;
	TInt listfilesize,returnCode;
	RFs fileSystem; //For file operation create a file system
	TUint numSuccessful = 0;

	
	User::LeaveIfError(fileSystem.Connect());
	CleanupClosePushL(fileSystem);
	
	//Open the file in the read mode
	User::LeaveIfError(theFile.Open(fileSystem,KFloggerOutputFile,EFileRead)); 

	CleanupClosePushL(theFile);

	User::LeaveIfError(theFile.Size(listfilesize)); //Size of the file
	
	hBuffer = HBufC8::New(listfilesize); //Allocate the buffer
	CleanupStack::PushL(hBuffer);

	TPtr8 ptrString = hBuffer->Des();  ; //To hold the buffer

	// Read from position 0: start of file
	User::LeaveIfError(returnCode = theFile.Read(ptrString));
	
	returnCode = ptrString.Find(KTestMessage8); //find the test descriptor in the buffer read
											//from the file

	if (returnCode == KErrNotFound)
		{
		numSuccessful++;
		returnCode = ptrString.Find(KErrOOM);
		if (returnCode > 0)
			User::Leave(KErrNoMemory);
		}

	
	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy();	//theFile
	CleanupStack::PopAndDestroy();  //fileSystem
	
	if (numSuccessful == 1)
		return KErrNone;
	else 
		return KErrNotFound;
	}



/**
* 030_TagCreateLog06: Test that flogger handles length 16 tags
*/ 

CFloggerTest030_TagCreateLog06::CFloggerTest030_TagCreateLog06()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_030_TagCreateLog06"));
	}




CFloggerTest030_TagCreateLog06::~CFloggerTest030_TagCreateLog06()
	{
	}




TVerdict CFloggerTest030_TagCreateLog06::doTestStepL( )
	{
	if ( executeStepL() == KErrNone  )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	User::After(KTimeForDisplay);

	
	return TestStepResult();
	}


TInt CFloggerTest030_TagCreateLog06::executeStepL(TBool)
	{
	return KErrGeneral;
	}	

TInt CFloggerTest030_TagCreateLog06::executeStepL()
	{
	
	TInt ret ;
	RFileLogger theFlogger; 
	ret = theFlogger.Connect();
	
	if ( ret == KErrNone)
		{
		theFlogger.SetLogTags(KStdSubsysTag8, KStdCompTag8);
		theFlogger.ClearLog();
		theFlogger.CreateLog(KSubsysTag16char16, KCompTag16char16, EFileLoggingModeOverwrite); 
		theFlogger.Write(KTestMessage);
		theFlogger.Close();
		User::After(KTimeToLog);
		TRAPD(r, ret = DoTestCheckWriteL());
		if (r != KErrNone)
			ret = r;
		}

	return ret;		

	}





TInt CFloggerTest030_TagCreateLog06::DoTestCheckWriteL()
	{
	RFile theFile;
	HBufC8 * hBuffer;
	TInt listfilesize,returnCode;
	RFs fileSystem; //For file operation create a file system
	TBuf8<256> testData; //To hold the test descriptor


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

	if (returnCode == KErrNotFound)
		{
		returnCode = ptrString.Find(KOOMError);
		if (returnCode > 0)
			User::Leave(KErrNoMemory);
		}

	
	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy();	//theFile
	if (returnCode > 0)
		return KErrNone;
	else 
		return KErrNotFound;
	}








/**
* 030_TagCreateLog07: Test that flogger handles length 17 tags by truncating
*/ 

CFloggerTest030_TagCreateLog07::CFloggerTest030_TagCreateLog07()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_030_TagCreateLog07"));
	}




CFloggerTest030_TagCreateLog07::~CFloggerTest030_TagCreateLog07()
	{
	}




TVerdict CFloggerTest030_TagCreateLog07::doTestStepL( )
	{
	if ( executeStepL() == KErrNone  )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	User::After(KTimeForDisplay);

	
	return TestStepResult();
	}


TInt CFloggerTest030_TagCreateLog07::executeStepL(TBool)
	{
	return KErrGeneral;
	}

TInt CFloggerTest030_TagCreateLog07::executeStepL()
	{
	
	TInt ret ;
	RFileLogger theFlogger; 
	ret = theFlogger.Connect();
	
	if ( ret == KErrNone)
		{
		theFlogger.SetLogTags(KStdSubsysTag8, KStdCompTag8);
		theFlogger.ClearLog();
		theFlogger.CreateLog(KSubsysTag17char16, KCompTag17char16, EFileLoggingModeOverwrite); 
		theFlogger.Write(KTestMessage);
		theFlogger.Close();
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


TInt CFloggerTest030_TagCreateLog07::DoTestCheckWriteL()
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
											
	// We expect to see the string "subsystem0123456" but not "subsystem01234567"
	testData.Copy(KSubsysTag16char16); //Copy the test descriptor
	returnCode = ptrString.Find(testData); //find the test descriptor in the buffer read

	if (returnCode > 0)
		{
		numSuccessful++;
		}

	testData.Copy(KSubsysTag17char16); //Copy the test descriptor
	returnCode = ptrString.Find(testData); //find the test descriptor in the buffer read
	
	if (returnCode == KErrNotFound)
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
	if (numSuccessful == 3)
		return KErrNone;
	else 
		return KErrNotFound;
	}


/**
* 030_TagCreateLog08: Test that flogger handles very long tags
*/ 

CFloggerTest030_TagCreateLog08::CFloggerTest030_TagCreateLog08()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_030_TagCreateLog08"));
	}




CFloggerTest030_TagCreateLog08::~CFloggerTest030_TagCreateLog08()
	{
	}




TVerdict CFloggerTest030_TagCreateLog08::doTestStepL( )
	{
	if ( executeStepL() == KErrNone  )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	User::After(KTimeForDisplay);

	
	return TestStepResult();
	}


TInt CFloggerTest030_TagCreateLog08::executeStepL(TBool)
	{
	return KErrGeneral;
	}

TInt CFloggerTest030_TagCreateLog08::executeStepL()
	{
	
	TInt ret ;
	RFileLogger theFlogger; 
	ret = theFlogger.Connect();
	
	if ( ret == KErrNone)
		{
		theFlogger.SetLogTags(KStdSubsysTag8, KStdCompTag8);
		theFlogger.ClearLog();
		theFlogger.CreateLog(KSubsysTag66char16, KCompTag66char16, EFileLoggingModeOverwrite);
		theFlogger.Write(KTestMessage);
		theFlogger.Close();
		User::After(KTimeToLog);
		TRAPD(r, ret = DoTestCheckWriteL());
		if (r != KErrNone)
			ret = r;
		}

	return ret;		

	}




TInt CFloggerTest030_TagCreateLog08::DoTestCheckWriteL()
	{
	RFile theFile;
	HBufC8 * hBuffer;
	TInt listfilesize,returnCode;
	RFs fileSystem; //For file operation create a file system
	TUint numSuccessful = 0;

	
	User::LeaveIfError(fileSystem.Connect());
	CleanupClosePushL(fileSystem);
	
	//Open the file in the read mode
	User::LeaveIfError(theFile.Open(fileSystem,KFloggerOutputFile,EFileRead)); 

	CleanupClosePushL(theFile);

	User::LeaveIfError(theFile.Size(listfilesize)); //Size of the file
	
	hBuffer = HBufC8::New(listfilesize); //Allocate the buffer
	CleanupStack::PushL(hBuffer);

	TPtr8 ptrString = hBuffer->Des();  ; //To hold the buffer

	// Read from position 0: start of file
	User::LeaveIfError(returnCode = theFile.Read(ptrString));
	
	returnCode = ptrString.Find(KTestMessage8); //find the test descriptor in the buffer read
											//from the file
	
	if (returnCode > 0)
		{
		numSuccessful++;
		}
	else if (returnCode == KErrNotFound)
		{
		returnCode = ptrString.Find(KErrOOM);
		if (returnCode > 0)
			User::Leave(KErrNoMemory);
		}

	// We expect to see the string "subsystem0123456" but not "subsystem01234567"
	returnCode = ptrString.Find(KSubsysTag16char8); //find the test descriptor in the buffer read

	if (returnCode > 0)
		{
		numSuccessful++;
		}
	
	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy();	//theFile
	CleanupStack::PopAndDestroy();  //fileSystem
	
	if (numSuccessful == 2)
		return KErrNone;
	else 
		return KErrNotFound;
	}




/***************************************************************************************/


/**
* 030_TagWrite01: Test that flogger handles empty tags
*/ 

CFloggerTest030_TagWrite01::CFloggerTest030_TagWrite01()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_030_TagWrite01"));
	}




CFloggerTest030_TagWrite01::~CFloggerTest030_TagWrite01()
	{
	}




TVerdict CFloggerTest030_TagWrite01::doTestStepL( )
	{
	if ( executeStepL() == KErrNone  )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	User::After(KTimeForDisplay);

	
	return TestStepResult();
	}


TInt CFloggerTest030_TagWrite01::executeStepL(TBool)
	{
	return KErrGeneral;
	}

TInt CFloggerTest030_TagWrite01::executeStepL()
	{
	
	TInt ret ;
	RFileLogger theFlogger; 
	ret = theFlogger.Connect();
	
	if ( ret == KErrNone)
		{
		theFlogger.SetLogTags(KStdSubsysTag8, KStdCompTag8);
		theFlogger.ClearLog();
		theFlogger.Close();
		RFileLogger::Write(KNullDesC8, KNullDesC8,KTestMessage);
		User::After(KTimeToLog);
		TRAPD(r, ret = DoTestCheckWriteL());
		if (r != KErrNone)
			ret = r;
		}

	return ret;		

	}




TInt CFloggerTest030_TagWrite01::DoTestCheckWriteL()
	{
	RFile theFile;
	HBufC8 * hBuffer;
	TInt listfilesize,returnCode;
	RFs fileSystem; //For file operation create a file system
	TUint numSuccessful = 0;

	
	User::LeaveIfError(fileSystem.Connect());
	CleanupClosePushL(fileSystem);
	
	//Open the file in the read mode
	User::LeaveIfError(theFile.Open(fileSystem,KFloggerOutputFile,EFileRead)); 

	CleanupClosePushL(theFile);

	User::LeaveIfError(theFile.Size(listfilesize)); //Size of the file
	
	hBuffer = HBufC8::New(listfilesize); //Allocate the buffer
	CleanupStack::PushL(hBuffer);

	TPtr8 ptrString = hBuffer->Des();  ; //To hold the buffer

	// Read from position 0: start of file
	User::LeaveIfError(returnCode = theFile.Read(ptrString));
	
	returnCode = ptrString.Find(KTestMessage8); //find the test descriptor in the buffer read
											//from the file

	if (returnCode == KErrNotFound)
		{
		numSuccessful++;
		returnCode = ptrString.Find(KErrOOM);
		if (returnCode > 0)
			User::Leave(KErrNoMemory);
		}

	
	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy();	//theFile
	CleanupStack::PopAndDestroy();  //fileSystem
	
	if (numSuccessful == 1)
		return KErrNone;
	else 
		return KErrNotFound;
	}




/**
* 030_TagWrite02: Test that flogger handles empty subsys tag
*/ 

CFloggerTest030_TagWrite02::CFloggerTest030_TagWrite02()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_030_TagWrite02"));
	}




CFloggerTest030_TagWrite02::~CFloggerTest030_TagWrite02()
	{
	}




TVerdict CFloggerTest030_TagWrite02::doTestStepL( )
	{
	if ( executeStepL() == KErrNone  )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	User::After(KTimeForDisplay);

	
	return TestStepResult();
	}


TInt CFloggerTest030_TagWrite02::executeStepL(TBool)
	{
	return KErrGeneral;
	}

TInt CFloggerTest030_TagWrite02::executeStepL()
	{
	
	TInt ret ;
	RFileLogger theFlogger; 
	ret = theFlogger.Connect();
	
	if ( ret == KErrNone)
		{
		//Create Log by passing the subsystem and component name as parameters
		theFlogger.SetLogTags(KStdSubsysTag8, KStdCompTag8);
		theFlogger.ClearLog();
		theFlogger.Close();
		RFileLogger::Write(KNullDesC8, KStdCompTag8,KTestMessage);
		User::After(KTimeToLog);
		TRAPD(r, ret = DoTestCheckWriteL());
		if (r != KErrNone)
			ret = r;
		}

	return ret;		

	}




TInt CFloggerTest030_TagWrite02::DoTestCheckWriteL()
	{
	RFile theFile;
	HBufC8 * hBuffer;
	TInt listfilesize,returnCode;
	RFs fileSystem; //For file operation create a file system
	TUint numSuccessful = 0;

	
	User::LeaveIfError(fileSystem.Connect());
	CleanupClosePushL(fileSystem);
	
	//Open the file in the read mode
	User::LeaveIfError(theFile.Open(fileSystem,KFloggerOutputFile,EFileRead)); 

	CleanupClosePushL(theFile);

	User::LeaveIfError(theFile.Size(listfilesize)); //Size of the file
	
	hBuffer = HBufC8::New(listfilesize); //Allocate the buffer
	CleanupStack::PushL(hBuffer);

	TPtr8 ptrString = hBuffer->Des();  ; //To hold the buffer

	// Read from position 0: start of file
	User::LeaveIfError(returnCode = theFile.Read(ptrString));
	
	returnCode = ptrString.Find(KTestMessage8); //find the test descriptor in the buffer read
											//from the file

	if (returnCode == KErrNotFound)
		{
		numSuccessful++;
		returnCode = ptrString.Find(KErrOOM);
		if (returnCode > 0)
			User::Leave(KErrNoMemory);
		}

	
	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy();	//theFile
	CleanupStack::PopAndDestroy();  //fileSystem
	
	if (numSuccessful == 1)
		return KErrNone;
	else 
		return KErrNotFound;
	}



/**
* 030_TagWrite03: Test that flogger handles empty component tag
*/ 

CFloggerTest030_TagWrite03::CFloggerTest030_TagWrite03()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_030_TagWrite03"));
	}




CFloggerTest030_TagWrite03::~CFloggerTest030_TagWrite03()
	{
	}




TVerdict CFloggerTest030_TagWrite03::doTestStepL( )
	{
	if ( executeStepL() == KErrNone  )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	User::After(KTimeForDisplay);

	
	return TestStepResult();
	}


TInt CFloggerTest030_TagWrite03::executeStepL(TBool)
	{
	return KErrGeneral;
	}

TInt CFloggerTest030_TagWrite03::executeStepL()
	{
	
	TInt ret ;
	RFileLogger theFlogger; 
	ret = theFlogger.Connect();
	
	if ( ret == KErrNone)
		{
		//Create Log by passing the subsystem and component name as parameters
		theFlogger.SetLogTags(KStdSubsysTag8, KStdCompTag8);
		theFlogger.ClearLog();
		theFlogger.Close();
		RFileLogger::Write(KStdSubsysTag8, KNullDesC8,KTestMessage);
		User::After(KTimeToLog);
		TRAPD(r, ret = DoTestCheckWriteL());
		if (r != KErrNone)
			ret = r;
		}

	return ret;		

	}




TInt CFloggerTest030_TagWrite03::DoTestCheckWriteL()
	{
	RFile theFile;
	HBufC8 * hBuffer;
	TInt listfilesize,returnCode;
	RFs fileSystem; //For file operation create a file system
	TUint numSuccessful = 0;

	
	User::LeaveIfError(fileSystem.Connect());
	CleanupClosePushL(fileSystem);
	
	//Open the file in the read mode
	User::LeaveIfError(theFile.Open(fileSystem,KFloggerOutputFile,EFileRead)); 

	CleanupClosePushL(theFile);

	User::LeaveIfError(theFile.Size(listfilesize)); //Size of the file
	
	hBuffer = HBufC8::New(listfilesize); //Allocate the buffer
	CleanupStack::PushL(hBuffer);

	TPtr8 ptrString = hBuffer->Des();  ; //To hold the buffer

	// Read from position 0: start of file
	User::LeaveIfError(returnCode = theFile.Read(ptrString));
	
	returnCode = ptrString.Find(KTestMessage8); //find the test descriptor in the buffer read
											//from the file

	if (returnCode == KErrNotFound)
		{
		numSuccessful++;
		returnCode = ptrString.Find(KErrOOM);
		if (returnCode > 0)
			User::Leave(KErrNoMemory);
		}

	
	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy();	//theFile
	CleanupStack::PopAndDestroy();  //fileSystem
	
	if (numSuccessful == 1)
		return KErrNone;
	else 
		return KErrNotFound;
	}




/**
* 030_TagWrite04: Test that flogger handles spaces in tags
*/ 

CFloggerTest030_TagWrite04::CFloggerTest030_TagWrite04()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_030_TagWrite04"));
	}




CFloggerTest030_TagWrite04::~CFloggerTest030_TagWrite04()
	{
	}




TVerdict CFloggerTest030_TagWrite04::doTestStepL( )
	{
	if ( executeStepL() == KErrNone  )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	User::After(KTimeForDisplay);

	
	return TestStepResult();
	}


TInt CFloggerTest030_TagWrite04::executeStepL(TBool)
	{
	return KErrGeneral;
	}

TInt CFloggerTest030_TagWrite04::executeStepL()
	{
	
	TInt ret ;
	RFileLogger theFlogger; 
	ret = theFlogger.Connect();
	
	if ( ret == KErrNone)
		{
		//Create Log by passing the subsystem and component name as parameters
		theFlogger.SetLogTags(KStdSubsysTag8, KStdCompTag8);
		theFlogger.ClearLog();
		theFlogger.Close();
		RFileLogger::Write(KStdSubsysTag8, KCompTagWithSpaceChars8,KTestMessage);
		User::After(KTimeToLog);
		TRAPD(r, ret = DoTestCheckWriteL());
		if (r != KErrNone)
			ret = r;
		}

	return ret;		

	}




TInt CFloggerTest030_TagWrite04::DoTestCheckWriteL()
	{
	RFile theFile;
	HBufC8 * hBuffer;
	TInt listfilesize,returnCode;
	RFs fileSystem; //For file operation create a file system
	TUint numSuccessful = 0;

	
	User::LeaveIfError(fileSystem.Connect());
	CleanupClosePushL(fileSystem);
	
	//Open the file in the read mode
	User::LeaveIfError(theFile.Open(fileSystem,KFloggerOutputFile,EFileRead)); 

	CleanupClosePushL(theFile);

	User::LeaveIfError(theFile.Size(listfilesize)); //Size of the file
	
	hBuffer = HBufC8::New(listfilesize); //Allocate the buffer
	CleanupStack::PushL(hBuffer);

	TPtr8 ptrString = hBuffer->Des();  ; //To hold the buffer

	// Read from position 0: start of file
	User::LeaveIfError(returnCode = theFile.Read(ptrString));
	
	returnCode = ptrString.Find(KTestMessage8); //find the test descriptor in the buffer read
											//from the file

	if (returnCode == KErrNotFound)
		{
		numSuccessful++;
		returnCode = ptrString.Find(KErrOOM);
		if (returnCode > 0)
			User::Leave(KErrNoMemory);
		}

	
	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy();	//theFile
	CleanupStack::PopAndDestroy();  //fileSystem
	
	if (numSuccessful == 1)
		return KErrNone;
	else 
		return KErrNotFound;
	}





/**
* 030_TagWrite05: Test that flogger handles punctuation in tags
*/ 

CFloggerTest030_TagWrite05::CFloggerTest030_TagWrite05()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_030_TagWrite05"));
	}




CFloggerTest030_TagWrite05::~CFloggerTest030_TagWrite05()
	{
	}




TVerdict CFloggerTest030_TagWrite05::doTestStepL( )
	{
	if ( executeStepL() == KErrNone  )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	User::After(KTimeForDisplay);

	
	return TestStepResult();
	}

TInt CFloggerTest030_TagWrite05::executeStepL(TBool)
	{
	return KErrGeneral;
	}


TInt CFloggerTest030_TagWrite05::executeStepL()
	{
	
	TInt ret ;
	RFileLogger theFlogger; 
	ret = theFlogger.Connect();
	
	if ( ret == KErrNone)
		{
		//Create Log by passing the subsystem and component name as parameters
		theFlogger.SetLogTags(KStdSubsysTag8, KStdCompTag8);
		theFlogger.ClearLog();
		theFlogger.Close();
		RFileLogger::Write(KSubsysTagWithPunctuationChars8, KCompTagWithPunctuationChars8,KTestMessage);
		User::After(KTimeToLog);
		TRAPD(r, ret = DoTestCheckWriteL());
		if (r != KErrNone)
			ret = r;
		}

	return ret;		

	}




TInt CFloggerTest030_TagWrite05::DoTestCheckWriteL()
	{
	RFile theFile;
	HBufC8 * hBuffer;
	TInt listfilesize,returnCode;
	RFs fileSystem; //For file operation create a file system
	TUint numSuccessful = 0;

	
	User::LeaveIfError(fileSystem.Connect());
	CleanupClosePushL(fileSystem);
	
	//Open the file in the read mode
	User::LeaveIfError(theFile.Open(fileSystem,KFloggerOutputFile,EFileRead)); 

	CleanupClosePushL(theFile);

	User::LeaveIfError(theFile.Size(listfilesize)); //Size of the file
	
	hBuffer = HBufC8::New(listfilesize); //Allocate the buffer
	CleanupStack::PushL(hBuffer);

	TPtr8 ptrString = hBuffer->Des();  ; //To hold the buffer

	// Read from position 0: start of file
	User::LeaveIfError(returnCode = theFile.Read(ptrString));
	
	returnCode = ptrString.Find(KTestMessage8); //find the test descriptor in the buffer read
											//from the file

	if (returnCode == KErrNotFound)
		{
		numSuccessful++;
		returnCode = ptrString.Find(KErrOOM);
		if (returnCode > 0)
			User::Leave(KErrNoMemory);
		}

	
	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy();	//theFile
	CleanupStack::PopAndDestroy();  //fileSystem
	
	if (numSuccessful == 1)
		return KErrNone;
	else 
		return KErrNotFound;
	}


/**
* 030_TagWrite06: Test that flogger handles long tags
*/ 

CFloggerTest030_TagWrite06::CFloggerTest030_TagWrite06()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_030_TagWrite06"));
	}




CFloggerTest030_TagWrite06::~CFloggerTest030_TagWrite06()
	{
	}




TVerdict CFloggerTest030_TagWrite06::doTestStepL( )
	{
	if ( executeStepL() == KErrNone  )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	User::After(KTimeForDisplay);

	
	return TestStepResult();
	}


TInt CFloggerTest030_TagWrite06::executeStepL(TBool)
	{
	return KErrGeneral;
	}

TInt CFloggerTest030_TagWrite06::executeStepL()
	{
	
	TInt ret ;
	RFileLogger theFlogger; 
	ret = theFlogger.Connect();
	
	if ( ret == KErrNone)
		{
		//Create Log by passing the subsystem and component name as parameters
		theFlogger.SetLogTags(KStdSubsysTag8, KStdCompTag8);
		theFlogger.ClearLog();
		theFlogger.Close();
		RFileLogger::Write(KSubsysTag16char8, KCompTag16char8,KTestMessage8);
		User::After(KTimeToLog);
		TRAPD(r, ret = DoTestCheckWriteL());
		if (r != KErrNone)
			ret = r;
		}

	return ret;		

	}




TInt CFloggerTest030_TagWrite06::DoTestCheckWriteL()
	{
	RFile theFile;
	HBufC8 * hBuffer;
	TInt listfilesize,returnCode;
	RFs fileSystem; //For file operation create a file system
	TUint numSuccessful = 0;

	
	User::LeaveIfError(fileSystem.Connect());
	CleanupClosePushL(fileSystem);
	
	//Open the file in the read mode
	User::LeaveIfError(theFile.Open(fileSystem,KFloggerOutputFile,EFileRead)); 

	CleanupClosePushL(theFile);

	User::LeaveIfError(theFile.Size(listfilesize)); //Size of the file
	
	hBuffer = HBufC8::New(listfilesize); //Allocate the buffer
	CleanupStack::PushL(hBuffer);

	TPtr8 ptrString = hBuffer->Des();  ; //To hold the buffer

	// Read from position 0: start of file
	User::LeaveIfError(returnCode = theFile.Read(ptrString));
	
	returnCode = ptrString.Find(KTestMessage8); //find the test descriptor in the buffer read
											//from the file
	
	if (returnCode > 0)
		{
		numSuccessful++;
		}
	else if (returnCode == KErrNotFound)
		{
		returnCode = ptrString.Find(KErrOOM);
		if (returnCode > 0)
			User::Leave(KErrNoMemory);
		}

	
	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy();	//theFile
	CleanupStack::PopAndDestroy();  //fileSystem
	
	if (numSuccessful == 1)
		return KErrNone;
	else 
		return KErrNotFound;
	}




/**
* 030_TagWrite07: Test that flogger handles long tags
*/ 

CFloggerTest030_TagWrite07::CFloggerTest030_TagWrite07()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_030_TagWrite07"));
	}




CFloggerTest030_TagWrite07::~CFloggerTest030_TagWrite07()
	{
	}




TVerdict CFloggerTest030_TagWrite07::doTestStepL( )
	{
	if ( executeStepL() == KErrNone  )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	User::After(KTimeForDisplay);

	
	return TestStepResult();
	}


TInt CFloggerTest030_TagWrite07::executeStepL(TBool)
	{
	return KErrGeneral;
	}

TInt CFloggerTest030_TagWrite07::executeStepL()
	{
	
	TInt ret ;
	RFileLogger theFlogger; 
	ret = theFlogger.Connect();
	
	if ( ret == KErrNone)
		{
		//Create Log by passing the subsystem and component name as parameters
		theFlogger.SetLogTags(KStdSubsysTag8, KStdCompTag8);
		theFlogger.ClearLog();
		theFlogger.Close();
		RFileLogger::Write(KSubsysTag17char8, KCompTag17char8,KTestMessage8);
		User::After(KTimeToLog);
		TRAPD(r, ret = DoTestCheckWriteL());
		if (r != KErrNone)
			ret = r;
		}

	return ret;		

	}




TInt CFloggerTest030_TagWrite07::DoTestCheckWriteL()
	{
	RFile theFile;
	HBufC8 * hBuffer;
	TInt listfilesize,returnCode;
	RFs fileSystem; //For file operation create a file system
	TUint numSuccessful = 0;

	
	User::LeaveIfError(fileSystem.Connect());
	CleanupClosePushL(fileSystem);
	
	//Open the file in the read mode
	User::LeaveIfError(theFile.Open(fileSystem,KFloggerOutputFile,EFileRead)); 

	CleanupClosePushL(theFile);

	User::LeaveIfError(theFile.Size(listfilesize)); //Size of the file
	
	hBuffer = HBufC8::New(listfilesize); //Allocate the buffer
	CleanupStack::PushL(hBuffer);

	TPtr8 ptrString = hBuffer->Des();  ; //To hold the buffer

	// Read from position 0: start of file
	User::LeaveIfError(returnCode = theFile.Read(ptrString));
	
	returnCode = ptrString.Find(KTestMessage8); //find the test descriptor in the buffer read
											//from the file
	
	if (returnCode > 0)
		{
		numSuccessful++;
		}
	else if (returnCode == KErrNotFound)
		{
		returnCode = ptrString.Find(KErrOOM);
		if (returnCode > 0)
			User::Leave(KErrNoMemory);
		}

	// We expect to see the string "subsystem0123456" but not "subsystem01234567"
	returnCode = ptrString.Find(KSubsysTag16char8); //find the test descriptor in the buffer read

	if (returnCode > 0)
		{
		numSuccessful++;
		}

	returnCode = ptrString.Find(KSubsysTag17char8); //find the test descriptor in the buffer read
	
	if (returnCode == KErrNotFound)
		{
		numSuccessful++;
		}
	
	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy();	//theFile
	CleanupStack::PopAndDestroy();  //fileSystem
	
	if (numSuccessful == 3)
		return KErrNone;
	else 
		return KErrNotFound;
	}




/**
* 030_TagWrite08: Test that flogger handles very long tags
*/ 

CFloggerTest030_TagWrite08::CFloggerTest030_TagWrite08()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_030_TagWrite08"));
	}




CFloggerTest030_TagWrite08::~CFloggerTest030_TagWrite08()
	{
	}




TVerdict CFloggerTest030_TagWrite08::doTestStepL( )
	{
	if ( executeStepL() == KErrNone  )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	User::After(KTimeForDisplay);

	
	return TestStepResult();
	}


TInt CFloggerTest030_TagWrite08::executeStepL(TBool)
	{
	return KErrGeneral;
	}

TInt CFloggerTest030_TagWrite08::executeStepL()
	{
	
	TInt ret ;
	RFileLogger theFlogger; 
	ret = theFlogger.Connect();
	
	if ( ret == KErrNone)
		{
		theFlogger.SetLogTags(KStdSubsysTag8, KStdCompTag8);
		theFlogger.ClearLog();
		theFlogger.Close();
		RFileLogger::Write(KSubsysTag66char8, KCompTag66char8,KTestMessage);
		User::After(KTimeToLog);
		TRAPD(r, ret = DoTestCheckWriteL());
		if (r != KErrNone)
			ret = r;
		}

	return ret;		

	}




TInt CFloggerTest030_TagWrite08::DoTestCheckWriteL()
	{
	RFile theFile;
	HBufC8 * hBuffer;
	TInt listfilesize,returnCode;
	RFs fileSystem; //For file operation create a file system
	TUint numSuccessful = 0;

	
	User::LeaveIfError(fileSystem.Connect());
	CleanupClosePushL(fileSystem);
	
	//Open the file in the read mode
	User::LeaveIfError(theFile.Open(fileSystem,KFloggerOutputFile,EFileRead)); 

	CleanupClosePushL(theFile);

	User::LeaveIfError(theFile.Size(listfilesize)); //Size of the file
	
	hBuffer = HBufC8::New(listfilesize); //Allocate the buffer
	CleanupStack::PushL(hBuffer);

	TPtr8 ptrString = hBuffer->Des();  ; //To hold the buffer

	// Read from position 0: start of file
	User::LeaveIfError(returnCode = theFile.Read(ptrString));
	
	returnCode = ptrString.Find(KTestMessage8); //find the test descriptor in the buffer read
											//from the file
	
	if (returnCode > 0)
		{
		numSuccessful++;
		}
	else if (returnCode == KErrNotFound)
		{
		returnCode = ptrString.Find(KErrOOM);
		if (returnCode > 0)
			User::Leave(KErrNoMemory);
		}

	// We expect to see the string "subsystem0123456" but not "subsystem01234567"
	returnCode = ptrString.Find(KSubsysTag16char8); //find the test descriptor in the buffer read

	if (returnCode > 0)
		{
		numSuccessful++;
		}
	
	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy();	//theFile
	CleanupStack::PopAndDestroy();  //fileSystem
	
	if (numSuccessful == 2)
		return KErrNone;
	else 
		return KErrNotFound;
	}


