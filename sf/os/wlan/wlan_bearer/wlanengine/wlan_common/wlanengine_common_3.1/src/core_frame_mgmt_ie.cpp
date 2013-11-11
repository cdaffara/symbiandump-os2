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
* Description:  Class for parsing 802.11i (RSN) IEs.
*
*/

#include "core_frame_mgmt_ie.h"
#include "core_tools.h"
#include "core_tools_parser.h"
#include "am_debug.h"

const u8_t CORE_FRAME_MGMT_IE_SSID_MAX_LENGTH = 34;
const u8_t CORE_FRAME_MGMT_IE_SSID_OFFSET = 2;

const u8_t CORE_FRAME_MGMT_IE_DS_LENGTH = 3;
const u8_t CORE_FRAME_MGMT_IE_DS_OFFSET = 2;

const u8_t CORE_FRAME_MGMT_IE_COUNTRY_MIN_LENGTH = 8;
const u8_t CORE_FRAME_MGMT_IE_COUNTRY_OFFSET = 2;
const u8_t CORE_FRAME_MGMT_IE_COUNTRY_FIRST_CHANNEL_OFFSET = 5;
const u8_t CORE_FRAME_MGMT_IE_COUNTRY_SET_LENGTH = 3;
const u8_t CORE_FRAME_MGMT_IE_COUNTRY_SET_CH_OFFSET = 0;
const u8_t CORE_FRAME_MGMT_IE_COUNTRY_SET_NR_OFFSET = 1;
const u8_t CORE_FRAME_MGMT_IE_COUNTRY_SET_TX_OFFSET = 2;

const u8_t CORE_FRAME_MGMT_IE_POWER_CONSTRAINT_LENGTH = 3;
const u8_t CORE_FRAME_MGMT_IE_POWER_CONSTRAINT_OFFSET = 2;

const u16_t CORE_FRAME_MGMT_IE_ADMISSION_CAPACITY_LENGTH = 4;
const u16_t CORE_FRAME_MGMT_IE_ADMISSION_CAPACITY_BITMASK_OFFSET = 2;
const u16_t CORE_FRAME_MGMT_IE_ADMISSION_CAPACITY_OFFSET = 4;

const u16_t CORE_FRAME_MGMT_IE_POWER_CAPABILITY_LENGTH = 4;
const u16_t CORE_FRAME_MGMT_IE_POWER_CAPABILITY_OFFSET = 2;

const u16_t CORE_FRAME_MGMT_IE_RRM_CAPABILITIES_LENGTH = 7;
const u16_t CORE_FRAME_MGMT_IE_RRM_CAPABILITIES_OFFSET = 2;

const u8_t CORE_FRAME_MGMT_IE_REQUEST_IE_MAX_LENGTH = 237;
const u8_t CORE_FRAME_MGMT_IE_REQUEST_IE_OFFSET = 2;

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_mgmt_ie_ssid_c* core_frame_mgmt_ie_ssid_c::instance(
    const core_frame_dot11_ie_c& ie )
    {
    if ( ie.data_length() > CORE_FRAME_MGMT_IE_SSID_MAX_LENGTH )
        {
        DEBUG( "core_frame_mgmt_ie_ssid_c::instance() - not a valid IE, too long" );

        return NULL;
        }

    core_frame_mgmt_ie_ssid_c* instance = new core_frame_mgmt_ie_ssid_c(
        ie.data_length(),
        ie.data(),
        0 );
    if ( !instance )
        {
        DEBUG( "core_frame_mgmt_ie_ssid_c::instance() - unable to create an instance" );

        return NULL;
        }
    
    return instance;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//       
core_frame_mgmt_ie_ssid_c::~core_frame_mgmt_ie_ssid_c()
    {
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_ssid_s core_frame_mgmt_ie_ssid_c::ssid() const
    {
    core_ssid_s ssid( BROADCAST_SSID );

    ssid.length = length();
    core_tools_c::copy(
        &ssid.ssid[0],
        data_m + CORE_FRAME_MGMT_IE_SSID_OFFSET,
        ssid.length );    

    return ssid;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_mgmt_ie_ssid_c::core_frame_mgmt_ie_ssid_c(
    u16_t data_length,
    const u8_t* data,
    u16_t max_data_length ) :
    core_frame_dot11_ie_c( data_length, data, max_data_length )
    {
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_mgmt_ie_ds_c* core_frame_mgmt_ie_ds_c::instance(
    const core_frame_dot11_ie_c& ie )
    {
    if ( ie.data_length() != CORE_FRAME_MGMT_IE_DS_LENGTH )
        {
        DEBUG( "core_frame_mgmt_ie_ds_c::instance() - not a valid IE, invalid length" );

        return NULL;
        }

    core_frame_mgmt_ie_ds_c* instance = new core_frame_mgmt_ie_ds_c(
        ie.data_length(),
        ie.data(),
        0 );
    if ( !instance )
        {
        DEBUG( "core_frame_mgmt_ie_ds_c::instance() - unable to create an instance" );

        return NULL;
        }
    
    return instance;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//       
core_frame_mgmt_ie_ds_c::~core_frame_mgmt_ie_ds_c()
    {
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u8_t core_frame_mgmt_ie_ds_c::channel() const
    {
    return data_m[CORE_FRAME_MGMT_IE_DS_OFFSET];
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_mgmt_ie_ds_c::core_frame_mgmt_ie_ds_c(
    u16_t data_length,
    const u8_t* data,
    u16_t max_data_length ) :
    core_frame_dot11_ie_c( data_length, data, max_data_length )
    {
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_mgmt_ie_country_c* core_frame_mgmt_ie_country_c::instance(
    const core_frame_dot11_ie_c& ie )
    {
    if ( ie.data_length() < CORE_FRAME_MGMT_IE_COUNTRY_MIN_LENGTH )
        {
        DEBUG( "core_frame_mgmt_ie_country_c::instance() - not a valid IE, invalid length" );

        return NULL;
        }

    core_frame_mgmt_ie_country_c* instance = new core_frame_mgmt_ie_country_c(
        ie.data_length(),
        ie.data(),
        0 );
    if ( !instance )
        {
        DEBUG( "core_frame_mgmt_ie_country_c::instance() - unable to create an instance" );

        return NULL;
        }
    
    return instance;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//       
core_frame_mgmt_ie_country_c::~core_frame_mgmt_ie_country_c()
    {
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_country_string_s core_frame_mgmt_ie_country_c::country() const
    {
    core_country_string_s country = { 0, 0, 0 };
    core_tools_c::copy(
        &country.country[0],
        &data_m[CORE_FRAME_MGMT_IE_COUNTRY_OFFSET],
        MAX_COUNTRY_STRING_LENGTH );

    return country;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//    
u8_t core_frame_mgmt_ie_country_c::max_tx_power_level(
    u8_t channel ) const
    {
    for ( const u8_t* idx = data_m + CORE_FRAME_MGMT_IE_COUNTRY_FIRST_CHANNEL_OFFSET;
          idx + CORE_FRAME_MGMT_IE_COUNTRY_SET_LENGTH <= data_m + data_length_m;
          idx += CORE_FRAME_MGMT_IE_COUNTRY_SET_LENGTH )            
            {
            u8_t channel_start = idx[CORE_FRAME_MGMT_IE_COUNTRY_SET_CH_OFFSET];
            u8_t channel_end = channel_start + idx[CORE_FRAME_MGMT_IE_COUNTRY_SET_NR_OFFSET];

            if ( channel >= channel_start &&
                 channel < channel_end )
                {
                return idx[CORE_FRAME_MGMT_IE_COUNTRY_SET_TX_OFFSET];
                }
            }

    return MAX_TX_POWER_LEVEL_NOT_DEFINED;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_mgmt_ie_country_c::core_frame_mgmt_ie_country_c(
    u16_t data_length,
    const u8_t* data,
    u16_t max_data_length ) :
    core_frame_dot11_ie_c( data_length, data, max_data_length )
    {
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_mgmt_ie_power_constraint_c* core_frame_mgmt_ie_power_constraint_c::instance(
    const core_frame_dot11_ie_c& ie )
    {
    if ( ie.data_length() != CORE_FRAME_MGMT_IE_POWER_CONSTRAINT_LENGTH )
        {
        DEBUG( "core_frame_mgmt_ie_power_constraint_c::instance() - not a valid IE, invalid length" );

        return NULL;
        }

    core_frame_mgmt_ie_power_constraint_c* instance = new core_frame_mgmt_ie_power_constraint_c(
        ie.data_length(),
        ie.data(),
        0 );
    if ( !instance )
        {
        DEBUG( "core_frame_mgmt_ie_power_constraint_c::instance() - unable to create an instance" );

        return NULL;
        }
        
    return instance;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//       
core_frame_mgmt_ie_power_constraint_c::~core_frame_mgmt_ie_power_constraint_c()
    {
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u8_t core_frame_mgmt_ie_power_constraint_c::power_constraint() const
    {
    return data_m[CORE_FRAME_MGMT_IE_POWER_CONSTRAINT_OFFSET];
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_mgmt_ie_power_constraint_c::core_frame_mgmt_ie_power_constraint_c(
    u16_t data_length,
    const u8_t* data,
    u16_t max_data_length ) :
    core_frame_dot11_ie_c( data_length, data, max_data_length )
    {
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_mgmt_ie_ap_channel_report_c* core_frame_mgmt_ie_ap_channel_report_c::instance(
    const core_frame_dot11_ie_c& ie )
    {
    if ( ie.data_length() < CORE_FRAME_MGMT_IE_AP_CHANNEL_REPORT_LENGTH )
        {
        DEBUG( "core_frame_mgmt_ie_ap_channel_report_c::instance() - not a valid IE, invalid length" );

        return NULL;
        }

    core_frame_mgmt_ie_ap_channel_report_c* instance = new core_frame_mgmt_ie_ap_channel_report_c(
        ie.data_length(),
        ie.data(),
        0 );
    if ( !instance )
        {
        DEBUG( "core_frame_mgmt_ie_ap_channel_report_c::instance() - unable to create an instance" );

        return NULL;
        }

    DEBUG1( "core_frame_mgmt_ie_ap_channel_report_c::instance() - AP Channel Report: 0x%02X",
            instance->ap_channel_report_channel( 0 ) );
    
    return instance;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//       
core_frame_mgmt_ie_ap_channel_report_c::~core_frame_mgmt_ie_ap_channel_report_c()
    {
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u8_t core_frame_mgmt_ie_ap_channel_report_c::ap_channel_report_class() const
    {
    return data_m[CORE_FRAME_MGMT_IE_AP_CHANNEL_REPORT_CLASS_OFFSET];
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u8_t core_frame_mgmt_ie_ap_channel_report_c::ap_channel_report_channel( 
    u16_t channel_offset ) const
    {
    return data_m[CORE_FRAME_MGMT_IE_AP_CHANNEL_REPORT_CHANNEL_OFFSET + channel_offset];
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_mgmt_ie_ap_channel_report_c::core_frame_mgmt_ie_ap_channel_report_c(
    u16_t data_length,
    const u8_t* data,
    u16_t max_data_length ) :
    core_frame_dot11_ie_c( data_length, data, max_data_length )
    {
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_mgmt_ie_admission_capacity_c* core_frame_mgmt_ie_admission_capacity_c::instance(
    const core_frame_dot11_ie_c& ie )
    {
    if ( ie.data_length() < CORE_FRAME_MGMT_IE_ADMISSION_CAPACITY_LENGTH )
        {
        DEBUG( "core_frame_mgmt_ie_admission_capacity_c::instance() - not a valid IE, invalid length" );

        return NULL;
        }

    core_frame_mgmt_ie_admission_capacity_c* instance = new core_frame_mgmt_ie_admission_capacity_c(
        ie.data_length(),
        ie.data(),
        0 );
    if ( !instance )
        {
        DEBUG( "core_frame_mgmt_ie_admission_capacity_c::instance() - unable to create an instance" );

        return NULL;
        }

    return instance;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//       
core_frame_mgmt_ie_admission_capacity_c::~core_frame_mgmt_ie_admission_capacity_c()
    {
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u16_t core_frame_mgmt_ie_admission_capacity_c::admission_capacity_bitmask() const
    {
    return core_tools_c::get_u16(
        data_m,
        CORE_FRAME_MGMT_IE_ADMISSION_CAPACITY_BITMASK_OFFSET );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_frame_mgmt_ie_admission_capacity_c::get_admission_capacity(
    u16_t ie_data_length,
    u16_t* admission_capacity_table )
    {
    for( u16_t i=0; i < (ie_data_length - CORE_FRAME_MGMT_IE_ADMISSION_CAPACITY_OFFSET ); i=i+2 )
        {
        admission_capacity_table[i/2] = core_tools_c::get_u16(
                                                        data_m,
                                                        CORE_FRAME_MGMT_IE_ADMISSION_CAPACITY_OFFSET + i );
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_mgmt_ie_admission_capacity_c::core_frame_mgmt_ie_admission_capacity_c(
    u16_t data_length,
    const u8_t* data,
    u16_t max_data_length ) :
    core_frame_dot11_ie_c( data_length, data, max_data_length )
    {
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_mgmt_ie_power_capability_c* core_frame_mgmt_ie_power_capability_c::instance(
    const core_frame_dot11_ie_c& ie )
    {
    if ( ie.data_length() < CORE_FRAME_MGMT_IE_POWER_CAPABILITY_LENGTH )
        {
        DEBUG( "core_frame_mgmt_ie_power_capability_c::instance() - not a valid IE, invalid length" );

        return NULL;
        }

    core_frame_mgmt_ie_power_capability_c* instance = new core_frame_mgmt_ie_power_capability_c(
        ie.data_length(),
        ie.data(),
        0 );
    if ( !instance )
        {
        DEBUG( "core_frame_mgmt_ie_power_capability_c::instance() - unable to create an instance" );

        return NULL;
        }

    return instance;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//       
core_frame_mgmt_ie_power_capability_c::~core_frame_mgmt_ie_power_capability_c()
    {
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u8_t core_frame_mgmt_ie_power_capability_c::power_capability() const
    {
    return data_m[CORE_FRAME_MGMT_IE_POWER_CAPABILITY_OFFSET];
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_mgmt_ie_power_capability_c::core_frame_mgmt_ie_power_capability_c(
    u16_t data_length,
    const u8_t* data,
    u16_t max_data_length ) :
    core_frame_dot11_ie_c( data_length, data, max_data_length )
    {
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_radio_mgmt_ie_c* core_frame_radio_mgmt_ie_c::instance(
    u16_t max_data_length,
    u8_t min_capability,
    u8_t max_capability )
    {
    u8_t* buffer = new u8_t[max_data_length + CORE_FRAME_DOT11_IE_HEADER_LENGTH];

    if ( !buffer )
        {
        DEBUG( "core_frame_radio_mgmt_ie_c::instance() - unable create the internal buffer" );
        return NULL;
        }

    core_frame_radio_mgmt_ie_c* instance =
        new core_frame_radio_mgmt_ie_c( 0, buffer, max_data_length + CORE_FRAME_DOT11_IE_HEADER_LENGTH );
    if ( !instance )
        {
        DEBUG( "core_frame_radio_mgmt_ie_c::instance() - unable to create an instance" );
        delete[] buffer;
        
        return NULL;
        }

    instance->generate(
        min_capability,
        max_capability );

    return instance;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//       
core_frame_radio_mgmt_ie_c::~core_frame_radio_mgmt_ie_c()
    {
    DEBUG( "core_frame_radio_mgmt_ie_c::~core_frame_radio_mgmt_ie_c()" );   
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_frame_radio_mgmt_ie_c::generate(
    u8_t min_capability,
    u8_t max_capability )
    {
    ASSERT( !data_length_m );
    ASSERT( max_data_length_m );   

    core_frame_dot11_ie_c::generate( core_frame_dot11_ie_c::core_frame_dot11_ie_element_id_power_capability );

    // Minimum transmit power capability
    data_m[data_length_m++] = min_capability;

    // Maximum transmit power capability
    data_m[data_length_m++] = max_capability;
    
    set_length( data_length_m );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_radio_mgmt_ie_c::core_frame_radio_mgmt_ie_c(
    u16_t data_length,
    const u8_t* data,
    u16_t max_data_length ) :
    core_frame_dot11_ie_c( data_length, data, max_data_length )
    {
    DEBUG( "core_frame_radio_mgmt_ie_c::core_frame_radio_mgmt_ie_c()" );   
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_mgmt_ie_rrm_capabilities_c* core_frame_mgmt_ie_rrm_capabilities_c::instance(
    const core_frame_dot11_ie_c& ie )
    {
    if ( ie.data_length() < CORE_FRAME_MGMT_IE_RRM_CAPABILITIES_LENGTH )
        {
        DEBUG( "core_frame_mgmt_ie_rrm_capabilities_c::instance() - not a valid IE, invalid length" );

        return NULL;
        }

    core_frame_mgmt_ie_rrm_capabilities_c* instance = new core_frame_mgmt_ie_rrm_capabilities_c(
        ie.data_length(),
        ie.data(),
        0 );
    if ( !instance )
        {
        DEBUG( "core_frame_mgmt_ie_rrm_capabilities_c::instance() - unable to create an instance" );

        return NULL;
        }

    return instance;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//       
core_frame_mgmt_ie_rrm_capabilities_c::~core_frame_mgmt_ie_rrm_capabilities_c()
    {
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u64_t core_frame_mgmt_ie_rrm_capabilities_c::rrm_capabilities() const
    {
    return core_tools_c::get_u64(
        data_m,
        CORE_FRAME_MGMT_IE_RRM_CAPABILITIES_OFFSET );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_mgmt_ie_rrm_capabilities_c::core_frame_mgmt_ie_rrm_capabilities_c(
    u16_t data_length,
    const u8_t* data,
    u16_t max_data_length ) :
    core_frame_dot11_ie_c( data_length, data, max_data_length )
    {
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_rrm_mgmt_ie_c* core_frame_rrm_mgmt_ie_c::instance(
    const u8_t* rrm_capabilities )
    {
    u8_t* buffer = new u8_t[CORE_FRAME_RRM_MGMT_IE_LENGTH + CORE_FRAME_DOT11_IE_HEADER_LENGTH];
    if ( !buffer )
        {
        DEBUG( "core_frame_rrm_mgmt_ie_c::instance() - unable create the internal buffer" );
        return NULL;
        }

    core_frame_rrm_mgmt_ie_c* instance =
        new core_frame_rrm_mgmt_ie_c( 0, buffer, CORE_FRAME_RRM_MGMT_IE_LENGTH + CORE_FRAME_DOT11_IE_HEADER_LENGTH );
    if ( !instance )
        {
        DEBUG( "core_frame_rrm_mgmt_ie_c::instance() - unable to create an instance" );
        delete[] buffer;
        
        return NULL;
        }

    instance->generate( rrm_capabilities );

    return instance;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//       
core_frame_rrm_mgmt_ie_c::~core_frame_rrm_mgmt_ie_c()
    {
    DEBUG( "core_frame_rrm_mgmt_ie_c::~core_frame_rrm_mgmt_ie_c()" );   
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_frame_rrm_mgmt_ie_c::generate(
    const u8_t* rrm_capabilities )
    {
    ASSERT( !data_length_m );
    ASSERT( max_data_length_m );   

    core_frame_dot11_ie_c::generate( core_frame_dot11_ie_c::core_frame_dot11_ie_element_id_rrm_capabilities );

    // RRM capabilities
    core_tools_c::copy(
        &data_m[data_length_m],
        const_cast<u8_t*>(rrm_capabilities),
        CORE_FRAME_RRM_MGMT_IE_LENGTH );
    data_length_m += CORE_FRAME_RRM_MGMT_IE_LENGTH;
    
    set_length( data_length_m );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_rrm_mgmt_ie_c::core_frame_rrm_mgmt_ie_c(
    u16_t data_length,
    const u8_t* data,
    u16_t max_data_length ) :
    core_frame_dot11_ie_c( data_length, data, max_data_length )
    {
    DEBUG( "core_frame_rrm_mgmt_ie_c::core_frame_rrm_mgmt_ie_c()" );   
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_mgmt_ie_request_ie_c* core_frame_mgmt_ie_request_ie_c::instance(
    const core_frame_dot11_ie_c& ie )
    {
    if ( ie.data_length() > CORE_FRAME_MGMT_IE_REQUEST_IE_MAX_LENGTH )
        {
        DEBUG( "core_frame_mgmt_ie_request_ie_c::instance() - not a valid IE, too long" );

        return NULL;
        }

    core_frame_mgmt_ie_request_ie_c* instance = new core_frame_mgmt_ie_request_ie_c(
        ie.data_length(),
        ie.data(),
        0 );
    if ( !instance )
        {
        DEBUG( "core_frame_mgmt_ie_request_ie_c::instance() - unable to create an instance" );

        return NULL;
        }

    return instance;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//       
core_frame_mgmt_ie_request_ie_c::~core_frame_mgmt_ie_request_ie_c()
    {
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u8_t core_frame_mgmt_ie_request_ie_c::element_list( u8_t* ie_id_list ) const
    {
    u8_t ie_id_list_length = data_length_m - CORE_FRAME_DOT11_IE_HEADER_LENGTH;
    
    core_tools_c::copy(
        ie_id_list,
        const_cast<u8_t*>( &data_m[CORE_FRAME_MGMT_IE_REQUEST_IE_OFFSET] ),
        data_length_m );
    
    return ie_id_list_length;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u8_t core_frame_mgmt_ie_request_ie_c::element_data( 
    u16_t element_data_offset ) const
    {
    return data_m[CORE_FRAME_MGMT_IE_REQUEST_IE_OFFSET + element_data_offset];
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_mgmt_ie_request_ie_c::core_frame_mgmt_ie_request_ie_c(
    u16_t data_length,
    const u8_t* data,
    u16_t max_data_length ) :
    core_frame_dot11_ie_c( data_length, data, max_data_length )
    {
    }
