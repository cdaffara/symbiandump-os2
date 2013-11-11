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
#include "pluginloadstep.h"
#include "plugincharschecker.h"

#include <cryptospi/cryptosymmetriccipherapi.h>
#include <cryptospi/keys.h>

#include <cryptospi/cryptospistateapi.h>
#include <cryptospi/cryptohashapi.h>
#include <cryptospi/cryptorandomapi.h>
#include <cryptospi/cryptosymmetriccipherapi.h>
#include <cryptospi/cryptoasymmetriccipherapi.h>
#include <cryptospi/cryptosignatureapi.h>
#include <cryptospi/cryptokeypairgeneratorapi.h>
#include <cryptospi/cryptokeyagreementapi.h>
#include <cryptospi/ruleselector.h>

using namespace CryptoSpi;


CPluginLoadStep::~CPluginLoadStep()
	{
	}


CPluginLoadStep::CPluginLoadStep()
	{
	SetTestStepName(KPluginLoadStep);
	}


TVerdict CPluginLoadStep::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);	
	return TestStepResult();
	}


TVerdict CPluginLoadStep::doTestStepL()
	{
	
	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
	
  	if (TestStepResult()==EPass)
		{
		//Assume faliure, unless all is successful
		SetTestStepResult(EFail);
		
		TPtrC encryptKey;
		TVariantPtrC keyType;
		TVariantPtrC algorithm;
		TVariantPtrC operationMode;
		TVariantPtrC paddingMode;
		TBool ruleSelectorToggle = EFalse;
		
		
		if(	!GetStringFromConfig(ConfigSection(),KConfigEncryptKey,encryptKey) || 
			!GetStringFromConfig(ConfigSection(),KConfigEncryptKeyType,keyType) ||
			!GetStringFromConfig(ConfigSection(),KConfigAlgorithmUid, algorithm) || 
			!GetStringFromConfig(ConfigSection(),KConfigOperationMode, operationMode) ||
			!GetStringFromConfig(ConfigSection(),KConfigPaddingMode, paddingMode ) ||
			!GetBoolFromConfig(ConfigSection(),KConfigRuleSelectorToggle, ruleSelectorToggle ))
			{
			ERR_PRINTF1(_L("** Error: Failed to Load DoStep() Configuration Parameters **"));
			User::Leave(KErrNotFound);
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
			
			//***** Determine whether to set the Rule Selector *****
			
			CRuleSelector* ruleSelector = NULL;
			
			if(ruleSelectorToggle)
				{
				//Create Rule Selection Rules Object
				CSelectionRules* rules = CSelectionRules::NewL();
				CleanupStack::PushL(rules);
				
				//Create Rule Selector Object	
				ruleSelector = CRuleSelector::NewL(rules);
				CleanupStack::Pop(rules);
				CleanupStack::PushL(ruleSelector);

				//Set the Selector Passing in a pointer to the Default Selector and SPI State	
				CCryptoSpiStateApi::SetSelector(ruleSelector);	
				}
				
			//******************************************************

			// Create a Symmetric Cipher with the values from the ini file
			CryptoSpi::CSymmetricCipher * impl = NULL;	
	
			TRAPD(err,CSymmetricCipherFactory::CreateSymmetricCipherL(impl,
															algorithm,
															*key,
															KCryptoModeEncryptUid,
															operationMode,
															paddingMode,
															NULL));

			if(impl && (err==KErrNone))
				{
				INFO_PRINTF1(_L("Successful Implementation Object Load..."));
				
				CleanupStack::PushL(impl);
				
				//Define a pointer of type TCharacteristics in order to store the appropriate
				//encryption object's characterisctics
				const TCharacteristics* characteristics(NULL);
					
				//Retrieve the characteristics for the symmetric cipher implementation object
				TRAP_LOG(err, impl->GetCharacteristicsL(characteristics));
				
				TVariantPtrC exAlgorithmUid;
				TVariantPtrC exImplementationUid;
						
				if(!GetStringFromConfig(ConfigSection(),KConfigExAlgorithmUid,exAlgorithmUid) ||
					!GetStringFromConfig(ConfigSection(),KConfigExImplementationUid,exImplementationUid))
					{
					ERR_PRINTF1(_L("** .INI Error: Expected Algorithm Arguments Not Located **"));
					SetTestStepResult(EFail);
					}
				else
					{
					INFO_PRINTF1(_L("Checking Plug-in Selection..."));
					
					CPluginCharsChecker* pluginCheck = CPluginCharsChecker::NewLC();
					
					TPtrC errorMessage;
					
					//Perform plug-in Check
					if(pluginCheck->checkSelectedPlugin(characteristics,
														exAlgorithmUid,
														exImplementationUid,
														errorMessage))
						{
						INFO_PRINTF1(_L("** PASS: Expected Plugin Loaded Successfully **"));
						SetTestStepResult(EPass);	
						}
					else
						{
						ERR_PRINTF2(_L("** FAIL: Unexpected Plugin Implementation Loaded - %S **"),&errorMessage);
						}
					
					CleanupStack::PopAndDestroy(pluginCheck);
				
					}
					
				CleanupStack::PopAndDestroy(impl);
				
				
				}
			else
				{
				ERR_PRINTF2(_L("*** FAIL: Implementation Object Load Failure ***"), err);
				SetTestStepResult(EFail);
				}
				
			if(ruleSelectorToggle)
				{
				//Set the Selector Passing in a pointer to the Default Selector and SPI State	
				CCryptoSpiStateApi::UnsetSelector();	
				
				CleanupStack::PopAndDestroy();
				}
			
			CleanupStack::PopAndDestroy(3,keyStr);	
			}
		
		}
	else
		{
		ERR_PRINTF1(_L("*** FAIL: Test Case Initialistion Failure ***"));	
		}
	
	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
	
	return TestStepResult();
	}


TVerdict CPluginLoadStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}
