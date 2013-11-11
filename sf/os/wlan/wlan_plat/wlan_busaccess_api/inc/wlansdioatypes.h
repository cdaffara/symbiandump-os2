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

#ifndef WLANSDIOATYPES_H
#define WLANSDIOATYPES_H

#include <wlanosaplatform.h>
#include <wlansdioanamespace.h>

NAMESPACE_BEGIN_SDIOA

enum TStatus
    {
    ESuccess        = 0,
    EFailure,
    ESuccessXfer,
    EPending
    };


// Capability flags
	
// High speed mode support
const TUint32 KCapabilityHighSpeed = ( 1 << 0 );


// Request parameter flags

// If this flag is set to 1, the write operation will be done to an incrementing address
const TUint32 KRequestIncrAddress = ( 1 << 0 );


NAMESPACE_END_SDIOA

#endif // WLANSDIOATYPES_H
