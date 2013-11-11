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
* Description:   Declaration of the DataFrameMemMngr class.
*
*/

/*
* %version: 13 %
*/

#ifndef DATAFRAMEMMNGR_H
#define DATAFRAMEMMNGR_H

#include "EthernetFrameMemMngr.h"

class WlanChunk;

/**
*  Memory manager for protocol stack side client frame Rx memory
*
*  @since S60 v3.1
*/
class DataFrameMemMngr : public DEthernetFrameMemMngr
    {
public: 

    /** Ctor */
    DataFrameMemMngr( 
        DWlanLogicalChannel& aParent,
        WlanChunk*& aRxFrameMemoryPool,
        TInt aTxFrameBufAllocationUnit ) : 
        DEthernetFrameMemMngr( aParent, aRxFrameMemoryPool ),
        iFrameXferBlockProtoStack( NULL ),
        iUserToKernAddrOffset( 0 ),
        iTxDataChunk( NULL ),
        iTxFrameMemoryPool( NULL ),
        iTxFrameBufAllocationUnit ( aTxFrameBufAllocationUnit )
        {};

    /** Dtor */
    virtual ~DataFrameMemMngr() 
        {
        iFrameXferBlockProtoStack = NULL;
        iTxDataChunk = NULL;
        iTxFrameMemoryPool = NULL;
        };

protected: 

    /**
    * From DEthernetFrameMemMngr
    * Opens a handle to the allocated shared memory chunk
    *
    * @since S60 5.0
    * @param aThread
    * @param aSharedChunkInfo
    * @param aSharedMemoryChunk The shared memory chunk
    * @return system wide error code, KErrNone upon success
    */
    virtual TInt DoOpenHandle(
        DThread& aThread,
        TSharedChunkInfo& aSharedChunkInfo,
        DChunk* aSharedMemoryChunk );

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
     * Gets the highest priority frame (contained in a buffer allocated from
     * the shared memory) from the Rx queues.
     * Optionally frees the memory associated to a previously received frame. 
     * 
     * @param aFrameToFreeInUserSpace User space pointer to previously 
     *        received frame which can now be freed.
     *        NULL if nothing to free.
     * @return User space pointer to the Rx frame to be handled next.
     *         NULL, if there are no frames available.
     */ 
    virtual TDataBuffer* GetRxFrame( TDataBuffer* aFrameToFreeInUserSpace );
    
    /**
     * From DEthernetFrameMemMngr
     * Allocates a Tx packet from the shared memory.
     * 
     * @param aLength Length of the requested Tx buffer in bytes
     * @return User space pointer to the meta header attached to the 
     *         allocated packet, on success.
     *         NULL, in case of failure.
     */
    virtual TDataBuffer* AllocTxBuffer( TUint aLength );

    /**
     * From DEthernetFrameMemMngr
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
     * From DEthernetFrameMemMngr
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
     * From DEthernetFrameMemMngr
     * Deallocates a Tx packet.
     * 
     * All Tx packets allocated with AllocTxBuffer() must be deallocated using
     * this method.
     * 
     * @param aPacket Meta header of the packet to the deallocated
     */ 
    virtual void FreeTxPacket( TDataBuffer*& aPacket );

    /** 
     * From DEthernetFrameMemMngr
     * Determines if Tx from protocol stack side client should be resumed
     *  
     * @param aUserDataTxEnabled ETrue if user data Tx is enabled
     *        EFalse otherwise
     * @return ETrue if Tx should be resumed
     *         EFalse otherwise
     */
    virtual TBool ResumeClientTx( TBool aUserDataTxEnabled ) const;
    
    /** 
     * From DEthernetFrameMemMngr
     * Determines if all protocol stack side client's Tx queues are empty
     * 
     * @return ETrue if all Tx queues are empty
     *         EFalse otherwise
     */
    virtual TBool AllTxQueuesEmpty() const;
    
private:

    /**
    * From DEthernetFrameMemMngr
    * Memory finalization method.
    * 
    * @param aThread The user mode client thread
    */
    virtual void OnReleaseMemory( DThread& aThread );
    
    // Prohibit copy constructor.
    DataFrameMemMngr( const DataFrameMemMngr& );
    // Prohibit assigment operator.
    DataFrameMemMngr& operator= ( const DataFrameMemMngr& );  
    
private:    // Data

    /** kernel address of frame xfer block */
    RFrameXferBlockProtocolStack* iFrameXferBlockProtoStack;
    
    /** 
    * the offset from a User space address to the corresponding address
    * in the Kernel space in the shared memory chunk. May also be negative 
    */
    TInt32 iUserToKernAddrOffset;
    
    /** 
    * pointer to protocol stack side Tx area start in the kernel address 
    * space 
    */
    TUint8* iTxDataChunk;

    /** 
    * Tx frame memory pool manager
    * Own.
    */
    WlanChunk* iTxFrameMemoryPool;
    
    /**
    * size of the Tx frame buffer allocation unit in bytes
    */
    TInt iTxFrameBufAllocationUnit;
    };

#endif // DATAFRAMEMMNGR_H
