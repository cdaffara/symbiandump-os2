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
* Description:   Implementation of the WlanWsaAddKey class
*
*/

/*
* %version: 14 %
*/

#include "config.h"
#include "UmacWsaAddKey.h"
#include "UmacContextImpl.h"

#ifndef NDEBUG 
const TInt8 WlanWsaAddKey::iName[] = "wsa-addkey";
#endif // !NDEBUG 

// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanWsaAddKey::Entry( WlanContextImpl& aCtxImpl )
    {
    // mark WSA cmd active
    aCtxImpl.ActivateWsaCmd();

    OsTracePrint( 
        KWsaCmdStateDetails, 
        (TUint8*)("UMAC: WlanWsaAddKey::Entry: KeyType: %d"), 
        iKeyType);
    OsTracePrint( 
        KWsaCmdStateDetails, 
        (TUint8*)("UMAC: WlanWsaAddKey::Entry: iEntryIndex: %d"), 
        iEntryIndex);

#ifndef NDEBUG 
    TUint keyDataLen( 0 );
    
    switch ( iKeyType )
        {
        case WHA::EWepGroupKey:
            keyDataLen = 
                sizeof(WHA::SWepGroupKey) 
                - sizeof(reinterpret_cast<const WHA::SWepGroupKey*>(iKey)->iKey) 
                + (reinterpret_cast<const WHA::SWepGroupKey*>
                (iKey))->iKeyLengthInBytes;                               
            break;
        case WHA::EWepPairWiseKey:
            keyDataLen = 
                sizeof(WHA::SWepPairwiseKey) 
                - sizeof(reinterpret_cast<const WHA::SWepPairwiseKey*>
                (iKey)->iKey) 
                + (reinterpret_cast<const WHA::SWepPairwiseKey*>
                (iKey))->iKeyLengthInBytes;
            break;            
        case WHA::ETkipGroupKey: 
            keyDataLen = sizeof( WHA::STkipGroupKey );
            break;
        case WHA::ETkipPairWiseKey:
            keyDataLen = sizeof( WHA::STkipPairwiseKey );
            break;            
        case WHA::EAesGroupKey: 
            keyDataLen = sizeof( WHA::SAesGroupKey );
            break;
        case WHA::EAesPairWiseKey:        
            keyDataLen = sizeof( WHA::SAesPairwiseKey );
            break;
        case WHA::EWapiGroupKey: 
            keyDataLen = sizeof( WHA::SWapiGroupKey );
            break;
        case WHA::EWapiPairWiseKey:        
            keyDataLen = sizeof( WHA::SWapiPairwiseKey );
            break;
        default:
            OsTracePrint( KErrorLevel, 
                (TUint8*)("UMAC: iKeyType: %d"), iKeyType );
            OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
        }

    OsTracePrint( KWsaCmdStateDetails, 
        (TUint8*)("UMAC: WlanWsaAddKey::Entry: complete key data contents:"), 
        reinterpret_cast<const TUint8*>(iKey), 
        reinterpret_cast<const TUint8*>(iKey) + keyDataLen );            

    OsTracePrint( KWsaCmdState, (TUint8*)
        ("UMAC: WHA-CMD-AddKey") );
            
#endif // !NDEBUG 

    // execute the command
    aCtxImpl.Wha().AddKey( iKeyType, iKey, iEntryIndex );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanWsaAddKey::CommandResponse( 
    WlanContextImpl& aCtxImpl, 
    WHA::TCommandId aCommandId, 
    WHA::TStatus aStatus,
    const WHA::UCommandResponseParams& aCommandResponseParams )
    {
    if ( aCommandId != WHA::EAddKeyResponse )
        {
        OsTracePrint( KErrorLevel, 
            (TUint8*)("UMAC: aCommandId: %d"), aCommandId );
        OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
        }
    
    TraverseToHistoryState( aCtxImpl, 
        aCommandId, aStatus, aCommandResponseParams );    
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
#ifndef NDEBUG
const TInt8* WlanWsaAddKey::GetStateName( TUint8& aLength ) const
    {
    aLength = sizeof( iName );
    return iName;
    }
#endif
