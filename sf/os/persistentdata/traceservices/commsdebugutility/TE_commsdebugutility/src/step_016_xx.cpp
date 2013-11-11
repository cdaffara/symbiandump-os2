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
// This contains Flogger Unit Test Case 016.xx

// EPOC includes
#include <e32base.h>

// Test system includes
#include "TestMessage.h"
#include <comms-infras/commsdebugutility.h>
#include <f32file.h>


//User includes
#include "teststepcomsdbg.h"
#include "step_016_xx.h"


/**
* Function  Name	: CFloggerTest016_01
* Input parameters	: None
* Output parameters : None
* Description		: This is the constructor
*/


CFloggerTest016_01::CFloggerTest016_01()
	{
	// store the name of this test case
	SetTestStepName(_L("step_016_01"));
	}


/**
* Function  Name	:~ CFloggerTest016_01
* Input parameters	: None
* Output parameters : None
* Description		: This is the Destructor
*/


CFloggerTest016_01::~CFloggerTest016_01()
	{
	}

/**
* Function  Name	: doTestStepL
* Input parameters	: None
* Output parameters : TVerdict 
* Description		: This function returns weather the test case 016_01 has 
* 					  passed or failed

*/


TVerdict CFloggerTest016_01::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 016.01 called "));
	
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


TInt CFloggerTest016_01::executeStepL(TBool heapTest)
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
* Function  Name		: executeStepL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function writes the test data in to the log file 
* 						  This function check the test message is present in the 
* 						  log file 

*/


TInt CFloggerTest016_01::executeStepL()
	{
	return KErrGeneral;
	}

/**
* Function  Name		: DoTestWrite
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function writes the data to the file logger
 
*/


TInt CFloggerTest016_01::DoTestWrite()
	{
	_LIT8(KTestMessageOne,"TC 16_01: The value of test integer variable :%d");
	DoTestWriteFormat(KTestMessageOne, 100);  //Write 8 bit formatted test descriptor

	// connect so we can flush file buffer for heap check since timer is killed in heap checks
	RFileLogger theFlogger;
	theFlogger.Connect();
	theFlogger.SetLogTags(KStdSubsysTag8, KStdCompTag8);
	ForceLogFlush(theFlogger);
	theFlogger.Close();
	return KErrNone; 
	}


/**
* Function  Name		: DoTestWriteFormat
* Input parameters		: TRefByValue<const TDesC8>
* Output parameters		: TInt 
* Description 			: This function writes formatted data to the file logger
*/


TInt CFloggerTest016_01::DoTestWriteFormat(TRefByValue<const TDesC8> aFmt,...)
	{
	VA_LIST list;
	VA_START(list,aFmt);
	RFileLogger::WriteFormat(KStdSubsysTag8, KStdCompTag8, aFmt, list);
	return KErrNone;
	}


/**
* Function  Name		: DoTestCheckWriteL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function checks the weather test data was written
* 						  in to the log file by DoTestWrite() or not.
					  
*/


TInt CFloggerTest016_01::DoTestCheckWriteL()
	{
	User::After(KTimeToLog);

	RFile theFile;
	HBufC8 * hBuffer;
	TInt listfilesize;
	TInt returnCode;
	RFs fileSystem; //For file operation create a file system	
	TBuf8<256> testData; //To hold the test descriptor

	_LIT8(KTestMessageOne,"TC 16_01: The value of test integer variable :%d");
	testData.Format(KTestMessageOne, 100);

	_LIT(KLogFile,"c:\\logs\\log.txt"); // the log file path

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
	fileSystem.Close(); //Close the file server
	CleanupStack::PopAndDestroy(hBuffer);
	if (returnCode > 0)
		return KErrNone;
	else 
		return KErrNotFound;
	}




/**
* Function  Name	: CFloggerTest016_02
* Input parameters	: None
* Output parameters : None
* Description		: This is the constructor
*/ 


CFloggerTest016_02::CFloggerTest016_02()
	{
	// store the name of this test case
	SetTestStepName(_L("step_016_02"));
	}


/**
* Function  Name	:~ CFloggerTest016_02
* Input parameters	: None
* Output parameters : None
* Description		: This is the Destructor
*/ 


CFloggerTest016_02::~CFloggerTest016_02()
	{
	iFlogger.Close();
	}


/**
* Function  Name	: doTestStepL
* Input parameters	: None
* Output parameters : TVerdict 
* Description		: This function returns weather the test case 016_01 has 
* 					  passed or failed

*/


TVerdict CFloggerTest016_02::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 016.02 called "));
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


TInt CFloggerTest016_02::executeStepL(TBool)
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


TInt CFloggerTest016_02::executeStepL()
	{
	User::After(1000*1000);	//Need this or connect will return KErrServerTerminated
	TInt j = 0;
	for(TInt i=0; i < 100000000; i++)
		j++;

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


TInt CFloggerTest016_02::DoTestWrite()
	{
	_LIT8(KTestLongMessage,"TEST 16.02: This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message %d"); //8 bit test decriptor
	DoTestWriteFormat(KTestLongMessage, 100);  //Write 8 bit formatted test descriptor
	return KErrNone; 
	}


/**
* Function  Name		: DoTestWriteFormat
* Input parameters		: TRefByValue<const TDesC16>
* Output parameters		: TInt 
* Description 			: This function writes formatted data to the file logger
*/


TInt CFloggerTest016_02::DoTestWriteFormat(TRefByValue<const TDesC8> aFmt,...)
	{

	RFileLogger theFlogger;
	TInt ret = KErrGeneral;
	ret = theFlogger.Connect(); //Just to clear the old log message
	if ( ret == KErrNone)
		ret = theFlogger.SetLogTags(KStdSubsysTag8, KStdCompTag8);
		if ( ret == KErrNone)
			ret = theFlogger.ClearLog();
			if ( ret == KErrNone)
				theFlogger.Close();
	
	User::After(100000);
	
	if (ret == KErrNone)
		{
		VA_LIST list;
		VA_START(list,aFmt);
		
		RFileLogger::WriteFormat(KStdSubsysTag8, KStdCompTag8, aFmt, list);
		User::After(100000);
		return KErrNone;
		}

	return ret;
	}


/**
* Function  Name		: DoTestCheckWriteL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function checks the weather test data was written
* 						  in to the log file by DoTestWrite() or not.
					  
*/


TInt CFloggerTest016_02::DoTestCheckWriteL()
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
	_LIT(KTestLongMessage, "TEST 16.02: This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message This is the l");
	
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
* Function  Name	: CFloggerTest016_03
* Input parameters	: None
* Output parameters : None
* Description		: This is the constructor
*/ 


CFloggerTest016_03::CFloggerTest016_03()
	{
	// store the name of this test case
	SetTestStepName(_L("step_016_03"));
	}


/**
* Function  Name	:~ CFloggerTest016_03
* Input parameters	: None
* Output parameters : None
* Description		: This is the Destructor
*/ 


CFloggerTest016_03::~CFloggerTest016_03()
	{
	}


/**
* Function  Name : doTestStepL
* Input parameters : None
* Output parameters : TVerdict 
* Description : This function is responsible for doing the heap test analysis 

*/


TVerdict CFloggerTest016_03::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 016.03 called "));

	CFloggerTest016_01* step016_01 = new CFloggerTest016_01;
	CleanupStack::PushL(step016_01);
	doTestStepWithHeapFailureL( *step016_01, 1, 20, KErrNone, ETrue);

	CleanupStack::PopAndDestroy(step016_01);
	User::After(KTimeForDisplay);
	return TestStepResult();
	}

