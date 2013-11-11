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
#include <test/testexecutelog.h>

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
	// Use logging macro
	// Could use Flogger().Write.. directly
	INFO_PRINTF1(_L("In Test Step"));
	TInt theInt;
	TBool theBool;
	TBuf<40> buf;
	TPtrC theString;
	GetIntFromConfig(ConfigSection(),_L("TheInt"),theInt);
	GetBoolFromConfig(ConfigSection(),_L("TheBool"),theBool);
	GetStringFromConfig(ConfigSection(),_L("TheString"),theString);
	buf.Copy(theString);
	INFO_PRINTF4(_L("TheInt = %d The Bool = %d TheString = %S"),theInt,theBool,&buf);
	SetTestStepResult(EPass);
	// Demonstrate panic
	if(theString == _L("PanicMe"))
		{
		const TInt KSampleServerPanic = 1;
		User::Panic(_L("SampleServer"),KSampleServerPanic);
		}
	// Demonstrate fail
	if(theString != _L("TheString"))
		{
		SetTestStepResult(EFail);
		}
	User::After(5000000);
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

CSampleStep2::CSampleStep2(CTEFUnitServer& aParent) : iParent(aParent)
/**
 * Constructor
 */
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KSampleStep2);
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
	User::LeaveIfError(iParent.Fs().Connect());
	SetTestStepResult(EPass);
	User::After(5000000);
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

CSampleStep3::CSampleStep3(CTEFUnitServer& aParent) : iParent(aParent)
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
	iParent.Fs().Close();
	SetTestStepResult(EPass);
	User::After(5000000);
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

