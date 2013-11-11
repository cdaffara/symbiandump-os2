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

#include "MemSpyEngineHelperSysMemTrackerEntrySystemMemory.h"

// System includes
#include <e32base.h>
#include <badesca.h>
#include <hal.h>
#include <hal_data.h>

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





CMemSpyEngineHelperSysMemTrackerEntrySystemMemory::CMemSpyEngineHelperSysMemTrackerEntrySystemMemory( CMemSpyEngineHelperSysMemTrackerImp& aTracker, TSystemMemoryType aType )
:   CMemSpyEngineHelperSysMemTrackerEntry( aTracker, EMemSpyEngineSysMemTrackerTypeSystemMemory ), iType( aType )
    {
    }


CMemSpyEngineHelperSysMemTrackerEntrySystemMemory::~CMemSpyEngineHelperSysMemTrackerEntrySystemMemory()
    {
    }


void CMemSpyEngineHelperSysMemTrackerEntrySystemMemory::ConstructL()
    {
    if ( iType == ETypeFree )
        {
        HAL::Get( HALData::EMemoryRAMFree, iCurrent );
        }
    else if ( iType == ETypeTotal )
        {
        HAL::Get( HALData::EMemoryRAM, iCurrent );
        }
    //
    iLast = iCurrent;
    }


CMemSpyEngineHelperSysMemTrackerEntrySystemMemory* CMemSpyEngineHelperSysMemTrackerEntrySystemMemory::NewLC( CMemSpyEngineHelperSysMemTrackerImp& aTracker, TSystemMemoryType aType )
    {
    CMemSpyEngineHelperSysMemTrackerEntrySystemMemory* self = new(ELeave) CMemSpyEngineHelperSysMemTrackerEntrySystemMemory( aTracker, aType );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


TUint64 CMemSpyEngineHelperSysMemTrackerEntrySystemMemory::Key() const
    {
    const TUint32 val = ( Type() << 28 ); 
    TUint64 ret = val;
    ret <<= 32;
    ret += static_cast<TInt>( iType );
    return ret;
    }


void CMemSpyEngineHelperSysMemTrackerEntrySystemMemory::UpdateFromL( const CMemSpyEngineHelperSysMemTrackerEntry& aEntry )
    {
    const CMemSpyEngineHelperSysMemTrackerEntrySystemMemory& entry = static_cast< const CMemSpyEngineHelperSysMemTrackerEntrySystemMemory& >( aEntry );
    
    // Update state
    iLast = iCurrent;
    iCurrent = entry.iCurrent;
    }


void CMemSpyEngineHelperSysMemTrackerEntrySystemMemory::CreateChangeDescriptorL( CMemSpyEngineHelperSysMemTrackerCycle& aCycle )
    {
    CMemSpyEngineHelperSysMemTrackerCycleChangeSystemMemory* changeDescriptor = CMemSpyEngineHelperSysMemTrackerCycleChangeSystemMemory::NewLC( Attributes(), iType, iCurrent, iLast );
    aCycle.AddAndPopL( changeDescriptor );
    }


TBool CMemSpyEngineHelperSysMemTrackerEntrySystemMemory::HasChangedL( const TMemSpyEngineHelperSysMemTrackerConfig& /*aConfig*/ ) const
    {
    const TBool hasChanged = ( iCurrent != iLast );
    return hasChanged;
    }































CMemSpyEngineHelperSysMemTrackerCycleChangeSystemMemory::CMemSpyEngineHelperSysMemTrackerCycleChangeSystemMemory( TUint8 aAttribs, TSystemMemoryType aType, TInt aCurrent, TInt aLast )
:   CMemSpyEngineHelperSysMemTrackerCycleChange( aAttribs ), iType( aType ), iCurrent( aCurrent ), iLast( aLast )
    {
    }


CMemSpyEngineHelperSysMemTrackerCycleChangeSystemMemory::~CMemSpyEngineHelperSysMemTrackerCycleChangeSystemMemory()
    {
    }


void CMemSpyEngineHelperSysMemTrackerCycleChangeSystemMemory::ConstructL()
    {
    BaseConstructL();
    }


CMemSpyEngineHelperSysMemTrackerCycleChangeSystemMemory* CMemSpyEngineHelperSysMemTrackerCycleChangeSystemMemory::NewLC( TUint8 aAttribs, TSystemMemoryType aType, TInt aCurrent, TInt aLast )
    {
    CMemSpyEngineHelperSysMemTrackerCycleChangeSystemMemory* self = new(ELeave) CMemSpyEngineHelperSysMemTrackerCycleChangeSystemMemory( aAttribs, aType, aCurrent, aLast );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


TMemSpyEngineSysMemTrackerType CMemSpyEngineHelperSysMemTrackerCycleChangeSystemMemory::Type() const
    {
    return EMemSpyEngineSysMemTrackerTypeSystemMemory;
    }
   

void CMemSpyEngineHelperSysMemTrackerCycleChangeSystemMemory::OutputHeaderL( CMemSpyEngineOutputSink& aSink, CMemSpyEngineHelperSysMemTrackerCycle& /*aCycle*/ )
    {
    _LIT( KHeaderSystemMemory, "Type, Name, Size, Attribs");
    aSink.OutputLineL( KHeaderSystemMemory );
    }
 

void CMemSpyEngineHelperSysMemTrackerCycleChangeSystemMemory::OutputContentL( CMemSpyEngineOutputSink& aSink, CMemSpyEngineHelperSysMemTrackerCycle& /*aCycle*/ )
    {
    _LIT( KFormat, "%S,%S,%d,%S" );
    //
    TMemSpySWMTTypeName category;
    FormatType( category );
    //
    TBuf<20> attribs;
    FormatAttributes( attribs );
    //
    _LIT( KTypeSystemMemoryFree, "Free" );
    _LIT( KTypeSystemMemoryTotal, "Total" );
    TBuf<20> type( KTypeSystemMemoryFree );
    if ( iType == ETypeTotal )
        {
        type = KTypeSystemMemoryTotal;
        }
    //
    aSink.OutputLineFormattedL( KFormat, &category, &type, iCurrent, &attribs );
    }


