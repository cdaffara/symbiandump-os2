// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

/**
 @file
 @internalTechnology
*/
#include <s32mem.h>
#include "BaSsndStore.h"
#include "BASSNDUID.h"

// The repository format is: one setting for MajorUID, one
// setting for sound type, one setting for the actual sound
// data, and then leave one key space empty to round up to
// power of 2.
const TUint32 KSoundCatPartialKey = 0x1;
const TUint32 KSoundTypePartialKey = 0x2;
const TUint32 KSoundSettingPartialKey = 0x3;
const TUint32 KNumUnusedKeySpace = 1;
const TUint32 KSsndKeyMask = 0x3;

const TUint32 KKeyOffsetFromSoundType = 
	KSoundSettingPartialKey - KSoundTypePartialKey;
const TUint32 KKeyOffsetFromSoundCat = 
	KSoundSettingPartialKey - KSoundCatPartialKey;
const TUint32 KSsndBlockSize = KSoundSettingPartialKey + KNumUnusedKeySpace;

/** static factory method to instantiate an instance of CBaSsndStore */
CBaSsndStore* CBaSsndStore::NewL()
	{
	CBaSsndStore* self = new(ELeave)CBaSsndStore;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

/** CBaSsndStore constructor */
CBaSsndStore::CBaSsndStore()
	{
	}

/** standard two phase construction to setup the CBaSsndStore object */
void CBaSsndStore::ConstructL()
	{
	iRepository = CRepository::NewL(KSystemSoundRepositoryUID);
	}

/** CBaSsndStore destructor */
CBaSsndStore::~CBaSsndStore()
	{
	delete iRepository;
	}

/** Retrieve sound data from repository.
@param aSoundType identifies the sound to retrieve.
@param aInfo contains the sound data on return.
@leave any of the system-wide error codes.
*/
void CBaSsndStore::GetSoundL(const TBaSystemSoundType& aSoundType,
							 TBaSystemSoundInfo& aInfo) const
	{
	TUint32 key;
	FindKeyL(aSoundType, key, EFalse);

	HBufC8* hbuf8 = HBufC8::NewLC(sizeof(TBaSystemSoundInfo));
	TPtr8 buf8 = hbuf8->Des();
	User::LeaveIfError(iRepository->Get(key + KKeyOffsetFromSoundType, buf8));

	RDesReadStream strm(buf8);
	aInfo.InternalizeL(strm);

	CleanupStack::PopAndDestroy(hbuf8);

	if (! (aSoundType == aInfo.iType))
		{
		User::Leave(KErrCorrupt);
		}
	}

/** Search for a sound from storage and return the sound data.
@param aInfo as input contains the ID of the sound instance to
	get. As output, return the sound info if operation successful. 
@return KErrNone if successful, otherwise KErrNotFound or
	any other system-wide errors.
*/
TInt CBaSsndStore::GetSound(const TBaSystemSoundType& aSoundType,
							TBaSystemSoundInfo& aInfo) const
	{
	TRAPD(err, GetSoundL(aSoundType, aInfo));
	return err;
	}

/**
Overwrite existing sound if it is already in sound table.
Add it if it does not exist.
@param aInfo the sound data to save in repository.
@leave one of the system-wide errors if set fails.
*/
void CBaSsndStore::SetSoundL(const TBaSystemSoundInfo& aInfo) const
	{
	HBufC8* hbuf8 = HBufC8::NewLC(sizeof(TBaSystemSoundInfo));
	TPtr8 buf8 = hbuf8->Des();
	RDesWriteStream writeStream( buf8 );
	aInfo.ExternalizeL(writeStream);
	writeStream.CommitL();

	TBaSystemSoundType ssType(aInfo.iType);
	// If two threads simultaneously add sound, one of them
	// will fail. Hence retry.
	const TInt KMaxRetryTransaction = 3;
	TInt err(KErrLocked);
	for (TInt i = 0; 
		 (i < KMaxRetryTransaction) && (err == KErrLocked || err == KErrAbort);
		 i++)
		{
		TRAP(err, SetSoundInTransactionL(ssType, buf8));
		}

	CleanupStack::PopAndDestroy(hbuf8);
	User::LeaveIfError(err);
	}

/** If the given sound type exists in repository it is over written.
If not exists, create a new entry. This modify/create logic must be
wrapped in a CentRep transaction.
@param aSoundType Use this unique indentifier to find
       if the sound already exists in repository. If it exists
	   use Set. If not, use Create.
@param aDes8 contains the sound data streamed to a TDesC8 buffer.
@leave any of the system-wide errors.
*/
void CBaSsndStore::SetSoundInTransactionL(TBaSystemSoundType& aSoundType,
										  const TDesC8& aDes8) const
	{
	User::LeaveIfError( iRepository->StartTransaction(CRepository::EConcurrentReadWriteTransaction) );
	iRepository->CleanupCancelTransactionPushL();

	TUint32 errorId;

	TUint32 key;
	KeyOfSoundTypeL(aSoundType, key);

	if (key != NCentralRepositoryConstants::KUnspecifiedKey)
		{
		User::LeaveIfError(iRepository->Set(key + KKeyOffsetFromSoundType, aDes8));
		User::LeaveIfError(iRepository->CommitTransaction(errorId));
		CleanupStack::Pop(); // transaction
		return;
		}
	
	FindNextEmptySlotL(key);

	TPckg<TBaSystemSoundUid> ssuid(aSoundType.iMajor);
	User::LeaveIfError(iRepository->Create(key++, ssuid));

	TPckg<TBaSystemSoundType> sstype(aSoundType);
	User::LeaveIfError(iRepository->Create(key++, sstype));

	User::LeaveIfError(iRepository->Create(key, aDes8) );

	User::LeaveIfError(iRepository->CommitTransaction(errorId));
	CleanupStack::Pop(); // transaction
	}

/** Get all instances of a sound category
@param aSSUid identifies the category to retrieve.
@param aArray output parameter to return the sound instances
@return KErrNone if successful, else one of the system-wide error codes
*/
TInt CBaSsndStore::GetSoundCategory(const TBaSystemSoundUid& aSSUid,
									CArrayFixFlat<TBaSystemSoundInfo>& aArray) const
	{
	TRAPD(ret, GetSoundCategoryL(aSSUid, aArray));
	return ret;
	}

/** Get all instances of a sound category.
@param aSSUid identifies the category to retrieve.
@param aArray output parameter to return the sound instances.
@leave any of the system-wide error codes.
*/
void CBaSsndStore::GetSoundCategoryL(const TBaSystemSoundUid& aSSUid,
									 CArrayFixFlat<TBaSystemSoundInfo>& aArray) const
	{
	RArray<TUint32> keys;
	FindKeyL(aSSUid, keys);
	CleanupClosePushL(keys);

	HBufC8* hbuf8 = HBufC8::NewLC(sizeof(TBaSystemSoundInfo));
	TPtr8 buf8 = hbuf8->Des();
	TBaSystemSoundInfo* sound = new(ELeave) TBaSystemSoundInfo;
	CleanupStack::PushL(sound);

	TInt n = keys.Count();
	for (TInt i = 0; i < n; i++)
		{
		User::LeaveIfError(iRepository->Get(
			keys[i] + KKeyOffsetFromSoundCat, buf8));
		RDesReadStream strm(buf8);
		sound->InternalizeL(strm);
		aArray.AppendL(*sound);
		}

	CleanupStack::PopAndDestroy(sound);
	CleanupStack::PopAndDestroy(hbuf8);
	CleanupStack::PopAndDestroy(&keys);
	}

/**
Search for a sound instance in repository.
@param aSoundType identifies the TBaSystemSoundType to search for.
@param aKey output parameter containing the key of the sound type.
@param aNoLeaveIfNotFound indicate whether this method should leave if
		the sound type is not found.
@leave KErrNotFound if not found, plus other system-wide errors.
*/
void CBaSsndStore::FindKeyL(const TBaSystemSoundType& aSoundType,
							TUint32& aKey,
							TBool aNoLeaveIfNotFound) const
	{
	aKey = NCentralRepositoryConstants::KUnspecifiedKey;

	TBaSystemSoundType soundTypeCopy = aSoundType;
	TPckg<TBaSystemSoundType> target(soundTypeCopy);
	RArray<TUint32> foundIds;

	TInt ret = iRepository->FindEqL(KSoundTypePartialKey, 
		KSsndKeyMask, target, foundIds);
	if (ret == KErrNotFound && aNoLeaveIfNotFound)
		{
		return;
		}

	User::LeaveIfError(ret);
	aKey = foundIds[0];
	foundIds.Reset();
	}

/**
Search for a sound category
@param aSSUid the sound category to search for.
@param aKeys output parameter containing keys having the sound category
		as their values.
@leave KErrNotFound if not found, plus other system-wide errors.
*/
void CBaSsndStore::FindKeyL(const TBaSystemSoundUid& aSSUid, RArray<TUint32>& aKeys) const
	{
	TBaSystemSoundUid ssuidCopy = aSSUid;
	TPckg<TBaSystemSoundUid> target(ssuidCopy);

	User::LeaveIfError( iRepository->FindEqL(KSoundCatPartialKey, 
		KSsndKeyMask, target, aKeys) );
	}

/** Call FindKeyL with aNoLeaveIfNotFound parameter set to true.
Used in SetSound to intercept and save the key.
@param aSoundType is the sound type to search for.
@param aKey output parameter to hold the key of the sound type.
@see CBaSsndStore::FindKeyL
*/
void CBaSsndStore::KeyOfSoundTypeL(TBaSystemSoundType& aSoundType,
								   TUint32& aKey) const
	{
	FindKeyL(aSoundType, aKey, ETrue);
	}

/**
Find the next unused key to append a new sound.
@param aKey on return contains the key to use to store
	the next sound.
@leave any of the system-wide error codes.
*/
void CBaSsndStore::FindNextEmptySlotL(TUint32& aKey) const
	{
	RArray<TUint32> foundIds;

	TInt ret = iRepository->FindL(KSoundSettingPartialKey,
		KSsndKeyMask, foundIds);
	if (ret == KErrNotFound)
		{
		// Empty repository. Start storing at key 1
		aKey = 1;
		}
	else if (ret == KErrNone)
		{
	    // The array is sorted. The max key is the last one in array.
		TInt n = foundIds.Count();
		TUint32 maxkey = foundIds[n-1];
		foundIds.Reset();

		TUint32 expectedArraySize = maxkey / KSsndBlockSize + 1;
		if (expectedArraySize > n)
			{
			// Rogue app bypassed BaSystemSound and added non-contiguous
			// entries in repository.
			User::Leave(KErrCorrupt);
			}

		// next record to be stored at maxkey + 2
		aKey = maxkey + KNumUnusedKeySpace + 1;
		}
	else
		{
		User::Leave(ret);
		}
	}
