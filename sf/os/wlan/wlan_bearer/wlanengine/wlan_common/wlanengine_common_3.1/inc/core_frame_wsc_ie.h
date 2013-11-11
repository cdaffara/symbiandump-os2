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


#ifndef CORE_FRAME_WSC_IE_H
#define CORE_FRAME_WSC_IE_H

#include "core_frame_dot11_ie.h"
#include "core_types.h"

// Compile time assert. Used to make sure that datatypes in generate are correct size.
#define COMPILE_ASSERT(e) typedef char __C_ASSERT__[(e)?1:-1]


const u8_t CORE_FRAME_WSC_IE_USED_VERSION = 0x10;
const u8_t CORE_FRAME_WSC_IE_REQUEST_TYPE_ENROLLEE = 0x01;

/**
 * Class for parsing and generating WSC IEs (Wi-Fi Protected Setup).
 *
 * @since S60 v3.2
 */
NONSHARABLE_CLASS( core_frame_wsc_ie_c ) : public core_frame_dot11_ie_c
    {

public:

    enum core_frame_wsc_ie_config_method_e
        {
        core_frame_wsc_ie_config_method_usba            = 0x0001,
        core_frame_wsc_ie_config_method_ethernet        = 0x0002,
        core_frame_wsc_ie_config_method_pin_label       = 0x0004,
        core_frame_wsc_ie_config_method_pin_display     = 0x0008,
        core_frame_wsc_ie_config_method_nfc_ext_token   = 0x0010,
        core_frame_wsc_ie_config_method_nfc_int_token   = 0x0020,
        core_frame_wsc_ie_config_method_nfc_interface   = 0x0040,
        core_frame_wsc_ie_config_method_push_button     = 0x0080,
        core_frame_wsc_ie_config_method_keypad          = 0x0100
        };

    /**
     * Factory for creating a parser instance.
     *
     * @since S60 v3.2
     * @param ie Pointer to the IE data.
     * @return A pointer to the created IE instance.
     */
    static core_frame_wsc_ie_c* instance(
        const core_frame_dot11_ie_c& ie );

    /**
     * Factory for creating an IE with the given parameters.
     *
     * @since S60 v3.2
     * @param version Used Protected Setup version number.
     * @param request_type Specifies the mode in which the device will operate in this setup exchange.
     * @return A pointer to the created IE instance.
     */
    static core_frame_wsc_ie_c* instance(
        const u8_t version,
        const u8_t request_type );

    /** 
     * Destructor.
     */
    virtual ~core_frame_wsc_ie_c();

    /**
     * Following getters are used to read values from IE.
     * Conversion from Big endian to host order is done in these methods.
     * 
     * @return Attribute value or pointer to attribute data.
     */
    u16_t association_state() const;
    u16_t config_methods() const;
    u16_t configuration_error() const;
    const u8_t* device_name() const;
    u16_t device_password_id() const;
    const u8_t* manufacturer() const;
    const u8_t* model_name() const;
    const u8_t* model_number() const;
    u8_t request_type() const;
    u8_t response_type() const;
    u8_t rf_bands() const;
    bool_t selected_registrar() const;
    const u8_t* serial_number() const;
    u8_t protected_setup_state() const;
    const u8_t* uuid_e() const;
    const u8_t* uuid_r() const;
    u8_t version() const;
    u16_t selected_registrar_config_methods() const;
    const u8_t* primary_device_type() const;
    bool_t ap_setup_locked() const;

private:

    /**
     * Search through data and stores Attribute indexes to members.
     */
    void search_attributes();
    
    /**
     * Generate IE for Association request.
     *
     * @param version Used Protected Setup version number.
     * @param request_type Specifies the mode in which the device will operate in this setup exchange.
     */
    void generate(
        const u8_t version,
        const u8_t request_type );

    /**
     * Constructor
     *
     * @param data_length Length of the IE data.
     * @param data Pointer to the IE data.     
     */
    core_frame_wsc_ie_c(
       u16_t data_length,
       const u8_t* data,
       u16_t max_data_length );
       
private: // Data

    /* 
     * Indexes to data fields within IE.
     * These indexes are used in getters for easy access to attributes.
     */
    u8_t association_state_ind_m;
    u8_t config_methods_ind_m;
    u8_t configuration_error_ind_m;
    u8_t device_name_ind_m;
    u8_t device_password_id_ind_m;
    u8_t manufacturer_ind_m;
    u8_t model_name_ind_m;
    u8_t model_number_ind_m;
    u8_t request_type_ind_m;
    u8_t response_type_ind_m;
    u8_t rf_bands_ind_m;
    u8_t selected_registrar_ind_m;
    u8_t serial_number_ind_m;
    u8_t protected_setup_state_ind_m;
    u8_t uuid_e_ind_m;
    u8_t uuid_r_ind_m;
    u8_t version_ind_m;
    u8_t selected_registrar_config_methods_ind_m;
    u8_t primary_device_type_ind_m;
    u8_t ap_setup_locked_ind_m;

    };


#endif // CORE_FRAME_WSC_IE_H
