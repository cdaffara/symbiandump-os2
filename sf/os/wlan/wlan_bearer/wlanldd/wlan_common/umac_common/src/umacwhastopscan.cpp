/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of the WlanWhaStopScan class
*
*/

/*
* %version: 5 %
*/

#include "config.h"
#include "umacwhastopscan.h"
#include "UmacContextImpl.h"

#ifndef NDEBUG
const TInt8 WlanWhaStopScan::iName[] = "wha-stopscan";
#endif

// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanWhaStopScan::Entry( WlanContextImpl& aCtxImpl )
    {
    // mark WSA cmd active
    aCtxImpl.ActivateWsaCmd();

    OsTracePrint( KWsaCmdStateDetails, (TUint8*)
        ("UMAC: WlanWhaStopScan::Entry") );

    OsTracePrint( KWsaCmdState, (TUint8*)
        ("UMAC: WHA-CMD-StopScan") );

    // execute the command. 
    aCtxImpl.Wha().StopScan();
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
#ifndef NDEBUG 
const TInt8* WlanWhaStopScan::GetStateName( TUint8& aLength ) const
    {
    aLength = sizeof( iName );
    return iName;
    }
#endif

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanWhaStopScan::CommandResponse( 
    WlanContextImpl& aCtxImpl, 
    WHA::TCommandId aCommandId, 
    WHA::TStatus aStatus,
    const WHA::UCommandResponseParams& aCommandResponseParams )
    {
    if ( aCommandId == WHA::EStopScanResponse )
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
