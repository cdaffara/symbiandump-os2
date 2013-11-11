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

// Class CDbTableCursor::HColumns

class CDbTableCursor::HColumns
	{
public:
	static HColumns* NewL(const CDbDataSource* aSource);
	inline TInt Count() const
		{return iCount;}
	inline TDbColType Type(TDbColNo aCol) const
		{__DEBUG(Check(aCol));return TDbColType(iType[aCol-1]);}
	void Check(TDbColNo aCol) const;
private:
	TInt iCount;
	TUint8 iType[1];
	};

CDbTableCursor::HColumns* CDbTableCursor::HColumns::NewL(const CDbDataSource* aSource)
	{
	TInt count=aSource->ColumnCount();
	HColumns* self=(HColumns*)User::AllocL(_FOFF(HColumns,iType[count]));
	self->iCount=count;
	TUint8* pp=&self->iType[0];
	for (TDbColNo ii=1;ii<=count;++ii,++pp)
		*pp=aSource->ColumnDef(ii).iType;
	return self;
	}

void CDbTableCursor::HColumns::Check(TDbColNo aColNo) const
	{
	__ASSERT_ALWAYS(TUint(aColNo-1)<TUint(iCount),Panic(EDbInvalidColumn));
	}

// Class CDbTableCursor::CConstraint

NONSHARABLE_CLASS(CDbTableCursor::CConstraint) : public CDbRowConstraint
	{
public:
	CConstraint(CDbTableCursor& aCursor,CSqlSearchCondition* aSearchCondition,TDbTextComparison aComparison);
	~CConstraint();
//
	inline TBool Check(CDbTableCursor& aCursor) const;
	inline TBool MatchL() const;
private:
	CDbTableCursor& iCursor;
	CSqlSearchCondition* iSearchCondition;
	const TTextOps& iTextOps;
	};

CDbTableCursor::CConstraint::CConstraint(CDbTableCursor& aCursor,CSqlSearchCondition* aSearchCondition,TDbTextComparison aComparison)
	: iCursor(aCursor), iSearchCondition(aSearchCondition), iTextOps(TTextOps::Ops(aComparison))
	{}

CDbTableCursor::CConstraint::~CConstraint()
	{
	delete iSearchCondition;
	}

inline TBool CDbTableCursor::CConstraint::Check(CDbTableCursor& aCursor) const
	{return &iCursor==&aCursor;}
inline TBool CDbTableCursor::CConstraint::MatchL() const
	{return iSearchCondition->EvaluateL(iTextOps);}

// Class CDbTableCursor

inline void CDbTableCursor::CheckStateL() const
	{iValid.CheckL();}
inline RDbTransaction& CDbTableCursor::Transaction()
	{__ASSERT(iValid);return iValid.Transaction();}
inline TBool CDbTableCursor::InUpdate() const
	{return iFlags&(EUpdating|EInserting);}

CDbTableCursor::CDbTableCursor(RDbAccessPlan& aPlan,RDbRowSet::TAccess aAccess)
	: iState(ERowBeginning),iValid(aPlan.Table()),iDataSource(aPlan.Adopt())
	{
	switch (aAccess)
		{
	default:
		__ASSERT(0);
	case RDbRowSet::EUpdatable:
		iFlags=EUpdatable|EReadable;
		break;
	case RDbRowSet::EReadOnly:
		iFlags=EReadable;
		break;
	case RDbRowSet::EInsertOnly:
		iFlags=EUpdatable;
		break;
		}
	}

CDbTableCursor::~CDbTableCursor()
	{
	Cancel();
	delete iDataSource;
	delete iColumns;
	}

CDbTableCursor* CDbTableCursor::NewL(RDbAccessPlan& aPlan,RDbRowSet::TAccess aAccess)
	{
	CDbTableCursor* self=new(ELeave) CDbTableCursor(aPlan,aAccess);
	CleanupStack::PushL(self);
	self->iColumns=HColumns::NewL(self->iDataSource);
	self->Reset();
	CleanupStack::Pop();
	return self;
	}

TDbColType CDbTableCursor::Type(TDbColNo aCol) const
	{
	iColumns->Check(aCol);
	return iColumns->Type(aCol);
	}

void CDbTableCursor::Reset()
//
// Reset the cursor for re-evaluation
//
	{
	AssertNotInUpdate();
	if (iValid.Reset())
		{
		iDataSource->Reset();
		iState=ERowBeginning;
		}
	}

TBool CDbTableCursor::EvaluateL()
//
// Do a unit of evaluation work
//
	{
	AssertNotInUpdate();
	CheckStateL();
	TInt work=256;
	TBool atRow=EFalse;
	TBool more=iDataSource->EvaluateL(work,iRecord,atRow);
	if (atRow)
		{	// evaluation results in a record appearing under the cursor
		switch (iState)
			{
		case ERowEnd:
		case ERowBeginning:
			iState=ERowOK;
			break;
		case ERowDeletedAtEnd:
			iState=ERowDeletedAtNext;
			break;
		default:
			break;
			}
		}
	return more?1:0;
	}

void CDbTableCursor::Evaluate(TRequestStatus& aStatus)
//
// Asynchronous evaluation: invoke synchronous version
//
	{
	TRequestStatus* pStatus=&aStatus;
	User::RequestComplete(pStatus,CDbCursor::Evaluate());
	}

TBool CDbTableCursor::Unevaluated()
//
// Report if there is evaluation to be done
//
	{
	return iValid ? iDataSource->Unevaluated() : EFalse;
	}

TInt CDbTableCursor::CountL(RDbRowSet::TAccuracy aAccuracy)
	{
	AssertNotInUpdate();
	CheckReadL();
	TInt count=iDataSource->CountL();
	return (count==KDbUndefinedCount && aAccuracy==RDbRowSet::EEnsure)
		? CDbCursor::CountL(aAccuracy)
		: count;
	}

TBool CDbTableCursor::AtBeginning()
	{
	return iState==ERowBeginning;
	}

TBool CDbTableCursor::AtEnd()
	{
	return iState==ERowEnd;
	}

TBool CDbTableCursor::AtRow()
	{
	return (iState==ERowOK||(iFlags&EInserting));
	}

TBool CDbTableCursor::GotoL(RDbRowSet::TPosition aPosition)
//
// Move the cursor in the requested direction
// return whether we are at a row or not
//
	{
	AssertNotInUpdate();
	CheckReadL();
	iFlags&=~ERead;
	switch (aPosition)
		{
	default:
		__ASSERT(0);
	case RDbRowSet::EFirst:
	case RDbRowSet::ELast:
		break;
	case RDbRowSet::ENext:
		switch (iState)
			{
		default:
			__ASSERT(0);
		case ERowInLimbo:	// in between previous and next, must evaluate
		case ERowOK:
			break;
		case ERowBeginning:		// goto first record
			aPosition=RDbRowSet::EFirst;
			break;
		case ERowEnd:
		case ERowInvalid:
			Panic(EDbInvalidRow);
			break;
		case ERowDeletedAtNext:	// already have the id
			if (iDataSource->GotoL(iRecord))
				{	// and the record is still there
				iState=ERowOK;
				return ETrue;
				}
			break;
		case ERowDeletedAtEnd:	// straight to end
			iState=ERowEnd;
			return EFalse;
			}
		break;
	case RDbRowSet::EPrevious:
		switch (iState)
			{
		default:
			__ASSERT(0);
		case ERowOK:
		case ERowDeletedAtNext:		// goto previous will do what we want
		case ERowInLimbo:	// in between previous and next, must evaluate
			break;
		case ERowEnd:				// goto last row
		case ERowDeletedAtEnd:		// previous is last row
			aPosition=RDbRowSet::ELast;
			break;
		case ERowBeginning:
		case ERowInvalid:
			Panic(EDbInvalidRow);
			break;
			}
		break;
	case RDbRowSet::EBeginning:
		iState=ERowBeginning;
		return EFalse;
	case RDbRowSet::EEnd:
		iState=ERowEnd;
		return EFalse;
		}
	iState=ERowInvalid;
	switch (iDataSource->GotoL(TDbPosition(aPosition),iRecord))
		{
	default:
		__ASSERT(0);
	case CDbDataSource::ESynchFailure:
		__LEAVE(KErrNotReady);
	case CDbDataSource::ESuccess:
		iState=ERowOK;
		return ETrue;
	case CDbDataSource::ENoRow:
		iState=TUint8(aPosition<RDbRowSet::EPrevious ? ERowEnd : ERowBeginning);
		return EFalse;
		}
	}

void CDbTableCursor::Bookmark(TDbBookmark::TMark& aMark)
//
// Create a bookmark for the current cursor state
// Can bookmark ANY position.
//
	{
	AssertNotInUpdate();
	aMark.iMark[0]=iState;
	aMark.iMark[1]=iRecord.Value();
	}

void CDbTableCursor::GotoL(const TDbBookmark::TMark& aMark)
//
// Reestablish the cursor state from a bookmark (if possible)
//
	{
	AssertNotInUpdate();
	CheckStateL();
	iState=ERowInvalid;
	iRecord=aMark.iMark[1];
	TState state=TState(aMark.iMark[0]);
	switch (state)
		{
	default:
		Panic(EDbInvalidBookmark);
	case ERowBeginning:
	case ERowEnd:
	case ERowDeletedAtEnd:
	case ERowInLimbo:
	case ERowInvalid:
		break;
	case ERowDeletedAtNext:
	case ERowOK:
		if (!iDataSource->GotoL(iRecord))
			__LEAVE(KErrNotFound);
		break;
		}
	iState=TUint8(state);
	}

void CDbTableCursor::GetL()
//
// read the current row into the row buffer for access
//
	{
	AssertValidRow();
	CheckStateL();
	iFlags&=~ERead;
	iDataSource->ReadRowL(iRecord);
	iFlags|=ERead;
	}

void CDbTableCursor::InsertL(TInsert aClearRow)
//
// Insert a new row. If aCLearRow==aCopy, then copy the current row
//
	{
	AssertNotInUpdate();
	CheckUpdateL();
	Transaction().DMLPrepareL(*this);
	if (aClearRow==ECopy)
		{
		AssertValidRow();
		iFlags&=~ERead;	// in case of failure in NewRowL
		iDataSource->NewRowL(iRecord);
		}
	else
		iDataSource->NewRowL(KDbNullRecordId);
	iFlags|=EInserting|ERead;
	Transaction().DMLBegin();
	}

void CDbTableCursor::UpdateL()
	{
	CheckUpdateL();
	Transaction().DMLPrepareL(*this);
	GetL();
	iFlags|=EUpdating;
	Transaction().DMLBegin();
	}

void CDbTableCursor::Cancel()
	{
	AssertNoStreams();
	if (InUpdate())
		{
		RDbTransaction& t=Transaction();
		if (iFlags&EDirty)
			t.DMLTouch();			// we've mucked about with BLOBs, so force true roll-back
		t.DMLRollback();
		if (iFlags&EUpdating)
			{
			TRAP_IGNORE(iDataSource->ReadRowL(KDbNullRecordId));	// row buffer contains NULL row	(cannot fail)
			}
		iFlags&=(EUpdatable|EReadable);
		}
	}

void CDbTableCursor::PutL()
	{
	AssertInUpdate();
	CheckStateL();
	CDbDataSource::TWrite mode=iFlags&EUpdating ? CDbDataSource::EReplace : CDbDataSource::EAppend;
	iDataSource->PrepareToWriteRowL(mode);
	RDbTransaction& t=Transaction();
	t.DMLTouch();
	iFlags&=~EDirty;
	iRecord=iDataSource->WriteRowL(mode,iFlags&EReadable ? CDbDataSource::ESynch : CDbDataSource::ENoSynch);
	t.DMLCommitL();
	if ((iFlags&(EInserting|EReadable))==(EInserting|EReadable))
		iState=ERowOK;
	iFlags&=(EUpdatable|EReadable|ERead);
	}

void CDbTableCursor::DeleteL()
	{
	AssertValidRow();
	CheckUpdateL();
	RDbTransaction& t=Transaction();
	t.DMLPrepareL(*this);
	t.DMLBeginLC();
	CDbDataSource::TDelete del=iDataSource->DeleteRowL(iRecord);
	t.DMLCommitL();
	CleanupStack::Pop();	// rollback not required
	iState=TUint8(del+ERowDeletedAtNext);
	}

TInt CDbTableCursor::ColumnCount()
	{
	return iColumns->Count();
	}

void CDbTableCursor::ColumnDef(TDbCol& aCol,TDbColNo aColNo)
	{
	iColumns->Check(aColNo);
	if (iValid)
		iDataSource->ColumnDef(aColNo).AsTDbCol(aCol);
	}

TDbColType CDbTableCursor::ColumnType(TDbColNo aCol)
	{
	return Type(aCol);
	}

void CDbTableCursor::ReplaceBlobL(TDbColumn& aCol)
	{
	CheckStateL();
	const TDbBlob& blob=TDbColumnC(aCol).Blob();
	if (!blob.IsInline())
		{
		iFlags|=EDirty;
		BlobsL().DeleteL(blob.Id());
		}
	}

void CDbTableCursor::AddBlobSource()
//
// Increment the source count and take a read-lock on the database
//
	{
	AddSource();
	Transaction().ReadBegin(*this);
	}

void CDbTableCursor::ReleaseBlobSource()
//
// Decrement the source count and release the read-lock on the database
//
	{
	ReleaseSource();
	Transaction().ReadRelease(*this);
	}

MStreamBuf* CDbTableCursor::ColumnSourceL(TDbColNo aCol)
	{
	TDbColumnC col(ColumnC(aCol));
	if ((iFlags&EWriteBuf) || iReadBuf==EMaxReadBuf)
		__LEAVE(KErrInUse);			// only allow 1-write or 255-read streambufs
	TDbColType type=iColumns->Type(aCol);
	if (!TDbCol::IsLong(type))
		return HMemBuf::NewL(*this,col.PtrC8());
	// blobs
	const TDbBlob& blob=col.Blob();
	if (blob.IsInline())
		return HMemBuf::NewL(*this,blob.PtrC8());
	// if small enough, pull the blob data through immediately and avoid locking the database
	if (blob.Size()<=HHeapBuf::EMaxBlobBuffer)
		return HHeapBuf::NewL(*this,blob,type);
	//
	CheckStateL();
	Transaction().ReadPrepareL(*this);
	HReadBuf* buf=HReadBuf::NewLC(*this);
	buf->Set(BlobsL().ReadL(blob.Id(),type));
	CleanupStack::Pop();
	return buf;
	}

MStreamBuf* CDbTableCursor::ColumnSinkL(TDbColNo aCol)
	{
	TDbColType type=Type(aCol);
	__ASSERT_ALWAYS(TDbCol::IsLong(type),Panic(EDbWrongType));
	TDbColumn col=Column(aCol);
	ReplaceBlobL(col);
	iFlags|=EDirty;
	return HWriteBuf::NewL(*this,col,type);
	}

void CDbTableCursor::SetNullL(TDbColNo aCol)
//
// Make the column Null
//
	{
	TDbColumn col=Column(aCol);
	if (TDbCol::IsLong(Type(aCol)))
		ReplaceBlobL(col);
	col.SetNull();
	}

TInt CDbTableCursor::ColumnSize(TDbColNo aCol)
	{
	TDbColumnC col(ColumnC(aCol));
	return TDbCol::IsLong(Type(aCol)) ? col.Blob().Size() : col.Size();
	}

RDbRow* CDbTableCursor::RowBuffer()
//
// Invoked by the server for whole-row access where possible
//
	{
	__ASSERT(iFlags&ERead);
	return iDataSource->RowBuffer();
	}

TDbColumnC CDbTableCursor::ColumnC(TDbColNo aCol)
//
// check row is valid for extraction
//
	{
	__ASSERT_ALWAYS(iFlags&ERead,Panic(EDbRowNotRead));
	return iDataSource->Column(aCol);
	}

TDbColumn CDbTableCursor::Column(TDbColNo aCol)
//
// check row is valid for writing
//
	{
	AssertInUpdate();
	return iDataSource->Column(aCol);
	}

void CDbTableCursor::SetIndexL(const TDesC* anIndex)
	{
	AssertNotInUpdate();
	CheckReadL();
	iDataSource->SetIndexL(anIndex);
	iState=ERowBeginning;
	}

TBool CDbTableCursor::SeekL(const TDbLookupKey& aKey,RDbTable::TComparison aComparison)
	{
	AssertNotInUpdate();
	CheckReadL();
	iFlags&=~ERead;
	iState=ERowInvalid;
	TBool atrow=iDataSource->SeekL(aKey,aComparison,iRecord);
	if (atrow)
		iState=ERowOK;
	return atrow;
	}

CDbRowConstraint* CDbTableCursor::OpenConstraintL(const TDbQuery& aCriteria)
//
// Construct a constraint for this rowset
//
	{
	CSqlSearchCondition* sc=iDataSource->ParseConstraintLC(aCriteria.Query());
	CDbRowConstraint* constraint=new(ELeave) CConstraint(*this,sc,aCriteria.Comparison());
	CleanupStack::Pop();
	return constraint;
	}

TBool CDbTableCursor::MatchL(CDbRowConstraint& aConstraint)
	{
	CConstraint& c=STATIC_CAST(CConstraint&,aConstraint);
	__ASSERT_ALWAYS(c.Check(*this),Panic(EDbRowSetConstraintMismatch));
	GetL();
	return c.MatchL();
	}

void CDbTableCursor::CheckReadL() const
//
// Ensure we are a readable cursor
//
	{
	CheckStateL();
	if ((iFlags&EReadable)==0)
		__LEAVE(KErrWrite);
	}

void CDbTableCursor::CheckUpdateL() const
//
// Ensure we are a writable cursor
//
	{
	CheckStateL();
	if ((iFlags&EUpdatable)==0)
		__LEAVE(KErrWrite);
	}

void CDbTableCursor::AssertNoStreams() const
	{
	__ASSERT_ALWAYS((iFlags&EWriteBuf)==0 && iReadBuf==0,Panic(EDbStreamOpen));
	}

void CDbTableCursor::AssertNotInUpdate() const
	{
	__ASSERT_ALWAYS(!InUpdate(),Panic(EDbInUpdate));
	AssertNoStreams();
	}

void CDbTableCursor::AssertInUpdate() const
	{
	__ASSERT_ALWAYS(InUpdate(),Panic(EDbNotInUpdate));
	AssertNoStreams();
	}

void CDbTableCursor::AssertValidRow() const
	{
	AssertNotInUpdate();
	__ASSERT_ALWAYS(iState==ERowOK||(iFlags&EInserting),Panic(EDbInvalidRow));
	}

