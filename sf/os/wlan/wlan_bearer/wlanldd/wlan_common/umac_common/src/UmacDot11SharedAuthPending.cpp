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
* Description:   Implementation of the WlanDot11SharedAuthPending class
*
*/

/*
* %version: 27 %
*/

#include "config.h"
#include "UmacDot11SharedAuthPending.h"
#include "UmacContextImpl.h"

#ifndef NDEBUG 
const TInt8 WlanDot11SharedAuthPending::iName[] 
    = "dot11-sharedauthpending";
#endif // !NDEBUG 

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
#ifndef NDEBUG 
const TInt8* WlanDot11SharedAuthPending::GetStateName( 
    TUint8& aLength ) const
    {
    aLength = sizeof( iName );
    return iName;
    }
#endif

// -----------------------------------------------------------------------------
// set appropriate used algorithm number to authenticate request frame
// -----------------------------------------------------------------------------
//
void WlanDot11SharedAuthPending::OnSetAlgorithmNumber( 
    WlanContextImpl& aCtxImpl )
    {
    if ( aCtxImpl.HtSupportedByNw() )
        {
        aCtxImpl.GetHtAuthenticationFrame().SetAlgorithmNmbr( 
            K802Dot11AuthModeShared );
        }
    else
        {
        aCtxImpl.GetAuthenticationFrame().SetAlgorithmNmbr( 
            K802Dot11AuthModeShared );
        }
    }

// -----------------------------------------------------------------------------
// Handler for state entry event.
// -----------------------------------------------------------------------------
//
void WlanDot11SharedAuthPending::OnStateEntryEvent( 
    WlanContextImpl& aCtxImpl )
    {    
    TBool ret( ETrue );

    switch ( iState )
        {
        case EINIT:
            // do all the synchronous 
            // composite state entry actions
            StateEntryActions( aCtxImpl );
            // continue with the state traversal
            Fsm( aCtxImpl, ECONTINUE );
            break;
        case ETXAUTHFRAME:
            // send correct authenticate frame
            if ( aCtxImpl.GetAuthSeqNmbrExpected() 
                == E802Dot11AuthenticationSeqNmbr2 )
                {
                ret = SendAuthSeqNbr1Frame( aCtxImpl );
                }
            else
                {
                ret = SendAuthSeqNbr3Frame( aCtxImpl );
                }

            if (!ret )
                {
                // tx of dot11-authenticate frame failed  
                // because packet scheduler was full
                // or because we didn't get a Tx buffer                
                // so we enter to a wait state
                Fsm( aCtxImpl, ETX_SCHEDULER_FULL );
                }
            break;
        case ECONTINUEDOT11TRAVERSE:
            ContinueDot11StateTraversal( aCtxImpl );
            break;
        case EWAIT4AUTHRESPONSE:
            StartAuthenticationFrameResponseTimer( aCtxImpl );
            break;
        case EWAIT4PUSHPACKET:
            // nothing to do here than wait 
            break;
        default:
            // catch internal FSM programming error
#ifndef NDEBUG
            OsTracePrint( KErrorLevel, (TUint8*)("UMAC: state:"));
            OsTracePrint( KErrorLevel, iStateName[iState] );
#endif
            OsAssert( (TUint8*)("UMAC: panic"), 
                (TUint8*)(WLAN_FILE), __LINE__ );
            break;
        }
    }

// -----------------------------------------------------------------------------
// Handler for rx authentication response event.
// -----------------------------------------------------------------------------
//
void WlanDot11SharedAuthPending::OnRxAuthResponseEvent( 
    WlanContextImpl& aCtxImpl )
    {
    switch ( iState )
        {
        case EWAIT4AUTHRESPONSE:
            // check do we need to send an another authenticate frame or not
            if ( // current authentication frame exchange was a success 
                ((iFlags & KAuthSuccess)
                // AND authentication frame exchange is complete
                && ( aCtxImpl.GetAuthSeqNmbrExpected() 
                == E802Dot11AuthenticationSeqNmbr4 ))
                // OR current authentication frame exchange was a failure 
                || !(iFlags & KAuthSuccess)
                )
                {               
                ChangeInternalState( aCtxImpl, ECONTINUEDOT11TRAVERSE );
                }
            else
                {
                // current authentication frame exchange was a success 
                // but authentication frame exchange is NOT complete

                // incerement the seq.nmbr expected from AP counter 
                aCtxImpl.IncrementAuthSeqNmbrExpected();
                ChangeInternalState( aCtxImpl, ETXAUTHFRAME );
                }
            break;
        default:
            // this means that we have recieved a valid dot11 
            // authenticate response frame that we are waiting for
            // but we are not internally in such a state
            // only feasible situation for this is that 
            // someone  has skipped a call to the packet xfer method
            // that informs of authenticate request frame 
            // xfer to the WLAN device.
            // other case is that our fsm is totally messed up
            // so we catch this
#ifndef NDEBUG
            OsTracePrint( KErrorLevel, (TUint8*)("state:"));
            OsTracePrint( KErrorLevel, iStateName[iState] );
#endif
            OsAssert( (TUint8*)("UMAC: panic"), 
                (TUint8*)(WLAN_FILE), __LINE__ );
            break;
        }
    }

// -----------------------------------------------------------------------------
// Send authenticate seq. number 3 message
// -----------------------------------------------------------------------------
//
TBool WlanDot11SharedAuthPending::SendAuthSeqNbr3Frame( 
    WlanContextImpl& aCtxImpl ) const
    {
    OsTracePrint( KUmacAuth, (TUint8*)
        ("UMAC: WlanDot11SharedAuthPending::SendAuthSeqNbr3Frame") );

    TBool status ( EFalse );

    // client doesn't have to take care of the tx buffer header space
    // as the method below does that by itself
    TUint8* frame_ptr = aCtxImpl.TxBuffer( ETrue );

    if ( frame_ptr )
        {
        // store start of frame 
        const TUint8* start_of_frame = frame_ptr;
        TUint32 txFrameHdrAndFixedPartLen ( 0 ); 
        // construct auth message seq. number 3
        
        if ( aCtxImpl.HtSupportedByNw() && aCtxImpl.QosEnabled() )
            {
            // set our seq. nbr in next authenticate Tx-frame 
            aCtxImpl.GetHtAuthenticationFrame().IncrementSeqNmbr();
        
            // set the WEP bit, as that is the only thing that triggers 
            // the ecryption engine. Don't worry about clearing it here 
            // because we do it in WlanDot11AuthenticatePending::Entry()
            aCtxImpl.GetHtAuthenticationFrame().SetWepBit();
            
            OsTracePrint( KUmacAuth, (TUint8*)("UMAC: Algorithm number: %d"),
                aCtxImpl.GetHtAuthenticationFrame().GetAlgorithmNumber());
            OsTracePrint( KUmacAuth, (TUint8*)("UMAC: Sequence number: %d"),
                aCtxImpl.GetHtAuthenticationFrame().GetSeqNmbr());
            OsTracePrint( KUmacAuth, (TUint8*)("UMAC: Status code: %d"),
                aCtxImpl.GetHtAuthenticationFrame().GetStatusCode());
            
            // copy the dot11 authentication frame header to tx buffer 
            os_memcpy( frame_ptr,
                &(aCtxImpl.GetHtAuthenticationFrame().iHeader),
                sizeof( aCtxImpl.GetHtAuthenticationFrame().iHeader) );
        
            // adjust to end of copy
            frame_ptr 
                += sizeof( aCtxImpl.GetHtAuthenticationFrame().iHeader ); 
            
            txFrameHdrAndFixedPartLen = sizeof( SHtAuthenticationFrame );
            }
        else
            {
            // set our seq. nbr in next authenticate Tx-frame 
            aCtxImpl.GetAuthenticationFrame().IncrementSeqNmbr();
        
            // set the WEP bit, as that is the only thing that triggers 
            // the ecryption engine. Don't worry about clearing it here 
            // because we do it in WlanDot11AuthenticatePending::Entry()
            aCtxImpl.GetAuthenticationFrame().SetWepBit();
            
            OsTracePrint( KUmacAuth, (TUint8*)("UMAC: Algorithm number: %d"),
                aCtxImpl.GetAuthenticationFrame().GetAlgorithmNumber());
            OsTracePrint( KUmacAuth, (TUint8*)("UMAC: Sequence number: %d"),
                aCtxImpl.GetAuthenticationFrame().GetSeqNmbr());
            OsTracePrint( KUmacAuth, (TUint8*)("UMAC: Status code: %d"),
                aCtxImpl.GetAuthenticationFrame().GetStatusCode());
            
            // copy the dot11 authentication frame header to tx buffer 
            os_memcpy( frame_ptr,
                &(aCtxImpl.GetAuthenticationFrame().iHeader),
                sizeof( aCtxImpl.GetAuthenticationFrame().iHeader) );
        
            // adjust to end of copy
            frame_ptr 
                += sizeof( aCtxImpl.GetAuthenticationFrame().iHeader );
            
            txFrameHdrAndFixedPartLen = sizeof( SAuthenticationFrame );
            }
    
        // set the WEP IV field
        // do a 16-bit fill
        const TUint16 fill_value( 0 );
        fill( 
            reinterpret_cast<TUint16*>(frame_ptr),
            ( reinterpret_cast<TUint16*>(frame_ptr) ) 
            + ( KWepIVLength / sizeof( fill_value ) ),
            fill_value );
    
        // adjust to begin of the authentication frame fixed fields 
        frame_ptr += KWepIVLength;
    
        if ( aCtxImpl.HtSupportedByNw() )
            {
            // copy authentication frame fixed fields after WEP IV
            os_memcpy( 
                frame_ptr,
                &(aCtxImpl.GetHtAuthenticationFrame().iAuthenticationFields),
                sizeof( 
                   aCtxImpl.GetHtAuthenticationFrame().iAuthenticationFields) );
        
            // adjust to end of copy
            frame_ptr += sizeof( 
                aCtxImpl.GetHtAuthenticationFrame().iAuthenticationFields );
        
            // copy challenge text from Rx-buffer to Tx-buffer
            
            const TUint KRxFramehdrAndFixedPartLen = 
                HtcFieldPresent( 
                    aCtxImpl, 
                    iLatestRxAuthRespPtr, 
                    iLatestRxAuthRespFlags ) ? 
                        sizeof( SHtAuthenticationFrame ) :
                        sizeof( SAuthenticationFrame );
            os_memcpy( frame_ptr, 
                iLatestRxAuthRespPtr + KRxFramehdrAndFixedPartLen,
                KChallengeTextLength + KInfoElementHeaderLength );            
            }
        else
            {
            // copy authentication frame fixed fields after WEP IV
            os_memcpy( 
                frame_ptr,
                &(aCtxImpl.GetAuthenticationFrame().iAuthenticationFields),
                sizeof(
                    aCtxImpl.GetAuthenticationFrame().iAuthenticationFields) );
        
            // adjust to end of copy
            frame_ptr += sizeof( 
                aCtxImpl.GetAuthenticationFrame().iAuthenticationFields );
        
            // copy challenge text from Rx-buffer to Tx-buffer
            os_memcpy( frame_ptr, 
                iLatestRxAuthRespPtr + sizeof( SAuthenticationFrame ),
                KChallengeTextLength + KInfoElementHeaderLength );
            }
    
        // trace the frame critter
        OsTracePrint( KUmacAuth, (TUint8*)("UMAC: dot11 authenticate frame tx:"),
            *(reinterpret_cast<const Sdot11MacHeader*>(start_of_frame)) );
        
        const WHA::TQueueId queue_id 
            = QueueId( aCtxImpl, start_of_frame );
    
        // push the frame to packet scheduler for transmission
        status = aCtxImpl.PushPacketToPacketScheduler(
            start_of_frame,
            txFrameHdrAndFixedPartLen 
            + KChallengeTextLength 
            + KInfoElementHeaderLength
            + KWepIVLength
            + KWEPICVLength,
            queue_id,
            E802Dot11FrameTypeAuthSeqNmbr3,
            NULL,
            EFalse,
            EFalse,
            ETrue );
    
        if ( !status )
            {
            // as we came here we did get an internal Tx buffer for the frame
            // but packet push to scheduler failed. In this case we need to
            // cancel the internal Tx buffer reservation as we will request it
            // again when the Packet Scheduler is again ready for packet push
            aCtxImpl.MarkInternalTxBufFree();
            }
        }
    else
        {
        // we didn't get a Tx buffer => frame not sent. EFalse will be returned
        // to indicate that
        OsTracePrint( KUmacAuth, (TUint8*)
            ("UMAC: WlanDot11SharedAuthPending::SendAuthSeqNbr3Frame: no internal Tx buffer available") );
        }
        
    return status;
    }

// -----------------------------------------------------------------------------
// If we land here it means that we have received a frame of somekind
// with a success status
// -----------------------------------------------------------------------------
//
void WlanDot11SharedAuthPending::OnReceiveFrameSuccess(
    WlanContextImpl& aCtxImpl,
    const void* aFrame,
    TUint16 /*aLength*/,
    WHA::TRcpi /*aRcpi*/,
    TUint32 aFlags,
    TUint8* /*aBuffer*/ )
    {
    // receive success
    // parse frame in order to determine is it what we desire
    const SManagementFrameHeader* frame_hdr 
        = static_cast<const SManagementFrameHeader*>(aFrame);
    
    TBool type_match( EFalse );

    iFlags &= ~KAuthReceived;
    iFlags &= ~KAuthSuccess;
    
    if (// can we accept this frame 
        // is this a management type + authentication subtype frame 
        IsRequestedFrameType( 
        frame_hdr->iFrameControl.iType,
        E802Dot11FrameTypeAuthentication, type_match )
        // AND our MAC address is DA
        && (frame_hdr->iDA == aCtxImpl.iWlanMib.dot11StationId)
        // AND we are in correct state
        && ( iState == EWAIT4AUTHRESPONSE )
        )
        {

        // this is a valid authentication frame targeted to us
        // mark it so
        iFlags |= KAuthReceived;

        // cancel authentication timer
        aCtxImpl.CancelTimer();

        // at this point we don't know is this a authentication success 
        // or failure scenario for this frame exchange
        
        if ( ResolveAuthMessage( 
                aCtxImpl, 
                K802Dot11AuthModeShared, 
                aFrame, 
                aFlags ) )
            {
            // authentication frame exchange was a success
            // mark it also
            iFlags |= KAuthSuccess;
            // store pointer to the received frame. We need it to extract
            // the challenge text from the frame
            iLatestRxAuthRespPtr = reinterpret_cast<const TUint8*>(aFrame);
            // store also its receive flags for the same purpose
            iLatestRxAuthRespFlags = aFlags;
            
            OsTracePrint( KUmacAuth, (TUint8*)
                ("UMAC: dot11-sharedauthpending * authentication frame exchange success"));
            OsTracePrint( KUmacAuth, (TUint8*)("UMAC: sequence number expected: %d"),
                aCtxImpl.GetAuthSeqNmbrExpected());               

            if ( !((aCtxImpl.GetAuthSeqNmbrExpected() 
                == E802Dot11AuthenticationSeqNmbr2)
                || (aCtxImpl.GetAuthSeqNmbrExpected() 
                == E802Dot11AuthenticationSeqNmbr4 ))
                )
                {               
                // catch a programming error
                OsTracePrint( KErrorLevel, 
                    (TUint8*)("UMAC: sequence number expected"),
                    aCtxImpl.GetAuthSeqNmbrExpected());
                OsAssert( (TUint8*)("UMAC: panic"),
                    (TUint8*)(WLAN_FILE), __LINE__ );
                }
            }
        else 
            {
            // authentication frame exchange was a failure
                        
            OsTracePrint( KUmacAuth, (TUint8*)
                ("UMAC: dot11-sharedauthpending * authentication failure"));
            OsTracePrint( KUmacAuth, (TUint8*)
                ("UMAC: sequence number expected: %d"),
                aCtxImpl.GetAuthSeqNmbrExpected());               

            // authentication response message was NOT valid
            // lets's see why that's the case 
            const SAuthenticationFixedFields* auth_fields 
                = reinterpret_cast<const SAuthenticationFixedFields*>
                  (reinterpret_cast<const TUint8*>(aFrame) + 
                   sizeof( SManagementFrameHeader ));

            OsTracePrint( KWarningLevel | KUmacAuth, (TUint8*)
                ("UMAC: dot11-sharedauthpending * authentication failure") );
            OsTracePrint( KWarningLevel | KUmacAuth, (TUint8*)
                ("UMAC: authentication status code: %d"), 
                auth_fields->StatusCode() );
                        
            // set the completion (error) code value returned to user mode
            // as the dot11idle state does the OID completion in this case
            if ( auth_fields->StatusCode() == E802Dot11StatusSuccess )
                {
                // network returned success code but still an error in the
                // authentication sequence has occurred.
                // Either an authentication frame was received out of
                // sequence or the algorithm number wasn't the expected one
                aCtxImpl.iStates.iIdleState.Set( KErrGeneral );
                }
            else
                {
                // complete with the network returned error code
                aCtxImpl.iStates.iIdleState.Set( auth_fields->StatusCode() );           
                }
            }
        }
    else    
        {
        // incorrect frame type
        // or we are not in correct state 
        // so we shall discard its processing
        }

    if ( iFlags & KAuthReceived )
        {
        // authentication response was received
        // either success or failure
        // we don't really care in this state
        
        Fsm( aCtxImpl, ERXAUTHRESPONSE );
        }
    }
