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
 @file WriteIntStep.cpp
*/
#include "writeintstep.h"
#include "te_regsteptestsuitedefs.h"

CWriteIntStep::~CWriteIntStep()
/**
 * Destructor
 */
	{
	}

CWriteIntStep::CWriteIntStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KWriteIntStep);
	}

TVerdict CWriteIntStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	// process some pre setting to this test step then set SetTestStepResult to EFail or Epass.
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CWriteIntStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	INFO_PRINTF1(_L("This step tests WriteIntToConfig function."));
	SetTestStepResult(EFail);
		
	TInt TheInt;
	TInt originalValue;
	TBool ret = EFalse;
	
	if(!GetIntFromConfig(ConfigSection(),KTe_RegStepTestSuiteInt, originalValue))
		{
		// Leave if there's any error.
		User::Leave(KErrNotFound);
		}
	
	INFO_PRINTF2(_L("The ORIGINAL int-value is %d"),originalValue); // Block end
	
	TheInt=9999;
	if (WriteIntToConfig(ConfigSection(),KTe_RegStepTestSuiteInt,TheInt))
		{
		if (GetIntFromConfig(ConfigSection(),KTe_RegStepTestSuiteInt, TheInt) && TheInt==9999)
			{
			INFO_PRINTF2(_L("The CHANGED int-value is %d"),TheInt);
			ret = ETrue;
			}
		}
	
	TheInt=7777;
	if (WriteIntToConfig(ConfigSection(),KTe_RegStepTestSuiteInt,TheInt))
		{
		if (GetIntFromConfig(ConfigSection(),KTe_RegStepTestSuiteInt, TheInt) && TheInt==7777)
			{
			INFO_PRINTF2(_L("The CHANGED int-value is %d"),TheInt);
			}
		}
	else
		{
		ret = EFalse;
		}
	
	if (ret)
		{
		SetTestStepResult(EPass);
		}
	
	if (!WriteIntToConfig(ConfigSection(), KTe_RegStepTestSuiteInt, originalValue))
		{
		SetTestStepResult(EFail);
		}
	
	return TestStepResult();
	}

TVerdict CWriteIntStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}
