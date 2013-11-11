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
* Description:  Debugging support
*
*/


/*
Idea:

    Logging tool for developers, uniform style to write debug log to RDebug or to a File

Usage:

    Configuring:
     - add to .mmp file
        // define logging on and logging into file, file must be defined in code
        // the macros can also be in a header file
        // and inside ifdef if printing wanted in debug builds
        MACRO ENABLE_DEBUG_PRINT
        MACRO DEBUG_OUTPUT_TO_FILE
        // this library is needed for the file logging, remove if/when not using
        LIBRARY flogger.lib

        Alternatively you may enable debug print in a header with:
        #ifdef _DEBUG // comment this if you want logging in release build
        #define ENABLE_DEBUG_PRINT
        #define DEBUG_OUTPUT_TO_FILE
        #endif // _DEBUG

     - make a directory: Epoc32\Wins\c\logs\hti\

    Enabling:
     - in some source header
        // log file name if using file logging
        #define DEBUG_FILENAME "Example.log"
        // include the printing macros
        #include "DebugPrint.h"

    Printing:

        // normal string
        DEBUG_PRINT( DEBUG_STRING( "Debug Print" ) );
        // string with variables
        DEBUG_PRINT( DEBUG_LINE( "Debug Print %d" ), 100 );
        TInt index( 100 );
        DEBUG_PRINT( DEBUG_STRING( "Debug Print %d" ), index );

        // DEBUG_LINE automatically adds filename and linenumber to output
        _LIT( KDebugPrintExample, "Example" );
        DEBUG_PRINT( DEBUG_LINE( "Debug Print %S" ), &KDebugPrintExample );

Known issues:

    - If you use macros from .mmp file remember to abld makefile <target> to change flags from project
    - Using lots of DEBUG_LINE macros slows down compiling due to excessive preprocessing

Ideas & further development:

    - make rdebug and file logging work simultaneously, currently not possible
    - enable optional run-time configuring of debug info writing

*/

#ifndef __DEBUGPRINT_H__
#define __DEBUGPRINT_H__

// Debug logging is enabled, you may enable debug printing in release builds also
#ifdef ENABLE_DEBUG_PRINT

    // warn if this is a release build!!
    #ifndef _DEBUG
        #if defined(__VC32__)
            #pragma message( "Warning: debug printing is on in release build!" )
        #else // __GCC32__
            #warning "debug printing is on in release build!"
        #endif // __VC32__
    #endif

// no include files if no debug printing --> faster compile time
// INCLUDES
#include <e32std.h>
#include <e32svr.h>
#include <flogger.h>

    /**
     * Depending if the build is UNICODE or not, define the
     * helper macros that display the source code filename and
     * linenumber as a part of the debug message.
     */
    #ifdef _UNICODE
        #define STRINGIZE(l) L#l
        #define TO_NUMBER(l) STRINGIZE(l)
        #define TOKEN_PASTING(s) L##s
        #define TO_UNICODE(s) TOKEN_PASTING(s)
        #define DEBUG_STR(m) TO_UNICODE(__FILE__) L"(" TO_NUMBER(__LINE__) L") : " L##m
    #else
        #define STRINGIZE(l) #l
        #define TO_NUMBER(l) STRINGIZE(l)
        #define DEBUG_STR __FILE__ "(" TO_NUMBER(__LINE__) ") : "
    #endif // _UNICODE

    /**
     * Helper macro for defining debug strings with just the debug text.
     */
    #define DEBUG_STRING(s) _L(s)

    /**
     * Helper macro for defining debug string with filename and linenumber.
     */
    #define DEBUG_LINE(s) TPtrC((const TText *) DEBUG_STR(s))

    #ifdef DEBUG_OUTPUT_TO_FILE

        /**
         * Method to handle file writing
         */
        inline void DebugWriteFormat(TRefByValue<const TDesC> aFmt,...)
            {
            _LIT(KDir, "hti");
            #ifdef DEBUG_FILENAME
                const static TLitC<sizeof(DEBUG_FILENAME)> KName={sizeof(DEBUG_FILENAME)-1, TO_UNICODE( DEBUG_FILENAME ) };
            #else
                _LIT(KName, "DebugOutput.log" );
            #endif // DEBUG_FILENAME

            // take the ellipsis parameters
            VA_LIST args;
            VA_START(args,aFmt);
            RFileLogger::WriteFormat( KDir, KName, EFileLoggingModeAppend, aFmt, args );
            VA_END(args);
            }

        /**
         * Output to log file
         */
        #define DEBUG_PRINT DebugWriteFormat

    #else
        /**
         * Output to debugger output
         */
        #define DEBUG_PRINT RDebug::Print
    #endif

#else   // ENABLE_DEBUG_PRINT

    /**
     * Dummy struct for checking that all DEBUG_PRINT's define string
     * literals using space-saving DEBUG_STRING.
     */
    struct TEmptyDebugString { };

    /**
     * Macro for defining debug-only literal strings (empty release version)
     */
    #define DEBUG_STRING(s) TEmptyDebugString()

    /**
     * Macro for defining debug-only literal strings (empty release version)
     */
    #define DEBUG_LINE(s) TEmptyDebugString()

    /// Empty debug print function for release builds.
    inline void DEBUG_PRINT(TEmptyDebugString)
        {
        }

    /// Empty debug print function for release builds.
    template<class T1>
    inline void DEBUG_PRINT(TEmptyDebugString,T1)
        {
        }

    /// Empty debug print function for release builds.
    template<class T1,class T2>
    inline void DEBUG_PRINT(TEmptyDebugString,T1,T2)
        {
        }

    /// Empty debug print function for release builds.
    template<class T1,class T2,class T3>
    inline void DEBUG_PRINT(TEmptyDebugString,T1,T2,T3)
        {
        }

    /// Empty debug print function for release builds.
    template<class T1,class T2,class T3,class T4>
    inline void DEBUG_PRINT(TEmptyDebugString,T1,T2,T3,T4)
        {
        }

    /// Empty debug print function for release builds.
    template<class T1,class T2,class T3,class T4,class T5>
    inline void DEBUG_PRINT(TEmptyDebugString,T1,T2,T3,T4,T5)
        {
        }

    /// Empty debug print function for release builds.
    template<class T1,class T2,class T3,class T4,class T5,class T6>
    inline void DEBUG_PRINT(TEmptyDebugString,T1,T2,T3,T4,T5,T6)
        {
        }

    /// Empty debug print function for release builds.
    template<class T1,class T2,class T3,class T4,class T5,class T6,class T7>
    inline void DEBUG_PRINT(TEmptyDebugString,T1,T2,T3,T4,T5,T6,T7)
        {
        }

    /// Empty debug print function for release builds.
    template<class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8>
    inline void DEBUG_PRINT(TEmptyDebugString,T1,T2,T3,T4,T5,T6,T7,T8)
        {
        }

#endif  // ENABLE_DEBUG_PRINT

#endif //  __DEBUGPRINT_H__
