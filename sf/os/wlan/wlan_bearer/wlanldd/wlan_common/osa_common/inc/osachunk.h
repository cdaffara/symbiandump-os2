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
* Description:   WlanChunk declaration
*
*/

/*
* %version: 6 %
*/

#ifndef WLANCHUNK_H
#define WLANCHUNK_H

#include "wlanobject.h"

#include "osalist.h"
#include "osachunkbase.h"

class RWlanHeap;

/**
 *  Memory chunk allocation management object.
 *
 *  Management of memory is based on buddy system, where righ hand side (rhs)
 *  and left hand side (lhs) memory links are coalesced (if they are free) to
 *  the current link to be freed. Allocation size is always alinged to
 *  allocation unit, which equals to the size of cache line. The pointer
 *  returned to the caller is also aligned to the allocation unit.
 *
 */
class WlanChunk : 
    public DBase, public WlanObject, public MWlanOsaChunkBase
    {
public:

    /**
     * ctor
     * NOTE: call IsValid() member from super class 
     * to validate object after ctor
     *
     * @since S60 v3.2
     * @param aStartOfBuf start of buffer
     * @param aEndOfBuf 1 past end of buffer
     * @param aAllocationUnit allocation unit size in bytes
     */
    WlanChunk( TUint8* aStartOfBuf, TUint8* aEndOfBuf, TInt aAllocationUnit );

    /**
	 * Destructor.
	 *
	 * @since S60 v3.2
	 */
    virtual ~WlanChunk();

// from base class MWlanOsaChunkBase

    /**
     * From MWlanOsaChunkBase.
     * Memory allocation. Correct alignment guaranteed
     *
     * @since S60 v3.2
     * @param aSize size of the buffer to be allocated in bytes. 
     *              Must be positive otherwise the allocation fails
     * @param aZeroStamp zero stamp memory or not
     * @return begin of the allocated memory, NULL upon failure
     */
    virtual void* Alloc( TInt aSize, TBool aZeroStamp );

    /**
     * From MWlanOsaChunkBase.
     * Releases memory allocated by the Alloc method
     *
     * @since S60 v3.2
     * @param aPtr begin of the buffer to be freed
     */
    virtual void Free( void* aPtr );

    /**
     * From MWlanOsaChunkBase.
     * Returns the maximum free link size
     *
     * @since S60 v3.2
     * @return maximum free link size
     */
    virtual TInt MaxFreeLinkSize() const;

    /**
     * From MWlanOsaChunkBase.
     * Evaluates is chunk in use or not, 
     * meaning is memory allocated from there or not
     *
     * @since S60 v3.2
     * @return ETrue for unused chunk any other for used chunk
     */
    virtual TBool IsInUse() const;

    /**
     * From MWlanOsaChunkBase.
     * Returns the size of the memory chunk meta header in bytes
     *
     * @since S60 v3.2
     * @return size of the memory chunk meta header
     */
    virtual TInt HeaderSize() const;
    

    // Prohibit copy constructor.
    WlanChunk( const WlanChunk& );
    // Prohibit assigment operator.
    WlanChunk& operator= ( const WlanChunk& );

private: // data

    RWlanHeap* iHeap;
    
    };

#endif // WLANCHUNK_H
