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
 
#include "symmetriccipherctrmodeoutoforderstep.h"

using namespace CryptoSpi;

CSymmetricCipherCtrModeOutOfOrderStep::CSymmetricCipherCtrModeOutOfOrderStep()
	{
	SetTestStepName(KSymmetricCipherCtrModeOutOfOrderStep);
	}


TVerdict CSymmetricCipherCtrModeOutOfOrderStep::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CSymmetricCipherCtrModeOutOfOrderStep::doTestStepL()
	{
	INFO_PRINTF1(_L("*** Symmetric Cipher - Counter mode out of order operation ***"));
	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
  	
  	if (TestStepResult() != EPass)
  		{
  		return TestStepResult();
  		}
  	
  	//Assume failure, unless all is successful
	SetTestStepResult(EFail);
		
	TVariantPtrC operationMode;
		
	CSymmetricCipher* impl = NULL;
	CKey* key = NULL;
	SetupCipherL(EFalse, EFalse, operationMode, impl, key);
	
	INFO_PRINTF1(_L("Plugin loaded."));
	
	CleanupStack::PushL(key);
	CleanupStack::PushL(impl);

  	
  	if (TUid(operationMode) != KOperationModeCTRUid)
  		{
  		ERR_PRINTF2(_L("*** FAIL: This test supports CTR operation mode only and not mode id: %d ***"), (TUid(operationMode)).iUid);
  		CleanupStack::PopAndDestroy(2, key);
  		INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
  		return TestStepResult();
  		}
	
	
	HBufC8* plaintext = ReadInPlaintextL();
	CleanupStack::PushL(plaintext);

		
	TInt blockSize = CtrModeCalcBlockSizeL(*impl)/8;

	if (plaintext->Length() < ((blockSize * 2) + 1))
		{
		ERR_PRINTF2(_L("*** FAIL: Plaintext argument is not long enough for this test, length(B) = %d ***"), plaintext->Length());
		CleanupStack::PopAndDestroy(3, key);
		INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
		return TestStepResult();
		}
				
  	// This will store our calculated version of the ciphertext to compare with that in the .ini file
  	TUint8* calculatedCiphertext = new (ELeave) TUint8[plaintext->Length()];
  	CleanupStack::PushL(calculatedCiphertext);
  	// Ptr to the first block of the calculated ciphertext
  	TPtr8 calcCipherPtr1(calculatedCiphertext, blockSize);
  	// Ptr to the remaining blocks of the calculated ciphertext
  	TPtr8 calcCipherPtr2((calculatedCiphertext + blockSize), (plaintext->Length() - blockSize));
  	
  	// Ptr to the first block of the .ini file's plaintext
  	TPtrC8 knownPlainPtr1(plaintext->Ptr(), blockSize);
  	// Ptr to the remaining blocks of the .ini file's plaintext
  	TPtrC8 knownPlainPtr2((plaintext->Ptr() + blockSize), (plaintext->Length() - blockSize));
  	
	
	HBufC8* iv = ReadInIvL();
	CleanupStack::PushL(iv);
		
	// Increment IV to the value for the second block so we can encrypt blocks 2+ first
	HBufC8* incrementedIv1 = CtrModeIncrementCounterL((*iv));
	CleanupStack::PushL(incrementedIv1);
	
	impl->SetIvL(*incrementedIv1);
  	
  	
	INFO_PRINTF1(_L("Setup complete.  Encrypting blocks 2+."));
	impl->ProcessL(knownPlainPtr2, calcCipherPtr2);
	
	INFO_PRINTF1(_L("Blocks 2+ encrypted.  Reseting and encrypting block 1."));
	impl->SetIvL((*iv));
	impl->ProcessL(knownPlainPtr1, calcCipherPtr1);
	
	
	HBufC8* knownCiphertext = ReadInCiphertextL();
	CleanupStack::PushL(knownCiphertext);
	
	// Check that calculated ciphertext matches the expected value
	TPtrC8 wholeCalcCiphertext(calculatedCiphertext, (plaintext->Length()));
	if (wholeCalcCiphertext.Compare((*knownCiphertext)) != 0)
		{
		ERR_PRINTF1(_L("*** FAIL: Calculated ciphertext does not match expected value ***"));
  		CleanupStack::PopAndDestroy(7, key);
  		INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
  		return TestStepResult();
		}
	else
		{
		INFO_PRINTF1(_L("Calculated ciphertext matches the expected value."));
		}
	
	
	// ****  SWITCH TO DECRYPTION NOW  ****
	
	// This will store our calculated version of the plaintext to compare with that in the .ini file
  	TUint8* calculatedPlaintext = new (ELeave) TUint8[plaintext->Length()];
  	CleanupStack::PushL(calculatedPlaintext);
  	// Ptr to the first block of the calculated plaintext
  	TPtr8 calcPlainPtr1(calculatedPlaintext, (blockSize * 2));
  	// Ptr to the remaining blocks of the calculated plaintext
  	TPtr8 calcPlainPtr2((calculatedPlaintext + (blockSize * 2)), (plaintext->Length() - (blockSize * 2)));
  	
  	// Ptr to the first 2 blocks of the ciphertext
  	TPtrC8 knownCipherPtr1(wholeCalcCiphertext.Ptr(), (blockSize * 2));
  	// Ptr to the remaining blocks of the ciphertext
  	TPtrC8 knownCipherPtr2((wholeCalcCiphertext.Ptr() + (blockSize * 2)), (wholeCalcCiphertext.Length() - (blockSize * 2)));
	
	
	//	Increment IV to the value for the third block so we can decrypt blocks 3+ first
	HBufC8* incrementedIv2 = CtrModeIncrementCounterL(*incrementedIv1);
	CleanupStack::PushL(incrementedIv2);
	impl->SetIvL(*incrementedIv2);
	
	INFO_PRINTF1(_L("Setup complete.  Decrypting blocks 3+."));
	impl->ProcessL(knownCipherPtr2, calcPlainPtr2);
	
	INFO_PRINTF1(_L("Blocks 3+ encrypted.  Reseting and encrypting blocks 1 and 2."));
	impl->SetIvL((*iv));
	impl->ProcessL(knownCipherPtr1, calcPlainPtr1);	
	
	TPtrC8 wholeCalcPlaintext(calculatedPlaintext, (plaintext->Length()));
	
	if (wholeCalcPlaintext.Compare((*plaintext)) != 0)
		{
		ERR_PRINTF1(_L("*** FAIL: Calculated plaintext does not match expected value ***"));
		}
	else
		{
		INFO_PRINTF1(_L("*** PASS: Calculated plaintext matches the original one ***"));
		SetTestStepResult(EPass);			
		}
  	
  	CleanupStack::PopAndDestroy(9, key);
  	
	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());

	return TestStepResult();
	}


TVerdict CSymmetricCipherCtrModeOutOfOrderStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}
