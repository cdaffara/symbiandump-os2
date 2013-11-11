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
// This contains Flogger Unit Test Case 011.01

// EPOC includes
#include <e32base.h>

// Test system includes
#include "teststepcomsdbg.h"
#include "TestMessage.h"
#include "step_011_xx.h"

#include <comms-infras/commsdebugutility.h>


//*****************************************************************
//Function  Name	: CFloggerTest011_01
//Input parameters	: None
//Output parameters : None
//Description		: This is the constructor
//*****************************************************************

CFloggerTest011_01::CFloggerTest011_01()
	{
	// store the name of this test case
	SetTestStepName(_L("step_011_01"));
	}



//*****************************************************************
//Function  Name	:~ CFloggerTest011_01
//Input parameters	: None
//Output parameters : None
//Description		: This is the Destructor
//*****************************************************************

CFloggerTest011_01::~CFloggerTest011_01()
	{
	}



//*****************************************************************
//Function  Name	: doTestStepL
//Input parameters	: None
//Output parameters : TVerdict 
//Description		: This function returns weather the test case 011_01 has 
//					  passed or failed

//*****************************************************************

TVerdict CFloggerTest011_01::doTestStepL( )
	{

	INFO_PRINTF1(_L("Step 011.01 called "));
	
	if ( executeStepL(EFalse) == KErrNone )
		SetTestStepResult(EPass);
	else
		SetTestStepResult(EFail);
	User::After(KTimeForDisplay);
	return TestStepResult();
	}

//*****************************************************************
//Function  Name		: executeStepL
//Input parameters		: None
//Output parameters		: TInt 
//Description 			: This function writes the test data in to the log file 
//						  This function check the test message is present in the 
//						  log file 

//*****************************************************************


TInt CFloggerTest011_01::executeStepL()
	{
	return KErrGeneral;
	}


//*****************************************************************
//Function  Name		: executeStepL
//Input parameters		: None
//Output parameters		: TInt 
//Description 			: This function writes the test data in to the log file 
//						  This function check the test message is present in the 
//						  log file 

//*****************************************************************


TInt CFloggerTest011_01::executeStepL(TBool heapTest)
	{

	TInt ret = KErrGeneral;
		
	ret = DoTestWrite();
	if (ret == KErrNone )
		{
		User::After(KTimeToLog);
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


//*****************************************************************
//Function  Name		: DoTestWrite
//Input parameters		: None
//Output parameters		: TInt 
//Description 			: This function writes the data to the file logger
 
//*****************************************************************

TInt CFloggerTest011_01::DoTestWrite()
	{
	TInt ret = KErrNone;
	_LIT(KTestMessage,"TC 11_01: This is test messsage"); //unicode test decriptor

	RFileLogger theFlogger;

	//Just to clear the old log message
	ret = theFlogger.Connect(); 
	if ( ret == KErrNone)
		{
		ret = theFlogger.SetLogTags(KStdSubsysTag8, KStdCompTag8);
		if ( ret == KErrNone)
			{
			ret = theFlogger.ClearLog();
			}
		if ( ret == KErrNone)
			{
			theFlogger.Close();
			}
		}

	
	if ( ret == KErrNone)
		{
		// Test case
		RFileLogger::Write(KStdSubsysTag8, KStdCompTag8,KTestMessage);

		// reconnect so we can flush file buffer for heap check
		ret = theFlogger.Connect();
		if (ret == KErrNone)
			{
			theFlogger.SetLogTags(KStdSubsysTag8, KStdCompTag8);
			ForceLogFlush(theFlogger);
			theFlogger.Close();
			}
		}
	return ret;
	
	}


//*****************************************************************
//Function  Name		: DoTestCheckWriteL
//Input parameters		: None
//Output parameters		: TInt 
//Description 			: This function checks the weather test data was written
//						  in to the log file by DoTestWriteL() or not.
					  
//*****************************************************************

TInt CFloggerTest011_01::DoTestCheckWriteL()
	{


	RFile theFile;
	HBufC8 * hBuffer;
	TInt listfilesize;
	TInt returnCode;
	RFs fileSystem; //For file operation create a file system
	TBuf8<256> testData; //To hold the test descriptor
	_LIT(KLogFile, "c:\\logs\\log.txt"); // log file name and path
	
	_LIT(KTestMessage,"TC 11_01: This is test messsage"); //unicode test decriptor

	User::LeaveIfError(fileSystem.Connect());
	
	//Open the file in the read mode
	User::LeaveIfError(theFile.Open(fileSystem,KLogFile,EFileRead)); 
	CleanupClosePushL(theFile);
	
	User::LeaveIfError(returnCode=theFile.Size(listfilesize)); //Size of the file
	hBuffer=HBufC8::New(listfilesize); //Allocate the buffer
	CleanupStack::PushL(hBuffer);
	TPtr8 ptrString = hBuffer->Des();  ; //To hold the buffer

	// Read from position 0: start of file
	returnCode = theFile.Read(ptrString);
	
	testData.Copy(KTestMessage); //Copy the test descriptor
	returnCode = ptrString.Find(testData); //find the test descriptor in the buffer read
									//from the file


	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy();	//theFile
	if (returnCode > 0)
		return KErrNone;
	else 
		return KErrNotFound;
	}



TInt CFloggerTest011_01::DoHeapTestWrite()
	{
	

	_LIT(KTestMessage,"TC 11_03: This is heap test messsage"); //unicode test decriptor
	
	RFileLogger ::Write(KStdSubsysTag8, KStdCompTag8, KTestMessage);
	return KErrNone;
	}


/////////////////////////////////////////////////////////////////////

//*****************************************************************
//Function  Name	: CFloggerTest011_02
//Input parameters	: None
//Output parameters : None
//Description		: This is the constructor
//*****************************************************************



CFloggerTest011_02::CFloggerTest011_02()
	{
	// store the name of this test case
	SetTestStepName(_L("step_011_02"));
	
	}


//*****************************************************************
//Function  Name	:~ CFloggerTest011_02
//Input parameters	: None
//Output parameters : None
//Description		: This is the Destructor
//*****************************************************************


CFloggerTest011_02::~CFloggerTest011_02()
	{
	iFlogger.Close();
	}


//*****************************************************************
//Function  Name	: doTestStepL
//Input parameters	: None
//Output parameters : TVerdict 
//Description		: This function returns weather the test case 004_01 has 
//					  passed or failed

//*****************************************************************

TVerdict CFloggerTest011_02::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 011.02 called "));
			
	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);
	else
		SetTestStepResult(EFail);
	
	User::After(KTimeForDisplay);
	return TestStepResult();
	}

//*****************************************************************
//Function  Name		: executeStepL
//Input parameters		: None
//Output parameters		: TInt 
//Description 			: This function writes the test data in to the log file 
//						  This function check the test message is present in the 
//						  log file 

//*****************************************************************

TInt CFloggerTest011_02::executeStepL(TBool)
	{
	return KErrGeneral;
	}

//*****************************************************************
//Function  Name		: executeStepL
//Input parameters		: None
//Output parameters		: TInt 
//Description 			: This function writes the test data in to the log file 
//						  This function check the test message is present in the 
//						  log file 

//*****************************************************************

TInt CFloggerTest011_02::executeStepL()
	{
	TInt ret = KErrGeneral;
		
	ret = DoTestWrite();
	if (ret == KErrNone )
		{
		TRAPD(r, ret = DoTestCheckWriteL());
		if ( r != KErrNone)
			ret = r;
		}
	return ret;

	}


//*****************************************************************
//Function  Name		: DoTestWrite
//Input parameters		: None
//Output parameters		: TInt 
//Description 			: This function writes the data to the file logger
 
//*****************************************************************

TInt CFloggerTest011_02::DoTestWrite()
	{
	
	TInt ret = KErrNone;

	
	_LIT(KTestLongMessage,"TC 11_02: This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message"); //unicode test decriptor

	RFileLogger theFlogger;

	ret = theFlogger.Connect(); //Just to clear the old log message
	if ( ret == KErrNone)
		ret = theFlogger.SetLogTags(KStdSubsysTag8, KStdCompTag8);
		if ( ret == KErrNone)
			ret = theFlogger.ClearLog();
			if ( ret == KErrNone)
				theFlogger.Close();

	if (ret == KErrNone)
		RFileLogger ::Write(KStdSubsysTag8, KStdCompTag8, KTestLongMessage);
	User::After(100000);

	return KErrNone;

	}


TInt CFloggerTest011_02::DoTestCheckWriteL()
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
	_LIT(KTestLongMessage, "TC 11_02: This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message This is the long message This is the lon");
	
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




//////////////////////////////////////////////////////////////////////////////////

//*****************************************************************
//Function  Name	: CFloggerTest011_03
//Input parameters	: None
//Output parameters : None
//Description		: This is the constructor
//*****************************************************************


CFloggerTest011_03::CFloggerTest011_03()
	{
	// store the name of this test case
	SetTestStepName(_L("step_011_03"));
	}



//*****************************************************************
//Function  Name	:~ CFloggerTest011_03
//Input parameters	: None
//Output parameters : None
//Description		: This is the Destructor
//*****************************************************************


CFloggerTest011_03::~CFloggerTest011_03()
	{
	}





//*****************************************************************
//Function  Name : doTestStepL
//Input parameters : None
//Output parameters : TVerdict 
//Description : This function is responsible for doing the heap test analysis 

//*****************************************************************


TVerdict CFloggerTest011_03::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 011.03 called "));

	CFloggerTest011_01* step011_01 = new CFloggerTest011_01;
	CleanupStack::PushL(step011_01);
	doTestStepWithHeapFailureL( *step011_01, 1, 20, KErrNone, ETrue);

	CleanupStack::PopAndDestroy(step011_01);
	User::After(KTimeForDisplay);
	return TestStepResult();


	}



	

///////////////////////////////////////////////////////////////////////
