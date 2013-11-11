/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of WlanDot11PrepareForIbssMode class
*
*/

/*
* %version: 28 %
*/

#include "config.h"
#include "UmacDot11PrepareForIbssMode.h"
#include "UmacWsaWriteMib.h"
#include "UmacWsaJoin.h"
#include "UmacContextImpl.h"
#include "wha_mibDefaultvalues.h"

const TUint KAllocLen( WHA::KBeaconStorageSize + WHA::StemplateFrame::KHeaderSize );

#ifndef NDEBUG
const TInt8 WlanDot11PrepareForIbssMode::iName[] = "dot11-prepareforibssmode";

const TUint8 WlanDot11PrepareForIbssMode::iStateName
    [ESTATEMAX][KMaxStateStringLength] = 
    {
        {"EINIT"}, 
        {"ESETSLEEPMODE"},
        {"ESETDOT11SLOTTIME"},
        {"ESETCTSTOSELF"},
        {"ESETBEACON"},
        {"ESETPROBERESP"},
        {"ESETTXRATEPOLICY"},
        {"ESETBEACONLOSTCOUNT"},
        {"EJOIN"},
        {"ECONTINUEDOT11TRAVERSE"}
    };

const TUint8 WlanDot11PrepareForIbssMode::iEventName
    [EEVENTMAX][KMaxEventStringLength] = 
    {
        {"ESTATEENTRY"}, {"ETXCOMPLETE"}, {"EABORT"}
    };
#endif

// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
WlanDot11PrepareForIbssMode::~WlanDot11PrepareForIbssMode()
    {
    iMemory = NULL;
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
void WlanDot11PrepareForIbssMode::Set( TBool aStartIbss )
    {
    iStartIbss = aStartIbss;
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
void WlanDot11PrepareForIbssMode::Entry( 
    WlanContextImpl& aCtxImpl )
    {
    if ( aCtxImpl.WsaCmdActive() )
        {
        // sanity checking code
        OsAssert( (TUint8*)("UMAC * panic"), 
            (TUint8*)(WLAN_FILE), __LINE__ );
        }

    // don't want to do event dispatching here as we want
    // to run this dot11 state critter in non pre-emptive mode

    if ( iState != EINIT )
        {
        // this is NOT the start of the the FSM actions
        // note that we send the ETXCOMPLETE event as the states
        // that wait for it are the only ones that can be interrupted
        // as they are asynchronous operations by nature
        // and wait for corresponding WHA completion method
        Fsm( aCtxImpl, ETXCOMPLETE );
        }
    else
        {
        // this is the start of the the FSM actions
        Fsm( aCtxImpl, ESTATEENTRY );
        }
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
void WlanDot11PrepareForIbssMode::Exit( 
    WlanContextImpl& /*aCtxImpl*/ )
    {
    // we are traversing to a new dot11 state 
    // make sure we don't generate a memory leakage
    os_free( iMemory );
    iMemory = NULL;
    
    iState = EINIT; 
    iStartIbss = EFalse;
    }

#ifndef NDEBUG 
// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
const TInt8* WlanDot11PrepareForIbssMode::GetStateName( 
    TUint8& aLength ) const
    {
    aLength = sizeof( iName );
    return iName;
    }
#endif

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
void WlanDot11PrepareForIbssMode::ChangeInternalState( 
    WlanContextImpl& aCtxImpl, 
    TState aNewState )
    {
    iState = aNewState;
    Fsm( aCtxImpl, ESTATEENTRY );
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
void WlanDot11PrepareForIbssMode::Fsm( 
    WlanContextImpl& aCtxImpl, 
    TEvent aEvent )
    {
    OsTracePrint( KUmacDetails, 
        (TUint8*)("UMAC * dot11-prepareforibssmode * FSM EVENT") );
#ifndef NDEBUG
    OsTracePrint( KUmacDetails, (TUint8*)("event:"));
    OsTracePrint( KUmacDetails, iEventName[aEvent] );
    OsTracePrint( KUmacDetails, (TUint8*)("state:"));
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
        case EABORT:
            OnAbortEvent( aCtxImpl );
            break;
        default:
            // catch internal FSM programming error
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
// Handler for state entry event.
// It is guaranteed by the FSM framework that no WHA command is pending
// when this method is entered
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void WlanDot11PrepareForIbssMode::OnStateEntryEvent( 
    WlanContextImpl& aCtxImpl )
    {
    switch ( iState )
        {
        case EINIT:
            iMemory = static_cast<TUint8*>(os_alloc( KAllocLen ));
            if ( iMemory )
                {
                // as we are about to join/start a new network, reset BSS Loss
                // indicators
                aCtxImpl.ResetBssLossIndications();

                // start the FSM traversal
                ChangeInternalState( aCtxImpl, ESETSLEEPMODE );
                }
            else
                {
                // allocation failure
                Fsm( aCtxImpl, EABORT );
                }
            break;
        case ESETSLEEPMODE:
            SetSleepMode( aCtxImpl );
            break;
        case ESETDOT11SLOTTIME:
            SetDot11SlotTime( aCtxImpl );
            break;
        case ESETCTSTOSELF:
            SetCtsToSelf( aCtxImpl );
            break;
        case ESETBEACON:
            ConfigureBeaconTemplate( aCtxImpl );
            break;
        case ESETPROBERESP:
            ConfigureProbeResponseTemplate( aCtxImpl );
            break;
        case ESETTXRATEPOLICY:
            SetTxRatePolicy( aCtxImpl );
            break;        
        case ESETBEACONLOSTCOUNT:
            SetBeaconLostCount( aCtxImpl );
            break;        
        case EJOIN:
            Join( aCtxImpl );
            break;
        case ECONTINUEDOT11TRAVERSE:
            ContinueDot11StateTraversal( aCtxImpl );
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

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
void WlanDot11PrepareForIbssMode::OnTxCompleteEvent( 
    WlanContextImpl& aCtxImpl )
    {
    switch ( iState )
        {
        case ESETSLEEPMODE:
            // depending if the WLAN vendor specific solution
            // implements dot11slottime mib we will configure it
            if ( aCtxImpl.WHASettings().iCapability 
                & WHA::SSettings::KDot11SlotTime )
                {
                // supported
                ChangeInternalState( aCtxImpl, ESETDOT11SLOTTIME );
                }
            else
                {
                // not supported so skip it
                OsTracePrint( KWarningLevel, (TUint8*)
                    ("UMAC * dot11-prepareforibssmode"));
                OsTracePrint( KWarningLevel, (TUint8*)
                    ("no support for dot11slottime mib skipping"));

                ChangeInternalState( aCtxImpl, ESETCTSTOSELF );
                }
            break;
        case ESETDOT11SLOTTIME:
            ChangeInternalState( aCtxImpl, ESETCTSTOSELF );
            break;
        case ESETCTSTOSELF:
            ChangeInternalState( aCtxImpl, ESETBEACON );
            break;
        case ESETBEACON:
            ChangeInternalState( aCtxImpl, ESETPROBERESP );
            break;
        case ESETPROBERESP:
            ChangeInternalState( aCtxImpl, ESETTXRATEPOLICY );
            break;
        case ESETTXRATEPOLICY:
            ChangeInternalState( aCtxImpl, ESETBEACONLOSTCOUNT );
            break;
        case ESETBEACONLOSTCOUNT:
            ChangeInternalState( aCtxImpl, EJOIN );
            break;
        case EJOIN:
            ChangeInternalState( aCtxImpl, ECONTINUEDOT11TRAVERSE );
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

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
void WlanDot11PrepareForIbssMode::SetSleepMode( 
    WlanContextImpl& aCtxImpl )
    {
    WHA::SsleepMode* mib 
        = reinterpret_cast<WHA::SsleepMode*>(iMemory); 

    mib->iMode = WHA::KPowerDownMode;
    
    WlanWsaWriteMib& wha_cmd = aCtxImpl.WsaWriteMib();
    wha_cmd.Set( 
        aCtxImpl, WHA::KMibSleepMode, sizeof(*mib), mib );

    // change global state: entry procedure triggers action
    ChangeState( aCtxImpl, 
        *this,              // prev state
        wha_cmd             // next state
        );           
    }

// ---------------------------------------------------------
// the caller of thismethod has allready checked does the 
// WLAN vendor specific solution support this mib so we 
// don't have to anymore check it here again
// ---------------------------------------------------------
//
void WlanDot11PrepareForIbssMode::SetDot11SlotTime( 
    WlanContextImpl& aCtxImpl )
    {
    WHA::Sdot11SlotTime* mib 
        = reinterpret_cast<WHA::Sdot11SlotTime*>(iMemory); 

    if ( aCtxImpl.UseShortSlotTime() )
        {
        mib->iDot11SlotTime = WHA::KSlotTime9;        
        }
    else
        {
        mib->iDot11SlotTime = WHA::KSlotTime20;
        }

    WlanWsaWriteMib& wha_cmd = aCtxImpl.WsaWriteMib();
    wha_cmd.Set( 
        aCtxImpl, WHA::KMibDot11SlotTime, sizeof(*mib), mib );

    // change global state: entry procedure triggers action
    ChangeState( aCtxImpl, 
        *this,              // prev state
        wha_cmd             // next state
        );           
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
void WlanDot11PrepareForIbssMode::SetCtsToSelf( 
    WlanContextImpl& aCtxImpl )
    {    
    WHA::SctsToSelf* mib 
        = reinterpret_cast<WHA::SctsToSelf*>(iMemory); 

    if ( aCtxImpl.ProtectionBitSet() )
        {
        OsTracePrint( 
            KUmacDetails, 
            (TUint8*)("UMAC: WlanDot11PrepareForIbssMode::SetCtsToSelf(): enable CTS to self") );
            
        mib->iCtsToSelf = ETrue;
        }
    else
        {
        OsTracePrint( 
            KUmacDetails, 
            (TUint8*)("UMAC: WlanDot11PrepareForIbssMode::SetCtsToSelf(): disable CTS to self") );

        mib->iCtsToSelf = EFalse;
        }

    WlanWsaWriteMib& wha_cmd = aCtxImpl.WsaWriteMib();
    wha_cmd.Set( 
        aCtxImpl, WHA::KMibCtsToSelf, sizeof(*mib), mib );

    // change global state: entry procedure triggers action
    ChangeState( aCtxImpl, 
        *this,              // prev state
        wha_cmd             // next state
        );           
    }

// ---------------------------------------------------------
// simulate macnotresponding error
// ---------------------------------------------------------
//
void WlanDot11PrepareForIbssMode::OnAbortEvent( 
    WlanContextImpl& aCtxImpl )
    {
    // memory is released by Exit() method
    OsTracePrint( KWarningLevel, 
        (TUint8*)("UMAC: WlanDot11PrepareForIbssMode::OnAbortEvent") );

    DoErrorIndication( aCtxImpl, WHA::KErrorMacNotResponding );
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
void WlanDot11PrepareForIbssMode::ConfigureBeaconTemplate( 
    WlanContextImpl& aCtxImpl )
    {
    TUint32 length( 0 );
    ConfigureTemplate( aCtxImpl, length );

    // default frame configuration is done 
    // now set beacon frame specific configuration
    SManagementFrameHeader* hdr 
        = reinterpret_cast<SManagementFrameHeader*>
        (iMemory + WHA::StemplateFrame::KHeaderSize );
                    
    hdr->iDA = KBroadcastMacAddr;
    hdr->iFrameControl.iType = E802Dot11FrameTypeBeacon;

    // set privacy bit depending do we have a 
    // encryption key set or not
    const WHA::TKeyType group_key = aCtxImpl.GroupKeyType();
    const WHA::TKeyType pairwise_key = aCtxImpl.PairWiseKeyType();

    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanDot11PrepareForIbssMode::ConfigureBeaconTemplate: check for WEP") );

    if ( (group_key != WHA::EKeyNone
        || pairwise_key != WHA::EKeyNone) )
        {
        // we have set a group or pairwise key
        // enable privacy
        SCapabilityInformationField* cap_ptr
            = reinterpret_cast<SCapabilityInformationField*>
            (reinterpret_cast<TUint8*>(hdr + 1) 
            + KTimeStampFixedFieldLength 
            + KBeaconIntervalFixedFieldLength);
        
        OsTracePrint( KUmacDetails, (TUint8*)
            ("UMAC: WlanDot11PrepareForIbssMode::ConfigureBeaconTemplate: enable WEP") );

        cap_ptr->SetWepBit();
        }

    // all done now write the MIB

    WHA::StemplateFrame* mib 
        = reinterpret_cast<WHA::StemplateFrame*>(iMemory);

    mib->iFrameType = WHA::KBeaconTemplate;
    mib->iInitialTransmitRate = BeaconTxRate( aCtxImpl );
    mib->iLength = length;
    
    WlanWsaWriteMib& wha_cmd = aCtxImpl.WsaWriteMib();
    
    TUint32 mib_len( 
        // length of MIB header
        WHA::StemplateFrame::KHeaderSize
        // length of the frame template
        + mib->iLength );

    // align length of MIB to 4-byte boundary
    mib_len = Align4( mib_len );

    wha_cmd.Set( 
        aCtxImpl, 
        WHA::KMibTemplateFrame, 
        mib_len, 
        mib );
      
    if ( iStartIbss )
        {
        // we are starting a new IBSS        
        // transfer our own beacon also to mgmt client as an Rx frame
        
        TUint8* buffer = aCtxImpl.GetRxBuffer( 
            mib->iLength,
            // tell that this is an internally triggered buffer request
            ETrue );
        
        if ( buffer )
            {
            // copy the beacon to the beginning of the buffer
            os_memcpy( buffer, reinterpret_cast<TUint8*>(hdr), mib->iLength );
            
            XferDot11FrameToMgmtClient( 
                aCtxImpl,
                // frame beginning
                buffer, 
                mib->iLength, 
                // RCPI is not relevant in this case
                0,
                // buffer beginning
                buffer );
            }
#ifndef NDEBUG
        else
            {
            // Rx buffer reservation failed. That should not happen when we are
            // starting an IBSS
            OsTracePrint( KErrorLevel, (TUint8*)
                ("UMAC: Rx buf reservation failed") );        
            OsAssert( 
                (TUint8*)("UMAC: panic"), 
                (TUint8*)(WLAN_FILE), 
                __LINE__ );
            }
#endif        
        }
   
    // change global state: entry procedure triggers action
    ChangeState( aCtxImpl, 
        *this,              // prev state
        wha_cmd             // next state
        );            
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
void WlanDot11PrepareForIbssMode::ConfigureProbeResponseTemplate( 
    WlanContextImpl& aCtxImpl )
    {
    TUint32 length( 0 );
    ConfigureTemplate( aCtxImpl, length );

    // default frame configuration has been done allready 
    // now set probe response frame specific configuration
    SManagementFrameHeader* hdr 
        = reinterpret_cast<SManagementFrameHeader*>
        (iMemory + WHA::StemplateFrame::KHeaderSize );

    hdr->iFrameControl.iType = E802Dot11FrameTypeProbeResp;

    // set privacy bit depending do we have a 
    // encryption key set or not
    const WHA::TKeyType group_key = aCtxImpl.GroupKeyType();
    const WHA::TKeyType pairwise_key = aCtxImpl.PairWiseKeyType();

    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanDot11PrepareForIbssMode::ConfigureProbeResponseTemplate: check for WEP") );

    if ( (group_key != WHA::EKeyNone
        || pairwise_key != WHA::EKeyNone) )
        {
        // we have set a group or pairwise key
        // enable privacy
        SCapabilityInformationField* cap_ptr
            = reinterpret_cast<SCapabilityInformationField*>
            (reinterpret_cast<TUint8*>(hdr + 1) 
            + KTimeStampFixedFieldLength 
            + KBeaconIntervalFixedFieldLength);
        
        OsTracePrint( KUmacDetails, (TUint8*)
            ("UMAC: WlanDot11PrepareForIbssMode::ConfigureProbeResponseTemplate: enable WEP") );

        cap_ptr->SetWepBit();
        }

    // all done now write the MIB
    WHA::StemplateFrame* mib 
        = reinterpret_cast<WHA::StemplateFrame*>(iMemory);
    mib->iFrameType = WHA::KProbeResponseTemplate;
    mib->iInitialTransmitRate = ProbeResponseTxRate( aCtxImpl );
    mib->iLength = length;

    WlanWsaWriteMib& wha_cmd = aCtxImpl.WsaWriteMib();

    TUint32 mib_len( 
        // length of MIB header
        WHA::StemplateFrame::KHeaderSize
        // length of the frame template
        + mib->iLength );

    // align length of MIB to 4-byte boundary
    mib_len = Align4( mib_len );
    
    wha_cmd.Set( 
        aCtxImpl, 
        WHA::KMibTemplateFrame, 
        mib_len, 
        mib );
        
    // change global state: entry procedure triggers action
    ChangeState( aCtxImpl, 
        *this,              // prev state
        wha_cmd             // next state
        );                       
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11PrepareForIbssMode::SetTxRatePolicy( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanDot11PrepareForIbssMode::SetTxRatePolicy(): rate bitmask: 0x%08x"),
        aCtxImpl.RateBitMask() );

    if ( !ConfigureTxRatePolicies( aCtxImpl ) )
        {
        // alloc failue just send abort event to fsm 
        // it takes care of the rest
        Fsm( aCtxImpl, EABORT );
        return;        
        }
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11PrepareForIbssMode::SetBeaconLostCount( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( KUmacDetails, 
        (TUint8*)("UMAC: WlanDot11PrepareForIbssMode::SetBeaconLostCount()") );

    WHA::SbeaconLostCount* mib 
        = reinterpret_cast<WHA::SbeaconLostCount*>(iMemory);

    // disable BSS lost (and thus also BSS regained) indications
    const TUint8 KDisableIndications( 0 );
    mib->iLostCount = KDisableIndications;
        
    WlanWsaWriteMib& wsa_cmd = aCtxImpl.WsaWriteMib();
        
    wsa_cmd.Set( 
        aCtxImpl, WHA::KMibBeaconLostCount, sizeof(*mib), mib );
        
    // change global state: entry procedure triggers action
    ChangeState( aCtxImpl, 
        *this,              // prev state
        wsa_cmd             // next state
        );   
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
void WlanDot11PrepareForIbssMode::Join( 
    WlanContextImpl& aCtxImpl )
    {
    // templates are configured now do a join

    // make WHA types
    WHA::SSSID ssid;
    ssid.iSSIDLength = aCtxImpl.GetSsId().ssidLength;
    os_memcpy( 
        ssid.iSSID, 
        aCtxImpl.GetSsId().ssid, 
        aCtxImpl.GetSsId().ssidLength );
    WHA::TMacAddress mac;
    os_memcpy( 
        &mac, 
        &(aCtxImpl.GetBssId()), 
        WHA::TMacAddress::KMacAddressLength );

    // feed the critter with parameters
    aCtxImpl.WsaJoin().Set( 
        aCtxImpl, 
        aCtxImpl.NetworkOperationMode(), 
        mac, 
        // only 2.4 GHz band is supported for now, so we can hard 
        // code it should be changed as soon as our implemetation 
        // supports multiple bands
        WHA::KBand2dot4GHzMask,
        ssid, 
        aCtxImpl.NetworkChannelNumeber(), 
        aCtxImpl.NetworkBeaconInterval(), 
        aCtxImpl.BasicRateSet(), 
        aCtxImpl.AtimWindow(),  // ATIM
        (aCtxImpl.UseShortPreamble()) 
        ? WHA::EShortPreamble : WHA::ELongPreamble,
        ( aCtxImpl.WHASettings().iCapability 
          & WHA::SSettings::KProbe4Join ) ? ETrue : EFalse );
    
    // change global state: entry procedure triggers action
    ChangeState( aCtxImpl, 
        *this,              // prev state
        aCtxImpl.WsaJoin()  // next state
        );                       
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
void WlanDot11PrepareForIbssMode::ContinueDot11StateTraversal( 
    WlanContextImpl& aCtxImpl )
    {
    // join success continue state traversal
    ChangeState( aCtxImpl, 
        *this,                                  // prev state
        aCtxImpl.iStates.iIbssNormalMode        // next state
        );

    // there exists no valid use case for IBSS join to fail
    // so assume allways success
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
void WlanDot11PrepareForIbssMode::ConfigureTemplate( 
    WlanContextImpl& aCtxImpl,
    TUint32& aLength)
    {    
    // set up the template
    os_memset( iMemory, 0, KAllocLen );

    // set dot11 header

    SManagementFrameHeader* hdr 
        = reinterpret_cast<SManagementFrameHeader*>
        (iMemory + WHA::StemplateFrame::KHeaderSize );

    hdr->iSA = aCtxImpl.iWlanMib.dot11StationId;
    hdr->iBSSID = aCtxImpl.GetBssId();

    // set beacon interval
    TUint16* ptr = 
        (reinterpret_cast<TUint16*>(hdr + 1)) 
        + (KTimeStampFixedFieldLength / (sizeof(*ptr)));
    *ptr = aCtxImpl.NetworkBeaconInterval();

    // set capability information
    SCapabilityInformationField* cap_ptr
        = reinterpret_cast<SCapabilityInformationField*>(++ptr);
    cap_ptr->SetIbss();
    if ( aCtxImpl.UseShortPreamble() )
        {
        cap_ptr->SetShortPreamble();
        }
    else
        {
        cap_ptr->ClearShortPreamble();
        }

    // set SSID
    new (++cap_ptr) SSsIdIE( 
        aCtxImpl.GetSsId().ssid, 
        aCtxImpl.GetSsId().ssidLength );
            
    TUint8* p = reinterpret_cast<TUint8*>(cap_ptr);
    
    p += aCtxImpl.GetSsId().ssidLength + sizeof(SInformationElementHeader);

    // set supported rates IE
    os_memcpy( 
        p, 
        &(aCtxImpl.GetOurSupportedRatesIE()), 
        aCtxImpl.GetOurSupportedRatesIE().GetIeLength() );

    p += aCtxImpl.GetOurSupportedRatesIE().GetIeLength();

    // set DS parameter set IE
    new (p) SDsParameterSetIE( aCtxImpl.NetworkChannelNumeber() );        
    p += sizeof(SDsParameterSetIE);

    // set IBSS parameter set IE
    SIbssParameterSetIE ibssparam( aCtxImpl.AtimWindow() );

    // NOTE: we use memcpy for this as there is no gurantee that 
    // SDsParameterSetIE ends as 16-bit aligned and iValue member is
    // 16-bit long which means we could get an alignment fault
    os_memcpy( p, &ibssparam, sizeof(SIbssParameterSetIE) );
    p += sizeof(SIbssParameterSetIE);
    
    // set extended supported rates IE if it's not empty 
    // 
    if ( aCtxImpl.GetOurExtendedSupportedRatesIE().GetElementLength() )
        {
        os_memcpy( 
            p, 
            &(aCtxImpl.GetOurExtendedSupportedRatesIE()), 
            aCtxImpl.GetOurExtendedSupportedRatesIE().GetIeLength() );
        
            p += aCtxImpl.GetOurExtendedSupportedRatesIE().GetIeLength();
        }        
    
    // store length of the beacon
    aLength = reinterpret_cast<TUint32>(p) - reinterpret_cast<TUint32>(hdr);
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
WHA::TRate WlanDot11PrepareForIbssMode::BeaconTxRate( 
    WlanContextImpl& aCtxImpl ) const
    {
    return aCtxImpl.GetMinBasicRate();
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
WHA::TRate WlanDot11PrepareForIbssMode::ProbeResponseTxRate( 
    WlanContextImpl& aCtxImpl ) const
    {
    return aCtxImpl.GetMinBasicRate();
    }
