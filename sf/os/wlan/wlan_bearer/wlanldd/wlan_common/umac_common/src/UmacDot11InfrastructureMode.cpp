/*
* Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of the WlanDot11InfrastructureMode class
*
*/

/*
* %version: 64 %
*/

#include "config.h"
#include "UmacDot11InfrastructureMode.h"
#include "UmacWsaAddKey.h"
#include "umacaddbroadcastwepkey.h"
#include "UmacContextImpl.h"
#include "UmacWsaKeyIndexMapper.h"
#include "UmacWsaWriteMib.h"
#include "umacelementlocator.h"
#include "umacwhaconfigureac.h"
#include "FrameXferBlock.h"


// ================= MEMBER FUNCTIONS =======================


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanDot11InfrastructureMode::Connect(
    WlanContextImpl& aCtxImpl,
    const TSSID& aSSID,                 
    const TMacAddress& aBSSID,          
    TUint16 aAuthAlgorithmNbr,      
    TEncryptionStatus aEncryptionStatus,
    TBool /*aIsInfra*/,
    TUint16 aScanResponseFrameBodyLength,
    const TUint8* aScanResponseFrameBody,
    const TUint8* aIeData,
    TUint16 aIeDataLength )
    {
    // store data for later access.
    // Pointers supplied are valid to the point the
    // corresponding completion method is called

    aCtxImpl.SetScanResponseFrameBody( aScanResponseFrameBody );
    aCtxImpl.SetScanResponseFrameBodyLength( aScanResponseFrameBodyLength );            
    aCtxImpl.IeData( aIeData );
    aCtxImpl.IeDataLength( aIeDataLength );

    return Connect( 
        aCtxImpl, 
        aSSID, 
        aBSSID, 
        aAuthAlgorithmNbr, 
        aEncryptionStatus
        );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanDot11InfrastructureMode::Connect(
    WlanContextImpl& aCtxImpl,
    const TSSID& aSSID,                 
    const TMacAddress& aBSSID,          
    TUint16 aAuthAlgorithmNbr,      
    TEncryptionStatus aEncryptionStatus )
    {
    // construct disassociation frame
    // note that we don't have to set SA because we have already set it
    // in the initialize phase of the dot11 state machine
    //

    // set the BSSID of the existing network
    (aCtxImpl.GetDisassociationFrame()).iHeader.iBSSID = aCtxImpl.GetBssId();
    (aCtxImpl.GetHtDisassociationFrame()).iHeader.iBSSID = aCtxImpl.GetBssId();
    // set the DA
    (aCtxImpl.GetDisassociationFrame()).iHeader.iDA = aCtxImpl.GetBssId();
    (aCtxImpl.GetHtDisassociationFrame()).iHeader.iDA = aCtxImpl.GetBssId();
    
    // set current BSSID in reassociation request frame
    (aCtxImpl.GetReassociationRequestFrame()).iFixedFields.iCurrentApAddress = 
        aCtxImpl.GetBssId();
    (aCtxImpl.GetHtReassociationRequestFrame()).iFixedFields.iCurrentApAddress = 
        aCtxImpl.GetBssId();

    // ... and make a note that we need to perform reassociation 
    // instead of association later on towards the new AP
    aCtxImpl.Reassociate( ETrue );

    
    // Store parameters of the new BSS to connect to
    //
    (aCtxImpl.GetBssId())= aBSSID;   
    (aCtxImpl.GetSsId()) = aSSID;   
    (aCtxImpl.EncryptionStatus()) = aEncryptionStatus;   
    
    // set values in authentication frame
    //
    (aCtxImpl.AuthenticationAlgorithmNumber()) = aAuthAlgorithmNbr;
    // set the BSSID field
    (aCtxImpl.GetAuthenticationFrame()).iHeader.iBSSID = aBSSID;
    (aCtxImpl.GetHtAuthenticationFrame()).iHeader.iBSSID = aBSSID;
    // set the DA field
    (aCtxImpl.GetAuthenticationFrame()).iHeader.iDA = aBSSID;
    (aCtxImpl.GetHtAuthenticationFrame()).iHeader.iDA = aBSSID;
    // set the SA field
    (aCtxImpl.GetAuthenticationFrame()).iHeader.iSA 
        = aCtxImpl.iWlanMib.dot11StationId;
    (aCtxImpl.GetHtAuthenticationFrame()).iHeader.iSA 
        = aCtxImpl.iWlanMib.dot11StationId;
    aCtxImpl.NetworkOperationMode( WHA::EBSS );
    
    if ( aCtxImpl.DisassociatedByAp() )
        {
        // if the AP has already sent us a disassociation or deauthentication
        // frame, we won't send it a disassociation frame any more. So in this
        // case we skip the dot11DisassociationPending state and go directly to 
        // dot11Synchronize state.        

        aCtxImpl.DisassociatedByAp( EFalse ); // also reset the flag
        
        ChangeState( aCtxImpl, 
            *this,                                          // prev state
            aCtxImpl.iStates.iSynchronizeState              // next state
            );        
        }
    else
        {
        // the most common case, i.e. we are still associated with the current
        // AP. So make a state change to dot11DisassociationPending
        ChangeState( aCtxImpl, 
            *this,                                          // prev state
            aCtxImpl.iStates.iDisassociationPendingState    // next state
            );
        }

    // signal caller that state transition occurred
    return ETrue;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanDot11InfrastructureMode::SetRcpiTriggerLevel(
    WlanContextImpl& aCtxImpl,
    TUint32 aRcpiTrigger)
    {
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanDot11InfrastructureMode::SetRcpiTriggerLevel: aRcpiTrigger: %d"), 
        aRcpiTrigger );
 
    // update the MIB. Also request the WLAN mgmt client request
    // to be completed
    return SetRcpiTriggerLevelMib(aCtxImpl, aRcpiTrigger, ETrue );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11InfrastructureMode::SetRcpiTriggerLevelMib(
    WlanContextImpl& aCtxImpl,
    TUint32 aRcpiTrigger,
    TBool aCompleteManagementRequest )
    {
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanDot11InfrastructureMode::SetRcpiTriggerLevelMib: aRcpiTrigger: %d"), 
        aRcpiTrigger );
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanDot11InfrastructureMode::SetRcpiTriggerLevelMib: aCompleteManagementRequest: %d"), 
        aCompleteManagementRequest );

    // allocate memory for the mib to write
    WHA::SrcpiThreshold* mib = static_cast<WHA::SrcpiThreshold*>
        (os_alloc( sizeof(WHA::SrcpiThreshold) )); 

    if ( !mib )
        {
        // allocation failed
        // simulate macnotresponding error
        OsTracePrint( KWarningLevel, (TUint8*)
            ("UMAC: WlanDot11InfrastructureMode::SetRcpiTriggerLevelMib: abort") );
        return DoErrorIndication( aCtxImpl, WHA::KErrorMacNotResponding );
        }

    mib->iThreshold = aRcpiTrigger;

    WlanWsaWriteMib& wha_cmd = aCtxImpl.WsaWriteMib();
        
    wha_cmd.Set( aCtxImpl, WHA::KMibRcpiThreshold, sizeof(*mib), mib );

    const TUint32 KNoNeedToCompleteManagementRequest = 0;
        
    // change global state: entry procedure triggers action
    ChangeState( aCtxImpl, 
        *this,              // prev state
        wha_cmd,            // next state
        aCompleteManagementRequest ? KCompleteManagementRequest : 
                                     KNoNeedToCompleteManagementRequest
        );   

    os_free( mib ); // release the memory

    // signal caller that a state transition occurred
    return ETrue;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11InfrastructureMode::OnDeauthenticateFrameRx( 
    WlanContextImpl& aCtxImpl,
    TUint8* aBuffer )
    {
    // note that the AP has disassociated us (as the AP deauthenticated us
    // we are also disassociated)
    aCtxImpl.DisassociatedByAp( ETrue );
    aCtxImpl.StopVoiceOverWlanCallMaintenance();
    aCtxImpl.StopKeepAlive();
    OnInDicationEvent( aCtxImpl, EMediaDisconnect );

    // release the Rx buffer
    aCtxImpl.iUmac.MarkRxBufFree( aBuffer );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11InfrastructureMode::OnDisassociateFrameRx( 
    WlanContextImpl& aCtxImpl,
    TUint8* aBuffer )
    {
    // note that the AP has disassociated us
    aCtxImpl.DisassociatedByAp( ETrue );
    aCtxImpl.StopVoiceOverWlanCallMaintenance();
    aCtxImpl.StopKeepAlive();
    OnInDicationEvent( aCtxImpl, EMediaDisconnect );

    // release the Rx buffer
    aCtxImpl.iUmac.MarkRxBufFree( aBuffer );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11InfrastructureMode::OnBeaconFrameRx( 
    WlanContextImpl& aCtxImpl,
    const TAny* aFrame,
    const TUint32 aLength,
    WHA::TRcpi /*aRcpi*/,
    TUint8* aBuffer )
    {
    OsTracePrint( KRxFrame, (TUint8*)
        ("UMAC: WlanDot11InfrastructureMode::OnBeaconFrameRx()"));

    // buffer begin
    const TUint8* ptr = reinterpret_cast<const TUint8*>(aFrame);
    
    // bypass mac header, timestamp fixed field, beacon interval fixed field
    // and capability fixed field
    const TUint8 offset = 
        KMacHeaderLength 
        + KTimeStampFixedFieldLength 
        + KBeaconIntervalFixedFieldLength
        + KCapabilityInformationFixedFieldLength;        
    ptr += offset; // we now point to the beginning of IEs

    if ( aLength > offset )
        {
        //=================================================================
        // Check if any dynamic nw parameters, that we are monitoring, have 
        // changed. If they have, take the new parameters into use
        //=================================================================

        WlanElementLocator elementLocator( ptr, aLength - offset );
        
        ValidateErpParams( aCtxImpl, elementLocator );

        if ( aCtxImpl.QosEnabled() )
            {
            ValidateAcParams( aCtxImpl, elementLocator );
            }
        else
            {
            // this is not a QoS connection => not relevant to check
            // for AC parameter changes
            }

        if ( aCtxImpl.HtSupportedByNw() )
            {
            ValidateHtBssOperationParams( aCtxImpl, elementLocator );
            }
        else
            {
            // this is not a HT connection => not relevant to check
            // for HT operation parameter changes
            }
        }
    else
        {
        // frame too short to contain any IEs => ignore it
        OsTracePrint( KRxFrame | KWarningLevel, (TUint8*)
            ("UMAC: WlanDot11InfrastructureMode::OnBeaconFrameRx: WARNING: frame too short to contain any IEs => ignored") );        
        }

    // release the Rx buffer
    aCtxImpl.iUmac.MarkRxBufFree( aBuffer);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11InfrastructureMode::ValidateErpParams( 
    WlanContextImpl& aCtxImpl,
    WlanElementLocator& aElementLocator )
    {
    TUint8 length( 0 );
    const TUint8* elementData( NULL );

    if ( WlanElementLocator::EWlanLocateOk == 
         aElementLocator.InformationElement( 
            E802Dot11ErpInformationIE, 
            length, 
            &elementData ) )
        {
        // ERP IE found
        OsTracePrint( KInfoLevel, (TUint8*)
            ("UMAC: WlanDot11InfrastructureMode::ValidateErpParams(): ERP IE present"));
            
        if ( ( *elementData & KUseProtectionMask ) != aCtxImpl.ProtectionBitSet() )            
            {
            OsTracePrint( KInfoLevel, (TUint8*)
                ("UMAC: WlanDot11InfrastructureMode::ValidateErpParams(): use protection setting changed, is now: %d"),
                *elementData & KUseProtectionMask );

            // use protection setting has changed, update the setting & 
            // re-set the MIB
            //
            aCtxImpl.ProtectionBitSet( *elementData & KUseProtectionMask );

            if ( !(aCtxImpl.WsaCmdActive()) )
                {
                SetCtsToSelfMib( aCtxImpl );
                }
            else
                {
                // WHA command is in progress so we must defer this access
                aCtxImpl.RegisterEvent( KSetCtsToSelf );
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11InfrastructureMode::ValidateAcParams( 
    WlanContextImpl& aCtxImpl,
    WlanElementLocator& aElementLocator )
    {
    TUint8 length( 0 );
    const TUint8* elementData( NULL );

    // is WMM Parameter Element present                        
    if ( aElementLocator.InformationElement( 
            E802Dot11VendorSpecificIE,
            KWmmElemOui,
            KWmmElemOuiType,
            KWmmParamElemOuiSubtype,
            length, 
            &elementData ) 
        == WlanElementLocator::EWlanLocateOk )    
        {
        // element found
        OsTracePrint( KQos, (TUint8*)
            ("UMAC: WlanDot11InfrastructureMode::ValidateAcParams(): WMM param elem present"));
        
        if ( (reinterpret_cast<const SWmmParamElemData*>(elementData))->ParameterSetCount() 
            != aCtxImpl.WmmParameterSetCount() )
            {
            // AC parameters have changed => parse them again
            //
            OsTracePrint( KQos, (TUint8*)
                ("UMAC: WlanDot11InfrastructureMode::ValidateAcParams(): WMM param set count changed (value: %d) => params changed"),
                (reinterpret_cast<const SWmmParamElemData*>(elementData))->ParameterSetCount() );

            ParseAcParameters( aCtxImpl,
                reinterpret_cast<const SWmmParamElemData&>(*elementData ) );

            // we also need to configure the ACs again
            //
            if ( !(aCtxImpl.WsaCmdActive()) )
                {
                ConfigureAcParams( aCtxImpl );
                }
            else
                {
                // WHA command is in progress so we must defer this access
                aCtxImpl.RegisterEvent( KAcParamUpdate );
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11InfrastructureMode::ValidateHtBssOperationParams( 
    WlanContextImpl& aCtxImpl,
    WlanElementLocator& aElementLocator )
    {
    TUint8 length( 0 );
    const TUint8* elementData( NULL );

    if ( WlanElementLocator::EWlanLocateOk == 
         aElementLocator.InformationElement( 
            E802Dot11HtOperationIE, 
            length, 
            &elementData ) )
        {
        // HT Operation element found
        OsTracePrint( KInfoLevel, (TUint8*)
            ("UMAC: WlanDot11InfrastructureMode::ValidateHtBssOperationParams: element present"));
            
        if ( os_memcmp( 
                 elementData, 
                 &(aCtxImpl.GetNwHtOperationIe().iData), 
                 sizeof( SHtOperationIeData ) ) )            
            {
            // content of the element has changed 
            OsTracePrint( KInfoLevel, (TUint8*)
                ("UMAC: element changed"));

            // store the changed element content to our context
            aCtxImpl.GetNwHtOperationIe().SetIeData( 
                elementData, 
                length );
            
            // inform the lower layers about the new HT operation 
            // configuration 
            if ( !(aCtxImpl.WsaCmdActive()) )
                {
                ConfigureHtBssOperation( aCtxImpl );
                }
            else
                {
                // WHA command is in progress so we must defer this access
                aCtxImpl.RegisterEvent( KSetHtBssOperation );
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11InfrastructureMode::DoSetTxMpduDaAddress( 
    SDataFrameHeader& aDataFrameHeader, 
    const TMacAddress& aMac ) const
    {
    aDataFrameHeader.iAddress3 = aMac;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanDot11InfrastructureMode::DoIsRxFrameSAourAddress( 
    WlanContextImpl& aCtxImpl,
    const SDataFrameHeader& aFrameHeader,
    const SAmsduSubframeHeader* aSubFrameHeader ) const
    {
    if ( aSubFrameHeader )
        {
        // the MSDU is part of an A-MSDU and the caller wants to use the
        // SA in the subframe header for this check
        return aSubFrameHeader->iSa == aCtxImpl.iWlanMib.dot11StationId;
        }
    else
        {
        // the caller wants to use the SA in the MAC header for this check
        return aFrameHeader.iAddress3 == aCtxImpl.iWlanMib.dot11StationId;
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanDot11InfrastructureMode::DoIsValidAddressBitCombination(
    const SDataFrameHeader& aFrameHeader ) const
    {
    return (aFrameHeader.IsFromDsBitSet() && !(aFrameHeader.IsToDsBitSet()));
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11InfrastructureMode::DoBuildEthernetFrame(
    TDataBuffer& aBuffer,
    const SDataMpduHeader& aDot11DataMpdu,
    const TUint8* aStartOfEtherPayload,
    TUint aEtherPayloadLength,
    TBool aAmsdu,
    TUint8* aCopyBuffer )
    {
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanDot11InfrastructureMode::DoBuildEthernetFrame"));

    TUint8* realEtherPayloadStart ( const_cast<TUint8*>(aStartOfEtherPayload) );
    
    if ( aCopyBuffer )
        {
        // the frame needs to be copied to the copy buffer 

        // update to point to the new location
        realEtherPayloadStart = aCopyBuffer + ( 2 * sizeof( TMacAddress ) );
        
        os_memcpy( 
            realEtherPayloadStart,
            aStartOfEtherPayload, 
            aEtherPayloadLength );
        }

    if ( aAmsdu )
        {
        // this MSDU is part of an A-MSDU. Assign SA and DA from subframe
        // header
        const SAmsduSubframeHeader* KSubframeHeader ( 
            reinterpret_cast<const SAmsduSubframeHeader*>( 
                aStartOfEtherPayload 
                - sizeof( SSnapHeader ) 
                - sizeof( SAmsduSubframeHeader ) ) );

        // copy SA to the correct location.
        // We do that first so that it doesn't get overwritten by DA
        os_memcpy( 
            const_cast<TUint8*>(realEtherPayloadStart) - sizeof( TMacAddress ),
            reinterpret_cast<const TUint8*>(KSubframeHeader->iSa.iMacAddress),
            sizeof( TMacAddress ) );
    
        // copy DA to the correct location.
        os_memcpy( 
            const_cast<TUint8*>(realEtherPayloadStart) 
                - ( 2 * sizeof( TMacAddress ) ), 
            reinterpret_cast<const TUint8*>(KSubframeHeader->iDa.iMacAddress ),
            sizeof( TMacAddress ) );
        
        }
    else
        {
        // assign SA and DA from MAC header 
        
        // copy SA to the correct location.
        // We do that first so that it doesn't get overwritten by DA
        os_memcpy( 
            const_cast<TUint8*>(realEtherPayloadStart) - sizeof( TMacAddress ),
            reinterpret_cast<const TUint8*>(
                aDot11DataMpdu.iHdr.iAddress3.iMacAddress ),
            sizeof( TMacAddress ) );
    
        // copy DA to the correct location.
        os_memcpy( 
            const_cast<TUint8*>(realEtherPayloadStart) 
                - ( 2 * sizeof( TMacAddress ) ), 
            reinterpret_cast<const TUint8*>(
                aDot11DataMpdu.iHdr.iAddress1.iMacAddress ),
            sizeof( TMacAddress ) );
        }
    
    // set the length
    aBuffer.KeSetLength( 
        aEtherPayloadLength 
        + ( sizeof( TMacAddress ) * 2 ) );

    // set the frame type
    aBuffer.FrameType( TDataBuffer::KEthernetFrame );
    
    // set the offset to the beginning of the ready ethernet frame 
    // from the beginning of the Rx buf
    aBuffer.KeSetOffsetToFrameBeginning( 
        realEtherPayloadStart - ( 2 * sizeof( TMacAddress ) ) // frame beginning
        - aBuffer.KeGetBufferStart() );                      // buffer beginning

    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanDot11InfrastructureMode::DoBuildEthernetFrame: offset to frame beginning: %d"),
        aBuffer.KeOffsetToFrameBeginning());
    }
    
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11InfrastructureMode::DoConsecutivePwrModeSetFailuresIndication( 
    WlanContextImpl& aCtxImpl )
    {
    if ( aCtxImpl.OnConsecutivePwrModeSetFailures() )
        {
        OnInDicationEvent( aCtxImpl, EConsecutivePwrModeSetFailures );
        }
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanDot11InfrastructureMode::Disconnect( WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( KInfoLevel, (TUint8*)("UMAC: WlanDot11InfrastructureMode::Disconnect():"));

    // set completion code to idle state
    // as it does the request completion

    aCtxImpl.iStates.iIdleState.Set( KErrNone );
    ChangeState( aCtxImpl, 
        *this,                                  // prev state
        aCtxImpl.iStates.iDeauthPendingState    // next state
        );                      

    // signal caller that state transition occurred
    return ETrue;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanDot11InfrastructureMode::RealScan(
    WlanContextImpl& aCtxImpl,
    TScanMode aMode,                    
    const TSSID& aSSID,                 
    TUint32 aScanRate,                    
    SChannels& aChannels,
    TUint32 aMinChannelTime,            
    TUint32 aMaxChannelTime,
    TBool aSplitScan )
    {
    // scanning mode requested
    // set parameters
    // NOTE: OID command parameters are guaranteed to be valid
    // to the point a correcponding completion method is called

    aCtxImpl.iStates.iInfrastructureScanningMode.Set( 
        aMode, aSSID, aScanRate, aChannels,
        aMinChannelTime, aMaxChannelTime, aSplitScan );

    ChangeState( aCtxImpl, 
        *this,                                          // prev state
        aCtxImpl.iStates.iInfrastructureScanningMode    // next state
        );                      

    return ETrue; // global statemachine transition will occur 
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11InfrastructureMode::TxMgmtData( 
    WlanContextImpl& aCtxImpl,
    TDataBuffer& aDataBuffer )
    {
    TWlanUserTxDataCntx& data_cntx( aCtxImpl.GetMgmtTxDataCntx() );

    // make a note of the frame type
    const TDataBuffer::TFrameType frameType( aDataBuffer.FrameType() );

    TUint16 etherType( 0 ); // initial value: not relevant
    
    // construct a dot11 frame from databuffer to storage
    EncapsulateFrame( aCtxImpl, data_cntx, aDataBuffer, etherType );
    
    // start of dot11 frame to send
    const TUint8* start_of_frame( data_cntx.StartOfFrame() );
        
    // select correct tx queue
    const WHA::TQueueId queue_id( 
        QueueId( aCtxImpl, start_of_frame ) );

    T802Dot11FrameControlTypeMask dot11FrameType ( E802Dot11FrameTypeDataEapol );
    TBool useSpecialTxAutoRatePolicy( EFalse );
    
    switch ( frameType )
        {
        case TDataBuffer::KEthernetFrame:
            // dot11FrameType already correct
            
            // request special Tx autorate policy use for EAPOL and WAI frames
            useSpecialTxAutoRatePolicy = ETrue;
            break;
        case TDataBuffer::KDot11Frame:
            dot11FrameType = E802Dot11FrameTypeManagementAction;
            break;
        case TDataBuffer::KSnapFrame:
            // these frames have the same Tx completion handling as Eapol 
            // frames so we use the same dot11FrameType value  - which is
            // already correct - for them, too
            break;
        case TDataBuffer::KEthernetTestFrame:
            dot11FrameType = E802Dot11FrameTypeTestFrame;
            break;
        default:
            // programming error
            OsTracePrint( KErrorLevel, (TUint8*)
                ("UMAC: unsupported frame type: %d"), frameType );
            OsAssert( (TUint8*)("UMAC: panic"), 
                (TUint8*)(WLAN_FILE), __LINE__ );            
        }
    
    // push the frame to packet scheduler for transmission
    aCtxImpl.PushPacketToPacketScheduler(
        start_of_frame,
        data_cntx.LengthOfFrame(),
        queue_id,
        dot11FrameType,
        &aDataBuffer,
        EFalse,
        EFalse,
        useSpecialTxAutoRatePolicy );
            
    // now just wait for the scheduler to call completion methods

    if ( frameType == TDataBuffer::KEthernetFrame )
        {
        // check if we need to change power mgmt mode because of the frame Tx
    
        const TPowerMgmtModeChange KPowerMgmtModeChange ( 
            aCtxImpl.OnFrameTx( queue_id, etherType ) );
        
        // if any change change is needed regarding our power mgmt mode,
        // proceed with it
        PowerMgmtModeChange( aCtxImpl, KPowerMgmtModeChange );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11InfrastructureMode::DoRcpiIndication( 
    WlanContextImpl& aCtxImpl,
    WHA::TRcpi aRcpi )
    {
    OsTracePrint( KWlmIndication, (TUint8*)
        ("UMAC: WlanDot11InfrastructureMode::DoRcpiIndication: rcpi: %d"),
        aRcpi ); 
        
    OnInDicationEvent( aCtxImpl, ERcpiTrigger );
    
    // restore the MIB back to its default value, i.e. zero. 
    // This means that no further RCPI indications should arrive from WHA 
    // layer until the RCPI threshold is set again by WLAN Mgmt Client
    if ( !(aCtxImpl.WsaCmdActive()) )
        {
        SetRcpiTriggerLevelMib( aCtxImpl, WHA::KRcpiThresholdDefault, EFalse );
        }
    else
        {
        // WHA command is in progress so we must defer this access
        aCtxImpl.RegisterEvent( KSetRcpiTriggerLevel );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanDot11InfrastructureMode::OnVoiceCallEntryTimerTimeout( 
    WlanContextImpl& aCtxImpl )
    {
    aCtxImpl.OnVoiceCallEntryTimerTimeout();

    return EFalse;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanDot11InfrastructureMode::OnNullTimerTimeout( 
    WlanContextImpl& aCtxImpl )
    {
    aCtxImpl.OnNullTimerTimeout();

    return EFalse;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanDot11InfrastructureMode::OnNoVoiceTimerTimeout( 
    WlanContextImpl& aCtxImpl )
    {
    aCtxImpl.OnNoVoiceTimerTimeout();

    return EFalse;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanDot11InfrastructureMode::OnKeepAliveTimerTimeout( 
    WlanContextImpl& aCtxImpl )
    {
    aCtxImpl.OnKeepAliveTimerTimeout();

    return EFalse;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanDot11InfrastructureMode::OnWlanWakeUpIntervalChange( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( KPwrStateTransition, (TUint8*)
        ("UMAC: : WlanDot11InfrastructureMode::OnWlanWakeUpIntervalChange"));

    WHA::SwlanWakeUpInterval* mib 
        = static_cast<WHA::SwlanWakeUpInterval*>
        (os_alloc( sizeof( WHA::SwlanWakeUpInterval ) )); 

    if ( !mib )
        {
        // allocation failed
        // simulate macnotresponding error
        OnOidComplete( aCtxImpl );  // complete also
        OsTracePrint( KWarningLevel, 
            (TUint8*)("UMAC: WlanDot11InfrastructureMode::OnWlanWakeUpIntervalChange: alloc failed, abort") );
        return DoErrorIndication( aCtxImpl, WHA::KErrorMacNotResponding );
        }

    // determine the desired new wake-up setting
    const TDot11PsModeWakeupSetting KDesiredPsModeConfig (
        aCtxImpl.DesiredPsModeConfig() );
    
    // take it into use

    mib->iMode = KDesiredPsModeConfig.iWakeupMode;
    mib->iListenInterval = KDesiredPsModeConfig.iListenInterval;
    
    WlanWsaWriteMib& wha_cmd = aCtxImpl.WsaWriteMib();

    wha_cmd.Set( 
        aCtxImpl, WHA::KMibWlanWakeUpInterval, sizeof(*mib), mib );

    OsTracePrint( KPwrStateTransition, 
        (TUint8*)("UMAC: WlanDot11InfrastructureMode::OnWlanWakeUpIntervalChange: desired mode: %d"), 
        mib->iMode );
    OsTracePrint( KPwrStateTransition, (TUint8*)
        ("UMAC: WlanDot11InfrastructureMode::OnWlanWakeUpIntervalChange: desired listen interval: %d"), 
        mib->iListenInterval );

    // store the new setting also locally
    aCtxImpl.iWlanMib.iWlanWakeupInterval = mib->iMode;
    aCtxImpl.iWlanMib.iWlanListenInterval = mib->iListenInterval;

    // change global state: entry procedure triggers action
    ChangeState( aCtxImpl, 
        *this,              // prev state
        wha_cmd,            // next state
        // the ACT signals that this operation should be completed to user 
        WlanDot11State::KCompleteManagementRequest
        );           

    // as the parameters have been supplied we can now deallocate
    os_free( mib );      
    
    // signal state transition change
    return ETrue;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanDot11InfrastructureMode::TxNullDataFrame( 
    WlanContextImpl& aCtxImpl,
    TBool aQosNull )
    {
    OsTracePrint( KWsaTxDetails, (TUint8*)
        ("UMAC: WlanDot11InfrastructureMode::TxNullDataFrame: aQosNull: %d"),
        aQosNull );

    TBool status ( ETrue );
    
    TUint32 lengthOfFrame( 0 );
    T802Dot11FrameControlTypeMask frameType( E802Dot11FrameTypeDataNull );
    WHA::TQueueId queue_id( WHA::ELegacy );

    // copy Null Data frame to tx-buffer to correct offset
    // client doesn't need to take care of the tx buffer header space
    // as the method below does it by itself
    TUint8* start_of_frame = aCtxImpl.TxBuffer();

    if ( start_of_frame )
        {
        if ( aQosNull )
            {
            // transmit of QoS Null Data Frame requested
            
            lengthOfFrame = aCtxImpl.QosNullDataFrameLength();
            frameType = E802Dot11FrameTypeQosDataNull;
            
            os_memcpy( 
                start_of_frame,
                &(aCtxImpl.QosNullDataFrame()), 
                lengthOfFrame );

            // as this needs to be a trigger frame for Voice AC, set the User
            // Priority of the frame accordingly
                
            const TUint8 K802Dot1dPriorityVoice = 6;
            SQosDataFrameHeader* qosHdr = 
                reinterpret_cast<SQosDataFrameHeader*>(start_of_frame);
            qosHdr->SetUserPriority( K802Dot1dPriorityVoice );
            
            // note that the Order bit of the Frame Control field of the QoS 
            // Null data frame has already been given the correct value in 
            // WlanMacActionState::OnDot11InfrastructureModeStateSpaceEntry()
            // method
            }
        else
            {
            // transmit of Null Data Frame requested
            
            lengthOfFrame = sizeof( SNullDataFrame );
            // frame type is already correct for this case
            
            os_memcpy( 
                start_of_frame,
                &(aCtxImpl.NullDataFrame()), 
                lengthOfFrame );
            }

        // determine Tx queue
        queue_id = QueueId( aCtxImpl, start_of_frame );
        
        // send the Null Data frame by pushing it to packet scheduler
        aCtxImpl.PushPacketToPacketScheduler( 
                start_of_frame, 
                lengthOfFrame, 
                queue_id,
                frameType,
                NULL,
                EFalse,
                EFalse,
                ETrue );
        }                
    else
        {
        // we didn't get a Tx buffer so we can't submit a frame send request.
        status = EFalse;
        }

    TUint16 KetherType( 0 ); // N/A in this case
    // check if we need to change power mgmt mode; no matter whether we 
    // actually managed to send a frame or not
    const TPowerMgmtModeChange KPowerMgmtModeChange ( 
        aCtxImpl.OnFrameTx( queue_id, KetherType, frameType ) );
    
    // if any change is needed regarding our power mgmt mode,
    // proceed with it
    PowerMgmtModeChange( aCtxImpl, KPowerMgmtModeChange );
    
    return status;                
    }
