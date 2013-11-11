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
* Description:   Implementation of WlanAddBroadcastWepKey class
*
*/

/*
* %version: 6 %
*/

#include "config.h"
#include "umacaddbroadcastwepkey.h"
#include "UmacContextImpl.h"
#include "UmacWsaAddKey.h"
#include "UmacWsaWriteMib.h"
#include "UmacWsaKeyIndexMapper.h"

#ifndef NDEBUG
const TInt8 WlanAddBroadcastWepKey::iName[] 
    = "wsacomplex-addbroadcastwepkey";

const TUint8 WlanAddBroadcastWepKey::iStateName
    [ESTATEMAX][KMaxStateStringLength] = 
    {
        {"EINIT"}, 
        {"EADDGROUPKEY"}, 
        {"EWRITEMIB"},
        {"EADDPAIRWISEKEY"},
        {"EFINIT"}
    };

const TUint8 WlanAddBroadcastWepKey::iEventName
    [EEVENTMAX][KMaxEventStringLength] = 
    {
        {"ESTATEENTRY"}, {"ETXCOMPLETE"}, {"EABORT"}
    };
#endif // !NDEBUG

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanAddBroadcastWepKey::Set( 
    const TMacAddress& aMac,
    TUint32 aKeyIndex,             
    TBool aUseAsDefaulKey,                
    TBool aUseAsPairwiseKey,
    TUint32 aKeyLength,                      
    const TUint8 aKey[KMaxWEPKeyLength] )
    {
    ((aUseAsPairwiseKey) ? (iFlags |= KUseAsPairwiseKey) 
        : (iFlags &= ~KUseAsPairwiseKey));
    ((aUseAsDefaulKey) ? (iFlags |= KUseAsDefaultKey) 
        : (iFlags &= ~KUseAsDefaultKey));

    iMacAddr = aMac;
    iKeyIndex = aKeyIndex;
    iKeyLength = aKeyLength;                       
    iKey = aKey;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanAddBroadcastWepKey::Entry( 
    WlanContextImpl& aCtxImpl )
    {
    if ( aCtxImpl.WsaCmdActive() )
        {
        // sanity checking code
        OsAssert( (TUint8*)("UMAC * panic"), (TUint8*)(WLAN_FILE), __LINE__ );
        }

    if ( iState != EINIT )
        {
        // this is NOT the start of the the FSM actions
        // note that we send the ETXCOMPLETE event as the states
        // that wait for it are the only ones that can be interrupted
        // as they are asynchronous operations by nature
        // and wait for corresponding WHA completion method
        Fsm( aCtxImpl, ETXCOMPLETE );
        }
    else
        {
        // this is the start of the the FSM actions
        Fsm( aCtxImpl, ESTATEENTRY );
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanAddBroadcastWepKey::Exit( 
    WlanContextImpl& aCtxImpl)
    {
    iState = EINIT;
    // make sure we don't leak memory
    // don't worry about the key material as the memory associated
    // to it is released at command completion time
    os_free( iMemory );
    iMemory = NULL;

    // complete client request
    Dot11History().AddDefaultBroadcastWepKeyComplete( aCtxImpl );
    }

#ifndef NDEBUG 
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
const TInt8* WlanAddBroadcastWepKey::GetStateName( 
    TUint8& aLength ) const
    {
    aLength = sizeof( iName );
    return iName;
    }
#endif

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanAddBroadcastWepKey::ChangeInternalState( 
    WlanContextImpl& aCtxImpl, 
    TState aNewState )
    {
    iState = aNewState;
    Fsm( aCtxImpl, ESTATEENTRY );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanAddBroadcastWepKey::Fsm( 
    WlanContextImpl& aCtxImpl, 
    TEvent aEvent )
    {
    OsTracePrint( KUmacDetails, 
        (TUint8*)("UMAC: WlanAddBroadcastWepKey::Fsm: FSM EVENT") );
#ifndef NDEBUG
    OsTracePrint( KUmacDetails, (TUint8*)("UMAC: event:"));
    OsTracePrint( KUmacDetails, iEventName[aEvent] );
    OsTracePrint( KUmacDetails, (TUint8*)("UMAC: state:"));
    OsTracePrint( KUmacDetails, iStateName[iState] );
#endif // !NDEBUG

    switch ( aEvent )
        {
        case ESTATEENTRY:
            OnStateEntryEvent( aCtxImpl );
            break;
        case ETXCOMPLETE:
            OnTxCompleteEvent( aCtxImpl );
            break;
        case EABORT:
            OnAbortEvent( aCtxImpl );
            break;
        default:
            // catch internal FSM programming error
#ifndef NDEBUG
            OsTracePrint( KErrorLevel, (TUint8*)("UMAC: event:"));
            OsTracePrint( KErrorLevel, iEventName[aEvent] );                
#endif // !NDEBUG
            OsAssert( (TUint8*)("UMAC: panic"), 
                (TUint8*)(WLAN_FILE), __LINE__ );
            break;
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanAddBroadcastWepKey::OnStateEntryEvent( 
    WlanContextImpl& aCtxImpl )
    {
    const TUint KAllocLen( 100 ); 

    switch ( iState )
        {
        case EINIT:
            iMemory = os_alloc( KAllocLen );
            if ( iMemory )
                {
                // start the FSM traversal
                ChangeInternalState( aCtxImpl, 
                    EADDGROUPKEY );            
                }
            else
                {
                // allocation failure
                Fsm( aCtxImpl, EABORT );
                }
            break;
        case EADDGROUPKEY:
            AddGroupKey( aCtxImpl );
            break;
        case EWRITEMIB:
            WriteMib( aCtxImpl );
            break;
        case EADDPAIRWISEKEY:
            AddPairwiseKey( aCtxImpl );
            break;
        case EFINIT:
            // fsm execution complete traverse back to history state
            // after signalling completion in the Exit method
            TraverseToHistoryState( aCtxImpl );
            break;
        default:
            // catch internal FSM programming error
#ifndef NDEBUG
            OsTracePrint( KErrorLevel, (TUint8*)("UMAC: state:"));
            OsTracePrint( KErrorLevel, iStateName[iState] );
#endif // !NDEBUG
            OsAssert( (TUint8*)("UMAC: panic"), 
                (TUint8*)(WLAN_FILE), __LINE__ );
            break;
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanAddBroadcastWepKey::OnTxCompleteEvent( 
    WlanContextImpl& aCtxImpl )
    {
    switch ( iState )
        {
        case EADDGROUPKEY:
            if ( UseAsDefaultKey() )
                {
                ChangeInternalState( aCtxImpl, EWRITEMIB );
                }
            else if ( UseAsPairwiseKey() )
                {
                ChangeInternalState( aCtxImpl, EADDPAIRWISEKEY );
                }
            else
                {
                // not to be used as default key or PTK
                // we can end this fsm
                ChangeInternalState( aCtxImpl, EFINIT );
                }
            break;
        case EWRITEMIB:
            if ( UseAsPairwiseKey() )
                {
                ChangeInternalState( aCtxImpl, EADDPAIRWISEKEY );
                }
            else
                {
                // not to be used as PTK
                // we can end this fsm
                ChangeInternalState( aCtxImpl, EFINIT );
                }
            break;
        case EADDPAIRWISEKEY:
            ChangeInternalState( aCtxImpl, EFINIT );
            break;
        default:
            // catch internal FSM programming error
#ifndef NDEBUG
            OsTracePrint( KErrorLevel, (TUint8*)("UMAC: state:"));
            OsTracePrint( KErrorLevel, iStateName[iState] );
#endif // !NDEBUG
            OsAssert( (TUint8*)("UMAC: panic"), 
                (TUint8*)(WLAN_FILE), __LINE__ );
            break;
        }
    }

// -----------------------------------------------------------------------------
// WlanAddBroadcastWepKey::OnAbortEvent
// simulate macnotresponding error
// -----------------------------------------------------------------------------
//
void WlanAddBroadcastWepKey::OnAbortEvent( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( KWarningLevel, 
        (TUint8*)("UMAC: WlanAddBroadcastWepKey::OnAbortEvent") );

    DoErrorIndication( aCtxImpl, WHA::KErrorMacNotResponding );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanAddBroadcastWepKey::AddGroupKey( 
    WlanContextImpl& aCtxImpl )
    {
    // store info of default WEP GTK insertion
    aCtxImpl.GroupKeyType( WHA::EWepGroupKey );

    WHA::SWepGroupKey* key_cnxt = static_cast<WHA::SWepGroupKey*>(iMemory);

    key_cnxt->iKeyId = static_cast<WHA::TPrivacyKeyId>(iKeyIndex);
    key_cnxt->iKeyLengthInBytes = iKeyLength; 
    os_memcpy( key_cnxt->iKey, iKey, key_cnxt->iKeyLengthInBytes );

    WlanWsaAddKey& wha_cmd( aCtxImpl.WsaAddKey() );
    wha_cmd.Set( aCtxImpl, 
            WHA::EWepGroupKey,
            key_cnxt,
            WlanWsaKeyIndexMapper::Extract( 
            WHA::EWepGroupKey, key_cnxt->iKeyId ) 
            );
    
    // change global state: entry procedure triggers action
    ChangeState( aCtxImpl, 
        *this,              // prev state
        wha_cmd             // next state
        );                           
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanAddBroadcastWepKey::AddPairwiseKey( 
    WlanContextImpl& aCtxImpl )
    {
    WHA::SWepPairwiseKey* key_cnxt( 
        static_cast<WHA::SWepPairwiseKey*>(iMemory) );

    os_memcpy( 
        &(key_cnxt->iMacAddr), 
        iMacAddr.iMacAddress,
        sizeof(key_cnxt->iMacAddr) );
    key_cnxt->iKeyLengthInBytes = iKeyLength; 
    os_memcpy( key_cnxt->iKey, iKey, key_cnxt->iKeyLengthInBytes );
    
    WlanWsaAddKey& wha_cmd( aCtxImpl.WsaAddKey() );    
    wha_cmd.Set( aCtxImpl, 
        WHA::EWepPairWiseKey,
        key_cnxt,
        WlanWsaKeyIndexMapper::Extract( WHA::EWepPairWiseKey ) );

    // change global state: entry procedure triggers action
    ChangeState( aCtxImpl, 
        *this,              // prev state
        wha_cmd             // next state
        );                           
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanAddBroadcastWepKey::WriteMib( 
    WlanContextImpl& aCtxImpl )
    {        
    // allocate memory for the mib to write
    WHA::Sdot11WepDefaultKeyId* mib( 
        static_cast<WHA::Sdot11WepDefaultKeyId*>(iMemory) ); 
   
    mib->iDot11WepDefaultKeyId = static_cast<WHA::TPrivacyKeyId>(iKeyIndex);
        
    WlanWsaWriteMib& wha_cmd = aCtxImpl.WsaWriteMib();
        
    wha_cmd.Set( 
        aCtxImpl, WHA::KMibDot11WepDefaultKeyId, sizeof(*mib), mib );
        
    // change global state: entry procedure triggers action
    ChangeState( aCtxImpl, 
        *this,              // prev state
        wha_cmd             // next state
        );       
    }
