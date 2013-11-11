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
* Description:   Declaration of the WlanDot11State class
*
*/

/*
* %version: 59 %
*/

#ifndef WLANDOT11STATE_H
#define WLANDOT11STATE_H

#include "umacinternaldefinitions.h"
#include "UmacMacActionState.h"

class WlanWsaAddKey;
class WlanElementLocator;


/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib wlanumac.lib
*  @since S60 3.1
*/
class WlanDot11State : public WlanMacActionState
    {
public:

    /**
    * Destructor.
    */
    virtual ~WlanDot11State() {};

    void AddDefaultBroadcastWepKeyComplete( 
        WlanContextImpl& aCtxImpl ) const;
    
    // Functions from base classes

    /**
    * Scan all available networks. 
    * This SW module decides is background scan or foreground scan used
    * @param aCtxImpl statemachine context
    * @param aMode passive or active scan see #TScanMode
    * @param aSSID Network that APs are scanned. Can be broadcast SSID.
    * @param aScanRate Rate that is used in active scanning see #TRate
    * @param aMinChannelTime min. time to listen beacons/probe responses on 
    *                        a channel
    * @param aMaxChannelTime max. time to listen beacons/probe responses on 
    *                        a channel
    * @param aSplitScan if ETrue, use split scan
    * @return KErrNone = command completed successfully, 
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
    * Enable user data from/to protcol stack 
    * (802.1x: pass data through after succesfull authentication)
    * @param aCtxImpl statemachine context
    * @return KErrNone command completed successfully 
    */

    virtual TBool EnableUserData(
        WlanContextImpl& aCtxImpl );
    /**
    * Disable user data from/to protcol stack 
    * (802.1x: block data during authentication).
    * @param aCtxImpl global state machine context
    * @return KErrNone command completed successfully 
    * any other = failure
    */
    virtual TBool DisableUserData(
        WlanContextImpl& aCtxImpl );

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
        TBool aMore );

    virtual void TxMgmtData( 
        WlanContextImpl& aCtxImpl,
        TDataBuffer& aDataBuffer );

    // Frame receive

    virtual TAny* RequestForBuffer ( 
        WlanContextImpl& aCtxImpl,             
        TUint16 aLength );

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

    // from packet scheduler

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

    virtual void OnPacketSendComplete(
        WlanContextImpl& aCtxImpl, 
        WHA::TStatus aStatus,
        TUint32 aPacketId,
        WHA::TRate aRate,
        TUint32 aPacketQueueDelay,
        TUint32 aMediaDelay,
        TUint aTotalTxDelay,
        TUint8 aAckFailures,
        WHA::TQueueId aQueueId,
        WHA::TRate aRequestedRate,
        TBool aMulticastData );

    /**
    * Method called when Packet Scheduler's packet scheduling method 
    * should be called, as there exists a packet that is suitable for 
    * transmission.
    * NOTE: if any other Packet Scheduler method is called within this
    * context the result is undefined.
    * 
    * @param aMore ETrue if another frame is also ready to be transmitted
    *              EFalse otherwise
    */
    virtual void CallPacketSchedule( 
        WlanContextImpl& aCtxImpl,
        TBool aMore );

    /**
     * Method called when packet has been flushed (removed)
     * from packet scheduler
     *
     * @since S60 3.1
     * @param aCtxImpl global state machine context
     * @param aPacketId packet that was flushed
     * @param aMetaHeader frame meta header
     */
    virtual void OnPacketFlushEvent(
        WlanContextImpl& aCtxImpl, 
        TUint32 aPacketId,
        TDataBuffer* aMetaHeader );

    virtual void OnPacketPushPossible( WlanContextImpl& aCtxImpl );

    virtual void Indication( 
        WlanContextImpl& aCtxImpl, 
        WHA::TIndicationId aIndicationId,
        const WHA::UIndicationParams& aIndicationParams );
        
    /**
    * Configure Tx queue parameters to WHA (adaptation) layer
    * 
    * @param aCtxImpl statemachine context
    * @param aQueueId Id of the queue to configure
    * @param aCompleteManagementRequest ETrue if this request came from the
    *                                   WLAN Mgmt client and that request
    *                                   needs to be completed.
    *                                   EFalse otherwise.
    * @return ETrue if a state change occurred in the state machine 
    *         EFalse otherwise
    */
    TBool ConfigureTxQueue( 
        WlanContextImpl& aCtxImpl,
        WHA::TQueueId aQueueId,
        TBool aCompleteManagementRequest = EFalse );            

    virtual TBool ConfigureAcParams( 
        WlanContextImpl& aCtxImpl );                    
    
    /**
    * ?member_description.
    * @since Series 60 3.1
    * @param aCtxImpl global statemachine context
    */
    virtual TBool SetCtsToSelfMib( WlanContextImpl& aCtxImpl );
    
    /**
    * Informs lower layer about the dynamic HT configuration
    * of the target/current network
    * @param aCtxImpl statemachine context
    * @return ETrue if a state change occurred
    *         EFalse otherwise
    */
    virtual TBool ConfigureHtBssOperation( WlanContextImpl& aCtxImpl );

protected:

    static const TUint32 KCompleteManagementRequest = 0xACDCACDC;

    /**
    * C++ default constructor.
    */
    WlanDot11State() {};

    /**
     * Add/set (or replace) a broadcast WEP key,
     * conditionally set it as the default key and as a pairwise key 
     *
     * @since S60 3.1
     * @param aCtxImpl statemachine context
     * @param aKeyIndex Index of the key in the default key table 
     * @param aUseAsDefaulKey use as the default key
     * @param aUseAsPairwiseKey use as the pairwise key
     * @param aKeyLength Length of the key in BYTES
     * @param aKey The WEP key
     * @param aMac MAC address associated with the key
     * @return ETrue if state transition occurred, EFalse otherwise
     */
    TBool OnAddBroadcastWepKey(
        WlanContextImpl& aCtxImpl,
        TUint32 aKeyIndex,             
        TBool aUseAsDefaulKey,                
        TBool aUseAsPairwiseKey,
        TUint32 aKeyLength,                      
        const TUint8 aKey[KMaxWEPKeyLength],
        const TMacAddress& aMac );   

    /**
    * Set inital WLAN specific parameters. 
    * @param aCtxImpl statemachine context
    * @param aRTSThreshold 
    * limit for packet size when to use RTS/CTS protocol
    * @param aMaxTxMSDULifetime 
    * Max. time to (re-)send whole MSDU packet. (In TUs.)
    * @return status of the command, 

    */        
    static void OnConfigureUmacMib(
        WlanContextImpl& aCtxImpl,
        TUint16 aRTSThreshold,             
        TUint32 aMaxTxMSDULifetime);   


    /**
    * Determines if it is possible to try to connect to the target nw
    *
    * @since S60 3.2
    * @param aCtxImpl statemachine context
    * @param aScanResponseFrameBodyLength Length of the WLAN Mgmt Client 
    *        provided scan response frame body received from the target nw
    * @param aScanResponseFrameBody The WLAN Mgmt Client provided scan 
    *        response frame body received from the target nw
    * @return KErrNone if it is possible to try to connect to the target nw
    *         An error code otherwise
    */
    TInt InitNetworkConnect( 
        WlanContextImpl& aCtxImpl,
        TUint16 aScanResponseFrameBodyLength,
        const TUint8* aScanResponseFrameBody ) const;

    /**
    * Resolves correct transmit queue for frame
    *
    * @since S60 3.1
    * @param aCtxImpl statemachine context
    * @param aDot11MacHeader start of dot11 mac header
    * @return transmit queue id to use for the packet
    */
    WHA::TQueueId QueueId( 
        const WlanContextImpl& aCtxImpl,
        const TUint8* aDot11MacHeader ) const;

    /**
    * Transmit dot11-deauthenticate frame.
    *
    * @since S60 3.1
    * @param aCtxImpl global statemachine context
    * @param aReason reason code of the frame
    * @param aWaitIfIntTxBufNotFree if ETrue, the caller wants to wait for the
    *        internal Tx buffer to become free if it is not free now
    *        if EFalse, the caller doesn't want to wait for the internal Tx 
    *        buffer in case it happens to be occupied
    * @return ETrue if Packet Scheduler accepted the frame for transmit
    *         EFalse otherwise
    */
    TBool TxDeauthenticate( 
        WlanContextImpl& aCtxImpl, 
        T802Dot11ManagementReasonCode aReason,
        TBool aWaitIfIntTxBufNotFree = EFalse ) const;

    /**
    * Transmit dot11-disassociate frame.
    *
    * @since S60 3.1
    * @param aCtxImpl global statemachine context
    * @param aReason reason code of the frame
    * @param aWaitIfIntTxBufNotFree if ETrue, the caller wants to wait for the
    *        internal Tx buffer to become free if it is not free now
    *        if EFalse, the caller doesn't want to wait for the internal Tx 
    *        buffer in case it happens to be occupied
    * @return ETrue if Packet Scheduler accepted the frame for transmit
    *         EFalse otherwise
    */
    TBool TxDisassociate( 
        WlanContextImpl& aCtxImpl, 
        T802Dot11ManagementReasonCode aReason,
        TBool aWaitIfIntTxBufNotFree = EFalse ) const;

    /**
    * Creates AES pairwise key setting context
    *
    * @since Series 60 3.1
    * @param aCtxImpl global statemachine context
    * @param aWhaAddKey reference to WHA Add Key command object
    * @param aMacAddr MAC address to be associated to the key
    */
    static WHA::SAesPairwiseKey* CreateAesPtkCtx( 
        WlanContextImpl& aCtxImpl,
        WlanWsaAddKey& aWhaAddKey,
        const TUint8* aData, 
        const TMacAddress& aMacAddr );

    /**
    * Creates TKIP pairwise key setting context
    *
    * @since Series 60 3.1
    * @param aCtxImpl global statemachine context
    * @param aWhaAddKey reference to WHA Add Key command object
    * @param aMacAddr MAC address to be associated to the key
    */
    static WHA::STkipPairwiseKey* CreateTkipPtkCtx( 
        WlanContextImpl& aCtxImpl,
        WlanWsaAddKey& aWhaAddKey,
        const TUint8* aData, 
        T802Dot11WepKeyId aKeyIndex, 
        const TMacAddress& aMacAddr );

    /**
    * Creates WEP pairwise key setting context
    *
    * @since Series 60 3.1
    * @param aCtxImpl global statemachine context
    * @param aWhaAddKey reference to WHA Add Key command object
    * @param aMacAddr MAC address to be associated with the key
    * @param aKeyLength length of the key
    * @param aKey the key
    */
    static WHA::SWepPairwiseKey* CreateUnicastWepKeyCtx(
        WlanContextImpl& aCtxImpl,
        WlanWsaAddKey& aWhaAddKey,
        const TMacAddress& aMacAddr,
        TUint32 aKeyLength,                      
        const TUint8 aKey[KMaxWEPKeyLength] );

    /**
    * Creates WAPI pairwise key setting context
    *
    * @param aCtxImpl global statemachine context
    * @param aWhaAddKey reference to WHA Add Key command object
    * @param aData key data
    * @param aKeyIndex key index
    * @param aMacAddr MAC address to be associated to the key
    */
    static WHA::SWapiPairwiseKey* CreateWapiPtkCtx( 
        WlanContextImpl& aCtxImpl,
        WlanWsaAddKey& aWhaAddKey,
        const TUint8* aData, 
        T802Dot11WepKeyId aKeyIndex, 
        const TMacAddress& aMacAddr );

    /**
    * Determines the initial transmit rate for Null Data, Qos Null
    * and PS Poll templates.
    * @since Series 60 3.1
    * @param aCtxImpl Global statemachine context
    * @return Initial transmit rate
    */
    WHA::TRate InitialSpecialFrameTxRate( 
        const WlanContextImpl& aCtxImpl ) const;
        
    /**
    * Transfers a dot11 frame up to the client of the management interface
    *
    * @since Series 60 3.2
    * @param aCtxImpl global statemachine context
    * @param aFrame frame to be transferred
    * @param aLength length of the frame
    * @param aRcpi RCPI of the frame
    * @param aBuffer pointer to the beginning of the Rx buffer allocated
    *        for the frame
    */
    TBool XferDot11FrameToMgmtClient( 
        WlanContextImpl& aCtxImpl,
        const void* aFrame,
        TUint32 aLength,
        const WHA::TRcpi aRcpi,
        TUint8* aBuffer ) const;

    virtual TBool DoErrorIndication( 
        WlanContextImpl& aCtxImpl, 
        WHA::TStatus aStatus );

    virtual void OnWhaCommandResponse( 
        WlanContextImpl& aCtxImpl, 
        WHA::TCommandId aCommandId, 
        WHA::TStatus aStatus,
        const WHA::UCommandResponseParams& aCommandResponseParams,
        TUint32 aAct );

    virtual TBool OnWlanWakeUpIntervalChange( WlanContextImpl& aCtxImpl );    
    
    /**
    * Returns a WHA queue ID / WMM Access Category corresponding to the 802.1d
    * priority given as the parameter
    * @since Series 60 3.1
    * @param aPriority 802.1d priority
    */
    static WHA::TQueueId Queue( TUint8 aPriority );

    static TBool UapsdEnabledInNetwork( 
        const SRxWmmIeData& aRxWmmIE );
    
    static TBool UapsdEnabledInNetwork( 
        const SWmmParamElemData& aWmmParamElem );
    
    static void EnableQos( WlanContextImpl& aCtxImpl, 
        TBool aUapsdEnabledInNw );
    
    /**
     * Determines if U-APSD will be used for the ACs/Tx Queues.
     * This depends on whether U-APSD is supported by the AP, and if it
     * is, whether WLAN Mgmt Client has requested U-APSD to be used
     * for the AC.
     * Also freezes the dynamic power mode management traffic override 
     * settings.
     * 
     * @since S60 v3.2
     * @param aCtxImpl statemachine context
     */
    static void DetermineAcUapsdUsage( WlanContextImpl& aCtxImpl );

    /**
    * Resets the specified AC (Access Category) parameters to default values.
    * 
    * @param aCtxImpl statemachine context
    * @param aAccessCategory access category
    * @param aUseAandGvalues If ETrue uses the 802.11a/g specific
    *        default values. Otherwise uses the 802.11b default values
    */
    static void ResetAcParameters( 
        WlanContextImpl& aCtxImpl,
        WHA::TQueueId aAccessCategory,
        TBool aUseAandGvalues );

    /**
    * Resets the AC (Access Category) parameters of every AC to default values.
    * @param aCtxImpl statemachine context
    * @param aUseAandGvalues If ETrue uses the 802.11a/g specific
    *        default values. Otherwise uses the 802.11b default values
    */
    static void ResetAcParameters( 
        WlanContextImpl& aCtxImpl, 
        TBool aUseAandGvalues = EFalse );               
    
    /**
    * Checks if the AC parameters of the specified AC are valid & reasonable
    * @param aCtxImpl statemachine context
    * @param aAccessCategory access category
    */
    static TBool AcParametersValid( 
        WlanContextImpl& aCtxImpl,
        WHA::TQueueId aAccessCategory );
    
    static void ParseAcParameters( 
        WlanContextImpl& aCtxImpl, 
        const SWmmParamElemData& aWmmParamElem );
                   
    /**
     * Stores the Tx Rate Policy related information provided by wlan mgmt
     * client
     *
     * @since S60 v3.1
     * @param aCtxImpl statemachine context
     * @param aRatePolicy rate policy (policy objects)
     * @param aQueue2RateClass Tx queue (AC) to rate policy object mapping
     * @param aInitialMaxTxRate4RateClass initial max Tx rate for the
     *        policy objects
     * @param aAutoRatePolicy auto rate policy
     * @param aHtMcsPolicy HT MCS policy
     */
    static void StoreTxRatePolicyInfo( 
        WlanContextImpl& aCtxImpl,
        const TTxRatePolicy& aRatePolicy,
        const TQueue2RateClass& aQueue2RateClass,
        const TInitialMaxTxRate4RateClass& aInitialMaxTxRate4RateClass,
        const TTxAutoRatePolicy& aAutoRatePolicy,
        const THtMcsPolicy& aHtMcsPolicy );

    /**
    * Configures the Tx rate policies according to the information provided
    * by the WLAN mgmt client
    *
    * @since Series 60 3.1
    * @param aCtxImpl Global statemachine context
    * @param aCompleteMgmtRequest If ETrue, a WLAN Mgmt client request needs 
    *        to be completed
    *        If EFalse, a request doesn't need to be completed
    * @return ETrue if the operation succeeded & a state change occurred
    *         EFalse if a fatal error occurred
    */
    TBool ConfigureTxRatePolicies( 
        WlanContextImpl& aCtxImpl, 
        TBool aCompleteMgmtRequest = EFalse );
    
    /**
    * Determines whether the outgoing data frame is question is a multicast
    *
    * @since Series 60 3.2
    * @param aDataFrameHdr ptr to the start of the data MAC frame header
    * @return ETrue if the DA of the frame is a multicast address
    *         EFalse otherwise
    */
    static TBool OutgoingMulticastDataFrame( const SDataFrameHeader* aDataFrameHdr );
    
    /**
    * Updates frame statistics after completed data frame transmit attempt
    *
    * @since Series 60 3.2
    * @param aCtxImpl Global statemachine context
    * @param aAccessCategory AC/Queue via which the frame was transmitted
    * @param aStatus frame Tx status
    * @param aMulticastData ETrue if the frame was a multicast
    * @param aAckFailures The number of times the frame was transmitted 
    *                     without receiving an acknowledgement
    * @param aMediaDelay The total time the packet spent in the WLAN device
    *                    before transmission was completed.
    * @param aTotalTxDelay The time between SendPacket and SendPacketComplete
    */
    static void UpdateTxDataFrameStatistics( 
        WlanContextImpl& aCtxImpl,
        WHA::TQueueId aAccessCategory,
        WHA::TStatus aStatus,
        TBool aMulticastData, 
        TUint aAckFailures,
        TUint32 aMediaDelay,
        TUint aTotalTxDelay );

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
        const THtMcsPolicy& aHtMcsPolicy );

    /**
    * Determines if there is a meaningful diffrence between the desired 802.11
    * PS Mode wake-up settings compared to the current settings
    * @param aCtxImpl statemachine context
    * @return ETrue a meaningful difference exists
    *         EFalse otherwise
    */
    TBool DifferenceInPsModeWakeupSettings( 
        const WlanContextImpl& aCtxImpl ) const;

    /**
    * Informs lower layer about the HT capabilities
    * of the target network
    * @param aCtxImpl statemachine context
    * @return ETrue if a state change occurred
    *         EFalse otherwise
    */
    TBool ConfigureHtCapabilities(
        WlanContextImpl& aCtxImpl );

    /**
    * Resets the HT Capabilities MIB to its default value
    * @param aCtxImpl statemachine context
    */
    void ResetHtCapabilitiesMib( WlanContextImpl& aCtxImpl );
    
    /**
    * Resets the HT Block Ack Configure MIB to its default value
    * @param aCtxImpl statemachine context
    */
    void ResetHtBlockAckConfigureMib( WlanContextImpl& aCtxImpl );
    
    /**
    * Determines if HT control field is present in received
    * 802.11 management or 802.11 data frame
    * @param aCtxImpl statemachine context
    * @param aFrame received frame
    * @param aFlags flags from WHA frame receive method
    * @return ETrue if HT control field is present
    *         EFalse otherwise
    */
    static TBool HtcFieldPresent(
        WlanContextImpl& aCtxImpl,
        const TAny* aFrame,
        TUint32 aFlags );
    
private:

    /**
    * @param aCtxImpl statemachine context
    * @param aRate TRate value
    * @return ETrue mapping success, any other failure
    */ 
    static void ResolveScanRate( 
        WlanContextImpl& aCtxImpl,
        const TRate aRate, 
        WHA::TRate& aScanRate );

    static TBool NetworkCapabilityInformationMet( 
        WlanContextImpl& aCtxImpl );
    
    static TBool AreSupportedRatesMet( 
        WlanContextImpl& aCtxImpl,
        TBool aCheckAlsoExtendedRates );
    
    static TBool ProcessSingleSupportedRateElement(
        WlanContextImpl& aCtxImpl,
        const TUint8 aApRate,
        TUint32& aRateBitmask );

    /**
    * Sets the WHA::KMibArpIpAddressTable MIB
    * 
    * @param aCtxImpl statemachine context
    * @param aEnableFiltering If ETrue, filtering is enabled
    *                         If EFalse, filtering is disabled
    * @param aIpv4Address Address to set in the MIB. Relevant only when
    *        enabling filtering
    * @return ETrue if a state change occurred in the state machine 
    *         EFalse otherwise
    */
    TBool SetArpIpAddressTableMib(
        WlanContextImpl& aCtxImpl,
        TBool aEnableFiltering,
        TIpv4Address aIpv4Address );


    // Functions from base classes
    
    virtual TBool OnDot11PwrMgmtTransitRequired( 
        WlanContextImpl& aCtxImpl );

    /**
    * Indicates Consecutive Beacons Lost 
    * to WLAN Mgmt Client; if necessary
    * 
    * @since S60 5.0
    * @param aCtxImpl global statemachine context
    */
    virtual void DoConsecutiveBeaconsLostIndication( WlanContextImpl& aCtxImpl );

    virtual void DoRegainedBSSIndication( WlanContextImpl& aCtxImpl );

    virtual void DoRadarIndication( WlanContextImpl& aCtxImpl );

    virtual void DoRcpiIndication( 
         WlanContextImpl& aCtxImpl,
         WHA::TRcpi aRcpi );

    /**
    * Indicates that the WLAN device has detected problems in the power
    * save mode operation of the AP
    * 
    * @param aCtxImpl global statemachine context
    */
    virtual void DoPsModeErrorIndication( WlanContextImpl& aCtxImpl );
    
    /**
    * Add (or replace) a TKIP key
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
    *
    * Add (or replace) a multicast TKIP key.
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
    * Add (or replace) a pairwise AES key.
    *
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
        const TUint8* aData );

    /**
    * Add (or replace) a unicast WEP key.
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
    * Adds a multicast MAC address and starts to filter (Rx) multicast 
    * traffic sent to any other MAC addresses than those that have been 
    * specified using this method
    * @param aCtxImpl statemachine context
    * @param aMacAddr The address to be added
    * @return ETrue if a state change occurred in the state machine 
    *         EFalse otherwise
    */
    virtual TBool AddMulticastAddr(
        WlanContextImpl& aCtxImpl,
        const TMacAddress& aMacAddr );
        
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
        const TMacAddress& aMacAddr );

    /**
    * Set transmission power level. 
    * This has to be legal at the current region.
    * @param aCtxImpl statemachine context
    * @param aLevel Transmission power level in mW.
    * @return KErrNone command completed successfully 
    * any other = failure
    */
    virtual TBool SetTxPowerLevel(
        WlanContextImpl& aCtxImpl,
        TUint32 aLevel);    
    
    /**
    * Gets last RCPI value
    * @param aCtxImpl statemachine context
    * @return KErrNone command completed successfully 
    * any other = failure
    */
    virtual TBool GetLastRcpi(
        WlanContextImpl& aCtxImpl );       
    
    /**
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
        TUint16 aUapsdRxFrameLengthThreshold );
    
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
        TBool aStayInPsDespiteLegacyBackgroundTraffic );

    /**
     * Gets data frame Rx & Tx statistics
     *
     * @since S60 v3.2
     * @param aCtxImpl statemachine context
     * @return ETrue if a state change occurred
     *         EFalse otherwise
     */
    virtual TBool GetFrameStatistics( WlanContextImpl& aCtxImpl );

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
        TBool aUapsdForBackground );

    /**
     * Get our own MAC address
     *
     * @since S60 v3.1
     * @param aCtxImpl statemachine context
     * @return ETrue if a state change occurred
     *         EFalse otherwise
     */
    virtual TBool GetMacAddress(
        WlanContextImpl& aCtxImpl );

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
        const TSnapHeader& aSnapHeader );        

    /**
    * Sets the WHA::KMibDot11GroupAddressesTable MIB
    * 
    * @param aCtxImpl statemachine context
    * @return ETrue if a state change occurred in the state machine 
    *         EFalse otherwise
    */
    TBool SetGroupAddressesTableMib(
        WlanContextImpl& aCtxImpl );

    /**
     * Sets the beacon lost count value
     *
     * @since S60 v3.1
     * @param aCtxImpl statemachine context
     * @param aBeaconLostCount beacon lost count
     * @return ETrue if a state change occurred
     *         EFalse otherwise
     */
    TBool SetBeaconLostCountMib(
        WlanContextImpl& aCtxImpl,
        TUint32 aBeaconLostCount);    
    
    /**
     * Resorts to using only a single Tx rate policy.
     * This is done when the WLAN Mgmt client provides us with more rate 
     * classes / policies than the lower layers are able to handle
     *
     * @since S60 v3.1
     * @param aCtxImpl statemachine context
     * @param aRatePolicy the rate policy
     * @param aQueue2RateClass Tx queue to rate class mapping
     */
    void ResortToSingleTxRatePolicy(
        WlanContextImpl& aCtxImpl,
        TTxRatePolicy& aRatePolicy,
        TQueue2RateClass& aQueue2RateClass ) const;

    /**
     * Finalize the 802.11b/g non-autorate policy so that it contains only
     * rates which are also supported by the lower layers and the current
     * network 
     *
     * @since S60 v3.1
     * @param aCtxImpl statemachine context
     * @param aRatePolicy the rate policy
     * @param aRateMasks intersection of 802.11b/g rates which are:
     *        - included in the rate class AND
     *        - supported by the lower layers AND
     *        - supported by the current network
     * @param aInitialMaxTxRate4RateClass initial max Tx rate for every
     *        rate class 
     */
    void FinalizeTxRatePolicy(
        WlanContextImpl& aCtxImpl,
        TTxRatePolicy& aRatePolicy,
        TWhaRateMasks& aRateMasks,
        TInitialMaxTxRate4RateClass& aInitialMaxTxRate4RateClass ) const;
    
    /**
     * Builds a rate mask by including aRate in it if that rate has non-zero
     * Tx Attempts and is supported both by the nw and by WHA layer.
     * Additionally makes aTxAttempts zero if aRate is not supported either by
     * the nw or by WHA layer. Otherwise keeps the aTxAttempts value intact
     *
     * @since S60 v3.1
     * @param aCtxImpl statemachine context
     * @param aRate the rate
     * @param aTxAttempts nbr of Tx attempts to make with aRate
     * @param aRateMask the rate mask
     */
    void HandleRate(
        WlanContextImpl& aCtxImpl,
        WHA::TRate aRate,
        TUint8& aTxAttempts,
        WHA::TRate& aRateMask ) const;        

    /**
     * Builds a rate mask from a Rate Class and updates Tx attempts value in
     * the Rate Class when necessary (see HandleRate()).
     *
     * @since S60 v3.1
     * @param aCtxImpl statemachine context
     * @param aRateClass the rate class
     * @param aRateMask the rate mask
     */
    void HandleRates(
        WlanContextImpl& aCtxImpl,
        TTxRateClass& aRateClass,
        WHA::TRate& aRateMask ) const;
 
    /**
     * Updates rate class and rate mask to contain the rates which both the 
     * WHA layer and the nw support
     *
     * @since S60 v3.1
     * @param aCtxImpl statemachine context
     * @param aRatePolicy the rate policy
     * @param aRateClassInd denotes the rate class within the rate policy
     * @param aRateMask the rate mask
     */
    void RecoverRatePolicy(
        WlanContextImpl& aCtxImpl,
        TTxRatePolicy& aRatePolicy,
        TUint aRateClassInd,
        WHA::TRate& aRateMask ) const;

    /**
     * Finalize the 802.11 b/g autorate policies so that they contain only 
     * rates which are also supported by the lower layers and the current 
     * network 
     *
     * @since S60 v3.1
     * @param aCtxImpl statemachine context
     * @param aRatePolicy the rate policy
     * @param aAutoRatePolicy the autorate policies
     */
    void FinalizeTxAutoratePolicy(
        WlanContextImpl& aCtxImpl,
        const TTxRatePolicy& aRatePolicy,
        TTxAutoRatePolicy& aAutoRatePolicy ) const;
    
    /**
     * Modifies the rate policy configuration by adding a new special policy,
     * which can be used for the transmission of certain frames, whose 
     * successful delivery is of special importance
     *
     * @param aCtxImpl statemachine context
     * @param aRatePolicy the rate policy
     * @param aAutoRatePolicy the autorate policies
     * @param aHtMcsPolicy HT MCS policy
     */
    void SpecialTxAutoratePolicy(
        WlanContextImpl& aCtxImpl,
        TTxRatePolicy& aRatePolicy,
        TTxAutoRatePolicy& aAutoRatePolicy,
        THtMcsPolicy& aHtMcsPolicy ) const;
    
    /**
     * Configures autorate policies to lower layers.
     * Before that, finalizes the MCS policies so that they contain only 
     * MCSs which are also supported by the lower layers and the current 
     * network.
     *
     * @since S60 v3.1
     * @param aCtxImpl statemachine context
     * @param aRatePolicy the rate policy
     * @param aQueue2RateClass Tx queue to rate class mapping
     * @param aHtMcsPolicy HT MCS policy
     * @param aCompleteMgmtRequest If ETrue, a WLAN Mgmt client request needs 
     *        to be completed
     *        If EFalse, a request doesn't need to be completed
     */
    void ConfigureForTxAutoratePolicy(
        WlanContextImpl& aCtxImpl,
        const TTxRatePolicy& aRatePolicy,
        const TQueue2RateClass& aQueue2RateClass,
        THtMcsPolicy& aHtMcsPolicy,
        TBool aCompleteMgmtRequest );
    
    /**
     * Updates - if necessary - the provided HT MCS policy to contain only 
     * MCSs which both the WHA layer and the nw support
     *
     * @param aCtxImpl statemachine context
     * @param aHtMcsPolicy HT MCS policy
     * @param aNbrOfMcsSets Number of MCS sets in the MCS policy
     */
    void HandleHtMcsPolicy(
        WlanContextImpl& aCtxImpl,
        THtMcsPolicy& aHtMcsPolicy,
        TUint aNbrOfMcsSets ) const;

    /**
     * Takes non-autorate policy into use.
     *
     * @since S60 v3.1
     * @param aCtxImpl statemachine context
     * @param aRatePolicy the rate policy
     * @param aRateMasks intersection of 802.11b/g rates which are:
     *        - included in the rate class AND
     *        - supported by the lower layers AND
     *        - supported by the current network
     * @param aQueue2RateClass Tx queue to rate class mapping
     * @param aInitialMaxTxRate4RateClass initial max Tx rate for every
     *        rate class 
     * @param aCompleteMgmtRequest If ETrue, a WLAN Mgmt client request needs 
     *        to be completed
     *        If EFalse, a request doesn't need to be completed
     */
    TBool ConfigureForTxRatePolicy(
        WlanContextImpl& aCtxImpl,
        const TTxRatePolicy& aRatePolicy,
        const TWhaRateMasks& aRateMasks,
        const TQueue2RateClass& aQueue2RateClass,
        const TInitialMaxTxRate4RateClass& initialMaxTxRate4RateClass,
        TBool aCompleteMgmtRequest );

    /**
     * Locates and stores the HT Capabilities element data of the target nw  
     *
     * @param aCtxImpl Statemachine context
     * @param aElementLocator Initialized Element locator which can
     *        be used to locate information elements related to the
     *        target network
     * @return ETrue if connecting to the target network is possible
     *         EFalse otherwise 
     */
    TBool HandleHtCapabilities( 
        WlanContextImpl& aCtxImpl,
        WlanElementLocator& aElementLocator ) const;

    /**
     * Locates and stores the HT Operation element data of the target nw  
     *
     * @param aCtxImpl Statemachine context
     * @param aElementLocator Initialized Element locator which can
     *        be used to locate information elements related to the
     *        target network
     * @return ETrue if connecting to the target network is possible
     *         EFalse otherwise 
     */
    TBool HandleHtOperation( 
        WlanContextImpl& aCtxImpl,
        WlanElementLocator& aElementLocator ) const;

    /**
     * Locates the relevant HT related elements of the target nw and 
     * stores their data 
     *
     * @param aCtxImpl Statemachine context
     * @param aElementLocator Initialized Element locator which can
     *        be used to locate information elements related to the
     *        target network
     * @return ETrue if connecting to the target network is possible
     *         EFalse otherwise 
     */
    TBool HandleDot11n( 
        WlanContextImpl& aCtxImpl,
        WlanElementLocator& aElementLocator ) const;
    
    };

#endif      // WLANDOT11STATE_H
