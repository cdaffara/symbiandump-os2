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

#include "ncmcoexistbearerwatcher.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "ncmcoexistbearerwatcherTraces.h"
#endif


#ifdef _DEBUG
_LIT(KCoBearerPanic, "CoexistBearerPanic");
#endif

CNcmCoexistBearerWatcher* CNcmCoexistBearerWatcher::NewL()
    {
    CNcmCoexistBearerWatcher* self = new(ELeave) CNcmCoexistBearerWatcher();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CNcmCoexistBearerWatcher::CNcmCoexistBearerWatcher() : CActive(CActive::EPriorityStandard)
    {
    CActiveScheduler::Add(this);
    }


CNcmCoexistBearerWatcher::~CNcmCoexistBearerWatcher()
    {
    Cancel();
    iProperty.Close();
    }


void CNcmCoexistBearerWatcher::ConstructL()
    {
    OstTraceFunctionEntry0(CNCMCOEXISTBEARERWATCHER_CONSTRUCTL);

    const TUint KIPBearerCoexistenceProperty = 0x10286a95;

    TInt err = iProperty.Define(KIPBearerCoexistenceProperty, RProperty::EInt, KAllowAllPolicy, KNetworkControlPolicy);
    if ( (err != KErrNone) && (err != KErrAlreadyExists) )
        {
        OstTrace1( TRACE_ERROR, CNCMCOEXISTBEARERWATCHER_CONSTRUCTL_DUP02, "Failed to define the property;err=%d", err );
        User::Leave(err);
        }

    TSecureId thisSID = RProcess().SecureId();
    User::LeaveIfError(iProperty.Attach(thisSID, KIPBearerCoexistenceProperty));
    User::LeaveIfError(iProperty.Get(reinterpret_cast<TInt&>(iBearerState)));

    OstTraceFunctionExit0(CNCMCOEXISTBEARERWATCHER_CONSTRUCTL_DUP01);
    }


TInt CNcmCoexistBearerWatcher::SetIpOverUsbActive(TRequestStatus& aStatus)
    {
    OstTraceFunctionEntry0(CNCMCOEXISTBEARERWATCHER_SETIPOVERUSBACTIVE);

    TInt ret = DoActive();
    OstTrace1(TRACE_NORMAL, CNCMCOEXISTBEARERWATCHER_SETIPOVERUSBACTIVE_DUP01, "DoActive()=%d.", ret);

    if (KErrNone == ret)
        {
        OstTraceFunctionExitExt(CNCMCOEXISTBEARERWATCHER_SETIPOVERUSBACTIVE_DUP02, this, KErrCompletion);
        return KErrCompletion;
        }
    else if (KErrInUse == ret)
        {
        iReportStatus = &aStatus;
        *iReportStatus = KRequestPending;

        iProperty.Subscribe(iStatus);
        SetActive();
        return KErrNone;
        }

    OstTraceFunctionExitExt(CNCMCOEXISTBEARERWATCHER_SETIPOVERUSBACTIVE_DUP03, this, ret);
    return ret;
    }
/**
 * do Set IP Over USB Actived.
 */
TInt CNcmCoexistBearerWatcher::DoActive()
    {
    __ASSERT_DEBUG(iBearerState != EIPBearerIPOverUSB, User::Panic(KCoBearerPanic, __LINE__));

    _LIT(KIPBearerCoexistenceMutex, "IPBearerCoexistenceMutex");
    RMutex gMutex;
    TInt error = gMutex.CreateGlobal(KIPBearerCoexistenceMutex);
    if ( error != KErrNone )
        {
        if( error == KErrAlreadyExists )
            {
            error = gMutex.OpenGlobal(KIPBearerCoexistenceMutex);
            if(error != KErrNone)
                {
                OstTrace1(TRACE_ERROR, CNCMCOEXISTBEARERWATCHER_DOACTIVE, "Failed to open global mutex, error=%d", error);
                return KErrNotReady;
                }
            }
        else
            {
            OstTrace1(TRACE_ERROR, CNCMCOEXISTBEARERWATCHER_DOACTIVE_DUP01,  "Failed to create a global mutex, error=%d", error);
            return KErrNotReady;
            }
        }

    gMutex.Wait();
    error = iProperty.Get(reinterpret_cast<TInt&>(iBearerState));
    if (error != KErrNone)
        {
        OstTrace1(TRACE_ERROR, CNCMCOEXISTBEARERWATCHER_DOACTIVE_DUP02, "Failed to get property value, error=%d", error);
        }
    else
        {
        if (iBearerState == EIPBearerNoneActive)
            {
            error = iProperty.Set(EIPBearerIPOverUSB);
            if (error != KErrNone)
                {
                OstTrace1(TRACE_ERROR, CNCMCOEXISTBEARERWATCHER_DOACTIVE_DUP03, "Failed to set property value, error=%d", error);
                }
            else
                {
                iBearerState = EIPBearerIPOverUSB;
                }
            }
        else
            {
            error = KErrInUse;
            __ASSERT_DEBUG(iBearerState != EIPBearerIPOverUSB, User::Panic(KCoBearerPanic, __LINE__));
            OstTrace1(TRACE_WARNING, CNCMCOEXISTBEARERWATCHER_DOACTIVE_DUP04, "Failed to set CoexistIPBearer to IPOverUsb since iBearerState=%d", iBearerState);
            }
        }

    gMutex.Signal();
    gMutex.Close();

    return error;
    }

TInt CNcmCoexistBearerWatcher::SetIpOverUsbDeactive()
    {
    __ASSERT_DEBUG(iBearerState==EIPBearerIPOverUSB, User::Panic(KCoBearerPanic, __LINE__));

    TInt err = iProperty.Set(EIPBearerNoneActive);
    OstTrace1(TRACE_NORMAL, CNCMCOEXISTBEARERWATCHER_SETIPOVERUSBDEACTIVE, "Stop using Coexistence Bearer, result=%d", err);
    iBearerState = EIPBearerNoneActive;

    return err;
    }

void CNcmCoexistBearerWatcher::RunL()
    {
    OstTrace1(TRACE_NORMAL, CNCMCOEXISTBEARERWATCHER_RUNL, "iStatus.Int()=%d", iStatus.Int());

    if (KErrNone == iStatus.Int())
        {
        TInt ret = DoActive();
        OstTrace1(TRACE_NORMAL, CNCMCOEXISTBEARERWATCHER_RUNL_DUP01, "DoActive()=%d", ret);
        if (KErrNone == ret)
            {
            User::RequestComplete(iReportStatus, KErrNone);
            }
        else if (KErrInUse == ret)
            {
            iProperty.Subscribe(iStatus);
            SetActive();
            }
        else
            {
            User::RequestComplete(iReportStatus, ret);
            }
        }
    else if (KErrCancel != iStatus.Int())
        {
        User::RequestComplete(iReportStatus, iStatus.Int());
        }
    }

void CNcmCoexistBearerWatcher::DoCancel()
    {
    OstTraceFunctionEntry0(CNCMCOEXISTBEARERWATCHER_DOCANCEL);
    iProperty.Cancel();
    User::RequestComplete(iReportStatus, KErrCancel);
    }
