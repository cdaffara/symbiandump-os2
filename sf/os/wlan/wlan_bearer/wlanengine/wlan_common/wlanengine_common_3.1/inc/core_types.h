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
* Description:  Type definitions and constants for core engine
*
*/

/*
* %version: 100 %
*/

#ifndef CORE_TYPES_H
#define CORE_TYPES_H

#include "am_platform_libraries.h"

/** The maximum length of SSID. */
const u8_t MAX_SSID_LEN = 32;

/** The maximum length of MAC address. */
const u8_t MAC_ADDR_LEN = 6;

/** The maximum length of a timestamp (TSF). */ 
const u8_t TSF_VALUE_LEN = 8;

/**
 * The maximum length of a WEP key.
 *
 * 40-bit (64-bit) key has the length of 5.
 * 104-bit (128-bit) key has the length of 13.
 * 232-bit (256-bit) key has the length of 29.
 **/
const u8_t MAX_WEP_KEY_LENGTH = 29;

/**
 * The maximum length of a WPA-PSK key.
 *
 * WPA-PSK passphrase is limited to 8 - 63 characters
 * in the range of 32 to 126 (decimal), inclusive.
 *
 * Length of 64 indicates the actual hashed PSK.
 */
const u8_t MAX_WPA_PSK_LENGTH = 64;

/** The maximum length of a cipher key (256 bits). */
const u8_t MAX_CIPHER_KEY_LENGTH = 32;

/** The maximum length of a RSN/WPA IE. */
const u8_t MAX_WPAIE_LENGTH = 255;

const u8_t MAX_RATES = 40;

const u8_t TX_RATE_BASIC_MASK = 0x80;

/** Definitions for Capability bitmask in beacons/probe responses. */
const u16_t CAPABILITY_BIT_MASK_ESS       = 0x0001;
const u16_t CAPABILITY_BIT_MASK_IBSS      = 0x0002;
const u16_t CAPABILITY_BIT_MASK_PRIVACY   = 0x0010;
const u16_t CAPABILITY_BIT_MASK_PREAMBLE  = 0x0020;
const u16_t CAPABILITY_BIT_MASK_SLOT_TIME = 0x0400;

const u8_t SCAN_BAND_2DOT4GHZ = 1;
const u8_t SCAN_BAND_4DOT9GHZ = 2;
const u8_t SCAN_BAND_5GHZ = 4;

const u8_t MAX_TX_POWER_LEVEL_NOT_DEFINED = 0xFF;
const u32_t MAX_TX_MSDU_LIFETIME_NOT_DEFINED = 0;
const u16_t ADMISSION_CAPACITY_NOT_DEFINED = 0xFFFF;

const u32_t REQUEST_ID_CORE_INTERNAL = 0;

const u32_t WEP_KEY1 = 0;
const u32_t WEP_KEY2 = 1;
const u32_t WEP_KEY3 = 2;
const u32_t WEP_KEY4 = 3;

const u32_t EAP_TYPE_NONE = 0;
const u32_t EAP_ERROR_NONE = 0;

const u8_t RCPI_VALUE_NONE = 0;

const u8_t MAX_COUNTRY_STRING_LENGTH = 3;

const u8_t SCAN_BAND_2DOT4GHZ_MAX_CHANNEL_FCC = 11;
const u8_t SCAN_BAND_2DOT4GHZ_MAX_CHANNEL_EURO = 13;

/** Timer value for immediate queue execution. */
const u32_t CORE_TIMER_IMMEDIATELY = 0;

const u32_t SECONDS_FROM_MICROSECONDS = 1000000;
const u32_t SECONDS_FROM_MILLISECONDS = 1000;
const u32_t MILLISECONDS_FROM_MICROSECONDS = 1000;
const u32_t TIMEUNITS_FROM_MICROSECONDS = 1024;

/** The maximum number of TX rate policies. */
const u8_t MAX_TX_RATE_POLICY = 4;

/** The size of MCS information in a TX rate policy. */
const u8_t TX_RATE_POLICY_MCS_SIZE = 10;

/** The number of QoS access classes. */
const u8_t MAX_QOS_ACCESS_CLASS = 4;

/** The number of QoS user priority values. */
const u8_t MAX_QOS_USER_PRIORITY = 8;

/** The maximum and minimum values of different integers. */
const u32_t MAX_U32 = 0xFFFFFFFF;
const u32_t MIN_U32 = 0;
const i32_t MAX_I32 = 0x7FFFFFFF;
const i32_t MIN_I32 = static_cast<i32_t>( 0x80000000 );

const u32_t SCAN_LIST_AGE_ANY = MAX_U32;
const u32_t SCAN_LIST_AGE_LAST_SCAN = MIN_U32;

/** The length of OUI. */
const u8_t OUI_LENGTH = 3;

/** The value for undefined virtual traffic stream TID. */
const u8_t TRAFFIC_STREAM_TID_NONE = 0xFF;

/** The number of traffic stream TID values. */ 
const u8_t MAX_TRAFFIC_STREAM_TID = 8;

typedef enum _core_scan_list_tag_e
    {
    core_scan_list_tag_none = 0x00,
    core_scan_list_tag_scan = 0x01,
    core_scan_list_tag_direct_scan = 0x02,
    core_scan_list_tag_roam_scan = 0x04,
    core_scan_list_tag_ibss_merge = 0x08
    } core_scan_list_tag_e;

/**
 * Defines the possible 802.11 authentication algorithms.
 */
typedef enum _core_authentication_mode_e
    {
    /** 802.11 Open System authentication algorithm. */
    core_authentication_mode_open = 0,
    /** 802.11 Shared Key authentication algorithm. */
    core_authentication_mode_shared = 1,
    } core_authentication_mode_e;

/**
 * Definitions for the possible states indicated by core engine.
 */
typedef enum _core_connection_state_e
    {
    /** Connection is not active. No data flow. */
    core_connection_state_notconnected,
    /** Connection to an access point is active. */
    core_connection_state_infrastructure,
    /** Searching an access point. No data flow. */
    core_connection_state_searching,
    /** IBSS network is active. */
    core_connection_state_ibss,
    /** Secure connection to an access point is active. */
    core_connection_state_secureinfra,
    /** Secure IBSS network is active. */
    core_connection_state_secureibss
    } core_connection_state_e;

/**
 * Security mode according to IAP definition
 */
typedef enum _core_security_mode_e
    {
    /** No encryption used. */
    core_security_mode_allow_unsecure,
    /** Use WEP encryption with static keys. */
    core_security_mode_wep,
    /** Use WEP encryption, keys are negotiated by EAPOL. */
    core_security_mode_802dot1x,
    /** Use WEP/TKIP/CCMP encryption, keys are negotiated by EAPOL. */
    core_security_mode_wpa,
    /** Use CCMP encryption, keys are negotiated by EAPOL. */
    core_security_mode_wpa2only,
    /** Use Wi-Fi Protected Setup, keys are negotiated by EAPOL. */
    core_security_mode_protected_setup,
    /** Use WAPI, keys are negotiated by EAPOL. */
    core_security_mode_wapi,
    /** Use EAP authentication without encryption. */
    core_security_mode_802dot1x_unencrypted
    } core_security_mode_e;

/**
 * Security mode of the current connection
 */
typedef enum _core_connection_security_mode_e
    {
    /** No security used. */
    core_connection_security_mode_open,
    /** Use Open WEP encryption with static keys. */
    core_connection_security_mode_wep_open,
    /** Use Shared WEP encryption with static keys. */
    core_connection_security_mode_wep_shared,
    /** Use WEP/TKIP/CCMP encryption, keys are negotiated by EAPOL. */
    core_connection_security_mode_802d1x,
    /** Use TKIP/CCMP encryption, keys are negotiated by EAPOL. */
    core_connection_security_mode_wpa,
    /** WPA with preshared key. */
    core_connection_security_mode_wpa_psk,
    /** Use CCMP encryption, keys are negotiated by EAPOL. */
    core_connection_security_mode_wpa2,
    /** WPA2 with preshared key. */
    core_connection_security_mode_wpa2_psk,
    /** WAPI with certificates. */
    core_connection_security_mode_wapi,
    /** WAPI with preshared key. */
    core_connection_security_mode_wapi_psk    
    } core_connection_security_mode_e;

/**
 * The possible encryption modes.
 */
typedef enum _core_encryption_mode_e
    {
    /** No encryption used. */
    core_encryption_mode_disabled,
    /** Use WEP encryption with static keys. */
    core_encryption_mode_wep,
    /** Use WEP encryption with static keys in mixed cell. */
    core_encryption_mode_wep_mixed_cell,
    /** Use WEP encryption, keys are negotiated by EAPOL. */
    core_encryption_mode_802dot1x,
    /** Use WEP encryption in mixed cell, keys are negotiated by EAPOL. */
    core_encryption_mode_802dot1x_mixed_cell,
    /** Use WEP/TKIP/CCMP encryption, keys are negotiated by EAPOL. */
    core_encryption_mode_wpa,
    /** Use WPI encryption, keys are negotiated by EAPOL. */
    core_encryption_mode_wpi
    } core_encryption_mode_e;

/**
 * Defines all the possible indications that can be received from adaptation.
 */
typedef enum _core_am_indication_e
    {
    /**
     * AP has deauthenticated the station.
     */
    core_am_indication_wlan_media_disconnect,
    /**
     * A fatal error has occurred in the lower layer.
     */
    core_am_indication_wlan_hw_failed,
    /**
     * A number of consecutive beacons has been lost.
     *
     * @see core_bss_lost_parameters_s::beacon_lost_count
     */
    core_am_indication_wlan_beacon_lost,
    /**
     * UMAC has not been able to perform a power mode state transition.
     */
    core_am_indication_wlan_power_mode_failure,
    /**
     * A number of consecutive transmissions has failed.
     *
     * @see core_bss_lost_parameters_s::failed_tx_packet_count
     */
    core_am_indication_wlan_tx_fail,
    /**
     * Connection to the BSS has been regained. This indication can occur after
     * core_am_indication_wlan_beacon_lost,
     * core_am_indication_wlan_power_mode_failure or
     * core_am_indication_wlan_tx_fail.
     */
    core_am_indication_wlan_bss_regained,
    /**
     * Decryption of a received packet failed.
     */
    core_am_indication_wlan_wep_decrypt_failure,
    /**
     * A MIC failure has been detected on a received unicast packet.
     */
    core_am_indication_wlan_pairwise_key_mic_failure,
    /**
     * A MIC failure has been detected on a received broadcast
     * or multicast packet.
     */
    core_am_indication_wlan_group_key_mic_failure,
    /**
     * Scan has been completed.
     */
    core_am_indication_wlan_scan_complete,
    /**
     * Used to indicate that the RCPI trigger level has been been crossed.
     * This indication is one-shot, it will not be triggered again until
     * the level is set again.
     *
     * @see abs_core_driverif_callback_c::set_rcpi_trigger_level
     */
    core_am_indication_wlan_rcpi_trigger,
    /**
     * Signal loss prediction algorithm has indicated an imminent signal
     * loss.
     */
    core_am_indication_wlan_signal_loss_prediction,
    /**
     * UMAC has switched to power save mode after (re-)assocation, indicating
     * a possibility to run the power save test.
     */
    core_am_indication_wlan_power_save_test_trigger,
    /**
     * The lower layer has detected problems with power save in the
     * current access point.
     */
    core_am_indication_wlan_ap_ps_mode_error,
    /**
     * Indication of an imminent power loss.
     */
    core_am_indication_os_power_standby,
    /**
     * Bluetooth connection has been established.
     */
    core_am_indication_bt_connection_established,
    /**
     * Bluetooth connection has been disconnected.
     */
    core_am_indication_bt_connection_disconnected,
    /**
     * Indication that voice call is on.
     */
    core_am_indication_voice_call_on,
    /**
     * Indication that voice call is off.
     */
    core_am_indication_voice_call_off
    } core_am_indication_e;

/**
 * Structure for storing a MAC address.
 */
typedef struct _core_mac_address_s
    {
    u8_t addr[MAC_ADDR_LEN];
    } core_mac_address_s;

/**
 * Definition for an undefined MAC address.
 */
const core_mac_address_s ZERO_MAC_ADDR = {{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }};

/**
 * Definition for a broadcast MAC address.
 */
const core_mac_address_s BROADCAST_MAC_ADDR = {{ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }};

/**
 * Structure for storing a timestamp (TSF) from a beacon/probe response.
 */
typedef struct _core_tsf_value_s
    {
    u8_t tsf[TSF_VALUE_LEN];    
    } core_tsf_value_s;

/**
 * Data values for operating modes.
 */
typedef enum _core_operating_mode_e
    {
    /** The BSS is operating in infrastructure mode. */
    core_operating_mode_infrastructure = CAPABILITY_BIT_MASK_ESS,
    /** The BSS is operating in IBSS (ad-hoc) mode. */
    core_operating_mode_ibss           = CAPABILITY_BIT_MASK_IBSS
    } core_operating_mode_e;

/**
 * The possible scan modes.
 */
typedef enum _core_scan_mode_e
    {
    /** Send probe requests and listen to beacons and probe responses. */ 
    core_scan_mode_active,
    /** Listen to beacons and probe responses. */
    core_scan_mode_passive
    } core_scan_mode_e;

/**
 * Structure for storing an SSID.
 */
typedef struct _core_ssid_s
    {
    /** Length of the SSID. */
    u32_t length;
    /** SSID data. */
    u8_t ssid[MAX_SSID_LEN];
    } core_ssid_s;

/**
 * Definition for a broadcast SSID.
 */
const core_ssid_s BROADCAST_SSID = { 0, { 0x00 }};

/**
 * Data structure for storing a WEP key.
 */
typedef struct _core_wep_key_s
    {
    /** Specifies which key to add or remove. Range: 0-3. */
    u32_t key_index;
    /** The length of KeyMaterial in bytes. Range: 0-29. */
    u32_t key_length;
    /** Array that stores the WEP key. */
    u8_t key_data[MAX_WEP_KEY_LENGTH];
    } core_wep_key_s;

/**
 * Data structure for storing a cipher key.
 */
typedef struct _core_cipher_key_s
    {
    /** The length of KeyMaterial in bytes. Range: 0-29. */
    u32_t key_length;
    /** Specifies which key to add or remove. Range: 0-3. */
    u8_t key_index;
    /** Array that stores the WEP key. */
    u8_t key_data[MAX_CIPHER_KEY_LENGTH];
    } core_cipher_key_s;    

const core_cipher_key_s CIPHER_KEY_NOT_DEFINED = { 0,0 };

/**
 * Data structure for storing an ARP filters.
 */
typedef struct _core_arp_filter_s
    {
    u32_t ipv4_addr;
    bool_t enable_filter;
    } core_arp_filter_s;

/** 
 * Defines the possible error codes core engine can return.
 */
typedef enum _core_error_e
    {
    core_error_ok = 0,
    core_error_not_found,
    core_error_general,
    core_error_no_memory,
    core_error_illegal_rate,
    core_error_failed,
    core_error_drivers_not_loaded,
    core_error_in_power_save,
    core_error_challenge_failure,
    core_error_not_connected,
    core_error_illegal_argument,
    core_error_not_supported,
    core_error_in_use,
    core_error_timeout,
    core_error_internal_error,
    core_error_request_pending,
    core_error_cancel,
    core_error_eapol_total_failure,
    core_error_eapol_failure,
    core_error_eapol_canceled_by_user,
    core_error_connection_already_active,
    core_error_wlan_disabled,
    core_error_already_exists,
    core_error_eapol_auth_start_timeout,
    core_error_unsupported_config
    } core_error_e;

/**
 * Defines the notification core engine can send to adaptation.
 */
typedef enum _core_notification_e
    {
    /**
     * Indicates a change in connection state.
     *
     * This indication has one to four bytes of payload data. The first byte
     * always contains the current connection state (core_connection_state_e).
     *
     * If the state is core_connection_state_notconnected, the second byte
     * contains the reason connection was closed (core_release_reason_e).
     *
     * If the state is core_connection_state_notconnected and the release reason
     * is core_release_reason_max_roam_attempts_exceeded, there are two more
     * bytes of payload data. The third byte contains the reason roaming was
     * attempted (core_roam_reason_e) and the fourth byte has the reason why
     * roaming failed (core_roam_failed_reason_e).
     */
    core_notification_connection_state_changed,
    /**
     * Indicates a successful roam to another AP has been done.
     *
     * The payload data contains the BSSID of the AP as a core_mac_address_s
     * structure.
     */    
    core_notification_bssid_changed,
    /**
     * Indicates a temporary loss of connection.
     */
    core_notification_bss_lost,
    /**
     * Indicates that connection has been regained after a
     * core_notification_bss_lost indication.
     */
    core_notification_bss_regained,
    /**
     * Indicates the currently allowed maximum tx power.
     *
     * The payload data is four bytes, containing the maximum tx power
     * as u32_t. The value is in dBm.
     */
    core_notification_tx_power_level_changed,
    /**
     * Indicates the RCPI indication threshold has been crossed.
     *
     * The first byte of payload data indicates the direction (core_rcp_class_e).
     * The second byte contains the current RCPI value.
     */
    core_notification_rcp_changed,
    /**
     * Indicates a status change in a virtual traffic stream.
     *
     * The first four bytes of payload data contains the ID of the affected virtual
     * traffic stream. The second byte contains the status of the traffic stream
     * (core_traffic_stream_status_e).
     */
    core_notification_ts_status_changed,
    /**
     * Indicates a change in the parameters of the current AP.
     *
     * The payload data consists of core_ap_information_s structure.
     *
     * While this indication is most likely triggered by a successful roam,
     * it should not be considered as an indication that a roam has happened.
     */
    core_notification_ap_info_changed,
    /**
     * Indicates that an RCPI based roaming attempt has been started.
     *
     * If the roam attempt is successful, core_notification_bssid_changed and
     * core_notification_rcpi_roam_attempt_completed indications will be sent.
     *
     * If no suitable AP to roam to is found, only core_notification_rcpi_roam_attempt_completed
     * indication will be sent. 
     *
     * If the attempt fails due to an association or authentication failure,
     * core_notification_connection_state_changed( core_connection_state_searching )
     * will be sent to indicate that the terminal is no longer connected to
     * the previous AP.
     */
    core_notification_rcpi_roam_attempt_started,
    /**
     * Indicates that the RCPI based roaming attempt has been completed.
     * 
     * See the description of core_notification_rcpi_roam_attempt_started
     * for further information on this indication. 
     */
    core_notification_rcpi_roam_attempt_completed,
    /**
     * Indicates the traffic mode of an access class has changed.
     *
     * The first byte of payload data contains the access class
     * (core_access_class_e). The second byte contains the traffic mode
     * of the access class (core_access_class_traffic_mode_e).
     */
    core_notification_ac_traffic_mode_changed,
    /**
     * Indicates the traffic status of an access class has changed.
     *
     * The first byte of payload data contains the access class
     * (core_access_class_e). The second byte contains the traffic status
     * of the access class (core_access_class_traffic_status_e).
     */    
    core_notification_ac_traffic_status_changed
    } core_notification_e;

/**
 * The possible completion statuses for a connect request. 
 */
typedef enum _core_connect_status_e
    {
    core_connect_undefined,
    core_connect_ok,
    core_connect_unspecified_failure,
    core_connect_network_not_found,    
    core_connect_mode_infra_required_but_ibss_found,
    core_connect_mode_ibss_required_but_infra_found,   
    core_connect_ap_full,
    core_connect_ap_not_whitelisted,
    core_connect_ap_permanently_blacklisted,
    core_connect_ap_temporarily_blacklisted,
    core_connect_ap_outside_defined_region,
    core_connect_ap_signal_too_weak,
    core_connect_ap_has_no_admission_control,
    core_connect_ap_has_no_free_admission_capability,
    core_connect_ap_unsupported_configuration,
    core_connect_iap_open_but_ap_requires_encryption,
    core_connect_iap_wep_but_ap_has_no_privacy,
    core_connect_iap_wep_but_ap_has_wpa_ie,
    core_connect_iap_wpa_but_ap_has_no_privacy,
    core_connect_wep_open_authentication_unsupported,
    core_connect_wep_open_echo_test_failed,
    core_connect_wep_shared_authentication_unsupported,
    core_connect_wep_shared_authentication_failed,
    core_connect_wpa_ie_required_but_ap_has_none,
    core_connect_wpa_psk_required_but_ap_has_no_support,
    core_connect_wpa_eap_required_but_ap_has_no_support,
    core_connect_wpa_ap_has_no_valid_ciphers,
    core_connect_wpa_counter_measures_active,
    core_connect_wpa_authentication_canceled_by_user,
    core_connect_wpa_eap_failure,
    core_connect_wpa_psk_failure,
    core_connect_802_1x_authentication_canceled_by_user,
    core_connect_802_1x_authentication_algorithm_not_supported,
    core_connect_802_1x_failure,
    core_connect_eap_gtc_failure,
    core_connect_eap_tls_failure,
    core_connect_eap_tls_server_certificate_expired,
    core_connect_eap_tls_server_certificate_unknown,
    core_connect_eap_tls_user_certificate_expired,
    core_connect_eap_tls_user_certificate_unknown,
    core_connect_eap_tls_illegal_cipher_suite,
    core_connect_eap_tls_user_rejected,
    core_connect_eap_leap_failure,
    core_connect_eap_sim_failure,
    core_connect_eap_sim_identity_query_failed,
    core_connect_eap_sim_user_has_not_subscribed_to_the_requested_service,
    core_connect_eap_sim_users_calls_are_barred,
    core_connect_eap_ttls_failure,
    core_connect_eap_ttls_server_certificate_expired,
    core_connect_eap_ttls_server_certificate_unknown,
    core_connect_eap_ttls_user_certificate_expired,
    core_connect_eap_ttls_user_certificate_unknown,
    core_connect_eap_ttls_illegal_cipher_suite,
    core_connect_eap_ttls_user_rejected,    
    core_connect_eap_aka_failure,
    core_connect_eap_aka_identity_query_failed,
    core_connect_eap_aka_user_has_not_subscribed_to_the_requested_service,
    core_connect_eap_aka_users_calls_are_barred,
    core_connect_eap_peap_failure,
    core_connect_eap_peap_server_certificate_expired,
    core_connect_eap_peap_server_certificate_unknown,
    core_connect_eap_peap_user_certificate_expired,
    core_connect_eap_peap_user_certificate_unknown,
    core_connect_eap_peap_illegal_cipher_suite,
    core_connect_eap_peap_user_rejected,      
    core_connect_eap_mschapv2_failure,
    core_connect_eap_mschapv2_password_expired,
    core_connect_eap_mschapv2_no_dialin_permission,
    core_connect_eap_mschapv2_account_disabled,
    core_connect_eap_mschapv2_restricted_logon_hours,
    core_connect_eapol_auth_start_timeout,
    core_connect_eap_fast_tunnel_compromise_error,
    core_connect_eap_fast_unexpected_tlv_exhanged,
    core_connect_eap_fast_no_pac_nor_certs_to_authenticate_with_provision_disabled,
    core_connect_eap_fast_no_matching_pac_for_aid,
    core_connect_eap_fast_pac_store_corrupted,
    core_connect_eap_fast_authentication_failed,
    core_connect_iap_wapi_but_ap_has_no_privacy,
    core_connect_wapi_ie_required_but_ap_has_none,
    core_connect_wapi_certificate_required_but_ap_has_no_support,
    core_connect_wapi_psk_required_but_ap_has_no_support,
    core_connect_wapi_ap_has_no_valid_ciphers,
    core_connect_wapi_psk_failure,
    core_connect_wapi_certificate_failure,
    } core_connect_status_e;

/**
 * Possible completion status for a protected setup request. 
 */
typedef enum _core_protected_setup_status_e
    {
    core_protected_setup_status_undefined,
    core_protected_setup_status_ok,
    core_protected_setup_status_session_overlap,
    core_protected_setup_status_walktime_expired,
    core_protected_setup_status_network_auth_failure,
    core_protected_setup_status_network_assoc_failure,
    core_protected_setup_status_OOB_interface_read_error,
    core_protected_setup_status_decryption_CRC_failure,
    core_protected_setup_status_RF_band_2_4_ghz_not_supported,
    core_protected_setup_status_RF_band_5_0_ghz_not_supported,
    core_protected_setup_status_signal_too_weak,
    core_protected_setup_status_no_DHCP_response,
    core_protected_setup_status_failed_DHCP_configure,
    core_protected_setup_status_ip_address_conflict,
    core_protected_setup_status_could_not_connect_to_registrar,
    core_protected_setup_status_multiple_PBC_sessions_detected,
    core_protected_setup_status_rogue_activity_suspected,
    core_protected_setup_status_device_busy,
    core_protected_setup_status_setup_locked,
    core_protected_setup_status_message_timeout,
    core_protected_setup_status_registration_session_timeout,
    core_protected_setup_status_device_password_authentication_failure,
    core_protected_setup_status_pin_code_authentication_not_supported,
    core_protected_setup_status_push_button_authentication_not_supported
    } core_protected_setup_status_e;

typedef enum _core_rcp_class_e
    {
    core_rcp_normal,    
    core_rcp_weak
    } core_rcp_class_e;

/**
 * Definitions for possible WLAN regions.
 */
typedef enum _core_wlan_region_e
    {
    /** Undefined region */
    core_wlan_region_undefined = 0x00,
    /** Channels 1-11, USA */
    core_wlan_region_fcc       = 0x10,    
    /** Channels 1-13, Europe */
    core_wlan_region_etsi      = 0x30,
    } core_wlan_region_e;

typedef struct _core_wpa_preshared_key_s
    {
    /** The length of KeyMaterial in bytes. Range: 0-63 */
    uint_t key_length;
    /** Array of that stores the WPA Pre-Shared Key. */
    u8_t key_data[MAX_WPA_PSK_LENGTH];
    } core_wpa_preshared_key_s;

typedef enum _core_cipher_key_type_e
    {
    core_cipher_key_type_none,
    core_cipher_key_type_tkip,
    core_cipher_key_type_ccmp,
    core_cipher_key_type_wep,
    core_cipher_key_type_wpi,
    } core_cipher_key_type_e;

typedef enum _core_cipher_suite_e
    {
    core_cipher_suite_none   = 0,
    core_cipher_suite_wep40  = 1,
    core_cipher_suite_wep104 = 2,
    core_cipher_suite_tkip   = 4,
    core_cipher_suite_ccmp   = 8,
    core_cipher_suite_wpi    = 16
    } core_cipher_suite_e;

typedef enum _core_key_management_e
    {
    core_key_management_none             = 0,
    core_key_management_eap              = 1,
    core_key_management_preshared        = 2,
    core_key_management_wpx_fast_roam    = 4,
    core_key_management_wapi_certificate = 8,
    core_key_management_wapi_psk         = 16
    } core_key_management_e;

/**
 * Definitions for power save modes.
 */
typedef enum _core_power_save_mode_e
    {
    /**
     * Automatic mode, wake-up mode and interval adjusted automatically.
     */
    core_power_save_mode_automatic,
    /**
     * Continuous access mode (CAM), no power save.
     */
    core_power_save_mode_none,
    /**
     * Power save, wake-up at every Nth beacon.
     */
    core_power_save_mode_beacon,
    /**
     * Power save, wake-up at every Nth DTIM.
     */
    core_power_save_mode_dtim,
    /**
     * Power save, wake-up at every Nth DTIM, where N is defined by
     * DTIM skipping interval.
     */
    core_power_save_mode_dtim_skipping
    } core_power_save_mode_e;

typedef enum _core_slot_time_e
    {
    core_slot_time_short,
    core_slot_time_long
    } core_slot_time_e;

/**
 * Possible 802.11b and 802.11g status codes
 */
typedef enum _core_management_status_e
    {
    core_management_status_success                     = 0,
    core_management_status_unspecified_failure         = 1,
    core_management_status_unsupported_capabilities    = 10,
    core_management_status_reassociation_denied_other  = 11,
    core_management_status_association_denied_other    = 12,
    core_management_status_auth_algo_not_supported     = 13,
    core_management_status_auth_frame_out_of_sequence  = 14,
    core_management_status_auth_challenge_failure      = 15,
    core_management_status_auth_timeout                = 16,
    core_management_status_assoc_denied_full_ap        = 17,
    core_management_status_assoc_unsup_basic_rates     = 18,
    core_management_status_assoc_unsup_short_preamble  = 19,
    core_management_status_assoc_unsup_pbcc            = 20,
    core_management_status_assoc_unsup_channel_agility = 21,
    core_management_status_assoc_unsup_short_slot_time = 25,
    core_management_status_assoc_unsup_dsss_ofdm       = 26,
    core_management_status_assoc_unsup_ht_features     = 27,
    core_management_status_assoc_unsup_pco             = 29
    } core_management_status_e;

/**
 * The frame types the core server can send and receive.
 */
typedef enum _core_frame_type_e
    {
    core_frame_type_ethernet,    
    core_frame_type_dot11,
    core_frame_type_snap,
    core_frame_type_test
    } core_frame_type_e;

/**
 * The possible physical medium types.
 */
typedef enum _core_phy_type_e
    {
    core_phy_type_ofdm,
    core_phy_type_high_rate_dss,
    core_phy_type_erp
    } core_phy_type_e;

/**
 * operation types.
 */
typedef enum _core_operation_type_e
    {
    core_operation_unspecified,
    core_operation_check_rcpi,
    core_operation_configure_multicast_group,
    core_operation_connect,
    core_operation_get_available_iaps,
    core_operation_get_rcpi,
    core_operation_handle_bss_lost,
    core_operation_handle_frame,
    core_operation_handle_delete_ts,
    core_operation_handle_measurement_request,
    core_operation_handle_neighbor_response,
    core_operation_ibss_merge,
    core_operation_null,
    core_operation_release,
    core_operation_roam,
    core_operation_scan,
    core_operation_set_cipher_key,
    core_operation_unload_drivers,
    core_operation_update_device_settings,
    core_operation_update_power_mode,
    core_operation_update_rxtx_parameters,
    core_operation_create_ts,
    core_operation_delete_ts,
    core_operation_get_statistics,
    core_operation_set_uapsd_settings,
    core_operation_set_power_save_settings,
    core_operation_protected_setup,
    core_operation_power_save_test,
    core_operation_set_arp_filter,
    core_operation_directed_roam,
    /** Not a real operation, defined as upper bound. */
    core_operation_max   
    } core_operation_type_e;

/**
 * Defines the possible values of maximum service period length.
 */
typedef enum _core_max_service_period_length_e
    {
    core_max_service_period_length_all,
    core_max_service_period_length_two,
    core_max_service_period_length_four,
    core_max_service_period_length_six
    } core_max_service_period_length_e;

/**
 * Defines the possible QoS Access Classes.
 */
typedef enum _core_access_class_e
	{
	core_access_class_best_effort = 0,
	core_access_class_background = 1,
	core_access_class_video = 2,
	core_access_class_voice = 3
  } core_access_class_e;

/**
 * Defines the possible TX rate values.
 */
typedef enum _core_tx_rate_e
    {
    core_tx_rate_none       = 0x00000000,
    core_tx_rate_1mbit      = 0x00000001,
    core_tx_rate_2mbit      = 0x00000002,
    core_tx_rate_5p5mbit    = 0x00000004,
    core_tx_rate_6mbit      = 0x00000008,
    core_tx_rate_9mbit      = 0x00000010,
    core_tx_rate_11mbit     = 0x00000020,
    core_tx_rate_12mbit     = 0x00000040,
    core_tx_rate_18mbit     = 0x00000080,
    core_tx_rate_22mbit     = 0x00000100,
    core_tx_rate_24mbit     = 0x00000200,
    core_tx_rate_33mbit     = 0x00000400,
    core_tx_rate_36mbit     = 0x00000800,
    core_tx_rate_48mbit     = 0x00001000,
    core_tx_rate_54mbit     = 0x00002000
    } core_tx_rate_e;

const u32_t CORE_TX_RATES_802P11B =
    core_tx_rate_1mbit |
    core_tx_rate_2mbit |
    core_tx_rate_5p5mbit |
    core_tx_rate_11mbit;
    
const u32_t CORE_TX_RATES_802P11G =
    core_tx_rate_6mbit |
    core_tx_rate_9mbit |
    core_tx_rate_12mbit |
    core_tx_rate_18mbit |
    core_tx_rate_22mbit |
    core_tx_rate_24mbit |
    core_tx_rate_33mbit |
    core_tx_rate_36mbit |
    core_tx_rate_48mbit |
    core_tx_rate_54mbit;

const u32_t CORE_TX_RATES_802P11BG =
    CORE_TX_RATES_802P11B |
    CORE_TX_RATES_802P11G;

/**
 * Define the possible TX rate value as units of 500kbit/s.
 */
typedef enum _core_tx_rate_value_e
    {
    core_tx_rate_value_none    = 0,
    core_tx_rate_value_1mbit   = 2,
    core_tx_rate_value_2mbit   = 4,
    core_tx_rate_value_5p5mbit = 11,
    core_tx_rate_value_6mbit   = 12,
    core_tx_rate_value_9mbit   = 18,
    core_tx_rate_value_11mbit  = 22,
    core_tx_rate_value_12mbit  = 24,
    core_tx_rate_value_18mbit  = 36,
    core_tx_rate_value_22mbit  = 44,
    core_tx_rate_value_24mbit  = 48,
    core_tx_rate_value_33mbit  = 66,
    core_tx_rate_value_36mbit  = 72,
    core_tx_rate_value_48mbit  = 96,
    core_tx_rate_value_54mbit  = 108,
    } _core_tx_rate_value_e;

/**
 * The possible statuses of a virtual traffic stream.
 */
typedef enum _core_traffic_stream_status_e
    {
    /**
     * Initialization value, never returned as a status. 
     */
    core_traffic_stream_status_undefined,
    /**
     * The traffic stream is active in the current access point.
     */
    core_traffic_stream_status_active,
    /**
     * The traffic stream is not active in the current access point
     * because the AP doesn't require admission control.
     */
    core_traffic_stream_status_inactive_not_required,
    /**
     * The traffic stream is not active in the current access point
     * because the AP has deleted the traffic stream.
     */    
    core_traffic_stream_status_inactive_deleted_by_ap,
    /**
     * The traffic stream is not active in the current access point
     * because the AP has refused the traffic stream request due to
     * insufficient over-the-air bandwidth.
     */
    core_traffic_stream_status_inactive_no_bandwidth,
    /**
     * The traffic stream is not active in the current access point
     * because the AP has refused the traffic stream request due to
     * invalid traffic stream parameters.
     */
    core_traffic_stream_status_inactive_invalid_parameters,
    /**
     * The traffic stream is not active in the current access point
     * because the AP has refused the traffic stream request due to
     * other reasons.
     */
    core_traffic_stream_status_inactive_other
    } core_traffic_stream_status_e;

/**
 * The possible reasons for a roam.
 */
 typedef enum _core_roam_reason_e
    {
    /** Initial value if no roaming has been done. */
    core_roam_reason_none,
    /** Initial connection attempt. */
    core_roam_reason_initial_connect,
    /** Connection to the previous AP has been lost. */
    core_roam_reason_bss_lost,
    /** The previous AP has deauthenticated us. */
    core_roam_reason_media_disconnect,
    /** Signal strength was too low. */
    core_roam_reason_signal_strength,
    /** Signal loss was predicted. */
    core_roam_reason_signal_loss_prediction,
    /** Failed EAPOL reauthentication. */
    core_roam_reason_failed_reauthentication,
    /** Due to directed roam request. */
    core_roam_reason_directed_roam,
    /** Not a real reason, defined as an upper bound. */
    core_roam_reason_max    
    } core_roam_reason_e;

/**
 * The possible reasons for a failed roaming attempt.
 */
 typedef enum _core_roam_failed_reason_e
    {
    /** Initial value if no roaming has been done. */
    core_roam_failed_reason_none,
    /** Authentication or (re-)association has timed out. */
    core_roam_failed_reason_timeout,
    /** No suitable AP found either due to an empty scan or blacklisting. */
    core_roam_failed_reason_no_suitable_ap,
    /** AP has rejected authentication or (re-)association with a status code. */
    core_roam_failed_reason_ap_status_code,
    /** EAPOL authentication has failed. */
    core_roam_failed_reason_eapol_failure,
    /** Failures not covered by defined reasons above. */
    core_roam_failed_reason_other_failure,
    /** Not a real reason, defined as an upper bound. */
    core_roam_failed_reason_max
    } core_roam_failed_reason_e;

/**
 * The possible reasons why a connection was released.
 */
 typedef enum _core_release_reason_e
    {
    /** Release was requested by an external party. */
    core_release_reason_external_request,
    /** Maximum number of roaming attempts was exceeded. */
    core_release_reason_max_roam_attempts_exceeded,
    /** Release was due to a HW failure indication. */
    core_release_reason_hw_failure,
    /** Release was due to TKIP MIC failure. */
    core_release_reason_tkip_mic_failure,
    /** Reasons not covered by defined reasons above. */
    core_release_reason_other,
    /** Not a real reason, defined as an upper bound. */
    core_release_reason_max
    } core_release_reason_e;

/**
 * The possible reasons an access point was blacklisted.
 */
typedef enum _core_ap_blacklist_reason_e
    {
    /** No blacklisting required. */
    core_ap_blacklist_reason_none                           = 0,
    /** Blacklisting was requested by an external party. */
    core_ap_blacklist_reason_external                       = 1,
    /** Blacklisting was based on failed WPA/802.1x authentication. */
    core_ap_blacklist_reason_eapol_failure                  = 2,
    /** Blacklisting was based on association response status. */
    core_ap_blacklist_reason_association_status             = 4,
    /** Blacklisting due to exceeded max_ap_association_failure_count. */
    core_ap_blacklist_reason_max_association_failure_count  = 8,
    /** Blacklisting due to exceeded max_ap_deauthentication_count. */
    core_ap_blacklist_reason_max_deauthentication_count     = 16,
    /** Not a real reason, defined as an upper bound. */
    core_ap_blacklist_reason_max
    } core_ap_blacklist_reason_e;

/**
 * Defines the operating modes of EAPOL stack.
 */
typedef enum _core_eapol_operating_mode_e
    {
    core_eapol_operating_mode_wfa,
    core_eapol_operating_mode_wapi
    } core_eapol_operating_mode_e;

/**
 * Definitions for possible power modes.
 */
typedef enum _core_power_mode_e
    {
    /** Continuous access mode (CAM), no power save. */
    core_power_mode_cam,
    /** Power save mode (PS). */
    core_power_mode_ps
    } core_power_mode_e;

/**
 * Definitions for power save wake-up modes.
 */
typedef enum _core_power_save_wakeup_mode_e
    {
    /** Wake-up at every Nth beacon. */
    core_power_save_wakeup_mode_beacon,
    /** Wake-up at every Nth DTIM. */
    core_power_save_wakeup_mode_dtim
    } core_power_save_wakeup_mode_e;

/**
 * Definitions for run-time feature configuration flags. 
 */
typedef enum _core_feature_e
    {
    /** No configurable features enabled. */
    core_feature_none                   = 0,
    /** Power save test enabled. */    
    core_feature_power_save_test        = 1,
    /** 802.11k support enabled. */
    core_feature_802dot11k              = 2,
    /** 802.11n support enabled. */
    core_feature_802dot11n              = 4    
    } core_feature_e;

/**
 * Definitions for possible traffic modes for an access class.
 */
typedef enum _core_access_class_traffic_mode_e
    {
    /** Automatic traffic stream creation is allowed. */
    core_access_class_traffic_mode_automatic,
    /** Automatic traffic stream creation is NOT allowed. */
    core_access_class_traffic_mode_manual
    } core_access_class_traffic_mode_e;

/**
 * Definitions for possible traffic statuses for an access class.
 */
typedef enum _core_access_class_traffic_status_e
    {
    /**
     * Traffic for this access class has been admitted.
     */
    core_access_class_traffic_status_admitted,
    /** 
     * Traffic for this access class has NOT been admitted,
     * traffic needs to be downgraded to a lower access class.
     */
    core_access_class_traffic_status_not_admitted
    } core_access_class_traffic_status_e;

/**
 * Enumeration for traffic stream direction.
 */
typedef enum _core_traffic_stream_direction_e
    {
    /** Admission is requested for uplink traffic. */ 
    core_traffic_stream_direction_uplink = 0,
    /** Admission is requested for downlink traffic. */ 
    core_traffic_stream_direction_downlink = 1,
    /** Admission is requested for both uplink and downlink traffic. */ 
    core_traffic_stream_direction_bidirectional = 3    
    } core_traffic_stream_direction_e;

/** 
 * Structure for storing WLAN packet statistics.
 */
struct core_packet_statistics_s
    {
    /**
     * Number of successfully received data frames.
     */
    u32_t rx_frames;
    /**
     * Number of sent data frames.
     */    
    u32_t tx_frames;
    /**
     * Number of successfully received multicast data frames.
     */ 
    u32_t rx_multicast_frames;
    /**
     * Number of sent multicast data frames.
     */ 
    u32_t tx_multicast_frames;
    /**
     * Number of received frames with FCS errors.
     */
    u32_t fcs_errors;
    /**
     * Total number of retransmissions done.
     */
    u32_t tx_retries;
    /**
     * Number of data frames that could not be to delivered.     
     */
    u32_t tx_errors;
    };

/**
 * Structure for storing WLAN packet statistics per access category.
 */
 struct core_packet_statistics_for_access_category_s
    {
    /**
     * Number of successfully received data frames.
     */
    u32_t rx_frames;
    /**
     * Number of sent data frames.
     */
    u32_t tx_frames;
    /**
     * Number of successfully received multicast data frames.
     */
    u32_t rx_multicast_frames;
    /**
     * Number of sent multicast data frames.
     */
    u32_t tx_multicast_frames;
    /**
     * Total number of retransmissions done.
     */
    u32_t tx_retries;
    /**
     * Number of data frames that could not be to delivered.     
     */
    u32_t tx_errors;
    /**
     * Average data frame Transmit / Media Delay in microseconds.
     */
    u32_t tx_media_delay;
    /**
     * Average data frame Total Transmit Delay in microseconds.
     */
    u32_t tx_total_delay;
    /**
     * Number of frame whose whose total transmit delay was <= 10ms.
     */
    u32_t tx_total_delay_bin0;
    /**
     * Number of frame whose whose total transmit delay was ]10,20]ms.
     */
    u32_t tx_total_delay_bin1;
    /**
     * Number of frame whose whose total transmit delay was ]20,40]ms.
     */
    u32_t tx_total_delay_bin2;
    /**
     * Number of frame whose whose total transmit delay was > 40ms.
     */
    u32_t tx_total_delay_bin3;
    };

/**
 * Structure for storing WLAN packet statistics for all the access categories.
 */
 struct core_packet_statistics_by_access_category_s
    {
    /**
     * Statistics per access category.
     */
    core_packet_statistics_for_access_category_s ac[MAX_QOS_ACCESS_CLASS];
    /**
     * Number of received frames with FCS errors.
     */
    u32_t fcs_errors;
    };

/**
 * Structure for storing U-APSD settings for the access categories.
 */
struct core_uapsd_settings_s
    {
    /**
     * Defines the maximum number of frames to send during a service period.
     */
    core_max_service_period_length_e max_service_period;
    /**
     * Whether U-APSD is trigger and delivery-enabled for Voice.
     */
    bool_t uapsd_enabled_for_voice;
    /**
     * Whether U-APSD is trigger and delivery-enabled for Video.
     */    
    bool_t uapsd_enabled_for_video;
    /**
     * Whether U-APSD is trigger and delivery-enabled for BestEffort.
     */    
    bool_t uapsd_enabled_for_best_effort;
    /**
     * Whether U-APSD is trigger and delivery-enabled for Background.
     */    
    bool_t uapsd_enabled_for_background;
    };

/**
 * Structure for storing parameters related to AP lost indications.
 */
struct core_bss_lost_parameters_s
    {
    /**
     * The number of consecutive beacons that can be lost before
     * core_am_indication_wlan_beacon_lost is indicated.
     */    
    u32_t beacon_lost_count;
    /**
     * The number of consecutive transmissions that can fail
     * before core_am_indication_wlan_tx_fail indicated.
     */
    u8_t failed_tx_packet_count;
    };

/**
 * Structure for storing parameters related to tx rate adaptation algorithm.
 */
struct core_tx_rate_adaptation_parameters_s
    {
    /**
     * Minimum and initial rate increase checkpoint in units of frames.
     */    
    u8_t min_stepup_checkpoint;
    /**
     * Maximum rate increase checkpoint in units of frames.
     */
    u8_t max_stepup_checkpoint;
    /**
     * Rate increase checkpoint is multiplied with this value if sending of a probe frame fails.
     */
    u8_t stepup_checkpoint_factor;
    /**
     * After this many frames the need to decrease the rate is checked.
     */
    u8_t stepdown_checkpoint;
    /**
     * Minimum and initial rate increase threshold percentage.
     */
    u8_t min_stepup_threshold;
    /** 
     * Maximum rate increase threshold percentage value.
     */
    u8_t max_stepup_threshold;
    /**
     * Rate increase threshold is incremented by this value if sending of a probe frame fails.
     */
    u8_t stepup_threshold_increment;
    /**
     * Rate decrease threshold percentage.
     */
    u8_t stepdown_threshold;
    /**
     * If false_t, the rate adaptation algorithm handles the first frame transmitted after a
     * rate increase in a special way.
     */
    bool_t disable_probe_handling;
    };

/**
 * Structure for storing parameters related to power mode management algorithm.
 */
struct core_power_mode_mgmt_parameters_s
    {
    /**
     * Time interval in microseconds after which transition from Active mode 
     * to Light PS mode is considered.
     */
    u32_t active_to_light_timeout;
    /**
     * Frame count threshold used when considering transition from Active
     * to Light PS mode.
     */ 
    u16_t active_to_light_threshold;
    /**
     * Time interval in microseconds after which the frame counter used when 
     * considering transition from Light PS to Active mode is reset.
     */
    u32_t light_to_active_timeout;
    /**
     * Frame count threshold used when considering transition from Light PS
     * to Active mode.
     */
    u16_t light_to_active_threshold;
    /**
     * Time interval in microseconds after which transition from Light PS mode 
     * to Deep PS mode is considered.
     */
    u32_t light_to_deep_timeout;    
    /**
     * Frame count threshold used when considering transition from Light PS
     * to Deep PS mode.
     */
    u16_t light_to_deep_threshold;
    /**
     * Received frame payload length (in bytes) threshold in U-APSD network for
     * Best Effort Access Category.
     */
    u16_t uapsd_rx_frame_length_threshold;    
    };

struct core_device_settings_s
    {
	u32_t beacon;						        ///< IBSS beacon interval in TUs.
  	u32_t long_retry;                           ///< How many times packets bigger than 'rts' will be resent.
    u32_t short_retry;                          ///< How many times packets smaller than 'rts' will be resent.
	u32_t rts;						            ///< Minimum size of a packet for CTS/RTS handshake.	
    u32_t tx_power_level;                       ///< Maximum allowed transmission power level in dBm.
	u32_t scan_rate;	                        ///< Data rate used to send probe requests.
	                                            ///< The rate is defined in units of 500kbit/s.
	u32_t rcpi_trigger;                         ///< Roaming is attemped when RCPI value is lower than this.
	u32_t active_scan_min_ch_time;              ///< Min time (TUs) to listen a channel in active direct scanning / during an active connection.
	u32_t active_scan_max_ch_time;              ///< Max time (TUs) to listen a channel in active direct scanning / during an active connection.
    u32_t active_broadcast_scan_min_ch_time;    ///< Min time (TUs) to listen a channel in active broadcast scanning when not connected.
    u32_t active_broadcast_scan_max_ch_time;    ///< Max time (TUs) to listen a channel in active broadcast scanning when not connected.
	u32_t passive_scan_min_ch_time;             ///< Min time (TUs) to listen a channel in passive scanning.
	u32_t passive_scan_max_ch_time;             ///< Max time (TUs) to listen a channel in passive scanning.
    u32_t max_tx_msdu_life_time;                ///< Max time (TUs) to (re-)send one (fragmented) packet.
    u32_t unload_driver_timer;                  ///< Number of seconds to wait before unloading WLAN drivers
                                                ///< if no WLAN connection is active.
    u32_t roam_timer;                           ///< Defines how often to search for a better AP in poor
                                                ///< signal conditions. The value is in microseconds.
    u32_t rcpi_difference;                      ///< Difference in dBm required between an AP and the current
                                                ///< AP before roaming is attemped.
    u32_t max_tries_to_find_nw;                 ///< Defines how many are made to find a suitable AP before
                                                ///< giving up during initial connect.
    u32_t delay_between_find_nw;                ///< Defines the amount of microseconds to wait between
                                                ///< initial connection attempts.
	bool_t power_save_enabled;                  ///< true_t if WLAN power save is allowed, false_t if not.
    bool_t allow_radio_measurements;            ///< true_t if radio measurements are allowed, false_t if not.
    u32_t max_ap_association_failure_count;     ///< Number of times association may fail before the AP
                                                ///< is blacklisted for the duration of the connection.
    u32_t max_ap_authentication_failure_count;  ///< Number of times authentication may fail before the AP
                                                ///< is blacklisted for the duration of the connection.
    u32_t max_ap_deauthentication_count;        ///< The maximum number of times AP can deauthenticate the
                                                ///< terminal within ap_deauthentication_timeout after a
                                                ///< successful roam before it's blacklisted for the
                                                ///< duration of the connection.
    u32_t ap_deauthentication_timeout;          ///< Deauthentication within this many microseconds after
                                                ///< a successful roam is calculated towards AP's 
                                                ///< deauthentication count.
    u32_t long_beacon_find_count;               ///< Number of attempts to find an AP with long beacon
                                                ///< interval.
    u32_t qos_null_frame_interval;              ///< Defines how often (in microseconds) a QoS NULL data frame
                                                ///< is sent during a VoIP call if no other frames to send.
    u32_t qos_null_frame_exit_timeout;          ///< Defines how soon after the last Voice AC packet QoS NULL
                                                ///< data frame sending is stopped (in microseconds).
    u32_t qos_null_frame_entry_timeout;         ///< Defines the time period in microseconds which is used
                                                ///< to evaluate whether QoS NULL data frame sending should
                                                ///< be started.
    u32_t qos_null_frame_entry_tx_count;        ///< Defines how many Voice AC packets must be sent during
                                                ///< the time period defined in qos_null_frame_entry_timeout
                                                ///< before QoS NULL data frame sending is started.
    u32_t keep_alive_interval;                  ///< How often NULL data frames are sent when nothing is being
                                                ///< transmitted. The value is in microseconds.
    u32_t scan_stop_rcpi_threshold;             ///< If AP RCPI value is better than this, roaming scan can be
                                                ///< stopped.
    u32_t iap_availability_rcpi_threshold;      ///< If AP RCPI value is less than this, IAP will not marked
                                                ///< as active.
    u32_t sp_rcpi_target;                       ///< Defines the signal predictor algorithm "target" RCPI
                                                ///< value for roam indication.
    u32_t sp_time_target;                       ///< Defines the signal predictor algorithm "target" time for
                                                //// roam indication (in microseconds).
    u32_t sp_min_indication_interval;           ///< Defines the minimum time interval for consecutive roam
                                                ///< indications from the signal predictor algorithm
                                                ///< (in microseconds).
    u32_t scan_list_expiration_time;            ///< This value defines how long beacons/probe responses are
                                                ///< stored in the internal scan list (in microseconds).
    u32_t rcpi_roam_min_interval;               ///< Minimum delay between RCPI checks (in microseconds).
    u32_t rcpi_roam_max_interval;               ///< Maximum delay between RCPI checks (in microseconds).
    u32_t rcpi_roam_attempts_per_interval;      ///< Number of times to try with the current interval value.
    u32_t rcpi_roam_next_interval_factor;       ///< The current interval value is multiplied by this factor
                                                ///< to get the next interval value.
    u32_t rcpi_roam_next_interval_addition;     ///< This value is added to the multiplied interval value
                                                ///< (in microseconds).
    u32_t bss_lost_roam_min_interval;           ///< Minimum delay between roam attempts due to BSS lost.
    u32_t bss_lost_roam_max_interval;           ///< Maximum delay between roam attempts due to BSS lost.
    u32_t bss_lost_roam_attempts_per_interval;  ///< Number of times to try with the current interval value.
    u32_t bss_lost_roam_next_interval_factor;   ///< The current interval value is multiplied by this factor
                                                ///< to get the next interval value.
    u32_t bss_lost_roam_next_interval_addition; ///< This value is added to the multiplied interval value
                                                ///< (in microseconds).
    u32_t bss_lost_roam_max_tries_to_find_nw;   ///< Defines how many are made to find a suitable AP before
                                                ///< giving up during BSS lost.
    u32_t max_dtim_skip_interval;               ///< Maximum amount of microseconds to sleep when waking up
                                                ///< on DTIMs. Zero value means waking up on every DTIM.
    u32_t ps_active_to_light_timeout;           ///< Time interval in microseconds after which transition
                                                ///< from Active mode to Light PS mode is considered.               
    u16_t ps_active_to_light_threshold;         ///< Frame count threshold used when considering transition
                                                ///< from Active to Light PS mode.
    u32_t ps_light_to_active_timeout;           ///< Time interval in microseconds after which the frame counter
                                                ///< used when considering transition from Light PS to Active mode is reset.
    u16_t ps_light_to_active_threshold;         ///< Frame count threshold used when considering transition
                                                ///< from Light PS to Active mode.
    u32_t ps_light_to_deep_timeout;             ///< Time interval in microseconds after which transition
                                                ///< from Light PS mode to Deep PS mode is considered.    
    u16_t ps_light_to_deep_threshold;           ///< Frame count threshold used when considering transition
                                                ///< from Light PS to Deep PS mode.
    u16_t ps_uapsd_rx_frame_length;             ///< Received frame payload length (in bytes) threshold in
                                                ///< U-APSD network for Best Effort Access Category.    
    u32_t rrm_min_measurement_interval;         ///< This value defines how often 802.11k Measurement Request are accepted.
                                                ///< The value is in microseconds. If zero, request coming with any
                                                ///< interval are accepted.
    };

struct core_iap_data_s
    {
    /** ID of the IAP in CommsDB. */
    u32_t id;
    /** Is the connection mode IBSS or Infrastructure. */
    core_operating_mode_e op_mode;
    /** Name of the network. */
    core_ssid_s ssid;
    /** Is SSID hidden */
    bool_t is_hidden;
   	/** WEP key 1. */
    core_wep_key_s wep_key1;
   	/** WEP key 2. */
    core_wep_key_s wep_key2;
   	/** WEP key 3. */
    core_wep_key_s wep_key3;
   	/** WEP key 4. */
    core_wep_key_s wep_key4;
    /** Default WEP key id. */
    u32_t default_wep_key;
    /** Security mode. */
    core_security_mode_e security_mode;
    /** Authentication mode (open, shared). */
    core_authentication_mode_e authentication_mode;
    /** Is preshared key in use (ETrue) or not (EFalse). */
    bool_t wpa_preshared_key_in_use;
    /** Is WPA settings overriden or not. */
    bool_t is_wpa_overriden;
    /** Overridden WPA PSK */
    core_wpa_preshared_key_s wpa_preshared_key;
    /** Used channel in IBSS (ad-hoc) mode */
    u32_t used_adhoc_channel;
    /** Is IP address configured dynamically. */
    bool_t is_dynamic_ip_addr;
    /** Is roaming between APs allowed. */
    bool_t is_roaming_allowed;
    /** ID of the IAP. */
    u32_t iap_id;
    };

/**
 * Structure for storing IAP availability data.
 */
struct core_iap_availability_data_s
    {
    /** ID of the IAP. */
    u32_t id;
    /** Signal strength of the strongest AP in the network. */
    u8_t rcpi;
    };

/**
 * Struct containing SSIDs related to secondary SSID feature.
 */
struct core_ssid_entry_s
	{
    /** ID of the IAP this entry refers to. */
    u32_t id;
	/** Defines the SSID to look for in the scan results. */
	core_ssid_s ssid;
	/** Defines the SSID used for connecting. */
	core_ssid_s used_ssid;
	};

struct core_scan_channels_s
    {
    u8_t band;                      ///< 1 = 2.4GHz, 2 = 4.9GHz, 4 = 5GHz
    u8_t channels2dot4ghz[2];
    u8_t channels4dot9ghz[3];
    u8_t channels5ghz[26];
	};

const core_scan_channels_s SCAN_CHANNELS_2DOT4GHZ_US =
    { SCAN_BAND_2DOT4GHZ, { 0xFF, 0x07 }, { 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} };

const core_scan_channels_s SCAN_CHANNELS_2DOT4GHZ_ETSI =
    { SCAN_BAND_2DOT4GHZ, { 0xFF, 0x1F }, { 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} };

/**
 * Structure for storing 802.11d country strings.
 */
struct core_country_string_s
    {
    u8_t country[MAX_COUNTRY_STRING_LENGTH];
    };

/** Definition for an empty country string. */
const core_country_string_s COUNTRY_STRING_NONE = { 0, 0, 0 };

/** AP country information for Canada, North America, Taiwan, Brazil, Argentina, Mexico and Colombia. */
const core_country_string_s country_info_table[] = { {'C','A','0'}, {'U','S','0'}, {'V','I','0'}, {'T','W','0'}, {'B','R','0'}, {'A','R','0'}, {'M','X','0'}, {'C','O','0'} };
const u8_t country_info_table_length = 8;

/**
 * Structure for storing a single TX rate policy.
 */
struct core_tx_rate_policy_s
    {
    /** Number of attempts to transmit at 54 Mbits/s. */
    u8_t tx_policy_54;
    /** Number of attempts to transmit at 48 Mbits/s. */
    u8_t tx_policy_48;
    /** Number of attempts to transmit at 36 Mbits/s. */
    u8_t tx_policy_36;
    /** Number of attempts to transmit at 33 Mbits/s. */
    u8_t tx_policy_33;
    /** Number of attempts to transmit at 24 Mbits/s. */
    u8_t tx_policy_24;
    /** Number of attempts to transmit at 22 Mbits/s. */
    u8_t tx_policy_22;
    /** Number of attempts to transmit at 18 Mbits/s. */
    u8_t tx_policy_18;
    /** Number of attempts to transmit at 12 Mbits/s. */
    u8_t tx_policy_12;
    /** Number of attempts to transmit at 11 Mbits/s. */
    u8_t tx_policy_11;
    /** Number of attempts to transmit at 9 Mbits/s. */
    u8_t tx_policy_9;
    /** Number of attempts to transmit at 6 Mbits/s. */
    u8_t tx_policy_6;
    /** Number of attempts to transmit at 5.5 Mbits/s. */
    u8_t tx_policy_5p5;
    /** Number of attempts to transmit at 2 Mbits/s. */
    u8_t tx_policy_2;
    /** Number of attempts to transmit at 1 Mbits/s. */
    u8_t tx_policy_1;
    /** Number of retransmissions to packets that are smaller than RTS. */
    u8_t short_retry_limit;
    /** Number of retransmissions to packets that are bigger than RTS. */    
    u8_t long_retry_limit;
    /** Flags defined in HAL API that apply to this policy. */
    u32_t flags;
    /** Initial TX rate used for transmitting. */
    core_tx_rate_e initial_tx_rate;
    /** TX rate mask for auto rate policy. */
    u32_t tx_auto_rate_policy;
    /** Bitmap of enabled 802.11n modulation and coding schemes. */    
    u8_t mcs_set[TX_RATE_POLICY_MCS_SIZE];
    };

/**
 * Structure for storing a group of TX rate policies.
 */
struct core_tx_rate_policies_s
    {
    /** Number of TX rate policies in the array. */
    u8_t policy_count;
    /** Array of TX rate policies. */
    core_tx_rate_policy_s policy[MAX_TX_RATE_POLICY];
    };

/**
 * Structure for storing mappings between QoS Access Classes and TX rate policies.
 */
struct core_tx_rate_policy_mappings_s
    {
    u8_t policy_for_best_effort;
    u8_t policy_for_background;
    u8_t policy_for_video;
    u8_t policy_for_voice;
    };

/** 
 * Structure for storing traffic stream parameters.
 */
struct core_traffic_stream_params_s
    {
    /**
     * Whether the traffic pattern is periodic or aperiodic.
     */
    bool_t is_periodic_traffic;
    /**
     * Traffic stream direction.
     */
    core_traffic_stream_direction_e direction;
    /**
     * The current value of Nominal MSDU Size.
     */
    u16_t nominal_msdu_size;
    /**
     * The current value of Maximum MSDU Size.
     */
    u16_t maximum_msdu_size;
    /**
     * The current value of Minimum Service Interval.
     */        
    u32_t minimum_service_interval;
    /**
     * The current value of Maximum Service Interval.
     */   
    u32_t maximum_service_interval;
    /**
     * The current value of Inactivity Interval.
     */    
    u32_t inactivity_interval;
    /**
     * The current value of Suspension Interval.
     */    
    u32_t suspension_interval;
    /**
     * The current value of Service Start Time.
     */    
    u32_t service_start_time;
    /**
     * The current value of Minimum Data Rate.
     */
    u32_t minimum_data_rate;
    /**
     * The current value of Mean Data Rate.
     */    
    u32_t mean_data_rate;
    /**
     * The current value of Peak Data Rate.
     */    
    u32_t peak_data_rate;   
    /**
     * The current value of Maximum Burst Size.
     */    
    u32_t maximum_burst_size;
    /**
     * The current value of Delay Bound
     */    
    u32_t delay_bound;
    /**
     * The current value of Minimum PHY Rate.
     */    
    u32_t minimum_phy_rate;
    /**
     * The current value of Nominal PHY Rate in units 
     */
    core_tx_rate_e nominal_phy_rate;
    /**
     * The current value of Bandwidth Allowance.
     */
    u16_t surplus_bandwidth_allowance;
    /**
     * Whether re-creation of traffic streams is allowed.
     */
    bool_t is_retry_allowed;
    /**
     * The current value of Medium Time.
     */
    u16_t medium_time;
    /**
     * The tx rates to be used with this AC, 0 if defaults are used.
     */
    u32_t override_rates;
    /**
     * The Maximum Transmit MSDU Lifetime, 0 if default is used.
     */
    u32_t override_max_tx_msdu_lifetime;
    };

/** 
 * Structure for storing information about channels that have
 * APs with long beacon intervals.
 */
struct core_long_beacon_interval_channels_s
    {
    u8_t channel_scan_count[SCAN_BAND_2DOT4GHZ_MAX_CHANNEL_EURO];
    };

/**
 * Structure for storing power save settings for the access categories.
 */
struct core_power_save_settings_s
    {
    /** Whether the terminal stays in U-APSD power save when using Voice. */
    bool_t stay_in_uapsd_power_save_for_voice;
    /** Whether the terminal stays in U-APSD power save when using Video. */
    bool_t stay_in_uapsd_power_save_for_video;
    /** Whether the terminal stays in U-APSD power save when using BestEffort. */
    bool_t stay_in_uapsd_power_save_for_best_effort;
    /** Whether the terminal stays in U-APSD power save when using Background. */
    bool_t stay_in_uapsd_power_save_for_background;
    /** Whether the terminal stays in legacy power save when using Voice. */
    bool_t stay_in_legacy_power_save_for_voice;
    /** Whether the terminal stays in legacy power save when using Video. */
    bool_t stay_in_legacy_power_save_for_video;
    /** Whether the terminal stays in legacy power save when using BestEffort. */
    bool_t stay_in_legacy_power_save_for_best_effort;
    /** Whether the terminal stays in legacy power save when using Background. */
    bool_t stay_in_legacy_power_save_for_background;
    };

/**
 * Structure for storing RCPI roam interval parameters.
 */
struct core_rcpi_roam_interval_s
    {
    /** Number of times the current interval has been tried. */
    u32_t count;
    /** The current interval value. */
    u32_t interval;
    };

/**
 * Structure for storing information about an access point.
 */
struct core_ap_information_s
    {
    /** The SSID of the AP. */
    core_ssid_s ssid;
    /** The BSSID of the AP. */
    core_mac_address_s bssid;
    /** Capabilities of the AP. */
    u16_t capabilities;
    /** The channel AP is on. */
    u8_t channel;
    /** The received channel power indicator (RCPI). */
    u8_t rcpi;
    /** Basic rates of the AP. */
    u32_t basic_rates;
    /** Supported rates of the AP. */
    u32_t supported_rates;
    /** The security mode of the AP. */
    core_connection_security_mode_e security_mode;
    /** Whether the AP supports WPX. */
    bool_t is_wpx;
    /** Whether Admission Control must be used with Voice. */
    bool_t is_ac_required_for_voice;
    /** Whether Admission Control must be used with Video. */
    bool_t is_ac_required_for_video;
    /** Whether Admission Control must be used with BestEffort. */
    bool_t is_ac_required_for_best_effort;
    /** Whether Admission Control must be used with Background. */
    bool_t is_ac_required_for_background;
    };

/**
 * Structure for storing roaming metrics information.
 */
struct core_roam_metrics_s
    {
    /** Connection attempt total counter. */
    u32_t connection_attempt_total_count;
    /** Unsuccesfull connection attempt counter. */
    u32_t unsuccesfull_connection_attempt_count;
    /** Roaming counter. */
    u32_t roaming_counter;
    /** Coverage loss counter. */
    u32_t coverage_loss_count;

    /** The total duration (ms) of the last roaming (= data path broken time + scanning time). */
    u32_t last_roam_total_duration;
    /** The duration (ms) how long the data path was broken during the last roaming. */
    u32_t last_roam_data_path_broken_duration;
    /** The cause for the last roaming. */
    core_roam_reason_e last_roam_reason;
    };

/**
 * Structure for storing parameters related to power save mode.
 */
struct core_power_save_mode_s
    {
    /** The power save mode to use. */
    core_power_save_mode_e mode;
    /**
     * The wake-up interval.
     * @note This parameter is only applicable if the mode is
     *       core_power_save_mode_beacon or core_power_save_mode_dtim,
     *       otherwise it should set to zero.
     */
    u8_t wakeup_interval;
    };

const core_power_save_mode_s CORE_POWER_SAVE_MODE_EVERY_BEACON =
    { core_power_save_mode_beacon, 1 };
const core_power_save_mode_s CORE_POWER_SAVE_MODE_EVERY_DTIM =
    { core_power_save_mode_dtim, 1 };
const core_power_save_mode_s CORE_POWER_SAVE_MODE_NONE =
    { core_power_save_mode_none, 0 };
const core_power_save_mode_s CORE_POWER_SAVE_MODE_AUTOMATIC =
    { core_power_save_mode_automatic, 0 };
const core_power_save_mode_s CORE_POWER_SAVE_MODE_DTIM_SKIPPING =
    { core_power_save_mode_dtim_skipping, 0 };

/**
 * Structure for storing parameters related to power mode.
 */
struct core_power_mode_s
    {
    /** The power mode to use. */
    core_power_mode_e mode;
    /** Disable dynamic power save management. */
    bool_t disable_dynamic_ps;
    /** Defines the wake-up mode for Light PS mode. */
    core_power_save_wakeup_mode_e wakeup_mode_light;
    /** Defines the wake-up interval (N) for Light PS mode. */
    u8_t wakeup_interval_light;
    /** Defines the wake-up mode for Deep PS mode. */
    core_power_save_wakeup_mode_e wakeup_mode_deep;
    /** Defines the wake-up interval (N) for Deep PS mode. */
    u8_t wakeup_interval_deep;
    };

const core_power_mode_s CORE_POWER_MODE_CAM =
    { core_power_mode_cam, true_t, core_power_save_wakeup_mode_beacon, 1, core_power_save_wakeup_mode_beacon, 1 };
const core_power_mode_s CORE_POWER_MODE_PS =
    { core_power_mode_ps, false_t, core_power_save_wakeup_mode_beacon, 1, core_power_save_wakeup_mode_dtim, 1 };
const core_power_mode_s CORE_POWER_MODE_PS_BEACON =
    { core_power_mode_ps, true_t, core_power_save_wakeup_mode_beacon, 1, core_power_save_wakeup_mode_beacon, 1 };
const core_power_mode_s CORE_POWER_MODE_PS_DTIM =
    { core_power_mode_ps, true_t, core_power_save_wakeup_mode_dtim, 1, core_power_save_wakeup_mode_dtim, 1 };

/**
 * Structure for storing an access point blacklist entry.
 */
struct core_ap_blacklist_entry_s
    {
    /** BSSID of the AP. */
    core_mac_address_s bssid;
    /** Reason the AP was blacklisted. */
    core_ap_blacklist_reason_e reason;
    };

/**
 * Structure for storing block ACK usage information.
 */
struct core_block_ack_usage_s
    {
    /** Bitmap of TIDs where TX block ACK is allowed. */
    u8_t tx_usage;
    /** Bitmap of TIDs where RX block ACK is allowed. */
    u8_t rx_usage;
    };

/**
 * Structure for storing a SNAP header.
 */
struct core_snap_header_s
    {
    /** Destination Service Access Point. */
    u8_t dsap;
    /** Source Service Access Point. */
    u8_t ssap;
    /** Control Field. */
    u8_t control;
    /** Organizationally Unique Identifier (OUI). */
    u8_t oui[OUI_LENGTH];
    };

/**
 * Structure for access class traffic statuses and traffic modes.
 */
struct core_ac_traffic_information_s
    {
    /** Current traffic status for Voice. */
    core_access_class_traffic_status_e status_for_voice;
    /** Current traffic status for Video. */
    core_access_class_traffic_status_e status_for_video;
    /** Current traffic status for BestEffort. */
    core_access_class_traffic_status_e status_for_best_effort;
    /** Current traffic status for Background. */
    core_access_class_traffic_status_e status_for_background;
    /** Current traffic mode for Voice. */
    core_access_class_traffic_mode_e mode_for_voice;
    /** Current traffic mode for Video. */
    core_access_class_traffic_mode_e mode_for_video;
    /** Current traffic mode for BestEffort. */
    core_access_class_traffic_mode_e mode_for_best_effort;
    /** Current traffic mode for Background. */
    core_access_class_traffic_mode_e mode_for_background;
    };

/**
 * Enums for WLAN Engine - EAPOL interface datatypes.
 */

typedef enum _wlan_eapol_if_eap_protocol_layer_e
    {
    wlan_eapol_if_eap_protocol_layer_none,
    wlan_eapol_if_eap_protocol_layer_general,
    wlan_eapol_if_eap_protocol_layer_internal_type,
    wlan_eapol_if_eap_protocol_layer_am_eap_type,
    wlan_eapol_if_eap_protocol_layer_radius,
    wlan_eapol_if_eap_protocol_layer_eap_type,
    wlan_eapol_if_eap_protocol_layer_eap,
    wlan_eapol_if_eap_protocol_layer_eapol,
    wlan_eapol_if_eap_protocol_layer_eapol_key,
    wlan_eapol_if_eap_protocol_layer_ethernet,
    wlan_eapol_if_eap_protocol_layer_wlan_authentication,
    wlan_eapol_if_eap_protocol_layer_authentication_server,
    wlan_eapol_if_eap_protocol_layer_wapi
    } wlan_eapol_if_eap_protocol_layer_e;

typedef enum _wlan_eapol_if_eapol_key_authentication_mode_e
    {
    wlan_eapol_if_eapol_key_authentication_mode_none,
    wlan_eapol_if_eapol_key_authentication_mode_open,
    wlan_eapol_if_eapol_key_authentication_mode_shared,
    wlan_eapol_if_eapol_key_authentication_mode_wpx,
    } wlan_eapol_if_eapol_key_authentication_mode_e;

typedef enum _wlan_eapol_if_eapol_key_authentication_type_e
    {
    wlan_eapol_if_eapol_key_authentication_type_none,
    wlan_eapol_if_eapol_key_authentication_type_rsna_eap,
    wlan_eapol_if_eapol_key_authentication_type_rsna_psk,
    wlan_eapol_if_eapol_key_authentication_type_wpa_eap,
    wlan_eapol_if_eapol_key_authentication_type_wpa_psk,
    wlan_eapol_if_eapol_key_authentication_type_802_1x,
    wlan_eapol_if_eapol_key_authentication_type_wpx_fast_roam,
    wlan_eapol_if_eapol_key_authentication_type_wfa_sc,
    wlan_eapol_if_eapol_key_authentication_type_wapi_psk,
    wlan_eapol_if_eapol_key_authentication_type_wapi,
    wlan_eapol_if_eapol_key_authentication_type_802_1x_unencrypted
    } wlan_eapol_if_eapol_key_authentication_type_e;

typedef enum _wlan_eapol_if_eapol_key_type_e
    {
    wlan_eapol_if_eapol_key_type_broadcast,
    wlan_eapol_if_eapol_key_type_unicast,
    wlan_eapol_if_eapol_key_type_wpx_fast_roam_krk,
    wlan_eapol_if_eapol_key_type_wpx_fast_roam_btk,
    wlan_eapol_if_eapol_key_type_pmkid,
    } wlan_eapol_if_eapol_key_type_e;

typedef enum _wlan_eapol_if_eapol_tkip_mic_failure_type_e
    {
    wlan_eapol_if_eapol_tkip_mic_failure_type_group_key,
    wlan_eapol_if_eapol_tkip_mic_failure_type_pairwise_key,
    } wlan_eapol_if_eapol_tkip_mic_failure_type_e;

typedef enum _wlan_eapol_if_eapol_wlan_authentication_state_e
    {
    wlan_eapol_if_eapol_wlan_authentication_state_none,
    wlan_eapol_if_eapol_wlan_authentication_state_association_ok,
    wlan_eapol_if_eapol_wlan_authentication_state_this_ap_failed,
    wlan_eapol_if_eapol_wlan_authentication_state_failed_completely,
    wlan_eapol_if_eapol_wlan_authentication_state_802_11_auth_algorithm_not_supported,
    wlan_eapol_if_eapol_wlan_authentication_state_obsolete, // <- Do not use this value.
    wlan_eapol_if_eapol_wlan_authentication_state_eap_authentication_running,
    wlan_eapol_if_eapol_wlan_authentication_state_no_response,
    wlan_eapol_if_eapol_wlan_authentication_state_4_way_handshake_running,
    wlan_eapol_if_eapol_wlan_authentication_state_authentication_successfull,
    wlan_eapol_if_eapol_wlan_authentication_state_authentication_cancelled,
    wlan_eapol_if_eapol_wlan_authentication_state_immediate_reconnect,
    wlan_eapol_if_eapol_wlan_authentication_state_wapi_authentication_running
    } wlan_eapol_if_eapol_wlan_authentication_state_e;

typedef enum _wlan_eapol_if_error_e
    {
    wlan_eapol_if_error_ok,
    wlan_eapol_if_error_pending_request,
    wlan_eapol_if_error_allocation_error,
    wlan_eapol_if_error_illegal_parameter,
    wlan_eapol_if_error_process_general_error,
    } wlan_eapol_if_error_e;

typedef enum _wlan_eapol_if_rsna_cipher_e
    {
    wlan_eapol_if_rsna_cipher_none,
    wlan_eapol_if_rsna_cipher_ccmp,
    wlan_eapol_if_rsna_cipher_tkip,
    wlan_eapol_if_rsna_cipher_wep_40,
    wlan_eapol_if_rsna_cipher_wep_104,
    wlan_eapol_if_wapi_cipher_wpi
    } wlan_eapol_if_rsna_cipher_e;

typedef enum _wlan_eapol_ethernet_type_e
    {
    wlan_eapol_ethernet_type_none              = 0,
    wlan_eapol_ethernet_type_pae               = 0x888e, ///< Ethernet type Port Access Entity (PAE)
    wlan_eapol_ethernet_type_preauthentication = 0x88c7, ///< Ethernet type Preauthentication
    wlan_eapol_ethernet_type_wapi              = 0x88b4  ///< Ethernet type WAPI
    } wlan_eapol_ethernet_type_e;

typedef enum _wlan_eapol_if_eap_type_e
    {
    wlan_eapol_if_eap_type_none     = 0,  ///< This is internal value for no type case.
    wlan_eapol_if_eap_type_tls      = 13, ///< This is Transport Layer Security (TLS) type.
    wlan_eapol_if_eap_type_leap     = 17, ///< This is LEAP type.
    wlan_eapol_if_eap_type_gsmsim   = 18, ///< This is SIM type.
    wlan_eapol_if_eap_type_ttls     = 21, ///< This is tunneled TLS.
    wlan_eapol_if_eap_type_aka      = 23, ///< This is AKA type.
    wlan_eapol_if_eap_type_peap     = 25, ///< This is PEAP type.
    wlan_eapol_if_eap_type_mschapv2 = 26, ///< This is MsChapv2 type.
    wlan_eapol_if_eap_type_fast     = 43, ///< This is EAP-FAST type.
    } wlan_eapol_if_eap_type_e;

typedef enum _wlan_eapol_if_eap_status_e
    {
    wlan_eapol_if_eap_status_none                                            	        = 0,
    wlan_eapol_if_eap_status_authentication_failure                          	        = 14,				
    wlan_eapol_if_eap_status_illegal_cipher_suite                            	        = 70,					
    wlan_eapol_if_eap_status_bad_certificate                                 	        = 71,					
    wlan_eapol_if_eap_status_unsupported_certificate                         	        = 72,					
    wlan_eapol_if_eap_status_certificate_revoked                             	        = 73,					
    wlan_eapol_if_eap_status_certificate_expired                             	        = 74,					
    wlan_eapol_if_eap_status_user_certificate_unknown                        	        = 76,					
    wlan_eapol_if_eap_status_identity_query_failed                           	        = 94,					
    wlan_eapol_if_eap_status_user_has_not_subscribed_to_the_requested_service	        = 96,					
    wlan_eapol_if_eap_status_users_calls_are_barred                          	        = 97,					
    wlan_eapol_if_eap_status_restricted_logon_hours                          	        = 98,					
    wlan_eapol_if_eap_status_account_disabled                                	        = 99,					
    wlan_eapol_if_eap_status_no_dialin_permission                            	        = 100,					
    wlan_eapol_if_eap_status_password_expired                                	        = 101,					
    wlan_eapol_if_eap_status_OOB_interface_read_error                        	        = 103,					
    wlan_eapol_if_eap_status_decryption_CRC_failure                          	        = 104,					
    wlan_eapol_if_eap_status_RF_band_2_4_ghz_not_supported                   	        = 105,					
    wlan_eapol_if_eap_status_RF_band_5_0_ghz_not_supported                   	        = 106,					
    wlan_eapol_if_eap_status_signal_too_weak                                 	        = 107,					
    wlan_eapol_if_eap_status_network_authentication_failure                  	        = 108,					
    wlan_eapol_if_eap_status_network_association_failure                     	        = 109,					
    wlan_eapol_if_eap_status_no_DHCP_response                                	        = 110,					
    wlan_eapol_if_eap_status_failed_DHCP_configure                           	        = 111,					
    wlan_eapol_if_eap_status_ip_address_conflict                             	        = 112,					
    wlan_eapol_if_eap_status_could_not_connect_to_registrar                  	        = 113,					
    wlan_eapol_if_eap_status_multiple_PBC_sessions_detected                  	        = 114,					
    wlan_eapol_if_eap_status_rogue_activity_suspected                        	        = 115,					
    wlan_eapol_if_eap_status_device_busy                                     	        = 116,					
    wlan_eapol_if_eap_status_setup_locked                                    	        = 117,					
    wlan_eapol_if_eap_status_message_timeout                                 	        = 118,					
    wlan_eapol_if_eap_status_registration_session_timeout                    	        = 119,					
    wlan_eapol_if_eap_status_device_password_authentication_failure          	        = 120,					
    wlan_eapol_if_eap_status_pin_code_authentication_not_supported           	        = 121,					
    wlan_eapol_if_eap_status_push_button_authentication_not_supported        	        = 122,					
    wlan_eapol_if_eap_status_tunnel_compromise_error                         	        = 124,					
    wlan_eapol_if_eap_status_unexpected_tlv_exhanged                         	        = 125,					
    wlan_eapol_if_eap_status_no_pac_nor_certs_to_authenticate_with_provision_disabled   = 126,
    wlan_eapol_if_eap_status_no_matching_pac_for_aid                                    = 127,
    wlan_eapol_if_eap_status_pac_store_corrupted                                        = 128,    
    } wlan_eapol_if_eap_status_e;

enum wlan_eapol_if_eap_state_e
    {
    wlan_eapol_if_eap_state_general_authentication_error = 4,
    wlan_eapol_if_eap_state_authentication_terminated_unsuccessfully = 7,
    };

/**
 * Structure for storing medium time by user priority.
 */
typedef u16_t medium_time_by_user_priority[MAX_QOS_USER_PRIORITY];

/**
 * Structure for storing medium time by access class.
 */
typedef u16_t medium_time_by_access_class[MAX_QOS_ACCESS_CLASS];

/**
 * Structure for storing medium time by user priority and by access class.
 */
typedef struct _medium_time_s
    {
    /** Admission capacity by user priority. */
    medium_time_by_user_priority up;
    /** Admission capacity by access class. */
    medium_time_by_access_class ac;
    } medium_time_s;

const medium_time_s ADMISSION_CAPACITIES_NOT_DEFINED = { 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF };
const medium_time_s MEDIUM_TIME_NOT_DEFINED = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };    

#endif // CORE_TYPES_H
