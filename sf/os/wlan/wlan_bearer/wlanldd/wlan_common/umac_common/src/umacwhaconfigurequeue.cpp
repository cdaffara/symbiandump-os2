/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   Implementation of the WlanWhaConfigureQueue class.
*
*/

/*
* %version: 13 %
*/

#include "config.h"
#include "umacwhaconfigurequeue.h"
#include "UmacContextImpl.h"

#ifndef NDEBUG
const TInt8 WlanWhaConfigureQueue::iName[] = "wha-configurequeue";
#endif

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanWhaConfigureQueue::Set( 
    WHA::TQueueId aQueueId,
    TUint32 aMaxLifeTime,
    WHA::TPsScheme aPsScheme,
    WHA::TAckPolicy aAckPolicy,
    TUint16 aMediumTime )
    {
    iQueueId = aQueueId;
    iMaxLifeTime = aMaxLifeTime;
    iPsScheme = aPsScheme;
    iAckPolicy = aAckPolicy;
    iMediumTime = aMediumTime;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanWhaConfigureQueue::Entry( WlanContextImpl& aCtxImpl )
    {
    // mark WSA cmd active
    aCtxImpl.ActivateWsaCmd();

    OsTracePrint( KWsaCmdStateDetails, (TUint8*)
        ("UMAC: WlanWhaConfigureQueue::Entry: iQueueId: %d"), 
        iQueueId );
    OsTracePrint( KWsaCmdStateDetails, (TUint8*)
        ("UMAC: WlanWhaConfigureQueue::Entry: iMaxLifeTime: %d"),
        iMaxLifeTime );
    OsTracePrint( KWsaCmdStateDetails, (TUint8*)
        ("UMAC: WlanWhaConfigureQueue::Entry: iPsScheme: %d"),
        iPsScheme );
    OsTracePrint( KWsaCmdStateDetails, (TUint8*)
        ("UMAC: WlanWhaConfigureQueue::Entry: iAckPolicy: %d"),
        iAckPolicy );
    OsTracePrint( KWsaCmdStateDetails, (TUint8*)
        ("UMAC: WlanWhaConfigureQueue::Entry: iMediumTime: %d"),
        iMediumTime );

    const WHA::SSAPSDConfig insignificant = { 0, 0 };

    OsTracePrint( KWsaCmdState, (TUint8*)
        ("UMAC: WHA-CMD-ConfigureQueue") );

    // execute the command
    aCtxImpl.Wha().ConfigureQueue( 
        iQueueId, 
        iMaxLifeTime, 
        iPsScheme,
        insignificant,  // deprecated
        iAckPolicy,
        iMediumTime );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
#ifndef NDEBUG 
const TInt8* WlanWhaConfigureQueue::GetStateName( TUint8& aLength ) const
    {
    aLength = sizeof( iName );
    return iName;
    }
#endif

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanWhaConfigureQueue::CommandResponse( 
    WlanContextImpl& aCtxImpl, 
    WHA::TCommandId aCommandId, 
    WHA::TStatus aStatus,
    const WHA::UCommandResponseParams& aCommandResponseParams )
    {
    if ( aCommandId == WHA::EConfigureQueueResponse )
        {
        TraverseToHistoryState( aCtxImpl, 
            aCommandId, aStatus, aCommandResponseParams );    
        }
    else
        {
        OsTracePrint( KErrorLevel, 
            (TUint8*)("UMAC: aCommandId: %d"), aCommandId );
        OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
        }
    }
