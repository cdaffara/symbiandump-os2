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
*
*/

/**
 @file
 @internalComponent
 @released
*/

#ifndef USBINTERNALOBSERVERS_H
#define USBINTERNALOBSERVERS_H

/**
 * The MUsbMessageObserver class
 *
 * The mixin used by the USB Otg/Host watcher objects to notify their
 * owners of any messages that occur
 *
 * @internalComponent
 * @released
 */
class MUsbOtgObserver
	{
public:
	virtual void NotifyMessage(TInt aMessage = 0) = 0;
	};

/**
 * The MUsbHostObserver class
 *
 * The mixin used by the USB Host watcher objects to notify their
 * owners of any Host events that occur
 *
 * @internalComponent
 * @released
 */
class MUsbHostObserver
	{
public:
	virtual void NotifyHostEvent(TUint aWatcherId) = 0;
	};

#endif
