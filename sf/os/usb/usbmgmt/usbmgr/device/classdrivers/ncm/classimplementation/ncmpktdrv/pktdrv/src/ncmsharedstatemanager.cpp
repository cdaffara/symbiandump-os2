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

#include "ncmsharedstatemanager.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "ncmsharedstatemanagerTraces.h"
#endif



#ifdef _DEBUG
_LIT(KPanicSharedState, "SharedStatePanic");
#endif

CNcmSharedStateManager* CNcmSharedStateManager::NewL(MShareStateObserver& aEngine)
    {
    CNcmSharedStateManager* self = new(ELeave) CNcmSharedStateManager(aEngine);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CNcmSharedStateManager::CNcmSharedStateManager(MShareStateObserver& aEngine) : CActive(CActive::EPriorityStandard),
                                          iNotifyMsg(RMessage2()), iEngine(aEngine)
    {
    CActiveScheduler::Add(this);
    }


CNcmSharedStateManager::~CNcmSharedStateManager()
    {
    OstTraceFunctionEntry0(CNCMSHAREDSTATEMANAGER_CNCMSHAREDSTATEMANAGER);

    Cancel();

    iProperty.Close();
    iProperty.Delete(KKeyNcmConnectionEvent);

    delete iCoexistBearer;
    OstTraceFunctionExit0(CNCMSHAREDSTATEMANAGER_CNCMSHAREDSTATEMANAGER_DUP01);
    }


void CNcmSharedStateManager::ConstructL()
    {
    OstTraceFunctionEntry0(CNCMSHAREDSTATEMANAGER_CONSTRUCTL);

    TInt  ccStartRequested  = ENcmStartRequested;
    User::LeaveIfError(RProperty::Get(KUsbmanSvrUid, KKeyNcmSharedState, ccStartRequested));
    if (ccStartRequested != ENcmStartRequested)
        {
        User::Leave(KErrNotReady);
        }

    User::LeaveIfError(iProperty.Define(KKeyNcmConnectionEvent, RProperty::EByteArray, KAllowAllPolicy, KNetworkControlPolicy));
    User::LeaveIfError(iProperty.Attach(RProcess().SecureId(), KKeyNcmConnectionEvent, EOwnerThread));

    iCoexistBearer = CNcmCoexistBearerWatcher::NewL();

    iNcmState().iIapId = 0;
    OstTraceFunctionExit0(CNCMSHAREDSTATEMANAGER_CONSTRUCTL_DUP01);
    }

/**
Maintains the shared state between the NCM class controller and packet driver.
*/
void CNcmSharedStateManager::RunL()
    {
    TInt err = iStatus.Int();

    OstTrace1(TRACE_NORMAL, CNCMSHAREDSTATEMANAGER_RUNL, "iStatus.Int()=%d", iStatus.Int());

    if (KErrNone == err)
        {
        DoDhcpProvisionRequest();
        }
    else if (KErrCancel != err)
        {
        NotifyNcmDisconnected(err);
        }
    }

/**
Stop checking the shared state
*/
void CNcmSharedStateManager::DoCancel()
    {
    iCoexistBearer->Cancel();
    }

TInt CNcmSharedStateManager::NotifyDhcpProvisionRequested()
    {
    OstTraceFunctionEntry0(CNCMSHAREDSTATEMANAGER_NOTIFYDHCPPROVISIONREQUESTED);

    TInt ret = iCoexistBearer->SetIpOverUsbActive(iStatus);

    if (ret == KErrCompletion)
        {
        return DoDhcpProvisionRequest();
        }
    else if (ret == KErrNone)
        {
        //notify the upper apps BTPAN is used by others.
        NotifyNcmDisconnected(KErrInUse);
        SetActive();
        }
    else
        {
        NotifyNcmDisconnected(ret);
        }

    OstTraceFunctionEntry0(CNCMSHAREDSTATEMANAGER_NOTIFYDHCPPROVISIONREQUESTED_DUP01);
    return ret;
    }

TInt CNcmSharedStateManager::DoDhcpProvisionRequest()
    {
    if (iNotifyMsg.IsNull())
        {
        __ASSERT_DEBUG(0, User::Panic(KPanicSharedState, __LINE__));

        //return AlreadyExists, so that Engine can start immediately.
        return KErrAlreadyExists;
        }
    else
        {
        iNotifyMsg.Complete(KErrNone);
        iNotifyMsg = RMessage2();
        }

    return KErrNone;
    }


void CNcmSharedStateManager::NotifyNcmConnected()
    {
    iNcmState().iState = ENcmStateConnected;
    iProperty.Set(iNcmState);
    }

void CNcmSharedStateManager::NotifyNcmDisconnected(TInt aReason)
    {
    iNcmState().iState = ENcmStateDisconnected;
    iNcmState().iErrCode = aReason;
    iProperty.Set(iNcmState);

    if (iCoexistBearer->IsIpOverUsbActived())
        {
        iCoexistBearer->SetIpOverUsbDeactive();
        }
    else
        {
        Cancel();
        }
    }

TInt CNcmSharedStateManager::SetStateValue(TInt aType, TInt aValue)
    {
    OstTraceExt2(TRACE_NORMAL, CNCMSHAREDSTATEMANAGER_SETSTATEVALUE, "SetStateValue: type=%d, val=%d", aType, aValue);

    const TInt KTypeIapId = 0;
    const TInt KTypeDhcpReturn = 1;
    TInt ret = KErrNone;
    switch(aType)
        {
        case KTypeIapId:
            //don't assign more than once.
            __ASSERT_DEBUG(iNcmState().iIapId==0, User::Panic(KPanicSharedState, __LINE__));
            iNcmState().iIapId = aValue;
            break;

        case KTypeDhcpReturn:
            if (KErrNone == aValue)
                {
                iEngine.NotifyDhcpStarted();
                }
            else
                {
                OstTrace1(TRACE_WARNING, CNCMSHAREDSTATEMANAGER_SETSTATEVALUE_DUP01, "Dhcp Provision Request return error:%d", aValue);
                NotifyNcmDisconnected(aValue);
                }
            break;

        default:
            OstTraceExt2(TRACE_ERROR, CNCMSHAREDSTATEMANAGER_SETSTATEVALUE_DUP02, "unknow type:%d, value:%d", aType, aValue);
            __ASSERT_DEBUG(0, User::Panic(KPanicSharedState, __LINE__));
            break;
        }

    return ret;
    }

TInt CNcmSharedStateManager::RegisterNotify(const RMessage2& aMsg)
    {
    if (!iNotifyMsg.IsNull())
        {
        return KErrInUse;
        }

    iNotifyMsg = aMsg;

    return KErrNone;
    }

TInt CNcmSharedStateManager::DeRegisterNotify()
    {
    if (iNotifyMsg.IsNull())
        {
        return KErrNotReady;
        }
    else
        {
        iNotifyMsg.Complete(KErrCancel);
        iNotifyMsg = RMessage2();
        }

    return KErrNone;
    }
