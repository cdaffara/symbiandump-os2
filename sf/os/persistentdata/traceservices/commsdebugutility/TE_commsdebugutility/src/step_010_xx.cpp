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
// This contains Flogger Unit Test Case 010.xx

// EPOC includes
#include <e32base.h>

// Test system includes
#include <comms-infras/commsdebugutility.h>
#include "TestMessage.h"
#include <f32file.h>


//User includes
#include "teststepcomsdbg.h"
#include "step_010_xx.h"




CFloggerTest010_01::CFloggerTest010_01()
	{
	// store the name of this test case
	SetTestStepName(_L("step_010_01"));
	}




CFloggerTest010_01::~CFloggerTest010_01()
	{
	}




TVerdict CFloggerTest010_01::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 010.01 called "));
	
	if ( executeStepL(EFalse) == KErrNone )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	
	User::After(KTimeForDisplay);
	return TestStepResult();
	}



TInt CFloggerTest010_01::executeStepL(TBool heapTest)
	{
	User::After(100*1000);	//Need this or connect could return KErrServerTerminated
	TInt ret = KErrGeneral;
	
	ret = DoTestConnect();
	if (ret == KErrNone)
		{
		ret = DoTestWrite();
			if (ret == KErrNone)
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
	iFlogger.Close();
	return ret;
	}

TInt CFloggerTest010_01::executeStepL()
	{
	return KErrGeneral;
	}


TInt CFloggerTest010_01::DoTestWrite()
	{
	_LIT8(KDescTxt,"TC 10_1:Test Msg----"); //8 bit test descriptor

	iFlogger.HexDump(KDescTxt,KHexTestHeader); 
	
	ForceLogFlush(iFlogger);
	
	iFlogger.__DbgShutDownServer();
	iFlogger.Close();
	return KErrNone; 
	}




TInt CFloggerTest010_01::DoTestCheckWriteL()
	{
	User::After(KTimeToLog);

	RFile theFile;
	HBufC8 * hBuffer;
	TInt listfilesize;
	TInt returnCode;
	RFs fileSystem; //For file operation create a file system	
	TBuf8<256> testData; //To hold the test descriptor
	_LIT(KLogFile,"c:\\logs\\log.txt"); // the log file path

	_LIT(KHeaderTxt,"Test Header");//  Test header
	_LIT8(KBodyTxt1,"TC 10_1:Test Msg");  // first 16 chars

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
	
	testData.Copy(KHeaderTxt); //Copy the test header descriptor 
	returnCode = ptrString.Find(testData); //find the test header descriptor in the buffer read
										 //from the file
	if (returnCode > 0)
		{
		testData.Copy(KHexTestMargin); //Copy the test margin descriptor	
		returnCode = ptrString.Find(testData); //find the test margin descriptor in the buffer read
									 	 //from the file
		}

	if (returnCode > 0)
		{
		testData.Copy(KBodyTxt1); //Copy the test descriptor 
		returnCode=ptrString.Find(testData); //find the test descriptor in the buffer read
										 //from the file
		}

	if (returnCode == KErrNotFound)  //Find the error message
		{
		returnCode = ptrString.Find(KOOMError);
		if (returnCode > 0)
			User::Leave(KErrNoMemory);
		}

	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy();	//theFile object
	if (returnCode > 0)
		return KErrNone;
	else 
		return KErrNotFound;
	}




TInt CFloggerTest010_01::DoTestConnect()
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









CFloggerTest010_02::CFloggerTest010_02()
	{
	// store the name of this test case
	SetTestStepName(_L("step_010_02"));
	}




CFloggerTest010_02::~CFloggerTest010_02()
	{
	}



TVerdict CFloggerTest010_02::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 010.02 called "));

	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	
	User::After(KTimeForDisplay);
	return TestStepResult();
	}



TInt CFloggerTest010_02::executeStepL( )
	{
	TInt ret = KErrGeneral;
	ret = DoTestConnect();

	if (ret == KErrNone)
		{
		ret = DoTestWrite();
			if (ret == KErrNone)
			{
			TRAPD(r, ret = DoTestCheckWriteL());
		
			if (r != KErrNone)
				ret = r;
				
			}
			
		}
	iFlogger.Close();
	return ret;
	}

TInt CFloggerTest010_02::executeStepL(TBool)
	{
	return KErrGeneral;
	}


TInt CFloggerTest010_02::DoTestWrite()
	{
	_LIT8(KDescText,"TC 10_2:Test Msg");

	iFlogger.HexDump(KDescText, KEightSpaces8); 
	ForceLogFlush(iFlogger);

	iFlogger.__DbgShutDownServer();
	iFlogger.Close();
	return KErrNone; 
	}




TInt CFloggerTest010_02::DoTestCheckWriteL()
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



TInt CFloggerTest010_02::DoTestConnect()
	{
	TInt ret = KErrGeneral;
	TPtrC8 ptrSubSystem, ptrComponent;
	ptrSubSystem.Set(_L8("SubSystem"));
	ptrComponent.Set(_L8("Component"));
	
	ret = iFlogger.Connect(); //Call connect() of Flogger
	if (ret == KErrNone)
		ret = iFlogger.SetLogTags(ptrSubSystem, ptrComponent); //SetLogTags() of Flogger called

	if (ret == KErrNone)
		ret = iFlogger.ClearLog(); //clear the contents from the log

	return ret;
	}








CFloggerTest010_03::CFloggerTest010_03()
	{
	// store the name of this test case
	SetTestStepName(_L("step_010_03"));
	}




CFloggerTest010_03::~CFloggerTest010_03()
	{
	//iFlogger.Close(); //Close the flogger 
	}




TVerdict CFloggerTest010_03::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 010.03 called "));
	
	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	
	User::After(KTimeForDisplay);
	return TestStepResult();
	}	


TInt CFloggerTest010_03::executeStepL(TBool)
	{
	return KErrGeneral;
	}

TInt CFloggerTest010_03::executeStepL( )
	{
	TInt ret = KErrGeneral;
	ret = DoTestConnect();

	if (ret == KErrNone)
		{
		ret = DoTestWrite();
			if (ret == KErrNone)
			{
			TRAPD(r, ret = DoTestCheckWriteL());
		
			if (r != KErrNone)
				ret = r;
			}
		}
	iFlogger.Close();
	return ret;
	}




TInt CFloggerTest010_03::DoTestWrite()
	{
	_LIT8(KDescTxt,"TC 10_3:Test Msg"); //8 bit test descriptor


	iFlogger.HexDump(KDescTxt); 
	ForceLogFlush(iFlogger);
	
	iFlogger.__DbgShutDownServer();
	iFlogger.Close();
	return KErrNone; 
	}





TInt CFloggerTest010_03::DoTestCheckWriteL()
	{
	User::After(KTimeToLog);
	RFile theFile;
	HBufC8 * hBuffer;
	TInt listfilesize;
	TInt returnCode;
	RFs fileSystem; //For file operation create a file system	
	TBuf8<256> testData; //To hold the test descriptor
	_LIT(KLogFile,"c:\\logs\\log.txt"); // the log file path

	_LIT8(KBodyTxt,"TC 10_3:Test Msg");//  Test body descriptor 
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
	returnCode=theFile.Read(ptrString);
		
	testData.Copy(KBodyTxt); //Copy the test descriptor 
	returnCode=ptrString.Find(testData); //find the test descriptor in the buffer read
										 //from the file
										 
	if (returnCode == KErrNotFound)  //Find the error message
		{
		returnCode = ptrString.Find(KOOMError);
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




TInt CFloggerTest010_03::DoTestConnect()
	{
	TInt result;
	TPtrC8 ptrSubSystem;
	TPtrC8 ptrComponent;
	ptrSubSystem.Set(_L8("SubSystem"));
	ptrComponent.Set(_L8("Component"));
	
	result = iFlogger.Connect(); //Call connect() of Flogger
	if (result == KErrNone)
		{
		result = iFlogger.SetLogTags(ptrSubSystem, ptrComponent); //SetLogTags() of Flogger called
																//to set the tags
		result = iFlogger.ClearLog(); //clear the contents in the log
		
		}
	return result;

	}








CFloggerTest010_04::CFloggerTest010_04()
	{
	// store the name of this test case
	SetTestStepName(_L("step_010_04"));
	}




CFloggerTest010_04::~CFloggerTest010_04()
	{
	}



TVerdict CFloggerTest010_04::doTestStepL( )
	{

	INFO_PRINTF1(_L("Step 010.04 called "));
	
	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	
	User::After(KTimeForDisplay);
	return TestStepResult();
	}

TInt CFloggerTest010_04::executeStepL(TBool)
	{
	return KErrGeneral;
	}

TInt CFloggerTest010_04::executeStepL( )
	{
	TInt ret = KErrGeneral;
	ret = DoTestConnect();

	if (ret == KErrNone)
		{
		ret = DoTestWrite();
			if (ret == KErrNone)
			{
			TRAPD(r, ret = DoTestCheckWriteL());
		
			if (r != KErrNone)
				ret = r;
			}
		}
	iFlogger.Close();
	return ret;
	}




TInt CFloggerTest010_04::DoTestWrite()
	{
	_LIT8(KTestDesc,"TC 10_4:Test Msg");

	iFlogger.HexDump(KTestDesc, KNullDesC8);
	ForceLogFlush(iFlogger);
	iFlogger.__DbgShutDownServer();
	iFlogger.Close();
	return KErrNone; 
	}




TInt CFloggerTest010_04::DoTestCheckWriteL()
	{
	User::After(KTimeToLog);
	RFile theFile;
	HBufC8 * hBuffer;
	TInt listfilesize;
	TInt returnCode;
	RFs fileSystem; //For file operation create a file system	
	TBuf8<256> testData; //To hold the test descriptor
	_LIT(KLogFile,"c:\\logs\\log.txt"); // the log file path

	_LIT8(KBodyTxt,"TC 10_4:Test Msg");//  Test body descriptor 
	_LIT8(KOOMError, "#Logs may be lost out of memory!!"); //Error message
	TInt numSuccessful= 0;

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
		returnCode = ptrString.Find(KOOMError);
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




TInt CFloggerTest010_04::DoTestConnect()
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









CFloggerTest010_05::CFloggerTest010_05()
	{
	// store the name of this test case
	SetTestStepName(_L("step_010_05"));
	}	



CFloggerTest010_05::~CFloggerTest010_05()
	{
	}




TVerdict CFloggerTest010_05::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 010.05 called "));
	
	if ( executeStepL() == KErrNone )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	
	User::After(KTimeForDisplay);
	return TestStepResult();
	}



TInt CFloggerTest010_05::executeStepL(TBool)
	{
	return KErrGeneral;
	}
	
TInt CFloggerTest010_05::executeStepL( )
	{
	User::After(100*1000);	//Need this or connect will return KErrServerTerminated
	TInt ret = KErrGeneral;
	ret = DoTestConnect();

	if (ret == KErrNone)
		{
		ret = DoTestWrite();
			if (ret == KErrNone)
			{
			TRAPD(r, ret = DoTestCheckWriteL());
		
			if (r != KErrNone)
				ret = r;
			}
		}
	if (ret == KErrNone)
		iFlogger.__DbgShutDownServer();
	iFlogger.Close();
	return ret;
	}	



TInt CFloggerTest010_05::DoTestWrite()
	{
	ForceLogFlush(iFlogger);

	iFlogger.HexDump(KNullDesC8);
	return KErrNone; 
	}



TInt CFloggerTest010_05::DoTestCheckWriteL()
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





TInt CFloggerTest010_05::DoTestConnect()
	{
	TInt ret = KErrGeneral;
	TPtrC8 ptrSubSystem, ptrComponent;
	ptrSubSystem.Set(_L8("SubSystem"));
	ptrComponent.Set(_L8("Component"));
	
	ret = iFlogger.Connect(); //Call connect() of Flogger
	if (ret == KErrNone)
		ret = iFlogger.SetLogTags(ptrSubSystem, ptrComponent); //SetLogTags() of Flogger called

	if (ret == KErrNone)
		ret = iFlogger.ClearLog(); //clear the contents from the log

	return ret;

	}






CFloggerTest010_06::CFloggerTest010_06()
	{
	// store the name of this test case
	SetTestStepName(_L("step_010_06"));
	}




CFloggerTest010_06::~CFloggerTest010_06()
	{
	}



TVerdict CFloggerTest010_06::doTestStepL( )
	{
	User::After(100*1000);	//Need this or connect will return KErrServerTerminated

	INFO_PRINTF1(_L("Step 010.06 called "));

	CFloggerTest010_01* step010_01 = new CFloggerTest010_01;
	CleanupStack::PushL(step010_01);
	doTestStepWithHeapFailureL( *step010_01, 1, 20, KErrNone, ETrue);

	CleanupStack::PopAndDestroy(step010_01);

	User::After(KTimeForDisplay);
	return TestStepResult();
	}

