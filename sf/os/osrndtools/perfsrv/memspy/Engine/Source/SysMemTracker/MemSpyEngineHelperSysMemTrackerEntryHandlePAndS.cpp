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

#include "MemSpyEngineHelperSysMemTrackerEntryHandlePAndS.h"

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
#include<memspy/engine/memspyenginehelperkernelcontainers.h>
#include <memspy/engine/memspyenginehelpersysmemtrackercycle.h>
#include <memspy/engine/memspyenginehelpersysmemtrackerconfig.h>




CMemSpyEngineHelperSysMemTrackerEntryHandlePAndS::CMemSpyEngineHelperSysMemTrackerEntryHandlePAndS( CMemSpyEngineHelperSysMemTrackerImp& aTracker )
:   CMemSpyEngineHelperSysMemTrackerEntry( aTracker, EMemSpyEngineSysMemTrackerTypeHandlePAndS )
    {
    }


CMemSpyEngineHelperSysMemTrackerEntryHandlePAndS::~CMemSpyEngineHelperSysMemTrackerEntryHandlePAndS()
    {
    delete iName;
    delete iThreadName;
    }


void CMemSpyEngineHelperSysMemTrackerEntryHandlePAndS::ConstructL( TAny* aHandle )
    {
    SetHandle( aHandle );
    TInt err = Engine().Driver().GetPAndSInfo( aHandle, iInfo );
    User::LeaveIfError( err );
    SetThread( iInfo.iTid );

    // Get thread name, if possible.
    CMemSpyThread* thread = NULL;
    CMemSpyProcess* process = NULL;
    err = Engine().Container().ProcessAndThreadByThreadId( iInfo.iTid, process, thread );
    if  ( err == KErrNone && thread )
        {
        TFullName* name = new(ELeave) TFullName();
        CleanupStack::PushL( name );
        thread->FullName( *name );
        iThreadName = name->AllocL();
        CleanupStack::PopAndDestroy( name );
        }
    else
        {
        iThreadName = KMemSpySWMTThreadNotFound().AllocL();
        }
    }


CMemSpyEngineHelperSysMemTrackerEntryHandlePAndS* CMemSpyEngineHelperSysMemTrackerEntryHandlePAndS::NewLC( CMemSpyEngineHelperSysMemTrackerImp& aTracker, TAny* aHandle )
    {
    CMemSpyEngineHelperSysMemTrackerEntryHandlePAndS* self = new(ELeave) CMemSpyEngineHelperSysMemTrackerEntryHandlePAndS( aTracker );
    CleanupStack::PushL( self );
    self->ConstructL( aHandle );
    return self;
    }


TUint64 CMemSpyEngineHelperSysMemTrackerEntryHandlePAndS::Key() const
    {
    const TUint32 val = ( Type() << 28 ); 
    TUint64 ret = val;
    ret <<= 32;
    ret += Handle();
    return ret;
    }


void CMemSpyEngineHelperSysMemTrackerEntryHandlePAndS::CreateChangeDescriptorL( CMemSpyEngineHelperSysMemTrackerCycle& aCycle )
    {
    // We lazy fetch the name to avoid having to get info for every single handle whenever a change cycle occurs...
    if  ( iName == NULL )
        {
        TMemSpyDriverHandleInfoGeneric info;
        const TInt err = Engine().Driver().GetGenericHandleInfo( KMemSpyDriverEnumerateContainerHandles, EMemSpyDriverContainerTypePropertyRef, (TAny*) Handle(), info );
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
    
    CMemSpyEngineHelperSysMemTrackerCycleChangeHandlePAndS* changeDescriptor = CMemSpyEngineHelperSysMemTrackerCycleChangeHandlePAndS::NewLC( Attributes(), Handle(), iInfo, *iName, *iThreadName );
    aCycle.AddAndPopL( changeDescriptor );
    }

















CMemSpyEngineHelperSysMemTrackerCycleChangeHandlePAndS::CMemSpyEngineHelperSysMemTrackerCycleChangeHandlePAndS( TUint8 aAttribs, TUint32 aHandle, const TMemSpyDriverPAndSInfo& aInfo )
:   CMemSpyEngineHelperSysMemTrackerCycleChange( aAttribs ), iHandle( aHandle ), iInfo( aInfo )
    {
    }


CMemSpyEngineHelperSysMemTrackerCycleChangeHandlePAndS::~CMemSpyEngineHelperSysMemTrackerCycleChangeHandlePAndS()
    {
    delete iName;
    delete iThreadName;
    }


void CMemSpyEngineHelperSysMemTrackerCycleChangeHandlePAndS::ConstructL( const TDesC& aName, const TDesC& aThreadName )
    {
    BaseConstructL();
    //
    iName = aName.AllocL();
    iThreadName = aThreadName.AllocL();
    }


CMemSpyEngineHelperSysMemTrackerCycleChangeHandlePAndS* CMemSpyEngineHelperSysMemTrackerCycleChangeHandlePAndS::NewLC( TUint8 aAttribs, TUint32 aHandle, const TMemSpyDriverPAndSInfo& aInfo, const TDesC& aName, const TDesC& aThreadName )
    {
    CMemSpyEngineHelperSysMemTrackerCycleChangeHandlePAndS* self = new(ELeave) CMemSpyEngineHelperSysMemTrackerCycleChangeHandlePAndS( aAttribs, aHandle, aInfo );
    CleanupStack::PushL( self );
    self->ConstructL( aName, aThreadName );
    return self;
    }


TMemSpyEngineSysMemTrackerType CMemSpyEngineHelperSysMemTrackerCycleChangeHandlePAndS::Type() const
    {
    return EMemSpyEngineSysMemTrackerTypeHandlePAndS;
    }
   

void CMemSpyEngineHelperSysMemTrackerCycleChangeHandlePAndS::OutputHeaderL( CMemSpyEngineOutputSink& aSink, CMemSpyEngineHelperSysMemTrackerCycle& /*aCycle*/ )
    {
    _LIT( KHeaderLine, "Type, Name, Handle, Key Type, Category, Key, Ref. Count, Creator SID, Thread Id, Thread Name, Attribs");
    aSink.OutputLineL( KHeaderLine );
    }
 

void CMemSpyEngineHelperSysMemTrackerCycleChangeHandlePAndS::OutputContentL( CMemSpyEngineOutputSink& aSink, CMemSpyEngineHelperSysMemTrackerCycle& /*aCycle*/ )
    {
    _LIT( KFormat, "%S,%S,0x%08x,%d,0x%08x,0x%08x,%d,0x%08x,%d,%S,%S" );
    //
    TMemSpySWMTTypeName type;
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
                 iName, 
                 iHandle,
                 iInfo.iType,
                 iInfo.iCategory,
                 iInfo.iKey,
                 iInfo.iRefCount,
                 iInfo.iCreatorSID,
                 iInfo.iTid,
                 iThreadName,
                 &attribs
                 );

    aSink.OutputLineL( pBuf );
    CleanupStack::PopAndDestroy( buf );
    }







