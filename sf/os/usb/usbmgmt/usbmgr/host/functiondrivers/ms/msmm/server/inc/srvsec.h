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

#ifndef SRVSEC_H
#define SRVSEC_H

const TInt KMsmmServerRanges[] = 
    {
    EHostMsmmServerAddFunction,
    EHostMsmmServerEjectUsbDrives, //separate policy check for Eject usb drives client
    EHostMsmmServerEndMarker
    };
const TUint KMsmmServerRangeCount = 
    (sizeof(KMsmmServerRanges) / sizeof(KMsmmServerRanges[0])); 
 
const TUint8 KMsmmServerElementsIndex[KMsmmServerRangeCount] = 
    {
    0,
    1,
    CPolicyServer::ENotSupported
    };

const CPolicyServer::TPolicyElement KMsmmServerElements[] = 
    {
        {_INIT_SECURITY_POLICY_S1( KFDFWSecureId, ECapabilityCommDD ), 
            CPolicyServer::EFailClient},
        {_INIT_SECURITY_POLICY_S1( KSidHbDeviceDialogAppServer, ECapabilityTrustedUI ), 
            CPolicyServer::EFailClient}
    };

const CPolicyServer::TPolicy KMsmmServerSecurityPolicy =
    {
    CPolicyServer::ECustomCheck,
    KMsmmServerRangeCount,
    KMsmmServerRanges,
    KMsmmServerElementsIndex,
    KMsmmServerElements,
    };

#endif /*SRVSEC_H*/
