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
* %version: 34 %
*/

#include "core_ap_data.h"
#include "core_tools.h"
#include "core_tools_parser.h"
#include "core_frame_beacon.h"
#include "core_frame_dot11_ie.h"
#include "core_frame_mgmt_ie.h"
#include "core_frame_rsn_ie.h"
#include "core_frame_wpa_ie.h"
#include "core_frame_wmm_ie.h"
#include "core_frame_wsc_ie.h"
#include "core_frame_qbss_load_ie.h"
#include "core_frame_wapi_ie.h"
#include "am_debug.h"

/** Defining this enables IE parsing related traces. */
//#define WLAN_CORE_DEEP_DEBUG 1
const u16_t AP_CHANNEL_REPORT_CHANNEL_OFFSET = 3;
const u8_t REQUEST_IE_OFFSET = 2;
/** The maximum number of admission capacities. */
const u16_t MAX_NBR_OF_ADMISSION_CAPACITIES = 12;
const u16_t CHECKSUM_LENGTH = 4;
const u16_t REPORTED_FRAME_BODY_MAX_LENGTH = 224;
const u16_t HEADER_LENGTH = 5;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//   
core_ap_data_c::core_ap_data_c(
    abs_core_wpx_adaptation_c& wpx_adaptation ) :
    wpx_adaptation_m( wpx_adaptation ),
    frame_m( NULL ),
    rcpi_m( 0 ),
    pairwise_cipher_m( core_cipher_suite_none ),
    group_cipher_m( core_cipher_suite_none ),
    key_management_m( core_key_management_none ),
    wpx_version_m( 0 ),
    capabilities_bitmap_m( 0 ),
    is_ac_required_bitmap_m( 0 ),
    dtim_period_m( 0 )
    {
    DEBUG( "core_ap_data_c::core_ap_data_c()" );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_ap_data_c::~core_ap_data_c()
    {
    DEBUG( "core_ap_data_c::~core_ap_data_c()" ); 
    
    delete frame_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_mac_address_s core_ap_data_c::bssid() const
    {
    return frame_m->bssid();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_ssid_s core_ap_data_c::ssid()
    {
    for( core_frame_dot11_ie_c* ie = frame_m->first_ie(); ie; ie = frame_m->next_ie() )
        {
        if ( ie->element_id() == core_frame_dot11_ie_c::core_frame_dot11_ie_element_id_ssid )
            {
            core_frame_mgmt_ie_ssid_c* parser = core_frame_mgmt_ie_ssid_c::instance( *ie );
            if ( parser )
                {
                core_ssid_s ssid = parser->ssid();

                delete parser;
                parser = NULL;
                delete ie;
                ie = NULL;

                return ssid;
                }
            }

        delete ie;
        ie = NULL;
        }

    return BROADCAST_SSID;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u16_t core_ap_data_c::capabilities() const
    {
    return frame_m->capability();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//   
u8_t core_ap_data_c::max_tx_power_level() const
    {
    u8_t tx_power_level( MAX_TX_POWER_LEVEL_NOT_DEFINED );
    u8_t power_constraint( 0 );
    u8_t channel = core_ap_data_c::channel();
    
    for( core_frame_dot11_ie_c* ie = frame_m->first_ie(); ie; ie = frame_m->next_ie() )
        {
        if ( ie->element_id() == core_frame_dot11_ie_c::core_frame_dot11_ie_element_id_country )
            {
            core_frame_mgmt_ie_country_c* parser = core_frame_mgmt_ie_country_c::instance( *ie );
            if ( parser )
                {
                tx_power_level = parser->max_tx_power_level(
                    channel );

                delete parser;
                parser = NULL;
                }
            }
        else if ( ie->element_id() == core_frame_dot11_ie_c::core_frame_dot11_ie_element_id_power_constraint )
            {
            core_frame_mgmt_ie_power_constraint_c* parser2 = core_frame_mgmt_ie_power_constraint_c::instance( *ie );
            if ( parser2 )
                {
                power_constraint = parser2->power_constraint();

                delete parser2;
                parser2 = NULL;
                }
            }

        delete ie;
        ie = NULL;
        }
    
    if(tx_power_level != MAX_TX_POWER_LEVEL_NOT_DEFINED)
        {
        tx_power_level = tx_power_level - power_constraint;
        }

    u8_t temp_power_level = wpx_adaptation_m.max_tx_power_level( frame_m );
    if ( temp_power_level < tx_power_level )
        {
        tx_power_level = temp_power_level;
        }

    return tx_power_level;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//   
core_country_string_s core_ap_data_c::country_info() const
    {
    core_country_string_s country_info( COUNTRY_STRING_NONE );

    for( core_frame_dot11_ie_c* ie = frame_m->first_ie(); ie; ie = frame_m->next_ie() )
        {
        if ( ie->element_id() == core_frame_dot11_ie_c::core_frame_dot11_ie_element_id_country )
            {
            core_frame_mgmt_ie_country_c* parser = core_frame_mgmt_ie_country_c::instance( *ie );
            if ( parser )
                {
                country_info = parser->country();

                delete parser;
                parser = NULL;
                delete ie;
                ie = NULL;

                return country_info;
                }
            }

        delete ie;
        ie = NULL;
        }

   	return country_info;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//   
bool_t core_ap_data_c::is_privacy_enabled() const
    {
    return frame_m->capability() & CAPABILITY_BIT_MASK_PRIVACY;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_ap_data_c* core_ap_data_c::instance(
    const core_ap_data_c& copy )
    {
    DEBUG( "core_ap_data_c::instance()" );
    
    core_ap_data_c* instance = new core_ap_data_c( copy.wpx_adaptation_m );
    if ( !instance )
        {
        DEBUG( "core_ap_data_c::instance() - unable to create an instance" );
        
        return NULL;
        }
        
    *instance = copy;
    
    return instance;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_ap_data_c* core_ap_data_c::instance(
    abs_core_wpx_adaptation_c& wpx_adaptation,
    const core_frame_dot11_c* frame,
    u8_t rcpi,
    bool_t is_copied )
    {
    DEBUG( "core_ap_data_c::instance()" );
    DEBUG1( "core_ap_data_c::instance() - is_copied %u",
        is_copied );

    core_ap_data_c* instance = new core_ap_data_c( wpx_adaptation );
    if ( !instance )
        {
        DEBUG( "core_ap_data_c::instance() - unable to create core_ap_data_c instance" );
        
        return NULL;
        }    
    instance->rcpi_m = rcpi;

    instance->frame_m = core_frame_beacon_c::instance(
        *frame,
        is_copied );
    if ( !instance->frame_m )
        {
        DEBUG( "core_ap_data_c::instance() - unable to create core_frame_beacon_c instance" );

        delete instance;
        instance = NULL;
        
        return NULL;        
        }

    const core_mac_address_s bssid = instance->frame_m->bssid();
    DEBUG6( "core_ap_data_c::instance() - BSSID: %02X:%02X:%02X:%02X:%02X:%02X",
        bssid.addr[0], bssid.addr[1], bssid.addr[2], 
        bssid.addr[3], bssid.addr[4], bssid.addr[5] );
    
    const core_tsf_value_s timestamp = instance->frame_m->timestamp();
    DEBUG8( "core_ap_data_c::instance() - TSF: 0x%02X%02X%02X%02X%02X%02X%02X%02X",
        timestamp.tsf[0], timestamp.tsf[1], timestamp.tsf[2],
        timestamp.tsf[3], timestamp.tsf[4], timestamp.tsf[5], 
        timestamp.tsf[6], timestamp.tsf[7] );
    DEBUG1( "core_ap_data_c::instance() - capability: 0x%04X",
        instance->frame_m->capability() );
    DEBUG1( "core_ap_data_c::instance() - beacon_interval: %u",
        instance->frame_m->beacon_interval() );

    DEBUG1( "core_ap_data_c::instance() - class size: %u",
        sizeof( *instance ) );
    DEBUG1( "core_ap_data_c::instance() - beacon class size: %u",
        frame->data_length() );

#ifdef WLAN_CORE_DEEP_DEBUG
    for( core_frame_dot11_ie_c* ie = instance->frame_m->first_ie(); ie; ie = instance->frame_m->next_ie() )
        {
        DEBUG1( "core_ap_data_c::instance() - IE id: 0x%02X",
            ie->element_id() );
        DEBUG1( "core_ap_data_c::instance() - IE length: %u",
            ie->length() );
        delete ie;
        ie = NULL;
        }
#endif // WLAN_CORE_DEEP_DEBUG

    const core_frame_rsn_ie_c* rsn_ie = instance->rsn_ie();
    if ( rsn_ie )
        {
        DEBUG( "core_ap_data_c::instance() - RSN IE detected" );
        instance->capabilities_bitmap_m |= core_am_ap_data_capability_rsn_ie_present;
        instance->pairwise_cipher_m |= rsn_ie->pairwise_cipher_suites();
        instance->group_cipher_m |=  rsn_ie->group_cipher_suite();
        instance->key_management_m |= rsn_ie->key_management_suites();
        delete rsn_ie;
        rsn_ie = NULL;
        }

    const core_frame_wpa_ie_c* wpa_ie = instance->wpa_ie();
    if ( wpa_ie )
        {
        DEBUG( "core_ap_data_c::instance() - WPA IE detected" );
        instance->capabilities_bitmap_m |= core_am_ap_data_capability_wpa_ie_present;
        instance->pairwise_cipher_m |= wpa_ie->pairwise_cipher_suites();
        instance->group_cipher_m |=  wpa_ie->group_cipher_suite();
        instance->key_management_m |= wpa_ie->key_management_suites();
        delete wpa_ie;
        wpa_ie = NULL;
        }

    const core_frame_wapi_ie_c* wapi_ie = instance->wapi_ie();
    if ( wapi_ie )
        {
        DEBUG( "core_ap_data_c::instance() - WAPI IE detected" );
        instance->capabilities_bitmap_m |= core_am_ap_data_capability_wapi_ie_present;
        instance->pairwise_cipher_m |= wapi_ie->pairwise_cipher_suites();
        instance->group_cipher_m |=  wapi_ie->group_cipher_suite();
        instance->key_management_m |= wapi_ie->key_management_suites();
        delete wapi_ie;
        wapi_ie = NULL;        
        }

    if ( instance->is_rsn_ie_present() ||
         instance->is_wpa_ie_present() ||
         instance->is_wapi_ie_present() )
        {
        DEBUG1( "core_ap_data_c::instance() - supported pairwise ciphers: 0x%02X",
            instance->pairwise_cipher_m );
        DEBUG1( "core_ap_data_c::instance() - supported group cipher: 0x%02X",
            instance->group_cipher_m );
        DEBUG1( "core_ap_data_c::instance() - supported key management suites: 0x%02X",
            instance->key_management_m );
        }

    DEBUG1( "core_ap_data_c::instance() - RCPI: %u",
        instance->rcpi_m );
    DEBUG1( "core_ap_data_c::instance() - channel: %u",
        instance->channel() );
    u8_t max_tx_power_level = instance->max_tx_power_level();
    if ( max_tx_power_level < MAX_TX_POWER_LEVEL_NOT_DEFINED )
        {
        DEBUG1( "core_ap_data_c::instance() - maximum tx power level: %u",
            max_tx_power_level );
        }
    else
        {
        DEBUG( "core_ap_data_c::instance() - maximum tx power level: undefined" );
        }
    
    core_country_string_s country_value = instance->country_info();
    if ( COUNTRY_STRING_NONE != country_value )
        {
        DEBUG2( "core_ap_data_c::instance() - country %c%c",
                country_value.country[0], country_value.country[1] );
        }
    
    instance->wpx_version_m = instance->wpx_adaptation_m.wpx_version(
        instance->frame_m );

    if ( instance->wpx_version_m )
        {
        DEBUG1( "core_ap_data_c::instance() - WPX: v%u",
            instance->wpx_version_m );
        }
    else
        {
        DEBUG( "core_ap_data_c::instance() - WPX: no" );
        }

    const core_frame_wmm_ie_parameter_c* wmm_ie = instance->wmm_parameter_ie();
    if ( wmm_ie )
        {        
        DEBUG( "core_ap_data_c::instance() - WMM Parameter Element IE detected" );
        instance->capabilities_bitmap_m |= core_am_ap_data_capability_wmm_ie_present;
        if ( wmm_ie->is_uapsd_supported() )
            {
            instance->capabilities_bitmap_m |= core_am_ap_data_capability_uapsd_supported;
            }
        if ( wmm_ie->is_admission_control_required( core_access_class_best_effort ) )
            {
            instance->is_ac_required_bitmap_m |= ( 1 << core_access_class_best_effort );
            }        
        if ( wmm_ie->is_admission_control_required( core_access_class_background ) )
            {
            instance->is_ac_required_bitmap_m |= ( 1 << core_access_class_background );
            }        
        if ( wmm_ie->is_admission_control_required( core_access_class_video ) )
            {
            instance->is_ac_required_bitmap_m |= ( 1 << core_access_class_video );
            }        
        if ( wmm_ie->is_admission_control_required( core_access_class_voice ) )
            {
            instance->is_ac_required_bitmap_m |= ( 1 << core_access_class_voice );
            }
        DEBUG1( "core_ap_data_c::instance() - U-APSD: %u",
            instance->is_uapsd_supported() );
        DEBUG1( "core_ap_data_c::instance() - Admission Control for Best Effort: %u",
            instance->is_admission_control_required( core_access_class_best_effort ) );
        DEBUG1( "core_ap_data_c::instance() - Admission Control for Background: %u",
            instance->is_admission_control_required( core_access_class_background ) );
        DEBUG1( "core_ap_data_c::instance() - Admission Control for Video: %u",
            instance->is_admission_control_required( core_access_class_video ) );
        DEBUG1( "core_ap_data_c::instance() - Admission Control for Voice: %u",
            instance->is_admission_control_required( core_access_class_voice ) );

        delete wmm_ie;
        wmm_ie = NULL;
        }

#ifdef _DEBUG
    const core_frame_qbss_load_ie_c* qbss_load_ie = instance->qbss_load_ie();
    if ( qbss_load_ie )
        {
        DEBUG( "core_ap_data_c::instance() - QBSS Load IE detected" );
        DEBUG1( "core_ap_data_c::instance() - Station Count: %u",
            qbss_load_ie->station_count() );
        DEBUG1( "core_ap_data_c::instance() - Channel Utilization: %u",
            qbss_load_ie->channel_utilitization() );
        DEBUG1( "core_ap_data_c::instance() - Available Admission Capacity: %u",
            qbss_load_ie->admission_capacity() );

        delete qbss_load_ie;
        qbss_load_ie = NULL;
        }
#endif // _DEBUG

    return instance;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_ap_data_c::is_short_preamble_allowed() const
    {
    if ( frame_m->capability() & CAPABILITY_BIT_MASK_PREAMBLE )
        {
        return true_t;
        }

    return false_t;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_ap_data_c::is_short_slot_time_used() const
    {
    if ( frame_m->capability() & CAPABILITY_BIT_MASK_SLOT_TIME )
        {
        return true_t;
        }

    return false_t;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_ap_data_c::is_infra() const
    {
    if ( frame_m->capability() & CAPABILITY_BIT_MASK_ESS )
        {
        return true_t;
        }

    return false_t;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_ap_data_c::is_radio_measurement_supported() const
    {
    if ( frame_m->capability() & core_frame_dot11_c::core_dot11_capability_radio_measurement )
        {
        return true_t;
        }

    return false_t;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_ap_data_c::is_wpa_ie_present() const
    {
    if ( capabilities_bitmap_m & core_am_ap_data_capability_wpa_ie_present )
        {
        return true_t;
        }
    
    return false_t;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_ap_data_c::is_rsn_ie_present() const
    {
    if ( capabilities_bitmap_m & core_am_ap_data_capability_rsn_ie_present )
        {
        return true_t;
        }

    return false_t;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_ap_data_c::is_wmm_ie_present() const
    {
    if ( capabilities_bitmap_m & core_am_ap_data_capability_wmm_ie_present )
        {
        return true_t;
        }

    return false_t;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_ap_data_c::is_uapsd_supported() const
    {
    if ( capabilities_bitmap_m & core_am_ap_data_capability_uapsd_supported )
        {
        return true_t;
        }

    return false_t;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_ap_data_c::is_admission_control_required(
    core_access_class_e access_class ) const
    {
    if ( is_ac_required_bitmap_m & ( 1 << access_class ) )
        {
        return true_t;
        }

    return false_t;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_ap_data_c::is_wapi_ie_present() const
    {
    if ( capabilities_bitmap_m & core_am_ap_data_capability_wapi_ie_present )
        {
        return true_t;
        }
    
    return false_t;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//   
u8_t core_ap_data_c::pairwise_ciphers() const
    {
    return pairwise_cipher_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//           
u8_t core_ap_data_c::group_cipher() const
    {
    return group_cipher_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//               
u8_t core_ap_data_c::key_management_suites() const
    {
    return key_management_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//               
core_cipher_suite_e core_ap_data_c::best_pairwise_cipher() const
    {
    return best_cipher(
        pairwise_cipher_m );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//                       
core_cipher_suite_e core_ap_data_c::best_group_cipher() const
    {
    return best_cipher(
        group_cipher_m );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//   
core_cipher_suite_e core_ap_data_c::best_cipher(
    u32_t ciphers ) const
    {
    if ( ciphers & core_cipher_suite_wpi )
        {
        return core_cipher_suite_wpi;
        }

    if( ciphers & core_cipher_suite_ccmp )
        {
        return core_cipher_suite_ccmp;
        }

    if( ciphers & core_cipher_suite_tkip )
        {
        return core_cipher_suite_tkip;
        }

    if( ciphers & core_cipher_suite_wep104 )
        {
        return core_cipher_suite_wep104;
        }

    if( ciphers & core_cipher_suite_wep40 )
        {
        return core_cipher_suite_wep40;
        }

    return core_cipher_suite_none;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_ap_data_c::is_wpx() const
    {
    return wpx_version_m != 0;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u8_t core_ap_data_c::wpx_version() const
    {
    return wpx_version_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u8_t core_ap_data_c::channel() const
    {
    for( core_frame_dot11_ie_c* ie = frame_m->first_ie(); ie; ie = frame_m->next_ie() )
        {
        if ( ie->element_id() == core_frame_dot11_ie_c::core_frame_dot11_ie_element_id_ds_parameter_set )
            {
            core_frame_mgmt_ie_ds_c* parser = core_frame_mgmt_ie_ds_c::instance( *ie );
            if ( parser )
                {
                u8_t channel = parser->channel();

                delete parser;
                parser = NULL;
                delete ie;
                ie = NULL;

                return channel;
                }
            }

        delete ie;
        ie = NULL;
        }

    return 0;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_ap_data_c& core_ap_data_c::operator=(
    const core_ap_data_c& src )
    {
    // Check first assignment to itself
    if( &src == this)
    {
        DEBUG( "core_ap_data_c::operator=() - assignment to this");
        return *this;
    }
    delete frame_m;
    frame_m = NULL;

    frame_m = core_frame_beacon_c::instance(
        *src.frame(),
        true_t );

    rcpi_m = src.rcpi_m;
    pairwise_cipher_m = src.pairwise_cipher_m;
    group_cipher_m = src.group_cipher_m;
    key_management_m = src.key_management_m;
    wpx_version_m = src.wpx_version_m;
    capabilities_bitmap_m = src.capabilities_bitmap_m;
    is_ac_required_bitmap_m = src.is_ac_required_bitmap_m;
    dtim_period_m = src.dtim_period_m;

    return *this;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_ap_data_c::is_wpx_fast_roam_available() const
    {
    return key_management_m & core_key_management_wpx_fast_roam;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_tsf_value_s core_ap_data_c::timestamp() const
    {
    return frame_m->timestamp();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u8_t core_ap_data_c::band() const 
    {
    return SCAN_BAND_2DOT4GHZ;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u8_t core_ap_data_c::rcpi() const
    {
    return rcpi_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u16_t core_ap_data_c::beacon_interval() const
    {
    return frame_m->beacon_interval();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u8_t core_ap_data_c::dtim_period() const
    {
    return dtim_period_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_ap_data_c::set_dtim_period(
    u8_t dtim_period )
    {
    dtim_period_m = dtim_period; 
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_beacon_c* core_ap_data_c::frame() const
    {
    return frame_m;    
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
const core_frame_wpa_ie_c* core_ap_data_c::wpa_ie()
    {
    for( core_frame_dot11_ie_c* ie = frame_m->first_ie(); ie; ie = frame_m->next_ie() )
        {
        if ( ie->element_id() == core_frame_dot11_ie_c::core_frame_dot11_ie_element_id_wpa )
            {
            core_frame_wpa_ie_c* wpa_ie = core_frame_wpa_ie_c::instance( wpx_adaptation_m, *ie );
            if ( wpa_ie )
                {
                delete ie;
                ie = NULL;

                return wpa_ie;
                }
            }

        delete ie;
        ie = NULL;
        }

    return NULL;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
const core_frame_rsn_ie_c* core_ap_data_c::rsn_ie()
    {    
    for( core_frame_dot11_ie_c* ie = frame_m->first_ie(); ie; ie = frame_m->next_ie() )
        {
        if ( ie->element_id() == core_frame_dot11_ie_c::core_frame_dot11_ie_element_id_rsn )
            {
            core_frame_rsn_ie_c* rsn_ie = core_frame_rsn_ie_c::instance( wpx_adaptation_m, *ie );
            if ( rsn_ie )
                {
                delete ie;
                ie = NULL;

                return rsn_ie;
                }
            }

        delete ie;
        ie = NULL;
        }

    return NULL;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
const core_frame_wsc_ie_c* core_ap_data_c::wsc_ie()
    {
    for( core_frame_dot11_ie_c* ie = frame_m->first_ie(); ie ; ie = frame_m->next_ie() )
        {
        if ( ie->element_id() == core_frame_dot11_ie_c::core_frame_dot11_ie_element_id_wpa )
            {
            core_frame_wsc_ie_c* wsc_ie = core_frame_wsc_ie_c::instance( *ie );
            if ( wsc_ie )
                {
                delete ie;
                ie = NULL;

                return wsc_ie;
                }
            }

        delete ie;
        ie = NULL;
        }

    return NULL;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
const core_frame_wmm_ie_parameter_c* core_ap_data_c::wmm_parameter_ie()
    {
    for( core_frame_dot11_ie_c* ie = frame_m->first_ie(); ie; ie = frame_m->next_ie() )
        {
        if ( ie->element_id() == core_frame_dot11_ie_c::core_frame_dot11_ie_element_id_wmm_parameter )
            {
            core_frame_wmm_ie_parameter_c* wmm_ie = core_frame_wmm_ie_parameter_c::instance( *ie );
            if ( wmm_ie )
                {
                delete ie;
                ie = NULL;

                return wmm_ie;
                }
            }

        delete ie;
        ie = NULL;
        }

    return NULL;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
const core_frame_qbss_load_ie_c* core_ap_data_c::qbss_load_ie()
    {
    for( core_frame_dot11_ie_c* ie = frame_m->first_ie(); ie; ie = frame_m->next_ie() )
        {
        if ( ie->element_id() == core_frame_dot11_ie_c::core_frame_dot11_ie_element_id_qbss_load )
            {
            core_frame_qbss_load_ie_c* qbss_load_ie = core_frame_qbss_load_ie_c::instance( *ie );
            if ( qbss_load_ie )
                {
                delete ie;
                ie = NULL;

                return qbss_load_ie;
                }
            }

        delete ie;
        ie = NULL;
        }

    return NULL;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
const core_frame_wapi_ie_c* core_ap_data_c::wapi_ie()
    {    
    for( core_frame_dot11_ie_c* ie = frame_m->first_ie(); ie; ie = frame_m->next_ie() )
        {
        if ( ie->element_id() == core_frame_dot11_ie_c::core_frame_dot11_ie_element_id_wapi )
            {
            core_frame_wapi_ie_c* wapi_ie = core_frame_wapi_ie_c::instance( *ie );
            if ( wapi_ie )
                {
                delete ie;
                ie = NULL;

                return wapi_ie;
                }
            }

        delete ie;
        ie = NULL;
        }

    return NULL;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_scan_channels_c core_ap_data_c::ap_channel_report()
    {
    u8_t channel( 0 );
    core_scan_channels_c ap_channel_list;

    for( core_frame_dot11_ie_c* ie = frame_m->first_ie(); ie ; ie = frame_m->next_ie() )
        {
        if ( ie->element_id() == core_frame_dot11_ie_c::core_frame_dot11_ie_element_id_ap_channel_report )
            {
            core_frame_mgmt_ie_ap_channel_report_c* parser = core_frame_mgmt_ie_ap_channel_report_c::instance( *ie );
            if ( parser )
                {
                for( u16_t i=0; i < (ie->data_length() - AP_CHANNEL_REPORT_CHANNEL_OFFSET); i++ )
                    {
                    channel = parser->ap_channel_report_channel( i );
                    ap_channel_list.add( SCAN_BAND_2DOT4GHZ, channel );
                    }
                
                delete parser;
                parser = NULL;
                }
            }

        delete ie;
        ie = NULL;
        }

    return ap_channel_list;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_ap_data_c::admission_capacity(
    medium_time_s& admission_capacities )
    {
    u16_t admission_capacity_table[MAX_NBR_OF_ADMISSION_CAPACITIES];
    admission_capacities = ADMISSION_CAPACITIES_NOT_DEFINED;

    for( core_frame_dot11_ie_c* ie = frame_m->first_ie(); ie; ie = frame_m->next_ie() )
        {
        if ( ie->element_id() == core_frame_dot11_ie_c::core_frame_dot11_ie_element_id_admission_capacity )
            {
            core_frame_mgmt_ie_admission_capacity_c* parser = core_frame_mgmt_ie_admission_capacity_c::instance( *ie );
            if ( parser )
                {
                u16_t bitmask(
                    parser->admission_capacity_bitmask() );
                parser->get_admission_capacity( ie->data_length(), admission_capacity_table );

                /**
                 * Admission capacity by user priority.
                 */                
                for( u8_t idx( 0 ); idx < MAX_QOS_USER_PRIORITY; ++idx )
                    {
                    if( bitmask & 0x0001 )
                        {
                        admission_capacities.up[idx] = admission_capacity_table[idx];
                        }

                    bitmask = bitmask >> 1;
                    }

                /**
                 * Admission capacity by access class.
                 */                
                for( u8_t idx( 0 ); idx < MAX_QOS_ACCESS_CLASS; ++idx )
                    {
                    if( bitmask & 0x0001 )
                        {
                        admission_capacities.ac[idx] = admission_capacity_table[idx + MAX_QOS_USER_PRIORITY];
                        }

                    bitmask = bitmask >> 1;                    
                    }

                delete parser;
                parser = NULL;
                }
            }

        delete ie;
        ie = NULL;
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u8_t core_ap_data_c::power_capability()
    {
    u8_t min_capability( 0 );

    for( core_frame_dot11_ie_c* ie = frame_m->first_ie(); ie ; ie = frame_m->next_ie() )
        {
        if ( ie->element_id() == core_frame_dot11_ie_c::core_frame_dot11_ie_element_id_power_capability )
            {
            core_frame_mgmt_ie_power_capability_c* parser = core_frame_mgmt_ie_power_capability_c::instance( *ie );
            if ( parser )
                {
                min_capability = parser->power_capability();

                delete parser;
                parser = NULL;
                }
            }

        delete ie;
        ie = NULL;
        }

    return min_capability;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u64_t core_ap_data_c::rrm_capabilities()
    {
    u64_t rrm_capabilities( 0 );

    for( core_frame_dot11_ie_c* ie = frame_m->first_ie(); ie ; ie = frame_m->next_ie() )
        {
        if ( ie->element_id() == core_frame_dot11_ie_c::core_frame_dot11_ie_element_id_rrm_capabilities )
            {
            core_frame_mgmt_ie_rrm_capabilities_c* parser = core_frame_mgmt_ie_rrm_capabilities_c::instance( *ie );
            if ( parser )
                {
                rrm_capabilities = parser->rrm_capabilities();

                delete parser;
                parser = NULL;

                delete ie;
                ie = NULL;

                return rrm_capabilities;
                }
            }

        delete ie;
        ie = NULL;
        }

    return rrm_capabilities;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_ap_data_c::get_reported_frame_body(
    u16_t* reported_frame_body_length,
    u8_t* reported_frame_body )
    {
    u8_t frame_body[REPORTED_FRAME_BODY_MAX_LENGTH] = {0};
    
    *reported_frame_body_length = frame_m->data_length() - frame_m->first_fixed_element_offset() - CHECKSUM_LENGTH;

    core_tools_c::copy(
        &frame_body[0],
        const_cast<u8_t*>( frame_m->data() ),
        frame_m->data_length() );

    core_tools_c::copy(
        &reported_frame_body[0],
        const_cast<u8_t*>( &frame_body[frame_m->first_fixed_element_offset()] ),
        *reported_frame_body_length );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_ap_data_c::request_ie(
    u16_t* reported_frame_body_length,
    u8_t* reported_frame_body,
    u8_t* ie_id_list,
    u8_t ie_id_list_length )
    {
    u8_t index = HEADER_LENGTH;
    bool_t frame_too_long = false_t;

    *reported_frame_body_length = frame_m->data_length() - frame_m->first_fixed_element_offset() - CHECKSUM_LENGTH;

    core_tools_c::copy(
        &reported_frame_body[0],
        const_cast<u8_t*>( frame_m->data() ),
        index );

    for( core_frame_dot11_ie_c* ie = frame_m->first_ie(); ie ; ie = frame_m->next_ie() )
        {
        for( u16_t j=0; j < ie_id_list_length; j++ )
            {
            if ( ie->element_id() == ie_id_list[j] )
                {
                if( index + ie->data_length() <= REPORTED_FRAME_BODY_MAX_LENGTH )
                    {
                    core_frame_mgmt_ie_request_ie_c* parser = core_frame_mgmt_ie_request_ie_c::instance( *ie );
                    if ( parser )
                        {
                        reported_frame_body[index] = ie->element_id();
                        index++;
                        reported_frame_body[index] = ie->data_length() - REQUEST_IE_OFFSET;
                        index++;
                        for( u16_t i=0; i < ( ie->data_length() - REQUEST_IE_OFFSET ); i++ )
                            {
                            reported_frame_body[index] = parser->element_data( i );
                            index++;
                            }
                        
                        delete parser;
                        parser = NULL;
                        }
                    }
                else
                    {
                    frame_too_long = true_t;
                    break;
                    }
                }
            }

        delete ie;
        ie = NULL;
        
        if ( frame_too_long == true_t )
            {
            break;
            }
        }
    
    *reported_frame_body_length = index;
    }
