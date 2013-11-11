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

// Class CDbDataSource

CDbDataSource::TGoto CDbDataSource::GotoL(TDbPosition aPosition,TDbRecordId& aRecordId)
	{
	for (;;)
		{
		TInt work=KMaxTInt;
		TGoto r=GotoL(work,aPosition,aRecordId);
		if (r!=EExhausted)
			return r;
		}
	}

// Class CDbDataStage

CDbDataStage::~CDbDataStage()
	{
	delete iSource;
	}

RDbRow* CDbDataStage::RowBuffer()
	{
	return Source().RowBuffer();
	}

TDbColumn CDbDataStage::Column(TDbColNo aColNo)
	{
	return Source().Column(aColNo);
	}

void CDbDataStage::Reset()
	{
	Source().Reset();
	}

TBool CDbDataStage::EvaluateL(TInt& aWork,TDbRecordId& aRecordId,TBool& aAtRow)
	{
	return Source().EvaluateL(aWork,aRecordId,aAtRow);
	}

TBool CDbDataStage::Unevaluated()
	{
	return Source().Unevaluated();
	}

TInt CDbDataStage::CountL()
	{
	return Source().CountL();
	}

CDbDataStage::TGoto CDbDataStage::GotoL(TInt& aWork,TDbPosition aPosition,TDbRecordId& aRecordId)
	{
	return Source().GotoL(aWork,aPosition,aRecordId);
	}

TBool CDbDataStage::GotoL(TDbRecordId aRecordId)
	{
	return Source().GotoL(aRecordId);
	}

void CDbDataStage::ReadRowL(TDbRecordId aRecordId)
	{
	Source().ReadRowL(aRecordId);
	}

void CDbDataStage::NewRowL(TDbRecordId aCopyRecord)
	{
	Source().NewRowL(aCopyRecord);
	}

void CDbDataStage::PrepareToWriteRowL(TWrite aWrite)
	{
	Source().PrepareToWriteRowL(aWrite);
	}

TDbRecordId CDbDataStage::WriteRowL(TWrite aWrite,TSynch aSynch)
	{
	return Source().WriteRowL(aWrite,aSynch);
	}

CDbDataStage::TDelete CDbDataStage::DeleteRowL(TDbRecordId& aRecordId,TSynch aSynch)
	{
	return Source().DeleteRowL(aRecordId,aSynch);
	}

TInt CDbDataStage::ColumnCount() const
	{
	return Source().ColumnCount();
	}

const TDbColumnDef& CDbDataStage::ColumnDef(TDbColNo aCol) const
	{
	return Source().ColumnDef(aCol);
	}

void CDbDataStage::SetIndexL(const TDesC*)
//
// Only invoked from a RDbTable.
// No data stages are present for such a view
//
	{
	__ASSERT(0);
	}

TBool CDbDataStage::SeekL(const TDbLookupKey&,RDbTable::TComparison,TDbRecordId&)
//
// Only invoked from a RDbTable.
// No data stages are present for such a view
//
	{
	__ASSERT(0);
	return EFalse;
	}

CSqlSearchCondition* CDbDataStage::ParseConstraintLC(const TDesC& aCondition)
	{
	return Source().ParseConstraintLC(aCondition);
	}

