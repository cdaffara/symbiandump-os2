/*
* Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   Implementation of the MgmtFrameMemMngr class.
*
*/

/*
* %version: 21 %
*/

#include "WlLddWlanLddConfig.h"
#include "MgmtFrameMemMngr.h"
#include "osachunk.h"
#include <kern_priv.h>

extern TAny* os_alloc( const TUint32 );
extern void os_free( const TAny* );

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt MgmtFrameMemMngr::DoAllocate( DChunk*& aSharedMemoryChunk )
    {
    TraceDump( INIT_LEVEL, 
        ("WLANLDD: MgmtFrameMemMngr::DoAllocate:") );

    // determine if cached memory shall be used
    TUint cacheOption = iUseCachedMemory ? 
        EMapAttrCachedMax : 
        EMapAttrFullyBlocking;

    // Start creating the chunk
    
    TChunkCreateInfo info;
    info.iType         = TChunkCreateInfo::ESharedKernelMultiple;
    info.iMaxSize      = iChunkSize;
    info.iMapAttr      = EMapAttrReadUser |
                         EMapAttrWriteUser |
                         EMapAttrShared |
                         cacheOption;
    info.iOwnsMemory   = ETrue;
    info.iDestroyedDfc = NULL;
    
    DChunk* chunk;

    TraceDump( INIT_LEVEL, 
        (("WLANLDD: MgmtFrameMemMngr::DoAllocate: create chunk of size: %d"),
        iChunkSize) );

    TUint32 chunkMapAttrNotNeeded ( 0 );

    // Enter critical section so we can't die and leak the object we are 
    // creating, i.e. DChunk (shared memory chunk)
    NKern::ThreadEnterCS();

    TInt r = Kern::ChunkCreate( 
        info, 
        chunk, 
        iChunkKernelAddr, 
        chunkMapAttrNotNeeded );

    if( r != KErrNone)
        {
        TraceDump( WARNING_LEVEL, 
            (("WLANLDD: MgmtFrameMemMngr::DoAllocate: create chunk failed. Status: %d"),
            r ) );

        NKern::ThreadLeaveCS();
        return r;
        }

    TraceDump(MEMORY, (("WLANLDD: new DChunk: 0x%08x"), 
        reinterpret_cast<TUint32>(chunk)));

    TraceDump(INIT_LEVEL, 
        (("MgmtFrameMemMngr::DoAllocate: Platform Hw Chunk create success with cacheOption: 0x%08x"), 
        cacheOption));

    TUint32 physicalAddressNotNeeded ( 0 );

    // Map our device's memory into the chunk (at offset 0)
    r = Kern::ChunkCommitContiguous( 
            chunk, 
            0, 
            iChunkSize, 
            physicalAddressNotNeeded );

    if ( r != KErrNone)
        {
        TraceDump( WARNING_LEVEL, 
            (("WLANLDD: MgmtFrameMemMngr::DoAllocate: chunk commit failed. Status: %d"),
            r ) );

        // Commit failed so tidy-up.
        // Close chunk, which will then get deleted at some point
        Kern::ChunkClose( chunk );
        }
    else
        {
        TraceDump( INIT_LEVEL, 
            ("WLANLDD: MgmtFrameMemMngr::DoAllocate: chunk commit success") );

        // Commit succeeded so pass back the chunk pointer
        aSharedMemoryChunk = chunk;
        }

    // Can leave critical section now that we have saved pointer to created object
    NKern::ThreadLeaveCS();

    TraceDump( INIT_LEVEL, 
        (("WLANLDD: MgmtFrameMemMngr::DoAllocate: chunk map attr: %d"),
        chunkMapAttrNotNeeded ) );
    TraceDump( INIT_LEVEL, 
        (("WLANLDD: MgmtFrameMemMngr::DoAllocate: iChunkKernelAddr: 0x%08x"),
        static_cast<TUint32>(iChunkKernelAddr) ) );
    TraceDump( INIT_LEVEL, 
        (("WLANLDD: MgmtFrameMemMngr::DoAllocate: chunk physical address: 0x%08x"),
        physicalAddressNotNeeded ) );

    return r;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt MgmtFrameMemMngr::DoOpenHandle(
    DThread& aThread,
    TSharedChunkInfo& aSharedChunkInfo,
    DChunk* aSharedMemoryChunk )
    {
    TInt ret ( KErrGeneral );

    if ( aSharedMemoryChunk )
        {
        
        // Need to be in critical section while creating handles
        NKern::ThreadEnterCS();

        // Create handle to shared memory chunk for client thread
        TInt r = Kern::MakeHandleAndOpen( &aThread, aSharedMemoryChunk );

        // Leave critical section 
        NKern::ThreadLeaveCS();

        // positive r value is a handle, negative value is an error code
        if( r >= 0 )
            {
            // mapping success

            TraceDump( INIT_LEVEL, 
                (("WLANLDD: MgmtFrameMemMngr::OnInitialiseMemory: Handle create & open ok: handle: %d"),
                r ) );
    
            // store the handle & the chunk size
            aSharedChunkInfo.iChunkHandle = r;
            aSharedChunkInfo.iSize = iChunkSize;
            iClientChunkHandle = r;

            // store the kernel addresses

            TUint8* start_of_mem = reinterpret_cast<TUint8*>(iChunkKernelAddr );

            const TUint KRxDataChunkSize( 
                iChunkSize 
                - ( sizeof( TDataBuffer )
                    + KMgmtSideTxBufferLength
                    + KProtocolStackSideTxDataChunkSize
                    + sizeof( RFrameXferBlock ) 
                    + sizeof( RFrameXferBlockProtocolStack ) ) );

            TraceDump( INIT_LEVEL, 
                (("WLANLDD: MgmtFrameMemMngr::DoOpenHandle: KRxDataChunkSize: %d"),
                KRxDataChunkSize ) );

            iRxDataChunk = start_of_mem;

            TraceDump( INIT_LEVEL, 
                (("WLANLDD: MgmtFrameMemMngr::DoOpenHandle: iRxDataChunk start addr: 0x%08x"),
                reinterpret_cast<TUint32>(iRxDataChunk) ) );
            TraceDump( INIT_LEVEL, 
                (("WLANLDD: MgmtFrameMemMngr::DoOpenHandle: iRxDataChunk end addr: 0x%08x"),
                reinterpret_cast<TUint32>(iRxDataChunk + KRxDataChunkSize) ) );

            // create the Rx frame memory pool manager
            iRxFrameMemoryPool = new WlanChunk( 
                iRxDataChunk, 
                iRxDataChunk + KRxDataChunkSize,
                iRxFrameBufAllocationUnit );
                
            if ( iRxFrameMemoryPool && iRxFrameMemoryPool->IsValid() )
                {
                ret = KErrNone;

                TraceDump(MEMORY, (("WLANLDD: new WlanChunk: 0x%08x"), 
                    reinterpret_cast<TUint32>(iRxFrameMemoryPool)));

                iTxDataBuffer = reinterpret_cast<TDataBuffer*>(
                    start_of_mem
                    + KRxDataChunkSize );

                TraceDump( INIT_LEVEL, 
                    (("WLANLDD: MgmtFrameMemMngr::DoOpenHandle: Engine TxDataBuf start addr: 0x%08x"),
                    reinterpret_cast<TUint32>(iTxDataBuffer) ) );

                // for the single Tx buffer the actual buffer memory immediately
                // follows the Tx frame meta header
                iTxDataBuffer->KeSetBufferOffset( sizeof( TDataBuffer ) );        

                iFrameXferBlock = reinterpret_cast<RFrameXferBlock*>(
                    start_of_mem
                    + KRxDataChunkSize
                    + sizeof( TDataBuffer )
                    + KMgmtSideTxBufferLength 
                    + KProtocolStackSideTxDataChunkSize );

                iFrameXferBlockBase = iFrameXferBlock;
                
                TraceDump( INIT_LEVEL, 
                    (("WLANLDD: MgmtFrameMemMngr::DoOpenHandle: Engine RFrameXferBlock addr: 0x%08x"),
                    reinterpret_cast<TUint32>(iFrameXferBlock) ) );

                // initiliase xfer block
                iFrameXferBlock->Initialize( KMgmtSideTxBufferLength );
                
                iRxBufAlignmentPadding = RxBufAlignmentPadding();
                
                iParent.SetRxBufAlignmentPadding( iRxBufAlignmentPadding );
                }
            else
                {
                // create failed
                delete iRxFrameMemoryPool;
                iRxFrameMemoryPool = NULL;
                // error is returned
                }
            }
        else
            {
            // handle creation & open failed. Error is returned

            TraceDump( INIT_LEVEL | ERROR_LEVEL, 
                (("WLANLDD: MgmtFrameMemMngr::OnInitialiseMemory: Handle create & open error: %d"),
                r ) );            
            }
        }
    else
        {
        // at this point the shared memory chunk should always exist. However,
        // as it doesn't exist in this case, we return an error

        TraceDump( INIT_LEVEL | ERROR_LEVEL, 
            ("WLANLDD: MgmtFrameMemMngr::OnInitialiseMemory: Error aSharedMemoryChunk is NULL") );
        }    
    
    return ret;    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TUint8* MgmtFrameMemMngr::DoGetNextFreeRxBuffer( TUint aLengthinBytes )
    {
    TraceDump( RX_FRAME, 
        ("WLANLDD: MgmtFrameMemMngr::DoGetNextFreeRxBuffer") );

    // reserve a new Rx buffer. 
    
    TUint8* buffer ( NULL );
    
    if ( iRxFrameMemoryPool )
        {
        buffer = reinterpret_cast<TUint8*>(iRxFrameMemoryPool->Alloc( 
            aLengthinBytes
            + iRxBufAlignmentPadding,
            EFalse ) ); // no need to zero stamp the buffer content
        
        if ( buffer )
            {
            // allocation succeeded
            
            // the alignment padding is before the Rx buffer
            buffer += iRxBufAlignmentPadding;
            
            TraceDump(RX_FRAME, 
               (("WLANLDD: MgmtFrameMemMngr::DoGetNextFreeRxBuffer: addr of allocated Rx buf: 0x%08x"),
               reinterpret_cast<TUint32>(buffer)) );
            }
        else
            {
            TraceDump(RX_FRAME | WARNING_LEVEL, 
               ("WLANLDD: MgmtFrameMemMngr::DoGetNextFreeRxBuffer: WARNING: not enough free memory => failed"));            
            }
        }
    else
        {
         TraceDump(RX_FRAME | WARNING_LEVEL, 
            ("WLANLDD: MgmtFrameMemMngr::DoGetNextFreeRxBuffer: WARNING: no Rx mem pool mgr => failed"));       
        }
        
    return buffer;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool MgmtFrameMemMngr::DoEthernetFrameRxComplete( 
    TDataBuffer*& aBufferStart, 
    TUint32 aNumOfBuffers )
    {
    TraceDump( RX_FRAME, 
        (("WLANLDD: MgmtFrameMemMngr::DoEthernetFrameRxComplete: "
          "aNumOfBuffers: %d"), 
        aNumOfBuffers) );

    TBool ret( EFalse );
    TDataBuffer** metaHdrPtrArray(&aBufferStart);

    if ( iFrameXferBlock )
        {
        for ( TUint i = 0; i < aNumOfBuffers; ++i )
            {
            if ( metaHdrPtrArray[i] )
                {
                iFrameXferBlock->AddRxFrame( metaHdrPtrArray[i] );
                }
            }
        
        if ( iReadStatus == EPending )
            {
            ret = ETrue;
            }
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void MgmtFrameMemMngr::DoMarkRxBufFree( TUint8* aBufferToFree )
    {
    TraceDump( RX_FRAME, 
        (("WLANLDD: MgmtFrameMemMngr::DoMarkRxBufFree: free Rx buf at addr: 0x%08x"),
        reinterpret_cast<TUint32>(aBufferToFree) ) );

    if ( IsMemInUse() && iRxFrameMemoryPool )
        {
        iRxFrameMemoryPool->Free( 
            aBufferToFree
            // take into account the alignment padding 
            - iRxBufAlignmentPadding );
        }
    else
        {
        // the whole Rx memory pool - including aBufferToFree - has already
        // been deallocated, so nothing is done in this case
        TraceDump( RX_FRAME, 
            ("WLANLDD: MgmtFrameMemMngr::DoMarkRxBufFree: Rx memory pool already "
             "deallocated; no action needed") );                
        }    
    }

// ---------------------------------------------------------------------------
//  
// ---------------------------------------------------------------------------
//
TBool MgmtFrameMemMngr::OnReadRequest()
    {
    TBool ret( EFalse );

    if ( IsMemInUse() && iFrameXferBlock )
        {
        if ( iFrameXferBlock->RxFrameAvailable() )
            {
            // there are Rx frames ready for the user mode client retrieval
            ret = ETrue;
            
            // the frame Rx request won't be left pending as the callee will 
            // complete it
            iReadStatus = ENotPending;
            }
        else
            {
            // there are no Rx frames ready for the user mode client retrieval 
            // the Rx request is left pending
            iReadStatus = EPending;
            }
        }
#ifndef NDEBUG
    else
        {
        os_assert( (TUint8*)("WLANLDD: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
        }
#endif        

    TraceDump( RX_FRAME, 
        (("WLANLDD: MgmtFrameMemMngr::OnReadRequest: ret (bool): %d"), 
        ret) );

    return ret;
    }

// ---------------------------------------------------------------------------
// Note! This method is executed in the context of the user mode client 
// thread, but in supervisor mode
// ---------------------------------------------------------------------------
//
TDataBuffer* MgmtFrameMemMngr::GetRxFrame( 
    TDataBuffer* aFrameToFreeInUserSpace )
    {
    TDataBuffer* rxFrame( NULL );
    
    if ( IsMemInUse() && iFrameXferBlock )
        {
        if ( aFrameToFreeInUserSpace )
            {
            FreeRxPacket( aFrameToFreeInUserSpace );
            }
        
        rxFrame = iFrameXferBlock->GetRxFrame();
        }
        
    return rxFrame;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TDataBuffer* MgmtFrameMemMngr::OnWriteEthernetFrame() const
    {
    if ( iTxDataBuffer &&
         ( iTxDataBuffer->GetLength() >= sizeof( SEthernetHeader ) ) )
        {
        return iTxDataBuffer;
        }
    else
        {
#ifndef NDEBUG    
        os_assert( (TUint8*)("WLANLDD: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
#endif        
        return NULL;
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt MgmtFrameMemMngr::RxBufAlignmentPadding() const
    {
    if ( iRxFrameMemoryPool )
        {
        const TInt KMemMgrHdrLen = iRxFrameMemoryPool->HeaderSize();
        const TInt KRemainder ( KMemMgrHdrLen % iRxFrameBufAllocationUnit );
        TInt padding = KRemainder ? 
            ( iRxFrameBufAllocationUnit - KRemainder ) : KRemainder;
            
        TraceDump(INIT_LEVEL, (("WLANLDD: MgmtFrameMemMngr::RxBufAlignmentPadding: %d"), 
            padding));
        
        return padding;
        }
    else
        {
        os_assert( (TUint8*)("WLANLDD: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
        return 0;
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void MgmtFrameMemMngr::OnReleaseMemory( DThread& aThread )
    {
    TraceDump(INIT_LEVEL, ("WLANLDD: MgmtFrameMemMngr::OnReleaseMemory"));

    if ( iRxFrameMemoryPool )
        {
        TraceDump(MEMORY, (("WLANLDD: delete WlanChunk: 0x%08x"), 
            reinterpret_cast<TUint32>(iRxFrameMemoryPool)));        
    
        delete iRxFrameMemoryPool;
        iRxFrameMemoryPool = NULL;
        }

    if ( iParent.SharedMemoryChunk() )
        {
        TraceDump(MEMORY, (("WLANLDD: delete DChunk: 0x%08x"), 
            reinterpret_cast<TUint32>(iParent.SharedMemoryChunk())));        

        if ( iClientChunkHandle >= 0 )
            {
            TraceDump(INIT_LEVEL, 
                (("WLANLDD: MgmtFrameMemMngr::OnReleaseMemory: close shared "
                 "chunk handle: %d"), 
                iClientChunkHandle));
            
            // We have a valid client handle to the shared chunk, so close it 
            Kern::CloseHandle( &aThread, iClientChunkHandle );
            iClientChunkHandle = -1;
            }
        
        // schedule the shared memory chunk for destruction
        Kern::ChunkClose( iParent.SharedMemoryChunk() );
        iParent.SharedMemoryChunk() = NULL;
        MarkMemFree();
        }
    }
