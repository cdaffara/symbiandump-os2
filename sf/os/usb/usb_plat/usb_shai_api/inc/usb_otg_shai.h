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
    @brief USB OTG SHAI header
    @version 0.3.0

    This header specifies the USB OTG SHAI.

    @publishedDeviceAbstraction
*/


#ifndef USB_OTG_SHAI_H
#define USB_OTG_SHAI_H

// System includes
#include <kern_priv.h>
#include <usb/usb_common_shai.h> // Common types shared between the USB SHAIs

/**
 * This macro specifies the version of the USB OTG SHAI header in
 * binary coded decimal format. This allows the PSL layer to confirm a
 * certain definition is available, if needed. This can for example
 * make it possible for a new PSL to support compilation in an older
 * environment with old USB SHAI version that is missing some new
 * definitions.
 */
#define USB_OTG_SHAI_VERSION 0x030

// The namespace is documented in file usb_common_shai.h, so it is not
// repeated here
namespace UsbShai
    {
    // Forward declarations

    class MOtgControllerIf;
    class MOtgObserverIf;
    class MPeripheralControllerIf;
    class TPeripheralControllerProperties;
    class MHostControllerIf;
    class THostControllerProperties;

    // Data types

    /**
     * Enumeration listing the possible states of the ID pin. Due to a
     * dependency between OTG and USB Battery Charging, this
     * enumeration lists also the special states introduced as part of
     * the Battery Charging Specification version 1.1.
     *
     * An OTG Controller PSL for a system that does not support
     * Accessory Charger Adapter (ACA) will always report only
     * EIdStateRidFloat or EIdStateRidGnd. An OTG Controller PSL that
     * supports ACA is required to report the ID pin state accurately
     * in order for the OTG State Machine to understand why VBUS
     * appears high even though we should default to the host role and
     * should normally drive VBUS ourselves (in case of RID_A).
     *
     * Reporting an ACA state via the ID pin notification mechanism is
     * not a substitute for reporting port type detection via the USB
     * Charger Detection SHAI that is documented separately in
     * usb_charger_detection_shai.h. See documentation of
     * MOtgObserverIf::NotifyIdPinAndVbusState() for more details.
     *
     * @see usb_charger_detection_shai.h
     */
    enum TIdPinState
        {
        /** ID pin is grounded */
        EIdStateRidGnd = 0,

        /** ID pin is floating */
        EIdStateRidFloat,

        /**
         * ID pin is in the RID_A range, as specified in Battery
         * Charging 1.1 specification
         */
        EIdStateRidA,

        /**
         * ID pin is in the RID_B range, as specified in Battery
         * Charging 1.1 specification
         */
        EIdStateRidB,

        /**
         * ID pin is in the RID_C range, as specified in Battery
         * Charging 1.1 specification
         */
        EIdStateRidC
        };

    /**
     * Enumeration listing the reported states of VBUS on the OTG
     * port.
     *
     * The threshold for Session Valid comparison is VOTG_SESS_VLD as
     * defined in the "On-The-Go and Embedded Host Supplement to the
     * USB Revision 2.0 Specification", Table 4-1 Electrical
     * Characteristics. The voltage level for a compliant
     * implementation can be anywhere between 0.8 V and 4.0 V.
     */
    enum TVbusState
        {
        /** VBUS is below the OTG Session Valid threshold */
        EVbusStateNoSession = 0,

        /**
         * VBUS is above the OTG Session Valid threshold, but below
         * the requirements of AVbusValid.
         */
        EVbusStateSessionValid,

        /**
         * When operating as the A-device and driving VBUS, indicates
         * that the VBUS is in regulation, as defined in "On-The-Go
         * and Embedded Host Supplement to the USB Revision 2.0
         * Specification" Section 4.2.1, "VBUS Output Voltage and
         * Current".
         *
         * When a VBUS session has been started as the A-device, the
         * OTG Controller PSL is required to report a VBUS level of
         * EVbusStateAVbusValid when VBUS has successfully risen to
         * allow the OTG State Machine to transition out of the
         * a_wait_vrise state.
         *
         * After VBUS has been successfully raised when operating as
         * the A-device, the PSL reporting a VBUS level less than
         * EVbusStateAVbusValid is considered as a report of a VBUS
         * error (over-current) situation and will result in ending
         * the session immediately.
         */
        EVbusStateAVbusValid
        };

    /**
     * Enumeration listing the state of the OTG 2.0 state machine.
     *
     * The states match those defined in the "On-The-Go and Embedded
     * Host Supplement to the USB Revision 2.0 Specification" for the
     * OTG A-device and B-device states.
     */
    enum TOtgState
        {
        /** The OTG state is b_idle */
        EOtgStateBIdle = 0,

        /** The OTG state is b_peripheral */
        EOtgStateBPeripheral,

        /** The OTG state is b_wait_acon */
        EOtgStateBWaitAcon,

        /** The OTG state is b_host */
        EOtgStateBHost,

        /** The OTG state is a_idle */
        EOtgStateAIdle,

        /** The OTG state is a_wait_vrise */
        EOtgStateAWaitVrise,

        /** The OTG state is a_wait_bcon */
        EOtgStateAWaitBcon,

        /** The OTG state is a_host */
        EOtgStateAHost,

        /** The OTG state is a_suspend */
        EOtgStateASuspend,

        /** The OTG state is a_peripheral */
        EOtgStateAPeripheral,

        /** The OTG state is a_wait_vfall */
        EOtgStateAWaitVfall,

        /** The OTG state is a_vbus_err */
        EOtgStateAVbusErr
        };

    /**
     * Enumeration listing the roles that our device can be in.
     */
    enum TOtgRole
        {
        /**
         * Our device is idle, i.e. we are not operating in either the
         * peripheral or the host role. This role indicates that
         * neither the host controller nor the peripheral controller
         * needs to be activated and the PSL is free to power down the
         * controllers.
         */
        EOtgRoleIdle = 0,

        /** Our device is operating in the peripheral role */
        EOtgRolePeripheral,
    
        /** Our device is operating in the host role */
        EOtgRoleHost
        };

    // Class declaration

    /**
     * An interface class that needs to be implemented by each OTG
     * Controller PSL that registers to the USB stack.
     *
     * The USB OTG Stack will call the functions of this interface
     * from a DFC queued on the DFC queue supplied by the OTG
     * Controller PSL in TOtgControllerProperties::iControllerDfcQueue
     * when the OTG Controller PSL registered.
     */
    NONSHARABLE_CLASS( MOtgControllerIf )
        {
        public:
        /**
         * Called by the OTG stack to set the observer callback
         * interface to be used by the OTG Controller PSL to report
         * events.
         *
         * When the observer pointer is set to non-NULL value, the OTG
         * Controller PSL is required to immediately report the
         * current VBUS and ID states to the observer to get the
         * status of the OTG stack up to date.
         *
         * If the OTG Controller PSL has detected that we are attached
         * to a bad device already before the OTG Observer was set by
         * this function, the PSL needs to immediately report ID
         * floating and VBUS low, and report the bad device by calling
         * MOtgObserverIf::NotifyBadDeviceAttached(). See the
         * documentation of MOtgObserverIf::NotifyBadDeviceAttached()
         * for more information on the detection of bad devices.
         *
         * @param aObserver Pointer to the observer interface to use,
         *   or NULL when the OTG stack is being unloaded.
         */
        virtual void SetOtgObserver( MOtgObserverIf* aObserver ) = 0;

        /**
         * When operating as the B-peripheral, the OTG stack calls
         * this function to indicate that the upper layers are
         * requesting our device to become the B-host. This means that
         * our OTG device will need start the HNP signalling by
         * disconnecting from the bus and allowing the A-device to
         * connect as the peripheral. The signalling shall be started
         * when the host has suspended the bus, which may already be
         * the case or happen later.
         *
         * This function call is only relevant for OTG controllers
         * that implement the HNP signalling in hardware and require
         * an explicit request from SW to start the HNP role switch.
         * If the OTG controller is under SW control by the Host and
         * Peripheral Controller PSLs, the OTG Controller PSL should
         * ignore this call. The HNP signalling in the SW-controlled
         * case will be handled as normal calls to disconnect as
         * peripheral and then start the host controller.
         *
         * For all controller types, the Host Controller PSL and the
         * Peripheral Controller PSL associated with the OTG port are
         * required to report events as they normally would when
         * operating as the default role. For the Host Controller PSL,
         * this includes notifying device connection, disconnection,
         * and other relevant events via MRootHubCallbackIf. For the
         * Peripheral Controller PSL, this includes notifying bus
         * state events such as reset, suspend, and resume via
         * MUsbPeripheralPilCallbackIf::DeviceEventNotification.
         *
         * The OTG Controller PSL is not required to perform any
         * monitoring of HNP success or failure, or report that to the
         * PIL layer. The PSL is only responsible for making the
         * normal host and peripheral notifications mentioned above,
         * and the PIL can see the HNP success or failure from those
         * notifications.
         */
        virtual void SetBHnpRequest() = 0;

        /**
         * When operating as the B-device, the OTG stack calls this
         * function to request the PSL to drive SRP signalling on the
         * bus by pulsing the D+ dataline. The OTG Controller PSL may
         * synchronously drive the 5..10 millisecond pulse before
         * returning, but it may also do it asynchronously.
         *
         * The OTG PIL layer guarantees that the initial conditions
         * for driving SRP are satisfied before the PIL calls this
         * function. That is, the PIL guarantees that sufficient time
         * has elapsed since the end of the previous VBUS session (if
         * any) and the bus has been idle long enough.
         *
         * No special report from the OTG Controller PSL is required
         * in either success or fail case. In a success case, the
         * A-device will raise VBUS and the OTG state machine gets
         * this as a normal VBUS notifications from the OTG Controller
         * PSL. In a fail case, a timer in the upper layers will
         * expire, indicating to upper layers that the SRP was not
         * successful.
         */
        virtual void SignalBSrp() = 0;

        /**
         * Called by the OTG state machine to indicate a change in the
         * required controller role.
         *
         * Whether the PSL needs to do any actions depends on the HW.
         * For controllers that require special configuration in
         * changing a role (other than just starting the peripheral
         * controller or the host controller normally), the OTG
         * Controller should do that special configuration when it
         * gets this call.
         *
         * When changing a role, the OTG state machine will first
         * disable the stack for the previous role, causing that stack
         * to issue a stop request to the respective controller
         * PSL. The OTG state machine will then call this function
         * SetControllerRole() to set the controller role to the
         * target role. Following this, the OTG state machine will
         * enable the stack for the target role, causing that stack to
         * issue a start request to the respective controller PSL.
         *
         * @param aControllerRole The OTG role to set our device to
         *
         * @return KErrNone if the OTG Controller successfully set the
         *   role or required no actions. Otherwise a system-wide
         *   error code.
         */
        virtual TInt SetControllerRole( TOtgRole aControllerRole ) = 0;
        };


    /**
     * An interface class implemented by the USB stack to allow the
     * OTG controller to report events to the USB stack. This includes
     * events like VBUS rising and falling, ID pin becoming grounded
     * or floating, and SRP being detected.
     *
     * It is required that the OTG Controller PSL calls these
     * functions in the context of the DFC queue supplied by the OTG
     * Controller PSL in TOtgControllerProperties::iControllerDfcQueue
     * when the OTG Controller PSL registered.
     */
    NONSHARABLE_CLASS( MOtgObserverIf )
        {
        public:
        /**
         * Notify the current ID-pin and VBUS state to the OTG
         * stack. This needs to be called by the OTG Controller PSL
         * everytime there is a change in the ID pin or VBUS
         * level. Redundant notifications that don't change the
         * previously reported state are silently ignored, so the
         * function is safe to call without worrying about extra
         * calls.
         *
         * When USB Battery Charging is supported on the OTG-capable
         * port, there is a dependency between normal USB
         * functionality and USB Battery Charging (see
         * usb_charger_detection_shai.h and specifically the
         * description of class MChargerDetectorIf). In this case it
         * is the responsibility of the OTG Controller PSL to
         * communicate with the Charger Detector PSL (which it may
         * implement itself) with respect to VBUS and ID pin events.
         *
         * Due to Accessory Charger Adapter specified in Battery
         * Charging 1.1, the ID pin state is relevant for both
         * charging and OTG. The USB OTG SHAI and the USB Charger
         * Detection SHAI receive the ID pin notifications from the
         * PSL independently. For ID and VBUS events that indicate
         * connection to an Accessory Charger Adapter, it is required
         * that a BC 1.1 capable PSL reports the state to both the
         * Charger Detector PSL Observer (via
         * MChargerDetectorObserverIf::NotifyPortType), and the OTG
         * Observer (via this function). The reporting order is not
         * significant and can be freely chosen by the PSL.
         *
         * When VBUS rises on the OTG-capable port that is currently
         * the B-device and fully supports Battery Charging
         * Specification Revision 1.1, the Charger Detector PSL and
         * the OTG Controller PSL need to together guarantee that Data
         * Contact Detect is completed and the port type detected
         * before reporting the ID and VBUS state. When the port type
         * is known, the port type needs to be notified to the Charger
         * Detector PSL Observer, followed by notifying the ID and
         * VBUS state to the OTG Observer (via this function).
         *
         * Where Data Contact Detect is not supported, the initial
         * B-device VBUS rise event needs to be notified to the OTG
         * Observer (via this function) immediately and charger
         * detection needs to proceed in parallel with the upper
         * layers preparing the USB personality. This is necessary in
         * order to ensure that we can fulfill the requirement to
         * connect to the bus within a second, while still making as
         * long as possible charger detection cycle to minimize the
         * chances of false detections due to datalines not making
         * contact yet.
         *
         * The OTG Controller PSL, the Peripheral Controller PSL and
         * the Charger Detector PSL need to together guarantee that
         * the peripheral controller does not attempt to connect to
         * the bus while charger detection is still on-going. When
         * detection has been completed and upper layers have
         * indicated readiness to connect to the bus (see
         * MPeripheralControllerIf::PeripheralConnect(), the
         * Peripheral Controller PSL must connect to the bus.
         *
         * @param aIdPinState The current ID-pin state
         * @param aVbusState The current VBUS state
         */
        virtual void NotifyIdPinAndVbusState( TIdPinState aIdPinState,
                                              TVbusState  aVbusState ) = 0;

        /**
         * When operating as the A-device with VBUS low, notify the
         * OTG stack that SRP signalling has been detected on the
         * bus. The OTG Controller must detect the SRP signalling from
         * dataline pulsing, as specified in the "On-The-Go and
         * Embedded Host Supplement to the USB Revision 2.0
         * Specification".
         */
        virtual void NotifySrpDetected() = 0;

        /**
         * This function is called by the OTG Controller PSL to report
         * that it has detected the attachment of a device (A or B)
         * that is malfunctioning in a low-level way that prevents
         * attempting communication with the connected device. Such
         * cases may include but are not necessarily limited to:
         *
         * 1. A B-device that drives its upstream VBUS. To prevent
         * damage to the VBUS charge pump in our A-device, it may be
         * necessary to prevent VBUS from being raised by our device.
         *
         * 2. A B-device that presents a single-ended one (both
         * datalines high) on the bus.
         *
         * The detection of such malfunctioning devices is left to the
         * OTG Controller PSL, as this type of malfunctions are
         * low-level problems not necessarily detectable with the
         * standard inputs available to the OTG state machine.
         *
         * To ensure that the OTG state machine stays in an idle
         * state, the OTG Controller PSL should report ID floating and
         * VBUS low prior to reporting the bad device attachment by
         * calling this function. When the bad device is detached, the
         * OTG Controller PSL can resume reporting ID and VBUS state
         * truthfully, and must call NotifyBadDeviceDetached() to
         * allow the upper layers to see the error condition has been
         * cleared.
         *
         * @see NotifyBadDeviceDetached()
         */
        virtual void NotifyBadDeviceAttached() = 0;

        /**
         * This function is called by the OTG Controller PSL to report
         * that a previously detected bad device has been detached.
         * See NotifyBadDeviceAttached() for description of this
         * functionality.
         *
         * @see NotifyBadDeviceAttached()
         */
        virtual void NotifyBadDeviceDetached() = 0;
        };


    /**
     * This class specifies the information provided by an OTG
     * Controller PSL when registering to the USB OTG stack.
     *
     * The PSL should prepare for the possibility that members may be
     * added to the end of this class in later SHAI versions if new
     * information is needed to support new features. The PSL should
     * not use this class as a direct member in an object that is not
     * allowed to grow in size due to binary compatibility reasons.
     *
     * @see UsbOtgPil::RegisterOtgController()
     */
    NONSHARABLE_CLASS( TOtgControllerProperties )
        {
        public: // Types and constants
        /**
         * A bitmask type used to indicate the static capabilities of
         * the OTG Controller.
         */
        typedef TUint32 TOtgCaps;

        public:
        /**
         * Inline constructor for the OTG Controller properties
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
        inline TOtgControllerProperties() :
            iCapabilities(0),
            iControllerDfcQueue(NULL)
            {
            };

        public: // Data
        /**
         * A bitmask specifying the static capabilities of this OTG
         * Controller. No capabilities are specified at the moment and
         * the PSL shall fill this field with a zero value.
         *
         * The field is added for sake of future proofing the binary
         * compatibility of the OTG SHAI. By having a field reserved
         * for capability bits, we can later specify bits to indicate
         * added virtual functions or extension to this controller
         * properties structure. The PIL layer can then at runtime
         * confirm the existence of the new functions or fields and
         * safely support an older binary, if we choose to.
         */
        TOtgCaps iCapabilities;

        /**
         * Pointer to a DFC queue that will be used for DFCs of this
         * controller and the associated peripheral and host
         * controllers.
         *
         * The OTG Controller must supply a pointer to a dedicated DFC
         * queue that has been created for this OTG Controller PSL.
         * Both the OTG Controller PSL itself and the OTG stack must
         * queue their DFCs for this controller in this DFC queue to
         * ensure the code is running in the same context.
         *
         * Furthermore, it is the responsibility of the OTG Controller
         * PSL that registers to ensure that the Peripheral and Host
         * Controller PSLs that are registered at the same time use
         * the same DFC Queue in their respective properties object.
         */
        TDfcQue* iControllerDfcQueue;
        };


    /**
     * A static class implemented by the USB OTG PIL layer to allow
     * the OTG controller PSL to register to the PIL layer.
     */
    NONSHARABLE_CLASS( UsbOtgPil )
        {
        public:
        /**
         * Registration function to be used by the OTG Controller PSL
         * to register itself and the associated peripheral and host
         * controller PSLs to the PIL layer.
         *
         * The intended usage is that OTG Controller PSL (of which
         * only one can exists in a given system) is a kernel
         * extension that registers itself and the associated
         * peripheral and host controller PSLs to the USB PIL layer by
         * making this call from its kernel extension entry point
         * function (or an equivalent code that runs during bootup).
         *
         * @param aOtgControllerIf Reference to the OTG Controller
         *   interface implemented by the registering PSL. The PIL
         *   layer requires that the supplied reference remains valid
         *   indefinitely, as the OTG Controller cannot unregister.
         *
         * @param aOtgProperties Reference to an object describing the
         *   static properties of the OTG Controller. The PIL takes a
         *   copy and the PSL is free to release the properties object
         *   upon return.
         *
         * @param aPeripheralControllerIf Reference to the Peripheral
         *   Controller interface implemented by the PSL controlling the
         *   Peripheral Controller associated with the registering OTG
         *   port. The PIL layer requires that the supplied reference
         *   remains valid indefinitely, as the OTG Controller cannot
         *   unregister.
         *
         * @param aPeripheralProperties Reference to an object
         *   describing the static properties of the Peripheral
         *   Controller. The PIL takes a copy and the PSL is free to
         *   release the properties object upon return.
         *
         * @param aHostControllerIf Reference to the Host Controller
         *   interface implemented by the PSL controlling the Host
         *   Controller associated with the registering OTG port. The
         *   PIL layer requires that the supplied reference remains
         *   valid indefinitely, as the OTG Controller cannot
         *   unregister.
         *
         * @param aHostProperties Reference to an object describing the
         *   static properties of the Host Controller. The PIL takes a
         *   copy and the PSL is free to release the properties object
         *   upon return.
         *
         * @lib usbotghostpil.lib
         */
        IMPORT_C static void RegisterOtgController(
            MOtgControllerIf&                      aOtgControllerIf,
            const TOtgControllerProperties&        aOtgProperties,
            MPeripheralControllerIf&               aPeripheralControllerIf, 
            const TPeripheralControllerProperties& aPeripheralProperties,
            MHostControllerIf&                     aHostControllerIf,
            const THostControllerProperties&       aHostProperties );
        };
    };

#endif // USB_OTG_SHAI_H

/* End of File */
