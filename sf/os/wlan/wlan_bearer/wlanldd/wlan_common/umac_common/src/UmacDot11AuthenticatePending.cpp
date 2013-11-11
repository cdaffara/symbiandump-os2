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
* Description:   Implementation of the WlanDot11AuthenticatePending class
*
*/

/*
* %version: 38 %
*/

#include "config.h"
#include "UmacDot11AuthenticatePending.h"
#include "UmacContextImpl.h"

#ifndef NDEBUG
const TUint8 WlanDot11AuthenticatePending::iStateName
    [ESTATEMAX][KMaxStateStringLength] = 
    {
        {"EINIT"}, 
        {"ETXAUTHFRAME"}, 
        {"EWAIT4AUTHRESPONSE"},
        {"EWAIT4PUSHPACKET"},
        {"ECONTINUEDOT11TRAVERSE"}
    };

const TUint8 WlanDot11AuthenticatePending::iEventName
    [EEVENTMAX][KMaxEventStringLength] = 
    {
        {"ESTATEENTRY"}, {"ECONTINUE"}, {"ERXAUTHRESPONSE"}, 
        {"ETX_AUTHFRAME_XFER"}, {"ETX_SCHEDULER_FULL"}, 
        {"ETIMEOUT"}, {"EPUSHPACKET"}
    };
#endif 
// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11AuthenticatePending::Entry( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( KUmacProtocolState | KUmacAuth, 
        (TUint8*)("UMAC * execute dot11 authenticate"));

    if ( aCtxImpl.WsaCmdActive() )
        {
        // sanity checking code
        OsAssert( (TUint8*)("UMAC * panic"), (TUint8*)(WLAN_FILE), __LINE__ );
        }

    // no need to do event dispatching as this
    // thing is triggered by the user and
    // is executed synchronously as we only do OID completion
    // at the end of authenticate + association process
    // for the same reason we don't execute any asynchronous WHA commands 
    // from here so authetication protocol does not include multiple 
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
        OsTracePrint( KErrorLevel, (TUint8*)("state:"));
        OsTracePrint( KErrorLevel, iStateName[iState] );
#endif
        OsAssert( (TUint8*)("UMAC * panic"), (TUint8*)(WLAN_FILE), __LINE__ );
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11AuthenticatePending::Exit( 
    WlanContextImpl& /*aCtxImpl*/ )
    {
    // we are departing this dot11state to another dot11state,
    // which means we are either:
    // 1) proceeding to WlanDot11AssociationPending state
    // in case of authentication success
    // 2) proceeding to WlanDot11Idle state
    // in case of authentication failure
    // we simple reset our local FSM for the next time...
    iState = EINIT;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11AuthenticatePending::StateEntryActions( 
    WlanContextImpl& aCtxImpl )
    {
    iFlags = 0;

    if ( aCtxImpl.HtSupportedByNw() )
        {
        aCtxImpl.GetHtAuthenticationFrame().ResetSeqNmbr();
        aCtxImpl.GetHtAuthenticationFrame().ClearWepBit();        
        }
    else
        {
        aCtxImpl.GetAuthenticationFrame().ResetSeqNmbr();
        aCtxImpl.GetAuthenticationFrame().ClearWepBit();
        }

    aCtxImpl.ResetAuthSeqNmbrExpected();

    // its up to our sub states to fill this block
    OnSetAlgorithmNumber( aCtxImpl );
    }

// -----------------------------------------------------------------------------
// Start authentication frame response timer
// -----------------------------------------------------------------------------
//
void WlanDot11AuthenticatePending::StartAuthenticationFrameResponseTimer( 
    WlanContextImpl& aCtxImpl )
    {
    // start authentication frame response timeout timer 
    const TUint32 timeout( dot11AuthenticationResponseTimeOut * KTU );
    
    OsTracePrint( KUmacAuth, (
        TUint8*)("UMAC: WlanDot11AuthenticatePending::StartAuthenticationFrameResponseTimer:timeout in microseconds: %d"), 
        timeout );

    aCtxImpl.iUmac.RegisterTimeout( timeout );
    }

// -----------------------------------------------------------------------------
// Send authenticate seq.number 1 message
// -----------------------------------------------------------------------------
//
TBool WlanDot11AuthenticatePending::SendAuthSeqNbr1Frame( 
    WlanContextImpl& aCtxImpl ) const
    {
    OsTracePrint( KUmacAuth, (TUint8*)
        ("UMAC: WlanDot11AuthenticatePending::SendAuthSeqNbr1Frame") );

    TBool status ( EFalse );

    // client doesn't have to take care of the tx buffer header space
    // as the method below does that by itself
    TUint8* start_of_frame = aCtxImpl.TxBuffer( ETrue );

    if ( start_of_frame )
        {
        TUint32 frameLength ( 0 );

        if ( aCtxImpl.HtSupportedByNw() && aCtxImpl.QosEnabled() )
            {
            OsTracePrint( KUmacAuth, (TUint8*)
                ("UMAC: Algorithm number: %d"), 
                aCtxImpl.GetHtAuthenticationFrame().GetAlgorithmNumber());
            OsTracePrint( KUmacAuth, (TUint8*)
                ("UMAC: Sequence number: %d"), 
                aCtxImpl.GetHtAuthenticationFrame().GetSeqNmbr());
            OsTracePrint( KUmacAuth, (TUint8*)
                ("UMAC: Status code: %d"), 
                aCtxImpl.GetHtAuthenticationFrame().GetStatusCode());
        
            frameLength = sizeof( SHtAuthenticationFrame );
            
            os_memcpy( 
                start_of_frame,
                &(aCtxImpl.GetHtAuthenticationFrame()), 
                frameLength );
            }
        else
            {
            OsTracePrint( KUmacAuth, (TUint8*)
                ("UMAC: Algorithm number: %d"), 
                aCtxImpl.GetAuthenticationFrame().GetAlgorithmNumber());
            OsTracePrint( KUmacAuth, (TUint8*)
                ("UMAC: Sequence number: %d"), 
                aCtxImpl.GetAuthenticationFrame().GetSeqNmbr());
            OsTracePrint( KUmacAuth, (TUint8*)
                ("UMAC: Status code: %d"), 
                aCtxImpl.GetAuthenticationFrame().GetStatusCode());
        
            frameLength = sizeof( SAuthenticationFrame );
            
            os_memcpy( 
                start_of_frame,
                &(aCtxImpl.GetAuthenticationFrame()), 
                frameLength );
            }
    
        // trace the frame critter
        OsTracePrint( KUmacAuth, (TUint8*)("UMAC: dot11 authenticate frame tx:"),
            *(reinterpret_cast<const Sdot11MacHeader*>(start_of_frame)) );
    
        const WHA::TQueueId queue_id 
            = QueueId( aCtxImpl, start_of_frame );
    
        // push the frame to packet scheduler for transmission
        status = aCtxImpl.PushPacketToPacketScheduler(
                    start_of_frame,
                    frameLength,
                    queue_id,
                    E802Dot11FrameTypeAuthSeqNmbr1,
                    NULL,
                    EFalse,
                    EFalse,
                    ETrue );
    
        if ( !status )
            {
            // as we came here we did get an internal Tx buffer for the frame
            // but packet push to scheduler failed. In this case we need cancel
            // the internal Tx buffer reservation as we will request it again
            // when the Packet Scheduler is again ready for packet push
            aCtxImpl.MarkInternalTxBufFree();
            }
        }
    else
        {
        // we didn't get a Tx buffer. EFalse will be returned
        // to indicate that
        OsTracePrint( KUmacAuth, (TUint8*)
            ("UMAC: WlanDot11AuthenticatePending::SendAuthSeqNbr1Frame: no internal Tx buffer available") );
        }
        
    return status;
    }

// -----------------------------------------------------------------------------
// completion method called when packet has been xferred to the WLAN device
// -----------------------------------------------------------------------------
//
void WlanDot11AuthenticatePending::OnPacketTransferComplete( 
    WlanContextImpl& aCtxImpl, 
    TUint32 aPacketId,
    TDataBuffer* aMetaHeader )
    {
    OsTracePrint( KUmacAuth, (TUint8*)
        ("UMAC: WlanDot11AuthenticatePending::OnPacketTransferComplete"));

    if ( aPacketId == E802Dot11FrameTypeData )
        {
        OnTxProtocolStackDataComplete( aCtxImpl, aMetaHeader );
        }
    else if ( aPacketId == E802Dot11FrameTypeDataEapol || 
              aPacketId == E802Dot11FrameTypeManagementAction ||
              aPacketId == E802Dot11FrameTypeTestFrame )
        {
        OnMgmtPathWriteComplete( aCtxImpl );
        }
    else
        {
        // this frame Tx request didn't come from above us (i.e. neither 
        // through the user data nor the management data API) but is
        // related to a frame Tx we have done internally. So we need to mark
        // the internal Tx buffer free again
        aCtxImpl.MarkInternalTxBufFree();
        }

    if ( aPacketId == E802Dot11FrameTypeAuthSeqNmbr1 
        || aPacketId == E802Dot11FrameTypeAuthSeqNmbr3 )
        {
        // authenticate tx message has been xferred to the WLAN device
        
        // so feed an event critter to the fsm
        Fsm( aCtxImpl, ETX_AUTHFRAME_XFER );
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11AuthenticatePending::ResolveAuthMessage( 
    WlanContextImpl& aCtxImpl,
    TUint16 aAuthModeDesired,
    const void* aFrame,
    TUint32 aFlags )
    {
    TBool ret( EFalse );
    const SAuthenticationFixedFields* auth_fields = 
        HtcFieldPresent( aCtxImpl, aFrame, aFlags ) ?
            reinterpret_cast<const SAuthenticationFixedFields*>
              (reinterpret_cast<const TUint8*>(aFrame) + 
               sizeof( SHtManagementFrameHeader )) :
            reinterpret_cast<const SAuthenticationFixedFields*>
              (reinterpret_cast<const TUint8*>(aFrame) + 
               sizeof( SManagementFrameHeader ));

    OsTracePrint( KUmacAuth, (TUint8*)
        ("UMAC: WlanDot11AuthenticatePending::ResolveAuthMessage: auth message sequence number expected: %d"), 
        aCtxImpl.GetAuthSeqNmbrExpected() );
    OsTracePrint( KUmacAuth, (TUint8*)
        ("UMAC: WlanDot11AuthenticatePending::ResolveAuthMessage: auth message sequence number: %d"), 
        auth_fields->SequenceNumber() );
    OsTracePrint( KUmacAuth, (TUint8*)
        ("UMAC: WlanDot11AuthenticatePending::ResolveAuthMessage: algorithm number expected: %d"), 
        aAuthModeDesired );
    OsTracePrint( KUmacAuth, (TUint8*)
        ("UMAC: WlanDot11AuthenticatePending::ResolveAuthMessage: algorithm number: %d"), 
        auth_fields->AlgorithmNumber() );
    OsTracePrint( KUmacAuth, (TUint8*)
        ("UMAC: WlanDot11AuthenticatePending::ResolveAuthMessage: status code: %d"), 
        auth_fields->StatusCode() );

    if ( // is desired mode authentication
        ( auth_fields->AlgorithmNumber() == aAuthModeDesired )
        // AND is authentication transaction sequence number expected
        && ( auth_fields->SequenceNumber() == aCtxImpl.GetAuthSeqNmbrExpected() )
        // AND the status code is successful
        && ( auth_fields->StatusCode() == E802Dot11StatusSuccess ))
        {
        ret = ETrue;
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11AuthenticatePending::ChangeInternalState( 
    WlanContextImpl& aCtxImpl, 
    TState aNewState )
    {
    iState = aNewState;
    Fsm( aCtxImpl, ESTATEENTRY );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11AuthenticatePending::Fsm( 
    WlanContextImpl& aCtxImpl, 
    TEvent aEvent )
    {
    OsTracePrint( KUmacAuth, 
        (TUint8*)("UMAC * dot11-authenticatepending * FSM EVENT") );
#ifndef NDEBUG
    OsTracePrint( KUmacAuth, (TUint8*)("event:"));
    OsTracePrint( KUmacAuth, iEventName[aEvent] );
    OsTracePrint( KUmacAuth, (TUint8*)("state:"));
    OsTracePrint( KUmacAuth, iStateName[iState] );
#endif

    switch ( aEvent )
        {
        case ESTATEENTRY:
            OnStateEntryEvent( aCtxImpl );
            break;
        case ECONTINUE:
            OnContinueEvent( aCtxImpl );
            break;
        case ERXAUTHRESPONSE:
            OnRxAuthResponseEvent( aCtxImpl );
            break;
        case ETX_AUTHFRAME_XFER:
            OnTxAuthFrameXferEvent( aCtxImpl );
            break;
        case ETX_SCHEDULER_FULL:
            OnTxSchedulerFullEvent( aCtxImpl );
            break;
        case ETIMEOUT:
            OnTimeoutEvent( aCtxImpl );
            break;
        case EPUSHPACKET:
            OnPushPacketEvent( aCtxImpl );
            break;
        default:
            // cath internal FSM programming error
#ifndef NDEBUG
            OsTracePrint( KErrorLevel, (TUint8*)("event:"));
            OsTracePrint( KErrorLevel, iEventName[aEvent] );                
#endif
            OsAssert( (TUint8*)("* UMAC * panic"), 
                (TUint8*)(WLAN_FILE), __LINE__ );
            break;
        }
    }

// -----------------------------------------------------------------------------
// Handler for generic continue event.
// -----------------------------------------------------------------------------
//
void WlanDot11AuthenticatePending::OnContinueEvent( 
    WlanContextImpl& aCtxImpl )
    {
    switch ( iState )
        {
        case EINIT:
            ChangeInternalState( aCtxImpl, ETXAUTHFRAME );
            break;
        default:
            // catch internal FSM programming error
#ifndef NDEBUG
            OsTracePrint( KErrorLevel, (TUint8*)("state:"));
            OsTracePrint( KErrorLevel, iStateName[iState] );
#endif
            OsAssert( (TUint8*)("* UMAC * panic"), 
                (TUint8*)(WLAN_FILE), __LINE__ );
            break;
        }
    }

// -----------------------------------------------------------------------------
// Handler for authentication response timeout event.
// -----------------------------------------------------------------------------
//
void WlanDot11AuthenticatePending::OnTimeoutEvent( 
    WlanContextImpl& aCtxImpl )
    {
    // set completion code
    // as dot11idle state does the OID completion
    aCtxImpl.iStates.iIdleState.Set( KErrTimedOut );
    // authentication was not successful
    iFlags &= ~KAuthSuccess;
    ChangeInternalState( aCtxImpl, ECONTINUEDOT11TRAVERSE );
    }

// -----------------------------------------------------------------------------
// Handler for push packet to packet scheduler possible event
// -----------------------------------------------------------------------------
//
void WlanDot11AuthenticatePending::OnPushPacketEvent( 
    WlanContextImpl& aCtxImpl )
    {
    if ( iState == EWAIT4PUSHPACKET )
        {
        ChangeInternalState( aCtxImpl, ETXAUTHFRAME );
        }
    }

// -----------------------------------------------------------------------------
// Handler for authenticate request frame 
// has been xferred to the WLAN device event
// -----------------------------------------------------------------------------
//
void WlanDot11AuthenticatePending::OnTxAuthFrameXferEvent( 
    WlanContextImpl& aCtxImpl )
    {
    // change state
    ChangeInternalState( aCtxImpl, EWAIT4AUTHRESPONSE );
    }

// -----------------------------------------------------------------------------
// Handler for scheduler full event upon trying to tx dot11-authenticate frame
// -----------------------------------------------------------------------------
//
void WlanDot11AuthenticatePending::OnTxSchedulerFullEvent( 
    WlanContextImpl& aCtxImpl )
    {
    // change state
    OsTracePrint( KWarningLevel | KUmacAuth, (TUint8*)
        ("UMAC: packet scheduler full during authentication process") );

    ChangeInternalState( aCtxImpl, EWAIT4PUSHPACKET );
    }

// -----------------------------------------------------------------------------
// continue dot11 protocol statemachine traversal
// -----------------------------------------------------------------------------
//
void WlanDot11AuthenticatePending::ContinueDot11StateTraversal( 
    WlanContextImpl& aCtxImpl )
    {
    if ( iFlags & KAuthSuccess )
        {
        // authentication was a success
        // so we proceed to reassociation state - if this happens to be a 
        // roaming case - or to association state
        //
        if ( aCtxImpl.Reassociate() )
            {
            ChangeState( aCtxImpl, 
                *this,                                       // prev state
                aCtxImpl.iStates.iReassociationPendingState  // next state            
                );                                   
            }
        else
            {
            ChangeState( aCtxImpl, 
                *this,                                      // prev state
                aCtxImpl.iStates.iAssociationPendingState   // next state
                );                                   
            }
        }
    else
        {
        // authentication was a failure
        // either due AP denial or by timeout
        // the reason does not really intrest us here
        // as the procedure is the same...
        ChangeState( aCtxImpl, 
            *this,                          // prev state
            aCtxImpl.iStates.iIdleState     // next state
            );                                     
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11AuthenticatePending::OnTimeout( 
    WlanContextImpl& aCtxImpl )
    {
    TBool stateChange ( ETrue );
    
    switch ( iState )
        {
        case EWAIT4AUTHRESPONSE:
            // authentication response timeout
            
            OsTracePrint( KWarningLevel | KUmacAssoc, (TUint8*)
                ("UMAC: WlanDot11AuthenticatePending::OnTimeout: authentication resp. timeout => authenication failed!") );

            Fsm( aCtxImpl, ETIMEOUT );
            
            // in this case we return ETrue, i.e. signal the caller that a 
            // state change occurred
            
            break;
        default:
            // a timeout occurred when we weren't expecting it (yet). This 
            // means that a timeout callback had already been registered when
            // we tried to cancel this timer the previous time (regarding
            // the previous authentication frame). So this callback is not 
            // relevant and can be ignored. 
            
            OsTracePrint( KWarningLevel | KUmacAssoc, (TUint8*)
                ("UMAC: WlanDot11AuthenticatePending::OnTimeout: irrelevant timeout; ignored") );

            // Signal the caller that no state change occurred
            stateChange = EFalse;
        }
        
    return stateChange;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11AuthenticatePending::ReceivePacket( 
    WlanContextImpl& aCtxImpl,
    WHA::TStatus aStatus,
    const void* aFrame,
    TUint16 aLength,
    WHA::TRate /*aRate*/,
    WHA::TRcpi aRcpi,
    WHA::TChannelNumber /*aChannel*/,
    TUint8* aBuffer,
    TUint32 aFlags )
    {
    OsTracePrint( KUmacAuth, (TUint8*)
        ("UMAC: dot11-authenticatepending::ReceivePacket()") );

    if ( aStatus == WHA::KSuccess )
        {
        // receive success
        const Sdot11MacHeader* dot11_hdr( 
            static_cast<const Sdot11MacHeader*>(aFrame) );

        // we accept only frames with ToDS bit cleared
        if ( dot11_hdr->IsToDsBitSet() )
            {
            OsTracePrint( KWarningLevel | KUmacAuth, 
                (TUint8*)("UMAC: authenticating to BSS:") );
            OsTracePrint( KWarningLevel | KUmacAuth, 
                (TUint8*)("UMAC: rx-frame: ToDs bit set, discard frame") );

            // release the Rx buffer & abort
            aCtxImpl.iUmac.MarkRxBufFree( aBuffer );
            return;
            }

        const TBool class2_frame( IsClass2Frame( dot11_hdr->iFrameControl.iType ) );
        const TBool class3_frame( IsClass3Frame( dot11_hdr->iFrameControl.iType ) );
        const TBool unicast_addr( !IsGroupBitSet( dot11_hdr->iAddress1 ) );

        if ( class2_frame && unicast_addr )
            {
            OsTracePrint( KWarningLevel | KUmacAuth, 
                (TUint8*)("UMAC: authenticating to BSS:") );
            OsTracePrint( KWarningLevel | KUmacAuth, 
                (TUint8*)("rx class2 frame with unicast DA address") );

            // class 2 frame rx and unicast address in address1 field
            // That's not the frame we are expecting. 
            // Note that we release the Rx buffer at the end of this method

            if ( !Authenticated() )
                {
                // we do not have a valid authentication with the 
                // BSS where the frame came
                OsTracePrint( KWarningLevel | KUmacAuth, 
                    (TUint8*)("TxDeauthenticate") );
                if ( !TxDeauthenticate( 
                        aCtxImpl, 
                        E802Dot11ReasonClass2FrameWhenNotAuth ) )
                    {
                    // frame was not sent because either packet scheduler was 
                    // full or because we didn't get a Tx buffer. In any case
                    // we won't try to send it again. 
                    }
                }
            }
        else if ( class3_frame && unicast_addr )
            {
            OsTracePrint( KWarningLevel | KUmacAuth, 
                (TUint8*)("UMAC: authenticating to BSS:") );
            OsTracePrint( KWarningLevel | KUmacAuth, 
                (TUint8*)("rx class3 frame with unicast DA address") );

            // class 3 frame rx and unicast address in address1 field
            // That's not the frame we are expecting
            // Note that we release the Rx buffer at the end of this method

            if ( !Authenticated() )
                {
                // we do not have a valid authentication with the 
                // BSS where the frame came
                OsTracePrint( KWarningLevel | KUmacAuth, 
                    (TUint8*)("TxDeauthenticate") );
                if ( !TxDeauthenticate( 
                        aCtxImpl, 
                        E802Dot11ReasonClass3FrameWhenNotAssoc ) )
                    {
                    // frame was not sent because either packet scheduler was 
                    // full or because we didn't get a Tx buffer. In any case
                    // we won't try to send it again. 
                    }                        
                }
            else
                {
                // we do have a valid authentication with the 
                // STA that sent the frame
                OsTracePrint( KWarningLevel | KUmacAuth, 
                    (TUint8*)("UMAC: TxDisassociate") );
                
                // set the BSSID of the existing network; and the DA
                if ( aCtxImpl.HtSupportedByNw() )
                    {
                    (aCtxImpl.GetHtDisassociationFrame()).iHeader.iBSSID = 
                        aCtxImpl.GetBssId();
                    (aCtxImpl.GetHtDisassociationFrame()).iHeader.iDA = 
                        aCtxImpl.GetBssId();                    
                    }
                else
                    {
                    (aCtxImpl.GetDisassociationFrame()).iHeader.iBSSID = 
                        aCtxImpl.GetBssId();
                    (aCtxImpl.GetDisassociationFrame()).iHeader.iDA = 
                        aCtxImpl.GetBssId();
                    }
                if ( !TxDisassociate( 
                        aCtxImpl, 
                        E802Dot11ReasonClass3FrameWhenNotAssoc ) )
                    {
                    // frame was not sent because either packet scheduler was 
                    // full or because we didn't get a Tx buffer. In any case
                    // we won't try to send it again. 
                    }                    
                }
            }
        else
            {
            // default handler
            OnReceiveFrameSuccess( 
                aCtxImpl, 
                aFrame, 
                aLength, 
                aRcpi, 
                aFlags, 
                aBuffer );
            }
        }
    else    // aStatus == WHA::KSuccess 
        {
        // receive failed, so no action here
        }

    // release the Rx buffer
    aCtxImpl.iUmac.MarkRxBufFree( aBuffer );
    }

// -----------------------------------------------------------------------------
// packet sceduler notification that a packet push is guaranteed to succeed 
// -----------------------------------------------------------------------------
//
void WlanDot11AuthenticatePending::OnPacketPushPossible( 
    WlanContextImpl& aCtxImpl )
    {
    // feed a critter to the fsm
    Fsm( aCtxImpl, EPUSHPACKET );
    }
