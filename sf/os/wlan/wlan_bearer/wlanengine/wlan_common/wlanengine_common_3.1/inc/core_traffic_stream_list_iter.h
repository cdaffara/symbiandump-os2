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
* Description:  Iterator for traffic stream list.
*
*/

/*
* %version: %
*/

#ifndef CORE_TRAFFIC_STREAM_LIST_ITER_H
#define CORE_TRAFFIC_STREAM_LIST_ITER_H

#include "core_traffic_stream_list.h"

/**
 * This class implements an iterator for traffic stream list.
 *
 * @since S60 v3.2
 */
NONSHARABLE_CLASS( core_traffic_stream_list_iter_c )
    {

public:

    /**
     * Constructor.
     *
     * @param list Reference to the list being iterated.
     */
    core_traffic_stream_list_iter_c(
        core_traffic_stream_list_c& list );

    /**
     * Destructor.
     */
    virtual ~core_traffic_stream_list_iter_c();

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
     * Remove the current entry from the list.
     * 
     * @return core_error_ok if removed successfully, an error code otherwise.
     */
    core_error_e remove();    

private: // data

    /**
     * An iterator for iterating the list.
     */
    core_type_list_iterator_c<core_traffic_stream_list_c::entry_s> iter_m;

    };

#endif // CORE_TRAFFIC_STREAM_LIST_ITER_H
