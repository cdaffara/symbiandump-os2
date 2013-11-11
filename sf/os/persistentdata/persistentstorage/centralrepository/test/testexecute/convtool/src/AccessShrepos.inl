// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 
inline TConvToolTester::TConvToolTester(CSharedRepository& aRep)
	: iSharedRep(aRep)
	{
	}

inline TUint32 TConvToolTester::DefaultMeta() const
	{
	return iSharedRep.iSimRep->iDefaultMeta;
	}

inline const RDefaultMetaArray& TConvToolTester::DefaultMetaArray() const
	{
	return iSharedRep.iSimRep->iRangeMeta;
	}

inline const RSettingsArray& TConvToolTester::Settings() const
	{
	return iSharedRep.iSimRep->iSettings;
	}

inline const TUid& TConvToolTester::Owner() const
	{
	return iSharedRep.iSimRep->iOwner;
	}

inline const TTime& TConvToolTester::TimeStamp() const
	{
	return iSharedRep.iSimRep->iTimeStamp;
	}

inline const RPointerArray<TSettingsAccessPolicy>& TConvToolTester::SinglePolicies() const
	{
	return iSharedRep.iSimRep->iSinglePolicies;
	}

inline const RRangePolicyArray& TConvToolTester::RangePolicies() const
	{
	return iSharedRep.iSimRep->iRangePolicies;
	}

inline const TSecurityPolicy& TConvToolTester::DefaultReadPolicy() const
	{
	return iSharedRep.iSimRep->GetDefaultReadAccessPolicy();
	}

inline const TSecurityPolicy& TConvToolTester::DefaultWritePolicy() const
	{
	return iSharedRep.iSimRep->GetDefaultWriteAccessPolicy();
	}

inline void TConvToolTester::SetMetaDataOnCreate(TServerSetting& aSetting)
	{
	iSharedRep.SetMetaDataOnCreate(aSetting, 0);
	}
