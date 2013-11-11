/*
* Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of the WlanDot11Associated class
*
*/

/*
* %version: 99 %
*/

#include "config.h"
#include "UmacDot11Associated.h"
#include "UmacContextImpl.h"
#include "UmacWsaWriteMib.h"
#include "umacinternaldefinitions.h"

/**
* Mask to determine the used encryption from WHA::ReceivePacket's aFlags 
* parameter
*/
const TUint32 KReceivePacketEncryptionMask = 0x00038000;


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11Associated::OnBeaconFrameRx( 
    WlanContextImpl& aCtxImpl,
    const TAny* /*aFrame*/,
    const TUint32 /*aLength*/,
    WHA::TRcpi /*aRcpi*/,
    TUint8* aBuffer )
    {
    // release the Rx buffer
    aCtxImpl.iUmac.MarkRxBufFree( aBuffer );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11Associated::OnProbeResponseFrameRx( 
    WlanContextImpl& aCtxImpl,
    const TAny* /*aFrame*/,
    const TUint32 /*aLength*/,
    WHA::TRcpi /*aRcpi*/,
    TUint8* aBuffer )
    {
    // release the Rx buffer
    aCtxImpl.iUmac.MarkRxBufFree( aBuffer );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11Associated::OnDeauthenticateFrameRx( 
    WlanContextImpl& aCtxImpl,
    TUint8* aBuffer )
    {
    // release the Rx buffer
    aCtxImpl.iUmac.MarkRxBufFree( aBuffer );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11Associated::OnDisassociateFrameRx( 
    WlanContextImpl& aCtxImpl,
    TUint8* aBuffer )
    {
    // release the Rx buffer
    aCtxImpl.iUmac.MarkRxBufFree( aBuffer );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TSnapStatus WlanDot11Associated::ValiDateSnapHeader( 
    WlanContextImpl& aCtxImpl,
    const TUint8* aStartOfSnap ) const
    {
    TSnapStatus snapStatus( ESnapUnknown );

    if ( !os_memcmp( 
            aStartOfSnap, 
            &KEncapsulatingRfc1042SnapHeader, 
            sizeof( KEncapsulatingRfc1042SnapHeader ) ) )
        {
        snapStatus = ESnapDot11Ok;
        }
    else if ( !os_memcmp( 
                aStartOfSnap, 
                &KEncapsulating802_1hSnapHeader, 
                sizeof( KEncapsulating802_1hSnapHeader ) ) )
        {
        snapStatus = ESnapDot11Ok;
        }
    else
        {
        // unknown SNAP. Status already correct. No action needed
        }

    if ( snapStatus == ESnapUnknown &&
         aCtxImpl.NetworkOperationMode() == WHA::EBSS &&
         aCtxImpl.GetProprietarySnapHeader() != KUndefinedSnapHeader )
        {
        // SNAP still unknown, but we a are in a infrastructure network and
        // a proprietary SNAP header has been defined.
        // Check if the SNAP is this proprietary SNAP header
        
        if ( !os_memcmp( 
                aStartOfSnap, 
                &(aCtxImpl.GetProprietarySnapHeader()), 
                sizeof( aCtxImpl.GetProprietarySnapHeader() ) ) )
            {
            snapStatus = ESnapProprietaryOk;
            }        
        }
    
    return snapStatus;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TBool WlanDot11Associated::RxDataMpduValid(
    const TUint32 aLength,
    const SDataFrameHeader& aFrameHeader,
    TBool aQosData,
    TBool aAmsdu,
    TUint aHtControlLen,
    TUint aSecurityHeaderLen,
    TUint aSecurityTrailerLen ) const
    {
    //========================================================================
    // validate frame length
    //========================================================================
    
    const TUint KMinAcceptableRxDataMpduLen ( 
        MinAcceptableRxDataMpduLen( 
            aQosData,
            aAmsdu,
            aHtControlLen,
            aSecurityHeaderLen,
            aSecurityTrailerLen ) );
    
    if ( aLength < KMinAcceptableRxDataMpduLen )
        {
        // frame shorter than min acceptable length; reject it 
        OsTracePrint( KRxFrame | KWarningLevel, (TUint8*)
            ("UMAC: WlanDot11Associated::RxDataMpduValid: WARNING: MPDU rejected because its shorter than min acceptable length of %d"),
            KMinAcceptableRxDataMpduLen );        

        return EFalse;
        }
    
    //========================================================================
    // validate To DS & From DS bits
    //========================================================================

    if ( !DoIsValidAddressBitCombination( aFrameHeader ) )
        {
        OsTracePrint( KRxFrame, (TUint8*)
            ("UMAC: WlanDot11Associated::RxDataMpduValid: MPDU rejected because of invalid To DS and/or From DS bit values") );

        return EFalse;
        }
            
    return ETrue;
    }

// ---------------------------------------------------------------------------
// defines the minimum acceptable Data MPDU length as:
// dot11 MAC header length including SNAP and possibly existing HT Control 
// field
// + security header length
// + subframe header length if the QoS Data MPDU contains an A-MSDU
// + security trailer length
// + Ethernet type field length
// ---------------------------------------------------------------------------
//
inline TUint WlanDot11Associated::MinAcceptableRxDataMpduLen(
    TBool aQosData,
    TBool aAmsdu,
    TUint aHtControlLen,
    TUint aSecurityHeaderLen,
    TUint aSecurityTrailerLen ) const
    {
    return aQosData ? 
        sizeof( SQosDataMpduHeader ) + aHtControlLen
        + aSecurityHeaderLen
        + ( aAmsdu ? sizeof( SAmsduSubframeHeader ) : 0 )
        + aSecurityTrailerLen
        + sizeof( SEthernetType )
        :         
        sizeof( SDataMpduHeader )
        + aSecurityHeaderLen
        + aSecurityTrailerLen
        + sizeof( SEthernetType );
    }

// ---------------------------------------------------------------------------
// Note that address 1 is the DA (Destination Address) both in infrastructure
// and IBSS network Rx frames
// ---------------------------------------------------------------------------
//
inline TDaType WlanDot11Associated::RxFrameDaType(
    const SDataFrameHeader& aFrameHeader ) const
    {
    if ( IsGroupBitSet( aFrameHeader.iAddress1 ) )
        {
        // a multicast
        if ( aFrameHeader.iAddress1 == KBroadcastMacAddr )
            {
            // also a brodcast
            return EBroadcastAddress;
            }
        else
            {
            // a multicast but not a broadcast
            return EMulticastAddress;
            }
        }
    else
        {
        // a unicast
        return EUnicastAddress;
        }
    }

// ---------------------------------------------------------------------------
//  
// ---------------------------------------------------------------------------
//
inline TBool WlanDot11Associated::RxMsduValid(
    WlanContextImpl& aCtxImpl,
    const SDataFrameHeader& aFrameHeader,
    const SAmsduSubframeHeader* aSubFrameHeader,
    const TUint8* aStartOfSnap,
    TUint16 aEtherType,
    TBool aMulticast,
    TUint32 aFlags,
    TSnapStatus& aSnapStatus ) const
    {
    //========================================================================
    // validate SNAP header
    //========================================================================
    
    aSnapStatus = ValiDateSnapHeader( aCtxImpl, aStartOfSnap );
    
    if ( aSnapStatus == ESnapUnknown )
        {
        // SNAP header is invalid
        
        OsTracePrint( KRxFrame, (TUint8*)
            ("UMAC: WlanDot11Associated::RxMsduValid: MSDU rejected because of invalid snap:"),
            aStartOfSnap, aStartOfSnap + sizeof( SSnapHeader ) );    

        return EFalse;
        }

    // determine if SA is our MAC address
    const TBool KSaIsOurMac ( 
        DoIsRxFrameSAourAddress( aCtxImpl, aFrameHeader, aSubFrameHeader ) );           

    //========================================================================
    // if this is a proprietary test MSDU but not sent by us, its not valid 
    // for us
    //========================================================================

    if ( aEtherType == KBounceType && !KSaIsOurMac )
        {
#ifndef NDEBUG
        OsTracePrint( KRxFrame, (TUint8*)
            ("UMAC: WlanDot11Associated::RxMsduValid: proprietary test MSDU, but not sent by us. MSDU rejected") );

        if ( aSubFrameHeader )
            {
            OsTracePrint( KRxFrame, (TUint8*)
                ("UMAC: Subframe DA:"), aSubFrameHeader->iDa );                
            OsTracePrint( KRxFrame, (TUint8*)
                ("UMAC: Subframe SA:"), aSubFrameHeader->iSa );                
            }
        else
            {
            OsTracePrint( KRxFrame, (TUint8*)
                ("UMAC: Address1:"), aFrameHeader.iAddress1);    
            OsTracePrint( KRxFrame, (TUint8*)
                ("UMAC: Address2:"), aFrameHeader.iAddress2);    
            OsTracePrint( KRxFrame, (TUint8*)
                ("UMAC: Address3:"), aFrameHeader.iAddress3);
            }
#endif        

        return EFalse;
        }

    //========================================================================
    // non-test multicast MSDUs sent by us are not relevant for us
    //========================================================================

    if ( // SA is our MAC address
         KSaIsOurMac
         // AND this is a multicast frame
         && aMulticast
         // AND this is not a test frame
         && aEtherType != KBounceType )
        {
        // we have received a non-test multicast frame that was sent by us.
        // Ignore it.
        
#ifndef NDEBUG
        OsTracePrint( KRxFrame, (TUint8*)
            ("UMAC: WlanDot11Associated::RxMsduValid: MSDU rejected because its a non-test multicast MSDU sent by us"));
        
        if ( aSubFrameHeader )
            {
            OsTracePrint( KRxFrame, (TUint8*)
                ("UMAC: Subframe DA:"), aSubFrameHeader->iDa );                
            OsTracePrint( KRxFrame, (TUint8*)
                ("UMAC: Subframe SA:"), aSubFrameHeader->iSa );                
            }
        else
            {
            OsTracePrint( KRxFrame, (TUint8*)
                ("UMAC: Address1:"), aFrameHeader.iAddress1);    
            OsTracePrint( KRxFrame, (TUint8*)
                ("UMAC: Address2:"), aFrameHeader.iAddress2);    
            OsTracePrint( KRxFrame, (TUint8*)
                ("UMAC: Address3:"), aFrameHeader.iAddress3);
            }
#endif

        return EFalse;
        }
        
    // determine the encryption of the frame
    const TUint32 KEncryption ( aFlags & KReceivePacketEncryptionMask );

    if ( aSnapStatus != ESnapProprietaryOk )
        {
        //====================================================================
        // validate MSDU with the applicable privacy mode filter
        //====================================================================
        if ( !aCtxImpl.ExecuteActivePrivacyModeFilter(
                aFrameHeader,
                aCtxImpl.iEnableUserData,
                aEtherType,
                aCtxImpl.PairWiseKeyType() != WHA::EKeyNone,
                ( KEncryption == WHA::KEncryptAes || 
                  KEncryption == WHA::KEncryptTkip ||
                  KEncryption == WHA::KEncryptWapi ) ) )
            {
            return EFalse;
            }
        }

    // this MSDU has passed all our checks, so it is valid for us
    return ETrue;
    }

// ---------------------------------------------------------------------------
//  
// ---------------------------------------------------------------------------
//
inline TUint WlanDot11Associated::RxMsduEthernetPayloadLength(
    const TUint32 aMpduLength, 
    TUint aSubframeLength, 
    TBool aQosData,
    TUint aHtControlLen,
    TUint aSecurityHeaderLen,
    TUint aSecurityTrailerLen ) const
    {
    TUint length ( 0 );
    
    if ( aSubframeLength )
        {
        // this ethernet payload is carried in an A-MSDU subframe
        
        length = aSubframeLength - sizeof( SSnapHeader );

        OsTracePrint( KRxFrame, (TUint8*)
            ("UMAC: subframe eth payload len: %d"),
            length );                        
        }
    else
        {
        // this ethernet payload is carried in a non-A-MSDU frame

        if ( aQosData )
            {
            length = 
                // total length of the Rx frame
                aMpduLength
                // - MAC header length including SNAP
                - ( sizeof( SQosDataMpduHeader ) + aHtControlLen )
                // - security header length
                - aSecurityHeaderLen
                // - security trailer length
                - aSecurityTrailerLen;
                
            OsTracePrint( KRxFrame, (TUint8*)
                ("UMAC: qos data eth payload len: %d"),
                length );                        
            }
        else
            {
            length =
                // total length of the Rx frame
                aMpduLength
                // MAC header length including SNAP
                - sizeof( SDataMpduHeader )
                // - security header length
                - aSecurityHeaderLen
                // - security trailer length
                - aSecurityTrailerLen;
        
            OsTracePrint( KRxFrame, (TUint8*)
                ("UMAC: non-qos data eth payload len: %d"),
                length );                        
            }
        }
    
    return length;
    }

// ---------------------------------------------------------------------------
//   
// ---------------------------------------------------------------------------
//
inline TUint8* WlanDot11Associated::NewBufForMgmtClientRxFrame(
    WlanContextImpl& aCtxImpl,
    TBool aProprieatarySnapFrame,
    TBool aQosFrame,
    TUint aHtControlLen,
    TUint aEtherPayloadLength ) const
    {
    TUint requiredLength ( 0 );
    
    if ( aProprieatarySnapFrame )
        {
        // forwarded as a Data MPDU
        
        requiredLength = aQosFrame ? 
            sizeof( SQosDataFrameHeader ) 
            + aHtControlLen 
            + aEtherPayloadLength :
            sizeof( SDataFrameHeader ) 
            + aEtherPayloadLength;        
        }
    else
        {
        // forwarded as Ethernet frame
        
        requiredLength = 2 * sizeof( TMacAddress ) + aEtherPayloadLength;
        }
    
    // request for a new Rx buffer
    return aCtxImpl.GetRxBuffer( 
        requiredLength, 
        // tell that this is an internally triggered buffer request
        ETrue );
    }
        
// ---------------------------------------------------------------------------
//  
// ---------------------------------------------------------------------------
//
inline TBool WlanDot11Associated::RxMsduForUser(
    TUint16 aEtherType,
    TSnapStatus aSnapstatus ) const
    {
    if ( // not an EAPOL frame AND
         aEtherType != KEapolType &&
         // not a WAI frame AND
         aEtherType != KWaiType &&
         // not a WLAN Mgmt Client test frame
         aEtherType != KBounceType &&
         // not a proprietary SNAP frame
         aSnapstatus != ESnapProprietaryOk )
        {
        // a user data / protocol stack data frame

        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

// ---------------------------------------------------------------------------
//  
// ---------------------------------------------------------------------------
//
inline void WlanDot11Associated::UpdateDataFrameRxStatistics(
    WlanContextImpl& aCtxImpl,
    TUint16 aEtherType,
    TBool aMulticast,
    WHA::TQueueId aAccessCategory ) const
    {
    if ( aEtherType != KBounceType )
        {
        // other than WLAN Mgmt Client test data frame received
        
        if ( aMulticast )
            {
            // multicast frame
    
            OsTracePrint( KRxFrame, (TUint8*)
                ("UMAC: inc rx mcast cnt for AC: %d"),
                aAccessCategory );
    
            // update frame statistics
            aCtxImpl.IncrementRxMulticastDataFrameCount( aAccessCategory );            
            }
        else
            {
            // unicast frame
    
            OsTracePrint( KRxFrame, (TUint8*)
                ("UMAC: inc rx unicast cnt for AC: %d"),
                aAccessCategory );
    
            // update frame statistics
            aCtxImpl.IncrementRxUnicastDataFrameCount( aAccessCategory );
            }
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11Associated::OnDataFrameRx(
    WlanContextImpl& aCtxImpl,
    const TAny* aFrame,
    const TUint32 aLength,
    TUint32 aFlags,
    WHA::TRcpi aRcpi,
    TUint8* aBuffer )
    {
    OsTracePrint( KRxFrame, (TUint8*)
        ("UMAC: WlanDot11Associated::OnDataFrameRx") );    

    SDataMpduHeader* hdr 
        = reinterpret_cast<SDataMpduHeader*>(const_cast<TAny*>(aFrame));
    SQosDataMpduHeader* qoshdr 
        = reinterpret_cast<SQosDataMpduHeader*>(const_cast<TAny*>(aFrame));

    // determine the length of the security header (e.g. IV etc.) ...
    const TUint KSecurityHeaderLen( DecryptHdrOffset( aCtxImpl, aFlags ) );

    OsTracePrint( KRxFrame, (TUint8*)("UMAC: KSecurityHeaderLen: %d"),
        KSecurityHeaderLen );    

    // ... and the security trailer (e.g. MIC etc.)
    const TUint KSecurityTrailerLen( DecryptTrailerOffset( aCtxImpl, aFlags ) );

    OsTracePrint( KRxFrame, (TUint8*)("UMAC: KSecurityTrailerLen: %d"),
        KSecurityTrailerLen );    

    // as the MAC header of a QoS data MPDU has an additional field
    // we need to take that into account; so make a note if we indeed have 
    // received a QoS data MPDU
    const TBool KQosData = 
        ( hdr->iHdr.iFrameControl.iType == E802Dot11FrameTypeQosData ) ? 
        ETrue : EFalse;

    // the MAC header of HT QoS data MPDU also has an additional field,
    // so determine if that field is present and hence has a non-zero length
    const TUint KHtControlLen ( 
        KQosData && HtcFieldPresent( aCtxImpl, aFrame, aFlags ) ? 
            KHtControlFieldLength : 0 );

    // determine if the MPDU contains an A-MSDU
    const TBool KAmsdu ( 
        KQosData && qoshdr->iHdr.AmsduPresent() ? ETrue : EFalse );
    
    if ( !RxDataMpduValid(
            aLength,
            hdr->iHdr,
            KQosData,
            KAmsdu,
            KHtControlLen,
            KSecurityHeaderLen,
            KSecurityTrailerLen ) )
        {
        // the overall MAC MPDU (disregarding the included MSDU(s) at this
        // point) is not valid for us. Release the Rx buffer & abort
        aCtxImpl.iUmac.MarkRxBufFree( aBuffer );                            
        return;        
        }
    
    TUint8* framePtr ( 0 );
    WHA::TQueueId accessCategory( WHA::ELegacy );

    // User Priority is regarded as Best Effort (i.e. Legacy, i.e. zero) unless
    // the received MPDU is a QoS Data MPDU & a different priority is 
    // specified in its MAC header
    TUint8 userPriority( 0 );
    
    if ( KQosData )
        {
        // a QoS Data MPDU
        
        // move framePtr past the MAC header
        framePtr = reinterpret_cast<TUint8 *>(
            reinterpret_cast<SQosDataFrameHeader *>( hdr ) + 1 ) 
            + KHtControlLen;        

        // get the User Priority
        userPriority = qoshdr->iHdr.UserPriority();

        // make a note of the Access Category corresponding to the
        // user priority of the MPDU (note that there's a one to one 
        // mapping between Queue id and Access Category)       
        accessCategory = Queue( userPriority );
        }
    else
        {
        // move framePtr past the MAC header
        framePtr = reinterpret_cast<TUint8 *>(
            reinterpret_cast<SDataFrameHeader *>( hdr ) + 1 );
        }

    // move after possibly existing security header and possibly existing
    // A-MSDU subframe header, i.e. beginning of SNAP header
    framePtr += KAmsdu ? 
                KSecurityHeaderLen + sizeof( SAmsduSubframeHeader ) : 
                KSecurityHeaderLen;
    TUint8* snapLocation ( framePtr );
    
    // determine the DA type
    const TDaType KDaType ( RxFrameDaType( hdr->iHdr ) );

    TSnapStatus snapstatus( ESnapUnknown );
    TDataBuffer* latestValidPacketForUsr ( NULL );
    TUint nbrOfpacketsForUsr ( 0 );
    TUint nbrOfpacketsForMgmtClient ( 0 );
    const TUint KMaxNbrOfPacketsForUsr ( 30 );
    const TUint KMaxNbrOfPacketsForMgmtClient ( 10 );
    TDataBuffer* packetsForUsr[KMaxNbrOfPacketsForUsr];
    TDataBuffer* packetsForMgmtClient[KMaxNbrOfPacketsForMgmtClient];
    // one byte past the last actual payload byte (so excluding the potentially
    // present security trailer) of the MPDU
    const TUint8* const KMpduPayloadEnd ( 
        reinterpret_cast<const TUint8*>(aFrame) 
        + aLength 
        - KSecurityTrailerLen );
    TPowerMgmtModeChange powerMgmtModeChange ( ENoChange );
    
    // handle every MSDU contained in this MPDU. If this is not an A-MSDU
    // there's only a single MSDU in it.
    do
        {
        TUint8* rxBuffer ( aBuffer );
        
        // move after SNAP header to Ethernet type field
        framePtr += sizeof( SSnapHeader );     
        const SEthernetType* const KEtherTypeLocation 
            = reinterpret_cast<const SEthernetType*>( framePtr );
    
        // determine Ethernet type
        const TUint16 KEtherType = KEtherTypeLocation->Type();
    
        OsTracePrint( KRxFrame, (TUint8*)
            ("UMAC: ether type: 0x%04x"), 
            KEtherType );

        // pointer to subframe heaader; or NULL if the MPDU doesn't contain
        // an A-MSDU
        const SAmsduSubframeHeader* KSubframeHdr ( KAmsdu ?  
            ( reinterpret_cast<const SAmsduSubframeHeader*>(
                    snapLocation) - 1 ) : 
            NULL );
        
        // determine if this is a multicast MSDU
        TBool KMulticastMsdu ( EFalse );
        if ( KSubframeHdr )
            {
            KMulticastMsdu = IsGroupBitSet( KSubframeHdr->iDa );
            }
        else
            {
            KMulticastMsdu = ( KDaType == EUnicastAddress ? EFalse : ETrue );
            }

        // determine subframe length, which can be non-zero only if the MPDU
        // contains an A-MSDU
        const TUint KSubframeLen ( KSubframeHdr ?  KSubframeHdr->Length() : 0 );
        
        if ( !RxMsduValid(
                aCtxImpl,
                hdr->iHdr,
                KSubframeHdr,
                snapLocation,
                KEtherType,
                KMulticastMsdu,
                aFlags,
                snapstatus ) )
            {
            // this MSDU is not valid/relevant for us

            if ( KAmsdu )
                {
                // move pointer to the SNAP of the possibly existing next 
                // subframe
                snapLocation += 
                    Align4( sizeof( SAmsduSubframeHeader ) + KSubframeLen );
                // update frame pointer accordingly
                framePtr = snapLocation;

                continue;
                }
            else
                {
                // this MPDU doesn't contain an A-MSDU, so there cannot be
                // more than a single MSDU in it
                break;
                }
            }
            
        // determine if this MSDU is for user / protocol stack
        const TBool KMsduForUser ( RxMsduForUser( KEtherType, snapstatus) );
        
        if ( KMsduForUser && !aCtxImpl.iUmac.ProtocolStackSideClientReady() )
            {
            // this MSDU should be forwarded up the protocol stack but the 
            // protocol stack client is not ready. So we need to skip at
            // least this MSDU. 
            // Move pointer to the SNAP of the possibly existing next subframe
            snapLocation += 
                Align4( sizeof( SAmsduSubframeHeader ) + KSubframeLen );            
            // update frame pointer accordingly
            framePtr = snapLocation;

            OsTracePrint( KRxFrame | KWarningLevel, (TUint8*)
                ("UMAC: WARNING: protocol stack client not ready; MSDU ignored") );

            continue;            
            }

        TDataBuffer* metaHdr ( aCtxImpl.GetRxFrameMetaHeader() );        
        if ( !metaHdr )
            {
            // no memory available for Rx frame meta header. This means that
            // we are not able to forward to higher layers the current MSDU or 
            // any MSDU(s) that possibly follow it in this same MPDU

            OsTracePrint( KRxFrame | KWarningLevel, (TUint8*)
                ("UMAC: WARNING: no memory for Rx frame meta hdr; MSDU ignored") );
            break;
            }        
    
        const TUint KEtherPayloadLen( RxMsduEthernetPayloadLength(
                aLength, 
                KSubframeLen, 
                KQosData,
                KHtControlLen,
                KSecurityHeaderLen,
                KSecurityTrailerLen ) );
    
        if ( !KMsduForUser && KAmsdu )
            {
            // an MSDU for WLAN Mgmt Client which is also a part of an 
            // A-MSDU. In this case we need to allocate a new buffer for
            // this packet and copy it there. This is necessary as an 
            // A-MSDU may contain MSDUs both for the protocol stack side 
            // and for WLAN Mgmt Client. Either one of those clients may 
            // complete the handling of its MSDUs first at which point the
            // Rx buffer(s) for its MSDU(s) is freed. So we need to make 
            // sure that a buffer is not freed while a client is still 
            // handling MSDU(s) contained in it. We do this by always 
            // copying WLAN Mgmt client MSDU(s) part of an A-MSDU to 
            // new buffers.
            
            rxBuffer = NewBufForMgmtClientRxFrame(
                aCtxImpl,
                snapstatus == ESnapProprietaryOk,
                KQosData,
                KHtControlLen,
                snapstatus == ESnapProprietaryOk ? 
                    KSubframeLen : KEtherPayloadLen );
            
            if ( !rxBuffer )
                {
                // allocation failed so we need to skip at least this MSDU. 

                // Move pointer to the SNAP of the possibly existing next 
                // subframe
                snapLocation += 
                    Align4( sizeof( SAmsduSubframeHeader ) + KSubframeLen );                // update frame pointer accordingly
                // update frame pointer accordingly
                framePtr = snapLocation;

                // also release the meta hdr which was planned to be used for
                // this MSDU 
                aCtxImpl.FreeRxFrameMetaHeader( metaHdr );

                OsTracePrint( KRxFrame | KWarningLevel, (TUint8*)
                    ("UMAC: WARNING: new buf alloc for mgmt client frame failed; MSDU ignored") );
                continue;
                }
            }
        
        // set the offset to the beginning of the Rx buffer from the beginning
        // of the meta header. Note that this may be also negative
        metaHdr->KeSetBufferOffset(
            rxBuffer
            - reinterpret_cast<TUint8*>(metaHdr) );
        
        if ( snapstatus == ESnapProprietaryOk )
            {
            HandleProprietarySnapRxFrame( 
                *metaHdr, 
                KQosData,
                aFrame, 
                KSubframeHdr,
                aLength, 
                KSecurityHeaderLen, 
                KSecurityTrailerLen,
                KHtControlLen,
                KAmsdu ? rxBuffer : NULL );
            }
        else
            {
            DoBuildEthernetFrame( 
                *metaHdr, 
                *hdr, 
                // start of ethernet payload (includes ether type field)
                reinterpret_cast<const TUint8*>(KEtherTypeLocation), 
                // length of ethernet payload
                KEtherPayloadLen,
                KAmsdu,
                !KMsduForUser && KAmsdu ? rxBuffer : NULL );
            }
        
        // set the frame's User Priority to the Rx buffer being passed 
        // to the client
        metaHdr->SetUserPriority( userPriority );                    
        // set the RCPI
        metaHdr->KeSetRcpi( aRcpi );
    
        if ( KEtherType == KBounceType )
            {
            metaHdr->FrameType( 
                TDataBuffer::KEthernetTestFrame );
            }

        if ( KMsduForUser )
            {
            // a user data / protocol stack data frame

            OsTracePrint( KRxFrame, (TUint8*)
                ("UMAC: data frame rx"));    

            packetsForUsr[nbrOfpacketsForUsr++] = metaHdr;            
            latestValidPacketForUsr = metaHdr;
            if ( KAmsdu )
                {
                metaHdr->KeSetFlags( TDataBuffer::KDontReleaseBuffer );
                }
            }
        else
            {
            // a WLAN Mgmt Client data frame
            
            OsTracePrint( KRxFrame, (TUint8*)
                ("UMAC: mgmt client frame rx"));    

            packetsForMgmtClient[nbrOfpacketsForMgmtClient++] = metaHdr;
            }

        //  move pointer to the SNAP of the possibly existing next subframe
        snapLocation += Align4( sizeof( SAmsduSubframeHeader ) + KSubframeLen );
        // update frame pointer accordingly
        framePtr = snapLocation;

        UpdateDataFrameRxStatistics(
            aCtxImpl,
            KEtherType,
            KMulticastMsdu,
            accessCategory );        

        // inform dynamic power mode mgr about Rx data frame acceptance        
        if ( // if this is not our test frame AND 
             KEtherType != KBounceType &&
             // need to change power mgmt mode hasn't already been detected
             // based on this received (A-)MSDU
             powerMgmtModeChange == ENoChange )
            {
            powerMgmtModeChange = aCtxImpl.OnFrameRx( 
                accessCategory, 
                KEtherType, 
                KEtherPayloadLen, 
                KDaType );
            }
        
        // inform Null Send Controller about data frame Rx
        aCtxImpl.OnDataRxCompleted( 
            ( KEtherType == KEapolType || KEtherType == KWaiType ) ?
            // as EAPOL and WAI frames are not really Voice data (they
            // are just sometimes sent with Voice priority), handle them 
            // here as Best Effort (Legacy)
            WHA::ELegacy : 
            accessCategory, 
            KEtherPayloadLen );

          // for a non-A-MSDU this loop is executed only once
        } while ( KAmsdu && snapLocation < KMpduPayloadEnd );

    if ( nbrOfpacketsForUsr )
        {
        latestValidPacketForUsr->KeClearFlags( 
            TDataBuffer::KDontReleaseBuffer );

        // complete user data / protocol stack data frame(s)
        if ( !aCtxImpl.iUmac.ProtocolStackDataReceiveComplete( 
                packetsForUsr[0], 
                nbrOfpacketsForUsr ) )
            {
            // there's no protocol stack client to whom to complete Rx packets.
            // (Actually the control should never arrive here as we have 
            // checked the readiness of the protocol stack client already
            // earlier)
            nbrOfpacketsForUsr = 0;
            }
        }

    if ( nbrOfpacketsForMgmtClient )
        {
        // complete WLAN Mgmt Client data frame(s)
        aCtxImpl.iUmac.MgmtDataReceiveComplete( 
            packetsForMgmtClient[0], 
            nbrOfpacketsForMgmtClient );
        }

    if ( ( !nbrOfpacketsForUsr && !nbrOfpacketsForMgmtClient ) ||
         ( KAmsdu && !nbrOfpacketsForUsr ) )
        {
        // the contents of the original Rx buffer are not needed any more,
        // so deallocate it        
        OsTracePrint( KRxFrame, (TUint8*)
            ("UMAC: free original Rx buf"));    
        aCtxImpl.iUmac.MarkRxBufFree( aBuffer );
        }
    
    // if any change is needed regarding our power mgmt mode,
    // proceed with it
    PowerMgmtModeChange( aCtxImpl, powerMgmtModeChange );    
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11Associated::OnManagementActionFrameRx(
    WlanContextImpl& aCtxImpl,
    const TAny* aFrame,
    const TUint32 aLength,
    WHA::TRcpi aRcpi,
    TUint8* aBuffer ) const
    {
    OsTracePrint(  KRxFrame, (TUint8*)
        ("UMAC: WlanDot11Associated::OnManagementActionFrameRx"));    

    TDataBuffer* metaHdr ( aCtxImpl.GetRxFrameMetaHeader() );

    if ( metaHdr )
        {
        // set length and type
        metaHdr->KeSetLength( aLength );
        metaHdr->FrameType( TDataBuffer::KDot11Frame );
        // set RCPI
        metaHdr->KeSetRcpi( aRcpi );
        
        // set the offset to the beginning of the Rx buffer from the beginning
        // of the meta header. Note that this may be also negative
        metaHdr->KeSetBufferOffset(
            aBuffer
            - reinterpret_cast<TUint8*>(metaHdr) );
        
        // set the offset to the beginning of the actual frame within the
        // Rx buffer
        metaHdr->KeSetOffsetToFrameBeginning( 
            reinterpret_cast<const TUint8*>(aFrame)   // frame beginning
            - aBuffer );                              // buffer beginning
        
        // complete
        aCtxImpl.iUmac.MgmtDataReceiveComplete( metaHdr, 1 );
        }
    else
        {
        // no memory available for the meta header. In this case we have no
        // other choice than to discard the received frame. 
        aCtxImpl.iUmac.MarkRxBufFree( aBuffer );
        OsTracePrint( KWarningLevel | KRxFrame, (TUint8*)
            ("UMAC: WlanDot11Associated::OnManagementActionFrameRx: WARNING: no memory for meta hdr => abort rx") );            
        }
    }
        
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TAny* WlanDot11Associated::RequestForBuffer( 
    WlanContextImpl& aCtxImpl,
    TUint16 aLength )
    {
    return aCtxImpl.GetRxBuffer( aLength );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11Associated::ReceivePacket( 
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
    SDataMpduHeader* hdr( 
        reinterpret_cast<SDataMpduHeader*>(const_cast<TAny*>(aFrame)) );

    if ( aStatus == WHA::KSuccess )
        {
        // packet reception success lets see what type of frame we have
        OsTracePrint( KRxFrame, 
            (TUint8*)
            ("UMAC: WlanDot11Associated::ReceivePacket:frame receive success, frame type: 0x%02x"), 
            hdr->iHdr.iFrameControl.iType );

        if ( ( hdr->iHdr.iFrameControl.iType == E802Dot11FrameTypeData )
             || ( hdr->iHdr.iFrameControl.iType == E802Dot11FrameTypeQosData ) )
            {
            OnDataFrameRx( aCtxImpl, aFrame, aLength, aFlags, aRcpi, aBuffer );
            
            if ( aCtxImpl.InsertNewRcpiIntoPredictor( os_systemTime(), aRcpi ) )
                {
                // indicate WLAN signal loss prediction to WLAN Mgmt Client
                OnInDicationEvent( aCtxImpl, ESignalLossPrediction );
                }
            }        
        else if ( hdr->iHdr.iFrameControl.iType 
            == E802Dot11FrameTypeManagementAction )
            {
            OnManagementActionFrameRx( 
                aCtxImpl, 
                aFrame, 
                aLength, 
                aRcpi, 
                aBuffer );

            if ( aCtxImpl.InsertNewRcpiIntoPredictor( os_systemTime(), aRcpi ) )
                {
                // indicate WLAN signal loss prediction to WLAN Mgmt Client
                OnInDicationEvent( aCtxImpl, ESignalLossPrediction );
                }
            }
        else if ( hdr->iHdr.iFrameControl.iType 
            == E802Dot11FrameTypeDeauthentication )
            {
            OnDeauthenticateFrameRx( aCtxImpl, aBuffer );
            }
        else if ( hdr->iHdr.iFrameControl.iType 
            == E802Dot11FrameTypeDisassociation )
            {
            OnDisassociateFrameRx( aCtxImpl, aBuffer );
            }
        else if ( hdr->iHdr.iFrameControl.iType 
            == E802Dot11FrameTypeBeacon )
            {
            OnBeaconFrameRx( aCtxImpl, aFrame, aLength, aRcpi, aBuffer );
            }
        else if ( hdr->iHdr.iFrameControl.iType 
            == E802Dot11FrameTypeProbeResp )
            {
            OnProbeResponseFrameRx( aCtxImpl, aFrame, aLength, aRcpi, aBuffer );
            }
        else
            {
            OsTracePrint( KRxFrame | KWarningLevel, (TUint8*)
                ("UMAC: WlanDot11Associated::ReceivePacket: unsupported frame type: 0x%02x"), 
                hdr->iHdr.iFrameControl.iType );            

            // release the Rx buffer
            aCtxImpl.iUmac.MarkRxBufFree( aBuffer );
            }
        }
    else if ( aStatus == WHA::KDecryptFailure )
        {
        // decryption error
        OsTracePrint( KRxFrame | KWarningLevel, (TUint8*)
            ("UMAC: WlanDot11Associated::ReceivePacket: decrypt error for frame:"), hdr->iHdr );    

        OnInDicationEvent( aCtxImpl, EWepDecryptFailure );

        // release the Rx buffer
        aCtxImpl.iUmac.MarkRxBufFree( aBuffer );
        }
    else if ( aStatus == WHA::KMicFailure )
        {
        // MIC failed
        OsTracePrint( KRxFrame | KWarningLevel, (TUint8*)
            ("UMAC: WlanDot11Associated::ReceivePacket: MIC error for frame:"), hdr->iHdr );    

        // address 1 is always the DA in our case
        OnInDicationEvent( aCtxImpl, 
            (IsGroupBitSet( hdr->iHdr.iAddress1 )) 
            ? EGroupKeyMicFailure : EPairwiseKeyMicFailure 
            );

        // release the Rx buffer
        aCtxImpl.iUmac.MarkRxBufFree( aBuffer );
        }    
    else
        {
        // packet rececption failed 
        OsTracePrint( KRxFrame | KWarningLevel, (TUint8*)
            ("UMAC: WlanDot11Associated::ReceivePacket: frame receive failure"));

        // release the Rx buffer
        aCtxImpl.iUmac.MarkRxBufFree( aBuffer );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanDot11Associated::EncryptTxFrames( 
    WlanContextImpl& aCtxImpl,
    const TDataBuffer& aDataBuffer ) const
    {
    TBool encrypt ( EFalse );

    if ( aDataBuffer.KeFlags() & TDataBuffer::KTxFrameMustNotBeEncrypted )
        {
        // our client has instructed us not the encrypt this frame under
        // any circumstances. EFalse will be returned. 
        // No further action needed
        }
    else
        {
        const WHA::TKeyType pairwiseKey ( aCtxImpl.PairWiseKeyType() );
        const WHA::TKeyType groupKey ( aCtxImpl.GroupKeyType() );
    
        if ( pairwiseKey != WHA::EKeyNone )
            {
            // pairwise key set => use encryption
            encrypt = ETrue;
            }
        else
            {
            // pairwise key not set
            
            if ( groupKey == WHA::EWepGroupKey )
                {
                // wep group key set => use encryption
                encrypt = ETrue;
                }        
            }
        }
        
    return encrypt;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TUint WlanDot11Associated::DecryptHdrOffset( 
    WlanContextImpl& aCtxImpl, 
    TUint32 aFlags ) const
    {
    TUint offset( 0 );

    const TUint32 encryption ( aFlags & KReceivePacketEncryptionMask );

    if ( aCtxImpl.WHASettings().iCapability & WHA::SSettings::KNoSecHdrAndTrailer )
        {
        // no security header is present on this sw layer. It is removed
        // on lower layers; when necessary.
        // We will return zero, so no further actions
        }
    else
        {
        // IV and/or Ext IV or CCMP header is present
        // on this sw layer; when applicable
        
        if ( encryption == WHA::KEncryptAes )
            {
            offset = KCcmpHeaderLength;
            }
        else if ( encryption == WHA::KEncryptTkip )
            {
            offset = KWepIVLength + KWepExtendedIVLength;  
            }
        else if ( encryption == WHA::KEncryptWep )
            {
            offset = KWepIVLength;
            }
        else if ( encryption == WHA::KEncryptWapi )
            {
            offset = KWapiHeaderLength;
            }
        else
            {
            // frame not encrypted; returns zero
            }        
        }

    return offset;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TUint WlanDot11Associated::DecryptTrailerOffset( 
    WlanContextImpl& aCtxImpl, 
    TUint32 aFlags ) const
    {
    TUint offset( 0 );
    
    const TUint32 encryption ( aFlags & KReceivePacketEncryptionMask );

    if ( aCtxImpl.WHASettings().iCapability & WHA::SSettings::KNoSecHdrAndTrailer )
        {
        // no security trailer is present on this sw layer. It is removed
        // on lower layers; when necessary.
        // We will return zero, so no further actions
        }
    else
        {
        // ICV and/or MIC field is present on this sw layer; when applicable
    
        if ( encryption == WHA::KEncryptAes )
            {
            offset = KMicLength;
            }
        else if ( encryption == WHA::KEncryptTkip )
            {
            offset = KMicLength + KWEPICVLength;
            }
        else if ( encryption == WHA::KEncryptWep )
            {
            offset = KWEPICVLength;
            }
        else if ( encryption == WHA::KEncryptWapi )
            {
            offset = KWapiMicLength;
            }
        else
            {
            // frame not encrypted; returns zero
            }        
        }

    return offset;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TUint WlanDot11Associated::ComputeEncryptionOffsetAmount(
    const WlanContextImpl& aCtxImpl,
    const TDataBuffer& aDataBuffer ) const
    {
    TUint offset( 0 );

    if ( // our client has instructed us not the encrypt this frame under
         // any circumstances OR
         ( aDataBuffer.KeFlags() & TDataBuffer::KTxFrameMustNotBeEncrypted ) ||
         // no space is reserved for security header on this sw layer. It is
         // done on lower layers; when necessary.
         ( aCtxImpl.WHASettings().iCapability & 
           WHA::SSettings::KNoSecHdrAndTrailer ) )
        {
        // We will return zero, so no further actions
        }
    else
        {
        // encryption is allowed if relevant and
        // space is reserved for IV and/or Ext IV or CCMP header
        // on this sw layer; when necessary

        const WHA::TKeyType groupKey( aCtxImpl.GroupKeyType() );
        const WHA::TKeyType pairwiseKey( aCtxImpl.PairWiseKeyType() );

        if ( pairwiseKey == WHA::EAesPairWiseKey )
            {
            offset = KCcmpHeaderLength;
            }
        else if ( pairwiseKey == WHA::ETkipPairWiseKey )
            {
            offset = KWepIVLength + KWepExtendedIVLength;
            }
        else if ( pairwiseKey == WHA::EWepPairWiseKey )
            {
            offset = KWepIVLength;
            }
        else if ( pairwiseKey == WHA::EWapiPairWiseKey )
            {
            offset = KWapiHeaderLength;
            }
        else
            {
            // don't care of anything else
            }

        if ( !offset )
            {
            // no encryption used based on pairwise key presence
            // check for WEP groupkey presence
            if ( groupKey == WHA::EWepGroupKey )
                {
                offset = KWepIVLength;
                }
            else
                {
                // don't care of anything else as group key encyption is not 
                // supported for the other key types. For them we always have 
                // a pairwise key
                }
            }        
        }

    return offset;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TUint WlanDot11Associated::EncryptTrailerLength(
    WlanContextImpl& aCtxImpl,
    const TDataBuffer& aDataBuffer ) const
    {
    TUint length( 0 );

    if ( // our client has instructed us not the encrypt this frame under
         // any circumstances OR
         ( aDataBuffer.KeFlags() & TDataBuffer::KTxFrameMustNotBeEncrypted ) ||
         // no space is reserved for security trailer on this sw layer. It is
         // done on lower layers; when necessary.
         ( aCtxImpl.WHASettings().iCapability & 
           WHA::SSettings::KNoSecHdrAndTrailer ) )
        {
        // We will return zero, so no further actions
        }
    else
        {
        // encryption is allowed if relevant and
        // space is reserved for ICV and/or MIC
        // on this sw layer; when necessary

        const WHA::TKeyType groupKey( aCtxImpl.GroupKeyType() );
        const WHA::TKeyType pairwiseKey( aCtxImpl.PairWiseKeyType() );    

        if ( pairwiseKey == WHA::EAesPairWiseKey )
            {
            length = KMicLength;
            }
        else if ( pairwiseKey == WHA::ETkipPairWiseKey )    
            {
            length = KMicLength + KWEPICVLength;
            }
        else if ( pairwiseKey == WHA::EWepPairWiseKey )
            {
            length = KWEPICVLength;
            }
        else if ( pairwiseKey == WHA::EWapiPairWiseKey )
            {
            length = KWapiMicLength;
            }
        else
            {
            // don't care of anything else
            }

        if ( !length )
            {
            // no pairwise key present
            // check for groupkey
            if ( groupKey == WHA::EWepGroupKey )
                {
                length = KWEPICVLength;
                }
            else
                {
                // don't care of anything else as group key encyption is not 
                // supported for the other key types. For them we always have 
                // a pairwise key
                }
            }        
        }

    return length;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TUint WlanDot11Associated::ComputeQosOffsetAmount(
    WlanContextImpl& aCtxImpl ) const
    {
    const TUint KNoQosHeader( 0 );
    
    if ( aCtxImpl.QosEnabled() )        
        {
        return sizeof( T802Dot11QosControl );
        }
    else
        {
        return KNoQosHeader;
        }    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11Associated::EncapsulateEthernetFrame(
    WlanContextImpl& aCtxImpl,
    TWlanUserTxDataCntx& aDataCntx,
    TDataBuffer& aDataBuffer,
    TUint16& aEtherType ) const
    {
    TUint8* etherFrameBeginning ( aDataBuffer.GetBuffer() );

    OsTracePrint( KWsaTxDetails, (TUint8*)
        ("UMAC: WlanDot11Associated::EncapsulateEthernetFrame: supplied ether frame start address: 0x%08x"),
        reinterpret_cast<TUint32>(etherFrameBeginning) );

    // start of dot11 frame
    SDataFrameHeader* dot11_dataframe_hdr ( NULL );

    // start of ethernet frame
    const SEthernetHeader* ether_hdr 
        = reinterpret_cast<const SEthernetHeader*>(etherFrameBeginning);

    // determine Ethernet type
    aEtherType = ether_hdr->Type();

    OsTracePrint( KWsaTxDetails, (TUint8*)
        ("UMAC: ether type: 0x%04x"),
        aEtherType );    

    // compute space required for encryption header
    // after dot11 radio header and before data payload
    const TUint encryption_offset = ( 
        ComputeEncryptionOffsetAmount( aCtxImpl, aDataBuffer ) );

    OsTracePrint( KWsaTxDetails, (TUint8*)
        ("UMAC: encryption_offset: %d"),
        encryption_offset );    

    const TUint32 ether_offset = (KMacAddressLength << 1);
    
    // take a backup copy of the destination and source addresses from the
    // ethernet frame as they will get overwritten
    
    TMacAddress da;
    os_memcpy( 
        reinterpret_cast<TUint8*>(&da), 
        etherFrameBeginning, 
        sizeof( TMacAddress ) );
    TMacAddress sa;
    os_memcpy( 
        reinterpret_cast<TUint8*>(&sa), 
        etherFrameBeginning + sizeof( TMacAddress ), 
        sizeof( TMacAddress ) );

    // compute space possibly required at the end of the dot11 mac
    // header for the QoS control field, which is required for QoS data frames
    // This will be needed (i.e. the qosOffset will be > 0) if QoS is enabled
    const TUint qosOffset = ComputeQosOffsetAmount( aCtxImpl );
    // the mac header of HT QoS data frames also has an additional field,
    // so determine if that field needs to be present and hence has a non-zero
    // length
    const TUint KHtControlOffset ( aCtxImpl.HtSupportedByNw() ? 
            KHtControlFieldLength : 0 );
    
    if ( qosOffset )
        {
        // a QoS data frame
        
        OsTracePrint( KWsaTxDetails | KQos, (TUint8*)
            ("UMAC: qos data frame"));    

        SQosDataFrameHeader* dot11QosDataFrameHdr = 
            reinterpret_cast<SQosDataFrameHeader*>(
                etherFrameBeginning 
                + ether_offset
                - sizeof( KEncapsulatingRfc1042SnapHeader )
                - encryption_offset
                - KHtControlOffset
                - sizeof( SQosDataFrameHeader ) );

        // construct the MAC header
        new (dot11QosDataFrameHdr) SQosDataFrameHeader;
        
        // set the frame type
        dot11QosDataFrameHdr->iHdr.iFrameControl.iType = E802Dot11FrameTypeQosData;
        
        // reset the QoS control field
        // => ack policy == acknowledge && priority == best effort
        dot11QosDataFrameHdr->ResetQosControl();
        
        // set the user priority
        dot11QosDataFrameHdr->SetUserPriority( aDataBuffer.UserPriority() );

        dot11_dataframe_hdr = reinterpret_cast<SDataFrameHeader*>(
            dot11QosDataFrameHdr);

        if ( KHtControlOffset )
            {
            // HT control field is present => order bit needs to be set
            dot11QosDataFrameHdr->iHdr.SetOrderBit();

            // clear the HT Control field, too
            reinterpret_cast<SHtQosDataFrameHeader*>(
                dot11QosDataFrameHdr)->ResetHtControl();
            }
        else
            {
            // HT control field is not present => order bit needs to be cleared
            dot11QosDataFrameHdr->iHdr.ClearOrderBit();
            }        
        }
    else
        {
        // a non-QoS data frame

        OsTracePrint( KWsaTxDetails, (TUint8*)
            ("UMAC: non-qos data frame"));    

        dot11_dataframe_hdr = reinterpret_cast<SDataFrameHeader*>(
            etherFrameBeginning 
            + ether_offset
            - sizeof( KEncapsulatingRfc1042SnapHeader )
            - encryption_offset
            - sizeof( SDataFrameHeader ) );

        // construct the MAC header. In this case this also sets the frame type
        // correctly
        new (dot11_dataframe_hdr) SDataFrameHeader;
        }

    // set the source address
    dot11_dataframe_hdr->iAddress2 = sa;

    // set the destination address
    DoSetTxMpduDaAddress( *dot11_dataframe_hdr, da );
    
    // set the To DS bit
    if ( aCtxImpl.NetworkOperationMode() == WHA::EBSS )
        {
        dot11_dataframe_hdr->SetToDsBit();        
        // set the BSS ID
        dot11_dataframe_hdr->iAddress1 = aCtxImpl.GetBssId();
        }
    else
        {
        dot11_dataframe_hdr->ClearToDsBit();
        // set the BSS ID
        dot11_dataframe_hdr->iAddress3 = aCtxImpl.GetBssId();
        }
        
    // determine if the frame needs to be encrypted
    if ( EncryptTxFrames( aCtxImpl, aDataBuffer ) )
        {
        dot11_dataframe_hdr->SetWepBit();
        }
    else
        {
        dot11_dataframe_hdr->ClearWepBit();
        }
        
    // set the snap header to correct location. 
    os_memcpy( 
        reinterpret_cast<TUint8*>(dot11_dataframe_hdr)
        + sizeof( SDataFrameHeader )
        + qosOffset
        + KHtControlOffset
        // space occupied by encryption header(s)
        + encryption_offset,  
        &KEncapsulatingRfc1042SnapHeader,
        sizeof( KEncapsulatingRfc1042SnapHeader ) );        

    // clear the area reserved for IV etc, when necessary
    if ( encryption_offset )
        {
        os_memset( 
            reinterpret_cast<TUint8*>(dot11_dataframe_hdr) 
                + sizeof( SDataFrameHeader ) 
                + qosOffset
                + KHtControlOffset,
            0,
            encryption_offset );           
        }

    // compute padding required for encryption trailer (ICV, MIC etc)
    const TUint encryptTrailerLength = (
        EncryptTrailerLength( aCtxImpl, aDataBuffer ) );
    
    // clear the area reserved for encryption trailer, when necessary
    if ( encryptTrailerLength )
        {
        os_memset( 
            reinterpret_cast<TUint8*>(dot11_dataframe_hdr) 
            + sizeof( SDataFrameHeader )
            + qosOffset
            + KHtControlOffset
            + encryption_offset
            + sizeof( SSnapHeader )  
            + aDataBuffer.GetLength() - ether_offset,
            0,
            encryptTrailerLength );                   
        }
    
    // calculate frame length 
    const TUint length_of_frame = 
        // MAC header length
        sizeof( SDataFrameHeader )
        // length of possibly existing QoS control field 
        + qosOffset
        // length of possibly existing HT control field
        + KHtControlOffset
        // SNAP header length
        + sizeof( SSnapHeader ) 
        // length of ethernet payload (including ether type)
        + aDataBuffer.GetLength() - ether_offset
        // encryption header length
        + encryption_offset
        // encryption trailer length
        + encryptTrailerLength;

    // we now have a dot11 frame ready to be sent
    aDataCntx.Dot11FrameReady( 
        reinterpret_cast<TUint8*>(dot11_dataframe_hdr),
        length_of_frame );

    OsTracePrint( KWsaTxDetails, (TUint8*)
        ("UMAC: length_of_frame (excl. fcs): %d"),
        length_of_frame );
    OsTracePrint( KWsaTxDetails, (TUint8*)
        ("UMAC: frame start address: 0x%08x"),
        reinterpret_cast<TUint32>(dot11_dataframe_hdr) );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11Associated::EncapsulateSnapFrame(
    WlanContextImpl& aCtxImpl,
    TWlanUserTxDataCntx& aDataCntx,
    TDataBuffer& aDataBuffer,
    TBool aEncrypt,
    TUint aEncryptionOffset,
    TUint aEncryptTrailerLength,
    TUint aQosOffset,
    TUint aHtControlOffset ) const
    {
    // extract start of frame. The frame starts with a SNAP header
    TUint8* snapFrameBeginning ( aDataBuffer.GetBuffer() );

    // start of dot11 frame
    SDataFrameHeader* dot11_dataframe_hdr( NULL );
    
    if ( aQosOffset )
        {
        OsTracePrint( KWsaTxDetails | KQos, (TUint8*)
            ("UMAC: qos data frame"));    

        SQosDataFrameHeader* dot11QosDataFrameHdr( 
            reinterpret_cast<SQosDataFrameHeader*>(
                snapFrameBeginning 
                - aEncryptionOffset
                - aHtControlOffset
                - sizeof( SQosDataFrameHeader )) );

        // construct the MAC header
        new (dot11QosDataFrameHdr) SQosDataFrameHeader;
        
        // set the frame type
        dot11QosDataFrameHdr->iHdr.iFrameControl.iType = E802Dot11FrameTypeQosData;
        
        // reset the QoS control field
        // => ack policy == acknowledge && priority == best effort
        dot11QosDataFrameHdr->ResetQosControl();        

        // set the user priority
        dot11QosDataFrameHdr->SetUserPriority( aDataBuffer.UserPriority() );

        dot11_dataframe_hdr = reinterpret_cast<SDataFrameHeader*>(
            dot11QosDataFrameHdr);

        if ( aHtControlOffset )
            {
            // HT control field is present => order bit needs to be set
            dot11QosDataFrameHdr->iHdr.SetOrderBit();
            // clear the HT Control field, too
            reinterpret_cast<SHtQosDataFrameHeader*>(
                dot11QosDataFrameHdr)->ResetHtControl();
            }
        else
            {
            // HT control field is not present => order bit needs to be cleared
            dot11QosDataFrameHdr->iHdr.ClearOrderBit();
            }        
        }
    else
        {
        OsTracePrint( KWsaTxDetails | KQos, (TUint8*)
            ("UMAC: non-qos data frame"));    

        dot11_dataframe_hdr = reinterpret_cast<SDataFrameHeader*>(
            snapFrameBeginning 
            - aEncryptionOffset
            - sizeof( SDataFrameHeader ) );

        // construct the MAC header. In this case this also sets the frame type
        // correctly
        new (dot11_dataframe_hdr) SDataFrameHeader;
        }

    // set the source address
    dot11_dataframe_hdr->iAddress2 = aCtxImpl.iWlanMib.dot11StationId;

    // set the destination address
    DoSetTxMpduDaAddress( *dot11_dataframe_hdr, 
        aDataBuffer.KeDestinationAddress() );
    
    // set the To DS bit
    if ( aCtxImpl.NetworkOperationMode() == WHA::EBSS )
        {
        dot11_dataframe_hdr->SetToDsBit();        
        // set the BSS ID
        dot11_dataframe_hdr->iAddress1 = aCtxImpl.GetBssId();
        }
    else
        {
        dot11_dataframe_hdr->ClearToDsBit();
        // set the BSS ID
        dot11_dataframe_hdr->iAddress3 = aCtxImpl.GetBssId();
        }
        
    // determine if the frame needs to be encrypted
    if ( aEncrypt )
        {
        dot11_dataframe_hdr->SetWepBit();
        }
    else
        {
        dot11_dataframe_hdr->ClearWepBit();
        }

    // clear the area reserved for IV etc, when necessary
    if ( aEncryptionOffset )
        {
        os_memset( 
            reinterpret_cast<TUint8*>(dot11_dataframe_hdr) 
                + sizeof( SDataFrameHeader ) 
                + aQosOffset
                + aHtControlOffset,
            0,
            aEncryptionOffset );           
        }
    
    // clear the area reserved for encryption trailer, when necessary
    if ( aEncryptTrailerLength )
        {
        os_memset( 
            reinterpret_cast<TUint8*>(dot11_dataframe_hdr) 
            + sizeof( SDataFrameHeader )
            + aQosOffset
            + aHtControlOffset
            + aEncryptionOffset
            + aDataBuffer.GetLength(),
            0,
            aEncryptTrailerLength );                   
        }

    // calculate frame length
    const TUint length_of_frame = 
        // MAC header length
        sizeof( SDataFrameHeader ) +
        // length of possibly existing QoS control field 
        aQosOffset + 
        // length of possibly existing HT control field
        aHtControlOffset +
        // payload (including SNAP header)
        aDataBuffer.GetLength()
        // encryption header length
        + aEncryptionOffset
        // encryption trailer length
        + aEncryptTrailerLength;

    // we now have a dot11 frame ready to be sent
    aDataCntx.Dot11FrameReady( 
        reinterpret_cast<TUint8*>(dot11_dataframe_hdr),
        length_of_frame );

    OsTracePrint( KWsaTxDetails, (TUint8*)
        ("UMAC: length_of_frame: %d"),
        length_of_frame );
    OsTracePrint( KWsaTxDetails, (TUint8*)
        ("UMAC: frame start address: 0x%08x"),
        reinterpret_cast<TUint32>(dot11_dataframe_hdr) );
    // trace the dot11 frame header
    OsTracePrint( KWsaTxDetails, (TUint8*)
        ("UMAC: Encapsulated prorietary SNAP Tx frame:"), 
        *(reinterpret_cast<Sdot11MacHeader*>(dot11_dataframe_hdr))); 
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11Associated::SetDot11FrameToTxBuffer(
    const WlanContextImpl& aCtxImpl,
    TWlanUserTxDataCntx& aDataCntx,
    TDataBuffer& aDataBuffer ) const
    {
    OsTracePrint( KWsaTxDetails, (TUint8*)
        ("UMAC: WlanDot11Associated::SetDot11FrameToTxBuffer") );    

    if ( aCtxImpl.HtSupportedByNw() && aCtxImpl.QosEnabled() )
        {
        // in this case we need to insert the HT Control field to the
        // otherwise ready 802.11 MAC frame

        const TUint KOrigLengthOfFrame = aDataBuffer.GetLength();
        const TUint KMgmtFrameMacHdrLen = sizeof( SManagementFrameHeader );
        TUint8* KFrameStart = aDataBuffer.GetBuffer();

        os_memcpy( 
            KFrameStart + 
            KMgmtFrameMacHdrLen +
            KHtControlFieldLength,
            KFrameStart + 
            KMgmtFrameMacHdrLen,
            KOrigLengthOfFrame - KMgmtFrameMacHdrLen );

        // clear the added HT Control field
        reinterpret_cast<SHtManagementFrameHeader*>( 
            KFrameStart)->ResetHtControl();
        // update frame length
        aDataBuffer.KeSetLength( KOrigLengthOfFrame + KHtControlFieldLength );
        // as the HT control field is present the order bit needs to be set
        reinterpret_cast<SHtManagementFrameHeader*>( 
            KFrameStart)->iMgmtFrameHdr.SetOrderBit();        
        }
    
    // we now have a dot11 frame ready to be sent
    aDataCntx.Dot11FrameReady( 
        aDataBuffer.GetBuffer(), 
        aDataBuffer.GetLength() );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11Associated::EncapsulateFrame(
    WlanContextImpl& aCtxImpl,
    TWlanUserTxDataCntx& aDataCntx,
    TDataBuffer& aDataBuffer,
    TUint16& aEtherType )
    {
    const TDataBuffer::TFrameType KFrameType( aDataBuffer.FrameType() );
    
    OsTracePrint( KWsaTx, (TUint8*)
        ("UMAC: WlanDot11Associated::EncapsulateFrame: frame type: %d"),
        KFrameType );
    
    if ( KFrameType == TDataBuffer::KEthernetFrame || 
         KFrameType == TDataBuffer::KEthernetTestFrame )
        {
        // ethernet II frame in the buffer for tx

        EncapsulateEthernetFrame( 
            aCtxImpl, 
            aDataCntx, 
            aDataBuffer, 
            aEtherType );
        }
    else if ( KFrameType == TDataBuffer::KSnapFrame )
        {
        // frame beginning with a SNAP header in the buffer for tx

        EncapsulateSnapFrame( 
            aCtxImpl, 
            aDataCntx, 
            aDataBuffer,
            EncryptTxFrames( aCtxImpl, aDataBuffer ),
            ComputeEncryptionOffsetAmount( aCtxImpl, aDataBuffer ),
            EncryptTrailerLength( aCtxImpl, aDataBuffer ),
            ComputeQosOffsetAmount( aCtxImpl ),
            aCtxImpl.HtSupportedByNw() ? 
                KHtControlFieldLength : 
                0 );
        }
    else if ( KFrameType == TDataBuffer::KDot11Frame )
        {
        // ready made 802.11 frame in the buffer for tx

        SetDot11FrameToTxBuffer( aCtxImpl, aDataCntx, aDataBuffer );
        }
    else
        {
        // not supported
        OsTracePrint( KErrorLevel, (TUint8*)("UMAC: frame_type: %d"),
            KFrameType );
        OsAssert( (TUint8*)("UMAC: panic"),(TUint8*)(WLAN_FILE), __LINE__ );                
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanDot11Associated::TxData( 
    WlanContextImpl& aCtxImpl,
    TDataBuffer& aDataBuffer,
    TBool aMore )
    {
    TWlanUserTxDataCntx& data_cntx( aCtxImpl.GetTxDataCntx() );
    
    TBool stateChange( EFalse );

    if ( (aCtxImpl.ProtocolStackTxDataAllowed()) )
        {
        // protocol stack tx data allowed
        // now construct a dot11 frame from databuffer to storage

        TUint16 etherType( 0 ); // initial value: not relevant
        
        // construct the frame
        EncapsulateFrame( aCtxImpl, data_cntx, aDataBuffer, etherType );

        // dot11 frame ready to be sent so push it to the packet sceduler

        // start of dot11 frame to send
        const TUint8* start_of_frame( 
            data_cntx.StartOfFrame() );

        // select correct tx queue
        const WHA::TQueueId queue_id( 
            QueueId( aCtxImpl, start_of_frame ) );

        // push the frame to packet scheduler for transmission
        aCtxImpl.PushPacketToPacketScheduler(
            start_of_frame,
            data_cntx.LengthOfFrame(),
            queue_id,
            E802Dot11FrameTypeData,
            &aDataBuffer,
            aMore,
            OutgoingMulticastDataFrame( 
                reinterpret_cast<const SDataFrameHeader*>( start_of_frame ) ) );
            // now just wait for the scheduler to call completion methods
            
        // check if we need to change power mgmt mode because of frame Tx
        const TPowerMgmtModeChange KPowerMgmtModeChange ( 
            aCtxImpl.OnFrameTx( queue_id, etherType ) );
        
        // if any change is needed regarding our power mgmt mode,
        // proceed with it
        stateChange = PowerMgmtModeChange( aCtxImpl, KPowerMgmtModeChange );        
        }
    else
        {
        // protocol stack tx data not allowed

#ifndef NDEBUG
        // programming error
        OsTracePrint( KErrorLevel, (TUint8*)
            ("UMAC: Tx attempted when it's not allowed") );
        OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
#else
        aCtxImpl.iUmac.OnTxProtocolStackDataComplete( 
            KErrNone,
            &aDataBuffer );        
#endif        
        }

    return stateChange;        
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11Associated::OnPacketSendComplete(
    WlanContextImpl& aCtxImpl, 
    WHA::TStatus aStatus,
    TUint32 aPacketId,
    WHA::TRate aRate,
    TUint32 /*aPacketQueueDelay*/,
    TUint32 aMediaDelay,
    TUint aTotalTxDelay,
    TUint8 aAckFailures,
    WHA::TQueueId aQueueId,
    WHA::TRate aRequestedRate,
    TBool aMulticastData )
    {
    if ( aPacketId == E802Dot11FrameTypeData ||
         aPacketId == E802Dot11FrameTypeDataEapol )
        {
        // update data frame statistics
        UpdateTxDataFrameStatistics( 
            aCtxImpl,
            aQueueId, 
            aStatus, 
            aMulticastData, 
            aAckFailures,
            aMediaDelay,
            aTotalTxDelay );
        }
    else if ( aPacketId == E802Dot11FrameTypeQosDataNull )        
        {
        // inform Null Data frame sending controller of QoS Null data Tx 
        // completion; successful or not
        aCtxImpl.OnQosNullDataTxCompleted();
        }        
    else if ( aPacketId == E802Dot11FrameTypeDataNull )        
        {
        // inform Null Data frame sending controller of Null data Tx 
        // completion; successful or not
        aCtxImpl.OnNullDataTxCompleted();
        }        

    if ( aStatus == WHA::KSuccess )
        {
        aCtxImpl.OnTxCompleted( aRate, ETrue, aQueueId, aRequestedRate );
    
        aCtxImpl.ResetFailedTxPacketCount();
        DoRegainedBSSIndication( aCtxImpl );

        if ( aPacketId == E802Dot11FrameTypeData ||
             aPacketId == E802Dot11FrameTypeDataEapol ||
             aPacketId == E802Dot11FrameTypeTestFrame )
            {
            // inform Null Data frame sending controller of successful
            // data frame Tx completion
            aCtxImpl.OnDataTxCompleted( 
                aPacketId == E802Dot11FrameTypeDataEapol ?
                // as EAPOL and WAI frames are not really Voice data (we just
                // send them with Voice priority in WMM nw), handle them
                // here as Best Effort (Legacy)
                WHA::ELegacy : 
                aQueueId );
            }
        }
    else if ( aStatus == WHA::KErrorLifetimeExceeded && !aAckFailures )
        {
        // the packet was discarded by WLAN PDD without any Tx attempts
        // So this is not a Tx failure and we don't need to take
        // any further actions
        OsTracePrint( KWsaTxDetails, (TUint8*)
            ("UMAC: WlanDot11Associated::OnPacketSendComplete: packet expired in PDD without any Tx attempts)"));
        }
    else
        {
        // an actual Tx failure has occurred
        
        aCtxImpl.OnTxCompleted( aRate, EFalse, aQueueId, aRequestedRate );
    
        aCtxImpl.IncrementFailedTxPacketCount();

        // if we have failed to send more than threshold number of 
        // consecutive packets, send Consecutive Tx Failures indication to 
        // WLAN Mgmt Client - unless already sent
        if ( aCtxImpl.FailedTxPacketCount() > 
             aCtxImpl.iWlanMib.iFailedTxPacketCountThreshold )
            {
            DoConsecutiveTxFailuresIndication( aCtxImpl );
            aCtxImpl.ResetFailedTxPacketCount();
            }        
        }
    
    aCtxImpl.iUmac.OnTxDataSent();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11Associated::DoConsecutiveBeaconsLostIndication( 
    WlanContextImpl& aCtxImpl )
    {
    if ( aCtxImpl.OnConsecutiveBeaconsLost() )
        {
        OnInDicationEvent( aCtxImpl, EConsecutiveBeaconsLost );
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11Associated::DoConsecutiveTxFailuresIndication( 
    WlanContextImpl& aCtxImpl )
    {
    if ( aCtxImpl.OnConsecutiveTxFailures() )
        {
        OnInDicationEvent( aCtxImpl, EConsecutiveTxFailures );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11Associated::DoRegainedBSSIndication( 
    WlanContextImpl& aCtxImpl )
    {
    if ( aCtxImpl.OnBssRegained() )
        {
        OnInDicationEvent( aCtxImpl, EBSSRegained );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanDot11Associated::AddBroadcastWepKey(
    WlanContextImpl& aCtxImpl,
    TUint32 aKeyIndex,             
    TBool aUseAsDefaulKey,                
    TUint32 aKeyLength,                      
    const TUint8 aKey[KMaxWEPKeyLength],
    const TMacAddress& aMac )
    {
    return OnAddBroadcastWepKey( aCtxImpl, aKeyIndex, aUseAsDefaulKey, 
        EFalse, // do NOT set as PTK
        aKeyLength, aKey, aMac );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11Associated::ConfigureTxQueueIfNecessary( 
    WlanContextImpl& aCtxImpl,
    TQueueId aQueueId,
    TUint16 aMediumTime,
    TUint32 aMaxTxMSDULifetime )
    {
    // this cast is safe as the types are effectively the same
    const WHA::TQueueId whaQueueId = static_cast<WHA::TQueueId>(aQueueId);
    
    if ( aMediumTime != aCtxImpl.iWlanMib.iMediumTime[whaQueueId] ||
         aMaxTxMSDULifetime != 
            aCtxImpl.iWlanMib.dot11MaxTransmitMSDULifetime[whaQueueId] )
        {
        // at least one of the parameters for this queue is changed => a
        // reconfiguration is needed
        
        // update the queue parameters. These values will be used in the
        // queue reconfiguration

        aCtxImpl.iWlanMib.iMediumTime[aQueueId] = aMediumTime;
        aCtxImpl.iWlanMib.dot11MaxTransmitMSDULifetime[whaQueueId] = 
            aMaxTxMSDULifetime;

        OsTracePrint( KUmacDetails, (TUint8*)
            ("UMAC: WlanDot11Associated::ConfigureTxQueueIfNecessary: reconfiguring the queue is necessary") );

        // reconfigure the queue. Also request the WLAN mgmt client request
        // to be completed
        return ConfigureTxQueue( aCtxImpl, whaQueueId, ETrue );
        }
    else
        {
        // the provided queue parameters have not changed, so no need to
        // reconfigure the queue
        
        OsTracePrint( KUmacDetails, (TUint8*)
            ("UMAC: WlanDot11Associated::ConfigureTxQueueIfNecessary: no queue reconfigure is necessary") );

        // complete the WLAN Mgmt Client request
        OnOidComplete( aCtxImpl, KErrNone );

        // signal caller that no state transition occurred
        return EFalse;
        }
    }
 
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanDot11Associated::PowerMgmtModeChange(
    WlanContextImpl& aCtxImpl,
    TPowerMgmtModeChange aPowerMgmtModeChange )
    {
    TBool stateChange ( EFalse );
    
    if ( aPowerMgmtModeChange != ENoChange )
        {
        // power mgmt mode change needed
        
        if ( aPowerMgmtModeChange == EToActive )
            {
            aCtxImpl.DesiredDot11PwrMgmtMode( WHA::KPsDisable );
            }
        else if ( aPowerMgmtModeChange == EToLightPs )
            {
            aCtxImpl.DesiredDot11PwrMgmtMode( WHA::KPsEnable );
            aCtxImpl.SetDesiredPsModeConfig( 
                aCtxImpl.ClientLightPsModeConfig() );                
            }
        else // aPowerMgmtModeChange == EToDeepPs
            {
            aCtxImpl.DesiredDot11PwrMgmtMode( WHA::KPsEnable );
            aCtxImpl.SetDesiredPsModeConfig( 
                aCtxImpl.ClientDeepPsModeConfig() );                
            }
        
        if ( !(aCtxImpl.WsaCmdActive()) )
            {
            // proceed with the power mgmt mode change
            stateChange = ChangePowerMgmtMode( aCtxImpl );
            }
        else
            {
            // WHA command is in progress so we must defer this access
            aCtxImpl.RegisterEvent( KPowerMgmtTransition );

            OsTracePrint( KEventDispatcher | KPwrStateTransition, 
                (TUint8*)("UMAC: WlanDot11Associated::PowerMgmtModeChange: power mgmt mode change event registered"));                
            }
        }
    
    return stateChange;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11Associated::HandleProprietarySnapRxFrame( 
    TDataBuffer& aBuffer,
    TBool aQosData, 
    const TAny* const aFrame, 
    const SAmsduSubframeHeader* aSubFrameHeader,
    TUint aLength,
    TUint aDecryptHeaderLen,
    TUint aDecryptTrailerLen,
    TUint aHtControlLen,
    TUint8* aCopyBuffer ) const
    {
    OsTracePrint( KRxFrame, (TUint8*)
        ("UMAC: WlanDot11Associated::HandleProprietarySnapRxFrame"));

    // prepare received frame with proprietary SNAP header for wlan mgmt client
    // Remove the possibly existing security header & trailer before
    // passing the frame up
    
    const TUint8* frameBeginning = reinterpret_cast<const TUint8*>(aFrame);
    const TUint KMacHdrLen( aQosData ? 
        sizeof( SQosDataFrameHeader ) + aHtControlLen : 
        sizeof( SDataFrameHeader ) );
    // subframe header length is non-zero only if the frame is part of an 
    // A-MSDU, i.e. if aCopyBuffer is not NULL
    const TUint KSubframeHdrLen ( 
        aCopyBuffer ? sizeof( SAmsduSubframeHeader ) : 0 );    
    // determine subframe length
    const TUint KSubframeLen ( 
        aSubFrameHeader ? aSubFrameHeader->Length() : 0 );    

    if ( aCopyBuffer )
        {
        // the frame needs to be copied to the copy buffer, which means
        // that it is part of an A-MSDU

        // 1st copy the MAC header
        os_memcpy( aCopyBuffer, frameBeginning, KMacHdrLen );
                
        // then copy the subframe body following the subframe header
        os_memcpy( 
            aCopyBuffer 
            + KMacHdrLen, 
            reinterpret_cast<const TUint8*>(aSubFrameHeader) + KSubframeHdrLen,
            KSubframeLen );

        // update to point to the new location
        frameBeginning = aCopyBuffer;
        }
    else
        {
        // no copying to the copy buffer is required
        
        if ( aDecryptHeaderLen )
            {
            // decrypt header exists. Shift the MAC header so that it
            // overwrites the decrypt header, thus removing it
            
            TUint8* dest( const_cast<TUint8*>(frameBeginning) 
                          + aDecryptHeaderLen );
    
            TUint copyBlockSize( aQosData ? 
                sizeof( SQosDataFrameHeader ) + aHtControlLen : 
                sizeof( SDataFrameHeader ) );
    
            os_memcpy( dest, frameBeginning, copyBlockSize );

            // update to point to the new location
            frameBeginning = dest;            
            }
        }

#ifndef NDEBUG
    OsTracePrint( KRxFrame, (TUint8*)
        ("UMAC: WlanDot11Associated::HandleProprietarySnapRxFrame: MPDU header:"), 
        *(reinterpret_cast<const Sdot11MacHeader*>(
            frameBeginning)));
#endif

    // set the frame length
    if ( aCopyBuffer )
        {
        aBuffer.KeSetLength( KMacHdrLen + KSubframeLen );        
        }
    else
        {
        aBuffer.KeSetLength( 
            aLength - aDecryptHeaderLen - aDecryptTrailerLen );
        }
    // set the frame type
    aBuffer.FrameType( TDataBuffer::KDot11Frame );
    // set the offset to the beginning of the actual frame within the
    // Rx buffer
    aBuffer.KeSetOffsetToFrameBeginning(
        frameBeginning                  // frame beginning
        - aBuffer.KeGetBufferStart() ); // buffer beginning
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanDot11Associated::ConfigureTxRatePolicies( 
    WlanContextImpl& aCtxImpl,
    const TTxRatePolicy& aRatePolicy,
    const TQueue2RateClass& aQueue2RateClass,
    const TInitialMaxTxRate4RateClass& aInitialMaxTxRate4RateClass,
    const TTxAutoRatePolicy& aAutoRatePolicy,
    const THtMcsPolicy& aHtMcsPolicy )
    {
    OsTracePrint( KTxRateAdapt, (TUint8*)
        ("UMAC: WlanDot11Associated::ConfigureTxRatePolicies"));

    TBool stateChange( EFalse );

    if ( aCtxImpl.ProtocolStackTxDataAllowed() )
        {
        // store the provided information ...
        StoreTxRatePolicyInfo( 
            aCtxImpl,
            aRatePolicy,
            aQueue2RateClass,
            aInitialMaxTxRate4RateClass,
            aAutoRatePolicy,
            aHtMcsPolicy );

        // ... take it into use; and specify that the mgmt client request needs
        // to be completed when doing it
        stateChange = WlanDot11State::ConfigureTxRatePolicies( aCtxImpl, 
                                                               ETrue );
        if ( !stateChange )
            {
            // a fatal error occurred. Simulate MAC Not Responding error
            // Note that the Mgmt Client request will be completed when 
            // entering the dot11error state
            stateChange = DoErrorIndication( 
                aCtxImpl, 
                WHA::KErrorMacNotResponding );            
            }
        }
    else
        {
        // as user data is not allowed currently, it means that WLAN Mgmt client
        // will request us to connect to a new nw shortly and this policy is for
        // that new nw. So we shouldn't take the new rate policy into use yet, 
        // as we don't know which rates the new nw will be supporting. We just 
        // store the provided information for later use
        stateChange = WlanDot11State::ConfigureTxRatePolicies( 
            aCtxImpl,
            aRatePolicy,
            aQueue2RateClass,
            aInitialMaxTxRate4RateClass,
            aAutoRatePolicy,
            aHtMcsPolicy );
        }
        
    return stateChange;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11Associated::ConfigurePwrModeMgmtTrafficOverride( 
    WlanContextImpl& aCtxImpl,
    TBool aStayInPsDespiteUapsdVoiceTraffic,
    TBool aStayInPsDespiteUapsdVideoTraffic,
    TBool aStayInPsDespiteUapsdBestEffortTraffic, 
    TBool aStayInPsDespiteUapsdBackgroundTraffic,
    TBool aStayInPsDespiteLegacyVoiceTraffic,
    TBool aStayInPsDespiteLegacyVideoTraffic,
    TBool aStayInPsDespiteLegacyBestEffortTraffic,
    TBool aStayInPsDespiteLegacyBackgroundTraffic )
    {
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanDot11Associated::ConfigurePwrModeMgmtTrafficOverride"));

    aCtxImpl.ConfigurePwrModeMgmtTrafficOverride( 
        aStayInPsDespiteUapsdVoiceTraffic,
        aStayInPsDespiteUapsdVideoTraffic,
        aStayInPsDespiteUapsdBestEffortTraffic, 
        aStayInPsDespiteUapsdBackgroundTraffic,
        aStayInPsDespiteLegacyVoiceTraffic,
        aStayInPsDespiteLegacyVideoTraffic,
        aStayInPsDespiteLegacyBestEffortTraffic,
        aStayInPsDespiteLegacyBackgroundTraffic );

    // as we are already connected and aware of the network capabilities, we 
    // also need to freeze the dynamic power mode mgmt traffic 
    // override/ignoration settings so that they become immediately effective
    aCtxImpl.FreezePwrModeMgmtTrafficOverride();

    OnOidComplete( aCtxImpl, KErrNone );

    // signal caller that no state transition occurred
    return EFalse;    
    }
