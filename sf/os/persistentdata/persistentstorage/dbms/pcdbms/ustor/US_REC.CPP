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

#include "US_STD.H"

// Class CDbStoreRecords::TIteratorC

class CDbStoreRecords::TIteratorC
	{
	friend class CDbStoreRecords;
public:
	inline TDbRecordId Current() const;
private:
	TClusterDes iDes;
	TDbRecordId iCurrent;
	};

inline TDbRecordId CDbStoreRecords::TIteratorC::Current() const
	{return iCurrent;}


// Class CDbStoreRecords::CIter

NONSHARABLE_CLASS(CDbStoreRecords::CIter) : public CDbRecordIter
	{
public:
	CIter(CDbStoreRecords& aRecords);
private:
	inline CDbStoreRecords& Records() const;
//
	TInt Count() const;
	TDbRecordId CurrentL();
	TBool GotoL(TDbPosition aPosition);
	TBool GotoL(TDbRecordId aRecordId,RDbTableRow& aBuffer);
	TBool SeekL(const TDbLookupKey& aKey,RDbTable::TComparison aComparison);
	TDeleted DoDeletedL(TDbPosition aPosition,TDbRecordId aRecordId,const RDbTableRow* aRow);
private:
	CDbStoreRecords::TIteratorC iIter;
	};

CDbStoreRecords::CIter::CIter(CDbStoreRecords& aRecords)
	: CDbRecordIter(aRecords)
	{}

inline CDbStoreRecords& CDbStoreRecords::CIter::Records() const
	{return STATIC_CAST(CDbStoreRecords&,Host());}

TInt CDbStoreRecords::CIter::Count() const
	{
	return Records().Count();
	}

TDbRecordId CDbStoreRecords::CIter::CurrentL()
	{
	return iIter.Current();
	}

TBool CDbStoreRecords::CIter::GotoL(TDbPosition aPosition)
	{
	return Records().GotoL(aPosition,iIter);
	}

TBool CDbStoreRecords::CIter::GotoL(TDbRecordId aRecordId,RDbTableRow&)
	{
	return Records().GotoL(aRecordId,iIter);
	}

TBool CDbStoreRecords::CIter::SeekL(const TDbLookupKey&,RDbTable::TComparison)
//
// Cannot do this on a table iterator
//
	{
	Panic(EDbCannotSeek);
	return EFalse;
	}

CDbStoreRecords::CIter::TDeleted CDbStoreRecords::CIter::DoDeletedL(TDbPosition aPosition,TDbRecordId,const RDbTableRow*)
//
// reposition to next after a record is deleted
// Previous only required for reversed (index) iterators
//
	{
	return Records().DeletedL(aPosition,iIter) ? EAtRow : ENoRow;
	}


// Class CDbStoreRecords::TToken

void CDbStoreRecords::TToken::ExternalizeL(RWriteStream& aStream) const
	{
	aStream<<iHead<<iNext.Value()<<TCardinality(iCount)<<TUint32(iAutoIncrement);
	}

void CDbStoreRecords::TToken::InternalizeL(RReadStream& aStream)
	{
	aStream>>iHead;
	iNext=aStream.ReadUint32L();
	TCardinality card;
	aStream>>card;
	iCount=card;
	iAutoIncrement=aStream.ReadUint32L();
	}


// Class CDbStoreRecords

CDbStoreRecords::CDbStoreRecords(CClusterCache& aCache)
	: iCache(aCache)
	{}

CDbStoreRecords::~CDbStoreRecords()
	{
	iMap.Close();
	}

TStreamId CDbStoreRecords::CreateL(CClusterCache& aCache)
//
// Create a new record space in the store, do not create a records object
//
	{
	TToken token;
	token.iHead=ClusterId(aCache.Store().ExtendL());
	aCache.ClusterL().Create(token.iHead);
	token.iNext=RecordId(token.iHead,0);
	token.iCount=0;
	token.iAutoIncrement=0;
	RStoreWriteStream strm;
	TStreamId id=strm.CreateLC(aCache.Store());
	strm<<token;
	strm.CommitL();
	CleanupStack::PopAndDestroy();
	return id;
	}

CDbStoreRecords* CDbStoreRecords::NewL(CClusterCache& aCache,const CDbStoreDef& aDef)
//
// Create a record space
//
	{
	CDbStoreRecords* self=new(ELeave) CDbStoreRecords(aCache);
	CleanupStack::PushL(self);
	self->iClustering=aDef.Clustering();
	self->iTokenId=aDef.TokenId();
	CleanupStack::Pop();
	return self;
	}

TBool CDbStoreRecords::RestoreL()
//
// Restore an existing record space from the store
//
	{
	RStoreReadStream strm;
	strm.OpenLC(iCache.Store(),iTokenId);
	strm>>iToken;
	CleanupStack::PopAndDestroy();
	iLinks.Invalidate();
	iMap.ResetL(iToken.iHead);
	return EFalse;
	}

void CDbStoreRecords::DestroyL()
//
// Destroy the record space
//
	{
	iCache.Store().DeleteL(iTokenId);
	}

TInt CDbStoreRecords::CardinalityL(CStreamStore& aStore,const CDbStoreDef& aDef)
//
// Return the record count without constructing the entire table
//
	{
	RStoreReadStream strm;
	strm.OpenLC(aStore,aDef.TokenId());
	TToken token;
	strm>>token;
	CleanupStack::PopAndDestroy();
	return token.iCount;
	}

void CDbStoreRecords::SynchL()
//
// write persistent token to the store
//
	{
	RStoreWriteStream strm;
	strm.ReplaceLC(iCache.Store(),iTokenId);
	strm<<iToken;
	strm.CommitL();
	CleanupStack::PopAndDestroy();
	}

TInt CDbStoreRecords::DiscardL(TClusterId& aCluster)
//
// discard the cluster as part of the incremental drop
// aCluster is updated to the next cluster, the number of records contained is returned
//
	{
	TClusterDes des;
	DesL(des,aCluster);
	CCluster* cluster=iCache.Cluster(aCluster);
	if (cluster)
		cluster->Discard();
	iCache.Store().DeleteL(aCluster);
	aCluster=des.iNext;
	TInt records=0;
	for (TUint members=des.iMembership;members;members&=members-1)
		++records;
	return records;
	}

TClusterId CDbStoreRecords::AlterL(TClusterId aCluster,CCluster::MAlter& aAlterer)
	{
	CCluster& cluster=iCache.ClusterL(aCluster);
	cluster.AlterL(aAlterer);
	return cluster.Des().iNext;
	}

TPtrC8 CDbStoreRecords::ReadL(TDbRecordId aRecordId) const
	{
	return iCache.ClusterL(ClusterId(aRecordId)).RecordL(RecordIndex(aRecordId));
	}

TUint CDbStoreRecords::AutoIncrementL()
//
// Provide the next value for an auto-increment column
//
	{
	return iToken.iAutoIncrement++;
	}

TUint8* CDbStoreRecords::UpdateRecordL(TDbRecordId aRecordId,TInt aNewSize)
//
// read the cluster and return a writable descriptor over the new record data
//
	{
	return iCache.ClusterL(ClusterId(aRecordId)).UpdateL(RecordIndex(aRecordId),aNewSize);
	}


TUint8* CDbStoreRecords::DoNewL(TInt aRecordSize)
//
// Phase 1 of appending a records
//
	{
	return UpdateRecordL(iToken.iNext,aRecordSize);
	}

TDbRecordId CDbStoreRecords::AppendL()
//
// Phase 2 of appending a record
//
	{
	TDbRecordId id=iToken.iNext;
	TClusterId clusterId=ClusterId(id);
	CCluster* cluster=iCache.Cluster(clusterId);
	__ASSERT(cluster);
	TInt nextIndex=RecordIndex(id)+1;
	if (nextIndex>=iClustering || cluster->IsFull())
		{
		TClusterId newcluster=ClusterId(iCache.Store().ExtendL());
		cluster->Relink(newcluster);
		cluster->FlushL();
		cluster->Create(newcluster);
		iMap.BindL(clusterId,newcluster);
		iLinks.Bind(clusterId,newcluster,iMap);
		iToken.iNext=RecordId(newcluster,0);
		}
	else
		iToken.iNext=RecordId(clusterId,nextIndex);
	++iToken.iCount;
	return id;
	}

TUint8* CDbStoreRecords::DoReplaceL(TDbRecordId aRecordId,TInt aRecordSize)
	{
	return UpdateRecordL(aRecordId,aRecordSize);
	}

void CDbStoreRecords::DoEraseL(TDbRecordId aRecordId)
	{
	TClusterId clusterId=ClusterId(aRecordId);
	CCluster& cluster=iCache.ClusterL(clusterId);
	if (!cluster.DeleteL(RecordIndex(aRecordId)) && clusterId!=ClusterId(iToken.iNext))
		{	// cluster is now empty, but don't drop the last cluster, coz it hasn't all been used!
		TClusterDes des;
		TClusterId prev=PreviousClusterL(des,clusterId);
		TClusterId next=cluster.Des().iNext;	// next cluster
		cluster.Discard();						// discard the cluster
		iCache.Store().DeleteL(clusterId);
		if (prev!=KNullClusterId)
			iCache.ClusterL(prev).Relink(next);
		else
			iToken.iHead=next;
		iLinks.Drop(clusterId,next);
		iMap.Drop(clusterId,next);
		}
	--iToken.iCount;
	}

CDbRecordIter* CDbStoreRecords::IteratorL()
	{
	return new(ELeave) CIter(*this);
	}

void CDbStoreRecords::CompleteMapL()
	{
	TClusterId cluster=iMap.LastBound();
	TClusterDes des;
	DesL(des,cluster);
	do cluster=NextClusterL(des,cluster); while (cluster!=KNullClusterId);
	}

void CDbStoreRecords::DesL(TClusterDes& aDes,TClusterId aCluster)
//
// Read just the cluster descriptor
//
	{
	CCluster* cluster=iCache.Cluster(aCluster);
	if (cluster)
		aDes=cluster->Des();
	else
		{
		RStoreReadStream stream;
		stream.OpenLC(iCache.Store(),aCluster);
		stream>>aDes;
		CleanupStack::PopAndDestroy();
		}
	}

TClusterId CDbStoreRecords::NextClusterL(TClusterDes& aDes,TClusterId aCluster)
	{
	TClusterId next=aDes.iNext;
	if (next==KNullClusterId)
		iMap.Complete(aCluster);
	else
		{
		iMap.BindL(aCluster,next);
		iLinks.Bind(aCluster,next,iMap);
		DesL(aDes,next);
		}
	return next;
	}

TBool CDbStoreRecords::LocateL(TClusterId aCluster)
//
// Locate the cluster in the table. If not present return EFalse
// If present fill up the cluster link cache with the loop
// containing the predecessor to aCluster
// aDes will have the previous cluster des
//
	{
	TClusterId cluster=aCluster;
	__ASSERT(aCluster!=iToken.iHead);
	__ASSERT(!iLinks.At(aCluster,cluster));
//
	if (!iMap.IsComplete())
		CompleteMapL();
//
	TClusterDes des;
	TClusterId links[RClusterMap::ESeparation];
	TClusterId* p=links;
	for (TInt n=RClusterMap::ESeparation;n>0;--n)
		{
		*p++=cluster;
		TBool r=iMap.At(cluster,cluster);
		DesL(des,cluster);
		if (r)
			{
			__ASSERT(cluster!=KNullClusterId);	// only iHead->Null
			iLinks.Reset(cluster);
			while (aCluster!=des.iNext)
				cluster=NextClusterL(des,cluster);
			iLinks.Add(links,p);
			return ETrue;
			}
		cluster=des.iNext;
		}
	return EFalse;	// not in this table!
	}

TClusterId CDbStoreRecords::PreviousClusterL(TClusterDes& aDes,TClusterId aCluster)
	{
	if (aCluster==iToken.iHead)
		return KNullClusterId;
	if (!iLinks.At(aCluster,aCluster))
		{
		TBool dbgchk= LocateL(aCluster);
		__ASSERT(dbgchk);
		dbgchk= iLinks.At(aCluster,aCluster);
		__ASSERT(dbgchk);
		}
	DesL(aDes,aCluster);
	return aCluster;
	}

TBool CDbStoreRecords::GotoL(TDbPosition aPosition,TIteratorC& anIterator)
	{
	TClusterId cluster=ClusterId(anIterator.iCurrent);
	TInt index=RecordIndex(anIterator.iCurrent);
	switch (aPosition)
		{
	default:
		__ASSERT(0);
	case EDbFirst:
		DesL(anIterator.iDes,cluster=iToken.iHead);
		iLinks.Reset(cluster);
		index=-1;
		// drop through to next
	case EDbNext:
		for (;;)
			{
			TUint membership=anIterator.iDes.iMembership;
			while (++index<KMaxClustering)
				{
				if ((membership>>index)&1)
					{
					__ASSERT(cluster!=ClusterId(iToken.iNext)||index<RecordIndex(iToken.iNext));
					anIterator.iCurrent=RecordId(cluster,index);
					return ETrue;
					}
				}
			cluster=NextClusterL(anIterator.iDes,cluster);
			if (cluster==KNullClusterId)
				return EFalse;	// ran out of data
			index=-1;
			}
	case EDbLast:
		DesL(anIterator.iDes,cluster=ClusterId(iToken.iNext));
		index=KMaxClustering;
		// drop through to previous
	case EDbPrevious:
		for (;;)
			{
			TUint membership=anIterator.iDes.iMembership;
			while (--index>=0)
				{
				if ((membership>>index)&1)
					{
					anIterator.iCurrent=RecordId(cluster,index);
					return ETrue;
					}
				}
			__ASSERT(index==-1);
			cluster=PreviousClusterL(anIterator.iDes,cluster);
			if (cluster==KNullClusterId)
				return EFalse;	// ran out of data
			index=KMaxClustering;
			}
		}
	}

TBool CDbStoreRecords::DeletedL(TDbPosition aPosition,TIteratorC& anIterator)
//
// Record has been deleted
//
	{
	anIterator.iDes.iMembership&=~(1<<RecordIndex(anIterator.iCurrent));
	return GotoL(aPosition,anIterator);
	}

TBool CDbStoreRecords::GotoL(TDbRecordId aRecordId,TIteratorC& anIterator)
//
// Set the iterator to the record id, return false if the record is not present
//
	{
	TClusterId cluster=ClusterId(aRecordId);
	if (cluster!=iToken.iHead && !iLinks.Has(cluster) && !LocateL(cluster))
		return EFalse;
	anIterator.iCurrent=aRecordId;
	DesL(anIterator.iDes,cluster);
	return (anIterator.iDes.iMembership>>RecordIndex(aRecordId))&1;
	}

TBool CDbStoreRecords::ExistsL(TDbRecordId aRecordId)
//
// Ensure that the record is in this table
//
	{
	TIteratorC iter;
	return GotoL(aRecordId,iter);
	}

// Class HUnicodeCompressor

NONSHARABLE_CLASS(HUnicodeCompressor) : public TStreamFilter
	{
public:
	HUnicodeCompressor(MStreamBuf* aSink);
private:
	void DoRelease();
	void DoSynchL();
	TInt Capacity(TInt aMaxLength);
	TInt FilterL(TAny* aPtr,TInt aMaxLength,const TUint8*& aFrom,const TUint8* anEnd);
private:
	enum {EFlushBufferSize=16};
private:
	TUnicodeCompressor iCompressor;
	};

HUnicodeCompressor::HUnicodeCompressor(MStreamBuf* aSink)
	{
	Set(aSink,EAttached|EWrite);
	}

void HUnicodeCompressor::DoRelease()
	{
	TStreamFilter::DoRelease();
	delete this;
	}

TInt HUnicodeCompressor::Capacity(TInt aMaxLength)
//
// Return the maximum guaranteed input used for aMaxLength output.
// SUC at worst expands n chars to 3n bytes
//
	{
	aMaxLength=(aMaxLength+2)/3;	// # chars input guaranteed
	return aMaxLength*2;			// # bytes
	}

TInt HUnicodeCompressor::FilterL(TAny* aPtr,TInt aMaxLength,const TUint8*& aFrom,const TUint8* aEnd)
	{
	TMemoryUnicodeSource source(reinterpret_cast<const TUint16*>(aFrom));
	TInt used;
	iCompressor.CompressL(reinterpret_cast<TUint8*>(aPtr),source,aMaxLength,(aEnd-aFrom)>>1,&aMaxLength,&used);
	aFrom+=used<<1;
	return aMaxLength;
	}

void HUnicodeCompressor::DoSynchL()
	{
	if (IsCommitted())
		return;
//
	TUint8 buf[EFlushBufferSize];
	TInt emit;
	iCompressor.FlushL(buf,EFlushBufferSize,emit);
	if (emit)
		EmitL(buf,emit);
//
	TStreamFilter::DoSynchL();
	Committed();
	}

// Class HUnicodeExander

NONSHARABLE_CLASS(HUnicodeExpander) : public TStreamFilter
	{
public:
	HUnicodeExpander(MStreamBuf* aSource);
private:
	void DoRelease();
//	void DoSynchL();
	TInt Capacity(TInt aMaxLength);
	TInt FilterL(TAny* aPtr,TInt aMaxLength,const TUint8*& aFrom,const TUint8* anEnd);
private:
	enum {EFlushBufferSize=16};
private:
	TUnicodeExpander iExpander;
	};

HUnicodeExpander::HUnicodeExpander(MStreamBuf* aSource)
	{
	Set(aSource,EAttached|ERead);
	}

void HUnicodeExpander::DoRelease()
	{
	TStreamFilter::DoRelease();
	delete this;
	}

TInt HUnicodeExpander::Capacity(TInt aMaxLength)
//
// Return the maximum guaranteed input used for aMaxLength output.
// SUC at worst expands n chars to 3n bytes
//
	{
	return aMaxLength>>1;			// best expansion from ASCII chars
	}

TInt HUnicodeExpander::FilterL(TAny* aPtr,TInt aMaxLength,const TUint8*& aFrom,const TUint8* aEnd)
	{
	TMemoryUnicodeSink sink(reinterpret_cast<TUint16*>(aPtr));
	TInt used;
	iExpander.ExpandL(sink,aFrom,aMaxLength>>1,aEnd-aFrom,&aMaxLength,&used);
	aFrom+=used;
	return aMaxLength<<1;
	}

/*
void HUnicodeExpander::DoSynchL()
	{
	if (IsCommitted())
		return;
//
//	TUint8 buf[EFlushBufferSize];
//	TInt emit;
//	iCompressor.FlushL(buf,EFlushBufferSize,&emit);
//	if (emit)
//		EmitL(buf,emit);
//
	TStreamFilter::DoSynchL();
	Committed();
	}
*/

// Class CDbStoreBlobs

CDbStoreBlobs::CDbStoreBlobs(CDbStoreDatabase& aDatabase,TInt aInlineLimit)
	: iDatabase(aDatabase)
	{
	SetInlineLimit(aInlineLimit);
	}

MStreamBuf* CDbStoreBlobs::DoCreateL(TDbBlobId& aBlobId,TDbColType aType)
	{
	__ASSERT(TDbCol::IsLong(aType));
//
	RDbStoreWriteStream strm(iDatabase);
	aBlobId=strm.CreateLC(iDatabase.Store()).Value();
	strm.FilterL(aType!=EDbColLongBinary?strm.EText:strm.EBinary,aBlobId);
	MStreamBuf* blob=strm.Sink();
	if (aType==EDbColLongText16)
		blob=new(ELeave) HUnicodeCompressor(blob);
	CleanupStack::Pop();
	return blob;
	}

MStreamBuf* CDbStoreBlobs::ReadL(TDbBlobId aBlobId,TDbColType aType) const
	{
	__ASSERT(TDbCol::IsLong(aType));
//
	RDbStoreReadStream strm(iDatabase);
	strm.OpenLC(iDatabase.Store(),aBlobId);
	strm.FilterL(aType!=EDbColLongBinary?strm.EText:strm.EBinary,aBlobId);
	MStreamBuf* blob=strm.Source();
	if (aType==EDbColLongText16)
		blob=new(ELeave) HUnicodeExpander(blob);
	CleanupStack::Pop();
	return blob;
	}

void CDbStoreBlobs::DoDeleteL(TDbBlobId aBlobId)
	{
	iDatabase.Store().DeleteL(TStreamId(aBlobId));
	}
