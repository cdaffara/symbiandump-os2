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
* Description:   Implementation of the WlanDot11SoftReset class
*
*/

/*
* %version: 29 %
*/

#include "config.h"
#include "UmacDot11SoftReset.h"
#include "UmacContextImpl.h"
#include "UmacWsaWriteMib.h"
#include "UmacWsaDisconnect.h"
#include "umacconfiguretxqueueparams.h"

#ifndef NDEBUG
const TInt8 WlanDot11SoftReset::iName[] = "dot11-softreset";

const TUint8 WlanDot11SoftReset::iStateName
    [ESTATEMAX][KMaxStateStringLength] = 
    {
        {"EINIT"},
        {"EISSUEDISCONNECT"}, 
        {"ECONFTXQUEUE"},
        {"ECONTINUEDOT11TRAVERSE"}
    };

const TUint8 WlanDot11SoftReset::iEventName
    [EEVENTMAX][KMaxEventStringLength] = 
    {
        {"ESTATEENTRY"}, 
        {"ETXCOMPLETE"},
        {"EABORT"}
    };
#endif 
// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11SoftReset::Fsm( 
    WlanContextImpl& aCtxImpl, 
    TEvent aEvent )
    {
#ifndef NDEBUG
    OsTracePrint( 
        KUmacDetails, 
        (TUint8*)("UMAC: WlanDot11SoftReset::Fsm(): event: "));
    OsTracePrint( 
        KUmacDetails, 
        iEventName[aEvent] );
    OsTracePrint( 
        KUmacDetails, 
        (TUint8*)("UMAC: WlanDot11SoftReset::Fsm(): state: ")); 
    OsTracePrint( 
        KUmacDetails, 
        iStateName[iState] );
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
                (TUint8*)("UMAC: WlanDot11SoftReset::Fsm(): event: %d"), 
                aEvent);        
            OsAssert( 
                (TUint8*)("UMAC: WlanDot11SoftReset::Fsm(): panic"), 
                (TUint8*)(WLAN_FILE), __LINE__ );
            break;
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11SoftReset::OnStateEntryEvent( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( 
        KUmacDetails, 
        (TUint8*)("UMAC: WlanDot11SoftReset::OnStateEntryEvent()"));

    switch ( iState )
        {
        case EINIT:
            InitActions( aCtxImpl );
            ChangeInternalState( aCtxImpl, EISSUEDISCONNECT );            
            break;
        case EISSUEDISCONNECT:
            IssueDisconnect( aCtxImpl );
            break;
        case ECONFTXQUEUE:
            // configure just the legacy Tx queue    
            ConfigureQueue( aCtxImpl );
            break;
        case ECONTINUEDOT11TRAVERSE:
            ContinueDot11StateTraversal( aCtxImpl );
            break;
        default:
            // programming error
            OsTracePrint( KErrorLevel, (TUint8*)("UMAC: state: %d"), iState);
            OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11SoftReset::InitActions( WlanContextImpl& aCtxImpl )
    {
    // we are breaking an existing join so we have to reset some
    // state variables and settings
    aCtxImpl.CurrentDot11PwrMgmtMode( WHA::KPsDisable );
    aCtxImpl.RestorePwrModeMgmtParameters();
    aCtxImpl.Reassociate( EFalse );
    aCtxImpl.QosEnabled( EFalse );
    aCtxImpl.UapsdEnabled( EFalse );
    DetermineAcUapsdUsage( aCtxImpl );
    aCtxImpl.TerminateVoiceOverWlanCallMaintenance();
    aCtxImpl.MulticastFilteringDisAllowed( EFalse );
    aCtxImpl.ResetMulticastAddresses();
    aCtxImpl.ResetFailedTxPacketCount();            
    aCtxImpl.IeData( NULL );
    aCtxImpl.DisassociatedByAp( EFalse );
    aCtxImpl.ResetTxRateAdaptation();
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
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11SoftReset::OnTxCompleteEvent( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( 
        KUmacDetails, 
        (TUint8*)("UMAC: WlanDot11SoftReset::OnTxCompleteEvent()"));

    switch ( iState )
        {
        case EISSUEDISCONNECT:
            // continue state traversal
            ChangeInternalState( aCtxImpl, ECONFTXQUEUE );
            break;
        case ECONFTXQUEUE:
            // continue state traversal
            ChangeInternalState( aCtxImpl, ECONTINUEDOT11TRAVERSE );
            break;
        default:
            // catch internal FSM programming error
            OsTracePrint( KErrorLevel, (TUint8*)("UMAC: state: %d"), iState);
            OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11SoftReset::OnAbortEvent( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( KWarningLevel, 
        (TUint8*)("UMAC: WlanDot11SoftReset::OnAbortEvent()") );
    DoErrorIndication( aCtxImpl, WHA::KErrorMacNotResponding );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11SoftReset::ContinueDot11StateTraversal( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanDot11SoftReset::ContinueDot11StateTraversal()"));

#ifndef NDEBUG
    
    // trace current frame statistics
    
    const TStatisticsResponse& frameStatistics ( aCtxImpl.FrameStatistics() );
       
    OsTracePrint( 
        KUmacDetails, (TUint8*)
        ("UMAC: WlanDot11SoftReset::ContinueDot11StateTraversal: *** current frame statistics ***:") );

    for ( TUint i = 0; i < EQueueIdMax; ++i )
        {
        OsTracePrint( 
            KUmacDetails, (TUint8*)
            ("UMAC: WlanDot11SoftReset::ContinueDot11StateTraversal: * Access Category: %d *"),
            i );

        OsTracePrint( 
            KUmacDetails, (TUint8*)
            ("UMAC: WlanDot11SoftReset::ContinueDot11StateTraversal: successfully received unicast data frames: %d"), 
             frameStatistics.acSpecific[i].rxUnicastDataFrameCount );

        OsTracePrint( 
            KUmacDetails, (TUint8*)
            ("UMAC: WlanDot11SoftReset::ContinueDot11StateTraversal: successfully transmitted unicast data frames: %d"), 
             frameStatistics.acSpecific[i].txUnicastDataFrameCount );

        OsTracePrint( 
            KUmacDetails, (TUint8*)
            ("UMAC: WlanDot11SoftReset::ContinueDot11StateTraversal: successfully received multicast data frames: %d"), 
             frameStatistics.acSpecific[i].rxMulticastDataFrameCount );
        
        OsTracePrint( 
            KUmacDetails, (TUint8*)
            ("UMAC: WlanDot11SoftReset::ContinueDot11StateTraversal: successfully transmitted multicast data frames: %d"), 
             frameStatistics.acSpecific[i].txMulticastDataFrameCount );

        OsTracePrint( 
            KUmacDetails, (TUint8*)
            ("UMAC: WlanDot11SoftReset::ContinueDot11StateTraversal: nbr of data frame transmit retries: %d"), 
             frameStatistics.acSpecific[i].txRetryCount );

        OsTracePrint( 
            KUmacDetails, (TUint8*)
            ("UMAC: WlanDot11SoftReset::ContinueDot11StateTraversal: nbr of data frame WLAN delivery failures: %d"), 
             frameStatistics.acSpecific[i].txErrorCount );                    

        OsTracePrint( 
            KUmacDetails, (TUint8*)
            ("UMAC: WlanDot11SoftReset::ContinueDot11StateTraversal: average data frame Tx media delay in microsecs: %d"), 
            aCtxImpl.AverageTxMediaDelay( static_cast<WHA::TQueueId>(i) ) );                    

        OsTracePrint( 
            KUmacDetails, (TUint8*)
            ("UMAC: WlanDot11SoftReset::ContinueDot11StateTraversal: average data frame total Tx delay in microsecs: %d"), 
             aCtxImpl.AverageTotalTxDelay( static_cast<WHA::TQueueId>(i) ) );                    

        OsTracePrint( 
            KUmacDetails, (TUint8*)
            ("UMAC: WlanDot11SoftReset::ContinueDot11StateTraversal: data frame total Tx delay bin 0 count: %d"), 
             frameStatistics.acSpecific[i].totalTxDelayBin0 );

        OsTracePrint( 
            KUmacDetails, (TUint8*)
            ("UMAC: WlanDot11SoftReset::ContinueDot11StateTraversal: data frame total Tx delay bin 1 count: %d"), 
             frameStatistics.acSpecific[i].totalTxDelayBin1 );

        OsTracePrint( 
            KUmacDetails, (TUint8*)
            ("UMAC: WlanDot11SoftReset::ContinueDot11StateTraversal: data frame total Tx delay bin 2 count: %d"), 
             frameStatistics.acSpecific[i].totalTxDelayBin2 );

        OsTracePrint( 
            KUmacDetails, (TUint8*)
            ("UMAC: WlanDot11SoftReset::ContinueDot11StateTraversal: data frame total Tx delay bin 3 count: %d"), 
             frameStatistics.acSpecific[i].totalTxDelayBin3 );
        }

    OsTracePrint( 
        KUmacDetails, (TUint8*)
        ("UMAC: WlanDot11SoftReset::ContinueDot11StateTraversal: all ACs: nbr of FCS errors in received MPDUs: %d"), 
         frameStatistics.fcsErrorCount );
#endif

    // change global state: entry procedure triggers action
    ChangeState( aCtxImpl,              
        *this,                                  // prev state
        aCtxImpl.iStates.iMibDefaultConfigure   // next state
        );                               
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11SoftReset::ChangeInternalState( 
    WlanContextImpl& aCtxImpl, 
    TState aNewState )
    {
#ifndef NDEBUG
    OsTracePrint( 
        KUmacDetails, 
        (TUint8*)
        ("UMAC: WlanDot11SoftReset::ChangeInternalState(): old state:"));
    OsTracePrint( 
        KUmacDetails, 
        iStateName[iState] );
    OsTracePrint( 
        KUmacDetails, 
        (TUint8*)
        ("UMAC: WlanDot11SoftReset::ChangeInternalState(): new state:"));
    OsTracePrint( 
        KUmacDetails, 
        iStateName[aNewState] );
#endif

    iState = aNewState;
    Fsm( aCtxImpl, ESTATEENTRY );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11SoftReset::IssueDisconnect( 
    WlanContextImpl& aCtxImpl )
    {
    // change global state: entry procedure triggers action
    ChangeState( aCtxImpl, 
        *this,                      // prev state
        aCtxImpl.WsaDisconnect()    // next state
        );      
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11SoftReset::ConfigureQueue( 
    WlanContextImpl& aCtxImpl )
    {
    ConfigureTxQueue( aCtxImpl, WHA::ELegacy );    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
#ifndef NDEBUG 
const TInt8* WlanDot11SoftReset::GetStateName( TUint8& aLength ) const
    {
    aLength = sizeof( iName );
    return iName;
    }
#endif

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11SoftReset::Entry( WlanContextImpl& aCtxImpl )
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

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11SoftReset::Exit( WlanContextImpl& /*aCtxImpl*/ )
    {
    OsTracePrint( 
        KUmacProtocolState, 
        (TUint8*)("UMAC: WlanDot11SoftReset::Exit()"));

    // reset our local FSM for the next time...
    iState = EINIT;
    }
