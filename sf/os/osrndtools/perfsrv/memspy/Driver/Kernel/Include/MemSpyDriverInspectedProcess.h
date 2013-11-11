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

#ifndef MEMSPYINSPECTEDPROCESS_H
#define MEMSPYINSPECTEDPROCESS_H

// System includes
#include <kern_priv.h>

// Shared includes
#include <memspy/driver/memspydriverobjectsshared.h>

// User includes
#include "MemSpyDriverUtils.h"
#include "MemSpyDriverEventMonitor.h"

// Classes referenced
class DMemSpyInspectedProcessManager;
class TMemSpyDriverProcessInspectionInfo;



class TMemSpyTrackedChunk
	{
public:
    enum TType
        {
        ETypeNotRelevant = -1,
        //
        ETypeChunkHeap = 0,
        ETypeChunkLocal,
        ETypeChunkGlobalData,
        ETypeChunkProcessGlobalDataAndUserStack,
        ETypeChunkShared
        };

public:
	TMemSpyTrackedChunk( DChunk* aChunk, TType aType );

public: // API
    void SetSize( TInt aSize );
    TInt Size() const { return iSize; }
    //
    void SetUnused( TBool aUnused );
    TBool Unused() const { return iUnused; }
    //
    TAny* Handle() const { return iChunk; }
    //
    inline TType Type() const { return iType; }
    inline void SetType( TType aType ) { iType = aType; }

public: // Data members
	SDblQueLink iLink;
    DChunk* iChunk;
    TType iType;
    TInt iSize;
    TBool iUnused;
	};


class TMemSpyTrackedChunkChangeInfo
    {
public: // Data members
    TMemSpyDriverProcessInspectionInfo iInfo;
	SDblQueLink iLink;
    };





NONSHARABLE_CLASS(DMemSpyInspectedProcess) : public DBase, public MMemSpyEventMonitorObserver
    {
public: // Construct / destruct
	DMemSpyInspectedProcess( DMemSpyDriverDevice& aDevice );
	~DMemSpyInspectedProcess();

public: // API
    TInt Open( DProcess* aProcess );
    inline TUint ProcessId() const { return iProcessId; }

public: // Change notification API
	TInt NotifyOnChange( DThread* aThread, TRequestStatus* aRequestStatus, TMemSpyDriverProcessInspectionInfo* aInfo );
	TInt NotifyOnChangeCancel();
    TBool NotifyOnChangeQueued() const;

public: // Queue link
	SDblQueLink iLink;

public: // From MMemSpyEventMonitorObserver
    TUint EMTypeMask() const;
    void EMHandleProcessUpdated( DProcess& aProcess );
    void EMHandleProcessRemoved( DProcess& aProcess );
    void EMHandleThreadAdd( DThread& aThread );
    void EMHandleThreadRemoved( DThread& aThread );
    void EMHandleThreadKilled( DThread& aThread );
    void EMHandleChunkAdd( DChunk& aChunk );
    void EMHandleChunkUpdated( DChunk& aChunk );
    void EMHandleChunkDeleted( DChunk& aChunk );

    // Internal helper
    void EMHandleThreadChanged( DThread& aThread );

private: // Tracked item management
    void ResetTrackedList();
    void SetTrackedListUnused();
    void SetTrackedListUnusedStatusByType( TMemSpyTrackedChunk::TType aType, TBool aUnusedStatus );
    void DiscardUnusedTrackListItems();
    void AddTrackedChunk( DChunk* aChunk, TMemSpyTrackedChunk::TType aType );
    TMemSpyTrackedChunk* TrackedChunkByHandle( TAny* aHandle );

private: // Internal methods
    DMemSpyInspectedProcessManager& PManager();
    DMemSpyEventMonitor& EventMonitor();
    TMemSpyTrackedChunk::TType ChunkType( DObject* aObject ) const;
    TBool IsHeapChunk( DChunk& aChunk, const TName& aName ) const;
    static TUint32 TotalStatistics( const TMemSpyDriverProcessInspectionInfo& aStats, TBool aIncludeShared = ETrue );
    static TBool IsEqual( const TMemSpyDriverProcessInspectionInfo& aLeft, const TMemSpyDriverProcessInspectionInfo& aRight );
    void ResetStatistics( TMemSpyDriverProcessInspectionInfo& aStats );
    TBool UpdateStatistics();
    void CompleteClientsRequest( TInt aCompletionCode, TMemSpyDriverProcessInspectionInfo* aInfo = NULL );
    void FindChunks( DProcess& aProcess );
    TInt StackSize( DProcess& aProcess );
    void ResetPendingChanges();
    void PrintChunkInfo( DChunk& aChunk ) const;
    TBool IsChunkRelevantToOurProcess( DChunk& aChunk ) const;
	void Lock() const;
	void Unlock() const;

public: // Queue link for process manager
	SDblQueLink iPMLink;

private: // Data members
    DMemSpyDriverDevice& iDevice;
	DMutex* iLock;
    TUint iProcessId;
    DProcess* iProcess;
    TBool iAmDead;

    // This differs from the 'inspection info' iMemoryStack field
    // because it just contains the size of all user-thread stacks
    // within the process.
    TInt iUserThreadStackSize;

    // Tracking chunks mapped into process
	SDblQue iTrackedChunks;
	
	// Changes that have occurred whilst the client was AWOL
	SDblQue iPendingChanges;

    // Tracking totals
    TMemSpyDriverProcessInspectionInfo iInfoLast;
    TMemSpyDriverProcessInspectionInfo iInfoCurrent;
    TMemSpyDriverProcessInspectionInfo iInfoPeaks;
    TMemSpyDriverProcessInspectionInfo iInfoHWMIncShared;
    TMemSpyDriverProcessInspectionInfo iInfoHWMExcShared;

    // For change notification
    TInt iChangeDeliveryCounter;
	DThread* iChangeObserverThread;
	TRequestStatus* iChangeObserverRS;
    TMemSpyDriverProcessInspectionInfo* iChangeObserverInfo;
    TBool iEventReceivedWhilstObserverWasAWOL;
    };


#endif
