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
* Description:  Interface implemented by core server.
*                Adaptation layer can use core services through this interface.
*
*/

/*
* %version: 32 %
*/

#ifndef ABS_CORE_SERVER_H
#define ABS_CORE_SERVER_H

#include "am_platform_libraries.h"
#include "core_types.h"
#include "core_type_list.h"

class ScanList;

/**
 * Abstract core server interface class for WLAN management functionality.
 *
 * This interface is implemented by the core server and it provides
 * methods for the adaptation layer.
 *
 * @since S60 v3.1
 */
class abs_core_server_c
    {

public:

    /**
     * Destructor.
     */
    virtual ~abs_core_server_c() {};

    /**
     * Asynchronous methods that the core server will complete by calling
     * abs_core_server_callback_c::request_complete.
     */

    /**
     * Connect to a network.
     *
     * @since S60 v3.1
     * @param request_id An unique identification for the request.
     * @param settings The settings used for establishing the connection.
     * @param connect_status contains the error code of connection attempt
     *        on completion
     * @param ssid_list List of possible secondary SSIDs.
     */
    virtual void connect(
        u32_t request_id,
        const core_iap_data_s& settings,
        core_connect_status_e& connect_status,
        core_type_list_c<core_ssid_entry_s>* ssid_list = NULL ) = 0;

    /**
     * Disconnect an active connection.
     * 
     * @since S60 v3.1
     * @param request_id An unique identification for the request.
     */
    virtual void release(
        u32_t request_id ) = 0;

    /**
     * Request available networks with the given SSID.
     * 
     * @since S60 v3.1
     * @param request_id An unique identification for the request.
     * @param scan_mode Defines whether active or passive scan is performed.
     * @param scan_ssid Name of the network to scan for.
     * @param scan_channels Channels to scan.
     * @param scan_data Scan results are stored here.
     * @param scan_max_age Maximum age of returned scan results in seconds.
     * @param is_current_ap_added Whether the current AP needs to be added during an ongoing
     *                            connection if not otherwise found in the scan.
     * @note If the length of the SSID is zero, a broadcast scan is performed.
     */
    virtual void get_scan_result(
        u32_t request_id,
        core_scan_mode_e scan_mode,
        const core_ssid_s& scan_ssid,
        const core_scan_channels_s& scan_channels,
        u8_t scan_max_age,
        ScanList& scan_data,
        bool_t is_current_ap_added = true_t ) = 0;

    /**
     * Find which iaps from the given list are currently available.
     * @since S60 v3.1
     * @param request_id An unique identification for the request.
     * @param is_active_scan_allowed specifies whether active scanning is allowed.
     * @param iap_data_list containing list of iap data structures.
     * @param iap_availability_list List of available IAPs.
     * @param iap_ssid_list List of possible secondary SSIDs.
     * @param scan_data contains the scan results
     */
    virtual void get_available_iaps(
        u32_t request_id,
        bool_t is_active_scan_allowed,
        core_type_list_c<core_iap_data_s>& iap_data_list,
        core_type_list_c<core_iap_availability_data_s>& iap_availability_list,
        core_type_list_c<core_ssid_entry_s>* iap_ssid_list,
        ScanList& scan_data ) = 0;

    /**
     * Get the current RCPI value of the connection.
     * 
     * @since S60 v3.1
     * @param request_id An unique identification for the request.
     * @param rcpi Current channel power indicator (RCPI).
     */
    virtual void get_current_rcpi(
        u32_t request_id,
        u32_t& rcpi ) = 0;

    /**
     * Disable all WLAN activity.
     *
     * Disconnects the active connection and prevents any further
     * WLAN activity until enable_wlan() is called.
     *
     * @since S60 v3.1
     * @param request_id An unique identification for the request.
     */        
    virtual void disable_wlan(
        u32_t request_id ) = 0;
        
    /**
     * Allow WLAN activity after disabled_wlan().
     *
     * @since S60 v3.1     
     * @param request_id An unique identification for the request.
     */
    virtual void enable_wlan(
        u32_t request_id ) = 0;

    /**
     * Get packet statistics of the current connection.
     *
     * @since S60 v3.2
     * @param request_id An unique identification for the request.
     * @param statistics Packet statistics of the current connection.     
     */
    virtual void get_packet_statistics(
        u32_t request_id,
        core_packet_statistics_s& statistics ) = 0;

    /**
     * Request creation of a virtual traffic stream.
     *
     * @since S60 v3.2
     * @param request_id An unique identification for the request.
     * @param tid TID of the virtual traffic stream.
     * @param user_priority User Priority of the virtual traffic stream.
     * @param is_automatic_stream Whether the virtual traffic stream
     *                            has been created automatically.
     * @param params Parameters of the virtual traffic stream.
     * @param stream_id ID assigned to this virtual traffic stream on
     *                  successful completion.
     * @param stream_status Status of the virtual traffic stream on
     *                      successful completion. 
     * @note If the TID has been defined as TRAFFIC_STREAM_ID_NONE,
     *       core server will select the next free ID.
     */
    virtual void create_traffic_stream(
        u32_t request_id,
        u8_t tid,
        u8_t user_priority,
        bool_t is_automatic_stream,
        const core_traffic_stream_params_s& params,
        u32_t& stream_id,
        core_traffic_stream_status_e& stream_status ) = 0;

    /**
     * Request deletion of a virtual traffic stream.
     *
     * @since S60 v3.2
     * @param request_id An unique identification for the request.
     * @param stream_id ID of the virtual traffic stream.
     */
    virtual void delete_traffic_stream(
        u32_t request_id,
        u32_t stream_id ) = 0;

    /**
     * Start Protected Setup.
     * 
     * @since S60 v3.2
     * @param request_id An unique identification for the request.
     * @param iap_data IAP data used for running Protected Setup.
     * @param iap_data_list IAP data results from a successful Protected Setup operation.
     * @param protected_setup_status contains the error code of Protected Setup attempt
     *        on completion.
     */
    virtual void run_protected_setup(
        u32_t request_id,
        const core_iap_data_s& iap_data,
        core_type_list_c<core_iap_data_s>& iap_data_list,
        core_protected_setup_status_e& protected_setup_status ) = 0;

    /**
     * Initiate a roam to the given BSSID.
     * 
     * @since S60 v3.2
     * @param request_id An unique identification for the request.
     * @param bssid BSSID to roam to. If BSSID is set to BROADCAST_MAC_ADDR,
     *              search for a better BSS is initiated.
     * @note This method does not guarantee that the a roam to the given BSSID
     *       will occur. If the roam succeeds, the request will be completed
     *       with core_error_ok, otherwise with an error code.  
     */
    virtual void directed_roam(
        u32_t request_id,
        const core_mac_address_s& bssid ) = 0;

    /**
     * Synchronous methods.
     */
    
    /**
     * Get the BSSID of the access point currently connected to.
     * 
     * @since S60 v3.1
     * @param bssid BSSID of the access point.
     * @return core_error_ok if success, an error code otherwise.
     */
    virtual core_error_e get_current_bssid(
        core_mac_address_s& bssid ) = 0;

    /**
     * Get the name of the network (SSID) currently connected to.
     * 
     * @since S60 v3.1
     * @param ssid Current SSID.
     * @return core_error_ok if success, an error code otherwise.
     */
    virtual core_error_e get_current_ssid(
        core_ssid_s& ssid ) = 0;

    /**
     * Get the current security mode of the connection.
     * 
     * @since S60 v3.1
     * @param mode Current security mode.
     * @return core_error_ok if success, an error code otherwise.
     */
    virtual core_error_e get_current_security_mode(
        core_connection_security_mode_e& mode ) = 0;

    /**
     * Get the current connection state.
     * 
     * @since S60 v3.1
     * @param state Current connection state.
     * @return core_error_ok if success, an error code otherwise.
     */
    virtual core_error_e get_current_connection_state(
        core_connection_state_e& state ) = 0;

    /**
     * Inform core server about updated device settings.
     * 
     * @since S60 v3.1     
     * @param settings Updated device settings.
     * @return core_error_ok if success, an error code otherwise.
     * @note This method does not guarantee that the new parameters
     *       are taken into use immediately.
     */
    virtual core_error_e update_device_settings(
        core_device_settings_s& settings ) = 0;

    /**
     * Set the preferred power save mode.
     * 
     * @since S60 v3.1
     * @param mode Preferred power save mode.
     * @return core_error_ok if success, an error code otherwise.
     * @note This method does not guarantee that the new parameters
     *       are taken into use immediately.
     */
    virtual core_error_e set_power_save_mode(
        const core_power_save_mode_s& mode ) = 0;

    /**
     * Add a BSSID to the rogue list.
     *
     * Prevents roaming to the given BSSID and hides it from
     * the scan results.
     *
     * @since S60 v3.1
     * @param bssid BSSID to add to the list.
     * @return core_error_ok if success, an error code otherwise.
     * @note The list entry will be persistent, meaning it will not
     *       be cleared when the connection is closed.
     */
    virtual core_error_e add_bssid_to_rogue_list(
        const core_mac_address_s& bssid ) = 0;

    /**
     * Remove a BSSID from the rogue list.
     *
     * @since S60 v3.1
     * @param bssid BSSID to remove from the list.
     * @return core_error_ok if success, an error code otherwise.
     */
    virtual core_error_e remove_bssid_from_rogue_list(
        const core_mac_address_s& bssid ) = 0;

    /**
     * Return a list of BSSIDs on the rogue list.
     *
     * @since S60 v3.2
     * @param rogue_list List of BSSIDs on the rogue list.
     * @return core_error_ok if success, an error code otherwise.
     */
    virtual core_error_e get_rogue_list(
        core_type_list_c<core_mac_address_s>& rogue_list ) = 0;

    /**
     * Set the RCP level notification boundary.
     *
     * When the signal strength is getting worse (RCPI value is getting smaller),
     * the core server will indicate core_notification_e::core_notification_rcp_changed
     * when the RCPI value gets smaller than the the rcp_level_boundary value.
     *
     * When the signal strength is getting better, the core server will indicate
     * core_notification_e::core_notification_rcp_changed when the RCPI value
     * gets bigger than the combined value of rcp_level_boundary and hysteresis.
     *
     * @since S60 v3.1
     * @param rcp_level_boundary RCP level notification boundary.
     * @param hysteresis Specifies the difference between boundaries when 
     *        the RCP level is rising vs. descending
     * @return core_error_ok if success, an error code otherwise.
     */
    virtual core_error_e set_rcp_level_notification_boundary(
        const i32_t rcp_level_boundary,
        const i32_t hysteresis ) = 0;

    /**
     * Clear packet statistics of the current connection.
     *
     * @since S60 v3.2
     * @return core_error_ok if success, an error code otherwise.
     */
    virtual core_error_e clear_packet_statistics() = 0;

    /**
     * Get the current U-APSD settings.
     *
     * @since S60 v3.2
     * @param settings Current U-APSD settings.
     * @return core_error_ok if success, an error code otherwise.
     */
    virtual core_error_e get_uapsd_settings(
        core_uapsd_settings_s& settings ) = 0;

    /**
     * Set the U-APSD settings.
     *
     * @since S60 v3.2
     * @param settings Current U-APSD settings to be set.
     * @return core_error_ok if success, an error code otherwise.
     * @note This method does not guarantee that the new parameters
     *       are taken into use immediately.
     */
    virtual core_error_e set_uapsd_settings(
        const core_uapsd_settings_s& settings ) = 0;

    /**
     * Get the current power save settings.
     *
     * @since S60 v3.2
     * @param settings Current power save settings.
     * @return core_error_ok if success, an error code otherwise.
     */
    virtual core_error_e get_power_save_settings(
        core_power_save_settings_s& settings ) = 0;

    /**
     * Set the power save settings.
     *
     * @since S60 v3.2
     * @param settings Current power save settings to be set.
     * @return core_error_ok if success, an error code otherwise.
     * @note This method does not guarantee that the new parameters
     *       are taken into use immediately.
     */
    virtual core_error_e set_power_save_settings(
        const core_power_save_settings_s& settings ) = 0;

    /**
     * Get information about the current AP.
     *
     * @since S60 v3.2
     * @param info Information about the current AP.
     * @return core_error_ok if success, an error code otherwise.
     */
    virtual core_error_e get_current_ap_info(
        core_ap_information_s& info ) = 0;

    /**
     * Get roam metrics of the current connection.
     *
     * @since S60 v3.2
     * @param roam_metrics Roam metrics of the current connection.
     */
    virtual void get_roam_metrics(
        core_roam_metrics_s& roam_metrics ) = 0;

    /**
     * Set the arp filter.
     *
     * @since S60 v5.0
     * @param filter Current ARP filter to be set.
     * @return core_error_ok if success, an error code otherwise.
     * @note This method does not guarantee that the new filter
     *       is taken into use immediately.
     */
    virtual core_error_e set_arp_filter(
        const core_arp_filter_s& filter ) = 0;

    /**
     * Delivers multicast MAC address to drivers.
     *
     * @since S60 v3.1
     * @param join_group is set true_t if adding a multicast address
     *        otherwise the address will be removed.
     * @param multicast_addr contains the MAC address to add/remove
     * @return core_error_ok if success, an error code otherwise.
     * @note This method does not guarantee that the new address
     *       is taken into use immediately.
     */
    virtual core_error_e configure_multicast_group(
        bool_t join_group,
        const core_mac_address_s& multicast_addr ) = 0;

    /**
     * Get information about current traffic statuses and traffic modes for
     * access classes.
     *
     * @since S60 v3.2
     * @param info Information about the current traffic statuses and traffic modes.
     * @return core_error_ok if success, an error code otherwise.
     */
    virtual core_error_e get_current_ac_traffic_info(
        core_ac_traffic_information_s& info ) = 0;

    /**
     * Set the currently enabled feature set.
     *
     * @since S60 v5.2
     * @param features Bitmask of enabled features.
     * @return core_error_ok if success, an error code otherwise.
     */
    virtual core_error_e set_enabled_features(
        u32_t features ) = 0;

    /**
     * Schedule an immediate driver unload.
     *
     * @since S60 v3.1     
     * @return core_error_ok if success, an error code otherwise.
     */
    virtual core_error_e unload_drivers() = 0;

    /**
     * An asynchronous request from the core server has been completed.
     *
     * @since S60 v3.1
     * @param request_id Identification of the corresponding request.
     * @param status Completion status of the request.
     */
    virtual void request_complete(
        u32_t request_id,
        core_error_e status ) = 0;

    /**
     * Cancel a pending asynchronous request.
     * 
     * @since S60 v3.1
     * @param request_id Identification of the corresponding request.
     */
    virtual void cancel_request(
        u32_t request_id ) = 0;

    };

#endif // ABS_CORE_SERVER_H
