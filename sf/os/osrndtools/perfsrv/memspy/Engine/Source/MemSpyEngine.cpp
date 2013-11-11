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

#include <memspy/engine/memspyengine.h>

// System includes
#include <e32debug.h>

// User includes
#include <memspy/engine/memspyengineimp.h>


CMemSpyEngine::CMemSpyEngine()
    {
    }


EXPORT_C CMemSpyEngine::~CMemSpyEngine()
    {
#ifdef _DEBUG
    RDebug::Printf( "CMemSpyEngine::~CMemSpyEngine() - START" );
#endif

    delete iImp;

#ifdef _DEBUG
    RDebug::Printf( "CMemSpyEngine::~CMemSpyEngine() - END" );
#endif
    }


void CMemSpyEngine::ConstructL( RFs& aFsSession )
    {
#ifdef _DEBUG
    RDebug::Printf( "CMemSpyEngine::ConstructL() - START" );
#endif
    
    iImp = new(ELeave) CMemSpyEngineImp( aFsSession, *this );
    iImp->ConstructL();

#ifdef _DEBUG
    RDebug::Printf( "CMemSpyEngine::ConstructL() - END" );
#endif
    }


EXPORT_C CMemSpyEngine* CMemSpyEngine::NewL( RFs& aFsSession )
    {
    CMemSpyEngine* self = new(ELeave) CMemSpyEngine();
    CleanupStack::PushL( self );
    self->ConstructL( aFsSession );
    CleanupStack::Pop( self );
    return self;
    }

EXPORT_C CMemSpyEngine* CMemSpyEngine::NewL( RFs& aFsSession, TBool aStartServer )
    {
    return NewL(aFsSession);
    }

EXPORT_C RFs& CMemSpyEngine::FsSession()
    {
    return iImp->FsSession();
    }


EXPORT_C CMemSpyEngineObjectContainer& CMemSpyEngine::Container()
    {
    return iImp->Container();
    }


EXPORT_C const CMemSpyEngineObjectContainer& CMemSpyEngine::Container() const
    {
    return iImp->Container();
    }


EXPORT_C void CMemSpyEngine::SetObserver( MMemSpyEngineObserver* aObserver )
    {
    iImp->SetObserver( aObserver );
    }


void CMemSpyEngine::NotifyContainerChangeL()
    {
    iImp->NotifyContainerChangeL();
    }


void CMemSpyEngine::NotifyClientServerOperationRequestL( TInt aType )
    {
    iImp->NotifyClientServerOperationRequestL( aType );
    }


EXPORT_C CMemSpyEngineOutputSink& CMemSpyEngine::Sink()
    {
    return iImp->Sink();
    }


EXPORT_C TMemSpySinkType CMemSpyEngine::SinkType()
    {
    return iImp->SinkType();
    }


EXPORT_C void CMemSpyEngine::InstallSinkL( TMemSpySinkType aType )
    {
    iImp->InstallSinkL( aType, KNullDesC );
    }

EXPORT_C void CMemSpyEngine::InstallDebugSinkL()
    {
    iImp->InstallSinkL( ESinkTypeDebug, KNullDesC );
    }

EXPORT_C void CMemSpyEngine::InstallFileSinkL( const TDesC& aRootFolder )
    {
    iImp->InstallSinkL( ESinkTypeFile, aRootFolder );
    }

EXPORT_C void CMemSpyEngine::ListOpenFilesL()
    {
    iImp->ListOpenFilesL();
    }


EXPORT_C void CMemSpyEngine::GetVersion( TVersion& aVersion )
    {
    iImp->GetVersion( aVersion );
    }


EXPORT_C TBool CMemSpyEngine::IsHelperWindowServerSupported()
    {
    return iImp->IsHelperWindowServerSupported();
    }


CMemSpyEngineMidwife& CMemSpyEngine::Midwife()
    {
    return iImp->Midwife();
    }


CMemSpyEngineUndertaker& CMemSpyEngine::Undertaker()
    {
    return iImp->Undertaker();
    }


CMemSpyEngineChunkWatcher& CMemSpyEngine::ChunkWatcher()
    {
    return iImp->ChunkWatcher();
    }


EXPORT_C CMemSpyEngineHelperHeap& CMemSpyEngine::HelperHeap()
    {
    return iImp->HelperHeap();
    }


EXPORT_C CMemSpyEngineHelperStack& CMemSpyEngine::HelperStack()
    {
    return iImp->HelperStack();
    }


EXPORT_C CMemSpyEngineHelperCodeSegment& CMemSpyEngine::HelperCodeSegment()
    {
    return iImp->HelperCodeSegment();
    }


EXPORT_C CMemSpyEngineHelperChunk& CMemSpyEngine::HelperChunk()
    {
    return iImp->HelperChunk();
    }


EXPORT_C CMemSpyEngineHelperThread& CMemSpyEngine::HelperThread()
    {
    return iImp->HelperThread();
    }


EXPORT_C CMemSpyEngineHelperProcess& CMemSpyEngine::HelperProcess()
    {
    return iImp->HelperProcess();
    }


EXPORT_C CMemSpyEngineHelperServer& CMemSpyEngine::HelperServer()
    {
    return iImp->HelperServer();
    }


EXPORT_C CMemSpyEngineHelperActiveObject& CMemSpyEngine::HelperActiveObject()
    {
    return iImp->HelperActiveObject();
    }


EXPORT_C CMemSpyEngineHelperKernelContainers& CMemSpyEngine::HelperKernelContainers()
    {
    return iImp->HelperKernelContainers();
    }


EXPORT_C CMemSpyEngineHelperFileSystem& CMemSpyEngine::HelperFileSystem()
    {
    return iImp->HelperFileSystem();
    }


EXPORT_C CMemSpyEngineHelperECom& CMemSpyEngine::HelperECom()
    {
    return iImp->HelperECom();
    }


EXPORT_C CMemSpyEngineHelperSysMemTracker& CMemSpyEngine::HelperSysMemTracker()
    {
    return iImp->HelperSysMemTracker();
    }


EXPORT_C CMemSpyEngineHelperFbServ& CMemSpyEngine::HelperFbServ()
    {
    return iImp->HelperFbServ();
    }


EXPORT_C CMemSpyEngineHelperROM& CMemSpyEngine::HelperROM()
    {
    return iImp->HelperROM();
    }


EXPORT_C CMemSpyEngineHelperRAM& CMemSpyEngine::HelperRAM()
    {
    return iImp->HelperRAM();
    }


EXPORT_C MMemSpyEngineHelperWindowServer& CMemSpyEngine::HelperWindowServer()
    {
    return iImp->HelperWindowServer();
    }


EXPORT_C CMemSpyEngineHelperCondVar& CMemSpyEngine::HelperCondVar()
    {
    return iImp->HelperCondVar();
    }


RMemSpyDriverClient& CMemSpyEngine::Driver()
    {
    return iImp->Driver();
    }


TInt CMemSpyEngine::ProcessSuspendAndGetErrorLC( TProcessId aId )
    {
    return iImp->ProcessSuspendAndGetErrorLC( aId );
    }


void CMemSpyEngine::ProcessSuspendLC( TProcessId aId )
    {
    iImp->ProcessSuspendLC( aId );
    }


void CMemSpyEngine::ProcessResume()
    {
    iImp->ProcessResume();
    }
    

TProcessId CMemSpyEngine::SuspendedProcessId() const
    {
    return iImp->SuspendedProcessId();
    }



