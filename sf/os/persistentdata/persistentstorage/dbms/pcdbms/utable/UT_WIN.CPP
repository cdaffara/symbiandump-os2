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

// Class CDbBasicWindowStage

CDbBasicWindowStage::CDbBasicWindowStage(const TDbWindow& aWindow)
	: iWindow(aWindow), iRecords(EWindowArrayGranularity), iPos(-1)
	{
	__ASSERT(aWindow.Size()!=aWindow.ENone);
	}

TBool CDbBasicWindowStage::GetRecord(TDbRecordId& aRecordId)
	{
	if (TUint(iPos)>=TUint(iRecords.Count()))
		return EFalse;
	aRecordId=iRecords[iPos];
	return ETrue;
	}

void CDbBasicWindowStage::Reset()
//
// Reset the window to initial state
//
	{
	CDbDataStage::Reset();
	iRecords.Reset();
	iPos=-1;
	}

TBool CDbBasicWindowStage::EvaluateL(TInt& aWork,TDbRecordId& aRecordId,TBool& aAtRow)
//
// Do as much work as we can to make the window match the desired shape
//
	{
	TBool eval=CDbDataStage::EvaluateL(aWork,aRecordId,aAtRow);
	if (!eval)
		{
		eval=DoEvaluateL(aWork);
		aAtRow=GetRecord(aRecordId);
		}
	return eval;
	}

TInt CDbBasicWindowStage::CountL()
//
// Window'd views only report the evaluated records
//
	{
	return iRecords.Count();
	}

CDbBasicWindowStage::TGoto CDbBasicWindowStage::GotoL(TInt& /*aWork*/,TDbPosition aPosition,TDbRecordId& aRecordId)
	{
	switch (aPosition)
		{
	default:
		__ASSERT(0);
		break;
	case EDbFirst:
		iPos=0;
		break;
	case EDbLast:
		iPos=iRecords.Count()-1;
		break;
	case EDbNext:
		++iPos;
		break;
	case EDbPrevious:
		--iPos;
		break;
		}
	return GetRecord(aRecordId) ? ESuccess : ENoRow;
	}

TInt CDbBasicWindowStage::Find(TDbRecordId aRecordId,TInt& aPos)
	{
	TKeyArrayFix key(0,ECmpTUint32);
	return iRecords.Find(aRecordId,key,aPos);
	}

TBool CDbBasicWindowStage::GotoL(TDbRecordId aRecordId)
	{
	return Find(aRecordId,iPos)==0;
	}

void CDbBasicWindowStage::ReadRowL(TDbRecordId aRecordId)
	{
	TRAPD(r,CDbDataStage::ReadRowL(aRecordId));
	if (r==KErrNotFound)
		{
		TInt pos;
		if (Find(aRecordId,pos)==KErrNone)
			{
			iRecords.Delete(pos);
			if (pos<iPos)
				--iPos;
			}
		}
	__LEAVE_IF_ERROR(r);
	}

TDbRecordId CDbBasicWindowStage::WriteRowL(TWrite aWrite,TSynch aSynch)
	{
	TDbRecordId id=CDbDataStage::WriteRowL(aWrite,ENoSynch);
	if (aWrite==EAppend && iWindow.Size()==iWindow.EUnlimited)
		{
		iRecords.AppendL(id);
		if (aSynch==ESynch)
			iPos=iRecords.Count()-1;	// follow the append
		}
	return id;
	}

CDbBasicWindowStage::TDelete CDbBasicWindowStage::DeleteRowL(TDbRecordId& aRecordId,TSynch)
//
// Remove the row from the window if it is present
//
	{
	CDbDataStage::DeleteRowL(aRecordId,ENoSynch);
	if (GotoL(aRecordId))
		{
		iRecords.Delete(iPos);
		if (GetRecord(aRecordId))
			return EDeletedAtNext;
		}
	return EDeletedAtEnd;
	}

// Class CDbWindowStage

CDbWindowStage::CDbWindowStage(const TDbWindow& aWindow)
	: CDbBasicWindowStage(aWindow), iIterPos(EAtBeginning), iView(EBeginning)
	{}

void CDbWindowStage::Reset()
//
// Reset the window to initial state
//
	{
	CDbBasicWindowStage::Reset();
	iIterPos=EAtBeginning;
	iView=EBeginning;
	}

TInt CDbWindowStage::WhatToEvaluate()
//
// count of slots to fill, <0 at beginning, >0 at end. 0 none
//
	{
	if (iView==EAll)
		return 0;
	if (iWindow.Size()==iWindow.EUnlimited)
		return KMaxTInt;
	TInt space=iWindow.Size()-iRecords.Count();
	TInt lag=iPos-iWindow.PreferredPos();
	switch (iView)
		{
	default:
		__ASSERT(0);
	case EBeginning:
		return space+Max(lag,0);	// fill up and use forward lag if any
	case EEnd:
		return Min(lag,0)-space;	// fill up backwards and use rear lag if any
	case EMiddle:
		if (lag<0 && iIterPos==EAtBeginning)	// use iterator position if we can
			return lag;
		if (space+lag>0)
			return space+lag;
		if (lag<0)
			return lag;
		return 0;
		}
	}

TDbPosition CDbWindowStage::ResetIterToBeginningL()
	{
	for (TInt ii=iRecords.Count();--ii>=0;)
		{
		if (CDbDataStage::GotoL(iRecords[0]))
			return EDbPrevious;
// has been deleted, try the next one
		iRecords.Delete(0);
		if (iPos>0)
			--iPos;
		}
// no records to work with, start at the end
	return EDbLast;
	}

TDbPosition CDbWindowStage::ResetIterToEndL()
	{
	for (TInt ii=iRecords.Count();--ii>=0;)
		{
		if (CDbDataStage::GotoL(iRecords[ii]))
			return EDbNext;
// has been deleted, try the next one
		iRecords.Delete(ii);
		if (iPos>ii)
			--iPos;
		}
// no records to work with, start at the beginning
	return EDbFirst;
	}

TDbPosition CDbWindowStage::SetIteratorL(TInt anEval)
//
// Set the iterator for some work and return the first iterator direction
//
	{
	switch (iIterPos)
		{
	default:
		__ASSERT(0);
	case EAtBeginning:
		if (anEval<0)
			return EDbPrevious;
// turn around iterator to work at end
		iIterPos=EAtEnd;
		return ResetIterToEndL();
	case EAtEnd:
		if (anEval>0)
			return EDbNext;
// turn around iterator to work at beginning
		iIterPos=EAtBeginning;
		return ResetIterToBeginningL();
		}
	}

void CDbWindowStage::ExtendAtBeginningL(TInt aRecords,TDbPosition aFirst,TInt& aWork)
	{
	TDbRecordId id=iRecords.Count()>0 ? iRecords[0] : KDbNullRecordId;
	while (aRecords>0)
		{
		switch (CDbDataStage::GotoL(aWork,aFirst,id))
			{
		default:
			__ASSERT(0);
		case EExhausted:
			return;
		case ESuccess:
			if (iRecords.Count()==iWindow.Size())
				{	// drop last record
				iRecords.Delete(iRecords.Count()-1);
				if (iView==EEnd)
					iView=EMiddle;
				}
			iRecords.InsertL(0,id);
			++iPos;
			if (aFirst==EDbLast)
				aFirst=EDbPrevious;
			--aRecords;
			break;
		case ENoRow:	// no more data that way
			iView=iView==EEnd ? EAll : EBeginning;
			return;
		case ESynchFailure:	// have to do some work on the iterator now
			aFirst=ResetIterToBeginningL();
			break;
			}
		}
	}

void CDbWindowStage::ExtendAtEndL(TInt aRecords,TDbPosition aFirst,TInt& aWork)
	{
	TDbRecordId id=iRecords.Count()>0 ? iRecords[iRecords.Count()-1] : KDbNullRecordId;
	while (aRecords>0)
		{
		switch (CDbDataStage::GotoL(aWork,aFirst,id))
			{
		default:
			__ASSERT(0);
		case EExhausted:
			return;
		case ESuccess:
			if (iRecords.Count()==iWindow.Size())
				{	// drop first record
				iRecords.Delete(0);
				--iPos;
				if (iView==EBeginning)
					iView=EMiddle;
				}
			iRecords.AppendL(id);
			if (aFirst==EDbFirst)
				aFirst=EDbNext;
			--aRecords;
			break;
		case ENoRow:
			iView=iView==EBeginning ? EAll : EEnd;
			return;
		case ESynchFailure:
			aFirst=ResetIterToEndL();
			break;
			}
		}
	}

TBool CDbWindowStage::DoEvaluateL(TInt& aWork)
//
// Do as much work as we can to make the window match the desired shape
//
	{
	for (;;)
		{
		TInt eval=WhatToEvaluate();
		if (eval==0)
			return EFalse;
		if (aWork<=0)
			return ETrue;
		TDbPosition dir=SetIteratorL(eval);
		if (eval>0)
			ExtendAtEndL(eval,dir,aWork);
		else
			ExtendAtBeginningL(-eval,dir,aWork);
		}
	}

TBool CDbWindowStage::Unevaluated()
//
// Return whether it is worth Evaluating
//
	{
	return WhatToEvaluate()==0 ? CDbDataStage::Unevaluated() : ETrue;
	}
