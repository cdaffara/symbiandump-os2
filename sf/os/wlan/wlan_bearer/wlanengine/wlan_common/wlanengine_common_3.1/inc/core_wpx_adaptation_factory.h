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
* Description:  Factory class for WPX adaptation implementation.
*
*/


#ifndef CORE_WPX_ADAPTATION_FACTORY_H
#define CORE_WPX_ADAPTATION_FACTORY_H

#include "am_platform_libraries.h"

class abs_core_wpx_adaptation_c;
class core_server_c;
class abs_core_driverif_c;
class abs_core_server_callback_c;

/**
* This class implements a factory that is used to instantiate
* WPX adaptation implementation.
* @see abs_core_wpx_adaptation_c
*/
NONSHARABLE_CLASS( core_wpx_adaptation_factory_c )
    {
public:

    /**
     * Create a WPX adaptation instance.
     * @param server Callback to core server.
     * @param drivers Callback to lower adaptation.
     * @param adaptation Callback to upper adaptation.
     * @return Pointer to the created timer.
     */
    static abs_core_wpx_adaptation_c* instance(
        core_server_c* server,
        abs_core_driverif_c* drivers,
        abs_core_server_callback_c* adaptation );

private: // private constructor

    /** 
     * Constructor.
     */
    core_wpx_adaptation_factory_c();

    };    

#endif // CORE_WPX_ADAPTATION_FACTORY_H
