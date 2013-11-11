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
#include "asymmetriccipherencrypteddatacheckstep.h"

#include <cryptospi/cryptoasymmetriccipherapi.h>
#include <cryptospi/cryptokeypairgeneratorapi.h>
#include <cryptospi/keypair.h>
#include "filereader.h"

using namespace CryptoSpi;

CASymmetricCipherEncryptedDataCheckStep::~CASymmetricCipherEncryptedDataCheckStep()
	{
	}


CASymmetricCipherEncryptedDataCheckStep::CASymmetricCipherEncryptedDataCheckStep()
	{
	SetTestStepName(KASymmetricCipherEncryptedDataCheckStep);
	}


TVerdict CASymmetricCipherEncryptedDataCheckStep::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CASymmetricCipherEncryptedDataCheckStep::doTestStepL()
	{

	INFO_PRINTF1(_L("*** Asymmetric Cipher - Encrypted Data Check ***"));
	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
	
  	if (TestStepResult()==EPass)
		{
		
		//Assume faliure, unless all is successful
		SetTestStepResult(EFail);
		
		TVariantPtrC algorithm;
		TVariantPtrC srcPath;
		TVariantPtrC encrypted;
		TVariantPtrC paddingMode;
		
		
		if(!GetStringFromConfig(ConfigSection(),KConfigAlgorithmUid, algorithm) || 
			!GetStringFromConfig(ConfigSection(),KConfigPaddingMode, paddingMode) ||
			!GetStringFromConfig(ConfigSection(),KConfigSourcePath, srcPath) || 
			!GetStringFromConfig(ConfigSection(),KConfigExEncryptedData, encrypted))
			{
			User::Leave(KErrNotFound);
			}
		else
			{
			//assume failure, unless all is successful
			SetTestStepResult(EFail);
			
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
			TRAPD_LOG(err,CKeyPairGeneratorFactory::CreateKeyPairGeneratorL(keypairImpl, 
													KRSAKeyPairGeneratorUid, 
													keyParams));
													
			CleanupStack::PushL(keypairImpl);

			// Create a Key Pair	
			TRAP_LOG(err,keypairImpl->GenerateKeyPairL(1024, *keyParams, keyPair));
			
			CleanupStack::PushL(keyPair);
			
			//*****************************************************
			
			INFO_PRINTF1(_L("Creating Asymmetric Cipher Object..."));

			// Create a Symmetric Cipher 
			CryptoSpi::CAsymmetricCipher * impl = NULL;	
			
			TRAP(err,CAsymmetricCipherFactory::CreateAsymmetricCipherL(impl,
														algorithm,
														keyPair->PublicKey(),
														KCryptoModeEncryptUid,
														paddingMode,
														NULL));
	
			if(impl && (err == KErrNone))
				{
				CleanupStack::PushL(impl);
				
				INFO_PRINTF1(_L("*** Asymmetric Cipher Implementation Created... ***"));
			
				//read from src file
				CFileReader* srcData = CFileReader::NewLC(srcPath);		
			
				//Create buffer for encrypted data
				TInt maxOutputLength = impl->GetMaximumOutputLengthL();
				HBufC8* encrypted =	HBufC8::NewLC(maxOutputLength);
				TPtr8 encryptedPtr = encrypted->Des();
				
				INFO_PRINTF1(_L("Encrypting Source Data..."));

				//Perform the encryption operation
				TRAP_LOG(err,impl->ProcessL(TPtrC8(*srcData), encryptedPtr));
				
				if(err == KErrNone)
					{
					INFO_PRINTF1(_L("*** DATA ENCRYPTED ***"));
					
					//Check that the source data matches the data thats 
					//been encrypted then decrypted
					if(	!encryptedPtr.Compare(TPtrC8(*srcData)))
						{
						INFO_PRINTF1(_L("PASS : Encrypted Data Matches Expected"));
						SetTestStepResult(EPass);
						}
					else
						{
						INFO_PRINTF1(_L("FAIL : Encrypted Data and Expected Mismatch"));	
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


TVerdict CASymmetricCipherEncryptedDataCheckStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}
