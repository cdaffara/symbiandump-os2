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
* Description:   Implementation of WlanConfigureTxQueueParams class
*
*/

/*
* %version: 12 %
*/

#include "config.h"
#include "umacconfiguretxqueueparams.h"
#include "UmacContextImpl.h"
#include "umacwhaconfigurequeue.h"
#include "umacwhaconfigureac.h"

#ifndef NDEBUG
const TInt8 WlanConfigureTxQueueParams::iName[] 
    = "wsacomplex-configuretxqueueparams";

const TUint8 WlanConfigureTxQueueParams::iStateName
    [ESTATEMAX][KMaxStateStringLength] = 
    {
        {"EINIT"}, 
        {"ECONFIGUREBESTEFFORTQUEUE"}, 
        {"ECONFIGUREVOICEQUEUE"}, 
        {"ECONFIGUREVIDEOQUEUE"}, 
        {"ECONFIGUREBACKGROUNDQUEUE"}, 
        {"ECONFIGUREAC"}, 
        {"EFINIT"}
    };

const TUint8 WlanConfigureTxQueueParams::iEventName
    [EEVENTMAX][KMaxEventStringLength] = 
    {
        {"ESTATEENTRY"}, 
        {"ETXCOMPLETE"}
    };
#endif // !NDEBUG

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanConfigureTxQueueParams::Fsm( 
    WlanContextImpl& aCtxImpl, 
    TEvent aEvent )
    {
    OsTracePrint( KUmacDetails, 
        (TUint8*)("UMAC: WlanConfigureTxQueueParams::Fsm(): FSM EVENT") );
#ifndef NDEBUG
    OsTracePrint( 
        KUmacDetails, 
        (TUint8*)("UMAC: WlanConfigureTxQueueParams::Fsm(): event:"));
    OsTracePrint( KUmacDetails, iEventName[aEvent] );
    OsTracePrint( 
        KUmacDetails, 
        (TUint8*)("UMAC: WlanConfigureTxQueueParams::Fsm(): state:"));
    OsTracePrint( KUmacDetails, iStateName[iState] );
#endif

    switch ( aEvent )
        {
        case ESTATEENTRY:
            OnStateEntryEvent( aCtxImpl );
            break;
        case ETXCOMPLETE:
            OnTxCompleteEvent( aCtxImpl );
            break;
        default:
            // catch internal FSM programming error
#ifndef NDEBUG
            OsTracePrint( KErrorLevel, (TUint8*)("UMAC: event:"));
            OsTracePrint( KErrorLevel, iEventName[aEvent] );                
#endif 
            OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanConfigureTxQueueParams::OnStateEntryEvent( 
    WlanContextImpl& aCtxImpl )
    {
    switch ( iState )
        {
        case EINIT:
            ChangeInternalState( aCtxImpl, ECONFIGUREBESTEFFORTQUEUE );
            break;
        case ECONFIGUREBESTEFFORTQUEUE:
            ConfigureQueue( aCtxImpl, WHA::ELegacy );                
            break;
        case ECONFIGUREVOICEQUEUE:
            ConfigureQueue( aCtxImpl, WHA::EVoice );
            break;
        case ECONFIGUREVIDEOQUEUE:
            ConfigureQueue( aCtxImpl, WHA::EVideo );
            break;
        case ECONFIGUREBACKGROUNDQUEUE:
            ConfigureQueue( aCtxImpl, WHA::EBackGround );
            break;
        case ECONFIGUREAC:
            ConfigureAc( aCtxImpl );
            break;
        case EFINIT:
            // fsm execution complete traverse back to history state
            TraverseToHistoryState( aCtxImpl );
            break;
        default:
            // cath internal FSM programming error
#ifndef NDEBUG
            OsTracePrint( KErrorLevel, (TUint8*)("UMAC: state:"));
            OsTracePrint( KErrorLevel, iStateName[iState] );
#endif 
            OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanConfigureTxQueueParams::OnTxCompleteEvent( 
    WlanContextImpl& aCtxImpl )
    {
    switch ( iState )
        {
        case ECONFIGUREBESTEFFORTQUEUE:
            // continue state traversal
            ChangeInternalState( aCtxImpl, ECONFIGUREVOICEQUEUE );
            break;
        case ECONFIGUREVOICEQUEUE:
            // continue state traversal
            ChangeInternalState( aCtxImpl, ECONFIGUREVIDEOQUEUE );
            break;
        case ECONFIGUREVIDEOQUEUE:
            // continue state traversal
            ChangeInternalState( aCtxImpl, ECONFIGUREBACKGROUNDQUEUE );
            break;
        case ECONFIGUREBACKGROUNDQUEUE:
            // continue state traversal
            ChangeInternalState( aCtxImpl, ECONFIGUREAC );
            break;
        case ECONFIGUREAC:
            // continue state traversal
            ChangeInternalState( aCtxImpl, EFINIT );
            break;
        default:
            // cath internal FSM programming error
#ifndef NDEBUG
            OsTracePrint( KErrorLevel, (TUint8*)("UMAC: state:"));
            OsTracePrint( KErrorLevel, iStateName[iState] );
#endif             
            OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanConfigureTxQueueParams::ChangeInternalState( 
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
void WlanConfigureTxQueueParams::ConfigureQueue( 
    WlanContextImpl& aCtxImpl,
    WHA::TQueueId aQueueId )
    {
    OsTracePrint( 
        KUmacDetails, 
        (TUint8*)("UMAC: WlanConfigureTxQueueParams::ConfigureQueue") );

    WHA::TPsScheme psScheme( WHA::ERegularPs );
    
    // enable U-APSD for the AC/Queue in question if necessary. 
    // Otherwise stick to regular PS
    switch ( aQueueId )
        {
        case WHA::ELegacy:
            if ( aCtxImpl.UapsdUsedForBestEffort() )
                {
                psScheme = WHA::EUapsd;
                }                
            break;
        case WHA::EBackGround:
            if ( aCtxImpl.UapsdUsedForBackground() )
                {
                psScheme = WHA::EUapsd;
                }
            break;
        case WHA::EVideo:
            if ( aCtxImpl.UapsdUsedForVideo() )
                {
                psScheme = WHA::EUapsd;
                }
            break;
        case WHA::EVoice:
            if ( aCtxImpl.UapsdUsedForVoice() )
                {
                psScheme = WHA::EUapsd;
                }
            break;
        default:
            // catch programming error
            OsTracePrint( KErrorLevel, (TUint8*)
                ("UMAC: ERROR: non-supported queue, aQueueId: %d"), aQueueId );
            OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
        }

    WlanWhaConfigureQueue& wha_command = aCtxImpl.WhaConfigureQueue();

    wha_command.Set( 
        aQueueId,
        aCtxImpl.iWlanMib.dot11MaxTransmitMSDULifetime[aQueueId], 
        psScheme, 
        WHA::ENormal,
        aCtxImpl.iWlanMib.iMediumTime[aQueueId] );

    // change global state: entry procedure triggers action
    ChangeState( aCtxImpl, 
        *this,              // prev state
        wha_command         // next state
        );                       
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanConfigureTxQueueParams::ConfigureAc( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( 
        KUmacDetails, 
        (TUint8*)("UMAC: WlanConfigureTxQueueParams::ConfigureAc()") );

    WlanWhaConfigureAc& wha_command = aCtxImpl.WhaConfigureAc();

    wha_command.Set( 
        aCtxImpl.CwMinVector(),
        aCtxImpl.CwMaxVector(),
        aCtxImpl.AifsVector(),
        aCtxImpl.TxOplimitVector() );

    // change global state: entry procedure triggers action
    ChangeState( aCtxImpl, 
        *this,              // prev state
        wha_command         // next state
        );                       
    }
    
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanConfigureTxQueueParams::Entry( 
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
void WlanConfigureTxQueueParams::Exit( 
    WlanContextImpl& /*aCtxImpl*/ )
    {
    iState = EINIT;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
#ifndef NDEBUG 
const TInt8* WlanConfigureTxQueueParams::GetStateName( 
    TUint8& aLength ) const
    {
    aLength = sizeof( iName );
    return iName;
    }
#endif
