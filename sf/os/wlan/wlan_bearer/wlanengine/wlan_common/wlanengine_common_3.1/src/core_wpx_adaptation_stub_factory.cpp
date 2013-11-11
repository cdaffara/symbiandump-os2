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
* Description:  Static factory class for instantiating a stub version WPX adaptation.
*
*/


// ======== MEMBER FUNCTIONS ========

#include "core_wpx_adaptation_factory.h"
#include "core_wpx_adaptation_stub.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
abs_core_wpx_adaptation_c* core_wpx_adaptation_factory_c::instance(
    core_server_c* /* server */,
    abs_core_driverif_c* /* drivers */,
    abs_core_server_callback_c* /* adaptation */ )
    {
    DEBUG( "core_wpx_adaptation_stub_factory::instance() " );

    core_wpx_adaptation_stub_c* instance = new core_wpx_adaptation_stub_c();
    if ( !instance )
        {
        DEBUG( "core_wpx_adaptation_stub_factory::instance() - unable to core_wpx_adaptation_stub_c" );
        return NULL;
        }

    return instance;
    }
