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
* Description:  Driver interface
*
*/

/*
* %version: 39 %
*/

#ifndef WLMDRIVERIF_H
#define WLMDRIVERIF_H

#include "802dot11.h"
#include "RWlanLogicalChannel.h"

#include "abs_core_driverif.h"        // Derived
#include "wlannotificationhandler.h"  // Derived
#include "wlanmgmtframehandler.h"     // Derived
#include "wlanmgmtcommandhandler.h"   // Derived
#include "wlanmgmtpacket.h"

class abs_core_driverif_callback_c;
class CWlanHwInit;

/**
*  CWlmDriverIf
*
*  @lib wlmserversrv.lib
*  @since Series 60 3.0
*/
NONSHARABLE_CLASS( CWlmDriverIf ) :
    public CBase, 
    public abs_core_driverif_c,
    public MWlanNotificationCallback,
    public MWlanMgmtFrameCallback,
    public MWlanMgmtCommandCallback
    {
public:  // Constructors and destructor
    
    /**
     * Two-phased constructor.
     */
    static CWlmDriverIf* NewL();
    
    /**
     * Destructor.
     */
    ~CWlmDriverIf();

public: // Functions from base classes

    //
    // Functions from abs_core_driverif_c
    //

    /**
     * Initialisation between core server - driverIf
     *
     * @since S60 v3.1
     * @param core_callback delivers callback interface to driverIf
     */
    void init(
        abs_core_driverif_callback_c* core_callback );

    /**
     * Start an IBSS network.
     *
     * @since S60 v3.1
     * @param request_id An unique identification for the request.
     * @param ssid Name of the network.
     * @param beacon_interval Beacon interval in TUs.
     * @param channel Used channel (1 - 11).
     * @param encrypt_level Used encryption level.
     */    
    void start_ibss(
        u32_t request_id,
        const core_ssid_s& ssid,
        u32_t beacon_interval,
        u32_t channel,
        core_encryption_mode_e encrypt_level );

    /**
     * Request available networks with the given SSID.
     * 
     * @since S60 v3.1
     * @param request_id An unique identification for the request.
     * @param scan_mode Defines whether active or passive scan is performed.
     * @param scan_ssid Name of the network to scan for.
     * @param scan_rate Scan rate in units of 500kbit/second.
     * @param scan_channels Channels to scan.
     * @param scan_min_ch_time Minimum time to listen for beacons/probe responses on a channel.
     * @param scan_max_ch_time Maximum time to listen for beacons/probe responses on a channel.
     * @param is_split_scan Whether the scan should be performed as a split-scan.
     * @note If the length of the SSID is zero, a broadcast scan is performed.
     */
    void scan(
        u32_t request_id,
        core_scan_mode_e scan_mode,
        const core_ssid_s& scan_ssid,
        int_t scan_rate,
        const core_scan_channels_s& scan_channels,
        u32_t scan_min_ch_time,
        u32_t scan_max_ch_time,
        bool_t is_split_scan );

    /**
     * Request an ongoing scan to be stopped.
     * 
     * @since S60 v3.1
     * @param request_id An unique identification for the request.
     */
    void stop_scan(
        u32_t request_id );

    /**
     * Connect (authenticate and associate) to a BSS.
     * 
     * @since S60 v3.1
     * @param request_id An unique identification for the request.
     * @param status The status code from the connection attempt is stored here.
     * @param ssid Name of the network.
     * @param bssid BSSID of the access point.
     * @param auth_algorithm Authentication algorithm number.
     * @param encryption_level Used encryption level.
     * @param pairwise_key_type Pairwise cipher key type to be used.
     * @param is_infra Whether the network is an IBSS or an infra network.
     * @param ie_data_length Length of IE data.
     * @param ie_data IE data.
     * @param scan_frame_length Length of beacon/probe response frame from the BSS.
     * @param scan_frame Pointer to beacon/probe response frame from the BSS.    
     * @param is_pairwise_key_invalidated Whether the pairwise key should be invalidated.
     * @param is_group_key_invalidated Whether the group key should be invalidated
     * @param is_radio_measurement_supported Whether the radio measurement is supported
     * @param pairwise_key Pairwise key to set before association. NULL if none.
     */
    void connect(
        u32_t request_id,
        core_management_status_e& status,
        const core_ssid_s& ssid,
        const core_mac_address_s& bssid,
        u16_t auth_algorithm,
        core_encryption_mode_e encryption_level,
        core_cipher_key_type_e pairwise_key_type,
        bool_t is_infra,
        u32_t ie_data_length,
        const u8_t* ie_data,
        u32_t scan_frame_length,
        const u8_t* scan_frame,
        bool_t is_pairwise_key_invalidated,
        bool_t is_group_key_invalidated,
        bool_t is_radio_measurement_supported,
        const core_cipher_key_s* pairwise_key );

    /**
     * Disconnect from the current network.
     *
     * @since S60 v3.1
     * @param request_id An unique identification for the request.
     */
    void disconnect(
        u32_t request_id );

    /**
     * Set WLAN power mode.
     *
     * @since S60 v3.1
     * @param request_id An unique identification for the request.
     * @param mode Power save mode to use.
     */
    void set_power_mode(
        u32_t request_id,
        const core_power_mode_s& mode );

    /**
     * Set transmission power level.
     *
     * @since S60 v3.1
     * @param request_id An unique identification for the request.
     * @param tx_level Transmission power level in dBm.     
     */
    void set_tx_power_level(
        u32_t request_id,
        u32_t tx_level );

    /**
     * Add a cipher key.
     *
     * @since S60 v3.1
     * @param cipher_suite The cipher suite of the key.
     * @param key_index Index of the key.
     * @param key_length Length of the cipher key.
     * @param key_data Cipher key data.
     * @param mac Defines the MAC address the key is used for.
     * @param use_as_default in case of wep, whether the key is used as default cipher key     
     */
    void add_cipher_key(
        core_cipher_key_type_e cipher_suite,
        u8_t key_index,
        u16_t key_length,
        const u8_t* key_data,
        const core_mac_address_s& mac,
        bool_t use_as_default );

    /**
     * Get the current RCPI value.
     *
     * @since S60 v3.1
     * @param request_id An unique identification for the request.
     * @param rcpi The current RCPI value.     
     */
    void get_current_rcpi(
        u32_t request_id,
        u32_t& rcpi );

    /**
     * Delivers multicast MAC address to drivers.
     *
     * @since S60 v3.1
     * @param request_id An unique identification for the request.
     * @param join_group is set true_t if adding a multicast address
     *        otherwise the address will be removed.
     * @param multicast_addr contains the MAC address to add/remove
     */
    void configure_multicast_group(
        u32_t request_id,
        bool_t join_group,
        const core_mac_address_s& multicast_addr );

    /**
     * Set the parameters related to BSS lost indication.
     * 
     * @since S60 v3.1
     * @param request_id Identification of the corresponding request.
     * @param parameters Parameters to set.
     */
    void set_bss_lost_parameters(
        u32_t request_id,
        const core_bss_lost_parameters_s& parameters );

    /**
     * Set the parameters related to tx rate adaptation algorithm.
     * 
     * @since S60 v3.1
     * @param request_id Identification of the corresponding request.
     * @param parameters Parameters to set.
     */
    void set_tx_rate_adaptation_parameters(
        u32_t request_id,
        const core_tx_rate_adaptation_parameters_s& parameters );

    /**
     * Set the parameters related to power mode management.
     * 
     * @since S60 v3.1
     * @param request_id Identification of the corresponding request.
     * @param parameters Parameters to set.
     */
    void set_power_mode_mgmt_parameters(
        u32_t request_id,
        const core_power_mode_mgmt_parameters_s& parameters );

    /**
     * Set the parameters related to TX rate policies.
     *
     * @since S60 v3.2
     * @param request_id Identification of the corresponding request.
     * @param policies TX rate policies to set.
     * @param mappings Mappings between Access Classes and TX rate policies.
     */
    void set_tx_rate_policies(
        u32_t request_id,
        const core_tx_rate_policies_s& policies,
        const core_tx_rate_policy_mappings_s& mappings );        

    /**
     * Get packet statistics for the currently active connection.
     *
     * @since S60 v3.2
     * @param request_id Identification of the corresponding request.
     * @param statistics Packet statistics for the currently active connection.
     */
    void get_packet_statistics(
        u32_t request_id,
        core_packet_statistics_by_access_category_s& statistics );

    /**
     * Set the RCPI trigger level.
     *
     * @since S60 v3.2
     * @param request_id Identification of the corresponding request.
     * @param rcpi_trigger The RCPI trigger level.
     */
    void set_rcpi_trigger_level(
        u32_t request_id,
        u8_t rcpi_trigger );

    /**
     * Set the U-APSD settings.
     *
     * @since S60 v3.2
     * @param request_id Identification of the corresponding request.
     * @param settings Current U-APSD settings to be set.
     */
    void set_uapsd_settings(
        u32_t request_id,
        const core_uapsd_settings_s& settings );

    /**
     * Set the power save settings.
     *
     * @since S60 v3.2
     * @param request_id Identification of the corresponding request.
     * @param settings Current power save settings to be set.
     */
    void set_power_save_settings(
        u32_t request_id,
        const core_power_save_settings_s& settings );

    /**
     * Set the parameters of a tx queue.
     *
     * @since S60 v3.2
     * @param request_id Identification of the corresponding request.
     * @param queue_id Queue to be configured.
     * @param medium_time Medium time to be used.
     * @param max_tx_msdu_lifetime Maximum Transmit MSDU Lifetime to be used.
     */
    void set_tx_queue_parameters(
        u32_t request_id,
        core_access_class_e queue_id,
        u16_t medium_time,
        u32_t max_tx_msdu_lifetime );

    /**
     * Allow sending of user data.
     *
     * @since S60 v3.1
     * @param request_id An unique identification for the request.
     */
    void enable_user_data(
        u32_t request_id );

    /**
     * Prevent sending of user data.
     *
     * @since S60 v3.1
     * @param request_id An unique identification for the request.
     */
    void disable_user_data(
        u32_t request_id );    

    /**
     * Unload the drivers and power down the WLAN hardware.  
     *
     * @since S60 v3.1
     * @param request_id An unique identification for the request.
     */
    void unload_drivers(
        u32_t request_id );

    /**
     * Load the drivers and power up the WLAN hardware.
     *
     * @since S60 v3.1
     * @param request_id An unique identification for the request.
     * @param rts_threshold Determines the maximum packet size that can be sent without RTS/CTS protocol.
     * @param max_tx_msdu_lifetime Maximum time to (re-)send the whole MSDU packet in TUs.
     * @param qos_null_frame_entry_timeout Defines the time period in microseconds which is used
     *                                     to evaluate whether QoS NULL data frame sending should be started.
     * @param qos_null_frame_entry_tx_count Defines how many Voice AC packets must be sent during the time
     *                                      period defined in qos_null_frame_entry_timeout before QoS NULL
     *                                      data frame sending is started.
     * @param qos_null_frame_interval Defines how often a QoS NULL data frame is sent.
     * @param qos_null_frame_exit_timeout Defines how soon after the last Voice AC packet QoS NULL data
     *                                    frame sending is stopped.
     * @param keep_alive_interval Defines how often NULL data frames are sent if there are no other frames
     *                            to send.
     * @param sp_rcpi_target Defines the signal predictor algorithm "target" RCPI value for roam indication.
     * @param sp_time_target Defines the signal predictor algorithm "target" time for roam indication
     *                       (in microseconds).
     * @param sp_min_indication_interval Defines the minimum time interval for consecutive roam
     *                                   indications from the signal predictor algorithm (in microseconds).
     * @param enabled_features Bitmask of enabled features. @see core_feature_e.
     */
    void load_drivers(
        u32_t request_id,
        uint_t rts_threshold,
        u32_t max_tx_msdu_lifetime,
        u32_t qos_null_frame_entry_timeout,
        u32_t qos_null_frame_entry_tx_count,
        u32_t qos_null_frame_interval,
        u32_t qos_null_frame_exit_timeout,
        u32_t keep_alive_interval,
        u32_t sp_rcpi_target,
        u32_t sp_time_target,
        u32_t sp_min_indication_interval,
        u32_t enabled_features );

    /**
     * Send a frame.
     *
     * @since S60 v3.1
     * @param frame_type The type of the frame to send.
     * @param frame_length The length of the frame.
     * @param frame_data Pointer to the frame data.
     * @param frame_priority User priority to use for sending the frame.
     * @param destination Destination MAC address.
     * @param send_unencrypted Whether the frame must be sent unencrypted.
     */
    void send_frame(        
        core_frame_type_e frame_type,
        const u16_t frame_length,
        const u8_t* const frame_data,
        u8_t frame_priority,
        const core_mac_address_s& destination,
        bool_t send_unencrypted );

    /**
     * Cancel a pending asynchronous request.
     * 
     * @since S60 v3.1
     * @param request_id Identification of the corresponding request.
     */
    void cancel_request(
        u32_t request_id );

	/**
     * Set the ARP filter.
     *
     * @since S60 v5.0
     * @param request_id Identification of the corresponding request.
     * @param filter Current ARP filter to be set.
     */
    void set_arp_filter(
        u32_t request_id,
        const core_arp_filter_s& filter );

    /**
     * Set block ACK usage per traffic stream.
     * 
     * @since S60 v5.1
     * @param request_id Identification of the corresponding request. 
     * @param usage Block ACK usage information. 
     */    
    void set_block_ack_usage(
        u32_t request_id,
        const core_block_ack_usage_s& usage );

    /**
     * Set the SNAP header of frames to receive.
     * 
     * @since S60 v5.1
     * @param request_id Identification of the corresponding request. 
     * @param header SNAP header of frames to receive. 
     */
    void set_snap_header_filter(
        u32_t request_id,
        const core_snap_header_s& header );

    //
    // Functions for handler callbacks
    //
    /**
     * From MWlanNotificationCallback
     * Notification event received from driver side
     * 
     * @since S60 v3.1
     * @param indication
     */
    void OnNotify( const TIndication& aIndication );

    /**
     * From MWlanMgmtFrameCallback
     * Management frame received from driver side
     * 
     * @since S60 v3.1
     * @param aFrameType specifies the frame type
     * @param aLength of the frame data
     * @param aData contains the frame
     * @param aRcpi RCPI value of the frame.
     */
    void OnFrameReceive(
        const TDataBuffer::TFrameType aFrameType,
        const TUint aLength,
        const TUint8* const aData,
        TUint8 aRcpi );
        
    /**
     * From MWlanMgmtCommandCallback
     * Management command completion callback received from driver side
     * 
     * @since S60 v3.1
     * @param command completion status
     */
    void OnRequestComplete( TInt status );

    /**
     * GetMacAddress is called by CWlmServer during startup
     * @param aMac contains the MAC address on completion
     * @return error code
     */ 
    TInt GetMacAddress( TMacAddress& aMac );

    /**
     * Notify the core server about an adaptation event.
     * @param aIndication Adaptation event.
     */
    void Notify( core_am_indication_e aIndication );

private:

    /**
     * C++ default constructor.
     */
    CWlmDriverIf();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

    /**
     * Async callback function
     * @param aThisPtr Pointer to the event data.
     * @return error code
     */
    static TInt AsynchCallbackFunction( TAny* aThisPtr );
    
    /**
     * UnloadDrivers  
     * @since S60 v3.1
     */
    void UnloadDrivers();

    /**
     * SetRequestId sets the new requestId to iPendingRequestId
     * if no prior requests pending.
     * @since S60 v3.1
     * @param aRequestId specifies the request id
     * @return error code 
     */
    TInt SetRequestId( TUint32 aRequestId );

private:    // Data

    /**
     * Callback interface to core server. Not owned by this pointer.
     */
    abs_core_driverif_callback_c* iCoreServer;
    /**
     * Pending command's requestId
     */
    TUint32 iPendingRequestId;
    
    /**
     * Handler for notifications
     */
    CWlanNotificationHandler* iNotificationHandler;
    /**
     * Handler for management commands
     */
    CWlanMgmtCommandHandler* iManagementCommandHandler;
    /**
     * Handler for received management frames
     */
    CWlanMgmtFrameHandler* iManagementFrameHandler;

    /** 
     * Asynchronous callback
     */
    CAsyncCallBack* iAsynchCallback;        
    /**
     * Error status that will be returned in case asynch callback is called
     */
    core_error_e iErrorStatus;
    
    /**
     * Handle to hardware specific client that reads initialization
     * data from CMT side
     */
    CWlanHwInit* iHwInit;

    /**
     * Interface to access LDD
     */
    RWlanLogicalChannel iChannel;
    /**
     * TRUE if PDD loaded 
     */
    TBool iIsPDDLoaded;
    /**
     * TRUE if LDD loaded
     */
    TBool iIsLDDLoaded;

    /**
     * If defined, association response status is stored here.
     * Not owned by this pointer.
     */
    core_management_status_e* iManagementStatus;
    };

#endif // WLMDRIVERIF_H
