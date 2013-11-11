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
* Description:
*
*/

#include "MemSpyEngineOutputSinkFile.h"

// System includes
#include <e32svr.h>

// User includes
#include <memspy/engine/memspyenginelogger.h>
#include <memspy/engine/memspyengineutils.h>

// Constants
const TInt KMemSpyLineBufferLength = 1024;
const TInt KMemSpyFileServerBufferExpandSize = 1024 * 32;

// Literal constants
_LIT8( KMemSpyCRLF, "\r\n" );


CMemSpyEngineOutputSinkFile::CMemSpyEngineOutputSinkFile( CMemSpyEngine& aEngine )
:   CMemSpyEngineOutputSink( aEngine )
    {
    }


CMemSpyEngineOutputSinkFile::~CMemSpyEngineOutputSinkFile()
    {
    TRACE( RDebug::Printf( "CMemSpyEngineOutputSinkFile::~CMemSpyEngineOutputSinkFile() - START" ) );
    
    delete iRoot;

    TRACE( RDebug::Printf( "CMemSpyEngineOutputSinkFile::~CMemSpyEngineOutputSinkFile() - destroying normal logs..." ) );
    iLogs.ResetAndDestroy();
    iLogs.Close();

    TRACE( RDebug::Printf( "CMemSpyEngineOutputSinkFile::~CMemSpyEngineOutputSinkFile() - destroying limbo logs..." ) );
    iLogsPendingDestruction.ResetAndDestroy();
    iLogsPendingDestruction.Close();

    iFsSession.Close();

    TRACE( RDebug::Printf( "CMemSpyEngineOutputSinkFile::~CMemSpyEngineOutputSinkFile() - END" ) );
    }


void CMemSpyEngineOutputSinkFile::ConstructL( const TDesC& aRootFolder )
    {
    TRACE( RDebug::Printf( "CMemSpyEngineOutputSinkFile::ConstructL() - START" ) );
    
    iRoot = aRootFolder.AllocL();

    BaseConstructL();

    // Identify the file server process id
    TRACE( RDebug::Printf( "CMemSpyEngineOutputSinkFile::ConstructL() - identifying f32 process id..." ) );
    iFileServerProcessId = MemSpyEngineUtils::IdentifyFileServerProcessIdL();

    // Create the standard output file
    TRACE( RDebug::Printf( "CMemSpyEngineOutputSinkFile::ConstructL() - connecting to f32..." ) );
    User::LeaveIfError( iFsSession.Connect() );
    
    // Make default log file
    TRACE( RDebug::Printf( "CMemSpyEngineOutputSinkFile::ConstructL() - preparing default log..." ) );
    ConstructDefaultLogFileL();

    TRACE( RDebug::Printf( "CMemSpyEngineOutputSinkFile::ConstructL() - END" ) );
    }


CMemSpyEngineOutputSinkFile* CMemSpyEngineOutputSinkFile::NewL( CMemSpyEngine& aEngine, const TDesC& aRootFolder )
    {
    CMemSpyEngineOutputSinkFile* self = new(ELeave) CMemSpyEngineOutputSinkFile( aEngine );
    CleanupStack::PushL( self );
    self->ConstructL( aRootFolder );
    CleanupStack::Pop( self );
    return self;
    }


void CMemSpyEngineOutputSinkFile::ProcessSuspendedL( TProcessId aId )
    {
    TRACE( RDebug::Print( _L("CMemSpyEngineOutputSinkFile::ProcessSuspendedL() - START - aId: %d, iFileServerSuspended: %d, iFileServerProcessId: %d"), (TUint) aId, iFileServerSuspended, (TUint) iFileServerProcessId ) );

    iFileServerSuspended = ( (TUint) aId == iFileServerProcessId );
    if  ( iFileServerSuspended )
        {
        const TInt count = iLogs.Count();
        TRACE( RDebug::Print( _L("CMemSpyEngineOutputSinkFile::ProcessSuspendedL() - enabling buffers for %d logs"), count ) );
        for( TInt i=0; i<count; i++ )
            {
            CMemSpyEngineFileHolder* log = iLogs[i];
            log->EnableBufferL();
            }
        }

    TRACE( RDebug::Print( _L("CMemSpyEngineOutputSinkFile::ProcessSuspendedL() - END - aId: %d, iFileServerSuspended: %d"), (TUint) aId, iFileServerSuspended ) );
    }


void CMemSpyEngineOutputSinkFile::ProcessResumed( TProcessId aId )
    {
    const TBool isF32Process = ( (TUint) aId == iFileServerProcessId );
    TRACE( RDebug::Print( _L("CMemSpyEngineOutputSinkFile::ProcessResumed() - START - aId: %d, iFileServerSuspended: %d, isF32Process: %d"), (TUint) aId, iFileServerSuspended, isF32Process ) );
    //
    if  ( iFileServerSuspended )
        {
        __ASSERT_ALWAYS( isF32Process, User::Invariant() );

        TRAPD( err, DisableAllBuffersAfterFileServerResumedL() );
        if ( err != KErrNone )
            {
            TRACE( RDebug::Printf( "CMemSpyEngineOutputSinkFile::ProcessResumed() - disabled all buffers, completed with err: %d", err ) );
            }

        iLogsPendingDestruction.ResetAndDestroy();
        TRACE( RDebug::Printf( "CMemSpyEngineOutputSinkFile::ProcessResumed() - deleted all pending destruction logs..." ) );

        // Indicate file server is no longer suspended
        iFileServerSuspended = EFalse;
        }
    
    TRACE( RDebug::Print( _L("CMemSpyEngineOutputSinkFile::ProcessResumed() - END - aId: %d, iFileServerSuspended: %d, isF32Process: %d"), (TUint) aId, iFileServerSuspended, isF32Process ) );
    }


TMemSpySinkType CMemSpyEngineOutputSinkFile::Type() const
    {
    return ESinkTypeFile;
    }


void CMemSpyEngineOutputSinkFile::DataStreamBeginL( const TDesC& aContext, const TDesC& aFolder, const TDesC& aExtension )
    {
    DataStreamBeginL( aContext, aFolder, aExtension, ETrue );
    }


void CMemSpyEngineOutputSinkFile::DataStreamBeginL( const TDesC& aContext, const TDesC& aFolder, const TDesC& aExtension, TBool aOverwrite )
    {
    DataStreamBeginL( aContext, aFolder, aExtension, aOverwrite, ETrue );
    }


void CMemSpyEngineOutputSinkFile::DataStreamBeginL( const TDesC& aContext, const TDesC& aFolder, const TDesC& aExtension, TBool aOverwrite, TBool aUseTimeStamp )
    {
    CMemSpyEngineSinkMetaData* meta = CMemSpyEngineSinkMetaData::NewL( iRoot->Des(), aContext, aFolder, aExtension, aOverwrite, aUseTimeStamp );
    CleanupStack::PushL( meta );

    TRACE( RDebug::Printf( "CMemSpyEngineOutputSinkFile::DataStreamBeginL() - START - log count: %d, iFileServerSuspended: %d", iLogs.Count(), iFileServerSuspended ) );
    //
    CMemSpyEngineFileHolder* log = NULL;
    if  ( iFileServerSuspended )
        {
        TRACE( RDebug::Printf( "CMemSpyEngineOutputSinkFile::DataStreamBeginL() - F32 SUSPENDED!" ) );
        log = CMemSpyEngineFileHolder::NewLogToRAML( *this, meta );
        }
    else
        {
        log = CMemSpyEngineFileHolder::NewL( *this, meta );
        }
    //
    TRACE( RDebug::Printf( "CMemSpyEngineOutputSinkFile::DataStreamBeginL() - log: 0x%08x", log ) );
    CleanupStack::Pop( meta );
    CleanupStack::PushL( log );
    iLogs.AppendL( log );
    CleanupStack::Pop( log );
    //
    TRACE( RDebug::Print( _L("CMemSpyEngineOutputSinkFile::DataStreamBeginL() - END - log count: %d, iFileServerSuspended: %d, fileName: [%S]"), iLogs.Count(), iFileServerSuspended, &log->FileName() ) );
    }


void CMemSpyEngineOutputSinkFile::DataStreamEndL()
    {
    const TInt count = iLogs.Count();
    TRACE( RDebug::Printf( "CMemSpyEngineOutputSinkFile::DataStreamEndL() - count: %d", count ) );

    __ASSERT_ALWAYS( count > 1, User::Invariant() );

    CMemSpyEngineFileHolder* headLog = iLogs[ count - 1 ];
    iLogs.Remove( count - 1 );
    TRACE( RDebug::Printf( "CMemSpyEngineOutputSinkFile::DataStreamEndL() - headLog: 0x%08x", headLog ) );

    if  ( headLog->UsingBuffer() )
        {
        TRACE( RDebug::Printf( "CMemSpyEngineOutputSinkFile::DataStreamEndL() - headLog is using buffer so doing deferred delete when F32 is not suspended anymore..." ) );

        // Must save the log until after its safe to resume 
        // writing to disk.
        CleanupStack::PushL( headLog );
        iLogsPendingDestruction.AppendL( headLog );
        CleanupStack::Pop( headLog );
        }
    else
        {
        TRACE( RDebug::Printf( "CMemSpyEngineOutputSinkFile::DataStreamEndL() - deleting head log immediately!" ) );
        delete headLog;
        }

    TRACE( RDebug::Printf( "CMemSpyEngineOutputSinkFile::DataStreamEndL() - END" ) );
    }


void CMemSpyEngineOutputSinkFile::DoOutputLineL( const TDesC& aLine )
    {
    HeadLog().WriteLineL( aLine );
    }


void CMemSpyEngineOutputSinkFile::DoOutputRawL( const TDesC8& aData )
    {
    HeadLog().WriteRawL( aData );
    }


TBool CMemSpyEngineOutputSinkFile::IsPrefixAllowed( const TDesC& /*aPrefix*/ )
    {
    // If we are outputting to the main log, then we must allow
    // the prefix. If we are outputting to a child log, then we don't
    // need it.
    return HeadLog().IsMainLog();
    }


void CMemSpyEngineOutputSinkFile::ConstructDefaultLogFileL()
    {
    CMemSpyEngineFileHolder* log = CMemSpyEngineFileHolder::NewLC( *this );

    TRACE( RDebug::Printf( "CMemSpyEngineOutputSinkFile::ConstructDefaultLogFileL() - START - headLog: 0x%08x", log ) );
    iLogs.AppendL( log );
    CleanupStack::Pop( log );

    TRACE( RDebug::Printf( "CMemSpyEngineOutputSinkFile::ConstructDefaultLogFileL() - END - headLog: 0x%08x", log ) );
    }


void CMemSpyEngineOutputSinkFile::DisableAllBuffersAfterFileServerResumedL()
    {
    const TInt count = iLogs.Count();
    const TInt count2 = iLogsPendingDestruction.Count();

    TRACE( RDebug::Printf( "CMemSpyEngineOutputSinkFile::DisableAllBuffersAfterFileServerResumedL() - START - logs: %d, pending: %d", count, count2 ) );

    TInt err = KErrNone;

    // Inform logging object that they should stop logging to RAM. This may cause files
    // to be opened and data flushed.
    for( TInt i=0; i<count; i++ )
        {
        CMemSpyEngineFileHolder* log = iLogs[ i ];
        TRACE( RDebug::Print( _L("CMemSpyEngineOutputSinkFile::DisableAllBuffersAfterFileServerResumedL() -  logN[%2d]: 0x%08x, name: %S"), i, log, &log->FileName() ) );
        TRAP(err, log->DisableBufferL() );
        if  ( err != KErrNone )
            {
            TRACE( RDebug::Printf( "CMemSpyEngineOutputSinkFile::DisableAllBuffersAfterFileServerResumedL() - error whilst closing log: %d", err ) );
            }
        }

    // Finalise any pending destruction log objects. These are log objects
    // that were created and finalised whilst the file server was suspended.
    for( TInt ii = count2-1; ii>=0; ii--)
        {
        CMemSpyEngineFileHolder* log = iLogsPendingDestruction[ ii ];
        TRACE( RDebug::Print( _L("CMemSpyEngineOutputSinkFile::DisableAllBuffersAfterFileServerResumedL() - logPD[%2d]: 0x%08x, name: %S"), ii, log, &log->FileName() ) );
        TRAP(err, log->DisableBufferL() );
        if  ( err != KErrNone )
            {
            TRACE( RDebug::Printf( "CMemSpyEngineOutputSinkFile::DisableAllBuffersAfterFileServerResumedL() - error whilst closing log: %d", err ) );
            }
        }

    TRACE( RDebug::Printf( "CMemSpyEngineOutputSinkFile::DisableAllBuffersAfterFileServerResumedL() - END" ) );
    }


CMemSpyEngineFileHolder& CMemSpyEngineOutputSinkFile::HeadLog() const
    {
    // Head log is always the first log
    const TInt count = iLogs.Count();
    __ASSERT_ALWAYS( count >= 1, User::Invariant() );
    CMemSpyEngineFileHolder* headLog = iLogs[ count - 1 ];

    //TRACE( RDebug::Print( _L("CMemSpyEngineOutputSinkFile::HeadLog() - headLog: 0x%08x %S"), headLog, &headLog->FileName() ) );
    return *headLog;
    }







































CMemSpyEngineFileHolder::CMemSpyEngineFileHolder( CMemSpyEngineOutputSinkFile& aParent, CMemSpyEngineSinkMetaData* aMetaData )
:   iParent( aParent ), iMetaData( aMetaData )
    {
    }


CMemSpyEngineFileHolder::~CMemSpyEngineFileHolder()
    {
    TPtrC pFileName( KNullDesC );
    if  ( iFileName != NULL )
        {
        pFileName.Set( *iFileName );
        }
    TRACE( RDebug::Print( _L("CMemSpyEngineFileHolder::~CMemSpyEngineFileHolder() - START - this: 0x%08x, iFileName: %S, handle: 0x%08x, iEntireFileBuffer: 0x%08x, iIsMainLog: %d"), this, &pFileName, iFile.SubSessionHandle(), iEntireFileBuffer, iIsMainLog ) );

    if  ( iEntireFileBuffer || iWorkingBuffer )
        {
        // This may actually open the file
        TRACE( RDebug::Printf( "CMemSpyEngineFileHolder::~CMemSpyEngineFileHolder() - disabling file buffers..." ) );
        TRAP_IGNORE( DisableBufferL() );
        }

    // Now close the file, deleting anything that is empty
    if  ( iFile.SubSessionHandle() != KNullHandle )
        {
        TRACE( RDebug::Printf( "CMemSpyEngineFileHolder::~CMemSpyEngineFileHolder() - flushing file..." ) );

        // Flush anything that is pending and then check whether 
        // the file is empty. If so, delete it.
        iFile.Flush(); // Ignore error
       
        // Delete empty files
        TInt size = 0;
        TInt err = iFile.Size( size );
        TRACE( RDebug::Printf( "CMemSpyEngineFileHolder::~CMemSpyEngineFileHolder() - size: %d, err: %d", size, err ) );

        if  ( err == KErrNone && size == 0 )
            {
            // Close the file and get F32 to delete it...
            TRACE( RDebug::Printf( "CMemSpyEngineFileHolder::~CMemSpyEngineFileHolder() - deleting empty file!" ) );
            iFile.Close();
            err = FsSession().Delete( *iFileName );
            TRACE( RDebug::Printf( "CMemSpyEngineFileHolder::~CMemSpyEngineFileHolder() - delete err: %d", err ) );
            }
        }
    //
    iFile.Close();
    //
    delete iLineBuffer;
    delete iWorkingBuffer;
    delete iEntireFileBuffer;
    TRACE( RDebug::Print( _L("CMemSpyEngineFileHolder::~CMemSpyEngineFileHolder() - END - this: 0x%08x, iFileName: %S, iIsMainLog: %d"), this, &pFileName, iIsMainLog ) );
    delete iFileName;
    delete iMetaData;
    }


void CMemSpyEngineFileHolder::CommonConstructL()
    {
    if  ( iLineBuffer == NULL )
        {
        iLineBuffer = HBufC8::NewL( KMemSpyLineBufferLength );
        }
    if  ( iWorkingBuffer == NULL )
        {
        iWorkingBuffer = CBufFlat::NewL( KMemSpyFileServerBufferExpandSize );
        }
    }


void CMemSpyEngineFileHolder::ConstructL()
    {
    TRACE( RDebug::Printf( "CMemSpyEngineFileHolder::ConstructL(D) - START" ) );
    iIsMainLog = ETrue;

    // Make emtpy meta data
    ASSERT( !iMetaData );
    iMetaData = CMemSpyEngineSinkMetaData::NewL( iParent.iRoot->Des(), KNullDesC, KNullDesC, KNullDesC, ETrue, ETrue );

    // Prepare common details
    CommonConstructL();
    TRACE( RDebug::Printf( "CMemSpyEngineFileHolder::ConstructL(D) - CommonConstructL() completed okay..." ) );

    // Construct the default log file.
    TRACE( RDebug::Printf( "CMemSpyEngineFileHolder::ConstructL(D) - calling OpenFileL()..." ) );
    OpenFileL();
    TRACE( RDebug::Printf( "CMemSpyEngineFileHolder::ConstructL(D) - called OpenFileL()" ) );

    TRACE( RDebug::Printf( "CMemSpyEngineFileHolder::ConstructL(D) - END - iIsMainLog: %d", iIsMainLog ) );
    }



CMemSpyEngineFileHolder* CMemSpyEngineFileHolder::NewLC( CMemSpyEngineOutputSinkFile& aParent )
    {
    TRACE( RDebug::Printf( "CMemSpyEngineFileHolder::NewLC() - START") );
    CMemSpyEngineFileHolder* self = new(ELeave) CMemSpyEngineFileHolder( aParent, NULL );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


CMemSpyEngineFileHolder* CMemSpyEngineFileHolder::NewL( CMemSpyEngineOutputSinkFile& aParent, CMemSpyEngineSinkMetaData* aMetaData )
    {
    TRACE( RDebug::Printf( "CMemSpyEngineFileHolder::NewL() - START") );
    CMemSpyEngineFileHolder* self = new(ELeave) CMemSpyEngineFileHolder( aParent, aMetaData );
    CleanupStack::PushL( self );
    self->CommonConstructL();
    self->OpenFileL();
    CleanupStack::Pop( self );
    return self;
    }


CMemSpyEngineFileHolder* CMemSpyEngineFileHolder::NewLogToRAML( CMemSpyEngineOutputSinkFile& aParent, CMemSpyEngineSinkMetaData* aMetaData )
    {
    TRACE( RDebug::Printf( "CMemSpyEngineFileHolder::NewLogToRAML() - START") );
    CMemSpyEngineFileHolder* self = new(ELeave) CMemSpyEngineFileHolder( aParent, aMetaData );
    CleanupStack::PushL( self );
    self->CommonConstructL();
    self->EnableBufferL();
    CleanupStack::Pop( self );
    return self;
    }


void CMemSpyEngineFileHolder::OpenFileL()
    {
    TInt err = KErrNone;

    TRACE( RDebug::Printf( "CMemSpyEngineFileHolder::OpenFileL() - START - iMetaData: 0x%08x", iMetaData ) );
    ASSERT( iMetaData );

    delete iFileName;
    iFileName = NULL;
    if  ( iFile.SubSessionHandle() != KNullHandle )
        {
        TRACE( RDebug::Printf( "CMemSpyEngineFileHolder::OpenFileL() - closing existing file..." ) );
        iFile.Flush();
        iFile.Close();
        }
    
    // First try, with current external drive
    TRAP( err, PrepareFileL() );
    TRACE( RDebug::Printf( "CMemSpyEngineFileHolder::OpenFileL() - PrepareFileL(1) - err: %d", err ) );
    
    if  ( err != KErrNone )
        {
        TRACE( RDebug::Printf( "CMemSpyEngineFileHolder::OpenFileL() - trying with forced C:\\ ..." ) );

        // Try again with C:\ instead...
        const TDriveNumber cDrive = EDriveC;
        TRAP(err, PrepareFileL( &cDrive ) );
        TRACE( RDebug::Printf( "CMemSpyEngineFileHolder::OpenFileL() - PrepareFileL(2) - err: %d", err ) );
        }

    TRACE( RDebug::Printf( "CMemSpyEngineFileHolder::OpenFileL() - final err: %d", err ) );
    User::LeaveIfError( err );

    TRACE( RDebug::Printf( "CMemSpyEngineFileHolder::OpenFileL() - END - OK - iIsMainLog: %d", iIsMainLog ) );
    }


void CMemSpyEngineFileHolder::PrepareFileL( const TDriveNumber* aForceDrive )
    {
    TRACE( RDebug::Printf( "CMemSpyEngineFileHolder::PrepareFileL() - START - this: 0x%08x", this ) );
    ASSERT( iMetaData );

    iFileName = GenerateFileNameLC( aForceDrive );
    CleanupStack::Pop( iFileName );

    // Try to make log directory - ignore if already exists
    TInt error = FsSession().MkDirAll( *iFileName );
    TRACE( RDebug::Print( _L("CMemSpyEngineFileHolder::PrepareFileL() - iFileName: %S, mkDir: %d, aOverwrite: %d"), iFileName, error, iMetaData->Overwrite() ) );

    if  ( error == KErrNone || error == KErrAlreadyExists )
        {
        if  ( iMetaData->Overwrite() )
            {
            error = iFile.Replace( FsSession(), *iFileName, EFileWrite );
            TRACE( RDebug::Printf( "CMemSpyEngineFileHolder::PrepareFileL() - overwrite/replace error: %d", error ) );
            }
        else
            {
            error = iFile.Open( FsSession(), *iFileName, EFileWrite );
            TRACE( RDebug::Printf( "CMemSpyEngineFileHolder::PrepareFileL() - open error: %d", error ) );
            
            // Try creating it then...
            if  ( error == KErrNotFound )
                {
                error = iFile.Create( FsSession(), *iFileName, EFileWrite );
                TRACE( RDebug::Printf( "CMemSpyEngineFileHolder::PrepareFileL() - create error: %d", error ) );
                }
            }
            
        // Seek to end of file
        TRACE( RDebug::Printf( "CMemSpyEngineFileHolder::PrepareFileL() - final error: %d", error ) );
        if  ( error == KErrNone )
            {
            TInt pos = 0;
            error = iFile.Seek( ESeekEnd, pos );
            TRACE( RDebug::Printf( "CMemSpyEngineFileHolder::PrepareFileL() - seek error: %d", error ) );
            }
        }

    TRACE( RDebug::Printf( "CMemSpyEngineFileHolder::PrepareFileL() - END - this: 0x%08x, error: %d", this, error ) );
    User::LeaveIfError( error );
    }


void CMemSpyEngineFileHolder::WriteLineL( const TDesC& aData )
    {
    TPtr8 pText( iLineBuffer->Des() );
    pText.Copy( aData );
    pText.Append( KMemSpyCRLF );

    if  ( iWorkingBuffer != NULL )
        {
        AddToWorkingBufferL( pText );
        }
    else if ( iFile.SubSessionHandle() != KNullHandle )
        {
        const TInt error = iFile.Write( pText );
        if  ( error != KErrNone )
            {
            TRACE( RDebug::Printf( "CMemSpyEngineFileHolder::WriteLineL() - FILE WRITE ERROR - this: 0x%08x, error: %d", this, error ) );
            }
        User::LeaveIfError( error );
        }
    else
        {
        TRACE( RDebug::Printf( "CMemSpyEngineFileHolder::WriteLineL() - DISCARDING LINE!" ) );
        }
    }


void CMemSpyEngineFileHolder::WriteRawL( const TDesC8& aData )
    {
    const TInt error = iFile.Write( aData );
    User::LeaveIfError( error );
    }


void CMemSpyEngineFileHolder::EnableBufferL()
    {
    TRACE( RDebug::Printf( "CMemSpyEngineFileHolder::EnableBufferL() - START - this: 0x%08x", this ) );
    if  ( iEntireFileBuffer == NULL )
        {
        iEntireFileBuffer = CBufFlat::NewL( KMemSpyFileServerBufferExpandSize );
        }
    //
    iEntireFileBuffer->Reset();
    TRACE( RDebug::Printf( "CMemSpyEngineFileHolder::EnableBufferL() - END - this: 0x%08x", this ) );
    }


void CMemSpyEngineFileHolder::DisableBufferL()
    {
#ifdef _DEBUG
    TInt size = 0;
    if  ( iFile.SubSessionHandle() != KNullHandle )
        {
        iFile.Size( size );
        }
    TInt ramBufferSize = 0;
    if  ( iEntireFileBuffer )
        {
        ramBufferSize = iEntireFileBuffer->Size();
        }
    //
    RDebug::Printf( "CMemSpyEngineFileHolder::DisableBufferL() - START - file handle: 0x%08x, iEntireFileBuffer: 0x%08x, fileSize: %d, ramBufferSize: %d", iFile.SubSessionHandle(), iEntireFileBuffer, size, ramBufferSize );
#endif

    // This will also open the file if necessary
    FlushWorkingBufferL();
    //
    if  ( iEntireFileBuffer != NULL )
        {
        // Probably need to open the file...
        if  ( iFile.SubSessionHandle() == KNullHandle )
            {
            TRACE( RDebug::Printf( "CMemSpyEngineFileHolder::DisableBufferL() - FILE IS NOT YET OPEN!" ) );
            OpenFileL();
            TRACE( RDebug::Printf( "CMemSpyEngineFileHolder::DisableBufferL() - file opened okay." ) );
            }

        TRACE( RDebug::Print( _L("CMemSpyEngineFileHolder::DisableBufferL() - about to write entire file buffer - file: %S"), &FileName() ) );
        const TPtr8 pBuffer( iEntireFileBuffer->Ptr( 0 ) );
        const TInt error = iFile.Write( pBuffer );
        TRACE( RDebug::Printf( "CMemSpyEngineFileHolder::DisableBufferL() - write result: %d", error ) );
        //
        delete iEntireFileBuffer;
        iEntireFileBuffer = NULL;
        //
        User::LeaveIfError( error );
        }
    
    TRACE( RDebug::Printf( "CMemSpyEngineFileHolder::DisableBufferL() - END" ) );
    }


void CMemSpyEngineFileHolder::FlushWorkingBufferL()
    {
    TRACE( RDebug::Print( _L("CMemSpyEngineFileHolder::FlushWorkingBufferL() - START - fileName: [%S], file handle: 0x%08x, iWorkingBuffer: 0x%08x"), &FileName(), iFile.SubSessionHandle(), iWorkingBuffer ) );
    __ASSERT_ALWAYS( iWorkingBuffer, MemSpyEngineUtils::Panic( EMemSpyEnginePanicSinkFileWorkingFileBufferIsNull ) );

    // The underlying file may not be open - it could be that we are buffering everything
    // to RAM (i.e. the file server is currently suspended).
    const TPtr8 pBuffer( iWorkingBuffer->Ptr(0) );
    //
    if  ( iEntireFileBuffer )
        {
        __ASSERT_ALWAYS( iEntireFileBuffer, MemSpyEngineUtils::Panic( EMemSpyEnginePanicSinkFileEntireFileBufferNull ) );

        TRACE( RDebug::Printf( "CMemSpyEngineFileHolder::FlushWorkingBufferL() - file not open or we are currently bufferring to RAM - copying %d bytes to entire file buffer (%d)", pBuffer.Length(), iEntireFileBuffer->Size() ) );
        iEntireFileBuffer->InsertL( iEntireFileBuffer->Size(), pBuffer );
        }
    else if ( iFile.SubSessionHandle() != KNullHandle )
        {
        const TInt error = iFile.Write( pBuffer );
        if  ( error != KErrNone )
            {
            TRACE( RDebug::Printf( "CMemSpyEngineFileHolder::FlushWorkingBufferL() - write result: %d", error ) );
            }
        User::LeaveIfError( error );
        }

    iWorkingBuffer->Reset();
    TRACE( RDebug::Print( _L("CMemSpyEngineFileHolder::FlushWorkingBufferL() - END - fileName: %S"), &FileName() ) );
    }


void CMemSpyEngineFileHolder::AddToWorkingBufferL( const TDesC8& aText )
    {
    if  ( iWorkingBuffer->Size() + aText.Length() > KMemSpyFileServerBufferExpandSize )
        {
        FlushWorkingBufferL();
        }
    //
    iWorkingBuffer->InsertL( iWorkingBuffer->Size(), aText );
    }


HBufC* CMemSpyEngineFileHolder::CleanContextInfoLC( const TDesC& aContext )
    {
    TRACE( RDebug::Print( _L("CMemSpyEngineFileHolder::CleanContextInfoLC() - START - %S"), &aContext ) );
    TFileName fileName;

    TBool seenDoubleColon = EFalse;
    const TInt length = aContext.Length();
    for( TInt i=0; i<length; i++ )
        {
        const TChar c( aContext[ i ] );
        const TBool haveNextChar = ( i+1 < length );
        //
        //TRACE( RDebug::Print( _L("CMemSpyEngineFileHolder::CleanContextInfoLC() - c[%03d]: \'%c\', haveNextChar: %d, seenDoubleColon: %d"), i, (TUint32) c, haveNextChar, seenDoubleColon ) );
        //
        if  ( c == ':' && haveNextChar && aContext[ i + 1 ] == ':' )
            {
            // Skip double colon
            i++;
            fileName.Append( '-' );
            seenDoubleColon = ETrue;
            }
        else if ( c == '.' )
            {
            if  ( seenDoubleColon )
                {
                break;
                }
            else
                {
                fileName.Append( '-' );
                }
            }
        else
            {
            fileName.Append( c );
            }
        }

    TRACE( RDebug::Print( _L("CMemSpyEngineFileHolder::CleanContextInfoLC() - END - %S"), &fileName ) );
    return fileName.AllocLC();
    }


HBufC* CMemSpyEngineFileHolder::GenerateFileNameLC( const TDriveNumber* aForceDrive )
    {
    TFileName name;
    MemSpyEngineUtils::GetFolderL( FsSession(), name, *iMetaData, aForceDrive );
    return name.AllocLC();
    }


RFs& CMemSpyEngineFileHolder::FsSession()
    {
    return iParent.iFsSession;
    }


const TDesC& CMemSpyEngineFileHolder::FileName() const 
    {
    if  ( iFileName )
        {
        return *iFileName;
        }
    //
    return KNullDesC;
    }

