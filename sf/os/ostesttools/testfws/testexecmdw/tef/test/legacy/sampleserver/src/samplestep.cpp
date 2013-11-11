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
 @file SampleStep.cpp
*/
#include "samplestep.h"

CSampleStep1::~CSampleStep1()
/**
 * Destructor
 */
	{
	}

CSampleStep1::CSampleStep1()
/**
 * Constructor
 */
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KSampleStep1);
	}

TVerdict CSampleStep1::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CSampleStep1::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Demonstrates reading configuration parameters fom an ini file section
 */
	{
	INFO_PRINTF1(_L("In Test Step"));

  	TDriveName scriptDrive(ScriptDriveName());
  	INFO_PRINTF2(_L("The Drive where script file is executed from is %S"), &scriptDrive);

	TInt theInt;
	TPtrC val;
	_LIT(tem,"NewData");
	val.Set(tem);
	TBool theBool;
	TBuf<40> buf;
	TPtrC theString;
	TInt changeInt(83);
	TInt changeHex(183);
	TBool changeBool(ETrue);
	_LIT(sectName,"Section3");

	GetHexFromConfig(sectName,_L("TheHex"),changeInt);
	GetIntFromConfig(ConfigSection(),_L("TheInt"),theInt);
	GetBoolFromConfig(ConfigSection(),_L("TheBool"),theBool);
	GetStringFromConfig(ConfigSection(),_L("TheString"),theString);
	buf.Copy(theString);

	WriteBoolToConfig(sectName,_L("TheBool"),changeBool);
	WriteIntToConfig(ConfigSection(),_L("TheInt"),changeInt);
	WriteHexToConfig(sectName,_L("TheHex"),changeHex);

	// Demonstrate panic
	if(buf == _L("PanicMe"))
		{
		const TInt KSampleServerPanic = 1;
		User::Panic(_L("SampleServer"),KSampleServerPanic);
		}
		
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CSampleStep1::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Test Step Postamble"));
	return TestStepResult();
	}

///////

CSampleStep2::~CSampleStep2()
/**
 * Destructor
 */
	{
	}

CSampleStep2::CSampleStep2(CSampleServer& aParent) : iParent(aParent)
/**
 * Constructor
 */
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KSampleStep2);
	SetTestStepResult(EPass);
	}

TVerdict CSampleStep2::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Test Step Preamble"));
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CSampleStep2::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Demonstrates reading configuration parameters fom an ini file section
 */
	{
	INFO_PRINTF1(_L("doTestStepL"));
  	TDriveName scriptDrive(ScriptDriveName());
  	INFO_PRINTF2(_L("The Drive where script file is executed from is %S"), &scriptDrive);

	User::LeaveIfError(iParent.Fs().Connect());
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CSampleStep2::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Test Step Postamble"));
	SetTestStepResult(EPass);
	return TestStepResult();
	}
///////

CSampleStep3::~CSampleStep3()
/**
 * Destructor
 */
	{
	}

CSampleStep3::CSampleStep3(CSampleServer& aParent) : iParent(aParent)
/**
 * Constructor
 */
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KSampleStep3);
	}

TVerdict CSampleStep3::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Test Step Preamble"));
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CSampleStep3::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Demonstrates reading configuration parameters fom an ini file section
 */
	{
	INFO_PRINTF1(_L("doTestStepL"));
  	TDriveName scriptDrive(ScriptDriveName());
  	INFO_PRINTF2(_L("The Drive where script file is executed from is %S"), &scriptDrive);

	iParent.Fs().Close();
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CSampleStep3::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Test Step Postamble"));
	INFO_PRINTF1( GetServerName() );
	SetTestStepResult(EPass);
	return TestStepResult();
	}

   ///////

CSampleStepSelective::~CSampleStepSelective()
/**
 * Destructor
 */
	{
	}

CSampleStepSelective::CSampleStepSelective()
/**
 * Constructor
 */
	{
	}

TVerdict CSampleStepSelective::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(TestStepName());
	INFO_PRINTF1(_L("Test Step Preamble"));
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CSampleStepSelective::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 */
	{
	INFO_PRINTF1(TestStepName());
	INFO_PRINTF1(_L("doTestStepL"));
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CSampleStepSelective::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(TestStepName());
	INFO_PRINTF1(_L("Test Step Postamble"));
	INFO_PRINTF1( GetServerName() );
	SetTestStepResult(EPass);
	return TestStepResult();
	}


CSampleStep4::~CSampleStep4()
/**
 * Destructor
 */
	{
	}

CSampleStep4::CSampleStep4()
/**
 * Constructor
 */
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KSampleStep4);
	}

TVerdict CSampleStep4::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CSampleStep4::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Demonstrates reading configuration parameters fom an ini file section
 */
	{
	INFO_PRINTF1(_L("In Test Step of SampleStep4"));
  	TDriveName scriptDrive(ScriptDriveName());
  	INFO_PRINTF2(_L("The Drive where script file is executed from is %S"), &scriptDrive);	

	SetTestStepError(-43);
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CSampleStep4::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Test Step Postamble"));
	return TestStepResult();
	}

CSampleStep5::CSampleStep5()
	{
		SetTestStepName(KSampleStep5);
	};

CSampleStep6::CSampleStep6()
	{
		SetTestStepName(KSampleStep6);
	};

CSampleStep7::CSampleStep7()
	{
		SetTestStepName(KSampleStep7);
	};

CSampleStep8::CSampleStep8()
	{
		SetTestStepName(KSampleStep8);
	};
CSampleStep9::CSampleStep9()
	{
		SetTestStepName(KSampleStep9);
	};

CSampleStep10::CSampleStep10()
	{
		SetTestStepName(KSampleStep10);
	};

CSampleStep11::CSampleStep11()
	{
		SetTestStepName(KSampleStep11);
	};

CSampleStep12::CSampleStep12()
	{
		SetTestStepName(KSampleStep12);
	};

CSampleStep13::CSampleStep13()
	{
		SetTestStepName(KSampleStep13);
	};
