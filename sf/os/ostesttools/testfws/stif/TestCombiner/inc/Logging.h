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
* Description: This file contains Logging macros for test combiner
*
*/

#ifndef LOGGINGDEFS_H
#define LOGGINGDEFS_H

//  INCLUDES
#include <e32std.h>
#include <e32base.h>


// CONSTANTS
const TUint KError              = 0x1;
const TUint KInit               = 0x2;
const TUint KPrint              = 0x4;
const TUint KMessage            = 0x10;
const TUint KFunction           = 0x20;
const TUint KVerbose            = 0x40;
const TUint KAlways             = 0xFFFFFFFF;
const TUint KDebugLevel         = ( KPrint | KError | KMessage );

// MACROS
#define __TRACING_ENABLED

#ifdef __TRACING_ENABLED

     // Disable conditional expression is constant -warning
    #pragma warning( disable : 4127 )

    // General tracing function
    #define __TRACE(level,p) if ( ( (level) & KDebugLevel ) && LOGGER )\
                                          {LOGGER->Log p;}
              
    // Direct RDebug::Print                            
    #define __RDEBUG(p) if( KVerbose & KDebugLevel ){ RDebug::Print p ; } 


     // Log function name
    #define __TRACEFUNC() if( ( KFunction & KDebugLevel ) && LOGGER ){\
                                const char* f = __FUNCTION__;\
                                 TPtrC8 F((const unsigned char*)f);\
                                 LOGGER->Log(F);}

#else // __TRACING_ENABLED
     // No tracing
    #define __TRACE(level,p)
    #define __RDEBUG(p)
    #define __TRACEFUNC()
#endif // __TRACING_ENABLED


// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

#endif // LOGGINGDEFS_H

// End of File
