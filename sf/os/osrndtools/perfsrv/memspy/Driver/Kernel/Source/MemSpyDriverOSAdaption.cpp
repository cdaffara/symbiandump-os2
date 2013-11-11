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
#include "MemSpyDriverOSAdaption.h"

// System includes
#include <kern_priv.h>
#include <nkern.h>
#include <nk_plat.h>

#ifdef __MARM__
#include <arm.h>
#endif

// I've removed UNALIGNED_DATA_MEMBER in preference for just using memcpy to get round the potential unaligned access. -TomS

// User includes
#include "MemSpyDriverLog.h"
#include "MemSpyDriverPAndS.h"
#include "MemSpyDriverDevice.h"

// Internal constants
const TInt KMemSpyLocalThreadDataSizeEstimate = 0x80; // The amount of user stack that MemSpy attempts to scan for the RHeap vTable



DMemSpyDriverOSAdaptionDObject::DMemSpyDriverOSAdaptionDObject( DMemSpyDriverOSAdaption& aOSAdaption )
:   iOSAdaption( aOSAdaption )
    {
    }


TUint8 DMemSpyDriverOSAdaptionDObject::GetContainerID( DObject& aObject ) const
    {
    return aObject.iContainerID;
    }


TObjectType DMemSpyDriverOSAdaptionDObject::GetObjectType( DObject& aObject ) const
    {
    const TUint8 containerId = GetContainerID( aObject );
    const TObjectType ret = static_cast< TObjectType >( containerId - 1 );
    return ret;
    }


DObject* DMemSpyDriverOSAdaptionDObject::GetOwner( DObject& aObject ) const
    {
    return aObject.iOwner;
    }


DObject* DMemSpyDriverOSAdaptionDObject::GetOwner( DObject& aObject, TUint8 aExpectedContainerId ) const
    {
    DObject* owner = GetOwner( aObject );
    //
    const TUint8 containerId = GetContainerID( aObject ) - 1;
    if ( containerId != aExpectedContainerId )
        {
        owner = NULL;
        }
    //
    return owner;
    }

TInt DMemSpyDriverOSAdaptionDObject::GetAccessCount( DObject& aObject ) const
    {
    return aObject.AccessCount();
    }

TInt DMemSpyDriverOSAdaptionDObject::GetUniqueID( DObject& aObject ) const
    {
    return aObject.UniqueID();
    }

TUint DMemSpyDriverOSAdaptionDObject::GetProtection( DObject& aObject ) const
    {
    return aObject.Protection();
    }

TUint8* DMemSpyDriverOSAdaptionDObject::GetAddressOfKernelOwner( DObject& aObject ) const
    {
    return (TUint8*)aObject.Owner();
    }











DMemSpyDriverOSAdaptionDThread::DMemSpyDriverOSAdaptionDThread( DMemSpyDriverOSAdaption& aOSAdaption )
:   DMemSpyDriverOSAdaptionDObject( aOSAdaption )
    {
    // Get current NThread and map it on to DThread
    NThread* nThread = NKern::CurrentThread();
    TRACE( Kern::Printf( "DMemSpyDriverOSAdaptionDThread::Construct() - nThread: 0x%08x", nThread ) );
    DThread* dThread = Kern::NThreadToDThread( nThread );
    TRACE( Kern::Printf( "DMemSpyDriverOSAdaptionDThread::Construct() - dThread: 0x%08x", dThread ) );

    // At this point, it should be possible to work out the offset of the NThread within DThread.
    iOffset_NThread = reinterpret_cast<TUint32>( nThread ) - reinterpret_cast<TUint32>( dThread );
    TRACE( Kern::Printf( "DMemSpyDriverOSAdaptionDThread::Construct() - difference: 0x%08x", iOffset_NThread ) );

    // Work out the delta between compile time and run time
    TInt delta = iOffset_NThread - _FOFF( DThread, iNThread );
    TRACE( Kern::Printf( "DMemSpyDriverOSAdaptionDThread::Construct() - compile time vs run time offset delta: %d", delta ));

    // iNThread
    TRACE( Kern::Printf( "OSA - [DThread::iNThread]              compile time offset: 0x%08x, run time offset: 0x%08x", _FOFF(DThread,iNThread), iOffset_NThread ));

    // iExitType
    iOffset_ExitType = _FOFF( DThread, iExitType ) + delta;
    TRACE( Kern::Printf( "OSA - [DThread::iExitType]             compile time offset: 0x%08x, run time offset: 0x%08x", _FOFF(DThread,iExitType), iOffset_ExitType ));

    // iSupervisorStack
    iOffset_SupervisorStackBase = _FOFF( DThread, iSupervisorStack ) + delta;
    TRACE( Kern::Printf( "OSA - [DThread::iSupervisorStack]      compile time offset: 0x%08x, run time offset: 0x%08x", _FOFF(DThread,iSupervisorStack), iOffset_SupervisorStackBase ));

    // iSupervisorStackSize
    iOffset_SupervisorStackSize = _FOFF( DThread, iSupervisorStackSize ) + delta;
    TRACE( Kern::Printf( "OSA - [DThread::iSupervisorStackSize]  compile time offset: 0x%08x, run time offset: 0x%08x", _FOFF(DThread,iSupervisorStackSize), iOffset_SupervisorStackSize ));
    }


NThread* DMemSpyDriverOSAdaptionDThread::GetNThread( DThread& aObject ) const
    {
    DThread* dThread = &aObject;
    TUint32 pTarget = reinterpret_cast<TUint32>( dThread ) + iOffset_NThread;
    NThread* pRet = reinterpret_cast< NThread* >( pTarget );
    TRACE( Kern::Printf( "DMemSpyDriverOSAdaptionDThread::GetNThread() - aObject: 0x%08x, ret: 0x%08x", &aObject, pRet ) );
    return pRet;
    }


TExitType DMemSpyDriverOSAdaptionDThread::GetExitType( DThread& aObject ) const
    {
    DThread* dThread = &aObject;
    TUint32 pTarget = reinterpret_cast<TUint32>( dThread ) + iOffset_ExitType;
	TUint8 exitType = *reinterpret_cast<TUint8*>(pTarget);
    TRACE( Kern::Printf( "DMemSpyDriverOSAdaptionDThread::GetExitType() - aObject: 0x%08x, ret: %d", &aObject, (TInt)exitType ) );
    return (TExitType)exitType;
    }


TUint32 DMemSpyDriverOSAdaptionDThread::GetSupervisorStackBase( DThread& aObject ) const
    {
    DThread* dThread = &aObject;
    TUint32 pTarget = reinterpret_cast<TUint32>( dThread ) + iOffset_SupervisorStackBase;

	TUint32 ret;
	memcpy(&ret, (const TAny*)pTarget, sizeof(TUint32));
    TRACE( Kern::Printf( "DMemSpyDriverOSAdaptionDThread::GetSupervisorStackBase() - aObject: 0x%08x, ret: 0x%08x", &aObject, ret ) );
    return ret;
    }


TInt DMemSpyDriverOSAdaptionDThread::GetSupervisorStackSize( DThread& aObject ) const
    {
    DThread* dThread = &aObject;
    TUint32 pTarget = reinterpret_cast<TUint32>( dThread ) + iOffset_SupervisorStackSize;
	
	TInt ret;
	memcpy(&ret, (const TAny*)pTarget, sizeof(TInt));
    TRACE( Kern::Printf( "DMemSpyDriverOSAdaptionDThread::GetSupervisorStackSize() - aObject: 0x%08x, ret: %d", &aObject, ret ) );
    return ret;
    }


RAllocator* DMemSpyDriverOSAdaptionDThread::GetAllocator( DThread& aObject ) const
    {
    TRACE( Kern::Printf( "DMemSpyDriverOSAdaptionDThread::GetAllocator() - START" ) );
    //
    RAllocator* ret = aObject.iAllocator;
    TRACE( Kern::Printf( "DMemSpyDriverOSAdaptionDThread::GetAllocator() - allocator: 0x%08x", ret ) );
    //
    if  ( ret == NULL )
        {
        TUint32 stackAddress = 0;
        ret = GetAllocatorAndStackAddress( aObject, stackAddress );
        }
    //
    TRACE( Kern::Printf( "DMemSpyDriverOSAdaptionDThread::GetAllocator() - END - ret: 0x%08x", ret ) );
    return ret;
    }


// TODO try to rework this without inspecting the stack!
CActiveScheduler* DMemSpyDriverOSAdaptionDThread::GetActiveScheduler( DThread& aObject ) const
    {
    TRACE( Kern::Printf( "DMemSpyDriverOSAdaptionDThread::GetActiveScheduler() - START" ) );
    CActiveScheduler* ret = aObject.iScheduler;
    //
    TRACE( Kern::Printf( "DMemSpyDriverOSAdaptionDThread::GetActiveScheduler() - scheduler: 0x%08x", ret ) );
    if  ( ret == NULL )
        {
        // We need the stack address of the heap in order to locate the active scheduler.
        // Implicitly this means that MemSpy can only list active scheduler contents for threads
        // that use a default RHeap. This has always been the case, so this is not a functional break.
        //
        // Assumed stack layout is something like this:
        // 
        // stack[0x00403ffc] 0x00000000, vTable: 0x00000000 (offset: 0004)
        // stack[0x00403ff8] 0x00000000, vTable: 0x00000000 (offset: 0008)
        // stack[0x00403ff4] 0x00000002, vTable: 0x00000000 (offset: 0012)
        // stack[0x00403ff0] 0x00000002, vTable: 0x00000000 (offset: 0016)
        // stack[0x00403fec] 0x00000000, vTable: 0x00000000 (offset: 0020)
        // stack[0x00403fe8] 0x0000000c, vTable: 0x00000000 (offset: 0024)
        // stack[0x00403fe4] 0x00600078, vTable: 0x80228938 (offset: 0028)
        // stack[0x00403fe0] 0x00000001, vTable: 0x00000000 (offset: 0032)
        // stack[0x00403fdc] 0x00600000, vTable: 0x80268430 (offset: 0036)		TLocalThreadData::iTlsHeap       RAllocator*		  80268428    0034    vtable for RHeap
        // stack[0x00403fd8] 0x0060009c, vTable: 0x80268394 (offset: 0040)		TLocalThreadData::iTrapHandler   TTrapHandler		  8026838c    0014    vtable for TCleanupTrapHandler
        // stack[0x00403fd4] 0x00600110, vTable: 0x802682ec (offset: 0044)		TLocalThreadData::iScheduler     CActiveScheduler*	  802682e4    002c    vtable for CActiveScheduler
        // stack[0x00403fd0] 0x00600000, vTable: 0x80268430 (offset: 0048)      TLocalThreadData::iHeap          RAllocator*          80268428    0034    vtable for RHeap
        // stack[0x00403fcc] 0x00000000, vTable: 0x00000000 (offset: 0052)
        // stack[0x00403fc8] 0x00100000, vTable: 0x00000000 (offset: 0056)
        //
        // GetAllocatorAndStackAddress() will return the first RHeap* it finds, so this will be iTlsHeap, which
        // is actually no bad thing as it will hopefully be initialised using an RHeap pointer.
        // CActiveScheduler* is 8 bytes further on from that.
        //
        const TUint32 KOffsetToCActiveScheduler = 8;
        // 
        TUint32 stackAddress = 0;
        RAllocator* allocator = GetAllocatorAndStackAddress( aObject, stackAddress );
        TRACE( Kern::Printf( "DMemSpyDriverOSAdaptionDThread::GetActiveScheduler() - allocator: 0x%08x, stackAddress: 0x%08x", allocator, stackAddress ) );

        // This assumes the layout of the thread local data structure, i.e. that the four bytes that follow the
        // allocator pointer are always the active scheduler pointer.
        if  ( allocator != NULL && stackAddress > 0 )
            {
            TUint32* ptr = reinterpret_cast< TUint32* >( stackAddress - KOffsetToCActiveScheduler );
            const TInt r = Kern::ThreadRawRead( &aObject, ptr, &ret, sizeof( ret ) );
            TRACE( Kern::Printf( "DMemSpyDriverOSAdaptionDThread::GetActiveScheduler() - stack address containing scheduler pointer: 0x%08x, read result: %d, CActiveScheduler*: 0x%08x", ptr, r, ret ) );
            if  ( r != KErrNone )
                {
                ret = NULL;
                }
            }
        }
    //
    TRACE( Kern::Printf( "DMemSpyDriverOSAdaptionDThread::GetActiveScheduler() - END - ret: 0x%08x", ret ) );
    return ret;
    }


TUint32 DMemSpyDriverOSAdaptionDThread::GetUserStackBase( DThread& aObject ) const
    {
    return aObject.iUserStackRunAddress;
    }


TInt DMemSpyDriverOSAdaptionDThread::GetUserStackSize( DThread& aObject ) const
    {
    return aObject.iUserStackSize;
    }


DProcess* DMemSpyDriverOSAdaptionDThread::GetOwningProcess( DThread& aObject ) const
    {
    return aObject.iOwningProcess;
    }


TUint DMemSpyDriverOSAdaptionDThread::GetId( DThread& aObject ) const
    {
    return aObject.iId;
    }


MemSpyObjectIx* DMemSpyDriverOSAdaptionDThread::GetHandles( DThread& aObject ) const
    {
    MemSpyObjectIx* handles = MemSpyObjectIx_GetHandlePointer_Thread( aObject );
    return handles;
    }


TUint DMemSpyDriverOSAdaptionDThread::GetOwningProcessId( DThread& aObject ) const
    {
    DProcess* process = GetOwningProcess( aObject );
    TUint ret = 0;
    //
    if  ( process )
        {
        ret = OSAdaption().DProcess().GetId( *process );
        }
    //
    return ret;
    }


TInt DMemSpyDriverOSAdaptionDThread::GetPriority( DThread& aObject ) const
    {
    return aObject.iThreadPriority;
    }


TUint8* DMemSpyDriverOSAdaptionDThread::GetAddressOfOwningProcess( DThread& aObject ) const
    {
    return (TUint8*)aObject.iOwningProcess;
    }


void DMemSpyDriverOSAdaptionDThread::GetNameOfOwningProcess( DThread& aObject, TDes& aName ) const
    {
    if  ( aObject.iOwningProcess )
        {
        aObject.iOwningProcess->FullName( aName );
        }
    }

// TODO re-work this without inspecting the stack.
// In particular we can get the allocator from the DThread object's iAllocator object
RAllocator* DMemSpyDriverOSAdaptionDThread::GetAllocatorAndStackAddress( DThread& aThread, TUint32& aStackAddress ) const
    {
    TRACE( Kern::Printf( "DMemSpyDriverOSAdaptionDThread::GetAllocatorAndStackAddress() - START for aThread 0x%08x with ID %d", &aThread, aThread.iId ) );
    //
    aStackAddress = 0;
    RAllocator* ret = NULL;

    // We will assume the thread is running and that the user-side stack has been set up
    // accordingly.
    const TUint32 base = GetUserStackBase( aThread );
    const TInt size = GetUserStackSize( aThread );
    TRACE( Kern::Printf( "DMemSpyDriverOSAdaptionDThread::GetAllocatorAndStackAddress() - base: 0x%08x, size: %d, KMemSpyLocalThreadDataSizeEstimate: %d", base, size, KMemSpyLocalThreadDataSizeEstimate ) );
    const TUint32 top = base + size;

    // This is the RHeap vtable we are looking for
    const TUint32 rHeapVTable = OSAdaption().Device().RHeapVTable();
    TRACE( Kern::Printf( "DMemSpyDriverOSAdaptionDThread::GetAllocatorAndStackAddress() - rHeapVTable: 0x%08x", rHeapVTable ) );
    //
    TInt r = KErrNone;

#ifdef TRACE_TYPE_GENERAL 
    for( TUint32 addr = top - 4; addr >= top - KMemSpyLocalThreadDataSizeEstimate; addr -= 4 )
        {
        TUint32 value = 0;
        TUint32 possibleVTable = 0;
        //
        TUint32* ptr = reinterpret_cast< TUint32* >( addr );
        //
        r = Kern::ThreadRawRead( &aThread, ptr, &value, sizeof( value ) );
        if  ( r == KErrNone )
            {
            r = Kern::ThreadRawRead( &aThread, reinterpret_cast< const TAny* >( value ), &possibleVTable, sizeof( possibleVTable ) );
            }

        if ( r == KErrNone )
            {
            TRACE( Kern::Printf( "DMemSpyDriverOSAdaptionDThread::GetAllocatorAndStackAddress() - stack[0x%08x] 0x%08x, vTable: 0x%08x (offset: %04d)", addr, value, possibleVTable, top - addr ) );
            }
        else
            {
            TRACE( Kern::Printf( "DMemSpyDriverOSAdaptionDThread::GetAllocatorAndStackAddress() - stack[0x%08x] 0x%08x, (offset: %04d) read failed: %d", addr, value, top - addr, r ) );
            }
        }
#endif

    for( TUint32 addr = top - 4; addr >= top - KMemSpyLocalThreadDataSizeEstimate; addr -= 4 )
        {
        TRACE( Kern::Printf( "DMemSpyDriverOSAdaptionDThread::GetAllocatorAndStackAddress() - addr to read from: 0x%08x", addr ) );
        TUint32 value = 0;
        //
        TUint32* ptr = reinterpret_cast< TUint32* >( addr );
        //
        r = Kern::ThreadRawRead( &aThread, ptr, &value, sizeof( value ) );
        TRACE( Kern::Printf( "DMemSpyDriverOSAdaptionDThread::GetAllocatorAndStackAddress() - read from: 0x%08x, result: %d, value: 0x%08x", addr, r, value ) );
        //
        if  ( r == KErrNone )
            {
            // Try and read from the address which is now stored within 'value'. We must do this because 
            // the TLD class holds an RAllocator* and we need to ascertain the vTable of the RAllocator* matches the
            // only supported vTable that MemSpy understands (RHeap*).
            TUint32 possibleVTable = 0;
            r = Kern::ThreadRawRead( &aThread, reinterpret_cast< const TAny* >( value ), &possibleVTable, sizeof( possibleVTable ) );
            TRACE( Kern::Printf( "DMemSpyDriverOSAdaptionDThread::GetAllocatorAndStackAddress() - possible vtable read from: 0x%08x, result: %d, possibleVTable: 0x%08x", value, r, possibleVTable ) );
            if  ( r == KErrNone && possibleVTable == rHeapVTable )
                {
                aStackAddress = addr;
                ret = reinterpret_cast< RAllocator* >( value );
                break;
                }
            }
        }
    //
    TRACE( Kern::Printf( "DMemSpyDriverOSAdaptionDThread::GetAllocatorAndStackAddress() - END - ret: 0x%08x, aStackAddress: 0x%08x", ret, aStackAddress ) );
    return ret;
    }    


TBool DMemSpyDriverOSAdaptionDThread::IsHandleIndexValid( DThread& aObject ) const
    {
    MemSpyObjectIx* handles = MemSpyObjectIx_GetHandlePointer_Thread( aObject );
    return ( handles != NULL );
    }













DMemSpyDriverOSAdaptionDProcess::DMemSpyDriverOSAdaptionDProcess( DMemSpyDriverOSAdaption& aOSAdaption )
:   DMemSpyDriverOSAdaptionDObject( aOSAdaption )
    {
    }


TUint DMemSpyDriverOSAdaptionDProcess::GetId( DProcess& aObject ) const
    {
    TRACE( Kern::Printf( "DMemSpyDriverOSAdaptionDProcess::GetId() - value: %d", aObject.iId ) );
    return aObject.iId;
    }


MemSpyObjectIx* DMemSpyDriverOSAdaptionDProcess::GetHandles( DProcess& aObject ) const
    {
    MemSpyObjectIx* handles = MemSpyObjectIx_GetHandlePointer_Process( aObject );
    return handles;
    }


TExitType DMemSpyDriverOSAdaptionDProcess::GetExitType( DProcess& aObject ) const
    {
    return static_cast< TExitType >( aObject.iExitType );
    }


DThread* DMemSpyDriverOSAdaptionDProcess::OpenFirstThread( DProcess& aProcess ) const
    {
	// It appears that the system lock needs to be held while manipulating the iThreadQ
	DThread* result = NULL;
	NKern::LockSystem();
	// We don't use DProcess::FirstThread() as that doesn't appear to do any checking of whether the list is empty, ie if there are no threads at all
	SDblQueLink* threadLink = aProcess.iThreadQ.First();
	if (threadLink != NULL && threadLink != &aProcess.iThreadQ.iA)
		{
		result = _LOFF(threadLink,DThread,iProcessLink);
		if (result->Open() != KErrNone)
			{
			result = NULL;
			}
		}
	NKern::UnlockSystem();
    return result;
    }


TUint32 DMemSpyDriverOSAdaptionDProcess::GetSID( DProcess& aObject ) const
    {
    return GetSecurityInfo( aObject ).iSecureId;
    }


TUint DMemSpyDriverOSAdaptionDProcess::GetSecurityZone( DProcess& aObject ) const
    {
    return aObject.iSecurityZone;
    }


SSecurityInfo& DMemSpyDriverOSAdaptionDProcess::GetSecurityInfo( DProcess& aObject ) const
    {
    return aObject.iS;
    }


TInt DMemSpyDriverOSAdaptionDProcess::GetFlags( DProcess& aObject ) const
    {
    return aObject.iFlags;
    }


TInt DMemSpyDriverOSAdaptionDProcess::GetGeneration( DProcess& aObject ) const
    {
    return aObject.iGeneration;
    }


SDblQue& DMemSpyDriverOSAdaptionDProcess::GetThreadQueue( DProcess& aObject ) const
    {
    return aObject.iThreadQ;
    }


DThread* DMemSpyDriverOSAdaptionDProcess::GetThread( SDblQueLink* aLink ) const
    {
	DThread* ret = _LOFF( aLink, DThread, iProcessLink );
    return ret;
    }


void DMemSpyDriverOSAdaptionDProcess::SetSID( DProcess& aObject, TUint32 aSID ) const
    {
    GetSecurityInfo( aObject ).iSecureId = aSID;
    }


void DMemSpyDriverOSAdaptionDProcess::SetSecurityZone( DProcess& aObject, TUint aSecurityZone ) const
    {
    aObject.iSecurityZone = aSecurityZone;
    }


TBool DMemSpyDriverOSAdaptionDProcess::IsHandleIndexValid( DProcess& aObject ) const
    {
    MemSpyObjectIx* handles = MemSpyObjectIx_GetHandlePointer_Process( aObject );
    return ( handles != NULL );
    }


TUint DMemSpyDriverOSAdaptionDProcess::GetCreatorId( DProcess& aObject ) const
    {
    return aObject.iCreatorId;
    }


TInt DMemSpyDriverOSAdaptionDProcess::GetAttributes( DProcess& aObject ) const
    {
    return aObject.iAttributes;
    }


TInt DMemSpyDriverOSAdaptionDProcess::GetPriority( DProcess& aObject ) const
    {
    return aObject.iPriority;
    }


TUint8* DMemSpyDriverOSAdaptionDProcess::GetAddressOfOwningProcess( DProcess& aObject ) const
    {
    return (TUint8*)aObject.iOwningProcess;
    }


TUint8* DMemSpyDriverOSAdaptionDProcess::GetAddressOfDataBssStackChunk( DProcess& aObject ) const
    {
    return (TUint8*)aObject.iDataBssStackChunk;
    }

TBool DMemSpyDriverOSAdaptionDProcess::IsKernProcess(DProcess& aProcess) const
	{
	// The kernel process always has pid 1
	return GetId(aProcess) == 1;
	}



DMemSpyDriverOSAdaptionDChunk::DMemSpyDriverOSAdaptionDChunk( DMemSpyDriverOSAdaption& aOSAdaption )
:   DMemSpyDriverOSAdaptionDObject( aOSAdaption )
    {
    }


TInt DMemSpyDriverOSAdaptionDChunk::GetSize( DChunk& aObject ) const
    {
    return aObject.Size();
    }


TInt DMemSpyDriverOSAdaptionDChunk::GetMaxSize( DChunk& aObject ) const
    {
    return aObject.MaxSize();
    }


TUint8* DMemSpyDriverOSAdaptionDChunk::GetBase( DChunk& aChunk ) const
    {
    TUint8* base = aChunk.Base();
	if (base == 0)
		{
		// Under flexible memory model, DChunk::Base() will return NULL (for non-fixed chunks anyway, and that means most of them)
		// A more useful thing to return is the base address in the owning process
		DProcess* proc = GetOwningProcess(aChunk);
		NKern::ThreadEnterCS();
		if (proc && proc->Open() == KErrNone)
			{
			// Probably shouldn't call ChunkUserBase for a non-user-owned chunk
			if (!OSAdaption().DProcess().IsKernProcess(*proc))
				{
				DThread* firstThread = OSAdaption().DProcess().OpenFirstThread(*proc);
				if (firstThread)
					{
					base = Kern::ChunkUserBase(&aChunk, firstThread);
					firstThread->Close(NULL);
					}
				}
			proc->Close(NULL);
			}
		NKern::ThreadLeaveCS();
		}
	return base; 
    }


DProcess* DMemSpyDriverOSAdaptionDChunk::GetOwningProcess( DChunk& aObject ) const
    {
    return aObject.OwningProcess();
    }


TUint DMemSpyDriverOSAdaptionDChunk::GetOwningProcessId( DChunk& aObject ) const
    {
    TUint ret = 0;
    //
    DProcess* process = GetOwningProcess( aObject );
    if  ( process )
        {
        ret = OSAdaption().DProcess().GetId( *process );
        }
    //
    return ret;
    }


TUint DMemSpyDriverOSAdaptionDChunk::GetControllingOwnerId( DChunk& aObject ) const
    {
    return aObject.iControllingOwner;
    }


TChunkType DMemSpyDriverOSAdaptionDChunk::GetType( DChunk& aObject ) const
    {
    return aObject.iChunkType;
    }


TInt DMemSpyDriverOSAdaptionDChunk::GetAttributes( DChunk& aObject ) const
    {
    return aObject.iAttributes;
    }


TUint8* DMemSpyDriverOSAdaptionDChunk::GetAddressOfOwningProcess( DChunk& aObject ) const
    {
    return (TUint8*)aObject.iOwningProcess;
    }


TInt DMemSpyDriverOSAdaptionDChunk::GetBottom( DChunk& aObject ) const
    {
    return aObject.Bottom();
    }


TInt DMemSpyDriverOSAdaptionDChunk::GetTop( DChunk& aObject ) const
    {
    return aObject.Top();
    }


TInt DMemSpyDriverOSAdaptionDChunk::GetStartPos( DChunk& aObject ) const
    {
    return aObject.iStartPos;
    }


TUint DMemSpyDriverOSAdaptionDChunk::GetRestrictions( DChunk& aObject ) const
    {
    return aObject.iRestrictions;
    }


TUint DMemSpyDriverOSAdaptionDChunk::GetMapAttr( DChunk& aObject ) const
    {
    return aObject.iMapAttr;
    }


void DMemSpyDriverOSAdaptionDChunk::GetNameOfOwningProcess( DChunk& aObject, TDes& aName ) const
    {
    if ( aObject.OwningProcess() )
        {
         aObject.OwningProcess()->FullName( aName );
        }
    }


















DMemSpyDriverOSAdaptionDServer::DMemSpyDriverOSAdaptionDServer( DMemSpyDriverOSAdaption& aOSAdaption )
:   DMemSpyDriverOSAdaptionDObject( aOSAdaption )
    {
    }


SDblQue& DMemSpyDriverOSAdaptionDServer::GetSessionQueue( DServer& aObject ) const
    {
    return aObject.iSessionQ;
    }


DSession* DMemSpyDriverOSAdaptionDServer::GetSession( SDblQueLink* aLink ) const
    {
	DSession* session = _LOFF( aLink, DSession, iServerLink );
    return session;
    }


TInt DMemSpyDriverOSAdaptionDServer::GetSessionCount( DServer& aObject ) const
    {
    TInt ret = 0;
    //
    SDblQueLink* anchor = &aObject.iSessionQ.iA;
    SDblQueLink* link = aObject.iSessionQ.First();
    //
    while( link != anchor)
	    {
 	    ++ret;
	    link = link->iNext;
	    }
    //
    return ret;
    }


DThread* DMemSpyDriverOSAdaptionDServer::GetOwningThread( DServer& aObject ) const
    {
    return aObject.iOwningThread;
    }


TUint DMemSpyDriverOSAdaptionDServer::GetOwningThreadId( DServer& aObject ) const
    {
    TUint ret = 0;
    //
    DThread* thread = GetOwningThread( aObject );
    if  ( thread )
        {
        ret = OSAdaption().DThread().GetId( *thread );
        }
    //
    return ret;
    }


TIpcSessionType DMemSpyDriverOSAdaptionDServer::GetSessionType( DServer& aObject ) const
    {
    return static_cast< TIpcSessionType >( aObject.iSessionType );
    }


TUint8* DMemSpyDriverOSAdaptionDServer::GetAddressOfOwningThread( DServer& aObject ) const
    {
    return (TUint8*)aObject.iOwningThread;
    }


void DMemSpyDriverOSAdaptionDServer::GetNameOfOwningThread( DServer& aObject, TDes& aName ) const
    {
    if ( aObject.iOwningThread )
        {
         aObject.iOwningThread->FullName( aName );
        }
    }












DMemSpyDriverOSAdaptionDSession::DMemSpyDriverOSAdaptionDSession( DMemSpyDriverOSAdaption& aOSAdaption )
:   DMemSpyDriverOSAdaptionDObject( aOSAdaption )
    {
    }


TIpcSessionType DMemSpyDriverOSAdaptionDSession::GetSessionType( DSession& aObject ) const
    {
    return static_cast< TIpcSessionType >( aObject.iSessionType );
    }


DServer* DMemSpyDriverOSAdaptionDSession::GetServer( DSession& aObject ) const
    {
    return aObject.iServer;
    }


TUint8* DMemSpyDriverOSAdaptionDSession::GetAddressOfServer( DSession& aObject ) const
    {
    return (TUint8*)aObject.iServer;
    }


TUint16 DMemSpyDriverOSAdaptionDSession::GetTotalAccessCount( DSession& aObject ) const
    {
    return aObject.iTotalAccessCount;
    }


TUint8 DMemSpyDriverOSAdaptionDSession::GetSrvSessionType( DSession& aObject ) const
    {
    return aObject.iSvrSessionType;
    }


TInt DMemSpyDriverOSAdaptionDSession::GetMsgCount( DSession& aObject ) const
    {
    return aObject.iMsgCount;
    }
    

TInt DMemSpyDriverOSAdaptionDSession::GetMsgLimit( DSession& aObject ) const
    {
    return aObject.iMsgLimit;
    }
















DMemSpyDriverOSAdaptionDCodeSeg::DMemSpyDriverOSAdaptionDCodeSeg( DMemSpyDriverOSAdaption& aOSAdaption )
:   DMemSpyDriverOSAdaptionDObject( aOSAdaption )
    {
    }


DCodeSeg* DMemSpyDriverOSAdaptionDCodeSeg::GetCodeSeg( SDblQueLink* aLink ) const
    {
    DCodeSeg* ret = _LOFF( aLink, DCodeSeg, iTempLink );
    return ret;
    }


DCodeSeg* DMemSpyDriverOSAdaptionDCodeSeg::GetCodeSeg( DLibrary& aLibrary ) const
    {
    return aLibrary.iCodeSeg;
    }


DCodeSeg* DMemSpyDriverOSAdaptionDCodeSeg::GetCodeSegFromHandle( TAny* aHandle ) const
    {
    DCodeSeg* ret = DCodeSeg::VerifyHandle( aHandle );
    return ret;
    }


TBool DMemSpyDriverOSAdaptionDCodeSeg::GetIsXIP( DCodeSeg& aCodeSeg ) const
    {
    TBool ret = ETrue;
    //
#ifdef __WINS__
    (void) aCodeSeg;
#else
    DEpocCodeSeg& epocCodeSegment = static_cast< DEpocCodeSeg& >( aCodeSeg );
    ret = ( epocCodeSegment.iXIP != 0 );
#endif
    //
    return ret;
    }


TInt DMemSpyDriverOSAdaptionDCodeSeg::GetCodeSegQueue( DProcess& aObject, SDblQue& aQueue ) const
    {
	const TInt count = aObject.TraverseCodeSegs( &aQueue, NULL, DCodeSeg::EMarkDebug, DProcess::ETraverseFlagAdd );
    return count;
    }


void DMemSpyDriverOSAdaptionDCodeSeg::EmptyCodeSegQueue( SDblQue& aQueue ) const
    {
	DCodeSeg::EmptyQueue( aQueue, DCodeSeg::EMarkDebug );
    }


TUint32 DMemSpyDriverOSAdaptionDCodeSeg::GetSize( DCodeSeg& aCodeSeg ) const
    {
    return aCodeSeg.iSize;
    }


void DMemSpyDriverOSAdaptionDCodeSeg::GetCreateInfo( DCodeSeg& aCodeSeg, TCodeSegCreateInfo& aInfo ) const
    {
    aCodeSeg.Info( aInfo );
    }


TUint8 DMemSpyDriverOSAdaptionDCodeSeg::GetState( DLibrary& aLibrary ) const
    {
    return aLibrary.iState;
    }


TInt DMemSpyDriverOSAdaptionDCodeSeg::GetMapCount( DLibrary& aLibrary ) const
    {
    return aLibrary.iMapCount;
    }

















DMemSpyDriverOSAdaptionDSemaphore::DMemSpyDriverOSAdaptionDSemaphore( DMemSpyDriverOSAdaption& aOSAdaption )
:   DMemSpyDriverOSAdaptionDObject( aOSAdaption )
    {
    }


TInt DMemSpyDriverOSAdaptionDSemaphore::GetCount( DSemaphore& aObject ) const
    {
    return aObject.iCount;
    }


TUint8 DMemSpyDriverOSAdaptionDSemaphore::GetResetting( DSemaphore& aObject ) const
    {
    return aObject.iResetting;
    }


















DMemSpyDriverOSAdaptionDMutex::DMemSpyDriverOSAdaptionDMutex( DMemSpyDriverOSAdaption& aOSAdaption )
:   DMemSpyDriverOSAdaptionDObject( aOSAdaption )
    {
    }


TInt DMemSpyDriverOSAdaptionDMutex::GetHoldCount( DMutex& aObject ) const
    {
    return aObject.iHoldCount;
    }


TInt DMemSpyDriverOSAdaptionDMutex::GetWaitCount( DMutex& aObject ) const
    {
    return aObject.iWaitCount;
    }


TUint8 DMemSpyDriverOSAdaptionDMutex::GetResetting( DMutex& aObject ) const
    {
    return aObject.iResetting;
    }


TUint8 DMemSpyDriverOSAdaptionDMutex::GetOrder( DMutex& aObject ) const
    {
    return aObject.iOrder;
    }









DMemSpyDriverOSAdaptionDLogicalDevice::DMemSpyDriverOSAdaptionDLogicalDevice( DMemSpyDriverOSAdaption& aOSAdaption )
:   DMemSpyDriverOSAdaptionDObject( aOSAdaption )
    {
    }


TInt DMemSpyDriverOSAdaptionDLogicalDevice::GetOpenChannels( DLogicalDevice& aObject ) const
    {
    return aObject.iOpenChannels;
    }


TVersion DMemSpyDriverOSAdaptionDLogicalDevice::GetVersion( DLogicalDevice& aObject ) const
    {
    return aObject.iVersion;
    }


TUint DMemSpyDriverOSAdaptionDLogicalDevice::GetParseMask( DLogicalDevice& aObject ) const
    {
    return aObject.iParseMask;
    }


TUint DMemSpyDriverOSAdaptionDLogicalDevice::GetUnitsMask( DLogicalDevice& aObject ) const
    {
    return aObject.iUnitsMask;
    }

















DMemSpyDriverOSAdaptionDPhysicalDevice::DMemSpyDriverOSAdaptionDPhysicalDevice( DMemSpyDriverOSAdaption& aOSAdaption )
:   DMemSpyDriverOSAdaptionDObject( aOSAdaption )
    {
    }


TVersion DMemSpyDriverOSAdaptionDPhysicalDevice::GetVersion( DPhysicalDevice& aObject ) const
    {
    return aObject.iVersion;
    }


TUint DMemSpyDriverOSAdaptionDPhysicalDevice::GetUnitsMask( DPhysicalDevice& aObject ) const
    {
    return aObject.iUnitsMask;
    }


TUint8* DMemSpyDriverOSAdaptionDPhysicalDevice::GetAddressOfCodeSeg( DPhysicalDevice& aObject ) const
    {
    return (TUint8*)aObject.iCodeSeg;
    }

















DMemSpyDriverOSAdaptionDChangeNotifier::DMemSpyDriverOSAdaptionDChangeNotifier( DMemSpyDriverOSAdaption& aOSAdaption )
:   DMemSpyDriverOSAdaptionDObject( aOSAdaption )
    {
    }


TUint DMemSpyDriverOSAdaptionDChangeNotifier::GetChanges( DChangeNotifier& aObject ) const
    {
    return aObject.iChanges;
    }


TUint8* DMemSpyDriverOSAdaptionDChangeNotifier::GetAddressOfOwningThread( DChangeNotifier& aObject ) const
    {
    return (TUint8*)aObject.iThread;
    }


void DMemSpyDriverOSAdaptionDChangeNotifier::GetNameOfOwningThread( DChangeNotifier& aObject, TDes& aName ) const
    {
    if ( aObject.iThread )
        {
         aObject.iThread->FullName( aName );
        }
    }
















DMemSpyDriverOSAdaptionDUndertaker::DMemSpyDriverOSAdaptionDUndertaker( DMemSpyDriverOSAdaption& aOSAdaption )
:   DMemSpyDriverOSAdaptionDObject( aOSAdaption )
    {
    }


TUint8* DMemSpyDriverOSAdaptionDUndertaker::GetAddressOfOwningThread( DUndertaker& aObject ) const
    {
    return (TUint8*)aObject.iOwningThread;
    }


void DMemSpyDriverOSAdaptionDUndertaker::GetNameOfOwningThread( DUndertaker& aObject, TDes& aName ) const
    {
    if ( aObject.iOwningThread )
        {
         aObject.iOwningThread->FullName( aName );
        }
    }

















DMemSpyDriverOSAdaptionDCondVar::DMemSpyDriverOSAdaptionDCondVar( DMemSpyDriverOSAdaption& aOSAdaption )
:   DMemSpyDriverOSAdaptionDObject( aOSAdaption )
    {
    }


TUint8 DMemSpyDriverOSAdaptionDCondVar::GetResetting( DCondVar& aObject ) const
    {
    return aObject.iResetting;
    }


TUint8* DMemSpyDriverOSAdaptionDCondVar::GetAddressOfMutex( DCondVar& aObject ) const
    {
    return (TUint8*)aObject.iMutex;
    }


void DMemSpyDriverOSAdaptionDCondVar::GetNameOfMutex( DCondVar& aObject, TDes& aName ) const
    {
    if ( aObject.iMutex )
        {
         aObject.iMutex->FullName( aName );
        }
    }


TInt DMemSpyDriverOSAdaptionDCondVar::GetWaitCount( DCondVar& aObject ) const
    {
    return aObject.iWaitCount;
    }


SDblQue& DMemSpyDriverOSAdaptionDCondVar::GetSuspendedQ( DCondVar& aObject ) const
    {
    return aObject.iSuspendedQ;
    }


DThread* DMemSpyDriverOSAdaptionDCondVar::GetThread( SDblQueLink* aLink ) const
    {
    DThread* thread = _LOFF( aLink, DThread, iWaitLink );
    return thread;
    }















DMemSpyDriverOSAdaptionDTimer::DMemSpyDriverOSAdaptionDTimer( DMemSpyDriverOSAdaption& aOSAdaption )
:   DMemSpyDriverOSAdaptionDObject( aOSAdaption )
    {
    }


TTimer::TTimerType DMemSpyDriverOSAdaptionDTimer::GetType( DTimer& aObject ) const
    {
    const TTimer::TTimerType ret = aObject.iTimer.Type();
    return ret;
    }


TTimer::TTimerState DMemSpyDriverOSAdaptionDTimer::GetState( DTimer& aObject ) const
    {
    const TTimer::TTimerState ret = static_cast<TTimer::TTimerState>( aObject.iTimer.iState );
    return ret;
    }














DMemSpyDriverOSAdaptionDPropertyRef::DMemSpyDriverOSAdaptionDPropertyRef( DMemSpyDriverOSAdaption& aOSAdaption )
:   DMemSpyDriverOSAdaptionDObject( aOSAdaption )
    {
    }


TBool DMemSpyDriverOSAdaptionDPropertyRef::GetIsReady( DObject& aObject ) const
    {
    DMemSpyPropertyRef* prop = GetPropertyRef( aObject );
    return ( prop != NULL );
    }


RProperty::TType DMemSpyDriverOSAdaptionDPropertyRef::GetType( DObject& aObject ) const
    {
    RProperty::TType ret = RProperty::EInt;
    //
    DMemSpyPropertyRef* prop = GetPropertyRef( aObject );
    if  ( prop )
        {
        ret = (RProperty::TType) prop->iProp->iType;
        }
    //
    return ret;
    }


TUint DMemSpyDriverOSAdaptionDPropertyRef::GetCategory( DObject& aObject ) const
    {
    TUint ret = 0;
    //
    DMemSpyPropertyRef* prop = GetPropertyRef( aObject );
    if  ( prop )
        {
        ret = prop->iProp->iCategory;
        }
    //
    return ret;
    }


TUint DMemSpyDriverOSAdaptionDPropertyRef::GetKey( DObject& aObject ) const
    {
    TUint ret = 0;
    //
    DMemSpyPropertyRef* prop = GetPropertyRef( aObject );
    if  ( prop )
        {
        ret = prop->iProp->iKey;
        }
    //
    return ret;
    }


TInt DMemSpyDriverOSAdaptionDPropertyRef::GetRefCount( DObject& aObject ) const
    {
    TInt ret = 0;
    //
    DMemSpyPropertyRef* prop = GetPropertyRef( aObject );
    if  ( prop )
        {
        ret = prop->iProp->iRefCount;
        }
    //
    return ret;
    }


TUint DMemSpyDriverOSAdaptionDPropertyRef::GetThreadId( DObject& aObject ) const
    {
    TUint ret = 0;
    //
    DMemSpyPropertyRef* prop = GetPropertyRef( aObject );
    if  ( prop )
        {
        DThread* thread = prop->iClient;
        if  ( thread )
            {
            ret = OSAdaption().DThread().GetId( *thread );
            }
        }
    //
    return ret;
    }


TUint32 DMemSpyDriverOSAdaptionDPropertyRef::GetCreatorSID( DObject& aObject ) const
    {
    TUint32 ret = 0;
    //
    DMemSpyPropertyRef* prop = GetPropertyRef( aObject );
    if  ( prop )
        {
        ret = prop->iProp->iOwner;
        }
    //
    return ret;
    }


DMemSpyPropertyRef* DMemSpyDriverOSAdaptionDPropertyRef::GetPropertyRef( DObject& aObject ) const
    {
    DMemSpyPropertyRef* ret = NULL;
    //
    const TUint8 containerId = GetContainerID( aObject ) - 1;
    if ( containerId == EPropertyRef )
        {
        DMemSpyPropertyRef* prop = reinterpret_cast< DMemSpyPropertyRef* >( &aObject );
        if ( prop->iProp && prop->iClient )
            {
            ret = prop;
            }
        }
    //
    return ret;
    }















DMemSpyDriverOSAdaption::DMemSpyDriverOSAdaption( DMemSpyDriverDevice& aDevice )
:   iDevice( aDevice )
    {
    }


DMemSpyDriverOSAdaption::~DMemSpyDriverOSAdaption()
    {
    TRACE( Kern::Printf( "DMemSpyDriverOSAdaption::~DMemSpyDriverOSAdaption() - START"));
    NKern::ThreadEnterCS();
    //
    delete iDThread;
    delete iDProcess;
    delete iDChunk;
    delete iDServer;
    delete iDSession;
    delete iDCodeSeg;
    delete iDSemaphore;
    delete iDMutex;
    delete iDLogicalDevice;
    delete iDPhysicalDevice;
    delete iDChangeNotifier;
    delete iDUndertaker;
    delete iDCondVar;
    delete iDTimer;
    delete iDPropertyRef;
    //
    NKern::ThreadLeaveCS();
    TRACE( Kern::Printf( "DMemSpyDriverOSAdaption::~DMemSpyDriverOSAdaption() - END"));
    }


TInt DMemSpyDriverOSAdaption::Construct()
    {
    TRACE( Kern::Printf( "DMemSpyDriverOSAdaption::Construct() - START" ) );
    //
    NKern::ThreadEnterCS();
    //
    TInt error = KErrNoMemory;
    //
    iDThread = new DMemSpyDriverOSAdaptionDThread( *this );
    if  ( iDThread )
        {
        iDProcess = new DMemSpyDriverOSAdaptionDProcess( *this );
        if  ( iDProcess )
            {
            iDChunk = new DMemSpyDriverOSAdaptionDChunk( *this );
            if  ( iDChunk )
                {
                iDServer = new DMemSpyDriverOSAdaptionDServer( *this );
                if  ( iDServer )
                    {
                    iDSession = new DMemSpyDriverOSAdaptionDSession( *this );
                    if  ( iDSession )
                        {
                        iDCodeSeg = new DMemSpyDriverOSAdaptionDCodeSeg( *this );
                        if  ( iDCodeSeg )
                            {
                            iDSemaphore = new DMemSpyDriverOSAdaptionDSemaphore( *this );
                            if  ( iDSemaphore )
                                {
                                iDMutex = new DMemSpyDriverOSAdaptionDMutex( *this );
                                if  ( iDMutex )
                                    {
                                    iDLogicalDevice = new DMemSpyDriverOSAdaptionDLogicalDevice( *this );
                                    if  ( iDLogicalDevice )
                                        {
                                        iDPhysicalDevice = new DMemSpyDriverOSAdaptionDPhysicalDevice( *this );
                                        if  ( iDPhysicalDevice )
                                            {
                                            iDChangeNotifier = new DMemSpyDriverOSAdaptionDChangeNotifier( *this );
                                            if  ( iDChangeNotifier )
                                                {
                                                iDUndertaker = new DMemSpyDriverOSAdaptionDUndertaker( *this );
                                                if ( iDUndertaker )
                                                    {
                                                    iDCondVar = new DMemSpyDriverOSAdaptionDCondVar( *this );
                                                    if ( iDCondVar )
                                                        {
                                                        iDTimer = new DMemSpyDriverOSAdaptionDTimer( *this );
                                                        if  ( iDTimer )
                                                            {
                                                            iDPropertyRef = new DMemSpyDriverOSAdaptionDPropertyRef( *this );
                                                            if  ( iDPropertyRef )
                                                                {
                                                                error = KErrNone;
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    //
    NKern::ThreadLeaveCS();

    TRACE( Kern::Printf( "DMemSpyDriverOSAdaption::Construct() - END - error: %d", error ) );
    return error;
    }





