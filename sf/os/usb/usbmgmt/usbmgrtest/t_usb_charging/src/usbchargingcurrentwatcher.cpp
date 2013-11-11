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

#include <e32cmn.h>
#include <e32debug.h>
#include <usbchargingpublishedinfo.h>

#include "usbchargingcurrentwatcher.h"
#include "testmanager.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "usbchargingcurrentwatcherTraces.h"
#endif


CUsbChargingCurrentWatcher* CUsbChargingCurrentWatcher::NewL(CUsbChargingArmTest& aUsbChargingArm)
    {
    OstTraceFunctionEntry0( CUSBCHARGINGCURRENTWATCHER_NEWL_ENTRY );
    CUsbChargingCurrentWatcher* self = new(ELeave) CUsbChargingCurrentWatcher(aUsbChargingArm);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    OstTraceFunctionExit0( CUSBCHARGINGCURRENTWATCHER_NEWL_EXIT );
    return self;
    }

CUsbChargingCurrentWatcher::~CUsbChargingCurrentWatcher()
    {
    OstTraceFunctionEntry0( CUSBCHARGINGCURRENTWATCHER_CUSBCHARGINGCURRENTWATCHER_ENTRY );
    Cancel();
    iChargingCurrentProp.Close();
    OstTraceFunctionExit0( CUSBCHARGINGCURRENTWATCHER_CUSBCHARGINGCURRENTWATCHER_EXIT );
    }

CUsbChargingCurrentWatcher::CUsbChargingCurrentWatcher(CUsbChargingArmTest& aUsbChargingArm)
    : CActive(EPriorityStandard)
    , iUsbChargingArm(aUsbChargingArm)
    {
    OstTraceFunctionEntry0( DUP1_CUSBCHARGINGCURRENTWATCHER_CUSBCHARGINGCURRENTWATCHER_ENTRY );
    CActiveScheduler::Add(this);
    OstTraceFunctionExit0( DUP1_CUSBCHARGINGCURRENTWATCHER_CUSBCHARGINGCURRENTWATCHER_EXIT );
    }

void CUsbChargingCurrentWatcher::ConstructL()
    {
    OstTraceFunctionEntry0( CUSBCHARGINGCURRENTWATCHER_CONSTRUCTL_ENTRY );
    User::LeaveIfError(iChargingCurrentProp.Attach(KPropertyUidUsbBatteryChargingCategory, KPropertyUidUsbBatteryChargingInfo));
    
    PostAndNotifyCurrentL();
    OstTraceFunctionExit0( CUSBCHARGINGCURRENTWATCHER_CONSTRUCTL_EXIT );
    }

void CUsbChargingCurrentWatcher::DoCancel()
    {
    OstTraceFunctionEntry0( CUSBCHARGINGCURRENTWATCHER_DOCANCEL_ENTRY );
    iChargingCurrentProp.Cancel();
    OstTraceFunctionExit0( CUSBCHARGINGCURRENTWATCHER_DOCANCEL_EXIT );
    }

void CUsbChargingCurrentWatcher::RunL()
    {
    OstTraceFunctionEntry0( CUSBCHARGINGCURRENTWATCHER_RUNL_ENTRY );
    PostAndNotifyCurrentL();
    OstTraceFunctionExit0( CUSBCHARGINGCURRENTWATCHER_RUNL_EXIT );
    }

void CUsbChargingCurrentWatcher::PostAndNotifyCurrentL()
    {
    OstTraceFunctionEntry0( CUSBCHARGINGCURRENTWATCHER_POSTANDNOTIFYCURRENTL_ENTRY );
    iChargingCurrentProp.Subscribe(iStatus);
    SetActive();

    TPckgBuf<TPublishedUsbChargingInfo>  aChargingInfo;
    TInt err = iChargingCurrentProp.Get(aChargingInfo);
    User::LeaveIfError(err);
    OstTrace0( TRACE_NORMAL, CUSBCHARGINGCURRENTWATCHER_POSTANDNOTIFYCURRENTL, "CUsbChargingCurrentWatcher::PostAndNotifyCurrentL get P/S key changed" );
    iUsbChargingArm.ShowUsbChargingValue(aChargingInfo());    
    OstTraceFunctionExit0( CUSBCHARGINGCURRENTWATCHER_POSTANDNOTIFYCURRENTL_EXIT );
    }

