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
* Description:   Declaration of the WlanDot11MacError class.
*
*/

/*
* %version: 18 %
*/

#ifndef WLANDOT11MACERROR_H
#define WLANDOT11MACERROR_H

#include "UmacDot11State.h"

/**
 *  dot11 state object for unrecoverable error
 *
 *  This state becomes the current state either 
 *  by external or internal reason.
 *  External reason is the WHA-Error indication and internal reason
 *  for example a memory allocation failure.
 *  In any cause system must be reset in order to recover.
 *  This is the termination point for the dot11 state fsm
 *  and the whole framework must be deallocated from the memory and
 *  reinitialised as there is no future anymore in this domain...
 *
 *  @lib wlanumac.lib
 *  @since S60 v3.1
 */
class WlanDot11MacError : public WlanDot11State
    {    
    /**  is entry method already executed or not */
    enum { KEntryExecuted   = (1 << 0) };
    /** oid to be completed or not */
    enum { KCompleteOid     = (1 << 1) };

public:
    
    /**
    * C++ default constructor.
    */
    WlanDot11MacError() : iFlags ( 0 ) {};

    /**
    * Destructor.
    */
    virtual ~WlanDot11MacError() {};
                
private:    

    // Functions from base classes

    /**
    * From WlanMacState
    * State entry method
    * @since S60 3.1
    * @param aCtxImpl global statemachine context
    */
    virtual void Entry( WlanContextImpl& aCtxImpl );

    /**
    * From WlanMacState
    * State exit method
    * @since S60 3.1
    * @param aCtxImpl global statemachine context
    */
    virtual void Exit( WlanContextImpl& aCtxImpl );

    /**
     * From MWlanUserEvent
     * Make system ready for unloading
     *
     * @since S60 3.1
     * @param aCtxImpl global statemachine context
     */
    virtual void FinitSystem( WlanContextImpl& aCtxImpl );

    /**
     * From WlanMacState
     *
     * @since S60 3.1
     * @param aCtxImpl global statemachine context
     */
    virtual void Indication( 
        WlanContextImpl& aCtxImpl, 
        WHA::TIndicationId aIndicationId,
        const WHA::UIndicationParams& aIndicationParams );

    /**
    * From MWlanUserEvent
    * Set inital WLAN specific parameters
    * @param aCtxImpl statemachine context
    * @param aRTSThreshold 
    * @param aMaxTxMSDULifetime 
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
    * @return KErrNone command completed successfully 
    *         any other = failure
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
        TUint32 aSpMinIndicationInterval );

    /**
    * From MWlanUserEvent
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

    /**
    * From MWlanUserEvent
    * Start an own IBSS network if there doesn't already exist a network
    * with that name. If does then it is joined
    * @param aCtxImpl statemachine context
    * @param aSSID name of the network
    * @param aBeaconInterval Beacon period in TUs (kusec). range:[1, 1024]
    * @param aAtim ATIM window
    * @param aChannel Used channel (1-14). 
    *        Has to be legal at the current region.
    * @param aEncryptionStatus, used encryption level
    * @return KErrNone command completed successfully 
    *         any other = failure
    */
    virtual TBool StartIBSS(
        WlanContextImpl& aCtxImpl,
        const TSSID& aSSID,                 
        TUint32 aBeaconInterval,            
        TUint32 aAtim,                      
        TUint32 aChannel,                   
        TEncryptionStatus aEncryptionStatus );

    /**
    * From MWlanUserEvent
    * Scan all available networks. 
    *
    * @since S60 3.1
    * @param aCtxImpl statemachine context
    * @param aMode passive or active scan see #TScanMode
    * @param aSSID Network that APs are scanned. Can be broadcast SSID.
    * @param aScanRate rate that is used in active scanning see #TRate
    * @param aMinChannelTime min. time to listen beacons/probe responses on 
    *                        a channel
    * @param aMaxChannelTime max. time to listen beacons/probe responses on 
    *                        a channel
    * @param aSplitScan if ETrue, use split scan
    * @return KErrNone command completed successfully 
    * any other = failure
    */
    virtual TBool Scan(
        WlanContextImpl& aCtxImpl,
        TScanMode aMode,                   
        const TSSID& aSSID,                
        TRate aScanRate,                   
        SChannels& aChannels,
        TUint32 aMinChannelTime,            
        TUint32 aMaxChannelTime,
        TBool aSplitScan );       
    
    /**
    * From MWlanUserEvent
    * Stop a previously started scan process. 
    *
    * @since S60 3.2
    * @param aCtxImpl global statemachine context
    * @return ETrue if a state transition occurred
    *         EFalse otherwise
    */
    virtual TBool StopScan( WlanContextImpl& aCtxImpl );

    /**
    * From MWlanUserEvent
    * Disconnect STA from current network.
    *
    * @since S60 3.1
    * @param aCtxImpl statemachine context
    * @return KErrNone command completed successfully 
    * any other = failure
    */
    virtual TBool Disconnect( WlanContextImpl& aCtxImpl );

    /**
    * From MWlanUserEvent
    * Set 802.11 power mgmt mode in infrastructure networks.
    * @param aCtxImpl statemachine context
    * @param aPowerMode desired power mode
    * @param aDisableDynamicPowerModeManagement If ETrue, disables the dynamic
    *        power mode management handling. Relevant only when aPowerMode
    *        is EPowerModePs
    * @param aWakeupModeInLightPs WLAN wake-up mode in Light PS mode
    * @param aListenIntervalInLightPs specifies the value of N for wake-up
    *        modes 2 and 3 in Light PS mode. 
    * @param aWakeupModeInDeepPs WLAN wake-up mode in Deep PS mode
    * @param aListenIntervalInDeepPs specifies the value of N for wake-up 
    *        modes 2 and 3 in Deep PS mode
    * @return ETrue if a state transition occurred
    *         EFalse otherwise
    */
    virtual TBool SetPowerMode(
        WlanContextImpl& aCtxImpl,
        TPowerMode aPowerMode,
        TBool aDisableDynamicPowerModeManagement,
        TWlanWakeUpInterval aWakeupModeInLightPs, 
        TUint8 aListenIntervalInLightPs,
        TWlanWakeUpInterval aWakeupModeInDeepPs,
        TUint8 aListenIntervalInDeepPs );

    /**
    * From MWlanUserEvent
    * Set trigger level for RCPI trigger. 
    * @param aCtxImpl statemachine context
    * @param aRcpiTrigger RCPI trigger level
    * @return KErrNone command completed successfully 
    *         any other = failure
    */
    virtual TBool SetRcpiTriggerLevel(
        WlanContextImpl& aCtxImpl,
        TUint32 aRcpiTrigger);          
    
    /**
    * From MWlanUserEvent
    * Gets last RCPI value
    * @param aCtxImpl statemachine context
    * @return KErrNone command completed successfully 
    *         any other = failure
    */
    virtual TBool GetLastRcpi(
        WlanContextImpl& aCtxImpl );   
    
    /**
     * From MWlanUserEvent
     * Add (or replace) a TKIP key
     *
     * @since S60 3.1
     * @param aCtxImpl statemachine context
     * @param aData data blob that holds TKIP parameters
     * @param aLength length of the data blob
     * @param aKeyIndex value for key id filed of WEP IV
     * @return ETrue if state transition occurred, EFalse otherwise
     */
    virtual TBool AddTkIPKey( 
        WlanContextImpl& aCtxImpl,
        const TUint8* aData, 
        TUint32 aLength,
        T802Dot11WepKeyId aKeyIndex,
        const TMacAddress& aMacAddr );

    /**
     * From MWlanUserEvent
     * Add (or replace) a multicast TKIP key.
     *
     * @since S60 3.1
     * @param aCtxImpl statemachine context
     * @param aKeyIndex Value for the key id field of WEP IV.
     * @param aLength Length of the TKIP parameters.
     * @param aData TKIP parameters.
     * @return ETrue if state transition occurred, EFalse otherwise
     */
    virtual TBool AddMulticastTKIPKey( 
        WlanContextImpl& aCtxImpl,
        T802Dot11WepKeyId aKeyIndex,
        TUint32 aLength,
        const TUint8* aData );

    /**
     * From MWlanUserEvent
     * Add (or replace) a pairwise AES key.
     *
     * @since S60 3.1
     * @param aCtxImpl statemachine context
     * @param aLength Length of the AES parameters.
     * @param aData AES parameters.
     * @return ETrue if state transition occurred, EFalse otherwise
     */
    virtual TBool AddAesKey( 
        WlanContextImpl& aCtxImpl,
        const TUint8* aData, 
        TUint32 aLength,
        const TMacAddress& aMacAddr );

    /**
     * From MWlanUserEvent
     * Add (or replace) an AES group key.
     *
     * @since S60 3.1
     * @param aCtxImpl statemachine context
     * @param aKeyIndex Value for the key id field of WEP IV.
     * @param aLength Length of the AES parameters.
     * @param aData AES parameters.
     * @return ETrue if state transition occurred, EFalse otherwise
     */
    virtual TBool AddMulticastAesKey( 
        WlanContextImpl& aCtxImpl,
        T802Dot11WepKeyId aKeyIndex,
        TUint32 aLength,
        const TUint8* aData );

    /**
     * From MWlanUserEvent
     * Add/set (or replace) a broadcast WEP key
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

    /**
     * From MWlanUserEvent
     * Add (or replace) a unicast WEP key.
     *
     * @since S60 3.1
     * @param aCtxImpl statemachine context
     * @param aMacAddr MAC address to which the WEP key corresponds to
     * @param aKeyLength Length of the key in BYTES
     * @param aKey The WEP key
     * @return ETrue if state transition occurred, EFalse otherwise
     */
    virtual TBool AddUnicastWepKey(
        WlanContextImpl& aCtxImpl,
        const TMacAddress& aMacAddr,
        TUint32 aKeyLength,                      
        const TUint8 aKey[KMaxWEPKeyLength]);

    /**
    * From MWlanUserEvent
    * Add (or replace) a multicast WAPI key.
    *
    * @param aCtxImpl statemachine context
    * @param aKeyIndex  Value for the key id field of WEP IV.
    * @param aLength    Length of the WAPI parameters.
    * @param aData      WAPI parameters.
    * @return ETrue if state transition occurred, EFalse otherwise
    */
    virtual TBool AddMulticastWapiKey( 
        WlanContextImpl& aCtxImpl,
        T802Dot11WepKeyId aKeyIndex,
        TUint32 aLength,
        const TUint8* aData );

    /**
    * From MWlanUserEvent
    * Add (or replace) a pairwise WAPI key.
    *
    * @param aCtxImpl statemachine context
    * @param aData data blob that holds WAPI parameters
    * @param aLength length of the data blob
    * @param aKeyIndex value for key id field of WEP IV
    * @param aMacAddr MAC address of the peer station
    * @return ETrue if state transition occurred, EFalse otherwise
    */
    virtual TBool AddUnicastWapiKey( 
        WlanContextImpl& aCtxImpl,
        const TUint8* aData, 
        TUint32 aLength,
        T802Dot11WepKeyId aKeyIndex,
        const TMacAddress& aMacAddr );

    /**
     * From MWlanUserEvent
     * Set transmission power level. 
     * This has to be legal at the current region.
     *
     * @since S60 3.1
     * @param aCtxImpl statemachine context
     * @param aLevel Transmission power level in mW.
     * @return ETrue if state transition occurred, EFalse otherwise
     */
    virtual TBool SetTxPowerLevel(
        WlanContextImpl& aCtxImpl,
        TUint32 aLevel);    

    /**
    * From MWlanUserEvent
    * Adds a multicast MAC address and starts to filter (Rx) multicast 
    * traffic sent to any other MAC addresses than those that have been 
    * specified using this method
    *
    * @since S60 3.1
    * @param aCtxImpl statemachine context
    * @param aMacAddr The address to be added
    * @return 
    */
    virtual TBool AddMulticastAddr(
        WlanContextImpl& aCtxImpl,
        const TMacAddress& aMacAddr );        
    
    /**
    * From MWlanUserEvent
    * Removes a multicast MAC address from multicast (Rx) filtering
    * configuration. So any packet that we receive and which has been sent
    * to the multicast address in question is not accepted any more (i.e. 
    * it is filtered).
    * However, if there are no addresses left in the multicast (Rx) filtering
    * configuration after this remove, the multicast filtering is disabled
    * and all (otherwise acceptable) multicast packets are accepted again.
    *
    * @since S60 3.1
    * @param aCtxImpl statemachine context
    * @param aMacAddr The address to be removed
    * @return ETrue if a state change occurred in the state machine 
    *         EFalse otherwise
    */
    virtual TBool RemoveMulticastAddr(
        WlanContextImpl& aCtxImpl,
        TBool aRemoveAll,
        const TMacAddress& aMacAddr );

    /**
     * From MWlanUserEvent
     * Configures the parameters which define when BSS lost is indicated.
     * The indication is sent when either one of the specified thresholds
     * is exceeded.
     *
     * @since S60 v3.1
     * @param aCtxImpl statemachine context
     * @param aBeaconLostCount beacon lost count threshold
     * @aFailedTxPacketCount failed Tx packet count threshold
     * @return ETrue if a state change occurred
     *         EFalse otherwise
     */
    virtual TBool ConfigureBssLost( 
        WlanContextImpl& aCtxImpl,
        TUint32 aBeaconLostCount,
        TUint8 aFailedTxPacketCount );

    /**
    * From MWlanUserEvent
    * Sets the Tx rate adaptation algorithm parameters
    *
    * @since S60 3.1
    * @param aCtxImpl statemachine context
    * @param aMinStepUpCheckpoint minimum and initial rate increase 
    *        checkpoint in units of frames
    *        Range: [aStepDownCheckpoint,aMaxStepUpCheckpoint]
    * @param aMaxStepUpCheckpoint maximum rate increase checkpoint in units 
    *        of frames
    *        Range: [aStepDownCheckpoint,UCHAR_MAX]
    * @param aStepUpCheckpointFactor StepUpCheckpoint is multiplied with this
    *        value if sending a probe frame fails
    *        Range: [1,aMaxStepUpCheckpoint]
    * @param aStepDownCheckpoint after this many frames the need to decrease
    *        the rate is checked
    *        Range: [2,UCHAR_MAX]
    * @param aMinStepUpThreshold minimum and initial StepUpThreshold 
    *        percentage
    *        Range: [1,aMaxStepUpThreshold]
    * @param aMaxStepUpThreshold maximum StepUpThreshold percentage
    *        Range: [1,100]
    * @param aStepUpThresholdIncrement StepUpThreshold is incremented by this
    *        value if sending a probe frame fails
    *        Range: [0,aMaxStepUpThreshold]
    * @param aStepDownThreshold if the percentage of frames which failed to
    *        be transmitted at the originally requested rate is at least 
    *        aStepDownThreshold at the aStepDownCheckpoint, the rate will
    *        be decreased
    *        Range: [1,100]
    * @param aDisableProbeHandling if EFalse, the rate adaptation algorithm
    *        handles the first frame transmitted after a rate increase in a
    *        special way. Otherwise the special handling is disabled
    * @return ETrue if a state change occurred
    *         EFalse otherwise
    */
    virtual TBool SetTxRateAdaptParams( 
        WlanContextImpl& aCtxImpl,
        TUint8 aMinStepUpCheckpoint,
        TUint8 aMaxStepUpCheckpoint,
        TUint8 aStepUpCheckpointFactor,
        TUint8 aStepDownCheckpoint,
        TUint8 aMinStepUpThreshold,
        TUint8 aMaxStepUpThreshold,
        TUint8 aStepUpThresholdIncrement,
        TUint8 aStepDownThreshold,
        TBool aDisableProbeHandling );
    
    /**
     * From MWlanUserEvent
     * Configures Tx rate policy objects, sets the policy object to use for 
     * every Tx Queue / QoS AC, and also sets the Initial Max Tx Rate to be 
     * used for the configured policy objects
     *
     * @since S60 v3.1
     * @param aCtxImpl statemachine context
     * @param aRatePolicy rate policy (policy objects)
     * @param aQueue2RateClass Tx queue (AC) to rate policy object mapping
     * @param aInitialMaxTxRate4RateClass initial max Tx rate for the
     *        policy objects
     * @param aAutoRatePolicy auto rate policy
     * @param aHtMcsPolicy HT MCS policy 
     * @return ETrue if a state change occurred
     *         EFalse otherwise
     */
    virtual TBool ConfigureTxRatePolicies( 
        WlanContextImpl& aCtxImpl,
        const TTxRatePolicy& aRatePolicy,
        const TQueue2RateClass& aQueue2RateClass,
        const TInitialMaxTxRate4RateClass& aInitialMaxTxRate4RateClass,
        const TTxAutoRatePolicy& aAutoRatePolicy,
        const THtMcsPolicy& aHtMcsPolicy );

    /**
     * From MWlanUserEvent
     * Gets data frame Rx & Tx statistics
     *
     * @since S60 v3.2
     * @param aCtxImpl statemachine context
     * @return ETrue if a state change occurred
     *         EFalse otherwise
     */
    virtual TBool GetFrameStatistics( WlanContextImpl& aCtxImpl );
    
    /**
     * From MWlanUserEvent
     * Reconfigures the specified Tx queue if necessary
     *
     * @since S60 v3.2
     * @param aCtxImpl statemachine context
     * @param aQueueId ID of the queue to reconfigure
     * @param aMediumTime The amount of time the queue is allowed to access 
     *                    the WLAN air interface.
     * @param aMaxTxMSDULifetime Maximum Transmit MSDU Lifetime to be used 
     *                           for the specified queue.
     * @return ETrue if a state change occurred
     *         EFalse otherwise
     */
    virtual TBool ConfigureTxQueueIfNecessary( 
            WlanContextImpl& aCtxImpl,
            TQueueId aQueueId,
            TUint16 aMediumTime,
            TUint32 aMaxTxMSDULifetime );

    /**
     * From MWlanUserEvent
     * Get our own MAC address
     *
     * @since S60 v3.1
     * @param aCtxImpl statemachine context
     * @return ETrue if a state change occurred
     *         EFalse otherwise
     */
    virtual TBool GetMacAddress( WlanContextImpl& aCtxImpl );
    
    /**
    * Configures ARP IP address filtering
    * 
    * @param aCtxImpl statemachine context
    * @param aEnableFiltering If ETrue, filtering is enabled
    *                         If EFalse, filtering is disabled
    * @param aIpv4Address If the target IP Address in a received ARP request 
    *        doesn't match this address, the packet shall be discarded
    *        on the lower layers.
    *        Relevant only when enabling filtering.
    * @return ETrue if a state change occurred in the state machine 
    *         EFalse otherwise
    */
    virtual TBool ConfigureArpIpAddressFiltering(
        WlanContextImpl& aCtxImpl,
        TBool aEnableFiltering,
        TIpv4Address aIpv4Address );

    /**
    * Configures HT Block Ack use
    * 
    * @param aCtxImpl statemachine context
    * @param aTxBlockAckUsage Bit map defining Block Ack use in Tx direction
    * @param aRxBlockAckUsage Bit map defining Block Ack use in Rx direction
    * @return ETrue if a state change occurred in the state machine 
    *         EFalse otherwise
    */
    virtual TBool ConfigureHtBlockAck(
        WlanContextImpl& aCtxImpl, 
        TUint8 aTxBlockAckUsage,
        TUint8 aRxBlockAckUsage );        

    /**
     * From MWlanUserEvent
     * Returns the states name
     *
     * @since S60 3.1
     * @param aLength (OUT) length of the name of the state
     * @return name of the state
     */
#ifndef NDEBUG 
    virtual const TInt8* GetStateName( TUint8& aLength ) const;
#endif // !NDEBUG 
               
private:

    /**
    * Completes WLAN Mgmt Client command
    * 
    * @param aCtxImpl Global statemachine context
    * @param aStatus Status code to return to WLAN Mgmt Client     
    * @return ETrue if a state change occurred in the global state machine 
    *         EFalse otherwise
    */
    TBool CompleteMgmtCommand(
        WlanContextImpl& aCtxImpl,
        TInt aStatus );
    
    // Prohibit copy constructor 
    WlanDot11MacError( const WlanDot11MacError& );
    // Prohibit assigment operator
    WlanDot11MacError& operator= ( const WlanDot11MacError& );         

private:   // Data
    
    /** flags */
    TUint32 iFlags;

#ifndef NDEBUG 
    /** name of the state */
    static const TInt8  iName[];        
#endif // !NDEBUG 
    };

#endif      // WLANDOT11MACERROR_H
