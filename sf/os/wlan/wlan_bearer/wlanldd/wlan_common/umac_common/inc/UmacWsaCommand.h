/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Declaration of the WlanWsaCommand class.
*
*/

/*
* %version: 22 %
*/

#ifndef WLANWSACOMMAND_H
#define WLANWSACOMMAND_H

#include "UmacMacState.h"

class WlanContextImpl;
class WlanMacActionState;

/**
*  WHA command base class.
*
*  @lib wlanumac.lib
*  @since S60 v3.1
*/
class WlanWsaCommand : public WlanMacState
    {
    
public:

    /**
    * Destructor.
    */
    virtual ~WlanWsaCommand();

public:

    virtual void Exit( WlanContextImpl& aCtxImpl );

    void Act( WlanContextImpl& aCtxImpl, TUint32 aAct ) const;

    inline void History( WlanMacActionState& aState );

protected:

    /**
    * C++ default constructor.
    */
    WlanWsaCommand () : iMacActionHistoryState( NULL ) {};

    void TraverseToHistoryState( 
        WlanContextImpl& aCtxImpl, 
        WHA::TCommandId aCommandId, 
        WHA::TStatus aStatus,
        const WHA::UCommandResponseParams& aCommandResponseParams );

    inline WlanMacActionState& History();

    void ChangeState( 
        WlanContextImpl& aCtxImpl, 
        WlanWsaCommand& aPrevState,
        WlanMacActionState& aNewState ) const;

private:

    /**
    * Transmit a protocol stack frame
    * 
    * The frame to be sent needs to be in 802.3 format
    * @param aCtxImpl global state machine context
    * @param aDataBuffer meta header of the frame to be transmitted
    * @param aMore ETrue if another frame is also ready to be transmitted
    *              EFalse otherwise
    */
    virtual TBool TxData( 
        WlanContextImpl& aCtxImpl,
        TDataBuffer& aDataBuffer,
        TBool aMore );

    virtual void TxMgmtData( 
        WlanContextImpl& aCtxImpl,
        TDataBuffer& aDataBuffer );

    // from wha side

    virtual void Indication( 
        WlanContextImpl& aCtxImpl, 
        WHA::TIndicationId aIndicationId,
        const WHA::UIndicationParams& aIndicationParams );

    // Frame receive

    virtual TAny* RequestForBuffer ( 
        WlanContextImpl& aCtxImpl,             
        TUint16 aLength );

    virtual void ReceivePacket( 
        WlanContextImpl& aCtxImpl, 
        WHA::TStatus aStatus,
        const void* aFrame,
        TUint16 aLength,
        WHA::TRate aRate,
        WHA::TRcpi aRcpi,
        WHA::TChannelNumber aChannel,
        TUint8* aBuffer,
        TUint32 aFlags );

    // from packet scheduler

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
        TDataBuffer* aMetaHeader );

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
        TBool aMulticastData );

    /**
    * Method called when Packet Scheduler's packet scheduling method 
    * should be called, as there exists a packet that is suitable for 
    * transmission.
    * NOTE: if any other Packet Scheduler method is called within this
    * context the result is undefined.
    * 
    * @param aMore ETrue if another frame is also ready to be transmitted
    *              EFalse otherwise
    */
    virtual void CallPacketSchedule( 
        WlanContextImpl& aCtxImpl,
        TBool aMore );

    virtual void OnPacketPushPossible( 
        WlanContextImpl& aCtxImpl );
    
private:    // Data

    /**
    * History (previous) state
    * Not own
    */
    WlanMacActionState* iMacActionHistoryState;
    };

#include "UmacWsaCommand.inl"

#endif      // WLANWSACOMMAND_H
