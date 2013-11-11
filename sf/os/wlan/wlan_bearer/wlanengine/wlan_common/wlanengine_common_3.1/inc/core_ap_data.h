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
* Description:  Class holding access point specific information
*
*/

/*
* %version: 23 %
*/

#ifndef CORE_AP_DATA_H
#define CORE_AP_DATA_H

#include "core_types.h"
#include "core_scan_channels.h"
#include "core_frame_mgmt_ie.h"
#include "abs_core_wpx_adaptation.h"

const u32_t CORE_EAPOL_WPAIE_DATA_OFFSET = 2;

class core_frame_dot11_c;
class core_frame_dot11_ie_c;
class core_frame_rsn_ie_c;
class core_frame_wpa_ie_c;
class core_frame_wsc_ie_c;
class core_frame_beacon_c;
class core_frame_wmm_ie_parameter_c;
class core_frame_qbss_load_ie_c;
class core_frame_wapi_ie_c;

/**
 * Class encapsulating a single AP parsed from the scan results.
 *
 * @since S60 v3.1
 */
NONSHARABLE_CLASS( core_ap_data_c )
    {

public:

    typedef enum _core_am_ap_data_capability
        {
        core_am_ap_data_capability_rsn_ie_present = 1,
        core_am_ap_data_capability_wpa_ie_present = 2,
        core_am_ap_data_capability_wmm_ie_present = 4, 
        core_am_ap_data_capability_uapsd_supported = 8,
        core_am_ap_data_capability_wapi_ie_present = 16
        } core_am_ap_data_capability;

    /**
     * Factory method used for creating an instance of core_ap_data_c
     * encapsulating the copy of the given core_ap_data_c instance.
     *
     * @since S60 v3.1
     * @param copy Instance to be copied.
     * @return Instance of the class, NULL if parsing failed.
     */    
    static core_ap_data_c* instance(
        const core_ap_data_c& copy );

    /**
     * Factory method used for creating an instance of core_ap_data_c
     * encapsulating the given beacon/probe response frame.
     *
     * @since S60 v3.1
     * @param wpx_adaptation WPX adaptation.
     * @param frame Beacon/probe response frame to be encapsulated.
     * @param rcpi RCPI value of the frame.
     * @param is_copied Whether the object should take a copy of the frame data.
     * @return Instance of the class, NULL if parsing failed.
     */
    static core_ap_data_c* instance(
        abs_core_wpx_adaptation_c& wpx_adaptation,
        const core_frame_dot11_c* frame,
        u8_t rcpi,
        bool_t is_copied );

    /**
     * Destructor.
     */
    virtual ~core_ap_data_c();

    /**
     * Get BSSID of the AP.
     *
     * @since S60 v3.1
     * @return BSSID of the AP.
     */
    core_mac_address_s bssid() const;

    /**
     * Get the SSID of the AP.
     */
    core_ssid_s ssid();

    /**
     * Get the capabilities field of the AP.
     *
     * @since S60 v3.1
     * @return Capabilities field of the AP.
     */
    u16_t capabilities() const;
    
    /**
     * Get the maximum allowed tx power level.
     *
     * @since S60 v3.1
     * @return Maximum allowed tx power level,
     *         MAX_TX_POWER_LEVEL_NOT_DEFINED if not defined.
     */
    u8_t max_tx_power_level() const;

    /**
     * Get the country information.
     *
     * @since S60 v5.0
     * @return Country information, country information is zero if not defined.
     */
    core_country_string_s country_info() const;
    
    /**
     * Get the privacy field of the AP.
     *
     * @since S60 v3.1
     * @return Privacy field of the AP.
     */
    bool_t is_privacy_enabled() const;
            
    /**
     * whether short preamble is allowed or not. If not, long must be used.
     *
     * @since S60 v3.1
     * @return true if short is allowed
     */
    bool_t is_short_preamble_allowed() const;
    
    /**
     * whether short slot time is allowed or not. If not, long must be used.
     *
     * @since S60 v3.1
     * @return true if short is allowed
     */
    bool_t is_short_slot_time_used() const;
    
    /**
     * whether accesspoint (i.e. infra) or ad hoc network
     *
     * @since S60 v3.1
     * @return true if infrastrcture network
     */
    bool_t is_infra() const;

    /**
     * Whether radio measurement is supported.
     *
     * @since S60 v5.2
     * @return Whether radio measurement is supported
     */
    bool_t is_radio_measurement_supported() const;

    /**
     * is WPA IE present or not
     *
     * @since S60 v3.1
     * @return true if present
     */
    bool_t is_wpa_ie_present() const;

    /**
     * is RSN IE present or not
     *
     * @since S60 v3.1
     * @return true if present
     */
    bool_t is_rsn_ie_present() const;

    /**
     * is WMM IE present or not
     *
     * @since S60 v3.2
     * @return true if present
     */
    bool_t is_wmm_ie_present() const;

    /**
     * Whether U-APSD is supported.
     * 
     * @since S60 v3.2
     * @return Whether U-APSD is supported.
     */
    bool_t is_uapsd_supported() const;    

    /**
     * Return whether Admission Control is mandatory for the given access class.
     * @return Whether Admission Control is mandatory for the given access class.
     */
    bool_t is_admission_control_required(
        core_access_class_e access_class ) const;

    /**
     * is WAPI IE present or not
     *
     * @since S60 v3.2
     * @return true if present
     */
    bool_t is_wapi_ie_present() const;
    
    /**
     * pairwise_ciphers
     *
     * @since S60 v3.1
     * @return pairwise key
     */
    u8_t pairwise_ciphers() const;
    
    /**
     * group_cipher
     *
     * @since S60 v3.1
     * @return group key
     */
    u8_t group_cipher() const;

    /**
     * key_management_suites of the AP
     *
     * @since S60 v3.1
     * @return 
     */
    u8_t key_management_suites() const;

    /**
     * best_pairwise_cipher
     *
     * @since S60 v3.1
     * @return 
     */
    core_cipher_suite_e best_pairwise_cipher() const;
    
    /**
     * best_group_cipher
     *
     * @since S60 v3.1
     * @return 
     */
    core_cipher_suite_e best_group_cipher() const;

    /**
     * is wpx supported or not by the AP
     *
     * @since S60 v3.1
     * @return 
     */
    bool_t is_wpx() const;

    /**
     * Return the WPX version.
     *
     * @since S60 v3.2
     * @return The WPX version.
     */
    u8_t wpx_version() const;

    /**
     * channel where AP operates
     *
     * @since S60 v3.1
     * @return 
     */
    u8_t channel() const;

    /**
     * is_wpx_fast_roam_available
     *
     * @since S60 v3.1
     * @return 
     */
    bool_t is_wpx_fast_roam_available() const;

    /**
     * timestamp
     *
     * @since S60 v3.1
     * @return 
     */
    core_tsf_value_s timestamp() const;

    /**
     * band
     *
     * @since S60 v3.1
     * @return 
     */
    u8_t band() const;

    /**
     * Get RCPI value of the frame.
     *
     * @since S60 v3.1
     * @return RCPI value of the frame.
     */
    u8_t rcpi() const;

    /**
     * Return the beacon interval of the frame.
     * 
     * @since S60 v3.1
     * @return The beacon interval of the frame.
     */
    u16_t beacon_interval() const;

    /**
     * Return the DTIM period of the AP.
     *
     * @since S60 v3.2
     * @return The DTIM period of the AP.
     */
    u8_t dtim_period() const;
    
    /**
     * Set the DTIM period of the AP.
     *
     * @since S60 v3.2
     * @param dtim_period The DTIM period of the AP.
     */
    void set_dtim_period(
        u8_t dtim_period );

    /**
     * Get the actual beacon/probe response frame of the AP.
     *
     * @since S60 v3.1
     * @return The actual beacon/probe response frame of the AP.
     */
    core_frame_beacon_c* frame() const;

    /**
     * Return an WPA IE parser instance.
     * @return NULL if no WPA IE present, pointer to instance otherwise.
     */
    const core_frame_wpa_ie_c* wpa_ie();

    /**
     * Return an RSN IE parser instance.
     * @return NULL if no RSN IE present, pointer to instance otherwise.
     */
    const core_frame_rsn_ie_c* rsn_ie();

    /**
     * Return an WSC IE parser instance.
     * @return NULL if no WSC IE present, pointer to instance otherwise.
     */
    const core_frame_wsc_ie_c* wsc_ie();
    
    /**
     * Return a WMM Parameter Element IE parser instance.
     * @return NULL if no WMM Parameter Element IE present, pointer to instance otherwise.
     */
    const core_frame_wmm_ie_parameter_c* wmm_parameter_ie();    

    /**
     * Return an 802.11e QBSS Load IE parser instance.
     * @return NULL if no 802.11e QBSS Load IE present, pointer to instance otherwise.
     */
    const core_frame_qbss_load_ie_c* qbss_load_ie();
    
    /**
     * Process AP channel report information elements.
     *
     * @since S60 v5.1
     * @return 
     */
    core_scan_channels_c ap_channel_report();

    /**
     * Return a WAPI IE parser instance.
     * @return NULL if no WAPI IE present, pointer to instance otherwise.
     */
    const core_frame_wapi_ie_c* wapi_ie();    

    /**
     * Get an 802.11k Admission Capacities.
     * @param admission_capacities Admission Capacities.
     */
    void admission_capacity(
        medium_time_s& admission_capacities );
    
    /**
     * Return power capability.
     * @return power capability.
     */
    u8_t power_capability();

    /**
     * Return RRM capabilities.
     * @return RRM capabilities.
     */
    u64_t rrm_capabilities();
    
    /**
     * Get reported frame body.
     * @param reported_frame_body_length Reported frame body length.
     * @param reported_frame_body Reported frame body.
     */
    void get_reported_frame_body( 
        u16_t* reported_frame_body_length,
        u8_t* reported_frame_body );

    /**
     * Get Request IE.
     * @param reported_frame_body_length Reported frame body length.
     * @param reported_frame_body Reported frame body.
     * @param ie_id_list IE ID list.
     * @param ie_id_list_length IE ID list length.
     */
    void request_ie(
        u16_t* reported_frame_body_length,
        u8_t* reported_frame_body,
        u8_t* ie_id_list,
        u8_t ie_id_list_length );
    
    /**
     * Assignment operator.
     */
    core_ap_data_c& operator=(
        const core_ap_data_c& src );

private:

    /**
     * Constructor.
     *
     * @param wpx_adaptation WPX adaptation.
     */
    core_ap_data_c(
        abs_core_wpx_adaptation_c& wpx_adaptation );

    /**
     * Return the most secure cipher from the given list.
     * @param ciphers List of ciphers to choose from.
     * @return The most secure cipher from the given list. 
     */
    core_cipher_suite_e best_cipher(
        u32_t ciphers ) const;

private: // data

    /** WPX adaptation instance. */
    abs_core_wpx_adaptation_c& wpx_adaptation_m;

    /** Beacon/Probe response frame. */
    core_frame_beacon_c* frame_m;

    /** RCPI value of the frame. */
    u8_t rcpi_m;

    /** Supported pairwise ciphers. */
    u8_t pairwise_cipher_m;

    /** Supported group ciphers. */
    u8_t group_cipher_m;

    /** Supported key management suites. */
    u8_t key_management_m;

    /** The WPX version supported by the AP. */
    u8_t wpx_version_m;

    /** Bitmap of different boolean capabilities. */
    u8_t capabilities_bitmap_m;

    /** Bitmap of AC classes that require admission control. */
    u8_t is_ac_required_bitmap_m;

    /** DTIM period. */
    u8_t dtim_period_m;

    };

#endif // CORE_AP_DATA_H
