/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Declaration of the WlanDot11DeauthPending class
*
*/

/*
* %version: 18 %
*/

#ifndef WLANDOT11DEAUTHPENDING_H
#define WLANDOT11DEAUTHPENDING_H

#include "UmacDot11State.h"

/**
* This is a state where deauthentication frame has been send to an AP 
*/
class WlanDot11DeauthPending : public WlanDot11State
    {
public:

    /**
    * C++ default constructor.
    */
    WlanDot11DeauthPending() : iState( EINIT ) {};

    /**
    * Destructor.
    */
    virtual ~WlanDot11DeauthPending() {};
                
private:

    // events for the FSM
    enum TEvent
        {
        // state entry action to be executed
        ESTATEENTRY,
        // underlying sw layer tx delivery complete event 
        ETXCOMPLETE,        
         // packet scheduler is full
        ETX_SCHEDULER_FULL,
        // packet push to packet scheduler possible
        EPUSHPACKET,
        // defined as an upper bound
        EEVENTMAX           
        };

    // states of the FSM
    enum TState
        {
        // start state of the state machine
        EINIT,                          
        // state for sending the deauthentication frame
        ETXDEAUTHENTICATIONFRAME,
        // EPUSHPACKET event waited
        EWAIT4PUSHPACKET,
        // junction state prior moving to next dot11 state
        ECONTINUEDOT11TRAVERSE,
        // defined as an upper bound        
        ESTATEMAX                       
        };

    void Fsm( 
        WlanContextImpl& aCtxImpl, 
        TEvent aEvent ); 
    
    void OnStateEntryEvent( 
        WlanContextImpl& aCtxImpl );
        
    void OnTxCompleteEvent( 
        WlanContextImpl& aCtxImpl );

    void OnTxSchedulerFullEvent( WlanContextImpl& aCtxImpl );

    void OnPushPacketEvent( WlanContextImpl& aCtxImpl );

    void ContinueDot11StateTraversal( 
        WlanContextImpl& aCtxImpl );

    void ChangeInternalState( 
        WlanContextImpl& aCtxImpl, 
        TState aNewState );
        
    virtual void Entry( WlanContextImpl& aCtxImpl);
    virtual void Exit( WlanContextImpl& aCtxImpl);

    virtual void OnPacketSendComplete(
        WlanContextImpl& aCtxImpl, 
        WHA::TStatus aStatus,
        TUint32 aPacketId,
        WHA::TRate aRate,
        TUint32 aPacketQueueDelay,
        TUint32 aMediaDelay,
        TUint aTotalTxDelay,
        TUint8 aAckFailures,
        WHA::TQueueId aQueueId,
        WHA::TRate aRequestedRate,
        TBool aMulticastData );

    virtual TAny* RequestForBuffer ( 
        WlanContextImpl& aCtxImpl,
        TUint16 aLength );

    /**
    * Returns the states name
    * @param aLength (OUT) length of the name of the state
    * @return name of the state
    */
#ifndef NDEBUG 
        virtual const TInt8* GetStateName( TUint8& aLength ) const;
#endif // !NDEBUG 

    virtual void OnPacketPushPossible( WlanContextImpl& aCtxImpl );

    // Prohibit copy constructor
    WlanDot11DeauthPending( 
        const WlanDot11DeauthPending& );
    // Prohibit assigment operator
    WlanDot11DeauthPending& operator= ( 
        const WlanDot11DeauthPending& );  
    
private:   // Data

    TState              iState;

#ifndef NDEBUG    
    // max length of state name for tracing
    enum { KMaxStateStringLength = 50 };
    // max length of event name for tracing
    enum { KMaxEventStringLength = KMaxStateStringLength };

    // state names for tracing
    static const TUint8 iStateName[ESTATEMAX][KMaxStateStringLength];
    // event names for tracing
    static const TUint8 iEventName[EEVENTMAX][KMaxEventStringLength];             
    /** name of the state */
    static const TInt8 iName[];        
#endif    
    };

#endif      // WLANDOT11DEAUTHPENDING_H

