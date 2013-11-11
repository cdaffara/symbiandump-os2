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

#include "MemSpyDriverHeapWalker.h"

// User includes
#include "MemSpyDriverUtils.h"

// Defines
#define PRINTDEBUG( a ) { if ( PrintDebug() ) a; }


RMemSpyDriverHeapWalker::RMemSpyDriverHeapWalker(RMemSpyDriverRHeapBase& aHeap, MMemSpyHeapWalkerObserver* aObserver)
	: iHeap(aHeap), iPrintDebug(EFalse), iObserver(aObserver)
	{
	InitialiseStats();
	}


TInt RMemSpyDriverHeapWalker::Traverse()
//
// Walk the heap calling the info function.
//
	{
    PRINTDEBUG( Kern::Printf("RMemSpyDriverHeapWalker::Traverse() - START"));
    InitialiseStats();
    if  ( iObserver )
        {
        PRINTDEBUG( Kern::Printf("RMemSpyDriverHeapWalker::Traverse() - heap walk init..." ));
        iObserver->HandleHeapWalkInit();
        }

    PRINTDEBUG( Kern::Printf("RMemSpyDriverHeapWalker::Traverse() - heap walk init complete" ));

	TInt err = iHeap.Helper()->Walk(&CellCallback, this);
    FinaliseStats();
    //PRINTDEBUG( Kern::Printf("RMemSpyDriverHeapWalker::Traverse() - END - pF: 0x%08x, pC: 0x%08x, heapBase: 0x%08x, heapTop: 0x%08x", pF, pC, heapBase, heapTop));
	return err;
	}

TBool RMemSpyDriverHeapWalker::CellCallback(RAllocatorHelper& aHelper, TAny* aContext, RAllocatorHelper::TExtendedCellType aCellType, TLinAddr aCellAddress, TInt aLength)
	{
	return static_cast<RMemSpyDriverHeapWalker*>(aContext)->DoCellCallback(aHelper, aCellType, aCellAddress, aLength);
	}

TBool RMemSpyDriverHeapWalker::DoCellCallback(RAllocatorHelper& aHelper, RAllocatorHelper::TExtendedCellType aCellType, TLinAddr aCellAddress, TInt aLength)
	{
	TAny* cellAddress = (TAny*)aCellAddress;
	TMemSpyDriverCellType memspyCellType = (TMemSpyDriverCellType)aCellType; // We make sure these use the same values
	switch (aCellType)
		{
		case RAllocatorHelper::EHeapBadFreeCellAddress:
			PRINTDEBUG(Kern::Printf("RMemSpyDriverHeapWalker::Traverse() - EBadFreeCellAddress: 0x%08x", cellAddress));
			NotifyCell(memspyCellType, cellAddress, 0);
			return EFalse;
		case RAllocatorHelper::EHeapBadFreeCellSize:
			PRINTDEBUG(Kern::Printf("RMemSpyDriverHeapWalker::Traverse() - EBadFreeCellSize: 0x%08x", cellAddress));
			NotifyCell(memspyCellType, cellAddress, aLength);
			return EFalse;
		case RAllocatorHelper::EHeapBadAllocatedCellSize:
			PRINTDEBUG(Kern::Printf("RMemSpyDriverHeapWalker::Traverse() - EBadAllocatedCellSize: 0x%08x", cellAddress));
			NotifyCell(memspyCellType, cellAddress, aLength);
			return EFalse;
		case RAllocatorHelper::EHeapBadAllocatedCellAddress:
			PRINTDEBUG(Kern::Printf("RMemSpyDriverHeapWalker::Traverse() - EBadAllocatedCellAddress: 0x%08x", cellAddress));
			NotifyCell(memspyCellType, cellAddress, aLength);
			return EFalse;
		default:
			break;
		}

	if (aCellType & RAllocatorHelper::EAllocationMask)
		{
		PRINTDEBUG(Kern::Printf("RMemSpyDriverHeapWalker::Traverse() - EGoodAllocatedCell: 0x%08x", cellAddress));
		TInt nestingLevel = -1;
		aHelper.GetCellNestingLevel(cellAddress, nestingLevel);
		TInt allocCount = aHelper.AllocCountForCell(cellAddress);
		if (allocCount < 0) allocCount = -1; // This is what NotifyCell expects
		return NotifyCell(memspyCellType, cellAddress, aLength, nestingLevel, allocCount);
		}
	else if (aCellType & RAllocatorHelper::EFreeMask)
		{
		PRINTDEBUG( Kern::Printf("RMemSpyDriverHeapWalker::Traverse() - EGoodFreeCell: 0x%08x", cellAddress));
		return NotifyCell(memspyCellType, cellAddress, aLength);
		}
	else if (aCellType & RAllocatorHelper::EBadnessMask)
		{
		NotifyCell(memspyCellType, cellAddress, aLength);
		return EFalse;
		}
	return ETrue; // For any new types that get added
	}


void RMemSpyDriverHeapWalker::CopyStatsTo( TMemSpyHeapStatisticsRHeap& aStats )
    {
	PRINTDEBUG( Kern::Printf("RMemSpyDriverHeapWalker::CopyStatsTo() - START"));

    // Copy free cell info
    TMemSpyHeapStatisticsRHeapFree& free = aStats.StatsFree();
    free.SetTypeCount( iStats.iFreeCellCount );
    free.SetTypeSize( iStats.iTotalFreeSpace );

    // If the last cell was a free cell, and it was also the largest cell
    // then we use the prior largest free cell instead. This is because
    // slack space is already reported separately.
    TAny* largestFreeCellAddress = (TAny*) iStats.iLargestCellAddressFree;
    TUint largestFreeCellSize = iStats.iLargestCellSizeFree;
    if ( iStats.iLastCellWasFreeCell && iStats.iLargestCellSizeFree == iStats.iSlackSpace && iStats.iSpackSpaceCellAddress == iStats.iLargestCellAddressFree )
        {
	    PRINTDEBUG( Kern::Printf("RMemSpyDriverHeapWalker::CopyStatsTo() - using previous max free cell stats, since largest free cell is slack cell at end of heap..."));
        largestFreeCellAddress = (TAny*) iStats.iLargestCellAddressFreePrevious;
        largestFreeCellSize = iStats.iLargestCellSizeFreePrevious;
        }

    free.SetLargestCellAddress( largestFreeCellAddress );
    free.SetLargestCellSize( largestFreeCellSize );
    free.SetSlackSpaceCellSize( iStats.iSlackSpace );
    free.SetSlackSpaceCellAddress( (TAny*) iStats.iSpackSpaceCellAddress );
    free.SetChecksum( iStats.iFreeCellCRC );

    // Copy allocated cell info
    TMemSpyHeapStatisticsRHeapAllocated& alloc = aStats.StatsAllocated();
    alloc.SetTypeCount( iStats.iAllocCellCount );
    alloc.SetTypeSize( iStats.iTotalAllocSpace );
    alloc.SetLargestCellAddress( (TAny*) iStats.iLargestCellAddressAlloc );
    alloc.SetLargestCellSize( iStats.iLargestCellSizeAlloc );

	aStats.iCommittedFreeSpace = iHeap.Helper()->CommittedFreeSpace();

	PRINTDEBUG( Kern::Printf("RMemSpyDriverHeapWalker::CopyStatsTo() - END"));
    }


void RMemSpyDriverHeapWalker::SetObserver( MMemSpyHeapWalkerObserver* aObserver )
    {
    PRINTDEBUG( Kern::Printf("RMemSpyDriverHeapWalker::SetObserver() - aObserver: 0x%08x", aObserver ));
    iObserver = aObserver;
    }

TBool RMemSpyDriverHeapWalker::NotifyCell( TMemSpyDriverCellType aType, TAny* aCellAddress, TInt aLength, TInt aNestingLevel, TInt aAllocNumber )
    {
    // Update stats first
    UpdateStats( aType, aCellAddress, aLength, aNestingLevel, aAllocNumber );
	
    // Notify observer
    TBool continueTraversal = ETrue;
    if  ( iObserver )
        {
        continueTraversal = iObserver->HandleHeapCell( aType, aCellAddress, aLength, aNestingLevel, aAllocNumber );
        }
    //
    return continueTraversal;
    }


void RMemSpyDriverHeapWalker::UpdateStats( TMemSpyDriverCellType aCellType, TAny* aCellAddress, TInt aLength, TInt aNestingLevel, TInt aAllocNumber )
    {
    PRINTDEBUG( Kern::Printf("RMemSpyDriverHeapWalker::UpdateStats - type: %d address: 0x%08x, len: %8d, nestingLev: %8d, allocNum: %8d", aCellType, aCellAddress, aLength, aNestingLevel, aAllocNumber ));

    if (aCellType & EMemSpyDriverFreeCellMask)
        {
        // Update checksum
        iStats.iFreeCellCRC = iStats.iFreeCellCRC ^ reinterpret_cast<TUint32>( aCellAddress );

        // Track cell counts and length
        ++iStats.iFreeCellCount;
        iStats.iTotalFreeSpace += aLength;
        iStats.iLastFreeCellLength = aLength;

        PRINTDEBUG( Kern::Printf("RMemSpyDriverHeapWalker::UpdateStats - WAS FREE CELL - iFreeCellCRC: 0x%08x, iFreeCellCount: %d, iTotalFreeSpace: %d, iLastFreeCellLength: %d", iStats.iFreeCellCRC, iStats.iFreeCellCount, iStats.iTotalFreeSpace, iStats.iLastFreeCellLength));
        
        // Identify biggest cell
        if  ( (TUint) aLength > iStats.iLargestCellSizeFree )
            {
            PRINTDEBUG( Kern::Printf("RMemSpyDriverHeapWalker::UpdateStats - this cell (%d bytes big) is bigger than previous largested FREE cell (%d bytes) => making it the new largest FREE cell", aLength, iStats.iLargestCellSizeFree));
            iStats.iLargestCellSizeFreePrevious = iStats.iLargestCellSizeFree;
            iStats.iLargestCellSizeFree = aLength;
            iStats.iLargestCellAddressFreePrevious = iStats.iLargestCellAddressFree;
            iStats.iLargestCellAddressFree = (TLinAddr) aCellAddress;
            }

        // Identify first cell
        if  ( iStats.iFirstFreeCellAddress == 0 )
            {
            iStats.iFirstFreeCellLength = aLength;
            iStats.iFirstFreeCellAddress = (TLinAddr) aCellAddress;
            }
        }
    else if (aCellType & EMemSpyDriverAllocatedCellMask)
        {
        // Track cell counts and length
        ++iStats.iAllocCellCount;
        iStats.iTotalAllocSpace += aLength;
        iStats.iLastFreeCellLength = 0; 

        PRINTDEBUG( Kern::Printf("RMemSpyDriverHeapWalker::UpdateStats - WAS ALLOC CELL - iAllocCellCount: %d, iTotalAllocSpace: %d", iStats.iAllocCellCount, iStats.iTotalAllocSpace));

        // Identify biggest cell
        if  ( (TUint) aLength > iStats.iLargestCellSizeAlloc )
            {
            PRINTDEBUG( Kern::Printf("RMemSpyDriverHeapWalker::UpdateStats - this cell (%d bytes big) is bigger than previous largested ALLOC cell (%d bytes) => making it the new largest ALLOC cell", aLength, iStats.iLargestCellSizeAlloc));
            iStats.iLargestCellSizeAlloc = aLength;
            iStats.iLargestCellAddressAlloc = (TLinAddr) aCellAddress;
            }
        }

    iStats.iLastCellType = aCellType;
    iStats.iLastCellAddress = (TLinAddr) aCellAddress;
    iStats.iLastCellWasFreeCell = (aCellType & EMemSpyDriverFreeCellMask);
    ++iStats.iNumberOfWalkedCells;
    }


void RMemSpyDriverHeapWalker::InitialiseStats()
    {
    iStats.iFreeCellCRC = 0;
    iStats.iNumberOfWalkedCells = 0;
    iStats.iFirstFreeCellAddress = 0;
    iStats.iFirstFreeCellLength = 0;
    iStats.iLastCellType = EMemSpyDriverAllocatedCellMask;
    iStats.iLastCellWasFreeCell = EFalse;
    iStats.iLastFreeCellLength = 0;
    iStats.iTotalFreeSpace = 0;
    iStats.iTotalAllocSpace = 0;
    iStats.iSlackSpace = 0;
    iStats.iFreeCellCount = 0;
    iStats.iAllocCellCount = 0;
    iStats.iLargestCellSizeFree = 0;
    iStats.iLargestCellSizeAlloc = 0;
    iStats.iLargestCellAddressFree = 0;
    iStats.iLargestCellAddressAlloc = 0;
    iStats.iLargestCellSizeFreePrevious = 0;
    iStats.iLargestCellAddressFreePrevious = 0;
    iStats.iSpackSpaceCellAddress = 0;
    iStats.iLastCellAddress = 0;
    }


void RMemSpyDriverHeapWalker::FinaliseStats()
    {
    if  ( iStats.iLastCellWasFreeCell )
        {
        iStats.iSlackSpace = iStats.iLastFreeCellLength;
        iStats.iSpackSpaceCellAddress = iStats.iLastCellAddress;
        }

    PrintStats();
    }


void RMemSpyDriverHeapWalker::PrintStats()
    {
    PRINTDEBUG( Kern::Printf("RMemSpyDriverHeapWalker::PrintStats - HEAP SUMMARY FOR THREAD:" ) );
    PRINTDEBUG( Kern::Printf("RMemSpyDriverHeapWalker::PrintStats - ------------------------------------------------------------" ) );
    PRINTDEBUG( Kern::Printf("RMemSpyDriverHeapWalker::PrintStats - iNumberOfWalkedCells         : %10d", iStats.iNumberOfWalkedCells ) );
    PRINTDEBUG( Kern::Printf("RMemSpyDriverHeapWalker::PrintStats - iFirstFreeCellAddress        : 0x%08x", iStats.iFirstFreeCellAddress ) );
    PRINTDEBUG( Kern::Printf("RMemSpyDriverHeapWalker::PrintStats - iFirstFreeCellLength         : %10d", iStats.iFirstFreeCellLength ) );
    PRINTDEBUG( Kern::Printf("RMemSpyDriverHeapWalker::PrintStats - iLastCellWasFreeCell         : %10d", iStats.iLastCellWasFreeCell ) );
    PRINTDEBUG( Kern::Printf("RMemSpyDriverHeapWalker::PrintStats - iLastCellType                : %10d", iStats.iLastCellType ) );
    PRINTDEBUG( Kern::Printf("RMemSpyDriverHeapWalker::PrintStats - iLastFreeCellLength          : %10d", iStats.iLastFreeCellLength ) );
    PRINTDEBUG( Kern::Printf("RMemSpyDriverHeapWalker::PrintStats - iTotalFreeSpace              : %10d", iStats.iTotalFreeSpace ) );
    PRINTDEBUG( Kern::Printf("RMemSpyDriverHeapWalker::PrintStats - iTotalAllocSpace             : %10d", iStats.iTotalAllocSpace ) );
    PRINTDEBUG( Kern::Printf("RMemSpyDriverHeapWalker::PrintStats - iSlackSpace                  : %10d", iStats.iSlackSpace ) );
    PRINTDEBUG( Kern::Printf("RMemSpyDriverHeapWalker::PrintStats - iFreeCellCount               : %10d", iStats.iFreeCellCount ) );
    PRINTDEBUG( Kern::Printf("RMemSpyDriverHeapWalker::PrintStats - iAllocCellCount              : %10d", iStats.iAllocCellCount ) );
    PRINTDEBUG( Kern::Printf("RMemSpyDriverHeapWalker::PrintStats - iLargestCellSizeFree         : %10d", iStats.iLargestCellSizeFree ) );
    PRINTDEBUG( Kern::Printf("RMemSpyDriverHeapWalker::PrintStats - iLastFreeCellLength          : %10d", iStats.iLastFreeCellLength ) );
    PRINTDEBUG( Kern::Printf("RMemSpyDriverHeapWalker::PrintStats - iLargestCellSizeAlloc        : %10d", iStats.iLargestCellSizeAlloc ) );
    PRINTDEBUG( Kern::Printf("RMemSpyDriverHeapWalker::PrintStats - iLargestCellAddressFree      : 0x%08x", iStats.iLargestCellAddressFree ) );
    PRINTDEBUG( Kern::Printf("RMemSpyDriverHeapWalker::PrintStats - iLargestCellAddressAlloc     : 0x%08x", iStats.iLargestCellAddressAlloc ) );
    PRINTDEBUG( Kern::Printf("RMemSpyDriverHeapWalker::PrintStats - iFreeCellCRC                 : 0x%08x", iStats.iFreeCellCRC ) );
    }
