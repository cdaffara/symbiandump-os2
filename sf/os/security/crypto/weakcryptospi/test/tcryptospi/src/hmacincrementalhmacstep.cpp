/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "hmacincrementalhmacstep.h"

#include <cryptospi/cryptohashapi.h>
#include <cryptospi/keys.h>
#include <cryptospi/plugincharacteristics.h>

using namespace CryptoSpi;

CHmacIncrementalHmacStep::~CHmacIncrementalHmacStep()
	{
	}


CHmacIncrementalHmacStep::CHmacIncrementalHmacStep()
	{
	SetTestStepName(KHmacIncrementalHmacStep);
	}


TVerdict CHmacIncrementalHmacStep::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CHmacIncrementalHmacStep::doTestStepL()
	{
	if (TestStepResult()==EPass)
		{
		//Assume faliure, unless all is successful
		SetTestStepResult(EFail);
		
		INFO_PRINTF1(_L("*** Hmac - Incremental Hash ***"));
		INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
		
		TVariantPtrC algorithmUid;
		TVariantPtrC operationModeUid;
		TPtrC sourcePath;
		TPtrC expectedHash;
		TPtrC encryptKey;
		TVariantPtrC keyType;
		
		//Extract the Test Case ID parameter from the specified INI file
		if(!GetStringFromConfig(ConfigSection(),KConfigAlgorithmUid,algorithmUid) ||
			!GetStringFromConfig(ConfigSection(),KConfigOperationMode,operationModeUid) ||
			!GetStringFromConfig(ConfigSection(),KConfigSourcePath,sourcePath) ||
			!GetStringFromConfig(ConfigSection(),KConfigExHashHmacValue,expectedHash) ||
			!GetStringFromConfig(ConfigSection(),KConfigEncryptKey,encryptKey) ||
			!GetStringFromConfig(ConfigSection(),KConfigEncryptKeyType,keyType))
			{
			ERR_PRINTF1(_L("** Error: Failed to Load Configuration Parameters **"));
			SetTestStepResult(EFail);
			}
		else
			{
			//Create a pointer for the Hash + Key (Hamc) Implementation Object
			CHash* hmacImpl = NULL;
			
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
			
			//Retrieve a Hmac Factory Object		
			TRAPD(err,CHashFactory::CreateHashL(hmacImpl,
												algorithmUid,
												operationModeUid,
												key,
												NULL));										
	
			if(hmacImpl && (err == KErrNone))
				{
				
				//Push the Hmac Implementation Object onto the Cleanup Stack
				CleanupStack::PushL(hmacImpl);
				
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
						TPtrC8 hashStr;
						
						User::LeaveIfError(sourceFile.Size(sourceLength));
						
						//Divide the total size of the source file up into individual equal sized blocks to read
						//over several increments
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
								//Read in the first block from the data file into the Hmac implementation object
								hmacImpl->Hash(*sourceData);
								INFO_PRINTF2(_L("Intial Hmac - Bytes Read: %d"), readPosition);
								}
							else if(readPosition >= sourceLength)
								{
								//Reading in the final block, constructs the complete hash value and returns it within a TPtrC8
								hashStr.Set(hmacImpl->Final(*sourceData));
								
								//Sets the Complete Flag to ETrue in order to drop out of the loop
								hashComplete = ETrue;
								
								TInt totalRead = (readPosition - readIncrement) + (*sourceData).Length();
								INFO_PRINTF2(_L("Final Hmac - Bytes Read: %d"),totalRead);
								}
							else
								{
								//Update the message data within the Hmac object with the new block
								hmacImpl->Update(*sourceData);
								INFO_PRINTF2(_L("Hmac Update - Bytes Read: %d"), readPosition);
								}
							
							CleanupStack::PopAndDestroy(sourceData);
								
							}while(hashComplete == EFalse);
						
						//Create a NULL TCharacteristics pointer
						const TCharacteristics* charsPtr(NULL);
						
						//Retrieve the characteristics for the hash implementation object
						TRAP_LOG(err, hmacImpl->GetCharacteristicsL(charsPtr));
						
						//Static cast the characteristics to type THashCharacteristics
						const THashCharacteristics* hashCharsPtr = static_cast<const THashCharacteristics*>(charsPtr);
						
						//The hash output size is returned in Bits, divide by 8 to get the Byte size
						TInt hashSize = hashCharsPtr->iOutputSize/8;
						
						//Retrieve the final 8bit hash value and convert to 16bit												
						HBufC* hashData = HBufC::NewLC(hashSize);
						TPtr hashPtr = hashData->Des();
						
						hashPtr.Copy(hashStr);
						
						//Take the 16bit descriptor and convert the string to hexadecimal
						TVariantPtrC convertHash;
						convertHash.Set(hashPtr);
						HBufC* hmacResult = convertHash.HexStringLC();								
						
						INFO_PRINTF2(_L("*** Hashed Data: %S ***"),&*hmacResult);
						INFO_PRINTF2(_L("*** Expected Hash: %S ***"),&expectedHash);
							
						//If the returned hash value matches the expected hash, Pass the test	
						if(*hmacResult == expectedHash)
							{
							INFO_PRINTF1(_L("*** Hmac - Incremental Hash : PASS ***"));
							SetTestStepResult(EPass);	
							}
						else
							{
							ERR_PRINTF2(_L("*** FAIL: Hashed and Expected Value Mismatch  ***"), err);
							SetTestStepResult(EFail);	
							}				
							
						CleanupStack::PopAndDestroy(hmacResult);
						CleanupStack::PopAndDestroy(hashData);
						}
					
					//Cleanup the Source RFile	
					CleanupStack::PopAndDestroy();	
					}

				fsSession.Close();
				
				CleanupStack::PopAndDestroy(hmacImpl);	
				}
			else
				{
				ERR_PRINTF2(_L("*** FAIL: Failed to Create Hmac Object - %d ***"), err);
				SetTestStepResult(EFail);	
				}
				
			CleanupStack::PopAndDestroy(key);
			CleanupStack::PopAndDestroy(keyParams);
			CleanupStack::PopAndDestroy(keyStr);	
			}
		}
		
	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
	return TestStepResult();
	}


TVerdict CHmacIncrementalHmacStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}
