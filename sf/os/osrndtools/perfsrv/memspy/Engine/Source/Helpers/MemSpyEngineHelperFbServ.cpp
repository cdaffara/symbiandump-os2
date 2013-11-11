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

#include <memspy/engine/memspyenginehelperfbserv.h>

// System includes
#include <s32mem.h>
#include <f32file.h>
#include <e32rom.h>

// Driver includes
#include <memspy/driver/memspydriverclient.h>

// User includes
#include <memspy/engine/memspyengine.h>
#include <memspy/engine/memspyengineutils.h>
#include <memspy/engine/memspyengineoutputsink.h>
#include <memspy/engine/memspyengineobjectprocess.h>
#include <memspy/engine/memspyengineobjectthread.h>
#include <memspy/engine/memspyengineobjectcontainer.h>

// Literal constants
_LIT( KMemSpyEngineFBServComma, ", " );
_LIT( KMemSpyEngineFBServExportFileName, "Image_%08x_%02d_bpp_(%d x %d).bmp");
_LIT( KMemSpyEngineFBSLargeChunkName, "FbsLargeChunk" );
_LIT( KMemSpyEngineFBSSharedChunkName, "FbsSharedChunk" );


CMemSpyEngineHelperFbServ::CMemSpyEngineHelperFbServ( CMemSpyEngine& aEngine )
:   iEngine( aEngine )
    {
    }


CMemSpyEngineHelperFbServ::~CMemSpyEngineHelperFbServ()
    {
    iEngine.Driver().WalkHeapClose();
    }


void CMemSpyEngineHelperFbServ::ConstructL()
    {
    }


CMemSpyEngineHelperFbServ* CMemSpyEngineHelperFbServ::NewL( CMemSpyEngine& aEngine )
    {
    CMemSpyEngineHelperFbServ* self = new(ELeave) CMemSpyEngineHelperFbServ( aEngine );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


TBool CMemSpyEngineHelperFbServ::IsLargeBitmapChunk( const TMemSpyDriverChunkInfo& aChunkInfo )
    {
    TBool ret = EFalse;
    //
    if  ( aChunkInfo.iType == EMemSpyDriverChunkTypeGlobal )
        {
        ret = ( aChunkInfo.iName.FindF( KMemSpyEngineFBSLargeChunkName ) >= 0 );
        }
    //
    return ret;
    }


TBool CMemSpyEngineHelperFbServ::IsSharedBitmapChunk( const TMemSpyDriverChunkInfo& aChunkInfo )
    {
    TBool ret = EFalse;
    //
    if  ( aChunkInfo.iType == EMemSpyDriverChunkTypeGlobal )
        {
        ret = ( aChunkInfo.iName.FindF( KMemSpyEngineFBSSharedChunkName ) >= 0 );
        }
    //
    return ret;
    }


EXPORT_C void CMemSpyEngineHelperFbServ::GetArrayOfBitmapHandlesL( RArray<TInt>& aHandles )
    {
    TInt fontConCount = 0;
    TInt bitmapConCount =0;
    //RArray<TInt> dirtyHandles;
    //CleanupClosePushL( dirtyHandles );
    GetArrayOfBitmapHandlesL( aHandles, bitmapConCount, fontConCount );
    //CleanupStack::PopAndDestroy( &dirtyHandles );
    }


void CMemSpyEngineHelperFbServ::GetArrayOfBitmapHandlesL( RArray<TInt>& aHandles, TInt& aBitmapConCount, TInt& aFontConCount )
    {
    TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::GetArrayOfBitmapHandlesL() - START") );
    CMemSpyThread& fbServThread = IdentifyFbServThreadL();

    // Suspend the process
    TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::GetArrayOfBitmapHandlesL() - got FBServ thread, suspending it..."));
    iEngine.ProcessSuspendLC( fbServThread.Process().Id() );

    // Get the heap cell data for the correct object (CFbTop)
    TAny* cellAddress = NULL;
    HBufC8* cellData = LocateCFbTopHeapCellDataLC( fbServThread, cellAddress );
    TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::GetArrayOfBitmapHandlesL() - got CFbTop cell data: %d bytes", cellData->Length()));

    // Work out the offset to the bitmap address handles array
    const TUint offsetBitmapCon = OffsetToCObjectConBitmapCon();
    const TUint offsetFontCon = OffsetToCObjectConFontCon();
    const TUint offsetHandleArray = OffsetToBitmapHandleArray();
    TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::GetArrayOfBitmapHandlesL() - offsetBitmapCon:   %d", offsetBitmapCon ));
    TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::GetArrayOfBitmapHandlesL() - offsetFontCon:     %d", offsetFontCon ));
    TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::GetArrayOfBitmapHandlesL() - offsetHandleArray: %d", offsetHandleArray ));

    // Create stream for reading heap data (makes life a bit easier).
    RDesReadStream stream( *cellData );
    CleanupClosePushL( stream );
    
    // We need to skip over the preamble to the start of the actual data.
    TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::GetArrayOfBitmapHandlesL() - skipping %d bytes [to start of bitmap con]...", offsetBitmapCon ));
    (void) stream.ReadL( offsetBitmapCon ); // skip this much

    // Read CFbTop::iBitmapCon cell address
    TAny* cellAddresBitmapCon = reinterpret_cast<TAny*>( stream.ReadUint32L() );
    TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::GetArrayOfBitmapHandlesL() - cellAddresBitmapCon: 0x%08x", cellAddresBitmapCon ));
    TInt bitmapConAllocated = 0;
    RArray<TAny*> bitmapConAddresses;
    CleanupClosePushL( bitmapConAddresses );
    ReadCObjectConInfoL( cellAddresBitmapCon, bitmapConAddresses, aBitmapConCount, bitmapConAllocated );
    CleanupStack::PopAndDestroy( &bitmapConAddresses );

    // Read CFbTop::iFontCon cell address
    TAny* cellAddresFontCon = reinterpret_cast<TAny*>( stream.ReadUint32L() );
    TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::GetArrayOfBitmapHandlesL() - cellAddresFontCon: 0x%08x", cellAddresFontCon ));
    TInt fontConAllocated = 0;
    RArray<TAny*> fontConAddresses;
    CleanupClosePushL( fontConAddresses );
    ReadCObjectConInfoL( cellAddresFontCon, fontConAddresses, aFontConCount, fontConAllocated );
    CleanupStack::PopAndDestroy( &fontConAddresses );

    // Skip forwards to the bitmap handle array info
    const TUint skipRemaining = ( offsetHandleArray - offsetFontCon );
    TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::GetArrayOfBitmapHandlesL() - skipRemaining: %d", skipRemaining ));
    (void) stream.ReadL( skipRemaining ); // skip this much

    // The layout of RPointerArrayBase is as follows:
    //
    //  TInt iCount;
	//  TAny** iEntries;
	//  TInt iAllocated;
	//  TInt iGranularity;
	//  TInt iSpare1;
	//  TInt iSpare2;

    // Now read the next 4 bytes. This is the number of allocated array entries.
    const TInt arrayEntryCount = stream.ReadInt32L();
    TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::GetArrayOfBitmapHandlesL() - arrayEntryCount: %d", arrayEntryCount ));

    // The next four bytes are the address of the cell we are interested in.
    cellAddress = reinterpret_cast<TAny*>( stream.ReadUint32L() );
    TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::GetArrayOfBitmapHandlesL() - cellAddress: 0x%08x", cellAddress ));
    
    // The next four bytes are the allocated count
    const TInt arrayAlloctedCount = stream.ReadInt32L();
    TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::GetArrayOfBitmapHandlesL() - arrayAlloctedCount: %d", arrayAlloctedCount ));
    
    // Validate
    if  ( arrayEntryCount > arrayAlloctedCount || arrayEntryCount < 0 || arrayAlloctedCount < 0 )
        {
        TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::GetArrayOfBitmapHandlesL() - invalid array details => Leave with KErrCorrupt"));
        User::Leave( KErrCorrupt );
        }
    
    // Clean up - don't need this data anymore. Real data is in another cell
    CleanupStack::PopAndDestroy( 2, cellData );  // stream & cellData

    // Now obtain the heap cell data for the actual array itself.
    TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::GetArrayOfBitmapHandlesL() - trying to find bitmap array data..."));
    cellData = LocateBitmapArrayHeapCellDataLC( cellAddress, arrayAlloctedCount );
    
    // Parse the cell data in order to obtain the bitmap handles.
    TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::GetArrayOfBitmapHandlesL() - trying to extract bitmap handles..."));
    ParseCellDataAndExtractHandlesL( *cellData, aHandles, arrayEntryCount );
    
    // Tidy up
    TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::GetArrayOfBitmapHandlesL() - resuming process..."));
    CleanupStack::PopAndDestroy( cellData );
 
    // Close heap walker & resume process
    iEngine.Driver().WalkHeapClose();
    CleanupStack::PopAndDestroy();
    TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::GetArrayOfBitmapHandlesL() - END"));
    }


void CMemSpyEngineHelperFbServ::GetBitmapInfoL( TInt aHandle, TMemSpyEngineFBServBitmapInfo& aInfo )
    {
    CFbsBitmap* bitmap = GetBitmapInfoLC( aHandle, aInfo );
    CleanupStack::PopAndDestroy( bitmap );
    }
 

CFbsBitmap* CMemSpyEngineHelperFbServ::GetBitmapInfoLC( TInt aHandle, TMemSpyEngineFBServBitmapInfo& aInfo )
    {
    CFbsBitmap* bitmap = new(ELeave) CFbsBitmap();
    CleanupStack::PushL( bitmap );
    const TInt error = bitmap->Duplicate( aHandle );
    User::LeaveIfError( error );

    // Basic info from bitmap itself
    aInfo.iHandle = aHandle;
    aInfo.iDisplayMode = bitmap->DisplayMode();

    // Extended info comes from header
    const SEpocBitmapHeader header( bitmap->Header() );
    
    aInfo.iSizeInPixels = header.iSizeInPixels;
    aInfo.iCompressionType = header.iCompression;
    aInfo.iColor = header.iColor;
    aInfo.iBitsPerPixel = header.iBitsPerPixel;
    aInfo.iSizeInBytes = header.iBitmapSize;

    // Flags
    aInfo.iFlags = TMemSpyEngineFBServBitmapInfo::EFlagsNone;
    if  ( bitmap->IsRomBitmap() )
        {
        aInfo.iFlags |= TMemSpyEngineFBServBitmapInfo::EFlagsIsRomBitmap;
        }
     if  ( bitmap->IsLargeBitmap() )
        {
        aInfo.iFlags |= TMemSpyEngineFBServBitmapInfo::EFlagsIsLarge;
        }
    if  ( bitmap->IsCompressedInRAM() )
        {
        aInfo.iFlags |= TMemSpyEngineFBServBitmapInfo::EFlagsIsCompressedInRam;
        }
    else
        {
        // Can only do this if not compressed
        if  ( bitmap->IsMonochrome() )
             {
             aInfo.iFlags |= TMemSpyEngineFBServBitmapInfo::EFlagsIsMonochrome;
             }
        }
 
    return bitmap;
    }


CMemSpyThread& CMemSpyEngineHelperFbServ::IdentifyFbServThreadL()
    {
    _LIT(KFbServThreadName, "FbServ::!Fontbitmapserver");
    //
    CMemSpyThread* thread = NULL;
    CMemSpyProcess* process = NULL;

    // Get the fbserv thread object
    CMemSpyEngineObjectContainer& container = iEngine.Container();
    User::LeaveIfError( container.ProcessAndThreadByFullName( KFbServThreadName, process, thread ) );
    //
    return *thread;
    }


HBufC8* CMemSpyEngineHelperFbServ::LocateCFbTopHeapCellDataLC( CMemSpyThread& aFbServThread, TAny*& aCellAddress )
    {
    TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::LocateCFbTopHeapCellDataLC() - START"));

    // We'll start looking for the CFbTop object at cell index 0, then we'll
    // continue to a maximum of cell index 10. These are not actual allocation
    // numbers, they are just cell indicies within FBServ's heap.
    // Realistically, the cell should be the 7th.
    const TInt KFbServWillingToStartAtCellIndex = 0;
    const TInt KFbServWillingToSearchUntilCellIndex = 10;

    // We know the size of the cell is pretty big, since it contains a large 
    // hash-map object (4096 elements by default, 4 bytes big => 16k). We'll
    // not try to retrieve more than 18k of data.
    const TInt KFbServExpectedMinimumCellSize = 14*1024;
    const TInt KFbServExpectedMaximumCellSize = 18*1024;

    // This is what we'll return, if we find it...
    aCellAddress = NULL;
    HBufC8* heapCellData = NULL;

    // Get the heap info - we need this for verification purposes
    TMemSpyHeapInfo info;
    TInt err = iEngine.Driver().GetHeapInfoUser( info, aFbServThread.Id() );
    if ( err == KErrNone && info.Type() == TMemSpyHeapInfo::ETypeUnknown )
        {
        err = KErrNotSupported;
        }
    User::LeaveIfError( err );

    // Now walk the heap!
    err = iEngine.Driver().WalkHeapInit( aFbServThread.Id() );
    if  ( err == KErrNone )
        {
        TMemSpyDriverCellType cellType;
        TAny* cellAddress;
        TInt cellLength;
        TInt cellNestingLevel;
        TInt cellAllocationNumber;
        TInt cellHeaderSize;
        TAny* cellPayloadAddress;
        //
        for( TInt cellIndex = KFbServWillingToStartAtCellIndex; err == KErrNone && cellIndex < KFbServWillingToSearchUntilCellIndex; cellIndex++ )
            {
            err = iEngine.Driver().WalkHeapNextCell( aFbServThread.Id(), cellType, cellAddress, cellLength, cellNestingLevel, cellAllocationNumber, cellHeaderSize, cellPayloadAddress );
            TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::LocateCFbTopHeapCellDataLC() - cellIndex[%d] err: %d, cellLength: %d, cellAllocationNumber: %d, cellType: %d", cellIndex, err, cellLength, cellAllocationNumber, cellType));

            if  ( err == KErrNone && (cellType & EMemSpyDriverAllocatedCellMask))
                {
                // We know we are looking for a relatively large *allocated* cell.
                if  ( cellLength >= KFbServExpectedMinimumCellSize && cellLength <= KFbServExpectedMaximumCellSize )
                    {
                    const TInt payloadLength = cellLength;

                    // This is *probably* the right cell. Let's get the data and check.
                    HBufC8* data = HBufC8::NewLC( payloadLength );
                    TPtr8 pData( data->Des() );
                    //
                    err = iEngine.Driver().WalkHeapReadCellData( cellAddress, pData, payloadLength );
                    TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::LocateCFbTopHeapCellDataLC() - data fetch returned error: %d", err));

                    if  ( err == KErrNone )
                        {
                        //_LIT(KHeapDumpDataFormat, "%S");
                        //iEngine.Sink().OutputBinaryDataL( KHeapDumpDataFormat, pData.Ptr(), (const TUint8*) cellAddress, pData.Length() );
                    
                        // Check the data
                        const TUint heapMaxSize = info.AsRHeap().MetaData().iMaxHeapSize;
                        const TUint heapBaseAddress = (TUint) info.AsRHeap().MetaData().ChunkBaseAddress();
                        const TBool correctHeapCellLocated = VerifyCorrectHeapCellL( *data, cellAddress, cellPayloadAddress, heapBaseAddress, heapMaxSize );
                        TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::LocateCFbTopHeapCellDataLC() - verified: %d", correctHeapCellLocated));

                        if  ( correctHeapCellLocated )
                            {
                            aCellAddress = cellAddress;
                            heapCellData = data;
                            CleanupStack::Pop( data );
                            break;
                            }
                        else
                            {
                            // Not the right cell. Keep going...
                            CleanupStack::PopAndDestroy( data );
                            }
                        }
                    }
                }
            }

        // NB: don't do this here - 
        // iEngine.Driver().WalkHeapClose();
        // It gets done after all reading is complete
        }

    if  ( heapCellData == NULL )
        {
        TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::LocateCFbTopHeapCellDataLC() - END - didn't find the right cell => KErrNotFound"));
        User::Leave( KErrNotFound );
        }
    //
    CleanupStack::PushL( heapCellData );

    TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::LocateCFbTopHeapCellDataLC() - END - everything okay, cell is: 0x%08x", aCellAddress));
    return heapCellData;
    }


void CMemSpyEngineHelperFbServ::ReadCObjectConInfoL( TAny* aCellAddress, RArray<TAny*>& aContainerObjects, TInt& aCount, TInt& aAllocated )
    {
    TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::ReadCObjectConInfoL() - START - trying cell: 0x%08x", aCellAddress ));

    // What we will return
    HBufC8* cellData = NULL;
    
    // Try to get info about real cell
    TMemSpyDriverCellType cellType;
    TInt cellLength;
    TInt cellNestingLevel;
    TInt cellAllocationNumber;
    TInt cellHeaderSize;
    TAny* cellPayloadAddress;

    // NOTE: this call may change the value of 'cellAddress'
    TInt err = iEngine.Driver().WalkHeapGetCellInfo( aCellAddress, cellType, cellLength, cellNestingLevel, cellAllocationNumber, cellHeaderSize, cellPayloadAddress );
    TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::ReadCObjectConInfoL() - err: %d, cellAddress: 0x%08x, cellLength: %d, cellAllocationNumber: %d, cellType: %d", err, aCellAddress, cellLength, cellAllocationNumber, cellType));

	if (err == KErrNone && (cellType & EMemSpyDriverAllocatedCellMask))
        {
        // Check that the cell size meets our expectations - it should be a CObjectCon cell.
        const TInt expectedCellSize = sizeof(CObjectCon*) + cellHeaderSize;
        TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::ReadCObjectConInfoL() - expectedCellSize: %d, actual: %d, address: 0x%08x", expectedCellSize, cellLength, aCellAddress));

        if  ( expectedCellSize <= cellLength )
            {
            TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::ReadCObjectConInfoL() - cell lengths are good, trying to get cell data..."));

            // Try to get real cell data        
            cellData = HBufC8::NewLC( cellLength );
            TPtr8 pData( cellData->Des() );
            //
            err = iEngine.Driver().WalkHeapReadCellData( aCellAddress, pData, cellLength );
            TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::ReadCObjectConInfoL() - reading cell data returned error: %d", err));
            }
        else
            {
            TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::ReadCObjectConInfoL() - not enough data -> KErrCorrupt"));
            err = KErrCorrupt;
            }
        }
    else
        {
        TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::ReadCObjectConInfoL() - END - didn't find cell data => KErrNotFound"));
        User::Leave( KErrNotFound );
        }

    CObjectCon* con = CObjectCon::NewL();
    const TUint32 KExpectedObjectConVTable = *((TUint32*) con);
    TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::ReadCObjectConInfoL() - KExpectedObjectConVTable: 0x%08x", KExpectedObjectConVTable ));
    delete con;

    // We should have the valid CObjectCon data now.
    RDesReadStream stream( *cellData );
    CleanupClosePushL( stream );
    
    // Read vtable
    const TUint32 vTable = stream.ReadUint32L();
    TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::ReadCObjectConInfoL() - actual vTable: 0x%08x", vTable ));
    if  ( vTable != KExpectedObjectConVTable )
        {
        User::Leave( KErrNotFound );
        }

    const TInt uniqueId = stream.ReadInt32L();
    TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::ReadCObjectConInfoL() - uniqueId: %d", uniqueId ));

    aCount = stream.ReadInt32L();
    TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::ReadCObjectConInfoL() - aCount: %d", aCount ));

    aAllocated = stream.ReadInt32L();
    TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::ReadCObjectConInfoL() - aAllocated: %d", aAllocated ));

    // This is actually CObject's iObject, i.e. CObject** iObjects.
    TAny* pObjects = reinterpret_cast< TAny*>( stream.ReadUint32L() );
    TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::ReadCObjectConInfoL() - pObjects: 0x%08x", pObjects ));

    CleanupStack::PopAndDestroy( 2, cellData ); // cellData & stream

    // Now fetch the cell containing the CObject pointers...
    err = iEngine.Driver().WalkHeapGetCellInfo( pObjects, cellType, cellLength, cellNestingLevel, cellAllocationNumber, cellHeaderSize, cellPayloadAddress );
    TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::ReadCObjectConInfoL() - array of CObject* cell fetch err: %d, cellAddress: 0x%08x, cellLength: %d, cellAllocationNumber: %d, cellType: %d", err, aCellAddress, cellLength, cellAllocationNumber, cellType));
    User::LeaveIfError( err );

    const TInt expectedSize = ( aAllocated * sizeof(CObject*) ) + cellHeaderSize;
    TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::ReadCObjectConInfoL() - expectedSize: %d, actual size: %d", expectedSize, cellLength ));
    if  ( cellLength < expectedSize )
        {
        User::Leave( KErrUnderflow );
        }
    
    // Get the data
    cellData = HBufC8::NewLC( cellLength );
    TPtr8 pData( cellData->Des() );
    err = iEngine.Driver().WalkHeapReadCellData( pObjects, pData, cellLength );
    TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::ReadCObjectConInfoL() - reading cell data returned error: %d", err));
    User::LeaveIfError( err );

    // Open stream
    stream.Open( *cellData );
    CleanupClosePushL( stream );

    // Extract array of pointers
    for( TInt i=0; i<aCount; i++ )
        {
        TAny* objectAddress = reinterpret_cast< TAny*>( stream.ReadUint32L() );
        TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::ReadCObjectConInfoL() - object[%04d]: 0x%08x", i, objectAddress ));
        aContainerObjects.AppendL( objectAddress );
        }
    CleanupStack::PopAndDestroy( 2, cellData ); // cellData & stream

    TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::ReadCObjectConInfoL() - END" ));
    }


HBufC8* CMemSpyEngineHelperFbServ::LocateBitmapArrayHeapCellDataLC( TAny*& aArrayCellAddress, TInt aArrayAllocCount )
    {
    TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::LocateBitmapArrayHeapCellDataLC() - START - trying cell: 0x%08x, aArrayAllocCount: %d", aArrayCellAddress, aArrayAllocCount));

    // What we will return
    HBufC8* cellData = NULL;
    
    // Try to get info about real cell
    TMemSpyDriverCellType cellType;
    TInt cellLength;
    TInt cellNestingLevel;
    TInt cellAllocationNumber;
    TInt cellHeaderSize;
    TAny* cellPayloadAddress;

    // NOTE: this call may change the value of 'cellAddress'
    TInt err = iEngine.Driver().WalkHeapGetCellInfo( aArrayCellAddress, cellType, cellLength, cellNestingLevel, cellAllocationNumber, cellHeaderSize, cellPayloadAddress );
    TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::LocateCFbTopHeapCellDataLC() - err: %d, cellAddress: 0x%08x, cellLength: %d, cellAllocationNumber: %d, cellType: %d", err, aArrayCellAddress, cellLength, cellAllocationNumber, cellType));

    if (err == KErrNone && (cellType & EMemSpyDriverAllocatedCellMask))
        {
        // Check that the cell size meets our expectations. 
        // The cell should be a very specific length
        const TInt expectedCellSize = (sizeof(CBase*) * aArrayAllocCount) + cellHeaderSize;
        TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::LocateBitmapArrayHeapCellDataLC() - expectedCellSize: %d, actual: %d, address: 0x%08x", expectedCellSize, cellLength, aArrayCellAddress));

        if  ( expectedCellSize <= cellLength )
            {
            TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::LocateBitmapArrayHeapCellDataLC() - cell lengths are good, trying to get cell data..."));

            // Try to get real cell data        
            cellData = HBufC8::NewLC( cellLength );
            TPtr8 pData( cellData->Des() );
            //
            err = iEngine.Driver().WalkHeapReadCellData( aArrayCellAddress, pData, cellLength );
            TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::LocateBitmapArrayHeapCellDataLC() - reading cell data returned error: %d", err));
            }
        else
            {
            TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::LocateBitmapArrayHeapCellDataLC() - not enough data -> KErrCorrupt"));
            err = KErrCorrupt;
            }
        }
    //
    if  ( cellData == NULL )
        {
        TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::LocateBitmapArrayHeapCellDataLC() - END - didn't find cell data => KErrNotFound"));
        err = KErrNotFound;
        }
    User::LeaveIfError( err );
    //
    TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::LocateBitmapArrayHeapCellDataLC() - END - ok! - got %d bytes", cellData->Length() ) );
    return cellData;
    }


void CMemSpyEngineHelperFbServ::ParseCellDataAndExtractHandlesL( const TDesC8& aData, RArray<TInt>& aHandles, TInt aArrayEntryCount )
    {
    aHandles.Reset();
    
    // Create read stream
    RDesReadStream stream( aData );
    CleanupClosePushL( stream );
    
    for( TInt i=0; i<aArrayEntryCount; i++ )
        {
        const TInt handle = stream.ReadInt32L();
        TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::ParseCellDataAndExtractHandlesL() - handle[%4d] = 0x%08x", i, handle));
        aHandles.AppendL( handle );
        }
    
    // Tidy up
    CleanupStack::PopAndDestroy( &stream );
    }


TBool CMemSpyEngineHelperFbServ::VerifyCorrectHeapCellL( const TDesC8& aData, TAny* aCellAddress, TAny* aPayloadAddress, TUint aHeapStartingAddress, TUint aHeapMaxSize )
    {
    (void) aPayloadAddress;
    (void) aCellAddress;
    TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::VerifyCorrectHeapCellL() - START - aDataLen: %d, aCellAddress: 0x%08x, aPayloadAddress: 0x%08x, aHeapStartingAddress: 0x%08x, aHeapSize: %d", aData.Length(), aCellAddress, aPayloadAddress, aHeapStartingAddress, aHeapMaxSize ));

    const TUint KFbServHeapCeilingAddress = aHeapStartingAddress + aHeapMaxSize;

    // Whether we can use this cell's data...
    TBool correctCell = EFalse;

    // We can use certain knowledge of the layout of the cell to work out
    // if we've found the right one. 
    // 
    // This is fragile because of the font and bitmap server heap layout changes 
    // significantly, then this can break.
    RDesReadStream reader( aData );
    CleanupClosePushL( reader );

    // Get vtable address
    TUint address = reader.ReadUint32L();
    TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::VerifyCorrectHeapCellL() - suspected vtable: 0x%08x", address));

#ifdef __EPOC32__
    TBool isROMAddress = EFalse;
    TInt err = User::IsRomAddress( isROMAddress, (TAny*) address );
    TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::VerifyCorrectHeapCellL() - vtable (0x%08x) is in ROM: %d (error: %d)", address, isROMAddress, err));
    User::LeaveIfError( err );

    if  ( !isROMAddress )
        {
        // Close stream
        CleanupStack::PopAndDestroy( &reader );

        // VTable value should be in the ROM range.
        correctCell = EFalse;
        TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::VerifyCorrectHeapCellL() - END - suspected CFbTop vTable isn't in ROM address range! - correctCell: %d", correctCell));
        return correctCell;
        }
#else
    TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::VerifyCorrectHeapCellL() - skipping vtable: 0x%08x", address));
#endif

    // First real item is an RFs object instance. We can't check this
    // very easily because its an embedded object-> We must skip it
    __ASSERT_ALWAYS( sizeof(RFs) == 4, User::Invariant() );
    address = reader.ReadUint32L();
    TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::VerifyCorrectHeapCellL() - skipping RFs: 0x%08x", address));

    // Next object is an address (CFontStore* iFontStore). We can validate this against
    // the method arguments.
    address = reader.ReadUint32L();
    TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::VerifyCorrectHeapCellL() - iFontStore: 0x%08x", address));

    if  ( address > aHeapStartingAddress && address < KFbServHeapCeilingAddress )
        {
        // Next comes another pointer (CObjectCon* iBitmapCon)
        address = reader.ReadUint32L();
        TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::VerifyCorrectHeapCellL() - iBitmapCon: 0x%08x", address));

        if  ( address > aHeapStartingAddress && address < KFbServHeapCeilingAddress )
            {
            // Next another pointer (CObjectCon* iFontCon)
            address = reader.ReadUint32L();
            TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::VerifyCorrectHeapCellL() - iFontCon: 0x%08x", address));

            if  ( address > aHeapStartingAddress && address < KFbServHeapCeilingAddress )
                {
                // And finally, yet another pointer (RHeap* iHeap)
                address = reader.ReadUint32L();
                TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::VerifyCorrectHeapCellL() - iHeap: 0x%08x", address));

                if  ( address > aHeapStartingAddress && address < KFbServHeapCeilingAddress )
                    {
                    // That'll do.
                    TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::VerifyCorrectHeapCellL() - data seems okay!"));
                    correctCell = ETrue;
                    }
                }
            }
        }

    CleanupStack::PopAndDestroy( &reader );
    //
    TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::VerifyCorrectHeapCellL() - END - correctCell: %d", correctCell));
    return correctCell;
    }


TUint CMemSpyEngineHelperFbServ::OffsetToCObjectConBitmapCon()
    {
    TUint ret = 0;

    // VTable
    ret += 4;

    // RFs
    ret += sizeof(RFs);

	// CFontStore* iFontStore;
    ret += sizeof(CBase*); // best effort

    return ret;
    }


TUint CMemSpyEngineHelperFbServ::OffsetToCObjectConFontCon()
    {
    TUint ret = OffsetToCObjectConBitmapCon();

	// CObjectCon* iBitmapCon;
    ret += sizeof(CObjectCon*);

    return ret;
    }


TUint CMemSpyEngineHelperFbServ::OffsetToBitmapHandleArray()
    {
    TUint ret = OffsetToCObjectConFontCon();

	// CObjectConIx* iConIx;
    ret += sizeof(CObjectConIx*);

	// RHeap* iHeap;
    ret += sizeof(RHeap*);

	// RChunk iChunk
    ret += sizeof(RChunk);

	// RChunk iLargeBitmapChunk
    ret += sizeof(RChunk);

	// RMutex iLargeBitmapAccess
    ret += sizeof(RMutex);

	// CChunkPile* iPile;
    ret += sizeof(CBase*); // best effort

	// RPointerArray<HBufC> iFontNameAlias;
    ret += sizeof(RPointerArray<HBufC>);
	
    return ret;
    }


CMemSpyEngineHelperFbServ::CBitmapObject* CMemSpyEngineHelperFbServ::GetBitmapObjectLC( TAny* aCellAddress )
    {
    TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::GetBitmapObjectLC() - START - aCellAddress: 0x%08x", aCellAddress ));

    // What we will return
    CBitmapObject* object = new(ELeave) CBitmapObject();
    CleanupStack::PushL( object );

    // The data we will read
    HBufC8* cellData = NULL;
    
    // Try to get info about real cell
    TMemSpyDriverCellType cellType;
    TInt cellLength;
    TInt cellNestingLevel;
    TInt cellAllocationNumber;
    TInt cellHeaderSize;
    TAny* cellPayloadAddress;

    // NOTE: this call may change the value of 'cellAddress'
    TInt err = iEngine.Driver().WalkHeapGetCellInfo( aCellAddress, cellType, cellLength, cellNestingLevel, cellAllocationNumber, cellHeaderSize, cellPayloadAddress );
    TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::GetBitmapObjectLC() - err: %d, cellAddress: 0x%08x, cellLength: %d, cellAllocationNumber: %d, cellType: %d", err, aCellAddress, cellLength, cellAllocationNumber, cellType));

    if (err == KErrNone && (cellType & EMemSpyDriverAllocatedCellMask))
        {
        // Check that the cell size meets our expectations - it should be a CBitmapObject, but without the additional "this" pointer
        // which we have tacked onto the object.
        const TInt expectedCellSize = sizeof( CBitmapObject ) + cellHeaderSize - sizeof( CBitmapObject* );
        TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::GetBitmapObjectLC() - expectedCellSize: %d, actual: %d, address: 0x%08x", expectedCellSize, cellLength, aCellAddress));

        if  ( expectedCellSize <= cellLength )
            {
            TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::GetBitmapObjectLC() - cell lengths are good, trying to get cell data..."));

            // Try to get real cell data        
            cellData = HBufC8::NewLC( cellLength );
            TPtr8 pData( cellData->Des() );
            //
            err = iEngine.Driver().WalkHeapReadCellData( aCellAddress, pData, cellLength );
            TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::GetBitmapObjectLC() - reading cell data returned error: %d", err));
            }
        else
            {
            TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::GetBitmapObjectLC() - not enough data -> KErrCorrupt"));
            err = KErrCorrupt;
            }
        }
    else
        {
        User::Leave( KErrNotFound );
        }

    RDesReadStream stream( *cellData );
    CleanupClosePushL( stream );
    
    // Read vtable
    const TUint32 vTable = stream.ReadUint32L();
    TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::GetBitmapObjectLC() - actual vTable: 0x%08x", vTable ));
    TBool isROMAddress = EFalse;
    err = User::IsRomAddress( isROMAddress, (TAny*) vTable );
    TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::GetBitmapObjectLC() - vtable (0x%08x) is in ROM: %d (error: %d)", vTable, isROMAddress, err));
    User::LeaveIfError( err );

    // Skip CObject members
    (void) stream.ReadL( sizeof( CObject ) ); // skip this much
    object->iThisPointer = reinterpret_cast< CBitmapObject* >( cellPayloadAddress );;
    object->iTop = reinterpret_cast< CBase* >( stream.ReadUint32L() );
    object->iAddressPointer = reinterpret_cast< CBitwiseBitmap* >( stream.ReadUint32L() );
    object->iHandle = stream.ReadInt32L();
    object->iCleanBitmap = reinterpret_cast< CBitmapObject* >( stream.ReadUint32L() );

    TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::GetBitmapObjectLC() - object->iThisPointer:    0x%08x", object->iThisPointer ));
    TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::GetBitmapObjectLC() - object->iTop:            0x%08x", object->iTop ));
    TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::GetBitmapObjectLC() - object->iAddressPointer: 0x%08x", object->iAddressPointer ));
    TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::GetBitmapObjectLC() - object->iHandle:         0x%08x", object->iHandle ));
    TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::GetBitmapObjectLC() - object->iCleanBitmap:    0x%08x", object->iCleanBitmap ));
    
    // Clean up - don't need this data anymore. Real data is in another cell
    CleanupStack::PopAndDestroy( 2, cellData );  // stream & cellData

    TRACE( RDebug::Printf("CMemSpyEngineHelperFbServ::GetBitmapObjectLC() - END - aCellAddress: 0x%08x", aCellAddress ));
    return object;
    }



























CMemSpyEngineFbServBitmapArray::CMemSpyEngineFbServBitmapArray( const RArray<TInt>& aBitmapHandles )
:   CActive( CActive::EPriorityIdle ), iBitmapHandles( aBitmapHandles ), iGranularity( 0 )
    {
    CActiveScheduler::Add( this );
    }


CMemSpyEngineFbServBitmapArray::CMemSpyEngineFbServBitmapArray( TInt aPriority, const RArray<TInt>& aBitmapHandles, MMemSpyEngineFbSerbBitmapArrayObserver& aObserver, TInt aGranularity )
:   CActive( aPriority ), iBitmapHandles( aBitmapHandles ), iObserver( &aObserver ), iGranularity( aGranularity )
    {
    CActiveScheduler::Add( this );
    CompleteSelf();
    }


EXPORT_C CMemSpyEngineFbServBitmapArray::~CMemSpyEngineFbServBitmapArray()
    {
    Cancel();
    //
    iBitmaps.ResetAndDestroy();
    iBitmaps.Close();
    }


void CMemSpyEngineFbServBitmapArray::ConstructL()
    {
    if  ( !IsActive() )
        {
        // Sync construction...
        const TInt count = iBitmapHandles.Count();
        for(TInt i=0; i<count; i++ )
            {
            const TInt handle = iBitmapHandles[ i ];
            //
            TRAP_IGNORE( CreateBitmapL( handle ) );
            }
    
        SortBySizeL();
        }
    }


EXPORT_C CMemSpyEngineFbServBitmapArray* CMemSpyEngineFbServBitmapArray::NewL( const RArray<TInt>& aBitmapHandles )
    {
    CMemSpyEngineFbServBitmapArray* self = new(ELeave) CMemSpyEngineFbServBitmapArray( aBitmapHandles );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


EXPORT_C CMemSpyEngineFbServBitmapArray* CMemSpyEngineFbServBitmapArray::NewL( TInt aPriority, const RArray<TInt>& aBitmapHandles, MMemSpyEngineFbSerbBitmapArrayObserver& aObserver, TInt aGranularity )
    {
    CMemSpyEngineFbServBitmapArray* self = new(ELeave) CMemSpyEngineFbServBitmapArray( aPriority, aBitmapHandles, aObserver, aGranularity );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


EXPORT_C TInt CMemSpyEngineFbServBitmapArray::Count() const
    {
    return iBitmaps.Count();
    }


EXPORT_C CMemSpyEngineFbServBitmap& CMemSpyEngineFbServBitmapArray::At( TInt aIndex )
    {
    return *iBitmaps[ aIndex ];
    }


EXPORT_C const CMemSpyEngineFbServBitmap& CMemSpyEngineFbServBitmapArray::At( TInt aIndex ) const
    {
    return *iBitmaps[ aIndex ];
    }


EXPORT_C CMemSpyEngineFbServBitmap& CMemSpyEngineFbServBitmapArray::BitmapByHandleL( TInt aHandle )
    {
    const TInt index = BitmapIndexByHandle( aHandle );
    User::LeaveIfError( index );
    CMemSpyEngineFbServBitmap& ret = At( index );
    return ret;
    }


EXPORT_C TInt CMemSpyEngineFbServBitmapArray::BitmapIndexByHandle( TInt aHandle ) const
    {
    TInt ret = KErrNotFound;
    //
    const TInt count = Count();
    for( TInt i=0; i<count; i++ )
        {
        const CMemSpyEngineFbServBitmap& bitmap = At( i );
        //
        if  ( bitmap.Handle() == aHandle )
            {
            ret = i;
            break;
            }
        }
    //
    return ret;
    }


EXPORT_C TInt CMemSpyEngineFbServBitmapArray::MdcaCount() const
    {
    return Count();
    }


EXPORT_C TPtrC CMemSpyEngineFbServBitmapArray::MdcaPoint( TInt aIndex ) const
    {
    const CMemSpyEngineFbServBitmap& bitmap = At( aIndex );
    return TPtrC( bitmap.Caption() );
    }


void CMemSpyEngineFbServBitmapArray::RunL()
    {
    const TInt bitmapHandleCount = iBitmapHandles.Count();
    const TInt endIndex = Min( iIndex + iGranularity, bitmapHandleCount );
    //
    for( ; iIndex < endIndex; iIndex++ )
        {
        const TInt handle = iBitmapHandles[ iIndex ];
        //
        TRAP_IGNORE( CreateBitmapL( handle ) );
        }
    
    // Report progress & queue for next iteration
    MMemSpyEngineFbSerbBitmapArrayObserver::TEvent event = MMemSpyEngineFbSerbBitmapArrayObserver::EBitmapArrayConstructionComplete;
    if  ( endIndex < bitmapHandleCount )
        {
        CompleteSelf();
        event = MMemSpyEngineFbSerbBitmapArrayObserver::EBitmapItemsCreated;
        }
    else
        {
        SortBySizeL();
        }

    if  ( iObserver )
        {
        iObserver->HandleFbServBitmapArrayEventL( event );
        }
    }


void CMemSpyEngineFbServBitmapArray::DoCancel()
    {
    // Nothing to do
    }


TInt CMemSpyEngineFbServBitmapArray::RunError( TInt /*aError*/ )
    {
    return KErrNone;
    }


void CMemSpyEngineFbServBitmapArray::CompleteSelf()
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    SetActive();
    }


void CMemSpyEngineFbServBitmapArray::CreateBitmapL( TInt aHandle )
    {
    CMemSpyEngineFbServBitmap* bitmap = CMemSpyEngineFbServBitmap::NewLC( aHandle );
    iBitmaps.AppendL( bitmap );
    CleanupStack::Pop( bitmap );
    }


void CMemSpyEngineFbServBitmapArray::SortBySizeL()
    {
    TLinearOrder< CMemSpyEngineFbServBitmap > comparer( CompareBySize );
    iBitmaps.Sort( comparer );
    }


TInt CMemSpyEngineFbServBitmapArray::CompareBySize( const CMemSpyEngineFbServBitmap& aLeft, const CMemSpyEngineFbServBitmap& aRight )
    {
    TInt ret = 1;
    //
    if  ( aLeft.BitmapSizeInBytes() > aRight.BitmapSizeInBytes() )
        {
        ret = -1;
        }
    else if ( aLeft.BitmapSizeInBytes() == aRight.BitmapSizeInBytes() )
        {
        ret = 0;
        }
    //
    return ret;
    }






















CMemSpyEngineFbServBitmap::CMemSpyEngineFbServBitmap()
:   CDesCArrayFlat( 5 )
    {
    }


EXPORT_C CMemSpyEngineFbServBitmap::~CMemSpyEngineFbServBitmap()
    {
    delete iBitmap;
    delete iCaption;
    }


void CMemSpyEngineFbServBitmap::ConstructL( TInt aHandle )
    {
    iBitmap = new(ELeave) CFbsBitmap();
    const TInt error = iBitmap->Duplicate( aHandle );
    User::LeaveIfError( error );
    const SEpocBitmapHeader header( iBitmap->Header() );

    // Prepare caption
    _LIT( KCaptionFormat, "\t%2d bpp, %S\t\t%4d x %4d pixels" );
    iCaption = HBufC::NewL( 128 );
    TPtr pCaption( iCaption->Des() );
    
    // Create size string
    const TMemSpySizeText size( MemSpyEngineUtils::FormatSizeText( header.iBitmapSize ) );
    pCaption.AppendFormat( KCaptionFormat, header.iBitsPerPixel, &size, header.iSizeInPixels.iWidth, header.iSizeInPixels.iHeight );

    // Prepare items
    PrepareItemsL();
    }


EXPORT_C CMemSpyEngineFbServBitmap* CMemSpyEngineFbServBitmap::NewLC( TInt aHandle )
    {
    CMemSpyEngineFbServBitmap* self = new(ELeave) CMemSpyEngineFbServBitmap();
    CleanupStack::PushL( self );
    self->ConstructL( aHandle );
    return self;
    }


TInt CMemSpyEngineFbServBitmap::BitmapSizeInBytes() const
    {
    const SEpocBitmapHeader header( iBitmap->Header() );
    return header.iBitmapSize;
    }


EXPORT_C const TDesC& CMemSpyEngineFbServBitmap::Caption() const
    {
    return *iCaption;
    }


EXPORT_C CFbsBitmap& CMemSpyEngineFbServBitmap::Bitmap()
    {
    return *iBitmap;
    }


EXPORT_C const CFbsBitmap& CMemSpyEngineFbServBitmap::Bitmap() const
    {
    return *iBitmap;
    }


EXPORT_C TInt CMemSpyEngineFbServBitmap::Handle() const
    {
    return iBitmap->Handle();
    }


EXPORT_C void CMemSpyEngineFbServBitmap::OutputDataL( CMemSpyEngine& aEngine ) const
    {
    const SEpocBitmapHeader header( iBitmap->Header() );
    //
    _LIT(KMemSpyFBServHex, "0x%08x");
    _LIT(KMemSpyFBServDecimal, "%d");
    //
    HBufC* columns = HBufC::NewLC( 1024 );
    TPtr pColumns( columns->Des() );
    //
    pColumns.AppendFormat( KMemSpyFBServHex, Handle() );
    pColumns.Append( KMemSpyEngineFBServComma );
    //
    pColumns.AppendFormat( KMemSpyFBServDecimal, header.iBitmapSize );
    pColumns.Append( KMemSpyEngineFBServComma );
    //
    pColumns.AppendFormat( KMemSpyFBServDecimal, header.iSizeInPixels.iWidth );
    pColumns.Append( KMemSpyEngineFBServComma );
    //
    pColumns.AppendFormat( KMemSpyFBServDecimal, header.iSizeInPixels.iHeight );
    pColumns.Append( KMemSpyEngineFBServComma );
    //
    pColumns.AppendFormat( KMemSpyFBServDecimal, header.iBitsPerPixel );
    pColumns.Append( KMemSpyEngineFBServComma );
    //
    pColumns.AppendFormat( KMemSpyFBServDecimal, header.iColor );
    pColumns.Append( KMemSpyEngineFBServComma );
    //
    TBuf<24> fileCompressionType;
    GetFileCompressionTypeAsString( header.iCompression, fileCompressionType );
    pColumns.Append( fileCompressionType );
    pColumns.Append( KMemSpyEngineFBServComma );
    //
    GetExportableFileNameL( pColumns );
    //
    aEngine.Sink().OutputLineL( pColumns );
    CleanupStack::PopAndDestroy( columns );
    }


EXPORT_C void CMemSpyEngineFbServBitmap::OutputDataColumnsL( CMemSpyEngine& aEngine )
    {
    HBufC* columns = HBufC::NewLC( 1024 );
    TPtr pColumns( columns->Des() );
    //
    _LIT(KCol1, "Handle");
    pColumns.Append( KCol1 );
    pColumns.Append( KMemSpyEngineFBServComma );
    //
    _LIT(KCol2, "Uncompressed Size (Bytes)");
    pColumns.Append( KCol2 );
    pColumns.Append( KMemSpyEngineFBServComma );
    //
    _LIT(KCol3, "Width");
    pColumns.Append( KCol3 );
    pColumns.Append( KMemSpyEngineFBServComma );
    //
    _LIT(KCol4, "Height");
    pColumns.Append( KCol4 );
    pColumns.Append( KMemSpyEngineFBServComma );
    //
    _LIT(KCol5, "BPP");
    pColumns.Append( KCol5 );
    pColumns.Append( KMemSpyEngineFBServComma );
    //
    _LIT(KCol6, "Color");
    pColumns.Append( KCol6 );
    pColumns.Append( KMemSpyEngineFBServComma );
    //
    _LIT(KCol7, "Compression");
    pColumns.Append( KCol7 );
    pColumns.Append( KMemSpyEngineFBServComma );
    //
    _LIT(KCol8, "File Name");
    pColumns.Append( KCol8 );
    //
    aEngine.Sink().OutputLineL( pColumns );
    CleanupStack::PopAndDestroy( columns );
    }


EXPORT_C void CMemSpyEngineFbServBitmap::GetExportableFileNameL( TDes& aAppendName ) const
    {
    const SEpocBitmapHeader header( iBitmap->Header() );

    // Create a filename
    aAppendName.AppendFormat( KMemSpyEngineFBServExportFileName, Handle(), header.iBitsPerPixel, header.iSizeInPixels.iWidth, header.iSizeInPixels.iHeight );
    }


void CMemSpyEngineFbServBitmap::GetExportableFileNameL( const TMemSpyEngineFBServBitmapInfo& aInfo, TDes& aAppendName )
    {
    aAppendName.AppendFormat( KMemSpyEngineFBServExportFileName, aInfo.iHandle, aInfo.iBitsPerPixel, aInfo.iSizeInPixels.iWidth, aInfo.iSizeInPixels.iHeight );
    }


void CMemSpyEngineFbServBitmap::PrepareItemsL()
    {
    const SEpocBitmapHeader header( iBitmap->Header() );
    TBuf<256> item;

    // View bitmap (ugly, but needed by UI)
    _LIT(KCaptionMinus2, "\tView Bitmap\t\t");
    AppendL( KCaptionMinus2 );

    // Dimensions
    _LIT(KCaptionMinus1, "\tDimensions\t\t%5d x %5d");
    item.Format( KCaptionMinus1, header.iSizeInPixels.iWidth, header.iSizeInPixels.iHeight );
    AppendL( item );

    // Handle
    _LIT(KCaption0, "\tHandle\t\t0x%08x");
    item.Format( KCaption0, iBitmap->Handle() );
    AppendL( item );

    // In Rom?
    _LIT(KCaption1, "\tIn ROM?\t\t%S");
    _LIT(KIsRomBitmap, "Yes");
    _LIT(KNotRomBitmap, "No");
    if  ( iBitmap->IsRomBitmap() )
        {
        item.Format( KCaption1, &KIsRomBitmap );
        }
    else
        {
        item.Format( KCaption1, &KNotRomBitmap );
        }
    AppendL( item );

    // Bits per pixel
    _LIT(KCaption2, "\tBits Per Pixel\t\t%2d");
    item.Format( KCaption2, header.iBitsPerPixel );
    AppendL( item );

    // Size of uncompressed bitmap data
    _LIT(KCaption3, "\tUncompressed data size\t\t%S");
    const TMemSpySizeText sizeStringUncompressed( MemSpyEngineUtils::FormatSizeText( header.iBitmapSize ) );
    item.Format( KCaption3, &sizeStringUncompressed );
    AppendL( item );

    // Palette entry count
    _LIT(KCaption4, "\tNumber of palette entries\t\t%d");
    item.Format( KCaption4, header.iPaletteEntries );
    AppendL( item );

    // Colour
    _LIT(KCaption5, "\tColour\t\t%d");
    item.Format( KCaption5, header.iColor);
    AppendL( item );

    // Palette entry count
    _LIT(KCaption6, "\tFile compression type\t\t%S");
    TBuf<24> fileCompressionType;
    GetFileCompressionTypeAsString( header.iCompression, fileCompressionType );
    item.Format( KCaption6, &fileCompressionType );
    AppendL( item );
    }


void CMemSpyEngineFbServBitmap::GetFileCompressionTypeAsString( TBitmapfileCompression aType, TDes& aString )
    {
    _LIT(KUnknown, "Unknown");
    _LIT(KNoCompression, "None");
    _LIT(KRLE, "RLE");
    _LIT(KTwelveBitRLE, "12-bit RLE");
    _LIT(KSixteenBitRLE, "16-bit RLE");
    _LIT(KTwentyFourBitRLE, "24-bit RLE");
    _LIT(KThirtyTwoBitRLE, "32-bit RLE");
    //
    switch( aType )
        {
    case ENoBitmapCompression:
        aString = KNoCompression;
        break;
    case EByteRLECompression:
        aString = KRLE;
        break;
    case ETwelveBitRLECompression:
        aString = KTwelveBitRLE;
        break;
    case ESixteenBitRLECompression:
        aString = KSixteenBitRLE;
        break;
    case ETwentyFourBitRLECompression:
        aString = KTwentyFourBitRLE;
        break;
    case EThirtyTwoUBitRLECompression:
        aString = KThirtyTwoBitRLE;
        break;
    default:
    case ERLECompressionLast:
        aString = KUnknown;
        break;
        }
    }








