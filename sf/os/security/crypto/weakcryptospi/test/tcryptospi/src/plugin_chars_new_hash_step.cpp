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
 
#include "plugin_chars_new_hash_step.h"
#include "plugincharschecker.h"
#include <cryptospi/cryptohashapi.h>
#include <cryptospi/keys.h>

using namespace CryptoSpi;

CPluginCharsNewHashStep::~CPluginCharsNewHashStep()
	{
	}

CPluginCharsNewHashStep::CPluginCharsNewHashStep()
	{
	SetTestStepName(KPluginCharsNewHashStep);
	}

TVerdict CPluginCharsNewHashStep::doTestStepPreambleL()
	{
	return EPass;
	}
				
TVerdict CPluginCharsNewHashStep::doTestStepL()
	{
	INFO_PRINTF1(_L("New Plugin Characteristics - Hash Chracteristics"));
	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
		
	//Assume faliure, unless all is successful
	SetTestStepResult(EFail);
	
	TVariantPtrC algorithmUid;
	TVariantPtrC invalidOperationMode;
	
	//Each of the individual parameters required to create the Hash object
	//are read in from the specified INI configuration file	
	if(!GetStringFromConfig(ConfigSection(),KConfigAlgorithmUid,algorithmUid) ||
		!GetStringFromConfig(ConfigSection(),KConfigInvalidOperationMode,invalidOperationMode))
		{
		ERR_PRINTF1(_L("** .INI Error: Hash Arguments Not Located **"));
		SetTestStepResult(EFail);
		}
	else
		{
		CHash* hashImpl = NULL;
		
		TRAPD_LOG(err,CHashFactory::CreateHashL(hashImpl,
												algorithmUid,
												NULL));
		
		if(hashImpl && (err == KErrNone))
			{
			CleanupStack::PushL(hashImpl);
			
			INFO_PRINTF1(_L("** Successfully Loaded Hash Object **"));
			
			//Define a pointer of type TCharacteristics in order to store the hash
			//cryptographic object's characterisctics	
			const TCharacteristics* chars(NULL);
			
			//Retrieve the characteristics for the hash implementation object
			TRAP_LOG(err, hashImpl->GetCharacteristicsL(chars));
						
			//Static cast the characteristics to type THashCharacteristics
			const THashCharacteristics* hashChars = static_cast<const THashCharacteristics*>(chars);
			
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
			
			//Retrieve all the Hash characteristics that are required for all test cases
			TInt exHashBlockSize;
			TInt exHashOutputSize;
			TVariantPtrC exHashOperationModes;
			TInt exHashOperationModeNum;
			
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
				!GetIntFromConfig(ConfigSection(),KConfigExBlockSize,exHashBlockSize) ||
				!GetIntFromConfig(ConfigSection(),KConfigExOutputSize,exHashOutputSize)  ||
				!GetStringFromConfig(ConfigSection(),KConfigExOperationModes,exHashOperationModes) ||
				!GetIntFromConfig(ConfigSection(),KConfigExOperationModeNum,exHashOperationModeNum))
				{
				ERR_PRINTF1(_L("** .INI Error: Expected Hash/Common Characteristic Arguments Not Located **"));
				SetTestStepResult(EFail);
				}
			else
				{
				INFO_PRINTF1(_L("** Checking Hash/Common Characteristics.... **"));
				
				CPluginCharsChecker* pluginCheck = CPluginCharsChecker::NewLC();
				
				//Retrieve the Common Characteristics from THashCharacteristics 
				const TCommonCharacteristics* hashCommonChars = &hashChars->cmn;
				
				TPtrC errorMessage;

				//Perform Hash/Common Characteristic Checks
				if(pluginCheck->checkCommonCharacteristics(hashCommonChars,
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
					pluginCheck->checkHashCharacteristics(hashChars,
											exHashBlockSize,
											exHashOutputSize,
											exHashOperationModes,
											exHashOperationModeNum,
											errorMessage))
					{
					INFO_PRINTF1(_L("Hash/Common characteristics successfully match expected values..."));
					
					if(hashChars->IsOperationModeSupported(KHashModeUid) && !hashChars->IsOperationModeSupported(invalidOperationMode))
						{
						
						INFO_PRINTF1(_L("Successful Operation Mode Supported Tests...."));
						
						INFO_PRINTF1(_L("** PASS: Hash/Common Characteristics Tests Successful **"));

						SetTestStepResult(EPass);
						}
					else
						{
						ERR_PRINTF1(_L("** FAIL: Unexpected 'Operation' Mode Supported Results"));
						}		
					}
				else
					{
					ERR_PRINTF2(_L("** FAIL: Characteristic Mismatch - %S **"),&errorMessage);	
					}

				CleanupStack::PopAndDestroy(pluginCheck);
				}
					
			CleanupStack::PopAndDestroy(hashImpl);	
			}
		else
			{
			ERR_PRINTF1(_L("** Error: Loading Hash Object **"));
			}
		}
		
	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());	
	return TestStepResult();
	}

TVerdict CPluginCharsNewHashStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}
