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

#include "MemSpyDriverUserEventMonitor.h"

// System includes
#include <memspy/driver/memspydriverenumerationsshared.h>

// User includes
#include "MemSpyDriverUtils.h"
#include "MemSpyDriverDevice.h"
#include "MemSpyDriverOSAdaption.h"
#include "MemSpyDriverEventMonitor.h"

// Literal constants
_LIT( KMemSpyDriverClientEMMutexName, "MemSpyDriverClientEM_0x" );




DMemSpyDriverClientEMManager::DMemSpyDriverClientEMManager( DMemSpyDriverDevice& aDevice )
:   iDevice( aDevice )
    {
    }


DMemSpyDriverClientEMManager::~DMemSpyDriverClientEMManager()
	{
	TRACE_EM( Kern::Printf("DMemSpyDriverClientEMManager::~DMemSpyDriverClientEMManager() - START"));

    NKern::ThreadEnterCS();
    FreeAllInstances();
    NKern::ThreadLeaveCS();

	TRACE_EM( Kern::Printf("DMemSpyDriverClientEMManager::~DMemSpyDriverClientEMManager() - END"));
	}


TInt DMemSpyDriverClientEMManager::Create()
    {
    return KErrNone;
    }


DMemSpyDriverClientEM* DMemSpyDriverClientEMManager::EMOpen()
    {
    TRACE_EM( Kern::Printf("DMemSpyDriverClientEMManager::EMOpen() - START - iNextHandle: %d, iDevice: 0x%08x", iNextHandle+1, &iDevice ) );
    NKern::ThreadEnterCS();
    //
    DMemSpyDriverClientEM* object = new DMemSpyDriverClientEM( iDevice, ++iNextHandle );
    if  ( object != NULL )
        {
        TRACE_EM( Kern::Printf("DMemSpyDriverClientEMManager::EMOpen() - calling create..." ) );
        const TInt error = object->Create();
        if ( error != KErrNone )
            {
            TRACE_EM( Kern::Printf("DMemSpyDriverClientEMManager::EMOpen() - creation error: %d", error ) );
            delete object;
            object = NULL;
            }
        else
            {
            iEMInstances.Add( &object->iLink );
            }
        }
    //
    NKern::ThreadLeaveCS();
    TRACE_EM( Kern::Printf("DMemSpyDriverClientEMManager::EMOpen() - END - object: 0x%08x", object ) );
    //
    return object;
    }


TInt DMemSpyDriverClientEMManager::EMClose( TUint aHandle )
    {
    TRACE_EM( Kern::Printf("DMemSpyDriverClientEMManager::EMClose() - START - aHandle: 0x%08x", aHandle ) );
    TInt error = KErrNotFound;
    //
    DMemSpyDriverClientEM* object = EMInstance( aHandle );
    if  ( object != NULL )
        {
        NKern::ThreadEnterCS();
        object->iLink.Deque();
        delete object;
        NKern::ThreadLeaveCS();
        error = KErrNone;
        }
    //
    TRACE_EM( Kern::Printf("DMemSpyDriverClientEMManager::EMClose() - END - error: %d", error ) );
    return error;
    }


DMemSpyDriverClientEM* DMemSpyDriverClientEMManager::EMInstance( TUint aHandle )
    {
    TRACE_EM( Kern::Printf("DMemSpyDriverClientEMManager::EMInstance() - START - aHandle: 0x%08x", aHandle ) );
    DMemSpyDriverClientEM* ret = NULL;
    //
	const SDblQueLink* const anchor = &iEMInstances.iA;
	for (SDblQueLink* link = iEMInstances.First(); link != anchor; link = link->iNext )
		{
		DMemSpyDriverClientEM* object = _LOFF( link, DMemSpyDriverClientEM, iLink );
        //
        if  ( object->Handle() == aHandle )
            {
            ret = object;
            break;
            }
        }
    //
    TRACE_EM( Kern::Printf("DMemSpyDriverClientEMManager::EMInstance() - END - aHandle: 0x%08x, ret: 0x%08x", aHandle, ret ) );
    return ret;
    }


void DMemSpyDriverClientEMManager::FreeAllInstances()
    {
    TRACE_EM( Kern::Printf("DMemSpyDriverClientEMManager::FreeAllInstances() - START") );
	
    SDblQueLink* link = iEMInstances.GetFirst();
	while( link )
		{
		DMemSpyDriverClientEM* object = _LOFF( link, DMemSpyDriverClientEM, iLink );
        delete object;
        link = iEMInstances.GetFirst();
		}

    TRACE_EM( Kern::Printf("DMemSpyDriverClientEMManager::FreeAllInstances() - END") );
    }





























DMemSpyDriverClientEM::DMemSpyDriverClientEM( DMemSpyDriverDevice& aDevice, TUint aHandle )
:   iDevice( aDevice ), iHandle( aHandle )
    {
    }


DMemSpyDriverClientEM::~DMemSpyDriverClientEM()
    {
    TRACE_EM( Kern::Printf("DMemSpyDriverClientEM::~DMemSpyDriverClientEM() - START - this: 0x%08x", this ));
	iDevice.EventMonitor().RequestEventsCancel( *this );

    TRACE_EM( Kern::Printf("DMemSpyDriverClientEM::~DMemSpyDriverClientEM() - calling NotifyChangesCancel..." ) );
    NotifyChangesCancel();
    
    TRACE_EM( Kern::Printf("DMemSpyDriverClientEM::~DMemSpyDriverClientEM() - calling ResetPendingChanges..." ) );
    ResetPendingChanges();

	if  ( iLock )
		{
        TRACE_EM( Kern::Printf("DMemSpyDriverClientEM::~DMemSpyDriverClientEM() - closing mutex..." ) );
		iLock->Close(NULL);
		}

    TRACE_EM( Kern::Printf("DMemSpyDriverClientEM::~DMemSpyDriverClientEM() - END - this: 0x%08x" ));
    }


TInt DMemSpyDriverClientEM::Create()
    {
    TRACE_EM( Kern::Printf("DMemSpyDriverClientEM::Create() - START - handle: 0x%08x", Handle() ) );

    // Create mutex
    TName name( KMemSpyDriverClientEMMutexName );
    name.AppendNumFixedWidth( (TUint) this, EHex, 8 );
    TInt error = Kern::MutexCreate( iLock, name, KMutexOrdNone );
    //
    if  ( error == KErrNone )
        {
        TRACE_EM( Kern::Printf("DMemSpyDriverClientEM::Create() - calling global device driver event monitor...") );
        iDevice.EventMonitor().RequestEvents( *this );
        }
    //
    TRACE_EM( Kern::Printf("DMemSpyDriverClientEM::Create() - END - handle: 0x%08x, err: %d", Handle(), error ) );
    return error;
    }


TInt DMemSpyDriverClientEM::NotifyChanges( DThread* aClientThread, TRequestStatus* aClientRS, TAny* aClientContext )
    {
	Kern::MutexWait( *iLock );

    TRACE_EM( Kern::Printf("DMemSpyDriverClientEM::NotifyChanges() - START - handle: 0x%08x", Handle() ) );
    TInt r = KErrInUse;
    //
    if  ( iClientRS == NULL )
        {
        TRACE_EM( Kern::Printf("DMemSpyDriverClientEM::NotifyChanges() - saving client's request...") );
        iClientThread = aClientThread;
        iClientRS = aClientRS;
        iClientContext = aClientContext;
        //
        if	( !iPendingChanges.IsEmpty() )
			{
            TRACE_EM( Kern::Printf("DMemSpyDriverClientEM::NotifyOnChange() - Have buffered changes - SENDING TO CLIENT IMMEDIATELY..." ) );
			
            // We have something in the pending buffer so we can
			// give it back to the client immediately.
	        DMemSpyDriverClientEM::TChange* cachedChange = _LOFF( iPendingChanges.First(), DMemSpyDriverClientEM::TChange, iLink );
			cachedChange->iLink.Deque();
			
            // Notify about change			
			CompleteClientsRequest( cachedChange->iCompletionCode, cachedChange->iContext );
			
			// Discard cached entry
            NKern::ThreadEnterCS();
			delete cachedChange;
            NKern::ThreadLeaveCS();
			}
        //
        r = KErrNone;
        }
    //
    TRACE_EM( Kern::Printf("DMemSpyDriverClientEM::NotifyChanges() - END - handle: 0x%08x, error: %d", Handle(), r ) );
	Kern::MutexSignal( *iLock );
    return r;
    }


TInt DMemSpyDriverClientEM::NotifyChangesCancel()
    {
	Kern::MutexWait( *iLock );
    TRACE_EM( Kern::Printf("DMemSpyDriverClientEM::NotifyChangesCancel() - START - handle: 0x%08x, iClientThread: 0x%08x, iClientRS: 0x%08x", Handle(), iClientThread, iClientRS ) );
    //
    TInt r = KErrNotReady;
    //
    if  ( iClientRS != NULL )
        {
        DThread* clientThread = iClientThread;
        TRequestStatus* clientRS = iClientRS;
        //
        iClientThread = NULL;
        iClientRS = NULL;
        iClientContext = NULL;
        //
        TRACE_EM( Kern::Printf("DMemSpyDriverClientEM::NotifyChangesCancel() - doing final request complete...") );
		Kern::RequestComplete( clientThread, clientRS, KErrCancel );
        r = KErrNone;
        }
    //
    TRACE_EM( Kern::Printf("DMemSpyDriverClientEM::NotifyChangesCancel() - END - handle: 0x%08x, error: %d", Handle(), r ) );
	Kern::MutexSignal( *iLock );
    return r;
    }


TUint DMemSpyDriverClientEM::EMTypeMask() const
    {
    const TUint ret = EMemSpyEventThreadAdd  | EMemSpyEventThreadKill |
                      EMemSpyEventProcessAdd | EMemSpyEventProcessRemove | 
                      EMemSpyEventChunkAdd   | EMemSpyEventChunkDelete;
    return ret;
    }


void DMemSpyDriverClientEM::EMHandleProcessAdd( DProcess& aProcess )
    {
    const TUint pid = iDevice.OSAdaption().DProcess().GetId( aProcess );
    //
	Kern::MutexWait( *iLock );
    CompleteClientsRequest( EMemSpyDriverEventTypeProcessCreate, pid );
	Kern::MutexSignal( *iLock );
    }


void DMemSpyDriverClientEM::EMHandleProcessRemoved( DProcess& aProcess )
    {
    const TUint pid = iDevice.OSAdaption().DProcess().GetId( aProcess );
    //
	Kern::MutexWait( *iLock );
    CompleteClientsRequest( EMemSpyDriverEventTypeProcessRemove, pid );
	Kern::MutexSignal( *iLock );
    }


void DMemSpyDriverClientEM::EMHandleThreadAdd( DThread& aThread )
    {
    const TUint tid = iDevice.OSAdaption().DThread().GetId( aThread );
    //
	Kern::MutexWait( *iLock );
    CompleteClientsRequest( EMemSpyDriverEventTypeThreadCreate, tid );
	Kern::MutexSignal( *iLock );
    }


void DMemSpyDriverClientEM::EMHandleThreadKilled( DThread& aThread )
    {
    const TUint tid = iDevice.OSAdaption().DThread().GetId( aThread );
    //
	Kern::MutexWait( *iLock );
    CompleteClientsRequest( EMemSpyDriverEventTypeThreadKill, tid );
	Kern::MutexSignal( *iLock );
    }


void DMemSpyDriverClientEM::EMHandleChunkAdd( DChunk& aChunk ) 
    {
	Kern::MutexWait( *iLock );
    CompleteClientsRequest( EMemSpyDriverEventTypeChunkAdd, (TUint) &aChunk );
	Kern::MutexSignal( *iLock );
    }


void DMemSpyDriverClientEM::EMHandleChunkDeleted( DChunk& aChunk )
    {
	Kern::MutexWait( *iLock );
    CompleteClientsRequest( EMemSpyDriverEventTypeChunkDestroy, (TUint) &aChunk );
	Kern::MutexSignal( *iLock );
    }


void DMemSpyDriverClientEM::CompleteClientsRequest( TInt aCompletionCode, TUint aContext )
    {
    TRACE_EM( Kern::Printf("DMemSpyDriverClientEM::CompleteClientsRequest() - START - handle: 0x%08x, iClientThread: 0x%08x, iClientRS: 0x%08x, iClientContext: 0x%08x, aCompletionCode: %d, aContext: %d, changesPending: %d", Handle(), iClientThread, iClientRS, iClientContext, aCompletionCode, aContext, !iPendingChanges.IsEmpty() ) );
    //
    if  ( iClientRS != NULL )
        {
        TRACE_EM( Kern::Printf("DMemSpyDriverClientEM::CompleteClientsRequest() - attempting to complete client's request...") );

        // First write context info
        const TInt writeErr = Kern::ThreadRawWrite( iClientThread, iClientContext, &aContext, sizeof(TUint) );
		if  ( writeErr != KErrNone )
		    {
            TRACE_EM( Kern::Printf("DMemSpyDriverClientEM::CompleteClientsRequest() - write error: %d", writeErr ) );
            aCompletionCode = writeErr;
		    }

        // Now complete event - avoiding race conditions!
        DThread* clientThread = iClientThread;
        TRequestStatus* clientRS = iClientRS;
        //
        iClientThread = NULL;
        iClientRS = NULL;
        iClientContext = NULL;
        //
        TRACE_EM( Kern::Printf("DMemSpyDriverClientEM::CompleteClientsRequest() - doing final request complete...") );
		Kern::RequestComplete( clientThread, clientRS, aCompletionCode );
        }
    else
        {
		// Buffer the change for next time around...
        TRACE_EM( Kern::Printf("DMemSpyDriverClientEM::CompleteClientsRequest() - BUFFERING change event whilst client is AWOL...", this ) );
        NKern::ThreadEnterCS();
        
        DMemSpyDriverClientEM::TChange* cachedChange = new DMemSpyDriverClientEM::TChange( aCompletionCode, aContext );
        if  ( cachedChange )
            {
            iPendingChanges.Add( &cachedChange->iLink );
            }
        //
        NKern::ThreadLeaveCS();
        }
    //
    TRACE_EM( Kern::Printf("DMemSpyDriverClientEM::CompleteClientsRequest() - END - handle: 0x%08x", Handle() ) );
    }


void DMemSpyDriverClientEM::ResetPendingChanges()
    {
    TRACE_EM( Kern::Printf("DMemSpyDriverClientEM::ResetPendingChanges() - START - this: 0x%08x", this ) );
    NKern::ThreadEnterCS();
    //
    SDblQueLink* link = iPendingChanges.GetFirst();
	while( link )
		{
        DMemSpyDriverClientEM::TChange* cachedChange = _LOFF( link, DMemSpyDriverClientEM::TChange, iLink );
        delete cachedChange;
        link = iPendingChanges.GetFirst();
		}
    //
    NKern::ThreadLeaveCS();
    TRACE_EM( Kern::Printf("DMemSpyDriverClientEM::ResetPendingChanges() - END - this: 0x%08x", this ) );
    }















