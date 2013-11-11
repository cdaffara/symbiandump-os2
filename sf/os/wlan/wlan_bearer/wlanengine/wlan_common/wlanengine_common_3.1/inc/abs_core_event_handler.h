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
* Description:  Interface for operations registered as event handlers
*
*/


#ifndef ABS_CORE_EVENT_HANDLER_H
#define ABS_CORE_EVENT_HANDLER_H

#include "am_platform_libraries.h"
#include "core_types.h"

/**
 * Abstract interface for operations wanting to handle events themselves.
 *
 * @since S60 v3.1
 */
class abs_core_event_handler_c
    {
public:

    /**
     * Destructor.
     */
    virtual ~abs_core_event_handler_c() {};

    /**
     * Called by the core server when an indication has been received.
     *
     * @since S60 v3.1
     * @param indication Adaptation layer event.
     * @return true_t if the indication was handled, false_t if not handled.
     */
    virtual bool_t notify(
        core_am_indication_e indication ) = 0;

    };

#endif // ABS_CORE_EVENT_HANDLER_H
