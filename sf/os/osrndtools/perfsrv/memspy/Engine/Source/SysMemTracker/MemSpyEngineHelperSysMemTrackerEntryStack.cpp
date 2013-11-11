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

#include "MemSpyEngineHelperSysMemTrackerEntryStack.h"

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


CMemSpyEngineHelperSysMemTrackerEntryStack::CMemSpyEngineHelperSysMemTrackerEntryStack( CMemSpyEngineHelperSysMemTrackerImp& aTracker, const TMemSpyDriverChunkInfo& aChunkInfo, const TThreadStackInfo& aStackInfo )
:   CMemSpyEngineHelperSysMemTrackerEntry( aTracker, EMemSpyEngineSysMemTrackerTypeStack ), iChunkInfo( aChunkInfo ), iStackInfo( aStackInfo )
    {
    SetHandle( aChunkInfo.iHandle );
    }


CMemSpyEngineHelperSysMemTrackerEntryStack::~CMemSpyEngineHelperSysMemTrackerEntryStack()
    {
    delete iThreadName;
    }


void CMemSpyEngineHelperSysMemTrackerEntryStack::ConstructL( CMemSpyThread& aThread )
    {
    TFullName* name = new(ELeave) TFullName();
    CleanupStack::PushL( name );
    aThread.FullName( *name );
    iThreadName = name->AllocL();
    CleanupStack::PopAndDestroy( name );
    //
    SetThread( aThread.Id() );
    SetProcess( aThread.Process().Id() );
    }


CMemSpyEngineHelperSysMemTrackerEntryStack* CMemSpyEngineHelperSysMemTrackerEntryStack::NewLC( CMemSpyEngineHelperSysMemTrackerImp& aTracker, CMemSpyThread& aThread, const TMemSpyDriverChunkInfo& aChunkInfo, const TThreadStackInfo& aStackInfo )
    {
    CMemSpyEngineHelperSysMemTrackerEntryStack* self = new(ELeave) CMemSpyEngineHelperSysMemTrackerEntryStack( aTracker, aChunkInfo, aStackInfo );
    CleanupStack::PushL( self );
    self->ConstructL( aThread );
    return self;
    }


TUint64 CMemSpyEngineHelperSysMemTrackerEntryStack::Key() const
    {
    const TUint32 val = ( Type() << 28 ) + ThreadId(); 
    TUint64 ret = val;
    ret <<= 32;
    ret += Handle();
    return ret;
    }


void CMemSpyEngineHelperSysMemTrackerEntryStack::CreateChangeDescriptorL( CMemSpyEngineHelperSysMemTrackerCycle& aCycle )
    {
    CMemSpyEngineHelperSysMemTrackerCycleChangeStack* changeDescriptor = CMemSpyEngineHelperSysMemTrackerCycleChangeStack::NewLC( Attributes(), *iThreadName, iChunkInfo, iStackInfo );
    aCycle.AddAndPopL( changeDescriptor );
    }


void CMemSpyEngineHelperSysMemTrackerEntryStack::UpdateCycleStatistics( CMemSpyEngineHelperSysMemTrackerCycle& aCycle )
    {
    const TInt size = StackSize();
    aCycle.AddToMemoryUsed( size );
    }


TInt CMemSpyEngineHelperSysMemTrackerEntryStack::StackSize() const
    {
    const TInt size = iStackInfo.iBase - iStackInfo.iLimit;
    return size;
    }








































CMemSpyEngineHelperSysMemTrackerCycleChangeStack::CMemSpyEngineHelperSysMemTrackerCycleChangeStack( TUint8 aAttribs, const TMemSpyDriverChunkInfo& aChunkInfo, const TThreadStackInfo& aStackInfo )
:   CMemSpyEngineHelperSysMemTrackerCycleChange( aAttribs ), iChunkInfo( aChunkInfo ), iStackInfo( aStackInfo )
    {
    }


CMemSpyEngineHelperSysMemTrackerCycleChangeStack::~CMemSpyEngineHelperSysMemTrackerCycleChangeStack()
    {
    delete iThreadName;
    }


void CMemSpyEngineHelperSysMemTrackerCycleChangeStack::ConstructL( const TDesC& aThreadName )
    {
    BaseConstructL();

    // Save the thread name
    iThreadName = aThreadName.AllocL();
    }


CMemSpyEngineHelperSysMemTrackerCycleChangeStack* CMemSpyEngineHelperSysMemTrackerCycleChangeStack::NewLC( TUint8 aAttribs, const TDesC& aThreadName, const TMemSpyDriverChunkInfo& aChunkInfo, const TThreadStackInfo& aStackInfo )
    {
    CMemSpyEngineHelperSysMemTrackerCycleChangeStack* self = new(ELeave) CMemSpyEngineHelperSysMemTrackerCycleChangeStack( aAttribs, aChunkInfo, aStackInfo );
    CleanupStack::PushL( self );
    self->ConstructL( aThreadName );
    return self;
    }


TMemSpyEngineSysMemTrackerType CMemSpyEngineHelperSysMemTrackerCycleChangeStack::Type() const
    {
    return EMemSpyEngineSysMemTrackerTypeStack;
    }


void CMemSpyEngineHelperSysMemTrackerCycleChangeStack::OutputHeaderL( CMemSpyEngineOutputSink& aSink, CMemSpyEngineHelperSysMemTrackerCycle& /*aCycle*/ )
    {
    _LIT( KHeaderStack, "Type, Thread, Chunk, Handle, Size, Attribs");
    aSink.OutputLineL( KHeaderStack );
    }


void CMemSpyEngineHelperSysMemTrackerCycleChangeStack::OutputContentL( CMemSpyEngineOutputSink& aSink, CMemSpyEngineHelperSysMemTrackerCycle& /*aCycle*/ )
    {
    _LIT( KFormat, "%S,%S,%S,0x%08x,%d,%S" );
    //
    TMemSpySWMTTypeName type;;
    FormatType( type );
    //
    TBuf<20> attribs;
    FormatAttributes( attribs );
    //
    HBufC* buf = HBufC::NewLC( 1024 );
    TPtr pBuf(buf->Des());

    pBuf.Format( KFormat, 
                 &type,
                 iThreadName, 
                 &iChunkInfo.iName,
                 iChunkInfo.iHandle,
                 StackSize(),
                 &attribs
                 );

    aSink.OutputLineL( pBuf );
    CleanupStack::PopAndDestroy( buf );
    }


void CMemSpyEngineHelperSysMemTrackerCycleChangeStack::OutputDataL( CMemSpyEngineOutputSink& aSink, CMemSpyEngineHelperSysMemTrackerCycle& /*aCycle*/ )
    {
    CMemSpyEngine& engine = aSink.Engine();
    }


TInt CMemSpyEngineHelperSysMemTrackerCycleChangeStack::StackSize() const
    {
    const TInt size = iStackInfo.iBase - iStackInfo.iLimit;
    return size;
    }


