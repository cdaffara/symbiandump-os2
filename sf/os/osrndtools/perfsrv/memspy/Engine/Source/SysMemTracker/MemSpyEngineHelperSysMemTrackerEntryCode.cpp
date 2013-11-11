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

#include "MemSpyEngineHelperSysMemTrackerEntryCode.h"

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




CMemSpyEngineHelperSysMemTrackerEntryCode::CMemSpyEngineHelperSysMemTrackerEntryCode( CMemSpyEngineHelperSysMemTrackerImp& aTracker )
:   CMemSpyEngineHelperSysMemTrackerEntry( aTracker, EMemSpyEngineSysMemTrackerTypeCode )
    {
    }


CMemSpyEngineHelperSysMemTrackerEntryCode::~CMemSpyEngineHelperSysMemTrackerEntryCode()
    {
    delete iCodeSegName;
    }


void CMemSpyEngineHelperSysMemTrackerEntryCode::ConstructL( const CMemSpyEngineCodeSegEntry& aInfo )
    {
    SetHandle( aInfo.Handle() );
    iSize = Engine().Driver().RoundToPageSize( aInfo.Size() );
    iCodeSegName = HBufC::NewL( aInfo.CreateInfo().iFileName.Length() );
    iCodeSegName->Des().Copy( aInfo.CreateInfo().iFileName );
    }


CMemSpyEngineHelperSysMemTrackerEntryCode* CMemSpyEngineHelperSysMemTrackerEntryCode::NewLC( CMemSpyEngineHelperSysMemTrackerImp& aTracker, const CMemSpyEngineCodeSegEntry& aInfo )
    {
    CMemSpyEngineHelperSysMemTrackerEntryCode* self = new(ELeave) CMemSpyEngineHelperSysMemTrackerEntryCode( aTracker );
    CleanupStack::PushL( self );
    self->ConstructL( aInfo );
    return self;
    }


void CMemSpyEngineHelperSysMemTrackerEntryCode::CreateChangeDescriptorL( CMemSpyEngineHelperSysMemTrackerCycle& aCycle )
    {
    CMemSpyEngineHelperSysMemTrackerCycleChangeCode* changeDescriptor = CMemSpyEngineHelperSysMemTrackerCycleChangeCode::NewLC( Attributes(), *iCodeSegName, iSize, Handle() );
    aCycle.AddAndPopL( changeDescriptor );
    }


void CMemSpyEngineHelperSysMemTrackerEntryCode::UpdateCycleStatistics( CMemSpyEngineHelperSysMemTrackerCycle& aCycle )
    {
    aCycle.AddToMemoryUsed( iSize );
    }





















CMemSpyEngineHelperSysMemTrackerCycleChangeCode::CMemSpyEngineHelperSysMemTrackerCycleChangeCode( TUint8 aAttribs, TUint32 aSize, TUint32 aHandle )
:   CMemSpyEngineHelperSysMemTrackerCycleChange( aAttribs ), iSize( aSize ), iHandle( aHandle )
    {
    }


CMemSpyEngineHelperSysMemTrackerCycleChangeCode::~CMemSpyEngineHelperSysMemTrackerCycleChangeCode()
    {
    delete iCodeSegName;
    }


void CMemSpyEngineHelperSysMemTrackerCycleChangeCode::ConstructL( const TDesC& aCodeSegName )
    {
    BaseConstructL();

    // Save the thread name
    iCodeSegName = aCodeSegName.AllocL();
    }


CMemSpyEngineHelperSysMemTrackerCycleChangeCode* CMemSpyEngineHelperSysMemTrackerCycleChangeCode::NewLC( TUint8 aAttribs, const TDesC& aCodeSegName, TUint32 aSize, TUint32 aHandle )
    {
    CMemSpyEngineHelperSysMemTrackerCycleChangeCode* self = new(ELeave) CMemSpyEngineHelperSysMemTrackerCycleChangeCode( aAttribs, aSize, aHandle );
    CleanupStack::PushL( self );
    self->ConstructL( aCodeSegName );
    return self;
    }


TMemSpyEngineSysMemTrackerType CMemSpyEngineHelperSysMemTrackerCycleChangeCode::Type() const
    {
    return EMemSpyEngineSysMemTrackerTypeCode;
    }
   

void CMemSpyEngineHelperSysMemTrackerCycleChangeCode::OutputHeaderL( CMemSpyEngineOutputSink& aSink, CMemSpyEngineHelperSysMemTrackerCycle& /*aCycle*/ )
    {
    _LIT( KHeaderCode, "Type, Name, Handle, Size, Attribs");
    aSink.OutputLineL( KHeaderCode );
    }
 

void CMemSpyEngineHelperSysMemTrackerCycleChangeCode::OutputContentL( CMemSpyEngineOutputSink& aSink, CMemSpyEngineHelperSysMemTrackerCycle& /*aCycle*/ )
    {
    _LIT( KFormat, "%S,%S,0x%08x,%d,%S" );
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
                 iCodeSegName, 
                 iHandle,
                 iSize,
                 &attribs
                 );

    aSink.OutputLineL( pBuf );
    CleanupStack::PopAndDestroy( buf );
    }
