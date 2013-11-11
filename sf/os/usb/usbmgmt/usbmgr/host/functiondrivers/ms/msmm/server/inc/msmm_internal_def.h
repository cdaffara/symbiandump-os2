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

#ifndef MSMM_INTERNAL_DEF_H
#define MSMM_INTERNAL_DEF_H

#include "msmm_pub_def.h"
#include "msmmnodebase.h"

// MSMM server internal device event type
enum TDeviceEventType
    {
    EDeviceEventAddFunction = 1,
    EDeviceEventRemoveDevice = 2,
    EDeviceEventEndMark
    };

// MSMM server internal device event
NONSHARABLE_CLASS(TDeviceEvent)
    {
public:
    TDeviceEvent(TDeviceEventType aEvent = EDeviceEventEndMark, 
            TUint aDeviceId = 0, TUint8 aInterfaceNumber = 0, 
            TUint32 aInterfaceToken = 0):
iEvent(aEvent),
iDeviceId(aDeviceId),
iInterfaceNumber(aInterfaceNumber),
iInterfaceToken(aInterfaceToken)
        {
        }
    
public:
    TDeviceEventType iEvent; // Event type
    TUint iDeviceId; // Related device identifier
    TUint8 iInterfaceNumber; // Related interface number
    TUint32 iInterfaceToken; // Related interface token
    };


// Information needed by file system extension
// They may changed before the final release available from base team 
// frequently.

// FAT file system name
_LIT(KFATFSNAME, "FAT");

// ELocal file system name
_LIT(KELOCALFSNAME, "ELOCAL");

// Proxy drive name
_LIT(KPROXYDRIVENAME, "usbhostms");

// File system extention name
_LIT(KFSEXTNAME, "usbhostms.pxy");


#endif /*MSMM_INTERNAL_DEF_H*/
