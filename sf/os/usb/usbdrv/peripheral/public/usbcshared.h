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
 @file usbcshared.h
 @internalTechnology
*/

#ifndef __USBCSHARED_H__
#define __USBCSHARED_H__

//#include <drivers/usbcque.h>
#include <usb/usbcque.h>

// Define here what options are required:
// (USB_SUPPORTS_CONTROLENDPOINTS and USB_SUPPORTS_SET_DESCRIPTOR_REQUEST
//  have never been tested though...)
//#define USB_SUPPORTS_CONTROLENDPOINTS
//#define USB_SUPPORTS_SET_DESCRIPTOR_REQUEST

//#include <drivers/usbcdesc.h>
#include <usb/usbcdesc.h>
//#include <drivers/usbotgperipheralcontrolif.h>
#include <usb/usbotgperipheralcontrolif.h>

#include <usb/usb_peripheral_shai.h>
#include <usb/usb_charger_detection_shai.h>

#include <usb/usbcontrolxferif.h>

// FIXME: Remove this typedef once PIL and LDD are ready for SHAI
typedef UsbShai::TUsbPeripheralEvent TUsbcDeviceEvent;
typedef UsbShai::TUsbPeripheralPacketArray TUsbcPacketArray;

// Forward declaration
class DControlTransferManager;
class TUsbcSetup;

// Debug Support

// Use for debugging purposes only (commented out for normal operation):
//#define USBC_LDD_BUFFER_TRACE

static const char KUsbPILPanicCat[] = "USB PIL FAULT"; // kernel fault category
_LIT(KUsbPILKillCat, "USB PIL KILL");                    // thread kill category
_LIT(KUsbLDDKillCat, "USB LDD KILL");                    // thread kill category

/** Error code for stalled endpoint.
*/
const TInt KErrEndpointStall = KErrLocked;

/** Error code for Ep0 write prematurely ended by a host OUT token.
*/
const TInt KErrPrematureEnd = KErrDiskFull;

/** The following constants control the buffer arrangement for OUT transfers (IN transfers have only 1
    buffer). The total size of buffering for an OUT endpoint will be number of buffers * buffersize,
    so that, for example, a Bulk OUT endpoint will have KUsbcDmaBufNumBulk * KUsbcDmaBufSzBulk bytes of
    buffering.
    These buffers will be physically contiguous, so that DMA may be used.
    The number of buffers MUST be >=2 - otherwise the buffering scheme won't work.
    The buffer sizes should be an exact fraction of 4kB and the number of buffers such that the
    buffersize * number of buffers is an exact multiple of 4kB, otherwise memory will be wasted.
*/
/** Size of a Control ep buffer.
*/
const TInt KUsbcDmaBufSzControl = 1024;

/** Size of a Bulk ep buffer.
*/
const TInt KUsbcDmaBufSzBulk = 4096;

/** Size of an Interrupt ep buffer.
*/
const TInt KUsbcDmaBufSzInterrupt = 4096;

/** Size of an Isochronous ep buffer.
*/
const TInt KUsbcDmaBufSzIsochronous = 4096;

/** Number of buffers for Control OUT endpoints.
*/
const TInt KUsbcDmaBufNumControl = 2;

/** Number of buffers for Isochronous OUT endpoints.
*/
const TInt KUsbcDmaBufNumIsochronous = 2;

/** Number of buffers for Bulk OUT endpoints.
*/
const TInt KUsbcDmaBufNumBulk = 2;

/** Number of buffers for Interrupt OUT endpoints.
*/
const TInt KUsbcDmaBufNumInterrupt = 2;

/** Maximum buffer number.
*/
const TInt KUsbcDmaBufNumMax = MAX4(KUsbcDmaBufNumControl, KUsbcDmaBufNumIsochronous,
                                    KUsbcDmaBufNumBulk, KUsbcDmaBufNumInterrupt);

/** Maximum number of recorded packets possible.
*/
const TUint KUsbcDmaBufMaxPkts = 2;

/** Number of arrays.
*/
const TInt KUsbcDmaBufNumArrays = 2;

/** Max size that Ep0 packets might have.
*/
const TInt KUsbcBufSzControl = 64;

/** The Ep0 RX data collection buffer area.
    (Arbitrary size, judged to be sufficient for SET_DESCRIPTOR requests)
*/
const TInt KUsbcBufSz_Ep0Rx = 1024;

/** The Ep0 TX buffer area.
    (Size sufficient to hold as much data as can be requested via GET_DESCRIPTOR)
*/
const TInt KUsbcBufSz_Ep0Tx = 1024 * 64; 


/** The USB version the stack is compliant with: 2.0 (BCD).
*/
const TUint16 KUsbcUsbVersion = 0x0200;

/** Maximum number of endpoints an interface (i.e. LDD) may have.
*/
const TInt KUsbcMaxEpNumber = 5;

/** Status FIFO depth; enough for 2 complete configs.
*/
const TInt KUsbDeviceStatusQueueDepth = 15;

/** = 'no status info'.
*/
const TUint32 KUsbDeviceStatusNull = 0xffffffffu;

/** = 'no buffer available'.
*/
const TInt KUsbcInvalidBufferIndex = -1;

/** = 'no packet available'.
*/
const TUint KUsbcInvalidPacketIndex = (TUint)(-1);

/** = 'no drainable buffers'.
*/
const TInt KUsbcInvalidDrainQueueIndex = -1;

/** Number of possible bandwidth priorities.
*/
const TInt KUsbcDmaBufMaxPriorities = 4;

// The following buffer sizes are used within the LDD for the different
// user-selectable endpoint bandwidth priorities
// (EUsbcBandwidthOUTDefault/Plus1/Plus2/Maximum + the same for 'IN').
// These values, in particular those for the Maximum setting, were obtained
// empirically.

/** Bulk IN buffer sizes for different priorities (4K, 16K, 64K, 512K).
*/
const TInt KUsbcDmaBufSizesBulkIN[KUsbcDmaBufMaxPriorities] =
    {KUsbcDmaBufSzBulk, 0x4000, 0x10000, 0x80000};

/** Bulk OUT buffer sizes for different priorities (4K, 16K, 64K, 512K).
*/
const TInt KUsbcDmaBufSizesBulkOUT[KUsbcDmaBufMaxPriorities] =
    {KUsbcDmaBufSzBulk, 0x4000, 0x10000, 0x80000};

/** Number of UDCs supported in the system.
    (Support for more than one UDC is preliminary.)
*/
const TInt KUsbcMaxUdcs = 2;

/** Number of endpoints a USB device can have.
    (30 regular endpoints + 2 x Ep0)
*/
const TInt KUsbcEpArraySize = KUsbcMaxEndpoints + 2;

/** Number of notification requests of the same kind that can be registered at
    a time. As normally not more than one request per kind per LDD is
    permitted, this number is roughly equivalent to the maximum number of LDDs
    that can be operating at the same time.
    This constant is used by the PIL while maintaining its request lists
    (iClientCallbacks, iStatusCallbacks, iEpStatusCallbacks, iOtgCallbacks) to
    ensure that the lists are of a finite length and thus the list traverse
    time is bounded.
    This value is chosen with the maximum number of USB interfaces (not
    settings) allowed by the spec for a single device in mind.
*/
const TInt KUsbcMaxListLength = 256;

/**
 * device event FIFO depth, there are 5 different event define currently,
 * 10 should be enough
 */
const TInt KUsbDeviceEventQueueDepth = 10;

/** USB LDD client callback.
*/
class TUsbcClientCallback
    {
public:
    inline TUsbcClientCallback(DBase* aOwner, TDfcFn aCallback, TInt aPriority);
    inline DBase* Owner() const;
    inline TInt DoCallback();
    inline void Cancel();
    inline void SetDfcQ(TDfcQue* aDfcQ);
public:
    /** Used by the PIL to queue callback objects into a TSglQue. */
    TSglQueLink iLink;
private:
    DBase* iOwner;
    TDfc iDfc;
    };


/** The endpoint halt/clear_halt status.
*/
class TUsbcEndpointStatusCallback
    {
public:
    inline TUsbcEndpointStatusCallback(DBase* aOwner, TDfcFn aCallback, TInt aPriority);
    inline void SetState(TUint aState);
    inline TUint State() const;
    inline DBase* Owner() const;
    inline TInt DoCallback();
    inline void Cancel();
    inline void SetDfcQ(TDfcQue* aDfcQ);
public:
    /** Used by the PIL to queue callback objects into a TSglQue. */
    TSglQueLink iLink;
private:
    DBase* iOwner;
    TDfc iDfc;
    TUint iState;
    };


/** Maximum number of device status requests that can be queued at a time.
    The value chosen is thought to be sufficient in all situations.
*/
const TInt KUsbcDeviceStateRequests = 8;

        
/** The USB device status.
*/
class TUsbcStatusCallback
    {
public:
    inline TUsbcStatusCallback(DBase* aOwner, TDfcFn aCallback, TInt aPriority);
    inline void SetState(UsbShai::TUsbPeripheralState aState);
    inline UsbShai::TUsbPeripheralState State(TInt aIndex) const;
    inline void ResetState();
    inline DBase* Owner() const;
    inline TInt DoCallback();
    inline void Cancel();
    inline void SetDfcQ(TDfcQue* aDfcQ);
public:
    /** Used by the PIL to queue callback objects into a TSglQue. */
    TSglQueLink iLink;
private:
    DBase* iOwner;
    TDfc iDfc;
    UsbShai::TUsbPeripheralState iState[KUsbcDeviceStateRequests];
    };


/** A USB transfer request.

    @publishedPartner
    @released
*/
class TUsbcRequestCallback : public UsbShai::TUsbPeripheralRequest
    {
public:
    /** @internalTechnology */
    inline TUsbcRequestCallback(const DBase* aOwner, TInt aEndpointNum, TDfcFn aDfcFunc,
                         TAny* aEndpoint, TDfcQue* aDfcQ, TInt aPriority);
    /** @internalTechnology    */
    inline ~TUsbcRequestCallback();
    
    IMPORT_C void SetRxBufferInfo(TUint8* aBufferStart,
                                      TUintPtr aBufferAddr,
                                      TUint32* aPacketIndex,
                                      TUint32* aPacketSize, 
                                      TInt aLength);
                                    
    IMPORT_C void SetTxBufferInfo(TUint8* aBufferStart, 
                                      TUintPtr aBufferAddr,
                                      TInt aLength);
                                        
    /** @internalTechnology    */
    inline void SetTransferDirection(UsbShai::TTransferDirection aTransferDir);
    /** @internalTechnology    */
    inline const DBase* Owner() const;
    /** @internalTechnology    */
    inline TInt DoCallback();
    /** @internalTechnology    */
    inline void Cancel();
public:
    /** Used by the PIL to queue callback objects into a TSglQue.
        @internalTechnology
    */
    TSglQueLink iLink;
public:
    /** The endpoint number. */
    const TInt iEndpointNum;

    /** Indicates the LDD client for this transfer. */
    const DBase* const iOwner;
    /** DFC, used by PIL to call back the LDD when transfer completes to the LDD. */
    TDfc iDfc;
    };

/** USB On-The-Go feature change callback.
*/
class TUsbcOtgFeatureCallback
    {
public:
    inline TUsbcOtgFeatureCallback(DBase* aOwner, TDfcFn aCallback, TInt aPriority);
    inline void SetFeatures(TUint8 aFeatures);
    inline TUint8 Features() const;
    inline DBase* Owner() const;
    inline TInt DoCallback();
    inline void Cancel();
    inline void SetDfcQ(TDfcQue* aDfcQ);
public:
    /** Used by the PIL to queue callback objects into a TSglQue. */
    TSglQueLink iLink;
private:
    DBase* iOwner;
    TDfc iDfc;
    TUint8 iValue;
    };
/** USB Charger Type change callback.
*/
class TUsbcChargerTypeCallback
    {
public:
    inline TUsbcChargerTypeCallback (DBase* aOwner, TDfcFn aCallback, TInt aPriority);
    inline void SetChargerType(TUint aType);
    inline TUint ChargerType () const;
	inline void SetPendingNotify(TBool aPendingNotify);
	inline TBool PendingNotify () const;
    inline DBase* Owner() const;
    inline TInt DoCallback();
    inline void Cancel();
    inline void SetDfcQ(TDfcQue* aDfcQ);
public:
    /** Used by the PIL to queue callback objects into a TSglQue. */
    TSglQueLink iLink;
private:
    DBase* iOwner;
    TDfc iDfc;
    TUint iChargerType;
	TBool iPendingNotify;
    };

//
//########################### Physical Device Driver (PIL + PSL) ######################
//

class TUsbcLogicalEndpoint;

/** This models a physical (real) endpoint of the UDC.
*/
class TUsbcPhysicalEndpoint
    {
public:
    TUsbcPhysicalEndpoint();
    ~TUsbcPhysicalEndpoint();
    TBool EndpointSuitable(const TUsbcEndpointInfo* aEpInfo, TInt aIfcNumber) const; // Check Todo, SC will pass pointer to derived class
    TInt TypeAvailable(TUint aType) const;
    TInt DirAvailable(TUint aDir) const;
public:
    /** This endpoint's capabilities. */
    UsbShai::TUsbPeripheralEndpointCaps iCaps;
    /** USB address: 0x00, 0x80, 0x01, 0x81, etc. */
    TUint8 iEndpointAddr;
    /** Pointer to interface # this endpoint has been assigned to. */
    const TUint8* iIfcNumber;
    /** Pointer to corresponding logical endpoint or NULL. */
    const TUsbcLogicalEndpoint* iLEndpoint;
    /** Only used when searching for available endpoints. */
    TBool iSettingReserve;
    /** True if endpoint is halted (i.e. issues STALL handshakes), false otherwise. */
    TBool iHalt;
    };


class DUsbClientController;
class TUsbcInterface;

/** This is a 'logical' endpoint, as used by our device configuration model.
*/
class TUsbcLogicalEndpoint
    {
public:
    TUsbcLogicalEndpoint(DUsbClientController* aController, TUint aEndpointNum,
                         const TUsbcEndpointInfo& aEpInfo, TUsbcInterface* aInterface,
                         TUsbcPhysicalEndpoint* aPEndpoint);        // Check Todo, SC will pass pointer to derived class
    ~TUsbcLogicalEndpoint();
public:
    /** Pointer to controller object. */
    DUsbClientController* iController;
    /** The virtual (logical) endpoint number. */
    const TInt iLEndpointNum;
    /** This endpoint's info structure. */
    TUsbcEndpointInfo iInfo;                                        // Check Todo, SC will pass pointer to derived class
    /** Stores the endpoint size to be used for FS. */
    TInt iEpSize_Fs;
    /** Stores the endpoint size to be used for HS. */
    TInt iEpSize_Hs;
    /** 'Back' pointer. */
    const TUsbcInterface* iInterface;
    /** Pointer to corresponding physical endpoint, never NULL. */
    TUsbcPhysicalEndpoint* const iPEndpoint;
    };


class TUsbcInterfaceSet;

/** This is an 'alternate setting' of an interface.
*/
class TUsbcInterface
    {
public:
    TUsbcInterface(TUsbcInterfaceSet* aIfcSet, TUint8 aSetting, TBool aNoEp0Requests);
    ~TUsbcInterface();
public:
    /** Array of endpoints making up (belonging to) this setting. */
    RPointerArray<TUsbcLogicalEndpoint> iEndpoints;
    /** 'Back' pointer. */
    TUsbcInterfaceSet* const iInterfaceSet;
    /** bAlternateSetting (zero-based). */
    const TUint8 iSettingCode;
    /** KUsbcInterfaceInfo_NoEp0RequestsPlease: stall non-std Setup requests. */
    const TBool iNoEp0Requests;
    };


/** This is an 'interface' (owning 1 or more alternate settings).

    @see TUsbcInterface
*/
class TUsbcInterfaceSet
    {
public:
    TUsbcInterfaceSet(const DBase* aClientId, TUint8 aIfcNum);
    ~TUsbcInterfaceSet();
    inline const TUsbcInterface* CurrentInterface() const;
    inline TUsbcInterface* CurrentInterface();
public:
    /** Array of alternate settings provided by (belonging to) this interface. */
    RPointerArray<TUsbcInterface> iInterfaces;
    /** Pointer to the LDD which created and owns this interface. */
    const DBase* const iClientId;
    /** bInterfaceNumber (zero-based). */
    TUint8 iInterfaceNumber;
    /** bAlternateSetting (zero-based). */
    TUint8 iCurrentInterface;
    };


/** This is a 'configuration' of the USB device.
    Currently we support only one configuration.
*/
class TUsbcConfiguration
    {
public:
    TUsbcConfiguration(TUint8 aConfigVal);
    ~TUsbcConfiguration();
public:
    /** Array of interfaces making up (belonging to) this configuration. */
    RPointerArray<TUsbcInterfaceSet> iInterfaceSets;
    /** bConfigurationValue (one-based). */
    const TUint8 iConfigValue;
    };


/** The USB controller's power handler class.
*/
NONSHARABLE_CLASS(DUsbcPowerHandler) : public DPowerHandler
    {
public:
    void PowerUp();
    void PowerDown(TPowerState);
public:
    DUsbcPowerHandler(DUsbClientController* aController);
private:
    DUsbClientController* iController;
    };


/*
This is the EndpointInfo class used by the usb shared chunk client driver. 
*/

class TUsbcScEndpointInfo;


/**
Used to represent an array of (or inheriting from) TUsbcEndpointInfo objects.

@see DUsbClientController::SetInterface
*/

class TUsbcEndpointInfoArray
    {
public:
    typedef enum {EUsbcEndpointInfo, EUsbcScEndpointInfo} TArrayType;

    TUsbcEndpointInfoArray(const TUsbcEndpointInfo* aData, TInt aDataSize=0);
    TUsbcEndpointInfoArray(const TUsbcScEndpointInfo* aData, TInt aDataSize=0);
    inline TUsbcEndpointInfo& operator[](TInt aIndex) const; 

    TArrayType iType;
private:
    TUint8* iData;
    TInt iDataSize;
    };

class TUsbPeriDeviceEventQueue
    {
public:
    TUsbPeriDeviceEventQueue();
    
    void FifoAdd(TUsbcDeviceEvent aDeviceStatus);
    TUsbcDeviceEvent FifoGet();

private:
    TUsbcDeviceEvent iDeviceEventQueue[KUsbDeviceEventQueueDepth];
    TInt iDeviceQueueHead;
    TInt iDeviceQueueTail;
    };
    
class TUsbcRequestCallback; // todo?? required only for class below

/** The USB Device software controller class.

    Implements the platform-independent layer (PIL), and defines the interface to the
    platform-specific layer PSL).

    The implementation of the platform-specific layer interfaces with the hardware.
*/
NONSHARABLE_CLASS(DUsbClientController) : public DBase, 
                                          public MUsbOtgPeripheralControlIf,
                                          public UsbShai::MUsbPeripheralPilCallbackIf, 
                                          public MControlTransferIf, 
                                          public UsbShai::MChargerDetectorObserverIf
    {   
    friend class UsbShai::UsbPeripheralPil;
    friend class TUsbHostWrapperEntry;
    friend class DUsbcPowerHandler;
    friend TUsbcLogicalEndpoint::~TUsbcLogicalEndpoint();
    //
    // --- Platform Independent Layer (PIL) ---
    //


private:
    /**
     * Two-phase constructor used by the friend classes that receive
     * the registration calls for peripheral controllers.
     *
     * @param aPeripheralControllerIf Reference to the Peripheral
     *   Controller interface implemented by the registering PSL.
     *
     * @param aProperties Reference to an object describing the
     *   static properties of the Peripheral Controller. The PIL
     *   layer requires that the supplied reference remains valid
     *   indefinitely, as a Peripheral Controller cannot unregister.
     *
     * @param aIsOtgPort ETrue if this Peripheral Controller is part
     *   of an OTG port. EFalse otherwise.
     *
     * @return Pointer to a new instance or NULL if failed
     */
    IMPORT_C static DUsbClientController* Create(UsbShai::MPeripheralControllerIf& aPeripheralControllerIf, 
                                                 const UsbShai::TPeripheralControllerProperties& aProperties,
                                                 TBool aIsOtgPort);
    

public:

    //
    // --- The following functions constitute the PIL interface to the LDD ---
    //
    virtual ~DUsbClientController();
    IMPORT_C void DisableClientStack();
    IMPORT_C void EnableClientStack();
    IMPORT_C TBool IsActive();
    IMPORT_C TInt RegisterClientCallback(TUsbcClientCallback& aCallback);
    IMPORT_C static DUsbClientController* UsbcControllerPointer(TInt aUdc=0);
    IMPORT_C void EndpointCaps(const DBase* aClientId, TDes8 &aCapsBuf) const;
    IMPORT_C void DeviceCaps(const DBase* aClientId, TDes8 &aCapsBuf) const;
    IMPORT_C TInt SetInterface(const DBase* aClientId, DThread* aThread, TInt aInterfaceNum,
                               TUsbcClassInfo& aClass, TDesC8* aString, TInt aTotalEndpointsUsed,
                               const TUsbcEndpointInfo aEndpointData[], TInt (*aRealEpNumbers)[6],
                               TUint32 aFeatureWord);
    IMPORT_C TInt SetInterface(const DBase* aClientId, DThread* aThread,
                                                 TInt aInterfaceNum, TUsbcClassInfo& aClass,
                                                 TDesC8* aString, TInt aTotalEndpointsUsed,
                                                 const TUsbcEndpointInfoArray aEndpointData,
                                                 TInt aRealEpNumbers[], TUint32 aFeatureWord);
    IMPORT_C TInt ReleaseInterface(const DBase* aClientId, TInt aInterfaceNum);
    IMPORT_C TInt ReEnumerate();
    IMPORT_C TInt PowerUpUdc();
    IMPORT_C TInt UsbConnect();
    IMPORT_C TInt UsbDisconnect();
    IMPORT_C TInt RegisterForStatusChange(TUsbcStatusCallback& aCallback);
    IMPORT_C TInt DeRegisterForStatusChange(const DBase* aClientId);
    IMPORT_C TInt RegisterForEndpointStatusChange(TUsbcEndpointStatusCallback& aCallback);
    IMPORT_C TInt DeRegisterForEndpointStatusChange(const DBase* aClientId);
    IMPORT_C TInt GetInterfaceNumber(const DBase* aClientId, TInt& aInterfaceNum) const;
    IMPORT_C TInt DeRegisterClient(const DBase* aClientId);
    IMPORT_C TInt Ep0PacketSize() const;
    IMPORT_C TInt Ep0Stall(const DBase* aClientId);
    IMPORT_C void SendEp0StatusPacket(const DBase* aClientId);
    IMPORT_C UsbShai::TUsbPeripheralState GetDeviceStatus() const;
    IMPORT_C TEndpointState GetEndpointStatus(const DBase* aClientId, TInt aEndpointNum) const;
    IMPORT_C TInt SetupReadBuffer(TUsbcRequestCallback& aCallback);
    IMPORT_C TInt SetupWriteBuffer(TUsbcRequestCallback& aCallback);
    IMPORT_C void CancelReadBuffer(const DBase* aClientId, TInt aRealEndpoint);
    IMPORT_C void CancelWriteBuffer(const DBase* aClientId, TInt aRealEndpoint);
    IMPORT_C TInt HaltEndpoint(const DBase* aClientId, TInt aEndpointNum);
    IMPORT_C TInt ClearHaltEndpoint(const DBase* aClientId, TInt aEndpointNum);
    IMPORT_C TInt SetDeviceControl(const DBase* aClientId);
    IMPORT_C TInt ReleaseDeviceControl(const DBase* aClientId);
    IMPORT_C TUint EndpointZeroMaxPacketSizes() const;
    IMPORT_C TInt SetEndpointZeroMaxPacketSize(TInt aMaxPacketSize);
    IMPORT_C TInt GetDeviceDescriptor(DThread* aThread, TDes8& aDeviceDescriptor);
    IMPORT_C TInt SetDeviceDescriptor(DThread* aThread, const TDes8& aDeviceDescriptor);
    IMPORT_C TInt GetDeviceDescriptorSize(DThread* aThread, TDes8& aSize);
    IMPORT_C TInt GetConfigurationDescriptor(DThread* aThread, TDes8& aConfigurationDescriptor);
    IMPORT_C TInt SetConfigurationDescriptor(DThread* aThread, const TDes8& aConfigurationDescriptor);
    IMPORT_C TInt GetConfigurationDescriptorSize(DThread* aThread, TDes8& aSize);
    IMPORT_C TInt SetOtgDescriptor(DThread* aThread, const TDesC8& aOtgDesc);
    IMPORT_C TInt GetOtgDescriptor(DThread* aThread, TDes8& aOtgDesc) const;
    IMPORT_C TInt GetOtgFeatures(DThread* aThread, TDes8& aFeatures) const;
    IMPORT_C TInt GetCurrentOtgFeatures(TUint8& aFeatures) const;
    IMPORT_C TInt RegisterForOtgFeatureChange(TUsbcOtgFeatureCallback& aCallback);
    IMPORT_C TInt DeRegisterForOtgFeatureChange(const DBase* aClientId);
    IMPORT_C TInt GetInterfaceDescriptor(DThread* aThread, const DBase* aClientId, TInt aSettingNum,
                                         TDes8& aInterfaceDescriptor);
    IMPORT_C TInt SetInterfaceDescriptor(DThread* aThread, const DBase* aClientId, TInt aSettingNum,
                                         const TDes8& aInterfaceDescriptor);
    IMPORT_C TInt GetInterfaceDescriptorSize(DThread* aThread, const DBase* aClientId, TInt aSettingNum,
                                             TDes8& aSize);
    IMPORT_C TInt GetEndpointDescriptor(DThread* aThread, const DBase* aClientId, TInt aSettingNum,
                                        TInt aEndpointNum, TDes8& aEndpointDescriptor);
    IMPORT_C TInt SetEndpointDescriptor(DThread* aThread, const DBase* aClientId, TInt aSettingNum,
                                        TInt aEndpointNum, const TDes8& aEndpointDescriptor);
    IMPORT_C TInt GetEndpointDescriptorSize(DThread* aThread, const DBase* aClientId, TInt aSettingNum,
                                            TInt aEndpointNum, TDes8& aSize);
    IMPORT_C TInt GetDeviceQualifierDescriptor(DThread* aThread, TDes8& aDeviceQualifierDescriptor);
    IMPORT_C TInt SetDeviceQualifierDescriptor(DThread* aThread, const TDes8& aDeviceQualifierDescriptor);
    IMPORT_C TInt GetOtherSpeedConfigurationDescriptor(DThread* aThread, TDes8& aConfigurationDescriptor);
    IMPORT_C TInt SetOtherSpeedConfigurationDescriptor(DThread* aThread, const TDes8& aConfigurationDescriptor);
    IMPORT_C TInt GetCSInterfaceDescriptorBlock(DThread* aThread, const DBase* aClientId, TInt aSettingNum,
                                                TDes8& aInterfaceDescriptor);
    IMPORT_C TInt SetCSInterfaceDescriptorBlock(DThread* aThread, const DBase* aClientId, TInt aSettingNum,
                                                const TDes8& aInterfaceDescriptor, TInt aSize);
    IMPORT_C TInt GetCSInterfaceDescriptorBlockSize(DThread* aThread, const DBase* aClientId,
                                                    TInt aSettingNum, TDes8& aSize);
    IMPORT_C TInt GetCSEndpointDescriptorBlock(DThread* aThread, const DBase* aClientId, TInt aSettingNum,
                                               TInt aEndpointNum, TDes8& aEndpointDescriptor);
    IMPORT_C TInt SetCSEndpointDescriptorBlock(DThread* aThread, const DBase* aClientId, TInt aSettingNum,
                                               TInt aEndpointNum, const TDes8& aEndpointDescriptor,
                                               TInt aSize);
    IMPORT_C TInt GetCSEndpointDescriptorBlockSize(DThread* aThread, const DBase* aClientId, TInt aSettingNum,
                                                   TInt aEndpointNum, TDes8& aSize);
    IMPORT_C TInt GetStringDescriptorLangId(DThread* aThread, TDes8& aLangId);
    IMPORT_C TInt SetStringDescriptorLangId(TUint16 aLangId);
    IMPORT_C TInt GetManufacturerStringDescriptor(DThread* aThread, TDes8& aString);
    IMPORT_C TInt SetManufacturerStringDescriptor(DThread* aThread, const TDes8& aString);
    IMPORT_C TInt RemoveManufacturerStringDescriptor();
    IMPORT_C TInt GetProductStringDescriptor(DThread* aThread, TDes8& aString);
    IMPORT_C TInt SetProductStringDescriptor(DThread* aThread, const TDes8& aString);
    IMPORT_C TInt RemoveProductStringDescriptor();
    IMPORT_C TInt GetSerialNumberStringDescriptor(DThread* aThread, TDes8& aString);
    IMPORT_C TInt SetSerialNumberStringDescriptor(DThread* aThread, const TDes8& aString);
    IMPORT_C TInt RemoveSerialNumberStringDescriptor();
    IMPORT_C TInt GetConfigurationStringDescriptor(DThread* aThread, TDes8& aString);
    IMPORT_C TInt SetConfigurationStringDescriptor(DThread* aThread, const TDes8& aString);
    IMPORT_C TInt RemoveConfigurationStringDescriptor();
    IMPORT_C TInt GetStringDescriptor(DThread* aThread, TUint8 aIndex, TDes8& aString);
    IMPORT_C TInt SetStringDescriptor(DThread* aThread, TUint8 aIndex, const TDes8& aString);
    IMPORT_C TInt RemoveStringDescriptor(TUint8 aIndex);
    IMPORT_C TBool QueryEndpointResource(const DBase* aClientId, TInt aEndpointNum,
                                         TUsbcEndpointResource aResource);
    IMPORT_C TInt EndpointPacketSize(const DBase* aClientId, TInt aEndpointNum);
    IMPORT_C TInt RegisterChargingPortTypeNotify(TUsbcChargerTypeCallback& aCallback);
    IMPORT_C TInt DeRegisterChargingPortTypeNotify(const DBase* aClientId);
	IMPORT_C void ChargerDetectorCaps(UsbShai::TChargerDetectorProperties& aProperties);
    
    // Called by LDD
    IMPORT_C TDfcQue*  DfcQ(TInt aIndex);
    IMPORT_C void DumpRegisters();
    IMPORT_C TInt SignalRemoteWakeup();
    IMPORT_C TBool CurrentlyUsingHighSpeed();
    
    TInt RegisterUdc(TInt aUdc);

protected:

    // From base class MUsbPeripheralPilCallbackIf
    void EnablePeripheralStack();
    void DisablePeripheralStack();
    void SetOtgObserver(MUsbOtgPeripheralObserverIf* aObserver);

    // Function derived from MUsbPeripheralPilCallbackIf
    UsbShai::TControlStage EnquireEp0NextStage(const TUint8* aSetupBuf) const;
    TInt Ep0RequestComplete(TInt aRealEndpoint, TInt aCount, TInt aError,UsbShai::TControlPacketType aPktType);
    void EndpointRequestComplete(UsbShai::TUsbPeripheralRequest* aCallback);
    TInt DeviceEventNotification(TUsbcDeviceEvent aEvent);
    void MoveToAddressState();
    TInt ProcessSetConfiguration(const TUsbcSetup& aPacket);
    void HandleHnpRequest(TInt aHnpState);
    void GetEp0RxBufferInfo(TUint8*& aBuffer, TInt& aBufferLen); 
    void SetRmWakeupStatusEnabled(TBool aRmEnabled);
    UsbShai::TUsbPeripheralState DeviceStatus() const;
    TBool Ep0ReceivedNonStdRequest();
    
    // Functions derived from MControlTransferIf
    // PSL -> PIL
    void ProcessDataInPacket(TInt aCount,TInt aErrCode);
    void ProcessStatusInPacket(TInt aErrCode);
    void ProcessDataOutPacket(TInt aCount,TInt aErrCode);
    void ProcessStatusOutPacket(TInt aErrCode);
    void ProcessSetupPacket(TInt aCount,TInt aErrCode);
    
    // Request from PIL -> PSL
    TInt ProcessSetupEndpointZeroRead();
    TInt ProcessSetupEndpointZeroWrite(const TUint8* aBuffer, TInt aLength, TBool aZlpReqd=EFalse);
    TInt ProcessSendEp0ZeroByteStatusPacket();
    TInt ProcessStallEndpoint(TInt aRealEndpoint);
    
    // Flow control PIL <--> PSL
    void ProcessEp0SetupPacketProceed();
    void ProcessEp0DataPacketProceed();
    
    // Functions from MChargerDetectorObserverIf
    void NotifyPortType(UsbShai::TPortType aPortType);
    //
    //
    // --- Functions and data members provided by PIL, called by PSL ---
    //
private:
    struct TUsbPeripheralDescriptorPool
        {
        TUsbcDeviceDescriptor* iDeviceDesc;
        TUsbcConfigDescriptor* iConfigDesc;
        TUsbcLangIdDescriptor* iLangId;
        TUsbcStringDescriptor* iManufacturer;
        TUsbcStringDescriptor* iProduct;
        TUsbcStringDescriptor* iSerialNum;
        TUsbcStringDescriptor* iConfig;
        TUsbcOtgDescriptor* iOtgDesc;
        };
        
private:
    // Internal functions that used in Construct only
    // This function will new a set of descriptors
    TBool CreateDescriptors(TUsbPeripheralDescriptorPool& aOutput);
    
    TBool Initialise(TUsbPeripheralDescriptorPool& aDescPool,
                     const UsbShai::TUsbPeripheralEndpointCaps* aEndpointCaps,
                     TInt aTotalEndpoint);                    

    DUsbClientController(UsbShai::MPeripheralControllerIf&               aPeripheralControllerIf, 
                         const UsbShai::TPeripheralControllerProperties& aProperties,
                         TBool                                  aIsOtgPort);
    
    TInt Construct();
    
    void ProcessStandardRequest(TInt aCount,const TUsbcSetup& aPacket);    
    void ProcessNonStandardRequest(TInt aCount,const TUsbcSetup& aPacket);
    
    const DBase* FindNonStandardRequestClient(TUint8 aPacketTypeDestination,const TUsbcSetup& aPacket);
    
    TInt SetupEp0StateMachine();
    
private:
    
    /** This info can be used by the PSL before sending ZLPs.

        @publishedPartner @released
    */
    TBool iEp0ReceivedNonStdRequest;

    /** True if RMW is currently enabled (set by either PIL or PSL).

        @publishedPartner @released
    */
    TBool iRmWakeupStatus_Enabled;

    /** Ep0 incoming (rx) data is placed here (one packet).

        @publishedPartner @released
    */
    TUint8 iEp0_RxBuf[KUsbcBufSzControl];

private:

    //
    // --- Private member functions (used by PIL) ---
    //
    TInt DeRegisterClientCallback(const DBase* aClientId);
    TBool CheckEpAvailability(TInt aEndpointsUsed, const TUsbcEndpointInfoArray& aEndpointData, TInt aIfcNumber)
        const;
    TUsbcInterface* CreateInterface(const DBase* aClientId, TInt aIfc, TUint32 aFeatureWord);
    TInt CreateEndpoints(TUsbcInterface* aIfc, TInt aEndpointsUsed, const TUsbcEndpointInfoArray& aEndpointData,
                         TInt *aRealEpNumbers);
    TInt SetupIfcDescriptor(TUsbcInterface* aIfc, TUsbcClassInfo& aClass, DThread* aThread, TDesC8* aString,
                            const TUsbcEndpointInfoArray& aEndpointData);
    TInt ClientId2InterfaceNumber(const DBase* aClientId) const;
    TUsbcInterfaceSet* ClientId2InterfacePointer(const DBase* aClientId) const;
    const DBase* InterfaceNumber2ClientId(TInt aIfcSet) const;
    TUsbcInterfaceSet* InterfaceNumber2InterfacePointer(TInt aIfcSet) const;
    inline const DBase* PEndpoint2ClientId(TInt aRealEndpoint) const;
    inline TInt PEndpoint2LEndpoint(TInt aRealEndpoint) const;
    TInt ActivateHardwareController();
    void DeActivateHardwareController();
    void DeleteInterfaceSet(TInt aIfcSet);
    void DeleteInterface(TInt aIfcSet, TInt aIfc);
    void CancelTransferRequests(TInt aRealEndpoint);
    void DeleteRequestCallback(const DBase* aClientId, TInt aEndpointNum, UsbShai::TTransferDirection aTransferDir);
    void DeleteRequestCallbacks(const DBase* aClientId);
    void StatusNotify(UsbShai::TUsbPeripheralState aState, const DBase* aClientId=NULL);
    void EpStatusNotify(TInt aRealEndpoint);
    void OtgFeaturesNotify();
    void RunClientCallbacks();
    void ProcessDataTransferDone(TUsbcRequestCallback& aRcb);
    void NextDeviceState(UsbShai::TUsbPeripheralState aNextState);
    TInt ProcessSuspendEvent();
    TInt ProcessSuspendEventProceed();
    TInt ProcessResumeEvent();
    TInt ProcessResetEvent(TBool aPslUpcall=ETrue);
    TInt ProcessVbusRisenEvent();
    TInt ProcessVbusFallenEvent();
    
    void ProcessGetDeviceStatus(const TUsbcSetup& aPacket);
    void ProcessGetInterfaceStatus(const TUsbcSetup& aPacket);
    void ProcessGetEndpointStatus(const TUsbcSetup& aPacket);
    void ProcessSetClearDevFeature(const TUsbcSetup& aPacket);
    void ProcessSetClearIfcFeature(const TUsbcSetup& aPacket);
    void ProcessSetClearEpFeature(const TUsbcSetup& aPacket);
    void ProcessSetAddress(const TUsbcSetup& aPacket);
    void ProcessGetDescriptor(const TUsbcSetup& aPacket);
    void ProcessSetDescriptor(const TUsbcSetup& aPacket);
    void ProcessGetConfiguration(const TUsbcSetup& aPacket);
    void ProcessGetInterface(const TUsbcSetup& aPacket);
    void ProcessSetInterface(const TUsbcSetup& aPacket);
    void ProcessSynchFrame(const TUsbcSetup& aPacket);
    
    void ProceedSetDescriptor();
    void ProcessDeviceEventNotification(TUsbcDeviceEvent aEvent);
    
    void SetClearHaltFeature(TInt aRealEndpoint, TUint8 aRequest);
    TInt ClearHaltFeature(TInt aRealEndpoint);
    void ChangeConfiguration(TUint16 aValue);
    void InterfaceSetup(TUsbcInterface* aIfc);
    void InterfaceSetTeardown(TUsbcInterfaceSet* aIfc);
    void ChangeInterface(TUsbcInterface* aIfc);
    TInt DoForEveryEndpointInUse(TInt (DUsbClientController::*aFunction)(TInt), TInt& aCount);
    void EnterFullSpeed();
    void EnterHighSpeed();
    TInt EvaluateOtgConnectFlags();
    inline const TUsbcConfiguration* CurrentConfig() const;
    inline TUsbcConfiguration* CurrentConfig();
    inline TBool InterfaceExists(TInt aNumber) const;
    inline TBool EndpointExists(TUint aAddress) const;
    void Buffer2Setup(const TAny* aBuf, TUsbcSetup& aSetup) const;
    inline TUint EpIdx2Addr(TUint aRealEndpoint) const;
    inline TUint EpAddr2Idx(TUint aAddress) const;
    inline void SetEp0DataOutVars(const DBase* aClientId = NULL);
    inline void ResetEp0DataOutVars();
    inline TBool IsInTheStatusList(const TUsbcStatusCallback& aCallback);
    inline TBool IsInTheEpStatusList(const TUsbcEndpointStatusCallback& aCallback);
    inline TBool IsInTheOtgFeatureList(const TUsbcOtgFeatureCallback& aCallback);
    inline TBool IsInTheRequestList(const TUsbcRequestCallback& aCallback);
    inline TBool IsInTheChargerTypeList(const TUsbcChargerTypeCallback& aCallback);
    static void ReconnectTimerCallback(TAny* aPtr);
    static void CableStatusTimerCallback(TAny* aPtr);
    static void PowerUpDfc(TAny* aPtr);
    static void PowerDownDfc(TAny* aPtr);
    static void DeviceEventNotifyDfc(TAny* aPtr);
    static void ThreadContextFinderDfc(TAny* aPtr);

private:

    //
    // --- Private data members ---
    //

    static DUsbClientController* UsbClientController[KUsbcMaxUdcs];

    TInt iDeviceTotalEndpoints;                                 // number of endpoints reported by PSL
    TInt iDeviceUsableEndpoints;                                // number of endpoints reported to LDD
    
    UsbShai::TUsbPeripheralState iDeviceState;                              // states as of USB spec chapter 9.1
    UsbShai::TUsbPeripheralState iDeviceStateB4Suspend;                     // state before entering suspend state

    TBool iSelfPowered;                                         // true if device is capable of beeing self-powered
    TBool iRemoteWakeup;                                        // true if device is capable of signalling rmwakeup
    TBool iHardwareActivated;                                   // true if controller silicon is in operating state
    TBool iOtgSupport;                                          // true if OTG is supported by this device
    TUint8 iOtgFuncMap;                                         // bitmap indicating OTG extension features
    TBool iHighSpeed;                                           // true if currently operating at high-speed
    
    TInt iEp0MaxPacketSize;                                     // currently configured max packet size for Ep0    
    const DBase* iEp0ClientId;                                  // see comment at the begin of ps_usbc.cpp
    TUint16 iEp0DataReceived;                                   // indicates how many bytes have already been received
    TBool iEp0WritePending;                                     // true if a write on ep0 has been set up
    TBool iEp0ClientDataTransmitting;                           // true if ep0's in DATA_IN on behalf of a client
    const DBase* iEp0DeviceControl;                             // Device Ep0 requests are delivered to this LDD
    
    TUsbcDescriptorPool iDescriptors;                           // the descriptors as of USB spec chapter 9.5
    TUint8 iCurrentConfig;                                      // bConfigurationValue of current Config (1-based!)
    RPointerArray<TUsbcConfiguration> iConfigs;                 // the root of the modelled USB device
    TUsbcPhysicalEndpoint iRealEndpoints[KUsbcEpArraySize];     // array will be filled once at startup
    TUint8 iEp0_TxBuf[KUsbcBufSz_Ep0Tx];                        // ep0 outgoing (tx) data is placed here
#ifdef USB_SUPPORTS_SET_DESCRIPTOR_REQUEST
    TUint8 iEp0_RxCollectionBuf[KUsbcBufSz_Ep0Rx];              // used for (optional) SET_DESCRIPTOR request
#endif
    TInt iEp0_RxExtraCount;                                     // number of bytes received but not yet delivered
    TBool iEp0_RxExtraData;                                     // true if iEp0_RxExtraCount is valid
    TInt iEp0_TxNonStdCount;                                    // number of bytes requested by non-std Ep0 request
    
    TUsbcRequestCallback* iRequestCallbacks[KUsbcEpArraySize];  // xfer requests; indexed by real ep number
    TSglQue<TUsbcRequestCallback> iEp0ReadRequestCallbacks;     // list of ep0 read requests
    TSglQue<TUsbcClientCallback> iClientCallbacks;              // registered LDD clients and their callback functions
    TSglQue<TUsbcStatusCallback> iStatusCallbacks;              // list of device state notification requests
    TSglQue<TUsbcEndpointStatusCallback> iEpStatusCallbacks;    // list of endpoint state notification requests
    TSglQue<TUsbcOtgFeatureCallback> iOtgCallbacks;             // list of OTG feature change requests
    
    NTimer iReconnectTimer;                                     // implements USB re-enumeration delay
    NTimer iCableStatusTimer;                                   // implements USB cable status detection delay
    DUsbcPowerHandler* iPowerHandler;                           // pointer to USB power handler object
    TSpinLock iUsbLock;                                         // implement SMP for USB PDD and LDD
    NFastMutex iMutex;                                          // To pretect interface set with NFastMutex
    UsbShai::MPeripheralControllerIf& iController;              // PSL code
    
    const UsbShai::TPeripheralControllerProperties iControllerProperties;   // Static configuration from PSL
    TBool iIsOtgPort;                                               // Is this instance a driver for otg port,
                                                                    // Set at construction time.

    /**
     * Pointer to the currently set OTG Observer, if any. Not owned.
     */
	MUsbOtgPeripheralObserverIf* iOtgObserver;  // Otg interface which is used to listen event from DeviceEventNotification
    
    DControlTransferManager* iConTransferMgr;    // Usb Control transfer manager, responsible for chaptor9
    
    TInt iLastError;                            // Used by chaptor9 sm. No else.
    TBool iSetupPacketPending;                  // Indicate whether the no-req-callback data is a setup packet
                                                            
    TUsbPeriDeviceEventQueue iDevEventQueue;    // PSL can call DeviceEventNotification(...) at any context, 
                                                // but for PIL, we want to serialize the execution of each event
    
    // The thread context for the DfcQ pass by PSL to us.
    NThread* iCommonDfcQThread;
    
protected:
    TDfc iPowerUpDfc;                         // queued by power handler upon power-up
    TDfc iPowerDownDfc;                       // queued by power handler upon power-down
    TDfc iDeviceEventNotifyDfc;               // queued by device event notification
    TDfc iThreadContextFinder;                // this is used to find which thread does the DfcQ passed by PSL
                                              // is runing.

private:
    TBool iStandby;                           // toggled by power handler as appropriate
    TBool iStackIsActive;                     // client stack's function is usable
    TBool iClientSupportReady;                // user-side USB Client support is loaded & active
    TBool iUsbResetDeferred;                  // set when user-side wasn't ready yet

public:
    // FIXME: These used to be filled by the OTG component Dummy DCD
    // so that the PIL layer can call it to enable and disable the D+
    // pull-up. This mechanism is no longer used
    TInt (*iEnablePullUpOnDPlus)(TAny* aOtgContext);
    TInt (*iDisablePullUpOnDPlus)(TAny* aOtgContext);
    TAny* iOtgContext;
private:	
    TSglQue<TUsbcChargerTypeCallback> iChargerTypeCallbacks;  // list of USB charger type notification requests
    TUint iCurrentChargerType;
    };
    
/** Simple queue of status changes to be recorded.
    Items are fetched by userside when able.
*/
class TUsbcDeviceStatusQueue
    {
public:
    TUsbcDeviceStatusQueue();
    void AddStatusToQueue(TUint32 aDeviceStatus);
    TInt GetDeviceQueuedStatus(TUint32& aDeviceStatus);
    void FlushQueue();

private:
    TUint32 iDeviceStatusQueue[KUsbDeviceStatusQueueDepth];
    TInt iStatusQueueHead;
    };
    
#include <usb/usbcshared.inl>

#endif    // __USBCSHARED_H__
