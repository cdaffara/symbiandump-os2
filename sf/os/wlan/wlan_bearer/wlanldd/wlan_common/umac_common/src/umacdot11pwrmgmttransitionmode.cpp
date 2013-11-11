/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of the WlanDot11PwrMgmtTransitionMode class.
*
*/

/*
* %version: 22 %
*/

#include "config.h"
#include "umacdot11pwrmgmttransitionmode.h"
#include "UmacContextImpl.h"
#include "UmacWsaSetPsMode.h"
#include "UmacWsaWriteMib.h"

#ifndef NDEBUG
const TInt8 WlanDot11PwrMgmtTransitionMode::iName[] 
    = "dot11-pwrmgmttransitionmode";

const TUint8 WlanDot11PwrMgmtTransitionMode::iStateName
    [ESTATEMAX][KMaxStateStringLength] = 
    {
        {"EINIT"},
        {"ESETAWAKEMODE"},
        {"ESETWAKEUPINTERVAL"},
        {"ESETPSMODE"},
        {"EWAIT4PWRMGMTTRANSITCOMPLETE"},
        {"ECONTINUEDOT11TRAVERSE"}
    };

const TUint8 WlanDot11PwrMgmtTransitionMode::iEventName
    [EEVENTMAX][KMaxEventStringLength] = 
    {
        {"ESTATEENTRY"}, 
        {"ETXCOMPLETE"},
        {"EPWRMGMTTRANSITCOMPLETE"},
        {"EABORT"}
    };
#endif

// this flag is set if we only need to set the wakeup interval.
const TUint32 KOnlySetWakeupInterval        = ( 1 << 0 );
// this flag is set if the status of the latest Set Ps Mode cmd was Success
const TUint32 KSetPsModeSuccess             = ( 1 << 1 );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// The internal state transition method.
// It is guaranteed by the FSM framework that no WHA command is pending
// when this method is entered
// -----------------------------------------------------------------------------
//
void WlanDot11PwrMgmtTransitionMode::ChangeInternalState( 
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
void WlanDot11PwrMgmtTransitionMode::Fsm( 
    WlanContextImpl& aCtxImpl, 
    TEvent aEvent )
    {
    OsTracePrint( KPwrStateTransition, 
        (TUint8*)("UMAC * dot11-pwrmgmttransitionmode * FSM EVENT") );
#ifndef NDEBUG
    OsTracePrint( KPwrStateTransition, (TUint8*)("event:"));
    OsTracePrint( KPwrStateTransition, iEventName[aEvent] );
    OsTracePrint( KPwrStateTransition, (TUint8*)("state:"));
    OsTracePrint( KPwrStateTransition, iStateName[iState] );
#endif

    switch ( aEvent )
        {
        case ESTATEENTRY:
            OnStateEntryEvent( aCtxImpl );
            break;
        case ETXCOMPLETE:
            OnTxCompleteEvent( aCtxImpl );
            break;
        case EPWRMGMTTRANSITCOMPLETE:
            OnPwrMgmtTransitCompleteEvent( aCtxImpl );
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
void WlanDot11PwrMgmtTransitionMode::OnStateEntryEvent( 
    WlanContextImpl& aCtxImpl )
    {
    TState state( ESTATEMAX );

    switch ( iState )
        {
        case EINIT:
            iFlags = 0;
            // start of fsm traversal
            // disable oid request so we run this fsm in non pre-emptive mode
            // regarding oid commands
            OsTracePrint( KPwrStateTransition, (TUint8*)
                ("UMAC * dot11-pwrmgmttransitionmode * disable oid events") );

            aCtxImpl.Disable( WlanEventDispatcher::KOidChannel );

            if (// PS mode desired
                aCtxImpl.DesiredDot11PwrMgmtMode() == WHA::KPsEnable
                && // AND
                // current mode is CAM
                aCtxImpl.CurrentDot11PwrMgmtMode() == WHA::KPsDisable )
                {
                // change to dot11 PS mode 
                // start by setting the  wakeup interval
                state = ESETWAKEUPINTERVAL;
                }
            else if (// CAM mode desired
                aCtxImpl.DesiredDot11PwrMgmtMode() == WHA::KPsDisable
                && // AND
                // current mode is PS mode
                aCtxImpl.CurrentDot11PwrMgmtMode() == WHA::KPsEnable )
                {
                // change to dot11 CAM mode
                state = ESETAWAKEMODE;
                }
            else
                {
                // we have a dot11 power management mode match.
                // We can land here e.g. if user changes desired 
                // dot11 power management mode when we are excuting this fsm
                // as we dispacth an oid as the fsm entry action.
                // We can come here also when only the wakeup interval needs
                // to be changed

                // if the current mode is PS mode we have to make sure 
                // that we have a wakeup interval match
                if ( aCtxImpl.CurrentDot11PwrMgmtMode() == WHA::KPsEnable )
                    {
                    // the way to do that is by re-setting (only) the wakeup
                    // interval - if necessary
                    iFlags |= KOnlySetWakeupInterval;
                    state = ESETWAKEUPINTERVAL;
                    }
                else
                    {                    
                    // the current mode is CAM, so the wakeup interval is
                    // not relevant; and we are done
                    state = ECONTINUEDOT11TRAVERSE;
                    }                
                }

            ChangeInternalState( aCtxImpl, state );
            break;
        case ESETAWAKEMODE:
            ActivateDot11AwakeMode( aCtxImpl );
            break;
        case ESETWAKEUPINTERVAL:
            {
            // first check if the wake-up setting already is what it should be.
            // if that's the case, we won't set it again
            
            if ( DifferenceInPsModeWakeupSettings( aCtxImpl ) )
                {
                // it's not what it needs to be, so set it
                SetWakeUpInterval( aCtxImpl );                
                }
            else
                {
                // the wake-up setting is already correct, skip setting it again 
                // and simulate ETXCOMPLETE event instead
                Fsm( aCtxImpl, ETXCOMPLETE );
                }            
            break;
            }
        case ESETPSMODE:
            ActivateDot11PsMode( aCtxImpl );
            break;
        case EWAIT4PWRMGMTTRANSITCOMPLETE:
            // nothing to do here than just wait for completion
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
void WlanDot11PwrMgmtTransitionMode::OnTxCompleteEvent( 
    WlanContextImpl& aCtxImpl )
    {
    switch ( iState )
        {
        case ESETAWAKEMODE:
        case ESETPSMODE:
            // as transition has sterted now we just wait for it to complete
            ChangeInternalState( aCtxImpl, EWAIT4PWRMGMTTRANSITCOMPLETE );
            break;
        case ESETWAKEUPINTERVAL:
            if ( iFlags & KOnlySetWakeupInterval )
                {
                // we only needed to set the wakeup interval. So we are
                // done now
                ChangeInternalState( aCtxImpl, ECONTINUEDOT11TRAVERSE );
                }
            else                
                {
                // we also need to set the PS mode. This is the most common case
                ChangeInternalState( aCtxImpl, ESETPSMODE );                
                }
            break;
        case EWAIT4PWRMGMTTRANSITCOMPLETE:
            // a re-entry to this state has occurred while waiting for the 
            // CommandComplete event to the SetPsMode cmd.
            // This will happen when we autonomously issue a (new) WHA cmd -
            // during the above mentioned wait period - to react to a change 
            // which has occurred in the network (e.g. in the Use Protection 
            // setting or in the QoS parameters).
            // Anyhow, no action is required here (except to allow for this
            // re-entry to happen)
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
void WlanDot11PwrMgmtTransitionMode::OnPwrMgmtTransitCompleteEvent( 
    WlanContextImpl& aCtxImpl )
    {
    // we can continue dot11 state traversal
    ChangeInternalState( aCtxImpl, ECONTINUEDOT11TRAVERSE );
    }

// ---------------------------------------------------------
// simulate macnotresponding error
// ---------------------------------------------------------
//
void WlanDot11PwrMgmtTransitionMode::OnAbortEvent( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( KWarningLevel, 
        (TUint8*)("UMAC * dot11-pwrmgmttransitionmode * abort") );

    DoErrorIndication( aCtxImpl, WHA::KErrorMacNotResponding );
    }

// -----------------------------------------------------------------------------
// Handler for the event.
// It is guaranteed by the FSM framework that no WHA command is pending
// when this method is entered
// -----------------------------------------------------------------------------
//
void WlanDot11PwrMgmtTransitionMode::ActivateDot11AwakeMode( 
    WlanContextImpl& aCtxImpl )
    {
    WlanWsaSetPsMode& wha_cmd( aCtxImpl.WsaSetPsMode() );
    wha_cmd.Set( aCtxImpl, WHA::KPsDisable );

    // change global state: entry procedure triggers action
    ChangeState( aCtxImpl, 
        *this,      // prev state
        wha_cmd     // next state
        );
    }
  
// -----------------------------------------------------------------------------
// Handler for the event.
// It is guaranteed by the FSM framework that no WHA command is pending
// when this method is entered
// -----------------------------------------------------------------------------
//  
void WlanDot11PwrMgmtTransitionMode::SetWakeUpInterval( 
    WlanContextImpl& aCtxImpl )
    {
    WHA::SwlanWakeUpInterval* mib 
        = static_cast<WHA::SwlanWakeUpInterval*>
        (os_alloc( sizeof( WHA::SwlanWakeUpInterval ) )); 

    if ( !mib )
        {
        // allocation failed
        // simulate macnotresponding error
        OsTracePrint( KWarningLevel, (TUint8*)
            ("UMAC: WlanDot11PwrMgmtTransitionMode::SetWakeUpInterval: alloc failed, abort") );
        Fsm( aCtxImpl, EABORT );
        return;
        }

    // determine the desired new wake-up setting
    const TDot11PsModeWakeupSetting KDesiredPsModeConfig (
        aCtxImpl.DesiredPsModeConfig() );    

    // take it into use

    mib->iMode = KDesiredPsModeConfig.iWakeupMode;
    mib->iListenInterval = KDesiredPsModeConfig.iListenInterval;
    
    WlanWsaWriteMib& wha_cmd = aCtxImpl.WsaWriteMib();

    wha_cmd.Set( 
        aCtxImpl, WHA::KMibWlanWakeUpInterval, sizeof(*mib), mib );

    OsTracePrint( KPwrStateTransition, (TUint8*)
        ("UMAC: UMAC: WlanDot11PwrMgmtTransitionMode::SetWakeUpInterval: desired mode: %d"), 
        mib->iMode );
    OsTracePrint( KPwrStateTransition, 
        (TUint8*)("UMAC: desired listeninterval: %d"), mib->iListenInterval );

    // store the new setting also locally
    aCtxImpl.iWlanMib.iWlanWakeupInterval = mib->iMode;
    aCtxImpl.iWlanMib.iWlanListenInterval = mib->iListenInterval;

    // change global state: entry procedure triggers action
    ChangeState( aCtxImpl, 
        *this,              // prev state
        wha_cmd             // next state
        );           

    // as the parameters have been supplied we can now deallocate
    os_free( mib );        
    }

// -----------------------------------------------------------------------------
// Handler for the event.
// It is guaranteed by the FSM framework that no WHA command is pending
// when this method is entered
// -----------------------------------------------------------------------------
//
void WlanDot11PwrMgmtTransitionMode::ActivateDot11PsMode( 
    WlanContextImpl& aCtxImpl )
    {
    WlanWsaSetPsMode& wha_cmd( aCtxImpl.WsaSetPsMode() );
    wha_cmd.Set( aCtxImpl, WHA::KPsEnable );

    // change global state: entry procedure triggers action
    ChangeState( aCtxImpl, 
        *this,      // prev state
        wha_cmd     // next state
        );
    }

// -----------------------------------------------------------------------------
// Handler for the event.
// It is guaranteed by the FSM framework that no WHA command is pending
// when this method is entered
// -----------------------------------------------------------------------------
//
void WlanDot11PwrMgmtTransitionMode::ContinueDot11StateTraversal( 
    WlanContextImpl& aCtxImpl )
    {
    if ( !( iFlags & KOnlySetWakeupInterval ) )
        {
        // we have tried to change the power mgmt mode with SetPsMode
        // WHA command
        
        if ( iFlags & KSetPsModeSuccess )
            {
            // the Set PS Mode WHA cmd was successfully executed, which means 
            // that we were able to communicate with the AP.
            // So we can indicate BSS Regained; if necessary
            DoRegainedBSSIndication( aCtxImpl );
            }
        else
            {
            if ( !aCtxImpl.SetPsModeCount() )
                {
                // the set power mgmt mode counter has counted down to zero, so
                // we will indicate Consecutive Power Mode Set Failures (unless
                // already indicated) to WLAN Mgmt Client
                DoConsecutivePwrModeSetFailuresIndication( aCtxImpl );
                // reset the counter
                aCtxImpl.SetPsModeCount();
                // it's possible that we come to this branch again shortly,
                // but it really doesn't matter. In any case we will indicate 
                // Consecutive Power Mode Set Failures only once at maximum
                }
            else
                {
                // we have made one (more) unsuccessful attempt to set the power
                // mgmt mode, so decrement the counter by one
                aCtxImpl.DecrementSetPsModeCount();
                }        
            }
        }

    TBool dot11PwrMgmtTransitComplete ( EFalse );
    const WHA::TPsMode KCurrentPwrMgmtMode( 
        aCtxImpl.CurrentDot11PwrMgmtMode() );
            
    if ( KCurrentPwrMgmtMode == aCtxImpl.DesiredDot11PwrMgmtMode() )
        {
        // current dot11 power management mode equals to desired mode
        
        if ( KCurrentPwrMgmtMode == WHA::KPsEnable )
            {
            // as the current and desired mode is PS, we are only done
            // if also the wakeup settings are as desired
            if ( !DifferenceInPsModeWakeupSettings( aCtxImpl ) )
                {
                dot11PwrMgmtTransitComplete = ETrue;                
                }
            else 
                {
                OsTracePrint( KPwrStateTransition, 
                    (TUint8*)("UMAC: wakeup settings not as desired") );                
                }
            }
        else
            {
            // as the current and desired mode is CAM, we are done
            dot11PwrMgmtTransitComplete = ETrue;
            }
        }
    else
        {
        // current dot11 power management mode does not equal to desired mode.
        // Se we are not done and will need to try to do the mode change again.
        // No action here. 
        }

    if ( dot11PwrMgmtTransitComplete )
        {
        // desired dot11 power management mode - and settings if the desired
        // mode is PS - have been acquired
        
        ChangeState( aCtxImpl, 
            *this,                                          // prev state
            aCtxImpl.iStates.iInfrastructureNormalMode );   // next state

        if ( aCtxImpl.CurrentDot11PwrMgmtMode() == WHA::KPsEnable )
            {
            // as we have entered PS mode, it is time to resume QoS null
            // data frame sending, if applicable
            aCtxImpl.ResumeQosNullSending();
            }
        }
    else
        {
        // dot11 power management mode/settings are not as desired, yet.
        // We shall repeat this process
        
        ChangeState( aCtxImpl, 
            *this,  // prev state
            *this   // next state
            );        
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11PwrMgmtTransitionMode::Entry( 
    WlanContextImpl& aCtxImpl )
    {
    if ( aCtxImpl.WsaCmdActive() )
        {
        // sanity checking code
        OsAssert( 
            (TUint8*)("UMAC * panic"), (TUint8*)(WLAN_FILE), __LINE__ );
        }

    if ( !(aCtxImpl.DispatchEvent()) )
        {
        // no state transition occurred
        // dispatcher might or might not have had an event to be dispatched
        // in any case we are still in the current state and can continue...
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
    else    // --- !(aCtxImpl.DispatchEvent())
        {
        // state transition occurred
        // we are no longer in the current state,
        // so we won't do anything as we might mess things up
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11PwrMgmtTransitionMode::Exit( 
    WlanContextImpl& aCtxImpl)
    {
    // reset fsm
    iState = EINIT;

    // enable oid requests as we are exiting this dot11 state
    OsTracePrint( KPwrStateTransition, (TUint8*)
        ("UMAC * dot11-pwrmgmttransitionmode * enable oid events") );

    aCtxImpl.Enable( WlanEventDispatcher::KOidChannel );
    }

#ifndef NDEBUG 
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
const TInt8* WlanDot11PwrMgmtTransitionMode::GetStateName( 
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
TBool WlanDot11PwrMgmtTransitionMode::CommandComplete( 
    WlanContextImpl& aCtxImpl, 
    WHA::TCompleteCommandId aCompleteCommandId, 
    WHA::TStatus aStatus,
    const WHA::UCommandCompletionParams& 
    aCommandCompletionParams )
    {
    if ( aCompleteCommandId != WHA::ESetPsModeComplete )
        {
        // implementation error
        OsTracePrint( KErrorLevel, (TUint8*)("UMAC: command id: %d"), aCompleteCommandId);
        OsAssert( (TUint8*)("UMAC panic"), (TUint8*)(WLAN_FILE), __LINE__ );
        }

    OsTracePrint( KPwrStateTransition, 
        (TUint8*)("UMAC: WlanDot11PwrMgmtTransitionMode::CommandComplete"));

    const WHA::TPsMode KCurrentPwrMgmtMode( 
        aCommandCompletionParams.iSetPsModeComplete.iDot11PowerManagementMode );
    aCtxImpl.CurrentDot11PwrMgmtMode( KCurrentPwrMgmtMode );

    // make a note whether the SetPsMode WHA command was executed successfully 
    // or not
    if ( aStatus == WHA::KSuccess )
        {
        iFlags |= KSetPsModeSuccess;

        OsTracePrint( KPwrStateTransition, 
            (TUint8*)("UMAC: transition success: current mode: %d"), 
            KCurrentPwrMgmtMode);
        }
    else
        {
        iFlags &= ~KSetPsModeSuccess;
        
        OsTracePrint( KWarningLevel | KPwrStateTransition, 
            (TUint8*)("UMAC: transition failure: current mode: %d"), 
            KCurrentPwrMgmtMode);
        }

    // continue fsm
    Fsm( aCtxImpl, EPWRMGMTTRANSITCOMPLETE );

    // signal global state transition
    return ETrue;
    }

// -----------------------------------------------------------------------------
// as oid reception is disbaled in this dot11 state, reception of this oid
// is a result of a manual oid dispatching
// -----------------------------------------------------------------------------
//
TBool WlanDot11PwrMgmtTransitionMode::SetPowerMode(
    WlanContextImpl& aCtxImpl,
    TPowerMode aPowerMode,
    TBool aDisableDynamicPowerModeManagement,
    TWlanWakeUpInterval aWakeupModeInLightPs, 
    TUint8 aListenIntervalInLightPs,
    TWlanWakeUpInterval aWakeupModeInDeepPs,
    TUint8 aListenIntervalInDeepPs )
    {
    OsTracePrint( KPwrStateTransition, 
        (TUint8*)("UMAC: WlanDot11PwrMgmtTransitionMode::SetPowerMode: desired power mode: %d"),
        aPowerMode );

    // store desired new dot11 power management mode by WLAN Mgmt Client
    aCtxImpl.ClientDot11PwrMgmtMode( aPowerMode );
    
    aCtxImpl.DynamicPwrModeMgtDisabled( aDisableDynamicPowerModeManagement );
    
    // it is now also our desired dot11 power management mode
    aCtxImpl.DesiredDot11PwrMgmtMode( aCtxImpl.ClientDot11PwrMgmtMode() );

    aCtxImpl.SetClientLightPsModeConfig( 
        aWakeupModeInLightPs, 
        aListenIntervalInLightPs );

    aCtxImpl.SetClientDeepPsModeConfig( 
            aWakeupModeInDeepPs, 
            aListenIntervalInDeepPs );
    
    // in case WLAN Mgmt Client wishes to use PS mode, Light PS is the initial
    // desired PS mode configuration
    aCtxImpl.SetDesiredPsModeConfig( 
                aCtxImpl.ClientLightPsModeConfig() );
    
    // complete the mgmt command
    OnOidComplete( aCtxImpl );

    // signal that no state transition occurred
    return EFalse;
    }
