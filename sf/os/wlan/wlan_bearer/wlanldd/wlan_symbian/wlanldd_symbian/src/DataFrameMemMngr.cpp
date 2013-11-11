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
* Description:   Implementation of the DataFrameMemMngr class.
*
*/

/*
* %version: 19 %
*/

#include "WlLddWlanLddConfig.h"
#include "DataFrameMemMngr.h"
#include "osachunk.h"
#include <kernel.h> 
#include <kern_priv.h>

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt DataFrameMemMngr::DoOpenHandle(
    DThread& aThread,
    TSharedChunkInfo& aSharedChunkInfo,
    DChunk* aSharedMemoryChunk )
    {
    TInt ret ( KErrGeneral );

    if ( aSharedMemoryChunk )
        {
        
        // Need to be in critical section while creating handles
        NKern::ThreadEnterCS();

        // Make handle to shared memory chunk for client thread
        TInt r = Kern::MakeHandleAndOpen( &aThread, aSharedMemoryChunk );

        // Leave critical section 
        NKern::ThreadLeaveCS();

        // r: positive value is a handle, negative value is an error code
        if( r >= 0 )
            {
            // mapping success

            TraceDump( INIT_LEVEL, 
                (("WLANLDD: DataFrameMemMngr::DoOpenHandle: Handle create & open ok: handle: %d"),
                r ) );
    
            // store the handle & chunk size
            aSharedChunkInfo.iChunkHandle = r;
            aSharedChunkInfo.iSize = aSharedMemoryChunk->Size();
            iClientChunkHandle = r;

            // store the kernel addresses

            TLinAddr start_of_mem_linear( 0 );
            aSharedMemoryChunk->Address( 0, aSharedChunkInfo.iSize, start_of_mem_linear );

            TraceDump( INIT_LEVEL, 
                (("WLANLDD: DataFrameMemMngr::DoOpenHandle: chunk kernel mode start addr: 0x%08x"),
                start_of_mem_linear ) );

            TUint8* start_of_mem = reinterpret_cast<TUint8*>(start_of_mem_linear );

            const TUint KRxDataChunkSize( 
                aSharedChunkInfo.iSize 
                - ( sizeof( TDataBuffer )
                    + KMgmtSideTxBufferLength
                    + KProtocolStackSideTxDataChunkSize
                    + sizeof( RFrameXferBlock )
                    + sizeof( RFrameXferBlockProtocolStack ) ) );

            TraceDump( INIT_LEVEL, 
                (("WLANLDD: DataFrameMemMngr::DoOpenHandle: KRxDataChunkSize: %d"),
                KRxDataChunkSize ) );

            iRxDataChunk = start_of_mem;

            TraceDump( INIT_LEVEL, 
                (("WLANLDD: DataFrameMemMngr::DoOpenHandle: iRxDataChunk start addr: 0x%08x"),
                reinterpret_cast<TUint32>(iRxDataChunk) ) );
            TraceDump( INIT_LEVEL, 
                (("WLANLDD: DataFrameMemMngr::DoOpenHandle: iRxDataChunk end addr: 0x%08x"),
                reinterpret_cast<TUint32>(iRxDataChunk + KRxDataChunkSize) ) );

            iTxDataChunk = 
                start_of_mem  
                + KRxDataChunkSize 
                + sizeof( TDataBuffer ) 
                + KMgmtSideTxBufferLength;

            TraceDump( INIT_LEVEL, 
                (("WLANLDD: DataFrameMemMngr::DoOpenHandle: iTxDataChunk start addr: 0x%08x"),
                reinterpret_cast<TUint32>(iTxDataChunk) ) );
            TraceDump( INIT_LEVEL, 
                (("WLANLDD: DataFrameMemMngr::DoOpenHandle: iTxDataChunk end addr: 0x%08x"),
                reinterpret_cast<TUint32>(
                    iTxDataChunk + KProtocolStackSideTxDataChunkSize) ) );

            // create the Tx frame memory pool manager to manage the Tx Data
            // chunk
            iTxFrameMemoryPool = new WlanChunk( 
                iTxDataChunk, 
                iTxDataChunk + KProtocolStackSideTxDataChunkSize,
                iTxFrameBufAllocationUnit );
            
            if ( iTxFrameMemoryPool && iTxFrameMemoryPool->IsValid() )
                {
                TraceDump(MEMORY, (("WLANLDD: new WlanChunk: 0x%08x"), 
                    reinterpret_cast<TUint32>(iTxFrameMemoryPool)));

                iFrameXferBlockProtoStack = 
                    reinterpret_cast<RFrameXferBlockProtocolStack*>(
                        start_of_mem  
                        + KRxDataChunkSize 
                        + sizeof( TDataBuffer ) 
                        + KMgmtSideTxBufferLength
                        + KProtocolStackSideTxDataChunkSize
                        + sizeof( RFrameXferBlock ) );

                iFrameXferBlockBase = iFrameXferBlockProtoStack;
                
                TraceDump( INIT_LEVEL, 
                    (("WLANLDD: DataFrameMemMngr::DoOpenHandle: Nif RFrameXferBlock addr: 0x%08x"),
                    reinterpret_cast<TUint32>(iFrameXferBlockProtoStack) ) );
    
                // initiliase xfer block
                iFrameXferBlockProtoStack->Initialize();
                
                iRxBufAlignmentPadding = iParent.RxBufAlignmentPadding();
                
                ret = KErrNone;
                }
            else
                {
                // create failed
                delete iTxFrameMemoryPool;
                iTxFrameMemoryPool = NULL;
                // error is returned
                }
            }
        else
            {
            // handle creation & open failed. Error is returned

            TraceDump( INIT_LEVEL | ERROR_LEVEL, 
                (("WLANLDD: DataFrameMemMngr::OnInitialiseMemory: Handle create & open error: %d"),
                r ) );            
            }
        }
    else
        {
        // at this point the shared memory chunk should always exist. However,
        // as it doesn't exist in this case, we return an error

        TraceDump( INIT_LEVEL | ERROR_LEVEL, 
            ("WLANLDD: DataFrameMemMngr::OnInitialiseMemory: Error aSharedMemoryChunk is NULL") );
        }    
    
    return ret;    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool DataFrameMemMngr::DoEthernetFrameRxComplete( 
    TDataBuffer*& aBufferStart, 
    TUint32 aNumOfBuffers )
    {
    TraceDump( RX_FRAME, 
        (("WLANLDD: DataFrameMemMngr::DoEthernetFrameRxComplete: "
          "aNumOfBuffers: %d"), 
        aNumOfBuffers) );

    TBool ret( EFalse );
    TDataBuffer** metaHdrPtrArray(&aBufferStart);

    if ( iFrameXferBlockProtoStack )
        {
        for ( TUint i = 0; i < aNumOfBuffers; ++i )
            {
            if ( metaHdrPtrArray[i] )
                {
                iFrameXferBlockProtoStack->AddRxFrame( metaHdrPtrArray[i] );
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
TBool DataFrameMemMngr::OnReadRequest()
    {
    TBool ret( EFalse );

    if ( IsMemInUse() && iFrameXferBlockProtoStack )
        {
        if ( iFrameXferBlockProtoStack->RxFrameAvailable() )
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
        (("WLANLDD: DataFrameMemMngr::OnReadRequest: ret (bool): %d"), 
        ret) );

    return ret;
    }

// ---------------------------------------------------------------------------
// Note! This method is executed in the context of the user mode client 
// thread, but in supervisor mode
// ---------------------------------------------------------------------------
//
TDataBuffer* DataFrameMemMngr::GetRxFrame( 
    TDataBuffer* aFrameToFreeInUserSpace )
    {
    TDataBuffer* rxFrame( NULL );
    
    if ( IsMemInUse() && iFrameXferBlockProtoStack )
        {
        if ( aFrameToFreeInUserSpace )
            {            
            FreeRxPacket( aFrameToFreeInUserSpace );
            }
        
        rxFrame = iFrameXferBlockProtoStack->GetRxFrame();
        }
        
    return rxFrame;
    }

// ---------------------------------------------------------------------------
// Note! This method is executed in the context of the user mode client 
// thread, but in supervisor mode
// ---------------------------------------------------------------------------
//
TDataBuffer* DataFrameMemMngr::AllocTxBuffer( TUint aLength )
    {
    TraceDump( NWSA_TX_DETAILS, 
        (("WLANLDD: DataFrameMemMngr::AllocTxBuffer: aLength: %d"), 
        aLength) );
    
    TDataBuffer* metaHdr ( NULL );

    if ( ( !IsMemInUse() ) ||
         ( !iTxFrameMemoryPool ) ||
         ( !iFrameXferBlockProtoStack ) ||
         ( aLength > KMaxEthernetFrameLength ) )
        {
#ifndef NDEBUG
        TraceDump( NWSA_TX_DETAILS, 
            ("WLANLDD: DataFrameMemMngr::AllocTxBuffer: WARNING: either "
             "memory not in use OR max size exceeded. Req. denied") );
        os_assert( 
            (TUint8*)("WLANLDD: panic"), 
            (TUint8*)(WLAN_FILE), 
            __LINE__ );
#endif        
        
        return metaHdr;
        }
    
    const TUint bufLen ( Align4(
        iVendorTxHdrLen +
        KHtQoSMacHeaderLength +  
        KMaxDot11SecurityEncapsulationLength +
        sizeof( SSnapHeader ) +
        aLength +
        KSpaceForAlignment +
        iVendorTxTrailerLen ) ); 
    
    TUint8* buf = reinterpret_cast<TUint8*>(
        iTxFrameMemoryPool->Alloc( bufLen, EFalse ));
    
    if ( buf )
        {
        TraceDump( NWSA_TX_DETAILS, 
            (("WLANLDD: DataFrameMemMngr::AllocTxBuffer: tx buf kern addr: 0x%08x"), 
            reinterpret_cast<TUint32>(buf) ) );
        
        metaHdr = iFrameXferBlockProtoStack->AllocTxBuffer( 
            buf, 
            static_cast<TUint16>(aLength) );
        
        if ( !metaHdr )
            {
            iTxFrameMemoryPool->Free( buf );
            }
        }
    
    return metaHdr;
    }

// ---------------------------------------------------------------------------
// Note! This method is executed in the context of the user mode client 
// thread, but in supervisor mode
// ---------------------------------------------------------------------------
//
TBool DataFrameMemMngr::AddTxFrame( 
    TDataBuffer* aPacketInUserSpace, 
    TDataBuffer*& aPacketInKernSpace,
    TBool aUserDataTxEnabled )
    {
    if ( IsMemInUse() && iFrameXferBlockProtoStack )
        {
        return iFrameXferBlockProtoStack->AddTxFrame( 
                    aPacketInUserSpace, 
                    aPacketInKernSpace,
                    aUserDataTxEnabled );
        }
    else
        {
        return EFalse;
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TDataBuffer* DataFrameMemMngr::GetTxFrame( 
    const TWhaTxQueueState& aTxQueueState,
    TBool& aMore )
    {
    if ( IsMemInUse() && iFrameXferBlockProtoStack )
        {
        return iFrameXferBlockProtoStack->GetTxFrame( aTxQueueState, aMore );
        }
    else
        {
        return NULL;
        }
    }

// ---------------------------------------------------------------------------
// Note! This method is executed also in the context of the user mode client 
// thread, but in supervisor mode
// ---------------------------------------------------------------------------
//
void DataFrameMemMngr::FreeTxPacket( TDataBuffer*& aPacket )
    {
    if ( aPacket )
        {
        if ( iTxFrameMemoryPool )
            {
            // free the actual Tx buffer
            iTxFrameMemoryPool->Free( aPacket->KeGetBufferStart() );
            }
        
        if ( iFrameXferBlockProtoStack )
            {
            // free the meta header
            iFrameXferBlockProtoStack->FreeTxPacket( aPacket );
            }
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool DataFrameMemMngr::ResumeClientTx( TBool aUserDataTxEnabled ) const 
    {
    TBool ret( EFalse );

    if ( iFrameXferBlockProtoStack )
        {
        ret = iFrameXferBlockProtoStack->ResumeClientTx( aUserDataTxEnabled );
        }
    
    return ret;
    }

// ---------------------------------------------------------------------------
// Note! This method is executed in the context of the user mode client 
// thread, but in supervisor mode
// ---------------------------------------------------------------------------
//
TBool DataFrameMemMngr::AllTxQueuesEmpty() const
    {
    TBool ret( EFalse );

    if ( iFrameXferBlockProtoStack )
        {
        ret = iFrameXferBlockProtoStack->AllTxQueuesEmpty();
        }
    
    return ret;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void DataFrameMemMngr::OnReleaseMemory( DThread& aThread )
    {
    TraceDump(INIT_LEVEL, ("WLANLDD: DataFrameMemMngr::OnReleaseMemory"));

    if ( iTxFrameMemoryPool )
        {
        TraceDump(MEMORY, (("WLANLDD: delete WlanChunk: 0x%08x"), 
            reinterpret_cast<TUint32>(iTxFrameMemoryPool)));        
    
        delete iTxFrameMemoryPool;
        iTxFrameMemoryPool = NULL;
        iTxDataChunk = NULL;
        
        MarkMemFree();
        }
    
    if ( iClientChunkHandle >= 0 )
        {
        TraceDump(INIT_LEVEL, 
            (("WLANLDD: DataFrameMemMngr::OnReleaseMemory: close shared chunk "
             "handle: %d"), 
            iClientChunkHandle));
        
        // We have a valid client handle to the shared chunk, so close it 
        Kern::CloseHandle( &aThread, iClientChunkHandle );
        iClientChunkHandle = -1;
        }
    }
