/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "hashbasichashofdatastep.h"

#include <cryptospi/cryptohashapi.h>
#include <cryptospi/plugincharacteristics.h>

using namespace CryptoSpi;


class THashParameters
	{
public:
	~THashParameters();
	TUid iAlgorithmUid;
	TUid iOperationUid;
	CHash* iHash;
	};
	
THashParameters::~THashParameters()
	{
	delete iHash;
	}

TInt CreatorThreadEntryPoint(TAny* aParameters)
	{
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if (!cleanup)
		User::Exit(KErrNoMemory);
	
	ASSERT(aParameters);
	THashParameters* params=static_cast<THashParameters*>(aParameters);

	TRAPD(result,CHashFactory::CreateHashL(params->iHash,
											params->iAlgorithmUid,
											params->iOperationUid,
											NULL,
											NULL));  					
	delete cleanup;
	User::Exit(result);
	return KErrNone;
	}


CHashBasicHashOfDataStep::~CHashBasicHashOfDataStep()
	{
	}


CHashBasicHashOfDataStep::CHashBasicHashOfDataStep()
	{
	SetTestStepName(KHashBasicHashOfDataStep);
	}


TVerdict CHashBasicHashOfDataStep::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CHashBasicHashOfDataStep::doTestStepL()
	{
	if (TestStepResult()==EPass)
		{
		
		//Assume faliure, unless all is successful
		SetTestStepResult(EFail);
		
		INFO_PRINTF1(_L("*** Hash - Basic Hash of Data ***"));
		INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
		
		TVariantPtrC algorithmUid;
		TVariantPtrC operationModeUid;
		TPtrC sourcePath;
		TPtrC expectedHash;
		
		//Extract the Test Case ID parameter from the specified INI file
		if(!GetStringFromConfig(ConfigSection(),KConfigAlgorithmUid,algorithmUid) ||
			!GetStringFromConfig(ConfigSection(),KConfigOperationMode,operationModeUid) ||
			!GetStringFromConfig(ConfigSection(),KConfigSourcePath,sourcePath) ||
			!GetStringFromConfig(ConfigSection(),KConfigExHashHmacValue,expectedHash))
			{
			ERR_PRINTF1(_L("** Error: Failed to Load Configuration Parameters **"));
			SetTestStepResult(EFail);
			}
		else
			{
			TPtrC threadName;
			CHash* hashImpl=NULL;
			TInt err=KErrNone;
			THashParameters params;
			RThread creatorThread;
			
			CleanupClosePushL(creatorThread);
			
			if (GetStringFromConfig(ConfigSection(),KConfigThreadName,threadName))
				{
				
				params.iAlgorithmUid=algorithmUid;
				params.iOperationUid=operationModeUid;
				params.iHash=NULL;
		
				
				User::LeaveIfError(creatorThread.Create(threadName, CreatorThreadEntryPoint, KDefaultStackSize, NULL, (TAny*)&params));
				TRequestStatus status=KRequestPending;
				creatorThread.Logon(status);
				creatorThread.Resume();
				User::WaitForRequest(status);
				//creatorThread.Close();
				hashImpl=params.iHash;
				err=status.Int();
				}
			else
				{
				//Retrieve a Hash Factory Object						
				TRAP(err,CHashFactory::CreateHashL(hashImpl,
													algorithmUid,
													operationModeUid,
													NULL,
													NULL));
				params.iHash=hashImpl;
				}
				
			if(hashImpl && (err == KErrNone))
				{
				
				//Push the Hash Implementation Object onto the Cleanup Stack
				//CleanupStack::PushL(hashImpl);
				
				RFs fsSession;
				
				//Create a connection to the file server	
				err = fsSession.Connect();
					
				if(err != KErrNone)
					{
					ERR_PRINTF2(_L("*** Error: File Server Connection - %d ***"), err);
					SetTestStepResult(EFail);
					}	
				else
					{
					RFile sourceFile;
	    			CleanupClosePushL(sourceFile);
	    			
	    			//Open the specified source file		
	    			err = sourceFile.Open(fsSession,sourcePath, EFileRead);
	    					
	    			if(err != KErrNone)
						{
						ERR_PRINTF2(_L("*** Error: Opening Source File - %d ***"), err);
						SetTestStepResult(EFail);
						}
					else
						{
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
							
							//Copy the hashed content into the heap based descriptor
							hashPtr.Copy(hashImpl->Hash(*sourceData));
							
						 	//Take the 16bit descriptor and convert the string to hexadecimal
							TVariantPtrC convertHash;
							convertHash.Set(hashPtr);
							HBufC* hashResult = convertHash.HexStringLC();							
							
							INFO_PRINTF2(_L("*** Hashed Data: %S ***"),&*hashResult);
							INFO_PRINTF2(_L("*** Expected Hash: %S ***"),&expectedHash);
							
							//If the returned hash value matches the expected hash, Pass the test				
							if(*hashResult == expectedHash)
								{
								INFO_PRINTF1(_L("*** Hash - Basic Hash of Data : PASS ***"));
								SetTestStepResult(EPass);	
								}
							else
								{
								ERR_PRINTF2(_L("*** FAIL: Hashed and Expected Value Mismatch  ***"), err);
								SetTestStepResult(EFail);	
								}
							
							CleanupStack::PopAndDestroy(hashResult);
							CleanupStack::PopAndDestroy(hashData);
							CleanupStack::PopAndDestroy(sourceData);					
							}
						}
						
					//Cleanup the Source RFile	
					CleanupStack::PopAndDestroy();	
					}
					
				fsSession.Close();	
				
				//CleanupStack::PopAndDestroy(hashImpl);
				}
			else
				{
				ERR_PRINTF2(_L("*** FAIL: Failed to Create Hash Object - %d ***"), err);
				SetTestStepResult(EFail);	
				}
			
			CleanupStack::PopAndDestroy(&creatorThread);
			}
		}
		
	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());	
	
	return TestStepResult();
	}


TVerdict CHashBasicHashOfDataStep::doTestStepPostambleL()
	{
	
	return TestStepResult();
	}
