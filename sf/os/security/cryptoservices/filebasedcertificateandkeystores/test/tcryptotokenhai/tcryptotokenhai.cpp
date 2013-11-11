/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* This class implements the reference Crypto Token Hardware Abstraction 
* Interface (HAI). It is just intended to show how operations using 
* device keys can be performed using crypto token framework. In the 
* real world scenario, this HAI should be replaced by device drivers 
* by the licensees. In such a case, all the operations performed by 
* the replacing class would be performed in Kernel Space.
*
*/


#include "tcryptotokenhai.h"
#include "tkeydetails.h"
#include "cryptosignatureapi.h"
#include "keys.h"

#include <cryptospi/cryptoparams.h>
#include <cryptospi/cryptospidef.h>

EXPORT_C CCryptoTokenHai* CCryptoTokenHai::NewLC(MCTToken* aToken)
	{
	CCryptoTokenHai* instance = new(ELeave) CCryptoTokenHai(*aToken);
	CleanupStack::PushL(instance);
	instance->ConstructL();
	return instance;
	}

EXPORT_C CCryptoTokenHai* CCryptoTokenHai::NewL(MCTToken* aToken)
	{
	CCryptoTokenHai* instance = CCryptoTokenHai::NewLC(aToken);
	CleanupStack::Pop(instance);
	return instance;
	}

void CCryptoTokenHai::ConstructL()
	{
	User::LeaveIfError(iFs.Connect());
	OpenStoreL();
	}

CCryptoTokenHai::CCryptoTokenHai(MCTToken& aToken)
	:iToken(aToken)
	{}

EXPORT_C CCryptoTokenHai::~CCryptoTokenHai()
	{
	if(iFileStore)
        {
        CompactStore();
        delete iFileStore;
        }
	
	iFs.Close();
	iKeys.ResetAndDestroy();
    iKeys.Close();
	}

/**
 * Performs the decryption operation.
 * 
 * This API gets called when the decryption is supposed to be done in  
 * the hardware.
 * 
 * @param aHandle The key handle
 * @param aCiphertext The cipher text. This is not being used presently 
 * due to decryption logic used in this function.
 * @param aPlainText Output param. The decrypted plain text. Ownership 
 * of the pointer lies with the caller.
 * 
 * @leave This function can leave with following error codes:-
 * - KErrNotFound - If the key corresponding to given handle is not 
 * found.
 * - Any other error code returned by AllocL().
 *
 * @note This function does not actually implement ECC decryption. It 
 * just intends to show that the key is with this class and it can 
 * do actual ECC decryption here. This function just returns the 
 * private key as decrypted text. The caller can verify the decryption 
 * by ensuring that test case has same public and private keys and then 
 * comparing the decrypted text with public key. 
 */
EXPORT_C void CCryptoTokenHai::DecryptL(	TInt aHandle,	
											const TDesC8& /* aCiphertext */,
											HBufC8*& aPlainText )
	{
    TInt keyIndex = KeyPresent(aHandle);
    if(keyIndex == KErrNotFound)
        {
        User::Leave(KErrNotFound);
        }
    
    ExportPrivateKeyL(aHandle, aPlainText);
	}

/**
 * Performs the signing operation.
 * 
 * This API gets called when the signing is supposed to be done inside 
 * the hardware.
 * 
 * @param aHandle The key handle
 * @param aPlaintext The text which has to be signed. This is not being 
 * used due to signing logic used in this function.
 * @param aSignature Output param. The signature in HBufC8 format.  
 * Ownership of the pointer lies with the caller. This should be 
 * converted to CCryptoParams by the crypto token reference plugin. 
 * 
 * @leave This function can leave with following error codes:-
 * - KErrNotFound - If the key corresponding to given handle is not 
 * found.
 * - Any other error code returned by AllocL().
 * 
 * @note This function does not actually implement ECC signing. It 
 * just intends to show that the key is with this class and it can 
 * do actual ECC signing here. Currently this function just returns 
 * the private key as output signature. The caller can verify the 
 * signature by ensuring that test case has same public and private 
 * keys and then comparing the signature with public key.
 */
EXPORT_C void CCryptoTokenHai::SignL( 	TInt aHandle,
										const TDesC8& /* aPlaintext */,
										HBufC8*& aSignature )
	{
	TInt keyIndex = KeyPresent(aHandle);
	if(keyIndex == KErrNotFound)
	    {
	    User::Leave(KErrNotFound);
	    }
	
	ExportPrivateKeyL(aHandle, aSignature);
	}

/**
 * Returns the index of the key whose handle is given.
 * 
 * @param aHandle Handle of the key. This is used to search the key.
 * 
 * @return index of the key if search is successful, KErrNotFound 
 * otherwise.
 */
EXPORT_C TInt CCryptoTokenHai::KeyPresent( TInt aHandle )
	{
	int keysCount = iKeys.Count();
	for(TInt i=0; i < keysCount; ++i)
		{
		if(iKeys[i]->Handle() == aHandle)
			{
			return i;
			}
		}
	return KErrNotFound;
	}

/**
 * Extracts the private key.
 * 
 * @param aHandle Handle of the private key to be extracted.
 * @param aKey Output Parameter. Stores the private key on success. 
 * Ownership of pointer is with the caller.
 * 
 * @leave Following leave codes possible:-
 * - Any leave code returned by AllocL().
 * - KErrNotFound - If key corresponding to the given handle is not 
 * found.
 * 
 * @note In the actual implementation, licensees should ensure that 
 * this function can be called only in Kernel space. In the reference 
 * implementation, this function gets called only by CCryptoSpiHai, 
 * which is assumed to operate in kernel space. This would ensure that 
 * the private key always stays inside the hardware.
 */
EXPORT_C void CCryptoTokenHai::ExportPrivateKeyL( TInt aHandle, HBufC8*& aKey )
	{
	int keysCount = iKeys.Count();
	for(int i = 0; i < keysCount; ++i)
		{
		if(iKeys[i]->Handle() == aHandle)
			{
			aKey = iKeys[i]->PrivateKey()->AllocL();
			return;
			}
		}
	User::Leave(KErrNotFound);
	}

/**
 * Extracts the public key.
 * 
 * @param aHandle Handle of the public key to be extracted.
 * @param aKey Output Parameter. Stores the public key on success.
 * Ownership of pointer is with the caller.
 * 
 * @leave Following leave codes possible:-
 * - Any leave code returned by AllocL().
 * - KErrNotFound - If key corresponding to the given handle is not 
 * found.
 */
EXPORT_C void CCryptoTokenHai::ExportPublicKeyL( TInt aHandle, HBufC8*& aKey )
    {
    int keysCount = iKeys.Count();
    for(int i = 0; i < keysCount; ++i)
        {
        if(iKeys[i]->Handle() == aHandle)
            {
            aKey = iKeys[i]->PublicKey()->AllocL();
            return;
            }
        }
    User::Leave(KErrNotFound);
    }

/**
 * Stores the key with given details.
 * 
 * @param aLabel Label of the key.
 * @param aPrivateKey Private component of the key.
 * @param aPublicKey Public component of the key.
 * 
 * @leave Following leave codes possible:-
 * - KErrAlreadyExists If there is already a key with the inputted
 * label.
 * - Any other leave code returned by NewL() or AppendL().
 *  
 * @note In the present reference implementation this function is not 
 * being used, since device keys are pre-provisioned by the licensees. 
 * Hence licensees may decide not to implement this function in their 
 * real implementation. 
 */
EXPORT_C void CCryptoTokenHai::ImportKeyL(const TDesC& aLabel, 
        const TDesC8& aPrivateKey, const TDesC8& aPublicKey)
	{
	int keysCount = iKeys.Count();
	for(int i = 0; i < keysCount; ++i)
		{
		if(iKeys[i]->Label() == aLabel)
			{
			User::Leave(KErrAlreadyExists);
			}
		}
	CKeyDetails* keyDetails = CKeyDetails::NewL(keysCount+1,aLabel,aPrivateKey,aPublicKey);
	iKeys.AppendL(keyDetails);
	}

/**
 * Populates the string containing full RAM path of file containing 
 * keys.
 */
void CCryptoTokenHai::MakePrivateFilenameL(RFs& aFs, const TDesC& aLeafName, TDes& aNameOut)
    {
    aNameOut.SetLength(0);  
    aNameOut.Append(RFs::GetSystemDriveChar());

    aNameOut.Append(':');

    // Get private path
    TBuf<20> privatePath;
    User::LeaveIfError(aFs.PrivatePath(privatePath));
    aNameOut.Append(privatePath);
    
    aNameOut.Append(aLeafName);
    }

/**
 * Creates the corresponding directory, if it does not exist.
 */
void CCryptoTokenHai::EnsurePathL(RFs& aFs, const TDesC& aFile)
    {
    TInt err = aFs.MkDirAll(aFile);
    if (err != KErrNone && err != KErrAlreadyExists)
        {
        User::Leave(err);
        }
    }

/**
 * Populates the string containing full ROM path of the keys file.
 */
void CCryptoTokenHai::MakePrivateROMFilenameL(RFs& aFs, const TDesC& aLeafName, TDes& aNameOut)
    {
    _LIT(KFileStoreROMDrive, "Z:");
    
    aNameOut.Copy(KFileStoreROMDrive);

    // Get private path
    TBuf<20> privatePath;
    User::LeaveIfError(aFs.PrivatePath(privatePath)); 
    aNameOut.Append(privatePath);
    aNameOut.Append(aLeafName);
    }

/**
 * Copies the contents of source file to destination file.
 * 
 * This is typically used to copy the keys file from ROM to RAM.
 */
void CCryptoTokenHai::CopyL(RFs& aFs, const TDesC& aSouce, const TDesC& aDest)
    {
    RFileReadStream in;
    User::LeaveIfError(in.Open(aFs, aSouce, EFileRead | EFileShareReadersOnly));
    CleanupClosePushL(in);

    RFileWriteStream out;
    User::LeaveIfError(out.Replace(aFs, aDest, EFileWrite | EFileShareExclusive));
    CleanupClosePushL(out);

    in.ReadL(out);  
    CleanupStack::PopAndDestroy(2, &in);
    }

/**
 * Keys corresponding to this store are present in hwkeys.dat. 
 * In the production code written by licensees, this would be the path 
 * where device keys are stored.
 */
_LIT(KKeyStoreFilename,"hwkeys.dat");

/**
 * Opens a store containing hardware keys.
 * 
 * This function uses the following logic to open the store:-
 * -# Try to open the store from the private directory.
 * -# If this fails copy the file from ROM to RAM.
 * -# If both fail, create your own keys store from scratch.
 */
void CCryptoTokenHai::OpenStoreL()
	{
	TFileName fullPath;
	MakePrivateFilenameL(iFs, KKeyStoreFilename, fullPath);

	EnsurePathL(iFs, fullPath);
	TRAPD(result, OpenStoreInFileL(fullPath));

	if (result == KErrInUse  ) 
		{		
		// Cannot access the file now. Abort rather than wiping the keystore.
		User::Leave(result); 
		}
	
	if (result != KErrNone )
		{		
		/*
		 * Not yet opened a valid store, either no file to be found, or 
		 * no valid store in it. Copy the original one stored in the 
		 * ROM.
		 */
		TRAPD(result2, CopyStoreFromROML(fullPath, result));
				
		if (KErrNone != result2)
			{
			/*
			 * We tried to copy the keystore from ROM. For some reason this
			 * failed and we still cannot open the file. Create a new one from
			 * scratch.
			 */ 
			CreateStoreInFileL(fullPath);
			}
		}

	}

/**
 * Copies the key store file from ROM to RAM.
 */
void CCryptoTokenHai::CopyStoreFromROML(const TDesC& fullPath, TInt result)
    {
    if (result != KErrNotFound)
        {
        // Wipe the keystore if we can't open it (it's corrupt anyway)
        User::LeaveIfError(iFs.Delete(fullPath));
        }

    TFileName romPath;
    MakePrivateROMFilenameL(iFs, KKeyStoreFilename, romPath);

    // Copy data from rom and open it   
    CopyL(iFs, romPath, fullPath);
    OpenStoreInFileL(fullPath);
    }

/**
 * Opens a store from the given file.
 */
void CCryptoTokenHai::OpenStoreInFileL(const TDesC& aFile)
	{
	RFile file;
	User::LeaveIfError(file.Open(iFs, aFile, EFileRead | EFileWrite | EFileShareAny));
	CleanupClosePushL(file);
    delete iFileStore;
    iFileStore = NULL;

	iFileStore = CPermanentFileStore::FromL(file);
    // iFileStore takes ownership of file now
	CleanupStack::Pop(&file);
	
    // Get the salt, root and manager TStreamIds
    iRootStreamId = iFileStore->Root();
    if (iRootStreamId == KNullStreamId)
        {
        User::Leave(KErrCorrupt);
        }
    RStoreReadStream rootStream;
    rootStream.OpenLC(*iFileStore, iRootStreamId);
    ReadKeysFromStoreL();
    CleanupStack::PopAndDestroy(&rootStream);
    }

/**
 * Creates a keys store in RAM from scratch.
 * 
 * @note This function should never get called as hwkeys.dat should be 
 * always present in ROM. If this function somehow gets called, it 
 * will create a hwkeys.dat file from scratch. However, this file would 
 * not contain any keys and tests would not pass.
 */
void CCryptoTokenHai::CreateStoreInFileL(const TDesC& aFile)
	{
	TInt r = iFs.MkDirAll(aFile);
	if ( (r!=KErrNone) && (r!=KErrAlreadyExists) )
		User::Leave(r);

    delete iFileStore;
    iFileStore = NULL;
	iFileStore = CPermanentFileStore::ReplaceL(iFs, aFile, EFileRead | EFileWrite | EFileShareExclusive);
	iFileStore->SetTypeL(KPermanentFileStoreLayoutUid);

	TCleanupItem cleanupStore(RevertStore, iFileStore);
	CleanupStack::PushL(cleanupStore);
	
	// Create root stream - just contains id of info stream
	RStoreWriteStream rootStream;
	iRootStreamId = rootStream.CreateLC(*iFileStore);
	iFileStore->SetRootL(iRootStreamId);
	WriteKeysToStoreL(rootStream);
	iFileStore->CommitL();
	CleanupStack::PopAndDestroy(&rootStream);
	CleanupStack::Pop(); // cleanupStore
	}

/**
 * Copies the keys stored in the instance to inputted write stream.
 * 
 * This invokes the CKeyDetails::ExternalizeL() function.
 */
void CCryptoTokenHai::WriteKeysToStoreL(RStoreWriteStream& aRootStream)
	{
	TInt keyCount = iKeys.Count();
	aRootStream.WriteInt32L(keyCount);

	for (TInt index = 0; index < keyCount; index++)
		{
		aRootStream << *iKeys[index];
		}
	aRootStream.CommitL();
	}

/**
 * Copies the keys present in the read store to instance of class.
 * 
 * This eventually invokes the CKeyDetails::InternalizeL() function.
 */
void CCryptoTokenHai::ReadKeysFromStoreL()
	{
	RStoreReadStream rootStream;
	
	rootStream.OpenLC(*iFileStore, iRootStreamId);
	TInt keyCount = rootStream.ReadInt32L();

	for (TInt index = 0; index < keyCount; index++)
		{
		CKeyDetails* keyDetails = CKeyDetails::NewL(rootStream);
		iKeys.Append(keyDetails);
		}
	CleanupStack::PopAndDestroy(&rootStream);
	}

/**
 * This is a cleanup item that reverts the store.
 */
void CCryptoTokenHai::RevertStore(TAny* aStore)
	{
	CPermanentFileStore* store = reinterpret_cast<CPermanentFileStore*>(aStore);
	TRAP_IGNORE(store->RevertL());
	}

/**
 * Compacts the store.
 */
void CCryptoTokenHai::CompactStore()
    {
    ASSERT(iFileStore);
    TRAP_IGNORE(iFileStore->ReclaimL(); iFileStore->CompactL());
    }

/**
 * Populates the list of keys based on the input filter.
 * 
 * @param aFilter Set of conditions to be used to decide which keys 
 * should be listed
 * @param aKeys Output param. Contains the array of keys which fulfil 
 * criteria mentioned in filter. Caller should take responsibility of 
 * this array.
 * 
 * @leave Any of the system wide error codes.
 * 
 * @note Though Crypto Token HAI internally operates in CKeyDetails, 
 * this function returns CCTKeyInfo array.
 */
EXPORT_C void CCryptoTokenHai::ListL(const TCTKeyAttributeFilter&  aFilter , 
                RPointerArray<CCTKeyInfo>& aKeys) const
    {
    TInt count = iKeys.Count();
    for(TInt index = 0 ;index < count; ++ index)
    	{
    	const CKeyDetails* keyDetails = iKeys[index];
    	
    	if(KeyMatchesFilterL(*keyDetails,aFilter))
    		{
			MCTAuthenticationObject* authObject = NULL;
			HBufC8* attribute = keyDetails->PKCS8AttributeSet().AllocLC();
			HBufC* label = keyDetails->Label().AllocLC();
			
			CCTKeyInfo* keyInfo = CCTKeyInfo::NewL(
					keyDetails->ID(),keyDetails->Usage(),keyDetails->Size(),
					authObject,label,iToken,keyDetails->Handle(),keyDetails->UsePolicy(),
					keyDetails->ManagementPolicy(),keyDetails->Algorithm(),keyDetails->AccessType(),
					keyDetails->Native(),keyDetails->StartDate(),keyDetails->EndDate(),attribute);
			
			CleanupStack::Pop(2, attribute); // label
			CleanupReleasePushL(*keyInfo);
						
			User::LeaveIfError(aKeys.Append(keyInfo));
			CleanupStack::Pop(keyInfo); 
			
			}
    	}
    	
    }

/**
 * Takes in a filter and key details and decides if key fulfils the 
 * filter criteria.
 * 
 * @param aInfo The Key Details
 * @param aFilter Filter specifying the conditions to be satisfied for 
 * listing the keys.
 * 
 * @retval ETrue if key satisfies the conditions specified in filter
 * @retval EFalse otherwise.
 * 
 * @leave KErrArgument If there is an issue in policy filter.
 */
TBool CCryptoTokenHai::KeyMatchesFilterL(const CKeyDetails& aInfo,
										   const TCTKeyAttributeFilter& aFilter) const
	{
		
	if (aFilter.iKeyId.Length() && aFilter.iKeyId != aInfo.ID())
		{
		return EFalse;
		}

	if (aFilter.iUsage != EPKCS15UsageAll)
		{
		if ((aInfo.Usage() & aFilter.iUsage) == 0)
			return EFalse;
		}

	if (aFilter.iKeyAlgorithm != CCTKeyInfo::EInvalidAlgorithm && 
		aFilter.iKeyAlgorithm != aInfo.Algorithm())
		{
		return EFalse;
		}
	
	switch (aFilter.iPolicyFilter)
		{
		case TCTKeyAttributeFilter::EAllKeys:
			// All keys pass
			break;
			   
		case TCTKeyAttributeFilter::EUsableKeys:
			if (!aInfo.UsePolicy().CheckPolicy(RThread()))
				{
				return EFalse;
				}
			break;
		case TCTKeyAttributeFilter::EManageableKeys:
			// As this key store implementation is a hardware simulation,
			// the support for managing through software interface has been diabled.
			return EFalse;

		case TCTKeyAttributeFilter::EUsableOrManageableKeys:
			if (!aInfo.UsePolicy().CheckPolicy(RThread()) &&
				!aInfo.ManagementPolicy().CheckPolicy(RThread()))
				{
				return EFalse;
				}
			break;
						
		default:
			User::Leave(KErrArgument);
		}

	return ETrue;
	}



