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
    @brief USB Charger Detection SHAI shared constants and types
    @version 0.3.0

    This header specifies USB Charger Detection SHAI common types and
    constants used by both the kernel side code and user side code. It
    is automatically included by the USB Charger Detection SHAI header
    and is not intended to be directly included by SHAI
    implementations.

    The file is versioned for the sake of the human reader and follows
    the version of the USB Charger Detection SHAI header
    usb_charger_detection_shai.h.

    @publishedDeviceAbstraction
*/


#ifndef USB_CHARGER_DETECTION_SHAI_SHARED_H
#define USB_CHARGER_DETECTION_SHAI_SHARED_H


// The namespace is documented in file usb_common_shai.h, so it is not
// repeated here
namespace UsbShai
    {
    // Data types

    /**
     * An enumeration listing the different port types that can be
     * reported to the PIL layer by a registered Charger Detector
     * PSL. The available types mostly correspond to those mentioned
     * in the Battery Charging Specification Revision 1.1.
     */
    enum TPortType
        {
        /**
         * This type is reported to indicate that the Charger Detector
         * PSL has detected that we are no longer connected to a
         * powered port. This situation occurs when VBUS driven from
         * outside drops, or the Accessory Charger Adapter changes the
         * RID state from RID_A to RID_GND (which usually also means
         * that VBUS will drop very soon).
         */
        EPortTypeNone = 0,

        /**
         * This type is reported to indicate that the Charger
         * Detector PSL has detected that our device is connected to
         * an unsupported port. One common type of an unsupported port
         * is a PS/2 to USB adapter connected to a PS/2 port of a
         * computer.
         */
        EPortTypeUnsupported,

        /** 
         * This type is reported when the Charger Detector PSL has
         * detected that our device is connected to a charging port,
         * but has not yet distinguished whether the port is a
         * Charging Downstream Port or a Dedicated Charging Port.
         *
         * When this port type is detected, the upper layers will
         * connect to the USB bus as the peripheral by requesting the
         * Peripheral Controller PSL to assert the D+ pull-up. The
         * Charger Detector PSL can then detect the exact port type by
         * observing what happens to the level of the D- line, as
         * specified in the Battery Charging Specification. Upon
         * detecting the exact port type, the Charger Detector PSL can
         * notify a new event with the correct type.
         *
         * If the Charger Detector PSL can directly distinguish the
         * exact port type, the PSL does not need to report this
         * generic charging port type, but can directly report the
         * more specific type EPortTypeDedicatedChargingPort or
         * EPortTypeChargingDownstreamPort.
         */
        EPortTypeChargingPort,

        /** 
         * This type is reported when the Charger Detector PSL has
         * detected that our device is connected to a Dedicated
         * Charging Port.
         *
         * When this port type is detected, the upper layers will
         * connect to the USB bus as the peripheral by requesting the
         * Peripheral Controller PSL to assert the D+ pull-up, as
         * specified in the Battery Charging Specification.
         */
        EPortTypeDedicatedChargingPort,
            
        /** 
         * This type is reported when the Charger Detector PSL has
         * detected that our device is connected to a Charging
         * Downstream Port.
         *
         * When this port type is detected, the upper layers will
         * connect to the USB bus as the peripheral by requesting the
         * Peripheral Controller PSL to assert the D+ pull-up, as
         * specified in the Battery Charging Specification.
         */
        EPortTypeChargingDownstreamPort,
            
        /**
         * This type is reported when the Charger Detector PSL has
         * detected that our device is connected to a Standard
         * Downstream Port.
         *
         * When this port type is detected, the upper layers will
         * connect to the USB bus as the peripheral by requesting the
         * Peripheral Controller PSL to assert the D+ pull-up, as
         * specified in the Battery Charging Specification.
         */
        EPortTypeStandardDownstreamPort,

        /** 
         * This type is reported when the Charger Detector PSL has
         * detected that our device is connected to the OTG port of an
         * Accessory Charger Adapter and the ID pin is in the RID_A
         * range.
         *
         * When this port type is detected in an OTG-capable device,
         * the OTG State Machine will default to the host role.
         */
        EPortTypeAcaRidA,

        /** 
         * This type is reported when the Charger Detector PSL has
         * detected that our device is connected to the OTG port of an
         * Accessory Charger Adapter and the ID pin is in the RID_B
         * range.
         *
         * When this port type is detected, the USB Peripheral PIL
         * layer will ensure that the Peripheral Controller PSL is not
         * allowed to connect to the bus, as required by the Battery
         * Charging Specification.
         */
        EPortTypeAcaRidB,

        /** 
         * This type is reported when the Charger Detector PSL has
         * detected that our device is connected to the OTG port of an
         * Accessory Charger Adapter and the ID pin is in the RID_C
         * range.
         *
         * When this port type is detected, the upper layers will
         * connect to the USB bus as the peripheral by requesting the
         * Peripheral Controller PSL to assert the D+ pull-up, as
         * specified in the Battery Charging Specification.
         */
        EPortTypeAcaRidC,
        };


     /**
     * This class specifies the information provided by a Charger
     * Detector PSL when registering to the PIL layer.
     *
     * The PSL should prepare for the possibility that members may be
     * added to the end of this class in later SHAI versions if new
     * information is needed to support new features. The PSL should
     * not use this class as a direct member in an object that is not
     * allowed to grow in size due to binary compatibility reasons.
     *
     * @see UsbChargerDetectionPil::RegisterChargerDetector()
     */
    NONSHARABLE_CLASS( TChargerDetectorProperties )
        {
        public: // Types and constants
        /**
         * A bitmask type used to indicate the static capabilities of
         * the Charger Detector.
         */
        typedef TUint32 TChargerDetectorCaps;

        /**
         * Capability bit to indicate whether the USB system below the
         * SHAI (either in HW or in the low-level SW) supports
         * automatically reducing charging current for the duration of
         * the USB high-speed chirp signalling. See Battery Charging
         * Specification Revision 1.1, Chapter 3.6.2 for description
         * of the problem.
         *
         * If the system does not support this feature, the upper
         * layer USB components that calculate available charging
         * current will always limit the charging current taken from a
         * Charging Downstream Port so that the maximum current during
         * chirp is not violated.
         *
         * If the system supports this feature, the full available
         * charging current from a Charging Downstream Port is
         * utilized. It is then the responsibility of the HW or some
         * low-level SW below to SHAI to ensure that the charging
         * current is automatically reduced for the duration of chirp
         * signalling.
         *
         * If the system supports this feature, the PSL shall set the
         * corresponding bit in iCapabilities (by bitwise OR'ing this
         * value). Otherwise the PSL shall clear the corresponding bit
         * in iCapabilities.
         */
        static const TChargerDetectorCaps KChargerDetectorCapChirpCurrentLimiting = 0x00000001;

        public:
        /**
         * Inline constructor for the Charger Detector properties
         * object. This is inline rather than an exported function to
         * prevent a binary break in a case where an older PSL binary
         * might provide the constructor a smaller object due to the
         * PSL being compiled against an older version of the SHAI
         * header. When it's inline, the function is always in sync
         * with the object size.
         *
         * We slightly violate the coding conventions which say that
         * inline functions should be in their own file. We don't want
         * to double the number of USB SHAI headers just for sake of a
         * trivial constructor.
         */
        inline TChargerDetectorProperties() :
            iCapabilities(0)
            {
            };

        public: // Data
        /**
         * A bitmask specifying the static capabilities of this
         * Charger Detector. The PSL fills this field by bitwise
         * OR'ing the TChargerDetectorCaps capability bits
         * corresponding to supported features.
         */
        TChargerDetectorCaps iCapabilities;
        };
    };

#endif //USB_CHARGER_DETECTION_SHAI_SHARED_H
// END of file

