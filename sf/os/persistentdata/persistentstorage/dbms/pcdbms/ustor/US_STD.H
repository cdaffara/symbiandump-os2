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

#include <e32std.h>
#include <u32std.h>
#include "D32REC.H"
#include "D32STOR.H"
#include <s32buf.h>
//#include <s32crypt.h>
#include <s32btree.h>
#include <s32ucmp.h>
//#include <e32hal.h>
#include <e32math.h>
#include "D32Assert.h"

// classes defined
class TDbStoreIndexStats;
class CDbStoreIndexDef;
class TRecordSize;
class CDbStoreDef;
class RClusterMap;
class TClusterLinkCache;
struct TClusterDes;
class CCluster;
class CClusterCache;
class CDbStoreRecords;
class CDbStoreBlobs;
class CDbStoreIndex;
class CDbStoreTable;
class RDbStoreReadStream;
class RDbStoreWriteStream;
class MDbStreamFilter;
class CDbStoreCompression;

/**
@internalComponent
*/
enum TStorePanic
	{
	EDbUnimplemented,
	EDbNoStore,
	EDbCannotSeek,
	EDbNotFixedFieldType,
	EDbWrongType
	};

/**
@internalComponent
*/
GLREF_C void Panic(TStorePanic aPanic);

/**
@internalComponent
*/
GLREF_D const TDbDriver KBuiltinDriver;

/**
@internalComponent
*/
const TUid KDbmsStoreDatabase={268435561};

/**
@internalComponent
*/
const TInt KDbStoreMaxRecordLength=0x200A;		// v5.10 32 nullable longtext columns
const TInt KClusterLimit=0x1000;
const TInt KMaxClustering=16;
const TInt KBlobRefSize=sizeof(TDbBlobId)+sizeof(TInt32);
const TInt KMinInlineLimit=16;
const TInt KMaxIndexKeySize=248;

/**
@internalComponent
*/
class TDbStoreIndexStats
	{
public:
	/**
	@internalComponent
	*/
	class TBound
		{
	public:
		inline void Set(TReal64 aBound);
		void Set(const TInt64& aBound);
		void Set(const TText8* aPtr,TInt aLen,const TTextOps& aConv);
		void Set(const TText16* aPtr,TInt aLen,const TTextOps& aConv);
		void Set(const TDbLookupKey::SColumn& aBound,const TTextOps& aConv);
	public:
		TReal64 iValue;
		};
	enum TType {EContinuous,EDiscrete};
public:
	inline TDbStoreIndexStats();
//
	void InternalizeL(RReadStream& aStream);
	void ExternalizeL(RWriteStream& aStream) const;
//
	inline void Reset(TInt aCardinality=0);
	inline void Inc();
	inline void Dec();
	inline void Refresh(TType aType);
//
	inline TBool IsValid() const;
	inline TBool NeedsRefresh() const;
	inline TInt Cardinality() const;
	TInt Span(TUint aInclusion,const TDbLookupKey* aLower,const TDbLookupKey* aUpper,const TTextOps& aConv) const;
private:
	inline void Touch();
	TInt ReverseSpan(TUint aInclusion,const TDbLookupKey* aLower,const TDbLookupKey* aUpper,const TTextOps& aConv) const;
private:
	enum {ERefreshFactor=3};
	enum {EInvalid=-1,ERefresh=0};
	enum {ERefreshShift=2+ERefreshFactor,EFlagsMask=(1<<ERefreshShift)-1};
	enum {EFlgDiscrete=0x1,EFlgClustering=0x2};
private:
	TInt iCardinality;
	TInt iRefresh;
	TUint iFlags;
public:
	TBound iLower;
	TBound iUpper;
	};

/**
@internalComponent
*/
class CDbStoreIndexDef : public CDbTableIndexDef
	{
public:
	static CDbStoreIndexDef* NewLC(const TDesC& aName,const CDbKey& aKey,const HDbColumnSet& aColumns);
	static CDbStoreIndexDef* NewL(RReadStream& aStream);
	void ExternalizeL(RWriteStream& aStream) const;
	inline void SetTokenId(TStreamId anId);
	inline TStreamId TokenId() const;
//
	static TInt KeySize(const TDbKeyCol& aKeyCol,const TDbColumnDef& aColumn);
	static void CheckSizeL(const CDbKey& aKey,const HDbColumnSet& aColSet);
private:
	CDbStoreIndexDef();
	static CDbStoreIndexDef* NewLC(const TDesC& aName);
private:
	TStreamId iTokenId;
public:
	__MUTABLE TDbStoreIndexStats iStats;	// cache the statistics here
	};

/**
@internalComponent
*/
class TRecordSize
	{
public:
	TBool Set(const HDbColumnSet& aColumns);
	inline TInt Clustering() const;
	inline TInt InlineLimit() const;
//
	static void CheckSizeL(const HDbColumnSet& aColumns);
	static TInt InlineLimit(const HDbColumnSet& aColumns);
//
	static inline TInt FixedFieldSize(TDbColType aType);
private:
	TInt iClustering;
	TInt iInlineLimit;
private:
	static TUint8 const FieldSizes[];
	};

/**
@internalComponent
*/
NONSHARABLE_CLASS(CDbStoreDef) : public CDbTableDef
	{
public:
	static CDbStoreDef* NewLC(const TDesC& aName,const CDbColSet& aColSet);
	static CDbStoreDef* NewL(RReadStream& aStream);
	void ExternalizeL(RWriteStream& aStream) const;
	inline void SetTokenId(TStreamId anId);
	inline TStreamId TokenId() const;
	inline TInt Clustering() const;
	inline TInt InlineLimit() const;
	void Changed();
private:
	void AlteredColumnSetL(HDbColumnSet& aSet,const CDbColSet& aChange,const CDbColSet& aAdd);
protected:
	CDbStoreDef();
private:
	static CDbStoreDef* NewLC(const TDesC& aName,TInt aColumnCount);
private:
	TStreamId iTokenId;
	TRecordSize iInfo;
	};

/**
@internalComponent
*/
typedef TUint32 TClusterId;

/**
@internalComponent
*/
const TClusterId KNullClusterId=0;

/**
@internalComponent
*/
inline TClusterId ClusterId(TDbRecordId aRecordId);
inline TInt RecordIndex(TDbRecordId aRecordId);
inline TDbRecordId RecordId(TClusterId aId,TInt aIndex);
inline TClusterId ClusterId(TStreamId aStreamId);

/**
@internalComponent
*/
class RClusterMap
	{
private:
	enum {EGranularity=8};
	class TIdPair
		{
	public:
		TClusterId iId;
		TClusterId iPreviousId;
		};
public:
	enum {ESeparation=16};
public:
	inline RClusterMap();
	inline void Close();
//
	void ResetL(TClusterId aHeadCluster);
	inline void BindL(TClusterId aPrevious,TClusterId aCluster);
	void Drop(TClusterId aCluster,TClusterId aNext);
	TBool At(TClusterId aCluster,TClusterId& aPrevious);
	inline TClusterId LastBound() const;
	inline void Complete(TClusterId aLastCluster);
	inline TBool IsComplete() const;
private:
	void AddL(TClusterId aCluster);
	void InsertL(TClusterId aCluster,TClusterId aPrevious);
	TIdPair* At(TClusterId aCluster);
private:
	TIdPair* iMap;
	TInt iEntries;
	TInt iAlloc;
	TClusterId iLastBound;
	TClusterId iLastMapped;
	TInt iSkipped;
	TBool iComplete;
	};

/**
@internalComponent
*/
class TClusterLinkCache
	{
public:
	inline void Invalidate();
	inline void Reset(TClusterId aBaseId);
	inline void Bind(TClusterId aPrevious,TClusterId aCluster,RClusterMap& aMap);
	void Add(const TClusterId* aFirst,const TClusterId* aLast);
	void Drop(TClusterId aCluster,TClusterId aNext);
	TBool Has(TClusterId aCluster) const;
	TBool At(TClusterId aCluster,TClusterId& aPrevious) const;
private:
	void Add(TClusterId aCluster,RClusterMap& aMap);
private:
	TClusterId iMap[RClusterMap::ESeparation+1];
	TClusterId* iEnd;
	};

/**
@internalComponent
*/
struct TClusterDes
	{
public:
	void InternalizeL(RReadStream& aStream);
	void ExternalizeL(RWriteStream& aStream) const;
public:
	TClusterId iNext;
	TUint32 iMembership;
	};

/**
@internalComponent
*/
NONSHARABLE_CLASS(CCluster) : public CBase
	{
	friend class CClusterCache;
private:
	enum {EGranularity=0x200};
	enum {EExpandBuffer=0x40};
public:
	/**
	@internalComponent
	*/
	NONSHARABLE_CLASS(MAlter)
		{
	public:
		virtual TInt RecordExpansion(const TUint8* aRec,TInt aLength);
		virtual TUint8* AlterRecordL(TUint8* aWPtr,const TUint8* aRPtr,TInt aLength) =0;
		};
public:
	static CCluster* NewL(CDbStoreDatabase& aDatabase);
	~CCluster();
//
	void FlushL();
	void Discard();
//
	void Create(TClusterId aClusterId);
	void ReadL(TClusterId aCluster);
	void Relink(TClusterId aNextClusterId);
	void AlterL(MAlter& anAlterer);
//
	TPtrC8 RecordL(TInt aIndex);
	TUint8* UpdateL(TInt aIndex,TInt aNewSize);
	TBool DeleteL(TInt aIndex);
	inline TBool IsFull() const;
//
	inline TClusterId Id() const;
	inline const TClusterDes& Des() const;
private:
	CCluster(CDbStoreDatabase& aDatabase);
//
	TInt SetSizeL(TInt aSize);
	void AdjustMap(TUint8** aMapEntry,TInt aAdjust);
	void SetRecordL(TInt aIndex,TInt aNewSize);
	void AdjustL(TUint8** aMapEntry,TInt aAdjust,TUint8* aData);
private:
	TDblQueLink iLink;
	CDbStoreDatabase& iDatabase;
	TInt iSize;
	TClusterId iCluster;
	TClusterDes iDes;
	TBool iModified;
	TUint8* iMap[KMaxClustering+1];
	};

/**
@internalComponent
*/
NONSHARABLE_CLASS(CClusterCache) : public CBase
	{
	#ifndef MAX_CLUSTER_CACHE_SIZE
		#define MAX_CLUSTER_CACHE_SIZE 8
	#else
		#if MAX_CLUSTER_CACHE_SIZE < 4
			#error "MAX_CLUSTER_CACHE_SIZE macro value can't be less than 4"
		#endif
	#endif
	enum {EMaxClusters=MAX_CLUSTER_CACHE_SIZE};
public:
	static CClusterCache* NewL(CDbStoreDatabase& aDatabase);
	~CClusterCache();
//
	void FlushL();
	void Discard();
//
	CCluster* Cluster(TClusterId aCluster);
	CCluster& ClusterL(TClusterId aCluster);
	CCluster& ClusterL();
//
	inline CDbStoreDatabase& Database();
	inline CStreamStore& Store();
private:
	inline CClusterCache(CDbStoreDatabase& aDatabase);
//
	void Apply(void (*aFunc)(CCluster*));
	CCluster& AddClusterL();
	CCluster& NewClusterL();
	CCluster& Touch(CCluster& aCluster);
private:
	CDbStoreDatabase& iDatabase;
	TClusterId iCachePlus1;
	TClusterId iCachePlus2;
	TUint8 iFollowOnHits;
	TUint8 iClusters;
	TDblQue<CCluster> iCache;
	};

/**
@internalComponent
*/
NONSHARABLE_CLASS(CDbStoreRecords) : public CDbRecordSpace
	{
public:
	class TIteratorC;
	class CIter;
public:
	static TStreamId CreateL(CClusterCache& aCache);
	static CDbStoreRecords* NewL(CClusterCache& aCache,const CDbStoreDef& aDef);
//
	static TInt CardinalityL(CStreamStore& aStore,const CDbStoreDef& aDef);
//
	TBool GotoL(TDbPosition aPosition,TIteratorC& anIterator);
	TBool GotoL(TDbRecordId aRecordId,TIteratorC& anIterator);
	TBool DeletedL(TDbPosition aPosition,TIteratorC& anIterator);
//
	inline TClusterId Head() const;
	inline TInt Count() const;
	TInt DiscardL(TClusterId& aCluster);
	TClusterId AlterL(TClusterId aCluster,CCluster::MAlter& anAlterer);
	void DestroyL();
// providing for CDbRecordSpace
	TBool ExistsL(TDbRecordId aRecordId);
	TPtrC8 ReadL(TDbRecordId aRecordId) const;
	TUint8* DoReplaceL(TDbRecordId aRecordId,TInt aRecordSize);
	TUint AutoIncrementL();
	TUint8* DoNewL(TInt aRecordSize);
	TDbRecordId AppendL();
	void DoEraseL(TDbRecordId aRecordId);
	CDbRecordIter* IteratorL();
	TBool RestoreL();
	void SynchL();
private:
	CDbStoreRecords(CClusterCache& aCache);
	~CDbStoreRecords();
// 
	TUint8* UpdateRecordL(TDbRecordId aRecordId,TInt aNewSize);
	void DesL(TClusterDes& aDes,TClusterId aCluster);
	void CompleteMapL();
	TClusterId NextClusterL(TClusterDes& aDes,TClusterId aCluster);
	TClusterId PreviousClusterL(TClusterDes& aDes,TClusterId aCluster);
	TBool LocateL(TClusterId aCluster);
private:
	class TToken
		{
	public:
		void InternalizeL(RReadStream& aStream);
		void ExternalizeL(RWriteStream& aStream) const;
	public:
		TClusterId iHead;
		TDbRecordId iNext;
		TInt iCount;
		TUint iAutoIncrement;
		};
private:
	CClusterCache& iCache;
	TStreamId iTokenId;
	TToken iToken;
	TInt iClustering;
	TClusterLinkCache iLinks;
	RClusterMap iMap;
	};

/**
@internalComponent
*/
NONSHARABLE_CLASS(CDbStoreBlobs) : public CDbBlobSpace
	{
public:
	CDbStoreBlobs(CDbStoreDatabase& aDatabase,TInt aInlineLimit);
//
	MStreamBuf* DoCreateL(TDbBlobId &aBlobId,TDbColType aType);
	MStreamBuf* ReadL(TDbBlobId aBlobId,TDbColType aType) const;
	void DoDeleteL(TDbBlobId aBlobId);
private:
	CDbStoreDatabase& iDatabase;
	};

/**
@internalComponent
*/
NONSHARABLE_CLASS(CDbStoreIndex) : public CDbRecordIndex
	{
public:
	class HKey;
	class HDupKey;
	class CIter;
	class CDiscarder;
	class CRecover;
	class CRepair;
public:
	static CDbStoreIndex* NewL(CDbStoreDatabase& aDatabase,const CDbStoreIndexDef& aDef,const CDbTableDef& aTable);
	~CDbStoreIndex();
//
	static TStreamId CreateL(CDbStoreDatabase& aDatabase,const CDbStoreIndexDef& aDef);
	static TBool IsDamagedL(CDbStoreDatabase& aDatabase,const CDbStoreIndexDef& aDef);
//
	inline HKey& Key() const;
	inline const TBtree& Tree() const;
	inline TInt Count() const;
	void RepairL();
	void DiscardL();
	void DestroyL();
private:
	CDbStoreIndex(CDbStoreDatabase& aDatabase,const CDbStoreIndexDef& aDef);
//
	void RefreshStatsL();
// CDbTableIndex framework
	TFind FindL(TDbRecordId aRecordId,const RDbTableRow& aRow);
	TBool DoInsertL(TDbRecordId aRecordId,const RDbTableRow& aRow);
	void DoDeleteL(TDbRecordId aRecordId,const RDbTableRow& aRow);
	CDbRecordIter* IteratorL(TUint aInclusion,const TDbLookupKey* aLowerBound,const TDbLookupKey* aUpperBound);
	TBool RestoreL();
	void AboutToModifyL();
	void SynchL();
private:
	CDbStoreDatabase& iDatabase;
	TStreamId iTokenId;
	TBtree iTree;
	HKey* iKey;
	TBtreeInlineLeafOrg iLeafOrg;
	TBtreeInlineIndexOrg iIndexOrg;
	TDbStoreIndexStats& iStats;
	};

/**
@internalComponent
*/
NONSHARABLE_CLASS(CDbStoreIndex::CDiscarder) : public CDbTableDatabase::CStepper
	{
public:
	CDiscarder();
	~CDiscarder();
	TInt Open(CDbStoreIndex* anIndex);
private:
	TInt StepL(TInt aStep);
private:
	CDbStoreIndex* iIndex;
	};

/**
@internalComponent
*/
NONSHARABLE_CLASS(CDbStoreTable) : public CDbTable
	{
public:
	class CDiscarder;
	class CAlter;
	class CCompressor;
	friend class CDiscarder;
	friend class CAlter;
	friend class CCompressor;
public:
	CDbStoreTable(CDbStoreDatabase& aDatabase,const CDbTableDef& aDef);
private:
	inline const CDbStoreDef& Def() const;
	inline CDbStoreDatabase& Database();
	inline CDbStoreRecords& StoreRecordsL();
//
	TInt RowSize(const TUint8* aRec,TInt aLength);
	const TUint8* CopyToRow(TDbCell* aCell,TInt aSize,const TUint8* aRec);
	TUint8* AlterRecordL(TUint8* aWPtr,const TUint8* aRPtr,TInt aLength,TInt aInlineLimit);
// providing for CDbTable framework
	TInt IndexSpanL(const CDbTableIndexDef& aIndex,TUint aInclusion,const TDbLookupKey* aLowerBound,const TDbLookupKey* aUpperBound);
	CDbRecordSpace* RecordSpaceL();
	CDbBlobSpace* BlobSpaceL();
	CDbRecordIndex* RecordIndexL(const CDbTableIndexDef& anIndex);
	void CopyToRowL(RDbRow& aRow,const TDesC8& aRecord);
	TInt RecordLength(const RDbRow& aRow);
	TInt OptimizedRowLength(const RDbRow& aRow);
	void CopyFromRow(TUint8* aRecord,const RDbRow& aRow);
	};

/**
@internalComponent
*/
NONSHARABLE_CLASS(CDbStoreTable::CDiscarder) : public CDbTableDatabase::CStepper, public CCluster::MAlter
	{
private:
	enum {EDiscardClusters=32,EBlobDiscardClusters=2};
public:
	CDiscarder();
	~CDiscarder();
	TInt OpenL(CDbStoreTable* aTable);
private:
	TInt StepL(TInt aStep);
	TUint8* AlterRecordL(TUint8* aWPtr,const TUint8* aRPtr,TInt aLength);
private:
	CDbStoreTable* iTable;
	CDbStoreRecords* iRecords;
	TClusterId iCluster;
	RDbRow iRow;
	};

/**
@internalComponent
*/
NONSHARABLE_CLASS(CDbStoreTable::CAlter) : public CDbTableDatabase::CStepper, public CCluster::MAlter
	{
public:
	CAlter();
	~CAlter();
	void OpenL(CDbStoreTable* aTable,const HDbColumnSet& aNewSet);
private:
	TUint8* AlterRecordL(TUint8* aWPtr,const TUint8* aRPtr,TInt aLength);
	TInt RecordExpansion(const TUint8* aRec,TInt aLength);
	TInt StepL(TInt aStep);
private:
	TInt iExpansion;
	TInt iInlineLimit;
	CDbStoreTable* iTable;
	CDbStoreRecords* iRecords;
	TClusterId iCluster;
	TInt iStep;
	};

/**
@internalComponent
*/
class RDbStoreReadStream : public RStoreReadStream
	{
public:
	enum TType {EMixed,EBinary,EText};
public:
	inline RDbStoreReadStream(CDbStoreDatabase& aDatabase)
		:iDatabase(aDatabase)
		{}
	void FilterL(TType aType,TUint32 aInit);
private:
	CDbStoreDatabase& iDatabase;
	};

/**
@internalComponent
*/
class RDbStoreWriteStream : public RStoreWriteStream
	{
public:
	enum TType {EMixed,EBinary,EText};
public:
	inline RDbStoreWriteStream(CDbStoreDatabase& aDatabase)
		:iDatabase(aDatabase)
		{}
	void FilterL(TType aType,TUint32 aInit);
private:
	CDbStoreDatabase& iDatabase;
	};

/**
@internalComponent
*/
class MDbStreamFilter
	{
public:
	virtual MStreamBuf* FilterL(MStreamBuf* aHost,TUint32 aInit,RDbStoreReadStream::TType aType) =0;
	virtual MStreamBuf* FilterL(MStreamBuf* aHost,TUint32 aInit,RDbStoreWriteStream::TType aType) =0;
	};


/**
@internalComponent
*/
NONSHARABLE_CLASS(CDbStoreCompression) : public CBase, public MDbStreamFilter
	{
public:
	/**
	@internalComponent
	*/
	class TEncoding
		{
	public:
		enum {ELiterals=256,ELengths=28,ESpecials=1,EDistances=44};
		enum {ELitLens=ELiterals+ELengths+ESpecials};
		enum {EEos=ELiterals+ELengths};
	public:
		TUint32 iLitLen[ELitLens];
		TUint32 iDistance[EDistances];
		};
public:
	static CDbStoreCompression* NewL();
//
	void EncodeL();
	inline void Inflate();
	void ExternalizeL(RWriteStream& aStream) const;
	void InternalizeL(RReadStream& aStream);
// for MDbStreamFilter
	MStreamBuf* FilterL(MStreamBuf* aHost,TUint32 aInit,RDbStoreReadStream::TType aType);
	MStreamBuf* FilterL(MStreamBuf* aHost,TUint32 aInit,RDbStoreWriteStream::TType aType);
private:
	inline CDbStoreCompression();
private:
	enum TState {EAnalysis,EEncoding,EDecoding,EInflating};
private:
	TState iState;
	TEncoding iEncoding[3];
	};

#include "US_STD.INL"
