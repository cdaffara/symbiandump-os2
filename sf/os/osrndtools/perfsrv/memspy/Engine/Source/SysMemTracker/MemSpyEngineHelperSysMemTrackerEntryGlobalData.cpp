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

#include "MemSpyEngineHelperSysMemTrackerEntryGlobalData.h"

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
#include <memspy/engine/memspyenginehelperchunk.h>
#include <memspy/engine/memspyengineobjectthread.h>
#include <memspy/engine/memspyengineobjectprocess.h>
#include <memspy/engine/memspyengineobjectcontainer.h>
#include <memspy/engine/memspyenginehelpercodesegment.h>
#include "MemSpyEngineHelperSysMemTrackerImp.h"
#include <memspy/engine/memspyenginehelpersysmemtrackercycle.h>
#include <memspy/engine/memspyenginehelpersysmemtrackerconfig.h>




CMemSpyEngineHelperSysMemTrackerGlobalData::CMemSpyEngineHelperSysMemTrackerGlobalData( CMemSpyEngineHelperSysMemTrackerImp& aTracker )
:   CMemSpyEngineHelperSysMemTrackerEntry( aTracker, EMemSpyEngineSysMemTrackerTypeGlobalData )
    {
    }


CMemSpyEngineHelperSysMemTrackerGlobalData::~CMemSpyEngineHelperSysMemTrackerGlobalData()
    {
    delete iChunkName;
    delete iLast;
    }


void CMemSpyEngineHelperSysMemTrackerGlobalData::ConstructL( CMemSpyProcess& aProcess, const TMemSpyDriverChunkInfo& aChunkInfo )
    {
    iCurrent = aChunkInfo;
    //
    SetProcess( aProcess.Id() );
    SetHandle( aChunkInfo.iHandle );

    if  ( aChunkInfo.iType == EMemSpyDriverChunkTypeGlobalData )
        {
        // Pure global data, unique chunk
        iLast = new(ELeave) TMemSpyDriverChunkInfo();
        *iLast = aChunkInfo;
        }
    else if ( aChunkInfo.iType == EMemSpyDriverChunkTypeStackAndProcessGlobalData )
        {
        // Mostly stack, possibly some global data too. Get the global data size from
        // the code seg associated with the process.
        CMemSpyEngineCodeSegList* codeSegs = Engine().HelperCodeSegment().CodeSegmentListL( aProcess.Id() );
        CleanupStack::PushL( codeSegs );

        // We're looking for the process codesegment...
        TFileName* fileName = new(ELeave) TFileName();
        CleanupStack::PushL( fileName );
        aProcess.GetFileName( *fileName );
        const TInt pos = codeSegs->IndexByName( *fileName );
        CleanupStack::PopAndDestroy( fileName );

        // Get total data size
        if  ( pos >= 0 )
            {
            const CMemSpyEngineCodeSegEntry& entry = codeSegs->At( pos );
            const TCodeSegCreateInfo& createInfo = entry.CreateInfo();
            iCurrent.iSize = Engine().Driver().RoundToPageSize( createInfo.iTotalDataSize );
            
            // If there is no data, then we don't want an entry in the list, so bail out.
            if  ( iCurrent.iSize == 0 )
                {
                User::Leave( KErrNotSupported );
                }
            }

        CleanupStack::PopAndDestroy( codeSegs );
        }

    iChunkName = aChunkInfo.iName.AllocL();
    }


CMemSpyEngineHelperSysMemTrackerGlobalData* CMemSpyEngineHelperSysMemTrackerGlobalData::NewLC( CMemSpyEngineHelperSysMemTrackerImp& aTracker, CMemSpyProcess& aProcess, const TMemSpyDriverChunkInfo& aChunkInfo )
    {
    CMemSpyEngineHelperSysMemTrackerGlobalData* self = new(ELeave) CMemSpyEngineHelperSysMemTrackerGlobalData( aTracker );
    CleanupStack::PushL( self );
    self->ConstructL( aProcess, aChunkInfo );
    return self;
    }


void CMemSpyEngineHelperSysMemTrackerGlobalData::UpdateFromL( const CMemSpyEngineHelperSysMemTrackerEntry& aEntry )
    {
    const CMemSpyEngineHelperSysMemTrackerGlobalData& entry = static_cast< const CMemSpyEngineHelperSysMemTrackerGlobalData& >( aEntry );
    __ASSERT_ALWAYS( entry.IsProcessGlobalData() == IsProcessGlobalData(), User::Panic(_L("MemSpySysMemT"), __LINE__ ) );
   
    if  ( !IsProcessGlobalData() )
        {
        // Update state
        *iLast = iCurrent;
        iCurrent = entry.iCurrent;
        }
    }


TBool CMemSpyEngineHelperSysMemTrackerGlobalData::HasChangedL( const TMemSpyEngineHelperSysMemTrackerConfig& /*aConfig*/ ) const
    {
    TBool hasChanged = EFalse;
    //
    if  ( !IsProcessGlobalData() )
        {
        hasChanged = ( iCurrent.iSize != iLast->iSize );
        }
    //
    return hasChanged;
    }


void CMemSpyEngineHelperSysMemTrackerGlobalData::CreateChangeDescriptorL( CMemSpyEngineHelperSysMemTrackerCycle& aCycle )
    {
    CMemSpyEngineHelperSysMemTrackerCycleChangeGlobalData* changeDescriptor = CMemSpyEngineHelperSysMemTrackerCycleChangeGlobalData::NewLC( Attributes(), *iChunkName, iCurrent, IsNew() ? NULL : iLast );
    aCycle.AddAndPopL( changeDescriptor );
    }


void CMemSpyEngineHelperSysMemTrackerGlobalData::UpdateCycleStatistics( CMemSpyEngineHelperSysMemTrackerCycle& aCycle )
    {
    const TInt size = CurrentGlobalDataSize();
    aCycle.AddToMemoryUsed( size );
    }


TBool CMemSpyEngineHelperSysMemTrackerGlobalData::IsProcessGlobalData() const
    {
    return ( iLast == NULL );
    }


TInt CMemSpyEngineHelperSysMemTrackerGlobalData::CurrentGlobalDataSize() const
    {
    const TInt size = iCurrent.iSize;
    return size;
    }
























CMemSpyEngineHelperSysMemTrackerCycleChangeGlobalData::CMemSpyEngineHelperSysMemTrackerCycleChangeGlobalData( TUint8 aAttribs, const TMemSpyDriverChunkInfoWithoutName& aCurrent )
:   CMemSpyEngineHelperSysMemTrackerCycleChange( aAttribs ), iCurrent( aCurrent )
    {
    }


CMemSpyEngineHelperSysMemTrackerCycleChangeGlobalData::~CMemSpyEngineHelperSysMemTrackerCycleChangeGlobalData()
    {
    delete iLast;
    delete iChunkName;
    }


void CMemSpyEngineHelperSysMemTrackerCycleChangeGlobalData::ConstructL( const TDesC& aChunkName, const TMemSpyDriverChunkInfoWithoutName* aLast )
    {
    BaseConstructL();

    // Save the thread name
    iChunkName = aChunkName.AllocL();

    // Save last heap data (if available)
    if  ( aLast )
        {
        iLast = new (ELeave) TMemSpyDriverChunkInfoWithoutName();
        *iLast = *aLast;
        }
    }


CMemSpyEngineHelperSysMemTrackerCycleChangeGlobalData* CMemSpyEngineHelperSysMemTrackerCycleChangeGlobalData::NewLC( TUint8 aAttribs, const TDesC& aChunkName, const TMemSpyDriverChunkInfoWithoutName& aCurrent, const TMemSpyDriverChunkInfoWithoutName* aLast )
    {
    CMemSpyEngineHelperSysMemTrackerCycleChangeGlobalData* self = new(ELeave) CMemSpyEngineHelperSysMemTrackerCycleChangeGlobalData( aAttribs, aCurrent );
    CleanupStack::PushL( self );
    self->ConstructL( aChunkName, aLast );
    return self;
    }


TMemSpyEngineSysMemTrackerType CMemSpyEngineHelperSysMemTrackerCycleChangeGlobalData::Type() const
    {
    return EMemSpyEngineSysMemTrackerTypeGlobalData;
    }
   

void CMemSpyEngineHelperSysMemTrackerCycleChangeGlobalData::OutputHeaderL( CMemSpyEngineOutputSink& aSink, CMemSpyEngineHelperSysMemTrackerCycle& /*aCycle*/ )
    {
    _LIT( KHeaderGlobalData, "Type, Process, Chunk, Handle, Base Addr, Size, Max Size, Attribs");
    aSink.OutputLineL( KHeaderGlobalData );
    }
 

void CMemSpyEngineHelperSysMemTrackerCycleChangeGlobalData::OutputContentL( CMemSpyEngineOutputSink& aSink, CMemSpyEngineHelperSysMemTrackerCycle& /*aCycle*/ )
    {
    _LIT( KFormat, "%S,%S,%S,0x%08x,0x%08x,%d,%d,%S" );
    //
    TMemSpySWMTTypeName type;;
    FormatType( type );
    //
    TBuf<20> attribs;
    FormatAttributes( attribs );
    //
    HBufC* buf = HBufC::NewLC( 1024 );
    TPtr pBuf(buf->Des());

    // Try to get associated process name
    TPtrC pProcessName( KNullDesC );
    CMemSpyEngine& engine = aSink.Engine();
    const TInt procIndex = engine.Container().ProcessIndexById( iCurrent.iOwnerId );
    if  ( procIndex >= 0 )
        {
        CMemSpyProcess& process = engine.Container().At( procIndex );
        pProcessName.Set( process.Name() );
        }

    // Now output line
    pBuf.Format( KFormat, 
                 &type,
                 &pProcessName, 
                 iChunkName,
                 iCurrent.iHandle,
                 iCurrent.iBaseAddress,
                 iCurrent.iSize,
                 iCurrent.iMaxSize,
                 &attribs
                 );

    aSink.OutputLineL( pBuf );
    CleanupStack::PopAndDestroy( buf );
    }


void CMemSpyEngineHelperSysMemTrackerCycleChangeGlobalData::OutputDataL( CMemSpyEngineOutputSink& aSink, CMemSpyEngineHelperSysMemTrackerCycle& /*aCycle*/ )
    {
    CMemSpyEngine& engine = aSink.Engine();
    }

