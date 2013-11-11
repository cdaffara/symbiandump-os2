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
// This contains Flogger Unit Test Case 006.xx

// EPOC includes
#include <e32base.h>

// Test system includes
#include <comms-infras/commsdebugutility.h>
#include <f32file.h>


//User includes
#include "teststepcomsdbg.h"
#include "step_006_xx.h"


/**
* Function  Name	: CFloggerTest006_01
* Input parameters	: None
* Output parameters : None
* Description		: This is the constructor
*/


CFloggerTest006_01::CFloggerTest006_01()
	{
	SetTestStepName(_L("step_006_01"));

	}


/**
* Function  Name	:~ CFloggerTest006_01
* Input parameters	: None
* Output parameters : None
* Description		: This is the Destructor
*/


CFloggerTest006_01::~CFloggerTest006_01()
	{
	
	}


/**
* Function  Name	: doTestStepL
* Input parameters	: None
* Output parameters : TVerdict 
* Description		: This function returns weather the test case 006_01 has 
* 					  passed or failed

*/


TVerdict CFloggerTest006_01::doTestStepL( )
	{
	RFileLogger logger;
	INFO_PRINTF1(_L("Step 006.01 called "));
	
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
*						  log file or not.

*/


TInt CFloggerTest006_01::executeStepL(TBool heapTest)
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
			else if (r != KErrNone)
				{
				ret = r;
				}
			}
		}

	CleanupStack::PopAndDestroy();	//logger
	return ret;
	}

/**
* Function  Name		: executeStepL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function writes the test data in to the log file 
*						  This function check the test message is present in the 
*						  log file or not.

*/


TInt CFloggerTest006_01::executeStepL()
	{
	return KErrGeneral;
	}

/**
* Function  Name		: DoTestWrite
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function writes the data to the file logger
 
*/


TInt CFloggerTest006_01::DoTestWrite()
	{
	_LIT16(KTestMessageOne,"TC 006_01: The value of test integer variable :%d");// The value of second test integer variable : %d");
	return DoTestWriteFormat(KTestMessageOne, 100);  //Write unicode formatted test descriptor
	}


/**
* Function  Name		: DoTestWriteFormat
* Input parameters		: TRefByValue<const TDesC16>
* Output parameters		: TInt 
* Description 			: This function writes formatted data to the file logger
 
*/


TInt CFloggerTest006_01::DoTestWriteFormat(TRefByValue<const TDesC16> aFmt,...)
	{
	VA_LIST list;
	VA_START(list,aFmt);
	iFlogger.WriteFormat(aFmt,list);

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


TInt CFloggerTest006_01::DoTestCheckWriteL()
	{
	User::After(KTimeToLog);

	RFile theFile;
	HBufC8 * hBuffer;
	TInt listfilesize;
	TInt returnCode;
	RFs fileSystem; //For file operation create a file system	
	TBuf8<256> testData; //To hold the test descriptor

	_LIT8(KTestMessageOne,"TC 006_01: The value of test integer variable :%d");
	testData.Format(KTestMessageOne,100);

	_LIT8(KOOMError, "#Logs may be lost out of memory!!"); //Error message

	_LIT(KLogFile,"c:\\logs\\log.txt"); // the log file path

	User::LeaveIfError(fileSystem.Connect());
	
	//Open the file in the read mode
	User::LeaveIfError(theFile.Open(fileSystem,KLogFile,EFileRead)); 

	CleanupClosePushL(theFile);

	
	User::LeaveIfError(returnCode = theFile.Size(listfilesize)); //Size of the file
	hBuffer = HBufC8::New(listfilesize); //Allocate the buffer
	CleanupStack::PushL(hBuffer);
	TPtr8 ptrString = hBuffer->Des();  ; //To hold the buffer

	// Read from position 0: start of file
	returnCode = theFile.Read(ptrString);
	
	returnCode = ptrString.Find(testData); //find the test descriptor in the buffer read
									//from the file

	if (returnCode == KErrNotFound)  //Find the error message
		{
		returnCode = ptrString.Find(KOOMError);
		if (returnCode > 0)
			User::Leave(KErrNoMemory);
		}


	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy(); // For theFile object
	if (returnCode > 0)
		return KErrNone;
	else 
		return KErrNotFound;
	}



/**
* Function  Name		: DoTestConnect
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function establishes a connection to file logger
*						  and sets the system and component name in file logger.
					  
*/


TInt CFloggerTest006_01::DoTestConnect()
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
* Function  Name	: CFloggerTest006_02
* Input parameters	: None
* Output parameters : None
* Description		: This is the constructor
*/


CFloggerTest006_02::CFloggerTest006_02()
	{
	// store the name of this test case
	SetTestStepName(_L("step_006_02"));
	}


/**
* Function  Name	:~ CFloggerTest006_02
* Input parameters	: None
* Output parameters : None
* Description		: This is the Destructor
*/


CFloggerTest006_02::~CFloggerTest006_02()
	{
	}


/**
* Function  Name	: doTestStepL
* Input parameters	: None
* Output parameters : TVerdict 
* Description		: This function returns weather the test case 006_02 has 
* 					  passed or failed

*/


TVerdict CFloggerTest006_02::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 006.02 called "));
		
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


TInt CFloggerTest006_02::executeStepL()
	{
	TInt ret = KErrGeneral;
	
	ret = DoTestConnect();
	CleanupClosePushL(iFlogger);
	if (ret == KErrNone)
		ret = DoTestWrite();
		if (ret == KErrNone)
			ret = DoTestCheckWriteL();
	
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


TInt CFloggerTest006_02::executeStepL(TBool)
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


TInt CFloggerTest006_02::DoTestConnect()
	{
	TInt ret = KErrGeneral;
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


TInt CFloggerTest006_02::DoTestWrite()
	{
	//KTestLongMessage contains a very long test message 
	_LIT(KTestLongMessage,"TC 6_02: This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message %d");
	DoTestWriteFormat(KTestLongMessage, 100); //this causes a PANIC
	ForceLogFlush(iFlogger);	
	
	return KErrNone; 
	}


/**
* Function  Name		: DoTestWriteFormat
* Input parameters		: TRefByValue<const TDesC16>
* Output parameters		: TInt 
* Description 			: This function writes formatted data to the file logger
 
*/


TInt CFloggerTest006_02::DoTestWriteFormat(TRefByValue<const TDesC16> aFmt,...)
	{
	VA_LIST list;
	VA_START(list,aFmt);
	TRAPD(res, iFlogger.WriteFormat(aFmt,list)); 
	return res;
	}


/**
* Function  Name		: DoTestCheckWriteL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function checks the weather test data was written
*						  in to the log file by DoTestWriteL() or not.
					  
*/


TInt CFloggerTest006_02::DoTestCheckWriteL()
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
	_LIT(KTestLongMessage, "TC 6_02: This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message This is the long");
	
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
//Function  Name	: CFloggerTest006_03
//Input parameters	: None
//Output parameters : None
//Description		: This is the constructor
*/


CFloggerTest006_03::CFloggerTest006_03()
	{
	// store the name of this test case
	SetTestStepName(_L("step_006_03"));
	}


/**
* Function  Name	:~ CFloggerTest006_03
* Input parameters	: None
* Output parameters : None
* Description		: This is the Destructor
*/


CFloggerTest006_03::~CFloggerTest006_03()
	{
	
	}



/**
* Function  Name : doTestStepL
* Input parameters : None
* Output parameters : TVerdict 
* Description : This function is responsible for doing the heap test analysis 

*/


TVerdict CFloggerTest006_03::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 006.03 called "));

	CFloggerTest006_01* step006_01 = new CFloggerTest006_01;
	CleanupStack::PushL(step006_01);
	doTestStepWithHeapFailureL( *step006_01, 1, 20, KErrNone, ETrue);

	CleanupStack::PopAndDestroy(step006_01);
	User::After(KTimeForDisplay);
	return TestStepResult();
	}

