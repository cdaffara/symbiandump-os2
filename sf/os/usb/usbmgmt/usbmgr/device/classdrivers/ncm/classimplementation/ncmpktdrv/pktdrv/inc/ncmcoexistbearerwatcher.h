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

#ifndef NCMCOEXISTBEARERWATCHER_H
#define NCMCOEXISTBEARERWATCHER_H

#include <e32base.h>
#include <e32property.h>

#include "ncmpktdrvcommon.h"


/**
The IPBearerCoexistence P&S key watcher.
*/
NONSHARABLE_CLASS(CNcmCoexistBearerWatcher) : public CActive
    {
public:
    static CNcmCoexistBearerWatcher* NewL();
    ~CNcmCoexistBearerWatcher();

    /**
    Set the IP Over USB active if none is actived, or wait
    @param[in]   aStatus, asychronous request status.
    @return      KErrCompletion, successfully active IP Over USB.
                 KErrNone, asychronous call, the result will be returned by aStatus later.
                 other, system-wide error code.
    */
    TInt SetIpOverUsbActive(TRequestStatus& aStatus);
    /** Deactive IP Over USB */
    TInt SetIpOverUsbDeactive();
    /** Check whether IP Over USB Bearer is actived or not. */
    inline TBool IsIpOverUsbActived() const;

private:
    //From CActive
    void RunL();
    void DoCancel();

    CNcmCoexistBearerWatcher();
    void ConstructL();
    TInt DoActive();

private:
    enum TIPBearerCoexistenceStatus
        {
        EIPBearerNoneActive  = 0,
        EIPBearerIPOverUSB   = 1,
        EIPBearerBTPan       = 2
        };

private:
    RProperty                   iProperty;
    TRequestStatus*             iReportStatus;
    TIPBearerCoexistenceStatus  iBearerState;
    };

// Inline functions
#include "ncmcoexistbearerwatcher.inl"

#endif /* NCMCOEXISTBEARERWATCHER_H */
