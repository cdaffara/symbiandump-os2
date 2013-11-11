/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __DUMMY_LDD_H__
#define __DUMMY_LDD_H__

#ifdef SYMBIAN_USB_BATTERYCHARGING_V1_1
#include <usb/usb_charger_detection_shai_shared.h>
#endif

static const TInt KDummyConfigDescSize = 9;

#ifdef SYMBIAN_USB_BATTERYCHARGING_V1_1
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
#endif

/******
 * NOTE: This dummy implementation of RDevUsbcClient is actually a C-class!!!!!!
 */
class RDevUsbcClient
	{
public:
	// functions needed by charging plugin
	inline TInt GetConfigurationDescriptor(TDes8& aConfigurationDescriptor);
	inline TInt SetConfigurationDescriptor(const TDesC8& aConfigurationDescriptor);
	inline TInt GetConfigurationDescriptorSize(TInt& aSize);
	inline void ReEnumerate(TRequestStatus& aStatus);
	inline void ReEnumerateCancel();
    inline TBool CurrentlyUsingHighSpeed();

	// used to initialise config desc.
	inline void Initialise();
	inline void SetCurrentlyUsingHighSpeed(TBool aUsingHighSpeed);
#ifdef SYMBIAN_USB_BATTERYCHARGING_V1_1	
	inline TInt ChargerDetectorCaps(TUsbcChargerDetectorProperties& aProperties);
    inline void SetChargerDetectorCaps(TUsbcChargerDetectorProperties aProperties);
#endif	
	
private:
	TBuf8<KDummyConfigDescSize> iConfigDesc;
	TBool iUsingHighSpeed;
#ifdef SYMBIAN_USB_BATTERYCHARGING_V1_1		
	TUsbcChargerDetectorProperties iChargerDetectorCaps;
#endif
	};

inline TInt RDevUsbcClient::GetConfigurationDescriptor(TDes8& aConfigurationDescriptor)
	{
	// 8th byte is bMaxPower
	aConfigurationDescriptor.Copy(iConfigDesc);
	return KErrNone;
	}

inline TInt RDevUsbcClient::SetConfigurationDescriptor(const TDesC8& aConfigurationDescriptor)
	{
	// 8th byte is bMaxPower
	iConfigDesc[8] = aConfigurationDescriptor[8];
	return KErrNone;
	}

inline TInt RDevUsbcClient::GetConfigurationDescriptorSize(TInt& aSize)
	{
	aSize = KDummyConfigDescSize;
	return KErrNone;
	}

inline void RDevUsbcClient::ReEnumerate(TRequestStatus& aStatus)
	{
	// just complete "synchronously". The plugin takes no notice of when this completes
	// so we don't need to fake it ever failing
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}

inline void RDevUsbcClient::ReEnumerateCancel()
	{
	// nothing to do, as ReEnumerate always completes "synchronously".
	}

inline void RDevUsbcClient::Initialise()
	{
	iConfigDesc.FillZ(KDummyConfigDescSize);
	// 8th byte is bMaxPower
	iConfigDesc[8] = 0;
	iUsingHighSpeed = EFalse;
	}
inline TBool RDevUsbcClient::CurrentlyUsingHighSpeed()
    {
    return iUsingHighSpeed;
    }

inline void RDevUsbcClient::SetCurrentlyUsingHighSpeed(TBool aUsingHighSpeed)
    {
    iUsingHighSpeed = aUsingHighSpeed;
    }

#ifdef SYMBIAN_USB_BATTERYCHARGING_V1_1
inline TInt RDevUsbcClient::ChargerDetectorCaps(TUsbcChargerDetectorProperties& aProperties)
    {
    aProperties = iChargerDetectorCaps;
    return KErrNone;
    }

inline void RDevUsbcClient::SetChargerDetectorCaps(TUsbcChargerDetectorProperties aProperties)
    {
    iChargerDetectorCaps = aProperties;
    }
#endif

#endif // __DUMMY_LDD_H__
