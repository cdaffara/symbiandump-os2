// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#include "IpuTestUtils.h"
#include <e32consf.h>
//
//	Constants
_LIT(KTestPanic, "IpuTestHarness");
const TInt KFailedTestsGranularity = 10;
const TInt KMaxLogEntrySize = 256;

//
//	CIpuTestHarness
//
CIpuTestHarness::CIpuTestHarness(const TDesC& aTitle)
	: iTest(aTitle)
//
//	Default c'tor
	{
    LogRTestToFile(iTest);
	iTest.Title();
	iCanStartTest = ETrue;
	}

CIpuTestHarness::~CIpuTestHarness()
//
//	D'tor
	{
	TTime endtime;
	endtime.UniversalTime();

	// Do resource handle leak test?
	if (iDoResourceLeakTest)
		ResourceLeakTest();

	//	End of tests - see if failed or ok
	if (iFailedTests->Count())
		{
		TestHarnessFailed();
		}
	else
		{
		TestHarnessComplete();
		}

	iFailedTests->ResetAndDestroy();
	delete iFailedTests;

	//	Log finish time
	TDateTime t = endtime.DateTime();
	LogIt(_L("Ended @ %d:%d:%d:%d"),t.Hour(),t.Minute(),t.Second(),t.MicroSecond());
	TTime difftime(endtime.Int64() - iStartTime.Int64());
	t = difftime.DateTime();
	LogIt(_L("Execution time %d:%d:%d:%d"),t.Hour(),t.Minute(),t.Second(),t.MicroSecond());

	//	Close logs and test harness
	iFlogger.CloseLog();
	
	// iTest test harness performs UHEAP MARK/UNMARK check upon creation/destruction
	//   therefore, it must be destroyed last since it is created first in 
	//   CIpuTestHarness
	iTest.Close();
	}

EXPORT_C CIpuTestHarness* CIpuTestHarness::NewLC(const TDesC& aTitle)
//
//	Static factory c'tor
	{
	CIpuTestHarness* self = new (ELeave) CIpuTestHarness(aTitle);
	CleanupStack::PushL(self);
	self->ConstructL(aTitle);
	return self;
	}

EXPORT_C CIpuTestHarness* CIpuTestHarness::NewL(const TDesC& aTitle)
//
//	Static factiry c'tor
	{
	CIpuTestHarness* self = CIpuTestHarness::NewLC(aTitle);
	CleanupStack::Pop();
	return self;
	}

void CIpuTestHarness::ConstructL(const TDesC& aTitle)
//
//	Non-trivial c'tor
	{
	//	Create iFailedTests
	iFailedTests = new (ELeave) CArrayPtrFlat<CTestInfo> (KFailedTestsGranularity);

	//	Start up logging server connection
	TBuf<64> temp(aTitle);
	DefaultLogFileName(temp);
	CreateFlogger(temp, EFalse, EFalse);

	iStartTime.UniversalTime();
	TDateTime t = iStartTime.DateTime();
	LogIt(_L("Started @ %d:%d:%d:%d"),t.Hour(),t.Minute(),t.Second(),t.MicroSecond());

	// Find number of open resource handles
	TInt processHandleCount=0;
	RThread().HandleCount(processHandleCount,iStartHandleCount);
	}

EXPORT_C void CIpuTestHarness::StartTestL(const TDesC& aName)
//
//	Logs start of test aName
	{
	if (iCanStartTest)
		{
		//  - increment test count
		++iTestCount;
		
		if (iTestMode == ETestModeNormal) // don't add this info when we are doing memory leak testing otherwise it
										  // would get leaked!
			{

			//	Add this test to failed test list - set errorcode to zero
			CTestInfo* temp = CTestInfo::NewLC(aName, iTestCount, 0);
			iFailedTests->AppendL(temp);
			CleanupStack::Pop();	//	temp

			//	Stop new test being started until this one has ended
			iTest.Start(aName);
			iCanStartTest = EFalse;
			}

		
		TBuf<KMaxFileName + 4> buf;
		buf.Format(KTestStartingWithDesc, iTestCount, &aName);
		WriteComment(buf);

		// Reset iStepNumber - start at 1
		iStepNumber = 1;
		}
	else
		{
		//	Panic client - bad usage - not allowed to nest tests
		Panic(EBadStartTest);
		}
	}

EXPORT_C void CIpuTestHarness::NextStep(const TDesC& aStepName)
//
//	Logs the next step in a test - for informative use.
	{
	if (!iCanStartTest)
		{
		TBuf<KMaxFileName + 4> buf;
		buf.Format(KNextTestStepWithDesc, iTestCount, iStepNumber, &aStepName);
		WriteComment(buf);
		iTest.Next(aStepName);
		++iStepNumber;
		}
	else
		{
		//	Panic client - bad usage - test not started
		Panic(EBadStartTest);
		}
	}

EXPORT_C void CIpuTestHarness::EndTest(TInt aErrorCode)
//
//	Logs end of test
	{
	if (!iCanStartTest)
		{
		if (iTestMode == ETestModeNormal)
			{
			//	Get ptr to this test's entry in failed list - will be the last entry
			TBuf<KMaxFileName + 4> buf;
			TInt index = iFailedTests->Count();
			CTestInfo* ptr = iFailedTests->At(--index);
			if (aErrorCode)
				{
				//	Set the error code
				ptr->SetErrorCode(aErrorCode);
				buf.Format(KTestFailed, iTestCount, aErrorCode);
				WriteComment(buf);
				}
			else
				{
				//	Remove entry from list of failed tests
				delete ptr;
				iFailedTests->Delete(index);
				}
			
			}
		//	Allow new test to start
		iTest.End();
		iCanStartTest = ETrue;
		}
	else
		{
		if (iTestMode == ETestModeNormal)
			//	Panic client - bad usage - test not started
			Panic(EBadEndTest);
		// don't panic when we are memory leak testing as EndTestL will never get called to reset the test properly
		}
	}

EXPORT_C void CIpuTestHarness::LogIt(TRefByValue<const TDesC> aFmt, ...)
//
//	Messages to the front end emulator and to the Inu log
	{
	VA_LIST list;
	VA_START(list,aFmt);

	TBuf<KMaxFileName + 4> buf;
	buf.Append(KTestCommentPrepend);
	buf.AppendFormatList(aFmt,list);
	VA_END(list);

	WriteComment(buf);
	}

EXPORT_C void CIpuTestHarness::operator()(TInt aResult,TInt aLineNum)
//
//	Overload operator ()
	{
	iTest(aResult, aLineNum);
	}

EXPORT_C void CIpuTestHarness::operator()(TInt aResult)
//
//	Overload operator ()
	{
	iTest(aResult);
	}

EXPORT_C void CIpuTestHarness::PressAnyKey()
//
//	Request a key press from user and wait - unless we are running a script
	{
	if (!iScriptRunning)
		{
		iTest.Printf(TRefByValue<const TDesC>_L("\nPress a key"));	
		iTest.Getch();
		}
	}

EXPORT_C void CIpuTestHarness::DumpData(HBufC8& aData, TBool logIt)
//
//	Do a formatted dump of binary data, optionally logging it
	{
	// Iterate the supplied block of data in blocks of 16 bytes
	TInt pos = 0;
	TBuf<KMaxLogEntrySize> logLine;
	TBuf<KMaxLogEntrySize> anEntry;
	while (pos < aData.Length())
		{
		anEntry.Format(TRefByValue<const TDesC>_L("%04x : "), pos);
		logLine.Append(anEntry);

		// Hex output
		TInt offset = 0;
		for (offset = 0; offset < 16; offset++)
			{
			if (pos + offset < aData.Length())
				{
				TInt nextByte = aData[pos + offset];
				anEntry.Format(TRefByValue<const TDesC>_L("%02x "), nextByte);
				logLine.Append(anEntry);
				}
			else
				{
				anEntry.Format(TRefByValue<const TDesC>_L("   "));
				logLine.Append(anEntry);
				}
			}
			anEntry.Format(TRefByValue<const TDesC>_L(": "));
			logLine.Append(anEntry);

		// Char output
		for (offset = 0; offset < 16; offset++)
			{
			if (pos + offset < aData.Length())
				{
				TInt nextByte = aData[pos + offset];
				if ((nextByte >= 32) && (nextByte <= 127))
					{
					anEntry.Format(TRefByValue<const TDesC>_L("%c"), nextByte);
					logLine.Append(anEntry);
					}
				else
					{
					anEntry.Format(TRefByValue<const TDesC>_L("."));
					logLine.Append(anEntry);
					}
				}
			else
				{
				anEntry.Format(TRefByValue<const TDesC>_L(" "));
				logLine.Append(anEntry);
				}
			}
			if (logIt)
				{
				LogIt(TRefByValue<const TDesC>_L("%S"), &logLine);
				}
			else
				{
				iTest.Printf(TRefByValue<const TDesC>_L("%S\n"), &logLine);	
				}
			logLine.Zero();

		// Advance to next 16 byte segment
		pos += 16;
		}
	}

EXPORT_C void CIpuTestHarness::GetAnEntry(const TDesC& ourPrompt, TDes& currentstring)
//
//	Get an input string from the user, displaying a supplied prompt and default string value
	{
	// If we're scripting, try reading from script first
	TInt readScriptErr = KErrNotFound;
	if (iScriptRunning)
		{
		readScriptErr = ReadLineFromScript(currentstring);
		}
	if (!readScriptErr)
		return;

	// Either not scripting, or hit end of script - continue with user input
	TBuf16<KMaxUserEntrySize> ourLine;
	TBuf<KMaxUserEntrySize> tempstring;				//tempstring is a unicode descriptor
										//create a temporary buffer where the
										//unicode strings are stored in order to 
										//be displayed
	ourLine.Zero ();
	tempstring.Copy(currentstring);		//Copy current string to Unicode buffer
	TKeyCode key = EKeyNull;						//current string buffer is 8 bits wide.
										//Unicode string bufffer (tempstring) is 16 bits wide.
	for (;;)
		{
		if (ourLine.Length () == 0)
			{
			iTest.Console()->SetPos (0, iTest.Console()->WhereY ());
			iTest.Console()->Printf (_L ("%S"), &ourPrompt);
			if (tempstring.Length () != 0)						//get tempstring's number of items
				iTest.Console()->Printf (_L (" = %S"), &tempstring);	//if not zero print them to iTest.Console()
			iTest.Console()->Printf (_L (" : "));
			iTest.Console()->ClearToEndOfLine ();
			}
		key = iTest.Getch();
		
		  if (key == EKeyBackspace)
				{
					if (ourLine.Length() !=0)
					{
						ourLine.SetLength(ourLine.Length()-1);
						iTest.Console()->Printf (_L ("%c"), key);
						iTest.Console()->SetPos(iTest.Console()->WhereX(),iTest.Console()->WhereY());
						iTest.Console()->ClearToEndOfLine();
					}	// end if (ourLine.Length() !=0)
				}	// end if (key == KeyBackSpace)
		  
		  		  
		  if (key == EKeyDelete) 			
				{
					ourLine.Zero();
					iTest.Console()->SetPos (0, iTest.Console()->WhereY ());
					iTest.Console()->ClearToEndOfLine ();
					tempstring.Copy(ourLine);
					break;
				}
		  
		  if (key == EKeyEnter)
			break;
		
		  if (key < 32)
			{
			continue;
			}
		
		ourLine.Append (key);
		iTest.Console()->Printf (_L ("%c"), key);
		iTest.Console()->SetPos(iTest.Console()->WhereX(),iTest.Console()->WhereY());
		iTest.Console()->ClearToEndOfLine();
		if (ourLine.Length () == ourLine.MaxLength ())
			break;
		}	// end of for statement

	if ((key == EKeyEnter) && (ourLine.Length () == 0))
		tempstring.Copy (currentstring);				//copy contents of 8 bit "ourLine" descriptor
	
	iTest.Console()->SetPos (0, iTest.Console()->WhereY ());		
	iTest.Console()->ClearToEndOfLine ();
	iTest.Console()->Printf (_L ("%S"), &ourPrompt);
	
	if ((key == EKeyEnter) && (ourLine.Length() !=0))
		tempstring.Copy(ourLine);
	if (tempstring.Length () != 0)						//if temstring length is not zero
		{
		iTest.Console()->Printf (_L (" = %S\n"), &tempstring);	//print the contents to iTest.Console()
		LogIt(_L ("%S = %S\n"), &ourPrompt, &tempstring);
		}

	else
		//iTest.Console()->Printf (_L (" is empty"));
	iTest.Console()->Printf (_L ("\n"));
	currentstring.Copy(tempstring);						//copy 16 bit tempstring descriptor back 
	}


EXPORT_C TInt CIpuTestHarness::GetSelection(const TDesC& ourPrompt, const TDesC& validChoices)
//
//	Present the user with a list of options, and get their selection
	{
	// If we're scripting, try reading from script first
	TInt readScriptErr = KErrNotFound;
	if (iScriptRunning)
		{
		TBuf<1> oneCharBuf;
		readScriptErr = ReadLineFromScript(oneCharBuf);
		if (!readScriptErr)
			{
			return validChoices.Locate((TChar)oneCharBuf[0]);
			}
		}

	// Either not scripting, or hit end of script - continue with user input
	TKeyCode key = EKeyNull;
	iTest.Console()->SetPos (0, iTest.Console()->WhereY ());
	iTest.Console()->Printf(_L("%S "), &ourPrompt);
	iTest.Console()->Printf(_L("[%S] :"), &validChoices);
	TInt retVal = KErrNotFound;
	while (retVal == KErrNotFound)
		{
		key = iTest.Getch();

		// Check that key is in the list of valid choices
		retVal = validChoices.Locate((TChar)key);
		}
	iTest.Console()->Printf(_L("%c\n\n"), key);
	return retVal;
	}


EXPORT_C void CIpuTestHarness::SetScript(RFile& scriptFile)
//
//	Sets the file to be used for a test script - ie. a file that contains commands used by
//  GetEntry() and GetSelection()
	{
	iScriptFile = &scriptFile;
	iScriptRunning = ETrue;
	LogIt(_L("***SCRIPT STARTING***\n"));
	}

TInt CIpuTestHarness::ReadLineFromScript(TDes& aBuffer)
//
// Reads the next line from the script file, and sets the passed-in descriptor with its contents.
// Returns KErrNone if reading succeeded; KErrNotFound if the EOF was reached. When EOF is reached,
// the file is closed.
	{
	// *********************************
	// Assume script is 8-bit text file
	// *********************************
	TBool isAComment = ETrue;
	TInt err = KErrNone;
	TBuf<512> line;
	while (isAComment && !err)
		{
		TFileText text;
		text.Set(*iScriptFile);
		line.SetLength(0);
		for(;;)
			{
			TBuf8<2> c;
			err = iScriptFile->Read(c,1);
			if (err && err != KErrEof)
				{
				iTest.Printf(_L("Error reading file: %d\n"), err);
				break;
				}
			if (c.Length() == 0)
				{
				err = KErrEof;
				break;
				}
			else
				{
				if (c[0] == '\n') // break out if it is CR
					break;
				else if (c[0] != (TUint8)(0x0d)) // otherwise append the char, _unless_ it is a LF
					line.Append(c[0]);
				}
			}
		if (err == KErrNone && line.Locate('/') != 0) // comment (only works if it's the first character)
			{
			isAComment = EFalse;
			}
		}

	// The line read is not a comment, or have hit end of file
	if (!err)
		{
		// copy to passed in descriptor, but do not allow an overflow
		aBuffer.Copy(line.Left(aBuffer.MaxLength()));
		LogIt(_L("***SCRIPT : read command '%S' ***\n"), &aBuffer);
		}
	else
		{
		iScriptFile->Close();
		err = KErrNotFound;
		iScriptRunning = EFalse;
		LogIt(_L("***SCRIPT ENDED***\n"));
		}
	return err;
	}

void CIpuTestHarness::Panic(TInt aPanic)
//
//	Panic the client program.
	{
	User::Panic(KTestPanic,aPanic);
	}

void CIpuTestHarness::TestHarnessComplete()
//
//	Test harness completed without failures
	{
	_LIT(KTestCompleteFormat, "Total Tests %d, Failed Tests %d");
	TBuf<50> text;
	text.AppendFormat(KTestCompleteFormat, iTestCount, iFailedTests->Count()); 
	WriteComment(text);
	WriteComment(KTestHarnessCompleted);
	}

void CIpuTestHarness::TestHarnessFailed()
//
//	Test harness has a failure - log information
	{
	TBuf<KMaxFileName + 4> buf;
	buf.Format(KTestHarnessFailed, iFailedTests->Count());
	WriteComment(buf);
	//	Log fialed tests' information
	for (TInt ii=0; ii<iFailedTests->Count(); ++ii)
		{
		CTestInfo* failed = iFailedTests->At(ii);
		TPtrC name = failed->Name();
		LogIt(KTestFailInfo, failed->Number(), &name, failed->ErrorCode());
		}
	}

void CIpuTestHarness::ResourceLeakTest()
//
// Creates a new test that fails if any there are any leaked resource handles
	{
	// Start new test
	_LIT(KResourceTestName, "Resource Handle Leak Test");
	TRAPD(testError, StartTestL(KResourceTestName));
	if(testError==KErrNone)
		{
		//	Find number of opened handles
		TInt processHandleCount=0;
		TInt threadHandleCount=0;
		RThread().HandleCount(processHandleCount,threadHandleCount);
		TInt openHandleCount = iStartHandleCount-threadHandleCount;
		TInt err = KErrNone;
		if ( openHandleCount !=0 )
			{
			err = KErrGeneral;
			LogIt(_L("Number leaked handles is %D"), openHandleCount);
			}
		EndTest(err);
		}
	else
		{
		_LIT(KTxtResourceTestRunError, "Unable to complete Resource Leak Test, error: %d");
		LogIt(KTxtResourceTestRunError, testError);
		EndTest(testError);
		}
	}

//
//	CTestInfo
//
CIpuTestHarness::CTestInfo::CTestInfo()
//
//	Default c'tor
	{
	}

CIpuTestHarness::CTestInfo::~CTestInfo()
//
//	D'tor
	{
	delete iName;
	}

CIpuTestHarness::CTestInfo* CIpuTestHarness::CTestInfo::NewLC(const TDesC& aName, TInt aNumber, TInt aErrorCode)
//
//	Static factory c'tor
	{
	CTestInfo* self = new (ELeave) CTestInfo();
	CleanupStack::PushL(self);
	self->ConstructL(aName, aNumber, aErrorCode);
	return self;
	}

CIpuTestHarness::CTestInfo* CIpuTestHarness::CTestInfo::NewL(const TDesC& aName, TInt aNumber, TInt aErrorCode)
//
//	Static factory c'tor
	{
	CTestInfo* self = NewLC(aName, aNumber, aErrorCode);
	CleanupStack::Pop();	//	self
	return self;
	}

void CIpuTestHarness::CTestInfo::ConstructL(const TDesC& aName, TInt aNumber, TInt aErrorCode)
//
//	Non-trivial c'tor
	{
	iName = aName.AllocLC();
	CleanupStack::Pop();	//	iName

	iNumber = aNumber;
	iErrorCode = aErrorCode;
	}

void CIpuTestHarness::CTestInfo::SetNameL(const TDesC& aName)
//
//	Sets iName
	{
	HBufC* temp = aName.AllocLC();
	CleanupStack::Pop();	//	temp
	delete iName;
	iName = temp;
	}

void CIpuTestHarness::CTestInfo::SetNumber(TInt aNumber)
//
//	Sets iNumber
	{
	iNumber = aNumber;
	}

void CIpuTestHarness::CTestInfo::SetErrorCode(TInt aErrorCode)
//
//	Sets iErrorCode
	{
	iErrorCode = aErrorCode;
	}
