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
* Description:   Declaration of the WlanDot11PwrMgmtTransitionMode class
*
*/

/*
* %version: 12 %
*/

#ifndef C_WLANDOT11PWRMGMTTRANSITIONMODE_H
#define C_WLANDOT11PWRMGMTTRANSITIONMODE_H

#include "UmacDot11InfrastructureMode.h"

/**
 *  State for performing a change between CAM and PS power management states.
 *
 *  @lib wlanumac.lib
 *  @since S60 v3.1
 */
class WlanDot11PwrMgmtTransitionMode : public WlanDot11InfrastructureMode
    {

public:  

    /**  Types for the FSM */
    
    /**  events for the FSM */
    enum TEvent
        {
        ESTATEENTRY,        // state entry action to be executed                        
        ETXCOMPLETE,        // underlying sw layer tx delivery complete event   
        // dot11 power management transition complete event
        EPWRMGMTTRANSITCOMPLETE,
        EABORT,             // abort execution event
        EEVENTMAX           // defined as an upper bound
        };

    /**  states of the FSM */
    enum TState
        {
        EINIT,                  // start state of the state machine
        ESETAWAKEMODE,          // set dot11 power management awake mode
        ESETWAKEUPINTERVAL,     // set wlanwakeupinterval mib
        ESETPSMODE,             // set dot11 power management ps mode
        // wait state for dot11 power management transition complete event
        EWAIT4PWRMGMTTRANSITCOMPLETE,
        ECONTINUEDOT11TRAVERSE, // continue dot11 state traversal
        ESTATEMAX               // defined as an upper bound        
        }; 

    WlanDot11PwrMgmtTransitionMode() : iState( EINIT ), iFlags( 0 ) {};


    virtual ~WlanDot11PwrMgmtTransitionMode() {};




    private:
            
    // Prohibit copy constructor.
    WlanDot11PwrMgmtTransitionMode( const WlanDot11PwrMgmtTransitionMode& );
    // Prohibit assigment operator.
    WlanDot11PwrMgmtTransitionMode& operator= ( 
        const WlanDot11PwrMgmtTransitionMode& );

    /**
     * Internal state transition method
     *
     * @since S60 3.1
     * @param aCtxImpl global statemachine context
     * @param aNewState new internal state
     */
    void ChangeInternalState( WlanContextImpl& aCtxImpl, TState aNewState );

    /**
     * Feeds an event to internal fsm
     *
     * @since S60 3.1
     * @param aCtxImpl global statemachine context
     * @param aEvent fsm event
     */
    void Fsm( WlanContextImpl& aCtxImpl, TEvent aEvent ); 

    /**
     *
     * @since S60 3.1
     * @param aCtxImpl global statemachine context
     */
    void OnStateEntryEvent( WlanContextImpl& aCtxImpl );

    /**
     *
     * @since S60 3.1
     * @param aCtxImpl global statemachine context
     */
    void OnTxCompleteEvent( WlanContextImpl& aCtxImpl );

    /**
     *
     * @since S60 3.1
     * @param aCtxImpl global statemachine context
     */
    void OnPwrMgmtTransitCompleteEvent( WlanContextImpl& aCtxImpl );

    /**
     *
     * @since S60 3.1
     * @param aCtxImpl global statemachine context
     */
    void OnAbortEvent( WlanContextImpl& aCtxImpl );
    
    /**
     *
     * @since S60 3.1
     * @param aCtxImpl global statemachine context
     */
    void ActivateDot11AwakeMode( WlanContextImpl& aCtxImpl );
    
    /**
     *
     * @since S60 3.1
     * @param aCtxImpl global statemachine context
     */
    void SetWakeUpInterval( WlanContextImpl& aCtxImpl );

    /**
     *
     * @since S60 3.1
     * @param aCtxImpl global statemachine context
     */
    void ActivateDot11PsMode( WlanContextImpl& aCtxImpl );

    /**
     *
     * @since S60 3.1
     * @param aCtxImpl global statemachine context
     */
    void ContinueDot11StateTraversal( WlanContextImpl& aCtxImpl );

// from base class WlanMacState
        
    /**
     * From WlanMacState.
     * ?description
     *
     * @since S60 3.1
     * @param ?arg1 ?description
     */
    virtual void Entry( WlanContextImpl& aCtxImpl );

    /**
     * From WlanMacState.
     * ?description
     *
     * @since S60 3.1
     * @param ?arg1 ?description
     */
    virtual void Exit( WlanContextImpl& aCtxImpl);

    /**
     * From WlanMacState.
     * ?description
     *
     * @since S60 3.1
     * @param ?arg1 ?description
     */
#ifndef NDEBUG 
        virtual const TInt8* GetStateName( TUint8& aLength ) const;
#endif // !NDEBUG 
    
// from base class MWlanWsaEvent

    /**
     * From MWlanWsaEvent.
     * ?description
     *
     * @since S60 3.1
     * @param ?arg1 ?description
     */
    virtual TBool CommandComplete( 
        WlanContextImpl& aCtxImpl, 
        WHA::TCompleteCommandId aCompleteCommandId, 
        WHA::TStatus aStatus,
        const WHA::UCommandCompletionParams& 
        aCommandCompletionParams );

// from base class WlanMacState

    /**
    * From WlanMacState.
    * Set 802.11 power mgmt mode in infrastructure networks.
    * @param aCtxImpl statemachine context
    * @param aPowerMode desired power mode
    * @param aDisableDynamicPowerModeManagement If ETrue, disables the dynamic
    *        power mode management handling. Relevant only when aPowerMode
    *        is EPowerModePs
    * @param aWakeupModeInLightPs WLAN wake-up mode in Light PS mode
    * @param aListenIntervalInLightPs specifies the value of N for wake-up
    *        modes 2 and 3 in Light PS mode. 
    * @param aWakeupModeInDeepPs WLAN wake-up mode in Deep PS mode
    * @param aListenIntervalInDeepPs specifies the value of N for wake-up 
    *        modes 2 and 3 in Deep PS mode
    * @return ETrue if a state transition occurred
    *         EFalse otherwise
    */
    virtual TBool SetPowerMode(
        WlanContextImpl& aCtxImpl,
        TPowerMode aPowerMode,
        TBool aDisableDynamicPowerModeManagement,
        TWlanWakeUpInterval aWakeupModeInLightPs, 
        TUint8 aListenIntervalInLightPs,
        TWlanWakeUpInterval aWakeupModeInDeepPs,
        TUint8 aListenIntervalInDeepPs );

private: // data
    
#ifndef NDEBUG
    /**  max length of state name for tracing */
    enum { KMaxStateStringLength = 50 };

    /**  max length of event name for tracing */
    enum { KMaxEventStringLength = KMaxStateStringLength };

    /**
     * state names for tracing
     */
    static const TUint8 iStateName[ESTATEMAX][KMaxStateStringLength];

    /**
     * event names for tracing 
     */
    static const TUint8 iEventName[EEVENTMAX][KMaxEventStringLength];         
    /**
     * name of the state
     */
    static const TInt8  iName[];
#endif 

    /**
     * state of the fsm 
     */
    TState              iState;

    /**
     * internal flags
     */
    TUint32             iFlags;
    };

#endif      // C_WLANDOT11PWRMGMTTRANSITIONMODE_H

