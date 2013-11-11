/**
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Implements the main object of UsbMan that manages all the USB Classes
* and the USB Logical Device (via CUSBDeviceHandler and CUSBStateWatcher).
* 
*
*/



/**
 @file
*/

#ifndef __CUSBDEVICE_INL__
#define __CUSBDEVICE_INL__


/**
 * The CUsbDevice::LastError method
 *
 * Fetch the last error that occurred
 *
 * @internalComponent
 *
 * @return	Last error that occurred, KErrNone if no error
 */
inline TInt CUsbDevice::LastError() const
	{
	return iLastError;
	}


/**
 * The CUsbDevice::UsbBus method
 *
 * Fetch a reference to the USB Logical Device Device
 *
 * @internalComponent
 *
 * @return	A reference to the USB Logical Device Device
 */
inline RDevUsbcClient& CUsbDevice::UsbBus()
	{
	return iLdd;
	}


/**
 * The CUsbDevice::DeviceState method
 *
 * Fetch the device's current state
 *
 * @internalComponent
 *
 * @return	Device's current state
 */
inline TUsbDeviceState CUsbDevice::DeviceState() const
	{
	return iDeviceState;
	}

/**
 * The CUsbDevice::ServiceState method
 *
 * Fetch the device's current state
 *
 * @internalComponent
 *
 * @return	Device's current service state
 */
inline TUsbServiceState CUsbDevice::ServiceState() const
	{
	return iServiceState;
	}

/**
 * Checks if personalities are configured for the device
 *
 * @internalComponent 
 * @return ETrue or EFalse
 */
 
inline TBool CUsbDevice::isPersonalityCfged() const
	{
	return iPersonalityCfged;
	}
	
#endif
