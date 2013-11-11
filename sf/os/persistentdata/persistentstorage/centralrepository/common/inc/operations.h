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

#ifndef OPERATIONS_H
#define OPERATIONS_H

#include <e32debug.h>
#include <e32std.h>
#include "datatype.h"

/**
This class encapsulates the operation logic used in both the client-server and the PC side library. The MOperationLogic
defines a set of pure virtual functions to be implemented. Currently the two classes responsible for performing operations
respectively are the CServerRepository and the CPcRepImpl
@internalTechnology
*/
class MOperationLogic
	{
	public:

/** DELETE RANGE
Delete a settings from a source list. Settings are marked as deleted rather than deleted immediately
@param aSourceList the source array of settings's pointer matching the partial key and mask
@param aPartialKey the partialKey of the settings to be delted
@param aErrorKey to hold the error encountered during the Delete operation
*/
void DeleteSettingsRangeL(RSettingPointerArray& aSourceList,TUint32 aPartialKey,TUint32& aErrorKey)
	{
	if (aSourceList.Count()==0)	
		{
		aErrorKey=aPartialKey;		
		User::Leave(KErrNotFound);
		}
		
	aErrorKey = KUnspecifiedKey;
	TInt numSettings = aSourceList.Count();
	TInt error=KErrNone;		
	for (TInt i = 0; (i < numSettings) && (error == KErrNone); i++)
		{
		ASSERT(aSourceList[i]);
		TServerSetting& settingToDelete = *(aSourceList[i]);
		TUint32 key = settingToDelete.Key();
		// delete it Ensure there is a delete placeholder at the location
		if (GetWritableSettingList().Find(key) == &settingToDelete)
			{
			// we are deleting a setting that is already in the transaction list: Flag it as deleted
			settingToDelete.Reset();
			settingToDelete.SetDeleted();
			}
		else
			{
			// create a new placeholder and set as deleted
			TServerSetting newSetting(key);
			newSetting.SetMeta(settingToDelete.Meta());
			newSetting.SetDeleted();
			GetWritableSettingList().OrderedInsertL(newSetting);
			}
		}	
	}

/** DELETE
Mark a setting in the source list as deleted if found
@param aId the setting Id to be deleted
@param aSettingList the list of source setting to look for
*/
void DeleteSettingL(TUint32 aId)
	{
	TServerSetting* ts=GetWritableSettingList().Find(aId);
	if (ts)
		{
		if (ts->IsDeleted())
			User::Leave(KErrNotFound);
		else
			{
			ts->Reset();
			ts->SetDeleted();	
			}
		}
	else
		{
		TServerSetting* s=GetSetting(aId);
		if (!s)	
			User::Leave(KErrNotFound);
		else
			{
			TServerSetting newSetting(aId);
			newSetting.SetMeta(s->Meta());
			newSetting.SetDeleted();
			GetWritableSettingList().OrderedInsertL(newSetting);
			}
		}
	}

/** SET
Set a setting to a new value, create the setting if it does not exist yet
@param aKey the id of the setting
@param aVal the new value of the setting
*/
template <class T>
void SetSettingL(TUint32 aKey,const T& aVal)
	{
	TServerSetting* s = GetWritableSettingList().Find(aKey);
	if (s)
		{
		if (s->IsDeleted())
			{
	 		// replace the deleted entry with the new values
			s->CopyValueL(aVal);
			s->SetAccessPolicy(GetFallbackAccessPolicy(aKey)); 			
			}
		else
			{
			User::LeaveIfError(s->AssignValueFrom(aVal));	
			s->SetMeta(s->Meta() & (~KMetaDefaultValue));	
			}		
		}
	else	
		{
		TServerSetting* ns=GetSetting(aKey);
		TServerSetting newSetting(aKey);
		newSetting.CopyValueL(aVal);
		newSetting.SetAccessPolicy(GetFallbackAccessPolicy(aKey));
		TUint32 metadata;		
		if (!ns)
			{
			GetSingleMeta(aKey,metadata);
			}
		else
			{
			if (!ns->IsType(aVal))
				{
				User::Leave(KErrArgument);
				}			
			metadata = ~KMetaDefaultValue & ns->Meta();
			}
		newSetting.SetMeta(metadata);			
		newSetting.PushL(); // only needed for strings
		GetWritableSettingList().OrderedInsertL(newSetting);	
		newSetting.Pop();			
		}
	}

/** CREATE
Create a setting with a new value with a meta value.If meta value
not specified it will attempt to look for the meta in order of single
,range, and finally default meta
@param aKey the id of the setting
@param aVal the new value of the setting
@param aMeta the meta value of the setting
@leave KErrAlreadyExists if setting with that id already exist
*/
template <class T>
void CreateSettingL(TUint32 aKey, const T& aVal, TUint32* aMeta)
	{
	TServerSetting* s = GetSetting(aKey);
	if (s)
		{
		if (!s->IsDeleted())
			User::Leave(KErrAlreadyExists);
		else
			{
			//previously deleted settings			
			s->CopyValueL(aVal);
			s->SetAccessPolicy(GetFallbackAccessPolicy(aKey));		
			}
		}
	else
		{
		//brand new settings, create this
		TServerSetting newSetting(aKey);
		newSetting.CopyValueL(aVal);
		if (aMeta)		
			{
			newSetting.SetMeta(*aMeta);
			}
		else
			{
			TUint32 singleMeta;
			GetSingleMeta(aKey,singleMeta);
			newSetting.SetMeta(singleMeta);	
			}
		newSetting.SetAccessPolicy(GetFallbackAccessPolicy(aKey));
		newSetting.PushL(); // only needed for strings
		GetWritableSettingList().OrderedInsertL(newSetting);	
		newSetting.Pop();		
		}
	}

/** GETMETA
Retrieve the meta associated with a setting
@param aId the id of the setting
@param aMeta return value for the setting's meta
@return KErrNotFound if setting does not exist
*/
TInt GetMeta(TUint32 aId, TUint32& aMeta)
	{
	const TServerSetting* s = GetSetting(aId);
	//if is deleted or cannot be found
	if (s && s->IsDeleted() || !s)
		{
		return KErrNotFound;
		}
	aMeta = ~KMetaDefaultValue & s->Meta();
	return KErrNone;
	}

/** GET
Retrieve the value of a setting
@param aId the id of the setting
@param aVal return value for the setting's value
@return KErrNotFound if setting does not exist
		KErrArgument if specified setting type does not match	
*/
template <class T>
TInt Get(TUint32 aId, T& aVal)
	{
	const TServerSetting* s = GetSetting(aId);
	//if is deleted or cannot be found
	if (s && s->IsDeleted() || !s)
		{
		return KErrNotFound;
		}
	return s->AssignValueTo(aVal);
	}

/** FIND COMPARE
Retrieve a list of settings' id that match the value based on either the equal or not equal comparison
@param aInputArray the source array of pointer to the settings
@param aVal the value to be compared for the setting
@param aEqual the comparison rule to be applied
@param aFoundIds the passed in ID array to hold the matching settings
*/
template <class T>
void FindCompareL(const RSettingPointerArray& aInputArray,const T& aVal,TComparison aEqual,RArray<TUint32>& aFoundIds) const
	{
	aFoundIds.Reset();
	TInt numSettings=aInputArray.Count();
	for (TInt i=0;i< numSettings;i++)
		{
		ASSERT(aInputArray[i]);
		const TServerSetting& setting = *(aInputArray[i]);
		ASSERT(!setting.IsDeleted());
		TInt error=KErrNone;		
		if(aEqual && setting==aVal || !aEqual && setting!=aVal)
			{
			error = aFoundIds.Append(setting.Key());
			if (error != KErrNone)
				{
				aFoundIds.Reset();
				User::Leave(error);
				}
			}
		}
	if (aFoundIds.Count() == 0)
		{
		User::Leave(KErrNotFound);
		}
	}
		
/** FINDL
Retrieve a list of settings that match the partial id and mask
@param aSourcePartialKey the partial key to match
@param aMask the mask to be used with the partial key for matching
@param aFoundIds, the array to hold the found settings id
@param aFoundIdsMaxLimit, specify the max id to fit in the aFoundIds
@param aExcessIds, the array to hold the remaining settings id
*/		
void FindL(TUint32 aSourcePartialKey,TUint32 aMask,RArray<TUint32>& aFoundIds,TUint aFoundIdsMaxLimit,RArray<TUint32>& aExcessIds)
	{
	RSettingPointerArray settings;
	CleanupClosePushL(settings);
	TInt error = FindSettings(aSourcePartialKey,aMask, settings);
	if (error == KErrNone)
		{
		const TUint numSettings = settings.Count();
		if (numSettings==0)
			{
			User::Leave(KErrNotFound);
			}
		aFoundIds.Reset();
		
		const TUint numInitial = numSettings > aFoundIdsMaxLimit ? aFoundIdsMaxLimit : numSettings;
		const TUint numFinal = numSettings > aFoundIdsMaxLimit ? numSettings - aFoundIdsMaxLimit : 0;
		
		//reserve memory for everything that needs to be added to the array
		aFoundIds.ReserveL(numSettings);
		
		//now append up to aFoundIdsMaxLimit settings
		for(TUint i = 0; i < numInitial; i++)
			{
			ASSERT(settings[i]);
			// all settings flagged as deleted should have been removed by now, but just to be safe:
			ASSERT(!settings[i]->IsDeleted());
			aFoundIds.AppendL(settings[i]->Key());
			}
			
		//fill the aExcessIds array with any remaining settings
		if(numFinal)
			{
			aExcessIds.Reset();
			aExcessIds.ReserveL(numFinal);
			for(TUint i = numInitial; i < numSettings; i++)
				{
				ASSERT(settings[i]);
				// all settings flagged as deleted should have been removed by now, but just to be safe:
				ASSERT(!settings[i]->IsDeleted());
				aExcessIds.AppendL(settings[i]->Key());
				}
			}
		}
	CleanupStack::PopAndDestroy();
	User::LeaveIfError(error);
	}

/** MOVE
Move settings that match a given partial key to another target partial key given the mask
@param aSourcePartialKey, the source partialKey
@param aTargetPartialKey the target partialKey
@param aMask the mask to be used with the partial keys
@param aErrorKey to hold the error encountered during the move operation
@param aSourcePointerArray the array containing the source settings' pointer
*/
TInt MoveL(TUint32 aSourcePartialKey,TUint32 aTargetPartialKey,TUint32 aMask, TUint32& aErrorKey,
			const RSettingPointerArray& aSourcePointerArray)
	{
	// all write operations now done in a transaction
	TInt error = KErrNone;
	aErrorKey = KUnspecifiedKey;
	
	TUint32 maskedSourcePartialKey = aSourcePartialKey & aMask;
	TUint32 maskedTargetPartialKey = aTargetPartialKey & aMask;
	TUint32 sourceToTarget = maskedSourcePartialKey ^ maskedTargetPartialKey;
	if(!sourceToTarget)
		{
		// not moving anywhere: must return now as this trivial case fails with later logic
		return KErrNone;
		}

	//Validation of the SourcePointerArray whether any item exist
	if (aSourcePointerArray.Count() == 0)
		{
		aErrorKey=aSourcePartialKey;
		return KErrNotFound;
		}

	// create a local copy of settings as the settings pointed to by RSettingPointerArray 
	// could move and cause CServerRepository to point to the wrong key, added as fix for DEF080104
	RSettingsArray settingsCopy;
	CleanupClosePushL(settingsCopy);
	settingsCopy.CopyFromPointerArrayL(aSourcePointerArray);
			
	for (TInt i = 0; (i < settingsCopy.Count()) && (error == KErrNone); i++)
		{
		ASSERT(&settingsCopy[i]);
		TServerSetting& sourceSetting = settingsCopy[i];
		TUint32 sourceKey = sourceSetting.Key();
		TUint32 targetKey = sourceKey ^ sourceToTarget;
		
		TServerSetting* targetSetting = GetSetting(targetKey);		
		if (targetSetting)
			{
			// must be set as deleted only(for PC side this can never be reached, all setting either exist or not and if exist
			// this will return KErrAlreadyExists and this error already captured earlier
			ASSERT(targetSetting->IsDeleted());
			error = targetSetting->Replace(sourceSetting);
			if (error == KErrNone)
				{
				targetSetting->SetKey(targetKey);
				// setting takes the access policy of the target key
				targetSetting->SetAccessPolicy(GetFallbackAccessPolicy(targetKey));
				}
			}
		else
			{
			TServerSetting newSetting;
			error = newSetting.Replace(sourceSetting);
			if (error == KErrNone)
				{
				TUint32 metaFromIni;
				newSetting.SetKey(targetKey);
				GetSingleMeta(targetKey,metaFromIni);
				newSetting.SetMeta(metaFromIni);
			
				// setting takes the access policy of the target key
				newSetting.SetAccessPolicy(GetFallbackAccessPolicy(targetKey));
				newSetting.PushL(); // only needed for strings
				GetWritableSettingList().OrderedInsertL(newSetting);
				newSetting.Pop();	// only needed for strings
				}
			}
			
		// ensure there is a delete placeholder at the old location
		TServerSetting* oldSetting=GetWritableSettingList().Find(sourceKey);
		if (oldSetting)
			{
			oldSetting->Reset();
			oldSetting->SetDeleted();
			}
		else
			{
			//this part cannot happen for PC side as the search is on the persistent directly
			TServerSetting newSetting(sourceKey);
			newSetting.SetDeleted();
			GetWritableSettingList().OrderedInsertL(newSetting);			
			}
		}
	// destroy local copy of settings
	settingsCopy.Reset();
	CleanupStack::PopAndDestroy(&settingsCopy);
	return error;		
	}


	//pure virtual functions to be implemented by CServerRepository and CPcRepImpl
	
	/**
	Retrieve the meta for a setting, look for the meta in the order of individual setting meta
	range meta and then default meta.
	*/
	virtual void GetSingleMeta(TUint aKey,TUint32& aMeta)=0;
	
	/**
	Retrieve the fall back policy associated with a setting
	*/
	virtual TSettingsAccessPolicy* GetFallbackAccessPolicy(TUint32 aId) const =0;
	
	/**
	Retrieve a pointer to a setting having the key specified in aKey
	*/
	virtual TServerSetting* GetSetting(TUint aKey)=0;
	
	/**
	Retrieve the settings that match the partial key and mask and add it into the settings pointer array
	*/
	virtual TInt FindSettings(TUint32 aSourcePartialKey,TUint32 aMask,RSettingPointerArray& aOutputArray) const=0;
	
	/**
	Get the list of settings array where modification should be made
	*/
	virtual RSettingsArray& GetWritableSettingList() =0;
};
#endif // OPERATIONS_H

