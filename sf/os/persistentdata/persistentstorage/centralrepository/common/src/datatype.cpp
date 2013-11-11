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

#include "datatype.h"

#ifdef SYMBIAN_CENTRALREPOSITORY_PARANOID_CHECKING

/** Validates that the required invariants hold: (1) ordered by ascending key value (2) key uniqueness.
Panics upon violation.
*/
void RSettingsArray::ValidateInvariantProperties() const
	{
	if (Count() > 0)
		{
		const TServerSetting* prev = &( operator[](0) );
		const TServerSetting* curr = prev + 1;
		const TServerSetting* upperBound = prev + Count() - 1;
		while(curr <= upperBound)
			{
			// future: should replace with diagnostic panic code
			ASSERT(curr->Key() > prev->Key());
			prev = curr;
			++curr;
			}
		}
	}

/** Validates that the required invariants hold for the given setting pointer array: 
(1) ordered by ascending key value (2) key uniqueness. Panics upon violation.
*/
void RSettingsArray::ValidateInvariantProperties(const RSettingPointerArray& aPtrArray)
	{
	if (aPtrArray.Count() > 0)
		{
		const TServerSetting** prev = const_cast<const TServerSetting**>( &(aPtrArray[0]) );
		const TServerSetting** curr = prev + 1;
		const TServerSetting** upperBound = prev + aPtrArray.Count() - 1;
		while(curr <= upperBound)
			{
			// future: should replace with diagnostic panic code
			ASSERT((*curr)->Key() > (*prev)->Key());
			prev = curr;
			++curr;
			}
		}
	}

#endif

/** Returns pointers to all settings whose key matches all bits of the target id also included
in the bitwise mask.

@param	aTargetKey Bit pattern to match
@param	aMask Bitwise mask. Where bits are set the corresponding bit in aTargetKey is significant, ie
		AND behaviour conventional with masking
@param	aMatches Returns pointers to all settings with keys matching the mask, in ascending key order. The
		caller should ensure it is appropriately constructed for the likely growth pattern.
@return	Error code. aMatches may be modified even upon error return but the contents must not be relied upon.
*/
TInt RSettingsArray::Find(TUint32 aTargetKey, TUint32 aMask, RSettingPointerArray& aMatches) const
	{
#ifdef SYMBIAN_CENTRALREPOSITORY_PARANOID_CHECKING
	ValidateInvariantProperties();
#endif

	aMatches.Reset();
	// Find the first setting with key >= the lowest possible match
	const TInt upperIdx = Count() - 1;
	const TUint32 maskedTarget = aTargetKey & aMask;
	TInt lowerIdx;
	FindInUnsignedKeyOrder(TServerSetting(maskedTarget), lowerIdx);
	if(lowerIdx > upperIdx)
		{
		return KErrNone;
		}
	
	// this simply traverses the settings from the first possible match to the last. For masks with
	// only with higher bits this would be wasteful - in principle could jump to next possible match start
	// with another Find...() call. However logic more complex and counterproductive in some cases; would
	// need some heuristic such as probing N settings ahead of current point and only invoking the refind
	// if that still falls below the next range start, where N would allow for the cost of the binsearch
	// and other book-keeping (for real smartness construct a new RArray from the current point onwards,
	// so binsearch excludes the already known infertile preceding settings). Only worth pursuing after
	// profiling.
	const TUint32 upperMatchingKey = aTargetKey | ~aMask;
	const TServerSetting* setting = &( operator[](lowerIdx) );
	const TServerSetting* upperBound = setting + (upperIdx - lowerIdx);
	while(setting <= upperBound && setting->Key() <= upperMatchingKey)
		{
		if((setting->Key() & aMask) == maskedTarget)
			{
			TInt err = aMatches.Append(setting);
			// Small optimisation of not testing err in while() condition, presuming
			// that Append() far less common than key tests, ie repeatedly testing err
			// is wasteful. However (arguably) makes code more complex
			if(err != KErrNone)
				{
				aMatches.Reset();
				return err;
				}
			}
		++setting;
		}
	return KErrNone;
	}

/** Given source and destination arrays of settings, replaces the destination with the union of the
arrays. The properties of ascending key order and key uniqueness are preserved: where a key is common
to both arrays the source setting is kept, excepting where the source setting is flagged as deleted, which
simply deletes the destination

@param	aDst destination settings array
@param	aSrc source settings array
@return Error code. If not KErrNone then the contents of aDst are may have been modified and must be
		disregarded
@post	aDst contains the union and matches the setting array invariant
*/
TInt RSettingsArray::Merge(RSettingPointerArray& aDst, const RSettingPointerArray& aSrc)
	{
	const TServerSetting** srcBound;
	const TServerSetting** srcPtr;
	TInt count = aSrc.Count();
	if(count > 0)
		{
		srcPtr = const_cast<const TServerSetting**>( &(aSrc[0]) );
		srcBound = srcPtr + count;
		}
	else
		{
		srcPtr = NULL;
		srcBound = NULL;
		}

	TInt dstIdx = 0;
	const TServerSetting** dstPtr=NULL;
	const TServerSetting** dstBound=NULL;

	// The approach taken here of inserting entries one at a time is necessarily inefficient
	// given the RPointerArray interface, but may be adequate for the projected (small) use. For 
	// greater efficiency the ability to insert ranges is required, ie probably custom array code
	while(srcPtr < srcBound)
		{
		if (aDst.Count() > 0)
			{
			dstBound = const_cast<const TServerSetting**>( &(aDst[0]) ) + aDst.Count();
			dstPtr=const_cast<const TServerSetting**>( &(aDst[0]) )+ dstIdx;
			}
		const TUint32 srcKey = (*srcPtr)->Key();
		while(dstPtr < dstBound && srcKey > (*dstPtr)->Key())
			{
			++dstPtr;
			++dstIdx;
			}
		// Three possibilities: end of dst reached (append src), src and dst match (replace dst), 
		// and dst now greater than src (insert src)
		if((dstPtr == dstBound || (*dstPtr)->Key() > srcKey))
			{
			if(!(*srcPtr)->IsDeleted())	// drop deleted records
				{
				TInt ret = aDst.Insert(*srcPtr, dstIdx);	// presumes insert at end == append
				if(ret != KErrNone)
					{
					return ret;
					}
				++dstIdx;
				}
			}
		else
			{
			// Src key matches dst, replace or delete dst
			if(!(*srcPtr)->IsDeleted())
				{
				*dstPtr= *srcPtr;
				++dstIdx;
				}
			else
				{
				// may be more efficient ways of handling remove
				aDst.Remove(dstIdx);
				--dstBound;
				}
			}
		++srcPtr;
		}
#ifdef SYMBIAN_CENTRALREPOSITORY_PARANOID_CHECKING
	RSettingsArray::ValidateInvariantProperties(aDst);
#endif
	return KErrNone;
	}

// In the case of SWI downgrade merge, "Unchanged SWI keys" must be removed.
// Note: "Unchanged SWI keys" means those keys which were not in the ROM but installed later and also
// not modified by the user.
// This function will delete those keys from current settings array
// and will also append those deleted keys to the aRemovedSettings.
// Notification will take place in the calling code if the changes are committed successfully.
TInt RSettingsArray::RemoveUnchangedSWISettings(RSettingsArray& aChanges, RSettingsArray& aRemovedSettings)
	{
	TInt error = KErrNone;
	
	// Note: For SWI downgrades aChanges represents the ROM settings and the current 
	// RSettings array represents the persisted values.
	// Rom updates may result in entries being removed from the rom repository. 
	// If an entry is removed that is still marked as clean in the persisted repository,
	// then it should be removed from the final merge of the user entries, SWI and ROM.
	
	// Cycle thru all settings in current settings array ( persisted settings ) and look for them in aChanges ( ROM settings )
	for (TInt i = Count() - 1;  (i >= 0) && (KErrNone == error) ; i--)
		{
		TServerSetting source = operator[](i);
		if (source.IsClean())				// Persisted key is clean, ie unchanged by user
			{
			// find index of item in aChanges array matching the index of this setting
			TInt targetIndex = aChanges.FindIndex(source);
			if (targetIndex == KErrNotFound)		// Persisted key is not in ROM
				{
				// If key is clean and wasn't in the ROM, it was an installed key so the SWI downgrade
				// needs to remove it.
				// The key is saved for later addition to aChanges for notification reasons.
				TServerSetting setting(source.Key());
				error = aRemovedSettings.Append(setting);
				DeleteElement(i);
				}
			}
		}
	return error;
	}


/**
@internalComponent

This function merges the current RSettings array with the RSettings array provided by the
caller of the function in aChanges.

This function is used in 5 different cases, which are specified by the parameter aMergerType:
	Transaction Merges
	Back-up & Restore Merges
	SWI Upgrade Merges
	SWI Downgrade Merges
	ROM reflash merges
	
It will call the specified sub-function to complete the merger for each merge type.  

@param aChanges: This parameter is used as both input & output. When this function is called
it contains the settings which need to be merged with the current settings.
When this function returns it contains a list of all the keys in this RSettings array that 
have been changed( created, deleted or edited ) by the merge activity. This list is used to 
perform any notifies that are required after the merge changes have been successfully committed.

@param aDeletedSettings: This parameter is updated to keep track of settings that are deleted 
by the merge activity. Settings deleted by the merge activity are added to this list and settings created 
by the merge activity are removed from this list if they are on it. This information is used by B&R.

@param aMergeType: this parameter specifies which type of merge is to be carried out.

@return KErrNone in case of no error. System wide errors in other cases.

@panic In debug mode, panics if parameter aMergeType is not a supported type.
*/
TInt RSettingsArray::MergeArray(RSettingsArray& aChanges, RArray<TUint32> & aDeletedSettings, TMergeType aMergeType)
	{
	TInt error = KErrNone;
	
	switch (aMergeType)
		{
		case ESWIDowngradeMerge:
			error = MergeSWIDowngrade(aChanges, aDeletedSettings);
			break;
			
		case ETransactionMerge:
			error = MergeTransaction(aChanges, aDeletedSettings);
			break;
			
		case ERestoreMerge:
			error = MergeRestore(aChanges, aDeletedSettings);
			break;
			
		case ESWIUpgradeMerge:
			error = MergeSWIUpgrade(aChanges, aDeletedSettings);
			break;
	
		case ERomFlash:
			error = MergeRomFlash(aChanges, aDeletedSettings);
			break;
	
		default:
			ASSERT(EFalse);	
			error = KErrArgument;
		}
	
	return error;
	} 
	
/**
@internalComponent

This function complies the rule of the SWI downgrade merging to merge the current RSettings array with 
the RSettings array provided by the caller of the function in aChanges.

The rule of the SWI downgrage merging:
1. Remove unchanged SWI settings from the current RSettings array. Unchanged SWI settings are
   those settings which were not in the ROM but installed later and also not modified by the user.
   Those removed unchanged SWI settings will be appended to aChanges for later notification.
  
2. For each setting in aChanges:
   .if it has not a counterpart in the current array, create its counterpart in the current array. If this 
    setting exists in aDeletedSettings, remove it.
   .if it has a counterpart in the current array, check whether its value has been changed and the persisted 
    setting is clean. If so, replace the persisted setting with the new value. Otherwise, remove the setting 
    from aChanges.
    
@param aChanges: This parameter is used as both input & output. When this function is called, it contains 
the settings which have been internalized from the original cenrep initialisation ROM file and need to be merged 
with the current settings. All settings must not be marked as deleted. The program will not check the "deleted" 
mark in release mode but will panic upon a "deleted" marked setting in debug mode.

When this function returns it contains a list of all the keys in this RSettings array that 
have been changed( created, deleted or edited ) by the merge activity. This list is used to 
perform any notifies that are required after the merge changes have been successfully committed.

@param aDeletedSettings: This parameter is updated to keep track of settings that are deleted 
by the merge activity. Settings deleted by the merge activity are added to this list and settings created 
by the merge activity are removed from this list if they are on it. This information is used by B&R.

@panic In debug mode, panics if aChanges contains a setting being marked as deleted.

@return KErrNone in case of no error. System wide errors in other cases.
*/
TInt RSettingsArray::MergeSWIDowngrade(RSettingsArray& aChanges, RArray<TUint32>& aDeletedSettings)
	{
	RSettingsArray removedUnchangedSWIsettings;
	TInt error = RemoveUnchangedSWISettings(aChanges, removedUnchangedSWIsettings);
	
	if (KErrNone != error)
		{
		removedUnchangedSWIsettings.Reset();
		return error;
		}
	
	// Cycles thru all settings in aChanges and look for them in current settings array 
	for (TInt i = aChanges.Count() - 1;  (i >= 0) && (KErrNone == error) ; i--)
		{
		const TServerSetting& source = aChanges[i];

		// aChanges item must not be marked as deleted.		
		ASSERT(! source.IsDeleted());

		// Finds index of matching item in persistent array.
		TInt targetIndex = FindIndex(source);
		
		if (targetIndex == KErrNotFound)		// aChanges key not found in current RSettings array, 
			{
			error = MergeCreateSetting(source, aDeletedSettings);
			}
		else		// aChanges key found in current RSettings array,
			{
			TServerSetting& target = operator[](targetIndex);
			if ((target != source) && target.IsClean()) // value is changed and setting is clean
				{
				error = target.Replace(source);
				}
			else
				{  // value not changing or setting not clean: remove setting from aChanges to prevent notification
				aChanges.DeleteElement(i);
				}
			}
		}

	// Appends the removed unchanged SWI settings to aChanges for later notification
	for(TInt i = 0; (i < removedUnchangedSWIsettings.Count()) && (KErrNone == error) ; i++)
		{
		error = aChanges.Append(removedUnchangedSWIsettings[i]);
		}

	removedUnchangedSWIsettings.Reset();
	
	aDeletedSettings.Compress();
	
	return error;
	}


/**
@internalComponent

This function complies the rule of the transaction merging to merge the current RSettings array with 
the RSettings array provided by the caller of the function in aChanges.

The rule of the transaction merging:
For each setting in aChanges:
   .if it has not a counterpart in the current array and is marked as deleted, remove it from aChanges
   .if it has not a counterpart in the current array and is not marked as deleted, create its counterpart 
    in the current array. If this setting exists in aDeletedSettings, remove it.
   .if it has a counterpart in the current array and is marked as deleted, remove it from the current 
    array and insert it into aDeletedSettings array if its backup bit is set.
   .if it has a counterpart in the current array and is not marked as deleted, check whether its value 
    has been changed. If so, replace the persisted setting with the new value. Otherwise, remove the 
    setting from aChanges.
    
@param aChanges: This parameter is used as both input & output. When this function is called, it contains 
the settings which have been modified by the user and need to be merged with the current settings. They 
could be new, changed or deleted.

When this function returns it contains a list of all the keys in this RSettings array that 
have been changed( created, deleted or edited ) by the merge activity. This list is used to 
perform any notifies that are required after the merge changes have been successfully committed.

@param aDeletedSettings: This parameter is updated to keep track of settings that are deleted 
by the merge activity. Settings deleted by the merge activity are added to this list and settings created 
by the merge activity are removed from this list if they are on it. This information is used by B&R.

@return KErrNone in case of no error. System wide errors in other cases.
*/
TInt RSettingsArray::MergeTransaction(RSettingsArray& aChanges, RArray<TUint32>& aDeletedSettings)
	{
	
	TInt error = KErrNone;
	
	// Cycles thru all settings in aChanges and look for them in current settings array 
	for (TInt i = aChanges.Count() - 1;  (i >= 0) && (KErrNone == error) ; i--)
		{
		const TServerSetting& source = aChanges[i];
		// Finds index of matching item in persistent array.
		TInt targetIndex = FindIndex(source);
		if (targetIndex == KErrNotFound)		// aChanges key not found in current RSettings array, 
			{
			if (source.IsDeleted())				// aChanges key marked for delete, but does not exist in current RSettings array
				{
				// remove setting that is flagged as deleted from aChanges array to prevent notification
				aChanges.DeleteElement(i);
				}
			else	// aChanges key not marked for delete, add it to current RSettings array
				{
				error = MergeCreateSetting(source, aDeletedSettings);
				}
			}
		else		// aChanges key found in current RSettings array,
			{
			if (source.IsDeleted())				// aChanges key marked for delete, remove it from current RSettings array
				{
				// Retain "deleted element" state for settings marked for backup so that
				// we can persist state correctly across a backup/restore cycle.
				if (operator[](targetIndex).Meta() & KMetaBackup)
					{
					error = aDeletedSettings.InsertInUnsignedKeyOrder(source.Key()) ;
					}
				DeleteElement(targetIndex);
				}
			else								// aChanges key not marked for delete, needs to be merged
				{
				TServerSetting& target = operator[](targetIndex);
				if (target != source) // value is changed 
					{
					error = target.Replace(source);
					}
				else
					{  // value not changing or setting not clean: remove setting from aChanges to prevent notification
					aChanges.DeleteElement(i);
					}
				}
			}
		}

	aDeletedSettings.Compress();
	
	return error;
	}

/**
@internalComponent

This function complies the rule of the restore merging to merge the current RSettings array with 
the RSettings array provided by the caller of the function in aChanges.

The rule of the restore merging:
For each setting in aChanges:
   .if it has not a counterpart in the current array, create its counterpart in the current array. If this 
    setting exists in aDeletedSettings, remove it.
   .if it has a counterpart in the current array, check whether its value has been changed. If so, replace 
    the persisted setting with the new value. Otherwise, remove the setting from aChanges.
    
@param aChanges: This parameter is used as both input & output. When this function is called, it contains 
the settings which have been internalized from the previously backed up file and need to be merged 
with the current settings. All settings must not be marked as deleted. The program will not check the "deleted" 
mark in release mode but will panic upon a "deleted" marked setting in debug mode.

When this function returns it contains a list of all the keys in this RSettings array that 
have been changed( created, deleted or edited ) by the merge activity. This list is used to 
perform any notifies that are required after the merge changes have been successfully committed.

@param aDeletedSettings: This parameter is updated to keep track of settings that are deleted 
by the merge activity. Settings deleted by the merge activity are added to this list and settings created 
by the merge activity are removed from this list if they are on it. This information is used by B&R.

@panic In debug mode, panics if aChanges contains a setting being marked as deleted.

@return KErrNone in case of no error. System wide errors in other cases.
*/	
TInt RSettingsArray::MergeRestore(RSettingsArray& aChanges, RArray<TUint32>& aDeletedSettings)
	{
	TInt error = KErrNone;

	// Cycles thru all settings in aChanges and look for them in current settings array 
	for (TInt i = aChanges.Count() - 1;  (i >= 0) && (KErrNone == error) ; i--)
		{
		const TServerSetting& source = aChanges[i];

		// aChanges item must not be marked as deleted.		
		ASSERT(! source.IsDeleted());
		
		// Finds index of matching item in persistent array.
		TInt targetIndex = FindIndex(source);
		if (targetIndex == KErrNotFound)		// aChanges key not found in current RSettings array, 
			{
			error = MergeCreateSetting(source, aDeletedSettings);
			}
		else		// aChanges key found in current RSettings array,
			{
			TServerSetting& target = operator[](targetIndex);
			if (target != source) // value is changed 
				{
				error = target.Replace(source);
				}
			else
				{  // value not changing or setting not clean: remove setting from aChanges to prevent notification
				aChanges.DeleteElement(i);
					}
			}
		}

	aDeletedSettings.Compress();
	
	return error;
	}

/**
@internalComponent

This function complies the rule of the SWI Upgrade merging to merge the current RSettings array with 
the RSettings array provided by the caller of the function in aChanges.

The rule of the SWI Upgrade merging:
For each setting in aChanges:
   .if it has not a counterpart in the current array, create its counterpart in the current array. If this 
    setting exists in aDeletedSettings, remove it.
   .if it has a counterpart in the current array, check whether its value has been changed and the persisted 
    setting is clean. If so, replace the persisted setting with the new value. Otherwise, remove the setting 
    from aChanges.
    
@param aChanges: This parameter is used as both input & output. When this function is called, it contains 
the settings which have been internalized from a new cenrep initialisation file in the install directory or an 
updated existing cenrep initialisation file, and need to be merged with the current settings. All settings must 
not be marked as deleted. The program will not check the "deleted" mark in release mode but will panic upon a 
"deleted" marked setting in debug mode.

When this function returns it contains a list of all the keys in this RSettings array that 
have been changed( created, deleted or edited ) by the merge activity. This list is used to 
perform any notifies that are required after the merge changes have been successfully committed.

@param aDeletedSettings: This parameter is updated to keep track of settings that are deleted 
by the merge activity. Settings deleted by the merge activity are added to this list and settings created 
by the merge activity are removed from this list if they are on it. This information is used by B&R.

@panic In debug mode, panics if aChanges contains a setting being marked as deleted.

@return KErrNone in case of no error. System wide errors in other cases.
*/	
TInt RSettingsArray::MergeSWIUpgrade(RSettingsArray& aChanges, RArray<TUint32>& aDeletedSettings)
	{
	TInt error = KErrNone;

	// Cycles thru all settings in aChanges and look for them in current settings array 
	for (TInt i = aChanges.Count() - 1;  (i >= 0) && (KErrNone == error) ; i--)
		{
		const TServerSetting& source = aChanges[i];

		// aChanges item must not be marked as deleted.		
		ASSERT(! source.IsDeleted());
		
		// Finds index of matching item in persistent array.
		TInt targetIndex = FindIndex(source);
		if (targetIndex == KErrNotFound)		// aChanges key not found in current RSettings array, 
			{
			error = MergeCreateSetting(source, aDeletedSettings);
			}
		else		// aChanges key found in current RSettings array,
			{
			TServerSetting& target = operator[](targetIndex);
			if ((target != source) && target.IsClean()) // value is changed and setting is clean
				{
				error = target.Replace(source);
				}
			else
				{  // value not changing or setting not clean: remove setting from aChanges to prevent notification
				aChanges.DeleteElement(i);
				}
			}
		}

	aDeletedSettings.Compress();
	
	return error;
	}

/**
@internalComponent

This function complies the rule of the ROM flash merging to merge the current RSettings array with 
the RSettings array provided by the caller of the function in aChanges.

The rule of the ROM flash merging:
1. Remove unchanged SWI settings from the current RSettings array. Unchanged SWI settings are
   those settings which were not in the ROM but installed later and also not modified by the user.
  
2. For each setting in aChanges:
   .if it has not a counterpart in the current array, create its counterpart in the current array. If this 
    setting exists in aDeletedSettings, remove it.
   .if it has a counterpart in the current array, check whether its value has been changed and the persisted 
    setting is clean. If so, replace the persisted setting with the new value. Otherwise, remove the setting 
    from aChanges.
    
@param aChanges: This parameter is used as both input & output. When this function is called, it contains 
the settings which are the merging result of the ROM and install files and need to be merged 
with the current settings. All settings must not be marked as deleted. The program will not check the "deleted" 
mark in release mode but will panic upon a "deleted" marked setting in debug mode.

@param aDeletedSettings: This parameter is updated to keep track of settings that are deleted 
by the merge activity. Settings deleted by the merge activity are added to this list and settings created 
by the merge activity are removed from this list if they are on it. This information is used by B&R.

@panic In debug mode, panics if aChanges contains a setting being marked as deleted.

@return KErrNone in case of no error. System wide errors in other cases.
*/
TInt RSettingsArray::MergeRomFlash(RSettingsArray& aChanges, RArray<TUint32>& aDeletedSettings)
	{
	TInt error = KErrNone;
	
	// Cycle thru all settings in current settings array ( persisted settings ) and look for them in aChanges ( ROM settings )
	for (TInt i = Count() - 1;  i >= 0 ; i--)
		{
		TServerSetting source = operator[](i);
		if (source.IsClean())				// Persisted key is clean, ie unchanged by user
			{
			// find index of item in aChanges array matching the index of this setting
			TInt targetIndex = aChanges.FindIndex(source);
			if (targetIndex == KErrNotFound)		// Persisted key is not in ROM
				{
				// If key is clean and wasn't in the ROM, it was an installed key so the SWI downgrade
				// needs to remove it.
				DeleteElement(i);
				}
			}
		}	

	// Cycles thru all settings in aChanges and look for them in current settings array 
	for (TInt i = aChanges.Count() - 1;  (i >= 0) && (KErrNone == error) ; i--)
		{
		const TServerSetting& source = aChanges[i];

		// aChanges item must not be marked as deleted.		
		ASSERT(! source.IsDeleted());
		
		// Finds index of matching item in persistent array.
		TInt targetIndex = FindIndex(source);
		if (targetIndex == KErrNotFound)		// aChanges key not found in current RSettings array, 
			{
			error = MergeCreateSetting(source, aDeletedSettings);
			}
		else		// aChanges key found in current RSettings array,
			{
			TServerSetting& target = operator[](targetIndex);
			if (target!=source)
				{
				if (target.IsClean())
					error=target.Replace(source);
				else
					{
					//if not clean and persist and rom/swi is different
					if (target.Type()!=source.Type())
						error=target.Replace(source);
					}
				}
			else
				{
				//update the meta with the new ROM meta
				if (target.IsClean())
					{
					target.iMeta=source.iMeta;
					}
				else
					{
					target.iMeta=source.iMeta;
					target.SetNotClean();
					}
				}		
			}
		}
	
	aDeletedSettings.Compress();
	
	return error;
	}

/**
This function creates a setting in the current array with the same contents of the provided setting.

@param aSource The reference to the TServerSetting holding the setting's contents to be created.
@param aDeletedSettings The reference to RArray<TUint32>. If the created setting is found in it, the 
setting should be removed from it.

@return KErrNone in case of no error. System wide errors in other cases.
*/
TInt RSettingsArray::MergeCreateSetting(const TServerSetting& aSource, RArray<TUint32>& aDeletedSettings)
	{
	TServerSetting newSetting(aSource.Key());
	TInt error = newSetting.Replace(aSource);
	if (KErrNone == error)
		{
		error = OrderedInsert(newSetting);
		if (KErrNone == error)
			{
			// Removes the item from aDeletedSettings if it exsisting in aDeletedSettings
			TInt deletedSetting = aDeletedSettings.FindInUnsignedKeyOrder(aSource.Key());
			if (KErrNotFound != deletedSetting)
				{
				aDeletedSettings.Remove(deletedSetting) ;
				}	
			}
		else // this object did not take the ownership of the pointers under newSetting, need to free them.
			{
			newSetting.Reset();
			}
		}
	return error;
	}
	
/**
This function replaces the current RSettings array with the RSettings array provided by the
caller of the function in aChanges, Its  a variant of  the mergeArray function defined above.

@param aChanges: This parameter is used as both input & output. When this function is called
it contains the settings which need to merged with the current settings.
When this function returns it contains a list of all the keys in this RSettings array that 
have been changed( created, deleted or edited ) by the merge activity. This list is used to 
perform any notifies that are required after the merge changes have been successfully committed.


@param aDeletedSettings: This parameter is updated to keep track of settings that are deleted 
by the merge activity. Keys deleted by the merge activity are added to this list and keys created 
by the merge activity are removed from this list if they are on it. This information is used by B&R.
*/
TInt RSettingsArray::ReplaceMerge(RSettingsArray& aChanges, RArray<TUint32>& aDeletedSettings)
	{

	TInt error = KErrNone;
	TInt numChanges, changeArrayCount;

	changeArrayCount = aChanges.Count(); //Get the intial count.

	// Step1 :	Delete the existing settings  from Current Repository
	// The main loop cycles thru current Settings and delete one by one 
	// Deleted keys are appended to aChanges array to notify the user
	// after returning from the function. 	
	
	//Current settings count
	numChanges = Count(); 
	// Cycle thru all settings in current settings array ( persisted settings ) and look for them in aChanges ( ROM settings )
	for (;numChanges>0 && error==KErrNone;--numChanges)
		{
		TServerSetting source = operator[](0);
		//only existing settings that can't be found in aChanges should be added to aChanges
		if (aChanges.FindIndex(source)==KErrNotFound)
			{
			// The key is saved for later addition to aChanges for notification reasons.
			TServerSetting setting(source.Key());
			// CAUTION: setting have to be appended at the end of array so the main loop won't process them.
			error = aChanges.Append(setting);
			}
		DeleteElement(0);
		}

	//Step 2 : Restore the  Backed up Settings which is  received here through 'aChanges
	//Iterate through  each settings in aChanges and add it to the current settings.
	numChanges = changeArrayCount;
	for (TInt i=0;i<numChanges && error==KErrNone;i++)
		{
		const TServerSetting& source = aChanges[i];		
		TServerSetting newSetting(source.Key());
		error = newSetting.Replace(source);
		if (error == KErrNone)
			{
			error = OrderedInsert(newSetting);
			if (error != KErrNone)
				{
				newSetting.Reset();
				}
			}
		}	

	//reset all the items in aDeletedSettings as this will be completely replaced by the restore file
	aDeletedSettings.Reset();
	return error;
	}

void RSettingsArray::CopyFromPointerArrayL(const RSettingPointerArray& aArray)
	{
	TInt numSettings = aArray.Count();
	for (TInt i = 0; i < numSettings; i++)
		{	
		TServerSetting& source =*(aArray[i]);
		TServerSetting copiedSetting;
		// copied setting is set as deleted first so ReplaceTypeAndValue will work
		copiedSetting.SetDeleted();
		User::LeaveIfError(copiedSetting.ReplaceTypeAndValue(source));
		
		// set values not copied by ReplaceTypeAndValue	
		copiedSetting.SetKey(source.Key());
		copiedSetting.SetMeta(source.Meta() & (~KMetaDefaultValue));
		copiedSetting.SetAccessPolicy(source.AccessPolicy());
		if(source.IsDeleted())
			{
			copiedSetting.SetDeleted();
			}
		copiedSetting.PushL();
		OrderedInsertL(copiedSetting);
		copiedSetting.Pop();
		}
	}
