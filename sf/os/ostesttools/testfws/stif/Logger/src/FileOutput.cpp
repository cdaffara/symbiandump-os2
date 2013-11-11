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
* Description: This module contains implementation of CFileOutput 
* class member functions.
*
*/


// INCLUDE FILES
#include "FileOutput.h"

// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES  
// None

// CONSTANTS
// None

// MACROS
// None
//#define RDEBUG_FILEWRITECRASH(a) RDebug::Print(a)
#define RDEBUG_FILEWRITECRASH(a)

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

    Class: CFileOutput

    Method: CFileOutput

    Description: Default constructor

    C++ default constructor can NOT contain any code, that
    might leave.

    Parameters: TLoggerType aLoggerType: in: File type
                TBool aWithTimeStamp: in: For timestamp
                TBool aWithLineBreak: in: For line break
                TBool aWithEventRanking: in: For events ranking to file
                TBool aThreadIdToLogFile: in: Indicator to thread id adding to
                                              end of the log file

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CFileOutput::CFileOutput( CStifLogger::TLoggerType aLoggerType,
                            TBool aWithTimeStamp,
                            TBool aWithLineBreak,
                            TBool aWithEventRanking,
                            TBool aThreadIdToLogFile ) :
    COutput()
    {
    // Indicates file type
    iLoggerType = aLoggerType;

    // Time stamp indicator
    iWithTimeStamp = aWithTimeStamp;

    // Line break indicator
    iWithLineBreak = aWithLineBreak;

    // Log event ranking indicator
    iWithEventRanking = aWithEventRanking;

    // Indicator to thread id adding to end of the log file
    iThreadIdToLogFile = aThreadIdToLogFile;

    // Is file open indicator
    iIsFileOpen = 0;

    }

/*
-------------------------------------------------------------------------------

    Class: CFileOutput

    Method: ConstructL

    Description: Symbian OS second phase constructor

    Symbian OS default constructor can leave.

    Parameters: const TDesC& aTestPath: in: Log path
                const TDesC& aTestFile: in: Log filename
                TBool aOverWrite: in: For file overwrite
                TBool aCreateLogDir: in: Indicator to directory creation
                TInt  aStaticBufferSize
                TBool aUnicode: in: Indicator if file has to be in unicode format

    Return Values: None

    Errors/Exceptions:  Leaves if called CreateNewFileL method fails
                        Leaves if called OpenExistingFileL method fails

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CFileOutput::ConstructL( const TDesC& aTestPath,
                                const TDesC& aTestFile,
                                TBool aOverWrite,
                                TBool aCreateLogDir,
                                TInt aStaticBufferSize,
                                TBool aUnicode)
    {
    iUnicode = aUnicode;

    // Open file session
    User::LeaveIfError( iFileSession.Connect() );

    // Create a log directory
    if( aCreateLogDir )
        {
        CreateDirectoryL( aTestPath );
        }

    // Create a new file
    if ( aOverWrite )
        {
        CreateNewFileL( aTestPath, aTestFile );
        }
    // Open an existing file
    else 
        {
        OpenExistingFileL( aTestPath, aTestFile );
        }
        
    if( aStaticBufferSize != 0)
        {
        iDataHBuf8 = HBufC8::NewL( aStaticBufferSize );
        iDataHBuf16 = HBufC::NewL(aStaticBufferSize);
        iStaticBufferSize = aStaticBufferSize;
        }
    }

/*
-------------------------------------------------------------------------------

    Class: CFileOutput

    Method: NewL

    Description: Two-phased constructor.

    Parameters: const TDesC& aTestPath: in: Log path
                const TDesC& aTestFile: in: Log filename
                TLoggerType aLoggerType: in: File type
                TBool aOverWrite: in: For file overwrite
                TBool aWithTimeStamp: in: For timestamp
                TBool aWithLineBreak: in: For line break
                TBool aWithEventRanking: in: For events ranking to file
                TBool aThreadIdToLogFile: in: Indicator to thread id adding to
                                              end of the log file
                TBool aCreateLogDir: in: Indicator to directory creation
                TInt  aStaticBufferSize
                TBool aUnicode: in: Indicator if file has to be in unicode format

    Return Values: CFileOutput*: pointer to CFileOutput object

    Errors/Exceptions: Leaves if ConstructL leaves

    Status: Proposal

-------------------------------------------------------------------------------
*/
CFileOutput* CFileOutput::NewL( const TDesC& aTestPath,
                                const TDesC& aTestFile,
                                CStifLogger::TLoggerType aLoggerType,
                                TBool aOverWrite,
                                TBool aWithTimeStamp,
                                TBool aWithLineBreak,
                                TBool aWithEventRanking,
                                TBool aThreadIdToLogFile,
                                TBool aCreateLogDir,
                                TInt  aStaticBufferSize,
                                TBool aUnicode )
    {
    // Create CFileOutput object fileWriter
    CFileOutput* fileWriter = new (ELeave) CFileOutput( aLoggerType,
                                                        aWithTimeStamp,
                                                        aWithLineBreak,
                                                        aWithEventRanking,
                                                        aThreadIdToLogFile );

    CleanupStack::PushL( fileWriter );
    fileWriter->ConstructL( aTestPath, aTestFile, aOverWrite, aCreateLogDir, aStaticBufferSize,
                            aUnicode );
    CleanupStack::Pop( fileWriter );

    return fileWriter;

    }

/*
-------------------------------------------------------------------------------

    Class: CFileOutput

    Method: ~CFileOutput

    Description: Destructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CFileOutput::~CFileOutput()
    {
    // Close file
    iFile.Close();
    // Close file session
    iFileSession.Close();

    delete iDataHBuf8;
    delete iDataHBuf16;
    }

/*
-------------------------------------------------------------------------------

    Class: CFileOutput

    Method: FileType

    Description: Check file type.

    Check is file type set with a aTestFile's name. If file type is not set
    the type will set according to StifLogger's type.

    Parameters: const TDesC& aTestFile: in: Log filename
                TFileType& fileType: inout: Generated file type

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/    
void CFileOutput::FileType( const TDesC& aTestFile,
                            TFileType& aFileType )
    {
    TBool fileTypeIsSet( EFalse ); // File type indicator

    // Check is file type set to aTestFile parameter
    TInt ret = aTestFile.Find( _L( "." ) );
    if( ret != KErrNotFound )
        {
        fileTypeIsSet = ETrue;
        }
    // If not, add thread id name if allowed
    else if ( iThreadIdToLogFile )
        {
        // Thread id maximum length is 8 in hexadesimal format
        // Construct unique thread name
        aFileType.Append( _L("_") );
        RThread threadHandle;                   // A handle to a thread
        TThreadId id = threadHandle.Id();       // Encapsulates the 
                                                // Id of a thread
        // Appends id in hexadesimal format             
        aFileType.AppendFormat( _L( "%x" ), (TInt)id );
        }

    // Add file type after the thread id name
    if ( iLoggerType == CStifLogger::ETxt && !fileTypeIsSet )
        {
        // ".txt"
        aFileType.Append( _L( "." ) );
        aFileType.Append( _L( "txt" ) );
        }
    else if ( iLoggerType == CStifLogger::EHtml && !fileTypeIsSet )
        {
        // ".html"
        aFileType.Append( _L( "." ) );
        aFileType.Append( _L( "html" ) );
        }
    // EData file type comes from the aTestFile name. If not the file type 
    // will be empty
    else if ( iLoggerType == CStifLogger::EData )
        {
        aFileType.Append( _L( "" ) );
        }
    else
        {
        aFileType.Append( _L( "" ) );
        }

    }

/*
-------------------------------------------------------------------------------

    Class: CFileOutput

    Method: TestFileWithThreadId

    Description: Generate thread id

    Generate thread id name between the test file name and test file type.

    Parameters: TDesC& aTestFile: in: Test file name
                TFileName& aNewTestFile: inout: Generated test file name 

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CFileOutput::TestFileWithThreadId( const TDesC& aTestFile, 
                                        TFileName& aNewTestFile )
    {
    for( TInt a = 0; a < aTestFile.Length(); a++ )
        {
        // Find a dot
        if( aTestFile[a] == '.' )
            {
            TPtrC parsedFileType = aTestFile.Mid( a );  // Take rest of the 
                                                        // aTestFile
            aNewTestFile.Append( _L( "_" ) );
            RThread threadHandle;                       // A handle to a thread
            TThreadId id = threadHandle.Id();           // Encapsulates the 
                                                        // Id of a thread
            aNewTestFile.AppendFormat( _L( "%x" ), (TInt)id );// Appends id in
                                                        // hexadesimal format
            aNewTestFile.Append( parsedFileType );
            break;
            }
        // Dot not found yet
        else
            {
            aNewTestFile.Append( aTestFile[a] );
            }
        }

    }

/*
-------------------------------------------------------------------------------

    Class: CFileOutput

    Method: CreateDirectoryL

    Description: Create a log directory.

    Method creates directory if not allready exist.

    Parameters: const TDesC& aTestPath: in: Test path definition

    Return Values: None

    Errors/Exceptions: Leaves if directory creation fails

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CFileOutput::CreateDirectoryL( const TDesC& aTestPath )
    {
    __TRACE( KInfo, ( _L( "STIFLOGGER: Create a directory" ) ) );

    TInt ret = iFileSession.MkDirAll( aTestPath );
	if( ret != KErrNone && ret != KErrAlreadyExists )
        {
        __TRACE( KError, 
            ( _L( "STIFLOGGER: Directory creation fails with error: %d" ), ret ) );
		User::Leave( ret );
        }

    }

/*
-------------------------------------------------------------------------------

    Class: CFileOutput

    Method: OpenExistingFileL

    Description: Open an existing file.

    Method is used when is need for log new information after the existing
    log file.

    Parameters: const TDesC& aTestPath: in: Test path definition
                const TDesC& aTestFile: in: Test file name

    Return Values: None

    Errors/Exceptions:  Leaves if path or file lengths are over KMaxFileName
                        Leaves if file Replace method fails
                        Leaves if file Open method fails
                        Leaves if file path not found

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CFileOutput::OpenExistingFileL( const TDesC& aTestPath, 
                                        const TDesC& aTestFile )
    {
    __TRACE( KInfo, ( _L( "STIFLOGGER: Opening an existing file for logging" ) ) );

    // If path and file name lengths are over KMaxFileName(TFileName)
    __ASSERT_ALWAYS(
        ( aTestPath.Length() + aTestFile.Length() ) < KMaxFileName,
        User::Leave( KErrArgument ) );

    // Check file type and generate threat id name
    TFileType fileType;
    FileType( aTestFile, fileType );

    // File directory name, file name and file type definitions
    TPtrC dirNamePrt( aTestPath );
    iFileAndDirName.Copy( dirNamePrt );

    TInt isDotFound = aTestFile.Find( _L( "." ) );
    // If dot is found and iThreadIdToLogFile is true
    if( isDotFound != KErrNotFound && iThreadIdToLogFile )
        {
        TFileName testFileWithThreadId;
        // Generate thread id name
        TestFileWithThreadId( aTestFile, testFileWithThreadId );
        iFileAndDirName.Insert( 
            iFileAndDirName.Length(), testFileWithThreadId );
        }
    else
        {
        TPtrC txtPrt( fileType );
        iFileAndDirName.Insert( iFileAndDirName.Length(), aTestFile );
        iFileAndDirName.Insert( iFileAndDirName.Length(), txtPrt );
        }

//  TBool isOpen( EFalse );
    TInt ret( KErrNone );

  
    iIsFileOpen=iFile.Open( iFileSession, iFileAndDirName,
            EFileWrite | EFileStreamText |
                    EFileShareAny );
    if( iIsFileOpen == KErrNotFound )
        {
        ret =  iFile.Create( iFileSession, iFileAndDirName, 
                                    EFileWrite | EFileStreamText |
                                    EFileShareAny );
        if(iUnicode && ret == KErrNone)
            {
            char FF = 0xFF;
            char FE = 0xFE;
            TBuf8<2> beg;
            beg.Append(FF);
            beg.Append(FE);
            if(iFile.Write(beg) == KErrNone)
                iFile.Flush();
            }
        }
    else if( iIsFileOpen == KErrNone )
        {
      
        ret=KErrNone;
        }
    // Probably path not found
    else
        {
        User::Leave( iIsFileOpen );
        }

    if ( ret != KErrNone )
        {
        User::Leave( ret );
        }

    }

/*
-------------------------------------------------------------------------------

    Class: CFileOutput

    Method: CreateNewFileL

    Description: Create a new file.

    Method creates new file to the log information.

    Parameters: const TDesC& aTestPath: in: Test path definition
                const TDesC& aTestFile: in: Test file name

    Return Values: None

    Errors/Exceptions:  Leaves if path or file lengths are over KMaxFileName
                        Leaves if file server Connect method fails
                        Leaves if file Replace method fails
                        Leaves if file Open method fails
                        Leaves if file path not found

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CFileOutput::CreateNewFileL( const TDesC& aTestPath,
                                    const TDesC& aTestFile )
    {
    __TRACE( KInfo, ( _L( "STIFLOGGER: Create a new file for logging" ) ) );

    // If path and file name lengths are over KMaxFileName(TFileName)
    __ASSERT_ALWAYS(
        ( aTestPath.Length() + aTestFile.Length() ) < KMaxFileName,
        User::Leave( KErrArgument ) );

    // Check file type and generate threat id name
    TFileType fileType;
    FileType( aTestFile, fileType );

    // File directory name, file name and file type definitions
    TPtrC dirNamePrt( aTestPath );
    iFileAndDirName.Copy( dirNamePrt );

    TInt isDotFound = aTestFile.Find( _L( "." ) );
    // If dot is found and iThreadIdToLogFile is true
    if( isDotFound != KErrNotFound && iThreadIdToLogFile )
        {
        TFileName testFileWithThreadId;
        // Generate thread id name
        TestFileWithThreadId( aTestFile, testFileWithThreadId );
        iFileAndDirName.Insert( 
            iFileAndDirName.Length(), testFileWithThreadId );
        }
    else
        {
        TPtrC txtPrt( fileType );
        iFileAndDirName.Insert( iFileAndDirName.Length(), aTestFile );
        iFileAndDirName.Insert( iFileAndDirName.Length(), txtPrt );
        }

    // Delete file if exist
    iFileSession.Delete( iFileAndDirName );

    TBool isOpen( EFalse );
    TInt ret( KErrNone );

    iIsFileOpen = iFileSession.IsFileOpen( iFileAndDirName, isOpen );
    if( ( iIsFileOpen == KErrNotFound ) ||
        ( iIsFileOpen == KErrNone ) )
        {
        ret = iFile.Replace( iFileSession, iFileAndDirName,
                                    EFileWrite | EFileStreamText |
                                    EFileShareAny );
        if(iUnicode && ret == KErrNone)
            {
            char FF = 0xFF;
            char FE = 0xFE;
            TBuf8<2> beg;
            beg.Append(FF);
            beg.Append(FE);
            if(iFile.Write(beg) == KErrNone)
                iFile.Flush();
            }
        }
    // Probably path not found
    else
        {
        User::Leave( iIsFileOpen );
        }

    if ( ret != KErrNone )
        {
        User::Leave( ret );
        }

    }

/*
-------------------------------------------------------------------------------

    Class: CFileOutput

    Method: Write

    Description: Write log information or data to the file. 16 bit.

    Parameters: TBool aWithTimeStamp: in: Is time stamp flag used
                TBool aWithLineBreak: in: Is line break flag used
                TBool aWithEventRanking: in: Is event ranking flag used
                const TDesC& aData: in: Logged or saved data

    Return Values: TInt: Symbian error code.

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CFileOutput::Write( TBool aWithTimeStamp, TBool aWithLineBreak,
                            TBool aWithEventRanking, const TDesC& aData )
    {
    TInt extraLength( 0 );  // Line and/or event required length

    TPtr data16(0, 0);
    TPtr8 data8(0, 0);

    // Extra space calculation
    if( iWithEventRanking && aWithEventRanking )
        {
        extraLength = extraLength + KMaxEventRanking + KMaxSpace;
        }
    if( ( iLoggerType == CStifLogger::EHtml ) && iWithLineBreak && aWithLineBreak )
        {
        extraLength = extraLength + KMaxHtmlLineBreak;
        }
    if( iWithLineBreak && aWithLineBreak )
        {
        extraLength = extraLength + KMaxLineBreak;
        }
    if( aWithTimeStamp && iWithTimeStamp )
        {
        extraLength = extraLength + KMaxTimeStamp;
        }

    // Calculated space
    TInt space = aData.Length();

    HBufC8* dataHBuf8 = NULL;
    HBufC16* dataHBuf16 = NULL;

    if( iDataHBuf8 != NULL )
        {
        // We'll use HBufC created in ConstructL
        data8.Set(iDataHBuf8->Des());
        data16.Set(iDataHBuf16->Des());
        data16.Copy(_L("")); // Initialize aData buffer
        
        if( (aData.Length() + extraLength) > iStaticBufferSize )
            {
             RDebug::Print(_L("Warning: buffer size too small, not able to log!"));
             return KErrOverflow;
            }        
        }
    else
        {        
        // We'll create a local HBufC, deleting it in the end of this method
        dataHBuf8 = HBufC8::New( space + extraLength );
        if( dataHBuf8 == NULL )
            {
            return KErrNoMemory;
            }
        dataHBuf16 = HBufC16::New(space + extraLength);
        if(dataHBuf16 == NULL)
            {
            delete dataHBuf8;
            return KErrNoMemory;
            }
        data8.Set(dataHBuf8->Des());
        data16.Set(dataHBuf16->Des());
        }

    // Event ranking
    if( iWithEventRanking && aWithEventRanking && !iWithTimeStamp )
        {
        EventRanking( data16 );      // Event ranking to data
        data16.Append( aData );      // Unicode aData to normal text
        }
    // Time stamp
    else if( iWithTimeStamp && aWithTimeStamp )
        {
        // With event ranking
        if( iWithEventRanking && aWithEventRanking )
            {
            EventRanking( data16 );  // Event ranking to data
            }
        AddTimeStampToData( data16 );// Add time stamp
        data16.Append( aData );      // Unicode aData to normal text
        }
    else
        {
        data16.Copy( aData );        // Unicode aData to normal text
        }

    // NOTE: If need some special line break do it with logging phase

    // If html logging and line break is used add the line break.
    if (( iLoggerType == CStifLogger::EHtml ) && iWithLineBreak && aWithLineBreak )
        {
        data16.Append( _L( "<BR>" ) );
        data16.Append( _L( "\n" ) );  // To make html file more readable
                                    // with text editor
        }

    // Other cases line break is normal '\n' if logging is used
    else if( iWithLineBreak && aWithLineBreak )
        {
        data16.Append( 0x0D ); // 13 or '\' in Symbian OS
        data16.Append( 0x0A ); // 10 or 'n' in Symbian OS
        }

    // Write the data to file
    TInt tmp = 0;
    if( iFile.Seek( ESeekEnd, tmp ) == KErrNone )
        {
        RDEBUG_FILEWRITECRASH(_L("iFile.Write 1 in"));
        TPtrC8 dataToFile(0, 0);
        if(iUnicode)
            {
            dataToFile.Set((TUint8 *)(data16.Ptr()), data16.Length() * 2);
            }
        else
            {
            data8.Copy(data16);
            dataToFile.Set(data8);
            }
        if(iFile.Write(dataToFile) == KErrNone)
            iFile.Flush();
        RDEBUG_FILEWRITECRASH(_L("iFile.Write 1 out"));
        }
    if( iDataHBuf8 == NULL )
        {
        delete dataHBuf8;
        dataHBuf8 = 0;
        delete dataHBuf16;
        dataHBuf16 = 0;
        }

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CFileOutput

    Method: Write

    Description: Write log information or data to the file. 8 bit.

    Parameters: TBool aWithTimeStamp: in: Is time stamp flag used
                TBool aWithLineBreak: in: Is line break flag used
                TBool aWithEventRanking: in: Is event ranking flag used
                const TDesC8& aData: in: Logged or saved data

    Return Values: TInt: Symbian error code.

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CFileOutput::Write( TBool aWithTimeStamp, TBool aWithLineBreak,
                            TBool aWithEventRanking, const TDesC8& aData )
    {
    TInt extraLength( 0 );  // Line and/or event required length

    TPtr data16(0, 0);
    TPtr8 data8(0, 0);

    // Extra space calculation
    if( iWithEventRanking && aWithEventRanking )
        {
        extraLength = extraLength + KMaxEventRanking + KMaxSpace;
        }
    if( ( iLoggerType == CStifLogger::EHtml ) && iWithLineBreak && aWithLineBreak )
        {
        extraLength = extraLength + KMaxHtmlLineBreak;
        }
    if( iWithLineBreak && aWithLineBreak )
        {
        extraLength = extraLength + KMaxLineBreak;
        }
    if( aWithTimeStamp && iWithTimeStamp )
        {
        extraLength = extraLength + KMaxTimeStamp;
        }

    // aData straight to the file
    if ( extraLength == 0 && !iUnicode)
        {
        // Write the data to file
        TInt tmp = 0;
        if( iFile.Seek( ESeekEnd, tmp ) == KErrNone )
            {
            RDEBUG_FILEWRITECRASH(_L("iFile.Write 2 in"));
            if( iFile.Write( aData ) == KErrNone )
                iFile.Flush();
            RDEBUG_FILEWRITECRASH(_L("iFile.Write 2 out"));
            }
        return KErrNone;
        }

    // Calculated space
    TInt space = aData.Length();
    HBufC8* dataHBuf8 = NULL;
    HBufC* dataHBuf16 = NULL;

    if( iDataHBuf8 != NULL )
        {
        // We'll use HBufC created in ConstructL
        data8.Set(iDataHBuf8->Des());
        data8.Copy(_L("")); // Initialize aData buffer
        data16.Set(iDataHBuf16->Des());
        
        if( (aData.Length() + extraLength) > iStaticBufferSize )
            {
             RDebug::Print(_L("Warning: buffer size too small, not able to log!"));
             return KErrOverflow;
            }        
        }
    else
        {        
        // We'll create a local HBufC, deleting it in the end of this method
        dataHBuf8 = HBufC8::New( space + extraLength );
        if( dataHBuf8 == NULL )
            {
            return KErrNoMemory;
            }
        dataHBuf16 = HBufC16::New(space + extraLength);
        if(dataHBuf16 == NULL)
            {
            delete dataHBuf8;
            return KErrNoMemory;
            }
        data8.Set(dataHBuf8->Des());  // Memory allocation for data
        data16.Set(dataHBuf16->Des());  // Memory allocation for data
        }

    // Event ranking
    if( iWithEventRanking && aWithEventRanking && !iWithTimeStamp )
        {
        EventRanking( data8 );      // Event ranking to data
        data8.Append( aData );      // Unicode aData to normal text
        }
    // Time stamp
    else if( iWithTimeStamp && aWithTimeStamp )
        {
        // With event ranking
        if( iWithEventRanking && aWithEventRanking )
            {
            EventRanking( data8 );  // Event ranking to data
            }
        AddTimeStampToData( data8 );// Add time stamp
        data8.Append( aData );      // Unicode aData to normal text
        }
    else
        {
        data8.Copy( aData );        // Unicode aData to normal text
        }

    // NOTE: If need some special line break do it with logging phase

    // If html logging and line break is used add the line break.
    if ( ( iLoggerType == CStifLogger::EHtml ) && iWithLineBreak && aWithLineBreak )
        {
        data8.Append( _L( "<BR>" ) );
        data8.Append( _L( "\n" ) );  // To make html file more readable
                                    // with text editor
        }

    // Other cases line break is normal '\n' if logging is used
    else if( iWithLineBreak && aWithLineBreak )
        {
        data8.Append( 0x0D ); // 13 or '\' in Symbian OS
        data8.Append( 0x0A ); // 10 or 'n' in Symbian OS
        }

    // Write the data to file
    TInt tmp = 0;
    if( iFile.Seek( ESeekEnd, tmp ) == KErrNone )
        {
        RDEBUG_FILEWRITECRASH(_L("iFile.Write 3 in"));
        TPtrC8 dataToFile(0, 0);
        if(iUnicode)
            {
            data16.Copy(data8);
            dataToFile.Set((TUint8 *)(data16.Ptr()), data16.Length() * 2);
            }
        else
            {
            dataToFile.Set(data8);
            }
        if(iFile.Write(dataToFile) == KErrNone)
            iFile.Flush();
        RDEBUG_FILEWRITECRASH(_L("iFile.Write 3 out"));
        }
    if( iDataHBuf8 == NULL )
        {
        delete dataHBuf8;
        dataHBuf8 = 0;
        delete dataHBuf16;
        dataHBuf16 = 0;
        }

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CFileOutput

    Method: EventRanking

    Description: For event ranking logging.

    Parameters: TPtr& aData: inout: Data with event ranking

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CFileOutput::EventRanking( TPtr& aData )
    {
    // Returns the machine's current tick count.
    TUint tick = User::TickCount();
    tick &= 0xffff;
    aData.AppendNum( tick );
    aData.Append( _L( " " ) );
    }

/*
-------------------------------------------------------------------------------

    Class: CFileOutput

    Method: EventRanking

    Description: For event ranking logging.

    Parameters: TPtr8& aData: inout: Data with event ranking

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CFileOutput::EventRanking( TPtr8& aData )
    {
    // Returns the machine's current tick count.
    TUint tick = User::TickCount();
    tick &= 0xffff;
    aData.AppendNum( tick );
    aData.Append( _L8( " " ) );

    }

/*
-------------------------------------------------------------------------------

    Class: CFileOutput

    Method: AddTimeStampToData

    Description: For date and time logging.

    Parameters: TPtr& aData: inout: Data with time stamps

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CFileOutput::AddTimeStampToData( TPtr& aData )
    {
    TStifLoggerTimeStamp timeStamp;
    GetDateAndTime( timeStamp );

    TStifLoggerTimeStamp8 tStamp16;
    tStamp16.Copy(timeStamp);

    aData.Append( tStamp16 );
    }

/*
-------------------------------------------------------------------------------

    Class: CFileOutput

    Method: AddTimeStampToData

    Description: For date and time logging.

    Parameters: TPtr8& aData: inout: Data with time stamps

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CFileOutput::AddTimeStampToData( TPtr8& aData )
    {
    TStifLoggerTimeStamp timeStamp;
    GetDateAndTime( timeStamp );
    aData.Append( timeStamp );

    }

/*
-------------------------------------------------------------------------------

    Class: CFileOutput

    Method: GetDateAndTime

    Description: For date and time logging.

    Parameters: TStifLoggerTimeStamp& aDataAndTime: inout: Generated date and time

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CFileOutput::GetDateAndTime( TStifLoggerTimeStamp& aDataAndTime )
    {
    TInt num( 0 );
    TInt microseconds( 0 );

    // Append date
    TTime time;
    time.HomeTime();
    TDateTime dateTime( time.DateTime() );

    num = dateTime.Day() + 1;
    if( num < 10 )
        {
        aDataAndTime.Append( '0' );
        }
    // Append month
    aDataAndTime.AppendNum( num );
    aDataAndTime.Append( '.' );
    num = 0;
    num = dateTime.Month();
    switch( num )
        {
        case EJanuary:      // 1
            {
             aDataAndTime.Append( _L( "Jan" ) );
             break;
            }
        case EFebruary:     // 2
            {
             aDataAndTime.Append( _L( "Feb" ) );
             break;
            }
        case EMarch:        // 3
            {
             aDataAndTime.Append( _L( "Mar" ) );
             break;
            }
        case EApril:        // 4
            {
             aDataAndTime.Append( _L( "Apr" ) );
             break;
            }
        case EMay:          // 5
            {
             aDataAndTime.Append( _L( "May" ) );
             break;
            }
        case EJune:         // 6
            {
             aDataAndTime.Append( _L( "Jun" ) );
             break;
            }
        case EJuly:         // 7
            {
             aDataAndTime.Append( _L( "Jul" ) );
             break;
            }
        case EAugust:       // 8
            {
             aDataAndTime.Append( _L( "Aug" ) );
             break;
            }
        case ESeptember:    // 9
            {
             aDataAndTime.Append( _L( "Sep" ) );
             break;
            }
        case EOctober:      // 10
            {
             aDataAndTime.Append( _L( "Oct" ) );
             break;
            }
        case ENovember:     // 11
            {
             aDataAndTime.Append( _L( "Nov" ) );
             break;
            }
        case EDecember:     // 12
            {
             aDataAndTime.Append( _L( "Dec" ) );
             break;
            }
        default:
            {
             aDataAndTime.Append( '-' );
             break;
            }
        }
   // Append year
    aDataAndTime.Append( '.' );
    aDataAndTime.AppendNum( dateTime.Year() );

    // Append time
    aDataAndTime.Append( ' ' );
    num = dateTime.Hour();
    if( num < 10 )
        {
        aDataAndTime.Append( '0' );
        }
    aDataAndTime.AppendNum( num );
    aDataAndTime.Append( ':' );
    num = dateTime.Minute();
    if( num < 10 )
        {
        aDataAndTime.Append( '0' );
        }
    aDataAndTime.AppendNum( num );
    aDataAndTime.Append( ':' );

    num = dateTime.Second();                // Seconds
    microseconds = dateTime.MicroSecond();  // Microseconds

    // Seconds
    if( num < 10 )
        {
        aDataAndTime.Append( '0' );
        }
    aDataAndTime.AppendNum( num );

    // Milliseconds
    aDataAndTime.Append( '.' );
    aDataAndTime.AppendNumFixedWidth( microseconds, EDecimal, 3 );

    // NOTE: If you add more spaces etc. remember increment KMaxTimeStamp size!

    if ( iLoggerType == CStifLogger::EHtml )
        {
        aDataAndTime.Append( _L("....") );
        }
    else
        {
        aDataAndTime.Append( _L("    ") );
        }

    }

// ================= OTHER EXPORTED FUNCTIONS =================================
// None

// End of File
