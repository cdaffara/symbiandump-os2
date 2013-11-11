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
* Description:  Simple parser functions for core
*
*/

/*
* %version: 37 %
*/

#include "core_tools_parser.h"
#include "core_tools.h"
#include "core_frame_beacon.h"
#include "core_frame_dot11.h"
#include "core_frame_wmm_ie.h"
#include "core_frame_wmm_ie_tspec.h"
#include "core_frame_qbss_load_ie.h"
#include "am_debug.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_connect_status_e core_tools_parser_c::is_ap_compatible_with_iap(
    abs_core_wpx_adaptation_c& wpx_adaptation,
    core_ap_data_c& ap_data,
    core_iap_data_c& iap_data,
    core_settings_c& core_settings,
    bool_t is_cm_active,
    bool_t ignore_channels )
    {
    const core_mac_address_s mac = ap_data.bssid();

    DEBUG6( "core_tools_parser_c::is_ap_compatible_with_iap() - BSSID %02X:%02X:%02X:%02X:%02X:%02X",
        mac.addr[0], mac.addr[1], mac.addr[2],
        mac.addr[3], mac.addr[4], mac.addr[5] );

    if ( iap_data.operating_mode() == core_operating_mode_infrastructure &&
         !ap_data.is_infra() )
        {
        DEBUG( "core_tools_parser_c::is_ap_compatible_with_iap() - IAP is infrastructure, AP is IBSS" );
        return core_connect_mode_infra_required_but_ibss_found;
        }
    else if ( iap_data.operating_mode() == core_operating_mode_ibss &&
              ap_data.is_infra() )
        {
        DEBUG( "core_tools_parser_c::is_ap_compatible_with_iap() - IAP is IBSS, AP is infrastructure" );
        return core_connect_mode_ibss_required_but_infra_found;
        }

    if ( !core_settings.is_permanent_whitelist_empty() ||
         !iap_data.is_iap_whitelist_empty() )
        {
        if ( !core_settings.is_mac_in_permanent_whitelist(mac ) &&
             !iap_data.is_mac_in_iap_whitelist( mac ) )
            {
            DEBUG( "core_tools_parser_c::is_ap_compatible_with_iap() - whitelist defined, not in list" );
            return core_connect_ap_not_whitelisted;
            }
        }

    if ( core_settings.is_mac_in_permanent_blacklist( mac ) ||
         iap_data.is_mac_in_iap_blacklist( mac ) )
        {
        DEBUG( "core_tools_parser_c::is_ap_compatible_with_iap() - blacklisted" );
        return core_connect_ap_permanently_blacklisted;
        }

    if ( !ignore_channels )
    	{
        if ( !core_settings.is_valid_channel(
            ap_data.band(), ap_data.channel() ) )
            {
            DEBUG( "core_tools_parser_c::is_ap_compatible_with_iap() - invalid channel" );
            return core_connect_ap_outside_defined_region;
            }
    	}

    core_connect_status_e wpx_reason( core_connect_ok );

    switch ( iap_data.security_mode() )
        {
        case core_security_mode_allow_unsecure:
            /**
             * Check WPX requirements.
             */
            if ( !wpx_adaptation.is_ap_wpx_compatible_with_iap(
                wpx_reason,
                ap_data,
                iap_data ) )
                {
                return wpx_reason;
                }

            /**
             * The only requirement in this mode is that the AP doesn't have privacy enabled.
             */
            if ( ap_data.is_privacy_enabled() )
                {
                DEBUG( "core_tools_parser_c::is_ap_compatible_with_iap() - AP has privacy enabled" );
                return core_connect_iap_open_but_ap_requires_encryption;
                }

            break;
        case core_security_mode_wep:
            /**
             * Check WPX requirements.
             */
            if ( !wpx_adaptation.is_ap_wpx_compatible_with_iap(
                wpx_reason,
                ap_data,
                iap_data ) )
                {
                return wpx_reason;
                }

            /**
             * Privacy has to be enabled.
             */
            if ( !ap_data.is_privacy_enabled() )
                {
                DEBUG( "core_tools_parser_c::is_ap_compatible_with_iap() - AP doesn't have privacy enabled" );
                return core_connect_iap_wep_but_ap_has_no_privacy;
                }

            /**
             * If WPA IE or RSN IE is present, static WEP is not supported.
             */
            if ( ap_data.is_wpa_ie_present() ||
                 ap_data.is_rsn_ie_present() )
                {
                DEBUG( "core_tools_parser_c::is_ap_compatible_with_iap() - AP has WPA/RSN IE present" );
                return core_connect_iap_wep_but_ap_has_wpa_ie;
                }

            break;
        case core_security_mode_802dot1x_unencrypted:
            /** Falls through on purpose. */
        case core_security_mode_802dot1x:
            /**
             * Check WPX requirements.
             */
            if ( !wpx_adaptation.is_ap_wpx_compatible_with_iap(
                wpx_reason,
                ap_data,
                iap_data ) )
                {
                return wpx_reason;
                }

            /**
             * If WPA IE or RSN IE is present, we must check that AP supports EAP or WPX fast-roam.
             */
            if ( ap_data.is_wpa_ie_present() ||
                 ap_data.is_rsn_ie_present() )
                {
                u32_t key_management = ap_data.key_management_suites() &
                    ( core_key_management_eap | core_key_management_wpx_fast_roam );
                if ( !key_management )
                    {
                    DEBUG( "core_tools_parser_c::is_ap_compatible_with_iap() - no EAP as key management" );
                    return core_connect_wpa_eap_required_but_ap_has_no_support;
                    }
                }

            break;
        case core_security_mode_wpa:
            {
            /**
             * Check WPX requirements.
             */
            if ( !wpx_adaptation.is_ap_wpx_compatible_with_iap(
                wpx_reason,
                ap_data,
                iap_data ) )
                {
                return wpx_reason;
                }

            /**
             * In this mode WPA IE or RSN IE must be present and we must have valid
             * ciphers (TKIP or CCMP) and a valid key management suite (EAP or PSK).
             * Privacy is required.
             */
            if ( !ap_data.is_privacy_enabled() )
                {
                DEBUG( "core_tools_parser_c::is_ap_compatible_with_iap() - AP doesn't have privacy enabled" );
                return core_connect_iap_wpa_but_ap_has_no_privacy;
                }

            if ( !ap_data.is_wpa_ie_present() &&
                 !ap_data.is_rsn_ie_present() )
                {
                DEBUG( "core_tools_parser_c::is_ap_compatible_with_iap() - no WPA IE or RSN IE present" );
                return core_connect_wpa_ie_required_but_ap_has_none;
                }

            if ( iap_data.is_psk_used() )
                {
                u32_t key_management = ap_data.key_management_suites() &
                    core_key_management_preshared;
                if ( !key_management )
                    {
                    DEBUG( "core_tools_parser_c::is_ap_compatible_with_iap() - no PSK as key management" );
                    return core_connect_wpa_psk_required_but_ap_has_no_support;
                    }
                }
            else
                {
                u32_t key_management = ap_data.key_management_suites() &
                    core_key_management_eap;
                if ( !key_management )
                    {
                    DEBUG( "core_tools_parser_c::is_ap_compatible_with_iap() - no EAP as key management" );
                    return core_connect_wpa_eap_required_but_ap_has_no_support;
                    }
                }

            u32_t pairwise_ciphers = ap_data.pairwise_ciphers() &
                ( core_cipher_suite_tkip | core_cipher_suite_ccmp );
            u32_t group_cipher = ap_data.group_cipher() &
                ( core_cipher_suite_tkip | core_cipher_suite_ccmp );
            if ( !pairwise_ciphers ||
                 !group_cipher )
                {
                DEBUG( "core_tools_parser_c::is_ap_compatible_with_iap() - no valid ciphers" );
                return core_connect_wpa_ap_has_no_valid_ciphers;
                }

            if ( is_cm_active &&
                 ( ap_data.best_pairwise_cipher() == core_cipher_suite_tkip ||
                   ap_data.best_group_cipher() == core_cipher_suite_tkip ) )
                {
                DEBUG( "core_tools_parser_c::is_ap_compatible_with_iap() - WPA counter measures active" );
                return core_connect_wpa_counter_measures_active;
                }

            break;
            }
        case core_security_mode_wpa2only:
            {
            /**
             * Check WPX requirements.
             */
            if ( !wpx_adaptation.is_ap_wpx_compatible_with_iap(
                wpx_reason,
                ap_data,
                iap_data ) )
                {
                return wpx_reason;
                }

            /**
             * RSN IE, CCMP cipher and valid key management suite (EAP or PSK) are required in
             * this mode. Privacy is required.
             */
            if ( !ap_data.is_privacy_enabled() )
                {
                DEBUG( "core_tools_parser_c::is_ap_compatible_with_iap() - AP doesn't have privacy enabled" );
                return core_connect_iap_wpa_but_ap_has_no_privacy;
                }

            if ( !ap_data.is_rsn_ie_present() )
                {
                DEBUG( "core_tools_parser_c::is_ap_compatible_with_iap() - no RSN IE present" );
                return core_connect_wpa_ie_required_but_ap_has_none;
                }

            if ( iap_data.is_psk_used() )
                {
                u32_t key_management = ap_data.key_management_suites() &
                    core_key_management_preshared;
                if ( !key_management )
                    {
                    DEBUG( "core_tools_parser_c::is_ap_compatible_with_iap() - no PSK as key management" );
                    return core_connect_wpa_psk_required_but_ap_has_no_support;
                    }
                }
            else
                {
                u32_t key_management = ap_data.key_management_suites() &
                    core_key_management_eap;
                if ( !key_management )
                    {
                    DEBUG( "core_tools_parser_c::is_ap_compatible_with_iap() - no EAP as key management" );
                    return core_connect_wpa_eap_required_but_ap_has_no_support;
                    }
                }

            u32_t pairwise_ciphers = ap_data.pairwise_ciphers() &
                core_cipher_suite_ccmp;
            u32_t group_cipher = ap_data.group_cipher() &
                core_cipher_suite_ccmp;
            if ( !pairwise_ciphers ||
                 !group_cipher )
                {
                DEBUG( "core_tools_parser_c::is_ap_compatible_with_iap() - CCMP not supported" );
                return core_connect_wpa_ap_has_no_valid_ciphers;
                }

            break;
            }
        case core_security_mode_wapi:
            {
            /**
             * Check WPX requirements.
             */
            if ( !wpx_adaptation.is_ap_wpx_compatible_with_iap(
                wpx_reason,
                ap_data,
                iap_data ) )
                {
                return wpx_reason;
                }

            /**
             * In this mode WAPI IE must be present and we must have valid ciphers
             * and a valid key management suite. Privacy is required.
             */
            if ( !ap_data.is_privacy_enabled() )
                {
                DEBUG( "core_tools_parser_c::is_ap_compatible_with_iap() - AP doesn't have privacy enabled" );
                return core_connect_iap_wapi_but_ap_has_no_privacy;
                }

            if ( !ap_data.is_wapi_ie_present() )
                {
                DEBUG( "core_tools_parser_c::is_ap_compatible_with_iap() - no WAPI IE present" );
                return core_connect_wapi_ie_required_but_ap_has_none;
                }

            if ( iap_data.is_psk_used() )
                {
                u32_t key_management = ap_data.key_management_suites() &
                    core_key_management_wapi_psk;
                if ( !key_management )
                    {
                    DEBUG( "core_tools_parser_c::is_ap_compatible_with_iap() - no WAPI PSK as key management" );
                    return core_connect_wapi_psk_required_but_ap_has_no_support;
                    }
                }
            else
                {
                u32_t key_management = ap_data.key_management_suites() &
                    core_key_management_wapi_certificate;
                if ( !key_management )
                    {
                    DEBUG( "core_tools_parser_c::is_ap_compatible_with_iap() - no WAPI certificate as key management" );
                    return core_connect_wapi_certificate_required_but_ap_has_no_support;
                    }
                }

            u32_t pairwise_ciphers = ap_data.pairwise_ciphers() &
                core_cipher_suite_wpi;
            u32_t group_cipher = ap_data.group_cipher() &
                core_cipher_suite_wpi;
            if ( !pairwise_ciphers ||
                 !group_cipher )
                {
                DEBUG( "core_tools_parser_c::is_ap_compatible_with_iap() - no valid ciphers" );
                return core_connect_wapi_ap_has_no_valid_ciphers;
                }

            break;
            }

        }

    DEBUG( "core_tools_parser_c::is_ap_compatible_with_iap() - OK" );
    return core_connect_ok;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_connect_status_e core_tools_parser_c::is_ap_suitable(
    abs_core_wpx_adaptation_c& wpx_adaptation,
    core_ap_data_c& ap_data,
    core_iap_data_c& iap_data,
    core_settings_c& core_settings,
    core_connection_data_c& connection_data,
    u8_t min_rcpi,
    const medium_time_s& min_medium_time,
    bool_t is_cm_active,
    const core_mac_address_s& current_bssid )
    {
    const core_mac_address_s bssid(
        ap_data.bssid() );

    if ( current_bssid == bssid )
        {
        DEBUG( "core_tools_parser_c::is_ap_suitable() - currently connected to this AP" );
        return core_connect_unspecified_failure;
        }

    if ( connection_data.is_mac_in_temporary_blacklist( bssid ) )
        {
        DEBUG( "core_tools_parser_c::is_ap_suitable() - AP is temporarily blacklisted" );
        return core_connect_ap_temporarily_blacklisted;
        }

    core_connect_status_e ret = core_tools_parser_c::is_ap_compatible_with_iap(
        wpx_adaptation,
        ap_data,
        iap_data,
        core_settings,
        is_cm_active );
    if ( ret != core_connect_ok )
        {
        DEBUG( "core_tools_parser_c::is_ap_suitable() - IAP doesn't match the AP" );
        return ret;
        }

    if ( min_rcpi != RCPI_VALUE_NONE &&
         ap_data.rcpi() < min_rcpi )
        {
        DEBUG2( "core_tools_parser_c::is_ap_suitable() - signal too weak (RCPI is %u, required RCPI is %u)",
            ap_data.rcpi(), min_rcpi );
        return core_connect_ap_signal_too_weak;
        }

    bool_t is_ac_required_for_voice =
        connection_data.traffic_stream_list().is_traffic_stream_for_access_class( core_access_class_voice );
    bool_t is_ac_required_for_video =
        connection_data.traffic_stream_list().is_traffic_stream_for_access_class( core_access_class_video );
    bool_t is_ac_required_for_best_effort =
        connection_data.traffic_stream_list().is_traffic_stream_for_access_class( core_access_class_best_effort );
    bool_t is_ac_required_for_background =
        connection_data.traffic_stream_list().is_traffic_stream_for_access_class( core_access_class_background );

    if ( min_medium_time != MEDIUM_TIME_NOT_DEFINED )
        {
        ASSERT( is_ac_required_for_voice ||
            is_ac_required_for_video ||
            is_ac_required_for_best_effort ||
            is_ac_required_for_background );

        if ( !ap_data.is_wmm_ie_present() )
            {
            DEBUG( "core_tools_parser_c::is_ap_suitable() - AP doesn't have a WMM IE" );
            return core_connect_ap_has_no_admission_control;
            }

        if ( is_ac_required_for_voice &&
             !ap_data.is_admission_control_required( core_access_class_voice ) )
            {
            DEBUG( "core_tools_parser_c::is_ap_suitable() - AP doesn't require admission control for Voice" );
            return core_connect_ap_has_no_admission_control;
            }

        if ( is_ac_required_for_video &&
             !ap_data.is_admission_control_required( core_access_class_video ) )
            {
            DEBUG( "core_tools_parser_c::is_ap_suitable() - AP doesn't require admission control for Video" );
            return core_connect_ap_has_no_admission_control;
            }

        if ( is_ac_required_for_best_effort &&
             !ap_data.is_admission_control_required( core_access_class_best_effort ) )
            {
            DEBUG( "core_tools_parser_c::is_ap_suitable() - AP doesn't require admission control for Background" );
            return core_connect_ap_has_no_admission_control;
            }

        if ( is_ac_required_for_background &&
             !ap_data.is_admission_control_required( core_access_class_background ) )
            {
            DEBUG( "core_tools_parser_c::is_ap_suitable() - AP doesn't require admission control for BestEffort" );
            return core_connect_ap_has_no_admission_control;
            }

        bool_t is_adm_capacity_found( false_t );
        medium_time_s admission_capacities(
            ADMISSION_CAPACITIES_NOT_DEFINED );
        ap_data.admission_capacity( admission_capacities );

        /**
         * Check required medium time by user priority.
         */
        for( u8_t idx( 0 ); idx < MAX_QOS_USER_PRIORITY; ++idx )
            {
            if( min_medium_time.up[idx] )
                {
                if( admission_capacities.up[idx] < min_medium_time.up[idx] )
                    {
                    DEBUG3( "core_tools_parser_c::is_ap_suitable() - not enough admission capacity for UP %u: %u, required %u",
                        idx, admission_capacities.up[idx], min_medium_time.up[idx] );

                    return core_connect_ap_has_no_free_admission_capability;
                    }
                else if( admission_capacities.up[idx] != ADMISSION_CAPACITY_NOT_DEFINED )
                    {
                    DEBUG3( "core_tools_parser_c::is_ap_suitable() - admission capacity for UP %u: %u, required %u",
                        idx, admission_capacities.up[idx], min_medium_time.up[idx] );

                    is_adm_capacity_found = true_t;
                    }
                else
                    {
                    DEBUG1( "core_tools_parser_c::is_ap_suitable() - no admission capacity advertised for UP %u",
                        idx );
                    }
                }
            }

        /**
         * Check required medium time by access class.
         */
        u16_t total_min_medium_time( 0 );
        for( u8_t idx( 0 ); idx < MAX_QOS_ACCESS_CLASS; ++idx )
            {           
            if( min_medium_time.ac[idx] )
                {
                total_min_medium_time += min_medium_time.ac[idx];

                if( admission_capacities.ac[idx] < min_medium_time.ac[idx] )
                    {
                    DEBUG3( "core_tools_parser_c::is_ap_suitable() - not enough admission capacity for AC %u: %u, required %u",
                        idx, admission_capacities.ac[idx], min_medium_time.ac[idx] );

                    return core_connect_ap_has_no_free_admission_capability;
                    }
                else if( admission_capacities.ac[idx] != ADMISSION_CAPACITY_NOT_DEFINED )
                    {
                    DEBUG3( "core_tools_parser_c::is_ap_suitable() - admission capacity for AC %u: %u, required %u",
                        idx, admission_capacities.ac[idx], min_medium_time.ac[idx] );

                    is_adm_capacity_found = true_t;
                    }
                else
                    {
                    DEBUG1( "core_tools_parser_c::is_ap_suitable() - no admission capacity advertised for AC %u",
                        idx );
                    }
                }
            }

        if( !is_adm_capacity_found )            
            {
            const core_frame_qbss_load_ie_c* qbss_load_ie = ap_data.qbss_load_ie();
            if ( qbss_load_ie &&
                 qbss_load_ie->admission_capacity() < total_min_medium_time )
                {
                is_adm_capacity_found = true_t;
                
                DEBUG2( "core_tools_parser_c::is_ap_suitable() - not enough admission capacity (%u, required %u)",
                    qbss_load_ie->admission_capacity(), total_min_medium_time );
                delete qbss_load_ie;

                return core_connect_ap_has_no_free_admission_capability;
                }

            delete qbss_load_ie;
            qbss_load_ie = NULL;
            }

        /**
         * If the AP doesn't advertise any admission capacity information,
         * we'll assume there's enough capacity.
         */
        if( !is_adm_capacity_found )
            {
            DEBUG( "core_tools_parser_c::is_ap_suitable() - AP doesn't advertise any admission capacity information" );
            }
        }

    DEBUG( "core_tools_parser_c::is_ap_suitable() - AP is suitable" );
    return core_connect_ok;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_ap_information_s core_tools_parser_c::get_ap_info(
    const core_iap_data_c& iap_data,
    core_ap_data_c& ap_data )
    {
    core_ap_information_s info;
    core_tools_c::fillz(
        &info,
        sizeof( info ) );

    info.ssid = ap_data.ssid();
    info.bssid = ap_data.bssid();
    info.capabilities = ap_data.capabilities();
    info.channel = ap_data.channel();
    info.rcpi = ap_data.rcpi();
    info.security_mode = core_tools_c::security_mode(
        iap_data,
        ap_data );

    /**
     * Parse basic and supported rates to own bitmaps.
     */
    info.basic_rates = 0;
    info.supported_rates = 0;
    for( core_frame_dot11_ie_c* ie = ap_data.frame()->first_ie(); ie; ie = ap_data.frame()->next_ie() )
        {
        if ( ie->element_id() == core_frame_dot11_ie_c::core_frame_dot11_ie_element_id_supported_rates ||
             ie->element_id() == core_frame_dot11_ie_c::core_frame_dot11_ie_element_id_extended_rates )
            {
            u32_t temp_basic_rates( 0 );
            u32_t temp_supported_rates( 0 );

            core_tools_parser_c::parse_rates(
                ie->data() + CORE_FRAME_DOT11_IE_HEADER_LENGTH,
                ie->length(),
                temp_basic_rates,
                temp_supported_rates );

            info.basic_rates |= temp_basic_rates;
            info.supported_rates |= temp_supported_rates;
            }

        delete ie;
        }
    info.is_ac_required_for_voice =
        ap_data.is_admission_control_required( core_access_class_voice );
    info.is_ac_required_for_video =
        ap_data.is_admission_control_required( core_access_class_video );
    info.is_ac_required_for_best_effort =
        ap_data.is_admission_control_required( core_access_class_best_effort );
    info.is_ac_required_for_background =
        ap_data.is_admission_control_required( core_access_class_background );
    info.is_wpx = ap_data.is_wpx();

    return info;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_tools_parser_c::get_wmm_traffic_stream_params(
    core_frame_dot11_c& frame,
    u8_t tid,
    core_traffic_stream_params_s& tspec )
    {
    for( core_frame_dot11_ie_c* ie = frame.first_ie(); ie; ie = frame.next_ie() )
        {
        if ( ie->element_id() == core_frame_dot11_ie_c::core_frame_dot11_ie_element_id_wmm_tspec )
            {
            core_frame_wmm_ie_tspec_c* tspec_ie = core_frame_wmm_ie_tspec_c::instance( *ie );
            if ( tspec_ie &&
                 tspec_ie->tid() == tid )
                {
                DEBUG1( "core_tools_parser_c::get_wmm_traffic_stream_params() - TSPEC with TID %u found",
                    tid );

                tspec.is_periodic_traffic = tspec_ie->is_periodic_traffic();
                tspec.direction = tspec_ie->direction();
                tspec.nominal_msdu_size = tspec_ie->nominal_msdu_size();
                tspec.maximum_msdu_size = tspec_ie->maximum_msdu_size();
                tspec.minimum_service_interval = tspec_ie->minimum_service_interval();
                tspec.maximum_service_interval = tspec_ie->maximum_service_interval();
                tspec.inactivity_interval = tspec_ie->inactivity_interval();
                tspec.suspension_interval = tspec_ie->suspension_interval();
                tspec.service_start_time = tspec_ie->service_start_time();
                tspec.minimum_data_rate = tspec_ie->minimum_data_rate();
                tspec.mean_data_rate = tspec_ie->mean_data_rate();
                tspec.peak_data_rate = tspec_ie->peak_data_rate();
                tspec.maximum_burst_size = tspec_ie->maximum_burst_size();
                tspec.delay_bound = tspec_ie->delay_bound();
                tspec.minimum_phy_rate = tspec_ie->minimum_phy_rate();
                tspec.surplus_bandwidth_allowance = tspec_ie->surplus_bandwidth_allowance();
                tspec.medium_time = tspec_ie->medium_time();

                delete tspec_ie;
                tspec_ie = NULL;

                delete ie;
                ie = NULL;

                return true_t;
                }

            delete tspec_ie;
            tspec_ie = NULL;
            }

        delete ie;
        ie = NULL;
        }

    return false_t;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_tools_parser_c::parse_rates(
    const u8_t* data,
    u8_t data_length,
    u32_t& basic_rates,
    u32_t& supported_rates )
    {
    DEBUG( "core_tools_parser_c::parse_rates()" );

    basic_rates = 0;
    supported_rates = 0;

    for( u8_t idx( 0 ); idx < data_length; ++idx )
        {
        core_tx_rate_e rate(
            core_tools_c::convert_tx_rate_to_tx_rate_enum(
                data[idx] & ~TX_RATE_BASIC_MASK ) );

        supported_rates |= rate;

        if ( data[idx] & TX_RATE_BASIC_MASK )
            {
            /**
             * The highest bit is enabled, the rate is both a basic rate
             * and a supported rate.
             */
            basic_rates |= rate;
            }
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_tools_parser_c::core_tools_parser_c()
    {
    DEBUG( "core_tools_parser_c::core_tools_parser_c()" );
    }
