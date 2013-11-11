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
 @file te_regconcurrentteststep3.cpp
 @internalTechnology
*/
#include "te_regaccessstaticvarstep.h"
#include "te_regconcurrenttestsuitedefs.h"

TInt CTe_RegAccessStaticVarStep::Iterator = 0;

CTe_RegAccessStaticVarStep::~CTe_RegAccessStaticVarStep()
/**
 * Destructor
 */
	{
	
	}

CTe_RegAccessStaticVarStep::CTe_RegAccessStaticVarStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KTe_RegAccessStaticVarStep);
	}

TVerdict CTe_RegAccessStaticVarStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	INFO_PRINTF3(_L("Running %S in server %S, increase static variable 10 times..."), &ConfigSection(), &GetServerName());
	for (TInt i=0; i<10; i++)
		{
		++Iterator;
		}
	return TestStepResult();
	}
