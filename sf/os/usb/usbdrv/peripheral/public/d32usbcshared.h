// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// e32/include/d32usbcshared.h
// User side class definitions for USB Device support.
// 
//

/**
 @file d32usbcshared.h
 @publishedPartner
 @released
*/

#ifndef __D32USBCSHARED_H__
#define __D32USBCSHARED_H__

#include <usb/usb_peripheral_shai_shared.h>
#include <usb/usb_charger_detection_shai_shared.h>


// FIXME: for dummy dcd compitible, move this line to usbcshared.h
// used by shared chunk version LDD's user app interface
typedef TInt TUsbcDeviceState;

// USB driver error codes

/** USB driver specific error codes start from here
*/
const TInt KErrUsbDriverBase = -6701;

/**	Attempt at data transfer, or something interface related,
	when neither an Interface has been set up nor Device Control is owned by
	the channel
*/
const TInt KErrUsbInterfaceNotReady = -6702;

/**	Attempt at data transfer on an endpoint that does not belong to the active interface
*/
const TInt KErrUsbEpNotInInterface = -6703;

/**	Attempt at data transfer in a direction not supported by the endpoint
*/
const TInt KErrUsbEpBadDirection = -6704;

/**	The data transfer size specified exceeds that of the source or
	destination buffer descriptor
*/
const TInt KErrUsbTransferSize = -6705;

/**	This has multiple uses:
	1) User request completed because device is no longer in configured state
	2) Something endpoint related, stall, unstall, status enquiry etc,
	   that requires the device to be configured
*/
const TInt KErrUsbDeviceNotConfigured = -6706;

/**	Requested endpoint properties inconsistent during Interface setup
*/
const TInt KErrUsbBadEndpoint = -6707;

/**	User data request completed because channel is closing (channel destructor called)
*/
const TInt KErrUsbDeviceClosing = -6708;

/**	User data request completed because current endpoint set is being
	replaced since alternate setting is changing
*/
const TInt KErrUsbInterfaceChange = -6709;

/**	User data request completed because cable has been detached (or equivalent)
*/
const TInt KErrUsbCableDetached = -6710;

/**	User data request completed because cable has been detached (or equivalent)
*/
const TInt KErrUsbDeviceBusReset = -6711;

/**	This means that read data is still available when a write request is made.
	Relates to bidirectional eps only (ep0).
	A bidirectional ep must consume all of its read data before attempting to write.
*/
const TInt KErrUsbEpNotReady = -6712;

// FIXME: REMOVE THIS GUY, ONCE DUMMY DCD is modified according to new SHAI.
const TInt EUsbcDeviceStateUndefined = UsbShai::EUsbPeripheralStateUndefined; // 0
const TInt EUsbcDeviceStateAttached = UsbShai::EUsbPeripheralStateAttached; // 1
const TInt EUsbcDeviceStatePowered = UsbShai::EUsbPeripheralStatePowered;// 2
const TInt EUsbcDeviceStateDefault = UsbShai::EUsbPeripheralStateDefault;// 3
const TInt EUsbcDeviceStateAddress = UsbShai::EUsbPeripheralStateAddress; // 4
const TInt EUsbcDeviceStateConfigured = UsbShai::EUsbPeripheralStateConfigured;// 5
const TInt EUsbcDeviceStateSuspended = UsbShai::EUsbPeripheralStateSuspended;// 6
const TInt EUsbcNoState = UsbShai::EUsbPeripheralNoState; // 255 (used as a place holder)

/** The endpoint states.

	@see RDevUsbcClient::EndpointStatus()
	@see RDevUsbcClient::EndpointStatusNotify()
*/
enum TEndpointState
	{
	EEndpointStateNotStalled,
	EEndpointStateStalled,
	EEndpointStateUnknown
	};

class TUsbcEndpointCaps : public UsbShai::TUsbPeripheralEndpointCaps
    {
    public:
	/** Returns the greatest available packet size for this endpoint. */
	TInt MaxPacketSize() const;
	/** Returns the smallest available packet size for this endpoint. */
	TInt MinPacketSize() const;    
    };

typedef UsbShai::TPortType TUsbcChargingPortType;
typedef UsbShai::TChargerDetectorProperties TUsbcChargerDetectorProperties;
const TUint KUsbChargingPortTypeNone = UsbShai::EPortTypeNone;
const TUint KUsbChargingPortTypeUnsupported = UsbShai::EPortTypeUnsupported;
const TUint KUsbChargingPortTypeChargingPort = UsbShai::EPortTypeChargingPort;
const TUint KUsbChargingPortTypeDedicatedChargingPort = UsbShai::EPortTypeDedicatedChargingPort;
const TUint KUsbChargingPortTypeChargingDownstreamPort = UsbShai::EPortTypeChargingDownstreamPort;
const TUint KUsbChargingPortTypeStandardDownstreamPort = UsbShai::EPortTypeStandardDownstreamPort;
const TUint KUsbChargingPortTypeAcaRidA = UsbShai::EPortTypeAcaRidA;
const TUint KUsbChargingPortTypeAcaRidB = UsbShai::EPortTypeAcaRidB;
const TUint KUsbChargingPortTypeAcaRidC = UsbShai::EPortTypeAcaRidC;
const TUint KChargerDetectorCapChirpCurrentLimiting = UsbShai::TChargerDetectorProperties::KChargerDetectorCapChirpCurrentLimiting;


	
/** Endpoint resources/behaviours.

	@see AllocateEndpointResource()
	@see DeAllocateEndpointResource()
	@see QueryEndpointResourceUse()
*/
enum TUsbcEndpointResource
	{
	/** Requests the use of DMA. */
	EUsbcEndpointResourceDMA = 0,
	/** Requests the use of double FIFO buffering. */
	EUsbcEndpointResourceDoubleBuffering = 1
	};

const TUint KUsbcEndpointInfoFeatureWord1_DMA = UsbShai::KUsbEndpointInfoFeatureWord1_DMA;
const TUint KUsbcEndpointInfoFeatureWord1_DoubleBuffering = UsbShai::KUsbEndpointInfoFeatureWord1_DoubleBuffering;
    
/** The USB client device capability class.
*/
class TCapsDevUsbc
	{
    public:
	/** The device version. */
	TVersion version;
	};

/** The maximum number of endpoints supported by the device, excluding ep0.
*/
const TInt KUsbcMaxEndpoints = 30;

/** The maximum number of endpoints per interface, excluding ep0.
*/
const TInt KMaxEndpointsPerClient = 5;

/** @internalComponent
*/
const TInt KInvalidEndpointNumber = 31;

/** The alternate setting flag; when this bit is set the state change notified by
	RDevUsbcClient::AlternateDeviceStatusNotify() is an alternate setting number.
*/
const TUint KUsbAlternateSetting = 0x80000000;

/** The USB cable detection feature flag; used by TUsbDeviceCapsV01::iFeatureWord1.
	When this bit is set then the USB controller hardware (UDC) supports detection
	of a plugged-in USB cable even when not powered.

	@see TUsbDeviceCapsV01
*/
const TUint KUsbDevCapsFeatureWord1_CableDetectWithoutPower = 0x00000001;

/** If this flag is set then the driver supports the new endpoint resource
	allocation scheme for DMA and Double-buffering via
	TUsbcEndpointInfo::iFeatureWord1.

	@see TUsbDeviceCapsV01
*/
const TUint KUsbDevCapsFeatureWord1_EndpointResourceAllocV2 = 0x00000002;


/** Device USB capabilities.
*/
class TUsbDeviceCapsV01
	{
public:
	/** The total number of endpoints on the device. */
	TInt iTotalEndpoints;
	/** Indicates whether the device supports software connect/disconnect. */
	TBool iConnect;
	/** Indicates whether the device is self powered. */
	TBool iSelfPowered;
	/** Indicates whether the device can send Remote Wakeup. */
	TBool iRemoteWakeup;
	/** Indicates whether the device supports High-speed mode. */
	TBool iHighSpeed;
	/** 32 flag bits indicating miscellaneous UDC/device features.
		Currently defined are:
		- KUsbDevCapsFeatureWord1_CableDetectWithoutPower = 0x00000001
		- KUsbDevCapsFeatureWord1_EndpointResourceAllocV2 = 0x00000002
	*/
	TUint32 iFeatureWord1;
	/** Reserved for future use. */
	TUint32 iReserved;
	};

/** Package buffer for a TUsbDeviceCapsV01 object.

	@see TUsbDeviceCapsV01
*/
typedef TPckgBuf<TUsbDeviceCapsV01> TUsbDeviceCaps;

/** Bitmaps for TUsbcEndpointCaps.iSizes.

	This endpoint is not available (= no size).
*/
const TUint KUsbEpNotAvailable = 0x00000000;
/**	Max packet size is continuously variable up to some size specified.
	(Interrupt and Isochronous endpoints only.)
*/
const TUint KUsbEpSizeCont     = 0x00000001;
/** Max packet size 8 bytes is supported
*/
const TUint KUsbEpSize8        = 0x00000008;
/** Max packet size 16 bytes is supported
*/
const TUint KUsbEpSize16       = 0x00000010;
/** Max packet size 32 bytes is supported
*/
const TUint KUsbEpSize32       = 0x00000020;
/** Max packet size 64 bytes is supported
*/
const TUint KUsbEpSize64       = 0x00000040;
/** Max packet size 128 bytes is supported
*/
const TUint KUsbEpSize128      = 0x00000080;
/** Max packet size 256 bytes is supported
*/
const TUint KUsbEpSize256      = 0x00000100;
/** Max packet size 512 bytes is supported
*/
const TUint KUsbEpSize512      = 0x00000200;
/** Max packet size 1023 bytes is supported
*/
const TUint KUsbEpSize1023     = 0x00000400;
/** Max packet size 1024 bytes is supported
*/
const TUint KUsbEpSize1024     = 0x00000800;

/** Bitmaps for TUsbcEndpointCaps.iSupportedTypesAndDir.

	Endpoint supports Control transfer type.
*/
const TUint KUsbEpTypeControl     = UsbShai::KUsbEpTypeControl;
/** Endpoint supports Isochronous transfer type.
*/
const TUint KUsbEpTypeIsochronous = UsbShai::KUsbEpTypeIsochronous;
/** Endpoint supports Bulk transfer type.
*/
const TUint KUsbEpTypeBulk        = UsbShai::KUsbEpTypeBulk;
/** Endpoint supports Interrupt transfer type.
*/
const TUint KUsbEpTypeInterrupt   = UsbShai::KUsbEpTypeInterrupt;
/** Endpoint supports IN transfers.
*/
const TUint KUsbEpDirIn           = UsbShai::KUsbEpDirIn;
/** Endpoint supports OUT transfers.
*/
const TUint KUsbEpDirOut          = UsbShai::KUsbEpDirOut;
/** Endpoint supports bidirectional (Control) transfers only.
*/
const TUint KUsbEpDirBidirect     = UsbShai::KUsbEpDirBidirect;


/** Converts an absolute size value into a KUsbEpSize... mask.
*/
static inline TUint PacketSize2Mask(TInt aSize);

/** Converts an endpoint type mask KUsbEpType...  into an endpoint attribute
	value KUsbEpAttr_....
*/
static inline TUint EpTypeMask2Value(TInt aType);


/** Endpoint capabilities as returned by RDevUsbcClient::EndpointCaps().
*/
class TUsbcEndpointData
	{
public:
	/** Detail of endpoint capabilities. */
	TUsbcEndpointCaps iCaps;
	/** Indicates whether this endpoint is already claimed. */
	TBool iInUse;
	};

class TUsbcEndpointInfo : public UsbShai::TUsbPeripheralEndpointInfo 
    {
    public:
	TUsbcEndpointInfo(TUint aType=UsbShai::KUsbEpTypeBulk, TUint aDir=UsbShai::KUsbEpDirOut,
					  TInt aSize=0, TInt aInterval=0, TInt aExtra=0);
	
	TInt AdjustEpSizes(TInt& aEpSize_Fs, TInt& aEpSize_Hs) const;
	
    TInt AdjustPollInterval();
    };
    
/** USB Class information used in RDevUsbcClient::SetInterface().
*/
class TUsbcClassInfo
	{
public:
	TUsbcClassInfo(TInt aClass=0, TInt aSubClass=0, TInt aProtocol=0);
public:
	/** The class type number. */
	TInt iClassNum;
	/** The sub-class type number. */
	TInt iSubClassNum;
	/** The protocol number. */
	TInt iProtocolNum;
	/** Reserved for future use. */
	TUint32 iReserved;
	};


/** The Ep0 Setup request 'unsubscribe' flag; used by
	TUsbcInterfaceInfo::iFeatureWord. When this bit is set then valid vendor-
	or class-specific Ep0 requests addressed to this interface or any of its
	endpoints will be stalled by the USB PDD PIL.

	@see TUsbcInterfaceInfo
*/
const TUint KUsbcInterfaceInfo_NoEp0RequestsPlease = 0x00000001;

#include <usb/d32usbcshared.inl>

#endif 
