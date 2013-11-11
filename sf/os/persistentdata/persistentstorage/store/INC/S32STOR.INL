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

// Class CStreamStore
inline TStreamId CStreamStore::ExtendL()
/** Generates a new stream within this store, and returns its id. This function 
can be used to create a new stream in advance of being written to.

This function is not supported by the direct file store, CDirectFileStore.

@return The stream id for the newly generated stream.
@see CDirectFileStore */
	{return DoExtendL();}
inline void CStreamStore::CommitL()
/** Commit changes and leaves if unsuccessful.

@see CStreamStore::Commit() */
	{DoCommitL();}
inline void CStreamStore::RevertL()
/** Rolls back the store to its state at the last commit point and leaves if unsuccessful.

The function is not supported by the direct file store CDirectFileStore and 
the non-persistent in-memory store CBufStore.

@see CDirectFileStore
@see CBufStore */
	{DoRevertL();}

// Class CPersistentStore
inline CPersistentStore::CPersistentStore()
//	: iRoot(KNullStreamId)
	{}
inline TStreamId CPersistentStore::Root() const
/** Returns the stream id of the root stream.

@return The id of the root stream. */
	{return iRoot;}
inline void CPersistentStore::SetRootL(TStreamId anId)
/** Sets the specified stream as the root stream.

@param anId The id of the stream which is to be the root stream of this store. */
	{DoSetRootL(anId);}

// Class RStoreReclaim
inline RStoreReclaim::RStoreReclaim()
	: iCol(NULL)
	{}
inline void RStoreReclaim::Close()
/** Stops space reclamation or compaction. All allocated resources are freed.

Notes:

If a cleanup item was placed on the cleanup stack when the RStoreReclaim object 
was prepared for space reclamation or compaction (i.e. by a call to OpenLC() 
or CompactLC()), then this function need not be called explicitly; clean up 
is implicitly done by CleanupStack::PopAndDestroy().

The ResetL() member function can be used to restart abandoned space reclamation 
or compaction activity. */
	{Release();}
inline TInt RStoreReclaim::Available() const
/** Returns the amount of free space currently available within the store. The 
function may be called at any time during space reclamation or compaction.

@return The amount of free space currently available within the store. */
	{return CONST_CAST(TPckgBuf<TInt>&,iAvail)();}

// Class CEmbeddedStore
inline TStreamPos CEmbeddedStore::Position(TStreamId anId)
/** Returns the position of the specified stream, within the host stream.

@param anId The stream id of the stream in the embedded store whose position 
within the host stream is to be returned.
@return The position of the stream with id anId within the host stream. */
	{return TStreamPos(anId.Value());}
inline void CEmbeddedStore::Reattach(MStreamBuf* aHost)
/** Takes ownership of the specified stream buffer. On return from this function, 
the embedded store assumes this to be the host stream buffer.

@param aHost A pointer to the stream buffer over which the embedded store 
is to take ownership. */
	{iHost.Share(aHost);}
inline MStreamBuf* CEmbeddedStore::Host() const
/** Returns a pointer to the stream buffer which is acting as host to this embedded 
store. The embedded store retains ownership of the host stream buffer.

@return A pointer to the stream buffer which is host to this embedded store. */
	{return MUTABLE_CAST(TStreamExchange&,iHost).Host();}
inline TStreamPos CEmbeddedStore::Start() const
/** Returns the start position of this embedded store within the host stream.

@return The position of the this embedded store within the host stream. */
	{return iStart;}

// Class RDictionaryWriteStream
inline RDictionaryWriteStream::RDictionaryWriteStream(const MExternalizer<TStreamRef>& anExter)
	: RWriteStream(anExter)
/** Constructs the object with an externaliser. The store map CStoreMap is an example 
of an externaliser.

@param anExter Specifies an externaliser.
@see CStoreMap */
	{}

// Class TStorePagePoolToken
inline TStorePagePoolToken::TStorePagePoolToken(TEmpty)
	: iHead(KNullStreamId),iAvail(KNullPageRef)
/** Constructor that intialises the TStorePagePoolToken for an empty page pool.

@param Intialises for an empty page pool */
	{}
inline void TStorePagePoolToken::Touch()
/** Sets the available pages to 0. */
	{iAvail=KNullPageRef;}
inline TBool TStorePagePoolToken::HasAvailable() const
/** Tests if there is a free page in the pool.

@return True if there is a free page in the pool, otherwise false */
	{return iAvail!=KNullPageRef;}
inline TBool TStorePagePoolToken::IsEmpty() const
/** Tests if the page pool is empty.

@return True if the page pool is empty, otherwise false. */
	{return iHead==KNullStreamId;}

// Class RStorePagePool
inline void RStorePagePool::Release()
/** Purges the page cache and stops using the stream store. */
	{Purge();iStore=NULL;}
inline TBool RStorePagePool::IsDirty() const
/** Tests if the dirty flag has been set on the page pool.

@return True if the dirty flag has been set, otherwise false */
	{return iDirty;}
inline void RStorePagePool::MarkCurrent()
/** Clears the dirty flag. */
	{iDirty=EFalse;}
inline void RStorePagePool::MarkDirty()	
/** Sets the dirty flag on the pool. */
	{iDirty=ETrue;}
inline TBool RStorePagePool::HasAvailable() const
/** Tests if there is a free page in the pool.

@return True if there is a free page in the pool, otherwise false */
	{return iAvail!=KNullPageRef;}
inline void RStorePagePool::Discard()
/** Invalidates the list of free pages in the reclaimable set.

This should be done if there has been a failure in updating the BTree, as 
this list is possibly corrupt now. The reclaimable set can be deleted using 
ReclaimAllL(), and the BTree then repaired. */
	{iAvail=KNullPageRef;}
inline TBool RStorePagePool::IsEmpty() const
/** Tests if the page pool is empty.

@return True if the page pool is empty, otherwise false */
	{return iHead==KNullStreamId;}

// Class MIncrementalCollector
inline void MIncrementalCollector::Close()
/** Closes the collector after it is no longer required. The object should not 
be referenced after calling this function. The semantics for this function 
and Release() are effectively the same. */
	{DoRelease();}
inline void MIncrementalCollector::Release()
/** Releases the collector after it is no longer required. The object should not 
be referenced after calling this function. The semantics for this function 
and Close() are effectively the same.

@see DoRelease() */
	{DoRelease();}
inline void MIncrementalCollector::ResetL(TInt& aCount)
/** Resets the collector. This leaves the collector in a state ready to start or 
restart reclaiming/compacting. On return aCount contains a value that can 
be used to determine the progress of the operation the same value should 
be passed in to subsequent calls to NextL() until it reaches 0.

@param aCount A progress count for the reclamation/compaction process.
@see DoResetL() */
	{DoResetL(aCount);}
inline void MIncrementalCollector::NextL(TInt& aStep,TInt& aTotal)
/** Does the next step in the reclaim synchronously.

aStep contains a progress value, which decreases every time the function is 
called. aTotal contains the running total of free-space that remains in the 
store.

@param aStep The progress value from either the last NextL() increment of 
from ResetL() if the reclamation/compaction was restarted. On return, 
contains the new progress value, which can be used in subsequent calls to 
NextL(). This will be equal to, or less than, the previous value a zero 
value indicates that the operation is complete.
@param aTotal On return, contains the total amount of free space in the store. 
This should increase every time the function is called.
@see DoNextL() */
	{DoNextL(aStep,aTotal);}
inline void MIncrementalCollector::NextL(TPckgBuf<TInt>& aStep,TRequestStatus& aStatus,TPckgBuf<TInt>& aTotal)
/** Does the next step in the reclaim asynchronously. 

aStep contains a progress value, which decreases every time the function is 
called. aTotal contains the running total of free-space that remains in the 
store. These parameters are asynchronous equivalents of the other NextL() 
overload.

@param aStep The progress value from either the last NextL() increment of 
from ResetL() if the reclamation/compaction was restarted. On return, 
contains the new progress value, which can be used in subsequent calls to 
NextL(). This will be equal to, or less than, the previous value a zero 
value indicates that the operation is complete.
@param aStatus A status variable. KErrNone on successful completion, otherwise 
another of the system-wide error codes.
@param aTotal On return, contains the total amount of free space in the store. 
This should increase every time the function is called.
@see DoNextL() */
	{DoNextL(aStep,aStatus,aTotal);}

