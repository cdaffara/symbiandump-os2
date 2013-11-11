/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Part of WLAN HAL API
*
*/

/*
* %version: 6 %
*/

#ifndef WHA_H
#define WHA_H

#include <wlanwhanamespace.h>
#include <wlanwha_mib.h>

class MWlanOsa;
class WlanHpa;
class WlanSpia;

struct SHwBusAccessLayer;

NAMESPACE_BEGIN_WHA

struct SConfigureData;

/**
 *  Defines the WHA callback interface
 *
 *  @since S60 3.1
 */
class MWhaCb
    {
    
public:  // Methods
    
    virtual ~MWhaCb() {};
    
    /**
     * The WLAN PDD uses this method to give a command response event to the 
     * WLAN LDD.
     *
     * @since S60 3.1
     * @param aCommandId Event identifier.
     * @param aStatus Status code.
     * @param aCommandResponseParams Event specific data structure.
     */
    virtual void CommandResponse( 
        TCommandId aCommandId, 
        TStatus aStatus,
        const UCommandResponseParams& aCommandResponseParams ) = 0;

    /**
     * The WLAN PDD uses this method to give a command completion event to the
     * WLAN LDD.
     *
     * @since S60 3.1
     * @param aCompleteCommandId Event identifier.
     * @param aStatus Status code.
     * @param aCommandCompletionParams Event specific data structure.
     */
    virtual void CommandComplete( 
        TCompleteCommandId aCompleteCommandId, 
        TStatus aStatus,
        const UCommandCompletionParams& aCommandCompletionParams ) = 0;

    /**
     * The WLAN PDD uses this method to indicate a spontaneous event to the
     * WLAN LDD.
     *
     * @since S60 3.1
     * @param aIndicationId Event identifier.
     * @param aIndicationParams Event specific data structure.
     */
    virtual void Indication( 
        TIndicationId aIndicationId,
        const UIndicationParams& aIndicationParams ) = 0;

    /**
     * The WLAN PDD calls this method when it has transferred a packet to the
     * WLAN device.
     *
     * @since S60 3.1
     * @param aPacketId The ID that the WLAN LDD gave for the packet in the
     *        SendPacket call.
     */
    virtual void SendPacketTransfer(
        TPacketId aPacketId ) = 0;

    /**
     * The WLAN PDD calls method when the WLAN device has processed a packet
     * from its send queue.
     *
     * @since S60 3.1
     * @param aStatus The result of the packet sending:
     *        KSuccess: The packet sending was successful.
     *        KErrorRetryExceeded: Packet sending failed because of exceeding
     *        dot11ShortRetryLimit or dot11LongRetryLimit.
     *        KErrorLifetimeExceeded: Packet sending failed because of exceeding
     *        dot11MaxTransmitMsduLifeTime.
     *        KErrorNoLink: Packet sending failed because of the loss of the 
     *        network link. The WLAN flushed the packet from the queue before
     *        making a single transmit attempt for it.
     * @param aPacketId The ID that the WLAN LDD gave for the packet in the
     *        SendPacket call.
     * @param aRate The data rate or MCS at which the packet sending succeeded.
     *        (this parameter is only valid if aStatus is KSuccess.)
     *        If the transmission was a non-HT transmission, then this parameter
     *        contains the data rate in bit map format.
     *        If the transmission was an HT transmission, then this parameter
     *        contains the numeric value of the MCS.
     * @param aPacketQueueDelay The time the packet spent in the WLAN device 
     *        transmit queue before the WLAN device started transmission.
     *        The time is calculated from the point the WLAN device gets the
     *        packet from the host to the point the packet is ready for
     *        transmission.
     *        This parameter is only valid if aStatus is KSuccess.
     *        This value is zero if the WLAN device does not support the
     *        calculation.
     *        Unit: microseconds.
     * @param aMediaDelay The total time the packet spent in the WLAN device
     *        before transmission was completed.
     *        The time is calculated from the point the WLAN device gets the
     *        packet from the host to the point it gets an acknowledgement for
     *        the packet from the peer.
     *        This parameter is only valid if aStatus is KSuccess.
     *        This value is zero if the WLAN device does not support the
     *        calculation.
     *        Unit: microseconds.
     * @param aAckFailures The number of times the WLAN device transmitted the
     *        packet without receiving an acknowledgement.
     * @param The sequence number the WLAN device used when it transmitted the
     *        frame.
     */
    virtual void SendPacketComplete(
        TStatus aStatus,
        TPacketId aPacketId,
        TRate aRate,
        TUint32 aPacketQueueDelay,
        TUint32 aMediaDelay,
        TUint8 aAckFailures,
        TUint16 aSequenceNumber = 0 ) = 0;

    /**
     * The WLAN PDD calls this method to request a memory buffer for packet
     * reception.
     *
     * @since S60 3.1
     * @param aLength The length of the requested buffer in bytes.
     *        The upper limit for the length is the maximum length of
     *        an 802.11n A-MSDU.
     * @return The method allocates a buffer whose length is aLength + Rx
     *        offset bytes and returns a pointer to the beginning of the buffer.
     *        If the allocation fails, the method returns a NULL pointer and the
     *        WLAN PDD should discard the received packet.
     */
    virtual TAny* RequestForBuffer ( TUint16 aLength ) = 0;
    
    /**
     * The WLAN PDD uses this method to deliver a received packet.
     *
     * @since S60 3.1
     * @param aStatus The result of the of the packet reception:
     *        KSuccess: The packet reception was successful.
     *        KDecryptFailure: The packet reception failed because of a
     *        decryption error.
     *        KMicFailure: The packet reception failed because of a MIC failure.
     *        KFailed: The packet reception failed because of some other reason.
     * @param aFrame A pointer to the beginning of the packet content (the first
     *        byte of the MSDU or A-MSDU)
     *        If aStatus indicates a failure (KDecryptFailure, KMicFailure,
     *        KFailed ), the packet should contain only the MAC header (i.e.,
     *        the frame body is omitted).
     * @param aLength The length of the packet. Measured from the first byte of
     *        the MAC header to the last byte of the frame body.
     * @param aRate The data rate or MCS at which the frame was received
     *        If the packet was received as a non-HT transmission, then this 
     *        parameter contains the data rate in bit map format.
     *        If the packet was received as an HT transmission, then this 
     *        parameter contains the numeric value of the MCS.
     * @param aRCPI RCPI value of the received packet.
     * @param aChannel The channel on which the packet was received.
     * @param aBuffer A pointer to the beginning of the allocated memory buffer
     *        (i.e., the pointer that the WLAN PDD got with RequestForBuffer).
     *        This parameter is only used when operating in "multi buffer mode".
     *        In single buffer mode, the WLAN PDD should set it to NULL.
     * @param aFlags Bit field providing status information of the received
     *        frame.
     */
    virtual void ReceivePacket( 
        TStatus aStatus,
        const void* aFrame,
        TUint16 aLength,
        TRate aRate,
        TRcpi aRCPI,
        TChannelNumber aChannel,
        void* aBuffer,
        TUint32 aFlags ) = 0;
    };


/**
 *  Defines the WHA interface
 *
 *  @since S60 3.1
 */
class Wha
    {

public: // constants
    
    enum { KNumOfEdcaQueues = 4 };

public: // Methods
    
    virtual ~Wha() {};
    
    /**
     * WHA callback attachment method
     *
     * @since S60 3.1
     * @param aWhaCb 
     */
    inline void Attach( MWhaCb& aWhaCb );

    static Wha* Create( MWlanOsa& aOsa, 
                        WlanHpa& aHpa, 
                        const SHwBusAccessLayer& aTransPortLayer );

    static void Destroy( Wha* aWha );

    /**
     * This method downloads the firmware to the WLAN device and performs the
     * low level initialization of the device.
     *
     * @since S60 3.1
     * @param aData Firmware data. The content is vendor-specific.
     *        The memory behind the pointer is valid to the point when the WLAN
     *        PDD sends the corresponding command response event.
     * @param aLength The length of the data in bytes placed in the parameter 
     *        aData.
     */
    virtual void Initialize( 
        const void* aData, 
        TUint32 aLength ) = 0;

    /**
     * The WLAN LDD calls this method to configure the WLAN device.
     *
     * @since S60 3.1
     * @param aData Configuration data. The content is vendor-specific. 
     *        The memory behind the reference is valid to the point when the
     *        WLAN PDD sends the corresponding command response event.
     * @param aSettings Output data that holds the capabilities of the
     *        WLAN device.
     */
    virtual void Configure( 
        const SConfigureData& aData, 
        SSettings& aSettings ) = 0;

    /**
     * WLAN LDD calls this method to prepare the WLAN PDD for unloading.
     *
     * @since S60 3.1
     * @param aSynchronous Should the command be executed synchronously (ETrue)
     *        or not (EFalse).
     * @return KSuccess: The command was executed synchronously and no command 
     *         response event is sent.
     *         KPending: The command is executed asynchronously and the 
     *         corresponding command response event is sent upon command 
     *         completion.
     */
    virtual TStatus Release( TBool aSynchronous ) = 0;

    /**
     * The WLAN LDD uses this method to command the WLAN device to start
     * scanning the available networks.
     *
     * @since S60 3.1
     * @param aMaxTransmitRate The transmission rate of the probe requests
     *        Note: Just a single rate is selected as rate fallback is
     *        not to be used during the scan process.
     * @param aBand The used frequency band. 
     *        Only one band is scanned at a time and one bit is used to select
     *        the band to be scanned.
     * @param aNumOfChannels The number of channels provided in the array
     *       (aChannels).
     * @param aChannels A structure that specifies the scanned channels.
     * @param aScanType The scan type:
     *        EFgScan: foreground scan.
     *        EBgScan: background scan.
     *        EForcedBgScan: forced background scan.
     * @param aNumOfProbeRequests The number of probe requests (per SSID) to be
     *        sent to each channel.
     *        Value 0 means the device does not send any probe requests and the
     *        scan is a passive scan.
     * @param aSplitScan ETrue - use split scan.
     *        EFalse - do not use split scan.
     * @param aNumOfSSID The number of SSIDs in the SSID array (aSSID).
     *        Value 0 means that the array is empty and this is a broadcast
     *        scan. In a broadcast scan, the WLAN device puts an empty SSID in
     *        the probe requests.
     *        Value greater than 0 means that this is a directed scan. The WLAN
     *        device puts the SSIDs in the array to the probe requests.
     * @param aSsid An array of the SSID to be scanned in a directed scan.
     */
    virtual void Scan( 
        TRate aMaxTransmitRate, 
        TBand aBand,
        TUint8 aNumOfChannels,
        const SChannels* aChannels, 
        TScanType aScanType,
        TUint8 aNumOfProbeRequests,
        TBool aSplitScan,
        TUint8 aNumOfSSID,
        const SSSID* aSsid ) = 0;
        
    /**
     * The WLAN LDD uses this method to stop an ongoing scan process.
     *
     * @since S60 3.1
     */
    virtual void StopScan() = 0;

    /**
     * The WLAN LDD uses this method to command the WLAN device to join a BSS
     * or IBSS or to start an IBSS.
     *
     * @since S60 3.1
     * @param aMode The operation mode of the network:
     *        EIBSS: IBSS mode
     *        EBSS:  BSS mode
     * @param aBSSID The BSSID of network.
     * @param aSSID The SSID of the network.
     * @param aBand The used frequency band. Only one bit is used to
     *        select the band.
     * @param aChannel The channel number of the network.
     * @param aBeaconInterval The time between TBTTs in TUs.
     * @param aBasicRateSet The BSS basic rate set.
     * @param aAtimWindow The ATIM window of the IBSS.
     *        Note: When the ATIM window is zero, the initiated IBSS does not
     *        support power save.
     * @param aPreambleType The preamble type.
     *        Note: 1 and 2 Mbit/s transmissions always use a long preamble
     *        regardless of this setting.
     * @param aProbeForJoin Specifies if the device should send a Probe request
     *        with the specified SSID when joining the network.
     *        [Optional]
     */
    virtual void Join(
        TOperationMode aMode,
        const TMacAddress& aBSSID,
        const SSSID& aSSID, 
        TBand aBand,
        TChannelNumber aChannel,
        TUint32 aBeaconInterval,
        TRate aBasicRateSet,
        TUint16 aAtimWindow,
        TPreamble aPreambleType,
        TBool aProbeForJoin ) = 0;


    /**
     * The WLAN LDD uses this command to reset the WLAN device and the WLAN PDD
     * to their initial states (the state after the Initialize and Configure
     * commands).
     *
     * @since S60 3.1
     */
    virtual void Reset() = 0;

    /**
     * The WLAN LDD uses this method to change the WLAN device’s 802.11 power
     * management mode.
     *
     * @since S60 3.1
     * @param aPsMode The 802.11 power management mode the WLAN device should
     *        enter.
     */
    virtual void SetPsMode( TPsMode aPsMode ) = 0;

    /**
     * The WLAN LDD uses this method to fix connection parameters after the
     * initial connection setup (after the association response in the
     * infrastructure mode).
     *
     * @since S60 3.1
     * @param aDTIM Specifies the DTIM interval in multiples of beacons.
     * @param aAID Specifies the AID received during the association process.
     */
    virtual void SetBssParameters( 
        TUint8 aDTIM, 
        TUint16 aAID ) = 0;

    /** deprecated */
    virtual void Measure( 
        TPowerLevel /*aTxPowerLevel*/,
        TBand /*aBand*/,
        TChannelNumber /*aChannel*/,
        TUint8 /*aActivationDelay*/,
        TUint8 /*aMeasurementOffset*/,
        TUint8 /*aNumberOfMeasurementTypes*/,
        const SParameterSet* /*aParameterSet*/ ) {};

    /** deprecated */
    virtual void StopMeasure() {};
    
    /**
     * The WLAN LDD uses this method to read configuration information and
     * statistics from the WLAN device.
     *
     * @since S60 3.1
     * @param aMib The ID of the MIB to be accessed.
     */
    virtual void ReadMib( TMib aMib ) = 0;
    
    /**
     * The WLAN LDD uses this method to write configuration information to the
     * WLAN device.
     *
     * @since S60 3.1
     * @param aMib The ID of the MIB to be written.
     * @param aLength The length of the MIB.
     * @param aData A pointer to the structure specifying the MIB data according
     *        to the MIB ID.
     * @param aMore This flag is set to ETrue if the WLAN host driver is about
     *        to write more MIBs after this call.
     * @return KSuccess: The command was executed synchronously and no command
     *         response event is sent.
     *         KPending: The command is executed asynchronously and the
     *         corresponding command response event is sent upon command
     *         completion.
     */
    virtual TStatus WriteMib( 
        TMib aMib,
        TUint16 aLength,
        const void* aData,
        TBool aMore ) = 0;
    
    /**
     * This method adds a new (or replaces an old) encryption key to the WLAN
     * device.
     *
     * @since S60 3.1
     * @param aType The type of the key to be added:
     *        EWepGroupKey      – WEP group key
     *        EWepPairWiseKey   – WEP pairwise key
     *        ETkipGroupKey     – TKIP group key
     *        ETkipPairWiseKey  – TKIP pairwise key
     *        EAesGroupKey      – AES group key
     *        EAesPairWiseKey   – AES pairwise ket
     *        EWapiGroupKey     – WAPI group key
     *        EWapiPairWiseKey  – WAPI pairwise key
     * @param aKey A pointer to the structure containing the key material.
     * @param aEntryIndex The key entry index of the key to be added.
     */
    virtual void AddKey( 
        TKeyType aType, 
        const void* aKey,
        TUint8 aEntryIndex ) = 0;
    
    /**
     * This method removes encryption keys from the active key set.
     *
     * @since S60 3.1
     * @param aEntryIndex The index of the key to remove from the set.
     */
    virtual void RemoveKey( TUint8 aEntryIndex ) = 0;
    
    /**
     * The WLAN LDD uses this method to configure QoS parameters of
     * transmission queues.
     *
     * @since S60 3.1
     * @param aQueueId The ID for the queue.
     * @param aMaxLifeTime The time (in TUs) how long the MSDU can stay in the
     * transmit queue. If the timer expires before the MSDU is successfully
     * transmitted, the WLAN device shall discard the packet. [Optional]
     * @param aPsScheme The PS mode of the queue:
     *        ERegularPs - Regular PS mode. The queue is not trigger enabled.
     *        EUapsd     - U-APSD mode. The queue is trigger enabled.
     * @param aAckPolicy The ACK frame policy of the specified queue:
     *        ENormal - Normal ack.
     *        ENoaAck - No ack.
     * @param aMediumTime The amount of time the queue is allowed to access the
     *        WLAN air interface during one second interval.
     *        The unit of the parameter is 32 microseconds. Value 0
     *        means that the medium time is unlimited.
     */
    virtual void ConfigureQueue( 
        TQueueId aQueueId,
        TUint32 aMaxLifeTime,
        TPsScheme aPsScheme,
        const SSAPSDConfig& aSAPSDConfig, /** deprecated */
        TAckPolicy aAckPolicy,
        TUint16 aMediumTime ) = 0;

    /**
     * The WLAN LDD uses this method to configure EDCA access category
     * parameters.
     * 
     * @since S60 3.1
     * @param aCwMin CWmin (in slots) for the access class.
     * @param aCwMax CWmax (in slots) for the access class.
     * @param aAIFS AIFS value (in slots) for the access class.
     * @param aTxOplimit Tx Op Limit (in microseconds) for the access class.
     * @param aMaxReceiveLifeTime The maximum receive lifetime for the access 
     *        class. [Optional]
     */
    virtual void ConfigureAC( 
        TUint16 aCwMin[KNumOfEdcaQueues],
        TUint16 aCwMax[KNumOfEdcaQueues],
        TUint8 aAIFS[KNumOfEdcaQueues],
        TUint16 aTxOplimit[KNumOfEdcaQueues],
        TUint16 aMaxReceiveLifeTime[KNumOfEdcaQueues] ) = 0;
    
    /**
     * The WLAN LDD uses this method to send packets.
     * 
     * @since S60 3.1
     * @param aFrame A pointer to the beginning of the packet content (the first
     *        byte of the MSDU or A-MSDU).
     * @param aLength The length of the packet.
     *        Measured from the first byte of the MAC header to the last
     *        byte of the frame body.
     * @param aQueueId The transmit queue.
     * @param aTxRateClassId The index of the rate policy for this transmission.
     *        If the WLAN device supports autonomous rate adaptation, the index
     *        refers to the txAutoRatePolicy MIB.
     *        If the WLAN device does not support autonomous rate adaptation,
     *        the index refers to the txRatePolicy MIB.
     *        In both cases, the value 1 refers to the first policy.
     * @param aMaxTransmitRate The highest transmission rate to be used for this
     *        packet.
     *        This parameter is valid only when non-autonomous rate adaptation
     *        (txRatePolicy) is used.
     * @param aMore Informs the WLAN PDD if an another packet is pending for
     *        transmission in the WLAN LDD:
     *        EFalse: No, this is it for the time being.
     *        ETrue: Yes, another packet is pending for transmission.
     * @param aPacketId The packet identifier.
     * @param aPowerLevel The transmission power level. Unit: dBm. [Optional]
     * @param aExpiryTime The time (in TUs) how long the MSDU can stay in the
     *        transmit queue. [Optional]
     * @param aReserved Reserved field.
     * @return KSuccess: The WLAN PDD has buffered the packet for transfer to
     *         the WLAN device.
     *         The WLAN LDD can call SendPacket again to send a new packet.
     *         KSuccessXfer: The WLAN PDD has transferred the packet and all
     *         the previously buffered packets to the WLAN device.
     *         KPending: The WLAN PDD has buffered the packet for transfer to
     *         the WLAN device.
     *         The WLAN LDD can not call SendPacket until the WLAN PDD calls
     *         SendPacketTransfer.
     *         KQueueFull: The WLAN PDD has discarded the packet because the
     *         corresponding queue in the WLAN device is full.
     *         KSuccessQueueFull: The WLAN PDD has buffered the packet for
     *         transfer to the WLAN device. However, the corresponding queue
     *         in the WLAN device is now full.
     */
    virtual TStatus SendPacket(
        const void* aFrame,
        TUint16 aLength,
        TQueueId aQueueId,
        TUint8 aTxRateClassId,
        TRate aMaxTransmitRate,
        TBool aMore,
        TPacketId aPacketId,
        TPowerLevel aPowerLevel,
        TUint32 aExpiryTime,
        void* aReserved ) = 0;

    /**
     * The WLAN LDD calls this method to perform production line testing of the
     * WLAN device.
     *
     * @since S60 5.0
     * @param aType A parameter indicating which test to perform.
     * @param aParams Test specific parameters.
     *        An in/out parameter.
     */
    virtual void Plt( TPltType aType, void* aParams ) = 0;

protected:  // methods

    Wha( MWlanOsa& aOsa, WlanHpa& aHpa, WlanSpia& aSpia ) 
        : iWhaCb( NULL ), iOsa( aOsa ), iHpa( aHpa ), iSpia( aSpia ) {};

    inline MWhaCb& WhaCb();
    inline WlanHpa& Hpa();
    inline MWlanOsa& OsaCb();
    inline WlanSpia& Spia();

protected:  // data
    
    MWhaCb*     iWhaCb;

    MWlanOsa&   iOsa;
    WlanHpa&    iHpa;
    WlanSpia&   iSpia;
    };

#include <wlanwha.inl>

NAMESPACE_END_WHA

#endif  // WHA_H
