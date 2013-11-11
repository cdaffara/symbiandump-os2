/*
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
*
*/

/** @file
 *
 * The mixin used by the USB Device object to notify all of
 * it's observers of any state changes that occur
 *
 * Copyright (c) 1997-2007 Symbian Ltd.  All rights reserved.
 */

#ifndef __MUSBDEVICENOTIFY_H__
#define __MUSBDEVICENOTIFY_H__

#include <usbstates.h>

/**
 * The MUsbDeviceNotify class
 *
 * The mixin used by the USB Device object to notify all of
 * it's observers of any state changes that occur

  @publishedPartner
  @released
 */
class MUsbDeviceNotify
	{
public:
	/**
	 * Called when the USB service state has changed
	 *
	 * @param aLastError The last error code detected
	 * @param aOldState The previous service state
	 * @param aNewState The new service state
	 */
	virtual void UsbServiceStateChange(TInt aLastError, TUsbServiceState aOldState, TUsbServiceState aNewState) = 0;

	/**
	 * Called when the USB device state has changed
	 *
	 * @param aLastError The last error code detected
	 * @param aOldState The previous device state
	 * @param aNewState The new device state
	 */
	virtual void UsbDeviceStateChange(TInt aLastError, TUsbDeviceState aOldState, TUsbDeviceState aNewState) = 0;
	};

#endif

