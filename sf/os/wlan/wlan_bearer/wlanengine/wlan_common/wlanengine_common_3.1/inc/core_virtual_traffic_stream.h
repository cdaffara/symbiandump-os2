/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class for storing the parameters of a virtual traffic stream.
*
*/

/*
* %version: 1 %
*/

#ifndef CORE_VIRTUAL_TRAFFIC_STREAM_H
#define CORE_VIRTUAL_TRAFFIC_STREAM_H

#include "core_types.h"

/**
 * Class for storing the parameters of a virtual traffic stream.
 *
 * @since S60 v5.1
 */
NONSHARABLE_CLASS( core_virtual_traffic_stream_c )
    {

public:

    /**
     * Constructor.
     *
     * @param requested_tid TID the virtual traffic stream was requested with.
     * @param tid Current TID of the virtual traffic stream.
     * @param user_priority User Priority of the virtual traffic stream.
     * @param is_automatic_stream Whether the virtual traffic stream
     *                            has been created automatically.
     * @param params Parameters of the virtual traffic stream.
     * @param id ID of the virtual traffic stream.
     * @param stream_status Status of the virtual traffic stream.
     */
    core_virtual_traffic_stream_c(
        u8_t requested_tid,
        u8_t tid,
        u8_t user_priority,
        bool_t is_automatic_stream,
        const core_traffic_stream_params_s& params,
        u32_t stream_id,
        core_traffic_stream_status_e stream_status );

    /**
     * Destructor.
     */
    virtual ~core_virtual_traffic_stream_c();

    /**
     * Get the ID of the virtual traffic stream.
     */
    u32_t id() const;

    /**
     * Get the TID value the virtual traffic stream was requested with.
     */
    u8_t requested_tid() const;

    /**
     * Get the current TID value of the virtual traffic stream.
     *
     * @since S60 v5.1
     * @return The TID value of the virtual traffic stream.
     */
    u8_t tid() const;

    /**
     * Set the current TID value of the virtual traffic stream.
     *
     * @since S60 v5.1
     * @param tid TID value of the virtual traffic stream.
     */    
    void set_tid(
        u8_t tid );

    /**
     * Get the User Priority value of the virtual traffic stream.
     *
     * @since S60 v5.1
     * @return The User Priority value of the virtual traffic stream.
     */
    u8_t user_priority() const;

    /**
     * Get the access class of the traffic stream.
     * 
     * @since S60 v5.1
     * @return The access class of the traffic stream.
     */
    core_access_class_e access_class() const;    

    /**
     * Whether the virtual traffic stream has been created automatically.
     * 
     * @since S60 v5.1
     * @return true_t if the virtual traffic stream has been created automatically,
     *         false_t otherwise.
     */
    bool_t is_automatic_stream() const;

    /**
     * Get the TSPEC parameters of the virtual traffic stream.
     * 
     * @since S60 v5.1 
     * @return The TSPEC parameters of the virtual traffic stream.
     */
    const core_traffic_stream_params_s& params() const;

    /**
     * Get the current status of the virtual traffic stream.
     *
     * @since S60 v3.2
     * @return The current status of the virtual traffic stream.
     */
    core_traffic_stream_status_e status() const;

    /**
     * Set the current status of the virtual traffic stream.
     *
     * @since S60 v3.2
     * @param status The current status of the virtual traffic stream.
     */
    void set_status(    
        core_traffic_stream_status_e status );

private: // data

    /**
     * The TID the virtual traffic stream was requested with.
     */
    u8_t requested_tid_m;
    
    /**
     * The TID of the virtual traffic stream.
     */
    u8_t tid_m;
    
    /**
     * The User Priority of the virtual traffic stream.
     */    
    u8_t user_priority_m;

    /**
     * Whether the virtual traffic stream has been created automatically.
     */
    bool_t is_automatic_stream_m;

    /**
     * Parameters of the virtual traffic stream.
     */
    core_traffic_stream_params_s params_m;

    /**
     * ID of the virtual traffic stream.
     */
    u32_t id_m;
    
    /**
     * The current status of the virtual traffic stream.
     */
    core_traffic_stream_status_e status_m;

    };

#endif // CORE_VIRTUAL_TRAFFIC_STREAM_H
