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

#include "MemSpyEngineHelperSysMemTrackerEntryBitmap.h"

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
#include <memspy/engine/memspyenginehelperfbserv.h>
#include <memspy/engine/memspyengineobjectthread.h>
#include <memspy/engine/memspyengineobjectprocess.h>
#include <memspy/engine/memspyengineobjectcontainer.h>
#include <memspy/engine/memspyenginehelperfilesystem.h>
#include "MemSpyEngineHelperSysMemTrackerImp.h"
#include <memspy/engine/memspyenginehelpersysmemtrackercycle.h>
#include <memspy/engine/memspyenginehelpersysmemtrackerconfig.h>


CMemSpyEngineHelperSysMemTrackerEntryBitmap::CMemSpyEngineHelperSysMemTrackerEntryBitmap( CMemSpyEngineHelperSysMemTrackerImp& aTracker )
:   CMemSpyEngineHelperSysMemTrackerEntry( aTracker, EMemSpyEngineSysMemTrackerTypeOpenFile )
    {
    }


CMemSpyEngineHelperSysMemTrackerEntryBitmap::~CMemSpyEngineHelperSysMemTrackerEntryBitmap()
    {
    delete iInfo;
    }


void CMemSpyEngineHelperSysMemTrackerEntryBitmap::ConstructL( TInt aHandle )
    {
    iInfo = new(ELeave) TMemSpyEngineFBServBitmapInfo();
    CMemSpyEngineHelperFbServ::GetBitmapInfoL( aHandle, *iInfo );
    }


CMemSpyEngineHelperSysMemTrackerEntryBitmap* CMemSpyEngineHelperSysMemTrackerEntryBitmap::NewLC( CMemSpyEngineHelperSysMemTrackerImp& aTracker, TInt aHandle )
    {
    CMemSpyEngineHelperSysMemTrackerEntryBitmap* self = new(ELeave) CMemSpyEngineHelperSysMemTrackerEntryBitmap( aTracker );
    CleanupStack::PushL( self );
    self->ConstructL( aHandle );
    return self;
    }


TUint64 CMemSpyEngineHelperSysMemTrackerEntryBitmap::Key() const
    {
    return TUint64( iInfo->iHandle );
    }


void CMemSpyEngineHelperSysMemTrackerEntryBitmap::CreateChangeDescriptorL( CMemSpyEngineHelperSysMemTrackerCycle& aCycle )
    {
    CMemSpyEngineHelperSysMemTrackerCycleChangeBitmap* changeDescriptor = CMemSpyEngineHelperSysMemTrackerCycleChangeBitmap::NewLC( Attributes(), *iInfo );
    aCycle.AddAndPopL( changeDescriptor );
    }


void CMemSpyEngineHelperSysMemTrackerEntryBitmap::UpdateCycleStatistics( CMemSpyEngineHelperSysMemTrackerCycle& /*aCycle*/ )
    {
    // Doesn't participate in direct memory usage and indirect usage cannot really be measured.
    }














CMemSpyEngineHelperSysMemTrackerCycleChangeBitmap::CMemSpyEngineHelperSysMemTrackerCycleChangeBitmap( TUint8 aAttribs )
:   CMemSpyEngineHelperSysMemTrackerCycleChange( aAttribs )
    {
    }


CMemSpyEngineHelperSysMemTrackerCycleChangeBitmap::~CMemSpyEngineHelperSysMemTrackerCycleChangeBitmap()
    {
    delete iInfo;
    delete iBitmap;
    }


void CMemSpyEngineHelperSysMemTrackerCycleChangeBitmap::ConstructL( const TMemSpyEngineFBServBitmapInfo& aInfo )
    {
    BaseConstructL();

    iInfo = new(ELeave) TMemSpyEngineFBServBitmapInfo();
    *iInfo = aInfo;

    iBitmap = new(ELeave) CFbsBitmap();
    const TInt err = iBitmap->Duplicate( aInfo.iHandle );
    if ( err != KErrNone )
        {
        RDebug::Printf( "CMemSpyEngineHelperSysMemTrackerCycleChangeBitmap::ConstructL() - aInfo.iHandle: 0x%08x, dupe Err: %d", aInfo.iHandle, err );
        delete iBitmap;
        iBitmap = NULL;
        }
    }


CMemSpyEngineHelperSysMemTrackerCycleChangeBitmap* CMemSpyEngineHelperSysMemTrackerCycleChangeBitmap::NewLC( TUint8 aAttribs, const TMemSpyEngineFBServBitmapInfo& aInfo )
    {
    CMemSpyEngineHelperSysMemTrackerCycleChangeBitmap* self = new(ELeave) CMemSpyEngineHelperSysMemTrackerCycleChangeBitmap( aAttribs );
    CleanupStack::PushL( self );
    self->ConstructL( aInfo );
    return self;
    }


TMemSpyEngineSysMemTrackerType CMemSpyEngineHelperSysMemTrackerCycleChangeBitmap::Type() const
    {
    return EMemSpyEngineSysMemTrackerTypeBitmap;
    }
   

void CMemSpyEngineHelperSysMemTrackerCycleChangeBitmap::OutputHeaderL( CMemSpyEngineOutputSink& aSink, CMemSpyEngineHelperSysMemTrackerCycle& /*aCycle*/ )
    {
    _LIT( KHeaderLine, "Type, Handle, Dimensions [W x H], BPP, Disp. Mode, Color, Comp. Type, Size (Bytes), In Rom, Is Large, Is Mono, Is Compressed in Ram, Attribs");
    aSink.OutputLineL( KHeaderLine );
    }
 

void CMemSpyEngineHelperSysMemTrackerCycleChangeBitmap::OutputContentL( CMemSpyEngineOutputSink& aSink, CMemSpyEngineHelperSysMemTrackerCycle& /*aCycle*/ )
    {
    _LIT( KFormat, "%S,0x%08x,[%05d x %05d],%d,%d,%d,%d,%d,%d,%d,%d,%d,%S" );
    //
    TMemSpySWMTTypeName type;;
    FormatType( type );
    //
    TBuf<20> attribs;
    FormatAttributes( attribs );

    // Now output line
    aSink.OutputLineFormattedL( KFormat,
                                &type,
                                iInfo->iHandle,
                                iInfo->iSizeInPixels.iWidth,
                                iInfo->iSizeInPixels.iHeight,
                                iInfo->iBitsPerPixel,
                                iInfo->iDisplayMode,
                                iInfo->iColor,
                                iInfo->iCompressionType,
                                iInfo->iSizeInBytes,
                                ( iInfo->iFlags & TMemSpyEngineFBServBitmapInfo::EFlagsIsRomBitmap ) ? 1 : 0,
                                ( iInfo->iFlags & TMemSpyEngineFBServBitmapInfo::EFlagsIsMonochrome ) ? 1 : 0,
                                ( iInfo->iFlags & TMemSpyEngineFBServBitmapInfo::EFlagsIsLarge ) ? 1 : 0,
                                ( iInfo->iFlags & TMemSpyEngineFBServBitmapInfo::EFlagsIsCompressedInRam ) ? 1 : 0,
                                &attribs
                                );
    }


void CMemSpyEngineHelperSysMemTrackerCycleChangeBitmap::OutputDataL( CMemSpyEngineOutputSink& aSink, CMemSpyEngineHelperSysMemTrackerCycle& aCycle )
    {
    TRACE( RDebug::Printf( "CMemSpyEngineHelperSysMemTrackerCycleChangeBitmap::OutputDataL() - iInfo.iHandle: 0x%08x, iBitmap: 0x%08x, size: %03d x %03d", iInfo->iHandle, iBitmap, iInfo->iSizeInPixels.iWidth, iInfo->iSizeInPixels.iHeight ) );
    if  ( iBitmap && iInfo->iSizeInPixels.iWidth > 0 && iInfo->iSizeInPixels.iHeight > 0 )
        {
        // Get folder
        HBufC* fileName = HBufC::NewLC( KMaxFileName );
        TPtr pFileName( fileName->Des() );
        aCycle.GetDataFolderL( aSink.Engine().FsSession(), pFileName );
        TRACE( RDebug::Print( _L("CMemSpyEngineHelperSysMemTrackerCycleChangeBitmap::OutputDataL() - fileName1: [%S]"), fileName ) );

        // Get file name
        CMemSpyEngineFbServBitmap::GetExportableFileNameL( *iInfo, pFileName );
        TRACE( RDebug::Print( _L("CMemSpyEngineHelperSysMemTrackerCycleChangeBitmap::OutputDataL() - fileName2: [%S]"), fileName ) );

        // Remove .bmp extension, since we're actually saving mbms
        const TParsePtrC parser( pFileName );
        if ( parser.ExtPresent() )
            {
            const TInt extLength = parser.Ext().Length();
            const TInt totalLength = pFileName.Length();
            pFileName.SetLength( totalLength - extLength );
            }

        // Add .MBM
        _LIT( KEpocBitmapExtension, ".mbm" );
        pFileName.Append( KEpocBitmapExtension );

        const TInt err = iBitmap->Save( pFileName );
        if  ( err != KErrNone )
            {
            RDebug::Print( _L("CMemSpyEngineHelperSysMemTrackerCycleChangeBitmap::OutputDataL() - err: %d, fileName: [%S]"), err, fileName ); 
            }

        // Clean up
        CleanupStack::PopAndDestroy( fileName );
        delete iBitmap;
        iBitmap = NULL;
        }
    }



