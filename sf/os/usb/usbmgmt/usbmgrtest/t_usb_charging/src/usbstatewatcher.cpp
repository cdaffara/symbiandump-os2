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

#include "usbstatewatcher.h"
#include <e32cmn.h>
#include "testmanager.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "usbstatewatcherTraces.h"
#endif


CUsbStateWatcher* CUsbStateWatcher::NewL(CUsbChargingArmTest& aUsbChargingArm)
    {
    OstTraceFunctionEntry0( CUSBSTATEWATCHER_NEWL_ENTRY );
    CUsbStateWatcher* self = new(ELeave) CUsbStateWatcher(aUsbChargingArm);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    OstTraceFunctionExit0( CUSBSTATEWATCHER_NEWL_EXIT );
    return self;
    }

CUsbStateWatcher::~CUsbStateWatcher()
    {
    OstTraceFunctionEntry0( CUSBSTATEWATCHER_CUSBSTATEWATCHER_ENTRY );
    Cancel();
    OstTraceFunctionExit0( CUSBSTATEWATCHER_CUSBSTATEWATCHER_EXIT );
    }

CUsbStateWatcher::CUsbStateWatcher(CUsbChargingArmTest& aUsbChargingArm)
    : CActive(EPriorityStandard)
    , iUsbChargingArm(aUsbChargingArm)
    {
    OstTraceFunctionEntry0( DUP1_CUSBSTATEWATCHER_CUSBSTATEWATCHER_ENTRY );
    CActiveScheduler::Add(this);
    OstTraceFunctionExit0( DUP1_CUSBSTATEWATCHER_CUSBSTATEWATCHER_EXIT );
    }

void CUsbStateWatcher::ConstructL()
    {
    OstTraceFunctionEntry0( CUSBSTATEWATCHER_CONSTRUCTL_ENTRY );
    GetAndShowDeviceStateL();
    OstTraceFunctionExit0( CUSBSTATEWATCHER_CONSTRUCTL_EXIT );
    }

void CUsbStateWatcher::DoCancel()
    {
    OstTraceFunctionEntry0( CUSBSTATEWATCHER_DOCANCEL_ENTRY );
    iUsbChargingArm.Usb().DeviceStateNotificationCancel();
    OstTraceFunctionExit0( CUSBSTATEWATCHER_DOCANCEL_EXIT );
    }

void CUsbStateWatcher::RunL()
    {
    OstTraceFunctionEntry0( CUSBSTATEWATCHER_RUNL_ENTRY );
    GetAndShowDeviceStateL();
    OstTraceFunctionExit0( CUSBSTATEWATCHER_RUNL_EXIT );
    }

void CUsbStateWatcher::GetAndShowDeviceStateL()
    {
    OstTraceFunctionEntry0( CUSBSTATEWATCHER_GETANDSHOWDEVICESTATEL_ENTRY );
    TUsbDeviceState deviceState;
    User::LeaveIfError(iUsbChargingArm.Usb().GetDeviceState(deviceState));
    if(deviceState != iDeviceState)
        {
        deviceState = iDeviceState;
        }
    iUsbChargingArm.ShowUsbDeviceState(deviceState);  
    
    iUsbChargingArm.Usb().DeviceStateNotification(0xffffffff, iDeviceState, iStatus);
    SetActive();    
    OstTraceFunctionExit0( CUSBSTATEWATCHER_GETANDSHOWDEVICESTATEL_EXIT );
    }

