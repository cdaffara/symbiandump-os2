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
#include "mac_basic_data_step.h"

#include <cryptospi/cryptomacapi.h>
#include <cryptospi/keys.h>
#include <cryptospi/plugincharacteristics.h>

using namespace CryptoSpi;

CMacBasicDataStep::~CMacBasicDataStep()
	{
	}


CMacBasicDataStep::CMacBasicDataStep()
	{
	SetTestStepName(KMacBasicDataStep);
	}


TVerdict CMacBasicDataStep::doTestStepPreambleL()
	{
	return EPass;
	}


TVerdict CMacBasicDataStep::doTestStepL()
	{
	//Assume faliure, unless all is successful
	SetTestStepResult(EFail);
	
	TBool oOMCondition;
	
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

void CMacBasicDataStep::doTestL()
	{
	INFO_PRINTF1(_L("*** Mac - Basic Data ***"));
	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
	
	TVariantPtrC algorithmUid;
	TPtrC sourcePath;
	TPtrC expectedMac;
	TPtrC encryptKey;
	TVariantPtrC keyType;
	
	//Extract the Test Case ID parameter from the specified INI file
	if (!GetStringFromConfig(ConfigSection(),KConfigAlgorithmUid,algorithmUid) ||
		!GetStringFromConfig(ConfigSection(),KConfigSourcePath,sourcePath) ||
		!GetStringFromConfig(ConfigSection(),KConfigExMacValue,expectedMac)||
		!GetStringFromConfig(ConfigSection(),KConfigEncryptKey,encryptKey) ||
		!GetStringFromConfig(ConfigSection(),KConfigEncryptKeyType,keyType))
		{
		ERR_PRINTF1(_L("** Error: Failed to Load Configuration Parameters **"));
		return;
		}
	
	//Create a pointer for the Mac Implementation Object
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
	
	//Retrieve a Mac Factory Object			
	TRAPD(err, CMacFactory::CreateMacL(macImpl,
										algorithmUid,
										*key,
										NULL));											

	if (err != KErrNone)
		{
		CleanupStack::PopAndDestroy(3, keyStr);	// keyStr, keyParams, key
		delete macImpl;
		ERR_PRINTF2(_L("*** FAIL: Failed to Create Mac Object - %d ***"), err);
		User::Leave(err);
		}

	//Push the Mac Implementation Object onto the Cleanup Stack
	CleanupStack::PushL(macImpl);
	
	RFs fsSession;
	User::LeaveIfError(fsSession.Connect());
	CleanupClosePushL(fsSession);
		
	RFile sourceFile;
	
	//Open the specified source file
	User::LeaveIfError(sourceFile.Open(fsSession,sourcePath, EFileRead));
	CleanupClosePushL(sourceFile);
	
	TInt sourceLength = 0;
	User::LeaveIfError(sourceFile.Size(sourceLength));

	//Create a heap based descriptor to store the data
	HBufC8* sourceData = HBufC8::NewL(sourceLength);						
	CleanupStack::PushL(sourceData);
	TPtr8 sourcePtr = sourceData->Des();
	
	sourceFile.Read(sourcePtr);
	
	if(sourcePtr.Length() != sourceLength)
		{
		ERR_PRINTF1(_L("*** Error: Reading Source File ***"));
		}
	else
		{
		//Create a NULL TCharacteristics pointer
		const TCharacteristics* charsPtr(NULL);
		
		//Retrieve the characteristics for the Mac implementation object
		TRAP_LOG(err, macImpl->GetCharacteristicsL(charsPtr));
		
		//Static cast the characteristics to type TMacCharacteristics
		const TMacCharacteristics* macCharsPtr = static_cast<const TMacCharacteristics*>(charsPtr);
		
		//The mac output size is returned in Bits, divide by 8 to get the Byte size
	 	TInt macSize = macCharsPtr->iHashAlgorithmChar->iOutputSize/8;
		
		//Retrieve the final 8bit mac value and convert to 16bit												
		HBufC* macData = HBufC::NewLC(macSize);
		TPtr macPtr = macData->Des();
		
		// This is just a simple performance measurement on generating 
		// the Mac
		TTime startTime;
		startTime.UniversalTime();
		
		macPtr.Copy(macImpl->MacL(*sourceData));
		
		TTime endTime;
		endTime.UniversalTime();
		TTimeIntervalMicroSeconds time = endTime.MicroSecondsFrom(startTime);
		
		//Take the 16bit descriptor and convert the string to hexadecimal
		TVariantPtrC convertMac;
		convertMac.Set(macPtr);
		HBufC* macResult = convertMac.HexStringLC();		
		
		INFO_PRINTF2(_L("*** Mac: %S ***"), &*macResult);
		INFO_PRINTF2(_L("*** Expected Mac: %S ***"), &expectedMac);
		
		//If the returned mac value matches the expected mac, Pass the test	
		if(*macResult == expectedMac)
			{
			INFO_PRINTF2(_L("*** Mac - Basic Data generated after setup in %d micro sec.: PASS ***"), time.Int64());
			SetTestStepResult(EPass);	
			}
		else
			{
			ERR_PRINTF1(_L("*** FAIL: Generated Mac and Expected Mac Mismatch ***"));
			}
		
		CleanupStack::PopAndDestroy(2, macData);	// macData, macResult 	
		}

	CleanupStack::PopAndDestroy(7, keyStr);	// keyStr, keyParams, key, macImpl, &fsSession, &sourceFile, sourceData
	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
	}

void CMacBasicDataStep::doOomTestL()
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

TVerdict CMacBasicDataStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}
