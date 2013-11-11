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

#include "MemSpyDriverEventMonitor.h"

// System includes
#include <kern_priv.h>
#include <nk_trace.h>

// User includes
#include "MemSpyDriverUtils.h"
#include "MemSpyDriverDevice.h"

#ifdef __MARM__
#include "kdebug.h"
#endif //__MARM__

_LIT( KMemSpyEventMonitorMutexName, "MemSpyEventMonitor");


DMemSpyEventMonitor::DMemSpyEventMonitor()
:	DKernelEventHandler( EventHandler, this )
	{
	}


DMemSpyEventMonitor::~DMemSpyEventMonitor()
	{
    TRACE_EM( Kern::Printf("DMemSpyEventMonitor::~DMemSpyEventMonitor() - START" ));

	if  ( iLock )
		{
        TRACE_EM( Kern::Printf("DMemSpyEventMonitor::~DMemSpyEventMonitor() - closing iLock mutex" ));
		iLock->Close(NULL);
		}

	if  ( iDevice )
		{
        TRACE_EM( Kern::Printf("DMemSpyEventMonitor::~DMemSpyEventMonitor() - closing iDevice" ));
		iDevice->Close(NULL);
		}

    TRACE_EM( Kern::Printf("DMemSpyEventMonitor::~DMemSpyEventMonitor() - END" ));
	}


TInt DMemSpyEventMonitor::Create( DMemSpyDriverDevice* aDevice )
	{
    TRACE_EM( Kern::Printf("DMemSpyEventMonitor::Create() - START - aDevice: 0x%08x", aDevice ));
    TInt err = aDevice->Open();
    TRACE_EM( Kern::Printf("DMemSpyEventMonitor::Create() - device open err: %d", err ));
    //
    if (err == KErrNone )
	    {
        iDevice = aDevice;
        //
        err = Kern::MutexCreate( iLock, KMemSpyEventMonitorMutexName, KMutexOrdNone );
        TRACE_EM( Kern::Printf("DMemSpyEventMonitor::Create() - mutex create error: %d", err ));
        //
        if (!err)
	        {
	        err = Add();
	        }
        }
    //	
    TRACE_EM( Kern::Printf("DMemSpyEventMonitor::Create() - END - err: %d", err ));
	return err;
	}


TInt DMemSpyEventMonitor::Start()
	{
    TRACE_EM( Kern::Printf("DMemSpyEventMonitor::Start() - START"));

	NKern::ThreadEnterCS();
	Kern::MutexWait( *iLock );

    iTracking = ETrue;

	Kern::MutexSignal( *iLock );
	NKern::ThreadLeaveCS();

    TRACE_EM( Kern::Printf("DMemSpyEventMonitor::Start() - END") );
	return KErrNone;
	}


TInt DMemSpyEventMonitor::Stop()
	{
    TRACE_EM( Kern::Printf("DMemSpyEventMonitor::Stop() - START") );

    NKern::ThreadEnterCS();
	Kern::MutexWait( *iLock );

	iTracking = EFalse;

	Kern::MutexSignal( *iLock );
	NKern::ThreadLeaveCS();

    TRACE_EM( Kern::Printf("DMemSpyEventMonitor::Stop() - END") );
    return KErrNone;
    }
    
    
TInt DMemSpyEventMonitor::RequestEvents( MMemSpyEventMonitorObserver& aObserver )
    {
    NKern::ThreadEnterCS();
	Kern::MutexWait( *iLock );

    TInt error = KErrAlreadyExists;
    //
    if  ( IsObserving( aObserver ) == EFalse )
        {
        iObservers.Add( &aObserver.__iEMLink );
        error = KErrNone;
        }
    //
	Kern::MutexSignal( *iLock );
	NKern::ThreadLeaveCS();
    //
    TRACE_EM( Kern::Printf("DMemSpyEventMonitor::RequestEvents() - END - error: %d", error) );
    return error;
    }


TInt DMemSpyEventMonitor::RequestEventsCancel( MMemSpyEventMonitorObserver& aObserver )
    {
    TRACE_EM( Kern::Printf("DMemSpyEventMonitor::RequestEventsCancel() - START - aObserver: 0x%08x", &aObserver) );
    NKern::ThreadEnterCS();
	Kern::MutexWait( *iLock );

    const TBool isObserving = IsObserving( aObserver );
    if  ( isObserving )
        {
        TRACE_EM( Kern::Printf("DMemSpyEventMonitor::RequestEventsCancel() - dequing observer...") );
        aObserver.__iEMLink.Deque();
        TRACE_EM( Kern::Printf("DMemSpyEventMonitor::RequestEventsCancel() - observer dequeued") );
        }

	Kern::MutexSignal( *iLock );
	NKern::ThreadLeaveCS();
    //
    TRACE_EM( Kern::Printf("DMemSpyEventMonitor::RequestEventsCancel() - END") );
    return KErrNone;
    }


TUint DMemSpyEventMonitor::EventHandler( TKernelEvent aType, TAny* a1, TAny* a2, TAny* aSelf )
	{
    DMemSpyEventMonitor* self = (DMemSpyEventMonitor*) aSelf;
	const TUint ret = self->HandleEvent( aType, a1, a2 );
    return ret;
	}


TUint DMemSpyEventMonitor::HandleEvent( TKernelEvent aType, TAny* a1, TAny* /*a2*/ )
	{ 
	// TRACE_EM( Kern::Printf("DMemSpyEventMonitor::HandleEvent() - PRE WAIT"));
	NKern::ThreadEnterCS();
	Kern::MutexWait(*iLock);
	// TRACE_EM( Kern::Printf("DMemSpyEventMonitor::HandleEvent() - POST WAIT"));

	if  ( iTracking )
		{
		switch( aType )
			{
        //////////////////////////////////
        // THREAD HANDLING
        //////////////////////////////////
		case EEventAddThread:
			{
			// TRACE_EM( Kern::Printf("DMemSpyEventMonitor::HandleEvent() - EEventAddThread"));
			EventThreadAdd( (DThread*) a1 );
			break;
			}
		case EEventRemoveThread:
			{
			// TRACE_EM( Kern::Printf("DMemSpyEventMonitor::HandleEvent() - EEventRemoveThread"));
			EventThreadRemoved( (DThread*) a1 );
			break;
			}
		case EEventKillThread:
			{
			// TRACE_EM( Kern::Printf("DMemSpyEventMonitor::HandleEvent() - EEventKillThread"));
			EventThreadKilled( (DThread*) a1 );
			break;
			}

        //////////////////////////////////
        // PROCESS HANDLING
        //////////////////////////////////
	    case EEventAddProcess:
			{
			// TRACE_EM( Kern::Printf("DMemSpyEventMonitor::HandleEvent() - EEventAddProcess"));
			EventProcessAdd( (DProcess*) a1 );
			break;
			}
	    case EEventUpdateProcess:
			{
			// TRACE_EM( Kern::Printf("DMemSpyEventMonitor::HandleEvent() - EEventUpdateProcess"));
			EventProcessUpdate( (DProcess*) a1 );
			break;
			}
	    case EEventRemoveProcess:
			{
			// TRACE_EM( Kern::Printf("DMemSpyEventMonitor::HandleEvent() - EEventRemoveProcess"));
			EventProcessRemoved( (DProcess*) a1 );
			break;
			}
			
        //////////////////////////////////
        // CHUNK HANDLING
        //////////////////////////////////
	    case EEventNewChunk:
			{
			// TRACE_EM( Kern::Printf("DMemSpyEventMonitor::HandleEvent() - EEventNewChunk"));
			EventChunkAdd( (DChunk*) a1 );
			break;
			}
	    case EEventUpdateChunk:
			{
			// TRACE_EM( Kern::Printf("DMemSpyEventMonitor::HandleEvent() - EEventUpdateChunk"));
			EventChunkUpdate( (DChunk*) a1 );
			break;
			}
		case EEventDeleteChunk:
			{
			// TRACE_EM( Kern::Printf("DMemSpyEventMonitor::HandleEvent() - EEventDeleteChunk"));
			EventChunkDelete( (DChunk*) a1 );
			break;
			}

        default:
			break;
			}
		}

	// TRACE_EM( Kern::Printf("DMemSpyEventMonitor::HandleEvent() - PRE SIGNAL "));
	Kern::MutexSignal( *iLock );
	NKern::ThreadLeaveCS();
	// TRACE_EM( Kern::Printf("DMemSpyEventMonitor::HandleEvent() - POST SIGNAL "));

	// Allow other handlers to see this event
	return DKernelEventHandler::ERunNext;
	}


void DMemSpyEventMonitor::EventProcessAdd( DProcess* aProcess )
    {
	TRACE_EM( Kern::Printf("DMemSpyEventMonitor::EventProcessAdd() - aProcess: 0x%08x [%d] (%O)", aProcess, aProcess->iId, aProcess ));
    //
    const SDblQueLink* const anchor = &iObservers.iA;
	for (SDblQueLink* link = iObservers.First(); link != anchor; link = link->iNext)
		{
		MMemSpyEventMonitorObserver* const observer = _LOFF(link, MMemSpyEventMonitorObserver, __iEMLink);
        const TInt typeMask = observer->EMTypeMask();
        //
        if ( typeMask & EMemSpyEventProcessAdd )
			{
            observer->EMHandleProcessAdd( *aProcess );
			}
        }
    }


void DMemSpyEventMonitor::EventProcessUpdate( DProcess* aProcess )
    {
	TRACE_EM( Kern::Printf("DMemSpyEventMonitor::EventProcessUpdate() - aProcess: 0x%08x [%d] (%O)", aProcess, aProcess->iId, aProcess ));
    //
	const SDblQueLink* const anchor = &iObservers.iA;
	for (SDblQueLink* link = iObservers.First(); link != anchor; link = link->iNext)
		{
		MMemSpyEventMonitorObserver* const observer = _LOFF(link, MMemSpyEventMonitorObserver, __iEMLink);
        const TInt typeMask = observer->EMTypeMask();
        //
        if ( typeMask & EMemSpyEventProcessUpdate )
			{
            observer->EMHandleProcessUpdated( *aProcess );
			}
        }
    }


void DMemSpyEventMonitor::EventProcessRemoved( DProcess* aProcess )
    {
	TRACE_EM( Kern::Printf("DMemSpyEventMonitor::EventProcessRemoved() - aProcess: 0x%08x [%d] (%O)", aProcess, aProcess->iId, aProcess ));
    //
	const SDblQueLink* const anchor = &iObservers.iA;
	for (SDblQueLink* link = iObservers.First(); link != anchor; link = link->iNext)
		{
		MMemSpyEventMonitorObserver* const observer = _LOFF(link, MMemSpyEventMonitorObserver, __iEMLink);
        const TInt typeMask = observer->EMTypeMask();
        //
        if ( typeMask & EMemSpyEventProcessRemove )
			{
            observer->EMHandleProcessRemoved( *aProcess );
			}
        }
    }


void DMemSpyEventMonitor::EventThreadAdd( DThread* aThread )
    {
	TRACE_EM( Kern::Printf("DMemSpyEventMonitor::EventThreadAdd() - aThread: 0x%08x [%4d] (%O)", aThread, aThread->iId, aThread ));
    //
	const SDblQueLink* const anchor = &iObservers.iA;
	for (SDblQueLink* link = iObservers.First(); link != anchor; link = link->iNext)
		{
		MMemSpyEventMonitorObserver* const observer = _LOFF(link, MMemSpyEventMonitorObserver, __iEMLink);
        const TInt typeMask = observer->EMTypeMask();
        //
        if ( typeMask & EMemSpyEventThreadAdd )
			{
            observer->EMHandleThreadAdd( *aThread );
			}
        }
    }


void DMemSpyEventMonitor::EventThreadRemoved( DThread* aThread )
    {
	TRACE_EM( Kern::Printf("DMemSpyEventMonitor::EventThreadRemoved() - aThread: 0x%08x [%4d] (%O)", aThread, aThread->iId, aThread ));
    //
	const SDblQueLink* const anchor = &iObservers.iA;
	for (SDblQueLink* link = iObservers.First(); link != anchor; link = link->iNext)
		{
		MMemSpyEventMonitorObserver* const observer = _LOFF(link, MMemSpyEventMonitorObserver, __iEMLink);
        const TInt typeMask = observer->EMTypeMask();
        //
        if ( typeMask & EMemSpyEventThreadRemove )
			{
            observer->EMHandleThreadRemoved( *aThread );
			}
        }
    }


void DMemSpyEventMonitor::EventThreadKilled( DThread* aThread )
    {
	TRACE_EM( Kern::Printf("DMemSpyEventMonitor::EventThreadKilled() - aThread: 0x%08x [%4d] (%O)", aThread, aThread->iId, aThread ));
    //
	const SDblQueLink* const anchor = &iObservers.iA;
	for (SDblQueLink* link = iObservers.First(); link != anchor; link = link->iNext)
		{
		MMemSpyEventMonitorObserver* const observer = _LOFF(link, MMemSpyEventMonitorObserver, __iEMLink);
        const TInt typeMask = observer->EMTypeMask();
        //
        if ( typeMask & EMemSpyEventThreadKill )
			{
            observer->EMHandleThreadKilled( *aThread );
			}
        }
    }


void DMemSpyEventMonitor::EventChunkAdd( DChunk* aChunk )
    {
    TRACE_EM( Kern::Printf("DMemSpyEventMonitor::EventChunkAdd() - aChunk: 0x%08x [%10d] {OP: %4d, CO: %4d} (%O)", aChunk, aChunk->Size(), ( aChunk->iOwningProcess ? aChunk->iOwningProcess->iId : 0 ), aChunk->iControllingOwner, aChunk ));
    //
	const SDblQueLink* const anchor = &iObservers.iA;
	for (SDblQueLink* link = iObservers.First(); link != anchor; link = link->iNext)
		{
		MMemSpyEventMonitorObserver* const observer = _LOFF(link, MMemSpyEventMonitorObserver, __iEMLink);
        const TInt typeMask = observer->EMTypeMask();
        //
        if ( typeMask & EMemSpyEventChunkAdd )
			{
            observer->EMHandleChunkAdd( *aChunk );
			}
        }
    }


void DMemSpyEventMonitor::EventChunkUpdate( DChunk* aChunk )
    {
    TRACE_EM( Kern::Printf("DMemSpyEventMonitor::EventChunkUpdate() - aChunk: 0x%08x [%10d] {OP: %4d, CO: %4d} (%O)", aChunk, aChunk->Size(), ( aChunk->iOwningProcess ? aChunk->iOwningProcess->iId : 0 ), aChunk->iControllingOwner, aChunk ));
    //
	const SDblQueLink* const anchor = &iObservers.iA;
	for (SDblQueLink* link = iObservers.First(); link != anchor; link = link->iNext)
		{
		MMemSpyEventMonitorObserver* const observer = _LOFF(link, MMemSpyEventMonitorObserver, __iEMLink);
        const TInt typeMask = observer->EMTypeMask();
        //
        if ( typeMask & EMemSpyEventChunkUpdate )
			{
            observer->EMHandleChunkUpdated( *aChunk );
			}
        }
    }


void DMemSpyEventMonitor::EventChunkDelete( DChunk* aChunk )
    {
    TRACE_EM( Kern::Printf("DMemSpyEventMonitor::EventChunkDelete() - aChunk: 0x%08x [%10d] {OP: %4d, CO: %4d} (%O)", aChunk, aChunk->Size(), ( aChunk->iOwningProcess ? aChunk->iOwningProcess->iId : 0 ), aChunk->iControllingOwner, aChunk ));
    //
	const SDblQueLink* const anchor = &iObservers.iA;
	for (SDblQueLink* link = iObservers.First(); link != anchor; link = link->iNext)
		{
		MMemSpyEventMonitorObserver* const observer = _LOFF(link, MMemSpyEventMonitorObserver, __iEMLink);
        const TInt typeMask = observer->EMTypeMask();
        //
        if ( typeMask & EMemSpyEventChunkDelete )
			{
            observer->EMHandleChunkDeleted( *aChunk );
			}
        }
    }










TBool DMemSpyEventMonitor::IsObserving( MMemSpyEventMonitorObserver& aObserver )
    {
    TBool ret = EFalse;
	const SDblQueLink* const anchor = &iObservers.iA;
	
	for (SDblQueLink* link = iObservers.First(); link != anchor; link = link->iNext)
		{
		MMemSpyEventMonitorObserver* const observer = _LOFF(link, MMemSpyEventMonitorObserver, __iEMLink);

		if  ( observer == &aObserver )
			{
			ret = ETrue;
            break;
			}
		}

	return ret;
    }

