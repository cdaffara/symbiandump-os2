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
#ifndef USBMANPRIVATECRKEYS_H
#define USBMANPRIVATECRKEYS_H

const TUid KCRUidUSBManagerConfiguration = { 0x101fe1db };

// Keys for USB configuration format
const TUint32 KUsbManConfigSign           = 0;

const TUint32 KUsbManDevicePartialKey = 0x00010000;
const TUint32 KUsbManDeviceCountIndexKey = 0x0001ff00;

const TUint32 KUsbManDeviceConfigurationsPartialKey = 0x00020000;
const TUint32 KUsbManDeviceConfigurationsCountIndexKey = 0x0002ff00;

const TUint32 KUsbManDevicePersonalitiesPartialKey = 0x00030000;
const TUint32 KUsbManDevicePersonalitiesCountIndexKey = 0x0003ff00;

const TUint32 KUsbManDeviceConfigurableClassesPartialKey = 0x00040000;
const TUint32 KUsbManDeviceConfigurableClassesCountIndexKey = 0x0004ff00;

const TUint32 KUsbManConfigKeyMask = 0x00FFFF00; 
const TUint32 KUsbManConfigIndexMask = 0x0000FF00;
const TUint32 KUsbManConfigFieldMask = 0x000000FF; 
const TUint32 KUsbManConfigFirstEntryMask = 0x00FF00FF;

const TUint32 KUsbManPersonalitiesOffset = 8;

const TInt KUsbManFeatureNotConfigurable = -1;

//Device
const TUint32 KUsbManDeviceVendorIdKey = 0x00000000;
const TUint32 KUsbManDeviceManufacturerNameKey = 0x00000001;
const TUint32 KUsbManDeviceProductNameKey = 0x00000002;

//Configurations
const TUint32 KUsbManDeviceConfigurationsPersonalityIdKey = 0x00000000;
const TUint32 KUsbManDeviceConfigurationsIdKey = 0x00000001;
const TUint32 KUsbManDeviceConfigurationsClassUidsKey = 0x00000002;
const TUint32 KUsbManDeviceConfigurationsKeyCountVersionFour = 3; //Key count of each configuration.

//Personalities
const TUint32 KUsbManDevicePersonalitiesDeviceClassKey = 0x00000000;
const TUint32 KUsbManDevicePersonalitiesDeviceSubClassKey = 0x00000001;
const TUint32 KUsbManDevicePersonalitiesProtocolKey = 0x00000002;
const TUint32 KUsbManDevicePersonalitiesNumConfigKey = 0x00000003;
const TUint32 KUsbManDevicePersonalitiesProductIdKey = 0x00000004;
const TUint32 KUsbManDevicePersonalitiesBcdDeviceKey = 0x00000005;
const TUint32 KUsbManDevicePersonalitiesFeatureIdKey = 0x00000006;
const TUint32 KUsbManDevicePersonalitiesPersonalityIdKey = 0x00000007;
const TUint32 KUsbManDevicePersonalitiesPropertyKey = 0x00000008;
const TUint32 KUsbManDevicePersonalitiesDescriptionKey = 0x00000009;

//Configurable Classes
const TUint32 KUsbManDeviceConfigurableClassesUidKey = 0x00000000;
const TUint32 KUsbManDeviceConfigurableClassesFeatureIdKey = 0x00000001;

#endif      // USBMANPRIVATECRKEYS_H
