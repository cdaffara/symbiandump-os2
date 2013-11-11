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

// Class CDbReverseIterator

NONSHARABLE_CLASS(CDbReverseIter) : public CDbRecordIter
	{
public:
	CDbReverseIter(CDbRecordIter* anIterator);
	~CDbReverseIter();
private:
	TInt Count() const;
	TDbRecordId CurrentL();
	TBool GotoL(TDbPosition aPosition);
	TBool GotoL(TDbRecordId aRecordId,RDbTableRow& aBuffer);
	TBool SeekL(const TDbLookupKey& aKey,RDbTable::TComparison aComparison);
	TDeleted DoDeletedL(TDbPosition aPosition,TDbRecordId aRecordId,const RDbTableRow* aRow);
private:
	CDbRecordIter* iIter;
	};

CDbReverseIter::CDbReverseIter(CDbRecordIter* aIterator)
	: CDbRecordIter(aIterator->Host()),iIter(aIterator)
	{}

CDbReverseIter::~CDbReverseIter()
	{
	delete iIter;
	}

TInt CDbReverseIter::Count() const
	{
	return iIter->Count();
	}

TDbRecordId CDbReverseIter::CurrentL()
	{
	return iIter->CurrentL();
	}

TBool CDbReverseIter::GotoL(TDbPosition aPosition)
	{
	return iIter->GotoL(TDbPosition(EDbLast-aPosition));
	}

TBool CDbReverseIter::GotoL(TDbRecordId aRecord,RDbTableRow& aRow)
	{
	return iIter->GotoL(aRecord,aRow);
	}

TBool CDbReverseIter::SeekL(const TDbLookupKey& aKey,RDbTable::TComparison aComparison)
//
// Reverse the comparison
//
	{
	return iIter->SeekL(aKey,RDbTable::TComparison(RDbTable::EGreaterThan-aComparison));
	}

CDbReverseIter::TDeleted CDbReverseIter::DoDeletedL(TDbPosition aPosition,TDbRecordId aRecordId,const RDbTableRow* aRow)
//
// Need to invert the position.
//
	{
	return iIter->DoDeletedL(TDbPosition(EDbLast-aPosition),aRecordId,aRow);
	}

// Class CDbTableSource

CDbTableSource::CDbTableSource()
	{}

CDbTableSource::~CDbTableSource()
	{
	delete iIter;
	iRow.Close();
	}

void CDbTableSource::Construct(CDbTable* aTable)
//
// We own the table pass in
//
	{
	iIterMark.Construct(aTable->Generation());
	iRow.Open(aTable);
	}

void CDbTableSource::ReverseIteratorL()
//
// Plug in a reverse iterator adaptor for the iterator
//
	{
	iIter=new(ELeave) CDbReverseIter(iIter);
	}

TInt CDbTableSource::ColumnCount() const
	{
	return iRow.Table().Def().Columns().Count();
	}

const TDbColumnDef& CDbTableSource::ColumnDef(TDbColNo aCol) const
	{
	return iRow.Table().Def().Columns()[aCol];
	}

RDbRow* CDbTableSource::RowBuffer()
	{
	return &iRow;
	}

TDbColumn CDbTableSource::Column(TDbColNo aColNo)
	{
	return TDbColumn(iRow,aColNo);
	}

void CDbTableSource::Reset()
	{
	iReset=ETrue;
	}

TBool CDbTableSource::EvaluateL(TInt&,TDbRecordId&,TBool&)
	{
	return EFalse;
	}

TBool CDbTableSource::Unevaluated()
	{
	return EFalse;
	}

TInt CDbTableSource::CountL()
//
// Count the records
//
	{
	if (iReset)
		OpenIteratorL();
	return iIter->Count();
	}

void CDbTableSource::OpenIteratorL()
//
// Re-open the iterator after a reset
//
	{
	__ASSERT(iIter);
	if (iIter->Host().OpenL())
		__LEAVE(KErrCorrupt);
	iReset=EFalse;
	}

CDbTableSource::TGoto CDbTableSource::GotoL(TInt& aWork,TDbPosition aPosition,TDbRecordId& aRecordId)
//
// Move to the position specified. If out of synch, try to resynch the iterator
// using aRecordId
//
	{
	__ASSERT(iIter);
	TInt work=aWork;
	if (work<0)
		return EExhausted;
	aWork=work-EWorkToIterate;
	CDbRecordIter& iter=*iIter;
	if (iIterMark.Changed())
		{	// two possible resaons for this
		if (iReset)
			OpenIteratorL();
		if (aPosition==EDbNext || aPosition==EDbPrevious)
			{	// table modified
			if (aRecordId==KDbNullRecordId)
				return ESynchFailure;
			if (!iter.GotoL(aRecordId,iRow))
				{	// record has been deleted
				switch(iter.DeletedL(aPosition,aRecordId))
					{
				case CDbRecordIter::EAtRow:			// found it
					aRecordId=iter.CurrentL();
					return ESuccess;
				case CDbRecordIter::ENoRow:			// no more rows
					aRecordId=KDbNullRecordId;
					return ENoRow;
				case CDbRecordIter::ENotSupported:	// couldn't re-snych
					return ESynchFailure;
					}
				}
			}
		iIterMark.Mark();
		}
	if (iter.GotoL(aPosition))
		{
		aRecordId=iter.CurrentL();
		return ESuccess;
		}
	else
		{
		aRecordId=KDbNullRecordId;
		return ENoRow;
		}
	}

TBool CDbTableSource::SynchL(TDbRecordId aRecordId)
	{
	if (iReset)
		OpenIteratorL();
	TBool found=iIter->GotoL(aRecordId,iRow);
	iIterMark.Mark();
	return found;
	}

TBool CDbTableSource::GotoL(TDbRecordId aRecordId)
	{
	return iRow.Table().ExistsL(aRecordId) ? SynchL(aRecordId) : EFalse;
	}

void CDbTableSource::ReadRowL(TDbRecordId aRecordId)
	{
	iRow.ReadL(aRecordId);
	}

void CDbTableSource::NewRowL(TDbRecordId aCopyRecord)
	{
	if (iIter!=NULL&&iReset)
		OpenIteratorL();
	iRow.NewL(aCopyRecord);
	}

void CDbTableSource::PrepareToWriteRowL(TWrite aWrite)
	{
	__ASSERT(aWrite==EReplace||aWrite==EAppend);
	if (aWrite==EReplace)
		iRow.PrepareReplaceL();
	else
		iRow.PrepareAppendL();
	}

TDbRecordId CDbTableSource::WriteRowL(TWrite aWrite,TSynch aSynch)
	{
	__ASSERT(aWrite==EReplace||aWrite==EAppend);
	TDbRecordId id=aWrite==EReplace ? iRow.ReplaceL() : iRow.AppendL();
	if (aSynch==ESynch)
		{
		__DEBUG(TBool dbgchk=) SynchL(id);
		__ASSERT(dbgchk);
		}
	return id;
	}

CDbTableSource::TDelete CDbTableSource::DeleteRowL(TDbRecordId& aRecordId,TSynch aSynch)
//
// Remove the row from the table. Synchronise the iterator if required
// When not synchronising, the return value is not significant
//
	{
	// if the table has changed, we may need to re-synch the iterator before deleting
	// this is important if neighbouring rows have been deleted by another view
	if (aSynch==ESynch && iIterMark.Changed() && !SynchL(aRecordId))
		__LEAVE(KErrNotFound);		// it has been deleted already!
	iRow.DeleteL(aRecordId);
	if (aSynch==ESynch)
		{
		CDbRecordIter::TDeleted del=iIter->DeletedL(EDbNext,aRecordId,iRow);
		__ASSERT(del!=CDbRecordIter::ENotSupported);	// must always be able to synch with row data
		iIterMark.Mark();
		if (del==CDbRecordIter::EAtRow)
			{	// pass back the next record id
			aRecordId=iIter->CurrentL();
			return EDeletedAtNext;
			}
		}
	return EDeletedAtEnd;
	}

void CDbTableSource::SetIndexL(const TDesC* anIndex)
	{
	CDbRecordIter* iter=anIndex ? iRow.Table().IteratorL(*anIndex) : iRow.Table().IteratorL();
	delete iIter;
	iIter=iter;
	}

TBool CDbTableSource::SeekL(const TDbLookupKey& aKey,RDbTable::TComparison aComparison,TDbRecordId& aRecordId)
	{
	if (iReset)
		OpenIteratorL();
	CDbRecordIter& iter=*iIter;
	TBool atrow=iter.SeekL(aKey,aComparison);
	aRecordId=atrow ? iter.CurrentL() : KDbNullRecordId;
	iIterMark.Mark();
	return atrow;
	}

CSqlSearchCondition* CDbTableSource::ParseConstraintLC(const TDesC& aCondition)
	{
	CSqlSearchCondition* sc=Sql::ParseSearchConditionLC(aCondition);
	sc->BindL(iRow);
	return sc;
	}
