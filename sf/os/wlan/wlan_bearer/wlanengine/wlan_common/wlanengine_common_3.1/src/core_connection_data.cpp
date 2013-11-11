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
* %version: 38 %
*/

#include "core_connection_data.h"
#include "core_tools.h"
#include "am_debug.h"

const u8_t CORE_FAILURE_NONE = 0;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//   
core_connection_data_c::core_connection_data_c(
    const core_iap_data_s& iap_data,
    const core_device_settings_s& device_settings ) :
    iap_data_m( iap_data ),
    device_settings_m( device_settings ),
    connect_status_m( core_connect_undefined ),
    previous_ap_m( NULL ),
    current_ap_m( NULL ),
    ssid_m( BROADCAST_SSID ),
    temp_blacklist_m( ),
    last_rcp_class_m( core_rcp_normal ),
    last_tx_level_m( MAX_TX_POWER_LEVEL_NOT_DEFINED ),
    last_bssid_m( ZERO_MAC_ADDR ),
    last_connection_state_m( core_connection_state_notconnected ),
    association_failure_count_m( ),
    deauthentication_count_m( ),
    last_eap_type_m( EAP_TYPE_NONE ),
    last_eap_error_code_m( EAP_ERROR_NONE ),
    adjacent_ap_channels_m( ),
    is_eapol_authentication_started_m( false_t ),
    is_eapol_authenticating_m( false_t ),
    eapol_auth_bssid_m( ZERO_MAC_ADDR ),
    eapol_auth_failure_m( core_error_ok ),
    traffic_stream_list_m( ),
    virtual_traffic_stream_list_m( ),
    is_eapol_connecting_m( false_t ),
    is_eapol_disconnecting_m( false_t ),
    is_disconnecting_m( false_t ),
    last_roam_reason_m( core_roam_reason_none ),
    last_roam_failed_reason_m( core_roam_failed_reason_none ),
    is_eapol_require_immediate_reconnect_m( false_t ),
    is_voice_call_on_m( false_t ),
    previous_rrm_measurement_request_time_m( 0 )
    {
    DEBUG( "core_connection_data_c::core_connection_data_c()" );

    for( u8_t idx( 0 ); idx < MAX_QOS_ACCESS_CLASS; ++idx )
        {
        ac_traffic_status_m[idx] = core_access_class_traffic_status_admitted;
        ac_traffic_mode_m[idx] = core_access_class_traffic_mode_automatic;
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_connection_data_c::~core_connection_data_c()
    {
    DEBUG( "core_connection_data_c::~core_connection_data_c()" );
    delete current_ap_m;
    delete previous_ap_m;
    temp_blacklist_m.clear();
    association_failure_count_m.clear();
    deauthentication_count_m.clear();
    power_save_test_verdict_m.clear();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//   
core_iap_data_c& core_connection_data_c::iap_data()
    {
    return iap_data_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//   
core_connect_status_e core_connection_data_c::connect_status() const
    {
    return connect_status_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//   
void core_connection_data_c::set_connect_status(
    core_connect_status_e connect_status )
    {
    connect_status_m = connect_status;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//   
void core_connection_data_c::set_current_ap_data(
    const core_ap_data_c& ap_data )
    {
    DEBUG( "core_connection_data_c::set_current_ap_data()" );
    
    delete current_ap_m;
    current_ap_m = NULL;
    
    current_ap_m = core_ap_data_c::instance( ap_data );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_ap_data_c* core_connection_data_c::current_ap_data()
    {
    return current_ap_m;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_connection_data_c::set_previous_ap_data(
    const core_ap_data_c& ap_data )
    {
    delete previous_ap_m;
    previous_ap_m = NULL;
    
    previous_ap_m = core_ap_data_c::instance( ap_data );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_ap_data_c* core_connection_data_c::previous_ap_data()
    {
    return previous_ap_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
const core_ssid_s& core_connection_data_c::ssid() const
    {
    return ssid_m;
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_connection_data_c::set_ssid(
    const core_ssid_s& ssid )
    {
    ssid_m = ssid;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_connection_data_c::is_mac_in_temporary_blacklist(
    const core_mac_address_s& mac )
    {
    core_ap_blacklist_entry_s* addr = temp_blacklist_m.first();    
    while ( addr )
        {
        if ( addr->bssid == mac )
            {
            return true_t;
            }
        
        addr = temp_blacklist_m.next();
        }        

    return false_t;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_connection_data_c::add_mac_to_temporary_blacklist(
    const core_mac_address_s& mac,
    core_ap_blacklist_reason_e reason )
    {
    if ( !is_mac_in_temporary_blacklist( mac ) )
        {
        core_ap_blacklist_entry_s* entry = new core_ap_blacklist_entry_s;
        if ( entry )
            {
            entry->bssid = mac;
            entry->reason = reason;
            temp_blacklist_m.append( entry );
            }
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_connection_data_c::remove_mac_from_temporary_blacklist(
    const core_mac_address_s& mac )
    {
    core_ap_blacklist_entry_s* addr = temp_blacklist_m.first();    
    while ( addr )
        {
        if ( addr->bssid == mac )
            {
            temp_blacklist_m.remove( addr );
            delete addr;
            addr = NULL;

            return;
            }
        
        addr = temp_blacklist_m.next();
        }       
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_type_list_c<core_ap_blacklist_entry_s>& core_connection_data_c::temporary_blacklist()
    {
    return temp_blacklist_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_connection_data_c::remove_temporary_blacklist_entries(
    u32_t reasons )
    {
    core_ap_blacklist_entry_s* addr = temp_blacklist_m.first();    
    while( addr )
        {
        if( reasons & addr->reason )
            {
            DEBUG6( "core_connection_data_c::remove_temporary_blacklist_entries() - removing entry with BSSID %02X:%02X:%02X:%02X:%02X:%02X",
                addr->bssid.addr[0], addr->bssid.addr[1], addr->bssid.addr[2], 
                addr->bssid.addr[3], addr->bssid.addr[4], addr->bssid.addr[5] );

            temp_blacklist_m.remove( addr );
            delete addr;
            addr = NULL;

            return;
            }

        addr = temp_blacklist_m.next();
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_rcp_class_e core_connection_data_c::last_rcp_class() const
    {
    return last_rcp_class_m;  
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//   
void core_connection_data_c::set_last_rcp_class(
    core_rcp_class_e type )
    {
    last_rcp_class_m = type;
    };

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//   
u32_t core_connection_data_c::last_tx_level() const
    {
    return last_tx_level_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//      
void core_connection_data_c::set_last_tx_level(
    u32_t tx_level )
    {
    last_tx_level_m = tx_level;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
const core_mac_address_s& core_connection_data_c::last_bssid() const
    {
    return last_bssid_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_connection_data_c::set_last_bssid(
    const core_mac_address_s& bssid )
    {
    last_bssid_m = bssid;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_connection_state_e core_connection_data_c::connection_state() const
    {
    core_operating_mode_e mode =
        iap_data_m.operating_mode();
        
    if( mode == core_operating_mode_ibss )
        {
        if( iap_data_m.security_mode() == core_security_mode_allow_unsecure )
            {
            return core_connection_state_ibss;
            }
        else
            {
            return core_connection_state_secureibss;
            }
        }
    else
        {
        if( iap_data_m.security_mode() == core_security_mode_allow_unsecure )
            {
            return core_connection_state_infrastructure;
            }
        else
            {
            return core_connection_state_secureinfra;
            }
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_connection_state_e core_connection_data_c::last_connection_state() const
    {
    return last_connection_state_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_connection_data_c::set_last_connection_state(
    core_connection_state_e state )
    {
    last_connection_state_m = state;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u8_t core_connection_data_c::ap_association_failure_count(
    const core_mac_address_s& bssid )
    {    
    core_ap_failure_count_s* iter = association_failure_count_m.first();    
    while ( iter )
        {
        if ( iter->bssid == bssid )
            {
            return iter->count;
            }

        iter = association_failure_count_m.next();
        }

    return CORE_FAILURE_NONE;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_connection_data_c::increase_ap_association_failure_count(
    const core_mac_address_s& bssid )
    {
    core_ap_failure_count_s* iter = association_failure_count_m.first();    
    while ( iter )
        {
        if ( iter->bssid == bssid )
            {
            iter->count++;
            return;
            }

        iter = association_failure_count_m.next();
        }

    iter = new core_ap_failure_count_s;
    if ( iter )
        {
        iter->bssid = bssid;
        iter->count = CORE_FAILURE_NONE + 1;
        association_failure_count_m.append( iter );
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_connection_data_c::clear_ap_association_failure_count(
    const core_mac_address_s& bssid )
    {
    core_ap_failure_count_s* iter = association_failure_count_m.first();    
    while ( iter )
        {
        if ( iter->bssid == bssid )
            {
            DEBUG1( "core_connection_data_c::clear_ap_failure_count() - this AP had %u previous failures",
                iter->count );

            association_failure_count_m.remove( iter );

            delete iter;
            iter = NULL;

            return;
            }

        iter = association_failure_count_m.next();
        }

    DEBUG( "core_connection_data_c::clear_ap_failure_count() - this AP had no previous failures" );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u8_t core_connection_data_c::ap_authentication_failure_count(
    const core_mac_address_s& bssid )
    {    
    core_ap_auth_failure_count_s* iter = authentication_failure_count_m.first();    
    while ( iter )
        {
        if ( iter->bssid == bssid )
            {
            return iter->count;
            }

        iter = authentication_failure_count_m.next();
        }

    return CORE_FAILURE_NONE;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_connection_data_c::increase_ap_authentication_failure_count(
    const core_mac_address_s& bssid )
    {
    core_ap_auth_failure_count_s* iter = authentication_failure_count_m.first();    
    while ( iter )
        {
        if ( iter->bssid == bssid )
            {
            iter->count++;
            return;
            }

        iter = authentication_failure_count_m.next();
        }

    iter = new core_ap_auth_failure_count_s;
    if ( iter )
        {
        iter->bssid = bssid;
        iter->count = CORE_FAILURE_NONE + 1;
        authentication_failure_count_m.append( iter );
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_connection_data_c::clear_all_authentication_failure_counts()
    {
    DEBUG( "core_connection_data_c::clear_all_authentication_failure_counts()" );
    
    core_ap_auth_failure_count_s* iter = authentication_failure_count_m.first();    
    while ( iter )
        {
        DEBUG1( "core_connection_data_c::clear_all_authentication_failure_counts() - this AP had %u previous failures",
            iter->count );

        authentication_failure_count_m.remove( iter );

        delete iter;
        iter = NULL;
            
        iter = authentication_failure_count_m.next();
        }

    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u8_t core_connection_data_c::ap_deauthentication_count(
    const core_mac_address_s& bssid )
    {
    core_ap_failure_count_s* iter = deauthentication_count_m.first();    
    while ( iter )
        {
        if ( iter->bssid == bssid )
            {
            return iter->count;
            }

        iter = deauthentication_count_m.next();
        }

    return CORE_FAILURE_NONE;    
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_connection_data_c::increase_ap_deauthentication_count(
    const core_mac_address_s& bssid )
    {
    core_ap_failure_count_s* iter = deauthentication_count_m.first();    
    while ( iter )
        {
        if ( iter->bssid == bssid )
            {
            iter->count++;
            return;
            }

        iter = deauthentication_count_m.next();
        }

    iter = new core_ap_failure_count_s;
    if ( iter )
        {
        iter->bssid = bssid;
        iter->count = CORE_FAILURE_NONE + 1;
        deauthentication_count_m.append( iter );
        }    
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u32_t core_connection_data_c::last_failed_eap_type() const
    {
    return last_eap_type_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_connection_data_c::set_last_failed_eap_type(
    u32_t type )
    {
    last_eap_type_m = type;   
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//  
u32_t core_connection_data_c::last_eap_error() const
    {
    return last_eap_error_code_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_connection_data_c::set_last_eap_error(
    u32_t error )
    {
    last_eap_error_code_m = error;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
const core_scan_channels_c& core_connection_data_c::adjacent_ap_channels() const
    {
    return adjacent_ap_channels_m; 
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_connection_data_c::set_adjacent_ap_channels(
    const core_scan_channels_c& channels )
    {
    adjacent_ap_channels_m.set( channels );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_connection_data_c::merge_adjacent_ap_channels(
    const core_scan_channels_c& channels )
    {
    adjacent_ap_channels_m.merge( channels );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_connection_data_c::is_eapol_authentication_started() const
    {
    return is_eapol_authentication_started_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_connection_data_c::set_eapol_authentication_started(
    bool_t is_authentication_started )
    {
    is_eapol_authentication_started_m = is_authentication_started;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_connection_data_c::is_eapol_authenticating() const
    {
    return is_eapol_authenticating_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_connection_data_c::set_eapol_authenticating(
    bool_t is_authenticating )
    {
    is_eapol_authenticating_m = is_authenticating;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
const core_mac_address_s& core_connection_data_c::eapol_auth_bssid() const
    {
    return eapol_auth_bssid_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_connection_data_c::set_eapol_auth_bssid(
    const core_mac_address_s& bssid )
    {
    eapol_auth_bssid_m = bssid;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_connection_data_c::eapol_auth_failure() const
    {
    return eapol_auth_failure_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_connection_data_c::set_eapol_auth_failure(
    core_error_e error )
    {
    eapol_auth_failure_m = error;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_traffic_stream_list_c& core_connection_data_c::traffic_stream_list()
    {
    return traffic_stream_list_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_virtual_traffic_stream_list_c& core_connection_data_c::virtual_traffic_stream_list()
    {
    return virtual_traffic_stream_list_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_connection_data_c::is_eapol_connecting() const
    {
    return is_eapol_connecting_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_connection_data_c::set_eapol_connecting(
    bool_t is_eapol_connecting )
    {
    is_eapol_connecting_m = is_eapol_connecting;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_connection_data_c::is_eapol_disconnecting() const
    {
    return is_eapol_disconnecting_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_connection_data_c::set_eapol_disconnecting(
    bool_t is_eapol_disconnecting )
    {
    is_eapol_disconnecting_m = is_eapol_disconnecting;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_connection_data_c::is_disconnecting() const
    {
    return is_disconnecting_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_connection_data_c::set_disconnecting(
    bool_t is_disconnecting )
    {
    is_disconnecting_m = is_disconnecting;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
const core_rcpi_roam_interval_s& core_connection_data_c::rcpi_roam_interval() const
    {
    return rcpi_roam_interval_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_connection_data_c::reset_rcpi_roam_interval()
    {
    rcpi_roam_interval_m.count = 0;
    rcpi_roam_interval_m.interval = device_settings_m.rcpi_roam_min_interval;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_connection_data_c::set_core_rcpi_roam_interval(
    const core_rcpi_roam_interval_s& interval )
    {
    rcpi_roam_interval_m = interval;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//    
core_roam_reason_e core_connection_data_c::last_roam_reason() const
    {
    return last_roam_reason_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//    
void core_connection_data_c::set_last_roam_reason(
    core_roam_reason_e reason )
    {
    last_roam_reason_m = reason;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//    
core_roam_failed_reason_e core_connection_data_c::last_roam_failed_reason() const
    {
    return last_roam_failed_reason_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//    
void core_connection_data_c::set_last_roam_failed_reason(
    core_roam_failed_reason_e reason )
    {
    last_roam_failed_reason_m = reason;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_connection_data_c::is_ap_power_save_test_run(
    const core_mac_address_s& bssid,
    bool_t& is_success )
    {
    core_ap_power_save_test_verdict_s* iter = power_save_test_verdict_m.first();
    while ( iter )
        {
        if ( iter->bssid == bssid )
            {
            is_success = iter->is_success;

            return true_t;
            }

        iter = power_save_test_verdict_m.next();
        }

    return false_t;    
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_connection_data_c::add_ap_power_save_test_verdict(
    const core_mac_address_s& bssid,
    bool_t is_success )
    {
    core_ap_power_save_test_verdict_s* iter = new core_ap_power_save_test_verdict_s;
    if ( iter )
        {
        iter->bssid = bssid;
        iter->is_success = is_success;

        power_save_test_verdict_m.append( iter );
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_connection_data_c::is_eapol_require_immediate_reconnect() const
    {
    return is_eapol_require_immediate_reconnect_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_connection_data_c::set_eapol_require_immediate_reconnect(
    bool_t is_immediate_reconnect_required )
    {
    is_eapol_require_immediate_reconnect_m = is_immediate_reconnect_required;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_connection_data_c::voice_call_state() const
    {
    DEBUG1( "core_connection_data_c::voice_call_state() - returning %u", is_voice_call_on_m );
    return is_voice_call_on_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_connection_data_c::set_voice_call_state( const bool_t& is_voice_call_on ) 
    {
    DEBUG1( "core_connection_data_c::set_voice_call_state( %u )", is_voice_call_on );
    is_voice_call_on_m = is_voice_call_on;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
const u64_t& core_connection_data_c::get_previous_rrm_measurement_request_time() const
    {
    return previous_rrm_measurement_request_time_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_connection_data_c::set_previous_rrm_measurement_request_time( const u64_t& timestamp)
    {
    previous_rrm_measurement_request_time_m = timestamp;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_access_class_traffic_status_e core_connection_data_c::ac_traffic_status(
    core_access_class_e access_class ) const
    {
    return ac_traffic_status_m[access_class];
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_connection_data_c::set_ac_traffic_status(
    core_access_class_e access_class,
    core_access_class_traffic_status_e status )
    {
    ac_traffic_status_m[access_class] = status;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_access_class_traffic_mode_e core_connection_data_c::ac_traffic_mode(
    core_access_class_e access_class ) const
    {
    return ac_traffic_mode_m[access_class];
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//   
void core_connection_data_c::set_ac_traffic_mode(
    core_access_class_e access_class,
    core_access_class_traffic_mode_e mode )
    {
    ac_traffic_mode_m[access_class] = mode;
    }
