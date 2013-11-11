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
#include "signerpositiveobjectloadstep.h"

#include <cryptospi/keypair.h>
#include <cryptospi/cryptosignatureapi.h>
#include <cryptospi/cryptokeypairgeneratorapi.h>

using namespace CryptoSpi;


CSignerPositiveObjectLoadStep::~CSignerPositiveObjectLoadStep()
	{
	}

CSignerPositiveObjectLoadStep::CSignerPositiveObjectLoadStep()
	{
	SetTestStepName(KSignerPositiveObjectLoadStep);
	}

TVerdict CSignerPositiveObjectLoadStep::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CSignerPositiveObjectLoadStep::doTestStepL()
	{
	INFO_PRINTF1(_L("*** Signer - Positive Object Load ***"));
	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
	
  	if (TestStepResult()==EPass)
		{
		//Assume faliure, unless all is successful
		SetTestStepResult(EFail);
		
		TVariantPtrC testVariant;
		TVariantPtrC keyVariant;

		if(	!GetStringFromConfig(ConfigSection(),KConfigExchangeKey, keyVariant))
			{
			// Leave if there's any error.
			User::Leave(KErrNotFound);
			}
		else
			{
			//Create an new CryptoParams object to encapsulate the key type and secret key string
			CCryptoParams* keyParams = CCryptoParams::NewL();
			CleanupStack::PushL(keyParams);

			//Set the Key Parameters
			keyParams->AddL(KKeyExponent, KRsaKeyParameterEUid);
			keyParams->AddL(KRsaPrivateKeyStandard, KRsaKeyTypeUid);
			
			// Create a Key Pair Generator implementation
			INFO_PRINTF1(_L("Creating Key Pair Generator...")); 
			
			CKeyPairGenerator * keypairImpl = NULL;

			TRAPD_LOG(err,CKeyPairGeneratorFactory::CreateKeyPairGeneratorL(keypairImpl, 
																KRSAKeyPairGeneratorUid, 
																keyParams));
																
			CleanupStack::PushL(keypairImpl);
							
							
			// Generate a Key Pair 
			INFO_PRINTF1(_L("Generating Key Pair..."));
			
			CKeyPair* keyPair = NULL;
			
			TRAP_LOG(err,keypairImpl->GenerateKeyPairL(1024, 
												*keyParams, 
												keyPair));
												
			CleanupStack::PushL(keyPair);

			// Creating Signer 1 with the necessary values from the ini	file
			INFO_PRINTF1(_L("Constructing Signer 1 (No Padding)..."));
			CSigner * impl1 = NULL;	
		    TRAP_LOG(err,CSignatureFactory::CreateSignerL(impl1,
												   KRsaSignerUid,
												   keyPair->PublicKey(),
												   KPaddingModeNoneUid,
												   keyParams));
			
			CleanupStack::PushL(impl1);
			
			// Creating Signer 2 with the necessary values from the ini	file
			INFO_PRINTF1(_L("Constructing Signer 2 (Pkcs1_v1_5 Padding)..."));
			CSigner * impl2 = NULL;	
		    TRAP_LOG(err,CSignatureFactory::CreateSignerL(impl2,
												   KRsaSignerUid,
												   keyPair->PublicKey(),
												   KPaddingModePkcs1_v1_5_SignatureUid,
												   keyParams));
			
			CleanupStack::PushL(impl2);

			if((impl1) && (impl2) && (err == KErrNone))
				{
				// This is a basic test for load positivity
				// from the Factory
				INFO_PRINTF1(_L("PASS: Signer - Positive Object Load"));
				SetTestStepResult(EPass);
				}
			else
				{
				ERR_PRINTF2(_L("FAIL: Signer Object Construction Failure - %d"), err);
				SetTestStepResult(EFail);
				}

			CleanupStack::PopAndDestroy(impl2);	
			CleanupStack::PopAndDestroy(impl1);
			
			CleanupStack::PopAndDestroy(keyPair);
			CleanupStack::PopAndDestroy(keypairImpl);
			CleanupStack::PopAndDestroy(keyParams);
			}

		INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
		
		}
	  return TestStepResult();
	}



TVerdict CSignerPositiveObjectLoadStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}
