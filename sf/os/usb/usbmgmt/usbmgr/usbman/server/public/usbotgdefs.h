/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Shared OTG definitions
*
*/

/**
 @file
 @publishedPartner
 @released
*/

#ifndef __USBOTGDEFS_H__
#define __USBOTGDEFS_H__

#include <e32std.h>
	
const TUint32 KUsbmanSvrSid = {0x101fe1db};
const TUid KUidUsbManCategory = {0x101fe1db};
const TUint KUsbOtgIdPinPresentProperty = 3; 	// Id-Pin present
const TUint KUsbOtgVBusPoweredProperty = 4;  	// VBus powered up
const TUint KUsbOtgStateProperty = 5;  			// OTG State
const TUint KUsbRequestSessionProperty = 6;  	// Request VBus
const TUint KUsbOtgConnectionIdleProperty = 7;  // Connection Idle

/**
	OTG State, supplied in a form that will enable use as an array
	index (note also how this is used below in the bit-mask form)
*/
enum TUsbOtgState
	{
	/**
	Single case of non-stable state, used only during
	startup
	*/
	EUsbOtgStateReset = 0x01,

	/**
	'A'-connection states (names are derived from OTG-Supplement
	Figure 6-2 On-The-Go A-Device State Diagram)
	*/
	EUsbOtgStateAIdle = 0x02,
	EUsbOtgStateAHost = 0x04,
	EUsbOtgStateAPeripheral = 0x08,
	EUsbOtgStateAVbusError = 0x10,

	/**
	'B'-connection states (names are derived from OTG-Supplement
	Figure 6-3 On-The-Go B-Device State Diagram)
	*/
	EUsbOtgStateBIdle = 0x20,
	EUsbOtgStateBPeripheral = 0x40,
	EUsbOtgStateBHost = 0x80,
	};

/**
	OTG Events, supplied in a form that can be used to create a mask
	that can specify a watcher's events of interest.
*/
enum TUsbOtgEvent
	{
	/**
	OTG events related to plug insertion or removal
	*/
	EUsbOtgEventAPlugInserted 		= 0x01,
	EUsbOtgEventAPlugRemoved  		= 0x02,

	/**
	OTG events relating to changes visible on the bus
	*/
	EUsbOtgEventVbusRaised			= 0x04,
	EUsbOtgEventVbusDropped			= 0x08,

	EUsbOtgEventSrpInitiated		= 0x10,

	EUsbOtgEventSrpReceived			= 0x20,
	EUsbOtgEventHnpEnabled			= 0x40,
	EUsbOtgEventHnpDisabled			= 0x80,

	/**
	OTG events related to changes in the current role the device
	is performing (independant of the orientation of the connection)
	*/
	EUsbOtgEventRoleChangedToHost	= 0x100,
	EUsbOtgEventRoleChangedToDevice	= 0x200,
	EUsbOtgEventRoleChangedToIdle	= 0x400
	};

#endif //__USBOTGDEFS_H__
