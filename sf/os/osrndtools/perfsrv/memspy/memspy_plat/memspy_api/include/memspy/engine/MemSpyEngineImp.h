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

#ifndef MEMSPYENGINEIMP_H
#define MEMSPYENGINEIMP_H

// System includes
#include <e32base.h>
#include <badesca.h>

// User includes
#include <memspy/engine/memspyenginemidwife.h>
#include <memspy/engine/memspyengineoutputsink.h>
#include <memspy/engine/memspyengineundertaker.h>
#include <memspy/engine/memspyenginehelperwindowserver.h>

// Classes referenced
class RFs;
class CMemSpyProcess;
class CMemSpyThread;
class RMemSpyDriverClient;
class CMemSpyEngineServer;
class MMemSpyEngineObserver;
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


NONSHARABLE_CLASS( CMemSpyEngineImp ) : public CBase, public MMemSpyEngineUndertakerObserver, 
                                                   public MMemSpyEngineMidwifeObserver
    {
public:
    CMemSpyEngineImp( RFs& aFsSession, CMemSpyEngine& aEngine );
    ~CMemSpyEngineImp();
    void ConstructL();

private: // Construction support
    void ConstructHelpersL();

public: // API
    RFs& FsSession();
    CMemSpyEngineObjectContainer& Container();
    const CMemSpyEngineObjectContainer& Container() const;
    void SetObserver( MMemSpyEngineObserver* aObserver );

public:
    void NotifyContainerChangeL();
    void NotifyClientServerOperationRequestL( TInt aType );

public: // Sink related
    CMemSpyEngineOutputSink& Sink();
    TMemSpySinkType SinkType();
    void InstallSinkL( TMemSpySinkType aType );
    void InstallSinkL( TMemSpySinkType aType, const TDesC& aRootFolder );

public: // Misc
    void ListOpenFilesL();
    void GetVersion( TVersion& aVersion );
    TBool IsHelperWindowServerSupported();

public: 
    CMemSpyEngineHelperHeap& HelperHeap();
    CMemSpyEngineHelperStack& HelperStack();
    CMemSpyEngineHelperCodeSegment& HelperCodeSegment();
    CMemSpyEngineHelperChunk& HelperChunk();
    CMemSpyEngineHelperThread& HelperThread();
    CMemSpyEngineHelperProcess& HelperProcess();
    CMemSpyEngineHelperServer& HelperServer();
    CMemSpyEngineHelperActiveObject& HelperActiveObject();
    CMemSpyEngineHelperKernelContainers& HelperKernelContainers();
    CMemSpyEngineHelperFileSystem& HelperFileSystem();
    CMemSpyEngineHelperECom& HelperECom();
    CMemSpyEngineHelperSysMemTracker& HelperSysMemTracker();
    CMemSpyEngineHelperFbServ& HelperFbServ();
    CMemSpyEngineHelperROM& HelperROM();
    CMemSpyEngineHelperRAM& HelperRAM();
    MMemSpyEngineHelperWindowServer& HelperWindowServer();
    CMemSpyEngineHelperCondVar& HelperCondVar();

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

private: // From MMemSpyEngineUndertakerObserver
	void ThreadIsDeadL( const TThreadId& aId, const RThread& aThread );
	void ProcessIsDeadL( const TProcessId& aId, const RProcess& aProcess );

private: // From MMemSpyEngineMidwifeObserver
	void ThreadIsBornL( const TThreadId& aId, const RThread& aThread );
	void ProcessIsBornL( const TProcessId& aId, const RProcess& aProcess );

private: // Internal methods
    static void ResumeSuspendedProcess( TAny* aSelf );

private: // Data members
    RFs& iFsSession;
    CMemSpyEngine& iEngine;
    CMemSpyEngineObjectContainer* iContainer;
    RMemSpyDriverClient* iMemSpyDriver;
    MMemSpyEngineObserver* iObserver;
    CMemSpyEngineUndertaker* iUndertaker;
    CMemSpyEngineMidwife* iMidwife;
    CMemSpyEngineChunkWatcher* iChunkWatcher;
    //
    CMemSpyEngineHelperHeap* iHelperHeap;
    CMemSpyEngineHelperStack* iHelperStack;
    CMemSpyEngineHelperCodeSegment* iHelperCodeSegment;
    CMemSpyEngineHelperChunk* iHelperChunk;
    CMemSpyEngineHelperThread* iHelperThread;
    CMemSpyEngineHelperProcess* iHelperProcess;
    CMemSpyEngineHelperServer* iHelperServer;
    CMemSpyEngineHelperActiveObject* iHelperActiveObject;
    CMemSpyEngineHelperKernelContainers* iHelperKernelContainers;
    CMemSpyEngineHelperFileSystem* iHelperFileSystem;
    CMemSpyEngineHelperECom* iHelperECom;
    CMemSpyEngineHelperSysMemTracker* iHelperSysMemTracker;
    CMemSpyEngineHelperFbServ* iHelperFbServ;
    CMemSpyEngineHelperROM* iHelperROM;
    CMemSpyEngineHelperRAM* iHelperRAM;
    MMemSpyEngineHelperWindowServer* iHelperWindowServer;
    CMemSpyEngineHelperCondVar* iHelperCondVar;
    //
    CMemSpyEngineServer* iServer;
    //
    CMemSpyEngineOutputSink* iSink;
    TUint iSuspendedProcess;
    RPointerArray<CMemSpyEngineObjectContainer> iContainers;
    RLibrary iHelperWindowServerLoader;
    };




#endif
