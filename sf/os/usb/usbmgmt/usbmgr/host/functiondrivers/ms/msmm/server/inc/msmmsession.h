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

#ifndef CMSMMSESSION_H
#define CMSMMSESSION_H

#include <e32base.h>
#include <e32msgqueue.h>
#include "msmm_internal_def.h"

class CMsmmServer;
class CMsmmEngine;
class CDeviceEventQueue;
class THostMsErrData;

NONSHARABLE_CLASS(CMsmmSession) : public CSession2
    {
public: // Construction and Destruction
    ~CMsmmSession();
    static CMsmmSession* NewL(CMsmmServer& aServer, 
            CDeviceEventQueue& anEventQueue);

public: // CMsmmSession
    void ServiceL (const RMessage2& aMessage);
    void ServiceError(const RMessage2& aMessage, TInt aError);

private:
    CMsmmSession(CMsmmServer& aServer,
            CDeviceEventQueue& anEventQueue);
    void ConstructL();

    void AddUsbMsInterfaceL(const RMessage2& aMessage);
    void RemoveUsbMsDeviceL(const RMessage2& aMessage);

private: // data members
    CMsmmServer& iServer;
    CMsmmEngine& iEngine;
    CDeviceEventQueue& iEventQueue;
    TPckgBuf<TUSBMSDeviceDescription> iDevicePkg;
    THostMsErrData* iErrData; // The data nodes try, Owned
    TInt iInterfaceNumber;
    TInt32 iInterfaceToken;
    TInt iDeviceID;
    };

#endif /*CMSMMSESSION_H*/
