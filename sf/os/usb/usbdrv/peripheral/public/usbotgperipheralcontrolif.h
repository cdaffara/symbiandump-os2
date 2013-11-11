// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the License "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// e32\include\drivers\usbcshared.h
// Kernel side definitions for the USB Device driver stack (PIL + LDD).
// 
//

/**
 @file usbotgperipheralcontrolif.h
 @internalTechnology
*/

#ifndef USBOTGPERIPHERALCONTROLIF_H
#define USBOTGPERIPHERALCONTROLIF_H

#include <usb/usb_peripheral_shai.h> // For UsbShai::TUsbPeripheralEvent

/**
 * This M-class specifies the observer interface that the OTG state
 * machine implements to listen to events of the peripheral stack.
 */
NONSHARABLE_CLASS(MUsbOtgPeripheralObserverIf)
    {
    public:
    /**
     * Called by the peripheral stack to report a peripheral event to
     * the OTG stack.
     *
     * @param aEvent An enumeration value specifying the type of event
     *   that has occurred
     */
    virtual void NotifyPeripheralEvent(UsbShai::TUsbPeripheralEvent aEvent) = 0;
    };


/**
 * This M-class specifies the control interface that the OTG state
 * machine uses to control the peripheral stack and register for
 * peripheral events.
 */
NONSHARABLE_CLASS(MUsbOtgPeripheralControlIf)
    {
    public:
    /**
     * Called by the OTG state machine to enable the peripheral stack.
     * When called, the peripheral stack must activate the controller
     * and connect to the bus as soon as possible.
     */
    virtual void EnablePeripheralStack() = 0;

    /**
     * Called by the OTG state machine to disable the peripheral
     * stack. When called, the peripheral stack must immediately
     * disconnect from the bus and deactivate the controller.
     */
    virtual void DisablePeripheralStack() = 0;

    /**
     * Called by the OTG state machine to set the peripheral event
     * observer.
     *
     * @param aObserver Pointer to the OTG Observer, or NULL when the
     *   OTG stack is being shutdown.
     */
    virtual void SetOtgObserver(MUsbOtgPeripheralObserverIf* aObserver) = 0;
    };


#endif    // USBOTGPERIPHERALCONTROLIF_H
