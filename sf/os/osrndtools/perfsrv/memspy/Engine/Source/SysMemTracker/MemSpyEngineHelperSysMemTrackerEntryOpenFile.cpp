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

#include "MemSpyEngineHelperSysMemTrackerEntryOpenFile.h"

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
#include <memspy/engine/memspyenginehelperfilesystem.h>
#include "MemSpyEngineHelperSysMemTrackerImp.h"
#include <memspy/engine/memspyenginehelpersysmemtrackercycle.h>
#include <memspy/engine/memspyenginehelpersysmemtrackerconfig.h>


CMemSpyEngineHelperSysMemTrackerEntryOpenFile::CMemSpyEngineHelperSysMemTrackerEntryOpenFile( CMemSpyEngineHelperSysMemTrackerImp& aTracker )
:   CMemSpyEngineHelperSysMemTrackerEntry( aTracker, EMemSpyEngineSysMemTrackerTypeOpenFile )
    {
    }


CMemSpyEngineHelperSysMemTrackerEntryOpenFile::~CMemSpyEngineHelperSysMemTrackerEntryOpenFile()
    {
    delete iFileName;
    delete iThreadName;
    }


void CMemSpyEngineHelperSysMemTrackerEntryOpenFile::ConstructL( const CMemSpyEngineOpenFileListForThread& aThreadInfo, const CMemSpyEngineOpenFileListEntry& aEntryInfo )
    {
    // Get basic details
    iThreadName = aThreadInfo.ThreadName().AllocL();
    SetThread( aThreadInfo.ThreadId() );
    SetProcess( aThreadInfo.ProcessId() );

    // Set file details
    iFileName = aEntryInfo.FileName().AllocL();
    iSize = aEntryInfo.Size();
    iUniqueFileId = aEntryInfo.UniqueFileId();
    }


CMemSpyEngineHelperSysMemTrackerEntryOpenFile* CMemSpyEngineHelperSysMemTrackerEntryOpenFile::NewLC( CMemSpyEngineHelperSysMemTrackerImp& aTracker, const CMemSpyEngineOpenFileListForThread& aThreadInfo, const CMemSpyEngineOpenFileListEntry& aEntryInfo )
    {
    CMemSpyEngineHelperSysMemTrackerEntryOpenFile* self = new(ELeave) CMemSpyEngineHelperSysMemTrackerEntryOpenFile( aTracker );
    CleanupStack::PushL( self );
    self->ConstructL( aThreadInfo, aEntryInfo );
    return self;
    }


TUint64 CMemSpyEngineHelperSysMemTrackerEntryOpenFile::Key() const
    {
    const TUint32 val = ( Type() << 28 ) + ( iUniqueFileId < 24 ) + ThreadId(); 
    TUint64 ret = val;
    ret <<= 32;
    //
    const TUint32 fileNameHash = MemSpyEngineUtils::Hash( *iFileName );
    ret += fileNameHash;
    //
    return ret;
    }


void CMemSpyEngineHelperSysMemTrackerEntryOpenFile::CreateChangeDescriptorL( CMemSpyEngineHelperSysMemTrackerCycle& aCycle )
    {
    CMemSpyEngineHelperSysMemTrackerCycleChangeOpenFile* changeDescriptor = CMemSpyEngineHelperSysMemTrackerCycleChangeOpenFile::NewLC( Attributes(), *iThreadName, *iFileName, iSize, iUniqueFileId );
    aCycle.AddAndPopL( changeDescriptor );
    }


void CMemSpyEngineHelperSysMemTrackerEntryOpenFile::UpdateCycleStatistics( CMemSpyEngineHelperSysMemTrackerCycle& /*aCycle*/ )
    {
    // Doesn't participate in direct memory usage and indirect usage cannot really be measured.
    }























CMemSpyEngineHelperSysMemTrackerCycleChangeOpenFile::CMemSpyEngineHelperSysMemTrackerCycleChangeOpenFile( TUint8 aAttribs, TInt aSize, TInt aUniqueFileId )
:   CMemSpyEngineHelperSysMemTrackerCycleChange( aAttribs ), iSize( aSize ), iUniqueFileId( aUniqueFileId )
    {
    }


CMemSpyEngineHelperSysMemTrackerCycleChangeOpenFile::~CMemSpyEngineHelperSysMemTrackerCycleChangeOpenFile()
    {
    delete iFileName;
    delete iThreadName;
    }


void CMemSpyEngineHelperSysMemTrackerCycleChangeOpenFile::ConstructL( const TDesC& aThreadName, const TDesC& aFileName )
    {
    BaseConstructL();

    // Save the thread name
    iFileName = aFileName.AllocL();
    iThreadName = aThreadName.AllocL();
    }


CMemSpyEngineHelperSysMemTrackerCycleChangeOpenFile* CMemSpyEngineHelperSysMemTrackerCycleChangeOpenFile::NewLC( TUint8 aAttribs, const TDesC& aThreadName, const TDesC& aFileName, TInt aSize, TInt aUniqueFileId )
    {
    CMemSpyEngineHelperSysMemTrackerCycleChangeOpenFile* self = new(ELeave) CMemSpyEngineHelperSysMemTrackerCycleChangeOpenFile( aAttribs, aSize, aUniqueFileId );
    CleanupStack::PushL( self );
    self->ConstructL( aThreadName, aFileName );
    return self;
    }


TMemSpyEngineSysMemTrackerType CMemSpyEngineHelperSysMemTrackerCycleChangeOpenFile::Type() const
    {
    return EMemSpyEngineSysMemTrackerTypeOpenFile;
    }
   

void CMemSpyEngineHelperSysMemTrackerCycleChangeOpenFile::OutputHeaderL( CMemSpyEngineOutputSink& aSink, CMemSpyEngineHelperSysMemTrackerCycle& /*aCycle*/ )
    {
    _LIT( KHeaderLine, "Type, File Name, Thread, File Size, Instance Id, Attribs");
    aSink.OutputLineL( KHeaderLine );
    }
 

void CMemSpyEngineHelperSysMemTrackerCycleChangeOpenFile::OutputContentL( CMemSpyEngineOutputSink& aSink, CMemSpyEngineHelperSysMemTrackerCycle& /*aCycle*/ )
    {
    _LIT( KFormat, "%S,%S,%S,%d,%d,%S" );
    //
    TMemSpySWMTTypeName type;;
    FormatType( type );
    //
    TBuf<20> attribs;
    FormatAttributes( attribs );
    //
    HBufC* buf = HBufC::NewLC( 1024 );
    TPtr pBuf( buf->Des() );

    // Now output line
    pBuf.Format( KFormat,
                 &type,
                 iFileName,
                 iThreadName,
                 iSize,
                 iUniqueFileId,
                 &attribs
                 );

    aSink.OutputLineL( pBuf );
    CleanupStack::PopAndDestroy( buf );
    }


void CMemSpyEngineHelperSysMemTrackerCycleChangeOpenFile::OutputDataL( CMemSpyEngineOutputSink& /*aSink*/, CMemSpyEngineHelperSysMemTrackerCycle& /*aCycle*/ )
    {
    }
