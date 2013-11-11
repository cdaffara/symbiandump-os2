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
    
    @brief USB Peripheral SHAI header
    
    @version 0.4.0
 
    Abstract interface for USB Peripheral Controller.
 
    @publishedDeviceAbstraction
  
 */
 
#ifndef USB_PERIPHERAL_SHAI_H
#define USB_PERIPHERAL_SHAI_H

// System includes
#include <kern_priv.h>
#include <usb/usb_common_shai.h> // Common types shared between the USB SHAIs
#include <usb/usb_peripheral_shai_shared.h> // Common types shared with upper layers

/**
 * This macro specifies the version of the USB Peripheral SHAI header
 * in binary coded decimal format. This allows the PSL layer to
 * confirm a certain definition is available, if needed. This can for
 * example make it possible for a new PSL to support compilation in an
 * older environment with old USB SHAI version that is missing some
 * new definitions.
 */
#define USB_PERIPHERAL_SHAI_VERSION 0x040

// The namespace is documented in file usb_common_shai.h, so it is not
// repeated here
namespace UsbShai
    {
    /** 
     * Masks defined for TUsbPeripheralEndpointCaps.iSizes.
     *
     * Zero means this endpoint is not available (= no size).
     */
    const TUint KUsbEpNotAvailable = 0x00000000;
    
    /**    
     * Max packet size is continuously variable up to some size specified.
     * (Interrupt and Isochronous endpoints only.)
     */
    const TUint KUsbEpSizeCont     = 0x00000001;
    
    /** Max packet size 8 bytes is supported */
    const TUint KUsbEpSize8        = 0x00000008;
    
    /** Max packet size 16 bytes is supported */
    const TUint KUsbEpSize16       = 0x00000010;
    
    /** Max packet size 32 bytes is supported */
    const TUint KUsbEpSize32       = 0x00000020;
    
    /** Max packet size 64 bytes is supported */
    const TUint KUsbEpSize64       = 0x00000040;
    
    /** Max packet size 128 bytes is supported */
    const TUint KUsbEpSize128      = 0x00000080;
    
    /** Max packet size 256 bytes is supported */
    const TUint KUsbEpSize256      = 0x00000100;
    
    /** Max packet size 512 bytes is supported */
    const TUint KUsbEpSize512      = 0x00000200;
    
    /** Max packet size 1023 bytes is supported */
    const TUint KUsbEpSize1023     = 0x00000400;
    
    /** Max packet size 1024 bytes is supported */
    const TUint KUsbEpSize1024     = 0x00000800;
    
    /** 
     * Test Mode Selectors (Set/ClearFeature)
     * Refer to usb specification 2.0, Chapter 9.4.9, table 9-7
     */
    enum TUsbTestModeSelector
        {
        EUsbTestSelector_Test_J = 0x01,
        EUsbTestSelector_Test_K,
        EUsbTestSelector_Test_SE0_NAK,
        EUsbTestSelector_Test_Packet,
        EUsbTestSelector_Test_Force_Enable
        };
    
    /** 
     * Transfer direction of a USB transfer request.
     * @see TUsbPeripheralRequest
     */
    enum TTransferDirection 
        {
        EControllerNone,        
        EControllerRead,
        EControllerWrite
        };

    /** 
     * Enumeration listing the event codes that are passed from PSL to
     * PIL to indicate peripheral events
     */
    enum TUsbPeripheralEvent
        {
        /** The USB Suspend bus state has been detected.
         *
         * When the Peripheral Controller PSL is part of an
         * OTG-capable port, the PSL shall report this event also when
         * operating as the A-peripheral, in which case this final
         * suspend event starts the HNP role switch back to A-host.
         */
        EUsbEventSuspend,
        
        /** USB Resume signaling has been detected. */
        EUsbEventResume,
        
        /** A USB Reset condition has been detected. */
        EUsbEventReset,
        
        /**
         * VBUS level has risen above the B-session valid threshold. This
         * is only relevant for a stand-alone peripheral controllers
         * (those not associated with an OTG port). For an OTG port, the
         * VBUS state is reported to PIL layer by the OTG Controller PSL.
         */
        EUsbEventVbusRisen,
        
        /**
         * VBUS level has fallen below the B-session valid threshold. This
         * is only relevant for a stand-alone peripheral controllers
         * (those not associated with an OTG port). For an OTG port, the
         * VBUS state is reported to PIL layer by the OTG Controller PSL.
         */
        EUsbEventVbusFallen
        };   

    typedef TUint32 TUsbPeripheralPacketArray;

    /** 
     * @brief  A USB transfer request.
     * 
     * This class will be constructed in PIL or upper layer, and be
     * passed down to PSL. After the request was done, PSL layer will
     * modify corresponding members (like data length, packet info
     * etc.) and re-pass it back to PIL via interface
     * MUsbPeripheralPilCallbackIf::EndpointRequestComplete().
     * 
     */
    NONSHARABLE_CLASS(TUsbPeripheralRequest)
        {
    public:    
        /**
         * This class doesn't need a destructor because it has nothing to be
         * freed, closed, destroyed. It 'owns' nothing.
         */
        IMPORT_C TUsbPeripheralRequest(TInt aRealEpNum);
        
    public:
        /** The 'real' endpoint number this request to be bind to. */
        TInt iRealEpNum;
        
        /** 
         * Start address of this buffer. Filled-in by PIL.
         *
         * PSL needs to put received data in this buffer if it's a read request.
         * PSL needs to get data from this buffer if it is a write request.
         */
        TUint8* iBufferStart;
        
        /** Physical address of buffer start (used for DMA). */
        TUintPtr iBufferAddr;
        
        /** Length in bytes to be read/write. Filled-in by PIL. */
        TInt iLength;

        /** 
         * Number of bytes transmitted (if it is a write);
         * This value need to be updated by PSL. 
         */
        TUint iTxBytes;
        
        /** 
         * Number of packets received (if it is a read); 
         * This value need to be updated by PSL.
         */
        TUint iRxPackets;
        
        /** 
         * This is a re-interpretation of what's inside in buffer iBufferStart.
         * It is designed to be that the first packet always contains all the
         * bytes received. So only the following scenario are possible:
         *
         * 1. Nothing, iRxPackets is zero, no packet received.
         * 
         * 2. 1 data Packet, iRxPackets is one.
         *    iPacketIndex[0] is the offset against iBufferStart of where the
         *    first byte of received data begins.
         *    iPacketSize[0] is the size (in bytes) of the received data.
         * 
         * 3. 1 data packet, and one ZLP packet, iRxPackets is two.
         *    iPacketIndex[0] is the offset against iBufferStart of where the
         *    first byte of received data begins.
         *    iPacketSize[0] is the size (in bytes) of the data packet.
         *    
         *    iPacketIndex[1] is the offset of the ZLP packet.
         *    iPacketSize[1] is always set to zero since ZLP contains no data.
         * 
         */
        TUsbPeripheralPacketArray* iPacketIndex;
        
        /** Array of packet sizes. Details see iPacketIndex */
        TUsbPeripheralPacketArray* iPacketSize;
        
        /**
         * Indicates the transfer direction of this request.
         * Note, SetupEndpointRead/Write likewise functions already point out 
         * the transfer direction, PSL may select to use it or not.
         */
        TTransferDirection iTransferDir;
            
        /** 
         * For EControllerWrite (IN) transfers, it is used to Indicates that if 
         * a Zero Length Packet is required at the end of this transfer.
         * It will be set to ETrue if ZLP is needed, EFalse otherwise.
         */
        TBool iZlpReqd;
        
        /** 
         * The error code upon completion of the request;
         * PSL need to set it to KErrNone if no errors, Set it to other 
         * system error codes otherwise.
         */
        TInt iError;
        };
        
    /** 
     * Peripheral controller capabilities.
     * 
     * PSL should use those values to fill in data member iControllerCaps in
     * structure TPeripheralControllerProperties.
     *
     * @see TPeripheralControllerProperties.
     */
    typedef TUint32 TDeviceCaps;
    
    /**
     * Indicates whether peripheral controller hardware supports USB High-speed.
     * This capability affects driver functionality and behavior throughout the 
     * implementation.
     * 
     * Set this bit if this peripheral controller supports USB High-speed;
     * Clear it otherwise.
     */
    const TDeviceCaps KDevCapHighSpeed = 0x00000001;
    
    /** 
     * Indicates whether the peripheral controller supports hardware acceleration
     * for setting the device address, i.e. automatically setting device address
     * on conclusion of status phase of SET_ADDRESS.
     *
     * If this capability is supported PIL will call SetDeviceAddress() before
     * sending a zero byte status packet.
     *
     * Set this bit if hardware acceleration is supported.
     * Clear it otherwise.
     */
    const TDeviceCaps KDevCapSetAddressAcceleration = 0x00000002;
    
    /**
     * Indicates whether controller support Session Request Protocol.
     *
     * Set this bit if SRP is supported.
     * Clear it otherwise.
     */
    const TDeviceCaps KDevCapSrpSupport = 0x00000004;
    
    /**
     * Indicates whether controller support Host Negotiation Protocol.
     *
     * Set this bit if HNP is supported.
     * Clear it otherwise.
     */
    const TDeviceCaps KDevCapHnpSupport = 0x00000008;
    
    /**
     * Indicates whether controller support Remote wakeup
     * 
     * Set this bit if remote wakeup is supported.
     * Clear it otherwise.
     */
    const TDeviceCaps KDevCapRemoteWakeupSupport = 0x00000010;

    /** 
     * Configuration data from PSL to PIL when register a controller through
     * interface UsbPeripheralPil::RegisterPeripheralController().
     */
    NONSHARABLE_CLASS(TPeripheralControllerProperties)
        {
        public:
        TPeripheralControllerProperties();
        
        public: // Data
        /**
         * A bitmap used to Indicates the capabilities of this controller.
         * PSL should set the corresponding bit to '1' if one capability is
         * supported by this controller.
         */
        TDeviceCaps iControllerCaps;

        /**
         * Pointer to a DFC queue that will be used for DFCs of this controller.
         *
         * A stand-alone (one not associated with an OTG-port) peripheral PSL 
         * implementation must supply a pointer to a dedicated DFC queue that 
         * has been created for this controller. Both the PSL itself and the PIL
         * layer must queue their DFCs for this controller in this DFC queue to
         * ensure the code is running in the same context.
         *
         * A peripheral PSL that is part of an OTG port will be registered by 
         * the OTG PSL. In this case, the DFC queue supplied here must be the 
         * same DFC queue as the one supplied in the properties of the OTG 
         * Controller.
         */
        TDfcQue* iDfcQueue;
        
        /** 
         * Describe the capabilities of all endpoint owned by this controller.
         *
         * Note that there might be gaps in the array, because the endpoints 
         * must be numbered by using the 'real endpoint' numbering scheme.
         *
         * An example of end point capability array:
         *
         * @code
         * static const TUsbPeripheralEndpointCaps DevEndpointCaps[KTotalEps] =
         * {
         * // MaxSize mask  ,   Types&Dir                      , High  ,Reserved
         *                                                       speed?
         * {KEp0MaxPktSzMask,(KUsbEpTypeControl | KUsbEpDirOut), ETrue ,0 , 0},
         * {KEp0MaxPktSzMask,(KUsbEpTypeControl | KUsbEpDirIn ), ETrue ,0 , 0},
         *      
         * {KUsbEpNotAvailable,KUsbEpNotAvailable              , ETrue ,0 , 0},
         *      
         * {KBlkMaxPktSzMask,(KUsbEpTypeBulk | KUsbEpDirIn )   , ETrue ,0 , 0},
         * {KBlkMaxPktSzMask,(KUsbEpTypeBulk | KUsbEpDirOut)   , ETrue ,0 , 0}
         * };
         * @endcode
         *
         * For endpoint maxinum packet size on USB2.0 High-speed, PSL should
         * provide the overlaid values for both full speed and high speed, as 
         * PIL can deduce the appropriate values for either speed.
         *
         */
        const TUsbPeripheralEndpointCaps* iDeviceEndpointCaps;
        
        /** 
         * Set by the PSL to indicate the number of entries in 
         * iDeviceEndpointCaps array.
         * Be noted that this value include the ones that are marked as 
         * KUsbEpNotAvailable in table iDeviceEndpointCaps.
         */
        TInt iDeviceTotalEndpoints;
        
        /** 
         * Maximum size of ep0 packet.
         * @see USB spec 2.0, chapter 9, table 9-8.
         */
        TUint8  iMaxEp0Size;
        
        /** 
         * Device Release number in binary-coded decimal.
         * @see USB spec 2.0, chapter 9, table 9-8.
         */
        TUint16  iDeviceRelease;
        };
    
    /** 
     * Enumeration of stages within a control transfer
     * 
     * Possible stages defined in USB spec include:
     * 1. setup -> status in
     * 2. setup -> data in -> status out
     * 3. setup -> data out -> status in
     */
    enum TControlStage
        {
        /** 
         * Control transfer always start with a setup stage in which a setup
         * packet is expected.         
         */
        EControlTransferStageSetup,
        
        /** The stage that a data packet is expected from be received */
        EControlTransferStageDataOut,
        
        /**
         * Status in stage can either following a Setup stage (if no data stage)
         * or following a data out stage.
         */
        EControlTransferStageStatusIn,
                
        /** 
         * Depends on the request type of a setup packet, a data in stage maybe 
         * followed after a setup packet.
         *
         * Within this stage we expect upper/ourself to send some data to host.
         */
        EControlTransferStageDataIn,
        
        /**
         * This is an optional stage, some controller will silently swallow
         * Status out packet and not able to report it.
         * PIL has consider this situation when control transfer state machine
         * is introduced.
         */
        EControlTransferStageStatusOut,
               
        EControlTransferStageMax
        };
    
    /** 
     * Packet types in control transfers.
     * 
     * PSL need to tell PIL about the types of each packet it received.
     * @see MUsbPeripheralPilCallbackIf::Ep0RequestComplete().
     */
    enum TControlPacketType
        {
        /** Indicates a setup packet */
        EControlPacketTypeSetup,
        
        /** Indicates a data out packet */
        EControlPacketTypeDataOut,
        
        /** 
         * Indicates a status in packet.
         * optional, PSL can select not to complete this packet.
         */
        EControlPacketTypeStatusIn,
        
        /** Indicates a data in packet */
        EControlPacketTypeDataIn,
        
        /** 
         * Indicates that a status out packet.
         * optional, PSL can select not to complete this packet.
         */
        EControlPacketTypeStatusOut,

        };
    
    /** 
     * @brief Interface for PSL to do callback to PIL
     * 
     * This is the interface that a PSL need talk with PIL in order to:
     * 1. Pass device event (reset/suspend/resume/vbus high/vbus low etc) to PIL.
     * 2. Complete a read/write request to PIL.
     * 3. Query control transfer stage from PIL.
     * 3. Other functions like SetAdress/Handle HNP/Get remote wake up etc.
     *
     * This interface already been implemented and exported via library 
     * usbperipheralpil.lib. PSL need to add this library as a dependency in 
     * you PSL .mmp file.
     *
     * @lib  usbperipheralpil.lib
     */
    NONSHARABLE_CLASS(MUsbPeripheralPilCallbackIf)
        {
        public:
        /** 
         * Used to synchronize the Ep0 Stage machine between PSL and PIL.
         * Accepts a SETUP packet and returns the next Ep0 Stage.
         *
         * This function will NOT lead anything to be changed at PIL, it is 
         * functioning by parsing the supplied buffer only.
         *
         * @param aSetupBuf The SETUP packet received by PSL.
         * @return The next Ep0 Stage at PIL if PIL receive this setup packet.
         *
         */
        virtual TControlStage EnquireEp0NextStage(const TUint8* aSetupBuf) const = 0;
        
        /** 
         * This function gets called by PSL upon completion of a pending 
         * endpoint zero data transfer request.
         *
         * @param aRealEndpoint Either 0 for Ep0 OUT (= Read)
         *                      or 1 for Ep0 IN (= Write).
         * @param aCount The number of bytes received or transmitted.
         * @param aError The error code of the completed transfer request. 
         *               KErrNone if no error.
         *               KErrCancel if transfer was cancelled.
         *               KErrPrematureEnd if a premature end was encountered.
         * @param aPktType The type of the packet that being completed.
         * 
         * @return KErrNone if no error during transfer completion processing;
         *  KErrGeneral if request was a read & a Setup packet was received &
         *  the recipient for that packet couldn't be found (invalid packet: Ep0
         *  has been stalled); KErrNotFound if the request was a read & the 
         *  recipient for that packet (Setup or data) _was_ * found - however no
         *  read had been set up by that recipient (this case should be used by 
         *  PSL to disable Ep0 interrupt at that point and give the upper layer 
         *  software time to set up a new Ep0 read; once the 'missing' read was 
         *  set up either Ep0DataReceiveProceed or Ep0ReadSetupPktProceed of 
         *  MPeripheralControllerIf class  will be called by PIL).
         */
        virtual TInt Ep0RequestComplete(TInt aRealEndpoint, 
                                        TInt aCount, 
                                        TInt aError, 
                                        TControlPacketType aPktType) = 0;
    
        /** 
         * PSL call this function upon completion of a pending data transfer 
         * request. This function isn't to be used for endpoint zero completions
         * (use Ep0RequestComplete instead).
         * 
         * @param aCallback A pointer to a data transfer request callback 
         *        structure which was previously passed to PSL in a 
         *        SetupEndpointWrite() or SetupEndpointRead() call.
         * 
         */
        virtual void EndpointRequestComplete(TUsbPeripheralRequest* aCallback) = 0;
        
        /**
         * This function gets called by PSL upon detection of either of the 
         * following events:
         *   - USB Reset,
         *   - USB Suspend event,
         *   - USB Resume signaling,
         *
         * When the Peripheral Controller PSL is part of an
         * OTG-capable port, the PSL shall report the above events
         * also when operating as the A-peripheral, including
         * reporting the final suspend event that starts the HNP role
         * switch back to A-host.
         *
         * In addition to the events above, a stand-alone peripheral
         * controller PSL (one not associated with an OTG port) is
         * required to report the following events:
         *   - VBUS rising above the session valid threshold
         *   - VBUS falling below the session valid threshold
         * 
         * When the peripheral controller is part of an OTG port, the
         * PIL layer receives VBUS notifications via the OTG stack,
         * and the peripheral PSL is not expected to report them via
         * this interface.
         *
         * When USB Battery Charging is supported on the
         * peripheral-only port, there is a dependency between normal
         * USB functionality and USB Battery Charging (see
         * usb_charger_detection_shai.h and specifically the
         * description of class MChargerDetectorIf). In this case it
         * is the responsibility of the Peripheral Controller PSL to
         * communicate with the Charger Detector PSL (which it may
         * implement itself) with respect to VBUS events.
         *
         * When VBUS rises on the peripheral-only port that fully
         * supports Battery Charging Specification Revision 1.1, the
         * Charger Detector PSL and the Peripheral Controller PSL need
         * to together guarantee that Data Contact Detect is completed
         * and the port type detected before notifying VBUS rising.
         * When the port type is known, the port type needs to be
         * notified to the Charger Detector PSL Observer, followed by
         * notifying a VBUS rising event to the Peripheral Controller
         * PIL callback interface (via this function).
         *
         * Where Data Contact Detect is not supported, the VBUS rise
         * event needs to be notified to the Peripheral Controller PIL
         * callback interface (via this function) immediately and
         * charger detection needs to proceed in parallel with the
         * upper layers preparing the USB personality. This is
         * necessary in order to ensure that we can fulfill the
         * requirement to connect to the bus within a second, while
         * still making as long as possible charger detection cycle to
         * minimize the changes of false detections due to datalines
         * not making contact yet.
         *
         * The Peripheral Controller PSL and the Charger Detector PSL
         * need to together guarantee that the peripheral controller
         * does not attempt to connect to the bus while charger
         * detection is still on-going. When detection has been
         * completed and upper layers have indicated readiness to
         * connect to the bus (see MPeripheralControllerIf::PeripheralConnect(),
         * the Peripheral Controller PSL must connect to the bus.
         *
         * @param aEvent An enum denoting the event that has occurred.
         *        
         * @return KErrArgument if the event is not recognized, KErrNone
         *         otherwise
         */
        virtual TInt DeviceEventNotification(TUsbPeripheralEvent aEvent) = 0;
        
        /** 
         * This function should be called by PSL once the peripheral 
         * controller (and thus the USB device) is in the Address state.
         */
        virtual void MoveToAddressState() = 0;
        
        /** 
         * This function should be called by PSL after reception of an Ep0
         * SET_FEATURE request with a feature selector of either {b_hnp_enable,
         * a_hnp_support, a_alt_hnp_support}, but ONLY when that Setup packet is
         * not handed up to PIL (for instance because it is auto-decoded and
         * 'swallowed' by the peripheral controller hardware).
         * 
         * @param aHnpState A bitmask indicating the present state of the three
         * OTG feature selectors as follows:
         * 
         * bit.0 == a_alt_hnp_support
         * bit.1 == a_hnp_support
         * bit.2 == b_hnp_enable
         *
         * @see TUsbHnpCaps
         * 
         */
        virtual void HandleHnpRequest(TInt aHnpState) = 0;       
        
        /**
         * Returns the current USB device state.
         * 
         * 'USB device state' here refers to the device states as defined 
         * in chapter 9 of the USB specification.
         * 
         * @return The current USB device state, or EUsbPeripheralStateUndefined
         *         if peripheral stack is disabled.
         *
         */
        virtual TUsbPeripheralState DeviceStatus() const = 0;
        
        };
    
    /** 
     * MPeripheralControllerIf
     * @brief Abstract class for USB peripheral Chipset API.
     * @version   0.2.
     *
     * PSL driver need to implement all those virtual functions in this class.
     *    
     * Four Steps to create a vendor specific usb peripheral PDD.
     *
     *    Step 1: Derive a new class from MUsbPeripheralControllerIf and 
     *            implement all interfaces defined. 
     *              
     *    Step 2: Create a new instance (we call it inst_of_psl) of the new class.
     *            Prepare the configuration data defined by structure
     *            TUsbPeripheralControllerProps (let's call it cfg_data )
     *
     *    Step 3: Call UsbPeripheralPil::RegisterPeripheralController(inst_of_psl,cfg_data).
     *                        
     *    Step 4: Member function SetPilCallbackInterface() will be called. 
     *            An instance of MUsbPeripheralPilCallbackIf will be passed to 
     *            PSL via this call. Record it since PSL need to reuse it later 
     *            for any PIL functionality callback.
     *            
     *    That's all.
     *    
     *    dynamic view of PDD creation.
     *            PIL will be compiled and linked alone.
     *            PSL will be compiled alone and linked against a new lib named
     *            usbperipheralpil.lib.
     *
     *    For PSL vendors, the following stuff are provided:
     *            usb_peripheral_shai.h & usb_peripheral_shai_shared.h which is 
     *            used for PSL to implement against.
     *            usbperipheralpil.lib which is used for PSL to link against.
     *
     */
    NONSHARABLE_CLASS(MPeripheralControllerIf)
        {
        public: 
        /** 
         * This function will be called once and only once by PIL when a PSL
         * registered itself to PIL by calling 
         * UsbPeripheralPil::RegisterPeripheralController().
         *
         * PSL is expected to store the callback interface pointer so that make 
         * it possible to reporting events back to PIL layer.
         *
         * For stand-alone peripheral controllers (controllers not associated 
         * with an OTG port),the Peripheral Controller PSL is required to report
         * a EUsbEventVbusRisen event by calling MUsbPeripheralPilCallbackIf::
         * DeviceEventNotification() if VBUS is already high at the point this 
         * function is called.
         *
         * @param aPilCallbackIf A reference to PIL callback interface which
                  a PSL must use to report events.
         */
        virtual void SetPilCallbackInterface(MUsbPeripheralPilCallbackIf& aPilCallbackIf) = 0;
        
        /**
         * Sets endpoint zero's RX buffer.
         * This buffer is maintains at PIL and always exists.
         *
         * @param aBuffer the buffer address
         * @param aBufferLen the length of the buffer.
         *        
         */
        virtual void SetEp0RxBuffer(TUint8* aBuffer, TInt aBufferLen) = 0;
        
        /** 
         * Determines the speed mode that this controller is operating at.
         *
         * @return one of the speed mode defined by TSpeed.
         * @see TSpeed.
         */
        virtual TSpeed DeviceOperatingSpeed() = 0;
        
        /** 
         * Forces the peripheral controller into a non-idle state to perform a 
         * USB remote wakeup operation.
         *
         * PIL layer will make sure the following preconditions are meet before
         * calling this function:
         *   1. we're already in suspended state for at least 5ms (Twtrsm).
         *   2. remote wakeup had been enabled by host.
         *
         * @return KErrNone if successful, otherwise one of the system-wide 
         *         error codes.
         */
        virtual TInt SignalRemoteWakeup() = 0;

        /** 
         * This function will be called by PIL upon decoding a SET_ADDRESS
         * request. PIL will make sure that a status packet will be sent before
         * calling this function as USB spec required if capability of 
         * KDevCapSetAddressAcceleration is not support by hardware; Otherwise
         * SetAddress call will go ahead of status packet.
         * @see KDevCapSetAddressAcceleration
         *
         * @param aAddress A valid USB device address that was received with the
         *        SET_ADDRESS request.
         *
         * @return KErrNone if address was set successfully or if controller's 
         *         address cannot be set manually. otherwise one of the system-
         *         wide error codes.
         */
        virtual TInt SetDeviceAddress(TInt aAddress) = 0;
        
        /** 
         * Configures (enables) an endpoint (incl. Ep0) for data transmission or
         * reception.
         *
         * @param aRealEndpoint The number of the endpoint to be configured.
         * @param aEndpointInfo A properly filled-in endpoint info structure.
         *
         * @return KErrNone if endpoint successfully configured; KErrArgument if
         *         endpoint number or endpoint info invalid; otherwise one of 
         *         the system-wide error codes.
         */
        virtual TInt ConfigureEndpoint(TInt aRealEndpoint, 
                                       const TUsbPeripheralEndpointInfo& aEndpointInfo) = 0;
        
        /** 
         * De-configures (disables) an endpoint (incl. Ep0).
         *
         * @param aRealEndpoint The number of the endpoint to be disabled.
         *
         * @return KErrNone if endpoint successfully de-configured; KErrArgument
         *         if endpoint number invalid; otherwise one of the system-wide 
         *         error codes.
         */
        virtual TInt DeConfigureEndpoint(TInt aRealEndpoint) = 0;
        
        /** 
         * Queries the use of and endpoint resource.
         *
         * @param aRealEndpoint The number of the endpoint.
         * @param aResource The endpoint resource to be queried.
         *        they are one of the following currently:
         *           KUsbEndpointInfoFeatureWord1_DMA.
         *           KUsbEndpointInfoFeatureWord1_DoubleBuffering.
         *
         * @return ETrue if the specified resource is in use at the endpoint, 
         *         EFalse if not.
         */
        virtual TBool QueryEndpointResource(TInt aRealEndpoint, TUint32 aResource) const = 0;
        
        /** 
         * Sets up a read request on an endpoint (excl. Ep0) for data reception.
         * 
         * PIL will construct a read request as aCallback, and use this function
         * to setup and indicate the readiness for reading data from end point.
         * 
         * PSL need to update related data member of aCallback and complete this
         * request by calling EndpointRequestComplete() when request is done.
         * 
         * @see TUsbPeripheralRequest
         *
         * @param aRealEndpoint The number of the endpoint to be used.
         * @param aCallback A properly filled-in (by PIL) request callback.
         *
         * @return KErrNone if read successfully set up; KErrArgument if endpoint
         *         number is invalid. otherwise one of the system-wide error 
         *         codes.
         */
        virtual TInt SetupEndpointRead(TInt aRealEndpoint, TUsbPeripheralRequest& aCallback) = 0;
        
        /** 
         * Sets up a write request on an endpoint (excl. Ep0) for transmission.
         *
         * PIL will contruct a write request as aCallback, and use this function
         * to pass down it to PSL.
         * 
         * PSL need to update related data member of aCallback and complete this
         * request by calling EndpointRequestComplete() when data is transmitted
         * or any error found.
         * @see TUsbPeripheralRequest
         *
         * @param aRealEndpoint The number of the endpoint to be used.
         * @param aCallback A properly filled-in request callback.
         *
         * @return KErrNone if write successfully set up; KErrArgument if 
         *         endpoint number is invalid; otherwise one of the system-wide 
         *         error codes.
         */
        virtual TInt SetupEndpointWrite(TInt aRealEndpoint, TUsbPeripheralRequest& aCallback) = 0;
        
        /** 
         * Cancels a read request on an endpoint (excl. Ep0).
         *
         * Note that endpoint 0 read requests are never cancelled by PIL, so
         * there is also no CancelEndpointZeroRead() function.
         *
         * @param aRealEndpoint The number of the endpoint to be used.
         *
         * @return KErrNone if read successfully cancelled; KErrArgument if 
         *         endpoint number is invalid; otherwise one of the system-wide 
         *         error codes.
         */
        virtual TInt CancelEndpointRead(TInt aRealEndpoint) = 0;
        
        /** 
         * Cancels a write request on an endpoint (incl. Ep0).
         *
         * PIL calls this function also to cancel endpoint zero write requests.
         *
         * @param aRealEndpoint The number of the endpoint to be used.
         *
         * @return KErrNone if write successfully cancelled; KErrArgument if 
         *         endpoint number is invalid; otherwise one of the system-wide 
         *         error codes.
         */
        virtual TInt CancelEndpointWrite(TInt aRealEndpoint) = 0;
        
        /** 
         * Same as SetupEndpointRead(), but for endpoint zero only.
         *
         * No callback structure is used here as reading to ep0 (host may at any
         * time send a SETUP packet) must be ready even before upper application
         * is ready. So, a buffer will be passed down to PSL by 
         * MPeripheralControllerIf::SetEp0RxBuffer() during PIL construction,
         * this will benefits the PSL to have a buffer always available for this
         * purpose.
         *
         * PSL must complete this request by EndpointRequestComplete()
         *
         * @return KErrNone if read successfully set up; otherwise one of the 
         *         system-wide error codes.
         */
        virtual TInt SetupEndpointZeroRead() = 0;
        
        /** 
         * Same as SetupEndpointWrite(), but for endpoint zero only.
         *
         * No callback is used here as this function is only used by
         * PIL layer only and no user side request exists for it.
         *
         * @param aBuffer This points to the beginning of the data to be sent.
         * @param aLength The number of bytes to be sent.
         * @param aZlpReqd ETrue if a ZLP is must be sent after the data.
         *
         * PSL must complete this request by EndpointRequestComplete()
         *
         * @return KErrNone if write successfully set up; otherwise one of the 
         *         system-wide error codes.
         */
        virtual TInt SetupEndpointZeroWrite(const TUint8* aBuffer, 
                                            TInt aLength, 
                                            TBool aZlpReqd = EFalse) = 0;
        
        /** 
         * Sets up on Ep0 for the transmission of a single zero-length packet.
         *
         * @return KErrNone if ZLP successfully set up; otherwise one of the 
         *         system-wide error codes.
         */
        virtual TInt SendEp0ZeroByteStatusPacket() = 0;
        
        /** 
         * Stalls an endpoint (incl. Ep0).
         *
         * Not applicable to Isochronous endpoints.
         *
         * @param aRealEndpoint The number of the endpoint to be stalled.
         *
         * @return KErrNone if endpoint successfully stalled; KErrArgument if 
         *         endpoint number is invalid; otherwise one of the system-wide 
         *         error codes.
         */
        virtual TInt StallEndpoint(TInt aRealEndpoint) = 0;
        
        /** 
         * Clears the stall condition on an endpoint (incl. Ep0).
         *
         * Not applicable to Isochronous endpoints.
         *
         * @param aRealEndpoint The number of the endpoint to be stalled.
         *
         * @return KErrNone if endpoint successfully de-stalled; KErrArgument if
         *         endpoint number invalid; otherwise one of the system-wide 
         *         error codes.
         */
        virtual TInt ClearStallEndpoint(TInt aRealEndpoint) = 0;
        
        /**
         * Resets the data toggle bit for an endpoint (incl. Ep0).
         *
         * Isochronous endpoints don't use data toggles.
         *
         * @param aRealEndpoint The number of the endpoint to be used.
         *
         * @return KErrNone if data toggle successfully reset; KErrArgument if 
         *         endpoint number invalid; otherwise one of the system-wide 
         *         error codes.
         */
        virtual TInt ResetDataToggle(TInt aRealEndpoint) = 0;
        
        /** 
         * Returns the frame number of the last received SOF packet.
         *
         * @return The (11-bit) frame number of the last received SOF packet.
         *
         */
        virtual TInt SynchFrameNumber() const = 0;
        
        /** 
         * Stores the (11-bit) frame number that should be sent in response to
         * the next SYNCH_FRAME request(s).
         *
         * @param aFrameNumber The (11-bit) frame number to be set
         */
        virtual void SetSynchFrameNumber(TInt aFrameNumber) = 0;
        
        /** 
         * Starts peripheral controller.
         *
         * This initializes the device controller hardware before any other 
         * operation can be performed. 
         * Tasks to be carried out here might include:
         *     - resetting the whole peripheral controller design
         *     - enabling the peripheral controller's clock
         *     - binding & enabling the peripheral controller (primary) interrupt
         *     - write meaningful values to some general controller registers
         *     - enabling the USB Reset, Suspend, and Resume interrupts
         *     - enabling the peripheral controller proper (for instance by 
         *       setting an Enable bit).
         *
         * This function is also used in an OTG environment when the
         * peripheral stack becomes enabled and the controller needs
         * to be started and enabled for peripheral use. If the HW
         * platform shares resources such as clocks and power between
         * the host and peripheral HW, it is probably easiest for the
         * PSL to handle the role switch based on the
         * MOtgControllerIf::SetControllerRole() call to the OTG
         * Controller interface.
         *
         * @return KErrNone if the controller was successfully started;
         *         otherwise one of the system-wide error codes.
         *
         * @see usb_otg_shai.h, MOtgControllerIf::SetControllerRole()
         */
        virtual TInt StartPeripheralController() = 0;
    
        /** 
         * Stops the peripheral controller.
         *
         * This basically makes undone what happened in StartPeripheralController(). 
         * Tasks to be carried out here might include:
         * - disabling peripheral controller proper.
         * - disabling the USB Reset interrupt.
         * - disabling & unbinding the peripheral controller (primary) interrupt.
         * - disabling the peripheral controller's clock.
         *
         * This function is also used in an OTG environment when the peripheral 
         * stack becomes disabled and the peripheral hardware resources can be 
         * released. If the hardware platform shares resources such as clocks 
         * and power between the host and peripheral hardware, it is probably 
         * easiest for the PSL to handle the role switch based on the
         * MOtgControllerIf::SetControllerRole() call to the OTG Controller 
         * interface.
         *
         * @return KErrNone if peripheral controller successfully stopped;
         *         otherwise one of the system-wide error codes.
         *
         * @see usb_otg_shai.h, MOtgControllerIf::SetControllerRole()
         */         
        virtual TInt StopPeripheralController() = 0;
    
        /** 
         * This function is used to indicate that upper layers are ready to 
         * start operating as peripheral and want to connect to the USB bus
         * if possible.
         *
         * Since this functionality is not part of the USB specification it
         * has to be explicitly supported, either by the peripheral controller
         * itself or by the hardware platform.
         *
         * This call merely reports the upper layer readiness to connect, and
         * it is the responsiblity of the PSL to assess when all prerequisites
         * for connection are satisfied and physically connect when they are.
         * These prerequisites include:
         *
         *  1. Upper layer readiness has been indicated by calling 
         *     PeripheralConnect().
         *
         *  2. The peripheral controller has been started and enabled
         *     by StartPeripheralController().
         *
         * The connection readiness assessment responsibility is delegated to the
         * PSL because the physical connection may sometimes be handled by a HW
         * state machine, thus making the correct SW-controlled connection time
         * HW-specific.
         *
         * @return KErrNone if peripheral controller successfully connected;
         *         otherwise one of the system-wide error codes.
         */
        virtual TInt PeripheralConnect() = 0;
    
        /** 
         * Called to indicate that upper layers no longer want to operate as the
         * peripheral and we must disconnect from the USB bus.
         *
         * Since this functionality is not part of the USB specification it
         * has to be explicitly supported, either by the peripheral controller
         * itself or by the hardware platform.
         *
         * @return KErrNone if controller is successfully disconnected;
         *         otherwise one of the system-wide error codes.
         */
        virtual TInt PeripheralDisconnect() = 0;       
        
        /**
         * Implements anything peripheral controller might required by following
         * bus resume signal.
         *
         * This function gets called by PIL after it has been notified (by
         * PSL) about the Suspend condition.
         */
        virtual void Suspend() = 0;
        
        /**
         * Implements anything peripheral controller might required by following
         * bus resume signal.
         *
         * This function gets called by PIL after it has been notified (by
         * PSL) about the Resume event.
         */
        virtual void Resume() = 0;
        
        /**          
         * Implements anything peripheral controller might required by following
         * bus resume signal.
         *
         * This function gets called by PIL after it has been notified (by
         * PSL) about the Reset event.
         */
        virtual void Reset() = 0;
        
        /** 
         * PIL call this function to signal PSL that it has finished processing
         * a received Setup packet (on Ep0) and that PSL can now prepare 
         * itself for the next Ep0 reception.
         *
         * The reason for having this function is that the situation where no 
         * Ep0 read has been set up by user and thus a received Setup packet
         * cannot immediately be delivered to user. Once user however 
         * sets up an Ep0 read, PIL then completes the request and eventually 
         * calls this function. We can implement some sort of flow-control by
         * this way.
         */
        virtual void Ep0ReadSetupPktProceed() = 0;
        
        /** 
         * PIL call this function to signal PSL that it has finished processing 
         * a received Ep0 data packet and that PSL can now prepare itself for 
         * the next Ep0 reception.
         *
         * The reason for having this function is that the situation where no 
         * Ep0 read has been set up by user and thus a received Setup packet
         * cannot immediately be delivered to user. Once user however 
         * sets up an Ep0 read, PIL then completes the request and eventually 
         * calls this function. We can implement some sort of flow-control by
         * this way.
         */
        virtual void Ep0ReadDataPktProceed() = 0;
        
        /** 
         * Puts controller into a specific test mode (during HS operation only).
         *
         * 9.4.9 Set Feature: "The transition to test mode must be complete no
         * later than 3 ms after the completion of the status stage of the 
         * request." (The status stage will have been completed immediately 
         * before this function gets called.)
         *
         * @param aTestSelector The specific test mode selector.
         *
         * @return KErrNone if the specified test mode was entered successfully;
         *         otherwise one of the system-wide error codes.
         */
        virtual TInt EnterTestMode(TUsbTestModeSelector aTestSelector) = 0;
        };

   
    /**
     * This static class provides the interface which PSL implementation shall 
     * use to register itself to PIL layer.
     */
    NONSHARABLE_CLASS(UsbPeripheralPil)
        {
        public:
        /**
         * Registration function to be used by a stand-alone (non-OTG
         * port) Peripheral Controller PSL to register itself to the
         * PIL layer. Peripheral Controllers that are part of an
         * OTG-port are registered by the OTG Controller PSL (see
         * usb_otg_shai.h, function UsbOtgPil::RegisterOtgController).
         *
         * The intended usage is that each stand-alone Peripheral
         * Controller PSL is a kernel extension that registers itself
         * to the USB Peripheral PIL layer by making this call from
         * their own kernel extension entry point function (or an
         * equivalent code that runs during bootup).
         *
         * When the PSL makes this call, PIL layer will call 
         * SetPilCallbackInterface() function of the supplied Peripheral 
         * Controller interface to supply the PSL a reference to PIL 
         * callback interface to be used for reporting peripheral events.
         *
         * @param aPeripheralControllerIf Reference to the Peripheral
         *   Controller interface implemented by the registering PSL.
         *   The PIL layer requires that the supplied reference
         *   remains valid indefinitely, as the Peripheral Controller
         *   cannot unregister.
         *
         * @param aProperties Reference to an object describing the
         *   static properties of the Peripheral Controller. The PIL
         *   takes a copy and the PSL is free to release the properties
         *   object upon return.
         *
         * @lib usbperipheralpil.lib
         */
        IMPORT_C static void RegisterPeripheralController(
            MPeripheralControllerIf& aPeripheralControllerIf, 
            const TPeripheralControllerProperties& aProperties );
    
        private:
        /**
         *  No instance of this class need to be created
         */
        UsbPeripheralPil();
        };
    };

#include <usb/usb_peripheral_shai.inl>
    
#endif // USB_PERIPHERAL_SHAI_H

// End of File
