// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "clirep.h"

/** Creates a CRepository object for accessing a repository.
If there is no such repository, the function leaves with KErrNotFound.
A pointer to the object is left on the cleanup stack.
@param aRepositoryUid The UID of the repository to be accessed
@return A pointer to a newly created CRepository object
*/
EXPORT_C CRepository* CRepository::NewLC(TUid aRepositoryUid)
	{
	return CClientRepository::NewLC(aRepositoryUid);
	}


/** Creates a CRepository object for accessing a repository.
If there is no such repository, the function leaves with KErrNotFound.
@param aRepositoryUid The UID of the repository to be accessed
@return A pointer to a newly created CRepository object
*/
EXPORT_C CRepository* CRepository::NewL(TUid aRepositoryUid)
	{
	CRepository* rep = CRepository::NewLC(aRepositoryUid);
	CleanupStack::Pop();
	return rep;
	}


/** Destructor. 
*/
EXPORT_C CRepository::~CRepository()
	{
	}


/** Creates a new setting with an integer value.
@param aKey New setting key.
@param aValue Setting value.
@return
	KErrNone if successful,
	KErrAbort if in a transaction that has previously failed
	KErrPermissionDenied if caller fails capability check,
	KErrAlreadyExists if a setting with that key already exists
	plus other system-wide error codes.
@post
	Transactions fail on all error conditions.
	Outside transactions: on success the new setting is persistent,
	on failure the repository is unmodified.
@capability Dependent Caller must satisfy the write access policy of that key in the repository.
*/
EXPORT_C TInt CRepository::Create(TUint32 aKey, TInt aValue)
	{
	return (static_cast<CClientRepository*>(this))->Create(aKey, aValue);
	}


/** Creates a new setting with a floating-point value.
@param aKey New setting key.
@param aValue Setting value.
@return
	KErrNone if successful,
	KErrAbort if in a transaction that has previously failed
	KErrPermissionDenied if caller fails capability check,
	KErrAlreadyExists if a setting with that key already exists
	plus other system-wide error codes.
@post
	Transactions fail on all error conditions.
	Outside transactions: on success the new setting is persistent,
	on failure the repository is unmodified.
@capability Dependent Caller must satisfy the write access policy of that key in the repository.
*/
EXPORT_C TInt CRepository::Create(TUint32 aKey, const TReal& aValue)
	{
	return (static_cast<CClientRepository*>(this))->Create(aKey, aValue);
	}


/** Creates a new setting with a descriptor value.
@param aKey New setting key.
@param aValue Setting value.
@return
	KErrNone if successful,
	KErrAbort if in a transaction that has previously failed
	KErrPermissionDenied if caller fails capability check,
	KErrAlreadyExists if a setting with that key already exists
	KErrArgument if the descriptor is longer than KMaxBinaryLength,
	plus other system-wide error codes.
@post
	Transactions fail on all error conditions.
	Outside transactions: on success the new setting is persistent,
	on failure the repository is unmodified.
@capability Dependent Caller must satisfy the write access policy of that key in the repository.
*/
EXPORT_C TInt CRepository::Create(TUint32 aKey, const TDesC8& aValue)
	{
	return (static_cast<CClientRepository*>(this))->Create(aKey, aValue);
	}

/** Creates a new setting with a descriptor value.
@param aKey New setting key.
@param aValue Setting value.
@return
	KErrNone if successful,
	KErrAbort if in a transaction that has previously failed
	KErrPermissionDenied if caller fails capability check,
	KErrAlreadyExists if a setting with that key already exists
	KErrArgument if the descriptor is longer than KMaxUnicodeStringLength,
	plus other system-wide error codes.
@post
	Transactions fail on all error conditions.
	Outside transactions: on success the new setting is persistent,
	on failure the repository is unmodified.
@capability Dependent Caller must satisfy the write access policy of that key in the repository.
*/
EXPORT_C TInt CRepository::Create(TUint32 aKey, const TDesC16& aValue)
	{
	return (static_cast<CClientRepository*>(this))->Create(aKey, aValue);
	}

/** Deletes a setting.
@param aKey Key of setting to be deleted.
@return
	KErrNone if successful, 
	KErrAbort if in a transaction that has previously failed
	KErrPermissionDenied if caller fails capability check,
	KErrNotFound if the setting does not exist,
	plus other system-wide error codes.
@post
	Transactions fail on all error conditions except KErrNotFound.
	Outside transactions: on success the deletion of the setting is persistent,
	on failure the repository is unmodified.
@capability Dependent Caller must satisfy the write access policy for that key in the repository
*/
EXPORT_C TInt CRepository::Delete(TUint32 aKey)
	{
	return (static_cast<CClientRepository*>(this))->Delete(aKey);
	}
	
/** Deletes all the settings that exist and match the specification:
	(key & mask) == (partialKey & mask)
Partial key is guaranteed to be masked before use.
Examples of use:
- To delete a single key.
	Delete(key, 0xFFFFFFFF, errorKey);
- To delete all keys from 0 to 0xFF:
	Delete(0, 0xFFFFFF00, errorKey);
	(digits from 0 to 0xFF would be ignored if given in the partial key)
- To delete all keys matching 0x5B??3A?6:
	Delete(0x5B003A06, 0xFF00FF0F, errorKey);

@param aPartialKey
	Contains a bit pattern that all the keys must at least partially
	match.
@param aMask
	Has bits set for all the bits in aPartialKey that must match the keys being deleted.
@param aErrorKey If the delete operation fails this contains the key involved in the
	failure, or aPartialKey or KUnspecifiedKey if it could not be attributed to any key
@return
	KErrNone if successful,
	KErrAbort if in a transaction that has previously failed
	KErrNotFound if no items were found in the partial key range.
	KErrPermissionDenied if caller fails capability check.
	plus other system-wide error codes.
@post
	Transactions fail on all error conditions except KErrNotFound
	Outside transactions: on success the changes are persistent, on failure the
	repository is unmodified.
@capability Dependent Caller must satisfy the write policies of all settings found in the
					  partial key range.
*/	
EXPORT_C TInt CRepository::Delete (TUint32 aPartialKey, TUint32 aMask, TUint32 &aErrorKey)
	{
	return (static_cast<CClientRepository*>(this))->Delete(aPartialKey, aMask, aErrorKey);
	}


/** Reads an integer setting.
@param aKey Key of setting to be read.
@param aValue Returns the value of the setting if it is an integer.
@return
	KErrNone if successful,
	KErrAbort if in a transaction that has previously failed
	KErrPermissionDenied if caller fails capability check,
	KErrNotFound if the setting does not exist,
	KErrArgument if the setting exists but is not an integer,
	plus other system-wide error codes.
@post Transactions fail only on those "other system-wide error codes".
@capability Dependent Caller must satisfy the read access policy of that key in the repository.
*/
EXPORT_C TInt CRepository::Get(TUint32 aKey, TInt& aValue)
	{
	return (static_cast<CClientRepository*>(this))->Get(aKey, aValue);
	}


/** Sets an existing integer setting to a new value or creates a new setting 
with an integer value if the setting doesn't exist.
@param aKey Key of setting to be written to.
@param aValue Value to be written.
@return
	KErrNone if successful,
	KErrAbort if in a transaction that has previously failed
	KErrPermissionDenied if caller fails capability check,
	KErrArgument if the setting exists but is not an integer
	plus other system-wide error codes.
@post
	Transactions fail on all error conditions.
	Outside transactions: on success the new value is persistent,
	on failure the repository is unmodified.
@capability Dependent Caller must satisfy the write access policy of that key in the repository.
*/
EXPORT_C TInt CRepository::Set(TUint32 aKey, TInt aValue)
	{
	return (static_cast<CClientRepository*>(this))->Set(aKey, aValue);
	}


/** Reads a floating point setting.
@param aKey Key of setting to be read.
@param aValue Returns the value of the setting if it is a floating point value.
@return
	KErrNone if successful,
	KErrAbort if in a transaction that has previously failed
	KErrPermissionDenied if caller fails capability check,
	KErrNotFound if the setting does not exist,
	KErrArgument if the setting exists but is not a floating point value,
	plus other system-wide error codes.
@post Transactions fail only on those "other system-wide error codes".
@capability Dependent Caller must satisfy the read access policy of that key in the repository.
*/
EXPORT_C TInt CRepository::Get(TUint32 aKey, TReal& aValue)
	{
	return (static_cast<CClientRepository*>(this))->Get(aKey, aValue);
	}


/** Sets an existing floating point setting to a new value or creates a new setting
with a floating point value if the setting doesn't exist.
@param aKey Key of setting to be written to.
@param aValue Value to be written.
@return
	KErrNone if successful,
	KErrAbort if in a transaction that has previously failed
	KErrPermissionDenied if caller fails capability check,
	KErrArgument if the setting exists but is not a floating point value
	plus other system-wide error codes.
@post
	Transactions fail on all error conditions.
	Outside transactions: on success the new value is persistent,
	on failure the repository is unmodified.
@capability Dependent Caller must satisfy the write access policy of that key in the repository.
*/
EXPORT_C TInt CRepository::Set(TUint32 aKey, const TReal& aValue)
	{
	return (static_cast<CClientRepository*>(this))->Set(aKey, aValue);
	}

	
/** Reads a descriptor setting.
@param aKey Key of setting to be read.
@param aValue Returns the value of the setting if it is a descriptor.
@return
	KErrNone if successful,
	KErrAbort if in a transaction that has previously failed
	KErrPermissionDenied if caller fails capability check,
	KErrNotFound if the setting does not exist,
	KErrArgument if the setting exists but is not a descriptor,
	KErrOverflow if the descriptor is too small to receive the value in the repository,
	plus other system-wide error codes.
@post Transactions fail only on those "other system-wide error codes".
@capability Dependent Caller must satisfy the read access policy of that key in the repository.
*/
EXPORT_C TInt CRepository::Get(TUint32 aKey, TDes8& aValue)
	{
	return (static_cast<CClientRepository*>(this))->Get(aKey, aValue);
	}

/** Reads a descriptor setting.
@param aKey Key of setting to be read.
@param aValue Returns the value of the setting if it is a descriptor.
@param aActualLength Returns the actual length of the setting if it is a descriptor.
@return
	KErrNone if successful,
	KErrAbort if in a transaction that has previously failed
	KErrPermissionDenied if caller fails capability check,
	KErrNotFound if the setting does not exist,
	KErrArgument if the setting exists but is not a descriptor,
	KErrOverflow if the descriptor is too small to receive the value in the repository,
	plus other system-wide error codes.
@post Transactions fail only on those "other system-wide error codes".
@capability Dependent Caller must satisfy the read access policy of that key in the repository.
*/
EXPORT_C TInt CRepository::Get(TUint32 aKey, TDes8& aValue, TInt& aActualLength)
	{
	return (static_cast<CClientRepository*>(this))->Get(aKey, aValue, aActualLength);
	}

/** Sets an existing descriptor setting to a new value or creates a new setting
with a descriptor value if the setting doesn't exist.
@param aKey Key of setting to be written to.
@param aValue Value to be written.
@return
	KErrNone if successful,
	KErrAbort if in a transaction that has previously failed
	KErrPermissionDenied if caller fails capability check,
	KErrArgument if aValue is longer than KMaxBinaryLength or
	the setting exists but is not a descriptor,
	plus other system-wide error codes.
@post
	Transactions fail on all error conditions.
	Outside transactions: on success the new value is persistent,
	on failure the repository is unmodified.
@capability Dependent Caller must satisfy the write access policy of that key in the repository.
*/
EXPORT_C TInt CRepository::Set(TUint32 aKey, const TDesC8& aValue)
	{
	return (static_cast<CClientRepository*>(this))->Set(aKey, aValue);
	}


/** Reads a descriptor setting.
@param aKey Key of setting to be read.
@param aValue Returns the value of the setting if it is a descriptor.
@return
	KErrNone if successful,
	KErrAbort if in a transaction that has previously failed
	KErrPermissionDenied if caller fails capability check,
	KErrNotFound if the setting does not exist,
	KErrArgument if the setting exists but is not a descriptor,
	KErrOverflow if the descriptor is too small to receive the value in the repository,
	plus other system-wide error codes.
@post Transactions fail only on those "other system-wide error codes".
@capability Dependent Caller must satisfy the read access policy of that key in the repository.
*/
EXPORT_C TInt CRepository::Get(TUint32 aKey, TDes16& aValue)
	{
	return (static_cast<CClientRepository*>(this))->Get(aKey, aValue);
	}

/** Reads a descriptor setting.
@param aKey Key of setting to be read.
@param aValue Returns the value of the setting if it is a descriptor.
@param aActualLength Returns the actual length of the setting if it is a descriptor.
@return
	KErrNone if successful,
	KErrAbort if in a transaction that has previously failed
	KErrPermissionDenied if caller fails capability check,
	KErrNotFound if the setting does not exist,
	KErrArgument if the setting exists but is not a descriptor,
	KErrOverflow if the descriptor is too small to receive the value in the repository,
	plus other system-wide error codes.
@post Transactions fail only on those "other system-wide error codes".
@capability Dependent Caller must satisfy the read access policy of that key in the repository.
*/
EXPORT_C TInt CRepository::Get(TUint32 aKey, TDes16& aValue, TInt& aActualLength)
	{
	return (static_cast<CClientRepository*>(this))->Get(aKey, aValue, aActualLength);
	}

/** Sets an existing descriptor setting to a new value or creates a new setting
with a descriptor value if it doesn't exist.
@param aKey Key of setting to be written to.
@param aValue Value to be written.
@return
	KErrNone if successful,
	KErrAbort if in a transaction that has previously failed
	KErrPermissionDenied if caller fails capability check,
	KErrArgument if aValue is longer than KMaxUnicodeStringLength or
	the setting exists but is not a descriptor,
	plus other system-wide error codes.
@post
	Transactions fail on all error conditions.
	Outside transactions: on success the new value is persistent,
	on failure the repository is unmodified.
@capability Dependent Caller must satisfy the write access policy of that key in the repository.
*/
EXPORT_C TInt CRepository::Set(TUint32 aKey, const TDesC16& aValue)
	{
	return (static_cast<CClientRepository*>(this))->Set(aKey, aValue);
	}

/** Reads the metadata bound to a key
@param aKey The key
@param aMeta Returns the metadata value for the key
@return
	KErrNone if successful,
	KErrAbort if in a transaction that has previously failed
	KErrPermissionDenied if caller fails capability check,
	KErrNotFound if the setting does not exist,
	plus other system-wide error codes.
@post Transactions fail only on those "other system-wide error codes".
@capability Dependent Caller must satisfy the read access policy of that key in the repository.
*/
EXPORT_C TInt CRepository::GetMeta(TUint32 aKey, TUint32& aMeta)
	{
	return (static_cast<CClientRepository*>(this))->GetMeta(aKey, aMeta);
	}

/** Moves all the settings that exist and match the specification:
	(oldkey & mask) == (sourcePartialKey & mask)
to new locations where 
	(newkey & mask) == (targetPartialKey & mask).
For those keys that match the source specification, those bits in the key for
which the corresponding mask bit is zero remain unchanged. All remaining bits
change from (sourcePartialKey & mask) to (targetPartialKey & mask).
Both partial keys are guaranteed to be masked before use.
Examples of use:
- To move a single key from oldKey to newKey.
	Move(oldKey, newKey, 0xFFFFFFFF, errorKey);
- To move all keys from 0 to 0xFF to be from 0x100 to 0x1FF:
	Move(0, 0x100, 0xFFFFFF00, errorKey);
	(digits from 0 to 0xFF would be ignored if given in the partial keys)
- To move all keys matching 0x5B??3A?6 to 0xDC??44?F:
	Move(0x5B003A06, 0xDC00440F, 0xFF00FF0F, errorKey);

@param aSourcePartialKey
	Contains a bit pattern that all the source keys which must at least partially
	match.
@param aMask
	Has bits set for all the bits in aPartialKey that must match the keys being moved.
@param aTargetPartialKey
	Contains a bit pattern that all the target keys which must at least partially
	match.	
@param aErrorKey on failure, contains the key or partial key involved in the error
	or KUnspecifiedKey if failure could not be attributed to any key.
@return
	KErrNone if successful,
	KErrAbort if in a transaction that has previously failed
	KErrNotFound if no items were found in the source range.
	KErrPermissionDenied if caller fails capability check,
	KErrAlreadyExists if an existing setting is using any intended target key.
	plus other system-wide error codes.
@post
	Transactions fail on all error conditions except KErrNotFound.
	Outside transactions: on success the changes are persistent, on failure the
	repository is unmodified.
@capability Dependent Caller must satisfy the read and write policies of all settings found in the
					  source range, and write policies on all intended target keys.
*/
EXPORT_C TInt CRepository::Move (TUint32 aSourcePartialKey, TUint32 aTargetPartialKey, 
								 TUint32 aMask, TUint32 &aErrorKey)
	{
	return (static_cast<CClientRepository*>(this))->Move(aSourcePartialKey, aTargetPartialKey, aMask, aErrorKey);
	}

	
/** Finds all the settings that exist and match the specification given by
aPartialKey and aMask.
Matches occur whenever (key & mask) == (partialKey & mask).
The partial key is guaranteed to be masked before use.
@param aPartialKey
	Contains a bit pattern that all the keys returned must at least partially
	match.
@param aMask
	Has bits set for all the bits in aPartialKey that must match the returned
	keys.
@param aFoundKeys All the keys found.
@return
	KErrNone if successful,
	KErrAbort if in a transaction that has previously failed
	KErrNotFound if no items were found in the source range,
	plus other system-wide error codes.
@post Transactions fail only on those "other system-wide error codes".
*/
EXPORT_C TInt CRepository::FindL(TUint32 aPartialKey, TUint32 aMask,
	RArray<TUint32>& aFoundKeys)
	{
	return (static_cast<CClientRepository*>(this))->FindL(aPartialKey, aMask, aFoundKeys);
	}


/** Finds all the settings that contain a given integer and match the
specification given by aPartialKey and aMask.
@param aPartialKey
	Contains a bit pattern that all the keys returned must at least partially
	match.
@param aMask
	Has bits set for all the bits in aPartialKey that must match the returned
	keys.
@param aValue Settings for the keys found will be integers with value aValue.
@param aFoundKeys All the keys found.
	For each key k in aFoundKeys, (k & aMask) == (aPartialKey & aMask) and
	the setting with key k is an integer aValue.
@see FindL()
@return
	KErrNone if successful,
	KErrAbort if in a transaction that has previously failed
	KErrPermissionDenied if caller fails capability check,
	KErrNotFound if capability check passed but no matching items are found,
	plus other system-wide error codes.
@post Transactions fail only on those "other system-wide error codes".
@capability Dependent Caller must satisfy the read policies of all settings found in the source range.
*/
EXPORT_C TInt CRepository::FindEqL(TUint32 aPartialKey, TUint32 aMask,
	TInt aValue, RArray<TUint32>& aFoundKeys)
	{
	return (static_cast<CClientRepository*>(this))->FindEqL(aPartialKey, aMask, aValue, aFoundKeys);
	}


/** Finds all the settings that contain a given floating point value and match
the specification given by aPartialKey and aMask.
@param aPartialKey
	Contains a bit pattern that all the keys returned must at least partially
	match.
@param aMask
	Has bits set for all the bits in aPartialKey that must match the returned
	keys.
@param aValue
	Settings for the keys found will be floating point values with value aValue.
@param aFoundKeys All the keys found.
	For each key k in aFoundKeys, (k & aMask) == (aPartialKey & aMask) and
	the setting with key k is a floating point value aValue.
@see FindL()
@return
	KErrNone if successful,
	KErrAbort if in a transaction that has previously failed
	KErrPermissionDenied if caller fails capability check,
	KErrNotFound if capability check passed but no matching items are found,
	plus other system-wide error codes.
@post Transactions fail only on those "other system-wide error codes".
@capability Dependent Caller must satisfy the read policies of all settings found in the source range.
*/
EXPORT_C TInt CRepository::FindEqL(TUint32 aPartialKey, TUint32 aMask,
	const TReal& aValue, RArray<TUint32>& aFoundKeys)
	{
	return (static_cast<CClientRepository*>(this))->FindEqL(aPartialKey, aMask, aValue, aFoundKeys);
	}


/** Finds all the settings that contain a given string value and match the
specification given by aPartialKey and aMask.
@param aPartialKey
	Contains a bit pattern that all the keys returned must at least partially
	match.
@param aMask
	Has bits set for all the bits in aPartialKey that must match the returned
	keys.
@param aValue
	Settings for the keys found will be string values with value aValue.
@param aFoundKeys All the keys found.
	For each key k in aFoundKeys, (k & aMask) == (aPartialKey & aMask) and
	the setting with key k is a string value aValue.
@see FindL()
@return
	KErrNone if successful,
	KErrAbort if in a transaction that has previously failed
	KErrPermissionDenied if caller fails capability check,
	KErrNotFound if capability check passed but no matching items are found,
	plus other system-wide error codes.
@post Transactions fail only on those "other system-wide error codes".
@capability Dependent Caller must satisfy the read policies of all settings found in the source range.
*/
EXPORT_C TInt CRepository::FindEqL(TUint32 aPartialKey, TUint32 aMask,
	const TDesC8& aValue, RArray<TUint32>& aFoundKeys)
	{
	return (static_cast<CClientRepository*>(this))->FindEqL(aPartialKey, aMask, aValue, aFoundKeys);
	}


/** Finds all the settings that contain a given string value and match the
specification given by aPartialKey and aMask.
@param aPartialKey
	Contains a bit pattern that all the keys returned must at least partially
	match.
@param aMask
	Has bits set for all the bits in aPartialKey that must match the returned
	keys.
@param aValue
	Settings for the keys found will be string values with value aValue.
@param aFoundKeys All the keys found.
	For each key k in aFoundKeys, (k & aMask) == (aPartialKey & aMask) and
	the setting with key k is a string value aValue.
@see FindL()
@return
	KErrNone if successful,
	KErrAbort if in a transaction that has previously failed
	KErrPermissionDenied if caller fails capability check,
	KErrNotFound if capability check passed but no matching items are found,
	plus other system-wide error codes.
@post Transactions fail only on those "other system-wide error codes".
@capability Dependent Caller must satisfy the read policies of all settings found in the source range.
*/
EXPORT_C TInt CRepository::FindEqL(TUint32 aPartialKey, TUint32 aMask,
	const TDesC16& aValue, RArray<TUint32>& aFoundKeys)
	{
	return (static_cast<CClientRepository*>(this))->FindEqL(aPartialKey, aMask, aValue, aFoundKeys);
	}


/** Finds all the settings that match the specification given by aPartialKey
and aMask, but are either not integer values or do not have the given value.
@param aPartialKey
	Contains a bit pattern that all the keys returned must at least partially
	match.
@param aMask
	Has bits set for all the bits in aPartialKey that must match the returned
	keys.
@param aValue
	Settings for the keys found will be settings that either contain values
	that are not integers or integers other than aValue.
@param aFoundKeys All the keys found.
	For each key k in aFoundKeys, (k & aMask) == (aPartialKey & aMask) and
	the setting with key k is either not an integer or an integer not equal to
	aValue.
@see FindL()
@return
	KErrNone if successful,
	KErrAbort if in a transaction that has previously failed
	KErrPermissionDenied if caller fails capability check,
	KErrNotFound if capability check passed but no non-matching items are found,
	plus other system-wide error codes.
@post Transactions fail only on those "other system-wide error codes".
@capability Dependent Caller must satisfy the read policies of all settings found in the source range.
*/
EXPORT_C TInt CRepository::FindNeqL(TUint32 aPartialKey, TUint32 aMask,
	TInt aValue, RArray<TUint32>& aFoundKeys)
	{
	return (static_cast<CClientRepository*>(this))->FindNeqL(aPartialKey, aMask, aValue, aFoundKeys);
	}


/** Finds all the settings that match the specification given by aPartialKey
and aMask, but are either not floating point values or do not have the given value.
@param aPartialKey
	Contains a bit pattern that all the keys returned must at least partially
	match.
@param aMask
	Has bits set for all the bits in aPartialKey that must match the returned
	keys.
@param aValue
	Settings for the keys found will be settings that either contain values
	that are not floating point or floating point values other than aValue.
@param aFoundKeys All the keys found.
	For each key k in aFoundKeys, (k & aMask) == (aPartialKey & aMask) and
	the setting with key k is either not a floating point value or a floating
	point value not equal to aValue.
@see FindL()
@return
	KErrNone if successful,
	KErrAbort if in a transaction that has previously failed
	KErrPermissionDenied if caller fails capability check,
	KErrNotFound if capability check passed but no non-matching items are found,
	plus other system-wide error codes.
@post Transactions fail only on those "other system-wide error codes".
@capability Dependent Caller must satisfy the read policies of all settings found in the source range.
*/
EXPORT_C TInt CRepository::FindNeqL(TUint32 aPartialKey, TUint32 aMask,
	const TReal& aValue, RArray<TUint32>& aFoundKeys)
	{
	return (static_cast<CClientRepository*>(this))->FindNeqL(aPartialKey, aMask, aValue, aFoundKeys);
	}


/** Finds all the settings that match the specification given by aPartialKey
and aMask, but are either not string values or do not match the given string.
@param aPartialKey
	Contains a bit pattern that all the keys returned must at least partially
	match.
@param aMask
	Has bits set for all the bits in aPartialKey that must match the returned
	keys.
@param aValue
	Settings for the keys found will be settings that either contain values
	that are not strings or strings with value other than aValue.
@param aFoundKeys All the keys found.
	For each key k in aFoundKeys, (k & aMask) == (aPartialKey & aMask) and
	the setting with key k is either not a string value or a string value not
	equal to aValue.
@see FindL()
@return
	KErrNone if successful,
	KErrAbort if in a transaction that has previously failed
	KErrPermissionDenied if caller fails capability check,
	KErrNotFound if capability check passed but no non-matching items are found,
	plus other system-wide error codes.
@post Transactions fail only on those "other system-wide error codes".
@capability Dependent Caller must satisfy the read policies of all settings found in the source range.
*/
EXPORT_C TInt CRepository::FindNeqL(TUint32 aPartialKey, TUint32 aMask,
	const TDesC8& aValue, RArray<TUint32>& aFoundKeys)
	{
	return (static_cast<CClientRepository*>(this))->FindNeqL(aPartialKey, aMask, aValue, aFoundKeys);
	}

/** Finds all the settings that match the specification given by aPartialKey
and aMask, but are either not string values or do not match the given string.
@param aPartialKey
	Contains a bit pattern that all the keys returned must at least partially
	match.
@param aMask
	Has bits set for all the bits in aPartialKey that must match the returned
	keys.
@param aValue
	Settings for the keys found will be settings that either contain values
	that are not strings or strings with value other than aValue.
@param aFoundKeys All the keys found.
	For each key k in aFoundKeys, (k & aMask) == (aPartialKey & aMask) and
	the setting with key k is either not a string value or a string value not
	equal to aValue.
@see FindL()
@return
	KErrNone if successful,
	KErrAbort if in a transaction that has previously failed
	KErrPermissionDenied if caller fails capability check,
	KErrNotFound if capability check passed but no non-matching items are found,
	plus other system-wide error codes.
@post Transactions fail only on those "other system-wide error codes".
@capability Dependent Caller must satisfy the read policies of all settings found in the source range.
*/
EXPORT_C TInt CRepository::FindNeqL(TUint32 aPartialKey, TUint32 aMask,
	const TDesC16& aValue, RArray<TUint32>& aFoundKeys)
	{
	return (static_cast<CClientRepository*>(this))->FindNeqL(aPartialKey, aMask, aValue, aFoundKeys);
	}


/** Requests a notification if the value of a given setting changes.
Only one notification can be received per call to NotifyRequest.
Only one notification per setting per CRepository may be active at any one
time.
@param aKey The key setting to be informed about.
@param aStatus The object that will receive the notification. On a successful
	outcome, this will contain the Uid of the changed setting.
	If there is an existing notification on the same setting and same session, aStatus
	will be set to KErrAlreadyExists and the return value will be KErrNone.
@return
	KErrNone if successful or if aStatus is set to KErrAlreadyExists.
	KErrPermissionDenied if caller fails capability check.
	KErrNotFound if the requested setting does not exist.
@capability Note The caller must satisfy the relevant access policies for the repository
*/
EXPORT_C TInt CRepository::NotifyRequest(TUint32 aKey, TRequestStatus& aStatus)
	{
	return (static_cast<CClientRepository*>(this))->NotifyRequest(aKey, aStatus);
	}


/** Cancels a notification previously requested from the two-argument overload
of NotifyRequest.
@param aKey The parameter to the previous call to NotifyRequest to be cancelled.
@return KErrNone The method always returns KErrNone.
*/
EXPORT_C TInt CRepository::NotifyCancel(TUint32 aKey)
	{
 	//We deliberately ignore the return value from the server and return KErrNone because 
 	//it is incorrect for a cancel function to return an error code - if there is no 
 	//outstanding notification then the cancel should do nothing.
	(void)(static_cast<CClientRepository*>(this))->NotifyCancel(aKey);
	return KErrNone;
	}


/** Cancels all uncompleted notifications from this CRepository.
@return KErrNone The method always returns KErrNone.
*/
EXPORT_C TInt CRepository::NotifyCancelAll()
	{
 	//We deliberately ignore the return value from the server and return KErrNone because 
 	//it is incorrect for a cancel function to return an error code - if there is no 
 	//outstanding notification then the cancel should do nothing.
	(void)(static_cast<CClientRepository*>(this))->NotifyCancelAll();
	return KErrNone;
	}


/** Requests a notification if the value of a given setting changes.
Only one notification can be received per call to NotifyRequest.
Only one notification per setting per CRepository may be active at any one
time.
@param aPartialKey The partial key setting to be informed about.
@param aMask The mask to be used with the partial key.
@param aStatus The object that will receive the notification. On a successful
	outcome, this will contain the Uid of the changed setting.
	On error the error code is stored in aStatus and KErrNone is returned.
@return
	KErrNone The method always returns KErrNone.
@capability Note The caller must satisfy the relevant access policies for the repository
*/
EXPORT_C TInt CRepository::NotifyRequest(TUint32 aPartialKey, TUint32 aMask,
	TRequestStatus& aStatus)
	{
	return (static_cast<CClientRepository*>(this))
		->NotifyRequest(aPartialKey, aMask, aStatus);
	}


/** Cancels a notification previously requested from the three-argument overload
of NotifyRequest.
@param aPartialKey The parameter to the previous call to NotifyRequest to be cancelled.
@param aMask The mask to be used with the partial key
@return KErrNone The method always returns KErrNone.
*/
EXPORT_C TInt CRepository::NotifyCancel(TUint32 aPartialKey, TUint32 aMask)
	{
	(void)(static_cast<CClientRepository*>(this))->NotifyCancel(aPartialKey, aMask);
	return KErrNone;
	}


/** Resets the whole repository to the state of the initialization file
originally used to set it up. Not currently supported in transactions.
@return
	KErrNone if successful,
	KErrNotSupported if this client session is in a transaction
	plus other system-wide error codes.
@post Fails transaction if called when session is in one.
@capability Dependent The caller must satisfy the relevant access policies for the repository
*/
EXPORT_C TInt CRepository::Reset()
	{
	return (static_cast<CClientRepository*>(this))->Reset();
	}


/** Resets the a setting within the repository to the state of the setting
described by the initialization file originally used to set the repository up.
Not currently supported in transactions.
@param aKey Key of setting to be reset.
@return
	KErrNone if successful,
	KErrNotSupported if this client session is in a transaction
	plus other system-wide error codes.
@post Fails transaction if called when session is in one.
@capability Note The caller must satisfy the relevant access policies for the repository
*/
EXPORT_C TInt CRepository::Reset(TUint32 aKey)
	{
	return (static_cast<CClientRepository*>(this))->Reset(aKey);
	}


/** Attempts to starts a transaction in the given mode.
Consistency and persistence of all values read and written during the transaction
is only guaranteed after a successful return from CommitTransaction.
@param aMode transaction mode: EConcurrentReadWriteTransaction (standard),
	EReadTransaction or EReadWriteTransaction.
@return KErrNone if successful - guaranteed for EConcurrentReadWriteTransaction,
	KErrLocked for other transaction types if read or write locks held by other clients
	prevent transaction from starting.
@see CRepository::TTransactionMode
@pre Must not be in a transaction.
@post If it returns KErrNone: in a transaction; Any other error code: not in a transaction.
@panic CENREPCLI 3 Panics client if this session is already in a transaction.
*/
EXPORT_C TInt CRepository::StartTransaction(TTransactionMode aMode)
	{
	return (static_cast<CClientRepository*>(this))->StartTransaction(aMode);
	}


/** Attempts to starts a transaction in the given mode asynchronously to
allow client to avoid being blocked by server activity before starting.
Consistency and persistence of all values read and written during the transaction
is only guaranteed after a successful return from CommitTransaction.
Use CancelTransaction to cancel asynchronous request.
@param aMode transaction mode: EConcurrentReadWriteTransaction (standard),
	EReadTransaction or EReadWriteTransaction.
@see CRepository::TTransactionMode
@param aStatus On completion of asynchronous request: KErrNone if successful -
	guaranteed for EConcurrentReadWriteTransaction unless cancelled,
	KErrLocked for other transaction types if read or write locks held by other clients
	prevent transaction from starting.
@pre Must not be in a transaction.
@post If it completes with KErrNone and request not cancelled: in a transaction;
	Any other error code or request cancelled: not in a transaction.
@panic CENREPCLI 3 Panics client if this session is already in a transaction.
*/
EXPORT_C void CRepository::StartTransaction(TTransactionMode aMode, TRequestStatus& aStatus)
	{
	(static_cast<CClientRepository*>(this))->StartTransaction(aMode, aStatus);
	}


/** Commits a transaction. A successful return guarantees the consistency and
persistence of all values read and written during the transaction.
@return KErrNone on success, or error code giving first reason for failing.
	If KErrLocked is returned for EConcurrentReadWriteTransaction, transaction
	was interrupted by another client committing changes and should be repeated.
@param aKeyInfo
	On success: returns the number of keys whose values were modified.
	On failure: returns the key or partial key involved in the first error, or
	KUnspecifiedKey if failure could not be attributed to any key.
@pre Must be in a transaction.
@post Not in a transaction. On success, changes have been made persistent. On failure,
	transaction changes have been rolled back.
@panic CENREPCLI 4 Panics client if this session is not in a transaction.
*/
EXPORT_C TInt CRepository::CommitTransaction(TUint32& aKeyInfo)
	{
	return (static_cast<CClientRepository*>(this))->CommitTransaction(aKeyInfo);
	}


/** Commits a transaction asynchronously to allow client to avoid being blocked
during the persist operation and other server activity. A successful return guarantees
the consistency and persistence of all values read and written during the transaction.
Use CancelTransaction to cancel asynchronous request.
@param aKeyInfo
	A descriptor to receive a TUint32 value, e.g. TTransactionKeyInfoBuf, which
	client must ensure remains in scope for the duration of the asynchronous request.
	On success: returns the number of keys whose values were modified.
	On failure: returns the key or partial key involved in the first error, or
	KUnspecifiedKey if failure could not be attributed to any key.
@see CRepository::TTransactionKeyInfoBuf
@param aStatus Completion status of asynchronous request:
	On success (if not cancelled): KErrNone;
	On failure: error code giving first reason for failing.
	If KErrLocked is returned for EConcurrentReadWriteTransaction, transaction
	was interrupted by another client committing changes and should be repeated.
@pre Must be in a transaction.
@post Cannot assume that transaction is "pending commit" as may have completed already.
	Once request is complete, session is not in a transaction: on success, changes
	have been made persistent; on failure, transaction changes have been rolled back.
@panic CENREPCLI 4 Panics client if this session is not in a transaction.
*/
EXPORT_C void CRepository::CommitTransaction(TDes8& aKeyInfo, TRequestStatus& aStatus)
	{
	(static_cast<CClientRepository*>(this))->CommitTransaction(aKeyInfo, aStatus);
	}


/** Cancels the current transaction with rollback: discards any uncommitted changes.
If the transaction is pending start or commit asynchronously, the request is
completed with KErrCancel (unless it had already completed).
@post Not in a transaction.
*/
EXPORT_C void CRepository::CancelTransaction()
	{
	(static_cast<CClientRepository*>(this))->CancelTransaction();
	}


/** Pushes onto the CleanupStack a TCleanupItem that calls CancelTransaction if
activated by a Leave or PopAndDestroy. Important for releasing transaction resources
including caches and read/write locks held over the repository.
@post CleanupStack has another item on it which must be popped later with 
CleanupStack::Pop() or similar.
*/
EXPORT_C void CRepository::CleanupCancelTransactionPushL()
	{
	(static_cast<CClientRepository*>(this))->CleanupCancelTransactionPushL();
	}


/** Sets the active transaction to a failed state, releasing cache and locks but
keeping it open. Has no effect when not in an active transaction, including when it
has already failed or when pending asynchronous start or commit request completion.
Use in preference to CancelTransaction whenever it is inappropriate to close the
transaction at the time.
@post
	If previously in an active transaction: It is marked as failed. All uncommitted
	changes are discarded. Transaction resources including cache and read/write locks
	are released in the repository. All subsequent operations in the transaction fail
	until it is closed by CancelTransaction or CommitTransaction. CommitTransaction
	will fail and return KErrAbort.
*/
EXPORT_C void CRepository::FailTransaction()
	{
	(static_cast<CClientRepository*>(this))->FailTransaction();
	}


/** Pushes onto the CleanupStack a TCleanupItem that calls FailTransaction if
activated by a Leave or PopAndDestroy.
@post CleanupStack has another item on it which must be popped later with 
CleanupStack::Pop() or similar.
*/
EXPORT_C void CRepository::CleanupFailTransactionPushL()
	{
	(static_cast<CClientRepository*>(this))->CleanupFailTransactionPushL();
	}
