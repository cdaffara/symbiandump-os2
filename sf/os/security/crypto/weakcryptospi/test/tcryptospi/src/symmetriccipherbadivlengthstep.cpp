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
*
*/


/**
 @file
 @internalTechnology
*/

#include "symmetriccipherbadivlengthstep.h"

using namespace CryptoSpi;

CSymmetricCipherBadIvLengthStep::CSymmetricCipherBadIvLengthStep()
	{
	SetTestStepName(KSymmetricCipherBadIvLengthStep);
	}


TVerdict CSymmetricCipherBadIvLengthStep::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CSymmetricCipherBadIvLengthStep::doTestStepL()
	{
	INFO_PRINTF1(_L("*** Symmetric Cipher - Trying to set the IV to a descriptor of the wrong length ***"));
	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
	
  	if (TestStepResult()!=EPass)
  		{
  		return TestStepResult(); 
  		}
		
	// Assume failure, unless all is successful
	SetTestStepResult(EFail);
	
	TVariantPtrC operationMode;
	CSymmetricCipher* impl = NULL;
	CKey* key = NULL;
	SetupCipherL(EFalse, EFalse, operationMode, impl, key);
	
	INFO_PRINTF1(_L("Plugin loaded."));
	
	CleanupStack::PushL(key);
	CleanupStack::PushL(impl);
			
	if((TUid(operationMode) == KOperationModeCBCUid) || (TUid(operationMode) == KOperationModeCTRUid))
		{
		TInt blockSize(0);
				
		if (TUid(operationMode) == KOperationModeCTRUid)
			{
			blockSize = CtrModeCalcBlockSizeL(*impl) * 2;
			}
		else
			{
			blockSize = impl->BlockSize() * 2;
			}
		
		// blocksize is in bits so to allocate the correct number of bytes devide by 8
		HBufC8* iv = HBufC8::NewLC(blockSize/8);
		
		// blocksize is in bits so to allocate the correct number of 8 byte chunks divide by 64
		for(TInt i = 0 ; i <blockSize/64 ; i++)
			{
			iv->Des().Append(_L8("12345678"));
			}
									
		TRAPD(err,impl->SetIvL(iv->Des()));
				
		if(err == KErrArgument)
			{
			INFO_PRINTF1(_L("PASS : SetIvL() errored with KErrArgument as expected"));
			SetTestStepResult(EPass);
			}
		else if (err == KErrNone)
			{
			ERR_PRINTF1(_L("*** FAIL: SetIvL() did not error ***"));
			SetTestStepResult(EFail);						
			}
		else
			{
			ERR_PRINTF2(_L("*** FAIL: SetIvL() errored with unexpected error code - %d ***"), err);
			SetTestStepResult(EFail);				
			}
		CleanupStack::PopAndDestroy(iv);					
		}
	else
		{
		ERR_PRINTF2(_L("*** FAIL: This test requires an operation mode that supports an initialisation vector and does not support operation id: %d ***"), (TUid(operationMode)).iUid);
		SetTestStepResult(EFail);					
		}

	CleanupStack::PopAndDestroy(impl);
	CleanupStack::PopAndDestroy(key);
			
	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());

	return TestStepResult();
	}


TVerdict CSymmetricCipherBadIvLengthStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}
