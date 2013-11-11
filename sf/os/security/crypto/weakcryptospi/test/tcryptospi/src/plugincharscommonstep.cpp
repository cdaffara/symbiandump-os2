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
 
#include "plugincharscommonstep.h"
#include "plugincharschecker.h"

#include <cryptospi/cryptosymmetriccipherapi.h>
#include <cryptospi/keys.h>

using namespace CryptoSpi;

CPluginCharsCommonStep::~CPluginCharsCommonStep()
	{
	}

CPluginCharsCommonStep::CPluginCharsCommonStep()
	{
	SetTestStepName(KPluginCharsCommonStep);
	}

TVerdict CPluginCharsCommonStep::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}
				
TVerdict CPluginCharsCommonStep::doTestStepL()
	{
	
	INFO_PRINTF1(_L("Plugin Characteristics - Common Chracteristics"));
	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
		
	if (TestStepResult()==EPass)
		{
		
		//Assume faliure, unless all is successful
		SetTestStepResult(EFail);
		
		TPtrC encryptKey;
		TVariantPtrC keyType;
		TVariantPtrC algorithmUid;
		TVariantPtrC cryptoMode;
		TVariantPtrC operationMode;
		TVariantPtrC paddingMode;
		
		//Each of the individual parameters required to create the Symmetric Cipher object
		//are read in from the specified INI configuration file	
		if(!GetStringFromConfig(ConfigSection(),KConfigEncryptKey,encryptKey) || 
			!GetStringFromConfig(ConfigSection(),KConfigEncryptKeyType,keyType) ||
			!GetStringFromConfig(ConfigSection(),KConfigAlgorithmUid,algorithmUid) ||
			!GetStringFromConfig(ConfigSection(),KConfigCryptoMode,cryptoMode) ||
			!GetStringFromConfig(ConfigSection(),KConfigOperationMode,operationMode) ||
			!GetStringFromConfig(ConfigSection(),KConfigPaddingMode,paddingMode))
			{
			ERR_PRINTF1(_L("** .INI Error: Symmetric Cipher Arguments Not Located **"));
			SetTestStepResult(EFail);
			}
		else
			{						
			//Convert encryption key to an 8 Bit Descriptor
			HBufC8* keyStr = HBufC8::NewLC(encryptKey.Length());
			TPtr8 keyStrPtr = keyStr->Des();

			keyStrPtr.Copy(encryptKey);
			
			//Create an new CryptoParams object to encapsulate the key type and secret key string
			CCryptoParams* keyParams = CCryptoParams::NewL();
			CleanupStack::PushL(keyParams);
			keyParams->AddL(*keyStr,keyType);

			//Create Key Object
			TKeyProperty keyProperty;
			CKey* key=CKey::NewL(keyProperty,*keyParams);
			CleanupStack::PushL(key);
			
			CSymmetricCipher* symmetricCipherImpl = NULL;	
				
			TRAPD_LOG(err,CSymmetricCipherFactory::CreateSymmetricCipherL(symmetricCipherImpl,
																algorithmUid,
																*key,
																cryptoMode,
																operationMode,
																paddingMode,
																NULL)); 
			
			
						
			if(symmetricCipherImpl && (err == KErrNone))
				{
				
				CleanupStack::PushL(symmetricCipherImpl);
				
				INFO_PRINTF1(_L("** Successfully Loaded Symmetric Cipher Object **"));
				
				//Define a pointer of type TCharacteristics in order to store the symmetric cipher
				//encryption object's characterisctics	
				const TCharacteristics* chars(NULL);
							
				//Retrieve the characteristics for the symmetric cipher implementation object
				TRAP_LOG(err, symmetricCipherImpl->GetCharacteristicsL(chars));
				
				//Static cast the characteristics to type TCommonCharacteristics
				const TCommonCharacteristics* commonChars = static_cast<const TCommonCharacteristics*>(chars);

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
					ERR_PRINTF1(_L("** .INI Error: Expected Common Characteristic Arguments Not Located **"));
					SetTestStepResult(EFail);
					}
				else
					{
			
					INFO_PRINTF1(_L("** Checking common Characteristics.... **"));
					
					CPluginCharsChecker* pluginCheck = CPluginCharsChecker::NewLC();
					
					TPtrC errorMessage;
					
					//******************************************************************
					//Perform Characteristic Checks
					//******************************************************************
					if(pluginCheck->checkCommonCharacteristics(commonChars,
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
						INFO_PRINTF1(_L("** PASS : Common characteristics successfully match expected values **"));
						SetTestStepResult(EPass);	
						}
					else
						{
						ERR_PRINTF2(_L("** FAIL: Characteristic Mismatch - %S **"),&errorMessage);	
						}
						
					CleanupStack::PopAndDestroy(pluginCheck);
					}
				
				CleanupStack::PopAndDestroy(symmetricCipherImpl);
				}
			else
				{
				ERR_PRINTF2(_L("*** FAIL: Failed to Create Symmetric Cipher Object - %d ***"), err);	
				}
				
			CleanupStack::PopAndDestroy(3,keyStr);
			}
		}
						
		
	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());	
	return TestStepResult();
	}

TVerdict CPluginCharsCommonStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}
