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
* Description:   Implementation of the UmacDot11DisassociationPending class
*
*/

/*
* %version: 22 %
*/

#include "config.h"
#include "UmacDot11DisassociationPending.h"
#include "UmacContextImpl.h"

#ifndef NDEBUG
const TInt8 WlanDot11DisassociationPending::iName[] = "dot11-disassociationpending";

const TUint8 WlanDot11DisassociationPending::iStateName
    [ESTATEMAX][KMaxStateStringLength] = 
    {
        {"EINIT"}, 
        {"ETXDISASSOCIATIONFRAME"}, 
        {"EWAIT4PUSHPACKET"},
        {"ECONTINUEDOT11TRAVERSE"}
    };

const TUint8 WlanDot11DisassociationPending::iEventName
    [EEVENTMAX][KMaxEventStringLength] = 
    {
        {"ESTATEENTRY"}, 
        {"ETXPACKETCOMPLETE"},
        {"ETX_SCHEDULER_FULL"},
        {"EPUSHPACKET"}
    };
#endif 

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
WlanDot11DisassociationPending::WlanDot11DisassociationPending() 
    : iState( EINIT )
    {
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
WlanDot11DisassociationPending::~WlanDot11DisassociationPending()
    {
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11DisassociationPending::Entry( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( 
        KUmacProtocolState | KUmacAssoc, 
        (TUint8*)("UMAC: WlanDot11DisassociationPending::Entry()"));

    if ( aCtxImpl.WsaCmdActive() )
        {
        // sanity checking code
        OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
        }

    // no need to do event dispatching as this
    // thing is triggered by the user and
    // is executed synchronously as we only do OID completion
    // at the end of reassociation process
    // Additionally we don't execute any asynchronous WHA commands 
    // from here so disassociation protocol does not include multiple 
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
// completion method called when packet has been sent from the WLAN device
// -----------------------------------------------------------------------------
//
void WlanDot11DisassociationPending::OnPacketSendComplete(
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
        KUmacAssoc, 
        (TUint8*)("UMAC: WlanDot11DisassociationPending::OnPacketSendComplete"));

    aCtxImpl.OnTxCompleted( aRate, 
        static_cast<TBool>(aStatus == WHA::KSuccess), 
        aQueueId,
        aRequestedRate );

    if ( aPacketId == E802Dot11FrameTypeDisassociation )
        {
        // disassociation tx message has been sent from the WLAN device
        // No matter whether the sending was successful or not we will proceed
        // with our roaming actions

        Fsm( aCtxImpl, ETXPACKETCOMPLETE );
        }
    }
    
#ifndef NDEBUG 
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
const TInt8* WlanDot11DisassociationPending::GetStateName( TUint8& aLength ) const
    {
    aLength = sizeof( iName );
    return iName;
    }
#endif

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11DisassociationPending::Exit( WlanContextImpl& /*aCtxImpl*/ )
    {
    OsTracePrint( 
        KUmacProtocolState, 
        (TUint8*)("UMAC: WlanDot11DisassociationPending::Exit()"));

    // we are departing this dot11state to another dot11state (dot11synchronize)
    // we simple reset our local FSM for the next time...
    iState = EINIT;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11DisassociationPending::Fsm( 
    WlanContextImpl& aCtxImpl, 
    TEvent aEvent )
    {
#ifndef NDEBUG
    OsTracePrint( 
        KUmacAssoc, 
        (TUint8*)("UMAC: WlanDot11DisassociationPending::Fsm(): event: "));
    OsTracePrint( KUmacAssoc, iEventName[aEvent] );
    OsTracePrint( 
        KUmacAssoc, 
        (TUint8*)("UMAC: WlanDot11DisassociationPending::Fsm(): state: ")); 
    OsTracePrint( KUmacAssoc, iStateName[iState] );
#endif

    switch ( aEvent )
        {
        case ESTATEENTRY:
            OnStateEntryEvent( aCtxImpl );
            break;
        case ETXPACKETCOMPLETE:
            OnTxCompleteEvent( aCtxImpl );
            break;
        case ETX_SCHEDULER_FULL:
            OnTxSchedulerFullEvent( aCtxImpl );
            break;
        case EPUSHPACKET:
            OnPushPacketEvent( aCtxImpl );
            break;
        default:
            OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
        }
    }

// -----------------------------------------------------------------------------
// Handler for state entry event.
// -----------------------------------------------------------------------------
//
void WlanDot11DisassociationPending::OnStateEntryEvent( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( 
        KUmacAssoc, 
        (TUint8*)("UMAC: WlanDot11DisassociationPending::OnStateEntryEvent()"));

    switch ( iState )
        {
        case EINIT:
            // continue with the state traversal
            ChangeInternalState( aCtxImpl, ETXDISASSOCIATIONFRAME );
            break;
        case ETXDISASSOCIATIONFRAME:
            if ( !TxDisassociate( 
                    aCtxImpl, 
                    E802Dot11ReasonDisAssocStationLeft,
                    ETrue ) )
                {
                // tx of dot11 disassociate frame failed  
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
            OsTracePrint( KErrorLevel, 
                (TUint8*)("UMAC: state: %d"), iState);        
            OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
            break;
        }
    }

// -----------------------------------------------------------------------------
// Handler for tx complete event.
// -----------------------------------------------------------------------------
//
void WlanDot11DisassociationPending::OnTxCompleteEvent( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( 
        KUmacAssoc, 
        (TUint8*)("UMAC: WlanDot11DisassociationPending::OnTxCompleteEvent()"));

    if ( iState == ETXDISASSOCIATIONFRAME )
        {

        // disassociate frame has been sent
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
// Handler for scheduler full event upon trying to tx dot11 disassociate frame
// -----------------------------------------------------------------------------
//
void WlanDot11DisassociationPending::OnTxSchedulerFullEvent( 
    WlanContextImpl& aCtxImpl )
    {
    // change state
    OsTracePrint( KWarningLevel | KUmacAssoc, (TUint8*)
        ("UMAC: WlanDot11DisassociationPending::OnTxSchedulerFullEvent:packet scheduler full during disassociation process") );

    ChangeInternalState( aCtxImpl, EWAIT4PUSHPACKET );
    }

// -----------------------------------------------------------------------------
// Handler for push packet to packet scheduler possible event
// -----------------------------------------------------------------------------
//
void WlanDot11DisassociationPending::OnPushPacketEvent( 
    WlanContextImpl& aCtxImpl )
    {
    if ( iState == EWAIT4PUSHPACKET )
        {
        ChangeInternalState( aCtxImpl, ETXDISASSOCIATIONFRAME );
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11DisassociationPending::ContinueDot11StateTraversal( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( 
        KUmacAssoc, 
        (TUint8*)
        ("UMAC: WlanDot11DisassociationPending::ContinueDot11StateTraversal()"));
        
    // proceed to synchronize state...
    ChangeState( aCtxImpl, 
        *this,                              // prev state
        aCtxImpl.iStates.iSynchronizeState  // next state
        );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11DisassociationPending::ChangeInternalState( 
    WlanContextImpl& aCtxImpl, 
    TState aNewState )
    {
#ifndef NDEBUG
    OsTracePrint( 
        KUmacDetails, 
        (TUint8*)
        ("UMAC: WlanDot11DisassociationPending::ChangeInternalState(): old state:"));
    OsTracePrint( KUmacAssoc, iStateName[iState] );
    OsTracePrint( 
        KUmacDetails, 
        (TUint8*)
        ("UMAC: WlanDot11DisassociationPending::ChangeInternalState(): new state:"));
    OsTracePrint( KUmacAssoc, iStateName[aNewState] );
#endif

    iState = aNewState;
    Fsm( aCtxImpl, ESTATEENTRY );
    }

// -----------------------------------------------------------------------------
// packet sceduler notification that a packet push is guaranteed to succeed 
// -----------------------------------------------------------------------------
//
void WlanDot11DisassociationPending::OnPacketPushPossible( 
    WlanContextImpl& aCtxImpl )
    {
    // feed a critter to the fsm
    Fsm( aCtxImpl, EPUSHPACKET );
    }
