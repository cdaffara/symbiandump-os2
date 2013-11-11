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
* Description:   Declaration of the WlanPacketScheduler class.
*
*/

/*
* %version: 21 %
*/

#ifndef T_WLANPACKETSCHEDULER_H
#define T_WLANPACKETSCHEDULER_H

#ifndef RD_WLAN_DDK
#include <wha.h>
#else
#include <wlanwha.h>
#endif

#include "802dot11.h"

#include "umaccarray.h"

class MWlanPacketSchedulerClient;
class WlanContextImpl;
class TDataBuffer;

/**
 *  802.11 Packet scheduler
 *  When packet transmit is possible the packets are transmitted 
 *  in the priority order 
 *
 *  @lib wlanumac.lib
 *  @since S60 v3.1
 */
class WlanPacketScheduler
    {
    // stores context information about a packet
    struct SPacketIdCntx
        {
        // free or not
        TBool           iFree;
        // id of the send queue
        WHA::TQueueId   iQueueId;
        // frame id used by the packet scheduler client
        TUint32         iFrameId;
        // frame meta header used by the packet scheduler client
        const TDataBuffer* iMetaHeader;
        // requested Tx rate
        WHA::TRate      iRequestedTxRate;
        // ETrue if this is a multicast data frame
        TBool           iMulticastData;
        // time when the packet transmit request was submitted to WHA layer
        TInt64          iSendReqTimeStamp;
        // ETrue if the use of the special rate policy is requested for the
        // transmission of this frame
        TBool           iUseSpecialRatePolicy;
        };

    struct SElement
        {
        // free or not
        TBool           iFree;
        // start of packet buffer to send
        const TAny*     iPacket;
        // length of the packet buffer to send
        TUint32         iLength;
        // link to packet id context
        SPacketIdCntx*  iPacketIdCntx;
        };

    class TPacketIdCntxsPredicate
        {
        public:

            TPacketIdCntxsPredicate() : iKey( ETrue ) {};

            TBool operator() ( const SPacketIdCntx& aEntry ) const
                {
                return aEntry.iFree == iKey;
                }

        private:

            // Prohibit copy constructor.
            TPacketIdCntxsPredicate( const TPacketIdCntxsPredicate& );
            // Prohibit assigment operator.
            TPacketIdCntxsPredicate& operator= ( const TPacketIdCntxsPredicate& );

            const TBool iKey;
        };

    class TElementPredicate
        {
        public:

            TElementPredicate() : iKey( ETrue ) {};
            explicit TElementPredicate( TBool aKey ) : iKey( aKey ) {};

            TBool operator() ( const SElement& aEntry ) const
                {
                return aEntry.iFree == iKey;
                }

        private:

            // Prohibit copy constructor.
            TElementPredicate( const TElementPredicate& );
            // Prohibit assigment operator.
            TElementPredicate& operator= ( const TElementPredicate& );

            const TBool iKey;
        };

    /**
     * max number of packet IDs existing any given time
     */
    static const TUint32   KMaxNumOfPacketIds = 60;
    
    /**
     * storage type for packet ID contexes
     */
    typedef Carray<
        SPacketIdCntx, 
        KMaxNumOfPacketIds, 
        EFalse,                 // no delete pointees
        NoCopy<SPacketIdCntx>   // disallow copying
        > TPacketIdCntxs;
    
    /**
     * number of packets supported in the storage
     * (4 user data - 1 per AC, 1 Wlan Mgmt Client and 1 internal packet)
     */
    static const TUint32   KNumOfElements = 6;
    
    /**
     * storage type for packet contexts
     */
    typedef Carray<
        SElement, 
        KNumOfElements, 
        EFalse,             // no delete pointees
        NoCopy<SElement>    // disallow copying
        > TPacketElements;
    
    /**
     * state of the transmit queue
     */
    enum TQueueState
        {
        EQueueFull,
        EQueueNotFull
        };
    
    /**
     * mapper for transmit queue state
     */
    typedef Carray<
        TQueueState, 
        WHA::EQueueIdMax, 
        EFalse,             // no delete pointees
        NoCopy<TQueueState> // disallow copying
        > TQueueStates;

public:

    /**
     * Constructor.
     *
     * @since S60 3.1
     * @param aWlanPacketSchedulerClient reference to Packet Scheduler Client
     */
    explicit WlanPacketScheduler( 
        MWlanPacketSchedulerClient& aWlanPacketSchedulerClient );

    /**
     * Pushes a packet to be sent to storage.
     * NOTE: if no space is available in the storage the operation fails
     * and packet scheduler informs the client by using a method call
     * when it should call this method again
     *
     * @since S60 3.1
     * @param aCtxImpl the one and only global state machine context
     * @param aPacket the packet to transmit
     * @param aLength length of the packet
     * @param aQueueId id of the queue to use when sending the packet
     * @param aPacketId frame type
     * @param aMetaHeader frame meta header
     * @param aMore ETrue if another frame is also ready to be transmitted
     *              EFalse otherwise
     * @param aMulticastData ETrue if this is a multicast data frame
     * @param aUseSpecialRatePolicy ETrue if use of the special Tx rate
     *        policy is requested for this frame Tx
     * @return ETrue packet was accepted, EFalse otherwise
     */
    TBool Push( 
        WlanContextImpl& aCtxImpl,
        const TAny* aPacket, 
        TUint32 aLength, 
        WHA::TQueueId aQueueId,
        TUint32 aPacketId,
        const TDataBuffer* aMetaHeader,
        TBool aMore,
        TBool aMulticastData,
        TBool aUseSpecialRatePolicy );

    /**
     * Removes all pending packet transmission entrys 
     * meaning the scheduler is empty after this call
     *
     * @since S60 3.1
     * @param aCtxImpl the one and only global state machine context
     */
    void Flush( WlanContextImpl& aCtxImpl );

    /**
     * Schedules a packet and sends it if possible.
     * If no packets to schedule exist, does nothing
     * If packet is sent, it is removed from the storage
     *
     * @since S60 3.1
     * @param aCtxImpl the one and only global state machine context
     * @param aMore ETrue if another frame is also ready to be transmitted
     *              EFalse otherwise
     */
    void SchedulePackets( WlanContextImpl& aCtxImpl, TBool aMore );

    /**
     * Packet transfer event
     *
     * @since S60 3.1
     * @param aCtxImpl the one and only global state machine context
     */
    void SendPacketTransfer( 
        WlanContextImpl& aCtxImpl, 
        WHA::TPacketId aPacketId );

    /**
     * Packet processed from WLAN device transmit queue event
     *
     * @since S60 3.1
     * @param aCtxImpl the one and only global state machine context
     */
    void SendPacketComplete(
        WlanContextImpl& aCtxImpl, 
        WHA::TStatus aStatus,
        WHA::TPacketId aPacketId,
        WHA::TRate aRate,
        TUint32 aPacketQueueDelay,
        TUint32 aMediaDelay,        
        TUint8 aAckFailures );

    /**
     * Gets WHA layer transmission status
     *
     * @param aCtxImpl the one and only global state machine context
     * @param aTxQueueState State (full / not full) of every WHA transmit queue
     * @return ETrue if the Tx pipeline is active, i.e. Tx frame submission
     *         is allowed
     *         EFalse if the Tx pipeline is not active, i.e. Tx frame submission
     *         is not allowed
     */
    TBool GetWhaTxStatus(
        const WlanContextImpl& aCtxImpl, 
        TWhaTxQueueState& aTxQueueState ) const;
    
    /**
     * Are there unsent Tx packets, i.e. packets not yet completed by WHA 
     * layer
     *
     * @since S60 3.2
     * @return ETrue if there are packets not yet completed by WHA layer
     *         EFalse otherwise
     */
    inline TBool UnsentPackets() const;

private:

    // Prohibit copy constructor 
    WlanPacketScheduler( const WlanPacketScheduler& );
    // Prohibit assigment operator 
    WlanPacketScheduler& operator= ( const WlanPacketScheduler& );  

    /**
     * Evalutes if multiple packets are ready for transmitting.
     * Checks does a packet exist in a non-full queue   
     *
     * @since S60 3.1
     * @return ETrue multiple packets ready for transmitting exists, 
     *         EFalse otherwise
     */
    TBool MultipleReadyPacketsPending();

    /**
     * Sets current packet pointer to point 
     * to next current packet if such exists
     *
     * @since S60 3.1
     */
    void SetNextCurrent();

    /**
     * Extracts a free element slot
     *
     * @since S60 3.1
     * @return free element slot, NULL upon failure
     */
    SElement* FreeElementSlot();

    /**
     * Extracts a free packet ID context
     *
     * @since S60 3.1
     * @return free packet ID context, NULL upon failure
     */
    SPacketIdCntx* FreePacketIdCntx();

    /**
     * Sets the current packet pointer point to a new packet to be transmitted,
     * if one exists in a queue defined by supplied parameter.
     * This method is called after packet has been processed from WLAN device's
     * transmit queue. As the current packet pointer is always supposed to 
     * point to the highest priority packet that is in a non-full queue this 
     * method might or might not set the current packet pointer
     *
     * @since S60 3.1
     * @param aCompletedCntx    packet ID context of a packet processed from 
     *                          WLAN devices transmit queue
     */
    void SetCurrentPacket( const SPacketIdCntx& aCompletedCntx );

    /**
     * Sets the state of the tx pipeline to active
     *
     * @since S60 3.1
     * @param aQueueId id of the queue
     */
    inline void StartTxPipeLine();

    /**
     * Sets the state of the given queue to full
     *
     * @since S60 3.1
     * @param aQueueId id of the queue
     */
    inline void TranmsitQueueFull( WHA::TQueueId aQueueId );

    /**
     * Sets the state of the tx pipeline to stop
     *
     * @since S60 3.1
     * @param aQueueId id of the queue
     */
    inline void StopTxPipeLine();

    /**
     * Evaluates can the packet scheduler accept 
     * any more packets or is it full
     *
     * @since S60 3.1
     * @return ETrue if the scheduler is full 
     * and can not accept any more packets, EFalse otherwise
     */
    inline TBool Full() const;
    
    /**
     * Returns the priority of the provided WHA Queue
     *
     * @param aQueueId id of the WHA queue
     * @return Priority of the provided queue
     */
    static inline TUint Priority( WHA::TQueueId aQueueId );

private: // data

    /**
     * this flag is set when we need to signal the Packet Scheduler client
     * when packet push is again possible
     */
    static const TUint32  KSignalPushPacket = (1 << 0);

    /**
     * packet scheduler client
     */
    MWlanPacketSchedulerClient& iPacketSchedulerClient;
    
    /**
     * points to highest priority packet of queue that is not full 
     */
    SElement*       iCurrent;
    /**
     * is tx pipeline active or stopped
     */
    TBool           iTxPipelineActive;
    /**
     * number of packets stored inside scheduler
     */
    TUint32         iNumOfPackets;
    /**
     * number of packets not yet completed by WHA layer
     */
    TUint32         iNumOfNotCompletedPackets;
    /**
     * packet ID contexts
     */    
    TPacketIdCntxs  iPacketIdCntxs;
    /**
     * packet contexts
     */    
    TPacketElements iPacketElements;
    /**
     * transmit queue states
     */
    TQueueStates    iQueueStates;   
    /**
     * holds internal state
     */    
    TUint32         iFlags;
    };

#include "umacpacketscheduler.inl"


#endif // T_WLANPACKETSCHEDULER_H
