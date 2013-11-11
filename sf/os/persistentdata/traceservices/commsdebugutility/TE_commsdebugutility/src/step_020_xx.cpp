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
// This contains  Unit Test Case 020.01

// EPOC includes
#include <e32base.h>

// Test system includes
#include "teststepcomsdbg.h"
#include "step_020_xx.h"

#include <comms-infras/commsdebugutility.h>


/**
* Function  Name	: CFloggerTest020_01
* Input parameters	: None
* Output parameters : None
* Description		: This is the constructor
*/


CFloggerTest020_01::CFloggerTest020_01()
	{
	// store the name of this test case
	SetTestStepName(_L("step_020_01"));
	}


/**
* Function  Name	: CFloggerTest020_01
* Input parameters	: None
* Output parameters : None
* Description		: This is the constructor
*/


CFloggerTest020_01::~CFloggerTest020_01()
	{
	}


/**
* Function  Name	: doTestStepL
* Input parameters	: None
* Output parameters : TVerdict 
* Description		: This function returns weather the test case 020_01 has 
* 					  passed or failed

*/


TVerdict CFloggerTest020_01::doTestStepL( )
	{
	User::After(10000*100);
	SetTestStepResult(EFail);

	if (executeStepL(EFalse) ==KErrNone)
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EPass);

		
	return TestStepResult();	
	}


TInt CFloggerTest020_01::executeStepL(TBool heapTest)
	{
	User::After(10000*100);
	return DoTestWrite(heapTest);
	}

TInt CFloggerTest020_01::executeStepL()
	{
	return KErrGeneral;
	}




/**
* Function  Name		: DoTestWrite
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function writes the data to the file logger
 
*/


TInt CFloggerTest020_01::DoTestWrite(TBool heapTest)
	{
	TInt ret ;
	TPtrC ptrSubSystem;
	TPtrC ptrComponent;
	ptrSubSystem.Set(_L("SubSystem"));
	ptrComponent.Set(_L("Component"));
	_LIT8(KTestMessage ,"TC 020_00: This is test message");
	
	RFileLogger theFlogger; 
	ret = theFlogger.Connect();
	
	if ( ret == KErrNone)
	{
		//CreateLog by passing the subsystem and component name as parameters
		theFlogger.CreateLog(ptrSubSystem, ptrComponent, EFileLoggingModeOverwrite); 
		theFlogger.ClearLog();		
		theFlogger.Write(KTestMessage);
		ForceLogFlush(theFlogger);

		// Close the logger and allow it time to close before checking the log!
		theFlogger.Close();
		User::After(1000000*1);

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

	return ret;		
	}




/**
* Function  Name		: DoTestCheckWriteL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function checks the weather test data was written
* 						  in to the log file by DoTestWriteL() or not.
					  
*/


TInt CFloggerTest020_01::DoTestCheckWriteL()
	{
	RFile theFile;
	HBufC8 * hBuffer;
	TInt listfilesize,returnCode;
	RFs fileSystem; //For file operation create a file system
	TBuf8<256> testData; //To hold the test descriptor
	_LIT(KLogFile,"c:\\logs\\log.txt"); // log file name and path

	_LIT(KTestMessage,"TC 020_00: This is test message"); //unicode test decriptor
	_LIT8(KOOMError, "#Logs may be lost out of memory!!");
	
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
* Function  Name	: CFloggerTest020_02
* Input parameters	: None
* Output parameters : None
* Description		: This is the constructor
*/


CFloggerTest020_02::CFloggerTest020_02()
	{
	// store the name of this test case
	SetTestStepName(_L("step_020_02"));
	
	}


/**
* Function  Name	:~ CFloggerTest020_02
* Input parameters	: None
* Output parameters : None
* Description		: This is the Destructor
*/


CFloggerTest020_02::~CFloggerTest020_02()
	{
	}


/**
* Function  Name	: doTestStepL
* Input parameters	: None
* Output parameters : TVerdict 
* Description		: This function returns weather the test case 020_02 has 
* 					  passed or failed

*/


TVerdict CFloggerTest020_02::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 020.02 called "));

	TInt ret = KErrGeneral;
		
	TRAPD(r, ret = executeStepL());
	if (r == KErrNone && ret == KErrNone)
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	
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


TInt CFloggerTest020_02::executeStepL()
	{
	TPtrC ptrSubSystem;
	TPtrC ptrComponent;
	ptrSubSystem.Set(_L("SubSystem"));
	ptrComponent.Set(_L("Component"));
	
	RFileLogger theFlogger; 
	//CreateLog by passing the subsystem and component name as parameters
	
	theFlogger.CreateLog(ptrSubSystem, ptrComponent, EFileLoggingModeAppend); 
	
	return KErrNone;
	}

/**
* Function  Name		: executeStepL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function writes the test data in to the log file 
* 						  This function check the test message is present in the 
* 						  log file 

*/


TInt CFloggerTest020_02::executeStepL(TBool)
	{
	return KErrGeneral;
	}
/**
* Function  Name	: CFloggerTest020_03
* Input parameters	: None
* Output parameters : None
* Description		: This is the constructor
*/



CFloggerTest020_03::CFloggerTest020_03()
	{
	// store the name of this test case
	SetTestStepName(_L("step_020_03"));
	}


//*****************************************************************
//Function  Name	:~ CFloggerTest020_03
//Input parameters	: None
//Output parameters : None
//Description		: This is the Destructor
//*****************************************************************


CFloggerTest020_03::~CFloggerTest020_03()
	{
	}


/**
* Function  Name : doTestStepL
* Input parameters : None
* Output parameters : TVerdict 
* Description : This function is responsible for doing the heap test analysis 

*/


TVerdict CFloggerTest020_03::doTestStepL( )
	{
	
	INFO_PRINTF1(_L("Step 020.03 called "));

	CFloggerTest020_01* step020_01 = new CFloggerTest020_01;
	CleanupStack::PushL(step020_01);
	doTestStepWithHeapFailureL( *step020_01, 1, 20, KErrNone, ETrue);

	CleanupStack::PopAndDestroy(step020_01);

	return TestStepResult();
	}
