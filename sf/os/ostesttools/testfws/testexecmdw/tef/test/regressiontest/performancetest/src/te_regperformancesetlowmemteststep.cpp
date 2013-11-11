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
 @file te_regperformancesetlowmemteststep.cpp
 @internalTechnology
*/
#include <hal.h>
#include "te_regperformancesetlowmemteststep.h"

_LIT(KFreeRamSize, "FreeRamSize");
_LIT(KChunkName, "ChunkName");
const TInt KDefaultFreeRamSize = 8196;

CTe_RegPerformanceSetLowMemTestStep::~CTe_RegPerformanceSetLowMemTestStep()
/**
 * Destructor
 */
	{
	
	}

CTe_RegPerformanceSetLowMemTestStep::CTe_RegPerformanceSetLowMemTestStep(RChunk& aChunk) : iChunk(aChunk)
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KTe_RegPerformanceSetLowMemTestStep);
	}

TVerdict CTe_RegPerformanceSetLowMemTestStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CTe_RegPerformanceSetLowMemTestStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	INFO_PRINTF1(_L("Set Low Ram Condition"));
	
	TInt	freeRamSize=0;
	
	if(GetIntFromConfig(ConfigSection(), KFreeRamSize, freeRamSize))
		{
		INFO_PRINTF2(_L("The value of freeramsize is %D bytes"), freeRamSize);
		}
	else
		{
		freeRamSize = KDefaultFreeRamSize;
		INFO_PRINTF2(_L("No freeramsize value!, default value(freeramsize=%d bytes) is used!"), freeRamSize);
		}

	TPtrC	chunkNamePtr;
	if(GetStringFromConfig(ConfigSection(), KChunkName, chunkNamePtr))
		{
		INFO_PRINTF2(_L("ChunkName = %S"),&chunkNamePtr);
		}
	else
		{
		INFO_PRINTF1(_L("No Chunk Name!"));
		SetTestStepResult(EFail);
		}

	iChunk.Close();
	TInt totalFree = 0;
	User::LeaveIfError(HAL::Get(HALData::EMemoryRAMFree, totalFree));
	INFO_PRINTF2(_L("RAM status before stress condition: Total Free RAM:%D bytes"), totalFree);

	TInt maxHeapSize = totalFree-freeRamSize;
	if(TestStepResult() == EPass)
		{
		TInt err = KErrNone;
		err = iChunk.CreateGlobal(chunkNamePtr, maxHeapSize, maxHeapSize);
		if(err != KErrNone)
			{
			ERR_PRINTF1(_L("Create global chunk error!"));
			SetTestStepResult(EFail);
			}
		}

	if(TestStepResult() == EPass)
		{
		User::LeaveIfError(HAL::Get(HALData::EMemoryRAMFree, totalFree));
		INFO_PRINTF2(_L("RAM status after stress condition: Total Free RAM:%D bytes"), totalFree);
		}

	return TestStepResult();
	}



TVerdict CTe_RegPerformanceSetLowMemTestStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}
