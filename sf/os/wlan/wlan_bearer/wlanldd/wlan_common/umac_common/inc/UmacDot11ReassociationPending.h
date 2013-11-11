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
* Description:   Declaration of the UmacDot11ReassociationPending class
*
*/

/*
* %version: 22 %
*/

#ifndef WLANDOT11REASSOCIATIONPENDING_H
#define WLANDOT11REASSOCIATIONPENDING_H

#include "UmacDot11Connecting.h"

/**
*  This is a state where STA has been successfully 
*  authenticated with an AP, STA issues a reassociation request 
*  and then waits for a response to it.
*
*  @lib wlanumac.lib
*  @since S60 v3.1
*/
class WlanDot11ReassociationPending : public WlanDot11Connecting
    {
public:
   
    /**
    * C++ default constructor.
    */
    WlanDot11ReassociationPending();

    /**
    * Destructor.
    */
    virtual ~WlanDot11ReassociationPending();

protected:
            
    // events for the FSM
    enum TEvent
        {
        // state entry action to be executed
        ESTATEENTRY,
        // reassociation request frame has been xferred 
        // to the WLAN device tx queue
        ETXREASSOCFRAMEXFER,        
        // reassociation response message has been processed
        ERXREASSOCRESPONSE,
        ETXCOMPLETE,
        // reassociation timeout event       
        ETIMEOUT,
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
        // dot11-reassociation message is sent                        
        ETXREASSOCIATIONFRAME,        
        // reassociation response message is waited
        EWAIT4REASSOCIATIONRESPONSE,
        // AC configuration is done
        ECONFIGUREAC,
        // EPUSHPACKET event waited
        EWAIT4PUSHPACKET,
        // junction state prior moving to next dot11 state
        ECONTINUEDOT11TRAVERSE,
        // defined as an upper bound        
        ESTATEMAX                       
        }; 

    // reassociation frame received flag
    static const TUint32 KReassocReceived              = (1 << 0);
    // reassociation was a success, flag
    static const TUint32 KReassocSuccess               = (1 << 1);
    // need to issue the ConfigureAc WHA cmd, flag
    static const TUint32 KConfigureAc                  = (1 << 2);

    /**
    * ?member_description.
    * @since S60 3.1
    * @return ETrue if we are associated EFalse in otherwise
    */    
    inline TBool Associated() const;

    void Fsm( WlanContextImpl& aCtxImpl, 
              TEvent aEvent );

    void OnStateEntryEvent( WlanContextImpl& aCtxImpl );

    inline void OnTxReassocFrameXferEvent( WlanContextImpl& aCtxImpl );

    void OnRxReassocResponseEvent( WlanContextImpl& aCtxImpl );

    void OnTxCompleteEvent( WlanContextImpl& aCtxImpl );

    void OnTimeoutEvent( WlanContextImpl& aCtxImpl );

    void OnTxSchedulerFullEvent( WlanContextImpl& aCtxImpl );

    void OnPushPacketEvent( WlanContextImpl& aCtxImpl );

    void ContinueDot11StateTraversal( WlanContextImpl& aCtxImpl );

    void ChangeInternalState( 
        WlanContextImpl& aCtxImpl, 
        TState aNewState );

    TBool SendReassociationRequest( WlanContextImpl& aCtxImpl );

    virtual TUint32 ConstructReassociationRequestFrame( 
        WlanContextImpl& aCtxImpl,
        TUint8*& aStartOfFrame );

    void StartReassociationResponseTimer( 
        WlanContextImpl& aCtxImpl ) const;

    static T802Dot11ManagementStatusCode IsRxReassociationSuccess( 
        WlanContextImpl& aCtxImpl,
        const void* aFrame,
        TUint32 aFlags );
        
    inline void ConfigureAc( 
        WlanContextImpl& aCtxImpl );

    /**
    * Returns the state's name
    * @since Series 60 3.1
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

    virtual void OnPacketPushPossible( WlanContextImpl& aCtxImpl );

    virtual TBool OnTimeout( WlanContextImpl& aCtxImpl );        

private:

    // Prohibit copy constructor 
    WlanDot11ReassociationPending( 
        const WlanDot11ReassociationPending& );
    // Prohibit assigment operator 
    WlanDot11ReassociationPending& operator= ( 
        const WlanDot11ReassociationPending& );

protected:   // Data

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
    // holds the name of the state
    static const TInt8 iName[];        
#endif 
    };

#include "umacdot11reassociationpending.inl"

#endif      // WLANDOT11REASSOCIATIONPENDING_H
