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
#include "asymmetriccipherpositiveobjectloadstep.h"

#include <cryptospi/cryptoasymmetriccipherapi.h>
#include <cryptospi/cryptokeypairgeneratorapi.h>
#include <cryptospi/keypair.h>
#include "filereader.h"

using namespace CryptoSpi;

CASymmetricCipherPositiveObjectLoadStep::~CASymmetricCipherPositiveObjectLoadStep()
	{
	}


CASymmetricCipherPositiveObjectLoadStep::CASymmetricCipherPositiveObjectLoadStep()
	{
	SetTestStepName(KASymmetricCipherPositiveObjectLoadStep);
	}


TVerdict CASymmetricCipherPositiveObjectLoadStep::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CASymmetricCipherPositiveObjectLoadStep::doTestStepL()
	{

	INFO_PRINTF1(_L("*** Asymmetric Cipher - Positive Object Load ***"));
	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
	
  	if (TestStepResult()==EPass)
		{
		
		//Assume faliure, unless all is successful
		SetTestStepResult(EFail);
		
		TVariantPtrC algorithm;
		TVariantPtrC paddingMode;
		
		
		if(!GetStringFromConfig(ConfigSection(),KConfigAlgorithmUid, algorithm) || 
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
			
			INFO_PRINTF1(_L("Generating RSA keys"));
			
			// create an RSA key pair
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
			
			// Create a Symmetric Cipher 
			CryptoSpi::CAsymmetricCipher * impl = NULL;
				
			TRAP(err,CAsymmetricCipherFactory::CreateAsymmetricCipherL
												(
												impl,
												algorithm,
												keyPair->PrivateKey(),
												KCryptoModeEncryptUid,
												paddingMode,
												NULL));
	
			if(impl && (err == KErrNone))
				{
				// This is a basic test for load positivity
				// from the Factory
				delete impl;
				INFO_PRINTF1(_L("*** Asymmetric Cipher - Positive Object Load: PASS ***"));
				SetTestStepResult(EPass);
				}
			else
				{
				ERR_PRINTF2(_L("*** FAIL: Failed to Create Asymmetric Cipher Object - %d ***"), err);
				SetTestStepResult(EFail);
				}
				
			CleanupStack::PopAndDestroy(keyPair);
			CleanupStack::PopAndDestroy(keypairImpl);			
			CleanupStack::PopAndDestroy(keyParams);			
			}
			
		}
	
	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());

	return TestStepResult();
	}



TVerdict CASymmetricCipherPositiveObjectLoadStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}
