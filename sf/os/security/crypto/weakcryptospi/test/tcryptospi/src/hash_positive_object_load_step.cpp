/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "hash_positive_object_load_step.h"
#include <cryptospi/cryptohashapi.h>


using namespace CryptoSpi;

CHashPositiveLoadStep::~CHashPositiveLoadStep()
	{
	}


CHashPositiveLoadStep::CHashPositiveLoadStep()
	{
	SetTestStepName(KHashPositiveLoadStep);
	}


TVerdict CHashPositiveLoadStep::doTestStepPreambleL()
	{
	return EPass;
	}


TVerdict CHashPositiveLoadStep::doTestStepL()
	{
	//Assume faliure, unless all is successful
	SetTestStepResult(EFail);
			
	INFO_PRINTF1(_L("*** Hash - Positive Load ***"));
	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
	
	TVariantPtrC algorithmUid;
	
	//Extract the Test Case ID parameter from the specified INI file
	if(!GetStringFromConfig(ConfigSection(),KConfigAlgorithmUid,algorithmUid))
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
											NULL));  
			
		//If there are no errors (KErrNone) creating the Hash factory object, Pass the Test	
		if(err == KErrNone)
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
	return TestStepResult();
	}


TVerdict CHashPositiveLoadStep::doTestStepPostambleL()
	{
	
	return TestStepResult();
	}
