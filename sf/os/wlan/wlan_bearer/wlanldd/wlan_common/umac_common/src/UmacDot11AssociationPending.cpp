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
* Description:   Implementation of the WlanDot11AssociationPending class
*
*/

/*
* %version: 45 %
*/

#include "config.h"
#include "UmacDot11AssociationPending.h"
#include "UmacContextImpl.h"
#include "umacelementlocator.h"
#include "umacwhaconfigureac.h"
#include "802dot11DefaultValues.h"

#ifndef NDEBUG
const TInt8 WlanDot11AssociationPending::iName[] = "dot11-associationpending";

const TUint8 WlanDot11AssociationPending::iStateName
    [ESTATEMAX][KMaxStateStringLength] = 
    {
        {"EINIT"}, 
        {"ETXASSOCREQFRAME"}, 
        {"EWAIT4ASSOCRESPONSE"},
        {"ECONFIGUREAC"},
        {"EWAIT4PUSHPACKET"},
        {"ECONTINUEDOT11TRAVERSE"}
    };

const TUint8 WlanDot11AssociationPending::iEventName
    [EEVENTMAX][KMaxEventStringLength] = 
    {
        {"ESTATEENTRY"}, 
        {"ERXASSOCRESPONSE"},
        {"ETX_ASSOCFRAME_XFER"},
        {"ETXCOMPLETE"},
        {"ETIMEOUT"},
        {"ETX_SCHEDULER_FULL"},
        {"EPUSHPACKET"}
    };
#endif 

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void WlanDot11AssociationPending::Entry( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( KUmacProtocolState | KUmacAssoc, 
        (TUint8*)("UMAC: WlanDot11AssociationPending::Entry()"));

    if ( aCtxImpl.WsaCmdActive() )
        {
        // sanity checking code
        OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
        }

    // no need to do event dispatching as this
    // thing is triggered by the user and
    // is executed synchronously as we only do OID completion
    // at the end of authenticate + association process
    
    if ( iState == EINIT )
        {
        // this is the start of the the FSM actions
        Fsm( aCtxImpl, ESTATEENTRY );
        }
    else
        {
        // this is NOT the start of the the FSM actions
        // note that we send the ETXCOMPLETE event as the states
        // that wait for it are the only ones that can be interrupted
        // as they are asynchronous operations by nature
        // and wait for corresponding WHA completion method
        Fsm( aCtxImpl, ETXCOMPLETE );        
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11AssociationPending::Exit( 
    WlanContextImpl& /*aCtxImpl*/ )
    {
    // reset our local FSM for the next time...
    iState = EINIT;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11AssociationPending::Fsm( 
    WlanContextImpl& aCtxImpl, 
    TEvent aEvent )
    {
    OsTracePrint( KUmacAssoc, 
        (TUint8*)("UMAC: WlanDot11AssociationPending::Fsm(): FSM EVENT") );
#ifndef NDEBUG
    OsTracePrint( KUmacAssoc, (TUint8*)("UMAC: event:"));
    OsTracePrint( KUmacAssoc, iEventName[aEvent] );
    OsTracePrint( KUmacAssoc, (TUint8*)("UMAC: state:"));
    OsTracePrint( KUmacAssoc, iStateName[iState] );
#endif

    switch ( aEvent )
        {
        case ESTATEENTRY:
            OnStateEntryEvent( aCtxImpl );
            break;
        case ETX_ASSOCFRAME_XFER:
            OnTxAssocFrameXferEvent( aCtxImpl );
            break;
        case ERXASSOCRESPONSE:
            OnRxAssociationResponseEvent( aCtxImpl );
            break;
        case ETXCOMPLETE:
            OnTxCompleteEvent( aCtxImpl );
            break;
        case ETIMEOUT:
            OnTimeoutEvent( aCtxImpl );
            break;
        case ETX_SCHEDULER_FULL:
            OnTxSchedulerFullEvent( aCtxImpl );
            break;
        case EPUSHPACKET:
            OnPushPacketEvent( aCtxImpl );
            break;
        default:
            // cath internal FSM programming error
            OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
            break;
        }
    }

// -----------------------------------------------------------------------------
// Handler for state entry event.
// -----------------------------------------------------------------------------
//
void WlanDot11AssociationPending::OnStateEntryEvent( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( 
        KUmacAssoc, 
        (TUint8*)("UMAC: WlanDot11AssociationPending::OnStateEntryEvent()"));

    switch ( iState )
        {
        case EINIT:
            iFlags = 0;
            ChangeInternalState( aCtxImpl, ETXASSOCREQFRAME );            
            break;
        case ETXASSOCREQFRAME:
            // send the associate frame
            if ( !SendAssociationRequest( aCtxImpl ) )
                {
                // tx of dot11-associate frame failed  
                // because packet scheduler was full
                // or because we didn't get a Tx buffer
                // so we enter to a wait state
                Fsm( aCtxImpl, ETX_SCHEDULER_FULL );
                }            
            break;
        case EWAIT4ASSOCRESPONSE:
            // start a timer to wait for the response frame
            StartAssociationFrameResponseTimer( aCtxImpl );
            break;
        case ECONFIGUREAC:
            ConfigureAc( aCtxImpl );
            break;
        case ECONTINUEDOT11TRAVERSE:
            ContinueDot11StateTraversal( aCtxImpl );
            break;
        case EWAIT4PUSHPACKET:
            // nothing to do here than wait 
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
// Handler for association response timeout event.
// -----------------------------------------------------------------------------
//
void WlanDot11AssociationPending::OnTimeoutEvent( 
    WlanContextImpl& aCtxImpl )
    {
    // set completion code
    // as dot11idle state does the OID completion
    aCtxImpl.iStates.iIdleState.Set( KErrTimedOut );
    ChangeInternalState( aCtxImpl, ECONTINUEDOT11TRAVERSE );
    }

// -----------------------------------------------------------------------------
// Handler for associate request frame 
// has been xferred to the WLAN device event
// -----------------------------------------------------------------------------
//
void WlanDot11AssociationPending::OnTxAssocFrameXferEvent( 
    WlanContextImpl& aCtxImpl )
    {
    // change state
    ChangeInternalState( aCtxImpl, EWAIT4ASSOCRESPONSE );
    }

// -----------------------------------------------------------------------------
// Handler for rx association response event.
// -----------------------------------------------------------------------------
//
void WlanDot11AssociationPending::OnRxAssociationResponseEvent( 
    WlanContextImpl& aCtxImpl )
    {
    switch ( iState )
        {
        case EWAIT4ASSOCRESPONSE:
            if ( iFlags & KConfigureAc )
                {
                ChangeInternalState( aCtxImpl, ECONFIGUREAC );                                
                }
            else
                {
                ChangeInternalState( aCtxImpl, ECONTINUEDOT11TRAVERSE );                
                }
            break;
        default:
            // this means that we have recieved a valid dot11 
            // associate response frame that we are waiting for
            // but we are not internally in such a state
            // only feasible situation for this is that 
            // someone  has skipped a call to the packet xfer method
            // that informs of associate request frame 
            // xfer to the WLAN device.
            // other case is that our fsm is totally messed up
            // so we catch this
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
// 
// -----------------------------------------------------------------------------
//
void WlanDot11AssociationPending::OnTxCompleteEvent( 
    WlanContextImpl& aCtxImpl )
    {
    switch ( iState )
        {
        case ECONFIGUREAC:
            // continue state traversal
            ChangeInternalState( aCtxImpl, ECONTINUEDOT11TRAVERSE );
            break;
        default:
            // catch internal FSM programming error
            OsTracePrint( KErrorLevel, (TUint8*)("UMAC: state: %d"), iState);        
            OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
            break;
        }
    }

// -----------------------------------------------------------------------------
// Handler for scheduler full event upon trying to tx dot11-associate frame
// -----------------------------------------------------------------------------
//
void WlanDot11AssociationPending::OnTxSchedulerFullEvent( 
    WlanContextImpl& aCtxImpl )
    {
    // change state
    OsTracePrint( KWarningLevel | KUmacAssoc, (TUint8*)
        ("UMAC: packet scheduler full during association process") );

    ChangeInternalState( aCtxImpl, EWAIT4PUSHPACKET );
    }

// -----------------------------------------------------------------------------
// Handler for push packet to packet scheduler possible event
// -----------------------------------------------------------------------------
//
void WlanDot11AssociationPending::OnPushPacketEvent( 
    WlanContextImpl& aCtxImpl )
    {
    if ( iState == EWAIT4PUSHPACKET )
        {
        ChangeInternalState( aCtxImpl, ETXASSOCREQFRAME );
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11AssociationPending::ContinueDot11StateTraversal( 
    WlanContextImpl& aCtxImpl )
    {
    if ( iFlags & KAssocSuccess )
        {
        // traverse to next dot11state
        ChangeState( aCtxImpl,
            *this,                                      // prev state   
            aCtxImpl.iStates.iInfrastructureModeInit    // next state
            );
        }
    else
        {
        // proceed back to idle
        //
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
void WlanDot11AssociationPending::ChangeInternalState( 
    WlanContextImpl& aCtxImpl, 
    TState aNewState )
    {
    OsTracePrint( KUmacAssoc, (TUint8*)
        ("UMAC: WlanDot11AssociationPending::ChangeInternalState(): old state:"));
#ifndef NDEBUG
    OsTracePrint( KUmacAssoc, iStateName[iState] );
    OsTracePrint( KUmacAssoc, (TUint8*)("new state:"));
    OsTracePrint( KUmacAssoc, iStateName[aNewState] );
#endif 
    iState = aNewState;
    Fsm( aCtxImpl, ESTATEENTRY );
    }
    
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
#ifndef NDEBUG 
const TInt8* WlanDot11AssociationPending::GetStateName( 
    TUint8& aLength ) const
    {
    aLength = sizeof( iName );
    return iName;
    }
#endif 

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11AssociationPending::SendAssociationRequest( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( KUmacAssoc, (TUint8*)
        ("UMAC: WlanDot11AssociationPending::SendAssociationRequest"));

    TBool status ( EFalse );
    TUint8* startOfFrame ( NULL );
    
    const TUint32 length_of_frame 
        = ConstructAssociationRequestFrame( aCtxImpl, startOfFrame );
    
    if ( length_of_frame )
        {
        // frame is ready for delivery in the tx buffer
        // send association request message to the AP

        const WHA::TQueueId queue_id 
            = QueueId( aCtxImpl, startOfFrame );

        // push the frame to packet scheduler for transmission
        status = aCtxImpl.PushPacketToPacketScheduler(
                    startOfFrame,
                    length_of_frame,
                    queue_id,
                    E802Dot11FrameTypeAssociationReq,
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
        // frame not ready for delivery. EFalse will be returned
        }
        
    return status;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TUint WlanDot11AssociationPending::ConstructAssociationRequestFrame( 
    WlanContextImpl& aCtxImpl,
    TUint8*& aStartOfFrame )
    {
    OsTracePrint( KUmacAssoc, (TUint8*)
        ("UMAC: WlanDot11AssociationPending::ConstructAssociationRequestFrame") );

    TUint32 lengthOfFrame ( 0 );
    
    // client doesn't have to take care of the tx buffer header space
    // as the method below does that by itself
    aStartOfFrame = aCtxImpl.TxBuffer( ETrue );

    if ( aStartOfFrame )
        {
        // construct association request frame
        // note that we don't need to set SA because we have already set it 
        // in the initialization phase of the state machine
        // Also capabilty information field is already set to frame template 
        // in AreNetworkRequirementsMet() method

        TUint8* buffer_ptr = aStartOfFrame;

        if ( aCtxImpl.HtSupportedByNw() && aCtxImpl.QosEnabled() )
            {
            // set the BSSID field     
            (aCtxImpl.GetHtAssociationRequestFrame()).iHeader.iBSSID = 
                aCtxImpl.GetBssId();
            // set the DA field 
            (aCtxImpl.GetHtAssociationRequestFrame()).iHeader.iDA = 
                aCtxImpl.GetBssId();
            // set listen interval (in units of beacon interval)
            (aCtxImpl.GetHtAssociationRequestFrame()).iFixedFields.iListenInterval 
                = KDot11ListenIntervalInMs / aCtxImpl.NetworkBeaconInterval();
    
            // copy frame to tx-buffer to correct offset
            os_memcpy( 
                buffer_ptr,
                &(aCtxImpl.GetHtAssociationRequestFrame()), 
                sizeof( SHtAssociationRequestFrame ) );
    
            buffer_ptr += sizeof( SHtAssociationRequestFrame );            
            }
        else
            {
            // set the BSSID field     
            (aCtxImpl.GetAssociationRequestFrame()).iHeader.iBSSID = 
                aCtxImpl.GetBssId();
            // set the DA field 
            (aCtxImpl.GetAssociationRequestFrame()).iHeader.iDA = 
                aCtxImpl.GetBssId();
            // set listen interval (in units of beacon interval)
            (aCtxImpl.GetAssociationRequestFrame()).iFixedFields.iListenInterval 
                = KDot11ListenIntervalInMs / aCtxImpl.NetworkBeaconInterval();
    
            // copy frame to tx-buffer to correct offset
            os_memcpy( 
                buffer_ptr,
                &(aCtxImpl.GetAssociationRequestFrame()), 
                sizeof( SAssociationRequestFrame ) );
    
            buffer_ptr += sizeof( SAssociationRequestFrame );
            }

        // set SSID IE
        
        SSsIdIE ssid_ie( (aCtxImpl.GetSsId()).ssid, 
            (aCtxImpl.GetSsId()).ssidLength );

        const TUint8 ssidIeLength( ssid_ie.GetIeLength() );
                
        os_memcpy(
            buffer_ptr, 
            &ssid_ie, 
            ssidIeLength );

        buffer_ptr += ssidIeLength;

        // set supported rates IE

        const TUint8 supportedRatesIeLength( 
            aCtxImpl.GetOurSupportedRatesIE().GetIeLength() );

        os_memcpy( 
            buffer_ptr, 
            &(aCtxImpl.GetOurSupportedRatesIE()), 
            supportedRatesIeLength );

        buffer_ptr += supportedRatesIeLength;

        if ( aCtxImpl.HtSupportedByNw() )
            {
            // set HT capabilities element
    
            const TUint8 htCapabilitiesIeLength( 
                aCtxImpl.GetOurHtCapabilitiesIe().GetIeLength() );
    
            os_memcpy( 
                buffer_ptr, 
                &(aCtxImpl.GetOurHtCapabilitiesIe()), 
                htCapabilitiesIeLength );
    
            buffer_ptr += htCapabilitiesIeLength;

            OsTracePrint( KUmacAssoc, (TUint8*)
                ("UMAC: HT capabilities element added") );
            }
        
        // set extended supported rates IE if it's not empty 
        if ( aCtxImpl.GetOurExtendedSupportedRatesIE().GetElementLength() )
            {        
            const TUint8 extSupportedRatesIeLength( 
                aCtxImpl.GetOurExtendedSupportedRatesIE().GetIeLength() );

            os_memcpy( 
                buffer_ptr, 
                &(aCtxImpl.GetOurExtendedSupportedRatesIE()), 
                extSupportedRatesIeLength );

            buffer_ptr += extSupportedRatesIeLength;        
            }

        // set any IEs possibly provided by management client
        const TUint8* ieData( aCtxImpl.IeData() );
        if ( ieData )
            {   
            const TUint16 ieDataLength( aCtxImpl.IeDataLength() );
            
            os_memcpy( buffer_ptr, ieData, ieDataLength );
            buffer_ptr += ieDataLength;

            OsTracePrint( KUmacAssoc, (TUint8*)
                ("UMAC: management client supplied IE(s) added") );                
            }
            
        // set WMM IE if needed
        if ( aCtxImpl.QosEnabled() )        
            {
            const TUint8 wmmIeLength( aCtxImpl.OurWmmIe().GetIeLength() );

            os_memcpy( 
                buffer_ptr, 
                &(aCtxImpl.OurWmmIe()),
                wmmIeLength);

            buffer_ptr += wmmIeLength;        

            OsTracePrint( KUmacAssoc, (TUint8*)
                ("UMAC: WMM IE added") );            
            }
        
        // length of frame
        lengthOfFrame = buffer_ptr - aStartOfFrame;
        }
    else
        {
        // we didn't get a Tx buffer => frame not sent. Zero will be returned 
        // as the frame length to indicate that
        OsTracePrint( KUmacAssoc, (TUint8*)
            ("UMAC: WlanDot11AssociationPending::ConstructAssociationRequestFrame: no internal Tx buffer available") );
        }
    
    return lengthOfFrame;        
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
T802Dot11ManagementStatusCode 
WlanDot11AssociationPending::IsRxAssociationSuccess( 
    WlanContextImpl& aCtxImpl,
    const void* aFrame,
    TUint32 aFlags )
    {
    OsTracePrint( KUmacAssoc, (TUint8*)
        ("UMAC: WlanDot11AssociationPending::IsRxAssociationSuccess"));
    // get the fixed fields from association response    
    const SAssociationResponseFixedFields* fields = 
        HtcFieldPresent( aCtxImpl, aFrame, aFlags ) ?
            reinterpret_cast<const SAssociationResponseFixedFields*>
              (reinterpret_cast<const TUint8*>(aFrame) + 
               sizeof( SHtManagementFrameHeader )) :
            reinterpret_cast<const SAssociationResponseFixedFields*>
              (reinterpret_cast<const TUint8*>(aFrame) + 
               sizeof( SManagementFrameHeader ) );

    // store AID 
    OsTracePrint( KUmacAssoc, (TUint8*)
        ("UMAC: WlanDot11AssociationPending::IsRxAssociationSuccess: AID extracted from association response: 0x%04x"), 
        fields->Aid() );
    aCtxImpl.Aid( fields->Aid() );
    
    return static_cast<T802Dot11ManagementStatusCode>(
        fields->StatusCode() );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11AssociationPending::StartAssociationFrameResponseTimer( 
    WlanContextImpl& aCtxImpl )
    {
    // start association response timeout timer
    const TUint32 timeout( dot11AssociateResponseTimeout * KTU );
    
    OsTracePrint( KUmacAssoc, (TUint8*)
        ("UMAC: WlanDot11AssociationPending::StartAssociationFrameResponseTimer: timeout in microseconds: %d"), 
        timeout);
    
    aCtxImpl.iUmac.RegisterTimeout( timeout );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11AssociationPending::ConfigureAc( 
    WlanContextImpl& aCtxImpl )
    {
    ConfigureAcParams( aCtxImpl );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11AssociationPending::ReceivePacket( 
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
    OsTracePrint( KUmacAssoc, (TUint8*)
        ("UMAC: dot11-associatepending::ReceivePacket()") );

    if ( aStatus == WHA::KSuccess )
        {
        // receive success
        const Sdot11MacHeader* dot11_hdr( 
            static_cast<const Sdot11MacHeader*>(aFrame) );

        // we accept only frames with ToDS bit cleared
        if ( dot11_hdr->IsToDsBitSet() )
            {
            OsTracePrint( KWarningLevel | KUmacAssoc, 
                (TUint8*)("UMAC: associating to BSS:") );
            OsTracePrint( KWarningLevel | KUmacAssoc, 
                (TUint8*)("UMAC: rx-frame: ToDs bit set, discard frame") );

            // release the Rx buffer & abort
            aCtxImpl.iUmac.MarkRxBufFree( aBuffer );
            return;
            }

        const TBool class3_frame( IsClass3Frame( 
            dot11_hdr->iFrameControl.iType ) );
        const TBool unicast_addr( !IsGroupBitSet( dot11_hdr->iAddress1 ) );

        if ( class3_frame && unicast_addr )
            {
            OsTracePrint( KWarningLevel | KUmacAssoc, 
                (TUint8*)("UMAC: associating to BSS:") );
            OsTracePrint( KWarningLevel | KUmacAssoc, 
                (TUint8*)("rx class3 frame with unicast DA address") );

            // class 3 frame rx and unicast address in address1 field

            // That's not the frame we are expecting so release the Rx buffer
            aCtxImpl.iUmac.MarkRxBufFree( aBuffer );
            
            if ( !Associated() )
                {
                // we do not have a valid association with the 
                // BSS where the frame came
                OsTracePrint( KWarningLevel | KUmacAssoc, 
                    (TUint8*)("UMAC: TxDisassociate") );
                
                // set the BSSID of the existing network; and the DA
                if ( aCtxImpl.HtSupportedByNw() )
                    {
                    (aCtxImpl.GetHtDisassociationFrame()).iHeader.iBSSID 
                        = aCtxImpl.GetBssId();
                    (aCtxImpl.GetHtDisassociationFrame()).iHeader.iDA 
                        = aCtxImpl.GetBssId();                    
                    }
                else
                    {
                    (aCtxImpl.GetDisassociationFrame()).iHeader.iBSSID 
                        = aCtxImpl.GetBssId();
                    (aCtxImpl.GetDisassociationFrame()).iHeader.iDA 
                        = aCtxImpl.GetBssId();
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
            else
                {
                // this section is left intentionally empty
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
    else    // --- aStatus == WHA::KSuccess ---
        {
        // receive failed, so discard and release the Rx buffer
        aCtxImpl.iUmac.MarkRxBufFree( aBuffer );        
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void WlanDot11AssociationPending::OnReceiveFrameSuccess(
    WlanContextImpl& aCtxImpl,
    const void* aFrame,
    TUint16 aLength,
    WHA::TRcpi aRcpi,
    TUint32 aFlags,
    TUint8* aBuffer )
    {
    // receive success
    // parse frame in order to determine if it is what we are expecting
    SManagementFrameHeader* frame_hdr
        = static_cast<SManagementFrameHeader*>
        (const_cast<TAny*>(aFrame));

    TBool type_match( EFalse );

    if ( // this an association response frame
         IsRequestedFrameType(
             frame_hdr->iFrameControl.iType,
             E802Dot11FrameTypeAssociationResp, type_match )
         // AND our MAC address is DA
         && ( frame_hdr->iDA == aCtxImpl.iWlanMib.dot11StationId )
         // AND we haven't received the assoc response yet
         && ( !( iFlags & KAssocReceived ) ) )
        {
        T802Dot11ManagementStatusCode status
            = IsRxAssociationSuccess( aCtxImpl, aFrame, aFlags );

        if ( status == E802Dot11StatusSuccess )
            {
            // --- begin WMM
            if ( aCtxImpl.QosEnabled() )
                {
                WlanElementLocator elementLocator(
                    reinterpret_cast<const TUint8*>( aFrame ) +
                    sizeof( SManagementFrameHeader ) +
                    sizeof( SAssociationResponseFixedFields ),
                    aLength -
                    sizeof( SManagementFrameHeader ) +
                    sizeof( SAssociationResponseFixedFields ) );

                TUint8 length( 0 );
                const TUint8* data( NULL );

                // is WMM Parameter Element present
                if ( elementLocator.InformationElement(
                    E802Dot11VendorSpecificIE,
                    KWmmElemOui,
                    KWmmElemOuiType,
                    KWmmParamElemOuiSubtype,
                    length,
                    &data ) == WlanElementLocator::EWlanLocateOk )
                    {
                    // WMM Parameter Element found
                    OsTracePrint( KUmacAssoc, (TUint8*)
                        ("UMAC: WlanDot11AssociationPending::ReceivePacket(): WMM param set cnt: %d"),
                        (reinterpret_cast<const SWmmParamElemData*>(data))->ParameterSetCount() );

                    if ( (reinterpret_cast<const SWmmParamElemData*>
                        (data))->ParameterSetCount()
                        != aCtxImpl.WmmParameterSetCount() )
                        {
                        // AC parameters have changed => parse again
                        ParseAcParameters(
                            aCtxImpl,
                            reinterpret_cast<const SWmmParamElemData&>(*data ) );
                        // in this case we need to re-issue configure AC wha cmd
                        // so make a note of that
                        iFlags |= KConfigureAc;
                        }
                    }
                else
                    {
                    // protocol error from AP; just try to continue with
                    // current WMM parameters
                    OsTracePrint( KWarningLevel | KUmacAssoc, (TUint8*)
                        ("UMAC: WlanDot11AssociationPending::ReceivePacket(): PROTOCOL ERROR from AP side") );
                    }
                }
            // --- end WMM
            }
        else
            {
            // association failed
            OsTracePrint( KWarningLevel | KUmacAssoc, (TUint8*)
                ("UMAC: dot11-association denied; status code: %d"),
                status);

            // in this case we will go back to idle state
            // where the connect oid will be completed.
            // So set the completion code value to be returned to user mode
            aCtxImpl.iStates.iIdleState.Set( status );
            }

        // forward the association response frame to Wlan Mgmt client
        if ( XferDot11FrameToMgmtClient(
                aCtxImpl,
                aFrame,
                aLength,
                aRcpi,
                aBuffer ) )
            {
            // forwarding succeeded. Now we can say that we have received
            // the assoc response successfully

            iFlags |= KAssocReceived;
            aCtxImpl.CancelTimer();

            if ( status == E802Dot11StatusSuccess )
                {
                OsTracePrint( KUmacAssoc, (TUint8*)
                    ("UMAC: association pending: association success") );

                iFlags |= KAssocSuccess;
                }

            Fsm( aCtxImpl, ERXASSOCRESPONSE );
            }
        else
            {
            // forwarding the frame to WLAN Mgmt client failed, which
            // won't happen in this situation under the normal circumstances.
            // Anyhow, it has happened now, so we have no other choice than to
            // discard the frame. The Rx buffer has already been
            // released. So no action here
            }
        }
    else
        {
        // not a valid frame in this situation; we just silently discard it
        OsTracePrint( KUmacAssoc | KUmacDetails, (TUint8*)
            ("UMAC: WlanDot11AssociationPending::OnReceiveFrameSuccess: "
             "not relevant frame; ignore"));

        // release the Rx buffer
        aCtxImpl.iUmac.MarkRxBufFree( aBuffer );
        }
    }

// -----------------------------------------------------------------------------
// completion method called when packet has been xferred to the WLAN device
// -----------------------------------------------------------------------------
//
void WlanDot11AssociationPending::OnPacketTransferComplete( 
    WlanContextImpl& aCtxImpl, 
    TUint32 aPacketId,
    TDataBuffer* aMetaHeader )
    {
    OsTracePrint( KUmacAssoc, (TUint8*)
        ("UMAC: WlanDot11AssociationPending::OnPacketTransferComplete"));

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

    if ( aPacketId == E802Dot11FrameTypeAssociationReq )
        {
        // associate tx message has been xferred to the WLAN device
        
        Fsm( aCtxImpl, ETX_ASSOCFRAME_XFER );
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11AssociationPending::OnTimeout( 
    WlanContextImpl& aCtxImpl )
    {
    TBool stateChange ( ETrue );
    
    switch ( iState )
        {
        case EWAIT4ASSOCRESPONSE:
            // association timeout
            
            OsTracePrint( KWarningLevel | KUmacAssoc, (TUint8*)
                ("UMAC: WlanDot11AssociationPending::OnTimeout: timeout => association failed!") );

            Fsm( aCtxImpl, ETIMEOUT );
            
            // in this case we return ETrue, i.e. signal the caller that a 
            // state change occurred
                        
            break;
        default:
            // a timeout occurred when we weren't expecting it (yet). This 
            // means that a timeout callback had already been registered when
            // we tried to cancel this timer the previous time (regarding
            // authentication). So this callback is not relevant for 
            // association and can be ignored. 
            
            OsTracePrint( KWarningLevel | KUmacAssoc, (TUint8*)
                ("UMAC: WlanDot11AssociationPending::OnTimeout: irrelevant timeout; ignored") );

            // Signal the caller that no state change occurred
            stateChange = EFalse;
        }
        
    return stateChange;
    }

// -----------------------------------------------------------------------------
// packet sceduler notification that a packet push is guaranteed to succeed 
// -----------------------------------------------------------------------------
//
void WlanDot11AssociationPending::OnPacketPushPossible( 
    WlanContextImpl& aCtxImpl )
    {
    // feed a critter to the fsm
    Fsm( aCtxImpl, EPUSHPACKET );
    }
