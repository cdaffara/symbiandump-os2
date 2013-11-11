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
* Description:  Base class for scan list iterators
*
*/


#ifndef ABS_CORE_SCAN_LIST_ITERATOR_H
#define ABS_CORE_SCAN_LIST_ITERATOR_H

#include "core_types.h"

class core_ap_data_c;

/**
 * This class acts as the abstract base class for the iterators. 
 */
NONSHARABLE_CLASS( abs_core_scan_list_iterator_c )
    {

public:

    /**
     * Destructor.
     */
    virtual ~abs_core_scan_list_iterator_c() {};

    /**
     * Return the first entry in the list that matches the iterator type.
     *
     * @since S60 v3.1
     * @return NULL if the list empty, pointer to the first entry otherwise.
     * @note This method will reset the internal iterator.
     */
    virtual core_ap_data_c* first() = 0;

    /**
     * Return the next entry in the list.
     *
     * @since S60 v3.1
     * @return NULL if the current entry is the last one,
     *         pointer to the next entry otherwise.
     */
    virtual core_ap_data_c* next() = 0;

    /**
     * Return the current entry in the list.
     *
     * @since S60 v3.1
     * @return pointer to the current entry.
     */
    virtual core_ap_data_c* current() = 0;

    };

#endif // ABS_CORE_SCAN_LIST_ITERATOR_H
