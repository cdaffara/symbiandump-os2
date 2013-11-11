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
 
#include "plugincharsasymmetriccipherstep.h"
#include "plugincharschecker.h"

#include <cryptospi/cryptoasymmetriccipherapi.h>
#include <cryptospi/cryptokeypairgeneratorapi.h>
#include <cryptospi/keypair.h>

using namespace CryptoSpi;

CPluginCharsAsymmetricCipherStep::~CPluginCharsAsymmetricCipherStep()
	{
	}

CPluginCharsAsymmetricCipherStep::CPluginCharsAsymmetricCipherStep()
	{
	SetTestStepName(KPluginCharsAsymmetricCipherStep);
	}

TVerdict CPluginCharsAsymmetricCipherStep::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}
				
TVerdict CPluginCharsAsymmetricCipherStep::doTestStepL()
	{
	
	INFO_PRINTF1(_L("Plugin Characteristics - Asymmetric Cipher Chracteristics"));
	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
		
	if (TestStepResult()==EPass)
		{
		
		//Assume faliure, unless all is successful
		SetTestStepResult(EFail);
		
		TVariantPtrC algorithmUid;
		TVariantPtrC cryptoMode;
		TVariantPtrC paddingMode;
		TVariantPtrC invalidPaddingMode;
		
		//Each of the individual parameters required to create the Asymmetric Cipher object
		//are read in from the specified INI configuration file					
		if(!GetStringFromConfig(ConfigSection(),KConfigAlgorithmUid,algorithmUid) ||
			!GetStringFromConfig(ConfigSection(),KConfigCryptoMode,cryptoMode) ||
			!GetStringFromConfig(ConfigSection(),KConfigPaddingMode,paddingMode) ||
			!GetStringFromConfig(ConfigSection(),KConfigInvalidPaddingMode,invalidPaddingMode))
			{
			ERR_PRINTF1(_L("** .INI Error: Asymmetric Cipher Arguments Not Located **"));
			SetTestStepResult(EFail);
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
			TRAPD_LOG(err,CKeyPairGeneratorFactory::CreateKeyPairGeneratorL(keypairImpl, 
																			KRSAKeyPairGeneratorUid, 
																			keyParams));
											
			CleanupStack::PushL(keypairImpl);

			// Create a Key Pair	
			TRAP_LOG(err,keypairImpl->GenerateKeyPairL(1024, *keyParams, keyPair));
			
			CleanupStack::PushL(keyPair);
			
			//*****************************************************
			
			CAsymmetricCipher* asymmetricCipherImpl = NULL;	
				
			TRAP(err,CAsymmetricCipherFactory::CreateAsymmetricCipherL(asymmetricCipherImpl,
																	algorithmUid,
																	keyPair->PrivateKey(),
																	cryptoMode,
																	paddingMode,
																	NULL));
			
			if(asymmetricCipherImpl && (err == KErrNone))
				{
				
				CleanupStack::PushL(asymmetricCipherImpl);
				
				INFO_PRINTF1(_L("** Successfully Loaded Asymmetric Cipher Object **"));
				
				//Define a pointer of type TCharacteristics in order to store the asymmetric cipher
				//encryption object's characterisctics	
				const TCharacteristics* chars(NULL);
							
				//Retrieve the characteristics for the asymmetric cipher implementation object
				TRAP_LOG(err, asymmetricCipherImpl->GetCharacteristicsL(chars));
					
				//Static cast the characteristics to type TAsymmetricCipherCharacteristics
				const TAsymmetricCipherCharacteristics* asymmetricChars = static_cast<const TAsymmetricCipherCharacteristics*>(chars);
				
				//Retrieve all the Common characteristics that are required for all test cases
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
				
				//Retrieve all the Asymmetric characteristics that are required for all test cases
				TInt exAsymmetricMaxKeyLength;
				TVariantPtrC exAsymmetricPaddingModes;
				TInt exAsymmetricPaddingModeNum;
				TInt exAsymmetricKeySupportMode;
				
				if(!GetStringFromConfig(ConfigSection(),KConfigExInterfaceUid,exInterfaceUid) ||
					!GetStringFromConfig(ConfigSection(),KConfigExAlgorithmUid,exAlgorithmUid) ||
					!GetStringFromConfig(ConfigSection(),KConfigExImplementationUid,exImplementationUid) ||
					!GetStringFromConfig(ConfigSection(),KConfigExCreatorName,exCreatorName) ||
					!GetBoolFromConfig(ConfigSection(),KConfigExFIPSApproved,exFIPSApproved) ||
					!GetBoolFromConfig(ConfigSection(),KConfigExHardwareSupport,exHardwareSupported) ||
					!GetIntFromConfig(ConfigSection(),KConfigExMaxConcurrency,exMaxConcurrencySupported) ||
					!GetStringFromConfig(ConfigSection(),KConfigExAlgorithmName,exAlgorithmName) ||
					!GetIntFromConfig(ConfigSection(),KConfigExLatency,exLatency) ||
					!GetIntFromConfig(ConfigSection(),KConfigExThroughput,exThroughput) ||
					!GetIntFromConfig(ConfigSection(),KConfigExMaxKeyLength,exAsymmetricMaxKeyLength) ||
					!GetStringFromConfig(ConfigSection(),KConfigExPaddingModes,exAsymmetricPaddingModes) ||
					!GetIntFromConfig(ConfigSection(),KConfigExPaddingModeNum,exAsymmetricPaddingModeNum) ||
					!GetIntFromConfig(ConfigSection(),KConfigExKeySupportMode,exAsymmetricKeySupportMode))
					{
					ERR_PRINTF1(_L("** .INI Error: Expected Asymmetric Cipher/Common Chracteristic Arguments Not Located **"));
					SetTestStepResult(EFail);
					}
				else
					{
					INFO_PRINTF1(_L("** Checking Asymmetric Cipher/Common Characteristics.... **"));
					
					CPluginCharsChecker* pluginCheck = CPluginCharsChecker::NewLC();
					
					//Retrieve the Common Characteristics from TAsymmetricCipherCharacteristics 
					const TCommonCharacteristics* asymCommonChars = &asymmetricChars->cmn;
					
					TPtrC errorMessage;
					
					//Perform Asymmetric Cipher/Common Characteristic Checks
					if(pluginCheck->checkCommonCharacteristics(asymCommonChars,
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
																errorMessage) &&
						pluginCheck->checkAsymmetricCharacteristics(asymmetricChars,
																exAsymmetricMaxKeyLength,
																exAsymmetricPaddingModes,
																exAsymmetricPaddingModeNum,
																exAsymmetricKeySupportMode,
																errorMessage))
						{
						INFO_PRINTF1(_L("Asymmetric Cipher/Common characteristics successfully match expected values..."));
						
						if(asymmetricChars->IsPaddingModeSupported(paddingMode) && !asymmetricChars->IsPaddingModeSupported(invalidPaddingMode))
							{
							INFO_PRINTF1(_L("Successful Padding Mode Supported Tests...."));
							
							INFO_PRINTF1(_L("** PASS: Asymmetric Cipher/Common Characteristics Tests Successful **"));

							SetTestStepResult(EPass);
							}
						else
							{
							ERR_PRINTF1(_L("** FAIL: Unexpected 'Padding' Mode Supported Results"));	
							}	
						}
					else
						{
						ERR_PRINTF2(_L("** FAIL: Characteristic Mismatch - %S **"),&errorMessage);		
						}
						
					CleanupStack::PopAndDestroy(pluginCheck);
					}
					
				CleanupStack::PopAndDestroy(asymmetricCipherImpl);
				}				
			else
				{
				ERR_PRINTF1(_L("** Error: Loading Asymmetric Cipher Object **"));
				}
				
			CleanupStack::PopAndDestroy(3,keyParams);
			}
			
		}
		
	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());	
	return TestStepResult();
	}

TVerdict CPluginCharsAsymmetricCipherStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}
