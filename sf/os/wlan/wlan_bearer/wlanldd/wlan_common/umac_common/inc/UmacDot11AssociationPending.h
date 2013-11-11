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
* Description:   Declaration of the WlanDot11AssociationPending class
*
*/

/*
* %version: 23 %
*/

#ifndef WLANDOT11ASSOCIATIONPENDING_H
#define WLANDOT11ASSOCIATIONPENDING_H

#include "UmacDot11Connecting.h"

/**
* AssociationPending is a state where STA has been successfully authenticated
* itself with an AP and has issued an association request 
* and it's waiting a response to it.
*/
class WlanDot11AssociationPending : public WlanDot11Connecting
    {

public:
   
    /**
    * C++ default constructor.
    */
    WlanDot11AssociationPending() : iState( EINIT ) {};

    /**
    * Destructor.
    */
    virtual ~WlanDot11AssociationPending() {};

protected:

    // Types for the FSM
    //

    // events for the FSM
    enum TEvent
        {
        // state entry action to be executed
        ESTATEENTRY,
        // an association response message has been processed
        ERXASSOCRESPONSE,
        // associate request frame has been xferred 
        // to the WLAN device tx queue
        ETX_ASSOCFRAME_XFER,
        ETXCOMPLETE,
        ETIMEOUT,           // association timer timeout event       
        ETX_SCHEDULER_FULL, // packet scheduler is full
        EPUSHPACKET,        // packet push to packet scheduler possible
        // defined as an upper bound
        EEVENTMAX           
        };

    // states of the FSM
    enum TState
        {
        // start state of the state machine
        EINIT,           
        // dot11-association message is sent            
        ETXASSOCREQFRAME,
        // association response message is waited
        EWAIT4ASSOCRESPONSE,
        // AC configuration is done
        ECONFIGUREAC,
        // EPUSHPACKET event waited
        EWAIT4PUSHPACKET,
        // junction state prior moving to next dot11 state
        ECONTINUEDOT11TRAVERSE,
        // defined as an upper bound        
        ESTATEMAX                       
        }; 

    // association frame received flag
    static const TUint32 KAssocReceived            = (1 << 0);
    // association was a success flag
    static const TUint32 KAssocSuccess             = (1 << 1);
    // need to issue the ConfigureAc WHA cmd flag
    static const TUint32 KConfigureAc              = (1 << 2);

    /**
    * ?member_description.
    * @since S60 3.1
    * @return ETrue if we are associated EFalse in otherwise
    */    
    inline TBool Associated() const;

    void Fsm( 
        WlanContextImpl& aCtxImpl, 
        TEvent aEvent );

    void OnStateEntryEvent( 
        WlanContextImpl& aCtxImpl );

    void OnTimeoutEvent( WlanContextImpl& aCtxImpl );

    void OnTxAssocFrameXferEvent( WlanContextImpl& aCtxImpl );

    void OnRxAssociationResponseEvent( WlanContextImpl& aCtxImpl );

    void OnTxCompleteEvent( WlanContextImpl& aCtxImpl );

    void OnTxSchedulerFullEvent( WlanContextImpl& aCtxImpl );

    void OnPushPacketEvent( WlanContextImpl& aCtxImpl );

    void ContinueDot11StateTraversal( 
        WlanContextImpl& aCtxImpl );

    void ChangeInternalState( 
        WlanContextImpl& aCtxImpl, 
        TState aNewState );

    TBool SendAssociationRequest( WlanContextImpl& aCtxImpl );

    TUint virtual ConstructAssociationRequestFrame( 
        WlanContextImpl& aCtxImpl,
        TUint8*& aStartOfFrame );

    /**
    * Determines if association succeeded
    * @param aCtxImpl global statemachine context
    * @param aFrame pointer to the beginning of the association response
    *               frame
    * @param aFlags flags from WHA frame receive method
    */    
    static T802Dot11ManagementStatusCode IsRxAssociationSuccess( 
        WlanContextImpl& aCtxImpl,
        const void* aFrame,
        TUint32 aFlags );

    static void StartAssociationFrameResponseTimer( 
        WlanContextImpl& aCtxImpl );

    void ConfigureAc( 
        WlanContextImpl& aCtxImpl );

    // from base class ?

    /**
    * Returns the states name
    * @param aLength (OUT) length of the name of the state
    * @return name of the state
    */
#ifndef NDEBUG 
        virtual const TInt8* GetStateName( TUint8& aLength ) const;
#endif // !NDEBUG 

    virtual void Entry( WlanContextImpl& aCtxImpl );
    
    virtual void Exit( WlanContextImpl& aCtxImpl );
    
    /**
     * Method called when packet has been transferred to the WLAN device
     *
     * @since S60 3.1
     * @param aCtxImpl global state machine context
     * @param aPacketId packet whose transfer is complete
     * @param aMetaHeader frame meta header
     */
    virtual void OnPacketTransferComplete( 
        WlanContextImpl& aCtxImpl, 
        TUint32 aPacketId,
        TDataBuffer* aMetaHeader );

    virtual void ReceivePacket( 
        WlanContextImpl& aCtxImpl,
        WHA::TStatus aStatus,
        const void* aFrame,
        TUint16 aLength,
        WHA::TRate aRate,
        WHA::TRcpi aRcpi,
        WHA::TChannelNumber aChannel,
        TUint8* aBuffer,
        TUint32 aFlags );

    virtual void OnReceiveFrameSuccess(
        WlanContextImpl& aCtxImpl,
        const void* aFrame,
        TUint16 aLength,
        WHA::TRcpi aRcpi,
        TUint32 aFlags,
        TUint8* aBuffer );

    /**
    * Timer timeout function
    *
    * @since Series 60 3.1
    * @param aCtxImpl global statemachine context
    * @return ETrue if a state change occurred in the state machine
    *         EFalse otherwise
    */
    virtual TBool OnTimeout( WlanContextImpl& aCtxImpl );

    virtual void OnPacketPushPossible( WlanContextImpl& aCtxImpl );

private:
    
    // Prohibit copy constructor
    WlanDot11AssociationPending( 
        const WlanDot11AssociationPending& );
    // Prohibit assigment operator
    WlanDot11AssociationPending& operator= ( 
        const WlanDot11AssociationPending& );

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
    /** holds the name of the state */
    static const TInt8 iName[];        
#endif 
    };

#include "umacdot11associatepending.inl"

#endif      // WLANDOT11ASSOCIATIONPENDING_H

