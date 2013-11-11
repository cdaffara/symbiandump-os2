/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 
#include "plugincharsmacstep.h"
#include "plugincharschecker.h"

#include <cryptospi/cryptomacapi.h>
#include <cryptospi/keys.h>

using namespace CryptoSpi;

CPluginCharsMacStep::~CPluginCharsMacStep()
	{
	}

CPluginCharsMacStep::CPluginCharsMacStep()
	{
	SetTestStepName(KPluginCharsMacStep);
	}

TVerdict CPluginCharsMacStep::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}
				
TVerdict CPluginCharsMacStep::doTestStepL()
	{
	
	INFO_PRINTF1(_L("Plugin Characteristics - Mac Chracteristics"));
	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
		
	if (TestStepResult()==EPass)
		{
		//Assume failure, unless all is successful
		SetTestStepResult(EFail);
		
		TVariantPtrC algorithmUid;
		TPtrC encryptKey;
		TVariantPtrC keyType;

		//Each of the individual parameters required to create the Mac object
		//are read in from the specified INI configuration file	
		if (!GetStringFromConfig(ConfigSection(),KConfigAlgorithmUid,algorithmUid) ||
			!GetStringFromConfig(ConfigSection(),KConfigEncryptKey,encryptKey) ||
			!GetStringFromConfig(ConfigSection(),KConfigEncryptKeyType,keyType))
			{
			ERR_PRINTF1(_L("** .INI Error: Mac Arguments Not Located **"));
			SetTestStepResult(EFail);
			}
		else
			{
			CMac* macImpl = NULL;
			
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

			TRAPD_LOG(err,CMacFactory::CreateMacL(macImpl,
													algorithmUid,
													*key,
													NULL));
			
			if (err != KErrNone)
				{
				delete macImpl;
				ERR_PRINTF2(_L("*** FAIL: Failed to Create Mac Object - %d ***"), err);
				User::Leave(err);
				}
			
			CleanupStack::PushL(macImpl);
			
			INFO_PRINTF1(_L("** Successfully Loaded Mac Object **"));

			//Define a pointer of type TCharacteristics in order to store the hash
			//cryptographic object's characterisctics	
			const TCharacteristics* chars(NULL);
			
			//Retrieve the characteristics for the hash implementation object
			TRAP_LOG(err, macImpl->GetCharacteristicsL(chars));
						
			//Static cast the characteristics to type TMacCharacteristics
			const TMacCharacteristics* macChars = static_cast<const TMacCharacteristics*>(chars);
			
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
			
			//Retrieve all the Mac characteristics that are required for all test cases
			TVariantPtrC exMacMode;
			
			if (!GetStringFromConfig(ConfigSection(),KConfigExInterfaceUid,exInterfaceUid) ||
				!GetStringFromConfig(ConfigSection(),KConfigExAlgorithmUid,exAlgorithmUid) ||
				!GetStringFromConfig(ConfigSection(),KConfigExImplementationUid,exImplementationUid) ||
				!GetStringFromConfig(ConfigSection(),KConfigExCreatorName,exCreatorName) ||
				!GetBoolFromConfig(ConfigSection(),KConfigExFIPSApproved,exFIPSApproved) ||
				!GetBoolFromConfig(ConfigSection(),KConfigExHardwareSupport,exHardwareSupported) ||
				!GetIntFromConfig(ConfigSection(),KConfigExMaxConcurrency,exMaxConcurrencySupported) ||
				!GetStringFromConfig(ConfigSection(),KConfigExAlgorithmName,exAlgorithmName) ||
				!GetIntFromConfig(ConfigSection(),KConfigExLatency,exLatency) ||
				!GetIntFromConfig(ConfigSection(),KConfigExThroughput,exThroughput) ||
				!GetStringFromConfig(ConfigSection(),KConfigExMacMode,exMacMode))
				{
				ERR_PRINTF1(_L("** .INI Error: Expected Mac/Common Characteristic Arguments Not Located **"));
				SetTestStepResult(EFail);
				}
			else
				{
				INFO_PRINTF1(_L("** Checking Mac/Common Characteristics.... **"));
				
				CPluginCharsChecker* pluginCheck = CPluginCharsChecker::NewLC();

				//Retrieve the Common Characteristics from TMacCharacteristics 
				const TCommonCharacteristics* macCommonChars = &macChars->iMacChar;
				
				TPtrC errorMessage;

				//Perform Mac/Common Characteristic Checks
				if(pluginCheck->checkCommonCharacteristics(macCommonChars,
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
					pluginCheck->checkMacCharacteristics(macChars, exMacMode, errorMessage))
					{
					INFO_PRINTF1(_L("Mac/Common characteristics successfully match expected values..."));
					SetTestStepResult(EPass);
					}
				else
					{
					ERR_PRINTF2(_L("** FAIL: Characteristic Mismatch - %S **"),&errorMessage);	
					}

				CleanupStack::PopAndDestroy(pluginCheck);
				}

			CleanupStack::PopAndDestroy(macImpl);
			CleanupStack::PopAndDestroy(key);
			CleanupStack::PopAndDestroy(keyParams);
			CleanupStack::PopAndDestroy(keyStr);
			}
		}
		
	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
	return TestStepResult();
	}

TVerdict CPluginCharsMacStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}
