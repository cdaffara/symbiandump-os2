/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/

#ifndef WLANSDIOA_H
#define WLANSDIOA_H

class MWlanSdioaClient;
class MWlanOsaExt;

#include <wlansdioatypes.h>

class WlanSdioa
    {

public:

    typedef TInt TRequestId;

    enum { KMaxFunctions = 8 };

    enum TSdioBusWidth
        {
        ESdioBusWidth1 = 1,
        ESdioBusWidth4 = 4
        };

    struct TCapabilities
        {
        /* Capability mask (values defined in wlansdioatypes.h) */
        TUint32 iCapability;
        
        /* Maximum number of buffers the SDIOA client is allowed to provide in buffer lists
           of the TReadRequest and TWriteRequest structures.
           The value of 1 means that only a single contiguous buffer can be used. */
        TUint32 iMaxNumberOfBuffers;
        
        /* Length of the memory cache line (in bytes) */
        TUint32 iCacheLineLength;
        };

    struct TBufferItem 
        {
        /* Buffer length */
        TUint32 iLength;
        
        /* Data buffer */
        TUint8 *iBuffer;
        
        /* Pointer to the next list item (NULL if this is the last item) */
        struct TBufferItem *iNext;
        };

    struct TReadRequest
        {
        /* Command number */
        TUint8 iCmdIndex;
        
        /* Function number */
        TUint8 iFunctionNum;
        
        /* Register address */
        TUint32 iRegAddr;
        
        /* Flags (defined in wlansdioatypes.h) */
        TUint32 iFlags;
        
        /* Receive buffer (only valid for CMD52) */
        TUint8 iRxBuffer;
        
        /* Receive buffer list (only valid for CMD53) */
        TBufferItem *iRxBufferList;
        
        /* Transfer size (bytes) */
        TUint32 iRxCount;
        
        /* Completion callback */
        MWlanSdioaClient* iRequestClient;
        
        /* Request identifier (passed back to client with callback) */
        TRequestId iRequestId;
        };

    struct TWriteRequest
        {
        /* Command number */
        TUint8 iCmdIndex;
        
        /* Function number */
        TUint8 iFunctionNum;
        
        /* register address */
        TUint32 iRegAddr;
        
        /* Flags (defined in wlansdioatypes.h) */
        TUint32 iFlags;
        
        /* Command argument or CMD52 byte data */
        TUint8 iTxBuffer;
        
        /* Transmit buffer list (only valid for CMD53) */
        TBufferItem *iTxBufferList;
        
        /* Transfer size (bytes) */
        TUint32 iTxCount;
        
        /* Completion callback */
        MWlanSdioaClient* iRequestClient;
        
        /* Request identifier (passed back to client with callback) */
        TRequestId iRequestId;
        };

    /**
     * Destructor
     *
     */
    virtual ~WlanSdioa() {};

    /**
     * Create SDIOA object
     *
     */
    static WlanSdioa* Create();

    /**
     * Attach the OSA extension object
     *
     * @param aOsaExt OSA extension object
     */
    void Attach( MWlanOsaExt& aOsaExt );

    /**
     * Destroy the SDIOA object
     *
     * @param aWlanSdioa sdioa layer object
     */
    static void Destroy( WlanSdioa* aWlanSdioa );

    /**
     * Extract module capabilities
     *
     * @param aCapabilities capabilities of the module
     */    
    virtual void Capability( TCapabilities& aCapabilities ) = 0;        

    /**
     * Issue an SDIOA request (separate methods for read and write)
     *
     * @param aRequest Request structure
     * @return Status of the operation
     *         ESuccess     -   The request was accepted for asynchronous
     *                          execution and will complete later.
     *                          Completion is notified by the SDIOA request
     *                          completion callback.
     *                          The SDIOA client is allowed to issue additional
     *                          requests.
     *
     *         EFailure     -   The request was not accepted for execution.
     *                          The request completion callback will not be called.
     *
     *         ESuccessXfer -   The request was accepted and completed
     *                          synchronously.
     *                          The SDIOA request completion callback will not be called.
     *                          The SDIOA client is allowed to issue additional requests.
     *
     *         EPending     -   The request was accepted for asynchronous
     *                          excecution and will complete later.
     *                          Completion is notified by the SDIOA request
     *                          completion callback.
     *                          The SDIOA client is not allowed to issue additional
     *                          requests until the completion callback has been called.
     */
    virtual SDIOA::TStatus ReadRequest( TReadRequest& aRequest ) = 0;
    virtual SDIOA::TStatus WriteRequest( TWriteRequest& aRequest ) = 0;

    /**
     * Cancel a pending SDIOA request
     *
     * @param aRequestId Request ID
     * @return Status of the operation
     *         ESuccess     -   The SDIOA request was cancelled.
     *                          The completion callback will not be called.
     *
     *         EFailure     -   The request could not be cancelled.
     */
    virtual SDIOA::TStatus Cancel( TRequestId aRequestId ) = 0;

    /**
     * Enable SDIO interrupts (multiplexed on DAT1 line)
     *
     * @param aEnable ETrue (enable) or EFalse (disable)
     * @param aCallbackClient Pointer to the object containing the OnSDIOInterrupt callback method
     *                        (can be omitted when aEnable == EFalse)
     * @return Status of the operation
     *         ESuccess     -   SDIO interrupts were enabled.
     *                          The MWlanSdioaClient::OnSDIOInterrupt()
     *                          callback will be called after an
     *                          interrupt has occurred.
     *
     *         EFailure     -   SDIO interrupts could not be enabled.
     */
	virtual SDIOA::TStatus EnableSDIOInterrupt( TBool aEnable, MWlanSdioaClient* aCallbackClient = NULL ) = 0;

    /**
     * Initialize the SDIO device
     * 
     * This method will perform device initialization so that the device
     * will be ready for receiving CMD52 and CMD53 requests.
     * The method must be called after the WLAN device has been powered on.
     */
	virtual SDIOA::TStatus Initialize() = 0;

    /**
     * Set the SDIO bus width
     * 
     * @param aBusWidth SDIO bus width
     *
     * This method sets the SDIO bus width to 1 or 4 bits.
     */
    virtual void SetBusWidth( TSdioBusWidth aBusWidth ) = 0;

    /**
     * Set the SDIO bus clock speed
     * 
     * @param aMHz SDIO bus clock speed in MHz
     *
     * This method sets the SDIO bus clock speed to the value
     * specified by the aMHz parameter or the nearest lower
     * value supported by the hardware.
     */
    virtual void SetClockSpeed( TUint32 aMHz ) = 0;

protected:

    /**
     * Constructor
     *
     * @param aOsaExt OSA extension object
     */
    explicit WlanSdioa() {};

    /**
     * Extract OSA extension
     *
     * @return OSA extension
     */
    inline MWlanOsaExt& OsaExtCb();

private:

    /**
     * OSA extension
     */
    MWlanOsaExt*    iOsaExt;

    };

#include <wlansdioa.inl>

#endif // WLANSDIOA_H
