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
* Talks directly to the USB Logical Device Driver (LDD) and 
* watches any state changes
* 
*
*/



/**
 @file
*/

#ifndef __CUSBDEVICESTATEWATCHER_H__
#define __CUSBDEVICESTATEWATCHER_H__

#include <e32def.h>

class CUsbDevice;
class RDevUsbcClient;

/**
 * The CUsbDeviceStateWatcher class
 *
 * Talks directly to the USB Logical Device Driver (LDD) and 
 * watches any state changes
 */
NONSHARABLE_CLASS(CUsbDeviceStateWatcher) : public CActive
	{
public:
	static CUsbDeviceStateWatcher* NewL(CUsbDevice& aOwner, RDevUsbcClient& aLdd);
	virtual ~CUsbDeviceStateWatcher();

	inline CUsbDevice& Owner() const;

	// From CActive
	virtual void RunL();
	virtual void DoCancel();

	virtual void Start();

protected:
	CUsbDeviceStateWatcher(CUsbDevice& aOwner, RDevUsbcClient& aLdd);

private:
	CUsbDevice& iOwner;
	RDevUsbcClient& iLdd;
	TUint iState;
	};

#include "CUsbDeviceStateWatcher.inl"

#endif

