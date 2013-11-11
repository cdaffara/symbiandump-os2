/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of the WlanDmaUsableMemory class.
*
*/

/*
* %version: 11 %
*/

#include "WlLddWlanLddConfig.h"
#include "wlldddmausablememory.h"

#include <platform.h>

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TUint32 WlanDmaUsableMemory::Init( 
    TUint32 aSizeInBytes, 
    TBool aUseCachedMemory )
    {
    TraceDump(INIT_LEVEL, (("WLANLDD: init dma usable memory")));
    TraceDump(INIT_LEVEL, (("size requested: %d"), aSizeInBytes));

    // extract MMU page size in bytes
	iSizeInBytes = Kern::RoundToPageSize( aSizeInBytes );

    TraceDump(INIT_LEVEL, (("MMU page size: %d"), iSizeInBytes));

    // allocate a block of physically contiguous RAM  
    TInt ret( Epoc::AllocPhysicalRam( iSizeInBytes, iPhysicalRamAddr ) );
    if ( ret != KErrNone )
        {
        // this can happen if we have  no memory
        TraceDump(ERROR_LEVEL, (("WLANLDD: physical RAM alloc failure: %d"), 
            ret));
        iSizeInBytes = 0;
        return 0;
        }

    TraceDump(INIT_LEVEL, (("physical RAM address: 0x%08x"), iPhysicalRamAddr));

    // determine if cached memory shall be used
    TUint cacheOption = aUseCachedMemory ? 
        EMapAttrCachedMax : 
        EMapAttrFullyBlocking;

    // use a DPlatChunk
    ret = DPlatChunkHw::New( iChunk, iPhysicalRamAddr,
                          iSizeInBytes,
                          EMapAttrUserRw | cacheOption );
    if ( ret != KErrNone )
        {
        // can not fail
        TraceDump(ERROR_LEVEL, (("WLANLDD: DPlatChunkHw alloc failure: %d"), ret));
        os_assert( (TUint8*)("WLANLDD: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
        return 0;
        }

    // init success
    TraceDump(INIT_LEVEL, 
        (("WlanDmaUsableMemory::Init: Platform Hw Chunk create success with cacheOption: 0x%08x"), 
        cacheOption));
    iFlags |= KResourceAcquired;
    return iSizeInBytes;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanDmaUsableMemory::Finit()
    {
    if ( iFlags & KResourceAcquired )
        {
        TraceDump(INIT_LEVEL, (("WLANLDD: finit dma usable memory")));

        iFlags &= ~KResourceAcquired;
        // close the chunk. This also automatically deletes the object 
        // as its reference count goes to 0
        iChunk->Close( NULL );

        // free the physical ram which was associated with the chunk
        const TInt ret( Epoc::FreePhysicalRam( 
            iPhysicalRamAddr, iSizeInBytes ) );

        iPhysicalRamAddr = 0;
        iSizeInBytes = 0;

#ifndef NDEBUG
        if ( ret != KErrNone )
            {
            // can not fail
            TraceDump(ERROR_LEVEL, (("WLANLDD: physical RAM free failure: %d"), ret));
            os_assert( (TUint8*)("WLANLDD: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
            }
#endif        
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TLinAddr WlanDmaUsableMemory::Addr( 
    TUint32 aSizeInBytes )
    {
    return (aSizeInBytes <= iSizeInBytes) 
        ? iChunk->LinearAddress() : NULL;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TLinAddr WlanDmaUsableMemory::Addr()
    {
    return iChunk->LinearAddress();
    }
