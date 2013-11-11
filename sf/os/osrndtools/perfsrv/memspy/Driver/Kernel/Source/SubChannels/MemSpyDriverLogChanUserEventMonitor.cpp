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

#include "MemSpyDriverLogChanUserEventMonitor.h"

// System includes
#include <memspy/driver/memspydriverobjectsshared.h>

// Shared includes
#include "MemSpyDriverOpCodes.h"
#include "MemSpyDriverObjectsInternal.h"

// User includes
#include "MemSpyDriverUtils.h"
#include "MemSpyDriverUserEventMonitor.h"



DMemSpyDriverLogChanUserEventMonitor::DMemSpyDriverLogChanUserEventMonitor( DMemSpyDriverDevice& aDevice, DThread& aThread )
:   DMemSpyDriverLogChanBase( aDevice, aThread )
    {
    TRACE( Kern::Printf("DMemSpyDriverLogChanThreadAndProcess::DMemSpyDriverLogChanThreadAndProcess() - this: 0x%08x", this ));
    }


DMemSpyDriverLogChanUserEventMonitor::~DMemSpyDriverLogChanUserEventMonitor()
	{
	TRACE( Kern::Printf("DMemSpyDriverLogChanThreadAndProcess::~DMemSpyDriverLogChanThreadAndProcess() - START - this: 0x%08x", this ));

    NKern::ThreadEnterCS();
    delete iEventMonitorManager;
    NKern::ThreadLeaveCS();

	TRACE( Kern::Printf("DMemSpyDriverLogChanThreadAndProcess::~DMemSpyDriverLogChanThreadAndProcess() - END - this: 0x%08x", this ));
	}


TInt DMemSpyDriverLogChanUserEventMonitor::Construct()
    {
    TRACE( Kern::Printf("DMemSpyDriverLogChanUserEventMonitor::Construct() - START"));
    //
    TInt error = BaseConstruct();
    //
    if  ( error == KErrNone )
        {
        iEventMonitorManager = new DMemSpyDriverClientEMManager( MemSpyDevice() );

        if  ( iEventMonitorManager )
            {
            TRACE( Kern::Printf("DMemSpyDriverLogChanUserEventMonitor::DoCreate - creating event monitor" ));

            error = iEventMonitorManager->Create();

            TRACE( Kern::Printf("DMemSpyDriverLogChanUserEventMonitor::DoCreate - event monitor create error: %d", error ));
            }
        }
    //
    TRACE( Kern::Printf("DMemSpyDriverLogChanUserEventMonitor::Construct() - END - error: %d", error ));
    return error;
    }



TInt DMemSpyDriverLogChanUserEventMonitor::Request( TInt aFunction, TAny* a1, TAny* a2 )
	{
	TInt r = DMemSpyDriverLogChanBase::Request( aFunction, a1, a2 );
    if  ( r == KErrNone )
        {
	    switch( aFunction )
		    {
        case EMemSpyDriverOpCodeEventMonitorOpen:
            r = EventMonitorOpen( a1 );
            break;
        case EMemSpyDriverOpCodeEventMonitorClose:
            r = EventMonitorClose( (TUint) a1 );
            break;
        case EMemSpyDriverOpCodeEventMonitorNotify:
            r = EventMonitorNotify( (TMemSpyDriverInternalEventMonitorParams*) a1 );
            break;
        case EMemSpyDriverOpCodeEventMonitorNotifyCancel:
            r = EventMonitorNotifyCancel( (TUint) a1 );
            break;

        default:
            r = KErrNotSupported;
		    break;
		    }
        }
    //
    return r;
	}


TBool DMemSpyDriverLogChanUserEventMonitor::IsHandler( TInt aFunction ) const
    {
    return ( aFunction > EMemSpyDriverOpCodeEventMonitorBase && aFunction < EMemSpyDriverOpCodeEventMonitorEnd );
    }



















TInt DMemSpyDriverLogChanUserEventMonitor::EventMonitorOpen( TAny* aHandle )
    {
	TRACE( Kern::Printf("DMemSpyDriverLogChanUserEventMonitor::EventMonitorOpen() - START") );
    NKern::ThreadEnterCS();
    
    DMemSpyDriverClientEM* monitor = iEventMonitorManager->EMOpen();
    TInt error = ( monitor != NULL )? KErrNone : KErrNoMemory;
    const TUint handle = ( monitor != NULL )? monitor->Handle() : KNullHandle;
    
    // Write handle back
    const TInt writeErr = Kern::ThreadRawWrite( &ClientThread(), aHandle, &handle, sizeof(TUint) );
	if  ( writeErr != KErrNone )
		{
        TRACE( Kern::Printf("DMemSpyDriverLogChanUserEventMonitor::EventMonitorOpen() - write error: %d", writeErr ) );
        error = writeErr;
		}
    //
    NKern::ThreadLeaveCS();
	TRACE( Kern::Printf("DMemSpyDriverLogChanUserEventMonitor::EventMonitorOpen() - END - handle: 0x%08x, error: %d", handle, error ));
    return error;
    }


TInt DMemSpyDriverLogChanUserEventMonitor::EventMonitorClose( TUint aHandle )
    {
	TRACE( Kern::Printf("DMemSpyDriverLogChanUserEventMonitor::EventMonitorClose() - START - aHandle: 0x%08x", aHandle));

    NKern::ThreadEnterCS();
    const TInt error = iEventMonitorManager->EMClose( aHandle );
    NKern::ThreadLeaveCS();

	TRACE( Kern::Printf("DMemSpyDriverLogChanUserEventMonitor::EventMonitorClose() - END - error: %d", error ));
    return error;
    }


TInt DMemSpyDriverLogChanUserEventMonitor::EventMonitorNotify( TMemSpyDriverInternalEventMonitorParams* aParams )
    {
	TRACE( Kern::Printf("DMemSpyDriverLogChanUserEventMonitor::EventMonitorNotify() - START"));
    NKern::ThreadEnterCS();

    // Read client info
    TMemSpyDriverInternalEventMonitorParams params;
	TInt error = Kern::ThreadRawRead( &ClientThread(), aParams, &params, sizeof(TMemSpyDriverInternalEventMonitorParams) );
    //
    if  ( error == KErrNone )
        {
        DMemSpyDriverClientEM* monitor = iEventMonitorManager->EMInstance( params.iHandle );
        //
        if  ( monitor != NULL )
            {
            error = monitor->NotifyChanges( &ClientThread(), params.iStatus, params.iContext );
            }
        else
            {
            error = KErrNotFound;
            }
        }

    // NB: Let client take care of completing request in error situations
    NKern::ThreadLeaveCS();

    TRACE( Kern::Printf("DMemSpyDriverLogChanUserEventMonitor::EventMonitorNotify() - END - error: %d", error ));
    return error;
    }


TInt DMemSpyDriverLogChanUserEventMonitor::EventMonitorNotifyCancel( TUint aHandle )
    {
	TRACE( Kern::Printf("DMemSpyDriverLogChanUserEventMonitor::EventMonitorNotifyCancel() - START - aHandle: 0x%08x", aHandle));
    NKern::ThreadEnterCS();

    TInt error = KErrNotFound;
    DMemSpyDriverClientEM* monitor = iEventMonitorManager->EMInstance( aHandle );
    //
    if  ( monitor != NULL )
        {
        error = monitor->NotifyChangesCancel();
        }

    NKern::ThreadLeaveCS();

	TRACE( Kern::Printf("DMemSpyDriverLogChanUserEventMonitor::EventMonitorNotifyCancel() - END - error: %d", error ));
    return error;
    }




