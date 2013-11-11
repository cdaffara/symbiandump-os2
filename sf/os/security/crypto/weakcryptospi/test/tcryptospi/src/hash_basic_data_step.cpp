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
#include "hash_basic_data_step.h"
#include <cryptospi/cryptohashapi.h>
#include <cryptospi/plugincharacteristics.h>

using namespace CryptoSpi;


class CHashParam:public CBase
	{
public:
	~CHashParam();
	TUid iAlgorithmUid;
	CHash* iHash;
	};

CHashParam::~CHashParam()
	{
	delete iHash;
	}

static TInt CreatorThreadEntryPt(TAny* aParameters)
	{
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if (!cleanup)
		User::Exit(KErrNoMemory);
	
	ASSERT(aParameters);
	CHashParam* params=static_cast<CHashParam*>(aParameters);

	TRAPD(result,CHashFactory::CreateHashL(params->iHash,
											params->iAlgorithmUid,
											NULL));  					
	delete cleanup;
	return result;
	}


CHashBasicDataStep::~CHashBasicDataStep()
	{
	}


CHashBasicDataStep::CHashBasicDataStep()
	{
	SetTestStepName(KHashBasicDataStep);
	}


TVerdict CHashBasicDataStep::doTestStepPreambleL()
	{
	return EPass;
	}


TVerdict CHashBasicDataStep::doTestStepL()
	{
	//Assume faliure, unless all is successful
	SetTestStepResult(EFail);
		
	INFO_PRINTF1(_L("*** Hash - Basic Data ***"));
	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
	
	TVariantPtrC algorithmUid;
	TPtrC sourcePath;
	TPtrC expectedHash;
	
	//Extract the Test Case ID parameter from the specified INI file
	if(!GetStringFromConfig(ConfigSection(),KConfigAlgorithmUid,algorithmUid) ||
		!GetStringFromConfig(ConfigSection(),KConfigSourcePath,sourcePath) ||
		!GetStringFromConfig(ConfigSection(),KConfigExHashHmacValue,expectedHash))
		{
		ERR_PRINTF1(_L("** Error: Failed to Load Configuration Parameters **"));
		User::Leave(KErrNotFound);
		}
	
	TPtrC threadName;
	CHash* hashImpl=NULL;
	TInt err=KErrNone;
	CHashParam* params = new (ELeave) CHashParam();
	CleanupStack::PushL(params);
	
	if (GetStringFromConfig(ConfigSection(),KConfigThreadName,threadName))
		{
		RThread creatorThread;
		
		CleanupClosePushL(creatorThread);
		params->iAlgorithmUid=algorithmUid;
		params->iHash=NULL;

		User::LeaveIfError(creatorThread.Create(threadName, CreatorThreadEntryPt, KDefaultStackSize, NULL, (TAny*)params));
		TRequestStatus status=KRequestPending;
		creatorThread.Logon(status);
		creatorThread.Resume();
		User::WaitForRequest(status);
		hashImpl=params->iHash;
		err=status.Int();
		}
	else
		{
		//Retrieve a Hash Factory Object		
		TRAP(err,CHashFactory::CreateHashL(hashImpl,
											algorithmUid,
											NULL));
		}
	
	if(err == KErrNone)
		{
		RFs fsSession;
		
		//Create a connection to the file server	
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
			SetTestStepResult(EFail);
			}
		else
			{
			//Create a NULL TCharacteristics pointer
			const TCharacteristics* charsPtr(NULL);
			
			//Retrieve the characteristics for the hash implementation object
			TRAP_LOG(err, hashImpl->GetCharacteristicsL(charsPtr));
			
			//Static cast the characteristics to type THashCharacteristics
			const THashCharacteristics* hashCharsPtr = static_cast<const THashCharacteristics*>(charsPtr);
			
			//The hash output size is returned in Bits, divide by 8 to get the Byte size
			TInt hashSize = hashCharsPtr->iOutputSize/8;
			
			//Retrieve the final 8bit hash value and convert to 16bit								
			HBufC* hashData = HBufC::NewLC(hashSize);
			TPtr hashPtr = hashData->Des();
			
			// This is just a simple performance measurement on generating 
			// the hash
			TTime startTime;
			startTime.UniversalTime();
		
			//Copy the hashed content into the heap based descriptor
			hashPtr.Copy(hashImpl->Hash(*sourceData));
			
			TTime endTime;
			endTime.UniversalTime();
			TTimeIntervalMicroSeconds time = endTime.MicroSecondsFrom(startTime);

			//Take the 16bit descriptor and convert the string to hexadecimal
			TVariantPtrC convertHash;
			convertHash.Set(hashPtr);
			HBufC* hashResult = convertHash.HexStringLC();	
			
			INFO_PRINTF2(_L("*** Hashed Data: %S ***"),&*hashResult);
			INFO_PRINTF2(_L("*** Expected Hash: %S ***"),&expectedHash);
			
			//If the returned hash value matches the expected hash, Pass the test				
			if(*hashResult == expectedHash)
				{
			    INFO_PRINTF2(_L("*** Hash - Basic Data generated after setup in %d micro sec.: PASS ***"), time.Int64());
				SetTestStepResult(EPass);	
				}
			else
				{
				ERR_PRINTF2(_L("*** FAIL: Hashed and Expected Value Mismatch  ***"), err);
				SetTestStepResult(EFail);
				}
			
			CleanupStack::PopAndDestroy(3, sourceData); // hashResult, hashData, sourceData
			}

		CleanupStack::PopAndDestroy(2, &fsSession);	// &fsSession, sourceFile
		
		if (threadName.Ptr() != NULL)
			{
			CleanupStack::PopAndDestroy(); // &creatorThread
			}
		}
	else
		{
		ERR_PRINTF2(_L("*** FAIL: Failed to Create Hash Object - %d ***"), err);
		SetTestStepResult(EFail);	
		}
	
	CleanupStack::PopAndDestroy(); //params
	
	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());	
	
	return TestStepResult();
	}


TVerdict CHashBasicDataStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}
