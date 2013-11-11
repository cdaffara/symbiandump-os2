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
#include "signerverifierstep.h"

#include <cryptospi/keypair.h>
#include <cryptospi/cryptosignatureapi.h>
#include <cryptospi/cryptokeypairgeneratorapi.h>

using namespace CryptoSpi;


CSignerVerifierStep::~CSignerVerifierStep()
	{
	}

CSignerVerifierStep::CSignerVerifierStep()
	{
	SetTestStepName(KSignerVerifierStep);
	}

TVerdict CSignerVerifierStep::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CSignerVerifierStep::doTestStepL()
	{
	INFO_PRINTF1(_L("*** Signer/Verifier - Signature and Verification ***"));
	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
	
  	if (TestStepResult()==EPass)
		{
		//Assume faliure, unless all is successful
		SetTestStepResult(EFail);
		
		TVariantPtrC testVariant;
		TVariantPtrC typeVariant;
		TVariantPtrC dataVariant;

		if(	!GetStringFromConfig(ConfigSection(),KConfigSignVerifyType, typeVariant) 
			|| !GetStringFromConfig(ConfigSection(),KConfigSignVerifyData, dataVariant))
			{
			// Leave if there's any error.
			User::Leave(KErrNotFound);
			}
		else
			{
			TUid sigType = typeVariant;
						
			//Construct a Crypto Parameters object to store the necessary key pair generator parameters
			CCryptoParams* keyParams = CCryptoParams::NewL();
			CleanupStack::PushL(keyParams);

			//Create Key Pair Generator Object
			CKeyPairGenerator * keypairImpl = NULL;
			
			//Define error code as first TRAP is beyond scope
			TInt err = 0;
			
			if (sigType == KRsaSignerUid)			
				{
				INFO_PRINTF1(_L("Creating Key Pair Generator (RSA)..."));

				keyParams->AddL(KKeyExponent, KRsaKeyParameterEUid);
				keyParams->AddL(KRsaPrivateKeyStandard, KRsaKeyTypeUid);
				
				//Construct an 'RSA' Key Pair Generator Implementation Object
				TRAP_LOG(err,CKeyPairGeneratorFactory::CreateKeyPairGeneratorL(keypairImpl, 
																KRSAKeyPairGeneratorUid, 
																keyParams));
				}
			else
				{
				INFO_PRINTF1(_L("Creating Key Pair Generator (DSA)..."));
				
				//Construct a 'DSA' Key Pair Generator Implementation Object
				TRAP_LOG(err,CKeyPairGeneratorFactory::CreateKeyPairGeneratorL(keypairImpl,
                                            					KDSAKeyPairGeneratorUid,
                                            					NULL));
				}
				
			CleanupStack::PushL(keypairImpl);
	
			// Generate a Key Pair 
			INFO_PRINTF1(_L("Generating Key Pair..."));
			
			CKeyPair* keyPair = NULL;
				
			TRAP_LOG(err,keypairImpl->GenerateKeyPairL(1024, 
													*keyParams, 
													keyPair));

			CleanupStack::PushL(keyPair);
			
			
			// Create a Signer Object	
			INFO_PRINTF1(_L("Creating Signer Object..."));
			CCryptoParams* svParams = CCryptoParams::NewL();
			CleanupStack::PushL(svParams);

			CSigner * implsig = NULL;	
		    TRAP_LOG(err,CSignatureFactory::CreateSignerL(implsig,
													   sigType,
													   keyPair->PublicKey(),
													   KPaddingModePkcs1_v1_5_SignatureUid,
													   svParams));
										   
			CleanupStack::PushL(implsig);
		
		
			// Create a Verifier
			INFO_PRINTF1(_L("Creating Verifier Object..."));
			
			CVerifier * implver = NULL;	
		    TRAP_LOG(err,CSignatureFactory::CreateVerifierL(implver,
										   				sigType == KRsaSignerUid ? KRsaVerifierUid : KDsaVerifierUid,
										   				keyPair->PrivateKey(),
										   				KPaddingModePkcs1_v1_5_SignatureUid,
										   				svParams));
										   				
			CleanupStack::PushL(implver);
										   
			//Define a new signature object
			CCryptoParams *signature = CCryptoParams::NewL();
			CleanupStack::PushL(signature);

			//Convert the source data to an 8 Bit Descriptor
			HBufC8* sourceData = HBufC8::NewLC(dataVariant.Length());
			TPtr8 sourceDataPtr = sourceData->Des();
			
			sourceDataPtr.Copy(dataVariant);
			
			//Set the signer object to use the 'Private' Key of the key pair
			INFO_PRINTF1(_L("Setting Signer Private Key..."));
			TRAP_LOG(err,implsig->SetKeyL(keyPair->PrivateKey()));
			
			//Generate a signature for the source data
			INFO_PRINTF1(_L("Signing..."));
			TRAP_LOG(err,implsig->SignL(*sourceData, *signature));
    	
    		TBool bResult = EFalse;	
    		
    		//Set the verifier object to use the 'Public' Key of the key pair
    		INFO_PRINTF1(_L("Setting Verifier Public Key..."));
			TRAP_LOG(err,implver->SetKeyL(keyPair->PublicKey()));
			
			//Verify that the generated signature is valid for the given data
			INFO_PRINTF1(_L("Verifying..."));
			TRAP_LOG(err,implver->VerifyL(*sourceData, *signature, bResult));

			if (bResult)
				{
				// verifier agrees signature is valid
				INFO_PRINTF1(_L("PASS: Verification of Generated Signature Successful"));
				SetTestStepResult(EPass);
				}
			else
				{
				//verification failure, signature is invalid
				ERR_PRINTF2(_L("FAIL: Verification Failure of Generated Signature - %d"), err);
				SetTestStepResult(EFail);
				}
			
			CleanupStack::PopAndDestroy(sourceData);	
			CleanupStack::PopAndDestroy(signature);
			CleanupStack::PopAndDestroy(implver);
			CleanupStack::PopAndDestroy(implsig);
			CleanupStack::PopAndDestroy(svParams);
			
			CleanupStack::PopAndDestroy(keyPair);
			CleanupStack::PopAndDestroy(keypairImpl);
			CleanupStack::PopAndDestroy(keyParams);
			}

		INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
		
		}
	  return TestStepResult();
	}



TVerdict CSignerVerifierStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}
