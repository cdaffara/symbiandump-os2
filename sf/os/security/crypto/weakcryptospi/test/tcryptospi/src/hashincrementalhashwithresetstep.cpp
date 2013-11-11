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
#include "hashincrementalhashwithresetstep.h"

#include <cryptospi/cryptohashapi.h>
#include <cryptospi/plugincharacteristics.h>

using namespace CryptoSpi;

CHashIncrementalHashWithResetStep::~CHashIncrementalHashWithResetStep()
	{
	}


CHashIncrementalHashWithResetStep::CHashIncrementalHashWithResetStep()
	{
	SetTestStepName(KHashIncrementalHashWithResetStep);
	}


TVerdict CHashIncrementalHashWithResetStep::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CHashIncrementalHashWithResetStep::doTestStepL()
	{
	if (TestStepResult()==EPass)
		{
		
		//Assume faliure, unless all is successful
		SetTestStepResult(EFail);
		
		INFO_PRINTF1(_L("*** Hash - Incremental Hash with Reset ***"));
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
			//Create a pointer for the Hash Implementation Object
			CHash* hashImpl = NULL;
			
			//Retrieve a Hash Factory Object					
			TRAPD(err,CHashFactory::CreateHashL(hashImpl,
												algorithmUid,
												operationModeUid,
												NULL,
												NULL));  	
															
			if(hashImpl && (err == KErrNone))
				{
				
				//Push the Hash Implementation Object onto the Cleanup Stack
				CleanupStack::PushL(hashImpl);
				
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
						TInt readPosition = 0;
						TInt readIncrement = 0;
						TBool hashComplete = EFalse;
						TBool hashReset = EFalse;
						TPtrC8 hashStr;
						
						User::LeaveIfError(sourceFile.Size(sourceLength));
						
						//Divide the file size into seperate incremental blocks to read
						readIncrement = sourceLength/KDataReadBlocks;
						
						do 
							{
							//Create a heap based descriptor to store the data
							HBufC8* sourceData = HBufC8::NewL(readIncrement);
							CleanupStack::PushL(sourceData);
							TPtr8 sourcePtr = sourceData->Des();
							
							//Read in a block of data from the source file from the current position
							err = sourceFile.Read(readPosition,sourcePtr,readIncrement);
							
							//Update the read position by adding the number of bytes read
							readPosition += readIncrement;
							
							if(readPosition == readIncrement)
								{
								//Read in the first block from the data file into the Hash implementation object
								hashImpl->Hash(*sourceData);
								INFO_PRINTF2(_L("Intial Hash - Bytes Read: %d"), readPosition);
								}
							else if(readPosition >= sourceLength)
								{
								//Reading in the final block, constructs the complete hash value and returns it within a TPtrC8
								hashStr.Set(hashImpl->Final(*sourceData));
								
								//Sets the Complete Flag to ETrue in order to drop out of the loop
								hashComplete = ETrue;
								
								TInt totalRead = (readPosition - readIncrement) + (*sourceData).Length();
								INFO_PRINTF2(_L("Final Hash - Bytes Read: %d"),totalRead);
								}
							//If the read position is half the source length and the implementation
							//object hasn't already been reset
							else if((readPosition >= sourceLength/2) && (hashReset == EFalse))
								{
								INFO_PRINTF1(_L("Resetting Hash Object..."));
								
								hashImpl->Reset();
								
								//Sets the read position back to 0 inorder to restart the file read from the beginning
								readPosition = 0;
								
								hashReset = ETrue;
								
								INFO_PRINTF2(_L("*** HASH RESET - Bytes Read: %d ***"), readPosition);
								}
							else
								{
								//Update the message data within the Hash object with the new block
								hashImpl->Update(*sourceData);
								INFO_PRINTF2(_L("Hash Update - Bytes Read: %d"), readPosition);	
								}
							
							CleanupStack::PopAndDestroy(sourceData);
								
							}while(hashComplete == EFalse);
							
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
						hashPtr.Copy(hashStr);
						
					 	//Take the 16bit descriptor and convert the string to hexadecimal
						TVariantPtrC convertHash;
						convertHash.Set(hashPtr);
						HBufC* hashResult = convertHash.HexStringLC();
						
						INFO_PRINTF2(_L("*** Hashed Data: %S ***"),&*hashResult);
						INFO_PRINTF2(_L("*** Expected Hash: %S ***"),&expectedHash);
						
						//If the returned hash value matches the expected hash, Pass the test	
						if(*hashResult == expectedHash)
							{
							INFO_PRINTF1(_L("*** Hash - Incremental Hash with Reset : PASS ***"));
							SetTestStepResult(EPass);	
							}
						else
							{
							ERR_PRINTF2(_L("*** FAIL: Hashed and Expected Value Mismatch  ***"), err);
							SetTestStepResult(EFail);	
							}
							
						CleanupStack::PopAndDestroy(hashResult);
						CleanupStack::PopAndDestroy(hashData);						
						}
						
					//Cleanup the Source RFile	
					CleanupStack::PopAndDestroy();	
					}

				fsSession.Close();	
				
				CleanupStack::PopAndDestroy(hashImpl);
				}
			else
				{
				ERR_PRINTF2(_L("*** FAIL: Failed to Create Hash Object - %d ***"), err);
				SetTestStepResult(EFail);	
				}				
			}
		}
		
	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
	return TestStepResult();
	}


TVerdict CHashIncrementalHashWithResetStep::doTestStepPostambleL()
	{
	
	return TestStepResult();
	}
