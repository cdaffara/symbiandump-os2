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
    @brief USB Host SHAI header
    @version 0.3.0

    This header specifies the USB host SHAI.

    @publishedDeviceAbstraction
*/


#ifndef USB_HOST_SHAI_H
#define USB_HOST_SHAI_H

// System includes
#include <kern_priv.h>
#include <usb/usb_common_shai.h> // Common types shared between the USB SHAIs

/**
 * This macro specifies the version of the USB Host SHAI header in
 * binary coded decimal format. This allows the PSL layer to confirm a
 * certain definition is available, if needed. This can for example
 * make it possible for a new PSL to support compilation in an older
 * environment with old USB SHAI version that is missing some new
 * definitions.
 */
#define USB_HOST_SHAI_VERSION 0x030

// The namespace is documented in file usb_common_shai.h, so it is not
// repeated here
namespace UsbShai
    {
    // Forward declarations

    class MHostControllerIf;
    class MHostTransferCallbackIf;
    class MRootHubCallbackIf;
    class MRootHubIf;

    // Data types

    /**
     * An enumeration listing the host role electrical test modes that
     * require special support from the host controller. These
     * correspond to the test modes specified in Section "6.4.1.1 Test
     * Modes" of the "On-The-Go and Embedded Host Supplement to the
     * USB Revision 2.0 Specification" Revision 2.0 (a.k.a. USB OTG
     * 2.0).
     *
     * The single-step control transfer tests are performed as
     * transfers on the control pipe, but a special transfer flag
     * THostTransfer::KSingleStepControlDelayedDataPhase requests the
     * Host Controller PSL to delay the DATA phase when needed.
     *
     * Suspend and resume tests are performed using the normal suspend
     * and resume functionality in the USB Host SHAI, so they are not
     * a special case for the Host Controller PSL.
     *
     * @see MHostControllerIf::EnterHostTestMode()
     */
    enum THostTestMode
        {
        /**
         * Drive SE0 until the host controller is reset.
         */
        ETestModeTestSE0NAK = 0,

        /**
         * Drive high-speed J until the host controller is reset.
         */
        ETestModeTestJ,

        /**
         * Drive high-speed K until the host controller is reset.
         */
        ETestModeTestK,

        /**
         * Send test packets until the host controller is reset. The
         * format of the required test packet is specified in Section
         * "7.1.20 Test Mode Support" of the USB 2.0 specification.
         */
        ETestModeTestPacket
        };


    // Class declaration

    /**
     * This class specifies a memory block that is used for USB
     * transfers. The memory pointers and the buffer length are filled
     * by the USB Host stack before issuing the transfer to the Host
     * Controller PSL.
     */
    NONSHARABLE_CLASS( TUsbMemoryBlock )
        {
        public:
        /**
         * Linear (virtual) start address of the memory buffer. This
         * address is always provided.
         */
        TLinAddr iLinAddr;

        /**
         * Physical start address of the memory buffer. This address
         * is always provided, but it is the responsibility of the
         * Host Controller PSL to guarantee that the memory is
         * physically contiguous, if contiguous memory is needed. The
         * PSL can guarantee this by implementing
         * MHostControllerIf::ChunkCreate() to allocate physically
         * contiguous memory.
         *
         * @see MHostControllerIf::ChunkCreate()
         */
        TPhysAddr iPhysAddr;

        /**
         * Pointer to an array of physical addresses each pointing to
         * one physical page of the memory block. This address is
         * always provided.
         *
         * The size of each page matches the size reported in
         * THostControllerProperties::iControllerPageSize by the Host
         * Controller PSL when it registered to the PIL.
         */
        TPhysAddr* iPhysPages;

        /** The length of the memory buffer in bytes */
        TUint iLength;
        };


    /**
     * This class specifies the enumerated types for different endpoint
     * configuration items and specifies the structure of the endpoint
     * configuration. Instances of this class are used to specify the
     * endpoint information when opening a pipe.
     *
     * @see MHostControllerIf::OpenPipe()
     */
    NONSHARABLE_CLASS( THostPipe )
        {
        public: // Types and constants
        /**
         * A bitmask type used to indicate some boolean properties of
         * the endpoint targeted by this pipe.
         */
        typedef TUint32 TEndpointFlags;

        // These are flag constants that specify the values that can
        // be bitwise OR'ed to the endpoint flags field iEndpointFlags
        // to indicate some boolean parameters of the endpoint

        /**
         * This flag specifies whether the targeted endpoint is behind
         * a hub that is performing transaction translation. This
         * occurs when the attached device is a low/full-speed device
         * attached to a high-speed hub.
         *
         * When this flag is set, the iHubAddr and iHubPort members
         * specify the hub address and the port to which the targeted
         * device is connected, and the iEndpointFlags flag
         * KHubHasMultipleTTs specifies whether the hub has been
         * configured to use multiple transaction translators.
         */
        static const TEndpointFlags KHubTranslates = 0x00000001;

        /**
         * When the iEndpointFlags flag KHubTranslates is set, this
         * flag is set if the hub has multiple transaction
         * translators.
         */
        static const TEndpointFlags KHubHasMultipleTTs = 0x00000002;

        public: // Data
        /**
         * The type of the endpoint (control, bulk, interrupt,
         * isochronous)
         */
        TEndpointType iType;

        /** Maximum packet size to be used on this endpoint */
        TUint iMaxPacketSize;

        /** Flags specifying some boolean parameters of the endpoint */
        TEndpointFlags iEndpointFlags;

        /**
         * The speed (low, full, high) to use for this endpoint. This
         * may differ from the current speed of the local port if the
         * targeted device is a low-speed or full-speed device
         * connected to a high-speed hub.
         */
        TSpeed iSpeed;

        /**
         * The polling interval specified as number of frames or
         * microframes, depending on the speed. This is used for
         * interrupt and isochronous endpoints only.
         */
        TUint iPollingInterval;

        /**
         * The USB address of the remote device that we are
         * communicating with
         */
        TUint8 iRemoteAddr;

        /**
         * The remote endpoint number that we are communicating
         * with. This is the physical endpoint number in the USB 2.0
         * descriptor format, i.e. the most significant bit indicates
         * the direction (1 for IN, 0 for OUT), and the least
         * significant bits indicate the endpoint number.
         */
        TUint8 iRemoteEpNum;

        /**
         * For cases where the attached device is a low/full-speed
         * device attached to a high-speed hub that does the necessary
         * transaction translation, this member records the address of
         * the hub doing the translation.
         */
        TUint8 iHubAddr;

        /**
         * For cases where the attached device is a low/full-speed
         * device attached to a high-speed hub that does the necessary
         * transaction translation, this member records the number of
         * the port where the device is connected on the hub that does
         * the transaction translation.
         */
        TUint8 iHubPort;
        };


    /**
     * This class specifies the structure of the endpoint transfer
     * requests. The USB Host Stack passes the Host Controller PSL a
     * reference to an object of this class when setting up a new transfer
     * with MHostPipeIf::StartTransfer().
     */
    NONSHARABLE_CLASS( THostTransfer )
        {
        public: // Types and constants
        /**
         * A bitmask type used to indicate some boolean properties of
         * a transfer.
         */
        typedef TUint32 TTransferFlags;

        // These are public constants that specify the values that can
        // be bitwise OR'ed to the request flags field iRequestFlags
        // to indicate some boolean parameters of the transfer

        /**
         * This flag is set or cleared by the PIL layer and is only
         * relevant for bi-directional pipes, i.e. control pipes
         * performing control transfers. The bit is not set for other
         * endpoint types.
         *
         * For a control transfer, this bit is set to specify that the
         * transfer direction is IN, i.e. the host is reading from the
         * connected device. For a control transfer with the bit
         * cleared, the transfer direction is OUT, i.e. the host is
         * writing to the connected device.
         *
         * This bit is set or cleared by the USB Host stack before
         * issuing a new transfer.
         */
        static const TTransferFlags KTransferDirIsIN = 0x00000001;

        /**
         * This flag is set or cleared by the PIL layer and is only
         * relevant for bi-directional pipes, i.e. control pipes
         * performing control transfers. The bit is not set for other
         * endpoint types.
         *
         * For a control transfer, this bit is set to specify that the
         * transfer is a special single-step transfer with a delayed
         * data phase. This type of transfer is only used during the
         * high-speed host electrical tests.
         *
         * When this flag is set, the Host Controller PSL should
         * perform the transfer like it normally would, except that it
         * must have a 15 second delay between the completion of the
         * SETUP phase and the sending of the first IN token for the
         * DATA phase.
         *
         * For description and background of the usage of this special
         * transfer option, see the OTG 2.0 Supplement Section 6.4.1.1
         * Test Modes, and specifically the test
         * SINGLE_STEP_GET_DEVICE_DESCRIPTOR_DATA.
         *
         * This bit is set or cleared by the USB Host stack before
         * issuing a new transfer.
         */
        static const TTransferFlags KSingleStepControlDelayedDataPhase = 0x00000002;

        /**
         * This flag is set or cleared by the PIL layer before issuing
         * a new transfer and is relevant for OUT transfers on bulk,
         * interrupt, and control pipes. It is never set for transfers
         * on isochronous pipes.
         *
         * For bulk, interrupt, or control OUT endpoint transfers,
         * indicates whether the Host Controller PSL is required to
         * force a short packet at the end of the transfer.
         *
         * When the bit is set, the Host Controller PSL must send a
         * ZLP (Zero-Length Packet) after the last packet of the
         * transfer, if and only if the last packet was a full packet,
         * i.e. had the same size as the maximum packet size for the
         * endpoint.
         */
        static const TTransferFlags KOutForceShortPacket = 0x00000002;

        /**
         * This flag is set or cleared by the Host Controller PSL
         * before completing the transfer and is only relevant for IN
         * pipes.
         *
         * For IN endpoint transfers, this flag indicates whether the
         * transfer was terminated with a ZLP (Zero-Length Packet).
         */
        static const TTransferFlags KInZlpReceived = 0x00000004;

        /**
         * Enumeration specifying the packet status values to be used
         * for isochronous transfers.
         */
        enum TIsocPacketStatus
            {
            /** No error has occurred, transfer succesful */
            ESuccess = 0,

            /**
             * Data error has occurred in a data transfer, i.e. the
             * CRC did not match
             */
            EDataError,

            /** Overrun has occurred */
            EOverrun,

            /** Underrun has occurred */
            EUnderrun,
            };

        public: // Data
        /**
         * Pointer to the information of the pipe that this transfer
         * is being made on. This is the same information that was
         * used when opening the pipe that the transfer is being made
         * on.
         */
        THostPipe* iPipeInfo;

        /**
         * A pointer to the callback interface to be used for
         * indicating the completion of the transfer. This is set by
         * the USB Host Stack before making a transfer request.
         */
        MHostTransferCallbackIf* iTransferCbIf;

        /**
         * Request flags specifying some boolean parameters for the
         * transfer.
         */
        TTransferFlags iRequestFlags;

        /**
         * Memory block specifying the memory buffer used for the data
         * transfer. This is filled by the USB Host Stack before
         * making a transfer request.
         *
         * The memory block used here will be part of a chunk
         * previously created by a call to
         * MHostControllerIf::ChunkCreate() and the block will
         * fullfill the alignment requirements reported by the call to
         * MHostControllerIf::GetSizeAndAlignment().
         *
         * For control transfers, this specifies the buffer for the
         * data stage and has zero length and NULL pointers if the
         * transaction does not include a data stage.
         */
        TUsbMemoryBlock iDataBuffer;

        /**
         * For control transfers, this memory block specifies the
         * contents of the setup packet to be sent. For other than
         * control transfers, it is not used, and it has zero length
         * and NULL pointers.
         */
        TUsbMemoryBlock iSetupBuffer;

        /**
         * For isochronous transfers, this sets the number of
         * isochronous packets to send/receive. This is set by the
         * USB Host Stack before making a transfer request.
         */
        TInt iNumIsocPackets;

        /**
         * The number of bytes successfully transferred during the
         * data transfer. This is set by the Host Controller PSL
         * before completing the transfer.
         */
        TUint iTransferLength;

        /**
         * The status code of a completed transfer. This is set by the
         * Host Controller PSL before completing the request to the
         * transfer callback interface.
         */
        TInt iStatus;

        /**
         * For isochronous transfers, a pointer to an array of
         * iNumIsoPackets entries that specify the length of each
         * packet in the transfer. For OUT transfers, the array is
         * filled by the USB Host Stack and is read by the Host
         * Controller PSL to decide the length for each packet. For IN
         * transfers, the array is filled by the Host Controller PSL
         * to record to length of each received packet.
         */
        TUint16* iIsocPacketLengths;

        /**
         * For isochronous transfers, a pointer to an array of
         * iNumIsoPackets entries that specify the status of each
         * packet in the transfer. The array is filled by the Host
         * Controller PSL to record to success or failure of each
         * received packet.
         */
        TIsocPacketStatus* iIsocPacketStatuses;
        };


    /**
     * This class specifies the structure and contents of root hub
     * information that is filled by the adaptation to let the USB Host
     * Stack hub driver learn the capabilities of the root hub.
     *
     * @see MHostControllerIf::OpenRootHub
     */
    NONSHARABLE_CLASS( TRootHubInfo )
        {
        public: // Types and constants
        /**
         * A bitmask type used to indicate the static capabilities of
         * the root hub.
         */
        typedef TUint32 TRootHubCaps;

        /**
         * Capability bit to indicate whether the root hub supports
         * user-visible port indications such as LEDs.
         *
         * If the root hub supports this feature, the PSL shall set
         * the corresponding bit in iCapabilities (by bitwise OR'ing
         * this value). Otherwise the PSL shall clear the
         * corresponding bit in iCapabilities.
         */
        static const TRootHubCaps KRootHubCapPortIndication = 0x00000001;

        public: // Data
        /**
         * A bitmask specifying the static capabilities of this root
         * hub. The PSL fills this field by bitwise OR'ing the
         * TRootHubCaps capability bits corresponding to supported
         * features.
         */
        TRootHubCaps iCapabilities;

        /** Number of ports in this root hub */
        TUint iNumberOfPorts;

        /**
         * Maximum number of milliamps of current that this root hub
         * can supply to VBUS.
         */
        TUint iMaxSuppliedVbusCurrentMA;

        /**
         * Time taken by this root hub between starting to power up a
         * port and power being good on that port. This is expressed
         * in units of one millisecond.
         */
        TUint iTimePowerOnToPowerGoodMs;

        /**
         * The maximum speed supported by the root hub.
         */
        TSpeed iMaxSpeed;

        /** Pointer to the root hub interface */
        MRootHubIf* iRootHubIf;
        };


    /**
     * An interface class used by the USB Host Stack to perform data
     * transfers on a USB pipe. The Host Controller PSL must implement
     * this interface and provide a pointer to a pipe-specific
     * instance when the USB Host Stack creates the pipe by calling
     * MHostControllerIf::OpenPipe().
     *
     * The USB Host Stack will call the functions of this interface
     * from a DFC queued on the DFC queue supplied by the Host
     * Controller in THostControllerProperties::iControllerDfcQueue when the
     * Host Controller PSL registered.
     *
     * @see MHostControllerIf::OpenPipe()
     */
    NONSHARABLE_CLASS( MHostPipeIf )
        {
        public:
        /**
         * Close the USB pipe. This is called by the USB Host Stack
         * when the pipe is no longer used and the Host Controller PSL
         * may release any resources that were allocated for this
         * instance, including deleting the object itself.
         *
         * @param aPipeInfo Information describing the target endpoint
         *   of the pipe. This is the same information passed to
         *   MHostControllerIf::OpenPipe() when creating the
         *   pipe.
         *
         * @post After calling the function, the USB Host stack will
         *   consider the invoked instance to have been deleted and will
         *   no longer call any of the functions.
         */
        virtual void ClosePipeD( const THostPipe& aPipeInfo ) = 0;

        /**
         * Clear the data toggle bit of this pipe. This is used for
         * interrupt and bulk pipes when the host stack needs to
         * guarantee that the host controller will start the next
         * interrupt or bulk transfer with data toggle value zero.
         *
         * @param aPipeInfo Information describing the target endpoint
         *   of the pipe. This is the same information passed to
         *   MHostControllerIf::OpenPipe() when creating the
         *   pipe.
         */
        virtual void ClearDataToggle( const THostPipe& aPipeInfo ) = 0;

        /**
         * Start a transfer on this pipe.
         *
         * When the transfer is considered completed, the Host
         * Controller PSL must update the transfer information object
         * with the actual length and status of the transfer and then
         * complete it to the callback interface by calling
         * MHostTransferCallbackIf::CompleteTransfer(). The pointer to
         * the callback interface is provided in the member
         * THostTransfer::iTransferCbIf of the supplied aTransferInfo.
         *
         * To maximize throughput, the PIL layer may call
         * StartTransfer() directly from a previous completion call
         * (see description in
         * MHostTransferCallbackIf::CompleteTransfer()). To prevent
         * recursion, the PSL should not call
         * MHostTransferCallbackIf::CompleteTransfer() directly from
         * within the MHostPipeIf::StartTransfer() call, but should
         * instead queue a DFC to complete the transfer, if
         * immediately completed.
         *
         * The following rules shall be used by the Host Controller
         * PSL to assess the completeness of a transfer:
         *
         * 1. A read or write transfer on a control pipe is completed
         * when all the stages (two or three) of the control transfer
         * have been completed successfully, or when an error
         * occurs. It is the responsibility of the upper layers to
         * guarantee that there is sufficient buffer space for a read
         * request, so the buffer should not run out.
         *
         * 2. A read or write transfer on an isochronous pipe is
         * completed when the number of isochronous packets specified
         * in the member THostTransfer::iNumIsocPackets of the
         * supplied aTransferInfo have been sent or received
         * successfully, or if a fatal error occurs. It is the
         * responsibility of the upper layers to guarantee that there
         * is sufficient buffer space for a read request, so the
         * buffer should not run out.
         *
         * 3. A read transfer on a bulk or interrupt pipe is completed
         * when the buffer space is exhausted, a short packet (a
         * packet smaller than the maximum packet size of the pipe) is
         * received, or an error occurs.
         *
         * 4. A write transfer on a bulk or interrupt pipe is
         * completed when the requested buffer has been fully sent, or
         * an error occurs.
         *
         * @param aTransferInfo Information specifying the transfer to perform
         *
         * @return KErrNone if the transfer was started successfully,
         *   otherwise a system-wide error code
         */
        virtual TInt StartTransfer( THostTransfer& aTransferInfo ) = 0;

        /**
         * Cancel a transfer that was previously started with
         * StartTransfer(). The Host Controller PSL must flush any
         * pending data (either received or about to be sent) and
         * discard the transfer. The PSL must not attempt to complete
         * the canceled transfer by calling
         * MHostTransferCallbackIf::CompleteTransfer().
         *
         * @param aTransferInfo Information specifying the transfer to
         *   cancel. This is the same information passed to
         *   StartTransfer() when the transfer as started.
         */
        virtual void CancelTransfer( THostTransfer& aTransferInfo ) = 0;
        };


    /**
     * An interface class used by the USB Host Stack hub driver to
     * operate on the root hub ports. The Host Controller PSL must
     * implement this interface and provide a pointer when the USB
     * Host Stack opens the root hub by calling
     * MHostControllerIf::OpenRootHub().
     *
     * The USB Host Stack will call the functions of this interface
     * from a DFC queued on the DFC queue supplied by the Host
     * Controller in THostControllerProperties::iControllerDfcQueue
     * when the Host Controller PSL registered.
     *
     * @see MHostControllerIf::OpenRootHub()
     */
    NONSHARABLE_CLASS( MRootHubIf )
        {
        public:
        /**
         * Close the connection to the root hub. This is called by the
         * USB Host Stack when the Host Controller is about to be
         * deleted and the root hub is no longer used. The Host
         * Controller PSL may release any resources that were
         * allocated for this instance, including deleting the object
         * itself.
         *
         * @post After calling the function, the USB Host stack will
         *   consider the invoked instance to have been deleted and will
         *   no longer call any of the functions.
         */
        virtual void CloseRootHubD() = 0;

        /**
         * Enable the power supply, i.e. raise VBUS, on the specified
         * root hub port.
         *
         * If there are problems raising VBUS, the Host Controller PSL
         * must report a VBUS error by notifying the root hub callback
         * interface MRootHubCallbackIf of an over-current condition
         * on the port.
         *
         * @param aPort The number of the root hub port to operate
         *   on. For a root hub with N ports, the port numbers are in
         *   range [1..N].
         *
         * @see MRootHubCallbackIf::OverCurrentDetected()
         */
        virtual void PowerOnPort( TUint aPort ) = 0;

        /**
         * Stop session (A-device only).
         *
         * @param aPort The number of the root hub port to operate
         *   on. For a root hub with N ports, the port numbers are in
         *   range [1..N].
         */
        virtual void PowerOffPort( TUint aPort ) = 0;

        /**
         * Reset the specified root hub port. The total reset length
         * has to be at least 50 milliseconds and the reset signalling
         * must follow the requirements specified in USB 2.0
         * Specification Section 7.1.7.5 Reset Signaling.
         *
         * This function is synchronous and should return when the
         * reset signalling on the port has completed.
         *
         * @param aPort The number of the root hub port to operate
         *   on. For a root hub with N ports, the port numbers are in
         *   range [1..N].
         */
        virtual void ResetPort( TUint aPort ) = 0;

        /**
         * Place the specified root hub port to the USB suspend state
         * where it does not send SOF packets to the connected device.
         *
         * When all ports of the root hub have been suspended, the USB
         * Host Stack will suspend the entire host controller by
         * calling MHostControllerIf::SuspendController().
         *
         * @param aPort The number of the root hub port to operate
         *   on. For a root hub with N ports, the port numbers are in
         *   range [1..N].
         */
        virtual void SuspendPort( TUint aPort ) = 0;

        /**
         * Resume the specified root hub port from the USB suspend
         * state by driving the resume signalling on the port. The
         * signalling must last at least 20 milliseconds and follow
         * the requirements specified in USB 2.0 Specification Section
         * 7.1.7.7 Resume Signaling. After the resume signalling has
         * completed, the port must allow SOFs to be sent to the
         * connected device again.
         *
         * Before resuming the first port of a completely suspended
         * controller, the USB Host Stack will first resume the entire
         * host controller by calling
         * MHostControllerIf::ResumeController().
         *
         * This function is synchronous and should return when the
         * resume signalling on the port has completed.
         *
         * This function is also called when the Host Controller PSL
         * has reported detection of a remote wakeup on a port of the
         * root hub. This is to make sure the host port takes over
         * driving the resume on the bus. The Host Controller PSL or
         * the HW may have already started driving resume upon
         * detecting the remote wakeup, in which case the PSL only
         * needs to wait in this function until the resume signalling
         * has been completed.
         *
         * @param aPort The number of the root hub port to operate
         *   on. For a root hub with N ports, the port numbers are in
         *   range [1..N].
         */
        virtual void ResumePort( TUint aPort ) = 0;

        /**
         * Query whether a device (a hub or a function) is currently
         * connected to the specified root hub port, and what speed it
         * is.
         *
         * @param aPort The number of the root hub port to check for a
         *   connected device on. For a root hub with N ports, the port
         *   numbers are in range [1..N].
         *
         * @param aSpeed Variable to fill with the speed, if connected
         *
         * @return ETrue if a device is currently connected and aSpeed
         *   has been set to the speed of the connected device. EFalse
         *   otherwise.
         */
        virtual TBool IsDeviceConnected( TUint   aPort,
                                         TSpeed& aSpeed ) = 0;
        };


    /**
     * An interface class that needs to be implemented by each Host
     * Controller PSL that registers to the USB stack.
     *
     * This interface is used by the USB Host Stack to do controller level
     * operations like suspending and resuming the host controller. It is
     * also used to open the root hub interface upon creating the SHAI
     * Host Controller, and to open USB pipes to connected devices.
     *
     * The USB Host Stack will call the functions of this interface
     * from a DFC queued on the DFC queue supplied by the Host
     * Controller in THostControllerProperties::iControllerDfcQueue when the
     * Host Controller PSL registered.
     */
    NONSHARABLE_CLASS( MHostControllerIf )
        {
        public:
        /**
         * Open a pipe to an endpoint on a remote USB peripheral.
         *
         * This function is called by the USB host stack when it wants
         * to communicate with a certain endpoint on a discovered USB
         * peripheral. The opened pipe is then used to read or write
         * from/to the peripheral until the pipe is later closed by
         * MHostPipeIf::ClosePipeD().
         *
         * @param aPipeInfo Information describing the target endpoint
         *   that the USB host stack wants to create a pipe to. The
         *   ownership of the info object remains with the USB host
         *   stack and the Host Controller PSL must not refer to data
         *   inside this after the function has returned. All
         *   MHostPipeIf functions that operate on the pipe will be
         *   passed the same pipe information so that the Host
         *   Controller PSL does not necessarily need to take a copy.
         *
         * @param aPipe Upon success, set by the Host Controller PSL
         *   to point to an instance that implements the SHAI pipe
         *   interface for the opened pipe. This interface will then be
         *   used by the USB Host Stack to perform transfers until the
         *   pipe is eventually closed by the USB Host Stack calling
         *   MHostPipeIf::ClosePipeD().
         *
         * @return KErrNone if successful, otherwise a system error code
         */
        virtual TInt OpenPipe( const THostPipe& aPipeInfo,
                               MHostPipeIf*&    aPipe ) = 0;

        /**
         * Open the root hub for the Host Controller.
         *
         * This is called by the USB Host Stack after creating the
         * Host Controller PSL to learn the capabilities of the
         * respective root hub and to establish the communication
         * between the hub driver in the USB Host Stack and the root
         * hub ports controlled by the Host Controller PSL.
         *
         * @param aRootHubInfo Reference to a root hub information
         *   object to be filled by the adaptation.
         *
         * @param aRootHubCbIf Reference to the callback interface of
         *   the root hub driver in the USB stack. The root hub
         *   implementation of the Host Controller PSL shall use this
         *   interface to report events of this root hub.
         *
         * @return KErrNone if successful, otherwise a system error code
         */
        virtual TInt OpenRootHub( TRootHubInfo&       aRootHubInfo,
                                  MRootHubCallbackIf& aRootHubCbIf ) = 0;

        /**
         * Start and enable the host controller. This is called prior
         * to powering up any port of the root hub to allow to Host
         * Controller PSL to perform any preparation that may be
         * necessary to activate the controller. This may include
         * actions such as enabling clocks or power to the host
         * controller HW.
         *
         * This function is also used in an OTG environment when the
         * host stack becomes enabled and the controller needs to be
         * started and enabled for host use. If the HW platform shares
         * resources such as clocks and power between the host and
         * peripheral HW, it is probably easiest for the PSL to handle
         * the role switch based on the
         * MOtgControllerIf::SetControllerRole() call to the OTG
         * Controller interface.
         *
         * @return KErrNone if the host controller was successfully started,
         *         otherwise a system-wide error code.
         *
         * @see usb_otg_shai.h, MOtgControllerIf::SetControllerRole()
         */
        virtual TInt StartHostController() = 0;

        /**
         * Disable and stop the host controller. This is called after
         * powering down all ports of the root hub to allow to Host
         * Controller PSL to release any HW resources needed by the
         * controller. This may include actions such as disabling
         * clocks or power to the host controller HW.
         *
         * This function is also used in an OTG environment when the
         * host stack becomes disabled and the host HW resources can
         * be released. If the HW platform shares resources such as
         * clocks and power between the host and peripheral HW, it is
         * probably easiest for the PSL to handle the role switch
         * based on the MOtgControllerIf::SetControllerRole() call to
         * the OTG Controller interface.
         *
         * @return KErrNone if the host controller was successfully started,
         *         otherwise a system-wide error code.
         *
         * @see usb_otg_shai.h, MOtgControllerIf::SetControllerRole()
         */
        virtual TInt StopHostController() = 0;
        
        /**
         * Place the host controller to the USB suspend state where it
         * does not send SOF packets. The Host Controller PSL may also
         * go into a power-saving mode where it for example shuts down
         * some clocks needed by the host controller when active. The
         * Host Controller PSL must still ensure that the host
         * controller or the root hub will be capable of detecting
         * device disconnection and remote wakeup when suspended.
         */
        virtual void SuspendController() = 0;

        /**
         * Resume the host controller from the suspend state. If the
         * Host Controller PSL put the host controller into a
         * power-saving state by shutting down some clocks, the PSL
         * can re-enable them in this call. This call shall not result
         * in resume signalling to be driven on the bus, as that
         * should be done from MRootHubIf::ResumePort().
         */
        virtual void ResumeController() = 0;

        /**
         * Force the controller into a specific high-speed host test
         * mode. This function needs to be implemented only by Host
         * Controller PSLs for high-speed capable controllers. The
         * function will only be called when the controller is
         * operating in high-speed mode and a special test device has
         * been enumerated.
         *
         * The function shall return immediately when the controller
         * has been set to the requested test mode. The host
         * controller is expected to remain in the test mode until it
         * is shutdown by a call to StopHostController().
         *
         * @param aTestMode Specifies the test mode to enter
         *
         * @return KErrNone if the specified test mode was entered
         *   successfully, otherwise a system-wide error
         */
        virtual TInt EnterHostTestMode( THostTestMode    aTestMode ) = 0;

        /**
         * Get the size and alignment requirements for the Host
         * Controller. The reference parameters are both input and
         * output. Upon input, they describe the values requested by
         * the client, but they may be increased by this function if
         * the Host Controller requires additional memory or has some
         * alignment requirements.
         *
         * @param aType The type of the endpoint that the memory will
         *   be used for.
         *
         * @param aAlignment Output for required alignment in bytes
         *
         * @param aSize Input/output for buffer size (may be increased
         *   in this function)
         *
         * @param aMaxPackets Input/output for maximum number of
         *   packets in the transfer (may be increased in this function)
         *
         * @return KErrNone if successful, otherwise a system-wide
         *   error code
         */
        virtual TInt GetSizeAndAlignment( TEndpointType aType,
                                          TUint&        aAlignment,
                                          TInt&         aSize,
                                          TInt&         aMaxPackets ) = 0;

        /**
         * Creates a shared chunk and commits memory to it. The memory
         * allocated with this function will later be used when making
         * data transfers with this Host Controller (see
         * THostTransfer::iDataBuffer).
         *
         * The adaptation shall create a chunk by calling
         * Kern::ChunkCreate(), and then commit memory to it with one
         * of the Commit functions Kern::ChunkCommit(),
         * Kern::ChunkCommitContiguous(), and Kern::ChunkCommitPhysical().
         *
         * This task is delegated to the adaptation so that it can
         * allocate the correct type of memory. For example, if the
         * Host Controller requires the memory to be physically
         * contiguous, the implementation of this function must commit
         * contiguous memory to the chunk.
         *
         * @param aChunk Output handle to the chunk that was created.
         *   The adaptation must pass this value as the aChunk argument
         *   to the Kern::ChunkCreate() call.
         *
         * @param aOffset The offset (in bytes) from start of chunk,
         *   which indicates the start of the memory region to be
         *   committed. This will be a multiple of the MMU page
         *   size. The adaptation must pass this value as the aOffset
         *   argument to the used Commit call.
         *
         * @param aSize Number of bytes to commit. This will be a
         *   multiple of the MMU page size. The adaptation must pass
         *   this value as the aSize argument to the used Commit call.
         *
         * @param aInfo At input, specifies the chunk properties
         *   requested by the upper layers. The adaptation is allowed
         *   to change the attributes in aInfo.iMapAttr to match the
         *   requirements of the Host Controller. The adaptation must
         *   pass this value as the aInfo argument to the
         *   Kern::ChunkCreate() call. If the adaptation commits
         *   cached memory, it is the responsibility of the adaptation
         *   to deal with cache synchronization when making data
         *   transfers. If the adaptation needs special cleanup when
         *   the chunk is deleted, the adaptation can set a cleanup
         *   DFC pointer in aInfo.iDestroyedDfc.
         *
         * @param aPhysicalAddress On success, will be set to the
         *   physical address of the chunk as referenced by the HC
         *
         * @return KErrNone if successful, otherwise a system-wide
         *   error code
         *
         * @see System documentation for Kern::ChunkCreate(),
         *   Kern::ChunkCommit(), Kern::ChunkCommitContiguous(), and
         *   Kern::ChunkCommitPhysical()
         */
        virtual TInt ChunkCreate( DChunk*&          aChunk,
                                  TUint             aOffset,
                                  TUint             aSize,
                                  TChunkCreateInfo& aInfo,
                                  TPhysAddr&        aPhysicalAddress ) = 0;

        /**
         * Maps virtual to physical addresses. If the memory addresses
         * used by the host controller are different to the ones of
         * the main memory bus (e.g. due to memory mapping or use of
         * different address spaces), this method is used to translate
         * from kernel physical addresses to host controller physical
         * addresses.
         *
         * @param aPhysicalAddressList On entering, the list of
         *   physical addresses as used kernel-side, on successful
         *   return it contains the list of physical addresses as
         *   referenced by the HC
         *
         * @param aPhysicalAddressListSize Number of elements in
         *   aPhysicalAddressList
         *
         * @return KErrNone if successful, otherwise a system-wide
         *   error code
         */
        virtual TInt MapPhysicalAddresses( TPhysAddr*& aPhysicalAddressList, 
                                           TInt        aPhysicalAddressListSize ) = 0;
        };


    /**
     * An interface class implemented by the USB stack to allow the
     * host controller to report events to the root hub. This includes
     * notifications of a device being connected or disconnected on a
     * port, and other port conditions such as seeing a remote wakeup
     * signalling, an error on the port (such as babble from a
     * peripheral), or VBUS over-current.
     *
     * It is required that the Host Controller PSL calls these
     * functions in the context of the DFC queue supplied by the Host
     * Controller PSL in
     * THostControllerProperties::iControllerDfcQueue when the Host
     * Controller PSL registered.
     */
    NONSHARABLE_CLASS( MRootHubCallbackIf )
        {
        public:
        /**
         * A constant to specify an unknown port. In those port
         * notifications where this is explicitly specified to be OK,
         * this value is allowed to be used as the port number where
         * the real port number is unknown.
         */
        static const TUint KPortUnknown = 0;

        /**
         * Informs the root hub that a device (a hub or function) has
         * been connected to the specified root hub port.
         *
         * The Host Controller PSL uses this function to inform device
         * connection also during HNP role switch when the remote
         * device connects in peripheral role.
         *
         * @param aPort The number of the root hub port that the event
         *   occurred on. For a root hub with N ports, the port numbers
         *   are in range [1..N].
         */
        virtual void DeviceConnected( TUint aPort ) = 0;

        /**
         * Informs the root hub that a device (a hub or function) has
         * been disconnected from the specified root hub port.
         *
         * The Host Controller PSL uses this function to inform device
         * disconnection also during HNP role switch when the remote
         * peripheral disconnects to start the role switch.
         *
         * The Host Controller PSL must report device disconnection by
         * calling this function even if the port is currently
         * suspended. A disconnection that is part of an HNP role
         * switch will always occur in suspended state, and even a
         * normal peripheral may be physically disconnected at any
         * time.
         *
         * @param aPort The number of the root hub port that the event
         *   occurred on. For a root hub with N ports, the port numbers
         *   are in range [1..N].
         */
        virtual void DeviceDisconnected( TUint aPort ) = 0;

        /**
         * Informs the root hub that a device connected to the
         * specified root hub port has requested remote wakeup.
         *
         * @param aPort The number of the root hub port that the event
         *   occurred on. For a root hub with N ports, the port numbers
         *   are in range [1..N].
         */
        virtual void RemoteWakeupRequested( TUint aPort ) = 0;

        /**
         * Informs the root hub that a port error (such as babble) has
         * been detected on the specified root hub port. The
         * adaptation is expected to disable the port that has caused
         * the error condition.
         *
         * @param aPort The number of the root hub port that the event
         *   occurred on. For a root hub with N ports, the port numbers
         *   are in range [1..N].
         */
        virtual void PortErrorDetected( TUint aPort ) = 0;

        /**
         * Informs the root hub that an over-current condition has
         * been detected on the specified port.
         *
         * It may not always be possible to see which port out of
         * several caused the overcurrent. In this case the Host
         * Controller PSL may use the port number
         * MRootHubCallbackIf::KPortUnknown.
         *
         * @param aPort The number of the root hub port that the event
         *   occurred on, if known. For a root hub with N ports, the
         *   port numbers are in range [1..N]. If the exact port is
         *   not known, the Host Controller PSL root hub may report
         *   the port number MRootHubCallbackIf::KPortUnknown.
         */
        virtual void OverCurrentDetected( TUint aPort ) = 0;
        };


    /**
     * An interface class implemented by the USB Host Stack to allow the
     * host controller to complete transfers to the USB Host Stack.
     *
     * It is required that the Host Controller PSL calls these
     * functions in the context of the DFC queue supplied by the Host
     * Controller PSL in THostControllerProperties::iControllerDfcQueue when
     * the Host Controller PSL registered.
     */
    NONSHARABLE_CLASS( MHostTransferCallbackIf )
        {
        public:
        /**
         * Complete a transfer request that had been set up with
         * MHostPipeIf::StartTransfer().
         *
         * To maximize throughput, the PIL layer may set up the next
         * transfer by calling MHostPipeIf::StartTransfer()
         * immediately from within this completion function. The PSL
         * has to be prepared for this and make sure it does not upon
         * return of the callback accidentally clean up some internal
         * state belonging to a new transfer started during the
         * callback.
         *
         * @param aTransferInfo The transfer information that
         *   identifies the transfer that has completed
         */
        virtual void CompleteTransfer( const THostTransfer& aTransferInfo ) = 0;
        };


    /**
     * This class specifies the static information provided by a Host
     * Controller PSL when registering to the USB Host stack.
     *
     * The PSL should prepare for the possibility that members may be
     * added to the end of this class in later SHAI versions if new
     * information is needed to support new features. The PSL should
     * not use this class as a direct member in an object that is not
     * allowed to grow in size due to binary compatibility reasons.
     *
     * @see UsbHostPil::RegisterHostController()
     */
    NONSHARABLE_CLASS( THostControllerProperties )
        {
        public: // Types and constants
        /**
         * A bitmask type used to indicate the static capabilities of
         * the Host Controller.
         */
        typedef TUint32 THostCaps;

        public:
        /**
         * Inline constructor for the Host Controller properties
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
        inline THostControllerProperties() :
            iCapabilities(0),
            iControllerDfcQueue(NULL),
            iControllerPageSize(0)
            {
            };

        public: // Data
        /**
         * A bitmask specifying the static capabilities of this Host
         * Controller. No capabilities are specified at the moment and
         * the PSL shall fill this field with a zero value.
         *
         * The field is added for sake of future proofing the binary
         * compatibility of the Host SHAI. By having a field reserved
         * for capability bits, we can later specify bits to indicate
         * added virtual functions or extension to this controller
         * properties structure. The PIL layer can then at runtime
         * confirm the existence of the new functions or fields and
         * safely support an older binary, if we choose to.
         */
        THostCaps iCapabilities;

        /**
         * Pointer to a DFC queue that will be used for DFCs of this
         * controller.
         *
         * A stand-alone (one not associated with an OTG-port) Host
         * Controller PSL must supply a pointer to a dedicated DFC
         * queue that has been created for this controller. Both the
         * PSL itself and the PIL layer must queue their DFCs for this
         * controller in this DFC queue to ensure the code is running
         * in the same context.
         *
         * A Host Controller PSL that is part of an OTG port will be
         * registered by the OTG PSL. In this case, the DFC queue
         * supplied here must be the same DFC queue as the one
         * supplied in the properties of the OTG Controller.
         */
        TDfcQue* iControllerDfcQueue;

        /**
         * The page size used by this Host Controller. The host stack
         * will fill scatter-gather lists with pages of this size. If
         * the Host Controller does not have specific page
         * requirements itself, the registering Host Controller PSL
         * must supply here the CPU MMU page size that can be obtained
         * by the call Kern::RoundToPageSize(1).
         */
        TInt iControllerPageSize;
        };


    /**
     * A static class implemented by the USB Host PIL layer to allow
     * the Host Controller PSL to register to the PIL layer.
     */
    NONSHARABLE_CLASS( UsbHostPil )
        {
        public:
        /**
         * Registration function to be used by a stand-alone (non-OTG
         * port) Host Controller PSL to register itself to the PIL
         * layer. Host Controllers that are part of an OTG-port are
         * registered by the OTG Controller PSL (see usb_otg_shai.h,
         * function UsbOtgPil::RegisterOtgController).
         *
         * The intended usage is that each stand-alone Host Controller
         * PSL is a kernel extension that registers itself to the USB
         * Host PIL layer by making this call from their own kernel
         * extension entry point function (or an equivalent code that
         * runs during bootup).
         *
         * @param aHostControllerIf Reference to the Host Controller
         *   interface implemented by the registering PSL. The PIL layer
         *   requires that the supplied reference remains valid
         *   indefinitely, as the Host Controller cannot unregister.
         *
         * @param aProperties Reference to an object describing the
         *   static properties of the Host Controller. The PIL takes a
         *   copy and the PSL is free to release the properties object
         *   upon return.
         *
         * @lib usbotghostpil.lib
         */
        IMPORT_C static void RegisterHostController(
            MHostControllerIf&               aHostControllerIf,
            const THostControllerProperties& aProperties );
        };
    };

#endif // USB_HOST_SHAI_H

/* End of File */
