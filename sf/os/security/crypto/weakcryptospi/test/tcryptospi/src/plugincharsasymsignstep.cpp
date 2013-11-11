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
 
#include "plugincharsasymsignstep.h"
#include "plugincharschecker.h"

#include <cryptospi/keypair.h>
#include <cryptospi/cryptokeypairgeneratorapi.h>
#include <cryptospi/cryptosignatureapi.h>

using namespace CryptoSpi;

CPluginCharsAsymSignStep::~CPluginCharsAsymSignStep()
	{
	}

CPluginCharsAsymSignStep::CPluginCharsAsymSignStep()
	{
	SetTestStepName(KPluginCharsAsymSignStep);
	}

TVerdict CPluginCharsAsymSignStep::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}
				
TVerdict CPluginCharsAsymSignStep::doTestStepL()
	{
		
	INFO_PRINTF1(_L("Plugin Characteristics - Asymmetric Signer Chracteristics"));
	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
		
	if (TestStepResult()==EPass)
		{
		
		//Assume faliure, unless all is successful
		SetTestStepResult(EFail);
		
		TVariantPtrC algorithmUid;
		TVariantPtrC paddingMode;
		TVariantPtrC invalidPaddingMode;
		
		//Each of the individual parameters required to create the Asymmetric Singer object
		//are read in from the specified INI configuration file					
		if(!GetStringFromConfig(ConfigSection(),KConfigAlgorithmUid,algorithmUid) ||
			!GetStringFromConfig(ConfigSection(),KConfigPaddingMode,paddingMode) ||
			!GetStringFromConfig(ConfigSection(),KConfigInvalidPaddingMode,invalidPaddingMode))
			{
			ERR_PRINTF1(_L("** .INI Error: Asymmetric Signer Arguments Not Located **"));
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
			
			CSigner* asymmetricSignerImpl = NULL;	
				
			TRAP(err,CSignatureFactory::CreateSignerL(asymmetricSignerImpl,
														algorithmUid,
														keyPair->PublicKey(),
														paddingMode,
														NULL));
			
			if(asymmetricSignerImpl && (err == KErrNone))
				{
				
				CleanupStack::PushL(asymmetricSignerImpl);
				
				INFO_PRINTF1(_L("** Successfully Loaded Asymmetric Signer Object **"));
				
				//Define a pointer of type TCharacteristics in order to store the asymmetric signer
				//object's characterisctics	
				const TCharacteristics* chars(NULL);
							
				//Retrieve the characteristics for the asymmetric signer implementation object
				TRAP_LOG(err, asymmetricSignerImpl->GetCharacteristicsL(chars));
				
				//Static cast the characteristics to type TAsymmetricSignatureCharacteristics
				const TAsymmetricSignatureCharacteristics* asymsignChars = static_cast<const TAsymmetricSignatureCharacteristics*>(chars);

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
				
				//Retrieve all the Asymmetric Signer characteristics that are required for the test
				TInt exAsymSignMaxKeyLength;
				TInt exAsymSignKeySupportMode;
				TVariantPtrC exAsymSignPaddingModes;
				TInt exAsymSignPaddingModeNum;
			
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
					!GetIntFromConfig(ConfigSection(),KConfigExMaxKeyLength,exAsymSignMaxKeyLength) ||
					!GetStringFromConfig(ConfigSection(),KConfigExPaddingModes,exAsymSignPaddingModes) ||
					!GetIntFromConfig(ConfigSection(),KConfigExPaddingModeNum,exAsymSignPaddingModeNum) ||
					!GetIntFromConfig(ConfigSection(),KConfigExKeySupportMode,exAsymSignKeySupportMode))
					{
					ERR_PRINTF1(_L("** .INI Error: Expected Asymmetric Signer/Common Characteristics Not Located **"));
					SetTestStepResult(EFail);
					}
				else
					{
			
					INFO_PRINTF1(_L("** Checking Asymmetric Signer/Common Characteristics.... **"));
					
					CPluginCharsChecker* pluginCheck = CPluginCharsChecker::NewLC();
					
					//Retrieve the Common Characteristics from TAsymmetricSignatureCharacteristics 
					const TCommonCharacteristics* asymsignCommonChars = &asymsignChars->cmn;
					
					TPtrC errorMessage;
					
					//Perform Asymmetric Signer/Common Characteristic Checks
					if(pluginCheck->checkCommonCharacteristics(asymsignCommonChars,
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
					pluginCheck->checkAsymSignatureCharacteristics(asymsignChars,
																exAsymSignMaxKeyLength,
																exAsymSignPaddingModes,
																exAsymSignPaddingModeNum,
																exAsymSignKeySupportMode,
																errorMessage))
						{
						INFO_PRINTF1(_L("Asymmetric Signer/Common characteristics successfully match expected values..."));
						
						if(asymsignChars->IsPaddingModeSupported(paddingMode) && !asymsignChars->IsPaddingModeSupported(invalidPaddingMode))
							{
							INFO_PRINTF1(_L("Successful Padding Mode Supported Tests...."));
							
							INFO_PRINTF1(_L("** PASS: Asymmetric Signer/Common Characteristics Tests Successful **"));

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
				
				CleanupStack::PopAndDestroy(asymmetricSignerImpl);
				}
			else
				{
				ERR_PRINTF2(_L("*** FAIL: Failed to Create Asymmetric Signer Object - %d ***"), err);	
				}
				
			CleanupStack::PopAndDestroy(3,keyParams);
			}
		
		}
		
	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());	
	return TestStepResult();
	
	}

TVerdict CPluginCharsAsymSignStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}
