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

#include "UD_STD.H"

#define UNUSED_VAR(a) a = a

// Class TDbColumn

EXPORT_C void TDbColumn::SetL(const TAny* aPtr,TInt aSize)
	{
	Mem::Copy(Row().SetColumnWidthL(iColumn,aSize),aPtr,aSize);
	}

EXPORT_C void TDbColumn::SetL(TUint32 aValue)
	{
	*(TUint32*)Row().SetColumnWidthL(iColumn,sizeof(aValue))=aValue;
	}

EXPORT_C void TDbColumn::SetL(TInt64 aValue)
	{
	*(TInt64*)Row().SetColumnWidthL(iColumn,sizeof(aValue))=aValue;
	}

EXPORT_C void TDbColumn::SetL(TReal32 aValue) __SOFTFP
	{
	*(TReal32*)Row().SetColumnWidthL(iColumn,sizeof(aValue))=aValue;
	}

EXPORT_C void TDbColumn::SetL(TReal64 aValue) __SOFTFP
	{
	*(TReal64*)Row().SetColumnWidthL(iColumn,sizeof(aValue))=aValue;
	}

EXPORT_C void TDbColumn::SetL(const TDesC8& aValue)
	{
	SetL(aValue.Ptr(),aValue.Length());
	}

EXPORT_C void TDbColumn::SetL(const TDesC16& aValue)
	{
	SetL(aValue.Ptr(),aValue.Length()<<1);
	}

EXPORT_C void TDbColumn::SetBlobL(TDbBlobId aBlobId,TInt aSize)
	{
	new(Row().SetColumnWidthL(iColumn,TDbBlob::RefSize())) TDbBlob(aBlobId,aSize);
	}

EXPORT_C void TDbColumn::SetBlobL(const TUint8* aData,TInt aSize)
	{
	new(Row().SetColumnWidthL(iColumn,TDbBlob::InlineSize(aSize))) TDbBlob(aData,aSize);
	}

EXPORT_C TDbBlob& TDbColumn::InitBlobL()
	{
	return *new(Row().SetColumnWidthL(iColumn,sizeof(TDbBlob))) TDbBlob;
	}

EXPORT_C void TDbColumn::CommitBlob(const TDbBlob& aBlob)
	{
#if defined(_DEBUG)
	TDbColumnC colC(*this);
	__ASSERT(&colC.Blob()==&aBlob);
	__ASSERT(colC.Size()>=aBlob.CellSize());
#endif
	TRAPD(errCode, Row().SetColumnWidthL(iColumn,aBlob.Size() ? aBlob.CellSize() : 0));
    UNUSED_VAR(errCode);
	}

// The row buffer
// Minimum data width is 4 bytes for strict alignment
// No integral set members for small integers and extractors do the cast down from 4-byte integers

EXPORT_C RDbRow::RDbRow()
//
// default to dynamic-owned empty row buffer
//
	:iFirst(0),iLast(0),iEnd(0),iCell(0),iColumn(TUint(EOwned))
	{}

EXPORT_C void RDbRow::Open(TAny* aBuf,TInt aSize,TInt aMaxSize)
//
// Use the provided row buffer
//
	{
	iFirst=(TDbCell*)aBuf;
	iEnd=PtrAdd(iFirst,aMaxSize);
	iColumn=0;			// non-owned buffer
	SetSize(aSize);
	}

EXPORT_C void RDbRow::CreateL(TInt aMaxSize)
	{
	__ASSERT(!iFirst && Owned());
	GrowL(aMaxSize);
	}

void RDbRow::PushL()
//
// Dynamic reallocation requires that the buffer is pushed
//
	{
	CleanupClosePushL(*this);
	}

EXPORT_C void RDbRow::Close()
//
// release the buffer if owned, and return to default constructed state
//
	{
	if (Owned())
		User::Free(iFirst);
	iFirst=iLast=iEnd=iCell=0;
	iColumn=TUint(EOwned);
	}

EXPORT_C void RDbRow::SetSize(TInt aSize)
//
// Set the current buffer size: assert that it is valid
//
	{
	__ASSERT(PtrAdd(iFirst,aSize)<=iEnd);
	iLast=PtrAdd(iFirst,aSize);
	SetCache(iFirst,0);
	}

EXPORT_C void RDbRow::GrowL(TInt aMaxSize)
//
// Grow the buffer to at least aMaxSize, the buffer empty on return
//
	{
	if (aMaxSize>MaxSize())
		{
		ReallocL(aMaxSize);
		Reset();
	//
		__ASSERT(iFirst<=iEnd);
		__ASSERT(iFirst<=iLast && iLast<=iEnd);
		__ASSERT(iFirst<=iCell && iCell<=iLast);
		}
	}

void RDbRow::ExtendL(TInt aAdjust)
//
// Adjust the buffer length by aAdjust, leave if no room, preserve contents and cache
//
	{
	TDbCell* newLast=PtrAdd(iLast,aAdjust);
	if (newLast>iEnd)
		{
		TInt move=ReallocL(Diff(iFirst,newLast));
		iCell=PtrAdd(iCell,move);
		newLast=PtrAdd(newLast,move);
		}
	iLast=newLast;
// check invariant
	__ASSERT(iFirst<=iEnd);
	__ASSERT(iFirst<=iLast && iLast<=iEnd);
	__ASSERT(iFirst<=iCell && iCell<=iLast);
	}

TInt RDbRow::ReallocL(TInt aMaxSize)
//
// Grow the buffer to aMaxSize, return the offset of buffer movement
// leave iLast and cache untouched
//
	{
	__ASSERT(aMaxSize>MaxSize());
	if (!Owned())
		__LEAVE(KErrTooBig);	// cannot reallocate if not owned
//
	aMaxSize+=EGranularity-1;
	aMaxSize&=~(EGranularity-1);
	TDbCell* buf=(TDbCell*)User::ReAllocL(iFirst,aMaxSize);
	TInt move=Diff(iFirst,buf);
	iFirst=buf;
	iEnd=PtrAdd(buf,aMaxSize);
	return move;
	}

TDbCell* RDbRow::Column(TInt aColIx) const
//
// aCol is now zero-based
// Return a pointer to column aCol in the buffer
// Return 0 if the column is past the end
//
	{
	TInt ix=Column();
	TDbCell* pC;
	if (aColIx<ix)
		{
		ix=0;
		pC=iFirst;
		}
	else
		pC=iCell;
	TDbCell* last=iLast;
	for (;;)
		{
		if (pC>=last)
			break;
		if (ix>=aColIx)
			break;
		pC=pC->Next();
		++ix;
		}
	CONST_CAST(RDbRow*,this)->SetCache(pC,ix);
	return pC<last?pC:0;
	}

LOCAL_C const TDbCell* NullCell()
	{
	static TUint32 const NullCell[3]={0,0,0};	// 3 zero words represent any "Null" value
//
	return REINTERPRET_CAST(const TDbCell*,&NullCell[0]);
	}

EXPORT_C const TDbCell* RDbRow::ColCell(TDbColNo aColNo) const
	{
	__ASSERT(aColNo>0);
	const TDbCell* cell=Column(aColNo-1);
	return cell && cell->Length() ? cell : NullCell();
	}

EXPORT_C TAny* RDbRow::SetColumnWidthL(TDbColNo aColNo,TInt aWidth)
// set the width for column aCol to Width
// add extra NULL columns to buffer as required
// return pointer to data for that column
	{
	__ASSERT(aColNo>0);
	TDbCell* pC=Column(--aColNo);
	if (pC==0)
		{		// add extra NULL columns to buffer
		if (aWidth==0)
			return 0;	// setting to NULL, don't bother padding
		TInt cFill=(aColNo-iColumn)*sizeof(TInt);
		ExtendL(cFill+TDbCell::Size(aWidth));
		pC=iCell;
		Mem::FillZ(pC,cFill);
		pC=PtrAdd(pC,cFill);		// set cache
		SetCache(pC,aColNo);
		}
	else
		{
		TInt adjust=TDbCell::Size(aWidth)-pC->Size();	// how much to add
		if (adjust!=0)
			{
			ExtendL(adjust);
			pC=iCell;		// may have moved in extension
			TDbCell* pNext=pC->Next();
			TDbCell* pAdjust=PtrAdd(pNext,adjust);
			Mem::Move(pAdjust,pNext,Diff(pAdjust,iLast));
			}
		}
	pC->SetLength(aWidth);
	return pC->Data();
	}
