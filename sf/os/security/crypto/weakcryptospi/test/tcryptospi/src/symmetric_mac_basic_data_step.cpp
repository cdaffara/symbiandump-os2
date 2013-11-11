/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "symmetric_mac_basic_data_step.h" 
#include <cryptospi/cryptomacapi.h>

using namespace CryptoSpi;


CSymmetricMacBasicDataStep::CSymmetricMacBasicDataStep()
	{
	}


CSymmetricMacBasicDataStep::~CSymmetricMacBasicDataStep()

	{
	}


TVerdict CSymmetricMacBasicDataStep::doTestStepPreambleL()
	{
	return EPass;
	}


TVerdict CSymmetricMacBasicDataStep::doTestStepL()
	{
	//Assume faliure, unless all is successful
	SetTestStepResult(EFail);

	TBool oOMCondition;
	
	// If OOMCondition has been set to true
	if (GetBoolFromConfig(ConfigSection(), _L("OOMCondition"),oOMCondition) && oOMCondition)
		{
		doOomTestL();
		}
	else
		{
		doTestL();
		}
	
	return TestStepResult();
	}

void CSymmetricMacBasicDataStep::doTestL()
	{
	INFO_PRINTF1(_L("*** Symmetric Mac - Basic Data ***"));
	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
  	
	TPtrC keyPath;
	TVariantPtrC algorithm;

	if(	!GetStringFromConfig(ConfigSection(),KConfigEncryptKeyPath, keyPath) ||
		!GetStringFromConfig(ConfigSection(),KConfigAlgorithmUid, algorithm))
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
		
		//Create buffer for encrypted data
		TInt maxOutputLength = macCharsPtr->iCipherAlgorithmChar->iBlockSize/8;
		HBufC8* encrypted =	HBufC8::NewLC(maxOutputLength);
		TPtr8 encryptedPtr = encrypted->Des();
		
		INFO_PRINTF1(_L("Generating Mac ..."));

		//read from src file
		HBufC8* srcData = ReadInPlaintextL();
		CleanupStack::PushL(srcData);
		
		HBufC8* convertSrc = ConvertFromHexFormatToRawL(*srcData);
		CleanupStack::PushL(convertSrc);
		
		// This is just a simple performance measurement on generating 
		// the Mac
		TTime startTime;
		startTime.UniversalTime();

		//Perform the encryption operation
		encryptedPtr.Copy(macImpl->MacL(*convertSrc));
		
		TTime endTime;
		endTime.UniversalTime();
		TTimeIntervalMicroSeconds time = endTime.MicroSecondsFrom(startTime);

		//Check that expected data equals the encrypted data
		HBufC8* encryptedFileData = ReadInHexCiphertextL();
		CleanupStack::PushL(encryptedFileData);
		
		if(	!encryptedPtr.Compare(TPtrC8(*encryptedFileData)) && (commonChars->iAlgorithmUID == algorithm))
			{
			INFO_PRINTF2(_L("PASS : Generated Symmetric MAC after setup in %d micro sec. matches Expected MAC"), time.Int64());
			SetTestStepResult(EPass);
			}
		else if ((commonChars->iAlgorithmUID == algorithm))
			{
			INFO_PRINTF1(_L("FAIL : Generated Symmetric MAC and Expected MAC Mismatch"));	
			}
		else
			{
			INFO_PRINTF2(_L("FAIL : iAlgorithmUID Characteristic Mismatch 0x%x"), commonChars->iAlgorithmUID);
			}

		CleanupStack::PopAndDestroy(4, encrypted);	// encrypted, srcData, convertSrc, encryptedFileData
		}
	else
		{
		ERR_PRINTF2(_L("*** FAIL: Failed to get Symmetric Mac Object Characteristics- %d ***"), err);
		}

	CleanupStack::PopAndDestroy(4, keyParams);	// keyParams, convertKey, key, macImpl
	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
	}

void CSymmetricMacBasicDataStep::doOomTestL()
	{
	TVerdict verdict = EFail;
	TInt countAfter = 0;
	TInt countBefore = 0;
	
	for (TInt oomCount = 0; ; oomCount++)
		{
		INFO_PRINTF2(_L("==== Number of memory allocations %d ===="), oomCount);

		verdict = EFail;
		__UHEAP_RESET;
		__UHEAP_SETFAIL(RHeap::EDeterministic, oomCount);
		countBefore = User::CountAllocCells();

		TRAPD(error, doTestL());// ----> This is the actual test that runs under OOM conditions.

		countAfter = User::CountAllocCells();

		if(countBefore != countAfter)
			{
			INFO_PRINTF3(_L("Heap alloc count: %d final vs %d initial"), countAfter,countBefore);
			}  

		__UHEAP_RESET;

		if(error == KErrNone)  // First posibility: Test sequence was able to run to completion
			{
			verdict = EPass;
			INFO_PRINTF1(_L("OOM Test sequence completed"));

			if(countBefore != countAfter)
				{
				// Memory has to balance.
				verdict = EFail;
				INFO_PRINTF1(_L("Memory did not balance. Test outcome : Fail"));
				break;
				}  
			break;
			} 
		// Second possibility: Test sequence was NOT completed and error other than KErrNoMemory was returned. 
		else if (error != KErrNoMemory)  
			{
			if(countBefore != countAfter)
				{
				// For any error returned memory has to balance.
				verdict = EFail;
				INFO_PRINTF1(_L("Memory did not balance. Test outcome : Fail"));
				break;
				}  
			}
		// Third possibility: Test sequence was NOT completed and error KErrNoMemory was returned. 
		else   
			{
			if (countBefore != countAfter)
				{
				verdict = EFail;
				INFO_PRINTF2(_L("OOM Status %d"),error);
				INFO_PRINTF2(_L("MEMORY DID NOT BALANCE!!. OOM Failed at %d"), oomCount);
				break;  
				}   
			}  
		INFO_PRINTF2(_L("OOM Failed Point status %d"), error);
		}  // End of for loop.
	
	SetTestStepResult(verdict);
	return;	
	}

TVerdict CSymmetricMacBasicDataStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}
