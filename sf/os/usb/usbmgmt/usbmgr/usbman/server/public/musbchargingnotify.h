/*
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
*/

/**
 @file
 @publishedPartner
 @released
*/

#ifndef __MUSBCHARGINGNOTIFY_H__
#define __MUSBCHARGINGNOTIFY_H__

const TInt KUsbChargingNotifyInterface = 0x102872D8;


class MUsbChargingNotify
	{
public:

	/**
	Called by USBMAN when it receives charging notification.
	@param aChargingType The concrete charging type of notification.
	*/
	
	virtual void UsbChargingPortType(TUint aPortType) = 0;
	virtual void PeerDeviceMaxPower(TUint aCurrent) = 0;
	};

#endif