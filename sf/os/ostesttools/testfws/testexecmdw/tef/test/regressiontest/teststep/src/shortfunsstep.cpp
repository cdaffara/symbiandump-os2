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
* Example CTestStep derived implementation
*
*/



/**
 @file WriteStringStep.cpp
*/
#include "shortfunsstep.h"
#include "te_regsteptestsuitedefs.h"

_LIT(KPassed, "passed");
_LIT(KFailed, "failed");
#define PASSORFAIL (funcTestPassed?&KPassed():&KFailed())

CShortFunsTestStep::~CShortFunsTestStep()
/**
 * Destructor
 */
	{
	}

CShortFunsTestStep::CShortFunsTestStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KShortFunsTestStep);
	}

TVerdict CShortFunsTestStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	// There is no initiation work so set this result to pass.
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CShortFunsTestStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	INFO_PRINTF1(_L("Start to test some small functions."));
	SetTestStepResult(EFail);
	
	TBool funcTestPassed = EFalse;
	//TBool totalTestPassed = ETrue;
	TInt passCount = 0;
	TInt expectedPassCount = 9;

	_LIT(KTestCaseName, "TestCaseName");
	_LIT(KOOMTest, "OOMTest");
	_LIT(KTestStepError, "TestStepError");
	_LIT(KLoopIndex, "LoopIndex");
	_LIT(KScriptDriveName, "ScriptDriveName");
	_LIT(KSystemDrive, "SystemDrive");
	_LIT(KSetupParaValue, "SetupParaValue");

	TPtrC serverName = GetServerName();
	if (serverName == KServerName())
		{
		funcTestPassed = EPass;
		++passCount;
		}
	INFO_PRINTF4(_L("GetServerName test %S! ServerName=%S, ExpectedServerName=%S"), PASSORFAIL, &serverName, &KServerName());

	funcTestPassed = EFalse;
	TBuf<KMaxTestExecuteNameLength> testCaseName;
	TBool ret = TestCaseName(testCaseName);
	TPtrC expectedTestCaseName;
	GetStringFromConfig(ConfigSection(), KTestCaseName, expectedTestCaseName);
	if (testCaseName == expectedTestCaseName)
		{
		funcTestPassed = ETrue;
		++passCount;
		}
	INFO_PRINTF4(_L("TestCaseName test %S! TestCaseName=%S, ExpectedTestCaseName=%S"), PASSORFAIL, &testCaseName, &expectedTestCaseName);
	
	funcTestPassed = ETrue;
	TPtrC testStepName = TestStepName();
	if (testStepName == KShortFunsTestStep)
		{
		funcTestPassed = ETrue;
		++passCount;
		}
	INFO_PRINTF4(_L("TestStepName test %S! TestStepName=%S, ExpectedTestStepName=%S"), PASSORFAIL, &testStepName, &KShortFunsTestStep());
	
	funcTestPassed = ETrue;
	TBool OOMTest;
	GetBoolFromConfig(ConfigSection(), KOOMTest, OOMTest);
	if (ShouldRunOOMTest() == OOMTest)
		{
		funcTestPassed = ETrue;
		++passCount;
		}
	INFO_PRINTF4(_L("ShouldRunOOMTest test %S! OOMValue=%d, ExpectedOOMValue=%d"), PASSORFAIL, ShouldRunOOMTest(), OOMTest);
	
	funcTestPassed = ETrue;
	TInt expectedTestStepError = 0;
	GetIntFromConfig(ConfigSection(), KTestStepError, expectedTestStepError);
	SetTestStepError(-46);
	TInt testStepError = TestStepError();
	SetTestStepError(0);
	if (testStepError == expectedTestStepError)
		{
		funcTestPassed = ETrue;
		++passCount;
		}
	INFO_PRINTF4(_L("TestStepError test %S! TestStepError=%d, ExpectedTestStepError=%d"), PASSORFAIL, testStepError, expectedTestStepError);
	
	funcTestPassed = ETrue;
	TInt expectedLoopIndex = 0;
	GetIntFromConfig(ConfigSection(), KLoopIndex, expectedLoopIndex);
	TInt loopIndex = GetLoopIndex();
	if (loopIndex == expectedLoopIndex)
		{
		funcTestPassed = ETrue;
		++passCount;
		}
	INFO_PRINTF4(_L("GetLoopIndex test %S! LoopIndex=%d, ExpectedLoopIndex=%d"), PASSORFAIL, loopIndex, expectedLoopIndex);
	
	funcTestPassed = ETrue;
	TPtrC expectedScriptDriveName;
	GetStringFromConfig(ConfigSection(), KScriptDriveName, expectedScriptDriveName);
	TPtrC scriptDriveName = ScriptDriveName();
	if (0 == scriptDriveName.CompareF(expectedScriptDriveName))
		{
		funcTestPassed = ETrue;
		++passCount;
		}
	INFO_PRINTF4(_L("ScriptDriveName test %S! ScriptDriveName=%S, ExpectedScriptDriveName=%S"), PASSORFAIL, &scriptDriveName, &expectedScriptDriveName);
	
	funcTestPassed = ETrue;
	TPtrC expectedSystemDriveName;
	GetStringFromConfig(ConfigSection(), KSystemDrive, expectedSystemDriveName);
	TPtrC systemDriveName = GetSystemDrive();
	if (0 == scriptDriveName.CompareF(expectedSystemDriveName))
		{
		funcTestPassed = ETrue;
		++passCount;
		}
	INFO_PRINTF4(_L("GetSystemDrive test %S! SystemDrive=%S, ExpectedSystemDrive=%S"), PASSORFAIL, &systemDriveName, &expectedSystemDriveName);
	
	funcTestPassed = ETrue;
	TInt expectedSetupValue = 0;
	GetIntFromConfig(ConfigSection(), KSetupParaValue, expectedSetupValue);
	TInt setupValue = GetSetupState();
	if (setupValue == expectedSetupValue)
		{
		funcTestPassed = ETrue;
		++passCount;
		}
	INFO_PRINTF4(_L("GetSetupState test %S! SetupValue=%d, ExpectedSetupValue=%d"), PASSORFAIL, setupValue, expectedSetupValue);

	if (passCount != expectedPassCount)
		{
		ERR_PRINTF1(_L("Short functions test failed!"));
		}
	else
		{
		INFO_PRINTF1(_L("Short functions test passed!"));
		SetTestStepResult(EPass);
		}

	return TestStepResult();
	}

TVerdict CShortFunsTestStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}
