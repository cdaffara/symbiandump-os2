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

#include "UP_STD.H"

#include <pbe.h>

EXPORT_C void TStorePagePoolToken::ExternalizeL(RWriteStream& aStream) const
/** Externalises a TStorePagePoolToken object to a stream.

@param aStream Stream to which the object should be externalised */
	{
	aStream<<iHead;
	aStream<<iAvail;
	}

EXPORT_C void TStorePagePoolToken::InternalizeL(RReadStream& aStream)
/** Internalises a TStorePagePoolToken object from a stream.

@param aStream Stream from which the object should be internalised */
	{
	aStream>>iHead;
	aStream>>iAvail;
	}

// Class RStorePagePool

EXPORT_C RStorePagePool::RStorePagePool()
	: iStore(NULL)
/** Default constructor. */
	{}

EXPORT_C RStorePagePool::RStorePagePool(CPageCache& aCache)
	: TCachePagePool(aCache),iStore(NULL)
/** Constructor with a page cache for the pool.

@param aCache Page cache for the pool */
	{}

EXPORT_C RStorePagePool::RStorePagePool(CStreamStore& aStore)
/** Constructor with a stream store to use for the pool.

@param aStore Stream store to use for the pool */
	{
	Create(aStore);
	}

EXPORT_C RStorePagePool::RStorePagePool(CStreamStore& aStore,const TStorePagePoolToken& aToken)
/** Constructor with a stream store and settings to use for the pool.

@param aStore Stream store to use for the pool
@param aToken Stream store pool settings */
	{
	Open(aStore,aToken);
	}

EXPORT_C void RStorePagePool::Create(CStreamStore& aStore)
/** Creates a new pool.

@param aStore Stream store to use for the pool */
	{
	iStore=&aStore;
	iHead=KNullStreamId;
	iAvail=KNullPageRef;
	iDirty=EFalse;
	}

EXPORT_C void RStorePagePool::Open(CStreamStore& aStore,const TStorePagePoolToken& aToken)
/** Opens an existing pool.

@param aStore Stream store for the pool
@param aToken Pool settings */
	{
	iStore=&aStore;
	iHead=aToken.iHead;
	iAvail=aToken.iAvail;
	iDirty=EFalse;
	}

EXPORT_C TStorePagePoolToken RStorePagePool::Token() const
/** Gets an object that encapsulates the page pool settings.   

That object can then be used to externalise the settings.

@return Encapsulates the page pool settings */
	{
	return TStorePagePoolToken(iHead,iAvail);
	}

EXPORT_C void RStorePagePool::Close()
/** Flushes and purges the page cache and stops using the stream store. */
	{
	TCachePagePool::Flush();
	Release();
	}

EXPORT_C TBool RStorePagePool::ReclaimL()
/** Deletes the first stream that stores reclaimable pages in the store.

@return True if there are remaining streams (pages), false if there are no 
more streams */
	{
	__ASSERT_DEBUG(iStore!=NULL,Panic(EPageNotOpen));
	__ASSERT_ALWAYS(iAvail==KNullPageRef,Panic(EPageReclaimAvailable));
	if (iHead!=KNullStreamId)
		{
		RStoreReadStream out;
		out.OpenLC(*iStore,iHead);
		TStreamId next;
		out>>next;
		CleanupStack::PopAndDestroy();
		iStore->DeleteL(iHead);
		iHead=next;
		MarkDirty();
		if (next!=KNullStreamId)
			return ETrue;
		}
	return EFalse;
	}

EXPORT_C void RStorePagePool::ReclaimAllL()
/** Deletes all streams in the store. */
	{
	while (ReclaimL())
		;
	}

EXPORT_C TPageRef RStorePagePool::ExtendL(const TAny* aPage,TPageReclamation aReclamation)
//
// Create a new page.
//
	{
	return StorePagePool::ExtendL(*this,aPage,aReclamation);
	}

EXPORT_C void RStorePagePool::WriteL(TPageRef aRef,const TAny* aPage,TPageChange aChange)
//
// Write a page to the store.
//
	{
	StorePagePool::WriteL(*this,aRef,aPage,aChange);
	}

EXPORT_C void RStorePagePool::ReadL(TPageRef aRef,TAny* aPage)
//
// Read from the store into a page.
//
	{
	StorePagePool::ReadL(*this,aRef,aPage);
	}

EXPORT_C void RStorePagePool::DoDeleteL(TPageRef aRef)
//
// Delete from store.
//
	{
	StorePagePool::DeleteL(*this,aRef);
	}

// Class RSecureStorePagePool



EXPORT_C RSecureStorePagePool::RSecureStorePagePool(const CPBEncryptSet& aKey)
	: iKey(aKey)
	{}

EXPORT_C RSecureStorePagePool::RSecureStorePagePool(CPageCache& aCache, const CPBEncryptSet& aKey)
	: RStorePagePool(aCache),
	iKey(aKey)
	{}

/** Adds a new page to the pool.

@param aPage Data for the page
@param aReclamation Flags that define how allocated pages can be reclaimed
@return Reference to newly created page */
EXPORT_C TPageRef RSecureStorePagePool::ExtendL(const TAny* aPage,TPageReclamation aReclamation)
	{		
	return StorePagePool::ExtendL(*this,aPage,aReclamation);
	}

/** Writes data to a page in the pool.

@param aRef Reference to the page to which to write
@param aPage Data to write
@param aChange Flags that define how a page should be treated when it is unlocked 
for writing */
EXPORT_C void RSecureStorePagePool::WriteL(TPageRef aRef,const TAny* aPage,TPageChange aChange)
	{
	StorePagePool::WriteL(*this,aRef,aPage,aChange);
	}

/** Reads a specified page from the pool.

@param aRef Reference to page to read
@param aPage On return, the page data */
EXPORT_C void RSecureStorePagePool::ReadL(TPageRef aRef,TAny* aPage)
	{
	StorePagePool::ReadL(*this,aRef,aPage);
	}

/** Deletes a specified page.

@param aRef Reference to page to delete */
EXPORT_C void RSecureStorePagePool::DoDeleteL(TPageRef aRef)
	{
	StorePagePool::DeleteL(*this,aRef);
	}

// Class StorePagePool
// the implementation of the clever bits to share code between the secure and plain sotre page pools

const TInt KMaxPageIndex=15;

inline TInt PageSize(const CPBEncryptionBase* aKey)
	{
	return aKey ? aKey->MaxCiphertextLength(KPoolPageSize) : KPoolPageSize;
	}

inline TPageRef PageRef(TStreamId anId,TInt anIndex=0)
	{
	__ASSERT_DEBUG(anIndex>=0&&anIndex<=KMaxPageIndex,User::Invariant());
	return TPageRef((anId.Value()<<4)+anIndex);
	}
inline TInt PageIndex(TPageRef aRef)
	{return aRef.Value()&0xf;}
inline TStreamId StreamId(TPageRef aRef)
	{return TStreamId(aRef.Value()>>4);}
inline TStreamPos PagePos(TInt anIndex,TInt aPageSize)
	{return TStreamPos(sizeof(TStreamId)+(anIndex-1)*aPageSize);}

void StorePagePool::PadL(RWriteStream& aStream,TInt aLength)
//
// zero-pad a stream
//
	{
	TUint8 zero[KPoolPageSize];
	Mem::FillZ(zero,KPoolPageSize);
	while ((aLength-=KPoolPageSize)>0)
		aStream.WriteL(zero,KPoolPageSize);
	aStream.WriteL(zero,aLength+KPoolPageSize);
	}

//
// Encrypt the page to the stream, padding out to full page size
void StorePagePool::EncryptL(RWriteStream& aStream,const TAny* aPage,const CPBEncryptionBase& aKey)
	{
	REncryptStream encrypt;
	encrypt.OpenLC(aStream,aKey);
	encrypt.WriteL((const TUint8*)aPage,KPoolPageSize);
	encrypt.CommitL();
	CleanupStack::PopAndDestroy(&encrypt);
	}

//
// Encrypt the page to the stream, padding out to full page size
//
void StorePagePool::EncryptNewL(RWriteStream& aStream,const TAny* aPage,const CPBEncryptionBase& aKey)
	{
	TStreamPos pos=aStream.Sink()->TellL(MStreamBuf::EWrite);		// remember the start of the data
	EncryptL(aStream,aPage,aKey);
	TInt pad=PageSize(&aKey)-(aStream.Sink()->TellL(MStreamBuf::EWrite)-pos);		// bytes written
	__ASSERT_DEBUG(pad>=0,Panic(EPageCipherTextOverrun));
	if (pad)
		PadL(aStream,pad);
	}

//
// Encrypt the page to the stream, padding out to full page size
//
void StorePagePool::DecryptL(RReadStream& aStream,TAny* aPage,const CPBEncryptionBase& aKey)
	{
	RDecryptStream decrypt;
	decrypt.OpenLC(aStream,aKey);
	decrypt.ReadL((TUint8*)aPage,KPoolPageSize);
	CleanupStack::PopAndDestroy();
	}

void StorePagePool::SeekL(MStreamBuf* aBuf,TInt aMark,TPageRef aRef,const CPBEncryptionBase* aKey)
	{
	TInt ix=PageIndex(aRef);
	if (ix)
		aBuf->SeekL(aMark,PagePos(ix,PageSize(aKey)));
	}

TPageRef StorePagePool::ExtendL(RStorePagePool& aPool,const TAny* aPage,TPageReclamation aReclamation,const CPBEncryptionBase* aKey)
	{
	__ASSERT_DEBUG(aPool.iStore!=NULL,Panic(EPageNotOpen));
	TPageRef page;
	RStoreWriteStream out;
	if (aReclamation==EPageDeleteOnly)
		{	// non-reclaimable page
		page=PageRef(out.CreateLC(*aPool.iStore));
		}
	else if (aPool.iAvail!=KNullPageRef)
		{	// use free page
		page=aPool.iAvail;
		out.OpenLC(*aPool.iStore,StreamId(page));
		MStreamBuf* buf=out.Sink();
		SeekL(buf,MStreamBuf::ERead|MStreamBuf::EWrite,page,aKey);
		RReadStream in(buf);
		in>>aPool.iAvail;
		aPool.MarkDirty();
		}
	else
		{	// allocate new block, and create free list
		TStreamId id=out.CreateLC(*aPool.iStore);
		out<<aPool.iHead;			// block list
		const TInt pad=PageSize(aKey)-sizeof(TPageRef);
		page=KNullPageRef;	// free-list terminator
		for (TInt index=0;++index<KMaxPageIndex;)
			{
			out<<page;
			PadL(out,pad);
			page=PageRef(id,index);
			}
		aPool.MarkDirty();
		aPool.iHead=id;
		aPool.iAvail=page;
		page=PageRef(id,KMaxPageIndex);	// page is written at end
		}
	if (aKey)
		EncryptNewL(out,aPage,*aKey);
	else
		out.WriteL((const TUint8*)aPage,KPoolPageSize);
	out.CommitL();
	CleanupStack::PopAndDestroy();
	return page;
	}

void StorePagePool::WriteL(RStorePagePool& aPool,TPageRef aRef,const TAny* aPage,TPageChange aChange,const CPBEncryptionBase* aKey)
	{
	__ASSERT_DEBUG(aChange>=EPageDirty,Panic(EPageChangeInvalid));
	__ASSERT_DEBUG(aPool.iStore!=NULL,Panic(EPageNotOpen));
	RStoreWriteStream out;
	if (aChange==EPageUpdate)
		{
		__ASSERT_DEBUG(PageIndex(aRef)==0,Panic(EPageChangeInvalid));
		out.ReplaceLC(*aPool.iStore,StreamId(aRef));
		if (aKey)
			EncryptNewL(out,aPage,*aKey);
		else
			out.WriteL((const TUint8*)aPage,KPoolPageSize);
		}
	else
		{	// re-write the page, no padding req'd
		out.OpenLC(*aPool.iStore,StreamId(aRef));
		SeekL(out.Sink(),MStreamBuf::EWrite,aRef,aKey);
		if (aKey)
			EncryptL(out,aPage,*aKey);
		else
			out.WriteL((const TUint8*)aPage,KPoolPageSize);
		}
	out.CommitL();
	CleanupStack::PopAndDestroy();
	}

void StorePagePool::ReadL(RStorePagePool& aPool,TPageRef aRef,TAny* aPage,const CPBEncryptionBase* aKey)
	{
	__ASSERT_DEBUG(aPool.iStore!=NULL,Panic(EPageNotOpen));
	RStoreReadStream in;
	in.OpenLC(*aPool.iStore,StreamId(aRef));
	SeekL(in.Source(),MStreamBuf::ERead,aRef,aKey);
	if (aKey)
		DecryptL(in,aPage,*aKey);
	else
		in.ReadL((TUint8*)aPage,KPoolPageSize);
	CleanupStack::PopAndDestroy();	// in
	}

void StorePagePool::DeleteL(RStorePagePool& aPool,TPageRef aRef,const CPBEncryptionBase* aKey)
	{
	aPool.CacheDeleteL(aRef);
	__ASSERT_DEBUG(aPool.iStore!=NULL,Panic(EPageNotOpen));
	if (PageIndex(aRef)==0)
		{	//non-reclaimable
		aPool.iStore->DeleteL(StreamId(aRef));
		return;
		}
	// add to free list
	RStoreWriteStream out;
	out.OpenLC(*aPool.iStore,StreamId(aRef));
	SeekL(out.Sink(),MStreamBuf::EWrite,aRef,aKey);
	out<<aPool.iAvail;
	out.CommitL();
	CleanupStack::PopAndDestroy();
	aPool.iAvail=aRef;
	aPool.MarkDirty();
	}
