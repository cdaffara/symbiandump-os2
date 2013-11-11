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

#include "MemSpyDriverInspectedProcessManager.h"

// User includes
#include "MemSpyDriverUtils.h"
#include "MemSpyDriverDevice.h"
#include "MemSpyDriverInspectedProcess.h"


DMemSpyInspectedProcessManager::DMemSpyInspectedProcessManager()
    {
    }


DMemSpyInspectedProcessManager::~DMemSpyInspectedProcessManager()
	{
	TRACE( Kern::Printf("DMemSpyInspectedProcessManager::~DMemSpyInspectedProcessManager() - START"));
    NKern::ThreadEnterCS();

	TRACE( Kern::Printf("DMemSpyInspectedProcessManager::~DMemSpyInspectedProcessManager() - cancelling event monitor..."));
    EventMonitor().RequestEventsCancel( *this );

	TRACE( Kern::Printf("DMemSpyInspectedProcessManager::~DMemSpyInspectedProcessManager() - freeing all inspected processes..."));
    FreeAllInspectedProcesses();

    TRACE( Kern::Printf("DMemSpyInspectedProcessManager::~DMemSpyInspectedProcessManager() - closing auto-start SID list..."));
    iAutoStartSIDs.Close();

    if  ( iDevice )
		{
        TRACE( Kern::Printf("DMemSpyInspectedProcessManager::~DMemSpyInspectedProcessManager() - closing iDevice" ));
		iDevice->Close(NULL);
		}

    NKern::ThreadLeaveCS();
	TRACE( Kern::Printf("DMemSpyInspectedProcessManager::~DMemSpyInspectedProcessManager() - END"));
	}


TInt DMemSpyInspectedProcessManager::Create( DMemSpyDriverDevice* aDevice )
    {
    TRACE( Kern::Printf("DMemSpyInspectedProcessManager::Create() - START" ) );
    //
    TInt error = aDevice->Open();
    if  ( error == KErrNone )
        {
        iDevice = aDevice;

        TRACE( Kern::Printf("DMemSpyInspectedProcessManager::Create() - requesting events..." ) );
        EventMonitor().RequestEvents( *this );
        }
    //
    TRACE( Kern::Printf("DMemSpyInspectedProcessManager::Create() - END - error: %d", error ) );
    return error;
    }


TInt DMemSpyInspectedProcessManager::ProcessOpen( DProcess* aProcess )
    {
    TRACE( Kern::Printf("DMemSpyInspectedProcessManager::ProcessOpen() - START - aProcess: 0x%08x (%O)", aProcess, aProcess ) );
    NKern::ThreadEnterCS();
    TInt error = KErrNotFound;
    //
    DMemSpyInspectedProcess* object = InspectedProcessByProcessId( aProcess->iId );
    if  ( object == NULL )
        {
        object = new DMemSpyInspectedProcess( *iDevice );
        //
        if ( object == NULL )
            {
            error = KErrNoMemory;
            }
        else
            {
            error = object->Open( aProcess );
            //
            if  ( error == KErrNone )
                {
                iMonitoredProcesses.Add( &object->iPMLink );
                }
            else
                {
                delete object;
                }
            }
        }
    else
        {
        error = KErrNone;
        }
    //
    NKern::ThreadLeaveCS();
    TRACE( Kern::Printf("DMemSpyInspectedProcessManager::ProcessOpen() - END - aProcess: 0x%08x, error: %d", aProcess, error ) );
    return error;
    }


TInt DMemSpyInspectedProcessManager::ProcessClose( DProcess* aProcess )
    {
    TRACE( Kern::Printf("DMemSpyInspectedProcessManager::ProcessClose() - START - aProcess: 0x%08x (%O)", aProcess, aProcess ) );
    TInt error = KErrNotFound;
    //
    DMemSpyInspectedProcess* object = InspectedProcessByProcessId( aProcess->iId );
    if  ( object != NULL )
        {
        NKern::ThreadEnterCS();
        object->iPMLink.Deque();
        delete object;
        NKern::ThreadLeaveCS();
        error = KErrNone;
        }
    //
    TRACE( Kern::Printf("DMemSpyInspectedProcessManager::ProcessClose() - END - aProcess: 0x%08x, error: %d", aProcess, error ) );
    return error;
    }


TInt DMemSpyInspectedProcessManager::ProcessCount() const
    {
    TRACE( Kern::Printf("DMemSpyInspectedProcessManager::ProcessCount() - START" ) );
    TInt ret = 0;
    //
	const SDblQueLink* const anchor = &iMonitoredProcesses.iA;
	for (SDblQueLink* link = iMonitoredProcesses.First(); link != anchor; link = link->iNext )
		{
        ++ret;
        }
    //
    TRACE( Kern::Printf("DMemSpyInspectedProcessManager::ProcessCount() - END - count: %d", ret ) );
    return ret;
    }


DMemSpyInspectedProcess* DMemSpyInspectedProcessManager::InspectedProcessByProcessId( TUint aProcessId )
    {
    TRACE( Kern::Printf("DMemSpyInspectedProcessManager::InspectedProcessByProcessId() - START - aProcessId: 0x%08x", aProcessId ) );
    DMemSpyInspectedProcess* ret = 0;
    //
	const SDblQueLink* const anchor = &iMonitoredProcesses.iA;
	for (SDblQueLink* link = iMonitoredProcesses.First(); link != anchor; link = link->iNext )
		{
		DMemSpyInspectedProcess* object = _LOFF( link, DMemSpyInspectedProcess, iPMLink );
        //
        if  ( object->ProcessId() == aProcessId )
            {
            ret = object;
            break;
            }
        }
    //
    TRACE( Kern::Printf("DMemSpyInspectedProcessManager::InspectedProcessByProcessId() - END - aProcessId: 0x%08x, ret: 0x%08x", aProcessId, ret ) );
    return ret;
    }


void DMemSpyInspectedProcessManager::AutoStartListReset()
    {
    TRACE( Kern::Printf("DMemSpyInspectedProcessManager::AutoStartListReset() - START") );
    NKern::ThreadEnterCS();

    iAutoStartSIDs.Reset();

    NKern::ThreadLeaveCS();
    TRACE( Kern::Printf("DMemSpyInspectedProcessManager::AutoStartListReset() - END") );
    }


TInt DMemSpyInspectedProcessManager::AutoStartListAdd( TUint aSID )
    {
    TRACE( Kern::Printf("DMemSpyInspectedProcessManager::AutoStartListAdd() - START - aSID: 0x%08x", aSID ) );
    //
    NKern::ThreadEnterCS();
    const TInt error = iAutoStartSIDs.Append( aSID );
    NKern::ThreadLeaveCS();
    //
    TRACE( Kern::Printf("DMemSpyInspectedProcessManager::AutoStartListAdd() - END - error: %d", error ) );
    return error;
    }






void DMemSpyInspectedProcessManager::FreeAllInspectedProcesses()
    {
    TRACE( Kern::Printf("DMemSpyInspectedProcessManager::FreeAllInspectedProcesses() - START") );
	
    SDblQueLink* link = iMonitoredProcesses.GetFirst();
	while( link )
		{
		DMemSpyInspectedProcess* object = _LOFF( link, DMemSpyInspectedProcess, iPMLink );
        if  ( object )
            {
            delete object;
            }

        link = iMonitoredProcesses.GetFirst();
		}

    TRACE( Kern::Printf("DMemSpyInspectedProcessManager::FreeAllInspectedProcesses() - END") );
    }


DMemSpyEventMonitor& DMemSpyInspectedProcessManager::EventMonitor()
    {
    return iDevice->EventMonitor();
    }


TUint DMemSpyInspectedProcessManager::EMTypeMask() const
    {
    return EMemSpyEventProcessAdd;
    }


void DMemSpyInspectedProcessManager::EMHandleProcessAdd( DProcess& aProcess )
    {
    TRACE( Kern::Printf("DMemSpyInspectedProcessManager::EMHandleProcessAdd() - START - aProcess: 0x%08x %O", &aProcess, &aProcess ) );
 
    const TUint uid3 = aProcess.iUids.iUid[ 2 ].iUid;
    const TInt findResult = iAutoStartSIDs.Find( uid3 );

#ifdef _DEBUG
    const TUint uid1 = aProcess.iUids.iUid[ 0 ].iUid;
    const TUint uid2 = aProcess.iUids.iUid[ 1 ].iUid;
    Kern::Printf("DMemSpyInspectedProcessManager::EMHandleProcessAdd() - uids[ 0x%08x / 0x%08x / 0x%08x ], findResult: %d", uid1, uid2, uid3, findResult );
#endif
    //
    if  ( findResult != KErrNotFound )
        {
        TRACE( Kern::Printf("DMemSpyInspectedProcessManager::EMHandleProcessAdd() - AUTO-START FOR PROCESS %O DETECTED", &aProcess ) );
        const TInt error = ProcessOpen( &aProcess );
        (void) error;
        TRACE( Kern::Printf("DMemSpyInspectedProcessManager::EMHandleProcessAdd() - auto-start error: %d", error ) );
        }
    //
    TRACE( Kern::Printf("DMemSpyInspectedProcessManager::EMHandleProcessAdd() - END" ) );
    }








