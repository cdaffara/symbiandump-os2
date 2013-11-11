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
* Implements a Symbian OS server that exposes the RUsb API
* 
*
*/



/**
 @file
*/

#ifndef __CUSBSERVER_INL__
#define __CUSBSERVER_INL__


/**
 * The CUsbServer::Device method
 *
 * Fetch the device that the server owns
 *
 * @internalComponent
 *
 * @return	The server's device
 */
inline CUsbDevice& CUsbServer::Device() const
	{
	return *iUsbDevice;
	}

inline TInt CUsbServer::SessionCount() const
	{
	return iSessionCount;
	}

#ifdef SYMBIAN_ENABLE_USB_OTG_HOST_PRIV
/**
 * The CUsbServer::Otg method
 *
 * Fetch the CUsbOtg object that the server owns
 *
 * @internalComponent
 *
 * @return	The OTG object
 */
inline CUsbOtg& CUsbServer::Otg() const
	{
	return *iUsbOtg;
	}

/**
 * The CUsbServer::Host method
 *
 * Fetch the CUsbHost object that the server owns
 *
 * @internalComponent
 *
 * @return	The HOST object
 */
inline CUsbHost& CUsbServer::Host() const
	{
	return *iUsbHost;
	}

#endif // SYMBIAN_ENABLE_USB_OTG_HOST_PRIV

#endif

