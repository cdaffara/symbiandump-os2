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
* Description:   Declaration of the UmacDot11DisassociationPending class
*
*/

/*
* %version: 17 %
*/

#ifndef WLANDOT11DISASSOCIATIONPENDING_H
#define WLANDOT11DISASSOCIATIONPENDING_H

#include "UmacDot11Roam.h"

/**
*  This is a state where disassociation request frame is sent to an AP.
*
*  @lib wlanumac.lib
*  @since S60 v3.1
*/
class WlanDot11DisassociationPending : public WlanDot11Roam
    {
public:

    /**
    * C++ default constructor.
    */
    WlanDot11DisassociationPending();

    /**
    * Destructor.
    */
    virtual ~WlanDot11DisassociationPending();
            
private:

    // events for the FSM
    enum TEvent
        {
        // state entry action to be executed
        ESTATEENTRY,
        // disassociation request frame has been sent
        // from the WLAN device
        ETXPACKETCOMPLETE,
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
        // state for sending the disassociation frame
        ETXDISASSOCIATIONFRAME,
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

    /**
    * Returns the states name
    * @since Series 60 3.1
    * @param aLength (OUT) length of the name of the state
    * @return name of the state
    */
#ifndef NDEBUG 
        virtual const TInt8* GetStateName( TUint8& aLength ) const;
#endif // !NDEBUG 
    
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

    virtual void OnPacketPushPossible( WlanContextImpl& aCtxImpl );

    // Prohibit copy constructor
    WlanDot11DisassociationPending( 
        const WlanDot11DisassociationPending& );

    // Prohibit assignment operator
    WlanDot11DisassociationPending& operator= ( 
        const WlanDot11DisassociationPending& );  
    
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
    // name of the state 
    static const TInt8 iName[];        
#endif 
    };

#endif      // WLANDOT11DISASSOCIATIONPENDING_H

