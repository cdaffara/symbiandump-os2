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

#include "MemSpyEngineHelperSysMemTrackerEntryChunk.h"

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




CMemSpyEngineHelperSysMemTrackerEntryChunk::CMemSpyEngineHelperSysMemTrackerEntryChunk( CMemSpyEngineHelperSysMemTrackerImp& aTracker )
:   CMemSpyEngineHelperSysMemTrackerEntry( aTracker, EMemSpyEngineSysMemTrackerTypeChunk )
    {
    }


CMemSpyEngineHelperSysMemTrackerEntryChunk::~CMemSpyEngineHelperSysMemTrackerEntryChunk()
    {
    delete iChunkName;
    }


void CMemSpyEngineHelperSysMemTrackerEntryChunk::ConstructL( const TMemSpyDriverChunkInfo& aInfo, const TDesC& aFullChunkName )
    {
    iCurrent = aInfo;
    iLast = aInfo;
    //
    iChunkName = aFullChunkName.AllocL();
    SetHandle( aInfo.iHandle );
    }


void CMemSpyEngineHelperSysMemTrackerEntryChunk::ConstructL( const TMemSpyDriverChunkInfo& aInfo, CMemSpyProcess& aProcess )
    {
    ConstructL( aInfo, aInfo.iName );
    //
    SetProcess( aProcess.Id() );
    }


CMemSpyEngineHelperSysMemTrackerEntryChunk* CMemSpyEngineHelperSysMemTrackerEntryChunk::NewLC( CMemSpyEngineHelperSysMemTrackerImp& aTracker, const TMemSpyDriverChunkInfo& aInfo )
    {
    CMemSpyEngineHelperSysMemTrackerEntryChunk* self = new(ELeave) CMemSpyEngineHelperSysMemTrackerEntryChunk( aTracker );
    CleanupStack::PushL( self );
    self->ConstructL( aInfo, aInfo.iName );
    return self;
    }


CMemSpyEngineHelperSysMemTrackerEntryChunk* CMemSpyEngineHelperSysMemTrackerEntryChunk::NewLC( CMemSpyEngineHelperSysMemTrackerImp& aTracker, const TMemSpyDriverChunkInfo& aInfo, CMemSpyProcess& aProcess )
    {
    CMemSpyEngineHelperSysMemTrackerEntryChunk* self = new(ELeave) CMemSpyEngineHelperSysMemTrackerEntryChunk( aTracker );
    CleanupStack::PushL( self );
    self->ConstructL( aInfo, aProcess );
    return self;
    }


void CMemSpyEngineHelperSysMemTrackerEntryChunk::UpdateFromL( const CMemSpyEngineHelperSysMemTrackerEntry& aEntry )
    {
    const CMemSpyEngineHelperSysMemTrackerEntryChunk& entry = static_cast< const CMemSpyEngineHelperSysMemTrackerEntryChunk& >( aEntry );
    
    // Update state
    iLast = iCurrent;
    iCurrent = entry.iCurrent;
    }


TBool CMemSpyEngineHelperSysMemTrackerEntryChunk::HasChangedL( const TMemSpyEngineHelperSysMemTrackerConfig& /*aConfig*/ ) const
    {
    const TBool hasChanged = ( iCurrent.iSize != iLast.iSize );
    return hasChanged;
    }


void CMemSpyEngineHelperSysMemTrackerEntryChunk::CreateChangeDescriptorL( CMemSpyEngineHelperSysMemTrackerCycle& aCycle )
    {
    CMemSpyEngineHelperSysMemTrackerCycleChangeChunk* changeDescriptor = CMemSpyEngineHelperSysMemTrackerCycleChangeChunk::NewLC( Attributes(), *iChunkName, iCurrent, IsNew() ? NULL : &iLast );
    aCycle.AddAndPopL( changeDescriptor );
    }


void CMemSpyEngineHelperSysMemTrackerEntryChunk::UpdateCycleStatistics( CMemSpyEngineHelperSysMemTrackerCycle& aCycle )
    {
    aCycle.AddToMemoryUsed( iCurrent.iSize );
    }












































CMemSpyEngineHelperSysMemTrackerCycleChangeChunk::CMemSpyEngineHelperSysMemTrackerCycleChangeChunk( TUint8 aAttribs, const TMemSpyDriverChunkInfo& aCurrent )
:   CMemSpyEngineHelperSysMemTrackerCycleChange( aAttribs ), iCurrent( aCurrent )
    {
    }


CMemSpyEngineHelperSysMemTrackerCycleChangeChunk::~CMemSpyEngineHelperSysMemTrackerCycleChangeChunk()
    {
    delete iLast;
    delete iChunkName;
    }


void CMemSpyEngineHelperSysMemTrackerCycleChangeChunk::ConstructL( const TDesC& aChunkName, const TMemSpyDriverChunkInfo* aLast )
    {
    BaseConstructL();

    // Save the thread name
    iChunkName = aChunkName.AllocL();

    // Save last heap data (if available)
    if ( aLast )
        {
        iLast = new (ELeave) TMemSpyDriverChunkInfo();
        *iLast = *aLast;
        }
    }


CMemSpyEngineHelperSysMemTrackerCycleChangeChunk* CMemSpyEngineHelperSysMemTrackerCycleChangeChunk::NewLC( TUint8 aAttribs, const TDesC& aChunkName, const TMemSpyDriverChunkInfo& aCurrent, const TMemSpyDriverChunkInfo* aLast )
    {
    CMemSpyEngineHelperSysMemTrackerCycleChangeChunk* self = new(ELeave) CMemSpyEngineHelperSysMemTrackerCycleChangeChunk( aAttribs, aCurrent );
    CleanupStack::PushL( self );
    self->ConstructL( aChunkName, aLast );
    return self;
    }


TMemSpyEngineSysMemTrackerType CMemSpyEngineHelperSysMemTrackerCycleChangeChunk::Type() const
    {
    return EMemSpyEngineSysMemTrackerTypeChunk;
    }
   

void CMemSpyEngineHelperSysMemTrackerCycleChangeChunk::OutputHeaderL( CMemSpyEngineOutputSink& aSink, CMemSpyEngineHelperSysMemTrackerCycle& /*aCycle*/ )
    {
    _LIT( KHeaderChunk, "Type, Process, Chunk, Handle, Base Addr, Size, Max Size, Attribs");
    aSink.OutputLineL( KHeaderChunk );
    }
 

void CMemSpyEngineHelperSysMemTrackerCycleChangeChunk::OutputContentL( CMemSpyEngineOutputSink& aSink, CMemSpyEngineHelperSysMemTrackerCycle& /*aCycle*/ )
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
    TFullName processName( KNullDesC );
    CMemSpyEngine& engine = aSink.Engine();
    const TInt procIndex = engine.Container().ProcessIndexById( iCurrent.iOwnerId );
    if  ( procIndex >= 0 )
        {
        CMemSpyProcess& process = engine.Container().At( procIndex );
        process.FullName( processName );
        }

    // Now output line
    pBuf.Format( KFormat, 
                 &type,
                 &processName, 
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


void CMemSpyEngineHelperSysMemTrackerCycleChangeChunk::OutputDataL( CMemSpyEngineOutputSink& /*aSink*/, CMemSpyEngineHelperSysMemTrackerCycle& /*aCycle*/ )
    {
    }

