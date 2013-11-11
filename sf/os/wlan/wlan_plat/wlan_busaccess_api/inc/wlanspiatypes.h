/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  file holds SPIA specific type definitions
*
*/

/*
* %version: 5 %
*/

#ifndef WLANSPIATYPES_H
#define WLANSPIATYPES_H

#include <wlanosaplatform.h>
#include <wlanspianamespace.h>

NAMESPACE_BEGIN_SPIA

enum TStatus
    {
    ESuccess        = 0,
    EFailure,
    ESuccessXfer,
    EPending
    };
    
const TUint32 KCapabilityMemoryConfig       = ( 1 << 0 );
const TUint32 KCapabilityNormalRead         = ( 1 << 1 );
const TUint32 KCapabilityWriteB4Read        = ( 1 << 2 );
const TUint32 KCapabilityCombinedWriteRead  = ( 1 << 3 );
const TUint32 KCapabilityForcedActive       = ( 1 << 4 );
const TUint32 KCapabilityFrameControlled    = ( 1 << 5 );

NAMESPACE_END_SPIA

#endif // WLANSPIACLIENT_H
