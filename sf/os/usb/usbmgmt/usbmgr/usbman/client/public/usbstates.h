/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* State enumerations for the RUsb Client side header
*
*/

/**
 @file
*/

#ifndef __USBSTATES_H__
#define __USBSTATES_H__

#include <e32def.h>

/** TUsbServiceState

	Enumeration of all the USB service states.

	@publishedPartner
	@released
*/
enum TUsbServiceState
	{
	/** EUsbServiceIdle 
		The service is not started.
	*/
	EUsbServiceIdle        = 0x01,
	
	/** EUsbServiceStarting */
	EUsbServiceStarting    = 0x02,
	
	/** EUsbServiceStarted */
	EUsbServiceStarted     = 0x04,
	
	/** EUsbServiceStopping */
	EUsbServiceStopping    = 0x08,

	/** EUsbServiceFatalError */
	EUsbServiceFatalError  = 0x10
	};

/** TUsbDeviceState

	Enumeration of all of the states of the USB device.
	The states reported are dependent on the hardware.

	*** If this changes update KUsbDeviceStates below ***

	@publishedPartner
	@released
*/
enum TUsbDeviceState
	{
	/** EUsbDeviceStateUndefined */
	EUsbDeviceStateUndefined  = 0x00,

	/** EUsbDeviceStateDefault */
	EUsbDeviceStateDefault    = 0x01,

	/** EUsbDeviceStateAttached */
	EUsbDeviceStateAttached   = 0x02,

	/** EUsbDeviceStatePowered */
	EUsbDeviceStatePowered    = 0x04,

	/** EUsbDeviceStateConfigured */
	EUsbDeviceStateConfigured = 0x08,

	/** EUsbDeviceStateAddress */
	EUsbDeviceStateAddress    = 0x10,

	/** EUsbDeviceStateSuspended */
	EUsbDeviceStateSuspended  = 0x20
	};

/**
Number of different USB Device States

@publishedPartner
*/
const TInt KUsbDeviceStates = 7;


#endif //__USBSTATES_H__
