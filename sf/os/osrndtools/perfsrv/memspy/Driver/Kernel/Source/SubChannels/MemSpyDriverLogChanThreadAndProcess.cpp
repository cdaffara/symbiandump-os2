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

#include "MemSpyDriverLogChanThreadAndProcess.h"

// System includes
#include <platform.h>
#include <memspy/driver/memspydriverobjectsshared.h>
#include <memspy/driver/memspydriverpanics.h>

// Shared includes
#include "MemSpyDriverOpCodes.h"
#include "MemSpyDriverObjectsInternal.h"

// User includes
#include "MemSpyDriverUtils.h"
#include "MemSpyDriverOSAdaption.h"
#include "MemSpyDriverSuspensionManager.h"

// Constants
const TBool KMemSpyDriverAllowDeadOpenRequests = ETrue;
const TInt KMemSpyDriverLogChanThreadAndProcessXferBufferSize = 512;


DMemSpyDriverLogChanThreadAndProcess::DMemSpyDriverLogChanThreadAndProcess( DMemSpyDriverDevice& aDevice, DThread& aThread )
:   DMemSpyDriverLogChanBase( aDevice, aThread )
    {
    TRACE( Kern::Printf("DMemSpyDriverLogChanThreadAndProcess::DMemSpyDriverLogChanThreadAndProcess() - this: 0x%08x", this ));
    }


DMemSpyDriverLogChanThreadAndProcess::~DMemSpyDriverLogChanThreadAndProcess()
	{
	TRACE( Kern::Printf("DMemSpyDriverLogChanThreadAndProcess::~DMemSpyDriverLogChanThreadAndProcess() - START - this: 0x%08x", this ));

	TRACE( Kern::Printf("DMemSpyDriverLogChanThreadAndProcess::~DMemSpyDriverLogChanThreadAndProcess() - END - this: 0x%08x", this ));
	}


TInt DMemSpyDriverLogChanThreadAndProcess::Construct()
	{
	TRACE( Kern::Printf("DMemSpyDriverLogChanThreadAndProcess::Construct() - START - this: 0x%08x", this ));
    
    const TInt ret = BaseConstruct( KMemSpyDriverLogChanThreadAndProcessXferBufferSize );

	TRACE( Kern::Printf("DMemSpyDriverLogChanThreadAndProcess::Construct() - END - this: 0x%08x, err: %d", this, ret ));
    return ret;
	}


TInt DMemSpyDriverLogChanThreadAndProcess::Request( TInt aFunction, TAny* a1, TAny* a2 )
	{
	TInt r = DMemSpyDriverLogChanBase::Request( aFunction, a1, a2 );
    if  ( r == KErrNone )
        {
	    switch( aFunction )
		    {
	    case EMemSpyDriverOpCodeThreadAndProcessGetInfoThread:
		    r = GetInfoThread( (TUint)a1, (TMemSpyDriverInternalThreadInfoParams*) a2);
		    break;
	    case EMemSpyDriverOpCodeThreadAndProcessGetInfoProcess:
		    r = GetInfoProcess( (TUint)a1, (TMemSpyDriverProcessInfo*) a2);
		    break;
        case EMemSpyDriverOpCodeThreadAndProcessEndThread:
            r = EndThread( (TUint) a1, (TExitType) ((TUint) a2) );
            break;
        case EMemSpyDriverOpCodeThreadAndProcessOpenThread:
            r = OpenThread( (TUint) a1 );
            break;
        case EMemSpyDriverOpCodeThreadAndProcessOpenProcess:
            r = OpenProcess( (TUint) a1 );
            break;
        case EMemSpyDriverOpCodeThreadAndProcessSuspendAllThreads:
            r = SuspendAllThreadsInProcess( (TUint) a1 );
            break;
        case EMemSpyDriverOpCodeThreadAndProcessResumeAllThreads:
            r = ResumeAllThreadsInProcess( (TUint) a1 );
            break;
        case EMemSpyDriverOpCodeThreadAndProcessGetThreads:
            r = GetThreadsForProcess( (TUint) a1, (TDes8*) a2 );
            break;
        case EMemSpyDriverOpCodeThreadAndProcessSetPriorityThread:
            r = SetPriority( (TUint) a1, (TThreadPriority) ((TUint) a2) );
            break;

        default:
            r = KErrNotSupported;
		    break;
		    }
        }
    //
    return r;
	}


TBool DMemSpyDriverLogChanThreadAndProcess::IsHandler( TInt aFunction ) const
    {
    return ( aFunction > EMemSpyDriverOpCodeThreadAndProcessBase && aFunction < EMemSpyDriverOpCodeThreadAndProcessEnd );
    }






TInt DMemSpyDriverLogChanThreadAndProcess::GetInfoThread( TUint aTid, TMemSpyDriverInternalThreadInfoParams* aParams )
	{
    TRACE( Kern::Printf("DMemSpyDriverLogChanThreadAndProcess::GetInfoThread() - START"));

    TMemSpyDriverInternalThreadInfoParams params;
    TInt r = Kern::ThreadRawRead( &ClientThread(), aParams, &params, sizeof(TMemSpyDriverInternalThreadInfoParams) );
    if  ( r != KErrNone )
        {
    	TRACE( Kern::Printf("DMemSpyDriverLogChanThreadAndProcess::GetInfoThread() - END - params read error: %d", r));
        return r;
        }

	r = OpenTempObject( aTid, EThread );
	if  ( r == KErrNone )
		{
		DThread* dThread = (DThread*) TempObject();
  	    NKern::ThreadEnterCS();

        // Get DThread adaptor. This is a means of querying DThread internals
        // without accessing them directly. Takes into account possible differences
        // between compile time and run time
        DMemSpyDriverOSAdaptionDThread& dThreadAdaptor = OSAdaption().DThread();

        // Saved CPU registers for thread. Need to get NThread to do this.
        NThread* nThread = dThreadAdaptor.GetNThread( *dThread );
    	TRACE( Kern::Printf("DMemSpyDriverLogChanThreadAndProcess::GetInfoThread() - getting regs..." ));
        MemSpyDriverUtils::GetThreadRegisters( nThread, params.iCpu );
		
        // Name
    	TRACE( Kern::Printf("DMemSpyDriverLogChanThreadAndProcess::GetInfoThread() - getting full name..." ));
		dThread->FullName( params.iFullName );

        // User framework
    	TRACE( Kern::Printf("DMemSpyDriverLogChanThreadAndProcess::GetInfoThread() - getting allocator..." ));
        params.iAllocator = dThreadAdaptor.GetAllocator( *dThread );

        TRACE( Kern::Printf("DMemSpyDriverLogChanThreadAndProcess::GetInfoThread() - getting scheduler..." ));
        params.iScheduler = dThreadAdaptor.GetActiveScheduler( *dThread );

        // User stack information - rest comes from user side API calls
        TRACE( Kern::Printf("DMemSpyDriverLogChanThreadAndProcess::GetInfoThread() - getting user stack pointer..." ));
        params.iStackInfo.iUserStackPointer = params.iCpu.iRn[ 12 ];

        // Supervisor stack information
        TRACE( Kern::Printf("DMemSpyDriverLogChanThreadAndProcess::GetInfoThread() - getting supervisor stack..." ));
        params.iStackInfo.iSupervisorStackPointer = 0;
        params.iStackInfo.iSupervisorStackHighWatermark = 0;
        params.iStackInfo.iSupervisorStackBase = dThreadAdaptor.GetSupervisorStackBase( *dThread );
        params.iStackInfo.iSupervisorStackSize = dThreadAdaptor.GetSupervisorStackSize( *dThread );
 
        // Write back to user-side
        TRACE( Kern::Printf("DMemSpyDriverLogChanThreadAndProcess::GetInfoThread() - writing to user side..." ));
        r = Kern::ThreadRawWrite( &ClientThread(), aParams, &params, sizeof(TMemSpyDriverInternalThreadInfoParams) );
	    NKern::ThreadLeaveCS();
		CloseTempObject();
		}

    TRACE( Kern::Printf("DMemSpyDriverLogChanThreadAndProcess::GetInfoThread() - END - ret: %d", r));
    return r;
	}


TInt DMemSpyDriverLogChanThreadAndProcess::GetInfoProcess( TUint aPid, TMemSpyDriverProcessInfo* aParams )
	{
    TRACE( Kern::Printf("DMemSpyDriverLogChanThreadAndProcess::GetInfoProcess() - START"));

    TMemSpyDriverProcessInfo params;
    TInt r = Kern::ThreadRawRead( &ClientThread(), aParams, &params, sizeof(TMemSpyDriverProcessInfo) );
    if  ( r != KErrNone )
        {
    	TRACE( Kern::Printf("DMemSpyDriverLogChanThreadAndProcess::GetInfoProcess() - END - params read error: %d", r));
        return r;
        }

	r = OpenTempObject( aPid, EProcess );
	if  ( r == KErrNone )
		{
        DMemSpyDriverOSAdaptionDProcess& processAdaptor = OSAdaption().DProcess();
  
        DProcess* process = (DProcess*) TempObject();
  	    NKern::ThreadEnterCS();
		//
        params.iFlags = processAdaptor.GetFlags( *process );
        params.iGeneration = processAdaptor.GetGeneration( *process );
        params.iSecurityInfo = processAdaptor.GetSecurityInfo( *process );

        // Write back to user-side
        r = Kern::ThreadRawWrite( &ClientThread(), aParams, &params, sizeof(TMemSpyDriverProcessInfo) );
	    NKern::ThreadLeaveCS();
		CloseTempObject();
		}

    TRACE( Kern::Printf("DMemSpyDriverLogChanThreadAndProcess::GetInfoProcess() - END - ret: %d", r));
    return r;
	}









TInt DMemSpyDriverLogChanThreadAndProcess::EndThread( TUint aId, TExitType aType )
    {
    TRACE( Kern::Printf("DMemSpyDriverLogChanThreadAndProcess::EndThread() - START - aId: %d, aType: %d", aId, aType ));

	TInt r = OpenTempObject( aId, EThread );
	if  ( r == KErrNone )
		{
        DThread* thread = (DThread*) TempObject();
        //
        const TInt reason = MapToMemSpyExitReason( aType );
        Kern::ThreadKill( thread, aType, reason, KMemSpyClientPanic );
        //
	    CloseTempObject();
        }
    else
        {
        r = KErrNotFound;
        }

	TRACE( Kern::Printf("DMemSpyDriverLogChanThreadAndProcess::EndThread() - END - r: %d", r));
	return r;
    }






















TInt DMemSpyDriverLogChanThreadAndProcess::OpenThread( TUint aId )
    {
	TRACE( Kern::Printf("DMemSpyDriverLogChanThreadAndProcess::OpenThread() - START - aId: %d", aId));

	TInt r = OpenTempObject( aId, EThread, KMemSpyDriverAllowDeadOpenRequests );
	TRACE( Kern::Printf("DMemSpyDriverLogChanThreadAndProcess::OpenThread() - done open temp object, r: %d", r ));
	if  ( r == KErrNone )
		{
        NKern::ThreadEnterCS();
        DThread* threadToOpen = (DThread*) TempObject();
        TRACE( Kern::Printf("DMemSpyDriverLogChanThreadAndProcess::OpenThread() - thread exit type: %d", threadToOpen->iExitType ));

        TRACE( Kern::Printf("DMemSpyDriverLogChanThreadAndProcess::OpenThread() - making handle..." ));
        r = Kern::MakeHandleAndOpen( &ClientThread(), threadToOpen );
	    TRACE( Kern::Printf("DMemSpyDriverLogChanThreadAndProcess::OpenThread() - handle: %d",r ));
    	NKern::ThreadLeaveCS();

        // Balance reference count, handle is still open and mapped into our process since we opened
        // it above via MakeHandleAndOpen
	    TRACE( Kern::Printf("DMemSpyDriverLogChanThreadAndProcess::OpenThread() - closing temp object..." ));
	    CloseTempObject();
	    TRACE( Kern::Printf("DMemSpyDriverLogChanThreadAndProcess::OpenThread() - closed temp object" ));
        }
    else
        {
        r = KErrNotFound;
        }

	TRACE( Kern::Printf("DMemSpyDriverLogChanThreadAndProcess::OpenThread() - END - r: %d", r));
	return r;
    }


TInt DMemSpyDriverLogChanThreadAndProcess::OpenProcess( TUint aId )
    {
	TRACE( Kern::Printf("DMemSpyDriverLogChanThreadAndProcess::OpenProcess() - START - aId: %d", aId));

	TInt r = OpenTempObject( aId, EProcess, KMemSpyDriverAllowDeadOpenRequests );
	if  ( r == KErrNone )
		{
        NKern::ThreadEnterCS();
        DProcess* processToOpen = (DProcess*) TempObject();
        r = Kern::MakeHandleAndOpen( &ClientThread(), processToOpen );
    	NKern::ThreadLeaveCS();

        // Balance reference count, handle is still open and mapped into our process since we opened
        // it above via MakeHandleAndOpen
	    CloseTempObject();
        }
    else
        {
        r = KErrNotFound;
        }

	TRACE( Kern::Printf("DMemSpyDriverLogChanThreadAndProcess::OpenProcess() - END - r: %d", r));
	return r;
    }





























TInt DMemSpyDriverLogChanThreadAndProcess::SuspendAllThreadsInProcess( TUint aPid )
    {
	TRACE( Kern::Printf( "DMemSpyDriverLogChanThreadAndProcess::SuspendAllThreadsInProcess() - START - aPid: %d", aPid ));

    DMemSpySuspensionManager& susMan = SuspensionManager();
    const TInt err = susMan.SuspendAllThreadsInProcess( aPid, ClientThread() );

	TRACE( Kern::Printf( "DMemSpyDriverLogChanThreadAndProcess::SuspendAllThreadsInProcess() - END - aPid: %d, err: %d", aPid, err ));
	return err;
    }


TInt DMemSpyDriverLogChanThreadAndProcess::ResumeAllThreadsInProcess( TUint aPid )
    {
	TRACE( Kern::Printf( "DMemSpyDriverLogChanThreadAndProcess::ResumeAllThreadsInProcess() - START - aPid: %d", aPid ));

    DMemSpySuspensionManager& susMan = SuspensionManager();
    const TInt err = susMan.ResumeAllThreadsInProcess( aPid, ClientThread() );

	TRACE( Kern::Printf( "DMemSpyDriverLogChanThreadAndProcess::ResumeAllThreadsInProcess() - END - aPid: %d, err: %d", aPid, err ));
	return err;
    }


TInt DMemSpyDriverLogChanThreadAndProcess::GetThreadsForProcess( TUint aPid, TDes8* aBufferSink )
    {
	TRACE( Kern::Printf( "DMemSpyDriverLogChanThreadAndProcess::GetThreadsForProcess() - START - aPid: %d", aPid ) );

    // We open the source thread or process, just to ensure it doesn't die underneath us...
    TInt r = OpenTempObject( aPid, EProcess );
    if  ( r == KErrNone )
		{
		DProcess& process = TempObjectAsProcess();

        // Open stream
        RMemSpyMemStreamWriter stream = OpenXferStream();
        TRACE( Kern::Printf( "DMemSpyDriverLogChanThreadAndProcess::GetThreadsForProcess() - stream remaining: %d", stream.Remaining() ) ); 

        // Save marker pos for the thread count - we'll update it after the loop
        TInt count = 0;
        TInt32* pCountMarkerThread = stream.WriteInt32( 0 );

        DMemSpyDriverOSAdaptionDProcess& processAdaptor = OSAdaption().DProcess();
        SDblQue& threadQueue = processAdaptor.GetThreadQueue( process );
        SDblQueLink* pLink = threadQueue.First();
		while( pLink != & threadQueue.iA && !stream.IsFull() )
			{
			DThread* pT = processAdaptor.GetThread( pLink );
            //
            if  ( pT )
                {
                const TUint tid = OSAdaption().DThread().GetId( *pT );
                TRACE( Kern::Printf( "DMemSpyDriverLogChanThreadAndProcess::GetThreadsForProcess() - id: %d (0x%04x)", tid, tid ) );
                stream.WriteUint32( tid );
                }

            pLink = pLink->iNext;
			++count;
            }
 
        if  ( stream.IsFull() )
            {
            Kern::Printf( "DMemSpyDriverLogChanThreadAndProcess::GetThreadsForProcess() - STREAM FULL - id: %d (0x%04x), thread: %O", process.iId, process.iId, &process );
            }

        // Now write the count
        *pCountMarkerThread = count;

        // Tidy up
        r = stream.WriteAndClose( aBufferSink );
        TRACE( Kern::Printf( "DMemSpyDriverLogChanThreadAndProcess::GetThreadsForProcess() - r: %d, count: %d", r, count ));

        CloseTempObject();
        }

	TRACE( Kern::Printf( "DMemSpyDriverLogChanThreadAndProcess::GetThreadsForProcess() - END - aPid: %d, err: %d", aPid, r ));
	return r;
    }


TInt DMemSpyDriverLogChanThreadAndProcess::SetPriority( TUint aId, TThreadPriority aPriority )
    {
    TRACE( Kern::Printf("DMemSpyDriverLogChanThreadAndProcess::SetPriority(T) - START - aId: %d, aPriority: %d", aId, aPriority ));

	TInt r = OpenTempObject( aId, EThread );
	TRACE( Kern::Printf("DMemSpyDriverLogChanThreadAndProcess::SetPriority(T) - done open temp object, r: %d", r ));
	if  ( r == KErrNone )
		{
        // Map user side thread priority to kernel-side absolute thread priority (typically 0-63)
        const TInt kernelThreadPri = MemSpyDriverUtils::MapToAbsoluteThreadPriority( aPriority );
        TRACE( Kern::Printf("DMemSpyDriverLogChanThreadAndProcess::SetPriority(T) - user: %d, kernel absolute: %d", aPriority, kernelThreadPri ));

        if  ( kernelThreadPri > 0 && kernelThreadPri < KNumPriorities )
            {
            NKern::ThreadEnterCS();
            DThread& thread = TempObjectAsThread();
            r = Kern::SetThreadPriority( kernelThreadPri, &thread );
            TRACE( Kern::Printf("DMemSpyDriverLogChanThreadAndProcess::SetPriority(T) - Kern::SetThreadPriority() returned: %d", r ));
    	    NKern::ThreadLeaveCS();
            }
        else
            {
            // Error
            r = kernelThreadPri;
            }

        CloseTempObject();
        }

    TRACE( Kern::Printf("DMemSpyDriverLogChanThreadAndProcess::SetPriority(T) - END - r: %d", r));
	return r;
    }






TInt DMemSpyDriverLogChanThreadAndProcess::MapToMemSpyExitReason( TExitType aType )
    {
    TInt ret = 0;
    //
    switch( aType )
        {
    default:
    case EExitKill:
        ret = EPanicForcedKill;
        break;
    case EExitTerminate:
        ret = EPanicForcedTerminate;
        break;
    case EExitPanic:
        ret = EPanicForcedPanic;
        break;
        }
    //
    return ret;
    }




















