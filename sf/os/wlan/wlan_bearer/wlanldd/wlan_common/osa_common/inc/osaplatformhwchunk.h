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
* Description:   WlanPlatformHwChunk declaration
*
*/

/*
* %version: 6 %
*/

#ifndef WLANPLATFORMHWCHUNK_H
#define WLANPLATFORMHWCHUNK_H

#include "wlanobject.h"

class MWlanOsaChunkBase;
struct WlanPlatformHwChunkImpl;

/**
 *  platform memory chunk for memory suitable for DMA xfer
 *
 *
 *  @lib wlanosa.lib
 *  @since S60 v3.2
 */
class WlanPlatformHwChunk : public DBase, public WlanObject
    {

    friend class WlanPlatformHwChunkImpl;

public:


    /**
     * ctor
     * NOTE: call IsValid() member from super class 
     * to validate object after ctor
     *
     * @since S60 v3.2
     * @param aSizeInBytes size of the chunk to be created
     * @param aUseCachedMemory ETrue if cached memory shall be used
     *        EFalse otherwise
     * @param aAllocationUnit allocation unit size in bytes
     */
    explicit WlanPlatformHwChunk( 
        TInt aSizeInBytes, 
        TBool aUseCachedMemory, 
        TInt aAllocationUnit );

	/**
	 * Destructor.
	 *
	 * @since S60 v3.2
	 */
    virtual ~WlanPlatformHwChunk();

    /**
     * Returns the chunk managed
     *
     * @since S60 v3.2
     * @return the chunk managed
     */
    MWlanOsaChunkBase& Chunk();

    /**
     * Returns the chunk managed
     *
     * @since S60 v3.2
     * @return the chunk managed
     */
    const MWlanOsaChunkBase& Chunk() const;

private:

    /**
     * Rounds up memory request size to physical page size 
     *
     * @since S60 v3.2
     * @param size rounded up
     */
    static inline TInt RoundToPageSize( TInt aSizeInBytes );

    /**
     * Allocate physical ram
     *
     * @since S60 v3.2
     * @param aSizeInBytes amount of physical ram to be allocated
     * @param aPhysAddr (out) physical address of the memory allocated 
     * @return ETrue upon success, any other for failure
     */
    static inline TBool AllocatePhysicalRam( 
        TInt aSizeInBytes, TPhysAddr& aPhysAddr );

    /**
     * Frees the physical ram allocated
     *
     * @since S60 v3.2
     * @param aPhysRamAddr physical address of the memory to be freed 
     * @param aPhysRamSize size of the physical ram to be freed
     */
    static inline void FreePhysicalRam( 
        TPhysAddr aPhysRamAddr, TInt aPhysRamSize );


    /**
     * Allocate a hw chunk
     *
     * @since S60 v3.2
     * @param aPhysRamAddr physical address of the 
     * memory to be associuated with the chunk 
     * @param aPhysRamSize size of the memory
     * @param aAllocationUnit allocation unit size in bytes
     * @return ETrue upon success, any other for failure
     */
    inline TBool AllocateHardwareChunk( 
        TPhysAddr aPhysRamAddr, 
        TInt aPhysRamSize,
        TInt aAllocationUnit );

    /**
     * Extract implementation details
     *
     * @since S60 v3.2
     * @return implementation details
     */
    inline WlanPlatformHwChunkImpl& Pimpl();

    /**
     * Extract implementation details
     *
     * @since S60 v3.2
     * @return implementation details
     */
    inline const WlanPlatformHwChunkImpl& Pimpl() const;

    // deny copying.
    WlanPlatformHwChunk( const WlanPlatformHwChunk& );

    // deny assigment.
    WlanPlatformHwChunk& operator= ( const WlanPlatformHwChunk& );

private: // data

    /**
     * implementation
     * Own.  
     */
    WlanPlatformHwChunkImpl* iPimpl;    
    
    /** 
    * ETrue if cached memory shall be used,
    * EFalse otherwise 
    */
    TBool iUseCachedMemory;
    };

#endif // WLANPLATFORMHWCHUNK_H
