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
// This contains Flogger Unit Test Case 012.xx

// EPOC includes
#include <e32base.h>

// Test system includes
#include "teststepcomsdbg.h"
#include "TestMessage.h"
#include "step_012_xx.h"

#include <comms-infras/commsdebugutility.h>


/**
* Function  Name	: CFloggerTest012_01
* Input parameters	: None
* Output parameters : None
* Description		: This is the constructor
*/


CFloggerTest012_01::CFloggerTest012_01()
	{
	// store the name of this test case
	SetTestStepName(_L("step_012_01"));
	}


/**
* Function  Name	:~ CFloggerTest012_01
* Input parameters	: None
* Output parameters : None
* Description		: This is the Destructor
*/


CFloggerTest012_01::~CFloggerTest012_01()
	{
	}


/**
* Function  Name	: doTestStepL
* Input parameters	: None
* Output parameters : TVerdict 
* Description		: This function returns weather the test case 012_01 has 
* 					  passed or failed

*/


TVerdict CFloggerTest012_01::doTestStepL( )
	{

	INFO_PRINTF1(_L("Step 012.01 called "));
	
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


TInt CFloggerTest012_01::executeStepL(TBool heapTest)
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


TInt CFloggerTest012_01::executeStepL()
	{
	return KErrGeneral;
	}


/**
* Function  Name		: DoTestWrite
* Function  Name		: DoTestWrite
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function writes the data to the file logger
 
*/


TInt CFloggerTest012_01::DoTestWrite()
	{
	_LIT16(KTestMessageOne,"TEST 12.01: The value of test integer variable :%d"); //unicode test decriptor
	RFileLogger::WriteFormat(KStdSubsysTag8, KStdCompTag8, KTestMessageOne, 100);

	// connect so we can flush file buffer for heap check
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
* 						  in to the log file by DoTestWriteL() or not.
					  
*/


TInt CFloggerTest012_01::DoTestCheckWriteL()
	{
	RFile theFile;
	HBufC8 * hBuffer;
	TInt listfilesize;
	TInt returnCode;
	RFs fileSystem;				//For file operation create a file system
	TBuf8<256> testData;			//To hold the test descriptor
	_LIT(KLogFile, "c:\\logs\\log.txt"); // log file name and path

	_LIT8(KTestMessageOne,"TEST 12.01: The value of test integer variable :%d"); //unicode test decriptor
	testData.Format(KTestMessageOne,100);

	User::LeaveIfError(fileSystem.Connect());
	
	//Open the file in the read mode
	User::LeaveIfError(theFile.Open(fileSystem,KLogFile,EFileRead)); 

	
	returnCode = theFile.Size(listfilesize); //Size of the file
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


TInt CFloggerTest012_01::DoHeapTestWrite()
	{

	_LIT16(KTestMessageOne,"TEST 12.04: Heap Test: The value of test integer variable :%d"); //unicode test decriptor
	RFileLogger::WriteFormat(KStdSubsysTag8, KStdCompTag8, KTestMessageOne, 100);
	return KErrNone;
	}


/**
* Function  Name	: CFloggerTest012_02
* Input parameters	: None
* Output parameters : None
* Description		: This is the constructor
*/


CFloggerTest012_02::CFloggerTest012_02()
	{
	// store the name of this test case
	SetTestStepName(_L("step_012_02"));
	}


/**
* Function  Name	:~ CFloggerTest012_02
* Input parameters	: None
* Output parameters : None
* Description		: This is the Destructor
*/


CFloggerTest012_02::~CFloggerTest012_02()
	{
	iFlogger.Close();
	}



/**
* Function  Name	: doTestStepL
* Input parameters	: None
* Output parameters : TVerdict 
* Description		: This function returns weather the test case 012_02 has 
* 					  passed or failed

*/


TVerdict CFloggerTest012_02::doTestStepL( )
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


TInt CFloggerTest012_02::executeStepL(TBool)
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


TInt CFloggerTest012_02::executeStepL()
	{
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


TInt CFloggerTest012_02::DoTestWrite()
	{
	
	//unicode test decriptor
	_LIT16(KTestMessageTwo,"TEST 12.02: The value of first test integer variable :%d The value of second test integer variable : %d");

	RFileLogger ::WriteFormat(KStdSubsysTag8, KStdCompTag8, KTestMessageTwo, 100, 200);
	
	
	return KErrNone;
	}


/**
* Function  Name		: DoTestCheckWriteL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function checks the weather test data was written
* 						  in to the log file by DoTestWriteL() or not.
					  
*/


TInt CFloggerTest012_02::DoTestCheckWriteL()
	{

	
	User::After(KTimeToLog);

	RFile theFile;
	HBufC8 * hBuffer;
	TInt listfilesize;
	TInt returnCode;
	RFs fileSystem; //For file operation create a file system
	TBuf8<256> testData; //To hold the test descriptor
	_LIT(KLogFile, "c:\\logs\\log.txt"); // log file name and path

	_LIT8(KTestMessageTwo,"TEST 12.02: The value of first test integer variable :%d The value of second test integer variable : %d");
	testData.Format(KTestMessageTwo,100, 200);

	User::LeaveIfError(fileSystem.Connect());
	
	//Open the file in the read mode
	User::LeaveIfError(theFile.Open(fileSystem,KLogFile,EFileRead)); 

	
	returnCode = theFile.Size(listfilesize); //Size of the file
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
* Function  Name	: CFloggerTest012_03
* Input parameters	: None
* Output parameters : None
* Description		: This is the constructor
*/


CFloggerTest012_03::CFloggerTest012_03()
	{
	// store the name of this test case
	SetTestStepName(_L("step_012_03"));
	}

/**
* Function  Name	:~ CFloggerTest012_03
* Input parameters	: None
* Output parameters : None
* Description		: This is the Destructor
*/


CFloggerTest012_03::~CFloggerTest012_03()
	{
	}


/**
* Function  Name	: doTestStepL
* Input parameters	: None
* Output parameters : TVerdict 
* Description		: This function returns weather the test case 012_03 has 
* 					  passed or failed

*/


TVerdict CFloggerTest012_03::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 012.03 called "));
	
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
*						  This function check the test message is present in the 
*						  log file 

*/


TInt CFloggerTest012_03::executeStepL(TBool)
	{
	return KErrGeneral;
	}

/**
* Function  Name		: executeStepL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function writes the test data in to the log file 
*						  This function check the test message is present in the 
*						  log file 

*/


TInt CFloggerTest012_03::executeStepL()
	{
	TInt ret = KErrGeneral;
	User::After(1000*100);	//Need this or connect will return KErrServerTerminated
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


TInt CFloggerTest012_03::DoTestWrite()
	{
	_LIT(KTestLongMessage,"TEST 12.03: This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message ");

	RFileLogger ::WriteFormat(KStdSubsysTag8, KStdCompTag8, KTestLongMessage);
	return KErrNone;
	}



/**
* Function  Name		: DoTestCheckWriteL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function checks the weather test data was written
* 						  in to the log file by DoTestWriteL() or not.
					  
*/


TInt CFloggerTest012_03::DoTestCheckWriteL()
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
	_LIT(KTestLongMessage, "TEST 12.03: This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message This");
	
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
* Function  Name	: CFloggerTest005_02
* Input parameters	: None
* Output parameters : None
* Description		: This is the constructor
*/


CFloggerTest012_04::CFloggerTest012_04()
	{
	// store the name of this test case
	SetTestStepName(_L("step_012_04"));
	}


/**
* Function  Name	:~ CFloggerTest005_02
* Input parameters	: None
* Output parameters : None
* Description		: This is the Destructor
*/


CFloggerTest012_04::~CFloggerTest012_04()
	{
	}

/**
* Function  Name : doTestStepL
* Input parameters : None
* Output parameters : TVerdict 
* Description : This function is responsible for doing the heap test analysis 

*/
TVerdict CFloggerTest012_04::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 012.04 called "));

	CFloggerTest012_01* step012_01 = new CFloggerTest012_01;
	CleanupStack::PushL(step012_01);
	doTestStepWithHeapFailureL( *step012_01, 1,20, KErrNone, ETrue);

	CleanupStack::PopAndDestroy(step012_01);
	User::After(KTimeForDisplay);
	return TestStepResult();
	}




