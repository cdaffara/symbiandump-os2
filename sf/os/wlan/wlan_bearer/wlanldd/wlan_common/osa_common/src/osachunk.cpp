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
* Description:   WlanChunk implementation
*
*/

/*
* %version: 10 %
*/

#include "osa_includeme.h"

#include <wlanosa.h>

#include "osachunk.h"
#include "osaheap.h"

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
WlanChunk::WlanChunk( 
    TUint8* aStartOfBuf, 
    TUint8* aEndOfBuf, 
    TInt aAllocationUnit )
    {
    iHeap = RWlanHeap::FixedHeap(
            aStartOfBuf, 
            aEndOfBuf - aStartOfBuf,
            aAllocationUnit );
    
    TraceDump(INFO_LEVEL | MEMCHUNK, 
        (("[WLAN] WlanChunk ctor -: 0x%08x"), this));
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
WlanChunk::~WlanChunk()
    {
    TraceDump(INFO_LEVEL | MEMCHUNK, 
        (("[WLAN] WlanChunk dtor +: 0x%08x"), this));

    TraceDump(INFO_LEVEL | MEMCHUNK, 
        (("[WLAN] WlanChunk dtor -: 0x%08x"), this));
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void* WlanChunk::Alloc( TInt aSize, TBool aZeroStamp )
    {
    TraceDump(INFO_LEVEL | MEMCHUNK | MEMCHUNK_ALLOC, 
        (("[WLAN] WlanChunk::Alloc + this: 0x%08x"), this));
    TraceDump(MEMCHUNK_ALLOC, 
        (("[WLAN] allocation request size in bytes: %d"), aSize));

    TAny* ptr = iHeap->Alloc( aSize );
    
    if ( ptr && aZeroStamp )
        {
        memset( ptr, 0, aSize );
        }
    TraceDump(INFO_LEVEL | MEMCHUNK | MEMCHUNK_ALLOC, 
        (("[WLAN] WlanChunk::Alloc - this: 0x%08x"), this));
    
    return ptr;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanChunk::Free( void* aPtr )
    {
    iHeap->Free( aPtr );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt WlanChunk::MaxFreeLinkSize() const
    {
    TraceDump(INFO_LEVEL | MEMCHUNK , 
        (("[WLAN] WlanChunk::MaxFreeLinkSize +- this: 0x%08x"), this));

    TInt available;
    iHeap->Available( available );
    return available;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool WlanChunk::IsInUse() const
    {
    TraceDump(INFO_LEVEL | MEMCHUNK , 
        (("[WLAN] WlanChunk::IsInUse +- this: 0x%08x"), this));

    return ETrue; 
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt WlanChunk::HeaderSize() const
    {
    return RWlanHeap::EAllocCellSize;
    }
