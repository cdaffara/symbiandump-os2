// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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

// Class CDbStoreDatabase
inline CStreamStore& CDbStoreDatabase::Store()
	{
	return *iStore;
	}
	
inline MDbStreamFilter* CDbStoreDatabase::Filter()
	{
	return iFilter;
	}

// Class TDbStoreIndexStats
inline TDbStoreIndexStats::TDbStoreIndexStats() :
	iRefresh(EInvalid),
	iFlags(0)
	{
	}
	
inline TBool TDbStoreIndexStats::IsValid() const
	{
	return iRefresh>=ERefresh;
	}

// Class CDbStoreIndexDef
inline void CDbStoreIndexDef::SetTokenId(TStreamId anId)
	{
	iTokenId=anId;
	}
	
inline TStreamId CDbStoreIndexDef::TokenId() const
	{
	return iTokenId;
	}

// Class TRecordSize
inline TInt TRecordSize::Clustering() const
	{
	return iClustering;
	}
	
inline TInt TRecordSize::InlineLimit() const
	{
	return iInlineLimit;
	}
	
inline TInt TRecordSize::FixedFieldSize(TDbColType aType)
	{
	__ASSERT(aType>EDbColBit&&aType<EDbColText8);
	return FieldSizes[aType];
	}

// Class CDbStoreDef
inline void CDbStoreDef::SetTokenId(TStreamId anId)
	{
	iTokenId=anId;
	}
	
inline TStreamId CDbStoreDef::TokenId() const
	{
	return iTokenId;
	}
	
inline TInt CDbStoreDef::Clustering() const
	{
	return iInfo.Clustering();
	}
	
inline TInt CDbStoreDef::InlineLimit() const
	{
	return iInfo.InlineLimit();
	}
	
// Class TClusterId
inline TClusterId ClusterId(TDbRecordId aRecordId)
	{
	return aRecordId.Value()>>4;
	}
	
inline TInt RecordIndex(TDbRecordId aRecordId)
	{
	return aRecordId.Value()&0xfu;
	}
	
inline TDbRecordId RecordId(TClusterId aId,TInt aIndex)
	{
	__ASSERT(TUint(aIndex)<16u);
	return (aId<<4)+aIndex;
	}
	
inline TClusterId ClusterId(TStreamId aStreamId)
	{
	return aStreamId.Value();
	}
	
// Class RClusterMap
inline RClusterMap::RClusterMap()
	{
	__ASSERT(iMap==0 && iAlloc==0);
	}
	
inline void RClusterMap::Close()
	{
	User::Free(iMap);
	}
	
inline TClusterId RClusterMap::LastBound() const
	{
	return iLastBound;
	}
	
inline void RClusterMap::Complete(TClusterId aLastCluster)
	{
	if (aLastCluster==iLastBound) 
		{
		iComplete=ETrue;
		}
	}
	
inline TBool RClusterMap::IsComplete() const
	{
	return iComplete;
	}
	
inline void RClusterMap::BindL(TClusterId aPrevious,TClusterId aCluster)
	{
	if (aPrevious==iLastBound) 
		{
		AddL(aCluster);
		}
	}
		
// Class TClusterLinkCache
inline void TClusterLinkCache::Invalidate()
	{
	iEnd=NULL;
	}
	
void TClusterLinkCache::Reset(TClusterId aBaseId)
	{
	*(iEnd=iMap)=aBaseId;
	}
	
inline void TClusterLinkCache::Bind(TClusterId aPrevious,TClusterId aCluster,RClusterMap& aMap)
	{
	if (iEnd!=NULL && *iEnd==aPrevious) 
		{
		Add(aCluster,aMap);
		}
	}
		
// Class CCluster
inline CCluster::CCluster(CDbStoreDatabase& aDatabase) : 
	iDatabase(aDatabase)
	{
	}
	
inline TBool CCluster::IsFull() const
	{
	return iMap[KMaxClustering]-iMap[0]>=KClusterLimit;
	}
	
inline TClusterId CCluster::Id() const
	{
	return iCluster;
	}
	
inline const TClusterDes& CCluster::Des() const
	{
	return iDes;
	}
	
// Class CClusterCache
inline CDbStoreDatabase& CClusterCache::Database()
	{
	return iDatabase;
	}
	
inline CStreamStore& CClusterCache::Store()
	{
	return Database().Store();
	}

// Class CDbStoreRecords
inline TClusterId CDbStoreRecords::Head() const
	{
	return iToken.iHead;
	}
	
inline TInt CDbStoreRecords::Count() const
	{
	return iToken.iCount;
	}

// Class CDbStoreTable
inline const CDbStoreDef& CDbStoreTable::Def() const
	{
	return STATIC_CAST(const CDbStoreDef&,CDbTable::Def());
	}
	
inline CDbStoreDatabase& CDbStoreTable::Database()
	{
	return STATIC_CAST(CDbStoreDatabase&,CDbTable::Database());
	}
	
inline CDbStoreRecords& CDbStoreTable::StoreRecordsL()
	{
	return STATIC_CAST(CDbStoreRecords&,CDbTable::RecordsL());
	}
	

// Class CDbStoreCompression
inline void CDbStoreCompression::Inflate()
	{
	__ASSERT(iState==EDecoding);
	iState=EInflating;
	}

