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
 @file te_regperformancelowmemoryteststep.cpp
 @internalTechnology
*/
#include <stdlib.h>
#include <hal.h>
#include <errno.h>
#include "te_regperformancelowmemoryteststep.h"

CTe_RegPerformanceLowMemoryTestStep::~CTe_RegPerformanceLowMemoryTestStep()
/**
 * Destructor
 */
	{
	
	}

CTe_RegPerformanceLowMemoryTestStep::CTe_RegPerformanceLowMemoryTestStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KTe_RegPerformanceLowMemoryTestStep);
	}

TVerdict CTe_RegPerformanceLowMemoryTestStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CTe_RegPerformanceLowMemoryTestStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	TInt mallocSize = 5000;
	TInt reallocSize = 6000;
	INFO_PRINTF2(_L("Try to apply %D bytes memory!"), mallocSize);
	char* p = (char*) User::Alloc(mallocSize);
	if(p !=NULL )
		{
		INFO_PRINTF2(_L("Apply %D bytes memory successful!"), mallocSize);
		p = (char*)User::ReAlloc(p, sizeof(char)*reallocSize);
		if(p!=NULL)
			{
			INFO_PRINTF2(_L("realloc %D bytes memory successful!"), reallocSize);
			}
		else
			{
			ERR_PRINTF2(_L("realloc %D bytes memory failed!"), reallocSize);
			}
		User::Free(p);
		}
	else
		{
		ERR_PRINTF2(_L("Apply %D bytes memory failed!"), mallocSize);
		}
	
	return TestStepResult();
	}



TVerdict CTe_RegPerformanceLowMemoryTestStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}
