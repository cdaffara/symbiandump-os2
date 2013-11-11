/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of the WlanDot11OpenAuthPending class
*
*/

/*
* %version: 23 %
*/

#include "config.h"
#include "UmacDot11OpenAuthPending.h"
#include "UmacContextImpl.h"

#ifndef NDEBUG 
const TInt8 WlanDot11OpenAuthPending::iName[] = "dot11-openauthpending";
#endif // !NDEBUG 

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
#ifndef NDEBUG 
const TInt8* WlanDot11OpenAuthPending::GetStateName( 
    TUint8& aLength ) const
    {
    aLength = sizeof( iName );
    return iName;
    }
#endif

// -----------------------------------------------------------------------------
// Handler for state entry event.
// -----------------------------------------------------------------------------
//
void WlanDot11OpenAuthPending::OnStateEntryEvent( 
    WlanContextImpl& aCtxImpl )
    {
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
            // send the authenticate frame
            if ( !SendAuthSeqNbr1Frame( aCtxImpl ) )
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
            // start a timer to wait for the response frame
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
void WlanDot11OpenAuthPending::OnRxAuthResponseEvent( 
    WlanContextImpl& aCtxImpl )
    {
    switch ( iState )
        {
        case EWAIT4AUTHRESPONSE:
            ChangeInternalState( aCtxImpl, ECONTINUEDOT11TRAVERSE );
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
            OsTracePrint( KErrorLevel, (TUint8*)("UMAC: state:"));
            OsTracePrint( KErrorLevel, iStateName[iState] );
#endif 
            OsAssert( (TUint8*)("UMAC: panic"), 
                (TUint8*)(WLAN_FILE), __LINE__ );
            break;
        }
    }

// -----------------------------------------------------------------------------
// set appropriate used algorithm number to authenticate request frame
// -----------------------------------------------------------------------------
//
void WlanDot11OpenAuthPending::OnSetAlgorithmNumber( 
    WlanContextImpl& aCtxImpl )
    {
    if ( aCtxImpl.HtSupportedByNw() )
        {
        aCtxImpl.GetHtAuthenticationFrame().SetAlgorithmNmbr( 
            aCtxImpl.AuthenticationAlgorithmNumber() );
        }
    else
        {
        aCtxImpl.GetAuthenticationFrame().SetAlgorithmNmbr( 
            aCtxImpl.AuthenticationAlgorithmNumber() );
        }
    }

// -----------------------------------------------------------------------------
// If we land here it means that we have received a frame of somekind
// with a status success
// -----------------------------------------------------------------------------
//
void WlanDot11OpenAuthPending::OnReceiveFrameSuccess(
    WlanContextImpl& aCtxImpl,
    const void* aFrame,
    TUint16 /*aLength*/,
    WHA::TRcpi /*aRcpi*/,
    TUint32 aFlags,
    TUint8* /*aBuffer*/ )
    {
    // parse frame in order to determine is it what we want
    const SManagementFrameHeader* frame_hdr 
        = static_cast<const SManagementFrameHeader*>(aFrame);

    TBool type_match( EFalse );
    
    if ( // can we accept this frame 
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
        // cancel authentication frame response timer
        aCtxImpl.CancelTimer(); 

        iFlags |= KAuthReceived;

        // we have got a hit...let's proceed
        // store the used authentication algorithm number
        // for later use
        const TUint16 used_auth_algorithm = 
            aCtxImpl.AuthenticationAlgorithmNumber();
                
        // validate the authentication frame
        if ( ResolveAuthMessage( 
                aCtxImpl, 
                used_auth_algorithm, 
                aFrame, 
                aFlags ) )
            {
            // authentication success 
            OsTracePrint( KUmacAuth, (TUint8*)
                ("UMAC: dot11-openauthpending * authentication success") );

            // mark it also
            iFlags |= KAuthSuccess;
            }
        else
            {
            // authentication response message was NOT ok
            //   lets's see why that's the case 
            const SAuthenticationFixedFields* auth_fields 
                = reinterpret_cast<const SAuthenticationFixedFields*>
                  (reinterpret_cast<const TUint8*>(aFrame) + 
                   sizeof( SManagementFrameHeader ));
            
            TInt completion_code( KErrGeneral );

            if ( // is open mode authentication
                ( auth_fields->AlgorithmNumber() == used_auth_algorithm )
                // AND is authentication transaction sequence number expected
                && ( auth_fields->SequenceNumber() 
                == aCtxImpl.GetAuthSeqNmbrExpected())
                )
                {
                OsTracePrint( KWarningLevel | KUmacAuth, (TUint8*)
                    ("UMAC: dot11-openauthpending authentication denied") );
                OsTracePrint( KWarningLevel | KUmacAuth, (TUint8*)
                    ("UMAC: status code from frame: %d"), 
                    auth_fields->StatusCode() );
                
                if ( auth_fields->StatusCode() == E802Dot11StatusSuccess )
                    {
                    // catch a internal programming error
                    OsAssert( (TUint8*)("UMAC: panic"), 
                        (TUint8*)(WLAN_FILE), __LINE__ );
                    }
                
                // status code was something else than status success
                completion_code = auth_fields->StatusCode();
                }
            else
                {
                // a malformed message, but it is a failure in any case
                OsTracePrint( KWarningLevel | KUmacAuth, (TUint8*)
                    ("UMAC: dot11-openauthpending authentication failure") );
                OsTracePrint( KWarningLevel | KUmacAuth, (TUint8*)
                    ("UMAC: auth message not valid") );
                }

            // set the completion code value returned to user mode
            // as the dot11idle state does the OID completion in this case
            aCtxImpl.iStates.iIdleState.Set( completion_code );
            }
        }
    else    // can we accept this frame
        {
        // not a valid type of frame 
        // or we are not in correct state 
        // so we shall discard it's processing
        }

    if ( iFlags & KAuthReceived )
        {
        // authentication response was received
        // either success or failure
        // we don't really care in this state
        
        Fsm( aCtxImpl, ERXAUTHRESPONSE );
        }
    }
