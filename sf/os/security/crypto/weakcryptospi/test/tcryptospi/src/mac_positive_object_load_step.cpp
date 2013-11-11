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
* Example CTestStep derived implementation
*
*/


/**
 @file
 @internalTechnology
*/
#include "mac_positive_object_load_step.h"

#include <cryptospi/cryptomacapi.h>
#include <cryptospi/keys.h>

using namespace CryptoSpi;

CMacPositiveObjectLoadStep::~CMacPositiveObjectLoadStep()
	{
	}


CMacPositiveObjectLoadStep::CMacPositiveObjectLoadStep()
	{
	SetTestStepName(KMacPositiveObjectLoadStep);
	}


TVerdict CMacPositiveObjectLoadStep::doTestStepPreambleL()
	{
	return EPass;
	}


TVerdict CMacPositiveObjectLoadStep::doTestStepL()
	{
	//Assume faliure, unless all is successful
	SetTestStepResult(EFail);
		
	INFO_PRINTF1(_L("*** Mac - Positive Object Load ***"));
	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
	
	TVariantPtrC algorithmUid;
	TPtrC encryptKey;
	TVariantPtrC keyType;
	
	//Extract the Test Case ID parameter from the specified INI file
	if (!GetStringFromConfig(ConfigSection(),KConfigAlgorithmUid,algorithmUid) ||
		!GetStringFromConfig(ConfigSection(),KConfigEncryptKey,encryptKey) ||
		!GetStringFromConfig(ConfigSection(),KConfigEncryptKeyType,keyType))
		{
		ERR_PRINTF1(_L("** Error: Failed to Load Configuration Parameters **"));
		return EFail;
		}

	//Create and initialise a pointer for the Mac + Key Implementation Object
	CMac* macImpl = NULL;

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
	
	//Retrieve a Mac Factory Object			
	TRAPD(err,CMacFactory::CreateMacL(macImpl,
										algorithmUid,
										*key,
										NULL));						
	
	//If there are no errors (KErrNone) creating the Mac factory object, Pass the Test	
	if (err != KErrNone)
		{
		CleanupStack::PopAndDestroy(3, keyStr);	// keyStr, keyParams, key
		delete macImpl;
		ERR_PRINTF2(_L("*** FAIL: Failed to Create Symmetric Mac Object %d ***"), err);
		User::Leave(err);
		}
	
	INFO_PRINTF1(_L("Plugin loaded."));

	CleanupStack::PushL(macImpl);
	
	//Create a NULL TCharacteristics pointer
	const TCharacteristics* charsPtr(NULL);
						
	//Retrieve the characteristics for the Mac implementation object
	TRAP_LOG(err, macImpl->GetCharacteristicsL(charsPtr));

	if(err == KErrNone)
		{
		//Static cast the characteristics to type TMacCharacteristics
		const TMacCharacteristics* macCharsPtr = static_cast<const TMacCharacteristics*>(charsPtr);
		const TCommonCharacteristics* commonChars = &macCharsPtr->iMacChar;
		
		if (commonChars->iAlgorithmUID == algorithmUid)
			{
			INFO_PRINTF1(_L("*** Mac - Positive Object Load : PASS ***"));
			SetTestStepResult(EPass);
			}
		else
			{
			INFO_PRINTF2(_L("*** FAIL: iAlgorithmUID mismatch 0x%x ***"), commonChars->iAlgorithmUID);
			}
		}
	else
		{
		ERR_PRINTF2(_L("*** FAIL: Failed to get Symmetric Mac Characteristics- %d ***"), err);
		}
	
	CleanupStack::PopAndDestroy(4, keyStr);	// keyStr, keyParams, key, macImpl
	
	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
	
	return TestStepResult();
	}


TVerdict CMacPositiveObjectLoadStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}
