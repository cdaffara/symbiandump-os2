/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   WlanHeap implementation
*
*/

/*
* %version: 3 %
*/

#include <kernel.h>
#include <e32cmn.h>
#include "osaheap.h"

#define __NEXT_CELL(p)              ((SCell*)(((TUint8*)p)+p->len))

#define __ALIGN_X( _x,_y) (((_x) + ((_y) - 1)) & (~((_y) - 1)))

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
RWlanHeap* RWlanHeap::FixedHeap(
        TAny* aBase, 
        TInt aInitialSize,
        TInt aAllocationUnit )
    {
    return new(aBase) RWlanHeap(aInitialSize, aAllocationUnit );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
RWlanHeap::RWlanHeap(TInt aMaxLength, TInt aAlign )
    :   iMinLength(aMaxLength), iMaxLength(aMaxLength), iOffset(0), iGrowBy(0), iChunkHandle(0)
    {
    iAlign = aAlign ? aAlign : ECellAlignment;
    iPageSize = 0;
    Initialise();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TAny* RWlanHeap::operator new(TUint aSize, TAny* aBase)
    {
    RWlanHeap* h = (RWlanHeap*)aBase;
    h->iAlign = 0x80000000; // garbage value
    h->iBase = ((TUint8*)aBase) + aSize;
    return aBase;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void RWlanHeap::Initialise()
    {
    
    // Base address must be aligned to AllocationUnit
    iBase = (TUint8*)__ALIGN_X((TUint32)iBase + EAllocCellSize, iAlign );
    TInt b = iBase - ((TUint8*)this - iOffset);
    TInt len = _ALIGN_DOWN(iMinLength - b, iAlign);
    iTop = iBase + len;
    iMinLength = iTop - ((TUint8*)this - iOffset);
    
    // Min cell size equals to allocationUnit
    iMinCell = iAlign;
    
    SCell* pM = (SCell*)iBase; // First free cell
    iFree.next = pM; // Free list points to first free cell
    iFree.len = 0; // Stop free from joining this with a free block
    pM->next = NULL; // Terminate the free list
    pM->len = len; // Set the size of the free cell
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TAny* RWlanHeap::Alloc(TInt aSize)
    {
    // The size of allocatoted block MUST be aligned to allocationUnit
    aSize = Max(__ALIGN_X(aSize + EAllocCellSize, iAlign), iMinCell );
    
    SCell* pL = NULL;
    SCell* pC = (SCell*)DoAlloc(aSize, pL);

    if (pC)
        {
        TAny* result = ((TUint8*)pC) + EAllocCellSize;
        return result;
        }
    return NULL;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
UEXPORT_C void RWlanHeap::Free(TAny* aCell)
    {
    if (!aCell)
        {
        return;
        }
    SCell* pC = GetAddress(aCell);
    DoFree(pC);

    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
UEXPORT_C RWlanHeap::SCell* RWlanHeap::GetAddress(const TAny* aCell) const
    {
    SCell* pC = (SCell*)(((TUint8*)aCell)-EAllocCellSize);
    return pC;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
UEXPORT_C TInt RWlanHeap::AllocLen(const TAny* aCell) const
    {

    SCell* pC = GetAddress(aCell);
    return pC->len - EAllocCellSize;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
RWlanHeap::SCell* RWlanHeap::DoAlloc(TInt aSize, SCell*& aLastFree)
    {
    SCell* pP = &iFree;
    SCell* pC = pP->next;
    for (; pC; pP=pC, pC=pC->next) // Scan the free list
        {
        SCell* pE;
        if (pC->len >= aSize)               // Block size bigger than request
            {
            if (pC->len - aSize < iMinCell) // Leftover must be large enough to hold an SCell
                {
                aSize = pC->len;            // It isn't, so take it all
                pE = pC->next;              // Set the next field
                }
            else
                {
                pE = (SCell*)(((TUint8*)pC)+aSize); // Take amount required
                pE->len = pC->len - aSize;  // Initialize new free cell
                pE->next = pC->next;
                }
            pP->next = pE;                  // Update previous pointer
            pC->len = aSize;                // Set control size word
            return pC;
            }
        }
    aLastFree = pP;
    return NULL;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void RWlanHeap::DoFree(SCell* pC)
    {
    SCell* pP = &iFree;
    SCell* pE = pP->next;
    for (; pE && pE<pC; pP=pE, pE=pE->next) {}
    if (pE)         // Is there a following free cell?
        {
        SCell* pN = __NEXT_CELL(pC);
        if (pN==pE) // Is it adjacent
            {
            pC->len += pE->len; // Yes - coalesce adjacent free cells
            pC->next = pE->next;
            }
        else                    // pN<pE, non-adjacent free cells
            {
            pC->next = pE;      // Otherwise just point to it
            }
        }
    else
        {
        pC->next = NULL;        // No following free cell
        }
    SCell* pN = __NEXT_CELL(pP);    // pN=pP=&iFree if no preceding free cell
    if (pN==pC) // Is it adjacent
        {
        pP->len += pC->len;     // Yes - coalesce adjacent free cells
        pP->next = pC->next;
        pC = pP;                // for size reduction check
        }
    else                        // pN<pC, non-adjacent free cells
        {
        pP->next = pC;          // point previous cell to the one being freed
        }
    pN = __NEXT_CELL(pC);       // End of amalgamated free cell
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void RWlanHeap::FindFollowingFreeCell(SCell* aCell, SCell*& aPrev, SCell*& aNext)
    {
    aPrev = &iFree;
    aNext = aPrev->next;
    for (; aNext && aNext < aCell; aPrev = aNext, aNext = aNext->next ) {} 
    
    if (aNext) // If there is a following free cell, check its directly after aCell.
        {
            SCell* pNextCell = __NEXT_CELL(aCell);          // end of this cell
            if (pNextCell!= aNext) 
                {
                aNext = NULL;     
                }
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt RWlanHeap::TryToGrowCell(SCell* aCell,SCell* aPrev, SCell* aNext, TInt aSize)
    {
    TInt extra = aSize - aCell->len;
    if (aNext && (aNext->len >= extra)) // Is there a following free cell big enough?
        {
        if (aNext->len - extra >= iMinCell) // take part of free cell ?
            {
            SCell* pX = (SCell*)((TUint8*)aNext + extra);   // remainder of free cell
            pX->next = aNext->next;         // remainder->next = original free cell->next
            pX->len = aNext->len - extra;       // remainder length = original free cell length - extra
            aPrev->next = pX;                   // put remainder into free chain
            }
        else
            {
            extra = aNext->len;                 // Take whole free cell
            aPrev->next = aNext->next;          // remove from free chain
            }
     
        aCell->len += extra;                    // update reallocated cell length
        
        return KErrNone;
        }
    return KErrGeneral;  // No space to grow cell
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt RWlanHeap::Available(TInt& aBiggestBlock) const
    {

    TInt total = 0;
    TInt max = 0;
    SCell* pC = iFree.next;
    for (; pC; pC=pC->next)
        {
        TInt l = pC->len - EAllocCellSize;
        if (l > max)
            {
            max = l;
            }
        total += l;
        }
    aBiggestBlock = max;
    return total;
    }


