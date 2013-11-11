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

#include "MemSpyDriverSuspensionManager.h"

// System includes
#include <memspy/driver/memspydriverobjectsshared.h>

// Shared includes
#include "MemSpyDriverUtils.h"
#include "MemSpyDriverDevice.h"
#include "MemSpyDriverOSAdaption.h"

// Constants
const TInt KSuspendResumeCount = 1;



DMemSpySuspensionManager::DMemSpySuspensionManager( DMemSpyDriverDevice& aDevice )
:   iDevice( aDevice )
    {
    }


DMemSpySuspensionManager::~DMemSpySuspensionManager()
	{
	TRACE( Kern::Printf("DMemSpySuspensionManager::~DMemSpySuspensionManager() - START"));

    if  ( iAlreadySuspended )
        {
        TRACE( Kern::Printf("DMemSpySuspensionManager::~DMemSpySuspensionManager() - resuming threads in process with id: %d", iSuspendedProcessId ));
        DoResumeAllThreadsInProcess( iSuspendedProcessId );
        }
    if  ( iTempObj != NULL )
        {
    	Kern::SafeClose( iTempObj, NULL );
        }

    TRACE( Kern::Printf("DMemSpySuspensionManager::~DMemSpySuspensionManager() - END"));
    }


TInt DMemSpySuspensionManager::Construct()
    {
    return KErrNone;
    }







 



TBool DMemSpySuspensionManager::IsSuspended( TUint aPid ) const
    {
    TBool ret = iAlreadySuspended;
    //
    if  ( ret )
        {
        ret = ( aPid == iSuspendedProcessId );
        }
    //
    TRACE( Kern::Printf("DMemSpySuspensionManager::IsSuspended() - iAlreadySuspended: %d, iSuspendedProcessId: %d, aPid: %d, ret: %d", iAlreadySuspended, iSuspendedProcessId, aPid, ret ) );
    return ret;
    }


TBool DMemSpySuspensionManager::IsSuspended( DThread& aThread ) const
    {
    TBool ret = EFalse;
    //
    const TUint tid = iDevice.OSAdaption().DThread().GetId( aThread );
    DProcess* process = iDevice.OSAdaption().DThread().GetOwningProcess( aThread );
    TRACE( Kern::Printf("DMemSpySuspensionManager::IsSuspended() - START - iSuspendedProcessId: %d, aThread: (%d) %O, process: 0x%08x", iSuspendedProcessId, tid, &aThread, process ) );
    //
    if ( process )
        {
        ret = IsSuspended( *process );
        }
    //
    TRACE( Kern::Printf("DMemSpySuspensionManager::IsSuspended() - END - ret: %d", ret ) );
    return ret;
    }


TBool DMemSpySuspensionManager::IsSuspended( DProcess& aProcess ) const
    {
    const TUint pid = iDevice.OSAdaption().DProcess().GetId( aProcess );
    TRACE( Kern::Printf("DMemSpySuspensionManager::IsSuspended() - START - iSuspendedProcessId: %d, aProcess: (%d / %d) %O", iSuspendedProcessId, pid, aProcess.iId, &aProcess ) );
    //
    const TBool ret = IsSuspended( pid );
    //
    TRACE( Kern::Printf("DMemSpySuspensionManager::IsSuspended() - END - ret: %d", ret ) );
    return ret;
    }
















TInt DMemSpySuspensionManager::SuspendAllThreadsInProcess( TUint aPid, DThread& aClientThread )
    {
	// Suspend all threads in the process
	TRACE( Kern::Printf("DMemSpySuspensionManager::SuspendAllThreadsInProcess() - START - id: %8d, iSuspendedProcessId: %8d, iAlreadySuspended: %d", aPid, iSuspendedProcessId, iAlreadySuspended ));
    if  ( iAlreadySuspended && aPid != iSuspendedProcessId )
        {
		Kern::Printf("DMemSpySuspensionManager::SuspendAllThreadsInProcess() - END - trying to suspend multiple processes!");
        MemSpyDriverUtils::PanicThread( aClientThread, EPanicAttemptingToSuspendMultipleProcesses );
        return KErrNone;
        }
    else if ( IsProcessTheClientThread( aPid, aClientThread ) )
        {
		Kern::Printf("DMemSpySuspensionManager::SuspendAllThreadsInProcess() - END - trying to suspend client thread! - request ignored");
        return KErrLocked;
        }
    
	TInt r = KErrNone;
	TRACE( Kern::Printf("DMemSpySuspensionManager::SuspendAllThreadsInProcess - iAlreadySuspended: %d", iAlreadySuspended));
	if  ( !iAlreadySuspended ) 
    	{
    	r = DoSuspendAllThreadsInProcess( aPid, &aClientThread );
    	if (r != KErrNone)
    		{
    		TRACE( Kern::Printf("DMemSpySuspensionManager::SuspendAllThreadsInProcess() - END - process not found") );
    		return r;
    		}

        // To ensure we clean up in case of user-side problem...
        iSuspendedProcessId = aPid;	
        iAlreadySuspended = ETrue;
    	iSuspendCount = 1;
    	}
	else
    	{
    	// Just increment the count
    	++iSuspendCount;
    	}

    TRACE( Kern::Printf("DMemSpySuspensionManager::SuspendAllThreadsInProcess() - END - iSuspendCount: %d, iSuspendedProcessId: %d", iSuspendCount, iSuspendedProcessId ));
	return iSuspendCount;
    }


TInt DMemSpySuspensionManager::ResumeAllThreadsInProcess( TUint aPid, DThread& aClientThread )
    {
	TRACE( Kern::Printf("DMemSpySuspensionManager::ResumeAllThreadsInProcess() - START - id: %8d, iSuspendedProcessId: %8d, iAlreadySuspended: %d", aPid, iSuspendedProcessId, iAlreadySuspended));
	if  ( !iAlreadySuspended )
    	{
    	// Nothing suspended - don't panic
     	TRACE( Kern::Printf("DMemSpySuspensionManager::ResumeAllThreadsInProcess() - END - nothing suspended, ignoring client request"));
       	return KErrNone;
    	}
    else if  ( iAlreadySuspended && aPid != iSuspendedProcessId )
        {
		Kern::Printf("DMemSpySuspensionManager::ResumeAllThreadsInProcess() - END - trying to resume incorrect process!");
        MemSpyDriverUtils::PanicThread( aClientThread, EPanicAttemptingToResumeNonSuspendedProcess );
        return KErrNone;
        }
    else if  ( IsProcessTheClientThread( aPid, aClientThread ) )
        {
		Kern::Printf("DMemSpySuspensionManager::ResumeAllThreadsInProcess() - END - trying to resume client thread! - request ignored");
        return KErrLocked;
        }
    
    TRACE( Kern::Printf("DMemSpySuspensionManager::ResumeAllThreadsInProcess"));

    TInt r = KErrNone;
    if  ( --iSuspendCount <= 0 )
        {
    	r = DoResumeAllThreadsInProcess( aPid, &aClientThread );
    	if (r != KErrNone)
    		{
    		Kern::Printf("DMemSpySuspensionManager::ResumeAllThreadsInProcess() - END - process not found");
    		return r;
    		}

        // No longer need to clean up
        iAlreadySuspended = EFalse;
        iSuspendedProcessId = 0;
        iSuspendCount = 0;
        }
    else
        {
    	// No action needed - we've already decremented the counter
    	}

	TRACE( Kern::Printf("DMemSpySuspensionManager::ResumeAllThreadsInProcess() - END - iSuspendCount: %d", iSuspendCount));
	return iSuspendCount;
    }




























TInt DMemSpySuspensionManager::DoSuspendAllThreadsInProcess( TUint aPid, DThread* /*aClientThread*/ )
    {
	TRACE( Kern::Printf("DMemSpySuspensionManager::DoSuspendAllThreadsInProcess() - START - aPid: %d", aPid));
    TInt r = OpenTempObject( aPid, EProcess );
    if  (r == KErrNone)
    	{
        DProcess* process = (DProcess*) iTempObj;
        TFullName processName;
        process->FullName( processName );
        processName.Append('*');
    
        DObjectCon* container = Kern::Containers()[EThread];
        NKern::ThreadEnterCS();
        container->Wait();
        //
        TFullName result;
#ifdef MCL_FIND_HANDLES
        TFindHandle findHandle;
#else
        TInt findHandle = 0;
#endif
        r = container->FindByFullName( findHandle, processName, result );
        while( r == KErrNone )
            {
            DThread* thread = (DThread*) container->At( findHandle );
            if  ( thread )
                {
                const TUint tid = iDevice.OSAdaption().DThread().GetId( *thread );
                TRACE( Kern::Printf("DMemSpySuspensionManager::DoSuspendAllThreadsInProcess - suspending thread: (%d), %O", tid, thread ));
                //
                Kern::ThreadSuspend( *thread, KSuspendResumeCount );
                }
            //
            r = container->FindByFullName( findHandle, processName, result );
            }
        //
        container->Signal();
        NKern::ThreadLeaveCS();
    
        CloseTempObject();
        r = KErrNone;
    	}

    TRACE( Kern::Printf("DMemSpySuspensionManager::DoSuspendAllThreadsInProcess() - END - ret: %d", r));
    return r;
    }


TInt DMemSpySuspensionManager::DoResumeAllThreadsInProcess( TUint aPid, DThread* aClientThread )
    {
	TRACE( Kern::Printf("DMemSpySuspensionManager::DoResumeAllThreadsInProcess() - START - aPid: %d", aPid));
    TInt r = OpenTempObject( aPid, EProcess );
    if  (r == KErrNone)
    	{
        // Check that this process is suspended
        DProcess* process = (DProcess*) iTempObj;
        const TUint pid = iDevice.OSAdaption().DProcess().GetId( *process );
        if  ( !aClientThread  || ( aClientThread && CheckProcessSuspended( pid, *aClientThread ) ) )
            {
            // Resume all threads in the process
            TFullName processName;
            process->FullName( processName );
            processName.Append('*');
    
            DObjectCon* container = Kern::Containers()[EThread];
            NKern::ThreadEnterCS();
            container->Wait();
            //
            TFullName result;
    #ifdef MCL_FIND_HANDLES
            TFindHandle findHandle;
    #else
            TInt findHandle = 0;
    #endif
            r = container->FindByFullName( findHandle, processName, result );
            while( r == KErrNone )
                {
                DThread* thread = (DThread*) container->At( findHandle );
                TRACE( Kern::Printf("DMemSpySuspensionManager::DoResumeAllThreadsInProcess - resuming thread: %lS", &result));
                //
                Kern::ThreadResume(*thread);
                r = container->FindByFullName( findHandle, processName, result );
                }
            //
            container->Signal();
            NKern::ThreadLeaveCS();
    
            r = KErrNone;
            }
        else
            {
            TRACE( Kern::Printf("DMemSpySuspensionManager::DoResumeAllThreadsInProcess - parent process not suspended => KErrAccessDenied"));
            r = KErrAccessDenied;
            }

        CloseTempObject();
        }

    TRACE( Kern::Printf("DMemSpySuspensionManager::DoResumeAllThreadsInProcess() - END - ret: %d", r));
    return r;
    }

















TBool DMemSpySuspensionManager::IsProcessTheClientThread( TUint aPid, DThread& aClientThread ) const
    {
    TBool ret = EFalse;
    DProcess& clientProcess = *iDevice.OSAdaption().DThread().GetOwningProcess( aClientThread );
    const TUint pid = iDevice.OSAdaption().DProcess().GetId( clientProcess );
    if  ( pid == aPid )
        {
        ret = ETrue;
        }
    //
    return ret;
    }
    

TBool DMemSpySuspensionManager::CheckProcessSuspended( TUint aExpectedPid, DThread& aClientThread ) const
    {
    TBool suspended = ETrue;
    //
    if  ( !iAlreadySuspended )
        {
        MemSpyDriverUtils::PanicThread( aClientThread, EPanicThreadsInProcessNotSuspended );
        }
    else if ( aExpectedPid != iSuspendedProcessId )
        {
        MemSpyDriverUtils::PanicThread( aClientThread, EPanicWrongProcessSuspended );
        }
    //
    return suspended;
    }



















TInt DMemSpySuspensionManager::OpenTempObject(TUint aId, TObjectType aType)
	{
	__ASSERT_DEBUG( aType == EProcess || aType == EThread, MemSpyDriverUtils::Fault( __LINE__ ) );
	__ASSERT_DEBUG( !iTempObj, MemSpyDriverUtils::Fault( __LINE__ ) );

	TRACE( Kern::Printf("DMemSpySuspensionManager::OpenTempObject() - START - aId: %d, aType: %d", aId, aType ));
	DObjectCon* pC = Kern::Containers()[aType];
	NKern::ThreadEnterCS();
	pC->Wait();
	iTempObj = (aType == EProcess) ? (DObject*)Kern::ProcessFromId(aId) : (DObject*)Kern::ThreadFromId(aId);
	TRACE( Kern::Printf("DMemSpySuspensionManager::OpenTempObject() - kernel obj from id returned: 0x%08x %O", iTempObj, iTempObj ));
    //
    TBool openedOkay = EFalse;
    TInt r = KErrNone;
    //
	if ( iTempObj )
        {
		r = iTempObj->Open();
	    TRACE( Kern::Printf("DMemSpySuspensionManager::OpenTempObject() - open returned: %d", r ));
		//
        if  ( r == KErrNone )
            {
            openedOkay = ETrue;
            if  ( aType == EProcess )
                {
                // Check the process is still alive
                DProcess* process = (DProcess*) iTempObj;
                const TExitType exitType = iDevice.OSAdaption().DProcess().GetExitType( *process );
                if  ( exitType != EExitPending )
					{
                    r = KErrDied;
	                TRACE( Kern::Printf("DMemSpySuspensionManager::OpenTempObject() - PROCESS IS DEAD!" ));
					}
                }
            else if (aType == EThread )
                {
                // Check the thread is still alive
                DThread* thread = (DThread*) iTempObj;
                const TExitType exitType = iDevice.OSAdaption().DThread().GetExitType( *thread );
                if  ( exitType != EExitPending )
                    {
                    r = KErrDied;
	                TRACE( Kern::Printf("DMemSpySuspensionManager::OpenTempObject() - THREAD IS DEAD!" ));
                    }
                }
            }

        }
    else if ( ! iTempObj )
        {
        r = KErrNotFound;
	    TRACE( Kern::Printf("DMemSpySuspensionManager::OpenTempObject() - ENTITY NOT FOUND!" ));
        }
    //
	pC->Signal();
	TRACE( Kern::Printf("DMemSpySuspensionManager::OpenTempObject() - signalled container..." ));
	NKern::ThreadLeaveCS();
	TRACE( Kern::Printf("DMemSpySuspensionManager::OpenTempObject() - left CS..." ));
    //
    if  ( r != KErrNone && iTempObj )
        {
	    TRACE( Kern::Printf("DMemSpySuspensionManager::OpenTempObject() - ERROR CASE - closing temp object (r: %d, openedOkay: %d)...", r, openedOkay ));
        //
        if ( openedOkay )
            {
            CloseTempObject();
            }
        else
            {
            NKern::SafeSwap( NULL, (TAny*&) iTempObj );
            }
        //
	    TRACE( Kern::Printf("DMemSpySuspensionManager::OpenTempObject() - ERROR CASE - closed temp object" ));
        }
    //
	TRACE( Kern::Printf("DMemSpySuspensionManager::OpenTempObject() - END - r: %d", r ));
	return r;
	}


void DMemSpySuspensionManager::CloseTempObject()
	{
	TRACE( Kern::Printf("DMemSpySuspensionManager::CloseTempObject() - START - iTempObj: 0x%08x %O", iTempObj, iTempObj ));

	__ASSERT_DEBUG( iTempObj, MemSpyDriverUtils::Fault( __LINE__ ) );
    if  ( iTempObj )
        {
		NKern::ThreadEnterCS();
	    Kern::SafeClose( iTempObj, NULL );
		NKern::ThreadLeaveCS();
        }

    TRACE( Kern::Printf("DMemSpySuspensionManager::CloseTempObject() - END" ));
	}


