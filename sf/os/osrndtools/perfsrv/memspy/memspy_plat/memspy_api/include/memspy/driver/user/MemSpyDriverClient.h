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

#ifndef MEMSPYDRIVERCLIENT_H
#define MEMSPYDRIVERCLIENT_H

// System includes
#include <e32base.h>
#include <e32std.h>
#include <e32const.h>

// User includes
#include <memspy/driver/memspydriverstreamreader.h>
#include <memspy/driver/memspydriverobjectsshared.h>
#include <memspy/driver/memspydriverenumerationsshared.h>


class RMemSpyDriverClient : public RBusLogicalChannel
    {
public: // GENERAL API

    /**
     *
     */
    IMPORT_C TInt Open();

    /**
     *
     */
    IMPORT_C void Close();

    /**
     *
     */
    IMPORT_C void GetVersion( TVersion& aVersion );


public: // RAW MEMORY

    /**
     *
     */
    IMPORT_C TInt ReadMemory( TUint aTid, TLinAddr aSrc, TDes8& aDest );


public: // CODE SEGS

    /**
     *
     */
    IMPORT_C TInt GetCodeSegs( TAny** aHandleArray, TInt& aHandleCount, TBool aOnlyRamLoaded = EFalse );

    /**
     *
     */
    IMPORT_C TInt GetCodeSegs( TUint aPid, TAny** aHandleArray, TInt& aHandleCount );

    /**
     *
     */
    IMPORT_C TInt GetCodeSegInfo( TAny* aHandle, TUint aPid, TMemSpyDriverCodeSegInfo& aInfo );


public: // CHUNKS

    /**
     *
     */
    IMPORT_C TInt GetChunkHandles( TAny** aHandleArray, TInt& aHandleCount );

    /**
     *
     */
    IMPORT_C TInt GetChunkInfo( TAny* aHandle, TMemSpyDriverChunkInfo& aInfo );

    /**
     *
     */
    IMPORT_C TInt GetChunkInfo( TAny* aHandle, TMemSpyDriverChunkInfoWithoutName& aInfo );

    /**
     *
     */
    IMPORT_C TInt GetChunkHandlesForProcess( TUint aPid, TAny** aHandleArray, TInt& aHandleCount );

    /**
     *
     */
    IMPORT_C TInt GetChunkHandlesForThread( TUint aTid, TAny** aHandleArray, TInt& aHandleCount );


public: // THREAD & PROCESS 

    /**
     *
     */
    IMPORT_C TInt GetThreadInfo( TUint aTid, TMemSpyDriverThreadInfo& aInfo );

    /**
     *
     */
    IMPORT_C TInt GetProcessInfo( TUint aPid, TMemSpyDriverProcessInfo& aInfo );

    /**
     *
     */
    IMPORT_C TInt ProcessThreadsSuspend( TUint aPid );

    /**
     *
     */
    IMPORT_C TInt ProcessThreadsResume( TUint aPid );

    /**
     *
     */
    IMPORT_C TInt ThreadEnd( TUint aTid, TExitType aType );

    /**
     *
     */
    IMPORT_C TInt OpenThread( TUint aId, RThread& aThread );

    /**
     *
     */
    IMPORT_C TInt OpenProcess( TUint aId, RProcess& aProcess );

    /**
     *
     */
    IMPORT_C void GetThreadsL( const TProcessId& aId, RArray<TThreadId>& aThreads );

    /**
     *
     */
    IMPORT_C TInt SetPriority( TUint aId, TThreadPriority aPriority );

    
public: // HEAP KERNEL COPY

    /**
     * Creates a copy of the kernel heap so long running operations (like dumping the heap data)
     * don't need to lock the kernel heap.
     * 
     * At most one copy of the kernel heap is held at a time. Each time this function is called the 
     * previous copy, if any, is erased.
     */
    IMPORT_C TInt CopyHeapDataKernel();
    /**
     * Frees the copy made using CopyHeapDataKernel() 
     */
    IMPORT_C TInt FreeHeapDataKernel();
    
public: // HEAP INFO

    /**
     *
     */
    IMPORT_C TInt GetHeapInfoUser( TMemSpyHeapInfo& aInfo, TUint aTid );
    IMPORT_C TInt GetHeapInfoUser( TMemSpyHeapInfo& aInfo, TUint aTid, RArray<TMemSpyDriverFreeCell>& aFreeCells );
    IMPORT_C TInt GetHeapInfoUser(TMemSpyHeapInfo& aInfo, TUint aTid, RArray<TMemSpyDriverCell>& aCells, TBool aCollectAllocatedCellsAsWellAsFree);

    /**
     *
     */
    IMPORT_C TInt GetHeapInfoKernel( TMemSpyHeapInfo& aInfo );
    IMPORT_C TInt GetHeapInfoKernel( TMemSpyHeapInfo& aInfo, RArray<TMemSpyDriverFreeCell>& aFreeCells );
    IMPORT_C TInt GetHeapInfoKernel( TMemSpyHeapInfo& aInfo, RArray<TMemSpyDriverCell>& aCells, TBool aCollectAllocatedCellsAsWellAsFree);
    IMPORT_C TInt GetHeapInfoKernel( TMemSpyHeapInfo& aInfo, RArray<TMemSpyDriverCell>& aCells, TBool aCollectAllocatedCellsAsWellAsFree, TBool aUseKernelCopy);
    

public: // HEAP DATA

    /**
     * Get free cell data. If aFreeCellChecksum is 0, then no checksum comparison is performed.
     */
    IMPORT_C TInt GetHeapData( TUint aTid, TUint32 aFreeCellChecksum, TDes8& aDest, TUint& aReadAddress, TUint& aAmountRemaining );

    /**
     *
     */
    IMPORT_C TInt GetHeapDataNext( TUint aTid, TDes8& aDest, TUint& aReadAddress, TUint& aAmountRemaining );

    /**
     * Get free cell data. If aFreeCellChecksum is 0, then no checksum comparison is performed.
     * @pre must be called after CopyHeapDataKernel()    
     */
    IMPORT_C TInt GetHeapDataKernel( TUint aTid, TDes8& aDest, TUint& aReadAddress, TUint& aAmountRemaining );

    /**
     * @pre must be called after GetHeapDataKernelNext()
     */
    IMPORT_C TInt GetHeapDataKernelNext( TUint aTid, TDes8& aDest, TUint& aReadAddress, TUint& aAmountRemaining );
        

public: // HEAP WALK

    /**
     *
     */
    IMPORT_C TInt WalkHeapInit( TUint aTid );

    /**
     *
     */
    IMPORT_C TInt WalkHeapNextCell( TUint aTid, TMemSpyDriverCellType& aCellType, TAny*& aCellAddress, TInt& aLength, TInt& aNestingLevel, TInt& aAllocNumber, TInt& aCellheaderSize, TAny*& aCellPayloadAddress );

    /**
     *
     */
    IMPORT_C TInt WalkHeapReadCellData( TAny* aCellAddress, TDes8& aDest, TInt aReadLen );

    /**
     *
     */
    IMPORT_C TInt WalkHeapGetCellInfo( TAny*& aCellAddress, TMemSpyDriverCellType& aCellType, TInt& aLength, TInt& aNestingLevel, TInt& aAllocNumber, TInt& aCellheaderSize, TAny*& aCellPayloadAddress );

    /**
     *
     */
    IMPORT_C void WalkHeapClose();


public: // STACK RELATED

    /**
     *
     */
    IMPORT_C TInt GetStackInfo( TUint aTid, TMemSpyDriverStackInfo& aInfo );

    /**
     *
     */
    IMPORT_C TInt GetStackData( TUint aTid, TDes8& aDest, TUint& aAmountRemaining, TMemSpyDriverDomainType aDomain = EMemSpyDriverDomainUser, TBool aEntireStack = ETrue );

    /**
     *
     */
    IMPORT_C TInt GetStackDataNext( TUint aTid, TDes8& aDest, TUint& aAmountRemaining, TMemSpyDriverDomainType aDomain = EMemSpyDriverDomainUser, TBool aEntireStack = ETrue );


public: // EVENT MONITOR

    /**
     * @returns TInt handle
     */
    IMPORT_C TInt EventMonitorOpen( TUint& aHandle );

    /**
     *
     */
    IMPORT_C TInt EventMonitorClose( TUint aHandle );

    /**
     *
     */
    IMPORT_C void EventMonitorNotify( TUint aHandle, TRequestStatus& aStatus, TUint& aContext );

    /**
     *
     */
    IMPORT_C void EventMonitorNotifyCancel( TUint aHandle );


public: // PROCESS INSPECTION

    /**
     *
     */
    IMPORT_C TInt ProcessInspectionOpen( TUint aPid );

    /**
     *
     */
    IMPORT_C TInt ProcessInspectionClose( TUint aPid );

    /**
     *
     */
    IMPORT_C void ProcessInspectionRequestChanges( TUint aPid, TRequestStatus& aStatus, TMemSpyDriverProcessInspectionInfo& aInfo );

    /**
     *
     */
    IMPORT_C void ProcessInspectionRequestChangesCancel( TUint aPid );

    /**
     *
     */
    IMPORT_C void ProcessInspectionAutoStartItemsReset();

    /**
     *
     */
    IMPORT_C TInt ProcessInspectionAutoStartItemsAdd( TUint aSID );


public: // Handles related

    /**
     * Get all of the handles in a specific container
     */
    IMPORT_C TInt GetContainerHandles( TMemSpyDriverContainerType aContainer, TAny** aHandleArray, TInt& aHandleCount );

    /**
     * Get all handles of a specific type, for a specific thread. 
     */
    IMPORT_C TInt GetThreadHandlesByType( TInt aTid, TMemSpyDriverContainerType aType, TAny** aHandleArray, TInt& aHandleCount );

    /**
     * Get all handles of a specific type, for a specific process. 
     */
    IMPORT_C TInt GetProcessHandlesByType( TInt aPid, TMemSpyDriverContainerType aType, TAny** aHandleArray, TInt& aHandleCount );

    /**
     * Get handle info for a specific handle owner by a specific thread. If the handle is not thread-specific, then
     * use KMemSpyDriverEnumerateContainerHandles as the aTid parameter.
     */
    IMPORT_C TInt GetGenericHandleInfo( TInt aTid, TMemSpyDriverContainerType aType, TAny* aHandle, TMemSpyDriverHandleInfoGeneric& aParams );

    /**
     * Fetches the in-memory size of a specific kernel object (where possible). The size is an approximation since
     * it doesn't take into account the size of ASSP-specific classes.
     */
    IMPORT_C TInt GetApproximateKernelObjectSize( TMemSpyDriverContainerType aType );

    /**
     * Use this method to identify the handles of threads & processes that happen to reference
     * the specified thread or process.
     *
     * In brief, every kernel-side thread and process object contains a handle container. This container
     * is the DObject-handles which the thread or process has open.
     *
     * This method can be used to search all of the other handle containers within other threads & processes
     * to see if any of those handle containers happen to reference the specified thread or process (as defined by
     * aTid or aPid).
     */
    IMPORT_C TInt GetReferencesToMyThread( TUint aTid );
    IMPORT_C TInt GetReferencesToMyProcess( TUint aPid );

    /**
     * Get info about a P&S kernel object
     */
    IMPORT_C TInt GetPAndSInfo( TAny* aHandle, TMemSpyDriverPAndSInfo& aInfo );

    /**
     * Get handles to suspended threads from a CondVar
     */
    IMPORT_C TInt GetCondVarSuspendedThreads( TAny* aCondVarHandle, TAny** aThreadHandleArray, TInt& aThreadCount );

    /**
     * Get info about CondVar suspended thread
     */
    IMPORT_C TInt GetCondVarSuspendedThreadInfo( TAny* aHandle, TMemSpyDriverCondVarSuspendedThreadInfo& aInfo );
    
public: // CLIENT <-> SERVER

    /**
     * Gets handles of all sessions that are connected to a particular server
     */
    IMPORT_C TInt GetServerSessionHandles( TAny* aServerHandle, TAny** aSessionHandleArray, TInt& aSessionHandleCount );

    /**
     * Gets session information for a given session handle.
     */
    IMPORT_C TInt GetServerSessionInfo( TAny* aSessionHandle, TMemSpyDriverServerSessionInfo& aParams );


public: // STREAM INTERFACE

    /**
     * Opens a MemSpy data stream for reading
     */
    IMPORT_C RMemSpyMemStreamReader StreamOpenL();

public: // MISC

    /**
     *
     */
    IMPORT_C TMemSpyMemoryModelType MemoryModelType();

    /**
     *
     */
    IMPORT_C TBool IsDebugKernel();

    /**
     * Round value to a multiple of the current page size
     *
     */
    IMPORT_C TUint32 RoundToPageSize( TUint32 aValue );

    /**
     *
     */
    IMPORT_C TInt Impersonate( TUint32 aValue );

private: // Internal methods
    static TUint RHeapVTable();
    static TBool DebugEUser();
    void ResetStreamBuffer();
    void ReadHeapInfoFreeCellsFromXferBufferL( RArray<TMemSpyDriverFreeCell>& aFreeCells );

private: // Data members
    RBuf8 iBuffer;
    };



#endif
