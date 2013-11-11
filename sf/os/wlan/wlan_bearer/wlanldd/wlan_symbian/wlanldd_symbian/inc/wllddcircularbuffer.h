/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Declaration of the RWlanLddCircularBuffer class.
*
*/

/*
* %version: 2 %
*/

#ifndef R_WLANCIRCULARBUFFER_H
#define R_WLANCIRCULARBUFFER_H

class TDataBuffer;

/**
 * Wlan LDD Shared Memory FIFO queue for Tx packets.
 * 
 * @param size The size of the queue in packets.
 */
template <TUint size>
class RWlanCircularBuffer
    {
public:

    /* Because this class is mapped into shared memory, it does not have
     * a constructor or a desctructor.
     */ 

    /**
     * Initialization. This method acts as a contructor.
     */ 
    inline void DoInit();

    /**
     * Release. This method acts as a desctructor.
     */ 
    inline void Release();
    
    /**
     * This method returns and removes the next packet from the queue.
     * 
     * @return Pointer to the meta header attached to the packet on success.
     *         NULL if the queue is empty.
     */ 
    inline TDataBuffer* GetPacket();
   
    /**
     * This method is used to put a packet to the end of the queue.
     * 
     * @param aPacket Pointer to the meta header attached to the packet.
     * @return ETrue if the Packet was added to the queue.
     *         EFalse if the queue was already full and the Packet could not
     *         be added to the queue.
     */ 
    inline TBool PutPacket( TDataBuffer* aPacket );
    
    /**
     * Returns the next packet in the queue without removing it from the queue.
     * 
     * @return Pointer to the meta header attached to the packet on the top 
     *         of the queue.
     */
    inline TDataBuffer* PeekPacket();

    /**
     * Returns the number of packets in the queue.
     * 
     * @return number of packets in the queue.
     */
    inline TUint GetLength() const;
    
    /**
     * This method can be used to check if the queue is empty.
     * 
     * @return ETrue, when the queue is empty.
     *         EFalse, when the queue is not empty.
     */ 
    inline TBool IsEmpty() const;

    /**
     * This method can be used to check if the queue is full.
     * 
     * @return ETrue, when the queue is full.
     *         EFalse, when the queue is not full.
     */ 
    inline TBool IsFull() const ;
    
    /**
     * This method can be used to check if the queue is active.
     * 
     * @param aTimeNow Current time as microseconds since midnight, January 
     *        1st, 0 AD nominal Gregorian
     * @return ETrue, if the queue is active.
     *         EFalse, if the queue is not active.
     */ 
    inline TBool IsActive( TInt64 aTimeNow ) const;
    
private:    // Data
    
    /** Queue as a circular buffer */
    TDataBuffer* iBuffer[size];
    
    /** Index of the packet to get next. See GetPacket() */
    TUint iGetIndex;
    
    /** Index where to put the next packet. See PutPacket() */
    TUint iPutIndex;
    
    /** The number of packets currently in the queue */
    TUint iPacketAmount;
   
    /** The capacity of the queue */
    TUint iSize;
    
    /** 
    * Time stamp (as microseconds since midnight, January 1st, 0 AD nominal
    * Gregorian) when the queue became empty. 
    * Valid only if the queue is empty 
    */
    TInt64 iBecameEmptyAt;
};

#ifdef __KERNEL_MODE__

#include "wllddcircularbuffer.inl"

#endif

#endif /* R_WLANCIRCULARBUFFER_H */
