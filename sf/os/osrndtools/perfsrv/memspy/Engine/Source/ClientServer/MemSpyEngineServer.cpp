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

#include "MemSpyEngineServer.h"

// System includes
#include <e32svr.h>
#include <w32std.h>
//#include <coedef.h>  

// User includes
#include <memspy/engine/memspyengine.h>
#include <memspy/engine/memspyenginelogger.h>
#include <memspyengineclientinterface.h>
#include <memspy/engine/memspyengineobjectthread.h>
#include <memspy/engine/memspyengineobjectprocess.h>
#include <memspy/engine/memspyengineobjectcontainer.h>
#include <memspy/engine/memspyenginehelperchunk.h>
#include <memspy/engine/memspyenginehelpercodesegment.h>
#include <memspy/engine/memspyenginehelperheap.h>
#include <memspy/engine/memspyenginehelperstack.h>
#include <memspy/engine/memspyenginehelperthread.h>
#include <memspy/engine/memspyenginehelperprocess.h>
#include <memspy/engine/memspyenginehelperfilesystem.h>
#include <memspy/engine/memspyenginehelperram.h>
#include <memspy/engine/memspyenginehelpersysmemtracker.h>
#include <memspy/engine/memspyenginehelpersysmemtrackerconfig.h>
#include <memspy/engine/memspyenginehelperkernelcontainers.h>
#include <memspy/engine/memspyengineobjectthreadinfocontainer.h>
#include <memspy/engine/memspyengineobjectthreadinfoobjects.h>
#include <memspy/engine/memspyenginehelpersysmemtrackercycle.h>
#include <memspy/engine/memspyenginehelperserver.h>
#include <memspy/engine/memspyenginehelperecom.h>

#include <memspy/engine/memspyprocessdata.h>
#include <memspy/engine/memspythreaddata.h>
#include <memspy/engine/memspykernelobjectdata.h>
#include <memspy/engine/memspythreadinfoitemdata.h>
#include <memspy/engine/memspymemorytrackingcycledata.h>
#include <memspy/engine/memspyengineoutputsink.h>
#include <memspy/engine/memspyenginehelperactiveobject.h>
#include <memspy/engine/memspyserverdata.h>
#include <memspysession.h>
#include <memspy/engine/memspyecomdata.h>

inline CShutdown::CShutdown() :CTimer(-1)
    {
    CActiveScheduler::Add(this);
    }

inline void CShutdown::ConstructL()
    {
    CTimer::ConstructL();
    }

inline void CShutdown::Start()
    {
    After(KMyShutdownDelay);
    }

void CShutdown::RunL()
    //
    // Initiate server exit when the timer expires
    //
    {
    CActiveScheduler::Stop();
    }

CMemSpyEngineServer::CMemSpyEngineServer( CMemSpyEngine& aEngine )
:   CServer2( EPriorityNormal ), iEngine( aEngine )
    {
    }


CMemSpyEngineServer::~CMemSpyEngineServer()
    {
    }


void CMemSpyEngineServer::ConstructL()
    {
    StartL( KMemSpyServerName );
        
    iShutdown.ConstructL();
    // ensure that the server still exits even if the 1st client fails to connect
    iShutdown.Start();
    }


CMemSpyEngineServer* CMemSpyEngineServer::NewL( CMemSpyEngine& aEngine )
    {
    CMemSpyEngineServer* self = new(ELeave) CMemSpyEngineServer( aEngine );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


CSession2* CMemSpyEngineServer::NewSessionL( const TVersion& aVersion, const RMessage2& aMessage ) const
    {
    if  ( aVersion.iMajor != KMemSpyClientServerVersion )
        {
        RDebug::Printf( "[MemSpy] CMemSpyEngineSession::NewSessionL() - BAD VERSION" );
        User::Leave( KErrNotSupported );
        }
    //
    CMemSpyEngineSession* session = CMemSpyEngineSession::NewL( iEngine, aMessage );
	return session;
    }

void CMemSpyEngineServer::AddSession(TBool aCliRequest)
    {
    if (aCliRequest)
        {
        iCliConnected = ETrue;
        }
    else
        {
        ++iSessionCount;
        }
    iShutdown.Cancel();
    }

void CMemSpyEngineServer::DropSession(TBool aCliRequest)
    {
    if (!aCliRequest)
        {
        --iSessionCount;
        }
    
    if (iSessionCount == 0 && !iCliConnected)
        {
        iShutdown.Start();
        }
    }
























CMemSpyEngineSession::CMemSpyEngineSession( CMemSpyEngine& aEngine )
:   iEngine( aEngine )
    {
    }


CMemSpyEngineSession::~CMemSpyEngineSession()
    {
#ifdef _DEBUG
    TPtrC pThreadName( KNullDesC );
    if  ( iClientThreadName )
        {
        pThreadName.Set( *iClientThreadName );
        }

    RDebug::Print( _L("[MemSpy] CMemSpyEngineSession::~CMemSpyEngineSession() - DEAD SESSION - this: 0x%08x, id: %4d, name: %S"), this, iClientThreadId, iClientThreadName );
#endif

    delete iClientThreadName;
        
    Server().DropSession(iIsCliRequest);
    }


void CMemSpyEngineSession::ConstructL( const RMessage2& aMessage )
    {
	RThread thread;
    const TInt error = aMessage.Client( thread );
    CleanupClosePushL( thread );

    TRACE( RDebug::Printf( "[MemSpy] CMemSpyEngineSession::ConstructL() - this: 0x%08x - opening client thread - err: %d", this, error ) );

    User::LeaveIfError( error );

    const TFullName threadName( thread.FullName() );
    iClientThreadName = threadName.AllocL();
    iClientThreadId = thread.Id();

    CleanupStack::PopAndDestroy( &thread );
    
    const TUid KCliUid3 = { 0x2002129D };
    iIsCliRequest = aMessage.SecureId() == TSecureId(KCliUid3);
    
    TRACE( RDebug::Print( _L("[MemSpy] CMemSpyEngineSession::ConstructL() - NEW SESSION - this: 0x%08x, id: %4d, client: %S"), this, iClientThreadId, iClientThreadName ) );
    }

void CMemSpyEngineSession::CreateL()
    {   
    Server().AddSession(iIsCliRequest);
    }

CMemSpyEngineSession* CMemSpyEngineSession::NewL( CMemSpyEngine& aEngine, const RMessage2& aMessage )
    {
    CMemSpyEngineSession* self = new(ELeave) CMemSpyEngineSession( aEngine );
    CleanupStack::PushL( self );
    self->ConstructL( aMessage );
    CleanupStack::Pop( self );
    return self;
    }


void CMemSpyEngineSession::ServiceL( const RMessage2& aMessage )
    {
    TRACE( RDebug::Print( _L("[MemSpy] CMemSpyEngineSession::ServiceL() - START - this: 0x%08x, fn: 0x%08x, id: %4d, client: %S"), this, aMessage.Function(), iClientThreadId, iClientThreadName ) );

    TRAPD( error, DoServiceL( aMessage ) );
    if  ( error != KErrNone )
        {
        RDebug::Print( _L("[MemSpy] CMemSpyEngineSession::ServiceL() - SERVICE ERROR - this: 0x%08x, fn: %d, err: %d, client: %S"), this, aMessage.Function(), error, iClientThreadName );
        }
    
    if ((aMessage.Function() & KMemSpyOpFlagsAsyncOperation) == 0 || error != KErrNone)
    	{
		aMessage.Complete( error );
    	}

    TRACE( RDebug::Print( _L("[MemSpy] CMemSpyEngineSession::ServiceL() - END - this: 0x%08x, fn: 0x%08x, id: %4d, client: %S"), this, aMessage.Function(), iClientThreadId, iClientThreadName ) );
	}

// ---------------------------------------------------------
// DoServiceL( const RMessage2& aMessage )
// ---------------------------------------------------------
//
void CMemSpyEngineSession::DoServiceL( const RMessage2& aMessage )
	{
	TInt function = aMessage.Function() & KMemSpyOpFlagsTypeMask;
	if (function >= EMemSpyClientServerOpMarkerUiFirst && 
		function < EMemSpyClientServerOpMarkerUiLast)
		
		DoUiServiceL(aMessage);
	else
		DoCmdServiceL(aMessage);
	}
// ---------------------------------------------------------
// DoUiServiceL( const RMessage2& aMessage )
// ---------------------------------------------------------
//
void CMemSpyEngineSession::DoUiServiceL( const RMessage2& aMessage )
    {
	switch (aMessage.Function() & KMemSpyOpFlagsTypeMask)
		{
	    case EMemSpyClientServerOpGetOutputSink:
	        {
	        TMemSpySinkType sink = iEngine.SinkType();
	        TPckgBuf<TMemSpySinkType> type( sink );
	        
	        aMessage.WriteL( 0, type );
	        break;
	        }
		case EMemSpyClientServerOpGetProcessCount:
			{
			aMessage.WriteL(0, TPckgBuf<TInt>(iEngine.Container().Count()));
			break;
			}
		case EMemSpyClientServerOpGetProcesses:
			{
			CMemSpyEngineObjectContainer& list = iEngine.Container();
			
			TPckgBuf<TInt> a0;
			aMessage.ReadL(0, a0);
			TInt realCount = Min(a0(), list.Count());
			
			for(TInt i=0, offset = 0; i<realCount; i++, offset += sizeof(TMemSpyProcessData))
				{
				CMemSpyProcess& process = iEngine.Container().At(i);
				TMemSpyProcessData data;
				data.iIsDead = process.IsDead();
				data.iId = process.Id();
				data.iName.Copy(process.Name().Left(KMaxFullName));
				data.iThreadCount = process.Count();
				data.iPriority = process.Priority();
				data.iExitType = process.ExitType();
				data.iExitReason = process.ExitReason();
				data.iExitCategory = process.ExitCategory();
				data.iSID = process.SID();
				
				TPckgBuf<TMemSpyProcessData> buffer(data);
				aMessage.WriteL(1, buffer, offset);
				}
			
			a0 = list.Count();
			aMessage.WriteL(0, a0);

			break;
			}
		case EMemSpyClienServerOpGetProcessIdByName:
			{
			TFullName processName;
			aMessage.ReadL(0, processName);
			
			TBool found(EFalse);
			
			for (TInt i=0; i<iEngine.Container().Count(); i++)
				{
				CMemSpyProcess& process = iEngine.Container().At(i);
				if (process.Name().FindF(processName) >= 0)
					{
					found = ETrue;
					TPckgBuf<TProcessId> procId(process.Id());
					aMessage.WriteL(1, procId);
					}
				}
			
			if (!found)
				{
				User::Leave(KErrNotFound);
				}
			
			break;
			}
		case EMemSpyClientServerOpProcessSystemPermanentOrCritical:
			{
			TBool ret = EFalse;
			TPckgBuf<TProcessId> id;
			aMessage.ReadL( 0, id );
			
			CMemSpyEngineObjectContainer& container = iEngine.Container();
			CMemSpyProcess& process = container.ProcessByIdL( id() );
			
			if  ( process.IsSystemPermanent() || process.IsSystemCritical() )
				{
				ret = ETrue;
				}
			TPckgBuf<TBool> retBuf( ret );
			aMessage.WriteL( 1, retBuf );
			
			break;
			}
		case EMemSpyClientServerOpEndProcess:
			{
			TPckgBuf<TProcessId> id;
			aMessage.ReadL( 0, id );
			TPckgBuf<TMemSpyEndType> type;
			aMessage.ReadL( 1, type );
					
			CMemSpyEngineObjectContainer& container = iEngine.Container();			
			CMemSpyProcess& process = container.ProcessByIdL( id() );
									
			switch ( type() )
				{
				case ETerminate:
					{
					process.TerminateL();
					break;
					}
				case EPanic:
					{
					process.PanicL();
					break;
					}
				case EKill:
					{
					process.KillL();
					break;
					}
				}																
			break;
			}
		case EMemSpyClientServerOpSwitchToProcess:
			{/*
			TInt wgCount;
			RWsSession wsSession;
			User::LeaveIfError( wsSession.Connect() );
			CleanupClosePushL( wsSession );
			User::LeaveIfError( wgCount = wsSession.NumWindowGroups() );
			RArray<RWsSession::TWindowGroupChainInfo> wgArray;
			CleanupClosePushL( wgArray );
			User::LeaveIfError( wsSession.WindowGroupList( &wgArray ) );
			TApaTask task( wsSession );
			TBool brought( EFalse );
			TInt wgId( KErrNotFound );
			TThreadId threadId;
			
			TPckgBuf<TProcessId> id;
			aMessage.ReadL( 0, id );
			CMemSpyEngineObjectContainer& container = iEngine.Container();
			CMemSpyProcess& process = container.ProcessByIdL( id() );
			
			// loop trough threads in a process
			for ( TInt i = 0; i < process.MdcaCount(); i++ )
				{
				TInt wgCountLocal = wgCount;
							
				// loop trough all window groups and see if a thread id matches
				while( !brought && wgCountLocal-- )
					{
					wgId = wgArray[wgCountLocal].iId;
					User::LeaveIfError( wsSession.GetWindowGroupClientThreadId( wgId, threadId ) );
					if ( threadId == process.At( i ).Id() )
						{
						CApaWindowGroupName* wgName = CApaWindowGroupName::NewLC( wsSession, wgId );
						task.SetWgId( wgId );
						if ( !wgName->Hidden() && task.Exists() )
							{
							task.BringToForeground();
							brought = ETrue;                        
							}
						CleanupStack::PopAndDestroy( wgName );
						}
					}
				}
			
			TPckgBuf<TBool> ret( brought );
			aMessage.WriteL( 1, ret );
			
			break;*/
			}
		case EMemSpyClientServerOpGetThreadCount:
			{
			TPckgBuf<TProcessId> pid;
			aMessage.ReadL(1, pid);
			CMemSpyProcess& process = iEngine.Container().ProcessByIdL(pid());
			aMessage.WriteL(0, TPckgBuf<TInt>(process.Count()));
			break;
			}
		case EMemSpyClientServerOpGetThreads:
			{
			TPckgBuf<TProcessId> pid;
			aMessage.ReadL(2, pid);
			
			CMemSpyProcess& list = iEngine.Container().ProcessByIdL(pid());
			
			TPckgBuf<TInt> a0;
			aMessage.ReadL(0, a0);
			TInt realCount = Min(a0(), list.Count());
						
			for(TInt i=0, offset = 0; i<realCount; i++, offset += sizeof(TMemSpyThreadData))
				{
				CMemSpyThread& thread = list.At(i);
				
				TMemSpyThreadData data;
				data.iId = thread.Id();
				data.iName.Copy(thread.Name().Left(KMaxFullName));
				data.iThreadPriority = thread.Priority();
				
				TPckgBuf<TMemSpyThreadData> buffer(data);
				aMessage.WriteL(1, buffer, offset);
				}
			
			a0 = list.Count();
			aMessage.WriteL(0, a0);

			break;
			}
		case EMemSpyClientServerOpSetThreadPriority:
			{
			TPckgBuf<TThreadId> tid;
			TPckgBuf<TInt> priority;
			aMessage.ReadL(0, tid);
			aMessage.ReadL(1, priority);
			
			CMemSpyProcess* process = NULL;
			CMemSpyThread* thread = NULL; 
			User::LeaveIfError(iEngine.Container().ProcessAndThreadByThreadId(tid(), process, thread));
			
			if (thread)
				{				
				thread->SetPriorityL(static_cast<TThreadPriority>(priority()));
				}					
			break;
			}
		case EMemSpyClientServerOpThreadSystemPermanentOrCritical:
			{
			TPckgBuf<TThreadId> id;
			aMessage.ReadL( 0, id );
			
			CMemSpyEngineObjectContainer& container = iEngine.Container();            
			CMemSpyProcess* process = NULL;
			CMemSpyThread* thread = NULL; 
			User::LeaveIfError( container.ProcessAndThreadByThreadId( id(), process, thread ) );
			
			TBool ret = thread && ( thread->IsSystemPermanent() || thread->IsSystemCritical() );
			
			TPckgBuf<TBool> retBuf( ret );
			aMessage.WriteL( 1, retBuf );
							
			break;
			}
		case EMemSpyClientServerOpEndThread:
			{
			TPckgBuf<TThreadId> id;
			aMessage.ReadL( 0, id );
			TPckgBuf<TMemSpyEndType> type;
			aMessage.ReadL( 1, type );
			
			CMemSpyEngineObjectContainer& container = iEngine.Container();
			CMemSpyProcess* process = NULL;
			CMemSpyThread* thread = NULL; 
			User::LeaveIfError( container.ProcessAndThreadByThreadId( id(), process, thread ) );
			
			if( thread )
				{
				switch ( type() )
					{
					case ETerminate:
						{
						thread->TerminateL();
						break;
						}
					case EPanic:
						{
						thread->PanicL();
						break;
						}
					case EKill:
						{
						thread->KillL();
						break;
						}
					}				
				}			
			break;
			}
		case EMemSpyClientServerOpSwitchToThread:
			{
			TInt wgCount;
			RWsSession wsSession;
			User::LeaveIfError( wsSession.Connect() );
			CleanupClosePushL( wsSession );
			User::LeaveIfError( wgCount = wsSession.NumWindowGroups() );
			RArray<RWsSession::TWindowGroupChainInfo> wgArray;
			CleanupClosePushL( wgArray );
			User::LeaveIfError( wsSession.WindowGroupList( &wgArray ) );
						
			//TApaTask task( wsSession );
			TBool brought = EFalse;
			TInt wgId( KErrNotFound );
			TThreadId threadId;
					
			TPckgBuf<TThreadId> id;
			aMessage.ReadL( 0, id );
					
			// loop trough all window groups and see if a thread id matches
			while( !brought && wgCount-- )
				{
				wgId = wgArray[wgCount].iId;
				User::LeaveIfError( wsSession.GetWindowGroupClientThreadId( wgId, threadId ) );
				if ( threadId == id() )
					{
					TInt handle = wsSession.GetWindowGroupHandle( wgId );
					if( handle == KErrNone )
						{
						RWindowGroup* group = new (ELeave) RWindowGroup();
						group->Construct( handle );        
						group->SetOrdinalPosition( 0 ); //foreground
						
						brought = ETrue;
						
						group->Close();
						delete group;												
						}
					}					    					    					    										    										
					//task.SetWgId( wgId );
					//if ( !wgName->Hidden() && task.Exists() )
					//	{
					//	task.BringToForeground();
					//	brought = ETrue;                        
					//	}					
					}
													
			TPckgBuf<TBool> ret( brought );
			aMessage.WriteL( 1, ret );															
					
			break;
			}		
		case EMemSpyClientServerOpGetInfoItemType:
			{
			
			TPckgBuf<TInt> index;
			aMessage.ReadL( 0, index );			
			TPckgBuf<TThreadId> id;
			aMessage.ReadL( 1, id);
								
			CMemSpyEngineObjectContainer& container = iEngine.Container();            
			CMemSpyProcess* process = NULL; //not needed
			CMemSpyThread* thread = NULL; 
			User::LeaveIfError( container.ProcessAndThreadByThreadId( id(), process, thread ) );
		            
			CMemSpyThreadInfoContainer& threadInfoContainer = thread->InfoContainerForceSyncronousConstructionL();                        
			TMemSpyThreadInfoItemType retType = threadInfoContainer.Item( index() ).Type();
			
			TPckgBuf<TMemSpyThreadInfoItemType> ret( retType );
			aMessage.WriteL( 2, ret );			
			
			break;
			}
		case EMemSpyClientServerOpGetThreadInfoItemsCount:
			{
			TPckgBuf<TThreadId> id;
			aMessage.ReadL( 0, id );
			TPckgBuf<TMemSpyThreadInfoItemType> type;
			aMessage.ReadL( 1, type );					 
			
			CMemSpyEngineObjectContainer& container = iEngine.Container();            
			CMemSpyProcess* process = NULL;
			CMemSpyThread* thread = NULL; 
			
			container.ProcessAndThreadByThreadId( id(), process, thread );
			
			CMemSpyThreadInfoContainer& threadInfoContainer = thread->InfoContainerForceSyncronousConstructionL();                 
								
			CMemSpyThreadInfoItemBase& threadInfoItemBase = threadInfoContainer.Item( type() );
				    
			TInt count = threadInfoItemBase.MdcaCount();		    
			TPckgBuf<TInt> tempret( count );
			aMessage.WriteL( 2, tempret );
		
			break;
			}		
		case EMemSpyClientServerOpGetThreadInfoItems:
			{
			TPckgBuf<TInt> count;
			aMessage.ReadL( 0, count );						
			TPckgBuf<TThreadId> id;
			aMessage.ReadL( 1, id );
			TPckgBuf<TMemSpyThreadInfoItemType> type;
			aMessage.ReadL( 2, type );			
			
			CMemSpyEngineObjectContainer& container = iEngine.Container();            
			CMemSpyProcess* process = NULL;
			CMemSpyThread* thread = NULL; 
			User::LeaveIfError( container.ProcessAndThreadByThreadId( id() , process, thread ) );
							  
			CMemSpyThreadInfoContainer& threadInfoContainer = thread->InfoContainerForceSyncronousConstructionL();      

			CMemSpyThreadInfoItemBase& threadInfoItemBase = threadInfoContainer.Item( type() ); //get ThreadInfoItemBaseByType
			
			TInt itemCount = Min(count(), threadInfoItemBase.MdcaCount());
								
			for( TInt i=0, offset = 0; i<itemCount; i++, offset += sizeof( TMemSpyThreadInfoItemData ) )
				{
				TMemSpyThreadInfoItemData data;
				
				TPtrC caption(threadInfoItemBase.MdcaPoint(i).Mid(1));
				TInt tabPos = caption.Locate('\t');
				if (tabPos != KErrNotFound)
					caption.Set(caption.Left(tabPos));
				
				TPtrC value(threadInfoItemBase.MdcaPoint(i));
				tabPos = value.LocateReverse('\t');
				if (tabPos != KErrNotFound)
					value.Set(value.Mid(tabPos + 1));
												
				data.iCaption.Copy( caption.Left(64) );
				data.iValue.Copy( value.Left(32) );
							
				TPckgBuf<TMemSpyThreadInfoItemData> buffer(data);
				aMessage.WriteL(3, buffer, offset);				
				}			
			aMessage.WriteL(0, count);
					
			break;
			}
			
		case EMemSpyClientServerOpGetProcessIdByThreadId:
			{
			TPckgBuf<TThreadId> tid;
			aMessage.ReadL( 1, tid );					
			
			CMemSpyProcess* process = NULL;
			CMemSpyThread* thread = NULL;
			//
			const TInt error = iEngine.Container().ProcessAndThreadByThreadId( tid(), process, thread );
			
			TProcessId pid = process->Id();
			
			TPckgBuf<TProcessId> ret(pid);
			aMessage.WriteL( 0, ret );
									
			break;
			}
			
		// --- KernelObjects related functions ---
		case EMemSpyClientServerOpGetKernelObjectCount:
			{
			TInt iCount = EMemSpyDriverContainerTypeLast - EMemSpyDriverContainerTypeFirst + 1;
			TPckgBuf<TInt> ret( iCount );
			aMessage.WriteL(0, ret);			
			break;
			}
		case EMemSpyClientServerOpGetKernelObjects:
			{
			TPckgBuf<TInt> count;
			aMessage.ReadL(0, count);
			
			CMemSpyEngineGenericKernelObjectContainer* model = iEngine.HelperKernelContainers().ObjectsAllL(); //contains all the objects
			CleanupStack::PushL( model );
			
			for( TInt i=0, offset = 0; i<count(); i++, offset += sizeof( TMemSpyKernelObjectData ) )
				{
				TMemSpyKernelObjectData data;
				
				TPtrC name(model->At(i).Name().Mid(1));
				TInt tabPos = name.Locate('\t');
				if (tabPos != KErrNotFound)
					name.Set(name.Left(tabPos));
												
				data.iName.Copy(name);
				data.iType = model->At(i).Type();
				data.iCount = model->At(i).Count();											
				data.iSize = model->At(i).Count() * model->At(i).Count();

				TPckgBuf<TMemSpyKernelObjectData> buffer(data);
				aMessage.WriteL(1, buffer, offset);
				}			
			aMessage.WriteL(0, count);
			CleanupStack::PopAndDestroy( model );
			break;
			}
		case EMemSpyClientServerOpGetKernelObjectItemCount:
			{
			TPckgBuf<TMemSpyDriverContainerType> tempType;
			aMessage.ReadL(1, tempType); //get type of kernel object
			TMemSpyDriverContainerType type = tempType();
			
			CMemSpyEngineHelperKernelContainers& kernelContainerManager = iEngine.HelperKernelContainers();
			CMemSpyEngineGenericKernelObjectList* iObjectList = kernelContainerManager.ObjectsForSpecificContainerL( type );			
			
			TInt count = iObjectList->Count();
			TPckgBuf<TInt> ret( count );
			aMessage.WriteL( 0, ret );
						
			break;
			}
		case EMemSpyClientServerOpGetKernelObjectItems:
			{
			TPckgBuf<TInt> count;
			TPckgBuf<TMemSpyDriverContainerType> tempType;
			aMessage.ReadL( 0, count ); //get count of items
			aMessage.ReadL(1, tempType); //get type of kernel object
			TInt c = count();
						
			CMemSpyEngineHelperKernelContainers& kernelContainerManager = iEngine.HelperKernelContainers();
			CMemSpyEngineGenericKernelObjectList* iObjectList = kernelContainerManager.ObjectsForSpecificContainerL( tempType() );			
			
			for( TInt i=0, offset = 0; i<c; i++, offset += sizeof( TMemSpyDriverHandleInfoGeneric ) )
				{
				TMemSpyDriverHandleInfoGeneric data;								
															
				data = iObjectList->At( i );
				
				TPckgBuf<TMemSpyDriverHandleInfoGeneric> buffer(data);
				aMessage.WriteL(2, buffer, offset);
				}			
					
			break;
			}
			
		case EMemSpyClientServerOpOutputAllContainerContents:
			{
			CMemSpyEngineHelperKernelContainers& kernelContainerManager = iEngine.HelperKernelContainers();
			CMemSpyEngineGenericKernelObjectContainer* model = kernelContainerManager.ObjectsAllL();
			
			model->OutputL( iEngine.Sink() );

			break;
			}
			
		case EMemSpyClientServerOpDumpKernelHeap:
			{
		    iEngine.HelperHeap().OutputHeapDataKernelL();
			
			break;
			}
			
		case EMemSpyClientServerOpOutputInfoHandles:
			{
			TPckgBuf<TThreadId> id;
			aMessage.ReadL(0, id);
			CMemSpyEngineObjectContainer& container = iEngine.Container();            
			CMemSpyProcess* process = NULL;
			CMemSpyThread* thread = NULL; 
			User::LeaveIfError( container.ProcessAndThreadByThreadId( id() , process, thread ) );
										  
			CMemSpyThreadInfoContainer& threadInfoContainer = thread->InfoContainerForceSyncronousConstructionL();
			
			threadInfoContainer.PrintL();
			
			break;
			}
			
		case EMemSpyClientServerOpOutputAOList:
			{
			TPckgBuf<TThreadId> id;
			TPckgBuf<TMemSpyThreadInfoItemType> type;
			aMessage.ReadL(0, id);
			aMessage.ReadL(1, type);
			
			CMemSpyEngineObjectContainer& container = iEngine.Container();            
			CMemSpyProcess* process = NULL;
			CMemSpyThread* thread = NULL; 
			User::LeaveIfError( container.ProcessAndThreadByThreadId( id() , process, thread ) );
										  
			CMemSpyThreadInfoContainer& threadInfoContainer = thread->InfoContainerForceSyncronousConstructionL();      

			CMemSpyThreadInfoItemBase* threadInfoItem = &threadInfoContainer.Item( type() );
						
			CMemSpyThreadInfoActiveObjects* activeObjectArray = static_cast< CMemSpyThreadInfoActiveObjects* >( threadInfoItem );			
						
		    // Begin a new data stream
		    _LIT( KMemSpyContext, "Active Object List - " );
		    _LIT( KMemSpyFolder, "Active Objects" );
		    iEngine.Sink().DataStreamBeginL( KMemSpyContext, KMemSpyFolder );
		    		    
		    // Set prefix for overall listing
		    iEngine.Sink().OutputPrefixSetLC( KMemSpyContext );

		    // Create header
		    CMemSpyEngineActiveObjectArray::OutputDataColumnsL( iEngine );
		    
		    // List items
		    const TInt count = activeObjectArray->Array().Count();
		    for(TInt i=0; i<count; i++)
		        {
		        const CMemSpyEngineActiveObject& object = activeObjectArray->Array().At( i );
		        //
		        object.OutputDataL( iEngine );
		        }

		    // Tidy up
		    CleanupStack::PopAndDestroy(); // prefix

		    // End data stream		    		    
		    iEngine.Sink().DataStreamEndL();		    
			
			break;
			}
			
		// --- Kernel Heap related functions ---
		case EMemSpyClientServerOpGetHeap:
			{
			TMemSpyHeapInfo heapInfo;			
			iEngine.HelperHeap().GetHeapInfoKernelL( heapInfo );
			TMemSpyHeapData data = iEngine.HelperHeap().NewHeapRawInfo( heapInfo );
			
			TPckgBuf<TMemSpyHeapData> buffer(data);
			aMessage.WriteL(0, buffer);
			
			break;
			}
			
		case EMemSpyClientServerOpGetServerCount:
            {
            CMemSpyEngineServerList* list = iEngine.HelperServer().ServerListL();
            CleanupStack::PushL(list);
            // TODO: cache it between calls
            aMessage.WriteL(0, TPckgBuf<TInt>(list->MdcaCount()));
            
            CleanupStack::PopAndDestroy(list);
            break;
            }
        // --- Servers related functions
        case EMemSpyClientServerOpGetServers:
            {
            CMemSpyEngineServerList* list = iEngine.HelperServer().ServerListL();
            CleanupStack::PushL(list);
            
            TPckgBuf<TInt> a0;
            aMessage.ReadL(0, a0);
            TInt realCount = Min(a0(), list->MdcaCount());
            
            for(TInt i=0, offset = 0; i<realCount; i++, offset += sizeof(TMemSpyServerData))
                {
                const CMemSpyEngineServerEntry& server = list->At(i);
                TMemSpyServerData data;
                
                CMemSpyProcess* process = NULL;
                CMemSpyThread* thread = NULL;
                TInt error = iEngine.Container().ProcessAndThreadByThreadId( server.Id(), process, thread );
                if (error == KErrNone && thread)
                    {
                    data.iProcessId = thread->Process().Id();
                    data.iThreadId = thread->Id();
                    }
                data.iName.Copy(server.Name().Left(KMaxFullName));
                data.iSessionCount = server.SessionCount();
                
                TPckgBuf<TMemSpyServerData> buffer(data);
                aMessage.WriteL(1, buffer, offset);
                }
            
            a0 = list->Count();
            aMessage.WriteL(0, a0);
            
            CleanupStack::PopAndDestroy(list);

            break;
            }
            
        case EMemSpyClientServerOpGetSortedServers:
        	{
        	CMemSpyEngineServerList* list = iEngine.HelperServer().ServerListL();
        	CleanupStack::PushL(list);
        	
        	TPckgBuf<TSortType> a2;
        	aMessage.ReadL( 2, a2 );
        	
        	//sort the list of the servers
        	if( a2() == ESortServByName )
        		list->SortByNameL();
        	else
        		list->SortBySessionCountL();
        	
        	TPckgBuf<TInt> a0;        	
        	aMessage.ReadL(0, a0);        	        	        
        	
        	TInt realCount = Min(a0(), list->MdcaCount());
        	            
        	for(TInt i=0, offset = 0; i<realCount; i++, offset += sizeof(TMemSpyServerData))
        		{
				const CMemSpyEngineServerEntry& server = list->At(i);
				TMemSpyServerData data;
        	                
				CMemSpyProcess* process = NULL;
                CMemSpyThread* thread = NULL;
                TInt error = iEngine.Container().ProcessAndThreadByThreadId( server.Id(), process, thread );
                if (error == KErrNone && thread)
                    {
                    data.iProcessId = thread->Process().Id();
                    data.iThreadId = thread->Id();
                    }
				data.iName.Copy(server.Name().Left(KMaxFullName));
				data.iSessionCount = server.SessionCount();
        	                
				TPckgBuf<TMemSpyServerData> buffer(data);
				aMessage.WriteL(1, buffer, offset);
        		}        	           
			a0 = list->Count();
			aMessage.WriteL(0, a0);
        	            
			CleanupStack::PopAndDestroy(list);
			break;
        	}
          
        case EMemSpyClientServerOpServerListOutputGeneric:
        	{
            TPckgBuf<TBool> a0;
            aMessage.ReadL(0, a0);
        	
            CMemSpyEngineServerList* list;            
            list = iEngine.HelperServer().ServerListL();
            CleanupStack::PushL(list);
            
            _LIT( KMemSpyContext, "Server List - " );
            _LIT( KMemSpyFolder, "Servers" );
            iEngine.Sink().DataStreamBeginL( KMemSpyContext, KMemSpyFolder );

            // Set prefix for overall listing
            iEngine.Sink().OutputPrefixSetLC( KMemSpyContext );

            // Create header
            CMemSpyEngineServerList::OutputDataColumnsL( iEngine, a0() );
               
            // List items
            const TInt count = list->Count();
            for(TInt i=0; i<count; i++)
            	{
				const CMemSpyEngineServerEntry& server = list->At( i );
				//
				server.OutputDataL( iEngine.HelperServer(), a0() );
            	}

            // Tidy up
            CleanupStack::PopAndDestroy(); // prefix
            
            // End data stream
            iEngine.Sink().DataStreamEndL();
            
            CleanupStack::PopAndDestroy(list);        	
        	break;
        	}        	
        	
		case EMemSpyClientServerOpGetMemoryTrackingCycleCount:
			{
			TInt count = iEngine.HelperSysMemTracker().CompletedCycles().Count();
			TPckgBuf<TInt> ret( count );
			aMessage.WriteL( 0, ret );			
			break;
			}		
			
		case EMemSpyClientServerOpGetMemoryTrackingCycles:
			{
			const RPointerArray<CMemSpyEngineHelperSysMemTrackerCycle>& list = iEngine.HelperSysMemTracker().CompletedCycles();

			TPckgBuf<TInt> a0;
			aMessage.ReadL(0, a0);
			TInt realCount = Min(a0(), list.Count());
			
			for (TInt i=0, offset = 0; i<realCount; i++, offset += sizeof(TMemSpyMemoryTrackingCycleData))
				{
				CMemSpyProcess& process = iEngine.Container().At(i);
				TMemSpyMemoryTrackingCycleData data;
				data.iCycleNumber = list[i]->CycleNumber();
				data.iCaption.Copy(list[i]->Caption().Left(KMaxFullName));
				data.iTime = list[i]->Time();
				data.iFreeMemory = list[i]->MemoryFree();
				data.iMemoryDelta = list[i]->MemoryDelta();
				data.iPreviousCycleDiff = list[i]->MemoryFreePreviousCycle();
				data.iChangeCount = list[i]->ChangeCount();
				
				TPckgBuf<TMemSpyMemoryTrackingCycleData> buffer(data);
				aMessage.WriteL(1, buffer, offset);
				}
			
			a0 = list.Count();
			aMessage.WriteL(0, a0);

		break;
		}

	case EMemSpyClientServerOpIsSwmtRunning:
		{
		TPckgBuf<TBool> running(iEngine.HelperSysMemTracker().IsActive());
		aMessage.WriteL(0, running);
		break;
		}
		
	case EMemSpyClientServerOpSystemWideMemoryTrackingTimerPeriodGet:
	    {	    			   
	    // Get current config
	    TMemSpyEngineHelperSysMemTrackerConfig config;
	    iEngine.HelperSysMemTracker().GetConfig( config );
	    TInt time = config.iTimerPeriod.Int();
	    TPckgBuf<TInt> tim(time);			            
	    aMessage.WriteL( 0, tim );
	    break;
	    }
	    
	case EMemSpyClientServerOpSystemWideMemoryTrackingCategoriesGet:
		{
		// Get current config
		TMemSpyEngineHelperSysMemTrackerConfig config;
		iEngine.HelperSysMemTracker().GetConfig( config );		
		TInt categories = config.iEnabledCategories;		
		TPckgBuf<TInt> ret( categories );
		aMessage.WriteL( 0, ret );						
		break;
		}
		
	case EMemSpyClientServerOpSystemWideMemoryTrackingThreadNameFilterGet:
		{
		TMemSpyEngineHelperSysMemTrackerConfig config;
		iEngine.HelperSysMemTracker().GetConfig( config );                 
		TName threadNameFilter = config.iThreadNameFilter;
		TPckgBuf<TName> name(threadNameFilter);
		aMessage.WriteL( 0, name );
		
		break;
		}
		
	case EMemSpyClientServerOpSystemWideMemoryTrackingHeapDumpGet:
		{
		TMemSpyEngineHelperSysMemTrackerConfig config;
		iEngine.HelperSysMemTracker().GetConfig( config );		           
		TBool heapDump = config.iDumpData;
		TPckgBuf<TBool> heap(heapDump);			            
		aMessage.WriteL( 0, heap );
		break;
		}
						
	case EMemSpyClientServerOpSystemWideMemoryTrackingModeGet:
		{
		TMemSpyEngineHelperSysMemTrackerConfig config;
		iEngine.HelperSysMemTracker().GetConfig( config );	 		 
		TPckgBuf<TMemSpyEngineHelperSysMemTrackerConfig::TMemSpyEngineSysMemTrackerMode> mod(config.iMode);			             		            
		aMessage.WriteL(0, mod);	 			 		    
		break;
		}			
		
	case EMemSpyClientServerOpNotifyDeviceWideOperationProgress:
		{
		if (!Server().CurrentOperationTracker())
			{
			User::Leave(KErrNotReady);
			}
		
		Server().CurrentOperationTracker()->AddNotificationL(aMessage);
		break;
		}
		
	case EMemSpyClientServerOpCancelDeviceWideOperation:
		if (!Server().CurrentOperationTracker())
			{
			User::Leave(KErrNotReady);
			}
		
		Server().CurrentOperationTracker()->Cancel();
		break;
	
		
	case EMemSpyClientServerOpGetEComCategoryCount:
	    aMessage.WriteL(0, TPckgBuf<TInt>(iEngine.HelperECom().MdcaCount()));
	    break;
	    

	case EMemSpyClientServerOpGetEComCategories:
	    {
	    TPckgBuf<TInt> a0;
        aMessage.ReadL(0, a0);
        TInt realCount = Min(a0(), iEngine.HelperECom().MdcaCount());
        
        for (TInt i=0, offset = 0; i<realCount; i++, offset += sizeof(TMemSpyEComCategoryData))
            {
            TMemSpyEComCategoryData data;
            data.iId.iUid = i;
            data.iName.Copy(iEngine.HelperECom().At(i).Name());
            data.iInterfaceCount = iEngine.HelperECom().At(i).MdcaCount();
            
            TPckgBuf<TMemSpyEComCategoryData> buffer(data);
            aMessage.WriteL(1, buffer, offset);
		}
        
        a0 = iEngine.HelperECom().MdcaCount();
        aMessage.WriteL(0, a0);
	    break;
    }
	    
	case EMemSpyClientServerOpGetEComInterfaceCount:
	    {
	    TPckgBuf<TUid> a1;
        aMessage.ReadL(1, a1);
        TInt index = a1().iUid;
        
        if (index < 0 || index >= iEngine.HelperECom().MdcaCount())
            {
            User::Leave(KErrArgument);
            }
        
        aMessage.WriteL(0, TPckgBuf<TInt>(iEngine.HelperECom().At(index).MdcaCount()));
	    break;
	    }
	            
	case EMemSpyClientServerOpGetEComInterfaces:
	    {
	    TPckgBuf<TInt> a0;
        aMessage.ReadL(0, a0);
        TInt realCount = Min(a0(), iEngine.HelperECom().MdcaCount());
        
        TPckgBuf<TUid> a1;
        aMessage.ReadL(1, a1);
        TInt categoryIndex = a1().iUid;
        
        if (categoryIndex < 0 || categoryIndex >= iEngine.HelperECom().MdcaCount())
            {
            User::Leave(KErrArgument);
            }
        
        CMemSpyEngineEComCategory &category = iEngine.HelperECom().At(categoryIndex);
        
        for (TInt i=0, offset = 0; i<realCount; i++, offset += sizeof(TMemSpyEComInterfaceData))
            {
            TMemSpyEComInterfaceData data;
            data.iId.iUid = (TInt32) &category.At(i);
            data.iCategoryId.iUid = categoryIndex;
            data.iName.Copy(category.At(i).Name());
            data.iImplementationCount = category.At(i).MdcaCount();
            
            TPckgBuf<TMemSpyEComInterfaceData> buffer(data);
            aMessage.WriteL(2, buffer, offset);
            }
        
        a0 = realCount;
        aMessage.WriteL(0, a0);
        
        break;
	    }
	    
	case EMemSpyClientServerOpGetEComImplementationCount:
	    {
	    TPckgBuf<TUid> a1;
        aMessage.ReadL(1, a1);
        CMemSpyEngineEComInterface* interface = reinterpret_cast<CMemSpyEngineEComInterface*>(a1().iUid);
        
        // TODO: check if it really is valid interface 
        
        aMessage.WriteL(0, TPckgBuf<TInt>(interface->MdcaCount()));
	    break;
	    }
	                
	case EMemSpyClientServerOpGetEComImplementations:
	    {
        TPckgBuf<TUid> a1;
        aMessage.ReadL(1, a1);
        CMemSpyEngineEComInterface* interface = reinterpret_cast<CMemSpyEngineEComInterface*>(a1().iUid);
        
        TPckgBuf<TInt> a0;
        aMessage.ReadL(0, a0);
        TInt realCount = Min(a0(), interface->MdcaCount());
        
        
        for (TInt i=0, offset = 0; i<realCount; i++, offset += sizeof(TMemSpyEComImplementationData))
            {
            TMemSpyEComImplementationData data;
            data.iName.Copy(interface->At(i).Info().DisplayName());
            data.iImplementationUid = interface->At(i).Info().ImplementationUid();
            data.iVersion = interface->At(i).Info().Version();
            data.iDataType.Copy(interface->At(i).Info().DataType());
            data.iOpaqueData.Copy(interface->At(i).Info().OpaqueData());
            data.iDrive = interface->At(i).Info().Drive();
            data.iRomOnly = interface->At(i).Info().RomOnly();
            data.iRomBased = interface->At(i).Info().RomBased();
            data.iVendorId = interface->At(i).Info().VendorId();
            data.iDisabled = interface->At(i).Info().Disabled();
            
            TPckgBuf<TMemSpyEComImplementationData> buffer(data);
            aMessage.WriteL(2, buffer, offset);
            }
        
        a0 = realCount;
        aMessage.WriteL(0, a0);
        
        break;
	    }
	    
	case EMemSpyClientServerOpGetWindowGroupCount:
	    {
	    if (!iEngine.IsHelperWindowServerSupported())
	        {
	        User::Leave(KErrNotSupported);
	        }
	    
	    RWsSession windowSession;
	    windowSession.Connect();
	    TInt result = windowSession.NumWindowGroups();
	    windowSession.Close();
	    User::LeaveIfError(result);
	    
	    aMessage.WriteL(0, TPckgBuf<TInt>(result));
	    break;
	    }
        
	case EMemSpyClientServerOpGetWindowGroups:
	    {
	    if (!iEngine.IsHelperWindowServerSupported())
            {
            User::Leave(KErrNotSupported);
            }
	    
	    RArray<TMemSpyEngineWindowGroupBasicInfo> groups;
	    CleanupClosePushL( groups );
	    
	    iEngine.HelperWindowServer().GetWindowGroupListL( groups );
        
        TPckgBuf<TInt> a0;
        aMessage.ReadL(0, a0);
        TInt realCount = Min(a0(), groups.Count());
                        
        for (TInt i=0, offset = 0; i<realCount; i++, offset += sizeof(TMemSpyEngineWindowGroupDetails))
            {
            TMemSpyEngineWindowGroupDetails data;
            
            
            iEngine.HelperWindowServer().GetWindowGroupDetailsL(groups[i].iId, data);
            
            TPckgBuf<TMemSpyEngineWindowGroupDetails> buffer(data);
            aMessage.WriteL(1, buffer, offset);
            }
        
        a0 = realCount;
        aMessage.WriteL(0, a0);
        
        CleanupStack::PopAndDestroy( &groups ); 
        
	    break;
	    }
	    
	    
		}
    }

// ---------------------------------------------------------
// DoCmdServiceL( const RMessage2& aMessage )
// ---------------------------------------------------------
//
void CMemSpyEngineSession::DoCmdServiceL( const RMessage2& aMessage )
    {
    TInt error = KErrNone;

    // Check function attributes
    const TInt function = aMessage.Function() & KMemSpyOpFlagsTypeMask;
    const TInt argSpec = aMessage.Function() & KMemSpyOpFlagsInclusionMask;
    const TBool byThreadId = ( argSpec == KMemSpyOpFlagsIncludesThreadId );
    const TBool byThreadName = ( argSpec == KMemSpyOpFlagsIncludesThreadName );

    TRACE( RDebug::Printf( "[MemSpy] CMemSpyEngineSession::DoServiceL() - START - unmodified function: 0x%08x, opCode: %d [TID: %d, TN: %d]", aMessage.Function(), function, byThreadId, byThreadName ) );

    // Check function is supported and argument combination is valid
    error = ValidateFunction( function, byThreadId, byThreadName );
    TRACE( RDebug::Printf( "[MemSpy] CMemSpyEngineSession::DoServiceL() - validation result: %d", error ) );
    
    // Process function request
    if  ( error == KErrNone )
        {
        if  ( byThreadId )
            {
            TRACE( RDebug::Printf( "[MemSpy] CMemSpyEngineSession::DoServiceL() - [TID] thread-specific..." ) );
            
            const TThreadId threadId( aMessage.Int0() );
            HandleThreadSpecificOpL( function, threadId );
            }
        else if ( byThreadName )
            {
            TRACE( RDebug::Printf( "[MemSpy] CMemSpyEngineSession::DoServiceL() - [TN] thread-specific..." ) );

            error = aMessage.GetDesLength( 0 /*slot 0*/ );
        
            if  ( error > 0 && error <= KMaxFullName )
                {
                TFullName* threadName = new(ELeave) TFullName();
                CleanupStack::PushL( threadName );
                aMessage.ReadL( 0, *threadName );
                HandleThreadSpecificOpL( function, *threadName );
                CleanupStack::PopAndDestroy( threadName );
                }
            else
                {
                error = KErrArgument;
                }
            }
        else
            {
            TRACE( RDebug::Printf( "[MemSpy] CMemSpyEngineSession::DoServiceL() - thread-agnostic..." ) );

            HandleThreadAgnosticOpL( function, aMessage );
            }
        }

    User::LeaveIfError( error );

    TRACE( RDebug::Printf( "[MemSpy] CMemSpyEngineSession::DoServiceL() - END" ) );
    }



TInt CMemSpyEngineSession::ValidateFunction( TInt aFunction, TBool aIncludesThreadId, TBool aIncludesThreadName )
    {
    TInt err = KErrNotSupported;
    
    // Check the operation is within op-code range
    if  ( aFunction >= EMemSpyClientServerOpMarkerFirst && aFunction < EMemSpyClientServerOpMarkerLast )
        {
        // Check the operation doesn't include unnecessary or not supported information
        const TBool includesThreadIdentifier = ( aIncludesThreadId || aIncludesThreadName );
        if  ( includesThreadIdentifier && aFunction >= EMemSpyClientServerOpMarkerThreadAgnosticFirst )
            {
            // Passing a thread identifier to a thread agnostic operation
            err = KErrArgument;
            }
        else
            {
            err = KErrNone;
            }
        }
    //
    if  ( err != KErrNone )
        {
        RDebug::Printf( "[MemSpy] CMemSpyEngineSession::ValidateFunction() - function request did not validate - [withId: %d, withName: %d]", aIncludesThreadId, aIncludesThreadName );
        }
    //
    return err;
    }


void CMemSpyEngineSession::HandleThreadSpecificOpL( TInt aFunction, const TThreadId& aThreadId )
    {
    TRACE( RDebug::Printf( "[MemSpy] CMemSpyEngineSession::HandleThreadSpecificOpL() - START - aFunction: %d, aThreadId: %d", aFunction, (TUint) aThreadId ) );

    ASSERT( (TUint) aThreadId != 0 );
    TInt error = KErrNone;

    // Check if its a kernel thread identifier
    const TBool isKernel = ( static_cast<TUint32>( aThreadId ) == KMemSpyClientServerThreadIdKernel );

    // Treat as thread specific operation
    CMemSpyProcess* process = NULL;
    CMemSpyThread* thread = NULL;
    if  ( !isKernel )
        {
        error = iEngine.Container().ProcessAndThreadByThreadId( aThreadId, process, thread );
        TRACE( RDebug::Printf( "[MemSpy] CMemSpyEngineSession::HandleThreadSpecificOpL() - search result: %d, proc: 0x%08x, thread: 0x%08x", error, process, thread ) );
        }
    else
        {
        // Kernel is only supported for a couple of operations
        if  ( aFunction == EMemSpyClientServerOpHeapInfo || aFunction == EMemSpyClientServerOpHeapData )
            {
            }
        else
            {
            TRACE( RDebug::Printf( "[MemSpy] CMemSpyEngineSession::HandleThreadSpecificOpL() - trying to call unsupported function for kernel thread!" ) );
            error = KErrArgument;
            }
        }

    // Must be no error so far and we must have a valid thread & process when performing a non-kernel op
    // or then if we are performing a kernel op, we don't need the thread or process.
    if  ( error == KErrNone && ( ( thread && process && !isKernel ) || ( isKernel ) ) )
        {
#ifdef _DEBUG
        if  ( thread )
            {
            HBufC* threadName = thread->FullName().AllocLC();
            _LIT( KTrace2, "[MemSpy] CMemSpyEngineSession::HandleThreadSpecificOpL() - thread: %S" );
            RDebug::Print( KTrace2, threadName );
            CleanupStack::PopAndDestroy( threadName );
            }
        else if ( isKernel )
            {
            _LIT( KTrace2, "[MemSpy] CMemSpyEngineSession::HandleThreadSpecificOpL() - thread: Kernel" );
            RDebug::Print( KTrace2 );
            }
#endif

        // Got a valid thread object - now work out which operation to perform...
        switch( aFunction )
            {
        case EMemSpyClientServerOpSummaryInfo:
            iEngine.HelperProcess().OutputProcessInfoL( *process );
            break;
        case EMemSpyClientServerOpSummaryInfoDetailed:
            iEngine.HelperProcess().OutputProcessInfoDetailedL( *process );
            break;
        case EMemSpyClientServerOpHeapInfo:
            if  ( isKernel )
                {
                iEngine.HelperHeap().OutputHeapInfoKernelL();
                }
            else
                {
                iEngine.HelperHeap().OutputHeapInfoUserL( *thread );
                }
            break;
        case EMemSpyClientServerOpHeapCellListing:
            iEngine.HelperHeap().OutputCellListingUserL( *thread );
            break;
        case EMemSpyClientServerOpHeapData:
            if  ( isKernel )
                {
                iEngine.HelperHeap().OutputHeapDataKernelL();
                }
            else
                {
                iEngine.HelperHeap().OutputHeapDataUserL( *thread );
                }
            break;
        case EMemSpyClientServerOpStackInfo:
            iEngine.HelperStack().OutputStackInfoL( *thread );
            break;
        case EMemSpyClientServerOpStackDataUser:
            iEngine.HelperStack().OutputStackDataL( *thread, EMemSpyDriverDomainUser, EFalse );
            break;
        case EMemSpyClientServerOpStackDataKernel:
            iEngine.HelperStack().OutputStackDataL( *thread, EMemSpyDriverDomainKernel, EFalse );
            break;
        case EMemSpyClientServerOpOpenFiles:
            iEngine.HelperFileSystem().ListOpenFilesL( aThreadId );
            break;

        default:
            error = KErrNotSupported;
            break;
            }
        }

    TRACE( RDebug::Printf("[MemSpy] CMemSpyEngineSession::HandleThreadSpecificOpL() - END - aFunction: %d, aThreadId: %d, error: %d", aFunction, (TUint) aThreadId, error ) );
    User::LeaveIfError( error );
    }


void CMemSpyEngineSession::HandleThreadSpecificOpL( TInt aFunction, const TDesC& aThreadName )
    {
    TRACE( RDebug::Print( _L("[MemSpy] CMemSpyEngineSession::HandleThreadSpecificOpL() - START - aFunction: %d, aThreadName: %S"), aFunction, &aThreadName ) );
    //
    CMemSpyProcess* process = NULL;
    CMemSpyThread* thread = NULL;
    TInt error = iEngine.Container().ProcessAndThreadByPartialName( aThreadName, process, thread );
    User::LeaveIfError( error );
    //
    const TThreadId threadId( thread->Id() );
    HandleThreadSpecificOpL( aFunction, threadId );
    //
    TRACE( RDebug::Print( _L("[MemSpy] CMemSpyEngineSession::HandleThreadSpecificOpL() - END - aFunction: %d, aThreadName: %S"), aFunction, &aThreadName ) );
    }


void CMemSpyEngineSession::HandleThreadAgnosticOpL( TInt aFunction, const RMessage2& aMessage )
    {
    TRACE( RDebug::Printf("[MemSpy] CMemSpyEngineSession::HandleThreadAgnosticOpL() - START" ) );
    
    //
    if  ( aFunction ==  EMemSpyClientServerOpHeapInfoCompact )
        {
        TRACE( RDebug::Printf("[MemSpy] CMemSpyEngineSession::HandleThreadAgnosticOpL() - EMemSpyClientServerOpHeapInfoCompact") );
        if (aMessage.Function() & KMemSpyOpFlagsAsyncOperation)
        	{
			StartDeviceWideOperationL(CMemSpyDeviceWideOperations::EEntireDeviceHeapInfoCompact, aMessage);
        	}
        else
        	{
			iEngine.HelperHeap().OutputHeapInfoForDeviceL();
        	}
        }
    else if ( aFunction ==  EMemSpyClientServerOpStackInfoCompact )
        {
        TRACE( RDebug::Printf("[MemSpy] CMemSpyEngineSession::HandleThreadAgnosticOpL() - EMemSpyClientServerOpStackInfoCompact") );
        if (aMessage.Function() & KMemSpyOpFlagsAsyncOperation)
			{
			StartDeviceWideOperationL(CMemSpyDeviceWideOperations::EEntireDeviceStackInfoCompact, aMessage);
			}
		else
			{
			iEngine.HelperStack().OutputStackInfoForDeviceL();
			}
        }
    else if ( aFunction == EMemSpyClientServerOpSystemWideMemoryTrackingTimerStart )
        {
        TRACE( RDebug::Printf("[MemSpy] CMemSpyEngineSession::HandleThreadAgnosticOpL() - EMemSpyClientServerOpSystemWideMemoryTrackingTimerStart") );
        iEngine.HelperSysMemTracker().StartL();
        }
    else if ( aFunction == EMemSpyClientServerOpSystemWideMemoryTrackingTimerStop )
        {
        TRACE( RDebug::Printf("[MemSpy] CMemSpyEngineSession::HandleThreadAgnosticOpL() - EMemSpyClientServerOpSystemWideMemoryTrackingTimerStop") );
        iEngine.HelperSysMemTracker().StopL();
        }
    else if ( aFunction == EMemSpyClientServerOpSystemWideMemoryTrackingReset )
        {
        TRACE( RDebug::Printf("[MemSpy] CMemSpyEngineSession::HandleThreadAgnosticOpL() - EMemSpyClientServerOpSystemWideMemoryTrackingReset") );
        iEngine.HelperSysMemTracker().Reset();
        }
    else if ( aFunction == EMemSpyClientServerOpSystemWideMemoryTrackingForceUpdate )
        {
        TRACE( RDebug::Printf("[MemSpy] CMemSpyEngineSession::HandleThreadAgnosticOpL() - EMemSpyClientServerOpSystemWideMemoryTrackingForceUpdate") );
        iEngine.HelperSysMemTracker().CheckForChangesNowL();
        }
    else if ( aFunction == EMemSpyClientServerOpSystemWideMemoryTrackingTimerPeriodSet )
        {
        TRACE( RDebug::Printf("[MemSpy] CMemSpyEngineSession::HandleThreadAgnosticOpL() - EMemSpyClientServerOpSystemWideMemoryTrackingTimerPeriodSet") );
        
        // Get current config
        TMemSpyEngineHelperSysMemTrackerConfig config;
        iEngine.HelperSysMemTracker().GetConfig( config );

        // Set new timer value
        config.iTimerPeriod = aMessage.Int0();

        // And update config... which will leave if the config is invalid
        iEngine.HelperSysMemTracker().SetConfigL( config );
        }
    else if ( aFunction == EMemSpyClientServerOpSystemWideMemoryTrackingCategoriesSet )
        {
        TRACE( RDebug::Printf("[MemSpy] CMemSpyEngineSession::HandleThreadAgnosticOpL() - EMemSpyClientServerOpSystemWideMemoryTrackingCategoriesSet") );
        // Get current config
        TMemSpyEngineHelperSysMemTrackerConfig config;
        iEngine.HelperSysMemTracker().GetConfig( config );

        // Set new categories
        config.iEnabledCategories = aMessage.Int0();

        // And update config... which will leave if the config is invalid
        iEngine.HelperSysMemTracker().SetConfigL( config );
        }
    
    else if ( aFunction == EMemSpyClientServerOpSystemWideMemoryTrackingThreadNameFilterSet )
        {
        TRACE( RDebug::Printf("[MemSpy] CMemSpyEngineSession::HandleThreadAgnosticOpL() - EMemSpyClientServerOpSystemWideMemoryTrackingThreadNameFilterSet") );
        // Get current config
        TMemSpyEngineHelperSysMemTrackerConfig config;
        iEngine.HelperSysMemTracker().GetConfig( config );

        // Set new filter
        RBuf buf;
        buf.CleanupClosePushL();
        TInt len = aMessage.GetDesLength( 0 );
        if ( len > 0 )
            {
            buf.CreateL( len );
            aMessage.ReadL( 0, buf, 0 );
            config.iThreadNameFilter.Copy( buf );            
            }
        else
            {
            config.iThreadNameFilter.Zero();
            }
        CleanupStack::PopAndDestroy( &buf );

        // And update config... which will leave if the config is invalid
        iEngine.HelperSysMemTracker().SetConfigL( config );
        }
    else if ( aFunction == EMemSpyClientServerOpSystemWideMemoryTrackingHeapDumpSet )
        {
        // Get current config
        TMemSpyEngineHelperSysMemTrackerConfig config;
        iEngine.HelperSysMemTracker().GetConfig( config );
        
        // Set new Heap Dump value
        config.iDumpData = aMessage.Int0();
        
        // And update config... which will leave if the config is invalid
        iEngine.HelperSysMemTracker().SetConfigL( config );
        }
    else if( aFunction == EMemSpyClientServerOpSystemWideMemoryTrackingModeSet)
    	{
		TMemSpyEngineHelperSysMemTrackerConfig config;
		iEngine.HelperSysMemTracker().GetConfig( config );
		
		TPckgBuf<TMemSpyEngineHelperSysMemTrackerConfig::TMemSpyEngineSysMemTrackerMode> mode;
		aMessage.ReadL( 0, mode );
		
		config.iMode = mode();
			
		// And update config... which will leave if the config is invalid
		iEngine.HelperSysMemTracker().SetConfigL( config );
    	}
    
    else if ( aFunction == EMemSpyClientServerOpSwitchOutputSinkTrace )
        {
        TRACE( RDebug::Printf("[MemSpy] CMemSpyEngineSession::HandleThreadAgnosticOpL() - EMemSpyClientServerOpSwitchOutputSinkTrace") );
        iEngine.InstallDebugSinkL();
        }
    else if ( aFunction == EMemSpyClientServerOpSwitchOutputSinkFile )
        {
        TRACE( RDebug::Printf("[MemSpy] CMemSpyEngineSession::HandleThreadAgnosticOpL() - EMemSpyClientServerOpSwitchOutputSinkFile") );
        // Read file name from message.
        TFileName fileName;
        RBuf buf;
		buf.CleanupClosePushL();
		
		TInt len = aMessage.GetDesLength( 0 );
		if ( len > 0 )
			{
			buf.CreateL( len );
			aMessage.ReadL( 0, buf, 0 );
			
			iEngine.InstallFileSinkL( buf );           
			}
		else
			{
			iEngine.InstallFileSinkL( KNullDesC );
			}
		
		CleanupStack::PopAndDestroy( &buf );
        
        }
    else if ( aFunction == EMemSpyClientServerOpEnumerateKernelContainer )
        {
        const TMemSpyDriverContainerType type = CMemSpyEngineHelperKernelContainers::MapToType( static_cast< TObjectType >( aMessage.Int0() ) );

        TRACE( RDebug::Printf("[MemSpy] CMemSpyEngineSession::HandleThreadAgnosticOpL() - EMemSpyClientServerOpEnumerateKernelContainer - type: %d", type ) );

        CMemSpyEngineGenericKernelObjectList* model = iEngine.HelperKernelContainers().ObjectsForSpecificContainerL( type );
        CleanupStack::PushL( model );
        model->OutputL( iEngine.Sink() );
        CleanupStack::PopAndDestroy( model );
        }
    else if ( aFunction == EMemSpyClientServerOpEnumerateKernelContainerAll )
        {
        TRACE( RDebug::Printf("[MemSpy] CMemSpyEngineSession::HandleThreadAgnosticOpL() - EMemSpyClientServerOpEnumerateKernelContainerAll") );
        CMemSpyEngineGenericKernelObjectContainer* model = iEngine.HelperKernelContainers().ObjectsAllL();
        CleanupStack::PushL( model );
        model->OutputL( iEngine.Sink() );
        CleanupStack::PopAndDestroy( model );
        }
    else if ( aFunction == EMemSpyClientServerOpOpenFiles )
        {
        TRACE( RDebug::Printf("[MemSpy] CMemSpyEngineSession::HandleThreadAgnosticOpL() - EMemSpyClientServerOpOpenFiles") );
        iEngine.ListOpenFilesL();
        }
    else if ( aFunction == EMemSpyClientServerOpDisableAknIconCache )
        {
        TRACE( RDebug::Printf("[MemSpy] CMemSpyEngineSession::HandleThreadAgnosticOpL() - EMemSpyClientServerOpDisableAknIconCache") );
        iEngine.HelperRAM().SetAknIconCacheStatusL( EFalse );
        }
    else if ( aFunction == EMemSpyClientServerOpSummaryInfo )
    	{
		TRACE( RDebug::Printf("[MemSpy] CMemSpyEngineSession::HandleThreadAgnosticOpL() - EMemSpyClientServerOpSummaryInfo") );
		StartDeviceWideOperationL(CMemSpyDeviceWideOperations::EPerEntityGeneralSummary, aMessage);
    	}
    else if ( aFunction == EMemSpyClientServerOpSummaryInfoDetailed )
		{
		TRACE( RDebug::Printf("[MemSpy] CMemSpyEngineSession::HandleThreadAgnosticOpL() - EMemSpyClientServerOpSummaryInfoDetailed") );
		StartDeviceWideOperationL(CMemSpyDeviceWideOperations::EPerEntityGeneralDetailed, aMessage);
		}
    else if ( aFunction == EMemSpyClientServerOpHeapInfo )
		{
		TRACE( RDebug::Printf("[MemSpy] CMemSpyEngineSession::HandleThreadAgnosticOpL() - EMemSpyClientServerOpHeapInfo") );
		StartDeviceWideOperationL(CMemSpyDeviceWideOperations::EPerEntityHeapInfo, aMessage);
		}
    else if ( aFunction == EMemSpyClientServerOpHeapCellListing )
		{
		TRACE( RDebug::Printf("[MemSpy] CMemSpyEngineSession::HandleThreadAgnosticOpL() - EMemSpyClientServerOpHeapCellListing") );
		StartDeviceWideOperationL(CMemSpyDeviceWideOperations::EPerEntityHeapCellListing, aMessage);
		}
    else if ( aFunction == EMemSpyClientServerOpHeapData )
		{
		TRACE( RDebug::Printf("[MemSpy] CMemSpyEngineSession::HandleThreadAgnosticOpL() - EMemSpyClientServerOpHeapData") );
		StartDeviceWideOperationL(CMemSpyDeviceWideOperations::EPerEntityHeapData, aMessage);
		}
    else if ( aFunction == EMemSpyClientServerOpStackInfo )
		{
		TRACE( RDebug::Printf("[MemSpy] CMemSpyEngineSession::HandleThreadAgnosticOpL() - EMemSpyClientServerOpStackInfo") );
		StartDeviceWideOperationL(CMemSpyDeviceWideOperations::EPerEntityStackInfo, aMessage);
		}
    else if ( aFunction == EMemSpyClientServerOpStackDataUser )
		{
		TRACE( RDebug::Printf("[MemSpy] CMemSpyEngineSession::HandleThreadAgnosticOpL() - EMemSpyClientServerOpStackDataUser") );
		StartDeviceWideOperationL(CMemSpyDeviceWideOperations::EPerEntityStackDataUser, aMessage);
		}
    else if ( aFunction == EMemSpyClientServerOpStackDataKernel )
		{
		TRACE( RDebug::Printf("[MemSpy] CMemSpyEngineSession::HandleThreadAgnosticOpL() - EMemSpyClientServerOpStackDataKernel") );
		StartDeviceWideOperationL(CMemSpyDeviceWideOperations::EPerEntityStackDataKernel, aMessage);
		}
    else
        {
        TRACE( RDebug::Printf("[MemSpy] CMemSpyEngineSession::HandleThreadAgnosticOpL() - [device-wide operation] => invoking UI") );
        iEngine.NotifyClientServerOperationRequestL( aFunction );
        }
    //
    TRACE( RDebug::Printf("[MemSpy] CMemSpyEngineSession::HandleThreadAgnosticOpL() - END" ) );
    }

void CMemSpyEngineSession::StartDeviceWideOperationL(CMemSpyDeviceWideOperations::TOperation aOperation, const RMessage2& aMessage)
	{
	if (Server().CurrentOperationTracker())
		{
		User::Leave(KErrInUse);
		}
	
	Server().SetCurrentOperationTracker(CMemSpyDwOperationTracker::NewL(aOperation, aMessage, Server()));
	}









CMemSpyDwOperationTracker* CMemSpyDwOperationTracker::NewL(CMemSpyDeviceWideOperations::TOperation aOperation, 
		const RMessage2& aOperationMessage, CMemSpyEngineServer& aServer)
	{
	CMemSpyDwOperationTracker* self = new (ELeave) CMemSpyDwOperationTracker(aOperationMessage, aServer);
	CleanupStack::PushL( self );
	self->ConstructL(aOperation);
	CleanupStack::Pop( self );
	return self;
	}
	
CMemSpyDwOperationTracker::~CMemSpyDwOperationTracker()
	{
	delete iOperation;
	delete iPendingNotifications;
	}

CMemSpyDwOperationTracker::CMemSpyDwOperationTracker(const RMessage2& aOperationMessage, CMemSpyEngineServer& aServer) : 
		iOperationMessage(aOperationMessage),
		iServer(aServer),
		iPendingNotifications(0),
		iOperation(0),
		iProgress(0)
	{
	}


void CMemSpyDwOperationTracker::ConstructL(CMemSpyDeviceWideOperations::TOperation aOperation)
	{
	iPendingNotifications = new (ELeave) CArrayFixFlat<RMessage2>(3);
	iOperation = CMemSpyDeviceWideOperations::NewL(iServer.Engine(), *this, aOperation);
	}

void CMemSpyDwOperationTracker::AddNotificationL(const RMessage2& aMessage)
	{
	iPendingNotifications->AppendL(aMessage);
	}

void CMemSpyDwOperationTracker::Cancel()
	{
	iOperation->Cancel();
	}

void CMemSpyDwOperationTracker::HandleDeviceWideOperationEvent(TEvent aEvent, TInt aParam1, const TDesC& aParam2)
	{
	switch( aEvent )
		{
	case MMemSpyDeviceWideOperationsObserver::EOperationCompleted:
	case MMemSpyDeviceWideOperationsObserver::EOperationCancelled:
		iServer.SetCurrentOperationTracker(0);
		
		for (TInt i=0; i<iPendingNotifications->Count(); i++)
			{
			iPendingNotifications->At(i).Complete(KErrCancel);
			}
		
		if (iOperationMessage.Function() & KMemSpyOpFlagsAsyncOperation)
			{
			iOperationMessage.Complete(
				aEvent == MMemSpyDeviceWideOperationsObserver::EOperationCompleted ? KErrNone : KErrCancel);
			}
		
		iPendingNotifications->Reset();
		
		delete this;
		break;
		
	case MMemSpyDeviceWideOperationsObserver::EOperationProgressEnd:
		{
		iProgress += aParam1;
		for (TInt i=0; i<iPendingNotifications->Count(); i++)
			{
			TInt err;
			TRAP(err, iPendingNotifications->At(i).WriteL(0, TPckgBuf<TInt>( iProgress * 100 / iOperation->TotalOperationSize() )));
			TRAP(err, iPendingNotifications->At(i).WriteL(1, aParam2));
			if (err != KErrNone)
				{
				// TODO: iPendingProgressNotifications->At(i).Panic()
				}
			iPendingNotifications->At(i).Complete(KErrNone);
			}
		iPendingNotifications->Reset();
		break;
		}
		
		}
	
	}
