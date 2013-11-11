/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  spia interface declaration
*
*/

/*
* %version: 7 %
*/

#ifndef WLANSPIA_H
#define WLANSPIA_H

class MWlanSpiaClient;
class MWlanOsaExt;

#include <wlanspiatypes.h>

/**
 *  spia interface declaration
 *
 *
 *  @lib wlanpdd.pdd
 *  @since S60 v3.2
 */
class WlanSpia 
    {

public:

    typedef TInt TRequestId;

    /* SPI transfer clock speed. The closest one is selected if the 
     * exact clock frequency cannot be achieved. 
     * The closest is always less than the requested frequency.*/
    enum TClockSpeed
        {
        EClk1_5MHz = 0,
        EClk3MHz,
        EClk6MHz,
        EClk12MHz,
        EClk24MHz,
        EClk48MHz,
        EClk96MHz
        };

    /* SPI clock modes used for transfer. 
       Next picture shows what different configuration values means.

                               ,--> Sampling
                               | ,> Shift out
                               | |
            high/rising  ______|¯|_|¯|_|¯|_ (mode 0)

            high/falling ____|¯|_|¯|_|¯|_|¯ (mode 1)

            low/falling  ¯¯¯¯¯¯|_|¯|_|¯|_|¯ (mode 2)

            low/rising   ¯¯¯¯|_|¯|_|¯|_|¯|_ (mode 3)
    */
    enum TClockMode
        {
        /* SPI clock active high, sampling occurs on the rising edge */
        EPolarityHighRisingEdge = 0,
        /* SPI clock active high, sampling occurs on the falling edge */
        EPolarityHighFallingEdge,
        /* SPI clock active low, sampling occurs on the falling edge */
        EPolarityLowFallingEdge,
        /* SPI clock active low, sampling occurs on the rising edge */
        EPolarityLowRisingEdge
        };

    /* SPI slave select signal polarity used for transfer */
    enum TSsPolarity
        {
        /** SS is held \e high during the active state */
        ESsPolarityHigh = 0,
        /** SS is held \e low during the active state */
        ESsPolarityLow
        };

    /* SPI read mode, used when reading is done */
    enum TReadMode
        {
        /* normal read from SPI slave device 
         * (i.e no write involved for this call) */
        ENormalRead = 0,
        /* write buffer is written completely 
         * before the read operation is done */
        EWriteB4Read,
        /* full duplex mode 
         * Tx and Rx are done at same time
         * NOTE: Tx and Rx buffers must not overlap and must be of equal size  */        
        ECombinedWriteRead
        };

    struct TConfig
        {
        /* SPI clock mode */
        TClockMode iSpiaClockMode;
        /* SPI slave select signal polarity */
        TSsPolarity iSpiaSsPolarity;
        /* number of bytes per frame in tx */
        TUint8 iTxBytesPerFrame;
        /* number of bytes per frame in rx */
        TUint8 iRxBytsPerFrame;
        };
    
    /* SPI slave select mode */    
    enum TSsMode    
    {
        /* slave select is active during the whole SPI request operation */
        EForcedActive = 0,
        /* slave select is toggled on SPI frame basis during the whole SPI request */        
        EFrameControlled                
    };
    
    struct TCapabilities
    {
    /* bit 0 - memory config support: 
     * bit clear non cached memory supported, bit set cached memory supported
     * default value is cleared 
     * bit 1 - ENormalRead support in TReadMode: bit set when supported          
     * bit 2 - EWriteB4Read support in TReadMode: bit set when supported               
     * bit 3 - ECombinedWriteRead support in TReadMode: bit set when supported          
     * bit 4 - EForcedActive support in TSsMode: bit set when supported                       
     * bit 5 - EFrameControlled in TSsMode: bit set when supported */        
    TUint32 iCapability;
    /* Length of the memory cache line in bytes (needed for buffer alignment) */
    TUint32 iCacheLineLength;
    /* DMA threshold for SPI operations */
    TUint32 iDMAThreshold;
    };
    
    /* SPIA request structure */
    struct TRequest
        {
        /* tx buffer. Buffer length must be multiple of sizeof(TInt) */
        const TUint8* iTxBuffer;
        /* amount of data to transmit in the tx buffer in bytes */
        TUint   iTxBufferSize;
        /* rx buffer. Buffer length must be multiple of sizeof(TInt) */
        TUint8* iRxBuffer;
        /* amount of data to read to the rx buffer in bytes */
        TUint   iRxBufferSize;
        /* read operation type */
        TReadMode   iReadMode;
        /* complete callback method client to be called after 
         * the request has completed */
        MWlanSpiaClient* iRequestClient;
        /* unique request ID passed back to client as 
         * is in SPIA request callback */
        TRequestId iRequestId;
        /* SPI transfer clock speed. 
         * The closest one is selected if exact clock frequency 
         * can not be achieved. The closest is always less 
         * than the requested frequency. */
        TClockSpeed iSpiaClockSpeed;
        /* last request or not for a while, a hint for power management 
         * ETrue upon last request, any other not */
        TBool iLastRequest;
        /* SPI slave select mode used */        
        TSsMode iSsMode;
        };

	/**
	 * Destructor.
	 *
	 * @since S60 v3.2
	 */
    virtual ~WlanSpia() {};

    /**
     * Create spia layer object
     *
     * @since S60 v3.2
     * @param aOsaExt osa extension object
     * @return spia layer object, NULL upon failure
     * @deprecated Create() and Attach( MWlanOsaExt& ) should be used instead
     */
    static WlanSpia* Create( MWlanOsaExt& aOsaExt );

    /**
     * Create spia layer object
     *
     * @since S60 v3.2
     * @return spia layer object, NULL upon failure
     */
    static WlanSpia* Create();

    /**
     * Attach the OSA extension object
     *
     * @since S60 v3.2
     * @param aOsaExt osa extension object
     */
    void Attach( MWlanOsaExt& aOsaExt );

    /**
     * Destroy spia layer object
     *
     * @since S60 v3.2
     * @param aWlanSpia spia layer object
     */
    static void Destroy( WlanSpia* aWlanSpia );
    
    /**
     * Extract module capabilities
     *
     * @since S60 v3.2
     * @param aCapabilities capabilities of the module
     */    
    virtual void Capability( TCapabilities& aCapabilities ) { aCapabilities.iCapability = 0; }

    /**
     * Configures the SPI HW block
     *
     * @since S60 v3.2
     * @param aConfig SPI configuration data
     */
    virtual void Configure( const TConfig& aConfig ) = 0;

    /**
     * Issues SPIA request
     *
     * NOTE: the request must remain valid until operation completes 
     * (either synchronously or asynchronously by completion callback).
     * It is guaranteed by the system that no SPIA request complete callback 
     * gets called during this call
     *
     * @since S60 v3.2
     * @param aRequest request to execute
     * @return status of the operation
     *         ESuccess -       request was accepted for asynchronous 
     *                          excecution and will complete at a later time.
     *                          Request completion is notified by 
     *                          SPIA request complete callback.
     *                          SPIA user is allowed to issue additional 
     *                          requests
     *
     *         EFailure -       request was not accepted as it failed due 
     *                          unspecified reason.
     *                          SPIA request complete callback is not called 
     *
     *         ESuccessXfer -   request was accepted and completed 
     *                          synchronously inside the SPIA. 
     *                          SPIA request complete callback is not called. 
     *                          SPIA user is allowed to issue additional 
     *                          requests
     *
     *         EPending -       request was accepted for asynchronous 
     *                          excecution and will complete at later time. 
     *                          Request completion is notified by 
     *                          SPIA request complete callback.
     *                          SPIA user is NOT allowed to issue additional 
     *                          requests until SPIA request complete callback is called
     */
    virtual SPIA::TStatus Request( const TRequest& aRequest ) = 0; 

    /**
     * Cancels a pending SPIA request. Either succeeds or has no effect
     *
     * NOTE: it is guaranteed by the system that no SPIA request complete callback 
     * gets called during this call
     *
     * @since S60 v3.2
     * @param aRequestId request ID that identifies the SPIA request to cancel
     * @return status of the operation
     *         ESuccess -       operation success. 
     *                          SPIA request was cancelled and no SPIA request
     *                          complete callback gets executed for the request 
     *                          that was cancelled
     * 
     *
     *         EFailure -       the request is in use and can not be cancelled. 
     */
    virtual SPIA::TStatus Cancel( TRequestId aRequestId ) = 0;
        
protected:

    /**
     * Constructor 
     *
     * @since S60 v3.2
     */
    explicit WlanSpia() : iOsaExt( NULL ) {}

    /**
     * Constructor 
     *
     * @since S60 v3.2
     * @param aOsaExt osa extension object
     */
    explicit WlanSpia( MWlanOsaExt& aOsaExt ) : iOsaExt( &aOsaExt ) {}

    /**
     * Extract osa extension
     *
     * @since S60 v3.2
     * @return osa extension
     */
    inline MWlanOsaExt& OsaExtCb();

private:

    /**
     * osa extension
     */
    MWlanOsaExt*    iOsaExt;

    };

#include <wlanspia.inl>

#endif // WLANSPIA_H
