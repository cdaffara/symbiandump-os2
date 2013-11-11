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
#include "symmetric_mac_incremental_with_replicate_step.h"
#include <cryptospi/cryptomacapi.h>
#include <cryptospi/keys.h>
#include <cryptospi/plugincharacteristics.h>

using namespace CryptoSpi;

CSymmetricMacIncrementalWithReplicateStep::~CSymmetricMacIncrementalWithReplicateStep()
	{
	}


CSymmetricMacIncrementalWithReplicateStep::CSymmetricMacIncrementalWithReplicateStep()
	{
	SetTestStepName(KSymmetricMacIncrementalWithReplicateStep);
	}


TVerdict CSymmetricMacIncrementalWithReplicateStep::doTestStepPreambleL()
	{
	return EPass;
	}


TVerdict CSymmetricMacIncrementalWithReplicateStep::doTestStepL()
	{
	//Assume faliure, unless all is successful
	SetTestStepResult(EFail);
		
	INFO_PRINTF1(_L("*** Symmetric Mac - Incremental with Replicate ***"));
	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
		
	TPtrC keyPath;
	TPtrC sourcePath;
	TVariantPtrC algorithm;

	if(	!GetStringFromConfig(ConfigSection(),KConfigEncryptKeyPath, keyPath) ||
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
	
	//Create a pointer for the Hmac Implementation Object
	CMac* macImpl= NULL;
	
	//Retrieve a Mac Factory Object			
	TRAPD(err,CMacFactory::CreateMacL(macImpl,
										algorithm,
										*key,
										NULL));
				
	if (err != KErrNone)
		{
		CleanupStack::PopAndDestroy(3, keyParams); // keyParams, convertKey, key
		delete macImpl;
		ERR_PRINTF2(_L("*** FAIL: Failed to Create Symmetric Mac Object - %d ***"), err);
		return EFail;
		}
	
	INFO_PRINTF1(_L("Plugin loaded."));
	
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
	TBool macReplicated = EFalse;
	TPtrC8 macStr;
	
	CMac* macReplicateImpl = NULL;
	
	User::LeaveIfError(sourceFile.Size(sourceLength));
	
	//Divide the total size of the source file up into individual equal sized blocks to read
	//over several increments
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
		err = sourceFile.Read(readPosition,sourcePtr,readIncrement);
		
		HBufC8* convertSrc = ConvertFromHexFormatToRawL(*sourceData);
		CleanupStack::PopAndDestroy(sourceData);
		CleanupStack::PushL(convertSrc);
		
		//Update the read position by adding the number of bytes read
		readPosition += readIncrement;
		
		if(readPosition == readIncrement)
			{
			//Read in the first block from the data file into the Mac implementation object
			if(macReplicated == EFalse)
				{
				macImpl->MacL(*convertSrc);
				INFO_PRINTF2(_L("Intial Mac - Bytes Read: %d"), readPosition);
				}
			else
				{
				macReplicateImpl->MacL(*convertSrc);
				INFO_PRINTF2(_L("Intial Mac (Replicate) - Bytes Read: %d"), readPosition);	
				}
			CleanupStack::PopAndDestroy(convertSrc);
			}
		else if(readPosition >= sourceLength)
			{
			//Reading in the final block, constructs the complete hash value and returns it within a TPtrC8
			macStr.Set(macReplicateImpl->FinalL(*convertSrc));
			
			CleanupStack::PopAndDestroy(convertSrc);
			
			//Sets the Complete Flag to ETrue in order to drop out of the loop
			macComplete = ETrue;
			
			TInt totalRead = (readPosition - readIncrement) + (*sourceData).Length();
			INFO_PRINTF2(_L("Final Mac - Bytes Read: %d"),totalRead);
			}
		//If the read position is half the source length and the implementation
		//object hasn't already been replicated
		else if((readPosition >= sourceLength/2) && (macReplicated == EFalse))
			{
			INFO_PRINTF1(_L("Replicating Mac Object..."));
			macImpl->UpdateL(*convertSrc);
			
			CleanupStack::PopAndDestroy(convertSrc);
			
			//Create a Copy of the existing Mac Object with NO internal message state
			macReplicateImpl = macImpl->ReplicateL();
			
			macReplicated = ETrue;
			
			//Sets the read position back to 0 inorder to restart the file read from the beginning
			readPosition =0;
			CleanupStack::PushL(macReplicateImpl);
			INFO_PRINTF2(_L("*** Mac REPLICATE - Bytes Read: %d ***"), readPosition);
			}
		else
			{
			//Update the message data within the Mac object with the new block
			if(macReplicated == EFalse)
				{
				macImpl->UpdateL(*convertSrc);
				INFO_PRINTF2(_L("Mac Update - Bytes Read: %d"), readPosition);		
				}
			else
				{
				macReplicateImpl->UpdateL(*convertSrc);
				INFO_PRINTF2(_L("Mac Update (Replicate) - Bytes Read: %d"), readPosition);		
				}
			CleanupStack::PopAndDestroy(convertSrc);
			}					
		}while(macComplete == EFalse);

	//Create a NULL TCharacteristics pointer
	const TCharacteristics* charsPtr(NULL);
	
	//Retrieve the characteristics for the mac implementation object
	TRAP_LOG(err, macImpl->GetCharacteristicsL(charsPtr));
	
	//Static cast the characteristics to type TMacCharacteristics
	const TMacCharacteristics* macCharsPtr = static_cast<const TMacCharacteristics*>(charsPtr);
	
	//The mac output size is returned in Bits, divide by 8 to get the Byte size
	TInt macSize = macCharsPtr->iCipherAlgorithmChar->iBlockSize/8;
	HBufC8* macData = HBufC8::NewLC(macSize);
	TPtr8 macPtr = macData->Des();

	macPtr.Copy(macStr);

	//Check that expected data equals the encrypted data
	HBufC8* encryptedFileData = ReadInHexCiphertextL();
	CleanupStack::PushL(encryptedFileData);
			
	if(	!macPtr.Compare(TPtrC8(*encryptedFileData)))
		{
		INFO_PRINTF1(_L("*** Mac - Incremental with Replicate : PASS ***"));
		SetTestStepResult(EPass);	
		}
	else
		{
		ERR_PRINTF2(_L("*** FAIL: Mac Mismatch  ***"), err);
		}						

	CleanupStack::PopAndDestroy(9, keyParams);	// keyParams, convertKey, key, macImpl, &fsSession, &sourceFile, macReplicateImpl, macData, encryptedFileData
	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
	return TestStepResult();  
	}


TVerdict CSymmetricMacIncrementalWithReplicateStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}
