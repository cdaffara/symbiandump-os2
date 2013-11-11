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
* Description: Logging definitions.
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
const TUint KThreadOperation = 0x4;
const TUint KRunTestCase     = 0x8;
const TUint KPrint           = 0x10;
const TUint KMessage         = 0x20;
const TUint KVerbose         = 0x40;
const TUint KAlways          = 0xFFFFFFFF;
const TUint KDebugLevel      = ( KMessage | KInit | KError );

// MACROS
#define __TRACING_ENABLED

// Define USE_RDEBUG to use RDebug::Print for logging
//#define USE_RDEBUG

// Define USE_LOGGER to use logger for logging
#define USE_LOGGER

#ifdef __TRACING_ENABLED

    // TRACEI macro is used in test execution thread to allow the test use Tls for its own purposes
    // Note that handles to server (i.e to fileserver) are thread specific.

    // Disable conditional expression is constant -warning
    #pragma warning( disable : 4127 )

    // Using logger and not RDebug
    #if defined (USE_LOGGER) && !defined (USE_RDEBUG)
    #define __TRACEI(level,p) if ( (level) & KDebugLevel) { if(THREADLOGGER)THREADLOGGER->Log p; }
    #endif

    // Using RDebug and not Logger
    #if defined (USE_RDEBUG) && !defined (USE_LOGGER)
    #define __TRACEI(level,p) if ( (level) & KDebugLevel) { RDebug::Print p; }
    #endif

    // Using Logger and RDebug
    #if defined (USE_RDEBUG) && defined (USE_LOGGER)

    #define __TRACEI(level,p) if ( (level) & KDebugLevel) {\
        if(THREADLOGGER)THREADLOGGER->Log p;\
        RDebug::Print p;\
    }

    #endif

    // Not using RDebug and not using logger
    #if !defined(USE_RDEBUG) && !defined (USE_LOGGER)
    #define __TRACEI(level,p);
    #endif
    
    #define __RDEBUG(x) if( KVerbose & KDebugLevel){ RDebug::Print x ; }

#else
    // No tracing
    #define __TRACEI(level,p);

    #define  __RDEBUG(x)
#endif


// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

#endif // LOGGINGDEFS_H

// End of File
