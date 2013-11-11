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

#if !defined(__S32STOR_H__)
#define __S32STOR_H__
#if !defined(__S32STD_H__)
#include <s32std.h>
#endif
#if !defined(__S32SHARE_H__)
#include <s32share.h>
#endif
#if !defined(__S32PAGE_H__)
#include <s32page.h>
#endif

class MIncrementalCollector;
/**
 * @publishedAll 
 * @released
 * Provides the core abstract framework for stores allowing streams to be 
created and manipulated.  
*/
class CStreamStore : public CBase
	{
public:
	inline TStreamId ExtendL();
	IMPORT_C void Delete(TStreamId anId);
	IMPORT_C void DeleteL(TStreamId anId);
//
	IMPORT_C TInt Commit();
	inline void CommitL();
	IMPORT_C void Revert();
	inline void RevertL();
//
	IMPORT_C TInt ReclaimL();
	IMPORT_C TInt CompactL();
private:
	virtual IMPORT_C TStreamId DoExtendL();
	virtual IMPORT_C void DoDeleteL(TStreamId anId);
	
	/** Opens the requested stream for reading. The function should return a 
	stream buffer positioned at the beginning of this stream.
	
	This function is called by the OpenL() and OpenLC() member functions of 
	RStoreReadStream.
	
	@param anId The stream to be read.
	@return A stream buffer positioned at the beginning of the stream to be read.
	@see RStoreReadStream::OpenL()
	@see RStoreReadStream::OpenLC() */
	virtual MStreamBuf* DoReadL(TStreamId anId) const=0;
	
	/** Creates a new stream in the store. The function gets the allocated 
	stream id in the anId parameter. A stream buffer for the stream should be 
	returned, ready to write into the new stream. This provides the 
	implementation for the RStoreWriteStream::CreateL() functions.
	
	@param anId On return, contains the allocated stream id.
	@return The stream buffer to be written to. */
	virtual MStreamBuf* DoCreateL(TStreamId& anId)=0;
	virtual IMPORT_C MStreamBuf* DoWriteL(TStreamId anId);
	virtual IMPORT_C MStreamBuf* DoReplaceL(TStreamId anId);
	virtual IMPORT_C void DoCommitL();
	virtual IMPORT_C void DoRevertL();
	virtual IMPORT_C MIncrementalCollector* DoReclaimL();
	virtual IMPORT_C MIncrementalCollector* DoCompactL();
private:
	friend class RStoreReadStream;
	friend class RStoreWriteStream;
	friend class RStoreReclaim;
	};

/**
 * @publishedAll 
 * @released
 * Persistent store abstract base class. It provides the behaviour for setting 
and retrieving the root stream id.

Before closing a persistent store, the root stream id must be set. After opening 
a persistent store, the first thing done is to look up the root stream id. 
The root stream can then be opened and data read from the store.

@see CFileStore  
*/
class CPersistentStore : public CStreamStore
	{
public:
	inline TStreamId Root() const;
	inline void SetRootL(TStreamId anId);
protected:
	inline CPersistentStore();
private:
	virtual IMPORT_C void DoSetRootL(TStreamId anId);
protected:
	TStreamId iRoot;
	};

/**
 * @publishedAll 
 * @released
 * Performs space reclamation or compaction on a permanent file store in 
incremental steps.

Reclaiming unused space makes it available for re-use by the store. Compacting 
makes unused space available for re-use by the relevant system pool — for 
example, the filing system in the case of file-based stores.

Once compaction is complete, the store must be committed.

Notes:

Space reclamation and compaction are only supported by the file store 
CPermanentFileStore and are not supported by embedded or direct file stores. 

Use active objects when implementing space reclamation or compaction 
asynchronously.

This class performs incremental compaction/reclamation. These operations can 
be performed in a possibly long running single step using CStreamStore 
functions.  
*/
class RStoreReclaim
	{
public:
	inline RStoreReclaim();
	IMPORT_C void OpenL(CStreamStore& aStore,TInt& aCount);
	IMPORT_C void OpenLC(CStreamStore& aStore,TInt& aCount);
	IMPORT_C void CompactL(CStreamStore& aStore,TInt& aCount);
	IMPORT_C void CompactLC(CStreamStore& aStore,TInt& aCount);
	inline void Close();
	IMPORT_C void Release();
//
	IMPORT_C void ResetL(TInt& aCount);
	IMPORT_C void NextL(TInt& aStep);
	IMPORT_C void Next(TPckgBuf<TInt>& aStep,TRequestStatus& aStatus);
	IMPORT_C void NextL(TPckgBuf<TInt>& aStep,TRequestStatus& aStatus);
	IMPORT_C TInt Next(TInt& aStep);
//
	inline TInt Available() const;
private:
	MIncrementalCollector* iCol;
	TPckgBuf<TInt> iAvail;
	};

/**
 * @publishedAll 
 * @released
 * Encapsulates an embedded store. 

The embedded store may contain an arbitrarily complex network of streams, 
but is viewed as simply another stream by the embedding store. This means 
that the embedded store can dealt with as a single stream for purposes of 
copying or deleting.

Once streams within the embedded store have been committed and closed, they 
cannot subsequently be changed, i.e. streams cannot be replaced, deleted, 
extended or changed in any way. 

@see CPersistentStore  
*/
class CEmbeddedStore : public CPersistentStore
	{
public:
	IMPORT_C static CEmbeddedStore* FromL(RReadStream& aHost);
	IMPORT_C static CEmbeddedStore* FromLC(RReadStream& aHost);
	IMPORT_C static CEmbeddedStore* NewL(RWriteStream& aHost);
	IMPORT_C static CEmbeddedStore* NewLC(RWriteStream& aHost);
//
	inline static TStreamPos Position(TStreamId anId);
//
	IMPORT_C void Detach();
	inline void Reattach(MStreamBuf* aHost);
	inline MStreamBuf* Host() const;
	inline TStreamPos Start() const;
//
	IMPORT_C CEmbeddedStore(MStreamBuf* aHost);
	IMPORT_C void MarshalL(RReadStream& aStream);
	IMPORT_C void ConstructL(RWriteStream& aStream);
	IMPORT_C ~CEmbeddedStore();
protected:
	IMPORT_C MStreamBuf* DoReadL(TStreamId anId) const;
	IMPORT_C MStreamBuf* DoCreateL(TStreamId& anId);
private:
	IMPORT_C void DoSetRootL(TStreamId anId);
	IMPORT_C void DoCommitL();
//
	static CEmbeddedStore* DoNewLC(MStreamBuf* aHost);
private:
	__MUTABLE TStreamExchange iHost;
	TStreamPos iStart;
	};

/**
 * @publishedAll 
 * @released
 * Dictionary store interface.

This is an abstract class which provides the necessary interface for using 
concrete dictionary stores.

A dictionary store is a store where a stream is accessed by UID (TUid), rather 
than directly by stream id (TStreamId).

This type of store contains streams in the usual way but, in addition, the 
root stream is a stream dictionary. The stream dictionary provides a list 
of two-way associations between unique identifiers and stream ids.

Note that a dictionary store object does not derive from CStreamStore, but 
owns a persistent store and a stream dictionary as part of its implementation.

@see CStreamDictionary
@see CPersistentStore
@see CDictionaryFileStore
@see TUid
@see TStreamId  
*/
class CDictionaryStore : public CBase
	{
public:
	IMPORT_C TBool IsNullL() const;
	IMPORT_C TBool IsPresentL(TUid aUid) const;
	IMPORT_C void Remove(TUid aUid);
	IMPORT_C void RemoveL(TUid aUid);
//
	IMPORT_C TInt Commit();
	IMPORT_C void CommitL();
	IMPORT_C void Revert();
	IMPORT_C void RevertL();
//
	IMPORT_C ~CDictionaryStore();
protected:
	IMPORT_C void ConstructL();
private:
	CStreamDictionary* DictionaryL() const;
	MStreamBuf* GetSourceL(TUid aUid) const;
	MStreamBuf* GetSinkL(TUid aUid);
protected:
	CPersistentStore* iStore;
private:
	__MUTABLE CStreamDictionary* iDictionary;
	TBool iDictionaryHasChanged;
private:
	friend class RDictionaryReadStream;
	friend class RDictionaryWriteStream;
	friend class HDictionaryStoreBuf;
	};
//
const TInt KDictionaryCommitThreshold = 1024;

/**
 * @publishedAll 
 * @released
 * Supports the opening and manipulation of a stream in a dictionary store.

Construct an object of this type to open an existing stream in a dictionary 
store for reading.

@see CDictionaryStore  
*/
class RDictionaryReadStream : public RReadStream
{
public:
	IMPORT_C void OpenL(const CDictionaryStore& aDictStore,TUid aUid);
	IMPORT_C void OpenLC(const CDictionaryStore& aDictStore,TUid aUid);
	};

/**
 * @publishedAll 
 * @released
 * Supports the creation or replacement of a stream a dictionary store.

@see CDictionaryStore  
*/
class RDictionaryWriteStream : public RWriteStream
	{
public:
	/** Constructs an uninitialised object. It is necessary because there are 
	also non-default constructors in this class. */
	RDictionaryWriteStream() {}
	inline RDictionaryWriteStream(const MExternalizer<TStreamRef>& anExter);
	IMPORT_C void AssignL(CDictionaryStore& aDictStore,TUid aUid);
	IMPORT_C void AssignLC(CDictionaryStore& aDictStore,TUid aUid);
	};

/**
 * @publishedAll 
 * @released
 * Persistent settings to use for a RStorePagePool.

@see RStorePagePool 
*/
class TStorePagePoolToken
	{
public:
	/** Provides a TStorePagePoolToken initialisation flag. */
	enum TEmpty 
		/** Initialise for an empty page pool flag. */
		{EEmpty};
public:
	/** Default constructor. */
	TStorePagePoolToken() {}

	/** Constructor that intialises the TStorePagePoolToken for an empty page pool.
	
	@param Intialises for an empty page pool */
	inline TStorePagePoolToken(TEmpty);
	inline void Touch();
//
	inline TBool HasAvailable() const;
	inline TBool IsEmpty() const;
//
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
	IMPORT_C void InternalizeL(RReadStream& aStream);
private:
	inline TStorePagePoolToken(TStreamId aHead,TPageRef anAvail);
private:
	TStreamId iHead;
	TPageRef iAvail;
private:
	friend class RStorePagePool;
	};
#if defined(__NO_CLASS_CONSTS__)
#define KEmptyStorePagePoolToken TStorePagePoolToken(TStorePagePoolToken::EEmpty)
#else
/** Defines a TStorePagePoolToken object initialised for an empty page pool. */
const TStorePagePoolToken KEmptyStorePagePoolToken=TStorePagePoolToken::EEmpty;
#endif

/**
 * @publishedAll 
 * @released
 * Uses a store to implement the page pool interface MPagePool.

Pages can be reclaimable (tracked by the page pool, so that they can be freed 
when required) or not (in which case they must be deleted explicitly): this 
is indicated by a flag of type TPageReclamation. Non-reclaimable pages each 
have their own stream in the store; reclaimable pages are bundled 15 to a 
stream. To track the reclaimable pages, the page pool has a separate token, 
type TStorePagePoolToken, that must be saved by the user of the pool.

The store used must support CStreamStore::ExtendL(), CStreamStore::DeleteL() 
and allow streams to be re-written. CPermanentFileStore meets these conditions.

A store page pool uses a cache to store pages in-memory and to cache frequently 
accessed pages. You should provide a cache object (CPageCache) to the pool 
for this purpose.

@see CPageCache
@see CPermanentFileStore
@see CStreamStore
@see TPageReclamation  
*/
class RStorePagePool : public TCachePagePool
	{
	friend class StorePagePool;
public:
	IMPORT_C RStorePagePool();
	IMPORT_C RStorePagePool(CPageCache& aCache);
	IMPORT_C RStorePagePool(CStreamStore& aStore);
	IMPORT_C RStorePagePool(CStreamStore& aStore,const TStorePagePoolToken& aToken);
	IMPORT_C void Create(CStreamStore& aStore);
	IMPORT_C void Open(CStreamStore& aStore,const TStorePagePoolToken& aToken);
	IMPORT_C TStorePagePoolToken Token() const;
	IMPORT_C void Close();
	inline void Release();
//
	inline TBool IsDirty() const;
	inline void MarkCurrent();
	inline void MarkDirty();
//
	inline TBool HasAvailable() const;
	inline void Discard();
//
	inline TBool IsEmpty() const;
	IMPORT_C TBool ReclaimL();
	IMPORT_C void ReclaimAllL();
protected:
	IMPORT_C TPageRef ExtendL(const TAny* aPage,TPageReclamation aReclamation);
	IMPORT_C void WriteL(TPageRef aRef,const TAny* aPage,TPageChange aChange);
	IMPORT_C void ReadL(TPageRef aRef,TAny* aPage);
	IMPORT_C void DoDeleteL(TPageRef aRef);
private:
	inline void CacheDeleteL(TPageRef aRef);
private:
	CStreamStore* iStore;
	TStreamId iHead;
	TPageRef iAvail;
	TBool iDirty;
	};

/**
 * @publishedAll 
 * @released
 * Interface for incrementally reclaiming or compacting space in a stream store. 
The interface allows these actions to be performed in small steps, so that 
applications can remain responsive while doing these potentially long-running 
tasks.

An instance of a class derived from this interface is returned by 
StreamStore::DoReclaimL() and DoCompactL(). Each step is carried out in 
response to a call of DoNextL() and the object is released on completion of 
the last step.

Notes:

One-step reclaim using CStreamStore::ReclaimL() is actually implemented in 
terms of the incremental collector.

A CStreamStore implementation will only need to implement a collector class 
if it supports reclamation or compaction.  
*/
class MIncrementalCollector
	{
public:
	inline void Close();
	inline void Release();
//
	inline void ResetL(TInt& aCount);
	inline void NextL(TInt& aStep,TInt& aTotal);
	inline void NextL(TPckgBuf<TInt>& aStep,TRequestStatus& aStatus,TPckgBuf<TInt>& aTotal);
protected:
	/** Protected constructor. Protecting the constructor ensures that this 
	abstract class cannot be instantiated.
	
	MIncrementalCollector(const MIncrementalCollector&);
	
	MIncrementalCollector& operator=(const MIncrementalCollector&);
	
	Private copy constructor and copy assignment to prevent */
	MIncrementalCollector() {}
private:
	/** Protected constructor. Protecting the constructor ensures that this 
	abstract class cannot be instantiated.
	
	MIncrementalCollector(const MIncrementalCollector&);
	
	MIncrementalCollector& operator=(const MIncrementalCollector&);
	
	Private copy constructor and copy assignment to prevent */
	MIncrementalCollector(const MIncrementalCollector&);
	MIncrementalCollector& operator=(const MIncrementalCollector&);
//
	virtual IMPORT_C void DoRelease();
	
	/** Implementation of the public ResetL() function. This signals that the 
	client wants to start or retsart the operation from the beginning. A new 
	progress count should be returned in aCount.
	
	@param aCount On return, contains a progress count for the 
	reclamation/compaction 	process. */
	virtual void DoResetL(TInt& aCount)=0;

	/** Implementation of the public synchronous NextL() function. The next 
	step in the reclamation should be done, reporting progress in aStep and 
	aTotal.
	
	@param aStep The progress value from either the last NextL() increment of 
	from ResetL() if the reclamation/compaction was restarted. On return, 
	should contain the new progress value, which can be used in subsequent 
	calls to NextL(). This must be equal to, or less than, the previous 
	value — a zero value must be used to indicate that the operation is 
	complete.
	@param aTotal On return, should contain the total amount of free space in 
	the store. */
	virtual void DoNextL(TInt& aStep,TInt& aTotal)=0;
	virtual IMPORT_C void DoNextL(TPckgBuf<TInt>& aStep,TRequestStatus& aStatus,TPckgBuf<TInt>& aTotal);
	};

#include <s32stor.inl>
#endif
