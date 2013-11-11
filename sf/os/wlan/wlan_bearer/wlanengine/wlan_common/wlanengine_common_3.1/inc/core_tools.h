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


#ifndef CORE_TOOLS_H
#define CORE_TOOLS_H

#include "am_platform_libraries.h"
#include "core_types.h"
#include "core_iap_data.h"
#include "core_ap_data.h"
#include "genscanlist.h"

/**
* core_tools_c offers generic utilities
*
* @lib wlmserversrv.lib
* @since Series 60 3.0
*/
NONSHARABLE_CLASS(core_tools_c)
    {
    public: // New functions

        /**
        * compares two arrays of data
        * @param pl Pointer to parameter1 data.
        * @param ll Length of parameter1 data.
        * @param pr Pointer to parameter2 data.
        * @param rl Length of parameter2 data.
        * @return 0 if data is same or similar
        *         !0 if data is not similar
        */        
        static int_t compare(
            const u8_t* pl,
            int_t ll, 
            const u8_t* pr, 
            int_t rl );

        /**
        * copies data from source buffer to target buffer
        * @param trg Pointer to target buffer.
        * @param src Pointer to source buffer.
        * @param len Length of data to copy in bytes.
        * @return pointer to end of target
        */
        static u8_t* copy(
            void* trg,
            const void* src,
            int_t len );

        /**
        * fills buffer with zeroes
        * @param trg Pointer to buffer.
        * @param len Length of buffer in bytes.
        */
        static void fillz(
            void* trg, 
            int_t len );

        /**
         * Convert the given parameter to big-endian order.
         * @param source integer
         * @return converted integer
         */
        static u16_t convert_host_to_big_endian(
            u16_t src );

        /**
         * Convert the given parameter to big-endian order.
         * @param source integer
         * @return converted integer
         */           
        static u32_t convert_host_to_big_endian(
            u32_t src );

        /**
         * Convert the given parameter to big-endian order.
         * @param source integer
         * @return converted integer
         */
        static u16_t convert_big_endian_to_host(
            u16_t src );
            
        /**
         * Convert the given parameter to big-endian order.
         * @param source integer
         * @return converted integer
         */
        static u32_t convert_big_endian_to_host(
            u32_t src );
            
        /**
         * Return a big endian word from the given buffer index in host order.
         *
         * @since S60 v3.1
         * @param pointer to data
         * @param index in the buffer
         * @return A word from the given buffer index in host order.
         */    
        static u16_t get_u16_big_endian(
            const u8_t* data,
            u16_t index );                    

        /**
         * Return a host order word from the given buffer index.
         *
         * @since S60 v3.1
         * @param pointer to data
         * @param index in the buffer
         * @return A word from the given buffer index in host order.
         */    
        static u16_t get_u16(
            const u8_t* data,
            u16_t index );

        /**
         * Return a big endian long word from the given buffer index in host order.
         *
         * @since S60 v3.1
         * @param pointer to data
         * @param index in the buffer
         * @return A long word from the given buffer index in host order.
         */
        static u32_t get_u32_big_endian(
            const u8_t* data,
            u16_t index );          

        /**
         * Return a host order from the given buffer index.
         *
         * @since S60 v3.1
         * @param pointer to data
         * @param index in the buffer
         * @return A long word from the given buffer index in host order.
         */
        static u32_t get_u32(
            const u8_t* data,
            u16_t index ); 

        /**
         * Return a host order from the given buffer index.
         *
         * @since S60 v5.2
         * @param pointer to data
         * @param index in the buffer
         * @return A quadruple word from the given buffer index in host order.
         */
        static u64_t get_u64(
            const u8_t* data,
            u16_t index ); 

        /**
         * Insert a big endian u16_t to the given buffer index.
         *
         * @since S60 v3.2
         * @param data Pointer to the buffer.
         * @param index Index in the buffer.
         * @param value Data be to appended.
         */
        static void insert_u16_big_endian(
            u8_t* data,
            u16_t index,
            u16_t value );

        /**
         * Insert a host order u16_t to the given buffer index.
         *
         * @since S60 v3.2
         * @param data Pointer to the buffer.
         * @param index Index in the buffer.
         * @param value Data be to appended.
         */
        static void insert_u16(
            u8_t* data,
            u16_t index,
            u16_t value );

        /**
         * Insert a big endian u32_t to the given buffer index.
         *
         * @since S60 v3.2
         * @param data Pointer to the buffer.
         * @param index Index in the buffer.
         * @param value Data be to appended.
         */
        static void insert_u32_big_endian(
            u8_t* data,
            u16_t index,
            u32_t value );

        /**
         * Insert a host order u32_t to the given buffer index.
         *
         * @since S60 v3.2
         * @param data Pointer to the buffer.
         * @param index Index in the buffer.
         * @param value Data be to appended.
         */
        static void insert_u32(
            u8_t* data,
            u16_t index,
            u32_t value );

        /**
         * Append a big endian u16_t to the given buffer.
         *
         * @since S60 v3.2
         * @param data Pointer to the buffer.
         * @param data_length Current buffer length.
         * @param value Data be to appended.
         */
        static void append_u16_big_endian(
            u8_t* data,
            u16_t& data_length,
            u16_t value );            

        /**
         * Append a host order u16_t to the given buffer.
         *
         * @since S60 v3.2
         * @param data Pointer to the buffer.
         * @param data_length Current buffer length.
         * @param value Data be to appended.
         */
        static void append_u16(
            u8_t* data,
            u16_t& data_length,
            u16_t value );

        /**
         * Append a big endian u32_t to the given buffer.
         *
         * @since S60 v3.2
         * @param data Pointer to the buffer.
         * @param data_length Current buffer length.
         * @param value Data be to appended.
         */
        static void append_u32_big_endian(
            u8_t* data,
            u16_t& data_length,
            u32_t value );            

        /**
         * Append a host order u32_t to the given buffer.
         *
         * @since S60 v3.2
         * @param data Pointer to the buffer.
         * @param data_length Current buffer length.
         * @param value Data be to appended.
         */
        static void append_u32(
            u8_t* data,
            u16_t& data_length,
            u32_t value );

        /**
         * Convert the given cipher suite to matching cipher key type.
         * 
         * @since S60 v3.1
         * @param cipher Cipher suite to convert.
         * @return The type of the cipher key. 
         */        
        static core_cipher_key_type_e cipher_key_type(
            core_cipher_suite_e cipher );

        /**
         * Determine the type of the cipher key from the given parameters.
         *
         * @since S60 v3.1
         * @param type Type of the key reported by EAPOL.
         * @param pairwise_cipher The currently used pairwise cipher.
         * @param group_cipher The currently used group cipher.
         * @return The type of the cipher key.
         */
        static core_cipher_key_type_e cipher_key_type(
            wlan_eapol_if_eapol_key_type_e type,
            core_cipher_suite_e pairwise_cipher,
            core_cipher_suite_e group_cipher );            

        /**
         * Determine the EAPOL authentication type from the given parameters.
         *
         * @since S60 v3.1
         * @param iap_data Information about the used IAP.         
         * @param ap_data Information about the used AP.
         * @retun The authentication type.
         */        
        static wlan_eapol_if_eapol_key_authentication_type_e eap_authentication_type(
            const core_iap_data_c& iap_data,
            const core_ap_data_c& ap_data );

        /**
         * Convert the given cipher suite to the EAPOL equivalent.
         *
         * @since S60 v3.1
         * @param cipher Cipher suite to convert.
         * @return The converted cipher suite.
         */
        static wlan_eapol_if_rsna_cipher_e eapol_cipher(
            core_cipher_suite_e cipher );

        /**
         * Add the given beacon to the scan list.
         *
         * @since S60 v3.1
         * @param scan_list Scan list where the beacon is added.
         * @param ap_data Beacon to be added.
         * @param rcpi RCPI value of the beacon.
         */
        static void add_beacon_to_scan_list(
            ScanList& scan_list,
            const core_ap_data_c& ap_data,
            u32_t rcpi );            

        /**
         * Convert the given user priority value to an access class.
         *
         * @since S60 v3.1
         * @param user_priority User priority value to be converted.
         * @return The corresponding access class.
         */
        static core_access_class_e convert_user_priority_to_ac(
            u8_t user_priority );

        /**
         * Convert the given access class to an user priority value.
         *
         * @since S60 v3.2
         * @param access_class Access class to be converted
         * @return The corresponding user priority value.
         */
        static u8_t convert_ac_to_user_priority(
            core_access_class_e access_class );

        /**
         * Convert the given tx rate value (500kbit/s per unit) to a corresponding enum.
         *
         * @since S60 v3.2
         * @param tx_rate Tx rate value to be converted.
         * @return Corresponding tx rate enum.
         */
        static core_tx_rate_e convert_tx_rate_to_tx_rate_enum(
            u8_t tx_rate );

        /**
         * Convert the given tx rate enum to a corresponding tx rate value (500kbit/s per unit).
         *
         * @since S60 v3.2
         * @param tx_rate Tx rate enum to be converted.
         * @return Corresponding rx rate value.
         */
        static u8_t convert_tx_rate_enum_to_tx_rate(
            core_tx_rate_e tx_rate );

        /**
         * Return the highest tx rate defined.
         *
         * @since S60 v3.2
         * @param tx_rates Bitmask of tx rates.
         * @return The highest tx rate defined.
         */
        static core_tx_rate_e highest_tx_rate(
            u32_t tx_rates );

        /**
         * Convert the tx rates to a tx policy.
         *
         * @since S60 v3.2
         * @param tx_rates Bitmask of tx rates.
         * @return Corresponding tx rate policy.
         */
        static core_tx_rate_policy_s convert_tx_rates_to_tx_policy(
            u32_t tx_rates );

        /**
         * Determine the security mode from the given parameters.
         *
         * @since S60 v3.2
         * @param iap_data Information about the used IAP.
         * @param ap_data Information about the used AP.
         * @retun The security mode.
         */        
        static core_connection_security_mode_e security_mode(
            const core_iap_data_c& iap_data,
            const core_ap_data_c& ap_data );

        /**
         * Convert an EAPOL error code to the the corresponding
         * protected setup error code.
         *
         * @since S60 v3.2
         * @param error EAPOL error code.
         * @return The corresponding protected setup status code.
         */
         static core_protected_setup_status_e convert_eapol_error_to_protected_setup_status(
             const wlan_eapol_if_eap_status_e error );

         /**
          * Check which WLAN region the country information received from AP matches.
          * @since S60 5.0
          * @param found_country Country information received from AP.
          * @return WLAN region converted from the country information.
          */  
         static core_wlan_region_e convert_country_to_region(
         	    const core_country_string_s& found_country );
         
    
    };

#include "core_tools.inl"

#endif // CORE_TOOLS_H 
