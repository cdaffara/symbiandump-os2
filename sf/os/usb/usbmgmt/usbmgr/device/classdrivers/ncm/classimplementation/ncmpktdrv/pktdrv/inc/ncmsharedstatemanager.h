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
@internalComponent
*/

#ifndef NCMSHAREDSTATEMANAGER_H
#define NCMSHAREDSTATEMANAGER_H

#include <e32base.h>
#include <e32property.h>

#include "ncmpktdrvcommon.h"
#include "ncmcoexistbearerwatcher.h"


/**
The interface to Ncm Engine.
*/
NONSHARABLE_CLASS(MShareStateObserver)
    {
public:
    virtual void NotifyDhcpStarted() = 0;
    };

using namespace UsbNcm;

/**
Maintain the shared state between the Paket Driver and NCM class controller
*/
NONSHARABLE_CLASS(CNcmSharedStateManager) : public CActive
    {
public:
    static CNcmSharedStateManager* NewL(MShareStateObserver& aEngine);
    ~CNcmSharedStateManager();

    TInt NotifyDhcpProvisionRequested();
    void NotifyNcmConnected();
    void NotifyNcmDisconnected(TInt aReason);

    TInt SetStateValue(TInt aType, TInt aValue);
    TInt RegisterNotify(const RMessage2& aMsg);
    TInt DeRegisterNotify();

private:
    CNcmSharedStateManager(MShareStateObserver& aEngine);
    void ConstructL();

    //From CActive
    void RunL();
    void DoCancel();

    TInt DoDhcpProvisionRequest();

private:
    RProperty                    iProperty;
    RMessage2                    iNotifyMsg;
    TPckgBuf<TNcmConnectionEvent> iNcmState;
    MShareStateObserver&         iEngine;
    CNcmCoexistBearerWatcher*    iCoexistBearer;
    };

#endif // NCMSHAREDSTATEMANAGER_H
