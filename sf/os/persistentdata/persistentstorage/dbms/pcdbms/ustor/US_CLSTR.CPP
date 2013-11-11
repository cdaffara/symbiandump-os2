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

// Class RClusterMap

void RClusterMap::InsertL( TClusterId aCluster, TClusterId aPrevious )
//
// insert the entry into the map
//
	{
	TIdPair* map = iMap;
	if ( iEntries == iAlloc )
		{	// ensure there is space
		TInt size = iAlloc + EGranularity;
		iMap = map = ( TIdPair* )User::ReAllocL( map, size * sizeof( TIdPair ) );
		iAlloc = size;
		}
	TInt l = 0;
	TInt r = iEntries;
	while ( r > l )
		{
		TInt m = ( l + r ) >> 1;
		TClusterId id = map[ m ].iId;
		__ASSERT( aCluster != id );	// not already present
		if ( aCluster < id )
			r = m;
		else
			l = m + 1;
		}
	TIdPair* p = map + r;
	TIdPair* e = map + iEntries++;
	Mem::Move( p + 1, p, ( TUint8* )e - ( TUint8* )p );
	p->iId = aCluster;
	p->iPreviousId = aPrevious;
	}

RClusterMap::TIdPair* RClusterMap::At( TClusterId aCluster )
	{
	TInt l = 0;
	TInt r = iEntries;
	while ( r > l )
		{
		TInt m = ( l + r ) >> 1;
		TClusterId id = iMap[ m ].iId;
		if ( aCluster < id )
			r = m;
		else if ( aCluster > id )
			l = m + 1;
		else
			return iMap + m;
		}
	return 0;
	}

void RClusterMap::ResetL( TClusterId aHeadCluster )
	{
	iComplete = EFalse;
	iEntries = 0;
	InsertL( aHeadCluster, KNullClusterId );
	iLastMapped = iLastBound = aHeadCluster;
	iSkipped = ESeparation - 1;
	}

TBool RClusterMap::At( TClusterId aCluster, TClusterId& aPreviousCluster )
	{
	TIdPair* p = At( aCluster );
	if ( p )
		aPreviousCluster = p->iPreviousId;
	else if ( aCluster == iLastBound )
		aPreviousCluster = iLastMapped;
	else
		return EFalse;
	return ETrue;
	}

void RClusterMap::AddL( TClusterId aCluster )
	{
	__ASSERT( aCluster != KNullClusterId );
	if ( --iSkipped < 0 )
		{
		InsertL( aCluster, iLastMapped );
		iLastMapped = aCluster;
		iSkipped = ESeparation - 1;
		}
	iLastBound = aCluster;
	}

void RClusterMap::Drop( TClusterId aCluster, TClusterId aNext )
//
// Cluster has been deleted, modify entry to contain the next cluster
// last cluster in table is never deleted
//
	{
	if ( aCluster == iLastBound )
		iLastBound = aNext;
	TIdPair* entry = At( aCluster );
	if ( !entry )
		return;		// not in the sparse map
// remove entry for cluster->prev
	TClusterId prev = entry->iPreviousId;
	Mem::Move( entry, entry + 1, ( TUint8* )( iMap + --iEntries ) - ( TUint8* )entry );
//
	if ( aCluster == iLastMapped )
		iLastMapped = aNext;
	else
		{	// find the referring entry next->cluster
		TIdPair* pnext = iMap;
		while ( pnext->iPreviousId != aCluster )
			{
			++pnext;
			__ASSERT( pnext < iMap + iEntries );
			}
		if ( pnext->iId == aNext )
			{	// referring entry is the next one => drop a link
			pnext->iPreviousId = prev;
			return;
			}
		// adjust next->new
		pnext->iPreviousId = aNext;
		}
	// add in new link to replace deleted one
	InsertL( aNext, prev );	// will not fail allocation as space available
	}

// Class TClusterLinkCache

void TClusterLinkCache::Add( TClusterId aCluster, RClusterMap& aMap )
//
// Add an entry to the cache
//
	{
	__ASSERT( iEnd != NULL );
	TClusterId id;
	__ASSERT( aMap.At( iMap[0], id ) );
//
	TClusterId* p = iEnd;
	if ( p == &iMap[ RClusterMap::ESeparation ] )
		{	// full, requires a shift down
		for ( ; !aMap.At( *p, id ); --p )
			{
			__ASSERT( p > iMap );
			}
		__ASSERT( p > iMap );
		__ASSERT( Has( id ) );

		TClusterId* c = iMap;
		--c;
		while ( p <= iEnd )
			*++c = *p++;
		p = c;
		}
	*++p = aCluster;
	iEnd = p;
	}

void TClusterLinkCache::Add( const TClusterId* aFirst, const TClusterId* aLast )
//
// Add several linked TClusterIds
//
	{
	__ASSERT( iEnd != NULL );
//
	TClusterId* p = iEnd;
	while ( aFirst < aLast && p < &iMap[ RClusterMap::ESeparation ] )
		*++p = *aFirst++;
	iEnd = p;
	}

void TClusterLinkCache::Drop( TClusterId aCluster, TClusterId aNext )
//
// Drop the item if it is in the cache
//
	{
	TClusterId* p = iEnd;
	if ( !p )
		return;
	if ( *p == aCluster )
		{
		*p = aNext;
		return;
		}
	do
		{
		if ( p == iMap )
			return;
		} while ( *--p != aCluster );
	__ASSERT( *( p + 1 ) == aNext );
	for ( ; p < iEnd; ++p )
		*p = *( p + 1 );
	iEnd = p - 1;
	}

TBool TClusterLinkCache::Has( TClusterId aCluster ) const
//
// Check if the cluster id is in the cache
// iEnd==0 is a valid state (empty cache)
//
	{
	for ( const TClusterId* p = iEnd; p >= iMap; )
		{
		if ( *p-- == aCluster )
			return ETrue;
		}
	return EFalse;
	}

TBool TClusterLinkCache::At( TClusterId aCluster, TClusterId& aPrevious ) const
//
// If aCluster is in the cache, return the previous cluster in aPrevious
// iEnd==0 is a valid state (empty cache)
//
	{
	for ( const TClusterId* p = iEnd; p > iMap; )
		{
		if ( *p-- == aCluster )
			{
			aPrevious = *p;
			return ETrue;
			}
		}
	return EFalse;
	}

// Class TClusterDes

void TClusterDes::InternalizeL(RReadStream& aStream)
	{
	aStream>>iNext;
	iMembership=aStream.ReadUint16L();
	}

void TClusterDes::ExternalizeL(RWriteStream& aStream) const
	{
	aStream<<iNext;
	aStream.WriteUint16L(iMembership);
	}


// Class CClusterCache

inline CClusterCache::CClusterCache(CDbStoreDatabase& aDatabase)
	: iDatabase(aDatabase),iCache(_FOFF(CCluster,iLink))
	{}

CClusterCache* CClusterCache::NewL(CDbStoreDatabase& aDatabase)
	{
	CClusterCache* self=new(ELeave) CClusterCache(aDatabase);
	CleanupStack::PushL(self);
	// Add the initial clusters
	for(TInt i=0;i<(EMaxClusters/2);++i)
		{
		self->AddClusterL();
		}
	CleanupStack::Pop();
	return self;
	}

LOCAL_C void DeleteCluster(CCluster* aCluster)
//
// helper function which matches the Apply() prototype
//
	{
	delete aCluster;
	}

CClusterCache::~CClusterCache()
	{
	Apply(DeleteCluster);
	}

LOCAL_C void DiscardCluster(CCluster* aCluster)
//
// helper function which matches the Apply() prototype
//
	{
	aCluster->Discard();
	}

void CClusterCache::Discard()
//
// discard the current changes in all clusters
//
	{
	Apply(DiscardCluster);
	}

LOCAL_C void FlushClusterL(CCluster* aCluster)
//
// helper function which matches the Apply() prototype
//
	{
	aCluster->FlushL();
	}

void CClusterCache::FlushL()
//
// Flush all the clusters in the cache
//
	{
	Apply(FlushClusterL);
	}

CCluster* CClusterCache::Cluster(TClusterId aCluster)
//
// Look for a cluster in the cache
//
	{
	TDblQueIter<CCluster> iter(iCache);
	for (CCluster* cluster;(cluster=iter++)!=0;)
		{
		if (cluster->Id()==aCluster)
			return cluster;
		}
	return 0;
	}

CCluster& CClusterCache::ClusterL(TClusterId aCluster)
//
// Get a cluster from the cache or store and move it to the top of the cache
// Track hits to the two clusters which most recently dropped out of the cache
//
	{
	CCluster* cluster=Cluster(aCluster);	// check if it is cached
	iFollowOnHits<<=2;
	if (!cluster)
		{		// get an empty cluster and read it
		if (aCluster==iCachePlus1)
			{	// the cluster has recently been discarded
			iCachePlus1=iCachePlus2;	// re-sequence the cache follow-on
			iFollowOnHits|=0x1;
			}
		else if (aCluster==iCachePlus2)
			iFollowOnHits|=0x2;	// the cluster has recently been discarded
		cluster=&NewClusterL();
		cluster->ReadL(aCluster);
		}
	return Touch(*cluster);
	}

CCluster& CClusterCache::ClusterL()
//
// Get a new (empty) cluster from the cache and move it to the top
//
	{
	return Touch(NewClusterL());
	}

CCluster& CClusterCache::Touch(CCluster& aCluster)
//
// Move a cluster to the top of the LRU list
//
	{
	aCluster.iLink.Deque();
	iCache.AddFirst(aCluster);
	return aCluster;
	}

CCluster& CClusterCache::AddClusterL()
//
// Add a new cluster to the cache
//
	{
	__ASSERT(iClusters<EMaxClusters);
	CCluster& cluster=*CCluster::NewL(Database());
	iCache.AddLast(cluster);
	++iClusters;
	// move +2 hits into +1 zone and clear +1 hits
	iFollowOnHits=TUint8((TUint(iFollowOnHits)>>1)&0x55);
	return cluster;
	}

CCluster& CClusterCache::NewClusterL()
//
// Get an empty cluster from the cache, but do not touch it
// If the hit detector has registered enough near-misses the cache is expanded
// by adding another cluster object
//
	{
	CCluster* cluster=Cluster(KNullClusterId);	// look for a discarded cluster first
	if (cluster)
		return *cluster;
// check for cache expansion
	TUint detected=iFollowOnHits;
	if ((detected&(detected-1))!=0 && iClusters<EMaxClusters)
		return AddClusterL();
// retire the last cache entry
	cluster=iCache.Last();
	cluster->FlushL();
	iCachePlus2=iCachePlus1;
	iCachePlus1=cluster->Id();
	return *cluster;
	}

void CClusterCache::Apply(void (*aFunc)(CCluster*))
//
// Apply the function paramater to all clusters in the cache
// This function may leave <==> the parameter function may leave
//
	{
	TDblQueIter<CCluster> iter(iCache);
	for (CCluster* cluster;(cluster=iter++)!=0;)
		aFunc(cluster);
	}

// Class CCluster

CCluster* CCluster::NewL(CDbStoreDatabase& aDatabase)
	{
	return new(ELeave) CCluster(aDatabase);
	}

CCluster::~CCluster()
	{
	User::Free(iMap[0]);
	}

void CCluster::AdjustMap(TUint8** aMapEntry,TInt aAdjust)
//
// Adjust all map entries after aMapEntry
//
	{
	do *aMapEntry+=aAdjust; while (++aMapEntry<=&iMap[KMaxClustering]);
	}

TInt CCluster::SetSizeL(TInt aSize)
//
// Set the minimum size for the cluster buffer
// Return the offset between the new and old cells
//
	{
	if (iSize>=aSize)
		return 0;
//
	aSize+=EGranularity-1;		// round to granularity
	aSize&=~(EGranularity-1);
	TUint8* base=iMap[0];
	TInt offset=(TUint8*)User::ReAllocL(base,aSize)-base;
	iSize=aSize;
	if (offset)
		AdjustMap(&iMap[0],offset);
	return offset;
	}

void CCluster::Discard()
//
// discard the current changes
//
	{
	iCluster=KNullClusterId;
	iModified=EFalse;
	}

void CCluster::Create(TClusterId aClusterId)
//
// Create a new cluster
//
	{
	__ASSERT(!iModified);
//
	iCluster=aClusterId;
	iDes.iNext=KNullClusterId;
	iDes.iMembership=0;
	TUint8* base=iMap[0];
	for (TUint8** ptr=&iMap[1];ptr<=&iMap[KMaxClustering];++ptr)
		*ptr=base;
	iModified=ETrue;
	}

void CCluster::Relink(TClusterId aNextClusterId)
//
// Update the cluster to link to a different cluster
//
	{
	iDes.iNext=aNextClusterId;
	iModified=ETrue;
	}

void CCluster::AlterL(MAlter& aAlterer)
//
// alter all records in the cluster
//
	{
	TUint members=iDes.iMembership;
	TUint8* wptr=iMap[0];
	TUint8* rptr=wptr;
	for (TUint8** map=&iMap[0];map<&iMap[KMaxClustering];members>>=1,++map)
		{
		if (members&1)
			{
			TInt size=map[1]-rptr;
			TInt expand=wptr-rptr+aAlterer.RecordExpansion(rptr,size);
			if (expand>0)
				{	// requires more space for alteration
				AdjustL(map,expand+EExpandBuffer,rptr);
				wptr=map[0];	// compensate for possible moving cache
				rptr=map[1]-size;	// record data is at end of this entry
				}
			wptr=aAlterer.AlterRecordL(wptr,rptr,size);
			rptr+=size;
			__ASSERT(wptr<=rptr);
			}
		else
			{
			__ASSERT(map[1]==rptr);
			}
		map[1]=wptr;
		}
	iModified=ETrue;
	}

TPtrC8 CCluster::RecordL(TInt aIndex)
//
// Read the cluster and return the record data
//
	{
	if (!((iDes.iMembership>>aIndex)&1))
		__LEAVE(KErrNotFound);
	return TPtrC8(iMap[aIndex],iMap[aIndex+1]-iMap[aIndex]);
	}

TUint8* CCluster::UpdateL(TInt aIndex,TInt aNewSize)
//
// read the cluster and return a writable descriptor over the new record data
//
	{
	SetRecordL(aIndex,aNewSize);
	iDes.iMembership|=(1<<aIndex);
	return iMap[aIndex];
	}

TBool CCluster::DeleteL(TInt aIndex)
//
// return whether the cluster is empty or not
//
	{
	SetRecordL(aIndex,0);
	iDes.iMembership&=~(1<<aIndex);
	return iDes.iMembership;
	}

void CCluster::FlushL()
	{
	if (iModified)
		{	// Externalize the cluster
		RDbStoreWriteStream cluster(iDatabase);
		cluster.ReplaceLC(iDatabase.Store(),iCluster);
		cluster<<iDes;
		TUint8** map=&iMap[0];
		TUint8* base=*map;
		TUint8* ptr=base;
		for (TUint members=iDes.iMembership;members!=0;members>>=1)
			{
			++map;
			if (members&1)
				{
				TUint8* end=*map;
				cluster << TCardinality(end-ptr);
				ptr=end;
				}
			else
				{
				__ASSERT(*map==ptr);
				}
			}
		cluster.FilterL(cluster.EMixed,iCluster);
		cluster.WriteL(base,ptr-base);
		cluster.CommitL();
		CleanupStack::PopAndDestroy();
		iModified=EFalse;
		}
	}

void CCluster::ReadL(TClusterId aCluster)
//
// Internalize the cluster
//
	{
	__ASSERT(iCluster!=aCluster);
	__ASSERT(!iModified);
//
	iCluster=KNullClusterId;
	RDbStoreReadStream cluster(iDatabase);
	cluster.OpenLC(iDatabase.Store(),aCluster);
	cluster>>iDes;
	TUint8** map=&iMap[0];
	TUint8* base=*map;
	TUint8* ptr=base;
	for (TUint members=iDes.iMembership;members!=0;members>>=1)
		{
		if (members&1)
			{
			TCardinality card;
			cluster >> card;
			TInt size=card;
			if (size>KDbStoreMaxRecordLength)
				__LEAVE(KErrCorrupt);
			ptr+=size;
			}
		*++map=ptr;
		}
	while (map<&iMap[KMaxClustering])
		*++map=ptr;
	TInt len=ptr-base;
	base+=SetSizeL(len);
	cluster.FilterL(cluster.EMixed,aCluster);
	cluster.ReadL(base,len);
	CleanupStack::PopAndDestroy();
	iCluster=aCluster;
	}

void CCluster::SetRecordL(TInt aIndex,TInt aNewSize)
	{
	AdjustL(&iMap[aIndex],iMap[aIndex]+aNewSize-iMap[aIndex+1],iMap[aIndex+1]);
	iModified=ETrue;
	}

void CCluster::AdjustL(TUint8** aMapEntry,TInt aAdjust,TUint8* aData)
//
// Adjust the record at map entry by aAdjust bytes
// Move that entry data as well as the ones following for AlterCluster
//
	{
	if (!aAdjust)
		return;
//
	__ASSERT(aAdjust+aMapEntry[1]>=aMapEntry[0]);	// record cannot go -ve size
	__ASSERT(aData>=aMapEntry[0]);					// must not save data before this record
//
	aData+=SetSizeL(iMap[KMaxClustering]-iMap[0]+aAdjust);
	Mem::Copy(aData+aAdjust,aData,iMap[KMaxClustering]-aData);
	AdjustMap(aMapEntry+1,aAdjust);
	}

// class CCluster::MAlter

TInt CCluster::MAlter::RecordExpansion(const TUint8*,TInt)
//
// default to no expansion
//
	{
	return 0;
	}
