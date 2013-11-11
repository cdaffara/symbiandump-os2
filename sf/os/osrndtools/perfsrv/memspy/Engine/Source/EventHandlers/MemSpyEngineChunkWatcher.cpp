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

#include "MemSpyEngineChunkWatcher.h"

// Driver includes
#include <memspy/driver/memspydriverclient.h>
#include <memspy/driver/memspydriverenumerationsshared.h>



CMemSpyEngineChunkWatcher::CMemSpyEngineChunkWatcher( RMemSpyDriverClient& aDriver, TInt aPriority )
:	CActive( aPriority ), iDriver( aDriver )
	{
	CActiveScheduler::Add( this );
	}


CMemSpyEngineChunkWatcher::~CMemSpyEngineChunkWatcher()
	{
	Cancel();
    //
    if ( iEventMonitorHandle != KNullHandle )
        {
        iDriver.EventMonitorClose( iEventMonitorHandle );
        }
    //
    iObservers.Close();
	}


void CMemSpyEngineChunkWatcher::ConstructL()
	{
    const TInt error = iDriver.EventMonitorOpen( iEventMonitorHandle );
    User::LeaveIfError( error );
    //
	Request();
	}


CMemSpyEngineChunkWatcher* CMemSpyEngineChunkWatcher::NewL( RMemSpyDriverClient& aDriver, TInt aPriority )
	{
	CMemSpyEngineChunkWatcher* self = new(ELeave) CMemSpyEngineChunkWatcher( aDriver, aPriority );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}


void CMemSpyEngineChunkWatcher::AddObserverL( MMemSpyEngineChunkWatcherObserver& aObserver )
    {
    RemoveObserver( aObserver );
    iObservers.AppendL( &aObserver );
    }


void CMemSpyEngineChunkWatcher::RemoveObserver( MMemSpyEngineChunkWatcherObserver& aObserver )
    {
    const TInt count = iObservers.Count();
    for( TInt i=count-1; i>=0; i-- )
        {
        MMemSpyEngineChunkWatcherObserver* obs = iObservers[ i ];
        if ( obs == &aObserver )
            {
            iObservers.Remove( i );
            }
        }
    }


void CMemSpyEngineChunkWatcher::RunL()
	{
    const TUint notifiedId = iId;
    const TInt typeAsInt = iStatus.Int();
    User::LeaveIfError( typeAsInt );
	Request();
    //
    const TMemSpyDriverEventType type = static_cast< TMemSpyDriverEventType >( typeAsInt );
    switch( type )
        {
    case EMemSpyDriverEventTypeChunkAdd:
        {
        NotifyChunkAddL( notifiedId );
        break;
        }
    case EMemSpyDriverEventTypeChunkDestroy:
        {
        NotifyChunkDestroyL( notifiedId );
        break;
        }
    default:
        break;
        }
	}


void CMemSpyEngineChunkWatcher::DoCancel()
	{
	iDriver.EventMonitorNotifyCancel( iEventMonitorHandle );
	}


void CMemSpyEngineChunkWatcher::Request()
	{
    Cancel();
    iDriver.EventMonitorNotify( iEventMonitorHandle, iStatus, iId );
	SetActive();
	}


void CMemSpyEngineChunkWatcher::NotifyChunkAddL( TUint aChunkHandle )
    {
    const TInt count = iObservers.Count();
    for( TInt i=count-1; i>=0; i-- )
        {
        MMemSpyEngineChunkWatcherObserver* obs = iObservers[ i ];
        TRAP_IGNORE( obs->HandleChunkAddL( aChunkHandle ) );
        }
    }


void CMemSpyEngineChunkWatcher::NotifyChunkDestroyL( TUint aChunkHandle )
    {
    const TInt count = iObservers.Count();
    for( TInt i=count-1; i>=0; i-- )
        {
        MMemSpyEngineChunkWatcherObserver* obs = iObservers[ i ];
        TRAP_IGNORE( obs->HandleChunkDestroyL( aChunkHandle ) );
        }
    }

