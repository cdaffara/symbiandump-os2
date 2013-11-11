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

#ifndef MEMSPYENGINEOUTPUTSINKFILE_H
#define MEMSPYENGINEOUTPUTSINKFILE_H

// System includes
#include <e32base.h>
#include <f32file.h>

// User includes
#include <memspy/engine/memspyengineoutputsink.h>

// Classes referenced
class CMemSpyEngine;
class CMemSpyEngineFileHolder;

NONSHARABLE_CLASS( CMemSpyEngineOutputSinkFile ) : public CMemSpyEngineOutputSink
    {
public:
    static CMemSpyEngineOutputSinkFile* NewL( CMemSpyEngine& aEngine, const TDesC& aRootFolder );
    ~CMemSpyEngineOutputSinkFile();

public:
    CMemSpyEngineOutputSinkFile( CMemSpyEngine& aEngine );
    void ConstructL( const TDesC& aRootFolder );
    
private: // From CMemSpyEngineOutputSink
    void ProcessSuspendedL( TProcessId aId );
    void ProcessResumed( TProcessId aId );
    TMemSpySinkType Type() const;
    void DataStreamBeginL( const TDesC& aContext, const TDesC& aFolder, const TDesC& aExtension );
    void DataStreamBeginL( const TDesC& aContext, const TDesC& aFolder, const TDesC& aExtension, TBool aOverwrite );
    void DataStreamBeginL( const TDesC& aContext, const TDesC& aFolder, const TDesC& aExtension, TBool aOverwrite, TBool aUseTimeStamp );
    void DataStreamEndL();
    void DoOutputLineL( const TDesC& aLine );
    void DoOutputRawL( const TDesC8& aData );
    TBool IsPrefixAllowed( const TDesC& aPrefix );

private: // Internal
    void IdentifyFileServerProcessIdL();
    void ConstructDefaultLogFileL();
    void DisableAllBuffersAfterFileServerResumedL();
    CMemSpyEngineFileHolder& HeadLog() const;

private:
    RFs iFsSession;
    RPointerArray< CMemSpyEngineFileHolder > iLogs;
    RPointerArray< CMemSpyEngineFileHolder > iLogsPendingDestruction;

    TUint iFileServerProcessId;
    TBool iFileServerSuspended;
    
    HBufC* iRoot;

private:
    friend class CMemSpyEngineFileHolder;
    };




NONSHARABLE_CLASS( CMemSpyEngineFileHolder ) : public CBase
    {
public:
    static CMemSpyEngineFileHolder* NewLC( CMemSpyEngineOutputSinkFile& aParent );
    static CMemSpyEngineFileHolder* NewL( CMemSpyEngineOutputSinkFile& aParent, CMemSpyEngineSinkMetaData* aMetaData );
    static CMemSpyEngineFileHolder* NewLogToRAML( CMemSpyEngineOutputSinkFile& aParent, CMemSpyEngineSinkMetaData* aMetaData );
    ~CMemSpyEngineFileHolder();

private:
    CMemSpyEngineFileHolder( CMemSpyEngineOutputSinkFile& aParent, CMemSpyEngineSinkMetaData* aMetaData = NULL );
    void ConstructL();
    void CommonConstructL();

public: // API
    void WriteLineL( const TDesC& aData );
    void WriteRawL( const TDesC8& aData );
    void EnableBufferL();
    void DisableBufferL();
    const TDesC& FileName() const;
    //
    inline TBool UsingBuffer() const { return iEntireFileBuffer != NULL; }
    inline TBool IsMainLog() const { return iIsMainLog; }

private: // Internal
    void OpenFileL();
    void AddToWorkingBufferL( const TDesC8& aText );
    void FlushWorkingBufferL();
    void PrepareFileL( const TDriveNumber* aForceDrive = NULL );
    HBufC* CleanContextInfoLC( const TDesC& aContext );
    HBufC* GenerateFileNameLC( const TDriveNumber* aForceDrive = NULL );
    RFs& FsSession();

private: // Data members
    CMemSpyEngineOutputSinkFile& iParent;
    CMemSpyEngineSinkMetaData* iMetaData;
    RFile iFile;
    HBufC* iFileName;
    HBufC8* iLineBuffer;
    TBool iIsMainLog;
    CBufFlat* iEntireFileBuffer;
    CBufFlat* iWorkingBuffer;
    };



#endif
