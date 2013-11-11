/*
* Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class for WLAN EAPOL Plugin interface.
*
*/

/*
* %version: 14 %
*/

#include "core_wlan_eapol_if_message.h"
#include "core_types.h"
#include "core_tools.h"


/**
 * Constructor
 */
core_wlan_eapol_if_parameter_c::core_wlan_eapol_if_parameter_c()
    : data_m( NULL )
    , buffer_length_m( 0 )
    , delete_buffer_m( false_t)
    {
    }

/**
 * Constructor
 *
 * @param data Pointer to the TLV encoded data.
 * @param data_length Length of the TLV encoded data.
 */
core_wlan_eapol_if_parameter_c::core_wlan_eapol_if_parameter_c(
    u8_t *data, u32_t data_length )
    : data_m( data )
    , buffer_length_m( data_length )
    , delete_buffer_m( false_t )
    {
    }


/** 
 * Destructor.
 */
core_wlan_eapol_if_parameter_c::~core_wlan_eapol_if_parameter_c()
    {
    if ( delete_buffer_m )
        {
        core_tools_c::fillz( data_m, buffer_length_m );
        delete[] data_m;
        data_m = NULL;
        }
    }



/**
 * Update content of parameter
 */
void core_wlan_eapol_if_parameter_c::update(
    u8_t *data, u32_t data_length )
    {
    data_m = data;
    buffer_length_m = data_length;
    delete_buffer_m = false_t;
    }

// ============================================================================
// General methods to handle parameters

u32_t core_wlan_eapol_if_parameter_c::size() const
    {
    return WLAN_EAPOL_MESSAGE_IF_DATA_OFFSET + get_parameter_length();
    }

wlan_eapol_if_message_type_e core_wlan_eapol_if_parameter_c::get_parameter_type() const
    {
    return static_cast<wlan_eapol_if_message_type_e>( core_tools_c::get_u32_big_endian(
        &data_m[0],
        WLAN_EAPOL_MESSAGE_IF_TYPE_OFFSET ));
    }

u32_t core_wlan_eapol_if_parameter_c::get_parameter_length() const
    {
    return core_tools_c::get_u32_big_endian(
        &data_m[0],
        WLAN_EAPOL_MESSAGE_IF_LENGTH_OFFSET );
    }

u8_t* core_wlan_eapol_if_parameter_c::get_data() const
    { 
    return &data_m[0];
    }





// ============================================================================
// Parameter specific methods

core_error_e core_wlan_eapol_if_parameter_c::set_parameter_data(
    wlan_eapol_if_message_type_e type, 
    u32_t value )
    {
    DEBUG2( "core_wlan_eapol_if_parameter_c::set_parameter_data(type=%i, u32_t value=%i)", type, value );

    if ( type == wlan_eapol_if_message_type_u8_t )
        {
        return set_parameter_data_u8_t( type, value );
        }
    else if ( type == wlan_eapol_if_message_type_u16_t )
        {
        return set_parameter_data_u16_t( type, value );
        }
    else if ( type == wlan_eapol_if_message_type_boolean )
        {
        return set_parameter_data_boolean( type, value );
        }
    
    core_error_e error = reserve_buffer( WLAN_EAPOL_MESSAGE_IF_DATA_OFFSET + sizeof( value ) );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    add_header( type, sizeof( value ) );
    add_parameter_u32_t( value );
    return core_error_ok;
    }




core_error_e core_wlan_eapol_if_parameter_c::set_parameter_data(
    wlan_eapol_if_message_type_e type, 
    const u8_t* data, 
    u32_t length )
    { 
    DEBUG3( "core_wlan_eapol_if_parameter_c::set_parameter_data(type=%i, data=%08X, length=%i)", type, data, length );

    core_error_e error = reserve_buffer( WLAN_EAPOL_MESSAGE_IF_DATA_OFFSET + length );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    add_header( type, length );
    
    if ( data == NULL || length == 0 )
        {
        // Variable data and Array could be empty. In that case data is NULL.
        if ( type != wlan_eapol_if_message_type_array &&
             type != wlan_eapol_if_message_type_variable_data )
            {
            DEBUG( "core_wlan_eapol_if_parameter_c::set_parameter_data() - Illegal arguments" );
            ASSERT( false_t );
            return core_error_illegal_argument;
            }
        return core_error_ok;
        }
    else
        {
        core_tools_c::copy( 
            &data_m[WLAN_EAPOL_MESSAGE_IF_DATA_OFFSET],
            data, 
            length );
        }
    return core_error_ok;
    }

core_error_e core_wlan_eapol_if_parameter_c::set_parameter_data(
    wlan_eapol_if_message_type_e type, 
    u32_t vendor_id, 
    u32_t vendor_type )
    { 
    DEBUG3( "core_wlan_eapol_if_parameter_c::set_parameter_data(type=%i, vendor_id=%06X, vendor_type=%08X)", type, vendor_id, vendor_type );
    if ( type != wlan_eapol_if_message_type_eap_type )
        {
        return core_error_illegal_argument;
        }
    
    u32_t length( 8 ); // Size of EAP type.
    core_error_e error = reserve_buffer( WLAN_EAPOL_MESSAGE_IF_DATA_OFFSET + length );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    add_header( type, length );
    
    const u32_t eap_type = 254; // This is constant in Extended EAP type.
    vendor_id = (vendor_id & 0x00FFFFFF) ^ (eap_type<<24);
    vendor_id = core_tools_c::convert_host_to_big_endian( vendor_id );
    core_tools_c::copy( 
        &data_m[WLAN_EAPOL_MESSAGE_IF_DATA_OFFSET], 
        &vendor_id, 
        sizeof( vendor_id ) );
    
    vendor_type = core_tools_c::convert_host_to_big_endian( vendor_type );
    core_tools_c::copy( 
        &data_m[WLAN_EAPOL_MESSAGE_IF_DATA_OFFSET + sizeof( vendor_id )], 
        &vendor_type, 
        sizeof( vendor_type ) );
    
    return core_error_ok;
    }


core_error_e core_wlan_eapol_if_parameter_c::set_parameter_data_boolean(
    wlan_eapol_if_message_type_e type, 
    bool_t boolean )
    { 
    DEBUG2( "core_wlan_eapol_if_parameter_c::set_parameter_data_boolean(type=%i, bool_t value=%i)", type, boolean );
    core_error_e error = reserve_buffer( WLAN_EAPOL_MESSAGE_IF_DATA_OFFSET + sizeof( boolean ) );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    add_header( type, sizeof( boolean ) );
    add_parameter_u32_t( boolean );
    return core_error_ok;
    }

core_error_e core_wlan_eapol_if_parameter_c::set_parameter_data_u16_t(
    wlan_eapol_if_message_type_e type, 
    u16_t value )
    {
    DEBUG2( "core_wlan_eapol_if_parameter_c::set_parameter_data_u16_t(type=%i, u16_t value=%i)", type, value );
    core_error_e error = reserve_buffer( WLAN_EAPOL_MESSAGE_IF_DATA_OFFSET + sizeof( value ) );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    add_header( type, sizeof( value ) );
    add_parameter_u16_t( value );
    return core_error_ok;
    }

core_error_e core_wlan_eapol_if_parameter_c::set_parameter_data_u8_t(
    wlan_eapol_if_message_type_e type, 
    u8_t value )
    { 
    DEBUG2( "core_wlan_eapol_if_parameter_c::set_parameter_data_u8_t(type=%i, u8_t value=%i)", type, value );
    core_error_e error = reserve_buffer( WLAN_EAPOL_MESSAGE_IF_DATA_OFFSET + sizeof( value ) );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    add_header( type, sizeof( value ) );
    add_parameter_u8_t( value );
    return core_error_ok;
    }


core_error_e core_wlan_eapol_if_parameter_c::set_parameter_data_u64_t(
        u64_t data )
        {
        DEBUG( "core_wlan_eapol_if_parameter_c::set_parameter_data_u64_t()" );
        
        core_error_e error = reserve_buffer( WLAN_EAPOL_MESSAGE_IF_DATA_OFFSET + sizeof( data ) );
        if ( error != core_error_ok )
            {
            return error;
            }
        
        add_header( wlan_eapol_if_message_type_u64_t, sizeof( data ) );
        
        u64_t parameter_data = 
           static_cast<u64_t>( ((data & 0xFF) << 56)
                            | ((data & 0xFF00) << 40)
                            | ((data & 0xFF0000) << 24)
                            | ((data & 0xFF000000) << 8)
                            | (((data >> 32) & 0xFF) << 24)
                            | (((data >> 32) & 0xFF00) << 8)
                            | (((data >> 32) & 0xFF0000) >> 8)
                            | (((data >> 32) & 0xFF000000) >> 24) );

        core_tools_c::copy( 
            &data_m[WLAN_EAPOL_MESSAGE_IF_DATA_OFFSET], 
            &parameter_data, 
            sizeof( parameter_data ) );
        COMPILE_ASSERT( sizeof( parameter_data ) == 8 ); // Specification says this.
        return core_error_ok;
        }


// ============================================================================

// All 32 bit values
core_error_e core_wlan_eapol_if_parameter_c::get_parameter_data(
    u32_t * value ) const
    { 
    ASSERT( value );
    wlan_eapol_if_message_type_e type = get_parameter_type();
    if ( type == wlan_eapol_if_message_type_u32_t
        || type == wlan_eapol_if_message_type_function
        || type == wlan_eapol_if_message_type_eap_protocol_layer
        || type == wlan_eapol_if_message_type_eap_status
        || type == wlan_eapol_if_message_type_eapol_key_802_11_authentication_mode
        || type == wlan_eapol_if_message_type_eapol_key_authentication_type
        || type == wlan_eapol_if_message_type_eapol_key_type
        || type == wlan_eapol_if_message_type_eapol_tkip_mic_failure_type
        || type == wlan_eapol_if_message_type_eapol_wlan_authentication_state
        || type == wlan_eapol_if_message_type_error
        || type == wlan_eapol_if_message_type_RSNA_cipher )
        {
        *value = get_parameter_u32_t();
        return core_error_ok;
        }
    *value = 0;
    DEBUG( "core_wlan_eapol_if_parameter_c::get_parameter_data( u32_t ) - Error: parameter not found" );
    return core_error_not_found;
    }

// 
core_error_e core_wlan_eapol_if_parameter_c::get_parameter_data(
    u64_t * value ) const
    { 
    ASSERT( value );
    if ( get_parameter_type() == wlan_eapol_if_message_type_u64_t )
        {
        *value = get_parameter_u64_t();
        return core_error_ok;
        }
    *value = 0;
    DEBUG( "core_wlan_eapol_if_parameter_c::get_parameter_data( u64_t ) - Error: parameter not found" );
    return core_error_not_found;
    }

// 
core_error_e core_wlan_eapol_if_parameter_c::get_parameter_data(
    bool_t * value ) const
    {
    ASSERT( value );
    if ( get_parameter_type() == wlan_eapol_if_message_type_boolean )
        {
        *value = get_parameter_u32_t();
        return core_error_ok;
        }
    *value = 0;
    DEBUG( "core_wlan_eapol_if_parameter_c::get_parameter_data( bool_t ) - Error: parameter not found" );
    return core_error_not_found;
    }

core_error_e core_wlan_eapol_if_parameter_c::get_parameter_data(
    u16_t * value ) const
    {
    ASSERT( value );
    if ( get_parameter_type() == wlan_eapol_if_message_type_u16_t )
        {
        *value = get_parameter_u16_t();
        return core_error_ok;
        }
    *value = 0;
    DEBUG( "core_wlan_eapol_if_parameter_c::get_parameter_data( u16_t ) - Error: parameter not found" );
    return core_error_not_found;
    }

core_error_e core_wlan_eapol_if_parameter_c::get_parameter_data(
    u8_t * value ) const
    { 
    ASSERT( value );
    if ( get_parameter_type() == wlan_eapol_if_message_type_u8_t )
        {
        *value = get_parameter_u8_t();
        return core_error_ok;
        }
    *value = 0;
    DEBUG( "core_wlan_eapol_if_parameter_c::get_parameter_data( u8_t ) - Error: parameter not found" );
    return core_error_not_found;
    }

core_error_e core_wlan_eapol_if_parameter_c::get_parameter_data(
    u8_t ** const data, 
    u32_t * data_length ) const
    { 
    ASSERT( data );
    ASSERT( data_length );
    // All structured parameters must be mentioned here.
    if ( get_parameter_type() == wlan_eapol_if_message_type_variable_data
        || get_parameter_type() == wlan_eapol_if_message_type_network_id
        || get_parameter_type() == wlan_eapol_if_message_type_session_key
        || get_parameter_type() == wlan_eapol_if_message_type_network_key
        || get_parameter_type() == wlan_eapol_if_message_type_protected_setup_credential
        || get_parameter_type() == wlan_eapol_if_message_type_eap_state_notification
        || get_parameter_type() == wlan_eapol_if_message_type_array )
        {
        *data = &data_m[WLAN_EAPOL_MESSAGE_IF_DATA_OFFSET];
        *data_length = get_parameter_length();
        // If data length is zero, it is good to set also data pointer to NULL.
        if ( *data_length == 0 )
            {
            *data = NULL;
            }
        return core_error_ok;
        }
    *data = NULL;
    *data_length = 0;
    DEBUG( "core_wlan_eapol_if_parameter_c::get_parameter_data( non-basic types ) - Error: parameter not found" );
    return core_error_not_found;
    }


core_error_e core_wlan_eapol_if_parameter_c::get_parameter_data(
    u32_t * vendor_id, 
    u32_t * vendor_type ) const
    { 
    ASSERT( vendor_id );
    ASSERT( vendor_type );
    wlan_eapol_if_message_type_e type = get_parameter_type();
    if ( type == wlan_eapol_if_message_type_eap_type )
        {
        *vendor_id = core_tools_c::get_u32_big_endian(
            &data_m[0],
            WLAN_EAPOL_MESSAGE_IF_DATA_OFFSET );
        
        if( ( *vendor_id >> 24 ) == 254 )
            {
            *vendor_id &= 0x00FFFFFF; // Mask EAP-type constant (254) out
	        
            *vendor_type = core_tools_c::get_u32_big_endian(
                &data_m[0],
                WLAN_EAPOL_MESSAGE_IF_DATA_OFFSET + sizeof( vendor_id ) );
            }
        else // Not expanded EAP type, in this case EAP type is first byte of vendor_id
            {
            *vendor_type = ( *vendor_id  >> 24 );
            *vendor_id = 0;
            }
        
        return core_error_ok;
        }
    *vendor_id = 0;
    *vendor_type = 0;
    DEBUG( "core_wlan_eapol_if_parameter_c::get_parameter_data( vendor_id, vendor_type ) - Error: parameter not found" );
    return core_error_not_found;
    }



// ============================================================================


core_error_e core_wlan_eapol_if_parameter_c::reserve_buffer(
    u32_t needed_buffer_length )
    {
    if ( needed_buffer_length <= buffer_length_m )
        {
        return core_error_ok;
        }
    
    DEBUG2( "core_wlan_eapol_if_parameter_c::reserve_buffer - increase buffer %i->%i bytes", buffer_length_m, needed_buffer_length );
    u8_t* p = new u8_t[needed_buffer_length];
    if ( !p )
        {
        DEBUG( "core_wlan_eapol_if_parameter_c::reserve_buffer - Error: No enough memory!" );
        return core_error_no_memory;
        }
    core_tools_c::fillz( p, needed_buffer_length );
    core_tools_c::copy( 
        p,
        &data_m[0],
        buffer_length_m );
    core_tools_c::fillz( &data_m[0], buffer_length_m );
    
    if ( delete_buffer_m )
        {
        delete[] data_m;
        }
    delete_buffer_m = true_t;
    data_m = p;
    buffer_length_m = needed_buffer_length;
    
    return core_error_ok;
    }


void core_wlan_eapol_if_parameter_c::add_header(
    wlan_eapol_if_message_type_e type, 
    u32_t length ) 
    {
    // Type
    u32_t type_value( type );
    type_value = core_tools_c::convert_host_to_big_endian( type_value );
    core_tools_c::copy( 
        &data_m[0], 
        &type_value, 
        sizeof( type_value ) );
    COMPILE_ASSERT( sizeof( type_value ) == 4); // Specification says this.
    
    // Length
    u32_t parameter_length( length );
    parameter_length = core_tools_c::convert_host_to_big_endian( parameter_length );
    core_tools_c::copy( 
        &data_m[WLAN_EAPOL_MESSAGE_IF_LENGTH_OFFSET],
        &parameter_length, 
        sizeof( parameter_length ) );
    COMPILE_ASSERT( sizeof( parameter_length ) == 4); // Specification says this.
    }


u64_t core_wlan_eapol_if_parameter_c::get_parameter_u64_t() const
    {
    u64_t value = core_tools_c::get_u32_big_endian(
        &data_m[0],
        WLAN_EAPOL_MESSAGE_IF_DATA_OFFSET );
    value <<= 32;
    value += core_tools_c::get_u32_big_endian(
        &data_m[0],
        WLAN_EAPOL_MESSAGE_IF_DATA_OFFSET+4 );
        
    return value;
    }



void core_wlan_eapol_if_parameter_c::add_parameter_u32_t(
    u32_t data )
    {
    u32_t parameter_data( data );
    parameter_data = core_tools_c::convert_host_to_big_endian( parameter_data );
    core_tools_c::copy( 
        &data_m[WLAN_EAPOL_MESSAGE_IF_DATA_OFFSET], 
        &parameter_data, 
        sizeof( parameter_data ) );
    COMPILE_ASSERT( sizeof( parameter_data ) == 4); // Specification says this.
    }

u32_t core_wlan_eapol_if_parameter_c::get_parameter_u32_t() const
    {
    return core_tools_c::get_u32_big_endian(
        &data_m[0],
        WLAN_EAPOL_MESSAGE_IF_DATA_OFFSET );
    }

void core_wlan_eapol_if_parameter_c::add_parameter_u16_t(
    u16_t data )
    {
    u16_t parameter_data( data );
    parameter_data = core_tools_c::convert_host_to_big_endian( parameter_data );
    core_tools_c::copy( 
        &data_m[WLAN_EAPOL_MESSAGE_IF_DATA_OFFSET], 
        &parameter_data, 
        sizeof( parameter_data ) );
    COMPILE_ASSERT( sizeof( data ) == 2); // Specification says this.
    }

u16_t core_wlan_eapol_if_parameter_c::get_parameter_u16_t() const
    {
    return core_tools_c::get_u16_big_endian(
        &data_m[0],
        WLAN_EAPOL_MESSAGE_IF_DATA_OFFSET );
    }

void core_wlan_eapol_if_parameter_c::add_parameter_u8_t(
    u8_t data ) 
    {
    data_m[WLAN_EAPOL_MESSAGE_IF_DATA_OFFSET] = data;
    COMPILE_ASSERT( sizeof( data ) == 1 ); // Specification says this.
    }

u8_t core_wlan_eapol_if_parameter_c::get_parameter_u8_t() const
    {
    return data_m[WLAN_EAPOL_MESSAGE_IF_DATA_OFFSET];
    }















/**
 * Constructor
 */
core_wlan_eapol_if_function_c::core_wlan_eapol_if_function_c()
    : data_m( NULL )
    , data_length_m( 0 )
    , buffer_length_m( 0 )
    , delete_buffer_m( false_t)
    , iter_m( 0 )
    , current_parameter_m( )
    {
    DEBUG( "core_wlan_eapol_if_function_c::core_wlan_eapol_if_function_c()" );
    }

/**
 * Constructor
 *
 * @param data Pointer to the IE data.
 * @param data_length Length of the IE data.
 */
core_wlan_eapol_if_function_c::core_wlan_eapol_if_function_c(
    u8_t * data, 
    const u32_t length )
    : data_m( data )
    , data_length_m( length )
    , buffer_length_m( 0 )
    , delete_buffer_m( false_t)
    , iter_m( 0 )
    , current_parameter_m( )
    {
    // Update current_parameter_m
    first();
    }

/** 
 * Destructor.
 */
core_wlan_eapol_if_function_c::~core_wlan_eapol_if_function_c()
    {
    if ( delete_buffer_m )
        {
        core_tools_c::fillz( data_m, buffer_length_m );
        delete[] data_m;
        data_m = NULL;
        }
    }

wlan_eapol_if_message_type_e core_wlan_eapol_if_function_c::get_type()
    {
    first();
    return static_cast<wlan_eapol_if_message_type_e>( current()->get_parameter_type() );
    }

wlan_eapol_if_message_type_function_e core_wlan_eapol_if_function_c::get_function()
    {
    first();
    
    u32_t function;
    core_error_e error = current()->get_parameter_data( &function );
    if ( error != core_error_ok )
        {
        return wlan_eapol_if_message_type_function_none;
        }
    return static_cast<wlan_eapol_if_message_type_function_e>( function );
    }

core_error_e core_wlan_eapol_if_function_c::append(
    const core_wlan_eapol_if_parameter_c * const param )
    {
    ASSERT( param != NULL );
    core_error_e error = reserve_buffer( data_length_m + param->size() );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    core_tools_c::copy( 
        &data_m[data_length_m],
        param->get_data(), 
        param->size() );
    
    data_length_m += param->size();
    return core_error_ok;
    }

u32_t core_wlan_eapol_if_function_c::size() const
    {
    return data_length_m;
    }


u8_t* core_wlan_eapol_if_function_c::get_data()
    { 
    return &data_m[0];
    }

void core_wlan_eapol_if_function_c::clear()
    {
    core_tools_c::fillz( data_m, buffer_length_m );
    data_length_m = 0;
    first();
    }

// ============================================================================

// Iterators
void core_wlan_eapol_if_function_c::first()
    {
    // Update current_parameter_m
    iter_m = 0;
    current_parameter_m.update( &data_m[iter_m], data_length_m - iter_m );
    }

void core_wlan_eapol_if_function_c::next()
    {
    // Update current_parameter_m
    iter_m += current_parameter_m.size();
    current_parameter_m.update( &data_m[iter_m], data_length_m - iter_m );
    
    return; // ok
    }

const core_wlan_eapol_if_parameter_c* core_wlan_eapol_if_function_c::current()
    {
    return &current_parameter_m;
    }

bool_t core_wlan_eapol_if_function_c::is_done() const
    {
    return ( iter_m >= data_length_m );
    }

// ============================================================================

core_error_e core_wlan_eapol_if_function_c::reserve_buffer(
    u32_t needed_buffer_length )
    {
    if ( needed_buffer_length <= buffer_length_m )
        {
        return core_error_ok;
        }
    
    DEBUG2( "core_wlan_eapol_if_function_c::reserve_buffer - increase buffer %i->%i bytes", buffer_length_m, needed_buffer_length );
    u8_t* p = new u8_t[needed_buffer_length];
    if ( !p )
        {
        DEBUG( "core_wlan_eapol_if_function_c::reserve_buffer - Error: No enough memory!" );
        return core_error_no_memory;
        }
    core_tools_c::fillz( p, needed_buffer_length );
    core_tools_c::copy( 
        p,
        &data_m[0],
        data_length_m );
    core_tools_c::fillz( &data_m[0], buffer_length_m );
    
    if ( delete_buffer_m )
        {
        delete[] data_m;
        }
    delete_buffer_m = true_t;
    data_m = p;
    buffer_length_m = needed_buffer_length;
    
    return core_error_ok;
    }



// Higher level methods to handle structured parameters and functions

core_error_e core_wlan_eapol_if_function_c::generate_network_id(
    const network_id_c * network_id )
    {
    DEBUG( "core_wlan_eapol_if_function_c::generate_network_id()" );
    ASSERT( network_id != NULL);
    
    core_wlan_eapol_if_parameter_c source_parameter;
    core_wlan_eapol_if_parameter_c destination_parameter;
    core_wlan_eapol_if_parameter_c packet_type_parameter;
    core_wlan_eapol_if_parameter_c network_identity;
    

    core_error_e error = source_parameter.set_parameter_data( wlan_eapol_if_message_type_variable_data, network_id->source_ptr, network_id->source_length );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = destination_parameter.set_parameter_data( wlan_eapol_if_message_type_variable_data, network_id->destination_ptr, network_id->destination_length );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = packet_type_parameter.set_parameter_data( wlan_eapol_if_message_type_u16_t, network_id->packet_type );
    if ( error != core_error_ok )
        {
        return error;
        }

    core_wlan_eapol_if_function_c message;
    
    error = message.append( &source_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = message.append( &destination_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = message.append( &packet_type_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    error = network_identity.set_parameter_data( wlan_eapol_if_message_type_network_id, message.get_data(), message.size() );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    return append( &network_identity );
    }




core_error_e core_wlan_eapol_if_function_c::parse_network_id(
    network_id_c * network_id )
    {
    DEBUG( "core_wlan_eapol_if_function_c::parse_network_id()" );
    ASSERT( network_id );
    
    if ( is_done() || current()->get_parameter_type() != wlan_eapol_if_message_type_network_id )
        {
        return core_error_not_found;
        }

    u8_t* network_id_data( NULL );
    u32_t network_id_data_length( 0 );

    core_error_e error = current()->get_parameter_data( &network_id_data, &network_id_data_length );
    if ( error != core_error_ok )
        {
        return error;
        }

    core_wlan_eapol_if_function_c parsed_nwid_struct( network_id_data, network_id_data_length );


    // Source
    parsed_nwid_struct.first();
    if ( parsed_nwid_struct.current()->get_parameter_type() != wlan_eapol_if_message_type_variable_data )
        {
        return core_error_not_found;
        }
    error = parsed_nwid_struct.current()->get_parameter_data( &network_id->source_ptr, &network_id->source_length );
    if ( error != core_error_ok )
        {
        return error;
        }
    

    // Destination
    parsed_nwid_struct.next();
    if ( parsed_nwid_struct.current()->get_parameter_type() != wlan_eapol_if_message_type_variable_data )
        {
        return core_error_not_found;
        }
    error = parsed_nwid_struct.current()->get_parameter_data( &network_id->destination_ptr, &network_id->destination_length );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    
    // Packet type
    parsed_nwid_struct.next();
    if ( parsed_nwid_struct.current()->get_parameter_type() != wlan_eapol_if_message_type_u16_t )
        {
        return core_error_not_found;
        }
    error = parsed_nwid_struct.current()->get_parameter_data( &network_id->packet_type );
    if ( error != core_error_ok )
        {
        return error;
        }

    return error;
    }



        
core_error_e core_wlan_eapol_if_function_c::generate_network_key(
    network_key_c * network_key )
    {
    DEBUG( "core_wlan_eapol_if_function_c::generate_network_key()" );
    ASSERT( network_key );
    ASSERT( network_key->network_key );

    core_wlan_eapol_if_parameter_c network_key_index_parameter;
    core_wlan_eapol_if_parameter_c network_key_parameter;
    core_wlan_eapol_if_parameter_c temp_network_key;
    

    core_error_e error = network_key_index_parameter.set_parameter_data( wlan_eapol_if_message_type_u8_t, network_key->network_key_index );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = network_key_parameter.set_parameter_data( wlan_eapol_if_message_type_variable_data, network_key->network_key, network_key->network_key_length );
    if ( error != core_error_ok )
        {
        return error;
        }

    core_wlan_eapol_if_function_c message;
    
    error = message.append( &network_key_index_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = message.append( &network_key_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }
        
    error = temp_network_key.set_parameter_data( wlan_eapol_if_message_type_network_key, message.get_data(), message.size() );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    return append( &temp_network_key );
    }




core_error_e core_wlan_eapol_if_function_c::parse_network_key(
    network_key_c * network_key )
    {
    DEBUG( "core_wlan_eapol_if_function_c::parse_network_key()" );
    ASSERT( network_key );
    
    if ( is_done() || current()->get_parameter_type() != wlan_eapol_if_message_type_network_key )
        {
        return core_error_not_found;
        }

    u8_t* network_key_data( NULL );
    u32_t network_key_data_length( 0 );

    core_error_e error = current()->get_parameter_data( &network_key_data, &network_key_data_length );
    if ( error != core_error_ok )
        {
        return error;
        }

    core_wlan_eapol_if_function_c parsed_network_key_struct( network_key_data, network_key_data_length );


    parsed_network_key_struct.first();
    if ( parsed_network_key_struct.current()->get_parameter_type() != wlan_eapol_if_message_type_u8_t )
        {
        return core_error_not_found;
        }
    error = parsed_network_key_struct.current()->get_parameter_data( &network_key->network_key_index );
    if ( error != core_error_ok )
        {
        return error;
        }
    

    parsed_network_key_struct.next();
    if ( parsed_network_key_struct.current()->get_parameter_type() != wlan_eapol_if_message_type_variable_data )
        {
        return core_error_not_found;
        }
    error = parsed_network_key_struct.current()->get_parameter_data( &network_key->network_key, &network_key->network_key_length );
    if ( error != core_error_ok )
        {
        return error;
        }

    return error;
    }







core_error_e core_wlan_eapol_if_function_c::generate_protected_setup_credential(
    protected_setup_credential_c * credential )
    {
    DEBUG( "core_wlan_eapol_if_function_c::generate_protected_setup_credential()" );
    ASSERT( credential );
    
    core_wlan_eapol_if_parameter_c network_index_parameter;
    core_wlan_eapol_if_parameter_c ssid_parameter;
    core_wlan_eapol_if_parameter_c authentication_type_parameter;
    core_wlan_eapol_if_parameter_c encryption_type_parameter;
    core_wlan_eapol_if_parameter_c network_key_list_parameter;
    core_wlan_eapol_if_parameter_c mac_address_parameter;
    
    core_wlan_eapol_if_parameter_c protected_setup_credential;
    

    core_error_e error = network_index_parameter.set_parameter_data( wlan_eapol_if_message_type_u8_t, credential->network_index );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = ssid_parameter.set_parameter_data( wlan_eapol_if_message_type_variable_data, credential->ssid, credential->ssid_length );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = authentication_type_parameter.set_parameter_data( wlan_eapol_if_message_type_u16_t, credential->authentication_type );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = encryption_type_parameter.set_parameter_data( wlan_eapol_if_message_type_u16_t, credential->encryption_type );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    // Handle array here.
    core_wlan_eapol_if_function_c array;

    credential->network_key_list.first();
    while ( credential->network_key_list.current() )
        {
        core_wlan_eapol_if_function_c network_key;
        error = network_key.generate_network_key(
            credential->network_key_list.current() );
        if ( error != core_error_ok )
            {
            return error;
            }
        core_wlan_eapol_if_parameter_c network_key_parameter( network_key.get_data(), network_key.size() );
        
        error = array.append( &network_key_parameter );
        if ( error != core_error_ok )
            {
            return error;
            }
        credential->network_key_list.next();
        }
    network_key_list_parameter.set_parameter_data( wlan_eapol_if_message_type_array, array.get_data(), array.size() );

    
    error = mac_address_parameter.set_parameter_data( wlan_eapol_if_message_type_variable_data, credential->mac_address, credential->mac_address_length );
    if ( error != core_error_ok )
        {
        return error;
        }

    core_wlan_eapol_if_function_c message;
    
    error = message.append( &network_index_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = message.append( &ssid_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = message.append( &authentication_type_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = message.append( &encryption_type_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = message.append( &network_key_list_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = message.append( &mac_address_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }
        
    error = protected_setup_credential.set_parameter_data( wlan_eapol_if_message_type_protected_setup_credential, message.get_data(), message.size() );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    return append( &protected_setup_credential );
    }
    
    
    
    
core_error_e core_wlan_eapol_if_function_c::parse_protected_setup_credential(
    protected_setup_credential_c * credential )
    {
    DEBUG( "core_wlan_eapol_if_function_c::parse_protected_setup_credential()" );
    ASSERT( credential );
    
    if ( is_done() || current()->get_parameter_type() != wlan_eapol_if_message_type_protected_setup_credential )
        {
        return core_error_not_found;
        }

    u8_t* credential_data( NULL );
    u32_t credential_data_length( 0 );

    core_error_e error = current()->get_parameter_data( &credential_data, &credential_data_length );
    if ( error != core_error_ok )
        {
        return error;
        }

    core_wlan_eapol_if_function_c parsed_credential_struct( credential_data, credential_data_length );


    parsed_credential_struct.first();
    if ( parsed_credential_struct.current()->get_parameter_type() != wlan_eapol_if_message_type_u8_t )
        {
        return core_error_not_found;
        }
    error = parsed_credential_struct.current()->get_parameter_data( &credential->network_index );
    if ( error != core_error_ok )
        {
        return error;
        }
    

    parsed_credential_struct.next();
    if ( parsed_credential_struct.current()->get_parameter_type() != wlan_eapol_if_message_type_variable_data )
        {
        return core_error_not_found;
        }
    error = parsed_credential_struct.current()->get_parameter_data( &credential->ssid, &credential->ssid_length );
    if ( error != core_error_ok )
        {
        return error;
        }


    parsed_credential_struct.next();
    if ( parsed_credential_struct.current()->get_parameter_type() != wlan_eapol_if_message_type_u16_t )
        {
        return core_error_not_found;
        }
    error = parsed_credential_struct.current()->get_parameter_data( &credential->authentication_type );
    if ( error != core_error_ok )
        {
        return error;
        }


    parsed_credential_struct.next();
    if ( parsed_credential_struct.current()->get_parameter_type() != wlan_eapol_if_message_type_u16_t )
        {
        return core_error_not_found;
        }
    error = parsed_credential_struct.current()->get_parameter_data( &credential->encryption_type );
    if ( error != core_error_ok )
        {
        return error;
        }


    parsed_credential_struct.next();
    if ( parsed_credential_struct.current()->get_parameter_type() != wlan_eapol_if_message_type_array )
        {
        return core_error_not_found;
        }
    
    u32_t data_length;
    u8_t * data;
    error = parsed_credential_struct.current()->get_parameter_data( &data, &data_length );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    core_wlan_eapol_if_function_c array( data, data_length );
    
    // Loop through array, put every parsed network_key to network_key_list.
    array.first();
    while ( !array.is_done() )
        {
        if ( array.current()->get_parameter_type() != wlan_eapol_if_message_type_network_key )
            {
            return core_error_not_found;
            }
        network_key_c * network_key = new network_key_c( 0, NULL, 0 );
        if ( !network_key )
            {
            DEBUG( "core_wlan_eapol_if_function_c::parse_protected_setup_credential() - Error: No enough memory!" );
            return core_error_no_memory;
            }
        error = array.parse_network_key( network_key );
        if ( error != core_error_ok )
            {
            delete network_key;
            network_key = NULL;
            return error;
            }
        
        error = credential->network_key_list.append( network_key );
        if ( error != core_error_ok )
            {
            delete network_key;
            network_key = NULL;
            return error;
            }
        
        array.next();
        }


    parsed_credential_struct.next();
    if ( parsed_credential_struct.current()->get_parameter_type() != wlan_eapol_if_message_type_variable_data )
        {
        return core_error_not_found;
        }
    error = parsed_credential_struct.current()->get_parameter_data( &credential->mac_address, &credential->mac_address_length );
    if ( error != core_error_ok )
        {
        return error;
        }


    return error;
    }





core_error_e core_wlan_eapol_if_function_c::generate_session_key(
    session_key_c * session_key )
    {
    DEBUG( "core_wlan_eapol_if_function_c::generate_session_key()" );
    ASSERT( session_key );
    ASSERT( session_key->key );
    ASSERT( session_key->sequence_number );
    
    core_wlan_eapol_if_parameter_c key_parameter;
    core_wlan_eapol_if_parameter_c sequence_number_parameter;
    core_wlan_eapol_if_parameter_c eapol_key_type_parameter;
    core_wlan_eapol_if_parameter_c key_index_parameter;
    core_wlan_eapol_if_parameter_c key_tx_bit_parameter;
    
    core_wlan_eapol_if_parameter_c session_key_parameter;
    

    // Generate parameters
    core_error_e error = key_parameter.set_parameter_data( wlan_eapol_if_message_type_variable_data, session_key->key, session_key->key_length);
    if ( error != core_error_ok )
        {
        return error;
        }
    error = sequence_number_parameter.set_parameter_data( wlan_eapol_if_message_type_variable_data, session_key->sequence_number, session_key->sequence_number_length);
    if ( error != core_error_ok )
        {
        return error;
        }
    error = eapol_key_type_parameter.set_parameter_data( wlan_eapol_if_message_type_eapol_key_type, session_key->eapol_key_type );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = key_index_parameter.set_parameter_data( wlan_eapol_if_message_type_u32_t, session_key->key_index );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = key_tx_bit_parameter.set_parameter_data( wlan_eapol_if_message_type_boolean, session_key->key_tx_bit );
    if ( error != core_error_ok )
        {
        return error;
        }


    // Append parameters to parameter list
    core_wlan_eapol_if_function_c message;
    
    error = message.append( &key_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = message.append( &sequence_number_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = message.append( &eapol_key_type_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = message.append( &key_index_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = message.append( &key_tx_bit_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    
    // Group parameter list to session key type
    error = session_key_parameter.set_parameter_data( wlan_eapol_if_message_type_session_key, message.get_data(), message.size() );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    return append( &session_key_parameter );
    }


core_error_e core_wlan_eapol_if_function_c::parse_session_key(
    session_key_c * session_key )
    {
    DEBUG( "core_wlan_eapol_if_function_c::parse_session_key()" );
    ASSERT( session_key );
    
    if ( is_done() || current()->get_parameter_type() != wlan_eapol_if_message_type_session_key )
        {
        return core_error_not_found;
        }

    u8_t* session_key_data( NULL );
    u32_t session_key_data_length( 0 );

    core_error_e error = current()->get_parameter_data( &session_key_data, &session_key_data_length );
    if ( error != core_error_ok )
        {
        return error;
        }

    core_wlan_eapol_if_function_c parsed_session_key_struct( session_key_data, session_key_data_length );


    // Source
    parsed_session_key_struct.first();
    if ( parsed_session_key_struct.current()->get_parameter_type() != wlan_eapol_if_message_type_variable_data )
        {
        return core_error_not_found;
        }
    error = parsed_session_key_struct.current()->get_parameter_data( &session_key->key, &session_key->key_length );
    if ( error != core_error_ok )
        {
        return error;
        }
    

    // Sequence number
    parsed_session_key_struct.next();
    if ( parsed_session_key_struct.current()->get_parameter_type() != wlan_eapol_if_message_type_variable_data )
        {
        return core_error_not_found;
        }
    error = parsed_session_key_struct.current()->get_parameter_data( &session_key->sequence_number, &session_key->sequence_number_length );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    
    // EAPOL key type
    parsed_session_key_struct.next();
    if ( parsed_session_key_struct.current()->get_parameter_type() != wlan_eapol_if_message_type_eapol_key_type )
        {
        return core_error_not_found;
        }
    u32_t eapol_key_type( 0 );
    error = parsed_session_key_struct.current()->get_parameter_data( &eapol_key_type );
    if ( error != core_error_ok )
        {
        return error;
        }
    session_key->eapol_key_type = static_cast<wlan_eapol_if_eapol_key_type_e>( eapol_key_type );
    
    // Key index
    parsed_session_key_struct.next();
    if ( parsed_session_key_struct.current()->get_parameter_type() != wlan_eapol_if_message_type_u32_t )
        {
        return core_error_not_found;
        }
    error = parsed_session_key_struct.current()->get_parameter_data( &session_key->key_index );
    if ( error != core_error_ok )
        {
        return error;
        }

    // Key TX bit
    parsed_session_key_struct.next();
    if ( parsed_session_key_struct.current()->get_parameter_type() != wlan_eapol_if_message_type_boolean )
        {
        return core_error_not_found;
        }
    error = parsed_session_key_struct.current()->get_parameter_data( &session_key->key_tx_bit );
    if ( error != core_error_ok )
        {
        return error;
        }

    return error;
    }





core_error_e core_wlan_eapol_if_function_c::generate_eap_state_notification(
    state_notification_c * state_notification )
    {
    DEBUG( "core_wlan_eapol_if_function_c::generate_eap_state_notification()" );
    ASSERT( state_notification );

    core_wlan_eapol_if_parameter_c eap_state_notification;
    
    core_wlan_eapol_if_parameter_c protocol_layer_parameter;
    core_wlan_eapol_if_parameter_c protocol_parameter;
    core_wlan_eapol_if_parameter_c eap_type_parameter;
    core_wlan_eapol_if_parameter_c current_state_parameter;
    core_wlan_eapol_if_parameter_c is_client_parameter;
    core_wlan_eapol_if_parameter_c authentication_error_parameter;

    
    // Generate parameters
    core_wlan_eapol_if_function_c network_id;
    core_error_e error = network_id.generate_network_id(
        &state_notification->network_id );
    if ( error != core_error_ok )
        {
        return error;
        }
    core_wlan_eapol_if_parameter_c network_id_parameter( network_id.get_data(), network_id.size() );

    

    error = protocol_layer_parameter.set_parameter_data( wlan_eapol_if_message_type_eap_protocol_layer, state_notification->protocol_layer );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = protocol_parameter.set_parameter_data( wlan_eapol_if_message_type_u32_t, state_notification->protocol);
    if ( error != core_error_ok )
        {
        return error;
        }
    error = eap_type_parameter.set_parameter_data( wlan_eapol_if_message_type_eap_type, state_notification->eap_type_vendor_id, state_notification->eap_type_vendor_type );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = current_state_parameter.set_parameter_data( wlan_eapol_if_message_type_u32_t, state_notification->current_state );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = is_client_parameter.set_parameter_data( wlan_eapol_if_message_type_boolean, state_notification->is_client );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = authentication_error_parameter.set_parameter_data( wlan_eapol_if_message_type_eap_status, state_notification->authentication_error );
    if ( error != core_error_ok )
        {
        return error;
        }

    // Append parameters to parameter list
    core_wlan_eapol_if_function_c message;
    
    error = message.append( &network_id_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = message.append( &protocol_layer_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = message.append( &protocol_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = message.append( &eap_type_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = message.append( &current_state_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = message.append( &is_client_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = message.append( &authentication_error_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    
    // Group parameter list to EAP state notification type
    error = eap_state_notification.set_parameter_data( wlan_eapol_if_message_type_eap_state_notification, message.get_data(), message.size() );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    return append( &eap_state_notification );
    }


core_error_e core_wlan_eapol_if_function_c::parse_eap_state_notification(
    state_notification_c * state_notification )
    {
    DEBUG( "core_wlan_eapol_if_function_c::parse_eap_state_notification()" );
    ASSERT( state_notification );
    
    if ( is_done() || current()->get_parameter_type() != wlan_eapol_if_message_type_eap_state_notification )
        {
        return core_error_not_found;
        }

    u8_t* eap_state_notification_data( NULL );
    u32_t eap_state_notification_data_length( 0 );

    core_error_e error = current()->get_parameter_data( &eap_state_notification_data, &eap_state_notification_data_length );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    core_wlan_eapol_if_function_c parsed_state_notification_struct( eap_state_notification_data, eap_state_notification_data_length );

    
    // Network id
    parsed_state_notification_struct.first();
    error = parsed_state_notification_struct.parse_network_id(
        &state_notification->network_id );
    if ( error != core_error_ok )
        {
        return error;
        }
    

    // Protocol layer
    parsed_state_notification_struct.next();
    if ( parsed_state_notification_struct.current()->get_parameter_type() != wlan_eapol_if_message_type_eap_protocol_layer )
    {
        return core_error_not_found;
    }
    u32_t temp_protocol_layer( 0 );
    error = parsed_state_notification_struct.current()->get_parameter_data( &temp_protocol_layer );
    if ( error != core_error_ok )
        {
        return error;
        }
    state_notification->protocol_layer = static_cast<wlan_eapol_if_eap_protocol_layer_e>( temp_protocol_layer );
    
    
    // Protocol
    parsed_state_notification_struct.next();
    if ( parsed_state_notification_struct.current()->get_parameter_type() != wlan_eapol_if_message_type_u32_t )
        {
        return core_error_not_found;
        }
    error = parsed_state_notification_struct.current()->get_parameter_data( &state_notification->protocol );
    if ( error != core_error_ok )
        {
        return error;
        }

    // EAP-Type
    parsed_state_notification_struct.next();
    if ( parsed_state_notification_struct.current()->get_parameter_type() != wlan_eapol_if_message_type_eap_type )
        {
        return core_error_not_found;
        }
    error = parsed_state_notification_struct.current()->get_parameter_data( &state_notification->eap_type_vendor_id, &state_notification->eap_type_vendor_type );
    if ( error != core_error_ok )
        {
        return error;
        }

    // Current state
    parsed_state_notification_struct.next();
    if ( parsed_state_notification_struct.current()->get_parameter_type() != wlan_eapol_if_message_type_u32_t )
        {
        return core_error_not_found;
        }
    error = parsed_state_notification_struct.current()->get_parameter_data( &state_notification->current_state );
    if ( error != core_error_ok )
        {
        return error;
        }

    // Is client
    parsed_state_notification_struct.next();
    if ( parsed_state_notification_struct.current()->get_parameter_type() != wlan_eapol_if_message_type_boolean )
        {
        return core_error_not_found;
        }
    error = parsed_state_notification_struct.current()->get_parameter_data( &state_notification->is_client );
    if ( error != core_error_ok )
        {
        return error;
        }
        
    // Authentication error
    parsed_state_notification_struct.next();
    if ( parsed_state_notification_struct.current()->get_parameter_type() != wlan_eapol_if_message_type_eap_status )
        {
        return core_error_not_found;
        }
    u32_t temp_authentication_error( 0 );
    error = parsed_state_notification_struct.current()->get_parameter_data( &temp_authentication_error );
    if ( error != core_error_ok )
        {
        return error;
        }
    state_notification->authentication_error = static_cast<wlan_eapol_if_eap_status_e>( temp_authentication_error );
    return error;
    }


void core_wlan_eapol_if_function_c::debug_print()
    {
#ifdef _DEBUG
    DEBUG( "  -> debug_print()" );

    static const bool wlan_eapol_if_message_type_is_basic_type[] = 
        {
        true,  //"none",
        false, //"array",
        true,  //"boolean",
        true,  //"eap_protocol_layer",
        false, //"eap_state_notification",
        true,  //"eap_status",
        true,  //"eap_type",
        true,  //"eapol_key_802_11_authentication_mode",
        true,  //"eapol_key_authentication_type",
        true,  //"eapol_key_type",
        true,  //"eapol_tkip_mic_failure_type",
        true,  //"eapol_wlan_authentication_state",
        true,  //"error",
        true,  //"function",
        false, //"network_id",
        false, //"network_key",
        false, //"protected_setup_credential",
        true,  //"RSNA_cipher",
        false, //"session_key",
        true,  //"u8_t",
        true,  //"u16_t",
        true,  //"u32_t",
        true,  //"u64_t",
        true,  //"variable_data",
        };
    
    first();
    while ( !is_done() )
        {
        wlan_eapol_if_message_type_e type = current()->get_parameter_type();
        debug_print_type_string( type, current()->get_parameter_length() );
        
        // Basic types are printed now. Structured types are parsed recursively.
        if ( wlan_eapol_if_message_type_is_basic_type[ type ])
            {
            // Special handling for functions
            if ( current()->get_parameter_type() == wlan_eapol_if_message_type_function )
                {
                u32_t func(0);
                core_error_e error = current()->get_parameter_data( &func );
                if ( error != core_error_ok )
                    {
                    return;
                    }
                debug_print_function_string( static_cast<wlan_eapol_if_message_type_function_e>( func ) );
                }
            else
                {
                DEBUG( "Data:" );
                DEBUG_BUFFER( current()->size()-WLAN_EAPOL_MESSAGE_IF_DATA_OFFSET, 
                    current()->get_data()+WLAN_EAPOL_MESSAGE_IF_DATA_OFFSET );
                }
            }
        else
            {
            u8_t* data( NULL );
            u32_t data_length( 0 );
            core_error_e error = current()->get_parameter_data( &data, &data_length );
            if ( error != core_error_ok )
                {
                return;
                }
            
            core_wlan_eapol_if_function_c sub_message( data, data_length );
            sub_message.debug_print();
            }
        next();
        }
    DEBUG( "  <- debug_print()" );
#endif // _DEBUG
    }


#ifdef _DEBUG

void core_wlan_eapol_if_function_c::debug_print_type_string(
    wlan_eapol_if_message_type_e type, 
    u32_t length )
    {
    switch ( type )
        {
        case wlan_eapol_if_message_type_none                                : DEBUG2( "Type %i: none, length %i", type, length ); break;
        case wlan_eapol_if_message_type_array                               : DEBUG2( "Type %i: array, length %i", type, length ); break;
        case wlan_eapol_if_message_type_boolean                             : DEBUG2( "Type %i: boolean, length %i", type, length ); break;
        case wlan_eapol_if_message_type_eap_protocol_layer                  : DEBUG2( "Type %i: eap_protocol_layer, length %i", type, length ); break;
        case wlan_eapol_if_message_type_eap_state_notification              : DEBUG2( "Type %i: eap_state_notification, length %i", type, length ); break;
        case wlan_eapol_if_message_type_eap_status                          : DEBUG2( "Type %i: eap_status, length %i", type, length ); break;
        case wlan_eapol_if_message_type_eap_type                            : DEBUG2( "Type %i: eap_type, length %i", type, length ); break;
        case wlan_eapol_if_message_type_eapol_key_802_11_authentication_mode: DEBUG2( "Type %i: eapol_key_802_11_authentication_mode, length %i", type, length ); break;
        case wlan_eapol_if_message_type_eapol_key_authentication_type       : DEBUG2( "Type %i: eapol_key_authentication_type, length %i", type, length ); break;
        case wlan_eapol_if_message_type_eapol_key_type                      : DEBUG2( "Type %i: eapol_key_type, length %i", type, length ); break;
        case wlan_eapol_if_message_type_eapol_tkip_mic_failure_type         : DEBUG2( "Type %i: eapol_tkip_mic_failure_type, length %i", type, length ); break;
        case wlan_eapol_if_message_type_eapol_wlan_authentication_state     : DEBUG2( "Type %i: eapol_wlan_authentication_state, length %i", type, length ); break;
        case wlan_eapol_if_message_type_error                               : DEBUG2( "Type %i: error, length %i", type, length ); break;
        case wlan_eapol_if_message_type_function                            : DEBUG2( "Type %i: function, length %i", type, length ); break;
        case wlan_eapol_if_message_type_network_id                          : DEBUG2( "Type %i: network_id, length %i", type, length ); break;
        case wlan_eapol_if_message_type_network_key                         : DEBUG2( "Type %i: network_key, length %i", type, length ); break;
        case wlan_eapol_if_message_type_protected_setup_credential          : DEBUG2( "Type %i: protected_setup_credential, length %i", type, length ); break;
        case wlan_eapol_if_message_type_RSNA_cipher                         : DEBUG2( "Type %i: RSNA_cipher, length %i", type, length ); break;
        case wlan_eapol_if_message_type_session_key                         : DEBUG2( "Type %i: session_key, length %i", type, length ); break;
        case wlan_eapol_if_message_type_u8_t                                : DEBUG2( "Type %i: u8_t, length %i", type, length ); break;
        case wlan_eapol_if_message_type_u16_t                               : DEBUG2( "Type %i: u16_t, length %i", type, length ); break;
        case wlan_eapol_if_message_type_u32_t                               : DEBUG2( "Type %i: u32_t, length %i", type, length ); break;
        case wlan_eapol_if_message_type_u64_t                               : DEBUG2( "Type %i: u64_t, length %i", type, length ); break;
        case wlan_eapol_if_message_type_variable_data                       : DEBUG2( "Type %i: variable_data, length %i", type, length ); break;
        default: DEBUG1( "Illegal type number (%i)", type );
        }
    }

void core_wlan_eapol_if_function_c::debug_print_function_string(
    wlan_eapol_if_message_type_function_e function )
    {
    switch ( function )
        {
        case wlan_eapol_if_message_type_function_none                                 : DEBUG1( "Function %i: none", function ); break;
        case wlan_eapol_if_message_type_function_check_pmksa_cache                    : DEBUG1( "Function %i: check_pmksa_cache", function  ); break;
        case wlan_eapol_if_message_type_function_start_authentication                 : DEBUG1( "Function %i: start_authentication", function ); break;
        case wlan_eapol_if_message_type_function_complete_association                 : DEBUG1( "Function %i: complete_association", function ); break;
        case wlan_eapol_if_message_type_function_disassociation                       : DEBUG1( "Function %i: disassociation", function ); break;
        case wlan_eapol_if_message_type_function_start_preauthentication              : DEBUG1( "Function %i: start_preauthentication", function ); break;
        case wlan_eapol_if_message_type_function_start_reassociation                  : DEBUG1( "Function %i: start_reassociation", function ); break;
        case wlan_eapol_if_message_type_function_complete_reassociation               : DEBUG1( "Function %i: complete_reassociation", function ); break;
        case wlan_eapol_if_message_type_function_start_wpx_fast_roam_reassociation    : DEBUG1( "Function %i: start_wpx_fast_roam_reassociation", function ); break;
        case wlan_eapol_if_message_type_function_complete_wpx_fast_roam_reassociation : DEBUG1( "Function %i: complete_wpx_fast_roam_reassociation", function ); break;
        case wlan_eapol_if_message_type_function_packet_process                       : DEBUG1( "Function %i: packet_process", function ); break;
        case wlan_eapol_if_message_type_function_tkip_mic_failure                     : DEBUG1( "Function %i: tkip_mic_failure", function ); break;
        case wlan_eapol_if_message_type_function_eap_acknowledge                      : DEBUG1( "Function %i: eap_acknowledge", function ); break;
        case wlan_eapol_if_message_type_function_update_header_offset                 : DEBUG1( "Function %i: update_header_offset", function ); break;
        case wlan_eapol_if_message_type_function_complete_check_pmksa_cache           : DEBUG1( "Function %i: complete_check_pmksa_cache", function ); break;
        case wlan_eapol_if_message_type_function_packet_send                          : DEBUG1( "Function %i: packet_send", function ); break;
        case wlan_eapol_if_message_type_function_associate                            : DEBUG1( "Function %i: associate", function ); break;
        case wlan_eapol_if_message_type_function_disassociate                         : DEBUG1( "Function %i: disassociate", function ); break;
        case wlan_eapol_if_message_type_function_packet_data_session_key              : DEBUG1( "Function %i: packet_data_session_key", function ); break;
        case wlan_eapol_if_message_type_function_state_notification                   : DEBUG1( "Function %i: state_notification", function ); break;
        case wlan_eapol_if_message_type_function_reassociate                          : DEBUG1( "Function %i: reassociate", function ); break;
        case wlan_eapol_if_message_type_function_update_wlan_database_reference_values: DEBUG1( "Function %i: update_wlan_database_reference_values", function ); break;
        case wlan_eapol_if_message_type_function_complete_start_wpx_fast_roam_reassociation : DEBUG1( "Function %i: complete_start_wpx_fast_roam_reassociation", function ); break;
        case wlan_eapol_if_message_type_function_new_protected_setup_credentials      : DEBUG1( "Function %i: new_protected_setup_credentials", function ); break;
        default: DEBUG1( "Illegal function number (%i)", function );
        }
    }

#else

void core_wlan_eapol_if_function_c::debug_print_type_string(
    wlan_eapol_if_message_type_e /* type */, 
    u32_t /* length */ )
    {
    }

void core_wlan_eapol_if_function_c::debug_print_function_string(
    wlan_eapol_if_message_type_function_e /* function */ )
    {
    }

#endif // _DEBUG


// ============================================================================

core_error_e core_wlan_eapol_if_function_c::check_pmksa_cache(
    core_type_list_c<network_id_c> & network_id_list,
    const wlan_eapol_if_eapol_key_authentication_type_e selected_eapol_key_authentication_type,
    const wlan_eapol_if_rsna_cipher_e pairwise_key_cipher_suite,
    const wlan_eapol_if_rsna_cipher_e group_key_cipher_suite )
    {
    DEBUG( "core_wlan_eapol_if_function_c::check_pmksa_cache()" );
    clear();
    
    core_error_e error;
    core_wlan_eapol_if_parameter_c function;
    error = function.set_parameter_data( wlan_eapol_if_message_type_function, wlan_eapol_if_message_type_function_check_pmksa_cache );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    core_wlan_eapol_if_parameter_c key_authentication_type_parameter;
    core_wlan_eapol_if_parameter_c pairwise_key_cipher_suite_parameter;
    core_wlan_eapol_if_parameter_c group_key_cipher_suite_parameter;

    // Generate parameters

    // Generate array
    core_wlan_eapol_if_function_c network_id_array;
    core_wlan_eapol_if_parameter_c network_id_array_parameter;
    
    network_id_list.first();
    while ( network_id_list.current() )
        {
        core_wlan_eapol_if_function_c network_id;
        error = network_id.generate_network_id(
            network_id_list.current() );
        if ( error != core_error_ok )
            {
            return error;
            }
        core_wlan_eapol_if_parameter_c network_id_parameter( network_id.get_data(), network_id.size() );
    
        error = network_id_array.append( &network_id_parameter );
        if ( error != core_error_ok )
            {
            return error;
            }
        network_id_list.next();
        }
    
    network_id_array_parameter.set_parameter_data( wlan_eapol_if_message_type_array, network_id_array.get_data(), network_id_array.size() );
    

    error = key_authentication_type_parameter.set_parameter_data( wlan_eapol_if_message_type_eapol_key_authentication_type, selected_eapol_key_authentication_type );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = pairwise_key_cipher_suite_parameter.set_parameter_data( wlan_eapol_if_message_type_RSNA_cipher, pairwise_key_cipher_suite);
    if ( error != core_error_ok )
        {
        return error;
        }
    error = group_key_cipher_suite_parameter.set_parameter_data( wlan_eapol_if_message_type_RSNA_cipher, group_key_cipher_suite );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    
    // Append parameters to parameter list
    
    error = append( &function );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = append( &network_id_array_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = append( &key_authentication_type_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = append( &pairwise_key_cipher_suite_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = append( &group_key_cipher_suite_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    debug_print();
    
    return error;
    }





core_error_e core_wlan_eapol_if_function_c::start_authentication(
    u8_t * ssid, const u32_t ssid_length, 
    const wlan_eapol_if_eapol_key_authentication_type_e selected_eapol_key_authentication_type,
    u8_t * wpa_psk, const u32_t wpa_psk_length, 
    const bool_t wpa_override_enabled,
    const network_id_c * receive_network_id )
    {
    DEBUG( "core_wlan_eapol_if_function_c::start_authentication()" );
    clear();
    
    core_error_e error;
    core_wlan_eapol_if_parameter_c function;
    error = function.set_parameter_data( wlan_eapol_if_message_type_function, wlan_eapol_if_message_type_function_start_authentication );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    core_wlan_eapol_if_parameter_c ssid_parameter;
    core_wlan_eapol_if_parameter_c key_authentication_type_parameter;
    core_wlan_eapol_if_parameter_c wpa_psk_parameter;
    core_wlan_eapol_if_parameter_c wpa_override_enabled_parameter;

    
    // Generate parameters
    error = ssid_parameter.set_parameter_data( wlan_eapol_if_message_type_variable_data, ssid, ssid_length );
    if ( error != core_error_ok )
        {
        return error;
        }

    error = key_authentication_type_parameter.set_parameter_data( wlan_eapol_if_message_type_eapol_key_authentication_type, selected_eapol_key_authentication_type );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = wpa_psk_parameter.set_parameter_data( wlan_eapol_if_message_type_variable_data, wpa_psk, wpa_psk_length );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = wpa_override_enabled_parameter.set_parameter_data( wlan_eapol_if_message_type_boolean, wpa_override_enabled );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    core_wlan_eapol_if_function_c network_id;
    error = network_id.generate_network_id(
            receive_network_id );
    if ( error != core_error_ok )
        {
        return error;
        }
    core_wlan_eapol_if_parameter_c network_id_parameter( network_id.get_data(), network_id.size() );
    
    // Append parameters to parameter list
    
    error = append( &function );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = append( &ssid_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = append( &key_authentication_type_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = append( &wpa_psk_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = append( &wpa_override_enabled_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = append( &network_id_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    debug_print();
    
    return error;
    }


core_error_e core_wlan_eapol_if_function_c::complete_association(
    const wlan_eapol_if_eapol_wlan_authentication_state_e association_result,
    const network_id_c * receive_network_id,
    u8_t * received_wpa_ie, const u32_t received_wpa_ie_length,
    u8_t * sent_wpa_ie, const u32_t sent_wpa_ie_length,
    const wlan_eapol_if_rsna_cipher_e pairwise_key_cipher_suite,
    const wlan_eapol_if_rsna_cipher_e group_key_cipher_suite )
    {
    DEBUG( "core_wlan_eapol_if_function_c::complete_association()" );
    clear();
    
    core_error_e error;
    core_wlan_eapol_if_parameter_c function;
    error = function.set_parameter_data( wlan_eapol_if_message_type_function, wlan_eapol_if_message_type_function_complete_association );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    core_wlan_eapol_if_parameter_c association_result_parameter;
    core_wlan_eapol_if_parameter_c received_wpa_ie_parameter;
    core_wlan_eapol_if_parameter_c sent_wpa_ie_parameter;
    core_wlan_eapol_if_parameter_c pairwise_key_cipher_suite_parameter;
    core_wlan_eapol_if_parameter_c group_key_cipher_suite_parameter;
    
    
    // Generate parameters
    error = association_result_parameter.set_parameter_data( wlan_eapol_if_message_type_eapol_wlan_authentication_state, association_result );
    if ( error != core_error_ok )
        {
        return error;
        }

    core_wlan_eapol_if_function_c network_id;
    error = network_id.generate_network_id(
        receive_network_id );
    if ( error != core_error_ok )
        {
        return error;
        }
    core_wlan_eapol_if_parameter_c network_id_parameter( network_id.get_data(), network_id.size() );



    error = received_wpa_ie_parameter.set_parameter_data( wlan_eapol_if_message_type_variable_data, received_wpa_ie, received_wpa_ie_length );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = sent_wpa_ie_parameter.set_parameter_data( wlan_eapol_if_message_type_variable_data, sent_wpa_ie, sent_wpa_ie_length );
    if ( error != core_error_ok )
        {
        return error;
        }

    error = pairwise_key_cipher_suite_parameter.set_parameter_data( wlan_eapol_if_message_type_RSNA_cipher, pairwise_key_cipher_suite );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = group_key_cipher_suite_parameter.set_parameter_data( wlan_eapol_if_message_type_RSNA_cipher, group_key_cipher_suite );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    
    // Append parameters to parameter list
    
    error = append( &function );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = append( &association_result_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = append( &network_id_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = append( &received_wpa_ie_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = append( &sent_wpa_ie_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = append( &pairwise_key_cipher_suite_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = append( &group_key_cipher_suite_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    debug_print();
    
    return error;
    }




core_error_e core_wlan_eapol_if_function_c::disassociation(
    const network_id_c * receive_network_id )
    {
    DEBUG( "core_wlan_eapol_if_function_c::disassociation()" );
    clear();
    
    core_error_e error;
    core_wlan_eapol_if_parameter_c function;
    error = function.set_parameter_data( wlan_eapol_if_message_type_function, wlan_eapol_if_message_type_function_disassociation );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    // Generate parameters
    core_wlan_eapol_if_function_c network_id;
    error = network_id.generate_network_id(
        receive_network_id );
    if ( error != core_error_ok )
        {
        return error;
        }
    core_wlan_eapol_if_parameter_c network_id_parameter( network_id.get_data(), network_id.size() );
    
    
    // Append parameters to parameter list
    
    error = append( &function );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = append( &network_id_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    debug_print();
    
    return error;
    }



core_error_e core_wlan_eapol_if_function_c::start_preauthentication(
    const network_id_c * receive_network_id )
    {
    DEBUG( "core_wlan_eapol_if_function_c::start_preauthentication()" );
    clear();
    
    core_error_e error;
    core_wlan_eapol_if_parameter_c function;
    error = function.set_parameter_data( wlan_eapol_if_message_type_function, wlan_eapol_if_message_type_function_start_preauthentication );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    // Generate parameters
    core_wlan_eapol_if_function_c network_id;
    error = network_id.generate_network_id(
        receive_network_id );
    if ( error != core_error_ok )
        {
        return error;
        }
    core_wlan_eapol_if_parameter_c network_id_parameter( network_id.get_data(), network_id.size() );
    
    
    // Append parameters to parameter list
    
    error = append( &function );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = append( &network_id_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    debug_print();
    
    return error;
    }




core_error_e core_wlan_eapol_if_function_c::start_reassociation(
    const network_id_c * old_receive_network_id,
    const network_id_c * new_receive_network_id,
    const wlan_eapol_if_eapol_key_authentication_type_e selected_eapol_key_authentication_type )
    {
    DEBUG( "core_wlan_eapol_if_function_c::start_reassociation()" );
    clear();
    
    core_error_e error;
    core_wlan_eapol_if_parameter_c function;
    error = function.set_parameter_data( wlan_eapol_if_message_type_function, wlan_eapol_if_message_type_function_start_reassociation );
    if ( error != core_error_ok )
        {
        return error;
        }


    // Generate parameters
    core_wlan_eapol_if_function_c old_network_id;
    error = old_network_id.generate_network_id(
        old_receive_network_id );
    if ( error != core_error_ok )
        {
        return error;
        }
    core_wlan_eapol_if_parameter_c old_network_id_parameter( old_network_id.get_data(), old_network_id.size() );

    
    core_wlan_eapol_if_function_c new_network_id;
    error = new_network_id.generate_network_id(
        new_receive_network_id );
    if ( error != core_error_ok )
        {
        return error;
        }
    core_wlan_eapol_if_parameter_c new_network_id_parameter( new_network_id.get_data(), new_network_id.size() );

    
    core_wlan_eapol_if_parameter_c key_authentication_type_parameter;
    error = key_authentication_type_parameter.set_parameter_data( wlan_eapol_if_message_type_eapol_key_authentication_type, selected_eapol_key_authentication_type );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    
    // Append parameters to parameter list
    
    error = append( &function );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = append( &old_network_id_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = append( &new_network_id_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = append( &key_authentication_type_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }

    
    debug_print();
    
    return error;
    }



core_error_e core_wlan_eapol_if_function_c::complete_reassociation(
    const wlan_eapol_if_eapol_wlan_authentication_state_e association_result,
    const network_id_c * receive_network_id,
    u8_t * received_wpa_ie, const u32_t received_wpa_ie_length,
    u8_t * sent_wpa_ie, const u32_t sent_wpa_ie_length,
    const wlan_eapol_if_rsna_cipher_e pairwise_key_cipher_suite,
    const wlan_eapol_if_rsna_cipher_e group_key_cipher_suite )
    {
    DEBUG( "core_wlan_eapol_if_function_c::complete_reassociation()" );
    clear();
    
    core_error_e error;
    core_wlan_eapol_if_parameter_c function;
    error = function.set_parameter_data( wlan_eapol_if_message_type_function, wlan_eapol_if_message_type_function_complete_reassociation );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    core_wlan_eapol_if_parameter_c association_result_parameter;
    core_wlan_eapol_if_parameter_c received_wpa_ie_parameter;
    core_wlan_eapol_if_parameter_c sent_wpa_ie_parameter;
    core_wlan_eapol_if_parameter_c pairwise_key_cipher_suite_parameter;
    core_wlan_eapol_if_parameter_c group_key_cipher_suite_parameter;
    
    
    // Generate parameters
    error = association_result_parameter.set_parameter_data( wlan_eapol_if_message_type_eapol_wlan_authentication_state, association_result );
    if ( error != core_error_ok )
        {
        return error;
        }

    core_wlan_eapol_if_function_c network_id;
    error = network_id.generate_network_id(
        receive_network_id );
    if ( error != core_error_ok )
        {
        return error;
        }
    core_wlan_eapol_if_parameter_c network_id_parameter( network_id.get_data(), network_id.size() );



    error = received_wpa_ie_parameter.set_parameter_data( wlan_eapol_if_message_type_variable_data, received_wpa_ie, received_wpa_ie_length );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = sent_wpa_ie_parameter.set_parameter_data( wlan_eapol_if_message_type_variable_data, sent_wpa_ie, sent_wpa_ie_length );
    if ( error != core_error_ok )
        {
        return error;
        }

    error = pairwise_key_cipher_suite_parameter.set_parameter_data( wlan_eapol_if_message_type_RSNA_cipher, pairwise_key_cipher_suite );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = group_key_cipher_suite_parameter.set_parameter_data( wlan_eapol_if_message_type_RSNA_cipher, group_key_cipher_suite );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    
    // Append parameters to parameter list
    
    error = append( &function );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = append( &association_result_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = append( &network_id_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = append( &received_wpa_ie_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = append( &sent_wpa_ie_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = append( &pairwise_key_cipher_suite_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = append( &group_key_cipher_suite_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    debug_print();
    
    return error;
    }



core_error_e core_wlan_eapol_if_function_c::start_wpx_fast_roam_reassociation(
    const network_id_c * old_network_id,
    const network_id_c * new_network_id,
    u8_t * reassociation_request_ie,
    const u32_t reassociation_request_ie_length,
    const u8_t* received_wpa_ie,
    u32_t received_wpa_ie_length,
    const u8_t* sent_wpa_ie,
    u32_t sent_wpa_ie_length )
    {
    DEBUG( "core_wlan_eapol_if_function_c::start_wpx_fast_roam_reassociation()" );
    clear();
    
    core_error_e error;
    core_wlan_eapol_if_parameter_c function;
    error = function.set_parameter_data( wlan_eapol_if_message_type_function, wlan_eapol_if_message_type_function_start_wpx_fast_roam_reassociation );
    if ( error != core_error_ok )
        {
        return error;
        }


    // Generate parameters
    core_wlan_eapol_if_function_c network_id_old;
    error = network_id_old.generate_network_id(
        old_network_id );
    if ( error != core_error_ok )
        {
        return error;
        }
    core_wlan_eapol_if_parameter_c old_network_id_parameter( network_id_old.get_data(), network_id_old.size() );


    core_wlan_eapol_if_function_c network_id_new;
    error = network_id_new.generate_network_id(
        new_network_id );
    if ( error != core_error_ok )
        {
        return error;
        }
    core_wlan_eapol_if_parameter_c new_network_id_parameter( network_id_new.get_data(), network_id_new.size() );

    
    core_wlan_eapol_if_parameter_c reassociation_request_ie_parameter;
    error = reassociation_request_ie_parameter.set_parameter_data( wlan_eapol_if_message_type_variable_data, reassociation_request_ie, reassociation_request_ie_length );
    if ( error != core_error_ok )
        {
        return error;
        }

    core_wlan_eapol_if_parameter_c received_wpa_ie_parameter;
    error = received_wpa_ie_parameter.set_parameter_data( wlan_eapol_if_message_type_variable_data, received_wpa_ie, received_wpa_ie_length );
    if ( error != core_error_ok )
        {
        return error;
        }

    core_wlan_eapol_if_parameter_c sent_wpa_ie_parameter;
    error = sent_wpa_ie_parameter.set_parameter_data( wlan_eapol_if_message_type_variable_data, sent_wpa_ie, sent_wpa_ie_length );
    if ( error != core_error_ok )
        {
        return error;
        }

    // Append parameters to parameter list
    
    error = append( &function );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = append( &old_network_id_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = append( &new_network_id_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = append( &reassociation_request_ie_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = append( &received_wpa_ie_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = append( &sent_wpa_ie_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    debug_print();
    
    return error;
    }




core_error_e core_wlan_eapol_if_function_c::complete_wpx_fast_roam_reassociation(
    const wlan_eapol_if_eapol_wlan_authentication_state_e reassociation_result,
    const network_id_c * receive_network_id,
    u8_t * received_reassociation_ie, const u32_t received_reassociation_ie_length )
    {
    DEBUG( "core_wlan_eapol_if_function_c::complete_wpx_fast_roam_reassociation()" );
    clear();
    
    core_error_e error;
    core_wlan_eapol_if_parameter_c function;
    error = function.set_parameter_data( wlan_eapol_if_message_type_function, wlan_eapol_if_message_type_function_complete_wpx_fast_roam_reassociation );
    if ( error != core_error_ok )
        {
        return error;
        }


    // Generate parameters
    core_wlan_eapol_if_parameter_c reassociation_result_parameter;
    error = reassociation_result_parameter.set_parameter_data( wlan_eapol_if_message_type_eapol_wlan_authentication_state, reassociation_result );
    if ( error != core_error_ok )
        {
        return error;
        }

    core_wlan_eapol_if_function_c network_id;
    error = network_id.generate_network_id(
        receive_network_id );
    if ( error != core_error_ok )
        {
        return error;
        }
    core_wlan_eapol_if_parameter_c network_id_parameter( network_id.get_data(), network_id.size() );

    
    core_wlan_eapol_if_parameter_c received_reassociation_ie_parameter;
    error = received_reassociation_ie_parameter.set_parameter_data( wlan_eapol_if_message_type_variable_data, received_reassociation_ie, received_reassociation_ie_length );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    
    // Append parameters to parameter list
    
    error = append( &function );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = append( &reassociation_result_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = append( &network_id_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = append( &received_reassociation_ie_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }

    
    debug_print();
    
    return error;
    }



core_error_e core_wlan_eapol_if_function_c::packet_process(
    const network_id_c * receive_network_id,
    const u8_t * packet_data, const u32_t packet_data_length )
    {
    DEBUG( "core_wlan_eapol_if_function_c::packet_process()" );
    clear();
    
    core_error_e error;
    core_wlan_eapol_if_parameter_c function;
    error = function.set_parameter_data( wlan_eapol_if_message_type_function, wlan_eapol_if_message_type_function_packet_process );
    if ( error != core_error_ok )
        {
        return error;
        }


    // Generate parameters
    core_wlan_eapol_if_function_c network_id;
    error = network_id.generate_network_id(
        receive_network_id );
    if ( error != core_error_ok )
        {
        return error;
        }
    core_wlan_eapol_if_parameter_c network_id_parameter( network_id.get_data(), network_id.size() );

    
    core_wlan_eapol_if_parameter_c packet_data_parameter;
    error = packet_data_parameter.set_parameter_data( wlan_eapol_if_message_type_variable_data, packet_data, packet_data_length );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    
    // Append parameters to parameter list
    
    error = append( &function );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = append( &network_id_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = append( &packet_data_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }

    
    debug_print();
    
    return error;
    }




core_error_e core_wlan_eapol_if_function_c::tkip_mic_failure(
    const network_id_c * receive_network_id,
    const bool_t is_fatal_failure, 
    const wlan_eapol_if_eapol_tkip_mic_failure_type_e tkip_mic_failure_type )
    {
    DEBUG( "core_wlan_eapol_if_function_c::tkip_mic_failure()" );
    clear();
    
    core_error_e error;
    core_wlan_eapol_if_parameter_c function;
    error = function.set_parameter_data( wlan_eapol_if_message_type_function, wlan_eapol_if_message_type_function_tkip_mic_failure );
    if ( error != core_error_ok )
        {
        return error;
        }


    // Generate parameters
    core_wlan_eapol_if_function_c network_id;
    error = network_id.generate_network_id(
        receive_network_id );
    if ( error != core_error_ok )
        {
        return error;
        }
    core_wlan_eapol_if_parameter_c network_id_parameter( network_id.get_data(), network_id.size() );

    
    core_wlan_eapol_if_parameter_c is_fatal_failure_parameter;
    error = is_fatal_failure_parameter.set_parameter_data( wlan_eapol_if_message_type_boolean, is_fatal_failure );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    core_wlan_eapol_if_parameter_c tkip_mic_failure_type_parameter;
    error = tkip_mic_failure_type_parameter.set_parameter_data( wlan_eapol_if_message_type_eapol_tkip_mic_failure_type, tkip_mic_failure_type );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    // Append parameters to parameter list
    
    error = append( &function );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = append( &network_id_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = append( &is_fatal_failure_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = append( &tkip_mic_failure_type_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }

    
    debug_print();
    
    return error;
    }



core_error_e core_wlan_eapol_if_function_c::eap_acknowledge(
    const network_id_c * receive_network_id )
    {
    DEBUG( "core_wlan_eapol_if_function_c::eap_acknowledge()" );
    clear();
    
    core_error_e error;
    core_wlan_eapol_if_parameter_c function;
    error = function.set_parameter_data( wlan_eapol_if_message_type_function, wlan_eapol_if_message_type_function_eap_acknowledge );
    if ( error != core_error_ok )
        {
        return error;
        }


    // Generate parameters
    core_wlan_eapol_if_function_c network_id;
    error = network_id.generate_network_id(
        receive_network_id );
    if ( error != core_error_ok )
        {
        return error;
        }
    core_wlan_eapol_if_parameter_c network_id_parameter( network_id.get_data(), network_id.size() );

    
    // Append parameters to parameter list
    error = append( &function );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = append( &network_id_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    debug_print();
    
    return error;
    }



core_error_e core_wlan_eapol_if_function_c::update_header_offset(
    const u32_t header_offset,
    const u32_t MTU,
    const u32_t trailer_length )
    {
    DEBUG( "core_wlan_eapol_if_function_c::update_header_offset()" );
    clear();
    
    core_error_e error;
    core_wlan_eapol_if_parameter_c function;
    error = function.set_parameter_data( wlan_eapol_if_message_type_function, wlan_eapol_if_message_type_function_update_header_offset );
    if ( error != core_error_ok )
        {
        return error;
        }


    // Generate parameters
    core_wlan_eapol_if_parameter_c header_offset_parameter;
    error = header_offset_parameter.set_parameter_data( wlan_eapol_if_message_type_u32_t, header_offset );
    if ( error != core_error_ok )
        {
        return error;
        }
    core_wlan_eapol_if_parameter_c MTU_parameter;
    error = MTU_parameter.set_parameter_data( wlan_eapol_if_message_type_u32_t, MTU );
    if ( error != core_error_ok )
        {
        return error;
        }
    core_wlan_eapol_if_parameter_c trailer_length_parameter;
    error = trailer_length_parameter.set_parameter_data( wlan_eapol_if_message_type_u32_t, trailer_length );
    if ( error != core_error_ok )
        {
        return error;
        }


    
    // Append parameters to parameter list
    error = append( &function );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = append( &header_offset_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = append( &MTU_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = append( &trailer_length_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    debug_print();
    
    return error;
    }




core_error_e core_wlan_eapol_if_function_c::generate_complete_check_pmksa_cache(
    core_type_list_c<network_id_c> & network_id_list )
    {
    DEBUG( "core_wlan_eapol_if_function_c::generate_complete_check_pmksa_cache()" );
    clear();
    
    core_error_e error;
    core_wlan_eapol_if_parameter_c function;
    
    error = function.set_parameter_data( wlan_eapol_if_message_type_function, wlan_eapol_if_message_type_function_complete_check_pmksa_cache );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    error = append( &function );
    if ( error != core_error_ok )
        {
        return error;
        }

    // Generate parameters
    core_wlan_eapol_if_function_c array;
    core_wlan_eapol_if_parameter_c array_parameter;
    
    network_id_list.first();
    while ( network_id_list.current() )
        {
        core_wlan_eapol_if_function_c network_id;
        error = network_id.generate_network_id(
            network_id_list.current() );
        if ( error != core_error_ok )
            {
            return error;
            }
        core_wlan_eapol_if_parameter_c network_id_parameter( network_id.get_data(), network_id.size() );
    
        error = array.append( &network_id_parameter );
        if ( error != core_error_ok )
            {
            return error;
            }
        network_id_list.next();
        }
    
    array_parameter.set_parameter_data( wlan_eapol_if_message_type_array, array.get_data(), array.size() );
    error = append( &array_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }


    debug_print();
    
    return error;
    }




core_error_e core_wlan_eapol_if_function_c::parse_complete_check_pmksa_cache(
    core_type_list_c<network_id_c> & network_id_list )
    {
    DEBUG( "core_wlan_eapol_if_function_c::parse_complete_check_pmksa_cache()" );

    core_error_e error( core_error_ok );
    network_id_list.clear();
    
    first();
    if ( is_done() )
        {
        DEBUG( "core_wlan_eapol_if_function_c::complete_check_pmksa_cache() - message is empty" );
        return core_error_not_found;
        }
    
    // Check function
    if ( current()->get_parameter_type() != wlan_eapol_if_message_type_function )
        {
        return core_error_not_found;
        }
    
    u32_t function_value(0);
    current()->get_parameter_data( &function_value );
    wlan_eapol_if_message_type_function_e func( static_cast<wlan_eapol_if_message_type_function_e>( function_value ) );
    if ( func != wlan_eapol_if_message_type_function_complete_check_pmksa_cache )
        {
        return core_error_not_found;
        }
    
    next();
    
    
    // Check function parameters
    if ( current()->get_parameter_type() != wlan_eapol_if_message_type_array )
        {
        return core_error_not_found;
        }
    
    u32_t data_length;
    u8_t * data;
    error = current()->get_parameter_data( &data, &data_length );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    core_wlan_eapol_if_function_c array( data, data_length );
    
    // Loop through array, put every parsed network_id to network_id_list.
    array.first();
    while ( !array.is_done() )
        {
        if ( array.current()->get_parameter_type() != wlan_eapol_if_message_type_network_id )
            {
            return core_error_not_found;
            }
        network_id_c * network_id = new network_id_c( NULL, 0, NULL, 0, 0 );
        if ( !network_id )
            {
            DEBUG( "core_wlan_eapol_if_function_c::parse_complete_check_pmksa_cache() - Error: No enough memory!" );
            return core_error_no_memory;
            }
        error = array.parse_network_id( network_id );
        if ( error != core_error_ok )
            {
            delete network_id;
            network_id = NULL;
            return error;
            }
        
        error = network_id_list.append( network_id );
        if ( error != core_error_ok )
            {
            delete network_id;
            network_id = NULL;
            return error;
            }
        
        array.next();
        }

    return core_error_ok;
    }

core_error_e core_wlan_eapol_if_function_c::generate_packet_send(
    network_id_c * send_network_id,
    u8_t * packet_data, const u32_t packet_data_length )
    {
    DEBUG( "core_wlan_eapol_if_function_c::generate_packet_send()" );
    ASSERT( send_network_id );
    ASSERT( packet_data );
    clear();

    core_error_e error;
    core_wlan_eapol_if_parameter_c function;
    
    error = function.set_parameter_data( wlan_eapol_if_message_type_function, wlan_eapol_if_message_type_function_packet_send );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    error = append( &function );
    if ( error != core_error_ok )
        {
        return error;
        }

    // Generate parameters
    core_wlan_eapol_if_function_c network_id;
    error = network_id.generate_network_id(
        send_network_id );
    if ( error != core_error_ok )
        {
        return error;
        }
    core_wlan_eapol_if_parameter_c network_id_parameter( network_id.get_data(), network_id.size() );

    error = append( &network_id_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    core_wlan_eapol_if_parameter_c packet_parameter;
    error = packet_parameter.set_parameter_data( wlan_eapol_if_message_type_variable_data, packet_data, packet_data_length );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    error = append( &packet_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }


    debug_print();
    
    return error;
    }

core_error_e core_wlan_eapol_if_function_c::parse_packet_send(
    network_id_c * network_id,
    u8_t ** packet_data, u32_t * packet_data_length )
    {
    DEBUG( "core_wlan_eapol_if_function_c::parse_packet_send()" );
    ASSERT( network_id );
    ASSERT( packet_data );
    ASSERT( packet_data_length );
    
    core_error_e error( core_error_ok );
    
    first();
    if ( is_done() )
        {
        DEBUG( "core_wlan_eapol_if_function_c::parse_packet_send() - message is empty" );
        return core_error_not_found;
        }
    
    // Check function
    if ( current()->get_parameter_type() != wlan_eapol_if_message_type_function )
        {
        return core_error_not_found;
        }
    
    u32_t function_value(0);
    current()->get_parameter_data( &function_value );
    wlan_eapol_if_message_type_function_e func( static_cast<wlan_eapol_if_message_type_function_e>( function_value ) );
    if ( func != wlan_eapol_if_message_type_function_packet_send )
        {
        return core_error_not_found;
        }
    
    next();
    
    
    // Check function parameters
    error = parse_network_id( network_id );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    next();
    
    if ( current()->get_parameter_type() != wlan_eapol_if_message_type_variable_data )
        {
        return core_error_not_found;
        }
    error = current()->get_parameter_data( packet_data, packet_data_length );
    if ( error != core_error_ok )
        {
        return error;
        }
    

    return core_error_ok;
    }



core_error_e core_wlan_eapol_if_function_c::generate_associate(
    const wlan_eapol_if_eapol_key_authentication_mode_e authentication_mode )
    {
    DEBUG( "core_wlan_eapol_if_function_c::generate_associate()" );
    clear();
    
    core_error_e error;
    core_wlan_eapol_if_parameter_c function;
    
    error = function.set_parameter_data( wlan_eapol_if_message_type_function, wlan_eapol_if_message_type_function_associate );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    error = append( &function );
    if ( error != core_error_ok )
        {
        return error;
        }

    // Generate parameters
    core_wlan_eapol_if_parameter_c authentication_mode_parameter;
    error = authentication_mode_parameter.set_parameter_data( wlan_eapol_if_message_type_eapol_key_802_11_authentication_mode, authentication_mode );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    error = append( &authentication_mode_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }

    debug_print();
    
    return error;
    }



core_error_e core_wlan_eapol_if_function_c::parse_associate(
    wlan_eapol_if_eapol_key_authentication_mode_e * authentication_mode )
    {
    DEBUG( "core_wlan_eapol_if_function_c::parse_associate()" );
    ASSERT( authentication_mode );
    
    core_error_e error( core_error_ok );
    
    first();
    if ( is_done() )
        {
        DEBUG( "core_wlan_eapol_if_function_c::parse_associate() - message is empty" );
        return core_error_not_found;
        }
    
    // Check function
    if ( current()->get_parameter_type() != wlan_eapol_if_message_type_function )
        {
        return core_error_not_found;
        }
    
    u32_t function_value(0);
    current()->get_parameter_data( &function_value );
    wlan_eapol_if_message_type_function_e func( static_cast<wlan_eapol_if_message_type_function_e>( function_value ) );
    if ( func != wlan_eapol_if_message_type_function_associate )
        {
        return core_error_not_found;
        }
    
    next();
    
    
    // Check function parameters
    if ( current()->get_parameter_type() != wlan_eapol_if_message_type_eapol_key_802_11_authentication_mode )
        {
        return core_error_not_found;
        }
    u32_t temp_authentication_mode( 0 );
    error = current()->get_parameter_data( &temp_authentication_mode );
    if ( error != core_error_ok )
        {
        return error;
        }
    *authentication_mode = static_cast<wlan_eapol_if_eapol_key_authentication_mode_e>( temp_authentication_mode );

    return core_error_ok;
    }



core_error_e core_wlan_eapol_if_function_c::generate_disassociate(
    network_id_c * receive_network_id,
    const bool_t self_disassociation )
    {
    DEBUG( "core_wlan_eapol_if_function_c::generate_disassociate()" );
    ASSERT( receive_network_id );
    clear();

    core_error_e error;
    core_wlan_eapol_if_parameter_c function;
    
    error = function.set_parameter_data( wlan_eapol_if_message_type_function, wlan_eapol_if_message_type_function_disassociate );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    error = append( &function );
    if ( error != core_error_ok )
        {
        return error;
        }

    // Generate parameters
    core_wlan_eapol_if_function_c network_id;
    error = network_id.generate_network_id(
        receive_network_id );
    if ( error != core_error_ok )
        {
        return error;
        }
    core_wlan_eapol_if_parameter_c network_id_parameter( network_id.get_data(), network_id.size() );

    error = append( &network_id_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    core_wlan_eapol_if_parameter_c self_disassociation_parameter;
    error = self_disassociation_parameter.set_parameter_data( wlan_eapol_if_message_type_boolean, self_disassociation );
    if ( error != core_error_ok )
        {
        return error;
        }

    error = append( &self_disassociation_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }


    debug_print();
    
    return error;
    }

core_error_e core_wlan_eapol_if_function_c::parse_disassociate(
    network_id_c * receive_network_id,
    bool_t * self_disassociation )
    {
    DEBUG( "core_wlan_eapol_if_function_c::parse_disassociate()" );
    ASSERT( receive_network_id );
    ASSERT( self_disassociation );
    
    core_error_e error( core_error_ok );
    
    first();
    if ( is_done() )
        {
        DEBUG( "core_wlan_eapol_if_function_c::parse_disassociate() - message is empty" );
        return core_error_not_found;
        }
    
    // Check function
    if ( current()->get_parameter_type() != wlan_eapol_if_message_type_function )
        {
        return core_error_not_found;
        }
    
    u32_t function_value(0);
    current()->get_parameter_data( &function_value );
    wlan_eapol_if_message_type_function_e func( static_cast<wlan_eapol_if_message_type_function_e>( function_value ) );
    if ( func != wlan_eapol_if_message_type_function_disassociate )
        {
        return core_error_not_found;
        }
    
    next();
    
    
    // Check function parameters
    error = parse_network_id( receive_network_id );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    next();
    
    if ( current()->get_parameter_type() != wlan_eapol_if_message_type_boolean )
        {
        return core_error_not_found;
        }
    error = current()->get_parameter_data( self_disassociation );
    if ( error != core_error_ok )
        {
        return error;
        }
    

    return core_error_ok;
    }





core_error_e core_wlan_eapol_if_function_c::generate_packet_data_session_key(
    network_id_c * send_network_id,
    session_key_c * session_key )
    {
    DEBUG( "core_wlan_eapol_if_function_c::generate_packet_data_session_key()" );
    ASSERT( send_network_id );
    ASSERT( session_key );
    clear();

    core_error_e error;
    core_wlan_eapol_if_parameter_c function;
    
    error = function.set_parameter_data( wlan_eapol_if_message_type_function, wlan_eapol_if_message_type_function_packet_data_session_key );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    error = append( &function );
    if ( error != core_error_ok )
        {
        return error;
        }

    // Generate parameters
    core_wlan_eapol_if_function_c network_id;
    error = network_id.generate_network_id(
        send_network_id );
    if ( error != core_error_ok )
        {
        return error;
        }
    core_wlan_eapol_if_parameter_c network_id_parameter( network_id.get_data(), network_id.size() );

    error = append( &network_id_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    
    core_wlan_eapol_if_function_c generated_session_key;
    error = generated_session_key.generate_session_key(
        session_key );
    if ( error != core_error_ok )
        {
        return error;
        }
    core_wlan_eapol_if_parameter_c session_key_parameter( generated_session_key.get_data(), generated_session_key.size() );

    error = append( &session_key_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }

    debug_print();
    
    return error;
    }




core_error_e core_wlan_eapol_if_function_c::parse_packet_data_session_key(
    network_id_c * send_network_id,
    session_key_c * session_key )
    {
    DEBUG( "core_wlan_eapol_if_function_c::parse_packet_data_session_key()" );
    ASSERT( send_network_id );
    ASSERT( session_key );
    
    core_error_e error( core_error_ok );
    
    first();
    if ( is_done() )
        {
        DEBUG( "core_wlan_eapol_if_function_c::parse_packet_data_session_key() - message is empty" );
        return core_error_not_found;
        }
    
    // Check function
    if ( current()->get_parameter_type() != wlan_eapol_if_message_type_function )
        {
        return core_error_not_found;
        }
    
    u32_t function_value(0);
    current()->get_parameter_data( &function_value );
    wlan_eapol_if_message_type_function_e func( static_cast<wlan_eapol_if_message_type_function_e>( function_value ) );
    if ( func != wlan_eapol_if_message_type_function_packet_data_session_key )
        {
        return core_error_not_found;
        }
    
    next();
    
    
    // Check function parameters
    error = parse_network_id( send_network_id );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    next();
    
    error = parse_session_key(
        session_key );
    if ( error != core_error_ok )
        {
        return error;
        }

    return core_error_ok;
    }




core_error_e core_wlan_eapol_if_function_c::generate_state_notification(
    state_notification_c * state_notification )
    {
    DEBUG( "core_wlan_eapol_if_function_c::generate_state_notification()" );
    ASSERT( state_notification );
    clear();

    core_error_e error;
    core_wlan_eapol_if_parameter_c function;
    
    error = function.set_parameter_data( wlan_eapol_if_message_type_function, wlan_eapol_if_message_type_function_state_notification );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    error = append( &function );
    if ( error != core_error_ok )
        {
        return error;
        }

    // Generate parameters
    core_wlan_eapol_if_function_c state_notif;
    error = state_notif.generate_eap_state_notification(
        state_notification );
    if ( error != core_error_ok )
        {
        return error;
        }
    core_wlan_eapol_if_parameter_c state_notification_parameter( state_notif.get_data(), state_notif.size() );

    error = append( &state_notification_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    debug_print();
    
    return error;
    }




core_error_e core_wlan_eapol_if_function_c::parse_state_notification(
    state_notification_c * state_notification )
    {
    DEBUG( "core_wlan_eapol_if_function_c::parse_state_notification()" );
    ASSERT( state_notification );
    
    core_error_e error( core_error_ok );
    
    first();
    if ( is_done() )
        {
        DEBUG( "core_wlan_eapol_if_function_c::parse_state_notification() - message is empty" );
        return core_error_not_found;
        }
    
    // Check function
    if ( current()->get_parameter_type() != wlan_eapol_if_message_type_function )
        {
        return core_error_not_found;
        }
    
    u32_t function_value(0);
    current()->get_parameter_data( &function_value );
    wlan_eapol_if_message_type_function_e func( static_cast<wlan_eapol_if_message_type_function_e>( function_value ) );
    if ( func != wlan_eapol_if_message_type_function_state_notification )
        {
        return core_error_not_found;
        }
    
    next();
    
    
    // Check function parameters
    error = parse_eap_state_notification( state_notification );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    return core_error_ok;
    }


core_error_e core_wlan_eapol_if_function_c::generate_reassociate(
    network_id_c * send_network_id,
    wlan_eapol_if_eapol_key_authentication_type_e authentication_type,
    u8_t * pmkid,
    u32_t pmkid_length )
    {
    DEBUG( "core_wlan_eapol_if_function_c::generate_reassociate()" );
    ASSERT( send_network_id );
    ASSERT( pmkid );
    clear();

    core_error_e error;
    core_wlan_eapol_if_parameter_c function;
    
    error = function.set_parameter_data( wlan_eapol_if_message_type_function, wlan_eapol_if_message_type_function_reassociate );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    error = append( &function );
    if ( error != core_error_ok )
        {
        return error;
        }

    // Generate parameters
    core_wlan_eapol_if_function_c network_id;
    error = network_id.generate_network_id(
        send_network_id );
    if ( error != core_error_ok )
        {
        return error;
        }
    core_wlan_eapol_if_parameter_c network_id_parameter( network_id.get_data(), network_id.size() );

    error = append( &network_id_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    core_wlan_eapol_if_parameter_c authentication_type_parameter;
    error = authentication_type_parameter.set_parameter_data( wlan_eapol_if_message_type_eapol_key_authentication_type, authentication_type );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    error = append( &authentication_type_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }

    core_wlan_eapol_if_parameter_c pmkid_parameter;
    error = pmkid_parameter.set_parameter_data( wlan_eapol_if_message_type_variable_data, pmkid, pmkid_length );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    error = append( &pmkid_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }

    debug_print();
    
    return error;
    }


core_error_e core_wlan_eapol_if_function_c::parse_reassociate(
    network_id_c * send_network_id,
    wlan_eapol_if_eapol_key_authentication_type_e * authentication_type,
    u8_t ** pmkid,
    u32_t * pmkid_length )
    {
    DEBUG( "core_wlan_eapol_if_function_c::parse_reassociate()" );
    ASSERT( send_network_id );
    ASSERT( authentication_type );
    ASSERT( pmkid );
    ASSERT( pmkid_length );
    
    core_error_e error( core_error_ok );
    
    first();
    if ( is_done() )
        {
        DEBUG( "core_wlan_eapol_if_function_c::parse_reassociate() - message is empty" );
        return core_error_not_found;
        }
    
    // Check function
    if ( current()->get_parameter_type() != wlan_eapol_if_message_type_function )
        {
        return core_error_not_found;
        }
    
    u32_t function_value(0);
    current()->get_parameter_data( &function_value );
    wlan_eapol_if_message_type_function_e func( static_cast<wlan_eapol_if_message_type_function_e>( function_value ) );
    if ( func != wlan_eapol_if_message_type_function_reassociate )
        {
        return core_error_not_found;
        }
    
    next();
    
    // Check function parameters
    error = parse_network_id( send_network_id );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    next();
    
    if ( current()->get_parameter_type() != wlan_eapol_if_message_type_eapol_key_authentication_type )
        {
        return core_error_not_found;
        }
    u32_t temp_authentication_type( 0 );
    error = current()->get_parameter_data( &temp_authentication_type );
    if ( error != core_error_ok )
        {
        return error;
        }
    *authentication_type = static_cast<wlan_eapol_if_eapol_key_authentication_type_e>( temp_authentication_type );
    
    next();

    if ( current()->get_parameter_type() != wlan_eapol_if_message_type_variable_data )
        {
        return core_error_not_found;
        }
    error = current()->get_parameter_data( pmkid, pmkid_length );
    if ( error != core_error_ok )
        {
        return error;
        }
    

    return core_error_ok;
    }


core_error_e core_wlan_eapol_if_function_c::update_wlan_database_reference_values(
    u8_t * database_reference_value, const u32_t database_reference_value_length )
    {
    DEBUG( "core_wlan_eapol_if_function_c::update_wlan_database_reference_values()" );
    ASSERT( database_reference_value );
    clear();
    
    core_error_e error;
    core_wlan_eapol_if_parameter_c function;
    error = function.set_parameter_data( wlan_eapol_if_message_type_function, wlan_eapol_if_message_type_function_update_wlan_database_reference_values );
    if ( error != core_error_ok )
        {
        return error;
        }


    // Generate parameters
    core_wlan_eapol_if_parameter_c database_reference_value_parameter;
    error = database_reference_value_parameter.set_parameter_data( wlan_eapol_if_message_type_variable_data, database_reference_value, database_reference_value_length );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    // Append parameters to parameter list
    error = append( &function );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = append( &database_reference_value_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    debug_print();
    
    return error;
    }





core_error_e core_wlan_eapol_if_function_c::generate_complete_start_wpx_fast_roam_reassociation(
    network_id_c * receive_network_id,
    u8_t * reassociation_request_ie,
    u32_t reassociation_request_ie_length )
    {
    DEBUG( "core_wlan_eapol_if_function_c::generate_complete_start_wpx_fast_roam_reassociation()" );
    ASSERT( receive_network_id );
    ASSERT( reassociation_request_ie );
    clear();

    core_error_e error;
    core_wlan_eapol_if_parameter_c function;
    
    error = function.set_parameter_data( wlan_eapol_if_message_type_function, wlan_eapol_if_message_type_function_complete_start_wpx_fast_roam_reassociation );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    error = append( &function );
    if ( error != core_error_ok )
        {
        return error;
        }

    // Generate parameters
    core_wlan_eapol_if_function_c network_id;
    error = network_id.generate_network_id(
        receive_network_id );
    if ( error != core_error_ok )
        {
        return error;
        }
    core_wlan_eapol_if_parameter_c network_id_parameter( network_id.get_data(), network_id.size() );

    error = append( &network_id_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }
    

    core_wlan_eapol_if_parameter_c pmkid_parameter;
    error = pmkid_parameter.set_parameter_data( wlan_eapol_if_message_type_variable_data, reassociation_request_ie, reassociation_request_ie_length );
    if ( error != core_error_ok )
        {
        return error;
        }

    error = append( &pmkid_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }

    debug_print();
    
    return error;
    }

core_error_e core_wlan_eapol_if_function_c::parse_complete_start_wpx_fast_roam_reassociation(
    network_id_c * receive_network_id,
    u8_t ** reassociation_request_ie,
    u32_t * reassociation_request_ie_length )
    {
    DEBUG( "core_wlan_eapol_if_function_c::parse_complete_start_wpx_fast_roam_reassociation()" );
    ASSERT( receive_network_id );
    ASSERT( reassociation_request_ie );
    ASSERT( reassociation_request_ie_length );
    
    core_error_e error( core_error_ok );
    
    first();
    if ( is_done() )
        {
        DEBUG( "core_wlan_eapol_if_function_c::parse_complete_start_wpx_fast_roam_reassociation() - message is empty" );
        return core_error_not_found;
        }
    
    // Check function
    if ( current()->get_parameter_type() != wlan_eapol_if_message_type_function )
        {
        return core_error_not_found;
        }
    
    u32_t function_value(0);
    current()->get_parameter_data( &function_value );
    wlan_eapol_if_message_type_function_e func( static_cast<wlan_eapol_if_message_type_function_e>( function_value ) );
    if ( func != wlan_eapol_if_message_type_function_complete_start_wpx_fast_roam_reassociation )
        {
        return core_error_not_found;
        }
    
    next();
    
    // Check function parameters
    error = parse_network_id( receive_network_id );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    next();
    
    if ( current()->get_parameter_type() != wlan_eapol_if_message_type_variable_data )
        {
        return core_error_not_found;
        }
    error = current()->get_parameter_data( reassociation_request_ie, reassociation_request_ie_length );
    if ( error != core_error_ok )
        {
        return error;
        }
    

    return core_error_ok;
    }


core_error_e core_wlan_eapol_if_function_c::generate_error(
    wlan_eapol_if_error_e errorcode,
    wlan_eapol_if_message_type_function_e function )
    {
    DEBUG( "core_wlan_eapol_if_function_c::generate_error()" );
    clear();

    core_error_e error;
    core_wlan_eapol_if_parameter_c error_parameter;
    core_wlan_eapol_if_parameter_c function_parameter;
    
    error = error_parameter.set_parameter_data( wlan_eapol_if_message_type_error, errorcode );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = append( &error_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }


    error = function_parameter.set_parameter_data( wlan_eapol_if_message_type_function, function );
    if ( error != core_error_ok )
        {
        return error;
        }
    error = append( &function_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }

    debug_print();
    
    return error;
    }




core_error_e core_wlan_eapol_if_function_c::parse_error(
    wlan_eapol_if_error_e * errorcode,
    wlan_eapol_if_message_type_function_e * function )
    {
    DEBUG( "core_wlan_eapol_if_function_c::parse_error()" );
    ASSERT( errorcode );
    ASSERT( function );
    
    core_error_e error( core_error_ok );
    
    first();
    if ( is_done() )
        {
        DEBUG( "core_wlan_eapol_if_function_c::parse_error() - message is empty" );
        return core_error_not_found;
        }
    
    // Check error
    if ( current()->get_parameter_type() != wlan_eapol_if_message_type_error )
        {
        return core_error_not_found;
        }
    u32_t temp_errorcode( 0 );
    error = current()->get_parameter_data( &temp_errorcode );
    if ( error != core_error_ok )
        {
        return error;
        }
    *errorcode = static_cast<wlan_eapol_if_error_e>( temp_errorcode );
    
    next();
    u32_t function_value(0);
    current()->get_parameter_data( &function_value );
    *function = static_cast<wlan_eapol_if_message_type_function_e>( function_value );
    
    DEBUG2( "core_wlan_eapol_if_function_c::parse_error() - Error message received: errorcode=%i, function=%i", *errorcode, *function );
    
    return core_error_ok;
    }



core_error_e core_wlan_eapol_if_function_c::generate_new_protected_setup_credentials(
    core_type_list_c< protected_setup_credential_c > & credential_list )
    {
    DEBUG( "core_wlan_eapol_if_function_c::generate_new_protected_setup_credentials()" );
    clear();
    
    core_error_e error;
    core_wlan_eapol_if_parameter_c function;
    
    error = function.set_parameter_data( wlan_eapol_if_message_type_function, wlan_eapol_if_message_type_function_new_protected_setup_credentials );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    error = append( &function );
    if ( error != core_error_ok )
        {
        return error;
        }

    // Generate parameters
    core_wlan_eapol_if_function_c array;
    core_wlan_eapol_if_parameter_c array_parameter;
    
    credential_list.first();
    while ( credential_list.current() )
        {
        core_wlan_eapol_if_function_c credential;
        error = credential.generate_protected_setup_credential(
            credential_list.current() );
        if ( error != core_error_ok )
            {
            return error;
            }
        core_wlan_eapol_if_parameter_c credential_parameter( credential.get_data(), credential.size() );
    
        error = array.append( &credential_parameter );
        if ( error != core_error_ok )
            {
            return error;
            }
        credential_list.next();
        }
    
    array_parameter.set_parameter_data( wlan_eapol_if_message_type_array, array.get_data(), array.size() );
    error = append( &array_parameter );
    if ( error != core_error_ok )
        {
        return error;
        }


    debug_print();
    
    return error;
    }



core_error_e core_wlan_eapol_if_function_c::parse_new_protected_setup_credentials(
    core_type_list_c< protected_setup_credential_c > & credential_list )
    {
    DEBUG( "core_wlan_eapol_if_function_c::parse_new_protected_setup_credentials()" );

    core_error_e error( core_error_ok );
    credential_list.clear();
    
    first();
    if ( is_done() )
        {
        DEBUG( "core_wlan_eapol_if_function_c::parse_new_protected_setup_credentials() - message is empty" );
        return core_error_not_found;
        }
    
    // Check function
    if ( current()->get_parameter_type() != wlan_eapol_if_message_type_function )
        {
        return core_error_not_found;
        }
    
    u32_t function_value(0);
    current()->get_parameter_data( &function_value );
    wlan_eapol_if_message_type_function_e func( static_cast<wlan_eapol_if_message_type_function_e>( function_value ) );
    if ( func != wlan_eapol_if_message_type_function_new_protected_setup_credentials )
        {
        return core_error_not_found;
        }
    
    next();
    
    
    // Check function parameters
    if ( current()->get_parameter_type() != wlan_eapol_if_message_type_array )
        {
        return core_error_not_found;
        }
    
    u32_t data_length;
    u8_t * data;
    error = current()->get_parameter_data( &data, &data_length );
    if ( error != core_error_ok )
        {
        return error;
        }
    
    core_wlan_eapol_if_function_c array( data, data_length );
    
    // Loop through array, put every parsed credential to credential_list.
    array.first();
    while ( !array.is_done() )
        {
        if ( array.current()->get_parameter_type() != wlan_eapol_if_message_type_protected_setup_credential )
            {
            return core_error_not_found;
            }
        protected_setup_credential_c * credential = new protected_setup_credential_c( 0, NULL, 0, 0, 0, NULL, 0 );
        if ( !credential )
            {
            DEBUG( "core_wlan_eapol_if_function_c::parse_new_protected_setup_credentials() - Error: No enough memory!" );
            return core_error_no_memory;
            }
        error = array.parse_protected_setup_credential( credential );
        if ( error != core_error_ok )
            {
            delete credential;
            credential = NULL;
            return error;
            }
        
        error = credential_list.append( credential );
        if ( error != core_error_ok )
            {
            delete credential;
            credential = NULL;
            return error;
            }
        
        array.next();
        }

    return core_error_ok;
    }

core_error_e core_wlan_eapol_if_function_c::parse_complete_disassociation(
    network_id_c * receive_network_id )
    {
    DEBUG( "core_wlan_eapol_if_function_c::parse_complete_disassociation()" );
    ASSERT( receive_network_id );

    core_error_e error( core_error_ok );

    first();
    if ( is_done() )
        {
        DEBUG( "core_wlan_eapol_if_function_c::parse_complete_disassociation() - message is empty" );
        return core_error_not_found;
        }

    // Check function
    if ( current()->get_parameter_type() != wlan_eapol_if_message_type_function )
        {
        return core_error_not_found;
        }

    u32_t function_value(0);
    current()->get_parameter_data( &function_value );
    wlan_eapol_if_message_type_function_e func( static_cast<wlan_eapol_if_message_type_function_e>( function_value ) );
    if ( func != wlan_eapol_if_message_type_function_complete_disassociation )
        {
        return core_error_not_found;
        }
    
    next();

    // Check function parameters
    error = parse_network_id( receive_network_id );
    if ( error != core_error_ok )
        {
        return error;
        }

    return core_error_ok;
    }
