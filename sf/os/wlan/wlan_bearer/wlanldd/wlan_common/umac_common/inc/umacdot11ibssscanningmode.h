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
* Description:   Declaration of the WlanDot11IbssScanningMode class.
*
*/

/*
* %version: 13 %
*/

#ifndef C_WLANDOT11IBSSSCANNINGMODE_H
#define C_WLANDOT11IBSSSCANNINGMODE_H

#include "UmacDot11IbssMode.h"
#include "umacscanfsmcb.h"

class WlanContextImpl;
class WlanScanFsmCntx;

/**
 *  IBSS scanning mode state
 *
 *  @lib wlanumac.lib
 *  @since S60 v3.1
 */
class WlanDot11IbssScanningMode : 
    public WlanDot11IbssMode,
    public MWlanScanFsmCb
    {

public:

    /**  Types for the FSM */
    
    /**  events for the FSM */
    enum TEvent
        {
        ESTATEENTRY,        // state entry action to be executed                        
        ETXCOMPLETE,        // underlying sw layer tx delivery complete event   
        ESTARTSCANNINGMODE, // start scanning mode event
        ESCANMODERUNNING,   // scan running event
        ESTOPSCANNINGMODE,  // stop scanning mode request
        ESCANNINGMODEEXIT,  // scanning mode termination event 

        EABORT,             // abort execution event
        EEVENTMAX           // defined as an upper bound
        };

    /**  states of the FSM */
    enum TState
        {
        EINIT,                  // start state of the state machine
        EEXECUTESCANFSM,        // execute scan fsm
        ECONTINUEDOT11TRAVERSE, // continue dot11 state traversal
        ESTATEMAX               // defined as an upper bound        
        }; 

    WlanDot11IbssScanningMode() 
        : iState( EINIT ), iCtxImpl( NULL ), 
          iMode( static_cast<TScanMode>( 0 ) ), iScanRate( 0 ), iPimpl( NULL ),
          iSSID( NULL ),  iChannels( NULL ), iMinChannelTime( 0 ), 
          iMaxChannelTime( 0 ), iSplitScan( EFalse ) {};

    virtual ~WlanDot11IbssScanningMode();


    void Set( WlanContextImpl& aCtxImpl );

    /**
     * 
     *
     * @since S60 3.1
     * @param ?arg1 ?description
     * @param ?arg2 ?description
     */
    void Set( 
        WlanContextImpl& aCtxImpl,
        TScanMode aMode,                    
        const TSSID& aSSID,                 
        TUint32 aScanRate,                    
        SChannels& aChannels,
        TUint32 aMinChannelTime,            
        TUint32 aMaxChannelTime,
        TBool aSplitScan );                                          

private:

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
     * ?description
     *
     * @since S60 3.1
     * @param aCtxImpl global statemachine context
     */
    void OnStateEntryEvent( WlanContextImpl& aCtxImpl );

    /**
     * ?description
     *
     * @since S60 3.1
     * @param aCtxImpl global statemachine context
     */
    void OnTxCompleteEvent( WlanContextImpl& aCtxImpl ) const;

    /**
     * ?description
     *
     * @since S60 3.1
     * @param aCtxImpl global statemachine context
     */
    void OnStartScanningModeEvent( WlanContextImpl& aCtxImpl );

    /**
     * Handles Scan Running event from scan fsm
     *
     * @since S60 3.2
     * @param aCtxImpl global statemachine context
     */
    void OnScanModeRunningEvent( WlanContextImpl& aCtxImpl ) const;
    
    /**
     * Handles Stop Scanning Mode event from scan fsm
     *
     * @since S60 3.2
     * @param aCtxImpl global statemachine context
     */
    void OnStopScanningModeEvent( WlanContextImpl& aCtxImpl );

    /**
     * ?description
     *
     * @since S60 3.1
     * @param aCtxImpl global statemachine context
     */
    void OnScanningModeExitEvent( WlanContextImpl& aCtxImpl );

    /**
     * ?description
     *
     * @since S60 3.1
     * @param aCtxImpl global statemachine context
     */
    void OnAbortEvent( WlanContextImpl& aCtxImpl );

    /**
     * ?description
     *
     * @since S60 3.1
     * @param aCtxImpl global statemachine context
     */
    void ExecuteScanFsm( WlanContextImpl& aCtxImpl );

    /**
     * ?description
     *
     * @since S60 3.1
     * @param aCtxImpl global statemachine context
     */
    void ContinueDot11StateTraversal( WlanContextImpl& aCtxImpl );

    // from base class MWlanWhaCommandClient

    /**
     * From MWlanWhaCommandClient.
     * ?description
     *
     * @since S60 3.1
     * @param aCtxImpl global statemachine context
     */
    virtual void OnWhaCommandResponse( 
        WlanContextImpl& aCtxImpl, 
        WHA::TCommandId aCommandId, 
        WHA::TStatus aStatus,
        const WHA::UCommandResponseParams& aCommandResponseParams,
        TUint32 aAct );

    // from base class MWlanWsaEvent

    /**
     * From MWlanWsaEvent.
     * ?description
     *
     * @since S60 3.1
     * @param aCtxImpl global statemachine context
     */
    virtual TBool CommandComplete( 
        WlanContextImpl& aCtxImpl, 
        WHA::TCompleteCommandId aCompleteCommandId, 
        WHA::TStatus aStatus,
        const WHA::UCommandCompletionParams& aCommandCompletionParams );

    /**
     * From MWlanWsaEvent.
     * ?description
     *
     * @since S60 3.1
     * @param aCtxImpl global statemachine context
     */
    virtual TAny* RequestForBuffer( 
        WlanContextImpl& aCtxImpl, 
        TUint16 aLength );

    // from base class WlanDot11Associated

    /**
     * From WlanDot11Associated.
     * ?description
     *
     * @since S60 3.1
     * @param aCtxImpl global statemachine context
     * @param aRcpi RCPI of the frame
     * @param aBuffer pointer to the beginning of the Rx buffer allocated
     *        for the frame
     */
    virtual void OnBeaconFrameRx( 
        WlanContextImpl& aCtxImpl,
        const TAny* aFrame,
        const TUint32 aLength,
        WHA::TRcpi aRcpi,
        TUint8* aBuffer );

    /**
     * From WlanDot11Associated.
     * ?description
     *
     * @since S60 3.1
     * @param aCtxImpl global statemachine context
     * @param aRcpi RCPI of the frame
     * @param aBuffer pointer to the beginning of the Rx buffer allocated
     *        for the frame
     */
    virtual void OnProbeResponseFrameRx( 
        WlanContextImpl& aCtxImpl,
        const TAny* aFrame,
        const TUint32 aLength,
        WHA::TRcpi aRcpi,
        TUint8* aBuffer );

    // from base class WlanMacState

    /**
     * From WlanMacState.
     * ?description
     *
     * @since S60 3.1
     * @param aCtxImpl global statemachine context
     */
    virtual void Entry( WlanContextImpl& aCtxImpl );

    /**
     * From WlanMacState.
     * ?description
     *
     * @since S60 3.1
     * @param aCtxImpl global statemachine context
     */
    virtual void Exit( WlanContextImpl& aCtxImpl );
                                   
    /**
    * From WlanMacState.
    * Stop a previously started scan process. 
    *
    * @since S60 3.2
    * @param aCtxImpl global statemachine context
    * @return ETrue if a state transition occurred
    *         EFalse otherwise
    */
    virtual TBool StopScan( WlanContextImpl& aCtxImpl );

    /**
     * From WlanMacState.
     * ?description
     *
     * @since S60 3.1
     * @param aCtxImpl global statemachine context
     */
#ifndef NDEBUG 
        virtual const TInt8* GetStateName( TUint8& aLength ) const;
#endif // !NDEBUG 

    // from base class MWlanScanFsmCb

    virtual void OnScanFsmEvent( MWlanScanFsmCb::TEvent aEvent );

    // Prohibit copy constructor.
    WlanDot11IbssScanningMode( 
        const WlanDot11IbssScanningMode& );
    // Prohibit assigment operator.
    WlanDot11IbssScanningMode& operator= ( 
        const WlanDot11IbssScanningMode& ); 

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
     * global state machine context 
     * Not own.  
     */
    WlanContextImpl*    iCtxImpl;

    /**
     * scan parameter as non-WHA types
     */
    TScanMode           iMode;                  

    /**
     * scan parameter as non-WHA types
     */
    TUint32             iScanRate;                    

    /**
     * pointer to the scan fsm implementation
     * Own.
     */
    WlanScanFsmCntx*    iPimpl;

    /**
     * scan parameter as non-WHA types
     * Not own.  
     */
    const TSSID*        iSSID;                 

    /**
     * scan parameter as non-WHA types
     * Not own.  
     */
    SChannels*          iChannels;

    /**
     * minimum channel time
     */
    TUint32 iMinChannelTime;        

    /**
     * maximum channel time
     */
    TUint32 iMaxChannelTime;                                      

    /**
     * should split scan be used
     */
    TBool iSplitScan;
    };

#endif // C_WLANDOT11IBSSSCANNINGMODE_H
