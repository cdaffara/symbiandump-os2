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


#ifndef MEMSPYENGINECLIENTINTERFACE_H
#define MEMSPYENGINECLIENTINTERFACE_H


// System includes
#include <e32base.h>
#include <u32std.h>

// Constants
const TInt KMemSpyClientServerVersion           = 2;
const TInt KMemSpyListInfoForAllThreads         = -1;

/**
 * Special constant to be used to indicate kernel thread id
 * when performing kernel-specific thread agnostic operations.
 *
 * Note that the kernel thread is not supported for all thread-specific
 * operation types. 
 *
 * Use this value to indicate that the kernel thread is the target
 * thread when calling the thread-specific overload of PerformOperation().
 */
const TUint32 KMemSpyClientServerThreadIdKernel = KMaxTUint32;

// Internal constants
const TInt KMemSpyOpFlagsTypeMask               = 0x0000FFFF;
const TInt KMemSpyOpFlagsInclusionMask          = 0xFFFF0000;
const TInt KMemSpyOpFlagsIncludesThreadId       = 0x00010000;
const TInt KMemSpyOpFlagsIncludesThreadName     = 0x00020000;
const TInt KMemSpyOpFlagsAsyncOperation         = 0x00040000;
// Literal constants
_LIT( KMemSpyServerName, "MemSpyServer" );
_LIT( KMemSpyProcessName0, "MemSpyServer.exe" );
_LIT( KMemSpyProcessName1, "MemSpyUI.exe" );
//_LIT( KMemSpyProcessName2, "MemSpyConsole.exe" );

//
// Supported MemSpy operation types
//
// Operations which can only be applied to a specific thread are
// prefixed by [TS] - i.e. it is permitted to use the thread-specific 
// overload of PerformOperation() for this operation type.
//
// Operations that can be applied only to the entire device are
// prefixed by [ED] - i.e. it is permitted to use the 
// thread-agnostic overload of PerformOperation() for this operation type.
//
// Operations that support kernel-thread-specific requests
// are prefixed by [KS] - i.e. it is permitted to use the 
// thread-specific overload of PerformOperation() and the thread
// identification parameter should correspond to KMemSpyClientServerThreadIdKernel.
// NB: The overload of PerformOperation() which supports a descriptor cannot
// be used.
//
// Operations that are not relevant to a thread are prefixed
// by [NT] - i.e. use the thread-agnostic overload of
// PerformOperation()
//
// All other op codes are deemed internal and are not to be
// used except via provided APIs.
//
enum TMemSpyClientServerOp
    {
    /**
     * [INTERNAL REQUEST]
     */
    EMemSpyClientServerOpMarkerFirst = 0,

    /**
     * [TS][ED]
     * Show basic summary information about a thread/process
     */
    EMemSpyClientServerOpSummaryInfo = EMemSpyClientServerOpMarkerFirst,

    /**
     * [TS][ED]
     * Show detailed summary information about a thread/process
     */
    EMemSpyClientServerOpSummaryInfoDetailed,

    /**
     * [TS][ED][KS]
     * Show heap information & statistics for a user-side thread
     */
    EMemSpyClientServerOpHeapInfo,

    /**
     * [TS][ED]
     * Prvide heap cell listings for a thread
     */
    EMemSpyClientServerOpHeapCellListing,

    /**
     * [TS][ED][KS]
     * Dump a thread's heap
     */
    EMemSpyClientServerOpHeapData,

    /**
     * [TS][ED]
     * Show summary information about a thread's stack
     */
    EMemSpyClientServerOpStackInfo,

    /**
     * [TS][ED]
     * Dump a thread's user-side stack
     */
    EMemSpyClientServerOpStackDataUser,

    /**
     * [TS][ED]
     * Dump a thread's kernel-side stack
     */
    EMemSpyClientServerOpStackDataKernel,

    /**
     * [TS][ED]
     * List all open files in the OS or then the open files
     * for a given thread.
     */
    EMemSpyClientServerOpOpenFiles,
    
    /**
     * [INTERNAL REQUEST]
     */
    EMemSpyClientServerOpMarkerThreadAgnosticFirst,

    /**
     * [ED]
     * Provide compact heap information for the entire device, which
     * can be compared with other similar listings using Heap Analyser.
     */
    EMemSpyClientServerOpHeapInfoCompact = EMemSpyClientServerOpMarkerThreadAgnosticFirst,

    /**
     * [ED]
     * Show compact stack information for the device
     */
    EMemSpyClientServerOpStackInfoCompact,

    /**
     * [NT]
     * Start the system wide memory tracker (SWMT) running, so that when
     * the timer expires, the system characteristics will be sampled
     * and any changes notified. NB: Starting the SWMT running for the first time
     * will cause an initial preparatory cycle to be established. Future updates
     * will then follow when the timer tick period expires
     */
    EMemSpyClientServerOpSystemWideMemoryTrackingTimerStart,

    /** 
     * [NT]
     * Stop the system wide memory tracker. No futher automatic timer-based
     * SWMT cycles will be performed unless "Force update" is utilised or the timer
     * is started again.
     */
    EMemSpyClientServerOpSystemWideMemoryTrackingTimerStop,

    /** 
     * [NT]
     * Reset the SWMT so that all existing cycles are discarded (which can free
     * quite a lot of memory). All operations are cancelled, the SWMT will be idle.
     * 
     * If performing manual "force updates" to the SWMT, then the SWMT should be reset
     * before starting a new test run. For timer-based SWMT sampling, this operation
     * is performed internally by MemSpy and therefore is redundant in that situation.
     */
    EMemSpyClientServerOpSystemWideMemoryTrackingReset,

    /**
     * [NT]
     * Force the system wide memory tracker (internal to memspy) to
     * perform a refresh
     */
    EMemSpyClientServerOpSystemWideMemoryTrackingForceUpdate,

    /**
     * [INTERNAL REQUEST]
     */
    EMemSpyClientServerOpSystemWideMemoryTrackingTimerPeriodSet,

    /**
     * [INTERNAL REQUEST]
     */
    EMemSpyClientServerOpSwitchOutputSinkFile,

    /**
     * [INTERNAL REQUEST]
     */
    EMemSpyClientServerOpSwitchOutputSinkTrace,

    /**
     * [INTERNAL REQUEST]
     */
    EMemSpyClientServerOpEnumerateKernelContainer,

    /**
     * [INTERNAL REQUEST]
     */
    EMemSpyClientServerOpEnumerateKernelContainerAll,

    /**
     * [INTERNAL REQUEST]
     */
    EMemSpyClientServerOpBitmapsSave,

    /**
     * [INTERNAL REQUEST]
     */
    EMemSpyClientServerOpDisableAknIconCache,

    /**
     * [INTERNAL REQUEST]
     */
    EMemSpyClientServerOpSendToBackground,

    /**
     * [INTERNAL REQUEST]
     */
    EMemSpyClientServerOpBringToForeground,

    /**
     * [INTERNAL REQUEST]
     */
    EMemSpyClientServerOpExit,

    /**
     * [INTERNAL REQUEST]
     */
    EMemSpyClientServerOpSystemWideMemoryTrackingCategoriesSet,

    /**
     * [INTERNAL REQUEST]
     */
    EMemSpyClientServerOpSystemWideMemoryTrackingThreadNameFilterSet,

    /**
     * [INTERNAL REQUEST]
     */
    EMemSpyClientServerOpSystemWideMemoryTrackingHeapDumpSet,
        
    /**
     * [INTERNAL REQUEST]
     */
    EMemSpyClientServerOpSystemWideMemoryTrackingModeSet,        
    
    /**
	 * [INTERNAL REQUEST]
	 */
	EMemSpyClientServerOpMarkerUiFirst,
	
	EMemSpyClientServerOpGetProcessCount = EMemSpyClientServerOpMarkerUiFirst,
	
	EMemSpyClientServerOpGetProcesses,
	
	EMemSpyClienServerOpGetProcessIdByName,
	
	EMemSpyClientServerOpGetThreadCount,
	
	EMemSpyClientServerOpGetThreads,
	
	EMemSpyClientServerOpSetThreadPriority,
	
	EMemSpyClientServerOpProcessSystemPermanentOrCritical,
	
	EMemSpyClientServerOpEndProcess,
	
	EMemSpyClientServerOpSwitchToProcess,
	
	//Threads operations
	EMemSpyClientServerOpThreadSystemPermanentOrCritical,
	
	EMemSpyClientServerOpEndThread,
	
	EMemSpyClientServerOpSwitchToThread,
	
	EMemSpyClientServerOpGetInfoItemType,
	
	EMemSpyClientServerOpGetProcessIdByThreadId,
	
	EMemSpyClientServerOpGetThreadInfoItemsCount,
	
	EMemSpyClientServerOpGetThreadInfoItems,
	
	EMemSpyClientServerOpOutputInfoHandles,
	
	EMemSpyClientServerOpOutputAOList,
		
	
	//Kernel Objects operations
	EMemSpyClientServerOpGetKernelObjectCount,
	
	EMemSpyClientServerOpGetKernelObjects,
	
	EMemSpyClientServerOpGetKernelObjectItemCount,
	
	EMemSpyClientServerOpGetKernelObjectItems,
	
	EMemSpyClientServerOpGetHeap,
	
	EMemSpyClientServerOpGetMemoryTrackingCycleCount,
	
	EMemSpyClientServerOpGetMemoryTrackingCycles,
	
	EMemSpyClientServerOpGetMemoryTrackingMdcaCount,
	/**
	 * [INTERNAL REQUEST]
	 * Register for notifications of device wide operation progress.
	 */
	EMemSpyClientServerOpNotifyDeviceWideOperationProgress,
	
	/**
	 * [INTERNAL REQUEST]
	 * Cancel current device wide operation
	 */
	EMemSpyClientServerOpCancelDeviceWideOperation,	
	
	EMemSpyClientServerOpOutputAllContainerContents,
	    
	EMemSpyClientServerOpDumpKernelHeap,
	
	EMemSpyClientServerOpSetSwmtAutoStartProcessList,
		
	EMemSpyClientServerOpGetOutputSink,
	
	/**
	 * [INTERNAL REQUEST]
	 * Check if system wide memory tracking timer is running.
	 */
	EMemSpyClientServerOpIsSwmtRunning,
	
	EMemSpyClientServerOpSystemWideMemoryTrackingTimerPeriodGet,

	EMemSpyClientServerOpSystemWideMemoryTrackingThreadNameFilterGet,
    
    EMemSpyClientServerOpSystemWideMemoryTrackingCategoriesGet,
    
    EMemSpyClientServerOpSystemWideMemoryTrackingHeapDumpGet,

    EMemSpyClientServerOpSystemWideMemoryTrackingModeGet,   
	
    //Servers methods
	EMemSpyClientServerOpGetServerCount,
	
	EMemSpyClientServerOpGetServers,
	
	EMemSpyClientServerOpGetSortedServers,
	
	EMemSpyClientServerOpServerListOutputGeneric,
	
	EMemSpyClientServerOpGetEComCategoryCount,
	    
	EMemSpyClientServerOpGetEComCategories,
	
	EMemSpyClientServerOpGetEComInterfaceCount,
	        
	EMemSpyClientServerOpGetEComInterfaces,
	
	EMemSpyClientServerOpGetEComImplementationCount,
	            
	EMemSpyClientServerOpGetEComImplementations,
	
	EMemSpyClientServerOpGetWindowGroupCount,
	    
	EMemSpyClientServerOpGetWindowGroups,
	
	EMemSpyClientServerOpMarkerUiLast,		
	
	/**
	 * [INTERNAL REQUEST]
	 */
    EMemSpyClientServerOpMarkerLast,
    };


enum TMemSpyEndType
	{
	ETerminate,
	EKill,
	EPanic
	};





NONSHARABLE_CLASS( RMemSpyEngineClientInterface ) : public RSessionBase
    {
public:
    inline RMemSpyEngineClientInterface();
    inline TInt Connect();
    inline void Close();

public: // API

    /**
     * Thread-agnostic request function. Performs the operation in the context of
     * the entire device.
     *
     * I.e. can only be used for [NT] or [ED] requests.
     */
    inline TInt PerformOperation( TMemSpyClientServerOp aOperation );

    /**
     * Thread-specific request functions. Perform the requested operation in the context
     * of the specified thread id, or (possibly partial) thread name.
     *
     * To perform the operation in the context of the current thread, invoke the
     * function as follows:
     *
     *      PerformOperation( <...>, RThread().Id() );
     *
     * I.e. can only be used for [TS] requests.
     */
    inline TInt PerformOperation( TMemSpyClientServerOp aOperation, TThreadId aId );
    inline TInt PerformOperation( TMemSpyClientServerOp aOperation, const TDesC& aThreadName );

    /**
     * General functions to switch MemSpy's output mode.
     */
    inline TInt SwitchOutputModeFile();
    inline TInt SwitchOutputModeTrace();

    /**
     * Configure the system wide memory tracker timer interval (for automatic timer-based polling).
     * Change will take effect next time SWMT is started.
     */
    inline TInt SystemWideMemoryTrackerTimerIntervalSet( TInt aInterval );

    /**
     * Configure the system wide memory tracker categories.
     * Change will take effect next time SWMT is started.
     */
    inline TInt SystemWideMemoryTrackerCategoriesSet( TInt aCategories );

    /**
     * Configure the system wide memory tracker for filtering user heaps by thread name.
     * Change will take effect next time SWMT is started.
     */
    inline TInt SystemWideMemoryTrackerThreadFilterSet( const TDesC& aFilter );
    
    /**
     * Configure the system wide memory tracker for enabling/disabling Heap dumps during SWMT.
     * Change will take effect next time SWMT is started.
     */
    inline TInt SystemWideMemoryTrackerHeapDumpSet( TBool aDumpEnabled );
    
    /**
     * List the contents of a specific kernel container. See TObjectType for types.
     */
    inline TInt EnumerateKernelContainer( TObjectType aType );

    /**
     * List the contents of all kernel containers
     */
    inline TInt EnumerateKernelContainerAll();

    /**
     * Save bitmaps to memory card
     */
    inline TInt SaveAllBitmaps();

    /**
     * Disable AknIcon caching
     */
    inline TInt DisableAknIconCache();

    /**
     * Send MemSpy to the background/bring to foreground
     */
    inline TInt SendToBackground();
    inline TInt BringToForeground();

    /**
     * Exit MemSpy
     */
    inline TInt Exit();
    };














inline RMemSpyEngineClientInterface::RMemSpyEngineClientInterface()
    {
    }


inline TInt RMemSpyEngineClientInterface::Connect()
    {
    TVersion version( KMemSpyClientServerVersion, 0, 0 );
    return CreateSession( KMemSpyServerName, version );
    }


inline void RMemSpyEngineClientInterface::Close()
    {
    RSessionBase::Close();
    }


inline TInt RMemSpyEngineClientInterface::PerformOperation( TMemSpyClientServerOp aOperation )
    {
    TInt error = KErrNotReady;
    if  ( Handle() != KNullHandle )
        {
        const TInt opCode = aOperation;
        const TIpcArgs args( KMemSpyListInfoForAllThreads );
        //
        error = SendReceive( opCode, args );
        }
    return error;
    }


inline TInt RMemSpyEngineClientInterface::PerformOperation( TMemSpyClientServerOp aOperation, TThreadId aId )
    {
    TInt error = KErrNotReady;
    if  ( Handle() != KNullHandle )
        {
        const TInt opCode = aOperation | KMemSpyOpFlagsIncludesThreadId;
        const TIpcArgs args( aId );
        //
        error = SendReceive( opCode, args );
        }
    return error;
    }


inline TInt RMemSpyEngineClientInterface::PerformOperation( TMemSpyClientServerOp aOperation, const TDesC& aThreadName )
    {
    TInt error = KErrNotReady;
    if  ( Handle() != KNullHandle )
        {
        const TInt opCode = aOperation | KMemSpyOpFlagsIncludesThreadName;
        const TIpcArgs args( &aThreadName );
        //
        error = SendReceive( opCode, args );
        }
    return error;
    }






inline TInt RMemSpyEngineClientInterface::SwitchOutputModeFile()
    {
    TInt error = KErrNotReady;
    if  ( Handle() != KNullHandle )
        {
        const TInt opCode = EMemSpyClientServerOpSwitchOutputSinkFile;
        error = SendReceive( opCode );
        }
    return error;
    }


inline TInt RMemSpyEngineClientInterface::SwitchOutputModeTrace()
    {
    TInt error = KErrNotReady;
    if  ( Handle() != KNullHandle )
        {
        const TInt opCode = EMemSpyClientServerOpSwitchOutputSinkTrace;
        error = SendReceive( opCode );
        }
    return error;
    }








inline TInt RMemSpyEngineClientInterface::SystemWideMemoryTrackerTimerIntervalSet( TInt aInterval )
    {
    TInt error = KErrNotReady;
    if  ( Handle() != KNullHandle )
        {
        const TInt opCode = EMemSpyClientServerOpSystemWideMemoryTrackingTimerPeriodSet;
        const TIpcArgs args( aInterval );
        //
        error = SendReceive( opCode, args );
        }
    return error;
    }


inline TInt RMemSpyEngineClientInterface::SystemWideMemoryTrackerCategoriesSet( TInt aCategories )
    {
    TInt error = KErrNotReady;
    if  ( Handle() != KNullHandle )
        {
        const TInt opCode = EMemSpyClientServerOpSystemWideMemoryTrackingCategoriesSet;
        const TIpcArgs args( aCategories );
        //
        error = SendReceive( opCode, args );
        }
    return error;
    }


inline TInt RMemSpyEngineClientInterface::SystemWideMemoryTrackerThreadFilterSet( const TDesC& aFilter )
    {
    TInt error = KErrNotReady;
    if  ( Handle() != KNullHandle )
        {
        const TInt opCode = EMemSpyClientServerOpSystemWideMemoryTrackingThreadNameFilterSet;
        TIpcArgs args( &aFilter );
        error = SendReceive( opCode, args );
        }
    return error;
    }


inline TInt RMemSpyEngineClientInterface::SystemWideMemoryTrackerHeapDumpSet( TBool aDumpEnabled  )
    {
    TInt error = KErrNotReady;
    if  ( Handle() != KNullHandle )
        {
        const TInt opCode = EMemSpyClientServerOpSystemWideMemoryTrackingHeapDumpSet;
        const TIpcArgs args( aDumpEnabled );
        error = SendReceive( opCode, args );
        }
    return error;
    }


inline TInt RMemSpyEngineClientInterface::EnumerateKernelContainer( TObjectType aType )
    {
    TInt error = KErrNotReady;
    if  ( Handle() != KNullHandle )
        {
        const TInt opCode = EMemSpyClientServerOpEnumerateKernelContainer;
        const TIpcArgs args( aType );
        //
        error = SendReceive( opCode, args );
        }
    return error;
    }


inline TInt RMemSpyEngineClientInterface::EnumerateKernelContainerAll()
    {
    TInt error = KErrNotReady;
    if  ( Handle() != KNullHandle )
        {
        const TInt opCode = EMemSpyClientServerOpEnumerateKernelContainerAll;
        error = SendReceive( opCode );
        }
    return error;
    }


inline TInt RMemSpyEngineClientInterface::SaveAllBitmaps()
    {
    TInt error = KErrNotReady;
    if  ( Handle() != KNullHandle )
        {
        const TInt opCode = EMemSpyClientServerOpBitmapsSave;
        error = SendReceive( opCode );
        }
    return error;
    }


inline TInt RMemSpyEngineClientInterface::DisableAknIconCache()
    {
    TInt error = KErrNotReady;
    if  ( Handle() != KNullHandle )
        {
        const TInt opCode = EMemSpyClientServerOpDisableAknIconCache;
        error = SendReceive( opCode );
        }
    return error;
    }


inline TInt RMemSpyEngineClientInterface::SendToBackground()
    {
    TInt error = KErrNotReady;
    if  ( Handle() != KNullHandle )
        {
        const TInt opCode = EMemSpyClientServerOpSendToBackground;
        error = SendReceive( opCode );
        }
    return error;
    }


inline TInt RMemSpyEngineClientInterface::BringToForeground()
    {
    TInt error = KErrNotReady;
    if  ( Handle() != KNullHandle )
        {
        const TInt opCode = EMemSpyClientServerOpBringToForeground;
        error = SendReceive( opCode );
        }
    return error;
    }


inline TInt RMemSpyEngineClientInterface::Exit()
    {
    TInt error = KErrNotReady;
    if  ( Handle() != KNullHandle )
        {
        const TInt opCode = EMemSpyClientServerOpExit;
        error = SendReceive( opCode );
        }
    return error;
    }


#endif
