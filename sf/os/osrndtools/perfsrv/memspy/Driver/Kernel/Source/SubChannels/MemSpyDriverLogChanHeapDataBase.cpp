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

#include "MemSpyDriverLogChanHeapDataUser.h"

// System includes
#include <u32hal.h>
#include <e32rom.h>
#include <memspy/driver/memspydriverconstants.h>
#include <memspy/driver/memspydriverobjectsshared.h>

// Shared includes
#include "MemSpyDriverOpCodes.h"
#include "MemSpyDriverObjectsInternal.h"

// User includes
#include "MemSpyDriverLogChanHeapDataBase.h"
#include "MemSpyDriverHeap.h"
#include "MemSpyDriverUtils.h"
#include "MemSpyDriverDevice.h"
#include "MemSpyDriverOSAdaption.h"
#include "MemSpyDriverHeapWalker.h"
#include "MemSpyDriverUserEventMonitor.h"
#include "MemSpyDriverSuspensionManager.h"

DMemSpyDriverLogChanHeapDataBase::DMemSpyDriverLogChanHeapDataBase( DMemSpyDriverDevice& aDevice, DThread& aThread )
:	DMemSpyDriverLogChanHeapBase( aDevice, aThread )
    {
    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapDataBase::DMemSpyDriverLogChanHeapDataBase() - this: 0x%08x", this ));
    }

DMemSpyDriverLogChanHeapDataBase::~DMemSpyDriverLogChanHeapDataBase()
	{
	TRACE( Kern::Printf("DMemSpyDriverLogChanHeapDataBase::~DMemSpyDriverLogChanHeapDataBase() - START - this: 0x%08x", this ));
	ReleaseCellList();
	TRACE( Kern::Printf("DMemSpyDriverLogChanHeapDataBase::~DMemSpyDriverLogChanHeapDataBase() - END - this: 0x%08x", this ));
	}

TInt DMemSpyDriverLogChanHeapDataBase::PrepareCellListTransferBuffer()
    {
    // Transfer free cells immediately from xfer stream
    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapDataBase::PrepareCellListTransferBuffer() - START - iHeapStream: 0x%08x", iHeapStream ));
    __ASSERT_ALWAYS( !iHeapStream, MemSpyDriverUtils::PanicThread( ClientThread(), EPanicHeapFreeCellStreamNotClosed ) );
    //
    TInt r = KErrNoMemory;
    //
    NKern::ThreadEnterCS();
    //
    iHeapStream = new RMemSpyMemStreamWriter();
    if  ( iHeapStream )
        {
        const TInt requiredMemory = CalculateCellListBufferSize();
        r = OpenXferStream( *iHeapStream, requiredMemory );
        TRACE( Kern::Printf("DMemSpyDriverLogChanHeapDataBase::PrepareCellListTransferBuffer() - requested %d bytes for free cell list, r: %d", requiredMemory, r ));

        if  ( r == KErrNone )
            {
            const TInt count = iCellList.Count();
            TRACE( Kern::Printf("DMemSpyDriverLogChanHeapDataBase::PrepareCellListTransferBuffer() - cell count: %d", count ));
            //
            iHeapStream->WriteInt32( count );
            for( TInt i=0; i<count; i++ )
                {
                const TMemSpyDriverCell& cell = iCellList[ i ];
                TRACE( Kern::Printf("DMemSpyDriverLogChanHeapDataBase::PrepareCellListTransferBuffer() - storing entry: %d", i ));
                //
                iHeapStream->WriteInt32( cell.iType );
                iHeapStream->WriteUint32( reinterpret_cast<TUint32>( cell.iAddress ) );
                iHeapStream->WriteInt32( cell.iLength );
                }

            // Finished with the array now
            iCellList.Reset();

            // We return the amount of client-side memory that needs to be allocated to hold the buffer
            r = requiredMemory;
            }
        }
    //
    NKern::ThreadLeaveCS();
               
    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapDataBase::PrepareCellListTransferBuffer() - END - r: %d", r));
	return r;
    }

TInt DMemSpyDriverLogChanHeapDataBase::FetchCellList( TDes8* aBufferSink )
    {
    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapDataBase::FetchCellList() - START - iHeapStream: 0x%08x", iHeapStream ));
    __ASSERT_ALWAYS( iHeapStream, MemSpyDriverUtils::PanicThread( ClientThread(), EPanicHeapFreeCellStreamNotOpen ) );

    TInt r = KErrNone;

    // Write buffer to client
    NKern::ThreadEnterCS();
    r = iHeapStream->WriteAndClose( aBufferSink );

    // Tidy up
    ReleaseCellList();

    NKern::ThreadLeaveCS();
    //
    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapDataBase::FetchCellList() - END - r: %d", r));
	return r;
    }



TInt DMemSpyDriverLogChanHeapDataBase::CalculateCellListBufferSize() const
    {
    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapDataBase::CalculateCellListBufferSize() - START" ));

    const TInt count = iCellList.Count();
    const TInt entrySize = sizeof( TInt32 ) + sizeof( TInt32 ) + sizeof( TUint32 );
    const TInt r = ( count * entrySize ) + sizeof( TInt ); // Extra TInt to hold count
                
    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapDataBase::CalculateCellListBufferSize() - END - r: %d, count: %d, entrySize: %d", r, count, entrySize ));
	return r;
    }



void DMemSpyDriverLogChanHeapDataBase::ReleaseCellList()
    {
	TRACE( Kern::Printf("DMemSpyDriverLogChanHeapDataBase::ReleaseCellList() - START - this: 0x%08x", this ));

    NKern::ThreadEnterCS();
    iCellList.Reset();
    delete iHeapStream;
    iHeapStream = NULL;
    NKern::ThreadLeaveCS();

    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapDataBase::ReleaseCellList() - END - this: 0x%08x", this ));
    }

TBool DMemSpyDriverLogChanHeapDataBase::HandleHeapCell(TMemSpyDriverCellType aCellType, TAny* aCellAddress, TInt aLength, TInt /*aNestingLevel*/, TInt /*aAllocNumber*/)
    {
	TInt err = KErrNone;
    if (((aCellType & EMemSpyDriverFreeCellMask) && iHeapInfoParams.iBuildFreeCellList) || 
        ((aCellType & EMemSpyDriverAllocatedCellMask) && iHeapInfoParams.iBuildAllocCellList))
		{
		TMemSpyDriverCell cell;
		cell.iType = aCellType;
		cell.iAddress = aCellAddress;
		cell.iLength = aLength;

		NKern::ThreadEnterCS();
		err = iCellList.Append(cell);
		NKern::ThreadLeaveCS();
		}
	return err == KErrNone;
	}

void DMemSpyDriverLogChanHeapDataBase::HandleHeapWalkInit()
	{
	}

TInt DMemSpyDriverLogChanHeapDataBase::GetFullData( TMemSpyDriverInternalHeapDataParams* aParams )
    {
    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapDataBase::GetFullData() - START"));

    TMemSpyDriverInternalHeapDataParams params;
    TInt r = Kern::ThreadRawRead( &ClientThread(), aParams, &params, sizeof(TMemSpyDriverInternalHeapDataParams) );
    //
    if  ( r == KErrNone )
        {
        TRACE( Kern::Printf("DMemSpyDriverLogChanHeapDataBase::GetFullData - thread id: %d, remaining: %8d, vtable: 0x%08x", params.iTid, params.iRemaining, params.iRHeapVTable));

        // NB: The two 'get heap data' functions return either an error ( < KErrNone ) or then
        // return the length of the descriptor data that was written to the client's address
        // space.
        //
        r = GetFullData( params );

        // Write back to user space if everything went okay. Remember that the 
        // return value above will be the length of data that was written to the
        // client if there was no error.
        if  ( r >= KErrNone )
            {
            const TInt clientDescriptorLength = r;

            // Make sure we update client's remaining data
            r = Kern::ThreadRawWrite( &ClientThread(), aParams, &params, sizeof(TMemSpyDriverInternalHeapDataParams) );
            
            // ... and if that went okay, then we return the length of the descriptor data
            // back to the client.
            if  ( r == KErrNone )
                {
                r = clientDescriptorLength;
                }
            }
        }
    else
        {
        TRACE( Kern::Printf("DMemSpyDriverLogChanHeapDataBase::GetFullData - params read error: %d", r));
        }

    // Done
    TRACE( Kern::Printf("DMemSpyDriverLogChanHeapDataBase::GetFullData() - END - ret: %d", r));
    return r;
    }

TInt DMemSpyDriverLogChanHeapDataBase::DoGetFullData(TMemSpyDriverInternalHeapDataParams& aParams, 
                                                     DThread* aThread, 
                                                     RMemSpyDriverRHeapBase& aHeap)
    {
    TInt r = KErrNone;
    if  ( aParams.iChecksum != 0 )
        {
        TRACE( Kern::Printf("DMemSpyDriverLogChanHeapDataBase::DoGetFullData - checksum validation requested - expecting: 0x%08x", aParams.iChecksum ) );
        
        RMemSpyDriverHeapWalker heapWalker(aHeap);
        
        TRACE( Kern::Printf("DMemSpyDriverLogChanHeapDataBase::DoGetFullData - starting traversal..." ));
#if ( defined( TRACE_TYPE_USERHEAP ) && defined( TRACE_TYPE_HEAPWALK ) )
        heapWalker.SetPrintDebug();
#endif
        r = heapWalker.Traverse();
        const TUint32 calculatedChecksum = heapWalker.Stats().iFreeCellCRC;
        TRACE( Kern::Printf("DMemSpyDriverLogChanHeapDataBase::DoGetFullData - finished traversal - err: %d, checksum: 0x%08x", r, calculatedChecksum ));

        TRACE( Kern::Printf("DMemSpyDriverLogChanHeapDataBase::DoGetFullData - comparing CALCULATED: 0x%08x vs EXPECTED: 0x%08x", calculatedChecksum, aParams.iChecksum ));
        if  ( calculatedChecksum != aParams.iChecksum )
            {
            Kern::Printf("DMemSpyDriverLogChanHeapDataBase::DoGetFullData - comparing CALCULATED: 0x%08x vs EXPECTED: 0x%08x for thread %O", calculatedChecksum, aParams.iChecksum, aThread );
            r = KErrCorrupt;
            }
        }

    // Get user side (MemSpyApp) descriptor length info
    if  ( r == KErrNone )
        {
        TInt destLen = 0;
        TInt destMax = 0;
        TUint8* destPtr = NULL;
        r = Kern::ThreadGetDesInfo( &ClientThread(), aParams.iDes, destLen, destMax, destPtr, ETrue );
        TRACE( Kern::Printf("DMemSpyDriverLogChanHeapDataBase::DoGetFullData - user side descriptor: 0x%08x (0x%08x), len: %8d, maxLen: %8d, r: %d", aParams.iDes, destPtr, destLen, destMax, r ));
        destMax = destMax & ~(KPageSize-1); // Round down dest max to page size
        if (destMax <= 0 || (aParams.iReadAddress & (KPageSize-1)))
            {
            // If destMax is less than a page or the read address isn't a multiple of page size then we don't want to know
            r = KErrArgument;
            }

        if  ( r == KErrNone )
            {
            const TLinAddr chunkBase = aHeap.ChunkBase(); 
            const TLinAddr chunkMaxAddr = chunkBase + OSAdaption().DChunk().GetMaxSize(aHeap.Chunk());
            TRACE( Kern::Printf("DMemSpyDriverLogChanHeapDataBase::DoGetFullData - chunkBase:    0x%08x", chunkBase) );

            TLinAddr readAddress = aParams.iReadAddress;
            if (aParams.iRemaining < 0 )
                {
                // Initial case, start from the bottom
                readAddress = chunkBase;
                aParams.iRemaining = aHeap.Helper()->CommittedSize();
                }

            // The remaining number of bytes should allow us to calculate the position
            // to read from.
            TInt amountToRead = Min( aParams.iRemaining, destMax );
            TRACE( Kern::Printf("DMemSpyDriverLogChanHeapDataBase::DoGetFullData - amountToRead:           %8d", amountToRead) );

            // Do the read from the heap we are spying on into MemSpy's address space
            do
                {
                r = Kern::ThreadRawRead( aThread, (const void*)readAddress, destPtr, amountToRead );
                TRACE( Kern::Printf("DMemSpyDriverLogChanHeapDataBase::DoGetFullData - reading %d bytes starting at 0x%08x result: %d", 
                                     amountToRead, readAddress, r) );

                if (r == KErrBadDescriptor)
                    {
                    // This is not necessarily an error - it could be we've hit an unmapped page
                    if (amountToRead > KPageSize)
                        {
                        // retry reading a single page instead
                        amountToRead = KPageSize;
                        }
                    else
                        {
                        // Try the next page
                        readAddress += KPageSize;
                        }
                    }
                } while (r == KErrBadDescriptor && readAddress < chunkMaxAddr);
            //
            if  (r == KErrNone)
                {
                // Client takes care of updating descriptor length.
                r = amountToRead;
                }

            // Update remaining bytes
            aParams.iRemaining -= amountToRead;
            aParams.iReadAddress = readAddress;
            }
        }
    return r;
    }
