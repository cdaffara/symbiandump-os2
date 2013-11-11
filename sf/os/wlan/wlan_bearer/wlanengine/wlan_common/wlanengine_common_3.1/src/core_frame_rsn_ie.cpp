/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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


#include "core_frame_rsn_ie.h"
#include "core_tools.h"
#include "core_tools_parser.h"
#include "am_debug.h"

const u8_t CORE_FRAME_RSN_IE_MIN_LENGTH = 22;
const u8_t CORE_FRAME_RSN_IE_PMKID_COUNT_SIZE = 2;
const u8_t CORE_FRAME_RSN_IE_GROUP_SUITE_OFFSET = 4;
const u8_t CORE_FRAME_RSN_IE_PAIRWISE_SUITE_COUNT_OFFSET = 8;
const u8_t CORE_FRAME_RSN_IE_PAIRWISE_SUITE_OFFSET = 10;
const u8_t CORE_FRAME_RSN_IE_CIPHER_OUI_LENGTH = 4;
const u8_t CORE_FRAME_RSN_IE_KEY_MANAGEMENT_COUNT_SIZE = 2;
const u8_t CORE_FRAME_RSN_IE_KEY_MANAGEMENT_OUI_LENGTH = 4;

/** IDs for different cipher suites in RSN IE. */
const u8_t CORE_FRAME_RSN_IE_CIPHER_ID_NONE    = 0x00;
const u8_t CORE_FRAME_RSN_IE_CIPHER_ID_WEP40   = 0x01;
const u8_t CORE_FRAME_RSN_IE_CIPHER_ID_WEP104  = 0x05;
const u8_t CORE_FRAME_RSN_IE_CIPHER_ID_TKIP    = 0x02;
const u8_t CORE_FRAME_RSN_IE_CIPHER_ID_CCMP    = 0x04;

/** IDs for different key management suites. */
const u8_t CORE_FRAME_RSN_IE_KEY_MANAGEMENT_ID_NONE = 0x00;
const u8_t CORE_FRAME_RSN_IE_KEY_MANAGEMENT_ID_EAP  = 0x01;
const u8_t CORE_FRAME_RSN_IE_KEY_MANAGEMENT_ID_PSK  = 0x02;

const u8_t CORE_FRAME_RSN_IE_OUI_PREFIX_LENGTH = 3;
const u8_t CORE_FRAME_RSN_IE_OUI_PREFIX[] = { 0x00, 0x0F, 0xAC };

const u16_t CORE_FRAME_RSN_IE_VERSION = 1;
const u16_t CORE_FRAME_RSN_IE_PAIRWISE_SUITE_COUNT = 1;
const u16_t CORE_FRAME_RSN_IE_KEY_MANAGEMENT_COUNT = 1;
const u16_t CORE_FRAME_RSN_IE_CAPABILITIES = 0;
const u16_t CORE_FRAME_RSN_IE_PMKID_COUNT = 1;
const u16_t CORE_FRAME_RSN_IE_PMKID_LENGTH = 16;

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_rsn_ie_c* core_frame_rsn_ie_c::instance(
    abs_core_wpx_adaptation_c& wpx_adaptation,
    const core_frame_dot11_ie_c& ie )
    {
    if ( ie.data_length() < CORE_FRAME_RSN_IE_MIN_LENGTH )
        {
        DEBUG( "core_frame_rsn_ie_c::instance() - not a valid IE, too short" );

        return NULL;
        }

    core_frame_rsn_ie_c* instance = new core_frame_rsn_ie_c(
        wpx_adaptation,
        ie.data_length(),
        ie.data(),
        0 );
    if ( !instance )
        {
        DEBUG( "core_frame_rsn_ie_c::instance() - unable to create an instance" );

        return NULL;
        }
    
    return instance;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_rsn_ie_c* core_frame_rsn_ie_c::instance(
    abs_core_wpx_adaptation_c& wpx_adaptation,
    core_cipher_suite_e group_cipher,
    core_cipher_suite_e pairwise_cipher,
    core_key_management_e key_management,
    u16_t pmkid_length,
    const u8_t* pmkid_data )
    {
    const u8_t max_length = CORE_FRAME_RSN_IE_MIN_LENGTH + CORE_FRAME_RSN_IE_PMKID_COUNT_SIZE + pmkid_length;
    
    u8_t* buffer = new u8_t[max_length];
    if ( !buffer )
        {
        DEBUG( "core_frame_rsn_ie_c::instance() - unable create the internal buffer" );
        return NULL;
        }

    core_frame_rsn_ie_c* instance =
        new core_frame_rsn_ie_c( wpx_adaptation, 0, buffer, max_length );
    if ( !instance )
        {
        DEBUG( "core_frame_rsn_ie_c::instance() - unable to create an instance" );
        delete[] buffer;

        return NULL;
        }

    instance->generate(
        group_cipher,
        pairwise_cipher,
        key_management,
        pmkid_length,
        pmkid_data );

    return instance;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//       
core_frame_rsn_ie_c::~core_frame_rsn_ie_c()
    {
    DEBUG( "core_frame_rsn_ie_c::~core_frame_rsn_ie_c()" );   
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u8_t core_frame_rsn_ie_c::group_cipher_suite() const
    {
    return cipher_oui_to_enum( data_m + CORE_FRAME_RSN_IE_GROUP_SUITE_OFFSET );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u8_t core_frame_rsn_ie_c::pairwise_cipher_suites() const
    {
    u16_t num_of_suites = core_tools_c::get_u16(
        data_m, CORE_FRAME_RSN_IE_PAIRWISE_SUITE_COUNT_OFFSET );    
    u8_t suites( 0 );

    DEBUG1( "core_frame_rsn_ie_c::pairwise_cipher_suites() - num_of_suites = %u",
        num_of_suites );

    u32_t offset_end(
        CORE_FRAME_RSN_IE_PAIRWISE_SUITE_OFFSET +
        ( num_of_suites * CORE_FRAME_RSN_IE_CIPHER_OUI_LENGTH ) );
    if ( offset_end > data_length() )
        {
        DEBUG( "core_frame_rsn_ie_c::pairwise_cipher_suites() - num_of_suites points past the end of IE, ignoring" );

        return suites;
        }

    while ( num_of_suites-- )
        {
        suites += cipher_oui_to_enum( data_m + CORE_FRAME_RSN_IE_PAIRWISE_SUITE_OFFSET +
            ( num_of_suites * CORE_FRAME_RSN_IE_CIPHER_OUI_LENGTH ) );
        }

    return suites;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u8_t core_frame_rsn_ie_c::key_management_suites() const
    {
    u8_t suites( 0 );
    u16_t pairwise_num_of_suites = core_tools_c::get_u16(
        data_m, CORE_FRAME_RSN_IE_PAIRWISE_SUITE_COUNT_OFFSET );
    u32_t pairwise_offset_end(
        CORE_FRAME_RSN_IE_PAIRWISE_SUITE_COUNT_OFFSET +
        ( pairwise_num_of_suites * CORE_FRAME_RSN_IE_CIPHER_OUI_LENGTH ) );
    if ( pairwise_offset_end > data_length() )
        {
        DEBUG( "core_frame_rsn_ie_c::key_management_suites() - pairwise_num_of_suites points past the end of IE, ignoring" );

        return suites;
        }

    u8_t pairwise_length( pairwise_num_of_suites * CORE_FRAME_RSN_IE_CIPHER_OUI_LENGTH );
    u16_t num_of_suites = core_tools_c::get_u16(
        data_m, CORE_FRAME_RSN_IE_PAIRWISE_SUITE_OFFSET + pairwise_length );
    u32_t offset_end(
        CORE_FRAME_RSN_IE_PAIRWISE_SUITE_OFFSET +
        pairwise_length +
        CORE_FRAME_RSN_IE_KEY_MANAGEMENT_COUNT_SIZE +
        ( num_of_suites * CORE_FRAME_RSN_IE_KEY_MANAGEMENT_OUI_LENGTH ) );
    if ( offset_end > data_length() )
        {
        DEBUG( "core_frame_rsn_ie_c::key_management_suites() - num_of_suites points past the end of IE, ignoring" );

        return suites;
        }

    DEBUG1( "core_frame_rsn_ie_c::key_management_suites() - num_of_suites = %u",
        num_of_suites );
        
    while ( num_of_suites-- )
        {
        suites += key_management_oui_to_enum(
            data_m + CORE_FRAME_RSN_IE_PAIRWISE_SUITE_OFFSET
            + pairwise_length
            + CORE_FRAME_RSN_IE_KEY_MANAGEMENT_COUNT_SIZE
            + ( num_of_suites * CORE_FRAME_RSN_IE_KEY_MANAGEMENT_OUI_LENGTH ) );
        }

    return suites;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
core_cipher_suite_e core_frame_rsn_ie_c::cipher_oui_to_enum(
    const u8_t* oui ) const
    {
    if ( core_tools_c::compare(
        oui,
        CORE_FRAME_RSN_IE_OUI_PREFIX_LENGTH,
        &CORE_FRAME_RSN_IE_OUI_PREFIX[0],
        CORE_FRAME_RSN_IE_OUI_PREFIX_LENGTH ) )
        {
        DEBUG( "core_frame_rsn_ie_c::cipher_oui_to_enum() - unknown cipher suite" );
        return core_cipher_suite_none;
        }

    switch ( *(oui + CORE_FRAME_RSN_IE_OUI_PREFIX_LENGTH) )
        {
        case CORE_FRAME_RSN_IE_CIPHER_ID_WEP40:
            return core_cipher_suite_wep40;
        case CORE_FRAME_RSN_IE_CIPHER_ID_WEP104:
            return core_cipher_suite_wep104;
        case CORE_FRAME_RSN_IE_CIPHER_ID_TKIP:
            return core_cipher_suite_tkip;
        case CORE_FRAME_RSN_IE_CIPHER_ID_CCMP:
            return core_cipher_suite_ccmp;
        default:
            return core_cipher_suite_none;
        }
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//    
core_key_management_e core_frame_rsn_ie_c::key_management_oui_to_enum(
    const u8_t* oui ) const
    {
    if ( !core_tools_c::compare(
        oui,
        CORE_FRAME_RSN_IE_OUI_PREFIX_LENGTH,
        &CORE_FRAME_RSN_IE_OUI_PREFIX[0],
        CORE_FRAME_RSN_IE_OUI_PREFIX_LENGTH ) )
        {
        switch ( *(oui + CORE_FRAME_RSN_IE_OUI_PREFIX_LENGTH) )
            {
            case CORE_FRAME_RSN_IE_KEY_MANAGEMENT_ID_EAP:
                DEBUG( "core_frame_rsn_ie_c::key_management_oui_to_enum() - core_key_management_eap" );
                return core_key_management_eap;
            case CORE_FRAME_RSN_IE_KEY_MANAGEMENT_ID_PSK:
                DEBUG( "core_frame_rsn_ie_c::key_management_oui_to_enum() - core_key_management_preshared" );
                return core_key_management_preshared;
            default:
                DEBUG( "core_frame_rsn_ie_c::key_management_oui_to_enum() - core_key_management_none" );
                return core_key_management_none;
            }
        }

    return wpx_adaptation_m.wpx_key_management_suite( oui );
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
u8_t core_frame_rsn_ie_c::get_raw_group_cipher(
    core_cipher_suite_e cipher ) const
    {
    switch( cipher )
        {        
        case core_cipher_suite_wep40:
            DEBUG( "core_frame_rsn_ie_c::get_raw_group_cipher() - core_cipher_suite_wep40" );
            return CORE_FRAME_RSN_IE_CIPHER_ID_WEP40;
        case core_cipher_suite_wep104:
            DEBUG( "core_frame_rsn_ie_c::get_raw_group_cipher() - core_cipher_suite_wep104" );
            return CORE_FRAME_RSN_IE_CIPHER_ID_WEP104;
        case core_cipher_suite_tkip:
            DEBUG( "core_frame_rsn_ie_c::get_raw_group_cipher() - core_cipher_suite_tkip" );
            return CORE_FRAME_RSN_IE_CIPHER_ID_TKIP;
        case core_cipher_suite_ccmp:
            DEBUG( "core_frame_rsn_ie_c::get_raw_group_cipher() - core_cipher_suite_ccmp" );
            return CORE_FRAME_RSN_IE_CIPHER_ID_CCMP;
        default:
            DEBUG( "core_frame_rsn_ie_c::get_raw_group_cipher() - core_cipher_suite_none" );
            return CORE_FRAME_RSN_IE_CIPHER_ID_NONE;
        }
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
u8_t core_frame_rsn_ie_c::get_raw_pairwise_cipher(
    core_cipher_suite_e cipher ) const
    {
    switch( cipher )
        {        
        case core_cipher_suite_wep40:
            DEBUG( "core_frame_rsn_ie_c::get_raw_pairwise_cipher() - core_cipher_suite_wep40" );
            return CORE_FRAME_RSN_IE_CIPHER_ID_WEP40;
        case core_cipher_suite_wep104:
            DEBUG( "core_frame_rsn_ie_c::get_raw_pairwise_cipher() - core_cipher_suite_wep104" );
            return CORE_FRAME_RSN_IE_CIPHER_ID_WEP104;
        case core_cipher_suite_tkip:
            DEBUG( "core_frame_rsn_ie_c::get_raw_pairwise_cipher() - core_cipher_suite_tkip" );
            return CORE_FRAME_RSN_IE_CIPHER_ID_TKIP;
        case core_cipher_suite_ccmp:
            DEBUG( "core_frame_rsn_ie_c::get_raw_pairwise_cipher() - core_cipher_suite_ccmp" );
            return CORE_FRAME_RSN_IE_CIPHER_ID_CCMP;
        default:
            DEBUG( "core_frame_rsn_ie_c::get_raw_pairwise_cipher() - core_cipher_suite_none" );
            return CORE_FRAME_RSN_IE_CIPHER_ID_NONE;
        }
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
u8_t core_frame_rsn_ie_c::get_raw_key_management(
    core_key_management_e key_management ) const
    {
    switch( key_management )
        {        
        case core_key_management_eap:
            DEBUG( "core_frame_rsn_ie_c::get_raw_key_management() - core_key_management_eap" );
            return CORE_FRAME_RSN_IE_KEY_MANAGEMENT_ID_EAP;
        case core_key_management_preshared:
            DEBUG( "core_frame_rsn_ie_c::get_raw_key_management() - core_key_management_preshared" );
            return CORE_FRAME_RSN_IE_KEY_MANAGEMENT_ID_PSK;
        default:
            DEBUG( "core_frame_rsn_ie_c::get_raw_key_management() - core_key_management_none" );
            return CORE_FRAME_RSN_IE_KEY_MANAGEMENT_ID_NONE;
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_frame_rsn_ie_c::generate(
    core_cipher_suite_e group_cipher,
    core_cipher_suite_e pairwise_cipher,
    core_key_management_e key_management,
    u16_t pmkid_length,
    const u8_t* pmkid_data)
    {
    ASSERT( !data_length_m );
    ASSERT( max_data_length_m );   

    core_frame_dot11_ie_c::generate(
        core_frame_dot11_ie_element_id_rsn );

    // Version field
    u16_t value( CORE_FRAME_RSN_IE_VERSION ); 
    core_tools_c::copy(
        &data_m[data_length_m],
        reinterpret_cast<u8_t*>(&value),
        sizeof( value ) );
    data_length_m += sizeof( value );

    // Group Cipher Suite field
    core_tools_c::copy(
        &data_m[data_length_m],
        &CORE_FRAME_RSN_IE_OUI_PREFIX[0],
        CORE_FRAME_RSN_IE_OUI_PREFIX_LENGTH );
    data_length_m += CORE_FRAME_RSN_IE_OUI_PREFIX_LENGTH;
    data_m[data_length_m++] = get_raw_group_cipher( group_cipher );

    // Pairwise Cipher Suite Count field
    value = CORE_FRAME_RSN_IE_PAIRWISE_SUITE_COUNT; 
    core_tools_c::copy(
        &data_m[data_length_m],
        reinterpret_cast<u8_t*>(&value),
        sizeof( value ) );
    data_length_m += sizeof( value );

    // Pairwise Cipher Suite List field
    core_tools_c::copy(
        &data_m[data_length_m],
        &CORE_FRAME_RSN_IE_OUI_PREFIX[0],
        CORE_FRAME_RSN_IE_OUI_PREFIX_LENGTH );
    data_length_m += CORE_FRAME_RSN_IE_OUI_PREFIX_LENGTH;
    data_m[data_length_m++] = get_raw_pairwise_cipher( pairwise_cipher );

    // Key Management Suite Count field
    value = CORE_FRAME_RSN_IE_KEY_MANAGEMENT_COUNT;
    core_tools_c::copy(
        &data_m[data_length_m],
        reinterpret_cast<u8_t*>(&value),
        sizeof( value ) );
    data_length_m += sizeof( value );

    // Key Management Suite List field
    u8_t wpx_oui_length( 0 );
    if ( wpx_adaptation_m.get_wpx_key_management_oui(
        key_management,
        wpx_oui_length,
        &data_m[data_length_m] ) == core_error_ok )
        {
        data_length_m = static_cast<u8_t>( data_length_m + wpx_oui_length );
        }
    else
        {
        core_tools_c::copy(
            &data_m[data_length_m],
            &CORE_FRAME_RSN_IE_OUI_PREFIX[0],
            CORE_FRAME_RSN_IE_OUI_PREFIX_LENGTH );
        data_length_m += CORE_FRAME_RSN_IE_OUI_PREFIX_LENGTH;
        data_m[data_length_m++] = get_raw_key_management( key_management );
        }

    // RSN Capabilities field
    value = CORE_FRAME_RSN_IE_CAPABILITIES;    
    core_tools_c::copy(
        &data_m[data_length_m],
        reinterpret_cast<u8_t*>(&value),
        sizeof( value ) );
    data_length_m += sizeof( value );

    if ( pmkid_data &&
         pmkid_length == CORE_FRAME_RSN_IE_PMKID_LENGTH )
        {
        DEBUG( "core_frame_rsn_ie_c::core_frame_rsn_ie_c() - adding PMKID" );
        
        // PMKID Count field
        value = CORE_FRAME_RSN_IE_PMKID_COUNT;
        core_tools_c::copy(
            &data_m[data_length_m],
            reinterpret_cast<u8_t*>(&value),
            sizeof( value ) );
        data_length_m += sizeof( value );

        // PMKID List field
        core_tools_c::copy(
            &data_m[data_length_m],
            &pmkid_data[0],
            CORE_FRAME_RSN_IE_PMKID_LENGTH );
        data_length_m += CORE_FRAME_RSN_IE_PMKID_LENGTH;
        }

    set_length( data_length_m );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_rsn_ie_c::core_frame_rsn_ie_c(
    abs_core_wpx_adaptation_c& wpx_adaptation,
    u16_t data_length,
    const u8_t* data,
    u16_t max_data_length ) :    
    core_frame_dot11_ie_c( data_length, data, max_data_length ),
    wpx_adaptation_m( wpx_adaptation )
    {
    DEBUG( "core_frame_rsn_ie_c::core_frame_rsn_ie_c()" );   
    }
