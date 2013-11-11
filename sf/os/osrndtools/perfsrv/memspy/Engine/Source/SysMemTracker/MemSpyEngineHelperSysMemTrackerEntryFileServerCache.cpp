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

#include "MemSpyEngineHelperSysMemTrackerEntryFileServerCache.h"

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




CMemSpyEngineHelperSysMemTrackerEntryFileServerCache::CMemSpyEngineHelperSysMemTrackerEntryFileServerCache( CMemSpyEngineHelperSysMemTrackerImp& aTracker, const TMemSpyDriverChunkInfo& aInfo )
:   CMemSpyEngineHelperSysMemTrackerEntry( aTracker, EMemSpyEngineSysMemTrackerTypeFileServerCache ), iLast( aInfo ), iCurrent( aInfo )
    {
    }


CMemSpyEngineHelperSysMemTrackerEntryFileServerCache::~CMemSpyEngineHelperSysMemTrackerEntryFileServerCache()
    {
    }


void CMemSpyEngineHelperSysMemTrackerEntryFileServerCache::ConstructL()
    {
    SetHandle( iCurrent.iHandle );
    }


CMemSpyEngineHelperSysMemTrackerEntryFileServerCache* CMemSpyEngineHelperSysMemTrackerEntryFileServerCache::NewLC( CMemSpyEngineHelperSysMemTrackerImp& aTracker, const TMemSpyDriverChunkInfo& aInfo )
    {
    CMemSpyEngineHelperSysMemTrackerEntryFileServerCache* self = new(ELeave) CMemSpyEngineHelperSysMemTrackerEntryFileServerCache( aTracker, aInfo );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


void CMemSpyEngineHelperSysMemTrackerEntryFileServerCache::UpdateFromL( const CMemSpyEngineHelperSysMemTrackerEntry& aEntry )
    {
    const CMemSpyEngineHelperSysMemTrackerEntryFileServerCache& entry = static_cast< const CMemSpyEngineHelperSysMemTrackerEntryFileServerCache& >( aEntry );
    
    // Update state
    iLast = iCurrent;
    iCurrent = entry.iCurrent;
    }


TBool CMemSpyEngineHelperSysMemTrackerEntryFileServerCache::HasChangedL( const TMemSpyEngineHelperSysMemTrackerConfig& /*aConfig*/ ) const
    {
    const TBool hasChanged = ( iCurrent.iSize != iLast.iSize );
    return hasChanged;
    }


void CMemSpyEngineHelperSysMemTrackerEntryFileServerCache::CreateChangeDescriptorL( CMemSpyEngineHelperSysMemTrackerCycle& aCycle )
    {
    CMemSpyEngineHelperSysMemTrackerCycleChangeFileServerCache* changeDescriptor = CMemSpyEngineHelperSysMemTrackerCycleChangeFileServerCache::NewLC( Attributes(), iCurrent );
    aCycle.AddAndPopL( changeDescriptor );
    }


void CMemSpyEngineHelperSysMemTrackerEntryFileServerCache::UpdateCycleStatistics( CMemSpyEngineHelperSysMemTrackerCycle& aCycle )
    {
    aCycle.AddToMemoryUsed( iCurrent.iSize );
    }












































CMemSpyEngineHelperSysMemTrackerCycleChangeFileServerCache::CMemSpyEngineHelperSysMemTrackerCycleChangeFileServerCache( TUint8 aAttribs, const TMemSpyDriverChunkInfo& aCurrent )
:   CMemSpyEngineHelperSysMemTrackerCycleChange( aAttribs ), iCurrent( aCurrent )
    {
    }


CMemSpyEngineHelperSysMemTrackerCycleChangeFileServerCache::~CMemSpyEngineHelperSysMemTrackerCycleChangeFileServerCache()
    {
    }


void CMemSpyEngineHelperSysMemTrackerCycleChangeFileServerCache::ConstructL()
    {
    BaseConstructL();
    }


CMemSpyEngineHelperSysMemTrackerCycleChangeFileServerCache* CMemSpyEngineHelperSysMemTrackerCycleChangeFileServerCache::NewLC( TUint8 aAttribs, const TMemSpyDriverChunkInfo& aCurrent )
    {
    CMemSpyEngineHelperSysMemTrackerCycleChangeFileServerCache* self = new(ELeave) CMemSpyEngineHelperSysMemTrackerCycleChangeFileServerCache( aAttribs, aCurrent );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


TMemSpyEngineSysMemTrackerType CMemSpyEngineHelperSysMemTrackerCycleChangeFileServerCache::Type() const
    {
    return EMemSpyEngineSysMemTrackerTypeFileServerCache;
    }
   

void CMemSpyEngineHelperSysMemTrackerCycleChangeFileServerCache::OutputHeaderL( CMemSpyEngineOutputSink& aSink, CMemSpyEngineHelperSysMemTrackerCycle& /*aCycle*/ )
    {
    _LIT( KHeaderChunk, "Type, Name, Handle, Base Addr, Size, Max Size, Attribs");
    aSink.OutputLineL( KHeaderChunk );
    }
 

void CMemSpyEngineHelperSysMemTrackerCycleChangeFileServerCache::OutputContentL( CMemSpyEngineOutputSink& aSink, CMemSpyEngineHelperSysMemTrackerCycle& /*aCycle*/ )
    {
    _LIT( KFormat, "%S,%S,0x%08x,0x%08x,%d,%d,%S" );
    //
    TMemSpySWMTTypeName type;;
    FormatType( type );
    //
    TBuf<20> attribs;
    FormatAttributes( attribs );
    //
    HBufC* buf = HBufC::NewLC( 1024 );
    TPtr pBuf(buf->Des());

    // Now output line
    pBuf.Format( KFormat, 
                 &type,
                 &iCurrent.iName, 
                 iCurrent.iHandle,
                 iCurrent.iBaseAddress,
                 iCurrent.iSize,
                 iCurrent.iMaxSize,
                 &attribs
                 );

    aSink.OutputLineL( pBuf );
    CleanupStack::PopAndDestroy( buf );
    }


void CMemSpyEngineHelperSysMemTrackerCycleChangeFileServerCache::OutputDataL( CMemSpyEngineOutputSink& aSink, CMemSpyEngineHelperSysMemTrackerCycle& /*aCycle*/ )
    {
    CMemSpyEngine& engine = aSink.Engine();
    }

