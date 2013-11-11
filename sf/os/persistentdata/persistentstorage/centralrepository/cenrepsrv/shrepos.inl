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

inline const TSecurityPolicy& CSharedRepository::GetReadAccessPolicy(const TServerSetting& aSetting)
	{
	return iSimRep->GetReadAccessPolicy(aSetting);
	}

inline const TSecurityPolicy& CSharedRepository::GetReadAccessPolicy(TUint32 aId)
	{
	return iSimRep->GetReadAccessPolicy(aId);
	}

inline const TSecurityPolicy& CSharedRepository::GetDefaultReadAccessPolicy()
	{
	return iSimRep->GetDefaultReadAccessPolicy();
	}

inline const TSecurityPolicy& CSharedRepository::GetWriteAccessPolicy(const TServerSetting& aSetting)
	{
	return iSimRep->GetWriteAccessPolicy(aSetting);
	}

inline const TSecurityPolicy& CSharedRepository::GetWriteAccessPolicy(TUint32 aId)
	{
	return iSimRep->GetWriteAccessPolicy(aId);
	}
	
inline const TSecurityPolicy& CSharedRepository::GetDefaultWriteAccessPolicy()
	{
	return iSimRep->GetDefaultWriteAccessPolicy();
	}

inline void CSharedRepository::WriteBackupStream(RWriteStream& aStream) const
	{
	iSimRep->SettingsArray().WriteBackupStream(aStream) ;
	aStream << iSimRep->Uid() ;
	aStream << iSimRep->Owner() ;
	aStream << iSimRep->TimeStamp().Int64() ;
		
	aStream << iSimRep->RangePolicyArray() ;
	aStream << iSimRep->GetDefaultReadAccessPolicy().Package() ;
	aStream << iSimRep->GetDefaultWriteAccessPolicy().Package() ;
	
	aStream << iSimRep->DefaultMeta() ;
	aStream << iSimRep->RangeMetaArray();

	// placeholders for 3 booleans that should never have been externalised
	TInt8 dummyBoolValue = 0;
	aStream << dummyBoolValue;
	aStream << dummyBoolValue;
	aStream << dummyBoolValue;	
	}
	
inline void CSharedRepository::WriteDeletedSettingsStream(RWriteStream& aStream) const
	{
	TCardinality numDeletedSettings = iSimRep->DeletedSettingsArray().Count() ;
	aStream << numDeletedSettings ;
	for (TInt i = 0; i < numDeletedSettings; i++)
		{
		aStream << iSimRep->DeletedSettingsArray()[i] ;	
		}	
	}	

inline void CSharedRepository::InternalizeL(RReadStream& aStream, CRestoredRepository& aRstRepos)
	{
 	RSettingsArray mergeArray;

 
 	aStream >> mergeArray ;
 	// Many keys-value pairs were restored from the backup file to mergeArray. But most of 
 	// the pairs are the same as what are currently in the repository. To prevent too many 
 	// notifications with no changed values, we only merge the keys whose values are going
 	// to be changed by passing EFalse to parameter aKeyOnly. Thus, mergeArray will only 
 	// contains the really changed keys after merging.
 	
	// For CommsDat repository(KCommsDatUid) and test repository(KTestUid) ,replace all the
	// current settings with the settings from the backup file
 	
#ifdef _DEBUG
	if((iSimRep->Uid() == KCommsDatUid) || (iSimRep->Uid() == KTestUid))
#else
	if (iSimRep->Uid() == KCommsDatUid)
#endif					
		{
		User::LeaveIfError(iSimRep->SettingsArray().ReplaceMerge(mergeArray, iSimRep->DeletedSettingsArray()));
		}
	else
		{        
		User::LeaveIfError(iSimRep->SettingsArray().MergeArray(mergeArray, iSimRep->DeletedSettingsArray(), ERestoreMerge));
		}

 	TInt numChanges = mergeArray.Count();
	for (TInt i = 0; i < numChanges; i++)
		{
		aRstRepos.AddKeyL(mergeArray[i].Key());
		}
	TUid repUid;
 	aStream >> repUid ;
 	iSimRep->SetUid(repUid);
 	TUid repOwner;
	aStream >> repOwner ;
	iSimRep->SetOwner(repOwner);
	
	TInt64 timeStampInt ;
	aStream >> timeStampInt ;
	iSimRep->SetTimeStamp(timeStampInt);

	TInt32 numElements = iSimRep->SettingsArray().Count();
	for (TInt32 count = 0; count < numElements; count++)
		{
		iSimRep->SettingsArray()[count].SetAccessPolicy(GetFallbackAccessPolicy(count));
		}
	iSimRep->RangePolicyArray().Reset();		
	iSimRep->RangePolicyArray().InternalizeL(aStream);
	
	HBufC8* securityPolicyPackage ;
	securityPolicyPackage = HBufC8::NewLC(aStream, 10000) ;
	TSecurityPolicy defaultReadPolicy;
	defaultReadPolicy.Set(securityPolicyPackage->Des()) ;
	CleanupStack::PopAndDestroy(securityPolicyPackage) ;
	securityPolicyPackage = HBufC8::NewLC(aStream, 10000) ;
	TSecurityPolicy defaultWritePolicy;
	defaultWritePolicy.Set(securityPolicyPackage->Des()) ;
	CleanupStack::PopAndDestroy(securityPolicyPackage) ;
	
	iSimRep->SetDefaultPolicy(TSettingsAccessPolicy(defaultReadPolicy,defaultWritePolicy, KUnspecifiedKey));
	
	TUint32 defaultMeta;
	aStream >> defaultMeta ;
	iSimRep->SetDefaultMeta(defaultMeta);
	
	iSimRep->RangeMetaArray().Reset();
	iSimRep->RangeMetaArray().InternalizeL(aStream);

	// placeholders for 3 booleans that should never have been externalised
	TInt8 dummyBoolValue;
	aStream >> dummyBoolValue;
	aStream >> dummyBoolValue;
	aStream >> dummyBoolValue;
	}

inline void CSharedRepository::SetSize(TInt aSize) 
	{ 
	iSize=aSize;
	}

inline TInt CSharedRepository::Size() const 
	{ 
	return iSize;
	} 
