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
#include "readwriteconfignegstep.h"
#include "te_regsteptestsuitedefs.h"

#define StringDefine(s) _LIT(K##s, #s)
StringDefine(IntValue);
StringDefine(BoolValue);
StringDefine(HexValue);
StringDefine(StringValue);

CReadWriteConfigNegStep::~CReadWriteConfigNegStep()
/**
 * Destructor
 */
	{
	}

CReadWriteConfigNegStep::CReadWriteConfigNegStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KReadWriteConfigNegStep);
	}

TVerdict CReadWriteConfigNegStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	// There is no initiation work so set this result to pass.
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CReadWriteConfigNegStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	INFO_PRINTF1(_L("Negative test of read/write config file."));
	SetTestStepResult(EFail);
	
	TBool ret = EFalse;
	ret = TestInt();
	ret &= TestBool();
	ret &= TestHex();
	ret &= TestString();
	
	if (ret)
		{
		SetTestStepResult(EPass);
		}
	return TestStepResult();
	}

TVerdict CReadWriteConfigNegStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}

TBool CReadWriteConfigNegStep::TestInt()
	{
	INFO_PRINTF1(_L("---Start negative test of read/write integer from/to configuration file..."));
	
	TInt value = 10;
	TInt getValue = 0;
	TBool ret = EFalse;
	
	if (!GetIntFromConfig(ConfigSection(), KIntValue, getValue))
		{
		if (0 == ConfigSection().Length())
			{
			ret = !WriteIntToConfig(ConfigSection(), KIntValue, value);
			}
		else
			{
			ret = WriteIntToConfig(ConfigSection(), KIntValue, value);
			}
		}
	
	INFO_PRINTF1(ret?_L("Test Passed!"):_L("Test Failed!"));
	return ret;
	}

TBool CReadWriteConfigNegStep::TestBool()
	{
	INFO_PRINTF1(_L("---Start negative test of read/write bool from/to configuration file..."));
		
	TBool value = EFalse;
	TBool getValue = EFalse;
	TBool ret = EFalse;
	
	if (!GetBoolFromConfig(ConfigSection(), KBoolValue, getValue)
		&& !WriteBoolToConfig(ConfigSection(), KBoolValue, 10))
		{
		if (0 == ConfigSection().Length())
			{
			ret = !WriteBoolToConfig(ConfigSection(), KBoolValue, value);
			}
		else
			{
			ret = WriteBoolToConfig(ConfigSection(), KBoolValue, value);
			}
		}
	
	INFO_PRINTF1(ret?_L("Test Passed!"):_L("Test Failed!"));;
	return ret;
	}

TBool CReadWriteConfigNegStep::TestHex()
	{
	INFO_PRINTF1(_L("---Start negative test of read/write hex from/to configuration file..."));
			
	TInt value = 0x00AB;
	TInt getValue = 0;
	TBool ret = EFalse;
	if (!GetHexFromConfig(ConfigSection(), KHexValue, getValue))
		{
		ret = (0==ConfigSection().Length()?!WriteHexToConfig(ConfigSection(), KHexValue, value):WriteHexToConfig(ConfigSection(), KHexValue, value));
		}
	
	INFO_PRINTF1(ret?_L("Test Passed!"):_L("Test Failed!"));;
	return ret;
	}

TBool CReadWriteConfigNegStep::TestString()
	{
	INFO_PRINTF1(_L("---Start negative test of read/write string from/to configuration file..."));
	TBool ret = EFalse;
		
	_LIT(KChangedString, "Changed string!");
	TPtrC getValue;
	
	if (!GetStringFromConfig(ConfigSection(), KStringValue, getValue))
		{
		ret = (0==ConfigSection().Length()?!WriteStringToConfig(ConfigSection(), KStringValue, KChangedString()):WriteStringToConfig(ConfigSection(), KStringValue, KChangedString()));
		}
	
	INFO_PRINTF1(ret?_L("Test Passed!"):_L("Test Failed!"));;
	return ret;
	}
