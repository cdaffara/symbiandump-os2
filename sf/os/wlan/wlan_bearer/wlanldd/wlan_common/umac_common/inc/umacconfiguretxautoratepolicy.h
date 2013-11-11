/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Declaration of the WlanConfigureTxAutoRatePolicy class.
*
*/

/*
* %version: 6 %
*/

#ifndef WLAN_CONFIGURE_TX_AUTO_RATE_POLICY_H
#define WLAN_CONFIGURE_TX_AUTO_RATE_POLICY_H

#include "UmacWsaComplexCommand.h"
#include "umacinternaldefinitions.h"


/**
 *  Configures the relevant txAutoRatePolicy MIB entries to enable the lower
 *  layers to perform autonomous rate adaptation
 *
 *  @lib wlanumac.lib
 *  @since S60 v5.0
 */
class WlanConfigureTxAutoRatePolicy : public WlanWsaComplexCommand
    {

public: 

    /**
    * C++ default constructor.
    */
    WlanConfigureTxAutoRatePolicy();

    /**
    * Destructor.
    */
    virtual ~WlanConfigureTxAutoRatePolicy();

    /**
    * Provides the input parameters for this state
    * 
    * @since S60 5.0
    * @param aCompleteMgmtRequest ETrue if WLAN Mgmt Client request needs to be
    *        completed when we are done; EFalse otherwise
    */
    void Set( TBool aCompleteMgmtRequest );

private:

    // Types for the FSM

    // events for the FSM
    enum TEvent
        {
        ESTATEENTRY,    // state entry action to be executed                        
        ETXCOMPLETE,    // underlying sw layer tx delivery complete event
        EABORT,         // abort execution event
        EEVENTMAX       // defined as an upper bound
        };

    // states of the FSM
    enum TState
        {
        EINIT,          // start state of the state machine
        ESETAUTORATEPOLICY,
        EFINIT,
        ESTATEMAX       // defined as an upper bound        
        }; 

    /**
    * Dispatches the internal state machine events
    *
    * @since S60 5.0
    * @param aCtxImpl Global statemachine context
    * @param aEvent The event to dispatch
    */
    void Fsm( 
        WlanContextImpl& aCtxImpl, 
        TEvent aEvent ); 

    /**
    * Handles the Internal State Entry event
    *
    * @since S60 5.0
    * @param aCtxImpl Global statemachine context
    */
    void OnStateEntryEvent( WlanContextImpl& aCtxImpl );

    /**
    * Handles the Tx Complete event.
    *
    * @since S60 5.0
    * @param aCtxImpl Global statemachine context
    */
    void OnTxCompleteEvent( WlanContextImpl& aCtxImpl );

    /**
    * Handles the Abort event.
    *
    * @since S60 5.0
    * @param aCtxImpl Global statemachine context
    */
    void OnAbortEvent( WlanContextImpl& aCtxImpl );

    /**
    * Handles internal state machine state changes.
    *
    * @since S60 5.0
    * @param aCtxImpl Global statemachine context
    * @param aNewState The state to change to
    */
    void ChangeInternalState( 
        WlanContextImpl& aCtxImpl, 
        TState aNewState );

    /**
    * ?member_description.
    *
    * @since S60 5.0
    * @param aCtxImpl Global statemachine context
    */
    void SetAutoRatePolicy( 
        WlanContextImpl& aCtxImpl );

    /**
     * From WlanMacState.
     * Performs state entry actions
     *
     * @since S60 5.0
     * @param aCtxImpl Global statemachine context
     */
    virtual void Entry( WlanContextImpl& aCtxImpl );

    /**
     * From WlanMacState.
     * Performs state exit actions
     *
     * @since S60 5.0
     * @param aCtxImpl Global statemachine context
     */
    virtual void Exit( WlanContextImpl& aCtxImpl);

    /**
     * From WlanMacState.
     * Returns the name of this state.
     *
     * @since S60 5.0
     * @param aLength Length of the returned name buffer
     */
#ifndef NDEBUG 
        virtual const TInt8* GetStateName( TUint8& aLength ) const;
#endif // !NDEBUG 

    // Prohibit copy constructor.
    WlanConfigureTxAutoRatePolicy( const WlanConfigureTxAutoRatePolicy& );
    // Prohibit assignment operator.
    WlanConfigureTxAutoRatePolicy& operator= 
        ( const WlanConfigureTxAutoRatePolicy& );

private:    // Data

    /** state of the internal state machine */
    TState iState;
    /** 
     * ETrue if WLAN Mgmt Client request needs to be completed when we are 
     * done, EFalse otherwise
     */
    TBool iCompleteMgmtRequest;
    // memory handle for TxAutoRatePolicyMib objects. Own
    WHA::StxAutoRatePolicy* iTxAutoRatePolicyMib;
    /** number of rate policy classes to configure */
    TUint iNumberOfPolicyClassesToConfigure;
    /** ID of the rate policy class being configured */
    TUint iRatePolicyClass;

#ifndef NDEBUG
    /** max length of state name for tracing */
    enum { KMaxStateStringLength = 50 };
    /** max length of event name for tracing */
    enum { KMaxEventStringLength = KMaxStateStringLength };

    /** state names for tracing */
    static const TUint8 iStateName[ESTATEMAX][KMaxStateStringLength];
    /** event names for tracing */
    static const TUint8 iEventName[EEVENTMAX][KMaxEventStringLength];
    static const TInt8  iName[];
#endif // !NDEBUG

    };

#endif      // WLAN_CONFIGURE_TX_AUTO_RATE_POLICY_H
