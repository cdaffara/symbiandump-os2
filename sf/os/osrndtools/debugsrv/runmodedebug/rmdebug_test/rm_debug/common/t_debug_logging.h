// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the License "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Logging macros for use in debug subsystem
// 
//

#ifndef RMDEBUG_LOGGING_H
#define RMDEBUG_LOGGING_H

/* Debug messages
 * 
 * Debug messages are only generated for debug builds.
 * 
 * As user mode use RDebug::Printf(). 
 * 
 */

// Uncomment if logging of required
//#define RMDEBUG_LOGGING 

#ifdef RMDEBUG_LOGGING

   #include <e32debug.h>

    #define LOG_MSG(args...)            RDebug::Printf(args)
    #define LOG_DES(args...)            RDebug::Print(args) // For wide descriptors
    #define LOG_ENTRY()                 RDebug::Printf("+%s", __PRETTY_FUNCTION__)
    #define LOG_EXIT()                  RDebug::Printf("-%s", __PRETTY_FUNCTION__)
    #define LOG_ARGS(fmt, args...)      RDebug::Printf("+%s " fmt, __PRETTY_FUNCTION__, args)
    #define LOG_RETURN(x)               RDebug::Printf("Returning %d from [%s]", x, __PRETTY_FUNCTION__)

    // Kept for compatibility
    #define LOG_MSG2( a, b )            RDebug::Printf( a, b )
    #define LOG_MSG3( a, b, c )         RDebug::Printf( a, b, c )
    #define LOG_MSG4( a, b, c, d )      RDebug::Printf( a, b, c, d )
    #define LOG_MSG5( a, b, c, d, e )   RDebug::Printf( a, b, c, d, e )

#else

   #include <e32debug.h>

    #define LOG_MSG(args...)            
    #define LOG_DES(args...)           
    #define LOG_ENTRY()                 
    #define LOG_EXIT()                 
    #define LOG_ARGS(fmt, args...)      
    #define LOG_RETURN(x)               

    // Kept for compatibility
    #define LOG_MSG2( a, b )            
    #define LOG_MSG3( a, b, c )         
    #define LOG_MSG4( a, b, c, d )     
    #define LOG_MSG5( a, b, c, d, e )  

#endif 

#endif //RMDEBUG_LOGGING
