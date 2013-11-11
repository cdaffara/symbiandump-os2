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
// DBMS server proxy cursor
// 
//

#include "SD_STD.H"

const TDbBlobId KUnknownBlobId=~0u;

// Class HDbsColumns

class HDbsColumns
	{
public:
	static HDbsColumns* NewL(TInt aCount);
	inline void Set(const TUint8* aPtr);
	inline TPtr8 Ptr();
	inline TInt Count() const;
	inline TDbColType Type(TDbColNo aCol) const;
	void Check(TDbColNo aCol) const;
private:
	inline HDbsColumns(TInt aCount);
private:
	TInt iCount;
	TUint8 iType[1];
	};

inline HDbsColumns::HDbsColumns(TInt aCount)
	:iCount(aCount)
	{}
inline void HDbsColumns::Set(const TUint8* aPtr)
	{Mem::Copy(iType,aPtr,iCount);}
inline TPtr8 HDbsColumns::Ptr()
	{return TPtr8(iType,iCount);}
inline TInt HDbsColumns::Count() const
	{return iCount;}
inline TDbColType HDbsColumns::Type(TDbColNo aCol) const
	{__DEBUG(Check(aCol));return TDbColType(iType[aCol-1]);}

HDbsColumns* HDbsColumns::NewL(TInt aCount)
	{
	return new(User::AllocL(_FOFF(HDbsColumns,iType[aCount]))) HDbsColumns(aCount);
	}

void HDbsColumns::Check(TDbColNo aColNo) const
	{
	__ASSERT_ALWAYS(aColNo>0&&aColNo<=iCount,Panic(EDbsInvalidColumn));
	}

// Class CDbsCursor

CDbsCursor::CDbsCursor()
	:iState(EAtBeginning)
	{}

CDbsCursor* CDbsCursor::NewL(const RDbsObject& aDbs,TDbsFunction aFunction,TIpcArgs& aArgs)
	{
	CDbsCursor* self=new(ELeave) CDbsCursor;
	CleanupStack::PushL(self);
	self->ConstructL(aDbs,aFunction,aArgs);
	CleanupStack::Pop();
	return self;
	}

void CDbsCursor::ConstructL(const RDbsObject& aDbs,TDbsFunction aFunction,TIpcArgs& aArgs)
//
// construct a cursor from the supplied parameters
// Retrieve the column types, in one pass if possible
//
	{
	TPckgBuf<TDbsColumns> cols;
	aArgs.Set(3,&cols);
	iObject.OpenL(aDbs,aFunction,aArgs);
	iColumns=HDbsColumns::NewL(cols().iCount);
	if (cols().iCount<=cols().EMax)
		iColumns->Set(cols().iData);	// we have the column types
	else
		{	// too many for the fixed buffer, make a second call to get the columns
		TPtr8 ptr=iColumns->Ptr();
		aArgs.Set(3,&ptr);
		iObject.SendReceiveL(EDbsCursorColumnTypes,aArgs);
		}
	}

CDbsCursor::~CDbsCursor()
	{
	iObject.Close();
	delete iColumns;
	iRow.Close();
	}

TDbColType CDbsCursor::Type(TDbColNo aCol) const
	{
	iColumns->Check(aCol);
	return iColumns->Type(aCol);
	}

void CDbsCursor::Reset()
	{
	iState=EAtBeginning;
	iObject.SendReceive(EDbsCursorReset);
	}

TBool CDbsCursor::EvaluateL()
	{
	iState=EUnknown;
	return iObject.SendReceiveL(EDbsCursorEvaluate);
	}

void CDbsCursor::Evaluate(TRequestStatus& aStatus)
	{
	iState=EUnknown;
	iObject.SendReceive(EDbsCursorEvaluate,aStatus);
	}

TBool CDbsCursor::Unevaluated()
	{
	return iObject.SendReceive(EDbsCursorUnevaluated);
	}

void CDbsCursor::SetIndexL(const TDesC* anIndex)
	{
	iObject.SendReceiveL(EDbsCursorSetIndex,TIpcArgs(anIndex));
	iState=EAtBeginning;
	}

TBool CDbsCursor::SeekL(const TDbLookupKey& aKey,RDbTable::TComparison aComparison)
	{
	TPtrC8 key(TDbsParam::PrepareLC(aKey));
	iState=EUnknown;
	TBool found=iObject.SendReceiveL(EDbsCursorSeek,TIpcArgs(&key,key.Length(),aComparison));
	CleanupStack::PopAndDestroy();	// key
	if (found)
		iState=EAtRow;
	return found;
	}

TBool CDbsCursor::AtBeginning()
	{
	if (iState==EUnknown && iObject.SendReceive(EDbsCursorAtBeginning)>0)
		iState=EAtBeginning;
	return iState==EAtBeginning;
	}

TBool CDbsCursor::AtEnd()
	{
	if (iState==EUnknown && iObject.SendReceive(EDbsCursorAtEnd)>0)
		iState=EAtEnd;
	return iState==EAtEnd;
	}

TBool CDbsCursor::AtRow()
	{
	if (iState==EUnknown && iObject.SendReceive(EDbsCursorAtRow)>0)
		iState=EAtRow;
	return iState>=EAtRow;
	}

TInt CDbsCursor::CountL(RDbRowSet::TAccuracy aAccuracy)
//
// valid response is >=0 + undefined (==-1)
// server returns +1 for non-error conditions
//
	{
	return iObject.SendReceiveL(EDbsCursorCount,TIpcArgs(aAccuracy))-1;
	}

TBool CDbsCursor::GotoL(RDbRowSet::TPosition aPosition)
	{
	TBool atrow=RetrieveL(EDbsCursorGotoPos,aPosition);
	if (atrow)
		iState=ERetrieve;
	else
		{
		switch (aPosition)
			{
		default:
			__ASSERT(0);
		case RDbRowSet::EEnd:
		case RDbRowSet::EFirst:
		case RDbRowSet::ENext:
			iState=EAtEnd;
			break;
		case RDbRowSet::EBeginning:
		case RDbRowSet::ELast:
		case RDbRowSet::EPrevious:
			iState=EAtBeginning;
			break;
			}
		}
	return atrow;
	}

void CDbsCursor::Bookmark(TDbBookmark::TMark& aMark)
	{
	TPckg<TDbBookmark::TMark> pckg(aMark);
	iObject.SendReceive(EDbsCursorBookmark,TIpcArgs(TIpcArgs::ENothing,TIpcArgs::ENothing,TIpcArgs::ENothing,&pckg));
	}

void CDbsCursor::GotoL(const TDbBookmark::TMark& aMark)
	{
	TPckgC<TDbBookmark::TMark> pckg(aMark);
	iState=EUnknown;
	iObject.SendReceiveL(EDbsCursorGotoBookmark,TIpcArgs(&pckg));
	}

LOCAL_C void CancelCursor(TAny* aPtr)
	{
	STATIC_CAST(CDbCursor*,aPtr)->Cancel();
	}

TBool CDbsCursor::RetrieveL(TDbsFunction aFunction,TInt aArg0)
//
// Read the row buffer
//
	{
	iState=EUnknown;
	TInt size;
	for (;;)
		{
		TInt max=iRow.MaxSize();
		TPtr8 row((TUint8*)iRow.First(),max);
		size=iObject.SendReceiveL(aFunction,TIpcArgs(aArg0,TIpcArgs::ENothing,max,&row))-1;
		if (size<0)
			return EFalse;		// no row to retrieve
		if (size<=max)
			break;
		// didn't fit! Grow the buffer
		if (aFunction!=EDbsCursorRetrieveRow)
			{
			CleanupStack::PushL(TCleanupItem(CancelCursor,this));	// in case this goes wrong!
			aFunction=EDbsCursorRetrieveRow;
			}
		iRow.GrowL(size);
		}
	iRow.SetSize(size);
	if (aFunction==EDbsCursorRetrieveRow)
		CleanupStack::Pop();
	return ETrue;
	}

void CDbsCursor::GetL()
	{
	if (iState!=ERetrieve)
		RetrieveL(EDbsCursorGet);
	iState=ERead;
	}

void CDbsCursor::InsertL(TInsert aClearRow)
	{
	RetrieveL(EDbsCursorInsert,aClearRow);
	iState=EWrite;
	iChangedBlob=EFalse;
	}

void CDbsCursor::UpdateL()
	{
	RetrieveL(EDbsCursorUpdate);
	iState=EWrite;
	iChangedBlob=EFalse;
	}

void CDbsCursor::Cancel()
	{
	if (iState==EUnknown || iState==EWrite)
		iObject.SendReceive(EDbsCursorCancel);
	}

void CDbsCursor::PutL()
	{
	TInt size=iRow.Size();
	TPtrC8 row((const TUint8*)iRow.First(),size);
	iObject.SendReceiveL(EDbsCursorPut,TIpcArgs(&row,size,iChangedBlob));// if iChangedBlob false, server can optimize put
	iState=ERead;		// we can still look at the row we put
	}

void CDbsCursor::DeleteL()
	{
	iState=EUnknown;
	iObject.SendReceiveL(EDbsCursorDelete);
	}

TInt CDbsCursor::ColumnCount()
	{
	return iColumns->Count();
	}

void CDbsCursor::ColumnsL(CDbColSet& aColSet)
//
// optimise the retreival of all columns
//
	{
	TIpcArgs m;
	RReadStream in(HDbsBuf::NewLC(iObject,EDbsCursorColumns,m));
	in>>aColSet;
	CleanupStack::PopAndDestroy();	// buffer
	}

void CDbsCursor::ColumnDef(TDbCol& aCol,TDbColNo aColNo)
	{
	TPckg<TDbCol> pckg(aCol);
	iColumns->Check(aColNo);
	iObject.SendReceive(EDbsCursorColumnDef,TIpcArgs(aColNo,TIpcArgs::ENothing,TIpcArgs::ENothing,&pckg));
	}

TDbColType CDbsCursor::ColumnType(TDbColNo aCol)
	{
	return Type(aCol);
	}

RDbRow* CDbsCursor::RowBuffer()
//
// Invoked by the server-- should not be called
//
	{
	__ASSERT(0);
	return 0;
	}

TDbColumnC CDbsCursor::ColumnC(TDbColNo aCol)
//
// check row is valid for extraction
//
	{
	__ASSERT_ALWAYS(iState>=ERead,Panic(EDbsNoRowData));
	return TDbColumnC(iRow,aCol);
	}

TDbColumn CDbsCursor::Column(TDbColNo aCol)
//
// check row is valid for writing
//
	{
	__ASSERT_ALWAYS(iState==EWrite,Panic(EDbsNotInUpdate));
	return TDbColumn(iRow,aCol);
	}

void CDbsCursor::ReplaceBlobL(TDbColumn& aCol)
//
// We no longer know what the shape of a blob is, mark it as "unknown"
//
	{
	iChangedBlob=ETrue;
	aCol.SetBlobL(KUnknownBlobId);
	}

void CDbsCursor::SetNullL(TDbColNo aCol)
//
// Make the column Null
//
	{
	TDbColumn col=Column(aCol);
	if (!TDbCol::IsLong(Type(aCol)))
		col.SetNull();
	else if (!TDbColumnC(col).IsNull())
		{
		ReplaceBlobL(col);
		iObject.SendReceiveL(EDbsCursorSetNull,TIpcArgs(aCol));
		}
	}

TInt CDbsCursor::ColumnSize(TDbColNo aCol)
	{
	TDbColumnC col(ColumnC(aCol));
	if (!TDbCol::IsLong(Type(aCol)))
		return col.Size();
	TInt size=col.Blob().Size();
	if (size>=0)
		return size;
	// unknown size, so ask the server
	return iObject.SendReceive(EDbsCursorColumnSize,TIpcArgs(aCol));
	}

MStreamBuf* CDbsCursor::ColumnSourceL(TDbColNo aCol)
	{
	TDbColumnC col(ColumnC(aCol));
	if (!TDbCol::IsLong(iColumns->Type(aCol)))
		return HDbsReadBuf::NewL(col.PtrC8());
	// blobs
	const TDbBlob& blob=col.Blob();
	if (blob.IsInline())
		return HDbsReadBuf::NewL(blob.PtrC8());
	// get it from the server
	TIpcArgs args(aCol);
	return HDbsBuf::NewL(iObject,EDbsCursorColumnSource,args);
	}

MStreamBuf* CDbsCursor::ColumnSinkL(TDbColNo aCol)
	{
	TDbColumn col(Column(aCol));
	ReplaceBlobL(col);
	TIpcArgs args(aCol);
	return HDbsBuf::NewL(iObject,EDbsCursorColumnSink,args);
	}

CDbRowConstraint* CDbsCursor::OpenConstraintL(const TDbQuery& aQuery)
	{
	const TDesC& des = aQuery.Query();
	CDbsConstraint* c=new(ELeave) CDbsConstraint();
	CleanupStack::PushL(c);
	c->iObject.OpenL(iObject,EDbsCursorOpenConstraint,TIpcArgs(&des,aQuery.Comparison()));
	CleanupStack::Pop();
	return c;
	}

TBool CDbsCursor::MatchL(CDbRowConstraint& aConstraint)
	{
	TInt handle = STATIC_CAST(CDbsConstraint&,aConstraint).iObject.Handle();
	return iObject.SendReceiveL(EDbsCursorMatch,TIpcArgs(handle));
	}

TInt CDbsCursor::FindL(RDbRowSet::TDirection aDirection,const TDbQuery& aCriteria)
//
// Server returns result-KErrNotFound, to ensure non-leaving on not found
//
	{
	const TDesC& des = aCriteria.Query();
	iState=EUnknown;
	TInt f=iObject.SendReceiveL(EDbsCursorFind,TIpcArgs(&des,aCriteria.Comparison(),aDirection))+KErrNotFound;
	if (f>=0)
		iState=EAtRow;
	return f;
	}

// Class CDbsConstraint

CDbsConstraint::~CDbsConstraint()
	{
	iObject.Close();
	}
