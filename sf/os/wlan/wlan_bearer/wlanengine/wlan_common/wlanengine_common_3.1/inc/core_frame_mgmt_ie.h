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
* Description:  Class for parsing 802.11 management frame IEs.
*
*/


#ifndef CORE_FRAME_MGMT_IE_H
#define CORE_FRAME_MGMT_IE_H

#include "core_frame_dot11_ie.h"
#include "core_types.h"

const u16_t CORE_FRAME_RADIO_MGMT_IE_LENGTH = 2;
const u16_t CORE_FRAME_RADIO_MGMT_IE_MIN_POWER_CAPABILITY = 0;
const u16_t CORE_FRAME_RRM_MGMT_IE_LENGTH = 5;

const u8_t CORE_FRAME_MGMT_IE_AP_CHANNEL_REPORT_LENGTH = 4;
const u8_t CORE_FRAME_MGMT_IE_AP_CHANNEL_REPORT_CLASS_OFFSET = 2;
const u8_t CORE_FRAME_MGMT_IE_AP_CHANNEL_REPORT_CHANNEL_OFFSET = 3;

/**
 * Class for parsing 802.11 management frame SSID IE.
 *
 * @since S60 v3.1
 */
NONSHARABLE_CLASS( core_frame_mgmt_ie_ssid_c ) : public core_frame_dot11_ie_c
    {

public:

    /**
     * Factory for creating a parser instance.
     *
     * @since S60 v3.1
     * @param ie Pointer to the IE data.
     * @return A pointer to the created IE instance.
     */
    static core_frame_mgmt_ie_ssid_c* instance(
        const core_frame_dot11_ie_c& ie );

    /** 
     * Destructor.
     */
    virtual ~core_frame_mgmt_ie_ssid_c();

    /**
     * Return the SSID.
     * @return The SSID. BROADCAST_SSID if none or invalid.
     */
    core_ssid_s ssid() const;

private:

    /**
     * Constructor
     *
     * @param data_length Length of the IE data.
     * @param data Pointer to the IE data.     
     */
    core_frame_mgmt_ie_ssid_c(
       u16_t data_length,
       const u8_t* data,
       u16_t max_data_length );

    };

/**
 * Class for parsing 802.11 management frame DS IE.
 *
 * @since S60 v3.1
 */
NONSHARABLE_CLASS( core_frame_mgmt_ie_ds_c ) : public core_frame_dot11_ie_c
    {

public:

    /**
     * Factory for creating a parser instance.
     *
     * @since S60 v3.1
     * @param ie Pointer to the IE data.
     * @return A pointer to the created IE instance.
     */
    static core_frame_mgmt_ie_ds_c* instance(
        const core_frame_dot11_ie_c& ie );

    /** 
     * Destructor.
     */
    virtual ~core_frame_mgmt_ie_ds_c();

    /**
     * Return the channel field.
     * @return The channel field.
     */
    u8_t channel() const;

private:

    /**
     * Constructor
     *
     * @param data_length Length of the IE data.
     * @param data Pointer to the IE data.     
     */
    core_frame_mgmt_ie_ds_c(
       u16_t data_length,
       const u8_t* data,
       u16_t max_data_length );

    };

/**
 * Class for parsing 802.11 management frame country IE.
 *
 * @since S60 v3.1
 */
NONSHARABLE_CLASS( core_frame_mgmt_ie_country_c ) : public core_frame_dot11_ie_c
    {

public:

    /**
     * Factory for creating a parser instance.
     *
     * @since S60 v3.1
     * @param ie Pointer to the IE data.
     * @return A pointer to the created IE instance.
     */
    static core_frame_mgmt_ie_country_c* instance(
        const core_frame_dot11_ie_c& ie );

    /** 
     * Destructor.
     */
    virtual ~core_frame_mgmt_ie_country_c();

    /**
     * Return the country string field.
     * @return The country string field.
     */
    core_country_string_s country() const;
    
    /**
     * Return the maximum transmit power level for the given channel.
     * @param channel Channel to check.
     * @return The maximum transmit power level for the given channel.
     */    
    u8_t max_tx_power_level(
        u8_t channel ) const;

private:

    /**
     * Constructor
     *
     * @param data_length Length of the IE data.
     * @param data Pointer to the IE data.     
     */
    core_frame_mgmt_ie_country_c(
       u16_t data_length,
       const u8_t* data,
       u16_t max_data_length );

    };

/**
 * Class for parsing 802.11 management frame power constraint IE.
 *
 * @since S60 v5.2
 */
NONSHARABLE_CLASS( core_frame_mgmt_ie_power_constraint_c ) : public core_frame_dot11_ie_c
    {

public:

    /**
     * Factory for creating a parser instance.
     *
     * @since S60 v5.2
     * @param ie Pointer to the IE data.
     * @return A pointer to the created IE instance.
     */
    static core_frame_mgmt_ie_power_constraint_c* instance(
        const core_frame_dot11_ie_c& ie );

    /** 
     * Destructor.
     */
    virtual ~core_frame_mgmt_ie_power_constraint_c();

    /**
     * Return the power constraint value.
     * @return The power constraint value.
     */
    u8_t power_constraint() const;

private:

    /**
     * Constructor
     *
     * @param data_length Length of the IE data.
     * @param data Pointer to the IE data.     
     */
    core_frame_mgmt_ie_power_constraint_c(
       u16_t data_length,
       const u8_t* data,
       u16_t max_data_length );

    };

/**
 * Class for parsing 802.11 management frame ap channel report IE.
 *
 * @since S60 v5.2
 */
NONSHARABLE_CLASS( core_frame_mgmt_ie_ap_channel_report_c ) : public core_frame_dot11_ie_c
    {

public:

    enum core_frame_mgmt_ie_class_id_e
        {
        core_frame_mgmt_ie_class_id_europe_2dot407          = 4,
        core_frame_mgmt_ie_class_id_usa_5                   = 5,
        core_frame_mgmt_ie_class_id_usa_2dot407             = 12,
        core_frame_mgmt_ie_class_id_japan_2dot407           = 30,
        core_frame_mgmt_ie_class_id_japan_2dot414           = 31,
        core_frame_mgmt_ie_class_id_japan_5                 = 32
        };
    
    /**
     * Factory for creating a parser instance.
     *
     * @since S60 v5.2
     * @param ie Pointer to the IE data.
     * @return A pointer to the created IE instance.
     */
    static core_frame_mgmt_ie_ap_channel_report_c* instance(
        const core_frame_dot11_ie_c& ie );

    /** 
     * Destructor.
     */
    virtual ~core_frame_mgmt_ie_ap_channel_report_c();

    /**
     * Return the class value.
     * 
     * @since S60 v5.2
     * @return The class value.
     */
    u8_t ap_channel_report_class() const;

    /**
     * Return the channel value.
     * 
     * @since S60 v5.2
     * @return The channel value.
     */
    u8_t ap_channel_report_channel(
        u16_t channel_offset ) const;

private:

    /**
     * Constructor
     *
     * @param data_length Length of the IE data.
     * @param data Pointer to the IE data.     
     */
    core_frame_mgmt_ie_ap_channel_report_c(
       u16_t data_length,
       const u8_t* data,
       u16_t max_data_length );

    };

/**
 * Class for parsing 802.11 management frame admission capacity IE.
 *
 * @since S60 v5.2
 */
NONSHARABLE_CLASS( core_frame_mgmt_ie_admission_capacity_c ) : public core_frame_dot11_ie_c
    {

public:

    /**
     * Factory for creating a parser instance.
     *
     * @since S60 v5.1
     * @param ie Pointer to the IE data.
     * @return A pointer to the created IE instance.
     */
    static core_frame_mgmt_ie_admission_capacity_c* instance(
        const core_frame_dot11_ie_c& ie );

    /** 
     * Destructor.
     */
    virtual ~core_frame_mgmt_ie_admission_capacity_c();

    /**
     * Return the admission capacity bitmask.
     * 
     * @since S60 v5.2
     * @return The admission capacity bitmask.
     */
    u16_t admission_capacity_bitmask() const;

    /**
     * Get admission capacities.
     * 
     * @since S60 v5.2
     * @param ie_data_length IE data length.
     * @param admission_capacity_table Admission capacities.
     */
    void get_admission_capacity(
        u16_t ie_data_length,
        u16_t* admission_capacity_table );

private:

    /**
     * Constructor
     *
     * @param data_length Length of the IE data.
     * @param data Pointer to the IE data.     
     */
    core_frame_mgmt_ie_admission_capacity_c(
       u16_t data_length,
       const u8_t* data,
       u16_t max_data_length );

    };

/**
 * Class for parsing 802.11 management frame power capability IE.
 *
 * @since S60 v5.2
 */
NONSHARABLE_CLASS( core_frame_mgmt_ie_power_capability_c ) : public core_frame_dot11_ie_c
    {

public:

    /**
     * Factory for creating a parser instance.
     *
     * @since S60 v5.2
     * @param ie Pointer to the IE data.
     * @return A pointer to the created IE instance.
     */
    static core_frame_mgmt_ie_power_capability_c* instance(
        const core_frame_dot11_ie_c& ie );

    /** 
     * Destructor.
     */
    virtual ~core_frame_mgmt_ie_power_capability_c();

    /**
     * Return the power capability.
     * 
     * @since S60 v5.2
     * @return The power capability.
     */
    u8_t power_capability() const;

private:

    /**
     * Constructor
     *
     * @param data_length Length of the IE data.
     * @param data Pointer to the IE data.     
     * @param max_data_length Maximum length of the IE data.
     */
    core_frame_mgmt_ie_power_capability_c(
       u16_t data_length,
       const u8_t* data,
       u16_t max_data_length );

    };

/**
 * Class for parsing 802.11 radio management frame IE.
 *
 * @since S60 v5.2
 */
NONSHARABLE_CLASS( core_frame_radio_mgmt_ie_c ) : public core_frame_dot11_ie_c
    {

public:

    /**
     * Factory for creating a parser instance.
     *
     * @since S60 v5.2
     * @param max_data_length Maximum length of the data.
     * @param min_capability Minimum transmit power capability.
     * @param max_capability Maximum transmit power capability.
     * @return A pointer to the created IE instance.
     */
    static core_frame_radio_mgmt_ie_c* instance(
        u16_t max_data_length,
        u8_t min_capability,
        u8_t max_capability );

    /** 
     * Destructor.
     */
    virtual ~core_frame_radio_mgmt_ie_c();

private:

    /**
     * Generate an IE with the given parameters.
     *
     * @param min_capability Minimum transmit power capability.
     * @param max_capability Maximum transmit power capability.
     */
    void generate(
        u8_t min_capability,
        u8_t max_capability );

    /**
     * Constructor
     *
     * @param data_length Length of the IE data.
     * @param data Pointer to the IE data.     
     * @param max_data_length Maximum length of the IE data.
     */
    core_frame_radio_mgmt_ie_c(
       u16_t data_length,
       const u8_t* data,
       u16_t max_data_length );

    };

/**
 * Class for parsing 802.11 management frame rrm capabilities IE.
 *
 * @since S60 v5.2
 */
NONSHARABLE_CLASS( core_frame_mgmt_ie_rrm_capabilities_c ) : public core_frame_dot11_ie_c
    {

public:

    /**
     * Factory for creating a parser instance.
     *
     * @since S60 v5.2
     * @param ie Pointer to the IE data.
     * @return A pointer to the created IE instance.
     */
    static core_frame_mgmt_ie_rrm_capabilities_c* instance(
        const core_frame_dot11_ie_c& ie );

    /** 
     * Destructor.
     */
    virtual ~core_frame_mgmt_ie_rrm_capabilities_c();

    /**
     * Return the rrm capability bits.
     * 
     * @since S60 v5.2
     * @return The rrm capability bits.
     */
    u64_t rrm_capabilities() const;

private:

    /**
     * Constructor
     *
     * @param data_length Length of the IE data.
     * @param data Pointer to the IE data.     
     * @param max_data_length Maximum length of the IE data.
     */
    core_frame_mgmt_ie_rrm_capabilities_c(
       u16_t data_length,
       const u8_t* data,
       u16_t max_data_length );

    };

/**
 * Class for parsing 802.11 radio management frame IE.
 *
 * @since S60 v5.2
 */
NONSHARABLE_CLASS( core_frame_rrm_mgmt_ie_c ) : public core_frame_dot11_ie_c
    {

public:

    /**
     * Factory for creating a parser instance.
     *
     * @since S60 v5.2
     * @param rrm_capabilities RRM capabilities.
     * @return A pointer to the created IE instance.
     */
    static core_frame_rrm_mgmt_ie_c* instance(
        const u8_t* rrm_capabilities );

    /** 
     * Destructor.
     */
    virtual ~core_frame_rrm_mgmt_ie_c();

private:

    /**
     * Generate an IE with the given parameters.
     *
     * @since S60 v5.2
     * @param rrm_capabilities RRM capabilities.
     */
    void generate( 
        const u8_t* rrm_capabilities );

    /**
     * Constructor
     *
     * @param data_length Length of the IE data.
     * @param data Pointer to the IE data.     
     * @param max_data_length Maximum length of the IE data.
     */
    core_frame_rrm_mgmt_ie_c(
       u16_t data_length,
       const u8_t* data,
       u16_t max_data_length );

    };

/**
 * Class for parsing 802.11 management frame request IE.
 *
 * @since S60 v5.2
 */
NONSHARABLE_CLASS( core_frame_mgmt_ie_request_ie_c ) : public core_frame_dot11_ie_c
    {

public:

    /**
     * Factory for creating a parser instance.
     *
     * @since S60 v5.2
     * @param ie Pointer to the IE data.
     * @return A pointer to the created IE instance.
     */
    static core_frame_mgmt_ie_request_ie_c* instance(
        const core_frame_dot11_ie_c& ie );

    /** 
     * Destructor.
     */
    virtual ~core_frame_mgmt_ie_request_ie_c();

    /**
     * Get information element list.
     * 
     * @since S60 v5.2
     * @param ie_id_list Information element list.
     * @return Information element list length.
     */
    u8_t element_list( u8_t* ie_id_list ) const;

    /**
     * Return the element data.
     * 
     * @since S60 v5.2
     * @param request_ie_offset Element data offset.
     * @return The element data.
     */
    u8_t element_data(
        u16_t element_data_offset ) const;

private:

    /**
     * Constructor
     *
     * @param data_length Length of the IE data.
     * @param data Pointer to the IE data.     
     */
    core_frame_mgmt_ie_request_ie_c(
       u16_t data_length,
       const u8_t* data,
       u16_t max_data_length );

    };

#endif // CORE_FRAME_MGMT_IE_H
