/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/

#include "MemSpyDriverInspectedProcess.h"

// System includes
#include <kern_priv.h>
#include <nk_trace.h>
#include <u32hal.h>

#ifdef __MARM__
#include "kdebug.h"
#endif //__MARM__

// User includes
#include "MemSpyDriverUtils.h"
#include "MemSpyDriverDevice.h"
#include "MemSpyDriverOSAdaption.h"
#include "MemSpyDriverInspectedProcessManager.h"

// Constants
_LIT8( KMemSpyLitDollarHeap, "$HEAP" );
_LIT8( KMemSpyLitDollarDllData, "DLL$DATA" );
_LIT8( KMemSpyLitDollarDat, "$DAT" );
_LIT( KMemSpyInspectedProcessMutexName, "MemSpyInspectedProcess_0x" );


DMemSpyInspectedProcess::DMemSpyInspectedProcess( DMemSpyDriverDevice& aDevice )
:   iDevice( aDevice )
	{
	}


DMemSpyInspectedProcess::~DMemSpyInspectedProcess()
	{
    TRACE( Kern::Printf("DMemSpyInspectedProcess::~DMemSpyInspectedProcess() - START - this: 0x%08x, %O", this, iProcess ));
	EventMonitor().RequestEventsCancel( *this );

    TRACE( Kern::Printf("DMemSpyInspectedProcess::~DMemSpyInspectedProcess() - calling NotifyOnChangeCancel..." ) );
    NotifyOnChangeCancel();

    TRACE( Kern::Printf("DMemSpyInspectedProcess::~DMemSpyInspectedProcess() - calling ResetTrackedList..." ) );
    ResetTrackedList();

    TRACE( Kern::Printf("DMemSpyInspectedProcess::~DMemSpyInspectedProcess() - calling ResetPendingChanges..." ) );
    ResetPendingChanges();

	if  ( iLock )
		{
        TRACE( Kern::Printf("DMemSpyInspectedProcess::~DMemSpyInspectedProcess() - closing mutex..." ) );
		iLock->Close(NULL);
		}

    if  ( iProcess )
        {
        TRACE( Kern::Printf("DMemSpyInspectedProcess::~DMemSpyInspectedProcess() - closing process..." ) );
	    Kern::SafeClose( (DObject*&) iProcess, NULL );
        TRACE( Kern::Printf("DMemSpyInspectedProcess::~DMemSpyInspectedProcess() - closed process!" ) );
        }

    TRACE( Kern::Printf("DMemSpyInspectedProcess::~DMemSpyInspectedProcess() - END - this: 0x%08x", this ) );
	}


TInt DMemSpyInspectedProcess::Open( DProcess* aProcess )
    {
	__ASSERT_CRITICAL;
    TRACE( Kern::Printf("DMemSpyInspectedProcess::Open() - START - this: 0x%08x, aProcess: 0x%08x (%O)", this, aProcess, aProcess ));

    TInt error = KErrNone;
    //
    iProcess = aProcess;
    error = iProcess->Open();
    //
    if  ( error == KErrNone )
        {
        iProcessId = iDevice.OSAdaption().DProcess().GetId( *aProcess );

        // Create mutex
        TName name( KMemSpyInspectedProcessMutexName );
        name.AppendNumFixedWidth( (TUint) this, EHex, 8 );
        error = Kern::MutexCreate( iLock, name, KMutexOrdNone );
        //
        if  ( error == KErrNone )
            {
            // Get size of all stacks (user & supervsior) for process. Also
            // updates iUserThreadStackSize with the current size of just
            // the user-side thread stacks.
            iInfoCurrent.iMemoryStack = StackSize( *aProcess );
        
            // Request events
            EventMonitor().RequestEvents( *this );
        
            // Find initial chunks that are mapped into process
            FindChunks( *aProcess );
        
            // Indicate that we have data waiting for client. This will
            // cause the client's RS to be completed as soon as it 
            // registers with us...
            UpdateStatistics();
            CompleteClientsRequest( KErrNone, &iInfoCurrent );
            }
        }
    //
    TRACE( Kern::Printf("DMemSpyInspectedProcess::Open() - END - this: 0x%08x, error: %d", this, error ));
    return error;
    }








TInt DMemSpyInspectedProcess::NotifyOnChange( DThread* aThread, TRequestStatus* aRequestStatus, TMemSpyDriverProcessInspectionInfo* aInfo )
    {
	Lock();

    TInt err = KErrInUse;
    const TBool notificationQueued = NotifyOnChangeQueued();
    TRACE( Kern::Printf("DMemSpyInspectedProcess::NotifyOnChange() - START - this: 0x%08x, iAmDead: %d, aRequestStatus: 0x%08x, notificationQueued: %d, iChangeObserverThread: 0x%08x (%O)", this, iAmDead, aRequestStatus, notificationQueued, iChangeObserverThread, iChangeObserverThread ) );
    //
    if  ( notificationQueued == EFalse )
        {
        TRACE( Kern::Printf("DMemSpyInspectedProcess::NotifyOnChange() - Saving client RS..." ) );
        iChangeObserverThread = aThread;
        iChangeObserverRS = aRequestStatus;
        iChangeObserverInfo = aInfo;
        
        // Whilst we still have items in the buffer, we let the client drain them fully.
        // However, if the process is now marked as dead and the buffer is exhausted,
        // we indicate this via KErrDied completion which will cause the client to
        // stop requesting any more changes.
        if	( !iPendingChanges.IsEmpty() )
			{
            TRACE( Kern::Printf("DMemSpyInspectedProcess::NotifyOnChange() - Have buffered changes - SENDING TO CLIENT IMMEDIATELY..." ) );
			
            // We have something in the pending buffer so we can
			// give it back to the client immediately.
	        TMemSpyTrackedChunkChangeInfo* cachedChange = _LOFF( iPendingChanges.First(), TMemSpyTrackedChunkChangeInfo, iLink );
			cachedChange->iLink.Deque();
			
            // Notify about change			
			CompleteClientsRequest( KErrNone, &cachedChange->iInfo );
			
			// Discard cached entry
			delete cachedChange;
			}
        else if ( iAmDead )
            {
            // We must stop listening outside of an event monitor callback...
    	    EventMonitor().RequestEventsCancel( *this );
			CompleteClientsRequest( KErrDied );
            }
        //
		err = KErrNone;
        }
	//
    TRACE( Kern::Printf("DMemSpyInspectedProcess::NotifyOnChange() - END - this: 0x%08x, err: %d", this, err ) );

	Unlock();
    return err;
    }


TInt DMemSpyInspectedProcess::NotifyOnChangeCancel()
    {
	Lock();
    TRACE( Kern::Printf("DMemSpyInspectedProcess::NotifyOnChangeCancel() - START - this: 0x%08x, queued: %d, iChangeObserverThread: 0x%08x, iChangeObserverRS: 0x%08x", this, NotifyOnChangeQueued(), iChangeObserverThread, iChangeObserverRS ) );
    //
    if  ( NotifyOnChangeQueued() )
        {
        TRACE( Kern::Printf( "DMemSpyInspectedProcess::NotifyOnChangeCancel() - this: 0x%08x, iChangeObserverRS: 0x%08x, iProcessId: %d (0x%04x)", this, iChangeObserverRS, iProcessId, iProcessId ) );
		Kern::RequestComplete( iChangeObserverThread, iChangeObserverRS, KErrCancel );
        iChangeObserverThread = NULL;
        iChangeObserverRS = NULL;
        iChangeObserverInfo = NULL;
        }
	//
    TRACE( Kern::Printf("DMemSpyInspectedProcess::NotifyOnChangeCancel() - END - this: 0x%08x", this ) );
	Unlock();

    return KErrNone;
    }


TBool DMemSpyInspectedProcess::NotifyOnChangeQueued() const
    {
    TRACE( Kern::Printf("DMemSpyInspectedProcess::NotifyOnChangeQueued() - START - this: 0x%08x", this ) );
    //
	Lock();
    const TBool queued = ( iChangeObserverRS != NULL );
	Unlock();
    //
    TRACE( Kern::Printf("DMemSpyInspectedProcess::NotifyOnChangeQueued() - END - this: 0x%08x, queued: %d", this, queued ) );
    return queued;
    }









void DMemSpyInspectedProcess::CompleteClientsRequest( TInt aCompletionCode, TMemSpyDriverProcessInspectionInfo* aInfo )
    {
    const TBool notificationQueued = NotifyOnChangeQueued();
    TRACE( Kern::Printf( "DMemSpyInspectedProcess::CompleteClientsRequest() - START - this: 0x%08x, iChangeObserverThread: 0x%08x, iChangeObserverRS: 0x%08x, iChangeObserverInfo: 0x%08x, notificationQueued: %d", this, iChangeObserverThread, iChangeObserverRS, iChangeObserverInfo, notificationQueued ) );
    TRACE( Kern::Printf( "DMemSpyInspectedProcess::CompleteClientsRequest() - iAmDead: %d, buffer is empty: %d, aCompletionCode: %d, iProcessId: %d (0x%04x), aInfo: 0x%08x, iChangeDeliveryCounter: %d", iAmDead, iPendingChanges.IsEmpty(), aCompletionCode, iProcessId, iProcessId, aInfo, iChangeDeliveryCounter ) );

    if  ( notificationQueued )
        {
        ++iChangeDeliveryCounter;
        TInt completionCode = aCompletionCode;
        
        // Write them to client...
        if ( aInfo != NULL )
            {
            const TInt writeErr = Kern::ThreadRawWrite( iChangeObserverThread, iChangeObserverInfo, aInfo, sizeof(TMemSpyDriverProcessInspectionInfo) );
    		if  ( writeErr != KErrNone )
    		    {
                completionCode = writeErr;
    		    }
            }

        // Complete client's async request
        DThread* changeThread = iChangeObserverThread;
        TRequestStatus* changeRS = iChangeObserverRS;

        // Zero these out first to avoid race condition
        iChangeObserverThread = NULL;
        iChangeObserverRS = NULL;
        iChangeObserverInfo = NULL;

        // ...and then tell client.
        TRACE( Kern::Printf( "DMemSpyInspectedProcess::CompleteClientsRequest() - SENDING CHANGE [%4d] TO CLIENT [err: %d]", iChangeDeliveryCounter, completionCode ) );
		Kern::RequestComplete( changeThread, changeRS, completionCode );
        }
	else if ( aInfo )
		{
		// Buffer the change for next time around...
        TRACE( Kern::Printf("DMemSpyInspectedProcess::CompleteClientsRequest() - BUFFERING change event whilst client is AWOL...", this ) );
        NKern::ThreadEnterCS();
        
        TMemSpyTrackedChunkChangeInfo* changeInfo = new TMemSpyTrackedChunkChangeInfo();
        if ( changeInfo )
            {
            changeInfo->iInfo = *aInfo;
            iPendingChanges.Add( &changeInfo->iLink );
            }
        //
        NKern::ThreadLeaveCS();
		}

    TRACE( Kern::Printf("DMemSpyInspectedProcess::CompleteClientsRequest() - END - this: 0x%08x", this ) );
    }












void DMemSpyInspectedProcess::ResetTrackedList()
    {
    TRACE( Kern::Printf("DMemSpyInspectedProcess::ResetTrackedList() - START - this: 0x%08x", this ) );
    NKern::ThreadEnterCS();
    //
    SDblQueLink* link = iTrackedChunks.GetFirst();
	while( link )
		{
		TMemSpyTrackedChunk* object = _LOFF( link, TMemSpyTrackedChunk, iLink );
        delete object;
        link = iTrackedChunks.GetFirst();
		}
    //
    NKern::ThreadLeaveCS();
    TRACE( Kern::Printf("DMemSpyInspectedProcess::ResetTrackedList() - END - this: 0x%08x", this ) );
    }


void DMemSpyInspectedProcess::SetTrackedListUnused()
    {
    TRACE( Kern::Printf("DMemSpyInspectedProcess::SetTrackedListUnused() - START - this: 0x%08x", this ) );
	SDblQueLink* anchor = &iTrackedChunks.iA;
	for (SDblQueLink* link = iTrackedChunks.First(); link != anchor; link = link->iNext)
		{
		TMemSpyTrackedChunk* trackedChunk = _LOFF( link, TMemSpyTrackedChunk, iLink );
        trackedChunk->SetUnused( ETrue );
		}
    TRACE( Kern::Printf("DMemSpyInspectedProcess::SetTrackedListUnused() - END - this: 0x%08x", this ) );
    }


void DMemSpyInspectedProcess::SetTrackedListUnusedStatusByType( TMemSpyTrackedChunk::TType aType, TBool aUnusedStatus )
    {
    TRACE( Kern::Printf("DMemSpyInspectedProcess::SetTrackedListUnusedStatusByType() - START - this: 0x%08x", this ) );
	SDblQueLink* anchor = &iTrackedChunks.iA;
	for (SDblQueLink* link = iTrackedChunks.First(); link != anchor; link = link->iNext)
		{
		TMemSpyTrackedChunk* trackedChunk = _LOFF( link, TMemSpyTrackedChunk, iLink );
		if ( trackedChunk->Type() == aType )
		    {
            trackedChunk->SetUnused( aUnusedStatus );
		    }
		}
    TRACE( Kern::Printf("DMemSpyInspectedProcess::SetTrackedListUnusedStatusByType() - END - this: 0x%08x", this ) );
    }


void DMemSpyInspectedProcess::DiscardUnusedTrackListItems()
    {
    TRACE( Kern::Printf("DMemSpyInspectedProcess::DiscardUnusedTrackListItems() - START - this: 0x%08x", this ) );
    NKern::ThreadEnterCS();
	const SDblQueLink* const anchor = &iTrackedChunks.iA;
    //	
	SDblQueLink* link = iTrackedChunks.First();
	while( link && link != anchor )
		{
		TMemSpyTrackedChunk* trackedChunk = _LOFF( link, TMemSpyTrackedChunk, iLink );
        const TBool unused = trackedChunk->iUnused;
        //
        link = link->iNext;
        //
        if  ( unused )
            {
    		trackedChunk->iLink.Deque();
            delete trackedChunk;
            }
        else
            {
            trackedChunk->SetUnused( EFalse );
            }
		}
    //
    NKern::ThreadLeaveCS();
    TRACE( Kern::Printf("DMemSpyInspectedProcess::DiscardUnusedTrackListItems() - END - this: 0x%08x", this ) );
    }


void DMemSpyInspectedProcess::AddTrackedChunk( DChunk* aChunk, TMemSpyTrackedChunk::TType aType )
    {
    TRACE( Kern::Printf("DMemSpyInspectedProcess::AddTrackedChunk() - START - this: 0x%08x, aChunk: 0x%08x (%O)", this, aChunk, aChunk ) );
    NKern::ThreadEnterCS();
    //
    TMemSpyTrackedChunk* wrapper = new TMemSpyTrackedChunk( aChunk, aType );
    if ( wrapper )
        {
        DMemSpyDriverOSAdaptionDChunk& chunkAdaption = iDevice.OSAdaption().DChunk();
        const TInt cSize = chunkAdaption.GetSize( *aChunk );
        wrapper->SetSize( cSize );
        iTrackedChunks.Add( &wrapper->iLink );
        }
    //
    NKern::ThreadLeaveCS();
    TRACE( Kern::Printf("DMemSpyInspectedProcess::AddTrackedChunk() - END - this: 0x%08x", this ) );
    }


TMemSpyTrackedChunk* DMemSpyInspectedProcess::TrackedChunkByHandle( TAny* aHandle )
    {
    TRACE( Kern::Printf("DMemSpyInspectedProcess::TrackedChunkByHandle() - START - this: 0x%08x", this ) );
    TMemSpyTrackedChunk* ret = NULL;
    //
	const SDblQueLink* const anchor = &iTrackedChunks.iA;
	for (SDblQueLink* link = iTrackedChunks.First(); link != anchor; link = link->iNext)
		{
		TMemSpyTrackedChunk* const trackedChunk = _LOFF( link, TMemSpyTrackedChunk, iLink );

		if  ( trackedChunk && trackedChunk->Handle() == aHandle )
			{
			ret = trackedChunk;
            break;
			}
		}
    //
    TRACE( Kern::Printf("DMemSpyInspectedProcess::TrackedChunkByHandle() - END - this: 0x%08x, entry: 0x%08x (%O)", this, ret, ret ? ret->iChunk : NULL ) );
	return ret;
    }








    


DMemSpyInspectedProcessManager& DMemSpyInspectedProcess::PManager()
    {
    return iDevice.ProcessInspectionManager();
    }


DMemSpyEventMonitor& DMemSpyInspectedProcess::EventMonitor()
    {
    return iDevice.EventMonitor();
    }







TUint DMemSpyInspectedProcess::EMTypeMask() const
    {
    TUint ret = EMemSpyEventThreadAdd     | EMemSpyEventThreadRemove  | EMemSpyEventThreadKill |
                EMemSpyEventProcessUpdate | EMemSpyEventProcessRemove | 
                EMemSpyEventChunkAdd      | EMemSpyEventChunkUpdate   | EMemSpyEventChunkDelete;

    if  ( iAmDead )
        {
        // Not interested anymore...
        ret = 0;
        }

    return ret;
    }


void DMemSpyInspectedProcess::EMHandleProcessUpdated( DProcess& aProcess )
    {
    const TUint procId = iDevice.OSAdaption().DProcess().GetId( aProcess );
    if  ( procId == iProcessId )
        {
	    Lock();

        TRACE( Kern::Printf("DMemSpyInspectedProcess::EMHandleProcessUpdated() - START - this: 0x%08x, iProcess: 0x%08x (%O)", this, iProcess, iProcess ) );

        // Mark all tracked chunks as dirty whilst we work out
        // what is and isn't mapped into the process
        SetTrackedListUnused();
        SetTrackedListUnusedStatusByType( TMemSpyTrackedChunk::ETypeChunkGlobalData, EFalse /* global data chunks are still in use */ );

        // Locate any suitable chunks, tagging existing entries as
        // 'in use' so that we can easily spot ones which are no longer
        // mapped into the process.
        FindChunks( *iProcess );

        // Throw away and tracked chunks which aren't mapped into the 
        // process anymore.
        DiscardUnusedTrackListItems();

        // Calculate latest statistics...
        const TBool changeDetected = UpdateStatistics();
        if  ( changeDetected )
            {
            // Inform observer about new results.
            CompleteClientsRequest( KErrNone, &iInfoCurrent );
            }

        TRACE( Kern::Printf("DMemSpyInspectedProcess::EMHandleProcessUpdated() - END - this: 0x%08x", this ) );

        Unlock();
        }
    }


void DMemSpyInspectedProcess::EMHandleProcessRemoved( DProcess& aProcess )
    {
    DMemSpyDriverOSAdaptionDProcess& dProcessAdaption = iDevice.OSAdaption().DProcess();
    const TUint pid = dProcessAdaption.GetId( aProcess );

    if  ( pid == iProcessId )
        {
	    Lock();

        TRACE( Kern::Printf("DMemSpyInspectedProcess::EMHandleProcessRemoved() - START - this: 0x%08x", this ) );

        // We will implement a multi phased approach to the process being removed.
        //
        // The first notification we will send will show that the process heap and
        // local chunks have been removed, leaving shared chunk sizes intact.
        //
        // We will then send another change, this time setting everything to zero...
        SetTrackedListUnused();
        SetTrackedListUnusedStatusByType( TMemSpyTrackedChunk::ETypeChunkShared, EFalse /* shared chunks are still in use */ );
        SetTrackedListUnusedStatusByType( TMemSpyTrackedChunk::ETypeChunkGlobalData, EFalse /* global data chunks are still in use */ );
        DiscardUnusedTrackListItems();
        const TBool changeDetected1 = UpdateStatistics();
        if  ( changeDetected1 )
            {
            // Inform observer about new results.
            CompleteClientsRequest( KErrNone, &iInfoCurrent );
            }

        // Now repeat the exercise, this time removing everything.
        ResetTrackedList();
    
        // ... including stack
        iInfoCurrent.iMemoryStack = 0;
        
        const TBool changeDetected2 = UpdateStatistics();
        if  ( changeDetected2 )
            {
            // Inform observer about new results.
            CompleteClientsRequest( KErrNone, &iInfoCurrent );
            }

        // Stop listening to events since we've drained everything now...
        iAmDead = ETrue;

        TRACE( Kern::Printf("DMemSpyInspectedProcess::EMHandleProcessRemoved() - END - this: 0x%08x", this ) );

        Unlock();
        }
    }


void DMemSpyInspectedProcess::EMHandleThreadAdd( DThread& aThread )
    {
    DMemSpyDriverOSAdaptionDThread& dThreadAdaption = iDevice.OSAdaption().DThread();
    DMemSpyDriverOSAdaptionDProcess& dProcessAdaption = iDevice.OSAdaption().DProcess();
    //
    DProcess* owningProcess = dThreadAdaption.GetOwningProcess( aThread );
    TRACE( Kern::Printf("DMemSpyInspectedProcess::EMHandleThreadAdd() - this: 0x%08x, aThread: 0x%08x, owningProcess: 0x%08x", this, &aThread, owningProcess ) );
    if ( owningProcess )
        {
        const TUint owningProcessId = dProcessAdaption.GetId( *owningProcess );
        const TUint myId = dProcessAdaption.GetId( *iProcess );
        //
        if ( myId == owningProcessId )
            {
            EMHandleThreadChanged( aThread );
            }
        }
    }


void DMemSpyInspectedProcess::EMHandleThreadRemoved( DThread& aThread )
    {
    DMemSpyDriverOSAdaptionDThread& dThreadAdaption = iDevice.OSAdaption().DThread();
    DMemSpyDriverOSAdaptionDProcess& dProcessAdaption = iDevice.OSAdaption().DProcess();
    //
    DProcess* owningProcess = dThreadAdaption.GetOwningProcess( aThread );
    TRACE( Kern::Printf("DMemSpyInspectedProcess::EMHandleThreadRemoved() - this: 0x%08x, aThread: 0x%08x, owningProcess: 0x%08x", this, &aThread, owningProcess ) );
    if ( owningProcess )
        {
        const TUint owningProcessId = dProcessAdaption.GetId( *owningProcess );
        const TUint myId = dProcessAdaption.GetId( *iProcess );
        //
        if ( myId == owningProcessId )
            {
            EMHandleThreadChanged( aThread );
            }
        }
    }


void DMemSpyInspectedProcess::EMHandleThreadKilled( DThread& aThread )
    {
    //
    DMemSpyDriverOSAdaptionDThread& dThreadAdaption = iDevice.OSAdaption().DThread();
    DMemSpyDriverOSAdaptionDProcess& dProcessAdaption = iDevice.OSAdaption().DProcess();
    //
    DProcess* owningProcess = dThreadAdaption.GetOwningProcess( aThread );
    TRACE( Kern::Printf("DMemSpyInspectedProcess::EMHandleThreadKilled() - this: 0x%08x, aThread: 0x%08x, owningProcess: 0x%08x", this, &aThread, owningProcess ) );
    if ( owningProcess )
        {
        const TUint owningProcessId = dProcessAdaption.GetId( *owningProcess );
        const TUint myId = dProcessAdaption.GetId( *iProcess );
        //
        if ( myId == owningProcessId )
            {
            EMHandleThreadChanged( aThread );
            }
        }
    }


void DMemSpyInspectedProcess::EMHandleThreadChanged( DThread& /*aThread*/ )
    {
	Lock();

    TRACE( Kern::Printf("DMemSpyInspectedProcess::EMHandleThreadChanged() - START - this: 0x%08x", this ) );

    // This is called when a thread is added, changed, or terminated/killed.
    // We must be careful to only access the members of aThread that still
    // exist as if it is being destroyed, the object may be in an intermediate
    // state.

    // All we are really interested in is recalculating the stack usage
    // for the process... 
    iInfoCurrent.iMemoryStack = StackSize( *iProcess );

    // Always inform observer about new results.
    CompleteClientsRequest( KErrNone, &iInfoCurrent );

    TRACE( Kern::Printf("DMemSpyInspectedProcess::EMHandleThreadChanged() - END - this: 0x%08x", this ) );

    Unlock();
    }


void DMemSpyInspectedProcess::EMHandleChunkAdd( DChunk& aChunk )
    {
	Lock();

    TRACE( Kern::Printf("DMemSpyInspectedProcess::EMHandleChunkAdd() - START - this: 0x%08x, aChunk: 0x%08x (%O)", this, &aChunk, &aChunk ) );

    // Is this chunk related to our process somehow?
    if  ( IsChunkRelevantToOurProcess( aChunk ) )
        {
        TRACE( Kern::Printf("DMemSpyInspectedProcess::EMHandleChunkAdd() - processes match, checking chunk type..." ) );

        const TMemSpyTrackedChunk::TType type = ChunkType( &aChunk );
        TRACE( Kern::Printf("DMemSpyInspectedProcess::EMHandleChunkAdd() - chunkType: %d", type ) );

        if  ( type != TMemSpyTrackedChunk::ETypeNotRelevant )
            {
            // It's a new entry in our process
            TRACE( Kern::Printf("DMemSpyInspectedProcess::EMHandleChunkAdd() - this: 0x%08x, creating new entry for chunk: 0x%08x", this, &aChunk ) );

            AddTrackedChunk( &aChunk, type );
            TRACE( Kern::Printf("DMemSpyInspectedProcess::EMHandleChunkAdd() - added chunk..." ));

            const TBool changeDetected = UpdateStatistics();
            if  ( changeDetected )
                {
                // Inform observer about new results.
                CompleteClientsRequest( KErrNone, &iInfoCurrent );
                }
            }
        }

    TRACE( Kern::Printf("DMemSpyInspectedProcess::EMHandleChunkAdd() - END - this: 0x%08x", this ) );

    Unlock();
    }


void DMemSpyInspectedProcess::EMHandleChunkUpdated( DChunk& aChunk )
    {
	Lock();

    TRACE( Kern::Printf("DMemSpyInspectedProcess::EMHandleChunkUpdated() - START - this: 0x%08x, aChunk: 0x%08x [S: %8d] (%O)", this, &aChunk, aChunk.Size(), &aChunk ) );

    // Is this chunk mapped into our process?
    TMemSpyTrackedChunk* trackedEntry = TrackedChunkByHandle( &aChunk );
    if  ( trackedEntry != NULL )
        {
        const TInt oldSize = trackedEntry->Size();
        const TInt newSize = iDevice.OSAdaption().DChunk().GetSize( aChunk );

        TRACE( Kern::Printf("DMemSpyInspectedProcess::EMHandleChunkUpdated() - was tracked entry [0x%08x, size; %10d, type: %d] vs new size: %d", trackedEntry, oldSize, trackedEntry->Type(), newSize ) );
        
        // If the existing entry had a size of 0 and the new size is non-zero
        // then we may have enough data such that we can validate type information.
        // For example, any secondary heap chunk that is created within the process will
        // most likely have a Local-NNNNNN style name, and this chunk will be created with
        // an initial size of zero.
        //
        // We can only identify it's type once the chunk has been updated with some data
        // that supports vTable verification. Hence the type may fluctuate...
        if  ( oldSize == 0 && newSize > 0 )
            {
            TRACE( Kern::Printf("DMemSpyInspectedProcess::EMHandleChunkUpdated() - checking type again as chunk size was zero..." ) );

            PrintChunkInfo( aChunk );
            const TMemSpyTrackedChunk::TType type = ChunkType( &aChunk );
            if  ( type != trackedEntry->Type() )
                {
                // Type has changed
                TRACE( Kern::Printf("DMemSpyInspectedProcess::EMHandleChunkUpdated() - type has transitioned from: %d to %d", trackedEntry->Type(), type ) );
                trackedEntry->SetType( type );
                }
            }

        // Update our record with new chunk size
        trackedEntry->SetSize( newSize );

        const TBool changeDetected = UpdateStatistics();
        if  ( changeDetected )
            {
            // Inform observer about new results.
            CompleteClientsRequest( KErrNone, &iInfoCurrent );
            }
        }

    TRACE( Kern::Printf("DMemSpyInspectedProcess::EMHandleChunkUpdated() - END - this: 0x%08x", this ) );

    Unlock();
    }


void DMemSpyInspectedProcess::EMHandleChunkDeleted( DChunk& aChunk )
    {
	Lock();

    TRACE( Kern::Printf("DMemSpyInspectedProcess::EMHandleChunkDeleted() - START - this: 0x%08x", this ) );

    // Is this chunk mapped into our process?
    TMemSpyTrackedChunk* trackedEntry = TrackedChunkByHandle( &aChunk );
    if ( trackedEntry != NULL )
        {
        // Delete entry
		trackedEntry->iLink.Deque();
        delete trackedEntry;

        const TBool changeDetected = UpdateStatistics();
        if  ( changeDetected )
            {
            // Inform observer about new results.
            CompleteClientsRequest( KErrNone, &iInfoCurrent );
            }
        }

    TRACE( Kern::Printf("DMemSpyInspectedProcess::EMHandleChunkDeleted() - END - this: 0x%08x", this ) );

    Unlock();
    }


void DMemSpyInspectedProcess::PrintChunkInfo( DChunk& aChunk ) const
    {
    TRACE( Kern::Printf("DMemSpyInspectedProcess::PrintChunkInfo() - iProcess*:           0x%08x", iProcess ) );
    TRACE( Kern::Printf("DMemSpyInspectedProcess::PrintChunkInfo() - iProcess id:         0x%04x", iProcessId ) );
    MemSpyDriverUtils::PrintChunkInfo( aChunk, iDevice.OSAdaption() );
    }


TBool DMemSpyInspectedProcess::IsChunkRelevantToOurProcess( DChunk& aChunk ) const
    {
    TRACE( Kern::Printf("DMemSpyInspectedProcess::IsChunkRelevantToOurProcess() - START - this: 0x%08x, iProcess: 0x%08x (%4d), aChunk: 0x%08x (%O)", this, iProcess, iProcessId, &aChunk, &aChunk) );

    TBool relevant = EFalse;
    DMemSpyDriverOSAdaptionDChunk& chunkAdaption = iDevice.OSAdaption().DChunk();
    //
    PrintChunkInfo( aChunk );
    //
    DProcess* chunkProc = chunkAdaption.GetOwningProcess( aChunk );
    if  ( chunkProc )
        {
        const TUint procId = iDevice.OSAdaption().DProcess().GetId( *chunkProc );
        TRACE( Kern::Printf("DMemSpyInspectedProcess::IsChunkRelevantToOurProcess() - [Owning Process] Comparing chunk pid: 0x%04x with procPid: 0x%04x", this, procId, iProcessId ) );
        relevant = ( procId == iProcessId );
        }
    else if ( chunkAdaption.GetOwner( aChunk ) == iProcess )
        {
        TRACE( Kern::Printf("DMemSpyInspectedProcess::IsChunkRelevantToOurProcess() - [Owner Match]" ) );
        relevant = ETrue;
        }
    else
        {
        const TUint controllingOwnerPid = chunkAdaption.GetControllingOwnerId( aChunk );
        TRACE( Kern::Printf("DMemSpyInspectedProcess::IsChunkRelevantToOurProcess() - [Controlling Owner] Comparing chunk owner: 0x%04x with procPid: 0x%04x", this, controllingOwnerPid, iProcessId ) );
        relevant = ( controllingOwnerPid == iProcessId );
        }

    TRACE( Kern::Printf("DMemSpyInspectedProcess::IsChunkRelevantToOurProcess() - END - this: 0x%08x, relevant: %d", this, relevant ) );
    return relevant;
    }


TMemSpyTrackedChunk::TType DMemSpyInspectedProcess::ChunkType( DObject* aObject ) const
    {
    TRACE( Kern::Printf("DMemSpyInspectedProcess::ChunkType() - START - this: 0x%08x, iProcess: 0x%08x, aObject: 0x%08x (%O)", this, iProcess, aObject, aObject ) );
    TMemSpyTrackedChunk::TType ret = TMemSpyTrackedChunk::ETypeNotRelevant;

    // Firstly, check if it's actually held within the chunk container.
    if  ( aObject )
        {
        DMemSpyDriverOSAdaptionDChunk& chunkAdaption = iDevice.OSAdaption().DChunk();
        const TObjectType objectType = chunkAdaption.GetObjectType( *aObject );
        TRACE( Kern::Printf("DMemSpyInspectedProcess::ChunkType() - objectType: %d vs EChunk (%d)", objectType, EChunk ) );

        if  ( objectType == EChunk )
            {
            DChunk* chunk = (DChunk*) aObject;
            //
            if  ( IsChunkRelevantToOurProcess( *chunk ) )
                {
                TName name;
                TRACE( Kern::Printf("DMemSpyInspectedProcess::ChunkType() - getting chunk name..." ) );
                chunk->Name( name );
                TRACE( Kern::Printf("DMemSpyInspectedProcess::ChunkType() - name: %S", &name ) );

                // It is definitely owned by this process. That means that
                // it could be a heap, or then it's a just some other kind
                // of data chunk which our process happens to have created.
                //
                // The main thread within a process results in the creation
                // of a heap called $HEAP, but that isn't the case for other
                // secondary (etc) threads.
                //
                // Only way I can think to identify these is via vTables.
                if  ( name == KMemSpyLitDollarDllData )
                    {
                    // This chunk contains Dll Global Data for the process
                    TRACE( Kern::Printf("DMemSpyInspectedProcess::ChunkType() - TMemSpyTrackedChunk::ETypeChunkGlobalData" ) );
                    ret = TMemSpyTrackedChunk::ETypeChunkGlobalData;
                    }
                else if ( name == KMemSpyLitDollarDat )
                    {
                    // This chunk contains process global data as well as user-side stacks for
                    // the process. However, we calculate the stacks independently, so we must 
                    // adjust this later on to remove stack overhead.
                    TRACE( Kern::Printf("DMemSpyInspectedProcess::ChunkType() - TMemSpyTrackedChunk::ETypeChunkProcessGlobalDataAndUserStack" ) );
                    ret = TMemSpyTrackedChunk::ETypeChunkProcessGlobalDataAndUserStack;
                    }
                else if ( IsHeapChunk( *chunk, name ) )
                    {
                    TRACE( Kern::Printf("DMemSpyInspectedProcess::ChunkType() - TMemSpyTrackedChunk::ETypeChunkHeap" ) );
                    ret = TMemSpyTrackedChunk::ETypeChunkHeap;
                    }
                else
                    {
                    TRACE( Kern::Printf("DMemSpyInspectedProcess::ChunkType() - TMemSpyTrackedChunk::ETypeChunkLocal" ) );
                    ret = TMemSpyTrackedChunk::ETypeChunkLocal;
                    }
                }
            else
                {
                // It's a chunk that is mapped into our process,
                // but isn't owned by us. Therefore it is shared by some
                // other process.
                TRACE( Kern::Printf("DMemSpyInspectedProcess::ChunkType() - TMemSpyTrackedChunk::ETypeChunkShared" ) );
                ret = TMemSpyTrackedChunk::ETypeChunkShared;
                }
            }
        }

    TRACE( Kern::Printf("DMemSpyInspectedProcess::ChunkType() - END - this: 0x%08x, ret: %d", this, ret ) );
    return ret;
    }


TBool DMemSpyInspectedProcess::IsHeapChunk( DChunk& aChunk, const TName& aName ) const
    {
    const TUint rHeapVTable = iDevice.RHeapVTable();
    TRACE( Kern::Printf("DMemSpyInspectedProcess::IsHeapChunk() - START - this: 0x%08x, aChunk: 0x%08x, RHeapVTable: 0x%08x, iProcess: 0x%08x, aName: %S, (%O)", this, &aChunk, rHeapVTable, iProcess, &aName, &aChunk ) );
    
    // The first 4 bytes of every chunk correspond to the allocator VTable (For heap chunks).
    // If it matches RHeap's vtable, we'll treat it as a heap.
    TBool isHeap = EFalse;

    DMemSpyDriverOSAdaptionDChunk& chunkAdaption = iDevice.OSAdaption().DChunk();
    TUint8* base = chunkAdaption.GetBase( aChunk );
    const TInt size = chunkAdaption.GetSize( aChunk );
    TRACE( Kern::Printf("DMemSpyInspectedProcess::IsHeapChunk() - base: 0x%08x, size: %d", base, size ) );

    if  ( iProcess && size >= 4 )
        {
        // Chunks are mapped into entire process so any thread within the process is enough...
        DThread* firstThread = iProcess->FirstThread();
        TRACE( Kern::Printf("DMemSpyInspectedProcess::IsHeapChunk() - firstThread: 0x%08x (%O)", firstThread, firstThread ) );
        if  ( firstThread != NULL )
            {
			NKern::ThreadEnterCS();
            TInt err = firstThread->Open();
            TRACE( Kern::Printf("DMemSpyInspectedProcess::IsHeapChunk() - firstThread open result: %d", err ) );

            if  ( err == KErrNone )
                {
                TBuf8<4> allocatorVTableBuffer;
                err = Kern::ThreadRawRead( firstThread, base, (TUint8*) allocatorVTableBuffer.Ptr(), allocatorVTableBuffer.MaxLength() );
                TRACE( Kern::Printf("DMemSpyInspectedProcess::IsHeapChunk - read result of vtable data from requested thread is: %d", err ));
                //
                if  ( err == KErrNone )
                    {
                    TRACE( MemSpyDriverUtils::DataDump("possible chunk vtable data - %lS", allocatorVTableBuffer.Ptr(), allocatorVTableBuffer.MaxLength(), allocatorVTableBuffer.MaxLength() ) );
                    allocatorVTableBuffer.SetLength( allocatorVTableBuffer.MaxLength() );
                    
                    const TUint32 vtable =   allocatorVTableBuffer[0] +
                                            (allocatorVTableBuffer[1] << 8) + 
                                            (allocatorVTableBuffer[2] << 16) + 
                                            (allocatorVTableBuffer[3] << 24);
                    TRACE( Kern::Printf("DMemSpyInspectedProcess::IsHeapChunk - [possible] vTable within chunk is: 0x%08x", vtable) );

                    // Check the v-table to work out if it really is an RHeap
                    isHeap = ( vtable == rHeapVTable );
                    TRACE( Kern::Printf("DMemSpyInspectedProcess::IsHeapChunk() - isHeap: %d", isHeap ) );
                    }

                TRACE( Kern::Printf("DMemSpyInspectedProcess::IsHeapChunk() - closing first thread..." ) );
            	Kern::SafeClose( (DObject*&) firstThread, NULL );
                }
			NKern::ThreadLeaveCS();
            }
        }
    //
    if  ( !isHeap && aName == KMemSpyLitDollarHeap )
        {
        TRACE( Kern::Printf("DMemSpyInspectedProcess::IsHeapChunk() - is standard Symbian OS initial heap chunk - $HEAP" ) );
        isHeap = ETrue;
        }
    //
    TRACE( Kern::Printf("DMemSpyInspectedProcess::IsHeapChunk() - END - this: 0x%08x, isHeap: %d", this, isHeap ) );
    return isHeap;
    }


TUint32 DMemSpyInspectedProcess::TotalStatistics( const TMemSpyDriverProcessInspectionInfo& aStats, TBool aIncludeShared )
    {
    TUint32 total = aStats.iMemoryStack + 
                    aStats.iMemoryHeap +
                    aStats.iMemoryChunkLocal + 
                    aStats.iMemoryGlobalData;
    //
    if ( aIncludeShared )
        {
        total += aStats.iMemoryChunkShared;
        }
    //
    return total;
    }


TBool DMemSpyInspectedProcess::IsEqual( const TMemSpyDriverProcessInspectionInfo& aLeft, const TMemSpyDriverProcessInspectionInfo& aRight )
    {
    const TBool equal = ( aLeft.iMemoryStack == aRight.iMemoryStack &&
                          aLeft.iMemoryHeap == aRight.iMemoryHeap &&
                          aLeft.iMemoryChunkLocal == aRight.iMemoryChunkLocal &&
                          aLeft.iMemoryChunkShared == aRight.iMemoryChunkShared &&
                          aLeft.iMemoryGlobalData == aRight.iMemoryGlobalData 
                        );
    return equal;
    }


void DMemSpyInspectedProcess::ResetStatistics( TMemSpyDriverProcessInspectionInfo& aStats )
    {
    TRACE( Kern::Printf("DMemSpyInspectedProcess::ResetStatistics() - START - this: 0x%08x", this ) );
    aStats.iProcessId = iProcessId;
    aStats.iMemoryHeap = 0;
    aStats.iMemoryChunkLocal = 0;
    aStats.iMemoryChunkShared = 0;
    aStats.iMemoryGlobalData = 0;
    aStats.iTime = Kern::SystemTime();
    TRACE( Kern::Printf("DMemSpyInspectedProcess::ResetStatistics() - END - this: 0x%08x", this ) );
    }


TBool DMemSpyInspectedProcess::UpdateStatistics()
    {
    TRACE( Kern::Printf("DMemSpyInspectedProcess::UpdateStatistics() - START - this: 0x%08x, iChangeDeliveryCounter: %04d, iProcess: 0x%08x %O", this, iChangeDeliveryCounter, iProcess, iProcess ) );

    // Preserve last stats so we can identify if something really changed...
    iInfoLast = iInfoCurrent;

    // Reset current stats ready for updating. This doesn't wipe the stack field, since
    // that only changes when some kind of thread event occurs...
    ResetStatistics( iInfoCurrent );
    
    // Go through all tracked chunks and update our stats based upon
    // their current values...
    TRACE( Kern::Printf("DMemSpyInspectedProcess::UpdateStatistics() " ) );
    TRACE( Kern::Printf("DMemSpyInspectedProcess::UpdateStatistics() " ) );
    TRACE( Kern::Printf("DMemSpyInspectedProcess::UpdateStatistics() " ) );
    TRACE( Kern::Printf("DMemSpyInspectedProcess::UpdateStatistics() CHUNK ENTRIES:" ) );
    TRACE( Kern::Printf("DMemSpyInspectedProcess::UpdateStatistics() " ) );
	const SDblQueLink* const anchor = &iTrackedChunks.iA;
	for (SDblQueLink* link = iTrackedChunks.First(); link != anchor; link = link->iNext)
		{
		TMemSpyTrackedChunk* const trackedChunk = _LOFF( link, TMemSpyTrackedChunk, iLink );
        //
        switch( trackedChunk->Type() )
            {
        case TMemSpyTrackedChunk::ETypeChunkHeap:
            iInfoCurrent.iMemoryHeap += trackedChunk->Size();
            TRACE( Kern::Printf("DMemSpyInspectedProcess::UpdateStatistics() - [0x%08x] TMemSpyTrackedChunk::ETypeChunkHeap       - %12d (0x%08x, %O)", trackedChunk, trackedChunk->Size(), trackedChunk->iChunk, trackedChunk->iChunk ) );
            break;
        case TMemSpyTrackedChunk::ETypeChunkLocal:
            iInfoCurrent.iMemoryChunkLocal += trackedChunk->Size();
            TRACE( Kern::Printf("DMemSpyInspectedProcess::UpdateStatistics() - [0x%08x] TMemSpyTrackedChunk::ETypeChunkLocal      - %12d (0x%08x, %O)", trackedChunk, trackedChunk->Size(), trackedChunk->iChunk, trackedChunk->iChunk ) );
            break;
        case TMemSpyTrackedChunk::ETypeChunkShared:
            iInfoCurrent.iMemoryChunkShared += trackedChunk->Size();
            TRACE( Kern::Printf("DMemSpyInspectedProcess::UpdateStatistics() - [0x%08x] TMemSpyTrackedChunk::ETypeChunkShared     - %12d (0x%08x, %O)", trackedChunk, trackedChunk->Size(), trackedChunk->iChunk, trackedChunk->iChunk ) );
            break;
        case TMemSpyTrackedChunk::ETypeChunkGlobalData:
            iInfoCurrent.iMemoryGlobalData += trackedChunk->Size();
            TRACE( Kern::Printf("DMemSpyInspectedProcess::UpdateStatistics() - [0x%08x] TMemSpyTrackedChunk::ETypeChunkGlobalData - %12d (0x%08x, %O)", trackedChunk, trackedChunk->Size(), trackedChunk->iChunk, trackedChunk->iChunk ) );
            break;
        case TMemSpyTrackedChunk::ETypeChunkProcessGlobalDataAndUserStack:
            break;

        default:
        case TMemSpyTrackedChunk::ETypeNotRelevant:
            break;
            }
		}

    const TUint32 totalLastIncShared = TotalStatistics( iInfoLast );
    const TUint32 totalLastExcShared = TotalStatistics( iInfoLast, EFalse );
    TRACE( Kern::Printf("DMemSpyInspectedProcess::UpdateStatistics() " ) );
    TRACE( Kern::Printf("DMemSpyInspectedProcess::UpdateStatistics() " ) );
    TRACE( Kern::Printf("DMemSpyInspectedProcess::UpdateStatistics() LAST:" ) );
    TRACE( Kern::Printf("DMemSpyInspectedProcess::UpdateStatistics() " ) );
    TRACE( Kern::Printf("DMemSpyInspectedProcess::UpdateStatistics() - iInfoLast.iMemoryStack:                   %12d", iInfoLast.iMemoryStack ) );
    TRACE( Kern::Printf("DMemSpyInspectedProcess::UpdateStatistics() - iInfoLast.iMemoryHeap:                    %12d", iInfoLast.iMemoryHeap ) );
    TRACE( Kern::Printf("DMemSpyInspectedProcess::UpdateStatistics() - iInfoLast.iMemoryChunkLocal:              %12d", iInfoLast.iMemoryChunkLocal ) );
    TRACE( Kern::Printf("DMemSpyInspectedProcess::UpdateStatistics() - iInfoLast.iMemoryChunkShared:             %12d", iInfoLast.iMemoryChunkShared ) );
    TRACE( Kern::Printf("DMemSpyInspectedProcess::UpdateStatistics() - iInfoLast.iMemoryGlobalData:              %12d", iInfoLast.iMemoryGlobalData ) );
    TRACE( Kern::Printf("DMemSpyInspectedProcess::UpdateStatistics() - --------------------------------------------------------------------------" ) );
    TRACE( Kern::Printf("DMemSpyInspectedProcess::UpdateStatistics() - iInfoLast total:                          %12d / %12d", totalLastIncShared, totalLastExcShared ) );
    TRACE( Kern::Printf("DMemSpyInspectedProcess::UpdateStatistics() " ) );
    TRACE( Kern::Printf("DMemSpyInspectedProcess::UpdateStatistics() " ) );


    const TUint32 totalCurrentIncShared = TotalStatistics( iInfoCurrent );
    const TUint32 totalCurrentExcShared = TotalStatistics( iInfoCurrent, EFalse );
    TRACE( Kern::Printf("DMemSpyInspectedProcess::UpdateStatistics() CURRENT:" ) );
    TRACE( Kern::Printf("DMemSpyInspectedProcess::UpdateStatistics() " ) );
    TRACE( Kern::Printf("DMemSpyInspectedProcess::UpdateStatistics() - iInfoCurrent.iMemoryStack:                %12d", iInfoCurrent.iMemoryStack ) );
    TRACE( Kern::Printf("DMemSpyInspectedProcess::UpdateStatistics() - iInfoCurrent.iMemoryHeap:                 %12d", iInfoCurrent.iMemoryHeap ) );
    TRACE( Kern::Printf("DMemSpyInspectedProcess::UpdateStatistics() - iInfoCurrent.iMemoryChunkLocal:           %12d", iInfoCurrent.iMemoryChunkLocal ) );
    TRACE( Kern::Printf("DMemSpyInspectedProcess::UpdateStatistics() - iInfoCurrent.iMemoryChunkShared:          %12d", iInfoCurrent.iMemoryChunkShared ) );
    TRACE( Kern::Printf("DMemSpyInspectedProcess::UpdateStatistics() - iInfoCurrent.iMemoryGlobalData:           %12d", iInfoCurrent.iMemoryGlobalData ) );
    TRACE( Kern::Printf("DMemSpyInspectedProcess::UpdateStatistics() - --------------------------------------------------------------------------" ) );
    TRACE( Kern::Printf("DMemSpyInspectedProcess::UpdateStatistics() - iInfoCurrent total:                       %12d / %12d", totalCurrentIncShared, totalCurrentExcShared ) );
    TRACE( Kern::Printf("DMemSpyInspectedProcess::UpdateStatistics() " ) );
    TRACE( Kern::Printf("DMemSpyInspectedProcess::UpdateStatistics() " ) );

    // Update peaks, i.e. how large each individual element reached
    const TUint32 totalPeaks = TotalStatistics( iInfoPeaks );
    iInfoPeaks.iMemoryStack = Max( iInfoPeaks.iMemoryStack, iInfoCurrent.iMemoryStack );
    iInfoPeaks.iMemoryHeap = Max( iInfoPeaks.iMemoryHeap, iInfoCurrent.iMemoryHeap );
    iInfoPeaks.iMemoryChunkLocal = Max( iInfoPeaks.iMemoryChunkLocal, iInfoCurrent.iMemoryChunkLocal );
    iInfoPeaks.iMemoryChunkShared = Max( iInfoPeaks.iMemoryChunkShared, iInfoCurrent.iMemoryChunkShared );
    iInfoPeaks.iMemoryGlobalData = Max( iInfoPeaks.iMemoryGlobalData, iInfoCurrent.iMemoryGlobalData );
    TRACE( Kern::Printf("DMemSpyInspectedProcess::UpdateStatistics() PEAK:" ) );
    TRACE( Kern::Printf("DMemSpyInspectedProcess::UpdateStatistics() " ) );
    TRACE( Kern::Printf("DMemSpyInspectedProcess::UpdateStatistics() - iInfoPeaks.iMemoryStack:                  %12d", iInfoPeaks.iMemoryStack ) );
    TRACE( Kern::Printf("DMemSpyInspectedProcess::UpdateStatistics() - iInfoPeaks.iMemoryHeap:                   %12d", iInfoPeaks.iMemoryHeap ) );
    TRACE( Kern::Printf("DMemSpyInspectedProcess::UpdateStatistics() - iInfoPeaks.iMemoryChunkLocal:             %12d", iInfoPeaks.iMemoryChunkLocal ) );
    TRACE( Kern::Printf("DMemSpyInspectedProcess::UpdateStatistics() - iInfoPeaks.iMemoryChunkShared:            %12d", iInfoPeaks.iMemoryChunkShared ) );
    TRACE( Kern::Printf("DMemSpyInspectedProcess::UpdateStatistics() - iInfoPeaks.iMemoryGlobalData:             %12d", iInfoPeaks.iMemoryGlobalData ) );
    TRACE( Kern::Printf("DMemSpyInspectedProcess::UpdateStatistics() - -----------------------------------------------------" ) );
    TRACE( Kern::Printf("DMemSpyInspectedProcess::UpdateStatistics() - iInfoPeaks total:                         %12d", totalPeaks ) );
    TRACE( Kern::Printf("DMemSpyInspectedProcess::UpdateStatistics() " ) );
    TRACE( Kern::Printf("DMemSpyInspectedProcess::UpdateStatistics() " ) );

    // Update HWM, i.e. the largest total so far.
    const TUint32 totalHWMIncShared = TotalStatistics( iInfoHWMIncShared );
    const TUint32 totalHWMExcShared = TotalStatistics( iInfoHWMIncShared, EFalse );
    if  ( totalCurrentIncShared > totalHWMIncShared )
        {
        iInfoHWMIncShared = iInfoCurrent;
        TRACE( Kern::Printf("DMemSpyInspectedProcess::UpdateStatistics() HWM INC SHARED:" ) );
        TRACE( Kern::Printf("DMemSpyInspectedProcess::UpdateStatistics() " ) );
        TRACE( Kern::Printf("DMemSpyInspectedProcess::UpdateStatistics() - iInfoHWMIncShared.iMemoryStack:           %12d", iInfoHWMIncShared.iMemoryStack ) );
        TRACE( Kern::Printf("DMemSpyInspectedProcess::UpdateStatistics() - iInfoHWMIncShared.iMemoryHeap:            %12d", iInfoHWMIncShared.iMemoryHeap ) );
        TRACE( Kern::Printf("DMemSpyInspectedProcess::UpdateStatistics() - iInfoHWMIncShared.iMemoryChunkLocal:      %12d", iInfoHWMIncShared.iMemoryChunkLocal ) );
        TRACE( Kern::Printf("DMemSpyInspectedProcess::UpdateStatistics() - iInfoHWMIncShared.iMemoryChunkShared:     %12d", iInfoHWMIncShared.iMemoryChunkShared ) );
        TRACE( Kern::Printf("DMemSpyInspectedProcess::UpdateStatistics() - iInfoHWMIncShared.iMemoryGlobalData:      %12d", iInfoHWMIncShared.iMemoryGlobalData ) );
        TRACE( Kern::Printf("DMemSpyInspectedProcess::UpdateStatistics() - -----------------------------------------------------" ) );
        TRACE( Kern::Printf("DMemSpyInspectedProcess::UpdateStatistics() - iInfoHWMIncShared total:                  %12d", totalHWMIncShared ) );
        TRACE( Kern::Printf("DMemSpyInspectedProcess::UpdateStatistics() " ) );
        TRACE( Kern::Printf("DMemSpyInspectedProcess::UpdateStatistics() " ) );
        }
    if  ( totalCurrentExcShared > totalHWMExcShared )
        {
        iInfoHWMExcShared = iInfoCurrent;
        TRACE( Kern::Printf("DMemSpyInspectedProcess::UpdateStatistics() HWM EXC SHARED:" ) );
        TRACE( Kern::Printf("DMemSpyInspectedProcess::UpdateStatistics() " ) );
        TRACE( Kern::Printf("DMemSpyInspectedProcess::UpdateStatistics() - iInfoHWMExcShared.iMemoryStack:           %12d", iInfoHWMExcShared.iMemoryStack ) );
        TRACE( Kern::Printf("DMemSpyInspectedProcess::UpdateStatistics() - iInfoHWMExcShared.iMemoryHeap:            %12d", iInfoHWMExcShared.iMemoryHeap ) );
        TRACE( Kern::Printf("DMemSpyInspectedProcess::UpdateStatistics() - iInfoHWMExcShared.iMemoryChunkLocal:      %12d", iInfoHWMExcShared.iMemoryChunkLocal ) );
        TRACE( Kern::Printf("DMemSpyInspectedProcess::UpdateStatistics() - iInfoHWMExcShared.iMemoryChunkShared:     %12d", iInfoHWMExcShared.iMemoryChunkShared ) );
        TRACE( Kern::Printf("DMemSpyInspectedProcess::UpdateStatistics() - iInfoHWMExcShared.iMemoryGlobalData:      %12d", iInfoHWMExcShared.iMemoryGlobalData ) );
        TRACE( Kern::Printf("DMemSpyInspectedProcess::UpdateStatistics() - -----------------------------------------------------" ) );
        TRACE( Kern::Printf("DMemSpyInspectedProcess::UpdateStatistics() - iInfoHWMExcShared total:                  %12d", totalHWMExcShared ) );
        }

    TRACE( Kern::Printf("DMemSpyInspectedProcess::UpdateStatistics() " ) );
    TRACE( Kern::Printf("DMemSpyInspectedProcess::UpdateStatistics() " ) );

    // Work out if something changed...
    const TBool statsChanged = !IsEqual( iInfoLast, iInfoCurrent );
    if  ( statsChanged )
        {
        TRACE( Kern::Printf("DMemSpyInspectedProcess::UpdateStatistics() - STATS CHANGED!" ) );
        }

    TRACE( Kern::Printf("DMemSpyInspectedProcess::UpdateStatistics() - END - this: 0x%08x, statsChanged: %d, iChangeDeliveryCounter: %04d, iProcess: 0x%08x %O", this, statsChanged, iChangeDeliveryCounter, iProcess, iProcess ) );
    return statsChanged;
    }


void DMemSpyInspectedProcess::FindChunks( DProcess& aProcess )
    {
	__ASSERT_CRITICAL;
    TRACE( Kern::Printf("DMemSpyInspectedProcess::FindChunks() - START - this: 0x%08x", this ) );
  
    DMemSpyDriverOSAdaptionDChunk& chunkAdaption = iDevice.OSAdaption().DChunk();
    DMemSpyDriverOSAdaptionDProcess& processAdaption = iDevice.OSAdaption().DProcess();

    // Iterate through each handle in the process
    if  ( processAdaption.IsHandleIndexValid( aProcess ) )
        {
	    MemSpyObjectIx* processHandles = processAdaption.GetHandles( aProcess );
		
		MemSpyObjectIx_HandleLookupLock();
        const TInt count = processHandles->Count();
		MemSpyObjectIx_HandleLookupUnlock();

        TRACE( Kern::Printf("DMemSpyInspectedProcess::FindChunks() - got: %d handles...", count ) );

	    for( TInt i=0; i<count; i++ )
    	    {
            TRACE( Kern::Printf("DMemSpyInspectedProcess::FindChunks() - checking handle index: %2d", i ) );

    	    // Get a handle from the process container...
            MemSpyObjectIx_HandleLookupLock();
			if (i >= processHandles->Count()) break; // Count may have changed in the meantime
    	    DObject* object = (*processHandles)[ i ];
			if (object && object->Open() != KErrNone) object = NULL;
			MemSpyObjectIx_HandleLookupUnlock();

            const TObjectType objectType = ( object ? chunkAdaption.GetObjectType( *object ) : EObjectTypeAny );
            TRACE( Kern::Printf("DMemSpyInspectedProcess::FindChunks() - object: 0x%08x, type: %2d (%O)", object, objectType, object ) );

            // Is it a chunk that is already mapped into our process?
            // See if we're already aware of this chunk...
            if  ( object != NULL && objectType == EChunk )
                {
                TMemSpyTrackedChunk* existingEntry = TrackedChunkByHandle( object );
                TRACE( Kern::Printf("DMemSpyInspectedProcess::FindChunks() - found a chunk, existing lookup entry: 0x%08x", existingEntry ) );

                if  ( existingEntry != NULL )
                    {
                    const TInt cSize = chunkAdaption.GetSize( *existingEntry->iChunk );
                    TRACE( Kern::Printf("DMemSpyInspectedProcess::FindChunks() - setting existing entry size to: %d", cSize ) );

                    // It must be a chunk then... Update size
                    existingEntry->SetSize( cSize );

                    // This item is in use, i.e. it is not unused
                    existingEntry->SetUnused( EFalse );
                    }
                else
                    {
                    DChunk* chunk = (DChunk*) object;
                    TRACE( Kern::Printf("DMemSpyInspectedProcess::FindChunks() - chunk not known, checking type..." ) );

                    // We have no record of this item so far. Is it really a chunk?
                    const TMemSpyTrackedChunk::TType type = ChunkType( object );
                    TRACE( Kern::Printf("DMemSpyInspectedProcess::FindChunks() - type is: %d", type ) );

                    if  ( type != TMemSpyTrackedChunk::ETypeNotRelevant )
                        {
                        // It's a new entry
                        TRACE( Kern::Printf("DMemSpyInspectedProcess::FindChunks() - this: 0x%08x, creating new entry for chunk: 0x%08x", this, chunk ) );

                        AddTrackedChunk( chunk, type );
                        TRACE( Kern::Printf("DMemSpyInspectedProcess::FindChunks() - added chunk..." ));
                        }
                    }
                }
			if (object) object->Close(NULL);
    	    }
        }

    TRACE( Kern::Printf("DMemSpyInspectedProcess::FindChunks() - END - this: 0x%08x", this ) );
    }
    
    
TInt DMemSpyInspectedProcess::StackSize( DProcess& aProcess )
    {
    TRACE( Kern::Printf("DMemSpyInspectedProcess::StackSize() - START - this: 0x%08x", this ) );
    TInt ret = 0;
    //
    DMemSpyDriverOSAdaptionDThread& dThreadAdaption = iDevice.OSAdaption().DThread();

    iUserThreadStackSize = 0;
    //
	SDblQueLink* pLink = aProcess.iThreadQ.First();
	while(pLink != &aProcess.iThreadQ.iA)
		{
		DThread* pT = _LOFF( pLink, DThread, iProcessLink );
        //
        const TUint32 userStackSize = dThreadAdaption.GetUserStackSize( *pT );
        const TUint32 suprStackSize = dThreadAdaption.GetSupervisorStackSize( *pT );
        TRACE( Kern::Printf("DMemSpyInspectedProcess::StackSize() - thread: 0x%08x, userStack: %8d, suprStack: %8d, total: %8d %O", pT, userStackSize, suprStackSize, userStackSize + suprStackSize, pT ));
        //
        iUserThreadStackSize += userStackSize;
		ret += userStackSize + suprStackSize;
        //
		pLink = pLink->iNext;
		}
	//
    TRACE( Kern::Printf("DMemSpyInspectedProcess::StackSize() - END - this: 0x%08x, total stack size for process: %8d, iUserThreadStackSize: %8d", this, ret, iUserThreadStackSize ) );
	return ret;
    }


void DMemSpyInspectedProcess::ResetPendingChanges()
    {
    TRACE( Kern::Printf("DMemSpyInspectedProcess::ResetPendingChanges() - START - this: 0x%08x", this ) );
    NKern::ThreadEnterCS();
    //
    SDblQueLink* link = iPendingChanges.GetFirst();
	while( link )
		{
        TMemSpyTrackedChunkChangeInfo* cachedChange = _LOFF( link, TMemSpyTrackedChunkChangeInfo, iLink );
        delete cachedChange;
        link = iPendingChanges.GetFirst();
		}
    //
    NKern::ThreadLeaveCS();
    TRACE( Kern::Printf("DMemSpyInspectedProcess::ResetPendingChanges() - END - this: 0x%08x", this ) );
    }


void DMemSpyInspectedProcess::Lock() const
	{
	NKern::ThreadEnterCS();
	Kern::MutexWait(*iLock);
	}

void DMemSpyInspectedProcess::Unlock() const
	{
	Kern::MutexSignal(*iLock);
	NKern::ThreadLeaveCS();
	}










TMemSpyTrackedChunk::TMemSpyTrackedChunk( DChunk* aChunk, TType aType )
:   iChunk( aChunk ), iType( aType ), iSize( 0 ), iUnused( EFalse )
    {
    }


void TMemSpyTrackedChunk::SetSize( TInt aSize )
    {
    iSize = aSize;
    }


void TMemSpyTrackedChunk::SetUnused( TBool aUnused )
    {
    iUnused = aUnused;
    }

