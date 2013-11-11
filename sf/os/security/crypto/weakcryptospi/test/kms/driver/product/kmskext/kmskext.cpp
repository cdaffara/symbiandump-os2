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
* Implements an emulated hardware key store.
*
*/


/**
 @file
*/

#include <nkern/nkern.h>
#include "kmskext.h"
#include <e32def_private.h>

// singleton keystore instance
XKeyStore KeyStore;

/** Name of DFC key store thread.  More useful than "DfcThread<N>" when debugging. */
_LIT(KKeyStoreThreadName, "HwKeyStore");
/** Same as KDfcThread0Priority in sinit.cpp.  Suitable for non-RT drivers (OSI ch3.) */
const TInt KKeyStoreThreadPriority = 27;
TDfcQue KeyStoreDfcQ;

template <typename T>
inline void RemoveAndCompress(RArray<T>& aContainer, TInt aIndex)
/**
	Remove the indexed item from the supplied array and compress
	the array.
	
	Without compression, the memory at the end of the array will
	not be freed.  Unless the memory is freed, memory leak testing
	will fail.
	
	@param	aContainer		Array from which item must be removed.
	@param	aIndex			Item to remove from array.
 */
	{
	aContainer.Remove(aIndex);
	aContainer.Compress();
	}

// -------- TStoredKey --------

XKeyStore::TStoredKey::TStoredKey(TKeyHandleImpl aHandle, const HBuf8* aData, TSecureId aOwner)
/**
	This constructor initializes all of the constant data fields.
	
	@param	aHandle			Identifies existing key in store.
	@param	aData			Heap-based descripor containing the key data.
	@param	aOwner			Process which owns the supplied key.
 */
:	iHandle(aHandle),
	iData(aData),
	iOwner(aOwner)
	{
	// empty.
	}

#ifdef _DEBUG
inline TBool Implies(TBool aLhs, TBool aRhs)
/**
	Implements boolean => operator.  Used only for debugging.

	@param	aLhs			Left argument.
	@param	aRhs			Right argument.
	@return					aLhs => aRhs
 */
	{
	return (! aLhs) || aRhs;
	}
#endif

TBool XKeyStore::TStoredKey::operator==(const TStoredKey& aRhs) const
/**
	Tests if the supplied stored keys are the same.
	
	Only the handles are compared because no two keys should have the
	same handle.
	
	@param	aRhs			Stored key to compare against this object.
	@return					Whether the two keys have the same handle.
 */
	{
	TBool same = (iHandle == aRhs.iHandle);

	__ASSERT_DEBUG(
		Implies(same, iData == aRhs.iData && iOwner == aRhs.iOwner),
		XKeyStore::Panic(XKeyStore::ESKEqualsDupHandle) );

	return same;
	}


// -------- create, delete keys --------

TInt XKeyStore::AddUserKeyToStore(HBuf8* aKeyData, TSecureId aOwner, TKeyHandleImpl& aHandle)
/**
	Add a user key to the hardware store.  This is a helper function
	for GenerateKey and StoreKey.
	
	@param	aKeyData		Initialized key data.  Ownership is transferred
							on entry to this function so the caller does not
							have to clean up if it fails.
	@param	aOwner			Identifies process which will own this key.
	@param	aHandle			On success this is set to the newly-created key's handle.
							Its value is undefined on failure.
	@return					KErrNone on success, any other error code on failure.
 */
	{
	// can shift address right without losing data because word-aligned
	TUint32 id = reinterpret_cast<TUint32>(aKeyData) >> 1;
	
	// ARMV5 UREL: Warning: C2874W: <kh.0> may be used before being set
	TKeyHandleImpl kh(TKeyHandleImpl::EUser, id);
	
	TStoredKey sk(kh, aKeyData, aOwner);
	
	aHandle = kh;
	TInt r = iUserKeys.Append(sk);
	if (r != KErrNone)
		delete aKeyData;
	
	return r;
	}

TInt XKeyStore::GenerateKey(TInt aLength, TSecureId aOwner, TKeyHandleImpl& aHandle)
/**
	Generate a new key and store it in the emulated hardware key store.

	@param	aLength			Length of new key in bytes.  Must be positive.
	@param	aOwner			Identifies process which will own this key.
	@param	aHandle			On success this is set to the newly-created key's
							implementation handle.
							Its value is undefined on failure.
	@return					KErrNone on success, any other error code on failure.
	@see HwKeyStore::GenerateKey
 */
	{
	__ASSERT_DEBUG(Invariant(), Panic(EGKPreInv));
	__ASSERT_DEBUG(aLength > 0, Panic(EGKInvalidLength));

	// aLength must be positive here (this has been ensured by the testing LDD.)
	// Do not need to test if >= KMaxTInt / 2, because New calls Kern::Alloc which
	// fails with NULL.  This contrasts with User::Alloc which panicks for the
	// same input.
	HBuf8* keyData = HBuf8::New(aLength);
	if (keyData == 0)
		return KErrNoMemory;
	keyData->SetLength(aLength);
	
	// populate the key with some deterministic data, to simplify debugging
	static TUint8 fillByte = 0;
	
	++fillByte;
	keyData->Fill(fillByte, aLength);

	// AUKTS deletes keyData on failure
	TInt r = AddUserKeyToStore(keyData, aOwner, aHandle);
	
	__ASSERT_DEBUG(Invariant(), Panic(EGKPostInv));
	return r;
	}

TBool FindStoredKeyByHandle(
	const TKeyHandleImpl* aHandle, const XKeyStore::TStoredKey& aStoredKey)
/**
	This function is defined so RArray::Find can locate a stored key by its
	handle.

	@param	aHandle			Handle to search for.
	@param	aStoredKey		Candidate stored key.
	@return					Whether the supplied handle identifies the supplied key.
 */
	{
	return *aHandle == aStoredKey.iHandle;
	}

TInt XKeyStore::StoreKey(const TDesC8& aData, TSecureId aOwner, TKeyHandleImpl& aHandle)
/**
	Store the supplied key in the emulated hardware key store.

	@param	aData			Unformatted key data.  This cannot be empty.
	@param	aOwner			Identifies process which will own this key.
	@param	aHandle			On success this is set to the newly-created key's
							implementation handle.
	@return					KErrNone on success, any other error code on failure.
	@see HwKeyStore::StoreKey
 */
	{
	__ASSERT_DEBUG(Invariant(), Panic(ESKPreInv));
	__ASSERT_DEBUG(aData.Length() > 0, Panic(ESKEmptyKey));

	HBuf8* storeKeyData = HBuf8::New(aData);
	if (storeKeyData == 0)
		return KErrNoMemory;

	// AUKTS deletes keyData on failure
	TInt r = AddUserKeyToStore(storeKeyData, aOwner, aHandle);
	
	__ASSERT_DEBUG(Invariant(), Panic(ESKPostInv));	
	return r;
	}

TInt XKeyStore::DeleteKey(TSecureId aClaimedOwner, TKeyHandleImpl aHandle)
/**
	Delete the supplied key from the store.

	@param	aClaimedOwner	Process which claims to own this key.  Only the process
							which generated or stored a key can delete it.
	@param	aHandle			Identifies existing key in store.
	@return					KErrNone on success, any other error code on failure.
							For example, KErrNotFound is returned if the handle does
							not identify a user-defined or -generated key.
	@see HwKeyStore::DeleteKey
 */
	{
	__ASSERT_DEBUG(Invariant(), Panic(EDKPreInv));
	
	TInt r;
	
	TInt idx = iUserKeys.Find(aHandle, FindStoredKeyByHandle);
	if (idx == KErrNotFound)
		r = KErrNotFound;
	else
		{
		TStoredKey& sk = iUserKeys[idx];
		// only the process which owns (i.e. created / stored) a key can delete it
		if (sk.iOwner != aClaimedOwner)
			r = KErrPermissionDenied;
		else
			{
			delete sk.iData;
			RemoveAndCompress(iUserKeys, idx);
			
			// remove any usages for this key
			for (TInt i = iUsages.Count() - 1; i >= 0; --i)
				{
				const TUsage& usage = iUsages[i];
				if (usage.iHandle == aHandle)
					RemoveAndCompress(iUsages, i);
				}
			
			r = KErrNone;
			}
		}
	
	__ASSERT_DEBUG(Invariant(), Panic(EDKPostInv));
	return r;
	}

const XKeyStore::TStoredKey* XKeyStore::FindStoredKey(TKeyHandleImpl aHandle) const
/**
	Find the stored key in either the user or built-in key collection.
	
	@param	aHandle			Identifies stored key to find.
	@return					Pointer to stored key, NULL if could not find.
							Ownership is not transferred to the caller.
 */
	{
	const RArray<TStoredKey>& sks =
		(aHandle.iCreator == TKeyHandleImpl::EBuiltIn) ? iBuiltInKeys : iUserKeys;
	
	TInt idx = sks.Find(aHandle, FindStoredKeyByHandle);
	if (idx == KErrNotFound)
		return 0;
	
	return &sks[idx];
	}

// -------- usage --------

TInt XKeyStore::AddUsage(TSecureId aClaimedOwner, TKeyHandleImpl aHandle, TInt aOperation, const TSecurityPolicy& aSecPol)
/**
	Add a usage policy for an existing key.  If a policy is already defined for the key
	and operation then it is overwritten.

	@param	aClaimedOwner	Process which claims to own this key.  Only the process
							which generated or stored a key can add a usage policy.
	@param	aHandle			Identifies existing key in store.
	@param	aOperation		Numeric identifier for operation.  Example operations
							include signing, but the actual meaning is not defined here.
	@param	aSecPol			Security policy to apply.  A process which wants to use the
							identified key for the identified operation must satisfy this
							security policy.  Only one policy can be defined for each
							key, operation pair.
	@return					KErrNone on success, any other error code on failure.
	@see HwKeyStore::AddUsage
 */
	{
	__ASSERT_DEBUG(Invariant(), Panic(EAUPreInv));
	
	// confirm the key exists
	const TStoredKey* sk = FindStoredKey(aHandle);
	
	if (sk == 0)
		return KErrNotFound;
	
	// does the claimed owner really own the key?
	if (sk->iOwner != aClaimedOwner)
		return KErrPermissionDenied;
	
	// if there is an existing usage entry for this key then modify it...	
	TUsage* usage = FindUsage(aHandle, aOperation);
	TInt r;
	if (usage != 0)
		{
		usage->iSecPol = aSecPol;
		r = KErrNone;
		}
	// ...else attempt to create a new entry
	else
		{
		TUsage newUsage = {aHandle, aOperation, aSecPol};
		r = iUsages.Append(newUsage);
		}
	
	__ASSERT_DEBUG(Invariant(), Panic(EAUPostInv));
	return r;
	}

TInt XKeyStore::DeleteUsage(TSecureId aClaimedOwner, TKeyHandleImpl aHandle, TInt aOperation)
/**
	Delete a usage policy for an existing key, operation pair.

	@param	aClaimedOwner	Process which claims to own this key.  Only the process
							which generated or stored a key can delete a usage policy.
	@param	aHandle			Identifies existing key in store.
	@param	aOperation		Numeric identifier for operation.  Example operations
							include signing, but the actual meaning is not defined here.
	@return					KErrNone on success, any other error code on failure.
	@see HwKeyStore::DeleteUsage
 */
	{
	__ASSERT_DEBUG(Invariant(), Panic(EDUPreInv));
	
	TInt r;
	// confirm an existing usage exists
	TInt usageIndex = GetUsageIndex(aHandle, aOperation);
	if (usageIndex == KErrNotFound)
		r = KErrNotFound;
	else
		{
		// confirm the claimed owner actually owns this key.
		// (The key must exist because there is a usage for it.)
		const TStoredKey* sk = FindStoredKey(aHandle);
		
		if (sk->iOwner != aClaimedOwner)
			r = KErrPermissionDenied;
		else
			{
			RemoveAndCompress(iUsages, usageIndex);
			r = KErrNone;
			}
		}
	
	__ASSERT_DEBUG(Invariant(), Panic(EDUPostInv));
	return r;
	}

TInt XKeyStore::UsageAllowed(DProcess* aProcess, TKeyHandleImpl aHandle, TInt aOperation, TBool& aAllowed)
/**
	Predicate function tests whether the supplied process matches the usage policy
	for the identified key and operation.

	@param	aProcess		Process which wants to use the key.
	@param	aHandle			Identifies existing key in store.
	@param	aOperation		Numeric identifier for operation.  Example operations
							include signing, but the actual meaning is not defined here.
	@param	aAllowed		On success this is set to whether or not the process can
							perform the requested operation with the key.  Its value
							is undefined on failure.
	@return					KErrNone on success, any other error code on failure.
							In this context, "success" means can determine whether or
							not the operation is allowed.  It does not necessarily mean
							the operation <em>is</em> allowed.
	@see HwKeyStore::UsageAllowed
 */
	{
	__ASSERT_DEBUG(Invariant(), Panic(EUAPreInv));
	
	TUsage* usage = FindUsage(aHandle, aOperation);
	if (usage == 0)
		aAllowed = EFalse;
	else	
		aAllowed = usage->iSecPol.CheckPolicy(aProcess, __PLATSEC_DIAGNOSTIC_STRING("XKeyStore::IsUsageAllowed"));
	
	__ASSERT_DEBUG(Invariant(), Panic(EUAPostInv));
	return KErrNone;
	}

XKeyStore::TUsage* XKeyStore::FindUsage(TKeyHandleImpl aHandle, TInt aOperation)
/**
	Find the usage entry for the supplied key and operation.

	@param	aHandle			Identifies existing key in store.
	@param	aOperation		Numeric identifier for operation.  Example operations
							include signing, but the actual meaning is not defined here.
	@return					Pointer to existing usage, NULL if the usage is not defined.
 */
	{
	TInt idx = GetUsageIndex(aHandle, aOperation);
	
	return (idx == KErrNotFound) ? NULL : &iUsages[idx];
	}

TInt XKeyStore::GetUsageIndex(TKeyHandleImpl aHandle, TInt aOperation) const
/**
	Get the supplied usage's index.

	@param	aHandle			Identifies existing key in store.
	@param	aOperation		Numeric identifier for operation.  Example operations
							include signing, but the actual meaning is not defined here.
	@return					The supplied usage's index in iUsages, KErrNotFound if
							it is not defined.
 */
	{
	TInt idxUsage;
	for (idxUsage = iUsages.Count() - 1; idxUsage >= 0; --idxUsage)
		{
		const TUsage& usage = iUsages[idxUsage];
		if (usage.iHandle == aHandle && usage.iOperation == aOperation)
			return idxUsage;
		}
	
	return KErrNotFound;
	}

// -------- extraction --------

TInt XKeyStore::ExtractKey(DProcess* aProcess, TKeyHandleImpl aHandle, TInt aOperation, HBuf8*& aKeyData)
/**
	Extract the supplied key from the emulated hardware store.
	
	@param	aProcess		Process (accelerator client) which wants to use the key.
	@param	aHandle			Key which the process wants to use.
	@param	aOperation		Operation which the process wants to perform with the key.
	@param	aKeyData		On success this is set to a heap-based descriptor which
							contains the key data.  Ownership is transferred to the caller.
							Its value is undefined on failure.
	@return					Symbian OS error code, whether KErrNone means
							success and any other value means failure.
	@see HwKeyStore::ExtractKey
 */
	{
	// this pair of lookups scan the list of usages twice.  This is inefficient, but
	// makes for clearer code.
	
	TBool allowed;
	TInt r = UsageAllowed(aProcess, aHandle, aOperation, allowed);
	if (r != KErrNone)
		return r;
	if (! allowed)
		return KErrPermissionDenied;
	
	// usage is allowed so allocate a copy of the key data
	const TUsage* usage = FindUsage(aHandle, aOperation);
	const TStoredKey* sk = FindStoredKey(usage->iHandle);
	
	NKern::ThreadEnterCS();
	aKeyData = HBuf8::New(*sk->iData);
	NKern::ThreadLeaveCS();
	if (aKeyData == 0)
		return KErrNoMemory;
	
	return KErrNone;
	}

// -------- debugging --------

#ifdef _DEBUG

// templated functions can't be declared as static, but don't want to export this
namespace {

template <typename T>
TBool AnyDups(const RArray<T>& aCollection)
/**
	Tests whether each item in the supplied array is unique,
	where items are compared with the == operator.
	
	Time: theta(#aCollection)
	
	@param	aCollection		Array of items to test.
	@return					Whether each there are any duplicates items in
							the supplied array, according to the == operator.
 */
	{
	TInt itemCount = aCollection.Count();
	for (TInt i = 0; i < itemCount - 1; ++i)
		{
		const T& itemI = aCollection[i];
		
		for (TInt j = i + 1; j < itemCount; ++j)
			{
			const T& itemJ = aCollection[j];
			
			if (itemI == itemJ)
				return ETrue;
			}
		}
	
	return EFalse;
	}

}	// namespace {

TBool XKeyStore::VerifyStoredKeyCreators(
	const RArray<TStoredKey>& aStoredKeys, TKeyHandleImpl::TCreator aExpectedCreator) const
/**
	Verify whether each key in the supplied collection has the matching
	creator value in its handle.
	
	This function is used to verify that only built-in keys are stored
	in iBuiltInKeys and only user keys in iUserKeys.
	
	@param	aStoredKeys		Collection of stored keys to test.
	@param	aExpectedCreator Every stored key in aStoredKeys should have this creator.
	@return					Whether every key in the supplied collection has
							the expected creator.
 */
	{
	TInt keyCount = aStoredKeys.Count();
	for (TInt i = 0; i < keyCount; ++i)
		{
		const XKeyStore::TStoredKey& sk = aStoredKeys[i];
		if (sk.iHandle.iCreator != aExpectedCreator)
			return EFalse;
		}
	
	return ETrue;
	}

TBool XKeyStore::Invariant() const
/**
	This function is defined for debug builds.  It tests whether
	the store is valid.  Specifically, it tests the following.
	
	1. There are no duplicate built-in keys.
	2. All of the built-in keys have a built-in handle.
	3. There are no duplicate user keys.
	4. All of the user keys have a user handle.
	5. Every usage refers to an existing key.
	6. No key, operation pair is used by more than one usage.

	@return					Whether this store is valid.
 */
	{
	if (AnyDups(iBuiltInKeys))
		return EFalse;
	
	if (! VerifyStoredKeyCreators(iBuiltInKeys, TKeyHandleImpl::EBuiltIn))
		return EFalse;
	
	if (AnyDups(iUserKeys))
		return EFalse;
	
	if (! VerifyStoredKeyCreators(iBuiltInKeys, TKeyHandleImpl::EUser))
		return EFalse;
	
	TInt usageCount = iUsages.Count();
	for (TInt i = 0; i < usageCount; ++i)
		{
		const TUsage& usageI = iUsages[i];

		// verify usage refers to an existing key
		TKeyHandleImpl khiI = usageI.iHandle;
		const TStoredKey* sk = FindStoredKey(khiI);
		if (sk == 0)
			return EFalse;

		// verify no other usage describes the same key and operation
		for (TInt j = i + 1; j < usageCount; ++j)
			{
			const TUsage& usageJ = iUsages[j];
			if (usageI.iHandle == usageJ.iHandle && usageI.iOperation == usageJ.iOperation)
				return EFalse;
			}
		}
	
	return ETrue;
	}

void XKeyStore::Panic(XKeyStore::TPanic aPanic)
/**
	Terminate the current thread with category "XKeyStore"
	and the supplied reason.
	
	@param	aPanic			This argument's numeric value
							is used as the panic reason.
 */
	{
	_LIT(KPanicCat, "XKeyStore");
	Kern::PanicCurrentThread(KPanicCat, aPanic);
	}

#endif	// #ifdef _DEBUG


// -------- entrypoint --------

DECLARE_STANDARD_EXTENSION()
/**
	Allocate resources required by this kernel extension.
	Specifically, allocates a DFC thread on which requests
	will run.
 */
	{
	TInt r = Kern::DfcQInit(&KeyStoreDfcQ, KKeyStoreThreadPriority, &KKeyStoreThreadName);
	
	return r;
	}

