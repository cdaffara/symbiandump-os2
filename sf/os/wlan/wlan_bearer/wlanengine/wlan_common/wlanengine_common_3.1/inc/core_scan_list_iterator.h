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
* Description:  Class for different iterators for the scan list
*
*/


#ifndef CORE_SCAN_LIST_ITERATOR_H
#define CORE_SCAN_LIST_ITERATOR_H

#include "abs_core_scan_list_iterator.h"
#include "core_ap_data.h"

class core_scan_list_c;

/**
 * This class iterates the scan list by not tagged items.
 */
NONSHARABLE_CLASS( core_scan_list_iterator_by_tag_c ) : public abs_core_scan_list_iterator_c
    {

public:

    /**
     * Constructor
     *
     * @since S60 v3.1
     * @param scan_list Reference to the scan list instance.
     * @param tag The suitable entiries must not contain this tag.
     */
    core_scan_list_iterator_by_tag_c(
        core_scan_list_c& scan_list,
        u8_t tag );

    /**
     * Destructor.
     */
    virtual ~core_scan_list_iterator_by_tag_c();

    /**
     * Return the first entry in the list that matches the iterator type.
     *
     * @since S60 v3.1
     * @return NULL if the list empty, pointer to the first entry otherwise.
     * @note This method will reset the internal iterator.
     */
    virtual core_ap_data_c* first();

    /**
     * Return the next entry in the list.
     *
     * @since S60 v3.1
     * @return NULL if the current entry is the last one,
     *         pointer to the next entry otherwise.
     */
    virtual core_ap_data_c* next();

    /**
     * Return the current entry in the list.
     *
     * @since S60 v3.1
     * @return pointer to the current entry.
     */
    virtual core_ap_data_c* current();

private: // data

    /**
     * Handle to the scan list instance.
     */
    core_scan_list_c& scan_list_m;

    /**
     * The suitable entries must not contain this tag.
     */
    u8_t tag_m;

    };

/**
 * This class iterates the scan list by age.
 */
NONSHARABLE_CLASS( core_scan_list_iterator_by_age_c ) : public abs_core_scan_list_iterator_c
    {

public:

    /**
     * Constructor
     *
     * @since S60 v3.1
     * @param scan_list Reference to the scan list instance.
     * @param maximum_age The maximum age of suitable entries in seconds.
     */
    core_scan_list_iterator_by_age_c(
        core_scan_list_c& scan_list,
        u32_t maximum_age );

    /**
     * Destructor.
     */
    virtual ~core_scan_list_iterator_by_age_c();

    /**
     * Return the first entry in the list that matches the iterator type.
     *
     * @since S60 v3.1
     * @return NULL if the list empty, pointer to the first entry otherwise.
     * @note This method will reset the internal iterator.
     */
    virtual core_ap_data_c* first();

    /**
     * Return the next entry in the list.
     *
     * @since S60 v3.1
     * @return NULL if the current entry is the last one,
     *         pointer to the next entry otherwise.
     */
    virtual core_ap_data_c* next();

    /**
     * Return the current entry in the list.
     *
     * @since S60 v3.1
     * @return pointer to the current entry.
     */
    virtual core_ap_data_c* current();

private: // data

    /**
     * Handle to the scan list instance.
     */
    core_scan_list_c& scan_list_m;

    /**
     * The maximum age of suitable entries in seconds.
     */
    u32_t maximum_age_m;

    };

/**
 * This class iterates the scan list by not tagged items and matching SSID.
 */
NONSHARABLE_CLASS( core_scan_list_iterator_by_tag_and_ssid_c ) : public core_scan_list_iterator_by_tag_c
    {

public:

    /**
     * Constructor
     *
     * @since S60 v3.1
     * @param scan_list Reference to the scan list instance.
     * @param tag The suitable entiries must not contain this tag.
     * @param ssid SSID to match against entries.
     */
    core_scan_list_iterator_by_tag_and_ssid_c(
        core_scan_list_c& scan_list,
        u8_t tag,
        const core_ssid_s& ssid );

    /**
     * Destructor.
     */
    virtual ~core_scan_list_iterator_by_tag_and_ssid_c();

    /**
     * Return the first entry in the list that matches the iterator type.
     *
     * @since S60 v3.1
     * @return NULL if the list empty, pointer to the first entry otherwise.
     * @note This method will reset the internal iterator.
     */
    virtual core_ap_data_c* first();

    /**
     * Return the next entry in the list.
     *
     * @since S60 v3.1
     * @return NULL if the current entry is the last one,
     *         pointer to the next entry otherwise.
     */
    virtual core_ap_data_c* next();

private: // data

    /**
     * SSID to match against entries.
     */
    core_ssid_s ssid_m;

    };

/**
 * This class iterates the scan list by not tagged items and matching SSID
 * and BSSID.
 */
NONSHARABLE_CLASS( core_scan_list_iterator_by_tag_and_ssid_and_bssid_c ) : public core_scan_list_iterator_by_tag_and_ssid_c
    {

public:

    /**
     * Constructor
     *
     * @since S60 v3.1
     * @param scan_list Reference to the scan list instance.
     * @param tag The suitable entiries must not contain this tag.
     * @param ssid SSID to match against entries.
     * @param bssid BSSID to match against entries.
     */
    core_scan_list_iterator_by_tag_and_ssid_and_bssid_c(
        core_scan_list_c& scan_list,
        u8_t tag,
        const core_ssid_s& ssid,
        const core_mac_address_s& bssid );

    /**
     * Destructor.
     */
    virtual ~core_scan_list_iterator_by_tag_and_ssid_and_bssid_c();

    /**
     * Return the first entry in the list that matches the iterator type.
     *
     * @since S60 v3.1
     * @return NULL if the list empty, pointer to the first entry otherwise.
     * @note This method will reset the internal iterator.
     */
    virtual core_ap_data_c* first();

    /**
     * Return the next entry in the list.
     *
     * @since S60 v3.1
     * @return NULL if the current entry is the last one,
     *         pointer to the next entry otherwise.
     */
    virtual core_ap_data_c* next();

private: // data

    /**
     * BSSID to match against entries.
     */
    core_mac_address_s bssid_m;

    };

#endif // CORE_SCAN_LIST_ITERATOR_H
