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
#include "symmetriccipherincrementalencryptdecryptstep.h"

#include "filewriter.h"
#include "filecompare.h"

using namespace CryptoSpi;

CSymmetricCipherIncrementalEncryptDecryptStep::~CSymmetricCipherIncrementalEncryptDecryptStep()
	{
	}


CSymmetricCipherIncrementalEncryptDecryptStep::CSymmetricCipherIncrementalEncryptDecryptStep(TInt aOffset) : iOffset(aOffset)
	{
	SetTestStepName(KSymmetricCipherIncrementalEncryptDecryptStep);
	}


TVerdict CSymmetricCipherIncrementalEncryptDecryptStep::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CSymmetricCipherIncrementalEncryptDecryptStep::doTestStepL()
	{
	INFO_PRINTF1(_L("*** Symmetric Cipher - Incremental Encrypt/Decrypt ***"));
	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
	
  	if (TestStepResult()==EPass)
		{
		
		//Assume faliure, unless all is successful
		SetTestStepResult(EFail);
		
		TPtrC srcPath;
		if (!GetStringFromConfig(ConfigSection(),KConfigSourcePath, srcPath))
			{
			User::Leave(KErrNotFound);
			}
		
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
		
		TInt blockSize(0);
		if (TUid(operationMode) == KOperationModeCTRUid)
			{
			blockSize = CtrModeCalcBlockSizeL(*impl);
			}
		else
			{
			blockSize = impl->BlockSize();
			}

		if((TUid(operationMode) == KOperationModeCBCUid) || (TUid(operationMode) == KOperationModeCTRUid))
			{
			// blocksize is in bits so to allocate the correct number of bytes divide by 8
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

		// convert to bytesize
		blockSize/=8;
		blockSize += iOffset;
		
		//read from src file
		CFileReader* srcData = CFileReader::NewLC(srcPath,blockSize);


		// first step is to read from the src file one block
		// at a time, encrypt that block and then write
		// the encrypted block out to a temporary file.
		CFileWriter* encryptedDataWriter = CFileWriter::NewLC(TPtrC(KEncryptedFilePath));
			
		TInt numBlocks = srcData->NumBlocks();
				
		INFO_PRINTF1(_L("Encrypting Source Data..."));
	
		for(TInt i = 1 ; i <= numBlocks ; i++)
			{
			TRAP_LOG(err,srcData->ReadBlockL());
			
			//Create buffer for encrypted data
			TInt maxOutputLength = impl->MaxFinalOutputLength(TPtrC8(*srcData).Length());
			HBufC8* encrypted =	HBufC8::NewLC(maxOutputLength);
			TPtr8 encryptedPtr = encrypted->Des();

			if(i == numBlocks)
				{
				TRAP_LOG(err,impl->ProcessFinalL(*srcData, encryptedPtr));
				}
			else
				{
				TRAP_LOG(err,impl->ProcessL(*srcData, encryptedPtr));
				}
					
			TRAP_LOG(err,encryptedDataWriter->WriteBlockL(encryptedPtr));
				
			CleanupStack::PopAndDestroy(encrypted); 
			}
		CleanupStack::PopAndDestroy(encryptedDataWriter); 
				
		if(err == KErrNone)
			{
			//Switch to decrypt
			TRAP_LOG(err,impl->SetCryptoModeL(KCryptoModeDecryptUid));
					
			//If in CTR mode need to reset the keystream to the start of the sequence used for encryption.
			if(TUid(operationMode) == KOperationModeCTRUid)
				{
				impl->SetIvL(iv->Des());
				}
				
			// the next step is to read the previously encrypted data
			// from the temporary file decrypting this one block
			// at a time and outputing this to a temporary file.
			CFileReader* encryptedDataReader = CFileReader::NewLC(TPtrC(KEncryptedFilePath),blockSize);
			CFileWriter* decryptedDataWriter = CFileWriter::NewLC(TPtrC(KDecryptedFilePath));
				
			numBlocks = encryptedDataReader->NumBlocks();
					
			INFO_PRINTF1(_L("Decrypting Data..."));
					
			for(TInt i = 1 ; i <= numBlocks ; i++)
				{
				TRAP_LOG(err,encryptedDataReader->ReadBlockL());
						
				//Create buffer for encrypted data
				TInt maxOutputLength = impl->MaxFinalOutputLength(TPtrC8(*encryptedDataReader).Length());
				HBufC8* decrypted =	HBufC8::NewLC(maxOutputLength);
				TPtr8 decryptedPtr = decrypted->Des();

				//Perform the decryption operation
				if(i == numBlocks)
					{
					TRAP_LOG(err,impl->ProcessFinalL(*encryptedDataReader, decryptedPtr));
					}
				else
					{
					TRAP_LOG(err,impl->ProcessL(*encryptedDataReader, decryptedPtr));
					}
							
				TRAP_LOG(err,decryptedDataWriter->WriteBlockL(decryptedPtr));
					
				CleanupStack::PopAndDestroy(decrypted); 
				}
						
			CleanupStack::PopAndDestroy(decryptedDataWriter); 
			CleanupStack::PopAndDestroy(encryptedDataReader); 	
			}
				
		CleanupStack::PopAndDestroy(srcData);
		if((TUid(operationMode) == KOperationModeCBCUid) || (TUid(operationMode) == KOperationModeCTRUid))
			{
			// Iv is left on the cleanupstack at creation.  
			// If it becomes possible for operationMode to be modified during
			// the test this needs to be re-engineered.
			CleanupStack::PopAndDestroy(iv);
			}
		CleanupStack::PopAndDestroy(impl);							
		CleanupStack::PopAndDestroy(key);
	
		// compare the src with the file thats been
		// encrypted then decrypted
		if(!TFileCompare::CompareL(srcPath,TPtrC(KDecryptedFilePath)))
			{
			INFO_PRINTF1(_L("PASS : Source File and Decrypted Data Match"));
			SetTestStepResult(EPass);
			}
		else
			{
			INFO_PRINTF1(_L("FAIL : Source File and Decrypted Data Mismatch"));	
			SetTestStepResult(EFail);
			}
				
		RFs rFs;
		rFs.Connect();
		rFs.Delete(	KDecryptedFilePath );
		rFs.Delete(	KEncryptedFilePath );
		rFs.Close();
		}
	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());

	return TestStepResult();
	}


TVerdict CSymmetricCipherIncrementalEncryptDecryptStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}

