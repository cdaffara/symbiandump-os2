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

#include "MemSpyEngineHelperSysMemTrackerEntryHeap.h"

// System includes
#include <e32base.h>
#include <badesca.h>

// Driver includes
#include <memspy/driver/memspydriverclient.h>

// User includes
#include <memspy/engine/memspyengine.h>
#include <memspy/engine/memspyengineutils.h>
#include <memspy/engine/memspyengineoutputsink.h>
#include <memspy/engine/memspyengineoutputlist.h>
#include <memspy/engine/memspyenginehelperheap.h>
#include <memspy/engine/memspyenginehelperchunk.h>
#include <memspy/engine/memspyengineobjectthread.h>
#include <memspy/engine/memspyengineobjectprocess.h>
#include <memspy/engine/memspyengineobjectcontainer.h>
#include <memspy/engine/memspyenginehelpercodesegment.h>
#include "MemSpyEngineHelperSysMemTrackerImp.h"
#include <memspy/engine/memspyenginehelpersysmemtrackercycle.h>
#include <memspy/engine/memspyenginehelpersysmemtrackerconfig.h>
#include <memspy/engine/memspyenginehelpersysmemtracker.h>









CMemSpyEngineHelperSysMemTrackerEntryHeap::CMemSpyEngineHelperSysMemTrackerEntryHeap( CMemSpyEngineHelperSysMemTrackerImp& aTracker, TMemSpyEngineSysMemTrackerType aType )
:   CMemSpyEngineHelperSysMemTrackerEntry( aTracker, aType )
    {
    }


CMemSpyEngineHelperSysMemTrackerEntryHeap::~CMemSpyEngineHelperSysMemTrackerEntryHeap()
    {
    delete iThreadName;
    }


void CMemSpyEngineHelperSysMemTrackerEntryHeap::ConstructL()
    {
    // Kernel
    TFullName name;
    MemSpyEngineUtils::GetKernelHeapThreadName( name );
    iThreadName = name.AllocL();
    //
    UpdateHeapInfoL( iLast );
    iCurrent = iLast;
    //
    SetHandle( iCurrent.AsRHeap().MetaData().ChunkHandle() );
    }


void CMemSpyEngineHelperSysMemTrackerEntryHeap::ConstructL( CMemSpyThread& aThread )
    {
    TFullName* name = new(ELeave) TFullName();
    CleanupStack::PushL( name );
    aThread.FullName( *name );
    iThreadName = name->AllocL();
    CleanupStack::PopAndDestroy( name );
    //
    SetThread( aThread.Id() );
    SetProcess( aThread.Process().Id() );
    //
    UpdateHeapInfoL( iLast );
    iCurrent = iLast;
    //
    SetHandle( iCurrent.AsRHeap().MetaData().ChunkHandle() );
    }


CMemSpyEngineHelperSysMemTrackerEntryHeap* CMemSpyEngineHelperSysMemTrackerEntryHeap::NewUserLC( CMemSpyEngineHelperSysMemTrackerImp& aTracker, CMemSpyThread& aThread )
    {
    CMemSpyEngineHelperSysMemTrackerEntryHeap* self = new(ELeave) CMemSpyEngineHelperSysMemTrackerEntryHeap( aTracker, EMemSpyEngineSysMemTrackerTypeHeapUser );
    CleanupStack::PushL( self );
    self->ConstructL( aThread );
    return self;
    }


CMemSpyEngineHelperSysMemTrackerEntryHeap* CMemSpyEngineHelperSysMemTrackerEntryHeap::NewKernelLC( CMemSpyEngineHelperSysMemTrackerImp& aTracker )
    {
    CMemSpyEngineHelperSysMemTrackerEntryHeap* self = new(ELeave) CMemSpyEngineHelperSysMemTrackerEntryHeap( aTracker, EMemSpyEngineSysMemTrackerTypeHeapKernel );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


TUint64 CMemSpyEngineHelperSysMemTrackerEntryHeap::Key() const
    {
    const TUint32 val = ( Type() << 28 ) + ThreadId(); 
    TUint64 ret = val;
    ret <<= 32;
    ret += Handle();
    return ret;
    }


void CMemSpyEngineHelperSysMemTrackerEntryHeap::UpdateFromL( const CMemSpyEngineHelperSysMemTrackerEntry& aEntry )
    {
    const CMemSpyEngineHelperSysMemTrackerEntryHeap& entry = static_cast< const CMemSpyEngineHelperSysMemTrackerEntryHeap& >( aEntry );
    
    // Update state
    iLast = iCurrent;
    iCurrent = entry.iCurrent;
    }


TBool CMemSpyEngineHelperSysMemTrackerEntryHeap::HasChangedL( const TMemSpyEngineHelperSysMemTrackerConfig& /*aConfig*/ ) const
    {
    TBool hasChanged = EFalse;

    // Work out if something has changed...
    if  ( HasHeapSizeChanged() )
        {
        hasChanged = ETrue;
        }
    else if ( HaveAllocCellsChanged() )
        {
        hasChanged = ETrue;
        }
    else if ( HaveFreeCellsChanged() )
        {
        hasChanged = ETrue;
        }

    return hasChanged;
    }


void CMemSpyEngineHelperSysMemTrackerEntryHeap::CreateChangeDescriptorL( CMemSpyEngineHelperSysMemTrackerCycle& aCycle )
    {
    CMemSpyEngineHelperSysMemTrackerCycleChangeHeap* changeDescriptor = CMemSpyEngineHelperSysMemTrackerCycleChangeHeap::NewLC( Attributes(), *iThreadName, iCurrent, IsNew() ? NULL : &iLast );
    aCycle.AddAndPopL( changeDescriptor );
    }


void CMemSpyEngineHelperSysMemTrackerEntryHeap::UpdateCycleStatistics( CMemSpyEngineHelperSysMemTrackerCycle& aCycle )
    {
    const TMemSpyHeapMetaDataRHeap& metaC = iCurrent.AsRHeap().MetaData();
    const TMemSpyHeapStatisticsRHeap& statsC = iCurrent.AsRHeap().Statistics();
    //
    aCycle.AddToMemoryUsed( metaC.ChunkSize() );
    aCycle.AddToCellCountAlloc( statsC.StatsAllocated().TypeCount() );
    aCycle.AddToMemoryHeapAllocs( statsC.StatsAllocated().TypeSize() );
    aCycle.AddToCellCountFree( statsC.StatsFree().TypeCount() );
    aCycle.AddToMemoryHeapFrees( statsC.StatsFree().TypeSize() );
    }


void CMemSpyEngineHelperSysMemTrackerEntryHeap::SetAsShared( TBool aShared )
    {
    iCurrent.AsRHeap().MetaData().SetSharedHeap( aShared );
    }


void CMemSpyEngineHelperSysMemTrackerEntryHeap::UpdateHeapInfoL( TMemSpyHeapInfo& aInfo )
    {
    TInt error = KErrNone;
    //
    if  ( Type() == EMemSpyEngineSysMemTrackerTypeHeapUser )
        {
        // Get the heap info first of all
        Engine().ProcessSuspendLC( ProcessId() );
        error = Engine().Driver().GetHeapInfoUser( aInfo, ThreadId() );
        CleanupStack::PopAndDestroy(); // ProcessSuspendLC
        }
    else if ( Type() == EMemSpyEngineSysMemTrackerTypeHeapKernel )
        {
        // Kernel
        error = Engine().Driver().GetHeapInfoKernel( aInfo );
        }
    else
        {
        User::Invariant();
        }
    //
    User::LeaveIfError( error );
    //
    if  ( aInfo.Type() == TMemSpyHeapInfo::ETypeUnknown )
        {
        User::Leave( KErrNotSupported );
        }
    }


TBool CMemSpyEngineHelperSysMemTrackerEntryHeap::HasHeapSizeChanged() const
    {
    return ( iCurrent.AsRHeap().MetaData().ChunkSize() != iLast.AsRHeap().MetaData().ChunkSize() );
    }


TBool CMemSpyEngineHelperSysMemTrackerEntryHeap::HaveFreeCellsChanged() const
    {
    TBool changed = ( iCurrent.AsRHeap().Statistics().StatsFree().TypeCount() != iLast.AsRHeap().Statistics().StatsFree().TypeCount() ) ||
		( iCurrent.AsRHeap().Statistics().StatsFree().TypeSize() != iLast.AsRHeap().Statistics().StatsFree().TypeSize() );
    //
    return changed;
    }


TBool CMemSpyEngineHelperSysMemTrackerEntryHeap::HaveAllocCellsChanged() const
    {
    return 
        ( iCurrent.AsRHeap().Statistics().StatsAllocated().TypeCount() != iLast.AsRHeap().Statistics().StatsAllocated().TypeCount() ) ||
        ( iCurrent.AsRHeap().Statistics().StatsAllocated().TypeSize() != iLast.AsRHeap().Statistics().StatsAllocated().TypeSize() );
    }







CMemSpyEngineHelperSysMemTrackerCycleChangeHeap::CMemSpyEngineHelperSysMemTrackerCycleChangeHeap( TUint8 aAttribs, const TMemSpyHeapInfo& aCurrent )
:   CMemSpyEngineHelperSysMemTrackerCycleChange( aAttribs ), iCurrent( aCurrent )
    {
    }


CMemSpyEngineHelperSysMemTrackerCycleChangeHeap::~CMemSpyEngineHelperSysMemTrackerCycleChangeHeap()
    {
    delete iLast;
    delete iThreadName;
    }


void CMemSpyEngineHelperSysMemTrackerCycleChangeHeap::ConstructL( const TDesC& aThreadName, const TMemSpyHeapInfo* aLast )
    {
    BaseConstructL();

    // Save the thread name
    iThreadName = aThreadName.AllocL();

    // Save last heap data (if available)
    if ( aLast )
        {
        iLast = new (ELeave) TMemSpyHeapInfo();
        *iLast = *aLast;
        }
    }


CMemSpyEngineHelperSysMemTrackerCycleChangeHeap* CMemSpyEngineHelperSysMemTrackerCycleChangeHeap::NewLC( TUint8 aAttribs, const TDesC& aThreadName, const TMemSpyHeapInfo& aCurrent, const TMemSpyHeapInfo* aLast )
    {
    CMemSpyEngineHelperSysMemTrackerCycleChangeHeap* self = new(ELeave) CMemSpyEngineHelperSysMemTrackerCycleChangeHeap( aAttribs, aCurrent );
    CleanupStack::PushL( self );
    self->ConstructL( aThreadName, aLast );
    return self;
    }


TMemSpyEngineSysMemTrackerType CMemSpyEngineHelperSysMemTrackerCycleChangeHeap::Type() const
    {
    TMemSpyEngineSysMemTrackerType ret = EMemSpyEngineSysMemTrackerTypeHeapUser;
    //
    if  ( IsKernel() )
        {
        ret = EMemSpyEngineSysMemTrackerTypeHeapKernel;
        }
    //
    return ret;
    }


void CMemSpyEngineHelperSysMemTrackerCycleChangeHeap::OutputHeaderL( CMemSpyEngineOutputSink& aSink, CMemSpyEngineHelperSysMemTrackerCycle& /*aCycle*/ )
    {
    _LIT( KHeaderHeap, "Type, Thread, Chunk, Handle, Heap Addr, Size, Min, Max, 1st Free Addr, 1st Free Len, Alloc Count, Alloc Space, Free Count, Free Space, Free Slack, F.Largest, A.Largest, Attribs");
    aSink.OutputLineL( KHeaderHeap );
    }


void CMemSpyEngineHelperSysMemTrackerCycleChangeHeap::OutputContentL( CMemSpyEngineOutputSink& aSink, CMemSpyEngineHelperSysMemTrackerCycle& /*aCycle*/ )
    {
    _LIT( KFormat, "%S,%S,%S,0x%08x,0x%08x,%d,%d,%d,0x%08x,%d,%d,%d,%d,%d,%d,%d,%d,%S" );
    //
    TMemSpySWMTTypeName type;;
    FormatType( type );
    //
    TBuf<20> attribs;
    FormatAttributes( attribs );
    //
    HBufC* buf = HBufC::NewLC( 1024 );
    TPtr pBuf(buf->Des());

    const TMemSpyHeapMetaDataRHeap& metaData = iCurrent.AsRHeap().MetaData();
    const TMemSpyHeapStatisticsRHeap& stats = iCurrent.AsRHeap().Statistics();

    // Strip any process & thread
    const TPtrC pChunkName( MemSpyEngineUtils::TextAfterLastDoubleColon( metaData.ChunkName() ) );

    pBuf.Format( KFormat, 
                 &type,
                 iThreadName, 
                 &pChunkName,
                 metaData.ChunkHandle(),
                 /*objectData.Base(),*/ metaData.iAllocatorAddress,
                 metaData.ChunkSize(),
                 /*objectData.iMinLength,*/ metaData.iMinHeapSize,
                 /*objectData.iMaxLength,*/ metaData.iMaxHeapSize,
                 /*objectData.iFree.next,*/ NULL, //TODO
                 /*objectData.iFree.len,*/ 0,
                 stats.StatsAllocated().TypeCount(),
                 stats.StatsAllocated().TypeSize(),
                 stats.StatsFree().TypeCount(),
                 stats.StatsFree().TypeSize(),
                 stats.StatsFree().SlackSpaceCellSize(),
                 stats.StatsFree().LargestCellSize(),
                 stats.StatsAllocated().LargestCellSize(),
                 &attribs
                 );

    aSink.OutputLineL( pBuf );
    CleanupStack::PopAndDestroy( buf );
    }


void CMemSpyEngineHelperSysMemTrackerCycleChangeHeap::OutputDataL( CMemSpyEngineOutputSink& aSink, CMemSpyEngineHelperSysMemTrackerCycle& aCycle )
    {
    CMemSpyEngine& engine = aSink.Engine();

    // First check that we can find the user-thread okay.
    CMemSpyThread* thread = NULL;
    CMemSpyProcess* process = NULL;
    TBool allowedToDump = ETrue;
    TMemSpyEngineHelperSysMemTrackerConfig config;
    engine.HelperSysMemTracker().GetConfig( config );
    if ( config.iThreadNameFilter.Length() > 0 && iThreadName->FindF( config.iThreadNameFilter ) == KErrNotFound )
        {
        allowedToDump = EFalse;
        }
    if  ( allowedToDump && !IsKernel() )
        {
        allowedToDump = ( engine.Container().ProcessAndThreadByThreadId( iCurrent.Tid(), process, thread ) == KErrNone );
        }

	// Now dump the heap data
    if  ( allowedToDump )
        {
        // Starts a data Stream
        aCycle.DataStreamBeginL( aSink, *iThreadName );
        
        TInt err = KErrNone;

        if  ( IsKernel() )
            {
            TRAP(err, engine.HelperHeap().OutputHeapDataKernelL( KMemSpyEngineSinkDoNotCreateOwnDataStream ));
            }
        else if ( thread )
            {
            TRAP(err, engine.HelperHeap().OutputHeapDataUserL( *thread, KMemSpyEngineSinkDoNotCreateOwnDataStream )); 
            }

        // End the stream (commit the file)
        aCycle.DataStreamEndL( aSink );
        
        User::LeaveIfError(err);
        }
    }


TBool CMemSpyEngineHelperSysMemTrackerCycleChangeHeap::IsKernel() const
    {
    return ( iCurrent.Pid() == 0 && iCurrent.Tid() == 0 );
    }


void CMemSpyEngineHelperSysMemTrackerCycleChangeHeap::FormatAttributes( TDes& aBuffer ) const
    {
    _LIT( KAttribShared, "[S]" ); // Attribute for Shared Heap
    _LIT( KAttribPlus,  "+" );
    CMemSpyEngineHelperSysMemTrackerCycleChange::FormatAttributes( aBuffer );
    if ( iCurrent.AsRHeap().MetaData().IsSharedHeap() )
        {
        if ( aBuffer.Length() )
            {
            aBuffer.Append( KAttribPlus );
            }
        aBuffer.Append( KAttribShared );
        }
    }


