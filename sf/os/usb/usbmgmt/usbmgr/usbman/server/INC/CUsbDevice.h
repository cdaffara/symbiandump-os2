/**
* Copyright (c) 1997-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Implements the main object of Usbman that manages all the USB Classes
* and the USB Logical Device (via CUsbDeviceStateWatcher).
* 
*
*/



/**
 @file
*/

#ifndef __CUSBDEVICE_H__
#define __CUSBDEVICE_H__

#include <usbstates.h>
#include <musbclasscontrollernotify.h>
#include <ecom/ecom.h>

#ifndef __DUMMY_LDD__
#ifdef SYMBIAN_USB_BATTERYCHARGING_V1_1
#include <usb/d32usbc.h>	
#include <musbchargingnotify.h>
class CUsbChargingPortTypeWatcher;
#else
#include <d32usbc.h>	
#endif
#else
#include "dummyusblddapi.h"
#ifdef SYMBIAN_USB_BATTERYCHARGING_V1_1
#include <musbchargingnotify.h>
class CUsbChargingPortTypeWatcher;
#endif

#endif

#include <e32std.h>

#ifndef __DUMMY_LDD__
#include <musbmanextensionpluginobserver.h>
#else
#include <musbmanextensionpluginobserverdummy.h>
#endif


class CUsbDeviceStateWatcher;
class CUsbClassControllerBase;
class CUsbServer;
class MUsbDeviceNotify;
class CPersonality;
class CUsbmanExtensionPlugin;
class CUsbManCenRepManager;


const TUid KUidUsbPlugIns = {0x101fbf21};

/**
 * The CUsbDevice class
 *
 * Implements the main object of Usbman that manages all the USB Classes
 * and the USB Logical Device (via CUsbDeviceStateWatcher).
 * It owns one instance of CUsbDeviceStateWatcher and an instance of each USB 
 * Class Controller (CUsbClassControllerBase derived).
 * It also owns an instance of RDevUsbcClient, a handle on the logical device
 * driver for USB for Symbian OS.
 * It implements the MUsbClassControllerNotify mixin so all Usb Class
 * Controllers can notify it of any changes in their state.
 *
 * CUsbDevice is an active object which starts and stops Usb Class Controllers
 * asynchronously, one by one. Its RunL function will be called after each
 * start/stop.
 */
NONSHARABLE_CLASS(CUsbDevice) : public CActive, public MUsbClassControllerNotify, public MUsbmanExtensionPluginObserver
	{
public:
	class TUsbDeviceDescriptor
		{
	public:
		TUint8	iLength;
		TUint8  iDescriptorType;
		TUint16	iBcdUsb;
		TUint8  iDeviceClass;
		TUint8  iDeviceSubClass;
		TUint8  iDeviceProtocol;
		TUint8  iMaxPacketSize;
		TUint16 iIdVendor;
		TUint16	iProductId;
		TUint16	iBcdDevice;
		TUint8  iManufacturer;
		TUint8  iProduct;
		TUint8  iSerialNumber;
		TUint8  iNumConfigurations;
		};
		
      /**
       * See Central Repository
       */
      class TUsbDeviceConfiguration
            {
      public:
            TUint16 iVendorId;
            HBufC*  iManufacturerName;
            HBufC*  iProductName;
            };
public:
	static CUsbDevice* NewL(CUsbServer& aUsbServer);
	virtual ~CUsbDevice();

 	void EnumerateClassControllersL();
	void AddClassControllerL(CUsbClassControllerBase* aClassController, TLinearOrder<CUsbClassControllerBase> order);

	void RegisterObserverL(MUsbDeviceNotify& aObserver);
	void DeRegisterObserver(MUsbDeviceNotify& aObserver);

#ifdef SYMBIAN_USB_BATTERYCHARGING_V1_1	
	void RegisterChargingInfoObserverL(MUsbChargingNotify& aObserver);
	void DeRegisterChargingInfoObserver(MUsbChargingNotify& aObserver);	
	void SetChargingPortType(TUsbcChargingPortType aChargerType); 
	void PeerDeviceMaxPower(TUint aCurrent);
#endif

	void StartL();
	void Stop();

	inline TInt LastError() const;
	inline RDevUsbcClient& UsbBus();
	inline TUsbDeviceState DeviceState() const;
	inline TUsbServiceState ServiceState() const;
	inline TBool isPersonalityCfged() const;
	
	void SetServiceState(TUsbServiceState aState);
	void SetDeviceState(TUsbcDeviceState aState);

	void BusEnumerationCompleted();
	void BusEnumerationFailed(TInt aError);
	
	void TryStartL(TInt aPersonalityId);
	TInt CurrentPersonalityId() const;
	const RPointerArray<CPersonality>& Personalities() const;
	const CPersonality* GetPersonality(TInt aPersonalityId) const;
	void ValidatePersonalities();
	void ReadPersonalitiesL();
	void SetDefaultPersonalityL();
	void LoadFallbackClassControllersL();
	void ConvertUidsL(const TDesC& aStr, RArray<TUint>& aUidArray);
	
public: // From CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);

public: // Inherited from MUsbClassControllerNotify
	CUsbClassControllerIterator* UccnGetClassControllerIteratorL();
	void UccnError(TInt aError);

public: // from MUsbmanExtensionPluginObserver
	RDevUsbcClient& MuepoDoDevUsbcClient();
	void MuepoDoRegisterStateObserverL(MUsbDeviceNotify& aObserver);
#ifdef SYMBIAN_USB_BATTERYCHARGING_V1_1		
	void MuepoDoRegisterChargingObserverL(MUsbChargingNotify& aObserver);
#endif
protected:
	CUsbDevice(CUsbServer& aUsbServer);
	void ConstructL();
	void StartCurrentClassController();
	void StopCurrentClassController();

private:
	void SetDeviceDescriptorL();
	void SetUsbDeviceSettingsDefaultsL(TUsbDeviceDescriptor& aDeviceDescriptor);
	void SelectClassControllersL();
	void SetCurrentPersonalityL(TInt aPersonalityId);
	void SetUsbDeviceSettingsFromPersonalityL(CUsbDevice::TUsbDeviceDescriptor& aDeviceDescriptor);
	void ResourceFileNameL(TFileName& aFileName);
	void CreateClassControllersL(const RArray<TUid>& aClassUids);
	TInt PowerUpAndConnect();	
#ifdef _DEBUG
	void PrintDescriptor(CUsbDevice::TUsbDeviceDescriptor& aDeviceDescriptor);
#endif
	void InstantiateExtensionPluginsL();
private:
	RPointerArray<CUsbClassControllerBase> iSupportedClasses;
	RPointerArray<MUsbDeviceNotify> iObservers; 
	RPointerArray<CUsbmanExtensionPlugin> iExtensionPlugins;
	TUsbDeviceState  iDeviceState;
	TUsbServiceState iServiceState;
	TInt iLastError;
	RDevUsbcClient iLdd;
	CUsbDeviceStateWatcher* iDeviceStateWatcher;
#ifdef SYMBIAN_USB_BATTERYCHARGING_V1_1	
	CUsbChargingPortTypeWatcher* iChargerTypeWatcher;
	RPointerArray<MUsbChargingNotify> iChargingObservers;
#endif
	CUsbServer& iUsbServer;
	CUsbClassControllerIterator* iUsbClassControllerIterator;
	const CPersonality* iCurrentPersonality;
	RPointerArray<CPersonality> iSupportedPersonalities;
	RArray<TUid> iSupportedClassUids;
	TBool iPersonalityCfged;
	TBool iUdcSupportsCableDetectWhenUnpowered;
	HBufC16* iDefaultSerialNumber;
	CUsbManCenRepManager* iCenRepManager; // Own CenRepManager instance
	TUsbDeviceConfiguration iDeviceConfiguration;
	
	REComSession* iEcom;	//	Not to be deleted, only closed!
	};

#include "CUsbDevice.inl"

#endif

