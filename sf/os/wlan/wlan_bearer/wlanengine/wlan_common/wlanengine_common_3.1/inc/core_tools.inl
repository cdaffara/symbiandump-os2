/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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

/*
* %version: 5 %
*/

// ============================ MEMBER FUNCTIONS ===============================

/**
 * operator== for core_mac_address_s 
 * @param lhs left hand side
 * @param rhs right hand side
 * @return true_t if equal, false_t otherwise
 */
inline bool_t operator== ( 
    const core_mac_address_s& lhs,
    const core_mac_address_s& rhs )
    {
    if ( !core_tools_c::compare( lhs.addr, MAC_ADDR_LEN, 
           rhs.addr, MAC_ADDR_LEN ) )
        {
        return true_t;
        }

    return false_t;
    }

/**
 * operator!= for core_mac_address_s 
 * @param lhs left hand side
 * @param rhs right hand side
 * @return true_t if not equal, false_t otherwise
 */
inline bool_t operator!= ( 
    const core_mac_address_s& lhs,
    const core_mac_address_s& rhs )
    {
    return !( lhs == rhs );
    }

/**
 * operator== for core_ssid_s 
 * @param lhs left hand side
 * @param rhs right hand side
 * @return true_t if equal, false_t otherwise
 */
inline bool_t operator== ( 
    const core_ssid_s& lhs,
    const core_ssid_s& rhs )
    {
    if ( !core_tools_c::compare( lhs.ssid, lhs.length, 
           rhs.ssid, rhs.length ) )
        {
        return true_t;
        }

    return false_t;
    }

/**
 * operator!= for core_ssid_s 
 * @param lhs left hand side
 * @param rhs right hand side
 * @return true_t if not equal, false_t otherwise
 */
inline bool_t operator!= ( 
    const core_ssid_s& lhs,
    const core_ssid_s& rhs )
    {
    return !( lhs == rhs );
    }

/**
 * operator== for core_country_string_s
 * @param lhs left hand side
 * @param rhs right hand side
 * @return true_t equal, false_t otherwise
 */
inline bool_t operator== ( 
    const core_country_string_s& lhs,
    const core_country_string_s& rhs )
    {
    if ( !core_tools_c::compare( lhs.country, MAX_COUNTRY_STRING_LENGTH, 
           rhs.country, MAX_COUNTRY_STRING_LENGTH ) )
        {
        return true_t;
        }

    return false_t;
    }

/**
 * operator!= for core_country_string_s 
 * @param lhs left hand side
 * @param rhs right hand side
 * @return true_t if not equal, false_t otherwise
 */
inline bool_t operator!= ( 
    const core_country_string_s& lhs,
    const core_country_string_s& rhs )
    {
    return !( lhs == rhs );
    }

/**
 * operator== for medium_time_s 
 * @param lhs left hand side
 * @param rhs right hand side
 * @return true_t if not equal, false_t otherwise
 */
inline bool_t operator== (
    const medium_time_s& lhs,
    const medium_time_s& rhs )
    {
    if( !core_tools_c::compare(
        reinterpret_cast<const u8_t*>( lhs.up ), sizeof( lhs.up ),
        reinterpret_cast<const u8_t*>( rhs.up ), sizeof( rhs.up ) ) &&
        !core_tools_c::compare(
        reinterpret_cast<const u8_t*>( lhs.ac ), sizeof( lhs.ac ),
        reinterpret_cast<const u8_t*>( rhs.ac ), sizeof( rhs.ac ) ) )
        {
        return true_t;
        }

    return false_t;
    }

/**
 * operator!= for medium_time_s 
 * @param lhs left hand side
 * @param rhs right hand side
 * @return true_t if not equal, false_t otherwise
 */
inline bool_t operator!= ( 
    const medium_time_s& lhs,
    const medium_time_s& rhs )
    {
    return !( lhs == rhs );
    }
