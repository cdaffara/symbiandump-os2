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

#include <memspy/engine/memspyengineimp.h>

// System includes
#include <e32svr.h>

// Driver includes
#include <memspy/driver/memspydriverclient.h>

// User includes
#include <memspy/engine/memspyengineobjectthread.h>
#include <memspy/engine/memspyengineobjectprocess.h>
#include <memspy/engine/memspyengineobjectcontainer.h>
#include <memspy/engine/memspyengineundertaker.h>
#include "MemSpyEngineChunkWatcher.h"
#include <memspy/engine/memspyengineobserver.h>
#include "MemSpyEngineServer.h"
#include <memspy/engine/memspyengineutils.h>
#include "MemSpyEngineOutputSinkDebug.h"
#include "MemSpyEngineOutputSinkFile.h"
#include <memspy/engine/memspyenginehelperchunk.h>
#include <memspy/engine/memspyenginehelpercodesegment.h>
#include <memspy/engine/memspyenginehelperheap.h>
#include <memspy/engine/memspyenginehelperstack.h>
#include <memspy/engine/memspyenginehelperthread.h>
#include <memspy/engine/memspyenginehelperserver.h>
#include <memspy/engine/memspyenginehelperprocess.h>
#include <memspy/engine/memspyenginehelperactiveobject.h>
#include <memspy/engine/memspyenginehelperkernelcontainers.h>
#include <memspy/engine/memspyenginehelperfilesystem.h>
#include <memspy/engine/memspyenginehelperecom.h>
#include <memspy/engine/memspyenginehelpersysmemtracker.h>
#include <memspy/engine/memspyenginehelperfbserv.h>
#include <memspy/engine/memspyenginehelperrom.h>
#include <memspy/engine/memspyenginehelperram.h>
#include <memspy/engine/memspyenginehelperwindowserver.h>
#include <memspy/engine/memspyenginehelpercondvar.h>

#ifdef _DEBUG
#define LOG(args...) RDebug::Printf(args)
#else
#define LOG(args...)
#endif

CMemSpyEngineImp::CMemSpyEngineImp( RFs& aFsSession, CMemSpyEngine& aEngine )
:   iFsSession( aFsSession ), iEngine( aEngine )
    {
    }


CMemSpyEngineImp::~CMemSpyEngineImp()
    {
    LOG( "CMemSpyEngineImp::~CMemSpyEngineImp() - START" );

    if  ( iMidwife )
        {
        iMidwife->RemoveObserver( *this );
        }

    LOG( "CMemSpyEngineImp::~CMemSpyEngineImp() - deleting helpers..." );
    delete iHelperSysMemTracker;
    delete iServer;
    delete iHelperKernelContainers;
    delete iHelperFbServ;
    delete iHelperHeap;
    delete iHelperStack;
    delete iHelperCodeSegment;
    delete iHelperChunk;
    delete iHelperThread;
    delete iHelperProcess;
    delete iHelperServer;
    delete iHelperActiveObject;
    delete iHelperFileSystem;
    delete iHelperECom;
    delete iHelperROM;
    delete iHelperRAM;
    delete iHelperWindowServer;
    delete iHelperCondVar;
    
    iHelperWindowServerLoader.Close();

    LOG( "CMemSpyEngineImp::~CMemSpyEngineImp() - deleting utilities..." );
    delete iChunkWatcher;
    delete iUndertaker;
    delete iMidwife;

    LOG( "CMemSpyEngineImp::~CMemSpyEngineImp() - destroying containers..." );
    iContainers.ResetAndDestroy();
    iContainers.Close();

    LOG( "CMemSpyEngineImp::~CMemSpyEngineImp() - destroying driver..." );
    if  ( iMemSpyDriver )
        {
        iMemSpyDriver->Close();
        delete iMemSpyDriver;
        }

    LOG( "CMemSpyEngineImp::~CMemSpyEngineImp() - destroying sink..." );
    delete iSink;

    LOG( "CMemSpyEngineImp::~CMemSpyEngineImp() - END" );
    }


void CMemSpyEngineImp::ConstructL()
    {
    LOG( "CMemSpyEngineImp::ConstructL() - START" );
    //
    iFsSession.SetSessionPath( _L("\\") );
    
    iServer = CMemSpyEngineServer::NewL( iEngine );
    
    iMemSpyDriver = new(ELeave) RMemSpyDriverClient();
    const TInt error = Driver().Open();
    User::LeaveIfError( error );
    //
    InstallSinkL( ESinkTypeDebug );
    //
    iUndertaker = CMemSpyEngineUndertaker::NewL( Driver() );
    iUndertaker->AddObserverL( *this );
    //
    iMidwife = CMemSpyEngineMidwife::NewL( Driver() );
    //
    iChunkWatcher = CMemSpyEngineChunkWatcher::NewL( Driver() );
    //
    CMemSpyEngineObjectContainer* container = CMemSpyEngineObjectContainer::NewL( iEngine );
    CleanupStack::PushL( container );
    iContainers.InsertL( container, 0 );
    CleanupStack::Pop( container );
    //
    ConstructHelpersL();
    //
    iHelperSysMemTracker = CMemSpyEngineHelperSysMemTracker::NewL( iEngine );
    iMidwife->AddObserverL( *this );

    LOG( "CMemSpyEngineImp::ConstructL() - END" );
    }


void CMemSpyEngineImp::ConstructHelpersL()
    {
    LOG( "CMemSpyEngineImp::ConstructHelpersL() - START" );

    LOG( "CMemSpyEngineImp::ConstructHelpersL() - Heap..." );
    iHelperHeap = CMemSpyEngineHelperHeap::NewL( iEngine );

    LOG( "CMemSpyEngineImp::ConstructHelpersL() - Stack..." );
    iHelperStack = CMemSpyEngineHelperStack::NewL( iEngine );

    LOG( "CMemSpyEngineImp::ConstructHelpersL() - Code Segments..." );
    iHelperCodeSegment = CMemSpyEngineHelperCodeSegment::NewL( iEngine );

    LOG( "CMemSpyEngineImp::ConstructHelpersL() - Chunk..." );
    iHelperChunk = CMemSpyEngineHelperChunk::NewL( iEngine );

    LOG( "CMemSpyEngineImp::ConstructHelpersL() - Thread..." );
    iHelperThread = CMemSpyEngineHelperThread::NewL( iEngine );

    LOG( "CMemSpyEngineImp::ConstructHelpersL() - Process..." );
    iHelperProcess = CMemSpyEngineHelperProcess::NewL( iEngine );

    LOG( "CMemSpyEngineImp::ConstructHelpersL() - Server..." );
    iHelperServer = CMemSpyEngineHelperServer::NewL( iEngine );

    LOG( "CMemSpyEngineImp::ConstructHelpersL() - AO..." );
    iHelperActiveObject = CMemSpyEngineHelperActiveObject::NewL( iEngine );

    LOG( "CMemSpyEngineImp::ConstructHelpersL() - Kernel Containers..." );
    iHelperKernelContainers = CMemSpyEngineHelperKernelContainers::NewL( iEngine );

    LOG( "CMemSpyEngineImp::ConstructHelpersL() - File System..." );
    iHelperFileSystem = CMemSpyEngineHelperFileSystem::NewL( iEngine );

    LOG( "CMemSpyEngineImp::ConstructHelpersL() - ECOM..." );
    iHelperECom = CMemSpyEngineHelperECom::NewL( iEngine );

    LOG( "CMemSpyEngineImp::ConstructHelpersL() - FBSERV..." );
    iHelperFbServ = CMemSpyEngineHelperFbServ::NewL( iEngine );

    LOG( "CMemSpyEngineImp::ConstructHelpersL() - ROM..." );
    iHelperROM = CMemSpyEngineHelperROM::NewL( iEngine );

    LOG( "CMemSpyEngineImp::ConstructHelpersL() - RAM..." );
    iHelperRAM = CMemSpyEngineHelperRAM::NewL( iEngine );

    LOG( "CMemSpyEngineImp::ConstructHelpersL() - WindowServer..." );
    
    TInt err = iHelperWindowServerLoader.Load( _L("memspywindowserverhelper.dll") );
    LOG( "CMemSpyEngineImp::ConstructHelpersL() - WindowServer load err: %d", err );
    if ( !err )
        {
#ifdef __WINS__ // ordinal is different 
        TLibraryFunction entry = iHelperWindowServerLoader.Lookup( 3 );
#else
        TLibraryFunction entry = iHelperWindowServerLoader.Lookup( 1 );
#endif
        if ( entry != NULL )
            {
			typedef MMemSpyEngineHelperWindowServer* (*TEntryFn)(void);
			TRAP(err, iHelperWindowServer = ((TEntryFn)entry)());
			if (err)
				{
				LOG("err from memspywindowserverhelper.dll - %d", err);
				}
            }
        }
    
    LOG( "CMemSpyEngineImp::ConstructHelpersL() - CondVar..." );
    iHelperCondVar = CMemSpyEngineHelperCondVar::NewL( iEngine );

    LOG( "CMemSpyEngineImp::ConstructHelpersL() - END" );
    }


RFs& CMemSpyEngineImp::FsSession()
    {
    return iFsSession;
    }


CMemSpyEngineObjectContainer& CMemSpyEngineImp::Container()
    {
    __ASSERT_ALWAYS( iContainers.Count() >= 1, MemSpyEngineUtils::Panic( EMemSpyEnginePanicInvalidContainer1 ) );
    CMemSpyEngineObjectContainer* headContainer = iContainers[ 0 ];
    return *headContainer;
    }


const CMemSpyEngineObjectContainer& CMemSpyEngineImp::Container() const
    {
    __ASSERT_ALWAYS( iContainers.Count() >= 1, MemSpyEngineUtils::Panic( EMemSpyEnginePanicInvalidContainer2 ) );
    const CMemSpyEngineObjectContainer* headContainer = iContainers[ 0 ];
    return *headContainer;
    }


void CMemSpyEngineImp::SetObserver( MMemSpyEngineObserver* aObserver )
    {
    iObserver = aObserver;
    }


void CMemSpyEngineImp::NotifyContainerChangeL()
    {
    if  ( iObserver )
        {
        iObserver->HandleMemSpyEngineEventL( MMemSpyEngineObserver::EHandleThreadsOrProcessesChanged, &Container() );
        }
    }


void CMemSpyEngineImp::NotifyClientServerOperationRequestL( TInt aType )
    {
    if  ( iObserver )
        {
        iObserver->HandleMemSpyEngineEventL( MMemSpyEngineObserver::EHandleClientServerOperationRequest, reinterpret_cast< TAny* >( aType ) );
        }
    }


CMemSpyEngineOutputSink& CMemSpyEngineImp::Sink()
    {
    return *iSink;
    }


TMemSpySinkType CMemSpyEngineImp::SinkType()
    {
    return iSink->Type();
    }

void CMemSpyEngineImp::InstallSinkL( TMemSpySinkType aType )
	{
	InstallSinkL( aType, KNullDesC );
	}

void CMemSpyEngineImp::InstallSinkL( TMemSpySinkType aType, const TDesC& aRootFolder )
    {
    LOG( "CMemSpyEngineImp::InstallSinkL() - START - switching sink from %d to %d...", (iSink != NULL ? iSink->Type() : -1), aType );
    //
    CMemSpyEngineOutputSink* sink = NULL;
    //
    switch( aType )
        {
    case ESinkTypeDebug:
        sink = CMemSpyEngineOutputSinkDebug::NewL( iEngine );
        break;
    case ESinkTypeFile:
        sink = CMemSpyEngineOutputSinkFile::NewL( iEngine, aRootFolder );
        break;
        }
    //
    delete iSink;
    iSink = sink;
    //
    LOG( "CMemSpyEngineImp::InstallSinkL() - END - sink type: %d", iSink->Type() );
    }


void CMemSpyEngineImp::ListOpenFilesL()
    {
    HelperFileSystem().ListOpenFilesL();
    }


void CMemSpyEngineImp::GetVersion( TVersion& aVersion )
    {
    iMemSpyDriver->GetVersion( aVersion );
    }


TBool CMemSpyEngineImp::IsHelperWindowServerSupported()
    {
    return iHelperWindowServer != NULL;
    }


CMemSpyEngineMidwife& CMemSpyEngineImp::Midwife()
    {
    return *iMidwife;
    }


CMemSpyEngineUndertaker& CMemSpyEngineImp::Undertaker()
    {
    return *iUndertaker;
    }


CMemSpyEngineChunkWatcher& CMemSpyEngineImp::ChunkWatcher()
    {
    return *iChunkWatcher;
    }


CMemSpyEngineHelperHeap& CMemSpyEngineImp::HelperHeap()
    {
    return *iHelperHeap;
    }


CMemSpyEngineHelperStack& CMemSpyEngineImp::HelperStack()
    {
    return *iHelperStack;
    }


CMemSpyEngineHelperCodeSegment& CMemSpyEngineImp::HelperCodeSegment()
    {
    return *iHelperCodeSegment;
    }


CMemSpyEngineHelperChunk& CMemSpyEngineImp::HelperChunk()
    {
    return *iHelperChunk;
    }


CMemSpyEngineHelperThread& CMemSpyEngineImp::HelperThread()
    {
    return *iHelperThread;
    }


CMemSpyEngineHelperProcess& CMemSpyEngineImp::HelperProcess()
    {
    return *iHelperProcess;
    }


CMemSpyEngineHelperServer& CMemSpyEngineImp::HelperServer()
    {
    return *iHelperServer;
    }


CMemSpyEngineHelperActiveObject& CMemSpyEngineImp::HelperActiveObject()
    {
    return *iHelperActiveObject;
    }


CMemSpyEngineHelperKernelContainers& CMemSpyEngineImp::HelperKernelContainers()
    {
    return *iHelperKernelContainers;
    }


CMemSpyEngineHelperFileSystem& CMemSpyEngineImp::HelperFileSystem()
    {
    return *iHelperFileSystem;
    }


CMemSpyEngineHelperECom& CMemSpyEngineImp::HelperECom()
    {
    return *iHelperECom;
    }


CMemSpyEngineHelperSysMemTracker& CMemSpyEngineImp::HelperSysMemTracker()
    {
    return *iHelperSysMemTracker;
    }


CMemSpyEngineHelperFbServ& CMemSpyEngineImp::HelperFbServ()
    {
    return *iHelperFbServ;
    }


CMemSpyEngineHelperROM& CMemSpyEngineImp::HelperROM()
    {
    return *iHelperROM;
    }


CMemSpyEngineHelperRAM& CMemSpyEngineImp::HelperRAM()
    {
    return *iHelperRAM;
    }


MMemSpyEngineHelperWindowServer& CMemSpyEngineImp::HelperWindowServer()
    {
    return *iHelperWindowServer;
    }


CMemSpyEngineHelperCondVar& CMemSpyEngineImp::HelperCondVar()
    {
    return *iHelperCondVar;
    }


RMemSpyDriverClient& CMemSpyEngineImp::Driver()
    {
    return *iMemSpyDriver;
    }


TInt CMemSpyEngineImp::ProcessSuspendAndGetErrorLC( TProcessId aId )
    {
    __ASSERT_ALWAYS( iSuspendedProcess == aId || iSuspendedProcess == 0, MemSpyEngineUtils::Panic( EMemSpyEnginePanicSuspendRequest1 ) );

    TInt errorOrCount = Driver().ProcessThreadsSuspend( aId );
    if  ( errorOrCount >= 0 )
        {
        if  ( errorOrCount == 1 )
            {
            iSuspendedProcess = aId;
            }
        else
            {
            // Suspending already suspended process
            __ASSERT_ALWAYS( iSuspendedProcess == aId, MemSpyEngineUtils::Panic( EMemSpyEnginePanicSuspendRequest2 ) );
            }
        //
        CleanupStack::PushL( TCleanupItem( ResumeSuspendedProcess, this ) );

        if  ( errorOrCount == 1 )
            {
            iSink->ProcessSuspendedL( aId );
            }
            
        // At this point, all was well
        errorOrCount = KErrNone;
        }

    return errorOrCount;
    }


void CMemSpyEngineImp::ProcessSuspendLC( TProcessId aId )
    {
    const TInt error = ProcessSuspendAndGetErrorLC( aId );
    User::LeaveIfError( error );
    }


void CMemSpyEngineImp::ProcessResume()
    {
    const TUint id = iSuspendedProcess;
    //
    const TInt errorOrCount = Driver().ProcessThreadsResume( id );
    //
    if  ( errorOrCount == 0 )
        {
        iSink->ProcessResumed( id );
        iSuspendedProcess = 0;
        }
    }
    

TProcessId CMemSpyEngineImp::SuspendedProcessId() const
    {
    return TProcessId( iSuspendedProcess );
    }


void CMemSpyEngineImp::ResumeSuspendedProcess( TAny* aSelf )
    {
    CMemSpyEngineImp* self = reinterpret_cast< CMemSpyEngineImp* >( aSelf );
    self->ProcessResume();
    }


void CMemSpyEngineImp::ThreadIsDeadL( const TThreadId& aId, const RThread& aThread )
    {
    if  ( aThread.Handle() != KNullHandle )
        {
        const TFullName name( aThread.FullName() );
        //
        TBuf<128> exitInfo;
        const TExitCategoryName exitCategory( aThread.ExitCategory() );
        CMemSpyThread::AppendExitInfo( exitInfo, aThread.ExitType(), aThread.ExitReason(), exitCategory );
        //
        _LIT( KMemSpyEventInfoText, "[MemSpy] Thread died: %S [%d - 0x%04x] %S" );
        Sink().OutputLineFormattedL( KMemSpyEventInfoText, &name, (TUint32) aId, (TUint32) aId, &exitInfo );
        }
    }
    

void CMemSpyEngineImp::ProcessIsDeadL( const TProcessId& aId, const RProcess& aProcess )
    {
    if  ( aProcess.Handle() != KNullHandle )
        {
        const TFullName name( aProcess.FullName() );
        //
        TBuf<128> exitInfo;
        const TExitCategoryName exitCategory( aProcess.ExitCategory() );
        CMemSpyThread::AppendExitInfo( exitInfo, aProcess.ExitType(), aProcess.ExitReason(), exitCategory );
        //
        _LIT( KMemSpyEventInfoText, "[MemSpy] Process died: %S [%d - 0x%04x] %S" );
        Sink().OutputLineFormattedL( KMemSpyEventInfoText, &name, (TUint32) aId, (TUint32) aId, &exitInfo );
        }
    }


void CMemSpyEngineImp::ThreadIsBornL( const TThreadId& aId, const RThread& aThread )
    {
    if  ( aThread.Handle() != KNullHandle )
        {
        const TFullName name( aThread.FullName() );
        //
        _LIT( KMemSpyEventInfoText, "[MemSpy] Thread created: %S (%d / 0x%08x)" );
        Sink().OutputLineFormattedL( KMemSpyEventInfoText, &name, (TUint32) aId, (TUint32) aId );
        }
    }


void CMemSpyEngineImp::ProcessIsBornL( const TProcessId& aId, const RProcess& aProcess )
    {
    if  ( aProcess.Handle() != KNullHandle )
        {
        const TFullName name( aProcess.FullName() );
        //
        _LIT( KMemSpyEventInfoText, "[MemSpy] Process created: %S (%d / 0x%08x)" );
        Sink().OutputLineFormattedL( KMemSpyEventInfoText, &name, (TUint32) aId, (TUint32) aId );
        }
    }



