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
* Description:   WlanMemoryPool implementation
*
*/

/*
* %version: 8 %
*/

#include "osa_includeme.h"

#include <wlanosa.h>

#include "osamemorypool.h"
#include "osachunkbase.h"

// allocator for general purpose memory
extern void* GpAlloc( TInt aSize, TBool aZeroStamp );
// free for general purpose memory
extern void GpFree( void* aPtr );

// don't derive form DBase as we use placement new
struct SMemHeader
    {
#ifndef NDEBUG
    enum  { KMagic = 0xBEEFCACE };
    TInt  iMagicHead;
#endif // !NDEBUG

    TInt  iHwChunkId;

    explicit SMemHeader( TInt aHwChunkId = 0 ) : 
        #ifndef NDEBUG
        iMagicHead( KMagic ),
        #endif // NDEBUG
        iHwChunkId( aHwChunkId )
            {}; 

private:

    // Prohibit copy constructor.
    SMemHeader( const SMemHeader& );
    // Prohibit assigment operator.
    SMemHeader& operator= ( const SMemHeader& );
    };

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
WlanMemoryPool::WlanMemoryPool( TBool aUseCachedMemory, TInt aAllocationUnit )
    : 
    iUseCachedMemory( aUseCachedMemory ), 
    iAllocationUnit( aAllocationUnit ),
    iExtraPadding( 0 ) // actual value is determined later
    {
    TraceDump(INFO_LEVEL | MEMORYPOOL, 
        (("[WLAN] WlanMemoryPool ctor +: 0x%08x"), this));

    const TBool ret( InitPool() );
    if ( ret )
        {
        Validate();
        }
    else
        {
        TraceDump(ERROR_LEVEL, ("[WLAN] error: allocation"));
        Trace( ERROR_LEVEL, 
            reinterpret_cast<const TInt8*>(WLAN_FILE), __LINE__ );

        InValidate();
        }

    TraceDump(INFO_LEVEL | MEMORYPOOL, 
        (("[WLAN] WlanMemoryPool ctor -: 0x%08x"), this));
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
WlanMemoryPool::~WlanMemoryPool()
    {
    TraceDump(INFO_LEVEL | MEMORYPOOL, 
        (("[WLAN] WlanMemoryPool dtor +: 0x%08x"), this));
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void* WlanMemoryPool::Alloc( 
    MWlanOsa::TOsaMemoryType aOsaMemoryType, 
    TInt aSize,
    TBool aZeroStamp )
    {
    TraceDump(INFO_LEVEL | MEMORYPOOL, 
        (("[WLAN] WlanMemoryPool::Alloc +: 0x%08x"), this));
    TraceDump(MEMORYPOOL, (("[WLAN] memory type: %d"), aOsaMemoryType));
    TraceDump(MEMORYPOOL, (("[WLAN] size: %d"), aSize));

    TUint8* addr = NULL;

    // length of our memory header
    const TInt KOurHdrLen ( sizeof( SMemHeader ) );
    // actual size is the requested size + our header
    TInt alloc_size ( aSize + KOurHdrLen );     

    if ( MWlanOsa::ENormalMemory == aOsaMemoryType )
        {
        // normal allocation requested
        
        addr = static_cast<TUint8*>(GpAlloc( alloc_size, aZeroStamp ));
        if ( addr )
            {
            // set header 
            new (addr) SMemHeader;
            // set the address to be returned to the requester 
            addr += KOurHdrLen;
            }
        else
            {
            // failed: left intentionally empty
            }
        }
    else if ( MWlanOsa::EInterconnectMemory == aOsaMemoryType )
        {
        // hw pool allocation requested

        // in this case we need to make sure that the memory buffer start
        // address returned to the requester is aligned to allocation unit
        // boundary. We do this by adding the necessary number of padding 
        // bytes to the beginning of the allocated memory buffer
        
        alloc_size += iExtraPadding;     

        TInt hw_chunk_id( 0 );
        addr = static_cast<TUint8*>(HwChunkAlloc( 
            alloc_size, hw_chunk_id, aZeroStamp ));
        if ( addr )
            {
            // set header. The extra padding is before our header
            new (addr + iExtraPadding) SMemHeader( hw_chunk_id );
            // set the address to be returned to the requester 
            addr += iExtraPadding + KOurHdrLen;
            }
        else
            {
            // failed: left intentionally empty
            }
        }
    else
        {
        // unknown type
        TraceDump(CRIT_LEVEL, (("[WLAN] critical: type: %d"), aOsaMemoryType));
        MWlanOsa::Assert( reinterpret_cast<const TInt8*>(WLAN_FILE), __LINE__ );
        }

    if ( addr )
        {
        TraceDump(MEMORYPOOL, 
            (("[WLAN] address to be returned to client: 0x%08x"), addr));
        }
    else
        {
        TraceDump(ERROR_LEVEL, ("[WLAN] error: allocation"));
        Trace( ERROR_LEVEL, 
            reinterpret_cast<const TInt8*>(WLAN_FILE), __LINE__ );
        }

    TraceDump(INFO_LEVEL | MEMORYPOOL, 
        (("[WLAN] WlanMemoryPool::Alloc -: 0x%08x"), this));

    return addr;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanMemoryPool::Free( void* aPtr )
    {
    TraceDump(INFO_LEVEL | MEMORYPOOL, 
        (("[WLAN] WlanMemoryPool::Free +: 0x%08x"), this));
    TraceDump(MEMORYPOOL, (("[WLAN] free address: 0x%08x"), aPtr));

    SMemHeader* hdr = ((static_cast<SMemHeader*>(aPtr)) - 1);

    TraceDump(MEMORYPOOL, (("[WLAN] header free address: 0x%08x"), hdr));

#ifndef NDEBUG
    // validate magic 
    MWlanOsa::Assert( reinterpret_cast<const TInt8*>(WLAN_FILE), __LINE__,
        (SMemHeader::KMagic == hdr->iMagicHead) );
#endif // !NDEBUG

    if ( !(hdr->iHwChunkId) )
        {        
        GpFree( static_cast<void*>(hdr) );
        }
    else
        {
        HwChunkFree( *hdr );
        }

    TraceDump(INFO_LEVEL | MEMORYPOOL, 
        (("[WLAN] WlanMemoryPool::Free -: 0x%08x"), this));
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanMemoryPool::InitPool()
    {
    TraceDump(INFO_LEVEL | MEMORYPOOL, 
        (("[WLAN] WlanMemoryPool::InitPool +: 0x%08x"), this));

    TBool ret( EFalse );

    // create the initial hw chunk
    WlanPlatformHwChunk* chunk = new WlanPlatformHwChunk( 
        KInitialHwChunkSize, 
        iUseCachedMemory,
        iAllocationUnit );
    if ( chunk )
        {
        if ( chunk->IsValid() )
            {
            // chunk is valid
            HwChunkHandle handle( *chunk );
            if ( handle->IsValid() )
                {
                // handle is valid
                ret = PushBack( handle );
                if ( ret )
                    {
                    // push success
                    ret = ETrue;

                    // determine the number of extra padding bytes needed on 
                    // top of our memory header length for interconnect 
                    // memory type so that the memory buffer start address 
                    // returned to memory requester is always aligned to 
                    // allocation unit boundary
                    
                    const TInt KNextLevelHdrLen = 
                        (handle->Chunk()).HeaderSize();                    
                    const TInt KRemainder ( 
                        ( static_cast<TInt>(sizeof( SMemHeader )) 
                          + KNextLevelHdrLen )
                        % iAllocationUnit );                    
                    iExtraPadding = KRemainder ? 
                        ( iAllocationUnit - KRemainder ) : KRemainder;                    
                    TraceDump(INFO_LEVEL | MEMORYPOOL, 
                        (("[WLAN] Extra padding: %d"), iExtraPadding));
                    }
                else
                    {
                    // push failure: left intentionally empty
                    }
                }
            else
                {
                // handle is invalid: left intentionally empty
                }
            }
        else
            {
            // chunk is invalid: left intentionally empty
            }
        }
    else
        {
        // allocation failure: left intentionally empty
        TraceDump(ERROR_LEVEL, ("[WLAN] error: allocation"));
        Trace( ERROR_LEVEL, 
            reinterpret_cast<const TInt8*>(WLAN_FILE), __LINE__ );
        }

    if ( !ret )
        {
        // something went wrong
        delete chunk;
        }
    else
        {
        // left intentionally empty
        }

    TraceDump(INFO_LEVEL | MEMORYPOOL, 
        (("[WLAN] WlanMemoryPool::InitPool -: 0x%08x"), this));

    return ret;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanMemoryPool::PushBack( const HwChunkHandle& aHwChunkHandle )
    {
    TraceDump(INFO_LEVEL | MEMORYPOOL, 
        (("[WLAN] WlanMemoryPool::PushBack +: 0x%08x"), this));

    TBool ret( EFalse );
    const size_type size( iHwChunkHandles.size() );

    iHwChunkHandles.push_back( aHwChunkHandle );
    if ( size != iHwChunkHandles.size() )
        {
        // size difference prior and after push
        // so it must have been a success
        ret = ETrue;
        }
    else
        {
        // push failure :left intentionally empty
        TraceDump(ERROR_LEVEL, ("[WLAN] error: allocation"));
        Trace( ERROR_LEVEL, 
            reinterpret_cast<const TInt8*>(WLAN_FILE), __LINE__ );
        }

    TraceDump(INFO_LEVEL | MEMORYPOOL, 
        (("[WLAN] WlanMemoryPool::PushBack -: 0x%08x"), this));

    return ret;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void* WlanMemoryPool::HwChunkAllocWithCreate( const TInt aSize, 
                                              TInt& aHwChunkId,
                                              TBool aZeroStamp )
    {
    TraceDump(INFO_LEVEL | MEMORYPOOL, 
        (("[WLAN] WlanMemoryPool::HwChunkAlloc +: 0x%08x"), this));

    // initial allocation try is times 1.4 the requested size
    const TInt KInitialAllocSize( (aSize * 14) / 10 ); 
    // try initial allocation amount + the requested amount times
    const TInt KAllocationCounts( 2 );

    const TInt allocation_size[KAllocationCounts] 
        = { KInitialAllocSize, aSize };

    const TInt* pos = allocation_size;  // start of sequence
    // last of sequence
    const TInt* const end = &( allocation_size[KAllocationCounts] );

    WlanPlatformHwChunk* chunk = NULL;

    // loop in order to acquire a chunk for us
    do
        {
        chunk = new WlanPlatformHwChunk( 
            *pos, 
            iUseCachedMemory, 
            iAllocationUnit );
        if ( chunk )
            {
            if ( chunk->IsValid() )
                {
                // ok
                break;
                }
            else
                {
                // invalid chunk: 
                delete chunk;
                chunk = NULL;
                }
            }
        else
            {
            // allocation failure: left intentionally empty
            TraceDump(ERROR_LEVEL, ("[WLAN] error: allocation"));
            Trace( ERROR_LEVEL, 
                reinterpret_cast<const TInt8*>(WLAN_FILE), __LINE__ );
            }

        ++pos;
        } while ( pos < end );

    void* addr = NULL;

    if ( chunk )
        {
        // chunk acquire success
        // must also be valid at this point
        MWlanOsa::Assert( 
            reinterpret_cast<const TInt8*>(WLAN_FILE), __LINE__,
            (chunk->IsValid()) );
        
        HwChunkHandle handle( *chunk );
        if ( handle.IsValid() )
            {
            if ( PushBack( handle ) )
                {
                // allocation guaranteed as
                // chunk final creation success
                addr = chunk->Chunk().Alloc( 
                    aSize, aZeroStamp );
                MWlanOsa::Assert( 
                    reinterpret_cast<const TInt8*>(WLAN_FILE), __LINE__,
                    addr != NULL );

                // set he hw chunk id for the caller
                aHwChunkId = reinterpret_cast<TInt>(chunk);
                }
            else
                {
                // PushBack failure: left intentionally empty
                }
            }
        else
            {
            // invalid handle: left intentionally empty
            }

        if ( !addr )
            {
            // if we don't have an address at this point 
            // we don't need a chunk either
            delete chunk;
            }
        else
            {
            // everything okay: left intentionally empty
            }
        }
    else
        {
        // chunk acquire failure: left intentionally empty
        }

    TraceDump(INFO_LEVEL | MEMORYPOOL, 
        (("[WLAN] WlanMemoryPool::HwChunkAlloc -: 0x%08x"), this));

    return addr;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void* WlanMemoryPool::HwChunkAlloc( const TInt aSize, 
                                    TInt& aHwChunkId, 
                                    TBool aZeroStamp )
    {
    TraceDump(INFO_LEVEL | MEMORYPOOL, 
        (("[WLAN] WlanMemoryPool::HwChunkAlloc +: 0x%08x"), this));

    // lets search a chunk for us
    const_iterator end( iHwChunkHandles.end() );
    iterator pos( iHwChunkHandles.begin() );

    void* addr = NULL;

    // always atleast one exists in the sequence
    MWlanOsa::Assert( 
        reinterpret_cast<const TInt8*>(WLAN_FILE), __LINE__,
        (!(iHwChunkHandles.empty())) );

    do 
        {
        addr = ((*pos)->Chunk()).Alloc( aSize, aZeroStamp );
        if ( addr )
            {
            break;
            }
        else
            {
            // left intentionally empty
            }
            
        ++pos;
        } while ( pos != end );

    if ( addr )
        {
        // allocation success: 
        // set the hw chunk id
        aHwChunkId = reinterpret_cast<TInt>((*pos).Data());
        }
    else
        {
        // failed to allocate
        // we shall now create a new chunk for allocation
        addr = HwChunkAllocWithCreate( aSize, aHwChunkId, aZeroStamp );
        }

    TraceDump(INFO_LEVEL | MEMORYPOOL, 
        (("[WLAN] WlanMemoryPool::HwChunkAlloc -: 0x%08x"), this));

    return addr;
    }

// ---------------------------------------------------------------------------
// in the front of the sequence we always have our initial chunk,
// the lifetime of which is to be equal to the lifetime of this object
// ---------------------------------------------------------------------------
//
inline 
TBool WlanMemoryPool::IsInitialChunk( WlanPlatformHwChunk* aChunk )
    {
    return ( aChunk == (iHwChunkHandles.front().Data()) );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanMemoryPool::HwChunkFree( SMemHeader& aMemHeader )
    {
    TraceDump(INFO_LEVEL | MEMORYPOOL, 
        (("[WLAN] WlanMemoryPool::HwChunkFree +: 0x%08x"), this));
    
    // extract correct hw_chunk
    WlanPlatformHwChunk* hw_chunk 
        = reinterpret_cast<WlanPlatformHwChunk*>(aMemHeader.iHwChunkId);

    // deallocate; note the extra padding before our memory header 
    hw_chunk->Chunk().Free( 
        reinterpret_cast<TUint8*>(&aMemHeader) - iExtraPadding );

    if ( IsInitialChunk( hw_chunk ) )
        {
        // is the initial chunk: left intentionally empty
        }
    else
        {

        // not the initial chunk
        if ( !(hw_chunk->Chunk().IsInUse()) )
            {
            // the chunk is not inuse, this meaning nothing
            // has been allocated from there, which means we can erase it

            TraceDump(MEMORYPOOL, ("[WLAN] WlanMemoryPool erase unused chunk"));

            // lets found the critter and erase it from the sequnce

            iterator pos( iHwChunkHandles.begin() );
            const_iterator end( iHwChunkHandles.end() );

            // always atleast one exists in the sequence
            MWlanOsa::Assert( 
                reinterpret_cast<const TInt8*>(WLAN_FILE), __LINE__,
                (!(iHwChunkHandles.empty())) );

            do
                {
                if ( ((*pos).Data()) == hw_chunk )
                    {
                    // found a match
                    break;
                    }
                else
                    {
                    // no match: left intentionally empty
                    }

                ++pos;
                } while ( pos != end );

            MWlanOsa::Assert( 
                reinterpret_cast<const TInt8*>(WLAN_FILE), __LINE__,
                (pos != end) );

            // erase the critter
            iHwChunkHandles.erase( pos );    
            
            }
        else
            {
            // the chunk is in use
            // left intentionally empty
            }
        }

    TraceDump(INFO_LEVEL | MEMORYPOOL, 
        (("[WLAN] WlanMemoryPool::HwChunkFree -: 0x%08x"), this));
    }
