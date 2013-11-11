/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Simple utility functions for core
*
*/


#include "core_tools.h"
#include "core_frame_beacon.h"
#include "genscanoffsets.h"
#include "am_debug.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// core_tools_c::compare
// -----------------------------------------------------------------------------
//
int_t core_tools_c::compare(
    const u8_t* pl,
    int_t ll, 
    const u8_t* pr, 
    int_t rl )
    {
    if ( ll != rl )
        {
        return ll - rl;
        }        

    if ( pl == pr )
        {
        return 0;
        }        

    for ( int_t i( 0 ); i < ll; ++i )
        {
        if ( *(pl+i) != *(pr+i) )
            {
            return *(pl+i) - *(pr+i);
            }
        }

    return 0;
    }

// -----------------------------------------------------------------------------
// core_tools_c::copy
// -----------------------------------------------------------------------------
//
u8_t* core_tools_c::copy(
    void* trg,
    const void* src,
    int_t len )
    {
    if ( len <= 0 )
        {
        return reinterpret_cast<u8_t*>( trg );
        }

    if ( trg == src )
        {
        return reinterpret_cast<u8_t*>( trg ) + len;
        }

    // ensure areas do not overlap
    if ( trg > src )
        {
        ASSERT( reinterpret_cast<u8_t*>( trg ) >= ( reinterpret_cast<const u8_t*>( src ) + len ) );
        }
    else
        {
        ASSERT( reinterpret_cast<const u8_t*>( src ) >= ( reinterpret_cast<u8_t*>( trg ) + len ) );
        }

    for ( i32_t i( 0 ); i < len; ++i )
        {
        *( reinterpret_cast<u8_t*>( trg ) + i ) = *( reinterpret_cast<const u8_t*>( src ) + i );
        }

    return reinterpret_cast<u8_t*>( trg ) + len;
    }

// -----------------------------------------------------------------------------
// core_tools_c::fillz
// -----------------------------------------------------------------------------
//
void core_tools_c::fillz(
    void* trg,
    int_t len )
    {
    if ( len <= 0 )
        {
        return;
        }        

    for ( int_t i( 0 ); i < len; ++i )
        {
        *( reinterpret_cast<u8_t*>( trg ) + i ) = 0;
        }
    }

// -----------------------------------------------------------------------------
// core_tools_c::convert_host_to_big_endian
// -----------------------------------------------------------------------------
//
u16_t core_tools_c::convert_host_to_big_endian(
    u16_t src )
    {
    return( static_cast<u16_t>( ( src >> 8 ) | ( src << 8 ) ) );
    }

// -----------------------------------------------------------------------------
// core_tools_c::convert_host_to_big_endian
// -----------------------------------------------------------------------------
//           
u32_t core_tools_c::convert_host_to_big_endian(
    u32_t src )
    {
    return( static_cast<u32_t>( ( ( src & 0xFF ) << 24 ) | 
                                ( ( src & 0xFF00 ) << 8 ) |
                                ( ( src & 0xFF0000 ) >> 8 ) | 
                                ( ( src & 0xFF000000 ) >> 24 ) ) );      
    }

// -----------------------------------------------------------------------------
// core_tools_c::convert_big_endian_to_host
// -----------------------------------------------------------------------------
//
u16_t core_tools_c::convert_big_endian_to_host(
    u16_t src )
    {
    return( static_cast<u16_t>( ( src >> 8 ) | ( src << 8 ) ) ); 
    }

// -----------------------------------------------------------------------------
// core_tools_c::convert_big_endian_to_host
// -----------------------------------------------------------------------------
//
u32_t core_tools_c::convert_big_endian_to_host(
    u32_t src )
    {
    return( static_cast<u32_t>( ( ( src & 0xFF ) << 24 ) | 
                                ( ( src & 0xFF00 ) << 8 ) |
                                ( ( src & 0xFF0000 ) >> 8 ) | 
                                ( ( src & 0xFF000000 ) >> 24 ) ) );     
    }

// -----------------------------------------------------------------------------
// core_tools_c::get_u16_big_endian
// -----------------------------------------------------------------------------
//
u16_t core_tools_c::get_u16_big_endian(
    const u8_t* data,
    u16_t index )
    {
    return core_tools_c::convert_big_endian_to_host(
        get_u16( data, index ) );
    }

// -----------------------------------------------------------------------------
// core_tools_c::get_u16
// -----------------------------------------------------------------------------
//
u16_t core_tools_c::get_u16(
    const u8_t* data,
    u16_t index )
    {
    u16_t temp16( 0 );
    core_tools_c::copy( reinterpret_cast<u8_t*>( &temp16 ),
        data + index,
        sizeof( temp16 ) );
    
    return temp16;
    }

// -----------------------------------------------------------------------------
// core_tools_c::get_u32_big_endian
// -----------------------------------------------------------------------------
//
u32_t core_tools_c::get_u32_big_endian(
    const u8_t* data,
    u16_t index )
    {
    return core_tools_c::convert_big_endian_to_host(
        get_u32( data, index ) );
    }
            
// -----------------------------------------------------------------------------
// core_tools_c::get_u32
// -----------------------------------------------------------------------------
//
u32_t core_tools_c::get_u32(
    const u8_t* data,
    u16_t index )
    {
    u32_t temp32( 0 );
    core_tools_c::copy( reinterpret_cast<u8_t*>( &temp32 ),
        data + index,
        sizeof( temp32 ) );
    
    return temp32;       
    }

// -----------------------------------------------------------------------------
// core_tools_c::get_u64
// -----------------------------------------------------------------------------
//
u64_t core_tools_c::get_u64(
    const u8_t* data,
    u16_t index )
    {
    u64_t temp64( 0 );
    core_tools_c::copy( reinterpret_cast<u8_t*>( &temp64 ),
        data + index,
        sizeof( temp64 ) );
    
    return temp64;       
    }

// -----------------------------------------------------------------------------
// core_tools_c::insert_u16_big_endian
// -----------------------------------------------------------------------------
//
void core_tools_c::insert_u16_big_endian(
    u8_t* data,
    u16_t index,
    u16_t value )
    {
    u16_t temp16(
        convert_host_to_big_endian( value ) );

    core_tools_c::copy(
        &data[index],
        reinterpret_cast<u8_t*>( &temp16 ),
        sizeof( temp16 ) );
    }

// -----------------------------------------------------------------------------
// core_tools_c::insert_u16
// -----------------------------------------------------------------------------
//
void core_tools_c::insert_u16(
    u8_t* data,
    u16_t index,
    u16_t value )
    {
    u16_t temp16( value );

    core_tools_c::copy(
        &data[index],
        reinterpret_cast<u8_t*>( &temp16 ),
        sizeof( temp16 ) );    
    }

// -----------------------------------------------------------------------------
// core_tools_c::insert_u32_big_endian
// -----------------------------------------------------------------------------
//
void core_tools_c::insert_u32_big_endian(
    u8_t* data,
    u16_t index,
    u32_t value )
    {
    u32_t temp32(
        convert_host_to_big_endian( value ) );

    core_tools_c::copy(
        &data[index],
        reinterpret_cast<u8_t*>( &temp32 ),
        sizeof( temp32 ) );
    }

// -----------------------------------------------------------------------------
// core_tools_c::insert_u32
// -----------------------------------------------------------------------------
//
void core_tools_c::insert_u32(
    u8_t* data,
    u16_t index,
    u32_t value )
    {
    u32_t temp32( value );

    core_tools_c::copy(
        &data[index],
        reinterpret_cast<u8_t*>( &temp32 ),
        sizeof( temp32 ) );    
    }

// -----------------------------------------------------------------------------
// core_tools_c::append_u16_big_endian
// -----------------------------------------------------------------------------
//    
void core_tools_c::append_u16_big_endian(
    u8_t* data,
    u16_t& data_length,
    u16_t value )
    {
    insert_u16_big_endian(
        data,
        data_length,
        value );
    data_length += sizeof( value );
    }

// -----------------------------------------------------------------------------
// core_tools_c::append_u16
// -----------------------------------------------------------------------------
//
void core_tools_c::append_u16(
    u8_t* data,
    u16_t& data_length,
    u16_t value )
    {
    insert_u16(
        data,
        data_length,
        value );
    data_length += sizeof( value );    
    }

// -----------------------------------------------------------------------------
// core_tools_c::append_u32_big_endian
// -----------------------------------------------------------------------------
//    
void core_tools_c::append_u32_big_endian(
    u8_t* data,
    u16_t& data_length,
    u32_t value )
    {
    insert_u32_big_endian(
        data,
        data_length,
        value );
    data_length += sizeof( value );
    }

// -----------------------------------------------------------------------------
// core_tools_c::append_u32
// -----------------------------------------------------------------------------
//
void core_tools_c::append_u32(
    u8_t* data,
    u16_t& data_length,
    u32_t value )
    {
    insert_u32(
        data,
        data_length,
        value );
    data_length += sizeof( value );    
    }

// -----------------------------------------------------------------------------
// core_tools_c::cipher_key_type
// -----------------------------------------------------------------------------
//
core_cipher_key_type_e core_tools_c::cipher_key_type(
    core_cipher_suite_e cipher )
    {
    switch( cipher )
        {
        case core_cipher_suite_wep40:
            /** Falls through on purpose. */
        case core_cipher_suite_wep104:
            return core_cipher_key_type_wep;
        case core_cipher_suite_tkip:
            return core_cipher_key_type_tkip;
        case core_cipher_suite_ccmp:
            return core_cipher_key_type_ccmp;
        case core_cipher_suite_wpi:
            return core_cipher_key_type_wpi;
        default:
            return core_cipher_key_type_none;            
        }
    }

// -----------------------------------------------------------------------------
// core_tools_c::cipher_key_type
// -----------------------------------------------------------------------------
//
core_cipher_key_type_e core_tools_c::cipher_key_type(
    wlan_eapol_if_eapol_key_type_e type,
    core_cipher_suite_e pairwise_cipher,
    core_cipher_suite_e group_cipher )
    {
    core_cipher_key_type_e return_type( core_cipher_key_type_ccmp );
    
    switch( type )
        {
        case wlan_eapol_if_eapol_key_type_unicast:
            {
            if( pairwise_cipher == core_cipher_suite_ccmp )
                {
                DEBUG( "core_tools_c::cipher_key_type() - pairwise CCMP key" );
                return_type = core_cipher_key_type_ccmp;
                }
            else if( pairwise_cipher == core_cipher_suite_tkip )
                {
                DEBUG( "core_tools_c::cipher_key_type() - pairwise TKIP key" );
                return_type = core_cipher_key_type_tkip;
                }
            else if( pairwise_cipher == core_cipher_suite_wpi )
                {
                DEBUG( "core_tools_c::cipher_key_type() - pairwise WPI key" );
                return_type = core_cipher_key_type_wpi;                
                }
            else
                {
                DEBUG( "core_tools_c::cipher_key_type() - pairwise WEP key" );
                return_type = core_cipher_key_type_wep;
                }
            break;            
            }           
        case wlan_eapol_if_eapol_key_type_broadcast:
            {            
            if( group_cipher == core_cipher_suite_ccmp )
                {
                DEBUG( "core_tools_c::cipher_key_type() - group CCMP key" );
                return_type = core_cipher_key_type_ccmp;
                }
            else if( group_cipher == core_cipher_suite_tkip )
                {
                DEBUG( "core_tools_c::cipher_key_type() - group TKIP key" );
                return_type = core_cipher_key_type_tkip;
                }
            else if( pairwise_cipher == core_cipher_suite_wpi )
                {
                DEBUG( "core_tools_c::cipher_key_type() - group WPI key" );
                return_type = core_cipher_key_type_wpi;                
                }
            else
                {
                DEBUG( "core_tools_c::cipher_key_type() - group WEP key" );
                return_type = core_cipher_key_type_wep;
                }
            break;
            }            
        case wlan_eapol_if_eapol_key_type_pmkid:
            {
            DEBUG( "core_tools_c::cipher_key_type() - PMKID" );
            ASSERT( false_t );
            break;
            }
        default:
            {
            DEBUG1( "core_tools_c::cipher_key_type() - unknown EAPOL key type %u",
                type );
            ASSERT( false_t );
            }
        }

    return return_type;
    }

// -----------------------------------------------------------------------------
// core_tools_c::cipher_key_type
// -----------------------------------------------------------------------------
//
wlan_eapol_if_eapol_key_authentication_type_e core_tools_c::eap_authentication_type(
    const core_iap_data_c& iap_data,
    const core_ap_data_c& ap_data )
    {
    core_security_mode_e mode =
        iap_data.security_mode();
    bool_t is_psk_required =
        iap_data.is_psk_used();
    
    if( mode == core_security_mode_802dot1x &&
         !ap_data.is_rsn_ie_present() &&
         !ap_data.is_wpa_ie_present() )
        {
        DEBUG( "core_tools_c::eap_authentication_type() - wlan_eapol_if_eapol_key_authentication_type_802_1x" );
        return wlan_eapol_if_eapol_key_authentication_type_802_1x;
        }
    else if( (mode == core_security_mode_802dot1x || core_security_mode_802dot1x_unencrypted) &&
            (ap_data.key_management_suites() & core_key_management_wpx_fast_roam) )
        {
        DEBUG( "core_tools_c::eap_authentication_type() - wlan_eapol_if_eapol_key_authentication_type_wpx_fast_roam" );
        return wlan_eapol_if_eapol_key_authentication_type_wpx_fast_roam;
        }
    if( mode == core_security_mode_protected_setup )
        {
        DEBUG( "core_tools_c::eap_authentication_type() - wlan_eapol_if_eapol_key_authentication_type_wfa_sc" );
        return wlan_eapol_if_eapol_key_authentication_type_wfa_sc;
        }
    if( mode == core_security_mode_802dot1x_unencrypted &&
         !ap_data.is_rsn_ie_present() &&
         !ap_data.is_wpa_ie_present() )
        {
        if( !ap_data.is_privacy_enabled() )
            {
            DEBUG( "core_tools_c::eap_authentication_type() - wlan_eapol_if_eapol_key_authentication_type_802_1x_unencrypted" );
            return wlan_eapol_if_eapol_key_authentication_type_802_1x_unencrypted;
            }
        else
            {
            DEBUG( "core_tools_c::eap_authentication_type() - wlan_eapol_if_eapol_key_authentication_type_802_1x" );
            return wlan_eapol_if_eapol_key_authentication_type_802_1x;                
            }
        }
    else if( iap_data.is_eap_used() )
        {
        if( ap_data.is_rsn_ie_present() )
            {
            if( !is_psk_required &&
                ap_data.key_management_suites() & core_key_management_eap )
                {
                DEBUG( "core_tools_c::eap_authentication_type() - wlan_eapol_if_eapol_key_authentication_type_rsna_eap" );
                return wlan_eapol_if_eapol_key_authentication_type_rsna_eap;
                }
            else if( is_psk_required &&
                     ap_data.key_management_suites() & core_key_management_preshared )
                {
                DEBUG( "core_tools_c::eap_authentication_type() - wlan_eapol_if_eapol_key_authentication_type_rsna_psk" );
                return wlan_eapol_if_eapol_key_authentication_type_rsna_psk;
                }
            }
        else if( ap_data.is_wpa_ie_present() )
            {
            if( !is_psk_required &&
                ap_data.key_management_suites() & core_key_management_eap )
                {
                DEBUG( "core_tools_c::eap_authentication_type() - wlan_eapol_if_eapol_key_authentication_type_wpa_eap" );
                return wlan_eapol_if_eapol_key_authentication_type_wpa_eap;
                }
            else if( is_psk_required &&
                     ap_data.key_management_suites() & core_key_management_preshared )
                {
                DEBUG( "core_tools_c::eap_authentication_type() - wlan_eapol_if_eapol_key_authentication_type_wpa_psk" );
                return wlan_eapol_if_eapol_key_authentication_type_wpa_psk;
                }
            }
        }
    else if( iap_data.is_wapi_used() )
        {
        if( !is_psk_required &&
            ap_data.key_management_suites() & core_key_management_wapi_certificate )
            {
            DEBUG( "core_tools_c::eap_authentication_type() - wlan_eapol_if_eapol_key_authentication_type_wapi" );
            return wlan_eapol_if_eapol_key_authentication_type_wapi;
            }
        else if( is_psk_required &&
                 ap_data.key_management_suites() & core_key_management_wapi_psk )
            {
            DEBUG( "core_tools_c::eap_authentication_type() - wlan_eapol_if_eapol_key_authentication_type_wapi_psk" );
            return wlan_eapol_if_eapol_key_authentication_type_wapi_psk;
            }
        }

    DEBUG( "core_tools_c::eap_authentication_type() - unable to select security" );

    return wlan_eapol_if_eapol_key_authentication_type_none;   
    }

// -----------------------------------------------------------------------------
// core_tools_c::eapol_cipher
// -----------------------------------------------------------------------------
//
wlan_eapol_if_rsna_cipher_e core_tools_c::eapol_cipher(
    core_cipher_suite_e cipher )
    {
    switch( cipher )
        {
        case core_cipher_suite_wep40:
            return wlan_eapol_if_rsna_cipher_wep_40;
        case core_cipher_suite_wep104:
            return wlan_eapol_if_rsna_cipher_wep_104;
        case core_cipher_suite_tkip:
            return wlan_eapol_if_rsna_cipher_tkip;
        case core_cipher_suite_ccmp:
            return wlan_eapol_if_rsna_cipher_ccmp;
        case core_cipher_suite_wpi:
            return wlan_eapol_if_wapi_cipher_wpi;
        default:
            return wlan_eapol_if_rsna_cipher_none;
        }
    }

// -----------------------------------------------------------------------------
// core_tools_c::add_beacon_to_scan_list
// -----------------------------------------------------------------------------
//
void core_tools_c::add_beacon_to_scan_list(
    ScanList& scan_list,
    const core_ap_data_c& ap_data,
    u32_t rcpi )
    {
    const u32_t frame_length = ap_data.frame()->data_length();

    u8_t* buffer = new u8_t[frame_length + DOT11_BASE_OFFSET];
    if ( !buffer )
        {
        DEBUG( "core_tools_c::add_beacon_to_scan_list() - unable to create a frame buffer" );

        return;
        }                       

    core_tools_c::fillz(
        buffer,
        frame_length );

    /**
     * Set the RCPI value.
     */
    core_tools_c::insert_u32(
        buffer,
        CNTRL_RX_LEVEL_OFFSET,
        rcpi );

    /**
     * Set the frame length.
     */                         
    core_tools_c::insert_u32(
        buffer,
        CNTRL_LENGTH_OFFSET,
        frame_length );

    /**
     * Copy the actual frame data.
     */            
    core_tools_c::copy(
        &buffer[DOT11_BASE_OFFSET],
        ap_data.frame()->data(),
        frame_length );
    
    u32_t list_length = scan_list.Append(
        frame_length + DOT11_BASE_OFFSET,
        reinterpret_cast<ScanFrame*>( buffer ) );
    if ( list_length == APPEND_FAILED_NO_MEMORY )
        {
        DEBUG( "core_tools_c::add_beacon_to_scan_list() - unable to append to the scan list" );
        }
    else
        {
        DEBUG( "core_tools_c::add_beacon_to_scan_list() - BSSID added to the list" );
        DEBUG1( "core_tools_c::add_beacon_to_scan_list() - entry length: %u",
            frame_length + DOT11_BASE_OFFSET );
        DEBUG1( "core_tools_c::add_beacon_to_scan_list() - list size after append: %u",
            list_length );
        }

    delete[] buffer;
    buffer = NULL;
    }

// -----------------------------------------------------------------------------
// core_tools_c::convert_user_priority_to_ac
// -----------------------------------------------------------------------------
//
core_access_class_e core_tools_c::convert_user_priority_to_ac(
    u8_t user_priority )
    {
    /**
     * Mapping of 802.1D Priority to Access Class from WMM specification.
     */    
    const u8_t mapping_table[MAX_QOS_USER_PRIORITY] =
        {
        core_access_class_best_effort,
        core_access_class_background,
        core_access_class_background,
        core_access_class_best_effort,
        core_access_class_video,
        core_access_class_video,
        core_access_class_voice,
        core_access_class_voice
        };

    if ( user_priority >= MAX_QOS_USER_PRIORITY )
        {
        return core_access_class_best_effort;
        }

    return static_cast<core_access_class_e>( mapping_table[user_priority] );
    }

// -----------------------------------------------------------------------------
// core_tools_c::convert_ac_to_user_priority
// -----------------------------------------------------------------------------
//
u8_t core_tools_c::convert_ac_to_user_priority(
    core_access_class_e access_class )
    {
    /**
     * Based on 802.1D mapping from WMM specification.
     */
    const u8_t mapping_table[MAX_QOS_ACCESS_CLASS] =
        {
        3, // core_access_class_best_effort
        2, // core_access_class_background
        5, // core_access_class_video
        7, // core_access_class_voice
        };

    return mapping_table[access_class];
    }

// -----------------------------------------------------------------------------
// core_tools_c::convert_tx_rate_to_tx_rate_enum
// -----------------------------------------------------------------------------
//
core_tx_rate_e core_tools_c::convert_tx_rate_to_tx_rate_enum(
    u8_t tx_rate )
    {
    switch( tx_rate )
        {
        case core_tx_rate_value_1mbit:
            return core_tx_rate_1mbit;
        case core_tx_rate_value_2mbit:
            return core_tx_rate_2mbit;
        case core_tx_rate_value_5p5mbit:
            return core_tx_rate_5p5mbit;
        case core_tx_rate_value_6mbit:
            return core_tx_rate_6mbit;
        case core_tx_rate_value_9mbit:
            return core_tx_rate_9mbit;
        case core_tx_rate_value_11mbit:
            return core_tx_rate_11mbit;
        case core_tx_rate_value_12mbit:
            return core_tx_rate_12mbit;
        case core_tx_rate_value_18mbit:
            return core_tx_rate_18mbit;
        case core_tx_rate_value_22mbit:
            return core_tx_rate_22mbit;
        case core_tx_rate_value_24mbit:
            return core_tx_rate_24mbit;
        case core_tx_rate_value_33mbit:
            return core_tx_rate_33mbit;
        case core_tx_rate_value_36mbit:
            return core_tx_rate_36mbit;
        case core_tx_rate_value_48mbit:
            return core_tx_rate_48mbit;
        case core_tx_rate_value_54mbit:
            return core_tx_rate_54mbit;
        default:
            return core_tx_rate_none;
        }
    }

// -----------------------------------------------------------------------------
// core_tools_c::convert_tx_rate_enum_to_tx_rate
// -----------------------------------------------------------------------------
//            
u8_t core_tools_c::convert_tx_rate_enum_to_tx_rate(
    core_tx_rate_e tx_rate )
    {
    switch( tx_rate )
        {
        case core_tx_rate_1mbit:
            return core_tx_rate_value_1mbit;
        case core_tx_rate_2mbit:
            return core_tx_rate_value_2mbit;
        case core_tx_rate_5p5mbit:
            return core_tx_rate_value_5p5mbit;
        case core_tx_rate_6mbit:
            return core_tx_rate_value_6mbit;
        case core_tx_rate_9mbit:
            return core_tx_rate_value_9mbit;
        case core_tx_rate_11mbit:
            return core_tx_rate_value_11mbit;
        case core_tx_rate_12mbit:
            return core_tx_rate_value_12mbit;
        case core_tx_rate_18mbit:
            return core_tx_rate_value_18mbit;
        case core_tx_rate_22mbit:
            return core_tx_rate_value_22mbit;
        case core_tx_rate_24mbit:
            return core_tx_rate_value_24mbit;
        case core_tx_rate_33mbit:
            return core_tx_rate_value_33mbit;
        case core_tx_rate_36mbit:
            return core_tx_rate_value_36mbit;
        case core_tx_rate_48mbit:
            return core_tx_rate_value_48mbit;
        case core_tx_rate_54mbit:
            return core_tx_rate_value_54mbit;
        default:
            return core_tx_rate_value_none;
        }    
    }

// -----------------------------------------------------------------------------
// core_tools_c::highest_tx_rate
// -----------------------------------------------------------------------------
//
core_tx_rate_e core_tools_c::highest_tx_rate(
    u32_t tx_rates )
    {
    if ( tx_rates & core_tx_rate_54mbit )
        {
        return core_tx_rate_54mbit;
        }
    else if ( tx_rates & core_tx_rate_48mbit )
        {
        return core_tx_rate_48mbit;
        }
    else if ( tx_rates & core_tx_rate_36mbit )
        {
        return core_tx_rate_36mbit;
        }
    else if ( tx_rates & core_tx_rate_33mbit )
        {
        return core_tx_rate_33mbit;
        }
    else if ( tx_rates & core_tx_rate_24mbit )
        {
        return core_tx_rate_24mbit;
        }
    else if ( tx_rates & core_tx_rate_22mbit )
        {
        return core_tx_rate_22mbit;
        }
    else if ( tx_rates & core_tx_rate_18mbit )
        {
        return core_tx_rate_18mbit;
        }
    else if ( tx_rates & core_tx_rate_12mbit )
        {
        return core_tx_rate_12mbit;
        }
    else if ( tx_rates & core_tx_rate_11mbit )
        {
        return core_tx_rate_11mbit;
        }
    else if ( tx_rates & core_tx_rate_9mbit )
        {
        return core_tx_rate_9mbit;
        }
    else if ( tx_rates & core_tx_rate_6mbit )
        {
        return core_tx_rate_6mbit;
        }
    else if ( tx_rates & core_tx_rate_5p5mbit )
        {
        return core_tx_rate_5p5mbit;
        }                                                
    else if ( tx_rates & core_tx_rate_2mbit )
        {
        return core_tx_rate_2mbit;
        }                                                
    else if ( tx_rates & core_tx_rate_1mbit )
        {
        return core_tx_rate_1mbit;
        }                                                

    return core_tx_rate_none;
    }

// -----------------------------------------------------------------------------
// core_tools_c::convert_tx_rates_to_tx_policy
// -----------------------------------------------------------------------------
// 
core_tx_rate_policy_s core_tools_c::convert_tx_rates_to_tx_policy(
    u32_t tx_rates )
    {
    const u8_t RETRIES = 1;
    
    core_tx_rate_policy_s policy;
    core_tools_c::fillz(
        &policy,
        sizeof( policy ) );

    if ( tx_rates & core_tx_rate_54mbit )
        {
        policy.tx_policy_54 = RETRIES;
        }
    if( tx_rates & core_tx_rate_48mbit )
        {
        policy.tx_policy_48 = RETRIES;
        }
    if( tx_rates & core_tx_rate_36mbit )
        {
        policy.tx_policy_36 = RETRIES;
        }
    if( tx_rates & core_tx_rate_33mbit )
        {
        policy.tx_policy_33 = RETRIES;
        }
    if( tx_rates & core_tx_rate_24mbit )
        {
        policy.tx_policy_24 = RETRIES;
        }
    if( tx_rates & core_tx_rate_22mbit )
        {
        policy.tx_policy_22 = RETRIES;
        }
    if( tx_rates & core_tx_rate_18mbit )
        {
        policy.tx_policy_18 = RETRIES;
        }
    if( tx_rates & core_tx_rate_12mbit )
        {
        policy.tx_policy_12 = RETRIES;
        }
    if( tx_rates & core_tx_rate_11mbit )
        {
        policy.tx_policy_11 = RETRIES;
        }
    if( tx_rates & core_tx_rate_9mbit )
        {
        policy.tx_policy_9 = RETRIES;
        }
    if( tx_rates & core_tx_rate_6mbit )
        {
        policy.tx_policy_6 = RETRIES;
        }
    if( tx_rates & core_tx_rate_5p5mbit )
        {
        policy.tx_policy_5p5 = RETRIES;
        }
    if( tx_rates & core_tx_rate_2mbit )
        {
        policy.tx_policy_2 = RETRIES;
        }
    if( tx_rates & core_tx_rate_1mbit )
        {
        policy.tx_policy_1 = RETRIES;
        }

    /**
     * short_retry_limit, long_retry_limit, flags and initial_tx_rate
     * will be filled later.
     */
    
    return policy;
    }

// -----------------------------------------------------------------------------
// core_tools_c::security_mode
// -----------------------------------------------------------------------------
//
core_connection_security_mode_e core_tools_c::security_mode(
    const core_iap_data_c& iap_data,
    const core_ap_data_c& ap_data )
    {    
    // Check for open connection
    // (open security map 1:1 with iap's security mode)
    if( iap_data.security_mode() == core_security_mode_allow_unsecure )
        {
        DEBUG( "core_tools_c::security_mode() - core_connection_security_mode_open" );
        return core_connection_security_mode_open;
        }
    
    // Check for wep connection
    if( iap_data.security_mode() == core_security_mode_wep )
        {
        DEBUG( "core_tools_c::security_mode() - core_security_mode_wep" );
        if ( iap_data.authentication_mode() == core_authentication_mode_shared )
            {
            return core_connection_security_mode_wep_shared;
            }
        return core_connection_security_mode_wep_open;
        }

    // Check for WPX fast-roam
    if( ( iap_data.security_mode() == core_security_mode_802dot1x ) &&
    	( ap_data.key_management_suites() & core_key_management_wpx_fast_roam ) )
        {
        DEBUG( "core_tools_c::security_mode() - core_connection_security_mode_802d1x" );
        return core_connection_security_mode_802d1x;
        }

    // Check for wpa/wpa2
    if( ap_data.key_management_suites() & core_key_management_eap )
        {
        if( ap_data.is_rsn_ie_present() &&
            ap_data.group_cipher() == core_cipher_suite_ccmp &&
            ap_data.pairwise_ciphers() == core_cipher_suite_ccmp )
            {
            DEBUG( "core_tools_c::security_mode() - core_connection_security_mode_wpa2" );
            return core_connection_security_mode_wpa2;
            }
        else
            {
            DEBUG( "core_tools_c::security_mode() - core_connection_security_mode_wpa" );
            return core_connection_security_mode_wpa;
            }
        }
        
    // Check for wpa_psk/wpa2_psk
    if( ap_data.key_management_suites() & core_key_management_preshared )
        {
        if( ap_data.is_rsn_ie_present() &&
            ap_data.group_cipher() == core_cipher_suite_ccmp &&
            ap_data.pairwise_ciphers() == core_cipher_suite_ccmp )
            {
            DEBUG( "core_tools_c::security_mode() - core_connection_security_mode_wpa2_psk" );
            return core_connection_security_mode_wpa2_psk;
            }
        else
            {
            DEBUG( "core_tools_c::security_mode() - core_connection_security_mode_wpa_psk" );
            return core_connection_security_mode_wpa_psk;
            }
        }
    
    if( ap_data.key_management_suites() & core_key_management_wapi_certificate )
        {
        DEBUG( "core_tools_c::security_mode() - core_connection_security_mode_wapi" );
        return core_connection_security_mode_wapi;        
        }

    if( ap_data.key_management_suites() & core_key_management_wapi_psk )
        {
        DEBUG( "core_tools_c::security_mode() - core_key_management_wapi_psk" );
        return core_connection_security_mode_wapi_psk;        
        }

    // 802.1x is the only one left
    DEBUG( "core_tools_c::security_mode() - core_connection_security_mode_802d1x" );
    return core_connection_security_mode_802d1x;
    }

// -----------------------------------------------------------------------------
// core_tools_c::convert_eapol_error_to_protected_setup_status
// -----------------------------------------------------------------------------
//
core_protected_setup_status_e  core_tools_c::convert_eapol_error_to_protected_setup_status(
        const wlan_eapol_if_eap_status_e error )
    {
    core_protected_setup_status_e status( core_protected_setup_status_undefined );
    switch ( error )
        {
        case wlan_eapol_if_eap_status_OOB_interface_read_error:
            status = core_protected_setup_status_OOB_interface_read_error;
            break;
        case wlan_eapol_if_eap_status_decryption_CRC_failure:
            status = core_protected_setup_status_decryption_CRC_failure;
            break;
        case wlan_eapol_if_eap_status_RF_band_2_4_ghz_not_supported:
            status = core_protected_setup_status_RF_band_2_4_ghz_not_supported;
            break;
        case wlan_eapol_if_eap_status_RF_band_5_0_ghz_not_supported:
            status = core_protected_setup_status_RF_band_5_0_ghz_not_supported;
            break;
        case wlan_eapol_if_eap_status_signal_too_weak:
            status = core_protected_setup_status_signal_too_weak;
            break;
        case wlan_eapol_if_eap_status_authentication_failure: // Falls through on purpose.
        case wlan_eapol_if_eap_status_network_authentication_failure:
            status = core_protected_setup_status_network_auth_failure;
            break;
        case wlan_eapol_if_eap_status_network_association_failure:
            status = core_protected_setup_status_network_assoc_failure;
            break;
        case wlan_eapol_if_eap_status_no_DHCP_response:
            status = core_protected_setup_status_no_DHCP_response;
            break;
        case wlan_eapol_if_eap_status_failed_DHCP_configure:
            status = core_protected_setup_status_failed_DHCP_configure;
            break;
        case wlan_eapol_if_eap_status_ip_address_conflict:
            status = core_protected_setup_status_ip_address_conflict;
            break;
        case wlan_eapol_if_eap_status_could_not_connect_to_registrar:
            status = core_protected_setup_status_could_not_connect_to_registrar;
            break;
        case wlan_eapol_if_eap_status_multiple_PBC_sessions_detected:
            status = core_protected_setup_status_multiple_PBC_sessions_detected;
            break;
        case wlan_eapol_if_eap_status_rogue_activity_suspected:
            status = core_protected_setup_status_rogue_activity_suspected;
            break;
        case wlan_eapol_if_eap_status_device_busy:
            status = core_protected_setup_status_device_busy;
            break;
        case wlan_eapol_if_eap_status_setup_locked:
            status = core_protected_setup_status_setup_locked;
            break;
        case wlan_eapol_if_eap_status_message_timeout:
            status = core_protected_setup_status_message_timeout;
            break;
        case wlan_eapol_if_eap_status_registration_session_timeout:
            status = core_protected_setup_status_registration_session_timeout;
            break;
        case wlan_eapol_if_eap_status_device_password_authentication_failure:
            status = core_protected_setup_status_device_password_authentication_failure;
            break;
        case wlan_eapol_if_eap_status_pin_code_authentication_not_supported:
            status = core_protected_setup_status_pin_code_authentication_not_supported;
            break;
        case wlan_eapol_if_eap_status_push_button_authentication_not_supported:
            status = core_protected_setup_status_push_button_authentication_not_supported;
            break;
        default:
            status = core_protected_setup_status_undefined;
            break;  
        }
    
    return status;
    }

// -----------------------------------------------------------------------------
// core_tools_c::convert_country_to_region
// -----------------------------------------------------------------------------
//
core_wlan_region_e core_tools_c::convert_country_to_region(
    const core_country_string_s& found_country )
    {    
    bool_t match( true_t );
    for ( u8_t i(0); i < country_info_table_length; i++ )
        {
        match = true_t;
        for ( u8_t j( 0 ); j < MAX_COUNTRY_STRING_LENGTH-1; ++j )
            {
            if ( *( country_info_table[i].country+j ) != *( found_country.country+j ) )
                {
                match = false_t;
                break;
                }
            }
        if( match )
        	{
        	return core_wlan_region_fcc;
        	}
        }
    return core_wlan_region_etsi;
    }

