/*
* Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Interface implemented by lower adaptation. Core server uses
*                these services.
*
*/

/*
* %version: 36 %
*/

#ifndef ABS_CORE_DRIVERIF_H
#define ABS_CORE_DRIVERIF_H

#include "core_types.h"

class abs_core_driverif_callback_c;

/**
 * Abstract adaptation layer interface class for low-level management functionality.
 *
 * This interface is implemented by the adaptation layer and a reference
 * to the class implementing it is passed as a parameter to the core server
 * constructor.
 * 
 * @since S60 v3.1
 */
class abs_core_driverif_c
    {

public:

    /**
     * Destructor.
     */
    virtual ~abs_core_driverif_c() {};

    /**
     * Initialisation between core server - driverIf
     *
     * @since S60 v3.1
     * @param core_callback delivers callback interface to driverIf
     */
    virtual void init(
        abs_core_driverif_callback_c* core_callback ) = 0;

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
    virtual void start_ibss(
        u32_t request_id,
        const core_ssid_s& ssid,
        u32_t beacon_interval,
        u32_t channel,
        core_encryption_mode_e encrypt_level ) = 0;

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
    virtual void scan(
        u32_t request_id,
        core_scan_mode_e scan_mode,
        const core_ssid_s& scan_ssid,
        int_t scan_rate,
        const core_scan_channels_s& scan_channels,
        u32_t scan_min_ch_time,
        u32_t scan_max_ch_time,        
        bool_t is_split_scan ) = 0;

    /**
     * Request an ongoing scan to be stopped.
     * 
     * @since S60 v3.1
     * @param request_id An unique identification for the request.
     */
    virtual void stop_scan(
        u32_t request_id ) = 0;

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
    virtual void connect(
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
        const core_cipher_key_s* pairwise_key ) = 0;

    /**
     * Disconnect from the current network.
     *
     * @since S60 v3.1
     * @param request_id An unique identification for the request.
     */
    virtual void disconnect(
        u32_t request_id ) = 0;

    /**
     * Set WLAN power mode.
     *
     * @since S60 v3.1
     * @param request_id An unique identification for the request.
     * @param mode Power mode to use.
     */
    virtual void set_power_mode(
        u32_t request_id,
        const core_power_mode_s& mode ) = 0;

    /**
     * Set transmission power level.
     *
     * @since S60 v3.1
     * @param request_id An unique identification for the request.
     * @param tx_level Transmission power level in dBm.     
     */
    virtual void set_tx_power_level(
        u32_t request_id,
        u32_t tx_level ) = 0;

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
    virtual void add_cipher_key(
        core_cipher_key_type_e cipher_suite,
        u8_t key_index,
        u16_t key_length,
        const u8_t* key_data,
        const core_mac_address_s& mac,
        bool_t use_as_default ) = 0;

    /**
     * Get the current RCPI value.
     *
     * @since S60 v3.1
     * @param request_id An unique identification for the request.
     * @param rcpi The current RCPI value.     
     */
    virtual void get_current_rcpi(
        u32_t request_id,
        u32_t& rcpi ) = 0;

    /**
     * Delivers multicast MAC address to drivers.
     *
     * @since S60 v3.1
     * @param request_id An unique identification for the request.
     * @param join_group is set true_t if adding a multicast address
     *        otherwise the address will be removed.
     * @param multicast_addr contains the MAC address to add/remove
     */
    virtual void configure_multicast_group(
        u32_t request_id,
        bool_t join_group,
        const core_mac_address_s& multicast_addr ) = 0;

    /**
     * Set the parameters related to BSS lost indication.
     * 
     * @since S60 v3.1
     * @param request_id Identification of the corresponding request.
     * @param parameters Parameters to set.
     */
    virtual void set_bss_lost_parameters(
        u32_t request_id,
        const core_bss_lost_parameters_s& parameters ) = 0;

    /**
     * Set the parameters related to TX rate adaptation algorithm.
     * 
     * @since S60 v3.1
     * @param request_id Identification of the corresponding request.
     * @param parameters Parameters to set.
     */
    virtual void set_tx_rate_adaptation_parameters(
        u32_t request_id,
        const core_tx_rate_adaptation_parameters_s& parameters ) = 0;

    /**
     * Set the parameters related to power mode management.
     * 
     * @since S60 v3.1
     * @param request_id Identification of the corresponding request.
     * @param parameters Parameters to set.
     */
    virtual void set_power_mode_mgmt_parameters(
        u32_t request_id,
        const core_power_mode_mgmt_parameters_s& parameters ) = 0;

    /**
     * Set the parameters related to TX rate policies.
     *
     * @since S60 v3.2
     * @param request_id Identification of the corresponding request.
     * @param policies TX rate policies to set.
     * @param mappings Mappings between Access Classes and TX rate policies.
     */
    virtual void set_tx_rate_policies(
        u32_t request_id,
        const core_tx_rate_policies_s& policies,
        const core_tx_rate_policy_mappings_s& mappings ) = 0;        

    /**
     * Get packet statistics for the currently active connection.
     *
     * @since S60 v3.2
     * @param request_id Identification of the corresponding request.
     * @param statistics Packet statistics for the currently active connection.
     */
    virtual void get_packet_statistics(
        u32_t request_id,
        core_packet_statistics_by_access_category_s& statistics ) = 0;

    /**
     * Set the RCPI trigger level.
     *
     * @since S60 v3.2
     * @param request_id Identification of the corresponding request.
     * @param rcpi_trigger The RCPI trigger level.
     */
    virtual void set_rcpi_trigger_level(        
        u32_t request_id,
        u8_t rcpi_trigger ) = 0;

    /**
     * Set the U-APSD settings.
     *
     * @since S60 v3.2
     * @param request_id Identification of the corresponding request.
     * @param settings Current U-APSD settings to be set.
     */
    virtual void set_uapsd_settings(
        u32_t request_id,
        const core_uapsd_settings_s& settings ) = 0;

    /**
     * Set the power save settings.
     *
     * @since S60 v3.2
     * @param request_id Identification of the corresponding request.
     * @param settings Current power save settings to be set.
     */
    virtual void set_power_save_settings(
        u32_t request_id,
        const core_power_save_settings_s& settings ) = 0;

    /**
     * Set the parameters of a tx queue.
     *
     * @since S60 v3.2
     * @param request_id Identification of the corresponding request.
     * @param queue_id Queue to be configured.
     * @param medium_time Medium time to be used.
     * @param max_tx_msdu_lifetime Maximum Transmit MSDU Lifetime to be used.
     */
    virtual void set_tx_queue_parameters(
        u32_t request_id,
        core_access_class_e queue_id,
        u16_t medium_time,
        u32_t max_tx_msdu_lifetime ) = 0;

    /**
     * Allow sending of user data.
     *
     * @since S60 v3.1
     * @param request_id An unique identification for the request.
     */
    virtual void enable_user_data(
        u32_t request_id ) = 0;

    /**
     * Prevent sending of user data.
     *
     * @since S60 v3.1
     * @param request_id An unique identification for the request.
     */
    virtual void disable_user_data(
        u32_t request_id ) = 0;    

    /**
     * Unload the drivers and power down the WLAN hardware.  
     *
     * @since S60 v3.1
     * @param request_id An unique identification for the request.
     */
    virtual void unload_drivers(
        u32_t request_id ) = 0;

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
    virtual void load_drivers(
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
        u32_t enabled_features ) = 0;

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
    virtual void send_frame(        
        core_frame_type_e frame_type,
        const u16_t frame_length,
        const u8_t* const frame_data,
        u8_t frame_priority,
        const core_mac_address_s& destination,
        bool_t send_unencrypted ) = 0;

    /**
     * Cancel a pending asynchronous request.
     * 
     * @since S60 v3.1
     * @param request_id Identification of the corresponding request.
     */
    virtual void cancel_request(
        u32_t request_id ) = 0;

    /**
     * Set the ARP filter.
     *
     * @since S60 v5.0
     * @param request_id Identification of the corresponding request.
     * @param filter Current ARP filter to be set.
     */
    virtual void set_arp_filter(
        u32_t request_id,
        const core_arp_filter_s& filter ) = 0;

    /**
     * Set block ACK usage per traffic stream.
     * 
     * @since S60 v5.1
     * @param request_id Identification of the corresponding request. 
     * @param usage Block ACK usage information. 
     */    
    virtual void set_block_ack_usage(
        u32_t request_id,
        const core_block_ack_usage_s& usage ) = 0;

    /**
     * Set the SNAP header of frames to receive.  
     */
    virtual void set_snap_header_filter(
        u32_t request_id,
        const core_snap_header_s& header ) = 0;

    };

#endif // ABS_CORE_DRIVERIF_H
