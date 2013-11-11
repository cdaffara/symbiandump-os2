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
* Description: This module contains implementation of CStifLogger 
* class member functions.
*
*/

// INCLUDE FILES
#include <e32std.h>
#include "StifLogger.h"
#include "TxtLogger.h"
#include "HtmlLogger.h"
#include "DataLogger.h"
#include "LoggerTracing.h"
#include "LoggerOverFlow.h"
#include "SettingServerClient.h"

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

    Class: CStifLogger

    Method: CStifLogger

    Description: Default constructor

    C++ default constructor can NOT contain any code, that
    might leave.

    Parameters: None
    
    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CStifLogger::CStifLogger()
    {

    }

/*
-------------------------------------------------------------------------------

    Class: CStifLogger

    Method: NewL

    Description: Two-phased constructor.

    Parameters: const TDesC& aTestPath: in: Path to logged information
                const TDesC& aTestFile: in: Log name for information
                TLoggerType aLoggerType: in: Log file type(txt, html,
                                             data)
                TOutput aOutput: in: Output source(File)
                TBool aOverWrite: in: Indicator to file overwrite
                TBool aWithTimeStamp: in: Indicator to time stamp
                TBool aWithLineBreak: in: Indicator to line break
                TBool aWithEventRanking: in: Indicator to event ranking
                TBool aThreadIdToLogFile: in: Indicator to thread id adding to
                                              end of the log file
                TBool aCreateLogDir: in: Indicator to directory creation
                TInt  aStaticBufferSize
                TBool aUnicode: in: Indicator if file has to be in unicode format

    Return Values: CStifLogger* logger: pointer to CStifLogger object

    Errors/Exceptions: Leaves if aTestPath or aTestFile length is over KMaxName
                       Leaves if called serv.Connect() method fails
                       Leaves if called CHtmlLogger::NewL method fails
                       Leaves if called CDataLogger::NewL method fails
                       Leaves if called CTxtLogger::NewL method fails

    Status: Proposal

-------------------------------------------------------------------------------
*/
EXPORT_C CStifLogger* CStifLogger::NewL( const TDesC& aTestPath,
                                            const TDesC& aTestFile,
                                            TLoggerType aLoggerType,
                                            TOutput aOutput,
                                            TBool aOverWrite,
                                            TBool aWithTimeStamp,
                                            TBool aWithLineBreak,
                                            TBool aWithEventRanking,
                                            TBool aThreadIdToLogFile,
                                            TBool aCreateLogDir,
                                            TInt  aStaticBufferSize,
                                            TBool aUnicode )
    {
    if( KMaxName < aTestPath.Length() || KMaxName < aTestFile.Length() )
        {
        User::Leave( KErrArgument );
        }

    // Handle to Setting server.
    RSettingServer settingServer;
    // Connect to the Setting server and create session
    TInt ret = settingServer.Connect();
    if ( ret != KErrNone )
        {
        User::Leave( ret );
        }
    // Struct to StifLogger settigs.
    TLoggerSettings loggerSettings; 
    // Parse StifLogger defaults from STIF initialization file.
    ret = settingServer.GetLoggerSettings( loggerSettings );
    if ( ret != KErrNone )
        {
        User::Leave( ret );
        } 

    TName testPath = aTestPath;
    TName testFile = aTestFile;

    // Take in use the overwrite parameters
    OverwriteLoggerSettings( loggerSettings, testPath, testFile, aLoggerType,
                                aOutput, aOverWrite, aWithTimeStamp,
                                aWithLineBreak, aWithEventRanking,
                                aThreadIdToLogFile, aCreateLogDir,
                                aUnicode );

    // Close Setting server session
    settingServer.Close();

    // Html file logging
    if ( aLoggerType == EHtml )
        {
        // Create CHtmlLogger object logger
        CHtmlLogger* logger = CHtmlLogger::NewL( testPath,
                                                    testFile,
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
        return (CStifLogger*)logger;
        }
    // Data file
    else if ( aLoggerType == EData )
        {
        // Create CDataLogger object logger
        CDataLogger* logger = CDataLogger::NewL( testPath,
                                                    testFile,
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
        return (CStifLogger*)logger;
        }
    // For default: text file logging
    else
        {
        // Create CTxtLogger object logger
        CTxtLogger* logger = CTxtLogger::NewL( testPath,
                                                testFile,
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
        return (CStifLogger*)logger;
        }

    }

/*
-------------------------------------------------------------------------------

    Class: CStifLogger

    Method: NewL

    Description: Two-phased constructor.

    TestEngine's and TestServer's are friend. For TestEngine's and
    TestServer's StifLogger creation.

    Parameters: const TDesC& aTestPath: in: Path to logged information
                const TDesC& aTestFile: in: Log name for information
                TLoggerSettings& aLoggerSettings: in: Struct for StifLogger
                                                      settigs

    Return Values: CStifLogger* logger: pointer to CStifLogger object

    Errors/Exceptions:  Leaves if called CHtmlLogger::NewL method fails
                        Leaves if called CDataLogger::NewL method fails
                        Leaves if called CTxtLogger::NewL method fails

    Status: Proposal

-------------------------------------------------------------------------------
*/
EXPORT_C CStifLogger* CStifLogger::NewL( const TDesC& aTestPath,
                                            const TDesC& aTestFile,
                                            TLoggerSettings& aLoggerSettings )
    {
    TInt cpu( 0 );
    HAL::Get( HALData::ECPU, cpu );

    // HW environment
    if ( cpu == HALData::ECPU_ARM )
        {
        // Html file logging
        if ( aLoggerSettings.iHardwareFormat == EHtml )
            {
            // Create CHtmlLogger object logger
            CHtmlLogger* logger = CHtmlLogger::NewL( aTestPath,
                                    aTestFile,
                                    aLoggerSettings.iHardwareFormat,
                                    aLoggerSettings.iHardwareOutput,
                                    aLoggerSettings.iOverwrite,
                                    aLoggerSettings.iTimeStamp,
                                    aLoggerSettings.iLineBreak,
                                    aLoggerSettings.iEventRanking,
                                    aLoggerSettings.iThreadId,
                                    aLoggerSettings.iCreateLogDirectories,
                                    0,
                                    aLoggerSettings.iUnicode );
            return (CStifLogger*)logger;
            }
        // Data file
        else if ( aLoggerSettings.iHardwareFormat == EData )
            {
            // Create CDataLogger object logger
            CDataLogger* logger = CDataLogger::NewL( aTestPath,
                                    aTestFile,
                                    aLoggerSettings.iHardwareFormat,
                                    aLoggerSettings.iHardwareOutput,
                                    aLoggerSettings.iOverwrite,
                                    aLoggerSettings.iTimeStamp,
                                    aLoggerSettings.iLineBreak,
                                    aLoggerSettings.iEventRanking,
                                    aLoggerSettings.iThreadId,
                                    aLoggerSettings.iCreateLogDirectories,
                                    0,
                                    aLoggerSettings.iUnicode );
            return (CStifLogger*)logger;
            }
        // For default: text file logging
        else
            {
            // Create CTxtLogger object logger
            CTxtLogger* logger = CTxtLogger::NewL( aTestPath,
                                    aTestFile,
                                    aLoggerSettings.iHardwareFormat,
                                    aLoggerSettings.iHardwareOutput,
                                    aLoggerSettings.iOverwrite,
                                    aLoggerSettings.iTimeStamp,
                                    aLoggerSettings.iLineBreak,
                                    aLoggerSettings.iEventRanking,
                                    aLoggerSettings.iThreadId,
                                    aLoggerSettings.iCreateLogDirectories,
                                    0,
                                    aLoggerSettings.iUnicode );
            return (CStifLogger*)logger;
            }
        } // End of HW environment branch

    // Wins environment ( cpu == HALData::ECPU_X86 )
    else
        {
        // Html file logging
        if ( aLoggerSettings.iEmulatorFormat == EHtml )
            {
            // Create CHtmlLogger object logger
            CHtmlLogger* logger = CHtmlLogger::NewL( aTestPath,
                                    aTestFile,
                                    aLoggerSettings.iEmulatorFormat,
                                    aLoggerSettings.iEmulatorOutput,
                                    aLoggerSettings.iOverwrite,
                                    aLoggerSettings.iTimeStamp,
                                    aLoggerSettings.iLineBreak,
                                    aLoggerSettings.iEventRanking,
                                    aLoggerSettings.iThreadId,
                                    aLoggerSettings.iCreateLogDirectories,
                                    0,
                                    aLoggerSettings.iUnicode );
            return (CStifLogger*)logger;
            }
        // Data file
        else if ( aLoggerSettings.iEmulatorFormat == EData )
            {
            // Create CDataLogger object logger
            CDataLogger* logger = CDataLogger::NewL( aTestPath,
                                    aTestFile,
                                    aLoggerSettings.iEmulatorFormat,
                                    aLoggerSettings.iEmulatorOutput,
                                    aLoggerSettings.iOverwrite,
                                    aLoggerSettings.iTimeStamp,
                                    aLoggerSettings.iLineBreak,
                                    aLoggerSettings.iEventRanking,
                                    aLoggerSettings.iThreadId,
                                    aLoggerSettings.iCreateLogDirectories,
                                    0,
                                    aLoggerSettings.iUnicode );
            return (CStifLogger*)logger;
            }
        // For default: text file logging
        else
            {
            // Create CTxtLogger object logger
            CTxtLogger* logger = CTxtLogger::NewL( aTestPath,
                                    aTestFile,
                                    aLoggerSettings.iEmulatorFormat,
                                    aLoggerSettings.iEmulatorOutput,
                                    aLoggerSettings.iOverwrite,
                                    aLoggerSettings.iTimeStamp,
                                    aLoggerSettings.iLineBreak,
                                    aLoggerSettings.iEventRanking,
                                    aLoggerSettings.iThreadId,
                                    aLoggerSettings.iCreateLogDirectories,
                                    0,
                                    aLoggerSettings.iUnicode );
            return (CStifLogger*)logger;
            }
        } // End of WINS environment branch

    }

/*
-------------------------------------------------------------------------------

    Class: CStifLogger

    Method: ~CStifLogger

    Description: Destructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CStifLogger::~CStifLogger()
    {

    delete iOutput;
    iOutput = 0;
    
    }

/*
-------------------------------------------------------------------------------

    Class: CStifLogger

    Method: OverwriteLoggerSettings

    Description: Local CStifLogger's method for StifLogger settings
                 overwriting.

    Overwrite parameters may set in TestFramework.ini file.

    Parameters: TLoggerSettings& aLoggerSettings: inout; Overwrite parameters
                TName& aTestPath: inout: Path to logged information
                TName& aTestFile: inout: Log file name for information
                TLoggerType& aLoggerType: inout: Log file type(txt, html, data)
                TOutput& aOutput: inout: Output source(File)
                TBool& aOverWrite: inout: Indicator to file overwrite
                TBool& aWithTimeStamp: inout: Indicator to time stamp
                TBool& aWithLineBreak: inout: Indicator to line break
                TBool& aWithEventRanking: inout: Indicator to event ranking
                TBool& aThreadIdToLogFile: inout: Indicator to thread id adding to
                                                  end of the log file
                TBool& aCreateLogDir: inout: Indicator to directory creation
                TBool& aUnicode: inout: Indicator if log will be written to file in unicode format
    
    Return Values: None

    Errors/Exceptions: None 

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CStifLogger::OverwriteLoggerSettings( TLoggerSettings& aLoggerSettings,
                                            TName& aTestPath,
                                            TName& aTestFile,
                                            TLoggerType& aLoggerType,
                                            TOutput& aOutput,
                                            TBool& aOverWrite,
                                            TBool& aWithTimeStamp,
                                            TBool& aWithLineBreak,
                                            TBool& aWithEventRanking,
                                            TBool& aThreadIdToLogFile,
                                            TBool& aCreateLogDir,
                                            TBool& aUnicode )
    {
    // Which environment is in use
    TInt cpu( 0 );

    // Indicator is file type remove acceptable operation
    TBool removeFileType( EFalse );

    // Get environment
    HAL::Get( HALData::ECPU, cpu );

    // HW environment
    if ( cpu == HALData::ECPU_ARM )
        {
        TInt pathLen = aTestPath.Length() + 
                        aLoggerSettings.iHardwarePath.Length();

        // Check that path overwrite setting is defined and length is legal
        if( aLoggerSettings.iIsDefined.iHwPath && pathLen < KMaxName )
            {
            TName newPath;
            newPath = aLoggerSettings.iHardwarePath;
            // Check is '\' the last character
            TInt ret( 0 );
            ret = newPath.LocateReverse( 92 );
            // Is '\' character founded
            if( ret != KErrNotFound )
                {
                // Is '\' last character
                if( ret == ( newPath.Length() - 1 ) )
                    {
                    // delete last '\'
                    newPath.Delete( 
                        ( aLoggerSettings.iHardwarePath.Length() -1 ), 1 );
                    }
                }
            ret = aTestPath.LocateReverse(92);
                        // Is '\' character founded
                             
           if (ret != KErrNotFound)
                {
                // Is '\' last character
                if (ret == (aTestPath.Length() - 1))
                     {
                     // delete last '\'
                     aTestPath.Delete((aTestPath.Length()- 1), 1);
                     }
                }
             ret = aTestPath.LocateReverse('\\');
               
             //cut logger specyfic directory (like: \\Demomodule)
                           
             aTestPath = aTestPath.Right(aTestPath.Length()-ret);
                     
             // patch must end with '\'
             aTestPath.Append('\\');
                       
             // create path by combining path form .ini file with test module name
             newPath.Append(aTestPath);                                             
             aTestPath = newPath;
            }
        if( aLoggerSettings.iIsDefined.iHwFormat )
            {
            aLoggerType = aLoggerSettings.iHardwareFormat;
            // Remove file type if it is set
            removeFileType = ETrue;
            }
        if( aLoggerSettings.iIsDefined.iHwOutput )
            {
            aOutput = aLoggerSettings.iHardwareOutput;
            }
        }

    // Wins environment ( cpu == HALData::ECPU_X86 )
    else
        {
        TInt pathLen = aTestPath.Length() + 
                        aLoggerSettings.iEmulatorPath.Length();

        // Check that path overwrite setting is defined and length is legal
        if( aLoggerSettings.iIsDefined.iPath  && pathLen < KMaxName )
            {
            TName newPath;
            newPath = aLoggerSettings.iEmulatorPath;
            // Check is '\' the last character
            TInt ret( 0 );
            ret = newPath.LocateReverse( 92 );
            // Is '\' character founded
            if( ret != KErrNotFound )
                {
                // Is '\' last character
                if( ret == ( newPath.Length() - 1 ) )
                    {
                    // delete last '\'
                    newPath.Delete( 
                        ( aLoggerSettings.iEmulatorPath.Length() -1 ), 1 );
                    }
                }
            
                ret = aTestPath.LocateReverse(92);
                        // Is '\' character founded
                             
                       if (ret != KErrNotFound)
                            {
                            // Is '\' last character
                            if (ret == (aTestPath.Length() - 1))
                                {
                                // delete last '\'
                                aTestPath.Delete((aTestPath.Length()- 1), 1);
                                }
                            }
            ret = aTestPath.LocateReverse('\\');
                       
            //cut logger specyfic directory (like: \\Demomodule)      
            aTestPath = aTestPath.Right(aTestPath.Length()-ret);                       
            // patch must end with '\'
            aTestPath.Append('\\'); 
               
            // create path by combining path form .ini file with test module name
            newPath.Append(aTestPath);                                                          
            aTestPath = newPath;
            }
        if( aLoggerSettings.iIsDefined.iFormat )
            {
            aLoggerType = aLoggerSettings.iEmulatorFormat;
            // Remove file type if it is set
            removeFileType = ETrue;
            }
        if( aLoggerSettings.iIsDefined.iOutput )
            {
            aOutput = aLoggerSettings.iEmulatorOutput;
            }
        }

    // Rest of the overwrite values
    if( aLoggerSettings.iIsDefined.iOverwrite )
        {
        aOverWrite = aLoggerSettings.iOverwrite;
        }
    if( aLoggerSettings.iIsDefined.iTimeStamp )
        {
        aWithTimeStamp = aLoggerSettings.iTimeStamp;
        }
    if( aLoggerSettings.iIsDefined.iLineBreak )
        {
        aWithLineBreak = aLoggerSettings.iLineBreak;
        }
    if( aLoggerSettings.iIsDefined.iEventRanking )
        {
        aWithEventRanking = aLoggerSettings.iEventRanking;
        }
    if( aLoggerSettings.iIsDefined.iThreadId )
        {
        aThreadIdToLogFile = aLoggerSettings.iThreadId;
        }
    if( aLoggerSettings.iIsDefined.iCreateLogDir )
        {
        aCreateLogDir = aLoggerSettings.iCreateLogDirectories;
        }
    if( aLoggerSettings.iIsDefined.iUnicode )
        {
        aUnicode = aLoggerSettings.iUnicode;
        }

    // Remove file type if allowed and if file type is set to filename
    if( removeFileType )
        {
        // Remove file type '.XXX' from file name if exist
        if( aTestFile.Find( _L( "." ) ) )
            {
            TParse parse;
            parse.Set( aTestFile, NULL, NULL );
            // Type length
            TInt length( 0 );
            // '.XXX'
            length = parse.Ext().Length();
            // Delete type
            aTestFile.Delete ( aTestFile.Length()-length, length );
            }
        }

    }

/*
-------------------------------------------------------------------------------

    Class: CStifLogger

    Method: Log

    Description: Log a 16 bit information.

    This log method accepts only one parameters

    Parameters: const TDesC aLogInfo: in: 8 bit data to be logged

    Return Values: TInt: Symbian error code.

    Errors/Exceptions:  None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CStifLogger::Log( const TDesC& aLogInfo )
    {
    // No text style info
    return Send( ENoStyle, aLogInfo );

    }

/*
-------------------------------------------------------------------------------

    Class: CStifLogger

    Method: Log

    Description: Log a 8 bit information.

    This log method accepts only one parameters

    Parameters: const TDesC8 aLogInfo: in: 8 bit data to be logged

    Return Values: TInt: Symbian error code.

    Errors/Exceptions:  None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CStifLogger::Log( const TDesC8& aLogInfo )
    {
    // No text style info
    return Send( ENoStyle, aLogInfo );

    }

/*
-------------------------------------------------------------------------------

    Class: CStifLogger

    Method: Log

    Description: Log a 16 bit information.

    There is also parameter to styling text information e.g. text color.

    Parameters: TInt aStyle: in: Logged text forming parameter
                const TDesC aLogInfo: in: 16 bit data to be logged

    Return Values: TInt: Symbian error code.

    Errors/Exceptions:  None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CStifLogger::Log( TInt aStyle, const TDesC& aLogInfo )
    {
    return Send( aStyle, aLogInfo );

    }

/*
-------------------------------------------------------------------------------

    Class: CStifLogger

    Method: Log

    Description: Log a 8 bit information.

    There is also parameter to styling text information e.g. text color.

    Parameters: TInt aStyle: in: Logged text forming parameter
                const TDesC8 aLogInfo: in: 8 bit data to be logged

    Return Values: TInt: Symbian error code.

    Errors/Exceptions:  None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CStifLogger::Log( TInt aStyle, const TDesC8& aLogInfo )
    {
    return Send( aStyle, aLogInfo );

    }

/*
-------------------------------------------------------------------------------

    Class: CStifLogger

    Method: Log

    Description: Log a 16 bit information.

    This log method accepts several parameters.

    Parameters: TRefByValue<const TDesC> aLogInfo: in: A templated class which 
                encapsulates a reference to an object within a wrapper

    Return Values: TInt: Symbian error code.

    Errors/Exceptions:  TDesLoggerOverflowHandler called if logged information
                        is over KMaxLogData

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CStifLogger::Log( TRefByValue<const TDesC> aLogInfo,... )
    {
    VA_LIST list;
    VA_START( list, aLogInfo );
    TLogInfo logInfo;

    // Create overflow handler. If the log information size is over the
    // KMaxLogData rest of the information will cut.
    TDesLoggerOverflowHandler overFlowHandler( this, 1 );

    // Parse parameters
    logInfo.AppendFormatList( aLogInfo, list, &overFlowHandler );

    // No text style info
    return Send( ENoStyle, logInfo );

    }

/*
-------------------------------------------------------------------------------

    Class: CStifLogger

    Method: Log

    Description: Log a 8 bit information.

    This log method accepts several parameters.

    Parameters: TRefByValue<const TDesC8> aLogInfo: in: A templated class which 
                encapsulates a reference to an object within a wrapper

    Return Values: TInt: Symbian error code.

    Errors/Exceptions:  TDes8LoggerOverflowHandler called if logged information is 
                        over KMaxLogData

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CStifLogger::Log( TRefByValue<const TDesC8> aLogInfo,... )
    {
    VA_LIST list;
    VA_START( list, aLogInfo );
    TLogInfo8 logInfo;

    // Create overflow handler. If the log information size is over the
    // KMaxLogData rest of the information will cut.
    TDes8LoggerOverflowHandler overFlowHandler( this, 1 );

    // Parse parameters
    logInfo.AppendFormatList( aLogInfo, list, &overFlowHandler );

    // No text style info
    return Send( ENoStyle, logInfo );

    }

/*
-------------------------------------------------------------------------------

    Class: CStifLogger

    Method: Log

    Description: Log a 16 bit information.

    This log method accepts several parameters. There is also parameter to
    styling text information e.g. text color.

    Parameters: TInt aStyle: in: Logged text forming parameter
                TRefByValue<const TDesC> aLogInfo: in: A templated class 
                which encapsulates a reference to an object
                within a wrapper

    Return Values: TInt: Symbian error code.

    Errors/Exceptions:  TDesOverflowHandler called if logged information is
                        over KMaxLogData

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CStifLogger::Log( TInt aStyle, 
                           TRefByValue<const TDesC> aLogInfo,... )
    {
    VA_LIST list;
    VA_START( list, aLogInfo );
    TLogInfo logInfo;

    // Create overflow handler. If the log information size is over the
    // KMaxLogData rest of the information will cut.
    TDesLoggerOverflowHandler overFlowHandler( this, 2 );

    // Parse parameters
    logInfo.AppendFormatList( aLogInfo, list, &overFlowHandler );

    return Send( aStyle, logInfo );

    }

/*
-------------------------------------------------------------------------------

    Class: CStifLogger

    Method: Log

    Description: Log a 8 bit information.

    This log method accepts several parameters. There is also parameter to
    styling text information e.g. text color.

    Parameters: TInt aStyle: in: Logged text forming parameter
                TRefByValue<const TDesC8> aLogInfo: in: A templated class 
                which encapsulates a reference to an object
                within a wrapper

    Return Values: TInt: Symbian error code.

    Errors/Exceptions:  TDes8LoggerOverflowHandler called if logged information is
                        over KMaxLogData

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CStifLogger::Log( TInt aStyle, 
                           TRefByValue<const TDesC8> aLogInfo,... )
    {
    VA_LIST list;
    VA_START( list, aLogInfo );
    TLogInfo8 logInfo;

    // Create overflow handler. If the log information size is over the
    // KMaxLogData rest of the information will cut.
    TDes8LoggerOverflowHandler overFlowHandler( this, 2 );

    // Parse parameters
    logInfo.AppendFormatList( aLogInfo, list, &overFlowHandler );

    return Send( aStyle, logInfo );

    }

/*
-------------------------------------------------------------------------------

    Class: CStifLogger

    Method: WriteDelimiter

    Description: Log a 16 bit delimiter.

    Log a delimiters required locations to the log information.
    This will be used if parameters are not given when calling this method.

    Parameters: const TDesC& aDelimiter: in: Logged delimiter(e.g. '#' or 'XO')
                TInt aCount: in: Repeated count for delimiter

    Return Values: TInt: Symbian error code.

    Errors/Exceptions:  TDesLoggerOverflowHandler called if logged information
                        is over KMaxLogData.

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CStifLogger::WriteDelimiter( const TDesC& aDelimiter, TInt aCount )
    {
    TLogInfo delimiter;

    // Create overflow handler. If the delimiter size expands over the
    // KMaxLogData the TDesLoggerOverflowHandler will call.
    TDesLoggerOverflowHandler overFlowHandler( this, 3 );

    // Create a delimiter
    for( TInt a = 0; a < aCount; a++ )
        {
        // If delimiter creation keeps under the KMaxLogData.
        // If not we use TDesLoggerOverflowHandler.
        if( ( a * aDelimiter.Length() ) < KMaxLogData ) 
            {
            delimiter.Append( aDelimiter );
            }
        // KMaxLogData is exceeded
        else
            {
            // If the title size is over the KMaxLogData default delimiter will
            // use. Use normal overflowhandler to print overflow information.
            TBuf<4> empty; // Not really used.
            overFlowHandler.Overflow( empty );
            delimiter.Copy( 
                _L( "##################################################" ) );
            break;
            }
        }

    // No text style info
    return Send( ENoStyle, delimiter );

    }

/*
-------------------------------------------------------------------------------

    Class: CStifLogger

    Method: WriteDelimiter

    Description: Log a 8 bit delimiter.

    Log a delimiters required locations to the log information.

    Parameters: const TDesC8& aDelimiter: in: Logged delimiter
                                              (e.g. '#' or 'XO')
                TInt aCount: in: Repeated count for delimiter

    Return Values: TInt: Symbian error code.

    Errors/Exceptions:  TDes8LoggerOverflowHandler called if logged information is
                        over KMaxLogData.

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CStifLogger::WriteDelimiter( const TDesC8& aDelimiter, TInt aCount )
    {
    TLogInfo8 delimiter;

    // Create overflow handler. If the delimiter size expands over the
    // KMaxLogData the TDesLoggerOverflowHandler will call.
    TDes8LoggerOverflowHandler overFlowHandler( this, 3 );

    // Create a delimiter
    for( TInt a = 0; a < aCount; a++ )
        {
        // If delimiter creation keeps under the KMaxLogData.
        // If not we use TDesLoggerOverflowHandler.
        if( ( a * aDelimiter.Length() ) < KMaxLogData ) 
            {
            delimiter.Append( aDelimiter );
            }
        // KMaxLogData is exceeded
        else
            {
            // If the title size is over the KMaxLogData default delimiter will
            // use. Use normal overflowhandler to print overflow information.
            TBuf8<4> empty; // Not really used.
            overFlowHandler.Overflow( empty );
            delimiter.Copy( 
                _L8( "##################################################" ) );
            break;
            }
        }

    // No text style info
    return Send( ENoStyle, delimiter );

    }

/*
-------------------------------------------------------------------------------

    Class: CStifLogger

    Method: SaveData

    Description: Save file or data( 16 bit ).

    Used when is need to save file or data to storage e.g. web page.

    Parameters: TDesC& aData: in: Data to be saved
    
    Return Values: TInt: Symbian error code.

    Errors/Exceptions:  None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CStifLogger::SaveData( TDesC& aData )
    {
    // No text style info
    return Send( ENoStyle, aData );

    }

/*
-------------------------------------------------------------------------------

    Class: CStifLogger

    Method: SaveData

    Description: Save file or data( 8 bit ).

    Used when is need to save file or data to storage e.g. web page.

    Parameters: TDesC8& aData: in: Data to be saved
    
    Return Values: TInt: Symbian error code.

    Errors/Exceptions:  None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CStifLogger::SaveData( TDesC8& aData )
    {
    // No text style info
    return Send( ENoStyle, aData );

    }

/*
-------------------------------------------------------------------------------

    Class: CStifLogger

    Method: CreationResult

    Description: Return StifLogger creation result.

    Parameters: None
    
    Return Values: StifLogger creation result

    Errors/Exceptions:  None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CStifLogger::CreationResult()
    {
     
    TOutput outputType;
    return iOutput->OutputCreationResult( outputType );
    
    }
    
/*
-------------------------------------------------------------------------------

    Class: CStifLogger

    Method: OutputType

    Description: Get output type. Valid only if CreationResult returns KErrNone.

    Parameters: TOutput& aOutput
    
    Return Values: StifLogger creation result

    Errors/Exceptions:  None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C CStifLogger::TOutput CStifLogger::OutputType()
    {
    
    TOutput outputType;
    iOutput->OutputCreationResult( outputType );
    return outputType;
    
    }

// End of File
