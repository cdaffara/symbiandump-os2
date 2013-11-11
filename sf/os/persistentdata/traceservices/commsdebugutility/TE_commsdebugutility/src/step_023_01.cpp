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
// This contains Flogger Unit Test Case 023.01

// EPOC includes
#include <e32base.h>

// Test system includes
#include "teststepcomsdbg.h"
#include "step_023_01.h"


#include <comms-infras/commsdebugutility.h>


/**
* Function  Name	: CFloggerTest023_01
* Input parameters	: None
* Output parameters : None
* Description		: This is the constructor
*/

CFloggerTest023_01::CFloggerTest023_01()
	{
	// store the name of this test case
	SetTestStepName(_L("step_023_01"));
	}


/**
* Function  Name	:~ CFloggerTest023_01
* Input parameters	: None
* Output parameters : None
* Description		: This is the Destructor
*/


CFloggerTest023_01::~CFloggerTest023_01()
	{
	}



/**
* Function  Name	: doTestStepL
* Input parameters	: None
* Output parameters : TVerdict 
* Description		: This function returns weather the test case 023_01 has 
* 					  passed or failed

*/


TVerdict CFloggerTest023_01::doTestStepL( )
	{
	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	
	return TestStepResult();
	}

TInt CFloggerTest023_01::executeStepL(TBool)
	{
	return KErrGeneral;
	}

TInt CFloggerTest023_01::executeStepL( )
	{
	TInt ret = KErrGeneral;
	ret = DoTestWrite();
	if ( ret == KErrNone)
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
* Description 			: This function writes the test data in to the log file 
					  
*/


TInt CFloggerTest023_01::DoTestWrite()
	{
	INFO_PRINTF1(_L("Step 023.01 called "));

	RFileLogger theFlogger;
	TPtrC8 ptrSubSystem;
	TPtrC8 ptrComponent;
	ptrSubSystem.Set(_L8("SubSystem")); //Test system name
	ptrComponent.Set(_L8("Component")); //Test component name
	_LIT8(KTestMessage,"This is the test message"); //Test decriptor

	TInt ret = KErrGeneral;
	ret = theFlogger.Connect();

	if (ret == KErrNone) // To Write the test descriptor the following lines are used.
	{
		ret = theFlogger.SetLogTags(ptrSubSystem, ptrComponent); //To set the tags 
		if (ret == KErrNone)
		{
			theFlogger.Write(KTestMessage);
			theFlogger.__DbgShutDownServer();
			theFlogger.Close();	
		}
	}

	User::After(100*1000);	//Need this or connect will return KErrServerTerminated

	if (ret == KErrNone) // To actually test the ClearLog() functionality
		ret = theFlogger.Connect();
		if (ret == KErrNone)
			ret = theFlogger.SetLogTags(ptrSubSystem,ptrComponent);
			if (ret == KErrNone)
			{
				theFlogger.ClearLog(); //ClearLog called to clear the log message
				theFlogger.__DbgShutDownServer();
				User::After(100*1000);	//need this to give server time to clear the log
				theFlogger.Close();
			}

	theFlogger.Close();
	return ret;
	}


/**
* Function  Name		: DoTestCheckWriteL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function writes the data to the file logger
 
*/


TInt CFloggerTest023_01::DoTestCheckWriteL()
	{
	RFile theFile;
	HBufC8 * hBuffer;
	TInt listfilesize;
	TInt returnCode;
	RFs fileSystem; //For file operation create a file system
	TBuf8<256> testData; //To hold the test descriptor
	_LIT(KLogFile, "c:\\logs\\log.txt"); // log file name and path

	_LIT8(KTestMessage,"This is the test message"); //Test decriptor
	_LIT8(KSystemName,"SubSystem"); //Test subsystem name
	_LIT8(KComponentName,"Component");// Test component name

	User::LeaveIfError(fileSystem.Connect()); //Connect to the file server
	
	//Open the file in the read mode
	User::LeaveIfError(theFile.Open(fileSystem,KLogFile,EFileRead)); 
	CleanupClosePushL(theFile);
	
	User::LeaveIfError(returnCode = theFile.Size(listfilesize)); //Size of the file
	hBuffer = HBufC8::New(listfilesize); //Allocate the buffer
	CleanupStack::PushL(hBuffer);
	TPtr8 ptrString = hBuffer->Des();  ; //To hold the buffer

	// Read from position 0: start of file
	returnCode = theFile.Read(ptrString);
	
	testData.Copy(KTestMessage); //Copy the test descriptor
	returnCode = ptrString.Find(testData); //find the test descriptor in the buffer read
									//from the file
	
	if (returnCode < 0) //Note test message should not be present
		{
		testData.Copy(KSystemName); //Note test system name should not be present
		returnCode = ptrString.Find(testData); //find the test system in the buffer read
									//from the file
		}
	
	if (returnCode < 0) //Note test component name should not be present
		{
		testData.Copy(KComponentName); //Note test message should not be present
		returnCode = ptrString.Find(testData); //find the test component in the buffer read
									//from the file
		}

	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy();	//theFile

	if (returnCode < 0)
		return KErrNone;
	else 
		return KErrGeneral;

	}


/**
* Function  Name	: CFloggerTest023_02
* Input parameters	: None
* Output parameters : None
* Description		: This is the Constructor
*/


CFloggerTest023_02::CFloggerTest023_02()
	{
	// store the name of this test case
	SetTestStepName(_L("step_023_02"));
	}



/**
* Function  Name	:~ CFloggerTest023_02
* Input parameters	: None
* Output parameters : None
* Description		: This is the Destructor
*/ 


CFloggerTest023_02::~CFloggerTest023_02()
	{
	}



/**
* Function  Name	: doTestStepL
* Input parameters	: None
* Output parameters : TVerdict 
* Description		: This function returns weather the test case 023_02 has 
* 					  passed or failed

*/


TVerdict CFloggerTest023_02::doTestStepL( )
	{
	// INFO_PRINTF1(_L("Step 023.01 called ")); // This line was panicing in release mode testing

	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	
	return TestStepResult();
	}

TInt CFloggerTest023_02::executeStepL(TBool)
	{
	return KErrGeneral;
	}
	
TInt CFloggerTest023_02::executeStepL( )
	{
	TPtrC8 ptrSubSystemTmp;
	TPtrC8 ptrComponentTmp;
	ptrSubSystemTmp.Set(_L8("SubSystem"));
	ptrComponentTmp.Set(_L8("Component"));

		RFileLogger theLogger;
		theLogger.Connect();
		theLogger.ClearLog(); // This line of code panics since it is called prior to SetLogTags()!!!!
		theLogger.SetLogTags(ptrSubSystemTmp,ptrComponentTmp);
		return KErrNone;
	}

