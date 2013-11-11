/*
* Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of the WlanContextImpl class
*
*/

/*
* %version: 73 %
*/

#include "config.h"
#include "UmacContextImpl.h"
#include "umacconnectcontext.h"
#include "UmacWsaInitiliaze.h"
#include "UmacWsaConfigure.h"
#include "UmacWsaReadMib.h"
#include "UmacWsaWriteMib.h"
#include "UmacWsaJoin.h"
#include "UmacWsaDisconnect.h"
#include "UmacWsaScan.h"
#include "umacwhastopscan.h"
#include "UmacWsaSetBssParameters.h"
#include "UmacWsaAddKey.h"
#include "UmacWsaSetPsMode.h"
#include "umacwhaconfigurequeue.h"
#include "umacwhaconfigureac.h"
#include "umacwharelease.h"

#include "umacaddbroadcastwepkey.h"
#include "umacconfiguretxqueueparams.h"
#include "umacconfiguretxautoratepolicy.h"


struct SWsaCommands
    {
    WlanConfigureTxQueueParams      iConfigureTxQueueParams;
    WlanAddBroadcastWepKey          iAddBroadcastWepKey;
    WlanConfigureTxAutoRatePolicy   iConfigureTxAutoRatePolicy;
    WlanWsaInitiliaze               iWsaInitiliaze;
    WlanWsaConfigure                iWsaConfigure;
    WlanWsaReadMib                  iWsaReadMib;
    WlanWsaWriteMib                 iWsaWriteMib;
    WlanWsaJoin                     iWsaJoin;
    WlanWsaDisconnect               iWsaDisconnect;
    WlanWsaScan                     iWsaScan;
    WlanWhaStopScan                 iWhaStopScan;
    WlanWsaSetBssParameters         iWsaSetBssParameters;
    WlanWsaAddKey                   iWsaAddKey;
    WlanWsaSetPsMode                iWsaSetPsMode;
    WlanWhaConfigureQueue           iWhaConfigureQueue;
    WlanWhaConfigureAc              iWhaConfigureAc;
    WlanWhaRelease                  iWhaRelease;
    };

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
WlanContextImpl::WlanContextImpl( Umac& aUmac ) : 
            iCurrentMacState( NULL ),
            iUmac( aUmac ), 
            iEnableUserData( EFalse ),
            iCurrentRxBuffer( NULL ),
            iAllowedWlanFeatures( 0 ),
            iInternalTxBufFree ( ETrue ),
            iInternalTxBufBeingWaited ( EFalse ),            
            iAlignNullData( 0 ), 
            iAlignQosNullData( 0 ),
            iConsecutiveBeaconsLostIndicated ( EFalse ),
            iConsecutiveTxFailuresIndicated ( EFalse ),
            iConsecutivePwrModeSetFailuresIndicated ( EFalse ),
            iFailedTxPacketCount ( 0 ), 
            iWsaCmdActive( EFalse ), 
            iWhaCommandAct( 0 ), 
            iWha( NULL ), 
            iWsaCommands( NULL ), 
            iEventDispatcher( aUmac ), 
            iPacketScheduler( *this ), 
            iDynamicPowerModeCntx ( *this ),
            iNullSendController( *this, *this ),
            iProprietarySnapHeader( KUndefinedSnapHeader )
    {
    OsTracePrint( KInitLevel, 
        (TUint8*)("UMAC: WlanContextImpl::WlanContextImpl(): this: 0x%08x"), 
        reinterpret_cast<TUint32>(this) );    

    os_memset( 
        iSupportedRatesLookUpTable, 
        0, 
        sizeof( iSupportedRatesLookUpTable ) );

    os_memset( &iWHASettings, 0, sizeof( iWHASettings ) );

    ClearBssMembershipFeatureList();

    os_memset( 
        &iHtBlockAckConfigure, 
        0, 
        sizeof( iHtBlockAckConfigure ) );

    ResetFrameStatistics();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanContextImpl::BootUp(
    const TUint8* aPda, 
    TUint32 aPdaLength,
    const TUint8* aFw, 
    TUint32 aFwLength )
    {    
    // boot the system up
    OsTracePrint( KInitLevel, (TUint8*)("UMAC:  WlanContextImpl::BootUp"));    

    iStates.iInitPhase1State.BootUp( *this, aPda, aPdaLength, aFw, aFwLength );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
WlanAddBroadcastWepKey& WlanContextImpl::AddBroadcastWepKey()
    {
    return iWsaCommands->iAddBroadcastWepKey;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
WlanConfigureTxQueueParams& WlanContextImpl::ConfigureTxQueueParams()
    {
    return iWsaCommands->iConfigureTxQueueParams;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
WlanConfigureTxAutoRatePolicy& WlanContextImpl::ConfigureTxAutoRatePolicy()
    {
    return iWsaCommands->iConfigureTxAutoRatePolicy;    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
WlanWsaInitiliaze& WlanContextImpl::WsaInitiliaze()
    {
    return iWsaCommands->iWsaInitiliaze;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
WlanWsaConfigure& WlanContextImpl::WsaConfigure()
    {
    return iWsaCommands->iWsaConfigure;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
WlanWsaReadMib& WlanContextImpl::WsaReadMib()
    {
    return iWsaCommands->iWsaReadMib;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
WlanWsaWriteMib& WlanContextImpl::WsaWriteMib()
    {
    return iWsaCommands->iWsaWriteMib;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
WlanWsaJoin& WlanContextImpl::WsaJoin()
    {
    return iWsaCommands->iWsaJoin;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
WlanWsaDisconnect& WlanContextImpl::WsaDisconnect()
    {
    return iWsaCommands->iWsaDisconnect;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
WlanWsaSetBssParameters& WlanContextImpl::WsaSetBssParameters()
    {
    return iWsaCommands->iWsaSetBssParameters;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
WlanWsaScan& WlanContextImpl::WsaScan()
    {
    return iWsaCommands->iWsaScan;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
WlanWhaStopScan& WlanContextImpl::WhaStopScan()
    {
    return iWsaCommands->iWhaStopScan;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
WlanWsaAddKey& WlanContextImpl::WsaAddKey()
    {
    return iWsaCommands->iWsaAddKey;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
WlanWsaSetPsMode& WlanContextImpl::WsaSetPsMode()
    {
    return iWsaCommands->iWsaSetPsMode;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
WlanWhaConfigureQueue& WlanContextImpl::WhaConfigureQueue()
    {
    return iWsaCommands->iWhaConfigureQueue;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
WlanWhaConfigureAc& WlanContextImpl::WhaConfigureAc()
    {
    return iWsaCommands->iWhaConfigureAc;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
WlanWhaRelease& WlanContextImpl::WhaRelease()
    {
    return iWsaCommands->iWhaRelease;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanContextImpl::Initialize()
    {
    // deallocation of objecs allocated here is done in destructor

    iStates.iIdleScanningMode.Set( *this );
    iStates.iIbssScanningMode.Set( *this );
    iStates.iInfrastructureScanningMode.Set( *this );

    iCurrentMacState = &(iStates.iInitPhase1State);

    OsTracePrint( KInitLevel, (TUint8*)("UMAC: WlanContextImpl::Initialize: alloc WSA commands; size: %d"), sizeof(SWsaCommands));

    iWsaCommands 
        = static_cast<SWsaCommands*>(os_alloc( sizeof(SWsaCommands) )); 
    if ( !iWsaCommands ) 
        {        
        OsTracePrint( KErrorLevel, 
            (TUint8*)("UMAC: WlanContextImpl::Initialize: alloc WSA commands failure -> abort"));                        
        return EFalse;
        }

    new (iWsaCommands) SWsaCommands;

    // made it 
    return ETrue;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
WlanContextImpl::~WlanContextImpl()
    {
    OsTracePrint( KInitLevel, 
        (TUint8*)("UMAC: WlanContextImpl::~WlanContextImpl"));        

#ifndef RD_WLAN_DDK
    delete iWha;
#endif
    iWha = NULL;

    if ( iWsaCommands ) 
        {        
        iWsaCommands->~SWsaCommands();
        os_free( iWsaCommands );
        iWsaCommands = NULL;
        }

    iCurrentMacState = NULL;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanContextImpl::MakeLookUpTable()
    {    
    // make bit rate lookup table

    OsTracePrint( KUmacDetails, 
        (TUint8*)("UMAC * construct dot11 rate to WHA rate lookup table"));

    TUint index(0);
    
    if ( iWHASettings.iRates & WHA::KRate1Mbits )
        {
        iSupportedRatesLookUpTable[index].iSupportedRate = E802Dot11Rate1MBit;
        iSupportedRatesLookUpTable[index].iWsaRate = WHA::KRate1Mbits;

        OsTracePrint( KUmacDetails, (TUint8*)("current index: %d"), index);
        OsTracePrint( KUmacDetails, (TUint8*)("dot11 rate added: 0x%02x"), 
            iSupportedRatesLookUpTable[index].iSupportedRate);
        OsTracePrint( KUmacDetails, (TUint8*)("WHA rate added: 0x%08x"), 
            iSupportedRatesLookUpTable[index].iWsaRate);

        ++index;
        }
    if ( iWHASettings.iRates & WHA::KRate2Mbits )
        {
        iSupportedRatesLookUpTable[index].iSupportedRate = E802Dot11Rate2MBit;
        iSupportedRatesLookUpTable[index].iWsaRate = WHA::KRate2Mbits;

        OsTracePrint( KUmacDetails, (TUint8*)("current index: %d"), index);
        OsTracePrint( KUmacDetails, (TUint8*)("dot11 rate added: 0x%02x"), 
            iSupportedRatesLookUpTable[index].iSupportedRate);
        OsTracePrint( KUmacDetails, (TUint8*)("WHA rate added: 0x%08x"), 
            iSupportedRatesLookUpTable[index].iWsaRate);

        ++index;
        }
    if ( iWHASettings.iRates & WHA::KRate5_5Mbits )
        {
        iSupportedRatesLookUpTable[index].iSupportedRate = E802Dot11Rate5p5MBit;
        iSupportedRatesLookUpTable[index].iWsaRate = WHA::KRate5_5Mbits;

        OsTracePrint( KUmacDetails, (TUint8*)("current index: %d"), index);
        OsTracePrint( KUmacDetails, (TUint8*)("dot11 rate added: 0x%02x"), 
            iSupportedRatesLookUpTable[index].iSupportedRate);
        OsTracePrint( KUmacDetails, (TUint8*)("WHA rate added: 0x%08x"), 
            iSupportedRatesLookUpTable[index].iWsaRate);

        ++index;
        }
    if ( iWHASettings.iRates & WHA::KRate6Mbits )
        {
        iSupportedRatesLookUpTable[index].iSupportedRate = E802Dot11Rate6MBit;
        iSupportedRatesLookUpTable[index].iWsaRate = WHA::KRate6Mbits;

        OsTracePrint( KUmacDetails, (TUint8*)("current index: %d"), index);
        OsTracePrint( KUmacDetails, (TUint8*)("dot11 rate added: 0x%02x"), 
            iSupportedRatesLookUpTable[index].iSupportedRate);
        OsTracePrint( KUmacDetails, (TUint8*)("WHA rate added: 0x%08x"), 
            iSupportedRatesLookUpTable[index].iWsaRate);

        ++index;
        }
    if ( iWHASettings.iRates & WHA::KRate9Mbits )
        {
        iSupportedRatesLookUpTable[index].iSupportedRate = E802Dot11Rate9MBit;
        iSupportedRatesLookUpTable[index].iWsaRate = WHA::KRate9Mbits;

        OsTracePrint( KUmacDetails, (TUint8*)("current index: %d"), index);
        OsTracePrint( KUmacDetails, (TUint8*)("dot11 rate added: 0x%02x"), 
            iSupportedRatesLookUpTable[index].iSupportedRate);
        OsTracePrint( KUmacDetails, (TUint8*)("WHA rate added: 0x%08x"), 
            iSupportedRatesLookUpTable[index].iWsaRate);

        ++index;
        }
    if ( iWHASettings.iRates & WHA::KRate11Mbits )
        {
        iSupportedRatesLookUpTable[index].iSupportedRate = E802Dot11Rate11MBit;
        iSupportedRatesLookUpTable[index].iWsaRate = WHA::KRate11Mbits;

        OsTracePrint( KUmacDetails, (TUint8*)("current index: %d"), index);
        OsTracePrint( KUmacDetails, (TUint8*)("dot11 rate added: 0x%02x"), 
            iSupportedRatesLookUpTable[index].iSupportedRate);
        OsTracePrint( KUmacDetails, (TUint8*)("WHA rate added: 0x%08x"), 
            iSupportedRatesLookUpTable[index].iWsaRate);

        ++index;
        }
    if ( iWHASettings.iRates & WHA::KRate12Mbits )
        {
        iSupportedRatesLookUpTable[index].iSupportedRate = E802Dot11Rate12MBit;
        iSupportedRatesLookUpTable[index].iWsaRate = WHA::KRate12Mbits;

        OsTracePrint( KUmacDetails, (TUint8*)("current index: %d"), index);
        OsTracePrint( KUmacDetails, (TUint8*)("dot11 rate added: 0x%02x"), 
            iSupportedRatesLookUpTable[index].iSupportedRate);
        OsTracePrint( KUmacDetails, (TUint8*)("WHA rate added: 0x%08x"), 
            iSupportedRatesLookUpTable[index].iWsaRate);

        ++index;
        }
    if ( iWHASettings.iRates & WHA::KRate18Mbits )
        {
        iSupportedRatesLookUpTable[index].iSupportedRate = E802Dot11Rate18MBit;
        iSupportedRatesLookUpTable[index].iWsaRate = WHA::KRate18Mbits;

        OsTracePrint( KUmacDetails, (TUint8*)("current index: %d"), index);
        OsTracePrint( KUmacDetails, (TUint8*)("dot11 rate added: 0x%02x"), 
            iSupportedRatesLookUpTable[index].iSupportedRate);
        OsTracePrint( KUmacDetails, (TUint8*)("WHA rate added: 0x%08x"), 
            iSupportedRatesLookUpTable[index].iWsaRate);

        ++index;
        }
    if ( iWHASettings.iRates & WHA::KRate22Mbits )
        {
        iSupportedRatesLookUpTable[index].iSupportedRate = E802Dot11Rate22MBit;
        iSupportedRatesLookUpTable[index].iWsaRate = WHA::KRate22Mbits;

        OsTracePrint( KUmacDetails, (TUint8*)("current index: %d"), index);
        OsTracePrint( KUmacDetails, (TUint8*)("dot11 rate added: 0x%02x"), 
            iSupportedRatesLookUpTable[index].iSupportedRate);
        OsTracePrint( KUmacDetails, (TUint8*)("WHA rate added: 0x%08x"), 
            iSupportedRatesLookUpTable[index].iWsaRate);

        ++index;
        }
    if ( iWHASettings.iRates & WHA::KRate24Mbits )
        {
        iSupportedRatesLookUpTable[index].iSupportedRate = E802Dot11Rate24MBit;
        iSupportedRatesLookUpTable[index].iWsaRate = WHA::KRate24Mbits;

        OsTracePrint( KUmacDetails, (TUint8*)("current index: %d"), index);
        OsTracePrint( KUmacDetails, (TUint8*)("dot11 rate added: 0x%02x"), 
            iSupportedRatesLookUpTable[index].iSupportedRate);
        OsTracePrint( KUmacDetails, (TUint8*)("WHA rate added: 0x%08x"), 
            iSupportedRatesLookUpTable[index].iWsaRate);

        ++index;
        }
    if ( iWHASettings.iRates & WHA::KRate33Mbits )
        {
        iSupportedRatesLookUpTable[index].iSupportedRate = E802Dot11Rate33MBit;
        iSupportedRatesLookUpTable[index].iWsaRate = WHA::KRate33Mbits;

        OsTracePrint( KUmacDetails, (TUint8*)("current index: %d"), index);
        OsTracePrint( KUmacDetails, (TUint8*)("dot11 rate added: 0x%02x"), 
            iSupportedRatesLookUpTable[index].iSupportedRate);
        OsTracePrint( KUmacDetails, (TUint8*)("WHA rate added: 0x%08x"), 
            iSupportedRatesLookUpTable[index].iWsaRate);

        ++index;
        }
    if ( iWHASettings.iRates & WHA::KRate36Mbits )
        {
        iSupportedRatesLookUpTable[index].iSupportedRate = E802Dot11Rate36MBit;
        iSupportedRatesLookUpTable[index].iWsaRate = WHA::KRate36Mbits;

        OsTracePrint( KUmacDetails, (TUint8*)("current index: %d"), index);
        OsTracePrint( KUmacDetails, (TUint8*)("dot11 rate added: 0x%02x"), 
            iSupportedRatesLookUpTable[index].iSupportedRate);
        OsTracePrint( KUmacDetails, (TUint8*)("WHA rate added: 0x%08x"), 
            iSupportedRatesLookUpTable[index].iWsaRate);

        ++index;
        }
    if ( iWHASettings.iRates & WHA::KRate48Mbits )
        {
        iSupportedRatesLookUpTable[index].iSupportedRate = E802Dot11Rate48MBit;
        iSupportedRatesLookUpTable[index].iWsaRate = WHA::KRate48Mbits;

        OsTracePrint( KUmacDetails, (TUint8*)("current index: %d"), index);
        OsTracePrint( KUmacDetails, (TUint8*)("dot11 rate added: 0x%02x"), 
            iSupportedRatesLookUpTable[index].iSupportedRate);
        OsTracePrint( KUmacDetails, (TUint8*)("WHA rate added: 0x%08x"), 
            iSupportedRatesLookUpTable[index].iWsaRate);

        ++index;
        }
    if ( iWHASettings.iRates & WHA::KRate54Mbits )
        {
        iSupportedRatesLookUpTable[index].iSupportedRate = E802Dot11Rate54MBit;
        iSupportedRatesLookUpTable[index].iWsaRate = WHA::KRate54Mbits;

        OsTracePrint( KUmacDetails, (TUint8*)("current index: %d"), index);
        OsTracePrint( KUmacDetails, (TUint8*)("dot11 rate added: 0x%02x"), 
            iSupportedRatesLookUpTable[index].iSupportedRate);
        OsTracePrint( KUmacDetails, (TUint8*)("WHA rate added: 0x%08x"), 
            iSupportedRatesLookUpTable[index].iWsaRate);

        ++index;
        }                
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanContextImpl::AttachWsa( WHA::Wha* aWha )
    {
    iWha = aWha;

    OsTracePrint( KInitLevel, 
        (TUint8*)("UMAC: WlanContextImpl::AttachWsa: address: 0x%08x"), 
        reinterpret_cast<TUint32>(iWha) );    
    OsTracePrint( KInitLevel, (TUint8*)("UMAC: WHA-CMD-Attach"));
    // attach the NWSA-callback
    iWha->Attach( *this );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanContextImpl::CommandResponse( 
    WHA::TCommandId aCommandId, 
    WHA::TStatus aStatus,
    const WHA::UCommandResponseParams& aCommandResponseParams )
    {
    OsTracePrint( KWsaCallback, 
        (TUint8*)("UMAC * WHA-CB-CommandResponse()"));

    iCurrentMacState->CommandResponse( 
        *this, 
        aCommandId,
        aStatus,
        aCommandResponseParams );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanContextImpl::OnDeferredWhaCommandComplete( 
    WHA::TCompleteCommandId aCompleteCommandId, 
    WHA::TStatus aStatus,
    const WHA::UCommandCompletionParams& aCommandCompletionParams )
    {
    if ( WsaCmdActive() )
        {
        // programming error
        OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
        }

    return iCurrentMacState->CommandComplete( 
        *this, 
        aCompleteCommandId, 
        aStatus,
        aCommandCompletionParams );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanContextImpl::OnDeferredInternalEvent( TInternalEvent aInternalEvent )
    {
    OsTracePrint( KUmacDetails, 
        (TUint8*)("UMAC: WlanContextImpl::OnDeferredInternalEvent()"));    
    
    TBool stateChanged ( EFalse );

    if ( WsaCmdActive() )
        {
        // programming error
        OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
        }

    switch ( aInternalEvent )
        {
        case KAcParamUpdate:
            stateChanged = iCurrentMacState->ConfigureAcParams( *this );
            break;
        case KPowerMgmtTransition:
            stateChanged = iCurrentMacState->ChangePowerMgmtMode( *this );
            break;
        case KTimeout:
            stateChanged = iCurrentMacState->OnTimeout( *this );
            break;
        case KVoiceCallEntryTimerTimeout:
            stateChanged = iCurrentMacState->OnVoiceCallEntryTimerTimeout( 
                *this );
            break;
        case KNullTimerTimeout:
            stateChanged = iCurrentMacState->OnNullTimerTimeout( *this );
            break;
        case KNoVoiceTimerTimeout:
            stateChanged = iCurrentMacState->OnNoVoiceTimerTimeout( *this );
            break;
        case KKeepAliveTimerTimeout:
            stateChanged = iCurrentMacState->OnKeepAliveTimerTimeout( *this );
            break;
        case KActiveToLightPsTimerTimeout:
            stateChanged = iCurrentMacState->OnActiveToLightPsTimerTimeout( 
                *this );
            break;
        case KLightPsToActiveTimerTimeout:
            stateChanged = iCurrentMacState->OnLightPsToActiveTimerTimeout( 
                *this );
            break;
        case KLightPsToDeepPsTimerTimeout:
            stateChanged = iCurrentMacState->OnLightPsToDeepPsTimerTimeout( 
                *this );
            break;
        case KSetCtsToSelf:
            stateChanged = iCurrentMacState->SetCtsToSelfMib( *this );
            break;
        case KSetRcpiTriggerLevel:
            stateChanged = iCurrentMacState->SetRcpiTriggerLevelMib( 
                *this, 
                WHA::KRcpiThresholdDefault, 
                EFalse );
            break;
        case KSetHtBssOperation:
            stateChanged = iCurrentMacState->ConfigureHtBssOperation( *this );
            break;
        default:
            OsTracePrint( KErrorLevel, 
                (TUint8*)("UMAC: aInternalEvent: %d"), aInternalEvent );
            OsAssert( 
                (TUint8*)("UMAC: panic"), 
                (TUint8*)(WLAN_FILE), 
                __LINE__ );        
        }        
    return stateChanged;
    }
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanContextImpl::CommandComplete( 
    WHA::TCompleteCommandId aCompleteCommandId, 
    WHA::TStatus aStatus,
    const WHA::UCommandCompletionParams& aCommandCompletionParams )
    {
    OsTracePrint( KWsaCallback, 
        (TUint8*)("UMAC: WHA-CB-CommandComplete: aStatus: %d"),
        aStatus );

    if ( !WsaCmdActive() )
        {
        iCurrentMacState->CommandComplete( 
            *this, 
            aCompleteCommandId, 
            aStatus,
            aCommandCompletionParams );
        }
    else
        {
        // WHA command is in progress so we must defer this access
        iEventDispatcher.Register( 
            aCompleteCommandId, 
            aStatus,
            aCommandCompletionParams );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanContextImpl::Indication( 
    WHA::TIndicationId aIndicationId,
    const WHA::UIndicationParams& aIndicationParams )
    {
    OsTracePrint( KWsaCallback, 
        (TUint8*)("UMAC: WHA-CB-Indication: indication: %d"), aIndicationId);

    iCurrentMacState->Indication( 
        *this, 
        aIndicationId, 
        aIndicationParams );
    }

// Frame sending callbacks

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanContextImpl::SendPacketTransfer(
    WHA::TPacketId aPacketId )
    {
#ifndef NDEBUG
    OsTracePrint( KWsaCallback | KWsaTx, 
        (TUint8*)("UMAC: WHA-CB-SendPacketTransfer: aPacketId: 0x%08x"),
        aPacketId );
#endif // !NDEBUG

    iPacketScheduler.SendPacketTransfer( *this, aPacketId );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanContextImpl::SendPacketComplete(
    WHA::TStatus aStatus,
    WHA::TPacketId aPacketId,
    WHA::TRate aRate,
    TUint32 aPacketQueueDelay,
    TUint32 aMediaDelay,
    TUint8 aAckFailures,
    TUint16 aSequenceNumber )
    {
#ifndef NDEBUG
    OsTracePrint( KWsaCallback | KWsaTx, (TUint8*)
        ("UMAC: WHA-CB-SendPacketComplete: aStatus: %d"),
        aStatus );
    OsTracePrint( KWsaTx, (TUint8*)
        ("UMAC: WHA-CB-SendPacketComplete: aPacketId: 0x%08x"),
        aPacketId );
    OsTracePrint( KWsaTx, (TUint8*)
        ("UMAC: WHA-CB-SendPacketComplete: aRate: 0x%08x"),
        aRate );
    OsTracePrint( KWsaTx, (TUint8*)
        ("UMAC: WHA-CB-SendPacketComplete: aPacketQueueDelay: %d"),
        aPacketQueueDelay );
    OsTracePrint( KWsaTx, (TUint8*)
        ("UMAC: WHA-CB-SendPacketComplete: aMediaDelay: %d"),
        aMediaDelay );
    OsTracePrint( KWsaTx, (TUint8*)
        ("UMAC: WHA-CB-SendPacketComplete: aAckFailures: %d"),
        aAckFailures );
    OsTracePrint( KWsaTx, (TUint8*)
        ("UMAC: WHA-CB-SendPacketComplete: aSequenceNumber: %d"),
        aSequenceNumber );
#endif // !NDEBUG
    
    iPacketScheduler.SendPacketComplete( 
        *this,
        aStatus,
        aPacketId,
        aRate,
        aPacketQueueDelay,
        aMediaDelay,
        aAckFailures );
    }

// Frame receive

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TAny* WlanContextImpl::RequestForBuffer( 
    TUint16 aLength )
    {
#ifndef NDEBUG
    OsTracePrint( KWsaCallback, 
        (TUint8*)("UMAC: WHA-CB-RequestForBuffer: aLength: %d"), aLength);
#endif // !NDEBUG

    TAny* buffer = iCurrentMacState->RequestForBuffer( *this, aLength );

    return buffer;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanContextImpl::ReceivePacket( 
    WHA::TStatus aStatus,
    const void* aFrame,
    TUint16 aLength,
    WHA::TRate aRate,
    WHA::TRcpi aRcpi,
    WHA::TChannelNumber aChannel,
    void* aBuffer,
    TUint32 aFlags )
    {
#ifndef NDEBUG
    OsTracePrint( KWsaCallback | KRxFrame, (TUint8*)
        ("UMAC: WHA-CB-ReceivePacket(): aStatus: %d"), aStatus);
    OsTracePrint( KWsaCallback | KRxFrame, (TUint8*)
        ("UMAC: aLength: %d"), aLength);
    OsTracePrint( KWsaCallback | KRxFrame, (TUint8*)
        ("UMAC: aBuffer: 0x%08x"), reinterpret_cast<TUint32>(aBuffer) );
    OsTracePrint( KWsaCallback | KRxFrame, (TUint8*)
        ("UMAC: aFlags: 0x%08x"), aFlags );

    if ( aFrame )
        {
        SDataMpduHeader* hdr 
            = reinterpret_cast<SDataMpduHeader*>(const_cast<TAny*>(aFrame));
    
        OsTracePrint( KRxFrame, 
            (TUint8*)("UMAC: WlanContextImpl::ReceivePacket: sequence nbr: %d"), 
            hdr->iHdr.SequenceNumber() );
        OsTracePrint( KRxFrame, 
            (TUint8*)("UMAC: WlanContextImpl::ReceivePacket: sequence nbr: 0x%04x"), 
            hdr->iHdr.SequenceNumber() );
        }
#endif // !NDEBUG

    if ( !(WHASettings().iCapability & WHA::SSettings::KMultipleRxBuffers ) )
        {
        aBuffer = iCurrentRxBuffer;
        OsTracePrint( KRxFrame, (TUint8*)
            ("UMAC: single Rx buf mode; aBuffer set to: 0x%08x"), 
            reinterpret_cast<TUint32>(aBuffer) );
        }

#ifndef NDEBUG
    if ( !aBuffer )
        {
        // the Rx buffer pointer is NULL. Irrespective of the mode (single or
        // multiple Rx buffers) this is a sign of an implementation error in 
        // the lower layers
        OsTracePrint( KErrorLevel, (TUint8*)("UMAC: Rx buf pointer is NULL") );
        OsAssert( (TUint8*)("UMAC: panic"),(TUint8*)(WLAN_FILE), __LINE__ );
        }
#endif // !NDEBUG
    
    iCurrentMacState->ReceivePacket( 
        *this, 
        aStatus, 
        aFrame, 
        aLength, 
        aRate, 
        aRcpi, 
        aChannel,
        reinterpret_cast<TUint8*>(aBuffer), 
        aFlags );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanContextImpl::TxNullDataFrame( 
    WlanContextImpl& aCtxImpl, 
    TBool aQosNull )
    {
    return iCurrentMacState->TxNullDataFrame( aCtxImpl, aQosNull );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
WlanContextImpl::JoinedMulticastGroups::JoinedMulticastGroups()
    : iFirstFreeIndex( 0 )
    {
    os_memset( iStorage, 0, sizeof( iStorage ) );
    Reset();
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
WlanContextImpl::TGroupAddStatus 
    WlanContextImpl::JoinedMulticastGroups::AddGroup( 
        const TMacAddress& aGroup )
    {
    TGroupAddStatus addStatus( EOk );
    TUint index( 0 );
    
    if ( iFirstFreeIndex == KMaxNbrOfGroups )
        {
        // container full, cannot add any more
        addStatus = EFull;
        }
    else
        {
        // not full
        
        if ( FindGroup( aGroup, index ) )
            {
            // group already exists, don't add again
            addStatus = EAlreadyExists;
            }
        else
            {
            // doesn't exist yet, so go ahead and add it            
            iStorage[iFirstFreeIndex] = aGroup;
            ++iFirstFreeIndex;
            }
        }
        
    return addStatus;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanContextImpl::JoinedMulticastGroups::RemoveGroup( 
    const TMacAddress& aGroup )
    {
    TBool foundAndRemoved( EFalse );
    TUint index( 0 );
    
    if ( FindGroup( aGroup, index ) )
        { 
        // entry found
        
        if ( index < ( iFirstFreeIndex - 1 ) )
            {
            // it wasn't the last (or the only) entry, so replace the entry to 
            // be removed with the last entry so that we don't create an empty 
            // slot in the middle (or in the beginning)
            iStorage[index] = iStorage[iFirstFreeIndex - 1];
            iFree[iFirstFreeIndex - 1] = ETrue;
            }
        else
            {
            // it was the last (and possibly also the only) entry
            // just mark the entry to be free
            iFree[index] = ETrue;
            }

        foundAndRemoved = ETrue;
        --iFirstFreeIndex;        
        }
        
    return foundAndRemoved;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TUint8 WlanContextImpl::JoinedMulticastGroups::Count() const
    {
    return iFirstFreeIndex;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TUint8 WlanContextImpl::JoinedMulticastGroups::GetGroups( 
    const TMacAddress*& aGroups ) const
    {
    aGroups = iStorage;
    return iFirstFreeIndex;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanContextImpl::JoinedMulticastGroups::Reset()
    {
    for (TUint8 i = 0; i < KMaxNbrOfGroups; i++ )
        {
        iFree[i] = ETrue;
        }
        
    iFirstFreeIndex = 0;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanContextImpl::JoinedMulticastGroups::FindGroup( 
    const TMacAddress& aGroup, TUint& aIndex ) const
    {
    TBool found( EFalse );
    TUint index( 0 );
    
    while ( !found && ( index < iFirstFreeIndex ) )
        {
        if ( aGroup == iStorage[index] )
            {
            found = ETrue;
            aIndex = index;
            }
        else
            {
            ++index;
            }
        }
        
    return found;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanContextImpl::UseShortSlotTime( 
    TBool aValue )
    {
    if ( aValue )
        {
        iConnectContext.iFlags |= WlanConnectContext::KUseShortSlotTime;
        }
    else
       {
        iConnectContext.iFlags &= ~(WlanConnectContext::KUseShortSlotTime);
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanContextImpl::ProtectionBitSet( TBool aValue )
    {
    if ( aValue )
        {
        iConnectContext.iFlags |= WlanConnectContext::KProtectionBitSet;
        }
    else
       {
        iConnectContext.iFlags &= ~(WlanConnectContext::KProtectionBitSet);
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanContextImpl::QosEnabled( TBool aValue )
    {
    if ( aValue )
        {
        iConnectContext.iFlags |= WlanConnectContext::KQosEnabled;
        }
    else
       {
        iConnectContext.iFlags &= ~(WlanConnectContext::KQosEnabled);
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanContextImpl::UapsdEnabled( TBool aValue )
    {
    if ( aValue )
        {
        iConnectContext.iFlags |= WlanConnectContext::KUapsdEnabled;
        }
    else
       {
        iConnectContext.iFlags &= ~(WlanConnectContext::KUapsdEnabled);
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanContextImpl::MulticastFilteringDisAllowed( TBool aValue )
    {
    if ( aValue )
        {
        iConnectContext.iFlags |= WlanConnectContext::KMulticastFilteringDisAllowed;
        }
    else
       {
        iConnectContext.iFlags &= ~(WlanConnectContext::KMulticastFilteringDisAllowed);
        }    
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanContextImpl::ErpIePresent( TBool aValue )
    {
    if ( aValue )
        {
        iConnectContext.iFlags |= WlanConnectContext::KErpIePresent;
        }
    else
       {
        iConnectContext.iFlags &= ~(WlanConnectContext::KErpIePresent);
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanContextImpl::DisassociatedByAp( TBool aValue )
    {
    if ( aValue )
        {
        iConnectContext.iFlags |= WlanConnectContext::KDisassociatedByAp;
        }
    else
       {
        iConnectContext.iFlags &= ~(WlanConnectContext::KDisassociatedByAp);
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanContextImpl::UapsdRequestedForVoice( TBool aValue )
    {
    if ( aValue )
        {
        iConnectContext.iFlags |= WlanConnectContext::KUapsdRequestedForVoice;
        }
    else
       {
        iConnectContext.iFlags &= ~(WlanConnectContext::KUapsdRequestedForVoice);
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanContextImpl::UapsdRequestedForVideo( TBool aValue )
    {
    if ( aValue )
        {
        iConnectContext.iFlags |= 
            WlanConnectContext::KUapsdRequestedForVideo;
        }
    else
       {
        iConnectContext.iFlags &= 
            ~(WlanConnectContext::KUapsdRequestedForVideo);
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanContextImpl::UapsdRequestedForBestEffort( TBool aValue )
    {
    if ( aValue )
        {
        iConnectContext.iFlags |= 
            WlanConnectContext::KUapsdRequestedForBestEffort;
        }
    else
       {
        iConnectContext.iFlags &= 
            ~(WlanConnectContext::KUapsdRequestedForBestEffort);
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanContextImpl::UapsdRequestedForBackground( TBool aValue )
    {
    if ( aValue )
        {
        iConnectContext.iFlags |= 
            WlanConnectContext::KUapsdRequestedForBackground;
        }
    else
       {
        iConnectContext.iFlags &= 
            ~(WlanConnectContext::KUapsdRequestedForBackground);
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanContextImpl::UapsdUsedForVoice( TBool aValue )
    {
    if ( aValue )
        {
        iConnectContext.iFlags |= WlanConnectContext::KUapsdUsedForVoice;
        }
    else
       {
        iConnectContext.iFlags &= ~(WlanConnectContext::KUapsdUsedForVoice);
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanContextImpl::UapsdUsedForVideo( TBool aValue )
    {
    if ( aValue )
        {
        iConnectContext.iFlags |= 
            WlanConnectContext::KUapsdUsedForVideo;
        }
    else
       {
        iConnectContext.iFlags &= 
            ~(WlanConnectContext::KUapsdUsedForVideo);
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanContextImpl::UapsdUsedForBestEffort( TBool aValue )
    {
    if ( aValue )
        {
        iConnectContext.iFlags |= 
            WlanConnectContext::KUapsdUsedForBestEffort;
        }
    else
       {
        iConnectContext.iFlags &= 
            ~(WlanConnectContext::KUapsdUsedForBestEffort);
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanContextImpl::UapsdUsedForBackground( TBool aValue )
    {
    if ( aValue )
        {
        iConnectContext.iFlags |= 
            WlanConnectContext::KUapsdUsedForBackground;
        }
    else
       {
        iConnectContext.iFlags &= 
            ~(WlanConnectContext::KUapsdUsedForBackground);
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanContextImpl::ApTestOpportunitySeekStarted( TBool aValue )
    {
    if ( aValue )
        {
        iConnectContext.iFlags |= 
            WlanConnectContext::KApTestOpportunitySeekStarted;
        }
    else
       {
        iConnectContext.iFlags &= 
            ~(WlanConnectContext::KApTestOpportunitySeekStarted);
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanContextImpl::ApTestOpportunityIndicated( TBool aValue )
    {
    if ( aValue )
        {
        iConnectContext.iFlags |= 
            WlanConnectContext::KApTestOpportunityIndicated;
        }
    else
       {
        iConnectContext.iFlags &= 
            ~(WlanConnectContext::KApTestOpportunityIndicated);
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanContextImpl::HtSupportedByNw( TBool aValue )
    {
    if ( aValue )
        {
        iConnectContext.iFlags |= 
            WlanConnectContext::KHtSupportedByNw;
        }
    else
       {
        iConnectContext.iFlags &= 
            ~(WlanConnectContext::KHtSupportedByNw);
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanContextImpl::Reassociate( TBool aValue )
    {
    if ( aValue )
        {
        iConnectContext.iFlags |= WlanConnectContext::KReassociate;
        }
    else
       {
        iConnectContext.iFlags &= ~(WlanConnectContext::KReassociate);
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanContextImpl::UseShortPreamble( TBool aValue )
    {
    if ( aValue )
        {
        iConnectContext.iFlags |= WlanConnectContext::KUseShortPreamble;
        }
    else
       {
        iConnectContext.iFlags &= ~(WlanConnectContext::KUseShortPreamble);
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TPowerMgmtMode WlanContextImpl::CurrentPwrMgmtMode() const
    {
    if ( CurrentDot11PwrMgmtMode() == WHA::KPsDisable )
        {
        return EActive;
        }
    else
        {
        const TDot11PsModeWakeupSetting lightPsModeWakeupSetting = 
            ClientLightPsModeConfig();
        
        if ( iWlanMib.iWlanWakeupInterval == 
             lightPsModeWakeupSetting.iWakeupMode )
            {
            if ( iWlanMib.iWlanWakeupInterval 
                 == EWakeUpIntervalEveryNthBeacon || 
                 iWlanMib.iWlanWakeupInterval 
                 == EWakeUpIntervalEveryNthDtim )
                {
                if ( iWlanMib.iWlanListenInterval == 
                     lightPsModeWakeupSetting.iListenInterval )
                    {
                    return ELightPs;                
                    }
                else
                    {
                    // implementation error
                    OsAssert( (TUint8*)("UMAC: panic"), 
                        (TUint8*)(WLAN_FILE), __LINE__ );
                    // this return value is not relevant
                    return ELightPs;                    
                    }                        
                }
            else
                {
                // for these wake-up modes a possible difference in listen
                // interval is not meaningful                
                return ELightPs;
                }
            }
        else 
            {
            const TDot11PsModeWakeupSetting deepPsModeWakeupSetting = 
                ClientDeepPsModeConfig();
            
            if ( iWlanMib.iWlanWakeupInterval == 
                 deepPsModeWakeupSetting.iWakeupMode )
                {
                if ( iWlanMib.iWlanWakeupInterval 
                     == EWakeUpIntervalEveryNthBeacon || 
                     iWlanMib.iWlanWakeupInterval 
                     == EWakeUpIntervalEveryNthDtim )
                    {
                    if ( iWlanMib.iWlanListenInterval == 
                         deepPsModeWakeupSetting.iListenInterval )
                        {
                        return EDeepPs;                
                        }
                    else
                        {
                        // implementation error
                        OsAssert( (TUint8*)("UMAC: panic"), 
                            (TUint8*)(WLAN_FILE), __LINE__ );
                        // this return value is not relevant
                        return ELightPs;                    
                        }                        
                    }
                else
                    {
                    // for these wake-up modes a possible difference in listen
                    // interval is not meaningful                
                    return EDeepPs;
                    }
                }
            else
                {
                // implementation error
                OsAssert( (TUint8*)("UMAC: panic"), 
                    (TUint8*)(WLAN_FILE), __LINE__ );
                // this return value is not relevant
                return EActive;
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanContextImpl::DynamicPwrModeMgtDisabled( TBool aValue )
    {
    if ( aValue )
        {
        iConnectContext.iFlags |= 
            WlanConnectContext::KDynamicPwrModeMgmtDisabled;
        }
    else
       {
        iConnectContext.iFlags &= 
            ~(WlanConnectContext::KDynamicPwrModeMgmtDisabled);
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TUint8* WlanContextImpl::GetRxBuffer( 
    const TUint16 aLengthinBytes, 
    TBool aInternal )
    {
    TUint8* buffer ( NULL );
    
    if ( aLengthinBytes <= KMaxDot11RxMpduLength )    
        {
        if ( aInternal )
            {
            buffer = iUmac.GetBufferForRxData( aLengthinBytes  );
            }
        else
            {
            buffer = iUmac.GetBufferForRxData( 
                aLengthinBytes + iWHASettings.iRxoffset );
            
            // set the current Rx buffer. This must be done only for 
            // non-internal Rx buf requests, i.e. requests from WHA layer
            iCurrentRxBuffer = buffer;
            }
        
        if ( buffer )
            {
            OsTracePrint( KRxFrame, (TUint8*)
                ("UMAC: WlanContextImpl::GetRxBuffer: buf addr: 0x%08x"),
                reinterpret_cast<TUint32>(buffer) );
            return buffer;
            }
        else
            {
            OsTracePrint( KRxFrame | KWarningLevel, (TUint8*)
                ("UMAC: WlanContextImpl::GetRxBuffer: WARNING: buffer reservation failed") );
                
            return NULL;
            }
        }
    else
        {
        OsTracePrint( KRxFrame | KWarningLevel, (TUint8*)
            ("UMAC: WlanContextImpl::GetRxBuffer: WARNING: Buffer longer than max MPDU len requested. NULL returned") );
        return NULL;        
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TDataBuffer* WlanContextImpl::GetRxFrameMetaHeader()
    {
    return iUmac.GetRxFrameMetaHeader();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanContextImpl::FreeRxFrameMetaHeader( TDataBuffer* aMetaHeader )
    {
    iUmac.FreeRxFrameMetaHeader( aMetaHeader );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TUint8* WlanContextImpl::TxBuffer( TBool aWaitIfNotFree )
    {
    TUint8* address ( NULL );
    
    if ( iInternalTxBufFree )
        {
        address = iUmac.DmaPrivateTxMemory() + iWHASettings.iTxFrameHeaderSpace;

        if ( address )
            {
            // valid address will be returned => mark buffer to be in use
            iInternalTxBufFree = EFalse;

            OsTracePrint( KUmacDetails, (TUint8*)
                ("UMAC: WlanContextImpl::TxBuffer: internal Tx buf now in use"));
            }
        }
    else
        {
        // buffer already in use. NULL will be returned.
        
        if ( aWaitIfNotFree )
            {
            // client would like to wait for the buffer to become free
            
            if ( !iInternalTxBufBeingWaited )
                {
                // the buffer is not being waited for currently
                
                // Note that someone is now waiting for the buffer
                iInternalTxBufBeingWaited = ETrue;

                OsTracePrint( KUmacDetails, (TUint8*)
                    ("UMAC: WlanContextImpl::TxBuffer: internal Tx buf already in use => now being waited for"));
                }
            else
                {
                // the buffer is already being waited for. This is an
                // implementation error
                OsAssert( 
                    (TUint8*)("UMAC: panic"), 
                    (TUint8*)(WLAN_FILE), 
                    __LINE__ );
                }
            }
        }
        
    return address;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanContextImpl::OnBssRegained()
    {
    TBool ret( EFalse );

    if ( iConsecutiveBeaconsLostIndicated ||
         iConsecutiveTxFailuresIndicated ||
         iConsecutivePwrModeSetFailuresIndicated )
        {
        ResetBssLossIndications();
        ret = ETrue;        
        }

    return ret;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TUint WlanContextImpl::AverageTxMediaDelay( 
    WHA::TQueueId aAccessCategory ) const
    {
    const TUint totalTxDataFrameCount = 
        iFrameStatistics.acSpecific[aAccessCategory].txUnicastDataFrameCount +
        iFrameStatistics.acSpecific[aAccessCategory].txMulticastDataFrameCount;
        
    // return zero if no frames have been transmitted via this AC
    return totalTxDataFrameCount ? 
            iFrameStatistics.acSpecific[aAccessCategory].txMediaDelay /
            totalTxDataFrameCount : 
            0;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanContextImpl::CalculateAverageTxMediaDelays()
    {
    for ( TUint accessCategory = 0; 
          accessCategory < EQueueIdMax; 
          ++accessCategory )
        {
        iFrameStatistics.acSpecific[accessCategory].txMediaDelay =
            AverageTxMediaDelay( static_cast<WHA::TQueueId>(accessCategory) );
        }    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanContextImpl::UpdateTotalTxDelayHistogram( 
    WHA::TQueueId aAccessCategory,
    TUint aDelay )
    {
    const TUint KTotalTxDelayBin0UpperBound = 10000; // 10 ms
    const TUint KTotalTxDelayBin1UpperBound = 20000; // 20 ms
    const TUint KTotalTxDelayBin2UpperBound = 40000; // 40 ms

    if ( aDelay <= KTotalTxDelayBin0UpperBound )
        {
        ++iFrameStatistics.acSpecific[aAccessCategory].totalTxDelayBin0;

        OsTracePrint( KWsaTxDetails, (TUint8*)
            ("UMAC: WlanContextImpl::UpdateTotalTxDelayHistogram: bin 0 cnt incremented. Count now: %d"),
            iFrameStatistics.acSpecific[aAccessCategory].totalTxDelayBin0 );
        }
    else if ( aDelay > KTotalTxDelayBin0UpperBound && 
              aDelay <= KTotalTxDelayBin1UpperBound )
        {
        ++iFrameStatistics.acSpecific[aAccessCategory].totalTxDelayBin1;

        OsTracePrint( KWsaTxDetails, (TUint8*)
            ("UMAC: WlanContextImpl::UpdateTotalTxDelayHistogram: bin 1 cnt incremented. Count now: %d"),
            iFrameStatistics.acSpecific[aAccessCategory].totalTxDelayBin1 );
        }
    else if ( aDelay > KTotalTxDelayBin1UpperBound && 
              aDelay <= KTotalTxDelayBin2UpperBound )
        {
        ++iFrameStatistics.acSpecific[aAccessCategory].totalTxDelayBin2;

        OsTracePrint( KWsaTxDetails, (TUint8*)
            ("UMAC: WlanContextImpl::UpdateTotalTxDelayHistogram: bin 2 cnt incremented. Count now: %d"),
            iFrameStatistics.acSpecific[aAccessCategory].totalTxDelayBin2 );
        }
    else // aDelay > KTotalTxDelayBin2UpperBound
        {
        ++iFrameStatistics.acSpecific[aAccessCategory].totalTxDelayBin3;

        OsTracePrint( KWsaTxDetails, (TUint8*)
            ("UMAC: WlanContextImpl::UpdateTotalTxDelayHistogram: bin 3 cnt incremented. Count now: %d"),
            iFrameStatistics.acSpecific[aAccessCategory].totalTxDelayBin3 );
        }    
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TUint WlanContextImpl::AverageTotalTxDelay( 
    WHA::TQueueId aAccessCategory ) const
    {
    const TUint totalTxDataFrameCount = 
        iFrameStatistics.acSpecific[aAccessCategory].txUnicastDataFrameCount +
        iFrameStatistics.acSpecific[aAccessCategory].txMulticastDataFrameCount;
        
    // return zero if no frames have been transmitted via this AC
    return totalTxDataFrameCount ? 
            iFrameStatistics.acSpecific[aAccessCategory].totalTxDelay /
            totalTxDataFrameCount : 
            0;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanContextImpl::CalculateAverageTotalTxDelays()
    {
    for ( TUint accessCategory = 0; 
          accessCategory < EQueueIdMax; 
          ++accessCategory )
        {
        iFrameStatistics.acSpecific[accessCategory].totalTxDelay =
            AverageTotalTxDelay( static_cast<WHA::TQueueId>(accessCategory) );
        }    
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanContextImpl::AddBssMembershipFeature( 
    T802Dot11BssMembershipSelector aFeature )
    {
    TUint8 i ( 0 );

    do
        {
        if ( iOurBssMembershipFeatureArray[i] == KUnallocated )
            {
            iOurBssMembershipFeatureArray[i] = aFeature;

            OsTracePrint( KUmacDetails, (TUint8*)
                ("UMAC: WlanContextImpl::AddBssMembershipFeature: feature %d added to our feature list"),
                aFeature );            
            
            break;
            }
        else
            {
            ++i;
            }
        } while ( i != KMaxNumOfWlanFeatures );
        
#ifndef NDEBUG 
    if ( i == KMaxNumOfWlanFeatures )
        {
        // not enough space reserved for features
        OsTracePrint( KErrorLevel, (TUint8*)
            ("UMAC: WlanContextImpl::AddBssMembershipFeature: ERROR: not enough space reserved for features") );
        OsAssert( 
            (TUint8*)("UMAC: panic"), 
            (TUint8*)(WLAN_FILE), __LINE__ );            
        }
#endif
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanContextImpl::RemoveBssMembershipFeature( TUint8 aItem )
    {
    TUint8 i ( 0 );

    do
        {
        if ( aItem == iOurBssMembershipFeatureArray[i] )
            {
            iOurBssMembershipFeatureArray[i] = KUnallocated;

            OsTracePrint( KUmacDetails, (TUint8*)
                ("UMAC: WlanContextImpl::RemoveBssMembershipFeature: "
                 "feature %d removed"),
                aItem );

            break;
            }
        else
            {
            ++i;
            }
        } while ( i != KMaxNumOfWlanFeatures );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanContextImpl::BssMembershipFeatureSupported( TUint8 aItem ) const
    {
    TBool supported ( EFalse );
    
    TUint8 i ( 0 );

    do
        {
        if ( aItem == iOurBssMembershipFeatureArray[i] )
            {
            supported = ETrue;

            OsTracePrint( KUmacDetails, (TUint8*)
                ("UMAC: WlanContextImpl::BssMembershipFeatureSupported: feature %d is supported by us"),
                aItem );            

            break;
            }
        else
            {
            ++i;
            }
        } while ( i != KMaxNumOfWlanFeatures );

    return supported;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanContextImpl::OnPacketTransferComplete( 
    WlanContextImpl& aCtxImpl, 
    TUint32 aPacketId,
    TDataBuffer* aMetaHeader )
    {
    iCurrentMacState->OnPacketTransferComplete( 
        aCtxImpl, 
        aPacketId,
        aMetaHeader );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanContextImpl::OnPacketSendComplete(
    WlanContextImpl& aCtxImpl, 
    WHA::TStatus aStatus,
    TUint32 aPacketId,
    WHA::TRate aRate,
    TUint32 aPacketQueueDelay,
    TUint32 aMediaDelay,
    TUint aTotalTxDelay,
    TUint8 aAckFailures,
    WHA::TQueueId aQueueId,
    WHA::TRate aRequestedRate,
    TBool aMulticastData )
    {
    iCurrentMacState->OnPacketSendComplete(
        aCtxImpl,
        aStatus,
        aPacketId,
        aRate,
        aPacketQueueDelay,
        aMediaDelay,
        aTotalTxDelay,
        aAckFailures,
        aQueueId,
        aRequestedRate,
        aMulticastData );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanContextImpl::CallPacketSchedule( 
    WlanContextImpl& aCtxImpl,
    TBool aMore )
    {
    iCurrentMacState->CallPacketSchedule( aCtxImpl, aMore );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanContextImpl::OnPacketFlushEvent(
    WlanContextImpl& aCtxImpl, 
    TUint32 aPacketId, 
    TDataBuffer* aMetaHeader )
    {
    iCurrentMacState->OnPacketFlushEvent( aCtxImpl, aPacketId, aMetaHeader );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanContextImpl::OnPacketPushPossible( 
    WlanContextImpl& aCtxImpl )
    {
    iCurrentMacState->OnPacketPushPossible( aCtxImpl );
    }
