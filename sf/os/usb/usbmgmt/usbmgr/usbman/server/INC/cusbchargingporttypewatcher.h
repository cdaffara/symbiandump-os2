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

#ifndef CUSBCHARGINGPORTTYPEWATCHER_H
#define CUSBCHARGINGPORTTYPEWATCHER_H

#include <e32def.h>

class CUsbDevice;
class RDevUsbcClient;

/**
 * The CUsbChargingPortTypeWatcher class
 *
 * Talks directly to the USB Logical Device Driver (LDD) and 
 * watches charging port type
 */
NONSHARABLE_CLASS(CUsbChargingPortTypeWatcher) : public CActive
	{
public:
	static CUsbChargingPortTypeWatcher* NewL(CUsbDevice& aOwner, RDevUsbcClient& aLdd);
	virtual ~CUsbChargingPortTypeWatcher();

	inline CUsbDevice& Owner() const;

	// From CActive
	virtual void RunL();
	virtual void DoCancel();

	virtual void Start();

protected:
	CUsbChargingPortTypeWatcher(CUsbDevice& aOwner, RDevUsbcClient& aLdd);

private:
	CUsbDevice& iOwner;
	RDevUsbcClient& iLdd;
	TUint iChargingPortType;
	};

#include "cusbchargingporttypewatcher.inl"

#endif //CUSBCHARGINGPORTTYPEWATCHER_H

