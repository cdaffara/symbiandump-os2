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

/** @file

    @brief USB Peripheral SHAI header shared part.

    @version 0.4.0

    Some Non-kernel USB code will also use structure defined in this
    header.  Since usb_peripheral_shai.h always need to include
    <kern_priv.h> which is not visible to non-kernel layers, a
    separated header is needed.
    
    This header is the one that defined all common structures used by
    kernel & non-kernel layers software.
    
    The file is versioned for the sake of the human reader and follows
    the version of the USB Peripheral SHAI header
    usb_peripheral_shai.h.

    Peripheral SHAI header always include this header automatically.
    
    @publishedDeviceAbstraction
*/

#ifndef USB_PERIPHERAL_SHAI_SHARED_H
#define USB_PERIPHERAL_SHAI_SHARED_H

// The namespace is documented in file usb_common_shai.h, so it is not
// repeated here
namespace UsbShai
    {
    /** 
     * Bitmaps for TUsbPeripheralEndpointCaps.iSupportedTypesAndDir.
     * Endpoint supports Control transfer type.
     */
    const TUint KUsbEpTypeControl     = 0x00000001;
    const TUint KUsbEpTypeIsochronous = 0x00000002;
    const TUint KUsbEpTypeBulk        = 0x00000004;
    const TUint KUsbEpTypeInterrupt   = 0x00000008;

    /** Endpoint supports IN transfers. */
    const TUint KUsbEpDirIn           = 0x80000000;
    /** Endpoint supports OUT transfers. */
    const TUint KUsbEpDirOut          = 0x40000000;
    /** Endpoint supports bidirectional (Control) transfers only.*/
    const TUint KUsbEpDirBidirect     = 0x20000000;
    
    /**
     * Indicates that whether DMA is preferred to be applied for this endpoint.
     *
     * Upper application will declare whether DMA is expected to be used for
     * this endpoint if there is one available (PSL knows about it) when setup
     * an interface.
     * 
     * PSL can ignore this request if NO DMA is available.
     *
     * PIL may query whether a DMA is really applied via interface 
     * MPeripheralControllerIf::QueryEndpointResource() later.
     *
     * @see TUsbPeripheralEndpointInfo::iFeatureWord1.
     */
    const TUint KUsbEndpointInfoFeatureWord1_DMA             = 0x00000001;

    /**
     * Indicates that whether double buffering is preferred to be applied for 
     * this endpoint. 
     *
     * Upper application will declare whether double buffering is expected to 
     * be used for this endpoint if there is one available (PSL knows about it)
     * when setup an interface.
     * 
     * PSL can ignore this request if NO double buffering is possible.
     * 
     * PIL may query whether a double buffering is really applied via interface 
     * MPeripheralControllerIf::QueryEndpointResource() later.
     *
     * @see TUsbPeripheralEndpointInfo::iFeatureWord1.
     */
    const TUint KUsbEndpointInfoFeatureWord1_DoubleBuffering = 0x00000002;
    
    /** 
     * These are states that are described in the USB standard.
     */
    enum TUsbPeripheralState
        {
        EUsbPeripheralStateUndefined,  // 0
        EUsbPeripheralStateAttached,   // 1
        EUsbPeripheralStatePowered,    // 2
        EUsbPeripheralStateDefault,    // 3
        EUsbPeripheralStateAddress,    // 4
        EUsbPeripheralStateConfigured, // 5
        EUsbPeripheralStateSuspended,  // 6
        EUsbPeripheralNoState = 0xff   // 255 (used as a place holder)
        };
    
    /** 
     * Endpoint capabilities as reported by the PSL driver.
     */
    class TUsbPeripheralEndpointCaps
        {
        public:
        /** The supported maximum packet sizes. */
        TUint iSizes;
        
        /** 
         * The supported endpoint types and directions.
         * see the bitmap definition for type and dir inside this header.
         */
        TUint iTypesAndDir;
        
        /** set it to ETrue if it is a 'high-speed, high bandwidth' endpoint. */
        TBool iHighBandwidth;
        
        /** 
         * Reserved for future use. 
         * PSL must set those reserved ram to zero to keep binary compatibility
         * with new (in furture) PIL binary which may utilize some of the 
         * reserved space.
         */
        TUint32 iReserved[2];
        };
    
    /**
     * The desired endpoint capabilities.
     * @see MPeripheralControllerIf::ConfigureEndpoint.
     */
    class TUsbPeripheralEndpointInfo
        {
        public:
        /** 
         * Endpoint type.
         *(mask: UsbShai::KUsbEpTypeControl, etc., but used as value).
         */
        TUint iType;
        
        /** Direction (mask: UsbShai::KUsbEpDirIn, etc., but used as value). */
        TUint iDir;
        
        /** Maximum packet size (literal, no mask). */
        TInt iSize;
        
        /** 
         * Interval for polling full-speed interrupt and isochronous endpoints.
         * Expressed either directly in milliseconds with a valid range 1..255
         * (interrupt), or for use as 'value' in the expression of 
         * interval=2^(value-1) with a valid range 1..16 (isochronous).
         */
        TInt iInterval;
        
        /** 
         * Interval for polling high-speed interrupt and isochronous endpoints,
         * or to specify the NAK rate for high-speed control and bulk OUT
         * endpoints. Expressed either for use as 'value' in the expression
         * interval=2^(value-1) with a valid range 1..16 (interrupt and 
         * isochronous), or directly as the maximum NAK rate with a valid range
         * 0..255 (control and bulk).
         */
        TInt iInterval_Hs;
        
        /**
         * The number of additional transactions per frame to be scheduled(0..2)
         * (A value greater than zero is only valid for high-speed high bandwidth
         * interrupt and isochronous endpoints. Also note that there are endpoint
         * size restrictions associated with additional transactions - see 9.6.6.).
         */
        TInt iTransactions;
        
        /** 
         * The number of extra bytes that the standard endpoint descriptor
         * should be extended by. In almost all cases, this should be 0 (zero).
         */    
        TInt iExtra;
        
        /** 
         * 32 flag bits indicating miscellaneous endpoint features.
         * Currently defined are:
         * - KUsbEndpointInfoFeatureWord1_DMA which means apply DMA to this 
         *   endpoint if available.
         * - KUsbEndpointInfoFeatureWord1_DoubleBuffering which means apply 
         *   double buffering to this endpoint if available.
         */
        TUint32 iFeatureWord1;
        
        /** Reserved for future use. */
        TUint32 iReserved;
        };
    }
#endif //USB_PERIPHERAL_SHAI_SHARED_H

// End of files
