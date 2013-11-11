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
 
#include "plugincharsextendedstep.h"
#include "plugincharschecker.h"

#include <cryptospi/cryptosymmetriccipherapi.h>
#include <cryptospi/keys.h>
#include <cryptospi/plugincharacteristics.h>

using namespace CryptoSpi;

CPluginCharsExtendedStep::~CPluginCharsExtendedStep()
	{
	}

CPluginCharsExtendedStep::CPluginCharsExtendedStep()
	{
	SetTestStepName(KPluginCharsExtendedStep);
	}

TVerdict CPluginCharsExtendedStep::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}
				
TVerdict CPluginCharsExtendedStep::doTestStepL()
	{
		
	INFO_PRINTF1(_L("Plugin Characteristics - Extended Chracteristics"));
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
				
				//Retrieve all the Symmetric Cipher characteristics that are required for all test cases
				TInt exExtendedTestChar1;
				TInt exExtendedTestChar2;
				TPtrC exExtendedTestChar3;
				TVariantPtrC exExtendedConcurrency;
				TBool exExtendedExclusiveUse;
				
				if(!GetIntFromConfig(ConfigSection(),KConfigExExtendTestChar1,exExtendedTestChar1) ||
					!GetIntFromConfig(ConfigSection(),KConfigExExtendTestChar2,exExtendedTestChar2) ||
					!GetStringFromConfig(ConfigSection(),KConfigExExtendTestChar3,exExtendedTestChar3) ||
					!GetStringFromConfig(ConfigSection(),KConfigExExtendConcurrency,exExtendedConcurrency) ||
					!GetBoolFromConfig(ConfigSection(),KConfigExExtendExclusiveUse,exExtendedExclusiveUse))
					{
					ERR_PRINTF1(_L("** .INI Error: Expected Symmetric Cipher Extended Characteristics Not Located **"));
					SetTestStepResult(EFail);
					}
				else
					{
					INFO_PRINTF1(_L("** Checking Symmetric Cipher Extended Characteristics.... **"));
					
					//******************EXTENDED CHARACTERISITICS****************************
				
					//Retrieve Characteristics via CExtendedCharacteristics Object
					const CExtendedCharacteristics* extendedChars = NULL;
					
					TRAP_LOG(err,extendedChars = symmetricCipherImpl->GetExtendedCharacteristicsL());
					
					//Retrieve Charcteristics via the ListExtendedCharacteristics() of the CExtendedCharacteristics Object
					const CCryptoParams* extendedCryptoParams = NULL;

					TRAP_LOG(err,extendedCryptoParams = extendedChars->ListExtendedCharacteristics());
					
					//***********************************************************************

					//Convert expected string atrributes an 8 Bit Descriptor
					HBufC8* exTestChar3Str = HBufC8::NewLC(exExtendedTestChar3.Length());
					TPtr8 tmpPtr = exTestChar3Str->Des();
			
					tmpPtr.Copy(exExtendedTestChar3);
					
					//Checks BOTH the returned Extended Characteristic values from the CExtendedCharacteristics 'GET' Methods and
					//the CCryptoParams List returned from the ListExtendedCharacteristics() method

					if((!(extendedChars->GetTIntCharacteristicL(KExtendCharType1Uid) == exExtendedTestChar1)) 
						&& (!(extendedCryptoParams->GetTIntL(KExtendCharType1Uid) == exExtendedTestChar1)))
						{
						ERR_PRINTF1(_L("** FAIL: Extended Characteristic 1 Mismatch **"));	
						User::Leave(KErrNotFound);
						}
						
					if((!(extendedChars->GetTIntCharacteristicL(KExtendCharType2Uid) == exExtendedTestChar2))
						&& (!(extendedCryptoParams->GetTIntL(KExtendCharType2Uid) == exExtendedTestChar2)))
						{
						ERR_PRINTF1(_L("** FAIL: Extended Characteristic 2 Mismatch **"));	
						User::Leave(KErrNotFound);
						}
						
					if((!(extendedChars->GetTDesC8CharacteristicL(KExtendCharType3Uid) == *exTestChar3Str))
						&& (!(extendedCryptoParams->GetTDesC8L(KExtendCharType3Uid) == *exTestChar3Str)))
						{
						ERR_PRINTF1(_L("** FAIL: Extended Characteristic 3 Mismatch **"));	
						User::Leave(KErrNotFound);	
						}
						
					if((!(extendedChars->GetTIntCharacteristicL(KConcurrencyTypeUid) == exExtendedConcurrency))
						&& (!(extendedCryptoParams->GetTIntL(KConcurrencyTypeUid) == exExtendedConcurrency)))
						{
						ERR_PRINTF1(_L("** FAIL: 'Concurreny' Extended Characteristic Mismatch **"));	
						User::Leave(KErrNotFound);
						}
						
					if((!(extendedChars->GetTIntCharacteristicL(KExclusiveUseTypeUid) == exExtendedExclusiveUse))
						&& (!(extendedCryptoParams->GetTIntL(KExclusiveUseTypeUid) == exExtendedExclusiveUse)))
						{
						ERR_PRINTF1(_L("** FAIL: 'Exclusive Use' Extended Characteristic Mismatch **"));
						User::Leave(KErrNotFound);
						}
						
					INFO_PRINTF1(_L("** PASS: Extended Characteristics Successfully Match **"));	
					SetTestStepResult(EPass);
						
					CleanupStack::PopAndDestroy(exTestChar3Str);
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

TVerdict CPluginCharsExtendedStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}
