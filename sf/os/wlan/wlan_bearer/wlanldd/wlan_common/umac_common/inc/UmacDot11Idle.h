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
* Description:   Declaration of the WlanDot11Idle class
*
*/

/*
* %version: 26 %
*/

#ifndef WLANDOT11IDLE_H
#define WLANDOT11IDLE_H

#include "UmacDot11State.h"

/**
*  Logical start state of the dot11 protocol statemachine
*
*  @lib wlanumac.lib
*  @since S60 v3.1
*/
class WlanDot11Idle : public WlanDot11State
    {
    // Types for the FSM
    
    // events for the FSM
    enum TEvent
        {
        ESTATEENTRY,    // state entry action to be executed                        
        ETXCOMPLETE,    // underlying sw layer tx delivery complete event   
        ESCAN,          // scan event
        ECONNECT,       // connect to BSS event
        ECONNECTIBSS,   // connect to IBSS event
        EDISCONNECT,    // disconnect event
        ERELEASE,       // release event
        EABORT,         // abort execution event
        EEVENTMAX       // defined as an upper bound
        };

    // states of the FSM
    enum TState
        {
        EINIT,          // start state of the state machine
        EWRITEMIB,      // write mib
        EFINIT,         // end state
        ESTATEMAX       // defined as an upper bound        
        }; 
       
public:
   
    /**
    * C++ default constructor.
    */
    WlanDot11Idle() : 
            iState( EINIT ),
            iCompletionCode( KErrNone ), 
            iEventMask( 0 ) {};

    /**
    * Destructor.
    */
    virtual ~WlanDot11Idle() {};

    void Set( TInt aCompletionCode );

    /**
    * Requests this state to indicate scan completion to management client
    * upon state entry.
    *
    * @since S60 3.2
    */
    void CompleteScanUponEntry();
    
private:

    virtual void Entry( WlanContextImpl& aCtxImpl );
    virtual void Exit( WlanContextImpl& aCtxImpl );

    /**
    * Connect (authenticate and associate) to a WLAN network
    *
    * @since S60 3.1
    * @param aCtxImpl statemachine context
    * @param aSSID Name of the network
    * @param aBSSID BSSID of the access point
    * @param aAuthAlgorithmNbr Authentication algorithm number to be used
    * @param aEncryptionStatus Used encryption level
    * @param aIsInfra ETrue when connecting to infrastructure network
    * @param aScanResponseFrameBodyLength length of the scan response frame body
    * @param aScanResponseFrameBody scan response frame body
    * @param aIeData The IE(s) to be included into the (re-)association request.
    *              NULL, if none to be included
    * @param aIeDataLength Length of the IE(s) to be included into the 
    *                    (re-)association request
    * @return ETrue if a state transition occurred
    *         EFalse otherwise
    */
    virtual TBool Connect(
        WlanContextImpl& aCtxImpl,
        const TSSID& aSSID,                 
        const TMacAddress& aBSSID,          
        TUint16 aAuthAlgorithmNbr,      
        TEncryptionStatus aEncryptionStatus,
        TBool aIsInfra,
        TUint16 aScanResponseFrameBodyLength,
        const TUint8* aScanResponseFrameBody,
        const TUint8* aIeData,
        TUint16 aIeDataLength );

    virtual TBool StartIBSS(
        WlanContextImpl& aCtxImpl,
        const TSSID& aSSID,                 
        TUint32 aBeaconInterval,            
        TUint32 aAtim,                      
        TUint32 aChannel,                   
        TEncryptionStatus aEncryptionStatus); 
    
    virtual TBool Disconnect( WlanContextImpl& aCtxImpl );

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
     * From MWlanUserEvent
     * Make system ready for unloading
     *
     * @since S60 3.1
     */
    virtual void FinitSystem( WlanContextImpl& aCtxImpl );

    /**
     * From MWlanUserEvent
     * Add/set (or replace) a broadcast WEP key also sets it as a PTK
     *
     * @since S60 3.1
     * @param aCtxImpl statemachine context
     * @param aKeyIndex Index of the key in the default key table 
     * @param aKeyLength Length of the key in BYTES
     * @param aKey The WEP key
     * @param aMac MAC address associated with the key
     * @return ETrue if state transition occurred, EFalse otherwise
     */
    virtual TBool AddBroadcastWepKey(
        WlanContextImpl& aCtxImpl,
        TUint32 aKeyIndex,             
        TBool aUseAsDefaulKey,                
        TUint32 aKeyLength,                      
        const TUint8 aKey[KMaxWEPKeyLength],
        const TMacAddress& aMac );   

    virtual TBool AddUnicastWepKey(
        WlanContextImpl& aCtxImpl,
        const TMacAddress& aMacAddr,
        TUint32 aKeyLength,                      
        const TUint8 aKey[KMaxWEPKeyLength]);

    /**
    * Returns the states name
    * @param aLength (OUT) length of the name of the state
    * @return name of the state
    */
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
    * @since S60 3.1
    * @param aCtxImpl global statemachine context
    */
    void OnScanEvent( WlanContextImpl& aCtxImpl );

    /**
    * ?member_description.
    * @since S60 3.1
    * @param aCtxImpl global statemachine context
    */
    void OnConnectEvent( WlanContextImpl& aCtxImpl );

    /**
    * ?member_description.
    * @since S60 3.1
    * @param aCtxImpl global statemachine context
    */
    void OnConnectIbssEvent( WlanContextImpl& aCtxImpl );

    /**
    * ?member_description.
    * @since S60 3.1
    * @param aCtxImpl global statemachine context
    */
    void OnDisconnectEvent( WlanContextImpl& aCtxImpl );

    /**
     * ?description
     *
     * @since S60 3.1
     * @param aCtxImpl global statemachine context
     */
    void OnReleaseEvent( WlanContextImpl& aCtxImpl );

    /**
    * ?member_description.
    * @since S60 3.1
    * @param aCtxImpl global statemachine context
    */
    void WriteSleepModeMib( WlanContextImpl& aCtxImpl );

    void GenerateRandomBssIDForIbss( WlanContextImpl& aCtxImpl ) const;

    void CompleteOid( WlanContextImpl& aCtxImpl );

    /**
     * Indicates scan completion to management client if necessary
     *
     * @since S60 3.2
     * @param aCtxImpl global statemachine context
     */
    void IndicateScanCompletion( WlanContextImpl& aCtxImpl );

    // Prohibit copy constructor 
    WlanDot11Idle( const WlanDot11Idle& );
    // Prohibit assigment operator 
    WlanDot11Idle& operator= ( const WlanDot11Idle& );

private:   // Data

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
    /** state name */
    static const TInt8  iName[];
#endif 

    /** completion code for user request */
    TInt                iCompletionCode;
    
    static const TUint  KCompleteUponEntry               = (1 << 0);
    /** indicate scan completion to management client upon state entry */
    static const TUint  KIndicateScanCompletionUponEntry = (1 << 1);

    TUint               iEventMask;
    };

#endif      // WLANDOT11IDLE_H
