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


#include "ncmserver.h"
#include "ncmsession.h"
#include "ncmserversecuritypolicy.h"
#include "ncmserverconsts.h"


CNcmServer* CNcmServer::NewL(CNcmEngine& aEngine)
    {
    CNcmServer* self = new(ELeave) CNcmServer(aEngine);
    CleanupStack::PushL(self);
    TInt err = self->Start(KNcmServerName);
    // KErrAlreadyExists is a success case (c.f. transient server boilerplate
    // code).
    if ( err != KErrAlreadyExists )
        {
        User::LeaveIfError(err);
        }
    CleanupStack::Pop(self);
    return self;
    }

CNcmServer::~CNcmServer()
    {
    }

CNcmServer::CNcmServer(CNcmEngine& aEngine)
 :  CPolicyServer(CActive::EPriorityStandard, KNcmServerPolicy, ESharableSessions),
    iNcmEngine(aEngine)
    {
    }

CSession2* CNcmServer::NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const
    {
    //Validate session as coming from UsbSvr
    static _LIT_SECURITY_POLICY_S0(KSidPolicy, 0x101fe1db);
    TBool auth = KSidPolicy.CheckPolicy(aMessage);
    if(!auth)
        {
        User::Leave(KErrPermissionDenied);
        }

    // Version number check...
    TVersion v( KNcmSrvMajorVersionNumber,
                KNcmSrvMinorVersionNumber,
                KNcmSrvBuildNumber);

    if ( !User::QueryVersionSupported(v, aVersion) )
        {
        User::Leave(KErrNotSupported);
        }

    CNcmSession* sess = CNcmSession::NewL(iNcmEngine);
    return sess;
    }
