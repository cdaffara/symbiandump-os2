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

#include "MemSpyDriverLogChanHeapDataKernel.h"

// System includes
#include <u32hal.h>
#include <e32rom.h>
#include <memspy/driver/memspydriverconstants.h>
#include <memspy/driver/memspydriverobjectsshared.h>

// Shared includes
#include "MemSpyDriverOpCodes.h"
#include "MemSpyDriverObjectsInternal.h"

// User includes
#include "MemSpyDriverHeap.h"
#include "MemSpyDriverUtils.h"
#include "MemSpyDriverDevice.h"
#include "MemSpyDriverOSAdaption.h"
#include "MemSpyDriverHeapWalker.h"
#include "MemSpyDriverUserEventMonitor.h"
#include "MemSpyDriverSuspensionManager.h"

DMemSpyDriverLogChanHeapDataKernel::DMemSpyDriverLogChanHeapDataKernel( DMemSpyDriverDevice& aDevice, DThread& aThread )
:   DMemSpyDriverLogChanHeapDataBase( aDevice, aThread ), iKernelHeap( aDevice.OSAdaption() )
    {
    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapDataKernel::DMemSpyDriverLogChanHeapDataKernel() - this: 0x%08x", this ));
    }

TInt DMemSpyDriverLogChanHeapDataKernel::Request( TInt aFunction, TAny* a1, TAny* a2 )
	{
	TInt r = DMemSpyDriverLogChanHeapBase::Request( aFunction, a1, a2 );
    if  ( r == KErrNone )
        {
        if  ( aFunction != EMemSpyDriverOpCodeHeapKernelDataFetchCellList )
            {
            ReleaseCellList();
            }
        //
        switch( aFunction )
		    {
	    case EMemSpyDriverOpCodeHeapKernelDataGetInfo:
            r = GetInfoData( (TMemSpyDriverInternalHeapRequestParameters*) a1 );
            break;
        case EMemSpyDriverOpCodeHeapKernelDataGetIsDebugKernel:
            r = GetIsDebugKernel(a1);
            break;
        case EMemSpyDriverOpCodeHeapKernelDataFetchCellList:
            r = FetchCellList( (TDes8*) a1 );
            break;
        case EMemSpyDriverOpCodeHeapKernelDataCopyHeap:
            r = MakeKernelHeapCopy();
            break;
        case EMemSpyDriverOpCodeHeapKernelDataGetFull:
            r = DMemSpyDriverLogChanHeapDataBase::GetFullData( (TMemSpyDriverInternalHeapDataParams*) a1 );
            break;
        case EMemSpyDriverOpCodeHeapKernelDataFreeHeapCopy:
            FreeKernelHeapCopy();
            break;

        default:
            r = KErrNotSupported;
		    break;
            }
        }
    //
    return r;
	}


TBool DMemSpyDriverLogChanHeapDataKernel::IsHandler( TInt aFunction ) const
    {
    return ( aFunction > EMemSpyDriverOpCodeHeapKernelDataBase && aFunction < EMemSpyDriverOpCodeHeapKernelDataEnd );
    }

TInt DMemSpyDriverLogChanHeapDataKernel::GetInfoData( TMemSpyDriverInternalHeapRequestParameters* aParams )
    {
    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapDataKernel::GetInfoData() - START" ) );

    TInt r = Kern::ThreadRawRead( &ClientThread(), aParams, &iHeapInfoParams, sizeof(TMemSpyDriverInternalHeapRequestParameters) );
    if  ( r != KErrNone )
        {
    	TRACE( Kern::Printf("DMemSpyDriverLogChanHeapDataKernel::GetInfoDataUser - params read error: %d", r));
        }
    else
        {
        if (iHeapInfoParams.iUseKernelHeapCopy)
            {
            __ASSERT_ALWAYS( iKernelHeap.IsOpen(), MemSpyDriverUtils::PanicThread( ClientThread(), EPanicHeapKernelCopyExpected ) );
            r = DMemSpyDriverLogChanHeapDataKernel::GetInfoData(iKernelHeap);
            }
        else
            {
            RMemSpyDriverRHeapKernelInPlace rHeap(OSAdaption());
            r = rHeap.OpenKernelHeap();
            if  ( r == KErrNone )
                {
                r = DMemSpyDriverLogChanHeapDataKernel::GetInfoData(rHeap);
                }
            else 
                {
                TRACE( Kern::Printf("DMemSpyDriverLogChanHeapDataKernel::GetInfoData() - open err: %d", r ) );                
                }
            }
        }

	TRACE( Kern::Printf("DMemSpyDriverLogChanHeapDataKernel::GetInfoData() - END - ret: %d", r) );
    return r;
    }

TInt DMemSpyDriverLogChanHeapDataKernel::GetInfoData(RMemSpyDriverRHeapBase& aHeap)
    {
    NKern::ThreadEnterCS();

    TInt r = KErrNone;
    
    // This object holds all of the info we will accumulate for the client.
    TMemSpyHeapInfo masterHeapInfo;
    masterHeapInfo.SetType(aHeap.GetTypeFromHelper());
    masterHeapInfo.SetTid( 2 );
    masterHeapInfo.SetPid( 1 );

    // This is the RHeap-specific object that contains all RHeap info
    TMemSpyHeapInfoRHeap& rHeapInfo = masterHeapInfo.AsRHeap();

    // We must walk the heap in order to build statistics
    TRACE_KH( Kern::Printf("DMemSpyDriverLogChanHeapDataKernel::GetInfoData - calling heap walker constructor..."));
    RMemSpyDriverHeapWalker heapWalker(aHeap);
    if  (iHeapInfoParams.iBuildFreeCellList || iHeapInfoParams.iBuildAllocCellList)
        {
        heapWalker.SetObserver( this );
        TRACE( Kern::Printf("DMemSpyDriverLogChanHeapDataKernel::GetInfoData - collecting cells"));
        }
    else
        {
        TRACE( Kern::Printf("DMemSpyDriverLogChanHeapDataKernel::GetInfoData - not collecting cells"));
        }

    TRACE_KH( Kern::Printf("DMemSpyDriverLogChanHeapDataKernel::GetInfoData - starting traversal..." ));

#if defined( TRACE_TYPE_KERNELHEAP )
    heapWalker.SetPrintDebug();
#endif
    if (r == KErrNone) r = heapWalker.Traverse();
    TRACE_KH( Kern::Printf("DMemSpyDriverLogChanHeapDataKernel::GetInfoData - finished traversal - err: %d", r ));

    TMemSpyHeapStatisticsRHeap& rHeapStats = rHeapInfo.Statistics();
    heapWalker.CopyStatsTo( rHeapStats );

    // Get remaining meta data that isn't stored elsewhere
    TMemSpyHeapMetaDataRHeap& rHeapMetaData = rHeapInfo.MetaData();
    TFullName chunkName;
    aHeap.Chunk().FullName(chunkName);
    rHeapMetaData.SetChunkName(chunkName);
    rHeapMetaData.SetChunkSize( (TUint) aHeap.Chunk().Size() );
    rHeapMetaData.SetChunkHandle( &aHeap.Chunk() );
    rHeapMetaData.SetChunkBaseAddress( OSAdaption().DChunk().GetBase(aHeap.Chunk()) );
    rHeapMetaData.SetDebugAllocator(aHeap.Helper()->AllocatorIsUdeb());
    rHeapMetaData.SetUserThread( EFalse );
    rHeapMetaData.SetSharedHeap( ETrue );
    rHeapMetaData.iHeapSize = aHeap.Helper()->CommittedSize();
    rHeapMetaData.iAllocatorAddress = (TAny*)aHeap.Helper()->AllocatorAddress();
    rHeapMetaData.iMinHeapSize = aHeap.Helper()->MinCommittedSize();
    rHeapMetaData.iMaxHeapSize = aHeap.Helper()->MaxCommittedSize();

    PrintHeapInfo( masterHeapInfo );

    // Write free cells if requested
    if  ( r == KErrNone && (iHeapInfoParams.iBuildFreeCellList || iHeapInfoParams.iBuildAllocCellList))
    	{
        r = PrepareCellListTransferBuffer();
        }

    if  ( r >= KErrNone )
    	{
        // Write results back to user-side
        TRACE_KH( Kern::Printf("DMemSpyDriverLogChanHeapDataKernel::GetInfoData - writing to user-side..."));
        TMemSpyHeapInfo* kernelMasterInfo = iHeapInfoParams.iMasterInfo;
        const TInt error = Kern::ThreadRawWrite( &ClientThread(), kernelMasterInfo, &masterHeapInfo, sizeof(TMemSpyHeapInfo) );
        if  ( error < 0 )
        	{
            r = error;
        	}
        }

    NKern::ThreadLeaveCS();
    
    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapDataKernel::GetInfoData() - END - ret: %d", r) );
    return r;
    }

TInt DMemSpyDriverLogChanHeapDataKernel::GetIsDebugKernel(TAny* aResult)
    {
    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapDataKernel::GetIsDebugKernel() - START") );
    
    TInt r = KErrNone;
    TBool debugKernel = EFalse;

    NKern::ThreadEnterCS();
    
    RMemSpyDriverRHeapKernelInPlace rHeap(OSAdaption());
    r = rHeap.OpenKernelHeap();
    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapDataKernel::GetIsDebugKernel() - open kernel heap returned: %d", r) );

    if  ( r == KErrNone )
        {
        debugKernel = rHeap.Helper()->AllocatorIsUdeb();

        // Tidy up
        rHeap.Close();
        }

    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapDataKernel::GetIsDebugKernel() - debugKernel: %d", debugKernel) );

    // Write back to user-land
    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapDataKernel::GetIsDebugKernel() - writing to user-side...") );
    r = Kern::ThreadRawWrite( &ClientThread(), aResult, &debugKernel, sizeof(TBool) );

    NKern::ThreadLeaveCS();

 	TRACE( Kern::Printf("DMemSpyDriverLogChanHeapDataKernel::GetIsDebugKernel() - END - ret: %d", r) );
    return r;
    }

TInt DMemSpyDriverLogChanHeapDataKernel::OpenKernelHeap( RMemSpyDriverRHeapKernelFromCopy& aHeap )
    {
    TRACE_KH( Kern::Printf("DMemSpyDriverLogChanHeapDataKernel::OpenKernelHeap(CP) - START") );

    RAllocatorHelper kernelHeapHelper;
    TInt r = kernelHeapHelper.OpenKernelHeap();
    TRACE_KH( Kern::Printf("DMemSpyDriverLogChanHeapDataKernel::OpenKernelHeap(CP) - open err: %d", r ) );
    if  ( r == KErrNone )
        {
        DChunk* kernelChunk = kernelHeapHelper.OpenUnderlyingChunk();
        if (kernelChunk) {
            // TODO can we lock just the kernel heap here to avoid the problem below?
            
            // Make a new chunk that we can copy the kernel heap into. We cannot lock the system the entire time
            // we need to do this, therefore there is no guarantee that the chunk will be large enough to hold the
            // (current) heap data at the time we need to make the copy. We oversize the chunk by 1mb in the "hope"
            // that it will be enough... :(
            TChunkCreateInfo info;
            info.iType         = TChunkCreateInfo::ESharedKernelSingle;
            info.iMaxSize      = kernelChunk->MaxSize() + ( 1024 * 1024 );
            info.iOwnsMemory   = ETrue; // Use memory from system's free pool
            info.iDestroyedDfc = NULL;
            #ifdef __EPOC32__
            info.iMapAttr      = (TInt)EMapAttrFullyBlocking; // Full caching
            #endif
            
            // Holds a copy of the client's heap chunk
            DChunk* heapCopyChunk;
            TLinAddr heapCopyChunkAddress;
            TUint32 heapCopyChunkMappingAttributes;
            r = Kern::ChunkCreate( info, heapCopyChunk, heapCopyChunkAddress, heapCopyChunkMappingAttributes );
            TRACE_KH( Kern::Printf("DMemSpyDriverLogChanHeapDataKernel::OpenKernelHeap(CP) - creating chunk returned: %d", r));
    
            // Unfortunately we have to commit every page in the copied chunk irrespective of whether that's the case
            // with the kernel chunk due to mutex ordering enforced by the kernel. See the note about this below.  
            // This results in waste but with the way the kernel heap currently works it's not too bad.
            // TODO fix this so it's more generic and doesn't rely on details of how the kernel heap works.
            r = Kern::ChunkCommit(heapCopyChunk, 0, info.iMaxSize);            
            
            // Keep track of the pages we need to de-commit from the copy
            // We allocate enough space here so we don't attempt to resise
            TLinAddr* pageAddrsToDeCommit = new TLinAddr[info.iMaxSize / KPageSize];
            TUint pageAddrsToDeCommitIndex = 0;
            
            TBool cleanupCopyChunk = EFalse;
            if  ( r == KErrNone )
                {
                HBuf8* data = HBuf8::New(KPageSize);
                if (data)
                    {
                    TAny* dataPtr = (TAny*) data->Ptr();
                
                    r = kernelHeapHelper.TryLock();
                    TInt actualKernelChunkSize = kernelChunk->Size();
                    
                    if ( r == KErrNone )
                        {
                        
                        // We now attempt to copy the kernel heap page by page
                        // This is because the kernel chunk is disconnected and hence can have pages
                        // in the middle of the heap that havent' been committed yet.
                        // TODO can we make this more efficient?
                        TInt err = KErrNone;
                        TUint8* kernChunkAddr = kernelChunk->Base();
                        TUint8* copyChunkAddr = (TUint8*) heapCopyChunkAddress;
                        while(err == KErrNone && 
                              kernChunkAddr < kernelChunk->Base() + kernelChunk->MaxSize())
                            {
                            XTRAP(err, XT_DEFAULT, memcpy(dataPtr, kernChunkAddr, KPageSize));
                            if (!err)
                                {
                                // It'd be nice if we could just commit the pages of the copy chunk to match 
                                // the commited pages in the kernel heap here but that violates the following
                                // mutex ordering:
                                // mutex KernHeap order 8 [from kernelHeapHelper.TryLock()] vs 
                                // mutex MemoryObjectMutex1 order 9 [from Kern::ChunkCommit()]
                                memcpy(copyChunkAddr, dataPtr, KPageSize);
                                }
                            else
                                {
                                // This page in the kernel heap wasn't committed so we can continue onto the next   
                                err = KErrNone;
                                // but we do need to remember this so ...
                                pageAddrsToDeCommit[pageAddrsToDeCommitIndex++] = (TLinAddr) copyChunkAddr;                                  
                                }
                            kernChunkAddr += KPageSize;    
                            copyChunkAddr += KPageSize;
                            }
                        kernelHeapHelper.TryUnlock();
                        TRACE_KH( Kern::Printf("DMemSpyDriverLogChanHeapDataKernel::OpenKernelHeap(CP) - copied kernel heap data" ));

                        // Now remove the bits we didn't actually need to commit
                        for(TUint i=0; i < pageAddrsToDeCommitIndex; i++) 
                            {
                            r = heapCopyChunk->Decommit(pageAddrsToDeCommit[i], KPageSize);
                            if (r != KErrNone) 
                                {
                                break;
                                }
                            }
                        if (r != KErrNone)
                            {
                            TInt oversizedEndLength = heapCopyChunk->Size() - actualKernelChunkSize;
                            r = heapCopyChunk->Decommit(actualKernelChunkSize, oversizedEndLength);                                    
                            }
                        
                        if (r == KErrNone)
                            {
                            // Transfer ownership of the copy heap chunk to the heap object.
                            TInt offset = (TInt) heapCopyChunkAddress - (TInt) kernelChunk->Base();
                            TRACE_KH( Kern::Printf("DMemSpyDriverLogChanHeapDataKernel::OpenKernelHeap(CP) - heapCopyChunkAddress: 0x%08x, kernel chunk base: 0x%08x",
                                                    heapCopyChunkAddress, kernelChunk->Base()));
                            r = aHeap.AssociateWithKernelChunk( kernelChunk, heapCopyChunk, heapCopyChunkAddress, offset );
                            }
                        else
                            {
                            TRACE_KH( Kern::Printf("DMemSpyDriverLogChanHeapDataKernel::OpenKernelHeap(CP) - failed to decommit all the unnecessary pages from the copy chunk - %d", r ));
                            cleanupCopyChunk = ETrue;
                            }                        
                        }         
                    else
                        {
                        TRACE_KH( Kern::Printf("DMemSpyDriverLogChanHeapDataKernel::OpenKernelHeap(CP) - failed to lock the kernel heap" ));
                        cleanupCopyChunk = ETrue;
                        }
                    
                    delete data;
                    }
                else
                    {
                    TRACE_KH( Kern::Printf("Failed to allocate a 4K buffer" ) );
                    r = KErrNoMemory;
                    cleanupCopyChunk = ETrue;
                    }
                }
            else
                {
                TRACE_KH( Kern::Printf("DMemSpyDriverLogChanHeapDataKernel::OpenKernelHeap(CP) - copy chunk create error: %d", r ) );
                cleanupCopyChunk = ETrue;
                }
            
            if (cleanupCopyChunk)
                {
                NKern::ThreadEnterCS();
                Kern::ChunkClose( heapCopyChunk );
                heapCopyChunk = NULL;
                NKern::ThreadLeaveCS();
                }
            
            delete[] pageAddrsToDeCommit;
            pageAddrsToDeCommit = NULL;
            }
        else
            {
            TRACE_KH( Kern::Printf("DMemSpyDriverLogChanHeapDataKernel::OpenKernelHeap(CP) - failed to open the kernel chunk" ) );
            }
        }
    else
        {
        TRACE_KH( Kern::Printf("DMemSpyDriverLogChanHeapDataKernel::OpenKernelHeap(CP) - failed to open the kernel heap: %d", r ) );
        }
    
    kernelHeapHelper.Close();

    if  ( r != KErrNone )
        {                    
        aHeap.Close(); // also deals with the chunk                    
        }                           
    
    
    TRACE_KH( Kern::Printf("DMemSpyDriverLogChanHeapDataKernel::OpenKernelHeap(CP) - END - ret: %d", r ) );
    return r;
    }

TInt DMemSpyDriverLogChanHeapDataKernel::MakeKernelHeapCopy()
    {
    // First phase is to 
    // a) Open kernel heap
    // b) Make a copy of the heap data
    //
    // The driver leaves kernel context with the copy of the kernel heap still associated with MemSpy's process.
    // The second driver call will copy the chunk data to user side and release the kernel side chunk.

    iKernelHeap.Reset();
    NKern::ThreadEnterCS();

    TInt r = OpenKernelHeap( iKernelHeap );
    TRACE_KH( Kern::Printf("DMemSpyDriverLogChanHeapDataKernel::GetFullDataInit() - open err: %d", r));

    NKern::ThreadLeaveCS();

    TRACE_KH( Kern::Printf("DMemSpyDriverLogChanHeapDataKernel::GetFullDataInit() - END - ret: %d", r));
    return r;
    }

void DMemSpyDriverLogChanHeapDataKernel::FreeKernelHeapCopy()
    {
    iKernelHeap.Close();
    }

TInt DMemSpyDriverLogChanHeapDataKernel::GetFullData( TMemSpyDriverInternalHeapDataParams& aParams )
    {
    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapDataKernel::GetFullData() - START") );

    TInt r = OpenTempObject( aParams.iTid, EThread );
    if  ( r != KErrNone )
        {
        Kern::Printf("DMemSpyDriverLogChanHeapDataKernel::GetFullData() - END - thread not found");
        return r;
        }
    // Don't need to check if the kernel heap contains DRM data unlike for user heaps

    DThread* thread = (DThread*) TempObject();
    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapDataKernel::GetFullData - thread: %O", thread) );

    r = DoGetFullData(aParams, thread, iKernelHeap);

    CloseTempObject();

    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapDataKernel::GetFullData() - END - ret: %d", r) );
    return r;
    }
