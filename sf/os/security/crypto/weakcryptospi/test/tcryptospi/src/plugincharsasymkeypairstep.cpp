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
 
#include "plugincharsasymkeypairstep.h"
#include "plugincharschecker.h"

#include <cryptospi/keypair.h>
#include <cryptospi/cryptokeypairgeneratorapi.h>

using namespace CryptoSpi;

CPluginCharsAsymKeyPairStep::~CPluginCharsAsymKeyPairStep()
	{
	}

CPluginCharsAsymKeyPairStep::CPluginCharsAsymKeyPairStep()
	{
	SetTestStepName(KPluginCharsAsymKeyPairStep);
	}

TVerdict CPluginCharsAsymKeyPairStep::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}
				
TVerdict CPluginCharsAsymKeyPairStep::doTestStepL()
	{
	
	INFO_PRINTF1(_L("Plugin Characteristics - Asymmetric Key Pair Generator Chracteristics"));
	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
		
	if (TestStepResult()==EPass)
		{
		
		//Assume faliure, unless all is successful
		SetTestStepResult(EFail);
		
		TVariantPtrC algorithmUid;
		
		//Each of the individual parameters required to create the Asymmetric Key Pair Generator object
		//are read in from the specified INI configuration file					
		if(!GetStringFromConfig(ConfigSection(),KConfigAlgorithmUid,algorithmUid))
			{
			ERR_PRINTF1(_L("** .INI Error: Asymmetric Key Pair Generator Cipher Arguments Not Located **"));
			SetTestStepResult(EFail);
			}
		else
			{
			CCryptoParams* keyParams = CCryptoParams::NewLC(); 	
			
			//Create Key Pair Generator Object
			CKeyPairGenerator* keyPairGenImpl = NULL;
			
			//Create a Asymmetric Key Pair Generator Implementation
			TRAPD(err,CKeyPairGeneratorFactory::CreateKeyPairGeneratorL(keyPairGenImpl, 
																		algorithmUid, 
																		keyParams));
																			
			if(keyPairGenImpl && (err == KErrNone))
				{
				
				CleanupStack::PushL(keyPairGenImpl);
				
				INFO_PRINTF1(_L("** Successfully Loaded Asymmetric Key Pair Generator Object **"));
				
				//Define a pointer of type TCharacteristics in order to store the asymmetric key pair generator
				//object's characterisctics	
				const TCharacteristics* chars(NULL);
							
				//Retrieve the characteristics for the asymmetric key pair generator implementation object
				keyPairGenImpl->GetCharacteristicsL(chars);
				
				//Static cast the characteristics to type TKeyAgreementCharacteristics
				const TAsymmetricKeypairGeneratorCharacteristics* asymkeypairgenChars = static_cast<const TAsymmetricKeypairGeneratorCharacteristics*>(chars);
				
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
				
				//Retrieve all the Asymmetric Key Pair Generator characteristics that are required for the test
				TInt exAsymKeyPairMaxKeyLength;
					
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
					!GetIntFromConfig(ConfigSection(),KConfigExMaxKeyLength,exAsymKeyPairMaxKeyLength))
					{
					ERR_PRINTF1(_L("** .INI Error: Expected Asymmetric Key Pair Generator/Common Characteristics Not Located **"));
					SetTestStepResult(EFail);
					}
				else
					{
			
					INFO_PRINTF1(_L("** Checking Asymmetric Key Pair Generator/Common Characteristics.... **"));
					
					CPluginCharsChecker* pluginCheck = CPluginCharsChecker::NewLC();
					
					//Retrieve the Common Characteristics from TAsymmetricKeypairGeneratorCharacteristics 
					const TCommonCharacteristics* asymkeypairgenCommonChars = &asymkeypairgenChars->cmn;
					
					TPtrC errorMessage;
					
					//Perform Key Agreement/Common Characteristic Checks
					if(pluginCheck->checkCommonCharacteristics(asymkeypairgenCommonChars,
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
					pluginCheck->checkAsymKeypairGenCharacteristics(asymkeypairgenChars,
																exAsymKeyPairMaxKeyLength,
																errorMessage))
						{
						INFO_PRINTF1(_L("** PASS : Asymmetric Key Pair Generator/Common characteristics successfully match expected values **"));
						SetTestStepResult(EPass);	
						}
					else
						{
						ERR_PRINTF2(_L("** FAIL: Characteristic Mismatch - %S **"),&errorMessage);	
						}
					
					CleanupStack::PopAndDestroy(pluginCheck);
					}
				
				CleanupStack::PopAndDestroy(keyPairGenImpl);
				}
			else
				{
				ERR_PRINTF2(_L("*** FAIL: Failed to Create Asymmetric Key Pair Generator Object - %d ***"), err);
				}
				
			CleanupStack::PopAndDestroy(keyParams);			
			}
		
		}
		
	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());	
	return TestStepResult();
	}

TVerdict CPluginCharsAsymKeyPairStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}
