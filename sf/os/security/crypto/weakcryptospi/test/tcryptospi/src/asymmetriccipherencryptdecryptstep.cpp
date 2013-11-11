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
#include "asymmetriccipherencryptdecryptstep.h"

#include <cryptospi/cryptoasymmetriccipherapi.h>
#include <cryptospi/cryptokeypairgeneratorapi.h>
#include <cryptospi/keypair.h>
#include "filereader.h"

using namespace CryptoSpi;

CASymmetricCipherEncryptDecryptStep::~CASymmetricCipherEncryptDecryptStep()
	{
	}


CASymmetricCipherEncryptDecryptStep::CASymmetricCipherEncryptDecryptStep()
	{
	SetTestStepName(KASymmetricCipherEncryptDecryptStep);
	}


TVerdict CASymmetricCipherEncryptDecryptStep::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CASymmetricCipherEncryptDecryptStep::doTestStepL()
	{
	INFO_PRINTF1(_L("*** Asymmetric Cipher - Encrypt/Decrypt ***"));
	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
  	
  	if (TestStepResult()==EPass)
		{
		
		//Assume faliure, unless all is successful
		SetTestStepResult(EFail);
		
		TPtrC srcPath;
		TVariantPtrC paddingMode;
		
		if(!GetStringFromConfig(ConfigSection(),KConfigSourcePath, srcPath) ||
			!GetStringFromConfig(ConfigSection(),KConfigPaddingMode, paddingMode ))
			{
			User::Leave(KErrNotFound);
			}
		else
			{
			
			CCryptoParams* keyParams = CCryptoParams::NewLC(); 
			
			//****************************************************
			//Create Key Pair and Key Pair Generator Objects
   			CKeyPair* keyPair = NULL;
			CKeyPairGenerator * keypairImpl = NULL;
			
			// create an RSA key pair
			INFO_PRINTF1(_L("Generating RSA keys"));

			keyParams->AddL(KKeyExponent, KRsaKeyParameterEUid);
			keyParams->AddL(KRsaPrivateKeyStandard, KRsaKeyTypeUid);

			// create a key pair generator implementation interface
			TRAPD_LOG(err,CKeyPairGeneratorFactory::CreateKeyPairGeneratorL(
											keypairImpl, 
											KRSAKeyPairGeneratorUid, 
											keyParams));
											
			CleanupStack::PushL(keypairImpl);

			// Create a Key Pair	
			TRAP_LOG(err,keypairImpl->GenerateKeyPairL(1024, *keyParams, keyPair));
			
			CleanupStack::PushL(keyPair);
			
			//*****************************************************
			
			INFO_PRINTF1(_L("Creating Asymmetric Cipher Object..."));
			
			// Create an Asymmetric Cipher with the values fromt he ini	
			CryptoSpi::CAsymmetricCipher * impl = NULL;	
			
			TRAP(err,CAsymmetricCipherFactory::CreateAsymmetricCipherL
										(
										impl,
										KRsaCipherUid,
										keyPair->PublicKey(),
										KCryptoModeEncryptUid,
										paddingMode,
										NULL));
	
			if(impl && (err == KErrNone))
				{
				INFO_PRINTF1(_L("*** Asymmetric Cipher Implementation Created... ***"));
				
				CleanupStack::PushL(impl);
				
				//read from src file
				CFileReader* srcData = CFileReader::NewLC(srcPath);
				
				//Create buffer for encrypted data
				TInt maxOutputLength = impl->GetMaximumOutputLengthL();
				HBufC8* encrypted =	HBufC8::NewLC(maxOutputLength);
				TPtr8 encryptedPtr = encrypted->Des();
				
				INFO_PRINTF1(_L("Encrypting Source Data..."));

				//Perform the encryption operation
				TRAP_LOG(err, impl->ProcessL(static_cast<TPtrC8>(*srcData), encryptedPtr));
				
				if(err == KErrNone)
					{
					
					if(encryptedPtr.Compare(TPtrC8(*srcData)) != 0)
						{

						INFO_PRINTF1(_L("*** DATA ENCRYPTED ***"));
						
						//Switch to decrypt
						TRAP_LOG(err,impl->SetCryptoModeL(KCryptoModeDecryptUid));
						
						TRAP_LOG(err,impl->SetKeyL(keyPair->PrivateKey()));

						//Create a buffer for the decrypted data
						maxOutputLength = encryptedPtr.Length();
						
						TInt bufSize = impl->GetMaximumOutputLengthL();
						
						HBufC8* output = HBufC8::NewLC(bufSize);
						TPtr8 outputPtr = output->Des();
						
						INFO_PRINTF1(_L("Decrypting Data..."));
						
						//Perform the decryption operation
						TRAP_LOG(err, impl->ProcessL(encryptedPtr, outputPtr));
						
						if(err == KErrNone)
							{
							INFO_PRINTF1(_L("*** DATA DECRYPTED ***"));
							//Check that the source data matches the data thats 
							//been encrypted then decrypted
							if(!outputPtr.Compare(*srcData))
								{
								INFO_PRINTF1(_L("PASS : Decrypted Data matches expected Source"));
								SetTestStepResult(EPass);
								}
							else
								{
								INFO_PRINTF1(_L("FAIL : Decrypted Data and expected Mismatch"));	
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
					}
				
				
				CleanupStack::PopAndDestroy(encrypted);
				CleanupStack::PopAndDestroy(srcData);
				CleanupStack::PopAndDestroy(impl);
				}
			else
				{
				ERR_PRINTF2(_L("*** FAIL: Failed to Create Asymmetric Object - %d ***"), err);
				User::Leave(err);	
				}
			
			CleanupStack::PopAndDestroy(keyPair);
			CleanupStack::PopAndDestroy(keypairImpl);	
			CleanupStack::PopAndDestroy(keyParams);
			}
		}
	
	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());

	return TestStepResult();
	}


TVerdict CASymmetricCipherEncryptDecryptStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}
