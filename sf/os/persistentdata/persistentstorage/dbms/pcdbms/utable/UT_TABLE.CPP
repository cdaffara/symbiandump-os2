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

#include "UT_STD.H"
#include "U32STD_DBMS.H"

#define UNUSED_VAR(a) a = a

const TUint KTableExpiry=0x100000;	// ~1.0s

// Class Blob cleanup

NONSHARABLE_CLASS(CDbBlobCleanup) : public CArrayFixFlat<TDbBlobId>
	{
public:
	static CDbBlobCleanup* NewLC(CDbBlobSpace& aBlobSpace);
	~CDbBlobCleanup();
private:
	inline CDbBlobCleanup(CDbBlobSpace& aBlobSpace);
private:
	CDbBlobSpace& iBlobSpace;
	};

inline CDbBlobCleanup::CDbBlobCleanup(CDbBlobSpace& aBlobSpace)
	: CArrayFixFlat<TDbBlobId>(8),iBlobSpace(aBlobSpace)
	{}

CDbBlobCleanup* CDbBlobCleanup::NewLC(CDbBlobSpace& aBlobSpace)
	{
	CDbBlobCleanup* self=new(ELeave) CDbBlobCleanup(aBlobSpace);
	CleanupStack::PushL(self);
	return self;
	}

CDbBlobCleanup::~CDbBlobCleanup()
	{
	TInt count=Count();
	if (count)
		{
		const TDbBlobId* blob=&(*this)[0];
		const TDbBlobId* const end=blob+count;
		for (;blob<end;++blob)
			iBlobSpace.Delete(*blob);
		}
	}

// Class CDbTable


EXPORT_C CDbTable::CDbTable(CDbTableDatabase& aDatabase,const CDbTableDef& aDef)
	: iDatabase(&aDatabase),iDef(&aDef)
	{
	aDatabase.Open();
	aDatabase.AddTable(*this);		// we reference database
	}

EXPORT_C CDbTable::~CDbTable()
//
// Destroy components
//
	{
	__ASSERT(!InUse());		// cannot be directly deleted
	if (IsActive())
		Disconnect();
	}

void CDbTable::Disconnect()
//
// Disconnect the table from the database collection.
//
	{
	__ASSERT(IsActive());
	Database().RemoveTable(*this);
	TRAPD(errCode, ApplyToComponentsL(CDbRecordBase::DoDelete));
    UNUSED_VAR(errCode);
	}

void CDbTable::Open()
	{
	__ASSERT(IsActive());
	TInt r=++iRef;
	if (r<=0)
		{	// were idle or cached
		if (r<0)
			{
			Cache().Release(*this);	// was cached
			iRef=0;
			}
		Database().Open();
		}
	}

void CDbTable::Close()
//
// We may destroy this object when the last reference goes away
//
	{
	__ASSERT(InUse());
	if (--iRef<0)
		{
		if (!IsActive())
			delete this;		// disconnected table
		else
			{
			CDbTableDatabase& db=Database();
			if (!db.Transaction().IsLocked())
				Idle();			// no transaction, idle now
			db.Close();			// this must be done last to avoid early self destruction
			}
		}
	}

void CDbTable::Idle()
//
// Called when idle, change to cached state
//
	{
	__ASSERT(IsIdle());
	__ASSERT(IsActive());
//
	iRef=ECached;
	Cache().Hold(this,KTableExpiry);	// may delete this
	}

void CDbTable::FlushL()
//
// Ensure all records objects are flushed
//
	{
	__ASSERT(IsActive());
	if (iRef!=ECached)
		ApplyToComponentsL(CDbRecordBase::DoFlushL);
	}

void CDbTable::Abandon()
//
// Discard all components
//
	{
	__ASSERT(IsActive());
	TRAPD(errCode, ApplyToComponentsL(CDbRecordBase::DoAbandon));
    UNUSED_VAR(errCode);
	iIndexesEnd=NULL;	// flags indexes as abandoned
	++iGeneration;
	}

void CDbTable::Release()
//
// Release the table and all its cursors as DDL is about to begin
//
	{
	__ASSERT(IsActive());
	switch (iRef)
		{
	case ECached:
		Cache().Release(*this);
		// fall throught to Idle
	case EIdle:
		delete this;
		break;
	default:
		__ASSERT(InUse());
		Database().Close();
		Disconnect();
		iDatabase=0;		// this marks us as released
		iDef=0;
		break;
		}
	}

void CDbTable::ApplyToBlobsL(RDbRow& aRow,TBlobFuncL aFuncL,CDbBlobCleanup* aCleanup)
	{
	__ASSERT(Def().Columns().HasLongColumns());
	CDbBlobSpace* blobs=BlobsL();
	__ASSERT(blobs);
	TDbColNo col=1;
	HDbColumnSet::TIteratorC iter=Def().Columns().Begin();
	const HDbColumnSet::TIteratorC end=Def().Columns().End();
	do
		{
		if (!TDbCol::IsLong(iter->Type()))
			continue;
		const TDbColumnC column(aRow,col);
		if (column.IsNull())
			continue;
		aFuncL(*blobs,CONST_CAST(TDbBlob&,column.Blob()),iter->Type(),aCleanup);
		} while (++col,++iter<end);
	}

LOCAL_C void DuplicateBlobL(CDbBlobSpace& aBlobStore,TDbBlob& aBlob,TDbColType aType,CDbBlobCleanup* aCleanup)
	{
	__ASSERT(aCleanup);
	if (aBlob.IsInline())
		return;
// need to duplicate blob
	RReadStream old(aBlobStore.ReadLC(aBlob.Id(),aType));
	TDbBlobId& newId=aCleanup->ExtendL();
	newId=KDbNullBlobId;
	RWriteStream dup(aBlobStore.CreateL(newId,aType));
	dup.PushL();
	dup.WriteL(old,aBlob.Size());
	dup.CommitL();
	CleanupStack::PopAndDestroy(2);		// old and dup streams
	aBlob.SetId(newId);		// row is writable
	}

void CDbTable::DuplicateBlobsL(RDbRow& aRow)
//
// duplicate any blobs
//
	{
	if (!Def().Columns().HasLongColumns())
		return;

	CDbBlobCleanup* cleaner=CDbBlobCleanup::NewLC(*BlobsL());
	ApplyToBlobsL(aRow,DuplicateBlobL,cleaner);
	cleaner->Reset();
	CleanupStack::PopAndDestroy();
	}

TBool CDbTable::ExistsL(TDbRecordId aRecordId)
//
// Check that aRecordId is good for this table
//
	{
	__ASSERT(IsActive() && InUse());
	return RecordsL().ExistsL(aRecordId);
	}

void CDbTable::NewRowL(RDbRow& aRow)
//
// Initialise any auto-increment columns in the row
//
	{
	const HDbColumnSet& columns=Def().Columns();
	if (!columns.HasAutoIncrement())
		return;

	TUint value=RecordsL().AutoIncrementL();
	TDbColNo col=1;
	HDbColumnSet::TIteratorC iter=columns.Begin();
	const HDbColumnSet::TIteratorC end=columns.End();
	do
		{
		if (iter->iAttributes&TDbCol::EAutoIncrement)
			{
			// auto-increment only for integral types <=32 bits wide
			__ASSERT(iter->iType<=EDbColUint32);
			TDbColumn column(aRow,col);
			column.SetL(TUint32(value));
			}
		} while (++col,++iter<end);
	}

void CDbTable::ValidateL(const RDbRow& aRow)
//
// Ensure that the column data conforms to type size/flags etc
//
	{
	HDbColumnSet::TIteratorC iter=Def().Columns().Begin();
	const HDbColumnSet::TIteratorC end=Def().Columns().End();
	const TDbCell* const last=aRow.Last();
	for (const TDbCell* column=aRow.First();column<last;++iter,column=column->Next())
		{
		TInt size=column->Length();
		if (size==0)
			{	// check for Null
			if (iter->iAttributes&TDbCol::ENotNull)
				{
				__LEAVE(KErrNotFound);
				return;
				}
			continue;
			}
		const TUint32* data=(const TUint32*)column->Data();
		switch (iter->iType)
			{
		case EDbColBit:
			if (*data>1)
				__LEAVE(KErrOverflow);
			break;
		case EDbColInt8:
			{
			TInt val=*data;
			if (TInt8(val)!=val)
				__LEAVE(KErrOverflow);
			}
			break;
		case EDbColInt16:
			{
			TInt val=*data;
			if (TInt16(val)!=val)
				__LEAVE(KErrOverflow);
			}
			break;
		case EDbColUint8:
			{
			TUint val=*data;
			if (TUint8(val)!=val)
				__LEAVE(KErrOverflow);
			}
			break;
		case EDbColUint16:
			{
			TUint val=*data;
			if (TUint16(val)!=val)
				__LEAVE(KErrOverflow);
			}
			break;
		case EDbColText16:
			size>>=1;
		case EDbColBinary:
		case EDbColText8:
			if (iter->iMaxLength==KDbUndefinedLength)
				break;
			if (size>iter->iMaxLength)
				__LEAVE(KErrOverflow);
			break;
		case EDbColLongBinary:
		case EDbColLongText8:
		case EDbColLongText16:
			if (iter->iMaxLength==KDbUndefinedLength)
				break;
			size=((TDbBlob*)data)->Size();
			if (size==KDbUndefinedLength)
				break;
			if (iter->iType==EDbColText16)
				size>>=1;
			if (size>iter->iMaxLength)
				__LEAVE(KErrOverflow);
			break;
		default:
			break;
			}
		}
	for (;iter<end;++iter)
		{	// check for Null
		if (iter->iAttributes&TDbCol::ENotNull)
			{
			__LEAVE(KErrNotFound);
			return;
			}
		}
	}

void CDbTable::ReadRowL(RDbRow& aRow,TDbRecordId aRecordId)
//
// Read a record from the table
//
	{
	CopyToRowL(aRow,RecordsL().ReadL(aRecordId));
	}

void CDbTable::PrepareAppendL(const RDbTableRow& aRow)
//
// Validate a new record for appending
//
	{
	EnsureIndexesL();
	ValidateL(aRow);
	CDbRecordIndex** end=iIndexesEnd;
	for (CDbRecordIndex** pix=iIndexes;pix<end;++pix)
		{
		CDbRecordIndex& ix=**pix;
		if (ix.IsBroken())
			continue;
		if (ix.FindL(KDbNullRecordId,aRow)==CDbRecordIndex::EKeyMatch)
			__LEAVE(KErrAlreadyExists);	// duplicate found
		}
	}

TDbRecordId CDbTable::AppendRowL(const RDbTableRow& aRow)
//
// Validate and add a new record to the table and any open indexes
//
	{
	CDbRecordSpace& records=RecordsL();
	CopyFromRow(records.NewL(RecordLength(aRow)),aRow);
	TDbRecordId id=records.AppendL();
	CDbRecordIndex** end=iIndexesEnd;
	for (CDbRecordIndex** pix=iIndexes;pix<end;++pix)
		{
		CDbRecordIndex& ix=**pix;
		if (ix.IsBroken())
			continue;
		__DEBUG(TInt dbgchk=) ix.InsertL(id,aRow);
		__ASSERT(dbgchk);
		}
	++iGeneration;
	return id;
	}

void CDbTable::PrepareReplaceL(const RDbTableRow& aRow,TDbRecordId aRecordId)
//
// Validate a record for replacement
//
	{
	EnsureIndexesL();
	ValidateL(aRow);
	TUint32 update=0;
	CDbRecordIndex** end=iIndexes;
	for (CDbRecordIndex** pix=iIndexesEnd;--pix>=end;)
		{
		update<<=1;
		CDbRecordIndex& ix=**pix;
		if (ix.IsBroken())
			continue;
		switch (ix.FindL(aRecordId,aRow))
			{
		case CDbRecordIndex::ENoMatch:		// key has changed in index
			update|=1;
			break;
		case CDbRecordIndex::EKeyMatch:	// duplicate found
			__LEAVE(KErrAlreadyExists);
		case CDbRecordIndex::EEntryMatch:	// no change in index
			break;
			}
		}
	iUpdateMap=update;
	}

void CDbTable::DoReplaceRowL(const RDbRow& aRow,TDbRecordId aRecordId)
	{
	CopyFromRow(RecordsL().ReplaceL(aRecordId,RecordLength(aRow)),aRow);
	++iGeneration;
	}

void CDbTable::ReplaceRowL(RDbTableRow& aRow,TDbRecordId aRecordId)
//
// Replace a record in the table
//
	{
	if (Def().Columns().HasLongColumns())
		CheckInliningL(aRow);
	TUint32 update=iUpdateMap;
	if (update==0)
		{
		DoReplaceRowL(aRow,aRecordId);
		return;
		}
	RDbTableRow oldRow;		// temporary row buffer for old row values
	oldRow.Open(this);
	oldRow.PushL();			// cleanup buffer if there is trouble
	ReadRowL(oldRow,aRecordId);
	DoReplaceRowL(aRow,aRecordId);
	for (CDbRecordIndex** pix=iIndexes;update;++pix,update>>=1)
		{
		if (update&1)
			{
			CDbRecordIndex& index=**pix;
			index.DeleteL(aRecordId,oldRow);
			__DEBUG(TInt dbgchk=) index.InsertL(aRecordId,aRow);
			__ASSERT(dbgchk);
			}
		}
	CleanupStack::PopAndDestroy();	// temp row buffer
	}

LOCAL_C void CheckInlineL(CDbBlobSpace& aBlobStore,TDbBlob& aBlob,TDbColType aType,CDbBlobCleanup*)
	{
	if (!aBlob.IsInline())
		return;
	if (aBlob.Size()>aBlobStore.InlineLimit())
		aBlob.SetId(aBlobStore.CreateL(aType,aBlob.Data(),aBlob.Size()));
	}

void CDbTable::CheckInliningL(RDbRow& aRow)
//
// Ensure that all Blobs are within the current inline limit
//
	{
	ApplyToBlobsL(aRow,CheckInlineL);
	}

LOCAL_C void DiscardBlobL(CDbBlobSpace& aBlobStore,TDbBlob& aBlob,TDbColType,CDbBlobCleanup*)
	{
	if (!aBlob.IsInline())
		aBlobStore.DeleteL(aBlob.Id());
	}

EXPORT_C void CDbTable::DiscardBlobsL(RDbRow& aRow)
//
// Default implemtation xlates the record and then walks the row buffer
//
	{
	ApplyToBlobsL(aRow,DiscardBlobL);
	}

void CDbTable::DeleteRowL(RDbTableRow& aRow,TDbRecordId aRecordId)
//
// Delete the record from the file and unlock it.
//
	{
	EnsureIndexesL();

	if (Def().Columns().HasLongColumns())
		{
		// Read data from the stream but do not delete the stream yet.
		aRow.ReadL(aRecordId);
		}

	CDbRecordIndex** end=iIndexes;
	CDbRecordIndex** pix=iIndexesEnd;
	if (pix!=end)
		aRow.ReadL(aRecordId);
	RecordsL().EraseL(aRecordId);
	while (--pix>=end)
		{
		CDbRecordIndex& ix=**pix;
		if (!ix.IsBroken())
			ix.DeleteL(aRecordId,aRow);
		}

	if (Def().Columns().HasLongColumns())
		{
		// Now delete the stream.
		DiscardBlobsL(aRow);
		}

	++iGeneration;
	}

EXPORT_C CDbRecordSpace& CDbTable::RecordsL()
	{
	__ASSERT(IsActive() && InUse());
	CDbRecordSpace* rec=iRecords;
	if (rec==NULL)
		iRecords=rec=RecordSpaceL();
	if (rec->OpenL())
		__LEAVE(KErrCorrupt);
	return *rec;
	}

EXPORT_C CDbBlobSpace* CDbTable::BlobsL()
	{
	__ASSERT(IsActive() && InUse());
	CDbBlobSpace* blob=iBlobs;
	if (blob==NULL)
		iBlobs=blob=BlobSpaceL();
	if (blob->OpenL())
		__LEAVE(KErrCorrupt);
	return blob;
	}

EXPORT_C CDbRecordIndex& CDbTable::IndexL(const CDbTableIndexDef& aIndex)
//
// Load the index associated with the index definition and ensure it is operational
//
	{
	__ASSERT(IsActive() && InUse());
	// find the matching slot in the indexes array
	CDbRecordIndex** slot=&iIndexes[0];
	for (TSglQueIterC<CDbTableIndexDef> iter(Def().Indexes().AsQue());iter++!=&aIndex;)
		++slot;
	__ASSERT(iIndexesEnd==NULL||(slot>=iIndexes&&slot<iIndexesEnd));
	// load (if required) and open the index
	CDbRecordIndex* index=*slot;
	if (index==0)
		*slot=index=RecordIndexL(aIndex);
	if (index->OpenL())
		__LEAVE(KErrCorrupt);
	return *index;
	}

void CDbTable::EnsureIndexesL()
//
// Ensure that all indexes are open
//
	{
	__ASSERT(IsActive() && InUse());
	if (iIndexesEnd==NULL)
		{
		CDbRecordIndex** pp=iIndexes;
		TSglQueIterC<CDbTableIndexDef> iter(Def().Indexes().AsQue());
		for (const CDbTableIndexDef* xDef;(xDef=iter++)!=NULL;++pp)
			{
			CDbRecordIndex* ix=*pp;
			if (ix==NULL)
				*pp=ix=RecordIndexL(*xDef);
			ix->OpenL();	// ignore broken-ness
			}
		iIndexesEnd=pp;
		}
	}

CDbRecordIter* CDbTable::IteratorL()
	{
	return RecordsL().IteratorL();
	}

CDbRecordIter* CDbTable::IteratorL(const CDbTableIndexDef& aIndex,TUint aInclusion,const TDbLookupKey* aLowerBound,const TDbLookupKey* aUpperBound)
//
// create an interator for the index parameter
//
	{
	return IndexL(aIndex).IteratorL(aInclusion,aLowerBound,aUpperBound);
	}

EXPORT_C TInt CDbTable::IndexSpanL(const CDbTableIndexDef&,TUint,const TDbLookupKey*,const TDbLookupKey*)
//
// Default implementation: no statistics are available
//
	{
	return EUnavailableSpan;
	}

CDbRecordIter* CDbTable::IteratorL(const TDesC& aIndex)
//
// create an interator for the index named
//
	{
	return IteratorL(Def().Indexes().FindL(aIndex));
	}

void CDbTable::ApplyToComponentsL(void (*anOperationL)(CDbRecordBase*))
//
// Invoke anOperation on all components of the table
//
	{
	if (iRecords)
		anOperationL(iRecords);
	if (iBlobs)
		anOperationL(iBlobs);
	CDbRecordIndex** const ixs=iIndexes;
	CDbRecordIndex** pix=iIndexesEnd;
	if (pix==NULL)
		pix=&iIndexes[KDbTableMaxIndexes];
	while (--pix>=ixs)
		if (*pix)
			anOperationL(*pix);
	}

// Class CDbtable::TValid

// this class is used by the cursor to check that it is still operational

CDbTable::TValid::TValid(CDbTable& aTable)
	:iTable(aTable)
	{
	__ASSERT(aTable.IsActive());
	iRollback.Construct(aTable.Database().Transaction().RollbackGeneration());
	}

TBool CDbTable::TValid::Reset()
	{
	TBool b=Table().IsActive();
	if (b)
		iRollback.Mark();
	return b;
	}

void CDbTable::TValid::CheckL() const
	{
	CDbTableDatabase* d=Table().iDatabase;
	if (!d)
		__LEAVE(KErrDisconnected);
	else
		d->Transaction().ReadyL();
	if (iRollback.Changed())
		__LEAVE(KErrNotReady);
	}
