/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class encapsulating core engine settings and statuses.
*
*/

/*
* %version: 21 %
*/

#ifndef CORE_SETTINGS_H
#define CORE_SETTINGS_H

#include "core_types.h"
#include "core_type_list.h"
#include "core_roam_metrics.h"

/**
 * Class encapsulating core engine settings and statuses.
 *
 * @since S60 v3.1
 */
NONSHARABLE_CLASS( core_settings_c )
    {

public:

    /**
     * Constructor.
     * 
     * @param features Bitmask of enabled features.
     */
    core_settings_c(
        u32_t features );

    /**
     * Destructor.
     */
    virtual ~core_settings_c();

    /**
     * Check whether the WLAN drivers have been loaded.
     *
     * @since S60 v3.1
     * @return true_t if the WLAN drivers have been loaded,
     *         false_t otherwise.
     */
    bool_t is_driver_loaded() const;
    
    /**
     * Set the state of the WLAN drivers.
     *
     * @since S60 v3.1
     * @param is_driver_loaded true_t the WLAN drivers have been loaded,
     *                         false_t otherwise.
     */
    void set_driver_state(
        bool_t is_driver_loaded );

    /**
     * Check whether WLAN is enabled or disabled.
     *
     * @since S60 v3.1
     * @return true_t if enabled,
     *         false_t otherwise.
     */
    bool_t is_wlan_enabled() const;
    
    /**
     * Set WLAN state enabled or disabled.
     *
     * @since S60 v3.1
     * @param is_wlan_enabled true_t if enabling WLAN,
     *                        false_t otherwise.
     */
    void set_wlan_enabled(
        bool_t is_wlan_enabled );

    /**
     * Get the currently used power mode.
     *
     * @since S60 v3.1
     * @return Currently used power mode.
     */
    const core_power_mode_s& power_mode() const;

    /**
     * Set the currently used power mode.
     *
     * @since S60 v3.1
     * @param Currently used power mode.
     */    
    void set_power_mode(
        const core_power_mode_s& mode );

    /**
     * Get the preferred power save mode.
     *
     * @since S60 v3.1
     * @return Preferred power save mode.
     */
    const core_power_save_mode_s& preferred_power_save_mode() const;

    /**
     * Set the preferred power save mode.
     *
     * @since S60 v3.1
     * @param Preferred power save mode.
     */    
    void set_preferred_power_save_mode(
        const core_power_save_mode_s& mode );

    /**
     * Get current connection state.
     *
     * @since S60 v3.1
     * @return connection state.
     */
    core_connection_state_e connection_state() const;

    /**
     * Set current connection state.
     *
     * @since S60 v3.1
     * @param connection state.
     */    
    void set_connection_state(
        core_connection_state_e state );

    /**
     * Returns true/false based on connection state information
     *
     * @since S60 v3.1
     * @return true or false
     */
    bool_t is_connected() const;
    
    /**
     * Check whether the permanent whitelist is empty.
     *
     * @since S60 v3.1
     * @return Whether the permanent whitelist is empty.
     */    
    bool_t is_permanent_whitelist_empty() const;
    
    /**
     * Check whether the given MAC address is permanently whitelisted.
     *
     * @since S60 v3.1
     * @param mac MAC address to be checked.
     * @return Whether the given MAC address is permanently whitelisted.
     */    
    bool_t is_mac_in_permanent_whitelist(
        const core_mac_address_s& mac );

    /**
     * Check whether the given MAC address is permanently blacklisted.
     *
     * @since S60 v3.1
     * @param mac MAC address to be checked.
     * @return Whether the given MAC address is permanently blacklisted.
     */    
    bool_t is_mac_in_permanent_blacklist(
        const core_mac_address_s& mac );

    /**
     * Add a MAC address to the permanent whitelist.
     *
     * @since S60 v3.1
     * @param mac MAC address to be added.
     */    
    void add_mac_to_permanent_whitelist(
        const core_mac_address_s& mac );

    /**
     * Remove a MAC address from the permanent whitelist.
     *
     * @since S60 v3.1
     * @param mac MAC address to be removed.
     */    
    void remove_mac_from_permanent_whitelist(
        const core_mac_address_s& mac );

    /**
     * Add a MAC address to the permanent blacklist.
     *
     * @since S60 v3.1
     * @param mac MAC address to be added.
     * @param reason Reason for the blacklist.
     */           
    void add_mac_to_permanent_blacklist(
        const core_mac_address_s& mac,
        core_ap_blacklist_reason_e reason );

    /**
     * Remove a MAC address from the permanent blacklist.
     *
     * @since S60 v3.1
     * @param mac MAC address to be removed.
     */    
    void remove_mac_from_permanent_blacklist(
        const core_mac_address_s& mac );

    /**
     * Return the permanent blacklist.
     *
     * @since S60 v3.2
     * @return The permanent blacklist.
     */
    core_type_list_c<core_ap_blacklist_entry_s>& permanent_blacklist();

    /**
     * Set RCPI notification boundaries
     *
     * @since S60 v3.1
     * @param rcp_decline_boundary Threshold for 'signal getting worse'.
     * @param rcp_improve_boundary Threshold for 'signal getting better'.
     */
    void set_rcpi_boundaries(
        u32_t rcp_decline_boundary,
        u32_t rcp_improve_boundary );

    /**
     * Get the RCPI threshold for 'signal getting worse' indication.
     *
     * @since S60 v3.1
     * @return The RCPI threshold for 'signal getting worse' indication.
     */
    u32_t rcp_decline_boundary() const;

    /**
     * Get the RCPI threshold for 'signal getting better' indication.
     *
     * @since S60 v3.1
     * @return The RCPI threshold for 'signal getting better' indication.
     */    
    u32_t rcp_improve_boundary() const;

    /**
     * Get the currently used regional domain.
     *
     * @since S60 v3.1
     * @return The currently used regional domain.
     */
    core_wlan_region_e regional_domain() const;

    /**
     * Set the currently used regional domain.
     *
     * @since S60 v3.1
     * @param region The currently used regional domain.
     */
    void set_regional_domain(
        core_wlan_region_e region );

    /**
     * Get the information whether mobile country code is currently known.
     *
     * @since S60 v5.0
     * @return Whether or not the mobile country code is currently known.
     */
    bool_t mcc_known() const;

    /**
     * Set the information whether or not the mobile country code is currently known.
     *
     * @since S60 v5.0
     * @param mcc_known The information whether or not the mobile country code is currently known.
     */
    void set_mcc_known(
        bool_t mcc_known );

    
    /**
     * Check whether the given channel is valid in the current regional domain.
     *
     * @since S60 v3.1
     * @param band The band the channel is in.
     * @param channel The channel to check.
     * @return Whether the given channel is valid in the current regional domain.
     */
    bool_t is_valid_channel(
        u8_t band,
        u8_t channel ) const;

    /**
     * Return a channel mask with all of the allowed channels in the current regional domain.
     *
     * @since S60 v3.1
     * @return All of the allowed channels in the current regional domain.
     */
    core_scan_channels_s all_valid_scan_channels() const;
    
    /**
     * Return a channel mask with invalid channels filtered out.
     *
     * @since S60 v3.1
     * @param channels Channel mask to be filtered.
     * @return A channel mask with invalid channels filtered out.
     */
    core_scan_channels_s valid_scan_channels(
        const core_scan_channels_s& channels );

    /**
     * Return a channel mask with valid channels filtered out.
     *
     * @since S60 v5.0
     * @param channels Channel mask to be filtered.
     * @return A channel mask with valid channels filtered out.
     */
    core_scan_channels_s invalid_scan_channels(
        const core_scan_channels_s& channels );

    /**
     * Return whether a BT connection has been established.
     *
     * @since S60 v3.1
     * @return Whether a BT connection has been established.
     */
    bool_t is_bt_connection_established() const;

    /**
     * Set whether a BT connection has been established.
     *
     * @since S60 v3.1
     * @param true_t if connection established, false_t otherwise.
     */
    void set_bt_connection_established(
        bool_t established );

    /**
     * Get packet statistics for the connection that is/was active.
     * 
     * @since S60 v3.2
     * @return Packet statistics for the connection that is/was active.
     */
    const core_packet_statistics_by_access_category_s& connection_statistics_by_access_category();

    /**
     * Get packet statistics for the connection that is/was active.
     * 
     * @since S60 v3.2
     * @return Packet statistics for the connection that is/was active.
     */
    core_packet_statistics_s connection_statistics();

    /**
     * Update packet statistics for the currently active connection.
     *
     * @since S60 v3.2
     * @param statistics Packet statistics for the currently active connection.     
     */
    void update_connection_statistics(
        const core_packet_statistics_by_access_category_s& statistics );

    /**
     * Clear the packet statistics.
     *
     * @since S60 v3.2
     */
    void clear_connection_statistics();

    /**
     * Return the roam metrics for the currently active connection.
     *
     * @since S60 v3.2
     * @return The roam metrics for the currently active connection.
     */
    core_roam_metrics_c& roam_metrics();

    /**
     * Get a list of channels that have APs with long beacon intervals.
     *
     * @since S60 v3.2
     * @return List of channels that have APs with long beacon intervals.
     */
    core_long_beacon_interval_channels_s& long_beacon_interval_channels();

    /**
     * Get the U-APSD settings for access classes.
     *
     * @since S60 v3.2
     * @return The U-APSD settings.
     */
    const core_uapsd_settings_s& uapsd_settings();

    /**
     * Check whether U-APSD is enable for the given access class
     *
     * @since S60 v3.2
     * @param access_class Access class to check.
     * @return Whether U-APSD is enable for the given access class.
     */
    bool_t is_uapsd_enable_for_access_class(
        core_access_class_e access_class );

    /**
     * Set the U-APSD settings for access classes.
     *
     * @since S60 v3.2
     * @param settings The U-APSD settings to be stored.
     */
    void set_uapsd_settings(
        const core_uapsd_settings_s& settings );

    /**
     * Get the power save settings for access classes.
     *
     * @since S60 v3.2
     * @return The power save settings.
     */
    const core_power_save_settings_s& power_save_settings();

    /**
     * Set the power save settings for access classes.
     *
     * @since S60 v3.2
     * @param settings The power save settings to be stored.
     */
    void set_power_save_settings(
        const core_power_save_settings_s& settings );

    /**
     * Get the block ACK usage information.
     *
     * @since S60 v5.1
     * @return The block ACK usage information.
     */
    const core_block_ack_usage_s& block_ack_usage();

    /**
     * Set the block ACK usage per traffic stream.
     *
     * @since S60 v5.1
     * @param usage The block ACK usage information to be stored.
     */
    void set_block_ack_usage(
        const core_block_ack_usage_s& usage );    

    /**
     * Set the currently enabled feature set.
     *
     * @since S60 v5.2
     * @param features Bitmask of enabled features.
     * @return core_error_ok if success, an error code otherwise.
     */
    void set_enabled_features(
        u32_t features );

    /**
     * Get the currently enabled feature set.
     *
     * @since S60 v5.2
     * @return Bitmask of enabled features.
     */
    u32_t enabled_features() const;

	/**
     * Check whether the given feature is enabled.
     *
     * @since S60 v5.2
     * @param feature Feature flag to check.
     * @return true_t if the feature is enabled, false_t otherwise. 
     */
    bool_t is_feature_enabled(
        core_feature_e feature ) const;
       
private: // data

    /** Defines whether wlan functionality is enabled or disabled */
    bool_t is_wlan_enabled_m;    

    /** Defines whether the drivers have been loaded. */
    bool_t is_driver_loaded_m;

    /** Specifies the current connection state */
    core_connection_state_e connection_state_m;

    /** Currently used power mode. */
    core_power_mode_s power_mode_m;

    /** Preferred power save mode. */
    core_power_save_mode_s preferred_power_save_m;

    /** Permanent white list of APs. */
    core_type_list_c<core_mac_address_s> perm_whitelist_m;

    /** Permanent black list of APs. */
    core_type_list_c<core_ap_blacklist_entry_s> perm_blacklist_m;

    /** Threshold for RCPI is getting worse notification. */    
    u32_t rcp_decline_boundary_m;

    /** Threshold for RCPI is getting better notification. */    
    u32_t rcp_improve_boundary_m;

    /** Defines the currently used regional domain. */
    core_wlan_region_e region_m;

    /** Defines whether the mobile country code is known or not. */
    bool_t mcc_known_m;

    /** Defines whether a BT connection has been established. */
    bool_t is_bt_connection_established_m;

    /** Packet statistics for the connection that is/was active. */
    core_packet_statistics_by_access_category_s current_statistics_m;

    /** Roam metrics for the connection that is/was active. */
    core_roam_metrics_c roam_metrics_m;

    /** List of channels that have APs with long beacon intervals. */
    core_long_beacon_interval_channels_s long_beacon_interval_channels_m;

    /** U-APSD settings for access classes. */
    core_uapsd_settings_s uapsd_settings_m;

    /** Power save settings for access classes. */
    core_power_save_settings_s power_save_settings_m;

    /** Block ACK usage per traffic stream. */
    core_block_ack_usage_s block_ack_usage_m;

    /** Bitmask of enabled features. */
    u32_t features_m;

    };

#endif // CORE_SETTINGS_H
