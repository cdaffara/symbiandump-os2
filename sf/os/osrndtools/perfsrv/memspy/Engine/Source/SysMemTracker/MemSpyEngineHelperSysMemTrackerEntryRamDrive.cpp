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

#include "MemSpyEngineHelperSysMemTrackerEntryRamDrive.h"

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





CMemSpyEngineHelperSysMemTrackerEntryRamDrive::CMemSpyEngineHelperSysMemTrackerEntryRamDrive( CMemSpyEngineHelperSysMemTrackerImp& aTracker )
:   CMemSpyEngineHelperSysMemTrackerEntry( aTracker, EMemSpyEngineSysMemTrackerTypeRamDrive )
    {
    }


CMemSpyEngineHelperSysMemTrackerEntryRamDrive::~CMemSpyEngineHelperSysMemTrackerEntryRamDrive()
    {
    }


void CMemSpyEngineHelperSysMemTrackerEntryRamDrive::ConstructL( const TMemSpyDriverChunkInfo& aInfo )
    {
    iCurrent = aInfo;
    iLast = aInfo;
    //
    SetHandle( aInfo.iHandle );
    }


CMemSpyEngineHelperSysMemTrackerEntryRamDrive* CMemSpyEngineHelperSysMemTrackerEntryRamDrive::NewLC( CMemSpyEngineHelperSysMemTrackerImp& aTracker, const TMemSpyDriverChunkInfo& aInfo )
    {
    CMemSpyEngineHelperSysMemTrackerEntryRamDrive* self = new(ELeave) CMemSpyEngineHelperSysMemTrackerEntryRamDrive( aTracker );
    CleanupStack::PushL( self );
    self->ConstructL( aInfo );
    return self;
    }


void CMemSpyEngineHelperSysMemTrackerEntryRamDrive::UpdateFromL( const CMemSpyEngineHelperSysMemTrackerEntry& aEntry )
    {
    const CMemSpyEngineHelperSysMemTrackerEntryRamDrive& entry = static_cast< const CMemSpyEngineHelperSysMemTrackerEntryRamDrive& >( aEntry );
    
    // Update state
    iLast = iCurrent;
    iCurrent = entry.iCurrent;
    }


TBool CMemSpyEngineHelperSysMemTrackerEntryRamDrive::HasChangedL( const TMemSpyEngineHelperSysMemTrackerConfig& /*aConfig*/ ) const
    {
    const TBool hasChanged = ( iCurrent.iSize != iLast.iSize );
    return hasChanged;
    }


void CMemSpyEngineHelperSysMemTrackerEntryRamDrive::CreateChangeDescriptorL( CMemSpyEngineHelperSysMemTrackerCycle& aCycle )
    {
    CMemSpyEngineHelperSysMemTrackerCycleChangeRamDrive* changeDescriptor = CMemSpyEngineHelperSysMemTrackerCycleChangeRamDrive::NewLC( Attributes(), iCurrent, IsNew() ? NULL : &iLast );
    aCycle.AddAndPopL( changeDescriptor );
    }


void CMemSpyEngineHelperSysMemTrackerEntryRamDrive::UpdateCycleStatistics( CMemSpyEngineHelperSysMemTrackerCycle& aCycle )
    {
    aCycle.AddToMemoryUsed( iCurrent.iSize );
    }


































CMemSpyEngineHelperSysMemTrackerCycleChangeRamDrive::CMemSpyEngineHelperSysMemTrackerCycleChangeRamDrive( TUint8 aAttribs, const TMemSpyDriverChunkInfo& aCurrent )
:   CMemSpyEngineHelperSysMemTrackerCycleChange( aAttribs ), iCurrent( aCurrent )
    {
    }


CMemSpyEngineHelperSysMemTrackerCycleChangeRamDrive::~CMemSpyEngineHelperSysMemTrackerCycleChangeRamDrive()
    {
    delete iLast;
    }


void CMemSpyEngineHelperSysMemTrackerCycleChangeRamDrive::ConstructL( const TMemSpyDriverChunkInfo* aLast )
    {
    BaseConstructL();

    // Save last heap data (if available)
    if ( aLast )
        {
        iLast = new (ELeave) TMemSpyDriverChunkInfo();
        *iLast = *aLast;
        }
    }


CMemSpyEngineHelperSysMemTrackerCycleChangeRamDrive* CMemSpyEngineHelperSysMemTrackerCycleChangeRamDrive::NewLC( TUint8 aAttribs, const TMemSpyDriverChunkInfo& aCurrent, const TMemSpyDriverChunkInfo* aLast )
    {
    CMemSpyEngineHelperSysMemTrackerCycleChangeRamDrive* self = new(ELeave) CMemSpyEngineHelperSysMemTrackerCycleChangeRamDrive( aAttribs, aCurrent );
    CleanupStack::PushL( self );
    self->ConstructL( aLast );
    return self;
    }


TMemSpyEngineSysMemTrackerType CMemSpyEngineHelperSysMemTrackerCycleChangeRamDrive::Type() const
    {
    return EMemSpyEngineSysMemTrackerTypeRamDrive;
    }
   

void CMemSpyEngineHelperSysMemTrackerCycleChangeRamDrive::OutputHeaderL( CMemSpyEngineOutputSink& aSink, CMemSpyEngineHelperSysMemTrackerCycle& /*aCycle*/ )
    {
    _LIT( KHeaderRamDrive, "Type, Chunk, Handle, Base Addr, Size, Max Size, Attribs");
    aSink.OutputLineL( KHeaderRamDrive );
    }
 

void CMemSpyEngineHelperSysMemTrackerCycleChangeRamDrive::OutputContentL( CMemSpyEngineOutputSink& aSink, CMemSpyEngineHelperSysMemTrackerCycle& /*aCycle*/ )
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


