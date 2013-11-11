/*
* Copyright (c) 2005-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Declaration of the WlanDot11PrepareForIbssMode class.
*
*/

/*
* %version: 12 %
*/

#ifndef WLANDOT11PREPAREFORIBSSMODE_H
#define WLANDOT11PREPAREFORIBSSMODE_H

#include "UmacDot11State.h"


/**
*  State to join to an IBSS network
*
*  @lib wlanumac.lib
*  @since S60 v3.1
*/
class WlanDot11PrepareForIbssMode : public WlanDot11State
    {

public:

    /**
    * C++ default constructor.
    */
    WlanDot11PrepareForIbssMode() : 
         iState( EINIT ), iStartIbss( EFalse ), iMemory( NULL ) {};

    /**
    * Destructor.
    */
    virtual ~WlanDot11PrepareForIbssMode();

    /**
    * Sets input parameters for this state
    *
    * @since S60 3.2
    * @param aStartIbss are we starting a new IBSS
    */
    void Set( TBool aStartIbss );

private:  // definitions

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
        EINIT,                  // start state of the state machine
        ESETSLEEPMODE,          // configure sleep mode mib
        ESETDOT11SLOTTIME,      // configure slottime mib if 
                                // supported by the WLAN vendor solution
        ESETCTSTOSELF,          // configure ctstoself mib 
        ESETBEACON,             // configure beacon template
        ESETPROBERESP,          // configure probe response template
        ESETTXRATEPOLICY,       // set Tx rate policy
        ESETBEACONLOSTCOUNT,    // set beacon lost count mib
        EJOIN,                  // issue join
        ECONTINUEDOT11TRAVERSE, // continue dot11 state traversal
        ESTATEMAX               // defined as an upper bound        
        }; 

private:

    virtual void Entry( WlanContextImpl& aCtxImpl );
    virtual void Exit( WlanContextImpl& aCtxImpl);
#ifndef NDEBUG 
    virtual const TInt8* GetStateName( TUint8& aLength ) const;
#endif // !NDEBUG 

    /**
    * ?member_description.
    * @since S60 3.1
    * @param aCtxImpl global statemachine context
    */
    void ChangeInternalState( 
        WlanContextImpl& aCtxImpl, 
        TState aNewState );

    /**
    * ?member_description.
    * @since S60 3.1
    * @param aCtxImpl global statemachine context
    */
    void Fsm( 
        WlanContextImpl& aCtxImpl, 
        TEvent aEvent ); 

    /**
    * ?member_description.
    * @since S60 3.1
    * @param aCtxImpl global statemachine context
    */
    void OnStateEntryEvent( WlanContextImpl& aCtxImpl );

    /**
    * ?member_description.
    * @since S60 3.1
    * @param aCtxImpl global statemachine context
    */
    void OnTxCompleteEvent( WlanContextImpl& aCtxImpl );

    /**
    * ?member_description.
    * @since S60 3.1
    * @param aCtxImpl global statemachine context
    */
    void OnAbortEvent( WlanContextImpl& aCtxImpl );

    /**
    * ?member_description.
    * @since Series 60 3.1
    * @param aCtxImpl global statemachine context
    */
    void SetSleepMode( WlanContextImpl& aCtxImpl );

    /**
    * ?member_description.
    * @since Series 60 3.1
    * @param aCtxImpl global statemachine context
    */
    void SetDot11SlotTime( WlanContextImpl& aCtxImpl );

    /**
    * ?member_description.
    * @since Series 60 3.1
    * @param aCtxImpl global statemachine context
    */
    void SetCtsToSelf( WlanContextImpl& aCtxImpl );

    /**
    * ?member_description.
    * @since S60 3.1
    * @param aCtxImpl global statemachine context
    */
    void ConfigureBeaconTemplate( WlanContextImpl& aCtxImpl );

    /**
    * ?member_description.
    * @since S60 3.1
    * @param aCtxImpl global statemachine context
    */
    void ConfigureProbeResponseTemplate( WlanContextImpl& aCtxImpl );

    /**
    * ?member_description.
    * @since Series 60 3.1
    * @param aCtxImpl global statemachine context
    */
    void SetTxRatePolicy( WlanContextImpl& aCtxImpl );

    /**
    * ?member_description.
    * @since Series 60 3.1
    * @param aCtxImpl global statemachine context
    */
    void SetBeaconLostCount( WlanContextImpl& aCtxImpl );

    /**
    * ?member_description.
    * @since S60 3.1
    * @param aCtxImpl global statemachine context
    */
    void Join( WlanContextImpl& aCtxImpl );

    /**
    * ?member_description.
    * @since Series 60 3.1
    * @param aCtxImpl global statemachine context
    */
    void ContinueDot11StateTraversal( WlanContextImpl& aCtxImpl );

    /**
    * ?member_description.
    * @since S60 3.1
    * @param aCtxImpl global statemachine context
    */
    void ConfigureTemplate( WlanContextImpl& aCtxImpl, TUint32& aLength );

    /**
    * ?member_description.
    * @since S60 3.1
    * @param aCtxImpl global statemachine context
    */
    WHA::TRate BeaconTxRate( WlanContextImpl& aCtxImpl ) const;

    /**
    * ?member_description.
    * @since S60 3.1
    * @param aCtxImpl global statemachine context
    */
    WHA::TRate ProbeResponseTxRate( WlanContextImpl& aCtxImpl ) const;

    // Prohibit copy constructor.
    WlanDot11PrepareForIbssMode( const WlanDot11PrepareForIbssMode& );
    // Prohibit assigment operator.
    WlanDot11PrepareForIbssMode& operator= 
        ( const WlanDot11PrepareForIbssMode& );

private:    // Data
    
    TState              iState;
    
    /** ETrue, if we are starting a new IBSS */
    TBool               iStartIbss;

#ifndef NDEBUG
    /** max length of state name for tracing */
    enum { KMaxStateStringLength = 50 };
    /** max length of event name for tracing */
    enum { KMaxEventStringLength = KMaxStateStringLength };

    /** state names for tracing */
    static const TUint8 iStateName[ESTATEMAX][KMaxStateStringLength];
    /** event names for tracing */
    static const TUint8 iEventName[EEVENTMAX][KMaxEventStringLength];
    /** state name */
        static const TInt8  iName[];
#endif 
        TUint8*             iMemory;
    };

#endif      // WLANDOT11PREPAREFORIBSSMODE_H
