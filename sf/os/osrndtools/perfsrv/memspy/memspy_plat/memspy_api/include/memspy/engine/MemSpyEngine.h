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

#ifndef MEMSPYENGINE_H
#define MEMSPYENGINE_H

// System includes
#include <e32base.h>
#include <memspy/engine/memspyengineoutputsinktype.h>
#include <memspy/engine/memspyenginelogger.h>
#include <memspy/engine/memspyenginehelperwindowserver.h>

// Classes referenced
class RFs;
class RMemSpyDriverClient;
class MMemSpyEngineObserver;
class CMemSpyProcess;
class CMemSpyThread;
class CMemSpyEngineImp;
class CMemSpyEngineOutputSink;
class CMemSpyEngineServer;
class CMemSpyEngineMidwife;
class CMemSpyEngineUndertaker;
class CMemSpyEngineChunkWatcher;
class CMemSpyEngineChunkWatcher;
class CMemSpyEngineObjectContainer;
class CMemSpyEngineHelperHeap;
class CMemSpyEngineHelperStack;
class CMemSpyEngineHelperCodeSegment;
class CMemSpyEngineHelperChunk;
class CMemSpyEngineHelperThread;
class CMemSpyEngineHelperProcess;
class CMemSpyEngineHelperServer;
class CMemSpyEngineHelperActiveObject;
class CMemSpyEngineHelperKernelContainers;
class CMemSpyEngineHelperFileSystem;
class CMemSpyEngineHelperECom;
class CMemSpyEngineHelperFbServ;
class CMemSpyEngineHelperSysMemTracker;
class CMemSpyEngineHelperROM;
class CMemSpyEngineHelperRAM;
class CMemSpyEngineHelperCondVar;


NONSHARABLE_CLASS( CMemSpyEngine ) : public CBase
    {
public:
    IMPORT_C static CMemSpyEngine* NewL( RFs& aFsSession );
    IMPORT_C static CMemSpyEngine* NewL( RFs& aFsSession, TBool aStartServer );
    IMPORT_C ~CMemSpyEngine();

private:
    CMemSpyEngine();
    void ConstructL( RFs& aFsSession );

public: // API
    IMPORT_C RFs& FsSession();
    IMPORT_C CMemSpyEngineObjectContainer& Container();
    IMPORT_C const CMemSpyEngineObjectContainer& Container() const;
    IMPORT_C void SetObserver( MMemSpyEngineObserver* aObserver );

public:
    void NotifyContainerChangeL();
    void NotifyClientServerOperationRequestL( TInt aType );

public: // Sink related
    IMPORT_C CMemSpyEngineOutputSink& Sink();
    IMPORT_C TMemSpySinkType SinkType();
    IMPORT_C void InstallSinkL( TMemSpySinkType aType );
    IMPORT_C void InstallDebugSinkL();
    IMPORT_C void InstallFileSinkL( const TDesC& aRootFolder );

public: // Misc
    IMPORT_C void ListOpenFilesL();
    IMPORT_C void GetVersion( TVersion& aVersion );
    IMPORT_C TBool IsHelperWindowServerSupported();

public: 
    IMPORT_C CMemSpyEngineHelperHeap& HelperHeap();
    IMPORT_C CMemSpyEngineHelperStack& HelperStack();
    IMPORT_C CMemSpyEngineHelperCodeSegment& HelperCodeSegment();
    IMPORT_C CMemSpyEngineHelperChunk& HelperChunk();
    IMPORT_C CMemSpyEngineHelperThread& HelperThread();
    IMPORT_C CMemSpyEngineHelperProcess& HelperProcess();
    IMPORT_C CMemSpyEngineHelperServer& HelperServer();
    IMPORT_C CMemSpyEngineHelperActiveObject& HelperActiveObject();
    IMPORT_C CMemSpyEngineHelperKernelContainers& HelperKernelContainers();
    IMPORT_C CMemSpyEngineHelperFileSystem& HelperFileSystem();
    IMPORT_C CMemSpyEngineHelperECom& HelperECom();
    IMPORT_C CMemSpyEngineHelperSysMemTracker& HelperSysMemTracker();
    IMPORT_C CMemSpyEngineHelperFbServ& HelperFbServ();
    IMPORT_C CMemSpyEngineHelperROM& HelperROM();
    IMPORT_C CMemSpyEngineHelperRAM& HelperRAM();
    IMPORT_C MMemSpyEngineHelperWindowServer& HelperWindowServer();
    IMPORT_C CMemSpyEngineHelperCondVar& HelperCondVar();

public: // Driver
    RMemSpyDriverClient& Driver();

public: // Event handlers
    CMemSpyEngineMidwife& Midwife();
    CMemSpyEngineUndertaker& Undertaker();
    CMemSpyEngineChunkWatcher& ChunkWatcher();

public: // Process related
    TInt ProcessSuspendAndGetErrorLC( TProcessId aId );
    void ProcessSuspendLC( TProcessId aId );
    void ProcessResume();
    TProcessId SuspendedProcessId() const;

private: // Data members
    CMemSpyEngineImp* iImp;
    };




#endif
