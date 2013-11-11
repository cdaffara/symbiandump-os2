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
#include "U32STD_DBMS.H"

// Class CDbStoreDatabase::CCompactor

NONSHARABLE_CLASS(CDbStoreDatabase::CCompactor) : public CDbStoreDatabase::CStepper
	{
public:
	static CCompactor* NewL(CDbDatabase::TUtility aType, CStreamStore& aStore,
                            TInt& aReclaim, TInt& aStep);
	~CCompactor();
private:
	inline CCompactor(TInt& aReclaim);
// from CStepper
	TInt StepL(TInt aStep);
private:
	RStoreReclaim iReclaimer;
	TInt& iReclaim;
	};

inline CDbStoreDatabase::CCompactor::CCompactor(TInt& aReclaim)
	:iReclaim(aReclaim)
	{
	}

CDbStoreDatabase::CCompactor* CDbStoreDatabase::CCompactor::NewL(CDbDatabase::TUtility aType,
																 CStreamStore& aStore,
																 TInt& aReclaim,TInt& aStep)
	{
	CCompactor* self=new(ELeave) CCompactor(aReclaim);
	CleanupStack::PushL(self);
	if (aType==CDbDatabase::ECompact)
		self->iReclaimer.CompactL(aStore,aStep);
	else
		self->iReclaimer.OpenL(aStore,aStep);
	CleanupStack::Pop();
	return self;
	}

CDbStoreDatabase::CCompactor::~CCompactor()
	{
	iReclaimer.Close();
	}

//
// Single step the compactor
// We cannot deal with the "in use" scenario as we could end up locking out forever
// that has to be left to clients using the RDbIncremental interface
//
TInt CDbStoreDatabase::CCompactor::StepL(TInt aStep)
	{
	iReclaimer.NextL(aStep);
	if (aStep==0)
		{
		iReclaim=iReclaimer.Available();
		iReclaimer.Close();
		}
	return aStep;
	}


// Class CDbStoreDatabase

EXPORT_C CDbStoreDatabase::CDbStoreDatabase()
	:iReclaim(KErrGeneral)
	{
	}

//
// Create a StoreDatabase object. This type shares the store
//
CDbStoreDatabase* CDbStoreDatabase::NewLC(CStreamStore* aStore)
	{
	__ASSERT(aStore);
	CDbStoreDatabase* self=new(ELeave) CDbStoreDatabase;
	CleanupStack::PushL(self);
	self->iStore=aStore;
	self->iSharedStore=1;
	return self;
	}


//SYMBIAN_REMOVE_TRIVIAL_ENCRYPTION version of the method.
CDbDatabase* CDbStoreDatabase::CreateL(CStreamStore* aStore,TStreamId& aStreamId)
	{
	CDbStoreDatabase* self=NewLC(aStore);
	aStreamId=self->ConstructL();
	CDbDatabase* db=self->InterfaceL();
	CleanupStack::Pop();			// self
	return db;
	}
	
// SYMBIAN_REMOVE_TRIVIAL_ENCRYPTION version of the method.
// Phase 2 construction for creating a new database
// Initialise a new database object, creating the required persistent structure
//
EXPORT_C TStreamId CDbStoreDatabase::ConstructL()
	{
	__ASSERT(iStore);	// this must have been provided by now
	iVersion=TUint8(EDbStoreVersion2);
	InitPagePoolL();
	iPagePool->Create(Store());
	iTokenId=Store().ExtendL();
	ReplaceTokenL(0);
	iSchemaId=Store().ExtendL();
	Schema().Loaded();
	ReplaceSchemaL();
	return iSchemaId;
	}
	
// SYMBIAN_REMOVE_TRIVIAL_ENCRYPTION version of the method.
// Open phase #2: Authenticate the client
//
EXPORT_C void CDbStoreDatabase::AuthenticateL()
	{
	if (!iPagePool)
		{
		// first client to open the database, so complete initialisation now
		InitPagePoolL();
		SchemaL();
		}
	}

// SYMBIAN_REMOVE_TRIVIAL_ENCRYPTION version of the method.
void CDbStoreDatabase::InitPagePoolL()
	{
	iPagePool = new(ELeave) RStorePagePool;
	}
	

CDbSource* CDbStoreDatabase::OpenL(CStreamStore* aStore,TStreamId aStreamId)
	{
	CDbStoreDatabase* self=NewLC(aStore);
	self->RestoreL(aStreamId);
	CDbSource* src=self->SourceL();
	CleanupStack::Pop();			// self
	return src;
	}

//
// Phase 2 construction for opening a database
// Client must still authenticate before it can be used
//
EXPORT_C void CDbStoreDatabase::RestoreL(TStreamId aStreamId)
	{
	__ASSERT(iStore);	// this must have been provided by now
	iSchemaId=aStreamId;
// read the databse header for encryption information
	RStoreReadStream strm;
	strm.OpenLC(Store(),aStreamId);
	ReadHeaderL(strm);
	CleanupStack::PopAndDestroy();	// strm
	}

//
// Load the root stream header (up to the security key)
//
void CDbStoreDatabase::ReadHeaderL(RReadStream& aStream)
	{
	TUid uid;
	aStream>>uid;
	if (uid!=KDbmsStoreDatabase)
		__LEAVE(KErrArgument);
	aStream>>iVersion;
	switch (iVersion)
		{
	case EDbStoreCompressed:
		aStream>>CompressionL();
		break;
	case EDbStoreVersion2:
		break;
	default:
		__LEAVE(KErrNotSupported);
		break;
		}
	}

CDbStoreCompression& CDbStoreDatabase::CompressionL()
	{
	CDbStoreCompression* c=iCompression;
	if (!c)
		iFilter=iCompression=c=CDbStoreCompression::NewL();
	return *c;
	}

EXPORT_C CDbStoreDatabase::~CDbStoreDatabase()
	{
	if (iPageCache)
		{
		iPagePool->Release();
		delete iPageCache;
		}
	delete iClusterCache;
	delete iPagePool;
	delete iCompression;
	if (!iSharedStore)
		delete iStore;
	}

//
// Validate the column set first
//
EXPORT_C CDbTableDef* CDbStoreDatabase::CreateTableL(const TDesC& aName,const CDbColSet& aColSet,const CDbKey* aPrimaryKey)
	{
	if (aPrimaryKey)
		__LEAVE(KErrNotSupported);	// Store database does not support primary keys
	CDbStoreDef* def=CDbStoreDef::NewLC(aName,aColSet);
	def->SetTokenId(CDbStoreRecords::CreateL(ClusterCacheL()));
	CleanupStack::Pop();
	return def;
	}

EXPORT_C CDbTableIndexDef* CDbStoreDatabase::CreateIndexL(const CDbTableDef& aTable,const TDesC& aName,const CDbKey& aKey)
	{
	CDbStoreIndexDef* def=CDbStoreIndexDef::NewLC(aName,aKey,aTable.Columns());
	def->SetTokenId(CDbStoreIndex::CreateL(*this,*def));
	CleanupStack::Pop();			// IndexDef
	return def;
	}

//
// Destroy the entire database...
//
EXPORT_C void CDbStoreDatabase::DestroyL()
	{
	iPagePool->Discard();
	iPagePool->ReclaimAllL();	// reclaim all page pool space
	iStore->DeleteL(iSchemaId);
	iStore->DeleteL(iTokenId);
	iStore->CommitL();
	}

EXPORT_C CDbTable* CDbStoreDatabase::TableL(const CDbTableDef& aDef)
	{
	return new(ELeave) CDbStoreTable(*this,aDef);
	}

//
// load the schema for the database
//
EXPORT_C void CDbStoreDatabase::LoadSchemaL()
	{
	RDbStoreReadStream strm(*this);
	strm.OpenLC(Store(),iSchemaId);
	ReadHeaderL(strm);
	strm.FilterL(strm.EMixed,iSchemaId.Value());
	strm>>iTokenId;
	RDbTableSchema& schema=Schema();
	TCardinality tables;
	strm>>tables;
	for (TInt ii=tables;ii>0;--ii)
		schema.Add(CDbStoreDef::NewL(strm));
	CleanupStack::PopAndDestroy();
	strm.OpenLC(Store(),iTokenId);
	strm>>iFlags>>iPoolToken;
	iPagePool->Open(Store(),iPoolToken);
	CleanupStack::PopAndDestroy();
	}

//
// Re-write the schema stream
//
void CDbStoreDatabase::ReplaceSchemaL()
	{
	RDbStoreWriteStream out(*this);
	out.ReplaceLC(Store(),iSchemaId);
	out<<KDbmsStoreDatabase<<iVersion;
	switch (iVersion)
		{
	case EDbStoreCompressed:
		__ASSERT(iCompression);
		out<<*iCompression;
		break;
	case EDbStoreVersion2:
		break;
	default:
		__ASSERT(0);
		}
	out.FilterL(out.EMixed,iSchemaId.Value());
	out<<iTokenId;
	TSglQueIterC<CDbStoreDef> iter(Schema());
	TInt count=0;
	while (iter++)
		++count;
	out<<TCardinality(count);
	iter.SetToFirst();
	for (const CDbStoreDef* def;(def=iter++)!=0;)
		out<<*def;
	out.CommitL();
	CleanupStack::PopAndDestroy();
	}

//
// Re-write the token stream, removing the mark
//
void CDbStoreDatabase::ReplaceTokenL(TUint aFlags)
	{
	RStoreWriteStream out;
	out.ReplaceLC(Store(),iTokenId);
	out<<TUint8(aFlags&EDamaged)<<iPagePool->Token();
	out.CommitL();
	CleanupStack::PopAndDestroy();
	}

//
// Return some database property
//
EXPORT_C TInt CDbStoreDatabase::Property(CDbDatabase::TProperty aProperty)
	{
	switch (aProperty)
		{
	case CDbDatabase::EIsDamaged:
		return iFlags&EDamaged ? 1 : 0;
	case CDbDatabase::ECompactable:
		return 1;
	default:
		return CDbTableDatabase::Property(aProperty);
		}
	}

//
// mark the database as dirty
//
void CDbStoreDatabase::MarkL()
	{
	if (!(iFlags&EModified))
		{
		RStoreWriteStream out;
		out.OpenLC(Store(),iTokenId);
		out.WriteUint8L(EDamaged);		// mark as dirty
		iPoolToken.Touch();
		out<<iPoolToken;
		out.CommitL();
		CleanupStack::PopAndDestroy();
		iFlags|=EModified;
		}
	}

//
// Reset all cache buffers
//
EXPORT_C void CDbStoreDatabase::Idle()
	{
	if (iPageCache)
		{
		iPagePool->Purge();
		delete iPageCache;
		iPageCache=NULL;
		}
	if (iClusterCache)
		{
		delete iClusterCache;
		iClusterCache=NULL;
		}
	}

//
// Commit the store, and when all is well, clear the token
//
void CDbStoreDatabase::SynchStoreL(TDbLockType aLock)
	{
	if (iPageCache)
		iPagePool->FlushL();
	TUint newflags=iFlags&~EModified;
	if (aLock==EDbRecoveryLock)
		newflags&=~EDamaged;
	if (aLock==EDbRecoveryLock || iFlags&EModified)
		ReplaceTokenL(newflags);
	if (aLock>=EDbWriteLock || iSharedStore)
		{
		iStore->CommitL();
		iFlags=TUint8(newflags);
		iPoolToken=iPagePool->Token();
		}
	}

//
// An index has been successfully recovered, commit it
//
void CDbStoreDatabase::IndexRecoveredL()
	{
	SynchStoreL(EDbSchemaLock);
	}

//
// Ensure all data is in the store
//
EXPORT_C void CDbStoreDatabase::SynchL(TDbLockType aLock)
	{
	if (aLock==EDbSchemaLock)
		ReplaceSchemaL();
	if (iClusterCache)
		iClusterCache->FlushL();
	SynchStoreL(aLock);
	}

//
// Unwind the store, throw out changes, etc
//
EXPORT_C void CDbStoreDatabase::Revert(TDbLockType aLock)
	{
	if (aLock>=EDbWriteLock)
		{
		if (iClusterCache)
			iClusterCache->Discard();
		if (iPageCache)
			iPagePool->Purge();
		if (iFlags&EModified)
			iFlags|=EDamaged;
		iPagePool->Open(Store(),iPoolToken);	// reset the page pool
		}
	else if (!iSharedStore)		// don't touch the store if not shared
		return;
	iStore->Revert();
	}

//
// Ensure we have a cluster cache and return it
//
CClusterCache& CDbStoreDatabase::ClusterCacheL()
	{
	CClusterCache* cache=iClusterCache;
	if (!cache)
		iClusterCache=cache=CClusterCache::NewL(*this);
	return *cache;
	}

//
// Ensure we have a page cache and return the pool
//
MPagePool& CDbStoreDatabase::PagePoolL()
	{
	if (!iPageCache)
		{
		iPageCache=CPageCache::NewL(EPageCachePages);
		iPagePool->Set(*iPageCache);
		}
	return *iPagePool;
	}

//
// Create an incremental object that compacts the store
//
EXPORT_C CDbTableDatabase::CStepper* CDbStoreDatabase::UtilityL(CDbDatabase::TUtility aType,TInt& aStep)
	{
	switch (aType)
		{
	case CDbDatabase::EStats:
	case CDbDatabase::ECompact:
		return CCompactor::NewL(aType,Store(),iReclaim,aStep);
	case CDbDatabase::ERecover:
		return RecoverL(aStep);
	default:
		return CDbTableDatabase::UtilityL(aType,aStep);
		}
	}

//
// Create an incremental object to destroy a table
//
EXPORT_C CDbTableDatabase::CStepper* CDbStoreDatabase::RecordDiscarderL(const CDbTableDef& aTable,TInt& aStep)
	{
	CDbStoreTable::CDiscarder* discarder=new(ELeave) CDbStoreTable::CDiscarder;
	CleanupStack::PushL(discarder);
	aStep=discarder->OpenL((CDbStoreTable*)TableL(aTable));
	CleanupStack::Pop();
	return discarder;
	}

//
// Create an incremental object to destroy an index
//
EXPORT_C CDbTableDatabase::CStepper* CDbStoreDatabase::IndexDiscarderL(const CDbTableDef& aTable,const CDbTableIndexDef& anIndex,TInt& aStep)
	{
	CDbStoreIndex::CDiscarder* discarder=new(ELeave) CDbStoreIndex::CDiscarder;
	CleanupStack::PushL(discarder);
	CDbStoreIndex* index=CDbStoreIndex::NewL(*this,(const CDbStoreIndexDef&)anIndex,aTable);
	aStep=discarder->Open(index);
	index->OpenL();
	CleanupStack::Pop();
	return discarder;
	}

//
// Provide a stepper to alter the table data
// if no data to alter, return 0
//
EXPORT_C CDbTableDatabase::CStepper* CDbStoreDatabase::TableAlterL(CDbTableDef& aTable,const HDbColumnSet& aNewSet,TInt& aStep)
	{
	CDbStoreDef& def=STATIC_CAST(CDbStoreDef&,aTable);
//
	aStep=CDbStoreRecords::CardinalityL(Store(),def);
	if (!aStep)
		return NULL; // no data to modify

// check that all added columns are nullable
	HDbColumnSet::TIteratorC col=aNewSet.Begin();
	HDbColumnSet::TIteratorC end=aNewSet.End();
	do
		{
		if (col->iFlags&TDbColumnDef::EAdded && col->iAttributes&TDbCol::ENotNull)
			__LEAVE(KErrArgument);		// added column is not nullable
		} while (++col<end);
//
// check to see if anything is being dropped or changed type
	col=aTable.Columns().Begin();
	end=aTable.Columns().End();
	while (!(col->iFlags&(TDbColumnDef::EDropped|TDbColumnDef::EChangedType|TDbColumnDef::EChangedLen)))
		{
		if (++col==end)
			{	// no changes which affect layout, so no work required
			aStep=0;
			return NULL;
			}
		}
// work required
	CDbStoreTable::CAlter* alter=new(ELeave) CDbStoreTable::CAlter;
	CleanupStack::PushL(alter);
	alter->OpenL((CDbStoreTable*)TableL(def),aNewSet);
	CleanupStack::Pop();
	return alter;
	}

EXPORT_C void CDbStoreDatabase::Reserved_1()
	{
	}

EXPORT_C void CDbStoreDatabase::Reserved_2()
	{
	}

