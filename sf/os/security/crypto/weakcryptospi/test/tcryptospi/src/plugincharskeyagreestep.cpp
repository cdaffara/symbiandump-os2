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
*
*/


/**
 @file
 @internalTechnology
*/
 
#include "plugincharskeyagreestep.h"
#include "plugincharschecker.h"

#include <cryptospi/keypair.h>
#include <cryptospi/cryptokeypairgeneratorapi.h>
#include <cryptospi/cryptokeyagreementapi.h>

using namespace CryptoSpi;

CPluginCharsKeyAgreeStep::~CPluginCharsKeyAgreeStep()
	{
	}

CPluginCharsKeyAgreeStep::CPluginCharsKeyAgreeStep()
	{
	SetTestStepName(KPluginCharsKeyAgreeStep);
	}

TVerdict CPluginCharsKeyAgreeStep::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}
				
TVerdict CPluginCharsKeyAgreeStep::doTestStepL()
	{
	
	INFO_PRINTF1(_L("Plugin Characteristics - Key Agreement Chracteristics"));
	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
		
	if (TestStepResult()==EPass)
		{
		
		//Assume faliure, unless all is successful
		SetTestStepResult(EFail);
		
		TVariantPtrC algorithmUid;
		
		//Each of the individual parameters required to create the Key Agreement object
		//are read in from the specified INI configuration file					
		if(!GetStringFromConfig(ConfigSection(),KConfigAlgorithmUid,algorithmUid))
			{
			ERR_PRINTF1(_L("** .INI Error: Key Agreement Object Arguments Not Located **"));
			SetTestStepResult(EFail);
			}
		else
			{
			INFO_PRINTF1(_L("Creating Primes and Base Integers...")); 
	 		 
			RInteger DH_N = RInteger::NewPrimeL(64);
			CleanupClosePushL(DH_N);
			
			RInteger DH_N_MinusOne = RInteger::NewL(DH_N);
			CleanupClosePushL(DH_N_MinusOne);
			DH_N_MinusOne-=1;
			
			RInteger DH_G = RInteger::NewRandomL(TInteger::Two(), DH_N_MinusOne);	
			CleanupClosePushL(DH_G);
			
			CCryptoParams* keyParams = CCryptoParams::NewLC(); 
						
			TRAPD_LOG(err,keyParams->AddL(DH_N, KDhKeyParameterNUid));
			TRAP_LOG(err,keyParams->AddL(DH_G, KDhKeyParameterGUid));
			
			//****************************************************
			//Create Key Pair and Key Pair Generator Objects
   			CKeyPair* keyPair = NULL;
			CKeyPairGenerator * keypairImpl = NULL;
			
			INFO_PRINTF1(_L("Generating Key Pair..."));
			
			// create a key pair generator implementation interface
			TRAP_LOG(err,CKeyPairGeneratorFactory::CreateKeyPairGeneratorL(keypairImpl, 
																			KDHKeyPairGeneratorUid, 
																			keyParams));								
			CleanupStack::PushL(keypairImpl);

			// Create a Key Pair	
			TRAP_LOG(err,keypairImpl->GenerateKeyPairL(NULL, *keyParams, keyPair));
			
			CleanupStack::PushL(keyPair);
			
			//*****************************************************
			
			CKeyAgreement* keyAgreementImpl = NULL;	
				
			TRAP(err,CKeyAgreementFactory::CreateKeyAgreementL(keyAgreementImpl,
																algorithmUid,
																keyPair->PrivateKey(),
																keyParams));
			
			if(keyAgreementImpl && (err == KErrNone))
				{
				
				CleanupStack::PushL(keyAgreementImpl);
				
				INFO_PRINTF1(_L("** Successfully Loaded Key Agreement Object **"));
				
				//Define a pointer of type TCharacteristics in order to store the key agreement
				//object's characterisctics	
				const TCharacteristics* chars(NULL);
							
				//Retrieve the characteristics for the key agreement signer implementation object
				TRAP_LOG(err, keyAgreementImpl->GetCharacteristicsL(chars));
				
				//Static cast the characteristics to type TKeyAgreementCharacteristics
				const TKeyAgreementCharacteristics* keyagreeChars = static_cast<const TKeyAgreementCharacteristics*>(chars);

				//Retrieve all the Common characteristics that are required for the test
				TVariantPtrC exInterfaceUid;
				TVariantPtrC exAlgorithmUid;
				TVariantPtrC exImplementationUid;
				TVariantPtrC exCreatorName;
				TBool exFIPSApproved;
				TBool exHardwareSupported;
				TInt exMaxConcurrencySupported;
				TVariantPtrC exAlgorithmName;
				TInt exLatency;
				TInt exThroughput;
				
				if(!GetStringFromConfig(ConfigSection(),KConfigExInterfaceUid,exInterfaceUid) ||
					!GetStringFromConfig(ConfigSection(),KConfigExAlgorithmUid,exAlgorithmUid) ||
					!GetStringFromConfig(ConfigSection(),KConfigExImplementationUid,exImplementationUid) ||
					!GetStringFromConfig(ConfigSection(),KConfigExCreatorName,exCreatorName) ||
					!GetBoolFromConfig(ConfigSection(),KConfigExFIPSApproved,exFIPSApproved) ||
					!GetBoolFromConfig(ConfigSection(),KConfigExHardwareSupport,exHardwareSupported) ||
					!GetIntFromConfig(ConfigSection(),KConfigExMaxConcurrency,exMaxConcurrencySupported) ||
					!GetStringFromConfig(ConfigSection(),KConfigExAlgorithmName,exAlgorithmName) ||
					!GetIntFromConfig(ConfigSection(),KConfigExLatency,exLatency) ||
					!GetIntFromConfig(ConfigSection(),KConfigExThroughput,exThroughput))
					{
					ERR_PRINTF1(_L("** .INI Error: Expected Key Agreement/Common Characteristics Not Located **"));
					SetTestStepResult(EFail);
					}
				else
					{
			
					INFO_PRINTF1(_L("** Checking Key Agreement/Common Characteristics.... **"));
					
					CPluginCharsChecker* pluginCheck = CPluginCharsChecker::NewLC();
					
					//Retrieve the Common Characteristics from TKeyAgreementCharacteristics 
					const TCommonCharacteristics* keyagreeCommonChars = &keyagreeChars->cmn;
					
					TPtrC errorMessage;
					
					//Perform Key Agreement/Common Characteristic Checks
					if(pluginCheck->checkCommonCharacteristics(keyagreeCommonChars,
																exInterfaceUid,
																exAlgorithmUid,
																exImplementationUid,
																exCreatorName,
																exFIPSApproved,
																exHardwareSupported,
																exMaxConcurrencySupported,
																exAlgorithmName,
																exLatency,
																exThroughput,
																errorMessage))
						{
						INFO_PRINTF1(_L("** PASS : Key Agreement/Common characteristics successfully match expected values **"));
						SetTestStepResult(EPass);	
						}
					else
						{
						ERR_PRINTF2(_L("** FAIL: Characteristic Mismatch - %S **"),&errorMessage);
						}
					
					CleanupStack::PopAndDestroy(pluginCheck);
					}
				
				CleanupStack::PopAndDestroy(keyAgreementImpl);
				}
			else
				{
				ERR_PRINTF2(_L("*** FAIL: Failed to Create Key Agreement Object - %d ***"), err);	
				}
				
			CleanupStack::PopAndDestroy(6,&DH_N);
			}
		
		}
		
	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());	
	return TestStepResult();
	}

TVerdict CPluginCharsKeyAgreeStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}
