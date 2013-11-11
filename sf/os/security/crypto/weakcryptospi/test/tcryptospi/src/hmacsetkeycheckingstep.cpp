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
#include "hmacsetkeycheckingstep.h"

#include <cryptospi/cryptohashapi.h>
#include <cryptospi/keys.h>
#include <cryptospi/plugincharacteristics.h>

using namespace CryptoSpi;

CHmacSetKeyCheckingStep::~CHmacSetKeyCheckingStep()
	{
	}


CHmacSetKeyCheckingStep::CHmacSetKeyCheckingStep()
	{
	SetTestStepName(KHmacSetKeyCheckingStep);
	}


TVerdict CHmacSetKeyCheckingStep::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CHmacSetKeyCheckingStep::doTestStepL()
	{
	if (TestStepResult()==EPass)
		{
		
		//Assume faliure, unless all is successful
		SetTestStepResult(EFail);
		
		INFO_PRINTF1(_L("*** Hmac - Set Key Checking ***"));
		INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
		
		TVariantPtrC algorithmUid;
		TVariantPtrC operationModeUid;
		TPtrC sourcePath;
		TPtrC expectedHash;
		TPtrC invalidEncryptKey;
		TPtrC encryptKey;
		TVariantPtrC keyType;
		
		//Extract the Test Case ID parameter from the specified INI file
		if(!GetStringFromConfig(ConfigSection(),KConfigAlgorithmUid,algorithmUid) ||
			!GetStringFromConfig(ConfigSection(),KConfigOperationMode,operationModeUid) ||
			!GetStringFromConfig(ConfigSection(),KConfigSourcePath,sourcePath) ||
			!GetStringFromConfig(ConfigSection(),KConfigExHashHmacValue,expectedHash) ||
			!GetStringFromConfig(ConfigSection(),KConfigInvalidKey,invalidEncryptKey) ||
			!GetStringFromConfig(ConfigSection(),KConfigEncryptKey,encryptKey) ||
			!GetStringFromConfig(ConfigSection(),KConfigEncryptKeyType,keyType))
			{
			ERR_PRINTF1(_L("** Error: Failed to Load Configuration Parameters **"));
			SetTestStepResult(EFail);
			}
		else
			{	
			RFs fsSession;
				
			//Create a connection to the file server	
			User::LeaveIfError(fsSession.Connect());
						
			RFile sourceFile;
	    	CleanupClosePushL(sourceFile);
	    			
	    	//Open the specified source file		
	    	User::LeaveIfError(sourceFile.Open(fsSession,sourcePath, EFileRead));
	    					
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
				//Create a pointer for the Hash + Key (Hmac) Implementation Object
				CHash* hmacImpl = NULL;

				//Convert encryption key to an 8 Bit Descriptor
				HBufC8* invalidKeyStr = HBufC8::NewLC(invalidEncryptKey.Length());
				TPtr8 invalidKeyStrPtr = invalidKeyStr->Des();
			
				invalidKeyStrPtr.Copy(invalidEncryptKey);
				
				//Create an new CryptoParams object to encapsulate the invalid key type and key string
				CCryptoParams* invalidKeyParams = CCryptoParams::NewL();
				CleanupStack::PushL(invalidKeyParams);
				invalidKeyParams->AddL(*invalidKeyStr,keyType);
				
				//Create Invalid Key Object
				TKeyProperty invalidKeyProperty;
				CKey* invalidKey = CKey::NewL(invalidKeyProperty,*invalidKeyParams);
				CleanupStack::PushL(invalidKey);
				
				//Retrieve a Hmac Factory Object with an Invalid Key			
				TRAPD(err,CHashFactory::CreateHashL(hmacImpl,
													algorithmUid,
													operationModeUid,
													invalidKey,
													NULL));						
				
				if(hmacImpl && (err == KErrNone))
					{
					
					//Push the Hmac Implementation Object onto the Cleanup Stack
					CleanupStack::PushL(hmacImpl);
										
					//Create a NULL TCharacteristics pointer
					const TCharacteristics* invalidCharsPtr(NULL);
					
					//Retrieve the characteristics for the hash implementation object
					TRAP_LOG(err, hmacImpl->GetCharacteristicsL(invalidCharsPtr));
					
					//Static cast the characteristics to type THashCharacteristics
					const THashCharacteristics* hashInvalidCharsPtr = static_cast<const THashCharacteristics*>(invalidCharsPtr);
					
					//The hash output size is returned in Bits, divide by 8 to get the Byte size
					TInt hashSize = hashInvalidCharsPtr->iOutputSize/8;
					
					//Retrieve the final 8bit hash value and convert to 16bit												
					HBufC* invalidHashData = HBufC::NewLC(hashSize);
					TPtr invalidHashPtr = invalidHashData->Des();
					
					invalidHashPtr.Copy(hmacImpl->Hash(*sourceData));
					
					//Take the 16bit descriptor and convert the string to hexadecimal
					TVariantPtrC convertHash;
					convertHash.Set(invalidHashPtr);
					HBufC* invalidHmacResult = convertHash.HexStringLC();							
						
					INFO_PRINTF2(_L("*** Hashed Data: %S ***"),&*invalidHmacResult);
					INFO_PRINTF2(_L("*** Expected Hash: %S ***"),&expectedHash);
					
					if(*invalidHmacResult != expectedHash)
						{
						INFO_PRINTF1(_L("*** INVALID KEY - STAGE 1 PASS ***"));
						
						//Convert encryption key to an 8 Bit Descriptor
						HBufC8* validKeyStr = HBufC8::NewLC(encryptKey.Length());
						TPtr8 validKeyStrPtr = validKeyStr->Des();
			
						validKeyStrPtr.Copy(encryptKey);
						
						//Create an new CryptoParams object to encapsulate the valid key type and secret key string
						CCryptoParams* validKeyParams = CCryptoParams::NewL();
						CleanupStack::PushL(validKeyParams);
						validKeyParams->AddL(*validKeyStr,keyType);
						
						//Create Valid Key Object
						TKeyProperty validKeyProperty;
						CKey* validKey = CKey::NewL(validKeyProperty,*validKeyParams);
						CleanupStack::PushL(validKey);
												
						//Set the valid key within the Hmac Implementation Object
						TRAP(err,hmacImpl->SetKeyL(*validKey));
						
						if(err!=KErrNone)
							{
							ERR_PRINTF2(_L("*** ERROR %d: Setting Valid Key ***"),err);
							User::Leave(err);	
							}
						else
							{
							INFO_PRINTF1(_L("*** HMAC VALID KEY SET ***"));	
							}
							
						//Create a NULL TCharacteristics pointer
						const TCharacteristics* validCharsPtr(NULL);
					
						//Retrieve the characteristics for the hash implementation object
						TRAP_LOG(err, hmacImpl->GetCharacteristicsL(validCharsPtr));
					
						//Static cast the characteristics to type THashCharacteristics
						const THashCharacteristics* hashValidCharsPtr = static_cast<const THashCharacteristics*>(validCharsPtr);
						
						//The hash output size is returned in Bits, divide by 8 to get the Byte size
						hashSize = hashValidCharsPtr->iOutputSize/8;
						
						//Retrieve the final 8bit hash value and convert to 16bit												
						HBufC* validHashData = HBufC::NewLC(hashSize);
						TPtr validHashPtr = validHashData->Des();
						
						validHashPtr.Copy(hmacImpl->Hash(*sourceData));
						
						//Take the 16bit descriptor and convert the string to hexadecimal
						convertHash.Set(validHashPtr);
						HBufC* validHmacResult = convertHash.HexStringLC();							
						
						INFO_PRINTF2(_L("*** Hashed Data: %S ***"),&*validHmacResult);
						INFO_PRINTF2(_L("*** Expected Hash: %S ***"),&expectedHash);
						
						if(*validHmacResult == expectedHash)
							{
							INFO_PRINTF1(_L("*** VALID KEY - STAGE 2 PASS ***"));
							INFO_PRINTF1(_L("*** Hmac - Set Key Checking : PASS ***"));
							SetTestStepResult(EPass);
							}
						else
							{
							ERR_PRINTF1(_L("*** STAGE 2 FAIL: Valid Hash and Expected Value Mismatch ***"));
							SetTestStepResult(EFail);	
							}
						CleanupStack::PopAndDestroy(validHmacResult);
						CleanupStack::PopAndDestroy(validHashData);
						
						CleanupStack::PopAndDestroy(validKey);
						CleanupStack::PopAndDestroy(validKeyParams);
						CleanupStack::PopAndDestroy(validKeyStr);	
						}
					else
						{
						ERR_PRINTF1(_L("*** STAGE 1 FAIL: Invalid Hash and Expected Value Match ***"));
						SetTestStepResult(EFail);	
						}
						
					CleanupStack::PopAndDestroy(invalidHmacResult);	
					CleanupStack::PopAndDestroy(invalidHashData);
					CleanupStack::PopAndDestroy(hmacImpl);									
					}
				else
					{
					ERR_PRINTF2(_L("*** FAIL: Failed to Create Hmac Object - %d ***"), err);
					SetTestStepResult(EFail);	
					}
					
				CleanupStack::PopAndDestroy(invalidKey);
				CleanupStack::PopAndDestroy(invalidKeyParams);
				CleanupStack::PopAndDestroy(invalidKeyStr);
				}
						
			CleanupStack::PopAndDestroy(sourceData);
								
			//Cleanup the Source RFile	
			CleanupStack::PopAndDestroy();	
						
			fsSession.Close();	
			}
			
			
		INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());	
		}
		
	return TestStepResult();
	}


TVerdict CHmacSetKeyCheckingStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}
