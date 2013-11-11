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
* Description:   Implementation of the WlanMacState class
*
*/

/*
* %version: 77 %
*/

#include "config.h"
#include "UmacWsaCommand.h"
#include "UmacDot11State.h"
#include "UmacWsaComplexCommand.h"
#include "UmacContextImpl.h"

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanMacState::ConfigureAcParams( 
    WlanContextImpl& /*aCtxImpl*/ )
    {
    // not supported in default handler
    OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );

    return EFalse;    
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanMacState::SetCtsToSelfMib( WlanContextImpl& /*aCtxImpl*/ )
    {
    // not supported in default handler
    OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );

    return EFalse;        
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanMacState::SetRcpiTriggerLevelMib(
    WlanContextImpl& /*aCtxImpl*/,
    TUint32 /*aRcpiTrigger*/,
    TBool /*aCompleteManagementRequest*/ )
    {
    // not supported in default handler
    OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );

    return EFalse;            
    }    

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanMacState::ConfigureHtBssOperation( WlanContextImpl& /*aCtxImpl*/ )
    {
    // not supported in default handler
    OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );

    return EFalse;                
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanMacState::ChangePowerMgmtMode( 
    WlanContextImpl& /*aCtxImpl*/ )
    {
    // not supported in default handler
    OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );

    return EFalse;    
    }    

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanMacState::SetRcpiTriggerLevel(
    WlanContextImpl& aCtxImpl,
    TUint32 /*aRcpiTrigger*/ )          
    {
    // not supported in default handler
#ifndef NDEBUG                
    OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
#endif
    OnOidComplete( aCtxImpl, KErrNotSupported );
    return EFalse; // no state change
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanMacState::AddBroadcastWepKey(
    WlanContextImpl& aCtxImpl,
    TUint32 /*aKeyIndex*/, 
    TBool /*aUseAsDefaulKey*/,        
    TUint32 /*aKeyLength*/,                      
    const TUint8 /*aKey*/[KMaxWEPKeyLength],
    const TMacAddress& /*aMac*/ )    
    {
    // not supported in default handler
#ifndef NDEBUG                
    OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
#endif
    OnOidComplete( aCtxImpl, KErrNotSupported );
    return EFalse; // no state change
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanMacState::AddUnicastWepKey(
    WlanContextImpl& /*aCtxImpl*/,
    const ::TMacAddress& /*aMacAddr*/,
    TUint32 /*aKeyLength*/,                      
    const TUint8 /*aKey*/[KMaxWEPKeyLength])
    {
    // not supported in default handler
    OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
    return EFalse;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanMacState::DisableUserData(
    WlanContextImpl& /*aCtxImpl*/ )
    {
    // not supported in default handler
    OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
    return EFalse;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanMacState::EnableUserData(
    WlanContextImpl& /*aCtxImpl*/ )
    {
    // not supported in default handler
    OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
    return EFalse;
    }
  
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanMacState::SetPowerMode(
    WlanContextImpl& /*aCtxImpl*/,
    TPowerMode /*aPowerMode*/,
    TBool /*aDisableDynamicPowerModeManagement*/,
    TWlanWakeUpInterval /*aWakeupModeInLightPs*/, 
    TUint8 /*aListenIntervalInLightPs*/,
    TWlanWakeUpInterval /*aWakeupModeInDeepPs*/,
    TUint8 /*aListenIntervalInDeepPs*/ )
    {
    TBool ret( EFalse );
    // not supported in default handler
    OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
    return ret;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanMacState::OnDot11PwrMgmtTransitRequired( WlanContextImpl& /*aCtxImpl*/ )
    {
    // not supported in default handler
    OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
    return EFalse;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool WlanMacState::Connect(
    WlanContextImpl& aCtxImpl,
    const TSSID& /*aSSID*/,                 
    const TMacAddress& /*aBSSID*/,          
    TUint16 /*aAuthAlgorithmNbr*/,      
    TEncryptionStatus /*aEncryptionStatus*/,
    TBool /*aIsInfra*/,
    TUint16 /*aScanResponseFrameBodyLength*/,
    const TUint8* /*aScanResponseFrameBody*/,
    const TUint8* /*aIeData*/,
    TUint16 /*aIeDataLength*/ )
    {
    // not supported in default handler
#ifndef NDEBUG                
    OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
#endif
    OnOidComplete( aCtxImpl, KErrNotSupported );
    return EFalse; // no state change
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanMacState::Disconnect( WlanContextImpl& aCtxImpl )
    {
    // not supported in default handler
#ifndef NDEBUG                
    OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
#endif
    OnOidComplete( aCtxImpl, KErrNotSupported );
    return EFalse; // no state change
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanMacState::AddTkIPKey( 
    WlanContextImpl& /*aCtxImpl*/,
    const TUint8* /*aData*/, 
    TUint32 /*aLength*/,
    T802Dot11WepKeyId /*aKeyIndex*/,
    const TMacAddress& /*aMacAddr*/ )
    {
    // not supported in default handler
    OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
    return EFalse;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanMacState::AddMulticastTKIPKey( 
    WlanContextImpl& /*aCtxImpl*/,
    T802Dot11WepKeyId /*aKeyIndex*/,
    TUint32 /*aLength*/,
    const TUint8* /*aData*/ )
    {
    // not supported in default handler
    OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
    return EFalse;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanMacState::AddAesKey( 
    WlanContextImpl& /*aCtxImpl*/,
    const TUint8* /*aData*/, 
    TUint32 /*aLength*/,
    const TMacAddress& /*aMacAddr*/ )
    {
    // not supported in default handler
    OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
    return EFalse;        
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanMacState::AddMulticastAesKey( 
    WlanContextImpl& /*aCtxImpl*/,
    T802Dot11WepKeyId /*aKeyIndex*/,
    TUint32 /*aLength*/,
    const TUint8* /*aData*/ )
    {
    // not supported in default handler
    OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
    return EFalse;                
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanMacState::AddMulticastWapiKey( 
    WlanContextImpl& /*aCtxImpl*/,
    T802Dot11WepKeyId /*aKeyIndex*/,
    TUint32 /*aLength*/,
    const TUint8* /*aData*/ )
    {
    // not supported in default handler
    OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
    return EFalse;                
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanMacState::AddUnicastWapiKey( 
    WlanContextImpl& /*aCtxImpl*/,
    const TUint8* /*aData*/,
    TUint32 /*aLength*/,
    T802Dot11WepKeyId /*aKeyIndex*/,
    const TMacAddress& /*aMacAddr*/ )
    {
    // not supported in default handler
    OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
    return EFalse;                
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanMacState::StartIBSS(
    WlanContextImpl& aCtxImpl,
    const TSSID& /*aSSID*/,                 
    TUint32 /*aBeaconInterval*/,            
    TUint32 /*aAtim*/,                      
    TUint32 /*aChannel*/,                   
    TEncryptionStatus /*aEncryptionStatus*/ )
    {
    // not supported in default handler
#ifndef NDEBUG                
    OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
#endif
    OnOidComplete( aCtxImpl, KErrNotSupported );
    return EFalse; // no state change
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanMacState::Scan(
    WlanContextImpl& /*aCtxImpl*/,
    TScanMode /*aMode*/,                    
    const TSSID& /*aSSID*/,                 
    TRate /*aScanRate*/, 
    SChannels& /*aChannels*/,
    TUint32 /*aMinChannelTime*/,            
    TUint32 /*aMaxChannelTime*/,
    TBool /*aSplitScan*/ )
    {
    // not supported in default handler
    OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
    return EFalse;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanMacState::RealScan(
    WlanContextImpl& aCtxImpl,
    TScanMode /*aMode*/,                    
    const TSSID& /*aSSID*/,                 
    TUint32 /*aScanRate*/,                    
    SChannels& /*aChannels*/,
    TUint32 /*aMinChannelTime*/,            
    TUint32 /*aMaxChannelTime*/,
    TBool /*aSplitScan*/ )
    {
    OnOidComplete( aCtxImpl, KErrNotSupported );
    return EFalse; // no state change
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanMacState::StopScan( WlanContextImpl& /*aCtxImpl*/ )
    {
    // not supported in default handler
    OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
    return EFalse;    
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanMacState::SetTxPowerLevel(
    WlanContextImpl& /*aCtxImpl*/,
    TUint32 /*aLevel*/ )
    {    
    // not supported in default handler
    OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
    return EFalse;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanMacState::Configure(
    WlanContextImpl& aCtxImpl,
    TUint32 /*aRTSThreshold*/,              
    TUint32 /*aMaxTxMSDULifetime*/,
    TUint32 /*aVoiceCallEntryTimeout*/,
    TUint32 /*aVoiceCallEntryTxThreshold*/,
    TUint32 /*aVoiceNullTimeout*/,
    TUint32 /*aNoVoiceTimeout*/,
    TUint32 /*aKeepAliveTimeout*/,         
    TUint32 /*aspRcpiIndicationLevel*/,
    TUint32 /*spTimetoCountPrediction*/,
    TUint32 /*aSpMinIndicationInterval*/ )         
    {
    // not supported in default handler
#ifndef NDEBUG                
    OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
#endif
    OnOidComplete( aCtxImpl, KErrNotSupported );
    return EFalse; // no state change
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanMacState::GetLastRcpi(
    WlanContextImpl& /*aCtxImpl*/ )
    {
    // not supported in default handler
    OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
    return EFalse;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanMacState::AddMulticastAddr(
    WlanContextImpl& /*aCtxImpl*/,
    const TMacAddress& /*aMacAddr*/ )
    {
    // not supported in default handler
    OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
    return EFalse;    
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanMacState::RemoveMulticastAddr(
    WlanContextImpl& /*aCtxImpl*/,
    TBool /*aRemoveAll*/,
    const TMacAddress& /*aMacAddr*/ )
    {
    // not supported in default handler
    OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
    return EFalse;        
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanMacState::ConfigureBssLost( 
    WlanContextImpl& /*aCtxImpl*/,
    TUint32 /*aBeaconLostCount*/,
    TUint8 /*aFailedTxPacketCount*/ )
    {
    // not supported in default handler
    OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
    return EFalse;        
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanMacState::SetTxRateAdaptParams( 
    WlanContextImpl& /*aCtxImpl*/,
    TUint8 /*aMinStepUpCheckpoint*/,
    TUint8 /*aMaxStepUpCheckpoint*/,
    TUint8 /*aStepUpCheckpointFactor*/,
    TUint8 /*aStepDownCheckpoint*/,
    TUint8 /*aMinStepUpThreshold*/,
    TUint8 /*aMaxStepUpThreshold*/,
    TUint8 /*aStepUpThresholdIncrement*/,
    TUint8 /*aStepDownThreshold*/,
    TBool /*aDisableProbeHandling*/ )
    {
    // not supported in default handler
    OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
    return EFalse;            
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanMacState::ConfigureTxRatePolicies( 
    WlanContextImpl& /*aCtxImpl*/,
    const TTxRatePolicy& /*aRatePolicy*/,
    const TQueue2RateClass& /*aQueue2RateClass*/,
    const TInitialMaxTxRate4RateClass& /*aInitialMaxTxRate4RateClass*/,
    const TTxAutoRatePolicy& /*aAutoRatePolicy*/,
    const THtMcsPolicy& /*aHtMcsPolicy*/ )
    {
    // not supported in default handler
    OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
    return EFalse;
    }
    
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanMacState::SetPowerModeManagementParameters(
    WlanContextImpl& /*aCtxImpl*/,
    TUint32 /*aToLightPsTimeout*/,
    TUint16 /*aToLightPsFrameThreshold*/,
    TUint32 /*aToActiveTimeout*/,
    TUint16 /*aToActiveFrameThreshold*/,
    TUint32 /*aToDeepPsTimeout*/,
    TUint16 /*aToDeepPsFrameThreshold*/,
    TUint16 /*aUapsdRxFrameLengthThreshold*/ )
    {
    // not supported in default handler
    OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
    return EFalse;            
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanMacState::ConfigurePwrModeMgmtTrafficOverride( 
    WlanContextImpl& /*aCtxImpl*/,
    TBool /*aStayInPsDespiteUapsdVoiceTraffic*/,
    TBool /*aStayInPsDespiteUapsdVideoTraffic*/,
    TBool /*aStayInPsDespiteUapsdBestEffortTraffic*/, 
    TBool /*aStayInPsDespiteUapsdBackgroundTraffic*/,
    TBool /*aStayInPsDespiteLegacyVoiceTraffic*/,
    TBool /*aStayInPsDespiteLegacyVideoTraffic*/,
    TBool /*aStayInPsDespiteLegacyBestEffortTraffic*/,
    TBool /*aStayInPsDespiteLegacyBackgroundTraffic*/ )
    {
    // not supported in default handler
    OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
    return EFalse;            
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanMacState::GetFrameStatistics( WlanContextImpl& /*aCtxImpl*/ )
    {
    // not supported in default handler
    OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
    return EFalse;            
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanMacState::ConfigureUapsd( 
    WlanContextImpl& /*aCtxImpl*/,
    TMaxServicePeriodLength /*aMaxServicePeriodLength*/,
    TBool /*aUapsdForVoice*/,
    TBool /*aUapsdForVideo*/,
    TBool /*aUapsdForBestEffort*/,
    TBool /*aUapsdForBackground*/ )
    {
    // not supported in default handler
    OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
    return EFalse;            
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanMacState::ConfigureTxQueueIfNecessary( 
    WlanContextImpl& aCtxImpl,
    TQueueId /*aQueueId*/,
    TUint16 /*aMediumTime*/,
    TUint32 /*aMaxTxMSDULifetime*/ )
    {
    // not supported in default handler
#ifndef NDEBUG                
    OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
#endif
    OnOidComplete( aCtxImpl, KErrNotSupported );
    return EFalse; // no state change
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanMacState::GetMacAddress(
    WlanContextImpl& /*aCtxImpl*/ )                 
    {
    // not supported in default handler
    OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
    return EFalse;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanMacState::SetProtocolStackTxOffset( 
    WlanContextImpl& aCtxImpl ) const
    {
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanMacState::SetProtocolStackTxOffset") );

    TUint32 ethernetFrameTxOffset ( 0 );
    TUint32 dot11FrameTxOffsetNotRelevant ( 0 );
    TUint32 snapFrameTxOffsetNotRelevant ( 0 );
    
    DetermineTxOffsets( 
        aCtxImpl,
        ethernetFrameTxOffset,
        dot11FrameTxOffsetNotRelevant,
        snapFrameTxOffsetNotRelevant );

    aCtxImpl.iUmac.SetProtocolStackTxOffset( 
        ethernetFrameTxOffset );    
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanMacState::ConfigureArpIpAddressFiltering(
    WlanContextImpl& /*aCtxImpl*/,
    TBool /*aEnableFiltering*/,
    TIpv4Address /*aIpv4Address*/ )
    {
    // not supported in default handler
    OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
    return EFalse;    
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanMacState::ConfigureHtBlockAck(
    WlanContextImpl& /*aCtxImpl*/, 
    TUint8 /*aTxBlockAckUsage*/,
    TUint8 /*aRxBlockAckUsage*/ )        
    {
    // not supported in default handler
    OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
    return EFalse;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanMacState::ConfigureProprietarySnapHdr(
    WlanContextImpl& /*aCtxImpl*/, 
    const TSnapHeader& /*aSnapHeader*/ )        
    {
    // not supported in default handler
    OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
    return EFalse;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanMacState::TxData( 
    WlanContextImpl& /*aCtxImpl*/, 
    TDataBuffer& /*aDataBuffer*/,
    TBool /*aMore*/ )
    {
    // not supported in default handler
    OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
    return EFalse;    
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanMacState::TxMgmtData( 
    WlanContextImpl& /*aCtxImpl*/, 
    TDataBuffer& /* aDataBuffer */)
    {
    // not supported in default handler
    OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanMacState::FinitSystem( 
    WlanContextImpl& /*aCtxImpl*/ )
    {
    // not supported in default handler
    OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanMacState::OnTimeout( WlanContextImpl& /*aCtxImpl*/ )
    {
    // a timeout occurred when we weren't expecting it. Because this can happen
    // if a timeout callback had already been registered when we tried to cancel
    // the timer, we need to ignore this event

    OsTracePrint( KWarningLevel, (TUint8*)
        ("UMAC: WlanMacState::OnTimeout: unexpected timeout; ignored") );

    return EFalse;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanMacState::OnVoiceCallEntryTimerTimeout( 
    WlanContextImpl& /*aCtxImpl*/ )
    {
    // a timeout occurred when we weren't expecting it. Because this can happen
    // if a timeout callback had already been registered when we tried to cancel
    // the timer, we need to ignore this event

    OsTracePrint( KWarningLevel, (TUint8*)
        ("UMAC: WlanMacState::OnVoiceCallEntryTimerTimeout: unexpected timeout; ignored") );

    return EFalse;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanMacState::OnNullTimerTimeout( WlanContextImpl& /*aCtxImpl*/ )
    {
    // a timeout occurred when we weren't expecting it. Because this can happen
    // if a timeout callback had already been registered when we tried to cancel
    // the timer, we need to ignore this event

    OsTracePrint( KWarningLevel, (TUint8*)
        ("UMAC: WlanMacState::OnNullTimerTimeout: unexpected timeout; ignored") );

    return EFalse;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanMacState::OnNoVoiceTimerTimeout( WlanContextImpl& /*aCtxImpl*/ )
    {
    // a timeout occurred when we weren't expecting it. Because this can happen
    // if a timeout callback had already been registered when we tried to cancel
    // the timer, we need to ignore this event

    OsTracePrint( KWarningLevel, (TUint8*)
        ("UMAC: WlanMacState::OnNoVoiceTimerTimeout: unexpected timeout; ignored") );

    return EFalse;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanMacState::OnKeepAliveTimerTimeout( WlanContextImpl& /*aCtxImpl*/ )
    {
    // a timeout occurred when we weren't expecting it. Because this can happen
    // if a timeout callback had already been registered when we tried to cancel
    // the timer, we need to ignore this event

    OsTracePrint( KWarningLevel, (TUint8*)
        ("UMAC: WlanMacState::OnKeepAliveTimerTimeout: unexpected timeout; ignored") );

    return EFalse;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanMacState::OnActiveToLightPsTimerTimeout( 
    WlanContextImpl& /*aCtxImpl*/ )
    {
    // a timeout occurred when we weren't expecting it. Because this can happen
    // if a timeout callback had already been registered when we tried to cancel
    // the timer, we need to ignore this event

    OsTracePrint( KWarningLevel, (TUint8*)
        ("UMAC: WlanMacState::OnActiveToLightPsTimerTimeout: unexpected timeout; ignored") );

    return EFalse;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanMacState::OnLightPsToActiveTimerTimeout( WlanContextImpl& /*aCtxImpl*/ )
    {
    // a timeout occurred when we weren't expecting it. Because this can happen
    // if a timeout callback had already been registered when we tried to cancel
    // the timer, we need to ignore this event

    OsTracePrint( KWarningLevel, (TUint8*)
        ("UMAC: WlanMacState::OnLightPsToActiveTimerTimeout: unexpected timeout; ignored") );

    return EFalse;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanMacState::OnLightPsToDeepPsTimerTimeout( WlanContextImpl& /*aCtxImpl*/ )
    {
    // a timeout occurred when we weren't expecting it. Because this can happen
    // if a timeout callback had already been registered when we tried to cancel
    // the timer, we need to ignore this event

    OsTracePrint( KWarningLevel, (TUint8*)
        ("UMAC: WlanMacState::OnLightPsToDeepPsTimerTimeout: unexpected timeout; ignored") );

    return EFalse;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanMacState::OnDfc( TAny* /*aCtx*/ )
    {
    OsAssert( (TUint8*)("UMAC: panic"),(TUint8*)(WLAN_FILE), __LINE__ );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanMacState::TxNullDataFrame( 
    WlanContextImpl& /*aCtxImpl*/,
    TBool /*aQosNull*/ )
    {
    // not supported in default handler
    OsAssert( (TUint8*)("UMAC: panic"),(TUint8*)(WLAN_FILE), __LINE__ );    
    return EFalse;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanMacState::OnPacketTransferComplete( 
    WlanContextImpl& /*aCtxImpl*/, 
    TUint32 /*aPacketId*/,
    TDataBuffer* /*aMetaHeader*/ )
    {
    OsAssert( (TUint8*)("UMAC: panic"),(TUint8*)(WLAN_FILE), __LINE__ );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanMacState::OnPacketSendComplete(
    WlanContextImpl& /*aCtxImpl*/, 
    WHA::TStatus /*aStatus*/,
    TUint32 /*aPacketId*/,
    WHA::TRate /*aRate*/,
    TUint32 /*aPacketQueueDelay*/,
    TUint32 /*aMediaDelay*/,
    TUint /*aTotalTxDelay*/,
    TUint8 /*aAckFailures*/,
    WHA::TQueueId /*aQueueId*/,
    WHA::TRate /*aRequestedRate*/,
    TBool /*aMulticastData*/ )
    {
    OsAssert( (TUint8*)("UMAC: panic"),(TUint8*)(WLAN_FILE), __LINE__ );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanMacState::CallPacketSchedule( 
    WlanContextImpl& /*aCtxImpl*/,
    TBool /*aMore*/ )
    {
    OsAssert( (TUint8*)("UMAC: panic"),(TUint8*)(WLAN_FILE), __LINE__ );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanMacState::OnPacketFlushEvent(
    WlanContextImpl& /*aCtxImpl*/, 
    TUint32 /*aPacketId*/,
    TDataBuffer* /*aMetaHeader*/ )
    {
    OsAssert( (TUint8*)("UMAC: panic"),(TUint8*)(WLAN_FILE), __LINE__ );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanMacState::OnPacketPushPossible( 
    WlanContextImpl& /*aCtxImpl*/ )
    {
    OsAssert( (TUint8*)("UMAC: panic"),(TUint8*)(WLAN_FILE), __LINE__ );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanMacState::IsRequestedFrameType(
    TUint8 aLhs,
    TUint8 aRhs,
    TBool& aTypeMatch)
    {
    // do the type fields match?
    aTypeMatch = 
        static_cast<TBool>(( ( aLhs & E802Dot11FrameControlTypeMask ) 
        == ( aRhs & E802Dot11FrameControlTypeMask ) ));

    // we don't have to care about possible protocol version bits mismatch

    return static_cast<TBool>(( aLhs == aRhs ));
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanMacState::CommandResponse( 
    WlanContextImpl& /*aCtxImpl*/, 
    WHA::TCommandId aCommandId, 
    WHA::TStatus /*aStatus*/,
    const WHA::UCommandResponseParams& /*aCommandResponseParams*/ )
    {
    // only supported by whacommand state
    // anywhere else is a programming error 
    // in the underlying adaptation software
    OsTracePrint( KErrorLevel, 
        (TUint8*)("UMAC: aCommandId: %d"), aCommandId );
    OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanMacState::CommandComplete( 
    WlanContextImpl& /*aCtxImpl*/, 
    WHA::TCompleteCommandId aCompleteCommandId, 
    WHA::TStatus /*aStatus*/,
    const WHA::UCommandCompletionParams& /*aCommandCompletionParams*/ )
    {
    // not supported in default handler
    OsTracePrint( KErrorLevel, 
        (TUint8*)("UMAC: aCompleteCommandId: %d"), aCompleteCommandId );
    OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );

    return EFalse;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanMacState::Indication( 
    WlanContextImpl& /*aCtxImpl*/, 
    WHA::TIndicationId aIndicationId,
    const WHA::UIndicationParams& /*aIndicationParams*/ )
    {
    // not supported in default handler
    OsTracePrint( KErrorLevel, 
        (TUint8*)("UMAC: WlanMacState::Indication: aIndicationId: %d"), 
        aIndicationId );
    OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TAny* WlanMacState::RequestForBuffer ( 
    WlanContextImpl& /*aCtxImpl*/,
    TUint16 /*aLength*/ )
    {
    // not supported in default handler
    OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
    return NULL;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanMacState::ReceivePacket( 
    WlanContextImpl& /*aCtxImpl*/, 
    WHA::TStatus /*aStatus*/,
    const void* /*aFrame*/,
    TUint16 /*aLength*/,
    WHA::TRate /*aRate*/,
    WHA::TRcpi /*aRcpi*/,
    WHA::TChannelNumber /*aChannel*/,
    TUint8* /*aBuffer*/,
    TUint32 /*aFlags*/ )
    {
    // not supported in default handler
    OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanMacState::DoErrorIndication( 
    WlanContextImpl& /*aCtxImpl*/, 
    WHA::TStatus /*aStatus*/ )
    {
    // not supported in default handler
    OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
    return EFalse;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanMacState::DoConsecutiveBeaconsLostIndication( 
    WlanContextImpl& /*aCtxImpl*/ )
    {
    // not supported in default handler
    OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanMacState::DoConsecutiveTxFailuresIndication( 
    WlanContextImpl& /*aCtxImpl*/ )
    {
    // not supported in default handler
    OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanMacState::DoConsecutivePwrModeSetFailuresIndication( 
    WlanContextImpl& /*aCtxImpl*/ )
    {
    // not supported in default handler
    OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanMacState::DoRegainedBSSIndication( WlanContextImpl& /*aCtxImpl*/ )
    {
    // not supported in default handler
    OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanMacState::DoRadarIndication( WlanContextImpl& /*aCtxImpl*/ )
    {
    // not supported in default handler
    OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanMacState::DoRcpiIndication( 
    WlanContextImpl& /*aCtxImpl*/,
    WHA::TRcpi /*aRcpi*/ )
    {
    // not supported in default handler
    OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanMacState::DoPsModeErrorIndication( 
    WlanContextImpl& /*aCtxImpl*/ )
    {
    // not supported in default handler
    OsAssert( (TUint8*)("UMAC: panic"), (TUint8*)(WLAN_FILE), __LINE__ );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanMacState::OnOidComplete( 
    WlanContextImpl& aCtxImpl,
    TInt aReason,
    const TAny* aData, 
    TUint32 aLengthInBytes )
    {
    OsTracePrint( KWlmCmd, (TUint8*)
        ("UMAC: WlanMacState::OnOidComplete: WLM-request complete: status: %d"), 
        aReason );    

    aCtxImpl.iUmac.CompleteManagementCommand( 
        aReason, 
        aData, 
        aLengthInBytes );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanMacState::OnTxProtocolStackDataComplete( 
    WlanContextImpl& aCtxImpl,
    TDataBuffer* aMetaHeader,
    TInt aCompletionCode )
    {
    OsTracePrint( KWlmCmd, (TUint8*)
        ("UMAC: WlanMacState::OnTxProtocolStackDataComplete: WLM-request complete: status: %d"), 
        aCompletionCode );    
    aCtxImpl.iUmac.OnTxProtocolStackDataComplete( 
        aCompletionCode,
        aMetaHeader );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanMacState::OnMgmtPathWriteComplete( 
    WlanContextImpl& aCtxImpl,
    TInt aCompletionCode )
    {
    OsTracePrint( KWlmCmd, (TUint8*)
        ("UMAC: WlanMacState::OnMgmtPathWriteComplete: WLM-request complete: status: %d"), 
        aCompletionCode );    
    aCtxImpl.iUmac.MgmtPathWriteComplete( 
        aCompletionCode );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanMacState::OnInDicationEvent( 
    WlanContextImpl& aCtxImpl,
    TIndication aIndication )
    {
    OsTracePrint( KWlmIndication, (TUint8*)
        ("UMAC: WlanMacState::OnInDicationEvent: WLM-indication: %d"), 
        static_cast<TUint32>(aIndication) );    

    aCtxImpl.iUmac.OnInDicationEvent( aIndication );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void WlanMacState::DetermineTxOffsets( 
    WlanContextImpl& aCtxImpl,
    TUint32& aEthernetFrameTxOffset,
    TUint32& aDot11FrameTxOffset,
    TUint32& aSnapFrameTxOffset ) const
    {
    const TUint32 KVendorTxOffset ( 
        aCtxImpl.WHASettings().iTxFrameHeaderSpace );

    const TUint32 KMacHdrLen ( aCtxImpl.QosEnabled() ? 
                                sizeof( SHtQosDataFrameHeader ) :
                                sizeof( SDataFrameHeader ) );

    // regarding the security header we simplify things by assuming the worst
    // case (i.e. the longest security header). Doing this means that we don't
    // need to update the frame Tx offsets every time a cipher key is taken
    // into use
    const TUint32 KMaxSecurityHdrLen ( Align4( KMaxDot11SecurityHeaderLength ) );

    const TUint32 KSnapHdrLen ( sizeof( SSnapHeader ) );
    
    const TUint32 KMacAddrLen ( sizeof( TMacAddress ) );

    OsTracePrint( KWsaTxDetails, (TUint8*)
        ("UMAC: WlanMacState::DetermineTxOffsets: KVendorTxOffset: %d"),
        KVendorTxOffset );

    OsTracePrint( KWsaTxDetails, (TUint8*)
        ("UMAC: WlanMacState::DetermineTxOffsets: KMacHdrLen: %d"),
        KMacHdrLen );

    OsTracePrint( KWsaTxDetails, (TUint8*)
        ("UMAC: WlanMacState::DetermineTxOffsets: KMaxSecurityHdrLen: %d"),
        KMaxSecurityHdrLen );

    OsTracePrint( KWsaTxDetails, (TUint8*)
        ("UMAC: WlanMacState::DetermineTxOffsets: KSnapHdrLen: %d"),
        KSnapHdrLen );

    OsTracePrint( KWsaTxDetails, (TUint8*)
        ("UMAC: WlanMacState::DetermineTxOffsets: KMacAddrLen: %d"),
        KMacAddrLen );

    aEthernetFrameTxOffset =
        KVendorTxOffset + 
        KMacHdrLen + 
        KMaxSecurityHdrLen + 
        KSnapHdrLen -
        ( 2 * KMacAddrLen ); // destination and source address
    
    aDot11FrameTxOffset = KVendorTxOffset;
    
    aSnapFrameTxOffset =
        KVendorTxOffset + 
        KMacHdrLen + 
        KMaxSecurityHdrLen;

    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanMacState::DetermineTxOffsets: aEthernetFrameTxOffset: %d"),
        aEthernetFrameTxOffset );
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanMacState::DetermineTxOffsets: aDot11FrameTxOffset: %d"),
        aDot11FrameTxOffset );
    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanMacState::DetermineTxOffsets: aSnapFrameTxOffset: %d"),
        aSnapFrameTxOffset );        
    }
