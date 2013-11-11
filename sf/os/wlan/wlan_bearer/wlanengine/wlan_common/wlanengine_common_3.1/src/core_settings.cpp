/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* %version: 26 %
*/

#include "core_settings.h"
#include "core_settings_default.h"
#include "core_scan_channels.h"
#include "core_tools.h"
#include "am_debug.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_settings_c::core_settings_c(
    u32_t features ) :
    is_wlan_enabled_m( true_t ),    
    is_driver_loaded_m( false_t ),
    connection_state_m( core_connection_state_notconnected ),
    power_mode_m( CORE_POWER_MODE_CAM ),
    preferred_power_save_m( CORE_POWER_SAVE_MODE_AUTOMATIC ),
    perm_whitelist_m( ),
    perm_blacklist_m( ),
    rcp_decline_boundary_m( 0 ),
    rcp_improve_boundary_m( 0 ),
    region_m( core_wlan_region_fcc ),
    mcc_known_m( false_t ),
    is_bt_connection_established_m( false_t ),
    roam_metrics_m( ),
    uapsd_settings_m( DEFAULT_UAPSD_SETTINGS ),
    power_save_settings_m( DEFAULT_POWER_SAVE_SETTINGS ),
    block_ack_usage_m( DEFAULT_BLOCK_ACK_USAGE ),
	features_m( features )
    {
    DEBUG1( "core_settings_c::core_settings_c() - features: 0x%08X",
        features ); 

    clear_connection_statistics();

    core_tools_c::fillz(    
        &long_beacon_interval_channels_m.channel_scan_count[0],
        sizeof ( long_beacon_interval_channels_m ) );            
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_settings_c::~core_settings_c()
    {
    DEBUG( "core_settings_c::~core_settings_c()" ); 
    
    perm_whitelist_m.clear();
    perm_blacklist_m.clear();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//   
bool_t core_settings_c::is_driver_loaded() const
    {
    return is_driver_loaded_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_settings_c::set_driver_state(
    bool_t is_driver_loaded )
    {
    is_driver_loaded_m = is_driver_loaded;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_settings_c::is_wlan_enabled() const
    {
    return is_wlan_enabled_m;
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_settings_c::set_wlan_enabled(
    bool_t is_wlan_enabled )
    {
    is_wlan_enabled_m = is_wlan_enabled;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
const core_power_mode_s& core_settings_c::power_mode() const
    {
    return power_mode_m;    
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_settings_c::set_power_mode(
    const core_power_mode_s& mode )
    {
    power_mode_m = mode;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
const core_power_save_mode_s& core_settings_c::preferred_power_save_mode() const
    {
    return preferred_power_save_m;
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_settings_c::set_preferred_power_save_mode(
    const core_power_save_mode_s& mode )
    {
    preferred_power_save_m = mode;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_connection_state_e core_settings_c::connection_state() const
    {
    return connection_state_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_settings_c::set_connection_state(
    core_connection_state_e state )
    {
    connection_state_m = state;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_settings_c::is_connected() const
    {
    if( connection_state_m == core_connection_state_notconnected )
        {
        return false_t;
        }

    return true_t;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_settings_c::is_permanent_whitelist_empty() const
    {
    return !perm_whitelist_m.count();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_settings_c::is_mac_in_permanent_whitelist(
    const core_mac_address_s& mac )
    {
    core_mac_address_s* addr = perm_whitelist_m.first();    
    while ( addr )
        {
        if ( *addr == mac )
            {
            return true_t;
            }
        
        addr = perm_whitelist_m.next();
        }        
    
    return false_t;    
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_settings_c::is_mac_in_permanent_blacklist(
    const core_mac_address_s& mac )
    {
    core_ap_blacklist_entry_s* addr = perm_blacklist_m.first();    
    while ( addr )
        {
        if ( addr->bssid == mac )
            {
            return true_t;
            }
        
        addr = perm_blacklist_m.next();
        }        

    return false_t;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_settings_c::add_mac_to_permanent_whitelist(
    const core_mac_address_s& mac )
    {
    if ( !is_mac_in_permanent_whitelist( mac ) )
        {
        core_mac_address_s* entry = new core_mac_address_s;
        if ( entry )
            {
            *entry = mac;
            perm_whitelist_m.append( entry );
            }                        
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_settings_c::remove_mac_from_permanent_whitelist(
    const core_mac_address_s& mac )
    {
    core_mac_address_s* addr = perm_whitelist_m.first();    
    while ( addr )
        {
        if ( *addr == mac )
            {
            perm_whitelist_m.remove( addr );
            delete addr;

            return;
            }
        
        addr = perm_whitelist_m.next();
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_settings_c::add_mac_to_permanent_blacklist(
    const core_mac_address_s& mac,
    core_ap_blacklist_reason_e reason )
    {
    if ( !is_mac_in_permanent_blacklist( mac ) )
        {
        core_ap_blacklist_entry_s* entry = new core_ap_blacklist_entry_s;
        if ( entry )
            {
            entry->bssid = mac;
            entry->reason = reason;
            perm_blacklist_m.append( entry );
            }                        
        }       
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_settings_c::remove_mac_from_permanent_blacklist(
    const core_mac_address_s& mac )
    {
    core_ap_blacklist_entry_s* addr = perm_blacklist_m.first();    
    while ( addr )
        {
        if ( addr->bssid == mac )
            {
            perm_blacklist_m.remove( addr );
            delete addr;

            return;
            }
        
        addr = perm_blacklist_m.next();
        }       
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_type_list_c<core_ap_blacklist_entry_s>& core_settings_c::permanent_blacklist()
    {
    return perm_blacklist_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_settings_c::set_rcpi_boundaries(
    u32_t rcp_decline_boundary,
    u32_t rcp_improve_boundary )
    {
    rcp_decline_boundary_m = rcp_decline_boundary;
    rcp_improve_boundary_m = rcp_improve_boundary;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u32_t core_settings_c::rcp_decline_boundary() const
    {
    return rcp_decline_boundary_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u32_t core_settings_c::rcp_improve_boundary() const
    {
    return rcp_improve_boundary_m;    
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_wlan_region_e core_settings_c::regional_domain() const
    {
    return region_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_settings_c::set_regional_domain(
    core_wlan_region_e region )
    {
    region_m = region;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_settings_c::mcc_known() const
    {
    return mcc_known_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_settings_c::set_mcc_known(
    bool_t mcc_known )
    {
    mcc_known_m = mcc_known;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_settings_c::is_valid_channel(
    u8_t band,
    u8_t channel ) const
    {
    if ( band & SCAN_BAND_2DOT4GHZ )
        {
        if( !channel )
            {
            return false_t;
            }
        
        if( region_m == core_wlan_region_fcc &&
            channel > SCAN_BAND_2DOT4GHZ_MAX_CHANNEL_FCC )
            {
            return false_t;
            }
        else if ( channel > SCAN_BAND_2DOT4GHZ_MAX_CHANNEL_EURO )
            {
            return false_t;
            }
        
        return true_t;        
        }

    return false_t;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_scan_channels_s core_settings_c::all_valid_scan_channels() const
    {
    if( region_m == core_wlan_region_fcc )
        {
        return SCAN_CHANNELS_2DOT4GHZ_US;
        }

    return SCAN_CHANNELS_2DOT4GHZ_ETSI;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_scan_channels_s core_settings_c::valid_scan_channels(
    const core_scan_channels_s& channels )
    {    
    core_scan_channels_c filtered_channels;
    
    if ( channels.band & SCAN_BAND_2DOT4GHZ )
        {
        u16_t mask( 0 );
        core_tools_c::copy(
            reinterpret_cast<u8_t*>( &mask ),
            &channels.channels2dot4ghz[0],            
            sizeof( mask ) );       
        
        for ( u8_t channel( 0 ); channel < SCAN_BAND_2DOT4GHZ_MAX_CHANNEL_EURO; ++channel )
            {
            if ( mask & ( 1 << channel ) &&
                 is_valid_channel(
                    SCAN_BAND_2DOT4GHZ,
                    channel + 1 ) )                    
                {
                filtered_channels.add(
                    SCAN_BAND_2DOT4GHZ,
                    channel + 1 );
                }
            }
        }

    return filtered_channels.channels();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_scan_channels_s core_settings_c::invalid_scan_channels(
    const core_scan_channels_s& channels )
    {    
    core_scan_channels_c filtered_channels;
    
    if ( channels.band & SCAN_BAND_2DOT4GHZ )
        {
        u16_t mask( 0 );
        core_tools_c::copy(
            reinterpret_cast<u8_t*>( &mask ),
            &channels.channels2dot4ghz[0],            
            sizeof( mask ) );       
        
        for ( u8_t channel( 0 ); channel < SCAN_BAND_2DOT4GHZ_MAX_CHANNEL_EURO; ++channel )
            {
            if ( !( mask & ( 1 << channel ) &&
                 is_valid_channel(
                    SCAN_BAND_2DOT4GHZ,
                    channel + 1 ) ) )                    
                {
                filtered_channels.add(
                    SCAN_BAND_2DOT4GHZ,
                    channel + 1 );
                }
            }
        }

    return filtered_channels.channels();
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_settings_c::is_bt_connection_established() const
    {
    return is_bt_connection_established_m;   
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_settings_c::set_bt_connection_established(
    bool_t established )
    {
    is_bt_connection_established_m = established;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
const core_packet_statistics_by_access_category_s& core_settings_c::connection_statistics_by_access_category()
    {
    return current_statistics_m;   
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_packet_statistics_s core_settings_c::connection_statistics()
    {
    core_packet_statistics_s statistics;
    core_tools_c::fillz(
        &statistics,
        sizeof( statistics ) );

    for ( u8_t idx( 0 ); idx < MAX_QOS_ACCESS_CLASS; ++idx )
        {
        statistics.rx_frames += current_statistics_m.ac[idx].rx_frames;
        statistics.tx_frames += current_statistics_m.ac[idx].tx_frames;
        statistics.rx_multicast_frames += current_statistics_m.ac[idx].rx_multicast_frames;
        statistics.tx_multicast_frames += current_statistics_m.ac[idx].tx_multicast_frames;
        statistics.tx_retries += current_statistics_m.ac[idx].tx_retries;
        statistics.tx_errors += current_statistics_m.ac[idx].tx_errors;
        }
    statistics.fcs_errors = current_statistics_m.fcs_errors;

    return statistics;   
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_settings_c::update_connection_statistics(
    const core_packet_statistics_by_access_category_s& statistics )
    {
    DEBUG( "core_settings_c::update_connection_statistics()" );

    for ( u8_t idx( 0 ); idx < MAX_QOS_ACCESS_CLASS; ++idx )
        {
        DEBUG1( "core_settings_c::update_connection_statistics() - delta statistics for AC %u:",
            idx );
        DEBUG1( "core_settings_c::update_connection_statistics() - rx_frames:           %u",
            statistics.ac[idx].rx_frames );
        DEBUG1( "core_settings_c::update_connection_statistics() - tx_frames:           %u",
            statistics.ac[idx].tx_frames );
        DEBUG1( "core_settings_c::update_connection_statistics() - rx_multicast_frames: %u",
            statistics.ac[idx].rx_multicast_frames );
        DEBUG1( "core_settings_c::update_connection_statistics() - tx_multicast_frames: %u",
            statistics.ac[idx].tx_multicast_frames );
        DEBUG1( "core_settings_c::update_connection_statistics() - tx_retries:          %u",
            statistics.ac[idx].tx_retries );
        DEBUG1( "core_settings_c::update_connection_statistics() - tx_errors:           %u",
            statistics.ac[idx].tx_errors );
        DEBUG1( "core_settings_c::update_connection_statistics() - tx_media_delay:      %u",
            statistics.ac[idx].tx_media_delay );
        DEBUG1( "core_settings_c::update_connection_statistics() - tx_total_delay:      %u",
            statistics.ac[idx].tx_total_delay );
        DEBUG1( "core_settings_c::update_connection_statistics() - tx_total_delay_bin0: %u",
            statistics.ac[idx].tx_total_delay_bin0 );
        DEBUG1( "core_settings_c::update_connection_statistics() - tx_total_delay_bin1: %u",
            statistics.ac[idx].tx_total_delay_bin1 );
        DEBUG1( "core_settings_c::update_connection_statistics() - tx_total_delay_bin2: %u",
            statistics.ac[idx].tx_total_delay_bin2 );
        DEBUG1( "core_settings_c::update_connection_statistics() - tx_total_delay_bin3: %u",
            statistics.ac[idx].tx_total_delay_bin3 );

        current_statistics_m.ac[idx].rx_frames += statistics.ac[idx].rx_frames;
        current_statistics_m.ac[idx].tx_frames += statistics.ac[idx].tx_frames;
        current_statistics_m.ac[idx].rx_multicast_frames += statistics.ac[idx].rx_multicast_frames;
        current_statistics_m.ac[idx].tx_multicast_frames += statistics.ac[idx].tx_multicast_frames;
        current_statistics_m.ac[idx].tx_retries += statistics.ac[idx].tx_retries;
        current_statistics_m.ac[idx].tx_errors += statistics.ac[idx].tx_errors;

        current_statistics_m.ac[idx].tx_media_delay = statistics.ac[idx].tx_media_delay;
        current_statistics_m.ac[idx].tx_total_delay = statistics.ac[idx].tx_total_delay;

        current_statistics_m.ac[idx].tx_total_delay_bin0 += statistics.ac[idx].tx_total_delay_bin0;
        current_statistics_m.ac[idx].tx_total_delay_bin1 += statistics.ac[idx].tx_total_delay_bin1;
        current_statistics_m.ac[idx].tx_total_delay_bin2 += statistics.ac[idx].tx_total_delay_bin2;
        current_statistics_m.ac[idx].tx_total_delay_bin3 += statistics.ac[idx].tx_total_delay_bin3;
        }

    DEBUG1( "core_settings_c::update_connection_statistics() - delta fcs_errors:    %u",
        statistics.fcs_errors );
    current_statistics_m.fcs_errors += statistics.fcs_errors;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_settings_c::clear_connection_statistics()
    {
    core_tools_c::fillz(
        &current_statistics_m,
        sizeof( current_statistics_m ) );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_roam_metrics_c& core_settings_c::roam_metrics()
    {
    return roam_metrics_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_long_beacon_interval_channels_s& core_settings_c::long_beacon_interval_channels()
    {
    return long_beacon_interval_channels_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
const core_uapsd_settings_s& core_settings_c::uapsd_settings()
    {
    return uapsd_settings_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_settings_c::is_uapsd_enable_for_access_class(
    core_access_class_e access_class )
    {
    if ( access_class == core_access_class_voice &&
         uapsd_settings_m.uapsd_enabled_for_voice )
        {
        return true_t;
        }
    else if ( access_class == core_access_class_video &&
              uapsd_settings_m.uapsd_enabled_for_video )
        {
        return true_t;
        }
    else if ( access_class == core_access_class_best_effort &&
              uapsd_settings_m.uapsd_enabled_for_best_effort )
        {
        return true_t;
        }
    else if ( access_class == core_access_class_background &&
              uapsd_settings_m.uapsd_enabled_for_background )
        {
        return true_t;
        }

    return false_t;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_settings_c::set_uapsd_settings(
    const core_uapsd_settings_s& settings )
    {
    uapsd_settings_m = settings;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
const core_power_save_settings_s& core_settings_c::power_save_settings()
    {
    return power_save_settings_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_settings_c::set_power_save_settings(
    const core_power_save_settings_s& settings )
    {
    power_save_settings_m = settings;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
const core_block_ack_usage_s& core_settings_c::block_ack_usage()
    {
    return block_ack_usage_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_settings_c::set_block_ack_usage(
    const core_block_ack_usage_s& usage )
    {
    block_ack_usage_m = usage;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_settings_c::set_enabled_features(
    u32_t features )
    {
    DEBUG1( "core_settings_c::set_enabled_features() - features: 0x%08X",
        features );
    if( features & core_feature_power_save_test )
        {
        DEBUG( "core_settings_c::set_enabled_features() - core_feature_power_save_test" );
        }
    if( features & core_feature_802dot11k )
        {
        DEBUG( "core_settings_c::set_enabled_features() - core_feature_802dot11k" );
        }
    if( features & core_feature_802dot11n )
        {
        DEBUG( "core_settings_c::set_enabled_features() - core_feature_802dot11n" );
        }

    features_m = features;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u32_t core_settings_c::enabled_features() const
    {
    return features_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// 
bool_t core_settings_c::is_feature_enabled(
    core_feature_e feature ) const
    {
    if( features_m & feature )
        {
        return true_t;
        }

    return false_t;
    }
