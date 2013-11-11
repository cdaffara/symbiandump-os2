/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of the WlanWsaDisconnect class
*
*/

/*
* %version: 11 %
*/

#include "config.h"
#include "UmacWsaDisconnect.h"
#include "UmacContextImpl.h"

#ifndef NDEBUG 
const TInt8 WlanWsaDisconnect::iName[] = "wsa-disconnect";
#endif

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanWsaDisconnect::Entry( WlanContextImpl& aCtxImpl )
    {
    // mark WSA cmd active
    aCtxImpl.ActivateWsaCmd();

    OsTracePrint( KWsaCmdStateDetails, 
        (TUint8*)("UMAC: WlanWsaDisconnect::Entry"));

    OsTracePrint( KWsaCmdState, (TUint8*)
        ("UMAC: WHA-CMD-Reset") );
            
    // execute the disconnect command
    aCtxImpl.Wha().Reset();        
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanWsaDisconnect::CommandResponse( 
    WlanContextImpl& aCtxImpl, 
    WHA::TCommandId aCommandId, 
    WHA::TStatus aStatus,
    const WHA::UCommandResponseParams& aCommandResponseParams )
    {
    if ( aCommandId == WHA::EResetResponse )
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

#ifndef NDEBUG 
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
const TInt8* WlanWsaDisconnect::GetStateName( TUint8& aLength ) const
    {
    aLength = sizeof( iName );
    return iName;
    }
#endif
