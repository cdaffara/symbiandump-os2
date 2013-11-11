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

#include "UM_STD.H"

const TUint32 KFirstStreamIdValue=KNullStreamIdValue+1;
const TInt KBufStoreGranularity=4;

EXPORT_C CBufStore* CBufStore::NewL(TInt anExpandSize)
/** Allocates and constructs a new in-memory store and returns a pointer to it.

@param anExpandSize The granularity of the buffers used in the implementation 
of the store. Each stream is contained in a separate CBufSeg buffer.
@return A pointer to the memory store object. */
	{
	return new(ELeave) CBufStore(anExpandSize);
	}

EXPORT_C CBufStore* CBufStore::NewLC(TInt anExpandSize)
/** Allocates and constructs a new in-memory store and returns a pointer to it, 
putting a pointer to the object onto the cleanup stack.

Putting a pointer to the object on the cleanup stack allows the object and 
allocated resources to be cleaned up if a subsequent leave occurs.

@param anExpandSize The granularity of the buffers used in the implementation 
of the store. Each stream is contained in a separate CBufSeg buffer.
@return A pointer to the memory store object. */
	{
	CBufStore* store=NewL(anExpandSize);
	CleanupStack::PushL(store);
	return store;
	}

EXPORT_C CBufStore::CBufStore(TInt anExpandSize)
//
// Construct a buffer store.
//
	: iBufArray(KBufStoreGranularity),iExpandSize(anExpandSize)
	{}

EXPORT_C CBufStore::~CBufStore()
/** Frees resources owned by the object, prior to its destruction. */
	{
	for (TInt i=0,n=iBufArray.Count(); i<n; i++)
		delete iBufArray[i];
	}

EXPORT_C TStreamId CBufStore::DoExtendL()
//
// Create a new buffer slot.
//
	{
	iBufArray.AppendL((CBufSeg*)NULL);
	return TStreamId(iBufArray.Count()-1+KFirstStreamIdValue);
	}

EXPORT_C void CBufStore::DoDeleteL(TStreamId anId)
//
// Delete the buffer at anId.
//
	{
	TInt i=anId.Value()-KFirstStreamIdValue;
	if (i<0||i>=iBufArray.Count())
		__LEAVE(KErrNotFound);
//
	CBufSeg*& buf=iBufArray[i];
	delete buf;
	buf=NULL;
	}

EXPORT_C MStreamBuf* CBufStore::DoReadL(TStreamId anId) const
//
// Open a stream for reading from the buffer at anId.
//
	{
	return HBufBuf::NewL(BufL(anId),0,HBufBuf::ERead);
	}

EXPORT_C MStreamBuf* CBufStore::DoCreateL(TStreamId& anId)
//
// Create a new buffer and open a stream for writing to it.
//
	{
	anId=DoExtendL();
	return HBufBuf::NewL(BufL(anId),0,HBufBuf::ERead|HBufBuf::EWrite);
	}

EXPORT_C MStreamBuf* CBufStore::DoWriteL(TStreamId anId)
//
// Open a stream for writing to the buffer at anId.
//
	{
	return HBufBuf::NewL(BufL(anId),0,HBufBuf::ERead|HBufBuf::EWrite);
	}

EXPORT_C MStreamBuf* CBufStore::DoReplaceL(TStreamId anId)
//
// Open a truncating stream for writing to the buffer at anId.
//
	{
	return HBufBuf::NewL(BufL(anId),0,HBufBuf::ERead|HBufBuf::ETruncate);
	}

CBufSeg& CBufStore::BufL(TStreamId anId) const
//
// Return this buffer store's buffer at anId.
//
	{
	TInt i=anId.Value()-KFirstStreamIdValue;
	if (i<0||i>=iBufArray.Count())
		__LEAVE(KErrNotFound);
//
	CBufSeg*& buf=CONST_CAST(CArrayFixFlat<CBufSeg*>&,iBufArray)[i];
	if (buf==NULL)
		buf=CBufSeg::NewL(iExpandSize);
	return *buf;
	}

