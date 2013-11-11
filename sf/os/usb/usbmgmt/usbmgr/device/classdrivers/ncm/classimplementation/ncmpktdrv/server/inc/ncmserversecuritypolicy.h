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
 
#ifndef NCMSERVERSECURITYPOLICY_H
#define NCMSERVERSECURITYPOLICY_H

#include <e32base.h>
#include "ncmserverconsts.h"

/** NCM Server Security Policy Definition */
const TInt KNcmServerRanges[] = 
    {
    ENcmTransferHandle,             /** pass */
    ENcmNotSupport,        /** fail (to KMaxTInt) */
    };

const TUint KNcmServerRangeCount = sizeof(KNcmServerRanges) / sizeof(KNcmServerRanges[0]);

/** Index numbers into KAcmServerElements[] */
const TInt KPolicyPass = 0;

/** Mapping IPCs to policy element */
const TUint8 KNcmServerElementsIndex[KNcmServerRangeCount] = 
    {
    KPolicyPass,                  /** All (valid) APIs */
    CPolicyServer::ENotSupported,   /** remainder of possible IPCs */
    };

/** Individual policy elements */
const CPolicyServer::TPolicyElement KNcmServerElements[] = 
    {
        { 
        // These caps are those of USBSVR's process. This is the recommended 
        // way to check that an API is called by a specific process only. [The 
        // alternative would be an SID check.]
        //_INIT_SECURITY_POLICY_C5(ECapabilityCommDD, ECapabilityNetworkControl, ECapabilityNetworkServices, ECapabilityLocalServices, ECapabilityProtServ), CPolicyServer::EFailClient
        _INIT_SECURITY_POLICY_PASS
        },
    };

/** Main policy */
const CPolicyServer::TPolicy KNcmServerPolicy = 
    {
    CPolicyServer::EAlwaysPass, /** Specifies all connect attempts should pass */
    KNcmServerRangeCount,
    KNcmServerRanges,
    KNcmServerElementsIndex,
    KNcmServerElements,
    };
    
#endif // NCMSERVERSECURITYPOLICY_H
