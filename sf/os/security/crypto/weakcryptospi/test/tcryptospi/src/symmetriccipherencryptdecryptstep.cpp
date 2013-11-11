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
#include "symmetriccipherencryptdecryptstep.h"

using namespace CryptoSpi;

CSymmetricCipherEncryptDecryptStep::~CSymmetricCipherEncryptDecryptStep()
	{
	}


CSymmetricCipherEncryptDecryptStep::CSymmetricCipherEncryptDecryptStep()
	{
	SetTestStepName(KSymmetricCipherEncryptDecryptStep);
	}


TVerdict CSymmetricCipherEncryptDecryptStep::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CSymmetricCipherEncryptDecryptStep::doTestStepL()
	{
	INFO_PRINTF1(_L("*** Symmetric Cipher - Encrypt/Decrypt ***"));
	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
	
  	if (TestStepResult()==EPass)
		{
		
		//Assume faliure, unless all is successful
		SetTestStepResult(EFail);
		
		TVariantPtrC operationMode;

		// Create a Symmetric Cipher with the values from the ini file	
		CryptoSpi::CSymmetricCipher * impl = NULL;	
			
		CKey* key = NULL;
		SetupCipherL(ETrue, EFalse, operationMode, impl, key);

		INFO_PRINTF1(_L("Plugin loaded."));
	
		CleanupStack::PushL(key);
		CleanupStack::PushL(impl);

				
		HBufC8* iv = NULL;
		TInt err(0);
				
		if((TUid(operationMode) == KOperationModeCBCUid) || (TUid(operationMode) == KOperationModeCTRUid))
			{
			TInt blockSize(0);
					
			if (TUid(operationMode) == KOperationModeCTRUid)
				{
				blockSize = CtrModeCalcBlockSizeL(*impl);
				}
			else
				{
				blockSize = impl->BlockSize();
				}
			
			// (blocksize is in bits so to allocate the correct number of bytes divide by 8)
			// iv is left on the cleanup stack for the duration of the test and deleted in a conditional at the end of the outer block.
			// If this conditional block changes, take care to update the condition for deleting this allocated IV, near the end of this function.
			iv = HBufC8::NewLC(blockSize/8);	
			
			// blocksize is in bits so to allocate the correct number of 8 byte chunks divide by 64
			for(TInt i = 0 ; i <blockSize/64 ; i++)
				{
				iv->Des().Append(_L8("12345678"));
				}
					
			TRAP_LOG(err,impl->SetIvL(iv->Des()));
			}
			
		//read from src file
		HBufC8* srcData = ReadInPlaintextL();
		CleanupStack::PushL(srcData);
					
		//Create buffer for encrypted data
		TInt maxOutputLength = impl->MaxFinalOutputLength(srcData->Length());
		HBufC8* encrypted =	HBufC8::NewLC(maxOutputLength);
		TPtr8 encryptedPtr = encrypted->Des();

		INFO_PRINTF1(_L("Encrypting Source Data..."));

		//Perform the encryption operation
		TRAP_LOG(err, impl->ProcessFinalL((*srcData), encryptedPtr));
					
		if(encryptedPtr.Compare((*srcData)) != 0)
			{
			//Switch to decrypt
			TRAP_LOG(err,impl->SetCryptoModeL(KCryptoModeDecryptUid));

			//If in CTR mode need to reset the keystream to the start of the sequence used for encryption
			if(TUid(operationMode) == KOperationModeCTRUid)
				{
				impl->SetIvL(iv->Des());
				}
					
			//Create a buffer for the decrypted data
			maxOutputLength = encryptedPtr.Length();
					
			TInt bufSize = impl->MaxFinalOutputLength(maxOutputLength);
					
			HBufC8* output = HBufC8::NewLC(bufSize);
			TPtr8 outputPtr = output->Des();
					
			INFO_PRINTF1(_L("Decrypting Data..."));
					
			//Perform the decryption operation
			TRAP_LOG(err, impl->ProcessFinalL(encryptedPtr, outputPtr));
					
			if(err == KErrNone)
				{
				//Check that the source data matches the data thats 
				//been encrypted then decrypted
				if(	!outputPtr.Compare(*srcData) )
					{
					INFO_PRINTF1(_L("PASS : Decrypted Data and Source Match"));
					SetTestStepResult(EPass);
					}
				else
					{
					INFO_PRINTF1(_L("FAIL : Decrypted Data and Source Mismatch"));	
					SetTestStepResult(EFail);
					}
				}
						
			CleanupStack::PopAndDestroy(output); 
			}
		else
			{
			INFO_PRINTF1(_L("FAIL : Encrpyted Data and Source Data length and content is the same"));	
			SetTestStepResult(EFail);	
			}
					
		CleanupStack::PopAndDestroy(encrypted); 
		CleanupStack::PopAndDestroy(srcData);
		if((TUid(operationMode) == KOperationModeCBCUid) || (TUid(operationMode) == KOperationModeCTRUid))
			{
			// Iv is left on the cleanupstack at creation.  
			// If it becomes possible for operationMode to be modified during
			// the test this needs to be re-engineered.
			CleanupStack::PopAndDestroy(iv);	
			}
		CleanupStack::PopAndDestroy(2, key);		
		}
	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());

	return TestStepResult();

	}


TVerdict CSymmetricCipherEncryptDecryptStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}
