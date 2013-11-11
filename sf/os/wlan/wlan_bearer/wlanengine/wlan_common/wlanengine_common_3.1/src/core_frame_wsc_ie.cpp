/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class for parsing WSC IEs.
*
*/


#include "core_frame_wsc_ie.h"
#include "core_tools.h"
#include "am_debug.h"

/** Defining this enables IE parsing related traces. */
//#define WLAN_CORE_DEEP_DEBUG 1

const u8_t CORE_FRAME_WSC_IE_OUI_OFFSET = 2;
const u8_t CORE_FRAME_WSC_IE_OUI_LENGTH = 4;
const u8_t CORE_FRAME_WSC_IE_OUI[] = { 0x00, 0x50, 0xF2, 0x04 };

/** Offset to first Protected Setup attribute type in IE */
const u8_t CORE_FRAME_WSC_IE_PROTECTED_SETUP_DATA_OFFSET = 
                CORE_FRAME_WSC_IE_OUI_OFFSET + CORE_FRAME_WSC_IE_OUI_LENGTH;
/** Offset from AttributeType field to DataLength field */
const u8_t CORE_FRAME_WSC_IE_DATA_LENGTH_OFFSET = 2;
/** Offset from AttributeType field to Data field */
const u8_t CORE_FRAME_WSC_IE_DATA_AREA_OFFSET   = 4;

/** Maximum length of ASCII text fields. Used to prevent possible buffer overrun in debug print. */
const u16_t CORE_FRAME_WSC_IE_MANUFACTURER_MAX_LENGTH   = 64;
const u16_t CORE_FRAME_WSC_IE_MODEL_NAME_MAX_LENGTH     = 32;

/** IDs for different data elements in WSC IE. */
const u16_t CORE_FRAME_WSC_IE_ASSOCIATION_STATE     = 0x1002;
const u16_t CORE_FRAME_WSC_IE_CONFIG_METHODS        = 0x1008;
const u16_t CORE_FRAME_WSC_IE_CONFIGURATION_ERROR   = 0x1009;
const u16_t CORE_FRAME_WSC_IE_DEVICE_NAME           = 0x1011;
const u16_t CORE_FRAME_WSC_IE_DEVICE_PASSWORD_ID    = 0x1012;
const u16_t CORE_FRAME_WSC_IE_MANUFACTURER          = 0x1021;
const u16_t CORE_FRAME_WSC_IE_MODEL_NAME            = 0x1023;
const u16_t CORE_FRAME_WSC_IE_MODEL_NUMBER          = 0x1024;
const u16_t CORE_FRAME_WSC_IE_REQUEST_TYPE          = 0x103A;
const u16_t CORE_FRAME_WSC_IE_RESPONSE_TYPE         = 0x103B;
const u16_t CORE_FRAME_WSC_IE_RF_BANDS              = 0x103C;
const u16_t CORE_FRAME_WSC_IE_SELECTED_REGISTRAR    = 0x1041;
const u16_t CORE_FRAME_WSC_IE_SERIAL_NUMBER         = 0x1042;
const u16_t CORE_FRAME_WSC_IE_PROTECTED_SETUP_STATE = 0x1044;
const u16_t CORE_FRAME_WSC_IE_UUID_E                = 0x1047;
const u16_t CORE_FRAME_WSC_IE_UUID_R                = 0x1048;
const u16_t CORE_FRAME_WSC_IE_VERSION               = 0x104A;
const u16_t CORE_FRAME_WSC_IE_SELECTED_REGISTRAR_CONFIG_METHODS = 0x1053;
const u16_t CORE_FRAME_WSC_IE_PRIMARY_DEVICE_TYPE   = 0x1054;
const u16_t CORE_FRAME_WSC_IE_AP_SETUP_LOCKED       = 0x1057;



// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_wsc_ie_c* core_frame_wsc_ie_c::instance(
    const core_frame_dot11_ie_c& ie )
    {
    if ( core_tools_c::compare(
        &ie.data()[CORE_FRAME_WSC_IE_OUI_OFFSET],
        CORE_FRAME_WSC_IE_OUI_LENGTH,
        &CORE_FRAME_WSC_IE_OUI[0],
        CORE_FRAME_WSC_IE_OUI_LENGTH ) )
        {
#ifdef WLAN_CORE_DEEP_DEBUG
        DEBUG( "core_frame_wsc_ie_c::instance() - not a valid IE, WSC OUI missing" );
#endif // WLAN_CORE_DEEP_DEBUG

        return NULL;
        }

    core_frame_wsc_ie_c* instance = new core_frame_wsc_ie_c(
        ie.data_length(),
        ie.data(),
        0 );
    if ( !instance )
        {
        DEBUG( "core_frame_wsc_ie_c::instance() - unable to create an instance" );

        return NULL;
        }
    
    instance->search_attributes();
    
    return instance;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// 
core_frame_wsc_ie_c* core_frame_wsc_ie_c::instance(
        const u8_t version,
        const u8_t request_type )
    {
    // Count size of needed buffer.
    const u16_t max_length = 
        CORE_FRAME_DOT11_IE_HEADER_LENGTH
        + CORE_FRAME_WSC_IE_OUI_LENGTH
        + CORE_FRAME_WSC_IE_DATA_AREA_OFFSET + sizeof( version )
        + CORE_FRAME_WSC_IE_DATA_AREA_OFFSET + sizeof( request_type );

    u8_t* buffer = new u8_t[max_length];
    if ( !buffer )
        {
        DEBUG( "core_frame_wsc_ie_c::instance() - unable create the internal buffer" );
        return NULL;
        }

    core_frame_wsc_ie_c* instance =
        new core_frame_wsc_ie_c( 0, buffer, max_length );
    if ( !instance )
        {
        DEBUG( "core_frame_wsc_ie_c::instance() - unable to create an instance" );
        delete[] buffer;

        return NULL;
        }

    instance->generate(
        version,
        request_type );
    
    // Update members according to generated values.
    // This is not necessary needed, but in testing phase this is useful.
    instance->search_attributes();
    
    return instance;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//       
core_frame_wsc_ie_c::~core_frame_wsc_ie_c()
    {
    DEBUG( "core_frame_wsc_ie_c::~core_frame_wsc_ie_c()" );
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u16_t core_frame_wsc_ie_c::association_state() const
    {
    if ( association_state_ind_m == 0 )
        {
        DEBUG( "core_frame_wsc_ie_c::association_state() - association_state field not found from IE" );
        return 0;
        }
    return core_tools_c::get_u16_big_endian(
        data_m, association_state_ind_m);
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u16_t core_frame_wsc_ie_c::config_methods() const
    {
    if ( config_methods_ind_m == 0 )
        {
        DEBUG( "core_frame_wsc_ie_c::config_methods() - config_methods field not found from IE" );
        return 0;
        }
    return core_tools_c::get_u16_big_endian(
        data_m, config_methods_ind_m);
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u16_t core_frame_wsc_ie_c::configuration_error() const
    {
    if ( configuration_error_ind_m == 0 )
        {
        DEBUG( "core_frame_wsc_ie_c::configuration_error() - configuration_error field not found from IE" );
        return 0;
        }
    return core_tools_c::get_u16_big_endian(
        data_m, configuration_error_ind_m);
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
const u8_t* core_frame_wsc_ie_c::device_name() const
    {
    if ( device_name_ind_m == 0 )
        {
        DEBUG( "core_frame_wsc_ie_c::device_name() - device_name field not found from IE" );
        return NULL;
        }
    return &data_m[device_name_ind_m];
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u16_t core_frame_wsc_ie_c::device_password_id() const
    {
    if ( device_password_id_ind_m == 0 )
        {
        DEBUG( "core_frame_wsc_ie_c::device_password_id() - device_password_id field not found from IE" );
        return 0xFFFF;
        }
    return core_tools_c::get_u16_big_endian(
        data_m, device_password_id_ind_m);
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
const u8_t* core_frame_wsc_ie_c::manufacturer() const
    {
    if ( manufacturer_ind_m == 0 )
        {
        DEBUG( "core_frame_wsc_ie_c::manufacturer() - manufacturer field not found from IE" );
        return NULL;
        }
    return &data_m[manufacturer_ind_m];
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
const u8_t* core_frame_wsc_ie_c::model_name() const
    {
    if ( model_name_ind_m == 0 )
        {
        DEBUG( "core_frame_wsc_ie_c::model_name() - model_name field not found from IE" );
        return NULL;
        }
    return &data_m[model_name_ind_m];
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
const u8_t* core_frame_wsc_ie_c::model_number() const
    {
    if ( model_number_ind_m == 0 )
        {
        DEBUG( "core_frame_wsc_ie_c::model_number() - model_number field not found from IE" );
        return NULL;
        }
    return &data_m[model_number_ind_m];
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u8_t core_frame_wsc_ie_c::request_type() const
    {
    if ( request_type_ind_m == 0 )
        {
        DEBUG( "core_frame_wsc_ie_c::request_type() - request_type field not found from IE" );
        return 0xFF;
        }
    return data_m[request_type_ind_m];
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u8_t core_frame_wsc_ie_c::response_type() const
    {
    if ( response_type_ind_m == 0 )
        {
        DEBUG( "core_frame_wsc_ie_c::response_type() - response_type field not found from IE" );
        return 0xFF;
        }
    return data_m[response_type_ind_m];
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u8_t core_frame_wsc_ie_c::rf_bands() const
    {
    if ( rf_bands_ind_m == 0 )
        {
        DEBUG( "core_frame_wsc_ie_c::rf_bands() - rf_bands field not found from IE" );
        return 0;
        }
    return data_m[rf_bands_ind_m];
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_frame_wsc_ie_c::selected_registrar() const
    {
    if ( selected_registrar_ind_m == 0 )
        {
        DEBUG( "core_frame_wsc_ie_c::selected_registrar() - selected_registrar field not found from IE" );
        return false_t;
        }
    else if ( data_m[selected_registrar_ind_m] == 0 )
        {
        return false_t;
        }
    else
        {
        return true_t;
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
const u8_t* core_frame_wsc_ie_c::serial_number() const
    {
    if ( serial_number_ind_m == 0 )
        {
        DEBUG( "core_frame_wsc_ie_c::serial_number() - serial_number field not found from IE" );
        return NULL;
        }
    return &data_m[serial_number_ind_m];
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u8_t core_frame_wsc_ie_c::protected_setup_state() const
    {
    if ( protected_setup_state_ind_m == 0 )
        {
        DEBUG( "core_frame_wsc_ie_c::protected_setup_state() - protected_setup_state field not found from IE" );
        return 0;
        }
    return data_m[protected_setup_state_ind_m];
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
const u8_t* core_frame_wsc_ie_c::uuid_e() const
    {
    if ( uuid_e_ind_m == 0 )
        {
        DEBUG( "core_frame_wsc_ie_c::uuid_e() - uuid_e field not found from IE" );
        return NULL;
        }
    return &data_m[uuid_e_ind_m];
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
const u8_t* core_frame_wsc_ie_c::uuid_r() const
    {
    if ( uuid_r_ind_m == 0 )
        {
        DEBUG( "core_frame_wsc_ie_c::uuid_r() - uuid_r field not found from IE" );
        return NULL;
        }
    return &data_m[uuid_r_ind_m];
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u8_t core_frame_wsc_ie_c::version() const
    {
    if ( version_ind_m == 0 )
        {
        DEBUG( "core_frame_wsc_ie_c::version() - version field not found from IE" );
        return 0x00;
        }
    return data_m[version_ind_m];
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u16_t core_frame_wsc_ie_c::selected_registrar_config_methods() const
    {
    if ( selected_registrar_config_methods_ind_m == 0 )
        {
        DEBUG( "core_frame_wsc_ie_c::selected_registrar_config_methods() - selected_registrar_config_methods field not found from IE" );
        return 0;
        }
    return core_tools_c::get_u16_big_endian(
        data_m, selected_registrar_config_methods_ind_m);
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
const u8_t* core_frame_wsc_ie_c::primary_device_type() const
    {
    if ( primary_device_type_ind_m == 0 )
        {
        DEBUG( "core_frame_wsc_ie_c::primary_device_type() - primary_device_type field not found from IE" );
        return 0;
        }
    return &data_m[primary_device_type_ind_m];
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_frame_wsc_ie_c::ap_setup_locked() const
    {
    if ( ap_setup_locked_ind_m == 0 )
        {
        DEBUG( "core_frame_wsc_ie_c::ap_setup_locked() - ap_setup_locked field not found from IE" );
        return false_t;
        }
    else if ( data_m[ap_setup_locked_ind_m] == 0 )
        {
        return false_t;
        }
    else
        {
        return true_t;
        }
    }




// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
/** Search attributes from WSC IE.
 * 
 * The main idea in searching is to loop through IE, and store index of each attribute
 * to member variable. After loop, it is easy to get needed attribute with stored index.
 * 
 * All data is in big endian format.
 */
void core_frame_wsc_ie_c::search_attributes()
    {
    DEBUG( "core_frame_wsc_ie_c::search_attributes()" );
    
    u16_t index( CORE_FRAME_WSC_IE_PROTECTED_SETUP_DATA_OFFSET );
    while (index < data_length())
        {
        u16_t attribute_type = core_tools_c::get_u16_big_endian(
            data_m, index);
            
        u16_t attribute_data_length = core_tools_c::get_u16_big_endian(
            data_m, index + CORE_FRAME_WSC_IE_DATA_LENGTH_OFFSET);
        //DEBUG3( "core_frame_wsc_ie_c::search_attributes() - found attribute 0x%04x at index %u (length=%u)", attribute_type, index, attribute_data_length);
        
        // Check if all attribute data does not fit current IE.
        if ( ( index + attribute_data_length + CORE_FRAME_WSC_IE_DATA_AREA_OFFSET ) > data_length())
            {
            DEBUG5( "core_frame_wsc_ie_c::search_attributes() - attribute 0x%04x does not fit current IE, giving up. IE length=%u, Attribute length=%u ([%u..%u])", 
                attribute_type,
                data_length(), 
                attribute_data_length,
                index + CORE_FRAME_WSC_IE_DATA_AREA_OFFSET,
                index + CORE_FRAME_WSC_IE_DATA_AREA_OFFSET + attribute_data_length - 1 );
            return;
            }
            
        switch (attribute_type)
            {
            case CORE_FRAME_WSC_IE_ASSOCIATION_STATE:
                {
                association_state_ind_m = index + CORE_FRAME_WSC_IE_DATA_AREA_OFFSET;
                DEBUG2( "core_frame_wsc_ie_c::search_attributes() - ASSOCIATION_STATE data at index %u (value = %u)", 
                    association_state_ind_m, association_state() );
                break;
                }
                
            case CORE_FRAME_WSC_IE_CONFIG_METHODS:
                {
                config_methods_ind_m = index + CORE_FRAME_WSC_IE_DATA_AREA_OFFSET;
                DEBUG2( "core_frame_wsc_ie_c::search_attributes() - CONFIG_METHODS data at index %u (value = 0x%02X)", 
                    config_methods_ind_m, config_methods() );
                break;
                }
                
            case CORE_FRAME_WSC_IE_CONFIGURATION_ERROR:
                {
                configuration_error_ind_m = index + CORE_FRAME_WSC_IE_DATA_AREA_OFFSET;
                DEBUG2( "core_frame_wsc_ie_c::search_attributes() - CONFIGURATION_ERROR data at index %u (value = %u)", 
                    configuration_error_ind_m, configuration_error() );
                break;
                }
                
            case CORE_FRAME_WSC_IE_DEVICE_NAME:
                {
                device_name_ind_m = index + CORE_FRAME_WSC_IE_DATA_AREA_OFFSET;
                DEBUG1( "core_frame_wsc_ie_c::search_attributes() - DEVICE_NAME data at index %u, data (UTF-8):", 
                    device_name_ind_m );
                const u8_t* debug_device_name = device_name();
                if (debug_device_name)
                    {
                    DEBUG_BUFFER( attribute_data_length, debug_device_name);
                    }
                break;
                }
                
            case CORE_FRAME_WSC_IE_DEVICE_PASSWORD_ID:
                {
                device_password_id_ind_m = index + CORE_FRAME_WSC_IE_DATA_AREA_OFFSET;
                DEBUG2( "core_frame_wsc_ie_c::search_attributes() - DEVICE_PASSWORD_ID data at index %u (value = %u)", 
                    device_password_id_ind_m, device_password_id() );
                break;
                }
                
            case CORE_FRAME_WSC_IE_MANUFACTURER:
                {
                manufacturer_ind_m = index + CORE_FRAME_WSC_IE_DATA_AREA_OFFSET;
                DEBUG1( "core_frame_wsc_ie_c::search_attributes() - MANUFACTURER data at index %u, data (ASCII):", 
                    manufacturer_ind_m );
                const u8_t* debug_manufacturer = manufacturer();
                if (debug_manufacturer)
                    {
                    u16_t length = attribute_data_length;
                    if ( length > CORE_FRAME_WSC_IE_MANUFACTURER_MAX_LENGTH )
                        {
                        length = CORE_FRAME_WSC_IE_MANUFACTURER_MAX_LENGTH;
                        }
                    DEBUG1S("MANUFACTURER: ", length, debug_manufacturer);
                    }
                break;
                }

            case CORE_FRAME_WSC_IE_MODEL_NAME:
                {
                model_name_ind_m = index + CORE_FRAME_WSC_IE_DATA_AREA_OFFSET;
                DEBUG1( "core_frame_wsc_ie_c::search_attributes() - MODEL_NAME data at index %u, data (ASCII):", 
                    model_name_ind_m );
                const u8_t* debug_model_name = model_name();
                if (debug_model_name)
                    {
                    u16_t length = attribute_data_length;
                    if ( length > CORE_FRAME_WSC_IE_MODEL_NAME_MAX_LENGTH )
                        {
                        length = CORE_FRAME_WSC_IE_MODEL_NAME_MAX_LENGTH;
                        }
                    DEBUG1S("MODEL_NAME: ", length, debug_model_name);
                    }
                break;
                }
                
            case CORE_FRAME_WSC_IE_MODEL_NUMBER:
                {
                model_number_ind_m = index + CORE_FRAME_WSC_IE_DATA_AREA_OFFSET;
                DEBUG1( "core_frame_wsc_ie_c::search_attributes() - MODEL_NUMBER data at index %u, data:", 
                    model_number_ind_m );
                const u8_t* debug_model_number = model_number();
                if (debug_model_number)
                    {
                    DEBUG_BUFFER( attribute_data_length, debug_model_number);
                    }
                break;
                }

            case CORE_FRAME_WSC_IE_REQUEST_TYPE:
                {
                request_type_ind_m = index + CORE_FRAME_WSC_IE_DATA_AREA_OFFSET;
                DEBUG2( "core_frame_wsc_ie_c::search_attributes() - REQUEST_TYPE data at index %u (value = %u)", 
                    request_type_ind_m, request_type() );
                break;
                }
                
            case CORE_FRAME_WSC_IE_RESPONSE_TYPE:
                {
                response_type_ind_m = index + CORE_FRAME_WSC_IE_DATA_AREA_OFFSET;
                DEBUG2( "core_frame_wsc_ie_c::search_attributes() - RESPONSE_TYPE data at index %u (value = %u)", 
                    response_type_ind_m, response_type() );
                break;
                }
                
            case CORE_FRAME_WSC_IE_RF_BANDS:
                {
                rf_bands_ind_m = index + CORE_FRAME_WSC_IE_DATA_AREA_OFFSET;
                DEBUG2( "core_frame_wsc_ie_c::search_attributes() - RF_BANDS data at index %u (value = %u)", 
                    rf_bands_ind_m, rf_bands() );
                break;
                }
                
            case CORE_FRAME_WSC_IE_SELECTED_REGISTRAR:
                {
                selected_registrar_ind_m = index + CORE_FRAME_WSC_IE_DATA_AREA_OFFSET;
                DEBUG2( "core_frame_wsc_ie_c::search_attributes() - SELECTED_REGISTRAR data at index %u (value = %u)", 
                    selected_registrar_ind_m, selected_registrar() );
                break;
                }

            case CORE_FRAME_WSC_IE_SERIAL_NUMBER:
                {
                serial_number_ind_m = index + CORE_FRAME_WSC_IE_DATA_AREA_OFFSET;
                DEBUG1( "core_frame_wsc_ie_c::search_attributes() - SERIAL_NUMBER data at index %u, data:", 
                    serial_number_ind_m );
                const u8_t* debug_serial_number = serial_number();
                if (debug_serial_number)
                    {
                    DEBUG_BUFFER( attribute_data_length, debug_serial_number);
                    }
                break;
                }

            case CORE_FRAME_WSC_IE_PROTECTED_SETUP_STATE:
                {
                protected_setup_state_ind_m = index + CORE_FRAME_WSC_IE_DATA_AREA_OFFSET;
                DEBUG2( "core_frame_wsc_ie_c::search_attributes() - PROTECTED_SETUP_STATE data at index %u (value = %u)", 
                    protected_setup_state_ind_m, protected_setup_state() );
                break;
                }
                
            case CORE_FRAME_WSC_IE_UUID_E:
                {
                uuid_e_ind_m = index + CORE_FRAME_WSC_IE_DATA_AREA_OFFSET;
                DEBUG1( "core_frame_wsc_ie_c::search_attributes() - UUID_E data at index %u, data:", 
                    uuid_e_ind_m );
                
                const u8_t* debug_uuid_e = uuid_e();
                if (debug_uuid_e)
                    {
                    DEBUG_BUFFER( attribute_data_length, debug_uuid_e);
                    }
                break;
                }
                
            case CORE_FRAME_WSC_IE_UUID_R:
                {
                uuid_r_ind_m = index + CORE_FRAME_WSC_IE_DATA_AREA_OFFSET;
                DEBUG1( "core_frame_wsc_ie_c::search_attributes() - UUID_R data at index %u, data:", 
                    uuid_r_ind_m );
                
                const u8_t* debug_uuid_r = uuid_r();
                if (debug_uuid_r)
                    {
                    DEBUG_BUFFER( attribute_data_length, debug_uuid_r);
                    }
                break;
                }
                
            case CORE_FRAME_WSC_IE_VERSION:
                {
                version_ind_m = index + CORE_FRAME_WSC_IE_DATA_AREA_OFFSET;
                u8_t wsc_version = version();
                DEBUG3( "core_frame_wsc_ie_c::search_attributes() - VERSION data at index %u (value = %x.%x)", 
                    version_ind_m, (wsc_version&0xF0)>>4, (wsc_version&0x0F) );
                break;
                }
                
            case CORE_FRAME_WSC_IE_SELECTED_REGISTRAR_CONFIG_METHODS:
                {
                selected_registrar_config_methods_ind_m = index + CORE_FRAME_WSC_IE_DATA_AREA_OFFSET;
                DEBUG2( "core_frame_wsc_ie_c::search_attributes() - SELECTED_REGISTRAR_CONFIG_METHODS data at index %u (value = 0x%02X)", 
                    selected_registrar_config_methods_ind_m, selected_registrar_config_methods() );
                break;
                }
                
            case CORE_FRAME_WSC_IE_PRIMARY_DEVICE_TYPE:
                {
                primary_device_type_ind_m = index + CORE_FRAME_WSC_IE_DATA_AREA_OFFSET;
                
                const u8_t* device_type = primary_device_type();
                DEBUG3( "core_frame_wsc_ie_c::search_attributes() - PRIMARY_DEVICE_TYPE data at index %u (CategoryID = %u, SubcategoryID = %u)", 
                    primary_device_type_ind_m,
                    core_tools_c::get_u16_big_endian(
                        device_type, 0),
                    core_tools_c::get_u16_big_endian(
                        device_type, 2+CORE_FRAME_WSC_IE_OUI_LENGTH) );
                break;
                }
                
            case CORE_FRAME_WSC_IE_AP_SETUP_LOCKED:
                {
                ap_setup_locked_ind_m = index + CORE_FRAME_WSC_IE_DATA_AREA_OFFSET;
                DEBUG2( "core_frame_wsc_ie_c::search_attributes() - AP_SETUP_LOCKED data at index %u (value = %u)", 
                    ap_setup_locked_ind_m, ap_setup_locked() );
                break;
                }
                
            default:
                DEBUG3( "core_frame_wsc_ie_c::search_attributes() - unhandled attribute 0x%04x from index %u (length = %u)", 
                    attribute_type, index, attribute_data_length );
            }
        // update index to start of next attribute
        index += CORE_FRAME_WSC_IE_DATA_AREA_OFFSET + attribute_data_length;
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// Generate IE for Association request
void core_frame_wsc_ie_c::generate(
    const u8_t version,
    const u8_t request_type )
    {
    ASSERT( !data_length_m );
    ASSERT( max_data_length_m );   

    core_frame_dot11_ie_c::generate(
        core_frame_dot11_ie_element_id_wpa );

    // WSC OUI field
    core_tools_c::copy(
        &data_m[data_length_m],
        &CORE_FRAME_WSC_IE_OUI[0],
        CORE_FRAME_WSC_IE_OUI_LENGTH );
    data_length_m += CORE_FRAME_WSC_IE_OUI_LENGTH;


    // ------------------------------------
    // Version
    // AttributeType
    u16_t attribute_type = core_tools_c::convert_host_to_big_endian( CORE_FRAME_WSC_IE_VERSION );
    core_tools_c::copy(
        &data_m[data_length_m],
        &attribute_type,
        //reinterpret_cast<u8_t*>(&attribute_type),
        sizeof( attribute_type ) );
    data_length_m += sizeof( attribute_type );
    
    // DataLength
    u16_t data_length = sizeof(version);
    data_length = core_tools_c::convert_host_to_big_endian( data_length );
    core_tools_c::copy(
        &data_m[data_length_m],
        &data_length,
        //reinterpret_cast<u8_t*>(&data_length),
        sizeof( data_length ) );
    data_length_m += sizeof( data_length );
    
    // actual data
    data_m[data_length_m] = version;
    data_length_m += sizeof(version);
    
    // ------------------------------------
    // Request type
    // AttributeType
    attribute_type = core_tools_c::convert_host_to_big_endian( CORE_FRAME_WSC_IE_REQUEST_TYPE );
    core_tools_c::copy(
        &data_m[data_length_m],
        &attribute_type,
        sizeof( attribute_type ) );
    data_length_m += sizeof( attribute_type );
    
    // DataLength
    data_length = sizeof(request_type);
    data_length = core_tools_c::convert_host_to_big_endian( data_length );
    core_tools_c::copy(
        &data_m[data_length_m],
        &data_length,
        sizeof( data_length ) );
    data_length_m += sizeof( data_length );
    
    // actual data
    data_m[data_length_m] = request_type;
    data_length_m += sizeof(request_type);
    
    
    // Set length of whole IE
    set_length( data_length_m );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_wsc_ie_c::core_frame_wsc_ie_c(
    u16_t data_length,
    const u8_t* data,
    u16_t max_data_length ) :
    core_frame_dot11_ie_c( data_length, data, max_data_length ),
    association_state_ind_m( 0 ),
    config_methods_ind_m( 0 ),
    configuration_error_ind_m( 0 ),
    device_name_ind_m( 0 ),
    device_password_id_ind_m( 0 ),
    manufacturer_ind_m( 0 ),
    model_name_ind_m( 0 ),
    model_number_ind_m( 0 ),
    request_type_ind_m( 0 ),
    response_type_ind_m( 0 ),
    rf_bands_ind_m( 0 ),
    selected_registrar_ind_m( 0 ),
    serial_number_ind_m( 0 ),
    protected_setup_state_ind_m( 0 ),
    uuid_e_ind_m( 0 ),
    uuid_r_ind_m( 0 ),
    version_ind_m( 0 ),
    selected_registrar_config_methods_ind_m( 0 ),
    primary_device_type_ind_m( 0 ),
    ap_setup_locked_ind_m( 0 )
    {
    DEBUG( "core_frame_wsc_ie_c::core_frame_wsc_ie_c()" );
    }
