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
* Structures used in emulated key store implementation.  Provides
* the implementation for the HwKeyStore interface.
*
*/




/**
 @file 
 @internalComponent
*/

#ifndef KMSKEXT_H
#define KMSKEXT_H

#include <kernel/kernel.h>
#include <kernel/klib.h>

#include "hwkeystore.h"
#include "kmsldd.h"


class TKeyHandleImpl
/**
	Instances of TKeyHandle are translated to an object of this type,
	which the extension uses to manage the keys in the simulated hardware
	store.
 */
	{
public:
	enum TCreator
	/** Whether a key was built into hardware or created by the user. */
		{
		EBuiltIn = 0,		///< Predefined key in hardware.  Not used.
		EUser = 1			///< User key stored in hardware.
		};
	/** Key store uses this to identify which collection to search in. */
	TCreator iCreator:1;
	/**
		Bits 30 to 0 hold the address of the descriptor which holds the key
		data, divided by two.  This value is not used as a pointer, just an
		easy-to-calculate unique identifier.
	 */
	TUint iIdentifier:31;

public:
	inline TKeyHandleImpl(TKeyHandle aHandle);
	inline TKeyHandleImpl(TCreator aCreator, TUint aIdentifier);

	inline TBool operator==(const TKeyHandleImpl& aRhs) const;
	};

class XKeyStore
/**
	A singleton instance of this class holds the keys
	which are stored in hardware, and their usage mappings.
 */
	{
public:
	TInt GenerateKey(TInt aLength, TSecureId aOwner, TKeyHandleImpl& aHandleImpl);
	TInt StoreKey(const TDesC8& aData, TSecureId aOwner, TKeyHandleImpl& aHandle);
	TInt DeleteKey(TSecureId aClaimedOwner, TKeyHandleImpl aHandle);

	TInt AddUsage(TSecureId aClaimedOwner, TKeyHandleImpl aHandle, TInt aOperation, const TSecurityPolicy& aSecPol);
	TInt DeleteUsage(TSecureId aClaimedOwner, TKeyHandleImpl aHandle, TInt aOperation);
	TInt UsageAllowed(DProcess* aProcess, TKeyHandleImpl aHandle, TInt aOperation, TBool& aAllowed);
	
	TInt ExtractKey(DProcess* aProcess, TKeyHandleImpl aHandle, TInt aOperation, HBuf8*& aKeyData);
	
private:
	TInt AddUserKeyToStore(HBuf8* aKeyData, TSecureId aOwner, TKeyHandleImpl& aHandle);

private:
	class TStoredKey
	/** Maps a handle to the key data. */
		{
	public:
		TStoredKey(TKeyHandleImpl aHandle, const HBuf8* aData, TSecureId aOwner);
		TBool operator==(const TStoredKey& aRhs) const;

	public:
		const TKeyHandleImpl iHandle;			///< Corresponds to user-side TKeyHandle identifier.
		const HBuf8*const iData;				///< The actual key data, stored on the supervisor heap.
		const TSecureId iOwner;					///< Process which owns the key.
		};
	
	RArray<TStoredKey> iBuiltInKeys;			///< Which keys are built into the hardware.
	RArray<TStoredKey> iUserKeys;				///< Which keys the user has added to the store.
	
	friend TBool FindStoredKeyByHandle(
		const TKeyHandleImpl* aHandle, const XKeyStore::TStoredKey& aStoredKey);
	const TStoredKey* FindStoredKey(TKeyHandleImpl aHandle) const;
	
	class TUsage
	/**
		Associates a security policy with a key handle, operation pair.  A process
		which wants to use the target key for the target operation must satisfy the
		security policy.  This must be checked by the driver which performs the operation.
	 */
		{
	public:
		TKeyHandleImpl iHandle;					///< Corresponds to user-side TKeyHandle identifier.
		TInt iOperation;						///< Identifies requested operation, although interpretation of value not defined here.
		TSecurityPolicy iSecPol;				///< Security policy which using process must satisfy.
		};
	RArray<TUsage> iUsages;						///< Collection of defined usages.
	
	TUsage* FindUsage(TKeyHandleImpl aHandle, TInt aOperation);
	TInt GetUsageIndex(TKeyHandleImpl aHandle, TInt aOperation) const;
	
#ifdef _DEBUG
	enum TPanic
	/** Invalid states for this object. */
		{
		EGKPreInv = 0x00, EGKPostInv, EGKInvalidLength,
		ESKPreInv = 0x10, ESKPostInv, ESKEmptyKey,
		EDKPreInv = 0x20, EDKPostInv,
		EAUPreInv = 0x30, EAUPostInv,
		EDUPreInv = 0x40, EDUPostInv,
		EUAPreInv = 0x50, EUAPostInv,
		ESKEqualsDupHandle = 0x60
		};
	static void Panic(TPanic aPanic);
	
	TBool VerifyStoredKeyCreators(
		const RArray<TStoredKey>& aStoredKeys,
		TKeyHandleImpl::TCreator aExpectedCreator) const;
	TBool Invariant() const;

	friend class TStoredKey;			// provide access to Panic function.
#endif	// #ifdef _DEBUG
	};


/** The singleton instance of XKeyStore. */
extern XKeyStore KeyStore;
/** All key store requests must be run on this thread. */
extern TDfcQue KeyStoreDfcQ;

#include "kmskext.inl"

#endif	// #ifndef KMSKEXT_H

