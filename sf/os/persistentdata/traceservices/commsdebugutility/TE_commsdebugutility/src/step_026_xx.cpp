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
// This contains Flogger Unit Test Case 026.xx

// EPOC includes
#include <e32base.h>


// Test system includes
#include "teststepcomsdbg.h"
#include "step_026_xx.h"

#include <comms-infras/commsdebugutility.h>


/**
* Function  Name : CFloggerTest026_01
* Input parameters : None
* Output parameters : None
* Description : This is the constructor
*/


CFloggerTest026_01::CFloggerTest026_01()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_026_01"));
	}


/**
* Function  Name :~ CFloggerTest026_01
* Input parameters : None
* Output parameters : None
* Description : This is the Destructor
*/ 


CFloggerTest026_01::~CFloggerTest026_01()
	{
	}


/**
* Function  Name	: doTestStepL
* Input parameters	: None
* Output parameters : TVerdict 
* Description		: This function returns weather the test case 026_01 has 
* 					  passed or failed

*/


TVerdict CFloggerTest026_01::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 026.01 called "));
	User::After(KTimeForDisplay);


	if ( executeStepL(EFalse) == KErrNone  )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	INFO_PRINTF1(_L("leaving Step 026.01 "));
	User::After(KTimeForDisplay);

	
	return TestStepResult();
	}

/**
* Function  Name		: executeStepL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function sets the subsystem and component name
* 						  checks weather they are set in the log file correctly

*/


TInt CFloggerTest026_01::executeStepL()
	{
	return KErrGeneral;
	}
/**
* Function  Name		: executeStepL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function sets the subsystem and component name
* 						  checks weather they are set in the log file correctly

*/


TInt CFloggerTest026_01::executeStepL(TBool heapTest)
	{
	return DoTestWrite(heapTest);
	}


/**
* Function  Name		: DoTestWriteL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function writes the data to the file logger
					  
*/


TInt CFloggerTest026_01::DoTestWrite(TBool heapTest)
	{

	_LIT8(KTestMessage ,"TC no 26: This is test message");
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
				theFlogger.WriteBinary(KTestMessage);
				ForceLogFlush(theFlogger);
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


TInt CFloggerTest026_01::DoTestCheckWriteL()
	{
	User::After(1000*1000);
	RFile theFile;
	HBufC8 * hBuffer;
	TInt listfilesize,returnCode;
	RFs iFileSystem; //For file operation create a file system
	TBuf8<256> testData; //To hold the test descriptor
	_LIT(KLogFile,"c:\\logs\\log.txt"); 

	_LIT(KTestSystem, "SubSystem"); //Test sytem name
	_LIT(KTestComponent, "Component"); //Test component name
	_LIT8(KTestMessage ,"TC no 26: This is test message"); // Test descriptor

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
	
	testData.Copy(KTestMessage); //Copy the test message 
	returnCode = ptrString.Find(KTestMessage);	//find the test message  in the 
											//buffer read from the file
	if (returnCode == KErrNotFound)
		{
		returnCode = ptrString.Find(KOOMError);
		if (returnCode > 0)
			User::Leave(KErrNoMemory);
		}

	if (returnCode > 0) // Test message present
		{
		testData.Copy(KTestSystem); //Copy the test tag
		returnCode = ptrString.Find(testData);	//Find the test tag in the 
												//buffer read from the file
		}
	if (returnCode > 0) // Test tag present
		{
		testData.Copy(KTestComponent); //Copy the test tag
		returnCode = ptrString.Find(testData);	//Find the test tag in the 
												//buffer read from the file
		}
	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy();	//theFile

	if (returnCode > 0) // Test tag present
		return KErrNone;
	else 
		return KErrNotFound;
	}





/**
* Function  Name : CFloggerTest026_02
* Input parameters : None
* Output parameters : None
* Description : This is the constructor
*/


CFloggerTest026_02::CFloggerTest026_02()
	{
	// store the name of this test case
	SetTestStepName(_L("step_026_02"));
	}


/**
* Function  Name :~ CFloggerTest026_02
* Input parameters : None
* Output parameters : None
*Description : This is the Destructor
*/


CFloggerTest026_02::~CFloggerTest026_02()
	{
	}


/**
* Function  Name : doTestStepL
* Input parameters : None
* Output parameters : TVerdict 
* Description : This function is responsible for doing the heap test analysis 

*/

TVerdict CFloggerTest026_02::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 026.02 called "));
	User::After(KTimeForDisplay);

	CFloggerTest026_01* step026_01 = new CFloggerTest026_01;
	CleanupStack::PushL(step026_01);
	doTestStepWithHeapFailureL( *step026_01, 1, 20, KErrNone, ETrue);
	
	CleanupStack::PopAndDestroy(step026_01);

	return TestStepResult();
	}


/**
* Function  Name : CFloggerTest026_03
* Input parameters : None
* Output parameters : None
* Description : This is the constructor
*/


CFloggerTest026_03::CFloggerTest026_03()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_026_03"));
	}


/**
* Function  Name :~ CFloggerTest026_03
* Input parameters : None
* Output parameters : None
* Description : This is the Destructor
*/ 


CFloggerTest026_03::~CFloggerTest026_03()
	{
	}


/**
* Function  Name	: doTestStepL
* Input parameters	: None
* Output parameters : TVerdict 
* Description		: This function returns weather the test case 026_01 has 
* 					  passed or failed

*/


TVerdict CFloggerTest026_03::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 026.03 called "));
	User::After(KTimeForDisplay);


	if ( executeStepL() == KErrNone  )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	INFO_PRINTF1(_L("leaving Step 026.03 "));
	User::After(KTimeForDisplay);

	
	return TestStepResult();
	}


/**
* Function  Name		: executeStepL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function sets the subsystem and component name
* 						  checks weather they are set in the log file correctly

*/


TInt CFloggerTest026_03::executeStepL()
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


TInt CFloggerTest026_03::executeStepL(TBool)
	{
	return KErrGeneral;
	}

/**
* Function  Name		: DoTestWriteL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function writes the data to the file logger
					  
*/


TInt CFloggerTest026_03::DoTestWrite()
	{

	_LIT8(KTestLongMessage,"TEST 26.3:#À!}!}!} }4}%}&´zß}}&} } } } }'}}(}!æ~.=¢ÛR.... .~ÿ}#À!}!}!} }4}%}&„¥T}9}}&}}'}}(}U­~... .~. -ÿ}#À!}}!} }4}%}&„¥T}9}}&} } } } }'}}(}¾Ä~)... .~. .ÿ}#À!}!}} }$µZ}'}}(}U­~... .~. -ÿ}#À!}}!} }4}%}&„¥T}9}}&} } } } }'}}(}¾Ä~... .~. .ÿ}#À!}!}$µZ"); //8 bit test decriptor
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
				theFlogger.WriteBinary(KTestLongMessage);
//				ForceLogFlush(theFlogger);
				TRAPD(r, ret = DoTestCheckWriteL());
				if (r != KErrNone)
					ret = r;
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


TInt CFloggerTest026_03::DoTestCheckWriteL()
	{
	_LIT8(KTestLongMessage,"TEST 26.3:#À!}!}!} }4}%}&´zß}}&} } } } }'}}(}!æ~.=¢ÛR.... .~ÿ}#À!}!}!} }4}%}&„¥T}9}}&}}'}}(}U­~... .~. -ÿ}#À!}}!} }4}%}&„¥T}9}}&} } } } }'}}(}¾Ä~)... .~. .ÿ}#À!}!}} }$µZ}'}}(}U­~... .~. -ÿ}#À!}}!} }4}%}&„¥T}9}}&} } } } }'}}(}¾Ä~... .~. .ÿ}#À!}!}$µZ"); //8 bit test decriptor
	User::After(KTimeToLogLargeData);	// occasionally, KTimeToLog isn't quite long enough, so use longer time.
	RFile theFile;
	HBufC8 * hBuffer;
	TInt listfilesize,returnCode;
	RFs iFileSystem; //For file operation create a file system
	_LIT(KLogFile,"c:\\logs\\log.txt"); 
	TBuf8<256> testData; //To hold the test descriptor

	_LIT(KTestSystem, "SubSystem"); //Test sytem name
	_LIT(KTestComponent, "Component"); //Test component name
	//_LIT8(KTestLongMessage,"TEST 26.3: This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message"); //8 bit test decriptor
	
	
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
	
	//testData.Copy(KTestLongMessage); //Copy the test message 
	returnCode = ptrString.Find(KTestLongMessage);	//find the test message in the 
											//buffer read from the file

	if (returnCode > 0) // Test message present
		{
		testData.FillZ();
		testData.Copy(KTestSystem); //Copy the test tag
		returnCode = ptrString.Find(testData);	//Find the test tag in the 
												//buffer read from the file
		}
	if (returnCode > 0) // Test tag present
		{
		testData.FillZ();
		testData.Copy(KTestComponent); //Copy the component name
		returnCode = ptrString.Find(testData);	//Find the component name in the 
												//buffer read from the file
		}
	
	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy();	//theFile

	if (returnCode > 0) // Test tag present
		return KErrNone;
	else 
		return KErrGeneral;
	}




