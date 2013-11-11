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
* Description:   Declaration of the TDataBuffer and the RFrameXferBlock class.
*
*/

/*
* %version: 26 %
*/

#ifndef FRAMEXFERBLOCK_H
#define FRAMEXFERBLOCK_H

#include "802dot11.h"
#include "umac_types.h"
#include "pack.h"
#include "wllddcircularbuffer.h"


/** 
* This value (bytes) should be at least as large as the Tx offset required
* by the used WLAN vendor implementation. 
* As we need to decide the max size of the Tx buffers already at compile
* time, we use this value when doing that.
* If the value is too small, WLAN Mgmt client may receive an error when trying 
* to submit a close to maximum length frame for transmit
*/
static const TUint KVendorTxHdrMax = 200;

/** 
* Extra space to enable us to align the start of a 802.11 MAC Tx frame on four 
* byte boundary, if it isn't aligned already. Shall also make the total
* buffer length divisible by four
*/
static const TUint KSpaceForAlignment = 4;

/** 
* This value (bytes) should be at least as large as the Tx trailer space 
* required by the used WLAN vendor implementation.
* The other comments for KVendorTxHdrMax (above) apply also to this value.
*/
static const TUint KVendorTxTrailerMax = 4;

/** 
* The length (in bytes) of the only WLAN Mgmt client Tx buffer
*/
static const TUint KMgmtSideTxBufferLength =                 // 2576
    KVendorTxHdrMax +                                        //  200
    KMaxDot11TxMpduLength +                                  // 2368
    KSpaceForAlignment +                                     //    4
    KVendorTxTrailerMax;                                     //    4

/** Protocol Stack Side client's Tx queue max lengths in packets */

static const TUint KVoiceTxQueueLen = 4;
static const TUint KVideoTxQueueLen = 8;
static const TUint KBestEffortTxQueueLen = 24;
static const TUint KBackgroundTxQueueLen = 4;

/** 
 * Estimates used for calculating the total Protocol Stack Side client Tx 
 * memory pool size 
 */

static const TUint KEthernetVoiceTxFrameLenEstimate = 256;
static const TUint KEthernetVideoTxFrameLenEstimate = 1024;
static const TUint KEthernetBestEffortTxFrameLenEstimate = 
                       KMaxEthernetFrameLength;
static const TUint KEthernetBackgroundTxFrameLenEstimate = 
                       KMaxEthernetFrameLength;

static const TUint KVendorTxHdrLenEstimate = 64;
static const TUint KVendorTxTrailerLenEstimate = 4;

static const TUint KProtocolStackSideFrameLenOverhead = 
    KVendorTxHdrLenEstimate +
    KHtQoSMacHeaderLength +  
    KMaxDot11SecurityEncapsulationLength +
    sizeof( SSnapHeader ) +
    KSpaceForAlignment +
    KVendorTxTrailerLenEstimate;

/** 
* Protocol stack side Tx memory pool size in packets
*/
static const TUint KTxPoolSizeInPackets = 
    KVoiceTxQueueLen +
    KVideoTxQueueLen +
    KBestEffortTxQueueLen +
    KBackgroundTxQueueLen;

/** 
* Protocol stack side Tx data chunk size in bytes
* Note! Needs to be divisible by 4
*/
static const TUint KProtocolStackSideTxDataChunkSize = 
    KVoiceTxQueueLen * ( KEthernetVoiceTxFrameLenEstimate + 
                         KProtocolStackSideFrameLenOverhead ) +
    KVideoTxQueueLen * ( KEthernetVideoTxFrameLenEstimate + 
                         KProtocolStackSideFrameLenOverhead ) +
    KBestEffortTxQueueLen * ( KEthernetBestEffortTxFrameLenEstimate +
                              KProtocolStackSideFrameLenOverhead ) +
    KBackgroundTxQueueLen * ( KEthernetBackgroundTxFrameLenEstimate + 
                              KProtocolStackSideFrameLenOverhead );                        
    
/**
*  Meta header for Rx and Tx frames
*
*  @since S60 v3.1
*/
#pragma pack( 1 )
class TDataBuffer 
    {
    friend class RFrameXferBlock; 
    friend class RFrameXferBlockProtocolStack; // because of SetLength()

public:

    typedef TUint16 TFrameType;

    /** ethernet II frame */
    static const TFrameType KEthernetFrame      = 0;
    /** complete 802.11 frame */
    static const TFrameType KDot11Frame         = 1;
    /** frame beginning with a SNAP header */
    static const TFrameType KSnapFrame          = 2;
    /** ethernet II frame used to test an AP */
    static const TFrameType KEthernetTestFrame  = 3;
    /** upper bound, so not a real type */
    static const TFrameType KFrameTypeMax       = 4;

    typedef TUint32 TFlag;
    
    /** 
    * flag indicating that the associated Rx buffer shall not be released 
    * when releasing this meta header 
    */
    static const TFlag KDontReleaseBuffer = ( 1 << 0 );
    /** 
    * flag indicating that the Tx frame in the associated buffer shall not 
    * be encrypted even if a valid encryption key existed. If not set, the
    * frame may be encrypted if a valid key exists
    */
    static const TFlag KTxFrameMustNotBeEncrypted = ( 1 << 1 );
    
public:

    /**
    * Appends data to Tx Buffer
    * @param aData data to be apended
    * @param aLength length of data in bytes
    * @return KErrNone on success,
    *         KErrOverflow if aLength is greater than the available space in
    *         the buffer
    */
    inline TInt AppendBuffer( 
        const TUint8* aData, 
        TUint32 aLength );

    /**
    * Gets length of the data in the buffer
    *
    * @since S60 3.1
    * @return length of the data in the buffer
    */
    inline TUint32 GetLength() const;

    /**
    * Gets the begin of the buffer
    *
    * @since S60 3.1
    * @return beginning of the buffer
    */
    inline const TUint8* GetBuffer() const;

    /**
    * Gets the beginning of the buffer
    *
    * @since S60 3.1
    * @return begin of the buffer
    */
    inline TUint8* GetBuffer();

    /**
    * Gets the frame type in the buffer
    *
    * @since S60 3.1
    * @return frame type in the buffer
    */
    inline TFrameType FrameType() const;

    /**
    * Sets the frame type in the buffer
    *
    * @since S60 3.1
    */
    inline void FrameType( TFrameType aFrameType );

    /**
    * Sets the 802.1D User Priority of the frame
    *
    * @since S60 3.2
    * @param aUp UP value to be set
    */
    inline void SetUserPriority( TUint8 aUp );

    /**
    * Gets the 802.1D User Priority of the frame
    *
    * @since S60 3.2
    * @return UP value
    */
    inline TUint8 UserPriority() const;

    /**
    * Gets the RCPI value of the Rx frame
    *
    * @since S60 3.2
    * @return RCPI value
    */
    inline TUint8 Rcpi() const;

    
    //////////////////////////////////////////////////////////////////////////
    // Rest of the methods are meant to be used only in the device driver code
    //////////////////////////////////////////////////////////////////////////

    /**
    * Gets the beginning of the data buffer
    * NOTE: to be used only by the device driver
    *
    * @return Buffer beginning
    */
    inline TUint8* KeGetBufferStart() const;
    
    /**
    * Sets the specified flag(s)
    * NOTE: to be used only by the device driver
    *
    * @param aFlag The flag(s) to set
    */
    inline void KeSetFlags( TFlag aFlags );

    /**
    * Returns the flags set in this object
    * NOTE: to be used only by the device driver
    *
    * @return Flags set in this object
    */
    inline TFlag KeFlags() const;
    
    /**
    * Clears the specified flag(s)
    * NOTE: to be used only by the device driver
    *
    * @param aFlag The flag(s) to clear
    */
    inline void KeClearFlags( TFlag aFlags );


#ifdef __KERNEL_MODE__ /* Kernel mode */
    
    /**
    * Sets the RCPI value of a received frame
    * NOTE: to be used only by the device driver
    *
    * @since S60 3.2
    * @param aRcpi RCPI value to be set
    */
    inline void KeSetRcpi( TUint8 aRcpi );

   /**
    * Gets the Destination Address of the frame to be transmitted
    * NOTE: to be used only by the device driver
    *
    * @return Destination Address
    */
    inline const TMacAddress& KeDestinationAddress() const;

    /**
    * Sets the offset to actual frame beginning within the data buffer
    * NOTE: to be used only by the device driver
    *
    * @param aOffset Offset to frame beginning
    */
    inline void KeSetOffsetToFrameBeginning( TUint32 aOffset );

    /**
    * Returns the offset to actual frame beginning within the data buffer
    * NOTE: to be used only by the device driver
    *
    * @return Offset to frame beginning
    */
    inline TUint32 KeOffsetToFrameBeginning() const;

    /**
    * Sets the offset from the start address of this object to the actual data 
    * buffer start address
    * NOTE: to be used only by the device driver
    *
    * @param aOffset Offset to data buffer start address. May be positive or 
    *        negative.
    */
    inline void KeSetBufferOffset( TInt32 aOffset );
    
    /**
    * Sets the length field
    * NOTE: to be used only by the device driver
    *
    * @since S60 3.1
    * @param aLength length to be set
    */
    inline void KeSetLength( TUint32 aLength )
        {
        iLength = aLength;
        };

#endif /* Kernel mode end */
    
private:

    /** Default Ctor */
    TDataBuffer() :
        iFrameType( KEthernetFrame ), 
        iUp( 0 ), 
        iRcpi( 0 ),
        iLength( 0 ), 
        iDestinationAddress( KZeroMacAddr ),
        iBufLength( 0 ),
        iOffsetToFrameBeginning( 0 ),
        iBufferOffsetAddr( sizeof( TDataBuffer ) ),
        iFlags( 0 ),
        iNext( NULL )
        {};

    /**
    * Adds a value to the length field
    * @param aValue length to be set
    */
    inline void IncrementLength( TUint32 aValue );

    /**
    * Appends data to Buffer
    * @param aData data to be apended
    * @param aLength length of data in BYTEs
    * @param aOffsetToFrameBeginning Amount of empty space to leave in the
    *        beginning of the buffer, i.e. before the frame beginning
    */
    inline void AppendBuffer( 
        const TUint8* aData, 
        TUint32 aLength,
        TUint32 aOffsetToFrameBeginning );

    /**
    * Sets the length field
    * @param aLength length to be set
    */
    inline void SetLength( TUint32 aLength );

    /**
    * Sets the length of the attached data buffer
    * @param aBufLength length to be set
    */
    inline void SetBufLength( TUint16 aBufLength );

    /**
    * Sets the Destination Address of the frame to be transmitted
    *
    * @param aDa Destination Address to be set
    */
    inline void SetDestinationAddress( const TMacAddress& aDa );

    /** Prohibit copy constructor */
    TDataBuffer( const TDataBuffer& );
    /** Prohibit assigment operator */
    TDataBuffer& operator= ( const TDataBuffer& );

private:    // Data

    /** type of the frame in buffer */
    TFrameType iFrameType;
        
    /** 
    * 802.1D User Priority of the frame
    */
    TUint8 iUp;

    /** 
    * RCPI of the received frame; range: [0..250]
    */
    TUint8 iRcpi;

    /** length of the data in buffer */
    TUint32 iLength;
    
    /** 
    * Destination address of the frame to be transmitted. Used only when
    * the address cannot be determined from the frame content  
    */
    TMacAddress iDestinationAddress;
    
    /** 
    * Length of the attached data buffer. Currently used only for Protocol
    * Stack Side client Tx frames 
    */
    TUint16 iBufLength;
    
    /** the offset to actual frame beginning within the data buffer */
    TUint32 iOffsetToFrameBeginning;

    /** 
    * the offset from the start address of this object to the start address 
    * of the data buffer. May be positive or negative.
    */
    TInt32 iBufferOffsetAddr;

    /** may contain a combination of the flags defined for this class */
    TFlag iFlags;
    
    /** pointer to the meta header of the next frame in the same Rx queue */ 
    TDataBuffer* iNext;
    
    /** 
    * Note! The total length of this object needs to be divisible by four
    * to enable the items following it to be correctly aligned. 
    */
    } __PACKED;
#pragma pack()

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TInt TDataBuffer::AppendBuffer( 
    const TUint8* aData, 
    TUint32 aLength )
    {
    if ( aLength <= iBufLength - iLength )
        {
        os_memcpy( 
            KeGetBufferStart() + iOffsetToFrameBeginning + iLength, 
            aData, 
            aLength );
        IncrementLength( aLength );
        
        return KErrNone;
        }
    else
        {
        return KErrOverflow;
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TUint32 TDataBuffer::GetLength() const
    {
    return iLength;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline const TUint8* TDataBuffer::GetBuffer() const
    {
    return KeGetBufferStart() + iOffsetToFrameBeginning;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TUint8* TDataBuffer::GetBuffer()
    {
    return KeGetBufferStart() + iOffsetToFrameBeginning;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TDataBuffer::TFrameType TDataBuffer::FrameType() const
    {
    return iFrameType;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void TDataBuffer::FrameType( TFrameType aFrameType )
    {
    iFrameType = aFrameType;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void TDataBuffer::SetLength( TUint32 aLength )
    {
    iLength = aLength;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void TDataBuffer::SetBufLength( TUint16 aBufLength )
    {
    iBufLength = aBufLength;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void TDataBuffer::IncrementLength( TUint32 aValue )
    {
    iLength += aValue;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void TDataBuffer::AppendBuffer( 
    const TUint8* aData, 
    TUint32 aLength,
    TUint32 aOffsetToFrameBeginning )
    {
    iOffsetToFrameBeginning = aOffsetToFrameBeginning;
    os_memcpy( 
        KeGetBufferStart() + aOffsetToFrameBeginning + iLength, 
        aData, 
        aLength );
    IncrementLength( aLength );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void TDataBuffer::SetUserPriority( TUint8 aUp )
    {
    iUp = aUp;
    }

// ---------------------------------------------------------------------------
// return as a TUint8 value as the range of User Priority is [0..7]
// ---------------------------------------------------------------------------
//
inline TUint8 TDataBuffer::UserPriority() const
    {
    return static_cast<TUint8>(iUp);
    }

// ---------------------------------------------------------------------------
// return as a TUint8 value as the range of RCPI is [0..250]
// ---------------------------------------------------------------------------
//
inline TUint8 TDataBuffer::Rcpi() const
    {
    return static_cast<TUint8>(iRcpi);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void TDataBuffer::SetDestinationAddress( const TMacAddress& aDa )
    {
    iDestinationAddress = aDa;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TUint8* TDataBuffer::KeGetBufferStart() const
    {
    return reinterpret_cast<TUint8*>(
        const_cast<TDataBuffer*>(this)) 
        + iBufferOffsetAddr;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void TDataBuffer::KeSetFlags( TFlag aFlags )
    {
    iFlags |= aFlags;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TDataBuffer::TFlag TDataBuffer::KeFlags() const
    {
    return iFlags;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void TDataBuffer::KeClearFlags( TFlag aFlags )
    {
    iFlags &= ~aFlags;    
    }

#ifdef __KERNEL_MODE__ /* Kernel mode */
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void TDataBuffer::KeSetRcpi( TUint8 aRcpi )
    {
    iRcpi = aRcpi;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline const TMacAddress& TDataBuffer::KeDestinationAddress() const
    {
    return iDestinationAddress;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void TDataBuffer::KeSetOffsetToFrameBeginning( TUint32 aOffset )
    {
    iOffsetToFrameBeginning = aOffset;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TUint32 TDataBuffer::KeOffsetToFrameBeginning() const
    {
    return iOffsetToFrameBeginning;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void TDataBuffer::KeSetBufferOffset( TInt32 aOffset )
    {
    iBufferOffsetAddr = aOffset;
    }

#endif /* Kernel mode end */

/**
* Ethernet frame transfer context block base class between user and 
* kernel space
*/
class RFrameXferBlockBase
    {

public:

#ifndef __KERNEL_MODE__ /* User mode */
    
    //////////////////////////////////////////////////////////////////////////
    // Rest of the methods are meant to be used only in the device driver code
    //////////////////////////////////////////////////////////////////////////

    /** 
     * Initializes the object instance, with the address of this in user space.
     * 
     * @param aThisAddrUserSpace Address of this object in user space.
     */
    inline void UserInitialize( TUint32 aThisAddrUserSpace );
    
#endif /* User mode end */

#ifdef __KERNEL_MODE__ /* Kernel mode */
    
    /**
    * Sets the Tx offset for every frame type which can be transmitted
    *
    * @since S60 5.0
    * @param aEthernetFrameTxOffset Tx offset for Ethernet frames and Ethernet
    *        Test frames
    * @param aDot11FrameTxOffset Tx offset for 802.11 frames
    * @param aSnapFrameTxOffset Tx offset for SNAP frames
    */
    void KeSetTxOffsets( 
        TUint32 aEthernetFrameTxOffset,
        TUint32 aDot11FrameTxOffset,
        TUint32 aSnapFrameTxOffset );

    /**
    * Returns the offset from a User space address to the corresponding address
    * in the Kernel space in the shared memory chunk. 
    * May also be negative.
    * 
    * @return The offset
    */
    inline TInt32 UserToKernAddrOffset() const;
    
protected:
    
    /**
    * Initialises the buffer.
    *
    * @since S60 3.1
    */
    void KeInitialize();
    
#endif /* Kernel mode end */    
    
private:

    /** Prohibit default constructor */
    RFrameXferBlockBase();

    /** Prohibit copy constructor */
    RFrameXferBlockBase( const RFrameXferBlockBase& );
    /** Prohibit assigment operator */
    RFrameXferBlockBase& operator= ( const RFrameXferBlockBase& );
    
protected:    // Data

    /** Address of this object instance in the kernel mode address space */
    TUint32 iThisAddrKernelSpace;
    
    /** 
    * the offset from a User space address to the corresponding address
    * in the Kernel space in the shared memory chunk. May also be negative 
    */
    TInt32 iUserToKernAddrOffset;

    /** 
    * defines a Tx offset for every frame type which can be transmitted
    */
    TUint32 iTxOffset[TDataBuffer::KFrameTypeMax];

    /**
    * Note! The length of this object needs to be divisible by 4 to make
    * the objects following it to be aligned correctly
    */
    };

#ifndef __KERNEL_MODE__ /* User mode */

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline void RFrameXferBlockBase::UserInitialize( 
    TUint32 aThisAddrUserSpace)
    {
    iUserToKernAddrOffset = iThisAddrKernelSpace - aThisAddrUserSpace;
    }

#endif /* User mode end */

#ifdef __KERNEL_MODE__ /* Kernel mode */

// -----------------------------------------------------------------------------
// Note! This method is executed in the context of the user mode client 
// thread, but in supervisor mode
// -----------------------------------------------------------------------------
//
inline TInt32 RFrameXferBlockBase::UserToKernAddrOffset() const
    {
    return iUserToKernAddrOffset;
    }

#endif /* Kernel mode end */


/**
* Ethernet frame transfer context block between user and kernel space
* for the Management Client
*/
class RFrameXferBlock : public RFrameXferBlockBase
    {

public:

#ifndef __KERNEL_MODE__ /* User mode */
    
    /**
    * Appends data to iTxDataBuffer
    *
    * @since S60 3.1
    * @param aData data to be apended
    * @param aLength length of data in BYTEs
    * @param aFrameType frame type idenitifier
    * @param aUserPriority 802.1D User Priority of the frame
    * @param aMustNotBeEncrypted If EFalse it is allowed to encrypt the frame
    *        if a valid encryption key exists. This is the default behavior.
    *        If ETrue, the frame must not be encrypted even if a valid 
    *        encryption key existed. 
    * @param aDestinationAddress If not NULL, specifies the destination MAC
    *        address for the frame to be transmitted
    * @return KErrNone on success,
    *         KErrOverflow if aLength is greater than the available space in
    *         the Tx buffer
    *         KErrNoMemory if frame transfer memory hasn't been allocated
    */
    inline TInt AppendTxDataBuffer( 
        const TUint8* aData, 
        TUint32 aLength,
        TDataBuffer::TFrameType aFrameType = TDataBuffer::KEthernetFrame,
        TUint8 aUserPriority = 0,
        TBool aMustNotBeEncrypted = EFalse,
        const TMacAddress* aDestinationAddress = NULL );

    /**
    * Clears TxDataBuffer, makes it zero length size
    *
    * @since S60 3.1
    */
    inline void ClearTxDataBuffer();
    
    /**
    * Set data buffer address
    *
    * Note! This method is executed in user mode context by the user mode
    * client interface, i.e. not the client itself!
    * @since S60 3.1
    * @param aUserAddr user address of the buffer
    */
    inline void SetTxDataBufferField( TLinAddr aUserAddr );

#endif /* User mode end*/    

#ifdef __KERNEL_MODE__ /* Kernel mode */
    
    /**
    * Initializes Kernel side memory interface to shared memory between User
    * and Kernel Space.
    *
    *  @param aTxBufLength
    */
    void Initialize( TUint32 aTxBufLength );
    
    /**
     * Adds the specified Rx frame (contained in a buffer allocated from the 
     * shared memory) to the Rx queue
     *  
     * @param aFrame Meta header attached to the frame; as a kernel
     *        space pointer.
     */
    void AddRxFrame( TDataBuffer* aFrame );
    
    /**
     * Gets the next frame from the Rx queue for passing it to
     * the WLAN Mgmt client.
     * 
     * @return User space pointer to the meta header of the Rx frame to be
     *         passed to the user mode client.
     *         NULL, if there's no frame available.
     */ 
    TDataBuffer* GetRxFrame();
    
    /**
     * Finds out if there is an Rx frame ready for user mode client retrieval
     * 
     * @return ETrue if an Rx frame is ready for retrival
     *         EFalse otherwise
     */ 
    TBool RxFrameAvailable() const;
        
#endif /* Kernel mode end */
    
private:

    /** Prohibit default constructor */
    RFrameXferBlock();

    /** Prohibit copy constructor */
    RFrameXferBlock( const RFrameXferBlock& );
    /** Prohibit assigment operator */
    RFrameXferBlock& operator= ( const RFrameXferBlock& );
    
private: // data

    /** Tx data buffer */
    TDataBuffer* iTxDataBuffer;

    /** 
    * stores the total capacity (length) of the Tx buffer (iTxDataBuffer) 
    * associated with this object instance
    */
    TUint32 iTxBufLength;
    
    /** pointer to the meta header of the 1st frame in the Rx queue */
    TDataBuffer* iRxQueueFront;
    
    /** pointer to the meta header of the last frame in the Rx queue */
    TDataBuffer* iRxQueueRear;    
    };

#ifndef __KERNEL_MODE__ /* User mode */

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TInt RFrameXferBlock::AppendTxDataBuffer( 
    const TUint8* aData, 
    TUint32 aLength,
    TDataBuffer::TFrameType aFrameType,
    TUint8 aUserPriority,
    TBool aMustNotBeEncrypted,
    const TMacAddress* aDestinationAddress )
    {
    if ( iTxDataBuffer )
        {
        if ( aLength <= 
            iTxBufLength - 
            iTxOffset[aFrameType] - 
            iTxDataBuffer->GetLength() )
            {
            // provided data fits into buffer
            
            iTxDataBuffer->FrameType( aFrameType );
            iTxDataBuffer->AppendBuffer( aData, aLength, iTxOffset[aFrameType] );
            iTxDataBuffer->SetUserPriority( aUserPriority );
    
            if ( aMustNotBeEncrypted )
                {
                iTxDataBuffer->KeSetFlags( 
                    TDataBuffer::KTxFrameMustNotBeEncrypted );
                }
            else
                {
                iTxDataBuffer->KeClearFlags( 
                    TDataBuffer::KTxFrameMustNotBeEncrypted );
                }
    
            if ( aDestinationAddress )
                {
                iTxDataBuffer->SetDestinationAddress( *aDestinationAddress );
                }
            
            return KErrNone;
            }
        else
            {
            return KErrOverflow;        
            }
        }
    else
        {
        return KErrNoMemory;
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void RFrameXferBlock::ClearTxDataBuffer()
    {
    if ( iTxDataBuffer )
        {
        iTxDataBuffer->SetLength( 0 );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void RFrameXferBlock::SetTxDataBufferField( TLinAddr aUserAddr )
    {
    iTxDataBuffer = reinterpret_cast<TDataBuffer*>(aUserAddr);
    }

#endif /* User mode end */


/**
* Ethernet frame transfer context block between user and kernel space 
* for the protocol stack side client 
* 
*/
class RFrameXferBlockProtocolStack : public RFrameXferBlockBase
    {

public:

#ifdef __KERNEL_MODE__ /* Kernel mode */
    
    /**
     * Initializes Kernel side memory interface to shared memory between User
     * and Kernel Space.
     */ 
    void Initialize();
    
    /**
     * Allocates a Tx buffer from the shared memory.
     * 
     * @param aTxBuf Kernel space pointer to the pre-allocated actual Tx buffer
     * @param aBufLength Length of the Tx buffer.
     * @return User space pointer to the meta header attached to the 
     *         allocated buffer, on success.
     *         NULL, in case of allocation failure.
     */
    TDataBuffer* AllocTxBuffer( const TUint8* aTxBuf, TUint16 aBufLength );
    
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
    TBool AddTxFrame( 
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
    TDataBuffer* GetTxFrame( 
        const TWhaTxQueueState& aWhaTxQueueState,
        TBool& aMore );
    
    /**
     * Deallocates a Tx packet.
     * 
     * All Tx packets allocated with AllocTxBuffer() must be deallocated using
     * this method.
     * 
     * @param aPacket Meta header of the packet to the deallocated
     */ 
    inline void FreeTxPacket( TDataBuffer*& aPacket );
    
    /** 
     * Determines if Tx from protocol stack side client should be resumed
     * 
     * @param aUserDataTxEnabled ETrue if user data Tx is enabled
     *        EFalse otherwise
     * @return ETrue if Tx should be resumed
     *         EFalse otherwise
     */
    TBool ResumeClientTx( TBool aUserDataTxEnabled ) const;
    
    /** 
     * Determines if all Tx queues are empty
     * 
     * @return ETrue if all Tx queues are empty
     *         EFalse otherwise
     */
    inline TBool AllTxQueuesEmpty() const;
        
    /**
     * Adds the specified Rx frame (contained in a buffer allocated from the 
     * shared memory) to the relevant Rx queue according to its AC (i.e.
     * priority).
     *  
     * @param aFrame Meta header attached to the frame; as a kernel
     *        space pointer.
     */
    void AddRxFrame( TDataBuffer* aFrame );

    /**
     * Gets the highest priority frame from the Rx queues for passing it to
     * the protocol stack side client.
     * 
     * @return User space pointer to the meta header of the Rx frame to be
     *         passed to the user mode client.
     *         NULL, if there's no frame available.
     */ 
    TDataBuffer* GetRxFrame();
    
    /**
     * Finds out if there is an Rx frame ready for user mode client retrieval
     * 
     * @return ETrue if an Rx frame is ready for retrival
     *         EFalse otherwise
     */ 
    inline TBool RxFrameAvailable() const;
    
#endif /* Kernel mode end */
    
private:

    /** 
     * With the current WLAN LDD Tx queue contents and WHA queue status, is
     * it possible to schedule a new Tx packet
     * 
     * @param aWhaTxQueueState Status of all WHA Tx queues (full/not full)
     * @param aQueueId Upon returning ETrue, denotes the WLAN LDD queue from
     *        which a packet can be scheduled  
     * @return ETrue if Tx packet scheduling is possible
     *         EFalse if Tx packet scheduling is not possible
     */
    TBool TxPossible(
        const TWhaTxQueueState& aWhaTxQueueState,
        TQueueId& aQueueId );
    
    /** 
     * Determines if the Tx flow from client needs to be stopped
     *  
     * @param aTxQueue Tx queue to which the latest packet was added
     * @param aUserDataTxEnabled ETrue if user data Tx is enabled
     *        EFalse otherwise
     * return EFalse if Tx flow needs to be stopped
     *        ETrue otherwise
     */
    TBool TxFlowControl( TQueueId aTxQueue, TBool aUserDataTxEnabled );
    
    /** Prohibit default constructor */
    RFrameXferBlockProtocolStack();

    /** Prohibit copy constructor */
    RFrameXferBlockProtocolStack( const RFrameXferBlockProtocolStack& );
    /** Prohibit assigment operator */
    RFrameXferBlockProtocolStack& operator= ( const RFrameXferBlockProtocolStack& );
    
private:    // Data

    /** Tx Queue for Voice priority frames */
    RWlanCircularBuffer<KVoiceTxQueueLen> iVoiceTxQueue;
    
    /** Tx Queue for Video priority frames */
    RWlanCircularBuffer<KVideoTxQueueLen> iVideoTxQueue;
    
    /** Tx Queue for Best Effort priority frames */
    RWlanCircularBuffer<KBestEffortTxQueueLen> iBestEffortTxQueue;
    
    /** Tx Queue for Background priority frames */
    RWlanCircularBuffer<KBackgroundTxQueueLen> iBackgroundTxQueue;
    
    /** Free Queue */
    RWlanCircularBuffer<KTxPoolSizeInPackets> iFreeQueue;

    /** Tx frame meta header objects */
    TDataBuffer iDataBuffers[KTxPoolSizeInPackets];
    
    /** pointer to the meta header of the 1st frame in the VO Rx queue */
    TDataBuffer* iVoiceRxQueueFront;
    
    /** pointer to the meta header of the last frame in the VO Rx queue */
    TDataBuffer* iVoiceRxQueueRear;
    
    /** pointer to the meta header of the 1st frame in the VI Rx queue */
    TDataBuffer* iVideoRxQueueFront;
    
    /** pointer to the meta header of the last frame in the VI Rx queue */
    TDataBuffer* iVideoRxQueueRear;
    
    /** pointer to the meta header of the 1st frame in the BE Rx queue */
    TDataBuffer* iBestEffortRxQueueFront;
    
    /** pointer to the meta header of the last frame in the BE Rx queue */
    TDataBuffer* iBestEffortRxQueueRear;
    
    /** pointer to the meta header of the 1st frame in the BG Rx queue */
    TDataBuffer* iBackgroundRxQueueFront;
    
    /** pointer to the meta header of the last frame in the BG Rx queue */
    TDataBuffer* iBackgroundRxQueueRear;
    
    /**
    * Note! The length of this object needs to be divisible by 4 to make
    * the objects following it to be aligned correctly
    */
    };


#ifdef __KERNEL_MODE__ /* Kernel mode */

// -----------------------------------------------------------------------------
// Note! This method is executed also in the context of the user mode client 
// thread, but in supervisor mode
// -----------------------------------------------------------------------------
//
inline void RFrameXferBlockProtocolStack::FreeTxPacket( TDataBuffer*& aPacket )
    {
    if ( aPacket )
        {
        aPacket->SetLength( 0 );
        aPacket->SetUserPriority( 0 );
        // put the packet to the Free Queue
        iFreeQueue.PutPacket( aPacket );
        aPacket = NULL;
        }
    }

// -----------------------------------------------------------------------------
// Note! This method is executed in the context of the user mode client 
// thread, but in supervisor mode
// -----------------------------------------------------------------------------
//
inline TBool RFrameXferBlockProtocolStack::AllTxQueuesEmpty() const
    {
    return ( iVoiceTxQueue.IsEmpty() &&  
             iVideoTxQueue.IsEmpty() &&
             iBestEffortTxQueue.IsEmpty() &&
             iBackgroundTxQueue.IsEmpty() ) ? ETrue : EFalse;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline TBool RFrameXferBlockProtocolStack::RxFrameAvailable() const
    {
    if ( reinterpret_cast<TBool>(iVoiceRxQueueFront) || 
         reinterpret_cast<TBool>(iVideoRxQueueFront) || 
         reinterpret_cast<TBool>(iBestEffortRxQueueFront) ||
         reinterpret_cast<TBool>(iBackgroundRxQueueFront ) )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

#endif /* Kernel mode end */

#endif // FRAMEXFERBLOCK_H
