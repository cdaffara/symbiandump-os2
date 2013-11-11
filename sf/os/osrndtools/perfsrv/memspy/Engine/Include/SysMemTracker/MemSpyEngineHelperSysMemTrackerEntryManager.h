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

#ifndef MEMSPYENGINEHELPERSYSMEMTRACKERENTRYMANAGER_H
#define MEMSPYENGINEHELPERSYSMEMTRACKERENTRYMANAGER_H

// System includes
#include <e32base.h>

// User includes
#include <memspy/engine/memspyenginehelpersysmemtrackerenums.h>

// Classes referenced
class CMemSpyEngine;
class CMemSpyThread;
class CMemSpyProcess;
class TMemSpyDriverChunkInfo;
class CMemSpyEngineChunkList;
class CMemSpyEngineHelperSysMemTrackerImp;
class CMemSpyEngineHelperSysMemTrackerCycle;
class CMemSpyEngineHelperSysMemTrackerEntry;




NONSHARABLE_CLASS( CMemSpyEngineHelperSysMemTrackerEntryManager ) : public CBase
    {
public:
    static CMemSpyEngineHelperSysMemTrackerEntryManager* NewL( CMemSpyEngineHelperSysMemTrackerImp& aTracker );
    static CMemSpyEngineHelperSysMemTrackerEntryManager* NewL( CMemSpyEngineHelperSysMemTrackerImp& aTracker, CMemSpyEngineHelperSysMemTrackerEntryManager& aMasterList );
    ~CMemSpyEngineHelperSysMemTrackerEntryManager();

protected:
    CMemSpyEngineHelperSysMemTrackerEntryManager( CMemSpyEngineHelperSysMemTrackerImp& aTracker, CMemSpyEngineHelperSysMemTrackerEntryManager* aMasterList = NULL );
    void ConstructL();

public: // API
    void IdentifyChangesL( CMemSpyEngineHelperSysMemTrackerCycle& aCycle );
    void EverythingHasChangedL( CMemSpyEngineHelperSysMemTrackerCycle& aCycle );

private: // Internal methods
    void ProcessChangesL( CMemSpyEngineHelperSysMemTrackerCycle& aCycle );
    void AddItemAndPopL( CMemSpyEngineHelperSysMemTrackerEntry* aItem );
    void MarkEverythingDead();
    void MergeListIntoMeL( CMemSpyEngineHelperSysMemTrackerEntryManager& aOtherList );
    TInt FindByHandle( TUint aHandle, TMemSpyEngineSysMemTrackerType aType ) const;
    TInt FindByThreadId( const TThreadId& aThreadId ) const;
    CMemSpyEngineHelperSysMemTrackerEntry* EntryByKey( const TUint64& aKey );
    CMemSpyEngine& Engine();

private: // Internal access
    inline TBool ActingAsSecondaryList() const { return iMasterList != NULL; }

private: // Sort order
    static TInt CompareKey( const CMemSpyEngineHelperSysMemTrackerEntry& aLeft, const CMemSpyEngineHelperSysMemTrackerEntry& aRight );
    static TInt CompareKeyAndTimeStamp( const CMemSpyEngineHelperSysMemTrackerEntry& aLeft, const CMemSpyEngineHelperSysMemTrackerEntry& aRight );

private: // Seed creation
    void CreateSeedItemsL();
    void CreateSeedItemsHeapUserL( CMemSpyEngineChunkList& aList );
    void CreateSeedItemsHeapUserL( CMemSpyProcess& aProcess, CMemSpyEngineChunkList* aList = NULL );
    void CreateSeedItemsHeapUserL( CMemSpyThread& aThread, CMemSpyEngineChunkList* aList = NULL );
    void CreateSeedItemsHeapKernelL( CMemSpyEngineChunkList& aList );
    void CreateSeedItemsChunkLocalL( CMemSpyEngineChunkList& aList );
    void CreateSeedItemsChunkGlobalL( CMemSpyEngineChunkList& aList );
    void CreateSeedItemsGlobalDataL( CMemSpyEngineChunkList& aList );
    void CreateSeedItemRamDriveL( CMemSpyEngineChunkList& aList );
    void CreateSeedItemsBitmapL( CMemSpyEngineChunkList& aList );
    void CreateSeedItemsStacksL( CMemSpyEngineChunkList& aList );
    void CreateSeedItemsStacksL( CMemSpyThread& aThread, const TMemSpyDriverChunkInfo& aStackChunkInfo );
    void CreateSeedItemsFileServerCacheL( CMemSpyEngineChunkList& aList );
    void CreateSeedItemsCodeL();
    void CreateSeedItemsHandlesL();
    void CreateSeedItemsOpenFilesL();
    void CreateSeedItemsDiskSpaceL();
    void CreateSeedItemsSystemMemoryL();
    void CreateSeedItemsWindowServerL();

private:
    CMemSpyEngineHelperSysMemTrackerImp& iTracker;
    CMemSpyEngineHelperSysMemTrackerEntryManager* iMasterList;
    RPointerArray< CMemSpyEngineHelperSysMemTrackerEntry > iEntries;
    };


#endif