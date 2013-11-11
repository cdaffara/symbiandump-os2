/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
* 
* Description: This file contains logging macros for test engine.
*
*/

#ifndef LOGGINGDEFS_H
#define LOGGINGDEFS_H

//  INCLUDES
#include <e32std.h>
#include <e32base.h>

// CONSTANTS
const TUint KError           = 0x1;
const TUint KInit            = 0x2;
const TUint KVerbose         = 0x4;
const TUint KAlways          = 0xFFFFFFFF;
const TUint KDebugLevel      = ( KInit | KError );

// MACROS
#define __TRACING_ENABLED

// Define USE_LOGGER to use logger for logging
#define USE_LOGGER

#ifdef __TRACING_ENABLED

    // Disable conditional expression is constant -warning
    #pragma warning( disable : 4127 )

    // Using logger
    #if defined (USE_LOGGER)
    #define __TRACE(level,p) if ( (level) & KDebugLevel) { LOGGER->Log p; }
    #else
    #define __TRACE(level,p);
    #endif

    #define __RDEBUG(x) if( KVerbose & KDebugLevel){ RDebug::Print x ; }

#else
    // No tracing
    #define __TRACE(level,p);
    
    #define __RDEBUG(x)

#endif

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

#endif // LOGGINGDEFS_H

// End of File
