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
* Description:  Definitions for small utilities implemented by the adaptation
*
*/


#ifndef CORE_AM_TOOLS_H
#define CORE_AM_TOOLS_H

#include "core_types.h"

/**
 * Class for small platform-dependant helper methods.
 *
 * @since S60 v3.1
 */
NONSHARABLE_CLASS( core_am_tools_c )
    {

public:

    /**
     * Return the current moment of time.
     *
     * @since S60 v3.1
     * @return The moment of time.
     */
    static u64_t timestamp();
    
    /**
     * Return random number.
     *
     * @since S60 v5.2
     * @return Random number.
     */
    static u32_t random();

    };

#endif // CORE_AM_TOOLS_H
