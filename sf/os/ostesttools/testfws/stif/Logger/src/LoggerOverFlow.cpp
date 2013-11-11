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
* Description: This module contains implementation of 
* TDesLoggerOverflowHandler class member functions.
*
*/

// INCLUDE FILES
#include <e32std.h>
#include "LoggerOverFlow.h"

// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES
// None

// CONSTANTS
// None

// MACROS
// None

// LOCAL CONSTANTS AND MACROS
// None

// MODULE DATA STRUCTURES
// None

// LOCAL FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// ==================== LOCAL FUNCTIONS =======================================
// None

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

    Class: TDesLoggerOverflowHandler

    Method: TDesLoggerOverflowHandler

    Description: Default constructor

    Parameters: CStifLogger* aLogger: in: Pointer to CStifLogger object
                TInt aOverFlowSource: in: Over flow source

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TDesLoggerOverflowHandler::TDesLoggerOverflowHandler( CStifLogger* aLogger,
                                            TInt aOverFlowSource )
    {
    iLogger = aLogger;
    iOverFlowSource = aOverFlowSource;

    }

/*
-------------------------------------------------------------------------------

    Class: TDesLoggerOverflowHandler

    Method: TDesLoggerOverflowHandler

    Description: Default constructor

    Parameters: CRDebugOutput* aRDebugOutput: in: Pointer to CFileOutput object
                TInt aOverFlowSource: in: Over flow source

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
// For future needs
// TDesLoggerOverflowHandler::TDesLoggerOverflowHandler(
//                                             CRDebugOutput* aRDebugOutput,
//                                             TInt aOverFlowSource )
//     {
//     iRDebugOutput = aRDebugOutput;
//     iOverFlowSource = aOverFlowSource;
// 
//     }

/*
-------------------------------------------------------------------------------

    Class: TDesLoggerOverflowHandler

    Method: Overflow

    Description: Simple overflow handling(16 bit)

    Parameters: TDes16 &aDes: in: Reference to over flow data

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void TDesLoggerOverflowHandler::Overflow( TDes16& aDes )
    {
    __TRACE( KError, ( _L( "STIFLOGGER: Over flow" ) ) );

    // If overflow
    TInt len( 0 );
    len = aDes.Length();

    // const TInt to TInt avoiding warnings
    TInt maxLogData = KMaxLogData;
    TInt maxTestFileName = KMaxFileName;

    // Overflow: Log() without aStyle or Log() with aStyle
    if ( ( iOverFlowSource == 1 || iOverFlowSource == 2 )
            &&  maxLogData > 60 )
        {
        // Log overflow info if info is in allowed limits
        aDes[len-2] = 13;   // 13 or '\' in Symbian OS
        aDes[len-1] = 10;   // 10 or 'n' in Symbian OS
        // ~60
        iLogger->Send( 0, _L("Log() OVERFLOW: Check aLogInfo and KMaxLogData !!!") );
        }
    // Overflow: WriteDelimiter()
    if ( iOverFlowSource == 3 &&  maxLogData > 70 )
        {
        // Log overflow info if info is in allowed limits, ~70
        iLogger->Send( 0, _L( "WriteDelimiter() OVERFLOW: Check delimiter and KMaxLogData !!!" ) );
        }
    // Overflow: StartHtmlPage()
    if ( iOverFlowSource == 4 &&  maxTestFileName > 70 )
        {
        // Log overflow info if info is in allowed limits, ~70
        iLogger->Send( 0, _L( "aTestFile OVERFLOW: Check aTestFile and KMaxFileName !!!" ) );
        }

    }

/*
-------------------------------------------------------------------------------

    DESCRIPTION

    This module contains implementation of TDes8LoggerOverflowHandler class
    member functions.

-------------------------------------------------------------------------------
*/

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

    Class: TDes8OverflowHandler

    Method: TDes8LoggerOverflowHandler

    Description: Default constructor

    Parameters: CStifLogger* aLogger: in: Pointer to CStifLogger object
                TInt aOverFlowSource: in: Over flow source

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TDes8LoggerOverflowHandler::TDes8LoggerOverflowHandler( CStifLogger* aLogger,
                                                        TInt aOverFlowSource )
    {
    iLogger = aLogger;
    iOverFlowSource = aOverFlowSource;

    }

/*
-------------------------------------------------------------------------------

    Class: TDes8LoggerOverflowHandler

    Method: TDes8LoggerOverflowHandler

    Description: Default constructor

    Parameters: CRDebugOutput* aRDebugOutput: in: Pointer to CFileOutput object
                TInt aOverFlowSource: in: Over flow source

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
// For future needs
// TDes8LoggerOverflowHandler::TDes8LoggerOverflowHandler( 
//                                             CRDebugOutput* aRDebugOutput,
//                                             TInt aOverFlowSource )
//     {
//     iRDebugOutput = aRDebugOutput;
//     iOverFlowSource = aOverFlowSource;

//     }

/*
-------------------------------------------------------------------------------

    Class: TDes8LoggerOverflowHandler

    Method: Overflow

    Description: Simple overflow handling(8 bit)

    Parameters: TDes8 &aDes: in: Reference to over flow data

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void TDes8LoggerOverflowHandler::Overflow( TDes8& aDes )
    {
    __TRACE( KError, ( _L( "STIFLOGGER: Over flow" ) ) );

    // If overflow
    TInt len( 0 );
    len = aDes.Length();

    // const TInt to TInt avoiding warnings
    TInt maxLogData = KMaxLogData;
    TInt maxTestFileName = KMaxFileName;

    // Overflow: Log() without aStyle or Log() with aStyle
    if ( ( iOverFlowSource == 1 || iOverFlowSource == 2 )
            &&  maxLogData > 60 )
        {
        // Log overflow info if info is in allowed limits
        aDes[len-2] = 13;   // 13 or '\' in Symbian OS
        aDes[len-1] = 10;   // 10 or 'n' in Symbian OS
        // ~60
        iLogger->Send( 0, _L("Log() OVERFLOW: Check aLogInfo and KMaxLogData !!!") );
        }
    // Overflow: WriteDelimiter()
    if ( iOverFlowSource == 3 &&  maxLogData > 70 )
        {
        // Log overflow info if info is in allowed limits, ~70
        iLogger->Send( 0, _L( "WriteDelimiter() OVERFLOW: Check delimiter and KMaxLogData !!!" ) );
        }
    // Overflow: StartHtmlPage()
    if ( iOverFlowSource == 4 &&  maxTestFileName > 70 )
        {
        // Log overflow info if info is in allowed limits, ~70
        iLogger->Send( 0, _L( "aTestFile OVERFLOW: Check aTestFile and KMaxFileName !!!" ) );
        }

    }

// ================= OTHER EXPORTED FUNCTIONS =================================
// None

// End of File
