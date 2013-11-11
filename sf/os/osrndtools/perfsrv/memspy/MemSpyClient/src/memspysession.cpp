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


#include "memspysession.h"

#include <memspyengineclientinterface.h>
// API
#include <memspy/engine/memspyprocessdata.h>
#include <memspy/engine/memspythreaddata.h> 
#include <memspy/engine/memspykernelobjectdata.h>
#include <memspy/engine/memspyheapdata.h>
#include <memspy/engine/memspymemorytrackingcycledata.h>
//KernelObjects
#include <memspy/driver/memspydriverenumerationsshared.h>
// Servers
#include <memspy/engine/memspyserverdata.h>
// ECom
#include <memspy/engine/memspyecomdata.h>
// Window groups
#include <memspy/engine/memspyenginehelperwindowserver.h>

// IMPLEMENTATION

EXPORT_C RMemSpySession::RMemSpySession()
    {
    }


EXPORT_C TInt RMemSpySession::Connect()
    {
	TInt error(KErrNone);
	
	for (TInt i=0; i<2; i++) // Two retries max
		{
		TInt error = CreateSession(KMemSpyServerName, TVersion(KMemSpyVersion, 0, 0));
		
		if (error != KErrNotFound && error != KErrServerTerminated)
			return error;
		
		error = StartServer();
		
		if (error != KErrNone && error != KErrAlreadyExists)
			return error;
		}
	
	return error;	
    }

TInt RMemSpySession::StartServer()
	{
	RProcess server;
	_LIT(KCommand, "start");
	const TUid KServerUid3 = {0xE5883BC2};
	TInt error = server.Create(KMemSpyServerName, KCommand);//, KServerUid3);
	
	if (error != KErrNone)
		return error;
	
	TRequestStatus startStatus, stopStatus;
	server.Logon(stopStatus);
	if (stopStatus != KRequestPending)
		{
		User::WaitForRequest(stopStatus);
		server.Kill(0);
		server.Close();
		return stopStatus.Int();
		}
	
	server.Rendezvous(startStatus);
	server.Resume();
	User::WaitForRequest(startStatus, stopStatus);
	if (startStatus == KRequestPending)
		{
		// not started yet, i.e. stopStatus was signaled
		server.Close();
		return stopStatus.Int();
		}
	
	// Rendezvous was called - the server is ready
	
	// We first need to cancel Logon
	server.LogonCancel(stopStatus);
	// We don't need this anymore
	server.Close();
	// Wait for LogonCancel to complete
	User::WaitForRequest(stopStatus);

	// Everything went OK
	return KErrNone;
	}

EXPORT_C void RMemSpySession::GetProcessesL(RArray<CMemSpyApiProcess*> &aProcesses, TSortType aSortType)
	{
	TPckgBuf<TInt> count;
	User::LeaveIfError(SendReceive(EMemSpyClientServerOpGetProcessCount, TIpcArgs(&count)));
	
	TInt requestedCount = count();
	HBufC8* buffer = HBufC8::NewLC(requestedCount * sizeof(TMemSpyProcessData));
	TPtr8 bufferPtr(buffer->Des());
	
	User::LeaveIfError(SendReceive(EMemSpyClientServerOpGetProcesses, TIpcArgs(&count, &bufferPtr)));
	aProcesses.Reset();
	
	TInt receivedCount = Min(count(), requestedCount);
	for(TInt i=0, offset = 0; i<requestedCount; i++, offset+=sizeof(TMemSpyProcessData))
		{
		TPckgBuf<TMemSpyProcessData> data;
		data.Copy(bufferPtr.Ptr()+offset, sizeof(TMemSpyProcessData));
		aProcesses.AppendL(CMemSpyApiProcess::NewLC(data()));
		}
	CleanupStack::Pop(aProcesses.Count());
	CleanupStack::PopAndDestroy(buffer);
	}

EXPORT_C TProcessId RMemSpySession::GetProcessIdByNameL(const TDesC& aProcessName)
	{
	TPckgBuf<TProcessId> procId;
	User::LeaveIfError(SendReceive(EMemSpyClienServerOpGetProcessIdByName, 
			TIpcArgs(&aProcessName, &procId)));
	
	return procId();
	}

EXPORT_C TInt RMemSpySession::ProcessSystemPermanentOrCritical( TProcessId aId, TBool aValue )
	{
	TPckgBuf<TProcessId> arg1( aId );
	TPckgBuf<TBool> arg2( aValue );
	TIpcArgs args( &arg1, &arg2 );
		
	TInt error = SendReceive( EMemSpyClientServerOpProcessSystemPermanentOrCritical, args );
	
	aValue = arg2();
	
	return error;
	}

EXPORT_C TInt RMemSpySession::EndProcessL( TProcessId aId, TMemSpyEndType aType )
	{
	TPckgBuf<TProcessId> arg1( aId );
	TPckgBuf<TMemSpyEndType> arg2( aType );
	TIpcArgs args( &arg1, &arg2 );
	
	TInt error = SendReceive( EMemSpyClientServerOpEndProcess, args );
	
	return error;
	}

EXPORT_C TInt RMemSpySession::SwitchToProcess( TProcessId aId, TBool aBrought )
	{
	TPckgBuf<TProcessId> arg1( aId );
	TPckgBuf<TBool> arg2( aBrought );
	TIpcArgs args( &arg1, &arg2 );
	
	TInt error = SendReceive( EMemSpyClientServerOpSwitchToProcess, args );
	
	return error;	
	}

// Threads related methods
EXPORT_C void RMemSpySession::GetThreadsL(TProcessId aProcessId, RArray<CMemSpyApiThread*> &aThreads, TSortType aSortType)
	{
	TPckgBuf<TInt> count;
	TPckgBuf<TProcessId> pid(aProcessId);
	User::LeaveIfError(SendReceive(EMemSpyClientServerOpGetThreadCount, TIpcArgs(&count, &pid)));
	
	TInt requestedCount = count();
	HBufC8* buffer = HBufC8::NewLC(requestedCount * sizeof(TMemSpyThreadData));
	TPtr8 bufferPtr(buffer->Des());
	
	User::LeaveIfError(SendReceive(EMemSpyClientServerOpGetThreads, TIpcArgs(&count, &bufferPtr, &pid)));
	aThreads.Reset();
	
	TInt receivedCount = Min(count(), requestedCount);
	for(TInt i=0, offset = 0; i<requestedCount; i++, offset+=sizeof(TMemSpyThreadData))
		{
		TPckgBuf<TMemSpyThreadData> data;
		data.Copy(bufferPtr.Ptr()+offset, sizeof(TMemSpyThreadData));
		aThreads.AppendL(CMemSpyApiThread::NewLC(data()));
		}
	CleanupStack::Pop(aThreads.Count());
	CleanupStack::PopAndDestroy(buffer);
	}

EXPORT_C void RMemSpySession::SetThreadPriorityL(TThreadId aId, TInt aPriority)
	{
	TPckgBuf<TThreadId> arg1( aId );
	TPckgBuf<TInt> arg2( aPriority );
	
	User::LeaveIfError(SendReceive( EMemSpyClientServerOpSetThreadPriority, TIpcArgs(&arg1, &arg2)));
	}

EXPORT_C TInt RMemSpySession::ThreadSystemPermanentOrCritical( TThreadId aId, TBool aValue )
	{
	TPckgBuf<TThreadId> arg1( aId );
	TPckgBuf<TBool> arg2( aValue );
	TIpcArgs args( &arg1, &arg2 );
	
	TInt error = SendReceive( EMemSpyClientServerOpThreadSystemPermanentOrCritical, args );
	
	aValue = arg2();
	
	return error;
	}

EXPORT_C TInt RMemSpySession::EndThreadL( TThreadId aId, TMemSpyEndType aType )
	{
	TPckgBuf<TThreadId> arg1( aId );
	TPckgBuf<TMemSpyEndType> arg2( aType );
	TIpcArgs args( &arg1, &arg2 );
	
	TInt error = SendReceive( EMemSpyClientServerOpEndThread, args );
	
	return error;
	}

EXPORT_C TInt RMemSpySession::SwitchToThread( TThreadId aId, TBool aBrought )
	{
	TPckgBuf<TThreadId> arg1( aId );
	TPckgBuf<TBool> arg2( aBrought );
	TIpcArgs args( &arg1, &arg2 );
	
	TInt error = SendReceive( EMemSpyClientServerOpSwitchToThread, args );
	
	return error;	
	}

EXPORT_C TInt RMemSpySession::GetInfoItemType( TInt aIndex, TThreadId aId, TMemSpyThreadInfoItemType &aType )
	{
	TPckgBuf<TInt> arg1( aIndex );
	TPckgBuf<TThreadId> arg2( aId );
	TPckgBuf<TMemSpyThreadInfoItemType> arg3;
	TIpcArgs args( &arg1, &arg2, &arg3 );
		
	TInt error = SendReceive( EMemSpyClientServerOpGetInfoItemType, args );
	
	aType = arg3();
	
	return error;
	}

EXPORT_C void RMemSpySession::GetThreadInfoItemsL( RArray<CMemSpyApiThreadInfoItem*> &aInfoItems, TThreadId aId, TMemSpyThreadInfoItemType aType )
	{
	TPckgBuf<TThreadId> id( aId );	
	TPckgBuf<TMemSpyThreadInfoItemType> type( aType );
	TPckgBuf<TInt> count;	
	
	TInt error = SendReceive( EMemSpyClientServerOpGetThreadInfoItemsCount, TIpcArgs( &id, &type, &count ) );
	TInt itemCount = count();
	
	if( error == KErrNone )
		{		
		if( itemCount == 0 )
			{
			aInfoItems.Reset();
			}
		else
			{
			HBufC8* buffer = HBufC8::NewLC( itemCount * sizeof(TMemSpyThreadInfoItemData) );
			TPtr8 bufferPtr(buffer->Des());
			
			TPckgBuf<TInt> requestedCount( itemCount );
			
			TIpcArgs args( &requestedCount, &id, &type, &bufferPtr );
			TInt error = SendReceive( EMemSpyClientServerOpGetThreadInfoItems, args ); // TODO check error
			
			aInfoItems.Reset();
		
			for(TInt i=0, offset = 0; i < itemCount; i++, offset+=sizeof(TMemSpyThreadInfoItemData))
				{
				TPckgBuf<TMemSpyThreadInfoItemData> data;
				data.Copy(bufferPtr.Ptr()+offset, sizeof(TMemSpyThreadInfoItemData));
				aInfoItems.AppendL(CMemSpyApiThreadInfoItem::NewLC(data()));
				}
			
			CleanupStack::Pop(aInfoItems.Count());
			CleanupStack::PopAndDestroy(buffer);
			}
		}
	
	User::LeaveIfError(error);
	}

EXPORT_C TInt RMemSpySession::GetThreadInfoItems( RArray<CMemSpyApiThreadInfoItem*> &aInfoItems, TThreadId aId, TMemSpyThreadInfoItemType aType )
	{
	TRAPD(error, GetThreadInfoItemsL(aInfoItems, aId, aType));
	return error;
	}

EXPORT_C void RMemSpySession::GetProcessIdByThreadId( TProcessId& aPID, TThreadId aTID )
	{	
	TPckgBuf<TProcessId> pid(aPID);
	TPckgBuf<TThreadId> tid(aTID);
	
	TIpcArgs args( &pid, &tid );
	User::LeaveIfError( SendReceive( EMemSpyClientServerOpGetProcessIdByThreadId, args ) );
	aPID = pid();
	}

//Kernel Objects specific operations
EXPORT_C void RMemSpySession::GetKernelObjectsL( RArray<CMemSpyApiKernelObject*> &aKernelObjects )
	{		
	TPckgBuf<TInt> count;
	User::LeaveIfError(SendReceive( EMemSpyClientServerOpGetKernelObjectCount, TIpcArgs(&count) ));
	
	TInt requestedCount = count();
	HBufC8* buffer = HBufC8::NewLC(requestedCount * sizeof(TMemSpyKernelObjectData));
	TPtr8 bufferPtr(buffer->Des());
		
	TIpcArgs args( &count, &bufferPtr );
	User::LeaveIfError(SendReceive( EMemSpyClientServerOpGetKernelObjects, args ));
	
	aKernelObjects.Reset();
	
	for(TInt i=0, offset = 0; i<requestedCount; i++, offset+=sizeof(TMemSpyKernelObjectData))
		{
		TPckgBuf<TMemSpyKernelObjectData> data;
		data.Copy(bufferPtr.Ptr()+offset, sizeof(TMemSpyKernelObjectData));
		aKernelObjects.AppendL(CMemSpyApiKernelObject::NewLC(data()));
		}
	
	CleanupStack::Pop(aKernelObjects.Count());
	CleanupStack::PopAndDestroy(buffer);
	}

EXPORT_C TInt RMemSpySession::GetKernelObjects( RArray<CMemSpyApiKernelObject*> &aKernelObjects )
	{
	TRAPD(error, GetKernelObjectsL(aKernelObjects));
	return error;
	}

EXPORT_C void RMemSpySession::GetKernelObjectItemsL( RArray<CMemSpyApiKernelObjectItem*> &aKernelObjectItems, TMemSpyDriverContainerType aForContainer )
	{
	TPckgBuf<TInt> count;
	TPckgBuf<TMemSpyDriverContainerType> type(aForContainer);
	User::LeaveIfError(SendReceive( EMemSpyClientServerOpGetKernelObjectItemCount, TIpcArgs(&count, &type) ));

	TInt requestedCount = count();
	HBufC8* buffer = HBufC8::NewLC(requestedCount * sizeof(TMemSpyDriverHandleInfoGeneric));
	TPtr8 bufferPtr(buffer->Des());
	
	TIpcArgs args( &count, &type, &bufferPtr );
	User::LeaveIfError(SendReceive( EMemSpyClientServerOpGetKernelObjectItems, args ));
	
	aKernelObjectItems.Reset();
	
	for(TInt i=0, offset = 0; i<requestedCount; i++, offset+=sizeof(TMemSpyDriverHandleInfoGeneric))
		{
		TPckgBuf<TMemSpyDriverHandleInfoGeneric> data;
		data.Copy(bufferPtr.Ptr()+offset, sizeof(TMemSpyDriverHandleInfoGeneric));
		aKernelObjectItems.AppendL( CMemSpyApiKernelObjectItem::NewLC( data() ) );
		}
	CleanupStack::Pop(aKernelObjectItems.Count());
	CleanupStack::PopAndDestroy(buffer);
	}

EXPORT_C TInt RMemSpySession::GetKernelObjectItems( RArray<CMemSpyApiKernelObjectItem*> &aKernelObjectItems, TMemSpyDriverContainerType aForContainer )
	{
	TRAPD(error, GetKernelObjectItemsL(aKernelObjectItems, aForContainer));
	return error;
	}

EXPORT_C void RMemSpySession::GetMemoryTrackingCyclesL(RArray<CMemSpyApiMemoryTrackingCycle*>& aCycles)
	{
	TPckgBuf<TInt> count;
	User::LeaveIfError(SendReceive( EMemSpyClientServerOpGetMemoryTrackingCycleCount, TIpcArgs(&count) ));
	
	TInt requestedCount = count();
	HBufC8* buffer = HBufC8::NewLC(requestedCount * sizeof(TMemSpyMemoryTrackingCycleData));
	TPtr8 bufferPtr(buffer->Des());
		
	TIpcArgs args( &count, &bufferPtr );
	User::LeaveIfError(SendReceive( EMemSpyClientServerOpGetMemoryTrackingCycles, args ));
	
	aCycles.Reset();
	
	for(TInt i=0, offset = 0; i<requestedCount; i++, offset+=sizeof(TMemSpyMemoryTrackingCycleData))
		{
		TPckgBuf<TMemSpyMemoryTrackingCycleData> data;
		data.Copy(bufferPtr.Ptr()+offset, sizeof(TMemSpyMemoryTrackingCycleData));
		aCycles.AppendL(CMemSpyApiMemoryTrackingCycle::NewLC(data()));
		}
	
	CleanupStack::Pop(aCycles.Count());
	CleanupStack::PopAndDestroy(buffer);
	}

EXPORT_C void RMemSpySession::OutputAllContainerContents()
	{
	SendReceive( EMemSpyClientServerOpOutputAllContainerContents );
	}


//Heap specific operations

EXPORT_C CMemSpyApiHeap* RMemSpySession::GetHeapL()
	{
	CMemSpyApiHeap* aHeap;
	
	HBufC8* buffer = HBufC8::NewLC( sizeof(TMemSpyHeapData) );
	TPtr8 bufferPtr(buffer->Des());
	TIpcArgs args( &bufferPtr );
	
	User::LeaveIfError(SendReceive( EMemSpyClientServerOpGetHeap, args ));
	
	TPckgBuf<TMemSpyHeapData> data;
	data.Copy(bufferPtr.Ptr(), sizeof(TMemSpyHeapData));		
	aHeap = CMemSpyApiHeap::NewL( data() );
	
	CleanupStack::PopAndDestroy(buffer);
		
	return aHeap;
	}

EXPORT_C CMemSpyApiHeap* RMemSpySession::GetHeap()
	{
	CMemSpyApiHeap *result = NULL;
	TRAPD(error, result = GetHeapL());
	return error == KErrNone ? result : NULL;
	}



EXPORT_C void RMemSpySession::GetEComCategoriesL(RArray<CMemSpyApiEComCategory*> &aCategories)
    {
    TPckgBuf<TInt> count;
    User::LeaveIfError(SendReceive(EMemSpyClientServerOpGetEComCategoryCount, TIpcArgs(&count)));
    
    TInt requestedCount = count();
    HBufC8* buffer = HBufC8::NewLC(requestedCount * sizeof(TMemSpyEComCategoryData));
    TPtr8 bufferPtr(buffer->Des());
    
    User::LeaveIfError(SendReceive(EMemSpyClientServerOpGetEComCategories, TIpcArgs(&count, &bufferPtr)));
    aCategories.Reset();
    
    TInt receivedCount = Min(count(), requestedCount);
    for(TInt i=0, offset = 0; i<requestedCount; i++, offset+=sizeof(TMemSpyEComCategoryData))
        {
        TPckgBuf<TMemSpyEComCategoryData> data;
        data.Copy(bufferPtr.Ptr()+offset, sizeof(TMemSpyEComCategoryData));
        aCategories.AppendL(CMemSpyApiEComCategory::NewLC(data()));
        }
    CleanupStack::Pop(aCategories.Count());
    CleanupStack::PopAndDestroy(buffer);
    }
    
EXPORT_C void RMemSpySession::GetEComInterfacesL(TUid aCategory, RArray<CMemSpyApiEComInterface*> &aInterfaces)
    {
    TPckgBuf<TInt> count;
    TPckgBuf<TUid> category(aCategory);
    User::LeaveIfError(SendReceive(EMemSpyClientServerOpGetEComInterfaceCount, TIpcArgs(&count, &category)));
    
    TInt requestedCount = count();
    HBufC8* buffer = HBufC8::NewLC(requestedCount * sizeof(TMemSpyEComInterfaceData));
    TPtr8 bufferPtr(buffer->Des());
    
    User::LeaveIfError(SendReceive(EMemSpyClientServerOpGetEComInterfaces, TIpcArgs(&count, &category, &bufferPtr)));
    aInterfaces.Reset();
    
    TInt receivedCount = Min(count(), requestedCount);
    for(TInt i=0, offset = 0; i<requestedCount; i++, offset+=sizeof(TMemSpyEComInterfaceData))
        {
        TPckgBuf<TMemSpyEComInterfaceData> data;
        data.Copy(bufferPtr.Ptr()+offset, sizeof(TMemSpyEComInterfaceData));
        aInterfaces.AppendL(CMemSpyApiEComInterface::NewLC(data()));
        }
    CleanupStack::Pop(aInterfaces.Count());
    CleanupStack::PopAndDestroy(buffer);
    }

EXPORT_C void RMemSpySession::GetEComImplementationsL(TUid aInterface, RArray<CMemSpyApiEComImplementation*> &aImplementations)
    {
    TPckgBuf<TInt> count;
    TPckgBuf<TUid> interface(aInterface);
    User::LeaveIfError(SendReceive(EMemSpyClientServerOpGetEComImplementationCount, TIpcArgs(&count, &interface)));
    
    TInt requestedCount = count();
    HBufC8* buffer = HBufC8::NewLC(requestedCount * sizeof(TMemSpyEComImplementationData));
    TPtr8 bufferPtr(buffer->Des());
    
    User::LeaveIfError(SendReceive(EMemSpyClientServerOpGetEComImplementations, TIpcArgs(&count, &interface, &bufferPtr)));
    aImplementations.Reset();
    
    TInt receivedCount = Min(count(), requestedCount);
    for(TInt i=0, offset = 0; i<requestedCount; i++, offset+=sizeof(TMemSpyEComImplementationData))
        {
        TPckgBuf<TMemSpyEComImplementationData> data;
        data.Copy(bufferPtr.Ptr()+offset, sizeof(TMemSpyEComImplementationData));
        aImplementations.AppendL(CMemSpyApiEComImplementation::NewLC(data()));
        }
    CleanupStack::Pop(aImplementations.Count());
    CleanupStack::PopAndDestroy(buffer);
    }

EXPORT_C void RMemSpySession::GetWindowGroupsL(RArray<CMemSpyApiWindowGroup*> &aGroups)
    {
    TPckgBuf<TInt> count;
    User::LeaveIfError(SendReceive(EMemSpyClientServerOpGetWindowGroupCount, TIpcArgs(&count)));
    
    TInt requestedCount = count();
    HBufC8* buffer = HBufC8::NewLC(requestedCount * sizeof(TMemSpyEngineWindowGroupDetails));
    TPtr8 bufferPtr(buffer->Des());
    
    User::LeaveIfError(SendReceive(EMemSpyClientServerOpGetWindowGroups, TIpcArgs(&count, &bufferPtr)));
    aGroups.Reset();
    
    TInt receivedCount = Min(count(), requestedCount);
    for(TInt i=0, offset = 0; i<requestedCount; i++, offset+=sizeof(TMemSpyEngineWindowGroupDetails))
        {
        TPckgBuf<TMemSpyEngineWindowGroupDetails> data;
        data.Copy(bufferPtr.Ptr()+offset, sizeof(TMemSpyEngineWindowGroupDetails));
        aGroups.AppendL(CMemSpyApiWindowGroup::NewLC(data()));
        }
    CleanupStack::Pop(aGroups.Count());
    CleanupStack::PopAndDestroy(buffer);
    }

EXPORT_C void RMemSpySession::DumpKernelHeap()
	{
	SendReceive( EMemSpyClientServerOpDumpKernelHeap );
	}

EXPORT_C void RMemSpySession::OutputKernelHeapDataL()
	{		
	User::LeaveIfError(SendReceive(EMemSpyClientServerOpHeapData | KMemSpyOpFlagsIncludesThreadId,
			TIpcArgs(KMemSpyClientServerThreadIdKernel)));
	
	}

EXPORT_C void RMemSpySession::OutputKernelHeapData(TRequestStatus& aStatus)
	{
	SendReceive(EMemSpyClientServerOpHeapData,
		TIpcArgs(KMemSpyClientServerThreadIdKernel),
		aStatus);
	}

EXPORT_C void RMemSpySession::OutputThreadHeapDataL( TThreadId aThreadId)
	{			
	User::LeaveIfError(SendReceive(EMemSpyClientServerOpHeapData | KMemSpyOpFlagsIncludesThreadId,
			TIpcArgs(aThreadId)));
	}

EXPORT_C void RMemSpySession::OutputThreadHeapDataL(const TDesC& aThreadName)
	{	
	const TIpcArgs args( &aThreadName );
	        
	User::LeaveIfError( SendReceive( EMemSpyClientServerOpHeapData | KMemSpyOpFlagsIncludesThreadName, args ));	
	}

EXPORT_C void RMemSpySession::OutputThreadCellListL(TThreadId aThreadId)
	{	
	User::LeaveIfError(SendReceive(EMemSpyClientServerOpHeapCellListing | KMemSpyOpFlagsIncludesThreadId,
			TIpcArgs(aThreadId)));
	}

EXPORT_C void RMemSpySession::OutputHeapInfoUserL(TThreadId aThreadId)
	{
	User::LeaveIfError(SendReceive( EMemSpyClientServerOpHeapInfo | KMemSpyOpFlagsIncludesThreadId,
			TIpcArgs(aThreadId)));
	}

EXPORT_C void RMemSpySession::SwitchOutputSinkL( TMemSpySinkType aType )
	{
	TInt op;
	if( aType == ESinkTypeFile )
		op = EMemSpyClientServerOpSwitchOutputSinkFile;
	else
		op = EMemSpyClientServerOpSwitchOutputSinkTrace;
			
	User::LeaveIfError(SendReceive( op ));
	}

EXPORT_C void RMemSpySession::SwitchOutputToTraceL()
	{
	User::LeaveIfError(SendReceive(EMemSpyClientServerOpSwitchOutputSinkTrace));
	}
    
EXPORT_C void RMemSpySession::SwitchOutputToFileL(const TDesC& aRootFolder)
	{
	TIpcArgs args;
	if (aRootFolder.Length())
		{
		args.Set(0, &aRootFolder);
		}
	
	User::LeaveIfError(SendReceive(EMemSpyClientServerOpSwitchOutputSinkFile, args));
	}

EXPORT_C void RMemSpySession::OutputStackInfoL(TThreadId aThreadId)
	{
	User::LeaveIfError(SendReceive( EMemSpyClientServerOpStackInfo | KMemSpyOpFlagsIncludesThreadId,
			TIpcArgs(aThreadId)));
	}

EXPORT_C void RMemSpySession::OutputStackDataL(TThreadId aThreadId, TMemSpyDriverDomainType aType )
	{
	TInt op;
	if( aType == EMemSpyDriverDomainUser )
		op = EMemSpyClientServerOpStackDataUser;
	else
		op = EMemSpyClientServerOpStackDataKernel;
	
	User::LeaveIfError(SendReceive( op | KMemSpyOpFlagsIncludesThreadId,
			TIpcArgs(aThreadId, aType)));
		
	}

EXPORT_C void RMemSpySession::OutputThreadInfoHandlesL(TThreadId aThreadId)
	{
	TPckgBuf<TThreadId> id(aThreadId);
	User::LeaveIfError(SendReceive(EMemSpyClientServerOpOutputInfoHandles, TIpcArgs( &id )));	
	}

EXPORT_C void RMemSpySession::OutputAOListL(TThreadId aId, TMemSpyThreadInfoItemType aType)
	{
	TPckgBuf<TThreadId> id(aId);
	TPckgBuf<TMemSpyThreadInfoItemType> type(aType);
	
	User::LeaveIfError(SendReceive(EMemSpyClientServerOpOutputAOList, TIpcArgs( &id, &type )));
	}

EXPORT_C void RMemSpySession::OutputKernelObjectsL()
	{
	User::LeaveIfError(SendReceive(EMemSpyClientServerOpEnumerateKernelContainerAll));
	}

EXPORT_C void RMemSpySession::OutputCompactStackInfoL()
	{
	User::LeaveIfError(SendReceive(EMemSpyClientServerOpStackInfoCompact)); 
	}

EXPORT_C void RMemSpySession::OutputCompactHeapInfoL()
	{
	User::LeaveIfError(SendReceive(EMemSpyClientServerOpHeapInfoCompact)); 
	}
// Asynchronous operations
EXPORT_C void RMemSpySession::OutputPhoneInfo(TRequestStatus& aStatus)
	{	
	SendReceive(EMemSpyClientServerOpSummaryInfo | KMemSpyOpFlagsAsyncOperation, TIpcArgs(), aStatus);
	}

EXPORT_C void RMemSpySession::OutputDetailedPhoneInfo(TRequestStatus& aStatus)
	{	
	SendReceive(EMemSpyClientServerOpSummaryInfoDetailed | KMemSpyOpFlagsAsyncOperation, TIpcArgs(), aStatus);
	}

EXPORT_C void RMemSpySession::OutputHeapInfo(TRequestStatus& aStatus)
	{	
	SendReceive(EMemSpyClientServerOpHeapInfo | KMemSpyOpFlagsAsyncOperation, TIpcArgs(), aStatus);
	}

EXPORT_C void RMemSpySession::OutputCompactHeapInfo(TRequestStatus& aStatus)
	{	
	SendReceive(EMemSpyClientServerOpHeapInfoCompact | KMemSpyOpFlagsAsyncOperation, TIpcArgs(), aStatus);
	}

EXPORT_C void RMemSpySession::OutputHeapCellListing(TRequestStatus& aStatus)
	{	
	SendReceive(EMemSpyClientServerOpHeapCellListing | KMemSpyOpFlagsAsyncOperation, TIpcArgs(), aStatus);
	}

EXPORT_C void RMemSpySession::OutputHeapData(TRequestStatus& aStatus)
	{	
	SendReceive(EMemSpyClientServerOpHeapData | KMemSpyOpFlagsAsyncOperation, TIpcArgs(), aStatus);
	}

// synchronous version of the operation - for CLI
EXPORT_C void RMemSpySession::OutputHeapData()
	{
	SendReceive(EMemSpyClientServerOpHeapData);
	}

EXPORT_C void RMemSpySession::OutputStackInfo(TRequestStatus& aStatus)
	{	
	SendReceive(EMemSpyClientServerOpStackInfo | KMemSpyOpFlagsAsyncOperation, TIpcArgs(), aStatus);
	}

EXPORT_C void RMemSpySession::OutputCompactStackInfo(TRequestStatus& aStatus)
	{	
	SendReceive(EMemSpyClientServerOpStackInfoCompact | KMemSpyOpFlagsAsyncOperation, TIpcArgs(), aStatus);
	}

EXPORT_C void RMemSpySession::OutputUserStackData(TRequestStatus& aStatus)
	{	
	SendReceive(EMemSpyClientServerOpStackDataUser | KMemSpyOpFlagsAsyncOperation, TIpcArgs(), aStatus);
	}

EXPORT_C void RMemSpySession::OutputKernelStackData(TRequestStatus& aStatus)
	{	
	SendReceive(EMemSpyClientServerOpStackDataKernel | KMemSpyOpFlagsAsyncOperation, TIpcArgs(), aStatus);
	}

// Synchronous operations
EXPORT_C void RMemSpySession::OutputPhoneInfo()
	{	
	SendReceive( EMemSpyClientServerOpSummaryInfo , TIpcArgs() );
	}

EXPORT_C TInt RMemSpySession::GetSwmtCyclesCount()
	{
	TPckgBuf<TInt> count;
	TIpcArgs args( &count );
		
	User::LeaveIfError(SendReceive( EMemSpyClientServerOpGetMemoryTrackingCycleCount, args ));
	
	return count();
	}

EXPORT_C void RMemSpySession::SetSwmtAutoStartProcessList( CArrayFixFlat<TUid>* aList )
	{
	TInt count = aList->Count();
	TIpcArgs args( &aList, &count );
	
	SendReceive( EMemSpyClientServerOpSetSwmtAutoStartProcessList, args );
	}

EXPORT_C void RMemSpySession::SwmtResetTracking()
	{
	SendReceive( EMemSpyClientServerOpSystemWideMemoryTrackingReset );
	}

EXPORT_C void RMemSpySession::GetOutputSink( TMemSpySinkType& aType )
	{
	TPckgBuf<TMemSpySinkType> type( aType );
	TIpcArgs args( &type );
	
	SendReceive( EMemSpyClientServerOpGetOutputSink, args );
	
	aType = type();
	}

EXPORT_C void RMemSpySession::NotifyDeviceWideOperationProgress(TMemSpyDeviceWideOperationProgress &aProgress, TRequestStatus &aStatus)
	{
	SendReceive(EMemSpyClientServerOpNotifyDeviceWideOperationProgress | KMemSpyOpFlagsAsyncOperation,
			TIpcArgs(&aProgress.iProgress, &aProgress.iDescription), 
			aStatus);
	}

EXPORT_C void RMemSpySession::CancelDeviceWideOperationL()
	{
	User::LeaveIfError(SendReceive(EMemSpyClientServerOpCancelDeviceWideOperation));
	}

// SWMT operations
EXPORT_C void RMemSpySession::SetSwmtCategoriesL(TInt aCategories)
	{
	User::LeaveIfError(SendReceive(EMemSpyClientServerOpSystemWideMemoryTrackingCategoriesSet,
			TIpcArgs(aCategories)));
	}

EXPORT_C void RMemSpySession::SetSwmtHeapDumpsEnabledL(TBool aEnabled)
	{
	User::LeaveIfError(SendReceive(EMemSpyClientServerOpSystemWideMemoryTrackingHeapDumpSet,
			TIpcArgs(aEnabled)));
	}

EXPORT_C void RMemSpySession::SetSwmtMode(TMemSpyEngineHelperSysMemTrackerConfig::TMemSpyEngineSysMemTrackerMode aMode)
	{
	TPckgBuf<TMemSpyEngineHelperSysMemTrackerConfig::TMemSpyEngineSysMemTrackerMode> mode( aMode );
	TIpcArgs args(&mode);
	User::LeaveIfError(SendReceive(EMemSpyClientServerOpSystemWideMemoryTrackingModeSet, args));
	}

EXPORT_C TBool RMemSpySession::IsSwmtRunningL()
	{
	TPckgBuf<TBool> ret;
	User::LeaveIfError(SendReceive(EMemSpyClientServerOpIsSwmtRunning, TIpcArgs(&ret)));
		
	return ret();
	}

EXPORT_C void RMemSpySession::StartSwmtTimerL(TInt aPeriod)
	{
	SetSwmtTimerIntervalL(aPeriod);
	User::LeaveIfError(SendReceive(EMemSpyClientServerOpSystemWideMemoryTrackingTimerStart));
	}

EXPORT_C void RMemSpySession::StartSwmtTimer( TRequestStatus& aStatus )
	{
	SendReceive( EMemSpyClientServerOpSystemWideMemoryTrackingTimerStart, 
			TIpcArgs(), aStatus );
	}

EXPORT_C void RMemSpySession::StartSwmtTimerL()
	{
	User::LeaveIfError(SendReceive(EMemSpyClientServerOpSystemWideMemoryTrackingTimerStart));
	}

EXPORT_C void RMemSpySession::SetSwmtTimerIntervalL(TInt aPeriod)
	{
	User::LeaveIfError(SendReceive(EMemSpyClientServerOpSystemWideMemoryTrackingTimerPeriodSet,
			TIpcArgs(aPeriod)));
	}

EXPORT_C void RMemSpySession::StopSwmtTimerL()
	{
	User::LeaveIfError(SendReceive(EMemSpyClientServerOpSystemWideMemoryTrackingTimerStop));
	}


EXPORT_C void RMemSpySession::ForceSwmtUpdateL()
	{
	User::LeaveIfError(SendReceive(EMemSpyClientServerOpSystemWideMemoryTrackingForceUpdate));
	}

EXPORT_C void RMemSpySession::ForceSwmtUpdate(TRequestStatus& aStatus)
	{
	SendReceive(EMemSpyClientServerOpSystemWideMemoryTrackingForceUpdate, 
			TIpcArgs(),
			aStatus);
	}

EXPORT_C void RMemSpySession::SetSwmtFilter( const TDesC& aFilter )
	{	
	TIpcArgs args( &aFilter );
	User::LeaveIfError( SendReceive( EMemSpyClientServerOpSystemWideMemoryTrackingThreadNameFilterSet, args ) );
	}

EXPORT_C void RMemSpySession::GetSwmtFilter( TName& aFilter )
	{
    TPckgBuf<TName> name;
    TIpcArgs args( &name );
		User::LeaveIfError( SendReceive( EMemSpyClientServerOpSystemWideMemoryTrackingThreadNameFilterGet, args ) );
	aFilter = name();
	}
        
EXPORT_C void RMemSpySession::GetSwmtCategoriesL(TInt& aCategories)
	{
	TPckgBuf<TInt> cat;	
	TIpcArgs args( &cat );
		User::LeaveIfError( SendReceive( EMemSpyClientServerOpSystemWideMemoryTrackingCategoriesGet, args ) );
	aCategories = cat();
	}

EXPORT_C void RMemSpySession::GetSwmtHeapDumpsEnabledL(TBool& aEnabled)
	{
	TPckgBuf<TBool> enabled;
	TIpcArgs args( &enabled );
		User::LeaveIfError( SendReceive( EMemSpyClientServerOpSystemWideMemoryTrackingHeapDumpGet, args ) );
		
	aEnabled = enabled();
	}
        
EXPORT_C void RMemSpySession::GetSwmtMode(TMemSpyEngineHelperSysMemTrackerConfig::TMemSpyEngineSysMemTrackerMode& aMode)
	{
	TPckgBuf<TMemSpyEngineHelperSysMemTrackerConfig::TMemSpyEngineSysMemTrackerMode> mode;
	TIpcArgs args( &mode );
		User::LeaveIfError( SendReceive( EMemSpyClientServerOpSystemWideMemoryTrackingModeGet, args ) );
	aMode = mode();
	}
    
EXPORT_C void RMemSpySession::GetSwmtTimerIntervalL(TInt& aPeriod)
	{
	TPckgBuf<TInt> time;
	TIpcArgs args( &time );
	User::LeaveIfError(SendReceive(EMemSpyClientServerOpSystemWideMemoryTrackingTimerPeriodGet,
			args));
	aPeriod = time();
	}

EXPORT_C TInt TMemSpyDeviceWideOperationProgress::Progress() const 
	{
	return iProgress();
	}

EXPORT_C const TDesC& TMemSpyDeviceWideOperationProgress::Description() const
	{
	return iDescription;
	}

// Servers
EXPORT_C void RMemSpySession::GetServersL(RArray<CMemSpyApiServer*> &aServers)
    {
    TPckgBuf<TInt> count;
    User::LeaveIfError(SendReceive(EMemSpyClientServerOpGetServerCount, TIpcArgs(&count)));
    
    TInt requestedCount = count();
    HBufC8* buffer = HBufC8::NewLC(requestedCount * sizeof(TMemSpyServerData));
    TPtr8 bufferPtr(buffer->Des());
    
    User::LeaveIfError(SendReceive(EMemSpyClientServerOpGetServers, TIpcArgs(&count, &bufferPtr)));
    aServers.Reset();
    
    TInt receivedCount = Min(count(), requestedCount);
    for(TInt i=0, offset = 0; i<requestedCount; i++, offset+=sizeof(TMemSpyServerData))
        {
        TPckgBuf<TMemSpyServerData> data;
        data.Copy(bufferPtr.Ptr()+offset, sizeof(TMemSpyServerData));
        aServers.AppendL(CMemSpyApiServer::NewLC(data()));
        }
    CleanupStack::Pop(aServers.Count());
    CleanupStack::PopAndDestroy(buffer);
    }

EXPORT_C void RMemSpySession::GetServersL(RArray<CMemSpyApiServer*> &aServers, TSortType aSortType)
	{
	TPckgBuf<TInt> count;
	TPckgBuf<TSortType> sort( aSortType );
	User::LeaveIfError(SendReceive(EMemSpyClientServerOpGetServerCount, TIpcArgs(&count)));
	    	
	TInt requestedCount = count();
	HBufC8* buffer = HBufC8::NewLC(requestedCount * sizeof(TMemSpyServerData));
	TPtr8 bufferPtr(buffer->Des());
	    
	User::LeaveIfError(SendReceive(EMemSpyClientServerOpGetSortedServers, TIpcArgs(&count, &bufferPtr, &sort)));
	aServers.Reset();
	    
	TInt receivedCount = Min(count(), requestedCount);
	for(TInt i=0, offset = 0; i<requestedCount; i++, offset+=sizeof(TMemSpyServerData))
		{
		TPckgBuf<TMemSpyServerData> data;
		data.Copy(bufferPtr.Ptr()+offset, sizeof(TMemSpyServerData));
		aServers.AppendL(CMemSpyApiServer::NewLC(data()));
		}
	CleanupStack::Pop(aServers.Count());
	CleanupStack::PopAndDestroy(buffer);
	}

EXPORT_C void RMemSpySession::ServerListOutputGenericL( TBool aDetailed )
	{
	TPckgBuf<TBool> detailed( aDetailed );
	TIpcArgs args( &detailed );
	User::LeaveIfError( SendReceive( EMemSpyClientServerOpServerListOutputGeneric, args ) );
	}

