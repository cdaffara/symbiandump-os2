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
* Description:   Event dispatcher that is used to serialize MAC prototocl
*                statemachine access
*
*/

/*
* %version: 27 %
*/

#include "config.h"
#include "umaceventdispatcher.h"

#include "umaceventdispatcherclient.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
WlanEventDispatcher::WlanEventDispatcher( 
    MWlanEventDispatcherClient& aEventDispatcherClient ) 
    : iEventDispatcherClient( aEventDispatcherClient ), 
      iCommandCompletionRegistered ( EFalse ), iDisableChannelMask( 0 ),
      iOid( NULL ), iInternalEvents( 0 )
    {
    };

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanEventDispatcher::Dispatch()
    {
    TBool state_transition_event( EFalse );

    if ( iInternalEvents )
        {
        OsTracePrint( KEventDispatcher, (TUint8*)
            ("UMAC: WlanEventDispatcher::Dispatch(): dispatch internal event") );

        state_transition_event = DispatchInternalEvent();
        }
    if ( !state_transition_event )
        {
        if ( iCommandCompletionRegistered )
            {
            OsTracePrint( KEventDispatcher, (TUint8*)
                ("UMAC: WlanEventDispatcher::Dispatch(): dispatch WHA command completion") );

            state_transition_event = DispatchCommandCompletionEvent();
            }
        if ( !state_transition_event )
            {
            if (// oid exists 
                iOid 
                // AND channel is NOT disabled
                && (!(iDisableChannelMask & KOidChannel)) )
                {
                OsTracePrint( KEventDispatcher, (TUint8*)
                    ("UMAC: WlanEventDispatcher::Dispatch(): dispatch OID") );

                state_transition_event = DispatchOidEvent();
                }
            }
        }
    return state_transition_event;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanEventDispatcher::Register( 
    WHA::TCompleteCommandId aCompleteCommandId,
    WHA::TStatus aStatus,
    const WHA::UCommandCompletionParams& aCommandCompletionParams )
    {
    if ( iCommandCompletionRegistered )
        {
        OsTracePrint( KEventDispatcher, (TUint8*)
            ("UMAC: WlanEventDispatcher::Register(): register cmd completion when one is already registered") );
        OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
        }

    iWhaCommandCompletionParams.iId = aCompleteCommandId;
    iWhaCommandCompletionParams.iStatus = aStatus;
    iWhaCommandCompletionParams.iParams = aCommandCompletionParams;
    
    iCommandCompletionRegistered = ETrue;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanEventDispatcher::DispatchInternalEvent()
    {
    TBool ret ( EFalse );

    if ( iInternalEvents & KAcParamUpdate )
        {
        iInternalEvents &= ~KAcParamUpdate;
        ret = iEventDispatcherClient.OnInternalEvent( KAcParamUpdate );
        }
    else if ( iInternalEvents & KPowerMgmtTransition )
        {
        iInternalEvents &= ~KPowerMgmtTransition;
        ret = iEventDispatcherClient.OnInternalEvent( KPowerMgmtTransition );
        }
    else if ( iInternalEvents & KTimeout)
        {
        iInternalEvents &= ~KTimeout;
        ret = iEventDispatcherClient.OnInternalEvent( KTimeout );
        }
    else if ( iInternalEvents & KVoiceCallEntryTimerTimeout)
        {
        iInternalEvents &= ~KVoiceCallEntryTimerTimeout;
        ret = iEventDispatcherClient.OnInternalEvent( 
            KVoiceCallEntryTimerTimeout );
        }
    else if ( iInternalEvents & KNullTimerTimeout)
        {
        iInternalEvents &= ~KNullTimerTimeout;
        ret = iEventDispatcherClient.OnInternalEvent( KNullTimerTimeout );
        }
    else if ( iInternalEvents & KNoVoiceTimerTimeout)
        {
        iInternalEvents &= ~KNoVoiceTimerTimeout;
        ret = iEventDispatcherClient.OnInternalEvent( KNoVoiceTimerTimeout );
        }
    else if ( iInternalEvents & KKeepAliveTimerTimeout)
        {
        iInternalEvents &= ~KKeepAliveTimerTimeout;
        ret = iEventDispatcherClient.OnInternalEvent( KKeepAliveTimerTimeout );
        }
    else if ( iInternalEvents & KActiveToLightPsTimerTimeout)
        {
        iInternalEvents &= ~KActiveToLightPsTimerTimeout;
        ret = iEventDispatcherClient.OnInternalEvent( 
            KActiveToLightPsTimerTimeout );
        }
    else if ( iInternalEvents & KLightPsToActiveTimerTimeout)
        {
        iInternalEvents &= ~KLightPsToActiveTimerTimeout;
        ret = iEventDispatcherClient.OnInternalEvent( 
            KLightPsToActiveTimerTimeout );
        }
    else if ( iInternalEvents & KLightPsToDeepPsTimerTimeout)
        {
        iInternalEvents &= ~KLightPsToDeepPsTimerTimeout;
        ret = iEventDispatcherClient.OnInternalEvent( 
            KLightPsToDeepPsTimerTimeout );
        }    
    else if ( iInternalEvents & KSetCtsToSelf)
        {
        iInternalEvents &= ~KSetCtsToSelf;
        ret = iEventDispatcherClient.OnInternalEvent( KSetCtsToSelf );
        }
    else if ( iInternalEvents & KSetRcpiTriggerLevel )
        {
        iInternalEvents &= ~KSetRcpiTriggerLevel;
        ret = iEventDispatcherClient.OnInternalEvent( KSetRcpiTriggerLevel );
        }
    else if ( iInternalEvents & KSetHtBssOperation )
        {
        iInternalEvents &= ~KSetHtBssOperation;
        ret = iEventDispatcherClient.OnInternalEvent( KSetHtBssOperation );
        }
    else
        {
        // programming error
        OsTracePrint( KErrorLevel, 
            (TUint8*)("UMAC: iInternalEvents"), iInternalEvents );    
        OsAssert((TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
        }

    return ret;
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanEventDispatcher::DispatchCommandCompletionEvent()
    {
    // make a local copy of the completion parameters
    const TCommandResp resp ( iWhaCommandCompletionParams );
    // clear the event flag
    iCommandCompletionRegistered = EFalse;

    // handle the event
    return ( iEventDispatcherClient.OnWhaCommandComplete( 
        resp.iId,
        resp.iStatus,
        resp.iParams ));    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanEventDispatcher::DispatchOidEvent()
    {
    TBool ret ( EFalse );
    switch ( iOid->oid_id )
        {
        case E802_11_CONFIGURE:
            ret = iEventDispatcherClient.HandleConfigure( iOid );
            break;
        case E802_11_CONNECT:
            ret = iEventDispatcherClient.HandleConnect( iOid );
            break;
        case E802_11_START_IBSS:
            ret = iEventDispatcherClient.HandleStartIBSS( iOid );
            break;
        case E802_11_SCAN:
            ret = iEventDispatcherClient.HandleScan( iOid );
            break;
        case E802_11_STOP_SCAN:
            ret = iEventDispatcherClient.HandleStopScan();
            break;
        case E802_11_DISCONNECT:
            ret = iEventDispatcherClient.HandleDisconnect();
            break;
        case E802_11_SET_POWER_MODE:
            ret = iEventDispatcherClient.HandleSetPowerMode( iOid );
            break;
        case E802_11_SET_RCPI_TRIGGER_LEVEL:
            ret = iEventDispatcherClient.HandleSetRcpiTriggerLevel( iOid );
            break;
        case E802_11_SET_TX_POWER_LEVEL:
            ret = iEventDispatcherClient.HandleSetTxPowerLevel( iOid );
            break;
        case E802_11_GET_LAST_RCPI:
            ret = iEventDispatcherClient.HandleGetLastRcpi();
            break;
        case E802_11_DISABLE_USER_DATA:
            ret = iEventDispatcherClient.HandleDisableUserData();
            break;
        case E802_11_ENABLE_USER_DATA:
            ret = iEventDispatcherClient.HandleEnableUserData();
            break;
        case E802_11_ADD_CIPHER_KEY:
            ret = iEventDispatcherClient.HandleAddCipherKey( iOid );
            break;
        case E802_11_ADD_MULTICAST_ADDR:
            ret = iEventDispatcherClient.HandleAddMulticastAddr( iOid );
            break;            
        case E802_11_REMOVE_MULTICAST_ADDR:
            ret = iEventDispatcherClient.HandleRemoveMulticastAddr( iOid );
            break;            
        case E802_11_CONFIGURE_BSS_LOST:
            ret = iEventDispatcherClient.HandleConfigureBssLost( iOid );
            break;            
        case E802_11_SET_TX_RATE_ADAPT_PARAMS:
            ret = iEventDispatcherClient.HandleSetTxRateAdaptParams( iOid );
            break;            
        case E802_11_CONFIGURE_TX_RATE_POLICIES:
            ret = iEventDispatcherClient.HandleConfigureTxRatePolicies( iOid );
            break;
        case E802_11_SET_POWER_MODE_MGMT_PARAMS:
            ret = iEventDispatcherClient.HandleSetPowerModeMgmtParams( iOid );
            break;            
        case E802_11_CONFIGURE_PWR_MODE_MGMT_TRAFFIC_OVERRIDE:
            ret = iEventDispatcherClient.HandleConfigurePwrModeMgmtTrafficOverride( iOid );
            break;
        case E802_11_GET_FRAME_STATISTICS:
            ret = iEventDispatcherClient.HandleGetFrameStatistics();
            break;            
        case E802_11_CONFIGURE_UAPSD:
            ret = iEventDispatcherClient.HandleConfigureUapsd( iOid );
            break;
        case E802_11_CONFIGURE_TX_QUEUE:
            ret = iEventDispatcherClient.HandleConfigureTxQueue( iOid );
            break;            
        case E802_11_GET_MAC_ADDRESS:
            ret = iEventDispatcherClient.HandleGetMacAddress();
            break;
        case E802_11_CONFIGURE_ARP_IP_ADDRESS_FILTERING:
            ret = iEventDispatcherClient.HandleConfigureArpIpAddressFiltering( iOid );
            break;
        case E802_11_CONFIGURE_HT_BLOCK_ACK:
            ret = iEventDispatcherClient.HandleConfigureHtBlockAck( iOid );
            break;
        case E802_11_CONFIGURE_PROPRIETARY_SNAP_HDR:
            ret = iEventDispatcherClient.HandleConfigureProprietarySnapHdr( 
                iOid );
            break;
        default:
            // catch programming error
            OsTracePrint( KErrorLevel, (TUint8*)
                ("UMAC: OID: 0x%08x"), iOid->oid_id );    
            OsAssert((TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
            break;
        }

    // mark as used
    iOid = NULL;
    return ret;
    }
