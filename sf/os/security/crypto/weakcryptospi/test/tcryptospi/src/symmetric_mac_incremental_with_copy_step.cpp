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
#include "symmetric_mac_incremental_with_copy_step.h"
#include <cryptospi/cryptomacapi.h>
#include <cryptospi/keys.h>
#include <cryptospi/plugincharacteristics.h>

using namespace CryptoSpi;

CSymmetricMacIncrementalWithCopyStep::~CSymmetricMacIncrementalWithCopyStep()
	{
	}


CSymmetricMacIncrementalWithCopyStep::CSymmetricMacIncrementalWithCopyStep()
	{
	SetTestStepName(KSymmetricMacIncrementalWithCopyStep);
	}


TVerdict CSymmetricMacIncrementalWithCopyStep::doTestStepPreambleL()
	{
	return EPass;
	}


TVerdict CSymmetricMacIncrementalWithCopyStep::doTestStepL()
	{
	//Assume faliure, unless all is successful
	SetTestStepResult(EFail);
	
	INFO_PRINTF1(_L("*** Symmetric Mac - Incremental with Copy ***"));
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
	
	// Create a Symmetric Mac with the values from the ini file	
	CMac* macImpl = NULL;

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
				
	//Push the Hmac Implementation Object onto the Cleanup Stack
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
	TBool macCopied = EFalse;
	TPtrC8 macStr;
	CMac* macCopyImpl = NULL;
						
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
			macImpl->MacL(*convertSrc);
			CleanupStack::PopAndDestroy(convertSrc);
			INFO_PRINTF2(_L("Intial Mac - Bytes Read: %d"), readPosition);
			}
		else if(readPosition >= sourceLength)
			{
			//Reading in the final block, constructs the complete hash value and returns it within a TPtrC8
			macStr.Set(macCopyImpl->FinalL(*convertSrc));
			CleanupStack::PopAndDestroy(convertSrc);
			
			//Sets the Complete Flag to ETrue in order to drop out of the loop
			macComplete = ETrue;
								
			TInt totalRead = (readPosition - readIncrement) + (*sourceData).Length();
			INFO_PRINTF2(_L("Final Mac - Bytes Read: %d"),totalRead);
			}
		//If the read position is half the source length and the implementation
		//object hasn't already been copied
		else if((readPosition >= sourceLength/2) && (macCopied == EFalse))
			{
			//Update the hash message before copying
			macImpl->UpdateL(*convertSrc);
			CleanupStack::PopAndDestroy(convertSrc);
			
			INFO_PRINTF1(_L("Copying Mac Object..."));
			
			//Create a Copy of the existing Mac Object and all internal state of the message digest
			macCopyImpl = macImpl->CopyL();
			
			macCopied = ETrue;
			CleanupStack::PushL(macCopyImpl);
			INFO_PRINTF2(_L("*** Mac COPY - Bytes Read: %d ***"), readPosition);
			}
		else
			{
			//Update the message data within the Mac object with the new block
			if(macCopied == EFalse)
				{
				macImpl->UpdateL(*convertSrc);
				INFO_PRINTF2(_L("Mac Update - Bytes Read: %d"), readPosition);		
				}
			else
				{
				macCopyImpl->UpdateL(*convertSrc);
				INFO_PRINTF2(_L("Mac Update (Copy) - Bytes Read: %d"), readPosition);		
				}
			CleanupStack::PopAndDestroy(convertSrc);
			}					
		} while (macComplete == EFalse);
								
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
		INFO_PRINTF1(_L("*** Symmetric Mac - Incremental : PASS ***"));
		SetTestStepResult(EPass);	
		}
	else
		{
		ERR_PRINTF2(_L("*** FAIL: Generated Mac and Expected Mac Mismatch  ***"), err);
		}				

	CleanupStack::PopAndDestroy(9, keyParams);	// keyParams, convertKey, key, macImpl, &fsSession, &sourceFile, macCopyImpl, macData, encryptedFileData

	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
	return TestStepResult();
	}


TVerdict CSymmetricMacIncrementalWithCopyStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}
