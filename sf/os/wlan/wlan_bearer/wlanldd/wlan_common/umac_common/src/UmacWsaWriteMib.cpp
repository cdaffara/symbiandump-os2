/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of the WlanWsaWriteMib class
*
*/

/*
* %version: 13 %
*/

#include "config.h"
#include "UmacWsaWriteMib.h"
#include "UmacContextImpl.h"

#ifndef NDEBUG 
const TInt8 WlanWsaWriteMib::iName[] = "wsa-writemib";
#endif

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanWsaWriteMib::Entry( WlanContextImpl& aCtxImpl )
    {
    // mark WSA cmd active
    aCtxImpl.ActivateWsaCmd();

    OsTracePrint( KWsaCmdStateDetails, (TUint8*)
        ("UMAC: WlanWsaWriteMib::Entry: MIB ID: 0x%04x"), iMib);

    OsTracePrint( KWsaCmdStateDetails, (TUint8*)
        ("UMAC: WlanWsaWriteMib::Entry: total mib length: %d"), iLength);

    OsTracePrint( KWsaCmdStateDetails, (TUint8*)
        ("UMAC: WlanWsaWriteMib::Entry: complete mib contents:"), 
        reinterpret_cast<const TUint8*>(iData), 
        reinterpret_cast<const TUint8*>(iData) + iLength );            

    OsTracePrint( KWsaCmdState, (TUint8*)
        ("UMAC: WHA-CMD-WriteMib") );
            
    // execute the command
    // for now we will always set the aMore field to false
    const WHA::TStatus status( 
        aCtxImpl.Wha().WriteMib( iMib, iLength, iData, EFalse ) );
    
    if ( status == WHA::KSuccess )
        {
        // command was executed synchronously 
        // and no command response event is pending
        // we shall request a dfc in order to break the context

        OsTracePrint( KWsaCmdState, (TUint8*)
            ("UMAC: WlanWsaWriteMib::Entry: command executed synchronously: request dfc: status %d"), 
            status);
        DfcPending( ETrue );
        aCtxImpl.iUmac.RegisterDfc( &aCtxImpl );
        }
    else
        {
        // command is executed asynchronously 
        // and command response event is pending 
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanWsaWriteMib::Exit( WlanContextImpl& aCtxImpl)
    {
    // mark WSA cmd inactive
    aCtxImpl.DeActivateWsaCmd();

    if ( DfcPending() )
        {
        // dfc is pending and we are departing this state
        // only valid use case for taking this path is
        // that we have received an error indication

        // no we just must cancel the pending dfc so 
        // that it won't get executed in a wrong state

        OsTracePrint( KWarningLevel, (TUint8*)
            ("UMAC: WlanWsaWriteMib::Exit: state exit while dfc pending -> canceling dfc"));

        DfcPending( EFalse );
        aCtxImpl.iUmac.CancelDfc();
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanWsaWriteMib::CommandResponse( 
    WlanContextImpl& aCtxImpl, 
    WHA::TCommandId aCommandId, 
    WHA::TStatus aStatus,
    const WHA::UCommandResponseParams& aCommandResponseParams )
    {
    OsTracePrint( KWsaCmdState, (TUint8*)
        ("UMAC: WlanWsaWriteMib::CommandResponse") );

    if ( aCommandId != WHA::EWriteMIBResponse )
        {        
        OsTracePrint( KErrorLevel, 
            (TUint8*)("UMAC: aCommandId: %d"), aCommandId );
        OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
        }
    
    TraverseToHistoryState( aCtxImpl, 
        aCommandId, aStatus, aCommandResponseParams );    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanWsaWriteMib::OnDfc( TAny* aCtx )
    {
    // only valid reason for us to land here is,
    // that we have executed a synchronous MIB write

    OsTracePrint( KWsaCmdState, (TUint8*)("UMAC: WlanWsaWriteMib:OnDfc"));

    DfcPending( EFalse );   // mark dfc non pending

    WHA::UCommandResponseParams dont_care;
    CommandResponse( *(static_cast<WlanContextImpl*>(aCtx)), 
        WHA::EWriteMIBResponse, WHA::KSuccess, dont_care );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
#ifndef NDEBUG 
const TInt8* WlanWsaWriteMib::GetStateName( TUint8& aLength ) const
    {
    aLength = sizeof( iName );
    return iName;
    }
#endif
