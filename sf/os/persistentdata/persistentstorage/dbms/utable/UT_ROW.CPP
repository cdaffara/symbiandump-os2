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

// Class RDbTableRow

void RDbTableRow::Open(CDbTable* aTable)
//
// Initialise the row buffer for using the table
//
	{
	__ASSERT(!iTable);
	__ASSERT(aTable);
	iTable=aTable;
	iMark.Construct(aTable->Generation());
	}

void RDbTableRow::Close()
	{
	if (iTable)
		iTable->Close();
	RDbRow::Close();
	}

EXPORT_C void RDbTableRow::ReadL(TDbRecordId aRecordId)
	{
	if (aRecordId==KDbNullRecordId)
		{
		Reset();
		iRecord=aRecordId;
		iMark.Mark();
		}
	else if (aRecordId!=iRecord || iMark.Changed())
		{
		Reset();
		iRecord=KDbNullRecordId;
		iTable->ReadRowL(*this,aRecordId);
		iRecord=aRecordId;
		iMark.Mark();
		}
	}

void RDbTableRow::NewL(TDbRecordId aCopyRecord)
//
// Set the row buffer for a new row
//
	{
	ReadL(aCopyRecord);			// copy or empty the row
	if (aCopyRecord!=KDbNullRecordId)
		{
		iRecord=KDbNullRecordId;
		iTable->DuplicateBlobsL(*this);		// duplicate any Blobs
		}
	iTable->NewRowL(*this);
	}

void RDbTableRow::PrepareAppendL()
	{
	iTable->PrepareAppendL(*this);
	}

TDbRecordId RDbTableRow::AppendL()
	{
	iRecord=iTable->AppendRowL(*this);
	iMark.Mark();
	return iRecord;
	}

void RDbTableRow::PrepareReplaceL()
	{
	if (iMark.Changed())
		__LEAVE(KErrAccessDenied);
	iTable->PrepareReplaceL(*this,iRecord);
	}

TDbRecordId RDbTableRow::ReplaceL()
	{
	iTable->ReplaceRowL(*this,iRecord);
	iMark.Mark();
	return iRecord;
	}

void RDbTableRow::DeleteL(TDbRecordId aRecordId)
	{
	iTable->DeleteRowL(*this,aRecordId);
	}
