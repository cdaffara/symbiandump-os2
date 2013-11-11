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
* Description:   Implementation of the WlanDot11MibDefaultConfigure class
*
*/

/*
* %version: 26 %
*/

#include "config.h"
#include "UmacDot11MibDefaultConfigure.h"
#include "wha_mibDefaultvalues.h"
#include "umacwhatodot11typeconverter.h"
#include "UmacWsaWriteMib.h"
#include "UmacContextImpl.h"
    
const TUint KAllocLen = 512; 

#ifndef NDEBUG
const TInt8 WlanDot11MibDefaultConfigure::iName[] 
    = "dot11-mibdefaultconfigure";

const TUint8 WlanDot11MibDefaultConfigure::iStateName
    [ESTATEMAX][KMaxStateStringLength] = 
    {
        {"EINIT"}, 
        {"ESETDOT11MAXRECEIVELIFETIME"}, 
        {"ESETDOT11SLOTTIME"},
        {"ESETDOT11GROUPADDRSTABLE"},
        {"ESETDOT11WEPDEFAULTKEY"},
        {"ESETDOT11CURRENTTXPOWERLEVEL"},
        {"ESETDOT11RTSTHRESHOLD"},
        {"ESETCTSTOSELF"},
        {"ESETARPIPADDRSTABLE"},
        {"ESETPROBEREQUESTTEMPLATE"},
        {"ESETRXFILTER"},
        {"ESETBEACONFILTERIETABLE"},
        {"ESETBEACONFILTERENABLE"},
        {"ESETWLANWAKEUPINTERVAL"},
        {"ESETBEACONLOSTCOUNT"},
        {"ESETRCPITHRESHOLD"},
        {"ESETTXRATEPOLICY"},
        {"ESETHTCAPABILITIES"},
        {"ESETHTBSSOPERATION"},
        {"ESETHTSECONDARYBEACON"},
        {"ESETHTBLOCKACKCONFIGURE"},
        {"ECONTINUEDOT11TRAVERSE"}
    };

const TUint8 WlanDot11MibDefaultConfigure::iEventName
    [EEVENTMAX][KMaxEventStringLength] = 
    {
        {"ESTATEENTRY"}, {"ETXCOMPLETE"}, {"EABORT"}
    };
#endif

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
WlanDot11MibDefaultConfigure::~WlanDot11MibDefaultConfigure()
    {
    iMemory = NULL;
    }

#ifndef NDEBUG 
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
const TInt8* WlanDot11MibDefaultConfigure::GetStateName( 
    TUint8& aLength ) const
    {
    aLength = sizeof( iName );
    return iName;
    }
#endif

// ---------------------------------------------------------
// WlanDot11MibDefaultConfigure::Entry
// (other items were commented in a header).
// ---------------------------------------------------------
//
void WlanDot11MibDefaultConfigure::Entry( 
    WlanContextImpl& aCtxImpl )
    {
    if ( aCtxImpl.WsaCmdActive() )
        {
        // sanity checking code
        OsAssert( (TUint8*)("UMAC * panic"), 
            (TUint8*)(WLAN_FILE), __LINE__ );
        }

    // we don't want to do event dispatching here as we want to set the
    // mib default values without interruptions 

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
// WlanDot11MibDefaultConfigure::Exit
// (other items were commented in a header).
// ---------------------------------------------------------
//
void WlanDot11MibDefaultConfigure::Exit( 
    WlanContextImpl& /*aCtxImpl*/ )
    {
    // we are traversing to a new dot11 state 
    // make sure we don't generate a memory leakage
    os_free( iMemory );
    iState = EINIT; 
    }

// ---------------------------------------------------------
// WlanDot11MibDefaultConfigure::ChangeInternalState
// (other items were commented in a header).
// ---------------------------------------------------------
//
void WlanDot11MibDefaultConfigure::ChangeInternalState( 
    WlanContextImpl& aCtxImpl, 
    TState aNewState )
    {
    iState = aNewState;
    Fsm( aCtxImpl, ESTATEENTRY );
    }

// -----------------------------------------------------------------------------
// WlanDot11MibDefaultConfigure::Fsm
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void WlanDot11MibDefaultConfigure::Fsm( 
    WlanContextImpl& aCtxImpl, 
    TEvent aEvent )
    {
    OsTracePrint( KUmacDetails, 
        (TUint8*)("UMAC: WlanDot11MibDefaultConfigure::Fsm(): FSM EVENT") );
#ifndef NDEBUG
    OsTracePrint( 
        KUmacDetails, 
        (TUint8*)("UMAC: WlanDot11MibDefaultConfigure::Fsm(): event:"));
    OsTracePrint( KUmacDetails, iEventName[aEvent] );
    OsTracePrint( 
        KUmacDetails, 
        (TUint8*)("UMAC: WlanDot11MibDefaultConfigure::Fsm(): state:"));
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
            OsTracePrint( 
                KErrorLevel, 
                (TUint8*)("UMAC: WlanDot11MibDefaultConfigure::Fsm(): event: %d"), aEvent);        
            OsAssert( 
                (TUint8*)("UMAC: WlanDot11MibDefaultConfigure::Fsm(): panic"), 
                (TUint8*)(WLAN_FILE), __LINE__ );
            break;
        }
    }

// -----------------------------------------------------------------------------
// WlanDot11MibDefaultConfigure::OnStateEntryEvent
// Handler for state entry event.
// It is guaranteed by the FSM framework that no WHA command is pending
// when this method is entered
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void WlanDot11MibDefaultConfigure::OnStateEntryEvent( 
    WlanContextImpl& aCtxImpl )
    {
    switch ( iState )
        {
        case EINIT:
            iMemory = os_alloc( KAllocLen );
            if ( iMemory )
                {
                // start the FSM traversal
                ChangeInternalState( aCtxImpl, 
                    ESETDOT11MAXRECEIVELIFETIME );            
                }
            else
                {
                // allocation failure
                Fsm( aCtxImpl, EABORT );
                }
            break;
        case ESETDOT11MAXRECEIVELIFETIME:
            SetDot11MaxReceiveLifeTime( aCtxImpl );
            break;
        case ESETDOT11SLOTTIME:
            SetDot11SlotTime( aCtxImpl );
            break;
        case ESETDOT11GROUPADDRSTABLE:
            SetDot11GroupAddrsTable( aCtxImpl );
            break;
        case ESETDOT11WEPDEFAULTKEY:
            SetDot11WepDefaultKey( aCtxImpl );
            break;
        case ESETDOT11CURRENTTXPOWERLEVEL:
            SetDot11CurrentTxPowerLevel( aCtxImpl );
            break;
        case ESETDOT11RTSTHRESHOLD:
            SetDot11RtsThreshold( aCtxImpl );
            break;
        case ESETCTSTOSELF:
            SetCtsToSelf( aCtxImpl );
            break;
        case ESETARPIPADDRSTABLE:
            SetArpIpAddrsTable( aCtxImpl );
            break;
        case ESETPROBEREQUESTTEMPLATE:
            SetProbeRequestTemplate( aCtxImpl );
            break;
        case ESETRXFILTER:
            SetRxFilter( aCtxImpl );
            break;
        case ESETBEACONFILTERIETABLE:
            SetBeaconFilterIeTable( aCtxImpl );
            break;
        case ESETBEACONFILTERENABLE:
            SetBeaconFilterEnable( aCtxImpl );
            break;
        case ESETWLANWAKEUPINTERVAL:
            SetWlanWakeupInterval( aCtxImpl );
            break;
        case ESETBEACONLOSTCOUNT:
            SetBeaconLostCount( aCtxImpl );
            break;
        case ESETRCPITHRESHOLD:
            SetRcpiThreshold( aCtxImpl );
            break;
        case ESETTXRATEPOLICY:
            SetTxRatePolicy( aCtxImpl );
            break;
        case ESETHTCAPABILITIES:
            SetHtCapabilities( aCtxImpl );
            break;
        case ESETHTBSSOPERATION:
            SetHtBssOperation( aCtxImpl );
            break;
        case ESETHTSECONDARYBEACON:
            SetHtSecondaryBeacon( aCtxImpl );
            break;
        case ESETHTBLOCKACKCONFIGURE:
            SetHtBlockAckConfigure( aCtxImpl );
            break;
        case ECONTINUEDOT11TRAVERSE:
            // our dot11 transition is fixed and we shall take it
            // change global dot11 state: entry procedure triggers action
            ChangeState( aCtxImpl,              
                *this,                          // prev state
                aCtxImpl.iStates.iIdleState     // next state
                );
            break;
        default:
            // catch internal FSM programming error
            OsTracePrint( KErrorLevel, (TUint8*)
                ("UMAC: WlanDot11MibDefaultConfigure::OnStateEntryEvent(): state: %d"), 
                iState);        
            OsAssert( (TUint8*)
                ("UMAC: WlanDot11MibDefaultConfigure::OnStateEntryEvent(): panic"), 
                (TUint8*)(WLAN_FILE), __LINE__ );
            break;
        }
    }

// -----------------------------------------------------------------------------
// Handler for tx complete event.
// It is guaranteed by the FSM framework that no WHA command is pending
// when this method is entered
// -----------------------------------------------------------------------------
//
void WlanDot11MibDefaultConfigure::OnTxCompleteEvent( 
    WlanContextImpl& aCtxImpl )
    {
    switch ( iState )
        {
        case ESETDOT11MAXRECEIVELIFETIME:
            if ( aCtxImpl.WHASettings().iCapability 
                & WHA::SSettings::KDot11SlotTime )
                {
                // supported
                ChangeInternalState( 
                    aCtxImpl, ESETDOT11SLOTTIME );
                }
            else
                {
                OsTracePrint( KWarningLevel, (TUint8*)
                    ("UMAC * dot11-mibdefaultconfigure"));
                OsTracePrint( KWarningLevel, (TUint8*)
                    ("no support for dot11slottime mib skipping"));
                ChangeInternalState( aCtxImpl, 
                    ESETDOT11GROUPADDRSTABLE );
                }
            break;
        case ESETDOT11SLOTTIME:
            ChangeInternalState( aCtxImpl, 
                ESETDOT11GROUPADDRSTABLE );
            break;
        case ESETDOT11GROUPADDRSTABLE:
            ChangeInternalState( aCtxImpl, 
                ESETDOT11WEPDEFAULTKEY );
            break;
        case ESETDOT11WEPDEFAULTKEY:
            ChangeInternalState( aCtxImpl, 
                ESETDOT11CURRENTTXPOWERLEVEL );
            break;
        case ESETDOT11CURRENTTXPOWERLEVEL:
            ChangeInternalState( aCtxImpl, 
                ESETDOT11RTSTHRESHOLD );
            break;
        case ESETDOT11RTSTHRESHOLD:
            ChangeInternalState( aCtxImpl, 
                ESETCTSTOSELF );
            break;
        case ESETCTSTOSELF:
            ChangeInternalState( aCtxImpl, 
                ESETARPIPADDRSTABLE );
            break;
        case ESETARPIPADDRSTABLE:
            ChangeInternalState( aCtxImpl, 
                ESETPROBEREQUESTTEMPLATE );
            break;
        case ESETPROBEREQUESTTEMPLATE:
            ChangeInternalState( aCtxImpl, 
                ESETRXFILTER );
            break;
        case ESETRXFILTER:
            ChangeInternalState( aCtxImpl, 
                ESETBEACONFILTERIETABLE );
            break;
        case ESETBEACONFILTERIETABLE:
            ChangeInternalState( aCtxImpl, 
                ESETBEACONFILTERENABLE );
            break;
        case ESETBEACONFILTERENABLE:
            ChangeInternalState( aCtxImpl, 
                ESETWLANWAKEUPINTERVAL );
            break;
        case ESETWLANWAKEUPINTERVAL:
            ChangeInternalState( aCtxImpl, 
                ESETBEACONLOSTCOUNT );
            break;
        case ESETBEACONLOSTCOUNT:
            ChangeInternalState( aCtxImpl, 
                ESETRCPITHRESHOLD );
            break;
        case ESETRCPITHRESHOLD:
            ChangeInternalState( aCtxImpl, 
                ESETTXRATEPOLICY );
            break;
        case ESETTXRATEPOLICY:
            if ( aCtxImpl.WHASettings().iCapability 
                & WHA::SSettings::KHtOperation )
                {
                // supported
                ChangeInternalState( 
                    aCtxImpl, ESETHTCAPABILITIES );
                }
            else
                {
                // HT not supported, so we will skip setting the HT related
                // MIBs
                OsTracePrint( KWarningLevel, (TUint8*)
                    ("UMAC: WlanDot11MibDefaultConfigure::OnTxCompleteEvent: no HT support, skipping HT related mibs"));
                ChangeInternalState( aCtxImpl, 
                    ECONTINUEDOT11TRAVERSE );
                }
            break;
        case ESETHTCAPABILITIES:
            ChangeInternalState( aCtxImpl, 
                ESETHTBSSOPERATION );
            break;
        case ESETHTBSSOPERATION:
            ChangeInternalState( aCtxImpl, 
                ESETHTSECONDARYBEACON );
            break;
        case ESETHTSECONDARYBEACON:
            ChangeInternalState( aCtxImpl, 
                ESETHTBLOCKACKCONFIGURE );            
            break;
        case ESETHTBLOCKACKCONFIGURE:
            ChangeInternalState( aCtxImpl, 
                ECONTINUEDOT11TRAVERSE );            
            break;
        default:
            // catch internal FSM programming error
            OsTracePrint( 
                KErrorLevel, 
                (TUint8*)("UMAC: WlanDot11MibDefaultConfigure::OnTxCompleteEvent(): state: %d"), 
                iState);        
            OsAssert( 
                (TUint8*)("UMAC: UMAC: WlanDot11MibDefaultConfigure::OnTxCompleteEvent(): panic"), 
                (TUint8*)(WLAN_FILE), __LINE__ );
            break;
        }
    }

// ---------------------------------------------------------
// WlanDot11MibDefaultConfigure::OnAbortEvent
// simulate macnotresponding error
// (other items were commented in a header).
// ---------------------------------------------------------
//
void WlanDot11MibDefaultConfigure::OnAbortEvent( 
    WlanContextImpl& aCtxImpl )
    {
    // memory is released by Exit() method
    OsTracePrint( KWarningLevel, 
        (TUint8*)("UMAC * dot11-mibdefaultconfigure * abort") );

    DoErrorIndication( aCtxImpl, WHA::KErrorMacNotResponding );
    }

// ---------------------------------------------------------
// WlanDot11MibDefaultConfigure::SetDot11MaxReceiveLifeTime
// (other items were commented in a header).
// ---------------------------------------------------------
//
void WlanDot11MibDefaultConfigure::SetDot11MaxReceiveLifeTime( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( KUmacDetails, 
        (TUint8*)("UMAC: WlanDot11MibDefaultConfigure::SetDot11MaxReceiveLifeTime()") );

    WHA::Sdot11MaxReceiveLifeTime* mib 
        = static_cast<WHA::Sdot11MaxReceiveLifeTime*>(iMemory);
    *mib = WHA::KDot11MaxReceiveLifeTimeMibDefault;
        
    WlanWsaWriteMib& wsa_cmd = aCtxImpl.WsaWriteMib();
        
    wsa_cmd.Set( 
        aCtxImpl, WHA::KMibDot11MaxReceiveLifetime, sizeof(*mib), mib );
        
    // change global state: entry procedure triggers action
    ChangeState( aCtxImpl, 
        *this,              // prev state
        wsa_cmd             // next state
        );   
    }

// ---------------------------------------------------------
// WlanDot11MibDefaultConfigure::SetDot11SlotTime
// (other items were commented in a header).
// ---------------------------------------------------------
//
void WlanDot11MibDefaultConfigure::SetDot11SlotTime( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( KUmacDetails, 
        (TUint8*)("UMAC: WlanDot11MibDefaultConfigure::SetDot11SlotTime()") );

    WHA::Sdot11SlotTime* mib
        = static_cast<WHA::Sdot11SlotTime*>(iMemory);
    *mib = WHA::KDot11SlotTimeMibDefault;
        
    WlanWsaWriteMib& wsa_cmd = aCtxImpl.WsaWriteMib();
        
    wsa_cmd.Set( 
        aCtxImpl, WHA::KMibDot11SlotTime, sizeof(*mib), mib );
        
    // change global state: entry procedure triggers action
    ChangeState( aCtxImpl, 
        *this,              // prev state
        wsa_cmd             // next state
        );   
    }

// ---------------------------------------------------------
// WlanDot11MibDefaultConfigure::SetDot11GroupAddrsTable
// (other items were commented in a header).
// ---------------------------------------------------------
//
void WlanDot11MibDefaultConfigure::SetDot11GroupAddrsTable( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( KUmacDetails, 
        (TUint8*)("UMAC: WlanDot11MibDefaultConfigure::SetDot11GroupAddrsTable()") );

    WHA::Sdot11GroupAddressesTable* mib
        = static_cast<WHA::Sdot11GroupAddressesTable*>(iMemory);
    *mib = WHA::KDot11GroupAddressesTableMibDefault;
        
    WlanWsaWriteMib& wsa_cmd = aCtxImpl.WsaWriteMib();
        
    wsa_cmd.Set( 
        aCtxImpl, WHA::KMibDot11GroupAddressesTable, sizeof(*mib), mib );
        
    // change global state: entry procedure triggers action
    ChangeState( aCtxImpl, 
        *this,              // prev state
        wsa_cmd             // next state
        );   
    }

// ---------------------------------------------------------
// WlanDot11MibDefaultConfigure::SetDot11WepDefaultKey
// (other items were commented in a header).
// ---------------------------------------------------------
//
void WlanDot11MibDefaultConfigure::SetDot11WepDefaultKey( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( KUmacDetails, 
        (TUint8*)("UMAC: WlanDot11MibDefaultConfigure::SetDot11WepDefaultKey()") );

    WHA::Sdot11WepDefaultKeyId* mib
        = static_cast<WHA::Sdot11WepDefaultKeyId*>(iMemory);
    *mib = WHA::KDot11WepDefaultKeyIdMib;
        
    WlanWsaWriteMib& wsa_cmd = aCtxImpl.WsaWriteMib();
        
    wsa_cmd.Set( 
        aCtxImpl, WHA::KMibDot11WepDefaultKeyId, sizeof(*mib), mib );
        
    // change global state: entry procedure triggers action
    ChangeState( aCtxImpl, 
        *this,              // prev state
        wsa_cmd             // next state
        );   
    }

// ---------------------------------------------------------
// WlanDot11MibDefaultConfigure::SetDot11CurrentTxPowerLevel
// (other items were commented in a header).
// ---------------------------------------------------------
//
void WlanDot11MibDefaultConfigure::SetDot11CurrentTxPowerLevel( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( KUmacDetails, 
        (TUint8*)("UMAC: WlanDot11MibDefaultConfigure::SetDot11CurrentTxPowerLevel()") );

    WHA::Sdot11CurrentTxPowerLevel* mib 
        = static_cast<WHA::Sdot11CurrentTxPowerLevel*>(iMemory);
    *mib = WHA::KDot11CurrentTxPowerLevelMibDefault;

    // store the new power level also to our soft mib
    aCtxImpl.iWlanMib.dot11CurrentTxPowerLevel 
        = mib->iDot11CurrentTxPowerLevel;
        
    WlanWsaWriteMib& wsa_cmd = aCtxImpl.WsaWriteMib();
        
    wsa_cmd.Set( 
        aCtxImpl, WHA::KMibDot11CurrentTxPowerLevel, sizeof(*mib), mib );
        
    // change global state: entry procedure triggers action
    ChangeState( aCtxImpl, 
        *this,              // prev state
        wsa_cmd             // next state
        );   
    }

// ---------------------------------------------------------
// WlanDot11MibDefaultConfigure::SetDot11RtsThreshold
// (other items were commented in a header).
// ---------------------------------------------------------
//
void WlanDot11MibDefaultConfigure::SetDot11RtsThreshold( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( KUmacDetails, 
        (TUint8*)("UMAC: WlanDot11MibDefaultConfigure::SetDot11RtsThreshold()") );
    
    WHA::Sdot11RTSThreshold* mib 
        = static_cast<WHA::Sdot11RTSThreshold*>(iMemory);
    mib->iDot11RTSThreshold = aCtxImpl.iWlanMib.dot11RTSThreshold; 
   
    WlanWsaWriteMib& wsa_cmd = aCtxImpl.WsaWriteMib();
        
    wsa_cmd.Set( 
        aCtxImpl, WHA::KMibDot11RTSThreshold, sizeof(*mib), mib );
        
    // change global state: entry procedure triggers action
    ChangeState( aCtxImpl, 
        *this,              // prev state
        wsa_cmd             // next state
        );   
    }

// ---------------------------------------------------------
// WlanDot11MibDefaultConfigure::SetCtsToSelf
// (other items were commented in a header).
// ---------------------------------------------------------
//
void WlanDot11MibDefaultConfigure::SetCtsToSelf( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( KUmacDetails, 
        (TUint8*)("UMAC: WlanDot11MibDefaultConfigure::SetCtsToSelf()") );

    WHA::SctsToSelf* mib 
        = static_cast<WHA::SctsToSelf*>(iMemory);
    *mib = WHA::KCtsToSelfMibDefault; 
        
    WlanWsaWriteMib& wsa_cmd = aCtxImpl.WsaWriteMib();
        
    wsa_cmd.Set( 
        aCtxImpl, WHA::KMibCtsToSelf, sizeof(*mib), mib );
        
    // change global state: entry procedure triggers action
    ChangeState( aCtxImpl, 
        *this,              // prev state
        wsa_cmd             // next state
        );   
    }

// ---------------------------------------------------------
// WlanDot11MibDefaultConfigure::SetArpIpAddrsTable
// (other items were commented in a header).
// ---------------------------------------------------------
//
void WlanDot11MibDefaultConfigure::SetArpIpAddrsTable( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( KUmacDetails, 
        (TUint8*)("UMAC: WlanDot11MibDefaultConfigure::SetArpIpAddrsTable(): ") );

    WHA::SarpIpAddressTable* mib 
        = static_cast<WHA::SarpIpAddressTable*>(iMemory);
    *mib = WHA::KArpIpAddressTableMibDefault; 
        
    WlanWsaWriteMib& wsa_cmd = aCtxImpl.WsaWriteMib();
        
    wsa_cmd.Set( 
        aCtxImpl, WHA::KMibArpIpAddressTable, sizeof(*mib), mib );
        
    // change global state: entry procedure triggers action
    ChangeState( aCtxImpl, 
        *this,              // prev state
        wsa_cmd             // next state
        );   
    }

// ---------------------------------------------------------
// WlanDot11MibDefaultConfigure::SetProbeRequestTemplate
// (other items were commented in a header).
// ---------------------------------------------------------
//
void WlanDot11MibDefaultConfigure::SetProbeRequestTemplate( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanDot11MibDefaultConfigure::SetProbeRequestTemplate") );
    
    SSupportedRatesIE s_ie;
    const TUint8 KHtCapabilitiesIeLength( 
        aCtxImpl.GetOurHtCapabilitiesIe().GetIeLength() );
    
    // we don't want to create this IE to stack
    // as it is a big one
    SExtendedSupportedRatesIE* ext_s_ie(
        static_cast<SExtendedSupportedRatesIE*>
        (os_alloc( sizeof(SExtendedSupportedRatesIE) )));
    if ( !ext_s_ie )
        {
        // alloc failue; just send abort to fsm. It takes care of the rest
        Fsm( aCtxImpl, EABORT );
        return;
        }

    new (ext_s_ie) SExtendedSupportedRatesIE;
    
    // construct rate IEs from WHA rates this device supports...
    WlanWhaToDot11TypeConverter::Convert( 
        aCtxImpl.WHASettings().iRates,
        s_ie, 
        *ext_s_ie );

    // length of the dot11 probe request frame template to be written
    const TUint32 frame_template_len(
        // dot11 management header
        sizeof(SManagementFrameHeader) +
        // an empty SSID field holds just the IE header
        sizeof(SInformationElementHeader) +
        // standard supported rate IE length
        s_ie.GetIeLength() +
        // lenght of the HT capabilities element, which is only added if lower
        // layers support HT
        (aCtxImpl.WHASettings().iCapability & WHA::SSettings::KHtOperation ?
            KHtCapabilitiesIeLength : 0 ) +
        // if extended rate IE has any elements we shall append it
        // otherwise not 
        ((ext_s_ie->GetElementLength()) ? ext_s_ie->GetIeLength() : 0) +
        // if lower layers support DS Parameter Set IE in probe request,
        // we will append it; otherwise not
        (aCtxImpl.WHASettings().iCapability & 
            WHA::SSettings::KDsParamSetIeInProbe ? 
                sizeof( SDsParameterSetIE ) : 0 ) );

    // total length of the MIB to be written
    TUint32 mib_len( 
        // length of the dot11 probe request frame template to be written
        frame_template_len 
        // ...and the MIB header
        + WHA::StemplateFrame::KHeaderSize );

    // align length of MIB to 4-byte boundary
    mib_len = Align4( mib_len );

    if ( mib_len > KAllocLen )
        {
        // this is a programming error
        OsTracePrint( KErrorLevel, 
            (TUint8*)("UMAC: mib_len %d"), mib_len);        
        OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
        }

    // start of usable memory
    WHA::StemplateFrame* mib_hdr( static_cast<WHA::StemplateFrame*>(iMemory) );

    // --- begin: set MIB header 

    mib_hdr->iFrameType = WHA::KProbeRequestTemplate;
    // this field is a don't care for probe request
    mib_hdr->iInitialTransmitRate = 0;  
    mib_hdr->iLength = frame_template_len;

    // --- end: set MIB header 

    // --- begin: set dot11 frame header 

    SManagementFrameHeader* frame_hdr( 
        reinterpret_cast<SManagementFrameHeader*>(mib_hdr->iTemplateData) );
    new (frame_hdr) SManagementFrameHeader( E802Dot11FrameTypeProbeReq ); 
    frame_hdr->iDA = KBroadcastMacAddr;
    frame_hdr->iSA = aCtxImpl.iWlanMib.dot11StationId;
    frame_hdr->iBSSID = KBroadcastMacAddr; 

    // --- end: set dot11 frame header 

    // --- begin: set IEs 

    TUint8* ptr( reinterpret_cast<TUint8*>(++frame_hdr) );

    // set an empty SSID 
    new (ptr) SSsIdIE;
    // as we have an empty SSID its length only holds the 
    // information element header length
    ptr += sizeof( SInformationElementHeader );

    // copy supported rates IE
    os_memcpy( 
        ptr, &s_ie, s_ie.GetIeLength() );
    ptr += s_ie.GetIeLength();

    // add DS Parameter Set IE, if possible
    if ( aCtxImpl.WHASettings().iCapability & 
         WHA::SSettings::KDsParamSetIeInProbe )
        {
        new (ptr) SDsParameterSetIE( 0 ); // initialize to ch zero
        ptr += sizeof( SDsParameterSetIE );

        OsTracePrint( KUmacDetails, (TUint8*)
            ("UMAC: DS Param Set IE added") );
        }

    if ( aCtxImpl.WHASettings().iCapability & 
         WHA::SSettings::KHtOperation )
        {
        // copy HT capabilities element
        os_memcpy( 
            ptr, 
            &(aCtxImpl.GetOurHtCapabilitiesIe()), 
            KHtCapabilitiesIeLength );
    
        ptr += KHtCapabilitiesIeLength;

        OsTracePrint( KUmacDetails, (TUint8*)
            ("UMAC: HT capabilities element added") );
        }

    // copy extended supported rates IE if present
    if ( ext_s_ie->GetElementLength() )
        {
        os_memcpy( ptr, ext_s_ie, ext_s_ie->GetIeLength() );
        }

    // --- end: set IEs 

    // and now execute

    WlanWsaWriteMib& wsa_cmd( aCtxImpl.WsaWriteMib() );
        
    wsa_cmd.Set( 
        aCtxImpl, WHA::KMibTemplateFrame, mib_len, mib_hdr );
        
    // change global state: entry procedure triggers action
    ChangeState( aCtxImpl, 
        *this,              // prev state
        wsa_cmd             // next state
        );   

    // as the parameters have been supplied we can now deallocate
    os_free( ext_s_ie );
    }

// ---------------------------------------------------------
// WlanDot11MibDefaultConfigure::SetRxFilter
// (other items were commented in a header).
// ---------------------------------------------------------
//
void WlanDot11MibDefaultConfigure::SetRxFilter( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( KUmacDetails, 
        (TUint8*)("UMAC: WlanDot11MibDefaultConfigure::SetRxFilter()") );

    WHA::SrxFilter* mib 
        = static_cast<WHA::SrxFilter*>(iMemory);
    *mib = WHA::KRxFilterMibDefault;
        
    WlanWsaWriteMib& wsa_cmd = aCtxImpl.WsaWriteMib();
        
    wsa_cmd.Set( 
        aCtxImpl, WHA::KMibRxFilter, sizeof(*mib), mib );
        
    // change global state: entry procedure triggers action
    ChangeState( aCtxImpl, 
        *this,              // prev state
        wsa_cmd             // next state
        );   
    }

// ---------------------------------------------------------
// WlanDot11MibDefaultConfigure::SetBeaconFilterIeTable
// (other items were commented in a header).
// ---------------------------------------------------------
//
void WlanDot11MibDefaultConfigure::SetBeaconFilterIeTable( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( KUmacDetails, 
        (TUint8*)("UMAC: WlanDot11MibDefaultConfigure::SetBeaconFilterIeTable()") );

    WHA::SbeaconFilterIeTable* mib 
        = static_cast<WHA::SbeaconFilterIeTable*>(iMemory);
    *mib = WHA::KBeaconFilterIeTableMibDefault;
        
    WlanWsaWriteMib& wsa_cmd = aCtxImpl.WsaWriteMib();
        
    wsa_cmd.Set( 
        aCtxImpl, WHA::KMibBeaconFilterIeTable, sizeof(*mib), mib );
        
    // change global state: entry procedure triggers action
    ChangeState( aCtxImpl, 
        *this,              // prev state
        wsa_cmd             // next state
        );   
    }

// ---------------------------------------------------------
// WlanDot11MibDefaultConfigure::SetBeaconFilterEnable
// (other items were commented in a header).
// ---------------------------------------------------------
//
void WlanDot11MibDefaultConfigure::SetBeaconFilterEnable( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( KUmacDetails, 
        (TUint8*)("UMAC: WlanDot11MibDefaultConfigure::SetBeaconFilterEnable()") );

    WHA::SbeaconFilterEnable* mib 
        = static_cast<WHA::SbeaconFilterEnable*>(iMemory);
    *mib = WHA::KBeaconFilterEnableMibDefault;
        
    WlanWsaWriteMib& wsa_cmd = aCtxImpl.WsaWriteMib();
        
    wsa_cmd.Set( 
        aCtxImpl, WHA::KMibBeaconFilterEnable, sizeof(*mib), mib );
        
    // change global state: entry procedure triggers action
    ChangeState( aCtxImpl, 
        *this,              // prev state
        wsa_cmd             // next state
        );   
    }

// ---------------------------------------------------------
// WlanDot11MibDefaultConfigure::SetWlanWakeupInterval
// (other items were commented in a header).
// ---------------------------------------------------------
//
void WlanDot11MibDefaultConfigure::SetWlanWakeupInterval( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( KUmacDetails, 
        (TUint8*)("UMAC: WlanDot11MibDefaultConfigure::SetWlanWakeupInterval()") );

    WHA::SwlanWakeUpInterval* mib 
        = static_cast<WHA::SwlanWakeUpInterval*>(iMemory);
    *mib = WHA::KWlanWakeUpIntervalMibDefault;
        
    WlanWsaWriteMib& wsa_cmd = aCtxImpl.WsaWriteMib();
        
    wsa_cmd.Set( 
        aCtxImpl, WHA::KMibWlanWakeUpInterval, sizeof(*mib), mib );
        
    // store the new setting also locally
    aCtxImpl.iWlanMib.iWlanWakeupInterval = mib->iMode;
    aCtxImpl.iWlanMib.iWlanListenInterval = mib->iListenInterval;

    // change global state: entry procedure triggers action
    ChangeState( aCtxImpl, 
        *this,              // prev state
        wsa_cmd             // next state
        );   
    }

// ---------------------------------------------------------
// WlanDot11MibDefaultConfigure::SetBeaconLostCount
// (other items were commented in a header).
// ---------------------------------------------------------
//
void WlanDot11MibDefaultConfigure::SetBeaconLostCount( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( KUmacDetails, 
        (TUint8*)("UMAC: WlanDot11MibDefaultConfigure::SetBeaconLostCount()") );

    WHA::SbeaconLostCount* mib 
        = static_cast<WHA::SbeaconLostCount*>(iMemory);

    // if the beacon lost count has been set via the management interface,
    // that value is used (instead of the original default)
    mib->iLostCount = aCtxImpl.iWlanMib.iBeaconLostCount;
        
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
// WlanDot11MibDefaultConfigure::SetRcpiThreshold
// (other items were commented in a header).
// ---------------------------------------------------------
//
void WlanDot11MibDefaultConfigure::SetRcpiThreshold( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( KUmacDetails, 
        (TUint8*)("UMAC: WlanDot11MibDefaultConfigure::SetRcpiThreshold()") );

    WHA::SrcpiThreshold* mib 
        = static_cast<WHA::SrcpiThreshold*>(iMemory);
    *mib = WHA::KRcpiThresholdMibDefault;
        
    WlanWsaWriteMib& wsa_cmd = aCtxImpl.WsaWriteMib();
        
    wsa_cmd.Set( 
        aCtxImpl, WHA::KMibRcpiThreshold, sizeof(*mib), mib );
        
    // change global state: entry procedure triggers action
    ChangeState( aCtxImpl, 
        *this,              // prev state
        wsa_cmd             // next state
        );   
    }

// ---------------------------------------------------------
// WlanDot11MibDefaultConfigure::SetTxRatePolicy
// (other items were commented in a header).
// ---------------------------------------------------------
//
void WlanDot11MibDefaultConfigure::SetTxRatePolicy( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( KUmacDetails, 
        (TUint8*)("UMAC: WlanDot11MibDefaultConfigure::SetTxRatePolicy()") );

    // we define 1 global policy ...

    WHA::StxRatePolicy* mib = static_cast<WHA::StxRatePolicy*>(iMemory);

    // ... which is our default ...
    *mib = WHA::KTxRatePolicyMibDefault;
    
    WlanWsaWriteMib& wsa_cmd = aCtxImpl.WsaWriteMib();

    wsa_cmd.Set( 
        aCtxImpl, 
        WHA::KMibTxRatePolicy, 
        sizeof( *mib ), 
        mib );
        
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
void WlanDot11MibDefaultConfigure::SetHtCapabilities( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanDot11MibDefaultConfigure::SetHtCapabilities") );

    WHA::ShtCapabilities* mib 
        = static_cast<WHA::ShtCapabilities*>(iMemory);
    *mib = WHA::KHtCapabilitiesMibDefault;
        
    WlanWsaWriteMib& wsa_cmd = aCtxImpl.WsaWriteMib();
        
    wsa_cmd.Set( 
        aCtxImpl, WHA::KMibHtCapabilities, sizeof( *mib ), mib );
        
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
void WlanDot11MibDefaultConfigure::SetHtBssOperation( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanDot11MibDefaultConfigure::SetHtBssOperation") );

    WHA::ShtBssOperation* mib 
        = static_cast<WHA::ShtBssOperation*>(iMemory);
    *mib = WHA::KHtBssOperationMibDefault;
        
    WlanWsaWriteMib& wsa_cmd = aCtxImpl.WsaWriteMib();
        
    wsa_cmd.Set( 
        aCtxImpl, WHA::KMibHtBssOperation, sizeof( *mib ), mib );
        
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
void WlanDot11MibDefaultConfigure::SetHtSecondaryBeacon( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanDot11MibDefaultConfigure::SetHtSecondaryBeacon") );

    WHA::ShtSecondaryBeacon* mib 
        = static_cast<WHA::ShtSecondaryBeacon*>(iMemory);
    *mib = WHA::KHtSecondaryBeaconMibDefault;
        
    WlanWsaWriteMib& wsa_cmd = aCtxImpl.WsaWriteMib();
        
    wsa_cmd.Set( 
        aCtxImpl, WHA::KMibHtSecondaryBeacon, sizeof( *mib ), mib );
        
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
void WlanDot11MibDefaultConfigure::SetHtBlockAckConfigure( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanDot11MibDefaultConfigure::SetHtBlockAckConfigure") );

    WHA::ShtBlockAckConfigure* mib 
        = static_cast<WHA::ShtBlockAckConfigure*>(iMemory);
    *mib = WHA::KHtBlockAckConfigureMibDefault;
        
    WlanWsaWriteMib& wsa_cmd = aCtxImpl.WsaWriteMib();
        
    wsa_cmd.Set( 
        aCtxImpl, WHA::KMibHtBlockAckConfigure, sizeof( *mib ), mib );
        
    // change global state: entry procedure triggers action
    ChangeState( aCtxImpl, 
        *this,              // prev state
        wsa_cmd             // next state
        );
    }
