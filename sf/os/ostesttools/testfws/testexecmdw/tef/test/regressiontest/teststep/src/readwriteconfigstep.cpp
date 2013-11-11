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
 @file ReadConfigStep.cpp
*/
#include "readwriteconfigstep.h"
#include "te_regsteptestsuitedefs.h"

#define StringDefine(s) _LIT(K##s, #s)
StringDefine(IntValue);
StringDefine(Int64Value);
StringDefine(BoolValue);
StringDefine(HexValue);
StringDefine(StringValue);

CReadWriteConfigStep::~CReadWriteConfigStep()
/**
 * Destructor
 */
	{
	}

CReadWriteConfigStep::CReadWriteConfigStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KReadWriteConfigStep);
	}

TVerdict CReadWriteConfigStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	// process some pre setting to this test step then set SetTestStepResult to EFail or Epass.
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CReadWriteConfigStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	INFO_PRINTF1(_L("This test step tests the read/write config functionality."));
	TBool ret = EFalse;
	ret = TestInt();
	ret &= TestInt64();
	ret &= TestBool();
	ret &= TestHex();
	ret &= TestString();
	
	if (ret)
		{
		SetTestStepResult(EPass);
		}
	return TestStepResult();
	}



TVerdict CReadWriteConfigStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}

TBool CReadWriteConfigStep::TestInt()
	{
	INFO_PRINTF1(_L("Start to test read/write integer value of configuration file..."));
	
	TInt value = 0;
	TInt getValue = KMaxTInt;
	TBool ret = EFalse;
	if (GetIntFromConfig(ConfigSection(), KIntValue, getValue))
		{
		value = getValue;
		ret = (KMaxTInt != getValue);
		ret &= WriteIntToConfig(ConfigSection(), KIntValue, KMaxTInt);
		getValue = 0;
		ret &= GetIntFromConfig(ConfigSection(), KIntValue, getValue);
		ret &= (KMaxTInt == getValue);
		ret &= WriteIntToConfig(ConfigSection(), KIntValue, value);
		}
	else
		{
		ERR_PRINTF1(_L("Test ERROR! Cannot get value from configuration file!"));
		}
	
	INFO_PRINTF1(ret?_L("Test Passed!"):_L("Test Failed!"));
	return ret;
	}

TBool CReadWriteConfigStep::TestInt64()
	{
	INFO_PRINTF1(_L("Start to test read/write integer 64 bit value of configuration file..."));
	
	TInt64 value = 0;
	TInt64 getValue = KMaxTInt64;
	TBool ret = EFalse;
	
	if (GetIntFromConfig(ConfigSection(), KInt64Value, getValue))
		{
		value = getValue;
		ret = (KMaxTInt64 != getValue);
		ret &= WriteIntToConfig(ConfigSection(), KInt64Value, KMaxTInt64);
		getValue = 0;
		ret &= GetIntFromConfig(ConfigSection(), KInt64Value, getValue);
		ret &= (KMaxTInt64 == getValue);
		ret &= WriteIntToConfig(ConfigSection(), KInt64Value, value);
		}
	else
		{
		ERR_PRINTF1(_L("Test ERROR! Cannot get value from configuration file!"));
		}
	
	INFO_PRINTF1(ret?_L("Test Passed!"):_L("Test Failed!"));
	return ret;
	}

TBool CReadWriteConfigStep::TestBool()
	{
	INFO_PRINTF1(_L("Start to test read/write boolean value of configuration file..."));
		
	TBool value = EFalse;
	TBool getValue = EFalse;
	TBool ret = EFalse;
	
	if (GetBoolFromConfig(ConfigSection(), KBoolValue, getValue))
		{
		value = getValue;
		ret &= WriteBoolToConfig(ConfigSection(), KBoolValue, ETrue);
		getValue = EFalse;
		ret &= GetBoolFromConfig(ConfigSection(), KBoolValue, getValue);
		ret &= getValue;
		ret &= WriteBoolToConfig(ConfigSection(), KBoolValue, EFalse);
		getValue = ETrue;
		ret &= GetBoolFromConfig(ConfigSection(), KBoolValue, getValue);
		ret &= !getValue;
		ret &= WriteBoolToConfig(ConfigSection(), KBoolValue, value);
		}
	else
		{
		ERR_PRINTF1(_L("Test ERROR! Cannot get value from configuration file!"));
		}
	
	INFO_PRINTF1(ret?_L("Test Passed!"):_L("Test Failed!"));
	return ret;
	}

TBool CReadWriteConfigStep::TestHex()
	{
	INFO_PRINTF1(_L("Start to test read/write hex value of configuration file..."));
		
	TInt value = 0;
	TInt getValue = KMaxTInt;
	TBool ret = EFalse;
	
	if (GetHexFromConfig(ConfigSection(), KHexValue, getValue))
		{
		value = getValue;
		ret = (KMaxTInt != getValue);
		ret &= WriteHexToConfig(ConfigSection(), KHexValue, KMaxTInt);
		getValue = 0;
		ret &= GetHexFromConfig(ConfigSection(), KHexValue, getValue);
		ret &= (KMaxTInt == getValue);
		ret &= WriteHexToConfig(ConfigSection(), KHexValue, value);
		}
	else
		{
		ERR_PRINTF1(_L("Test ERROR! Cannot get value from configuration file!"));
		}
	
	INFO_PRINTF1(ret?_L("Test Passed!"):_L("Test Failed!"));
	return ret;
	}

TBool CReadWriteConfigStep::TestString()
	{
	INFO_PRINTF1(_L("Start to test read/write string value of configuration file..."));
			
	_LIT(KChangedString, "Changed string!");
	RBuf originalString;
	TPtrC getValue;
	TBool ret = EFalse;
	
	if (GetStringFromConfig(ConfigSection(), KStringValue, getValue))
		{
		originalString.Create(getValue.Length());
		originalString.Copy(getValue);
		ret = (getValue != KChangedString());
		ret &= WriteStringToConfig(ConfigSection(), KStringValue, KChangedString());
		getValue.Set(KNullDesC);
		ret &= GetStringFromConfig(ConfigSection(), KStringValue, getValue);
		ret &= (getValue == KChangedString());
		ret &= WriteStringToConfig(ConfigSection(), KStringValue, originalString);
		}
	else
		{
		ERR_PRINTF1(_L("Test ERROR! Cannot get value from configuration file!"));
		}
	originalString.Close();
	
	INFO_PRINTF1(ret?_L("Test Passed!"):_L("Test Failed!"));
	return ret;
	}
