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

#include <memspy/engine/memspyenginemidwife.h>

// Driver includes
#include <memspy/driver/memspydriverclient.h>
#include <memspy/driver/memspydriverenumerationsshared.h>



CMemSpyEngineMidwife::CMemSpyEngineMidwife( RMemSpyDriverClient& aDriver, TInt aPriority )
:	CActive( aPriority ), iDriver( aDriver )
	{
	CActiveScheduler::Add( this );
	}


CMemSpyEngineMidwife::~CMemSpyEngineMidwife()
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


void CMemSpyEngineMidwife::ConstructL()
	{
    const TInt error = iDriver.EventMonitorOpen( iEventMonitorHandle );
    User::LeaveIfError( error );
    //
	Request();
	}


CMemSpyEngineMidwife* CMemSpyEngineMidwife::NewL( RMemSpyDriverClient& aDriver, TInt aPriority )
	{
	CMemSpyEngineMidwife* self = new(ELeave) CMemSpyEngineMidwife( aDriver, aPriority );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}


void CMemSpyEngineMidwife::AddObserverL( MMemSpyEngineMidwifeObserver& aObserver )
    {
    RemoveObserver( aObserver );
    iObservers.AppendL( &aObserver );
    }


void CMemSpyEngineMidwife::RemoveObserver( MMemSpyEngineMidwifeObserver& aObserver )
    {
    const TInt count = iObservers.Count();
    for( TInt i=count-1; i>=0; i-- )
        {
        MMemSpyEngineMidwifeObserver* obs = iObservers[ i ];
        if ( obs == &aObserver )
            {
            iObservers.Remove( i );
            }
        }
    }


void CMemSpyEngineMidwife::RunL()
	{
    const TUint notifiedId = iId;
    const TInt typeAsInt = iStatus.Int();
    User::LeaveIfError( typeAsInt );
	Request();
    //
    const TMemSpyDriverEventType type = static_cast< TMemSpyDriverEventType >( typeAsInt );
    switch( type )
        {
    case EMemSpyDriverEventTypeThreadCreate:
        {
        const TThreadId id( notifiedId );
        NotifyThreadBornL( id );
        break;
        }
    case EMemSpyDriverEventTypeProcessCreate:
        {
        const TProcessId id( notifiedId );
        NotifyProcessBornL( id );
        break;
        }
    default:
        // Process removal not handled as not relevant
        break;
        }
	}


void CMemSpyEngineMidwife::DoCancel()
	{
	iDriver.EventMonitorNotifyCancel( iEventMonitorHandle );
	}


void CMemSpyEngineMidwife::Request()
	{
    Cancel();
    iDriver.EventMonitorNotify( iEventMonitorHandle, iStatus, iId );
	SetActive();
	}


void CMemSpyEngineMidwife::NotifyProcessBornL( const TProcessId& aId )
    {
    // NB: opening may fail - client's must tolerate this
    RProcess process;
    iDriver.OpenProcess( aId, process );
    CleanupClosePushL( process );
    //
    const TInt count = iObservers.Count();
    for( TInt i=0; i<count; i++ )
        {
        MMemSpyEngineMidwifeObserver* obs = iObservers[ i ];
        TRAP_IGNORE( obs->ProcessIsBornL( aId, process ) );
        }
    //
    CleanupStack::PopAndDestroy( &process );
    }


void CMemSpyEngineMidwife::NotifyThreadBornL( const TThreadId& aId )
    {
    // NB: opening may fail - client's must tolerate this
    RThread thread;
    iDriver.OpenThread( aId, thread );
    CleanupClosePushL( thread );
    //
    const TInt count = iObservers.Count();
    for( TInt i=0; i<count; i++ )
        {
        MMemSpyEngineMidwifeObserver* obs = iObservers[ i ];
        TRAP_IGNORE( obs->ThreadIsBornL( aId, thread ) );
        }
    //
    CleanupStack::PopAndDestroy( &thread );
    }

