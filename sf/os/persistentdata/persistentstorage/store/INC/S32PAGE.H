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

#if !defined(__S32PAGE_H__)
#define __S32PAGE_H__
#if !defined(__S32STRM_H__)
#include <s32strm.h>
#endif

/** Size of the pages in the page pool. */
const TInt KPoolPageSize=512;
//
const TUint32 KNullPageRefValue=0;

/**
 * @publishedAll 
 * @released
 * Encapsulates a page reference.

A page reference is an integer value that can be used to identify a page. 
 */
class TPageRef
	{
public:
	/** Default constructor. */
	inline TPageRef() {}
	inline TPageRef(TUint32 aValue);
//
	inline TBool operator==(TPageRef aRef) const;
	inline TBool operator!=(TPageRef aRef) const;
//
	inline void ExternalizeL(RWriteStream& aStream) const;
	inline void InternalizeL(RReadStream& aStream);
//
	inline TUint32 Value() const;
private:
	TUint32 iVal;
	};
#if defined(__NO_CLASS_CONSTS__)
#define KNullPageRef TPageRef(KNullPageRefValue)
#else
/** Defines a null page reference. */
const TPageRef KNullPageRef=TPageRef(KNullPageRefValue);
#endif

class MPagePool;
/** Typedef to define a function that abandons pages in page pool. It is used by 
MPagePool::AcquireL().

@see MPagePool::AcquireL() */
typedef void (*TPageAbandonFunction)(MPagePool& aPool);

/** Flags that define how allocated pages can be reclaimed.

A BTree can locate some pages even when the tree is broken, but not all. The 
ones it cannot track must be tracked by the page pool in order to reclaim 
the storage if the tree breaks. */
enum TPageReclamation 
	/** The page can be deleted, but its space not reclaimed.
	
	The page pool will not track these pages, so to retrieve the space the page 
	must be deleted explicitly. */
	{EPageDeleteOnly,
	/** Page can be reclaimed.
	
	The page pool will track these pages, and will be able to reclaim the pages 
	when, for example, RStorePagePool::ReclaimL() is called. */
	EPageReclaimable};

/** Flags that define how a page should be treated when it is unlocked. */
enum TPageChange 
	/** Unlock only. */
	{EPageNoChange,
	/** Mark the page as dirty. */
	EPageDirty,
	/** Mark the page as needing a safe update. */
	EPageUpdate,
	/** Discard the page. */
	EPageAbandon=-1};


/**
 * @publishedAll 
 * @released
 * Interface to a page pool, the storage abstraction used by the B-trees API. 

The interface is abstract and handles pages as TAny pointers. It is left to 
derived classes to implement page storage in a particular storage medium, 
such as memory or disk.  
*/
class MPagePool
	{
public:
	IMPORT_C void PushL();
	inline void Pop();

	/** Returns a function that abandons all locked pages for this page pool. 
	
	@return A function that abandons all locked pages for this page pool. */
	virtual TPageAbandonFunction AcquireL()=0;
//
	virtual TAny* AllocL()=0;

	/** Locks a page and returns a pointer to it.
	
	@param aRef Reference to the page to lock
	@return Locked page */
	virtual TAny* LockL(TPageRef aRef)=0;
//
	virtual TPageRef AssignL(const TAny* aPage,TPageReclamation aReclamation=EPageDeleteOnly)=0;

	/** Updates a page. 
	
	This can be used for cached pages that may have become outdated.
	
	@param aPage Page to update */
	virtual void UpdateL(const TAny* aPage)=0;

	/** Unlocks a page.
	
	@param aPage Page to unlock
	@param aChange How the page should be treated once it is unlocked */
	virtual void Unlock(const TAny* aPage,TPageChange aChange=EPageNoChange)=0;
//
	IMPORT_C void Delete(TPageRef aRef);
	IMPORT_C void DeleteL(TPageRef aRef);
protected:
	virtual void DoDeleteL(TPageRef aRef)=0;
	};

/**
 * @publishedAll 
 * @released
 * Uses memory to implement the MPagePool page pool interface.

The class allocates pages from the default heap, storing them in an array. 
This pool is not persistent.
 */
class CMemPagePool : public CBase,public MPagePool
	{
public:
	IMPORT_C static CMemPagePool* NewL();
	IMPORT_C static CMemPagePool* NewLC();
	IMPORT_C CMemPagePool();
	IMPORT_C ~CMemPagePool();
//
	IMPORT_C TPageAbandonFunction AcquireL();
	IMPORT_C TAny* AllocL();
	IMPORT_C TAny* LockL(TPageRef aRef);
	IMPORT_C TPageRef AssignL(const TAny* aPage,TPageReclamation aReclamation=EPageDeleteOnly);
	IMPORT_C void UpdateL(const TAny* aPage);
	IMPORT_C void Unlock(const TAny* aPage,TPageChange aChange=EPageNoChange);
protected:
	IMPORT_C void DoDeleteL(TPageRef aRef);
private:
	TAny*& PageL(TPageRef aRef);
	static void DoAbandon(MPagePool& aPool);
private:
	CArrayFixSeg<TAny*> iPages;
	};
//
#if defined(_DEBUG)&&!defined(__PAGE_CACHE_STATS)
//#define __PAGE_CACHE_STATS
#endif
//
class TCachePage;
struct SCachePage;
class TCachePagePool;

/**
 * @publishedAll 
 * @released
 * Provides a cache for page pools. 

Persistent page pools rely on a cache to provide in-memory space for their 
pages and to cache frequently accessed pages.  
*/
class CPageCache : public CBase
	{
public:
	enum {EDefaultPages=20};
#if defined(__PAGE_CACHE_STATS)
	
	/**
	 * @publishedAll 
	 * @released
	 */
	class TStats
		{
	public:
		inline TInt Requests() const;
		inline TInt Hits() const;
		inline TInt Misses() const;
		inline void Reset();
	private:
		inline void Hit();
		inline void Miss();
	private:
		TInt iRequests;
		TInt iMisses;
	private:
		friend class CPageCache;
		};
#endif
public:
	IMPORT_C static CPageCache* NewL(TInt aPages=EDefaultPages);
	IMPORT_C static CPageCache* NewLC(TInt aPages=EDefaultPages);
	IMPORT_C CPageCache();
	IMPORT_C void ConstructL(TInt aPages=EDefaultPages);
	IMPORT_C ~CPageCache();
//
#if defined(__PAGE_CACHE_STATS)
	inline TStats& Stats();
	inline const TStats& Stats() const;
#endif
private:
	TCachePage* Find(TCachePagePool* aPool,TPageRef aRef);
private:
	SCachePage* iPages;
	SCachePage* iEnd;
	TDblQue<TCachePage> iFree;
#if defined(__PAGE_CACHE_STATS)
	TStats iStats;
#endif
private:
	friend class TCachePagePool;
	};

/**
 * @publishedAll 
 * @released
 * Provides a page pool with cached pages.

It is an intermediary class, used in the definition of page pools that use 
a cache, such as RFilePagePool and RStorePagePool.

@see RFilePagePool
@see RStorePagePool  
*/
class TCachePagePool : public MPagePool
	{
public:
	inline void Set(CPageCache& aCache);
//
	IMPORT_C TPageAbandonFunction AcquireL();
	IMPORT_C TAny* AllocL();
	IMPORT_C TAny* LockL(TPageRef aRef);
	IMPORT_C TPageRef AssignL(const TAny* aPage,TPageReclamation aReclamation=EPageDeleteOnly);
	IMPORT_C void UpdateL(const TAny* aPage);
	IMPORT_C void Unlock(const TAny* aPage,TPageChange aChange=EPageNoChange);
//
	IMPORT_C TInt Flush();
	IMPORT_C void FlushL();
	IMPORT_C void Purge();
protected:
	inline TCachePagePool();
	inline TCachePagePool(CPageCache& aCache);
	IMPORT_C void DoDeleteL(TPageRef aRef);
private:
	virtual TPageRef ExtendL(const TAny* aPage,TPageReclamation aReclamation)=0;
	virtual void WriteL(TPageRef aRef,const TAny* aPage,TPageChange aChange)=0;
	virtual void ReadL(TPageRef aRef,TAny* aPage)=0;
//
	static void DoAbandon(MPagePool& aPool);
	static TCachePage* DoAllocL(CPageCache& aCache);
private:
	CPageCache* iCache;
	};

#include <s32page.inl>
#endif
