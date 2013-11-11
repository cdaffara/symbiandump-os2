/*
* Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Declaration of the MgmtFrameMemMngr class.
*
*/

/*
* %version: 16 %
*/

#ifndef MGMTFRAMEMMNGR_H
#define MGMTFRAMEMMNGR_H

#include "EthernetFrameMemMngr.h"

class WlanChunk;


/**
*  Memory manager for management client frame Rx memory
*
*  @since S60 v3.1
*/
class MgmtFrameMemMngr : public DEthernetFrameMemMngr
    {

public:

    /** Ctor */
    MgmtFrameMemMngr( 
        DWlanLogicalChannel& aParent,
        WlanChunk*& aRxFrameMemoryPool,
        TBool aUseCachedMemory,
        TInt aRxFrameBufAllocationUnit ) : 
        DEthernetFrameMemMngr( aParent, aRxFrameMemoryPool ),
        iFrameXferBlock( NULL ),
        iChunkKernelAddr( 0 ),
        iTxDataBuffer( NULL ),
        iUseCachedMemory( aUseCachedMemory ),
        iRxFrameBufAllocationUnit ( aRxFrameBufAllocationUnit ),
        iChunkSize( 
            Kern::RoundToPageSize( 
                4096 * 15 + KProtocolStackSideTxDataChunkSize ) ) // bytes
        {};

    /** Dtor */
    virtual ~MgmtFrameMemMngr()
        {
        iFrameXferBlock = NULL;
        iTxDataBuffer = NULL;
        };

protected:

    /**
    * From DEthernetFrameMemMngr
    * Allocates a shared memory chunk for frame transfer between user
    * and kernel address spaces
    *
    * @since S60 3.1
    * @param aSharedMemoryChunk The shared memory chunk
    * @return system wide error code, KErrNone upon success
    */
    virtual TInt DoAllocate( DChunk*& aSharedMemoryChunk );

    /**
    * Opens a handle for user mode client to the shared memory chunk
    * allocated for frame transfer between user and kernel address spaces
    *
    * @since S60 3.1
    * @param aThread The user mode client thread
    * @param aSharedChunkInfo After successful return contains the handle to the
    *        chunk
    * @param aSharedMemoryChunk The shared memory chunk
    * @return system wide error code, KErrNone upon success
    */
    virtual TInt DoOpenHandle(
        DThread& aThread,
        TSharedChunkInfo& aSharedChunkInfo,
        DChunk* aSharedMemoryChunk );

    /**
    * From DEthernetFrameMemMngr
    * Gets a free rx buffer
    *
    * @since S60 3.1
    * @param aLengthinBytes Requested buffer length
    * @return buffer for Rx data upon success
    *         NULL otherwise
    */
    virtual TUint8* DoGetNextFreeRxBuffer( TUint aLengthinBytes );

    /**
    * From DEthernetFrameMemMngr
    * Gets called when rx frame read cycle has ended.
    *
    * @since S60 3.1
    * @param aBufferStart first element of the array that holds pointers to
    *        Rx frame meta headers
    * @param aNumOfBuffers number of meta header pointers in the array
    * @return ETrue if a pending user mode frame read request exists 
    *         and callee should complete it, 
    *         EFalse otherwise
    */
    virtual TBool DoEthernetFrameRxComplete( 
        TDataBuffer*& aBufferStart, 
        TUint32 aNumOfBuffers );

    /**
    * From DEthernetFrameMemMngr
    * Frees the specified Rx frame buffer
    *
    * @since S60 3.1
    * @param aBufferToFree The buffer to free
    */
    virtual void DoMarkRxBufFree( TUint8* aBufferToFree );
    
    /**
    * From DEthernetFrameMemMngr
    * To be called when user mode client issues a frame Rx request
    *
    * @return ETrue if callee should complete the request immediately 
    *         as there exists Rx frame(s) which can be retrieved by the user
    *         mode client.
    *         EFalse otherwise
    */
    virtual TBool OnReadRequest();
    
    /**
     * From DEthernetFrameMemMngr
     * Gets the next frame (contained in a buffer allocated from
     * the shared memory) from the Rx queue.
     * Optionally frees the memory associated to a previously received frame. 
     * 
     * @param aFrameToFree User space pointer to previously received frame 
     *        which can now be freed.
     *        NULL if nothing to free.
     * @return User space pointer to the Rx frame to be handled next.
     *         NULL, if there are no frames available.
     */ 
    virtual TDataBuffer* GetRxFrame( TDataBuffer* aFrameToFreeInUserSpace );

private:

    /**
    * Gets a memory block that can be used for tx frame write
    *
    * @since S60 3.1
    * @return memory block that can be used for tx frame write, 
    *         NULL upon failure
    */
    virtual TDataBuffer* OnWriteEthernetFrame() const;

    /**
    * Returns the number of extra bytes required to align Rx buffer start
    * address, to be returned to WHA layer, to allocation unit boundary
    * @return See above
    */
    TInt RxBufAlignmentPadding() const;
    
    /**
    * From DEthernetFrameMemMngr
    * Memory finalization method.
    * Deallocates the shared memory chunk
    *
    * @since S60 3.1
    * @param aThread The user mode client thread
    */
    virtual void OnReleaseMemory( DThread& aThread );

    // Prohibit copy constructor.
    MgmtFrameMemMngr( const MgmtFrameMemMngr& );
    // Prohibit assigment operator.
    MgmtFrameMemMngr& operator= ( const MgmtFrameMemMngr& ); 
    
private:    // Data

    /** kernel address of frame xfer block */
    RFrameXferBlock* iFrameXferBlock;

    /** 
    * kernel address of the shared memory chunk
    */
    TLinAddr iChunkKernelAddr;

    /** kernel address of Tx data buffer */
    TDataBuffer* iTxDataBuffer;

    /** 
    * ETrue if cached frame transfer memory shall be used,
    * EFalse otherwise
    */
    TBool iUseCachedMemory;

    /**
    * size of the Rx frame buffer allocation unit in bytes
    */
    TInt iRxFrameBufAllocationUnit;

    /** size of the shared memory chunk */
    TInt iChunkSize;
    };

#endif // MGMTFRAMEMMNGR_H
