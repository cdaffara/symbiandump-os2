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
// This contains Flogger Unit Test Case 005.xx

// EPOC includes
#include <e32base.h>

// Test system includes
#include <comms-infras/commsdebugutility.h>
#include <f32file.h>


//User includes
#include "teststepcomsdbg.h"
#include "step_005_xx.h"
#include "TestMessage.h"


/**
* Function  Name : CFloggerTest005_01
* Input parameters : None
* Output parameters : None
* Description : This is the constructor
*/


CFloggerTest005_01::CFloggerTest005_01()
	{
	// store the name of this test case
	SetTestStepName(_L("step_005_01"));
	}


/**
* Function  Name :~ CFloggerTest005_01
* Input parameters : None
* Output parameters : None
* Description : This is the Destructor
*/


CFloggerTest005_01::~CFloggerTest005_01()
	{
	}


/**
* Function  Name : doTestStepL
* Input parameters : None
* Output parameters : TVerdict 
* Description : This function returns weather the test case 005_01 has 
				passed or failed

*/


TVerdict CFloggerTest005_01::doTestStepL( )
	{
	RFileLogger logger;
	INFO_PRINTF1(_L("Step 005.01 called "));
	
	if ( executeStepL(EFalse) == KErrNone )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	
	User::After(KTimeForDisplay);
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


TInt CFloggerTest005_01::executeStepL(TBool heapTest)
	{
	TInt ret = KErrNone;

	ret = DoTestConnect();
	CleanupClosePushL(iFlogger);
	if (ret == KErrNone)
		{
		ret = DoTestWrite();
		if ( ret == KErrNone)
			{
			TRAPD(r, ret = DoTestCheckWriteL());
			if ((heapTest) && ((ret == KErrNotFound) || (r != KErrNone)))
				{
				ret = KErrNoMemory;
				}
			else if ( r != KErrNone)
				{
				ret = r;
				}
			}
		}

//	if (ret == KErrNone)
	CleanupStack::PopAndDestroy();	//logger
	return ret;
	}

/**
* Function  Name		: executeStepL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function writes the test data in to the log file 
*						  This function check the test message is present in the 
*						  log file 

*/


TInt CFloggerTest005_01::executeStepL()
	{
	return KErrGeneral;
	}

/**
* Function  Name		: DoTestWrite
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function writes the data to the file logger
 
*/


TInt CFloggerTest005_01::DoTestWrite()
	{
	_LIT16(KTestMessageOne,"TC 005_01: The value of test integer variable :%d");
	iFlogger.WriteFormat(KTestMessageOne, 100);  //Write unicode formatted test descriptor

	ForceLogFlush(iFlogger);
	return KErrNone; 
	}


/**
* Function  Name		: DoTestCheckWriteL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function checks the weather test data was written
*						  in to the log file by DoTestWriteL() or not.
					  
*/


TInt CFloggerTest005_01::DoTestCheckWriteL()
	{
	User::After(KTimeToLog);
	RFile theFile;
	HBufC8 * hBuffer;
	TInt listfilesize;
	TInt returnCode;
	RFs fileSystem; //For file operation create a file system	
	TInt numSuccessful = 0;

	_LIT8(KExpectedTestMessageOne,"TC 005_01: The value of test integer variable :100");

	User::LeaveIfError(fileSystem.Connect());
	CleanupClosePushL(fileSystem);
	
	//Open the file in the read mode
	User::LeaveIfError(theFile.Open(fileSystem,KFloggerOutputFile,EFileRead)); 
	CleanupClosePushL(theFile);

	User::LeaveIfError(returnCode = theFile.Size(listfilesize)); //Size of the file
	hBuffer = HBufC8::New(listfilesize); //Allocate the buffer
	CleanupStack::PushL(hBuffer);
	TPtr8 ptrString = hBuffer->Des();  ; //To hold the buffer

	// Read from position 0: start of file
	returnCode = theFile.Read(ptrString);
	
	returnCode = ptrString.Find(KExpectedTestMessageOne); //find the test descriptor in the buffer read
											//from the file
	if (returnCode > 0)  //Find the error message
		{
		numSuccessful++;
		}

	returnCode = ptrString.Find(KErrOOM);
	if (returnCode > 0)
		{
		User::Leave(KErrNoMemory);
		}
	

	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy();	//theFile object
	CleanupStack::PopAndDestroy();	//fileSystem object

	if (numSuccessful == 1)
		{
		return KErrNone;
		}
	else 
		{
		return KErrNotFound;
		}
	}



/**
* Function  Name		: DoTestConnect
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function establishes a connection to file logger
*						  and sets the system and component name in file logger.
					  
*/


TInt CFloggerTest005_01::DoTestConnect()
	{
	TInt ret = EFail;
	TPtrC8 ptrSubSystem;
	TPtrC8 ptrComponent;
	ptrSubSystem.Set(_L8("SubSystem"));
	ptrComponent.Set(_L8("Component"));

	ret = iFlogger.Connect(); //Call connect() of Flogger
	if (ret == KErrNone)
		ret = iFlogger.SetLogTags(ptrSubSystem, ptrComponent); //SetLogTags() of Flogger called

	if (ret == KErrNone)
		ret = iFlogger.ClearLog(); //clear the contents from the log

	return ret;
	}





/**
* Function  Name	: CFloggerTest005_02
* Input parameters	: None
* Output parameters : None
* Description		: This is the constructor
*/


CFloggerTest005_02::CFloggerTest005_02()
	{
	// store the name of this test case
	SetTestStepName(_L("step_005_02"));
	}


/**
* Function  Name	:~ CFloggerTest005_02
* Input parameters	: None
* Output parameters : None
* Description		: This is the Destructor
*/


CFloggerTest005_02::~CFloggerTest005_02()
	{
	}


/**
*Function  Name	: doTestStepL
*Input parameters	: None
*Output parameters : TVerdict 
*Description		: This function returns weather the test case 005_02 has 
*					  passed or failed

*/


TVerdict CFloggerTest005_02::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 005.02 called "));

	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	
	User::After(KTimeForDisplay);
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


TInt CFloggerTest005_02::executeStepL()
	{
	TInt ret = KErrNone;
		
	 ret = DoTestConnect();
	 CleanupClosePushL(iFlogger);
	if (ret == KErrNone)
		{
		ret = DoTestWrite() ;
			if (ret == KErrNone)
			{
			 TRAPD(r, ret = DoTestCheckWriteL());
			if (r != KErrNone)
				ret = r;
			}
		}
	
	CleanupStack::PopAndDestroy();	//logger
//	if (ret == KErrNone)
//		iFlogger.__DbgShutDownServer();
//	iFlogger.Close();
	return ret;
	}

/**
* Function  Name		: executeStepL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function writes the test data in to the log file 
*						  This function check the test message is present in the 
*						  log file 

*/


TInt CFloggerTest005_02::executeStepL(TBool)
	{
	return KErrGeneral;
	}

/**
* Function  Name		: DoTestConnect
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function establishes a connection to file logger
*						  and sets the system and component name in file logger.
					  
*/


TInt CFloggerTest005_02::DoTestConnect()
	{
	TInt ret = EFail;
	TPtrC8 ptrSubSystem;
	TPtrC8 ptrComponent;
	ptrSubSystem.Set(_L8("SubSystem"));
	ptrComponent.Set(_L8("Component"));

	ret = iFlogger.Connect(); //Call connect() of Flogger
	if (ret == KErrNone)
		ret = iFlogger.SetLogTags(ptrSubSystem, ptrComponent); //SetLogTags() of Flogger called

	if (ret == KErrNone)
		ret = iFlogger.ClearLog(); //clear the contents from the log

	return ret;
	}	


/**
* Function  Name		: DoTestWrite
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function writes the data to the file logger
 
*/


TInt CFloggerTest005_02::DoTestWrite()
	{
	_LIT16(KTestMessageTwo,"TC 5_02: The value of first test integer variable :%d The value of second test integer variable : %d");
	iFlogger.WriteFormat(KTestMessageTwo, 100, 200); 
	ForceLogFlush(iFlogger);
	iFlogger.__DbgShutDownServer();
//	iFlogger.Close();
	return KErrNone; 
	}


/**
* Function  Name		: DoTestCheckWriteL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function checks the weather test data was written
*						  in to the log file by DoTestWriteL() or not.
					  
*/


TInt CFloggerTest005_02::DoTestCheckWriteL()
	{
	User::After(KTimeToLog);

	RFile theFile;
	HBufC8 * hBuffer;
	TInt listfilesize;
	TInt returnCode;
	RFs fileSystem; //For file operation create a file system

	TPtrC8 ptrSubSystem;
	TPtrC8 ptrComponent;
	ptrSubSystem.Set(_L8("SubSystem"));
	ptrComponent.Set(_L8("Component"));
	_LIT(KLogFile,"c:\\logs\\log.txt"); // the log file path
	
	TBuf8<256> testData; //To hold the test descriptor
	_LIT8(KTestMessageTwo,"TC 5_02: The value of first test integer variable :%d The value of second test integer variable : %d");
	testData.Format(KTestMessageTwo,100, 200);

	_LIT8(KOOMError, "#Logs may be lost out of memory!!"); // Error message 

	User::LeaveIfError(fileSystem.Connect());
	
	//Open the file in the read mode
	User::LeaveIfError(theFile.Open(fileSystem,KLogFile,EFileRead)); 
	
	CleanupClosePushL(theFile);
	
	User::LeaveIfError(returnCode = theFile.Size(listfilesize)); //Size of the file
	hBuffer = HBufC8::New(listfilesize); //Allocate the buffer

	CleanupStack::PushL(hBuffer);
	TPtr8 ptrString = hBuffer->Des();  ; //To hold the buffer

	// Read from position 0: start of file
	User::LeaveIfError(returnCode = theFile.Read(ptrString));
	
	returnCode = ptrString.Find(testData); //find the test descriptor in the buffer read
											//from the file

	
	if (returnCode == KErrNotFound) // Check for the error message in the log
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
		return KErrGeneral;
	}







/**
*
* Function  Name	: CFloggerTest005_03
* Input parameters	: None
* Output parameters : None
* Description		: This is the constructor
*/


CFloggerTest005_03::CFloggerTest005_03()
	{
	
	SetTestStepName(_L("step_005_03"));
	}


/**
* Function  Name	:~ CFloggerTest005_03
* Input parameters	: None
* Output parameters : None
* Description		: This is the Destructor
*/


CFloggerTest005_03::~CFloggerTest005_03()
	{
		
	}


/**
* Function  Name	: doTestStepL
* Input parameters	: None
* Output parameters : TVerdict 
* Description		: This function returns weather the test case 005_03 has 
* 					  passed or failed

*/


TVerdict CFloggerTest005_03::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 005.03 called "));
		
	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	
	User::After(KTimeForDisplay);
	return TestStepResult();
	}


/**
* Function  Name		: executeStepL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function writes the test data in to the log file 
* 						  This function check the test message is present in the 
*						  log file 

*/


TInt CFloggerTest005_03::executeStepL()
	{
	TInt ret = KErrGeneral;
	
	ret = DoTestConnect();
	 CleanupClosePushL(iFlogger);
	if (ret == KErrNone)
		{
		ret = DoTestWrite();
		if ( ret == KErrNone)
			{
			TRAPD(r, ret = DoTestCheckWriteL());
			if ( r != KErrNone)
				ret = r;
			}
		}
	
	CleanupStack::PopAndDestroy();	//logger

	User::After(1000000);
	return ret;
	}

/**
* Function  Name		: executeStepL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function writes the test data in to the log file 
* 						  This function check the test message is present in the 
*						  log file 

*/


TInt CFloggerTest005_03::executeStepL(TBool)
	{
	return KErrGeneral;
	}

/**
* Function  Name		: DoTestConnect
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function establishes a connection to file logger
* 						  and sets the system and component name in file logger.
					  
*/

TInt CFloggerTest005_03::DoTestConnect()
	{
	TInt ret;
	TPtrC8 ptrSubSystem;
	TPtrC8 ptrComponent;
	ptrSubSystem.Set(_L8("SubSystem"));
	ptrComponent.Set(_L8("Component"));

	ret = iFlogger.Connect(); //Call connect() of Flogger
	User::After(1000000);
	if (ret == KErrNone)
		ret = iFlogger.SetLogTags(ptrSubSystem, ptrComponent); //SetLogTags() of Flogger called

	if (ret == KErrNone)
		ret = iFlogger.ClearLog(); //clear the contents from the log
	
	return ret;
	}


/**
* Function  Name		: DoTestWrite
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function writes the data to the file logger
 
*/

TInt CFloggerTest005_03::DoTestWrite()
	{
	//KTestLongMessage contains a very lengthy message 
	
	_LIT16(KTestLongMessage,"TC 5_03: This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message This is the long %d"); //8 bit long test decriptor
	iFlogger.WriteFormat(KTestLongMessage, 100);
	ForceLogFlush(iFlogger);	
	return KErrNone; 
	}



/**
* Function  Name		: DoTestCheckWriteL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function checks the weather test data was written
*						  in to the log file by DoTestWriteL() or not.
					  
*/


TInt CFloggerTest005_03::DoTestCheckWriteL()
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
	_LIT8(KTestLongMessage, "TC 5_03: This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message This is the long");
	

	testData.Format(KTestLongMessage,100);

		
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
* Function  Name	: CFloggerTest005_04
* Input parameters	: None
* Output parameters : None
* Description		: This is the constructor
*/


CFloggerTest005_04::CFloggerTest005_04()
	{
	// store the name of this test case
	SetTestStepName(_L("step_005_04"));
	}


/**
* Function  Name	: CFloggerTest005_04
* Input parameters	: None
* Output parameters : None
* Description		: This is the constructor
*/


CFloggerTest005_04::~CFloggerTest005_04()
	{
	
	}



/**
* Function  Name : doTestStepL
* Input parameters : None
* Output parameters : TVerdict 
* Description : This function is responsible for doing the heap test analysis 

*/


TVerdict CFloggerTest005_04::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 005.04 called "));

	CFloggerTest005_01* step005_01 = new CFloggerTest005_01;
	CleanupStack::PushL(step005_01);
	doTestStepWithHeapFailureL( *step005_01, 445, 520, KErrNone, ETrue);

	CleanupStack::PopAndDestroy(step005_01);
	User::After(KTimeForDisplay);
	return TestStepResult();
	}	

