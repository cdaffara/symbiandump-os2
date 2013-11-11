/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of the WlanDot11DeauthPending class
*
*/

/*
* %version: 24 %
*/

#include "config.h"
#include "UmacDot11DeauthPending.h"
#include "UmacContextImpl.h"

#ifndef NDEBUG
const TInt8 WlanDot11DeauthPending::iName[] = "dot11-deauthpending";

const TUint8 WlanDot11DeauthPending::iStateName
    [ESTATEMAX][KMaxStateStringLength] = 
    {
        {"EINIT"}, 
        {"ETXDEAUTHENTICATIONFRAME"}, 
        {"EWAIT4PUSHPACKET"},
        {"ECONTINUEDOT11TRAVERSE"}
    };

const TUint8 WlanDot11DeauthPending::iEventName
    [EEVENTMAX][KMaxEventStringLength] = 
    {
        {"ESTATEENTRY"}, 
        {"ETXCOMPLETE"},
        {"ETX_SCHEDULER_FULL"},
        {"EPUSHPACKET"}
    };
#endif

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11DeauthPending::Fsm( 
    WlanContextImpl& aCtxImpl, 
    TEvent aEvent )
    {
#ifndef NDEBUG
    OsTracePrint( 
        KUmacDetails, 
        (TUint8*)("UMAC: WlanDot11DeauthPending::Fsm(): event: "));
    OsTracePrint( KUmacDetails, iEventName[aEvent] );
    OsTracePrint( 
        KUmacDetails, 
        (TUint8*)("UMAC: WlanDot11DeauthPending::Fsm(): state: ")); 
    OsTracePrint( KUmacDetails, iStateName[iState] );
#endif

    switch ( aEvent )
        {
        case ESTATEENTRY:
            OnStateEntryEvent( aCtxImpl );
            break;
        case ETXCOMPLETE:
            OnTxCompleteEvent( aCtxImpl );
            break;
         case ETX_SCHEDULER_FULL:
            OnTxSchedulerFullEvent( aCtxImpl );
            break;
        case EPUSHPACKET:
            OnPushPacketEvent( aCtxImpl );
            break;
       default:
            OsAssert( 
                (TUint8*)("UMAC: WlanDot11DeauthPending::Fsm: panic"), 
                (TUint8*)(WLAN_FILE), __LINE__ );
        }
    }

// -----------------------------------------------------------------------------
// Handler for state entry event.
// -----------------------------------------------------------------------------
//
void WlanDot11DeauthPending::OnStateEntryEvent( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( 
        KUmacDetails, 
        (TUint8*)("UMAC: WlanDot11DeauthPending::OnStateEntryEvent()"));

    switch ( iState )
        {
        case EINIT:
            ChangeInternalState( aCtxImpl, ETXDEAUTHENTICATIONFRAME );
            break;
        case ETXDEAUTHENTICATIONFRAME:
            if ( !TxDeauthenticate( 
                    aCtxImpl, 
                    E802Dot11ReasonDeauthStationLeft,
                    ETrue ) )
                {
                // tx of dot11 deauthentication frame failed  
                // because packet scheduler was full
                // or because we didn't get a Tx buffer
                // so we enter to a wait state
                Fsm( aCtxImpl, ETX_SCHEDULER_FULL );
                }
            break;
        case EWAIT4PUSHPACKET:
            // nothing to do here than wait 
            break;
        case ECONTINUEDOT11TRAVERSE:
            ContinueDot11StateTraversal( aCtxImpl );                    
            break;
        default:
            // programming error
            OsTracePrint( KErrorLevel, (TUint8*)("UMAC: state: %d"), iState);        
            OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
            break;
        }
    }

// -----------------------------------------------------------------------------
// Handler for tx complete event.
// -----------------------------------------------------------------------------
//
void WlanDot11DeauthPending::OnTxCompleteEvent( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( 
        KUmacDetails, 
        (TUint8*)("UMAC: WlanDot11DeauthPending::OnTxCompleteEvent()"));

    if ( iState == ETXDEAUTHENTICATIONFRAME )
        {
        // deauthentication frame has been sent
        // continue state traversal
        ChangeInternalState( aCtxImpl, ECONTINUEDOT11TRAVERSE );                
        }
    else
        {
        // programming error
        OsTracePrint( KErrorLevel, (TUint8*)("UMAC: state: %d"), iState);        
        OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
        }
    }

// -----------------------------------------------------------------------------
// Handler for scheduler full event upon trying to tx dot11 deauthenticate frame
// -----------------------------------------------------------------------------
//
void WlanDot11DeauthPending::OnTxSchedulerFullEvent( 
    WlanContextImpl& aCtxImpl )
    {
    // change state
    OsTracePrint( KWarningLevel | KUmacAuth, (TUint8*)
        ("UMAC: WlanDot11DeauthPending::OnTxSchedulerFullEvent:packet scheduler full during deauthentication process") );

    ChangeInternalState( aCtxImpl, EWAIT4PUSHPACKET );
    }

// -----------------------------------------------------------------------------
// Handler for push packet to packet scheduler possible event
// -----------------------------------------------------------------------------
//
void WlanDot11DeauthPending::OnPushPacketEvent( 
    WlanContextImpl& aCtxImpl )
    {
    if ( iState == EWAIT4PUSHPACKET )
        {
        ChangeInternalState( aCtxImpl, ETXDEAUTHENTICATIONFRAME );
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11DeauthPending::ContinueDot11StateTraversal( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( 
        KUmacDetails, 
        (TUint8*)
        ("UMAC: WlanDot11DeauthPending::ContinueDot11StateTraversal()"));
        
    // we will go to soft reset state in any case
    ChangeState( aCtxImpl, 
        *this,                              // prev state
        aCtxImpl.iStates.iSoftResetState    // next state
        );
    }
    
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11DeauthPending::ChangeInternalState( 
    WlanContextImpl& aCtxImpl, 
    TState aNewState )
    {
#ifndef NDEBUG
    OsTracePrint( 
        KUmacDetails, 
        (TUint8*)
        ("UMAC: WlanDot11DeauthPending::ChangeInternalState(): old state:"));
    OsTracePrint( KUmacDetails, iStateName[iState] );
    OsTracePrint( 
        KUmacDetails, 
        (TUint8*)
        ("UMAC: WlanDot11DeauthPending::ChangeInternalState(): new state:"));
    OsTracePrint( KUmacDetails, iStateName[aNewState] );
#endif

    iState = aNewState;
    Fsm( aCtxImpl, ESTATEENTRY );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
#ifndef NDEBUG 
const TInt8* WlanDot11DeauthPending::GetStateName( TUint8& aLength ) const
    {
    aLength = sizeof( iName );
    return iName;
    }
#endif

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11DeauthPending::Entry( WlanContextImpl& aCtxImpl)
    {
    OsTracePrint( KUmacProtocolState, 
        (TUint8*)("UMAC: WlanDot11DeauthPending::Entry()") );

    if ( aCtxImpl.WsaCmdActive() )
        {
        // sanity checking code
        OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
        }

    // no need to do event dispatching as this
    // thing is triggered by the user and
    // is executed synchronously as we only do OID completion
    // at the end of disconnect process
    // Additionally we don't execute any asynchronous WHA commands 
    // from here so deauthentication protocol does not include multiple 
    // this object entry method executions

    if ( iState == EINIT )
        {
        // this is the start of the the FSM actions
        Fsm( aCtxImpl, ESTATEENTRY );
        }
    else
        {
        // implementation error see comment block above why
#ifndef NDEBUG
        OsTracePrint( KErrorLevel, (TUint8*)("UMAC: state:"));
        OsTracePrint( KErrorLevel, iStateName[iState] );
#endif
        OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11DeauthPending::Exit( WlanContextImpl& /*aCtxImpl*/)
    {
    OsTracePrint( 
        KUmacProtocolState, 
        (TUint8*)("UMAC: WlanDot11DeauthPending::Exit()"));

    // reset our local FSM for the next time...
    iState = EINIT;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11DeauthPending::OnPacketSendComplete(
    WlanContextImpl& aCtxImpl, 
    WHA::TStatus aStatus,
    TUint32 aPacketId,
    WHA::TRate aRate,
    TUint32 /*aPacketQueueDelay*/,
    TUint32 /*aMediaDelay*/,
    TUint /*aTotalTxDelay*/,
    TUint8 /*aAckFailures*/,
    WHA::TQueueId aQueueId,
    WHA::TRate aRequestedRate,
    TBool /*aMulticastData*/ )
    {
    OsTracePrint( 
        KUmacDetails, 
        (TUint8*)("UMAC: WlanDot11DeauthPending::OnPacketSendComplete"));

    aCtxImpl.OnTxCompleted( aRate, 
        static_cast<TBool>(aStatus == WHA::KSuccess), 
        aQueueId,
        aRequestedRate );

    if ( aPacketId == E802Dot11FrameTypeDeauthentication )
        {
        // deauthentication tx message has been sent from the WLAN device
        // No matter whether the sending was successful or not we will continue
        // with our disconnect actions
        Fsm( aCtxImpl, ETXCOMPLETE );
        }
    }
    
// -----------------------------------------------------------------------------
// We don't want to accept frames of any kind in this state
// -----------------------------------------------------------------------------
//
TAny* WlanDot11DeauthPending::RequestForBuffer( 
    WlanContextImpl& /*aCtxImpl*/,
    TUint16 /*aLength*/ )
    {
    return NULL;
    }

// -----------------------------------------------------------------------------
// packet sceduler notification that a packet push is guaranteed to succeed 
// -----------------------------------------------------------------------------
//
void WlanDot11DeauthPending::OnPacketPushPossible( 
    WlanContextImpl& aCtxImpl )
    {
    // feed a critter to the fsm
    Fsm( aCtxImpl, EPUSHPACKET );
    }
