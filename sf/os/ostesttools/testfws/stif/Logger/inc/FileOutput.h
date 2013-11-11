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
* Description: This file contains the header file of the CFileOutput.
*
*/

#ifndef FILEOUTPUT_H
#define FILEOUTPUT_H

//  INCLUDES
#include <f32file.h>

#include "Output.h"

// CONSTANTS
// None

// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// CLASS DECLARATION

// DESCRIPTION
// CFileOutput is a STIF Test Framework StifLogger class.
// Class contains a file logging operations.
class CFileOutput 
            :public COutput
    {
    public:     // Enumerations

    private:    // Enumerations

    public:     // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CFileOutput* NewL( const TDesC& aTestPath,
                                    const TDesC& aTestFile,
                                    CStifLogger::TLoggerType aLoggerType,
                                    TBool aOverWrite,
                                    TBool aWithTimeStamp,
                                    TBool aWithLineBreak,
                                    TBool aWithEventRanking,
                                    TBool aThreadIdToLogFile,
                                    TBool aCreateLogDir,
                                    TInt  aStaticBufferSize,
                                    TBool aUnicode );

        /**
        * Destructor.
        */
        ~CFileOutput();

    public:     // New functions

        /**
        * Create a directory.
        */
        void CreateDirectoryL( const TDesC& aTestPath );

        /**
        * Open an existing file.
        */
        void OpenExistingFileL( const TDesC& aTestPath,
                                const TDesC& aTestFile );

        /**
        * Create a new file.
        */
        void CreateNewFileL( const TDesC& aTestPath, const TDesC& aTestFile );

        /**
        * Write log information or data to the file.
        * 16 bit.
        */
        TInt Write( TBool aWithTimeStamp,
                    TBool aWithLineBreak,
                    TBool aWithEventRanking,
                    const TDesC& aData );

        /**
        * Write log information or data to the file.
        * 8 bit.
        */
        TInt Write( TBool aWithTimeStamp,
                    TBool aWithLineBreak,
                    TBool aWithEventRanking,
                    const TDesC8& aData );

        /**
        * Add event ranking, specify number to line (1-byte version).
        */
        void EventRanking( TPtr8& aData );

        /**
        * Add event ranking, specify number to line (2-byte version).
        */
        void EventRanking( TPtr& aData );

        /**
        * Add date, time and line break to data (1-byte version).
        */
        void AddTimeStampToData( TPtr8& aData );

        /**
        * Add date, time and line break to data (2-byte version).
        */
        void AddTimeStampToData( TPtr& aData );

        /**
        * Get date and time.
        */
        void GetDateAndTime( TStifLoggerTimeStamp& aDataAndTime );

        /**
        * Make file type.
        */
        void FileType( const TDesC& aTestFile, TFileType& aFileType );

        /**
        * Generate thread id to test file name.
        */
        void TestFileWithThreadId( const TDesC& aTestFile,
                                    TFileName& aNewTestFile );

    public:     // Functions from base classes

    protected:  // New functions

    protected:  // Functions from base classes

    private:

        /**
        * C++ default constructor.
        */
        CFileOutput( CStifLogger::TLoggerType aLoggerType,
                        TBool aWithTimeStamp,
                        TBool aWithLineBreak,
                        TBool aWithEventRanking,
                        TBool aThreadIdToLogFile );

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL( const TDesC& aTestPath,
                            const TDesC& aTestFile,
                            TBool aOverWrite,
                            TBool aCreateLogDir,
                            TInt aStaticBufferSize,
                            TBool aUnicode );

    public:     // Data

    protected:  // Data

    private:    // Data

        /**
        * Log file directory name, file name and file type.
        */
        TFileName iFileAndDirName;

        /**
        * Indicates file type.
        */
        CStifLogger::TLoggerType iLoggerType;

        /**
        * Time stamp indicator.
        */
        TBool iWithTimeStamp;

        /**
        * Line break indicator.
        */
        TBool iWithLineBreak;

        /**
        * Event ranking indicator.
        */
        TBool iWithEventRanking;

        /**
        * Indicator to thread id adding to end of the log file
        */
        TBool iThreadIdToLogFile;

        /**
        * Indicator to thread id adding to end of the log file
        */
        TBool iCreateLogDir;

        /**
        * For the file handling
        */
        RFile iFile;

        /**
        * For the file session handling
        */
        RFs iFileSession;

        /**
        * Is file open indicator
        */
        TInt iIsFileOpen;

       /**
        * DataHBuf for logging purposes
        */
        HBufC8 * iDataHBuf8;

		// @js
        TInt iStaticBufferSize;

       /**
        * Determines if file has to be in unicode format
        */
        TBool iUnicode;

       /**
        * DataHBuf for logging purposes
        */
        HBufC16* iDataHBuf16;

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

    };

#endif      // FILEOUTPUT_H

// End of File
