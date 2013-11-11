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
#include "hmacpositiveobjectloadstep.h"

#include <cryptospi/cryptohashapi.h>
#include <cryptospi/keys.h>

using namespace CryptoSpi;

CHmacPositiveObjectLoadStep::~CHmacPositiveObjectLoadStep()
	{
	}


CHmacPositiveObjectLoadStep::CHmacPositiveObjectLoadStep()
	{
	SetTestStepName(KHmacPositiveObjectLoadStep);
	}


TVerdict CHmacPositiveObjectLoadStep::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CHmacPositiveObjectLoadStep::doTestStepL()
	{
	if (TestStepResult()==EPass)
		{
		
		//Assume faliure, unless all is successful
		SetTestStepResult(EFail);
		
		INFO_PRINTF1(_L("*** Hmac - Positive Object Load ***"));
		INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
		
		TVariantPtrC algorithmUid;
		TVariantPtrC operationModeUid;
		TPtrC encryptKey;
		TVariantPtrC keyType;
		
		//Extract the Test Case ID parameter from the specified INI file
		if(!GetStringFromConfig(ConfigSection(),KConfigAlgorithmUid,algorithmUid) ||
			!GetStringFromConfig(ConfigSection(),KConfigOperationMode,operationModeUid) ||
			!GetStringFromConfig(ConfigSection(),KConfigEncryptKey,encryptKey) ||
			!GetStringFromConfig(ConfigSection(),KConfigEncryptKeyType,keyType))
			{
			ERR_PRINTF1(_L("** Error: Failed to Load Configuration Parameters **"));
			SetTestStepResult(EFail);
			}
		else
			{
			//Create and initialise a pointer for the Hash + Key (Hmac) Implementation Object
			CHash* hmacImpl = NULL;

			///Convert encryption key to an 8 Bit Descriptor
			HBufC8* keyStr = HBufC8::NewLC(encryptKey.Length());
			TPtr8 keyStrPtr = keyStr->Des();
			
			keyStrPtr.Copy(encryptKey);
									
			//Create an new CryptoParams object to encapsulate the key type and secret key string
			CCryptoParams* keyParams = CCryptoParams::NewL();
			CleanupStack::PushL(keyParams);
			keyParams->AddL(*keyStr,keyType);
			
			//Create the Key Object
			TKeyProperty keyProperty;			
			CKey* key=CKey::NewL(keyProperty,*keyParams);
			CleanupStack::PushL(key);
			
			//Retrieve a Hmac Factory Object			
			TRAPD(err,CHashFactory::CreateHashL(hmacImpl,
												algorithmUid,
												operationModeUid,
												key,
												NULL));						
			
			//If there are no errors (KErrNone) creating the Hmac factory object, Pass the Test	
			if(hmacImpl && (err == KErrNone))
				{
				INFO_PRINTF1(_L("*** Hmac - Positive Object Load : PASS ***"));
				delete hmacImpl;
				SetTestStepResult(EPass);	
				}
			else
				{
				ERR_PRINTF2(_L("*** FAIL: Failed to Create Hmac Object - %d ***"), err);
				SetTestStepResult(EFail);	
				}
			
			CleanupStack::PopAndDestroy(key);
			CleanupStack::PopAndDestroy(keyParams);	
			CleanupStack::PopAndDestroy(keyStr);
			}
		
		INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
		
		}
	return TestStepResult();
	}


TVerdict CHmacPositiveObjectLoadStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}
