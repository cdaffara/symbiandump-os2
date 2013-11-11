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
* Description:   Implementation of the WlanWsaCommand class.
*
*/

/*
* %version: 25 %
*/

#include "config.h"
#include "UmacWsaCommand.h"
#include "UmacContextImpl.h"


// ============================ MEMBER FUNCTIONS ===============================


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
WlanWsaCommand::~WlanWsaCommand() 
    {
    iMacActionHistoryState = NULL;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanWsaCommand::Exit( WlanContextImpl& aCtxImpl )
    {
    // mark WSA cmd inactive
    aCtxImpl.DeActivateWsaCmd();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanWsaCommand::ChangeState( 
    WlanContextImpl& aCtxImpl, 
    WlanWsaCommand& aPrevState,
    WlanMacActionState& aNewState ) const
    {
#ifndef NDEBUG 
    TUint8 length;
    const TUint8* ptr = reinterpret_cast<const TUint8*>
        (aNewState.GetStateName( length ));
    const TUint8* ptr2 = reinterpret_cast<const TUint8*>
        (aCtxImpl.iCurrentMacState->GetStateName( length ));

    OsTracePrint( KUmacProtocolState, 
        (TUint8*)("UMAC: WlanWsaCommand::ChangeState(): previous state:") );
    OsTracePrint( KUmacProtocolState, ptr2 );
    OsTracePrint( KUmacProtocolState, 
        (TUint8*)("UMAC: WlanWsaCommand::ChangeState(): next state:") );
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
void WlanWsaCommand::TraverseToHistoryState( 
    WlanContextImpl& aCtxImpl, 
    WHA::TCommandId aCommandId, 
    WHA::TStatus aStatus,
    const WHA::UCommandResponseParams& aCommandResponseParams )
    {
    OsTracePrint( 
        KUmacDetails, 
        (TUint8*)
        ("UMAC: WlanWsaCommand::TraverseToHistoryState(): act: %d"), 
        aCtxImpl.WhaCommandAct() );

    History().OnWhaCommandResponse( 
        aCtxImpl,
        aCommandId,
        aStatus,
        aCommandResponseParams,
        aCtxImpl.WhaCommandAct() );
    
    ChangeState( aCtxImpl, 
        *this,                  // previous state
        History()          // recall history state (new state)
        );                    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanWsaCommand::Act( WlanContextImpl& aCtxImpl, TUint32 aAct ) const
    {
    aCtxImpl.WhaCommandAct( aAct );
    }

// from wha side

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanWsaCommand::Indication( 
    WlanContextImpl& aCtxImpl, 
    WHA::TIndicationId aIndicationId,
    const WHA::UIndicationParams& aIndicationParams )
    {
    return History().Indication( aCtxImpl, aIndicationId, aIndicationParams );
    }

// Frame receive

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TAny* WlanWsaCommand::RequestForBuffer( 
    WlanContextImpl& aCtxImpl,             
    TUint16 aLength )
    {
    return History().RequestForBuffer( aCtxImpl, aLength );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanWsaCommand::ReceivePacket( 
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
    return History().ReceivePacket( 
        aCtxImpl, 
        aStatus,        
        aFrame,
        aLength,
        aRate,
        aRcpi,
        aChannel,
        aBuffer,
        aFlags
        );
    }
        
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanWsaCommand::TxData( 
    WlanContextImpl& aCtxImpl,
    TDataBuffer& aDataBuffer,
    TBool aMore )
    {
    return History().TxData( aCtxImpl, aDataBuffer, aMore );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanWsaCommand::TxMgmtData( 
    WlanContextImpl& aCtxImpl,
    TDataBuffer& aDataBuffer )
    {
    History().TxMgmtData( aCtxImpl, aDataBuffer );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanWsaCommand::OnPacketTransferComplete( 
    WlanContextImpl& aCtxImpl, 
    TUint32 aPacketId,
    TDataBuffer* aMetaHeader )
    {
    History().OnPacketTransferComplete( aCtxImpl, aPacketId, aMetaHeader );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanWsaCommand::OnPacketSendComplete(
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
    History().OnPacketSendComplete( 
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
void WlanWsaCommand::CallPacketSchedule( 
    WlanContextImpl& aCtxImpl,
    TBool aMore )
    {
    History().CallPacketSchedule( aCtxImpl, aMore );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanWsaCommand::OnPacketPushPossible( 
    WlanContextImpl& aCtxImpl )
    {
    History().OnPacketPushPossible( aCtxImpl );
    }
