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
// This contains Flogger Unit Test Case 003.xx

// EPOC includes
#include <e32base.h>


// Test system includes
#include "teststepcomsdbg.h"
#include "step_003_xx.h"

#include <comms-infras/commsdebugutility.h>


/**
* Function  Name	: CFloggerTest003_01
* Input parameters	: None
* Output parameters : None
* Description		: This is the constructor
*/


CFloggerTest003_01::CFloggerTest003_01()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_003_01"));
	}


/**
* Function  Name	:~ CFloggerTest003_01
* Input parameters	: None
* Output parameters : None
* Description		: This is the Destructor
*/


CFloggerTest003_01::~CFloggerTest003_01()
	{
	}


/**
* Function  Name	: doTestStepL
* Input parameters	: None
* Output parameters : TVerdict 
* Description		: This function returns weather the test case 003_01 has 
*					  passed or failed

*/


TVerdict CFloggerTest003_01::doTestStepL( )
	{
	User::After(KTimeForDisplay1);
	INFO_PRINTF1(_L("Step 003.01 called "));

	if ( executeStepL(EFalse) == KErrNone  )
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
*						  checks weather they are set in the log file correctly

*/					  


TInt CFloggerTest003_01::executeStepL(TBool heapTest)
	{
	return DoTestWrite(heapTest);
	}

/**
* Function  Name		: executeStepL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function sets the subsystem and component name
*						  checks weather they are set in the log file correctly

*/					  


TInt CFloggerTest003_01::executeStepL()
	{
	return KErrGeneral;
	}

/**
* Function  Name		: DoTestWriteL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function checks the weather test data was written
						  in to the log file by DoTestWriteL() or not.
					  
*/


TInt CFloggerTest003_01::DoTestWrite(TBool heapTest)
	{
	_LIT8(KTestMessage ,"TC 3.01 :This is test msg");
	TPtrC8 ptrSubSystem;
	TPtrC8 ptrComponent;
	ptrSubSystem.Set(_L8("SubSystem"));
	ptrComponent.Set(_L8("Component"));

	TInt ret = KErrNone;
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
				theFlogger.Write(KTestMessage);
				ForceLogFlush(theFlogger);
				User::After(KTimeToLogLargeData);

				TRAPD(r, ret = DoTestCheckWriteL());
				if ((heapTest) && ((ret == KErrNotFound) || (r != KErrNone)))
					{
					ret = KErrNoMemory;
					}
				else if (r != KErrNone)
					{
					ret = r;
					}
				}
			}
		}
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


TInt CFloggerTest003_01::DoTestCheckWriteL()
	{
	TPtrC8 ptrSubSystem;
	TPtrC8 ptrComponent;
	ptrSubSystem.Set(_L8("SubSystem"));
	ptrComponent.Set(_L8("Component"));
	RFile theFile;

	HBufC8 * hBuffer;
	TInt listfilesize,returnCode;
	RFs iFileSystem; //For file operation create a file system
	TBuf8<256> testData; //To hold the test descriptor
	_LIT(KLogFile,"c:\\logs\\log.txt"); 

	_LIT(KTestSystem, "SubSystem"); //Test sytem name
	_LIT(KTestComponent, "Component"); //Test component name
	_LIT8(KTestMessage ,"TC 3.01 :This is test msg"); // Test descriptor
	_LIT8(KOOMError, "#Logs may be lost out of memory!!"); // Error message which has to be tested in log

	User::LeaveIfError(iFileSystem.Connect());
	
	//Open the file in the read mode
	User::LeaveIfError(theFile.Open(iFileSystem, KLogFile, EFileWrite|EFileShareAny)); 
	CleanupClosePushL(theFile);
	
	User::LeaveIfError(theFile.Size(listfilesize)); //Size of the file
	
	hBuffer = HBufC8::New(listfilesize); //Allocate the buffer
	CleanupStack::PushL(hBuffer);
	TPtr8 ptrString = hBuffer->Des();  ; //To hold the buffer

	// Read from position 0: start of file
	returnCode = theFile.Read(ptrString);
	
	testData.Copy(KTestMessage); //Copy the test message name
	returnCode = ptrString.Find(KTestMessage);	//find the test system name in the 
											//buffer read from the file
	if (returnCode == KErrNotFound)
		{
		User::Leave(KErrNotFound);
		}

	returnCode = ptrString.Find(KOOMError);
	if (returnCode > 0)
		{
		User::Leave(KErrNoMemory);
		}

	testData.Copy(KTestSystem);				//Copy the test subsystem name
	returnCode = ptrString.Find(testData);	//Find the test subsystem name in the 
												//buffer read from the file

	if (returnCode > 0) // Test subsystem name present
		{
		testData.Copy(KTestComponent);			//Copy the test component name
		returnCode = ptrString.Find(testData);	//Find the test component name in the 
												//buffer read from the file
		}
	
	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy();	//theFile

	if (returnCode > 0) // Test component name present
		return KErrNone;
	else 
		return KErrNotFound;
	}





/**
* Function  Name	: CFloggerTest003_02
* Input parameters	: None
* Output parameters : None
* Description		: This is the constructor
*/


CFloggerTest003_02::CFloggerTest003_02()
	{	
	// store the name of this test case
	SetTestStepName(_L("step_003_02"));
	}


/**
* Function  Name	:~ CFloggerTest003_02
* Input parameters	: None
* Output parameters : None
* Description		: This is the Destructor
*/


CFloggerTest003_02::~CFloggerTest003_02()
	{
	}


/**
* Function  Name	: doTestStepL
* Input parameters	: None
* Output parameters : TVerdict 
* Description		: This function returns weather the test case 003_02 has 
*					  passed or failed

*/


TVerdict CFloggerTest003_02::doTestStepL( )
	{	
	// INFO_PRINTF1(_L("Step 003.02 called ")); // This line was panicing in release mode testing
		
	if ( executeStepL() == KErrNone )
		SetTestStepResult(EFail);
	else
		SetTestStepResult(EPass);

	
	return TestStepResult();
	}


/**
* Function  Name		: executeStepL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function writes the test data in to the log file 
*						  This function check the test message is present in the 
* 						  log file 

*/


TInt CFloggerTest003_02::executeStepL()
	{
	return DoTestWrite() ;
	}

/**
* Function  Name		: executeStepL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function writes the test data in to the log file 
*						  This function check the test message is present in the 
* 						  log file 

*/


TInt CFloggerTest003_02::executeStepL(TBool)
	{
	return KErrGeneral;
	}
/**
* Function  Name		: DoTestWriteL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function writees the data to the file logger
 
*/


TInt CFloggerTest003_02::DoTestWrite()
	{
	_LIT(KTestMessage,"This is test message ");

	//Call Write() before SetLogTags(), this will cause a PANIC

	iFlogger.Connect();
	iFlogger.Write(KTestMessage);
	iFlogger.Close();
			
	//This test will cause a PANIC, so we will never get here
	return KErrNone;
	}


/**
* Function  Name	: CFloggerTest003_03
* Input parameters	: None
* Output parameters : None
* Description		: This is the constructor
*/


CFloggerTest003_03::CFloggerTest003_03()
	{
	// store the name of this test case
	SetTestStepName(_L("step_003_03"));
	}


/**
* Function  Name :~ CFloggerTest003_03
* Input parameters : None
* Output parameters : None
* Description : This is the Destructor
*/


CFloggerTest003_03::~CFloggerTest003_03()
	{
	}


/**
* Function  Name	: doTestStepL
* Input parameters	: None
* Output parameters : TVerdict 
* Description		: This function is responsible for doing the heap test analysis 

*/

TVerdict CFloggerTest003_03::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 003.03 called "));

	CFloggerTest003_01* step003_01 = new CFloggerTest003_01;
	CleanupStack::PushL(step003_01);
	if (doTestStepWithHeapFailureL( *step003_01, 450, 465, KErrNone, ETrue) != KErrNone)
		{
		SetTestStepResult(EFail);
		}
	
	CleanupStack::PopAndDestroy(step003_01);
	User::After(KTimeForDisplay);
	return TestStepResult();
	}

