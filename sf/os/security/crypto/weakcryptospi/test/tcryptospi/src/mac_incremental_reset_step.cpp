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
#include "mac_incremental_reset_step.h"

#include <cryptospi/cryptomacapi.h>
#include <cryptospi/keys.h>
#include <cryptospi/plugincharacteristics.h>

using namespace CryptoSpi;

CMacIncrementalResetStep::~CMacIncrementalResetStep()
	{
	}


CMacIncrementalResetStep::CMacIncrementalResetStep()
	{
	SetTestStepName(KMacIncrementalResetStep);
	}


TVerdict CMacIncrementalResetStep::doTestStepPreambleL()
	{
	return EPass;
	}


TVerdict CMacIncrementalResetStep::doTestStepL()
	{
	//Assume faliure, unless all is successful
	SetTestStepResult(EFail);
		
	INFO_PRINTF1(_L("*** Mac - Incremental with Reset ***"));
	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
	
	TVariantPtrC algorithmUid;
	TPtrC sourcePath;
	TPtrC expectedMac;
	TPtrC encryptKey;
	TVariantPtrC keyType;
	
	//Extract the Test Case ID parameter from the specified INI file
	if (!GetStringFromConfig(ConfigSection(),KConfigAlgorithmUid,algorithmUid) ||
		!GetStringFromConfig(ConfigSection(),KConfigSourcePath,sourcePath) ||
		!GetStringFromConfig(ConfigSection(),KConfigExMacValue,expectedMac) ||
		!GetStringFromConfig(ConfigSection(),KConfigEncryptKey,encryptKey) ||
		!GetStringFromConfig(ConfigSection(),KConfigEncryptKeyType,keyType))
		{
		ERR_PRINTF1(_L("** Error: Failed to Load Configuration Parameters **"));
		return EFail;
		}
	
	//Create a pointer for the Hmac Implementation Object
	CMac* macImpl= NULL;

	//Convert encryption key to an 8 Bit Descriptor
	HBufC8* keyStr = HBufC8::NewLC(encryptKey.Length());
	TPtr8 keyStrPtr = keyStr->Des();

	keyStrPtr.Copy(encryptKey);

	//Create an new CryptoParams object to encapsulate the key type and secret key string
	CCryptoParams* keyParams = CCryptoParams::NewL();
	CleanupStack::PushL(keyParams);
	keyParams->AddL(*keyStr, keyType);
		
	//Create Key Object
	TKeyProperty keyProperty;
	CKey* key=CKey::NewL(keyProperty,*keyParams);
	CleanupStack::PushL(key);
	
	//Retrieve a Mac Factory Object			
	TRAPD(err,CMacFactory::CreateMacL(macImpl,
										algorithmUid,
										*key,
										NULL));											

	if (err != KErrNone)
		{
		CleanupStack::PopAndDestroy(3, keyStr);	// keyStr, keyParams, key
		delete macImpl;
		ERR_PRINTF2(_L("*** FAIL: Failed to Create Mac Object - %d ***"), err);
		return EFail;
		}
	
	//Push the Mac Implementation Object onto the Cleanup Stack
	CleanupStack::PushL(macImpl);

	RFs fsSession;
	User::LeaveIfError(fsSession.Connect());
	CleanupClosePushL(fsSession);
		
	RFile sourceFile;
	CleanupClosePushL(sourceFile);
	
	//Open the specified source file
	User::LeaveIfError(sourceFile.Open(fsSession,sourcePath, EFileRead));

	TInt sourceLength = 0;
	TInt readPosition = 0;
	TInt readIncrement = 0;
	TBool macComplete = EFalse;
	TBool macReset = EFalse;
	TPtrC8 macStr;

	User::LeaveIfError(sourceFile.Size(sourceLength));

	//Divide the file size into seperate incremental blocks to read
	readIncrement = sourceLength/KDataReadBlocks;
	
	if (readIncrement == 0)
		{
		ERR_PRINTF2(_L("*** Error: Source File must be larger than %d bytes ***"), KDataReadBlocks);
		User::LeaveIfError(KErrNotSupported);
		}

	do
		{
		//Create a heap based descriptor to store the data
		HBufC8* sourceData = HBufC8::NewL(readIncrement);
		CleanupStack::PushL(sourceData);
		TPtr8 sourcePtr = sourceData->Des();

		//Read in a block of data from the source file from the current position
		err = sourceFile.Read(readPosition, sourcePtr, readIncrement);

		//Update the read position by adding the number of bytes read
		readPosition += readIncrement;

		if (readPosition == readIncrement)
			{
			//Read in the first block from the data file into the Mac implementation object
			macImpl->MacL(*sourceData);
			INFO_PRINTF2(_L("Intial Mac - Bytes Read: %d"), readPosition);
			}
		else if (readPosition >= sourceLength)
			{
			//Reading in the final block, constructs the complete hash value and returns it within a TPtrC8
			macStr.Set(macImpl->FinalL(*sourceData));

			//Sets the Complete Flag to ETrue in order to drop out of the loop
			macComplete = ETrue;

			TInt totalRead = (readPosition - readIncrement) + (*sourceData).Length();
			INFO_PRINTF2(_L("Final Mac - Bytes Read: %d"),totalRead);
			}
		//If the read position is half the source length and the implementation
		//object hasn't already been reset
		else if ((readPosition >= sourceLength/2) && (macReset	== EFalse))
			{
			INFO_PRINTF1(_L("Resetting Mac Object..."));

			macImpl->Reset();

			//Sets the read position back to 0 in order to restart the file read from the beginning
			readPosition = 0;

			macReset = ETrue;

			INFO_PRINTF2(_L("*** MAC RESET - Bytes Read: %d ***"), readPosition);
			}
		else
			{
			//Update the message data within the Mac object with the new block
			macImpl->UpdateL(*sourceData);
			INFO_PRINTF2(_L("Mac Update - Bytes Read: %d"), readPosition);
			}

		CleanupStack::PopAndDestroy(sourceData);

		} while (macComplete == EFalse);

	//Create a NULL TCharacteristics pointer
	const TCharacteristics* charsPtr(NULL);

	//Retrieve the characteristics for the mac implementation object
	TRAP_LOG(err, macImpl->GetCharacteristicsL(charsPtr));

	//Static cast the characteristics to type TMacCharacteristics
	const TMacCharacteristics* macCharsPtr = static_cast<const TMacCharacteristics*>(charsPtr);
	
	//The mac output size is returned in Bits, divide by 8 to get the Byte size
	 TInt macSize = macCharsPtr->iHashAlgorithmChar->iOutputSize/8;
						
	//Retrieve the final 8 bit Mac and convert to 16bit												
	HBufC* macData = HBufC::NewLC(macSize);
	TPtr macPtr = macData->Des();

	macPtr.Copy(macStr);

	//Take the 16 bit descriptor and convert the string to hexadecimal
	TVariantPtrC convertMac;
	convertMac.Set(macPtr);
	HBufC* macResult = convertMac.HexStringLC();

	INFO_PRINTF2(_L("*** Mac: %S ***"), &*macResult);
	INFO_PRINTF2(_L("*** Expected Mac: %S ***"), &expectedMac);

	//If the returned mac value matches the expected mac, Pass the test	
	if (*macResult == expectedMac)
		{
		INFO_PRINTF1(_L("*** Mac - Incremental with Reset : PASS ***"));
		SetTestStepResult(EPass);
		}
	else
		{
		ERR_PRINTF2(_L("*** FAIL: Generated Mac and Expected Mac Mismatch  ***"), err);
		}

	CleanupStack::PopAndDestroy(8, keyStr);	// keyStr, keyParams, key, macImpl, &fsSession, &sourceFile, macData, macResult

	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
	return TestStepResult();
	}


TVerdict CMacIncrementalResetStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}
