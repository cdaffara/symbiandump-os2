/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   WlanMemoryPool declaration
*
*/

/*
* %version: 6 %
*/

#ifndef WLANMEMORYPOOL_H
#define WLANMEMORYPOOL_H

#include "wlanobject.h"

#include "osaplatformhwchunk.h"
#include "osalist.h"
#include "osahandle.h"

struct SMemHeader;

/**
 *  memory pool for managing memory
 *
 *
 *  @lib wlanosa.lib
 *  @since S60 v3.2
 */
class WlanMemoryPool : public DBase, public WlanObject
    {

    typedef Handle<WlanPlatformHwChunk> HwChunkHandle;

public:

    /**
     * ctor
     * NOTE: call IsValid() member from super class 
     * to validate object after ctor
     *
     * @since S60 v3.2
     * @param aUseCachedMemory ETrue if cached memory shall be used
     *                         EFalse otherwise
     * @param aAllocationUnit allocation unit size for interconnect memory in
     *        bytes
     */
    explicit WlanMemoryPool( TBool aUseCachedMemory, TInt aAllocationUnit );

	/**
	 * Destructor.
	 *
	 * @since S60 v3.2
	 */
    virtual ~WlanMemoryPool();

    /**
     * Memory allocation. 
     * Correct alignment guaranteed
     *
     * @since S60 v3.2
     * @param aOsaMemoryType memory type to be allocated
     * @param aSize size of the buffer to be allocated in bytes. 
     *              Must be positive otherwise the allocation fails
     * @param aZeroStamp zero stamp the memory or not
     * @return begin of the allocated memory, NULL upon failure
     */
    void* Alloc( MWlanOsa::TOsaMemoryType aOsaMemoryType, 
                 TInt aSize, 
                 TBool aZeroStamp );

    /**
     * Releases memory allocated by the Alloc method
     *
     * @since S60 v3.2
     * @param aPtr begin of the buffer to be freed
     */
    void Free( void* aPtr );

private:

    /**
     * Init pool
     *
     * @since S60 v3.2
     * @return ETrue upon success any other for failure
     */
    TBool InitPool();

    /**
     * Appends hw chunk handle to the back of the list
     *
     * @since S60 v3.2
     * @param aHwChunkHandle hw chunk handle to be appended
     * @return ETrue upon success any other for failure
     */
    TBool PushBack( const HwChunkHandle& aHwChunkHandle );

    /**
     * Creates a new hw chunk and allocates memory from it
     *
     * @since S60 v3.2
     * @param aSize size of the buffer to be allocated in bytes. 
     *              Must be positive otherwise the allocation fails 
     * @param aHwChunkId unique chunk identifier used when freeing the memory
     * @param aZeroStamp zero stamp memory or not
     * @return begin of the allocated memory, NULL upon failure
     */
    void* HwChunkAllocWithCreate( const TInt aSize, 
                                  TInt& aHwChunkId, 
                                  TBool aZeroStamp );

    /**
     * Allocates memory from a hw chunk
     *
     * @since S60 v3.2
     * @param aSize size of the buffer to be allocated in bytes. Must be 
     *              positive otherwise the allocation fails 
     * @param aHwChunkId unique chunk identifier used when freeing the memory
     * @param aZeroStamp zero stamp memory or not
     * @return begin of the allocated memory, NULL upon failure
     */
    void* HwChunkAlloc( const TInt aSize, TInt& aHwChunkId, TBool aZeroStamp );

    /**
     * Evaluates is a chunk the initial chunk. 
     * The intial chunk is the one created at object creation time and its 
     * lifetime is equal to the lifetime of this object itself
     *
     * @since S60 v3.2
     * @param aChunk chunk to be evaluated against the intial chunk
     * @return ETrue when aChunk is the intial chunk, any other if not
     */
    inline TBool IsInitialChunk( WlanPlatformHwChunk* aChunk );

    /**
     * Frees memory from a hw chunk
     *
     * @since S60 v3.2
     * @param aMemHeader begin of the buffer to be freed
     */
    void HwChunkFree( SMemHeader& aMemHeader );

    // Prohibit copy constructor.
    WlanMemoryPool( const WlanMemoryPool& );
    // Prohibit assigment operator.
    WlanMemoryPool& operator= ( const WlanMemoryPool& );

private: // data

    typedef list<HwChunkHandle> HwChunkHandleList;

    typedef HwChunkHandleList::iterator iterator;
    typedef HwChunkHandleList::const_iterator const_iterator;
    typedef HwChunkHandleList::size_type size_type;

    /**
     * list of hw chunks
     */
    HwChunkHandleList   iHwChunkHandles;

    /** 
    * ETrue if cached memory shall be used,
    * EFalse otherwise 
    */
    TBool iUseCachedMemory;

    /** 
    * Allocation unit size in bytes 
    */
    TInt iAllocationUnit;
    
    /** 
    * Number of extra bytes required to align interconnect memory buffer 
    * start address returned to the requester to allocation unit boundary
    */
    TInt iExtraPadding;
    };

#endif // WLANMEMORYPOOL_H
