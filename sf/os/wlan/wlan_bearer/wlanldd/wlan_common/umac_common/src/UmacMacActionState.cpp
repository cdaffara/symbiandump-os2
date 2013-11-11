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
* Description:   Implementation of the WlanMacActionState class.
*
*/

/*
* %version: 43 %
*/

#include "config.h"
#include "UmacMacActionState.h"
#include "UmacDot11IbssMode.h"
#include "UmacDot11InfrastructureMode.h"
#include "UmacContextImpl.h"
#include "UmacWsaComplexCommand.h"
#include "UmacWsaCommand.h"

// ============================ MEMBER FUNCTIONS ===============================


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanMacActionState::ChangeState( 
    WlanContextImpl& aCtxImpl, 
    WlanDot11State& aPrevState,
    WlanWsaComplexCommand& aNewState,
    TUint32 aAct )
    {
    aNewState.Dot11History( aPrevState );

#ifndef NDEBUG 
    TUint8 length;
    const TUint8* ptr = reinterpret_cast<const TUint8*>
        (aNewState.GetStateName( length ));
    const TUint8* ptr2 = reinterpret_cast<const TUint8*>
        (aCtxImpl.iCurrentMacState->GetStateName( length ));

    OsTracePrint( KUmacProtocolState, (TUint8*)("UMAC: WlanMacActionState::ChangeState(): previous state:") );
    OsTracePrint( KUmacProtocolState, ptr2 );
    OsTracePrint( KUmacProtocolState, (TUint8*)("UMAC: WlanMacActionState::ChangeState(): next state:") );
    OsTracePrint( KUmacProtocolState, ptr );
#endif

    // set ACT 
    aNewState.Act( aCtxImpl, aAct );

    aCtxImpl.iCurrentMacState = &aNewState;
    aCtxImpl.iCurrentMacState->Entry( aCtxImpl );    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanMacActionState::ChangeState( 
    WlanContextImpl& aCtxImpl, 
    WlanDot11State& aPrevState,
    WlanDot11State& aNewState )
    {
    aNewState.Dot11History( aPrevState );

#ifndef NDEBUG 
    TUint8 length;
    const TUint8* ptr = reinterpret_cast<const TUint8*>
        (aNewState.GetStateName( length ));
    const TUint8* ptr2 = reinterpret_cast<const TUint8*>
        (aCtxImpl.iCurrentMacState->GetStateName( length ));

    OsTracePrint( KDot11StateTransit, (TUint8*)
        ("UMAC * dot11state to dot11state traversal") );
    OsTracePrint( KDot11StateTransit, (TUint8*)("previous state:") );
    OsTracePrint( KDot11StateTransit, ptr2 );
    OsTracePrint( KDot11StateTransit, (TUint8*)("next state:") );
    OsTracePrint( KDot11StateTransit, ptr );
#endif

    aPrevState.Exit( aCtxImpl );
    aCtxImpl.iCurrentMacState = &aNewState;
    aCtxImpl.iCurrentMacState->Entry( aCtxImpl );    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanMacActionState::ChangeState( 
    WlanContextImpl& aCtxImpl, 
    WlanWsaComplexCommand& aPrevState,
    WlanWsaCommand& aNewState )
    {
    aNewState.History( aPrevState );

#ifndef NDEBUG 
    TUint8 length;
    const TUint8* ptr = reinterpret_cast<const TUint8*>
        (aNewState.GetStateName( length ));
    const TUint8* ptr2 = reinterpret_cast<const TUint8*>
        (aCtxImpl.iCurrentMacState->GetStateName( length ));

    OsTracePrint( KUmacProtocolState, (TUint8*)("UMAC: WlanMacActionState::ChangeState(): previous state:") );
    OsTracePrint( KUmacProtocolState, ptr2 );
    OsTracePrint( KUmacProtocolState, (TUint8*)("UMAC: WlanMacActionState::ChangeState(): next state:") );
    OsTracePrint( KUmacProtocolState, ptr );
#endif
   
    aCtxImpl.iCurrentMacState = &aNewState;
    aCtxImpl.iCurrentMacState->Entry( aCtxImpl );    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanMacActionState::ChangeState( 
    WlanContextImpl& aCtxImpl, 
    WlanDot11State& aPrevState,
    WlanWsaCommand& aNewState,
    TUint32 aAct )
    {
    aNewState.History( aPrevState );

#ifndef NDEBUG 
    TUint8 length;
    const TUint8* ptr = reinterpret_cast<const TUint8*>
        (aNewState.GetStateName( length ));
    const TUint8* ptr2 = reinterpret_cast<const TUint8*>
        (aCtxImpl.iCurrentMacState->GetStateName( length ));

    OsTracePrint( KUmacProtocolState, (TUint8*)("UMAC: WlanMacActionState::ChangeState(): previous state:") );
    OsTracePrint( KUmacProtocolState, ptr2 );
    OsTracePrint( KUmacProtocolState, (TUint8*)("UMAC: WlanMacActionState::ChangeState(): next state:") );
    OsTracePrint( KUmacProtocolState, ptr );
#endif

    // set ACT 
    aNewState.Act( aCtxImpl, aAct );
    OsTracePrint( 
        KUmacDetails, 
        (TUint8*)
        ("UMAC: WlanMacActionState::ChangeState(): act: %d"), aAct );


    aCtxImpl.iCurrentMacState = &aNewState;
    aCtxImpl.iCurrentMacState->Entry( aCtxImpl );    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanMacActionState::ChangeState( 
    WlanContextImpl& aCtxImpl, 
    WlanWsaComplexCommand& aPrevState,
    WlanDot11State& aNewState )
    {
#ifndef NDEBUG 
    TUint8 length;
    const TUint8* ptr = reinterpret_cast<const TUint8*>
        (aNewState.GetStateName( length ));
    const TUint8* ptr2 = reinterpret_cast<const TUint8*>
        (aCtxImpl.iCurrentMacState->GetStateName( length ));

    OsTracePrint( KUmacProtocolState, (TUint8*)("UMAC: WlanMacActionState::ChangeState(): previous state:") );
    OsTracePrint( KUmacProtocolState, ptr2 );
    OsTracePrint( KUmacProtocolState, (TUint8*)("UMAC: WlanMacActionState::ChangeState(): next state:") );
    OsTracePrint( KUmacProtocolState, ptr );
#endif

    aPrevState.Exit( aCtxImpl );
    aCtxImpl.iCurrentMacState = &aNewState;
    aCtxImpl.iCurrentMacState->Entry( aCtxImpl );    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanMacActionState::ChangeState( 
    WlanContextImpl& aCtxImpl, 
    WlanDot11State& aPrevState,
    WlanDot11InfrastructureMode& aNewState )
    {
    aNewState.Dot11History( aPrevState );

#ifndef NDEBUG 
    TUint8 length;
    const TUint8* ptr = reinterpret_cast<const TUint8*>
        (aNewState.GetStateName( length ));
    const TUint8* ptr2 = reinterpret_cast<const TUint8*>
        (aCtxImpl.iCurrentMacState->GetStateName( length ));

    OsTracePrint( KDot11StateTransit, (TUint8*)
        ("UMAC * dot11state to dot11-infrastructuremode traversal") );
    OsTracePrint( KDot11StateTransit, (TUint8*)("previous state:") );
    OsTracePrint( KDot11StateTransit, ptr2 );
    OsTracePrint( KDot11StateTransit, (TUint8*)("next state:") );
    OsTracePrint( KDot11StateTransit, ptr );
#endif

    aPrevState.Exit( aCtxImpl );
    aCtxImpl.iCurrentMacState = &aNewState;

    // as we are entering dot11infrastructure mode we will
    // do the network specific configuration here
    OnDot11InfrastructureModeStateSpaceEntry( aCtxImpl );

    aCtxImpl.iCurrentMacState->Entry( aCtxImpl );    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanMacActionState::ChangeState( 
    WlanContextImpl& aCtxImpl, 
    WlanDot11InfrastructureMode& aPrevState,
    WlanDot11InfrastructureMode& aNewState )
    {
    aNewState.Dot11History( aPrevState );

#ifndef NDEBUG 
    TUint8 length;
    const TUint8* ptr = reinterpret_cast<const TUint8*>
        (aNewState.GetStateName( length ));
    const TUint8* ptr2 = reinterpret_cast<const TUint8*>
        (aCtxImpl.iCurrentMacState->GetStateName( length ));

    OsTracePrint( KDot11StateTransit, (TUint8*)
        ("UMAC * dot11-infrastructuremode to dot11-infrastructuremode") );
    OsTracePrint( KDot11StateTransit, (TUint8*)("previous state:") );
    OsTracePrint( KDot11StateTransit, ptr2 );
    OsTracePrint( KDot11StateTransit, (TUint8*)("next state:") );
    OsTracePrint( KDot11StateTransit, ptr );
#endif

    aPrevState.Exit( aCtxImpl );
    aCtxImpl.iCurrentMacState = &aNewState;
    aCtxImpl.iCurrentMacState->Entry( aCtxImpl );    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanMacActionState::ChangeState( 
    WlanContextImpl& aCtxImpl, 
    WlanDot11InfrastructureMode& aPrevState,
    WlanDot11State& aNewState )
    {
    aNewState.Dot11History( aPrevState );

#ifndef NDEBUG 
    TUint8 length;
    const TUint8* ptr = reinterpret_cast<const TUint8*>
        (aNewState.GetStateName( length ));
    const TUint8* ptr2 = reinterpret_cast<const TUint8*>
        (aCtxImpl.iCurrentMacState->GetStateName( length ));

    OsTracePrint( KDot11StateTransit, (TUint8*)
        ("UMAC * dot11-infrastructuremode to dot11-state traversal") );
    OsTracePrint( KDot11StateTransit, (TUint8*)("previous state:") );
    OsTracePrint( KDot11StateTransit, ptr2 );
    OsTracePrint( KDot11StateTransit, (TUint8*)("next state:") );
    OsTracePrint( KDot11StateTransit, ptr );
#endif

    aPrevState.Exit( aCtxImpl );
    aCtxImpl.iCurrentMacState = &aNewState;

    // as we are exiting dot11infrastructuremode state space 
    // we will do some magic here
    OnDot11InfrastructureModeStateSpaceExit( aCtxImpl );

    aCtxImpl.iCurrentMacState->Entry( aCtxImpl );    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanMacActionState::ChangeState( 
    WlanContextImpl& aCtxImpl, 
    WlanDot11State& aPrevState,
    WlanDot11IbssMode& aNewState )
    {
    aNewState.Dot11History( aPrevState );

#ifndef NDEBUG 
    TUint8 length;
    const TUint8* ptr = reinterpret_cast<const TUint8*>
        (aNewState.GetStateName( length ));
    const TUint8* ptr2 = reinterpret_cast<const TUint8*>
        (aCtxImpl.iCurrentMacState->GetStateName( length ));

    OsTracePrint( KDot11StateTransit, 
        (TUint8*)("UMAC * dot11-state to dot11-ibssmode traversal") );
    OsTracePrint( KDot11StateTransit, (TUint8*)("previous state:") );
    OsTracePrint( KDot11StateTransit, ptr2 );
    OsTracePrint( KDot11StateTransit, (TUint8*)("next state:") );
    OsTracePrint( KDot11StateTransit, ptr );
#endif

    aPrevState.Exit( aCtxImpl );
    aCtxImpl.iCurrentMacState = &aNewState;

    // as we are entering dot11ibssmode mode we will
    // do the network specific configuration here
    OnDot11IbssModeStateSpaceEntry( aCtxImpl );

    aCtxImpl.iCurrentMacState->Entry( aCtxImpl );      
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanMacActionState::ChangeState( 
    WlanContextImpl& aCtxImpl, 
    WlanDot11IbssMode& aPrevState,
    WlanDot11IbssMode& aNewState )
    {
    aNewState.Dot11History( aPrevState );

#ifndef NDEBUG 
    TUint8 length;
    const TUint8* ptr = reinterpret_cast<const TUint8*>
        (aNewState.GetStateName( length ));
    const TUint8* ptr2 = reinterpret_cast<const TUint8*>
        (aCtxImpl.iCurrentMacState->GetStateName( length ));

    OsTracePrint( KDot11StateTransit, 
        (TUint8*)("UMAC * dot11-ibssmode to dot11-ibssmode traversal") );
    OsTracePrint( KDot11StateTransit, (TUint8*)("previous state:") );
    OsTracePrint( KDot11StateTransit, ptr2 );
    OsTracePrint( KDot11StateTransit, (TUint8*)("next state:") );
    OsTracePrint( KDot11StateTransit, ptr );
#endif

    aPrevState.Exit( aCtxImpl );
    aCtxImpl.iCurrentMacState = &aNewState;
    aCtxImpl.iCurrentMacState->Entry( aCtxImpl );    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanMacActionState::ChangeState( 
    WlanContextImpl& aCtxImpl, 
    WlanDot11IbssMode& aPrevState,
    WlanDot11State& aNewState )
    {
    aNewState.Dot11History( aPrevState );

#ifndef NDEBUG 
    TUint8 length;
    const TUint8* ptr = reinterpret_cast<const TUint8*>
        (aNewState.GetStateName( length ));
    const TUint8* ptr2 = reinterpret_cast<const TUint8*>
        (aCtxImpl.iCurrentMacState->GetStateName( length ));

    OsTracePrint( KDot11StateTransit, 
        (TUint8*)("UMAC * dot11-ibssmode to dot11-state traversal") );
    OsTracePrint( KDot11StateTransit, (TUint8*)("previous state:") );
    OsTracePrint( KDot11StateTransit, ptr2 );
    OsTracePrint( KDot11StateTransit, (TUint8*)("next state:") );
    OsTracePrint( KDot11StateTransit, ptr );
#endif

    aPrevState.Exit( aCtxImpl );
    aCtxImpl.iCurrentMacState = &aNewState;

    // as we are exiting dot11ibssmode state space 
    // we will do some magic here
    OnDot11IbssModeStateSpaceExit( aCtxImpl );

    aCtxImpl.iCurrentMacState->Entry( aCtxImpl );    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanMacActionState::OnWhaCommandResponse( 
    WlanContextImpl& /*aCtxImpl*/, 
    WHA::TCommandId /*aCommandId*/, 
    WHA::TStatus /*aStatus*/,
    const WHA::UCommandResponseParams& /*aCommandResponseParams*/,
    TUint32 /*aAct*/ )
    {
    // not supported in default handler
    OsAssert( (TUint8*)("UMAC * panic"), (TUint8*)(WLAN_FILE), __LINE__ );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanMacActionState::TxData( 
    WlanContextImpl& aCtxImpl,
    TDataBuffer& aDataBuffer,
    TBool aMore )
    {
    // only supported in dot11 state. so forward the call
    return iDot11HistoryState->TxData( aCtxImpl, aDataBuffer, aMore );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanMacActionState::TxMgmtData( 
    WlanContextImpl& aCtxImpl,
    TDataBuffer& aDataBuffer )
    {
    // only supported in dot11 state. so forward
    iDot11HistoryState->TxMgmtData( aCtxImpl, aDataBuffer );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanMacActionState::Indication( 
    WlanContextImpl& aCtxImpl, 
    WHA::TIndicationId aIndicationId,
    const WHA::UIndicationParams& aIndicationParams )
    {
    // only supported in dot11 state. so forward
    iDot11HistoryState->Indication( 
        aCtxImpl, 
        aIndicationId, 
        aIndicationParams );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TAny* WlanMacActionState::RequestForBuffer( 
    WlanContextImpl& aCtxImpl,             
    TUint16 aLength )
    {
    // only supported in dot11 state. so forward
    return iDot11HistoryState->RequestForBuffer( 
        aCtxImpl, 
        aLength );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanMacActionState::ReceivePacket( 
            WlanContextImpl& aCtxImpl, 
            WHA::TStatus aStatus,
            const void* aFrame,
            TUint16 aLength,
            WHA::TRate aRate,
            WHA::TRcpi aRcpi,
            WHA::TChannelNumber aChannel,
            TUint8* aBuffer,
            TUint32 aFlags )
    {
    // only supported in dot11 state. so forward
    iDot11HistoryState->ReceivePacket( 
        aCtxImpl, 
        aStatus,
        aFrame,
        aLength,
        aRate,
        aRcpi,        
        aChannel,
        aBuffer,
        aFlags );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanMacActionState::OnPacketTransferComplete( 
    WlanContextImpl& aCtxImpl, 
    TUint32 aPacketId,
    TDataBuffer* aMetaHeader )
    {
    // only supported in dot11 state. so forward
    iDot11HistoryState->OnPacketTransferComplete( 
        aCtxImpl, 
        aPacketId, 
        aMetaHeader );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanMacActionState::OnPacketSendComplete(
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
    TBool aMulticastData )
    {
    // only supported in dot11 state. so forward
    iDot11HistoryState->OnPacketSendComplete( 
        aCtxImpl, 
        aStatus, 
        aPacketId, 
        aRate, 
        aPacketQueueDelay,
        aMediaDelay,
        aTotalTxDelay,
        aAckFailures,
        aQueueId,
        aRequestedRate,
        aMulticastData );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanMacActionState::CallPacketSchedule( 
    WlanContextImpl& aCtxImpl,
    TBool aMore )
    {
    // only supported in dot11 state. so forward
    iDot11HistoryState->CallPacketSchedule( aCtxImpl, aMore );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanMacActionState::OnPacketPushPossible( 
    WlanContextImpl& aCtxImpl )
    {
    // only supported in dot11 state. so forward
    iDot11HistoryState->OnPacketPushPossible( aCtxImpl );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanMacActionState::OnDot11InfrastructureModeStateSpaceEntry( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( KDot11StateTransit, (TUint8*)
        ("UMAC: WlanMacActionState::OnDot11InfrastructureModeStateSpaceEntry()") );

    // configure NULL data frame BSSID and DA addresses & to DS bit
    SNullDataFrame& null_data = aCtxImpl.NullDataFrame();
    null_data.iAddress1 = aCtxImpl.GetBssId();
    null_data.iAddress3 = aCtxImpl.GetBssId();
    null_data.SetToDsBit();

    // configure QoS NULL data frame BSSID and DA addresses & to DS bit
    SHtQosNullDataFrame& qosNullData = aCtxImpl.QosNullDataFrame();
    qosNullData.iQosDataFrameHdr.iHdr.iAddress1 = aCtxImpl.GetBssId();
    qosNullData.iQosDataFrameHdr.iHdr.iAddress3 = aCtxImpl.GetBssId();
    qosNullData.iQosDataFrameHdr.iHdr.SetToDsBit();
    // ... and order bit
    if ( aCtxImpl.HtSupportedByNw() )
        {
        // HT control field needs to be present => order bit needs to be set
        qosNullData.iQosDataFrameHdr.iHdr.SetOrderBit();
        }
    else
        {
        // HT control field won't be present => order bit needs to be cleared
        qosNullData.iQosDataFrameHdr.iHdr.ClearOrderBit();
        }
    
    // set the privacy mode filter
    aCtxImpl.SetActivePrivacyModeFilter( 
        aCtxImpl, aCtxImpl.EncryptionStatus() );

    // set frame Tx offsets
    SetMgmtSideTxOffsets( aCtxImpl );
    SetProtocolStackTxOffset( aCtxImpl );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanMacActionState::OnDot11InfrastructureModeStateSpaceExit( 
    WlanContextImpl& aCtxImpl ) const
    {
    // we are departing from dot11infrastructuremode state space
    
    OsTracePrint( KDot11StateTransit, 
        (TUint8*)("UMAC: WlanMacActionState::OnDot11InfrastructureModeStateSpaceExit()") );

    // reset current dot11 power management mode state.
    aCtxImpl.CurrentDot11PwrMgmtMode( WHA::KPsDisable );
    
    // reset the following items for the (potentially following) next roaming
    // connection
    aCtxImpl.ApTestOpportunitySeekStarted( EFalse );
    aCtxImpl.ApTestOpportunityIndicated( EFalse );
    
    // flush packet scheduler from all pending packets
    aCtxImpl.FlushPacketScheduler();

    // make sure that Voice over WLAN Call maintenance is not active
    aCtxImpl.StopVoiceOverWlanCallMaintenance();
    
    // stop keeping the WLAN connection alive
    aCtxImpl.StopKeepAlive();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanMacActionState::OnDot11IbssModeStateSpaceEntry( 
    WlanContextImpl& aCtxImpl ) const
    {
    OsTracePrint( KDot11StateTransit, 
        (TUint8*)("UMAC * dot11-ibssmode state space entry") );

    // set the privacy mode filter
    aCtxImpl.SetActivePrivacyModeFilter( 
        aCtxImpl, aCtxImpl.EncryptionStatus() );

    // set frame Tx offsets
    SetMgmtSideTxOffsets( aCtxImpl );
    SetProtocolStackTxOffset( aCtxImpl );

    // complete the user connect request
    OnOidComplete( aCtxImpl );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanMacActionState::OnDot11IbssModeStateSpaceExit( 
    WlanContextImpl& aCtxImpl ) const
    {
    OsTracePrint( KDot11StateTransit, 
        (TUint8*)("UMAC * dot11-ibssmode state space exit") );

    // as we are departing dot11ibssmode state space
    // flush packet scheduler from all pending packets
    aCtxImpl.FlushPacketScheduler();
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanMacActionState::SetMgmtSideTxOffsets( 
    WlanContextImpl& aCtxImpl ) const
    {
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanMacActionState::SetMgmtSideTxOffsets") );

    TUint32 ethernetFrameTxOffset ( 0 );
    TUint32 dot11FrameTxOffset ( 0 );
    TUint32 snapFrameTxOffset ( 0 );
    
    DetermineTxOffsets( 
        aCtxImpl,
        ethernetFrameTxOffset,
        dot11FrameTxOffset,
        snapFrameTxOffset );

    aCtxImpl.iUmac.SetMgmtSideTxOffsets( 
        ethernetFrameTxOffset,
        dot11FrameTxOffset,
        snapFrameTxOffset );
    }
