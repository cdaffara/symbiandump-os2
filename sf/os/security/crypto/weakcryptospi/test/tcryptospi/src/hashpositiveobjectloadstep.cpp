/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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
 @file
 @internalTechnology
*/
#include "hashpositiveobjectloadstep.h"

#include <cryptospi/cryptohashapi.h>


using namespace CryptoSpi;

CHashPositiveObjectLoadStep::~CHashPositiveObjectLoadStep()
	{
	}


CHashPositiveObjectLoadStep::CHashPositiveObjectLoadStep()
	{
	SetTestStepName(KHashPositiveObjectLoadStep);
	}


TVerdict CHashPositiveObjectLoadStep::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CHashPositiveObjectLoadStep::doTestStepL()
	{
	if (TestStepResult()==EPass)
		{
		
		//Assume faliure, unless all is successful
		SetTestStepResult(EFail);
			
		INFO_PRINTF1(_L("*** Hash - Positive Object Load ***"));
		INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
		
		TVariantPtrC algorithmUid;
		TVariantPtrC operationModeUid;
		
		//Extract the Test Case ID parameter from the specified INI file
		if(!GetStringFromConfig(ConfigSection(),KConfigAlgorithmUid,algorithmUid) ||
			!GetStringFromConfig(ConfigSection(),KConfigOperationMode,operationModeUid))
			{
			ERR_PRINTF1(_L("** Error: Failed to Load Configuration Parameters **"));
			SetTestStepResult(EFail);
			}
		else
			{
			//Create and intialise a pointer for the Hash implementation object
			CHash* hashImpl = NULL;
			
			INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
			
			//Retrieve a Hash factory object				
			TRAPD(err,CHashFactory::CreateHashL(hashImpl,
												algorithmUid,
												operationModeUid,
												NULL,
												NULL));  
				
			//If there are no errors (KErrNone) creating the Hash factory object, Pass the Test	
			if(hashImpl && (err == KErrNone))
				{
				INFO_PRINTF1(_L("*** Hash - Positive Object Load: PASS ***"));
				delete hashImpl;
				SetTestStepResult(EPass);	
				}
			else
				{
				ERR_PRINTF2(_L("*** FAIL: Failed to Create Hash Object - %d ***"), err);
				SetTestStepResult(EFail);	
				}
			}
		
		INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
		
		}
	return TestStepResult();
	}


TVerdict CHashPositiveObjectLoadStep::doTestStepPostambleL()
	{
	
	return TestStepResult();
	}
