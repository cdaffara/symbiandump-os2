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
* Description:  Main class for core server
*
*/

/*
* %version: 56 %
*/

#ifndef CORE_SERVER_H
#define CORE_SERVER_H

#include "am_platform_libraries.h"
#include "abs_core_server.h"
#include "abs_core_server_callback.h"
#include "abs_core_driverif.h"
#include "abs_core_driverif_callback.h"
#include "core_operation_base.h"
#include "core_type_list.h"
#include "core_settings.h"
#include "core_connection_data.h"
#include "core_timer_counter_measures.h"
#include "core_scan_list.h"
#include "abs_core_wpx_adaptation.h"
#include "core_operation_handle_bss_lost.h"
#include "core_wlan_eapol_interface.h"

class ScanList;
class abs_core_timer_c;
class abs_wlan_eapol_callback_interface_c;
class abs_core_frame_handler_c;
class abs_core_event_handler_c;
class abs_core_protected_setup_handler_c;
class core_eapol_handler_c;

/**
 * core server class for WLAN management functionality.
 *
 * @lib wlmserversrv.lib
 * @since S60 v3.1
 */
NONSHARABLE_CLASS( core_server_c ) :
    public abs_core_server_c,
    public abs_core_driverif_callback_c
    {

public: 

    /**
     * Constructor.
     *
     * @param callback External service interface.
     * @param drivers Driver interface
     * @param settings Wlan Device Settings
     * @param mac_address MAC address of the device.
     * @param features Bitmask of enabled features.
     */
    core_server_c(
        abs_core_server_callback_c& callback,
        abs_core_driverif_c& drivers,
        const core_device_settings_s& settings,
        const core_mac_address_s& mac_address,
        u32_t features );

    /**
     * Destructor.
     */
    virtual ~core_server_c();

    /**
     * Perform the necessary initialization procedures after construction.
     * 
     * @since S60 v3.1
     * @return An error code if initialization fails, core_error_ok otherwise.
     * @note This method MUST be called after the construction of this object.
     */
    core_error_e init();

    /**
     * Get a reference to device settings.
     *
     * @since S60 v3.1
     * @return Reference to device settings.
     */
    core_device_settings_s& get_device_settings();
   
    /**
     * Get a reference to core engine settings.
     *
     * @since S60 v3.1
     * @return Reference to core engine settings.
     */   
    core_settings_c& get_core_settings();
    
    /**
     * Get a reference to connection data.
     *
     * @since S60 v3.1
     * @return Pointer to connection data.
     */   
    core_connection_data_c* get_connection_data();

    /**
     * Get a reference to EAPOL instance.
     *
     * @since S60 v3.1
     * @return Reference to EAPOL instance.
     */   
    core_wlan_eapol_interface_c& get_eapol_instance();
   
    /**
     * Direct the callbacks from EAPOL to the given handler.
     *
     * @since S60 v3.1
     * @param New handler of EAPOL callbacks.
     */   
    void set_eapol_handler(
        abs_wlan_eapol_callback_interface_c* handler );
   
    /**
     * Return the device MAC address.
     *
     * @since S60 v3.1
     * @return The device MAC address.
     */        
    core_mac_address_s& own_mac_addr();

    /**
     * Queue the given internal operation. This method should be used
     * when queueing operations from inside other operations.
     *
     * @param operation Operation to be queued.
     * @return core_error_ok if successfully queued, an error otherwise.
     * @note Ownership of the operation instance is transferred immediately
     *       to this method. If there's an error, the instance is automatically
     *       deleted.
     */
    core_error_e queue_int_operation(
        core_operation_base_c* operation );

    /**
     * Queue the given internal operation and run the next operation if
     * no operation is executing.
     *
     * @param operation Operation to be queued.
     * @return core_error_ok if successfully queued, an error otherwise.
     * @note Ownership of the operation instance is transferred immediately
     *       to this method. If there's an error, the instance is automatically
     *       deleted.
     */
    core_error_e queue_int_operation_and_run_next(
        core_operation_base_c* operation );

    /**
     * Check whether WPA counter measures are active.
     *
     * @since S60 v3.1
     * @return Whether WPA counter measures are active.
     */
    bool_t is_cm_active();

    /**
     * Initialize the connection related data structures.
     * @param iap_data The settings used for establishing the connection.
     * @param device_settings Handle to device settings.
     * @since S60 v3.1
     */
    core_error_e init_connection_data(
        const core_iap_data_s& iap_data,
        const core_device_settings_s& device_settings );

    /**
     * Cleans up connection related data structures from core engine
     * @since S60 v3.1
     */
    void clear_connection_data();

    /**
     * Schedule roaming timer.
     *
     * @since S60 v3.1
     * @param delay Expiration time in micro seconds.
     */    
    void schedule_roam_timer(
        u32_t delay );

    /**
     * Cancel the roaming timer.
     *
     * @since S60 v3.1
     */
    void cancel_roam_timer();
    
    /**
     * Schedule operation timer.     
     *
     * @since S60 v3.1
     * @param delay Expiration time in micro seconds.
     * @note This timer should never be used while running a sub-operation
     *       since it's possible the sub-operation will also try to the use
     *       the same timer.
     */    
    void schedule_operation_timer(
        u32_t delay );

    /**
     * Cancel the operation timer.
     *
     * @since S60 v3.1
     */
    void cancel_operation_timer();

    /**
     * Check whether DHCP timer is active.
     * 
     * @since S60 v5.0
     * @return true_t if timer is active, false_t otherwise.
     */
    bool_t is_dhcp_timer_active();

    /**
     * Schedule the DHCP timer.
     *
     * @since S60 v3.1
     * @param delay Expiration time in micro seconds.
     */    
    void schedule_dhcp_timer(
        u32_t delay );

    /**
     * Cancel the DHCP timer.
     *
     * @since S60 v3.1
     */
    void cancel_dhcp_timer();

    /**
     * Schedule the driver unload timer.
     * 
     * @since S60 v3.1
     * @param delay Expiration time in micro seconds.
     */
    void schedule_unload_timer(
        u32_t delay );

    /**
     * Cancel the driver unload timer.
     *
     * @since S60 v3.1
     */
    void cancel_unload_timer();

    /**
     * Check whether an operation with the given flags is in queue.
     *
     * @param feature_flags Feature flags to check for.
     * @return Whether an operation with the given flags is in queue.
     */
    bool_t is_operation_in_queue_with_flags(
        u32_t feature_flags );

    /**
     * Check whether an operation of the given type is in queue.
     *
     * @param type Type of the operation to search for.
     * @return Whether an operation of the given type is in queue.
     */
    bool_t is_operation_in_queue_with_type(
        u32_t type );

    /**
     * Cancels all the operations in the queue that have
     * the given feature flags enabled. Does not touch
     * the currently executing operation.
     *
     * @since S60 v3.1
     * @param feature_flags Feature flags to check for.
     */    
    void cancel_operations_with_flags(
        u32_t feature_flags );

    /**
     * Cancels all the operations of the given type in the queue.
     * Does not touch the currently executing operation.
     *
     * @since S60 v3.2
     * @param type Operation type to check for.
     */    
    void cancel_operations_with_type(
        u32_t type );

    /**
     * Cancels all the operations in the queue including
     * the currently executing operation.
     *
     * @since S60 v3.2
     * @param is_graceful_cancel Whether cancel should be graceful or forced.     
     */
    void cancel_all_operations(
        bool_t is_graceful_cancel );

    /**
     * Creates an instance of EAPOL if needed.
     *
     * @since S60 v3.1
     * @param mode Operating mode to use.
     * @return true_t if an instance is successfully created or it already exists, false_t otherwise.
     */
    bool_t create_eapol_instance(
        core_eapol_operating_mode_e mode );

    /**
     * Return the current frame handler.
     *
     * @since S60 v3.2
     * @return Pointer to the current frame handler.
     */
    abs_core_frame_handler_c* frame_handler();

    /**
     * Register a handler for received frames.
     *
     * @since S60 v5.0
     * @param handler Pointer to the new handler.
     */
    void register_frame_handler(
        abs_core_frame_handler_c* handler );

    /**
     * Unregister a handler for received frames.
     *
     * @since S60 v5.0
     * @param handler Pointer to the registered handler.
     */
    void unregister_frame_handler(
        abs_core_frame_handler_c* handler );

    /**
     * Return a reference to the scan list.
     *
     * @since S60 v3.1
     * @return A reference to the scan list.
     */
    core_scan_list_c& get_scan_list();

    /**
     * Return the current event handler.
     *
     * @since S60 v3.2
     * @return Pointer to the current event handler.
     */
    abs_core_event_handler_c* event_handler();

    /**
     * Register a handler for received indications.
     *
     * @since S60 v5.0
     * @param handler Pointer to the new handler.
     */
    void register_event_handler(
        abs_core_event_handler_c* handler );

    /**
     * Unregister a handler for received indications.
     *
     * @since S60 v5.0
     * @param handler Pointer to the registered handler.
     */
    void unregister_event_handler(
        abs_core_event_handler_c* handler );

    /**
     * Forces the engine to roam to any available AP.
     * 
     * @since S60 v3.1
     * @param reason Reason to roam.
     */
    void schedule_roam(
        core_operation_handle_bss_lost_c::core_bss_lost_reason_e reason );

    /**
     * Send a management frame.
     *
     * @since S60 v3.2
     * @param frame_type The type of the frame to send.
     * @param frame_length The length of the frame.
     * @param frame_data Pointer to the frame data.
     * @param destination Destination MAC address.
     * @return true_t if frame was successfully sent, false_t otherwise.
     */
    bool_t send_management_frame(
        core_frame_type_e frame_type,
        const u16_t frame_length,
        const u8_t* const frame_data,
        const core_mac_address_s& destination );

    /**
     * Send a data frame.
     *
     * @since S60 v3.2
     * @param ap_data AP data is being sent to.
     * @param frame_type The type of the frame to send.
     * @param frame_length The length of the frame.
     * @param frame_data Pointer to the frame data.
     * @param frame_priority Desired priority of the frame.
     * @param destination Destination MAC address.
     * @param send_unencrypted Whether the frame must be sent unencrypted.    
     * @return true_t if frame was successfully sent, false_t otherwise.
     */
    bool_t send_data_frame(
        const core_ap_data_c& ap_data,
        core_frame_type_e frame_type,
        const u16_t frame_length,
        const u8_t* const frame_data,
        core_access_class_e frame_priority,
        const core_mac_address_s& destination,
        bool_t send_unencrypted = false_t );

    /**
     * Get a reference to WPX adaptation instance.
     *
     * @since S60 v3.1
     * @return Reference to WPX adaptation instance.
     */   
    abs_core_wpx_adaptation_c& get_wpx_adaptation_instance();

    /**
     * Direct the Protected Setup events to the given handler.
     *
     * @since S60 v3.2
     * @param New handler of Protected Setup events.
     */  
    void set_protected_setup_handler(
        abs_core_protected_setup_handler_c* handler );

public: // abs_core_server_c

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
    void connect(
        u32_t request_id,
        const core_iap_data_s& settings,
        core_connect_status_e& connect_status,
        core_type_list_c<core_ssid_entry_s>* ssid_list = NULL );

    /**
     * Disconnect an active connection.
     * 
     * @since S60 v3.1
     * @param request_id An unique identification for the request.
     */
    void release(
        u32_t request_id );

    /**
     * Request available networks with the given SSID.
     * 
     * @since S60 v3.1
     * @param request_id An unique identification for the request.
     * @param scan_mode Defines whether active or passive scan is performed.
     * @param scan_ssid Name of the network to scan for.
     * @param scan_channels Channels to scan.
     * @param scan_max_age Maximum age of returned scan results in seconds.     
     * @param scan_data Scan results are stored here.
     * @param is_current_ap_added Whether the current AP needs to be added during an ongoing
     *                            connection if not otherwise found in the scan.
     * @note If the length of the SSID is zero, a broadcast scan is performed.
     */
    void get_scan_result(
        u32_t request_id,
        core_scan_mode_e scan_mode,
        const core_ssid_s& scan_ssid,
        const core_scan_channels_s& scan_channels,
        u8_t scan_max_age,
        ScanList& scan_data,
        bool_t is_current_ap_added = true_t );

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
    void get_available_iaps(
        u32_t request_id,
        bool_t is_active_scan_allowed,
        core_type_list_c<core_iap_data_s>& iap_data_list,
        core_type_list_c<core_iap_availability_data_s>& iap_availability_list,
        core_type_list_c<core_ssid_entry_s>* iap_ssid_list,
        ScanList& scan_data );

    /**
     * Get the current RCPI value of the connection.
     * 
     * @since S60 v3.1
     * @param request_id An unique identification for the request.
     * @param rcpi Current channel power indicator (RCPI).
     */
    void get_current_rcpi(
        u32_t request_id,
        u32_t& rcpi );

    /**
     * Disable all WLAN activity.
     *
     * Disconnects the active connection and prevents any further
     * WLAN activity until enable_wlan() is called.
     *
     * @since S60 v3.1
     * @param request_id An unique identification for the request.
     */        
    void disable_wlan(
        u32_t request_id );
        
    /**
     * Allow WLAN activity after disabled_wlan().
     *
     * @since S60 v3.1     
     * @param request_id An unique identification for the request.
     */
    void enable_wlan(
        u32_t request_id );

    /**
     * Schedule an immediate driver unload.
     *
     * @since S60 v3.1     
     * @return core_error_ok if success, an error code otherwise.
     */
    core_error_e unload_drivers();

    /**
     * Get packet statistics of the current connection.
     *
     * @since S60 v3.2
     * @param request_id An unique identification for the request.
     * @param statistics Packet statistics of the current connection.     
     */
    void get_packet_statistics(
        u32_t request_id,
        core_packet_statistics_s& statistics );

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
    void create_traffic_stream(
        u32_t request_id,
        u8_t tid,
        u8_t user_priority,
        bool_t is_automatic_stream,
        const core_traffic_stream_params_s& params,
        u32_t& stream_id,
        core_traffic_stream_status_e& stream_status );

    /**
     * Request deletion of a virtual traffic stream.
     *
     * @since S60 v3.2
     * @param request_id An unique identification for the request.
     * @param stream_id ID of the virtual traffic stream.
     */
    void delete_traffic_stream(
        u32_t request_id,
        u32_t stream_id );   

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
    void run_protected_setup(
        u32_t request_id,
        const core_iap_data_s& iap_data,
        core_type_list_c<core_iap_data_s>& iap_data_list,
        core_protected_setup_status_e& protected_setup_status );

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
    void directed_roam(
        u32_t request_id,
        const core_mac_address_s& bssid );    

    /**
     * Get the BSSID of the access point currently connected to.
     * 
     * @since S60 v3.1
     * @param bssid BSSID of the access point.
     * @return core_error_ok if success, an error code otherwise.
     */
    core_error_e get_current_bssid(
        core_mac_address_s& bssid );

    /**
     * Get the name of the network (SSID) currently connected to.
     * 
     * @since S60 v3.1
     * @param ssid Current SSID.
     * @return core_error_ok if success, an error code otherwise.
     */
    core_error_e get_current_ssid(
        core_ssid_s& ssid );

    /**
     * Get the current security mode of the connection.
     * 
     * @since S60 v3.1
     * @param mode Current security mode.
     * @return core_error_ok if success, an error code otherwise.
     */
    core_error_e get_current_security_mode(
        core_connection_security_mode_e& mode );

    /**
     * Get the current connection state.
     * 
     * @since S60 v3.1
     * @param state Current connection state.
     * @return core_error_ok if success, an error code otherwise.
     */
    core_error_e get_current_connection_state(
        core_connection_state_e& state );

    /**
     * Inform core server about updated device settings.
     * 
     * @since S60 v3.1     
     * @param settings Updated device settings.
     * @return core_error_ok if success, an error code otherwise.
     * @note This method does not guarantee that the new parameters
     * are taken into use immediately.     
     */
    core_error_e update_device_settings(
        core_device_settings_s& settings );

    /**
     * Set the preferred power save mode.
     * 
     * @since S60 v3.1
     * @param mode Preferred power save mode.
     * @return core_error_ok if success, an error code otherwise.
     * @note This method does not guarantee that the new parameters
     *       are taken into use immediately.
     */
    core_error_e set_power_save_mode(
        const core_power_save_mode_s& mode );

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
     * be cleared when the connection is closed.
     */
    core_error_e add_bssid_to_rogue_list(
        const core_mac_address_s& bssid );

    /**
     * Remove a BSSID from the rogue list.
     *
     * @since S60 v3.1
     * @param bssid BSSID to remove from the list.
     * @return core_error_ok if success, an error code otherwise.
     */
    core_error_e remove_bssid_from_rogue_list(
        const core_mac_address_s& bssid );

    /**
     * Return a list of BSSIDs on the rogue list.
     *
     * @since S60 v3.2
     * @param rogue_list List of BSSIDs on the rogue list.
     * @return core_error_ok if success, an error code otherwise.
     */
    core_error_e get_rogue_list(
        core_type_list_c<core_mac_address_s>& rogue_list );

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
     *                   the RCP level is rising vs. descending
     * @return core_error_ok if success, an error code otherwise.
     */
    core_error_e set_rcp_level_notification_boundary(
        const i32_t rcp_level_boundary,
        const i32_t hysteresis );

    /**
     * Clear packet statistics of the current connection.
     *
     * @since S60 v3.2
     * @return core_error_ok if success, an error code otherwise.
     */
    core_error_e clear_packet_statistics();

    /**
     * Get the current U-APSD settings.
     *
     * @since S60 v3.2
     * @param settings Current U-APSD settings.
     * @return core_error_ok if success, an error code otherwise.
     */
    core_error_e get_uapsd_settings(
        core_uapsd_settings_s& settings );

    /**
     * Set the U-APSD settings.
     *
     * @since S60 v3.2
     * @param settings Current U-APSD settings to be set.
     * @return core_error_ok if success, an error code otherwise.
     * @note This method does not guarantee that the new parameters
     * are taken into use immediately.     
     */
    core_error_e set_uapsd_settings(
        const core_uapsd_settings_s& settings );

    /**
     * Get the current power save settings.
     *
     * @since S60 v3.2
     * @param settings Current power save settings.
     * @return core_error_ok if success, an error code otherwise.
     */
    core_error_e get_power_save_settings(
        core_power_save_settings_s& settings );

    /**
     * Set the power save settings.
     *
     * @since S60 v3.2
     * @param settings Current power save settings to be set.
     * @return core_error_ok if success, an error code otherwise.
     * @note This method does not guarantee that the new parameters
     * are taken into use immediately.     
     */
    core_error_e set_power_save_settings(
        const core_power_save_settings_s& settings );

    /**
     * Get information about the current AP.
     *
     * @since S60 v3.2
     * @param info Information about the current AP.
     * @return core_error_ok if success, an error code otherwise.
     */
    core_error_e get_current_ap_info(
        core_ap_information_s& info ); 

    /**
     * Get roam metrics of the current connection.
     *
     * @since S60 v3.2
     * @param roam_metrics Roam metrics of the current connection.
     */
    void get_roam_metrics(
        core_roam_metrics_s& roam_metrics );

    /**
     * Set the arp filter.
     *
     * @since S60 v5.0
     * @param filter Current ARP filter to be set.
     * @return core_error_ok if success, an error code otherwise.
     * @note This method does not guarantee that the new filter
     *       is taken into use immediately.
     */
    core_error_e set_arp_filter(
        const core_arp_filter_s& filter );

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
    core_error_e configure_multicast_group(
        bool_t join_group,
        const core_mac_address_s& multicast_addr );

    /**
     * Get information about current traffic statuses and traffic modes for
     * access classes.
     *
     * @since S60 v3.2
     * @param info Information about the current traffic statuses and traffic modes.
     * @return core_error_ok if success, an error code otherwise.
     */
    core_error_e get_current_ac_traffic_info(
        core_ac_traffic_information_s& info );

    /**
     * Set the currently enabled feature set.
     *
     * @since S60 v5.2
     * @param features Bitmask of enabled features.
     * @return core_error_ok if success, an error code otherwise.
     */
    core_error_e set_enabled_features(
        u32_t features );

    /**
     * An asynchronous request from the core server has been completed.
     *
     * @since S60 v3.1
     * @param request_id Identification of the corresponding request.
     * @param status Completion status of the request.
     */
    void request_complete(
        u32_t request_id,
        core_error_e status );

    /**
     * Cancel a pending asynchronous request.
     * 
     * @since S60 v3.1
     * @param request_id Identification of the corresponding request.
     */
    void cancel_request(
        u32_t request_id );

public: // abs_core_driverif_callback_c

    /**
     * Inform the core server about a new received frame.
     *
     * @since S60 v3.1
     * @param frame_type The type of the frame received.
     * @param frame_length The length of the frame.
     * @param frame_data Pointer to the frame data.     
     * @param frame_rcpi RCPI value of the frame.
     */
    void receive_frame(
        core_frame_type_e frame_type,
        const u16_t frame_length,
        const u8_t* const frame_data,
        u8_t frame_rcpi );

    /**
     * Notify the core server about an adaptation layer event.
     *
     * @note received notification is never related to an existing operation,
     * but it may lead to starting a new one.
     *
     * @since S60 v3.1
     * @param indication Adaptation layer event.
     */
    void notify(
        core_am_indication_e indication );

private:

    /**
     * Called by the timer framework when timer expires.
     * @param pointer to 'this' (note: the static method)
     */
    static void queue_timer_expired(
        void* this_ptr );

    /**
     * Called by the driver unload timer when it expires
     * @param pointer to 'this' (note: the static method)
     */
    static void unload_timer_expired(
        void* this_ptr );

    /**
     * Called by the roaming timer when it expires
     * @param pointer to 'this' (note: the static method)
     */
    static void roam_timer_expired(
        void* this_ptr );    

    /**
     * Called by the operation timer when it expires
     * @param pointer to 'this' (note: the static method)
     */
    static void operation_timer_expired(
        void* this_ptr );    

    /**
     * Called by the DHCP timer when it expires
     *
     * @since S60 v3.1
     * @param pointer to 'this' (note: the static method)
     */
    static void dhcp_timer_expired(
        void* this_ptr );    

    /**
     * Schedule the next operation to be run. If the queue is empty
     * or there's already a running operation, do nothing.
     */
    void schedule_operation();

    /**
     * Queue the given external operation and run the next operation if
     * no operation is executing.
     *
     * @param operation Operation to be queued.
     * @param request_id ID of the operation.
     * @note Ownership of the operation instance is transferred immediately
     *       to this method. If there's an error, the instance is automatically
     *       deleted.
     */
    void queue_ext_operation_and_run_next(
        core_operation_base_c* operation,
        u32_t request_id );

    /**
     * Notify EAPOL about a MIC failure.
     *
     * @param is_group_key_fail Is this a group or pairwise key failure.
     */
    void mic_failure(
        bool_t is_group_key_fail );

    /**
     * Queue an operation to unload the drivers.
     */
    void queue_unload_drivers();

private: // data

    abs_core_server_callback_c& callback_m;
    
    abs_core_driverif_c& drivers_m;

    /** Device settings. */
    core_device_settings_s device_settings_m;

    /** Queue for operations to be performed. */
    core_type_list_c<core_operation_base_c> queue_m;
    
    /** Timer for running queue operations. */
    abs_core_timer_c* queue_timer_m;
    
    /** Timer for unloading drivers */
    abs_core_timer_c* driver_unload_timer_m;
    
    /** Core engine settings and statuses. */
    core_settings_c core_settings_m;
    
    /** Connection data */
    core_connection_data_c* connection_data_m;
    
    /** Device MAC address. */
    core_mac_address_s own_mac_addr_m;
    
    /** Handler to EAPOL callbacks. */
    core_eapol_handler_c* eapol_handler_m;
    
    /** EAPOL */
    core_wlan_eapol_interface_c* eapol_m;

    /** Timer for WPA TKIP counter measures. */
    core_timer_counter_measures_c cm_timer_m;

    /** Timer for romaing. */
    abs_core_timer_c* roam_timer_m;

    /** Timer for currently active operation's purposes */
    abs_core_timer_c* operation_timer_m;

    /** Timer for enabling power save after DHCP is done. */
    abs_core_timer_c* driver_dhcp_timer_m;

    /** Handler for received frames. Not owned by this pointer. */
    abs_core_frame_handler_c* frame_handler_m;
    
    /** Handler for received indications. Not owned by this pointer. */
    abs_core_event_handler_c* event_handler_m;

    /** Scan list for storing beacons/probe responses. */
    core_scan_list_c scan_list_m;

    /** WPX adaptation instance. */
    abs_core_wpx_adaptation_c* wpx_adaptation_m;

    };

#endif // CORE_SERVER_H
