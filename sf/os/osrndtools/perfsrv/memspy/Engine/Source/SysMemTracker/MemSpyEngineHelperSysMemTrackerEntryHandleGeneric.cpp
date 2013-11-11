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

#include "MemSpyEngineHelperSysMemTrackerEntryHandleGeneric.h"

// System includes
#include <e32base.h>
#include <badesca.h>

// Driver includes
#include <memspy/driver/memspydriverclient.h>
#include <memspy/driver/memspydriverconstants.h>
#include <memspy/driver/memspydriverenumerationsshared.h>

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
#include <memspy/engine/memspyenginehelperkernelcontainers.h>
#include <memspy/engine/memspyenginehelpersysmemtrackercycle.h>
#include <memspy/engine/memspyenginehelpersysmemtrackerconfig.h>




CMemSpyEngineHelperSysMemTrackerEntryHandleGeneric::CMemSpyEngineHelperSysMemTrackerEntryHandleGeneric( CMemSpyEngineHelperSysMemTrackerImp& aTracker, TMemSpyDriverContainerType aType )
:   CMemSpyEngineHelperSysMemTrackerEntry( aTracker, EMemSpyEngineSysMemTrackerTypeHandleGeneric ), iHandleType( aType )
    {
    }


CMemSpyEngineHelperSysMemTrackerEntryHandleGeneric::~CMemSpyEngineHelperSysMemTrackerEntryHandleGeneric()
    {
    delete iName;
    }


void CMemSpyEngineHelperSysMemTrackerEntryHandleGeneric::ConstructL( TAny* aHandle )
    {
    SetHandle( aHandle );
    }


CMemSpyEngineHelperSysMemTrackerEntryHandleGeneric* CMemSpyEngineHelperSysMemTrackerEntryHandleGeneric::NewLC( CMemSpyEngineHelperSysMemTrackerImp& aTracker, TAny* aHandle, TMemSpyDriverContainerType aType )
    {
    CMemSpyEngineHelperSysMemTrackerEntryHandleGeneric* self = new(ELeave) CMemSpyEngineHelperSysMemTrackerEntryHandleGeneric( aTracker, aType );
    CleanupStack::PushL( self );
    self->ConstructL( aHandle );
    return self;
    }


TUint64 CMemSpyEngineHelperSysMemTrackerEntryHandleGeneric::Key() const
    {
    const TUint32 val = ( Type() << 28 ) + iHandleType; 
    TUint64 ret = val;
    ret <<= 32;
    ret += Handle();
    return ret;
    }


void CMemSpyEngineHelperSysMemTrackerEntryHandleGeneric::CreateChangeDescriptorL( CMemSpyEngineHelperSysMemTrackerCycle& aCycle )
    {
    // We lazy fetch the name to avoid having to get info for every single handle whenever a change cycle occurs...
    if  ( iName == NULL )
        {
        TMemSpyDriverHandleInfoGeneric info;
        const TInt err = Engine().Driver().GetGenericHandleInfo( KMemSpyDriverEnumerateContainerHandles, iHandleType, (TAny*) Handle(), info );
        if  ( err == KErrNone )
            {
            // Save name
            iName = HBufC::NewL( info.iName.Length() );
            iName->Des().Copy( info.iName );
            }
        else
            {
            iName = KNullDesC().AllocL();
            }
        }
    
    CMemSpyEngineHelperSysMemTrackerCycleChangeHandleGeneric* changeDescriptor = CMemSpyEngineHelperSysMemTrackerCycleChangeHandleGeneric::NewLC( Attributes(), *iName, Handle(), iHandleType );
    aCycle.AddAndPopL( changeDescriptor );
    }

















CMemSpyEngineHelperSysMemTrackerCycleChangeHandleGeneric::CMemSpyEngineHelperSysMemTrackerCycleChangeHandleGeneric( TUint8 aAttribs, TUint32 aHandle, TMemSpyDriverContainerType aType )
:   CMemSpyEngineHelperSysMemTrackerCycleChange( aAttribs ), iHandle( aHandle ), iType( aType )
    {
    }


CMemSpyEngineHelperSysMemTrackerCycleChangeHandleGeneric::~CMemSpyEngineHelperSysMemTrackerCycleChangeHandleGeneric()
    {
    delete iName;
    }


void CMemSpyEngineHelperSysMemTrackerCycleChangeHandleGeneric::ConstructL( const TDesC& aName )
    {
    BaseConstructL();

    // Save the thread name
    iName = aName.AllocL();
    }


CMemSpyEngineHelperSysMemTrackerCycleChangeHandleGeneric* CMemSpyEngineHelperSysMemTrackerCycleChangeHandleGeneric::NewLC( TUint8 aAttribs, const TDesC& aName, TUint32 aHandle, TMemSpyDriverContainerType aType )
    {
    CMemSpyEngineHelperSysMemTrackerCycleChangeHandleGeneric* self = new(ELeave) CMemSpyEngineHelperSysMemTrackerCycleChangeHandleGeneric( aAttribs, aHandle, aType );
    CleanupStack::PushL( self );
    self->ConstructL( aName );
    return self;
    }


TMemSpyEngineSysMemTrackerType CMemSpyEngineHelperSysMemTrackerCycleChangeHandleGeneric::Type() const
    {
    return EMemSpyEngineSysMemTrackerTypeHandleGeneric;
    }
   

void CMemSpyEngineHelperSysMemTrackerCycleChangeHandleGeneric::OutputHeaderL( CMemSpyEngineOutputSink& aSink, CMemSpyEngineHelperSysMemTrackerCycle& /*aCycle*/ )
    {
    _LIT( KHeaderLine, "Type, Name, Handle, Handle Type, Attribs");
    aSink.OutputLineL( KHeaderLine );
    }
 

void CMemSpyEngineHelperSysMemTrackerCycleChangeHandleGeneric::OutputContentL( CMemSpyEngineOutputSink& aSink, CMemSpyEngineHelperSysMemTrackerCycle& /*aCycle*/ )
    {
    _LIT( KFormat, "%S,%S,0x%08x,%S,%S" );
    //
    TMemSpySWMTTypeName type;
    FormatType( type );
    //
    TBuf<20> attribs;
    FormatAttributes( attribs );
    //
    HBufC* buf = HBufC::NewLC( 1024 );
    TPtr pBuf(buf->Des());
    //
    const TPtrC pType( CMemSpyEngineGenericKernelObjectList::TypeAsString( iType ) );

    // Now output line
    pBuf.Format( KFormat, 
                 &type,
                 iName, 
                 iHandle,
                 &pType,
                 &attribs
                 );

    aSink.OutputLineL( pBuf );
    CleanupStack::PopAndDestroy( buf );
    }



