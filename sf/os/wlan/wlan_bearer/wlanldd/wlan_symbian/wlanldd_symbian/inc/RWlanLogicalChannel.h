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
* Description:   Declaration of the RWlanLogicalChannel class.
*
*/

/*
* %version: 19 %
*/

#ifndef RWLAN_LOGICAL_CHANNEL_H_
#define RWLAN_LOGICAL_CHANNEL_H_

#include "wlanlddcommon.h"
#include "FrameXferBlock.h"

#ifndef __KERNEL_MODE__
#include <e32std.h> // for RChunk
#endif

// Open parameters
struct TOpenParam 
	{
    TAny*   iPda;               // start of PDA data 
                                // excludes the S60 specific configuration data
                                // in the beginning which is added by UMAC
                                // (pointee must be 32-bit aligned)
    TUint32 iPdaLength;         // length of PDA data in bytes
                                // excludes the S60 specific configuration data
                                // in the beginning


    TAny*   iFirmWare;          // firmware data 
                                // (pointee must be 32-bit aligned)
    TUint32 iFirmWareLength;    // length of firmware in bytes 	
	};

class RWlanLogicalChannel : public RBusLogicalChannel
    {
public:

    // Enumeration values for control messages
    enum TWlanControl
        {
        EWlanSvControlInitBuffers,  // Allocate frame transfer memory for WLAN Mgmt
                                    // client
        EWlanSvControlFreeBuffers   // Deallocate memory allocated by 
                                    // EWlanSvControlInitBuffers
        };

    // Enumeration values for asynchronous requests. Do not exceed KMaxRequests!
    enum TWlanRequest
        {
        EWlanRequestNotify,        // request a notification
        EWlanRequestFrame,         // frame Rx request
        EWlanRequestSend,          // frame Tx request
        EWlanCommand,              // WLAN management command
        EWlanInitSystem,           // internal init
        EWlanFinitSystem,          // internal finit
        EWlanMaxRequest
        };

    /** 
    * Calls executed in the context of the client's thread, but in 
    * supervisor mode
    */
    enum TControlFast
        {
        EWlanControlFastGetRxFrame = 0x0E000000
        };
    
public:
    
    /** 
      * Opens a channel to the WLAN device driver.
      *
      * @since S60 3.1
      * @param aUnit This parameter tells which kind of channel is opened, i.e.
      *        in practice identifies the client who wishes to open a channel.
      * @param aOpenParam Basic initialization parameters which WLAN LDD passes
      *        to WLAN PDD
      * @return KErrNone on success, any other on failure
      */
	inline TInt Open( 
        TWlanUnit aUnit, 
        TOpenParam& aOpenParam );

    /**
     * Closes the device driver channel
     * 
     * Last method to be called prior driver unload
     *
     * @since S60 3.1
     */
    inline void CloseChannel();

    /** 
      * Returns the version required by the WLAN device driver.
      * 
      * @since S60 3.1
      * @return The required version.
      */
 	inline TVersion VersionRequired() const;

    /** 
      * Submits a management command to the device driver. 
      * 
      * The command is executed either synchronously or asynchronously, 
      * depending on the parameters.
      *
      * @since S60 3.1
      *	@param aInBuffer Reference to input buffer descriptor for the command
      *	@param aOutBuffer Pointer to output buffer, can be NULL if no output
      *        is expected
      * @param aStatus Pointer to the status object. If aStatus is NULL, the 
      *        command is executed synchronously, otherwise asynchronously.
      * @return KErrNone, if the management command succeeded, 
      *         any other on failure
      */
	inline TInt ManagementCommand(
        const TDesC8& aInBuffer, 
        TDes8* aOutputBuffer = NULL, 
        TRequestStatus* aStatus = NULL);

    /** 
      * Requests generic notifications from device driver.
      * 
      * @since S60 3.1
      *	@param aStatus Status parameter for request completion
      *	@param aBuffer Reference to indication buffer. When an indication 
      *        appears the request completes and the buffer contains the
      *        indication information.
      */
    inline void RequestSignal( TRequestStatus &aStatus, TIndication &aBuffer );


    /** 
      * Cancels notification request.
      *
      * @since S60 3.1
      */
	inline void CancelRequestSignal();

    /**
    * Allocates frame transfer memory for WLAN Mgmt client use.
    *  
    * Does the necessary memory allocation and maps the
    * physical memory to the address space of the calling process, so that 
    * also it can access the memory.
    * Note! The channel must be open and the Configure management commmand
    * must have been issued before calling this method.
    *
    * @since S60 3.1
    * @param aFrameXferBlock FrameXferBlock to be initialised by 
    *        the device driver.
    * @return KErrNone on success, any other on failure
    */
    inline TInt InitialiseBuffers( RFrameXferBlock*& aFrameXferBlock );

    /**
    * Unmaps memory mapped by InitialiseBuffers method from the address space 
    * of the calling process and deallocates it.
    * 
    * Every call to InitialiseBuffers must be matched by a call to
    * ReleaseBuffers
    *
    * @since S60 3.1
    * @return KErrNone on success, any other on failure
    */
    inline TInt ReleaseBuffers();

    /**
    * Asynchronous frame transmit request
    *
    * @since S60 3.1
    * @param aStatus a TRequestStatus
    */
    inline void WriteFrame( TRequestStatus &aStatus );

    /**
    * Asynchronous frame receive request
    *
    * @since S60 3.1
    * @param aStatus a TRequestStatus
    */
    inline void RequestFrame( TRequestStatus &aStatus );

    /** 
      * Cancels WLAN Mgmt client frame receive request.
      *
      * @since S60 3.1
      * Buffers can be released after this call returns
      */
    inline void CancelRxRequests();

    /**
     * Gets the next frame (contained in a buffer allocated from
     * the shared memory) from the Rx queue.
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
    
private:

    inline TInt InitWlanSystem( TOpenParam& aOpenParam );

private: // data

    /**
     * If a WLAN Management Command is executed asynchronously, this data
     * member is used to store the Management Command message - thus keeping
     * it valid until the asynchronous request is actually scheduled.
     * Note that only a single WLAN Management Command can be under processing 
     * at a time.
     */
    SOidMsgStorage iAsyncOidCommandMsg;
    
    /**
     * If a WLAN Management Command is executed asynchronously, this data
     * member is used to store the Management Command output buffer context - 
     * thus keeping it valid until the asynchronous request is actually 
     * scheduled.
     * Note that only a single WLAN Management Command can be under processing 
     * at a time.
     */
    SOutputBuffer iAsyncOidCommandOutput;
    
    /** Used to store the channel open parameters */
    TOpenParam iOpenParam;
    
#ifndef __KERNEL_MODE__
    /** handle to kernel side shared memory chunk */
    RChunk iSharedMemoryChunk;
#endif
	};

#ifndef __KERNEL_MODE__
    #include "RWlanLogicalChannel.inl"
#endif // __KERNEL_MODE__

#endif// #if !defined(RWLAN_LOGICAL_CHANNEL_H_)
