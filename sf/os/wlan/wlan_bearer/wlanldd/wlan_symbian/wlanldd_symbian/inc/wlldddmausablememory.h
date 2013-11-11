/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Declaration of the WlanDmaUsableMemory class.
*
*/

/*
* %version: 7 %
*/

#ifndef C_WLANDMAUSABLEMEMORY_H
#define C_WLANDMAUSABLEMEMORY_H

class DPlatChunkHw;

/**
 *  Encapsulates DMA usable memory
 *
 *  @since S60 v3.1
 */
class WlanDmaUsableMemory 
    {

public:

    WlanDmaUsableMemory() : 
      iSizeInBytes( 0 ), iPhysicalRamAddr( 0 ), iFlags( 0 ), iChunk( NULL ) {};

    /**
     * Init method of the class
     *
     * @since S60 3.1
     * @param aSizeInBytes size of memory in bytes required 
     * @param aUseCachedMemory ETrue if cached memory shall be used
     *                         EFalse otherwise
     * @return if not 0 -> actual size of memory acquired 
     * (rounded to a multiple of page size).
     * operation failure upon 0 
     */
    TUint32 Init( TUint32 aSizeInBytes, TBool aUseCachedMemory );

    /**
     * Finit method of the class
     * Can be called even when the init of the class failed
     *
     * @since S60 3.1
     */
    void Finit();

    /**
     * Get the linear address of the memory buffer 
     *
     * @since S60 3.1
     * @param aSizeInBytes size of memory in bytes required 
     * @returns begin of the memory that can be used for DMA xfer 
     */
    TLinAddr Addr( TUint32 aSizeInBytes );

    /**
     * Get the linear address of the memory buffer 
     *
     * @since S60 3.1
     * @returns begin of the memory that can be used for DMA xfer 
     */
    TLinAddr Addr();

protected:

private:
    
    // Prohibit copy constructor.
    WlanDmaUsableMemory ( const WlanDmaUsableMemory& );
    // Prohibit assigment operator.
    WlanDmaUsableMemory& operator= ( const WlanDmaUsableMemory& );

private: // data

    static const TUint32 KResourceAcquired = (1 << 0);

    /**
     * size of memory in bytes
     */
    TInt iSizeInBytes;

    /**
     * physical memory address
     */
    TPhysAddr iPhysicalRamAddr;

    /**
     * internal flags
     */
    TUint32 iFlags;

    /**
     * the actual chunk of memory
     */
    DPlatChunkHw* iChunk;

    };


#endif // C_WLANDMAUSABLEMEMORY_H
