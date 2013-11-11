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
* Description:   Implementation of the UmacDot11PrepareForBssMode class
*
*/

/*
* %version: 31 %
*/

#include "config.h"
#include "UmacDot11PrepareForBssMode.h"
#include "UmacWsaWriteMib.h"
#include "UmacWsaJoin.h"
#include "umacconfiguretxqueueparams.h"
#include "UmacContextImpl.h"
#include "wha_mibDefaultvalues.h"


#ifndef NDEBUG
const TInt8 WlanDot11PrepareForBssMode::iName[] 
    = "dot11-prepareforbssmode";

const TUint8 WlanDot11PrepareForBssMode::iStateName
    [ESTATEMAX][KMaxStateStringLength] = 
    {
        {"EINIT"}, 
        {"ESETSLEEPMODE"}, 
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
        {"ECONTINUEDOT11TRAVERSE"}
    };

const TUint8 WlanDot11PrepareForBssMode::iEventName
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
#ifndef NDEBUG 
const TInt8* WlanDot11PrepareForBssMode::GetStateName( 
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
void WlanDot11PrepareForBssMode::Entry( 
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

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11PrepareForBssMode::Exit( 
    WlanContextImpl& /*aCtxImpl*/ )
    {
    OsTracePrint( KUmacProtocolState, (TUint8*)
        ("UMAC: WlanDot11PrepareForBssMode::Exit()") );

    // only thing we shall do is to reset our FSM for the next round...
    iState = EINIT; 
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11PrepareForBssMode::OnWhaCommandResponse( 
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
            OsTracePrint( KWarningLevel, 
                (TUint8*)("UMAC: WlanDot11PrepareForBssMode: join failed"));
            // make a note of the failure and act 
            // accordingly when we
            // soon again enter this state
            iJoinFailed = ETrue;
            }
        else
            {
            OsTracePrint( KInfoLevel, 
                (TUint8*)("UMAC: WlanDot11PrepareForBssMode: join success"));                        
            }
        }
    else    // --- aCommandId == WHA::EJoinResponse ---
        {
        // no action here
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11PrepareForBssMode::ChangeInternalState( 
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
void WlanDot11PrepareForBssMode::Fsm( 
    WlanContextImpl& aCtxImpl, 
    TEvent aEvent )
    {
    OsTracePrint( KUmacDetails, 
        (TUint8*)("UMAC: WlanDot11PrepareForBssMode::Fsm(): FSM EVENT") );
#ifndef NDEBUG
    OsTracePrint( 
        KUmacDetails, 
        (TUint8*)("UMAC: WlanDot11PrepareForBssMode::Fsm(): event:"));
    OsTracePrint( KUmacDetails, iEventName[aEvent] );
    OsTracePrint( 
        KUmacDetails, 
        (TUint8*)("UMAC: WlanDot11PrepareForBssMode::Fsm(): state:"));
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
                (TUint8*)("UMAC: WlanDot11PrepareForBssMode::Fsm(): event: %d"), 
                aEvent);        
            OsAssert( 
                (TUint8*)("UMAC: WlanDot11PrepareForBssMode::Fsm(): panic"), 
                (TUint8*)(WLAN_FILE), __LINE__ );
            break;
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11PrepareForBssMode::OnStateEntryEvent( 
    WlanContextImpl& aCtxImpl )
    {
    switch ( iState )
        {
        case EINIT:
            // as we are about to join a new AP, reset BSS Loss indicators
            aCtxImpl.ResetBssLossIndications();
            iJoinFailed = EFalse;
            // make sure that this counter is reset; also in case the 
            // previous connect attempt has failed
            aCtxImpl.ResetFailedTxPacketCount();            
            // start the FSM traversal
            ChangeInternalState( aCtxImpl, 
                ESETSLEEPMODE );            
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
        case ECONTINUEDOT11TRAVERSE:
            ContinueDot11StateTraversal( aCtxImpl );
            break;
        default:
            // catch internal FSM programming error
            OsTracePrint( 
                KErrorLevel, 
                (TUint8*)("UMAC: WlanDot11PrepareForBssMode::OnStateEntryEvent(): state: %d"), 
                iState);
            OsAssert( (TUint8*)("UMAC: WlanDot11PrepareForBssMode::OnStateEntryEvent(): panic"), 
                (TUint8*)(WLAN_FILE), __LINE__ );
            break;
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11PrepareForBssMode::OnTxCompleteEvent( 
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
                    ("UMAC: WlanDot11PrepareForBssMode::OnTxCompleteEvent"));
                OsTracePrint( KWarningLevel, (TUint8*)
                    ("UMAC: no support for dot11slottime mib skipping"));

                ChangeInternalState( aCtxImpl, ESETCTSTOSELF );
                }
            break;
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
                // This is done in order to have the correct queue 
                // configuration also in the case that the previous connect
                // attempt to a QoS AP failed (and we asked for a QoS queue 
                // configuration for it), and we are now about to
                // connect to a non-QoS AP
                ChangeInternalState( aCtxImpl, ECONFTXQUEUE );                                
                }
            break;
        case ECONFTXQUEUE:
        case ECONFTXQUEUEPARAMS:
            // continue state traversal
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
                    ChangeInternalState( aCtxImpl, ECONTINUEDOT11TRAVERSE );                    
                    }
                }
            break;
        case ESETHTBLOCKACKCONF:
            ChangeInternalState( aCtxImpl, ECONTINUEDOT11TRAVERSE );
            break;
        case ERESETHTBLOCKACKCONF:
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
// as there's really nothing else we can do in this situation
// simulate macNotResponding error
// -----------------------------------------------------------------------------
//
void WlanDot11PrepareForBssMode::OnAbortEvent( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( 
        KUmacDetails, 
        (TUint8*)("UMAC: WlanDot11PrepareForBssMode::OnAbortEvent()"));

    DoErrorIndication( aCtxImpl, WHA::KErrorMacNotResponding );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11PrepareForBssMode::SetSleepMode( 
    WlanContextImpl& aCtxImpl )
    {
    WHA::SsleepMode* mib 
        = static_cast<WHA::SsleepMode*>(os_alloc( sizeof( WHA::SsleepMode ) )); 

    if ( !mib )
        {
        // alloc failue just send abort event to fsm 
        // it takes care of the rest
        Fsm( aCtxImpl, EABORT );
        return;
        }

    mib->iMode = WHA::KPowerDownMode;
    
    OsTracePrint( 
        KUmacDetails, 
        (TUint8*)
        ("UMAC: WlanDot11PrepareForBssMode::SetSleepMode(): set sleepmode: %d"), 
        mib->iMode );

    WlanWsaWriteMib& wha_cmd = aCtxImpl.WsaWriteMib();
    wha_cmd.Set( 
        aCtxImpl, WHA::KMibSleepMode, sizeof(*mib), mib );

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
void WlanDot11PrepareForBssMode::SetDot11SlotTime( 
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

    OsTracePrint( KUmacDetails, (TUint8*)("UMAC * set slottime: %d"), 
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
void WlanDot11PrepareForBssMode::SetCtsToSelf( 
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
            (TUint8*)("UMAC: WlanDot11PrepareForBssMode::SetCtsToSelf(): enable CTS to self") );
            
        mib->iCtsToSelf = ETrue;
        }
    else
        {
        OsTracePrint( 
            KUmacDetails, 
            (TUint8*)("UMAC: WlanDot11PrepareForBssMode::SetCtsToSelf(): disable CTS to self") );
            
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
void WlanDot11PrepareForBssMode::ConfigureQueue( 
    WlanContextImpl& aCtxImpl )
    {
    ConfigureTxQueue( aCtxImpl, WHA::ELegacy );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11PrepareForBssMode::ConfigureTxQueueParams( 
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
void WlanDot11PrepareForBssMode::SetTxRatePolicy( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanDot11PrepareForBssMode::SetTxRatePolicy(): rate bitmask: 0x%08x"),
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
void WlanDot11PrepareForBssMode::SetHtCapabilities( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanDot11PrepareForBssMode::SetHtCapabilities") );

    ConfigureHtCapabilities( aCtxImpl );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11PrepareForBssMode::SetHtBssOperation( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanDot11PrepareForBssMode::SetHtBssOperation") );

    ConfigureHtBssOperation( aCtxImpl );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11PrepareForBssMode::ResetHtCapabilities( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanDot11PrepareForBssMode::ResetHtCapabilities") );

    ResetHtCapabilitiesMib( aCtxImpl );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11PrepareForBssMode::ResetHtBlockAckConfiguration( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanDot11PrepareForBssMode::ResetHtBlockAckConfiguration") );
    
    ResetHtBlockAckConfigureMib( aCtxImpl );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11PrepareForBssMode::SetHtBlockAckConfiguration( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanDot11PrepareForBssMode::SetHtBlockAckConfiguration") );

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

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11PrepareForBssMode::IssueJoin( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( 
        KUmacDetails, 
        (TUint8*)("UMAC: WlanDot11PrepareForBssMode::IssueJoin()") );
    
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
        0,  // ATIM
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

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11PrepareForBssMode::ContinueDot11StateTraversal( 
    WlanContextImpl& aCtxImpl )
    {
    if ( iJoinFailed )    
        {
        // set the completion code value to be returned to user mode
        // as the dot11idle state does the OID completion in this case
        aCtxImpl.iStates.iIdleState.Set( KErrGeneral );

        // go back to dot11Idle state
        ChangeState( aCtxImpl, 
            *this,                                  // prev state
            aCtxImpl.iStates.iIdleState             // next state
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
