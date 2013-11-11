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

struct SCachePage
	{
	TCachePage iPage[1];
	TUint8 iData[KPoolPageSize];
	};

EXPORT_C CPageCache* CPageCache::NewL(TInt aPages)
/** Allocates and constructs a new page cache.

@param aPages Number of pages in the cache
@return New page cache object */
	{
	CPageCache* cache=CPageCache::NewLC(aPages);
	CleanupStack::Pop();
	return cache;
	}

EXPORT_C CPageCache* CPageCache::NewLC(TInt aPages)
/** Allocates and constructs a new page cache, and leaves it on the cleanup stack.

@param aPages Number of pages in the cache
@return New page cache object */
	{
	CPageCache* cache=new(ELeave) CPageCache;
	CleanupStack::PushL(cache);
	cache->ConstructL(aPages);
	return cache;
	}

EXPORT_C CPageCache::CPageCache()
	: iFree(_FOFF(TCachePage,iLink)-_FOFF(SCachePage,iPage[1]))/*,iPages(NULL),iEnd(NULL)*/
/** Default constructor. */
	{
#if defined(__PAGE_CACHE_STATS)
//	iStats.Reset();
#endif
	}

EXPORT_C void CPageCache::ConstructL(TInt aPages)
/** Second phase construction.

@param aPages Number of pages in the cache */
	{
	SCachePage* page=new(ELeave) SCachePage[aPages];
	SCachePage* end=page+aPages;
	iPages=page;
	iEnd=end;
	for (;page<end;++page)
		{
		page->iPage[0].iOwner=NULL;
		page->iPage[0].iChange=EPageNoChange;
		iFree.AddFirst(page->iPage[1]);
		}
	}

EXPORT_C CPageCache::~CPageCache()
/** Destructor.

All cache pages are deleted. */
	{
#if defined (_DEBUG)
	for (SCachePage* page=iPages,*end=iEnd;page<end;++page)
		__ASSERT_DEBUG(page->iPage[0].iOwner==NULL,Panic(EPageCacheInUse));
#endif
	delete[] iPages;
	}

TCachePage* CPageCache::Find(TCachePagePool* aPool,TPageRef aRef)
//
// Find a page in the cache.
//
	{
	for (SCachePage* page=iPages,*end=iEnd;page<end;++page)
		{
		if (page->iPage[0].iOwner!=aPool)
			{
			__ASSERT_DEBUG(page->iPage[1].IsQued(),User::Invariant());
				// no other page pool can have locked pages
			continue;
			}
//
		if (page->iPage[0].iRef==aRef)
			{
#if defined(__PAGE_CACHE_STATS)
			if (page->iPage[1].IsQued())
				iStats.Hit(); // finding a locked page is not a hit, I'm afraid
#endif
			return &page->iPage[1];
			}
		}
#if defined(__PAGE_CACHE_STATS)
	iStats.Miss();
#endif
	return NULL;
	}

EXPORT_C TPageAbandonFunction TCachePagePool::AcquireL()
/** Returns a function that abandons all locked pages for this page pool. 

@return A function that abandons all locked pages for this page pool. */
	{
	return &DoAbandon;
	}

EXPORT_C TAny* TCachePagePool::AllocL()
/** Allocates an unassigned page. 

@return Newly allocated page. */
	{
	CPageCache& cache=*iCache;
	__ASSERT_DEBUG(iCache!=NULL,Panic(EPageNotOpen));
	TCachePage* page=DoAllocL(cache);
	page[-1].iOwner=this;
	__ASSERT_DEBUG(page[-1].iChange==EPageNoChange,User::Invariant());
	page[-1].iRef=KNullPageRef;
	page->Deque();
	return page;
	}

EXPORT_C TAny* TCachePagePool::LockL(TPageRef aRef)
/** Locks a page and returns a pointer to it.

@param aRef Reference to page to lock
@return Locked page */
	{
	if (aRef==KNullPageRef)
		__LEAVE(KErrNotFound);
//
	CPageCache& cache=*iCache;
	__ASSERT_DEBUG(iCache!=NULL,Panic(EPageNotOpen));
	TCachePage* page=cache.Find(this,aRef);
	if (page==NULL)
		{ // it's not in the cache, allocate a free page
		page=DoAllocL(cache);
		page[-1].iOwner=NULL;
		__ASSERT_DEBUG(page[-1].iChange==EPageNoChange,User::Invariant());
		ReadL(aRef,page);
		page[-1].iOwner=this;
		page[-1].iRef=aRef;
		}
	__ASSERT_DEBUG(page->IsQued(),Panic(EPageDoubleLock)); // it mustn't be locked already
	page->Deque();
	return page;
	}

EXPORT_C TPageRef TCachePagePool::AssignL(const TAny* aPage,TPageReclamation aReclamation)
/** Assigns a reference to a new page and unlocks it. 

@param aPage Page to assign
@param aReclamation Flag for page reclaimation settings
@return Reference to page */
	{
	TCachePage* page=(TCachePage*)aPage;
	__ASSERT_DEBUG(page!=NULL&&!page->IsQued(),Panic(EPageNotLocked));
	__ASSERT_DEBUG(page[-1].iOwner==this&&page[-1].iRef==KNullPageRef,User::Invariant());
	CPageCache& cache=*iCache;
	__ASSERT_DEBUG(iCache!=NULL,Panic(EPageNotOpen));
	page[-1].iOwner=NULL;
	__ASSERT_DEBUG(page[-1].iChange==EPageNoChange,User::Invariant());
	cache.iFree.AddLast(*page);
	TPageRef ref=ExtendL(page,aReclamation);
	page[-1].iOwner=this;
	page[-1].iRef=ref;
	return ref;
	}

EXPORT_C void TCachePagePool::UpdateL(const TAny* aPage)
/** Updates a page. 

@param aPage Page to update */
	{
	TCachePage* page=(TCachePage*)aPage;
	__ASSERT_DEBUG(page!=NULL&&!page->IsQued(),Panic(EPageNotLocked));
	__ASSERT_DEBUG(page[-1].iOwner==this&&page[-1].iRef!=KNullPageRef,User::Invariant());
	CPageCache& cache=*iCache;
	__ASSERT_DEBUG(iCache!=NULL,Panic(EPageNotOpen));
	page[-1].iOwner=NULL;
	page[-1].iChange=EPageNoChange;
	cache.iFree.AddLast(*page);
	WriteL(page[-1].iRef,page,EPageUpdate);
	page[-1].iOwner=this;
	}

EXPORT_C void TCachePagePool::Unlock(const TAny* aPage,TPageChange aChange)
//
// Unlock a page, depending on aChange:
//		EPageNoChange: just unlock
//		EPageDirty: mark the page as dirty
//		EPageUpdate: mark the page as needing a safe update
//		EPageAbandon: discard the page
//
/** Unlocks a page.

@param aPage Page to unlock
@param aChange How the page should be treated once it is unlocked */
	{
	TCachePage* page=(TCachePage*)aPage;
	__ASSERT_DEBUG(page!=NULL&&!page->IsQued(),Panic(EPageNotLocked));
	__ASSERT_DEBUG(page[-1].iOwner==this&&page[-1].iRef!=KNullPageRef,User::Invariant());
	CPageCache& cache=*iCache;
	__ASSERT_DEBUG(iCache!=NULL,Panic(EPageNotOpen));
	if (aChange>page[-1].iChange)
		page[-1].iChange=aChange;
	else if (aChange<EPageNoChange)
		{
		page[-1].iOwner=NULL;
		page[-1].iChange=EPageNoChange;
		}
	cache.iFree.AddLast(*page);
	}

EXPORT_C TInt TCachePagePool::Flush()
/** Flush all pages in this pool from the cache.

It ensures that any dirty pages are written into persistent storage, but does 
not remove them from the cache.

@return KErrNone if successful, otherwise another of the system-wide error 
codes. */
	{
	TRAPD(r,FlushL());
	return r;
	}

EXPORT_C void TCachePagePool::FlushL()
/** Flushes all pages in this pool from the cache, leaving with a system-wide error 
code if an error occurs. */
	{
	CPageCache& cache=*iCache;
	__ASSERT_DEBUG(iCache!=NULL,Panic(EPageNotOpen));
	for (SCachePage* page=cache.iPages,*end=cache.iEnd;page<end;++page)
		{
		__ASSERT_DEBUG(page->iPage[1].IsQued(),User::Invariant()); // there mustn't be any locked pages
		if (page->iPage[0].iOwner!=this)
			continue;
//
		TPageChange change=page->iPage[0].iChange;
		if (change>EPageNoChange)
			{
			WriteL(page->iPage[0].iRef,&page->iPage[1],change);
			page->iPage[0].iChange=EPageNoChange;
			}
		}
	}

EXPORT_C void TCachePagePool::Purge()
/** Purge all pages in this pool from the cache.

This discards all pages from the cache, without saving dirty pages. */
	{
	CPageCache& cache=*iCache;
	__ASSERT_DEBUG(iCache!=NULL,Panic(EPageNotOpen));
	for (SCachePage* page=cache.iPages,*end=cache.iEnd;page<end;++page)
		{
		__ASSERT_DEBUG(page->iPage[1].IsQued(),User::Invariant()); // there mustn't be any locked pages
		if (page->iPage[0].iOwner!=this)
			continue;
//
		page->iPage[0].iOwner=NULL;
		page->iPage[0].iChange=EPageNoChange;
		page->iPage[1].Link().Deque();
		cache.iFree.AddFirst(page->iPage[1]);
		}
	}

EXPORT_C void TCachePagePool::DoDeleteL(TPageRef aRef)
//
// Default implementation removing from cache.
//
	{
	__ASSERT_DEBUG(aRef!=KNullPageRef,User::Invariant());
	CPageCache& cache=*iCache;
	__ASSERT_DEBUG(iCache!=NULL,Panic(EPageNotOpen));
	TCachePage* page=cache.Find(this,aRef);
	if (page!=NULL)
		{
		page[-1].iOwner=NULL;
		page[-1].iChange=EPageNoChange;
		page->Link().Deque();
		cache.iFree.AddFirst(*page);
		}
	}

void TCachePagePool::DoAbandon(MPagePool& aPool)
//
// Abandon all locked pages in this page pool.
//
	{
	TCachePagePool& pool=STATIC_CAST(TCachePagePool&,aPool);
	CPageCache& cache=*pool.iCache;
	__ASSERT_DEBUG(pool.iCache!=NULL,Panic(EPageNotOpen));
	for (SCachePage* page=cache.iPages,*end=cache.iEnd;page<end;++page)
		{
		if (page->iPage[1].IsQued())
			continue;
//
		__ASSERT_DEBUG(page->iPage[0].iOwner==&pool,User::Invariant());
			// no other page pool can have locked pages
		page->iPage[0].iOwner=NULL;
		page->iPage[0].iChange=EPageNoChange;
		cache.iFree.AddFirst(page->iPage[1]);
		}
	}

TCachePage* TCachePagePool::DoAllocL(CPageCache& aCache)
	{
	__ASSERT_ALWAYS(!aCache.iFree.IsEmpty(),Panic(EPageCacheFull));
		// too small a cache or pages left locked unnecessarily
	TCachePage* page=aCache.iFree.First();
	TPageChange change=page[-1].iChange;
	if (change>EPageNoChange)
		{
		page[-1].iOwner->WriteL(page[-1].iRef,page,change);
		page[-1].iChange=EPageNoChange;
		}
	return page;
	}

