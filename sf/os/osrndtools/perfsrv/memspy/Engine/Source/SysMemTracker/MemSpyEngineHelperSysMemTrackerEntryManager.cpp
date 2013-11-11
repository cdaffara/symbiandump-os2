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

#include "MemSpyEngineHelperSysMemTrackerEntryManager.h"

// System includes
#include <e32debug.h>

// Driver includes
#include <memspy/driver/memspydriverclient.h>

// User includes
#include <memspy/engine/memspyengine.h>
#include <memspy/engine/memspyengineutils.h>
#include <memspy/engine/memspyenginemidwife.h>
#include <memspy/engine/memspyengineundertaker.h>
#include <memspy/engine/memspyengineobjectthread.h>
#include <memspy/engine/memspyengineobjectprocess.h>
#include <memspy/engine/memspyengineobjectcontainer.h>
#include <memspy/engine/memspyenginehelperheap.h>
#include <memspy/engine/memspyenginehelperchunk.h>
#include <memspy/engine/memspyenginehelperfbserv.h>
#include <memspy/engine/memspyenginehelperfilesystem.h>
#include <memspy/engine/memspyenginehelpercodesegment.h>
#include <memspy/engine/memspyenginehelperkernelcontainers.h>
#include <memspy/engine/memspyenginehelperwindowserver.h>
#include <memspy/engine/memspyenginehelpersysmemtracker.h>
#include "MemSpyEngineHelperSysMemTrackerImp.h"
#include "MemSpyEngineHelperSysMemTrackerLog.h"
#include <memspy/engine/memspyenginehelpersysmemtrackercycle.h>
#include <memspy/engine/memspyenginehelpersysmemtrackercyclechange.h>
#include "MemSpyEngineHelperSysMemTrackerEntries.h"
#include "MemSpyEngineHelperSysMemTrackerEntryChunk.h"
#include "MemSpyEngineHelperSysMemTrackerEntryGlobalData.h"
#include "MemSpyEngineHelperSysMemTrackerEntryHeap.h"
#include "MemSpyEngineHelperSysMemTrackerEntryRamDrive.h"
#include "MemSpyEngineHelperSysMemTrackerEntryStack.h"
#include "MemSpyEngineHelperSysMemTrackerEntryCode.h"
#include "MemSpyEngineHelperSysMemTrackerEntryHandleGeneric.h"
#include "MemSpyEngineHelperSysMemTrackerEntryHandlePAndS.h"
#include "MemSpyEngineHelperSysMemTrackerEntryOpenFile.h"
#include "MemSpyEngineHelperSysMemTrackerEntryDiskSpace.h"
#include "MemSpyEngineHelperSysMemTrackerEntryBitmap.h"
#include "MemSpyEngineHelperSysMemTrackerEntryFbserv.h"
#include "MemSpyEngineHelperSysMemTrackerEntryFileServerCache.h"
#include "MemSpyEngineHelperSysMemTrackerEntrySystemMemory.h"
#include "MemSpyEngineHelperSysMemTrackerEntryWindowServer.h"


// Constants

// Message printed to RDebug output for some clients that may be reading traces 
_LIT( KMemSpyKeepaliveMessage, "<MEMSPY_PROGRESS>" );


CMemSpyEngineHelperSysMemTrackerEntryManager::CMemSpyEngineHelperSysMemTrackerEntryManager( CMemSpyEngineHelperSysMemTrackerImp& aTracker, CMemSpyEngineHelperSysMemTrackerEntryManager* aMasterList )
:   iTracker( aTracker ), iMasterList( aMasterList )
    {
    }

    
CMemSpyEngineHelperSysMemTrackerEntryManager::~CMemSpyEngineHelperSysMemTrackerEntryManager()
    {
    iEntries.ResetAndDestroy();
    iEntries.Close();
    }


void CMemSpyEngineHelperSysMemTrackerEntryManager::ConstructL()
    {
    CreateSeedItemsL();
    }


CMemSpyEngineHelperSysMemTrackerEntryManager* CMemSpyEngineHelperSysMemTrackerEntryManager::NewL( CMemSpyEngineHelperSysMemTrackerImp& aTracker )
    {
    CMemSpyEngineHelperSysMemTrackerEntryManager* self = new(ELeave) CMemSpyEngineHelperSysMemTrackerEntryManager( aTracker );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


CMemSpyEngineHelperSysMemTrackerEntryManager* CMemSpyEngineHelperSysMemTrackerEntryManager::NewL( CMemSpyEngineHelperSysMemTrackerImp& aTracker, CMemSpyEngineHelperSysMemTrackerEntryManager& aMasterList )
    {
    CMemSpyEngineHelperSysMemTrackerEntryManager* self = new(ELeave) CMemSpyEngineHelperSysMemTrackerEntryManager( aTracker, &aMasterList );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


void CMemSpyEngineHelperSysMemTrackerEntryManager::IdentifyChangesL( CMemSpyEngineHelperSysMemTrackerCycle& aCycle )
    {
    // Mark everything in this list (i.e. the master list) as dead.
    MarkEverythingDead();

    // Create a new list which will act as a secondary list. This new list will only contain
    // new entries. Any entries which it attempts to create that already exist in the master list
    // will be discared (after updating the corresponding master list entry with new values).
    CMemSpyEngineHelperSysMemTrackerEntryManager* newEM = CMemSpyEngineHelperSysMemTrackerEntryManager::NewL( iTracker, *this );
    CleanupStack::PushL( newEM );

    // At this point, we have performed several key operations
    // 
    // 1) We have tagged everything that is still alive as "alive".
    // 2) We have left everything that no longer exists as "dead"
    // 3) We have updated all alive entries with new values (if they have changed)
    // 4) We have identified new entries (these are currently orphaned within the secondary list right now).
    //
    // We now need to merge the two lists, i.e. take all the new entries in 'newEM' and merge them
    // into the current master list.
    MergeListIntoMeL( *newEM );

    // We can throw the other list away now as it will be empty.
    CleanupStack::PopAndDestroy( newEM );

    // At this point we have our final new list, but it still contains dead entries.
    // We must next output all the change information required for this cycle.
    ProcessChangesL( aCycle );
    }


void CMemSpyEngineHelperSysMemTrackerEntryManager::EverythingHasChangedL( CMemSpyEngineHelperSysMemTrackerCycle& aCycle )
    {
    ProcessChangesL( aCycle );
    }























void CMemSpyEngineHelperSysMemTrackerEntryManager::ProcessChangesL( CMemSpyEngineHelperSysMemTrackerCycle& aCycle )
    {
    const TInt count = iEntries.Count();
    for( TInt i=count-1; i>=0; i-- )
        {
        CMemSpyEngineHelperSysMemTrackerEntry* entry = iEntries[ i ];
        
        // Create any change descriptors etc
        TRAP_IGNORE( entry->HandleNewCycleL( aCycle ) );

        // Destroy the entry if it's no longer needed
        if  ( entry->IsDead() )
            {
            delete entry;
            iEntries.Remove( i );
            }
        else
            {
            // It's not new anymore
            entry->SetNew( EFalse );
            }
        }
    }


void CMemSpyEngineHelperSysMemTrackerEntryManager::AddItemAndPopL( CMemSpyEngineHelperSysMemTrackerEntry* aItem )
    {
    // We sort by entry key, we do not allow duplicates
    TLinearOrder<CMemSpyEngineHelperSysMemTrackerEntry> orderer( CompareKey );

    // Before we save the entry we must check to see if the master list knows about it.
    const TUint64 key( aItem->Key() );

    // Operates in one of two ways. If we're in stand-alone mode, then it saves every specified entry.
    // If we're in secondary mode (i.e. we have an associated master list) then it only saves new entries.
    // Entries which are changed are updated (in the master list) and entries that don't exist anymore
    // are tagged as dead.
    //
    if  ( ActingAsSecondaryList() )
        {
        CMemSpyEngineHelperSysMemTrackerEntry* existingEntry = iMasterList->EntryByKey( key );
        if  ( existingEntry )
            {
            // Entry used to exist, still does. Update the master list entry with the information
            // from the transient (new) entry.
            existingEntry->UpdateFromL( *aItem );

            // Entry is still alive. Previously, at the start of the cycle, we'd tagged all master list
            // entries as dead, hence we must undo that if we find the entry really still exists...
            // NB: this also updates the timestamp for the entry, to show when it was last still alive.
            existingEntry->SetAlive();

            // But it definitely isn't new anymore as we've seen it at least once before
            existingEntry->SetNew( EFalse );

            // Transient entry not needed anymore, keep original
            CleanupStack::PopAndDestroy( aItem );
            }
        else
            {
            // Entry didn't exist before - it's a new one
            iEntries.InsertInOrderL( aItem, orderer );
            CleanupStack::Pop( aItem );
            }

        // Any entries in the master list which aren't present anymore in this list (aka, dead entries)
        // will remain tagged as dead and will be filtered out and dealt with shortly...
        }
    else
        {
        // We ARE the master list
        const TInt err = iEntries.InsertInOrder( aItem, orderer );
        if  ( err == KErrAlreadyExists )
            {
            // Don't allow duplicates
            RDebug::Printf( "CMemSpyEngineHelperSysMemTrackerEntryManager::AddItemAndPopL() - ******* duplicate key ******* key: %LU, type: %d, tid: 0x%08x, pid: 0x%08x, handle: 0x%08x", key, aItem->Type(), (TUint32) aItem->ThreadId(), (TUint32) aItem->ProcessId(), aItem->Handle() );
            delete aItem;
            }
        else if ( err != KErrNone )
            {
            User::Leave( err );
            }

        CleanupStack::Pop( aItem );
        }
    }


void CMemSpyEngineHelperSysMemTrackerEntryManager::MarkEverythingDead()
    {
    const TInt count = iEntries.Count();
    for( TInt i=count-1; i>=0; i-- )
        {
        CMemSpyEngineHelperSysMemTrackerEntry* entry = iEntries[ i ];
        entry->SetDead();
        }
    }


void CMemSpyEngineHelperSysMemTrackerEntryManager::MergeListIntoMeL( CMemSpyEngineHelperSysMemTrackerEntryManager& aOtherList )
    {
    const TInt count = aOtherList.iEntries.Count();
    for( TInt i=count-1; i>=0; i-- )
        {
        CMemSpyEngineHelperSysMemTrackerEntry* entry = aOtherList.iEntries[ i ];
        CleanupStack::PushL( entry );
        aOtherList.iEntries.Remove( i );
        AddItemAndPopL( entry );
        }
    }


TInt CMemSpyEngineHelperSysMemTrackerEntryManager::FindByHandle( TUint aHandle, TMemSpyEngineSysMemTrackerType aType ) const
    {
    TInt ret = KErrNotFound;
    //
    const TInt count = iEntries.Count();
    for( TInt i=0; i<count; i++ )
        {
        const CMemSpyEngineHelperSysMemTrackerEntry* entry = iEntries[ i ];
        if  ( entry->Handle() == aHandle && entry->Type() == aType )
            {
            ret = i;
            break;
            }
        }
    //
    return ret;
    }


TInt CMemSpyEngineHelperSysMemTrackerEntryManager::FindByThreadId( const TThreadId& aThreadId ) const
    {
    TInt ret = KErrNotFound;
    //
    const TInt count = iEntries.Count();
    for( TInt i=0; i<count; i++ )
        {
        const CMemSpyEngineHelperSysMemTrackerEntry* entry = iEntries[ i ];
        if  ( entry->ThreadId() == aThreadId )
            {
            ret = i;
            break;
            }
        }
    //
    return ret;
    }


CMemSpyEngineHelperSysMemTrackerEntry* CMemSpyEngineHelperSysMemTrackerEntryManager::EntryByKey( const TUint64& aKey )
    {
    TLinearOrder<CMemSpyEngineHelperSysMemTrackerEntry> orderer( CompareKey );
    //
    CMemSpyEngineHelperSysMemTrackerEntryWithSuppliedKey tempEntry( iTracker, aKey );
    const TInt pos = iEntries.FindInOrder( &tempEntry, orderer );
    //
    CMemSpyEngineHelperSysMemTrackerEntry* ret = NULL;
    if  ( pos >= 0 )
        {
        ret = iEntries[ pos ];
        }
    //
    return ret;
    }


TInt CMemSpyEngineHelperSysMemTrackerEntryManager::CompareKey( const CMemSpyEngineHelperSysMemTrackerEntry& aLeft, const CMemSpyEngineHelperSysMemTrackerEntry& aRight )
    {
    TInt ret = -1;
    //
    const TUint64 kLeft( aLeft.Key() );
    const TUint64 kRight( aRight.Key() );
    //
    if  ( kLeft > kRight )
        {
        ret = 1;
        }
    else if ( kLeft == kRight )
        {
        ret = 0;
        }
    //
    return ret;
    }


TInt CMemSpyEngineHelperSysMemTrackerEntryManager::CompareKeyAndTimeStamp( const CMemSpyEngineHelperSysMemTrackerEntry& aLeft, const CMemSpyEngineHelperSysMemTrackerEntry& aRight )
    {
    TInt ret = CompareKey( aLeft, aRight );
    if  ( ret == 0 )
        {
        // Keys are the same, so compare time stamps
        const TTime& tLeft = aLeft.LastUpdateTime();
        const TTime& tRight = aRight.LastUpdateTime();
        //
        ret = -1;
        if  ( tLeft > tRight )
            {
            ret = 1;
            }
        else if ( tLeft == tRight )
            {
            ret = 0;
            }
        }
    //
    return ret;
    }


CMemSpyEngine& CMemSpyEngineHelperSysMemTrackerEntryManager::Engine()
    {
    return iTracker.Engine();
    }
























void CMemSpyEngineHelperSysMemTrackerEntryManager::CreateSeedItemsL()
    {
    RDebug::Print( KMemSpyKeepaliveMessage );
    
    TMemSpyEngineHelperSysMemTrackerConfig config;
    Engine().HelperSysMemTracker().GetConfig( config );
    
    // Get chunk list for entire device
    CMemSpyEngineChunkList* chunks = Engine().HelperChunk().ListL( );
    CleanupStack::PushL( chunks );

#ifdef SYSMEMTRACKERLOGGING
    {
    const TInt chunkCount = chunks->Count();
    for( TInt i=chunkCount-1; i>=0; i-- )
        {
        const TMemSpyDriverChunkInfo& info = chunks->At( i ).Info();
        RDebug::Print( _L("CMemSpyEngineHelperSysMemTrackerEntryManager::CreateSeedItemsL() - MASTER - info[%03d]: type: %d, size: %d, name: %S"), i, info.iType, info.iSize, &info.iName );
        }
    }
#endif
    
    if ( config.iEnabledCategories & TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryFileServerCache )
        {
        RDebug::Print( KMemSpyKeepaliveMessage );
        // Extract F32 cache chunk
        CreateSeedItemsFileServerCacheL( *chunks );        
        }

#ifdef SYSMEMTRACKERLOGGING
    {
    const TInt chunkCount = chunks->Count();
    for( TInt i=chunkCount-1; i>=0; i-- )
        {
        const TMemSpyDriverChunkInfo& info = chunks->At( i ).Info();
        RDebug::Print( _L("CMemSpyEngineHelperSysMemTrackerEntryManager::CreateSeedItemsL() -  AFTER F32 CACHE - info[%03d]: type: %d, size: %d, name: %S"), i, info.iType, info.iSize, &info.iName );
        }
    }
#endif

// TODO: Uncomment when  bitmap handels are fixed
//    if ( config.iEnabledCategories & TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryBitmapHandles )
//        {
//        RDebug::Print( KMemSpyKeepaliveMessage );
//        // Bitmap
//        CreateSeedItemsBitmapL( *chunks );        
//        }

#ifdef SYSMEMTRACKERLOGGING
    {
    const TInt chunkCount = chunks->Count();
    for( TInt i=chunkCount-1; i>=0; i-- )
        {
        const TMemSpyDriverChunkInfo& info = chunks->At( i ).Info();
        RDebug::Print( _L("CMemSpyEngineHelperSysMemTrackerEntryManager::CreateSeedItemsL() -  AFTER BITMAPS - info[%03d]: type: %d, size: %d, name: %S"), i, info.iType, info.iSize, &info.iName );
        }
    }
#endif

    if ( config.iEnabledCategories & TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryUserHeap )
        {
        RDebug::Print( KMemSpyKeepaliveMessage );
        // Look for user heaps
        CreateSeedItemsHeapUserL( *chunks );
        }

#ifdef SYSMEMTRACKERLOGGING
    {
    const TInt chunkCount = chunks->Count();
    for( TInt i=chunkCount-1; i>=0; i-- )
        {
        const TMemSpyDriverChunkInfo& info = chunks->At( i ).Info();
        RDebug::Print( _L("CMemSpyEngineHelperSysMemTrackerEntryManager::CreateSeedItemsL() -  AFTER USER HEAP - info[%03d]: type: %d, size: %d, name: %S"), i, info.iType, info.iSize, &info.iName );
        }
    }
#endif

//  TODO: Uncomment after fix
//    if ( config.iEnabledCategories & TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryKernelHeap )
//        {
//        RDebug::Print( KMemSpyKeepaliveMessage );
//        // Look for kernel heaps
//        CreateSeedItemsHeapKernelL( *chunks );
//        }

#ifdef SYSMEMTRACKERLOGGING
    {
    const TInt chunkCount = chunks->Count();
    for( TInt i=chunkCount-1; i>=0; i-- )
        {
        const TMemSpyDriverChunkInfo& info = chunks->At( i ).Info();
        RDebug::Print( _L("CMemSpyEngineHelperSysMemTrackerEntryManager::CreateSeedItemsL() -  AFTER KERNEL HEAP - info[%03d]: type: %d, size: %d, name: %S"), i, info.iType, info.iSize, &info.iName );
        }
    }
#endif

    if ( config.iEnabledCategories & TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryLocalChunks )
        {
        RDebug::Print( KMemSpyKeepaliveMessage );
        // Local chunks
        CreateSeedItemsChunkLocalL( *chunks );        
        }

#ifdef SYSMEMTRACKERLOGGING
    {
    const TInt chunkCount = chunks->Count();
    for( TInt i=chunkCount-1; i>=0; i-- )
        {
        const TMemSpyDriverChunkInfo& info = chunks->At( i ).Info();
        RDebug::Print( _L("CMemSpyEngineHelperSysMemTrackerEntryManager::CreateSeedItemsL() -  AFTER LOCL CHUNK - info[%03d]: type: %d, size: %d, name: %S"), i, info.iType, info.iSize, &info.iName );
        }
    }
#endif

    if ( config.iEnabledCategories & TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryGlobalChunks )
        {
        RDebug::Print( KMemSpyKeepaliveMessage );
        // Global chunks
        CreateSeedItemsChunkGlobalL( *chunks );        
        }

#ifdef SYSMEMTRACKERLOGGING
    {
    const TInt chunkCount = chunks->Count();
    for( TInt i=chunkCount-1; i>=0; i-- )
        {
        const TMemSpyDriverChunkInfo& info = chunks->At( i ).Info();
        RDebug::Print( _L("CMemSpyEngineHelperSysMemTrackerEntryManager::CreateSeedItemsL() -  AFTER GLOB CHUNK - info[%03d]: type: %d, size: %d, name: %S"), i, info.iType, info.iSize, &info.iName );
        }
    }
#endif

    if ( config.iEnabledCategories & TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryRAMDrive )
        {
        RDebug::Print( KMemSpyKeepaliveMessage );
        // RAM drive
        CreateSeedItemRamDriveL( *chunks );        
        }

#ifdef SYSMEMTRACKERLOGGING
    {
    const TInt chunkCount = chunks->Count();
    for( TInt i=chunkCount-1; i>=0; i-- )
        {
        const TMemSpyDriverChunkInfo& info = chunks->At( i ).Info();
        RDebug::Print( _L("CMemSpyEngineHelperSysMemTrackerEntryManager::CreateSeedItemsL() -  AFTER RAMD - info[%03d]: type: %d, size: %d, name: %S"), i, info.iType, info.iSize, &info.iName );
        }
    }
#endif

//    TODO: Uncomment after fix
//    if ( config.iEnabledCategories & TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryUserStacks )
//        {
//        RDebug::Print( KMemSpyKeepaliveMessage );
//        // Stacks ($DAT)
//        CreateSeedItemsStacksL( *chunks );        
//        }

#ifdef SYSMEMTRACKERLOGGING
     {
   const TInt chunkCount = chunks->Count();
    for( TInt i=chunkCount-1; i>=0; i-- )
        {
        const TMemSpyDriverChunkInfo& info = chunks->At( i ).Info();
        RDebug::Print( _L("CMemSpyEngineHelperSysMemTrackerEntryManager::CreateSeedItemsL() -  AFTER STACKS - info[%03d]: type: %d, size: %d, name: %S"), i, info.iType, info.iSize, &info.iName );
        }
    }
#endif

//  TODO: Uncomment after fix
//    if ( config.iEnabledCategories & TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryGlobalData )
//        {
//        RDebug::Print( KMemSpyKeepaliveMessage );
//        // Global data (DLL$DATA)
//        CreateSeedItemsGlobalDataL( *chunks );        
//        }

 #ifdef SYSMEMTRACKERLOGGING
    {
    const TInt chunkCount = chunks->Count();
    for( TInt i=chunkCount-1; i>=0; i-- )
        {
        const TMemSpyDriverChunkInfo& info = chunks->At( i ).Info();
        RDebug::Print( _L("CMemSpyEngineHelperSysMemTrackerEntryManager::CreateSeedItemsL() -  AFTER GLOBAL DATA - info[%03d]: type: %d, size: %d, name: %S"), i, info.iType, info.iSize, &info.iName );
        }
    }
#endif

    // Don't need the chunks anymore
    CleanupStack::PopAndDestroy( chunks );

    if ( config.iEnabledCategories & TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryRAMLoadedCode )
        {
        RDebug::Print( KMemSpyKeepaliveMessage );
        // Code
        CreateSeedItemsCodeL();        
        }

    if ( config.iEnabledCategories & TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryKernelHandles )
        {
        // Handles
        RDebug::Print( KMemSpyKeepaliveMessage);
        CreateSeedItemsHandlesL();        
        }

    if ( config.iEnabledCategories & TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryOpenFiles )
        {
        RDebug::Print( KMemSpyKeepaliveMessage );
        // Open files
        CreateSeedItemsOpenFilesL();        
        }

    if ( config.iEnabledCategories & TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryDiskusage )
        {
        RDebug::Print( KMemSpyKeepaliveMessage );
        // Disk space
        CreateSeedItemsDiskSpaceL();        
        }

    if ( config.iEnabledCategories & TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategorySystemMemory )
        {
        RDebug::Print( KMemSpyKeepaliveMessage );
        // System memory
        CreateSeedItemsSystemMemoryL();        
        }
	
    if ( config.iEnabledCategories & TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryWindowGroups )
        {
        RDebug::Print( KMemSpyKeepaliveMessage );
        // Window Server
        CreateSeedItemsWindowServerL();        
        }
    }


void CMemSpyEngineHelperSysMemTrackerEntryManager::CreateSeedItemsHeapUserL( CMemSpyEngineChunkList& aList )
    {
    CMemSpyEngine& engine = Engine();
    const TInt procCount = engine.Container().Count();
    for(TInt i=0; i<procCount; i++)
        {
        if ( !( i % 10 ) ) // Print the message on only every 10th cycle
            {
            RDebug::Print( KMemSpyKeepaliveMessage  );
            }
        CMemSpyProcess& process = engine.Container().At( i );
        TRAP_IGNORE( CreateSeedItemsHeapUserL( process, &aList ) );
        }
    }


void CMemSpyEngineHelperSysMemTrackerEntryManager::CreateSeedItemsHeapUserL( CMemSpyProcess& aProcess, CMemSpyEngineChunkList* aList )
    {
    SYSMEMTRACKERLOG_PRINTF( RDebug::Printf( "CMemSpyEngineHelperSysMemTrackerEntryManager::CreateSeedItemsHeapUserL() - START - aProcess: %d", (TUint) aProcess.Id() ) );
    
    CMemSpyEngine& engine = Engine();
    engine.ProcessSuspendLC( aProcess.Id() );

    const TInt threadCount = aProcess.Count();
    SYSMEMTRACKERLOG_PRINTF( RDebug::Printf( "CMemSpyEngineHelperSysMemTrackerEntryManager::CreateSeedItemsHeapUserL() - threadCount: %d", threadCount ) );

    for( TInt j=0; j<threadCount; j++ )
        {
        CMemSpyThread& thread = aProcess.At( j );
        //
        if  ( !thread.IsDead() )
            {
            TRAP_IGNORE( CreateSeedItemsHeapUserL( thread, aList ) );
            }
        }
    
    CleanupStack::PopAndDestroy(); // ProcessSuspendLC

    SYSMEMTRACKERLOG_PRINTF( RDebug::Printf( "CMemSpyEngineHelperSysMemTrackerEntryManager::CreateSeedItemsHeapUserL() - END" ) );
    }


void CMemSpyEngineHelperSysMemTrackerEntryManager::CreateSeedItemsHeapUserL( CMemSpyThread& aThread, CMemSpyEngineChunkList* aList )
    {
    TBool exists = FindByThreadId( aThread.Id() ) != KErrNotFound;
    //
    if  ( !exists )
        {
        CMemSpyEngineHelperSysMemTrackerEntryHeap* entry = CMemSpyEngineHelperSysMemTrackerEntryHeap::NewUserLC( iTracker, aThread );

        // Check if chunk already logged
        TInt entryIndex = FindByHandle( entry->Handle(), EMemSpyEngineSysMemTrackerTypeHeapUser ); 
        exists =  entryIndex != KErrNotFound;
        SYSMEMTRACKERLOG_PRINTF( RDebug::Printf( "CMemSpyEngineHelperSysMemTrackerEntryManager::CreateSeedItemsHeapUserL() - heapChunkHandle: 0x%08x, already exists: %d", entry->Handle(), exists ) );
        if ( exists )
            {
            CMemSpyEngineHelperSysMemTrackerEntryHeap& sharedheapEntry = static_cast< CMemSpyEngineHelperSysMemTrackerEntryHeap& >( *iEntries[ entryIndex ] );
            sharedheapEntry.SetAsShared( ETrue );
            }

        // Remove utilised entries
        if  ( aList )
            {
            aList->RemoveByHandle( (TAny*) entry->Handle() );
            }

        // Save
        if  ( !exists )
            {
            AddItemAndPopL( entry );
            }
        else
            {
            CleanupStack::PopAndDestroy( entry );
            }
        }
    }


void CMemSpyEngineHelperSysMemTrackerEntryManager::CreateSeedItemsHeapKernelL( CMemSpyEngineChunkList& aList )
    {
    SYSMEMTRACKERLOG_PRINTF( RDebug::Printf( "CMemSpyEngineHelperSysMemTrackerEntryManager::CreateSeedItemsHeapKernelL() - START" ) );

    CMemSpyEngine& engine = Engine();
    CMemSpyEngineHelperSysMemTrackerEntryHeap* entry = CMemSpyEngineHelperSysMemTrackerEntryHeap::NewKernelLC( iTracker );
    AddItemAndPopL( entry );

    // Remove utilised entries
    aList.RemoveByHandle( (TAny*) entry->Handle() );

    SYSMEMTRACKERLOG_PRINTF( RDebug::Printf( "CMemSpyEngineHelperSysMemTrackerEntryManager::CreateSeedItemsHeapKernelL() - END" ) );
    }


void CMemSpyEngineHelperSysMemTrackerEntryManager::CreateSeedItemsChunkLocalL( CMemSpyEngineChunkList& aList )
    {
    SYSMEMTRACKERLOG_PRINTF( RDebug::Printf( "CMemSpyEngineHelperSysMemTrackerEntryManager::CreateSeedItemsChunkL() - START" ) );

    const TInt chunkCount = aList.Count();
    for( TInt i=chunkCount-1; i>=0; i-- )
        {
        const TMemSpyDriverChunkInfo& info = aList.At( i ).Info();
        SYSMEMTRACKERLOG_PRINTF( RDebug::Print( _L("CMemSpyEngineHelperSysMemTrackerEntryManager::CreateSeedItemsChunkLocalL() - info[%03d]: type: %d, pid: 0x%04x, size: %d, name: %S"), i, info.iType, info.iOwnerId, info.iSize, &info.iName ) );
        //
        if  ( info.iType == EMemSpyDriverChunkTypeLocal )
            {
            // Try to find corresponding process
            const TInt processIndex = Engine().Container().ProcessIndexById( info.iOwnerId );
            SYSMEMTRACKERLOG_PRINTF( RDebug::Print( _L("CMemSpyEngineHelperSysMemTrackerEntryManager::CreateSeedItemsChunkLocalL() - processIndex: %d"), processIndex ) );
            //
            if  ( processIndex >= 0 )
                {
                CMemSpyProcess& process = Engine().Container().At( processIndex );
                //
                TRAP_IGNORE(
                    CMemSpyEngineHelperSysMemTrackerEntryChunk* entry = CMemSpyEngineHelperSysMemTrackerEntryChunk::NewLC( iTracker, info, process );
                    AddItemAndPopL( entry );
                    );
                //
                aList.Remove( i );
                }
            }
        }

    SYSMEMTRACKERLOG_PRINTF( RDebug::Printf( "CMemSpyEngineHelperSysMemTrackerEntryManager::CreateSeedItemsChunkL() - END" ) );
    }


void CMemSpyEngineHelperSysMemTrackerEntryManager::CreateSeedItemsChunkGlobalL( CMemSpyEngineChunkList& aList )
    {
    const TInt chunkCount = aList.Count();
    for( TInt i=chunkCount-1; i>=0; i-- )
        {
        const TMemSpyDriverChunkInfo& info = aList.At( i ).Info();
        if  ( info.iType == EMemSpyDriverChunkTypeGlobal )
            {
            if  ( CMemSpyEngineHelperFbServ::IsSharedBitmapChunk( info ) || CMemSpyEngineHelperFbServ::IsLargeBitmapChunk( info ) )
                {
                // Don't process these two yet, they'll be handled by the bitmap code
                }
            else
                {
                TRAP_IGNORE(
                    CMemSpyEngineHelperSysMemTrackerEntryChunk* entry = CMemSpyEngineHelperSysMemTrackerEntryChunk::NewLC( iTracker, info );
                    AddItemAndPopL( entry );
                    );

                // Remove processed item
                aList.Remove( i );
                }
            }
        }
    }


void CMemSpyEngineHelperSysMemTrackerEntryManager::CreateSeedItemRamDriveL( CMemSpyEngineChunkList& aList )
    {
    const TInt chunkCount = aList.Count();
    for( TInt i=chunkCount-1; i>=0; i-- )
        {
        const TMemSpyDriverChunkInfo& info = aList.At( i ).Info();
        if  ( info.iType == EMemSpyDriverChunkTypeRamDrive )
            {
            TRAP_IGNORE(
                CMemSpyEngineHelperSysMemTrackerEntryRamDrive* entry = CMemSpyEngineHelperSysMemTrackerEntryRamDrive::NewLC( iTracker, info );
                AddItemAndPopL( entry );
                );

            // Remove processed item
            aList.Remove( i );
            }
        }
    }


void CMemSpyEngineHelperSysMemTrackerEntryManager::CreateSeedItemsStacksL( CMemSpyEngineChunkList& aList )
    {
    CMemSpyEngine& engine = Engine();
    CMemSpyEngineObjectContainer& container = engine.Container();
    //
    const TInt procCount = container.Count();
    for( TInt p=0; p<procCount; p++ )
        {
        CMemSpyProcess& process = container.At( p );

        // We've now sized the user-mode stacks for the process.
        // Try to locate the stack/data chunk
        const TInt index = aList.ItemIndexByProcessId( process.Id(), EMemSpyDriverChunkTypeStackAndProcessGlobalData );
        if  ( index >= 0 )
            {
            const TMemSpyDriverChunkInfo& stackChunkInfo = aList.At( index ).Info();
            //
            const TInt threadCount = process.Count();
            for( TInt t=0; t<threadCount; t++ )
                {
                CMemSpyThread& thread = process.At( t );
                //
                if  ( !thread.IsDead() )
                    {
                    TRAP_IGNORE( CreateSeedItemsStacksL( thread, stackChunkInfo ) );
                    }
                }
            }
        }
    }


void CMemSpyEngineHelperSysMemTrackerEntryManager::CreateSeedItemsStacksL( CMemSpyThread& aThread, const TMemSpyDriverChunkInfo& aStackChunkInfo )
    {
    RThread rThread;
    if  ( aThread.Open( rThread ) == KErrNone )
        {
        CleanupClosePushL( rThread );

        TThreadStackInfo stackInfo;
        if  ( rThread.StackInfo( stackInfo ) == KErrNone )
            {
            CMemSpyEngineHelperSysMemTrackerEntryStack* entry = CMemSpyEngineHelperSysMemTrackerEntryStack::NewLC( iTracker, aThread, aStackChunkInfo, stackInfo );
            AddItemAndPopL( entry );
            }

        CleanupStack::PopAndDestroy( &rThread );
        }
    }


void CMemSpyEngineHelperSysMemTrackerEntryManager::CreateSeedItemsGlobalDataL( CMemSpyEngineChunkList& aList )
    {
    CMemSpyEngine& engine = Engine();
    CMemSpyEngineObjectContainer& container = engine.Container();
    //
    const TInt chunkCount = aList.Count();
    for( TInt i=chunkCount-1; i>=0; i-- )
        {
        const TMemSpyDriverChunkInfo& info = aList.At( i ).Info();

        if  ( info.iType == EMemSpyDriverChunkTypeGlobalData || info.iType == EMemSpyDriverChunkTypeStackAndProcessGlobalData )
            {
            // Need to get associated process
            const TInt processIndex = container.ProcessIndexById( info.iOwnerId );
            if  ( processIndex >= 0 )
                {
                CMemSpyProcess& process = container.At( processIndex );
                //
                if  ( info.iType == EMemSpyDriverChunkTypeGlobalData )
                    {
                    if ( !( i % 10 ) ) // Print the message on only every 10th cycle
                        {
                        RDebug::Print( KMemSpyKeepaliveMessage  );
                        }
                    // Pure global data, unique chunk
                    TRAP_IGNORE( 
                        CMemSpyEngineHelperSysMemTrackerGlobalData* entry = CMemSpyEngineHelperSysMemTrackerGlobalData::NewLC( iTracker, process, info );
                        AddItemAndPopL( entry );
                        );

                    // Remove processed item
                    aList.Remove( i );
                    }
                else if ( info.iType == EMemSpyDriverChunkTypeStackAndProcessGlobalData )
                    {
                    if ( !( i % 10 ) ) // Print the message on only every 10th cycle
                        {
                        RDebug::Print( KMemSpyKeepaliveMessage  );
                        }
                    // We don't check uniqueness as any stack entries share a common handle with the process
                    // global data.
                    TRAP_IGNORE( 
                        CMemSpyEngineHelperSysMemTrackerGlobalData* entry = CMemSpyEngineHelperSysMemTrackerGlobalData::NewLC( iTracker, process, info );
                        AddItemAndPopL( entry );
                        );

                    // Remove processed item
                    aList.Remove( i );
                    }
                }
            }
        }
    }


void CMemSpyEngineHelperSysMemTrackerEntryManager::CreateSeedItemsCodeL()
    {
    CMemSpyEngineCodeSegList* code = Engine().HelperCodeSegment().CodeSegmentListRamLoadedL();
    CleanupStack::PushL( code );
    //
    const TInt count = code->Count();
    for( TInt i=0; i<count; i++ )
        {
        const CMemSpyEngineCodeSegEntry& info = code->At( i );
        //
        TRAP_IGNORE( 
            CMemSpyEngineHelperSysMemTrackerEntryCode* entry = CMemSpyEngineHelperSysMemTrackerEntryCode::NewLC( iTracker, info );
            AddItemAndPopL( entry );
            );
        }
    //
    CleanupStack::PopAndDestroy( code );
    }


void CMemSpyEngineHelperSysMemTrackerEntryManager::CreateSeedItemsHandlesL()
    {
    CMemSpyEngineGenericKernelObjectContainer* allContainers = Engine().HelperKernelContainers().ObjectsAllLightweightL();
    CleanupStack::PushL( allContainers );
    //
    const TInt count = allContainers->Count();
    for( TInt i=0; i<count; i++ )
        {
        const CMemSpyEngineGenericKernelObjectList& list = allContainers->At( i );
        //
        const TInt entryCount = list.Count();
        for( TInt j=0; j<entryCount; j++ )
            {
            TAny* handle = list.HandleAt( j );
            CMemSpyEngineHelperSysMemTrackerEntry* entry = NULL;
            //
            TRAP_IGNORE( 
                if  ( list.Type() == EMemSpyDriverContainerTypePropertyRef )
                    {
                    entry = CMemSpyEngineHelperSysMemTrackerEntryHandlePAndS::NewLC( iTracker, handle );
                    }
                else
                    {
                    entry = CMemSpyEngineHelperSysMemTrackerEntryHandleGeneric::NewLC( iTracker, handle, list.Type() );
                    }

                AddItemAndPopL( entry );
                );
            }
        }
    //
    CleanupStack::PopAndDestroy( allContainers );
    }


void CMemSpyEngineHelperSysMemTrackerEntryManager::CreateSeedItemsOpenFilesL()
    {
    CMemSpyEngineOpenFileList* list = Engine().HelperFileSystem().ListOpenFilesLC();
    //
    const TInt threadCount = list->Count();
    for( TInt t=0; t<threadCount; t++ )
        {
        const CMemSpyEngineOpenFileListForThread& thread = list->At( t );
        //
        const TInt entryCount = thread.Count();
        for( TInt e=0; e<entryCount; e++ )
            {
            const CMemSpyEngineOpenFileListEntry& fileEntry = thread.At( e );
            //
            TRAP_IGNORE( 
                CMemSpyEngineHelperSysMemTrackerEntryOpenFile* entry = CMemSpyEngineHelperSysMemTrackerEntryOpenFile::NewLC( iTracker, thread, fileEntry );
                AddItemAndPopL( entry );
                );
            }
        }
    //
    CleanupStack::PopAndDestroy( list );
    }


void CMemSpyEngineHelperSysMemTrackerEntryManager::CreateSeedItemsDiskSpaceL()
    {
    TDriveList list;
    //
    RFs& fsSession = Engine().FsSession();
    if  ( fsSession.DriveList( list ) == KErrNone )
        {
        TDriveInfo info;
        //
        for( TInt i=0; i<KMaxDrives; i++ )
            {
            if  ( list[ i ] != KDriveAbsent )
                {
                if  ( fsSession.Drive( info, i ) == KErrNone )
                    {
                    TRAP_IGNORE( 
                        CMemSpyEngineHelperSysMemTrackerEntryDiskSpace* entry = CMemSpyEngineHelperSysMemTrackerEntryDiskSpace::NewLC( iTracker, static_cast<TDriveNumber>( i ) );
                        AddItemAndPopL( entry );
                        );
                    }
                }
            }
        }
    }


void CMemSpyEngineHelperSysMemTrackerEntryManager::CreateSeedItemsSystemMemoryL()
    {
    TRAP_IGNORE( 
        CMemSpyEngineHelperSysMemTrackerEntrySystemMemory* entryT = CMemSpyEngineHelperSysMemTrackerEntrySystemMemory::NewLC( iTracker, ETypeTotal );
        AddItemAndPopL( entryT );
        );
    TRAP_IGNORE( 
        CMemSpyEngineHelperSysMemTrackerEntrySystemMemory* entryF = CMemSpyEngineHelperSysMemTrackerEntrySystemMemory::NewLC( iTracker, ETypeFree );
        AddItemAndPopL( entryF );
        );
    }


void CMemSpyEngineHelperSysMemTrackerEntryManager::CreateSeedItemsBitmapL( CMemSpyEngineChunkList& aList )
    {
    TInt bitmapConCount = 0;
    TInt fontConCount = 0;
    //
    RArray<TInt> handles;
    CleanupClosePushL( handles );
    Engine().HelperFbServ().GetArrayOfBitmapHandlesL( handles, bitmapConCount, fontConCount );
    //
    const TInt count = handles.Count();
    for( TInt i=0; i<count; i++ )
        {
        const TInt handle = handles[ i ];
        //
        TRAP_IGNORE( 
            CMemSpyEngineHelperSysMemTrackerEntryBitmap* entry = CMemSpyEngineHelperSysMemTrackerEntryBitmap::NewLC( iTracker, handle );
            AddItemAndPopL( entry );
            );
        }
    //
    CleanupStack::PopAndDestroy( &handles );
    
    // Make overall fbserv info item
    TMemSpyDriverChunkInfo infoChunkLarge;
    TMemSpyDriverChunkInfo infoChunkShared;
    //
    const TInt chunkCount = aList.Count();
    for( TInt i=chunkCount-1; i>=0; i-- )
        {
        const TMemSpyDriverChunkInfo& info = aList.At( i ).Info();
        if  ( CMemSpyEngineHelperFbServ::IsSharedBitmapChunk( info ) )
            {
            SYSMEMTRACKERLOG_PRINTF( RDebug::Printf( "CMemSpyEngineHelperSysMemTrackerEntryManager::CreateSeedItemsBitmapL() - found FbsSharedChunk..." ) );
            infoChunkShared = info;
            aList.Remove( i );
            }
        else if ( CMemSpyEngineHelperFbServ::IsLargeBitmapChunk( info ) )
            {
            SYSMEMTRACKERLOG_PRINTF( RDebug::Printf( "CMemSpyEngineHelperSysMemTrackerEntryManager::CreateSeedItemsBitmapL() - found FbsLargeChunk..." ) );
            infoChunkLarge = info;
            aList.Remove( i );
            }
        }

    TRAP_IGNORE( 
        CMemSpyEngineHelperSysMemTrackerEntryFbserv* entry = CMemSpyEngineHelperSysMemTrackerEntryFbserv::NewLC( iTracker, infoChunkLarge, infoChunkShared, bitmapConCount, fontConCount, count );
        AddItemAndPopL( entry );
        );
    }


void CMemSpyEngineHelperSysMemTrackerEntryManager::CreateSeedItemsFileServerCacheL( CMemSpyEngineChunkList& aList )
    {
    TInt index = KErrNotFound;
    const CMemSpyEngineChunkEntry* entry = Engine().HelperFileSystem().IdentifyCacheChunk( aList, index );
    //
    if  ( entry )
        {
        const TMemSpyDriverChunkInfo& info = entry->Info();
        SYSMEMTRACKERLOG_PRINTF( RDebug::Printf( "CMemSpyEngineHelperSysMemTrackerEntryManager::CreateSeedItemsFileServerCacheL() - found F32 cache chunk..." ) );
        //
        TRAP_IGNORE( 
            CMemSpyEngineHelperSysMemTrackerEntryFileServerCache* entry = CMemSpyEngineHelperSysMemTrackerEntryFileServerCache::NewLC( iTracker, info );
            AddItemAndPopL( entry );
        );
        //
        aList.Remove( index );
        }
    }


void CMemSpyEngineHelperSysMemTrackerEntryManager::CreateSeedItemsWindowServerL()
    {
    if ( Engine().IsHelperWindowServerSupported() )
        {
        MMemSpyEngineHelperWindowServer& windowServerManager = Engine().HelperWindowServer();
        MMemSpyEngineWindowGroupList* windowGroupList = windowServerManager.WindowGroupListL();
        CleanupDeletePushL( windowGroupList );
        TInt count = windowGroupList->Count();
        TBool isFocused( EFalse );
        for ( TInt i = 0; i < count; i++ )
            {
            TMemSpyEngineWindowGroupDetails windowGroupDetails;
            windowServerManager.GetWindowGroupDetailsL( windowGroupList->At( i ).iId, windowGroupDetails );
            windowGroupDetails.iOrdinalPosition = windowGroupList->At( i ).iOrdinalPosition;
            if ( !isFocused && windowGroupDetails.iPriority == 0 )
                {
                windowGroupDetails.iIsFocused = ETrue;
                isFocused = ETrue;
                }
            else
                {
                windowGroupDetails.iIsFocused = EFalse;
                }
            TRAP_IGNORE( 
                CMemSpyEngineHelperSysMemTrackerEntryWindowServer* entry = CMemSpyEngineHelperSysMemTrackerEntryWindowServer::NewLC( iTracker, windowGroupDetails );
                AddItemAndPopL( entry );
                );
            }
        CleanupStack::PopAndDestroy( windowGroupList );
        }
    else
        {
        SYSMEMTRACKERLOG_PRINTF( RDebug::Printf( "CMemSpyEngineHelperSysMemTrackerEntryManager::CreateSeedItemsWindowServerL() - Not supported" ) );
        }
    }




