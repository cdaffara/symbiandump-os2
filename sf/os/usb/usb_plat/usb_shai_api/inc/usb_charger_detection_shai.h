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
    @brief USB Charger Detection SHAI header
    @version 0.3.0

    This header specifies the USB Charger Detection SHAI.

    @publishedDeviceAbstraction
*/


#ifndef USB_CHARGER_DETECTION_SHAI_H
#define USB_CHARGER_DETECTION_SHAI_H

// System includes
#include <kern_priv.h>
#include <usb/usb_charger_detection_shai_shared.h>	// Common types shared with upper layers


/**
 * This macro specifies the version of the USB Charger Detection SHAI
 * header in binary coded decimal format. This allows the PSL layer to
 * confirm a certain definition is available, if needed. This can for
 * example make it possible for a new PSL to support compilation in an
 * older environment with old USB SHAI version that is missing some
 * new definitions.
 */
#define USB_CHARGER_DETECTION_SHAI_VERSION 0x030

// The namespace is documented in file usb_common_shai.h, so it is not
// repeated here
namespace UsbShai
    {
    // Class declaration

    /**
     * An interface class implemented by the PIL layer to allow the
     * Charger Detector PSL to report charger detection events to the
     * PIL layer.
     */
    NONSHARABLE_CLASS( MChargerDetectorObserverIf )
        {
        public:            
        /**
         * Called by the Charger Detector PSL to report the detection
         * of a specified type of a port.
         *
         * When the PIL layer has registered itself as the observer of
         * the Charger Detector PSL, it is the responsibility of the
         * Charger Detector PSL to run the charger detection algorithm
         * when applicable. These situations include:
         *
         * 1. When VBUS has risen while our device is the B-device. A
         *    Charger Detector PSL that supports Data Contact Detect
         *    (see Battery Charging Specification 1.1, Section 3.3)
         *    should complete Data Contact Detect and the charger
         *    detection algorithm before notifying the VBUS rising
         *    event to the respective PIL layer.
         *
         *    For a peripheral-only port, this requirement is
         *    documented in more detail in usb_peripheral_shai.h,
         *    function
         *    MUsbPeripheralPilCallbackIf::DeviceEventNotification(). For
         *    an OTG-capable port, the requirement is documented in
         *    usb_otg_shai.h, function
         *    MOtgObserverIf::NotifyIdPinAndVbusState().
         *
         * 2. When VBUS is high, the Charger Detector PSL needs to
         *    observe changes in the ID pin state, if the Charger
         *    Detector PSL supports detecting the port types relevant
         *    to Accessory Charger Adapter. This requirement is
         *    documented in more detail in usb_otg_shai.h, function
         *    MOtgObserverIf::NotifyIdPinAndVbusState().
         *
         * 3. When our device has been detached from a reported port
         *    type (for example, the Charger Detector PSL reported
         *    port type for Standard Downstream Port and later VBUS
         *    drops indicating disconnect), the Charger Detector PSL
         *    must report the disconnection by notifying port type
         *    EPortTypeNone.
         *
         * @param aPortType The type of the port detected
         */
        virtual void NotifyPortType( TPortType aPortType ) = 0;
        };


    /**
     * An interface class that needs to be implemented by the Charger
     * Detector PSL that registers to the PIL layer.
     *
     * A system that does not support USB Battery Charging does not
     * need a Charger Detector PSL. In this case the PIL layer will
     * always assume the device is connected to a Standard Downstream
     * Port and will always connect to the bus when VBUS rises.
     *
     * Due to the dependencies between normal USB functionality
     * (observing the state of the ID pin, the VBUS level, the line
     * state, and controlling connecting to the bus) and USB Battery
     * Charging (in terms of charger detection and requirements about
     * connecting to the bus and driving VBUS), the Charger Detector
     * PSL cannot be considered independent of the USB Controller
     * PSLs.
     *
     * In practice, it is expected that the Charger Detector interface
     * for a peripheral-only port is implemented by the Peripheral
     * Controller PSL, or at least that the Peripheral Controller PSL
     * is communicating with the Charger Detector PSL. This is
     * necessary to ensure that the necessary parts of charger
     * detection are run before reporting VBUS high (when Data Contact
     * Detect is supported), and that the Peripheral Controller and
     * the charger detection can safely share the bus without conflict
     * (as both will need to touch the line state). See
     * usb_peripheral_shai.h,
     * MUsbPeripheralPilCallbackIf::DeviceEventNotification() for
     * description of the requirements for peripheral-only PSLs.
     *
     * Similarly, it is expected that the Charger Detector interface
     * for an OTG-capable port is implemented by the OTG Controller
     * PSL, or at least that the OTG Controller PSL is communicating
     * with the Charger Detector PSL. See usb_otg_shai.h,
     * MOtgObserverIf::NotifyIdPinAndVbusState() for description of
     * the requirements for OTG-capable PSLs.
     *
     * When the PIL layer is ready to receive charger detection
     * notifications from the PSL, it will use this interface to
     * register itself as the Charger Detector PSL observer. This is
     * guaranteed to occur before any USB usage is attempted.
     */
    NONSHARABLE_CLASS( MChargerDetectorIf )
        {
        public:
        /**
         * Called by the PIL layer to set itself as the observer of
         * charger detection events.
         *
         * If the port type has already been detected by the Charger
         * Detector PSL when the observer is being set (typically
         * because VBUS was already high at boot time), it is the
         * responsibility of the Charger Detector PSL to immediately
         * report the previously detected port type to the observer to
         * get the PIL layer to the correct state.
         *
         * @param aObserver Reference to the observer interface that
         *   the charger detector is required to report events to
         */            
        virtual void SetChargerDetectorObserver( MChargerDetectorObserverIf& aObserver ) = 0;  
        };


    /**
     * A static class implemented by the USB PIL layer to allow the
     * PSL layer to register its charger detector component to the PIL
     * layer.
     */
    NONSHARABLE_CLASS( UsbChargerDetectionPil )
        {
        public:
        /**
         * Registration function to be used by the USB PSL layer to
         * register a charger detector component to the PIL layer. The
         * PIL layer will set itself as the observer of the charger
         * detector component to receive notification of detected USB
         * chargers.
         *
         * The intended usage is that the component that implements
         * USB charger detection registers itself to the USB PIL layer
         * by making this call from their own kernel extension entry
         * point function (or an equivalent code that runs during
         * bootup).
         *
         * @param aChargerDetector Reference to the Charger Detector
         *   interface implemented by the registering PSL. The PIL
         *   layer requires that the supplied reference remains valid
         *   indefinitely, as the Charger Detector PSL cannot unregister.
         *
         * @param aProperties Reference to an object describing the
         *   static properties of the Charger Detector. The PIL takes
         *   a copy and the PSL is free to release the properties
         *   object upon return.
         *
         * @lib usbperipheralpil.lib
         */
        IMPORT_C static void RegisterChargerDetector( MChargerDetectorIf&         aChargerDetector,
                                                      TChargerDetectorProperties& aProperties );
        };
    };

#endif //USB_CHARGER_DETECTION_SHAI_H
// END of file

