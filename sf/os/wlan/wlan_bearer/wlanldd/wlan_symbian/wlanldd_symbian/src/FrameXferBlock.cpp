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
* Description:   Implementation of the RFrameXferBlock class.
*
*/

/*
* %version: 17 %
*/

#include "WlLddWlanLddConfig.h"
#include "FrameXferBlock.h"
#include "wlanlddcommon.h"
#include "algorithm.h"
#include <kernel.h> // for Kern::SystemTime()


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void RFrameXferBlockBase::KeInitialize()
    {
    iThisAddrKernelSpace = reinterpret_cast<TUint32>(this);

    for ( TUint j = 0; j < TDataBuffer::KFrameTypeMax; ++j )
        {
        iTxOffset[j] = 0;
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void RFrameXferBlockBase::KeSetTxOffsets( 
    TUint32 aEthernetFrameTxOffset,
    TUint32 aDot11FrameTxOffset,
    TUint32 aSnapFrameTxOffset )
    {
    iTxOffset[TDataBuffer::KEthernetFrame] = aEthernetFrameTxOffset;
    iTxOffset[TDataBuffer::KDot11Frame] = aDot11FrameTxOffset;
    iTxOffset[TDataBuffer::KSnapFrame] = aSnapFrameTxOffset;
    iTxOffset[TDataBuffer::KEthernetTestFrame] = aEthernetFrameTxOffset;
        
    for ( TUint i = 0; i < TDataBuffer::KFrameTypeMax ; ++i )
        {
        TraceDump( RX_FRAME, 
            (("WLANLDD: RFrameXferBlockBase::KeSetTxOffsets: offset: %d"), 
            iTxOffset[i]) );        
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void RFrameXferBlock::Initialize( TUint32 aTxBufLength )
    {
    // perform base class initialization first
    KeInitialize();
    
    iTxDataBuffer = NULL;
    iTxBufLength = aTxBufLength;
    
    iRxQueueFront = NULL;
    iRxQueueRear = NULL;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void RFrameXferBlock::AddRxFrame( TDataBuffer* aFrame )
    {
    if ( aFrame )
        {
        aFrame->iNext = NULL;
    
        TraceDump( RX_FRAME, 
            (("WLANLDD: RFrameXferBlock::AddRxFrame: "
              "add to queue metahdr addr: 0x%08x"), 
            aFrame));
        
        if ( iRxQueueRear )
            {
            iRxQueueRear->iNext = aFrame;
            iRxQueueRear = aFrame;
            }
        else
            {
            iRxQueueFront = aFrame;
            iRxQueueRear = aFrame;
            }
        }
    }

// -----------------------------------------------------------------------------
// Note! This method is executed in the context of the user mode client 
// thread, but in supervisor mode
// -----------------------------------------------------------------------------
//
TDataBuffer* RFrameXferBlock::GetRxFrame()
    {
    TDataBuffer* aPacketInKernSpace( NULL );

    if ( iRxQueueFront )
        {
        aPacketInKernSpace = iRxQueueFront;
        
        TraceDump( RX_FRAME, 
            (("WLANLDD: RFrameXferBlock::GetRxFrame: "
              "krn metahdr addr: 0x%08x"), 
            reinterpret_cast<TUint32>(aPacketInKernSpace)));
        
        iRxQueueFront = iRxQueueFront->iNext;
        
        if ( !iRxQueueFront )
            {
            // the queue became empty
            iRxQueueRear = NULL;
            }
        
        return reinterpret_cast<TDataBuffer*>(
                   reinterpret_cast<TUint8*>(aPacketInKernSpace) 
                   - iUserToKernAddrOffset);
        }
    else
        {
        // the Rx queue is empty
        TraceDump( RX_FRAME, 
            (("WLANLDD: RFrameXferBlock::GetRxFrame: "
              "no frames available")));
        
        // return NULL
        return aPacketInKernSpace;
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool RFrameXferBlock::RxFrameAvailable() const
    {
    return reinterpret_cast<TBool>( iRxQueueFront );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void RFrameXferBlockProtocolStack::Initialize()
    {
    // perform base class initialization
    KeInitialize();
    
    iVoiceTxQueue.DoInit();
    iVideoTxQueue.DoInit();
    iBestEffortTxQueue.DoInit();
    iBackgroundTxQueue.DoInit();
    iFreeQueue.DoInit();

    for ( TUint i = 0; i < KTxPoolSizeInPackets; i++ )
        {
        // Set the default values
        
        iDataBuffers[i].FrameType( TDataBuffer::KEthernetFrame );
        iDataBuffers[i].KeClearFlags( TDataBuffer::KTxFrameMustNotBeEncrypted );
        iDataBuffers[i].SetLength( 0 );
        iDataBuffers[i].SetUserPriority( 0 );
        
        iFreeQueue.PutPacket( &iDataBuffers[i] );
        }
    
    iVoiceRxQueueFront = NULL;
    iVoiceRxQueueRear = NULL;

    iVideoRxQueueFront = NULL;
    iVideoRxQueueRear = NULL;
    
    iBestEffortRxQueueFront = NULL;
    iBestEffortRxQueueRear = NULL;
    
    iBackgroundRxQueueFront = NULL;
    iBackgroundRxQueueRear = NULL;
    }

// -----------------------------------------------------------------------------
// Note! This method is executed in the context of the user mode client 
// thread, but in supervisor mode
// -----------------------------------------------------------------------------
//
TDataBuffer* RFrameXferBlockProtocolStack::AllocTxBuffer( 
    const TUint8* aTxBuf,
    TUint16 aBufLength )
    {
    TDataBuffer* packet( NULL );

    if ( aTxBuf )
        {
        // Get Packet from Free Queue
        packet = iFreeQueue.GetPacket();
        
        TraceDump( NWSA_TX_DETAILS, 
            (("WLANLDD: RFrameXferBlockProtocolStack::AllocTxBuffer: krn metahdr addr: 0x%08x"), 
            packet) );
        
        if ( packet )
            {
            packet->KeSetBufferOffset( 
                aTxBuf - reinterpret_cast<TUint8*>(packet) );
            
            packet->SetBufLength( aBufLength );
            
            // reserve appropriate amount of empty space before the Ethernet
            // frame so that there is space for all the necessary headers, including
            // the 802.11 MAC header 
            packet->KeSetOffsetToFrameBeginning( 
                iTxOffset[TDataBuffer::KEthernetFrame] );
            
            // return the user space address
            packet = reinterpret_cast<TDataBuffer*>(
                reinterpret_cast<TUint8*>(packet) - iUserToKernAddrOffset);
            }
        }
    
    TraceDump( NWSA_TX_DETAILS, 
        (("WLANLDD: RFrameXferBlockProtocolStack::AllocTxBuffer: user metahdr addr: 0x%08x"), 
        packet) );
    
    return packet;
    }

// -----------------------------------------------------------------------------
// Note! This method is executed in the context of the user mode client 
// thread, but in supervisor mode
// -----------------------------------------------------------------------------
//
TBool RFrameXferBlockProtocolStack::AddTxFrame( 
    TDataBuffer* aPacketInUserSpace, 
    TDataBuffer*& aPacketInKernSpace,
    TBool aUserDataTxEnabled )
    {
    TBool ret( ETrue );
    aPacketInKernSpace = NULL;

    if ( !aPacketInUserSpace )
        {
#ifndef NDEBUG
        os_assert( (TUint8*)("WLANLDD: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
#endif        
        return ETrue;
        }
    
    TDataBuffer* metaHdrInKernSpace ( reinterpret_cast<TDataBuffer*>(
        reinterpret_cast<TUint8*>(aPacketInUserSpace) + iUserToKernAddrOffset) );
    
    TraceDump( NWSA_TX_DETAILS, 
        (("WLANLDD: RFrameXferBlockProtocolStack::AddTxFrame: user metahdr addr: 0x%08x"), 
        aPacketInUserSpace));

    // put the packet to the correct Tx queue according to the priority to AC
    // mapping defined in WiFi WMM Specification v1.1
    
    if ( aPacketInUserSpace->UserPriority() == 7 || 
         aPacketInUserSpace->UserPriority() == 6 )
        {
        TraceDump( NWSA_TX_DETAILS, 
            (("WLANLDD: add to VO queue; krn metahdr addr: 0x%08x"), 
            reinterpret_cast<TUint32>(metaHdrInKernSpace)));
        
        ret = iVoiceTxQueue.PutPacket( metaHdrInKernSpace );
        
        if ( !ret )
            {
            aPacketInKernSpace = metaHdrInKernSpace;
            }
        
        ret = TxFlowControl( EVoice, aUserDataTxEnabled );
        }
    else if ( aPacketInUserSpace->UserPriority() == 5 || 
              aPacketInUserSpace->UserPriority() == 4 )
        {
        TraceDump( NWSA_TX_DETAILS, 
            (("WLANLDD: add to VI queue; krn metahdr addr: 0x%08x"), 
            reinterpret_cast<TUint32>(metaHdrInKernSpace)));
        
        ret = iVideoTxQueue.PutPacket( metaHdrInKernSpace );
        
        if ( !ret )
            {
            aPacketInKernSpace = metaHdrInKernSpace;
            }
        
        ret = TxFlowControl( EVideo, aUserDataTxEnabled );
        }
    else if ( aPacketInUserSpace->UserPriority() == 2 || 
              aPacketInUserSpace->UserPriority() == 1 )
        {
        TraceDump( NWSA_TX_DETAILS, 
            (("WLANLDD: add to BG queue; krn metahdr addr: 0x%08x"), 
            reinterpret_cast<TUint32>(metaHdrInKernSpace)));
        
        ret = iBackgroundTxQueue.PutPacket( metaHdrInKernSpace );
        
        if ( !ret )
            {
            aPacketInKernSpace = metaHdrInKernSpace;
            }
        
        ret = TxFlowControl( EBackGround, aUserDataTxEnabled );
        }
    else 
        {
        // user priority is 3 or 0 or invalid
        TraceDump( NWSA_TX_DETAILS, 
            (("WLANLDD: add to BE queue; krn metahdr addr: 0x%08x"), 
            reinterpret_cast<TUint32>(metaHdrInKernSpace)));
        
        ret = iBestEffortTxQueue.PutPacket( metaHdrInKernSpace );
        
        if ( !ret )
            {
            aPacketInKernSpace = metaHdrInKernSpace;
            }
        
        ret = TxFlowControl( ELegacy, aUserDataTxEnabled );
        }

    TraceDump( NWSA_TX_DETAILS, 
        (("WLANLDD: VO: %d packets"), 
        iVoiceTxQueue.GetLength()));
    TraceDump( NWSA_TX_DETAILS, 
        (("WLANLDD: VI: %d packets"), 
        iVideoTxQueue.GetLength()));
    TraceDump( NWSA_TX_DETAILS, 
        (("WLANLDD: BE: %d packets"), 
        iBestEffortTxQueue.GetLength()));
    TraceDump( NWSA_TX_DETAILS, 
        (("WLANLDD: BG: %d packets"), 
        iBackgroundTxQueue.GetLength()));
    
    return ret;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TDataBuffer* RFrameXferBlockProtocolStack::GetTxFrame(
    const TWhaTxQueueState& aWhaTxQueueState,
    TBool& aMore )
    {
    TraceDump( NWSA_TX_DETAILS, 
        ("WLANLDD: RFrameXferBlockProtocolStack::GetTxFrame"));
    TraceDump( NWSA_TX_DETAILS, (("WLANLDD: VO: %d packets"), 
        iVoiceTxQueue.GetLength()));
    TraceDump( NWSA_TX_DETAILS, (("WLANLDD: VI: %d packets"), 
        iVideoTxQueue.GetLength()));
    TraceDump( NWSA_TX_DETAILS, (("WLANLDD: BE: %d packets"), 
        iBestEffortTxQueue.GetLength()));
    TraceDump( NWSA_TX_DETAILS, (("WLANLDD: BG: %d packets"), 
        iBackgroundTxQueue.GetLength()));
    
    TDataBuffer* packet = NULL;
    TQueueId queueId ( EQueueIdMax );
    
    if ( TxPossible( aWhaTxQueueState, queueId ) )
        {
        switch ( queueId )
            {
            case EVoice:
                packet = iVoiceTxQueue.GetPacket();
                break;
            case EVideo:
                packet = iVideoTxQueue.GetPacket();
                break;
            case ELegacy:
                packet = iBestEffortTxQueue.GetPacket();
                break;
            case EBackGround:
                packet = iBackgroundTxQueue.GetPacket();
                break;
#ifndef NDEBUG
            default:
                TraceDump(ERROR_LEVEL, (("WLANLDD: queueId: %d"), queueId));
                os_assert( 
                    (TUint8*)("WLANLDD: panic"), 
                    (TUint8*)(WLAN_FILE), 
                    __LINE__ );
#endif                
            }
        
        aMore = TxPossible( aWhaTxQueueState, queueId );
        }
    else
        {
        aMore = EFalse;        
        }

    TraceDump( NWSA_TX_DETAILS, (("WLANLDD: krn meta hdr: 0x%08x"), 
        reinterpret_cast<TUint32>(packet)));
    
    return packet;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TBool RFrameXferBlockProtocolStack::ResumeClientTx( 
    TBool aUserDataTxEnabled ) const
    {
    TBool ret ( EFalse );

    const TInt64 KTimeNow ( Kern::SystemTime() );

    if ( aUserDataTxEnabled )
        {
        // Note that in what follows below we want to consider resuming the
        // client Tx flow only based on the highest priority queue which is
        // currently active. In other words: if we find that some queue is
        // currently active but we don't want to resume the Tx flow yet
        // based on its situation, we must not consider resuming the Tx flow
        // based on any other queue with lower priority.
        
        if ( iVoiceTxQueue.IsActive( KTimeNow ) )
            {
            if ( iVoiceTxQueue.GetLength() < 
                 ( KVoiceTxQueueLen / 2 ) )
                {
                ret = ETrue;
                }
            }
        else if ( iVideoTxQueue.IsActive( KTimeNow ) )
            {
            if ( iVideoTxQueue.GetLength() < 
                 ( KVideoTxQueueLen / 2 ) )
                {
                ret = ETrue;
                }
            }
        else if ( iBestEffortTxQueue.IsActive( KTimeNow ) )
            {
            if ( iBestEffortTxQueue.GetLength() < 
                 ( KBestEffortTxQueueLen / 2 ) )
                {
                ret = ETrue;
                }
            }
        else if ( iBackgroundTxQueue.IsActive( KTimeNow ) )
            {
            if ( iBackgroundTxQueue.GetLength() < 
                 ( KBackgroundTxQueueLen / 2 ) )
                {
                ret = ETrue;
                }
            }
        else
            {
            // none of the Tx queues is currently active (meaning also that
            // they are all empty), but as this method was called, the
            // client Tx flow has to be currently stopped. So now - at the
            // latest - we need to resume the client Tx flow
            ret = ETrue;
            }
        }
    else
        {
        // as client Tx flow has been stopped and user data Tx is disabled
        // (which probably means that we are roaming), its not feasible to
        // resume the client Tx flow yet. So, no action needed; 
        // EFalse is returned
        }
    
#ifndef NDEBUG
    if ( ret )
        {
        TraceDump( NWSA_TX, 
            ("WLANLDD: RFrameXferBlockProtocolStack::ResumeClientTx: resume flow from protocol stack"));            
        }
#endif        
    
    return ret;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void RFrameXferBlockProtocolStack::AddRxFrame( TDataBuffer* aFrame )
    {
    if ( !aFrame )
        {
#ifndef NDEBUG
        os_assert( (TUint8*)("WLANLDD: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
#endif        
        return;
        }
    
    aFrame->iNext = NULL;

    if ( aFrame->UserPriority() == 7 || aFrame->UserPriority() == 6 )
        {
        TraceDump( RX_FRAME, 
            (("WLANLDD: RFrameXferBlockProtocolStack::AddRxFrame: "
              "add to VO queue metahdr addr: 0x%08x"), 
            aFrame));
        
        if ( iVoiceRxQueueRear )
            {
            iVoiceRxQueueRear->iNext = aFrame;
            iVoiceRxQueueRear = aFrame;
            }
        else
            {
            iVoiceRxQueueFront = aFrame;
            iVoiceRxQueueRear = aFrame;
            }
        }
    else if ( aFrame->UserPriority() == 5 || aFrame->UserPriority() == 4 )
        {
        TraceDump( RX_FRAME, 
            (("WLANLDD: RFrameXferBlockProtocolStack::AddRxFrame: "
              "add to VI queue metahdr addr: 0x%08x"), 
            aFrame));
        
        if ( iVideoRxQueueRear )
            {
            iVideoRxQueueRear->iNext = aFrame;
            iVideoRxQueueRear = aFrame;
            }
        else
            {
            iVideoRxQueueFront = aFrame;
            iVideoRxQueueRear = aFrame;
            }
        }
    else if ( aFrame->UserPriority() == 2 || aFrame->UserPriority() == 1 )
        {
        TraceDump( RX_FRAME, 
            (("WLANLDD: RFrameXferBlockProtocolStack::AddRxFrame: "
              "add to BG queue metahdr addr: 0x%08x"), 
            aFrame));
        
        if ( iBackgroundRxQueueRear )
            {
            iBackgroundRxQueueRear->iNext = aFrame;
            iBackgroundRxQueueRear = aFrame;
            }
        else
            {
            iBackgroundRxQueueFront = aFrame;
            iBackgroundRxQueueRear = aFrame;
            }
        }
    else 
        {
        // user priority is 3 or 0 or invalid

        TraceDump( RX_FRAME, 
            (("WLANLDD: RFrameXferBlockProtocolStack::AddRxFrame: "
              "add to BE queue metahdr addr: 0x%08x"), 
            aFrame));
        
        if ( iBestEffortRxQueueRear )
            {
            iBestEffortRxQueueRear->iNext = aFrame;
            iBestEffortRxQueueRear = aFrame;
            }
        else
            {
            iBestEffortRxQueueFront = aFrame;
            iBestEffortRxQueueRear = aFrame;
            }
        }
    }

// -----------------------------------------------------------------------------
// Note! This method is executed in the context of the user mode client 
// thread, but in supervisor mode
// -----------------------------------------------------------------------------
//
TDataBuffer* RFrameXferBlockProtocolStack::GetRxFrame()
    {
    TDataBuffer* aPacketInKernSpace( NULL );

    if ( iVoiceRxQueueFront )
        {
        aPacketInKernSpace = iVoiceRxQueueFront;
        
        TraceDump( RX_FRAME, 
            (("WLANLDD: RFrameXferBlockProtocolStack::GetRxFrame: from VO "
              "queue; krn metahdr addr: 0x%08x"), 
            reinterpret_cast<TUint32>(aPacketInKernSpace)));
        
        iVoiceRxQueueFront = iVoiceRxQueueFront->iNext;
        
        if ( !iVoiceRxQueueFront )
            {
            // the queue became empty
            iVoiceRxQueueRear = NULL;
            }
        }
    else if ( iVideoRxQueueFront )
        {
        aPacketInKernSpace = iVideoRxQueueFront;
        
        TraceDump( RX_FRAME, 
            (("WLANLDD: RFrameXferBlockProtocolStack::GetRxFrame: from VI "
              "queue; krn metahdr addr: 0x%08x"), 
            reinterpret_cast<TUint32>(aPacketInKernSpace)));
        
        iVideoRxQueueFront = iVideoRxQueueFront->iNext;
        
        if ( !iVideoRxQueueFront )
            {
            // the queue became empty        
            iVideoRxQueueRear = NULL;
            }
        }
    else if ( iBestEffortRxQueueFront )
        {
        aPacketInKernSpace = iBestEffortRxQueueFront;
        
        TraceDump( RX_FRAME, 
            (("WLANLDD: RFrameXferBlockProtocolStack::GetRxFrame: from BE "
              "queue; krn metahdr addr: 0x%08x"), 
            reinterpret_cast<TUint32>(aPacketInKernSpace)));
        
        iBestEffortRxQueueFront = iBestEffortRxQueueFront->iNext;
        
        if ( !iBestEffortRxQueueFront )
            {
            // the queue became empty        
            iBestEffortRxQueueRear = NULL;
            }
        }
    else if ( iBackgroundRxQueueFront )
        {
        aPacketInKernSpace = iBackgroundRxQueueFront;
        
        TraceDump( RX_FRAME, 
            (("WLANLDD: RFrameXferBlockProtocolStack::GetRxFrame: from BG "
              "queue; krn metahdr addr: 0x%08x"), 
            reinterpret_cast<TUint32>(aPacketInKernSpace)));
        
        iBackgroundRxQueueFront = iBackgroundRxQueueFront->iNext;
        
        if ( !iBackgroundRxQueueFront )
            {
            // the queue became empty
            iBackgroundRxQueueRear = NULL;
            }
        }
    else
        {
        // all Rx queues are empty; no action
        }
    
    if ( aPacketInKernSpace )
        {
        return reinterpret_cast<TDataBuffer*>(
                   reinterpret_cast<TUint8*>(aPacketInKernSpace) 
                   - iUserToKernAddrOffset);
        }
    else
        {
        TraceDump( RX_FRAME, 
            (("WLANLDD: RFrameXferBlockProtocolStack::GetRxFrame: "
              "no frames available")));
        
        // return NULL
        return aPacketInKernSpace;
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool RFrameXferBlockProtocolStack::TxPossible(
    const TWhaTxQueueState& aWhaTxQueueState,
    TQueueId& aQueueId )
    {
    TBool txPossible ( ETrue );
    
    // In queue priority order, try to locate a Tx packet which is for a 
    // non-full WHA Tx queue
    
    if ( aWhaTxQueueState[EVoice] == ETxQueueNotFull && 
         !iVoiceTxQueue.IsEmpty() )
        {
        aQueueId = EVoice;
        
        TraceDump( NWSA_TX_DETAILS, 
            ("WLANLDD: RFrameXferBlockProtocolStack::TxPossible: from VO queue"));
        }
    else if ( aWhaTxQueueState[EVideo] == ETxQueueNotFull &&
              !iVideoTxQueue.IsEmpty() )
        {
        aQueueId = EVideo;
        
        TraceDump( NWSA_TX_DETAILS, 
            ("WLANLDD: RFrameXferBlockProtocolStack::TxPossible: from VI queue"));
        }
    else if ( aWhaTxQueueState[ELegacy] == ETxQueueNotFull &&
              !iBestEffortTxQueue.IsEmpty() )
        {
        aQueueId = ELegacy;
        
        TraceDump( NWSA_TX_DETAILS, 
            ("WLANLDD: RFrameXferBlockProtocolStack::TxPossible: from BE queue"));
        }
    else if ( aWhaTxQueueState[EBackGround] == ETxQueueNotFull &&
              !iBackgroundTxQueue.IsEmpty() )
        {
        aQueueId = EBackGround;
        
        TraceDump( NWSA_TX_DETAILS, 
            ("WLANLDD: RFrameXferBlockProtocolStack::TxPossible: from BG queue"));
        }
    else
        {
        txPossible = EFalse;
        
        TraceDump( NWSA_TX_DETAILS, 
            ("WLANLDD: RFrameXferBlockProtocolStack::TxPossible: no packet for a non-full wha queue"));
        }
    
    return txPossible;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool RFrameXferBlockProtocolStack::TxFlowControl( 
    TQueueId aTxQueue, 
    TBool aUserDataTxEnabled )
    {
    TInt status ( ETrue );
    
    switch ( aTxQueue )
        {
        case EVoice:
            if ( iVoiceTxQueue.IsFull() )
                {
                status = EFalse;
                }
            break;
        case EVideo:
            if ( iVideoTxQueue.IsFull() )
                {
                if ( !aUserDataTxEnabled )
                    {
                    status = EFalse;
                    }
                else if ( !iVoiceTxQueue.IsActive( Kern::SystemTime() ) )
                    {
                    status = EFalse;
                    }
                }
            break;
        case ELegacy:
            {
            const TInt64 KTimeNow ( Kern::SystemTime() );
            
            if ( iBestEffortTxQueue.IsFull() )
                {
                if ( !aUserDataTxEnabled )
                    {
                    status = EFalse;
                    }
                else if ( !iVoiceTxQueue.IsActive( KTimeNow ) && 
                          !iVideoTxQueue.IsActive( KTimeNow ) )
                    {
                    status = EFalse;
                    }
                }
            break;
            }
        case EBackGround:
            {
            const TInt64 KTimeNow ( Kern::SystemTime() );
            
            if ( iBackgroundTxQueue.IsFull() )
                {
                if ( !aUserDataTxEnabled )
                    {
                    status = EFalse;
                    }
                else if ( !iVoiceTxQueue.IsActive( KTimeNow ) && 
                          !iVideoTxQueue.IsActive( KTimeNow ) && 
                          !iBestEffortTxQueue.IsActive( KTimeNow ) )
                    {
                    status = EFalse;
                    }
                }
            break;
            }
#ifndef NDEBUG
        default:
            TraceDump(ERROR_LEVEL, (("WLANLDD: aTxQueue: %d"), aTxQueue));
            os_assert( 
                (TUint8*)("WLANLDD: panic"), 
                (TUint8*)(WLAN_FILE), 
                __LINE__ );
#endif
        }
    
    return status;
    }
