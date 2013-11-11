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
#include "verifierpositiveobjectloadstep.h"

#include <cryptospi/keys.h>
#include <cryptospi/keypair.h>
#include <cryptospi/cryptosignatureapi.h>
#include <cryptospi/cryptokeypairgeneratorapi.h>

using namespace CryptoSpi;

CVerifierPositiveObjectLoadStep::~CVerifierPositiveObjectLoadStep()
	{
	}

CVerifierPositiveObjectLoadStep::CVerifierPositiveObjectLoadStep()
	{
	SetTestStepName(KVerifierPositiveObjectLoadStep);
	}

TVerdict CVerifierPositiveObjectLoadStep::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CVerifierPositiveObjectLoadStep::doTestStepL()
	{
	INFO_PRINTF1(_L("*** Verifier - Positive Object Load ***"));
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


			// Creating Verifier 1 with necessary values from the ini file	
			INFO_PRINTF1(_L("Constructing Verifier 1 (No Padding)..."));
			
			CVerifier * impl1 = NULL;	
		    TRAP_LOG(err,CSignatureFactory::CreateVerifierL
		    							   (impl1,
										   KRsaVerifierUid,
										   keyPair->PrivateKey(),
										   KPaddingModeNoneUid,
										   keyParams));
			
			CleanupStack::PushL(impl1);							   
			
			
			// Creating Verifier 2 with the necessary values from the ini file	
			INFO_PRINTF1(_L("Constructing Verifier 1 (No Padding)..."));

			CVerifier * impl2 = NULL;	
		    TRAP_LOG(err,CSignatureFactory::CreateVerifierL
		    							   (impl2,
										   KRsaVerifierUid,
										   keyPair->PrivateKey(),
										   KPaddingModePkcs1_v1_5_SignatureUid,
										   keyParams));
										   
			CleanupStack::PushL(impl2);
										   
			if((impl1) && (impl2) && (err == KErrNone))
				{
				// This is a basic test for load positivity
				// from the Factory
				INFO_PRINTF1(_L("PASS: Verifier - Positive Object Load"));
				SetTestStepResult(EPass);
				}
			else
				{
				ERR_PRINTF2(_L("FAIL: Verifier Object Construction Failure - %d"), err);
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



TVerdict CVerifierPositiveObjectLoadStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}
