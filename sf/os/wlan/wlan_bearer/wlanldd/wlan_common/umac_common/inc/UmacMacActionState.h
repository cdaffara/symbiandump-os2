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
* Description:   Declaration of the WlanMacActionState class.
*
*/

/*
* %version: 27 %
*/

#ifndef WLANMACACTIONSTATE_H
#define WLANMACACTIONSTATE_H

#include "UmacMacState.h"
#include "umacwhacommandclient.h"

class WlanDot11State;
class WlanWsaComplexCommand;
class WlanWsaCommand;
class WlanDot11InfrastructureMode;
class WlanDot11IbssMode;

/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib wlanumac.lib
*  @since Series 60 3.1
*/
class WlanMacActionState : 
    public WlanMacState,
    public MWlanWhaCommandClient
    {
public:

    /**
    * Destructor.
    */
    virtual inline ~WlanMacActionState();

public:

    virtual void OnWhaCommandResponse( 
        WlanContextImpl& aCtxImpl, 
        WHA::TCommandId aCommandId, 
        WHA::TStatus aStatus,
        const WHA::UCommandResponseParams& aCommandResponseParams,
        TUint32 aAct );

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
    
    virtual void OnPacketPushPossible( WlanContextImpl& aCtxImpl );
    
protected:

    /**
    * C++ default constructor.
    */
    WlanMacActionState() : iDot11HistoryState( NULL ) {};

    void ChangeState( 
        WlanContextImpl& aCtxImpl, 
        WlanDot11State& aPrevState,
        WlanDot11State& aNewState );

    void ChangeState( 
        WlanContextImpl& aCtxImpl, 
        WlanDot11State& aPrevState,
        WlanWsaComplexCommand& aNewState,
        TUint32 aAct = 0 );

    void ChangeState( 
        WlanContextImpl& aCtxImpl, 
        WlanDot11State& aPrevState,
        WlanWsaCommand& aNewState,
        TUint32 aAct = 0 );

    void ChangeState( 
        WlanContextImpl& aCtxImpl, 
        WlanWsaComplexCommand& aPrevState,
        WlanWsaCommand& aNewState );

    void ChangeState( 
        WlanContextImpl& aCtxImpl, 
        WlanWsaComplexCommand& aPrevState,
        WlanDot11State& aNewState );

    /**
     * dot11 protocol statemachine traversal from global state space
     * to dot11infrastructuremode state space
     *
     * @since S60 3.1
     * @param ?arg1 ?description
     * @param ?arg2 ?description
     */
    void ChangeState( 
        WlanContextImpl& aCtxImpl, 
        WlanDot11State& aPrevState,
        WlanDot11InfrastructureMode& aNewState );

    /**
     * dot11 protocol statemachine traversal 
     * within dot11infrastructuremode state space
     *
     * @since S60 3.1
     * @param ?arg1 ?description
     * @param ?arg2 ?description
     */
    void ChangeState( 
        WlanContextImpl& aCtxImpl, 
        WlanDot11InfrastructureMode& aPrevState,
        WlanDot11InfrastructureMode& aNewState );

    /**
     * dot11 protocol statemachine traversal 
     * from dot11infrastructuremode state space to the global state space
     *
     * @since S60 3.1
     * @param ?arg1 ?description
     * @param ?arg2 ?description
     */
    void ChangeState( 
        WlanContextImpl& aCtxImpl, 
        WlanDot11InfrastructureMode& aPrevState,
        WlanDot11State& aNewState );

    /**
     * dot11 protocol statemachine traversal from global state space
     * to dot11ibssmode state space
     *
     * @since S60 3.1
     * @param ?arg1 ?description
     * @param ?arg2 ?description
     */
    void ChangeState( 
        WlanContextImpl& aCtxImpl, 
        WlanDot11State& aPrevState,
        WlanDot11IbssMode& aNewState );

    /**
     * dot11 protocol statemachine traversal 
     * within dot11ibssmode state space
     *
     * @since S60 3.1
     * @param ?arg1 ?description
     * @param ?arg2 ?description
     */
    void ChangeState( 
        WlanContextImpl& aCtxImpl, 
        WlanDot11IbssMode& aPrevState,
        WlanDot11IbssMode& aNewState );

    /**
     * dot11 protocol statemachine traversal 
     * from dot11ibssmode state space to the global state space
     *
     * @since S60 3.1
     * @param ?arg1 ?description
     * @param ?arg2 ?description
     */
    void ChangeState( 
        WlanContextImpl& aCtxImpl, 
        WlanDot11IbssMode& aPrevState,
        WlanDot11State& aNewState );

    inline WlanDot11State& Dot11History();

private:

    inline void Dot11History( WlanDot11State& aState );

    void OnDot11InfrastructureModeStateSpaceEntry( 
        WlanContextImpl& aCtxImpl );

    void OnDot11InfrastructureModeStateSpaceExit( 
        WlanContextImpl& aCtxImpl ) const;

    void OnDot11IbssModeStateSpaceEntry(
        WlanContextImpl& aCtxImpl ) const;

    void OnDot11IbssModeStateSpaceExit(
        WlanContextImpl& aCtxImpl ) const;

    /**
    * Sets the Tx offset for every frame type which can be transmitted
    *
    * @param aCtxImpl global statemachine context
    */
    void SetMgmtSideTxOffsets( WlanContextImpl& aCtxImpl ) const;    

private:    // Data

    /**
    * History (previous) state
    * Not own
    */
    WlanDot11State*   iDot11HistoryState;
    };


#include "UmacMacActionState.inl"

#endif      // WLANMACACTIONSTATE_H

