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

#include <memspy/engine/memspyengineundertaker.h>

// Driver includes
#include <memspy/driver/memspydriverclient.h>
#include <memspy/driver/memspydriverenumerationsshared.h>



CMemSpyEngineUndertaker::CMemSpyEngineUndertaker( RMemSpyDriverClient& aDriver, TInt aPriority )
:	CActive( aPriority ), iDriver( aDriver )
	{
	CActiveScheduler::Add( this );
	}


CMemSpyEngineUndertaker::~CMemSpyEngineUndertaker()
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


void CMemSpyEngineUndertaker::ConstructL()
	{
    const TInt error = iDriver.EventMonitorOpen( iEventMonitorHandle );
    User::LeaveIfError( error );
    //
	Request();
	}


CMemSpyEngineUndertaker* CMemSpyEngineUndertaker::NewL( RMemSpyDriverClient& aDriver, TInt aPriority )
	{
	CMemSpyEngineUndertaker* self = new(ELeave) CMemSpyEngineUndertaker( aDriver, aPriority );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}


void CMemSpyEngineUndertaker::AddObserverL( MMemSpyEngineUndertakerObserver& aObserver )
    {
    RemoveObserver( aObserver );
    iObservers.AppendL( &aObserver );
    }


void CMemSpyEngineUndertaker::RemoveObserver( MMemSpyEngineUndertakerObserver& aObserver )
    {
    const TInt count = iObservers.Count();
    for( TInt i=count-1; i>=0; i-- )
        {
        MMemSpyEngineUndertakerObserver* obs = iObservers[ i ];
        if ( obs == &aObserver )
            {
            iObservers.Remove( i );
            }
        }
    }


void CMemSpyEngineUndertaker::RunL()
	{
    const TUint notifiedId = iId;
    const TInt typeAsInt = iStatus.Int();
    User::LeaveIfError( typeAsInt );
	Request();
    //
    const TMemSpyDriverEventType type = static_cast< TMemSpyDriverEventType >( typeAsInt );
    switch( type )
        {
    case EMemSpyDriverEventTypeProcessRemove:
        {
        const TProcessId id( notifiedId );
        NotifyProcessDeadL( id );
        break;
        }
    case EMemSpyDriverEventTypeThreadKill:
        {
        const TThreadId id( notifiedId );
        NotifyThreadDeadL( id );
        break;
        }
    default:
        // Process removal not handled as not relevant
        break;
        }
	}


void CMemSpyEngineUndertaker::DoCancel()
	{
	iDriver.EventMonitorNotifyCancel( iEventMonitorHandle );
	}


void CMemSpyEngineUndertaker::Request()
	{
    Cancel();
    iDriver.EventMonitorNotify( iEventMonitorHandle, iStatus, iId );
	SetActive();
	}


void CMemSpyEngineUndertaker::NotifyProcessDeadL( const TProcessId& aId )
    {
    // NB: opening may fail - client's must tolerate this
    RProcess process;
    iDriver.OpenProcess( aId, process );
    CleanupClosePushL( process );
    //
    const TInt count = iObservers.Count();
    for( TInt i=0; i<count; i++ )
        {
        MMemSpyEngineUndertakerObserver* obs = iObservers[ i ];
        TRAP_IGNORE( obs->ProcessIsDeadL( aId, process ) );
        }
    //
    CleanupStack::PopAndDestroy( &process );
    }


void CMemSpyEngineUndertaker::NotifyThreadDeadL( const TThreadId& aId )
    {
    // NB: opening may fail - client's must tolerate this
    RThread thread;
    iDriver.OpenThread( aId, thread );
    CleanupClosePushL( thread );
    //
    const TInt count = iObservers.Count();
    for( TInt i=0; i<count; i++ )
        {
        MMemSpyEngineUndertakerObserver* obs = iObservers[ i ];
        TRAP_IGNORE( obs->ThreadIsDeadL( aId, thread ) );
        }
    //
    CleanupStack::PopAndDestroy( &thread );
    }

