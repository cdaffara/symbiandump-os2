/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class for parsing GB 15629.11 (WAPI) IEs.
*
*/


#include "core_frame_wapi_ie.h"
#include "core_tools.h"
#include "core_tools_parser.h"
#include "am_debug.h"

const u8_t CORE_FRAME_WAPI_IE_MIN_LENGTH = 22;
const u8_t CORE_FRAME_WAPI_IE_BKID_COUNT_SIZE = 2;
const u8_t CORE_FRAME_WAPI_IE_KEY_MANAGEMENT_COUNT_OFFSET = 4;
const u8_t CORE_FRAME_WAPI_IE_KEY_MANAGEMENT_COUNT_SIZE = 2;
const u8_t CORE_FRAME_WAPI_IE_KEY_MANAGEMENT_OUI_LENGTH = 4;
const u8_t CORE_FRAME_WAPI_IE_CIPHER_COUNT_SIZE = 2;
const u8_t CORE_FRAME_WAPI_IE_CIPHER_OUI_LENGTH = 4;

/** IDs for different cipher suites in WAPI IE. */
const u8_t CORE_FRAME_WAPI_IE_CIPHER_ID_NONE    = 0x00;
const u8_t CORE_FRAME_WAPI_IE_CIPHER_ID_WPI     = 0x01;

/** IDs for different key management suites. */
const u8_t CORE_FRAME_WAPI_IE_KEY_MANAGEMENT_ID_NONE = 0x00;
const u8_t CORE_FRAME_WAPI_IE_KEY_MANAGEMENT_ID_CERTIFICATE = 0x01;
const u8_t CORE_FRAME_WAPI_IE_KEY_MANAGEMENT_ID_PSK  = 0x02;

const u8_t CORE_FRAME_WAPI_IE_OUI_PREFIX_LENGTH = 3;
const u8_t CORE_FRAME_WAPI_IE_OUI_PREFIX[] = { 0x00, 0x14, 0x72 };

const u16_t CORE_FRAME_WAPI_IE_VERSION = 1;
const u16_t CORE_FRAME_WAPI_IE_PAIRWISE_SUITE_COUNT = 1;
const u16_t CORE_FRAME_WAPI_IE_KEY_MANAGEMENT_COUNT = 1;
const u16_t CORE_FRAME_WAPI_IE_BKID_COUNT = 1;
const u16_t CORE_FRAME_WAPI_IE_BKID_LENGTH = 16;

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_wapi_ie_c* core_frame_wapi_ie_c::instance(
    const core_frame_dot11_ie_c& ie )
    {
    if( ie.data_length() < CORE_FRAME_WAPI_IE_MIN_LENGTH )
        {
        DEBUG( "core_frame_wapi_ie_c::instance() - not a valid IE, too short" );

        return NULL;
        }

    core_frame_wapi_ie_c* instance = new core_frame_wapi_ie_c(
        ie.data_length(),
        ie.data(),
        0 );
    if( !instance )
        {
        DEBUG( "core_frame_wapi_ie_c::instance() - unable to create an instance" );

        return NULL;
        }
    
    return instance;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_wapi_ie_c* core_frame_wapi_ie_c::instance(
    core_cipher_suite_e group_cipher,
    core_cipher_suite_e pairwise_cipher,
    core_key_management_e key_management,
    u16_t capability,
    u16_t bkid_length,
    const u8_t* bkid_data )
    {
    const u8_t max_length = CORE_FRAME_WAPI_IE_MIN_LENGTH +
        CORE_FRAME_WAPI_IE_BKID_COUNT_SIZE + bkid_length;

    u8_t* buffer = new u8_t[max_length];
    if( !buffer )
        {
        DEBUG( "core_frame_wapi_ie_c::instance() - unable create the internal buffer" );
        return NULL;
        }

    core_frame_wapi_ie_c* instance =
        new core_frame_wapi_ie_c( 0, buffer, max_length );
    if( !instance )
        {
        DEBUG( "core_frame_wapi_ie_c::instance() - unable to create an instance" );
        delete[] buffer;

        return NULL;
        }

    instance->generate(
        group_cipher,
        pairwise_cipher,
        key_management,
        capability,
        bkid_length,
        bkid_data );

    return instance;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//       
core_frame_wapi_ie_c::~core_frame_wapi_ie_c()
    {
    DEBUG( "core_frame_wapi_ie_c::~core_frame_wapi_ie_c()" );   
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u8_t core_frame_wapi_ie_c::group_cipher_suite() const
    {
    u8_t cipher_offset(
        group_cipher_offset() );
    u32_t offset_end(
        cipher_offset +
        CORE_FRAME_WAPI_IE_CIPHER_OUI_LENGTH );
    if( !cipher_offset ||
        offset_end > data_length() )
        {
        DEBUG( "core_frame_wapi_ie_c::pairwise_cipher_suites() - cipher_offset points past the end of IE, ignoring" );

        return core_cipher_suite_none;
        }

    return cipher_oui_to_enum(
        data_m + cipher_offset );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u8_t core_frame_wapi_ie_c::pairwise_cipher_suites() const
    {
    u8_t suites( 0 );
    u8_t pairwise_cipher_offset(
        pairwise_cipher_count_offset() );
    if( !pairwise_cipher_offset )
        {
        DEBUG( "core_frame_wapi_ie_c::pairwise_cipher_suites() - pairwise_cipher_offset points past the end of IE, ignoring" );
        
        return core_cipher_suite_none;
        }

    u16_t pairwise_cipher_count = core_tools_c::get_u16(
        data_m,
        pairwise_cipher_offset );

    DEBUG1( "core_frame_wapi_ie_c::pairwise_cipher_suites() - pairwise_cipher_count = %u",
        pairwise_cipher_count );

    u32_t offset_end(
        pairwise_cipher_offset +
        CORE_FRAME_WAPI_IE_CIPHER_COUNT_SIZE +
        ( pairwise_cipher_count * CORE_FRAME_WAPI_IE_CIPHER_OUI_LENGTH ) );
    if( offset_end > data_length() )
        {
        DEBUG( "core_frame_wapi_ie_c::pairwise_cipher_suites() - pairwise_cipher_count points past the end of IE, ignoring" );

        return core_cipher_suite_none;
        }

    while( pairwise_cipher_count-- )
        {
        suites += cipher_oui_to_enum(
            data_m + pairwise_cipher_offset +
            CORE_FRAME_WAPI_IE_CIPHER_COUNT_SIZE +
            ( pairwise_cipher_count * CORE_FRAME_WAPI_IE_CIPHER_OUI_LENGTH ) );
        }

    return suites;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u8_t core_frame_wapi_ie_c::key_management_suites() const
    {
    u8_t suites( 0 );
    u16_t key_management_suite_count = core_tools_c::get_u16(
        data_m, CORE_FRAME_WAPI_IE_KEY_MANAGEMENT_COUNT_OFFSET );
    u32_t offset_end(
        CORE_FRAME_WAPI_IE_KEY_MANAGEMENT_COUNT_OFFSET +
        CORE_FRAME_WAPI_IE_KEY_MANAGEMENT_COUNT_SIZE +
        ( key_management_suite_count * CORE_FRAME_WAPI_IE_KEY_MANAGEMENT_OUI_LENGTH ) );
    if( offset_end > data_length() )
        {
        DEBUG( "core_frame_wapi_ie_c::key_management_suites() - key_management_suite_count points past the end of IE, ignoring" );

        return core_key_management_none;
        }

    DEBUG1( "core_frame_wapi_ie_c::key_management_suites() - key_management_suite_count = %u",
        key_management_suite_count );

    while( key_management_suite_count-- )
        {
        suites += key_management_oui_to_enum(
            data_m + CORE_FRAME_WAPI_IE_KEY_MANAGEMENT_COUNT_OFFSET +
            CORE_FRAME_WAPI_IE_KEY_MANAGEMENT_COUNT_SIZE +
            ( key_management_suite_count * CORE_FRAME_WAPI_IE_KEY_MANAGEMENT_OUI_LENGTH ) );
        }

    return suites;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
u8_t core_frame_wapi_ie_c::pairwise_cipher_count_offset() const
    {
    u16_t key_management_suite_count = core_tools_c::get_u16(
        data_m,
        CORE_FRAME_WAPI_IE_KEY_MANAGEMENT_COUNT_OFFSET );
    u32_t offset_end(
        CORE_FRAME_WAPI_IE_KEY_MANAGEMENT_COUNT_OFFSET +
        CORE_FRAME_WAPI_IE_KEY_MANAGEMENT_COUNT_SIZE +
        ( key_management_suite_count * CORE_FRAME_WAPI_IE_KEY_MANAGEMENT_OUI_LENGTH ) );
    if( offset_end > data_length() )
        {
        return 0;
        }

    return offset_end;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
u8_t core_frame_wapi_ie_c::group_cipher_offset() const
    {
    u8_t pairwise_cipher_offset(
        pairwise_cipher_count_offset() );
    if( !pairwise_cipher_offset )
        {
        return 0;
        }

    u16_t pairwise_cipher_count = core_tools_c::get_u16(
        data_m,
        pairwise_cipher_offset );
    u32_t offset_end(
        pairwise_cipher_offset +
        CORE_FRAME_WAPI_IE_CIPHER_COUNT_SIZE +
        ( pairwise_cipher_count * CORE_FRAME_WAPI_IE_CIPHER_OUI_LENGTH ) );
    if( offset_end > data_length() )
        {
        return 0;
        }

    return offset_end;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
core_cipher_suite_e core_frame_wapi_ie_c::cipher_oui_to_enum(
    const u8_t* oui ) const
    {
    if( core_tools_c::compare(
        oui,
        CORE_FRAME_WAPI_IE_OUI_PREFIX_LENGTH,
        &CORE_FRAME_WAPI_IE_OUI_PREFIX[0],
        CORE_FRAME_WAPI_IE_OUI_PREFIX_LENGTH ) )
        {
        DEBUG( "core_frame_wapi_ie_c::cipher_oui_to_enum() - unknown cipher suite" );
        return core_cipher_suite_none;
        }

    switch( *(oui + CORE_FRAME_WAPI_IE_OUI_PREFIX_LENGTH) )
        {
        case CORE_FRAME_WAPI_IE_CIPHER_ID_WPI:
            return core_cipher_suite_wpi;
        default:
            return core_cipher_suite_none;
        }
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//    
core_key_management_e core_frame_wapi_ie_c::key_management_oui_to_enum(
    const u8_t* oui ) const
    {
    if( core_tools_c::compare(
        oui,
        CORE_FRAME_WAPI_IE_OUI_PREFIX_LENGTH,
        &CORE_FRAME_WAPI_IE_OUI_PREFIX[0],
        CORE_FRAME_WAPI_IE_OUI_PREFIX_LENGTH ) )
        {
        DEBUG( "core_frame_wapi_ie_c::key_management_oui_to_enum() - unknown key management suite" );
        return core_key_management_none;    
        }

    switch( *(oui + CORE_FRAME_WAPI_IE_OUI_PREFIX_LENGTH) )
        {
        case CORE_FRAME_WAPI_IE_KEY_MANAGEMENT_ID_CERTIFICATE:
            DEBUG( "core_frame_wapi_ie_c::key_management_oui_to_enum() - core_key_management_wapi_certificate" );
            return core_key_management_wapi_certificate;
        case CORE_FRAME_WAPI_IE_KEY_MANAGEMENT_ID_PSK:
            DEBUG( "core_frame_wapi_ie_c::key_management_oui_to_enum() - core_key_management_wapi_psk" );
            return core_key_management_wapi_psk;
        default:
            DEBUG( "core_frame_wapi_ie_c::key_management_oui_to_enum() - core_key_management_none" );
            return core_key_management_none;
        }
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
u8_t core_frame_wapi_ie_c::get_raw_group_cipher(
    core_cipher_suite_e cipher ) const
    {
    switch( cipher )
        {        
        case core_cipher_suite_wpi:
            DEBUG( "core_frame_wapi_ie_c::get_raw_group_cipher() - core_cipher_suite_wpi" );
            return CORE_FRAME_WAPI_IE_CIPHER_ID_WPI;
        default:
            DEBUG( "core_frame_wapi_ie_c::get_raw_group_cipher() - core_cipher_suite_none" );
            return CORE_FRAME_WAPI_IE_CIPHER_ID_NONE;
        }
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
u8_t core_frame_wapi_ie_c::get_raw_pairwise_cipher(
    core_cipher_suite_e cipher ) const
    {
    switch( cipher )
        {        
        case core_cipher_suite_wpi:
            DEBUG( "core_frame_wapi_ie_c::get_raw_pairwise_cipher() - core_cipher_suite_wpi" );
            return CORE_FRAME_WAPI_IE_CIPHER_ID_WPI;
        default:
            DEBUG( "core_frame_wapi_ie_c::get_raw_pairwise_cipher() - core_cipher_suite_none" );
            return CORE_FRAME_WAPI_IE_CIPHER_ID_NONE;
        }
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
u8_t core_frame_wapi_ie_c::get_raw_key_management(
    core_key_management_e key_management ) const
    {
    switch( key_management )
        {        
        case core_key_management_wapi_certificate:
            DEBUG( "core_frame_wapi_ie_c::get_raw_key_management() - core_key_management_wapi_certificate" );
            return CORE_FRAME_WAPI_IE_KEY_MANAGEMENT_ID_CERTIFICATE;
        case core_key_management_wapi_psk:
            DEBUG( "core_frame_wapi_ie_c::get_raw_key_management() - core_key_management_wapi_psk" );
            return CORE_FRAME_WAPI_IE_KEY_MANAGEMENT_ID_PSK;
        default:
            DEBUG( "core_frame_wapi_ie_c::get_raw_key_management() - core_key_management_none" );
            return CORE_FRAME_WAPI_IE_KEY_MANAGEMENT_ID_NONE;
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_frame_wapi_ie_c::generate(
    core_cipher_suite_e group_cipher,
    core_cipher_suite_e pairwise_cipher,
    core_key_management_e key_management,
    u16_t capability,
    u16_t bkid_length,
    const u8_t* bkid_data)
    {
    ASSERT( !data_length_m );
    ASSERT( max_data_length_m );   

    core_frame_dot11_ie_c::generate(
        core_frame_dot11_ie_element_id_wapi );

    // Version field
    core_tools_c::append_u16(
        data_m,
        data_length_m,
        CORE_FRAME_WAPI_IE_VERSION );

    // Key Management Suite Count field
    core_tools_c::append_u16(
        data_m,
        data_length_m,
        CORE_FRAME_WAPI_IE_KEY_MANAGEMENT_COUNT );

    // Key Management Suite List field
    core_tools_c::copy(
        &data_m[data_length_m],
        &CORE_FRAME_WAPI_IE_OUI_PREFIX[0],
        CORE_FRAME_WAPI_IE_OUI_PREFIX_LENGTH );
    data_length_m += CORE_FRAME_WAPI_IE_OUI_PREFIX_LENGTH;
    data_m[data_length_m++] = get_raw_key_management( key_management );

    // Pairwise Cipher Suite Count field
    core_tools_c::append_u16(
        data_m,
        data_length_m,
        CORE_FRAME_WAPI_IE_PAIRWISE_SUITE_COUNT );

    // Pairwise Cipher Suite List field
    core_tools_c::copy(
        &data_m[data_length_m],
        &CORE_FRAME_WAPI_IE_OUI_PREFIX[0],
        CORE_FRAME_WAPI_IE_OUI_PREFIX_LENGTH );
    data_length_m += CORE_FRAME_WAPI_IE_OUI_PREFIX_LENGTH;
    data_m[data_length_m++] = get_raw_pairwise_cipher( pairwise_cipher );
    
    // Group Cipher Suite field
    core_tools_c::copy(
        &data_m[data_length_m],
        &CORE_FRAME_WAPI_IE_OUI_PREFIX[0],
        CORE_FRAME_WAPI_IE_OUI_PREFIX_LENGTH );
    data_length_m += CORE_FRAME_WAPI_IE_OUI_PREFIX_LENGTH;
    data_m[data_length_m++] = get_raw_group_cipher( group_cipher );

    // Capability information
    core_tools_c::append_u16(
        data_m,
        data_length_m,
        capability );

    if( bkid_data &&
        bkid_length == CORE_FRAME_WAPI_IE_BKID_LENGTH )
        {
        DEBUG( "core_frame_wapi_ie_c::core_frame_wapi_ie_c() - adding BKID" );

        // BKID Count field
        core_tools_c::append_u16(
            data_m,
            data_length_m,
            CORE_FRAME_WAPI_IE_BKID_COUNT );

        // BKID List field
        core_tools_c::copy(
            &data_m[data_length_m],
            &bkid_data[0],
            CORE_FRAME_WAPI_IE_BKID_LENGTH );
        data_length_m += CORE_FRAME_WAPI_IE_BKID_LENGTH;
        }
    else
        {
        // BKID Count field
        core_tools_c::append_u16(
            data_m,
            data_length_m,
            0 );        
        }

    set_length( data_length_m );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_wapi_ie_c::core_frame_wapi_ie_c(
    u16_t data_length,
    const u8_t* data,
    u16_t max_data_length ) :    
    core_frame_dot11_ie_c( data_length, data, max_data_length )
    {
    DEBUG( "core_frame_wapi_ie_c::core_frame_wapi_ie_c()" );   
    }
