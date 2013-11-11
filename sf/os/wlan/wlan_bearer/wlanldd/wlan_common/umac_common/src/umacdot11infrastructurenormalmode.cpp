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
* Description:   Implementation of the WlanDot11InfrastructureNormalMode 
*                class.
*
*/

/*
* %version: 15 %
*/

#include "config.h"
#include "umacdot11infrastructurenormalmode.h"
#include "UmacContextImpl.h"
#include "umacoidmsg.h"

#ifndef NDEBUG
const TInt8 WlanDot11InfrastructureNormalMode::iName[] 
    = "dot11infrastructure::dot11-normalmode";
#endif


// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanDot11InfrastructureNormalMode::ChangePowerMgmtMode( 
    WlanContextImpl& aCtxImpl )
    {
    // initialize the counter which we use to determine if we should 
    // indicate BSS lost if we continuously are unsuccessful in changing the
    // power mgmt mode
    aCtxImpl.InitializeSetPsModeCount();
           
    ChangeState( aCtxImpl, 
        *this,                                      // prev state
        aCtxImpl.iStates.iPwrMgmtTransitionMode     // next state
        );
    return ETrue; // signal caller that a state change occurred                          
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11InfrastructureNormalMode::DoPsModeErrorIndication( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( KWlmIndication, (TUint8*)
        ("UMAC: WlanDot11InfrastructureMode::DoPsModeErrorIndication") );
    
    // as the lower layers indicate that there is a problem with the PS
    // operation of the current AP, we want to be in CAM mode for as long
    // as there's data activity. So, check if we need a mode change to be
    // in CAM now
    const TPowerMgmtModeChange KPowerMgmtModeChange ( 
        aCtxImpl.OnPsModeErrorIndication() );
    
    // if any change is needed regarding our power mgmt mode,
    // proceed with it
    PowerMgmtModeChange( aCtxImpl, KPowerMgmtModeChange );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanDot11InfrastructureNormalMode::OnActiveToLightPsTimerTimeout( 
    WlanContextImpl& aCtxImpl )
    {
    TBool statechange( EFalse );

    if ( aCtxImpl.OnActiveToLightPsTimerTimeout() )
        {
        // change to Light PS mode desired
        
        aCtxImpl.DesiredDot11PwrMgmtMode( WHA::KPsEnable );
        aCtxImpl.SetDesiredPsModeConfig( 
            aCtxImpl.ClientLightPsModeConfig() );
        
        // proceed with the mode change        
        statechange = ChangePowerMgmtMode( aCtxImpl );
        }
    
    return statechange;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanDot11InfrastructureNormalMode::OnLightPsToActiveTimerTimeout( 
    WlanContextImpl& aCtxImpl )
    {
    TBool statechange( EFalse );

    if ( aCtxImpl.OnLightPsToActiveTimerTimeout() )
        {
        // change to Active mode desired
        
        aCtxImpl.DesiredDot11PwrMgmtMode( WHA::KPsDisable );
        
        // proceed with the mode change        
        statechange = ChangePowerMgmtMode( aCtxImpl );
        }
    
    return statechange;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanDot11InfrastructureNormalMode::OnLightPsToDeepPsTimerTimeout( 
    WlanContextImpl& aCtxImpl )
    {
    TBool statechange( EFalse );

    if ( aCtxImpl.OnLightPsToDeepPsTimerTimeout() )
        {
        // change to Deep PS mode desired
        
        aCtxImpl.DesiredDot11PwrMgmtMode( WHA::KPsEnable );
        aCtxImpl.SetDesiredPsModeConfig( 
            aCtxImpl.ClientDeepPsModeConfig() );
        
        // proceed with the mode change        
        statechange = ChangePowerMgmtMode( aCtxImpl );
        }
    
    return statechange;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11InfrastructureNormalMode::Entry( 
    WlanContextImpl& aCtxImpl )
    {
    if ( ( aCtxImpl.ClientDot11PwrMgmtMode() == WHA::KPsEnable ) &&
         ( !aCtxImpl.DisassociatedByAp() ) &&
         ( !aCtxImpl.DynamicPwrModeMgtDisabled() ) )
        {
        // activate dynamic power mode management        
        aCtxImpl.StartPowerModeManagement();        
        }

    // dispatch an event that might be pending as we 
    // can come here from dot11-pwrmgmttransitionmode state
    // which has enabled oid deferring to eventdispatcher
    aCtxImpl.DispatchEvent();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDot11InfrastructureNormalMode::Exit( 
    WlanContextImpl& aCtxImpl)
    {
    // deactivate dynamic power mode management        
    aCtxImpl.StopPowerModeManagement();    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
#ifndef NDEBUG 
const TInt8* WlanDot11InfrastructureNormalMode::GetStateName( 
    TUint8& aLength ) const
    {
    aLength = sizeof( iName );
    return iName;
    }
#endif

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanDot11InfrastructureNormalMode::OnDot11PwrMgmtTransitRequired( 
    WlanContextImpl& aCtxImpl )
    {
    // initialize the counter which we use to determine if we should 
    // indicate BSS lost if we continuously are unsuccessful in changing the
    // power mgmt mode
    aCtxImpl.InitializeSetPsModeCount(); 
    
    ChangeState( aCtxImpl, 
        *this,                                      // prev state
        aCtxImpl.iStates.iPwrMgmtTransitionMode     // next state
        );                      

    OnOidComplete( aCtxImpl );
    // global statemachine transition will occur 
    return ETrue; 
    }
