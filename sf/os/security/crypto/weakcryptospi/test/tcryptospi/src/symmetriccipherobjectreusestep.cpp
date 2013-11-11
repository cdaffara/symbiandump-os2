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
#include "symmetriccipherobjectreusestep.h"

#include "filewriter.h"
#include "filecompare.h"

using namespace CryptoSpi;


CSymmetricCipherObjectReuseStep::CSymmetricCipherObjectReuseStep(TInt aOffset) : iOffset(aOffset)
	{
	}


CSymmetricCipherObjectReuseStep::~CSymmetricCipherObjectReuseStep()
	{
	}


TVerdict CSymmetricCipherObjectReuseStep::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CSymmetricCipherObjectReuseStep::doTestStepL()
	{
	INFO_PRINTF1(_L("*** Symmetric Cipher - Object Reuse ***"));
	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
  	if (TestStepResult()==EPass)
		{
		
		//Assume failure, unless all is successful
		SetTestStepResult(EFail);
		
		TPtrC keyPath;
		TPtrC srcPath;
		TVariantPtrC algorithm;
		TVariantPtrC operationMode;
		TVariantPtrC paddingMode;
		
		if(	!GetStringFromConfig(ConfigSection(),KConfigEncryptKey, keyPath) ||
			!GetStringFromConfig(ConfigSection(),KConfigSourcePath, srcPath) || 
			!GetStringFromConfig(ConfigSection(),KConfigAlgorithmUid, algorithm) || 
			!GetStringFromConfig(ConfigSection(),KConfigOperationMode, operationMode) ||
			!GetStringFromConfig(ConfigSection(),KConfigPaddingMode, paddingMode ))
			{
			User::Leave(KErrNotFound);
			}
		else
			{
			
			//Create an instance of TKeyProperty
			TKeyProperty keyProperty;
			
			//Load the key data using the
			CFileReader* keyData = CFileReader::NewLC(keyPath);
			
			CCryptoParams* params = CCryptoParams::NewLC(); 
			params->AddL( *keyData, KSymmetricKeyParameterUid); 
			
			CKey* key=CKey::NewL(keyProperty, *params);
			CleanupStack::PushL(key);
			
			CCryptoParams* xparams = NULL;
			
			if (TUid(algorithm) == KArc4Uid)
				{
				//Set the RC4 DiscardBytes to 0
				xparams = CCryptoParams::NewL();
				xparams->AddL(NULL, KARC4DiscardBytes);
				CleanupStack::PushL(xparams);
				}

			if (TUid(algorithm) == KRc2Uid)
				{
				TInt keylen = TPtrC8(*keyData).Length() * 8;
				xparams = CCryptoParams::NewLC();
				
				//Set the RC2 EffectiveKeyLen according to the input key size
				xparams->AddL( keylen, KRC2EffectiveKeyLenBits);
				}
			
			INFO_PRINTF1(_L("Creating Symmetric Cipher Object..."));

			// Create a Symmetric Cipher with the values from the ini config file	
			CryptoSpi::CSymmetricCipher * impl = NULL;	
			TRAPD(err,CSymmetricCipherFactory::CreateSymmetricCipherL
										(
										impl,
										algorithm,
										*key,
										KCryptoModeEncryptUid,
										operationMode,
										paddingMode,
										xparams));
	
			if(impl && (err == KErrNone))
				{
				CleanupStack::PushL(impl);
				
				const TInt KObjectReuseItterations = 5; // 5 iterations should be enough to check the object reuse feature
														// the no of iteration is reduced, to reduce the time taken for execution
				
				//Boolean to denote the state				
				TBool testPass = ETrue;
				
				/*************** Encrypt/Decrypt Reuse Loop ****************/
				for(TInt index = 0; index < KObjectReuseItterations; index++)
					{
					INFO_PRINTF3(_L("i=%d : START HEAP CELLS: %d"),index, User::CountAllocCells());
					
					//-----RESET IMPLEMENTATION OBJECT (NORMAL LOGGING)----------
					
					impl->Reset();
					
					TRAP(err,impl->SetKeyL(*key));
					
					if(err != KErrNone)
						{
						ERR_PRINTF3(_L("*** ERROR:%d - SetKeyL() i=%d ***"),err,index);
						User::Leave(err);	
						}
						
					TRAP(err,impl->SetCryptoModeL(KCryptoModeEncryptUid));
					
					if(err != KErrNone)
						{
						ERR_PRINTF3(_L("*** ERROR:%d - SetCryptoModeL() i=%d ***"),err,index);
						User::Leave(err);	
						}
						
					if(TUid(algorithm) != KArc4Uid)
						{
						
						impl->SetOperationModeL(operationMode);
					
						if(err != KErrNone)
							{
							ERR_PRINTF3(_L("*** ERROR:%d - SetOperationModeL() i=%d ***"),err,index);
							User::Leave(err);	
							}
							
						TRAP(err,impl->SetPaddingModeL(paddingMode));
						
						if(err != KErrNone)
							{
							ERR_PRINTF3(_L("*** ERROR:%d - SetPaddingModeL() i=%d ***"),err,index);
							User::Leave(err);	
							}
								
						}

					//------------------------------------------------------
					
					//find out the block size for this algorithm
					TInt blockSize(0);
					
					if (TUid(operationMode) == KOperationModeCTRUid)
						{
						blockSize = CtrModeCalcBlockSizeL(*impl);
						}
					else
						{
						blockSize = impl->BlockSize();	
						}
					
					HBufC8* iv = NULL;
					
					if ((TUid(operationMode) == KOperationModeCBCUid) || (TUid(operationMode) == KOperationModeCTRUid))
						{
						// block size is in bits so to allocate the correct number of bytes divide by 8
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
					CFileWriter* encryptedDataWriter = CFileWriter::NewLC(TPtrC(KTempEncryptedFilePath));
					
					TInt numBlocks = srcData->NumBlocks();
					
					INFO_PRINTF1(_L("Starting Incremental Encryption..."));
					
					for(TInt i = 1 ; i <= numBlocks ; i++)
						{
						TRAP_LOG(err,srcData->ReadBlockL());
					
						//Create buffer for encrypted data
						TInt maxOutputLength = impl->MaxFinalOutputLength(TPtrC8(*srcData).Length());
						HBufC8* encrypted =	HBufC8::NewLC(maxOutputLength);
						TPtr8 encryptedPtr = encrypted->Des();
					
						if(i == numBlocks)
							{
							TRAP(err,impl->ProcessFinalL(*srcData, encryptedPtr));
							
							if(err != KErrNone)
								{
								ERR_PRINTF3(_L("*** ERROR:%d - ProcessFinalL() Block=%d ***"),err,i);
								User::Leave(err);	
								}
							}
						else
							{
							TRAP(err,impl->ProcessL(*srcData, encryptedPtr));
							
							if(err != KErrNone)
								{
								ERR_PRINTF3(_L("*** ERROR:%d - ProcessL() Block=%d ***"),err,i);
								User::Leave(err);	
								}
							}	
						
						TRAP_LOG(err,encryptedDataWriter->WriteBlockL(encryptedPtr));
					
						CleanupStack::PopAndDestroy(encrypted); 
						}
						
					CleanupStack::PopAndDestroy(encryptedDataWriter); 
						
					//Switch to Decrypt Crypto Mode
					TRAP(err,impl->SetCryptoModeL(KCryptoModeDecryptUid));
					
					if(err != KErrNone)
						{
						ERR_PRINTF3(_L("*** ERROR:%d - SetCryptoModeL() i=%d ***"),err,index);
						User::Leave(err);	
						}

					//If in CTR mode need to reset the keystream to the start of the sequence used for encryption.
					if(TUid(operationMode) == KOperationModeCTRUid)
						{
						impl->SetIvL(iv->Des());
						}

					
					// the next step is to read the previously encrypted data
					// from the temporary file decrypting this one block
					// at a time and outputing this to a temporary file.
					CFileReader* encryptedDataReader = CFileReader::NewLC(TPtrC(KTempEncryptedFilePath),blockSize);
					CFileWriter* decryptedDataWriter = CFileWriter::NewLC(TPtrC(KTempDecryptedFilePath));
					
					numBlocks = encryptedDataReader->NumBlocks();
					
					INFO_PRINTF1(_L("Starting Incremental Decryption..."));
					
					for(TInt i = 1 ; i <= numBlocks ; i++)
						{
						encryptedDataReader->ReadBlockL();
						//Create buffer for encrypted data
						TInt maxOutputLength = impl->MaxFinalOutputLength(TPtrC8(*encryptedDataReader).Length());
						HBufC8* decrypted =	HBufC8::NewLC(maxOutputLength);
						TPtr8 decryptedPtr = decrypted->Des();

						//Perform the decryption operation
						if(i == numBlocks)
							{
							TRAP(err,impl->ProcessFinalL(*encryptedDataReader, decryptedPtr));
							
							if(err != KErrNone)
								{
								ERR_PRINTF3(_L("*** ERROR:%d - ProcessFinalL() Block=%d ***"),err,i);
								User::Leave(err);	
								}
							}
						else
							{
							TRAP(err,impl->ProcessL(*encryptedDataReader, decryptedPtr));
							
							if(err != KErrNone)
								{
								ERR_PRINTF3(_L("*** ERROR:%d - ProcessL() Block=%d ***"),err,i);
								User::Leave(err);	
								}
							}
							
						TRAP_LOG(err,decryptedDataWriter->WriteBlockL(decryptedPtr));
					
						CleanupStack::PopAndDestroy(decrypted); 
						}
						
					CleanupStack::PopAndDestroy(decryptedDataWriter); 
					CleanupStack::PopAndDestroy(encryptedDataReader); 
					CleanupStack::PopAndDestroy(srcData);
					
					if((TUid(operationMode) == KOperationModeCBCUid) || (TUid(operationMode) == KOperationModeCTRUid))
						{
						// Iv is left on the cleanupstack at creation.  
						// If it becomes possible for operationMode to be modified during
						// the test this needs to be re-engineered.
						CleanupStack::PopAndDestroy(iv);
						}
					
					
					// compare the src with the file thats been
					// encrypted then decrypted
					// Note: Returning 0 means that the files match
					if(!TFileCompare::CompareL(srcPath,TPtrC(KTempDecryptedFilePath)))
						{
						INFO_PRINTF2(_L("*** PASS = Source File and Decrypted Data Match - i=%d ***"),index);
						}
					else
						{
						testPass = EFalse;
						ERR_PRINTF2(_L("*** ERROR: Source File and Decrypted Data Mismatch - i=%d ***"),index);
						}
						
					RFs rFs;
					rFs.Connect();
					rFs.Delete(	KTempDecryptedFilePath );
					rFs.Delete(	KTempEncryptedFilePath );
					rFs.Close();
										
					INFO_PRINTF3(_L("*** i=%d : END HEAP CELLS: %d ***"),index, User::CountAllocCells());
					}
					
				/*************** END OF LOOP ****************/
					
				CleanupStack::PopAndDestroy(impl);
				
				if(testPass == EFalse)
					{
					ERR_PRINTF1(_L("*** TEST FAIL : Symmetric Cipher - Object Reuse ***"));
					}
				else
					{
					INFO_PRINTF1(_L("*** TEST PASS : Symmetric Cipher - Object Reuse ***"));
					SetTestStepResult(EPass);	
					}
				
				}
			else
				{
				ERR_PRINTF2(_L("*** FAIL: Failed to Create Symmetric Object - %d ***"), err);
				User::Leave(err);	
				}
								
			if (TUid(algorithm) == KArc4Uid || TUid(algorithm) == KRc2Uid)
				{
				CleanupStack::PopAndDestroy(xparams);				
				}
				
			CleanupStack::PopAndDestroy(key);
			CleanupStack::PopAndDestroy(params);
			CleanupStack::PopAndDestroy(keyData);
			}
		}
	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());

	return TestStepResult();

	}


TVerdict CSymmetricCipherObjectReuseStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}
