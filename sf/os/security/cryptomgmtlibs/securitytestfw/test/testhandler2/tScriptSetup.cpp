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


#include "tScriptSetup.h"
#include "tScriptTests.h"

#include "t_testactionspec.h"
#include "t_input.h"
#include "t_tefinput.h"
#include "t_certstoreactionmemfail.h"
#include "tcancel.h"
#include "t_sleep.h"
#include "t_message.h"
#include "t_output.h"
#include "tTestSpec.h"

#include <s32file.h>

#include "t_logger.h"

_LIT8(KStartMemFail, "startmemfailure");
_LIT8(KStopMemFail, "stopmemfailure");
_LIT8(KCancelStart, "startcancellation");
_LIT8(KCancelStop, "stopcancellation");
_LIT8(KMessage, "message");
_LIT8(KSleep, "sleep");

#include "t_policy.h"
_LIT8(KPolicyTest, "policytest");

EXPORT_C CScriptSetup* CScriptSetup::NewLC(CConsoleBase* aConsole)
	{
	CScriptSetup* self = new (ELeave) CScriptSetup(aConsole);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CScriptSetup::CScriptSetup(CConsoleBase* aConsole) : CTestSetup(aConsole)
	{
	}

EXPORT_C CScriptSetup::~CScriptSetup()
	{
	delete iTestInput;
	delete iScriptPath;
	}

EXPORT_C void CScriptSetup::SetupTestsL(RFs& aFs,
										CTestSpec& aTestSpec, 
										TScriptTests theTestTypes[], 
										const CTestHandlerSettings& aCommandLineSettings)
	{
	TInt pos = 0;
	TInt err = KErrNone;
	TBuf8<256> testCaseID;
	TBuf8<256> prevtestCaseID;
	TBuf8<512> scriptResult;
	TBuf8<512> actionType;
	TBool runtest = EFalse;
	TBool iniFile = EFalse;
	TBool startTest = EFalse;
	TBool endTest = EFalse;
	
	for (TInt i = 1 ; ; ++i)
		{
		TPtrC8 runTestStep;
		TPtrC8 testDescription;
		TPtrC8 actionData;
		
		if (iTefScript)
			{
			
			if(endTest)
				{
				startTest = EFalse;
				runtest = EFalse;
				testCaseID.Copy(Tefinput::ParseTestCaseID(*iTestInput, KStartTestStep, prevtestCaseID, pos, err, runtest, startTest));
				prevtestCaseID.Copy(testCaseID);
				}
			if(runtest)
				{
				if(endTest && startTest)
					{
					startTest = ETrue;
					}
				else
					{
					startTest = EFalse;
					}
				endTest = EFalse;
				testCaseID.Copy(prevtestCaseID);
				}
			else
				{
				testCaseID.Copy(Tefinput::ParseTestCaseID(*iTestInput, KStartTestStep, prevtestCaseID, pos, err, runtest, startTest));
				prevtestCaseID.Copy(testCaseID);
				}
			runTestStep.Set(Tefinput::ParseRunTestStep(*iTestInput, KRunTestStep, pos, err));
			endTest = Tefinput::EndTestCase(*iTestInput, runTestStep, KEndTestStep, pos);
			}
		else
			{
			actionData.Set(Input::ParseElement(*iTestInput, KActionStart, KActionEnd, pos, err));
			}
		if (err != KErrNone)
		  	{
			break;
			} 
			 
		TInt relativePos = 0;
		TInt scriptGroupings= DEFAULTGROUPING;
		TTestActionSpec actionSpec;
		//Parse the file to create action name, type , body and result
		if (iTefScript)
			{
			User::LeaveIfError(actionSpec.TEFInit(aFs, runTestStep, testCaseID, prevtestCaseID, iTefScript, runtest, iniFile, scriptResult, actionType, *iScriptPath, *iConsole, *iLogFile));
			}
		else
			{
			User::LeaveIfError(actionSpec.Init(actionData, relativePos, *iConsole, *iLogFile, scriptGroupings));
			}
		
		//Start off checking Exhaustive and Smoke flags, then calls groupings
		if (!CheckAllFlags(aCommandLineSettings, scriptGroupings))
			//The current test should not be run
			continue;
	
		
		CTestAction* newAction = 0;
		TRAP(err, newAction = CreateActionL(aFs, actionSpec, theTestTypes));
		
	
		

		if (err != KErrNone)
			{
			iLogFile->write(_L("CScriptSetup::CreateActionL failed: "));
			iLogFile->writeError(err);		
			iLogFile->writeNewLine();
			
			iLogFile->write(_L("Action name: "));
			iLogFile->writeString(actionSpec.iActionName);				
			iLogFile->writeNewLine();

			iLogFile->write(_L("Action type: "));
			iLogFile->writeString(actionSpec.iActionType);				
			iLogFile->writeNewLine();
			
			iLogFile->write(_L("1 tests failed out of 1\r\n"));
			
			User::Leave(err);
			}
		
		if(newAction->ScriptError() != CTestAction::ENone)
			{
			TBuf<KMaxErrorSize> scriptError;
			newAction->ScriptError(scriptError);
			
			iLogFile->write(_L("Script error \"%S\" in test %s\r\n"), &scriptError, i);
				
			delete newAction;
			User::Leave(KErrArgument);
			}
		
		CleanupStack::PushL(newAction);
		User::LeaveIfError(aTestSpec.AddNextTest(newAction));
		CleanupStack::Pop(newAction);
	
		if(iniFile)
			{
			CleanupStack::PopAndDestroy(1);
			}
		}
	}

EXPORT_C TBool CScriptSetup::InitialiseL(RFs &aFs, const TDesC& aDefaultScript, const TDesC& aDefaultLog, TBool aUseCommandLine)
	{
	// gets the script file argument
	iScriptPath = NULL;
	iTefScript = EFalse;

	if (aUseCommandLine)
		{
		iScriptPath = GetArgument();
		if(iScriptPath->Length()==0)
			{
			delete iScriptPath;  
			iScriptPath = NULL;
			}
		else
			{
			TPtr scriptFileNameDes = iScriptPath->Des();
			if (scriptFileNameDes.Right(8) == _L(".script1") || scriptFileNameDes.Right(7) == _L(".script"))
				{
				iTefScript = ETrue;
				}
			}
		}

	if(iScriptPath == NULL)
		{
		if(aDefaultScript.Length() == 0)
			{
			PRINTANDLOG(_L("No script file specified on command line and no default given"));
			return(EFalse);
			}
		else
			{
			iScriptPath = aDefaultScript.AllocLC();
			}
		};

	PRINTANDLOG1(_L("Script file: %S"), iScriptPath);

	// open the script file
	RFile scriptFile;
	TInt err = scriptFile.Open(aFs, *iScriptPath, EFileStream | EFileRead | EFileShareReadersOnly);
	if (err != KErrNone)
		{
		PRINTANDLOG1(_L("Error opening script file: %d"), err);
		delete iScriptPath;
		iScriptPath = NULL;
		return(EFalse);
		}
	CleanupClosePushL(scriptFile);
	
	TRAP(err, OpenLogFileL(aFs, 1, aDefaultLog, aUseCommandLine));	
	if (err != KErrNone)
		{
		PRINTANDLOG1(_L("Error opening log file: %d"), err);	
		delete iScriptPath;
		iScriptPath = NULL;
		CleanupStack::PopAndDestroy(1);// scripFile
		return(EFalse);
		}
		
	// gets size of script file
	TInt size;
	scriptFile.Size(size);
	CleanupStack::PopAndDestroy(); // scriptFile
		
	// reads script into iTestInput
	iTestInput = HBufC8::NewL(size);
	TPtr8 pInput(iTestInput->Des()); 
	pInput.SetLength(size); 

	RFileReadStream stream;
	User::LeaveIfError(stream.Open(aFs, *iScriptPath, EFileStream | EFileRead | EFileShareReadersOnly));
	CleanupClosePushL(stream);
	stream.ReadL(pInput, size);
	CleanupStack::PopAndDestroy(1); // stream
	return(ETrue);
	}

EXPORT_C CTestAction* CScriptSetup::CreateActionL(RFs& aFs,
												  const TTestActionSpec& aTestActionSpec,
												  const TScriptTests theTestTypes[]) const
	{
	// Instantiate built in test actions
	if (aTestActionSpec.iActionType == KStartMemFail)
		{
		return CMemFailStart::NewL(*iTestConsole, *iLogFile, aTestActionSpec);
		}
	else if (aTestActionSpec.iActionType == KStopMemFail)
		{
		return CMemFailStop::NewL(*iTestConsole, *iLogFile, aTestActionSpec);
		}
	else if (aTestActionSpec.iActionType == KCancelStart)
		{
		return CCancelStart::NewL(*iTestConsole, *iLogFile, aTestActionSpec);
		}
	else if (aTestActionSpec.iActionType == KCancelStop)
		{
		return CCancelStop::NewL(*iTestConsole, *iLogFile, aTestActionSpec);
		}
	else if (aTestActionSpec.iActionType == KMessage)
		{
		return CTestActionMessage::NewL(*iTestConsole, *iLogFile, aTestActionSpec);
		}
	else if (aTestActionSpec.iActionType == KSleep)
		{
		return CSleep::NewL(*iTestConsole, *iLogFile, aTestActionSpec);
		}
	else if (aTestActionSpec.iActionType == KPolicyTest)
		{
		return CPolicyTest::NewL(*iTestConsole, *iLogFile, aTestActionSpec);
		}

	// Look through the array of tests in order to find the correct class to return
	TInt loop=0;
	while (theTestTypes[loop].action!=NULL)
		{
		if (aTestActionSpec.iActionType == theTestTypes[loop].name)
			{
			return theTestTypes[loop].action(aFs, *iTestConsole, *iLogFile, aTestActionSpec);
			}
		loop++;
		}

	User::Leave(KErrNotSupported);
	return NULL;
	}
