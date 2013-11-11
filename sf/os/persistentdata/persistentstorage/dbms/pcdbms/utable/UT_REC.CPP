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

// Class CDbRecordBase

EXPORT_C TBool CDbRecordBase::DoRestoreL()
//
// If we haven't been opened, then try to restore the state
//
	{
	TBool broken=RestoreL();
	iFlags|=broken ? EBroken : EOpen;
	return broken;
	}

EXPORT_C void CDbRecordBase::TouchL()
//
// if we are not modified then mark
// Can touch a broken object, will not invoke AboutToModifyL()
//
	{
	__ASSERT(iFlags&(EOpen|EBroken));
	if (!(iFlags&EModified))
		{
		if (!(iFlags&EBroken))
			AboutToModifyL();
		iFlags|=EModified;
		}
	}

EXPORT_C void CDbRecordBase::FlushL()
//
// If we are open, modified and not broken then flush
// Must allow un-opened Flush during framework cleanup on failed open
//
	{
	if (iFlags==(EOpen|EModified))
		{
		SynchL();
		iFlags&=~EModified;
		}
	}

void CDbRecordBase::DoAbandon(CDbRecordBase* aRec)
	{
	aRec->Abandon();
	}

void CDbRecordBase::DoFlushL(CDbRecordBase* aRec)
	{
	aRec->FlushL();
	}

void CDbRecordBase::DoDelete(CDbRecordBase* aRec)
	{
	delete aRec;
	}

EXPORT_C TBool CDbRecordBase::RestoreL()
//
// default to do nothing
//
	{
	return EFalse;
	}

EXPORT_C void CDbRecordBase::AboutToModifyL()
//
// default to do nothing
//
	{}

EXPORT_C void CDbRecordBase::SynchL()
//
// default to do nothing
//
	{}


// Class CDbRecordIter

EXPORT_C CDbRecordIter::CDbRecordIter(CDbRecordBase& aHost)
	:iHost(aHost)
	{}

EXPORT_C CDbRecordIter::~CDbRecordIter()
	{}


// Class CDbRecordSpace

TUint8* CDbRecordSpace::ReplaceL(TDbRecordId aRecordId,TInt aRecordSize)
	{
	__ASSERT(!IsBroken());
	TouchL();
	return DoReplaceL(aRecordId,aRecordSize);
	}

TUint8* CDbRecordSpace::NewL(TInt aRecordSize)
	{
	__ASSERT(!IsBroken());
	TouchL();
	return DoNewL(aRecordSize);
	}

void CDbRecordSpace::EraseL(TDbRecordId aRecordId)
	{
	__ASSERT(!IsBroken());
	TouchL();
	DoEraseL(aRecordId);
	}

// Class CDbBlobSpace

EXPORT_C MStreamBuf* CDbBlobSpace::ReadLC(TDbBlobId aBlobId,TDbColType aType) const
	{
	MStreamBuf* buf=ReadL(aBlobId,aType);
	buf->PushL();
	return buf;
	}

EXPORT_C MStreamBuf* CDbBlobSpace::CreateL(TDbBlobId &aBlobId,TDbColType aType)
	{
	__ASSERT(!IsBroken());
	TouchL();
	return DoCreateL(aBlobId,aType);
	}

EXPORT_C TDbBlobId CDbBlobSpace::CreateL(TDbColType aType,const TAny* aData,TInt aLength)
//
// Create a blob containing the data buffer
//
	{
	TDbBlobId id;
	MStreamBuf* buf=CreateL(id,aType);
	buf->PushL();
	buf->WriteL(aData,aLength);
	buf->SynchL();
	CleanupStack::PopAndDestroy();
	return id;
	}

EXPORT_C void CDbBlobSpace::DeleteL(TDbBlobId aBlobId)
	{
	if (aBlobId!=KDbNullBlobId)
		{
		__ASSERT(!IsBroken());
		TouchL();
		DoDeleteL(aBlobId);
		}
	}

EXPORT_C TInt CDbBlobSpace::Delete(TDbBlobId aBlobId)
	{
	TRAPD(r,DeleteL(aBlobId));
	return r;
	}

// Class CDbRecordIndex

TBool CDbRecordIndex::InsertL(TDbRecordId aRecordId,const RDbTableRow& aRow)
	{
	__ASSERT(!IsBroken());
	TouchL();
	return DoInsertL(aRecordId,aRow);
	}

void CDbRecordIndex::DeleteL(TDbRecordId aRecordId,const RDbTableRow& aRow)
	{
	__ASSERT(!IsBroken());
	TouchL();
	DoDeleteL(aRecordId,aRow);
	}
