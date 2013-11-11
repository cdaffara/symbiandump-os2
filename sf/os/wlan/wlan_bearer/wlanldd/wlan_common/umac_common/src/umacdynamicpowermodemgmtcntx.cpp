/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of the WlanDynamicPowerModeMgmtCntx class
*
*/

/*
* %version: 15 %
*/

#include "config.h"
#include "umacdynamicpowermodemgmtcntx.h"
#include "UmacContextImpl.h"

// default time interval in microseconds after which transition from Active
// mode to Light PS mode is considered. 
// This value is used if another value hasn't been provided
const TUint32 KDefaultToLightPsTimeout = 100000; // 100ms

// default time interval in microseconds after which the frame counter
// used when considering transition from Light PS to Active mode is reset.
// This value is used if another value hasn't been provided
const TUint32 KDefaultToActiveTimeout = 300000; // 300ms

// default time interval in microseconds after which transition from Light PS
// mode to Deep PS mode is considered. 
// This value is used if another value hasn't been provided
const TUint32 KDefaultToDeepPsTimeout = 1000000; // 1s


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
WlanDynamicPowerModeMgmtCntx::WlanDynamicPowerModeMgmtCntx( 
    WlanContextImpl& aWlanCtxImpl ) : 
    iStateChange( EFalse ), 
    iFlags( 0 | KStayInPsDespiteUapsdVoiceTraffic ),
    iToLightPsTimeout( KDefaultToLightPsTimeout ),
    iToActiveTimeout( KDefaultToActiveTimeout ),
    iToDeepPsTimeout( KDefaultToDeepPsTimeout ),
    iActiveCntx( NULL ),
    iWlanContextImpl( aWlanCtxImpl ) 
    {
    iActiveParamsBackup.iToLightPsTimeout = KDefaultToLightPsTimeout;
    iActiveParamsBackup.iToLightPsFrameThreshold = 
        WlanActiveModePowerModeMgr::KDefaultToLightPsFrameThreshold;
    iActiveParamsBackup.iUapsdRxFrameLengthThreshold = 
        WlanPowerModeMgrBase::KDefaultUapsdRxFrameLengthThreshold;
    os_memset( iIgnoreTraffic, 0, sizeof( iIgnoreTraffic ) );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
WlanDynamicPowerModeMgmtCntx::~WlanDynamicPowerModeMgmtCntx() 
    {
    CancelTimeouts();
    iActiveCntx = NULL;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDynamicPowerModeMgmtCntx::StartPowerModeManagement()
    {
    if ( !iActiveCntx )
        {
        const TPowerMgmtMode KCurrentPwrMgmtMode ( 
            iWlanContextImpl.CurrentPwrMgmtMode() );
        
        switch ( KCurrentPwrMgmtMode )
            {
            case EActive:
                OsTracePrint( KPwrStateTransition, (TUint8*)
                    ("UMAC: WlanDynamicPowerModeMgmtCntx::StartPowerModeManagement: Activating Active mode context") );
    
                iActiveCntx = &iActiveModeCntx;
                RegisterToLightPsTimeout();            
                break;
            case ELightPs:
                OsTracePrint( KPwrStateTransition, (TUint8*)
                    ("UMAC: WlanDynamicPowerModeMgmtCntx::StartPowerModeManagement: Activating Light PS mode context") );
    
                iActiveCntx = &iLightPsModeCntx;
    
                if ( // we have roamed to the current AP
                     iWlanContextImpl.Reassociate() &&
                     // we have not yet indicated to WLAN Mgmt Client the 
                     // opportunity to perform tests on the current AP
                     !iWlanContextImpl.ApTestOpportunityIndicated() )
                    {
                    // start to seek for the test opportunity
                    iWlanContextImpl.ApTestOpportunitySeekStarted( ETrue );
    
                    OsTracePrint( KUmacDetails, (TUint8*)
                        ("UMAC: WlanDynamicPowerModeMgmtCntx::StartPowerModeManagement: Start AP test opportunity seek") );
                    }
                    
                RegisterToActiveTimeout();
                RegisterToDeepPsTimeout();
                break;
            case EDeepPs:
                OsTracePrint( KPwrStateTransition, (TUint8*)
                    ("UMAC: WlanDynamicPowerModeMgmtCntx::StartPowerModeManagement: Activating Deep PS mode context") );
    
                iActiveCntx = &iDeepPsModeCntx;
                break;
            default:
                // implementation error
                OsAssert( 
                    (TUint8*)("UMAC: panic"),
                    (TUint8*)(WLAN_FILE), 
                    __LINE__ );
            }
        
        // reset active context prior use
        iActiveCntx->Reset();
        // and also reset the state change flag
        iStateChange = EFalse;
        }
    else
        {
        // we already have an active context, so no action needed
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDynamicPowerModeMgmtCntx::StopPowerModeManagement()
    {
    if ( iActiveCntx )
        {
        OsTracePrint( KPwrStateTransition, (TUint8*)
            ("UMAC: WlanDynamicPowerModeMgmtCntx::StopPowerModeManagement: dynamic power mode mgmt stopped") );

        CancelTimeouts();
        iStateChange = EFalse;
        iActiveCntx = NULL; // no active context any more
        }
    }
        
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TPowerMgmtModeChange WlanDynamicPowerModeMgmtCntx::OnFrameTx( 
    WHA::TQueueId aQueueId,
    TUint16 aEtherType,
    T802Dot11FrameControlTypeMask aDot11FrameType )
    {
    TPowerMgmtModeChange powerMgmtModeChange( ENoChange );

    if ( iActiveCntx )
        {
        // we have an active context, i.e. we are doing power mode mgmt
        
        if ( !iStateChange )
            {
            powerMgmtModeChange = iActiveCntx->OnFrameTx( 
                iWlanContextImpl, 
                aQueueId,
                aEtherType,
                aDot11FrameType,
                iIgnoreTraffic[aQueueId] );
            
            if ( powerMgmtModeChange != ENoChange )
                {
                iStateChange = ETrue;
            
                // as we will do a mode change, cancel any possibly running 
                // power mode management timers
                CancelTimeouts();
                
                if ( aDot11FrameType == E802Dot11FrameTypeDataNull ||
                     aEtherType == KArpType )
                    {
                    // modify temporarily the Active mode parameters
                    SetKeepAliveActiveModeParameters();
                    }
                }
            }        
        else
            {
            // state change already signalled from this power mode context, 
            // don't do it more than once. No action needed
            OsTracePrint( KPwrStateTransition, (TUint8*)
                ("UMAC: WlanDynamicPowerModeMgmtCntx::OnFrameTx: statechange already signalled") );
            }
        }
    else
        {
        // dynamic power mode mgmt is not active => "No change" will be 
        // returned. No action needed
        }
        
    OsTracePrint( KPwrStateTransition, (TUint8*)
        ("UMAC: WlanDynamicPowerModeMgmtCntx::OnFrameTx: statechange: %d"),
        powerMgmtModeChange );
    
    return powerMgmtModeChange;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TPowerMgmtModeChange WlanDynamicPowerModeMgmtCntx::OnFrameRx(
    WHA::TQueueId aAccessCategory,
    TUint16 aEtherType,
    TUint aPayloadLength,
    TDaType aDaType ) 
    { 
    TPowerMgmtModeChange powerMgmtModeChange( ENoChange );

    if ( iActiveCntx )
        {
        // we have an active context, i.e. we are doing power mode mgmt
        
        if ( !iStateChange )
            {
            powerMgmtModeChange = iActiveCntx->OnFrameRx( 
                iWlanContextImpl, 
                aAccessCategory,
                aEtherType,
                iIgnoreTraffic[aAccessCategory], 
                aPayloadLength,
                aDaType );
            
            if ( powerMgmtModeChange != ENoChange )
                {
                iStateChange = ETrue;
            
                // as we will do a mode change, cancel any possibly running 
                // power mode management timers
                CancelTimeouts();
                }
            }        
        else
            {
            // state change already signalled from this power mode context, 
            // don't do it more than once. No action needed
            OsTracePrint( KPwrStateTransition, (TUint8*)
                ("UMAC: WlanDynamicPowerModeMgmtCntx::OnFrameRx: statechange already signalled") );
            }
        }
    else
        {
        // dynamic power mode mgmt is not active => "No change" will be 
        // returned. No action needed
        }
        
    OsTracePrint( KPwrStateTransition, (TUint8*)
        ("UMAC: WlanDynamicPowerModeMgmtCntx::OnFrameRx: statechange: %d"),
        powerMgmtModeChange );
    
    return powerMgmtModeChange;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TPowerMgmtModeChange WlanDynamicPowerModeMgmtCntx::OnPsModeErrorIndication()
    {
    TPowerMgmtModeChange powerMgmtModeChange( ENoChange );

    if ( iActiveCntx )
        {
        // we have an active context, i.e. we are doing power mode mgmt
        
        if ( !iStateChange )
            {
            powerMgmtModeChange = iActiveCntx->OnPsModeErrorIndication();
            
            if ( powerMgmtModeChange != ENoChange )
                {
                iStateChange = ETrue;
                
                // as we will do a mode change, cancel any possibly running 
                // power mode management timers
                CancelTimeouts();
                
                // modify temporarily the Active mode parameters
                SetPsModeErrorActiveModeParameters();
                }
            }
        else
            {
            // state change already signalled from this power mode context, 
            // don't do it more than once. No action needed
            OsTracePrint( KPwrStateTransition, (TUint8*)
                ("UMAC: WlanDynamicPowerModeMgmtCntx::OnPsModeErrorIndication: "
                 "statechange already signalled") );
            }
        }
    else
        {
        // dynamic power mode mgmt is not active => "No change" will be 
        // returned. No action needed
        }
        
    OsTracePrint( KPwrStateTransition, (TUint8*)
        ("UMAC: WlanDynamicPowerModeMgmtCntx::OnPsModeErrorIndication: "
         "statechange: %d"),
        powerMgmtModeChange );
    
    return powerMgmtModeChange;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanDynamicPowerModeMgmtCntx::OnActiveToLightPsTimerTimeout()
    {
    if ( ( iActiveCntx == &iActiveModeCntx ) && 
         ( iFlags & KToLightPsTimerStarted ) &&
         ( !iStateChange ) )
        {
        iFlags &= ~KToLightPsTimerStarted;
        
        iStateChange = iActiveCntx->OnActiveToLightPsTimerTimeout();
        
        OsTracePrint( KPwrStateTransition, (TUint8*)
            ("UMAC: WlanDynamicPowerModeMgmtCntx::OnActiveToLightPsTimerTimeout: change state: %d"),
            iStateChange );
        
        // make sure that the WLAN Mgmt Client provided Active mode 
        // parameter values are again used the next time by default
        //
        RestoreActiveModeParameters();
        
        if ( !iStateChange )
            {
            RegisterToLightPsTimeout();
            }
        }
    else
        {
        // either 
        // the timeout occurred in a wrong context or totally without an active
        // context (i.e. when we are not doing dynamic power mode mgmt) OR
        // the timer is not started, i.e. the timeout occurred when we weren't 
        // expecting it (which means that the timeout callback had already been
        // registered when we tried to cancel this timer the previous time) OR
        // a state change has already been signalled from this power mode 
        // context.
        // In all these cases the timeout is not relevant and we take no action
        OsTracePrint( KPwrStateTransition, (TUint8*)
            ("UMAC: WlanDynamicPowerModeMgmtCntx::OnActiveToLightPsTimerTimeout: not relevant timeout") );
        
        // however, make sure that the WLAN Mgmt Client provided Active mode
        // parameter values are again used the next time by default
        //
        RestoreActiveModeParameters();        
        }
    
    return iStateChange;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanDynamicPowerModeMgmtCntx::OnLightPsToActiveTimerTimeout()
    {
    if ( ( iActiveCntx == &iLightPsModeCntx ) && 
         ( iFlags & KToActiveTimerStarted ) &&
         ( !iStateChange ) )
        {
        iFlags &= ~KToActiveTimerStarted;

        iStateChange = iActiveCntx->OnLightPsToActiveTimerTimeout( 
            iWlanContextImpl );

        OsTracePrint( KPwrStateTransition, (TUint8*)
            ("UMAC: WlanDynamicPowerModeMgmtCntx::OnLightPsToActiveTimerTimeout: change state: %d"),
            iStateChange );
        
        // as we currently never do a mode change when this timeout occurs,
        // we arm the timer again
        RegisterToActiveTimeout();
        }
    else
        {
        // either 
        // the timeout occurred in a wrong context or totally without an active
        // context (i.e. when we are not doing dynamic power mode mgmt) OR
        // the timer is not started, i.e. the timeout occurred when we weren't 
        // expecting it (which means that the timeout callback had already been
        // registered when we tried to cancel this timer the previous time) OR
        // a state change has already been signalled from this power mode 
        // context.
        // In all these cases the timeout is not relevant and we take no action
        OsTracePrint( KPwrStateTransition, (TUint8*)
            ("UMAC: WlanDynamicPowerModeMgmtCntx::OnLightPsToActiveTimerTimeout: not relevant timeout") );
        }
    
    return iStateChange;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanDynamicPowerModeMgmtCntx::OnLightPsToDeepPsTimerTimeout()
    {
    if ( ( iActiveCntx == &iLightPsModeCntx ) && 
         ( iFlags & KToDeepPsTimerStarted ) &&
         ( !iStateChange ) )
        {
        iFlags &= ~KToDeepPsTimerStarted;

        iStateChange = iActiveCntx->OnLightPsToDeepPsTimerTimeout();
        
        OsTracePrint( KPwrStateTransition, (TUint8*)
            ("UMAC: WlanDynamicPowerModeMgmtCntx::OnLightPsToDeepPsTimerTimeout: change state: %d"),
            iStateChange );
        
        if ( !iStateChange )
            {
            RegisterToDeepPsTimeout();
            }
        }
    else
        {
        // either 
        // the timeout occurred in a wrong context or totally without an active
        // context (i.e. when we are not doing dynamic power mode mgmt) OR
        // the timer is not started, i.e. the timeout occurred when we weren't 
        // expecting it (which means that the timeout callback had already been
        // registered when we tried to cancel this timer the previous time) OR
        // a state change has already been signalled from this power mode 
        // context.
        // In all these cases the timeout is not relevant and we take no action
        OsTracePrint( KPwrStateTransition, (TUint8*)
            ("UMAC: WlanDynamicPowerModeMgmtCntx::OnLightPsToDeepPsTimerTimeout: not relevant timeout") );
        }
    
    return iStateChange;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDynamicPowerModeMgmtCntx::SetParameters(
    TUint32 aToLightPsTimeout,
    TUint16 aToLightPsFrameThreshold,
    TUint32 aToActiveTimeout,
    TUint16 aToActiveFrameThreshold,
    TUint32 aToDeepPsTimeout,
    TUint16 aToDeepPsFrameThreshold,
    TUint16 aUapsdRxFrameLengthThreshold )
    {
    OsTracePrint( KPwrStateTransition, (TUint8*)
        ("UMAC: WlanDynamicPowerModeMgmtCntx::SetParameters") );

    iToLightPsTimeout = aToLightPsTimeout;
    iActiveModeCntx.SetParameters( 
        aToLightPsFrameThreshold, 
        aUapsdRxFrameLengthThreshold );

    iToActiveTimeout = aToActiveTimeout;
    iToDeepPsTimeout = aToDeepPsTimeout;
    iLightPsModeCntx.SetParameters(
        aToActiveFrameThreshold,
        aToDeepPsFrameThreshold,
        aUapsdRxFrameLengthThreshold );
    
    iDeepPsModeCntx.SetParameters( aUapsdRxFrameLengthThreshold );
    
    // take also a backup of the Active mode parameters
    //
    iActiveParamsBackup.iToLightPsTimeout = aToLightPsTimeout;
    iActiveParamsBackup.iToLightPsFrameThreshold = aToLightPsFrameThreshold;
    iActiveParamsBackup.iUapsdRxFrameLengthThreshold = 
        aUapsdRxFrameLengthThreshold;    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDynamicPowerModeMgmtCntx::ConfigureTrafficOverride( 
    TBool aStayInPsDespiteUapsdVoiceTraffic,
    TBool aStayInPsDespiteUapsdVideoTraffic,
    TBool aStayInPsDespiteUapsdBestEffortTraffic, 
    TBool aStayInPsDespiteUapsdBackgroundTraffic,
    TBool aStayInPsDespiteLegacyVoiceTraffic,
    TBool aStayInPsDespiteLegacyVideoTraffic,
    TBool aStayInPsDespiteLegacyBestEffortTraffic,
    TBool aStayInPsDespiteLegacyBackgroundTraffic )
    {
    if ( aStayInPsDespiteUapsdVoiceTraffic )
        {
        iFlags |= KStayInPsDespiteUapsdVoiceTraffic;
        }
    else
        {
        iFlags &= ~KStayInPsDespiteUapsdVoiceTraffic;        
        }

    if ( aStayInPsDespiteUapsdVideoTraffic )
        {
        iFlags |= KStayInPsDespiteUapsdVideoTraffic;
        }
    else
        {
        iFlags &= ~KStayInPsDespiteUapsdVideoTraffic;        
        }

    if ( aStayInPsDespiteUapsdBestEffortTraffic )
        {
        iFlags |= KStayInPsDespiteUapsdBestEffortTraffic;
        }
    else
        {
        iFlags &= ~KStayInPsDespiteUapsdBestEffortTraffic;        
        }

    if ( aStayInPsDespiteUapsdBackgroundTraffic )
        {
        iFlags |= KStayInPsDespiteUapsdBackgroundTraffic;
        }
    else
        {
        iFlags &= ~KStayInPsDespiteUapsdBackgroundTraffic;        
        }

    if ( aStayInPsDespiteLegacyVoiceTraffic )
        {
        iFlags |= KStayInPsDespiteLegacyVoiceTraffic;
        }
    else
        {
        iFlags &= ~KStayInPsDespiteLegacyVoiceTraffic;        
        }

    if ( aStayInPsDespiteLegacyVideoTraffic )
        {
        iFlags |= KStayInPsDespiteLegacyVideoTraffic;
        }
    else
        {
        iFlags &= ~KStayInPsDespiteLegacyVideoTraffic;        
        }

    if ( aStayInPsDespiteLegacyBestEffortTraffic )
        {
        iFlags |= KStayInPsDespiteLegacyBestEffortTraffic;
        }
    else
        {
        iFlags &= ~KStayInPsDespiteLegacyBestEffortTraffic;        
        }

    if ( aStayInPsDespiteLegacyBackgroundTraffic )
        {
        iFlags |= KStayInPsDespiteLegacyBackgroundTraffic;
        }
    else
        {
        iFlags &= ~KStayInPsDespiteLegacyBackgroundTraffic;        
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDynamicPowerModeMgmtCntx::FreezeTrafficOverride()
    {
    if ( iWlanContextImpl.UapsdUsedForBestEffort() )
        {
        iIgnoreTraffic[WHA::ELegacy] = StayInPsDespiteUapsdBestEffortTraffic() ? 
            ETrue : EFalse;
        }
    else
        {
        iIgnoreTraffic[WHA::ELegacy] = StayInPsDespiteLegacyBestEffortTraffic() ? 
            ETrue : EFalse;
        }

    if ( iWlanContextImpl.UapsdUsedForBackground() )
        {
        iIgnoreTraffic[WHA::EBackGround] = StayInPsDespiteUapsdBackgroundTraffic() ? 
            ETrue : EFalse;
        }
    else
        {
        iIgnoreTraffic[WHA::EBackGround] = StayInPsDespiteLegacyBackgroundTraffic() ? 
            ETrue : EFalse;
        }

    if ( iWlanContextImpl.UapsdUsedForVideo() )
        {
        iIgnoreTraffic[WHA::EVideo] = StayInPsDespiteUapsdVideoTraffic() ? 
            ETrue : EFalse;
        }
    else
        {
        iIgnoreTraffic[WHA::EVideo] = StayInPsDespiteLegacyVideoTraffic() ? 
            ETrue : EFalse;
        }

    if ( iWlanContextImpl.UapsdUsedForVoice() )
        {
        iIgnoreTraffic[WHA::EVoice] = StayInPsDespiteUapsdVoiceTraffic() ? 
            ETrue : EFalse;
        }
    else
        {
        iIgnoreTraffic[WHA::EVoice] = StayInPsDespiteLegacyVoiceTraffic() ? 
            ETrue : EFalse;
        }

#ifndef NDEBUG
    OsTracePrint( KPwrStateTransition | KQos, (TUint8*)
        ("UMAC: WlanDynamicPowerModeMgmtCntx::FreezeTrafficOverride: traffic ignoration for BE, BG, VI, VO, HCCA frozen to:") );

    for ( TUint accessCategory = WHA::ELegacy; 
          accessCategory < WHA::EQueueIdMax;
          ++accessCategory )
        {
        OsTracePrint( KPwrStateTransition | KQos, (TUint8*)
            ("UMAC: WlanDynamicPowerModeMgmtCntx::FreezeTrafficOverride: ignore traffic: %d"), 
            iIgnoreTraffic[accessCategory] );        
        }
#endif // !NDEBUG
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDynamicPowerModeMgmtCntx::RegisterToLightPsTimeout()
    {
    if ( !( iFlags & KToLightPsTimerStarted ) )
        {
        iFlags |= KToLightPsTimerStarted;
        
        iWlanContextImpl.iUmac.RegisterTimeout( 
            iToLightPsTimeout, 
            EWlanActiveToLightPsTimer );        
        }
    else
        {
        // timer already running; no action needed
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDynamicPowerModeMgmtCntx::RegisterToActiveTimeout()
    {
    if ( !( iFlags & KToActiveTimerStarted ) )
        {
        iFlags |= KToActiveTimerStarted;
        
        iWlanContextImpl.iUmac.RegisterTimeout( 
            iToActiveTimeout, 
            EWlanLightPsToActiveTimer );        
        }
    else
        {
        // timer already running; no action needed
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDynamicPowerModeMgmtCntx::RegisterToDeepPsTimeout()
    {
    if ( !( iFlags & KToDeepPsTimerStarted ) )
        {
        iFlags |= KToDeepPsTimerStarted;
        
        iWlanContextImpl.iUmac.RegisterTimeout( 
            iToDeepPsTimeout, 
            EWlanLightPsToDeepPsTimer );        
        }
    else
        {
        // timer already running; no action needed
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDynamicPowerModeMgmtCntx::CancelToLightPsTimeout()
    {
    if ( iFlags & KToLightPsTimerStarted )
        {
        iFlags &= ~KToLightPsTimerStarted;
        iWlanContextImpl.iUmac.CancelTimeout( EWlanActiveToLightPsTimer );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDynamicPowerModeMgmtCntx::CancelToActiveTimeout()
    {
    if ( iFlags & KToActiveTimerStarted )
        {
        iFlags &= ~KToActiveTimerStarted;
        iWlanContextImpl.iUmac.CancelTimeout( EWlanLightPsToActiveTimer );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDynamicPowerModeMgmtCntx::CancelToDeepPsTimeout()
    {
    if ( iFlags & KToDeepPsTimerStarted )
        {
        iFlags &= ~KToDeepPsTimerStarted;
        iWlanContextImpl.iUmac.CancelTimeout( EWlanLightPsToDeepPsTimer );
        }
    }
