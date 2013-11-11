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
* Description:   Implementation of the WlanDot11InfrastructureScanningMode 
*                class.
*
*/

/*
* %version: 21 %
*/

#include "config.h"
#include "umacdot11infrastructurescanningmode.h"
#include "umacscanfsmcntx.h"
#include "UmacContextImpl.h"
#include "UmacWsaWriteMib.h"
#include "UmacWsaScan.h"
#include "umacwhastopscan.h"
#include "umacwhatodot11typeconverter.h"


#ifndef NDEBUG
const TInt8 WlanDot11InfrastructureScanningMode::iName[] 
    = "dot11-infrastructurescanningmode";

const TUint8 WlanDot11InfrastructureScanningMode::iStateName
    [ESTATEMAX][KMaxStateStringLength] = 
    {
        {"EINIT"}, 
        {"EEXECUTESCANFSM"},
        {"ECONTINUEDOT11TRAVERSE"}
    };

const TUint8 WlanDot11InfrastructureScanningMode::iEventName
    [EEVENTMAX][KMaxEventStringLength] = 
    {
        {"ESTATEENTRY"}, 
        {"ETXCOMPLETE"},
        {"ESTARTSCANNINGMODE"},
        {"ESCANMODERUNNING"},
        {"ESTOPSCANNINGMODE"},
        {"ESCANNINGMODEEXIT"},
        {"EABORT"}
    };
#endif

// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
WlanDot11InfrastructureScanningMode::~WlanDot11InfrastructureScanningMode() 
    {
    iCtxImpl = NULL;
    iSSID = NULL;
    iChannels = NULL;
    iPimpl = NULL;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11InfrastructureScanningMode::Set( 
    WlanContextImpl& aCtxImpl )
    {
    iCtxImpl = &aCtxImpl;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11InfrastructureScanningMode::Set( 
    TScanMode aMode,                    
    const TSSID& aSSID,                 
    TUint32 aScanRate,                    
    SChannels& aChannels,
    TUint32 aMinChannelTime,            
    TUint32 aMaxChannelTime,
    TBool aSplitScan,
    WHA::TScanType aScanType )    
    {
    // we can store pointers as oid parameters are guaranteed to be valid 
    // to the point the corresponding completion method is called
    iMode = aMode;
    iSSID = &aSSID;
    iScanRate = aScanRate;
    iChannels = &aChannels;
    iMinChannelTime = aMinChannelTime;           
    iMaxChannelTime = aMaxChannelTime;
    iSplitScan = aSplitScan;
    iScanType = aScanType;
    }

// -----------------------------------------------------------------------------
// The internal state transition method.
// It is guaranteed by the FSM framework that no WHA command is pending
// when this method is entered
// -----------------------------------------------------------------------------
//
void WlanDot11InfrastructureScanningMode::ChangeInternalState( 
    WlanContextImpl& aCtxImpl, 
    TState aNewState )
    {
    iState = aNewState;
    Fsm( aCtxImpl, ESTATEENTRY );
    }

// -----------------------------------------------------------------------------
// The event dispatcher method.
// It is guaranteed by the FSM framework that no WHA command is pending
// when this method is entered
// -----------------------------------------------------------------------------
//
void WlanDot11InfrastructureScanningMode::Fsm( 
    WlanContextImpl& aCtxImpl, 
    TEvent aEvent )
    {
    OsTracePrint( KScan, 
        (TUint8*)("UMAC * dot11-infrastructurescanningmode * FSM EVENT") );
#ifndef NDEBUG
    OsTracePrint( KScan, (TUint8*)("event:"));
    OsTracePrint( KScan, iEventName[aEvent] );
    OsTracePrint( KScan, (TUint8*)("state:"));
    OsTracePrint( KScan, iStateName[iState] );
#endif

    switch ( aEvent )
        {
        case ESTATEENTRY:
            OnStateEntryEvent( aCtxImpl );
            break;
        case ETXCOMPLETE:
            OnTxCompleteEvent( aCtxImpl );
            break;
        case ESTARTSCANNINGMODE:
            OnStartScanningModeEvent( aCtxImpl );
            break;
        case ESCANMODERUNNING:
            OnScanModeRunningEvent( aCtxImpl );
            break;
        case ESTOPSCANNINGMODE:
            OnStopScanningModeEvent( aCtxImpl );
            break;
        case ESCANNINGMODEEXIT:
            OnScanningModeExitEvent( aCtxImpl );
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
// Handler for the event.
// It is guaranteed by the FSM framework that no WHA command is pending
// when this method is entered
// -----------------------------------------------------------------------------
//
void WlanDot11InfrastructureScanningMode::OnStateEntryEvent( 
    WlanContextImpl& aCtxImpl )
    {
    switch ( iState )
        {
        case EINIT:
            iPimpl = static_cast<WlanScanFsmCntx*>
                (os_alloc( sizeof(WlanScanFsmCntx) ));
            if ( iPimpl )
                {
                // allocation success
                new (iPimpl) WlanScanFsmCntx( *this );
                // start fsm traversal
                ChangeInternalState( aCtxImpl, EEXECUTESCANFSM );                
                }
            else
                {
                // allocation failure
                Fsm( aCtxImpl, EABORT );
                }

            break;
        case EEXECUTESCANFSM:
            ExecuteScanFsm( aCtxImpl );
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

// -----------------------------------------------------------------------------
// Handler for the event.
// It is guaranteed by the FSM framework that no WHA command is pending
// when this method is entered
// -----------------------------------------------------------------------------
//
void WlanDot11InfrastructureScanningMode::OnTxCompleteEvent( 
    WlanContextImpl& /*aCtxImpl*/ ) const
    {
    switch ( iState )
        {
        case EEXECUTESCANFSM:
            // we are running the scan fsm 
            // and we have nothing to do here
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
// Handler for the event.
// It is guaranteed by the FSM framework that no WHA command is pending
// when this method is entered
// -----------------------------------------------------------------------------
//
void WlanDot11InfrastructureScanningMode::OnStartScanningModeEvent( 
    WlanContextImpl& aCtxImpl )
    {
    // start the scanning mode

    // convert scan parameters to WHA types
    // Note that we need to deallocate the memory from whaChannels
    // - which is allocated in ConvertToWhaTypes() - when we are done

    WHA::SSSID* wha_ssid = 
        static_cast<WHA::SSSID*>(os_alloc( sizeof(WHA::SSSID) )); 
    if ( !wha_ssid )
        {
        // memory allocation failue; that's fatal
        Fsm( aCtxImpl, EABORT );
        return;
        }

    WHA::SChannels* wha_channels( NULL );
    TUint8 wha_channelcount( 0 );

    if ( !(WlanWhaToDot11TypeConverter::ConvertToWhaTypes(       
        aCtxImpl, 
        iSSID, 
        *wha_ssid, 
        iChannels, 
        iMinChannelTime,
        iMaxChannelTime,
        wha_channels, 
        wha_channelcount )) )
        {
        // memory allocation failue; that's fatal
        os_free( wha_ssid );
        Fsm( aCtxImpl, EABORT );
        }
    else
        {
        // start the scanning process

        const TUint8 KNbrOfProbeReqsPerCh = 2;
        const TUint8 KNoProbeReqs = 0;        
        const TUint8 KNbrOfSsids = 1;
        const TUint8 KNoSsids = 0;
        
        // in the most common case we use forced background
        // scan, so that we wouldn't lose data frames while scanning.
        if ( aCtxImpl.DisassociatedByAp() )
            {
            if ( aCtxImpl.CurrentDot11PwrMgmtMode() == WHA::KPsDisable )
                {
                // we have been disassociated by the AP and we are
                // currently in Active mode => use foreground scan, so that we 
                // won't send the AP any more class 3 frames (to inform it 
                // about us moving to PS mode)
                iScanType = WHA::EFgScan;
                }
            }
        
        // set the context
        aCtxImpl.WsaScan().Set( 
            aCtxImpl,
            iScanRate,
            iChannels->iBand,
            wha_channelcount,
            wha_channels,
            iScanType,
            (iMode == EActiveScan) ? KNbrOfProbeReqsPerCh : KNoProbeReqs,
            // in the normal case, do a split scan
            iSplitScan,
            wha_ssid->iSSIDLength ? KNbrOfSsids : KNoSsids,
            wha_ssid );
        
        // change global state: entry procedure triggers action
        ChangeState( aCtxImpl, 
            *this,              // prev state
            aCtxImpl.WsaScan()  // next state
            );
            
        // as the parameters have been supplied we can now deallocate
        os_free( wha_ssid );
        os_free( wha_channels );                                    
        }
    }

// -----------------------------------------------------------------------------
// Handler for the event.
// It is guaranteed by the FSM framework that no WHA command is pending
// when this method is entered
// -----------------------------------------------------------------------------
//
void WlanDot11InfrastructureScanningMode::OnScanModeRunningEvent( 
    WlanContextImpl& aCtxImpl ) const
    {
    // complete the scan request
    
    OsTracePrint( KScan, (TUint8*)
        ("UMAC: WlanDot11InfrastructureScanningMode::OnScanModeRunningEvent: Complete scan request") );
    OnOidComplete( aCtxImpl );            
    }

// -----------------------------------------------------------------------------
// Handler for the event.
// It is guaranteed by the FSM framework that no WHA command is pending
// when this method is entered
// -----------------------------------------------------------------------------
//
void WlanDot11InfrastructureScanningMode::OnStopScanningModeEvent( 
    WlanContextImpl& aCtxImpl )
    {
    // change global state: entry procedure triggers action
    ChangeState( aCtxImpl, 
        *this,                  // prev state
        aCtxImpl.WhaStopScan()  // next state
        );
    }

// -----------------------------------------------------------------------------
// Handler for the event.
// It is guaranteed by the FSM framework that no WHA command is pending
// when this method is entered
// -----------------------------------------------------------------------------
//
void WlanDot11InfrastructureScanningMode::OnScanningModeExitEvent( 
    WlanContextImpl& aCtxImpl )
    {
    // scanning mode has stopped and so can we
    
    // inform the mgmt client about scan completion

    OsTracePrint( KScan, (TUint8*)
        ("UMAC: WlanDot11InfrastructureScanningMode::OnScanningModeExitEvent: Send scan complete indication"));
    
    OnInDicationEvent( aCtxImpl, EScanCompleted );
    
    // continue with the dot11 state traversal
    ChangeInternalState( aCtxImpl, ECONTINUEDOT11TRAVERSE );
    }

// ---------------------------------------------------------
// simulate macnotresponding error
// ---------------------------------------------------------
//
void WlanDot11InfrastructureScanningMode::OnAbortEvent( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( KWarningLevel, 
        (TUint8*)("UMAC * dot11-idlescanningmode * abort") );

    DoErrorIndication( aCtxImpl, WHA::KErrorMacNotResponding );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11InfrastructureScanningMode::ExecuteScanFsm( 
    WlanContextImpl& /*aCtxImpl*/ )
    {
    // start scan fsm
    iPimpl->Current().StartScanningMode( *iPimpl );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11InfrastructureScanningMode::ContinueDot11StateTraversal( 
    WlanContextImpl& aCtxImpl )
    {
    if ( iFlags & KDot11StateBackTrack )
        {
        // recall previous dot11 state
        ChangeState( aCtxImpl, 
            *this,              // prev state
            Dot11History()      // next state
            );
        }
    else
        {
        // we go somewhere else

        if ( !(iFlags & KDot11PwrMgmtModeDifference) )
            {
            // no dot11 power management mode difference 
            // between pre and post scanning mode detected
            ChangeState( aCtxImpl, 
                *this,                                      // prev state
                aCtxImpl.iStates.iInfrastructureNormalMode  // next state
                );
            }
        else
            {
            // dot11 power management mode difference 
            // between pre and post scanning mode detected
            
            // initialize the counter which we use to determine if we should 
            // indicate BSS lost if we continuously are unsuccessful in changing the
            // power mgmt mode
            aCtxImpl.InitializeSetPsModeCount();
            
            ChangeState( aCtxImpl, 
                *this,                                      // prev state
                aCtxImpl.iStates.iPwrMgmtTransitionMode     // next state
                );
            }
        }
    }

// -----------------------------------------------------------------------------
// Handler for WHA command response event.
// It is guaranteed by the global protocol fsm framework that no WHA command is 
// pending when this method is entered
// -----------------------------------------------------------------------------
//
void WlanDot11InfrastructureScanningMode::OnWhaCommandResponse( 
    WlanContextImpl& aCtxImpl, 
    WHA::TCommandId aCommandId, 
    WHA::TStatus aStatus,
    const WHA::UCommandResponseParams& aCommandResponseParams,
    TUint32 aAct )
    {
    if ( aCommandId == WHA::EScanCommandResponse )
        {
        OsTracePrint( KScan, (TUint8*)("UMAC * OnScanningModeStarted"));

        // morph event to the scan fsm
        iPimpl->Current().OnScanningModeStarted( *iPimpl );
        }
    else if ( aCommandId == WHA::EStopScanResponse )
        {
        // complete the stop scan request

        OsTracePrint( KScan, (TUint8*)
            ("UMAC: WlanDot11InfrastructureScanningMode::OnWhaCommandResponse: complete stop scan request"));

        OnOidComplete( aCtxImpl );
        }
    else
        {
        // this is a response to a command that was not generated 
        // by this dot11 state object
        // so let's forward it to the default handler 
        WlanDot11State::OnWhaCommandResponse(
            aCtxImpl, 
            aCommandId, 
            aStatus,
            aCommandResponseParams,
            aAct );
        }
    }

// -----------------------------------------------------------------------------
// Handler for WHA command completion event.
// It is guaranteed by the global protocol fsm framework that no WHA command is 
// pending when this method is entered
// -----------------------------------------------------------------------------
//
TBool WlanDot11InfrastructureScanningMode::CommandComplete( 
    WlanContextImpl& aCtxImpl, 
    WHA::TCompleteCommandId aCompleteCommandId, 
    WHA::TStatus /*aStatus*/,
    const WHA::UCommandCompletionParams& aCommandCompletionParams )
    {
    if ( aCompleteCommandId != WHA::EScanComplete )
        {
        // implementation error as this can be the only one
        OsAssert( (TUint8*)("UMAC * panic"), (TUint8*)(WLAN_FILE), __LINE__ );
        }
    
    const WHA::TPsMode current_ps_mode( 
        aCommandCompletionParams.iScanComplete.iDot11PowerManagementMode );
    aCtxImpl.CurrentDot11PwrMgmtMode( current_ps_mode );

    OsTracePrint( KPwrStateTransition | KScan, 
        (TUint8*)("current dot11 power managememnt mode after scanning mode: %d"), 
        current_ps_mode );

    if ( current_ps_mode != aCtxImpl.DesiredDot11PwrMgmtMode() 
        && !(iFlags & KDot11StateBackTrack) )
        {
        // difference in dot11 power management 
        // mode between actual and desired mode exists 
        // and we not in dot11 state backtrack mode
        OsTracePrint( KWarningLevel, 
            (TUint8*)("UMAC * dot11-infrastructurescanningmode") );
        OsTracePrint( KWarningLevel, 
            (TUint8*)("difference in dot11 power management mode between") );
        OsTracePrint( KWarningLevel, 
            (TUint8*)("actual and desired mode detected") );
        OsTracePrint( KWarningLevel, (TUint8*)("post scanning mode state: %d"),
            current_ps_mode );

        iFlags |= KDot11PwrMgmtModeDifference;
        }

    OsTracePrint( KScan, (TUint8*)("UMAC * OnScanningModeStopped"));

    // morph event to scan fsm
    iPimpl->Current().OnScanningModeStopped( *iPimpl );

    // as scanning mode has ended we will change dot11 state.
    // Signal it to caller
    return ETrue;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TAny* WlanDot11InfrastructureScanningMode::RequestForBuffer( 
    WlanContextImpl& aCtxImpl, 
    TUint16 aLength )
    {
    return aCtxImpl.GetRxBuffer( aLength );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11InfrastructureScanningMode::OnBeaconFrameRx( 
    WlanContextImpl& aCtxImpl,
    const TAny* aFrame,
    const TUint32 aLength,
    WHA::TRcpi aRcpi,
    TUint8* aBuffer )
    {
    // as we have a beacon frame
    // we will pass it to the client of the mgmt interface
    XferDot11FrameToMgmtClient( aCtxImpl, aFrame, aLength, aRcpi, aBuffer );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11InfrastructureScanningMode::OnProbeResponseFrameRx( 
    WlanContextImpl& aCtxImpl,
    const TAny* aFrame,
    const TUint32 aLength,
    WHA::TRcpi aRcpi,
    TUint8* aBuffer )
    {
    // as we have a probe response frame
    // we will pass it to the client of the mgmt interface
    XferDot11FrameToMgmtClient( aCtxImpl, aFrame, aLength, aRcpi, aBuffer );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11InfrastructureScanningMode::Entry( 
    WlanContextImpl& aCtxImpl )
    {
    if ( aCtxImpl.WsaCmdActive() )
        {
        // sanity checking code
        OsAssert( (TUint8*)("UMAC * panic"), (TUint8*)(WLAN_FILE), __LINE__ );
        }

    // we don't want to do full event dispatching here as we want
    // to run this dot11 state in non pre-emptive mode.
    // However, we do want to handle the scan complete event in case it has
    // been registered
    if ( aCtxImpl.CommandCompletionEventRegistered( WHA::EScanComplete ) )
        {
        OsTracePrint( KScan, (TUint8*)
            ("UMAC: WlanDot11InfrastructureScanningMode::Entry: Scan complete event registered") );
        
        aCtxImpl.DispatchCommandCompletionEvent();
        // in this case we have actually already changed to another state
        // so there's nothing more for us to do here
        }
    else
        {
        // scan complete event not registered. Continue in the normal fashion

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
            // this is the start of the FSM actions
            Fsm( aCtxImpl, ESTATEENTRY );
            }
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11InfrastructureScanningMode::Exit( 
    WlanContextImpl& /*aCtxImpl*/ )
    {
    // reset fsm
    iFlags = 0;
    iState = EINIT;

    // get rid of scan fsm implementation
    iPimpl->~WlanScanFsmCntx();
    os_free( iPimpl );
    iPimpl = NULL;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11InfrastructureScanningMode::StopScan( 
    WlanContextImpl& /*aCtxImpl*/ )
    {
    // inform scan fsm that scanning mode stop is desired
    iPimpl->Current().StopScanningMode( *iPimpl );
    // indicate that a state change has occurred
    return ETrue;
    }

#ifndef NDEBUG                                    
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
const TInt8* WlanDot11InfrastructureScanningMode::GetStateName( 
    TUint8& aLength ) const
    {
    aLength = sizeof( iName );
    return iName;
    }
#endif

// -----------------------------------------------------------------------------
// Handler for scan fsm event.
// It is guaranteed by the FSM framework that no WHA command is pending
// when this method is entered
// -----------------------------------------------------------------------------
//
void WlanDot11InfrastructureScanningMode::OnScanFsmEvent( 
    MWlanScanFsmCb::TEvent aEvent )
    {
    OsTracePrint( KScan, (TUint8*)
        ("UMAC * dot11-infrastructurescanningmode * SCAN FSM EVENT: %d"), aEvent );

    // do what the scan fsm desires
    switch( aEvent )
        {
        case EFSMSTARTSCANNINGMODE:
            // scanning mode start desired
            Fsm( *iCtxImpl, ESTARTSCANNINGMODE );
            break;
        case EFSMSCANMODERUNNING:
            // scanning mode is running
            Fsm( *iCtxImpl, ESCANMODERUNNING );            
            break;
        case EFSMSTOPSCANNINGMODE:
            // scanning mode stop desired
            Fsm( *iCtxImpl, ESTOPSCANNINGMODE );
            break;
        case EFSMSCANMODESTOPPED:
            // scanning mode has stopped
            Fsm( *iCtxImpl, ESCANNINGMODEEXIT );
            break;
        default:
            // catch scan FSM programming error
            OsTracePrint( KErrorLevel, 
                (TUint8*)("UMAC* scan fsm event: :%d"), aEvent);
            OsAssert( (TUint8*)("* UMAC * panic"), 
                (TUint8*)(WLAN_FILE), __LINE__ );
            break;
        }
    }
