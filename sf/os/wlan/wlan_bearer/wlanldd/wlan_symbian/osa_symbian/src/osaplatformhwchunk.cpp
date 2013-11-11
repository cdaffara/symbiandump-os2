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
* Description:   WlanPlatformHwChunk implementation
*
*/

/*
* %version: 8 %
*/

#include "osa_includeme.h"

#include <wlanosa.h>
#include <platform.h>

#include "osaplatformhwchunk.h"
#include "osachunk.h"

struct WlanPlatformHwChunkImpl : public DBase, public WlanObject
    {

    /**
     * Ctor
     *
     * @since S60 v3.2
     * @param aStartOfBuf begin of the memory buffer
     * @param aEndOfBuf 1 past end of the memory buffer
     * @param aPlatChunkHw platform chunk object
     * @param aPhysRamAddr physical ram address
     * @param aPhysRamSize size of the physical ram
     * @param aAllocationUnit size of the allocation unit in bytes
     */
    WlanPlatformHwChunkImpl( 
        TUint8* aStartOfBuf, 
        TUint8* aEndOfBuf, 
        DPlatChunkHw** aPlatChunkHw, 
        TPhysAddr aPhysRamAddr, 
        TInt aPhysRamSize,
        TInt aAllocationUnit );

    /**
     * Dtor
     *
     * @since S60 v3.2
     */
    virtual ~WlanPlatformHwChunkImpl();

    /**
     * Deallocate acquired resources (if any) 
     *
     * @since S60 v3.2
     */
    inline void Dispose();

    /**
     * the one and only chunk
     * Own.
     */
    WlanChunk*      iChunk;

    /**
     * the one and only platform chunk
     * Own.
     */
    DPlatChunkHw*   iPlatChunk;

    /**
     * physical ram address
     */
    TPhysAddr       iPhysRamAddr;

    /**
     * physical ram size
     */
    TInt            iPhysRamSize;
    
private:
        
    // Prohibit copy constructor.
    WlanPlatformHwChunkImpl( const WlanPlatformHwChunkImpl& );
    // Prohibit assigment operator.
    WlanPlatformHwChunkImpl& operator= ( 
        const WlanPlatformHwChunkImpl& );        
    };

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TInt WlanPlatformHwChunk::RoundToPageSize(  TInt aSizeInBytes  )
    {
    TraceDump(INFO_LEVEL, ("[WLAN] WlanPlatformHwChunk::RoundToPageSize +"));

    // extract MMU page size in bytes
	aSizeInBytes = Kern::RoundToPageSize( aSizeInBytes );

    TraceDump(INFO_LEVEL, ("[WLAN] WlanPlatformHwChunk::RoundToPageSize -"));

    return aSizeInBytes;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TBool WlanPlatformHwChunk::AllocatePhysicalRam( TInt aSizeInBytes, 
                                                       TPhysAddr& aPhysAddr )
    {
    TBool ret( ETrue );
    TraceDump(INFO_LEVEL, 
        ("[WLAN] WlanPlatformHwChunk::AllocatePhysicalRam +"));

    const TInt ret_code( Epoc::AllocPhysicalRam( aSizeInBytes, aPhysAddr ) );
    if ( ret_code != KErrNone )
    {
    // this can happen if we have  no memory
    ret = EFalse;
    TraceDump(ERROR_LEVEL, (("[WLAN] error: allocation: %d"), ret_code));
    Trace( ERROR_LEVEL, 
        reinterpret_cast<const TInt8*>(WLAN_FILE), __LINE__ );
    }

    TraceDump(INFO_LEVEL, 
        ("[WLAN] WlanPlatformHwChunk::AllocatePhysicalRam -"));

    return ret;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanPlatformHwChunk::FreePhysicalRam( TPhysAddr aPhysRamAddr, 
                                                  TInt aPhysRamSize )
    {
    TraceDump(INFO_LEVEL, ("[WLAN] WlanPlatformHwChunk::FreePhysicalRam +"));

    const TInt ret = Epoc::FreePhysicalRam( aPhysRamAddr, aPhysRamSize );
    if ( ret != KErrNone )
        {
        // no valid use case exists for this code path to be taken
        TraceDump(CRIT_LEVEL, (("[WLAN] critical: value: %d"), ret));
        MWlanOsa::Assert( 
            reinterpret_cast<const TInt8*>(WLAN_FILE), __LINE__ );
        }
    else
        {
        // left intentionally empty
        }

    TraceDump(INFO_LEVEL, ("[WLAN] WlanPlatformHwChunk::FreePhysicalRam -"));
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
WlanPlatformHwChunkImpl::WlanPlatformHwChunkImpl( 
    TUint8* aStartOfBuf, 
    TUint8* aEndOfBuf,
    DPlatChunkHw** aPlatChunkHw,
    TPhysAddr aPhysRamAddr, 
    TInt aPhysRamSize,
    TInt aAllocationUnit )
    : iChunk( NULL ), iPlatChunk( *aPlatChunkHw ), 
    iPhysRamAddr( aPhysRamAddr ), iPhysRamSize( aPhysRamSize )
    {
    TraceDump(INFO_LEVEL, 
        (("[WLAN] WlanPlatformHwChunkImpl ctor +: 0x%08x"), this));

    iChunk = new WlanChunk( aStartOfBuf, aEndOfBuf, aAllocationUnit );
    if ( iChunk )
        {
        if ( iChunk->IsValid() )
            {
            Validate();     // mark as valid
            }
        else
            {
            TraceDump(ERROR_LEVEL, ("[WLAN] error: allocation"));
            Trace( ERROR_LEVEL, 
                reinterpret_cast<const TInt8*>(WLAN_FILE), __LINE__ );

            InValidate();   // mark as invalid

            // chunk deallocated in dtor so we don't have to do it here
            }
        }
    else
        {
        // allocation failure
        TraceDump(ERROR_LEVEL, ("[WLAN] error: allocation"));
        Trace( ERROR_LEVEL, 
            reinterpret_cast<const TInt8*>(WLAN_FILE), __LINE__ );

        InValidate();   // mark as invalid
        }

    TraceDump(INFO_LEVEL, 
        (("[WLAN] WlanPlatformHwChunkImpl ctor -: 0x%08x"), this));
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
WlanPlatformHwChunkImpl::~WlanPlatformHwChunkImpl()
    {
    TraceDump(INFO_LEVEL, 
        (("[WLAN] WlanPlatformHwChunkImpl dtor +: 0x%08x"), this));

    Dispose();

    TraceDump(INFO_LEVEL, 
        (("[WLAN] WlanPlatformHwChunkImpl dtor +: 0x%08x"), this));
    }

// ---------------------------------------------------------------------------
// only 1 call point. That's the reason for inlining
// ---------------------------------------------------------------------------
//
inline void WlanPlatformHwChunkImpl::Dispose()
    {
    TraceDump(INFO_LEVEL, 
        (("[WLAN] WlanPlatformHwChunkImpl::Dispose +: 0x%08x"), this));

    // NOTE: always dellocate the chunk prior freeing the memory associated to
    // it
    delete iChunk;
    iChunk = NULL;

    if ( iPlatChunk )
        {
        // close the chunk. This also automatically deletes the object 
        // as its reference count goes to 0
        iPlatChunk->Close( NULL );
        // free the physical ram which was associated with the chunk
        WlanPlatformHwChunk::FreePhysicalRam( iPhysRamAddr, iPhysRamSize );
        }
    else
        {
        // left intentionally empty
        }

    TraceDump(INFO_LEVEL, 
        (("[WLAN] WlanPlatformHwChunkImpl::Dispose -: 0x%08x"), this));
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline WlanPlatformHwChunkImpl& WlanPlatformHwChunk::Pimpl()
    {
    return *iPimpl;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline const WlanPlatformHwChunkImpl& WlanPlatformHwChunk::Pimpl() const
    {
    return *iPimpl;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
WlanPlatformHwChunk::WlanPlatformHwChunk( 
    TInt aSizeInBytes, 
    TBool aUseCachedMemory,
    TInt aAllocationUnit ) 
    : iPimpl( NULL ), iUseCachedMemory ( aUseCachedMemory )
    {
    TraceDump(INFO_LEVEL, 
        (("[WLAN] WlanPlatformHwChunk ctor +: 0x%08x"), this));

    TBool ret( EFalse );
    TPhysAddr phys_ram_addr( 0 );   // physical ram address

    TraceDump(PLAT_HW_CHUNK, 
        (("[WLAN] WlanPlatformHwChunk requested memory size: %d"), 
        aSizeInBytes));

    // round up request to MMU page size boundary
    const TInt phys_ram_size( RoundToPageSize( aSizeInBytes ) );

    TraceDump(PLAT_HW_CHUNK, 
        (("[WLAN] WlanPlatformHwChunk memory size to be aquired: %d"), 
        phys_ram_size));

    // allocate the physical ram
    ret = AllocatePhysicalRam( phys_ram_size, phys_ram_addr );
    
    if ( ret )
        {
        // physical ram allocation success
        // allocate hw chunk for the physical ram
        ret = AllocateHardwareChunk( 
                phys_ram_addr, 
                phys_ram_size, 
                aAllocationUnit );
        if ( ret )
            {
            // chunk creation success
            Validate();     // mark as valid
            }
        else
            {
            // chunk creation failure
            // free the physical ram 
            FreePhysicalRam( phys_ram_addr, phys_ram_size );
            InValidate();   // mark as invalid
            }
        }
    else
        {
        // physical ram allocation failure
        InValidate();   // mark as invalid
        }

    TraceDump(INFO_LEVEL, 
        (("[WLAN] WlanPlatformHwChunk ctor -: 0x%08x"), this));
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
WlanPlatformHwChunk::~WlanPlatformHwChunk()
    {
    TraceDump(INFO_LEVEL, 
        (("[WLAN] WlanPlatformHwChunk dtor +: 0x%08x"), this));

    delete iPimpl;

    TraceDump(INFO_LEVEL, 
        (("[WLAN] WlanPlatformHwChunk dtor -: 0x%08x"), this));
    }

// ---------------------------------------------------------------------------
// only 1 call point thats the reason for inlining
// ---------------------------------------------------------------------------
//
inline TBool WlanPlatformHwChunk::AllocateHardwareChunk( 
    TPhysAddr aPhysRamAddr, 
    TInt aPhysRamSize,
    TInt aAllocationUnit )
    {
    TraceDump(INFO_LEVEL, 
        (("[WLAN] WlanPlatformHwChunk::AllocateHardwareChunk +: 0x%08x"), 
        this));

    TBool ret( EFalse );

    // determine if cached memory shall be used
    TUint cacheOption = iUseCachedMemory ? 
        EMapAttrCachedMax : 
        EMapAttrFullyBlocking;

    // lets create a hw chunk for our physical ram
    DPlatChunkHw* chunk = NULL;
    TInt code = DPlatChunkHw::New( 
        chunk, aPhysRamAddr, aPhysRamSize, 
        EMapAttrSupRw | cacheOption );

    if ( KErrNone != code )
        {
        // creation error -> bail out
        TraceDump(ERROR_LEVEL, (("[WLAN] error: allocation %d"), code));
        Trace( ERROR_LEVEL, 
            reinterpret_cast<const TInt8*>(WLAN_FILE), __LINE__ );

        TraceDump(INFO_LEVEL, 
            (("[WLAN] WlanPlatformHwChunk::AllocateHardwareChunk -: 0x%08x"), 
            this));

        return ret;
        }
    else
        {
        TraceDump(PLAT_HW_CHUNK, 
            (("[WLAN] Platform Hw Chunk create success with cacheOption: 0x%08x"), 
            cacheOption));
        }

    // hw chunk creation success proceed
    iPimpl = new WlanPlatformHwChunkImpl( 
        reinterpret_cast<TUint8*>(chunk->LinearAddress()), 
        (reinterpret_cast<TUint8*>(chunk->LinearAddress())) + aPhysRamSize,
        &chunk,
        aPhysRamAddr,
        aPhysRamSize,
        aAllocationUnit );

    if ( iPimpl )
        {
        // success -> validate implementation
        if ( Pimpl().IsValid() )
            {
            Validate();     // we are valid to go
            ret = ETrue;
            }
        else
            {
            TraceDump(ERROR_LEVEL, ("[WLAN] error: allocation"));
            Trace( ERROR_LEVEL, 
                reinterpret_cast<const TInt8*>(WLAN_FILE), __LINE__ );

            InValidate();   // we are invalid to go

            // allways dealloc in dtor
            }       
        }
    else
        {
        // allocation failed -> trace
        TraceDump(ERROR_LEVEL, ("[WLAN] error: allocation"));
        Trace( ERROR_LEVEL, 
            reinterpret_cast<const TInt8*>(WLAN_FILE), __LINE__ );

        // we must now deallocate allocated resources
        // allways close the chunk
        // this also automatically deletes the object 
        // as its reference count goes to 0
        chunk->Close( NULL );
        }

    TraceDump(INFO_LEVEL, 
        (("[WLAN] WlanPlatformHwChunk::AllocateHardwareChunk -: 0x%08x"), 
        this));

    return ret;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
MWlanOsaChunkBase& WlanPlatformHwChunk::Chunk()
    {
    return *(Pimpl().iChunk);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
const MWlanOsaChunkBase& WlanPlatformHwChunk::Chunk() const
    {
    return *(Pimpl().iChunk);
    }
