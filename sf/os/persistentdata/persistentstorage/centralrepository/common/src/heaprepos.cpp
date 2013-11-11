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
// simrepos.cpp
// 
//

#include "heaprepos.h"

CHeapRepository* CHeapRepository::NewL(TUid aUid)
	{
	CHeapRepository* repos=new (ELeave) CHeapRepository(aUid);
	return repos;
	}

CHeapRepository::CHeapRepository(TUid aUid)
	:iSettings(),iUid(aUid),iSinglePolicies(KGranularity)
	{
	}

CHeapRepository::~CHeapRepository()
	{
	iSinglePolicies.ResetAndDestroy();
	iRangePolicies.Close();
	iDeletedSettings.Close();
	iSettings.Close();
	iRangeMeta.Close();
	}

/**
Stores the repository in-memory content to the related repository file on drive C.
If the operation fails, the in-memory content won't match the content of 
the repository file (which will be kept as it was before the CommitChangesL() call).
In order to keep the consistency, the in-memory repository content is deleted now
and restored later, on the next repository operation.
*/
TInt CHeapRepository::CommitChanges(RFs& aFs,TUint8 aPersistVersion,const TDesC& aTargetFilePath)
	{
	TRAPD(error, DoCommitChangesL(aFs,aPersistVersion,aTargetFilePath));
	if (error != KErrNone)
		{	
		//If the commit fails reset the repository as it is in an inconsistent state
		ResetContent();
		}
	return error;
	}	

void CHeapRepository::SetMetaDataOnRead(TServerSetting& aSetting, TBool aSingleMetaFound)
	{
	TInt isMetaFlagSet = aSetting.Meta() & KMetaDefaultValue;
	
	if(!aSingleMetaFound)
		// No single metadata set for this key
		{
		// First check for a matching "range" default metadata
		// setting
		TSettingsDefaultMeta* defaultMeta = iRangeMeta.Find(aSetting.Key());
		if (defaultMeta)
			{
			if (isMetaFlagSet)
				//sets a default meta data
				//also sets the flag back to indicate that it is a default setting from ROM 
				//or previous install so it can be replaced later with a new one. 
				aSetting.SetMeta(defaultMeta->GetDefaultMetadata() | KMetaDefaultValue);
			else
				aSetting.SetMeta(defaultMeta->GetDefaultMetadata());
			}
		else
			{
			// Range value not found, try for a repository default
			if (isMetaFlagSet)	
				aSetting.SetMeta(iDefaultMeta | KMetaDefaultValue) ;
			else
				aSetting.SetMeta(iDefaultMeta) ;
			}
		}

#ifdef SYMBIAN_CENTREP_SUPPORT_MULTIROFS
#ifdef CENTREP_CONV_TOOL
	if (aSingleMetaFound && aSetting.IsClean() && !aSetting.IsIndividualMeta())
		aSingleMetaFound=EFalse;
#endif	
	aSetting.SetIndividualMeta(aSingleMetaFound);
#endif			
	}

TInt CHeapRepository::Create(TServerSetting& aSetting, TSettingsAccessPolicy* &aPolicy, TBool aSingleMetaFound)
	{
	if(iSettings.Find(aSetting.Key()))
		return KErrAlreadyExists;
	
	SetMetaDataOnRead( aSetting, aSingleMetaFound);
	aSetting.SetAccessPolicy(aPolicy);

	return iSettings.OrderedInsert(aSetting);
	}

// Comparison relation to allow single policies to be inserted in order
TInt CHeapRepository::CompareKeyIds(const TSettingsAccessPolicy& aSinglePolicy, const TSettingsAccessPolicy& aSinglePolicyIndexItem)
	{
	if(aSinglePolicy.iLowKey==aSinglePolicyIndexItem.iLowKey)
		return 0;
	return (aSinglePolicy.iLowKey < aSinglePolicyIndexItem.iLowKey)?-1:1;
	}

// Identity relation to allow single policy for a given key to be found
TBool CHeapRepository::SinglePolicyMatchOnKey(const TSettingsAccessPolicy& aSinglePolicy, const TSettingsAccessPolicy& aSinglePolicyIndexItem)
	{
	return aSinglePolicy.iLowKey==aSinglePolicyIndexItem.iLowKey;
	}

// returns the read security policy used if there is no per-setting policy at aId
const TSecurityPolicy& CHeapRepository::GetFallbackReadAccessPolicy(TUint32 aId)
	{
	return *(GetFallbackAccessPolicy(aId)->GetReadAccessPolicy());
	}

// returns the write security policy used if there is no per-setting policy at aId
const TSecurityPolicy& CHeapRepository::GetFallbackWriteAccessPolicy(TUint32 aId)
	{
	return *(GetFallbackAccessPolicy(aId)->GetWriteAccessPolicy());
	}

// Get pointer to security policy that applies to a given setting
TSettingsAccessPolicy* CHeapRepository::GetFallbackAccessPolicy(TUint32 aId
#ifdef SYMBIAN_CENTREP_SUPPORT_MULTIROFS
	,TBool aSkipSingle
#endif
	)
	{
#ifdef SYMBIAN_CENTREP_SUPPORT_MULTIROFS	
	if (!aSkipSingle)
		{
#endif		
		// Check for single policy
		TSettingsAccessPolicy policy(aId);
		TIdentityRelation<TSettingsAccessPolicy> identity(SinglePolicyMatchOnKey);
		TInt index = iSinglePolicies.Find(&policy, identity);
		if(KErrNotFound != index)
			return iSinglePolicies[index];
#ifdef SYMBIAN_CENTREP_SUPPORT_MULTIROFS		
		}
#endif		
	
	// check if the aId falls into any range specified in the ini file
	TSettingsAccessPolicy* rangePolicy = iRangePolicies.Find(aId);
	if(rangePolicy)
		return rangePolicy;
	
	// If no single policy or range policy, return default policy
	return &iDefaultPolicy;
	}

// this function saves idividual meta as well
TInt CHeapRepository::ReadSettingSavePolicyL(CIniFileIn& aFile,TServerSetting& aSetting, TSettingsAccessPolicy* &aPolicy, TBool& aSingleMetaFound)
	{
	TBool singleReadPolicyFound;
	TBool singleWritePolicyFound;
	TSecurityPolicy singleReadPolicy;
	TSecurityPolicy singleWritePolicy;

	TInt err=aFile.ReadSettingL(aSetting,singleReadPolicy, singleWritePolicy, singleReadPolicyFound, singleWritePolicyFound, aSingleMetaFound);
	if(err!=KErrNone)
		return err;
	
	// Set up single policies
	if(!singleReadPolicyFound)
		singleReadPolicy=GetDefaultReadAccessPolicy();
	if(!singleWritePolicyFound)
		singleWritePolicy=GetDefaultWriteAccessPolicy();

	aSetting.PushL();
	if(singleReadPolicyFound || singleWritePolicyFound)
		{
		aPolicy=new (ELeave) TSettingsAccessPolicy(singleReadPolicy,singleWritePolicy,aSetting.Key());
		CleanupStack::PushL(aPolicy);
#ifdef SYMBIAN_CENTREP_SUPPORT_MULTIROFS		
		//reused single setting high key and mask to indicate whether read or write has been specified
		//or they are just a default policy
		if (singleReadPolicyFound)
			aPolicy->iHighKey=1;
		if (singleWritePolicyFound)
			aPolicy->iKeyMask=1;
#endif		
		TLinearOrder<TSettingsAccessPolicy> order(&CHeapRepository::CompareKeyIds);
		iSinglePolicies.InsertInOrderL(aPolicy,order);
		CleanupStack::Pop(aPolicy);
		}
	else
		{
		// check if the aId falls into any range specified in the ini file
		// otherwise set policy to default policy
		TSettingsAccessPolicy* rangePolicy = iRangePolicies.Find(aSetting.Key());
		if(rangePolicy)
			aPolicy=rangePolicy;
		else
			aPolicy=&iDefaultPolicy;
		}
		
	aSetting.Pop();
	return err;
	}
		
#ifdef CENTREP_CONV_TOOL
void CHeapRepository::DoCommitChangesToIniFileL(RFs& aFs,const TDesC& aOutFileName
#ifdef SYMBIAN_CENTREP_SUPPORT_MULTIROFS
	,TUint32 aCreVersion
#endif
	)
	{
	CIniFileOut* out = CIniFileOut::NewLC(aFs,aOutFileName);

	out->WriteHeaderL();
	out->WriteOwnerSectionL(iOwner);
	out->WriteTimeStampL(iTimeStamp);
	out->WriteMetaDataL(iDefaultMeta, iRangeMeta);
#ifdef SYMBIAN_CENTREP_SUPPORT_MULTIROFS	
	out->WritePlatSecL(GetDefaultAccessPolicy(), iRangePolicies,aCreVersion);
#else
	out->WritePlatSecL(GetDefaultReadAccessPolicy(), GetDefaultWriteAccessPolicy(), iRangePolicies);
#endif	


	out->WriteMainSectionHeaderL();
	for(TInt i=0; i<iSettings.Count(); i++)
		{
		const TServerSetting& setting = iSettings[i];
		if (setting.HasAccessPolicy() && (iSinglePolicies.Find(setting.AccessPolicy()) != KErrNotFound))
			{
			out->WriteSettingL(setting, *setting.AccessPolicy()
#ifdef SYMBIAN_CENTREP_SUPPORT_MULTIROFS			
			,aCreVersion
#endif			
			);
			}
		else
			{
			out->WriteSettingL(setting
#ifdef SYMBIAN_CENTREP_SUPPORT_MULTIROFS			
			,aCreVersion
#endif			
			);
			}
		}

	out->CommitL();
	CleanupStack::PopAndDestroy(out);//out
	}
#endif

void CHeapRepository::DoCommitChangesL(RFs& aFs,TUint8 aPersistVersion,const TDesC& aTargetFilePath) 
	{
	CCreGenerator::CommitChangesToCreL(aFs,aPersistVersion,*this, aTargetFilePath);
	}	

/**
The method reloads the repository content from a repository file.
The current repository must be emptied (or must be empty already) before the call is made.
@param aIniFile A reference to CIniFileIn object, which will be used to load
				the repository content.
@leave System-wide error codes.
@leave KErrGeneral It's probably a programmer's error - current CHeapRepository 
				   object is partially initialised.
@leave KErrCorrupt Corrupted repository file.
*/
void CHeapRepository::ReloadContentL(CIniFileIn& aIniFile)
	{
	// Preconditions - CHeapRepository object should be an empty one.
	if(!IsEmpty())
		{
		User::Leave(KErrGeneral);
		}
	TInt err = ReloadContentExceptSettingsL(aIniFile);
	if(err == KErrCorrupt)
		{
		User::Leave(err);
		}
	CleanupClosePushL(iRangeMeta);
	CleanupClosePushL(iRangePolicies);
	
	// Settings
	TServerSetting setting;
	TSettingsAccessPolicy* policy;
	TBool singleMetaFound;
	TCleanupItem tc(SinglePoliciesCleanup, &iSinglePolicies);
	CleanupStack::PushL(tc);	
	CleanupClosePushL(iSettings);
	while((err = ReadSettingSavePolicyL(aIniFile, setting, policy, singleMetaFound)) == KErrNone)
		{
		setting.PushL();
		if(iSettings.IsDefault())
			{
			setting.SetClean();			
			}
		User::LeaveIfError(Create(setting, policy, singleMetaFound));
		setting.Pop();
		}
	if(err == KErrNotFound)			
		{
		err = KErrNone;
		}
	User::LeaveIfError(err);
	CleanupStack::Pop(4,&iRangeMeta);
	}

/**
Resets current repository data - actually all of them, which may be loaded from
the related ini file.
The iUid data member value is kept as it was at the moment of creation of 
CHeapRepository object.
*/
void CHeapRepository::ResetContent()
	{
	iSettings.Reset();
	iOwner = KNullUid;
	iTimeStamp = TTime(0);

	for (TInt i=0;i<iSinglePolicies.Count();i++)
		{
		delete iSinglePolicies[i];
		}
	iSinglePolicies.Reset();
	iRangePolicies.Reset();
	TSecurityPolicy emptyPolicy=TSecurityPolicy();
	iDefaultPolicy = TSettingsAccessPolicy(emptyPolicy,emptyPolicy, KUnspecifiedKey);
	iDefaultMeta = 0;
	iRangeMeta.Reset();
	}
  
void CHeapRepository::CreateRepositoryFromCreFileL(RFs& aFs,const TDesC& aFilePath )
	{
#ifdef SYMBIAN_CENTREP_SUPPORT_MULTIROFS	
	TUint8 dummyVersion;	
	CCreGenerator::CreateReposFromCreL(aFs,*this, aFilePath,dummyVersion);
#else
	CCreGenerator::CreateReposFromCreL(aFs,*this, aFilePath);
#endif	
	}

#ifdef SYMBIAN_CENTREP_SUPPORT_MULTIROFS
void CHeapRepository::CreateRepositoryFromCreFileL(RFs& aFs,const TDesC& aFilePath,TUint8& aCreVersion)
	{	
	CCreGenerator::CreateReposFromCreL(aFs,*this, aFilePath,aCreVersion);
	}
#endif

void CHeapRepository::ExternalizeCre(TUint8 aPersistVersion,RWriteStream& aStream) const
	{
	CCreGenerator::ExternalizeCre(aPersistVersion,*this, aStream);
	}


void CHeapRepository::InternalizeCreL(RReadStream& aStream)
	{
#ifdef SYMBIAN_CENTREP_SUPPORT_MULTIROFS	
	TUint8 dummyVersion;
	CCreGenerator::InternalizeCreL(*this, aStream,dummyVersion);
#else
	CCreGenerator::InternalizeCreL(*this, aStream);
#endif	
	}


void CHeapRepository::SinglePoliciesCleanup(TAny *aPtr)
	{
	static_cast<RPointerArray<TSettingsAccessPolicy>*>(aPtr)->ResetAndDestroy();
	}

TInt CHeapRepository::ReloadContentExceptSettingsL(CIniFileIn& aIniFile)
	{
	// Look for an "owner" section
	TUint32 uidValue(KNullUid.iUid);
	TInt err = aIniFile.ReadOwnerSectionL(uidValue);
	if(err == KErrCorrupt)
		{
		return err;
		}
	iOwner.iUid = uidValue;
	// Timestamp
	TTime timeStamp (0);
	err = aIniFile.ReadTimeStampSectionL(timeStamp);
	if(err == KErrCorrupt)
		{
		return err;
		}
	iTimeStamp=timeStamp;

	// Metadata
	err = aIniFile.ReadDefaultMetaSecSectionL(iDefaultMeta, iRangeMeta);
	// Even if err == KErrCorrupt, some items might have already been placed in the array
	CleanupClosePushL(iRangeMeta);
	if(err == KErrCorrupt)
		{
		CleanupStack::PopAndDestroy(&iRangeMeta);
		return err;
		}

	// Default read/write policies
	TBool gotDefaultReadPolicy;
	TBool gotDefaultWritePolicy;
	TSecurityPolicy defaultReadPolicy;
	TSecurityPolicy defaultWritePolicy;
	err = aIniFile.ReadPlatSecSectionL(defaultReadPolicy, gotDefaultReadPolicy,
									   defaultWritePolicy, gotDefaultWritePolicy,
									   iRangePolicies);
	// Even if err == KErrCorrupt, some items might have already been placed in the array
	CleanupClosePushL(iRangePolicies);	
	if(err == KErrCorrupt)
		{
		CleanupStack::PopAndDestroy(2,&iRangeMeta);
		return err;
		}

	iDefaultPolicy = TSettingsAccessPolicy(defaultReadPolicy,defaultWritePolicy, KUnspecifiedKey);
#ifdef SYMBIAN_CENTREP_SUPPORT_MULTIROFS	
	if (gotDefaultReadPolicy)
		iDefaultPolicy.iHighKey=1;
	if (gotDefaultWritePolicy)
		iDefaultPolicy.iKeyMask=1;
#endif	
	CleanupStack::Pop(2,&iRangeMeta);
	return KErrNone;
	}

TBool CHeapRepository::IsEmpty()
	{
	if(iSettings.Count() != 0 || iRangeMeta.Count() != 0 ||
	   iSinglePolicies.Count() != 0 || iRangePolicies.Count() != 0)
		{
		return EFalse;
		}
	return ETrue;
	}
