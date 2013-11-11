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
// This contains Flogger Unit Test Case 015.xx

// EPOC includes
#include <e32base.h>

// Test system includes
#include "teststepcomsdbg.h"
#include "TestMessage.h"
#include "step_015_xx.h"

#include <comms-infras/commsdebugutility.h>


/**
* Function  Name	: CFloggerTest015_01
* Input parameters	: None
* Output parameters : None
* Description		: This is the constructor
*/ 


CFloggerTest015_01::CFloggerTest015_01()
	{
	// store the name of this test case
	SetTestStepName(_L("step_015_01"));
	}


/**
* Function  Name	:~ CFloggerTest015_01
* Input parameters	: None
* Output parameters : None
* Description		: This is the Destructor
*/


CFloggerTest015_01::~CFloggerTest015_01()
	{
	}


/**
* Function  Name	: doTestStepL
* Input parameters	: None
* Output parameters : TVerdict 
* Description		: This function returns weather the test case 015_01 has 
* 					  passed or failed

*/


TVerdict CFloggerTest015_01::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 015.01 called "));
	
	if ( executeStepL(EFalse) == KErrNone )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	
	User::After(KTimeForDisplay1);
	return TestStepResult();
	}

/**
* Function  Name		: executeStepL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function writes the test data in to the log file 
* 						  This function check the test message is present in the 
* 						  log file 

*/


TInt CFloggerTest015_01::executeStepL()
	{
	return KErrGeneral;
	}

/**
* Function  Name		: executeStepL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function writes the test data in to the log file 
* 						  This function check the test message is present in the 
* 						  log file 

*/


TInt CFloggerTest015_01::executeStepL(TBool heapTest)
	{
	TInt ret = KErrGeneral;
		
	ret = DoTestWrite();
	if (ret == KErrNone )
		{
		User::After(KTimeToLog);
		TRAPD(r, ret = DoTestCheckWriteL() );
		if ((heapTest) && ((ret == KErrNotFound) || (r != KErrNone)))
			{
			ret = KErrNoMemory;
			}
		else if (r != KErrNone)
			{
			ret = r;
			}
		}
	return ret;
	}


/**
* Function  Name		: DoTestWrite
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function writes the data to the file logger
 
*/


TInt CFloggerTest015_01::DoTestWrite()
	{
	_LIT8(KTestMessageOne,"TEST 15.01: The value of test integer variable :%d");

	RFileLogger::WriteFormat(KStdSubsysTag8, KStdCompTag8, KTestMessageOne, 100);

	// connect so we can flush file buffer for heap check since timer is killed in heap checks
	RFileLogger theFlogger;
	theFlogger.Connect();
	theFlogger.SetLogTags(KStdSubsysTag8, KStdCompTag8);
	ForceLogFlush(theFlogger);
	theFlogger.Close();

	return KErrNone; 
	}
	

/**
* Function  Name		: DoTestCheckWriteL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function checks the weather test data was written
* 						  in to the log file by DoTestWrite() or not.
					  
*/


TInt CFloggerTest015_01::DoTestCheckWriteL()
	{

	RFile theFile;
	HBufC8 * hBuffer;
	TInt listfilesize;
	TInt returnCode;
	RFs fileSystem;				//For file operation create a file system
	TBuf8<256> testData;			//To hold the test descriptor
	_LIT(KLogFile, "c:\\logs\\log.txt"); // log file name and path

	_LIT8(KTestMessageOne,"TEST 15.01: The value of test integer variable :%d");
	testData.Format(KTestMessageOne,100);

	User::LeaveIfError(fileSystem.Connect());
	
	//Open the file in the read mode
	User::LeaveIfError(theFile.Open(fileSystem,KLogFile,EFileRead)); 

	
	User::LeaveIfError(returnCode = theFile.Size(listfilesize)); //Size of the file
	hBuffer = HBufC8::New(listfilesize); //Allocate the buffer
	CleanupStack::PushL(hBuffer);
	TPtr8 ptrString = hBuffer->Des();  ; //To hold the buffer read from file

	// Read from position 0: starting of file
	returnCode = theFile.Read(ptrString);
	
	returnCode = ptrString.Find(testData); //find the test descriptor in the buffer read
									//from the file

	theFile.Close();
	fileSystem.Close();
	CleanupStack::PopAndDestroy(hBuffer);
	if (returnCode > 0)
		return KErrNone;
	else 
		return KErrNotFound;
	}






/**
* Function  Name	: CFloggerTest015_02
* Input parameters	: None
* Output parameters : None
* Description		: This is the constructor
*/


CFloggerTest015_02::CFloggerTest015_02()
	{
	// store the name of this test case
	SetTestStepName(_L("step_015_02"));
	}


/**
* Function  Name	:~ CFloggerTest015_02
* Input parameters	: None
* Output parameters : None
* Description		: This is the Destructor
*/ 


CFloggerTest015_02::~CFloggerTest015_02()
	{
	iFlogger.Close();
	}


/**
* Function  Name	: doTestStepL
* Input parameters	: None
* Output parameters : TVerdict 
* Description		: This function returns weather the test case 015_02 has 
* 					  passed or failed

*/


TVerdict CFloggerTest015_02::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 011.02 called "));
	
	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	
	User::After(KTimeForDisplay1);
	return TestStepResult();
	}

/**
* Function  Name		: executeStepL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function writes the test data in to the log file 
* 						  This function check the test message is present in the 
* 						  log file 

*/


TInt CFloggerTest015_02::executeStepL(TBool)
	{
	return KErrGeneral;
	}

/**
* Function  Name		: executeStepL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function writes the test data in to the log file 
* 						  This function check the test message is present in the 
* 						  log file 

*/


TInt CFloggerTest015_02::executeStepL()
	{
	User::After(KTimeToLog);	//Need this or connect will return KErrServerTerminated
	
	TInt ret = KErrGeneral;
		
	ret = DoTestWrite();
	if (ret == KErrNone )
		{
		TRAPD(r, ret = DoTestCheckWriteL() );
		if ( r != KErrNone)
			ret = r;
		}
	return ret;
	}


/**
* Function  Name		: DoTestWrite
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function writes the data to the file logger
 
*/


TInt CFloggerTest015_02::DoTestWrite()
	{
	
	_LIT8(KTestMessageTwo,"Test case 15.02: The value of first test integer variable :%d The value of second test integer variable : %d");
	
	RFileLogger ::WriteFormat(KStdSubsysTag8, KStdCompTag8, KTestMessageTwo, 100, 200);
	return KErrNone;
	}


/**
* Function  Name		: DoTestCheckWriteL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function checks the weather test data was written
* 						  in to the log file by DoTestWrite() or not.
					  
*/


TInt CFloggerTest015_02::DoTestCheckWriteL()
	{
	User::After(KTimeToLog);

	RFile theFile;
	HBufC8 * hBuffer;
	TInt listfilesize;
	TInt returnCode;
	RFs fileSystem; //For file operation create a file system
	TBuf8<256> testData; //To hold the test descriptor
	_LIT(KLogFile, "c:\\logs\\log.txt"); // log file name and path

	_LIT8(KTestMessageTwo,"Test case 15.02: The value of first test integer variable :%d The value of second test integer variable : %d");
	testData.Format(KTestMessageTwo,100, 200);

	User::LeaveIfError(fileSystem.Connect());
	
	//Open the file in the read mode
	User::LeaveIfError(theFile.Open(fileSystem,KLogFile,EFileRead)); 

	
	User::LeaveIfError(returnCode = theFile.Size(listfilesize)); //Size of the file
	hBuffer = HBufC8::New(listfilesize); //Allocate the buffer
	CleanupStack::PushL(hBuffer);
	TPtr8 ptrString = hBuffer->Des();  ; //To hold the buffer

	// Read from position 0: start of file
	returnCode = theFile.Read(ptrString);
	
	returnCode = ptrString.Find(testData); //find the test descriptor in the buffer read
									//from the file

	theFile.Close();
	fileSystem.Close();
	CleanupStack::PopAndDestroy(hBuffer);
	if (returnCode > 0)
		return KErrNone;
	else 
		return KErrGeneral;
	}


/**
* Function  Name	: CFloggerTest015_03
* Input parameters	: None
* Output parameters : None
* Description		: This is the constructor
*/


CFloggerTest015_03::CFloggerTest015_03()
	{
	// store the name of this test case
	SetTestStepName(_L("step_015_03"));
	}


/**
* Function  Name	:~ CFloggerTest015_03
* Input parameters	: None
* Output parameters : None
* Description		: This is the Destructor
*/


CFloggerTest015_03::~CFloggerTest015_03()
	{	
	}


/**
* Function  Name	: doTestStepL
* Input parameters	: None
* Output parameters : TVerdict 
* Description		: This function returns weather the test case 015_03 has 
* 					  passed or failed

*/


TVerdict CFloggerTest015_03::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 015.03 called "));
	
	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	
	User::After(KTimeForDisplay1);
	return TestStepResult();
	}

/**
* Function  Name		: executeStepL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function writes the test data in to the log file 
* 						  This function check the test message is present in the 
* 						  log file 

*/


TInt CFloggerTest015_03::executeStepL(TBool)
	{
	return KErrGeneral;
	}


/**
* Function  Name		: executeStepL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function writes the test data in to the log file 
* 						  This function check the test message is present in the 
* 						  log file 

*/


TInt CFloggerTest015_03::executeStepL()
	{
	User::After(1000*1000);
	 TInt ret = KErrGeneral;
		
	ret = DoTestWrite();
	if (ret == KErrNone )
		{
		TRAPD(r, ret = DoTestCheckWriteL());
		if ( r != KErrNone)
			ret = r;
		}
	return ret;
	}

/**
* Function  Name		: DoTestWrite
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function writes the data to the file logger
 
*/


TInt CFloggerTest015_03::DoTestWrite()
	{
	
	TInt ret = KErrNone;
	
	//8 bit test decriptor which is very long
	_LIT8(KTestLongMessage,"TEST 15.03: This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message"); //8 bit test decriptor
	
	RFileLogger theFlogger;

	ret = theFlogger.Connect(); //Just to clear the old log message
	if ( ret == KErrNone)
		ret = theFlogger.SetLogTags(KStdSubsysTag8, KStdCompTag8);
		if ( ret == KErrNone)
			ret = theFlogger.ClearLog();
			if ( ret == KErrNone)
				theFlogger.Close();

	if (ret == KErrNone)
	
	RFileLogger ::WriteFormat(KStdSubsysTag8, KStdCompTag8, KTestLongMessage);
		
	return KErrNone;
	}


/*
* Function  Name		: DoTestCheckWriteL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function checks the weather test data was written
* 						  in to the log file by DoTestWrite() or not.
					  
*/


TInt CFloggerTest015_03::DoTestCheckWriteL()
	{
	User::After(KTimeToLog);

	RFile theFile;
	HBufC8 * hBuffer;
	TInt listfilesize,returnCode;
	RFs fileSystem; //For file operation create a file system
	TBuf8<256> testData; //To hold the test descriptor
	_LIT(KLogFile,"c:\\logs\\log.txt"); // log file name and path

	//The following contains the string of length 200(max) 
	//which is  written to the log file
	
	_LIT(KTestLongMessage, "TEST 15.03: This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message This is the l");
	
	//_LIT8(KOOMError, "#Logs may be lost out of memory!!");
	
	User::LeaveIfError(fileSystem.Connect());
	
	//Open the file in the read mode
	User::LeaveIfError(theFile.Open(fileSystem,KLogFile,EFileRead)); 

	CleanupClosePushL(theFile);

	User::LeaveIfError(theFile.Size(listfilesize)); //Size of the file
	
	hBuffer = HBufC8::New(listfilesize); //Allocate the buffer
	CleanupStack::PushL(hBuffer);

	TPtr8 ptrString = hBuffer->Des();  ; //To hold the buffer

	// Read from position 0: start of file
	User::LeaveIfError(returnCode = theFile.Read(ptrString));
	
	testData.Copy(KTestLongMessage); //Copy the test descriptor
	returnCode = ptrString.Find(testData); //find the test descriptor in the buffer read
											//from the file
	
	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy();	//theFile
	if (returnCode > 0)
		return KErrNone;
	else 
		return KErrGeneral;
	}


/**
* Function  Name	: CFloggerTest015_04
* Input parameters	: None
* Output parameters : None
* Description		: This is the constructor
*/ 


CFloggerTest015_04::CFloggerTest015_04()
	{
	// store the name of this test case
	SetTestStepName(_L("step_015_04"));
	}


/**
* Function  Name	:~ CFloggerTest015_04
* Input parameters	: None
* Output parameters : None
* Description		: This is the Destructor
*/


CFloggerTest015_04::~CFloggerTest015_04()
	{
	}


/**
* Function  Name : doTestStepL
* Input parameters : None
* Output parameters : TVerdict 
* Description : This function is responsible for doing the heap test analysis 

*/


TVerdict CFloggerTest015_04::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 015.04 called "));

	CFloggerTest015_01* step015_01 = new CFloggerTest015_01;
	CleanupStack::PushL(step015_01);
	doTestStepWithHeapFailureL( *step015_01, 1, 20, KErrNone, ETrue);

	CleanupStack::PopAndDestroy(step015_01);
	User::After(KTimeForDisplay);
	return TestStepResult();
	}


