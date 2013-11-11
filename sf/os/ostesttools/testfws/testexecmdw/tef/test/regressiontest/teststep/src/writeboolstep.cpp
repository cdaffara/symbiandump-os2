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
 @file WriteBoolStep.cpp
*/
#include "writeboolstep.h"
#include "te_regsteptestsuitedefs.h"

CWriteBoolStep::~CWriteBoolStep()
/**
 * Destructor
 */
	{
	}

CWriteBoolStep::CWriteBoolStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KWriteBoolStep);
	}

TVerdict CWriteBoolStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	// process some pre setting to this test step then set SetTestStepResult to EFail or Epass.
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CWriteBoolStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	INFO_PRINTF1(_L("This step tests WriteBoolToConfig function."));
	SetTestStepResult(EFail);
	
	TBool originalValue;
	TBool TheBool;
	TBool ret = EFalse;
	
	if(!GetBoolFromConfig(ConfigSection(),KTe_RegStepTestSuiteBool, originalValue))
		{
		// Leave if there's any error.
		User::Leave(KErrNotFound);
		}
	
	INFO_PRINTF2(_L("The ORIGINAL Bool is %d"), originalValue); // Block end
	TheBool = ETrue;
	if (WriteBoolToConfig(ConfigSection(),KTe_RegStepTestSuiteBool,TheBool))
		{
		if (GetBoolFromConfig(ConfigSection(),KTe_RegStepTestSuiteBool, TheBool) && TheBool)
			{
			INFO_PRINTF2(_L("The CHANGED Bool is %d"), TheBool);
			ret = ETrue;
			}
		}
	
	TheBool = EFalse;
	if (WriteBoolToConfig(ConfigSection(),KTe_RegStepTestSuiteBool,TheBool))
		{
		if (GetBoolFromConfig(ConfigSection(),KTe_RegStepTestSuiteBool, TheBool) && !TheBool)
			{
			INFO_PRINTF2(_L("The CHANGED Bool is %d"), TheBool);
			}
		}
	else
		{
		ret = EFalse;
		}
	
	if (!WriteBoolToConfig(ConfigSection(),KTe_RegStepTestSuiteBool,originalValue))
		{
		ret = EFalse;
		}
	
	if (ret)
		{
		SetTestStepResult(EPass);
		}

	return TestStepResult();
	}

TVerdict CWriteBoolStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}
