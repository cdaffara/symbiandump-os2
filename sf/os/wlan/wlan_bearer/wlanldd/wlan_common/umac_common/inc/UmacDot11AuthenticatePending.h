/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Declaration of the WlanDot11AuthenticatePending class
*
*/

/*
* %version: 22 %
*/

#ifndef WLANDOT11AUTHENTICATEPENDING_H
#define WLANDOT11AUTHENTICATEPENDING_H

#include "UmacDot11Connecting.h"

/**
*  Infrastructure mode authnetication establishment base class.
*  This is a state where authentication request has been sent
*  to an AP and response to it is waited.
*
*  @lib wlanumac.lib
*  @since S60 v3.1
*/
class WlanDot11AuthenticatePending : public WlanDot11Connecting
    {
public:

    /**
    * Destructor.
    */
    virtual ~WlanDot11AuthenticatePending() {};

protected:  

    // Types for the FSM
    
    // events for the FSM
    enum TEvent
        {
        ESTATEENTRY,        // state entry action to be executed
        ECONTINUE,          // generic continue event
        // a valid authentication response message has been processed
        // authentication attempt may have accpeted or denied
        ERXAUTHRESPONSE,
        // authenticate request frame has been xferred 
        // to the WLAN device tx queue
        ETX_AUTHFRAME_XFER,
        ETX_SCHEDULER_FULL, // packet scheduler is full
        ETIMEOUT,           // authentication timer timeout event       
        EPUSHPACKET,        // packet push to packet scheduler possible
        EEVENTMAX           // defined as an upper bound
        };

    // states of the FSM
    enum TState
        {
        EINIT,                          // start state of the state machine
        // dot11-authentication message is sent
        ETXAUTHFRAME,                               
        // authentication response message is waited
        EWAIT4AUTHRESPONSE,             
        // EPUSHPACKET event waited
        EWAIT4PUSHPACKET,
        // junction state prior moving to dot11associationpending
        // or dot11softreset dot11 state
        ECONTINUEDOT11TRAVERSE,   
        ESTATEMAX                       // defined as an upper bound        
        }; 

    // authentication frame received flag
    static const TUint32 KAuthReceived             = (1 << 0);
    // authentication was a success flag
    static const TUint32 KAuthSuccess              = (1 << 1);

protected:
    
    /**
    * C++ default constructor.
    */
    WlanDot11AuthenticatePending() : iState( EINIT ) {};

    /**
    * Parses authentication response message(s)
    * @since S60 3.1
    * @param aCtxImpl global statemachine context
    * @param aAuthModeDesired authentication mode expected
    * @param aFrame pointer to the beginning of the authentication response
    *               frame
    * @param aFlags flags from WHA frame receive method
    * @return ETrue proceed with authentication sequnce, otherwise abort
    */
    static TBool ResolveAuthMessage( 
        WlanContextImpl&,
        TUint16 aAuthModeDesired,
        const void* aFrame,
        TUint32 aFlags );

    /**
    * Sends authentication sequence number 1 message
    * @since S60 3.1
    * @param aCtxImpl global statemachine context
    * @return ETrue upon success, 
    * EFalse if packet scheduler discarded the frame
    */
    TBool SendAuthSeqNbr1Frame( WlanContextImpl& aCtxImpl ) const;

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
    void ContinueDot11StateTraversal( WlanContextImpl& aCtxImpl );

    /**
    * ?member_description.
    * @since S60 3.1
    * @param aCtxImpl global statemachine context
    */
    void StateEntryActions( WlanContextImpl& aCtxImpl );

    /**
    * ?member_description.
    * @since S60 3.1
    * @param aCtxImpl global statemachine context
    */
    static void StartAuthenticationFrameResponseTimer( 
        WlanContextImpl& aCtxImpl );
    
private:    
    
    /**
    * ?member_description.
    * @since S60 3.1
    * @return ETrue if we are authenticated EFalse in otherwise
    */    
    inline TBool Authenticated() const;
    
    /**
    * ?member_description.
    * @since S60 3.1
    * @param aCtxImpl global statemachine context
    */
    virtual void OnStateEntryEvent( WlanContextImpl& aCtxImpl ) = 0;

    /**
    * ?member_description.
    * @since S60 3.1
    * @param aCtxImpl global statemachine context
    */
    virtual void OnRxAuthResponseEvent( WlanContextImpl& aCtxImpl ) = 0;

    /**
    * Called internally to set the used 
    * authentication number to authenticate message
    * @since S60 3.1
    * @param aCtxImpl global statemachine context
    */
    virtual void OnSetAlgorithmNumber( WlanContextImpl& aCtxImpl ) = 0;

    /**
    * ?member_description.
    * @since S60 3.1
    * @param aCtxImpl global statemachine context
    */
    void OnContinueEvent( WlanContextImpl& aCtxImpl );

    /**
    * ?member_description.
    * @since S60 3.1
    * @param aCtxImpl global statemachine context
    */
    void OnTimeoutEvent( WlanContextImpl& aCtxImpl );

    /**
    * ?member_description.
    * @since S60 3.1
    * @param aCtxImpl global statemachine context
    */
    void OnPushPacketEvent( WlanContextImpl& aCtxImpl );

    /**
    * ?member_description.
    * @since S60 3.1
    * @param aCtxImpl global statemachine context
    */
    void OnTxAuthFrameXferEvent( WlanContextImpl& aCtxImpl );

    /**
    * ?member_description.
    * @since S60 3.1
    * @param aCtxImpl global statemachine context
    */
    void OnTxSchedulerFullEvent( WlanContextImpl& aCtxImpl );

    // Functions from base classes

    /**
    * From ?base_class ?member_description.
    * @since S60 3.1
    * @param aCtxImpl global statemachine context
    */
    virtual void Entry( WlanContextImpl& aCtxImpl );

    /**
    * From ?base_class ?member_description.
    * State exit method
    * @since S60 3.1
    * @param aCtxImpl global statemachine context
    */
    virtual void Exit( WlanContextImpl& aCtxImpl );

    /**
    * From ?base_class ?member_description.
    * Timer timeout function
    * @since S60 3.1
    * @param aCtxImpl global statemachine context
    * @return ETrue if a state change occurred in the state machine
    *         EFalse otherwise
    */
    virtual TBool OnTimeout( WlanContextImpl& aCtxImpl );

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

    /**
    * From ?base_class ?member_description.
    * @since S60 3.1
    * @param aCtxImpl global statemachine context
    * @param aRcpi RCPI of the frame
    * @param aFlags flags from WHA frame receive method
    * @param aBuffer pointer to the beginning of the Rx buffer allocated
    *        for the frame
    */
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

    /**
    * From ?base_class ?member_description.
    * @since S60 3.1
    * @param aCtxImpl global statemachine context
    */
    virtual void OnPacketPushPossible( WlanContextImpl& aCtxImpl );
        
    // Prohibit copy constructor 
    WlanDot11AuthenticatePending( const WlanDot11AuthenticatePending& );
    // Prohibit assigment operator
    WlanDot11AuthenticatePending& operator= ( 
        const WlanDot11AuthenticatePending& );        

protected:  // Data

    TState              iState;

#ifndef NDEBUG
    /** max length of state name for tracing */
    enum { KMaxStateStringLength = 50 };
    /** max length of event name for tracing */
    enum { KMaxEventStringLength = KMaxStateStringLength };

    /** state names for tracing */
    static const TUint8 iStateName[ESTATEMAX][KMaxStateStringLength];
    /** event names for tracing */
    static const TUint8 iEventName[EEVENTMAX][KMaxEventStringLength];
#endif
    };

#include "umacdot11authenticatepending.inl"

#endif      // WLANDOT11AUTHENTICATEPENDING_H
