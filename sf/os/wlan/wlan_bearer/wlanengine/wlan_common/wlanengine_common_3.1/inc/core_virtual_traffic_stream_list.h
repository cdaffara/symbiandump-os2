/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0""
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Class for storing virtual traffic streams.
*
*/

/*
* %version: 1 %
*/

#ifndef CORE_VIRTUAL_TRAFFIC_STREAM_LIST_H
#define CORE_VIRTUAL_TRAFFIC_STREAM_LIST_H

#include "core_type_list.h"
#include "core_virtual_traffic_stream.h"

/**
 * This is a storage class for virtual traffic streams.
 *
 * @since S60 v3.2
 */
NONSHARABLE_CLASS( core_virtual_traffic_stream_list_c )
    {

    friend class core_virtual_traffic_stream_list_iter_c;

public:

    /**
     * Structure for storing virtual traffic streams.
     */
    struct entry_s
        {
        /** Data of the AP. */
        core_virtual_traffic_stream_c* traffic_stream;
        };

    /**
     * Constructor.
     */
    core_virtual_traffic_stream_list_c();

    /**
     * Destructor.
     */
    virtual ~core_virtual_traffic_stream_list_c();

    /**
     * Return the amount of entries in the list.
     *
     * @since S60 v5.1
     * @return The amount of entries in the list.
     */
    u32_t count() const;

    /**
     * Add a copy of the given entry to the list.
     * 
     * @since S60 v5.1
     * @param stream Entry to copy.
     * @return core_error_ok if successful, an error code otherwise.  
     */
    core_error_e add_traffic_stream(
        const core_virtual_traffic_stream_c& stream );

    /**
     * Add an entry to the list. 
     *
     * @since S60 v5.1
     * @param requested_tid TID the virtual traffic stream was requested with.
     * @param tid Current TID of the virtual traffic stream.
     * @param user_priority User Priority of the virtual traffic stream.
     * @param is_automatic_stream Whether the virtual traffic stream
     *                            has been created automatically.
     * @param params Parameters of the virtual traffic stream.
     * @param id Contains the ID assigned to the stream if successfully added.
     * @param stream_status Status of the virtual traffic stream.
     * @return core_error_ok if successful, an error code otherwise.
     */
    core_error_e add_traffic_stream(
        u8_t requested_tid,
        u8_t tid,
        u8_t user_priority,
        bool_t is_automatic_stream,
        const core_traffic_stream_params_s& params,
        u32_t& stream_id,
        core_traffic_stream_status_e stream_status );

    /**
     * Remove entry with a matching TID.
     *
     * @since S60 v5.1
     * @param tid TID to match against.
     */
    void remove_traffic_stream_by_tid(
        u8_t tid );

    /**
     * Remove entry with a matching stream ID.
     *
     * @since S60 v5.1
     * @param stream_id ID to match against.
     */
    void remove_traffic_stream_by_id(
        u32_t stream_id );

    /**
     * Return the traffic mode of the given AC.
     * 
     * @since S60 v5.1
     * @return The traffic mode of the given AC.
     */
    core_access_class_traffic_mode_e traffic_mode_by_ac(
        core_access_class_e access_class );

    /**
     * Print the contents of the list.
     */
    void print_contents();

    /**
     * Get the next free TID. 
     *
     * @return The next free TID, MAX_TRAFFIC_STREAM_TID if no available.
     */
    u8_t next_tid();    

private: // data

    /**
     * List of virtual traffic streams.
     */
    core_type_list_c<entry_s> ts_list_m;

    /**
     * Contains the next stream ID to be used.
     */
    u32_t next_stream_id_m;

    };

#endif // CORE_VIRTUAL_TRAFFIC_STREAM_LIST_H
