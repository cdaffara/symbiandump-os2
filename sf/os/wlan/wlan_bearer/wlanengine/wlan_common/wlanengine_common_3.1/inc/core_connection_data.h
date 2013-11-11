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
* Description:  Class holding all connection related data
*
*/

/*
* %version: 34 %
*/

#ifndef CORE_CONNECTION_DATA_H
#define CORE_CONNECTION_DATA_H

#include "genscaninfo.h"
#include "core_types.h"
#include "core_type_list.h"
#include "core_ap_data.h"
#include "core_iap_data.h"
#include "core_scan_channels.h"
#include "core_traffic_stream_list.h"
#include "core_virtual_traffic_stream_list.h"

/**
 * Structure for storing AP failure count.
 */
struct core_ap_failure_count_s
    {
    core_mac_address_s bssid; ///< BSSID of the AP.
    u8_t count;               ///< The number of failures.
    };

/**
 * Structure for storing AP authentication failure count.
 */
struct core_ap_auth_failure_count_s
    {
    core_mac_address_s bssid; ///< BSSID of the AP.
    u8_t count;               ///< The number of authentication failures.
    };

/**
 * Structure for storing AP power save test verdicts.
 */
struct core_ap_power_save_test_verdict_s
    {
    core_mac_address_s bssid; ///< BSSID of the AP.
    bool_t is_success;        ///< Whether power save test was successful
    };

/**
 * Class holding all connection related data
 * @lib wlmserversrv.lib
 * @since S60 v3.1
 */
NONSHARABLE_CLASS( core_connection_data_c )
    {

public:

    /**
     * Constructor
     */
    core_connection_data_c(
        const core_iap_data_s& iap_data,
        const core_device_settings_s& device_settings );

    /**
     * Destructor
     */
    ~core_connection_data_c();

    /**
     * Return IAP specific data for the current connection.
     *
     * @since S60 v3.1
     * @return IAP specific data of the connection
     */
    core_iap_data_c& iap_data();

    /**
     * connect_status
     * 
     * @since S60 v3.1
     * @return connect status of the connection
     */
    core_connect_status_e connect_status() const;

    /**
     * set_connect_status
     *
     * @since S60 v3.1
     * @param connect_status specifying the status of the connection
     * (either OK or an error code)
     */
    void set_connect_status(
        core_connect_status_e connect_status );

    /**
     * set_current_ap_data
     * 
     * @since S60 v3.1
     * @param access point specific data
     */
    void set_current_ap_data(
        const core_ap_data_c& ap_data );

    /**
     * current_ap_data
     * 
     * @since S60 v3.1
     * @return access point specific data
     */
    core_ap_data_c* current_ap_data();

    /**
     * set_previous_ap_data
     *
     * @since S60 v3.1
     * @param access point specific data
     */
    void set_previous_ap_data(
        const core_ap_data_c& ap_data );

    /**
     * previous_ap_data
     *
     * @since S60 v3.1
     * @return access point specific data
     */
    core_ap_data_c* previous_ap_data();
    
    /**
     * ssid
     *
     * @since S60 v3.1
     * return SSID of the connection
     */
    const core_ssid_s& ssid() const;
    
    /**
     * set_ssid
     *
     * @since S60 v3.1
     * @param SSID of the connection
     */
    void set_ssid( const core_ssid_s& ssid );

    /**
     * Check whether the given MAC address is permanently blacklisted.
     *
     * @since S60 v3.1
     * @param mac MAC address to be checked.
     * @return Whether the given MAC address is permanently blacklisted.
     */    
    bool_t is_mac_in_temporary_blacklist(
        const core_mac_address_s& mac );

    /**
     * Add a MAC address to the permanent blacklist.
     *
     * @since S60 v3.1
     * @param mac MAC address to be added.
     * @param reason Reason for the blacklist.
     */           
    void add_mac_to_temporary_blacklist(
        const core_mac_address_s& mac,
        core_ap_blacklist_reason_e reason );

    /**
     * Remove a MAC address from the permanent blacklist.
     *
     * @since S60 v3.1
     * @param mac MAC address to be removed.
     */    
    void remove_mac_from_temporary_blacklist(
        const core_mac_address_s& mac );

    /**
     * Return the temporary blacklist.
     *
     * @since S60 v3.2
     * @return The temporary blacklist.
     */
    core_type_list_c<core_ap_blacklist_entry_s>& temporary_blacklist();

    /**
     * Remove the entries blacklisted with one of the given reasons.
     *
     * @since S60 v3.1
     * @param reasons Bitmap of reasons to remove.
     */
    void remove_temporary_blacklist_entries(
        u32_t reasons );

    /**
     * Return the last RCPI class notified to the clients.
     *
     * @since S60 v3.1
     * @return The last RCPI class notified to the clients.
     */
    core_rcp_class_e last_rcp_class() const;

    /**
     * Set the last RCPI class notified to the clients.
     *
     * @since S60 v3.1
     * @param type The last RCPI class notified to the clients.
     */
    void set_last_rcp_class(
        core_rcp_class_e type );

    /**
     * Return the last TX level notified to the clients.
     *
     * @since S60 v3.1
     * @return The last TX level notified to the clients.
     */
    u32_t last_tx_level() const;

    /**
     * Set the last TX level notified to the clients.
     *
     * @since S60 v3.1
     * @param type The last TX level notified to the clients.
     */
    void set_last_tx_level(
        u32_t tx_level );

    /**
     * Return the last BSSID notified to the clients.
     *
     * @since S60 v3.1
     * @return The last BSSID notified to the clients.
     */
    const core_mac_address_s& last_bssid() const;

    /**
     * Set the last BSSID notified to the clients.
     *
     * @since S60 v3.1
     * @param type The last BSSID notified to the clients.
     */
    void set_last_bssid(
        const core_mac_address_s& bssid );

    /**
     * Return the connection state.
     * NOTE: This should only be called when there exists some connection.
     *
     * @since S60 v3.1
     * @return type of the connection: {secure|insecure} {infrastructure|ad hoc} network
     */    
    core_connection_state_e connection_state() const;
    
    /**
     * Return the last connection state notified to the clients.
     * NOTE: This should not be considered as the actual/official WLAN connection state
     *       (use core_settings_c::connection_state() instead).
     *
     * @since S60 v3.1
     * @return The last connection state notified to the clients.
     */
    core_connection_state_e last_connection_state() const;

    /**
     * Set the last connection state notified to the clients.
     * NOTE: This should not be considered as the actual/official WLAN connection state
     *       (use core_settings_c::set_connection_state() instead).
     *
     * @since S60 v3.1
     * @param type The last connection state notified to the clients.
     */
    void set_last_connection_state(
        core_connection_state_e state );

    /**
     * Return the number of association failures to the given AP.
     *
     * @param bssid BSSID of the AP to check.
     * @return The number of association failures.
     */
    u8_t ap_association_failure_count(
        const core_mac_address_s& bssid );

    /**
     * Increase the given AP association failure count by one.
     *
     * @param bssid BSSID of the AP.
     */
    void increase_ap_association_failure_count(
        const core_mac_address_s& bssid );

    /**
     * Clear the association failure count for the given AP.
     */
    void clear_ap_association_failure_count(
        const core_mac_address_s& bssid );
    
    /**
     * Return the number of authentication failures to the given AP.
     *
     * @param bssid BSSID of the AP to check.
     * @return The number of association failures.
     */
    u8_t ap_authentication_failure_count(
        const core_mac_address_s& bssid );

    /**
     * Increase the given AP authentication failure count by one.
     *
     * @param bssid BSSID of the AP.
     */
    void increase_ap_authentication_failure_count(
        const core_mac_address_s& bssid );

    /**
     * Clear all authentication failure counts.
     */
    void clear_all_authentication_failure_counts();


    /**
     * Return the number of deauthentications.
     *
     * @param bssid BSSID of the AP to check.
     * @return The number of deauthentications.
     */
    u8_t ap_deauthentication_count(
        const core_mac_address_s& bssid );

    /**
     * Increase the given AP's deauthentication count.
     *
     * @param bssid BSSID of the AP.
     */
    void increase_ap_deauthentication_count(
        const core_mac_address_s& bssid );

    /**
     * Get the last failed EAP-type.
     *
     * @since S60 v3.1
     * @return The last failed EAP-type.
     */
    u32_t last_failed_eap_type() const;

    /**
     * Set the last failed EAP-type.
     *
     * @since S60 v3.1
     * @param type The last failed EAP-type.
     */
    void set_last_failed_eap_type(
        u32_t type );
    
    /**
     * Get the last EAP-specific error code.
     *
     * @since S60 v3.1
     * @return The last EAP-specific error code.     
     */    
    u32_t last_eap_error() const;

    /**
     * Set the last EAP-specific error code.
     *
     * @since S60 v3.1
     * @param error The last EAP-specific error code.
     */
    void set_last_eap_error(
        u32_t error );    
    
    /**
     * Return a channel mask of suitable APs based on the
     * information received from an AP.
     *
     * @since S60 v3.1
     * @return A channel mask of suitable APs.
     */
    const core_scan_channels_c& adjacent_ap_channels() const;
    
    /**
     * Store a channel mask of suitable APs based on the
     * information received from an AP.
     *
     * @since S60 v3.1
     * @param channels A channel mask of suitable APs.  
     */    
    void set_adjacent_ap_channels(
        const core_scan_channels_c& channels );    

    /**
     * Merge a channel mask of suitable APs based on the
     * information received from an AP.
     *
     * @since S60 v5.2
     * @param channels A channel mask of suitable APs.  
     */    
    void merge_adjacent_ap_channels(
        const core_scan_channels_c& channels );    

    /**
     * Check whether EAPOL authentication has been started.
     *
     * @since S60 v3.1
     * @return Whether EAPOL authentication has been started.
     */
    bool_t is_eapol_authentication_started() const;

    /**
     * Set the status of EAPOL authentication.
     *
     * This status indicates whether WLAN engine has called
     * start_reassociation() or start_authentication().
     *
     * @since S60 v3.1
     * @param is_authentication_started Whether EAPOL authentication has been started.
     */    
    void set_eapol_authentication_started(
        bool_t is_authentication_started );

    /**
     * Check whether EAPOL is running an authentication.
     *
     * @since S60 v3.1
     * @return Whether EAPOL is running an authentication.
     */    
    bool_t is_eapol_authenticating() const;
    
    /**
     * Set the status of EAPOL authentication.
     *
     * This status indicates whether EAPOL has signaled it has
     * started running EAP authentication and/or 4-way handshake.
     *
     * @since S60 v3.1
     * @param is_authenticating Whether EAPOL is running an authentication.
     */
    void set_eapol_authenticating(
        bool_t is_authenticating );

    /**
     * Get the BSSID currently being authenticated against.
     *
     * @since S60 v3.1
     * @return The BSSID currently being authenticated against. 
     */
    const core_mac_address_s& eapol_auth_bssid() const;

    /**
     * Get the BSSID currently being authenticated against.
     *
     * @since S60 v3.1
     * @param bssid The BSSID currently being authenticated against. 
     */    
    void set_eapol_auth_bssid(
        const core_mac_address_s& bssid );

    /**
     * Get the status of the ongoing authentication.
     *
     * @since S60 v3.1
     * @return The status of the ongoing authentication. 
     */
    core_error_e eapol_auth_failure() const;

    /**
     * Set the status of the ongoing authentication.
     *
     * @since S60 v3.1
     * @param error The status of the ongoing authentication. 
     */
    void set_eapol_auth_failure(
        core_error_e error );

    /**
     * Return the list of active traffic streams.
     *
     * @since S60 v3.2
     * @return The list of active traffic streams.
     */
    core_traffic_stream_list_c& traffic_stream_list();

    /**
     * Return the list of virtual traffic streams.
     *
     * @since S60 v3.2
     * @return The list of active traffic streams.
     */
    core_virtual_traffic_stream_list_c& virtual_traffic_stream_list();
    
    /**
     * Check whether EAPOL is connecting.
     *
     * @since S60 v3.1
     * @return Whether EAPOL is connecting.
     */    
    bool_t is_eapol_connecting() const;
    
    /**
     * Set the status of EAPOL connection.
     *
     * @since S60 v3.1
     * @param is_eapol_connecting Whether EAPOL is connecting.
     */
    void set_eapol_connecting(
        bool_t is_eapol_connecting );

    /**
     * Check whether EAPOL is disconnecting.
     *
     * @since S60 v3.1
     * @return Whether EAPOL is disconnecting.
     */    
    bool_t is_eapol_disconnecting() const;
    
    /**
     * Set the status of EAPOL disconnecting.
     *
     * @since S60 v3.1
     * @param is_eapol_disconnecting Whether EAPOL is disconnecting.
     */
    void set_eapol_disconnecting(
        bool_t is_eapol_disconnecting );
    
    /**
     * Check whether disconnection is ongoing.
     *
     * @since S60 v3.2
     * @return Whether disconnection is ongoing.
     */    
    bool_t is_disconnecting() const;
    
    /**
     * Set the status of disconnection.
     *
     * @since S60 v3.2
     * @param is_disconnecting Whether disconnection is ongoing.
     */
    void set_disconnecting(
        bool_t is_disconnecting );

    /**
     * Get the current RCPI roam check interval values.
     *
     * @since S60 v3.2
     * @return The current RCPI roam check interval values.
     */
    const core_rcpi_roam_interval_s& rcpi_roam_interval() const;

    /**
     * Set the current RCPI roam check interval values back to initial values.
     *
     * @since S60 v3.2
     */
    void reset_rcpi_roam_interval();

    /**
     * Set the current RCPI roam check interval values.
     *
     * @since S60 v3.2
     * @param interval The current RCPI roam check interval values.
     */
    void set_core_rcpi_roam_interval(
        const core_rcpi_roam_interval_s& interval );

    /**
     * Get the reason for the last roam.
     *
     * @since S60 v3.2
     * @return The reason for the last roam.
     */    
    core_roam_reason_e last_roam_reason() const;

    /**
     * Set the reason for the last roam.
     *
     * @since S60 v3.2
     * @param reason The reason for the last roam.
     */    
    void set_last_roam_reason(
        core_roam_reason_e reason );

    /**
     * Get the reason for the last roam failure.
     *
     * @since S60 v3.2
     * @return The reason for the last roam failure.
     */    
    core_roam_failed_reason_e last_roam_failed_reason() const;

    /**
     * Set the reason for the last roam failure.
     *
     * @since S60 v3.2
     * @param reason The reason for the last roam failure.
     */    
    void set_last_roam_failed_reason(
        core_roam_failed_reason_e reason );

    /**
     * Check whether power save test has been run on the given AP.
     *
     * @param bssid BSSID of the AP to check.
     * @param is_success Whether the test was successful or not.
     * @return Whether power save test has been run on the given AP.
     */
    bool_t is_ap_power_save_test_run(
        const core_mac_address_s& bssid,
        bool_t& is_success );

    /**
     * Set the power save test verdict on the given the AP.
     *
     * @param bssid BSSID of the AP.
     * @param is_success Whether the test was successful or not.
     */
    void add_ap_power_save_test_verdict(
        const core_mac_address_s& bssid,
        bool_t is_success );

    /**
     * Check whether EAPOL requires to do new connect right after failed authentication.
     *
     * @since S60 v3.2
     * @return Whether immediate reconnect is required.
     */
    bool_t is_eapol_require_immediate_reconnect() const;
    
    /**
     * Set the state that EAPOL requires to do new connect right after failed authentication.
     *
     * @since S60 v3.2
     * @param is_immediate_reconnect_required Whether EAPOL requires immediate reconnect 
     *                                        after failed authentication.
     */
    void set_eapol_require_immediate_reconnect(
        bool_t is_immediate_reconnect_required );
    
    /**
     * Check whether voice call is currently on.
     *
     * @since S60 v5.2
     * @return Whether voice call is on.
     */
    bool_t voice_call_state() const;
    
    /**
     * Set voice call state.
     *
     * @since S60 v5.2
     * @param is_voice_call_on Voice call state to set.
     */
    void set_voice_call_state( const bool_t& is_voice_call_on );
    
    /**
     * Get previous RRM Measurement Request time.
     * 
     * @since S60 v5.2
     * @return Timestamp when the previous request
     *         was received.
     */
    const u64_t& get_previous_rrm_measurement_request_time() const;
    
    /**
     * Set previous RRM Measurement Request time.
     * 
     * @since S60 v5.2
     * @param timestamp Timestamp to set.
     */
    void set_previous_rrm_measurement_request_time( const u64_t& timestamp );

    /**
     * Get the current traffic status for the given access class.
     * 
     * @since S60 v5.2
     * @param access_class Access class to get.
     * @return Current traffic status for the given access class.
     */
    core_access_class_traffic_status_e ac_traffic_status(
        core_access_class_e access_class ) const;

    /**
     * Set the current traffic status for the given access class.
     *
     * @since S60 v5.2
     * @param access_class Access class to set.
     * @param status Traffic status to set.
     */
    void set_ac_traffic_status(
        core_access_class_e access_class,
        core_access_class_traffic_status_e status );

    /**
     * Get the current traffic mode for the given access class.
     * 
     * @since S60 v5.2
     * @param access_class Access class to get.
     * @return Current traffic mode for the given access class.
     */
    core_access_class_traffic_mode_e ac_traffic_mode(
        core_access_class_e access_class ) const;
    
    /**
     * Set the current traffic mode for the given access class.
     *
     * @since S60 v5.2
     * @param access_class Access class to set.
     * @param status Traffic mode to set.
     */    
    void set_ac_traffic_mode(
        core_access_class_e access_class,
        core_access_class_traffic_mode_e mode );

private: // data

    /** Adaptation side data about the WLAN connection */
    core_iap_data_c iap_data_m;

    /** Handle to device settings. */
    const core_device_settings_s& device_settings_m;

    /** Connect status from adaptation */
    core_connect_status_e connect_status_m;

    /** Previous AP data */
    core_ap_data_c* previous_ap_m;

    /** Current AP data */
    core_ap_data_c* current_ap_m;

    /** SSID */
    core_ssid_s ssid_m;

    /** Temporary blacklist of APs. */
    core_type_list_c<core_ap_blacklist_entry_s> temp_blacklist_m;
    
    /** The RCPI class last notified. */
    core_rcp_class_e last_rcp_class_m;
    
    /** The TX level last notified. */
    u32_t last_tx_level_m;
    
    /** The BSSID last notified. */
    core_mac_address_s last_bssid_m;

    /** The connection state last notified. */
    core_connection_state_e last_connection_state_m;

    /** List for storing AP association failure counts. */
    core_type_list_c<core_ap_failure_count_s> association_failure_count_m;
    
    /** List for storing AP authentication failure counts. */
    core_type_list_c<core_ap_auth_failure_count_s> authentication_failure_count_m;

    /** List for storing AP deauthentication counts. */
    core_type_list_c<core_ap_failure_count_s> deauthentication_count_m;

    /** The last failed EAP type. */
    u32_t last_eap_type_m;

    /** The last EAP-specific error code. */
    u32_t last_eap_error_code_m;

    /** A channel mask of suitable APs. */
    core_scan_channels_c adjacent_ap_channels_m;

    /** Whether EAPOL authentication has been started. */
    bool_t is_eapol_authentication_started_m;

    /** Whether EAPOL is running an authentication. */
    bool_t is_eapol_authenticating_m;

    /** The BSSID currently being authenticated against. */
    core_mac_address_s eapol_auth_bssid_m;

    /** Status of the currently ongoing authentication. */
    core_error_e eapol_auth_failure_m;

    /** List of active traffic streams. */
    core_traffic_stream_list_c traffic_stream_list_m;

    /** List of virtual traffic streams. */
    core_virtual_traffic_stream_list_c virtual_traffic_stream_list_m;

    /** Whether EAPOL is connecting. */
    bool_t is_eapol_connecting_m;

    /** Whether EAPOL is disconnecting. */
    bool_t is_eapol_disconnecting_m;

    /** Whether disconnection is ongoing. */
    bool_t is_disconnecting_m;

    /** The reason for the last roam. */
    core_roam_reason_e last_roam_reason_m;

    /** The reason for the last roam failure. */
    core_roam_failed_reason_e last_roam_failed_reason_m;

    /** The current RCPI roam check interval values. */
    core_rcpi_roam_interval_s rcpi_roam_interval_m;

    /** Power save test verdicts. */    
    core_type_list_c<core_ap_power_save_test_verdict_s> power_save_test_verdict_m;

    /** Whether EAPOL requires to do new connect right after failed authentication. */
    bool_t is_eapol_require_immediate_reconnect_m;
    
    /** Whether voice call is on. */
    bool_t is_voice_call_on_m;
    
    /** Previous RRM Measurement Request time. */
    u64_t previous_rrm_measurement_request_time_m;

    /** Current traffic status for access classes. */
    core_access_class_traffic_status_e ac_traffic_status_m[MAX_QOS_ACCESS_CLASS];

    /** Current traffic mode for access classes. */
    core_access_class_traffic_mode_e ac_traffic_mode_m[MAX_QOS_ACCESS_CLASS];
    };

#endif // CORE_CONNECTION_DATA_H
