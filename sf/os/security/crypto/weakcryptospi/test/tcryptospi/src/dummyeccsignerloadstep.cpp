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
#include "dummyeccsignerloadstep.h"

#include <cryptospi/keypair.h>
#include <cryptospi/cryptosignatureapi.h>
#include <cryptospi/cryptokeypairgeneratorapi.h>

using namespace CryptoSpi;


CDummyEccSignerLoadStep::~CDummyEccSignerLoadStep()
	{
	}

CDummyEccSignerLoadStep::CDummyEccSignerLoadStep()
	{
	SetTestStepName(KDummyEccSignerLoadStep);
	}

TVerdict CDummyEccSignerLoadStep::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CDummyEccSignerLoadStep::doTestStepL()
	{
	INFO_PRINTF1(_L("*** Dummy Ecc Signer - Load ***"));
	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
	
    //Assume failure, unless all is successful
    SetTestStepResult(EFail);
    
    TVariantPtrC testVariant;
    TVariantPtrC keyVariant;

    if(	!GetStringFromConfig(ConfigSection(),KConfigExchangeKey, keyVariant))
        {
        // Leave if there's any error.
        ERR_PRINTF1(_L("*** FAIL: Problem in configuration file ***"));
        User::Leave(KErrNotFound);
        }
    //Create an new CryptoParams object to encapsulate the key type and secret key string
    CCryptoParams* keyParams = CCryptoParams::NewLC();

    /**
     * Note that we are actually generating RSA keys in place 
     * of ECC keys here. The reason for that is we do not have 
     * a working ECC implementation. This test case just tests 
     * if dummyecc signer is getting loaded. This is getting  
     * tested since KEccSignerUid is being passed as input to  
     * CreateSignerL. Keys have no significance in this test 
     * case.
     */
    //Set the Key Parameters
    keyParams->AddL(KKeyExponent, KRsaKeyParameterEUid);
    keyParams->AddL(KRsaPrivateKeyStandard, KRsaKeyTypeUid);
    
    // Create a Key Pair Generator implementation
    INFO_PRINTF1(_L("Creating Key Pair Generator...")); 
    
    CKeyPairGenerator * keypairImpl = NULL;
    TRAPD_LOG(err,CKeyPairGeneratorFactory::CreateKeyPairGeneratorL(keypairImpl, 
                                                        KRSAKeyPairGeneratorUid, 
                                                        keyParams));
    if(err != KErrNone)
        {
        ERR_PRINTF1(_L("*** FAIL: Failed to Create generator impl interface ***"));
        CleanupStack::PopAndDestroy(keyParams);
        return EFail;
        }                             
    CleanupStack::PushL(keypairImpl);
                    
    // Generate a Key Pair 
    INFO_PRINTF1(_L("Generating Key Pair..."));
    
    CKeyPair* keyPair = NULL;
    
    TRAP_LOG(err,keypairImpl->GenerateKeyPairL(1024, 
                                        *keyParams, 
                                        keyPair));
    if(err != KErrNone)
        {
        ERR_PRINTF1(_L("*** FAIL: Failed to Create key pair ***"));
        CleanupStack::PopAndDestroy(keypairImpl);
        CleanupStack::PopAndDestroy(keyParams);
        return EFail;
        }                                    
    CleanupStack::PushL(keyPair);

    // Creating Signer 1 with the necessary values from the ini	file
    INFO_PRINTF1(_L("Constructing Signer 1 (No Padding)..."));
    CSigner * impl = NULL;	
    TRAP_LOG(err,CSignatureFactory::CreateSignerL(impl,
                                           KEccSignerUid,
                                           keyPair->PublicKey(),
                                           KPaddingModeNoneUid,
                                           keyParams));
    
    CleanupStack::PushL(impl);
    
    if(err == KErrNone)
        {
        // This is a basic test for load positivity
        // from the Factory
        INFO_PRINTF1(_L("PASS: Dummy ECC Signer - Positive Object Load"));
        SetTestStepResult(EPass);
        }
    else
        {
        ERR_PRINTF2(_L("FAIL: Signer Object Construction Failure - %d"), err);
        }
    
    CleanupStack::PopAndDestroy(impl);
    CleanupStack::PopAndDestroy(keyPair);
    CleanupStack::PopAndDestroy(keypairImpl);
    CleanupStack::PopAndDestroy(keyParams);

    INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
    return TestStepResult();
	}



TVerdict CDummyEccSignerLoadStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}
