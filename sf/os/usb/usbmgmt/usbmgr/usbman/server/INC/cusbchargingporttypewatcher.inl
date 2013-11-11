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



/**
 @file
*/

#ifndef CUSBCHARGINGPORTTYPEWATCHER_INL
#define CUSBCHARGINGPORTTYPEWATCHER_INL


/**
 * The CUsbChargingPortTypeWatcher::Owner method
 *
 * Fetch the device that owns this charger type watcher
 *
 * @internalComponent
 *
 * @return	The device that owns this charger type watcher
 */
inline CUsbDevice& CUsbChargingPortTypeWatcher::Owner() const
	{
	return iOwner;
	}

#endif //CUSBCHARGINGPORTTYPEWATCHER_INL