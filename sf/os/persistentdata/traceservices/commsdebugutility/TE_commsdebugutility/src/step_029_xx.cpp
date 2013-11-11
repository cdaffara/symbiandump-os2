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
#include "TestMessage.h"
#include "step_029_xx.h"


#include <comms-infras/commsdebugutility.h>



/**
* 029_IniFile01: Test that flogger handles ini change to debugport and file
*/ 

CFloggerTest029_IniFile01::CFloggerTest029_IniFile01()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_029_IniFile01"));
	}




CFloggerTest029_IniFile01::~CFloggerTest029_IniFile01()
	{
	}




TVerdict CFloggerTest029_IniFile01::doTestStepL( )
	{
	if ( executeStepL() == KErrNone  )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	User::After(KTimeForDisplay);

	
	return TestStepResult();
	}

TInt CFloggerTest029_IniFile01::executeStepL(TBool)
	{
	return KErrGeneral;
	}


TInt CFloggerTest029_IniFile01::executeStepL()
	{
	TInt ret;
	TInt r;
	RFileLogger theFlogger;
	
	ret = theFlogger.Connect();
	
	if ( ret == KErrNone)
		{
		theFlogger.SetLogTags(KStdSubsysTag8, KStdCompTag8);
		theFlogger.ClearLog();
		theFlogger.Write(KLogFillerMessage8);
		theFlogger.Write(KLogFillerMessage8);
		theFlogger.Close();
	
		replaceFloggerIniL( KIniConfigWithFileAndDebugPort );
		RFileLogger::Write(KStdSubsysTag8, KStdCompTag8,KTestMessage8);
	
		User::After(KTimeToLog);
		TRAP(r, ret = DoTestCheckWriteL());
		if (r != KErrNone)
			ret = r;
		}

	constructFloggerIniL( KDefaultIniFileSettings );
	
	return ret;		

	}




TInt CFloggerTest029_IniFile01::DoTestCheckWriteL()
	{
	RFile theFile;
	HBufC8 * hBuffer;
	TInt listfilesize,returnCode;
	RFs fileSystem; //For file operation create a file system
	TUint numSuccessful = 0;

	
	User::LeaveIfError(fileSystem.Connect());
	CleanupClosePushL(fileSystem);
	
	//Open the file in the read mode
	User::LeaveIfError(theFile.Open(fileSystem,KFloggerOutputFile,EFileRead)); 

	CleanupClosePushL(theFile);

	User::LeaveIfError(theFile.Size(listfilesize)); //Size of the file
	
	hBuffer = HBufC8::New(listfilesize); //Allocate the buffer
	CleanupStack::PushL(hBuffer);

	TPtr8 ptrString = hBuffer->Des();  ; //To hold the buffer

	// Read from position 0: start of file
	User::LeaveIfError(returnCode = theFile.Read(ptrString));
	
	returnCode = ptrString.Find(KMessageIniHasChanged); //find the test descriptor in the buffer read
											//from the file
	
	if (returnCode > 0)
		{
		numSuccessful++;
		}
	else if (returnCode == KErrNotFound)
		{
		returnCode = ptrString.Find(KErrOOM);
		if (returnCode > 0)
			User::Leave(KErrNoMemory);
		}

	returnCode = ptrString.Find(KTestMessage8); //find the test descriptor in the buffer read
											//from the file
	
	if (returnCode > 0)
		{
		numSuccessful++;
		}


	// flogger should not have had a problem reading the ini file
	returnCode = ptrString.Find(KErrIniProblemMessage);
	
	if (returnCode == KErrNotFound)
		{
		numSuccessful++;
		}
	
	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy();	//theFile
	CleanupStack::PopAndDestroy();  //fileSystem
	
	if (numSuccessful == 3)
		return KErrNone;
	else 
		return KErrNotFound;
	}



/**
* 029_IniFile02: Test that flogger handles ini change to debugport and serial
*/ 

CFloggerTest029_IniFile02::CFloggerTest029_IniFile02()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_029_IniFile02"));
	}




CFloggerTest029_IniFile02::~CFloggerTest029_IniFile02()
	{
	}




TVerdict CFloggerTest029_IniFile02::doTestStepL( )
	{
	if ( executeStepL() == KErrNone  )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	User::After(KTimeForDisplay);

	
	return TestStepResult();
	}


TInt CFloggerTest029_IniFile02::executeStepL(TBool)
	{
	return KErrGeneral;
	}

TInt CFloggerTest029_IniFile02::executeStepL()
	{
	TInt ret;
	RFileLogger theFlogger;
	
	ret = theFlogger.Connect();
	
	if ( ret == KErrNone)
		{
		theFlogger.SetLogTags(KStdSubsysTag8, KStdCompTag8);
		theFlogger.ClearLog();
		theFlogger.Write(KLogFillerMessage8);
		theFlogger.Write(KLogFillerMessage8);
		theFlogger.Close();
	
		replaceFloggerIniL( KIniConfigWithSerialAndDebugPort );
		RFileLogger::Write(KStdSubsysTag8, KStdCompTag8,KTestMessage8);
	
		// serial logging means there isn't actually anything to check
		// since we can't check the serial output
		User::After(KTimeToLog);
		}

	constructFloggerIniL( KDefaultIniFileSettings );
	
	return ret;

	}








/**
* 029_IniFile03: Test that flogger handles ini change to debugport and file
*/ 

CFloggerTest029_IniFile03::CFloggerTest029_IniFile03()
	{
	// Store the name of this test case
	SetTestStepName(_L("step_029_IniFile03"));
	}




CFloggerTest029_IniFile03::~CFloggerTest029_IniFile03()
	{
	}




TVerdict CFloggerTest029_IniFile03::doTestStepL( )
	{
	if ( executeStepL() == KErrNone  )
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);

	User::After(KTimeForDisplay);

	
	return TestStepResult();
	}


TInt CFloggerTest029_IniFile03::executeStepL(TBool)
	{
	return KErrGeneral;
	}

TInt CFloggerTest029_IniFile03::executeStepL()
	{
	TInt ret;
	TInt r;
	RFileLogger theFlogger;
	
	ret = theFlogger.Connect();
	
	if ( ret == KErrNone)
		{
		theFlogger.SetLogTags(KStdSubsysTag8, KStdCompTag8);
		theFlogger.ClearLog();
		theFlogger.Write(KLogFillerMessage8);
		theFlogger.Write(KLogFillerMessage8);
		theFlogger.Close();
	
		TRAP(r,constructFloggerIniL( KSyncTestsIniFileSettings ));
		RFileLogger::Write(KStdSubsysTag8, KStdCompTag8,KTestMessage8);
	
		User::After(KTimeToLog);  // although we asked for synchronous mode, we cannot switch to it while flogger is running
		TRAP(r, ret = DoTestCheckWriteL());
		if (r != KErrNone)
			{
			ret = r;
			}
		}

	TRAP(r,constructFloggerIniL( KDefaultIniFileSettings ));
	if (r != KErrNone)
		{
		ret = r;
		}
	
	return ret;		

	}




TInt CFloggerTest029_IniFile03::DoTestCheckWriteL()
	{
	RFile theFile;
	HBufC8 * hBuffer;
	TInt listfilesize,returnCode;
	RFs fileSystem; //For file operation create a file system
	TUint numSuccessful = 0;

	
	User::LeaveIfError(fileSystem.Connect());
	CleanupClosePushL(fileSystem);
	
	//Open the file in the read mode
	User::LeaveIfError(theFile.Open(fileSystem,KFloggerOutputFile,EFileRead)); 

	CleanupClosePushL(theFile);

	User::LeaveIfError(theFile.Size(listfilesize)); //Size of the file
	
	hBuffer = HBufC8::New(listfilesize); //Allocate the buffer
	CleanupStack::PushL(hBuffer);

	TPtr8 ptrString = hBuffer->Des();  ; //To hold the buffer

	// Read from position 0: start of file
	User::LeaveIfError(returnCode = theFile.Read(ptrString));
	
	returnCode = ptrString.Find(KMessageIniHasChanged); //find the test descriptor in the buffer read
											//from the file
	
	if (returnCode > 0)
		{
		numSuccessful++;
		}
	else if (returnCode == KErrNotFound)
		{
		returnCode = ptrString.Find(KErrOOM);
		if (returnCode > 0)
			User::Leave(KErrNoMemory);
		}

	returnCode = ptrString.Find(KTestMessage8); //find the test descriptor in the buffer read
											//from the file
	
	if (returnCode > 0)
		{
		numSuccessful++;
		}


	// flogger should not have had a problem reading the ini file
	returnCode = ptrString.Find(KErrIniProblemMessage);
	
	if (returnCode == KErrNotFound)
		{
		numSuccessful++;
		}
	
	CleanupStack::PopAndDestroy(hBuffer);
	CleanupStack::PopAndDestroy();	//theFile
	CleanupStack::PopAndDestroy();  //fileSystem
	
	if (numSuccessful == 3)
		return KErrNone;
	else 
		return KErrNotFound;
	}


/**
 029_IniFile04 - test changing ini file on the fly and removing a tag from it
 doTestStep returns whether test case passed or failed. 
 When running heapcheck, doTestStep is not called - only executeStepL is called,
  and it must return either KErrNoMemory or KErrNone to pass the heap check.
*/ 

CFloggerTest029_IniFile04::CFloggerTest029_IniFile04()
	{
	// store the name of this test case
	SetTestStepName(_L("step_029_IniFile04"));
	}

// destructor
CFloggerTest029_IniFile04::~CFloggerTest029_IniFile04()
	{

	}


// do Test step 029.01
TVerdict CFloggerTest029_IniFile04::doTestStepL( )
	{
	if (executeStepL(EFalse) == KErrNone)
		SetTestStepResult(EPass);

	else
		SetTestStepResult(EFail);


	INFO_PRINTF1(_L("leaving Step 029.IniFile04 "));

	return TestStepResult() ;
	}

TInt CFloggerTest029_IniFile04::executeStepL()
	{
	return KErrGeneral;
	}

TInt CFloggerTest029_IniFile04::executeStepL(TBool heapTest)
/**
@param heapTest  supplied as ETrue when heap checking so that
  we can catch instances of data not being in the file and let harness know
  that perhaps its because no memory yet.
 */
	{
	TInt ret = KErrGeneral;
	
	TRAPD(r, ret = DoTestWrite(0))
	
	if (r == KErrNone && ret == KErrNone)
		{
		User::After(KTimeToLog);
		TRAP(r, ret = DoTestCheckWriteL(0));
			{
			if ((heapTest) && (ret == KErrNotFound))
				{
				//
				ret = KErrNoMemory;
				}
			else if (r == KErrNone && ret == KErrNone)
				{
				constructFloggerIniL( KIniFileChangeIniFileSettings );
				TRAP(r, ret = DoTestWrite(1))
				
				User::After(KTimeToLog);
				TRAP(r, ret = DoTestCheckWriteL(1));
				if ((heapTest) && (ret == KErrNotFound))
					{
					ret = KErrNoMemory;
					}
				}
			}

		}
	// regardless of what happens, we need to ensure we close the connection
	// if it was opened
	TRAP(r, DoTestWrite(2))
	constructFloggerIniL( KDefaultIniFileSettings );
	
	return ret;
	
	}




//NOTE
// If aStatus is 0 then first modification
// If aStatus is 1 then second modification

TInt CFloggerTest029_IniFile04::DoTestWrite(TInt aStatus)
	{
	
	TPtrC8 ptrSubSystem;
	TPtrC8 ptrComponent;
	ptrSubSystem.Set(_L8("SubSystem"));
	ptrComponent.Set(_L8("Component"));
	TInt ret = KErrGeneral;


	if (aStatus ==0)
		{	
		ret = iFlogger.Connect(); //Connect 
	
		if (ret == KErrNone)
			{
			//Set tags by passing the subsystem and component name as parameters
			ret = iFlogger.SetLogTags(ptrSubSystem, ptrComponent); 
			if ( ret == KErrNone)
				{
				ret = iFlogger.ClearLog(); //Clear old log messages in the log
				}
			else if (ret == KErrNotFound)
				{
				ret = KErrNoMemory; // keep heap checker happy
				}
			if ( ret == KErrNone)
				{
				iFlogger.Write(KFirstTestMessage); //Write the test descriptor
				ForceLogFlush(iFlogger);
				}
			}
		
		return ret;
		
		}
	if (aStatus == 1)
		{
		//Set tags by passing the subsystem and component name as parameters
		iFlogger.SetLogTags(ptrSubSystem, ptrComponent); 
		iFlogger.Write(KSecondTestMessage); //Write the test descriptor
		iFlogger.SetLogTags(_L8("Esock"),_L8("Esock.txt"));
		iFlogger.Write(KHexTestHeader); //Write the test descriptor
		}
	if (aStatus == 2)
		{
		iFlogger.Close();
		}
	
	return KErrNone;
		
	}

//NOTE
// If aFileStatus is 0 then status before modification
// If aFileStatus is 1 then status after modification

TInt CFloggerTest029_IniFile04::DoTestCheckWriteL(TInt aFileStatus)  
	{
	RFile theFile;
	HBufC8 * hBuffer;
	TInt listfilesize = KErrGeneral;
	TInt returnCode = KErrNotFound;
	RFs fileSystem; //For file operation create a file system
	TInt numSuccessful = 0;


	User::LeaveIfError(fileSystem.Connect()); //Connect to the file server
	
	//Open the file in the read mode
	User::LeaveIfError(theFile.Open(fileSystem, KFloggerOutputFile, EFileRead)); 

	User::LeaveIfError(returnCode = theFile.Size(listfilesize)); //Size of the file

	hBuffer = HBufC8::New(listfilesize); //Allocate the buffer
	CleanupStack::PushL(hBuffer);
	TPtr8 ptrString = hBuffer->Des();  ; //To hold the buffer

	// Read from position 0: starting of file
	returnCode = theFile.Read(ptrString);
			
	if (aFileStatus == 0)
		{
		// as a little extra test, the subsystem tag in the "ini" file is "Subsystem"
		// but we wrote the data as "SubSystem" so it should still be in the file,
		// but we need to search non-case sensitive
		returnCode = ptrString.FindF(KStdSubsysTag8);	//find the test system name in 
											//the buffer read from the file

		
		if (returnCode > 0) //Note test system should be present before/After the ini is modified
			{

			returnCode = ptrString.Find(KStdCompTag8);	//find the test component name in the buffer read
													//from the file
			}
			if (returnCode > 0) //Note test descriptor should be present before the ini is modified
				{
				returnCode = ptrString.Find(KFirstTestMessage); //find the test descriptor in the buffer read
													//from the file					
				}

		CleanupStack::PopAndDestroy(hBuffer);
		theFile.Close();
		fileSystem.Close();

		
		if (returnCode > 0) //Note test descriptor should be present before the ini is modified
			returnCode = KErrNone;
	
		}


	else if (aFileStatus == 1)
		{
		returnCode = ptrString.Find(KFirstTestMessage);	//find the first test descriptor in
												//the buffer read from the file

		if (returnCode > 0)	//Note first test message should be present even after 
							//the ini is modified
			{
			numSuccessful++;
			}
		returnCode = ptrString.Find(KErrIniProblemMessage);
		if (returnCode > 0)	//Note as a bonus the ini we supplied has an error, so a message as such should be there
			{
			numSuccessful++;
			}
		
		returnCode = ptrString.Find(KSecondTestMessage);
		if ( returnCode == KErrNotFound )	// The second test msg should not be there since there is no associated tag in the ini file
			{
			numSuccessful++;
			}
			
		returnCode = ptrString.Find(KMessageIniHasChanged);
		if ( returnCode > 0 )	// Flogger should see that the ini file has changed
			{
			numSuccessful++;
			}

		CleanupStack::PopAndDestroy(hBuffer);
		theFile.Close();
		fileSystem.Close();

		if (numSuccessful == 4)
			returnCode = KErrNone;
		}
	
	
	return returnCode;
	}




CFloggerTest029_IniFile04H::CFloggerTest029_IniFile04H()
	{
	// store the name of this test case
	SetTestStepName(_L("step_029_IniFile04H"));
	}



CFloggerTest029_IniFile04H::~CFloggerTest029_IniFile04H()
	{
	}



TVerdict CFloggerTest029_IniFile04H::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step 029.IniFile04H called "));
	User::After(KTimeForDisplay1);

	CFloggerTest029_IniFile04* step029_IniFile04 = new CFloggerTest029_IniFile04;
	CleanupStack::PushL(step029_IniFile04);
	doTestStepWithHeapFailureL( *step029_IniFile04, 523, 550, KErrNone, ETrue);
	
	CleanupStack::PopAndDestroy(step029_IniFile04);
	INFO_PRINTF1(_L("leaving Step 029.IniFile04H "));
	

	return TestStepResult();
	}

