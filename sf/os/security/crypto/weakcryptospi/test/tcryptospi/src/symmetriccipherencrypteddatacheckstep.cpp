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
#include "symmetriccipherencrypteddatacheckstep.h"

using namespace CryptoSpi;


CSymmetricCipherEncryptedDataCheckStep::CSymmetricCipherEncryptedDataCheckStep()
	{
	}


CSymmetricCipherEncryptedDataCheckStep::~CSymmetricCipherEncryptedDataCheckStep()

	{
	}


TVerdict CSymmetricCipherEncryptedDataCheckStep::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CSymmetricCipherEncryptedDataCheckStep::doTestStepL()
	{
	INFO_PRINTF1(_L("*** Symmetric Cipher - Encrypted Data Check ***"));
	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
  	
  	if (TestStepResult()==EPass)
		{
		//Assume faliure, unless all is successful
		SetTestStepResult(EFail);
		
		TVariantPtrC operationMode;
		
		// Create a Symmetric Cipher with the values from the ini file	
		CryptoSpi::CSymmetricCipher * impl = NULL;
			
		CKey* key = NULL;
		SetupCipherL(ETrue, ETrue, operationMode, impl, key);
		
		INFO_PRINTF1(_L("Plugin loaded."));
	
		CleanupStack::PushL(key);
		CleanupStack::PushL(impl);
			
		//read from src file
		HBufC8* srcData = ReadInPlaintextL();
		CleanupStack::PushL(srcData);
			
		//Create buffer for encrypted data
		TInt maxOutputLength = impl->MaxFinalOutputLength(srcData->Length());
		HBufC8* encrypted =	HBufC8::NewLC(maxOutputLength);
		TPtr8 encryptedPtr = encrypted->Des();
		
		TInt err;

		if (((TUid)operationMode == KOperationModeCBCUid) || (TUid(operationMode) == KOperationModeCTRUid))
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
			
			// blocksize is in bits so to allocate the correct number of bytes devide by 8
			HBufC8* iv = HBufC8::NewLC(blockSize/8);
				
			// blocksize is in bits so to allocate the correct number of 8 byte chunks divide by 64
			for(TInt i = 0 ; i < blockSize/64 ; i++)
				{
				iv->Des().Append(_L8("12345678"));
				}
						
			TRAP_LOG(err,impl->SetIvL(iv->Des()));
					
			CleanupStack::PopAndDestroy(iv);
			}
					
		INFO_PRINTF1(_L("Encrypting Source Data..."));

		//Perform the encryption operation
		TRAP_LOG(err, impl->ProcessFinalL((*srcData), encryptedPtr));
	
		if(err == KErrNone)
			{
			//Check that expected data equals the encrypted data
			HBufC8* encryptedFileData = ReadInHexCiphertextL();
			CleanupStack::PushL(encryptedFileData);

					
			if(	!encryptedPtr.Compare(TPtrC8(*encryptedFileData)))
				{
				INFO_PRINTF1(_L("PASS : Encrypted Cipher Text Matches Expected Data"));
				SetTestStepResult(EPass);
				}
			else
				{
				INFO_PRINTF1(_L("FAIL : Encrypted Cipher Text and Expected Data Mismatch"));	
				SetTestStepResult(EFail);
				}

			CleanupStack::PopAndDestroy(encryptedFileData);
			}
	
		CleanupStack::PopAndDestroy(encrypted); 
		CleanupStack::PopAndDestroy(srcData);
		CleanupStack::PopAndDestroy(impl);
		CleanupStack::PopAndDestroy(key);
		}

	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());

	return TestStepResult();

	}


TVerdict CSymmetricCipherEncryptedDataCheckStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}
