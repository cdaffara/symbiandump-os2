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

#include <memspy/engine/memspyenginehelperheap.h>

// System includes
#include <s32mem.h>

// Driver includes
#include <memspy/driver/memspydriverclient.h>

// User includes
#include <memspy/engine/memspyengine.h>
#include <memspy/engine/memspyengineutils.h>
#include <memspy/engine/memspyengineoutputsink.h>
#include <memspy/engine/memspyengineoutputlist.h>
#include <memspy/engine/memspyengineobjectthread.h>
#include <memspy/engine/memspyengineobjectprocess.h>
#include <memspy/engine/memspyenginehelperrom.h>
#include <memspy/engine/memspyengineobjectcontainer.h>
#include <memspy/engine/memspyenginehelpercodesegment.h>
#include <memspy/driver/memspydriverenumerationsshared.h>

// Constants
const TBool KMemSpyHeapDumpCreateOwnDataStream = ETrue;

// Literal constants
_LIT( KCellTypeGoodAllocatedCell,        "[Allocated Cell]            ");
_LIT( KCellTypeGoodFreeCell,             "[Free Cell]                 ");
_LIT( KCellTypeBadAllocatedCellSize,     "[Bad Allocated Cell Size]   ");
_LIT( KCellTypeBadAllocatedCellAddress,  "[Bad Allocated Cell Address]");
_LIT( KCellTypeBadFreeCellAddress,       "[Bad Free Cell Address]     ");
_LIT( KCellTypeBadFreeCellSize,          "[Bad Free Cell Size]        ");
_LIT( KCellTypeBad,                      "[Bad Cell]                  ");
_LIT( KCellTypeUnknown,                  "[Unknown!]                  ");
_LIT( KCellListLineFormat, "%S cell: 0x%08x, cellLen: %8d, allocNum: %8d, nestingLev: %8d, cellData: 0x%08x, cellDataAddr: 0x%08x, headerSize: %02d");
_LIT( KMemSpyMarkerHeapData, "<%SMEMSPY_HEAP_DATA_%03d>" );
_LIT( KMemSpyMarkerCSV, "<%SMEMSPY_HEAP_CSV>" );
_LIT( KMemSpyPrefixHeapData, "HeapData - %S - ");
_LIT( KMemSpyPrefixCellList, "CellList - %S - ");
_LIT( KMemSpyPrefixCSV, "CSV - " );


CMemSpyEngineHelperHeap::CMemSpyEngineHelperHeap( CMemSpyEngine& aEngine )
:   iEngine( aEngine )
    {
    }

    
CMemSpyEngineHelperHeap::~CMemSpyEngineHelperHeap()
    {
    }


void CMemSpyEngineHelperHeap::ConstructL()
    {
    }


CMemSpyEngineHelperHeap* CMemSpyEngineHelperHeap::NewL( CMemSpyEngine& aEngine )
    {
    CMemSpyEngineHelperHeap* self = new(ELeave) CMemSpyEngineHelperHeap( aEngine );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }










EXPORT_C void CMemSpyEngineHelperHeap::OutputCellListingUserL( const CMemSpyThread& aThread )
    {
    // Suspend the process
    iEngine.ProcessSuspendLC( aThread.Process().Id() );

    // Free cells
    RArray<TMemSpyDriverFreeCell> freeCells;
    CleanupClosePushL( freeCells );

    // Info section
    TMemSpyHeapInfo heapInfo;
    const TInt error = iEngine.Driver().GetHeapInfoUser( heapInfo, aThread.Id(), freeCells );
    if ( error == KErrNone )
        {
        UpdateSharedHeapInfoL( aThread.Process().Id(), aThread.Id(), heapInfo );
        }
    if  ( error == KErrNone && heapInfo.Type() != TMemSpyHeapInfo::ETypeUnknown )
        {
        // Get thread name for context
        const TFullName pName( aThread.FullName() );

        // Begin a new data stream
        _LIT( KMemSpyFolder, "Heap\\Cell List" );
        _LIT( KMemSpyContext, "Cell List - %S" );
        HBufC* context = HBufC::NewLC( KMaxFileName );
        TPtr pContext( context->Des() );
        pContext.Format( KMemSpyContext, &pName );
        iEngine.Sink().DataStreamBeginL( pContext, KMemSpyFolder );
        CleanupStack::PopAndDestroy( context );

        // Set prefix for overall listing
        iEngine.Sink().OutputPrefixSetFormattedLC( KMemSpyPrefixCellList, &pName );

        // Start new section
        _LIT(KHeader, "CELL LISTING");
        iEngine.Sink().OutputSectionHeadingL( KHeader, '=' );

        // Prepare temp buffers
        TBuf<KMaxFullName + 100> printFormat;
        HBufC* tempBuffer = HBufC::NewLC( 2048 );
        TPtr pTempBuffer( tempBuffer->Des() );

        // Print initial info
        OutputHeapInfoL( heapInfo, pName, &freeCells );

        // Code segments (needed for map file reading...)
        _LIT(KCellListCodeSegInfoFormat, "CodeSegs - ");
        iEngine.HelperCodeSegment().OutputCodeSegmentsL( aThread.Process().Id(), printFormat, KCellListCodeSegInfoFormat, '-', ETrue );
    
        // Now walk the heap!
        TInt r = iEngine.Driver().WalkHeapInit( aThread.Id() );
        if  ( r == KErrNone )
            {
             _LIT(KHeader2, "Cells");
            iEngine.Sink().OutputSectionHeadingL( KHeader2, '-' );

            TMemSpyDriverCellType cellType;
            TAny* cellAddress;
            TInt cellLength;
            TInt cellNestingLevel;
            TInt cellAllocationNumber;
            TInt cellHeaderSize;
            TAny* cellPayloadAddress;
            TBuf8<4> cellData;
            //
            r = iEngine.Driver().WalkHeapNextCell( aThread.Id(), cellType, cellAddress, cellLength, cellNestingLevel, cellAllocationNumber, cellHeaderSize, cellPayloadAddress );
            while( r == KErrNone )
                {
                TUint fourByteCellData = 0;
                TPtrC pType(KNullDesC);
                //
				if (cellType & EMemSpyDriverAllocatedCellMask)
					{
                    r = iEngine.Driver().WalkHeapReadCellData( cellAddress, cellData, 4 );
                    if  ( r == KErrNone )
                        {
                        fourByteCellData = DescriptorAsDWORD( cellData );
                        }
                    pType.Set(KCellTypeGoodAllocatedCell);
                    }
				else if (cellType & EMemSpyDriverFreeCellMask)
					{
                    pType.Set(KCellTypeGoodFreeCell);
					}
				else if (cellType & EMemSpyDriverBadCellMask)
					{
					switch (cellType)
						{
					case EMemSpyDriverHeapBadAllocatedCellSize:
						pType.Set(KCellTypeBadAllocatedCellSize);
						break;
					case EMemSpyDriverHeapBadAllocatedCellAddress:
						pType.Set(KCellTypeBadAllocatedCellAddress);
						break;
					case EMemSpyDriverHeapBadFreeCellAddress:
						pType.Set(KCellTypeBadFreeCellAddress);
						break;
					case EMemSpyDriverHeapBadFreeCellSize:
						pType.Set(KCellTypeBadFreeCellSize);
						break;
					default:
						pType.Set(KCellTypeBad);
						break;
						}
					}
				else
					{
                    pType.Set(KCellTypeUnknown);
                    }

                if  ( r == KErrNone )
                    {
                    pTempBuffer.Format( KCellListLineFormat, &pType, cellAddress, cellLength, cellAllocationNumber, cellNestingLevel, fourByteCellData, cellPayloadAddress, cellHeaderSize );
                    iEngine.Sink().OutputLineL( pTempBuffer );
                    //
                    r = iEngine.Driver().WalkHeapNextCell( aThread.Id(), cellType, cellAddress, cellLength, cellNestingLevel, cellAllocationNumber, cellHeaderSize, cellPayloadAddress );
                    }
                }
            //
            iEngine.Driver().WalkHeapClose();
            }
        CleanupStack::PopAndDestroy( tempBuffer );
        CleanupStack::PopAndDestroy(); // clear prefix

        iEngine.Sink().DataStreamEndL();
        }

    CleanupStack::PopAndDestroy( &freeCells );
    CleanupStack::PopAndDestroy(); // resume process
    }



















EXPORT_C void CMemSpyEngineHelperHeap::OutputHeapDataUserL( const CMemSpyThread& aThread )
    {
    OutputHeapDataUserL( aThread, KMemSpyHeapDumpCreateOwnDataStream );
    }


void CMemSpyEngineHelperHeap::OutputHeapDataUserL( const CMemSpyThread& aThread, TBool aCreateDataStream )
    {
    // Make sure the process is suspended for the entire time we are manipulating it's heap
    iEngine.ProcessSuspendLC( aThread.Process().Id() );

    // Get the heap info, including cell information
    RArray<TMemSpyDriverCell> cells;
    CleanupClosePushL( cells );
    TMemSpyHeapInfo heapInfo;
    TRACE( RDebug::Printf( "CMemSpyEngineHelperHeap::OutputHeapDataUserL() - checksum1: 0x%08x", heapInfo.AsRHeap().Statistics().StatsFree().Checksum() ) );
    GetHeapInfoUserL(aThread.Process().Id(), aThread.Id(), heapInfo, &cells, ETrue);
    TRACE( RDebug::Printf( "CMemSpyEngineHelperHeap::OutputHeapDataUserL() - checksum2: 0x%08x", heapInfo.AsRHeap().Statistics().StatsFree().Checksum() ) );

    // Get the heap data
    const TFullName pName( aThread.FullName() );
    OutputHeapDataUserL( aThread.Process().Id(), aThread.Id(), pName, heapInfo, aCreateDataStream, &cells );
    CleanupStack::PopAndDestroy( &cells );

    // Resume process
    CleanupStack::PopAndDestroy();
    }


EXPORT_C void CMemSpyEngineHelperHeap::OutputHeapDataUserL(const TProcessId& aPid, 
                                                           const TThreadId& aTid, 
                                                           const TDesC& aThreadName, 
                                                           const TMemSpyHeapInfo& aInfo, 
                                                           const RArray<TMemSpyDriverCell>* aCells)
    {
    OutputHeapDataUserL(aPid, aTid, aThreadName, aInfo, ETrue, aCells);
    }

void CMemSpyEngineHelperHeap::OutputHeapDataUserL(const TProcessId& aPid, 
                                                  const TThreadId& aTid, 
                                                  const TDesC& aThreadName, 
                                                  const TMemSpyHeapInfo& aInfo, 
                                                  TBool aCreateDataStream, 
                                                  const RArray<TMemSpyDriverCell>* aCells )
    {
    TBuf<KMaxFullName + 100> printFormat;

    // Begin a new data stream
    if  ( aCreateDataStream )
        {
        _LIT( KMemSpyFolder, "Heap\\Data" );
        _LIT( KMemSpyContext, "Heap Data - %S" );
        HBufC* context = HBufC::NewLC( KMaxFileName );
        TPtr pContext( context->Des() );
        pContext.Format( KMemSpyContext, &aThreadName );
        iEngine.Sink().DataStreamBeginL( pContext, KMemSpyFolder );
        CleanupStack::PopAndDestroy( context );
        }

    // Get the heap info first of all
    iEngine.ProcessSuspendLC( aPid );

    // Start marker
    iEngine.Sink().OutputLineFormattedL( KMemSpyMarkerHeapData, &KNullDesC, (TUint) aTid );

    // Set overall prefix
    iEngine.Sink().OutputPrefixSetFormattedLC( KMemSpyPrefixHeapData, &aThreadName );

    // Info section
    OutputHeapInfoL( aInfo, aThreadName, aCells );

    // Code segments (needed for map file reading...)
    _LIT(KCellListCodeSegInfoFormat, "CodeSegs - ");
    iEngine.HelperCodeSegment().OutputCodeSegmentsL( aPid, printFormat, KCellListCodeSegInfoFormat, '-', ETrue );

    // Dump section
    _LIT(KHeaderDump, "Heap Data");
    iEngine.Sink().OutputSectionHeadingL( KHeaderDump, '-' );

    HBufC8* data = HBufC8::NewLC( 4096 * 12 );
    TPtr8 pData(data->Des());
    TUint remaining = 0;
    TUint readAddress = 0;

    // When we obtained the heap info, we also obtained a checksum of all the free cells
    // within the specified heap. We validate that this hasn't changed at the time we
    // request the heap data for paranoia purposes (There have been "Issues" with MemSpy
    // not actually suspending a process between fetching heap info & heap data, causing
    // a mismatch in free cell information).
    const TUint32 checksum = aInfo.AsRHeap().Statistics().StatsFree().Checksum();
    TRACE( RDebug::Printf( "CMemSpyEngineHelperHeap::OutputHeapDataUserL() - checksum: 0x%08x", checksum ) );

    TInt r = iEngine.Driver().GetHeapData( aTid, checksum, pData, readAddress, remaining );
	TUint prevEndAddress = readAddress + pData.Length();
    if (r == KErrNone)
        {
        while (r == KErrNone)
            {
			if (readAddress > prevEndAddress)
				{
				// We've hit a discontinuity, ie one or more unmapped pages
				_LIT(KBreak, "........");
				iEngine.Sink().OutputLineL(KBreak);
				}
            _LIT(KHeapDumpDataFormat, "%S");
            iEngine.Sink().OutputBinaryDataL(KHeapDumpDataFormat, pData.Ptr(), (const TUint8*) readAddress, pData.Length());
			readAddress += pData.Length();
            if (remaining > 0)
				{
				prevEndAddress = readAddress;
                r = iEngine.Driver().GetHeapDataNext(aTid, pData, readAddress, remaining);
				}
            else
                break;
            }
        }
    else
        {
        _LIT( KHeapFetchError, "Heap error: %d");
        iEngine.Sink().OutputLineFormattedL( KHeapFetchError, r );
        }

    CleanupStack::PopAndDestroy( data );

    CleanupStack::PopAndDestroy(); // clear prefix
    CleanupStack::PopAndDestroy(); // resume process

    // End marker
    iEngine.Sink().OutputLineFormattedL( KMemSpyMarkerHeapData, &KMemSpySinkTagClose, (TUint) aTid );

    if  ( aCreateDataStream )
        {
        iEngine.Sink().DataStreamEndL();
        }
    }



















EXPORT_C void CMemSpyEngineHelperHeap::OutputHeapInfoL(const TMemSpyHeapInfo& aInfo, 
                                                       const TDesC& aThreadName, 
                                                       const RArray<TMemSpyDriverCell>* aCells )
	{
    CMemSpyEngineOutputList* list = NewHeapSummaryExtendedLC(aInfo, aCells);

    // Format the thread name according to upper/lower case request parameters
    _LIT( KOverallCaption1, "HEAP INFO FOR THREAD '%S'");
    list->InsertItemFormatUCL( 0, KOverallCaption1, &aThreadName );
    list->InsertUnderlineForItemAtL( 0 );

    // Print it
    list->PrintL();

    // Tidy up
    CleanupStack::PopAndDestroy( list );
    }






























void CMemSpyEngineHelperHeap::OutputCSVEntryL(TInt aIndex, 
                                              const TMemSpyHeapInfo& aInfo, 
                                              const TDesC& aThreadName, 
                                              const TDesC& aProcessName )
    {
    const TMemSpyHeapInfoRHeap& rHeapInfo = aInfo.AsRHeap();
    const TMemSpyHeapMetaDataRHeap& rHeapMetaData = rHeapInfo.MetaData();
    const TMemSpyHeapStatisticsRHeap& rHeapStats = rHeapInfo.Statistics();

    // Example:
    //
    //  <ENTRY_001>
    //      <THREAD_NAME_001>ESock_IP</THREAD_NAME_001>
    //      <PROCESS_NAME_001>c32exe.exe[101f7989]0001</PROCESS_NAME_001>
    //      <CHUNK_NAME_001>Local-c812ba58</CHUNK_NAME_001>
    //      <FIELDS_001>1,0x12400000,0x00c00074,36744,4092,524288,0x00c011a4,0,13,6872,1368,1680,2584,219,40,28996,0</FIELDS_001>
    //  </ENTRY_001>
    
    _LIT( KFmtTagOpenAndClose, "<%S_%04d>%S</%S_%04d>" );
    
    _LIT( KFmtEntryId, "<%SENTRY_%04d>");
    _LIT( KFmtNameThread, "THREAD_NAME");
    _LIT( KFmtNameProcess, "PROCESS_NAME");
    _LIT( KFmtNameChunk, "CHUNK_NAME");
    _LIT( KFmtFields, "FIELDS");
    _LIT( KFmtFieldContent , "<%S_%04d>%06d,0x%08x,0x%08x,%d,%d,%d,0x%08x,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d</%S_%04d>");

    // <ENTRY_001>
    iEngine.Sink().OutputLineFormattedL( KFmtEntryId, &KNullDesC, aIndex );
    
    //      <THREAD_NAME_001>ESock_IP</THREAD_NAME_001>
    iEngine.Sink().OutputLineFormattedL( KFmtTagOpenAndClose, &KFmtNameThread, aIndex, &aThreadName, &KFmtNameThread, aIndex );
    
    //      <PROCESS_NAME_001>c32exe.exe[101f7989]0001</PROCESS_NAME_001>
    iEngine.Sink().OutputLineFormattedL( KFmtTagOpenAndClose, &KFmtNameProcess, aIndex, &aProcessName, &KFmtNameProcess, aIndex );
    
    //      <CHUNK_NAME_001>Local-c812ba58</CHUNK_NAME_001>
    const TPtrC pChunkName( rHeapMetaData.ChunkName() );
    iEngine.Sink().OutputLineFormattedL( KFmtTagOpenAndClose, &KFmtNameChunk, aIndex, &pChunkName, &KFmtNameChunk, aIndex );
    
    // Fields
    iEngine.Sink().OutputLineFormattedL( KFmtFieldContent,
                                         &KFmtFields,
                                         aIndex,
                                         aInfo.Tid(),
                                         rHeapMetaData.ChunkHandle(),
                                         rHeapMetaData.iAllocatorAddress,
                                         rHeapMetaData.iHeapSize,
                                         rHeapMetaData.iMinHeapSize,
                                         rHeapMetaData.iMaxHeapSize,
                                         NULL, // there's no longer a free list so we can't return the next ptr
                                         0, // there's no longer a free list so we can't return it's length
                                         rHeapStats.StatsFree().TypeCount(),
                                         rHeapStats.StatsFree().TypeSize(),
                                         rHeapStats.StatsFree().SlackSpaceCellSize(),
                                         rHeapStats.StatsFree().LargestCellSize(),
                                         rHeapStats.StatsAllocated().LargestCellSize(),
                                         rHeapStats.StatsAllocated().TypeCount(),
                                         0, // min cell no longer makes sense
                                         rHeapStats.StatsAllocated().TypeSize(),
                                         rHeapMetaData.IsSharedHeap(),
                                         &KFmtFields,
                                         aIndex
                                         );
    
    // </ENTRY_001>
    iEngine.Sink().OutputLineFormattedL( KFmtEntryId, &KMemSpySinkTagClose, aIndex );
    }



EXPORT_C void CMemSpyEngineHelperHeap::OutputHeapInfoForDeviceL( TBool aIncludeKernel )
    {
    // NB: The goal here is to minimise the line length. We already look like we
    // could exceed the available RDebug::Print length...
    const TInt count = iEngine.Container().Count();
    TInt index = 0;
    //
    HBufC* buf = HBufC::NewLC( 1024 );
    TPtr pBuf(buf->Des());
    //
    _LIT( KMemSpyFolder, "Heap\\Compact" );
    _LIT( KMemSpyContext, "Heap Compact" );
    _LIT( KMemSpyExtension, ".log" );
    iEngine.Sink().DataStreamBeginL( KMemSpyContext, KMemSpyFolder, KMemSpyExtension );

    // Start marker
    iEngine.Sink().OutputLineFormattedL( KMemSpyMarkerCSV, &KNullDesC );

    // Set overall prefix
    iEngine.Sink().OutputPrefixSetLC( KMemSpyPrefixCSV );
    
    // Output version info
    _LIT( KVersionNumber, "<VERSION>1</VERSION>" );
    iEngine.Sink().OutputLineL( KVersionNumber );
    
    // Output time stamp
    _LIT( KTimeStamp, "<TIMESTAMP>%u</TIMESTAMP>" );
    iEngine.Sink().OutputLineFormattedL( KTimeStamp, User::FastCounter() );
 
    // Heap info we'll populate by calling the driver
    TMemSpyHeapInfo info;
    TFullName processName;

    if  ( aIncludeKernel )
        {
        // Get kernel heap info
        GetHeapInfoKernelL( info );

        if ( info.Type() != TMemSpyHeapInfo::ETypeUnknown )
            {
            TName threadName;
            MemSpyEngineUtils::GetKernelHeapThreadAndProcessNames( threadName, processName );
            OutputCSVEntryL( index++, info, threadName, processName );
            }
        }

	for(TInt ii=0; ii<count; ii++)
        {
        const CMemSpyProcess& process = iEngine.Container().At( ii );
        process.FullName( processName );
        //
        if  ( iEngine.ProcessSuspendAndGetErrorLC( process.Id() ) == KErrNone )
            {
            const TInt threadCount = process.Count();
            //
            for(TInt j=0; j<threadCount; j++)
                {
                const CMemSpyThread& thread = process.At( j );
                const TPtrC threadName( thread.Name() );
                //
                const TInt error = iEngine.Driver().GetHeapInfoUser( info, thread.Id() );
                if ( error == KErrNone )
                    {
                    UpdateSharedHeapInfoL( process.Id(), thread.Id(), info );
                    }
                if  ( error == KErrNone && info.Type() != TMemSpyHeapInfo::ETypeUnknown )
                    {
                    OutputCSVEntryL( index++, info, threadName, processName );
                    }
                }
    
            CleanupStack::PopAndDestroy(); // ProcessSuspendLC
            }
        }

    CleanupStack::PopAndDestroy(); // clear prefix
    CleanupStack::PopAndDestroy( buf );

	// End marker
	iEngine.Sink().OutputLineFormattedL( KMemSpyMarkerCSV, &KMemSpySinkTagClose );
    iEngine.Sink().DataStreamEndL();
    }














EXPORT_C void CMemSpyEngineHelperHeap::GetHeapInfoUserL(const TProcessId& aProcess, 
                                                        const TThreadId& aThread, 
                                                        TMemSpyHeapInfo& aInfo, 
                                                        RArray<TMemSpyDriverFreeCell>* aFreeCells)
    {
	GetHeapInfoUserL(aProcess, aThread, aInfo, aFreeCells, EFalse);
	}

EXPORT_C void CMemSpyEngineHelperHeap::GetHeapInfoUserL(const TProcessId& aProcess, 
                                                        const TThreadId& aThread, 
                                                        TMemSpyHeapInfo& aInfo, 
                                                        RArray<TMemSpyDriverCell>* aCells, 
                                                        TBool aCollectAllocatedCellsAsWellAsFree)
    {
    iEngine.ProcessSuspendLC( aProcess );
    TRACE( RDebug::Printf( "CMemSpyEngineHelperHeap::GetHeapInfoUserL() - checksum1: 0x%08x", aInfo.AsRHeap().Statistics().StatsFree().Checksum() ) );
    
    TInt r = KErrNone;
    //
    if  (aCells)
        {
        r = iEngine.Driver().GetHeapInfoUser( aInfo, aThread, *aCells, aCollectAllocatedCellsAsWellAsFree);
        }
    else
        {
        r = iEngine.Driver().GetHeapInfoUser( aInfo, aThread );
        }
    if ( !r )
        {
        UpdateSharedHeapInfoL( aProcess, aThread, aInfo );
        }
    
    //
    TRACE( RDebug::Printf( "CMemSpyEngineHelperHeap::GetHeapInfoUserL() - checksum2: 0x%08x", aInfo.AsRHeap().Statistics().StatsFree().Checksum() ) );
    CleanupStack::PopAndDestroy(); // ProcessSuspendLC
    User::LeaveIfError( r );
    }


EXPORT_C void CMemSpyEngineHelperHeap::GetHeapInfoUserL(const CMemSpyProcess& aProcess, 
                                                        RArray<TMemSpyHeapInfo >& aInfos)
    {
    aInfos.Reset();
    iEngine.ProcessSuspendLC( aProcess.Id() );
    //
    TMemSpyHeapInfo info;
    //
    const TInt count = aProcess.Count();
    for( TInt i=0; i<count; i++ )
        {
        const CMemSpyThread& thread = aProcess.At( i );
        //
        GetHeapInfoUserL( aProcess.Id(), thread.Id(), info );
        aInfos.AppendL( info );
        }
    //
    CleanupStack::PopAndDestroy(); // ProcessSuspendLC
    }


EXPORT_C void CMemSpyEngineHelperHeap::OutputHeapInfoUserL( const CMemSpyThread& aThread )
    {
    HBufC* threadName = aThread.FullName().AllocLC();
    //
    RArray<TMemSpyDriverFreeCell> freeCells;
    CleanupClosePushL( freeCells );
    //
    TMemSpyHeapInfo info;
    GetHeapInfoUserL( aThread.Process().Id(), aThread.Id(), info, &freeCells );
    OutputHeapInfoL( info, *threadName, &freeCells );
    //
    CleanupStack::PopAndDestroy( 2, threadName ); // freecells & thread name
    }
















EXPORT_C void CMemSpyEngineHelperHeap::GetHeapInfoKernelL( TMemSpyHeapInfo& aInfo, 
                                                           RArray<TMemSpyDriverFreeCell>* aFreeCells )
    {
    GetHeapInfoKernelL(aInfo, aFreeCells, EFalse, EFalse);
    }

EXPORT_C void CMemSpyEngineHelperHeap::GetHeapInfoKernelL( TMemSpyHeapInfo& aInfo, 
                                                           RArray<TMemSpyDriverFreeCell>* aFreeCells, 
                                                           TBool aCollectAllocatedCellsAsWellAsFree )
    {
    GetHeapInfoKernelL(aInfo, aFreeCells, aCollectAllocatedCellsAsWellAsFree, EFalse);
    }

void CMemSpyEngineHelperHeap::GetHeapInfoKernelL( TMemSpyHeapInfo& aInfo, 
                                                  RArray<TMemSpyDriverFreeCell>* aFreeCells, 
                                                  TBool aCollectAllocatedCellsAsWellAsFree, 
                                                  TBool aUseKernelCopy )
    {
    TInt error = KErrNone;
    //
    if  ( aFreeCells )
        {
        error = iEngine.Driver().GetHeapInfoKernel( aInfo, 
                                                    *aFreeCells, 
                                                    aCollectAllocatedCellsAsWellAsFree, 
                                                    aUseKernelCopy );
        }
    else
        {
        error = iEngine.Driver().GetHeapInfoKernel( aInfo );
        }
    //
    User::LeaveIfError( error );
    }


EXPORT_C void CMemSpyEngineHelperHeap::OutputHeapInfoKernelL()
    {
    // Get thread name
    TFullName threadName;
    MemSpyEngineUtils::GetKernelHeapThreadName( threadName );

    // Free cells
    RArray<TMemSpyDriverFreeCell> freeCells;
    CleanupClosePushL( freeCells );

    // Get info
    TMemSpyHeapInfo info;
    GetHeapInfoKernelL( info, &freeCells );

    // Ouput
    OutputHeapInfoL( info, threadName, &freeCells );

    CleanupStack::PopAndDestroy( &freeCells );
    }


EXPORT_C void CMemSpyEngineHelperHeap::OutputHeapDataKernelL()
    {
    OutputHeapDataKernelL( KMemSpyHeapDumpCreateOwnDataStream );
    }


void CMemSpyEngineHelperHeap::CleanupHeapDataKernel( TAny* aPtr )
    {
    if ( aPtr )
        {
        CMemSpyEngineHelperHeap* ptr = static_cast<CMemSpyEngineHelperHeap*>( aPtr );
        (void) ptr->iEngine.Driver().FreeHeapDataKernel();
        }
    }

void CMemSpyEngineHelperHeap::OutputHeapDataKernelL( TBool aCreateDataStream )
    {
    User::LeaveIfError(iEngine.Driver().CopyHeapDataKernel());
    TCleanupItem item = TCleanupItem(CleanupHeapDataKernel, this);
    CleanupStack::PushL(item);
    
    // Get the heap info, including cell information
    RArray<TMemSpyDriverCell> cells;
    CleanupClosePushL( cells );
    
    TMemSpyHeapInfo heapInfo;
    TRACE( RDebug::Printf( "CMemSpyEngineHelperHeap::OutputHeapDataKernelL() - checksum before: 0x%08x", 
                           heapInfo.AsRHeap().Statistics().StatsFree().Checksum() ) );
    GetHeapInfoKernelL(heapInfo, &cells, ETrue, ETrue);
    TRACE( RDebug::Printf( "CMemSpyEngineHelperHeap::OutputHeapDataKernelL() - checksum after: 0x%08x", 
                           heapInfo.AsRHeap().Statistics().StatsFree().Checksum() ) );

    // Output the heap data
    OutputHeapDataKernelL( heapInfo, aCreateDataStream, &cells );
    
    CleanupStack::PopAndDestroy( &cells );
    CleanupStack::PopAndDestroy( this ); // runs CleanupHeapDataKernel
    }    

void CMemSpyEngineHelperHeap::OutputHeapDataKernelL(const TMemSpyHeapInfo& aHeapInfo, 
                                                    TBool aCreateDataStream, 
                                                    const RArray<TMemSpyDriverCell>* aCells)
    {
    TFullName threadName;
    MemSpyEngineUtils::GetKernelHeapThreadName( threadName, EFalse );

    // Begin a new data stream
    if  ( aCreateDataStream )
        {
        _LIT( KMemSpyFolder, "Heap\\Data" );
        _LIT( KMemSpyContext, "Heap Data - %S" );
        HBufC* context = HBufC::NewLC( KMaxFileName );
        TPtr pContext( context->Des() );
        pContext.Format( KMemSpyContext, &threadName );
        iEngine.Sink().DataStreamBeginL( pContext, KMemSpyFolder );
        CleanupStack::PopAndDestroy( context );
        }

    TBuf<KMaxFullName + 100> printFormat;

    // Start marker
    iEngine.Sink().OutputLineFormattedL( KMemSpyMarkerHeapData, &KNullDesC, aHeapInfo.Tid() );

    // Set overall prefix
    iEngine.Sink().OutputPrefixSetFormattedLC( KMemSpyPrefixHeapData, &threadName );

    // Info section
    OutputHeapInfoL( aHeapInfo, threadName, aCells );

    // Code segments (needed for map file reading...)
    _LIT(KCellListCodeSegInfoFormat, "CodeSegs - ");
    iEngine.HelperCodeSegment().OutputCodeSegmentsL( aHeapInfo.Pid(), printFormat, KCellListCodeSegInfoFormat, '-', ETrue );

    // Dump section
    _LIT(KHeaderDump, "Heap Data");
    iEngine.Sink().OutputSectionHeadingL( KHeaderDump, '-' );

    HBufC8* data = HBufC8::NewLC( 4096 * 12 );
    TPtr8 pData(data->Des());
    TUint remaining = 0;
    TUint readAddress = 0;

    TInt r = iEngine.Driver().GetHeapDataKernel(aHeapInfo.Tid(), pData, readAddress, remaining);
    TUint prevEndAddress = readAddress + pData.Length();
    if (r == KErrNone)
        {
        while (r == KErrNone)
            {
            if (readAddress > prevEndAddress)
                {
                // We've hit a discontinuity, ie one or more unmapped pages
                _LIT(KBreak, "........");
                iEngine.Sink().OutputLineL(KBreak);
                }
            _LIT(KHeapDumpDataFormat, "%S");
            iEngine.Sink().OutputBinaryDataL(KHeapDumpDataFormat, pData.Ptr(), (const TUint8*) readAddress, pData.Length());
            readAddress += pData.Length();
            if (remaining > 0)
                {
                prevEndAddress = readAddress;
                r = iEngine.Driver().GetHeapDataKernelNext(aHeapInfo.Tid(), pData, readAddress, remaining);
                }
            else
                break;
            }
        }
    else
        {
        _LIT( KHeapFetchError, "Heap error: %d");
        iEngine.Sink().OutputLineFormattedL( KHeapFetchError, r );
        }

    CleanupStack::PopAndDestroy( data );

    CleanupStack::PopAndDestroy(); // clear prefix

    // End marker
    iEngine.Sink().OutputLineFormattedL( KMemSpyMarkerHeapData, &KMemSpySinkTagClose, aHeapInfo.Tid() );

    if  ( aCreateDataStream )
        {
        iEngine.Sink().DataStreamEndL();
        }
    }
































EXPORT_C CMemSpyEngineOutputList* CMemSpyEngineHelperHeap::NewHeapSummaryShortLC( const TMemSpyHeapInfo& aInfo )
    {
    CMemSpyEngineOutputList* list = CMemSpyEngineOutputList::NewLC( iEngine.Sink() );

    // Heap type
    _LIT( KItem0, "Heap type" );
    if ( aInfo.Type() == TMemSpyHeapInfo::ETypeUnknown )
        {
        _LIT( KItem0_Type_Unknown, "Unknown" );
        list->AddItemL( KItem0, KItem0_Type_Unknown );
        }
    else
        {
        const TMemSpyHeapInfoRHeap& rHeap = aInfo.AsRHeap();
        const TMemSpyHeapMetaDataRHeap& metaData = rHeap.MetaData();
        const TMemSpyHeapStatisticsRHeap& statistics = rHeap.Statistics();

        _LIT( KItem0_Type_RHeap, "RHeap" );
        _LIT( KItem0_Type_RHybridHeap, "RHybridHeap" );
		if (aInfo.Type() == TMemSpyHeapInfo::ETypeRHeap)
			{
	        list->AddItemL( KItem0, KItem0_Type_RHeap );
			}
		else
			{
	        list->AddItemL( KItem0, KItem0_Type_RHybridHeap );
			}

        // Heap size is the size of the heap minus the size of the embedded (in-place) RHeap. 
        _LIT( KItem1, "Heap size" );
        list->AddItemL(KItem1, metaData.iHeapSize);

        _LIT( KItem8b, "Allocator address" );
        list->AddItemHexL( KItem8b, (TUint)metaData.iAllocatorAddress );
        
        _LIT( KItem1b, "Shared" );
        list->AddItemYesNoL( KItem1b, metaData.IsSharedHeap() );

        // This is the size (rounded to the page) of memory associated with
        // the underlying heap chunk
        _LIT( KItem2, "Chunk size" );
        list->AddItemL( KItem2, metaData.ChunkSize() );

        _LIT( KItem3, "Alloc. count" );
        list->AddItemL( KItem3, statistics.StatsAllocated().TypeCount() );

        _LIT( KItem4, "Free. count" );
        list->AddItemL( KItem4, statistics.StatsFree().TypeCount() );

        _LIT( KItem5, "Biggest alloc." );
        list->AddItemL( KItem5, statistics.StatsAllocated().LargestCellSize() );

        _LIT( KItem6, "Biggest free" );
        list->AddItemL( KItem6, statistics.StatsFree().LargestCellSize() );

        _LIT( KItem6a, "Total alloc." );
        list->AddItemL( KItem6a, statistics.StatsAllocated().TypeSize() );

        _LIT( KItem6b, "Total free" );
        list->AddItemL( KItem6b, statistics.StatsFree().TypeSize() );

        // Slack is the free space at the end of the heap
        _LIT( KItem7, "Slack free space" );
        list->AddItemL( KItem7, statistics.StatsFree().SlackSpaceCellSize() );

        // Fragmentation is a measurement of free space scattered throughout the heap, but ignoring
        // any slack space at the end (which can often be recovered, to the granularity of one page of ram)
        _LIT( KItem8a, "Fragmentation" );
        list->AddItemPercentageL( KItem8a, metaData.iHeapSize, ( statistics.StatsFree().TypeSize()  - statistics.StatsFree().SlackSpaceCellSize() ) );


        _LIT( KItem9c, "Overhead (total)" );
		const TInt totalOverhead = metaData.iHeapSize - statistics.StatsAllocated().TypeSize();
        list->AddItemL( KItem9c, totalOverhead );

        _LIT( KItem9d, "Overhead" );
        list->AddItemPercentageL( KItem9d, metaData.iHeapSize, totalOverhead  );

        _LIT( KItem10, "Min. length" );
        list->AddItemL( KItem10, metaData.iMinHeapSize );

        _LIT( KItem11, "Max. length" );
        list->AddItemL( KItem11, metaData.iMaxHeapSize );

        _LIT( KItem12, "Debug Allocator Library" );
        list->AddItemYesNoL( KItem12, metaData.IsDebugAllocator() );
        }

    return list;
    }


EXPORT_C CMemSpyEngineOutputList* CMemSpyEngineHelperHeap::NewHeapSummaryExtendedLC( const TMemSpyHeapInfo& aInfo, 
                                                                                     const RArray<TMemSpyDriverCell>* aCells )
	{
    CMemSpyEngineOutputList* list = CMemSpyEngineOutputList::NewLC( iEngine.Sink() );
    //
    AppendMetaDataL( aInfo, *list );
    AppendStatisticsL( aInfo, *list );
    //
    if  ( aCells )
        {
        AppendCellsL( *aCells, *list );
        }
    //
    return list;
    }


//cigasto: not formatted - raw heap info 
EXPORT_C TMemSpyHeapData CMemSpyEngineHelperHeap::NewHeapRawInfo( const TMemSpyHeapInfo& aInfo )
	{
	_LIT(KUnknown, "Unknown");
	TMemSpyHeapData list;
	list.iType.Copy(KUnknown);

	// Heap type	
	if (aInfo.Type() != TMemSpyHeapInfo::ETypeUnknown)
		{
		const TMemSpyHeapInfoRHeap& rHeap = aInfo.AsRHeap();
		const TMemSpyHeapMetaDataRHeap& metaData = rHeap.MetaData();
		const TMemSpyHeapStatisticsRHeap& statistics = rHeap.Statistics();

		_LIT(KRHeap, "RHeap");
		_LIT(KRHybridHeap, "RHybridHeap");
		switch (aInfo.Type())
			{
			case TMemSpyHeapInfo::ETypeRHeap:
				list.iType.Copy(KRHeap);
				break;
			case TMemSpyHeapInfo::ETypeRHybridHeap:
				list.iType.Copy(KRHybridHeap);
				break;
			default:
				break;
			}

	    // Heap size is the total amount of memory committed to the heap, which includes the size of the embedded (in-place) RHeap/RHybridHeap.
	    list.iSize = metaData.iHeapSize;
	    list.iBaseAddress = (TUint)metaData.iAllocatorAddress; // TODO we can't do the base address any more, allocator address is the closest thing
	    list.iShared = metaData.IsSharedHeap();
	    list.iChunkSize = metaData.ChunkSize();
	    list.iAllocationsCount = statistics.StatsAllocated().TypeCount();
	    list.iFreeCount = statistics.StatsFree().TypeCount();
	    list.iBiggestAllocation = statistics.StatsAllocated().LargestCellSize();
	    list.iBiggestFree = statistics.StatsFree().LargestCellSize();
	    list.iTotalAllocations =  statistics.StatsAllocated().TypeSize();	        
	    list.iTotalFree =  statistics.StatsFree().TypeSize();
	    list.iSlackFreeSpace = statistics.StatsFree().SlackSpaceCellSize();
	    list.iFragmentation = statistics.StatsFree().TypeSize() - statistics.StatsFree().SlackSpaceCellSize(); //to calculate percentage value use iSize as 100% value
	    list.iHeaderSizeA = 0; //metaData.HeaderSizeAllocated();
	    list.iHeaderSizeF = 0; //metaData.HeaderSizeFree();
	    TInt allocOverhead = rHeap.Overhead(); //metaData.HeaderSizeAllocated() * statistics.StatsAllocated().TypeCount();
	    list.iAllocationOverhead = allocOverhead;
	    //TInt freeOverhead = metaData.HeaderSizeFree() * statistics.StatsFree().TypeCount();
	    list.iFreeOverhead = 0; // TODO there is no way of calculating this
	    list.iTotalOverhead = allocOverhead; // freeOverhead + allocOverhead
	    list.iOverhead = allocOverhead; //freeOverhead + allocOverhead; //to calculate percentage value use iSize as 100% value    
	    list.iMinLength = metaData.iMinHeapSize;
	    list.iMaxLength = metaData.iMaxHeapSize;
	    list.iDebugAllocatorLibrary = metaData.IsDebugAllocator();
		}

	return list;
	}































TUint CMemSpyEngineHelperHeap::DescriptorAsDWORD( const TDesC8& aItem)
    {
    __ASSERT_ALWAYS( aItem.Length() >= 4, User::Invariant() );
    const TUint ret =  aItem[0] +
                      (aItem[1] << 8) + 
                      (aItem[2] << 16) + 
                      (aItem[3] << 24);
    return ret;
    }


void CMemSpyEngineHelperHeap::AppendMetaDataL( const TMemSpyHeapInfo& aInfo, CMemSpyEngineOutputList& aList )
    {
    const TMemSpyHeapInfoRHeap& rHeap = aInfo.AsRHeap();

    // Make caption
    _LIT( KOverallCaption1, "Meta Data" );
    aList.AddItemL( KOverallCaption1 );
    aList.AddUnderlineForPreviousItemL( '=', 0 );

    // Type
    _LIT( KMetaData_Type,  "Type:" );
    if ( aInfo.Type() == TMemSpyHeapInfo::ETypeUnknown )
        {
        _LIT( KMetaData_Type_Unknown,  "Unknown" );
        aList.AddItemL( KMetaData_Type, KMetaData_Type_Unknown );
        }
    else
        {
        const TMemSpyHeapMetaDataRHeap& metaData = rHeap.MetaData();
    
        // Type
        _LIT( KMetaData_Type_RHeap,  "Symbian OS RHeap" );
        _LIT( KMetaData_Type_RHybridHeap,  "Symbian OS RHybridHeap" );
		if (aInfo.Type() == TMemSpyHeapInfo::ETypeRHeap)
			{
	        aList.AddItemL( KMetaData_Type, KMetaData_Type_RHeap );
			}
		else
			{
			aList.AddItemL( KMetaData_Type, KMetaData_Type_RHybridHeap );
			}

        // VTable
        //_LIT( KMetaData_VTable,  "VTable:" );
        //aList.AddItemHexL( KMetaData_VTable, metaData.VTable() );

        // Object size
        //_LIT( KMetaData_ObjectSize,  "Object Size:" );
        //aList.AddItemL( KMetaData_ObjectSize, metaData.ClassSize() );

        // Chunk name
        _LIT( KMetaData_ChunkName,  "Chunk Name:" );
        TPtrC pChunkName( metaData.ChunkName() );
        aList.AddItemL( KMetaData_ChunkName, pChunkName );

        // Chunk size
        _LIT( KMetaData_ChunkSize,  "Chunk Size:" );
        aList.AddItemL( KMetaData_ChunkSize, metaData.ChunkSize() );

        // Chunk base address
        _LIT( KMetaData_ChunkBaseAddress,  "Chunk Base Address:" );
        aList.AddItemL( KMetaData_ChunkBaseAddress, metaData.ChunkBaseAddress() );

        // Debug allocator
        _LIT( KMetaData_DebugAllocator,  "Debug Allocator:" );
        aList.AddItemYesNoL( KMetaData_DebugAllocator, metaData.IsDebugAllocator() );

        // Shared Heap
        _LIT( KMetaData_Shared,  "Shared:" );
        aList.AddItemYesNoL( KMetaData_Shared, metaData.IsSharedHeap() );

        // Add ROM info
        iEngine.HelperROM().AddInfoL( aList );
        }

    aList.AddBlankItemL( 1 );
    }

void CMemSpyEngineHelperHeap::AppendStatisticsL( const TMemSpyHeapInfo& aInfo, CMemSpyEngineOutputList& aList )
    {
    if (aInfo.Type() != TMemSpyHeapInfo::ETypeUnknown)
        {
        const TMemSpyHeapInfoRHeap& rHeap = aInfo.AsRHeap();
        const TMemSpyHeapStatisticsRHeap& rHeapStats = rHeap.Statistics();

        // Shared captions
        _LIT( KStatsData_CellCount,  "Number of cells:" );
        _LIT( KStatsData_CellSize,  "Size of cells:" );
        _LIT( KStatsData_LargestCellAddress,  "Largest cell:" );
        _LIT( KStatsData_LargestCellSize,  "Largest cell size:" );

        // Free space
        _LIT( KOverallCaption1, "Free Cell Statistics" );
        aList.AddItemL( KOverallCaption1 );
        aList.AddUnderlineForPreviousItemL( '=', 0 );

        aList.AddItemL( KStatsData_CellCount, rHeapStats.StatsFree().TypeCount() );
        aList.AddItemL( KStatsData_CellSize, rHeapStats.StatsFree().TypeSize() );
        aList.AddItemL( KStatsData_LargestCellAddress, rHeapStats.StatsFree().LargestCellAddress() );
        aList.AddItemL( KStatsData_LargestCellSize, rHeapStats.StatsFree().LargestCellSize() );
		if (aInfo.Type() == TMemSpyHeapInfo::ETypeRHeap)
			{
			_LIT( KStatsData_Free_SlackCellAddress,  "Slack:" );
			aList.AddItemL( KStatsData_Free_SlackCellAddress, rHeapStats.StatsFree().SlackSpaceCellAddress() );
			_LIT( KStatsData_Free_SlackCellSize,  "Slack size:" );
			aList.AddItemL( KStatsData_Free_SlackCellSize, rHeapStats.StatsFree().SlackSpaceCellSize() );
			}
        _LIT( KStatsData_Free_Checksum,  "Checksum:" );
        aList.AddItemHexL( KStatsData_Free_Checksum, rHeapStats.StatsFree().Checksum() );

        aList.AddBlankItemL( 1 );

        // Allocated space
        _LIT( KOverallCaption2, "Allocated Cell Statistics" );
        aList.AddItemL( KOverallCaption2 );
        aList.AddUnderlineForPreviousItemL( '=', 0 );

        aList.AddItemL( KStatsData_CellCount, rHeapStats.StatsAllocated().TypeCount() );
        aList.AddItemL( KStatsData_CellSize, rHeapStats.StatsAllocated().TypeSize() );
        aList.AddItemL( KStatsData_LargestCellAddress, rHeapStats.StatsAllocated().LargestCellAddress() );
        aList.AddItemL( KStatsData_LargestCellSize, rHeapStats.StatsAllocated().LargestCellSize() );

        aList.AddBlankItemL( 1 );
         }
    }


void CMemSpyEngineHelperHeap::AppendCellsL(const RArray<TMemSpyDriverCell>& aCells, CMemSpyEngineOutputList& aList)
    {
    // For reasons that may or may not turn out to be sensible, we separate free and allocated cells in the output data

    _LIT( KOverallCaption1, "Free Cell List" );
    aList.AddItemL( KOverallCaption1 );
    aList.AddUnderlineForPreviousItemL( '=', 0 );

    TBuf<128> caption;
    _LIT( KCaptionFormat, "FC %04d" );
    _LIT( KValueFormat, "0x%08x %8d %d" );

	TBool foundAllocatedCells = EFalse;
    const TInt count = aCells.Count();
    for( TInt i=0; i<count; i++ )
        {
        const TMemSpyDriverCell& cell = aCells[ i ];
		if (cell.iType & EMemSpyDriverAllocatedCellMask)
			{
			foundAllocatedCells = ETrue;
			}
		else if (cell.iType & EMemSpyDriverFreeCellMask)
			{
	        caption.Format( KCaptionFormat, i + 1 );
		    aList.AddItemFormatL( caption, KValueFormat, cell.iAddress, cell.iLength, cell.iType );
			}
        }

	if (foundAllocatedCells)
		{
        aList.AddBlankItemL( 1 );
		_LIT( KOverallCaption1, "Allocated Cell List" );
		aList.AddItemL( KOverallCaption1 );
		aList.AddUnderlineForPreviousItemL( '=', 0 );

		TBuf<128> caption;
		_LIT( KCaptionFormat, "AC %04d" );
		_LIT( KValueFormat, "0x%08x %8d %d" );

		for (TInt i = 0; i < count; i++)
			{
			const TMemSpyDriverCell& cell = aCells[ i ];
			if (cell.iType & EMemSpyDriverAllocatedCellMask)
				{
				caption.Format( KCaptionFormat, i + 1 );
				aList.AddItemFormatL( caption, KValueFormat, cell.iAddress, cell.iLength, cell.iType );
				}
			}
		}
    }

void CMemSpyEngineHelperHeap::UpdateSharedHeapInfoL( const TProcessId& aProcess, 
                                                     const TThreadId& aThread, 
                                                     TMemSpyHeapInfo& aInfo )
    {
    RArray<TThreadId> threads;
    CleanupClosePushL( threads );
    iEngine.Driver().GetThreadsL( aProcess, threads );
    TMemSpyHeapInfo otherHeap;
    TThreadId otherThreadId;
    TInt r( KErrNone );
    for ( TInt i = 0; i < threads.Count(); i++ )
        {
        otherThreadId = threads[i];
        if ( aThread != otherThreadId ) // skip current thread
            {
            r = iEngine.Driver().GetHeapInfoUser( otherHeap, otherThreadId );
            if ( !r && otherHeap.AsRHeap().MetaData().ChunkHandle() == aInfo.AsRHeap().MetaData().ChunkHandle() )
                {
                TRACE( RDebug::Printf( "CMemSpyEngineHelperHeap::UpdateSharedHeapInfoL - shared heap detected chunkhandle: 0x%08x", aInfo.AsRHeap().MetaData().ChunkHandle() ) );
                aInfo.AsRHeap().MetaData().SetSharedHeap( ETrue );
                break;
                }
            }
        }
    CleanupStack::PopAndDestroy( &threads );
    }

