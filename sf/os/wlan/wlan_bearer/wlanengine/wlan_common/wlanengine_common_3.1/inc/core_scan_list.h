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
* Description:  Class for storing beacons/probe responses.
*
*/


#ifndef CORE_SCAN_LIST_H
#define CORE_SCAN_LIST_H

#include "core_type_list.h"
#include "core_ap_data.h"
#include "core_scan_channels.h"
#include "core_scan_list_iterator.h"
#include "core_frame_beacon.h"

/**
 * This is a storage class for received beacons/probe responses.
 *
 * @since S60 v3.2
 */
NONSHARABLE_CLASS( core_scan_list_c )
    {

    friend class core_scan_list_iterator_by_tag_c;
    friend class core_scan_list_iterator_by_age_c;
    friend class core_scan_list_iterator_by_tag_and_ssid_c;

public:

    /**
     * Structure for storing beacon/probe response entries.
     */
    struct core_scan_list_entry_s
        {
        /** Data of the AP. */
        core_ap_data_c* ap_data;

        /** Timestamp of the entry. */
        u64_t timestamp;
        
        /** Tags of the entry. */
        u8_t tags;        
        };

    /**
     * Constructor.
     *
     * @since S60 v3.1
     * @param iap_data containing structure of IAP data
     */
    core_scan_list_c();

    /**
     * Destructor.
     */
    virtual ~core_scan_list_c();

    /**
     * Return the amount of entries in the list.
     *
     * @since S60 v3.1
     * @return The amount of entries in the list.
     */    
    u32_t count() const;

    /**
     * Append an entry to the list. 
     *
     * @since S60 v3.1
     * @param ap_data AP data to be added. If an entry already exists, it is replaced.
     * @return core_error_ok if success, an error code otherwise.
     * @note The timestamp is refreshed on all entries that match the BSSID of the AP.
     */
    core_error_e update_entry(
        core_ap_data_c& ap_data );

    /**
     * Remove all entries that have a matching BSSID.
     *
     * @since S60 v3.1
     * @param bssid BSSID to match against.
     */
    void remove_entries_by_bssid(
        const core_mac_address_s& bssid );

    /**
     * Remove all entries that are older than the given age.
     *
     * @since S60 v3.2
     * @param age Age to match against (in microseconds).
     */
    void remove_entries_by_age(
        u32_t age );

    /**
     * Set a tag on all the entries.
     *
     * @since S60 v3.1
     * @param tag The tag to set.
     */
    void set_tag(
        u8_t tag );

    /**
     * Clear a tag from all the entries.
     *
     * @since S60 v3.1
     * @param tag The tag to clear.
     */        
    void clear_tag(
        u8_t tag );

    /**
     * Clear all tags from the entries.
     *
     * @since S60 v3.1
     */                
    void clear_all_tags();

    /**
     * Print the contents of the list.
     */
    void print_contents();

    /**
     * Get a list of channels that have matching SSID.
     *
     * @since S60 v3.2
     * @param channels List of channels with matching SSID.
     * @param ssid SSID to match against.
     */
    void get_channels_by_ssid(
        core_scan_channels_c& channels,
        const core_ssid_s& ssid );

private: // data

    /**
     * List of beacons/probe responses.
     */
    core_type_list_c<core_scan_list_entry_s> scan_list_m;

    };

#endif // CORE_SCAN_LIST_H
