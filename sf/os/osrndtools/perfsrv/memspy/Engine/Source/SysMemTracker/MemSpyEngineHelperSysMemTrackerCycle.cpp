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

#include <memspy/engine/memspyenginehelpersysmemtrackercycle.h>

// System includes
#include <e32debug.h>
#include <hal.h>
#include <hal_data.h>

// User includes
#include <memspy/engine/memspyengine.h>
#include <memspy/engine/memspyengineutils.h>
#include <memspy/engine/memspyengineobjectthread.h>
#include <memspy/engine/memspyengineobjectprocess.h>
#include <memspy/engine/memspyengineobjectcontainer.h>
#include <memspy/engine/memspyenginehelperheap.h>
#include <memspy/engine/memspyenginehelperchunk.h>
#include <memspy/engine/memspyengineoutputsink.h>
#include <memspy/engine/memspyengineoutputlist.h>
#include <memspy/engine/memspyengineoutputsink.h>
#include <memspy/engine/memspyenginehelpersysmemtrackercyclechange.h>
    
// Literal constants
_LIT( KMemSpySWMTDataFolderSpecification, "Data" );
_LIT( KMemSpySWMTFolderSpecification, "System Wide MT\\Cycle %04d" );



CMemSpyEngineHelperSysMemTrackerCycle::CMemSpyEngineHelperSysMemTrackerCycle( const TMemSpyEngineHelperSysMemTrackerConfig& aConfig, const CMemSpyEngineHelperSysMemTrackerCycle* aPreviousCycle )
:   iConfig( aConfig ), iPreviousCycle( aPreviousCycle ), iCycleNumber( 1 )
    {
    iTime.HomeTime();
    if  ( aPreviousCycle )
        {
        iCycleNumber = aPreviousCycle->CycleNumber() + 1;
        }
    }


CMemSpyEngineHelperSysMemTrackerCycle::~CMemSpyEngineHelperSysMemTrackerCycle()
    {
    delete iFixedItems;
    delete iCaption;
    delete iTimeFormatted;
    //
    DiscardChanges();
    iChangeDescriptors.Close();
    }


void CMemSpyEngineHelperSysMemTrackerCycle::ConstructL()
    {
    iFixedItems = CMemSpyEngineOutputList::NewL();

    // Get time stamp
    TBuf<128> timeBuf;
    MemSpyEngineUtils::FormatTimeSimple( timeBuf, iTime );
    iTimeFormatted = timeBuf.AllocL();
    
    // Get free ram
    TInt freeRam = KErrNotFound;
    User::LeaveIfError( HAL::Get( HALData::EMemoryRAMFree, freeRam ) );
    iMemoryFree = freeRam;
    }


CMemSpyEngineHelperSysMemTrackerCycle* CMemSpyEngineHelperSysMemTrackerCycle::NewLC( const TMemSpyEngineHelperSysMemTrackerConfig& aConfig, const CMemSpyEngineHelperSysMemTrackerCycle* aPreviousCycle )
    {
    CMemSpyEngineHelperSysMemTrackerCycle* self = new(ELeave) CMemSpyEngineHelperSysMemTrackerCycle( aConfig, aPreviousCycle );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


EXPORT_C TInt CMemSpyEngineHelperSysMemTrackerCycle::CycleNumber() const
    {
    return iCycleNumber;
    }


EXPORT_C const TTime& CMemSpyEngineHelperSysMemTrackerCycle::Time() const
    {
    return iTime;
    }


EXPORT_C const TDesC& CMemSpyEngineHelperSysMemTrackerCycle::TimeFormatted() const
    {
    return *iTimeFormatted;
    }


EXPORT_C const TDesC& CMemSpyEngineHelperSysMemTrackerCycle::Caption() const
    {
    return *iCaption;
    }


EXPORT_C const TInt64& CMemSpyEngineHelperSysMemTrackerCycle::MemoryFree() const
    {
    return iMemoryFree;
    }


EXPORT_C TInt64 CMemSpyEngineHelperSysMemTrackerCycle::MemoryFreePreviousCycle() const
    {
    TInt64 ret = MemoryFree();
    //
    if  ( iPreviousCycle )
        {
        ret = iPreviousCycle->MemoryFree();
        }
    //
    return ret;
    }


EXPORT_C TInt64 CMemSpyEngineHelperSysMemTrackerCycle::MemoryDelta() const
    {
    const TInt64 ret = ( iMemoryFree - iMemoryUsed );
    return ret;
    }


EXPORT_C TInt CMemSpyEngineHelperSysMemTrackerCycle::ChangeCount() const
    {
    return iChangeCount;
    }


EXPORT_C TInt CMemSpyEngineHelperSysMemTrackerCycle::MdcaCount() const
    {
    return iFixedItems->Count();
    }


EXPORT_C TPtrC CMemSpyEngineHelperSysMemTrackerCycle::MdcaPoint( TInt aIndex ) const
    {
    return iFixedItems->MdcaPoint( aIndex );
    }


void CMemSpyEngineHelperSysMemTrackerCycle::FinalizeL()
    {
    _LIT( KChange, "change" );
    _LIT( KChanges, "changes" );

    // Save change count
    iChangeCount = iChangeDescriptors.Count();

    // First make the main caption
    TMemSpySizeText sizeText;
    TMemSpySizeText deltaText;
    TPtrC pChangeText( KChange );
   
    // Get free RAM formatted as a descriptor
    sizeText = MemSpyEngineUtils::FormatSizeTextPrecise( MemoryFree() );

    // Get delta formatted as a descriptor
    const TInt64 delta( MemoryDelta() );
    deltaText = MemSpyEngineUtils::FormatSizeTextPrecise( delta );

    // Get number of changed threads as a descriptor
    if  ( ChangeCount() == 0 || ChangeCount() > 1 )
        {
        pChangeText.Set( KChanges );
        }

    // Now make the caption
    if  ( MemoryDelta() != 0 )
        {
        TBuf<300> buf;
        _LIT( KMainCaption1, "\t[%03d] F: %S\t\tD: %S, %d %S" );
        buf.Format( KMainCaption1, CycleNumber(), &sizeText, &deltaText, ChangeCount(), &pChangeText );
        HBufC* caption = buf.AllocL();
        delete iCaption;
        iCaption = caption;
        }
    else
        {
        TBuf<300> buf;
        _LIT( KMainCaption2, "\t[%03d] F: %S\t\tNo Delta, %d %S" );
        buf.Format( KMainCaption2, CycleNumber(), &sizeText, ChangeCount(), &pChangeText );
        HBufC* caption = buf.AllocL();
        delete iCaption;
        iCaption = caption;
        }

    // Now make the child entries
    if  ( CycleNumber() > 0 )
        {
        _LIT(KItem1a, "Cycle");
        iFixedItems->AddItemL( KItem1a, CycleNumber() );
        }
    else
        {
        _LIT(KItem1a, "Baseline");
        iFixedItems->AddItemL( KItem1a );
        }
    //
    _LIT(KItem1c, "Time");
    iFixedItems->AddItemL( KItem1c, iTimeFormatted );
    //
    _LIT(KItem2, "Memory Free");
    iFixedItems->AddItemL( KItem2, sizeText );
    //
    if  ( CycleNumber() > 0 )
        {
        _LIT(KItem2b, "Memory Change (vs. Last Cycle)");
        sizeText = MemSpyEngineUtils::FormatSizeTextPrecise( MemoryFree() - MemoryFreePreviousCycle() );
        iFixedItems->AddItemL( KItem2b, sizeText );
        //
        _LIT(KItem3, "Number of Changes");
        iFixedItems->AddItemL( KItem3, ChangeCount() );
        }
    }


void CMemSpyEngineHelperSysMemTrackerCycle::DiscardChanges()
    {
    iChangeDescriptors.ResetAndDestroy();
    }


CMemSpyEngineHelperSysMemTrackerCycleChange& CMemSpyEngineHelperSysMemTrackerCycle::ChangeAt( TInt aIndex )
    {
    return *iChangeDescriptors[ aIndex ];
    }


void CMemSpyEngineHelperSysMemTrackerCycle::GetDataFolderL( RFs& aFsSession, TDes& aFolder )
    {
    TFileName fileName;
    fileName.Format( KMemSpySWMTFolderSpecification, CycleNumber() );

    CMemSpyEngineSinkMetaData* metaData = CMemSpyEngineSinkMetaData::NewL( KNullDesC, fileName, KNullDesC, ETrue, EFalse, Time() );
    CleanupStack::PushL( metaData );
    MemSpyEngineUtils::GetFolderL( aFsSession, aFolder, *metaData );
    CleanupStack::PopAndDestroy( metaData );

    // This get's us something like:
    //
    //    E:\MemSpy\System Wide MT\Data - Cycle 001\Log.txt
    //
    // Now we must strip off the filename and return just the folder specification
    const TParsePtrC parser( aFolder );
    if  ( parser.NamePresent() && parser.ExtPresent() )
        {
        const TPtrC nameAndExt( parser.NameAndExt() );
        aFolder.SetLength( aFolder.Length() - nameAndExt.Length() );
        }

    // Add "Data" on to the end so that we put all data in a clearly marked subdir)
    aFolder.Append( KMemSpySWMTDataFolderSpecification );
    TRACE( RDebug::Print(_L("CMemSpyEngineHelperSysMemTrackerCycle::GetDataFolderL() - folder1: %S"), &aFolder ) );

    // Now we should return something like:
    //
    //    E:\MemSpy\System Wide MT\Data - Cycle 001
    TRACE( RDebug::Print(_L("CMemSpyEngineHelperSysMemTrackerCycle::GetDataFolderL() - folder2: %S"), &aFolder ) );

    const TInt err = aFsSession.MkDirAll( aFolder );
    if  ( !(err == KErrNone || err == KErrAlreadyExists ) )
        {
        User::LeaveIfError( err );
        }
    }


void CMemSpyEngineHelperSysMemTrackerCycle::DataStreamBeginL( CMemSpyEngineOutputSink& aSink, const TDesC& aContext )
    {
    // First make the folder...
    HBufC* folder = HBufC::NewLC( KMemSpySWMTFolderSpecification().Length() + 10 );
    TPtr pFolder( folder->Des() );
    pFolder.Format( KMemSpySWMTFolderSpecification, CycleNumber() );

    // Now start the data stream
    aSink.DataStreamBeginL( aContext, pFolder, KMemSpyLogDefaultExtension, ETrue, EFalse );
    CleanupStack::PopAndDestroy( folder );
    }


void CMemSpyEngineHelperSysMemTrackerCycle::DataStreamEndL( CMemSpyEngineOutputSink& aSink )
    {
	aSink.DataStreamEndL();
    }


void CMemSpyEngineHelperSysMemTrackerCycle::AddAndPopL( CMemSpyEngineHelperSysMemTrackerCycleChange* aInfo )
    {
    iChangeDescriptors.AppendL( aInfo );
    CleanupStack::Pop( aInfo );
    }


const TMemSpyEngineHelperSysMemTrackerConfig& CMemSpyEngineHelperSysMemTrackerCycle::Config() const
    {
    return iConfig; 
    }


void CMemSpyEngineHelperSysMemTrackerCycle::AddToMemoryUsed( TInt aValue )
    {
    iMemoryUsed += aValue;
    }


void CMemSpyEngineHelperSysMemTrackerCycle::AddToMemoryHeapAllocs( TInt aValue )
    {
    iMemoryHeapAllocs += aValue;
    }


void CMemSpyEngineHelperSysMemTrackerCycle::AddToMemoryHeapFrees( TInt aValue )
    {
    iMemoryHeapFrees += aValue;
    }


void CMemSpyEngineHelperSysMemTrackerCycle::AddToCellCountFree( TInt aValue )
    {
    iCellCountsFrees += aValue;
    }


void CMemSpyEngineHelperSysMemTrackerCycle::AddToCellCountAlloc( TInt aValue )
    {
    iCellCountsAllocs += aValue;
    }


const TInt64& CMemSpyEngineHelperSysMemTrackerCycle::MemoryUsed() const
    {
    return iMemoryUsed;
    }


const TInt64& CMemSpyEngineHelperSysMemTrackerCycle::MemoryHeapAllocs() const
    {
    return iMemoryHeapAllocs;
    }


const TInt64& CMemSpyEngineHelperSysMemTrackerCycle::MemoryHeapFrees() const
    {
    return iMemoryHeapFrees;
    }


const TInt64& CMemSpyEngineHelperSysMemTrackerCycle::MemoryHeapCellCountAlloc() const
    {
    return iCellCountsAllocs;
    }


const TInt64& CMemSpyEngineHelperSysMemTrackerCycle::MemoryHeapCellCountFree() const
    {
    return iCellCountsFrees;
    }







