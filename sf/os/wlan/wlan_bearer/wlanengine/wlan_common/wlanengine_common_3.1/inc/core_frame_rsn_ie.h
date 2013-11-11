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


#ifndef CORE_FRAME_RSN_IE_H
#define CORE_FRAME_RSN_IE_H

#include "abs_core_wpx_adaptation.h"
#include "core_frame_dot11_ie.h"
#include "core_types.h"

/**
 * Class for parsing and generating 802.11i (RSN) IEs.
 *
 * @since S60 v3.1
 */
NONSHARABLE_CLASS( core_frame_rsn_ie_c ) : public core_frame_dot11_ie_c
    {

public:

    /**
     * Factory for creating a parser instance.
     *
     * @since S60 v3.1
     * @param wpx_adaptation WPX adaptation.
     * @param ie Pointer to the IE data.
     * @return A pointer to the created IE instance.
     */
    static core_frame_rsn_ie_c* instance(
        abs_core_wpx_adaptation_c& wpx_adaptation,
        const core_frame_dot11_ie_c& ie );

    /**
     * Factory for creating an IE with the given parameters.
     *
     * @since S60 v3.1
     * @param wpx_adaptation WPX adaptation.
     * @param group_cipher Used group cipher.
     * @param pairwise_cipher Used pairwise cipher.
     * @param key_management Used key management suite.
     * @param pmkid_length Length of optional PMKID data.
     * @param pmkid_data Pointer to the buffer of optional PMKID data.     
     * @return A pointer to the created IE instance.
     */
    static core_frame_rsn_ie_c* instance(
        abs_core_wpx_adaptation_c& wpx_adaptation,
        core_cipher_suite_e group_cipher,
        core_cipher_suite_e pairwise_cipher,
        core_key_management_e key_management,
        u16_t pmkid_length,
        const u8_t* pmkid_data );

    /** 
     * Destructor.
     */
    virtual ~core_frame_rsn_ie_c();

    /**
     * Return the supported group cipher.
     * @return The supported group cipher.
     */
    u8_t group_cipher_suite() const;

    /**
     * Return the supported pairwise ciphers.
     * @param The supported pairwise ciphers.
     */
    u8_t pairwise_cipher_suites() const;

    /**
     * Return the supported key management suites.
     * @param The supported key management suites.
     */
    u8_t key_management_suites() const;

private:

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
     * Generate a 802.11i (RSN) IE.
     *
     * @param group_cipher Used group cipher.
     * @param pairwise_cipher Used pairwise cipher.
     * @param key_management Used key management suite.
     * @param pmkid_length Length of optional PMKID data.
     * @param pmkid_data Pointer to the buffer of optional PMKID data.
     */
    void generate(
        core_cipher_suite_e group_cipher,
        core_cipher_suite_e pairwise_cipher,
        core_key_management_e key_management,
        u16_t pmkid_length,
        const u8_t* pmkid_dat );

    /**
     * Constructor
     *
     * @param wpx_adaptation WPX adaptation.
     * @param data_length Length of the IE data.
     * @param data Pointer to the IE data.     
     */
    core_frame_rsn_ie_c(
        abs_core_wpx_adaptation_c& wpx_adaptation,
        u16_t data_length,
        const u8_t* data,
        u16_t max_data_length );

private: // data

    /** WPX adaptation instance. */
    abs_core_wpx_adaptation_c& wpx_adaptation_m;

    };

#endif // CORE_FRAME_RSN_IE_H
