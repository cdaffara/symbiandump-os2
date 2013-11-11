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
// This contains Flogger Unit Test Case 014.xx

// EPOC includes
#include <e32base.h>

// Test system includes
#include "teststepcomsdbg.h"
#include "TestMessage.h"
#include "step_014_xx.h"

#include <comms-infras/commsdebugutility.h>


/**
* Function  Name	: CFloggerTest005_02
* Input parameters	: None
* Output parameters : None
* Description		: This is the constructor
*/


CFloggerTest014_01::CFloggerTest014_01()
	{
	// store the name of this test case
	SetTestStepName(_L("step_014_01"));
	}


/**
* Function  Name	:~ CFloggerTest014_01
* Input parameters	: None
* Output parameters : None
* Description		: This is the Destructor
*/


CFloggerTest014_01::~CFloggerTest014_01()
	{
	}


/**
* Function  Name	: doTestStepL
* Input parameters	: None
* Output parameters : TVerdict 
* Description		: This function returns weather the test case 014_01 has 
* 					  passed or failed

*/


TVerdict CFloggerTest014_01::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 011.01 called "));
	
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


TInt CFloggerTest014_01::executeStepL()
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


TInt CFloggerTest014_01::executeStepL(TBool heapTest)
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


TInt CFloggerTest014_01::DoTestWrite()
	{
	_LIT8(KTestMessage,"TC 14.01 :This is test msg"); //8 bit test decriptor
	
	//test step
	RFileLogger ::Write(KStdSubsysTag8, KStdCompTag8, KTestMessage);

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


TInt CFloggerTest014_01::DoTestCheckWriteL()
	{
	User::After(KTimeToLog);

	RFile theFile;
	HBufC8 * hBuffer;
	TInt listfilesize;
	TInt returnCode;
	RFs fileSystem; //For file operation create a file system
	TBuf8<256> testData; //To hold the test descriptor
	_LIT(KLogFile, "c:\\logs\\log.txt"); // log file name and path
	

	_LIT8(KTestMessage,"TC 14.01 :This is test msg"); //8 bit test decriptor

	User::LeaveIfError(fileSystem.Connect());
	
	//Open the file in the read mode
	User::LeaveIfError(theFile.Open(fileSystem,KLogFile,EFileRead)); 

	
	returnCode = theFile.Size(listfilesize); //Size of the file
	hBuffer = HBufC8::New(listfilesize); //Allocate the buffer
	CleanupStack::PushL(hBuffer);
	TPtr8 ptrString = hBuffer->Des();  ; //To hold the buffer

	// Read from position 0: start of file
	returnCode = theFile.Read(ptrString);
	
	testData.Copy(KTestMessage); //Copy the test descriptor
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
* Function  Name	: CFloggerTest014_02
* Input parameters	: None
* Output parameters : None
* Description		: This is the constructor
*/


CFloggerTest014_02::CFloggerTest014_02()
	{
	// store the name of this test case
	SetTestStepName(_L("step_014_02"));
	}


/**
* Function  Name	:~ CFloggerTest014_02
* Input parameters	: None
* Output parameters : None
* Description		: This is the Destructor
*/


CFloggerTest014_02::~CFloggerTest014_02()
	{
	}


/**
* Function  Name	: doTestStepL
* Input parameters	: None
* Output parameters : TVerdict 
* Description		: This function returns weather the test case 014_02 has 
* 					  passed or failed

*/


TVerdict CFloggerTest014_02::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 014.02 called "));
	
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


TInt CFloggerTest014_02::executeStepL()
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
* Function  Name		: executeStepL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function writes the test data in to the log file 
* 						  This function check the test message is present in the 
* 						  log file 

*/


TInt CFloggerTest014_02::executeStepL(TBool)
	{
	return KErrGeneral;
	}	

/**
* Function  Name		: DoTestWrite
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function writes the data to the file logger
 
*/


TInt CFloggerTest014_02::DoTestWrite()
	{
	_LIT8(KTestLongMessage,"TEST 14.2: This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message"); //8 bit test decriptor

	TInt ret;

	RFileLogger theFlogger;

	theFlogger.Connect(); //Just to clear the old log message
	ret = theFlogger.SetLogTags(KStdSubsysTag8, KStdCompTag8);
	theFlogger.ClearLog();
	theFlogger.Close();
	User::After(100000);
	
	RFileLogger ::Write(KStdSubsysTag8, KStdCompTag8, KTestLongMessage);
	return ret;
	}


/**
* Function  Name		: DoTestCheckWriteL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function checks the weather test data was written
* 						  in to the log file by DoTestWrite() or not.
					  
*/


TInt CFloggerTest014_02::DoTestCheckWriteL()
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
	//_LIT(KTestLongMessage, "This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message");
	_LIT(KTestLongMessage, "TEST 14.2: This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message This is the lo");
	
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
* Function  Name	: CFloggerTest014_03
* Input parameters	: None
* Output parameters : None
* Description		: This is the constructor
*/ 


CFloggerTest014_03::CFloggerTest014_03()
	{
	// store the name of this test case
	SetTestStepName(_L("step_014_03"));
	}


/**
* Function  Name	:~ CFloggerTest014_03
* Input parameters	: None
* Output parameters : None
* Description		: This is the Destructor
*/


CFloggerTest014_03::~CFloggerTest014_03()
	{
	}


/**
* Function  Name : doTestStepL
* Input parameters : None
* Output parameters : TVerdict 
* Description : This function is responsible for doing the heap test analysis 

*/


TVerdict CFloggerTest014_03::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 014.03 called "));

	CFloggerTest014_01* step014_01 = new CFloggerTest014_01;
	CleanupStack::PushL(step014_01);
	doTestStepWithHeapFailureL( *step014_01, 1, 20, KErrNone, ETrue);

	CleanupStack::PopAndDestroy(step014_01);
	User::After(KTimeForDisplay);
	return TestStepResult();


	}

