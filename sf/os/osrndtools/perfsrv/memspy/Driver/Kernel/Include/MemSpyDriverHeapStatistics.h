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

#ifndef MEMSPYDRIVERHEAPSTATISTICS_H
#define MEMSPYDRIVERHEAPSTATISTICS_H

// System includes
#include <e32cmn.h>


class TMemSpyHeapWalkStatistics
    {
public:

    // Total number of cells enumerated
    TInt iNumberOfWalkedCells;

    // The type of the last cell
    TMemSpyDriverCellType iLastCellType;

    // The address of the last cell
    TLinAddr iLastCellAddress;

public: // Free cell related

    // The address of the first free heap cell
    TLinAddr iFirstFreeCellAddress;
    
    // The length of the first free heap cell
    TUint iFirstFreeCellLength;

    // Whether the last cell in the entire heap was a free cell
    // (i.e. slack space)
    TBool iLastCellWasFreeCell;

    // The length of the last free cell in the heap
    TUint iLastFreeCellLength;

    // The total amount of free space in the heap
    TUint iTotalFreeSpace;

    // The number of free heap cells
    TUint iFreeCellCount;

    // The size of the largest free cell
    TUint iLargestCellSizeFree;

    // The previous largest free cell size
    TUint iLargestCellSizeFreePrevious;

    // The address of the largest free cell
    TLinAddr iLargestCellAddressFree;

    // The previous address of the largest free cell
    TUint iLargestCellAddressFreePrevious;

    // The overhead associated with a free cell (header length)
    //TUint iFreeCellOverheadHeaderLength;
	TUint iReserved1;

    // The slace space at the end of the heap
    TUint iSlackSpace;

    // The address of the slack space cell
    TLinAddr iSpackSpaceCellAddress;

    // A very simple XOR checksum of all the free cell addresses that were
    // detected during the heap navigation
    TUint32 iFreeCellCRC;

public: // Alloc cell related

    // The number of allocated heap cells
    TUint iAllocCellCount;

    // The size of the largest allocated cell
    TUint iLargestCellSizeAlloc;

    // The address of the largest allocated cell
    TLinAddr iLargestCellAddressAlloc;
   
    // The overhead associated with an allocated cell (header length)
    //TUint iAllocCellOverheadHeaderLength;
	TUint iReserved2;

public: // Common

    // The total amount of allocated space in the heap
    TUint iTotalAllocSpace;
    };



#endif
