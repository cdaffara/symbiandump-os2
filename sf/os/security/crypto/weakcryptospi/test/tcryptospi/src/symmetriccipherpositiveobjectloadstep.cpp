/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "symmetriccipherpositiveobjectloadstep.h"

#include <cryptospi/cryptosymmetriccipherapi.h>
#include <cryptospi/keys.h>
#include "filereader.h"

using namespace CryptoSpi;

CSymmetricCipherPositiveObjectLoadStep::~CSymmetricCipherPositiveObjectLoadStep()
	{
	}


CSymmetricCipherPositiveObjectLoadStep::CSymmetricCipherPositiveObjectLoadStep()
	{
	SetTestStepName(KSymmetricCipherPositiveObjectLoadStep);
	}


TVerdict CSymmetricCipherPositiveObjectLoadStep::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CSymmetricCipherPositiveObjectLoadStep::doTestStepL()
	{

	INFO_PRINTF1(_L("*** Symmetric Cipher - Positive Object Load ***"));
	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
	
  	if (TestStepResult()==EPass)
		{
		
		//Assume failure, unless all is successful
		SetTestStepResult(EFail);
		
		TVariantPtrC operationMode;
		
		TBool shouldLoad(EFalse);

		// Create a Symmetric Cipher with the values from the ini file	
		CryptoSpi::CSymmetricCipher * impl = NULL;	
			
		CKey* key = NULL;
		TRAPD(err, SetupCipherL(ETrue, EFalse, operationMode, impl, key));


		if(impl && err==KErrNone)
			{
			INFO_PRINTF1(_L("*** Symmetric Cipher - Positive Object Load: PASS ***"));
			delete impl;
			delete key;
			SetTestStepResult(EPass);
			}
		else if (!impl && err==KErrNotSupported && 
			     GetBoolFromConfig(ConfigSection(),_L("ShouldLoad"), shouldLoad) &&
			     shouldLoad == EFalse)
			{
			INFO_PRINTF1(_L("*** Symmetric Cipher - The object was not loaded which was expected ***"));
			INFO_PRINTF1(_L("*** Symmetric Cipher - Positive Object Load: PASS ***"));
			SetTestStepResult(EPass);
			}
		else
			{
			ERR_PRINTF2(_L("*** FAIL: Failed to Create Symmetric Cipher Object - %d ***"), err);
			SetTestStepResult(EFail);
			}	
		}
		
	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());

	return TestStepResult();
	}


TVerdict CSymmetricCipherPositiveObjectLoadStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}
