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
#include "hmacsetoperationmodecheckingstep.h"

#include <cryptospi/cryptohashapi.h>
#include <cryptospi/keys.h>
#include <cryptospi/plugincharacteristics.h>

using namespace CryptoSpi;

CHmacSetOperationModeCheckingStep::~CHmacSetOperationModeCheckingStep()
	{
	}


CHmacSetOperationModeCheckingStep::CHmacSetOperationModeCheckingStep()
	{
	SetTestStepName(KHmacSetOperationModeCheckingStep);
	}


TVerdict CHmacSetOperationModeCheckingStep::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CHmacSetOperationModeCheckingStep::doTestStepL()
	{
	if (TestStepResult()==EPass)
		{
		
		//Assume faliure, unless all is successful
		SetTestStepResult(EFail);
		
		INFO_PRINTF1(_L("*** Hmac - Set Operation Mode Checking ***"));
		INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());
		
		TVariantPtrC algorithmUid;
		TVariantPtrC operationModeUid;
		TVariantPtrC secondOperationModeUid;
		TPtrC sourcePath;
		TPtrC expectedHash;
		TPtrC expectedHmac;
		TPtrC encryptKey;
		TVariantPtrC keyType;
		
		//Extract the Test Case ID parameter from the specified INI file
		if(!GetStringFromConfig(ConfigSection(),KConfigAlgorithmUid,algorithmUid) ||
			!GetStringFromConfig(ConfigSection(),KConfigOperationMode,operationModeUid) ||
			!GetStringFromConfig(ConfigSection(),KConfigSecondOperationMode,secondOperationModeUid) ||
			!GetStringFromConfig(ConfigSection(),KConfigSourcePath,sourcePath) ||
			!GetStringFromConfig(ConfigSection(),KConfigExHashHmacValue,expectedHash) ||
			!GetStringFromConfig(ConfigSection(),KConfigExSecondHashHmacValue,expectedHmac) ||
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
				CHash* hashHmacImpl = NULL;
				
				//Convert encryption key to an 8 Bit Descriptor
				HBufC8* keyStr = HBufC8::NewLC(encryptKey.Length());
				TPtr8 keyStrPtr = keyStr->Des();
				
				keyStrPtr.Copy(encryptKey);
				
				//Create an new CryptoParams object to encapsulate the invalid key type and key string
				CCryptoParams* keyParams = CCryptoParams::NewL();
				CleanupStack::PushL(keyParams);
				keyParams->AddL(*keyStr,keyType);
				
				//Create a valid CKey Object
				TKeyProperty keyProperty;
				CKey* key = CKey::NewL(keyProperty,*keyParams);
				CleanupStack::PushL(key);
				
				//Construct an initial hash object with NO key, Catching any possible Leaves			
				TRAPD(err,CHashFactory::CreateHashL(
									hashHmacImpl,
									algorithmUid,
									operationModeUid,
									NULL,
									NULL));
									
										
				if(hashHmacImpl && (err == KErrNone))
					{
					
					//Push the Implementation Object onto the Cleanup Stack
					CleanupStack::PushL(hashHmacImpl);
					
					//Create a NULL TCharacteristics pointer
					const TCharacteristics* charsPtrA(NULL);
					
					//Retrieve the characteristics for the hash implementation object
					TRAP_LOG(err, hashHmacImpl->GetCharacteristicsL(charsPtrA));
					
					//Static cast the characteristics to type THashCharacteristics
					const THashCharacteristics* hashCharsPtrA = static_cast<const THashCharacteristics*>(charsPtrA);
					
					//The hash output size is returned in Bits, divide by 8 to get the Byte size
					TInt hashSize = hashCharsPtrA->iOutputSize/8;
					
					//Retrieve the final 8bit hash value and convert to 16bit												
					HBufC* hashDataA = HBufC::NewLC(hashSize);
					TPtr hashPtrA = hashDataA->Des();
					
					hashPtrA.Copy(hashHmacImpl->Hash(*sourceData));
					
					//Take the 16bit descriptor and convert the string to hexadecimal
					TVariantPtrC convertHash;
					convertHash.Set(hashPtrA);
					
					HBufC* resultA = convertHash.HexStringLC();							
						
					INFO_PRINTF2(_L("*** Hashed Data: %S ***"),&*resultA);
					INFO_PRINTF2(_L("*** Expected Hash: %S ***"),&expectedHash);
					
					if(*resultA == expectedHash)
						{
						INFO_PRINTF1(_L("*** PRIMARY HASH VALID - STAGE 1 PASS ***"));
						
						//Set the valid key within the Hmac Implementation Object
						TRAP(err,hashHmacImpl->SetKeyL(*key));
						
						if(err!=KErrNone)
							{
							ERR_PRINTF2(_L("*** ERROR %d: Setting Key ***"),err);
							User::Leave(err);	
							}
						else
							{
							INFO_PRINTF1(_L("*** HMAC KEY SET ***"));	
							}
										
						//Set the Operation Mode of the Hmac Implementation Object
						hashHmacImpl->SetOperationModeL(secondOperationModeUid);
						
						if(err!=KErrNone)
							{
							ERR_PRINTF3(_L("*** ERROR %d: Setting Operation Mode %S ***"),err,&secondOperationModeUid);	
							User::Leave(err);
							}
						else
							{
							INFO_PRINTF2(_L("*** OPERATION MODE SET : %S ***"),&secondOperationModeUid);	
							}
						
						//Create a NULL TCharacteristics pointer
						const TCharacteristics* charsPtrB(NULL);
					
						//Retrieve the characteristics for the hash implementation object
						TRAP_LOG(err, hashHmacImpl->GetCharacteristicsL(charsPtrB));
					
						//Static cast the characteristics to type THashCharacteristics
						const THashCharacteristics* hashCharsPtrB = static_cast<const THashCharacteristics*>(charsPtrB);
						
						//The hash output size is returned in Bits, divide by 8 to get the Byte size
						hashSize = hashCharsPtrB->iOutputSize/8;
						
						//Retrieve the final 8bit hash value and convert to 16bit												
						HBufC* hashDataB = HBufC::NewLC(hashSize);
						TPtr hashPtrB = hashDataB->Des();
						
						hashPtrB.Copy(hashHmacImpl->Hash(*sourceData));
						
						//Take the 16bit descriptor and convert the string to hexadecimal
						convertHash.Set(hashPtrB);
						HBufC* resultB = convertHash.HexStringLC();						
						
						INFO_PRINTF2(_L("*** Hashed Data: %S ***"),&*resultB);
						INFO_PRINTF2(_L("*** Expected Hash: %S ***"),&expectedHmac);
						
						if(*resultB == expectedHmac)
							{
							INFO_PRINTF1(_L("*** SECONDARY HASH VALID - STAGE 2 PASS ***"));
							
							//Set the Operation Mode of the Hmac Implementation Object
							TRAP(err,hashHmacImpl->SetOperationModeL(operationModeUid));
							
							if(err!=KErrNone)
								{
								ERR_PRINTF3(_L("*** ERROR %d: Setting Operation Mode %S ***"),err,&operationModeUid);	
								User::Leave(err);
								}
							else
								{
								INFO_PRINTF2(_L("*** OPERATION MODE SET : %S ***"),&operationModeUid);	
								}
								
							//Create a NULL TCharacteristics pointer
							const TCharacteristics* charsPtrC(NULL);
						
							//Retrieve the characteristics for the hash implementation object
							TRAP_LOG(err, hashHmacImpl->GetCharacteristicsL(charsPtrC));
						
							//Static cast the characteristics to type THashCharacteristics
							const THashCharacteristics* hashCharsPtrC = static_cast<const THashCharacteristics*>(charsPtrC);
							
							//The hash output size is returned in Bits, divide by 8 to get the Byte size
							hashSize = hashCharsPtrC->iOutputSize/8;
							
							//Retrieve the final 8bit hash value and convert to 16bit												
							HBufC* hashDataC = HBufC::NewLC(hashSize);
							TPtr hashPtrC = hashDataC->Des();
							
							hashPtrC.Copy(hashHmacImpl->Hash(*sourceData));
							
							//Take the 16bit descriptor and convert the string to hexadecimal
							convertHash.Set(hashPtrC);
							HBufC* resultC = convertHash.HexStringLC();							
							
							INFO_PRINTF2(_L("*** Hashed Data: %S ***"),&*resultC);
							INFO_PRINTF2(_L("*** Expected Hash: %S ***"),&expectedHash);
									
							if(*resultC == expectedHash)
								{
								INFO_PRINTF1(_L("*** FINAL HASH VALID - STAGE 3 PASS ***"));
								INFO_PRINTF1(_L("*** Hmac - Set Operation Mode Checking : PASS ***"));
								SetTestStepResult(EPass);
								}
							else
								{
								ERR_PRINTF1(_L("*** STAGE 3 FAIL: Hash and Expected Value Mismatch ***"));
								SetTestStepResult(EFail);	
								}
								
							CleanupStack::PopAndDestroy(resultC);
							CleanupStack::PopAndDestroy(hashDataC);
							}
						else
							{
							ERR_PRINTF1(_L("*** STAGE 2 FAIL: Hash and Expected Value Mismatch ***"));
							SetTestStepResult(EFail);	
							}
							
						CleanupStack::PopAndDestroy(resultB);
						CleanupStack::PopAndDestroy(hashDataB);	
						}
					else
						{
						ERR_PRINTF1(_L("*** STAGE 1 FAIL: Hash and Expected Value Match ***"));
						SetTestStepResult(EFail);	
						}
						
					CleanupStack::PopAndDestroy(resultA);
					CleanupStack::PopAndDestroy(hashDataA);
					CleanupStack::PopAndDestroy(hashHmacImpl);										
					}
				else if(err==KErrNotSupported)
					{
					if((((TUid)operationModeUid != KHashModeUid) && ((TUid)operationModeUid != KHmacModeUid)) ||
						((TUid)algorithmUid != KMd2Uid) && (TUid)algorithmUid != KMd5Uid && (TUid)algorithmUid != KSha1Uid && (TUid)algorithmUid != KMd4Uid && (TUid)algorithmUid != KSha224Uid && (TUid)algorithmUid != KSha256Uid && (TUid)algorithmUid != KSha384Uid && (TUid)algorithmUid != KSha512Uid)
						{
						ERR_PRINTF2(_L("*** Object Load Failure - Invalid Operation Mode : %d ***"), err);
						User::Leave(err);	
						}
					else
						{
						SetTestStepResult(EFail);	
						}
					}
				else
					{
					ERR_PRINTF2(_L("*** Hash/Hmac Facotry Object Load Failure : %d ***"), err);
					User::Leave(err);	
					}
					
				CleanupStack::PopAndDestroy(key);
				CleanupStack::PopAndDestroy(keyParams);
				CleanupStack::PopAndDestroy(keyStr);
				}
							
			CleanupStack::PopAndDestroy(sourceData);
								
			//Cleanup the Source RFile	
			CleanupStack::PopAndDestroy();	
						
			fsSession.Close();	
			}
			
		}
		else
		{
			SetTestStepResult(EFail);	
		}		
		
	INFO_PRINTF2(_L("HEAP CELLS: %d"), User::CountAllocCells());	
	
	return TestStepResult();
	}


TVerdict CHmacSetOperationModeCheckingStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}
