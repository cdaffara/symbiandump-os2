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
* Description:   Implementation of the WlanWsaSetPsMode class
*
*/

/*
* %version: 11 %
*/

#include "config.h"
#include "UmacWsaSetPsMode.h"
#include "UmacContextImpl.h"

#ifndef NDEBUG 
const TInt8 WlanWsaSetPsMode::iName[] = "wsa-setpsmode";
#endif

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanWsaSetPsMode::Entry( WlanContextImpl& aCtxImpl )
    {
    // mark WSA cmd active
    aCtxImpl.ActivateWsaCmd();

    OsTracePrint( KWsaCmdStateDetails, 
        (TUint8*)("WlanWsaSetPsMode::Entry: PsMode: %d"), iPsMode);

    OsTracePrint( KWsaCmdState, (TUint8*)
        ("UMAC: WHA-CMD-SetPsMode") );
            
    // execute the command
    aCtxImpl.Wha().SetPsMode( iPsMode );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanWsaSetPsMode::CommandResponse( 
    WlanContextImpl& aCtxImpl, 
    WHA::TCommandId aCommandId, 
    WHA::TStatus aStatus,
    const WHA::UCommandResponseParams& aCommandResponseParams )
    {
    OsTracePrint( KWsaCmdState, 
        (TUint8*)("UMAC: WlanWsaSetPsMode::CommandResponse") );

    if ( aCommandId == WHA::ESetPsModeCommandResponse )
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
const TInt8* WlanWsaSetPsMode::GetStateName( TUint8& aLength ) const
    {
    aLength = sizeof( iName );
    return iName;
    }
#endif
