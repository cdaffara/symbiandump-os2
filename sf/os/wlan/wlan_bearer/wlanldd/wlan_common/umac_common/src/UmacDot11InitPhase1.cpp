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
* Description:   Implementation of the WlanDot11InitPhase1 class
*
*/

/*
* %version: 36 %
*/

#include "config.h"
#include "UmacDot11InitPhase1.h"
#include "UmacContextImpl.h"
#include "UmacWsaInitiliaze.h"
#include "UmacWsaConfigure.h"
#include "UmacWsaReadMib.h"
#include "umacconfiguretxqueueparams.h"

#ifndef NDEBUG
const TInt8 WlanDot11InitPhase1::iName[] = "dot11-initphase";

const TUint8 WlanDot11InitPhase1::iStateName
    [ESTATEMAX][KMaxStateStringLength] = 
    {
        {"EINIT"}, 
        {"EINITILIAZE"}, 
        {"ECOMPLETEBOOTUP"},
        {"EWAIT4OIDCONFIGURE"},
        {"EHANDLEOIDCONFIGURE"},
        {"ECONFIGURE"},
        {"EREADSTATIONID"},
        {"ECONFTXQUEUE"},
        {"ECONTINUEDOT11TRAVERSE"}
    };

const TUint8 WlanDot11InitPhase1::iEventName
    [EEVENTMAX][KMaxEventStringLength] = 
    {
        {"ESTATEENTRY"}, {"ETXCOMPLETE"}, {"EOIDCONFIGURE"}, {"EABORT"}
    };
#endif

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
WlanDot11InitPhase1::~WlanDot11InitPhase1()
    { 
    iPda = NULL;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
#ifndef NDEBUG 
const TInt8* WlanDot11InitPhase1::GetStateName( 
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
void WlanDot11InitPhase1::Entry( WlanContextImpl& aCtxImpl)
    {
    if ( aCtxImpl.WsaCmdActive() )
        {
        // sanity checking code
        OsAssert( (TUint8*)("UMAC * panic"), 
            (TUint8*)(WLAN_FILE), __LINE__ );
        }

    // no need to do event dispatching in here

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

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11InitPhase1::Exit( WlanContextImpl& aCtxImpl)
    {
    // executed upon dot11 state traversal to another dot11 state
    // reset local statemachine
    // and set user completion code
    aCtxImpl.iStates.iIdleState.Set( KErrNone );
    iState = EINIT; 

    // always deallocate
    OsTracePrint( KInitLevel, 
        (TUint8*)("UMAC * dot11-initphase * free pda memory") );
    os_free( iPda );
    iPda = NULL;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline void WlanDot11InitPhase1::ChangeInternalState( 
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
void WlanDot11InitPhase1::Fsm( 
    WlanContextImpl& aCtxImpl, 
    TEvent aEvent )
    {
    OsTracePrint( KUmacDetails, 
        (TUint8*)("UMAC * dot11-initphase * FSM EVENT") );
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
        case EOIDCONFIGURE:
            OnOIDConfigureEvent( aCtxImpl );
            break;
        case EABORT:
            OnAbortEvent( aCtxImpl );
            break;
        default:
            // catch internal FSM programming error
            OsAssert( (TUint8*)("* UMAC * panic"), 
                (TUint8*)(WLAN_FILE), __LINE__ );
            break;
        }
    }

// -----------------------------------------------------------------------------
// WlanDot11InitPhase1::OnStateEntryEvent
// Handler for state entry event.
// It is guaranteed by the FSM framework that no WHA command is pending
// when this method is entered
// -----------------------------------------------------------------------------
//
void WlanDot11InitPhase1::OnStateEntryEvent( 
    WlanContextImpl& aCtxImpl )
    {
    switch ( iState )
        {
        case EINIT:
            // start the FSM traversal
            ChangeInternalState( aCtxImpl, EINITILIAZE );            
            break;
        case EINITILIAZE:
            Initialize( aCtxImpl );
            break;
        case ECOMPLETEBOOTUP:
            CompleteOid( aCtxImpl );
            ChangeInternalState( aCtxImpl, EWAIT4OIDCONFIGURE );            
            break;
        case EWAIT4OIDCONFIGURE:
            // nothing
            break;
        case EHANDLEOIDCONFIGURE:
            HandleOIDConfigure( aCtxImpl );
            ChangeInternalState( aCtxImpl, ECONFIGURE );            
            break;
        case ECONFIGURE:
            Configure( aCtxImpl );
            break;
        case EREADSTATIONID:
            ReadStationID( aCtxImpl );
            break;
        case ECONFTXQUEUE:
            // configure just the legacy Tx queue    
            ConfigureQueue( aCtxImpl );
            break;
        case ECONTINUEDOT11TRAVERSE:
            // our dot11 transition is fixed and we shall take it
            // change global dot11 state: entry procedure triggers action
            // but first we shall construct a rate lookup table
            aCtxImpl.MakeLookUpTable();

            if ( aCtxImpl.WHASettings().iCapability & 
                 WHA::SSettings::KHtOperation )
                {
                // HT supported by lower layer. As our HT capabilities can't
                // change dynamically, we'll configure our HT capabilities
                // element only once; and we do it here
                ConfigureOurHtCapabilitiesElement( aCtxImpl );
                }
            
            ChangeState( aCtxImpl,
                *this,                                  // prev state
                aCtxImpl.iStates.iMibDefaultConfigure   // next state
                );
            break;
        default:
            // catch internal FSM programming error
            OsTracePrint( KErrorLevel, (TUint8*)("UMAC: state: %d"), iState);        
            OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
        }
    }

// -----------------------------------------------------------------------------
// WlanDot11MibDefaultConfigure::OnTxCompleteEvent
// Handler for tx complete event.
// It is guaranteed by the FSM framework that no WHA command is pending
// when this method is entered
// -----------------------------------------------------------------------------
//
void WlanDot11InitPhase1::OnTxCompleteEvent( 
    WlanContextImpl& aCtxImpl )
    {
    switch ( iState )
        {
        case EINITILIAZE:
            ChangeInternalState( aCtxImpl, ECOMPLETEBOOTUP );
            break;
        case ECONFIGURE:
            ChangeInternalState( aCtxImpl, EREADSTATIONID );            
            break;
        case EREADSTATIONID:
            ChangeInternalState( aCtxImpl, ECONFTXQUEUE );            
            break;
        case ECONFTXQUEUE:
            ChangeInternalState( aCtxImpl, ECONTINUEDOT11TRAVERSE );            
            break;
        default:
            // catch internal FSM programming error
            OsTracePrint( KErrorLevel, (TUint8*)("UMAC: state: %d"), iState);        
            OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
        }
    }

// -----------------------------------------------------------------------------
// simulate macnotresponding error
// -----------------------------------------------------------------------------
//
void WlanDot11InitPhase1::OnAbortEvent( WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( KWarningLevel, (TUint8*)("UMAC * dot11-initphase * abort") );

    DoErrorIndication( aCtxImpl, WHA::KErrorMacNotResponding );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11InitPhase1::OnOIDConfigureEvent( 
    WlanContextImpl& aCtxImpl )
    {
    switch ( iState )
        {
        case EWAIT4OIDCONFIGURE:
            ChangeInternalState( aCtxImpl, EHANDLEOIDCONFIGURE );            
            break;
        default:
            // catch internal FSM programming error
            OsTracePrint( KErrorLevel, (TUint8*)("UMAC * state: %d"), iState);        
            OsAssert( 
                (TUint8*)("UMAC * panic"), (TUint8*)(WLAN_FILE), __LINE__ );
            break;
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11InitPhase1::Initialize( 
    WlanContextImpl& aCtxImpl )
    {
    ChangeState( aCtxImpl, 
        *this,                      // previous state
        aCtxImpl.WsaInitiliaze()    // next state
        );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11InitPhase1::Configure( WlanContextImpl& aCtxImpl )
    {
    // set dot11maxtransmitmsdulifetime
    WHA::SConfigureDataBase* ptr 
        = reinterpret_cast<WHA::SConfigureDataBase*>(iPda);
    ptr->iDot11MaxTransmitMsduLifeTime 
        = aCtxImpl.iWlanMib.dot11MaxTransmitMSDULifetimeDefault;

    // set context for configure
    aCtxImpl.WsaConfigure().Set( aCtxImpl, iPda, iPdaLen );
    ChangeState( aCtxImpl, 
        *this,                  // previous state
        aCtxImpl.WsaConfigure() // next state
        );                       
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11InitPhase1::ReadStationID( WlanContextImpl& aCtxImpl )
    {
    aCtxImpl.WsaReadMib().Set( aCtxImpl, WHA::KMibDot11StationId );
    // change global state: entry procedure triggers action
    ChangeState( aCtxImpl, 
        *this,                  // previous state
        aCtxImpl.WsaReadMib()   // next state
        );                  
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11InitPhase1::CompleteOid( WlanContextImpl& aCtxImpl ) const
    {
    // this is just an oid for us to complete...
    OnOidComplete( aCtxImpl );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11InitPhase1::HandleOIDConfigure( WlanContextImpl& aCtxImpl )
    {
    OnConfigureUmacMib( aCtxImpl, iRTSThreshold, iMaxTxMSDULifetime );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11InitPhase1::ConfigureQueue( 
    WlanContextImpl& aCtxImpl )
    {
    ConfigureTxQueue( aCtxImpl, WHA::ELegacy );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11InitPhase1::OnWhaCommandResponse( 
    WlanContextImpl& aCtxImpl, 
    WHA::TCommandId aCommandId, 
    WHA::TStatus aStatus,
    const WHA::UCommandResponseParams& aCommandResponseParams,
    TUint32 aAct )
    {
    if ( !aAct )
        {
        // this is a response to a command that was generated 
        // by this dot11 state object layer
        if ( aCommandId == WHA::EReadMIBResponse )
            {
            // store station ID to UMAC MIB
            os_memcpy(             
                &(aCtxImpl.iWlanMib.dot11StationId), 
                aCommandResponseParams.iReadMibResponse.iData, 
                sizeof(aCtxImpl.iWlanMib.dot11StationId) );

            // trace the critter
            OsTracePrint( KUmacDetails, 
                (TUint8*)("UMAC * MAC address of the interface: "),
                aCtxImpl.iWlanMib.dot11StationId );
            
            // as station id has been received 
            // we shall configure the frame templates
            ConfigureUmacFrameTemplates( aCtxImpl );
            }
        else    // -- aCommandId == WHA::EReadMIBResponse
            {
            // nothing else is of interest to us
            }
        }
    else    // -- aAct != WlanDot11State::KCompleteManagementRequest
        {
        // this is a response to a command that was not generated 
        // by this dot11 state object layer
        // so lets forward it to the default handler 
        WlanDot11State::OnWhaCommandResponse(
            aCtxImpl, 
            aCommandId, 
            aStatus,
            aCommandResponseParams,
            aAct );
        }        
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11InitPhase1::FinitSystem( WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanDot11InitPhase1::FinitSystem") );

    // if the control comes here it means that we have not been able to 
    // initialize the system, which also means that WHA::Initialize() has not
    // been called yet, but we are nevertheless instructed to stop using the 
    // system. As WHA::Initialize() hasn't been called, there's no need to ask 
    // WHA layer to release its resources (call WHA::Release()) in this 
    // situation. So the only thing we need to do is to complete the WLAN Mgmt 
    // Client request (with ok status)
    OnOidComplete( aCtxImpl );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11InitPhase1::ConfigureUmacFrameTemplates( 
    WlanContextImpl& aCtxImpl ) const
    {
    OsTracePrint( KUmacDetails, 
        (TUint8*)("UMAC: WlanDot11InitPhase1::ConfigureUmacFrameTemplates") );

    // set our station id to association request, 
    // deauthentication, disassociation and reassociation 
    // frame header templates 
    aCtxImpl.GetAssociationRequestFrame().iHeader.iSA 
        = aCtxImpl.iWlanMib.dot11StationId;
    aCtxImpl.GetHtAssociationRequestFrame().iHeader.iSA 
        = aCtxImpl.iWlanMib.dot11StationId;
    aCtxImpl.GetDeauthenticateFrame().iHeader.iSA 
        = aCtxImpl.iWlanMib.dot11StationId;
    aCtxImpl.GetHtDeauthenticateFrame().iHeader.iSA 
        = aCtxImpl.iWlanMib.dot11StationId;
    aCtxImpl.GetDisassociationFrame().iHeader.iSA 
        = aCtxImpl.iWlanMib.dot11StationId;
    aCtxImpl.GetHtDisassociationFrame().iHeader.iSA 
        = aCtxImpl.iWlanMib.dot11StationId;
    aCtxImpl.GetReassociationRequestFrame().iHeader.iSA 
        = aCtxImpl.iWlanMib.dot11StationId;
    aCtxImpl.GetHtReassociationRequestFrame().iHeader.iSA 
        = aCtxImpl.iWlanMib.dot11StationId;

    // set up NULL data frame
    SNullDataFrame& null_data = aCtxImpl.NullDataFrame();
    null_data.iFrameControl.iType 
        = E802Dot11FrameTypeDataNull;
    null_data.iAddress2 = aCtxImpl.iWlanMib.dot11StationId;
    
    // set up QoS NULL data frame
    SHtQosNullDataFrame& qosNullData = aCtxImpl.QosNullDataFrame();
    qosNullData.iQosDataFrameHdr.iHdr.iFrameControl.iType 
        = E802Dot11FrameTypeQosDataNull;
    qosNullData.iQosDataFrameHdr.iHdr.iAddress2 = 
        aCtxImpl.iWlanMib.dot11StationId;
    // reset the QoS control field => normal ack requested
    qosNullData.iQosDataFrameHdr.ResetQosControl();
    // clear the HT control field. Lower layer(s) will set it when applicable
    qosNullData.ResetHtControl();
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11InitPhase1::ConfigureOurHtCapabilitiesElement( 
    WlanContextImpl& aCtxImpl ) const
    {
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanDot11InitPhase1::ConfigureOurHtCapabilitiesElement") );

    // configure HT capabilities info field
    //
    aCtxImpl.GetOurHtCapabilitiesIe().iData.SetLdpcRx( 
        aCtxImpl.WHASettings().iHtCapabilities.iHTCapabilitiesBitMask & 
        WHA::KLdpcRx );
    aCtxImpl.GetOurHtCapabilitiesIe().iData.SetFortyMhzOperation( 
        aCtxImpl.WHASettings().iHtCapabilities.iHTCapabilitiesBitMask & 
        WHA::K40MhzChannel );
    aCtxImpl.GetOurHtCapabilitiesIe().iData.SetSmPowerSave( 
        SHtCapabilitiesIeData::ESmPowerSaveDisabled );
    aCtxImpl.GetOurHtCapabilitiesIe().iData.SetGreenfieldFormat( 
        aCtxImpl.WHASettings().iHtCapabilities.iHTCapabilitiesBitMask & 
        WHA::KGreenfieldFormat );
    aCtxImpl.GetOurHtCapabilitiesIe().iData.SetShortGiFor20Mhz( 
        aCtxImpl.WHASettings().iHtCapabilities.iHTCapabilitiesBitMask & 
        WHA::KShortGiFor20Mhz );
    aCtxImpl.GetOurHtCapabilitiesIe().iData.SetShortGiFor40Mhz( 
        aCtxImpl.WHASettings().iHtCapabilities.iHTCapabilitiesBitMask & 
        WHA::KShortGiFor40Mhz );
    aCtxImpl.GetOurHtCapabilitiesIe().iData.SetStbcTx( 
        aCtxImpl.WHASettings().iHtCapabilities.iHTCapabilitiesBitMask & 
        WHA::KStbcTx );
    aCtxImpl.GetOurHtCapabilitiesIe().iData.SetStbcRx(
            aCtxImpl.WHASettings().iHtCapabilities.iRxStbc );
    aCtxImpl.GetOurHtCapabilitiesIe().iData.SetDelayedBlockAck( 
        aCtxImpl.WHASettings().iHtCapabilities.iHTCapabilitiesBitMask & 
        WHA::KDelayedBlockAck );
    aCtxImpl.GetOurHtCapabilitiesIe().iData.SetMaxAmsduLength(
            aCtxImpl.WHASettings().iHtCapabilities.iMaxAmsdu );
    aCtxImpl.GetOurHtCapabilitiesIe().iData.SetDsssCckIn40Mhz( 
        aCtxImpl.WHASettings().iHtCapabilities.iHTCapabilitiesBitMask & 
        WHA::KDsssCckIn40Mhz );
    aCtxImpl.GetOurHtCapabilitiesIe().iData.SetLsigTxopProtection( 
        aCtxImpl.WHASettings().iHtCapabilities.iHTCapabilitiesBitMask & 
        WHA::KLsigTxopProtection );
    
    // configure A-MPDU parameters field
    //
    aCtxImpl.GetOurHtCapabilitiesIe().iData.SetMaxAmpduLenExponent( 
        aCtxImpl.WHASettings().iHtCapabilities.iMaxAmpdu );
    aCtxImpl.GetOurHtCapabilitiesIe().iData.SetMinMpduStartSpacing( 
        aCtxImpl.WHASettings().iHtCapabilities.iAmpduSpacing );
    
    // configure supported MCS set field
    //
    os_memcpy( 
        aCtxImpl.GetOurHtCapabilitiesIe().iData.iRxMcsBitmask, 
        aCtxImpl.WHASettings().iHtCapabilities.iRxMcs,
        sizeof( aCtxImpl.GetOurHtCapabilitiesIe().iData.iRxMcsBitmask ) );
    aCtxImpl.GetOurHtCapabilitiesIe().iData.SetMaxRxDataRate( 
        aCtxImpl.WHASettings().iHtCapabilities.iRxMaxDataRate );    
    aCtxImpl.GetOurHtCapabilitiesIe().iData.SetTxMcsSetDefined( ETrue );
    aCtxImpl.GetOurHtCapabilitiesIe().iData.SetTxRxMcsSetNotEqual(
        os_memcmp( 
            aCtxImpl.WHASettings().iHtCapabilities.iRxMcs, 
            aCtxImpl.WHASettings().iHtCapabilities.iTxMcs, 
            sizeof( WHA::THtMcsSet ) ) );
    
    // configure HT extended capabilities field
    //
    aCtxImpl.GetOurHtCapabilitiesIe().iData.SetPco( 
        aCtxImpl.WHASettings().iHtCapabilities.iHTCapabilitiesBitMask & 
        WHA::KPco );
    aCtxImpl.GetOurHtCapabilitiesIe().iData.SetPcoTransitionTime( 
        aCtxImpl.WHASettings().iHtCapabilities.iPcoTransTime );    
    aCtxImpl.GetOurHtCapabilitiesIe().iData.SetMcsFeedback( 
        aCtxImpl.WHASettings().iHtCapabilities.iMcsFeedback );    
    aCtxImpl.GetOurHtCapabilitiesIe().iData.SetHtc( 
        aCtxImpl.WHASettings().iHtCapabilities.iHTCapabilitiesBitMask & 
        WHA::KHtcField );
    aCtxImpl.GetOurHtCapabilitiesIe().iData.SetRdResponder( 
        aCtxImpl.WHASettings().iHtCapabilities.iHTCapabilitiesBitMask & 
        WHA::KReverseDirectionResp );
        
    // transmit beamforming capabilities; remains all zero

    // antenna selection capability; remains all zero
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11InitPhase1::BootUp( 
    WlanContextImpl& aCtxImpl,             
    const TUint8* aPda, 
    TUint32 aPdaLength,
    const TUint8* aFw, 
    TUint32 aFwLength )
    {
    // allocate storage for pda
    // pda must remain valid longer as we get dot11maxtransmitmsdulifetime
    // from oid-configure
    OsTracePrint( KInitLevel, 
        (TUint8*)("UMAC * dot11-initphase * FSM EVENT") );

    iPda = static_cast<TUint8*>(os_alloc( aPdaLength ));
    if ( iPda )
        {
        OsTracePrint( KInitLevel, 
            (TUint8*)("UMAC * dot11-initphase * allocate pda memory") );
        OsTracePrint( KInitLevel, (TUint8*)("length: %d"), aPdaLength );

        iPdaLen = aPdaLength;
        os_memcpy( iPda, aPda, iPdaLen );
        // set ctx for fw upload
        aCtxImpl.WsaInitiliaze().Set( aCtxImpl, aFw, aFwLength );
        // manually start the fsm
        Entry( aCtxImpl );
        }
    else
        {
        // allocation failure -> abort
        OsTracePrint( KWarningLevel, (TUint8*)
            ("UMAC * dot11-initphase * pda memory allocation failure") );
        Fsm( aCtxImpl, EABORT );
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11InitPhase1::Configure(
    WlanContextImpl& aCtxImpl,
    TUint32 aRTSThreshold,              
    TUint32 aMaxTxMSDULifetime,
    TUint32 aVoiceCallEntryTimeout,
    TUint32 aVoiceCallEntryTxThreshold,
    TUint32 aVoiceNullTimeout,
    TUint32 aNoVoiceTimeout,
    TUint32 aKeepAliveTimeout,
    TUint32 aSpRcpiIndicationLevel,
    TUint32 aSpTimeToCountPrediction,
    TUint32 aSpMinIndicationInterval )         
    {
    // store parameters and feed an event to the fsm
    iRTSThreshold = aRTSThreshold;              
    iMaxTxMSDULifetime = aMaxTxMSDULifetime;
    aCtxImpl.SetNullSendControllerParameters( 
        aVoiceCallEntryTimeout,
        aVoiceCallEntryTxThreshold,
        aVoiceNullTimeout, 
        aNoVoiceTimeout,
        aKeepAliveTimeout );

    aCtxImpl.ConfigureWlanSignalPredictor(
        aSpTimeToCountPrediction,
        aSpMinIndicationInterval, 
        static_cast<WHA::TRcpi>(aSpRcpiIndicationLevel) );

    Fsm( aCtxImpl, EOIDCONFIGURE );

    // state transition will occur upon this
    return ETrue;
    }
