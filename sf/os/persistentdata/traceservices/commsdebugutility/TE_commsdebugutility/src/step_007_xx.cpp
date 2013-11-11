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
// This contains Flogger Unit Test Case 007.xx

// EPOC includes
#include <e32base.h>

// Test system includes
#include <comms-infras/commsdebugutility.h>
#include <f32file.h>


//User includes
#include "teststepcomsdbg.h"
#include "step_007_xx.h"


/**
* Function  Name	: CFloggerTest007_01
* Input parameters	: None
* Output parameters : None
* Description		: This is the constructor
*/

CFloggerTest007_01::CFloggerTest007_01()
	{
	// store the name of this test case
	SetTestStepName(_L("step_007_01"));
	
	}
	

/**
* Function  Name	:~ CFloggerTest007_01
* Input parameters	: None
* Output parameters : None
* Description		: This is the Destructor
*/


CFloggerTest007_01::~CFloggerTest007_01()
	{

	}


/**
* Function  Name	: doTestStepL
* Input parameters	: None
* Output parameters : TVerdict 
* Description		: This function returns weather the test case 007_01 has 
*					  passed or failed
*/


TVerdict CFloggerTest007_01::doTestStepL( )
	{

	INFO_PRINTF1(_L("Step 007.01 called "));
	
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
						  log file  or not.

*/


TInt CFloggerTest007_01::executeStepL(TBool heapTest)
	{
	User::After(KTimeToLog);
	TInt ret = KErrGeneral;
	ret = DoTestConnect();
	if (ret == KErrNone)
		{
		ret = DoTestWrite() ;
		if ( ret == KErrNone)
			{
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
		}
	iFlogger.Close();

//	if (ret == KErrNone)
	return ret;
	}

/**
* Function  Name		: executeStepL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function writes the test data in to the log file 
*						  This function check the test message is present in the 
						  log file  or not.

*/


TInt CFloggerTest007_01::executeStepL()
	{
	return KErrGeneral;
	}


/******************************************************************
* Function  Name		: DoTestWrite
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function writes the data to the file logger
*
*/


TInt CFloggerTest007_01::DoTestWrite()
	{	
	_LIT8(KTestMessage,"TC 7_01: Just for compalation"); //8 bit test decriptor
	iFlogger.Write(KTestMessage); //Writes a 8 bit test descriptor
	
	ForceLogFlush(iFlogger);
	return KErrNone; 
	}



/******************************************************************
* Function  Name		: DoTestCheckWriteL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function checks the weather test data was written
*						  in to the log file by DoTestWriteL() or not.
					  
*/


TInt CFloggerTest007_01::DoTestCheckWriteL()
	{
	User::After(KTimeToLog);
	RFile theFile;
	HBufC8 * hBuffer;
	TInt listfilesize;
	TInt returnCode;
	RFs fileSystem; //For file operation create a file system	
	TBuf8<256> testData; //To hold the test descriptor
	_LIT(KLogFile,"c:\\logs\\log.txt"); // the log file path

	_LIT8(KTestMessage,"TC 7_01: Just for compalation"); //8 bit test decriptor

	_LIT8(KOOMError, "#Logs may be lost out of memory!!"); //Error message

	User::LeaveIfError(fileSystem.Connect()); //connect to file server
	
	//Open the file in the read mode
	User::LeaveIfError(theFile.Open(fileSystem,KLogFile,EFileRead));

	CleanupClosePushL(theFile);

	returnCode = theFile.Size(listfilesize); //Size of the file
	hBuffer = HBufC8::New(listfilesize); //Allocate the buffer
	CleanupStack::PushL(hBuffer);
	TPtr8 ptrString = hBuffer->Des();  ; //To hold the buffer

	// Read from position 0: start of file
	returnCode = theFile.Read(ptrString);
	
	testData.Copy(KTestMessage); //Copy the test descriptor
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
		return KErrNoMemory;
	}



/**
* Function  Name		: DoTestConnect
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function establishes a connection to file logger
* 						  and sets the system and component name in file logger.
					  
*/


TInt CFloggerTest007_01::DoTestConnect()
	{
	TInt ret;
	TPtrC8 ptrSubSystem;
	TPtrC8 ptrComponent;
	ptrSubSystem.Set(_L8("SubSystem"));
	ptrComponent.Set(_L8("Component"));

	ret = iFlogger.Connect(); //Call connect() of Flogger
	if (ret == KErrNone)
		ret = iFlogger.SetLogTags(ptrSubSystem, ptrComponent); //SetLogTags() of Flogger called

//	if (ret == KErrNone)
//		ret = iFlogger.ClearLog(); //clear the contents from the log
	return ret;
	}





/**
* Function  Name	: CFloggerTest007_02
* Input parameters	: None
* Output parameters : None
* Description		: This is the constructor
*/


CFloggerTest007_02::CFloggerTest007_02()
	{
	// store the name of this test case
	SetTestStepName(_L("step_007_02"));
	}


/**
* Function  Name	:~ CFloggerTest007_02
* Input parameters	: None
* Output parameters : None
* Description		: This is the Destructor
*/


CFloggerTest007_02::~CFloggerTest007_02()
	{
	}


/**
* Function  Name	: doTestStepL
* Input parameters	: None
* Output parameters : TVerdict 
* Description		: This function returns weather the test case 007_02 has 
* 					  passed or failed

*/


TVerdict CFloggerTest007_02::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 007.02 called "));
		
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
* 						  log file 

*/


TInt CFloggerTest007_02::executeStepL()
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
* 						  log file 

*/


TInt CFloggerTest007_02::executeStepL(TBool)
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


TInt CFloggerTest007_02::DoTestConnect()
	{
	TInt ret;
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


TInt CFloggerTest007_02::DoTestWrite()
	{
	//KTestLongMessage contains a very long test message 
	_LIT(KTestLongMessage,"TC 7_02: This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message");
	iFlogger.Write(KTestLongMessage); 		
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


TInt CFloggerTest007_02::DoTestCheckWriteL()
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
	_LIT(KTestLongMessage, "TC 7_02: This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message This is the long");
	
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
* Function  Name	: CFloggerTest007_03
* Input parameters	: None
* Output parameters : None
* Description		: This is the constructor
*/


CFloggerTest007_03::CFloggerTest007_03()
	{
	// store the name of this test case
	SetTestStepName(_L("step_007_03"));
	}



/**
* Function  Name	:~ CFloggerTest007_03
* Input parameters	: None
* Output parameters : None
* Description		: This is the Destructor
*/



CFloggerTest007_03::~CFloggerTest007_03()
	{
	}



/**
* Function  Name : doTestStepL
* Input parameters : None
* Output parameters : TVerdict 
* Description : This function is responsible for doing the heap test analysis 

*/


TVerdict CFloggerTest007_03::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 007.03 called "));

	CFloggerTest007_01* step007_01 = new CFloggerTest007_01;
	CleanupStack::PushL(step007_01);
	doTestStepWithHeapFailureL( *step007_01, 1, 20, KErrNone, ETrue);

	CleanupStack::PopAndDestroy(step007_01);
	User::After(KTimeForDisplay);
	return TestStepResult();
	}

