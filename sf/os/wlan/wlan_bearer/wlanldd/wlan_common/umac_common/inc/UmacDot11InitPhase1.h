/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Declaration of the WlanDot11InitPhase1 class
*
*/

/*
* %version: 18 %
*/

#ifndef WLANDOT11INITPHASE1_H
#define WLANDOT11INITPHASE1_H

//  INCLUDES
#include "UmacDot11State.h"

/**
*  This is the start (first level initialization) state of the statemachine
*/
class WlanDot11InitPhase1 : public WlanDot11State
    {
    // events for the FSM
    enum TEvent
        {
        ESTATEENTRY,        // state entry action to be executed
        ETXCOMPLETE,        // tx process complete event 
        EOIDCONFIGURE,      // recv: OID-configure
        EABORT,             // abort event
        EEVENTMAX           // defined as an upper bound
        };

    // states of the FSM
    enum TState
        {
        EINIT,                          // start state of the fsm
        EINITILIAZE,                    // execute WHA-Initialize
        ECOMPLETEBOOTUP,                // complete bootup oid
        EWAIT4OIDCONFIGURE,             // wait 4 OID-configure
        EHANDLEOIDCONFIGURE,            // handle OID-configure
        ECONFIGURE,                     // execute WHA-Configure
        EREADSTATIONID,                 // read station ID mib        
        ECONFTXQUEUE,                   // configure tx-queue
        ECONTINUEDOT11TRAVERSE,         // entry: traverse dot11 state
        ESTATEMAX                       // defined as an upper bound        
        };                              

public:
   
    /**
    * C++ default constructor.
    */
    WlanDot11InitPhase1() 
        : iState( EINIT ), 
        iRTSThreshold( 0 ),
        iMaxTxMSDULifetime( 0 ), iPda( NULL ), iPdaLen( 0 )  {};

    /**
    * Destructor.
    */
    virtual ~WlanDot11InitPhase1();

    void BootUp( 
        WlanContextImpl& aCtxImpl, 
        const TUint8* aPda, 
        TUint32 aPdaLength,
        const TUint8* aFw, 
        TUint32 aFwLength );
            
protected:

    /**
    * Returns the states name
    * @param aLength (OUT) length of the name of the state
    * @return name of the state
    */
#ifndef NDEBUG 
    virtual const TInt8* GetStateName( TUint8& aLength ) const;
#endif // !NDEBUG 
    
private:

    /**
    * State entry action
    * @param  aCtxImpl statemachine context
    */
    virtual void Entry( WlanContextImpl& aCtxImpl );
    virtual void Exit( WlanContextImpl& aCtxImpl );

    /**
    * ?member_description.
    * @since S60 3.1
    * @param ?arg1 ?description
    * @return ?description
    */
    inline void ChangeInternalState( 
        WlanContextImpl& aCtxImpl, 
        TState aNewState );

    /**
    * ?member_description.
    * @since S60 3.1
    * @param ?arg1 ?description
    * @return ?description
    */
    void Fsm( 
        WlanContextImpl& aCtxImpl, 
        TEvent aEvent );

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
    * @since S60 3.1
    * @param aCtxImpl global statemachine context
    */
    void OnAbortEvent( WlanContextImpl& aCtxImpl );

    /**
    * ?member_description.
    * @since Series 60 3.1
    * @param aCtxImpl global statemachine context
    */
    void OnOIDConfigureEvent( WlanContextImpl& aCtxImpl );

    /**
    * ?member_description.
    * @since Series 60 3.1
    * @param aCtxImpl global statemachine context
    */
    void Initialize( WlanContextImpl& aCtxImpl );

    /**
    * ?member_description.
    * @since Series 60 3.1
    * @param aCtxImpl global statemachine context
    */
    void Configure( WlanContextImpl& aCtxImpl );

    /**
    * ?member_description.
    * @since Series 60 3.1
    * @param aCtxImpl global statemachine context
    */
    void ReadStationID( WlanContextImpl& aCtxImpl );

    /**
    * ?member_description.
    * @since Series 60 3.1
    * @param aCtxImpl global statemachine context
    */
    void CompleteOid( WlanContextImpl& aCtxImpl ) const;

    /**
    * ?member_description.
    *    
    * @since Series 60 3.1
    * @param aCtxImpl global statemachine context
    */
    void HandleOIDConfigure( WlanContextImpl& aCtxImpl );

    /**
    * ?member_description.
    *    
    * @since Series 60 3.1
    * @param aCtxImpl global statemachine context
    */
    void ConfigureQueue( 
        WlanContextImpl& aCtxImpl );
         
    /**
    * ?member_description.
    *    
    * @since Series 60 3.1
    * @param aCtxImpl global statemachine context
    */
   void ConfigureUmacFrameTemplates( WlanContextImpl& aCtxImpl ) const;

   /**
   * Configures our own HT capabilities element
   *    
   * @param aCtxImpl global statemachine context
   */
   void ConfigureOurHtCapabilitiesElement( WlanContextImpl& aCtxImpl ) const;
   // Methods from base classes

    /**
    * Set inital WLAN specific parameters. 
    * @param aCtxImpl statemachine context
    * @param aRTSThreshold
    *        Limit for packet size when to use RTS/CTS protocol.
    * @param aMaxTxMSDULifetime
    *        Limit for packet size when to use RTS/CTS protocol.
    * @param aVoiceCallEntryTimeout when we are not in Voice over WLAN Call
    *        state and we transmit at least aVoiceCallEntryTxThreshold
    *        Voice priority frames during the time period (microseconds)
    *        denoted by this parameter, we enter Voice over WLAN Call state
    * @param aVoiceCallEntryTxThreshold Threshold value for the number of 
    *        Voice priority Tx frames to enter Voice over WLAN Call state    
    * @param aVoiceNullTimeout NULL frame sending interval during a Voice over
    *        WLAN call in U-APSD power save mode
    * @param aNoVoiceTimeout after this long time of no Voice priority data 
    *        Tx, exit Voice over WLAN call state
    * @param aKeepAliveTimeout Keep Alive frame sending interval in 
    *        infrastructure mode
    * @param aSpRcpiIndicationLevel If this RCPI level is predicted to be
    *        reached within the time specified by aSpTimeToCountPrediction,
    *        a signal loss prediction indication is sent. 
    * @param aSpTimeToCountPrediction Specifies the time (in microseconds)
    *        how far into the future signal prediction is done.
    * @param aSpMinIndicationInterval The minimum time difference (in 
    *        microseconds) between two signal loss prediction indications.
    * @return 0 = success, any other failure
    */
    virtual TBool Configure(
        WlanContextImpl& aCtxImpl,
        TUint32 aRTSThreshold,              
        TUint32 aMaxTxMSDULifetime,
        TUint32 aVoiceCallEntryTimeout,
        TUint32 aVoiceCallEntryTxThreshold,
        TUint32 aVoiceNullTimeout,
        TUint32 aNoVoiceTimeout,
        TUint32 aKeepAliveTimeout,
        TUint32 aSpRcpiIndicationLevel,
        TUint32 aSpTimeToCountPrediction,
        TUint32 aSpMinIndicationInterval);  
    
    virtual void OnWhaCommandResponse( 
        WlanContextImpl& aCtxImpl, 
        WHA::TCommandId aCommandId, 
        WHA::TStatus aStatus,
        const WHA::UCommandResponseParams& aCommandResponseParams,
        TUint32 aAct );

    /**
     * From MWlanUserEvent
     * Make system ready for unloading
     *
     * @since S60 3.1
     */
    virtual void FinitSystem( WlanContextImpl& aCtxImpl );

    // Prohibit copy constructor.
    WlanDot11InitPhase1( const WlanDot11InitPhase1& );
    // Prohibit assigment operator.
    WlanDot11InitPhase1& operator= ( const WlanDot11InitPhase1& );

private:   // Data  
    
#ifndef NDEBUG
    /** max length of state name for tracing */
    enum { KMaxStateStringLength = 50 };
    /** max length of event name for tracing */
    enum { KMaxEventStringLength = KMaxStateStringLength };

    /** state names for tracing */
    static const TUint8 iStateName[ESTATEMAX][KMaxStateStringLength];
    /** event names for tracing */
    static const TUint8 iEventName[EEVENTMAX][KMaxEventStringLength];
    /** name of the state */
    static const TInt8  iName[];  
#endif

    /** current state */
    TState              iState;    
    
    TUint32 iRTSThreshold;              
    TUint32 iMaxTxMSDULifetime;

    TUint8* iPda;
    TUint32 iPdaLen;
    };

#endif      // WLANDOT11INITPHASE1_H
