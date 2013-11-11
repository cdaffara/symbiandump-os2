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
 @file WriteHexStep.cpp
*/
#include "writehexstep.h"
#include "te_regsteptestsuitedefs.h"

CWriteHexStep::~CWriteHexStep()
/**
 * Destructor
 */
	{
	}

CWriteHexStep::CWriteHexStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KWriteHexStep);
	}

TVerdict CWriteHexStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	// process some pre setting to this test step then set SetTestStepResult to EFail or Epass.
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CWriteHexStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	INFO_PRINTF1(_L("This step tests WriteHexToConfig function."));
	SetTestStepResult(EFail);
		
	TInt TheHex;
	TInt originalValue;
	
	if(!GetHexFromConfig(ConfigSection(),KTe_RegStepTestSuiteHex, originalValue))
		{
		// Leave if there's any error.
		User::Leave(KErrNotFound);
		}
	
	INFO_PRINTF2(_L("The ORIGINAL Hex-value is %d"),originalValue); // Block end
	TheHex=1234;
	if (WriteHexToConfig(ConfigSection(),KTe_RegStepTestSuiteHex,TheHex))
		{
		if (GetHexFromConfig(ConfigSection(),KTe_RegStepTestSuiteHex, TheHex) && TheHex==1234)
			{
			INFO_PRINTF2(_L("The CHANGED Hex-value is %d"),TheHex);
			SetTestStepResult(EPass);
			}
		}
	
	if (!WriteHexToConfig(ConfigSection(),KTe_RegStepTestSuiteHex,originalValue))
		{
		SetTestStepResult(EFail);
		}
	
	return TestStepResult();
	}



TVerdict CWriteHexStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}
