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
* Description:   Implementation of the WlanDot11IdleScanningMode class.
*
*/

/*
* %version: 19 %
*/

#include "config.h"
#include "umacdot11idlescanningmode.h"
#include "umacscanfsmcntx.h"
#include "UmacContextImpl.h"
#include "UmacWsaWriteMib.h"
#include "UmacWsaScan.h"
#include "umacwhastopscan.h"
#include "umacwhatodot11typeconverter.h"

#ifndef NDEBUG
const TInt8 WlanDot11IdleScanningMode::iName[] = "dot11-idlescanningmode";

const TUint8 WlanDot11IdleScanningMode::iStateName
    [ESTATEMAX][KMaxStateStringLength] = 
    {
        {"EINIT"}, 
        {"ESETSLEEPMODE"}, 
        {"EEXECUTESCANFSM"},
        {"ECONTINUEDOT11TRAVERSE"}
    };

const TUint8 WlanDot11IdleScanningMode::iEventName
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
WlanDot11IdleScanningMode::~WlanDot11IdleScanningMode() 
    {
    iCtxImpl = NULL;
    iChannels = NULL;
    iSSID = NULL;
    iPimpl = NULL;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11IdleScanningMode::Set( 
    WlanContextImpl& aCtxImpl )
    {
    iCtxImpl = &aCtxImpl;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11IdleScanningMode::Set( 
    TScanMode aMode,                    
    const TSSID& aSSID,                 
    TUint32 aScanRate,                    
    SChannels& aChannels,
    TUint32 aMinChannelTime,            
    TUint32 aMaxChannelTime,
    TBool aSplitScan )    
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
    }

// -----------------------------------------------------------------------------
// The internal state transition method.
// It is guaranteed by the FSM framework that no WHA command is pending
// when this method is entered
// -----------------------------------------------------------------------------
//
void WlanDot11IdleScanningMode::ChangeInternalState( 
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
void WlanDot11IdleScanningMode::Fsm( 
    WlanContextImpl& aCtxImpl, 
    TEvent aEvent )
    {
    OsTracePrint( KScan, 
        (TUint8*)("UMAC * dot11-idlescanningmode * FSM EVENT") );
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
void WlanDot11IdleScanningMode::OnStateEntryEvent( 
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
                if ( aCtxImpl.Reassociate() )
                    {
                    // a roaming case
                    ChangeInternalState( aCtxImpl, EEXECUTESCANFSM );
                    }
                else
                    {
                    // not a roaming case
                    ChangeInternalState( aCtxImpl, ESETSLEEPMODE );
                    }
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
void WlanDot11IdleScanningMode::OnTxCompleteEvent( 
    WlanContextImpl& aCtxImpl )
    {
    switch ( iState )
        {
        case ESETSLEEPMODE:
            ChangeInternalState( aCtxImpl, EEXECUTESCANFSM );
            break;
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
void WlanDot11IdleScanningMode::OnStartScanningModeEvent( 
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
        
        // 1st set the context
        aCtxImpl.WsaScan().Set( 
            aCtxImpl,
            iScanRate,
            iChannels->iBand,
            wha_channelcount,
            wha_channels,
            WHA::EFgScan,
            (iMode == EActiveScan) ? KNbrOfProbeReqsPerCh : KNoProbeReqs,
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
void WlanDot11IdleScanningMode::OnScanModeRunningEvent( 
    WlanContextImpl& aCtxImpl ) const
    {
    // complete the scan request
    
    OsTracePrint( KScan, (TUint8*)
        ("UMAC: WlanDot11IdleScanningMode::OnScanModeRunningEvent: Complete scan request") );
    OnOidComplete( aCtxImpl );            
    }

// -----------------------------------------------------------------------------
// Handler for the event.
// It is guaranteed by the FSM framework that no WHA command is pending
// when this method is entered
// -----------------------------------------------------------------------------
//
void WlanDot11IdleScanningMode::OnStopScanningModeEvent( 
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
void WlanDot11IdleScanningMode::OnScanningModeExitEvent( 
    WlanContextImpl& aCtxImpl )
    {
    // scanning mode has stopped and so can we
    
    // request the next state (dot11IdleState) to inform the mgmt client about 
    // scan completion upon state entry
    aCtxImpl.iStates.iIdleState.CompleteScanUponEntry();
    
    ChangeInternalState( aCtxImpl, ECONTINUEDOT11TRAVERSE );
    }

// ---------------------------------------------------------
// simulate macnotresponding error
// ---------------------------------------------------------
//
void WlanDot11IdleScanningMode::OnAbortEvent( 
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
void WlanDot11IdleScanningMode::SetSleepMode( 
    WlanContextImpl& aCtxImpl )
    {
    WHA::SsleepMode* mib 
        = static_cast<WHA::SsleepMode*>
        (os_alloc( sizeof( WHA::SsleepMode ) )); 

    if ( !mib )
        {
        // this is fatal
        Fsm( aCtxImpl, EABORT );
        return;
        }

    mib->iMode = WHA::KPowerDownMode;
    
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
void WlanDot11IdleScanningMode::ExecuteScanFsm( 
    WlanContextImpl& /*aCtxImpl*/ )
    {
    // start scan fsm
    iPimpl->Current().StartScanningMode( *iPimpl );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11IdleScanningMode::ContinueDot11StateTraversal( 
    WlanContextImpl& aCtxImpl )
    {
    // return back to dot11idle    
    ChangeState( aCtxImpl, 
        *this,                          // prev state
        aCtxImpl.iStates.iIdleState     // next state
        );
    }

// -----------------------------------------------------------------------------
// Handler for WHA command response event.
// It is guaranteed by the global protocol fsm framework that no WHA command is 
// pending when this method is entered
// -----------------------------------------------------------------------------
//
void WlanDot11IdleScanningMode::OnWhaCommandResponse( 
    WlanContextImpl& aCtxImpl, 
    WHA::TCommandId aCommandId, 
    WHA::TStatus /*aStatus*/,
    const WHA::UCommandResponseParams& /*aCommandResponseParams*/,
    TUint32 /*aAct*/ )
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
            ("UMAC: WlanDot11IdleScanningMode::OnWhaCommandResponse: complete stop scan request"));        

        OnOidComplete( aCtxImpl );
        }
    else
        {
        // has to be the write mib command response.
        // We have nothing to do here
        }
    }

// -----------------------------------------------------------------------------
// Handler for WHA command completion event.
// It is guaranteed by the global protocol fsm framework that no WHA command is 
// pending when this method is entered
// -----------------------------------------------------------------------------
//
TBool WlanDot11IdleScanningMode::CommandComplete( 
    WlanContextImpl& /*aCtxImpl*/, 
    WHA::TCompleteCommandId aCompleteCommandId, 
    WHA::TStatus /*aStatus*/,
    const WHA::UCommandCompletionParams& /*aCommandCompletionParams*/ )
    {
    if ( aCompleteCommandId != WHA::EScanComplete )
        {
        // implementation error as this can be the only one
        OsAssert( (TUint8*)("UMAC * panic"), (TUint8*)(WLAN_FILE), __LINE__ );
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
TAny* WlanDot11IdleScanningMode::RequestForBuffer( 
    WlanContextImpl& aCtxImpl, 
    TUint16 aLength )
    {
    return aCtxImpl.GetRxBuffer( aLength );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11IdleScanningMode::ReceivePacket( 
    WlanContextImpl& aCtxImpl, 
    WHA::TStatus aStatus,
    const void* aFrame,
    TUint16 aLength,
    WHA::TRate /*aRate*/,
    WHA::TRcpi aRcpi,
    WHA::TChannelNumber /*aChannel*/,
    TUint8* aBuffer,
    TUint32 /*aFlags*/ )
    {
    if ( aStatus == WHA::KSuccess )
        {
        // frame reception success

        const Sdot11MacHeader* hdr 
            = reinterpret_cast<Sdot11MacHeader*>(const_cast<TAny*>(aFrame));

        OsTracePrint( KRxFrame, (TUint8*)
            ("UMAC: dot11-idlescanningmode * frame rx * frame type: 0x%02x"), 
            hdr->iFrameControl.iType );

        if ( (hdr->iFrameControl.iType 
            == E802Dot11FrameTypeBeacon)
            || (hdr->iFrameControl.iType 
            == E802Dot11FrameTypeProbeResp) )
            {            
            // as we have a beacon or probe response frame we will pass it to 
            // the client of the mgmt interface
            XferDot11FrameToMgmtClient( 
                aCtxImpl, 
                aFrame, 
                aLength, 
                aRcpi, 
                aBuffer );
            }
        else
            {
            // other than scan frame received. This may be legal if the frame
            // was received before receiving WHA::CommandResponse for WHA::Scan
            // cmd. However, in other cases it may be an implementation error 
            // in the underlying adaptation layer. In any case the frame is not
            // applicable for us as we are doing dot11idle mode scan (we are 
            // not connected) and we will ignore it
            OsTracePrint( KRxFrame | KWarningLevel, (TUint8*)
                ("UMAC: WlanDot11IdleScanningMode::ReceivePacket: WARNING: Other than scan frame received. Frame type: 0x%02x"), 
                hdr->iFrameControl.iType );

            // release the Rx buffer
            aCtxImpl.iUmac.MarkRxBufFree( aBuffer );
            }
        }
    else
        {
        // frame reception failure
        OsTracePrint( KErrorLevel, (TUint8*)
            ("UMAC * dot11-idlescanningmode * frame rx failure * status: %d"), 
            aStatus );

        // release the Rx buffer
        aCtxImpl.iUmac.MarkRxBufFree( aBuffer );
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11IdleScanningMode::Entry( 
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
            ("UMAC: WlanDot11IdleScanningMode::Entry: Scan complete event registered") );
        
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
void WlanDot11IdleScanningMode::Exit( 
    WlanContextImpl& /*aCtxImpl*/ )
    {
    // reset fsm
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
TBool WlanDot11IdleScanningMode::StopScan( WlanContextImpl& /*aCtxImpl*/ )
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
const TInt8* WlanDot11IdleScanningMode::GetStateName( 
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
void WlanDot11IdleScanningMode::OnScanFsmEvent( 
    MWlanScanFsmCb::TEvent aEvent )
    {
    OsTracePrint( KScan, (TUint8*)
        ("UMAC: * dot11-idlescanningmode * SCAN FSM EVENT: %d"), aEvent );

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
                (TUint8*)("UMAC: scan fsm event: :%d"), aEvent);
            OsAssert( (TUint8*)("UMAC: panic"), 
                (TUint8*)(WLAN_FILE), __LINE__ );
            break;
        }
    }
