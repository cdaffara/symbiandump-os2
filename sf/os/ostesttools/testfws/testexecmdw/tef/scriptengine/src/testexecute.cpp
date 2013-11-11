/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  
* Script Engine executable entry point.
* Log initialisation.
* Command line read.
* Trigger entry into the top level state machine.
*
*/



/**
 @file testexecute.cpp
*/

#include <test/testexecuteclient.h>
#include "scriptengine.h"
#include <test/testexecutelogger.h>
#include "testwatcher.h"
#include "version.h"
#include "tefutils.h"

#include <f32file.h>
#include <test/wrapperutilsplugin.h>


const TInt KBuffSize= 500;
GLDEF_D TInt CScriptControl::commentedCommandsCount;
GLDEF_D TInt CScriptControl::iRunScriptFailCount;

/**
 * Code for reading commandline set when testexecute is invoked
 * @param aCommandLine - Function fills descriptor with space separated command line args
 */
LOCAL_C void ReadCommandLineL(TDes& aCommandLine)
	{
	// Far simpler for EKA2
	if(User::CommandLineLength() > aCommandLine.MaxLength())
		User::Leave(KErrTooBig);
	User::CommandLine(aCommandLine);
	}

/**
 * Function to parse command line and set optional flags with ON/OFF status acordingly
 * @param aCommandLine - Command line descriptor with ' ' separated arguments/tokens
 * @param aSeparateLogFiles - Function sets to ETrue if -slf is detected in command line
 * @param aJustInTime - Function sets to ETrue if -d is detected in command line
 * @param aGraphicalWindowServer - Function sets to ETrue if -gws is detected in command line
 * @param HelpRequest - Function sets to ETrue if -help is detected in command line
 * @param aVersionRequest - Function sets to ETrue if -v is detected in command line
 * @param aParseTestExecuteIni - Object of the parser class for checking options set through testexecute.ini
 * @param aParserState - TInt value representing whether the parser object instance is active or not
 */
LOCAL_C void ParseCommandLine(const TDesC& aCommandLine,TBool& aSeparateLogFiles,TBool& aJustInTime,
TBool& aGraphicalWindowServer,TBool& aIncludeSelectiveCases,TBool& aExcludeSelectiveCases, TBool& aPipe, TBool& aHelpRequest, TBool& aVersionRequest, 
CTestExecuteIniData& aParseTestExecuteIni, TInt& aParserState)
	{
	// Set up a lex for looping through the command line arguments
	TLex flagLex(aCommandLine);
	// Initialise all flags to false
	aJustInTime = EFalse;
	aSeparateLogFiles = EFalse;
	aGraphicalWindowServer = EFalse;
	aHelpRequest = EFalse;
	aVersionRequest = EFalse;

	_LIT(KTestExecuteCommandLineHelp, "-help");
	_LIT(KTestExecuteCommandLineVersion, "-v");

	while(!flagLex.Eos())
		{
		TPtrC token(flagLex.NextToken());
		TInt result = 0;
		// Get the value for JustInTimeDebug from the TestExecute.ini parser 
		if (aParserState == KErrNone)
			{
			// Get the value from testexecute.ini for optional flag 'JustInTimeDebug'
			aParseTestExecuteIni.GetKeyValueFromIni(KTEFJustInTimeDebug, result);
			}
		const TInt KOne = 1;
		if(token == KTestExecuteCommandLineFlagDebugMode || result == KOne)
			{
			// if the command line contains the token '-d' or the testexecute.ini flag options for JustinTime is set to 'ON'
			aJustInTime = ETrue;
			}
		else if(token == KTestExecuteCommandLineFlagSeparateLogFileMode)
			{
			// if the command line contains the token '-slf'
			aSeparateLogFiles = ETrue;
			}
		else if(token == KTestExecuteCommandLineFlagIncludeSelectiveCases)
			{
			// if the command line contains the token '-tci'
			aIncludeSelectiveCases= ETrue;
			}
		else if(token == KTestExecuteCommandLineFlagExcludeSelectiveCases)
			{
			// if the command line contains the token '-tcx'
			aExcludeSelectiveCases= ETrue;
			}
		else if(token == KTestExecuteCommandLineFlagPipe)
			{
			aPipe = ETrue ; 
			}
		else if(token == KTestExecuteCommandLineHelp)
			{
			// if the command line contains the token '-help'
			aHelpRequest = ETrue;
			}
		else if(token == KTestExecuteCommandLineVersion)
			{
			// if the command line contains the token '-v'
			aVersionRequest = ETrue;
			}
		}
	}

LOCAL_C void DistinguishElement(const TPtrC& aElement, RArray<TRange>& aSelectiveCaseRange)
{

	TInt colonOccurance = aElement.FindC(KTEFColon);
	//we are expecting only a range or a test case ID over here...
	if( colonOccurance!=KErrNotFound )
		{
		//then this is a range of testcases, split it at the colon
		TRange newRange(aElement.Left(colonOccurance),aElement.Mid(colonOccurance+1));
		aSelectiveCaseRange.Append(newRange);
		}
	else
		{
		TRange newRange(aElement,aElement);
		aSelectiveCaseRange.Append(newRange);	
		}
}

/**
 * Read the Cfg File data into a heap buffer
 * And populate the arrays of selective test case IDs and 
 * Ranges to be used by the state machine
 * NOTE: we do not support nested cfgs...
 */
LOCAL_C void CreateCfgDataFromFileL(TPtrC& aCfgFilePath,RArray<TRange>& aSelectiveCaseRange, TDesC*& aSelTestCfgFileData)
	{
	
	RFs fS;
	User::LeaveIfError(fS.Connect());
	CleanupClosePushL(fS);
	RFile cfgFile;
	User::LeaveIfError(cfgFile.Open(fS,aCfgFilePath,EFileRead | EFileShareAny));
	CleanupClosePushL(cfgFile);
	TInt fileSize;
	User::LeaveIfError(cfgFile.Size(fileSize));
	// Create a 16bit heap buffer
	HBufC* cfgData = HBufC::NewL(fileSize);
	CleanupStack::PushL(cfgData);
	HBufC8* narrowData = HBufC8::NewL(fileSize);
	CleanupStack::PushL(narrowData);
	TPtr8 narrowPtr=narrowData->Des();
	// Read the file into an 8bit heap buffer
	User::LeaveIfError(cfgFile.Read(narrowPtr));
	TPtr widePtr(cfgData->Des());
	// Copy it to the 16bit buffer
	widePtr.Copy(narrowData->Des());
	CleanupStack::PopAndDestroy(narrowData);
	CleanupStack::Pop(cfgData);
	CleanupStack::Pop(2);
	cfgFile.Close();
	fS.Close();
	// Set up the instance token parser
	TLex cfgLex = cfgData->Des();
	aSelTestCfgFileData = cfgData; // to preserve the pointer of cfgdata and transfer the ownership to aSelTestCfgFileData 
	cfgData = NULL; // relinquish the ownership
	while(!cfgLex.Eos())
		{
		DistinguishElement(cfgLex.NextToken(),aSelectiveCaseRange) ; 
		}
	
	}



LOCAL_C void ParseCommandLineForSelectiveTestingOptions(const TDesC& aCommandLine,CTestExecuteIniData& aParseTestExecuteIni,RArray<TRange>& aSelectiveCaseRange,  TDesC*& aSelTestCfgFileData)
	{
	TLex flagLex(aCommandLine);
	//first of all navigate the command line arguments to the selective test cases flag...
	while(!flagLex.Eos())
	{
		TPtrC token(flagLex.NextToken());
		if( (token.CompareF(KTestExecuteCommandLineFlagIncludeSelectiveCases) == 0) || (token.CompareF(KTestExecuteCommandLineFlagExcludeSelectiveCases) == 0) )
			{
			break;
			}
	}
	
	TPtrC argument(flagLex.NextToken()) ; 
	while(argument.Length() > 0 )	
		{
		TInt commaOccurance = argument.FindC(KTEFComma) ;
		TPtrC element ; 
		if(commaOccurance == -1)	
			{
			//handle the case where no comma is found, 
			//assume only one item in the list
			element.Set(argument) ; 
			//reset argument to break the while
			argument.Set(KNull);
			}
		else
			{
			element.Set(argument.Left(commaOccurance));
			//take the remaining in the argument
			TInt len = argument.Length()-commaOccurance-1 ; 
			argument.Set(argument.Right(len)) ; 	
			}
		
		TInt cfgExtensionOccurance = element.FindC(KTEFCfgExtension) ; 
		if (cfgExtensionOccurance!=KErrNotFound) //madatory extension to be given
			{
			
			TPtrC cfgFilePath(element);
			//its probably the cfg file path.
			TBuf<KBuffSize> tempScriptPath(cfgFilePath);
			// Check whether cfg name is provided along with folder path in the command line
			// If not, take the path from testexecute.ini & name from the command line
			if(cfgFilePath.FindC(KTEFColon)==KErrNotFound)
				{
				TBuf<KMaxTestExecuteNameLength> tempBuffer;
				aParseTestExecuteIni.GetKeyValueFromIni(KTEFDefaultScriptPath, tempBuffer);
				cfgFilePath.Set(tempBuffer);
				// Construct the full file path from the values extracted from command line and ini file
				TBuf<KBuffSize> storeScriptPathTemp(cfgFilePath);
				storeScriptPathTemp.Append(tempScriptPath);
				cfgFilePath.Set(storeScriptPathTemp);
				tempScriptPath.Copy(cfgFilePath);
				}
				// and parse this cfg File to populate our structures
				TRAP_IGNORE(CreateCfgDataFromFileL(cfgFilePath,aSelectiveCaseRange, aSelTestCfgFileData));
				//actually do nothing with the error...
			}
		else
			{
			DistinguishElement(element,aSelectiveCaseRange  );
			}
		}
	}

/**
 * Subroutine performing pre-processing tasks for testexecute execution
 * Also, responsible for triggering the state machine through instance of ScriptMaster
 * @param aScheduler - Instance of ActiveScheduler created through MainL()
 * @param aSysDrive - Default system drive letter to be used for all script parsing
 */
LOCAL_C void ProcessMainL(CActiveScheduler* aScheduler, const TDriveName aSysDrive)
	{
	TDriveName defaultSysDrive(aSysDrive);
	TDriveName testSysDrive(KTEFLegacySysDrive);
	TInt waitForLoggingTime = 0;
	TBuf<KMaxTestExecuteNameLength> htmlLogPath;
	// Create a object of the Parser for TestExecute.ini
	CTestExecuteIniData* parseTestExecuteIni = NULL;
	TRAPD(err, parseTestExecuteIni = CTestExecuteIniData::NewL(defaultSysDrive));
	if (err == KErrNone)
		{
		CleanupStack::PushL(parseTestExecuteIni);
		// Extract all the key values within the object
		parseTestExecuteIni->ExtractValuesFromIni();
		}

	// Read and parse the command line for the flags
	// -d -slf -help and -v
	TBuf<KMaxTestExecuteCommandLength> commandLine;
	TDesC* selTestCfgFileData = NULL; //the pointer to the data of in the .tcs file
	ReadCommandLineL(commandLine);
	
	// Make lower case because we parse it for flags and search for ".script"
	commandLine.LowerCase();
	TBool separateLogFiles(EFalse); // -slf
	TBool justInTime(EFalse); // -d
	TBool graphicalWindowServer(EFalse); // -gws
	TBool helpRequest(EFalse); // -help
	TBool versionRequest(EFalse); // -v

	TBool includeSelectiveCases(EFalse); // -tci
	TBool excludeSelectiveCases(EFalse); // -tcx
	TBool pipe(EFalse) ; 
	
	// Set up the bools from the command line
	ParseCommandLine(commandLine,separateLogFiles,justInTime,graphicalWindowServer,includeSelectiveCases, excludeSelectiveCases,pipe, helpRequest,versionRequest,*parseTestExecuteIni,err);
	// If -d then set Just In Time debugging. Panicks break into debug on emulator
	(justInTime) ? (User::SetJustInTime(ETrue)) : (User::SetJustInTime(EFalse)); 

	// Hooks for creating the Graphical Window server
	#ifdef GWS
	#endif

	// Create a console
	_LIT(KMessage,"TestExecute Script Engine");
	CConsoleBase* console = Console::NewL(KMessage,TSize(KConsFullScreen,KConsFullScreen));
	CleanupStack::PushL(console);
	console->SetCursorHeight(0);
	RConsoleLogger consoleLogger(*console);
	
	CScriptControl::iRunScriptFailCount=0;
	// A lex for getting the first command line argument, ie the script file path
	TLex lex(commandLine);
	TPtrC scriptFilePath(lex.NextToken());
	TInt ret = KErrNotFound;
	if (scriptFilePath.CompareF(KNull) != 0)
		{
		_LIT(KTEFSwitchPrefix, "-");
		if(scriptFilePath.Mid(0,1).CompareF(KTEFSwitchPrefix) == 0)
			{
			// If the first command line argument is not the script file path but a optional switches
			// Then set the script file path for the execution to be 'blank'
			scriptFilePath.Set(KNull);
			}
		else
			{
			TBuf<KBuffSize> tempScriptPath(scriptFilePath);

			// Check whether script name is provided along with folder path in the command line
			// If not, take the path from testexecute.ini & name from the command line
			ret=scriptFilePath.FindC(KTEFColon);
			if(ret==KErrNotFound)
				{
				if (parseTestExecuteIni != NULL)
					{
					TBuf<KMaxTestExecuteNameLength> tempBuffer;
					parseTestExecuteIni->GetKeyValueFromIni(KTEFDefaultScriptPath, tempBuffer);
 					// If the relative script file path does not refer to the root,
 					// we will look for DefaultScriptDir entry in testexecute.ini
 					// If available prepend it to the relative path
 					// else if the relative path refers to root,
 					// then set the default system drive, i.e. c:
 					// else leaving it as it is (considering invalid path)
 					if (scriptFilePath.Left(1).CompareF(KTEFSlash) != 0 &&
 					 tempBuffer.Length() > 0)
 						scriptFilePath.Set(tempBuffer);
 					else if (scriptFilePath.Left(1).CompareF(KTEFSlash) == 0)
 						scriptFilePath.Set(defaultSysDrive);
 					else
 						scriptFilePath.Set(KNull);
					}
				else
					{
					// If the file path is not provided in command line as well as in testexecute.ini
					// then set the  script file path to be 'blank'
					scriptFilePath.Set(KNull);
					}

				// Construct the full file path from the values extracted from command line and ini file
				TBuf<KBuffSize> storeScriptPathTemp(scriptFilePath);
				storeScriptPathTemp.Append(tempScriptPath);
				scriptFilePath.Set(storeScriptPathTemp);
				tempScriptPath.Copy(scriptFilePath);
				}

			//If scriptFilePath is not appended by .script Append .script 
			if(scriptFilePath.Find(KTEFScriptExtension)==KErrNotFound)
				{
				tempScriptPath.Append(KTEFScriptExtension);
				}
			scriptFilePath.Set(tempScriptPath);
			}
		}

	TPtrC pipeName ; 
	if(pipe)
		{
		TLex flagLex(commandLine);
		//first of all navigate the command line arguments to the selective test cases flag...
		while(!flagLex.Eos())
		{
			TPtrC token(flagLex.NextToken());
			if( (token.CompareF(KTestExecuteCommandLineFlagPipe) == 0) )
				{
				break;
				}
		}
		pipeName.Set(flagLex.NextToken()) ; 	
		}
	
	TSelectiveTestingOptions* selTestingOptions =NULL;
	if (includeSelectiveCases && excludeSelectiveCases)
		{
			//mutually exclusive options have been encountered
			includeSelectiveCases =EFalse;
			excludeSelectiveCases =EFalse;					
			console->Printf(KTEFInvalidCommandSetMessage);
			console->Printf(KTEFEnterKeyMessage);
			console->Getch();
		}
	else
		{
		if(includeSelectiveCases || excludeSelectiveCases)
			{
			RArray<TRange> selectiveCaseRange;
			ParseCommandLineForSelectiveTestingOptions(commandLine,*parseTestExecuteIni,selectiveCaseRange, selTestCfgFileData);
			
			//you need to sort these two arrays first, and also if they are both empty ignore the entire option altogether.
			if(  selectiveCaseRange.Count() > 0 )
				{
				CleanupStack::PushL(selTestCfgFileData);
				TLinearOrder<TRange> orderingrng(TRange::CompareTRangeStartOrder) ; 
				selectiveCaseRange.Sort(orderingrng );
				ESelectiveTesting selectiveTestingType(iExclusive);
				if(includeSelectiveCases)
					{
					selectiveTestingType = iInclusive ; 
					}
				selTestingOptions = new(ELeave) TSelectiveTestingOptions(selectiveCaseRange, selectiveTestingType);
				}
			else
				{
				//if no arguments to this option have been found, ignore it...
				includeSelectiveCases =EFalse;
				excludeSelectiveCases =EFalse;
				delete selTestCfgFileData;
				}
			}
		}

	if (scriptFilePath.CompareF(KNull)==0)
		{
		if (!separateLogFiles && !justInTime)
			{
			// Print the product version details through console window
			console->Printf(KTEFVersionMessage);
			console->Printf(KTEFProductVersion);

			if (!(versionRequest && !helpRequest))
				{
				// Print the help & usage informations through console window along with product version
				console->Printf(KTEFConsoleHelpMessage1);
				console->Printf(KTEFConsoleHelpMessage2);
				console->Printf(KTEFConsoleHelpMessage3);
				console->Printf(KTEFConsoleHelpMessage4);
				console->Printf(KTEFConsoleHelpMessage5);
				}
			}
		else
			{
			// Display a error message on the console window for invalid set of arguments
			console->Printf(KTEFInvalidCommandSetMessage);
			}
		// Exit on a key press from user
		console->Printf(KTEFEnterKeyMessage);
		console->Getch();
		}
	else
		{
		// Create a Interface class object for generating HTML & XML logs
		CTestExecuteLogger *tefLogger = new(ELeave) CTestExecuteLogger();
		CleanupStack::PushL(tefLogger);

		TInt logMode;
		TInt logLevel;
		TInt remotePanicDetection;
		TBuf<KMaxTestExecuteNameLength> iniSysDrive;

		if (parseTestExecuteIni != NULL)
			{
			// Parse ini for retrieving logging options set through ini
			parseTestExecuteIni->GetKeyValueFromIni(KTEFLogMode, logMode);
			parseTestExecuteIni->GetKeyValueFromIni(KTEFLogSeverityKey, logLevel);
			parseTestExecuteIni->GetKeyValueFromIni(KTEFRemotePanicDetection, remotePanicDetection);
			parseTestExecuteIni->GetKeyValueFromIni(KTEFDefaultSysDrive, iniSysDrive);
			parseTestExecuteIni->GetKeyValueFromIni(KTEFWaitForLoggingTime, waitForLoggingTime);
			parseTestExecuteIni->GetKeyValueFromIni(KTEFHtmlKey, htmlLogPath);
			}
		else
			{
			// Set default values for logging options if parser is not functioning
			logMode = TLoggerOptions(ELogHTMLOnly);
			logLevel = RFileFlogger::TLogSeverity(ESevrAll);
			remotePanicDetection = 0;
			iniSysDrive.Copy(KTEFIniSysDrive);
			waitForLoggingTime = 5;
			htmlLogPath.Copy(KTestExecuteLogPath);
			htmlLogPath.Replace(0, 2, defaultSysDrive);
			}
		
		tefLogger->SetLoggerOptions(logMode);
		if(pipe)
			{
			tefLogger->SetPipeName(pipeName) ; 
			}
		
		// Initialise the logging passing in the script file path & log level to the interface
		tefLogger->InitialiseLoggingL(scriptFilePath, separateLogFiles, logLevel);

		// Check to see if defaultsysdrive key is set in testexecute.ini
		// if set to SYSDRIVE, assign the default system drive obtained from the plugin
		// else, if a true value is set in testexecute.ini, use it as system drive for all test artifacts
		if (iniSysDrive.Length() == 2 && iniSysDrive.Right(1).Compare(KTEFColon) == 0)
			testSysDrive.Copy(iniSysDrive);
		else if (iniSysDrive.CompareF(KTEFIniSysDrive) == 0)
			testSysDrive.Copy(defaultSysDrive);


		// Pass the first command line argument to the script master
		// which is always the command script
		CScriptMaster* scriptMaster = new (ELeave) CScriptMaster(scriptFilePath,*tefLogger, consoleLogger, defaultSysDrive, testSysDrive, selTestingOptions);
		// To kick the state machine of the script master off -
		// Call the kick method which jumps us into the RunL() of the CScriptMaster class
		// CScriptMaster is the top AO in the hierarchy.
		scriptMaster->Kick();

		// Construct and Install a test watcher object for capturing remote panics during test execution
		CTestWatcher* testWatcher = NULL;
		if (remotePanicDetection != 0)
			{
			testWatcher = CTestWatcher::NewL();
			testWatcher->StartL();
			}

		// Enter the Active Scheduler
		aScheduler->Start();
		
		// Cleanup
		delete scriptMaster;
		
		TInt commentedCommandsCnt=CScriptControl::commentedCommandsCount;
		if(commentedCommandsCnt==-1)
			{
			CScriptControl::commentedCommandsCount=0;
			//If the path specified fails check out for path from testexecute.ini
			if(ret!=KErrNotFound)
				{
				//To get scriptFile name  i.e get sampleFile.script
				TInt posOfLastSlash=scriptFilePath.LocateReverse('\\') ;
				scriptFilePath.Set(scriptFilePath.Mid(posOfLastSlash+1));
				TBuf<KBuffSize> tempStore(scriptFilePath);
				if (parseTestExecuteIni != NULL)
					{
					TBuf<KMaxTestExecuteNameLength> tempBuffer;
					parseTestExecuteIni->GetKeyValueFromIni(KTEFDefaultScriptPath, tempBuffer);
					if (tempBuffer.CompareF(KNull) != 0)
						{
						scriptFilePath.Set(tempBuffer);
						TBuf<KBuffSize> tempStoreScriptPath(scriptFilePath);
						tempStoreScriptPath.Append(tempStore);
						scriptFilePath.Set(tempStoreScriptPath);
						TBuf<KMaxTestExecuteNameLength> scriptFileLocation(scriptFilePath);
						CScriptMaster* scriptMaster = new (ELeave) CScriptMaster(scriptFilePath,*tefLogger, consoleLogger, defaultSysDrive, testSysDrive, selTestingOptions);
						// To kick the state machine of the script master off -
						// Call the kick method which jumps us into the RunL() of the CScriptMaster class
						// CScriptMaster is the top AO in the hierarchy.
						scriptMaster->Kick();
						// Enter the Active Scheduler
						aScheduler->Start();
						
						commentedCommandsCnt=CScriptControl::commentedCommandsCount;
						if(commentedCommandsCnt==-1)
							{
							CScriptControl::commentedCommandsCount=0;
							}
							
						// Cleanup
						delete scriptMaster;
						}
					}
				}
			}
		delete selTestingOptions;
		
		TInt commandsCount = CScriptControl::commentedCommandsCount;
		TInt countOfRemotePanics = 0;

		// Stop and Process the test watcher object for extracting panic informations and print them to the log files
		if (remotePanicDetection != 0)
			{
			testWatcher->Stop();
			countOfRemotePanics = testWatcher->iSharedData->iPanicDetails.Count();
			if (countOfRemotePanics > 0)
				{
				tefLogger->LogExtra((TText8*)__FILE__,__LINE__,ESevrErr,
					_L("The panic detection thread detected %d panics:"), countOfRemotePanics);
				for (TInt count = 0; count < countOfRemotePanics; count++)
					tefLogger->LogExtra((TText8*)__FILE__,__LINE__,ESevrErr,_L("Remote Panic %d - Name of Panicked Thread: %S, Exit Reason: %d, Exit Category %S"), count+1, &(testWatcher->iSharedData->iPanicDetails)[count]->iThreadName,(testWatcher->iSharedData->iPanicDetails)[count]->iReason,&(testWatcher->iSharedData->iPanicDetails)[count]->iCategory);
				}
			delete testWatcher;
			}

		// Call the Termination routine for logging within the interface
		tefLogger->TerminateLoggingL(commandsCount, countOfRemotePanics, CScriptControl::iRunScriptFailCount);
		CleanupStack::Pop(tefLogger);
		delete tefLogger;
		}
	if(includeSelectiveCases || excludeSelectiveCases)
		{	
		CleanupStack::PopAndDestroy(selTestCfgFileData);
		selTestCfgFileData = NULL;
		}
	// Close the parser instance if it is active
	CleanupStack::PopAndDestroy(console);
	if (parseTestExecuteIni != NULL)
		{
		CleanupStack::PopAndDestroy(parseTestExecuteIni);
		}

	if (scriptFilePath.CompareF(KNull)!=0)
		{
		// Wait for flogger to write to file
		_LIT(KHtmExtension,".htm");
		TParse fileNameParse;
		fileNameParse.Set(scriptFilePath, NULL, NULL);
		
		TPtrC fileName = fileNameParse.Name();
		htmlLogPath.Append(fileName);
		htmlLogPath.Append(KHtmExtension);

	#ifdef _DEBUG
		RDebug::Print(_L("Log file path--> %S"), &htmlLogPath);
	#endif
		RFs fs;
		fs.Connect();
		TBool answer = EFalse;
		while(ETrue)
			{
			TInt err = fs.IsFileOpen(htmlLogPath, answer);
			if ((KErrNone==err&&!answer) || KErrNotFound==err || KErrPathNotFound==err) 
				{
				break;
				}
			User::After(100000);
			}
		
		if (waitForLoggingTime > 0)
			{
			User::After(waitForLoggingTime*1000000);
			}
		}
	}

/**
 * Top level function inside the TRAP harness
 * TEF system calls for:
 *		Active scheduler initialisation
 *		Command line read and parse
 *		Console creation
 *		Logging Initialisation
 *		Entry to script engine
 *		Logging termination
 * @param aSysDrive - Default system drive letter to be used for all script parsing
 */
LOCAL_C void MainL(const TDriveName aSysDrive)
	{
#if (defined __DATA_CAGING__)
	// Platform security hooks
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().SecureApi(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched = new (ELeave) CActiveScheduler();
	CleanupStack::PushL(sched);
	CActiveScheduler::Install(sched);

	// Call up the subroutine to perform pre-processing tasks and to start test execution
	ProcessMainL(sched, aSysDrive);

	// Cleanup the scheduler object
	CleanupStack::PopAndDestroy(sched);
	}

#if !(defined TEF_LITE)
LOCAL_C void StartSystemL()
	{
	CActiveScheduler* theScheduler = new (ELeave) CActiveScheduler();
	CleanupStack::PushL(theScheduler);
	CActiveScheduler::Install(theScheduler);
	
	RLibrary pluginLibrary;
	CWrapperUtilsPlugin* plugin = TEFUtils::WrapperPluginNew(pluginLibrary);
	
	if (plugin!=NULL)
		{
		plugin->WaitForSystemStartL();
		delete plugin;
		pluginLibrary.Close();
		}
	else
		{
		User::Leave(KErrGeneral);
		}
	
	CleanupStack::PopAndDestroy(theScheduler);
	}
#endif

/**
 * Executable Entry Point
 * Top level always creates TRAP harness.
 * Calls MainL() inside the TRAP harness
 */
GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}

  // Fix PDEF124952, set the priority EPriorityAbsoluteHigh. In this case, the timeout will 
  // take effect even if the server executes with almost 100% CPU usage.
  RThread().SetPriority(EPriorityAbsoluteHigh);
  // End PDEF124952
  
	// Check to see if the plugin wrapper around the GetSystemDrive is loadable
	// If yes, then instantiate the wrapper object and obtain the default system drive
	// Else, use the hardcoded default drive as c:
	TDriveName defaultSysDrive(KTEFLegacySysDrive);	
	
	RLibrary pluginLibrary;
	CWrapperUtilsPlugin* plugin = TEFUtils::WrapperPluginNew(pluginLibrary);
	
	if (plugin!=NULL)
		{
		TDriveUnit driveUnit(plugin->GetSystemDrive());
		defaultSysDrive.Copy(driveUnit.Name());
		delete plugin;
		pluginLibrary.Close();
		}
	
	TBool enableSysStart = ETrue;
	CTestExecuteIniData* iniData = NULL;
	TRAPD(err, iniData = CTestExecuteIniData::NewL(defaultSysDrive));
	if (err == KErrNone)
		{
		// Extract all the key values within the object
		iniData->ExtractValuesFromIni();
		iniData->GetKeyValueFromIni(KTEFSystemStarter, enableSysStart);
		}

	err = KErrNone;
	#if !(defined TEF_LITE)
	if (enableSysStart)
		{
		TRAP(err, StartSystemL());
		__ASSERT_ALWAYS(!err, User::Panic(KTestExecuteName,err));
		}
	#endif
	if (iniData != NULL)
		{
		delete iniData;
		}	
	
	err = KErrNone;
	TRAP(err,MainL(defaultSysDrive));
	__ASSERT_ALWAYS(!err, User::Panic(KTestExecuteName,err));
	delete cleanup;
	__UHEAP_MARKEND;
	return KErrNone;
    }

