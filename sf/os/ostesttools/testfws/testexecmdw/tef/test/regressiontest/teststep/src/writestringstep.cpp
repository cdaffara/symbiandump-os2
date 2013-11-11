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
#include "writestringstep.h"
#include "te_regsteptestsuitedefs.h"

CWriteStringStep::~CWriteStringStep()
/**
 * Destructor
 */
	{
	}

CWriteStringStep::CWriteStringStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KWriteStringStep);
	}

TVerdict CWriteStringStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	// process some pre setting to this test step then set SetTestStepResult to EFail or Epass.
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CWriteStringStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	INFO_PRINTF1(_L("This step tests WriteStringToConfig function."));
	SetTestStepResult(EFail);
	
	TPtrC originalValue;
	TBool ret = EFalse;
	
	if(!GetStringFromConfig(ConfigSection(),KTe_RegStepTestSuiteString, originalValue))
		{
		// Leave if there's any error.
		User::Leave(KErrNotFound);
		}

	INFO_PRINTF2(_L("The Original String is %S"), &originalValue); // Block end
	
	RBuf buf;
	buf.Create(originalValue.Length());
	buf.Copy(originalValue);
	
	_LIT(KText,"GoodBye Jason");
	TBufC<16> buf1(KText); 
	TPtrC TheString1(buf1);
	if (WriteStringToConfig(ConfigSection(), KTe_RegStepTestSuiteString, TheString1))
		{
		if (GetStringFromConfig(ConfigSection(), KTe_RegStepTestSuiteString, TheString1) && 0==TheString1.Compare(KText()))
			{
			INFO_PRINTF2(_L("Changed String To %S"),&TheString1); 
			ret = ETrue;
			}
		}
		
	_LIT(KText2,"Hello Jason");
	TBufC<16> buf2(KText2); 
	TPtrC TheString2(buf2);
	
	if (WriteStringToConfig(ConfigSection(), KTe_RegStepTestSuiteString, TheString2))
		{
		if (GetStringFromConfig(ConfigSection(), KTe_RegStepTestSuiteString, TheString2) && 0==TheString2.Compare(KText2()))
			{
			INFO_PRINTF2(_L("Changed String To %S"), &TheString2); 
			}
		}
	else
		{
		ret = EFalse;
		}
	
	if (!WriteStringToConfig(ConfigSection(), KTe_RegStepTestSuiteString, buf))
		{
		ret = EFalse;
		}
	buf.Close();
	
	if (ret)
		{
		SetTestStepResult(EPass);
		}
	
	return TestStepResult();
	}



TVerdict CWriteStringStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}
