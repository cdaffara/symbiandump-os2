/*
* Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Handles sending management commands to drivers.
*
*/

/*
* %version: 32 %
*/

#ifndef WLANMGMTCOMMANDHANDLER_H
#define WLANMGMTCOMMANDHANDLER_H

#include "RWlanLogicalChannel.h"
#include "umacoidmsg.h"
#include "core_types.h"

const SChannels KWlanChannels802p11bgAll = {
    0x01,
    { 0xFF, 0x1F },
    { 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0
    } };

/**
 * Callback interface
 */
class MWlanMgmtCommandCallback
    {
    public:

        virtual void OnRequestComplete( TInt status ) = 0;
    };

/**
 * CWlanMgmtCommandHandler transfers commands from WlanEngine to
 * LDD (Logical Device Driver).
 * CWlanMgmtCommandHandler implements FW (Firmware) dependent functions.
 * @since S60 v.3.0
 * @lib wlmserversrv.lib
 */
NONSHARABLE_CLASS( CWlanMgmtCommandHandler ) :
    public CActive
    {
public: // Constructors and destructor
  
    /**
     * Static constructor.
     * @param aIndPtr Interface to send indications.
     * @return Pointer to created object.
     */
    static CWlanMgmtCommandHandler* NewL(
        RWlanLogicalChannel& aChannel,
        MWlanMgmtCommandCallback& aClient );

    /**
     * Destructor.
     */
    virtual ~CWlanMgmtCommandHandler();

    /**
     * Start an own IBSS network if there doesn't already exist 
     *           a network that has the same name.
     * @param aSSID              Name of the network.
     * @param aBeaconInterval    Beacon period in TUs (kusec).
     * @param aChannel           Used channel (1-11). 
     *                           Has to be leagal at the current region.
     * @param aEncryptionStatus  Used encryption level.
     */
    void StartIBSS(
        const TSSID& aSSID,
        TUint32 aBeaconInterval,
        TUint32 aChannel,
        TEncryptionStatus aEncryptionStatus );

    /**
     * Scan all available networks. WLAN driver decides is 
     *           background scan or foreground scan used.
     * @param aMode           Passive or active.
     * @param aSSID           Network that APs are scanned. Can be broadcast
     *                        SSID.
     * @param aScanRate       Rate that is used in active scanning.
     * @param aStatus         (IN/OUT)Request status. Based on active object
     *                        framework.
     * @param aChannels       Defines the channels to scan.
     * @param aMinChannelTime Minimum time to listen for beacons/probe responses on a channel.
     * @param aMaxChannelTime Maximum time to listen for beacons/probe responses on a channel.    
     * @param aIsSplitScan    Whether the scan should be performed as a split-scan.     
     */
    void Scan(
        TScanMode aMode,
        const TSSID& aSSID,
        TRate aScanRate,
        const SChannels& aChannels,
        TUint32 aMinChannelTime, 
        TUint32 aMaxChannelTime,
        TBool aIsSplitScan );

    /**
     * Request an ongoing scan to be stopped.
     */
    void StopScan();

    /**
     * Disconnect STA from current network.
     */
    void Disconnect();

    /**
     * Set WLAN power saving mode in infrastructure networks.
     * @param aMode The power save mode to be used.
     * @param aDisableDynamicPs Disable dynamic power save management.
     * @param aWakeUpModeLight Wake-up mode in Light PS mode.
     * @param aListenIntervalLight Wake-up interval in Light PS mode.
     * @param aWakeUpModeDeep Wake-up mode in Deep PS mode.
     * @param aListenIntervalDeep Wake-up interval in Deep PS mode.
     */
    void SetPowerMode(
        TPowerMode aMode,
        TBool aDisableDynamicPs,
        TWlanWakeUpInterval aWakeUpModeLight,
        TUint8 aListenIntervalLight,
        TWlanWakeUpInterval aWakeUpModeDeep,
        TUint8 aListenIntervalDeep );
    
    /**
     * Set level for RCPI trigger.
     * @param aRCPITrigger   RCPI level.
     */
    void SetRCPITriggerLevel( TUint32 aRCPITrigger );

    /**
     * Set transmission power level. This has to be 
     * leagal at the current region.
     * @param aLevel Transmission power level in dBm.
     */
    void SetTxPowerLevel( TUint32 aLevel );

    /**
     * Initialise WLAN.
     * @param aRTSThreshold      Limit for packet size when to use RTS/CTS
     *                           protocol.
     * @param aMaxTxMSDULifetime Max. time to (re-)send whole MSDU packet.
     *                           (In TUs.)
     * @param aQoSNullFrameEntryTimeout Defines the time period in microseconds which is used
     *                                  to evaluate whether QoS NULL data frame sending should be started.
     * @param aQosNullFrameEntryTxCount Defines how many Voice AC packets must be sent during the time
     *                                  period defined in aQoSNullFrameEntryTimeout before QoS NULL
     *                                  data frame sending is started.
     * @param aQoSNullFrameInterval Defines how often a QoS NULL data frame is sent.
     * @param aQoSNullFrameExitTimeout Defines how soon after the last Voice AC packet
     *                                 QoS NULL data frame sending is stopped.
     * @param aKeepAliveInterval Defines how often NULL data frames are sent
     *                           if there are no other frames to send.
     * @param aSpRcpiTarget Defines the signal predictor algorithm "target" RCPI value for roam indication.
     * @param aSpTimeTarget Defines the signal predictor algorithm "target" time for roam indication
     *                      (in microseconds).
     * @param aSpMinIndicationInterval Defines the minimum time interval for consecutive roam
     *                                 indications from the signal predictor algorithm (in microseconds).
     * @param aEnabledFeatures Bitmask of enabled features.
     */
    void Configure(
        TUint16 aRTSThreshold, 
        TUint32 aMaxTxMSDULifetime,
        TUint32 aQoSNullFrameEntryTimeout,
        TUint32 aQosNullFrameEntryTxCount,
        TUint32 aQoSNullFrameInterval,
        TUint32 aQoSNullFrameExitTimeout,
        TUint32 aKeepAliveInterval,
        TUint32 aSpRcpiTarget,
        TUint32 aSpTimeTarget,
        TUint32 aSpMinIndicationInterval,
        TUint32 aEnabledFeatures );

    /**
     * GetLastRCPI
     * @param aRCPI  (OUT) Current RCPI.
     */
    void GetLastRCPI( TUint32& aRCPI );

    /**
     * ConfigureMulticastGroup
     * @param aJoinGroup is set true_t if adding a multicast address
     *        otherwise the address will be removed.
     * @param aMulticastAddr contains the MAC address to add/remove
     */
    void ConfigureMulticastGroup( 
        TBool aJoinGroup,
        const TMacAddress& aMulticastAddr );
    
    /**
     * Set the parameters related to BSS lost indication.
     * @param aBssLostCount The number of consecutive beacons that can be lost
     *        before BSS lost is indicated.
     * @param aFailedTxCount The number of consecutive transmissions that can fail
     *        before BSS lost is indicated.
     */    
    void SetBssLostParameters(
        TUint32 aBssLostCount,
        TUint8 aFailedTxCount );    
    
    /**
     * Set the parameters related to tx rate adaptation algorithm.
     * @param aMinStepUpCheckpoint Minimum and initial rate increase checkpoint in units of frames.
     * @param aMaxStepUpCheckpoint Maximum rate increase checkpoint in units of frames.
     * @param aStepUpCheckpointFactor Rate increase checkpoint is multiplied with this
     *        value if sending of a probe frame fails.
     * @param aStepDownCheckpoint After this many frames the need to decrease the rate is checked.
     * @param aMinStepUpThreshold Minimum and initial rate increase threshold percentage.
     * @param aMaxStepUpThreshold Maximum rate increase threshold percentage value.
     * @param aStepUpThresholdIncrement Rate increase threshold is incremented by this
     *        value if sending of a probe frame fails.
     * @param aStepDownThreshold Rate decrease threshold percentage.
     * @param aDisableProbeHandling If EFalse, the rate adaptation algorithm handles the first frame 
     *        transmitted after a rate increase in a special way.
     */
    void SetTxRateAdaptationParameters(
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
     * Set the parameters related to power mode management.
     * @param aActiveToLightTimeout Timeout for Active->Light transition.
     * @param aActiveToLightThreshold Frame count threshold for Active->Light transition.
     * @param aLightToActiveTimeout Timeout for Light->Active transition.
     * @param aLightToActiveThreshold Frame count threshold for Light->Active transition.
     * @param aLightToDeepTimeout Timeout for Light->Deep transition.
     * @param aLightToDeepThreshold Frame count threshold for Light->Deep transition.
     * @param aUapsdRxFrameLengthThreshold Rx frame lenght threshold in U-APSD for Best Effort.
     */    
    void SetPowerModeMgmtParameters(
        TUint32 aActiveToLightTimeout,
        TUint16 aActiveToLightThreshold,
        TUint32 aLightToActiveTimeout,
        TUint16 aLightToActiveThreshold,
        TUint32 aLightToDeepTimeout,
        TUint16 aLightToDeepThreshold,
        TUint16 aUapsdRxFrameLengthThreshold );

    /**
     * Set the parameters related to power mode management.
     * @param aRatePolicies TX rate policies to set.
     * @param aMcsPolicies MCS policies to set.
     * @param aMappings Mappings between Access Classes and TX rate policies.
     * @param aInitialRates Initial rates for the policies.
     * @param aAutoRatePolicies TX auto rate policies to set.
     */
    void SetTxRatePolicies(
        const TTxRatePolicy& aRatePolicies,        
        const THtMcsPolicy& aMcsPolicies,
        const TQueue2RateClass& aMappings,
        const TInitialMaxTxRate4RateClass& aInitialRates,
        const TTxAutoRatePolicy& aAutoRatePolicies );

    /**
     * Get packet statistics for the current connection.
     * @param aStatistics Packet statistics for the current connection.
     */
    void GetPacketStatistics(
        TStatisticsResponse& aStatistics );

    /**
     * Set the U-APSD settings.
     * @param aMaxServicePeriodLength The maximum number of frames to send during a service period.
     * @param aUapsdEnabledForVoice Whether U-APSD is trigger and delivery-enabled for Voice.
     * @param aUapsdEnabledForVideo Whether U-APSD is trigger and delivery-enabled for Video.
     * @param aUapsdEnabledForBestEffort Whether U-APSD is trigger and delivery-enabled for BestEffort.
     * @param aUapsdEnabledForBackground Whether U-APSD is trigger and delivery-enabled for Background.     
     */
    void SetUapsdSettings(
        TMaxServicePeriodLength aMaxServicePeriodLength,
        TBool aUapsdEnabledForVoice,
        TBool aUapsdEnabledForVideo,
        TBool aUapsdEnabledForBestEffort,
        TBool aUapsdEnabledForBackground );

    /**
     * Set the RCPI trigger level.
     * @param aTriggerLevel The RCPI trigger level.
     */
    void SetRcpiTriggerLevel( 
        TUint8 aTriggerLevel );

    /**
     * Set the power save settings.
     * @param aStayInUapsdPsModeForVoice Whether the terminal stays in U-APSD power save when using Voice.
     * @param aStayInUapsdPsModeForVideo Whether the terminal stays in U-APSD power save when using Video.
     * @param aStayInUapsdPsModeForBestEffort Whether the terminal stays in U-APSD power save when using BestEffort.
     * @param aStayInUapsdPsModeForBackground Whether the terminal stays in U-APSD power save when using Background.
     * @param aStayInLegacyPsModeForVoice Whether the terminal stays in legacy power save when using Voice.
     * @param aStayInLegacyPsModeForVideo Whether the terminal stays in legacy power save when using Video.
     * @param aStayInLegacyPsModeForBestEffort Whether the terminal stays in legacy power save when using BestEffort.
     * @param aStayInLegacyPsModeForBackground Whether the terminal stays in legacy power save when using Background.
     */
    void SetPowerSaveSettings(
        TBool aStayInUapsdPsModeForVoice,
        TBool aStayInUapsdPsModeForVideo,
        TBool aStayInUapsdPsModeForBestEffort,
        TBool aStayInUapsdPsModeForBackground,
        TBool aStayInLegacyPsModeForVoice,
        TBool aStayInLegacyPsModeForVideo,
        TBool aStayInLegacyPsModeForBestEffort,
        TBool aStayInLegacyPsModeForBackground );       

    /**
     * Set parameters of a Tx queue.
     * @param aQueueId Queue to be configured.
     * @param aMediumTime Medium time to be used.
     * @param aMaxTxMSDULifetime Maximum Transmit MSDU Lifetime to be used.
     */
    void SetTxQueueParameters(
        TQueueId aQueueId,
        TUint16 aMediumTime,
        TUint32 aMaxTxMSDULifetime );

    /**
     * Set block ACK usage per traffic stream.
     * @param aTxUsage Bitmap of TIDs where TX block ACK is allowed.
     * @param aRxUsage Bitmap of TIDs where RX block ACK is allowed.
     */
    void SetBlockAckUsage(
        TUint8 aTxUsage,
        TUint8 aRxUsage );

    /**
     * Set the SNAP header of frames to receive. 
     * @param aHeader SNAP header of frames to receive..
     */
    void SetSnapHeaderFilter(
        TSnapHeader aHeader );

    /**
     * Disable user data (802.1x: block data during 
     *           authentication).
     */
    void DisableUserData();

    /**
     * Enable user data (802.1x: pass data through after
     *           succesfull authentication). Default value is to 
     *           enable user data.
     */
    void EnableUserData();

    /**
     * Add a cipher key.
     * @param aCipherSuite The cipher suite of the key.
     * @param aKeyIndex    Index of the key.
     * @param aLength      Length of the cipher key.
     * @param aData        Cipher key.        
     * @param aMacAddr     Defines the MAC address the key is used for.
     * @param aUseAsDefault in case of wep, whether the key is used as default cipher key
     */
    void AddCipherKey(
        TWlanCipherSuite aCipherSuite,
        TUint8 aKeyIndex,
        TUint32 aLength,
        const TUint8* aData,
        const TMacAddress& aMacAddr,
        TBool aUseAsDefault );

    /**
     * Connect (authenticate and associate) to a BSS.
     * @param aSSID                        Name of the network.
     * @param aBSSID                       BSSID of the access point.
     * @param aAuthAlgorithm               Authentication algorithm number.
     * @param aEncryptionStatus            Used encryption level.
     * @param aPairwiseKeyType             Pairwise cipher key type to be used.
     * @param aIsInfra                     The BSS type.
     * @param aIeDataLength                The IE data lenght.
     * @param aIeData                      The IE data.
     * @param aScanFrameLength             Length of beacon/probe response frame.
     * @param aScanFrame                   Pointer to beacon/probe response frame.
     * @param aIsPairwiseKeyInvalidated    Whether the pairwise key should be invalidated.
     * @param aIsGroupKeyInvalidated       Whether the group key should be invalidated
     * @param aIsRadioMeasurementSupported Whether the radio measurement is supported
     * @param aPairwiseKey                 Pairwise key to set before association.
     */
    void Connect(
        const TSSID& aSSID,                 
        const TMacAddress& aBSSID,          
        TUint16 aAuthAlgorithm,      
        TEncryptionStatus aEncryptionStatus,
        TWlanCipherSuite aPairwiseKeyType,
        TBool aIsInfra,        
        TUint32 aIeDataLength,
        const TUint8* aIeData,        
        TUint32 aScanFrameLength,
        const TUint8* aScanFrame,
        TBool aIsPairwiseKeyInvalidated,
        TBool aIsGroupKeyInvalidated,
        TBool aIsRadioMeasurementSupported,
        const TPairwiseKeyData& aPairwiseKey );

protected: // From CActive

    /**
     * DoCancel
     */
    void DoCancel();

    /**
     * RunL
     */
    void RunL();

private:    // Construction methods

    /**
     * C++ default constructor.
     * @param aIndPtr Interface to send indications.
     */
    CWlanMgmtCommandHandler(
        RWlanLogicalChannel& aChannel,
        MWlanMgmtCommandCallback& aClient );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

private:    // Members

    /** 
     * Callback interface to complete the requested commands
     */
    MWlanMgmtCommandCallback& iClient;

    /**
     * Interface to access LDD 
     */
    RWlanLogicalChannel& iChannel;

    /**
     * Package buffer for output parameter(s).
     */
    TPtr8 iBuffer;

    };

#endif // WLANMGMTCOMMANDHANDLER_H
