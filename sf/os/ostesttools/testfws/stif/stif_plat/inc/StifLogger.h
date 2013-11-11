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
* Description: This file contains the header file of the 
* CStifLogger.
*
*/

#ifndef STIFLOGGER_H
#define STIFLOGGER_H

//  INCLUDES
#include <e32base.h>

// If using older STIF TF than STIF200414 there is no this definition.
// If is need to combile e.g. testmodule with older STIF TF there may use
// in the testmodule this flag to indicating which enum definition is used.
// Example:
//          #ifdef __NEW_LOGGER_ENUMS_DEFS__
//              logger->Log( CStifLogger::ERed, _L( "Logged text" ) );
//          #else
//              logger->Log( ERed, _L( "Logged text" ) );
//          #endif
#define __NEW_LOGGER_ENUMS_DEFS__

// Maximum length of the data. Must be over 26 because HTML page initializing.
// Can be only max 254 length because rdebug prints max 256 length text and
// that includes line break(+2 character)
const TInt KMaxLogData = 254;

// This is for making partitions of the rdebug( minus 3 is for ' ->')
const TInt KMaxRDebug = KMaxLogData - 3;

// Maximum count of file type plus dot e.g. ".txt"
const TInt KMaxFileType = 10;

// Maximum length of the date and time logging
// Includes date, time, spaces(5) and end of line(2):
// e.g. "'06.Nov.2003 15:12:18.123    '...logged data...'  '"
const TInt KMaxTimeStamp = 30;

// MACROS
// None

// DATA TYPES

// For 16 bit data size definition.
typedef TBuf<KMaxLogData> TLogInfo;

// For 8 bit data size definition.
typedef TBuf8<KMaxLogData> TLogInfo8;

// For file type size definition.
typedef TBuf<KMaxFileType> TFileType;

// For time stamp size definition.
typedef TBuf8<KMaxTimeStamp> TStifLoggerTimeStamp;

// For time stamp size definition.
typedef TBuf<KMaxTimeStamp> TStifLoggerTimeStamp8;

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
class COutput;

// Structs are defined at the end of this file.
struct TOverwritedSettings;
struct TLoggerSettings;

// CLASS DECLARATION

// DESCRIPTION
// CStifLogger is a STIF Test Framework StifLogger class.
// Class contains a file logging operations.

class CStifLogger 
        :public CBase
    {
    public:     // Enumerations

        // Different StifLogger presentations types.
        enum TLoggerType
            {
            ETxt,       // txt file logging
            EData,      // data file logging(e.g. web page downloading )
            EHtml       // html file logging
            };

        // Different StifLogger output modules.
        enum TOutput
            {
            EFile,      // File logging
            ERDebug     // RDebug logging
            };

        // Different style forms for text.
        enum TStyle
            {
            ENoStyle    = 0x00000,  // No style parameter
            EBold       = 0x00001,  // Bold
            ECursive    = 0x00002,  // Cursive
            EUnderline  = 0x00004,  // Underline
            EBlue       = 0x00008,  // Blue
            EGreen      = 0x00010,  // Green
            ERed        = 0x00020,  // Red
            EImportant  = 0x00040,  // Important (Line will starts a 'IMPORTANT' text)
            EWarning    = 0x00080,  // Warning (Line will starts a 'WARNING' text)
            EError      = 0x00100   // Error (Line will starts a 'ERROR' text)
            };

    private:    // Enumerations

    public:     // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CStifLogger* NewL( const TDesC& aTestPath,
                                            const TDesC& aTestFile,
                                            // Default logger type is .txt
                                            TLoggerType aLoggerType = ETxt,
                                            // By default output is file
                                            TOutput aOutput = EFile,
                                            // By default overwrite is used
                                            TBool aOverWrite = ETrue,
                                            // By default time stamp is used
                                            TBool aWithTimeStamp = ETrue,
                                            // By default line break is used
                                            TBool aWithLineBreak = ETrue,
                                            // By default even ranking is not
                                            // used
                                            TBool aWithEventRanking = EFalse,
                                            // By default thread id is added
                                            // after the log file
                                            TBool aThreadIdToLogFile = ETrue,
                                            // By default log directory will
                                            // not create
                                            TBool aCreateLogDir = EFalse,
                                            // Static logger implementation is
                                            // not yet permanent, may change 
                                            TInt aStaticBufferSize = 0,
                                            // By default files are not written
                                            // in unicode format
                                            TBool aUnicode = EFalse );
        /**
        * Destructor.
        */
        virtual ~CStifLogger();

    public:     // New functions

        /**
        * C++ default constructor.
        */
        CStifLogger();

        /**
        * Log a 16 bit information to some storage e.g. text file.
        */
        IMPORT_C TInt Log( const TDesC& aLogInfo );

        /**
        * Log a 8 bit information to some storage e.g. text file.
        */
        IMPORT_C TInt Log( const TDesC8& aLogInfo );

        /**
        * Log a 16 bit information to some storage e.g. text file. There is
        * also parameter to styling text information e.g. text color.
        */
        IMPORT_C TInt Log( TInt aStyle, const TDesC& aLogInfo );

        /**
        * Log a 8 bit information to some storage e.g. text file. There is
        * also parameter to styling text information e.g. text color.
        */
        IMPORT_C TInt Log( TInt aStyle, const TDesC8& aLogInfo );

        /**
        * Log a 16 bit information to some storage e.g. text file. In this log
        * method may set several parameters.
        */
        IMPORT_C TInt Log( TRefByValue<const TDesC> aLogInfo,... );

        /**
        * Log a 8 bit information to some storage e.g. text file. In this log
        * method may set several parameters.
        */
        IMPORT_C TInt Log( TRefByValue<const TDesC8> aLogInfo,... );

        /**
        * Log a 16 bit information to some storage e.g. text file. There is
        * also parameter to styling text information e.g. text color.
        */
        IMPORT_C TInt Log( TInt aStyle,
            TRefByValue<const TDesC> aLogInfo,... );

        /**
        * Log a 8 bit information to some storage e.g. text file. There is
        * also parameter to styling text information e.g. text color.
        */
        IMPORT_C TInt Log( TInt aStyle,
            TRefByValue<const TDesC8> aLogInfo,... );

        /**
        * Log a 16 bit delimiter. Default char is '#' and default
        * repeat count is 60.
        * This will be used if parameters are not given when calling
        * this method.
        */
        IMPORT_C TInt WriteDelimiter( const TDesC& aDelimiter = _L( "#"),
                                        TInt aCount = 60 );

        /**
        * Log a 8 bit delimiter. 8 bit delimiter includes no default values.
        * Delimiter may be '#' or 'XO' etc.
        */
        IMPORT_C TInt WriteDelimiter( const TDesC8& aDelimiter, TInt aCount );

        /**
        * Used when is need to save file or data to storage e.g. web page.
        * 16 bit.
        */
        IMPORT_C TInt SaveData( TDesC& aData );

        /**
        * Used when is need to save file or data to storage e.g. web page.
        * 8 bit.
        */
        IMPORT_C TInt SaveData( TDesC8& aData );
        
        /**
        * Get creation result.
        */
        IMPORT_C TInt CreationResult();

        /**
        * Get output type. Valid only if CreationResult returns KErrNone.
        */
        IMPORT_C TOutput OutputType();

        /**
        * Pure virtual. Send style information and data to the output module.
        * 16 bit.
        */
        virtual TInt Send( TInt /*aStyle*/, const TDesC& /*aData*/ ) = 0;

        /**
        * Pure virtual. Send style information and data to the output module.
        * 8 bit.
        */
        virtual TInt Send( TInt /*aStyle*/, const TDesC8& /*aData*/ ) = 0;

        /**
        * Log 16 bit _LIT information to some storage e.g. text file.
        */
        template<TInt S> TInt Log( const TLitC<S>& aLogInfo )
            { 
            return Send( ENoStyle, (TDesC&)aLogInfo );
            };

        /**
        * Log 8 bit _LIT information to some storage e.g. text file.
        */
        template<TInt S> TInt Log( const TLitC8<S>& aLogInfo )
            { 
            return Send( ENoStyle, (TDesC8&)aLogInfo );
            };

        /**
        * Log 16 bit _LIT information to some storage e.g. text file. There is
        * also parameter to styling text information e.g. text color.
        */
        template<TInt S> TInt Log( TInt aStyle, const TLitC<S>& aLogInfo )
            { 
            return Send( aStyle, (TDesC&)aLogInfo );
            };

        /**
        * Log 8 bit _LIT information to some storage e.g. text file.  There is
        * also parameter to styling text information e.g. text color.
        */
        template<TInt S> TInt Log( TInt aStyle, const TLitC8<S>& aLogInfo )
            { 
            return Send( aStyle, (TDesC8&)aLogInfo );
            };

    public:     // Functions from base classes

    protected:  // New functions

        /**
        * Two-phased constructor. TestEngine's and TestServer's are friend.
        * For TestEngine's and TestServer's StifLogger creation.
        */
        IMPORT_C static CStifLogger* NewL( const TDesC& aTestPath,
                                            const TDesC& aTestFile,
                                            TLoggerSettings& aLoggerSettings );

    protected:  // Functions from base classes

        /**
        * Local CStifLogger's method for StifLogger settings overwriting.
        */
        static void OverwriteLoggerSettings( TLoggerSettings& aLoggerSettings,
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
                                                TBool& aUnicode );

    private:

    public:     // Data

    protected:  // Data
        /**
        * Output object.
        */
        COutput*    iOutput;
        
    private:    // Data

    public:     // Friend classes
        
    protected:  // Friend classes

    private:    // Friend classes

        // TestEngine classes
        friend class CTestEngineServer;
        friend class CTestReport;
        friend class CTestCaseController;
        friend class CTestEngine;

        // TestServer classes
        friend class CTestModuleContainer;
        friend class CTestServer;
        friend class CTestThreadContainer;
        friend class CTestThreadContainerRunner;

        // UIEngine classes
        friend class CUIEngine;

        // SettingServer classes
        friend class CSettingServer;

    };

// Struct to indicate StifLogger's settings overwriting
struct TOverwritedSettings
    {
    TBool               iCreateLogDir;
    TBool               iPath;
    TBool               iHwPath;
    TBool               iFormat;
    TBool               iHwFormat;
    TBool               iOutput;
    TBool               iHwOutput;
    TBool               iOverwrite;
    TBool               iTimeStamp;
    TBool               iLineBreak;
    TBool               iEventRanking;
    TBool               iThreadId;
    TBool               iUnicode;
    TBool               iAddTestCaseTitle;
    };

// Struct to StifLogger overwrite settings
struct TLoggerSettings
    {
    TBool                       iCreateLogDirectories;
    TName                       iEmulatorPath;
    CStifLogger::TLoggerType    iEmulatorFormat;
    CStifLogger::TOutput        iEmulatorOutput;
    TName                       iHardwarePath;
    CStifLogger::TLoggerType    iHardwareFormat;
    CStifLogger::TOutput        iHardwareOutput;
    TBool                       iOverwrite;
    TBool                       iTimeStamp;
    TBool                       iLineBreak;
    TBool                       iEventRanking;
    TBool                       iThreadId;
    TBool                       iUnicode;
    TBool                       iAddTestCaseTitle;
    TOverwritedSettings         iIsDefined;
    };

#endif      // STIFLOGGER_H

// End of File
