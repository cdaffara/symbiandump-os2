/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


#include "t_testsetup.h"
#include "t_testactionspec.h"
#include "t_input.h"
#include "t_certstoreactionmemfail.h"
#include "tcancel.h"
#include "t_message.h"
#include "tScriptSetup.h"
#include "Thardcodedsetup.h"
#include "t_testhandler.h"
#include "t_output.h"
#include "tTestSpec.h"
#include "ttesthandlersettings.h"
#include "testexecuteinterface.h"
#include "t_logger.h"
#include "t_testrunner.h"
#include "t_dummyconsole.h"

EXPORT_C HBufC* CTestSetup::GetArgument(TInt nPos)
	{
	// Get command line
	HBufC *argv = HBufC::NewLC(User::CommandLineLength());
	TPtr cmd(argv->Des());
	User::CommandLine(cmd);

	TLex arguments(cmd);
	TPtrC token;
	token.Set(KNullDesC);
	
	// finds nth parameter that doesnt have a - 
	while(nPos >= 0 && !arguments.Eos())
		{
		token.Set(arguments.NextToken());
		if(token.Length() > 0 && token[0] != '-')
			nPos--;
		}

	HBufC* result = token.AllocL();

	CleanupStack::PopAndDestroy(argv);

	return result;
	}

void CTestSetup::InitFileserverSessionLC(RFs& aFs)
	{
	User::LeaveIfError(aFs.Connect());
	CleanupClosePushL(aFs);
	
	// enable tests to pass file handles between processes
	aFs.ShareProtected();
	
	// Create the private directory, to fix failures in tests that write to
	// relative paths that were broken when the default path was changed to the
	// private path in build 03429
	TFileName privatePath;
	User::LeaveIfError(aFs.PrivatePath(privatePath));
	TInt err = aFs.MkDir(privatePath);
	if (err != KErrNone && err != KErrAlreadyExists)
		{
		User::Leave(err);
		}
	}

EXPORT_C void CTestSetup::CreateAndRunTestsL(TScriptTests theTestTypes[], 
		const TDesC& aScript, const TDesC& aLogFile, TBool aUseCommandLine, 
		CConsoleBase* aConsole, TBool* aResult)
{
    LOG(_L("-- Test handler starting")); 
 	
	RFs fs;

	InitFileserverSessionLC(fs);
	
	CScriptSetup* testSetup = CScriptSetup::NewLC(aConsole);
	CTestHandlerSettings* commandLineArgs = CTestHandlerSettings::NewLC();
 	CTestSpec* testSpec = CTestSpec::NewL();
	CleanupStack::PushL(testSpec);

	TTestSummary summary;
	if(testSetup->InitialiseL(fs, aScript, aLogFile, aUseCommandLine))
		{
		
		// Store the state of the heap and RFs resource count before the tests
		
		TInt initAllocCount = User::CountAllocCells();
		TInt initRFsCount = fs.ResourceCount();
		
		// Store the initial count of process and thread handles
		TInt initThreadHandleCount;
		TInt initProcessHandleCount;
		RThread().HandleCount(initProcessHandleCount, initThreadHandleCount);
		
		testSetup->LogFile().write(_L("<html><body><pre>\r\n"));
		testSetup->LogFile().write(_L("State of the system before the tests:\n"));
		testSetup->LogFile().write(_L("\tRFs resource count: %d\n"), initRFsCount);
		testSetup->LogFile().write(_L("\tProcess handle count: %d\n"), initProcessHandleCount);
		testSetup->LogFile().write(_L("\tThread handle count: %d\n"), initThreadHandleCount);
			
		TRAPD(error, testSetup->SetupTestsL(fs, *testSpec, theTestTypes, *commandLineArgs))
			
		if(error==KErrNone)
			{
			CTestHandler* handler = CTestHandler::NewLC(fs, *testSpec, *commandLineArgs, 
														&testSetup->Console(),
														&testSetup->LogFile());
			handler->RunTestsL();
			summary = handler->Summary();
			testSpec->FreeAllTests();
			CleanupStack::PopAndDestroy(handler);
			
			// Do heap, RFs resource, process and thread handle balance checks	
			
			TInt finalRFsCount = fs.ResourceCount();
			
			testSetup->LogFile().write(_L("State of the system after the tests:\n"));
			testSetup->LogFile().write(_L("\tRFs resource count: %d\n"), finalRFsCount);

			if (initRFsCount == finalRFsCount)
				{
				testSetup->LogFile().write(_L("\nRFs resource count ok: %d final\n\n"),
										   finalRFsCount);
				}
			else
				{
				testSetup->LogFile().write(_L("\nRFs resource count inbalance: %d final\n\n"),
										   finalRFsCount);
				++summary.iTestsFailed;
				}
	

			TInt finalAllocCount = User::CountAllocCells();
			if (initAllocCount == finalAllocCount)
				{
				testSetup->LogFile().write(_L("\nHeap alloc count ok: %d final vs %d initial\n\n"),
										   finalAllocCount, initAllocCount);
				}
			else
				{
				testSetup->LogFile().write(_L("\nHeap alloc count inbalance: %d final vs %d initial\n\n"),
										   finalAllocCount, initAllocCount);
				++summary.iTestsFailed;
				}

			TInt finalThreadHandleCount;
			TInt finalProcessHandleCount;
			RThread().HandleCount(finalProcessHandleCount, finalThreadHandleCount);

			if (initProcessHandleCount == finalProcessHandleCount)
				{
				testSetup->LogFile().write(_L("\nProcess handle count ok: %d final vs %d initial\n\n"),
										   finalProcessHandleCount, initProcessHandleCount);
				}
			else
				{
				testSetup->LogFile().write(_L("\nProcess handle count imbalance: %d final vs %d initial\n\n"),
										   finalProcessHandleCount, initProcessHandleCount);
				++summary.iTestsFailed;
				}

			if (initThreadHandleCount == finalThreadHandleCount)
				{
				testSetup->LogFile().write(_L("\nThread handle count ok: %d final vs %d initial\n\n"),
										   finalThreadHandleCount, initThreadHandleCount);
				}
			else
				{
				testSetup->LogFile().write(_L("\nThread handle count imbalance: %d final vs %d initial\n\n"),
										   finalThreadHandleCount, initThreadHandleCount);
				++summary.iTestsFailed;
				}
			++summary.iTestsRun;
			}
		
		// Set the result if required by caller
		if (aResult)
			{
			*aResult = summary.AllTestsPassed();
			}

		summary.PrintL(testSetup->LogFile());
	
		// pauses runtime if command line requests it i.e. -w
		if(commandLineArgs->iWaitForKeyPressAtEnd)
			{
			testSetup->Console().Printf(_L("\nPress a key to quit"));
			testSetup->Console().Getch();
			}

		}

	CleanupStack::PopAndDestroy(4, &fs); // fs, testsetup, commandLineArgs and testspec
	// this MUST be the last thing to do incase objects being destructed on cleanup
	// have debug info
	//if (bTestSuccess)
	//	RDebug::RawPrint(_L("RTEST: SUCCESS : testhandler\n"));
}

EXPORT_C void CTestSetup::CreateAndRunTestsL(THardcodedTests theTestTypes[], const TDesC& aDefaultLog)
{
    LOG(_L("-- Test handler starting")); 
 
	RFs fs;

	InitFileserverSessionLC(fs);
	
	InitFileserverSessionLC(fs);
	
	CHardcodedSetup* testSetup = CHardcodedSetup::NewLC();
	CTestHandlerSettings* commandLineArgs = CTestHandlerSettings::NewLC();
 	CTestSpec* testSpec = CTestSpec::NewL();
	CleanupStack::PushL(testSpec);

	TTestSummary summary;
	if(testSetup->InitialiseL(fs, aDefaultLog))
		{
		
		// Store the state of the heap and RFs resource count before the tests
		
		TInt initAllocCount = User::CountAllocCells();
		TInt initRFsCount = fs.ResourceCount();
				
		TRAPD(error, testSetup->SetupTestsL(fs, *testSpec, theTestTypes, *commandLineArgs));
			
		if(error==KErrNone)
			{
			CTestHandler* handler = CTestHandler::NewLC(fs, *testSpec, *commandLineArgs, 
														&testSetup->Console(),
														&testSetup->LogFile());
			handler->RunTestsL();
			summary = handler->Summary();
			testSpec->FreeAllTests();
			CleanupStack::PopAndDestroy(handler);

			// Do heap and RFs resource balance checks
			
			TInt finalRFsCount = fs.ResourceCount();
			
			testSetup->LogFile().write(_L("State of the system after the tests:\n"));
			testSetup->LogFile().write(_L("\tRFs resource count: %d\n"), finalRFsCount);

			if (initRFsCount == finalRFsCount)
				{
				testSetup->LogFile().write(_L("\nRFs resource count ok: %d final\n\n"),
										   finalRFsCount);
				}
			else
				{
				testSetup->LogFile().write(_L("\nRFs resource count inbalance: %d final\n\n"),
										   finalRFsCount);
				++summary.iTestsFailed;
				}

			TInt finalAllocCount = User::CountAllocCells();
			if (initAllocCount == finalAllocCount)
				{
				testSetup->LogFile().write(_L("\nHeap alloc count ok: %d final vs %d initial\n\n"),
										   finalAllocCount, initAllocCount);
				}
			else
				{
				testSetup->LogFile().write(_L("\nHeap alloc count inbalance: %d final vs %d initial\n\n"),
										   finalAllocCount, initAllocCount);
				++summary.iTestsFailed;
				}
			++summary.iTestsRun;
			}

		summary.PrintL(testSetup->LogFile());

		// pauses runtime if command line requests it i.e. -w
		if(commandLineArgs->iWaitForKeyPressAtEnd)
			{
			testSetup->Console().Printf(_L("\nPress a key to quit"));
			testSetup->Console().Getch();
			}
		}

	CleanupStack::PopAndDestroy(4, &fs); // fs, testsetup, commandLineArgs and testspec
	
	//if (bTestSuccess)
	//	RDebug::RawPrint(_L("RTEST: SUCCESS : testhandler\n"));
}

//Check all flags is provided for understanability - Each grouping function called calls the next function
// if it has succeeded
TBool CTestSetup::CheckAllFlags(const CTestHandlerSettings& aCommandLineSettings, TInt& aScriptGroupings)
	{
	return CheckExhaustiveandSmoke(aCommandLineSettings,aScriptGroupings);
	}

TBool CTestSetup::CheckExhaustiveandSmoke(const CTestHandlerSettings& aCommandLineSettings, TInt& aScriptGroupings)
	{
	
	if (aCommandLineSettings.iExhaust || (aScriptGroupings & SMOKE))
		{
		//Do other flags
		return CheckSkipped(aCommandLineSettings, aScriptGroupings);
		}
	else

		return EFalse;
	}


TBool CTestSetup::CheckSkipped(const CTestHandlerSettings& aCommandLineSettings,
							   TInt& aScriptGroupings)
	{
	if (aCommandLineSettings.iSkip || !(aScriptGroupings & SKIP))
		{
		//Do other flags
		return CheckInteractive(aCommandLineSettings, aScriptGroupings);
		}
	else
		return EFalse;
	}

TBool CTestSetup::CheckInteractive(const CTestHandlerSettings& aCommandLineSettings,
								   TInt& aScriptGroupings)
	{
	if (aCommandLineSettings.iInt || !(aScriptGroupings & INTER))
		{
		//Do other flags
		return CheckOOMandCancel(aCommandLineSettings, aScriptGroupings);
		}
	else
		return EFalse;
	}

TBool CTestSetup::CheckOOMandCancel(const CTestHandlerSettings& aCommandLineSettings, 
									TInt& aScriptGroupings)
	{

	if ((aScriptGroupings & (EXOOM | INOOM)) == (EXOOM | INOOM))
		User::Panic(_L("Test is both Included and Excluded from OOM"), 1);

	if ((aScriptGroupings & (EXCANCEL | INCANCEL)) == (EXCANCEL | INCANCEL))
		User::Panic(_L("Test is both Included and Excluded from Cancel"), 1);

	if ((aScriptGroupings & (INOOM | INCANCEL)) == (INOOM | INCANCEL))
		User::Panic(_L("Test is in both OOM and Cancel groups"), 1);

	// Check to see whether -o set
	if (aCommandLineSettings.iOOM)
		{
		//Is the test in OOM group?
		if (aScriptGroupings & INOOM)
			return ETrue;
		else
		//Is the test excluded from OOM?
		if (aScriptGroupings & EXOOM)
			return EFalse;
		}
	else
	if (aScriptGroupings & INOOM)
			return EFalse;

	// Check to see whether -c set
	if (aCommandLineSettings.iCancel)
		{
		//Is the test in CANCEL group?
		if (aScriptGroupings & INCANCEL)
			return ETrue;
		else
		//Is the test excluded from CANCEL?
		if (aScriptGroupings & EXCANCEL)
			return EFalse;
		}
	else
	if (aScriptGroupings & INCANCEL)
			return EFalse;


	return ETrue;
	}


EXPORT_C CTestSetup::~CTestSetup()
	{
	delete iLogFile;
	delete iTestConsole;
	if (iConsoleOwned)
		{
		// Note that the heap count when this is deleted must be the same as
		// when the console was allocated, otherwise a panic will occur.  This
		// is due to the techview implementation of the console doing a heap
		// mark in its destructor.
		delete iConsole;
		}
	}

CTestSetup::CTestSetup(CConsoleBase* aConsole) 
	: iConsole(aConsole), iConsoleOwned(aConsole == NULL)
	{
	}

void CTestSetup::ConstructL()
	{
	if (iConsoleOwned)
		{
		iConsole = Console::NewL(_L("Test code"), TSize(KDefaultConsWidth, KDefaultConsHeight));
		}

	// Currently the console passed to the test actions discards all output -
	// this is an attempt to make hardware tests run faster.  All information
	// should be written to the log file anyway, and I'd like to remove use of a
	// console as well as the log file.  -- jc
	iTestConsole = new (ELeave) CDummyConsole();
	}

void CTestSetup::OpenLogFileL(RFs &aFs, TInt nPos, const TDesC &aLogFile, TBool aUseCommandline)
	{
	HBufC* logFileName = NULL;

	if (aUseCommandline)
		{
		logFileName = GetArgument(nPos);
		CleanupStack::PushL(logFileName);
		
		// check if logfile was specified on command line
		if(logFileName->Length()==0)
			{
			// empty so remove it
			CleanupStack::PopAndDestroy(logFileName);
			logFileName = NULL;
			}
		}
	
	if(logFileName == NULL)
		{
		if(aLogFile.Length()==0)
			{
			PRINTANDLOG(_L("No log file specified on command line and no default given"));
			User::Leave(KErrArgument);
			}
		else
			{
			// there is a default copy that
			logFileName = aLogFile.AllocL();
			CleanupStack::PushL(logFileName);
			}
		}

	PRINTANDLOG1(_L("Log file: %S"), logFileName);
	
	RFile logFile;

	// attempts to create directories incase they dont exist
	aFs.MkDirAll(*logFileName); // ignore errors
 
	// write over any existing log
	User::LeaveIfError(logFile.Replace(aFs, *logFileName, EFileWrite));

	// output goes only to the log file
	iLogFile = new(ELeave) FileOutput(logFile);
	CleanupStack::PopAndDestroy(logFileName);
	};
