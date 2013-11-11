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
// This contains Flogger Unit Test Case 004.xx

// EPOC includes
#include <e32base.h>
#include <f32file.h>

// Test system includes
#include "teststepcomsdbg.h"
#include "step_004_xx.h"
#include "TestMessage.h"

#include <comms-infras/commsdebugutility.h>


/**
* Function  Name : CFloggerTest004_01
* Input parameters : None
* Output parameters : None
* Description : This is the constructor
*/


CFloggerTest004_01::CFloggerTest004_01()
	{
	SetTestStepName(_L("step_004_01"));
	}


/**
* Function  Name	:~ CFloggerTest004_01
* Input parameters	: None
* Output parameters : None
* Description		: This is the Destructor
*/


CFloggerTest004_01::~CFloggerTest004_01()
	{
	}


/**
* Function  Name	: doTestStepL
* Input parameters	: None
* Output parameters : TVerdict 
* Description		: This function returns weather the test case 004_01 has 
*					  passed or failed

*/


TVerdict CFloggerTest004_01::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 004.01 called "));
	
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


TInt CFloggerTest004_01::executeStepL(TBool heapTest)
	{
	TInt ret = KErrNone;

	ret = iFlogger.Connect(); //Call connect() of Flogger
	if (ret == KErrNone)	if (ret == KErrNone)
		{
		CleanupClosePushL(iFlogger);
		ret = iFlogger.SetLogTags(KStdSubsysTag8, KStdCompTag8); //SetLogTags() of Flogger called
		if (ret == KErrNone)
			{
			ret = iFlogger.ClearLog();
			if (ret == KErrNone)
				{
				if (ret == KErrNone)
					{
					ret = DoTestWrite();
					User::After(KTimeToLog);

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
				}
			}
		CleanupStack::PopAndDestroy();	//logger
		}
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


TInt CFloggerTest004_01::executeStepL()
	{
	return KErrGeneral;
	}


/**
* Function  Name		: DoTestWrite
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function writes the test data in to the log file 
					  
*/


TInt CFloggerTest004_01::DoTestWrite()
	{
	_LIT(KTestMessage,"TC 4.01 :This is test msg"); //unicode test decriptor
	iFlogger.Write(KTestMessage); 
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


TInt CFloggerTest004_01::DoTestCheckWriteL()
	{
	RFile theFile;
	HBufC8 * hBuffer;
	TInt listfilesize,returnCode;
	RFs fileSystem; //For file operation create a file system
	TBuf8<256> testData; //To hold the test descriptor
	_LIT(KLogFile,"c:\\logs\\log.txt"); // log file name and path
	TInt numSuccessful = 0;

	_LIT(KTestMessage,"TC 4.01 :This is test msg"); //unicode test decriptor
	
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

	if (returnCode != KErrNotFound)
		{
		numSuccessful++;
		}

	returnCode = ptrString.Find(KErrOOM);
	if (returnCode > 0)
		{
		User::Leave(KErrNoMemory);
		}
	

	
	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy();	//theFile
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
* Function  Name : CFloggerTest004_02
* Input parameters : None
* Output parameters : None
* Description : This is the constructor
*/


CFloggerTest004_02::CFloggerTest004_02()
	{
	// store the name of this test case
	SetTestStepName(_L("step_004_02"));
	
	}


/**
* Function  Name :~ CFloggerTest004_01
* Input parameters : None
* Output parameters : None
* Description : This is the Destructor
*/


CFloggerTest004_02::~CFloggerTest004_02()
	{
	}


/**
* Function  Name	: doTestStepL
* Input parameters	: None
* Output parameters : TVerdict 
* Description		: This function returns weather the test case 004_02 is 
*					  passed or failed

*/


TVerdict CFloggerTest004_02::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 004.02 called "));
		
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
* 						  log file or not.

*/


TInt CFloggerTest004_02::executeStepL()
	{
	TInt ret = KErrGeneral;
	
	ret = DoTestConnect();
	CleanupClosePushL(iFlogger);
	if (ret == KErrNone)
		{
		ret = DoTestWrite();
		// wait to give flogger time to log
		User::After(KTimeToLog);
		// catch any memory leaks
		iFlogger.__DbgShutDownServer();

		if (ret == KErrNone)
			{
			ret = DoTestCheckWriteL();
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
* 						  log file or not.

*/


TInt CFloggerTest004_02::executeStepL(TBool)
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


TInt CFloggerTest004_02::DoTestConnect()
	{
	TInt ret;
	TPtrC8 ptrSubSystem;
	TPtrC8  ptrComponent;
	ptrSubSystem.Set(_L8("SubSystem"));
	ptrComponent.Set(_L8("Component"));

	ret = iFlogger.Connect(); //Call connect() of Flogger
	if (ret == KErrNone)
		{
		ret = iFlogger.SetLogTags(ptrSubSystem, ptrComponent); //SetLogTags() of Flogger called
		}

	if (ret == KErrNone)
		{
		ret = iFlogger.ClearLog(); //clear the contents from the log
		}
	
	return ret;
	}


/**
* Function  Name		: DoTestWrite
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function writees the data to the file logger
 
*/


TInt CFloggerTest004_02::DoTestWrite()
	{
	//KTestLongMessage contains a very lengthy message 
	_LIT(KTestLongMessage,"TC 4_02: This is the long message This is the long message This is the long message This is the long messageThis is the long messageThis is the long messageThis is the long messageThis is the long messageThis is the long messageThis is the long messageThis is the long messageThis is ");
	
	iFlogger.Write(KTestLongMessage);
	return KErrNone; 
	}


/**
* Function  Name		: DoTestCheckWriteL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function checks the weather test data was written
*						  in to the log file by DoTestWriteL() or not.
					  
*/


TInt CFloggerTest004_02::DoTestCheckWriteL()
	{
	RFile theFile;
	HBufC8 * hBuffer;
	TInt listfilesize,returnCode;
	RFs fileSystem; //For file operation create a file system
	TBuf8<256> testData; //To hold the test descriptor
	_LIT(KLogFile,"c:\\logs\\log.txt"); // log file name and path

	//The following contains the string of length 200(max) 
	//which is  written to the log file
	//_LIT(KTestLongMessage, "This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message");
	_LIT(KTestLongMessage, "TC 4_02: This is the long message This is the long message This is the long message This is the long messageThis is the long messageThis is the long messageThis is the long messageThis is the long me");
	
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
* Function  Name : CFloggerTest004_03
* Input parameters : None
* Output parameters : None
* Description : This is the constructor
*/


CFloggerTest004_03::CFloggerTest004_03()
	{
	// store the name of this test case
	SetTestStepName(_L("step_004_03"));
	}


/**
* Function  Name :~ CFloggerTest004_01
* Input parameters : None
* Output parameters : None
* Description : This is the Destructor
*/


CFloggerTest004_03::~CFloggerTest004_03()
	{	
	}


/**
* Function  Name : doTestStepL
* Input parameters : None
* Output parameters : TVerdict 
* Description : This function is responsible for doing the heap test analysis 

*/


TVerdict CFloggerTest004_03::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 004.03 called "));

	CFloggerTest004_01* step004_01 = new CFloggerTest004_01;
	CleanupStack::PushL(step004_01);
	doTestStepWithHeapFailureL( *step004_01, 450, 470, KErrNone, ETrue);

	CleanupStack::PopAndDestroy(step004_01);
	User::After(KTimeForDisplay);
	return TestStepResult();
	}	

