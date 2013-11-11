/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   The one and only packet scheduler
*
*/

/*
* %version: 28 %
*/

#include "config.h"
#include "umacpacketscheduler.h"
#include "umacpacketschedulerclient.h"
#include "UmacContextImpl.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
WlanPacketScheduler::WlanPacketScheduler( 
    MWlanPacketSchedulerClient& aWlanPacketSchedulerClient ) 
    : iPacketSchedulerClient(aWlanPacketSchedulerClient), 
    iCurrent( NULL ), 
    iTxPipelineActive( ETrue ), 
    iNumOfPackets( 0 ), 
    iNumOfNotCompletedPackets( 0 ),
    iFlags( 0 )
    {
    // initially mark all as free
    for ( TPacketIdCntxs::iterator pos = iPacketIdCntxs.begin()
        ; pos != iPacketIdCntxs.end() ; ++pos )
        {
        pos->iFree = ETrue;
        }
    // initially mark all as free
    for ( TPacketElements::iterator pos = iPacketElements.begin()
        ; pos != iPacketElements.end() ; ++pos )
        {
        pos->iFree = ETrue;
        }
    // initially mark all as free
    fill( iQueueStates.begin(), iQueueStates.end(), EQueueNotFull );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanPacketScheduler::Push(
    WlanContextImpl& aCtxImpl,                               
    const TAny* aPacket, 
    TUint32 aLength, 
    WHA::TQueueId aQueueId,
    TUint32 aPacketId,
    const TDataBuffer* aMetaHeader,
    TBool aMore,
    TBool aMulticastData,
    TBool aUseSpecialRatePolicy )
    {
    OsTracePrint( KPacketScheduler, (TUint8*)
        ("UMAC: WlanPacketScheduler::Push: aMulticastData: %d"), 
        aMulticastData );
   
    SElement* free_slot = FreeElementSlot();
    if ( !free_slot )
        {
        // no free element slot was found,
        // which means that all slots are in use 
        // we do nothing else than silently fail the operation
        OsTracePrint( KPacketScheduler | KWarningLevel, (TUint8*)
            ("UMAC: packetscheduler Push: no free slot found -> fail") );

        // packet push failure we must signal
        // set up a flag to signal client at later time to retry 
        // this operation
        iFlags |= KSignalPushPacket;

        return EFalse;
        }

    // one more packet to schedule
    ++iNumOfPackets;

    // store packet send context
    free_slot->iFree = EFalse;  // context is no longer free
    free_slot->iPacket = aPacket;
    free_slot->iLength = aLength;

    // extract a free packet ID context
    SPacketIdCntx* packet_id_cnxt = FreePacketIdCntx();
    if ( !packet_id_cnxt )
        {
        // no free element was found
        // we should allways enough of elements available
        // so this issue means that we have either:
        // 1) programming error
        // 2) too small element storage
        // in any case the issue must be solved at compile time not runtime
        OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
        return EFalse;
        }

    ++iNumOfNotCompletedPackets;
    OsTracePrint( KPacketScheduler, (TUint8*)
        ("UMAC: WlanPacketScheduler::Push: the nbr of packets not yet completed by WHA layer is now: %d"),
        iNumOfNotCompletedPackets );

    // store packet ID context
    packet_id_cnxt->iQueueId = aQueueId;
    packet_id_cnxt->iFrameId = aPacketId;
    packet_id_cnxt->iMetaHeader = aMetaHeader;
    packet_id_cnxt->iFree = EFalse; // this id is no longer free
    packet_id_cnxt->iMulticastData = aMulticastData;
    packet_id_cnxt->iUseSpecialRatePolicy = aUseSpecialRatePolicy;
    
    // link packet element to packet id context
    free_slot->iPacketIdCntx = packet_id_cnxt;
    
    if ( iCurrent )
        {
        if ( iTxPipelineActive )
            {
            // it is not logical to have a valid current pointer 
            // when the tx pipeline is active
            OsAssert( (TUint8*)("UMAC * panic"), 
                (TUint8*)(WLAN_FILE), __LINE__ );
            }

        // we have a existing current pointer 
        // it means that the tx pipeline is stopped
        // lets see if the pushed packet has a 
        // queue full status and proceed from that

        if ( iQueueStates[packet_id_cnxt->iQueueId] == EQueueNotFull )
            {
            // as the pushed packet goes to non full queue let's 
            // set the one with highest priority as the current packet to send
            if ( Priority( packet_id_cnxt->iQueueId ) > 
                 Priority( iCurrent->iPacketIdCntx->iQueueId ) )
                {
                iCurrent = free_slot;
                }
            else   
                {
                // no action 
                }
            }
        else   // --- != EQueueNotFull ---
            {
            // pushed packet queue is full -> no action
            OsTracePrint( 
                KPacketScheduler, (TUint8*)
                ("UMAC: WlanPacketScheduler::Push: queue for the pushed packet is full, queue id: %d"),
                packet_id_cnxt->iQueueId );
            }
        }
    else    // --- iCurrent ---
        {
        // we do not have a valid current pointer, which means that all
        // the packets pending inside the scheduler have a queue full status
        // tough luck, but we can live with it...
        if ( iQueueStates[packet_id_cnxt->iQueueId] == EQueueNotFull )
            {
            // the pushed packet queue is not full queue
            // so we shall mark it as the current one
            iCurrent = free_slot;
            }
        }

    if ( iTxPipelineActive && iCurrent )
        {
        // packet scheduling feasible
        // as tx pipeline is active and we have a current packet to be send
        iPacketSchedulerClient.CallPacketSchedule( aCtxImpl, aMore );
        } 

    if (
        // push packet signalling to client flagged
        iFlags & KSignalPushPacket 
        // AND
        && 
        // packet scheduler is not full
        !Full() )
        {
        iFlags &= ~KSignalPushPacket;
        iPacketSchedulerClient.OnPacketPushPossible( aCtxImpl );
        }

    return ETrue;
    }

// ---------------------------------------------------------------------------
// make internal state as empty
// ---------------------------------------------------------------------------
//
void WlanPacketScheduler::Flush( WlanContextImpl& aCtxImpl )
    {
    // remove all pending packet transmission entrys
    // and call correct completion method for user plane packets
    OsTracePrint( 
        KPacketScheduler, 
        (TUint8*)("UMAC: WlanPacketScheduler::Flush") );

    for ( TPacketElements::iterator pos = iPacketElements.begin(); 
          pos != iPacketElements.end(); 
          ++pos )
        {
        if ( pos->iFree == EFalse )
            // slot in use -> complete the packet and release the slot
            {
            pos->iFree = ETrue;
            iPacketSchedulerClient.OnPacketFlushEvent( 
                aCtxImpl, 
                pos->iPacketIdCntx->iFrameId,
                const_cast<TDataBuffer*>(pos->iPacketIdCntx->iMetaHeader) );

            // as we won't be getting a packet tx completion for this packet
            // mark also the corresponding packet context free
            pos->iPacketIdCntx->iFree = ETrue;
            --iNumOfNotCompletedPackets;
            }
        }

    // NOTE: we do not clear other packet ID contexts as there can be pending
    // packet transmissions, which means we should get packet transmission  
    // callbacks and we must map the context then

    iCurrent = NULL;
    iNumOfPackets = 0;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanPacketScheduler::SchedulePackets( 
    WlanContextImpl& aCtxImpl,
    TBool aMore )
    {
    if ( !(iTxPipelineActive && iCurrent) )
        {
        // nothing to do
        return;
        }

    typedef 
        Carray<
        SPacketIdCntx*, 
        KNumOfElements, 
        EFalse,                 // no delete pointees
        NoCopy<SPacketIdCntx*>  // disallow copying
        > TPacketIdCntxsPtrs;

    // this critter stores the xferred packet id contexes in case of
    // synchronous xfer occurs and we must manullay call
    // completion method for the frame ids
    TPacketIdCntxsPtrs packetid_cntxs;
    TPacketIdCntxsPtrs::iterator pos(  packetid_cntxs.begin() );
    
    WHA::TStatus status ( WHA::KSuccess );

    // send all packets that are feasible for sending
    // as long as the adpatation layer does not stop
    // the tx pipeline
    do 
        {
        // store current packet id context
        *pos = iCurrent->iPacketIdCntx;


        // trace the frame critter
        OsTracePrint( KPacketScheduler, 
            (TUint8*)("UMAC: scheduling dot11 packet for tx"),
            *(reinterpret_cast<const Sdot11MacHeader*>(iCurrent->iPacket)) );
        OsTracePrint( KPacketScheduler, (TUint8*)
            ("UMAC: WlanPacketScheduler::SchedulePackets: queue ID: %d"), 
            (*pos)->iQueueId );
        OsTracePrint( KPacketScheduler, (TUint8*)
            ("UMAC: WlanPacketScheduler::SchedulePackets: frame ID: %d"), 
            (*pos)->iFrameId );
        OsTracePrint( KPacketScheduler, 
            (TUint8*)("UMAC: WlanPacketScheduler::SchedulePackets: packet id: 0x%08x"),
            reinterpret_cast<WHA::TPacketId>(*pos) );

        // are multiple packets ready for sending in the same context
        TBool morePackets ( aMore || MultipleReadyPacketsPending() );
        
        OsTracePrint( KPacketScheduler, (TUint8*)
            ("UMAC: WlanPacketScheduler::SchedulePackets: more: %d"), 
            morePackets );
    
        // determine and store the current Tx rate & rate policy to be used 
        // with the Tx queue in question
        //
        TUint8 txPolicyId ( 0 );
        aCtxImpl.TxRatePolicy( 
            iCurrent->iPacketIdCntx->iQueueId,
            iCurrent->iPacketIdCntx->iUseSpecialRatePolicy,
            iCurrent->iPacketIdCntx->iRequestedTxRate, 
            txPolicyId );

        OsTracePrint( KPacketScheduler, (TUint8*)
            ("UMAC: WlanPacketScheduler::SchedulePackets: txRate: 0x%08x"), 
            iCurrent->iPacketIdCntx->iRequestedTxRate );
        OsTracePrint( KPacketScheduler, (TUint8*)
            ("UMAC: WlanPacketScheduler::SchedulePackets: txPolicyId: %d"), 
            txPolicyId );
        
        const TUint8 KSizeOfTuint32 ( sizeof( TUint32 ) );
        const TUint8 KOffBy ( 
            reinterpret_cast<TUint32>(iCurrent->iPacket) % KSizeOfTuint32 );
            
        if ( KOffBy )
            {
            // The frame start address is not 32 bit aligned. This can
            // happen e.g. if the frame was left pending during
            // roaming, and we roamed from a non-QoS nw to a QoS nw, or vice
            // versa. That makes the alignment to be off by 2 bytes. The only
            // way to get the start address correctly aligned is to copy the 
            // whole frame. Note, that we always leave enough empty space at 
            // the end of the frame buffer to enable this
            OsTracePrint( KWsaTxDetails, (TUint8*)
                ("UMAC: WlanPacketScheduler::SchedulePackets: Frame start address 0x%08x not aligned; fixing it"), 
                reinterpret_cast<TUint32>(iCurrent->iPacket) );
            os_memcpy( 
                reinterpret_cast<TUint8*>(
                    const_cast<TAny*>(iCurrent->iPacket)) + 
                        ( KSizeOfTuint32 - KOffBy ),
                reinterpret_cast<const TUint8*>(iCurrent->iPacket),
                iCurrent->iLength );

            iCurrent->iPacket = 
                reinterpret_cast<const TUint8*>(iCurrent->iPacket) + 
                ( KSizeOfTuint32 - KOffBy );

            OsTracePrint( KWsaTxDetails, (TUint8*)
                ("UMAC: WlanPacketScheduler::SchedulePackets: New frame start address: 0x%08x"), 
                reinterpret_cast<TUint32>(iCurrent->iPacket) );
            }

        status = aCtxImpl.Wha().SendPacket( 
            iCurrent->iPacket, 
            iCurrent->iLength,
            iCurrent->iPacketIdCntx->iQueueId,
            txPolicyId,
            // note that this value is not relevant when autonomous rate
            // adaptation is being used
            iCurrent->iPacketIdCntx->iRequestedTxRate,
            morePackets,
            // packet id passed is used as an act to a complex type
            reinterpret_cast<WHA::TPacketId>(*pos),            
            aCtxImpl.iWlanMib.dot11CurrentTxPowerLevel,
            aCtxImpl.iWlanMib.dot11MaxTransmitMSDULifetime[
                iCurrent->iPacketIdCntx->iQueueId],
            NULL );

        // store current time in packet context
        iCurrent->iPacketIdCntx->iSendReqTimeStamp = os_systemTime();
            
        ++pos;  // next free slot
        // for the possible following frame submissions within this Tx loop we
        // cannot use this indication of more frames in UMAC adaptation any more
        // as it is valid only for the first submission
        aMore = EFalse;

        OsTracePrint( 
            KPacketScheduler, 
            (TUint8*)("UMAC: WlanPacketScheduler::SchedulePackets: SendPacket status: %d"),
            status);
        
        if ( status == WHA::KPending )
            {
            // stop the tx pipeline
            StopTxPipeLine();
            // packet was accepted for delivery
            --iNumOfPackets;
            iCurrent->iFree = ETrue;    // mark as free
            }
        else if ( status == WHA::KQueueFull )
            {
            OsTracePrint( 
                KPacketScheduler, (TUint8*)
                ("UMAC: WlanPacketScheduler::SchedulePackets: queue for the current packet is full, queue id: %d"),
                iCurrent->iPacketIdCntx->iQueueId );

            // packet was discarded
            // we may schedule from another queue
            // this is done automatically as we are executing a loop
            TranmsitQueueFull( iCurrent->iPacketIdCntx->iQueueId );

            // do not clear context as this packet needs to be resend
            // at some point
            }
        else if ( status == WHA::KSuccessXfer )
            {
            // synchronous xfer occurred and no packet transfer
            // method gets called for the packets send in this context,
            // which means that we must manually call completion method
            // for all of those packets
            --iNumOfPackets;    // packet was accepted for delivery
            iCurrent->iFree = ETrue;    // mark as free

            // call completion method for all stacked packet id contexes
            // as they are part of a synchronous xfer
            for ( TPacketIdCntxsPtrs::iterator beg 
                = packetid_cntxs.begin() 
                ; beg != pos 
                ; ++beg )
                {
                iPacketSchedulerClient.OnPacketTransferComplete(
                    aCtxImpl, 
                    (*beg)->iFrameId,
                    const_cast<TDataBuffer*>((*beg)->iMetaHeader) );
                }

            // now as all have been handled
            // clear the stack by resetting the position 
            // to begin of the buffer 
            pos = packetid_cntxs.begin();
            }
        else if ( status == WHA::KSuccess )
            {
            // this is the success scenario
            --iNumOfPackets; // packet was accepted for delivery
            iCurrent->iFree = ETrue;    // mark as free
            }
        else if ( status == WHA::KSuccessQueueFull )
            {
            // packet was accepted 
            --iNumOfPackets;
            // ... but the destination queue is now full
            TranmsitQueueFull( iCurrent->iPacketIdCntx->iQueueId );
            // we may schedule from another queue
            // this is done automatically as we are executing a loop

            iCurrent->iFree = ETrue;    // mark as free
            }
        else
            {
            // adaptation programming error
            OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
            }

        // select new current packet that is to be transferred,
        // if such exists
        SetNextCurrent();

        // stop packet sending if tx pipeleline is stopped or there 
        // is no current packet to be xferred
        } while ( iCurrent && iTxPipelineActive );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanPacketScheduler::SendPacketTransfer( 
    WlanContextImpl& aCtxImpl, 
    WHA::TPacketId aPacketId )
    {
    OsTracePrint( 
        KPacketScheduler, (TUint8*)
        ("UMAC: WlanPacketScheduler::SendPacketTransfer: packet id: 0x%08x"),
         aPacketId);

    // tx pipeline is always activated by this method
    StartTxPipeLine();

    if ( iCurrent )
        {
        // current packet to be xferred exists
        // notify the client code of this
        iPacketSchedulerClient.CallPacketSchedule( aCtxImpl, EFalse );
        }

    // call packet transfer completion method,
    // with client supplied frame id
    const SPacketIdCntx* packet_id_cnxt 
        = reinterpret_cast<const SPacketIdCntx*>(aPacketId);
    iPacketSchedulerClient.OnPacketTransferComplete( 
        aCtxImpl, 
        packet_id_cnxt->iFrameId,
        const_cast<TDataBuffer*>(packet_id_cnxt->iMetaHeader) );

    if ( // push packet signaling to client flagged
         iFlags & KSignalPushPacket
         // AND
         && 
         // packet scheduler is not full
         !Full() )
        {
        iFlags &= ~KSignalPushPacket;
        iPacketSchedulerClient.OnPacketPushPossible( aCtxImpl );
        }

    if ( // someone is waiting for internal Tx buffer to become available
         aCtxImpl.InternalTxBufBeingWaited() 
         // AND
         && 
         // packet scheduler still is not full
         !Full() )
        {
        aCtxImpl.ClearInternalTxBufBeingWaitedFlag();
        iPacketSchedulerClient.OnPacketPushPossible( aCtxImpl );
        }
    }
 
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanPacketScheduler::SendPacketComplete(
    WlanContextImpl& aCtxImpl, 
    WHA::TStatus aStatus,
    WHA::TPacketId aPacketId,
    WHA::TRate aRate,
    TUint32 aPacketQueueDelay,
    TUint32 aMediaDelay,
    TUint8 aAckFailures )
    {
    OsTracePrint( KPacketScheduler, (TUint8*)
        ("UMAC: WlanPacketScheduler::SendPacketComplete: packet id: 0x%08x"),
        aPacketId);

    // determine the packet context related to this frame
    SPacketIdCntx* packet_id_cnxt 
        = reinterpret_cast<SPacketIdCntx*>(aPacketId);

    // calculate Total Tx Delay for this frame. 
    // Using the TUint type for the result is safe
    const TUint totalTxDelay = 
        os_systemTime() - 
        packet_id_cnxt->iSendReqTimeStamp;

    // as packet has been processed from device transmit queue
    // mark that queue as not full
    iQueueStates[packet_id_cnxt->iQueueId] = EQueueNotFull;
    
    // note the queue via which the packet was transmitted
    const WHA::TQueueId queueId ( packet_id_cnxt->iQueueId );

    // note the originally requested Tx rate 
    const WHA::TRate requestedTxRate( packet_id_cnxt->iRequestedTxRate );
    
    // this context can now be reused
    packet_id_cnxt->iFree = ETrue;
    --iNumOfNotCompletedPackets;
    
    OsTracePrint( KPacketScheduler, (TUint8*)
        ("UMAC: WlanPacketScheduler::SendPacketComplete: the nbr of packets not yet completed by WHA layer is now: %d"),
        iNumOfNotCompletedPackets );
    
    // adjust current packet pointer if needed
    SetCurrentPacket( *packet_id_cnxt );

    if ( iTxPipelineActive && iCurrent )
        {
        // packet scheduling feasible
        // as tx pipeline is active and we have a current packet to be sent
        iPacketSchedulerClient.CallPacketSchedule( aCtxImpl, EFalse );
        } 

    // notify client of event
    iPacketSchedulerClient.OnPacketSendComplete( 
        aCtxImpl,
        aStatus,
        packet_id_cnxt->iFrameId,
        aRate,
        aPacketQueueDelay,
        aMediaDelay,
        totalTxDelay,
        aAckFailures,
        queueId,
        requestedTxRate,
        packet_id_cnxt->iMulticastData );

    if (
        // push packet signalling to client flagged
        iFlags & KSignalPushPacket 
        // AND
        && 
        // packet scheduler is not full
        !Full() )
        {
        iFlags &= ~KSignalPushPacket;
        iPacketSchedulerClient.OnPacketPushPossible( aCtxImpl );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanPacketScheduler::GetWhaTxStatus( 
    const WlanContextImpl& aCtxImpl,
    TWhaTxQueueState& aTxQueueState ) const
    {
    if ( aCtxImpl.QosEnabled() )
        {
        for ( TUint queueId = 0; queueId < EQueueIdMax; ++queueId )
            {
            aTxQueueState[queueId] = 
                static_cast<TTxQueueState>(iQueueStates[queueId]);
            }
        }
    else
        {
        for ( TUint queueId = 0; queueId < EQueueIdMax; ++queueId )
            {
            aTxQueueState[queueId] = 
                static_cast<TTxQueueState>(iQueueStates[ELegacy]);
            }        
        }
    
    return iTxPipelineActive;
    }

// ---------------------------------------------------------------------------
// check do we have more than the current packet ready for transmit
// ---------------------------------------------------------------------------
//
TBool WlanPacketScheduler::MultipleReadyPacketsPending()
    {    
    TUint32 cntr( 0 );

    for ( TPacketElements::iterator pos = iPacketElements.begin() 
        ; pos != iPacketElements.end() 
        ; ++pos )
        {
        if ( pos->iFree == EFalse )
            // element in use
            {
            if ( iQueueStates[pos->iPacketIdCntx->iQueueId] == EQueueNotFull )
                // and in non empty queue
                {
                if ( ++cntr > 1 )
                    // multiple entries found
                    {
                    OsTracePrint( KPacketScheduler, (TUint8*)
                        ("UMAC: WlanPacketScheduler::MultipleReadyPacketsPending: multiple pending entries exist for non-full queue(s)") );
                        
                    // terminate the loop
                    break;
                    }
                }
            }
        }

    return (cntr > 1);
    }

// ---------------------------------------------------------------------------
// select new current packet to be transmitted if one exists
// ---------------------------------------------------------------------------
//
void WlanPacketScheduler::SetNextCurrent()
    {
    // no current exist as we setting a new one
    iCurrent = NULL;

    if ( !iNumOfPackets  )
        {
        // as no packets exist there can not be a current one
        return;
        }

    TPacketElements::iterator pos( iPacketElements.begin() );    
    TUint cntr( iNumOfPackets );
    while ( cntr )
        {
        // process max amount of packets 
        // we have pending inside the scheduler
        if ( pos->iFree == EFalse )
            {
            // element in use -> process it

            if ( iCurrent )
                {
                // a current packet exists
                if (
                    // packet is in non full queue 
                    iQueueStates[pos->iPacketIdCntx->iQueueId] 
                    == EQueueNotFull
                    // AND 
                    && 
                    // has higher prioty than the current packet
                    ( Priority( pos->iPacketIdCntx->iQueueId ) 
                    > Priority( iCurrent->iPacketIdCntx->iQueueId ) ) )
                    {
                    // which means it is the new current packet
                    iCurrent = pos;
                    }
                }
            else    // --- iCurrent ---
                {
                // there is no current packet

                if (
                    // packet is in non full queue 
                    iQueueStates[pos->iPacketIdCntx->iQueueId] 
                    == EQueueNotFull )
                    {
                    // which means it is the new current packet
                    iCurrent = pos;
                    }
                }
            
            --cntr;
            }
        ++pos;
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
WlanPacketScheduler::SPacketIdCntx* WlanPacketScheduler::FreePacketIdCntx()
    {
    const TPacketIdCntxsPredicate unary_predicate;

    // find first free element
    SPacketIdCntx* pos 
        = find_if( 
        iPacketIdCntxs.begin(), 
        iPacketIdCntxs.end(), 
        unary_predicate );

    return (pos != iPacketIdCntxs.end() ? pos : NULL);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
WlanPacketScheduler::SElement* WlanPacketScheduler::FreeElementSlot()
    {
    const TElementPredicate unary_predicate;

    // find first free element
    SElement* pos 
        = find_if( 
        iPacketElements.begin(), 
        iPacketElements.end(), 
        unary_predicate );

    return (pos != iPacketElements.end() ? pos : NULL);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanPacketScheduler::SetCurrentPacket( 
    const SPacketIdCntx& aCompletedCntx )
    {
    TBool skip_current( EFalse );

    if ( iCurrent )
        {
        // current packet to be scheduled exists
        
        if ( Priority( iCurrent->iPacketIdCntx->iQueueId ) >= 
             Priority( aCompletedCntx.iQueueId ) )
            {
            // the current packet has at least equal priority as the
            // processed one, which means that there is no need
            // to adjust the current pointer

            // skip setting current
            skip_current = ETrue;
            }
        else
            {
            // processed packet has a higher priority as the current one,
            // which means that we have to check do we have packets in that
            // queue as the new current packet have to be set from those
            // packets (as they have higher priority)
            }
        }
    else    // --- iCurrent ---
        {
        // there is no current packet, which means that we have to check do we
        // have packets in the same queue as the processed one
        // as the current packet have to be set from those
        // packets (if any exists)        
        }

    // check and adjust if needed the current packet pointer
    // to point to the highest prioriy packet in non full transmit queue

    if ( skip_current == EFalse )
        {
        TPacketElements::iterator pos( iPacketElements.begin() );    
        TUint cntr( iNumOfPackets );
        while ( cntr )
            {
            // process max amount of packets 
            // we have pending inside the scheduler
            if ( pos->iFree == EFalse )
                {
                // element in use -> process it
                if ( iCurrent )
                    {
                    // a current packet exists

                    if ( // packet is in non full queue 
                         iQueueStates[pos->iPacketIdCntx->iQueueId] 
                         == EQueueNotFull
                         // AND 
                         && 
                         // has higher prioty than the current packet
                         ( Priority( pos->iPacketIdCntx->iQueueId ) 
                         > Priority( iCurrent->iPacketIdCntx->iQueueId ) ) )
                        {
                        // which means it is the new current packet
                        iCurrent = pos;
                        }
                    }
                else    // --- iCurrent ---
                    {
                    // there is no current packet
                    if ( // packet is in non full queue 
                         iQueueStates[pos->iPacketIdCntx->iQueueId] 
                         == EQueueNotFull )
                        {
                        // which means it is the new current packet
                        iCurrent = pos;
                        }
                    }
            
                --cntr;
                }
            ++pos;
            }
        }
    }
