/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Bluetooth serial engine panic codes.
*
*/


#ifndef __BTSERIALENGINE_PAN__
#define __BTSERIALENGINE_PAN__

#include <e32def.h>
#include <e32std.h>

/** Panic Categories */
_LIT( KPanicSerialEngine, "BTSerialEngine" ); // All engine classes
_LIT( KPanicBTServiceAdvertiser, "BTServiceAdvertiser" ); // only service advertiser


enum TBTSerialEnginePanics
    {
    EBTPointToPointReceiverInvalidState = 1,
    EBTPointToPointSenderExists = 2,
    EBTPointToPointReceiverExists = 3,
    EBTPointToPointSenderInvalidState = 4,
    EBTPointToPointNoSender = 5,
    EBTPointToPointAddMessage = 6,
    EBTPointToPointNextRecordRequestComplete = 7,
    EBTPointToPointAttributeRequestResult = 8,
    EBTPointToPointAttributeRequestComplete = 9,
    EBTPointToPointProtocolRead = 10,
    EBTPointToPointAttributeRequest = 11,
    EBTPointToPointSdpRecordDelete = 12,
    EBTPointToPointServerStop = 13,
    EBTPointToPointInvalidLogicState = 14,
    EBTPointToPointUnableToDisconnect = 15,

    EBTSerialEngineReadSocketBadState = 16,
    EBTSerialEngineWriteSocketBadState = 17
    };

inline void Panic( TBTSerialEnginePanics aReason )
    {
    User::Panic( KPanicSerialEngine, aReason );
    }


#endif // __BTSERIALENGINE_PAN__

// End of File
