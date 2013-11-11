/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "dummyecccipherloadstep.h"

#include <cryptospi/cryptoasymmetriccipherapi.h>
#include <cryptospi/cryptokeypairgeneratorapi.h>
#include <cryptospi/keypair.h>
#include "filereader.h"

using namespace CryptoSpi;

CDummyEccCipherLoadStep::~CDummyEccCipherLoadStep()
	{
	}


CDummyEccCipherLoadStep::CDummyEccCipherLoadStep()
	{
	SetTestStepName(KDummyEccCipherLoadStep);
	}


TVerdict CDummyEccCipherLoadStep::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CDummyEccCipherLoadStep::doTestStepL()
	{

	INFO_PRINTF1(_L("*** Dummy Ecc Cipher - Load ***"));
	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
	
    //Assume failure, unless all is successful
    SetTestStepResult(EFail);
    
    TVariantPtrC algorithm;
    TVariantPtrC paddingMode;
    
    if(!GetStringFromConfig(ConfigSection(),KConfigAlgorithmUid, algorithm) || 
        !GetStringFromConfig(ConfigSection(),KConfigPaddingMode, paddingMode ))
        {
        ERR_PRINTF1(_L("*** FAIL: Algorithm id or padding mode is missing ***"));
        User::Leave(KErrNotFound);
        }
    INFO_PRINTF1(_L("Generating dummy ECC keys"));
          
    /**
     * Note that we are actually generating RSA keys in place 
     * of ECC keys here. The reason for that is we do not have 
     * a working ECC implementation. This test case just tests 
     * if dummyecc cipher is getting loaded. This is being 
     * tested since algorithm is set to KAlgorithmCipherEcc. 
     * Keys have no significance in this test case.
     */
    CCryptoParams* keyParams = CCryptoParams::NewLC(); 
    keyParams->AddL(KKeyExponent, KRsaKeyParameterEUid);
    keyParams->AddL(KRsaPrivateKeyStandard, KRsaKeyTypeUid);

    //Create Key Pair Generator Objects
    CKeyPairGenerator * keypairImpl = NULL;
            
    // create a key pair generator implementation interface
    TRAPD_LOG(err,CKeyPairGeneratorFactory::CreateKeyPairGeneratorL(
                                    keypairImpl, 
                                    KRSAKeyPairGeneratorUid, 
                                    keyParams));
    if(err != KErrNone)
        {
        ERR_PRINTF1(_L("*** FAIL: Failed to Create generator impl interface ***"));
        CleanupStack::PopAndDestroy(keyParams);
        return EFail;
        }
    CleanupStack::PushL(keypairImpl);

    // Create a Key Pair
    CKeyPair* keyPair = NULL;
    TRAP_LOG(err,keypairImpl->GenerateKeyPairL(1024, *keyParams, keyPair));
    if(err != KErrNone)
        {
        ERR_PRINTF1(_L("*** FAIL: Failed to Create key pair ***"));
        CleanupStack::PopAndDestroy(keypairImpl);
        CleanupStack::PopAndDestroy(keyParams);
        return EFail;
        }
    CleanupStack::PushL(keyPair);
    
    //*****************************************************

    INFO_PRINTF1(_L("Creating Dummy ECC Cipher Object..."));
    
    CryptoSpi::CAsymmetricCipher * impl = NULL;
        
    TRAP(err,CAsymmetricCipherFactory::CreateAsymmetricCipherL
                                        (
                                        impl,
                                        algorithm,
                                        keyPair->PrivateKey(),
                                        KCryptoModeDecryptUid,
                                        paddingMode,
                                        NULL));

    if(err == KErrNone)
        {
        delete impl;
        INFO_PRINTF1(_L("*** Dummy Ecc Cipher - Load: PASS ***"));
        SetTestStepResult(EPass);
        }
    else
        {
        ERR_PRINTF2(_L("*** FAIL: Failed to Create dummy Ecc Cipher Object - %d ***"), err);
        }

    CleanupStack::PopAndDestroy(keyPair);
    CleanupStack::PopAndDestroy(keypairImpl);			
    CleanupStack::PopAndDestroy(keyParams);
		
	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());

	return TestStepResult();
	}



TVerdict CDummyEccCipherLoadStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}
