// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the License "Symbian Foundation License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.symbianfoundation.org/legal/sfl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#ifndef __USBSTATEWATCHER_H__
#define __USBSTATEWATCHER_H__

#include <e32property.h>
#include <e32cmn.h>
#include "usbchargingarmtest.h"
#include "testbase.h"

/**
*/
class CUsbStateWatcher : public CActive
    {
public:
    static CUsbStateWatcher* NewL(CUsbChargingArmTest& aUsbChargingArm);
    ~CUsbStateWatcher();

private:
    CUsbStateWatcher(CUsbChargingArmTest& aUsbChargingArm);
    void ConstructL();
    
    void DoCancel();
    void RunL();
    
    void GetAndShowDeviceStateL();

private:
    CUsbChargingArmTest& iUsbChargingArm;
    TUsbDeviceState     iDeviceState;    
    };

#endif // __USBSTATEWATCHER_H__
