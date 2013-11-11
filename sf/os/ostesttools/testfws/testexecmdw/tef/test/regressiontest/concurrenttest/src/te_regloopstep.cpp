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
 @file te_regconcurrentteststep2.cpp
 @internalTechnology
*/
#include "te_regloopstep.h"
#include "te_regconcurrenttestsuitedefs.h"

CTe_RegLoopStep::~CTe_RegLoopStep()
/**
 * Destructor
 */
	{
	
	}

CTe_RegLoopStep::CTe_RegLoopStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KTe_RegLoopStep);
	}

TVerdict CTe_RegLoopStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	INFO_PRINTF3(_L("Running %S in server %S, loop 1000000 times..."), &ConfigSection(), &GetServerName());
	for (TInt i=0; i<1000000; i++)
		{
		}
	return TestStepResult();
	}
