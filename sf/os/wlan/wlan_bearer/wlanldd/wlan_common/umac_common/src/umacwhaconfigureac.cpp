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
* Description:   Implementation of the WlanWhaConfigureAc class
*
*/

/*
* %version: 13 %
*/

#include "config.h"
#include "umacwhaconfigureac.h"
#include "wha_mibDefaultvalues.h"

#ifndef NDEBUG
const TInt8 WlanWhaConfigureAc::iName[] = "wha-configureac";
#endif

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
WlanWhaConfigureAc::WlanWhaConfigureAc()
    {
    os_memset( iCwMin, 0, sizeof( iCwMin ) );    
    os_memset( iCwMax, 0, sizeof( iCwMax ) );    
    os_memset( iAIFS, 0, sizeof( iAIFS ) );    
    os_memset( iTxOplimit, 0, sizeof( iTxOplimit ) );    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanWhaConfigureAc::Set( 
    const TCwMinVector& aCwMin,
    const TCwMaxVector& aCwMax,
    const TAifsVector& aAIFS,
    const TTxOplimitVector& aTxOplimit )
    {
    os_memcpy( iCwMin, aCwMin, sizeof( iCwMin ) );    
    os_memcpy( iCwMax, aCwMax, sizeof( iCwMax ) );    
    os_memcpy( iAIFS, aAIFS, sizeof( iAIFS ) );    
    os_memcpy( iTxOplimit, aTxOplimit, sizeof( iTxOplimit ) );    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanWhaConfigureAc::Entry( WlanContextImpl& aCtxImpl )
    {
    // mark WSA cmd active
    aCtxImpl.ActivateWsaCmd();

    OsTracePrint( KWsaCmdStateDetails, 
        (TUint8*)("UMAC: WlanWhaConfigureAc::Entry") );

    // currently we always use the default value for maxReceiveLifeTime
    //
    TUint16 maxReceiveLifeTime[WHA::Wha::KNumOfEdcaQueues] =
        { WHA::KDot11MaxReceiveLifeTimeDefault,
          WHA::KDot11MaxReceiveLifeTimeDefault,
          WHA::KDot11MaxReceiveLifeTimeDefault,
          WHA::KDot11MaxReceiveLifeTimeDefault };
    
    OsTracePrint( KWsaCmdState, (TUint8*)
        ("UMAC: WHA-CMD-ConfigureAC") );

#ifndef NDEBUG 
    for ( TUint i = 0; i < WHA::Wha::KNumOfEdcaQueues; ++i )
        {
        OsTracePrint( KWsaCmdStateDetails, (TUint8*)
            ("UMAC: WlanWhaConfigureAc::Entry: iCwMin: %d"),
            iCwMin[i] );        
        }
    for ( TUint i = 0; i < WHA::Wha::KNumOfEdcaQueues; ++i )
        {
        OsTracePrint( KWsaCmdStateDetails, (TUint8*)
            ("UMAC: WlanWhaConfigureAc::Entry: iCwMax: %d"),
            iCwMax[i] );        
        }
    for ( TUint i = 0; i < WHA::Wha::KNumOfEdcaQueues; ++i )
        {
        OsTracePrint( KWsaCmdStateDetails, (TUint8*)
            ("UMAC: WlanWhaConfigureAc::Entry: iAIFS: %d"),
            iAIFS[i] );        
        }
    for ( TUint i = 0; i < WHA::Wha::KNumOfEdcaQueues; ++i )
        {
        OsTracePrint( KWsaCmdStateDetails, (TUint8*)
            ("UMAC: WlanWhaConfigureAc::Entry: iTxOplimit: %d"),
            iTxOplimit[i] );        
        }
    for ( TUint i = 0; i < WHA::Wha::KNumOfEdcaQueues; ++i )
        {
        OsTracePrint( KWsaCmdStateDetails, (TUint8*)
            ("UMAC: WlanWhaConfigureAc::Entry: maxReceiveLifeTime: %d"),
            maxReceiveLifeTime[i] );        
        }
#endif

    // execute the command
    aCtxImpl.Wha().ConfigureAC( 
        iCwMin, 
        iCwMax, 
        iAIFS,
        iTxOplimit,  
        maxReceiveLifeTime );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
#ifndef NDEBUG 
const TInt8* WlanWhaConfigureAc::GetStateName( TUint8& aLength ) const
    {
    aLength = sizeof( iName );
    return iName;
    }
#endif

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanWhaConfigureAc::CommandResponse( 
    WlanContextImpl& aCtxImpl, 
    WHA::TCommandId aCommandId, 
    WHA::TStatus aStatus,
    const WHA::UCommandResponseParams& aCommandResponseParams )
    {
    if ( aCommandId == WHA::EConfigureACResponse )
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
