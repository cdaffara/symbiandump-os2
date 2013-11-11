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

#include "MemSpyEngineHelperSysMemTrackerEntryFbserv.h"

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




CMemSpyEngineHelperSysMemTrackerEntryFbserv::CMemSpyEngineHelperSysMemTrackerEntryFbserv( CMemSpyEngineHelperSysMemTrackerImp& aTracker, const TMemSpyDriverChunkInfo& aCurrentLarge, const TMemSpyDriverChunkInfo& aCurrentShared, TInt aBitmapConCount, TInt aFontConCount, TInt aAccessibleBitmapCount )
:   CMemSpyEngineHelperSysMemTrackerEntry( aTracker, EMemSpyEngineSysMemTrackerTypeFbserv ), 
    iCurrentLarge( aCurrentLarge ), iCurrentShared( aCurrentShared ), 
    iCurrentBitmapConCount( aBitmapConCount ), 
    iCurrentFontConCount( aFontConCount ),
    iCurrentAccessibleBitmapCount( aAccessibleBitmapCount )
    {
    iLastShared = aCurrentShared;
    iLastLarge = aCurrentLarge;
    iLastBitmapConCount = aBitmapConCount;
    iLastFontConCount = aFontConCount;
    iLastAccessibleBitmapCount = aAccessibleBitmapCount;
    }


CMemSpyEngineHelperSysMemTrackerEntryFbserv::~CMemSpyEngineHelperSysMemTrackerEntryFbserv()
    {
    }


void CMemSpyEngineHelperSysMemTrackerEntryFbserv::ConstructL()
    {
    }


CMemSpyEngineHelperSysMemTrackerEntryFbserv* CMemSpyEngineHelperSysMemTrackerEntryFbserv::NewLC( CMemSpyEngineHelperSysMemTrackerImp& aTracker, const TMemSpyDriverChunkInfo& aCurrentLarge, const TMemSpyDriverChunkInfo& aCurrentShared, TInt aBitmapConCount, TInt aFontConCount, TInt aAccessibleBitmapCount )
    {
    CMemSpyEngineHelperSysMemTrackerEntryFbserv* self = new(ELeave) CMemSpyEngineHelperSysMemTrackerEntryFbserv( aTracker, aCurrentLarge, aCurrentShared, aBitmapConCount, aFontConCount, aAccessibleBitmapCount );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


TUint64 CMemSpyEngineHelperSysMemTrackerEntryFbserv::Key() const
    {
    const TUint32 val = ( Type() << 28 );
    TUint64 ret = val;
    ret <<= 32;
    ret += reinterpret_cast< TUint32 >( iCurrentLarge.iHandle );
    return ret;
    }


void CMemSpyEngineHelperSysMemTrackerEntryFbserv::UpdateFromL( const CMemSpyEngineHelperSysMemTrackerEntry& aEntry )
    {
    const CMemSpyEngineHelperSysMemTrackerEntryFbserv& entry = static_cast< const CMemSpyEngineHelperSysMemTrackerEntryFbserv& >( aEntry );
    
    // Update state
    iLastShared = iCurrentShared;
    iCurrentShared = entry.iCurrentShared;
    iLastLarge = iCurrentLarge;
    iCurrentLarge = entry.iCurrentLarge;
    
    // Counts
    iLastBitmapConCount = iCurrentBitmapConCount;
    iCurrentBitmapConCount = entry.iCurrentBitmapConCount;
    //
    iLastFontConCount = iCurrentFontConCount;
    iCurrentFontConCount = entry.iCurrentFontConCount;
    //
    iLastAccessibleBitmapCount = iCurrentAccessibleBitmapCount;
    iCurrentAccessibleBitmapCount = entry.iCurrentAccessibleBitmapCount;
    }


TBool CMemSpyEngineHelperSysMemTrackerEntryFbserv::HasChangedL( const TMemSpyEngineHelperSysMemTrackerConfig& /*aConfig*/ ) const
    {
    const TBool hasChanged = ( iCurrentShared.iSize != iLastShared.iSize ) || ( iCurrentLarge.iSize != iLastLarge.iSize ) ||
                             ( iCurrentFontConCount != iLastFontConCount ) || ( iCurrentBitmapConCount != iLastBitmapConCount ) ||
                             ( iCurrentAccessibleBitmapCount != iCurrentAccessibleBitmapCount )
                             ;
    return hasChanged;
    }


void CMemSpyEngineHelperSysMemTrackerEntryFbserv::CreateChangeDescriptorL( CMemSpyEngineHelperSysMemTrackerCycle& aCycle )
    {
    CMemSpyEngineHelperSysMemTrackerCycleChangeFbserv* changeDescriptor = CMemSpyEngineHelperSysMemTrackerCycleChangeFbserv::NewLC( Attributes(), iCurrentLarge, iCurrentShared, iCurrentBitmapConCount, iCurrentFontConCount, iCurrentAccessibleBitmapCount );
    aCycle.AddAndPopL( changeDescriptor );
    }


void CMemSpyEngineHelperSysMemTrackerEntryFbserv::UpdateCycleStatistics( CMemSpyEngineHelperSysMemTrackerCycle& aCycle )
    {
    aCycle.AddToMemoryUsed( iCurrentShared.iSize );
    aCycle.AddToMemoryUsed( iCurrentLarge.iSize );
    }












































CMemSpyEngineHelperSysMemTrackerCycleChangeFbserv::CMemSpyEngineHelperSysMemTrackerCycleChangeFbserv( TUint8 aAttribs, const TMemSpyDriverChunkInfoWithoutName& aCurrentLarge, const TMemSpyDriverChunkInfoWithoutName& aCurrentShared, TInt aBitmapConCount, TInt aFontConCount, TInt aAccessibleBitmapCount )
:   CMemSpyEngineHelperSysMemTrackerCycleChange( aAttribs ), iSharedChunk( aCurrentShared ), iLargeChunk( aCurrentLarge ), iBitmapConCount( aBitmapConCount ), iFontConCount( aFontConCount ), iAccessibleBitmapCount( aAccessibleBitmapCount )
    {
    }


CMemSpyEngineHelperSysMemTrackerCycleChangeFbserv::~CMemSpyEngineHelperSysMemTrackerCycleChangeFbserv()
    {
    }


void CMemSpyEngineHelperSysMemTrackerCycleChangeFbserv::ConstructL()
    {
    BaseConstructL();
    }


CMemSpyEngineHelperSysMemTrackerCycleChangeFbserv* CMemSpyEngineHelperSysMemTrackerCycleChangeFbserv::NewLC( TUint8 aAttribs, const TMemSpyDriverChunkInfoWithoutName& aCurrentLarge, const TMemSpyDriverChunkInfoWithoutName& aCurrentShared, TInt aBitmapConCount, TInt aFontConCount, TInt aAccessibleBitmapCount )
    {
    CMemSpyEngineHelperSysMemTrackerCycleChangeFbserv* self = new(ELeave) CMemSpyEngineHelperSysMemTrackerCycleChangeFbserv( aAttribs, aCurrentLarge, aCurrentShared, aBitmapConCount, aFontConCount, aAccessibleBitmapCount );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


TMemSpyEngineSysMemTrackerType CMemSpyEngineHelperSysMemTrackerCycleChangeFbserv::Type() const
    {
    return EMemSpyEngineSysMemTrackerTypeFbserv;
    }
   

void CMemSpyEngineHelperSysMemTrackerCycleChangeFbserv::OutputHeaderL( CMemSpyEngineOutputSink& aSink, CMemSpyEngineHelperSysMemTrackerCycle& /*aCycle*/ )
    {
    _LIT( KHeaderChunk, "Type, FbsSharedChunk[Handle,Base,Size,Max Size], FbsLargeChunk[Handle,Base,Size,Max Size], AllBC, BCC, FCC, Attribs");
    aSink.OutputLineL( KHeaderChunk );
    }
 

void CMemSpyEngineHelperSysMemTrackerCycleChangeFbserv::OutputContentL( CMemSpyEngineOutputSink& aSink, CMemSpyEngineHelperSysMemTrackerCycle& /*aCycle*/ )
    {
    _LIT( KFormat, "%S,FbsSharedChunk[0x%08x,0x%08x,%d,%d],FbsLargeChunk[0x%08x,0x%08x,%d,%d],%d,%d,%d,%S" );
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

                 // Shared chunk
                 iSharedChunk.iHandle,
                 iSharedChunk.iBaseAddress,
                 iSharedChunk.iSize,
                 iSharedChunk.iMaxSize,

                 // Large chunk
                 iLargeChunk.iHandle,
                 iLargeChunk.iBaseAddress,
                 iLargeChunk.iSize,
                 iLargeChunk.iMaxSize,

                 // Container counts
                 iAccessibleBitmapCount,
                 iBitmapConCount,
                 iFontConCount,

                 &attribs
                 );

    aSink.OutputLineL( pBuf );
    CleanupStack::PopAndDestroy( buf );
    }


