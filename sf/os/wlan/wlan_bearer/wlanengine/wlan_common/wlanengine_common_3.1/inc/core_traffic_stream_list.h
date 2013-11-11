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
* Description:  Class for storing active traffic streams.
*
*/

/*
* %version: 9 %
*/

#ifndef CORE_TRAFFIC_STREAM_LIST_H
#define CORE_TRAFFIC_STREAM_LIST_H

#include "core_type_list.h"
#include "core_traffic_stream.h"

class core_traffic_stream_c;

/**
 * This is a storage class for active traffic streams.
 *
 * @since S60 v3.2
 */
NONSHARABLE_CLASS( core_traffic_stream_list_c )
    {

    friend class core_traffic_stream_list_iter_c;

public:

    /**
     * Structure for storing active traffic streams.
     */
    struct entry_s
        {
        /** Data of the AP. */
        core_traffic_stream_c* traffic_stream;
        };

    /**
     * Constructor.
     *
     * @since S60 v3.2
     * @param iap_data containing structure of IAP data
     */
    core_traffic_stream_list_c();

    /**
     * Destructor.
     */
    virtual ~core_traffic_stream_list_c();

    /**
     * Return the amount of entries in the list.
     *
     * @since S60 v3.2
     * @return The amount of entries in the list.
     */    
    u32_t count() const;

    /**
     * Return the first entry in the list.
     * @return The first entry in the list. NULL if none.
     */
    core_traffic_stream_c* first();

    /**
     * Return the next entry in the list.
     * @return The next entry in the list. NULL if none.
     */
    core_traffic_stream_c* next();

    /**
     * Return the current entry in the list.
     * @return The current entry in the list. NULL if none.
     */
    core_traffic_stream_c* current() const;

    /**
     * Append an entry to the list. 
     *
     * @since S60 v3.2
     * @param traffic_stream Traffic stream to be added. If an entry already exists, it is replaced.
     * @return core_error_ok if success, an error code otherwise.
     */
    core_error_e update_traffic_stream(
        const core_traffic_stream_c& traffic_stream );

    /**
     * Remove entry with a matching TID.
     *
     * @since S60 v3.2
     * @param tid TID to match against.
     */
    void remove_traffic_stream_by_tid(
        u8_t tid );

    /**
     * Print the contents of the list.
     */
    void print_contents();

    /**
     * Calculate the combined admitted medium time.
     * 
     * @since S60 v3.2
     * @return The combined admitted medium time. MEDIUM_TIME_NOT_DEFINED if none.
     */
    medium_time_s admitted_medium_time();

    /**
     * Check whether the given access class has any traffic streams.
     * 
     * @since S60 v3.2
     * @param access_class Access class to search for.
     * @return true_t if any traffic stream exist, false_t otherwise.
     */
    bool_t is_traffic_stream_for_access_class(
        core_access_class_e access_class );

    /**
     * Check whether a traffic stream exists for the given TID.
     * 
     * @since S60 v3.2
     * @param tid TID to search for.
     * @return true_t if a traffic stream exists, false_t otherwise.
     */
    bool_t is_traffic_stream_for_tid(
        u8_t tid );

    /**
     * Set the given status to all traffic streams.
     *
     * @since S60 v3.2
     * @param status Status to be set.
     */
    void set_traffic_stream_status(
        core_traffic_stream_status_e status );

    /**
     * Get the next free TID. 
     *
     * @return The next free TID, MAX_TRAFFIC_STREAM_TID if no available.
     */
    u8_t next_tid();
    
private: // data

    /**
     * List of beacons/probe responses.
     */
    core_type_list_c<entry_s> ts_list_m;

    };

#endif // CORE_TRAFFIC_STREAM_LIST_H
