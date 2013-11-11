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

#ifndef DATATYPE_H
#define DATATYPE_H

#include <e32base.h>
#include "srvdefs.h"
#include "setting.h"

typedef RPointerArray<TServerSetting> RSettingPointerArray;

enum TComparison {ENotEqual, EEqual};
enum TMergeType {EInternalizeMerge, ETransactionMerge,ERestoreMerge, ESWIUpgradeMerge, ESWIDowngradeMerge, ERomFlash };
//Repository having one of the following two Uids(KCommsDatUid,KTestUid) will have different merging 
//treatment during a restore from a backup file, specifically that existing settings for
//these respective repositories will be completely replaced by settings from the backup file.
const TUid KCommsDatUid  = {0xCCCCCC00};
#ifdef _DEBUG
	const TUid KTestUid  ={0xABCABC00};
#endif 

/** 
@internalTechnology
Uses private inheritance, delegating only those methods that are needed, to remove the possibility
of callers breaking invariants (such as non-repeating and strictly-increasing keys) by using
modifiers like Append directly.
*/
class RSettingsArray : private RArray<TServerSetting>
	{
public:
	inline RSettingsArray();
	inline ~RSettingsArray();
	inline TServerSetting* Find(TInt aKey) const;
	inline TInt FindIndex(const TServerSetting& aSetting) const;
	inline void OrderedInsertL(const TServerSetting& aEntry);
	inline TInt OrderedInsert(const TServerSetting& aEntry);
	inline TInt Count() const;
	inline const TServerSetting &operator[](TInt aIndex) const;
	inline TServerSetting &operator[](TInt aIndex);
	inline void Remove(TInt aIndex);
	inline void Reset();
	inline void Close();
	inline void AdoptL(RSettingsArray& aSrc);
	
	inline void WriteBackupStream (RWriteStream& aStream) const;
	inline void ExternalizeL(RWriteStream& aStream) const;
	inline void InternalizeL(RReadStream& aStream) ;

	TInt Find(TUint32 aPartialId, TUint32 aMask, RSettingPointerArray& aMatches) const;
	static TInt Merge(RSettingPointerArray& aDst, const RSettingPointerArray& aSrc);
	inline TBool IsDefault() const;
	inline void SetIsDefault(TBool aIsDefault);
#ifdef SYMBIAN_CENTRALREPOSITORY_PARANOID_CHECKING
	void ValidateInvariantProperties() const;
	static void ValidateInvariantProperties(const RSettingPointerArray& aPtrArray);
#endif
 	TInt MergeArray(RSettingsArray& aChanges, RArray<TUint32>& aDeletedSettings, TMergeType aMergeType);
	TInt ReplaceMerge(RSettingsArray& aChanges, RArray<TUint32>& aDeletedSettings);
	void CopyFromPointerArrayL(const RSettingPointerArray& aArray);

	//this one delete element based on the array index
	inline void DeleteElement(TInt aIndex);
	inline void RemoveElement(TInt aIndex);

private:
	enum {KGranularity=8};
	TInt MergeSWIDowngrade(RSettingsArray& aChanges, RArray<TUint32> & aDeletedSettings);
	TInt MergeTransaction(RSettingsArray& aChanges, RArray<TUint32>& aDeletedSettings);
	TInt MergeRestore(RSettingsArray& aChanges, RArray<TUint32>& aDeletedSettings);
	TInt MergeSWIUpgrade(RSettingsArray& aChanges, RArray<TUint32>& aDeletedSettings);
	TInt MergeRomFlash(RSettingsArray& aChanges, RArray<TUint32>& aDeletedSettings);	
	
	TInt RemoveUnchangedSWISettings(RSettingsArray& aChanges, RSettingsArray& aRemovedSettings);
	TInt MergeCreateSetting(const TServerSetting& aSource, RArray<TUint32>& aDeletedSettings);
	
	TBool iIsDefault;
	};
/**
@internalTechnology
This is a wrapper for TSecurityPolicies specified for a range of settings,
main purpose is to provide nice search trough the range */
class RRangePolicyArray : public RArray<TSettingsAccessPolicy>
	{
public:
	inline RRangePolicyArray();
	inline ~RRangePolicyArray();
	inline TSettingsAccessPolicy* Find(TInt aKey) const;

	inline void ExternalizeL(RWriteStream& aStream) const;
	inline void InternalizeL(RReadStream& aStream) ;
#ifdef SYMBIAN_CENTREP_SUPPORT_MULTIROFS	
	TBool IsEqual(const RRangePolicyArray& aRangePolicyArray) const
		{
		return (Mem::Compare((TUint8*)(&((*this)[0])),this->Count()*sizeof(TSettingsAccessPolicy),
		(TUint8*)(&(aRangePolicyArray[0])),aRangePolicyArray.Count()*sizeof(TSettingsAccessPolicy))==0);			
		}
#endif		
	};
	
/**
@internalTechnology
This is a wrapper for default metadata specified for ranges of settings,
main purpose is to provide nice search trough the range */
class RDefaultMetaArray : public RArray<TSettingsDefaultMeta>
	{
public:
	inline RDefaultMetaArray();
	inline ~RDefaultMetaArray();
	inline TSettingsDefaultMeta* Find(TInt aKey) const;
	inline void ExternalizeL(RWriteStream& aStream) const;
	inline void InternalizeL(RReadStream& aStream) ;
#ifdef SYMBIAN_CENTREP_SUPPORT_MULTIROFS	
	TBool IsEqual(const RDefaultMetaArray& aRangeMetaArray) const
		{
		return (Mem::Compare((TUint8*)(&((*this)[0])),this->Count()*sizeof(TSettingsDefaultMeta),
		(TUint8*)(&(aRangeMetaArray[0])),aRangeMetaArray.Count()*sizeof(TSettingsDefaultMeta))==0);
		}
#endif	
	};

/**
@internalTechnology
*/
class RSingleMetaArray : public RArray<TSettingSingleMeta>
	{
public:
	inline RSingleMetaArray();
	inline TInt Find (TUint32 aKey, TUint32& aMeta) const;
	};

#include "datatype.inl"

#endif // DATATYPE_H
