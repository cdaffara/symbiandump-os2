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
* Description:   Declaration of the RPcmNetCardIf class.
*
*/

/*
* %version: 13 %
*/

#if !defined(__ETHERAPI_H__)
#define __ETHERAPI_H__

#include "wlanlddcommon.h"
#include "FrameXferBlock.h"

#ifndef __KERNEL_MODE__
#include <e32std.h> // for RChunk
#endif

class TCapsPcmNetCd
	{
public:
	TVersion version;
	};

/**
*  User mode interface for ethernet transfer over WLAN
*
*  @since S60 v3.1
*/
class RPcmNetCardIf : public RBusLogicalChannel
	{
public:
    
    /** The asynchronous call types */
	enum TRequest
		{
		ERequestRead,
		EResumeTx,
		}; 
	
	/** The synchronous call types */
	enum TControl
		{
		EControlGetConfig,
		EControlReadCancel,
		EControlResumeTxCancel,
        ESvControlInitBuffers,
        ESvControlFreeBuffers
		}; 

	/** 
	* Calls executed in the context of the client's thread, but in 
	* supervisor mode
	*/
	enum TControlFast
	    {
	    EControlFastAllocTxBuffer = 0x0F000000,
	    EControlFastAddTxFrame,
	    EControlFastGetRxFrame
	    };
	
public:
    
    /**
    * Open a channel
    *
    * @since S60 3.1
    */
	inline TInt Open() ;

    /**
    * Get version numbering information.
    *
    * @since S60 3.1
    * @return version required
    */
	inline TVersion	VersionRequired() const;

    /**
    * Cancel read request.
    *
    * @since S60 3.1
    */
	inline void ReadCancel() ;

    /**
    * Cancel Resume Tx request.
    *
    * @since S60 5.2
    */
	inline void ResumeTxCancel();

    /**
    * Retrieve configuration information
    *
    * @since S60 3.1
    * @param aConfig buffer for configuration info
    * @return KErrNone on success, any other on failure
    */
	inline TInt GetConfig( TDes8& aConfig ) ;

    /**
    * Allocate memory for FrameXferBlock to be used in data frame 
    * Tx- and Rx-operations. 
    * Does necessary memory allocation and maps the physical memory to 
    * calling process's address space so that the calling process
    * is able to access the memory.
    *
    * @since S60 3.1
    * @return KErrNone on success, any other on failure
    */
    inline TInt InitialiseBuffers();

    /**
    * Unmap memory mapped by InitiliazeBuffers method from the client 
    * process's address space and deallocate it.
    * Every call to InitialiseBuffers must be matched by a call to
    * ReleaseBuffers.
    *
    * @since S60 3.1
    * @return KErrNone on success, any other on failure
    */
    inline TInt ReleaseBuffers();

    /**
    * Asynchronous request to get notified once Tx from the client is
    * once again allowed.
    *
    * @since S60 5.2
    * @param aStatus a TRequestStatus
    */
    inline void ResumeTx( TRequestStatus& aStatus );

    /**
    * Asynchronous frame read function
    *
    * @since S60 3.1
    * @param aStatus a TRequestStatus
    */
    inline void RequestFrame( TRequestStatus& aStatus );

	/**
	 * Allocate a Tx buffer from the shared memory.
	 * 
     * @since S60 5.2
     * @param aLength Length of the requested Tx buffer in bytes
	 * @return Pointer to the allocated buffer, on success.
	 *         NULL, in case the allocation failed. 
	 *         Note! If NULL is returned the client is not allowed to call 
	 *         AddTxFrame()(i.e. Tx flow is stopped) until the next time when
	 *         ResumeTx asynchronous request completes.
	 */
	inline TDataBuffer* AllocTxBuffer( TUint aLength );
	
	/**
	 * Add the specified Tx frame (contained in the buffer allocated from the 
	 * shared memory) to the relevant Tx queue according to the frame's AC 
	 * (i.e. priority). 
	 * The frame will automatically get transmitted in its turn.
	 *  
     * @since S60 5.2
     * @param aPacket The frame to add
     * @return ETrue if the client is allowed to continue calling this method
     *         (i.e. Tx flow is not stopped).
     *         EFalse if the client is not allowed to call this method again
     *         (i.e. Tx flow is stopped) until the next time when 
     *         ResumeTx asynchronous request completes.
	 */
	inline TBool AddTxFrame( TDataBuffer* aPacket );
	
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
	inline TDataBuffer* GetRxFrame( TDataBuffer* aFrameToFree );
	
	
#ifndef __KERNEL_MODE__
private:
    /** handle to kernel side shared memory chunk */
    RChunk iSharedMemoryChunk;
#endif
	};

#ifndef __KERNEL_MODE__
#include "EtherCardApi.inl"
#endif

#endif  // __ETHERAPI_H__
