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
* Description:   Implementation of the WlanWsaReadMib class
*
*/

/*
* %version: 12 %
*/

#include "config.h"
#include "UmacWsaReadMib.h"
#include "UmacContextImpl.h"

#ifndef NDEBUG 
const TInt8 WlanWsaReadMib::iName[] = "wsa-readmib";
#endif

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanWsaReadMib::Entry( WlanContextImpl& aCtxImpl )
    {
    // mark WSA cmd active
    aCtxImpl.ActivateWsaCmd();

    OsTracePrint( KWsaCmdState, 
        (TUint8*)("WlanWsaReadMib::Entry: MIB ID: 0x%04x"), iMib);

    OsTracePrint( KWsaCmdState, (TUint8*)
        ("UMAC: WHA-CMD-ReadMib") );
            
    // execute the command
    aCtxImpl.Wha().ReadMib( iMib );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanWsaReadMib::CommandResponse( 
    WlanContextImpl& aCtxImpl, 
    WHA::TCommandId aCommandId, 
    WHA::TStatus aStatus,
    const WHA::UCommandResponseParams& aCommandResponseParams )
    {
    OsTracePrint( KWsaCmdState, 
        (TUint8*)("UMAC: WlanWsaReadMib::CommandResponse") );    

    if ( aCommandId == WHA::EReadMIBResponse )
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
#ifndef NDEBUG 
const TInt8* WlanWsaReadMib::GetStateName( TUint8& aLength ) const
    {
    aLength = sizeof( iName );
    return iName;
    }
#endif
