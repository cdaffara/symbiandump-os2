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
* Description: This module contains implementation of CHtmlLogger 
* class member functions.
*
*/

// INCLUDE FILES
#include <e32std.h>
#include "HtmlLogger.h"
#include "Output.h"
#include "LoggerTracing.h"
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

    Class: CHtmlLogger

    Method: CHtmlLogger

    Description: Default constructor

    C++ default constructor can NOT contain any code, that
    might leave.

    Parameters: COutput* output: in: Output source
                const TDesC& aTestFile: in: Log filename
                TBool aThreadIdToLogFile: in: Indicator to thread id adding to
                                              end of the log file

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CHtmlLogger::CHtmlLogger( COutput* output,
                          const TDesC& aTestFile,
                          TBool aThreadIdToLogFile )
    {
    iOutput = output;
    StartHtmlPage( aTestFile, aThreadIdToLogFile );
    }


/*
-------------------------------------------------------------------------------

    Class: CHtmlLogger

    Method: ConstructL

    Description: Symbian OS second phase constructor

    Symbian OS default constructor can leave.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CHtmlLogger::ConstructL()
    {
    
    }

/*
-------------------------------------------------------------------------------

    Class: CHtmlLogger

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

    Return Values: CHtmlLogger*: pointer to CHtmlLogger object

    Errors/Exceptions: Leaves if called COutput::NewL method fails

    Status: Proposal

-------------------------------------------------------------------------------
*/
CHtmlLogger* CHtmlLogger::NewL( const TDesC& aTestPath,
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
    __TRACE( KInfo, ( _L( "STIFLOGGER: Creating html logger" ) ) );

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
    // Create CTxtLogger object htmlLogger and bind to COutput
    CHtmlLogger* htmlLogger = new (ELeave) CHtmlLogger( output,
                                                        aTestFile,
                                                        aThreadIdToLogFile );
    // HTML logger owns output object and it will be destroyed in STIFLogger
    // destructor.
    CleanupStack::Pop( output ); 
    
    CleanupStack::PushL( htmlLogger );
    htmlLogger->ConstructL();
    CleanupStack::Pop( htmlLogger );

    return htmlLogger;

    }

/*
-------------------------------------------------------------------------------

    Class: CHtmlLogger

    Method: ~CHtmlLogger

    Description: Destructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CHtmlLogger::~CHtmlLogger()
    {

    EndHtmlPage();

    }

/*
-------------------------------------------------------------------------------

    Class: CHtmlLogger

    Method: StartHtmlPage

    Description: Initialize html page.

    Parameters: const TDesC& aTestFile: in: Log filename
                TBool aThreadIdToLogFile: in: Indicator to thread id adding to
                                              end of the log file

    Return Values: None

    Errors/Exceptions:  TDesLoggerOverflowHandler called if logged data is 
                        over KMaxFileName.

    Status: Approved

-------------------------------------------------------------------------------
*/
void CHtmlLogger::StartHtmlPage( const TDesC& aTestFile,
                                    TBool aThreadIdToLogFile )
    {
    TFileName title;

    // Create overflow handler.
    TDesLoggerOverflowHandler overFlowHandler( this, 4 );

    // Title
    if( aTestFile.Length() < KMaxFileName )
        {
        title.Append( aTestFile );
        }    
    else
        {
        // If the title size is over the KMaxFileName rest of the title
        // will cut. Use normal overflowhandler to print overflow information.
        TBuf<4> empty; // Not really used.
        overFlowHandler.Overflow( empty );
        TPtrC cuttedtitle = aTestFile.Left( KMaxFileName );
        title.Append( cuttedtitle );
        }        
        
    // Add Thread id to title if enough space and test file is given and 
    // aThreadIdToLogFile is true
    if( title.Length() <  ( KMaxFileName - 3 ) && aTestFile.Length() > 0 &&
        aThreadIdToLogFile )
        {
        title.Append( _L( "_" ) );
        RThread threadHandle;                   // A handle to a thread
        TThreadId id = threadHandle.Id();       // Encapsulates the 
                                                // Id of a thread
        // Appends id in hexadesimal format                           
        title.AppendFormat( _L( "%x" ), (TInt)id );   
        }

    // Html start tags to whole page and header section
    iOutput->Write( EFalse, EFalse, EFalse,
                    _L8( "\n<html>\n<head>\n<title>" ) );
    // Html file title
    iOutput->Write( EFalse, EFalse, EFalse, title );
    // Html header section end tags and start tag to body section
    iOutput->Write( EFalse, EFalse, EFalse,
                    _L8( "</title>\n</head>\n\n\n<body>\n" ) );

    }

/*
-------------------------------------------------------------------------------

    Class: CHtmlLogger

    Method: EndHtmlPage

    Description: Finalize html page.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CHtmlLogger::EndHtmlPage()
    {
    // Html page and body section end tags
    iOutput->Write( EFalse, EFalse, EFalse,
                    _L8( "\n\n\n</html>\n</body>\n\n\n" ) );

    }

/*
-------------------------------------------------------------------------------

    Class: CHtmlLogger

    Method: StyleStarting

    Description: Start style information logging.

    Parameters: TInt aStyle: in: Style information
                TBool aTimeStamp: inout: Time stamp indicator
                TBool aEventRanking: inout: Event ranking indicator

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CHtmlLogger::StyleStarting( TInt aStyle, TBool& aTimeStamp,
                                    TBool& aEventRanking )
    {
    if ( 0x00020 <= aStyle ||       // ERed, 0x00020 => 32
            0x00040 <= aStyle ||    // EImportant, 0x00040 => 64
            0x00080 <= aStyle ||    // EWarning, 0x00080 => 128
            0x00100 <= aStyle )     // EError, 0x00100 => 256
        {
        iOutput->Write( EFalse, EFalse, EFalse,
                                        _L8( "<FONT COLOR='#FF0000'>" ) );
        if ( 0x00100 <= aStyle )
            {
            iOutput->Write( aTimeStamp, EFalse, aEventRanking,
                                        _L8( "ERROR..." ) );
            aStyle -= 0x00100;
            aTimeStamp = EFalse;    // Time stamp added no time stamp to the
                                    // forward operations
            aEventRanking = EFalse; // Event ranking added no event ranking
                                    // to the forward operations
            }
        if ( 0x00080 <= aStyle )
            {
            iOutput->Write( aTimeStamp, EFalse, aEventRanking,
                                        _L8( "WARNING..." ) );
            aStyle -= 0x00080;
            aTimeStamp = EFalse;    // Time stamp added no time stamp to the 
                                    // forward operations
            aEventRanking = EFalse; // Event ranking added no event ranking
                                    // to the forward operations
            }
        if ( 0x00040 <= aStyle )
            {
            iOutput->Write( aTimeStamp, EFalse, aEventRanking,
                                        _L8( "IMPORTANT..." ) );
            aStyle -= 0x00040;
            aTimeStamp = EFalse;    // Time stamp added no time stamp to the 
                                    // forward operations
            aEventRanking = EFalse; // Event ranking added no event ranking
                                    // to the forward operations
            }
        if ( 0x00020 <= aStyle )
            {
            aStyle -= 0x00020;
            }
        }
    // EGreen, 0x00010 => 16
    if ( 0x00010 <= aStyle )
        {
        iOutput->Write( EFalse, EFalse, EFalse,
                        _L8( "<FONT COLOR='#009900'>" ) );
        aStyle -= 0x00010;
        }
    // EBlue, 0x00008 => 8
    if ( 0x00008 <= aStyle )
        {
        iOutput->Write( EFalse, EFalse, EFalse,
                        _L8( "<FONT COLOR='#3333FF'>" ) );
        aStyle -= 0x00008;
        }
    // EUnderline, 0x00004 => 4
    if ( 0x00004 <= aStyle )
        {
        iOutput->Write( EFalse, EFalse, EFalse, _L8( "<U>" ) );
        aStyle -= 0x00004;
        }
    // ECursive, 0x00002 => 2
    if ( 0x00002 <= aStyle )
        {
        iOutput->Write( EFalse, EFalse, EFalse, _L8( "<I>" ) );
        aStyle -= 0x00002;
        }
    // EBold, 0x00001 => 1
    if ( 0x00001 <= aStyle )
        {
        iOutput->Write( EFalse, EFalse, EFalse, _L8( "<B>" ) );
        aStyle -= 0x00001;
        }

    }

/*
-------------------------------------------------------------------------------

    Class: CHtmlLogger

    Method: StyleEnding

    Description: Ends style information logging.

    Parameters: TInt aStyle: in: Style information

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CHtmlLogger::StyleEnding( TInt aStyle )
    {
    if ( 0x00020 <= aStyle ||       // ERed, 0x00020 => 32
            0x00040 <= aStyle ||    // EImportant, 0x00040 => 64
            0x00080 <= aStyle ||    // EWarning, 0x00080 => 128
            0x00100 <= aStyle )     // EError, 0x00100 => 256
        {
        iOutput->Write( EFalse, EFalse, EFalse, _L8( "</FONT>" ) );
        if ( 0x00100 <= aStyle )
            {
            aStyle -= 0x00100;
            }
        if ( 0x00080 <= aStyle )
            {
            aStyle -= 0x00080;
            }
        if ( 0x00040 <= aStyle )
            {
            aStyle -= 0x00040;
            }
        if ( 0x00020 <= aStyle )
            {
            aStyle -= 0x00020;
            }
        }
    // EGreen, 0x00010 => 16
    if ( 0x00010 <= aStyle )
        {
        iOutput->Write( EFalse, EFalse, EFalse, _L8( "</FONT>" ) );
        aStyle -= 0x00010;
        }
    // EBlue, 0x00008 => 8
    if ( 0x00008 <= aStyle )
        {
        iOutput->Write( EFalse, EFalse, EFalse, _L8( "</FONT>" ) );
        aStyle -= 0x00008;
        }
    // EUnderline, 0x00004 => 4
    if ( 0x00004 <= aStyle )
        {
        iOutput->Write( EFalse, EFalse, EFalse, _L8( "</U>" ) );
        aStyle -= 0x00004;
        }
    // ECursive, 0x00002 => 2
    if ( 0x00002 <= aStyle )
        {
        iOutput->Write( EFalse, EFalse, EFalse, _L8( "</I>" ) );
        aStyle -= 0x00002;
        }
    // EBold, 0x00001 => 1
    if ( 0x00001 <= aStyle )
        {
        iOutput->Write( EFalse, EFalse, EFalse, _L8( "</B>" ) );
        aStyle -= 0x00001;
        }

    }

/*
-------------------------------------------------------------------------------

    Class: CHtmlLogger

    Method: Send

    Description: Send style information and 16 bit data to the output module

    Parameters: TInt TStyle: in: Text forming
                const TDesC& aData: in: Data to be logged

    Return Values: TInt: Symbian error code.

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CHtmlLogger::Send( TInt aStyle, const TDesC& aData )
    {
    // Time stamp indicator
    TBool timestamp( ETrue );
    // Event ranking indicator
    TBool eventranking( ETrue );

    StyleStarting( aStyle, timestamp, eventranking );

    TInt ret = iOutput->Write( timestamp, ETrue, eventranking, aData );

    StyleEnding( aStyle );

    return ret;

    }

/*
-------------------------------------------------------------------------------

    Class: CHtmlLogger

    Method: Send

    Description: Send style information and 8 bit data to the output module

    Parameters: TInt TStyle: in: Text forming
                const TDesC8& aData: in: Data to be logged

    Return Values: TInt: Symbian error code.

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CHtmlLogger::Send( TInt aStyle, const TDesC8& aData )
    {
    // Time stamp indicator
    TBool timestamp( ETrue );
    // Event ranking indicator
    TBool eventranking( ETrue );

    StyleStarting( aStyle, timestamp, eventranking );

    TInt ret = iOutput->Write( timestamp, ETrue, eventranking, aData );

    StyleEnding( aStyle );

    return ret;

    }

// ================= OTHER EXPORTED FUNCTIONS =================================
// None

// End of File
