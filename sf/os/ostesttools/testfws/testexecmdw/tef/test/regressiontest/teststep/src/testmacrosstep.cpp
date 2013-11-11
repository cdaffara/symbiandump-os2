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
#include "testmacrosstep.h"
#include "te_regsteptestsuitedefs.h"

_LIT(KMacro, "Macro");
_LIT(KInitState, "InitState");
_LIT(KParameter, "Parameter");
_LIT(KParameter2, "Parameter2");
_LIT(KParameter3, "Parameter3");
_LIT(KExpectedResult, "ExpectedResult");

_LIT(KTest, "TEST&TESTE");
_LIT(KTest1, "TEST1&TESTE1");
//_LIT(KTestE, "TESTE");
//_LIT(KTestE1, "TESTE1");
_LIT(KTestCheckL, "TEST_CHECKL");

_LIT(KPositive, "positive");
_LIT(KNegative, "negative");

#define POS_NEG_SEL (parameter?&KPositive():&KNegative())


CTestMacrosTestStep::~CTestMacrosTestStep()
/**
 * Destructor
 */
	{
	}

CTestMacrosTestStep::CTestMacrosTestStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KMacrosTestStep);
	}

TVerdict CTestMacrosTestStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	// There is no initiation work so set this result to pass.
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CTestMacrosTestStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	TPtrC macro;
	GetStringFromConfig(ConfigSection(), KMacro, macro);
	if (macro == KTest)
		{
		TBool parameter;
		TInt parameter2;
		TInt expectedResult;
		GetBoolFromConfig(ConfigSection(), KParameter, parameter);
		GetIntFromConfig(ConfigSection(), KParameter2, parameter2);
		GetIntFromConfig(ConfigSection(), KExpectedResult, expectedResult);
		_LIT(KStartInfo, "Start to run %S test of macros TEST(E) & TEST(E)L");
		INFO_PRINTF2(KStartInfo, POS_NEG_SEL);
		TEST(parameter);
		TESTE(parameter, parameter2);
		if (expectedResult != TestStepResult())
			{
			ERR_PRINTF2(_L("The %S test of macro TEST(E) test is failed!"), POS_NEG_SEL);
			SetTestStepResult(EFail);
			}
		else
			{
			SetTestStepResult(EPass);
			INFO_PRINTF3(_L("The %S test of macro TEST(E) is passed. Start to run %S test of macro TEST(E)L"), POS_NEG_SEL, POS_NEG_SEL);
			
			TRAPD(err, TESTL(parameter));
			TRAPD(err1, TESTEL(parameter, parameter2));
			if ((expectedResult==EPass && KErrNone!=err) || (expectedResult==EFail && TEST_ERROR_CODE!=err && parameter2!=err1))
				{
				ERR_PRINTF2(_L("The %S test of macro TEST(E)L test is failed!"), POS_NEG_SEL);
				SetTestStepResult(EFail);
				}
			else
				{
				INFO_PRINTF2(_L("The %S test of macro TEST(E)L is passed."), POS_NEG_SEL);
				SetTestStepResult(EPass);
				}
			}
		}
	else if (macro == KTest1)
		{
		TBool parameter;
		TBool parameter2;
		TInt parameter3;
		TInt initState;
		TInt expectedResult;
		GetBoolFromConfig(ConfigSection(), KParameter, parameter);
		GetBoolFromConfig(ConfigSection(), KParameter2, parameter2);
		GetIntFromConfig(ConfigSection(), KParameter3, parameter3);
		GetIntFromConfig(ConfigSection(), KInitState, initState);
		GetIntFromConfig(ConfigSection(), KExpectedResult, expectedResult);
		SetTestStepResult((TVerdict)initState);
		_LIT(KStartInfo, "Start to run %S test of macros TEST(E)1 & TEST(E)1L");
		INFO_PRINTF2(KStartInfo, POS_NEG_SEL);
		TEST1(parameter, parameter2);
		TESTE1(parameter, parameter3, parameter2);
		if (expectedResult != TestStepResult())
			{
			ERR_PRINTF1(_L("Macro TEST(E)1 test failed!"));
			SetTestStepResult(EFail);
			}
		else
			{
			SetTestStepResult(EPass);
			INFO_PRINTF3(_L("The %S test of macro TEST(E)1 has passed. Start to run %S test of macro TEST(E)1L"), POS_NEG_SEL, POS_NEG_SEL);
			
			TRAPD(err, TEST1L(parameter, parameter2));
			TRAP_IGNORE(TESTE1L(parameter, parameter3, parameter2));
			if ((expectedResult==EPass && KErrNone!=err) || ((expectedResult==EFail) && (parameter && ((TEST_ERROR_CODE==err)||(parameter3==err))) || (!parameter && TEST_ERROR_CODE!=err && parameter3!=err)))
				{
				ERR_PRINTF1(_L("Macro TEST(E)1L test failed!"));
				SetTestStepResult(EFail);
				}
			else
				{
				INFO_PRINTF2(_L("The %S test of macro TEST(E)1L has passed."), POS_NEG_SEL);
				SetTestStepResult(EPass);
				}
			}
		}

	else if (macro == KTestCheckL)
		{
		TInt parameter;
		TInt parameter1;
		TPtrC parameter2;
		TInt expectedResult;
		GetIntFromConfig(ConfigSection(), KParameter, parameter1);
		GetStringFromConfig(ConfigSection(), KParameter2, parameter2);
		GetIntFromConfig(ConfigSection(), KExpectedResult, expectedResult);
		
		parameter = (parameter1==expectedResult);
		INFO_PRINTF3(_L("Start %S test of macro %S"), POS_NEG_SEL, &KTestCheckL());
		
		TRAPD(err, TEST_CHECKL(parameter1, expectedResult, parameter2));
		_LIT(KPassInfo, "This test should be passed!");
		_LIT(KFailInfo, "This test should be failed!");
		if ((KErrNone==err && parameter2==KPassInfo()) || (parameter1==err && parameter2==KFailInfo()))
			{
			INFO_PRINTF2(_L("The macro %S test is passed!"), &KTestCheckL());
			SetTestStepResult(EPass);
			}
		else
			{
			ERR_PRINTF2(_L("The macro %S test is failed!"), &KTestCheckL());
			SetTestStepResult(EFail);
			}
		}
	else
		{
		WARN_PRINTF2(_L("Unrecognized Marco %S"), macro);
		}
	
	return TestStepResult();
	}



TVerdict CTestMacrosTestStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}
