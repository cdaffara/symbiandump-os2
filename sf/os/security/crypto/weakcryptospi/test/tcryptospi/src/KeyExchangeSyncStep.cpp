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
#include "keyexchangesyncstep.h"

#include <cryptospi/keypair.h>
#include <cryptospi/cryptokeypairgeneratorapi.h>
#include <cryptospi/cryptokeyagreementapi.h>
#include <bigint.h>

using namespace CryptoSpi;

CKeyExchangeSyncStep::~CKeyExchangeSyncStep()
	{
	}

CKeyExchangeSyncStep::CKeyExchangeSyncStep()
	{
	SetTestStepName(KKeyExchangeSyncStep);
	}

TVerdict CKeyExchangeSyncStep::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CKeyExchangeSyncStep::doTestStepL()
	{
	INFO_PRINTF1(_L("*** Key Exchange - DH Key Agreement ***"));
	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
	
  	if (TestStepResult()==EPass)
		{
		//Assume faliure, unless all is successful
		SetTestStepResult(EFail);

		TVariantPtrC testVariant;
		TVariantPtrC dhnVariant;
		TVariantPtrC dhgVariant;

		if(	!GetStringFromConfig(ConfigSection(),KConfigExchangeKey, dhnVariant) ||
			!GetStringFromConfig(ConfigSection(),KConfigExchangeKey, dhgVariant )
			)
			{
			// Leave if there's any error.
			User::Leave(KErrNotFound);
			}
		else
			{
			/*
	 		 * both DH keys (ie our private and their public keys) must use the same N and G parameters
	 		 */
	 		INFO_PRINTF1(_L("Creating Primes and Base Integers...")); 
	 		 
			RInteger DH_N = RInteger::NewPrimeL(1024);	// from ini file
			CleanupClosePushL(DH_N);
			RInteger DH_N_MinusTwo = RInteger::NewL(DH_N);
			CleanupClosePushL(DH_N_MinusTwo);
			DH_N_MinusTwo-=2;
			
			RInteger DH_G = RInteger::NewRandomL(TInteger::Two(), DH_N_MinusTwo);	
			CleanupClosePushL(DH_G);
			
			INFO_PRINTF1(_L("Creating Key Pair Generator...")); 

			// create a DH key pair generator interface for creating the 2 key pairs
			CKeyPairGenerator* keyPairGeneratorImpl = NULL;
			
			TRAPD_LOG(err,CKeyPairGeneratorFactory::CreateKeyPairGeneratorL(keyPairGeneratorImpl, 
														KDHKeyPairGeneratorUid, 
														NULL));
														
			if(keyPairGeneratorImpl && (err==KErrNone))	
				{
				
				CleanupStack::PushL(keyPairGeneratorImpl);

				// package up the common parameters N and G for use through the rest of this method
				CCryptoParams* keyParameters = CCryptoParams::NewLC();
				keyParameters->AddL(DH_N, KDhKeyParameterNUid);
				keyParameters->AddL(DH_G, KDhKeyParameterGUid);

				/* 
				 * call the api to create a DH key pair for alice
				 */
				INFO_PRINTF1(_L("Generating DH ALICE Key Pair..."));
				CKeyPair* keyAlice = NULL;
				
				TRAP_LOG(err,keyPairGeneratorImpl->GenerateKeyPairL(NULL,
																*keyParameters, 
																keyAlice));
				CleanupStack::PushL(keyAlice);

				/* 
				 * call the api to create a DH key pair for bob
				 */
				INFO_PRINTF1(_L("Generating DH BOB Key Pair..."));
				CKeyPair* keyBob = NULL;
				TRAP_LOG(err,keyPairGeneratorImpl->GenerateKeyPairL(NULL, 
																*keyParameters, 
																keyBob));
				CleanupStack::PushL(keyBob);

				/* 
				 * get DH key agreement interfaces
				 */
				INFO_PRINTF1(_L("Generating ALICE & BOB Key Agreement Interfaces..."));
				CKeyAgreement* keyAgreementAliceImpl =  NULL;
				
				TRAP_LOG(err,CKeyAgreementFactory::CreateKeyAgreementL(keyAgreementAliceImpl, 
																	KDHAgreementUid, 
																	keyAlice->PrivateKey(), 
																	keyParameters));
				CleanupStack::PushL(keyAgreementAliceImpl);
				
				CKeyAgreement* keyAgreementBobImpl =  NULL;
				
				TRAP_LOG(err,CKeyAgreementFactory::CreateKeyAgreementL(keyAgreementBobImpl, 
																	KDHAgreementUid, 
																	keyBob->PrivateKey(), 
																	keyParameters));
				CleanupStack::PushL(keyAgreementBobImpl);

				/* 
				 * call the api to get a DH agreed keys
				 */
				INFO_PRINTF1(_L("Generating Agreed Keys..."));
				
				CKey* agreedKeyAlice = keyAgreementAliceImpl->AgreeL(keyBob->PublicKey(), keyParameters);
				CleanupStack::PushL(agreedKeyAlice);
				
				CKey* agreedKeyBob = keyAgreementBobImpl->AgreeL(keyAlice->PublicKey(), keyParameters);
				CleanupStack::PushL(agreedKeyBob);

				/*
				 * compare the agreed keys
				 */
				const TInteger& agreedKeyDataAlice = agreedKeyAlice->GetBigIntL(KSymmetricKeyParameterUid);
				const TInteger& agreedKeyDataBob = agreedKeyBob->GetBigIntL(KSymmetricKeyParameterUid);
				
				if (agreedKeyDataAlice == agreedKeyDataBob)
					{
					INFO_PRINTF1(_L("*** PASS : Key Agreement Success ***"));
					SetTestStepResult(EPass);
					}
				else
					{
					INFO_PRINTF1(_L("*** FAIL : Agreed Keys Mismatch ***"));
					SetTestStepResult(EFail);
					}

				// Set the private keys to check the new agreements.
				keyAgreementAliceImpl->SetKeyL(keyAlice->PrivateKey(), keyParameters);
				keyAgreementAliceImpl->SetKeyL(keyBob->PrivateKey(), keyParameters);
				
				/* 
				* call the api to get a DH agreed keys
				*/
				INFO_PRINTF1(_L("Generating Agreed Keys second time..."));
								
				CKey* agreedKeyAlice1 = keyAgreementAliceImpl->AgreeL(keyBob->PublicKey(), keyParameters);
				CleanupStack::PushL(agreedKeyAlice1);
					
				CKey* agreedKeyBob1 = keyAgreementBobImpl->AgreeL(keyAlice->PublicKey(), keyParameters);
				CleanupStack::PushL(agreedKeyBob1);

				/*
				 * compare the agreed keys
				 */
				const TInteger& agreedKeyDataAlice1 = agreedKeyAlice->GetBigIntL(KSymmetricKeyParameterUid);
				const TInteger& agreedKeyDataBob1 = agreedKeyBob->GetBigIntL(KSymmetricKeyParameterUid);
				
				if (agreedKeyDataAlice1 != agreedKeyDataBob1)
					{					
					INFO_PRINTF1(_L("*** FAIL : Second Agreed Keys Mismatch ***"));
					SetTestStepResult(EFail);
					}	
							
				const CCryptoParams& cryptoParams1 = agreedKeyAlice1->KeyParameters();
				const CCryptoParams& cryptoParams2 = agreedKeyBob1->KeyParameters();				
				if (cryptoParams1.Count() != cryptoParams2.GetParams().Count())					
					{
					INFO_PRINTF1(_L("*** FAIL : Key Parameters' Count Mismatch ***"));
					SetTestStepResult(EFail);
					}
				
				TInt paramLength = 10;
				HBufC16* buf = HBufC16::NewLC(paramLength);
				TPtr16 ptr = buf->Des();
				ptr.Copy(_L("DH_N"));
				CCryptoParams* params = CCryptoParams::NewL();
				params->AddL(*buf, KDhKeyParameterNUid);
				if(!params->Count())
					{
					INFO_PRINTF1(_L("*** FAIL : Parameter construction with descriptor failed ***"));
					SetTestStepResult(EFail);
					}

				if (agreedKeyAlice1->IsPresent(KSymmetricKeyParameterUid))
					{
					TRAPD(err, agreedKeyAlice1->GetTIntL(KSymmetricKeyParameterUid));
					if(err == KErrNone)
						{
						INFO_PRINTF1(_L("*** FAIL : Expected Key Parameter Int Value Mismatch ***"));
						SetTestStepResult(EFail);
						}
					}
				
				// Clear the second key agreement elements.
				CleanupStack::PopAndDestroy(3, agreedKeyAlice1);
				
				/* 
				 * cleanup stack - it should contain privateKey, keyAgreementImpl, publicKey, keyParameters and agreedKey
				 */
				CleanupStack::PopAndDestroy(agreedKeyBob);
				CleanupStack::PopAndDestroy(agreedKeyAlice);
				CleanupStack::PopAndDestroy(keyAgreementBobImpl);
				CleanupStack::PopAndDestroy(keyAgreementAliceImpl);
				CleanupStack::PopAndDestroy(keyBob);
				CleanupStack::PopAndDestroy(keyAlice);
				CleanupStack::PopAndDestroy(keyParameters);
				CleanupStack::PopAndDestroy(keyPairGeneratorImpl);	
				}
		
			CleanupStack::PopAndDestroy(&DH_G);
			CleanupStack::PopAndDestroy(&DH_N_MinusTwo);
			CleanupStack::PopAndDestroy(&DH_N);
			}
			
		INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());

		}
  	return TestStepResult();
	}

TVerdict CKeyExchangeSyncStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}
