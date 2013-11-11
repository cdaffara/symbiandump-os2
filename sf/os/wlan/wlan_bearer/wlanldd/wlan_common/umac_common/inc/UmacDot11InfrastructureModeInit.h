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
* Description:   Declaration of the WlanDot11InfrastructureModeInit class
*
*/

/*
* %version: 22 %
*/

#ifndef WLANDOT11INFRASTRUCTUREMODEINIT_H
#define WLANDOT11INFRASTRUCTUREMODEINIT_H

#include "UmacDot11InfrastructureMode.h"

/**
*  Initialization state for infrastructure mode
*
*  @lib wlanumac.lib
*  @since S60 v3.1
*/
class WlanDot11InfrastructureModeInit : public WlanDot11InfrastructureMode
    {     

public:

    /**
    * C++ default constructor.
    */
    WlanDot11InfrastructureModeInit() : 
            iFlags( 0 ), 
            iState( EINIT ), 
            iDtim( 0 ), 
            iMibMemoryBuffer ( NULL ) {};

    /**
    * Destructor.
    */
    virtual ~WlanDot11InfrastructureModeInit();
    
private:

    // events for the FSM
    enum TEvent
        {
        ESTATEENTRY,        // state entry action to be executed
        // underlying sw layer tx delivery complete event 
        ETXCOMPLETE,        
        EBEACONPROCESSED,   // beacon parsed for elements
        EUSERDATAENABLE,    // protocol stack side datapath enabled
        EABORT,             // abort fsm event
        EEVENTMAX           // defined as an upper bound
        };

    // states of the FSM
    enum TState
        {
        EINIT,                          // start state of the state machine                           
        EENABLEBEACONRX,                // enable beacon reception
        ESETNULLDATAFRAMETEMPLATE,      // set NULL data frame template 
        ESETQOSNULLDATAFRAMETEMPLATE,   // set QoS NULL data frame template 
        ESETPSPOLLTEMPLATE,             // set ps poll frame template
        EWAIT4BEACON,                   // wait 4 beacon state
        EDISABLEBEACONRX,               // disable beacon reception
        ESETBSSPARAMS,                  // set BSS parameters
        ECONFBEACONFILTERIETABLE,       // configure beacon filter IE table
        EWAIT4USERDATAENABLE,           // wait 4 EUSERDATAENABLE event 
        // junction state prior moving to dot11awakemode
        // or dot11enterdozemode dot11 state
        ECONTINUEDOT11TRAVERSE,   
        ESTATEMAX                       // defined as an upper bound        
        };         

    /**
    * ?member_description.
    * @since S60 3.1
    * @param ?arg1 ?description
    * @return ?description
    */
    void EnableBeaconRx( WlanContextImpl& aCtxImpl );

    /**
    * ?member_description.
    * @since S60 3.1
    * @param ?arg1 ?description
    * @return ?description
    */
    void DisableBeaconRx( WlanContextImpl& aCtxImpl );

    /**
    * ?member_description.
    * @since S60 3.1
    * @param ?arg1 ?description
    * @return ?description
    */
    void SetNullDataFrameTemplate( WlanContextImpl& aCtxImpl );

    /**
    * ?member_description.
    * @since S60 3.1
    * @param ?arg1 ?description
    * @return ?description
    */
    void SetQosNullDataFrameTemplate( WlanContextImpl& aCtxImpl );

    /**
    * ?member_description.
    * @since S60 3.1
    * @param ?arg1 ?description
    * @return ?description
    */
    void SetPsPollFrameTemplate( WlanContextImpl& aCtxImpl );

    /**
    * ?member_description.
    * @since S60 3.1
    * @param ?arg1 ?description
    * @return ?description
    */
    void SetBssParams( WlanContextImpl& aCtxImpl );

    /**
    * ?member_description.
    * @since S60 3.1
    * @param ?arg1 ?description
    * @return ?description
    */
    void ConfigureBeaconFilterIeTable( WlanContextImpl& aCtxImpl );

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
    * @since S60 3.1
    * @param ?arg1 ?description
    * @return ?description
    */
    void OnStateEntryEvent( WlanContextImpl& aCtxImpl );

    /**
    * ?member_description.
    * @since S60 3.1
    * @param ?arg1 ?description
    * @return ?description
    */
    void OnTxCompleteEvent( WlanContextImpl& aCtxImpl );

    /**
    * ?member_description.
    * @since S60 3.1
    * @param ?arg1 ?description
    * @return ?description
    */
    void OnBeaconProcessedEvent( WlanContextImpl& aCtxImpl );

    /**
    * ?member_description.
    * @since S60 3.1
    * @param ?arg1 ?description
    * @return ?description
    */
    void OnUserDataEnableEvent( WlanContextImpl& aCtxImpl );

    /**
     * 
     *
     * @since S60 3.1
     * @param ?arg1 ?description
     * @param ?arg2 ?description
     */
    void OnAbortEvent( WlanContextImpl& aCtxImpl );

    /**
    * ?member_description.
    * @since S60 3.1
    * @param ?arg1 ?description
    * @return ?description
    */
    void ContinueDot11StateTraversal( WlanContextImpl& aCtxImpl );

    /**
     * 
     *
     * @since S60 3.1
     * @param ?arg1 ?description
     * @param ?arg2 ?description
     */
    void CompleteConnectRequest( 
        WlanContextImpl& aCtxImpl, 
        TInt aCompletionCode ) const;
        
    /**
    * From 
    * @since S60 3.1
    * @param ?arg1 ?description
    */
#ifndef NDEBUG 
        virtual const TInt8* GetStateName( TUint8& aLength ) const;
#endif // !NDEBUG 

    /**
    * From 
    * @since S60 3.1
    * @param aCtxImpl Global state machine context
    */
    virtual void Entry( WlanContextImpl& aCtxImpl );

    /**
    * From 
    * @since S60 3.1
    * @param aCtxImpl Global state machine context
    */
    virtual void Exit( WlanContextImpl& aCtxImpl );

    /**
    * From 
    * @since S60 3.1
    * @param aCtxImpl Global state machine context
    */
    virtual TBool EnableUserData( WlanContextImpl& aCtxImpl );

    /**
    * Scan all available networks. 
    * This SW module decides is background scan or foreground scan used
    * @param aCtxImpl statemachine context
    * @param aMode passive or active scan see #TScanMode
    * @param aSSID Network that APs are scanned. Can be broadcast SSID.
    * @param aScanRate Rate that is used in active scanning 
    * @param aMinChannelTime min. time to listen beacons/probe responses on 
    *                        a channel
    * @param aMaxChannelTime max. time to listen beacons/probe responses on 
    *                        a channel
    * @param aSplitScan if ETrue, use split scan
    * @return KErrNone = command completed successfully, 
    * any other = failure
    */
    virtual TBool RealScan(
        WlanContextImpl& aCtxImpl,
        TScanMode aMode,                    
        const TSSID& aSSID,                 
        TUint32 aScanRate,                    
        SChannels& aChannels,
        TUint32 aMinChannelTime,            
        TUint32 aMaxChannelTime,
        TBool aSplitScan );                          
    
    /**
    * Called upon receiving a beacon type frame
    *
    * @param aCtxImpl statemachine context
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

    // Prohibit copy constructor 
    WlanDot11InfrastructureModeInit( 
        const WlanDot11InfrastructureModeInit& );
    // Prohibit assigment operator 
    WlanDot11InfrastructureModeInit& operator= ( 
        const WlanDot11InfrastructureModeInit& );

private: // Data

    /** event flags */
    TUint32             iFlags;
    /** current state */
    TState              iState;        
    /** DTIM */
    TUint               iDtim;

    // pointer to memory buffer used for mib writing
    TAny*               iMibMemoryBuffer;

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
    };

#endif      // WLANDOT11INFRASTRUCTUREMODEINIT_H

