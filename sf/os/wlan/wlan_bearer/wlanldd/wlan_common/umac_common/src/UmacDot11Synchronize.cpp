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
* Description:   Implementation of the UmacDot11Synchronize class
*
*/

/*
* %version: 31 %
*/

#include "config.h"
#include "UmacDot11Synchronize.h"
#include "UmacContextImpl.h"
#include "UmacWsaWriteMib.h"
#include "UmacWsaJoin.h"
#include "umacconfiguretxqueueparams.h"
#include "UmacWsaAddKey.h"
#include "wha_mibDefaultvalues.h"

#ifndef NDEBUG
const TInt8 WlanDot11Synchronize::iName[] = "dot11-synchronize";

const TUint8 WlanDot11Synchronize::iStateName
    [ESTATEMAX][KMaxStateStringLength] = 
    {
        {"EINIT"}, 
        {"ESETDOT11SLOTTIME"}, 
        {"ESETCTSTOSELF"},
        {"ECONFTXQUEUE"},
        {"ECONFTXQUEUEPARAMS"},
        {"ESETTXRATEPOLICY"},
        {"ESETHTCAPABILITIES"},
        {"ESETHTBSSOPERATION"},
        {"ERESETHTCAPABILITIES"},
        {"EISSUEJOIN"}, 
        {"ESETHTBLOCKACKCONF"},
        {"ERESETHTBLOCKACKCONF"},
        {"ESETPAIRWISEKEY"},
        {"ECONTINUEDOT11TRAVERSE"}
    };

const TUint8 WlanDot11Synchronize::iEventName
    [EEVENTMAX][KMaxEventStringLength] = 
    {
        {"ESTATEENTRY"}, 
        {"ETXCOMPLETE"},
        {"EABORT"}
    };
#endif
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
WlanDot11Synchronize::WlanDot11Synchronize() : 
    iState( EINIT ), iJoinStatus ( KErrNone ) 
    {
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
WlanDot11Synchronize::~WlanDot11Synchronize()
    {
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
#ifndef NDEBUG 
const TInt8* WlanDot11Synchronize::GetStateName( TUint8& aLength ) const
    {
    aLength = sizeof( iName );
    return iName;
    }
#endif

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11Synchronize::Entry( WlanContextImpl& aCtxImpl )
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

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11Synchronize::Exit( WlanContextImpl& /*aCtxImpl*/ )
    {
    OsTracePrint( 
        KUmacProtocolState, 
        (TUint8*)("UMAC: WlanDot11Synchronize::Exit()"));

    // we are departing this dot11state to another dot11state, so
    // we simple reset our local FSM for the next time...
    iState = EINIT;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11Synchronize::Fsm( 
    WlanContextImpl& aCtxImpl, 
    TEvent aEvent )
    {
#ifndef NDEBUG
    OsTracePrint( KUmacDetails, (TUint8*)("UMAC: WlanDot11Synchronize::Fsm(): event: "));
    OsTracePrint( KUmacDetails, iEventName[aEvent] );
    OsTracePrint( KUmacDetails, (TUint8*)("UMAC: WlanDot11Synchronize::Fsm(): state: ")); 
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
            OsTracePrint( 
                KErrorLevel, 
                (TUint8*)("UMAC: WlanDot11Synchronize::Fsm(): event: %d"), 
                aEvent);        
            OsAssert( 
                (TUint8*)("UMAC: WlanDot11Synchronize::Fsm(): panic"), 
                (TUint8*)(WLAN_FILE), __LINE__ );
            break;
        }
    }

// -----------------------------------------------------------------------------
// Handler for state entry event.
// -----------------------------------------------------------------------------
//
void WlanDot11Synchronize::OnStateEntryEvent( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( 
        KUmacDetails, 
        (TUint8*)("UMAC: WlanDot11Synchronize::OnStateEntryEvent()"));

    switch ( iState )
        {
        case EINIT:            
            if ( ( iJoinStatus = InitActions( aCtxImpl ) ) == KErrNone )
                {                
                // we meet the requirements of the network so we can continue
                
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
                        ("UMAC * dot11-synchronize"));
                    OsTracePrint( KWarningLevel, (TUint8*)
                        ("no support for dot11slottime mib skipping"));

                    ChangeInternalState( aCtxImpl, ESETCTSTOSELF );
                    }
                }
            else
                {
                // network requirements not met. Take the same action as 
                // in the join failed case
                
                OsTracePrint( 
                    KWarningLevel, (TUint8*)
                    ("UMAC: WlanDot11Synchronize::OnStateEntryEvent(): network requirements not met - abort"));            
                
                ChangeInternalState( aCtxImpl, ECONTINUEDOT11TRAVERSE );
                }
            break;
        case ESETDOT11SLOTTIME:
            SetDot11SlotTime( aCtxImpl );
            break;
        case ESETCTSTOSELF:
            SetCtsToSelf( aCtxImpl );
            break;
        case ECONFTXQUEUE:
            ConfigureQueue( aCtxImpl );
            break;
        case ECONFTXQUEUEPARAMS:
            ConfigureTxQueueParams( aCtxImpl );
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
        case ERESETHTCAPABILITIES:
            ResetHtCapabilities( aCtxImpl );
            break;
        case EISSUEJOIN:
            IssueJoin( aCtxImpl );
            break;
        case ESETHTBLOCKACKCONF:
            SetHtBlockAckConfiguration( aCtxImpl );
            break;
        case ERESETHTBLOCKACKCONF:
            ResetHtBlockAckConfiguration( aCtxImpl );            
            break;
        case ESETPAIRWISEKEY:
            SetPtk( aCtxImpl );
            break;
        case ECONTINUEDOT11TRAVERSE:
            ContinueDot11StateTraversal( aCtxImpl );
            break;
        default:
            // programming error
            OsTracePrint( 
                KErrorLevel, 
                (TUint8*)("UMAC: WlanDot11Synchronize::OnStateEntryEvent(): state: %d"), 
                iState);
            OsAssert( (TUint8*)("UMAC: WlanDot11Synchronize::OnStateEntryEvent(): panic"), 
                (TUint8*)(WLAN_FILE), __LINE__ );
            break;
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11Synchronize::OnTxCompleteEvent( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( 
        KUmacDetails, 
        (TUint8*)("UMAC: WlanDot11Synchronize::OnTxCompleteEvent()"));

    switch ( iState )
        {
        case ESETDOT11SLOTTIME:
            // continue state traversal
            ChangeInternalState( aCtxImpl, ESETCTSTOSELF );
            break;
        case ESETCTSTOSELF:
            if ( aCtxImpl.QosEnabled() )
                {
                // configure all the Tx queues & their AC parameters
                ChangeInternalState( aCtxImpl, ECONFTXQUEUEPARAMS );                
                }
            else
                {
                // configure just the legacy Tx queue
                ChangeInternalState( aCtxImpl, ECONFTXQUEUE );                                
                }            
            break;
        case ECONFTXQUEUE:
        case ECONFTXQUEUEPARAMS:
            ChangeInternalState( aCtxImpl, ESETTXRATEPOLICY );
            break;
        case ESETTXRATEPOLICY:
            if ( aCtxImpl.HtSupportedByNw() )
                {
                ChangeInternalState( aCtxImpl, ESETHTCAPABILITIES );
                }
            else
                {
                if ( aCtxImpl.WHASettings().iCapability & 
                     WHA::SSettings::KHtOperation )
                    {
                    ChangeInternalState( aCtxImpl, ERESETHTCAPABILITIES );
                    }
                else
                    {
                    ChangeInternalState( aCtxImpl, EISSUEJOIN );
                    }
                }
            break;
        case ESETHTCAPABILITIES:
            ChangeInternalState( aCtxImpl, ESETHTBSSOPERATION );
            break;
        case ESETHTBSSOPERATION:
            ChangeInternalState( aCtxImpl, EISSUEJOIN );
            break;
        case ERESETHTCAPABILITIES:
            ChangeInternalState( aCtxImpl, EISSUEJOIN );
            break;
        case EISSUEJOIN:
            if ( aCtxImpl.HtSupportedByNw() )
                {
                ChangeInternalState( aCtxImpl, ESETHTBLOCKACKCONF );
                }
            else
                {
                if ( aCtxImpl.WHASettings().iCapability & 
                     WHA::SSettings::KHtOperation )
                    {
                    ChangeInternalState( aCtxImpl, ERESETHTBLOCKACKCONF );
                    }
                else
                    {
                    if ( aCtxImpl.RoamingPairwiseKey() )
                        {
                        ChangeInternalState( aCtxImpl, ESETPAIRWISEKEY );
                        }
                    else
                        {
                        ChangeInternalState( aCtxImpl, ECONTINUEDOT11TRAVERSE );
                        }
                    }
                }
            break;
        case ESETHTBLOCKACKCONF:
        case ERESETHTBLOCKACKCONF:
            if ( aCtxImpl.RoamingPairwiseKey() )
                {
                ChangeInternalState( aCtxImpl, ESETPAIRWISEKEY );
                }
            else
                {
                ChangeInternalState( aCtxImpl, ECONTINUEDOT11TRAVERSE );
                }
            break;
        case ESETPAIRWISEKEY:
            ChangeInternalState( aCtxImpl, ECONTINUEDOT11TRAVERSE );            
            break;
        default:
            // catch internal FSM programming error
            OsAssert( 
                (TUint8*)("UMAC: WlanDot11Synchronize::OnTxCompleteEvent(): panic"), 
                (TUint8*)(WLAN_FILE), __LINE__ );
            break;
        }
    }

// -----------------------------------------------------------------------------
// as there's really nothing else we can do in this situation,
// simulate macNotResponding error
// -----------------------------------------------------------------------------
//
void WlanDot11Synchronize::OnAbortEvent( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( KWarningLevel, 
        (TUint8*)("UMAC * dot11-synchronize * abort") );

    DoErrorIndication( aCtxImpl, WHA::KErrorMacNotResponding );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11Synchronize::ContinueDot11StateTraversal( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( 
        KUmacDetails, 
        (TUint8*)("UMAC: WlanDot11Synchronize::ContinueDot11StateTraversal()"));

    if ( iJoinStatus != KErrNone )
        {
        // set the completion code value to be returned to user mode
        // as the dot11idle state does the OID completion in this case
        aCtxImpl.iStates.iIdleState.Set( iJoinStatus );
        // ... and proceed to dot11idle state                      
        ChangeState( aCtxImpl, 
            *this,                          // prev state
            aCtxImpl.iStates.iIdleState     // next state
            );
        }
    else
        {
        if ( aCtxImpl.AuthenticationAlgorithmNumber() != 
             K802Dot11AuthModeShared )
            {
            // proceed with open authentication
            ChangeState( aCtxImpl, 
                *this,                                  // prev state
                aCtxImpl.iStates.iOpenAuthPendingState  // next state
                );                       
            }
        else
            {
            // proceed with shared authentication
            ChangeState( aCtxImpl, 
                *this,                                  // prev state
                aCtxImpl.iStates.iSharedAuthPending     // next state
                );                       
            }
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11Synchronize::ChangeInternalState( 
    WlanContextImpl& aCtxImpl, 
    TState aNewState )
    {
#ifndef NDEBUG
    OsTracePrint( 
        KUmacDetails, 
        (TUint8*)("UMAC: WlanDot11Synchronize::ChangeInternalState(): old state:"));
    OsTracePrint( KUmacDetails, iStateName[iState] );
    OsTracePrint( 
        KUmacDetails, 
        (TUint8*)("UMAC: WlanDot11Synchronize::ChangeInternalState(): new state:"));
    OsTracePrint( KUmacDetails, iStateName[aNewState] );
#endif

    iState = aNewState;
    Fsm( aCtxImpl, ESTATEENTRY );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TInt WlanDot11Synchronize::InitActions( WlanContextImpl& aCtxImpl ) const
    {
    OsTracePrint( 
        KUmacDetails, 
        (TUint8*)("UMAC: WlanDot11Synchronize::InitActions()"));
        
    // as we are about to join a new AP, reset BSS Loss indicators
    aCtxImpl.ResetBssLossIndications();

    // reset counter for this new AP connection
    aCtxImpl.ResetFailedTxPacketCount();            
        
    os_memset( 
        reinterpret_cast<TUint8*>(&(aCtxImpl.GetNwHtCapabilitiesIe().iData)), 
        0, 
        K802Dot11HtCapabilitiesIeDataLen );
    
    for ( TUint i = 0; i < WHA::EQueueIdMax; ++i )
        {
        aCtxImpl.iWlanMib.dot11MaxTransmitMSDULifetime[i] = 
            aCtxImpl.iWlanMib.dot11MaxTransmitMSDULifetimeDefault;
        aCtxImpl.iWlanMib.iMediumTime[i] = KDot11MediumTimeDefault;
        }    

    // in case WLAN Mgmt Client has given us the permission to use PS mode,
    // Light PS is the initial desired PS mode configuration
    aCtxImpl.SetDesiredPsModeConfig( 
        aCtxImpl.ClientLightPsModeConfig() );
    // also make sure that the dynamic power mode managemen parameters are in
    // their WLAN Mgmt Client provided values
    aCtxImpl.RestorePwrModeMgmtParameters();
    
    // check do we meet the requirements for the network
    // and construct necessary objects for doing the connection
    //
    return InitNetworkConnect( 
        aCtxImpl,
        aCtxImpl.ScanResponseFrameBodyLength(),
        aCtxImpl.ScanResponseFrameBody() );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11Synchronize::SetDot11SlotTime( 
    WlanContextImpl& aCtxImpl )
    {
    WHA::Sdot11SlotTime* mib 
        = static_cast<WHA::Sdot11SlotTime*>
        (os_alloc( sizeof( WHA::Sdot11SlotTime ) )); 

    if ( !mib )
        {
        // alloc failue just send abort event to fsm 
        // it takes care of the rest
        Fsm( aCtxImpl, EABORT );
        return;
        }

    if ( aCtxImpl.UseShortSlotTime() )
        {
        mib->iDot11SlotTime = WHA::KSlotTime9;        
        }
    else
        {
        mib->iDot11SlotTime = WHA::KSlotTime20;
        }

    OsTracePrint( 
        KUmacDetails, 
        (TUint8*)
        ("UMAC: WlanDot11Synchronize::SetDot11SlotTime(): set slottime: %d"), 
        mib->iDot11SlotTime );

    WlanWsaWriteMib& wha_cmd = aCtxImpl.WsaWriteMib();
    wha_cmd.Set( 
        aCtxImpl, WHA::KMibDot11SlotTime, sizeof(*mib), mib );

    // change global state: entry procedure triggers action
    ChangeState( aCtxImpl, 
        *this,              // prev state
        wha_cmd             // next state
        );           

    // as the parameters have been supplied we can now deallocate
    os_free( mib );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11Synchronize::SetCtsToSelf( 
    WlanContextImpl& aCtxImpl )
    {
    WHA::SctsToSelf* mib 
        = static_cast<WHA::SctsToSelf*>
        (os_alloc( sizeof( WHA::SctsToSelf ) )); 

    if ( !mib )
        {
        // alloc failue just send abort event to fsm 
        // it takes care of the rest
        Fsm( aCtxImpl, EABORT );
        return;
        }

    if ( aCtxImpl.ProtectionBitSet() )
        {
        OsTracePrint( 
            KUmacDetails, 
            (TUint8*)("UMAC: WlanDot11Synchronize::SetCtsToSelf(): enable CTS to self") );

        mib->iCtsToSelf = ETrue;
        }
    else
        {
        OsTracePrint( 
            KUmacDetails, 
            (TUint8*)("UMAC: WlanDot11Synchronize::SetCtsToSelf(): disable CTS to self") );

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
    
    // as the parameters have been supplied we can now deallocate
    os_free( mib );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11Synchronize::ConfigureQueue( 
    WlanContextImpl& aCtxImpl )
    {
    ConfigureTxQueue( aCtxImpl, WHA::ELegacy );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11Synchronize::ConfigureTxQueueParams( 
    WlanContextImpl& aCtxImpl )
    {
    WlanConfigureTxQueueParams& complex_wsa_cmd = 
        aCtxImpl.ConfigureTxQueueParams();
    
    // change global state: entry procedure triggers action
    ChangeState( aCtxImpl, 
        *this,              // prev state
        complex_wsa_cmd     // next state
        ); 
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11Synchronize::SetTxRatePolicy( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanDot11Synchronize::SetTxRatePolicy(): rate bitmask: 0x%08x"),
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
void WlanDot11Synchronize::SetHtCapabilities( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanDot11Synchronize::SetHtCapabilities") );

    ConfigureHtCapabilities( aCtxImpl );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11Synchronize::SetHtBssOperation( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanDot11Synchronize::SetHtBssOperation") );

    ConfigureHtBssOperation( aCtxImpl );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11Synchronize::ResetHtCapabilities( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanDot11Synchronize::ResetHtCapabilities") );

    ResetHtCapabilitiesMib( aCtxImpl );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11Synchronize::ResetHtBlockAckConfiguration( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanDot11Synchronize::ResetHtBlockAckConfiguration") );
    
    ResetHtBlockAckConfigureMib( aCtxImpl );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11Synchronize::IssueJoin( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanDot11Synchronize::IssueJoin"));
    
    // make WHA types
    WHA::SSSID ssid;
    ssid.iSSIDLength = (aCtxImpl.GetSsId()).ssidLength;
    os_memcpy( ssid.iSSID, (aCtxImpl.GetSsId()).ssid, ssid.iSSIDLength );

    // determine the preamble to be used
    
    WHA::TPreamble preamble ( WHA::ELongPreamble ); 
    if ( aCtxImpl.HtSupportedByNw() )
        {
        preamble = (aCtxImpl.GetHtAssociationRequestFrame())
            .iFixedFields.iCapabilityInfo.IsShortPreambleBitSet() ? 
                WHA::EShortPreamble : 
                WHA::ELongPreamble;
        }
    else
        {
        preamble = (aCtxImpl.GetAssociationRequestFrame())
            .iFixedFields.iCapabilityInfo.IsShortPreambleBitSet() ? 
                WHA::EShortPreamble : 
                WHA::ELongPreamble;
        }
    
    // set context
    aCtxImpl.WsaJoin().Set( 
        aCtxImpl, 
        aCtxImpl.NetworkOperationMode(), 
        reinterpret_cast<WHA::TMacAddress&>(aCtxImpl.GetBssId()), 
        WHA::KBand2dot4GHzMask,
        ssid, 
        aCtxImpl.NetworkChannelNumeber(), 
        aCtxImpl.NetworkBeaconInterval(), 
        aCtxImpl.BasicRateSet(), 
        0,  // ATIM
        preamble, 
        ( aCtxImpl.WHASettings().iCapability 
          & WHA::SSettings::KProbe4Join ) ? ETrue : EFalse );
        
    // change global state: entry procedure triggers action
    ChangeState( aCtxImpl, 
        *this,              // prev state
        aCtxImpl.WsaJoin()  // next state
        );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11Synchronize::SetHtBlockAckConfiguration( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanDot11Synchronize::SetHtBlockAckConfiguration") );

    // allocate memory for the mib to write
    WHA::ShtBlockAckConfigure* mib 
        = static_cast<WHA::ShtBlockAckConfigure*>
        (os_alloc( sizeof( WHA::ShtBlockAckConfigure ) )); 

    if ( !mib )
        {
        // alloc failue just send abort event to fsm 
        // it takes care of the rest
        Fsm( aCtxImpl, EABORT );
        return;
        }
    
    // retrieve reference to the stored HT Block Ack configuration
    const WHA::ShtBlockAckConfigure& blockAckConf ( 
        aCtxImpl.GetHtBlockAckConfigure() );
    
    mib->iTxBlockAckUsage = blockAckConf.iTxBlockAckUsage;
    mib->iRxBlockAckUsage = blockAckConf.iRxBlockAckUsage;
    os_memset( mib->iReserved, 0, sizeof( mib->iReserved ) );
        
    WlanWsaWriteMib& wha_cmd = aCtxImpl.WsaWriteMib();
        
    wha_cmd.Set( 
        aCtxImpl, 
        WHA::KMibHtBlockAckConfigure, 
        sizeof( *mib ), 
        mib );
        
    // change global state: entry procedure triggers action
    ChangeState( aCtxImpl, 
        *this,              // prev state
        wha_cmd );          // next state

    // as the parameters have been supplied we can now deallocate
    os_free( mib );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11Synchronize::SetPtk( WlanContextImpl& aCtxImpl )
    {
    TBool ret( EFalse );

    const TWlanCipherSuite pairwiseCipher( aCtxImpl.PairwiseCipher());

    if ( pairwiseCipher == EWlanCipherSuiteCcmp )
        {
        ret = SetCcmpPtk( aCtxImpl );
        }
    else if ( pairwiseCipher == EWlanCipherSuiteTkip )
        {
        ret = SetTkipPtk( aCtxImpl );
        }
    else if ( pairwiseCipher == EWlanCipherSuiteWep )
        {
        ret = SetWepKey( aCtxImpl );
        }
    else
        {
        OsTracePrint( KErrorLevel, (TUint8*)
            ("UMAC: unsupported cipher: %d"), pairwiseCipher );
        OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
        }

    if ( !ret )
        {
        // Alloc failue. Send abort event to fsm. It takes care of the rest
        Fsm( aCtxImpl, EABORT );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanDot11Synchronize::SetCcmpPtk( WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanDot11Synchronize::SetCcmpPtk"));

    TBool ret( EFalse );

    const TPairwiseKeyData& keyData( *(aCtxImpl.RoamingPairwiseKey()) );
    WlanWsaAddKey& wsa_cmd( aCtxImpl.WsaAddKey() );
    WHA::SAesPairwiseKey* key( CreateAesPtkCtx( 
            aCtxImpl, 
            wsa_cmd,
            keyData.data, 
            aCtxImpl.GetBssId() ) 
            );

    if ( key )
        {
        ret = ETrue;
        // change global state: entry procedure triggers action
        ChangeState( aCtxImpl, 
            *this,  // prev state
            wsa_cmd // next state
            );
        
        os_free( key ); // release the memory
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanDot11Synchronize::SetTkipPtk( WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanDot11Synchronize::SetTkipPtk"));

    TBool ret( EFalse );

    const TPairwiseKeyData& keyData( *(aCtxImpl.RoamingPairwiseKey()) );
    WlanWsaAddKey& wsa_cmd( aCtxImpl.WsaAddKey() );    
    WHA::STkipPairwiseKey* key( CreateTkipPtkCtx( 
            aCtxImpl, 
            wsa_cmd,
            keyData.data, 
            static_cast<T802Dot11WepKeyId>(keyData.keyIndex),
            aCtxImpl.GetBssId() ) 
            );

    if ( key )
        {
        ret = ETrue;
        // change global state: entry procedure triggers action
        ChangeState( aCtxImpl, 
            *this,  // prev state
            wsa_cmd // next state
            );   

        os_free( key ); // release the memory
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//

TBool WlanDot11Synchronize::SetWepKey( WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanDot11Synchronize::SetWepPairwiseKey"));

    TBool ret( EFalse );

    const TPairwiseKeyData& keyData( *(aCtxImpl.RoamingPairwiseKey()) );
    WlanWsaAddKey& wha_cmd( aCtxImpl.WsaAddKey() );    
    WHA::SWepPairwiseKey* key( CreateUnicastWepKeyCtx( 
            aCtxImpl, 
            wha_cmd,
            aCtxImpl.GetBssId(),
            keyData.length,
            keyData.data ) );

    if ( key )
        {
        ret = ETrue;
        // change global state: entry procedure triggers action
        ChangeState( aCtxImpl, 
            *this,  // prev state
            wha_cmd // next state
            );

        os_free( key ); // release the memory
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11Synchronize::OnWhaCommandResponse( 
    WlanContextImpl& /*aCtxImpl*/, 
    WHA::TCommandId aCommandId, 
    WHA::TStatus aStatus,
    const WHA::UCommandResponseParams& /*aCommandResponseParams*/,
    TUint32 aAct )
    {
    if ( aAct )
        {
        // should not happen as we a runnng in non-pre-emptive mode
        // regarding oid commands
        OsTracePrint( KErrorLevel, (TUint8*)("UMAC: aAct: %d"), aAct );
        OsAssert( (TUint8*)("UMAC: panic"),(TUint8*)(WLAN_FILE), __LINE__ );
        }

    // this is a response to a command that was generated 
    // by this dot11 state object layer

    // we are only interested of join command response
    // as it is the oly one we trigger from here that 
    // has a meaningfull return value
    if ( aCommandId == WHA::EJoinResponse )
        {
        if ( aStatus == WHA::KFailed )
            {
            OsTracePrint( KWarningLevel, (TUint8*)
                ("UMAC: WlanDot11Synchronize::OnWhaCommandResponse(): join failed"));
            // make a note of the failure and act 
            // accordingly when we
            // soon again enter this state
            iJoinStatus = KErrGeneral;
            }
        else
            {
            OsTracePrint( KInfoLevel, 
                (TUint8*)("UMAC: WlanDot11Synchronize::OnWhaCommandResponse(): join success"));                        
            }
        }
    else    // --- aCommandId == WHA::EJoinResponse ---
        {
        // no action here
        }
    }
