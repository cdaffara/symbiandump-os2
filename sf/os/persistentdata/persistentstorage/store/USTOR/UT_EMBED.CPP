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

EXPORT_C CEmbeddedStore* CEmbeddedStore::FromL(RReadStream& aHost)
/** Opens the store hosted by the specified stream.

Note that ownership of the stream passes to the store; the referenced RReadStream 
is cleared.

@param aHost A reference to the stream hosting the embedded store.
@return A pointer to the embedded store object. */
	{
	CEmbeddedStore* store=FromLC(aHost);
	CleanupStack::Pop();
	return store;
	}

EXPORT_C CEmbeddedStore* CEmbeddedStore::FromLC(RReadStream& aHost)
/** Open the store hosted by the specified stream, putting a pointer to the store 
onto the cleanup stack.

Putting a pointer to the embedded store object onto the cleanup stack allows 
the object and allocated resources to be cleaned up if a subsequent leave 
occurs.

Note that ownership of the stream passes to the store and the referenced RReadStream 
is cleared.

@param aHost A reference to the stream hosting the embedded store.
@return A pointer to the embedded store object. */
	{
	CEmbeddedStore* store=CEmbeddedStore::DoNewLC(aHost.Source());
	store->MarshalL(aHost);
	return store;
	}

EXPORT_C CEmbeddedStore* CEmbeddedStore::NewL(RWriteStream& aHost)
/** Creates an embedded store within the specified host stream.

Note that ownership of the stream passes to the store and the referenced RWriteStream 
is cleared.

@param aHost A reference to the stream which is to host the embedded store.
@return A pointer to the embedded store object. */
	{
	CEmbeddedStore* store=NewLC(aHost);
	CleanupStack::Pop();
	return store;
	}

EXPORT_C CEmbeddedStore* CEmbeddedStore::NewLC(RWriteStream& aHost)
/** Creates an embedded store within the specified host stream, putting a pointer 
to the store onto the cleanup stack.

Putting a pointer to the embedded store object onto the cleanup stack allows 
the object and allocated resources to be cleaned up if a subsequent leave 
occurs.

Note that ownership of the stream passes to the store and the referenced RWriteStream 
is cleared.

@param aHost A reference to the stream which is to host the embedded store.
@return A pointer to the embedded store object. */
	{
	CEmbeddedStore* store=CEmbeddedStore::DoNewLC(aHost.Sink());
	store->ConstructL(aHost);
	return store;
	}

EXPORT_C void CEmbeddedStore::Detach()
/** Gives up ownership of the host stream buffer. The caller takes on the responsibility 
for discarding the buffer. */
	{
	iHost.Host();
	iHost.Release();
	}

EXPORT_C CEmbeddedStore::CEmbeddedStore(MStreamBuf* aHost)
	: iHost(aHost)
	{
	__ASSERT_DEBUG(aHost!=NULL,Panic(EStoreNotOpen));
	}

EXPORT_C void CEmbeddedStore::MarshalL(RReadStream& aStream)
//
// Second-phase construction for opening existing stores.
//
	{
	MStreamBuf* src=aStream.Source();
	aStream=RReadStream();
	__ASSERT_DEBUG(src!=NULL&&src==iHost.Host(),User::Invariant());
	iStart=src->TellL(MStreamBuf::ERead);
	RReadStream stream(src);
	stream>>iRoot;
	}

EXPORT_C void CEmbeddedStore::ConstructL(RWriteStream& aStream)
//
// Second-phase construction for creating new stores.
//
	{
	MStreamBuf* snk=aStream.Sink();
	aStream=RWriteStream();
	__ASSERT_DEBUG(snk!=NULL&&snk==iHost.Host(),User::Invariant());
	iStart=snk->TellL(MStreamBuf::EWrite);
	RWriteStream stream(snk);
	stream<<iRoot;
	}

EXPORT_C CEmbeddedStore::~CEmbeddedStore()
/** Frees resources owned by the object, prior to its destruction. In particular, 
the destructor closes the associated file. */
	{
	MStreamBuf* buf=iHost.Host();
	if (buf!=NULL)
		buf->Release();
	}

EXPORT_C MStreamBuf* CEmbeddedStore::DoReadL(TStreamId anId) const
	{
	return HDirectStoreBuf::OpenL(MUTABLE_CAST(TStreamExchange&,iHost),anId,HDirectStoreBuf::ERead);
	}

EXPORT_C MStreamBuf* CEmbeddedStore::DoCreateL(TStreamId& anId)
	{
	return HDirectStoreBuf::CreateL(iHost,anId,HDirectStoreBuf::ERead|HDirectStoreBuf::EWrite);
	}

EXPORT_C void CEmbeddedStore::DoSetRootL(TStreamId anId)
	{
	RShareWriteStream stream(iHost,iStart);
	stream.PushL();
	stream<<anId;
	CleanupStack::PopAndDestroy();
	iRoot=anId;
	}
	
EXPORT_C void CEmbeddedStore::DoCommitL()
	{
	iHost.HostL()->SynchL();
	}
	
CEmbeddedStore* CEmbeddedStore::DoNewLC(MStreamBuf* aHost)
	{
	aHost->PushL();
	CEmbeddedStore* store=new(ELeave) CEmbeddedStore(aHost);
	CleanupStack::Pop();
	CleanupStack::PushL(store);
	return store;
	}

