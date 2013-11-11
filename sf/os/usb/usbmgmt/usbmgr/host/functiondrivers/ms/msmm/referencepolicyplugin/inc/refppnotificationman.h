/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef REFPPNOTIFICATIONMAN_H
#define REFPPNOTIFICATIONMAN_H

#include <e32base.h>
#include <usb/hostms/msmm_policy_def.h>
#include <usb/hostms/srverr.h>

#ifdef __OVER_DUMMYCOMPONENT__
    #include <usb/hostms/dummycomponent/dummynotifier.h>
#endif

typedef RArray<THostMsErrorDataPckg> THostMsErrDataQueue;
const TInt KMaxResponseStringLen = 16;

NONSHARABLE_CLASS (CMsmmPolicyNotificationManager) : public CActive
    {
public:
    ~CMsmmPolicyNotificationManager();
    static CMsmmPolicyNotificationManager* NewL();
    static CMsmmPolicyNotificationManager* NewLC();
    
public:
    void SendErrorNotificationL(const THostMsErrData& aErrData);
    
    // CActive implementation
    void RunL();
    void DoCancel();
    
protected:
    CMsmmPolicyNotificationManager();
    void ConstructL();
    
private:
    void SendNotification();
        
private:
    THostMsErrDataQueue iErrorQueue;
    TBuf8<16> iResponse;
#ifdef __OVER_DUMMYCOMPONENT__
    RDummyNotifier iNotifier;
#else
    RNotifier iNotifier;
#endif
    };

#endif /*REFPPNOTIFICATIONMAN_H*/
