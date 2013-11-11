/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   WlanHeap declaration
*
*/

/*
* %version: 4 %
*/

#ifndef WLAN_OSA_HEAP_H
#define WLAN_OSA_HEAP_H

class RWlanHeap
    {

public:
    struct SCell {
         /**
         The length of the cell, which includes the length of
         this header.
         */
         TInt len; 
         
         
         /**
         A pointer to the next cell in the free list.
         */
         SCell* next;
         };
                        
    enum {EAllocCellSize = sizeof(TInt)};
    
    /**
    Size of a free cell header.
    */
    enum {EFreeCellSize = sizeof(SCell)};
    
    /**
    @internalComponent
    */
    struct _s_align {char c; double d;};
    
    /** 
    The default cell alignment.
    */
    enum {ECellAlignment = sizeof(_s_align)-sizeof(double)};
    
public:
    static RWlanHeap* FixedHeap(
            TAny* aBase, 
            TInt aMaxLength,
            TInt aAllocationUnit );
public:
    /**
    Allocates a cell of the specified size from the heap.

    If there is insufficient memory available on the heap from which to allocate
    a cell of the required size, the function returns NULL.

    The cell is aligned according to the alignment value specified at construction,
    or the default alignment value, if an explict value was not specified.

    The resulting size of the allocated cell may be rounded up to a
    value greater than aSize, but is guaranteed to be not less than aSize.

    @param aSize The 
    size of the cell to be allocated from the heap

    @return A pointer to the allocated cell. NULL if there is insufficient memory 
            available.
           
    @see KMaxTInt        
    */
    TAny* Alloc(TInt aSize);

    /**
    Frees the specified cell and returns it to the heap.

    @param aCell A pointer to a valid cell; this pointer can also be NULL,
                 in which case the function does nothing and just returns.
    */
    void Free(TAny* aPtr);

    /**
    Gets the total free space currently available on the heap and the space 
    available in the largest free block.

    The space available represents the total space which can be allocated.

    Note that compressing the heap may reduce the total free space available and 
    the space available in the largest free block.

    @param aBiggestBlock On return, contains the space available 
                         in the largest free block on the heap.
                         
    @return The total free space currently available on the heap.
    */
    TInt Available(TInt& aBiggestBlock) const;
    
private:

    RWlanHeap(TInt aInitialSize, TInt aAllocationUnit );
    
    TAny* operator new(TUint aSize, TAny* aBase);
    
    void Initialise();
    
    /**
    As much as possible, check a cell address and backspace it
    to point at the cell header.
    */   
    RWlanHeap::SCell* GetAddress(const TAny* aCell) const;
    
    /**
    Gets the length of the available space in the specified allocated cell.

    @param aCell A pointer to the allocated cell.

    @return The length of the available space in the allocated cell.

    */    
    TInt AllocLen(const TAny* aCell) const;

    /**
    Allocate without growing. aSize includes cell header and alignment.
    */
    RWlanHeap::SCell* DoAlloc(TInt aSize, SCell*& aLastFree);
    
    void DoFree(SCell* pC);

    /**
    Find the free cell that immediately follows aCell, if one exists
    If found, aNext is set to point to it, else it is set to NULL.
    aPrev is set to the free cell before aCell or the dummy free cell where there are no free cells before aCell.
    Called with lock enabled.
    */
    void FindFollowingFreeCell(SCell* aCell, SCell*& aPrev, SCell*& aNext);

    /**
    Try to grow the heap cell 'aCell' in place, to size 'aSize'.
    Requires the free cell immediately after aCell (aNext), and the free cell prior to
    that (aPrev), to be provided.  (As found by FindFollowingFreeCell)
    */
    TInt TryToGrowCell(SCell* aCell,SCell* aPrev, SCell* aNext, TInt aSize);
    
private:
    
    TInt iMinLength;
    TInt iMaxLength;
    TInt iOffset;
    TInt iGrowBy;
    TInt iChunkHandle;
    TUint8* iBase;
    TUint8* iTop;
    TInt iAlign;
    TInt iMinCell;
    TInt iPageSize;
    SCell iFree;
    
    };


#endif
