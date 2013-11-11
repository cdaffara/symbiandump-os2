/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Stub version of WPX adaptation.
*
*/

/*
* %version: 15 %
*/

#include "core_wpx_adaptation_stub.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_wpx_adaptation_stub_c::core_wpx_adaptation_stub_c()
    {
    DEBUG( "core_wpx_adaptation_stub_c::core_wpx_adaptation_stub_c()" );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_wpx_adaptation_stub_c::~core_wpx_adaptation_stub_c()
    {
    DEBUG( "core_wpx_adaptation_stub_c::~core_wpx_adaptation_stub_c()" );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_key_management_e core_wpx_adaptation_stub_c::wpx_key_management_suite(
    const u8_t* /* oui */ )
    {
    return core_key_management_none;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_wpx_adaptation_stub_c::get_wpx_key_management_oui(
    core_key_management_e /* key_management */,
    u8_t& /* data_length */,
    u8_t* /* data */ )
    {
    return core_error_not_supported;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_wpx_adaptation_stub_c::is_ap_wpx_compatible_with_iap(
    core_connect_status_e& /* reason */,
    const core_ap_data_c& /* ap_data */,
    const core_iap_data_c& /* iap_data */ )
    {
    return true_t;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_wpx_adaptation_stub_c::handle_wpx_frame(
    core_frame_type_e /* frame_type */,
    u16_t /* frame_length */,
    const u8_t* /* frame_data */ )
    {
    return false_t;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_wpx_adaptation_stub_c::handle_wpx_roam_success(
    const core_ap_data_c& /* ap_data */ )
    {
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_wpx_adaptation_stub_c::handle_wpx_connection_stop()
    {
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_wpx_adaptation_stub_c::handle_fast_roam_start_reassociation(
    core_ap_data_c& /* ap_data */,
    core_type_list_c<core_frame_dot11_ie_c>& /* assoc_ie_list */ )
    {
    return core_error_not_supported;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_wpx_adaptation_stub_c::handle_fast_roam_reassoc_resp(
    core_frame_assoc_resp_c* /* frame */ )
    {
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_wpx_adaptation_stub_c::get_association_request_wpx_ie(
    core_ap_data_c& /* ap_data */,
    core_type_list_c<core_frame_dot11_ie_c>& /* assoc_ie_list */ )
    {
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u8_t core_wpx_adaptation_stub_c::wpx_version(
    core_frame_beacon_c* /* frame */ )
    {
    return 0;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u8_t core_wpx_adaptation_stub_c::max_tx_power_level(
    core_frame_beacon_c* /* frame */ )
    {
    return MAX_TX_POWER_LEVEL_NOT_DEFINED;    
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_wpx_adaptation_stub_c::handle_ts_create_request(
    const core_ap_data_c& /* ap_data */,
    core_frame_dot11_c* /* frame */,
    u8_t /* tid */,
    const core_traffic_stream_params_s& /* tspec */ )
    {
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_wpx_adaptation_stub_c::get_wpx_traffic_stream_params(
    core_frame_dot11_c* /* frame */,
    u8_t /* tid */,
    core_traffic_stream_params_s& /* tspec */ )
    {
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_traffic_stream_status_e core_wpx_adaptation_stub_c::get_wpx_traffic_stream_status(
    u8_t /* status */ )
    {
    return core_traffic_stream_status_inactive_other;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_wpx_adaptation_stub_c::handle_association_response(
    core_ap_data_c& /* ap_data */,
    core_frame_assoc_resp_c* /* frame */ )
    {
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_wpx_adaptation_stub_c::handle_ts_create_success(
    core_frame_dot11_c* /* frame */,
    u8_t /* tid */,
    u8_t /* user_priority */ )
    {
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_wpx_adaptation_stub_c::handle_ts_delete(
    u8_t /* tid */,
    u8_t /* user_priority */ )
    {
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_wpx_adaptation_stub_c::is_wpx_management_status(
    u32_t /* management_status */ )
    {
    return false_t;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_wpx_adaptation_stub_c::is_fatal_wpx_management_status(
    u32_t /* management_status */ )
    {
    return false_t;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_encryption_mode_e core_wpx_adaptation_stub_c::encryption_mode(
    core_ap_data_c& /* ap_data */,
    core_encryption_mode_e mode )
    {
    return mode;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u16_t core_wpx_adaptation_stub_c::authentication_algorithm(
    wlan_eapol_if_eapol_key_authentication_type_e /* eapol_auth_type */,
    wlan_eapol_if_eapol_key_authentication_mode_e /* eapol_auth_mode */ )
    {
    DEBUG( "core_wpx_adaptation_stub_c::authentication_algorithm() - using open authentication algorithm" );
    return core_authentication_mode_open;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_operation_base_c* core_wpx_adaptation_stub_c::get_wpx_load_drivers_operation()
    {
    return NULL;
    }
