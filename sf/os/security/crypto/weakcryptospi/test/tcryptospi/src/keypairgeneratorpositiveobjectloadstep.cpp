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
#include "keypairgeneratorpositiveobjectloadstep.h"


#include <cryptospi/keypair.h>
#include <cryptospi/cryptokeypairgeneratorapi.h>

using namespace CryptoSpi;


CKeyPairGeneratorPositiveObjectLoadStep::~CKeyPairGeneratorPositiveObjectLoadStep()
	{
	}

CKeyPairGeneratorPositiveObjectLoadStep::CKeyPairGeneratorPositiveObjectLoadStep()
	{
	SetTestStepName(KKeyPairGeneratorPositiveObjectLoadStep);
	}

TVerdict CKeyPairGeneratorPositiveObjectLoadStep::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CKeyPairGeneratorPositiveObjectLoadStep::doTestStepL()
	{
	INFO_PRINTF1(_L("*** Key Pair Generator - Positive Object Load ***"));
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
			keyParams->AddL(KKeyExponent, KRsaKeyParameterEUid);
			keyParams->AddL(KRsaPrivateKeyStandard, KRsaKeyTypeUid);
			
			INFO_PRINTF1(_L("Creating Key Pair Generator...")); 

			// create a key pair generator implementation interface
			CKeyPairGenerator* impl = NULL;	
			
			TRAPD(err,CKeyPairGeneratorFactory::CreateKeyPairGeneratorL(impl, 
															KRSAKeyPairGeneratorUid, 
															keyParams));
			
			if(impl && (err == KErrNone))
				{
				CleanupStack::PushL(impl);

				INFO_PRINTF1(_L("Generating Key Pair...")); 
				// Create a Key Pair with the values from the ini	
				CKeyPair* keyPair = NULL;
				
				TRAP(err,impl->GenerateKeyPairL(1024, 
											*keyParams, 
											keyPair));
				
				if(keyPair && (err == KErrNone))
					{
					delete keyPair;
					INFO_PRINTF1(_L("*** PASS: Key Pair Successfully Generated ***"));
					SetTestStepResult(EPass);	
					}
				else
					{
					ERR_PRINTF2(_L("*** FAIL: Key Pair Generation Failure - %d ***"), err);
					SetTestStepResult(EFail);	
					}
	
				CleanupStack::PopAndDestroy(impl);	
				}
			else
				{
				ERR_PRINTF2(_L("*** FAIL: Key Pair Generator Load Failure - %d ***"), err);
				SetTestStepResult(EFail);	
				}
			
			CleanupStack::PopAndDestroy(keyParams);
			}

		INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
		
		}
	  return TestStepResult();
	}



TVerdict CKeyPairGeneratorPositiveObjectLoadStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}
