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
* Description: This module contains implementation of CTxtLogger 
* class member functions
*
*/

// INCLUDE FILES
#include <e32std.h>
#include "TxtLogger.h"
#include "Output.h"
#include "LoggerTracing.h"

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

    Class: CTxtLogger

    Method: CTxtLogger

    Description: Default constructor

    C++ default constructor can NOT contain any code, that
    might leave.

    Parameters: COutput* output: in: Output source

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CTxtLogger::CTxtLogger( COutput* output )
    {
    
    iOutput = output;

    }

/*
-------------------------------------------------------------------------------

    Class: CTxtLogger

    Method: ConstructL

    Description: Symbian OS second phase constructor

    Symbian OS default constructor can leave.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTxtLogger::ConstructL()
    {

    }

/*
-------------------------------------------------------------------------------

    Class: CTxtLogger

    Method: NewL

    Description: Two-phased constructor.

    Parameters: const TDesC& aTestPath: in: Log path
                const TDesC& aTestFile: in: Log filename
                TLoggerType aLoggerType: in: File type
                TOutput aOutput: in: Output source
                TBool aOverWrite: in: For file overwrite
                TBool aWithTimeStamp: in: For timestamp
                TBool aWithLineBreak: in: For line break
                TBool aWithEventRanking: in: For events ranking to file
                TBool aThreadIdToLogFile: in: Indicator to thread id adding to
                                              end of the log file
                TBool aCreateLogDir: in: Indicator to directory creation
                TInt  aStaticBufferSize
                TBool aUnicode: in: Indicator if file has to be in unicode format

    Return Values: CTxtLogger*: pointer to CTxtLogger object

    Errors/Exceptions: Leaves if called COutput::NewL method fails

    Status: Proposal

-------------------------------------------------------------------------------
*/
CTxtLogger* CTxtLogger::NewL( const TDesC& aTestPath, 
                                const TDesC& aTestFile,
                                TLoggerType aLoggerType,
                                TOutput aOutput,
                                TBool aOverWrite,
                                TBool aWithTimeStamp,
                                TBool aWithLineBreak,
                                TBool aWithEventRanking,
                                TBool aThreadIdToLogFile,
                                TBool aCreateLogDir,
                                TInt aStaticBufferSize,
                                TBool aUnicode )
    {
    __TRACE( KInfo, ( _L( "STIFLOGGER: Creating txt logger" ) ) );

    // Create COutput object output
    COutput* output = COutput::NewL( aTestPath,
                                        aTestFile,
                                        aLoggerType,
                                        aOutput,
                                        aOverWrite,
                                        aWithTimeStamp,
                                        aWithLineBreak,
                                        aWithEventRanking,
                                        aThreadIdToLogFile,
                                        aCreateLogDir,
                                        aStaticBufferSize,
                                        aUnicode );

    CleanupStack::PushL( output );
    __ASSERT_ALWAYS( output != NULL, User::Leave( KErrNotFound ) );
    // Create CTxtLogger object txtLogger and bind to COutput
    CTxtLogger* txtLogger = new (ELeave) CTxtLogger( output );
    // TXT logger owns output object and it will be destroyed in STIFLogger
    // destructor.
    CleanupStack::Pop( output );
    
    CleanupStack::PushL( txtLogger );
    txtLogger->ConstructL();
    CleanupStack::Pop( txtLogger );
    
    
    return txtLogger;

    }

/*
-------------------------------------------------------------------------------

    Class: CTxtLogger

    Method: ~CTxtLogger

    Description: Destructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CTxtLogger::~CTxtLogger()
    {
    }

/*
-------------------------------------------------------------------------------

    Class: CTxtLogger

    Method: Send

    Description: Send style information and 16 bit data to the output module.

    Parameters: TInt TStyle: in: Text forming
                const TDesC& aData: in: Data to be logged

    Return Values: TInt: Symbian error code.

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CTxtLogger::Send( TInt aStyle, const TDesC& aData )
    {
    // Time stamp indicator
    TBool timestamp( ETrue );
    // Event ranking indicator
    TBool eventranking( ETrue );

    // Only EError, EWarning and EImportant styles are supporter in txt logging
    if ( 0x00100 <= aStyle )    // EError, 0x00100 => 256
        {
        iOutput->Write( timestamp, EFalse, eventranking, _L( "ERROR    " ) );
        aStyle -= 0x00100;
        timestamp = EFalse;     // Time stamp added no time stamp to the
                                // forward operations
        eventranking = EFalse;  // Event ranking added no event ranking to the
                                // forward operations
        }
    if ( 0x00080 <= aStyle )    // EWarning, 0x00080 => 128
        {
        iOutput->Write( timestamp, EFalse, eventranking,
                                                    _L( "WARNING    " ) );
        aStyle -= 0x00080;
        timestamp = EFalse;     // Time stamp added no time stamp to the 
                                // forward operations
        eventranking = EFalse;  // Event ranking added no event ranking to the
                                // forward operations
        }
    if ( 0x00040 <= aStyle )    // EImportant, 0x00040 => 64
        {
        iOutput->Write( timestamp, EFalse, eventranking,
                                                    _L( "IMPORTANT    " ) );
        aStyle -= 0x00040;
        timestamp = EFalse;     // Time stamp added no time stamp to the 
                                // forward operations
        eventranking = EFalse;  // Event ranking added no event ranking to the
                                // forward operations
        }

    return iOutput->Write( timestamp, ETrue, eventranking, aData );

    }

/*
-------------------------------------------------------------------------------

    Class: CTxtLogger

    Method: Send

    Description: Send style information and 8 bit data to the output module.

    Parameters: TInt TStyle: in: Text forming
                const TDesC8& aData: in: Data to be logged

    Return Values: TInt: Symbian error code.

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CTxtLogger::Send( TInt aStyle, const TDesC8& aData )
    {
    // Time stamp indicator
    TBool timestamp( ETrue );
    // Event ranking indicator
    TBool eventranking( ETrue );

    // Only EError, EWarning and EImportant styles are supporter in txt logging
    if ( 0x00100 <= aStyle )    // EError, 0x00100 => 256
        {
        iOutput->Write( timestamp, EFalse, eventranking, _L8( "ERROR    " ) );
        aStyle -= 0x00100;
        timestamp = EFalse;     // Time stamp added no time stamp to the
                                // forward operations
        eventranking = EFalse;  // Event ranking added no event ranking to the
                                // forward operations
        }
    if ( 0x00080 <= aStyle )    // EWarning, 0x00080 => 128
        {
        iOutput->Write( timestamp, EFalse, eventranking,
                                                    _L8( "WARNING    " ) );
        aStyle -= 0x00080;
        timestamp = EFalse;     // Time stamp added no time stamp to the 
                                // forward operations
        eventranking = EFalse;  // Event ranking added no event ranking to the
                                // forward operations
        }
    if ( 0x00040 <= aStyle )    // EImportant, 0x00040 => 64
        {
        iOutput->Write( timestamp, EFalse, eventranking,
                                                    _L8( "IMPORTANT    " ) );
        aStyle -= 0x00040;
        timestamp = EFalse;     // Time stamp added no time stamp to the 
                                // forward operations
        eventranking = EFalse;  // Event ranking added no event ranking to the
                                // forward operations
        }

    return iOutput->Write( timestamp, ETrue, eventranking, aData );

    }

// ================= OTHER EXPORTED FUNCTIONS =================================
// None

// End of File
