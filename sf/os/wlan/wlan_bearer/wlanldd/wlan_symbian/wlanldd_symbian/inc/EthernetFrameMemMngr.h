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
* Description:   Declaration of the DEthernetFrameMemMngr class.
*
*/

/*
* %version: 19 %
*/

#ifndef DETHERNETFRAMEMEMMNGR_H
#define DETHERNETFRAMEMEMMNGR_H

#include "FrameXferBlock.h"
#include "WlanLogicalChannel.h"

class RFrameXferBlock;
class DThread;
class TDataBuffer;

class TSharedChunkInfo;
class WlanChunk;

/**
* Manager object for frame Tx and Rx memory
*
*  @since S60 v3.1
*/
class DEthernetFrameMemMngr
    {

public:

    /** Dtor */
    virtual ~DEthernetFrameMemMngr();

    /**
    * Memory intilisation method.
    * Allocates a shared memory chunk, if relevant, and opens a handle to it
    * for the user mode client.
    *
    * @since S60 3.1
    * @param aThread calling processes DThread object
    * @param aSharedChunkInfo After successful return contains the handle to the
    *        chunk
    * @param aVendorTxHdrLen Amount of free space (bytes) that the WLAN vendor
    *        implementation requires to exist in a Tx buffer before the 802.11
    *        MPDU to be sent
    * @param aVendorTxTrailerLen Amount of free space (bytes) that the WLAN 
    *        vendor implementation requires to exist in a Tx buffer after the
    *        802.11 MPDU to be sent 
    * @return system wide error code, KErrNone upon success
    */
    TInt OnInitialiseMemory( 
        DThread& aThread,
        TSharedChunkInfo* aSharedChunkInfo,
        TUint aVendorTxHdrLen,
        TUint aVendorTxTrailerLen );

    /**
    * Checks if the memory is in use, i.e. is it allocated and attached to the 
    * address space of the user mode client process 
    * @return the state of the memory: in use (ETrue), or not (EFalse)
    */
    TBool IsMemInUse() const { return iInUse; }

    /**
    * Memory finalization method.
    * Deallocates the shared memory chunk, if relevant
    *
    * @since S60 3.1
    * @param aThread The user mode client thread
    */
    virtual void OnReleaseMemory( DThread& aThread ) = 0;

    /**
    * Gets a memory block that can be used for tx frame write
    *
    * @since S60 3.1
    * @return memory block that can be used for tx frame write, 
    *         NULL upon failure
    */
    virtual TDataBuffer* OnWriteEthernetFrame() const;

    /**
    * Gets a memory block that can be used as rx frame buffer
    *
    * @since S60 3.1
    * @param aLengthinBytes Requested buffer length
    * @return memory block that can be used as rx frame buffer
    *         NULL upon failure
    */
    TUint8* OnGetEthernetFrameRxBuffer( TUint aLengthinBytes );

    /**
    * Gets a memory block for storing Rx frame meta header
    *
    * @return memory block for storing Rx frame meta header on success
    *         NULL upon failure
    */
    TDataBuffer* GetRxFrameMetaHeader();

    /**
    * Deallocates Rx frame meta header
    * @param aMetaHeader Meta header to deallocate
    */
    void FreeRxFrameMetaHeader( TDataBuffer* aMetaHeader );
    
    /**
     * Frees the memory associated to a frame. 
     * 
     * @param aFrameToFreeInUserSpace User space pointer to the meta header
     *        of the frame to be freed.
     */ 
    void FreeRxPacket( TDataBuffer* aFrameToFreeInUserSpace );
    
    /**
    * To be called when rx frame read cycle has ended.
    *
    * @since S60 3.1
    * @param aBufferStart first element of the array that holds pointers to
    *        Rx frame meta headers
    * @param aNumOfBuffers number of meta header pointers in the array
    * @return ETrue if a pending user mode frame read request exists 
    *         and callee should complete it, 
    *         EFalse otherwise
    */
    TBool OnEthernetFrameRxComplete( 
        TDataBuffer*& aBufferStart, 
        TUint32 aNumOfBuffers );

    /**
    * To be called when user mode client issues a frame Rx request
    *
    * @since S60 3.1
    * @return ETrue if callee should complete the request immediately 
    *         as there exists Rx frame(s) which can be retrieved by the user
    *         mode client.
    *         EFalse otherwise
    */
    virtual TBool OnReadRequest() = 0;

    /**
     * Gets the highest priority frame (contained in a buffer allocated from
     * the shared memory) from the Rx queues.
     * Optionally frees the memory associated to a previously received frame. 
     * 
     * @param aFrameToFree Previously received frame which can now be freed.
     *        NULL if nothing to free.
     * @return Pointer to the Rx frame to be handled next.
     *         NULL, if there are no frames available. If NULL is returned
     *         the client should re-issue the asynchronous frame Rx request
     *         (i.e. RequestFrame())
     */ 
    virtual TDataBuffer* GetRxFrame( TDataBuffer* aFrameToFree ) = 0;
    
    /**
    * Frees the specified Rx frame buffer
    *
    * @since S60 3.1
    * @param aBufferToFree The buffer to free
    */
    virtual void DoMarkRxBufFree( TUint8* aBufferToFree );

    /**
     * Allocates a Tx packet from the shared memory.
     * 
     * @param aLength Length of the requested Tx buffer in bytes
     * @return User space pointer to the meta header attached to the 
     *         allocated packet, on success.
     *         NULL, in case of failure.
     */
    virtual TDataBuffer* AllocTxBuffer( TUint aLength );
    
    /**
     * Adds the specified Tx frame (contained in the buffer allocated from the 
     * shared memory) to the relevant Tx queue according to its AC (i.e.
     * priority).
     *  
     * @param aPacketInUserSpace Meta header attached to the frame; as a user
     *        space pointer.
     * @param aPacketInKernSpace If not NULL on return, the frame needs to be 
     *        discarded and this is the kernel space pointer to its meta header.
     *        If NULL on return, the frame must not be discarded. 
     * @param aUserDataTxEnabled ETrue if user data Tx is enabled
     *        EFalse otherwise
     * @return ETrue if the client is allowed to continue calling this method
     *         (i.e. Tx flow is not stopped).
     *         EFalse if the client is not allowed to call this method again
     *         (i.e. Tx flow is stopped) until it is re-allowed.
     */
    virtual TBool AddTxFrame( 
        TDataBuffer* aPacketInUserSpace, 
        TDataBuffer*& aPacketInKernSpace,
        TBool aUserDataTxEnabled );
    
    /**
     * Gets the frame to be transmitted next from the Tx queues.
     * 
     * @param aWhaTxQueueState State (full / not full) of every WHA transmit 
     *        queue
     * @param aMore On return is ETrue if another frame is also ready to be 
     *        transmitted, EFalse otherwise
     * @return Pointer to the meta header of the frame to be transmitted, on
     *         success
     *         NULL, if there's no frame that could be transmitted, given the
     *         current status of the WHA Tx queues
     */ 
    virtual TDataBuffer* GetTxFrame( 
        const TWhaTxQueueState& aTxQueueState,
        TBool& aMore );
    
    /**
     * Deallocates a Tx packet.
     * 
     * All Tx packets allocated with AllocTxBuffer() must be deallocated using
     * this method.
     * 
     * @param aPacket Meta header of the packet to the deallocated
     */ 
    virtual void FreeTxPacket( TDataBuffer*& aPacket );
    
    /** 
     * Determines if Tx from protocol stack side client should be resumed
     *  
     * @param aUserDataTxEnabled ETrue if user data Tx is enabled
     *        EFalse otherwise
     * @return ETrue if Tx should be resumed
     *         EFalse otherwise
     */
    virtual TBool ResumeClientTx( TBool aUserDataTxEnabled ) const;
    
    /** 
     * Determines if all protocol stack side client's Tx queues are empty
     * 
     * @return ETrue if all Tx queues are empty
     *         EFalse otherwise
     */
    virtual TBool AllTxQueuesEmpty() const;
    
    /**
    * Static creator of the class instance
    * @param aUnit identifier of the type of object to be created
    * @param aParent 
    * @param aRxFrameMemoryPool 
    * @param aUseCachedMemory ETrue if cached frame transfer memory shall be
    *        used,
    *        EFalse otherwise 
    * @param aFrameBufAllocationUnit size of the Rx/Tx frame buffer allocation
    *        unit in bytes
    * @return DEthernetFrameMemMngr object or NULL upon failure
    *
    * @since S60 3.1
    */
    static DEthernetFrameMemMngr* Init( 
        TInt aUnit, 
        DWlanLogicalChannel& aParent,
        WlanChunk*& aRxFrameMemoryPool,
        TBool aUseCachedMemory,
        TInt aFrameBufAllocationUnit );

    /**
    * Sets the Tx offset for every frame type which can be transmitted
    *
    * @param aEthernetFrameTxOffset Tx offset for Ethernet frames and Ethernet
    *        Test frames
    * @param aDot11FrameTxOffset Tx offset for 802.11 frames
    * @param aSnapFrameTxOffset Tx offset for SNAP frames
    */
    void SetTxOffsets( 
        TUint32 aEthernetFrameTxOffset,
        TUint32 aDot11FrameTxOffset,
        TUint32 aSnapFrameTxOffset );

   
protected: 

    /** Ctor */
    DEthernetFrameMemMngr( 
        DWlanLogicalChannel& aParent, 
        WlanChunk*& aRxFrameMemoryPool ) :
        iReadStatus( ENotPending ), 
        iFrameXferBlockBase( NULL ),
        iRxDataChunk( NULL ),
        iParent( aParent ),
        iRxFrameMemoryPool( aRxFrameMemoryPool ),
        iRxBufAlignmentPadding( 0 ),
        iVendorTxHdrLen( 0 ),
        iVendorTxTrailerLen( 0 ),
        iClientChunkHandle( -1 ),
        iInUse( EFalse )
        {};

    /**
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
        DChunk* aSharedMemoryChunk ) = 0;

    /**
    * Gets a free rx buffer
    *
    * @since S60 3.1
    * @param aLengthinBytes Requested buffer length
    * @return buffer for Rx data upon success
    *         NULL otherwise
    */
    virtual TUint8* DoGetNextFreeRxBuffer( TUint aLengthinBytes );

    /**
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
        TUint32 aNumOfBuffers ) = 0;

    /**
    * Marks memory as not in use, meaning that it is not allocated 
    * and attached to calling process'es address space
    */
    void MarkMemFree() { iInUse = EFalse; }  
    
private:

    /**
    * Marks memory as in use, meaning that is it allocated 
    * and attached to calling processes address space
    */
    void MarkMemInUSe() { iInUse = ETrue; }

    // Prohibit copy constructor.
    DEthernetFrameMemMngr( const DEthernetFrameMemMngr& );
    // Prohibit assigment operator.
    DEthernetFrameMemMngr& operator= ( const DEthernetFrameMemMngr& );

protected:  // Data

    enum TFrameReadState 
        { 
        /** there is no rx frame read request pending in kernel-mode */
        ENotPending, 
        /** 
        * there is rx frame read request waiting 
        * to be completed in kernel-mode 
        */
	    EPending
        };

    /** state  of the rx frame read request */
    TFrameReadState    iReadStatus;

    /** kernel address of xfer block; as base class pointer */
    RFrameXferBlockBase* iFrameXferBlockBase;

    /** pointer to Rx area start in the kernel address space */
    TUint8*             iRxDataChunk;

    /** 
    * reference to logical channel object instance owning this object 
    * instance
    */
    DWlanLogicalChannel& iParent;
    
    /** 
    * reference to Rx frame memory pool manager
    */
    WlanChunk*&  iRxFrameMemoryPool;

    /** 
    * number of extra bytes required to align Rx buffer start address
    * to be returned to WHA layer to allocation unit boundary
    */
    TInt iRxBufAlignmentPadding;   
    
    /** 
    * amount of free space (bytes) that the WLAN vendor implementation 
    * requires to exist in a Tx buffer before the 802.11 MPDU to be sent
    */
    TUint iVendorTxHdrLen;
    
    /** 
    * amount of free space (bytes) that the WLAN vendor implementation 
    * requires to exist in a Tx buffer after the 802.11 MPDU to be sent
    */
    TUint iVendorTxTrailerLen;
    
    /** user mode client's handle to the shared memory chunk */
    TInt iClientChunkHandle;
    
private:    // Data
    
    /** is memory in use or not */
    TBool               iInUse;
    };

#endif // DETHERNETFRAMEMEMMNGR_H
