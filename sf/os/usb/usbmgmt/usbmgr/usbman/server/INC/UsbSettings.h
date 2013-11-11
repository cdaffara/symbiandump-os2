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
* Defines USB settings
* 
*
*/



/**
 @file
*/

#ifndef __USBSETTINGS_H__
#define __USBSETTINGS_H__

#include <e32std.h>

//	These definitions are used as the second level of defaults
//	and overlay the initial settings that are put in place by
//	the construction in the PDD (file pa_usbc.cpp)
//
//	********************************************************
//	* These values are *not* to be changed by the licensee *
//	********************************************************
//

const TUint16	KUsbDefaultBcdUsb				= 0x0000;
const TUint8	KUsbDefaultDeviceClass			= 0x02;
const TUint8	KUsbDefaultDeviceSubClass		= 0x00;
const TUint8	KUsbDefaultDeviceProtocol		= 0x00;
const TUint16	KUsbDefaultVendorId				= 0x0e22;
const TUint16	KUsbDefaultProductId			= 0x000b;
const TUint16	KUsbDefaultBcdDevice			= 0x0000;

_LIT(KUsbDefaultManufacturer, "Symbian Ltd.");
_LIT(KUsbDefaultProduct, "Symbian OS");
_LIT(KUsbDefaultSerialNumber, "0123456789");
_LIT(KUsbDefaultConfig, "First and Last and Always");

enum TUsbManagerSupportedVersion
	{
    TUsbManagerSupportedVersionMin = 4,
    TUsbManagerSupportedVersionFour = 4,        
    TUsbManagerSupportedVersionMax = 4
	};

#endif // __USBSETTINGS_H__
