/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of the WlanWhaRelease class
*
*/

/*
* %version: 9 %
*/

#include "config.h"
#include "umacwharelease.h"
#include "UmacContextImpl.h"

#ifndef NDEBUG
const TInt8 WlanWhaRelease::iName[] = "wha-release";
#endif

// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanWhaRelease::Entry( WlanContextImpl& aCtxImpl )
    {
    // mark WSA cmd active
    aCtxImpl.ActivateWsaCmd();

    OsTracePrint( KWsaCmdStateDetails, 
        (TUint8*)("UMAC: WlanWhaRelease::Entry") );

    OsTracePrint( KWsaCmdState, (TUint8*)
        ("UMAC: WHA-CMD-Release") );

    // execute the command. As we always ask for synchronous execution for
    // this command (which means that the WHA layer will not call 
    // CommandResponse()), we need to call CommandResponse() method by 
    // ourselves

    const WHA::TStatus status ( aCtxImpl.Wha().Release( ETrue ) );

    WHA::UCommandResponseParams notRelevant;
    CommandResponse( aCtxImpl, WHA::EReleaseResponse, status, notRelevant );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
#ifndef NDEBUG 
const TInt8* WlanWhaRelease::GetStateName( TUint8& aLength ) const
    {
    aLength = sizeof( iName );
    return iName;
    }
#endif

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanWhaRelease::CommandResponse( 
    WlanContextImpl& aCtxImpl, 
    WHA::TCommandId aCommandId, 
    WHA::TStatus aStatus,
    const WHA::UCommandResponseParams& aCommandResponseParams )
    {
    if ( aCommandId == WHA::EReleaseResponse )
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
