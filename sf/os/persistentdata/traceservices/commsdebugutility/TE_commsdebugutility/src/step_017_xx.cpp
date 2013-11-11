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
// This contains Flogger Unit Test Case 017.xx

// EPOC includes
#include <e32base.h>

// Test system includes
#include <comms-infras/commsdebugutility.h>
#include "TestMessage.h"
#include <f32file.h>


//User includes
#include "teststepcomsdbg.h"
#include "step_017_xx.h"


/**
* Function  Name	: CFloggerTest017_01
* Input parameters	: None
* Output parameters : None
* Description		: This is the constructor
*/ 


CFloggerTest017_01::CFloggerTest017_01()
	{
	// store the name of this test case
	SetTestStepName(_L("step_017_01"));
	}


/**
* Function  Name	:~ CFloggerTest017_01
* Input parameters	: None
* Output parameters : None
* Description		: This is the Destructor
*/ 


CFloggerTest017_01::~CFloggerTest017_01()
	{
	}	


/**
* Function  Name	: doTestStepL
* Input parameters	: None
* Output parameters : TVerdict 
* Description		: This function returns weather the test case 017_01 has 
* 					  passed or failed

*/


TVerdict CFloggerTest017_01::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 017.01 called "));
	
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


TInt CFloggerTest017_01::executeStepL()
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


TInt CFloggerTest017_01::executeStepL(TBool heapTest)
	{
	TInt ret = KErrGeneral;
	
	ret = DoTestWrite();
	if (ret == KErrNone)
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
* Function  Name		: DoTestWrite
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function writes the data to the file logger in hexa format
 
*/


TInt CFloggerTest017_01::DoTestWrite()
	{
	_LIT8(KDescText, "TC 17_1:Test Msg----"); //Test body descriptor
		
	//Write the test datas in the hexa format
	RFileLogger::HexDump(KStdSubsysTag8,KStdCompTag8, KDescText, KHexTestHeader);

	// connect so we can flush file buffer for heap check since timer is killed in heap checks
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
* 						  in to the log file by DoTestWrite() or not.
					  
*/


TInt CFloggerTest017_01::DoTestCheckWriteL()
	{
	User::After(KTimeToLog);

	RFile theFile;
	HBufC8 * hBuffer;
	TInt listfilesize;
	TInt returnCode;
	RFs fileSystem; //For file operation create a file system	
	_LIT(KLogFile,"c:\\logs\\log.txt"); // the log file path

	_LIT8(KBodyTxt,"TC 17_1:Test Msg");//  First 16 chars of Test body descriptor 

	User::LeaveIfError(fileSystem.Connect());

	//Open the file in the read mode
	User::LeaveIfError(theFile.Open(fileSystem,KLogFile,EFileRead)) ;

	
	User::LeaveIfError(returnCode = theFile.Size(listfilesize)); //Size of the file
	hBuffer = HBufC8::New(listfilesize); //Allocate the buffer
	CleanupStack::PushL(hBuffer);

	TPtr8 ptrString = hBuffer->Des();  ; //To hold the buffer

	// Read from position 0, i.e starting of file
	returnCode = theFile.Read(ptrString);

	returnCode = ptrString.Find(KHexTestHeader); //find the test  header descriptor in
										 //the buffer read from the file

	if (returnCode > 0) //The header is present
		{
		returnCode = ptrString.Find(KHexTestMargin); //find the test margin descriptor in
										 //the buffer read from the file
		}

	if (returnCode > 0) //The margin is present
		{
		returnCode = ptrString.Find(KBodyTxt); //find the test descriptor in the 
										 //buffer read from the file
		}

	theFile.Close();
	fileSystem.Close(); //Close the file server
	CleanupStack::PopAndDestroy(hBuffer);

	if (returnCode > 0) //The test body descriptor is present
		return KErrNone;
	else 
		return KErrNotFound;
	}





/**
* Function  Name	: CFloggerTest017_02
* Input parameters	: None
* Output parameters : None
* Description		: This is the constructor
*/ 


CFloggerTest017_02::CFloggerTest017_02()
	{
	// store the name of this test case
	SetTestStepName(_L("step_017_02"));
	}


/**
* Function  Name	:~ CFloggerTest017_02
* Input parameters	: None
* Output parameters : None
* Description		: This is the Destructor
*/


CFloggerTest017_02::~CFloggerTest017_02()
	{		
	}


/**
* Function  Name	: doTestStepL
* Input parameters	: None
* Output parameters : TVerdict 
* Description		: This function returns weather the test case 017_02 has 
* 					  passed or failed

*/


TVerdict CFloggerTest017_02::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 017.02 called "));
	
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



TInt CFloggerTest017_02::executeStepL(TBool)
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



TInt CFloggerTest017_02::executeStepL( )
	{
	User::After(1000*1000);	//Need this or connect will return KErrServerTerminated
	TInt ret = KErrGeneral;
	
	ret = DoTestWrite();
	if (ret == KErrNone )
		{
		TRAPD(r, ret = DoTestCheckWriteL());
		if (r != KErrNone)
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


TInt CFloggerTest017_02::DoTestWrite()
	{

	RFileLogger theFlogger;
	TInt ret = KErrNone;

	ret = theFlogger.Connect(); //Just to clear the old log message
	if (ret == KErrNone)
		ret = theFlogger.SetLogTags(KStdSubsysTag8,KStdCompTag8);
		if (ret == KErrNone)
			ret = theFlogger.ClearLog();
			if (ret == KErrNone)
				theFlogger.Close();

	if (ret == KErrNone)
		{
		_LIT8(KDescText,"TC 17_2:Test Msg");

		//Write the test datas in the hexa format
		RFileLogger::HexDump(KStdSubsysTag8,KStdCompTag8, KDescText, KEightSpaces8);
			
		return KErrNone;
		}
		return ret;
	}


/**
* Function  Name		: DoTestCheckWriteL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function checks the weather test data was written
* 						  in to the log file by DoTestWrite() or not.
					  
*/


TInt CFloggerTest017_02::DoTestCheckWriteL()
	{
	User::After(KTimeToLog);
	RFile theFile;
	HBufC8 * hBuffer;
	TInt listfilesize;
	TInt returnCode;
	RFs fileSystem; //For file operation create a file system	
	TBuf8<256> testData; //To hold the test descriptor
	_LIT(KLogFile,"c:\\logs\\log.txt"); // the log file path

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
	returnCode = theFile.Read(ptrString);
	
	testData.FillZ();
	testData.Copy(KEightSpaces8); //Copy the test descriptor 
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
* Function  Name	: CFloggerTest017_03
* Input parameters	: None
* Output parameters : None
* Description		: This is the constructor
* Description		: This is the constructor
*/


CFloggerTest017_03::CFloggerTest017_03()
	{
	// store the name of this test case
	SetTestStepName(_L("step_017_03"));
	}


/**
* Function  Name	:~ CFloggerTest017_03
* Input parameters	: None
* Output parameters : None
* Description		: This is the Destructor
*/


CFloggerTest017_03::~CFloggerTest017_03()
	{
	}


/**
* Function  Name	: doTestStepL
* Input parameters	: None
* Output parameters : TVerdict 
* Description		: This function returns weather the test case 017_03 has 
* 					  passed or failed

*/


TVerdict CFloggerTest017_03::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 017.03 called "));
	
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


TInt CFloggerTest017_03::executeStepL( )
	{
	User::After(1000*1000);	//Need this or connect will return KErrServerTerminated
	TInt ret = KErrGeneral;
	
	ret = DoTestWrite();
	if (ret == KErrNone )
		{
		TRAPD(r, ret = DoTestCheckWriteL());
		if (ret != KErrNone)
			ret = r;
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


TInt CFloggerTest017_03::executeStepL(TBool)
	{
	return KErrGeneral;
	}

//**
/* Function  Name		: DoTestWrite
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function writes the data to the file logger
 
*/


TInt CFloggerTest017_03::DoTestWrite()
	{
	
	RFileLogger theFlogger;
	TInt ret = KErrNone;
	
	ret = theFlogger.Connect(); //Just to clear the old log message
	if (ret == KErrNone)
		ret = theFlogger.SetLogTags(KStdSubsysTag8,KStdCompTag8);
		if (ret == KErrNone)
			ret = theFlogger.ClearLog();
			if (ret == KErrNone)
				theFlogger.Close();

	if (ret == KErrNone)
		{
		_LIT8(KDescText,"TC 17_3:Test Msg"); //8 bit test descriptor
		
		//Write the test datas in the hexa format
		RFileLogger::HexDump(KStdSubsysTag8,KStdCompTag8, KDescText);
		
		
		return KErrNone;
		}
	return ret;
	}


/**
* Function  Name		: DoTestCheckWriteL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function checks the weather test data was written
* 						  in to the log file by DoTestWrite() or not.
					  
*/


TInt CFloggerTest017_03::DoTestCheckWriteL()
	{

	User::After(KTimeToLog);
	RFile theFile;
	HBufC8 * hBuffer;
	TInt listfilesize;
	TInt returnCode;
	RFs fileSystem; //For file operation create a file system	
	TBuf8<256> testData; //To hold the test descriptor
	_LIT(KLogFile,"c:\\logs\\log.txt"); // the log file path

	_LIT8(KBodyTxt,"TC 17_3:Test Msg");//  Test body descriptor 

	User::LeaveIfError(fileSystem.Connect());
	
	//Open the file in the read mode
	User::LeaveIfError(theFile.Open(fileSystem,KLogFile,EFileRead)); 
	CleanupClosePushL(theFile);
	
	User::LeaveIfError(returnCode = theFile.Size(listfilesize)); //Size of the file
	hBuffer = HBufC8::New(listfilesize); //Allocate the buffer
	CleanupStack::PushL(hBuffer);
	TPtr8 ptrString = hBuffer->Des();  ; //To hold the buffer

	// Read from position 0: start of file
	returnCode=theFile.Read(ptrString);
		
	testData.Copy(KBodyTxt); //Copy the test descriptor 
	returnCode=ptrString.Find(testData); //find the test descriptor in the buffer read
										 //from the file
										 
	if (returnCode == KErrNotFound)  //Find the error message
		{
		returnCode = ptrString.Find(KErrOOM);
		if (returnCode > 0)
			User::Leave(KErrNoMemory);
		}
	
	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy();	//theFile object
	if (returnCode > 0)
		return KErrNone;
	else 
		return KErrGeneral;
	}



/**
* Function  Name	: CFloggerTest017_04
* Input parameters	: None
* Output parameters : None
* Description		: This is the constructor
*/


CFloggerTest017_04::CFloggerTest017_04()
	{
	// store the name of this test case
	SetTestStepName(_L("step_017_04"));
	}


/**
* Function  Name	:~ CFloggerTest017_04
* Input parameters	: None
* Output parameters : None
* Description		: This is the Destructor
*/


CFloggerTest017_04::~CFloggerTest017_04()
	{
	}

/**
* Function  Name	: doTestStepL
* Input parameters	: None
* Output parameters : TVerdict 
* Description		: This function returns weather the test case 017_04 has 
* 					  passed or failed

*/


TVerdict CFloggerTest017_04::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 017.04 called "));
	
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


TInt CFloggerTest017_04::executeStepL( )
	{
	User::After(1000*1000);	//Need this or connect will return KErrServerTerminated
	TInt ret = KErrGeneral;
	
	ret = DoTestWrite();
	if (ret == KErrNone )
		{
		TRAPD(r, ret = DoTestCheckWriteL());
		if (ret != KErrNone)
			ret = r;
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


TInt CFloggerTest017_04::executeStepL(TBool)
	{
	return KErrGeneral;
	}

/**
* Function  Name		: DoTestWrite
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function writes the data to the file logger
 
*/


TInt CFloggerTest017_04::DoTestWrite()
	{
	_LIT8(KDescText,"TC 17_4:Test Msg");
	
	RFileLogger theFlogger;
	TInt ret = KErrNone;
	ret = theFlogger.Connect(); //Just to clear the old log message
	if (ret == KErrNone)
		ret = theFlogger.SetLogTags(KStdSubsysTag8,KStdCompTag8);
		if (ret == KErrNone)
			ret = theFlogger.ClearLog();
			if (ret == KErrNone)
				theFlogger.Close();
	if (ret == KErrNone)
		{
		//Write the test datas in the hexa format
		RFileLogger::HexDump(KStdSubsysTag8,KStdCompTag8, KDescText, KNullDesC8);

		return KErrNone;
		}
	return ret;
	}


/**
* Function  Name		: DoTestCheckWriteL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function checks the weather test data was written
* 						  in to the log file by DoTestWrite() or not.
					  
*/


TInt CFloggerTest017_04::DoTestCheckWriteL()
	{
	User::After(KTimeToLog);
	RFile theFile;
	HBufC8 * hBuffer;
	TInt listfilesize;
	TInt returnCode;
	RFs fileSystem; //For file operation create a file system	
	TBuf8<256> testData; //To hold the test descriptor
	_LIT(KLogFile,"c:\\logs\\log.txt"); // the log file path

	_LIT(KBodyTxt,"TC 17_4:Test Msg");//  Test body descriptor 
	TUint numSuccessful = 0;

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
	

	testData.Copy(KBodyTxt); //Copy the test descriptor 
	returnCode=ptrString.Find(testData); //find the test descriptor in the buffer read
									 //from the file
									 
	if (returnCode > 0)
		numSuccessful++;
		
	returnCode=ptrString.Find(KHexTestHeader); //find the test descriptor in the buffer read
		
	if (returnCode == KErrNotFound)
		numSuccessful++;


	if (returnCode == KErrNotFound)  //Find the error message
		{
		returnCode = ptrString.Find(KErrOOM);
		if (returnCode > 0)
			User::Leave(KErrNoMemory);
		}

	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy();	//theFile object

	if (numSuccessful == 2)
		return KErrNone;
	else 
		return KErrGeneral;
	}




/**
* Function  Name	: CFloggerTest017_05
* Input parameters	: None
* Output parameters : None
* Description		: This is the constructor
*/ 


CFloggerTest017_05::CFloggerTest017_05()
	{
	// store the name of this test case
	SetTestStepName(_L("step_017_05"));
	}


/**
* Function  Name	:~ CFloggerTest017_05
* Input parameters	: None
* Output parameters : None
* Description		: This is the Destructor
*/ 


CFloggerTest017_05::~CFloggerTest017_05()
	{
	}

/**
* Function  Name	: doTestStepL
* Input parameters	: None
* Output parameters : TVerdict 
* Description		: This function returns weather the test case 017_05 has 
* 					  passed or failed

*/


TVerdict CFloggerTest017_05::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 017.05 called "));
	
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


TInt CFloggerTest017_05::executeStepL(TBool)
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


TInt CFloggerTest017_05::executeStepL( )
	{
	User::After(KTimeToLog);	//Need this or connect will return KErrServerTerminated
	TInt ret = KErrGeneral;
	
	ret = DoTestWrite();
	if (ret == KErrNone )
		{
		TRAPD(r, ret = DoTestCheckWriteL());
		if (ret != KErrNone)
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


TInt CFloggerTest017_05::DoTestWrite()
	{	
	RFileLogger theFlogger;
	TInt ret = KErrNone;
	
	ret = theFlogger.Connect(); //Just to clear the old log message
	if (ret == KErrNone)
		ret = theFlogger.SetLogTags(KStdSubsysTag8,KStdCompTag8);
		if (ret == KErrNone)
			ret = theFlogger.ClearLog();
			if (ret == KErrNone)
				theFlogger.Close();

	if (ret == KErrNone)
		{
		//Write the test datas in the hexa format
		RFileLogger::HexDump(KStdSubsysTag8,KStdCompTag8, KNullDesC8);
		return KErrNone;
		}
	return ret;
	}


/**
* Function  Name		: DoTestCheckWriteL
* Input parameters		: None
* Output parameters		: TInt 
* Description 			: This function checks the weather test data was written
* 						  in to the log file by DoTestWrite() or not.
					  
*/


TInt CFloggerTest017_05::DoTestCheckWriteL()
	{
	User::After(KTimeToLog);
	RFile theFile;
	HBufC8 * hBuffer;
	TInt listfilesize;
	TInt returnCode;
	RFs fileSystem; //For file operation create a file system	
	_LIT(KLogFile,"c:\\logs\\log.txt"); // the log file path
	_LIT8(KExpectedString,"0000 :");

	_LIT8(KOOMError, "#Logs may be lost out of memory!!"); //Error message

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
	
	//check for no OOM msg
	returnCode = ptrString.Find(KOOMError);
	if (returnCode > 0)
		{
		User::Leave(KErrNoMemory);
		}
	
	
	// we expect the output to be pretty blank, so not much to search for
	// except make sure the standard "0000: " ins't even there
	returnCode = ptrString.Find(KExpectedString); //find the test descriptor in the buffer read
										 //from the file
	
	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy();	//theFile object
	if (returnCode == KErrNotFound)
		return KErrNone;
	else 
		return KErrGeneral;
	}



/*
* Function  Name	: CFloggerTest017_06
* Input parameters	: None
* Output parameters : None
* Description		: This is the constructor
*/


CFloggerTest017_06::CFloggerTest017_06()
	{
	// store the name of this test case
	SetTestStepName(_L("step_017_06"));
	}


/**
* Function  Name	:~ CFloggerTest017_06
* Input parameters	: None
* Output parameters : None
* Description		: This is the Destructor
*/


CFloggerTest017_06::~CFloggerTest017_06()
	{
	}	


/**
* Function  Name : doTestStepL
* Input parameters : None
* Output parameters : TVerdict 
* Description : This function is responsible for doing the heap test analysis 

*/



TVerdict CFloggerTest017_06::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 017.06 called "));

	CFloggerTest017_01* step017_01 = new CFloggerTest017_01;
	CleanupStack::PushL(step017_01);
	doTestStepWithHeapFailureL( *step017_01, 1, 20, KErrNone, ETrue);

	CleanupStack::PopAndDestroy(step017_01);
	User::After(KTimeForDisplay);
	return TestStepResult();
	}

