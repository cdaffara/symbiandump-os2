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


#ifndef CORE_FRAME_WAPI_IE_H
#define CORE_FRAME_WAPI_IE_H

#include "abs_core_wpx_adaptation.h"
#include "core_frame_dot11_ie.h"
#include "core_types.h"

/**
 * Class for parsing and generating GB 15629.11 (WAPI) IEs.
 *
 * @since S60 v3.2
 */
NONSHARABLE_CLASS( core_frame_wapi_ie_c ) : public core_frame_dot11_ie_c
    {

public:

    /** Defines the capability bits in capability information field. */
    enum core_wapi_capability_e
        {
        core_wapi_capability_preauthentication = 0x0001
        };

    /**
     * Factory for creating a parser instance.
     *
     * @since S60 v3.2
     * @param ie Pointer to the IE data.
     * @return A pointer to the created IE instance.
     */
    static core_frame_wapi_ie_c* instance(
        const core_frame_dot11_ie_c& ie );

    /**
     * Factory for creating an IE with the given parameters.
     *
     * @since S60 v3.2
     * @param group_cipher Used group cipher.
     * @param pairwise_cipher Used pairwise cipher.
     * @param key_management Used key management suite.
     * @param capability Capability information. 
     * @param bkid_length Length of optional BKID data.
     * @param bkid_data Pointer to the buffer of optional BKID data.     
     * @return A pointer to the created IE instance.
     */
    static core_frame_wapi_ie_c* instance(
        core_cipher_suite_e group_cipher,
        core_cipher_suite_e pairwise_cipher,
        core_key_management_e key_management,
        u16_t capability,
        u16_t bkid_length,
        const u8_t* bkid_data );

    /** 
     * Destructor.
     */
    virtual ~core_frame_wapi_ie_c();

    /**
     * Return the supported group cipher.
     * @return The supported group cipher.
     */
    u8_t group_cipher_suite() const;

    /**
     * Return the supported pairwise ciphers.
     * @return The supported pairwise ciphers.
     */
    u8_t pairwise_cipher_suites() const;

    /**
     * Return the supported key management suites.
     * @return The supported key management suites.
     */
    u8_t key_management_suites() const;

    /**
     * Return the capability information.
     * @return The capability information.
     */    
    u16_t capability() const;
    
private:

    /**
     * Return the offset for pairwise cipher count.
     * @return Offset of pairwise cipher count, 0 if invalid.
     */
    u8_t pairwise_cipher_count_offset() const;

    /**
     * Return the offset for group cipher.
     * @return Offset of group cipher, 0 if invalid.
     */
    u8_t group_cipher_offset() const;

    /**
     * Convert OUI to core_cipher_suite_e type.
     * @param oui OUI to convert.
     * @return OUI convert to core_cipher_suite_e type.
     */    
    core_cipher_suite_e cipher_oui_to_enum(
        const u8_t* oui ) const;

    /**
     * Convert OUI to core_key_management_e type.
     * @param oui OUI to convert.
     * @return OUI converted to core_key_management_e type.
     */
    core_key_management_e key_management_oui_to_enum(
        const u8_t* oui ) const;

    /**
     * Return the "raw" value for the given group cipher.
     * @param cipher The cipher to return the value to.
     * @return The "raw" value for the given group cipher.
     */
    u8_t get_raw_group_cipher(
        core_cipher_suite_e cipher ) const;

    /**
     * Return the "raw" value for the given pairwise cipher.
     * @param cipher The cipher to return the value to.
     * @return The "raw" value for the given pairwise cipher.
     */
    u8_t get_raw_pairwise_cipher(
        core_cipher_suite_e cipher ) const;

    /**
     * Return the "raw" value for the given key management suite.
     * @param key_management The key management suite to return the value to.
     * @return The "raw" value for the given key management suite.
     */
    u8_t get_raw_key_management(
        core_key_management_e key_management ) const;

    /**
     * Generate an GB 15629.11 (WAPI) IEs.
     *
     * @param group_cipher Used group cipher.
     * @param pairwise_cipher Used pairwise cipher.
     * @param key_management Used key management suite.
     * @param capability Capability information.
     * @param bkid_length Length of optional PMKID data.
     * @param bkid_data Pointer to the buffer of optional PMKID data.
     */
    void generate(
        core_cipher_suite_e group_cipher,
        core_cipher_suite_e pairwise_cipher,
        core_key_management_e key_management,
        u16_t capability,
        u16_t bkid_length,
        const u8_t* bkid_data );

    /**
     * Constructor
     *
     * @param data_length Length of the IE data.
     * @param data Pointer to the IE data.
     */
    core_frame_wapi_ie_c(
        u16_t data_length,
        const u8_t* data,
        u16_t max_data_length );

    };

#endif // core_frame_wapi_ie_H
