// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "centralrepository.h"
#include "heaprepos.h"
#include "datatype.h"
#include "pccenrepimpl.h"

/** Creates a CRepository object for accessing a repository.
If there is no such repository, the function leaves with KErrNotFound.
A pointer to the object is left on the cleanup stack.
@param aRepositoryUid The UID of the repository to be accessed
@return A pointer to a newly created CRepository object
@capability None
*/
EXPORT_C CRepository* CRepository::NewLC(TUid aRepositoryUid)
	{
	CRepository* self=new (ELeave)CRepository();
	CleanupStack::PushL(self);
	self->ConstructL(aRepositoryUid,KNullDesC,KNullDesC,ETrue);
	return self;
	}


/** Creates a CRepository object for accessing a repository.
If there is no such repository, the function leaves with KErrNotFound.
@param aRepositoryUid The UID of the repository to be accessed
@return A pointer to a newly created CRepository object
@capability None
*/
EXPORT_C CRepository* CRepository::NewL(TUid aRepositoryUid)
	{
	CRepository* self=CRepository::NewLC(aRepositoryUid);
	CleanupStack::Pop();
	return self;
	}

/**
Creates a CRepository object for accessing a repository specified in the input file
@param aInputFileName the location of the input file it should be in the format <file_path><XXXXXXXX><.txt/.cre> where XXXXXXXX is a 32 bit hex number
@param aOutputFileName the location fo the output it should be in the format <file_path><XXXXXXXX><.cre> where XXXXXXX is a 32 bit hex number
@leave KErrArgument if the file specified in the input and output do not follow the specification above
	 KErrCorrupt if the input file is corrupted
   	 KErrNoMemory if run out of memory
*/
EXPORT_C CRepository* CRepository::NewL(const TDesC& aInputRepositoryFile,const TDesC& aOutputRepositoryFile)	
	{
	CRepository* self=CRepository::NewLC(aInputRepositoryFile,aOutputRepositoryFile);
	CleanupStack::Pop();
	return self;		
	}

/**
Creates a CRepository object for accessing a repository specified in the input file.
A pointer to the object is left on the cleanup stack.
@param aInputFileName the location of the input file it should be in the format <file_path><XXXXXXXX><.txt/.cre> where XXXXXXXX is a 32 bit hex number
@param aOutputFileName the location fo the output it should be in the format <file_path><XXXXXXXX><.cre> where XXXXXXX is a 32 bit hex number
@leave KErrArgument if the file specified in the input and output do not follow the specification above
	 KErrCorrupt if the input file is corrupted
   	 KErrNoMemory if run out of memory
*/
EXPORT_C CRepository* CRepository::NewLC(const TDesC& aInputRepositoryFile,const TDesC& aOutputRepositoryFile)
	{
	CRepository* self=new (ELeave)CRepository();
	CleanupStack::PushL(self);
	self->ConstructL(KNullUid,aInputRepositoryFile,aOutputRepositoryFile,EFalse);
	return self;
	}

void CRepository::ConstructL(TUid aRepositoryUid,const TDesC& aInFileName,const TDesC& aOutFileName,TBool aAutoLoading)
	{
	iImpl = CPcRepImpl::NewL(aRepositoryUid,aInFileName,aOutFileName,aAutoLoading);
	}

/**
Flush any content of the repository in the heap to physical file, the target file is either the explicitly
specified output file or the implicitly determined output file depending on which NewL function is used
to construct the repository
@return KErrNone if successful,
	plus other system-wide error codes.
*/
EXPORT_C TInt CRepository::Flush()
	{
	return iImpl->Flush();
	}

/** Destructor. 
@capability None
*/
EXPORT_C CRepository::~CRepository()
	{
	delete iImpl;
	}


/** Creates a new setting with an integer value.
@param aKey New setting key.
@param aValue Setting value.
@return
	KErrNone if successful,
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
	TRAPD(err,iImpl->CreateSettingL(aKey,aValue,NULL));
	return err;
	}


/** Creates a new setting with a floating-point value.
@param aKey New setting key.
@param aValue Setting value.
@return
	KErrNone if successful,
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
	TRAPD(err,iImpl->CreateSettingL(aKey,aValue,NULL));
	return err;
	}


/** Creates a new setting with a descriptor value.
@param aKey New setting key.
@param aValue Setting value.
@return
	KErrNone if successful,
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
	if (aValue.Length()>KMaxBinaryLength)
		return KErrArgument;	
	TRAPD(err,iImpl->CreateSettingL(aKey,aValue,NULL));
	return err;	
	}

/** Creates a new setting with a descriptor value.
@param aKey New setting key.
@param aValue Setting value.
@return
	KErrNone if successful,
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
	if (aValue.Length()>KMaxUnicodeStringLength)
		return KErrArgument;
	TPtrC8 pVal((const TUint8*)aValue.Ptr(), aValue.Length()*2);		
	return Create(aKey,pVal);
	}

/** Deletes a setting.
@param aKey Key of setting to be deleted.
@return
	KErrNone if successful, 
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
	TRAPD(err,iImpl->DeleteSettingL(aKey));
	iImpl->RemoveAnyMarkDeleted();
	return err;
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
	KErrNotFound if no items were found in the partial key range.
	plus other system-wide error codes.
@post
	Transactions fail on all error conditions except KErrNotFound
	Outside transactions: on success the changes are persistent, on failure the
	repository is unmodified.
@capability Dependent Caller must satisfy the write policies of all settings found in the
					  partial key range.
*/	
EXPORT_C TInt CRepository::Delete(TUint32 aPartialKey, TUint32 aMask, TUint32& aErrorKey)
	{
	TRAPD(ret,iImpl->DeleteRangeL(aPartialKey,aMask,aErrorKey));
	return ret;
	}


/** Reads an integer setting.
@param aKey Key of setting to be read.
@param aValue Returns the value of the setting if it is an integer.
@return
	KErrNone if successful,
	KErrNotFound if the setting does not exist,
	KErrArgument if the setting exists but is not an integer,
	plus other system-wide error codes.
@post Transactions fail only on those "other system-wide error codes".
@capability Dependent Caller must satisfy the read access policy of that key in the repository.
*/
EXPORT_C TInt CRepository::Get(TUint32 aKey, TInt& aValue)
	{
	return iImpl->Get(aKey,aValue);
	}

/** Reads a floating point setting.
@param aKey Key of setting to be read.
@param aValue Returns the value of the setting if it is a floating point value.
@return
	KErrNone if successful,
	KErrNotFound if the setting does not exist,
	KErrArgument if the setting exists but is not a floating point value,
	plus other system-wide error codes.
@post Transactions fail only on those "other system-wide error codes".
@capability Dependent Caller must satisfy the read access policy of that key in the repository.
*/
EXPORT_C TInt CRepository::Get(TUint32 aKey, TReal& aValue)
	{
	return iImpl->Get(aKey,aValue);
	}
	
/** Reads a descriptor setting.
@param aKey Key of setting to be read.
@param aValue Returns the value of the setting if it is a descriptor.
@return
	KErrNone if successful,
	KErrNotFound if the setting does not exist,
	KErrArgument if the setting exists but is not a descriptor,
	KErrOverflow if the descriptor is too small to receive the value in the repository,
	plus other system-wide error codes.
@post Transactions fail only on those "other system-wide error codes".
@capability Dependent Caller must satisfy the read access policy of that key in the repository.
*/
EXPORT_C TInt CRepository::Get(TUint32 aKey, TDes8& aValue)
	{
	TInt actual;
	return Get(aKey,aValue,actual);
	}

/** Reads a descriptor setting.
@param aKey Key of setting to be read.
@param aValue Returns the value of the setting if it is a descriptor.
@param aActualLength Returns the actual length of the setting if it is a descriptor.
@return
	KErrNone if successful,
	KErrNotFound if the setting does not exist,
	KErrArgument if the setting exists but is not a descriptor,
	KErrOverflow if the descriptor is too small to receive the value in the repository,
	plus other system-wide error codes.
@post Transactions fail only on those "other system-wide error codes".
@capability Dependent Caller must satisfy the read access policy of that key in the repository.
*/
EXPORT_C TInt CRepository::Get(TUint32 aKey, TDes8& aValue, TInt& aActualLength)
	{
	TBuf8<KMaxBinaryLength> val;
	TInt ret = iImpl->Get(aKey, val);	
	if (ret==KErrNone)
		{
		TInt settingValueLength=val.Length();
		//now check whether any string overflow
		if (settingValueLength > aValue.MaxLength())
			{
			aActualLength=settingValueLength;
			aValue.Copy(val.Left(aValue.MaxLength()));
			return KErrOverflow;
			}
		else
			{
			aValue.Copy(val);
			}
		}	
	return ret;	
	}

/** Reads a descriptor setting.
@param aKey Key of setting to be read.
@param aValue Returns the value of the setting if it is a descriptor.
@return
	KErrNone if successful,
	KErrNotFound if the setting does not exist,
	KErrArgument if the setting exists but is not a descriptor,
	KErrOverflow if the descriptor is too small to receive the value in the repository,
	plus other system-wide error codes.
@post Transactions fail only on those "other system-wide error codes".
@capability Dependent Caller must satisfy the read access policy of that key in the repository.
*/
EXPORT_C TInt CRepository::Get(TUint32 aKey, TDes16& aValue)
	{
	TPtr8 ptr8((TUint8*)aValue.Ptr(), 0, aValue.MaxSize());
	TInt ret=Get(aKey,ptr8);
	if (ret==KErrNone)
		aValue.SetLength(ptr8.Length()/2);
	return ret;
	}

/** Reads a descriptor setting.
@param aKey Key of setting to be read.
@param aValue Returns the value of the setting if it is a descriptor.
@param aActualLength Returns the actual length of the setting if it is a descriptor.
@return
	KErrNone if successful,
	KErrNotFound if the setting does not exist,
	KErrArgument if the setting exists but is not a descriptor,
	KErrOverflow if the descriptor is too small to receive the value in the repository,
	plus other system-wide error codes.
@post Transactions fail only on those "other system-wide error codes".
@capability Dependent Caller must satisfy the read access policy of that key in the repository.
*/
EXPORT_C TInt CRepository::Get(TUint32 aKey, TDes16& aValue, TInt& aActualLength)
	{
	TInt actualLength8;
	TPtr8 ptr8((TUint8*)aValue.Ptr(), 0, aValue.MaxSize());
	TInt ret=Get(aKey,ptr8,actualLength8);
	aValue.SetLength(ptr8.Length()/2);			
	aActualLength=actualLength8/2;		
	return ret;
	}

/** Sets an existing integer setting to a new value or creates a new setting 
with an integer value if the setting doesn't exist.
@param aKey Key of setting to be written to.
@param aValue Value to be written.
@return
	KErrNone if successful,
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
	TRAPD(err,iImpl->SetSettingL(aKey,aValue));
	return err;
	}

/** Sets an existing floating point setting to a new value or creates a new setting
with a floating point value if the setting doesn't exist.
@param aKey Key of setting to be written to.
@param aValue Value to be written.
@return
	KErrNone if successful,
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
	TRAPD(err,iImpl->SetSettingL(aKey,aValue));
	return err;
	}

/** Sets an existing descriptor setting to a new value or creates a new setting
with a descriptor value if the setting doesn't exist.
@param aKey Key of setting to be written to.
@param aValue Value to be written.
@return
	KErrNone if successful,
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
	if (aValue.Length()>KMaxBinaryLength)
		return KErrArgument;
	TRAPD(err,iImpl->SetSettingL(aKey,aValue));
	return err;
	}

/** Sets an existing descriptor setting to a new value or creates a new setting
with a descriptor value if it doesn't exist.
@param aKey Key of setting to be written to.
@param aValue Value to be written.
@return
	KErrNone if successful,
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
	if (aValue.Length()>KMaxUnicodeStringLength)
		return KErrArgument;	
	TPtrC8 pVal((const TUint8*)aValue.Ptr(), aValue.Length()*2);
	return Set(aKey,pVal);
	}

/** Reads the metadata bound to a key
@param aKey The key
@param aMeta Returns the metadata value for the key
@return
	KErrNone if successful,
	KErrNotFound if the setting does not exist,
	plus other system-wide error codes.
@post Transactions fail only on those "other system-wide error codes".
@capability Dependent Caller must satisfy the read access policy of that key in the repository.
*/
EXPORT_C TInt CRepository::GetMeta(TUint32 aKey, TUint32& aMeta)
	{
	return iImpl->GetMeta(aKey,aMeta);	
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
	KErrNotFound if no items were found in the source range.
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
								 TUint32 aMask, TUint32& aErrorKey)
	{
	TRAPD(ret,iImpl->MoveL(aSourcePartialKey,aTargetPartialKey,aMask,aErrorKey));
	return ret;
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
	KErrNotFound if no items were found in the source range,
	plus other system-wide error codes.
@post Transactions fail only on those "other system-wide error codes".
*/
EXPORT_C TInt CRepository::FindL(TUint32 aPartialKey, TUint32 aMask,
	RArray<TUint32>& aFoundKeys)
	{
	RArray<TUint32> dummy;
	TRAPD(ret,iImpl->FindL(aPartialKey,aMask,aFoundKeys,KMaximumKey,dummy));
	if (ret==KErrNoMemory)
		User::LeaveNoMemory();
	return ret;
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
	KErrNotFound if capability check passed but no matching items are found,
	plus other system-wide error codes.
@post Transactions fail only on those "other system-wide error codes".
@capability Dependent Caller must satisfy the read policies of all settings found in the source range.
*/
EXPORT_C TInt CRepository::FindEqL(TUint32 aPartialKey, TUint32 aMask,
	TInt aValue, RArray<TUint32>& aFoundKeys)
	{
	TRAPD(ret,iImpl->FindSettingsComparisonL(aPartialKey,aMask,aValue,EEqual,aFoundKeys));
	if (ret==KErrNoMemory)
		User::LeaveNoMemory();
	return ret;	
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
	KErrNotFound if capability check passed but no matching items are found,
	plus other system-wide error codes.
@post Transactions fail only on those "other system-wide error codes".
@capability Dependent Caller must satisfy the read policies of all settings found in the source range.
*/
EXPORT_C TInt CRepository::FindEqL(TUint32 aPartialKey, TUint32 aMask,
	const TReal& aValue, RArray<TUint32>& aFoundKeys)
	{
	TRAPD(ret,iImpl->FindSettingsComparisonL(aPartialKey,aMask,aValue,EEqual,aFoundKeys));
	if (ret==KErrNoMemory)
		User::LeaveNoMemory();
	return ret;	
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
	KErrNotFound if capability check passed but no matching items are found,
	plus other system-wide error codes.
@post Transactions fail only on those "other system-wide error codes".
@capability Dependent Caller must satisfy the read policies of all settings found in the source range.
*/
EXPORT_C TInt CRepository::FindEqL(TUint32 aPartialKey, TUint32 aMask,
	const TDesC8& aValue, RArray<TUint32>& aFoundKeys)
	{
	TRAPD(ret,iImpl->FindSettingsComparisonL(aPartialKey,aMask,aValue,EEqual,aFoundKeys));
	if (ret==KErrNoMemory)
		User::LeaveNoMemory();
	return ret;	
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
	KErrNotFound if capability check passed but no matching items are found,
	plus other system-wide error codes.
@post Transactions fail only on those "other system-wide error codes".
@capability Dependent Caller must satisfy the read policies of all settings found in the source range.
*/
EXPORT_C TInt CRepository::FindEqL(TUint32 aPartialKey, TUint32 aMask,
	const TDesC16& aValue, RArray<TUint32>& aFoundKeys)
	{
	TPtrC8 pVal((const TUint8*)aValue.Ptr(), aValue.Length()*2);	
	TRAPD(ret,iImpl->FindSettingsComparisonL(aPartialKey,aMask,pVal,EEqual,aFoundKeys));
	if (ret==KErrNoMemory)
		User::LeaveNoMemory();
	return ret;	
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
	KErrNotFound if capability check passed but no non-matching items are found,
	plus other system-wide error codes.
@post Transactions fail only on those "other system-wide error codes".
@capability Dependent Caller must satisfy the read policies of all settings found in the source range.
*/
EXPORT_C TInt CRepository::FindNeqL(TUint32 aPartialKey, TUint32 aMask,
	TInt aValue, RArray<TUint32>& aFoundKeys)
	{
	TRAPD(ret,iImpl->FindSettingsComparisonL(aPartialKey,aMask,aValue,ENotEqual,aFoundKeys));
	if (ret==KErrNoMemory)
		User::LeaveNoMemory();
	return ret;	
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
	KErrNotFound if capability check passed but no non-matching items are found,
	plus other system-wide error codes.
@post Transactions fail only on those "other system-wide error codes".
@capability Dependent Caller must satisfy the read policies of all settings found in the source range.
*/
EXPORT_C TInt CRepository::FindNeqL(TUint32 aPartialKey, TUint32 aMask,
	const TReal& aValue, RArray<TUint32>& aFoundKeys)
	{
	TRAPD(ret,iImpl->FindSettingsComparisonL(aPartialKey,aMask,aValue,ENotEqual,aFoundKeys));
	if (ret==KErrNoMemory)
		User::LeaveNoMemory();
	return ret;
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
	KErrNotFound if capability check passed but no non-matching items are found,
	plus other system-wide error codes.
@post Transactions fail only on those "other system-wide error codes".
@capability Dependent Caller must satisfy the read policies of all settings found in the source range.
*/
EXPORT_C TInt CRepository::FindNeqL(TUint32 aPartialKey, TUint32 aMask,
	const TDesC8& aValue, RArray<TUint32>& aFoundKeys)
	{
	TRAPD(ret,iImpl->FindSettingsComparisonL(aPartialKey,aMask,aValue,ENotEqual,aFoundKeys));
	if (ret==KErrNoMemory)
		User::LeaveNoMemory();
	return ret;
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
	KErrNotFound if capability check passed but no non-matching items are found,
	plus other system-wide error codes.
@post Transactions fail only on those "other system-wide error codes".
@capability Dependent Caller must satisfy the read policies of all settings found in the source range.
*/
EXPORT_C TInt CRepository::FindNeqL(TUint32 aPartialKey, TUint32 aMask,
	const TDesC16& aValue, RArray<TUint32>& aFoundKeys)
	{
	TPtrC8 pVal((const TUint8*)aValue.Ptr(), aValue.Length()*2);	
	TRAPD(ret,iImpl->FindSettingsComparisonL(aPartialKey,aMask,pVal,ENotEqual,aFoundKeys));
	if (ret==KErrNoMemory)
		User::LeaveNoMemory();
	return ret;
	}

/** Attempts to starts a transaction in the given mode.
This function is doing nothing and just return KErrNone
@param aMode transaction mode: EConcurrentReadWriteTransaction (standard),
	EReadTransaction or EReadWriteTransaction.
@return KErrNone
@see CRepository::TTransactionMode
*/
EXPORT_C TInt CRepository::StartTransaction(TTransactionMode /**aMode*/)
	{
	return KErrNone;
	}


/** Attempts to starts a transaction in the given mode asynchronously
This function does nothing
@param aMode transaction mode: EConcurrentReadWriteTransaction (standard),
	EReadTransaction or EReadWriteTransaction.
@see CRepository::TTransactionMode
@param aStatus No effect on this parameter
*/
EXPORT_C void CRepository::StartTransaction(TTransactionMode /**aMode*/, TRequestStatus& /**aStatus*/)
	{
	return;
	}


/** Commits a transaction.
This function does nothing and just return KErrNone
@return KErrNone
@param aKeyInfo no effect on this parameter
*/
EXPORT_C TInt CRepository::CommitTransaction(TUint32& /**aKeyInfo*/)
	{
	return KErrNone;
	}


/** Commits a transaction asynchronously
This function does nothing
@param aKeyInfo
	A descriptor to receive a TUint32 value, e.g. TTransactionKeyInfoBuf, which
	client must ensure remains in scope for the duration of the asynchronous request.
	No effect on this input parameter
@see CRepository::TTransactionKeyInfoBuf
@param aStatus Completion status of asynchronous request:
	No effect on this input parameter
*/
EXPORT_C void CRepository::CommitTransaction(TDes8& /**aKeyInfo*/, TRequestStatus& /**aStatus*/)
	{
	return;
	}


/** Cancels the current transaction with rollback
This function does nothing
*/
EXPORT_C void CRepository::CancelTransaction()
	{
	}

/** Sets the active transaction to a failed state
This function does nothing
*/
EXPORT_C void CRepository::FailTransaction()
	{
	}

/** Pushes onto the CleanupStack a TCleanupItem that calls CancelTransaction if
activated by a Leave or PopAndDestroy.
This function does nothing 
*/
EXPORT_C void CRepository::CleanupCancelTransactionPushL()
	{
	CleanupStack::PushL(TCleanupItem(CPcRepImpl::FailTransactionCleanupOperation, this));	
	}
	
/** Pushes onto the CleanupStack a TCleanupItem that calls FailTransaction if
activated by a Leave or PopAndDestroy.
This function does nothing
*/
EXPORT_C void CRepository::CleanupFailTransactionPushL()
	{
	CleanupStack::PushL(TCleanupItem(CPcRepImpl::FailTransactionCleanupOperation, this));	
	}

