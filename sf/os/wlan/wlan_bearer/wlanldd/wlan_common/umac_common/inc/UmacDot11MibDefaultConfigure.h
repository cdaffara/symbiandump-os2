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
* Description:   Declaration of the WlanDot11MibDefaultConfigure class.
*
*/

/*
* %version: 13 %
*/

#ifndef WLANDOT11MIBDEFAULTCONFIGURE_H
#define WLANDOT11MIBDEFAULTCONFIGURE_H

#include "UmacDot11State.h"

/**
*  State for setting the MIB elements to their default values
*
*  @lib wlanumac.lib
*  @since S60 v3.1
*/
class WlanDot11MibDefaultConfigure : public WlanDot11State
    {
    // events for the FSM
    enum TEvent
        {
        ESTATEENTRY,        // state entry action to be executed
        // underlying sw layer tx delivery complete event 
        ETXCOMPLETE,  
        // abort FSM execution 
        EABORT,
        EEVENTMAX           // defined as an upper bound
        };

    // states of the FSM
    enum TState
        {
        EINIT,                          // start state of the state machine
        ESETDOT11MAXRECEIVELIFETIME,
        ESETDOT11SLOTTIME,
        ESETDOT11GROUPADDRSTABLE,
        ESETDOT11WEPDEFAULTKEY,
        ESETDOT11CURRENTTXPOWERLEVEL,
        ESETDOT11RTSTHRESHOLD,
        ESETCTSTOSELF,
        ESETARPIPADDRSTABLE,
        ESETPROBEREQUESTTEMPLATE,
        ESETRXFILTER,
        ESETBEACONFILTERIETABLE,
        ESETBEACONFILTERENABLE,

        // omit setting sleepmode MIB as it is 
        // set allways in dot11idle state entry
        // after we traverse through this state

        ESETWLANWAKEUPINTERVAL,
        ESETBEACONLOSTCOUNT,
        ESETRCPITHRESHOLD,
        ESETTXRATEPOLICY,
        ESETHTCAPABILITIES,
        ESETHTBSSOPERATION,
        ESETHTSECONDARYBEACON,
        ESETHTBLOCKACKCONFIGURE,
        ECONTINUEDOT11TRAVERSE,         // continue dot11 state traversal
        ESTATEMAX                       // defined as an upper bound        
        }; 

public:
    
    /**
    * C++ default constructor.
    */
    WlanDot11MibDefaultConfigure() : iState( EINIT ), iMemory( NULL ) {};
    
    /**
    * Destructor.
    */
    virtual ~WlanDot11MibDefaultConfigure();
    
private:
    
    // Prohibit copy constructor 
    WlanDot11MibDefaultConfigure( const WlanDot11MibDefaultConfigure& );
    // Prohibit assigment operator 
    WlanDot11MibDefaultConfigure& operator=( 
        const WlanDot11MibDefaultConfigure& );

    /**
    * ?member_description.
    * @since Series 60 3.1
    * @param aCtxImpl global statemachine context
    */
    void ChangeInternalState( 
        WlanContextImpl& aCtxImpl, 
        TState aNewState );

    /**
    * ?member_description.
    * @since Series 60 3.1
    * @param aCtxImpl global statemachine context
    */
    void Fsm( 
        WlanContextImpl& aCtxImpl, 
        TEvent aEvent ); 

    void SetDot11MaxReceiveLifeTime( WlanContextImpl& aCtxImpl );
    void SetDot11SlotTime( WlanContextImpl& aCtxImpl );
    void SetDot11GroupAddrsTable( WlanContextImpl& aCtxImpl );
    void SetDot11WepDefaultKey( WlanContextImpl& aCtxImpl );
    void SetDot11CurrentTxPowerLevel( WlanContextImpl& aCtxImpl );
    void SetDot11RtsThreshold( WlanContextImpl& aCtxImpl );
    void SetCtsToSelf( WlanContextImpl& aCtxImpl );
    void SetArpIpAddrsTable( WlanContextImpl& aCtxImpl );
    void SetProbeRequestTemplate( WlanContextImpl& aCtxImpl );
    void SetRxFilter( WlanContextImpl& aCtxImpl );
    void SetBeaconFilterIeTable( WlanContextImpl& aCtxImpl );
    void SetBeaconFilterEnable( WlanContextImpl& aCtxImpl );
    void SetWlanWakeupInterval( WlanContextImpl& aCtxImpl );
    void SetBeaconLostCount( WlanContextImpl& aCtxImpl );
    void SetRcpiThreshold( WlanContextImpl& aCtxImpl );
    void SetTxRatePolicy( WlanContextImpl& aCtxImpl );
    void SetHtCapabilities( WlanContextImpl& aCtxImpl );
    void SetHtBssOperation( WlanContextImpl& aCtxImpl );
    void SetHtSecondaryBeacon( WlanContextImpl& aCtxImpl );
    void SetHtBlockAckConfigure( WlanContextImpl& aCtxImpl );

    /**
    * ?member_description.
    * @since Series 60 3.1
    * @param aCtxImpl global statemachine context
    */
    void OnStateEntryEvent( WlanContextImpl& aCtxImpl );

    /**
    * ?member_description.
    * @since Series 60 3.1
    * @param aCtxImpl global statemachine context
    */
    void OnTxCompleteEvent( WlanContextImpl& aCtxImpl );

    /**
    * ?member_description.
    * @since Series 60 3.1
    * @param aCtxImpl global statemachine context
    */
    void OnAbortEvent( WlanContextImpl& aCtxImpl );

    // Functions from base classes

    /**
    * From ?base_class ?member_description
    */
    virtual void Entry( WlanContextImpl& aCtxImpl );

    /**
    * From ?base_class ?member_description
    */
    virtual void Exit( WlanContextImpl& aCtxImpl );

    /**
    * From ?base_class ?member_description
    * Returns the states name
    */
#ifndef NDEBUG 
    virtual const TInt8* GetStateName( TUint8& aLength ) const;
#endif // !NDEBUG 

private:    // Data

    // state of the object
    TState              iState;

    // general purpose memory handle for the object
    TAny*               iMemory;

#ifndef NDEBUG
    // max length of state name for tracing 
    enum { KMaxStateStringLength = 50 };
    // max length of event name for tracing 
    enum { KMaxEventStringLength = KMaxStateStringLength };

    // state names for tracing 
    static const TUint8 iStateName[ESTATEMAX][KMaxStateStringLength];
    // event names for tracing 
    static const TUint8 iEventName[EEVENTMAX][KMaxEventStringLength];         
    // name of the state 
    static const TInt8  iName[];        
#endif
    };

#endif      // WLANDOT11MIBDEFAULTCONFIGURE_H
