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
* Description:   Implementation of the WlanWsaInitiliaze class
*
*/

/*
* %version: 13 %
*/

#include "config.h"
#include "UmacWsaInitiliaze.h"
#include "UmacContextImpl.h"

#ifndef NDEBUG 
const TInt8 WlanWsaInitiliaze::iName[] = "wha-initiliaze";
#endif

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanWsaInitiliaze::Entry( WlanContextImpl& aCtxImpl )
    {
    // mark WSA cmd active
    aCtxImpl.ActivateWsaCmd();

    OsTracePrint( KWsaCmdStateDetails, (TUint8*)
        ("UMAC: WlanWsaInitiliaze::Entry: firmware size: %d"), iLength);
    OsTracePrint( KWsaCmdStateDetails, (TUint8*)
        ("UMAC: WlanWsaInitiliaze::Entry: firmware start address 0x%08x"), 
        reinterpret_cast<TUint32>(iData));        

    OsTracePrint( KWsaCmdState, (TUint8*)
        ("UMAC: WHA-CMD-Initialize") );
            
    aCtxImpl.Wha().Initialize( iData, iLength );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanWsaInitiliaze::CommandResponse( 
    WlanContextImpl& aCtxImpl, 
    WHA::TCommandId aCommandId, 
    WHA::TStatus aStatus,
    const WHA::UCommandResponseParams& aCommandResponseParams )
    {
    if ( aCommandId == WHA::EInitializeResponse )
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

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanWsaInitiliaze::Set( 
    WlanContextImpl& /*aCtxImpl*/, 
    const TAny* aData, 
    TUint32 aLength )
    {
    OsTracePrint( KWsaCmdState, (TUint8*)
        ("UMAC: WlanWsaInitiliaze::Set: user space fw address: 0x%08x"), 
        reinterpret_cast<TUint32>(aData) );    
    OsTracePrint( KWsaCmdState, (TUint8*)
        ("UMAC: WlanWsaInitiliaze::Set: length of fw: %d"), aLength );

    iData = aData;
    iLength = aLength;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
#ifndef NDEBUG 
const TInt8* WlanWsaInitiliaze::GetStateName( TUint8& aLength ) const
    {
    aLength = sizeof( iName );
    return iName;
    }
#endif
