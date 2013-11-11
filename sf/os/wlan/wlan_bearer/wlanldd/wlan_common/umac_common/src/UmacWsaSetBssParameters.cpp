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
* Description:   Implementation of the WlanWsaSetBssParameters class
*
*/

/*
* %version: 12 %
*/

#include "config.h"
#include "UmacWsaSetBssParameters.h"
#include "UmacContextImpl.h"

#ifndef NDEBUG 
const TInt8 WlanWsaSetBssParameters::iName[] = "wsa-setbssparameters";
#endif

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanWsaSetBssParameters::Entry( WlanContextImpl& aCtxImpl )
    {
    // mark WSA cmd active
    aCtxImpl.ActivateWsaCmd();

    OsTracePrint( KWsaCmdStateDetails, 
        (TUint8*)("UMAC: WlanWsaSetBssParameters::Entry") );
    OsTracePrint( KWsaCmdStateDetails, (TUint8*)
        ("UMAC: WlanWsaSetBssParameters::Entry: iDtim: %d"), iDtim );
    OsTracePrint( KWsaCmdStateDetails, (TUint8*)
        ("UMAC: WlanWsaSetBssParameters::Entry: iAid: %d"),  iAid );

    OsTracePrint( KWsaCmdState, (TUint8*)
        ("UMAC: WHA-CMD-SetBssParameters") );
            
    // execute the command
    aCtxImpl.Wha().SetBssParameters( iDtim, iAid );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanWsaSetBssParameters::Set( 
    WlanContextImpl& /*aCtxImpl*/,
    TUint32 aDtim,
    TUint32 aAid )
    {
    iDtim = aDtim;
    iAid = aAid;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanWsaSetBssParameters::CommandResponse( 
    WlanContextImpl& aCtxImpl, 
    WHA::TCommandId aCommandId, 
    WHA::TStatus aStatus,
    const WHA::UCommandResponseParams& aCommandResponseParams )
    {
    OsTracePrint( KWsaCmdState, 
        (TUint8*)("UMAC: WlanWsaSetBssParameters::CommandResponse") );

    if ( aCommandId == WHA::ESetBssParametersResponse )
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
const TInt8* WlanWsaSetBssParameters::GetStateName( TUint8& aLength ) const
    {
    aLength = sizeof( iName );
    return iName;
    }
#endif
