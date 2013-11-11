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
* Design Overview:
* The diagram below shows the Script Engine classes and their parent child
* hierarchy. All classes have a CActive base, and apart from CTaskTimer, all classes
* have their own state machine. Apart from CScriptMaster, all classes have a reference
* to a Mixin/Interface completion call of its parent, which it calls when the object
* needs to notify the parent of an event.
* CScriptMaster (derived CActive)
* CScriptControl (derived CActiveBase - Can go recursive)
* CProgramControl (derived CTaskControlBase) CClientControl ( derived CTaskControlBase maps to n x test steps)
* CTaskTimer                                 CTaskTimer (derived CTimer)
*  state transition tables for CScriptMaster, CScriptControl & CClientControl
* CActiveBase:
* Derives CActive.
* Contains code for priming, triggering and completing AO's
* Implements the pure abstract MChildCompletion::ChildCompletion() method
* CTaskControlBase:
* Derives CActiveBase.
* Abstract class. Base for CClientControl and CProgramControl.
* CScriptMaster:
* Derives CActiveBase.
* Master control active object with simple state machine.
* Instantiates the top level CScriptControl object and triggers its state machine.
* CScriptContol:
* Derives CActiveBase.
* Main script interpreter state machine. Creates one or more CClientControl and/or
* CProgramControl objects.
* In CONCURRENT mode, it can create as many instances as there are RUN_TEST_STEP and/or 
* RUN_PROGRAM calls.
* Creates root sessions with the xxxtest servers using the RTestServ::Connect() client call.
* In the case of non-nested scripts, the parent object is CScriptMaster.
* When scripts are nested and it goes recursive, the parent object could be another 
* CScriptControl.
* Implements the MTaskCompletion pure abstract class for callbacks from
* CClientControl or CProgramControl objects
* Calls back into its parent object when the script is complete and all async commands have
* completed.
* CClientControl:
* Derives CTaskControlBase.
* Test Step Controler. Instantiated by CScriptControl when a RUN_TEST_STEP line is interpreted
* Creates and kicks off a CTaskTimer object set to the timeout value for the test step.
* Opens a test step using the RTestSession::Open() client call.
* Executes a test step using the RTestSession::RunTestStep() client call.
* This method issues the client RTestSession::AbortTestStep() call and
* the RunL() state machine handles the completion from the test server.
* Implements the MTaskTimerCompletion pure abstract class for callbacks from the
* CTaskTimer class.
* Calls back into its parent CScriptControl object when a test step completes.
* CProgramControl
* Derives CTaskControlBase
* Kicks off an executable in its own process and handles process completion asynchronously
* CTaskTimer:
* Derives CTimer.
* Calls back into its parent object if the timer expires.
* EPOC include
*
*/



/**
 @file scriptengine.cpp
*/

// User include
#include <test/testexecuteclient.h>
#include "scriptengine.h"

// Fix defect 119337, initialize the integer to zero
GLDEF_D TInt CScriptControl::iNestedNumRunScriptInLoop=0;
// End defect 119337

/**
 * @param aPtrC1 - Instance of TPtrC to compare
 * @param aPtrC2 - Instance of TPtrC to compare
 * Function to implement the comparison algo for RArray::Find to work with
 */
TBool TSelectiveTestingOptions::CompareTPtrC(const TPtrC& aPtrC1, const TPtrC& aPtrC2)
	{
	TInt ret =aPtrC1.CompareF(aPtrC2);
	return (ret==0) ? ETrue : EFalse;
	} 

/**
 * @param aRng1 - Instance of TRange to compare
 * @param aRng2 - Instance of TRange to compare
 * Function to implement the comparison algo for RArray::Find to work with.
 * TRange::iStartTestCase determines the match
 */
TBool TRange::CompareTRangeStartCase(const TRange& aRng1, const TRange& aRng2) 
	{
	TInt ret =aRng2.iStartTestCase.CompareF(aRng1.iStartTestCase);
	return (ret==0) ? ETrue : EFalse;
	}

/**
 * @param aRng1 - Instance of TRange to compare
 * @param aRng2 - Instance of TRange to compare
 * Function to implement the comparison algo for RArray::Find to work with.
 * TRange::iEndTestCase determines the match
 */
TBool TRange::CompareTRangeEnd(const TRange& aRng1, const TRange& aRng2) 
	{
	TInt ret =aRng2.iEndTestCase.CompareF(aRng1.iEndTestCase);
	return (ret==0) ? ETrue : EFalse;  
	}

/**
 * @param aRng1 - Instance of TRange to compare
 * @param aRng2 - Instance of TRange to compare
 * Function to implement the comparison algo for RArray::Sort to work with.
 * TRange::iStartTestCase is used as the sort key
 */
TInt TRange::CompareTRangeStartOrder(const TRange& aRng1, const TRange& aRng2) 
	{
	return aRng1.iStartTestCase.CompareF( aRng2.iStartTestCase);
	}

/**
 * @param aPtrC1 - Instance of TPtrC to compare
 * @param aPtrC2 - Instance of TPtrC to compare
 * Function to implement the comparison algo for RArray::Sort to work with.
 */
TInt TSelectiveTestingOptions::CompareTPtrCOrder(const TPtrC& aPtrC1, const TPtrC& aPtrC2) 
	{
	return aPtrC1.CompareF(aPtrC2);
	}

/**
 * @param aServ - Instance of the test server handle
 * @param aStepName - Test step name
 * Wrapper around the RTestServ class. Performs counting on test step sessions
 */
TInt RScriptTestSession::Open(RScriptTestServ& aServ, const TBool aIsTestStep, const TDesC& aStepName )
	{
	if(aServ.SharedData() && aServ.SessionCount())
			return KErrInUse;
	TInt ret = KErrNone;
	
	if( aIsTestStep )
		{
		ret = RTestSession::Open(aServ, aStepName, aServ.SharedData());
		}
	else
		{
		ret = RTestSession::Open(aServ, aServ.SharedData());
		}
	if(ret)
		return ret;
	aServ.AddSession();
	iServ = &aServ;
	return KErrNone;
	}
	
/**
 * @param aScriptFilePath - The full path and filename of a script command file.
 * @param aLogger - Reference to a logger interface object that contains HTML & XML log client sessions.
 * @param aConsole - Reference to console object for printing script line during test execution
 * @param aSysDrive - Default System drive letter
 * @param aTestSysDrive - Default System drive letter overwritten through testexecute.ini
 * Constructor
 */
CScriptMaster::CScriptMaster(const TDesC& aScriptFilePath, CTestExecuteLogger& aLogger, RConsoleLogger& aConsole, const TDriveName& aSysDrive, const TDriveName& aTestSysDrive, TSelectiveTestingOptions* aSelTestingOptions ) :
	iState(EInit), iScriptFilePath(aScriptFilePath), iLogger(aLogger), iConsoleLogger(aConsole), iDefaultSysDrive(aSysDrive), iTestSysDrive(aTestSysDrive), iSelTestingOptions(aSelTestingOptions)
	{
	}

/**
 * Destructor
 */
CScriptMaster::~CScriptMaster()
	{
	}

/**
 * Pure virtual implementation.
 * The Top level state machine Kick'd() into by MainL()
 * Picks up the completion from a CScriptControl instance then exits the scheduler
 */
void CScriptMaster::RunL()
	{
	switch (iState)
		{
	case EInit	:
		{
		// Create the master CScriptControl instance. 
		CScriptControl* scriptControl = new (ELeave) CScriptControl(*this,iScriptFilePath,Logger(),ConsoleLogger(),iStartLooping,iLoop,iDefaultSysDrive,iTestSysDrive,iSelTestingOptions);
		iState = ERunning;
		// Set our AO up ready for completion
		Prime();
		// Kick the CScriptControl state machine
		scriptControl->Kick();
		}
		break;
	case ERunning	:
		{
		// All child AO's have completed and been deleted so it's safe to exit.
		CActiveScheduler::Stop();
		}
		break;
	default:
		break;
		}
	}

/**
 * @param aCompletion - Callback into the parent object.
 * @param aScriptFilePath - The full path and filename of a script file
 * @param aLogger - Reference to a logger instance
 * @param aConsole - Reference to console object for printing script line during test execution
 * @param aStartLooping - Initiate the looping
 * @param aLoop - Check for nesting of loops
 * @param aSysDrive - Default System drive letter
 * @param aTestSysDrive - System drive letter overwritten from testexecute.ini
 * Constructor
 */
CScriptControl::CScriptControl(MChildCompletion& aCompletion, const TDesC& aScriptFilePath, CTestExecuteLogger& aLogger, RConsoleLogger& aConsole, TBool aStartLooping, TBool aLoop, const TDriveName& aSysDrive, const TDriveName& aTestSysDrive, TSelectiveTestingOptions* aSelTestingOptions):
	iScriptFile(aScriptFilePath),
	iState(EInit),
	iParent(aCompletion),
	iConcurrent(EFalse),
	iCanComplete(ETrue),
	iBreakOnError(EFalse),
	iAsyncTasksOutstanding(0),
	iCurrentScriptLineNumber(0),
	iLogger(aLogger),
	iConsoleLogger(aConsole),
	iScriptLinePrefixSet(EFalse),
	iStartLooping(aStartLooping),
	iLoop(aLoop),
	iSharedDataNum(KTEFZeroValue),
	iIsSharedData(EFalse),
	iSyncControl(NULL),
	iTestCaseID(KTEFTestCaseDefault),
	iDefaultSysDrive(aSysDrive),
	iTestSysDrive(aTestSysDrive),
	iSelTestingOptions(aSelTestingOptions),
	iSelectOne(EFalse),
	iRangeRefCounter(0),
	iTestCaseCounter(0)
	{
	}

/**
 * Destructor
 */
CScriptControl::~CScriptControl()
	{
	iTimer.Cancel();
	iTimer.Close();
	// We read the complete script into the heap for parsing so:
	delete iScriptData;
	for (TInt index = 0; index < iSharedDataNum; index++)
		{
		delete iSharedDataArray[index];
		}
	// Instance has an array of pointers to RTestServer objects.
	// Loop through deleting and closing
	TInt i = 0;
	TInt count = iServers.Count();
	for(i=0;i<count;i++)
		{
		iServers[0]->Close();
		delete iServers[0];
		iServers.Remove(0);
		}
	iServers.Close();
	if( iSyncControl )
		{
		delete iSyncControl;
		}
	}

/**
 * @param aError - Integer error value returned by the active object while leaving
 * @return TInt - System-wide error codes
 * Error Handler for active object
 */
TInt CScriptControl::RunError(TInt aError)
	{
	if (aError == KErrNoMemory)
		{
		ERR_PRINTF1(_L("Insufficient memory available to perform further operation.\n\tPlease increase the maximum heap size from the testexecute.mmp and try running the test again. Exiting test script..."));
		}
	else
		{
		ERR_PRINTF2(_L("Test execution failed with error %d. Terminating tests.."), aError);
		}
	iParent.ChildCompletion(KErrNone);
	delete this;
	return KErrNone;
	}

/**
 * Implementation of pure virtual
 * The main script interpreter state machine.
 * Kick'd() into by CScriptMaster or, in the case of nested scripts, another CScriptControl
 * instance.
 * Picks up the completions from CClientControls and/or, in the case of nested scripts,
 * another CScriptControl instance.
 */
void CScriptControl::RunL()
	{
	switch (iState)
		{
	case EInit		:
		// First state after Kick() from parent
		{
		// Standard log output goes to flogger
		// Result log goes to a propietary file in c:\logs\testexecute
		User::LeaveIfError(iTimer.CreateLocal());
		TRAPD(err,CreateScriptDataFromScriptFileL());
		if(err)
			{
			// If we can't open the script file then log the fact and gracefully exit
			// the state machine.
			TBuf<KMaxTestExecuteCommandLength> buf(iScriptFile);
			_LIT(KScriptFileError,"Failed Script File Open %S");
			ERR_PRINTF2(KScriptFileError,&buf);
			iState = EClosing;
			//condition used for checking failure in testexecute.cpp
			commentedCommandsCount=-1;
			iRunScriptFailCount++;
			Kick();
			break;
			}

		// Script file reading state next
		iState = ERunning;
		// Kick ourselves into the next state
		Kick();
		}
		break;

	case ERunning	:
		// Always in this state whilst we're reading lines from script file data member
		{
		TPtrC scriptLine;
		if(!iStartLooping)
			{
			if(!GetNextScriptLine(scriptLine))
				{
				// End of the script file
				// Check see if there are any async requests outstanding
				// In concurrent mode there are very likely to be
				if(iAsyncTasksOutstanding == 0)
					{
					// No requests outstanding so call into the parent
					iParent.ChildCompletion(KErrNone);
					delete this;
					}
				else
					{
					// Requests outstanding
					// Next time we're completed we'll be in the closing state
					iState = EClosing;
					iCanComplete =ETrue;
					// Prime ourselves for completion
					Prime();
					}
				break;
				}
			}
		//Get the script lines which are to be looped
		else if(!GetLoopScriptLine(scriptLine))
			{
			// End of the script file
			// Check see if there are any async requests outstanding
			// In concurrent mode there are very likely to be
			if(iAsyncTasksOutstanding == 0)
				{
				// No requests outstanding so call into the parent
				iParent.ChildCompletion(KErrNone);
				delete this;
				}
			else
				{
				// Requests outstanding
				// Next time we're completed we'll be in the closing state
				iState = EClosing;
				iCanComplete =ETrue;
				// Prime ourselves for completion
				Prime();
				}
			break;
			}

		iCurrentScriptLine.Set(scriptLine);
		
		TBool commentedCommand = CheckCommentedCommands();
		if(commentedCommand)
			{
			iCanComplete = EFalse;
			Kick();
			break;
			}

		if (iScriptLinePrefixSet )
			{
			TLex lookahead(iCurrentScriptLine);

			TPtrC firstCommand(lookahead.NextToken());

			if (firstCommand.CompareF(KTEFRemovePrefixCommand) != 0)
				{
				// If we aren't the Remove_Prefix command, prefix the current line...
				iPrefixedCurrentScriptLine = iScriptLinePrefix;
				iPrefixedCurrentScriptLine.Append(scriptLine);
				iCurrentScriptLine.Set(iPrefixedCurrentScriptLine);
				}
			}

		TRAPD(err, MakeAbsoluteFilePathsL(iCurrentScriptLine));
		PrintCurrentScriptLine();
		
		TLex lex(iCurrentScriptLine);

		TPtrC token(lex.NextToken());

		if (err == KTEFErrInvalidRelPath)
			{
			_LIT(KTEFErrInvalidRelPathText,"Invalid relative path provided in the script file. Skipping the script line from execution..");
			ERR_PRINTF1(KTEFErrInvalidRelPathText);

			if (token.CompareF(KTEFRunTestStepCommand) == 0 ||
				token.CompareF(KTEFRunPanicStepCommand) == 0 ||
				token.CompareF(KTEFRunTestStepResultCommand) == 0 ||
				token.CompareF(KTEFRunPanicStepResultCommand) == 0)
				{
				TExitCategoryName  blankPanicString; //Not a Panic
				LogResult(EIgnore, blankPanicString, iCurrentScriptLineNumber, iCurrentScriptLine);
				}
			iCanComplete = EFalse;
			Kick();
			break;
			}

		if(err == KErrTooBig)
			{
			_LIT(KTEFErrTooBigArguments, "One or more arguments for the command exceeded allowed limit for length. Skipping test..");
			ERR_PRINTF1(KTEFErrTooBigArguments);

			if (token.CompareF(KTEFRunTestStepCommand) == 0 ||
				token.CompareF(KTEFRunPanicStepCommand) == 0 ||
				token.CompareF(KTEFRunTestStepResultCommand) == 0 ||
				token.CompareF(KTEFRunPanicStepResultCommand) == 0)
				{
				TExitCategoryName  blankPanicString; //Not a Panic
				LogResult(EIgnore, blankPanicString, iCurrentScriptLineNumber, iCurrentScriptLine);
				}
			iCanComplete = EFalse;
			Kick();
			break;
			}

		// Main parser
		if(token.CompareF(KTEFLoadSuiteCommand) == 0 || token.CompareF(KTEFLoadServerCommand) == 0)
			{
			TRAPD(err,CreateServerFromScriptLineL());
			
			// Create a TLogField structure array
			// Size of array equals to number of fields to be displayed for the command
			TExtraLogField logField[2];

			// The first member of the structure stores the field name
			// The second one holds the value for the particular field
			_LIT(KSuiteName, "SUITE_NAME");
			logField[0].iLogFieldName.Copy(KSuiteName);
			logField[0].iLogFieldValue.Copy(lex.NextToken());
			
			logField[1].iLogFieldName.Copy(KTEFResultString);
			if(err != KErrNone)
				{
				logField[1].iLogFieldValue.Copy(KTEFResultFail);
				if( KErrNotFound == err )
					{
					_LIT(KServerNotFound,"Failed to create server, either the server or one of its dependancies could not be found.");
					ERR_PRINTF1(KServerNotFound);
					}
				else
					{
					_LIT(KServerCreateError,"Failed to Create Server Err = %d");
					ERR_PRINTF2(KServerCreateError,err);
					}
				}
			else
				{
				logField[1].iLogFieldValue.Copy(KTEFResultPass);
				}
			
			// Call the Logger().LogToXml routine which handles XML logging for individual commands
			// Takes in the command name, number of fields and the struture array
			Logger().LogToXml(((TText8*)__FILE__), __LINE__, RFileFlogger::ESevrMedium, token, 2, logField);
			iCanComplete = EFalse;
			Kick();
			}
		else if(token.CompareF(KTEFStartTestBlock)==0)
			{
			// Parse the START_TEST_BLOCK command line
			TInt index = 0;
			TBool taskCanComplete = ETrue;
			TBool concurrent = iConcurrent;
			// Make sure the server is loaded
			if(!GetServerIndexFromScriptLine(index))
				{
				// Not loaded. Skip the line, but ensure its logged as a failure.
				TExitCategoryName  blankPanicString; //Not a Panic
				LogResult(EIgnore, blankPanicString, iCurrentScriptLineNumber, iCurrentScriptLine);

				iCanComplete = EFalse;
				Kick();
				break;
				}
			
			TBuf<KMaxTestExecuteCommandLength>	startBlockLine = iCurrentScriptLine;
			
			// Parse the test block of commands
			TTEFItemArray* itemArray = new (ELeave) TTEFItemArray(1);
			CleanupStack::PushL( itemArray );
			TRAPD( err, ParseTestBlockL(*itemArray) );
			
			if( KErrNone == err )
				{
				if(!iConcurrent)
					{
					// If we're not in concurrent mode then child objects can complete us
					taskCanComplete = ETrue;
					// Prime ready for completion
					Prime();
					}
				else
					{
					// In concurrent mode children can't complete us as we kick() ourselves.
					taskCanComplete = EFalse;
					Kick();
					}
				// Create the test block controler object
				TInt loopIndex = -1;
				if (iLoop)
					{
					loopIndex = iLoopCounter + 1;
					}
				
				CClientControl* blockController = CClientControl::NewL(	*iServers[index],
																		startBlockLine,
																		*this,
																		iCurrentScriptLineNumber,
																		Logger(),
																		loopIndex,
																		iTestCaseID,
																		iCurrentScriptLine,
																		iScriptFile,
																		*itemArray,
																		iDefaultSysDrive,
																		iTestSysDrive);

				blockController->SetTaskComplete(taskCanComplete);
				iAsyncTasksOutstanding++;
				// Kick() the test step  object into its state machine
				blockController->Kick();
				}
			else
				{
				iCanComplete = EFalse;
				Kick();				
				}

			iConcurrent = concurrent;
			CleanupStack::PopAndDestroy( itemArray );
			}
		else if(token.CompareF(KTEFEndTestBlock)==0)
			{
			// If this is called then there is a missing START_TEST_BLOCK command
			TExitCategoryName  blankPanicString;
			LogResult(EIgnore, blankPanicString, iCurrentScriptLineNumber, iCurrentScriptLine);
			iCanComplete = EFalse;
			Kick();
			}
		else if(token.CompareF(KTEFStartRepeat)==0)
			{
			TPtrC iniFile(lex.NextToken());
			TPtrC iniSection(lex.NextToken());
			TPtrC tempRepeatParam=lex.NextToken();
			CIniData* configData = NULL;
			TInt err = 0;
			if(iniFile.Length())
				{
				TRAP(err,configData = CIniData::NewL(iniFile));
				}
			if(err != KErrNone)
				{
				_LIT(KTEFIniFileNotFound,"Ini file not found.. Looping Ignored");
				ERR_PRINTF1(KTEFIniFileNotFound);
				Kick();
				break;
				}
			iRepeatParam=0;
			CleanupStack::PushL(configData);
			if(configData)
				{
				//For Syntax Error  continue ignoring looping
				if (!configData->FindVar(iniSection, tempRepeatParam, iRepeatParam))
					{
					_LIT(KIniFailMessage,"The input data is not found in the ini specified");
					INFO_PRINTF1(KIniFailMessage);
					CleanupStack::PopAndDestroy(configData);
					Kick();
					break;
					}
				}
				
			CleanupStack::PopAndDestroy(configData);
			TExtraLogField logField[1];
			_LIT(KIterations,"ITERATIONS");
			logField[0].iLogFieldName.Copy(KIterations);
			logField[0].iLogFieldValue.Copy(KNull);
			logField[0].iLogFieldValue.AppendNum(iRepeatParam);
			Logger().LogToXml(((TText8*)__FILE__), __LINE__, RFileFlogger::ESevrMedium, token, 1, logField);

			if(tempRepeatParam.Compare(KNull)==0)
				{
				_LIT(KRepeatKeyNotFound,"Repeat Parameter Key Not Found");
				INFO_PRINTF1(KRepeatKeyNotFound);
				Kick();
				break;
				}
			//Nesting of Control Logic is Not supported
			if(iLoop)
				{
				_LIT(KNestingNotAllowed,"Nesting of START_REPEAT is not supported.. Looping Ignored");
				WARN_PRINTF1(KNestingNotAllowed);
				iLoop=EFalse;
				Kick();
				break;
				}
			//Looping is not to be started with Concurrent mode
			if(iConcurrent)
				{
				_LIT(KConcurrentNotAllowed,"No concurrent Execution is Allowed in Looping");
				INFO_PRINTF1(KConcurrentNotAllowed);
				Kick();
				break;
				}
				
			iLoopCounter=0;
			//For Invalid Parameter continue ignoring looping
			if(iRepeatParam<1)
				{
				_LIT(KInvalidRepeatParam,"The repeat Parameter is invalid");
				INFO_PRINTF1(KInvalidRepeatParam);
				Kick();
				break;
				}
			iLoop=ETrue;
			iCheckVar=EFalse;
			Kick();
			}
		else if(token.CompareF(KTEFEndRepeat)==0)
			{
			if(!iLoop)
				{
				_LIT(KLoopNotInitiated,"The Looping is Not Initiated");
				INFO_PRINTF1(KLoopNotInitiated);
				Logger().LogToXml(((TText8*)__FILE__), __LINE__, RFileFlogger::ESevrMedium, token);
				Kick();
				break;
				}
				
			iLoopCounter++;
			if(iLoopCounter==iRepeatParam)
				{
				Logger().LogToXml(((TText8*)__FILE__), __LINE__, RFileFlogger::ESevrMedium, token);
				iStartLooping=EFalse;
				iLoop=EFalse;
				}
			else
				{
				iStartLooping=ETrue;
				//Looping needs to be started in CONSECUTIVE mode
				if(iConcurrent)
					{
					iConcurrent=EFalse;
					}
				}
			Kick();
			}
		else if(token.CompareF(KTEFRunTestStepCommand) == 0 ||
				token.CompareF(KTEFRunPanicStepCommand) == 0 ||
				token.CompareF(KTEFRunTestStepResultCommand) == 0 ||
				token.CompareF(KTEFRunPanicStepResultCommand) == 0
				)
			{
			if(!CheckValidScriptLine())
				{
				// Not a Valid Script Line

				TExitCategoryName  blankPanicString; //Not a Panic
				LogResult(EIgnore, blankPanicString, iCurrentScriptLineNumber, iCurrentScriptLine);
				// End of defect 037066

				iCanComplete = EFalse;
				Kick();
				break;
				}
			
			TInt index;
			TBool taskCanComplete = ETrue;
			TBool concurrent = iConcurrent;
			// Make sure the server is loaded
			if(!GetServerIndexFromScriptLine(index))
				{
				// Not loaded. Skip the line, but ensure its logged as a failure.

				// Start of defect 037066
				TExitCategoryName  blankPanicString; //Not a Panic
				LogResult(EIgnore, blankPanicString, iCurrentScriptLineNumber, iCurrentScriptLine);
				// End of defect 037066

				iCanComplete = EFalse;
				Kick();
				break;
				}

			if(!iConcurrent)
				{
				// If we're not in concurrent mode then child objects can complete us
				taskCanComplete = ETrue;
				// Prime ready for completion
				Prime();
				}
			else
				{
				// In concurrent mode children can't complete us as we kick() ourselves.
				taskCanComplete = EFalse;
				Kick();
				}
			iConcurrent = concurrent;
			// Create the test step controler object
			TInt loopIndex = -1;
			if (iLoop)
				{
				loopIndex = iLoopCounter + 1;
				}
			CClientControl* stepController = new (ELeave) CClientControl(*iServers[index],iCurrentScriptLine,*this,iCurrentScriptLineNumber,Logger(), loopIndex, iTestCaseID, iScriptFile, iDefaultSysDrive, iTestSysDrive);
			stepController->SetTaskComplete(taskCanComplete);
			iAsyncTasksOutstanding++;
			// Kick() the test step  object into its state machine
			stepController->Kick();
			}
		else if(token.CompareF(KTEFRunProgramCommand) == 0)
			{
			TBool taskCanComplete = ETrue;
			if(!iConcurrent)
				{
				// If we're not in concurrent mode then child objects can complete us
				taskCanComplete = ETrue;
				// Prime ready for completion
				Prime();
				}
			else
				{
				// In concurrent mode children can't complete us as we kick() ourselves.
				taskCanComplete = EFalse;
				Kick();
				}
			// Create the test step controller object
			CProgramControl* programController = new (ELeave) CProgramControl(iCurrentScriptLine,*this,iCurrentScriptLineNumber,Logger());
			programController->SetTaskComplete(taskCanComplete);
			iAsyncTasksOutstanding++;
			// Kick() the test step  object into its state machine
			programController->Kick();
			}
		else if(token.CompareF(KTEFRunWSProgramCommand) == 0)
			{
			TBool taskCanComplete = ETrue;
			if(!iConcurrent)
				{
				// If we're not in concurrent mode then child objects can complete us
				taskCanComplete = ETrue;
				// Prime ready for completion
				Prime();
				}
			else
				{
				// In concurrent mode children can't complete us as we kick() ourselves.
				taskCanComplete = EFalse;
				Kick();
				}
			// Create the test step controller object
			CProgramControl* programController = new (ELeave) CProgramControl(iCurrentScriptLine,*this,iCurrentScriptLineNumber,Logger(),ETrue);
			iAsyncTasksOutstanding++;
			programController->SetTaskComplete(taskCanComplete);
			// Kick() the test step  object into its state machine
			programController->Kick();
			}
		else if(token.CompareF(KTEFConcurrentCommand) == 0)
			{
			// Go into concurrent mode
			// Whilst we're in concurrent mode we always kick() ourselves
			// around the state engine
				iConcurrent = ETrue;
			// Call the Logger()'s LogToXml routine to handle XML logging
			// Takes in just the command name without any field
			Logger().LogToXml(((TText8*)__FILE__), __LINE__, RFileFlogger::ESevrMedium, KTEFConcurrentCommand);
			iCanComplete = EFalse;
			Kick();
			}
		else if(token.CompareF(KTEFConsecutiveCommand) == 0)
			{
			// If we go into consecutive mode we have to make sure there are no
			// requests outstanding.Set the state accordingly
			iConcurrent = EFalse;

			// Call the Logger()'s LogToXml routine to handle XML logging
			// Takes in just the command name without any field
			Logger().LogToXml(((TText8*)__FILE__), __LINE__, RFileFlogger::ESevrMedium, KTEFConsecutiveCommand);
			if(iAsyncTasksOutstanding)
				{
				iCanComplete = ETrue;
				iState = EWaitCompletions;
				Prime();
				}
			else
				{
				iCanComplete = EFalse;
				iState = ERunning;
				Kick();
				}
			}
		else if(token.CompareF(KTEFRunUtilsCommand) == 0)
			{
			// All utils complete synchronously
			TRAPD(err,RunUtilsFromScriptLineL());
			
			// Create a TLogField structure array
			// Size of array equals to number of fields to be displayed for the command
			TExtraLogField logField[2];

			// The first member of the structure stores the field name
			// The second one holds the value for the particular field
			_LIT(KCommand,"COMMAND");
			logField[0].iLogFieldName.Copy(KCommand);
			logField[0].iLogFieldValue.Copy(lex.NextToken());
			
			logField[1].iLogFieldName.Copy(KTEFResultString);
			if (err == KErrNone)
				{
				logField[1].iLogFieldValue.Copy(KTEFResultPass);
				}
			else
				{
				logField[1].iLogFieldValue.Copy(KTEFResultFail);
				}

			// Call the Logger().LogToXml routine which handles XML logging for individual commands
			// Takes in the command name, number of fields and the struture array
			Logger().LogToXml(((TText8*)__FILE__), __LINE__, RFileFlogger::ESevrMedium, KTEFRunUtilsCommand, 2, logField);
			
			if(err != KErrNone)
				{
				_LIT(KRunUtilsError,"RUN_UTILS ret = %d");
				INFO_PRINTF2(KRunUtilsError,err);
				}
			iCanComplete = EFalse;
			Kick();
			}
		else if(token.CompareF(KTEFPrintCommand) == 0)
			{
			PrintFromScriptLine();
			iCanComplete = EFalse;
			Kick();
			}
		else if (token.CompareF(KTEFPrefixCommand) == 0)
			{

			if(iAsyncTasksOutstanding)
				// Don't run Prefix until all outstanding requests have completed
				{
				iState = EPrefixPending;
				iCanComplete = ETrue;
				Prime();
				}
			else
				{
				// Ok to run PREFIX - Kick the stated machine so it's run next time in the RunL()
				iState = ERunPrefix;
				iCanComplete = EFalse;
				Kick();
				}

			}
		else if (token.CompareF(KTEFRemovePrefixCommand) == 0)
			{
			iScriptLinePrefixSet = EFalse;
			iCanComplete = EFalse;
			Kick();
			}
		else if (token.CompareF(KTEFStartTestCaseCommand) == 0) 
			{
			if(ProceedTestCase())
				{	
				LogTestCaseMarkerL();
				iCanComplete = EFalse;
				Kick();	
				}
			else
				{
				iState = ETestCaseIgnore;
				iCanComplete = EFalse;
				Kick();	
				}
			}
		else if (token.CompareF(KTEFEndTestCaseCommand) == 0)
			{
			ProcessEndCase();
			if(iAsyncTasksOutstanding)
				// Don't run END_TESTCASE until all outstanding requests have completed
				{
				iState = EEndTestCasePending;
				iCanComplete = ETrue;
				Prime();
				}
			else
				{
				// Ok to run END_TESTCASE - Kick the stated machine so it's run next time in the RunL()
				iState = ERunEndTestCase;
				iCanComplete = EFalse;
				Kick();
				}
			}
		else if (token.CompareF(KTEFStartSyncTestCaseCommand) == 0) 
			{
			if(ProceedTestCase())
				{
				// Start Synchronised Test Case
				// Check to see if the Sync Data has been created
				// If not then create it
				if( iSyncControl == NULL )
					{
					iSyncControl = CSyncControl::NewL();
					}
				LogTestCaseMarkerL();
				iState = ERunStartSyncTestCase;
				iCanComplete = EFalse;
				Kick();	
				}
			else
				{
				//go into some sleep state until you 
				//encounter an end test case for this...
				iState = ETestCaseIgnore;
				iCanComplete = EFalse;
				Kick();	
				}
			}
		else if (token.CompareF(KTEFEndSyncTestCaseCommand) == 0)
			{
			ProcessEndCase();
			// End Synchronised Test Case
			if(iAsyncTasksOutstanding)
				// Don't run END_SYNCHRONISED_TESTCASE until all outstanding requests have completed
				{
				iState = EEndTestCasePending;
				iCanComplete = ETrue;
				Prime();
				}
			else
				{
				// Ok to run END_SYNCHRONISED_TESTCASE - Kick the stated machine so it's run next time in the RunL()
				iState = ERunEndTestCase;
				iCanComplete = EFalse;
				Kick();
				}
			}
		else if(token.CompareF(KTEFRunScriptCommand) == 0)
			{
			// Create a TLogField structure array
			// Size of array equals to number of fields to be displayed for the command
			TExtraLogField logField[1];

			// The first member of the structure stores the field name
			// The second one holds the value for the particular field
			_LIT(KScriptName,"SCRIPT_NAME");
			logField[0].iLogFieldName.Copy(KScriptName);
			logField[0].iLogFieldValue.Copy(lex.NextToken());
			
			// Call the Logger().LogToXml routine which handles XML logging for individual commands
			// Takes in the command name, number of fields and the struture array
			Logger().LogToXml(((TText8*)__FILE__), __LINE__, RFileFlogger::ESevrHigh, KTEFRunScriptCommand, 1, logField);
			if(iAsyncTasksOutstanding)
				{
				// Don't recursively process a new script until this one's async
				// requests are completed
				iState = ERunScriptPending;
				iCanComplete = ETrue;
				Prime();
				}
			else
				{
				// Ok to process the script recursively
				iState = ERunScript;
				iCanComplete = EFalse;
				Kick();
				}
			}
		else if(token.CompareF(KTEFCedCommand) == 0)
			// Run the CED comms database editor
			{
			if(iAsyncTasksOutstanding)
				// Don't run CED until all outstanding requests have completed
				{
				iState = ERunCedPending;
				iCanComplete = ETrue;
				Prime();
				}
			else
				{
				// Ok to run CED - Kick the stated machine so it's run next time in the RunL()
				iState = ERunCed;
				iCanComplete = EFalse;
				Kick();
				}
			}
		else if(token.CompareF(KTEFDelayCommand) == 0)
			// Delay n milliseconds
			{
			if(iAsyncTasksOutstanding)
				// Don't delay until all outstanding requests have completed
				{
				iState = EDelayPending;
				iCanComplete = ETrue;
				Prime();
				}
			else
				{
				// Ok to delay
				iState = EDelay;
				iCanComplete = EFalse;
				Kick();
				}
			}
		// Script can exit on error
		// Flag is checked on async task completion
		else if(token.CompareF(KTEFBreakErrorOnCommand) == 0)
			{
			// Set the flag and process next line
			iCanComplete = EFalse;
			iBreakOnError = ETrue;

			// Call the Logger()'s LogToXml routine to handle XML logging
			// Takes in just the command name without any field
			Logger().LogToXml(((TText8*)__FILE__), __LINE__, RFileFlogger::ESevrMedium, KTEFBreakErrorOnCommand);
			Kick();
			}
		else if(token.CompareF(KTEFBreakErrorOffCommand) == 0)
			{
			// Reset the flag and process next line
			iCanComplete = EFalse;
			iBreakOnError = EFalse;

			// Call the Logger()'s LogToXml routine to handle XML logging
			// Takes in just the command name without any field
			Logger().LogToXml(((TText8*)__FILE__), __LINE__, RFileFlogger::ESevrMedium, KTEFBreakErrorOffCommand);
			Kick();
			}
		// We only implement the pause command if JustInTime debugging is switched on
		else if(token.CompareF(KTEFPauseCommand) == 0 && User::JustInTime())
			{
			// Create a TLogField structure array
			// Size of array equals to number of fields to be displayed for the command
			TExtraLogField logField[1];

			// The first member of the structure stores the field name
			// The second one holds the value for the particular field
			_LIT(KDelay,"DELAY");
			logField[0].iLogFieldName.Copy(KDelay);
			logField[0].iLogFieldValue.Copy(lex.NextToken());

			// Call the Logger().LogToXml routine which handles XML logging for individual commands
			// Takes in the command name, number of fields and the struture array
			Logger().LogToXml(((TText8*)__FILE__), __LINE__, RFileFlogger::ESevrLow, KTEFPauseCommand, 1, logField);

			if(iAsyncTasksOutstanding)
				// Don't pause until all outstanding requests have completed
				{
				iState = EPausePending;
				iCanComplete = ETrue;
				Prime();
				}
			else
				{
				// Ok to Pause
				iState = EPause;
				iCanComplete = EFalse;
				Kick();
				}
			}
		// Handles the shared comand and also creates the shared object
		// on reading user inputs from ini file
		else if(token.CompareF(KTEFSharedDataCommand) == 0)
			{
			if (iIsSharedData)
				{
				WARN_PRINTF1(KTEFSharedDataCommandRepeated);
				}
			else
				{
				TRAPD(err,CreateSharedObjectsFromScriptLineL());
				if (err != KErrNone)
					{
					ERR_PRINTF1(KTEFErrInCreatingSharedObjects);
					}
				}
			iCanComplete = EFalse;
			Kick();
			}
		else
			{
			// Command not implemented or a comment line
			// Code implemented for defect 047340
			TBuf<KMaxTestExecuteCommandLength> bufWarning;
			if(token.Length())
				{
				TInt firstChar = iCurrentScriptLine[0];
				if(firstChar != '\r' && firstChar != '\n' && firstChar != '#' && firstChar != '/' && token.CompareF(KTEFPauseCommand) != 0)
					{
					_LIT(KUnrecognised,"Unrecognised Command - %S");
					if(token.Length() < bufWarning.MaxLength())
						{
						bufWarning.Copy(token);
						WARN_PRINTF2(KUnrecognised,&bufWarning);
						}
					else
						{
						_LIT(KLineTooLong,"Command line too long");
						bufWarning.Copy(KLineTooLong);
						WARN_PRINTF2(KUnrecognised,&bufWarning);
						}
					}
				}
			iCanComplete = EFalse;
			Kick();
			}
		}
		break;

	case EClosing :
		// Script has been processed
		// Pick up the completions
		{
		if(iAsyncTasksOutstanding == 0)
			{
			// Script finished
			// Call into the parent
			iParent.ChildCompletion(KErrNone);
			delete this;
			}
		else
			{
			// More requests to complete
			iCanComplete = ETrue;
			Prime();
			}
		}
		break;

	case ERunScriptPending :
	case EWaitCompletions :
	case ERunCedPending :
	case EDelayPending :
	case EPausePending :
	case EEndTestCasePending :
	case EPrefixPending:
		// We go into this state if we're waiting for RUN_TEST_STEP's
		// to complete before we execute another command
		{
		if(iAsyncTasksOutstanding == 0)
			{
			// All steps complete
			// Set up the next state and kick() the state machine
			if(iState == ERunScriptPending)
				iState = ERunScript;
			else if(iState == EWaitCompletions)
				iState = ERunning;
			else if(iState == ERunCedPending)
				iState = ERunCed;
			else if(iState == EDelayPending)
				iState = EDelay;
			else if(iState == EPausePending)
				iState = EPause;
			else if(iState == EEndTestCasePending)
				iState = ERunEndTestCase;
			else if(iState == EPrefixPending)
				iState = ERunPrefix;
			// Safe to Kick() the state machine again
			iCanComplete = EFalse;
			Kick();
			}
		else
			{
			// More requests outstanding
			iCanComplete = ETrue;
			Prime();
			}
		}
		break;

	case ERunScript :
		// Recursively instantiate the CScriptControl class
		{
		GetScriptFileFromScriptLine();
		CScriptControl* scriptControl = new (ELeave) CScriptControl(*this,iChildScriptFile,Logger(),ConsoleLogger(),iStartLooping,iLoop,iDefaultSysDrive,iTestSysDrive,iSelTestingOptions);
		// Kick the nested CScriptControl state machine
		scriptControl->Kick();
		// Put our instance in the idling state, Prime()'d ready for clild-parent
		// completion by the nested one.
		iState = EIdling;
		iCanComplete = ETrue;
		Prime();
		}
		break;

	case ERunCed :
		// Slightly tricky one
		// WIN32 & Non-secure means we execute CED synchronously
		{
		SetActive();
		TRAPD(err,RunCedFromScriptLineL());
		if(!err)
			// Expect completion asynchronously
			// We're set for completion so just set the state
			iState = EIdling;
		else
			{
			_LIT(KCedError,"CED Error = %d");
			ERR_PRINTF2(KCedError,err);
			// A CED error so kick the state machine
			iState = ERunning;
			Kick();
			}
		}
		break;
	
	case EDelay :
		{
		// Kick the timer and wait for completion
		SetActive();
		StartTimerFromScriptLine();
		iState = EIdling;
		}
		break;

	case ERunEndTestCase :
		{
			LogTestCaseMarkerL();
			iState = ERunning;
			iCanComplete = EFalse;
			Kick();
		}
		break;

	case ERunStartSyncTestCase :
		{
			// Check to see if the test case is ready to continue
			SetActive();
			iTimer.After( iStatus, KTEFStatusDelay*1000 );
			TBool syncContinue = iSyncControl->TestCaseContinueL();
			if( syncContinue )
				{
				iState = EIdling;
				}
		}
		break;
	case ERunPrefix :
		{
			SetPrefix();
			iState = ERunning;
			iCanComplete = EFalse;
			Kick();
		}
		break;
		
	case EIdling :
		{
		// Woken up due to either:
		// A child CScriptControl instance completing OR
		// the delay timer has completed.
		iState = ERunning;
		iCanComplete = EFalse;
		Kick();
		}
		break;

	case EPause :
		{
		_LIT(KPaused,"PAUSED - Hit Any Key to Continue\n");
		ConsoleLogger().Console().Printf(KPaused);
		ConsoleLogger().Console().Getch();
		iState = ERunning;
		iCanComplete = EFalse;
		Kick();
		}
		//Start of defect 115942 
		break;
		//End of defect 115942

	case ETestCaseIgnore:
		{
		TPtrC scriptLine;
		//do we want to while till we come to end of test case?
		while(GetNextScriptLine(scriptLine))
			{
			TLex lex(scriptLine);
			TPtrC token(lex.NextToken());
			if((token.CompareF(KTEFEndTestCaseCommand) == 0)
					|| (token.CompareF(KTEFEndSyncTestCaseCommand) == 0)) //we found an end test case one
				{
				TPtrC testCID(lex.NextToken());
				//to support nested test cases
				if(iTestCaseIDToIgnore.CompareF(testCID) == 0)
				{
					//in any case...go back to running and re-evaluate our position at
					iState = ERunning;
					iTestCaseIDToIgnore.Set(KTEFNull);
					iCanComplete = EFalse;
					iCurrentScriptLine.Set(scriptLine);
					//before going back to running re-evaluate the 
					//state of selective testing
					ProcessEndCase() ; 
					Kick();
					break;
				}
				}
			}
		break;
		}

	default:
		break;
		}
	}

/**
 * Implement the PRINT command
 * Print the string(s) following the PRINT command to the log file
 */
void CScriptControl::PrintFromScriptLine() const
	{
	TLex lex(iCurrentScriptLine);
	lex.NextToken();
	TBuf<KMaxTestExecuteCommandLength> buf;
	buf.Copy(lex.Remainder());

	_LIT(KCommentString, " //");

	TInt offset = buf.Find(KCommentString);
	if (offset != KErrNotFound)
		{
		buf.SetLength(offset);
		}

	_LIT(KS,"%S");
	INFO_PRINTF2(KS,&buf);
	}

/**
 * Implement the PREFIX command
 * Stores the prefix for command line prefixing
 */
void CScriptControl::SetPrefix() 
	{
	TLex lex(iCurrentScriptLine);
	// Bypass the PREFIX command
	lex.NextToken();

	// Get rid of any leading spaces
	while(!lex.Eos())
		{
		TChar peek = lex.Peek();
		if(peek == ' ')
			{
			lex.Inc();
			}
		else
			break;
		}

	// Chop off the carriage return and insert a space
	// If there  is a preceding comment line, get rid of that.

	iScriptLinePrefix.Copy(lex.Remainder());

	_LIT(KCarriageReturn, "\r\n");
	_LIT(KCommentString, " //");

	TInt offset = iScriptLinePrefix.Find(KCommentString);
	if (offset != KErrNotFound)
		{
		iScriptLinePrefix.SetLength(offset);
		}
	else
		{
		offset = iScriptLinePrefix.Find(KCarriageReturn);

		if (offset != KErrNotFound)
			{
			iScriptLinePrefix.SetLength(offset);
			}
		}

	_LIT(KTEFSpace, " ");
	iScriptLinePrefix.Append(KTEFSpace);

	iScriptLinePrefixSet = ETrue;
		
	}

/**
 * Function to evaluate the situation of selective testing 
 * Returns whether the testcase on the current script line 
 * should be run.
 */
TBool CScriptControl::ProceedTestCase() 
	{ 
	
	//if selective testing is not on, dont bother
	if(iSelTestingOptions == NULL)
		return ETrue;
	iTestCaseCounter++;
	if(iTestCaseCounter > 1) //if this is nested, let it run unconditionally
		return ETrue;
	// the remaining continues only if selective testing is on
	// AND we have a non-null, and hopefully valid instance of
	// iSelTestingOptions
	TLex lex(iCurrentScriptLine);
	TPtrC token(lex.NextToken());
	TPtrC testCaseID(lex.NextToken());
	//evaluating class state variables...
	//check range
	TIdentityRelation<TPtrC> crackID(TSelectiveTestingOptions::CompareTPtrC);
	TIdentityRelation<TRange> rangeComprtr(TRange::CompareTRangeStartCase);
	TRange dummy(testCaseID,testCaseID);
	for ( TInt index=0; index<iSelTestingOptions->iSelectiveCaseRange.Count(); ++index )
		{
		if ( testCaseID.CompareF(iSelTestingOptions->iSelectiveCaseRange[index].iStartTestCase) == 0 )
			{
			iSelTestingOptions->iSelectiveCaseRange[index].iState=TRange::EStateInUse;
			++iRangeRefCounter;//number of ranges now in operation	
			}
		}
	
	
	TBool runCase = ETrue ; //run everything by def
	if( iSelTestingOptions->iSelectiveTestingType == iInclusive )
		{
		//so selective testing is on and also its inclusive...
		runCase = (iRangeRefCounter>0) || iSelectOne ; 
		}
	else if(iSelTestingOptions->iSelectiveTestingType == iExclusive)
		{
		//so selective testing is on and also its exclusive...
		runCase = (iRangeRefCounter<=0) && !iSelectOne ; 
		}
	if(!runCase)//if the test case is to be selectively skipped, log it...
		{
		//use this one to log unexecuted cases...
		Logger().LogTestCaseResult(iScriptFile, iCurrentScriptLineNumber, RFileFlogger::ESevrInfo, token, testCaseID,ESkippedSelectively);
		}
	if(runCase == EFalse)
		{
			iTestCaseIDToIgnore.Set(testCaseID) ; 
		}
		
	return runCase ; 	
	}

/**
 * Function to evaluate the state variables 
 * at the end of test case 
 */
void CScriptControl::ProcessEndCase()
	{
	if(iSelTestingOptions==NULL) //selective testing is not on
		return;					// dont bother
	iTestCaseCounter--;
	if(iTestCaseCounter<0) //in case we encountered unmatched end cases
		iTestCaseCounter=0;
	TLex lex(iCurrentScriptLine);
	TPtrC token(lex.NextToken());
	TPtrC testCaseID(lex.NextToken());	
	//check if this is ending a range
	TRange dummy(testCaseID,testCaseID);
	TIdentityRelation<TRange> crackIDRangeend(TRange::CompareTRangeEnd);
	for ( TInt index=0; index<iSelTestingOptions->iSelectiveCaseRange.Count(); ++index )
		{
		if ( testCaseID.CompareF(iSelTestingOptions->iSelectiveCaseRange[index].iEndTestCase) == 0 )
			{
			if ( iSelTestingOptions->iSelectiveCaseRange[index].iState == TRange::EStateInUse )
				{
				iSelTestingOptions->iSelectiveCaseRange[index].iState=TRange::EStateUsed;
				--iRangeRefCounter;
				}
			else
				{
				//	Error condition. An end test case has been matched to a range that has not processed the start test case
				//	Either the start test case does not exist or the start test case comes after the end test case in the script
				//or maybe do nothing
				
				}
			}
		}
	
		
	//always reset the onetime test case thing
	iSelectOne = EFalse;
	}

/**
 * Implement START_TESTCASE/ END_TESTCASE commands
 * Write a testcase marker to the logfile
 */
void CScriptControl::LogTestCaseMarkerL() 
	{
	TLex lex(iCurrentScriptLine);
	TPtrC token(lex.NextToken());
	TVerdict TestCaseResult(EFail);

	TPtrC TestCaseMarker(lex.NextToken());

		if (token.CompareF(KTEFStartTestCaseCommand) == 0 || token.CompareF(KTEFStartSyncTestCaseCommand) == 0)
		{
		// Call the interface routine for logging in HTML & XML format
		Logger().LogTestCaseResult(iScriptFile, iCurrentScriptLineNumber, RFileFlogger::ESevrHigh, token, TestCaseMarker);
		iTestCaseID.Copy(TestCaseMarker);
		}
	else
		{
		// Its an END_TESTCASE.
		// Need to identify whether all preceding test steps in the file
		// passed (back to a *matching* START_TESTCASE).
		TestCaseResult = HasTestCasePassedL(TestCaseMarker);

			// Special case for KTEFEndSyncTestCaseCommand where the result in 
			// the shared data area needs to be updated so STAT can retrieve it later.
			if( token.CompareF(KTEFEndSyncTestCaseCommand) == 0 )
				{
				// Check to see if the Sync Data has been created
				if( iSyncControl != NULL )
					{
					// Update the TEFResult shared data value
					iSyncControl->SetResultL( TestCaseResult );
					}
				else
					{
					User::Leave( KErrNotReady );
					}
				}
		// Call the interface routine for logging in HTML & XML format
		Logger().LogTestCaseResult(iScriptFile, iCurrentScriptLineNumber, RFileFlogger::ESevrHigh, token, TestCaseMarker, TestCaseResult);
		iTestCaseID.Copy(KTEFTestCaseDefault);
		}
	}

/**
 *
 * Implement TestCase pass checking
 */
TVerdict CScriptControl::HasTestCasePassedL(TPtrC aTestCaseMarker)
	{
	// Looks from the top of the file for the equivalent Start_TestCase
	// when it finds it it checks that all the steps to the bottom of 
	// the file (the current position) 

	// Create a Cinidata object to parse through the testexecute.ini
	// To retrieve the path where the log is to be placed

	CTestExecuteIniData* parseTestExecuteIni = NULL;
	TBuf<KMaxTestExecuteNameLength> resultFilePath;

	TRAPD(err,parseTestExecuteIni = CTestExecuteIniData::NewL(iDefaultSysDrive));
	if (err == KErrNone)
		{
		CleanupStack::PushL(parseTestExecuteIni);
		parseTestExecuteIni->ExtractValuesFromIni();
		parseTestExecuteIni->GetKeyValueFromIni(KTEFHtmlKey, resultFilePath);
		}
	else
		{
		resultFilePath.Copy(KTestExecuteLogPath);
		resultFilePath.Replace(0, 2, iDefaultSysDrive);
		}

	TBuf<KMaxTestExecuteNameLength> resultFileName(resultFilePath);
	// loading the simplified test result in case of out-of-memory 
	resultFileName.Append(KTEFTestExecuteResultSimplifiedSummaryFile);
	if (parseTestExecuteIni != NULL)
		{
		CleanupStack::PopAndDestroy(parseTestExecuteIni);
		}

	// Open the result summary file
	RFs fS;
	User::LeaveIfError(fS.Connect());
	CleanupClosePushL(fS);

	RFile logFile;
	User::LeaveIfError(logFile.Open(fS,resultFileName, EFileWrite | EFileRead | EFileShareAny));
	CleanupClosePushL(logFile);
	TInt fileSize;
	// Read the complete result summary file onto the heap
	// It wont be that large
	User::LeaveIfError(logFile.Size(fileSize));
	HBufC* resultData = HBufC::NewLC(fileSize);
	HBufC8* resultData8 = HBufC8::NewLC(fileSize);	
	TPtr8 ptrData8(resultData8->Des());
	User::LeaveIfError(logFile.Read(ptrData8));
	TPtr ptrData(resultData->Des());
	ptrData.Copy(ptrData8);
	CleanupStack::PopAndDestroy(resultData8);

	TBool foundMarker(EFalse);
	TBool foundNonPassResult(EFalse);
	TInt duplicatesCounter(0);

	TLex lex(ptrData);
	while (!lex.Eos())
		{
		// Find the ***Result keywords
		TPtrC commandName( lex.NextToken() );
		if(	commandName.CompareF(KTEFStartTestCaseCommand) == 0 ||
			commandName.CompareF(KTEFStartSyncTestCaseCommand) == 0 )
			{
			if (lex.NextToken() == aTestCaseMarker)
				{
				// Increment the counter to identify that the test case id is duplicated
				duplicatesCounter++;
				if (duplicatesCounter == 2)
					{
					// If the test case id is duplicated for more than once,
					// issue a warning in the log file, mentioning duplicate of test case id
					_LIT(KWarnDuplicateTCID, "Test Case ID : %S re-used");
					WARN_PRINTF2(KWarnDuplicateTCID, &aTestCaseMarker);
					}

				// When a matching test case id is found, the found marker is set to ETrue
				foundMarker = ETrue;
				// Initialise the foundNonPassResult to EFalse, on entry into each test case result in the log
				foundNonPassResult = EFalse;
				continue;
				}
			continue;
			}
		else if (foundMarker && commandName == KTEFResultTag)
			{
			// If the START_TESTCASE is found and a RESULT tag is found in the test result file,
			// Extract the result value set for every test step called within
			// and check to see if there are any non-pas results
			_LIT(KTEFEquals,"=");
			if (lex.NextToken() != KTEFEquals)
			continue;
			TPtrC result(lex.NextToken());
			if(result != KTEFResultPass)
				{
				// Set the foundNonPassResult to ETrue indicating the test case to fail
				foundNonPassResult = ETrue;
				}
			}			
		}

	CleanupStack::PopAndDestroy(resultData);
	CleanupStack::Pop(&logFile);
	logFile.Close();
	CleanupStack::Pop(&fS);
	fS.Close();

	if (foundMarker)
		{
		if (foundNonPassResult)
			{
			return EFail;
			}
		else
			{
			return EPass;
			}
		}
	else
		{
		return EInconclusive;
		}
	}

/**
 * Implement the DELAY command
 */
void CScriptControl::StartTimerFromScriptLine()
	{
	TLex lex(iCurrentScriptLine);
	lex.NextToken();
	TLex delayLex(lex.NextToken());
	TInt delay;
	// Read the delay in milliseconds
	TInt err = delayLex.Val(delay);
	if(err)
		// Set the default
		delay = KDefaultDelayMilliseconds;
	iTimer.After(iStatus,delay*1000);
	}

/**
 * Secure - same for Target and Wins
 */
void CScriptControl::RunCedFromScriptLineL()
	{
	TLex lex(iCurrentScriptLine);
	// Skip CED
	lex.NextToken();
	TPtrC cedCommandLine(lex.Remainder());
	iCurrentScriptLine.Set(cedCommandLine);
	RProcess process;
	User::LeaveIfError(process.Create(_L("ced.exe"),iCurrentScriptLine));
	process.Rendezvous(iStatus);
	// Run CED asynchronously
	process.Resume();
	process.Close();
	}

/**
 * Set up the path of a script file for a child CScriptControl object
 */
void CScriptControl::GetScriptFileFromScriptLine()
	{
	TLex lex(iCurrentScriptLine);
	lex.NextToken();
	iChildScriptFile.Set(lex.NextToken());
	}

/**
 * @param aUtilsCommand - Command string for the utilities command
 * Implement basic commands:
 * NB: Always requires the full path
 * Logic borrowed from Scheduletest
 * Always complete synchronously
 * 
 * CopyFile
 * MKDir
 * MakeReadWrite
 * Delete
 * DeleteDirectory
 */
void CScriptControl::RunUtilsFromScriptLineL() const
	{
	_LIT(KDefault,				"?:\\default");
	_LIT(KCDrive,				"?:\\"); 
	_LIT(KTEFBackslash,			"\\" );

	TBuf<10> defaultPath(KDefault);
	defaultPath.Replace(0, 2, iTestSysDrive);

	TBuf<3> cDrive(KCDrive);
	cDrive.Replace(0, 2, iTestSysDrive);

	RFs fS;
	User::LeaveIfError(fS.Connect() );
	CleanupClosePushL(fS);

	TLex lex(iCurrentScriptLine);
	lex.NextToken();
	TPtrC token(lex.NextToken());
	if(token.CompareF(KTEFRunUtilsCopyFile) == 0)
		{
		TPtrC file1=lex.NextToken();
		TPtrC file2=lex.NextToken();
		TParse source, dest;
		CFileMan* fMan = CFileMan::NewL(fS);
		CleanupStack::PushL(fMan);
		User::LeaveIfError(source.Set(file1, &defaultPath, NULL) );
		User::LeaveIfError(dest.Set(file2, &defaultPath, NULL) );
		User::LeaveIfError(fMan->Copy(source.FullName(), dest.FullName(), CFileMan::EOverWrite) );
		CleanupStack::PopAndDestroy(fMan);
		}
	else if(token.CompareF(KTEFRunUtilsMkDir) == 0)
		{
		token.Set(lex.NextToken());
		TParse fileName;

		if (!token.Length())
			User::Leave(KErrPathNotFound);
		
		TPtrC lastChar(token.Mid(token.Length() - 1));
		if ( lastChar.CompareF(KTEFBackslash) != 0 )
			{
			TBuf<64> tempToken(token);
			tempToken.Append(KTEFBackslash);
			token.Set(tempToken);
			}

		User::LeaveIfError( fileName.Set(token, &cDrive, NULL) );
		User::LeaveIfError( fS.MkDir( fileName.DriveAndPath() ) );
		}
	else if(token.CompareF(KTEFRunUtilsDeleteFile) == 0 ||
	         token.CompareF(KTEFRunUtilsDelete) == 0)
		{
		token.Set(lex.NextToken());
		// defect047128 - Code change for handling wildcard deletes
		CFileMan* fMan = CFileMan::NewL(fS);
		CleanupStack::PushL(fMan);
		User::LeaveIfError(fMan->Delete(token) );
		CleanupStack::PopAndDestroy(fMan);
		}
	else if(token.CompareF(KTEFRunUtilsMakeReadWrite) == 0)
		{
		token.Set(lex.NextToken());
		TParse fileName;
		User::LeaveIfError(fileName.Set(token, &defaultPath, NULL) );
		TInt err = fS.SetAtt(fileName.FullName(),0, KEntryAttReadOnly);
		if (err != KErrNone && err != KErrNotFound)
			User::Leave(err);
		}
	//It deletes the specified directory
	else if(token.CompareF(KTEFDeleteDirectory) == 0)
		{
		token.Set(lex.NextToken());
		TParse fileName;

		if (!token.Length())
			User::Leave(KErrPathNotFound);
		
		TPtrC lastChar(token.Mid(token.Length() - 1));
		if ( lastChar.CompareF(KTEFBackslash) != 0 )
			{
			TBuf<64> tempToken(token);
			tempToken.Append(KTEFBackslash);
			token.Set(tempToken);
			}

		CFileMan* fMan = CFileMan::NewL(fS);
		CleanupStack::PushL(fMan);

		User::LeaveIfError(fileName.Set(token, &cDrive, NULL) );
		User::LeaveIfError( fMan->RmDir( fileName.DriveAndPath() ) );

		CleanupStack::PopAndDestroy(fMan);
		}
	
	fS.Close();
	CleanupStack::Pop(&fS);	
	}

/**
 * Read the Script File data into a heap buffer
 * We could read the file line by line but that would be cumbersome, and unless there
 * is a heap size problem, this is tidier.
 */
void CScriptControl::CreateScriptDataFromScriptFileL()
	{
	RFs fS;
	User::LeaveIfError(fS.Connect());
	CleanupClosePushL(fS);
	RFile scriptFile;
	User::LeaveIfError(scriptFile.Open(fS,iScriptFile,EFileRead | EFileShareAny));
	CleanupClosePushL(scriptFile);
	TInt fileSize;
	User::LeaveIfError(scriptFile.Size(fileSize));
	// Create a 16bit heap buffer
	iScriptData = HBufC::NewL(fileSize);
	HBufC8* narrowData = HBufC8::NewL(fileSize);
	CleanupStack::PushL(narrowData);
	TPtr8 narrowPtr=narrowData->Des();
	// Read the file into an 8bit heap buffer
	User::LeaveIfError(scriptFile.Read(narrowPtr));
	TPtr widePtr(iScriptData->Des());
	// Copy it to the 16bit buffer
	widePtr.Copy(narrowData->Des());
	CleanupStack::PopAndDestroy(narrowData);
	CleanupStack::Pop(2);
	scriptFile.Close();
	fS.Close();
	// Set up the instance token parser
	iScriptLex = iScriptData->Des();
	}

/**
 * Extracts the human readable server name from the current script line then
 * calls Client RTestServ Interface to make a connection to the server.
 */
void CScriptControl::CreateServerFromScriptLineL()
	{
	TLex lex(iCurrentScriptLine);
	lex.NextToken();
	TInt i=0;
	TInt count = iServers.Count();
	TPtrC serverName(lex.NextToken());
	
	// We loop through to see if the server has already been created
	for(i=0;i<count;i++)
		{
		if(iServers[i]->ServerName() == serverName)
			// Server already exists in our array
			break;
		}
	if(i == count)
		{
		// Create a new RTestServ pointer and add it to the list
		RScriptTestServ* serv = new (ELeave) RScriptTestServ;
		// Connect using the client API
		TInt err;
		if(lex.NextToken().CompareF(KTEFLoadSuiteSharedData) == 0)
			{
			err = serv->Connect(serverName,ETrue);
			}
		else
			err = serv->Connect(serverName,EFalse);
		if(err)
			{
			// Don't add the server to the array if we fail to connect.
			delete serv;
			// Caller TRAP's
			User::Leave(err);
			}
		iServers.Append(serv);
		}
	}

/**
 * @param aIndex - Return the index of the RTestServ instance - If found
 * @return - EFalse if server not found. ETrue if server found.
 * 
 */
TBool CScriptControl::GetServerIndexFromScriptLine(TInt& aIndex)
	{
	TLex lex(iCurrentScriptLine);
	TLex lexTimeout;
	_LIT(KErrInvalidArgumentSet,"The arguments are not provided in proper format.\
			Unable to retrieve the details of the server from the command line");

	TPtrC command(lex.NextToken());
	if(command.CompareF(KTEFRunTestStepResultCommand) == 0)
		{
		lex.NextToken();
		lex.SkipSpace();
		lex.Mark();
		}
		
	else if(command.CompareF(KTEFRunPanicStepResultCommand) == 0)
		{
		lex.NextToken();
		TPtrC panicString(lex.NextToken());
		// Check to see if the panic string contains open quotes and close quotes
		// If the panic string token contains open quote, it is understood that the panic string contains spaces
		// So, look for close quote in the subsequent tokens. If not found, return boolean false as return value
		if(panicString.Left(1).Compare(KTEFOpenQuotes) == 0 && panicString.Right(1).Compare(KTEFOpenQuotes) != 0)
			{
			TBool validCommandLine(EFalse);
			while(!lex.Eos() && !validCommandLine)
				{				
				panicString.Set(lex.NextToken());
				if(panicString.Right(1).Compare(KTEFOpenQuotes) == 0)
					validCommandLine = ETrue;
				}
			if (!validCommandLine)
				{
				ERR_PRINTF1(KErrInvalidArgumentSet);
				return EFalse;
				}
			}
		lex.SkipSpace();
		lex.Mark();
		}
	else if(command.CompareF(KTEFRunTestStepCommand) == 0 ||
			command.CompareF(KTEFStartTestBlock) == 0)
		{
		TInt firstChar;
		TPtrC commandStr;
		TBool panicStringComplete(ETrue);
		while(!lex.Eos())
			{
			lex.SkipSpace();
			lex.Mark();
			
			TPtrC	token = lex.NextToken();
			if( token.Length()>0 )
				{
				commandStr.Set( token );
				firstChar = commandStr[0];
				// 33 is the ascii value for "!". Used here for confirming switches
				if (firstChar != KTEFAsciiExclamation && panicStringComplete)
					{
					break;
					}
				// Check to see if !PanicString TEF parameter contains panic string with spaces
				// If so, see if they are enclosed within a open & close braces
				if(commandStr.Length() > 14 && 
					commandStr.Mid(0,14).Compare(_L("!PanicString=\"")) == 0)
						panicStringComplete = EFalse;
				if(!panicStringComplete && commandStr.Right(1).Compare(KTEFOpenQuotes) == 0)
					panicStringComplete = ETrue;
				}
			}
			if (!panicStringComplete)
				{
				ERR_PRINTF1(KErrInvalidArgumentSet);
				return EFalse;
				}
		}
	
	// We need to skip the timeout if it's there.
	if(	command.CompareF(KTEFRunTestStepCommand) == 0 ||
		command.CompareF(KTEFStartTestBlock) == 0)
		lexTimeout=lex.MarkedToken();
	else
		lexTimeout=lex.NextToken();
	TInt timeout;
	TPtrC serverName;
	if(lexTimeout.Val(timeout) != KErrNone)
		// No timeout so use the second token
		serverName.Set(lex.MarkedToken());
	else
		// Timeout value there
		serverName.Set(lex.NextToken());
	TInt i=0;
	// Loop through the installed servers
	TInt count = iServers.Count();
	for(i=0;i<count;i++)
		{
		if(iServers[i]->ServerName() == serverName)
			// Found server installed
			break;
		}
	// Return found or not found
	if(i == count)
		return EFalse;
	else
		{
		aIndex = i;
		if (iServers[i]->SharedData())
			{
			iConcurrent = EFalse;
			}
		return ETrue;
		}
	}

/**
 *  * @return - ETrue if the scriptline is valid, else retuens EFalse
 */
TBool CScriptControl::CheckValidScriptLine() const
	{
	TLex lex(iCurrentScriptLine);
	TPtrC command(lex.NextToken());
	TBool panicCodeSet(EFalse);
	TBool panicStringSet(EFalse);
	TBool heapValueSet(EFalse);
	TBool validScript(ETrue);
	TBool oomRequestSet(EFalse);
	TBool setupOptionSet(EFalse);
	
	if(command.CompareF(KTEFRunTestStepCommand) == 0)
		{
		TInt firstChar;
		TPtrC commandStr;
		while(!lex.Eos())
			{
			lex.SkipSpace();
			commandStr.Set(lex.NextToken());
			firstChar = commandStr[0];
			// 33 is the ascii value for "!". Used here for confirming switches
			if (firstChar == KTEFAsciiExclamation)
				{
				if (commandStr.Length() >= KTEFMinErrorParamLength && commandStr.Mid(0,KTEFMinErrorParamLength).CompareF(KTEFError) == 0)
					{
					if(command.CompareF(KTEFRunTestStepCommand) == 0)
						command.Set(KTEFRunErrorStepResultCommand);
					else
						validScript = EFalse;
					}
				else if (commandStr.Length() >= KTEFMinSetupParamLength && commandStr.Mid(0,KTEFMinSetupParamLength).CompareF(KTEFSetUpParam) == 0)
					{
					if(command.CompareF(KTEFRunTestStepCommand) == 0 && !setupOptionSet)
						setupOptionSet = ETrue;
					else
						validScript = EFalse;
					}
				else if (commandStr.Length() > KTEFMinResultParamLength && commandStr.Mid(0,KTEFMinResultParamLength).CompareF(KTEFResult) == 0 && commandStr.Mid(KTEFMinResultParamLength).Length() <= KTEFMaxVerdictLength)
					{
					if (command.CompareF(KTEFRunTestStepCommand)  == 0)
						command.Set(KTEFRunTestStepResultCommand);
					else
						validScript = EFalse;
					}
				else if (commandStr.Length() >= KTEFMinPanicCodeParamLength && commandStr.Mid(0,KTEFMinPanicCodeParamLength).CompareF(KTEFPanicCode) == 0)
					{
					if ((command.CompareF(KTEFRunTestStepCommand) == 0 || command.CompareF(KTEFRunPanicStepResultCommand) == 0) && !panicCodeSet)
						{
						command.Set(KTEFRunPanicStepResultCommand);
						panicCodeSet=ETrue;
						}
					else
						validScript = EFalse;
					}
				else if (commandStr.Length() >= KTEFMinPanicStringParamLength && commandStr.Mid(0,KTEFMinPanicStringParamLength).CompareF(KTEFPanicString) == 0)
					{
					if ((command.CompareF(KTEFRunTestStepCommand) == 0 || command.CompareF(KTEFRunPanicStepResultCommand) == 0) && !panicStringSet)
						{
						command.Set(KTEFRunPanicStepResultCommand);
						panicStringSet = ETrue;
						}
					else
						validScript = EFalse;
					}
				else
					{
					if (commandStr.Length() >= KTEFMinHeapParamLength && commandStr.Mid(0,KTEFMinHeapParamLength).CompareF(KTEFHeap) == 0 && !heapValueSet)
						heapValueSet = ETrue;
					else if (commandStr.Length() >= KTEFMinOomParamLength && commandStr.Mid(0,KTEFMinOomParamLength).CompareF(KTEFOom) == 0 && !oomRequestSet)
						oomRequestSet = ETrue;
					else
						validScript = EFalse;
					}
				}
			else
				break;
			}
		}
		return validScript;
	}

/**
 * Return the next line in the script file
 * @param aScriptLine - return line in the script file minus CRLF
 * @return - True if line found, false for end of file.
 */	
TBool CScriptControl::GetNextScriptLine(TPtrC& aScriptLine)
	{
	if(iScriptLex.Eos())
		{
		// Fix defect 1193337, check the value is zero or not. If zero, this should be no nested run script
		if((0 == iNestedNumRunScriptInLoop) && iLoop)
		// End defect 119337
			{
			_LIT(KEndRepeatNotFound,"The END_REPEAT command is not found");
			INFO_PRINTF1(KEndRepeatNotFound);
			}
		
		// Fix defect 119337, check if this is still in loop and this run script command is nested
		// decrease one shows that the nested number is decreased
		else if (iNestedNumRunScriptInLoop > 0 && iLoop)
			{
			--iNestedNumRunScriptInLoop;
			}
		// End defect 119337
		
		return EFalse;
		}
	// Mark the current script line to return
	iScriptLex.Mark();
	if(iLoop && !iCheckVar)
		{
		iStoreLoop.Assign(iScriptLex.RemainderFromMark());
		iTempStoreLoop.Assign(iScriptLex.RemainderFromMark());
		iCheckVar=ETrue;
		}
	// Place the lex marker for the next read
	while(!iScriptLex.Eos())
		{
		TChar peek = iScriptLex.Peek();
		if(peek == '\n')
			{
			iScriptLex.Inc();
			iCurrentScriptLineNumber++;
			break;
			}
		else
			iScriptLex.Inc();
		}
	aScriptLine.Set(iScriptLex.MarkedToken());
	if(aScriptLine.FindF(KTEFRunScriptCommand)!=KErrNotFound && iLoop)
		{
		// Fix defect 119337, add this integer shows a nested run script added.
		++iNestedNumRunScriptInLoop;
		// End Fix defect 119337
		}
	if(aScriptLine.Length() || !iScriptLex.Eos())
		return ETrue;
	else
		return EFalse;
	}

/**
 * Return the next line from script file which is to be looped
 * @param aScriptLine - return line in the script file which is under loop minus CRLF
 * @return -True if line is found else return false
 * 
 * If RUN_SCRIPT command has been encountered return the line from GetNextScriptLine Function
 */
TBool CScriptControl::GetLoopScriptLine(TPtrC& aScriptLine)
	{
	// Fix defect 119337, check if the run script is nested
	if (0 != iNestedNumRunScriptInLoop)
	// End defect 119337
		return(GetNextScriptLine(aScriptLine));
	if(iStoreLoop.Eos())
		return EFalse;
	iStoreLoop.Mark();
	while(!iStoreLoop.Eos())
		{
		TChar peek = iStoreLoop.Peek();
		if(peek == '\n')
			{
			iStoreLoop.Inc();
			break;
			}
		else
			iStoreLoop.Inc();
		}
	aScriptLine.Set(iStoreLoop.MarkedToken());
	if(aScriptLine.Find(KTEFEndRepeat)!=KErrNotFound)
		{
		iStoreLoop.Assign(iTempStoreLoop);	
		}
	if(aScriptLine.FindF(KTEFRunScriptCommand)!=KErrNotFound)
		{
		// Fix defect 119337, add this integer shows a nested run script added.
		++iNestedNumRunScriptInLoop;
		// End defect 119337
		}
	if(aScriptLine.Length() || !iStoreLoop.Eos())
		return ETrue;
	else
		return EFalse;
	}

/**
 * @param aErr - The completion code
 * @param aPanicString - Descriptor reference containing the panic string if a test were to panic
 * @param aScriptLineNumber - Script line number used for printing result into log file
 * @param aCommand - Command name also used for printing result into log file
 * Called by a CClientControl or CProgramControl child object
 */
void CScriptControl::TaskCompletion(TInt aErr, const TDesC& aPanicString, TInt aScriptLineNumber,const TDesC& aCommand,TBool aTaskCanComplete,TTEFItemArray* aItemArray)
	{
	if(aItemArray)
		{
		TVerdict err = Logger().LogBlock( aItemArray, aScriptLineNumber );
		
		// If no error was set on the server side (ie. a panic) then set
		//  it here with the post processing result
		if( EPass == aErr && aPanicString.Length() == 0 )
			{
			aErr = err;
			}
		}
	// Log the test step result to the output file
	LogResult((TVerdict)aErr, aPanicString, aScriptLineNumber, aCommand);
		
	if(aErr != KErrNone && aErr != KErrNotSupported && iBreakOnError)
		// Put this instance of the script engine into the closing state as we've
		// encountered the BREAK_ERROR_ON command.
		// KErrNotSupported is considered a benign error 
		{
		iState = EClosing;
		}
	iAsyncTasksOutstanding--;
	// Check the flag first then trigger our own RunL()
	if(aTaskCanComplete || iCanComplete)
		Complete(aErr);
	}

/**
 * Print the current script line to the console
 */
void CScriptControl::PrintCurrentScriptLine() const
	{
	// Check we don't overflow
	TBuf<KMaxTestExecuteLogLineLength> output;
	if((iCurrentScriptLine.Length() + iScriptFile.Length() + 4 ) > output.MaxLength())
		{
		output.Copy(iAlteredScriptLine);
		Logger().PrintCurrentScriptLine(output);
		return;
		}
	// Copy the script filename. Handy for recursion
	output.Copy(iScriptFile);
	output.Append(KTEFSpace);
	// Append the script file line
	output.Append(iCurrentScriptLine);
	// Write to console
	ConsoleLogger().Console().Printf(KTEFStringFormat,&output);

	Logger().PrintCurrentScriptLine(output);
	}

/**
 * Checks for commented commands and increments a counter for logging
 */
TBool CScriptControl::CheckCommentedCommands() const
	{
	if(!iCurrentScriptLine.Length())
		return ETrue;
	
	TLex lex(iCurrentScriptLine);
	TPtrC token(lex.NextToken());
	if (!token.Length())
		return ETrue;
	
	TInt firstChar = iCurrentScriptLine[0];
	if(firstChar == '/')
		{
		TInt findRunTestStep;
		TInt findRunPanicStep;
		TInt findRunScript;
		TInt findRunProgram;
		TInt findRunWSProgram;
		TInt findStartTestBlock;
		findRunTestStep=iCurrentScriptLine.Find(KTEFRunTestStepCommand);
		findRunScript=iCurrentScriptLine.Find(KTEFRunScriptCommand);
		findRunPanicStep=iCurrentScriptLine.Find(KTEFRunPanicStepCommand);
		findRunProgram=iCurrentScriptLine.Find(KTEFRunProgramCommand);
		findRunWSProgram=iCurrentScriptLine.Find(KTEFRunWSProgramCommand);
		findStartTestBlock=iCurrentScriptLine.Find(KTEFStartTestBlock);
		if(findRunTestStep>0 || findRunScript>0 || findRunPanicStep>0 || findRunProgram>0 || findRunWSProgram>0 || findStartTestBlock>0 && !iStartLooping)
			{
			commentedCommandsCount++;
			}
		}
	if(firstChar == '\r' || firstChar == '\n' || firstChar == '#' || firstChar == '/')
		return ETrue;
	return EFalse;
	}

/**
 * @param aResult - Test Step result
 * @param aPanicString - Descriptor containing the panic string if test were to panic
 * @param aScriptLineNumber - The line in the script file
 * @param aCommand - Command name whose result is set for logging
 * Log a RUN_TEST_STEP, RUN_PANIC_STEP or RUN_PROGRAM result to file
 */
void CScriptControl::LogResult(TVerdict aResult, const TDesC& aPanicString, TInt aScriptLineNumber,const TDesC& aCommand)
	{
	// Call the Logger()'s LogResult() routine to manipulate results of RUN_TEST_STEP command/variants
	// Also controls the fields for logging both HTML & XML logging
	Logger().LogResult(aResult, aPanicString, aScriptLineNumber, aCommand, iScriptFile, RFileFlogger::ESevrHigh);
	}

/**
 * Constructor
 * @param RTestServ - Reference to a root RTestServer instance
 * @param aCommand - Reference to a RUN_TEST_STEP script line
 * @param MTaskCompletion - Reference to the parent completion interface
 * @param aScriptLineNumber - The line in script file
 * @param aLogger - Reference to the Logger class
 * @param aLoopIndex - TInt reference that provides the loop index for the test. The value is 0 if not in loop.
 * @param aTestCaseID - Descriptor containing the test case id, if test run is within a test case
 * @param aScriptFilePath - The path of the script file being executed
 * @param aSysDrive - Default system drive letter
 * @param aTestSysDrive - System drive letter overwritten from testexecute.ini
 */
CClientControl::CClientControl(RScriptTestServ& aServ,const TDesC& aCommand, MTaskCompletion& aCompletion, TInt aScriptLineNumber, CTestExecuteLogger& aLogger, TInt aLoopIndex, const TDesC& aTestCaseID, TPtrC& aScriptFilePath, const TDriveName& aSysDrive, const TDriveName& aTestSysDrive)
:	CTaskControlBase(aCommand,aCompletion,aScriptLineNumber,aLogger)
,	iServ(aServ)
,	iRetryCount(0)
,	iLoopIndex(aLoopIndex)
,	iTestCaseID(aTestCaseID)
,	iBlockArrayPkg(NULL)
,	iBlockArrayPtr(STATIC_CAST(TUint8*,NULL), 0)
,	iScriptFilePath(aScriptFilePath)
,	iDefaultSysDrive(aSysDrive)
,	iTestSysDrive(aTestSysDrive)
	{
	// Extract the parameters to the test step and store them in the class
	GetStepParamsFromStepCommand();
	}

CClientControl* CClientControl::NewL(	RScriptTestServ& aTestServ,
										const TDesC& aCommand,
										MTaskCompletion& aCompletion,
										TInt aScriptLineNumber,
										CTestExecuteLogger& aLogger,
										TInt aLoopIndex,
										const TDesC& aTestCaseID,
										const TDesC& aEndBlockCommand,
										TPtrC& aScriptFilePath,
										const TTEFItemArray& aBlockArray,
										const TDriveName& aSysDrive,
										const TDriveName& aTestSysDrive)
	{
	CClientControl* self = new (ELeave) CClientControl(	aTestServ,
														aCommand,
														aCompletion,
														aScriptLineNumber,
														aLogger,
														aLoopIndex,
														aTestCaseID,
														aScriptFilePath,
														aSysDrive,
														aTestSysDrive);
	CleanupStack::PushL(self);
	self->ConstructL(aEndBlockCommand, aBlockArray);
	CleanupStack::Pop();
	return self;
	}

void CClientControl::ConstructL( const TDesC& aEndBlockCommand, const TTEFItemArray& aBlockArray )
	{
	iEndBlockCommand.Set( aEndBlockCommand );
	TTEFItemPkgBuf	itemPckgBuf;
	TInt			count = aBlockArray.Count();
	iBlockArrayPkg	= HBufC8::NewL( count * itemPckgBuf.Size() );
	iBlockArrayPtr.Set( iBlockArrayPkg->Des() );
	for( TInt i=0; i<count; i++ )
		{
		itemPckgBuf = aBlockArray.At(i);
		iBlockArrayPtr.Append( itemPckgBuf );
		}
	}

TTEFItemArray* CClientControl::CreateBlockArrayLC()
	{
	TTEFItemPkgBuf	itemPckgBuf;
	TPtr8			blockArrayPtr(iBlockArrayPkg->Des());
	TInt			count = blockArrayPtr.Size()/itemPckgBuf.Size();
	TTEFItemArray*	itemArray = new (ELeave) TTEFItemArray( count );
	CleanupStack::PushL( itemArray );

	TInt	pos = 0;
	for( TInt i=0; i<count; i++ )
		{
		itemPckgBuf.Copy(blockArrayPtr.Mid(pos, itemPckgBuf.Size()));
		pos += itemPckgBuf.Size();
		itemArray->AppendL( itemPckgBuf() );
		}

	return itemArray;
	}
	
/**
 * Destructor
 */
CClientControl::~CClientControl()
	{
	iTimer.Cancel();
	iTimer.Close();
	
	if( iBlockArrayPkg )
		{
		delete iBlockArrayPkg;
		iBlockArrayPkg = NULL;
		}
	}
const TInt KDisableTimer = -1;

/** 
 * Test step has done. log the test result. 
 */
void CClientControl::TestStepComplete()
	{
	iSession.Close();
	if(iBlockArrayPkg)
		{
		TTEFItemArray *itemArray = CreateBlockArrayLC();
		iParent.TaskCompletion(iStatus.Int(), iTaskExitCategory, iScriptLineNumber, iEndBlockCommand, iTaskCanComplete, itemArray);
		CleanupStack::PopAndDestroy(itemArray);
		}
	else
		{
		iParent.TaskCompletion(iStatus.Int(), iTaskExitCategory, iScriptLineNumber, iCommandLine, iTaskCanComplete);
		}
	}

/**
 * Pure virtual
 * Test step handling state machine
 * Kick'd() into by the parent CScriptControl object. Constructed in the EInit state
 */
void CClientControl::RunL()
	{
	switch(iState)
		{
	case EInit	:
		User::LeaveIfError(iTimer.CreateLocal());
		if(iBlockArrayPkg)
			{
			User::LeaveIfError(iSession.Open(iServ, EFalse));		
			}
		else
			{
			User::LeaveIfError(iSession.Open(iServ, ETrue, iStepName));
			}
		// Whilst testing concurrent mode, the OS occasionally completes with
		// KErrServerBusy or KErrInUse.
		
		// clean up retry counter. case ERetry will using.
		iRetryCount = 0;
	case EServerRetry :
		{
		SetActive();
		
		// Set the heap size for creating the thread heap size
		iTaskExitCategory.Copy(iTaskHeapSize);

		TBuf<5> checkOOMArgsBuf;

		// Check if the step args has the OOM arguments already
		// A minimum 5 chars expected to check this : "OOM=0" or "OOM=1"
		if (iStepArgs.Length() >=5)
			{
			// Extract the first 5 chars from the start of step args string
			checkOOMArgsBuf.Copy(iStepArgs.Mid(0,5));
			}

		_LIT(KOOMOne,"OOM=1");
		_LIT(KOOMZero,"OOM=0");

		// Set the OOM argument after checking for existence of OOM param already
		if ((checkOOMArgsBuf.CompareF(KOOMOne) != KErrNone) && (checkOOMArgsBuf.CompareF(KOOMZero) != KErrNone))
			{
			// Insert the OOM arguments to the start of iStepArgs string
			if (iOOMRequestSet) // Set OOM=1 if !OOM is used in the RUN_TEST_STEP command
				iStepArgs.Insert(0,_L("OOM=1 "));
			else
				iStepArgs.Insert(0,_L("OOM=0 "));
			}

		TBuf<KMaxTestExecuteCommandLength> checkSetUpArgsBuf;

		// Check if the step args has the Setup arguments already
		// A min of 13 chars expected to check this : "OOM=0 !Setup=" or "OOM=1 !Setup="
		if (iStepArgs.Length() >=13)
			{
			// Extract 7 chars after skipping the OOM arguments "OOM=0 " or "OOM=1 "
			checkSetUpArgsBuf.Copy(iStepArgs.Mid(6,7));
			}

		// Set the !Setup argument after checking for existence !Setup param already
		if (checkSetUpArgsBuf.CompareF(KTEFSetUpParam) != KErrNone)
			{
			// Retain the !Setup value from RUN_TEST_STEP command, if available
			if (iSetUpParamValue.Length() == KTEFZeroValue)
				{
				// If !Setup is not used in RUN_TEST_STEP, set the !Setup to TTestSetupState::ESetupNone (0)
				iSetUpParamValue.Copy(KTEFSetUpParam);
				iSetUpParamValue.AppendNum(TTestSetupState(ESetupNone));
				}

			// Append a space and also include the loop index
			iSetUpParamValue.Append(KTEFSpace);
			// The loop index is 0 if the test is not in loop; loop index, otherwise
			iSetUpParamValue.AppendNum(iLoopIndex);
			iSetUpParamValue.Append(KTEFSpace);
			
			// Append the TestCaseID along with the Setup params
			iSetUpParamValue.Append(iTestCaseID);
			iSetUpParamValue.Append(KTEFSpace);

			// Insert the !Setup arguments after the OOM arguments within the iStepArgs
			iStepArgs.Insert(6, iSetUpParamValue);
			}
		
		// Call the async client API to run the test step
		if(iBlockArrayPkg)
			{
			iSession.RunTestBlock(iStepArgs,iTaskExitCategory,iBlockArrayPtr,iStatus);
			}
		else
			{
			iSession.RunTestStep(iStepArgs,iTaskExitCategory,iStatus);
			}
		// Start a timer and provide it with our callback MTaskCompletion
		iState = ERunning;
		// Only start the timer if the disable value is not set
		if(iTaskTimeout != KDisableTimer)
			{
			iTaskTimer = CTaskTimer::NewL(*this);
			iTaskTimer->Timeout(iTaskTimeout);
			}
		}
		break;

	case ERunning :
		{
		// We have been completed but we need to find out the source
		// If the timer expires it Aborts the server test step which results in
		// the RunTestStep call completing with KErrAbort.
		// When the timer expires it calls us back and we delete it then NULL it.
		if(iTaskTimer)
			{
			// The timer did not expire
			iTaskTimer->Cancel();
			delete iTaskTimer;
			iTaskTimer = NULL;
			}
		const TInt KRetry10Microseconds = 100000;
		const TInt KMaxRetryCount = 20;
		if(iStatus.Int() == KErrInUse && iRetryCount < KMaxRetryCount)
			{
			// OS (not the server) has rejected the call.
			// Timed retry
			// Noticed this one after panic's in shared data mode
			iState = EServerRetry;
			// Keep increasing the timeout
			iRetryCount++;
			SetActive();
			iLogger.LogExtra((TText8*)__FILE__,__LINE__,ESevrInfo,
					_L("Error Message :: The Task Is Retrying "));
			iTimer.After(iStatus,KRetry10Microseconds * iRetryCount);							
			}		
		else if(iStatus.Int() == KErrServerBusy)
			{
			// tempt to do test again when test server return busy.
			iState = EServerRetry;
			iRetryCount++;
			if(iRetryCount < KMaxRetryCount)
				{
				iState = EServerRetry;
				SetActive();
				iLogger.LogExtra((TText8*)__FILE__,__LINE__,ESevrInfo,
						_L("Error Message : Server return -16 = KErrServerBusy : Retrying %d"),iRetryCount);
				iTimer.After(iStatus,KRetry10Microseconds );
				}
			else//server alwayse busy. stop test. 
				{
				iLogger.LogExtra((TText8*)__FILE__,__LINE__,ESevrErr,
										_L("Error Message : Server Busy Retrying %d times. Test Teminated!"),iRetryCount);
				iParent.TaskCompletion(iStatus.Int(),iTaskExitCategory,iScriptLineNumber,iCommandLine,iTaskCanComplete);

				iSession.Close();
				delete this;				
				}
			}			
		else
			{
			// see testserverbase.cpp::void SytemWideErrToTefErr(TInt &aErr). converting thi value.
			//Check the status, if the status is KErrTestExecuteInUse
			//then log the information  and change it back to 
			if(iStatus.Int() == KErrTestExecuteInUse)
				{
				//Convert the status back to KErrInUse
				iStatus=KErrInUse;
				}
			else if(iStatus.Int() == KErrTestExecuteServerBusy)
				{
				//this is not server really busy! we should change the server side iStatus back to -16
				//Convert the status back to KErrServerBusy
				iStatus = KErrServerBusy;
				}
			// Step completion
			TestStepComplete();
			delete this;
			}
		}
		break;

	default:
		break;
		}
	}

/**
 * @param aError - TInt value representing error returned due to processing request from RunL()
 * Handles the leave from RunL() and cleans up the allocated objects
 * Also returns KErrNone upon sucessful handling of leave
 */
TInt CClientControl::RunError(TInt aError)
	{
	if(iBlockArrayPkg)
		{
		TTEFItemArray*	itemArray = CreateBlockArrayLC();
		iParent.TaskCompletion(aError,iTaskExitCategory,iScriptLineNumber,iEndBlockCommand,iTaskCanComplete,itemArray);
		CleanupStack::PopAndDestroy( itemArray );
		}
	else
		{
		iParent.TaskCompletion(aError,iTaskExitCategory,iScriptLineNumber,iCommandLine,iTaskCanComplete);
		}
	delete this;
	return KErrNone;
	}

/**
 * Extract and save the timeout, step name and test step arguments
 */
void CClientControl::GetStepParamsFromStepCommand()
	{
	TLex scriptLineLex(iCommandLine);
	TPtrC command(scriptLineLex.NextToken());
	if(command.CompareF(KTEFRunTestStepResultCommand) == 0)
		scriptLineLex.NextToken();
	else if(command.CompareF(KTEFRunPanicStepResultCommand) == 0)
		{
		scriptLineLex.NextToken();
		TPtrC panicString(scriptLineLex.NextToken());
		if(panicString.Left(1).Compare(KTEFOpenQuotes) == 0 && panicString.Right(1).Compare(KTEFOpenQuotes) != 0)
			{
			TBool validCommandLine(EFalse);
			while(!scriptLineLex.Eos() && !validCommandLine)
				{				
				panicString.Set(scriptLineLex.NextToken());
				if(panicString.Right(1).Compare(KTEFOpenQuotes) == 0)
					validCommandLine = ETrue;
				}
			}
		}
	else if(command.CompareF(KTEFRunTestStepCommand) == 0 ||
			command.CompareF(KTEFStartTestBlock) == 0 )
		{
		TInt firstChar;
		TPtrC commandStr;
		TBool panicStringComplete(ETrue);
		while(!scriptLineLex.Eos())
			{
			scriptLineLex.SkipSpace();
			scriptLineLex.Mark();
			
			TPtrC	token = scriptLineLex.NextToken();
			if( token.Length()>0 )
				{
				commandStr.Set( token );
				firstChar = commandStr[0];
				// 33 is the ascii value for "!". Used here for confirming switches
				if (firstChar != KTEFAsciiExclamation && panicStringComplete)
					{
					break;
					}
				if(commandStr.Length() > 14 && 
					commandStr.Mid(0,14).Compare(_L("!PanicString=\"")) == 0)
						panicStringComplete = EFalse;
				if(!panicStringComplete && commandStr.Right(1).Compare(KTEFOpenQuotes) == 0)
					panicStringComplete = ETrue;
				if (commandStr.Length() >= KTEFMinHeapParamLength && commandStr.Mid(0,KTEFMinHeapParamLength).CompareF(KTEFHeap) == 0)
					iTaskHeapSize.Set(commandStr.Mid(KTEFMinHeapParamLength));
				else if (commandStr.Length() >= KTEFMinSetupParamLength && commandStr.Mid(0,KTEFMinSetupParamLength).CompareF(KTEFSetUpParam) == 0)
					iSetUpParamValue.Copy(commandStr);
				else if (commandStr.Length() >= KTEFMinOomParamLength && commandStr.Mid(0,KTEFMinOomParamLength).CompareF(KTEFOom) == 0)
					{
					// Out of memory cannot be supported for UREL builds due to UHEAP macros
					// not being supported in UREL builds
					#if defined(_DEBUG)
						iOOMRequestSet = ETrue;
					#else
						iOOMRequestSet = EFalse;
						iLogger.LogExtra((TText8*)__FILE__,__LINE__,ESevrWarn,
								_L("Out of Memory Testing is not supported for UREL builds"));
					#endif
					}
				}
			}
		}

	TLex timeoutLex;
	if(	command.CompareF(KTEFRunTestStepCommand) == 0 ||
		command.CompareF(KTEFStartTestBlock) == 0)
		timeoutLex = scriptLineLex.MarkedToken();
	else
		timeoutLex = scriptLineLex.NextToken();
	TInt err = timeoutLex.Val(iTaskTimeout);
	if(err)
		{
		// No timeout specified
		iTaskTimeout = KDefaultTimeoutSeconds;
		iStepName.Set(scriptLineLex.NextToken());
		}
	else
		{
		scriptLineLex.NextToken();
		if( command.CompareF(KTEFRunTestStepCommand) == 0 ||
			command.CompareF(KTEFRunTestStepResultCommand) == 0 ||
			command.CompareF(KTEFRunPanicStepResultCommand) == 0 ||
			command.CompareF(KTEFRunPanicStepCommand) == 0||
			command.CompareF(KTEFRunErrorStepResultCommand) == 0)
			{
			// Save the step name
			iStepName.Set(scriptLineLex.NextToken());
			}
		}
	// The rest is sent to the server
	scriptLineLex.SkipSpace();
  	iStepArgs.Copy(iScriptFilePath.Mid(0,2));
  	iStepArgs.Append(KTEFSpace);
	iStepArgs.Append(iDefaultSysDrive);
	iStepArgs.Append(KTEFSpace);
	iStepArgs.Append(iTestSysDrive);
	iStepArgs.Append(KTEFSpace);
  	iStepArgs.Append(scriptLineLex.Remainder());
	}

/**
 * Callback from a CTaskTimer object
 * Abort the test step, NULL the timer so we know that a timeout has caused test step completion
 */
void CClientControl::TaskTimerCompletion()
	{
	delete iTaskTimer;
	iTaskTimer = NULL;
	iSession.AbortTestStep();
	}

/**
 * Constructor
 * @param aCompletion - Reference to a callback method in the CClientControl parent object
 */
CTaskTimer::CTaskTimer(MTaskTimerCompletion& aCompletion) : CTimer(EPriorityUserInput), iParent(aCompletion)
	{
	CActiveScheduler::Add(this);
	}

/**
 * Iterate till the timer has expired and callback into the parent
 */
void CTaskTimer::RunL()
	{
	if (iTimesAfter > 0 || iSecondsRemaining > 0)
		{
		RunTimeout(); // Function call implementing the After() routines
		}
	else
		{
		iParent.TaskTimerCompletion(); // Call the completion routine for the timeout
		}
	}

/**
 * Implements a timeout based using CTimer::After()
 * Modified for handling huge timeout values
 * @param aSeconds - Timeout value in seconds
 */
 void CTaskTimer::Timeout(TInt aSeconds)
	{
	iTimesAfter = aSeconds/60; // Convert the aSeconds to equivalent minutes
	iSecondsRemaining = (aSeconds - (iTimesAfter*60)); // Remainder of seconds after converting to equivalent minutes
	RunTimeout();
	}

/**
 * Implements the After() routines for the timeout value specified
 * The call is initiated from Timeout() & is iterated from the RunL()
 */
void CTaskTimer::RunTimeout()
	{
	if (iTimesAfter > 0)
		{
		After(60*1000000); // Call After() for every minute until iTimesAfter is >0
		iTimesAfter--;
		}
	else if (iSecondsRemaining > 0)
		{
		After(iSecondsRemaining*1000000); // Call After() for remainder of microsec
		iSecondsRemaining = 0;
		}
	}

/**
 * Destructor
 */
CTaskTimer::~CTaskTimer()
	{
	}

/*
 * @param aCompletion - Reference to a callback method in a CTaskControlBase parent object
 * Two phase contruction
 */
CTaskTimer* CTaskTimer::NewL(MTaskTimerCompletion& aCompletion)
	{
	CTaskTimer* taskTimer = new(ELeave) CTaskTimer(aCompletion);
	CleanupStack::PushL(taskTimer);
	// We have to call the base class second phase constructor
	taskTimer->ConstructL();
	CleanupStack::Pop(taskTimer);
	return taskTimer;
	}

/**
 * @param aCommand - Command for the derived class
 * @param aCompletion - Reference to the parent class callback method
 * @param aScriptLineNumber - The script file line number of this command
 * @param aLogger - Reference to a Flogger derived session
 * Constructor - Just initialise the abstract class data
 */
CTaskControlBase::CTaskControlBase(const TDesC& aCommand, MTaskCompletion& aCompletion,TInt aScriptLineNumber, CTestExecuteLogger& aLogger) :
	iCommandLine(aCommand),
	iParent(aCompletion),
	iScriptLineNumber(aScriptLineNumber),
	iLogger(aLogger),
	iTaskTimer(NULL)
	{
	}

/**
 * Destructor
 */
CTaskControlBase::~CTaskControlBase()
	{
	}

/**
 * @param aCommand - Command line for the program (Store in base class)
 * @param aCompletion - Reference to the parent class callback method (Store in base class)
 * @param aScriptLineNumber - The script file line number of this command (Store in base class)
 * @param aLogger - Reference to a Flogger derived session (Store in base class)
 * @param aWSProgRun - Boolean value used for identifying window server programs
 */
CProgramControl::CProgramControl(const TDesC& aCommand,MTaskCompletion& aCompletion,TInt aScriptLineNumber, CTestExecuteLogger& aLogger, TBool aWSProgRun) : CTaskControlBase(aCommand,aCompletion,aScriptLineNumber,aLogger),
	iState(EInit)
	{
 	// Extract the program arguments from the command line
	GetProgramArgsFromCommand();

	// Try to connect to the window server (if available)
	iWSProgRun = aWSProgRun;
  	#if !defined TEF_LITE
	if( iWSProgRun )
		{
		iWs.Connect();
		}
  	#else
  	// Always set to false if TEF_LITE is set
  	//iWSProgRun = EFalse;
  	#endif
	}

/**
 * Destructor
 */
CProgramControl::~CProgramControl()
	{
	#if !defined TEF_LITE
	if( iWSProgRun )
		{
		iWs.Close();
		}
	#endif
	}

/**
 * Simple State machine.
 * Kick()'d into by parent object.
 * Runs an executable in its own process.
 */
void CProgramControl::RunL()
	{
	switch(iState)
		{
	case EInit	:
		{
		TRAPD(err,RunProgramL());
		if(err != KErrNone)
			{
			// Special case where the executable has already completed
			if( err == KErrCompletion )
				{
				// Reset the error code
				err = KErrNone;
				}
			// Failed so complete immediately
			iParent.TaskCompletion(err,iTaskExitCategory,iScriptLineNumber,iCommandLine,iTaskCanComplete);
			delete this;
			}
		else
			{
			SetActive();
			iState = ERunning;
			// Only start the timer if the disable value is not set
			if(iTaskTimeout != KDisableTimer)
				{
				iTaskTimer = CTaskTimer::NewL(*this);
				iTaskTimer->Timeout(iTaskTimeout);
				}
			if (iProgRenamed == 1)
 				{
  				iLogger.LogExtra((TText8*)__FILE__,__LINE__,ESevrInfo,
  					_L(".EXE not found - .APP run instead"));
  				}

			}
		}
		break;

	case ERunning :
		{
		if(iTaskTimer)
			{
			// The timer did not expire
			iTaskTimer->Cancel();
			delete iTaskTimer;
			iTaskTimer = NULL;
			}
		// Set up the panic string if the program panicked
		if( !iWSProgRun )
			{
			if(iProgram.ExitType() == EExitPanic)
				{
				iTaskExitCategory.Copy(KPanicEquals);
				iTaskExitCategory.Append(iProgram.ExitCategory());
				}
			else if (iProgram.ExitType() == EExitPending)
				{
				iProgram.Kill(KErrAbort);
				iLogger.LogExtra((TText8*)__FILE__,__LINE__,ESevrWarn,
				  	_L("Program has been killed as the timeout is achieved."));
				}
			iProgram.Close();
			}
  		#if !defined TEF_LITE
		else
			{
			if(iProgramWS.ExitType() == EExitPanic)
				{
				iTaskExitCategory.Copy(KPanicEquals);
				iTaskExitCategory.Append(iProgramWS.ExitCategory());
				}
                
		    // Delete dependence of MW apparc.lib. 
		    // apparc.lib is moved from OS layer to MW layer. 
		    // TEF will delete this dependece because TEF is built on OS layer.
			iProgramWS.Close();
			}
		#endif
		
		// Complete to the parent object.
		iParent.TaskCompletion(iStatus.Int(),iTaskExitCategory,iScriptLineNumber,iCommandLine,iTaskCanComplete);
		delete this;
		}
		break;

	default:
		break;
		}
	}

/**
 * Retrieve the program arguments from the script line.
 */
void CProgramControl::GetProgramArgsFromCommand()
	{
	TLex scriptLineLex(iCommandLine);
	// Skip the RUN_PROGRAM command
	scriptLineLex.NextToken();
	TLex lex(scriptLineLex);
	// Get the timer if it's been included
	TLex timeoutLex(lex.NextToken());
	TInt err = timeoutLex.Val(iTaskTimeout);
	if(err)
		// No timeout specified
		iTaskTimeout = KDefaultTimeoutSeconds;
	else
		// Skip the timeout value
		scriptLineLex.NextToken();
	scriptLineLex.SkipSpace();
	// Use the rest
	iProgramArgs.Set(scriptLineLex.Remainder());
	}

/**
 * Task timer expired
 */
void CProgramControl::TaskTimerCompletion()
	{
	delete iTaskTimer;
	iTaskTimer = NULL;
		if( !iWSProgRun )
			{
			iProgram.RendezvousCancel(iStatus);
			}
		#if !defined TEF_LITE
		else
			{
			iProgramWS.RendezvousCancel(iStatus);
			}
		#endif
		// We ORPHAN the process
		// Our RunL() gets called immediately
		// Kill requires KillAnyProcess capability
		// ups the stakes as regards CAPABILITY
	}

/**
 * Processes RUN_PROGRAM command arguments and starts the program
 */
void CProgramControl::RunProgramL()
	{
	TLex lex(iProgramArgs);
	//	Get program name.	
	lex.Mark();
	lex.NextToken();
#if   !defined TEF_LITE
	// Set up the program arguments
	TBuf<KMaxTestExecuteCommandLength> programArgs(lex.Remainder());
	programArgs.Trim();
	iProgramArgs.Set(programArgs);

	if( !iWSProgRun )
		{
		// Create the Process
		User::LeaveIfError(iProgram.Create(lex.MarkedToken(),iProgramArgs));
		}
	else
		{
 		if( !IsWindowServerAvailable() )
 			{
 			User::Leave(KErrNotSupported);
 			}
 	    // Delete dependence of MW apparc.lib. 
 	    // apparc.lib is moved from OS layer to MW layer. 
 	    // TEF will delete this dependece because TEF is built on OS layer.
 		User::LeaveIfError(iProgramWS.Create(lex.MarkedToken(),iProgramArgs));
		}
#else
/**
 * Kick off an exe in its own pocess
 * completes asynchronously
 */
	
	if (iWSProgRun)  //TEF lite doesn't support RUN_WS_PROGRAM.
		{
		iLogger.LogExtra((TText8*)__FILE__,__LINE__,ESevrErr,
				_L("Error Message :: TEF lite doesn't support RUN_WS_PROGRAM."));
		User::Leave(EFail);
		}
	
	// Set up the program arguments
	TPtrC commandLine(lex.Remainder());
	iProgramArgs.Set(commandLine);
	// Create the Process
	User::LeaveIfError(iProgram.Create(lex.MarkedToken(),iProgramArgs));

#endif 
	if( !iWSProgRun )
		{
		iProgram.Rendezvous(iStatus);
		iProgram.Resume();
		}
	#if !defined TEF_LITE
	else
		{
		iProgramWS.Logon(iStatus);
        iProgramWS.Resume();
		}
	#endif
	}

/**
 * @param aServerName - Test server to connect to
 * @param aSharedData - Flag for shared data mode session with test server
 * Connect to the test server
 * EKA2 version requires a just in time debug flag for the test server process
 * This is read from the command line.
 * Base class connect acts on this.
 */
TInt RScriptTestServ::Connect(const TDesC& aServerName,TBool aSharedData)
	{
	iSharedData = aSharedData;
	// Parse the command line for -d
	TBuf<KMaxTestExecuteCommandLength> commandLine;
	if(User::CommandLineLength() > commandLine.MaxLength())
		User::Leave(KErrTooBig);
	User::CommandLine(commandLine);
	TLex flagLex(commandLine);
	// Default to false
	TBool aJustInTime(EFalse);
	while(!flagLex.Eos())
		{
		TPtrC token(flagLex.NextToken());
		if(token == KTestExecuteCommandLineFlagDebugMode)
			{
			aJustInTime = ETrue;
			break;
			}
		}
	return RTestServ::Connect(aServerName,aJustInTime);
	
	}

/**
 * Copies the integer value read from ini file to the reference integer passed to the function
 * @param aConfigData - Pointer to CIniData object used for reading data from ini file
 * @param aConfigSection - Descriptor value describing the section name within an ini file for reading
 * @param aSharedData - KeyName within a section where the input data is available in ini file
 * @param aSharedDataNum - Reference integer variable for collecting the value at the keyname specified
 * @Leave system wide errors
 */
void CScriptControl::FindValueL(CIniData* aConfigData, TPtrC aConfigSection, TPtrC aSharedData, TInt& aSharedDataNum)
	{
	aConfigData->FindVar(aConfigSection, aSharedData, aSharedDataNum);
	if (aSharedDataNum == 0)
		{
		User::Leave(KErrGeneral);
		}
	}

/**
 * Copies the descriptor value read from ini file to the reference descriptor passed to the function
 * @param aConfigData - Pointer to CIniData object used for reading data from ini file
 * @param aConfigSection - Descriptor value describing the section name within an ini file for reading
 * @param aSharedData - KeyName within a section where the input data is available in ini file
 * @param aSharedDataName - Reference descriptor variable for collecting the value at the keyname specified
 * @Leave system wide errors
 */
void CScriptControl::FindValueL(CIniData* aConfigData, TPtrC aConfigSection, TPtrC aSharedData, TPtrC& aSharedDataName)
	{
	User::LeaveIfError(aConfigData->FindVar(aConfigSection, aSharedData, aSharedDataName));
	TPtrC blankString(KNull);
	if (aSharedDataName.CompareF(blankString) == 0)
		{
		User::Leave(KErrGeneral);
		}
	}

/**
 * Creates one or more shared objects based on the inputs provided in ini file
 * @Leave system wide errors
 */
void CScriptControl::CreateSharedObjectsFromScriptLineL()
	{
	CIniData* configData = NULL;
	TInt err = 0;
	TInt sharedDataCount = 0;
	TPtrC sharedDataName;

	// Sets the boolean to ETrue
	// Avoids SHARED_DATA command being called more than once
	iIsSharedData = ETrue;
	
	//It copies the current line from the script to TLex object
	TLex shareLex(iCurrentScriptLine);

	//reads the next word
	shareLex.NextToken();

	TPtrC configFile(shareLex.NextToken());

	TPtrC configSection(shareLex.NextToken());

	if(configFile.Length())
		{
		// Create instance of CIniData for reading ini input
		TRAP(err,configData = CIniData::NewL(configFile));
		}
	if(err != KErrNone)
		{
		ERR_PRINTF1(KTEFErrorReadingIni);
		User::Leave(err);
		}
	if(configData)
		{
		CleanupStack::PushL(configData);
		TPtrC sharedDataNumberKey(KTEFSharedDataNum);
		// Read the ini file for number of shared objects to be created
		// Store the value into variable sharedDataCount
		TRAP(err,FindValueL(configData,configSection,sharedDataNumberKey,sharedDataCount));
		if (err != KErrNone)
			{
			ERR_PRINTF1(KTEFErrNumberOfShareNotInIni);
			}
		else
			{
			// If ini input is available for number of shared data
			// Run a loop to read individual shared objects name
			for (TInt i=1; i<=sharedDataCount; i++)
				{
				TBuf<20> keyName(KTEFSharedName);

				keyName.AppendNum(i);

				keyName.ZeroTerminate();

				TPtrC sharedDataNameKey(keyName);

				sharedDataName.Set(KNull);

				// Read ini file for i th object name
				TRAP(err,FindValueL(configData,configSection,sharedDataNameKey,sharedDataName));
				if (err != KErrNone)
					{
					ERR_PRINTF2(KTEFErrShareNameNotInIni,i);
					}
				else
					{
					// Number of shared data is limited to the value set in the constant KTEFMaxSharedArraySize
					// The constant is defined in ScriptEngine.h which can be extended from there
					if (iSharedDataNum < KTEFMaxSharedArraySize)
						{
						// If ini input is available for i th object name
						// Creating template class object for sharing the object
						CTestSharedData* sharedObject = NULL;

						CTEFSharedData<CTestSharedData>* sharedData1 = NULL;
						TRAP(err,sharedData1 = CTEFSharedData<CTestSharedData>::NewL(sharedObject, KTEFSharedDataSize, sharedDataName));
						if (err != KErrNone)
							{
							User::Leave(err);
							}
						else
							{
							sharedData1->EnterCriticalSection();
							sharedObject->Construct();
							sharedData1->ExitCriticalSection();
							// Adding the template object to an array
							iSharedDataArray[iSharedDataNum] = sharedData1;
							// Counting the number of objects created
							// destructor can destroy the objects based on this count
							iSharedDataNum++;
							}
						}
					else
						{
						WARN_PRINTF1(KTEFMaxNumberOfSharedObjectsReached);
						}
					}
				}
			}
		// Cleanup CInidata object
		CleanupStack::PopAndDestroy(configData);
		}
	}
	
/**
 * Parses the test block header
 * @Return ETrue if pass ok else EFalse
 */
// Fix defect 118337, check the configuration file exists or not, and set the flag.
TBool CScriptControl::ParseTestBlockHeader()
	{
	TLex lex(iCurrentScriptLine);
	TPtrC token = lex.NextToken();
	TInt paraCount = 1;
	const TInt KLeastBlockHeaderParaCount = 4;
	_LIT(KLegacySysDrive, "C:");
	_LIT(KDefaultIniFilePath, "\\System\\Data\\");
	
	while (!lex.Eos())
		{
		token.Set(lex.NextToken());
		lex.SkipSpace();
		++paraCount;
		}
	
	if (paraCount < KLeastBlockHeaderParaCount)
		{
		_LIT(KBadError, "Bad error syntax!");
		ERR_PRINTF1(KBadError);
		return EFalse;
		}

	TFileName defaultIniFilePath(KLegacySysDrive);
	defaultIniFilePath.Append(KDefaultIniFilePath);
	
	iFS.Connect();
	TFindFile fileFind(iFS);
	TInt ret = fileFind.FindByDir(token, defaultIniFilePath);
	iFS.Close();
	if (KErrNotFound == ret)
		{
		_LIT(KFileNotFound, "Configuration File %S Not Found!");
		ERR_PRINTF2(KFileNotFound, &token);
		return EFalse;
		}
	return ETrue;
	}
// End defect 118337

/**
 * Parses the test block and populates the iBlockArray
 * @Leave system wide errors
 */
void CScriptControl::ParseTestBlockL( TTEFItemArray& aItemArray )
	{
	// Iterate through the commands contained within the test block
	//  and append them to the itemArray.
	TPtrC	startBlockScriptLine	= iCurrentScriptLine;
	TInt	startBlockLineNumber 	= iCurrentScriptLineNumber;
	
	TBool parseResult = ParseTestBlockHeader();
	
	FOREVER
		{
		TTEFBlockItem	blockItem;
		
		// Read in the next script line
		TBool	scriptLineRes = EFalse;
		if( !iStartLooping )
			{
			scriptLineRes = GetNextScriptLine(iCurrentScriptLine);
			}
		else
			{
			scriptLineRes = GetLoopScriptLine(iCurrentScriptLine);
			}
		
		if( !scriptLineRes )
			{
			_LIT( KTEFEoF, "Unexpectedly hit the end of the file" );
			ERR_PRINTF1( KTEFEoF );
			TExitCategoryName  blankPanicString;
			LogResult(EFail, blankPanicString, startBlockLineNumber, startBlockScriptLine);
			User::Leave( KErrEof );
			}

		// Strip off any trailling comment
		TInt offset = iCurrentScriptLine.Find(KTEFComment);
		if( offset != KErrNotFound )
			{
			iCurrentScriptLine.Set( iCurrentScriptLine.Mid(0, offset) );
			}

		TLex lex(iCurrentScriptLine);
		TPtrC token(lex.NextToken());
		
		// Populate the blockItem with the data required for each command
		
		// CREATE_OBJECT <object type> <object name section>
		if(	0 == token.CompareF(KTEFCreateObject) )
			{
			blockItem.iItemType		= ETEFCreateObject;
			blockItem.iObjectType	= lex.NextToken().Left(KTEFMaxNameLength);
			blockItem.iSection		= lex.NextToken().Left(KTEFMaxNameLength);
			}
		// RESTORE_OBJECT <object type> <object name section>
		else if( 0 == token.CompareF(KTEFRestoreObject) )
			{
			blockItem.iItemType		= ETEFRestoreObject;
			blockItem.iObjectType	= lex.NextToken().Left(KTEFMaxNameLength);
			blockItem.iSection		= lex.NextToken().Left(KTEFMaxNameLength);
			}
		// COMMAND [Error TEFParameter] <object name section> <function name> [section]
		else if( 0 == token.CompareF(KTEFCommand) )
			{
			blockItem.iItemType = ETEFCommand;
			TPtrC param = lex.NextToken().Left(KTEFMaxNameLength);
			
			if( param.Length() > 0 )
				{
				while( param[0] == KTEFAsciiExclamation )
					{
					// This is a TEF Error Parameter
					// Extract the type of TEF parameter being read in
					if( param.Length() >= KTEFError().Length() &&
						0 == param.Mid(0,KTEFError().Length()).CompareF(KTEFError) )
						{
						TInt	start = KTEFError().Length();
						TInt	length = param.Length()-start;
						TLex	errorCodeLex( param.Mid(start,
														length ));
						
						TInt errorCode = 0;
						if( errorCodeLex.Val(errorCode) == KErrNone )
							{
							blockItem.iExpectedError = errorCode;
							}
						else
							{
							_LIT(KBadError,"Bad error syntax.");
							ERR_PRINTF1(KBadError);	
							blockItem.iError = KErrNotSupported;
							}
						}
					else if( param.Length() >= KTEFAsyncError().Length() &&
							 0 == param.Mid(0,KTEFAsyncError().Length()).CompareF(KTEFAsyncError))
						{
						TInt	start = KTEFAsyncError().Length();
						TInt	length = param.Length()-start;
						TLex	errorCodeLex( param.Mid(start,
														length ));
						
						TInt errorCode = 0;
						if( errorCodeLex.Val(errorCode) == KErrNone )
							{
							blockItem.iExpectedAsyncError = errorCode;
							}
						else
							{
							_LIT(KBadError,"Bad error syntax.");
							ERR_PRINTF1(KBadError);
							blockItem.iAsyncError = KErrNotSupported;					
							}
						}
					else
						{
						// Unknown TEFParameter
						_LIT(KUnknownTEFParam,"Unknown Test Block TEFParameter.");
						ERR_PRINTF1(KUnknownTEFParam);
						TExitCategoryName  blankPanicString;
						LogResult(EFail, blankPanicString, startBlockLineNumber, startBlockScriptLine);
						User::Leave(KErrNotSupported);
						}

					// Read the next token
					param.Set( lex.NextToken().Left(KTEFMaxNameLength) );
					}
				}
				
			// Read in the object section name, function name and optional data section
			blockItem.iCommand.iObject		= param;
			blockItem.iCommand.iFunction	= lex.NextToken().Left(KTEFMaxNameLength);
			blockItem.iSection				= lex.NextToken().Left(KTEFMaxNameLength);
			}
		// STORE <section>
		else if( 0 == token.CompareF(KTEFStore) )
			{
			blockItem.iItemType		= ETEFStore;
			blockItem.iSection		= lex.NextToken().Left(KTEFMaxNameLength);
			}
		// OUTSTANDING <poll interval in ms>
		else if( 0 == token.CompareF(KTEFOutstanding) )
			{
			blockItem.iItemType		= ETEFOutstanding;
			TLex	pollLex( lex.NextToken().Left(KTEFMaxNameLength) );
			TInt	poll = 0;
			blockItem.iTime			= (KErrNone==pollLex.Val(poll)?poll:0);
			blockItem.iSection		= lex.NextToken().Left(KTEFMaxNameLength);		
			}
		// DELAY <time in ms>
		else if( 0 == token.CompareF(KTEFDelay) )
			{
			blockItem.iItemType		= ETEFDelay;
			TLex	delayLex( lex.NextToken().Left(KTEFMaxNameLength) );
			TInt	delay = 0;
			blockItem.iTime = (KErrNone==delayLex.Val(delay)?delay:0);		
			}
		// ASYNC_DELAY <time in ms>
		else if( 0 == token.CompareF(KTEFAsyncDelay) )
			{
			blockItem.iItemType		= ETEFAsyncDelay;
			TLex	delayLex( lex.NextToken().Left(KTEFMaxNameLength) );
			TInt	delay = 0;
			blockItem.iTime = (KErrNone==delayLex.Val(delay)?delay:0);
			}
		// SHARED_ACTIVE_SCHEDULER
		else if( 0 == token.CompareF(KTEFSharedActiveScheduler) )
			{
			blockItem.iItemType		= ETEFSharedActiveScheduler;			
			}
		// STORE_ACTIVE_SCHEDULER
		else if( 0 == token.CompareF(KTEFStoreActiveScheduler) )
			{
			blockItem.iItemType		= ETEFStoreActiveScheduler;			
			}
		// END_TEST_BLOCK [Scheduler Cleanup]
		else if( 0 == token.CompareF(KTEFEndTestBlock) )
			{
			TExitCategoryName  blankPanicString;
			if( 0 == aItemArray.Count() )
				{
				_LIT( KTEFEmptyBlock, "The Test Block is empty." );
				ERR_PRINTF1( KTEFEmptyBlock );
				LogResult(EFail, blankPanicString, startBlockLineNumber, startBlockScriptLine);
				User::Leave(KErrNotFound);
				}
			else
				{
				// Hit the end of the test block and the package is ready
				// Fix defect 118337, check parse result, and print corresponding message.
				LogResult(parseResult?EPass:EFail, blankPanicString, startBlockLineNumber, startBlockScriptLine);
				//End defect 118337
				break;
				}
			}
		else if( 0 == token.CompareF(KTEFStartTestBlock) )
			{
			// Error - there was no end test block command
			_LIT(KMissingEnd,"Missing END_TEST_BLOCK command.");
			ERR_PRINTF1(KMissingEnd);
			TExitCategoryName  blankPanicString;
			LogResult(EFail, blankPanicString, startBlockLineNumber, startBlockScriptLine);
			User::Leave(KErrNotFound);
			}
		else
			{
			if( token.Length() > 0 )
				{
				if( 0 != token.Left(2).Compare(KTEFComment) )
					{
					// The SART_TEST_BLOCK command failed
					_LIT(KUnknownCmd,"Unknown Test Block command.");
					ERR_PRINTF1(KUnknownCmd);
					TExitCategoryName  blankPanicString;
					LogResult(EFail, blankPanicString, startBlockLineNumber, startBlockScriptLine);
		
					// Special case for END_TESTCASE commands
					//  If one has been hit here because an END_TEST_BLOCK command was missing
					//  the we must log its completion.
					if( token.CompareF(KTEFEndTestCaseCommand) == 0 || token.CompareF(KTEFEndSyncTestCaseCommand) == 0 )
						{
						LogTestCaseMarkerL();
						}
					User::Leave(KErrNotSupported);
					}
				}
			}
			
		// Append the blockItem to the package
		if( ETEFNull != blockItem.iItemType )
			{
			aItemArray.AppendL( blockItem );
			}
		}
	}


_LIT( KTEFSyncStatus, "TEFStatus" );
_LIT( KTEFSyncResult, "TEFResult" );
const TInt KDelay		= 3000000;
const TInt KRetryCount	= 10;

enum TSyncStatus
	{
	ETEFSyncUnknown		= 0,
	ETEFSyncRunning		= 1,
	ETEFSyncComplete	= 2,
	ETEFSyncWaiting		= 3,
	ETEFSyncContinue	= 4,
	ETEFRetrieveResult	= 5
	};

/**
 * Constructor
 */
CSyncControl::CSyncControl()
	: iSharedTEFStatus(NULL), iSharedTEFResult(NULL)
	{
	}
/**
 * Destructor
 */
CSyncControl::~CSyncControl()
	{
	if( iSharedTEFResult && iSharedTEFStatus )
		{
		// Don't cleanup until the status is correct
		// This will allow time for any retrieve result calls to get through before TEF exits
		// Will retry for 30 seconds
		CTestSharedData* tefStatus = iSharedTEFStatus->Ptr();
		if( tefStatus != NULL )
			{
			TInt64 status = ETEFRetrieveResult;
			TInt count = 0;
			while( status == ETEFRetrieveResult && count < KRetryCount )
				{
				iSharedTEFStatus->EnterCriticalSection();
				HBufC* statusBuffer = NULL;
				TRAPD( err, statusBuffer = HBufC::NewL(tefStatus->TextLength()) );
				if( err == KErrNone )
					{
					TPtr statusPtr( statusBuffer->Des() );
					tefStatus->GetText( statusPtr );
					TLex lex(statusPtr);
					lex.Val(status);
					delete statusBuffer;
					}
				iSharedTEFStatus->ExitCriticalSection();

				// Don't deley for the first i
				if( count > 0 )
					{
					User::After( KDelay );
					}
				count++;
				}
			}
		delete iSharedTEFResult;
		delete iSharedTEFStatus;		
		}
	}

/**
 * Two phase construction
 */	
CSyncControl* CSyncControl::NewL()
	{
	CSyncControl* self = CSyncControl::NewLC();
	CleanupStack::Pop();
	return self;
	}

/**
 * Two phase construction
 */	
CSyncControl* CSyncControl::NewLC()
	{
	CSyncControl* self = new (ELeave) CSyncControl();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

/**
 * ConstructL
 */	
void CSyncControl::ConstructL()
	{
	// Initialise the sync status shared data
	CTestSharedData* tefStatus = NULL;
	iSharedTEFStatus = CTEFSharedData<CTestSharedData>::NewL(	tefStatus,
																KMaxSharedDataLength,
																KTEFSyncStatus );
	iSharedTEFStatus->EnterCriticalSection();
	tefStatus->Construct();
	HBufC* statusBuffer = NULL;
	TRAPD( err, statusBuffer = HBufC::NewL(sizeof(TInt64)) );
	if( err != KErrNone )
		{
		iSharedTEFStatus->ExitCriticalSection();
		User::Leave( err );
		}
	TPtr statusPtr( statusBuffer->Des() );
	statusPtr.Num( ETEFSyncWaiting );
	tefStatus->SetText( statusPtr );
	delete statusBuffer;
	iSharedTEFStatus->ExitCriticalSection();

	// Initialise the sync result shared data
	CTestSharedData* tefResult = NULL;
	iSharedTEFResult = CTEFSharedData<CTestSharedData>::NewL(	tefResult,
																KMaxSharedDataLength,
																KTEFSyncResult );
	iSharedTEFResult->EnterCriticalSection();
	tefResult->Construct();
	HBufC* resBuffer = NULL;
	TRAP( err, resBuffer = HBufC::NewL(sizeof(TInt64)) );
	if( err != KErrNone )
		{
		iSharedTEFResult->ExitCriticalSection();
		User::Leave( err );
		}
	TPtr resPtr( resBuffer->Des() );
	resPtr.Num( EInconclusive );
	tefResult->SetText( resPtr );
	delete resBuffer;
	iSharedTEFResult->ExitCriticalSection();
	}

/**
 * @param aError - Error value resulting from synchronised testcase for logging
 * Sets the result obtained from synchronised testcase for logging
 */
void CSyncControl::SetResultL( TVerdict aError )
	{
	// Retrieve the status and result shared data pointers
	CTestSharedData* tefResult = iSharedTEFResult->Ptr();
	CTestSharedData* tefStatus = iSharedTEFStatus->Ptr();

	if( tefResult != NULL && tefStatus != NULL )
		{
		// Update the shared sync test case result code
		iSharedTEFResult->EnterCriticalSection();
		HBufC* resBuffer = NULL;
		TRAPD( err, resBuffer = HBufC::NewL(sizeof(TInt64)) );
		if( err != KErrNone )
			{
			iSharedTEFResult->ExitCriticalSection();
			User::Leave( err );
			}
		TPtr resPtr( resBuffer->Des() );
		resPtr.Num( (TInt64)aError );
		tefResult->SetText( resPtr );
		delete resBuffer;
		
		iSharedTEFResult->ExitCriticalSection();
		
		// So now the test has complete and the result has been updated
		//  we need to update the status to reflect this
		iSharedTEFStatus->EnterCriticalSection();
		HBufC* statusBuffer = NULL;
		TRAP( err, statusBuffer = HBufC::NewL(sizeof(TInt64)) );
		if( err != KErrNone )
			{
			iSharedTEFStatus->ExitCriticalSection();
			User::Leave( err );
			}
		TPtr statusPtr( statusBuffer->Des() );
		statusPtr.Num( ETEFRetrieveResult );
		tefStatus->SetText( statusPtr );
		delete statusBuffer;
		
		iSharedTEFStatus->ExitCriticalSection();
		}
	else
		{
		User::Leave( KErrNotFound );
		}
	}

/**
 * Identifies state of synchronised testcases and continues with the test or completes the tests
 */
TBool CSyncControl::TestCaseContinueL()
	{
	TBool tefContinue = EFalse;
	
	// Retrieve the status and result shared data pointers
	CTestSharedData* tefStatus = iSharedTEFStatus->Ptr();
	CTestSharedData* tefResult = iSharedTEFResult->Ptr();

	if( tefStatus != NULL && tefResult != NULL )
		{
		iSharedTEFStatus->EnterCriticalSection();

		// Retrieve the current status
		HBufC* statusBuffer = NULL;
		TRAPD( err, statusBuffer = HBufC::NewL(tefStatus->TextLength()) );
		if( err != KErrNone )
			{
			iSharedTEFStatus->ExitCriticalSection();
			User::Leave( err );
			}

		TPtr statusPtr( statusBuffer->Des() );
		tefStatus->GetText( statusPtr );
		TLex lex(statusPtr);
		TInt64	status = ETEFSyncUnknown;
		User::LeaveIfError( lex.Val(status) );

		if( status == ETEFSyncContinue )
			{
			tefContinue = ETrue;
			// Update the status to running
			HBufC* buffer = NULL;
			TRAP( err, buffer = HBufC::NewL(sizeof(TInt64)) );
			if( err != KErrNone )
				{
				delete statusBuffer;
				iSharedTEFStatus->ExitCriticalSection();
				User::Leave( err );
				}

			TPtr ptr( buffer->Des() );
			ptr.Num( ETEFSyncRunning );
			tefStatus->SetText( ptr );
			delete buffer;
			}
		else if( status == ETEFSyncComplete )
			{
			// Previous test has completed and a new sync block has been hit
			// Update the status to waiting
			HBufC* buffer = NULL;
			TRAP( err, buffer = HBufC::NewL(sizeof(TInt64)) );
			if( err != KErrNone )
				{
				delete statusBuffer;
				iSharedTEFStatus->ExitCriticalSection();
				User::Leave( err );
				}

			TPtr ptr( buffer->Des() );
			ptr.Num( ETEFSyncWaiting );
			tefStatus->SetText( ptr );
			delete buffer;
			}
		delete statusBuffer;
		iSharedTEFStatus->ExitCriticalSection();
		}
	else
		{
		User::Leave( KErrNotFound );
		}
	return tefContinue;
	}

/**
 * Takes in a script line updates any relative file paths into corresponding absolute path based on the current script file path
 * @param aScriptLineString - Pointer descriptor containing a particular script line for updation
 */
void CScriptControl::MakeAbsoluteFilePathsL(TPtrC16& aScriptLineString)
	{
	TLex lex(aScriptLineString);
	iAlteredScriptLine.Zero(); // Initialise the altered script line to zero b4 we start processing
	TBuf<KMaxTestExecuteCommandLength> commandName; // To store the command name, basically first token of each script line
	TBuf<KMaxTestExecuteCommandLength> commandString; // To store individual tokens of script line for processing
	TBuf<KMaxTestExecuteCommandLength> prevCommandString;
	const TInt KTEFMinFileExtnSizeIni = 4;
	const TInt KTEFMinFileExtnSizeScript = 7;

	while(!lex.Eos()) // start a loop for each token until end of the script line
		{
		lex.NextToken();
		lex.SkipSpace();
		if (lex.MarkedToken().Length() > KMaxTestExecuteCommandLength)
			{
			User::Leave(KErrTooBig);
			}

		commandString.Copy(lex.MarkedToken());
		lex.Mark();

		if (commandName.CompareF(KTEFRunScriptCommand) == 0 &&
			commandString.CompareF(KTEFNull) != 0)
			{
			// if the command name is RUN_SCRIPT, then check the parameter is not not null and has valid .script extn
			// We append .script extn if the length of the token is less than 7 (.script -> Length)
			// Or when the token does not end with .script enxtn
			if(commandString.Length() < KTEFMinFileExtnSizeScript ||
				commandString.Right(KTEFMinFileExtnSizeScript).CompareF(KTEFScriptExtension) != 0)
				{
				commandString.Append(KTEFScriptExtension);
				}
			}

		// Expand recognised variables into their values
		// At the moment just ${SYSDRIVE} -> GetSystemDrive()
		ExpandVariables(commandString);

		// we consider eligible path if the token is an argument for RUN_SCRIPT
		// or any INI file or an argument for RUN_UTILS command
		// Check to see if the token contains a ":" as second character represeting a drive letter
		if (commandString.Length() >= KTEFMinFileExtnSizeIni &&
			commandString.Mid(1,1).CompareF(KTEFColon) != 0)
			{
			// if the ":" is not found, we process converting relative -> absolute path
			// Provided the token ends with .script or .ini extns
		    if ((commandString.Length() >= KTEFMinFileExtnSizeScript && 
				commandString.Right(KTEFMinFileExtnSizeScript).CompareF(KTEFScriptExtension) == 0) ||
				(commandString.Length() >= KTEFMinFileExtnSizeIni && 
				commandString.Right(KTEFMinFileExtnSizeIni).CompareF(KTEFIniExtension) == 0))
				{
				// token does not does not have a ':' but is a eligible path,
				// so, convert relative path to absolute path
				TRAPD(err, ConvertRelativeToAbsoluteL(commandString));
				if(err != KErrNone)
					{
					// Leave here since we have got an invalid path
					User::Leave(err);
					}				
				}
			else if(commandName.CompareF(KTEFRunUtilsCommand) == 0 &&
					(prevCommandString.CompareF(KTEFRunUtilsCopyFile) == 0 ||
					prevCommandString.CompareF(KTEFRunUtilsMkDir) == 0 ||
					prevCommandString.CompareF(KTEFRunUtilsDeleteFile) == 0 ||
					prevCommandString.CompareF(KTEFRunUtilsDelete) == 0 ||
					prevCommandString.CompareF(KTEFRunUtilsMakeReadWrite) == 0 ||
					prevCommandString.CompareF(KTEFDeleteDirectory) == 0))
				{
				// token does not does not have a ':' but is a eligible path,
				// so, convert relative path to absolute path
				TRAPD(err, ConvertRelativeToAbsoluteL(commandString));
				if(err != KErrNone)
					{
					// Leave here since we have got an invalid path
					User::Leave(err);
					}
				}
			}
		//start defect 120600 
		//remove the code which append an extra space.
		// if (iAlteredScriptLine.Length())
		//end defect 120600
		if (iAlteredScriptLine.Length() == 0)
			{
			// While completng the processing for the first token in the script line,
			// we record the token as command name, so that we can use it to identify
			// eligible paths in the following loop cycles for the script line
			commandName.Copy(commandString);
			}
		prevCommandString.Copy(commandString);
		// Construct the altered script line with individual verified tokens of script line
		if(iAlteredScriptLine.Length() + commandString.Length() <= iAlteredScriptLine.MaxLength() )
			{
			iAlteredScriptLine.Append(commandString);			
			}
		else// this scipte too long buffer not long enought
			{			
			User::Leave(KErrTooBig);
			}
		}
	// At the end of the while loop, we are ready with new processed script line
	// which we shall update it with the original reference taken in
	aScriptLineString.Set(iAlteredScriptLine);
	}

/**
 * Takes in lex token and updates any relative file paths into corresponding absolute path based on the current script file path
 * @param aCommandString - Pointer descriptor containing a particular token within a script line for updation
 */
void CScriptControl::ConvertRelativeToAbsoluteL(TDes& aCommandString)
	{
	TInt offset = 0;
	TInt posOfLastSlash=iScriptFile.LocateReverse('\\') ;
	TBuf<KMaxTestExecuteCommandLength> tempStore(iScriptFile.Mid(0,posOfLastSlash)); // Initial script file path

	if(aCommandString.FindC(KTEFOneUp) >= 0)
		{
		while(aCommandString.FindC(KTEFOneUp) >= 0)
			{
			offset = aCommandString.FindC(KTEFOneUp);
			posOfLastSlash=tempStore.LocateReverse('\\') ;
			if (posOfLastSlash <= 0)
				{
				User::Leave(KTEFErrInvalidRelPath);
				}
			tempStore.Copy(iScriptFile.Mid(0,posOfLastSlash));// script file path
			aCommandString.Copy(aCommandString.Mid(offset + 3)); // 3 for ..'\\'
			}

		tempStore.Append('\\');
		}
	else if (aCommandString.FindC(KTEFDotSlash) >= 0)
		{
		offset = aCommandString.FindC(KTEFDotSlash);
		aCommandString.Copy(aCommandString.Mid(offset + 1));
		}
	else if(aCommandString.Locate('\\') == 0)
		{
		tempStore.Copy(iScriptFile.Mid(0,2)); // Example: c:
		}
	else
		{
		tempStore.Append('\\');
		}

	tempStore.Append(aCommandString);
	aCommandString.Copy(tempStore);
	}

/**
 * Expand recognised variables in script file into their corresponding value
 * Currently supported variable is ${SYSDRIVE} -> GetSystemDrive() / Drive letter set from testexecute.ini
 * @param aCommandString - Descriptor containing a particular token within a script line for updation
 */
void CScriptControl::ExpandVariables(TDes& aCommandString)
	{
	TInt offset = 0;
	if (aCommandString.FindC(KTEFSysDrive) >= 0)
		{
		// If we find ${SYSDRIVE} in the token, replace it with the drive letter to be substitued
		offset = aCommandString.FindC(KTEFSysDrive);
		aCommandString.Replace(offset, 11, iTestSysDrive);
		}
	}
