/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of WlanConfigureTxAutoRatePolicy class
*
*/

/*
* %version: 6 %
*/

#include "config.h"
#include "umacconfiguretxautoratepolicy.h"
#include "UmacContextImpl.h"
#include "UmacWsaWriteMib.h"

#ifndef RD_WLAN_DDK
#include <wha_mib.h>
#else
#include <wlanwha_mib.h>
#endif


#ifndef NDEBUG
const TInt8 WlanConfigureTxAutoRatePolicy::iName[] 
    = "whacomplex-configuretxautoratepolicy";

const TUint8 WlanConfigureTxAutoRatePolicy::iStateName
    [ESTATEMAX][KMaxStateStringLength] = 
    {
        {"EINIT"}, 
        {"ESETAUTORATEPOLICY"},
        {"EFINIT"}
    };

const TUint8 WlanConfigureTxAutoRatePolicy::iEventName
    [EEVENTMAX][KMaxEventStringLength] = 
    {
        {"ESTATEENTRY"}, 
        {"ETXCOMPLETE"},
        {"EABORT"}
    };
#endif // !NDEBUG

// length of txAutoRatePolicy MIB
const TUint16 KtxAutoRatePolicyMibLen = sizeof( WHA::StxAutoRatePolicy );

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
WlanConfigureTxAutoRatePolicy::WlanConfigureTxAutoRatePolicy() : 
     iState ( EINIT ),
     iCompleteMgmtRequest ( EFalse ),
     iTxAutoRatePolicyMib ( NULL ),
     iNumberOfPolicyClassesToConfigure ( 0 ),
     iRatePolicyClass ( 0 )
    {
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
WlanConfigureTxAutoRatePolicy::~WlanConfigureTxAutoRatePolicy() 
    { 
    iTxAutoRatePolicyMib = NULL;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanConfigureTxAutoRatePolicy::Set( TBool aCompleteMgmtRequest  )
    {
    iCompleteMgmtRequest = aCompleteMgmtRequest;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanConfigureTxAutoRatePolicy::Fsm( 
    WlanContextImpl& aCtxImpl, 
    TEvent aEvent )
    {
#ifndef NDEBUG
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanConfigureTxAutoRatePolicy::Fsm: event:"));
    OsTracePrint( KUmacDetails, iEventName[aEvent] );
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanConfigureTxAutoRatePolicy::Fsm: state:"));
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
        case EABORT:
            OnAbortEvent( aCtxImpl );
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
void WlanConfigureTxAutoRatePolicy::OnStateEntryEvent( 
    WlanContextImpl& aCtxImpl )
    {
    switch ( iState )
        {
        case EINIT:
            // determine the number of rate policy classes to set
            iNumberOfPolicyClassesToConfigure = 
                (aCtxImpl.RatePolicy()).numOfPolicyObjects;
            OsTracePrint( KUmacDetails, (TUint8*)
                ("UMAC: WlanConfigureTxAutoRatePolicy::OnStateEntryEvent: nbr of policy classes to configure: %d"),
                iNumberOfPolicyClassesToConfigure );

            // rate policy class IDs start from 1, so that's the 1st to 
            // configure
            iRatePolicyClass = 1;
            // allocate memory for the MIB to write
            iTxAutoRatePolicyMib = 
                static_cast<WHA::StxAutoRatePolicy*>(
                    os_alloc( KtxAutoRatePolicyMibLen ) ); 

            if ( iTxAutoRatePolicyMib )
                {
                // start the FSM traversal
                ChangeInternalState( aCtxImpl, ESETAUTORATEPOLICY );
                }
            else
                {
                // allocation failure, abort processing
                Fsm( aCtxImpl, EABORT );                
                }
            break;
        case ESETAUTORATEPOLICY:
            SetAutoRatePolicy( aCtxImpl );                
            break;
        case EFINIT:
            // fsm execution complete. 
            
            // complete WLAN Mgmt Client request, if necessary
            if ( iCompleteMgmtRequest )
                {
                OnOidComplete( aCtxImpl, KErrNone );
                }
                
            // Traverse back to history state
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
void WlanConfigureTxAutoRatePolicy::OnTxCompleteEvent( 
    WlanContextImpl& aCtxImpl )
    {
    switch ( iState )
        {
        case ESETAUTORATEPOLICY:
            
            // one (more) rate policy class has been set
            ++iRatePolicyClass;
            
            if ( iRatePolicyClass <= iNumberOfPolicyClassesToConfigure )
                {
                // we haven't configured all the rate policy classes, yet
                // so proceed with the next one by re-entering the current
                // internal state
                ChangeInternalState( aCtxImpl, ESETAUTORATEPOLICY );
                }
            else
                {
                // all rate policy classes have been set. We are done
                ChangeInternalState( aCtxImpl, EFINIT );                
                }

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
// simulate macnotresponding error
// -----------------------------------------------------------------------------
//
void WlanConfigureTxAutoRatePolicy::OnAbortEvent( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( KWarningLevel, 
        (TUint8*)("UMAC: WlanConfigureTxAutoRatePolicy::OnAbortEvent") );

    DoErrorIndication( aCtxImpl, WHA::KErrorMacNotResponding );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanConfigureTxAutoRatePolicy::ChangeInternalState( 
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
void WlanConfigureTxAutoRatePolicy::SetAutoRatePolicy( 
    WlanContextImpl& aCtxImpl )
    {
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanConfigureTxAutoRatePolicy::SetAutoRatePolicy") );

    // retrieve reference to the stored rate policy information
    const TTxRatePolicy& ratePolicy ( aCtxImpl.RatePolicy() );
    // ... and to the stored auto rate policy
    TTxAutoRatePolicy& autoRatePolicy ( aCtxImpl.AutoRatePolicy() );
    // ... and to the stored HT MCS policy information
    const THtMcsPolicy& htMcsPolicy ( aCtxImpl.HtMcsPolicy() );

    // 1st clear
    os_memset( iTxAutoRatePolicyMib, 0, KtxAutoRatePolicyMibLen );
    
    // set the mib contents. Note that because rate policy class IDs start from
    // 1 and array indexes from 0 we need to subtract one from the rate policy
    // class ID when accessing corresponding arrays
    
    iTxAutoRatePolicyMib->iBAndGRates = autoRatePolicy[iRatePolicyClass - 1];
    iTxAutoRatePolicyMib->iTxRateClassId = iRatePolicyClass;
    iTxAutoRatePolicyMib->iShortRetryLimit = 
        ratePolicy.txRateClass[iRatePolicyClass - 1].shortRetryLimit;
    iTxAutoRatePolicyMib->iLongRetryLimit = 
        ratePolicy.txRateClass[iRatePolicyClass - 1].longRetryLimit;

    os_memcpy( 
        iTxAutoRatePolicyMib->iMcsSet,
        &(htMcsPolicy[iRatePolicyClass - 1]),
        sizeof( WHA::THtMcsSet ) );
    
    WlanWsaWriteMib& whaCmd = aCtxImpl.WsaWriteMib();
    whaCmd.Set( 
        aCtxImpl, 
        WHA::KMibTxAutoRatePolicy, 
        KtxAutoRatePolicyMibLen, 
        iTxAutoRatePolicyMib );

    // change global state: entry procedure triggers action
    ChangeState( aCtxImpl, 
        *this,              // prev state
        whaCmd              // next state
        );           
    }
    
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanConfigureTxAutoRatePolicy::Entry( 
    WlanContextImpl& aCtxImpl )
    {
    if ( aCtxImpl.WsaCmdActive() )
        {
        // sanity checking code
        OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
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
void WlanConfigureTxAutoRatePolicy::Exit( 
    WlanContextImpl& /*aCtxImpl*/ )
    {
    iState = EINIT;
    os_free( iTxAutoRatePolicyMib );
    iTxAutoRatePolicyMib = NULL;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
#ifndef NDEBUG 
const TInt8* WlanConfigureTxAutoRatePolicy::GetStateName( 
    TUint8& aLength ) const
    {
    aLength = sizeof( iName );
    return iName;
    }
#endif
