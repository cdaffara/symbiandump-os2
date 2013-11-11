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

#define UNUSED_VAR(a) a = a

LOCAL_C TInt runL(MIncrementalCollector* aCollector)
//
// Collect synchronously until finished.
//
	{
	TInt total=0;
	TInt step=0;
	CleanupReleasePushL(*aCollector);
	aCollector->ResetL(step);
	while (step>0)
		aCollector->NextL(step,total);
	CleanupStack::PopAndDestroy();
	return total;
	}

EXPORT_C void CStreamStore::Delete(TStreamId anId)
/** Deletes the specified stream from this store.

This function is deprecated.

If unsuccessful, the function fails silently with no way to return information 
to the user.

The function is not supported by the direct file store, CDirectFileStore.

@param anId The id of the stream to be deleted. */
	{
	TRAPD(ignore,DeleteL(anId));
    UNUSED_VAR(ignore);
	}

EXPORT_C void CStreamStore::DeleteL(TStreamId anId)
/** Deletes the specified stream from this store, leaving if unsuccessful.

The function is not supported by the direct file store, CDirectFileStore.

@param anId The id of the stream to be deleted from this store.
@see CDirectFileStore */
	{
	if (anId!=KNullStreamId)
		DoDeleteL(anId);
	}

EXPORT_C TInt CStreamStore::Commit()
/** Commits changes.

This function establishes a new commit point. Typically, this is done after 
changes to new or existing streams are complete and the streams themselves 
have been committed.

Establishing a new commit point makes changes to the store permanent. Until 
such changes are committed, they can be rolled back or reverted, effectively 
causing the store to revert back to its state before the changes were made.

This ensures that persistent data moves from one consistent state to another 
and guarantees the integrity of persistent store data in the event of failures. 
In particular, if a process terminates or a media failure occurs, the store 
reverts automatically to its state at the last successful commit point.

Note that this function is not implemented by the direct file store CDirectFileStore 
and the non-persistent in-memory store CBufStore.

@return KErrNone if successful, otherwise another of the system-wide error 
codes.
@see CDirectFileStore
@see CBufStore */
	{
	TRAPD(r,CommitL());
	return r;
	}

EXPORT_C void CStreamStore::Revert()
/** Rolls back the store to its state at the last commit point. 

This function is deprecated; use RevertL() instead.

If unsuccessful, the function fails silently with no way to return information 
to the user.

The function is not supported by the direct file store CDirectFileStore and 
the non-persistent in-memory store CBufStore.

@see CDirectFileStore
@see CBufStore */
	{
	TRAPD(ignore,RevertL());
    UNUSED_VAR(ignore);
	}

EXPORT_C TInt CStreamStore::ReclaimL()
/** Reclaims space within a store, returning the total amount of free space available 
within that store.

The function does not return until the reclamation process is complete. This 
can take an extended amount of time.

The function is only supported by the permanent file store, CPermanentFileStore, 
but not by other derived classes, e.g., CDirectFileStore or CBufStore.

@return The amount of free space available within the store.
@see CPermanentFileStore */
	{
	return runL(DoReclaimL());
	}

EXPORT_C TInt CStreamStore::CompactL()
/** Compacts the store. This returns free space to the appropriate system pool, 
for example, the filing system in the case of file-based stores.

On completion, the function returns the total amount of free space available 
within the store.

The function does not return until the compaction process is complete. This 
can take an extended amount of time.

Note:

this function is only supported by the permanent file store, CPermanentFileStore, 
and not by CDirectFileStore or CBufStore.

Streams must be closed before calling this function.

@return The amount of free space available within the store.
@see CPermanentFileStore */
	{
	return runL(DoCompactL());
	}

EXPORT_C TStreamId CStreamStore::DoExtendL()
/** Generates a new stream within this store, and returns its id. This function 
is intended to create a new stream in advance of being written to.

This is called by ExtendL().

@return The new stream id.
@see CStreamStore::ExtendL() */
	{
	__LEAVE(KErrNotSupported);
	return KNullStreamId;
	}

EXPORT_C void CStreamStore::DoDeleteL(TStreamId)
//
// Default implementation failing.
//
	{
	__LEAVE(KErrNotSupported);
	}

EXPORT_C MStreamBuf* CStreamStore::DoWriteL(TStreamId)
//
// Default implementation failing.
//
	{
	__LEAVE(KErrNotSupported);
	return NULL;
	}

EXPORT_C MStreamBuf* CStreamStore::DoReplaceL(TStreamId)
//
// Default implementation failing.
//
	{
	__LEAVE(KErrNotSupported);
	return NULL;
	}

EXPORT_C void CStreamStore::DoCommitL()
/** Commits any changes to the store. For a store that provides atomic updates, 
this writes all of the pending updates to the to the permanent storage medium. 
After committing the store contains all or none of the updates since the last 
commit/revert.

This function provides the implementation for the public CommitL() function. */
	{}

EXPORT_C void CStreamStore::DoRevertL()
/** Discards any pending changes to the store. This includes all changes which 
have not been committed to a permanent storage medium. 

This function provides the implementation for the public Revert() function.

Note:

The function need only be implemented by stores that provide atomic updates, 
as revert has no meaning for other implementations. */
	{
	__LEAVE(KErrNotSupported);
	}

EXPORT_C MIncrementalCollector* CStreamStore::DoReclaimL()
/** Initialises an object for reclaiming space in the store. This function provides 
the direct implementation for RStoreReclaim::OpenL(). 

Note:

Actually reclaiming the space is done by repeated calls to MIncrementalCollector::Next(), 
before releasing the object.

@return Pointer to an incremental collector, which implements the interface 
for reclaiming store space. */
	{
	__LEAVE(KErrNotSupported);
	return NULL;
	}

EXPORT_C MIncrementalCollector* CStreamStore::DoCompactL()
/** Initialises an object for compacting space in the store. This function provides 
the direct implementation for RStoreReclaim::CompactL(). 

Note:

Actually compacting the space is done by repeated calls to MIncrementalCollector::Next() 
before releasing the object.

@return Pointer to an incremental collector, which implements the interface 
for compacting store space. */
	{
	__LEAVE(KErrNotSupported);
	return NULL;
	}

EXPORT_C void CPersistentStore::DoSetRootL(TStreamId anId)
/** Implements the setting of theroot stream.

This function is called by SetRootL()

@param anId The id of the stream which is to be the root stream of the store.
@see CPersistentStore::SetRootL() */
	{
	iRoot=anId;
	}

EXPORT_C void RStoreReclaim::OpenL(CStreamStore& aStore,TInt& aCount)
/** Prepares the object to perform space reclamation.

@param aStore A reference to the store on which space reclamation or compaction 
is to be performed.
@param aCount A reference to a control value set by these functions. This value 
is required by all variants of Next() and NextL() (and ResetL(), if used). */
	{
	OpenLC(aStore,aCount);
	CleanupStack::Pop();
	}

EXPORT_C void RStoreReclaim::OpenLC(CStreamStore& aStore,TInt& aCount)
/** Prepares the object to perform space reclamation and puts a pointer onto the 
cleanup stack.

Placing a cleanup item for the object onto the cleanup stack allows allocated 
resources to be cleaned up if a subsequent leave occurs.

@param aStore A reference to the store on which space reclamation or compaction 
is to be performed.
@param aCount A reference to a control value set by these functions. This value 
is required by all variants of Next() and NextL() (and ResetL(), if used). */
	{
	iCol=aStore.DoReclaimL();
	CleanupReleasePushL(*this);
	ResetL(aCount);
	}

EXPORT_C void RStoreReclaim::CompactL(CStreamStore& aStore,TInt& aCount)
/** Prepares the object to perform compaction.

Streams must be closed before calling this function.

@param aStore A reference to the store on which space reclamation or compaction 
is to be performed.
@param aCount A reference to a control value set by these functions. This value 
is required by all variants of Next() and NextL() (and ResetL(), if used). */
	{
	CompactLC(aStore,aCount);
	CleanupStack::Pop();
	}

EXPORT_C void RStoreReclaim::CompactLC(CStreamStore& aStore,TInt& aCount)
/** Prepares the object to perform compaction, putting a cleanup item onto the 
cleanup stack. 

P lacing a cleanup item for the object onto the cleanup stack allows allocated 
resources to be cleaned up if a subsequent leave occurs.

Streams must be closed before calling this function.

@param aStore A reference to the store on which space reclamation or compaction 
is to be performed.
@param aCount A reference to a control value set by these functions. This value 
is required by all variants of Next() and NextL() (and ResetL(), if used). */
	{
	iCol=aStore.DoCompactL();
	CleanupReleasePushL(*this);
	ResetL(aCount);
	}

EXPORT_C void RStoreReclaim::Release()
/** Releases allocated resources. Any space reclamation or compaction in progress 
is abandoned.

Notes:

If a cleanup item was placed on the cleanup stack when the RStoreReclaim object 
was prepared for space reclamation or compaction (i.e. by a call to OpenLC() 
or CompactLC()), then this function need not be called explicitly; clean up 
is implicitly done by CleanupStack::PopAndDestroy().

The ResetL() member function can be used to restart abandoned space reclamation 
or compaction activity. */
	{
	if (iCol!=NULL)
		{
		iCol->Release();
		iCol=NULL;
		}
	}

EXPORT_C void RStoreReclaim::ResetL(TInt& aCount)
/** Restarts space reclamation or compaction.

The value in aCount must be:

that which was set by the most recent call to Next() or NextL(), if space 
reclamation or compaction had been started.

that which was set by OpenL(), OpenLC(), CompactL() or CompactLC(), if space 
reclamation or compaction had not been started.

@param aCount A reference to a control value originally set by OpenL(), OpenLC(), 
CompactL() or CompactLC() and updated by subsequent calls to Next() or NextL(). */
	{
	__ASSERT_DEBUG(iCol!=NULL,Panic(EStoreNotOpen));
	iCol->ResetL(aCount);
	iAvail()=0;
	}

EXPORT_C void RStoreReclaim::NextL(TInt& aStep)
/** Performs the next space reclamation or compaction step synchronous, leaves. 
The function updates the value in aStep, and should only be called while aStep 
is non-zero. Once this value is zero, no further calls should be made.

The step is performed synchronously, i.e. the function does not return until 
the step is complete.

@param aStep A reference to a control value originally set by OpenL(), OpenLC(), 
CompactL() or CompactLC() and updated by calls to Next() or NextL(). */
	{
	__ASSERT_DEBUG(iCol!=NULL,Panic(EStoreNotOpen));
	iCol->NextL(aStep,iAvail());
	}

EXPORT_C void RStoreReclaim::Next(TPckgBuf<TInt>& aStep,TRequestStatus& aStatus)
//
// Perform a reclamation step with guaranteed completion.
//
/** Initiates the next space reclamation or compaction step asynchronous, 
non-leaving. The function updates the value in aStep, and should only be called 
while aStep is non-zero. Once this value is zero, no further calls should 
be made.

The step itself is performed asynchronously.

Note:

The RStoreReclaim object should be made part of an active object to simplify 
the handling of the step completion event.

@param aStep A reference to a control value constructed from a TInt value 
originally set by OpenL(), OpenLC(), CompactL() or CompactLC().aStep is updated 
by calls to Next() or NextL().
@param aStatus On completion, contains the request status. If successful contains 
KErrNone. If the function fails during the initiation phase, the failure is 
reported as if the step had started successfully but completed with that error. */
	{
	__ASSERT_DEBUG(iCol!=NULL,Panic(EStoreNotOpen));
	TRAPD(r,iCol->NextL(aStep,aStatus,iAvail));
	if (r!=KErrNone)
		{
		TRequestStatus* stat=&aStatus;
		User::RequestComplete(stat,r);
		}
	}

EXPORT_C void RStoreReclaim::NextL(TPckgBuf<TInt>& aStep,TRequestStatus& aStatus)
/** Initiates the next space reclamation or compaction step asynchronous, 
leaving. The function updates the value in aStep, and should only be called 
while aStep is non-zero. Once this value is zero, no further calls should 
be made.

The step itself is performed asynchronously.

Note:

The RStoreReclaim object should be made part of an active object to simplify 
the handling of the step completion event.

@param aStep A reference to a control value constructed from a TInt value 
originally set by OpenL(), OpenLC(), CompactL() or CompactLC().aStep is updated 
by calls to Next() or NextL().
@param aStatus On completion, contains the request status. If successful contains 
KErrNone. If the function fails during the initiation phase, the failure is 
reported as if the step had started successfully but completed with that error. */
	{
	__ASSERT_DEBUG(iCol!=NULL,Panic(EStoreNotOpen));
	iCol->NextL(aStep,aStatus,iAvail);
	}

EXPORT_C TInt RStoreReclaim::Next(TInt& aStep)
/** Performs the next space reclamation or compaction step synchronous, non-leaving. 
The function updates the value in aStep, and should only be called while aStep 
is non-zero. Once this value is zero, no further calls should be made.

The step is performed synchronously, i.e. the function does not return until 
the step is complete.

@param aStep A reference to a control value originally set by OpenL(), OpenLC(), 
CompactL() or CompactLC() and updated by calls to Next() or NextL().
@return KErrNone if successful, otherwise another of the system-wide error 
codes. */
	{
	__ASSERT_DEBUG(iCol!=NULL,Panic(EStoreNotOpen));
	TRAPD(r,iCol->NextL(aStep,iAvail()));
	return r;
	}

