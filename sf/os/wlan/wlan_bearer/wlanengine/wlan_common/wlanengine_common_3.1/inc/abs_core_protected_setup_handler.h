/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Interface for operations handling protected setup events.
*
*/


#ifndef ABS_CORE_PROTECTED_SETUP_HANDLER_H
#define ABS_CORE_PROTECTED_SETUP_HANDLER_H

#include "am_platform_libraries.h"
#include "core_types.h"

/**
 * Abstract interface for operations handling protected setup events.
 *
 * @since S60 v3.2
 */
class abs_core_protected_setup_handler_c
    {
public:

    /**
     * Destructor.
     */
    virtual ~abs_core_protected_setup_handler_c() {};

    /**
     * Called by the core server when a network has been configured via
     * Protected Setup.
     *
     * @since S60 v3.2
     * @param iap_data Results of a successful Protected Setup operation. 
     */
    virtual void handle_protected_setup_network(
        const core_iap_data_s& iap_data ) = 0;

    };

#endif // ABS_CORE_PROTECTED_SETUP_HANDLER_H
