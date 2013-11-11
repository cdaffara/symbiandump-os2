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

inline RPointerArray<TSettingsAccessPolicy>& CHeapRepository::SinglePolicyArray()
	{
	return iSinglePolicies;
	}

inline RRangePolicyArray& CHeapRepository::RangePolicyArray()
	{
	return iRangePolicies;
	}
	
inline RDefaultMetaArray& CHeapRepository::RangeMetaArray()
	{
	return iRangeMeta;
	}
	
inline RSettingsArray& CHeapRepository::SettingsArray()
	{
	return iSettings;
	}

inline RArray<TUint32>& CHeapRepository::DeletedSettingsArray()
	{
	return iDeletedSettings;	
	}

inline const TSecurityPolicy& CHeapRepository::GetReadAccessPolicy(const TServerSetting& aSetting)
	{
	const TSecurityPolicy* policy = aSetting.GetReadAccessPolicy();
	if (policy)
		{
		return *policy;
		}
	return GetFallbackReadAccessPolicy(aSetting.Key());
	}

inline const TSecurityPolicy& CHeapRepository::GetReadAccessPolicy(TUint32 aId)
	{
	const TServerSetting* s = iSettings.Find(aId);
	if (s)
		{
		const TSecurityPolicy* policy = s->GetReadAccessPolicy();
		if (policy)
			{
			return *policy;
			}
		}
	return GetFallbackReadAccessPolicy(aId);
	}

inline const TSecurityPolicy& CHeapRepository::GetDefaultReadAccessPolicy()
	{
	return *(iDefaultPolicy.GetReadAccessPolicy());
	}

inline const TSecurityPolicy& CHeapRepository::GetWriteAccessPolicy(const TServerSetting& aSetting)
	{
	const TSecurityPolicy* policy = aSetting.GetWriteAccessPolicy();
	if (policy)
		{
		return *policy;
		}
	return GetFallbackWriteAccessPolicy(aSetting.Key());
	}

inline const TSecurityPolicy& CHeapRepository::GetWriteAccessPolicy(TUint32 aId)
	{
	const TServerSetting* s = iSettings.Find(aId);
	if (s)
		{
		const TSecurityPolicy* policy = s->GetWriteAccessPolicy();
		if (policy)
			{
			return *policy;
			}
		}
	return GetFallbackWriteAccessPolicy(aId);
	}
	
inline const TSecurityPolicy& CHeapRepository::GetDefaultWriteAccessPolicy()
	{
	return *(iDefaultPolicy.GetWriteAccessPolicy());
	}


