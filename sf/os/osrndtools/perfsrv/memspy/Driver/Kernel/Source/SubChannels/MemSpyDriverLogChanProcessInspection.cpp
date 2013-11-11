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

#include "MemSpyDriverLogChanProcessInspection.h"

// System includes
#include <u32hal.h>
#include <e32rom.h>
#include <memspy/driver/memspydriverobjectsshared.h>

// Shared includes
#include "MemSpyDriverOpCodes.h"
#include "MemSpyDriverObjectsInternal.h"

// User includes
#include "MemSpyDriverUtils.h"
#include "MemSpyDriverDevice.h"
#include "MemSpyDriverInspectedProcess.h"
#include "MemSpyDriverInspectedProcessManager.h"


DMemSpyDriverLogChanProcessInspection::DMemSpyDriverLogChanProcessInspection( DMemSpyDriverDevice& aDevice, DThread& aThread )
:   DMemSpyDriverLogChanBase( aDevice, aThread )
    {
    TRACE( Kern::Printf("DMemSpyDriverLogChanProcessInspection::DMemSpyDriverLogChanProcessInspection() - this: 0x%08x", this ));
    }


DMemSpyDriverLogChanProcessInspection::~DMemSpyDriverLogChanProcessInspection()
	{
	TRACE( Kern::Printf("DMemSpyDriverLogChanProcessInspection::~DMemSpyDriverLogChanProcessInspection() - START - this: 0x%08x", this ));

	TRACE( Kern::Printf("DMemSpyDriverLogChanProcessInspection::~DMemSpyDriverLogChanProcessInspection() - END - this: 0x%08x", this ));
	}






TInt DMemSpyDriverLogChanProcessInspection::Request( TInt aFunction, TAny* a1, TAny* a2 )
	{
	TInt r = DMemSpyDriverLogChanBase::Request( aFunction, a1, a2 );
    if  ( r == KErrNone )
        {
	    switch( aFunction )
		    {
        case EMemSpyDriverOpCodeProcessInspectOpen:
            r = ProcessInspectionOpen( (TUint) a1 );
            break;
        case EMemSpyDriverOpCodeProcessInspectClose:
            r = ProcessInspectionClose( (TUint) a1 );
            break;
        case EMemSpyDriverOpCodeProcessInspectRequestChanges:
            r = ProcessInspectionRequestChanges( (TRequestStatus*) a1, (TMemSpyDriverProcessInspectionInfo*) a2 );
            break;
        case EMemSpyDriverOpCodeProcessInspectRequestChangesCancel:
            r = ProcessInspectionRequestChangesCancel( (TUint) a1 );
            break;
        case EMemSpyDriverOpCodeProcessInspectAutoStartListReset:
            r = ProcessInspectionAutoStartItemsClear();
            break;
        case EMemSpyDriverOpCodeProcessInspectAutoStartListAdd:
            r = ProcessInspectionAutoStartItemsAdd( (TUint) a1 );
            break;

        default:
            r = KErrNotSupported;
		    break;
		    }
        }
    //
    return r;
	}


TBool DMemSpyDriverLogChanProcessInspection::IsHandler( TInt aFunction ) const
    {
    return ( aFunction > EMemSpyDriverOpCodeProcessInspectBase && aFunction < EMemSpyDriverOpCodeProcessInspectEnd );
    }


























TInt DMemSpyDriverLogChanProcessInspection::ProcessInspectionOpen( TUint aPid )
    {
	TRACE( Kern::Printf("DMemSpyDriverLogChanProcessInspection::ProcessInspectionOpen() - START"));
    NKern::ThreadEnterCS();
    
    TInt error = OpenTempObject( aPid, EProcess );
    if  ( error == KErrNone )
    	{
        DMemSpyInspectedProcessManager& pidManager = MemSpyDevice().ProcessInspectionManager();
        error = pidManager.ProcessOpen( (DProcess*) TempObject() );
	    CloseTempObject();
    	}
    //
    NKern::ThreadLeaveCS();
	TRACE( Kern::Printf("DMemSpyDriverLogChanProcessInspection::ProcessInspectionOpen() - END - error: %d", error ));
    return error;
    }


TInt DMemSpyDriverLogChanProcessInspection::ProcessInspectionClose( TUint aPid )
    {
	TRACE( Kern::Printf("DMemSpyDriverLogChanProcessInspection::ProcessInspectionClose() - START"));
    NKern::ThreadEnterCS();

    TInt error = OpenTempObject( aPid, EProcess );
    if  ( error == KErrNone )
    	{
        DMemSpyInspectedProcessManager& pidManager = MemSpyDevice().ProcessInspectionManager();
        error = pidManager.ProcessClose( (DProcess*) TempObject() );
	    CloseTempObject();
    	}
    
    NKern::ThreadLeaveCS();

	TRACE( Kern::Printf("DMemSpyDriverLogChanProcessInspection::ProcessInspectionClose() - END - error: %d", error ));
    return error;
    }


TInt DMemSpyDriverLogChanProcessInspection::ProcessInspectionRequestChanges( TRequestStatus* aStatus, TMemSpyDriverProcessInspectionInfo* aInfo )
    {
	TRACE( Kern::Printf("DMemSpyDriverLogChanProcessInspection::ProcessInspectionRequestChanges() - START"));
    NKern::ThreadEnterCS();

    // Read client info
    TMemSpyDriverProcessInspectionInfo info;
	TInt error = Kern::ThreadRawRead( &ClientThread(), aInfo, &info, sizeof(TMemSpyDriverProcessInspectionInfo) );
    //
    if  ( error == KErrNone )
        {
        DMemSpyInspectedProcessManager& pidManager = MemSpyDevice().ProcessInspectionManager();
        DMemSpyInspectedProcess* pid = pidManager.InspectedProcessByProcessId( info.iProcessId );
        //
        if ( pid != NULL )
            {
            error = pid->NotifyOnChange( &ClientThread(), aStatus, aInfo );
            }
        else
            {
            error = KErrNotFound;
            }
        }

    // NB: Let client take care of completing request in error situations
    NKern::ThreadLeaveCS();
	TRACE( Kern::Printf("DMemSpyDriverLogChanProcessInspection::ProcessInspectionRequestChanges() - END - error: %d", error ));
    return error;
    }


TInt DMemSpyDriverLogChanProcessInspection::ProcessInspectionRequestChangesCancel( TUint aPid )
    {
	TRACE( Kern::Printf("DMemSpyDriverLogChanProcessInspection::ProcessInspectionRequestChangesCancel() - START"));
    NKern::ThreadEnterCS();

    TInt error = KErrNotFound;

    DMemSpyInspectedProcessManager& pidManager = MemSpyDevice().ProcessInspectionManager();
    DMemSpyInspectedProcess* pid = pidManager.InspectedProcessByProcessId( aPid );
    //
    if ( pid != NULL )
        {
        error = pid->NotifyOnChangeCancel();
        }

    NKern::ThreadLeaveCS();

	TRACE( Kern::Printf("DMemSpyDriverLogChanProcessInspection::ProcessInspectionRequestChangesCancel() - END - error: %d", error ));
    return error;
    }


TInt DMemSpyDriverLogChanProcessInspection::ProcessInspectionAutoStartItemsClear()
    {
	TRACE( Kern::Printf("DMemSpyDriverLogChanProcessInspection::ProcessInspectionAutoStartItemsClear() - START"));

    NKern::ThreadEnterCS();

    DMemSpyInspectedProcessManager& pidManager = MemSpyDevice().ProcessInspectionManager();
    pidManager.AutoStartListReset();

    NKern::ThreadLeaveCS();

	TRACE( Kern::Printf("DMemSpyDriverLogChanProcessInspection::ProcessInspectionAutoStartItemsClear() - END" ));
    return KErrNone;
    }


TInt DMemSpyDriverLogChanProcessInspection::ProcessInspectionAutoStartItemsAdd( TUint aSID )
    {
	TRACE( Kern::Printf("DMemSpyDriverLogChanProcessInspection::ProcessInspectionAutoStartItemsAdd() - START - aSID: 0x%08x", aSID ));

    NKern::ThreadEnterCS();
    DMemSpyInspectedProcessManager& pidManager = MemSpyDevice().ProcessInspectionManager();
    const TInt error = pidManager.AutoStartListAdd( aSID );
    NKern::ThreadLeaveCS();

	TRACE( Kern::Printf("DMemSpyDriverLogChanProcessInspection::ProcessInspectionAutoStartItemsAdd() - END - error: %d", error ));
    return error;
    }
















