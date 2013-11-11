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


#ifndef MEMSPYSESSION_H
#define MEMSPYSESSION_H


// System includes
#include <e32base.h>
#include <u32std.h>

//user includes
#include <memspy/api/memspyapiprocess.h>

#include <memspy/api/memspyapithread.h>
#include <memspy/api/memspyapithreadinfoitem.h>
#include <memspy/engine/memspythreadinfoitemdata.h>
#include <memspy/engine/memspyengineobjectthreadinfoobjects.h>
#include <memspy/engine/memspydevicewideoperations.h>

#include <memspy/api/memspyapikernelobject.h>

#include <memspy/api/memspyapikernelobjectitem.h> //for KernelObjectItems

#include <memspy/api/memspyapiheap.h>

#include <memspy/api/memspyapimemorytrackingcycle.h>

#include <memspyengineclientinterface.h>
#include <memspy/engine/memspyengineoutputsinktype.h>

#include <memspy/engine/memspyenginehelpersysmemtrackerconfig.h>

#include <memspy/api/memspyapiserver.h>

#include <memspy/api/memspyapiecom.h>

#include <memspy/api/memspyapiwindowgroup.h>

// Constants
const TInt KMemSpyVersion           = 2;

enum TSortType
	{
	ESortProcById,
	ESortProcByName,
	ESortProcByThreadCount,
	ESortProcByCodeSegs,
	ESortProcByHeapUsage,
	ESortProcByStackUsage,
	ESortServByName,
	ESortServBySessionCount
	};

enum TMemSpyOutputType
    {
    EOutputTypeDebug = 0,
    EOutputTypeFile
    };

class TMemSpyDeviceWideOperationProgress 
	{
public:
	IMPORT_C TInt Progress() const;
	IMPORT_C const TDesC& Description() const;
	
private:
	TPckgBuf<TInt> iProgress;
	TFullName iDescription;
	
friend class RMemSpySession;
	};


NONSHARABLE_CLASS( RMemSpySession ) : public RSessionBase
    {
public:
    IMPORT_C RMemSpySession();
    IMPORT_C TInt Connect();
    
public:	//API
    //Thread speciifc operations
    IMPORT_C void OutputKernelHeapDataL(); //EMemSpyClientServerOpHeapData
    
    IMPORT_C void OutputKernelHeapData(TRequestStatus& aStatus); //EMemSpyClientServerOpHeapData
    
    IMPORT_C void OutputThreadHeapDataL(TThreadId aThreadId); //EMemSpyClientServerOpHeapData
    
    IMPORT_C void OutputThreadHeapDataL(const TDesC& aThreadName); //EMemSpyClientServerOpHeapData
    
    IMPORT_C void OutputThreadCellListL(TThreadId aThreadId);//EMemSpyClientServerOpHeapCellListing    
    
    IMPORT_C void OutputHeapInfoUserL(TThreadId aThreadId);	//EMemSpyClientServerOpHeapInfo
    
    IMPORT_C void SwitchOutputSinkL( TMemSpySinkType aType); //EMemSpyClientServerOpSwitchOutputSinkFile / EMemSpyClientServerOpSwitchOutputSinkTrace
    
    IMPORT_C void SwitchOutputToTraceL(); // EMemSpyClientServerOpSwitchOutputSinkTrace
    
    IMPORT_C void SwitchOutputToFileL(const TDesC& aRootFolder); // EMemSpyClientServerOpSwitchOutputSinkFile
    
    IMPORT_C void OutputStackInfoL(TThreadId aThreadId); //EMemSpyClientServerOpStackInfo
    
    IMPORT_C void OutputStackDataL(TThreadId aThreadId, TMemSpyDriverDomainType aType ); //EMemSpyClientServerOpStackDataUser / EMemSpyClientServerOpStackDataKernel    
    
    IMPORT_C void OutputThreadInfoHandlesL(TThreadId aThreadId); //EMemSpyClientServerOpOutputInfoHandles
    
    IMPORT_C void OutputAOListL(TThreadId aId, TMemSpyThreadInfoItemType aType);	//EMemSpyClientServerOpOutputAOList    
    
    IMPORT_C void OutputKernelObjectsL();// EMemSpyClientServerOpEnumerateKernelContainerAll
    
    IMPORT_C void OutputCompactStackInfoL();// EMemSpyClientServerOpStackInfoCompact
    
    IMPORT_C void OutputCompactHeapInfoL();// EMemSpyClientServerOpHeapInfoCompact
    
    // Device Wide Operations
    // Synchronous operations - for CLI
    IMPORT_C void OutputHeapData();
    
    // Asynchronous operations
    IMPORT_C void OutputPhoneInfo(TRequestStatus& aStatus);
    
    IMPORT_C void OutputDetailedPhoneInfo(TRequestStatus& aStatus);
    
    IMPORT_C void OutputHeapInfo(TRequestStatus& aStatus);
    
    IMPORT_C void OutputCompactHeapInfo(TRequestStatus &aStatus);
    
    IMPORT_C void OutputHeapCellListing(TRequestStatus& aStatus);
    
    IMPORT_C void OutputHeapData(TRequestStatus& aStatus);
    
    IMPORT_C void OutputStackInfo(TRequestStatus& aStatus);
    
    IMPORT_C void OutputCompactStackInfo(TRequestStatus &aStatus);
    
    IMPORT_C void OutputUserStackData(TRequestStatus& aStatus);
    
    IMPORT_C void OutputKernelStackData(TRequestStatus& aStatus);
    
    IMPORT_C void NotifyDeviceWideOperationProgress(TMemSpyDeviceWideOperationProgress &aProgress, TRequestStatus &aStatus);
    
    IMPORT_C void CancelDeviceWideOperationL();
    
    // Synchronous operations for MemSpyLauncher
    IMPORT_C void OutputPhoneInfo();
    
    // "Ui" operations
    
    IMPORT_C void GetProcessesL(RArray<CMemSpyApiProcess*> &aProcesses, TSortType aSortType = ESortProcById);
    
    IMPORT_C TProcessId GetProcessIdByNameL(const TDesC& aProcessName);
    
    IMPORT_C void GetThreadsL(TProcessId aProcessId, RArray<CMemSpyApiThread*> &aThreads, TSortType aSortType = ESortProcById);
    
    IMPORT_C TInt ProcessSystemPermanentOrCritical( TProcessId aId, TBool aValue ); //aValue -> return value
    
    IMPORT_C void SetThreadPriorityL(TThreadId aId, TInt aPriority);
    
    IMPORT_C TInt EndProcessL( TProcessId aId, TMemSpyEndType aType );
    
    IMPORT_C TInt SwitchToProcess( TProcessId aId, TBool aBrought  );
    
    IMPORT_C void GetProcessIdByThreadId( TProcessId& aPID, TThreadId aTID );
    
    //SWMT operations
    
    IMPORT_C void GetMemoryTrackingCyclesL(RArray<CMemSpyApiMemoryTrackingCycle*>& aCycles);
    
    IMPORT_C TInt GetSwmtCyclesCount();
    
    IMPORT_C void SetSwmtAutoStartProcessList( CArrayFixFlat<TUid>* aList );
    
    IMPORT_C void SetSwmtFilter( const TDesC& aFilter );            
    
    IMPORT_C void SetSwmtCategoriesL(TInt aCategories);

    IMPORT_C void SetSwmtHeapDumpsEnabledL(TBool aEnabled);
    
    IMPORT_C void SetSwmtMode(TMemSpyEngineHelperSysMemTrackerConfig::TMemSpyEngineSysMemTrackerMode aMode);      
    
    IMPORT_C void SwmtResetTracking();
    
    IMPORT_C void GetOutputSink( TMemSpySinkType& aType );
           
    IMPORT_C TBool IsSwmtRunningL();
    
    IMPORT_C void StartSwmtTimerL(TInt aPeriod);
    
    IMPORT_C void StartSwmtTimer( TRequestStatus& aStatus );
    
    IMPORT_C void StartSwmtTimerL(); // for CLI
    
    IMPORT_C void SetSwmtTimerIntervalL(TInt aPeriod); //for CLI
    
    IMPORT_C void StopSwmtTimerL();
    
    IMPORT_C void ForceSwmtUpdateL();
    
    IMPORT_C void ForceSwmtUpdate(TRequestStatus& aStatus);
    
    IMPORT_C void GetSwmtFilter( TName& aFilter );            
        
    IMPORT_C void GetSwmtCategoriesL(TInt& aCategories);

    IMPORT_C void GetSwmtHeapDumpsEnabledL(TBool& aEnabled);
        
    IMPORT_C void GetSwmtMode(TMemSpyEngineHelperSysMemTrackerConfig::TMemSpyEngineSysMemTrackerMode& aMode);
    
    IMPORT_C void GetSwmtTimerIntervalL(TInt& aPeriod);
    
    //Threads operations
    /**
     * 
     */
    IMPORT_C void GetThreadsL(TProcessId aProcessId, RArray<CMemSpyApiProcess*> &aThreads, TSortType aSortType = ESortProcById);
    
    IMPORT_C TInt ThreadSystemPermanentOrCritical( TThreadId aId, TBool aValue );
        
    IMPORT_C TInt EndThreadL( TThreadId aId, TMemSpyEndType aType );
    
    IMPORT_C TInt SwitchToThread( TThreadId aId, TBool aBrought );
    
    IMPORT_C TInt GetInfoItemType( TInt aIndex, TThreadId aId, TMemSpyThreadInfoItemType &aType );
    
    IMPORT_C TInt GetThreadInfoItems( RArray<CMemSpyApiThreadInfoItem*> &aInfoItems, TThreadId aId, TMemSpyThreadInfoItemType aType );
    
    IMPORT_C void GetThreadInfoItemsL( RArray<CMemSpyApiThreadInfoItem*> &aInfoItems, TThreadId aId, TMemSpyThreadInfoItemType aType );            
    
    //KernelObjects operations
    
    IMPORT_C void GetKernelObjectsL( RArray<CMemSpyApiKernelObject*> &aKernelObjects );
    
    IMPORT_C TInt GetKernelObjects( RArray<CMemSpyApiKernelObject*> &aKernelObjects );
    
    IMPORT_C void GetKernelObjectItemsL( RArray<CMemSpyApiKernelObjectItem*> &aKernelObjectItems, TMemSpyDriverContainerType aForContainer );
    
    IMPORT_C TInt GetKernelObjectItems( RArray<CMemSpyApiKernelObjectItem*> &aKernelObjectItems, TMemSpyDriverContainerType aForContainer );
    
    IMPORT_C void OutputAllContainerContents();
    
    // Heap 
    
    IMPORT_C CMemSpyApiHeap* GetHeapL();
    
	IMPORT_C CMemSpyApiHeap* GetHeap();
	
	IMPORT_C void DumpKernelHeap();
	
	// Servers
	IMPORT_C void GetServersL(RArray<CMemSpyApiServer*> &aServers);
	
	IMPORT_C void GetServersL(RArray<CMemSpyApiServer*> &aServers, TSortType aSortType); 
	
	IMPORT_C void ServerListOutputGenericL( TBool aDetailed );
		
	// ECom

	IMPORT_C void GetEComCategoriesL(RArray<CMemSpyApiEComCategory*> &aCategories);
	
	IMPORT_C void GetEComInterfacesL(TUid aCategory, RArray<CMemSpyApiEComInterface*> &aInterfaces);
	
	IMPORT_C void GetEComImplementationsL(TUid aInterface, RArray<CMemSpyApiEComImplementation*> &aImplementations);
	
	// Window Groups
	IMPORT_C void GetWindowGroupsL(RArray<CMemSpyApiWindowGroup*> &aGroups);
	
private:
    TInt StartServer();       
    
    void SetOutputTypeL(TMemSpyOutputType aOutputType);
    
    };

#endif // MEMSPYSESSION_H
