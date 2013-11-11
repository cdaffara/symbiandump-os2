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
// See the test specification for details of what these test cases do.

#include "teststepcomsdbg.h"
#include "step_027_xx.h"

#include <comms-infras/commsdebugutility.h>


/**
* Function  Name : CFloggerTest027_01
* Input parameters : None
* Output parameters : None
* Description : This is the constructor
*/


CFloggerTest027_01::CFloggerTest027_01()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_027_01"));
	}


/**
* Function  Name :~ CFloggerTest027_01
* Input parameters : None
* Output parameters : None
* Description : This is the Destructor
*/ 


CFloggerTest027_01::~CFloggerTest027_01()
	{
	}


/**
* Function  Name	: doTestStepL
* Input parameters	: None
* Output parameters : TVerdict 
* Description		: This function returns whether the test case 027_01 has 
* 					  passed or failed

*/


TVerdict CFloggerTest027_01::doTestStepL( )
	{
	if ( executeStepL() == KErrNone  )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);


	
	return TestStepResult();
	}


/**
* Function  Name		: executeStepL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function calls the Connect twice

*/


TInt CFloggerTest027_01::executeStepL(TBool)
	{
	return KErrGeneral;
	}
/**
* Function  Name		: executeStepL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function calls the Connect twice

*/


TInt CFloggerTest027_01::executeStepL()
	{
	TInt numSuccess = 0;
	TInt ret;
	RFileLogger flogger;
	ret = flogger.Connect();

	if (ret == KErrNone)
		{
		numSuccess++;
		}

	//Call Connect() twice this will cause 2nd one ot return KErrorAlreadyExists and nothing to happen.
	ret = flogger.Connect();

	if (ret == KErrAlreadyExists)
		{
		numSuccess++;
		}


	if (numSuccess == 2)
		{
		return KErrNone;
		}
	
	return KErrBadHandle;
	}




/**
* Function  Name : CFloggerTest027_02
* Input parameters : None
* Output parameters : None
* Description : This is the constructor
*/


CFloggerTest027_02::CFloggerTest027_02()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_027_02"));
	}


/**
* Function  Name :~ CFloggerTest027_02
* Input parameters : None
* Output parameters : None
* Description : This is the Destructor
*/ 


CFloggerTest027_02::~CFloggerTest027_02()
	{
	}


/**
* Function  Name	: doTestStepL
* Input parameters	: None
* Output parameters : TVerdict 
* Description		: This function returns whether the test case 027_02 has 
* 					  passed or failed

*/


TVerdict CFloggerTest027_02::doTestStepL( )
	{
	
	INFO_PRINTF1(_L("Step 027.02 called "));
	TRAPD(res ,executeStepL());
	if (res == KErrNone )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	INFO_PRINTF1(_L("leaving Step 027.02 "));
	User::After(KTimeForDisplay);

	
	return TestStepResult();
	}

/**
* Function  Name		: executeStepL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function leaves without calling CleanupStack::PopAndDestroy()

*/


TInt CFloggerTest027_02::executeStepL(TBool)
	{
	return KErrGeneral;
	}

/**
* Function  Name		: executeStepL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function leaves without calling CleanupStack::PopAndDestroy()

*/


TInt CFloggerTest027_02::executeStepL()
	{
	TInt r = KErrNone;
	TPtrC8 ptrSubSystem;
	TPtrC8 ptrComponent;
	ptrSubSystem.Set(_L8("SubSystem"));
	ptrComponent.Set(_L8("Component"));
	__FLOG_DECLARATION_MEMBER;
	__FLOG_OPENC(ptrSubSystem, ptrComponent);
	User::Leave(r);	
	return KErrNone;
	}



/**
* Function  Name : CFloggerTest027_03
* Input parameters : None
* Output parameters : None
* Description : This is the constructor
*/


CFloggerTest027_03::CFloggerTest027_03()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_027_03"));
	}


/**
* Function  Name :~ CFloggerTest027_03
* Input parameters : None
* Output parameters : None
* Description : This is the Destructor
*/ 


CFloggerTest027_03::~CFloggerTest027_03()
	{
	}


/**
* Function  Name	: doTestStepL
* Input parameters	: None
* Output parameters : TVerdict 
* Description		: This function returns whether the test case 027_03 has 
* 					  passed or failed

*/


TVerdict CFloggerTest027_03::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 027.03 called "));
	TRAPD(ret, executeStepL());
	if (ret == KErrNone  )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	INFO_PRINTF1(_L("leaving Step 027.02 "));
	User::After(KTimeForDisplay);

	
	return TestStepResult();
	}


/**
* Function  Name		: executeStepL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function calls CleanupStack::PopAndDestroy() to check whether the flogger is pushed to the stack

*/


TInt CFloggerTest027_03::executeStepL()
	{
	TPtrC8 ptrSubSystem;
	TPtrC8 ptrComponent;
	ptrSubSystem.Set(_L8("SubSystem"));
	ptrComponent.Set(_L8("Component"));
	__FLOG_DECLARATION_MEMBER;
	__FLOG_OPENC(ptrSubSystem, ptrComponent);
	CleanupStack::PopAndDestroy();
	return KErrNone;
	}

/**
* Function  Name		: executeStepL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function calls CleanupStack::PopAndDestroy() to check whether the flogger is pushed to the stack

*/


TInt CFloggerTest027_03::executeStepL(TBool)
	{
	return KErrGeneral;
	}

/**
* Function  Name : CFloggerTest027_04
* Input parameters : None
* Output parameters : None
* Description : This is the constructor
*/


CFloggerTest027_04::CFloggerTest027_04()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_027_04"));
	}


/**
* Function  Name :~ CFloggerTest027_04
* Input parameters : None
* Output parameters : None
* Description : This is the Destructor
*/ 


CFloggerTest027_04::~CFloggerTest027_04()
	{
	}


/**
* Function  Name	: doTestStepL
* Input parameters	: None
* Output parameters : TVerdict 
* Description		: This function returns whether the test case 027_04 has 
* 					  passed or failed

*/


TVerdict CFloggerTest027_04::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 027.04 called "));
	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);


	return TestStepResult();
	}


/**
* Function  Name		: executeStepL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function calls the flogger write and checks whether the log message is written in to the log file

*/


TInt CFloggerTest027_04::executeStepL(TBool)
	{
	return KErrGeneral;
	}
/**
* Function  Name		: executeStepL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function calls the flogger write and checks whether the log message is written in to the log file

*/


TInt CFloggerTest027_04::executeStepL()
	{
	TInt ret = KErrNone;
	ret = DoTestWrite();
	if ( ret == KErrNone)
		{
		TRAPD(r, ret = DoTestCheckWriteL());
		if ( r != KErrNone)
			ret = r;
		}
	return ret;
	}


TInt CFloggerTest027_04::DoTestWrite()
	{
	_LIT8(KTestMessage,"TC 27_04: The is Test for macros");
	TPtrC8 ptrSubSystem;
	TPtrC8 ptrComponent;
	ptrSubSystem.Set(_L8("SubSystem"));
	ptrComponent.Set(_L8("Component"));
	__FLOG_DECLARATION_MEMBER;
	__FLOG_OPEN(ptrSubSystem, ptrComponent);
	__FLOG_CLEAR;
	__FLOG_0(KTestMessage);
	__FLOG_CLOSE;
	return KErrNone; 
	}


/**
* Function  Name		: DoTestCheckWriteL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function checks whether test data was written
						  in to the log file or not.
					  
*/


TInt CFloggerTest027_04::DoTestCheckWriteL()
	{
	User::After(KTimeToLog);
	RFile theFile;
	HBufC8 * hBuffer;
	TInt listfilesize;
	TInt returnCode;
	RFs fileSystem; //For file operation create a file system	
	_LIT(KLogFile,"c:\\logs\\log.txt"); // the log file path

	_LIT8(KTestMessage,"TC 27_04: The is Test for macros");
	
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
	
	returnCode = ptrString.Find(KTestMessage); //find the test descriptor in the buffer read
									//from the file
	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy(); // For theFile object
	if (returnCode > 0)
		return KErrNone;
	else 
		return KErrGeneral;
	}



/**
* Function  Name : CFloggerTest027_05
* Input parameters : None
* Output parameters : None
* Description : This is the constructor
*/


CFloggerTest027_05::CFloggerTest027_05()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_027_05"));
	}


/**
* Function  Name :~ CFloggerTest027_05
* Input parameters : None
* Output parameters : None
* Description : This is the Destructor
*/ 


CFloggerTest027_05::~CFloggerTest027_05()
	{
	}


/**
* Function  Name	: doTestStepL
* Input parameters	: None
* Output parameters : TVerdict 
* Description		: This function returns whether the test case 027_05 has 
* 					  passed or failed

*/


TVerdict CFloggerTest027_05::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 027.05 called "));
	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	
	return TestStepResult();
	}
/**
* Function  Name		: executeStepL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function calls the flogger write and checks whether the log message is written in to the log file

*/


TInt CFloggerTest027_05::executeStepL(TBool)
	{
	return KErrGeneral;
	}

/**
* Function  Name		: executeStepL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function calls the flogger write and checks whether the log message is written in to the log file

*/


TInt CFloggerTest027_05::executeStepL()
	{
	TInt ret = KErrNone;
	ret = DoTestWrite();
	if ( ret == KErrNone)
		{
		TRAPD(r, ret = DoTestCheckWriteL());
		if ( r != KErrNone)
			ret = r;
		}
	return ret;
	}


TInt CFloggerTest027_05::DoTestWrite()
	{
	_LIT8(KTestMessageOne,"TC 27_05: The value of test integer variable :%d");
	TPtrC8 ptrSubSystem;
	TPtrC8 ptrComponent;
	ptrSubSystem.Set(_L8("SubSystem"));
	ptrComponent.Set(_L8("Component"));
	__FLOG_DECLARATION_MEMBER;
	__FLOG_OPEN(ptrSubSystem, ptrComponent);
	__FLOG_CLEAR;
	__FLOG_1(KTestMessageOne, 100);
	__FLOG_CLOSE;
	return KErrNone; 
	}


/**
* Function  Name		: DoTestCheckWriteL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function checks whether test data was written
						  in to the log file or not.
					  
*/


TInt CFloggerTest027_05::DoTestCheckWriteL()
	{
	User::After(KTimeToLog);
	RFile theFile;
	HBufC8 * hBuffer;
	TInt listfilesize;
	TInt returnCode;
	RFs fileSystem; //For file operation create a file system	
	_LIT(KLogFile,"c:\\logs\\log.txt"); // the log file path

	TBuf8<256> testData; //To hold the test descriptor

	_LIT8(KTestMessageOne,"TC 27_05: The value of test integer variable :%d");
	testData.Format(KTestMessageOne,100);
	
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
	if (returnCode == KErrNone) 
	returnCode = ptrString.Find(testData); //find the test descriptor in the buffer read
											//from the file

	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy(); // For theFile object
	if (returnCode > 0)
		return KErrNone;
	else 
		return KErrGeneral;
	}



/**
* Function  Name : CFloggerTest027_06
* Input parameters : None
* Output parameters : None
* Description : This is the constructor
*/


CFloggerTest027_06::CFloggerTest027_06()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_027_06"));
	}


/**
* Function  Name :~ CFloggerTest027_06
* Input parameters : None
* Output parameters : None
* Description : This is the Destructor
*/ 


CFloggerTest027_06::~CFloggerTest027_06()
	{
	}


/**
* Function  Name	: doTestStepL
* Input parameters	: None
* Output parameters : TVerdict 
* Description		: This function returns whether the test case 027_06 has 
* 					  passed or failed

*/


TVerdict CFloggerTest027_06::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 027.06 called "));
	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	
	return TestStepResult();
	}


/**
* Function  Name		: executeStepL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function calls the flogger write and checks whether the log message is written in to the log file

*/


TInt CFloggerTest027_06::executeStepL()
	{
	TInt ret = KErrNone;
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
* Function  Name		: executeStepL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function calls the flogger write and checks whether the log message is written in to the log file

*/


TInt CFloggerTest027_06::executeStepL(TBool)
	{
	return KErrGeneral;
	}

TInt CFloggerTest027_06::DoTestWrite()
	{
	TPtrC8 ptrSubSystem;
	TPtrC8 ptrComponent;
	ptrSubSystem.Set(_L8("SubSystem"));
	ptrComponent.Set(_L8("Component"));
	_LIT8(descTxt,"TC 27_6:Test Msg");

	__FLOG_DECLARATION_MEMBER;
	__FLOG_OPEN(ptrSubSystem, ptrComponent);
	__FLOG_CLEAR;
	__FLOG_HEXDUMP ( (descTxt) );
	__FLOG_CLOSE;
	return KErrNone; 
	}


/**
* Function  Name		: DoTestCheckWriteL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function checks whether test data was written
						  in to the log file or not.
					  
*/


TInt CFloggerTest027_06::DoTestCheckWriteL()
	{
	User::After(KTimeToLog);
	RFile theFile;
	HBufC8 * hBuffer;
	TInt listfilesize;
	TInt returnCode;
	RFs fileSystem; //For file operation create a file system	
	_LIT(KLogFile,"c:\\logs\\log.txt"); // the log file path
		
	_LIT8(KBodyTxt,"TC 27_6:Test Msg");//  Test body descriptor 

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
	if (returnCode == KErrNone) 
		returnCode = ptrString.Find(KBodyTxt);
			
	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy();	//theFile
	if (returnCode > 0)
		return KErrNone;
	else 
		return KErrGeneral;
	}



/**
* Function  Name : CFloggerTest027_07
* Input parameters : None
* Output parameters : None
* Description : This is the constructor
*/


CFloggerTest027_07::CFloggerTest027_07()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_027_07"));
	}


/**
* Function  Name :~ CFloggerTest027_07
* Input parameters : None
* Output parameters : None
* Description : This is the Destructor
*/ 


CFloggerTest027_07::~CFloggerTest027_07()
	{
	}


/**
* Function  Name	: doTestStepL
* Input parameters	: None
* Output parameters : TVerdict 
* Description		: This function returns whether the test case 027_07 has 
* 					  passed or failed

*/


TVerdict CFloggerTest027_07::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 027.07 called "));
	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	
	return TestStepResult();
	}


/**
* Function  Name		: executeStepL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function calls the flogger write and checks whether the log message is written in to the log file

*/


TInt CFloggerTest027_07::executeStepL()
	{
	TInt ret = KErrNone;
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
* Function  Name		: executeStepL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function calls the flogger write and checks whether the log message is written in to the log file

*/


TInt CFloggerTest027_07::executeStepL(TBool)
	{
	return KErrGeneral;
	}

TInt CFloggerTest027_07::DoTestWrite()
	{
	TPtrC8 ptrSubSystem;
	TPtrC8 ptrComponent;
	ptrSubSystem.Set(_L8("SubSystem"));
	ptrComponent.Set(_L8("Component"));
	__FLOG_DECLARATION_MEMBER;
	__FLOG_OPEN(ptrSubSystem, ptrComponent);
	__FLOG_CLEAR;
	_LIT8(KTestMessage ,"TC no 27.7: This is test message");
	__FLOG_BINARY(KTestMessage);  //Write 8 bit test descriptor
	__FLOG_CLOSE;
	return KErrNone; 
	}


/**
* Function  Name		: DoTestCheckWriteL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function checks whether test data was written
						  in to the log file or not.
					  
*/


TInt CFloggerTest027_07::DoTestCheckWriteL()
	{
	User::After(KTimeToLog);
	RFile theFile;
	HBufC8 * hBuffer;
	TInt listfilesize;
	TInt returnCode;
	RFs fileSystem; //For file operation create a file system	
	_LIT(KLogFile,"c:\\logs\\log.txt"); // the log file path

	_LIT8(KTestMessage ,"TC no 27.7: This is test message");
	
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
	
	returnCode = ptrString.Find(KTestMessage); //find the test descriptor in the buffer read
												//from the file

	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy(); // For theFile object
	if (returnCode > 0)
		return KErrNone;
	else 
		return KErrGeneral;
	}




/**
* Function  Name : CFloggerTest027_08
* Input parameters : None
* Output parameters : None
* Description : This is the constructor
*/


CFloggerTest027_08::CFloggerTest027_08()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_027_08"));
	}


/**
* Function  Name :~ CFloggerTest027_08
* Input parameters : None
* Output parameters : None
* Description : This is the Destructor
*/ 


CFloggerTest027_08::~CFloggerTest027_08()
	{
	}


/**
* Function  Name	: doTestStepL
* Input parameters	: None
* Output parameters : TVerdict 
* Description		: This function returns whether the test case 027_07 has 
* 					  passed or failed

*/


TVerdict CFloggerTest027_08::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 027.08 called "));
	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	
	return TestStepResult();
	}

/**
* Function  Name		: executeStepL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function calls the flogger write and checks whether the log message is written in to the log file
*/


TInt CFloggerTest027_08::executeStepL(TBool)
	{
	return KErrGeneral;
	}

/**
* Function  Name		: executeStepL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function calls the flogger write and checks whether the log message is written in to the log file
*/


TInt CFloggerTest027_08::executeStepL()
	{
	TInt ret = KErrNone;
	ret = DoTestWrite();
	if ( ret == KErrNone)
		{
		TRAPD(r, ret = DoTestCheckWriteL());
		if ( r != KErrNone)
			ret = r;
		}
	return ret;
	}


TInt CFloggerTest027_08::DoTestWrite()
	{
	_LIT8(ptrSubSystem,"SubSystem");
	_LIT8(ptrComponent,"Component");
	_LIT8(KTestMessage ,"TC no 27.8: This is test message");
	__FLOG_STATIC0(ptrSubSystem, ptrComponent, KTestMessage);
	return KErrNone; 
	}


/**
* Function  Name		: DoTestCheckWriteL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function checks whether test data was written
						  in to the log file or not.
					  
*/


TInt CFloggerTest027_08::DoTestCheckWriteL()
	{
	User::After(KTimeToLog);
	RFile theFile;
	HBufC8 * hBuffer;
	TInt listfilesize;
	TInt returnCode;
	RFs fileSystem; //For file operation create a file system	
	_LIT(KLogFile,"c:\\logs\\log.txt"); // the log file path

	_LIT8(KTestMessage ,"TC no 27.8: This is test message");
	
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
	
	returnCode = ptrString.Find(KTestMessage); //find the test descriptor in the buffer read
												//from the file

	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy(); // For theFile object
	if (returnCode > 0)
		return KErrNone;
	else 
		return KErrGeneral;
	}




/**
* Function  Name : CFloggerTest027_09
* Input parameters : None
* Output parameters : None
* Description : This is the constructor
*/


CFloggerTest027_09::CFloggerTest027_09()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_027_09"));
	}


/**
* Function  Name :~ CFloggerTest027_09
* Input parameters : None
* Output parameters : None
* Description : This is the Destructor
*/ 


CFloggerTest027_09::~CFloggerTest027_09()
	{
	}


/**
* Function  Name	: doTestStepL
* Input parameters	: None
* Output parameters : TVerdict 
* Description		: This function returns whether the test case 027_09 has 
* 					  passed or failed

*/


TVerdict CFloggerTest027_09::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 027.09 called "));
	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	
	return TestStepResult();
	}
/**
* Function  Name		: executeStepL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function calls the flogger write and checks whether the log message is written in to the log file

*/


TInt CFloggerTest027_09::executeStepL(TBool)
	{
	return KErrGeneral;
	}

/**
* Function  Name		: executeStepL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function calls the flogger write and checks whether the log message is written in to the log file

*/


TInt CFloggerTest027_09::executeStepL()
	{
	TInt ret = KErrNone;
	ret = DoTestWrite();
	if ( ret == KErrNone)
		{
		TRAPD(r, ret = DoTestCheckWriteL());
		if ( r != KErrNone)
			ret = r;
		}
	return ret;
	}


TInt CFloggerTest027_09::DoTestWrite()
	{
	_LIT8(ptrSubSystem,"SubSystem");
	_LIT8(ptrComponent,"Component");
	_LIT8(KTestMessageOne,"TEST 27.09: The value of test integer variable :%d");
	__FLOG_STATIC1(ptrSubSystem, ptrComponent, KTestMessageOne, 100);
	return KErrNone; 
	}


/**
* Function  Name		: DoTestCheckWriteL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function checks whether test data was written
						  in to the log file or not.
					  
*/


TInt CFloggerTest027_09::DoTestCheckWriteL()
	{
	User::After(KTimeToLog);
	RFile theFile;
	HBufC8 * hBuffer;
	TInt listfilesize;
	TInt returnCode;
	RFs fileSystem; //For file operation create a file system	
	_LIT(KLogFile,"c:\\logs\\log.txt"); // the log file path

	TBuf8<256> testData; //To hold the test descriptor

	_LIT8(KTestMessageOne,"TEST 27.09: The value of test integer variable :%d");
	testData.Format(KTestMessageOne,100);
	
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

	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy(); // For theFile object
	if (returnCode > 0)
		return KErrNone;
	else 
		return KErrGeneral;
	}




/**
* Function  Name : CFloggerTest027_10
* Input parameters : None
* Output parameters : None
* Description : This is the constructor
*/


CFloggerTest027_10::CFloggerTest027_10()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_027_10"));
	}


/**
* Function  Name :~ CFloggerTest027_10
* Input parameters : None
* Output parameters : None
* Description : This is the Destructor
*/ 


CFloggerTest027_10::~CFloggerTest027_10()
	{
	}


/**
* Function  Name	: doTestStepL
* Input parameters	: None
* Output parameters : TVerdict 
* Description		: This function returns whether the test case 027_10 has 
* 					  passed or failed

*/


TVerdict CFloggerTest027_10::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 027.10 called "));
	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	
	return TestStepResult();
	}

/**
* Function  Name		: executeStepL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function calls the flogger write and checks whether the log message is written in to the log file

*/


TInt CFloggerTest027_10::executeStepL(TBool)
	{
	return KErrGeneral;
	}
/**
* Function  Name		: executeStepL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function calls the flogger write and checks whether the log message is written in to the log file

*/


TInt CFloggerTest027_10::executeStepL()
	{
	TInt ret = KErrNone;
	ret = DoTestWrite();
	if ( ret == KErrNone)
		{
		TRAPD(r, ret = DoTestCheckWriteL());
		if ( r != KErrNone)
			ret = r;
		}
	return ret;
	}


TInt CFloggerTest027_10::DoTestWrite()
	{
	_LIT8(ptrSubSystem,"SubSystem");
	_LIT8(ptrComponent,"Component");
	_LIT8(KTestMessage ,"TC no 27.10: This is test message");
	__FLOG_STATIC(ptrSubSystem, ptrComponent, KTestMessage);
	return KErrNone; 
	}


/**
* Function  Name		: DoTestCheckWriteL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function checks whether test data was written
						  in to the log file or not..
					  
*/


TInt CFloggerTest027_10::DoTestCheckWriteL()
	{
	User::After(KTimeToLog);
	RFile theFile;
	HBufC8 * hBuffer;
	TInt listfilesize;
	TInt returnCode;
	RFs fileSystem; //For file operation create a file system	
	_LIT(KLogFile,"c:\\logs\\log.txt"); // the log file path
	_LIT8(KTestMessage ,"TC no 27.10: This is test message");
	
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
	
	returnCode = ptrString.Find(KTestMessage); //find the test descriptor in the buffer read
												//from the file

	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy(); // For theFile object
	if (returnCode > 0)
		return KErrNone;
	else 
		return KErrGeneral;
	}




/**
* Function  Name : CFloggerTest027_11
* Input parameters : None
* Output parameters : None
* Description : This is the constructor
*/


CFloggerTest027_11::CFloggerTest027_11()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_027_11"));
	}


/**
* Function  Name :~ CFloggerTest027_11
* Input parameters : None
* Output parameters : None
* Description : This is the Destructor
*/ 


CFloggerTest027_11::~CFloggerTest027_11()
	{
	}


/**
* Function  Name	: doTestStepL
* Input parameters	: None
* Output parameters : TVerdict 
* Description		: This function returns whether the test case 027_11 has 
* 					  passed or failed

*/


TVerdict CFloggerTest027_11::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 027.11 called "));
	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	
	return TestStepResult();
	}

/**
* Function  Name		: executeStepL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function calls the flogger write and checks whether the log message is written in to the log file

*/


TInt CFloggerTest027_11::executeStepL(TBool)
	{
	return KErrGeneral;
	}

/**
* Function  Name		: executeStepL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function calls the flogger write and checks whether the log message is written in to the log file

*/


TInt CFloggerTest027_11::executeStepL()
	{
	TInt ret = KErrNone;
	ret = DoTestWrite();
	if ( ret == KErrNone)
		{
		TRAPD(r, ret = DoTestCheckWriteL());
		if ( r != KErrNone)
			ret = r;
		}
	return ret;
	}


TInt CFloggerTest027_11::DoTestWrite()
	{
	TPtrC8 ptrSubSystem;
	TPtrC8 ptrComponent;
	ptrSubSystem.Set(_L8("SubSystem"));
	ptrComponent.Set(_L8("Component"));
	_LIT8(descTxt,"T 27_11:Test Msg"); //Test body descriptor
	__FLOG_STATIC_HEXDUMP((ptrSubSystem, ptrComponent, descTxt));
	return KErrNone;
	}


/**
* Function  Name		: DoTestCheckWriteL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function checks whether test data was written
						  in to the log file or not.
					  
*/


TInt CFloggerTest027_11::DoTestCheckWriteL()
	{
	User::After(KTimeToLog);
	RFile theFile;
	HBufC8 * hBuffer;
	TInt listfilesize;
	TInt returnCode;
	RFs fileSystem; //For file operation create a file system	
	_LIT(KLogFile,"c:\\logs\\log.txt"); // the log file path
	_LIT8(KTestMessage ,"T 27_11:Test Msg");
	
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
	
	returnCode = ptrString.Find(KTestMessage); //find the test descriptor in the buffer read
												//from the file

	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy(); // For theFile object
	if (returnCode > 0)
		return KErrNone;
	else 
		return KErrGeneral;
	}




/**
* Function  Name : CFloggerTest027_12
* Input parameters : None
* Output parameters : None
* Description : This is the constructor
*/


CFloggerTest027_12::CFloggerTest027_12()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_027_12"));
	}


/**
* Function  Name :~ CFloggerTest027_12
* Input parameters : None
* Output parameters : None
* Description : This is the Destructor
*/ 


CFloggerTest027_12::~CFloggerTest027_12()
	{
	}


/**
* Function  Name	: doTestStepL
* Input parameters	: None
* Output parameters : TVerdict 
* Description		: This function returns whether the test case 027_12 has 
* 					  passed or failed

*/


TVerdict CFloggerTest027_12::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 027.12 called "));
	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	
	return TestStepResult();
	}

/**
* Function  Name		: executeStepL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function calls the flogger write and checks whether the log message is written in to the log file
*/


TInt CFloggerTest027_12::executeStepL(TBool)
	{
	return KErrGeneral;
	}

/**
* Function  Name		: executeStepL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function calls the flogger write and checks whether the log message is written in to the log file
*/


TInt CFloggerTest027_12::executeStepL()
	{
	TInt ret = KErrNone;
	ret = DoTestWrite();
	if ( ret == KErrNone)
		{
		TRAPD(r, ret = DoTestCheckWriteL());
		if ( r != KErrNone)
			ret = r;
		}
	return ret;
	}


TInt CFloggerTest027_12::DoTestWrite()
	{
	TPtrC8 ptrSubSystem;
	TPtrC8 ptrComponent;
	ptrSubSystem.Set(_L8("SubSystem"));
	ptrComponent.Set(_L8("Component"));
	_LIT16(KTestMessageTwo,"TEST 27.12: The value of first test integer variable :%d The value of second test integer variable : %d");
	__FLOG_STATIC2(ptrSubSystem, ptrComponent, KTestMessageTwo, 100, 200);
	return KErrNone;
	}


/**
* Function  Name		: DoTestCheckWriteL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function checks whether test data was written
						  in to the log file or not.
					  
*/


TInt CFloggerTest027_12::DoTestCheckWriteL()
	{
	User::After(KTimeToLog);
	RFile theFile;
	HBufC8 * hBuffer;
	TInt listfilesize;
	TInt returnCode;
	RFs fileSystem; //For file operation create a file system	
	_LIT(KLogFile,"c:\\logs\\log.txt"); // the log file path

	TBuf8<256> testData; //To hold the test descriptor

	_LIT8(KTestMessageTwo,"TEST 27.12: The value of first test integer variable :%d The value of second test integer variable : %d");
	testData.Format(KTestMessageTwo,100, 200);

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

	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy(); // For theFile object
	if (returnCode > 0)
		return KErrNone;
	else 
		return KErrGeneral;
	}


