/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/** @file
 @internalComponent
 @test
 */

#include "ncmwatcher.h"
#include "ncmtestconsole.h"

using namespace UsbNcm;

//
// CSharedStateWatcher
//
CSharedStateWatcher* CSharedStateWatcher::NewL(CUsbNcmConsole& aConsole)
    {
    CSharedStateWatcher* self = new (ELeave) CSharedStateWatcher(aConsole);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CSharedStateWatcher::CSharedStateWatcher(CUsbNcmConsole& aConsole) :
    CActive(EPriorityStandard), iConsole(aConsole), iNcmStateDes(
            reinterpret_cast<TUint8*> (&iSharedState), sizeof(iSharedState),
            sizeof(iSharedState))
    {
    CActiveScheduler::Add(this);
    }

CSharedStateWatcher::~CSharedStateWatcher()
    {
    Cancel();
    iProperty.Close();
    }

void CSharedStateWatcher::ConstructL()
    {
    User::LeaveIfError(iProperty.Attach(KC32ExeSid, KKeyNcmConnectionEvent));
    reportState();
    iProperty.Subscribe(iStatus);
    SetActive();
    }

void CSharedStateWatcher::RunL()
    {
    TInt err = iStatus.Int();
    iProperty.Subscribe(iStatus);
    SetActive();

    if (KErrNone == err)
        {
        reportState();
        }
    }

void CSharedStateWatcher::reportState()
    {
    iProperty.Get(iNcmStateDes);
    TBuf<DISPLAY_NCM_CONNECTION_STATE> stateBuf;
    switch (iSharedState.iState)
        {
        case ENcmStateDisconnected:
            stateBuf = _L("DisConnected");
            break;
        case ENcmStateConnected:
            stateBuf = _L("Connected");
            break;
        default:
            stateBuf = _L("Unknown");
            break;
        }
    __FLOG_STATIC2(KSubSys, KLogComponent ,
            _L8("NCM state=%d, iapid=%d"), iSharedState.iState, iSharedState.iIapId);

    iConsole.SetLocalIapId(iSharedState.iIapId);
    iConsole.SetDisplayItem(ENcmConnectionStateItem, stateBuf);
    }

/**
 Stop checking the shared state 
 */
void CSharedStateWatcher::DoCancel()
    {
    iProperty.Cancel();
    }
