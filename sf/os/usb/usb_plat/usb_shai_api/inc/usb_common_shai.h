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
    @brief USB SHAI header for common types and constants
    @version 0.2.0

    This header specifies common types and constants used by both the
    peripheral and host/OTG USB SHAIs. It is automatically included by
    the USB Host and Peripheral SHAI headers and is not intended to be
    directly included by SHAI implementations.

    The file is versioned for the sake of the human reader, but a
    compile-time constant to show the header version is not available,
    as this header is not intended to be directly included by PSL
    implementations. If a change in this header impacts the overall
    USB SHAI interface (i.e. change other than documentation
    improvements), the version of the impacted interfaces (USB Host
    SHAI, USB OTG SHAI, and/or USB Peripheral SHAI) shall too be
    changed to make the change visible to the PSL.

    @publishedDeviceAbstraction
*/


#ifndef USB_COMMON_SHAI_H
#define USB_COMMON_SHAI_H

// System includes
#include <kern_priv.h>

/**
 * @brief USB SHAI types and interfaces
 *
 * This namespace holds all USB SHAI related types, constants,
 * classes, and functions.
 */
namespace UsbShai
    {
    // Data types

    /**
     * Enumeration specifying the available USB endpoint types.
     */
    enum TEndpointType
        {
        /** Endpoint type is control */
        EEpTypeControl = 0,
        
        /** Endpoint type is bulk */
        EEpTypeBulk,

        /** Endpoint type is interrupt */
        EEpTypeInterrupt,

        /** Endpoint type is isochronous */
        EEpTypeIsochronous
        };

    /**
     * Enumeration specifying the available USB speeds.
     */
    enum TSpeed
        {
        /** The bus operates in low speed mode */
        ELowSpeed = 0,
            
        /** The bus operates in full speed mode */
        EFullSpeed,
            
        /** The bus operates in high speed mode */
        EHighSpeed
        };
    };

#endif // USB_COMMON_SHAI_H

/* End of File */
