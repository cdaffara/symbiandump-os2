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
* Description:   Declaration of the MWlanPacketSchedulerClient class.
*
*/

/*
* %version: 14 %
*/

#ifndef M_WLANPACKETSCHEDULERCLIENT_H
#define M_WLANPACKETSCHEDULERCLIENT_H

#ifndef RD_WLAN_DDK
#include <wha.h>
#else
#include <wlanwha.h>
#endif

class WlanContextImpl;

/**
 *  Packet scheduler client (callback) interface declaration
 *
 *  @lib wlanumac.lib
 *  @since S60 v3.1
 */
class MWlanPacketSchedulerClient 
    {

public:

    /**
     * Method called when packet has been transferred to the WLAN device
     *
     * @since S60 3.1
     * @param aCtxImpl global state machine context
     * @param aPacketId packet whose transfer is complete
     * @param aMetaHeader frame meta header
     */
    virtual void OnPacketTransferComplete( 
        WlanContextImpl& aCtxImpl, 
        TUint32 aPacketId,
        TDataBuffer* aMetaHeader ) = 0;

    /**
     * Method called when packet has been processed 
     * from WLAN devices transmit queue
     *
     * @since S60 3.1
     * @param aCtxImpl the one and only global state machine context
     * @param aPacketId packet which transfer is complete
     * @param aRequestedRate Tx rate that was originally requested
     * @param aMulticastData ETrue if the packet in question is a multicast
     *                       data frame
     */
    virtual void OnPacketSendComplete(
        WlanContextImpl& aCtxImpl, 
        WHA::TStatus aStatus,
        TUint32 aPacketId,
        WHA::TRate aRate,
        TUint32 aPacketQueueDelay,
        TUint32 aMediaDelay,
        TUint aTotalTxDelay,        
        TUint8 aAckFailures,
        WHA::TQueueId aQueueId,
        WHA::TRate aRequestedRate,
        TBool aMulticastData ) = 0;

    /**
     * Method called when packet has been flushed (removed)
     * from packet scheduler
     *
     * @since S60 3.1
     * @param aCtxImpl global state machine context
     * @param aPacketId packet that was flushed
     * @param aMetaHeader frame meta header
     */
    virtual void OnPacketFlushEvent(
        WlanContextImpl& aCtxImpl, 
        TUint32 aPacketId,
        TDataBuffer* aMetaHeader ) = 0;

    /**
     * Method called when packet scheduling method should be called,
     * as there exists a packet that is suitable for transmission
     * NOTE: if any other packet scheduler method is called within this
     * context the result is undefined
     *
     * @since S60 3.1
     * @param aCtxImpl the one and only global state machine context
     * @param aMore ETrue if another frame is also ready to be transmitted
     *              EFalse otherwise
     */
    virtual void CallPacketSchedule( 
        WlanContextImpl& aCtxImpl,
        TBool aMore ) = 0;

    /**
     * Method called when packet push is guaranteed to succeed.
     * This method is only called once sometime after a packet push
     * has failed 
     *
     * @since S60 3.1
     * @param aCtxImpl the one and only global state machine context
     */
    virtual void OnPacketPushPossible( WlanContextImpl& aCtxImpl ) = 0;
    };


#endif // M_WLANPACKETSCHEDULERCLIENT_H
