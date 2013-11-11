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

#include <memspy/engine/memspyenginehelperprocess.h>

// System includes
#include <e32debug.h>

// Driver includes
#include <memspy/driver/memspydriverclient.h>

// User includes
#include <memspy/engine/memspyengine.h>
#include <memspy/engine/memspyenginemidwife.h>
#include <memspy/engine/memspyengineoutputsink.h>
#include <memspy/engine/memspyengineobjectthread.h>
#include <memspy/engine/memspyengineobjectprocess.h>
#include <memspy/engine/memspyenginehelperchunk.h>
#include <memspy/engine/memspyenginehelpercodesegment.h>
#include <memspy/engine/memspyenginehelperthread.h>
#include <memspy/engine/memspyengineprocessmemorytracker.h>



CMemSpyEngineHelperProcess::CMemSpyEngineHelperProcess( CMemSpyEngine& aEngine )
:   iEngine( aEngine )
    {
    }

    
CMemSpyEngineHelperProcess::~CMemSpyEngineHelperProcess()
    {
    delete iUndertaker;
    iEngine.Midwife().RemoveObserver( *this );
    //
    iMemoryTrackers.ResetAndDestroy();
    iMemoryTrackers.Close();
    iMemoryTrackerAutoStartProcessList.Close();
    }


void CMemSpyEngineHelperProcess::ConstructL()
    {
    iUndertaker = CMemSpyEngineUndertaker::NewL( iEngine.Driver() );
    iUndertaker->AddObserverL( *this );
    iEngine.Midwife().AddObserverL( *this );
    }


CMemSpyEngineHelperProcess* CMemSpyEngineHelperProcess::NewL( CMemSpyEngine& aEngine )
    {
    CMemSpyEngineHelperProcess* self = new(ELeave) CMemSpyEngineHelperProcess( aEngine );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


EXPORT_C void CMemSpyEngineHelperProcess::ImpersonateL( TUint32 aSID )
    {
    const TInt err = iEngine.Driver().Impersonate( aSID );
    User::LeaveIfError( err );
    }


EXPORT_C void CMemSpyEngineHelperProcess::OutputProcessInfoL( const CMemSpyProcess& aProcess )
    {
    OutputProcessInfoL( aProcess, EFalse );
    }


EXPORT_C void CMemSpyEngineHelperProcess::OutputProcessInfoDetailedL( const CMemSpyProcess& aProcess )
    {
    OutputProcessInfoL( aProcess, ETrue );
    }


void CMemSpyEngineHelperProcess::OutputProcessInfoL( const CMemSpyProcess& aProcess, TBool aDetailed )
    {
    const TUint pid = aProcess.Id();
    const TUint sid = aProcess.SID();
    RProcess process;
    User::LeaveIfError( iEngine.Driver().OpenProcess( pid, process ) );
    CleanupClosePushL( process );

    const TPtrC procName( aProcess.Name() );
    TBuf<512> infoBuf;
    //
    _LIT(KProcessInfo, "SUMMARY FOR PROCESS \'%S\'");
    infoBuf.Format(KProcessInfo, &procName);
    iEngine.Sink().OutputSectionHeadingL( infoBuf, '=' );
   
    _LIT(KProcessId, "Id");
    iEngine.Sink().OutputItemAndValueL( KProcessId, pid );

    _LIT(KProcessPri, "Priority");
    TBuf<30> priority;
    const TProcessPriority procPriValue = process.Priority();
    CMemSpyProcess::AppendPriority( priority, procPriValue );
    iEngine.Sink().OutputItemAndValueL( KProcessPri, priority );

    _LIT(KProcessSID, "SID");
    iEngine.Sink().OutputItemAndValueL( KProcessSID, sid, ETrue );
    CleanupStack::PopAndDestroy( &process );

    if  ( aDetailed )
        {
        iEngine.ProcessSuspendLC( pid );
        iEngine.HelperChunk().OutputChunkInfoForProcessL( aProcess );
        iEngine.HelperCodeSegment().OutputCodeSegmentsL( pid, infoBuf, KNullDesC, '-', ETrue );
        CleanupStack::PopAndDestroy(); // ProcessSuspendLC
        }

    const TInt count = aProcess.Count();
    for(TInt i=0; i<count; i++)
        {
        const CMemSpyThread& thread = aProcess.At( i );
        iEngine.HelperThread().OutputThreadInfoL( thread, aDetailed );
        }
    }


EXPORT_C void CMemSpyEngineHelperProcess::SetMemoryTrackingAutoStartProcessListL( const RArray<TUid>& aProcessUids )
    {
    iEngine.Driver().ProcessInspectionAutoStartItemsReset();
    //
    const TInt count = aProcessUids.Count();
    iMemoryTrackerAutoStartProcessList.Reset();
    //
    for( TInt i=0; i<count; i++ )
        {
        const TUid uid = aProcessUids[ i ];
#ifdef _DEBUG
        RDebug::Printf( "CMemSpyEngineHelperProcess::SetMemoryTrackingAutoStartProcessListL() - uid[%2d] => 0x%08x", i, uid.iUid );
#endif
        //
        User::LeaveIfError( iMemoryTrackerAutoStartProcessList.Append( uid ) );
        //
        const TInt driverErr = iEngine.Driver().ProcessInspectionAutoStartItemsAdd( uid.iUid );
#ifdef _DEBUG
        RDebug::Printf( "CMemSpyEngineHelperProcess::SetMemoryTrackingAutoStartProcessListL() - driver err: %d", driverErr );
#endif
        User::LeaveIfError( driverErr );
        }
    }


CMemSpyEngineProcessMemoryTracker* CMemSpyEngineHelperProcess::TrackerOrNull( const TProcessId& aId )
    {
    CMemSpyEngineProcessMemoryTracker* ret = NULL;
    //
    const TInt count = iMemoryTrackers.Count();
    for( TInt i=0; i<count; i++ )
        {
        CMemSpyEngineProcessMemoryTracker* tracker = iMemoryTrackers[ i ];
        //
        if ( tracker->ProcessId() == aId )
            {
            ret = tracker;
            break;
            }
        }
    //
    return ret;
    }
    

CMemSpyEngineProcessMemoryTracker& CMemSpyEngineHelperProcess::TrackerL( const TProcessId& aId )
    {
    CMemSpyEngineProcessMemoryTracker* tracker = TrackerOrNull( aId );
    //
    if  ( tracker == NULL )
        {
        // Make a new entry
        tracker = CMemSpyEngineProcessMemoryTracker::NewLC( iEngine, aId );
        User::LeaveIfError( iMemoryTrackers.Append( tracker ) );
        CleanupStack::Pop( tracker );
        }
    //
    return *tracker;
    }


CMemSpyEngineProcessMemoryTracker& CMemSpyEngineHelperProcess::TrackerL( const CMemSpyProcess& aProcess )
    {
    return TrackerL( aProcess.Id() );
    }


void CMemSpyEngineHelperProcess::ThreadIsBornL( const TThreadId& aId, const RThread& aThread )
    {
#ifdef _DEBUG
    RDebug::Printf( "CMemSpyEngineHelperProcess::ThreadIsBornL() - aId: 0x%08x, aThread handle: 0x%08x", (TUint) aId, aThread.Handle() );
#else
    (void) aId;
    (void) aThread;
#endif
    }


void CMemSpyEngineHelperProcess::ProcessIsBornL( const TProcessId& aId, const RProcess& aProcess )
    {
#ifdef _DEBUG
    RDebug::Printf( "CMemSpyEngineHelperProcess::ProcessIsBornL() - aId: 0x%08x, aProcess handle: 0x%08x", (TUint) aId, aProcess.Handle() );
#else
    (void) aId;
    (void) aProcess;
#endif

    if ( aProcess.Handle() != KNullHandle )
        {
        const TBool isAutoStart = IsAutoStartupProcess( aProcess.SecureId() );
        //
        if ( isAutoStart )
            {
            CMemSpyEngineProcessMemoryTracker& tracker = TrackerL( aId );
            tracker.StartL();
            }
        }
    }


void CMemSpyEngineHelperProcess::ThreadIsDeadL( const TThreadId& aId, const RThread& aThread )
    {
#ifdef _DEBUG
    RDebug::Printf( "CMemSpyEngineHelperProcess::ThreadIsDeadL() - aId: 0x%08x, aThread handle: 0x%08x", (TUint) aId, aThread.Handle() );
#else
    (void) aId;
    (void) aThread;
#endif
    }


void CMemSpyEngineHelperProcess::ProcessIsDeadL( const TProcessId& aId, const RProcess& aProcess )
    {
#ifdef _DEBUG
    RDebug::Printf( "CMemSpyEngineHelperProcess::ProcessIsDeadL() - aId: 0x%08x, aProcess handle: 0x%08x", (TUint) aId, aProcess.Handle() );
#else
    (void) aProcess;
#endif

    CMemSpyEngineProcessMemoryTracker* tracker = TrackerOrNull( aId );
    if  ( tracker )
        {
#ifdef _DEBUG
        RDebug::Printf( "CMemSpyEngineHelperProcess::ProcessIsDeadL() - tracker for this process should take care of cleanup by itself..." );
#endif
        }
    }
    

TBool CMemSpyEngineHelperProcess::IsAutoStartupProcess( TUid aUid ) const
    {
    TBool match = EFalse;
    //
    const TInt count = iMemoryTrackerAutoStartProcessList.Count();
    for( TInt i=0; i<count; i++ )
        {
        const TUid uid = iMemoryTrackerAutoStartProcessList[ i ];
        if ( uid == aUid )
            {
            match = ETrue;
            break;
            }
        }
    //
#ifdef _DEBUG
    RDebug::Printf( "CMemSpyEngineHelperProcess::IsAutoStartupProcess() - aUid: 0x%08x, ret: %d", aUid.iUid, match );
#endif
    return match;
    }
    


