/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent
*/

#include "refppnotificationman.h"
#include <usb/usblogger.h>
#include <usb/hostms/policypluginnotifier.hrh>
#include "srvpanic.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "refppnotificationmanTraces.h"
#endif



#ifdef __OVER_DUMMYCOMPONENT__
const TUid KMountPolicyNotifierUid = {0x1028653E};
#else
const TUid KMountPolicyNotifierUid = {KUidMountPolicyNotifier};
#endif

CMsmmPolicyNotificationManager::~CMsmmPolicyNotificationManager()
    {
    OstTraceFunctionEntry0( REF_CMSMMPOLICYNOTIFICATIONMANAGER_CMSMMPOLICYNOTIFICATIONMANAGER_DES_ENTRY );
    
    Cancel();
    iErrorQueue.Close();
    iNotifier.Close();
    OstTraceFunctionExit0( REF_CMSMMPOLICYNOTIFICATIONMANAGER_CMSMMPOLICYNOTIFICATIONMANAGER_DES_EXIT );
    }

CMsmmPolicyNotificationManager* CMsmmPolicyNotificationManager::NewL()
    {
    OstTraceFunctionEntry0( REF_CMSMMPOLICYNOTIFICATIONMANAGER_NEWL_ENTRY );
    
    CMsmmPolicyNotificationManager* self = 
        CMsmmPolicyNotificationManager::NewLC();
    CleanupStack::Pop(self);
    
    OstTraceFunctionExit0( REF_CMSMMPOLICYNOTIFICATIONMANAGER_NEWL_EXIT );
    return self;
    }

CMsmmPolicyNotificationManager* CMsmmPolicyNotificationManager::NewLC()
    {
    OstTraceFunctionEntry0( REF_CMSMMPOLICYNOTIFICATIONMANAGER_NEWLC_ENTRY );
    
    CMsmmPolicyNotificationManager* self = 
        new (ELeave) CMsmmPolicyNotificationManager();
    CleanupStack::PushL(self);
    self->ConstructL();
    
    OstTraceFunctionExit0( REF_CMSMMPOLICYNOTIFICATIONMANAGER_NEWLC_EXIT );
    return self;
    }

void CMsmmPolicyNotificationManager::SendErrorNotificationL(
        const THostMsErrData& aErrData)
    {
    OstTraceFunctionEntry0( REF_CMSMMPOLICYNOTIFICATIONMANAGER_SENDERRORNOTIFICATIONL_ENTRY );
    

    // Print error notification data to log
    OstTrace1( TRACE_NORMAL, REF_CMSMMPOLICYNOTIFICATIONMANAGER_SENDERRORNOTIFICATIONL, 
            "Err:iError = %d", aErrData.iError );
    OstTrace1( TRACE_NORMAL, REF_CMSMMPOLICYNOTIFICATIONMANAGER_SENDERRORNOTIFICATIONL_DUP1, 
            "Err:iE32Error = %d", aErrData.iE32Error );
    OstTrace1( TRACE_NORMAL, REF_CMSMMPOLICYNOTIFICATIONMANAGER_SENDERRORNOTIFICATIONL_DUP2, 
            "Err:iDriveName = %d", aErrData.iDriveName );
    OstTraceExt1( TRACE_NORMAL, REF_CMSMMPOLICYNOTIFICATIONMANAGER_SENDERRORNOTIFICATIONL_DUP3, 
            "Err:iManufacturerString = %S", aErrData.iManufacturerString );
    OstTraceExt1( TRACE_NORMAL, REF_CMSMMPOLICYNOTIFICATIONMANAGER_SENDERRORNOTIFICATIONL_DUP4, 
            "Err:iProductString = %S", aErrData.iProductString );
            
    THostMsErrorDataPckg errPckg = aErrData;
    iErrorQueue.AppendL(errPckg);
    if (!IsActive())
    	{
    	SendNotification();
    	}
    OstTraceFunctionExit0( REF_CMSMMPOLICYNOTIFICATIONMANAGER_SENDERRORNOTIFICATIONL_EXIT );
    }

void CMsmmPolicyNotificationManager::RunL()
    {
    OstTraceFunctionEntry0( REF_CMSMMPOLICYNOTIFICATIONMANAGER_RUNL_ENTRY );
    
    iErrorQueue.Remove(0);
    if (iErrorQueue.Count() > 0)
        {
        SendNotification();
        }
    OstTraceFunctionExit0( REF_CMSMMPOLICYNOTIFICATIONMANAGER_RUNL_EXIT );
    }

void CMsmmPolicyNotificationManager::DoCancel()
    {
    OstTraceFunctionEntry0( REF_CMSMMPOLICYNOTIFICATIONMANAGER_DOCANCEL_ENTRY );
    
    iErrorQueue.Reset();
    iNotifier.CancelNotifier(KMountPolicyNotifierUid);
    OstTraceFunctionExit0( REF_CMSMMPOLICYNOTIFICATIONMANAGER_DOCANCEL_EXIT );
    }

CMsmmPolicyNotificationManager::CMsmmPolicyNotificationManager():
CActive(EPriorityStandard)
    {
    OstTraceFunctionEntry0( REF_CMSMMPOLICYNOTIFICATIONMANAGER_CMSMMPOLICYNOTIFICATIONMANAGER_ENTRY );
    
    CActiveScheduler::Add(this);
    OstTraceFunctionExit0( REF_CMSMMPOLICYNOTIFICATIONMANAGER_CMSMMPOLICYNOTIFICATIONMANAGER_EXIT );
    }

void CMsmmPolicyNotificationManager::ConstructL()
    {
    OstTraceFunctionEntry0( REF_CMSMMPOLICYNOTIFICATIONMANAGER_CONSTRUCTL_ENTRY );
    
    User::LeaveIfError(iNotifier.Connect());
    OstTraceFunctionExit0( REF_CMSMMPOLICYNOTIFICATIONMANAGER_CONSTRUCTL_EXIT );
    }

void CMsmmPolicyNotificationManager::SendNotification()
    {
    OstTraceFunctionEntry0( REF_CMSMMPOLICYNOTIFICATIONMANAGER_SENDNOTIFICATION_ENTRY );
    
    iNotifier.StartNotifierAndGetResponse(
        iStatus, KMountPolicyNotifierUid, iErrorQueue[0], iResponse);
    SetActive();
    OstTraceFunctionExit0( REF_CMSMMPOLICYNOTIFICATIONMANAGER_SENDNOTIFICATION_EXIT );
    }

// End of file
