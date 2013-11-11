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
#include "symmetric_mac_incremental_re_init_step.h"

#include <cryptospi/cryptomacapi.h>
#include <cryptospi/keys.h>
#include <cryptospi/plugincharacteristics.h>

using namespace CryptoSpi;

CSymmetricMacIncrementalReInitStep::~CSymmetricMacIncrementalReInitStep()
	{
	}


CSymmetricMacIncrementalReInitStep::CSymmetricMacIncrementalReInitStep()
	{
	SetTestStepName(KSymmetricMacIncrementalReInitStep);
	}


TVerdict CSymmetricMacIncrementalReInitStep::doTestStepPreambleL()
	{
	return EPass;
	}


TVerdict CSymmetricMacIncrementalReInitStep::doTestStepL()
	{
	//Assume faliure, unless all is successful
	SetTestStepResult(EFail);

	INFO_PRINTF1(_L("*** Symmetric Mac - Incremental with ReInitialiseAndSetKeyL ***"));
	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
	
	TPtrC keyPath;
	TPtrC tempKey;
	TPtrC sourcePath;
	TVariantPtrC algorithm;

	if(	!GetStringFromConfig(ConfigSection(),KConfigEncryptKeyPath, keyPath) ||
		!GetStringFromConfig(ConfigSection(),KConfigEncryptKey, tempKey) ||
		!GetStringFromConfig(ConfigSection(),KConfigAlgorithmUid, algorithm) ||
		!GetStringFromConfig(ConfigSection(),KConfigSourcePath, sourcePath))
		{
		User::Leave(KErrNotFound);
		}
	
	// Create key 
	TKeyProperty keyProperty;
	CCryptoParams* keyParams = CCryptoParams::NewLC();
	
	HBufC8* convertKey = ReadInHexPlainTextL(keyPath);
	CleanupStack::PushL(convertKey);
	
	keyParams->AddL(*convertKey, KSymmetricKeyParameterUid);
	
	CKey* key=CKey::NewL(keyProperty, *keyParams);
	CleanupStack::PushL(key);
	
	//Convert tempKey to an 8 Bit Descriptor
	HBufC8* tempKeyStr = HBufC8::NewLC(tempKey.Length());
	TPtr8 tempKeyStrPtr = tempKeyStr->Des();
	
	tempKeyStrPtr.Copy(tempKey);
	
	CCryptoParams* tempKeyParams = CCryptoParams::NewLC();
	tempKeyParams->AddL(*tempKeyStr, KSymmetricKeyParameterUid);
	
	CKey* tKey=CKey::NewL(keyProperty,*tempKeyParams);
	CleanupStack::PushL(tKey);
	
	//Create a pointer for the Hmac Implementation Object
	CMac* macImpl= NULL;
	
	//Retrieve a Mac Factory Object			
	TRAPD(err,CMacFactory::CreateMacL(macImpl,
										algorithm,
										*tKey,
										NULL));
				
	if (err != KErrNone)
		{
		CleanupStack::PopAndDestroy(6, keyParams); // keyParams, convertKey, key, tempKeyStr, tempKeyParams, tKey
		delete macImpl;
		ERR_PRINTF2(_L("*** FAIL: Failed to Create Symmetric Mac Object - %d ***"), err);
		return EFail;
		}
	
	INFO_PRINTF1(_L("Plugin loaded."));
	
	//Push the Mac Implementation Object onto the Cleanup Stack
	CleanupStack::PushL(macImpl);

	RFs fsSession;
	CleanupClosePushL(fsSession);
	
	//Create a connection to the file server	
	err = fsSession.Connect();

	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("*** Error: File Server Connection - %d ***"), err);
		}
	else
		{
		RFile sourceFile;
		CleanupClosePushL(sourceFile);

		//Open the specified source file
		err = sourceFile.Open(fsSession, sourcePath, EFileRead);

		if (err != KErrNone)
			{
			ERR_PRINTF2(_L("*** Error: Opening Source File - %d ***"), err);
			}
		else
			{
			TInt sourceLength = 0;
			TInt readPosition = 0;
			TInt readIncrement = 0;
			TBool macComplete = EFalse;
			TBool macReInit = EFalse;
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
				
				HBufC8* convertSrc = ConvertFromHexFormatToRawL(*sourceData);
				CleanupStack::PushL(convertSrc);
				
				//Update the read position by adding the number of bytes read
				readPosition += readIncrement;

				if (readPosition == readIncrement)
					{
					//Read in the first block from the data file into the Mac implementation object
					macImpl->MacL(*convertSrc);
					INFO_PRINTF2(_L("Intial Mac - Bytes Read: %d"), readPosition);
					}
				else if (readPosition >= sourceLength)
					{
					//Reading in the final block, constructs the complete hash value and returns it within a TPtrC8
					macStr.Set(macImpl->FinalL(*convertSrc));

					//Sets the Complete Flag to ETrue in order to drop out of the loop
					macComplete = ETrue;

					TInt totalRead = (readPosition - readIncrement) + (*sourceData).Length();
					INFO_PRINTF2(_L("Final Mac - Bytes Read: %d"),totalRead);
					}
				//If the read position is half the source length and the implementation
				//object hasn't already been re-init
				else if ((readPosition >= sourceLength/2) && (macReInit	== EFalse))
					{
					INFO_PRINTF1(_L("ReInit the Mac Object..."));
					
					//ReInit with the right Key to match the expected MAC
					macImpl->ReInitialiseAndSetKeyL(*key);

					//Sets the read position back to 0 in order to restart the file read from the beginning
					readPosition = 0;

					macReInit = ETrue;

					INFO_PRINTF2(_L("*** MAC REINIT - Bytes Read: %d ***"), readPosition);
					}
				else
					{
					//Update the message data within the Mac object with the new block
					macImpl->UpdateL(*convertSrc);
					INFO_PRINTF2(_L("Mac Update - Bytes Read: %d"), readPosition);
					}

				CleanupStack::PopAndDestroy(2, sourceData);	// sourceData, convertSrc

				} while (macComplete == EFalse);

			//Create a NULL TCharacteristics pointer
			const TCharacteristics* charsPtr(NULL);

			//Retrieve the characteristics for the mac implementation object
			macImpl->GetCharacteristicsL(charsPtr);

			//Static cast the characteristics to type TMacCharacteristics
			const TMacCharacteristics* macCharsPtr = static_cast<const TMacCharacteristics*>(charsPtr);

			//Create buffer for encrypted data
			TInt macSize = macCharsPtr->iCipherAlgorithmChar->iBlockSize/8;
			HBufC8* macData = HBufC8::NewLC(macSize);
			TPtr8 macPtr = macData->Des();

			macPtr.Copy(macStr);

			//Check that expected data equals the encrypted data
			HBufC8* encryptedFileData = ReadInHexCiphertextL();
			CleanupStack::PushL(encryptedFileData);
			
			if(	!macPtr.Compare(TPtrC8(*encryptedFileData)))
				{
				INFO_PRINTF1(_L("*** Mac - Incremental with ReInitialiseAndSetKeyL : PASS ***"));
				SetTestStepResult(EPass);
				}
			else
				{
				ERR_PRINTF1(_L("*** FAIL: Generated Mac and Expected Mac Mismatch  ***"));
				}

			CleanupStack::PopAndDestroy(2, macData);	// macData, encryptedFileData
			}

		//Cleanup the Source RFile	
		CleanupStack::PopAndDestroy(&sourceFile);
		}

	CleanupStack::PopAndDestroy(8, keyParams);	// keyParams, convertKey, key, tempKeyStr, tempKeyParams, tKey, macImpl, &fsSession
		
	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
	return TestStepResult();
	}


TVerdict CSymmetricMacIncrementalReInitStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}
