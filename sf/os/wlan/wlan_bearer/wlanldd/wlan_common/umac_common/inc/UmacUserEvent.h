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
* Description:   Declaration of the MWlanUserEvent class
*
*/

/*
* %version: 34 %
*/

#ifndef MWLANUSEREVENT_H
#define MWLANUSEREVENT_H

#include "umacoidmsg.h"

class WlanContextImpl;
class TDataBuffer;
struct TMacAddress;

/**
*  Interface class for events originating from WLAN mgmt client
*/
class MWlanUserEvent
    {

public:
    
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
        TUint16 aIeDataLength ) = 0;

    /**
    * Add TKIP key
    * @param aCtxImpl statemachine context
    * @param aData data blob that holds TKIP parameters
    * @param aLength length of the data blob
    * @param aKeyIndex value for key id field of WEP IV
    * @param aMacAddr MAC address of the peer station
    * @return ETrue if state transition occurred, EFalse otherwise
    */
    virtual TBool AddTkIPKey( 
        WlanContextImpl& aCtxImpl,
        const TUint8* aData, 
        TUint32 aLength,
        T802Dot11WepKeyId aKeyIndex,
        const TMacAddress& aMacAddr ) = 0;

    /**
    * Add multicast TKIP key.
    * @param aCtxImpl statemachine context
    * @param aKeyIndex  Value for the key id field of WEP IV.
    * @param aLength    Length of the TKIP parameters.
    * @param aData      TKIP parameters.
    * @return ETrue if state transition occurred, EFalse otherwise
    */
    virtual TBool AddMulticastTKIPKey( 
        WlanContextImpl& aCtxImpl,
        T802Dot11WepKeyId aKeyIndex,
        TUint32 aLength,
        const TUint8* aData ) = 0;

    /**
    * Add (or replace) a pairwise AES key.
    *
    * @param aCtxImpl statemachine context
    * @param aData AES parameters.
    * @param aLength Length of the AES parameters.
    * @param aMacAddr MAC address of the peer station
    * @return ETrue if state transition occurred, EFalse otherwise
    */
    virtual TBool AddAesKey( 
        WlanContextImpl& aCtxImpl,
        const TUint8* aData, 
        TUint32 aLength,
        const TMacAddress& aMacAddr ) = 0;

    /**
    * Add (or replace) an AES group key.
    *
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
        const TUint8* aData ) = 0;
    
    /**
    * Add (or replace) a multicast WAPI key.
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
        const TUint8* aData ) = 0;

    /**
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
        const TMacAddress& aMacAddr ) = 0;

    /**
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
    * any other = failure
    */
    virtual TBool StartIBSS(
        WlanContextImpl& aCtxImpl,
        const TSSID& aSSID,                 
        TUint32 aBeaconInterval,            
        TUint32 aAtim,                      
        TUint32 aChannel,                   
        TEncryptionStatus aEncryptionStatus) = 0;  

    /**
    * Scan all available networks. 
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
        TBool aSplitScan ) = 0;       
    
    /**
    * Stop a previously started scan process. 
    *
    * @since S60 3.2
    * @param aCtxImpl global statemachine context
    * @return ETrue if a state transition occurred
    *         EFalse otherwise
    */
    virtual TBool StopScan( WlanContextImpl& aCtxImpl ) = 0;

    /**
    * Disconnect STA from current network.
    * @param aCtxImpl statemachine context
    * @return KErrNone command completed successfully 
    * any other = failure
    */
    virtual TBool Disconnect( WlanContextImpl& aCtxImpl ) = 0;

    /**
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
        TUint8 aListenIntervalInDeepPs ) = 0;

    /**
    * Set trigger level for RCPI trigger. 
    * @param aCtxImpl statemachine context
    * @param aRcpiTrigger RCPI trigger level
    * @return KErrNone command completed successfully 
    * any other = failure
    */
    virtual TBool SetRcpiTriggerLevel(
        WlanContextImpl& aCtxImpl,
        TUint32 aRcpiTrigger) = 0;          

    /**
    * Set transmission power level. This has to be legal at the current region.
    * @param aCtxImpl statemachine context
    * @param aLevel Transmission power level in mW.
    * @return KErrNone command completed successfully 
    * any other = failure
    */
    virtual TBool SetTxPowerLevel(
        WlanContextImpl& aCtxImpl,
        TUint32 aLevel) = 0;                

    /**
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
    * any other = failure
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
        TUint32 aSpMinIndicationInterval ) = 0;    

    /**
    * Add/set (or replace) a broadcast WEP key
    * @param aCtxImpl statemachine context
    * @param aKeyIndex Index of the key in the default key table 
    * @param aKeyLength Length of the key in BYTES
    * @param aKey The WEP key
    * @param aMac MAC address associated with the key
    * @return KErrNone command completed successfully 
    * any other = failure
    */
    virtual TBool AddBroadcastWepKey(
        WlanContextImpl& aCtxImpl,
        TUint32 aKeyIndex,             
        TBool aUseAsDefaulKey,                
        TUint32 aKeyLength,                      
        const TUint8 aKey[KMaxWEPKeyLength],
        const TMacAddress& aMac ) = 0;   

    /**
    * Add (or replace) a unicast WEP key.
    * @param aCtxImpl statemachine context
    * @param aMacAddr MAC address to which the WEP key corresponds to
    * @param aKeyLength Length of the key in BYTES
    * @param aKey The WEP key
    * @return KErrNone command completed successfully 
    * any other = failure
    */
    virtual TBool AddUnicastWepKey(
        WlanContextImpl& aCtxImpl,
        const TMacAddress& aMacAddr,
        TUint32 aKeyLength,                      
        const TUint8 aKey[KMaxWEPKeyLength]) = 0;   

    /**
    * Disable user data from/to protcol stack 
    * (802.1x: block data during authentication).
    * @param aCtxImpl statemachine context
    * @return KErrNone command completed successfully 
    * any other = failure
    */
    virtual TBool DisableUserData(
        WlanContextImpl& aCtxImpl ) = 0;

    /**
    * Enable user data from/to protcol stack 
    * (802.1x: pass data through after succesfull authentication)
    * @param aCtxImpl statemachine context
    * @return KErrNone command completed successfully 
    * any other = failure
    */
    virtual TBool EnableUserData(
        WlanContextImpl& aCtxImpl ) = 0;

    /**
    * Gets last RCPI value
    * @param aCtxImpl statemachine context
    * @return KErrNone command completed successfully 
    * any other = failure
    */
    virtual TBool GetLastRcpi(
        WlanContextImpl& aCtxImpl ) = 0;   
    
    /**
    * Adds a multicast MAC address and starts to filter (Rx) multicast 
    * traffic sent to any other MAC addresses than those that have been 
    * specified using this method
    * @param aCtxImpl statemachine context
    * @param aMacAddr The address to be added
    * @return 
    */
    virtual TBool AddMulticastAddr(
        WlanContextImpl& aCtxImpl,
        const TMacAddress& aMacAddr ) = 0;        
    
    /**
    * Removes a multicast MAC address from multicast (Rx) filtering
    * configuration. So any packet that we receive and which has been sent
    * to the multicast address in question is not accepted any more (i.e. 
    * it is filtered).
    * However, if there are no addresses left in the multicast (Rx) filtering
    * configuration after this remove, the multicast filtering is disabled
    * and all (otherwise acceptable) multicast packets are accepted again.
    * @param aCtxImpl statemachine context
    * @param aMacAddr The address to be removed
    * @return ETrue if a state change occurred in the state machine 
    *         EFalse otherwise
    */
    virtual TBool RemoveMulticastAddr(
        WlanContextImpl& aCtxImpl,
        TBool aRemoveAll,
        const TMacAddress& aMacAddr ) = 0;

    /**
    * Triggers the setting of the Tx offset on the protocol stack side for 
    * every frame type which can be transmitted
    *
    * @param aCtxImpl statemachine context
    */
    virtual void SetProtocolStackTxOffset( 
        WlanContextImpl& aCtxImpl ) const = 0;
    
    /**
    * Transmit a protocol stack frame
    * 
    * The frame to be sent needs to be in 802.3 format
    * @param aCtxImpl global state machine context
    * @param aDataBuffer meta header of the frame to be transmitted
    * @param aMore ETrue if another frame is also ready to be transmitted
    *              EFalse otherwise
    */
    virtual TBool TxData( 
        WlanContextImpl& aCtxImpl,
        TDataBuffer& aDataBuffer,
        TBool aMore ) = 0;

    /**
    * Write for management data
    * The frame to be sent needs to be in 802.3 format
    * @param aCtxImpl statemachine context
    * @param aDataBuffer meta header of the frame to be transmitted
    */
    virtual void TxMgmtData( 
        WlanContextImpl& aCtxImpl,
        TDataBuffer& aDataBuffer ) = 0;

    /**
     * System finit method.
     * Make system ready for unloading
     *
     * @since S60 3.1
     */
    virtual void FinitSystem( WlanContextImpl& aCtxImpl ) = 0;
    
    /**
     * Configures the parameters which define when BSS lost is indicated.
     * The indication is sent when either one of the specified thresholds
     * is exceeded.
     *
     * @since S60 v3.1
     * @param aCtxImpl statemachine context
     * @param aBeaconLostCount beacon lost count threshold
     * @param aFailedTxPacketCount failed Tx packet count threshold
     * @return ETrue if a state change occurred
     *         EFalse otherwise
     */
    virtual TBool ConfigureBssLost( 
        WlanContextImpl& aCtxImpl,
        TUint32 aBeaconLostCount,
        TUint8 aFailedTxPacketCount ) = 0;

    /**
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
        TBool aDisableProbeHandling ) = 0;
      
    /**
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
        const THtMcsPolicy& aHtMcsPolicy ) = 0;

    /**
    * Sets the dynamic power mode transition algorithm parameters
    *
    * @since S60 3.1
    * @param aCtxImpl statemachine context
    * @param aToLightPsTimeout time interval in microseconds after which 
    *        transition from Active mode to Light PS mode is considered
    * @param aToLightPsFrameThreshold frame count threshold used when 
    *        considering transition from Active to Light PS mode
    * @param aToActiveTimeout time interval in microseconds after which the 
    *        frame counter used when considering transition from Light PS 
    *        to Active mode is reset
    * @param aToActiveFrameThreshold frame count threshold used when 
    *        considering transition from Light PS to Active mode
    * @param aToDeepPsTimeout time interval in microseconds after which 
    *        transition from Light PS mode to Deep PS mode is considered
    * @param aToDeepPsFrameThreshold frame count threshold used when 
    *        considering transition from Light PS to Deep PS mode
    * @param aUapsdRxFrameLengthThreshold received frame 
    *        payload length (in bytes) threshold in U-APSD network for
    *        Best Effort Access Category
    * @return ETrue if a state change occurred
    *         EFalse otherwise
    */
    virtual TBool SetPowerModeManagementParameters(
        WlanContextImpl& aCtxImpl,
        TUint32 aToLightPsTimeout,
        TUint16 aToLightPsFrameThreshold,
        TUint32 aToActiveTimeout,
        TUint16 aToActiveFrameThreshold,
        TUint32 aToDeepPsTimeout,
        TUint16 aToDeepPsFrameThreshold,
        TUint16 aUapsdRxFrameLengthThreshold ) = 0;
        
    /**
    * Configures dynamic power mode management traffic override
    *
    * The settings here become effective once using the PS mode has been 
    * allowed by WLAN Mgmt Client.
    * When a setting below is ETrue, any amount of Rx or Tx traffic via
    * the AC in question won't cause a change from PS to CAM mode once PS
    * mode has been entered, and traffic via that AC won't make us to 
    * stay in CAM either.
    * Every AC has a separate setting for U-APSD and legacy PS.
    * The U-APSD setting is used if U-APSD is used for the AC in question.
    * Otherwise the corresponding legacy setting is used.
    *    
    * @since S60 3.2
    * @param aCtxImpl statemachine context
    * @param aStayInPsDespiteUapsdVoiceTraffic U-APSD Voice AC setting
    * @param aStayInPsDespiteUapsdVideoTraffic U-APSD Video AC setting
    * @param aStayInPsDespiteUapsdBestEffortTraffic U-APSD Best Effort AC 
    *                                               setting
    * @param aStayInPsDespiteUapsdBackgroundTraffic U-APSD Background AC 
    *                                               setting
    * @param aStayInPsDespiteLegacyVoiceTraffic legacy Voice AC setting
    * @param aStayInPsDespiteLegacyVideoTraffic legacy Video AC setting
    * @param aStayInPsDespiteLegacyBestEffortTraffic legacy Best Effort AC 
    *                                                setting
    * @param aStayInPsDespiteLegacyBackgroundTraffic legacy Background AC 
    *                                                setting
    * @return ETrue if a state change occurred
    *         EFalse otherwise
    */
    virtual TBool ConfigurePwrModeMgmtTrafficOverride( 
        WlanContextImpl& aCtxImpl,
        TBool aStayInPsDespiteUapsdVoiceTraffic,
        TBool aStayInPsDespiteUapsdVideoTraffic,
        TBool aStayInPsDespiteUapsdBestEffortTraffic, 
        TBool aStayInPsDespiteUapsdBackgroundTraffic,
        TBool aStayInPsDespiteLegacyVoiceTraffic,
        TBool aStayInPsDespiteLegacyVideoTraffic,
        TBool aStayInPsDespiteLegacyBestEffortTraffic,
        TBool aStayInPsDespiteLegacyBackgroundTraffic ) = 0;

    /**
     * Gets data frame Rx & Tx statistics
     *
     * @since S60 v3.2
     * @param aCtxImpl statemachine context
     * @return ETrue if a state change occurred
     *         EFalse otherwise
     */
    virtual TBool GetFrameStatistics( WlanContextImpl& aCtxImpl ) = 0;
    
    /**
     * Configures U-APSD usage
     *
     * @since S60 v3.2
     * @param aCtxImpl statemachine context
     * @param aMaxServicePeriodLength
     * @param aUapsdForVoice if ETrue the Voice AC is made both trigger and 
     *                       delivery enabled when connecting to a QoS AP 
     *                       supporting U-APSD.
     *                       Otherwise it's made neither trigger nor delivery 
     *                       enabled
     * @param aUapsdForVideo if ETrue the Video AC is made both trigger and 
     *                       delivery enabled 
     *                       when connecting to a QoS AP supporting U-APSD.
     *                       Otherwise it's made neither trigger nor delivery 
     *                       enabled
     * @param aUapsdForBestEffort if ETrue the Best Effort AC is made both 
     *                            trigger and delivery enabled when connecting
     *                            to a QoS AP supporting U-APSD.
     *                            Otherwise it's made neither trigger nor 
     *                            delivery enabled    
     * @param aUapsdForBackground if ETrue the Background AC is made both 
     *                            trigger and delivery enabled when connecting
     *                            to a QoS AP supporting U-APSD.
     *                            Otherwise it's made neither trigger nor 
     *                            delivery enabled    
     */
    virtual TBool ConfigureUapsd( 
        WlanContextImpl& aCtxImpl,
        TMaxServicePeriodLength aMaxServicePeriodLength,
        TBool aUapsdForVoice,
        TBool aUapsdForVideo,
        TBool aUapsdForBestEffort,
        TBool aUapsdForBackground ) = 0;    

    /**
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
            TUint32 aMaxTxMSDULifetime ) = 0;

    /**
     * Get our own MAC address
     *
     * @since S60 v3.1
     * @param aCtxImpl statemachine context
     * @return ETrue if a state change occurred
     *         EFalse otherwise
     */
    virtual TBool GetMacAddress(
        WlanContextImpl& aCtxImpl ) = 0;

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
        TIpv4Address aIpv4Address ) = 0;        

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
        TUint8 aRxBlockAckUsage ) = 0;
    
    /**
    * Configures Proprietary SNAP header. 
    * Valid received 802.11 Data frames containing this SNAP header
    * are accepted and forwarded to the WLAN Management Client.
    * 
    * @param aCtxImpl statemachine context
    * @param 
    * @return ETrue if a state change occurred in the state machine 
    *         EFalse otherwise
    */
    virtual TBool ConfigureProprietarySnapHdr(
        WlanContextImpl& aCtxImpl, 
        const TSnapHeader& aSnapHeader ) = 0;        
    };
    
#endif      // MWLANUSEREVENT_H
