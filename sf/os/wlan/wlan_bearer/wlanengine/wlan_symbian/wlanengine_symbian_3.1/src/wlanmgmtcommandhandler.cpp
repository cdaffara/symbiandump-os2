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
* Description:  Handles sending management commands to drivers.
*
*/

/*
* %version: 39 %
*/

// INCLUDES
#include <e32std.h>
#include <es_ini.h>
#include <es_mbuf.h>
#include "wlanmgmtcommandhandler.h"
#include "am_debug.h"

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// os_memcpy
// (Declared in 802dot11.h)
// -----------------------------------------------------------------------------
//
TAny* os_memcpy( 
    TAny* aDest, 
    const TAny* aSrc, 
    TUint32 aLengthinBytes )
    {
    Mem::Copy( aDest, aSrc, aLengthinBytes );
    return aDest;
    }

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CWlanMgmtCommandHandler::CWlanMgmtCommandHandler(
    RWlanLogicalChannel& aChannel,
    MWlanMgmtCommandCallback& aClient ):
    CActive( CActive::EPriorityStandard ),
    iClient( aClient ),
    iChannel( aChannel ),
    iBuffer( NULL, 0, 0 )
    {
    }

// ---------------------------------------------------------
// CWlanMgmtCommandHandler::NewL
// ---------------------------------------------------------
//
CWlanMgmtCommandHandler* CWlanMgmtCommandHandler::NewL(
    RWlanLogicalChannel& aChannel,
    MWlanMgmtCommandCallback& aClient )
    {
    DEBUG( "CWlanMgmtCommandHandler::NewL()" );
    CWlanMgmtCommandHandler* self = 
        new(ELeave) CWlanMgmtCommandHandler( aChannel, aClient );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CWlanMgmtCommandHandler::ConstructL
// ---------------------------------------------------------
//
void CWlanMgmtCommandHandler::ConstructL()
    {
    DEBUG( "CWlanMgmtFrameHandler::ConstructL()" );

    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
// CWlanMgmtCommandHandler::~CWlanMgmtCommandHandler
// ---------------------------------------------------------
//
CWlanMgmtCommandHandler::~CWlanMgmtCommandHandler()
    {
    DEBUG( "CWlanMgmtCommandHandler::~CWlanMgmtCommandHandler()" );
    }

// ---------------------------------------------------------
// CWlanMgmtCommandHandler::StartIBSS
// Start a IBSS network.
// ---------------------------------------------------------
//
void CWlanMgmtCommandHandler::StartIBSS(
    const TSSID& aSSID,
    TUint32 aBeaconInterval,
    TUint32 aChannel,
    TEncryptionStatus aEncryptionStatus )
    {
    DEBUG( "CWlanMgmtCommandHandler::StartIBSS()" );

    // First, start the adhoc network synchronously
    TStartIBSSMsg msg;
    msg.hdr.oid_id = E802_11_START_IBSS;
    msg.SSID = aSSID;
    msg.beaconInterval = aBeaconInterval;
    msg.ATIM = 0; // DEPRECATED, NOT USED ANY MORE
    msg.channel = aChannel;
    msg.encryptionStatus = aEncryptionStatus;
    TPckg<TStartIBSSMsg> buf( msg );

    TInt err = iChannel.ManagementCommand( buf, NULL, &iStatus );
    if( err )
        {
        DEBUG1( "ERROR calling RWlanLogicalChannel::ManagementCommand(): %d", err );
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, err );
        SetActive();
        return;
        }

    SetActive();
    }

// ---------------------------------------------------------
// CWlanMgmtCommandHandler::Scan
// Receive information about surrounding WLAN networks.
// ---------------------------------------------------------
//
void CWlanMgmtCommandHandler::Scan(
    TScanMode aMode,
    const TSSID& aSSID,
    TRate aScanRate,
    const SChannels& aChannels,
    TUint32 aMinChannelTime, 
    TUint32 aMaxChannelTime,
    TBool aIsSplitScan )
    {
    DEBUG( "CWlanMgmtCommandHandler::Scan()" );

    TScanMsg msg;
    msg.hdr.oid_id = E802_11_SCAN;
    msg.mode = aMode;
    msg.SSID = aSSID;
    msg.scanRate = aScanRate;
    msg.channels = aChannels;    
    DEBUG1( "CWlanMgmtCommandHandler::Scan() - aMinChannelTime %u",
        aMinChannelTime );
    DEBUG1( "CWlanMgmtCommandHandler::Scan() - aMaxChannelTime %u",
        aMaxChannelTime );
    DEBUG1( "CWlanMgmtCommandHandler::Scan() - aIsSplitScan %u",
        aIsSplitScan );
    msg.minChannelTime = aMinChannelTime;
    msg.maxChannelTime = aMaxChannelTime;
    msg.splitScan = aIsSplitScan; 
    TPckg<TScanMsg> buf( msg );
    TInt err = iChannel.ManagementCommand( buf, NULL, &iStatus );
    if( err )
        {
        DEBUG1( "ERROR calling RWlanLogicalChannel::ManagementCommand(): %d", err );
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, err );
        SetActive();
        return;
        }
   
    SetActive();
    }


// ---------------------------------------------------------
// CWlanMgmtCommandHandler::StopScan
// ---------------------------------------------------------
//
void CWlanMgmtCommandHandler::StopScan()
    {
    DEBUG( "CWlanMgmtCommandHandler::StopScan()" );

    TStopScanMsg msg;
    msg.hdr.oid_id = E802_11_STOP_SCAN;
    TPckg<TStopScanMsg> buf( msg );

    TInt err = iChannel.ManagementCommand( buf, NULL, &iStatus );
    if( err )
        {
        DEBUG1( "ERROR calling RWlanLogicalChannel::ManagementCommand(): %d", err );
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, err );
        }
    SetActive();
    }

// ---------------------------------------------------------
// CWlanMgmtCommandHandler::Disconnect
// Disconnect from the WLAN.
// ---------------------------------------------------------
//
void CWlanMgmtCommandHandler::Disconnect()
    {
    DEBUG( "CWlanMgmtCommandHandler::Disconnect()" );

    TDisconnectMsg msg;
    msg.hdr.oid_id = E802_11_DISCONNECT;
    TPckg<TDisconnectMsg> buf( msg );

    TInt err = iChannel.ManagementCommand( buf, NULL, &iStatus );
    if( err )
        {
        DEBUG1( "ERROR calling RWlanLogicalChannel::ManagementCommand(): %d", err );
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, err );
        }
    SetActive();
    }

// ---------------------------------------------------------
// CWlanMgmtCommandHandler::SetPowerMode
// Adjust current WLAN power mode.
// ---------------------------------------------------------
//
void CWlanMgmtCommandHandler::SetPowerMode(
    TPowerMode aMode,
    TBool aDisableDynamicPs,
    TWlanWakeUpInterval aWakeUpModeLight,
    TUint8 aListenIntervalLight,
    TWlanWakeUpInterval aWakeUpModeDeep,
    TUint8 aListenIntervalDeep )
    {
    DEBUG( "CWlanMgmtCommandHandler::SetPowerMode()" );
    DEBUG1( "CWlanMgmtCommandHandler::SetPowerMode() - aMode = %u ",
        aMode );
    DEBUG1( "CWlanMgmtCommandHandler::SetPowerMode() - aDisableDynamicPs = %u ",
        aDisableDynamicPs );
    DEBUG1( "CWlanMgmtCommandHandler::SetPowerMode() - aWakeUpModeLight = %u ",
        aWakeUpModeLight );
    DEBUG1( "CWlanMgmtCommandHandler::SetPowerMode() - aListenIntervalLight = %u ",
        aListenIntervalLight );
    DEBUG1( "CWlanMgmtCommandHandler::SetPowerMode() - aWakeUpModeDeep = %u ",
        aWakeUpModeDeep );
    DEBUG1( "CWlanMgmtCommandHandler::SetPowerMode() - aListenIntervalDeep = %u ",
        aListenIntervalDeep );
    
    TSetPowerModeMsg msg;
    msg.hdr.oid_id = E802_11_SET_POWER_MODE;
    msg.powerMode = aMode;
    msg.disableDynamicPowerModeManagement = aDisableDynamicPs;
    msg.wakeupModeInLightPs = aWakeUpModeLight;
    msg.listenIntervalInLightPs = aListenIntervalLight;
    msg.wakeupModeInDeepPs = aWakeUpModeDeep;
    msg.listenIntervalInDeepPs = aListenIntervalDeep; 
    TPckg<TSetPowerModeMsg> buf( msg );

    TInt err = iChannel.ManagementCommand( buf, NULL, &iStatus );
    if( err )
        {
        DEBUG1( "ERROR calling RWlanLogicalChannel::ManagementCommand(): %d", err );
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, err );
        }
    SetActive();        
    }

// ---------------------------------------------------------
// CWlanMgmtCommandHandler::SetRCPITriggerLevel
// Set RSSI trigger.
// When signal level of the current connection goes below
// this trigger an event is sent by WLAN drivers.
// ---------------------------------------------------------
//
void CWlanMgmtCommandHandler::SetRCPITriggerLevel( TUint32 aRCPITrigger )
    {
    DEBUG( "CWlanMgmtCommandHandler::SetRCPITriggerLevel()" );

    TSetRcpiTriggerLevelMsg msg;
    msg.hdr.oid_id = E802_11_SET_RCPI_TRIGGER_LEVEL;
    msg.RcpiTrigger = aRCPITrigger;

    TPckg<TSetRcpiTriggerLevelMsg> buf( msg );

    TInt err = iChannel.ManagementCommand( buf, NULL, &iStatus );
    if( err )
        {
        DEBUG1( "ERROR calling RWlanLogicalChannel::ManagementCommand(): %d", err );
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, err );
        }
    SetActive();        
    }

// ---------------------------------------------------------
// CWlanMgmtCommandHandler::SetTxPowerLevel
// Set WLAN TX power level.
// ---------------------------------------------------------
//
void CWlanMgmtCommandHandler::SetTxPowerLevel( TUint32 aLevel )
    {
    DEBUG( "CWlanMgmtCommandHandler::SetTxPowerLevel()" );

    TSetTxPowerLevelMsg msg;
    msg.hdr.oid_id = E802_11_SET_TX_POWER_LEVEL;
    msg.level = aLevel;
    TPckg<TSetTxPowerLevelMsg> buf( msg );

    TInt err = iChannel.ManagementCommand( buf, NULL, &iStatus );
    if( err )
        {
        DEBUG1( "ERROR calling RWlanLogicalChannel::ManagementCommand(): %d", err );
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, err );
        }
    SetActive();        
    }

// ---------------------------------------------------------
// CWlanMgmtCommandHandler::Configure
// Configure WLAN chip.
// This has to been done before the WLAN even scans.
// ---------------------------------------------------------
//
void CWlanMgmtCommandHandler::Configure( 
    TUint16 aRTSThreshold,
    TUint32 aMaxTxMSDULifetime,
    TUint32 aQoSNullFrameEntryTimeout,
    TUint32 aQosNullFrameEntryTxCount,
    TUint32 aQoSNullFrameInterval,
    TUint32 aQoSNullFrameExitTimeout,
    TUint32 aKeepAliveInterval,
    TUint32 aSpRcpiTarget,
    TUint32 aSpTimeTarget,
    TUint32 aSpMinIndicationInterval,
    TUint32 aEnabledFeatures )
    {
    DEBUG( "CWlanMgmtCommandHandler::Configure()" );

    DEBUG1( "CWlanMgmtCommandHandler::Configure() - aRTSThreshold = %u",
        aRTSThreshold );
    DEBUG1( "CWlanMgmtCommandHandler::Configure() - aMaxTxMSDULifetime = %u",
        aMaxTxMSDULifetime );
    DEBUG1( "CWlanMgmtCommandHandler::Configure() - aQoSNullFrameEntryTimeout = %u",
        aQoSNullFrameEntryTimeout );
    DEBUG1( "CWlanMgmtCommandHandler::Configure() - aQosNullFrameEntryTxCount = %u",
        aQosNullFrameEntryTxCount );
    DEBUG1( "CWlanMgmtCommandHandler::Configure() - aQoSNullFrameInterval = %u",
        aQoSNullFrameInterval );
    DEBUG1( "CWlanMgmtCommandHandler::Configure() - aQoSNullFrameExitTimeout = %u",
        aQoSNullFrameExitTimeout );
    DEBUG1( "CWlanMgmtCommandHandler::Configure() - aKeepAliveInterval = %u",
        aKeepAliveInterval );
    DEBUG1( "CWlanMgmtCommandHandler::Configure() - aSpRcpiTarget = %u",
        aSpRcpiTarget );
    DEBUG1( "CWlanMgmtCommandHandler::Configure() - aSpTimeTarget = %u",
        aSpTimeTarget );
    DEBUG1( "CWlanMgmtCommandHandler::Configure() - aSpMinIndicationInterval = %u",
        aSpMinIndicationInterval );
    DEBUG1( "CWlanMgmtCommandHandler::Configure() - aEnabledFeatures = 0x%08X",
        aEnabledFeatures );

    TConfigureMsg msg;
    msg.hdr.oid_id = E802_11_CONFIGURE;
    msg.allowedWlanFeatures = aEnabledFeatures;
    msg.RTSThreshold = aRTSThreshold;
    msg.maxTxMSDULifetime = aMaxTxMSDULifetime;
    msg.voiceCallEntryTimeout = aQoSNullFrameEntryTimeout;
    msg.voiceCallEntryTxThreshold = aQosNullFrameEntryTxCount;
    msg.voiceNullTimeout = aQoSNullFrameInterval;
    msg.noVoiceTimeout = aQoSNullFrameExitTimeout;
    msg.keepAliveTimeout = aKeepAliveInterval;
    msg.spRcpiIndicationLevel = aSpRcpiTarget;
    msg.spTimeToCountPrediction = aSpTimeTarget;
    msg.spMinIndicationInterval = aSpMinIndicationInterval;
    TPckg<TConfigureMsg> buf( msg );

    TInt err = iChannel.ManagementCommand( buf, NULL, &iStatus );
    if( err )
        {
        DEBUG1( "ERROR calling RWlanLogicalChannel::ManagementCommand(): %d", err );
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, err );
        }
    SetActive();
    }

// ---------------------------------------------------------
// CWlanMgmtCommandHandler::GetLastRCPI
// Get the signal quality of the last received packet.
// ---------------------------------------------------------
//
void CWlanMgmtCommandHandler::GetLastRCPI( TUint32& aRCPI )
    {
    DEBUG( "CWlanMgmtCommandHandler::GetLastRCPI()" );

    TGetLastRcpiMsg msg;
    msg.hdr.oid_id = E802_11_GET_LAST_RCPI;
    TPckg<TGetLastRcpiMsg> outbuf( msg );
    TPckg<TUint32> inbuf( aRCPI );
    iBuffer.Set( inbuf );

    TInt err = iChannel.ManagementCommand( outbuf, &iBuffer, &iStatus );
    if( err )
        {
        DEBUG1( "ERROR calling RWlanLogicalChannel::ManagementCommand(): %d", err );
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, err );
        }
    SetActive();
    }

// ---------------------------------------------------------
// CWlanMgmtCommandHandler::ConfigureMulticastGroup
// Get the signal quality of the last received packet.
// ---------------------------------------------------------
//
void CWlanMgmtCommandHandler::ConfigureMulticastGroup( 
    TBool aJoinGroup,
    const TMacAddress& aMulticastAddr )
    {
    DEBUG( "CWlanMgmtCommandHandler::ConfigureMulticastGroup()" );

    TInt err = KErrNone;
    if( aJoinGroup )
        {
        TAddMulticastAddrMsg msg;
        msg.hdr.oid_id = E802_11_ADD_MULTICAST_ADDR;
        msg.macAddress = aMulticastAddr;
        TPckg<TAddMulticastAddrMsg> outbuf( msg );
        err = iChannel.ManagementCommand( outbuf, NULL, &iStatus );
        }
    else
        {
        TRemoveMulticastAddrMsg msg;
        msg.hdr.oid_id = E802_11_REMOVE_MULTICAST_ADDR;
        msg.macAddress = aMulticastAddr;
        msg.removeAll = EFalse;
        TPckg<TRemoveMulticastAddrMsg> outbuf( msg );
        err = iChannel.ManagementCommand( outbuf, NULL, &iStatus );
        }

    if( err )
        {
        DEBUG1( "ERROR calling RWlanLogicalChannel::ManagementCommand(): %d", err );
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, err );
        }
    SetActive();
    }

// ---------------------------------------------------------
// CWlanMgmtCommandHandler::SetBssLostParameters
// ---------------------------------------------------------
//    
void CWlanMgmtCommandHandler::SetBssLostParameters(
    TUint32 aBssLostCount,
    TUint8 aFailedTxCount )
    {
    DEBUG( "CWlanMgmtCommandHandler::SetBssLostParameters()" );

    TConfigureBssLostMsg msg;
    msg.hdr.oid_id = E802_11_CONFIGURE_BSS_LOST;
    msg.beaconLostCount = aBssLostCount;
    msg.failedTxPacketCount = aFailedTxCount;
    TPckg<TConfigureBssLostMsg> buf( msg );

    TInt err = iChannel.ManagementCommand( buf, NULL, &iStatus );
    if( err )
        {
        DEBUG1( "ERROR calling RWlanLogicalChannel::ManagementCommand(): %d", err );
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, err );
        }
    SetActive();    
    }
    
// ---------------------------------------------------------
// CWlanMgmtCommandHandler::SetTxRateAdaptationParameters
// ---------------------------------------------------------
// 
void CWlanMgmtCommandHandler::SetTxRateAdaptationParameters(
    TUint8 aMinStepUpCheckpoint,
    TUint8 aMaxStepUpCheckpoint,
    TUint8 aStepUpCheckpointFactor,
    TUint8 aStepDownCheckpoint,
    TUint8 aMinStepUpThreshold,
    TUint8 aMaxStepUpThreshold,
    TUint8 aStepUpThresholdIncrement,
    TUint8 aStepDownThreshold,
    TBool aDisableProbeHandling )
    {
    DEBUG( "CWlanMgmtCommandHandler::SetTxRateAdaptationParameters()" );

    TSetTxRateAdaptationParamsMsg msg;
    msg.hdr.oid_id = E802_11_SET_TX_RATE_ADAPT_PARAMS;
    msg.minStepUpCheckpoint = aMinStepUpCheckpoint;
    msg.maxStepUpCheckpoint = aMaxStepUpCheckpoint;
    msg.stepUpCheckpointFactor = aStepUpCheckpointFactor;
    msg.stepDownCheckpoint = aStepDownCheckpoint;
    msg.minStepUpThreshold = aMinStepUpThreshold;
    msg.maxStepUpThreshold = aMaxStepUpThreshold;
    msg.stepUpThresholdIncrement = aStepUpThresholdIncrement;
    msg.stepDownThreshold = aStepDownThreshold;
    msg.disableProbeHandling = aDisableProbeHandling;
    TPckg<TSetTxRateAdaptationParamsMsg> buf( msg );

    TInt err = iChannel.ManagementCommand( buf, NULL, &iStatus );
    if( err )
        {
        DEBUG1( "ERROR calling RWlanLogicalChannel::ManagementCommand(): %d", err );
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, err );
        }
    SetActive();    
    }
    
// ---------------------------------------------------------
// CWlanMgmtCommandHandler::SetPowerModeMgmtParameters
// ---------------------------------------------------------
//    
void CWlanMgmtCommandHandler::SetPowerModeMgmtParameters(
    TUint32 aActiveToLightTimeout,
    TUint16 aActiveToLightThreshold,
    TUint32 aLightToActiveTimeout,
    TUint16 aLightToActiveThreshold,
    TUint32 aLightToDeepTimeout,
    TUint16 aLightToDeepThreshold,
    TUint16 aUapsdRxFrameLengthThreshold )
    {
    DEBUG( "CWlanMgmtCommandHandler::SetPowerModeMgmtParameters()" );

    DEBUG1( "CWlanMgmtCommandHandler::SetPowerModeMgmtParameters() - aActiveToLightTimeout = %u",
        aActiveToLightTimeout );
    DEBUG1( "CWlanMgmtCommandHandler::SetPowerModeMgmtParameters() - aActiveToLightThreshold = %u",
        aActiveToLightThreshold );
    DEBUG1( "CWlanMgmtCommandHandler::SetPowerModeMgmtParameters() - aLightToActiveTimeout = %u",
        aLightToActiveTimeout );
    DEBUG1( "CWlanMgmtCommandHandler::SetPowerModeMgmtParameters() - aLightToActiveThreshold = %u",
        aLightToActiveThreshold );
    DEBUG1( "CWlanMgmtCommandHandler::SetPowerModeMgmtParameters() - aLightToDeepTimeout = %u",
        aLightToDeepTimeout );
    DEBUG1( "CWlanMgmtCommandHandler::SetPowerModeMgmtParameters() - aLightToDeepThreshold = %u",
        aLightToDeepThreshold );
    DEBUG1( "CWlanMgmtCommandHandler::SetPowerModeMgmtParameters() - aUapsdRxFrameLengthThreshold = %u",
        aUapsdRxFrameLengthThreshold );
    
    TSetPowerModeMgmtParamsMsg msg;
    msg.hdr.oid_id = E802_11_SET_POWER_MODE_MGMT_PARAMS;
    msg.toLightPsTimeout = aActiveToLightTimeout;
    msg.toLightPsFrameThreshold = aActiveToLightThreshold;
    msg.toActiveTimeout = aLightToActiveTimeout;
    msg.toActiveFrameThreshold = aLightToActiveThreshold;
    msg.toDeepPsTimeout = aLightToDeepTimeout;
    msg.toDeepPsFrameThreshold = aLightToDeepThreshold;
    msg.uapsdRxFrameLengthThreshold = aUapsdRxFrameLengthThreshold;
    TPckg<TSetPowerModeMgmtParamsMsg> buf( msg );

    TInt err = iChannel.ManagementCommand( buf, NULL, &iStatus );
    if( err )
        {
        DEBUG1( "ERROR calling RWlanLogicalChannel::ManagementCommand(): %d", err );
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, err );
        }
    SetActive();          
    }

// ---------------------------------------------------------
// CWlanMgmtCommandHandler::SetTxRatePolicies
// ---------------------------------------------------------
//
void CWlanMgmtCommandHandler::SetTxRatePolicies(
    const TTxRatePolicy& aRatePolicies,
    const THtMcsPolicy& aMcsPolicies,
    const TQueue2RateClass& aMappings,
    const TInitialMaxTxRate4RateClass& aInitialRates,
    const TTxAutoRatePolicy& aAutoRatePolicies )
    {
    DEBUG( "CWlanMgmtCommandHandler::SetTxRatePolicies()" );

    TConfigureTxRatePoliciesMsg msg;
    msg.hdr.oid_id = E802_11_CONFIGURE_TX_RATE_POLICIES;
    msg.ratePolicy = aRatePolicies;
    Mem::Copy(
        &msg.htMcsPolicy[0][0],
        &aMcsPolicies[0][0],
        sizeof( msg.htMcsPolicy ) );        
    Mem::Copy(
        &msg.queue2RateClass[0],
        &aMappings[0],
        sizeof( msg.queue2RateClass ) );
    Mem::Copy(
        &msg.initialMaxTxRate4RateClass[0],
        &aInitialRates[0],
        sizeof( msg.initialMaxTxRate4RateClass ) );
    Mem::Copy(
        &msg.autoRatePolicy[0],
        &aAutoRatePolicies[0],
        sizeof( msg.autoRatePolicy ) );
    TPckg<TConfigureTxRatePoliciesMsg> buf( msg );

#ifdef _DEBUG
    DEBUG( "----------------------------------------------" );
    for ( TUint idx( 0 ); idx < msg.ratePolicy.numOfPolicyObjects; ++idx )
        {
        DEBUG1( "TX rate policy ID %u", idx );
        DEBUG1( "TX rate policy for 54 Mbit/s:   %u", msg.ratePolicy.txRateClass[idx].txPolicy54 );
        DEBUG1( "TX rate policy for 48 Mbit/s:   %u", msg.ratePolicy.txRateClass[idx].txPolicy48 );
        DEBUG1( "TX rate policy for 36 Mbit/s:   %u", msg.ratePolicy.txRateClass[idx].txPolicy36 );
        DEBUG1( "TX rate policy for 33 Mbit/s:   %u", msg.ratePolicy.txRateClass[idx].txPolicy33 );
        DEBUG1( "TX rate policy for 24 Mbit/s:   %u", msg.ratePolicy.txRateClass[idx].txPolicy24 );
        DEBUG1( "TX rate policy for 22 Mbit/s:   %u", msg.ratePolicy.txRateClass[idx].txPolicy22 );
        DEBUG1( "TX rate policy for 18 Mbit/s:   %u", msg.ratePolicy.txRateClass[idx].txPolicy18 );
        DEBUG1( "TX rate policy for 12 Mbit/s:   %u", msg.ratePolicy.txRateClass[idx].txPolicy12 );
        DEBUG1( "TX rate policy for 11 Mbit/s:   %u", msg.ratePolicy.txRateClass[idx].txPolicy11 );
        DEBUG1( "TX rate policy for 9 Mbit/s:    %u", msg.ratePolicy.txRateClass[idx].txPolicy9 );
        DEBUG1( "TX rate policy for 6 Mbit/s:    %u", msg.ratePolicy.txRateClass[idx].txPolicy6 );
        DEBUG1( "TX rate policy for 5.5 Mbit/s:  %u", msg.ratePolicy.txRateClass[idx].txPolicy5_5 );
        DEBUG1( "TX rate policy for 2 Mbit/s:    %u", msg.ratePolicy.txRateClass[idx].txPolicy2 );
        DEBUG1( "TX rate policy for 1 Mbit/s:    %u", msg.ratePolicy.txRateClass[idx].txPolicy1 );
        DEBUG1( "TX rate policy shortRetryLimit: %u", msg.ratePolicy.txRateClass[idx].shortRetryLimit );
        DEBUG1( "TX rate policy longRetryLimit:  %u", msg.ratePolicy.txRateClass[idx].longRetryLimit );
        DEBUG1( "TX rate policy flags:           0x%08X", msg.ratePolicy.txRateClass[idx].flags );
        DEBUG1( "TX rate policy initial rate:    0x%08X", msg.initialMaxTxRate4RateClass[idx] );        
        DEBUG1( "TX auto rate policy rate mask:  0x%08X", msg.autoRatePolicy[idx] );
        DEBUG( " " );
        }
        
    DEBUG1( "Access Class ELegacy points to policy ID %u", msg.queue2RateClass[ELegacy] );
    DEBUG1( "Access Class EBackGround points to policy ID %u", msg.queue2RateClass[EBackGround] );
    DEBUG1( "Access Class EVideo points to policy ID %u", msg.queue2RateClass[EVideo] );
    DEBUG1( "Access Class EVoice points to policy ID %u", msg.queue2RateClass[EVoice] );
    DEBUG( "----------------------------------------------" );
#endif // _DEBUG

    TInt err = iChannel.ManagementCommand( buf, NULL, &iStatus );
    if( err )
        {
        DEBUG1( "ERROR calling RWlanLogicalChannel::ManagementCommand(): %d", err );
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, err );
        }
    SetActive();          
    }    

// ---------------------------------------------------------
// CWlanMgmtCommandHandler::GetPacketStatistics
// ---------------------------------------------------------
//
void CWlanMgmtCommandHandler::GetPacketStatistics(
    TStatisticsResponse& aStatistics )
    {
    DEBUG( "CWlanMgmtCommandHandler::GetPacketStatistics()" );

    TGetFrameStatisticsMsg msg;
    msg.hdr.oid_id = E802_11_GET_FRAME_STATISTICS;
    TPckg<TGetFrameStatisticsMsg> outbuf( msg );
    TPckg<TStatisticsResponse> inbuf( aStatistics );
    iBuffer.Set( inbuf );

    TInt err = iChannel.ManagementCommand( outbuf, &iBuffer, &iStatus );
    if( err )
        {
        DEBUG1( "ERROR calling RWlanLogicalChannel::ManagementCommand(): %d", err );
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, err );
        }
    SetActive();
    }

// ---------------------------------------------------------
// CWlanMgmtCommandHandler::SetUapsdSettings
// ---------------------------------------------------------
//
void CWlanMgmtCommandHandler::SetUapsdSettings(
    TMaxServicePeriodLength aMaxServicePeriodLength,
    TBool aUapsdEnabledForVoice,
    TBool aUapsdEnabledForVideo,
    TBool aUapsdEnabledForBestEffort,
    TBool aUapsdEnabledForBackground )
    {
    DEBUG( "CWlanMgmtCommandHandler::SetUapsdSettings()" );
    
    TConfigureUapsdMsg msg;
    msg.hdr.oid_id = E802_11_CONFIGURE_UAPSD;
    msg.maxServicePeriodLength = aMaxServicePeriodLength;
    msg.uapsdForVoice = aUapsdEnabledForVoice;
    msg.uapsdForVideo = aUapsdEnabledForVideo;
    msg.uapsdForBestEffort = aUapsdEnabledForBestEffort;
    msg.uapsdForBackground = aUapsdEnabledForBackground;
    TPckg<TConfigureUapsdMsg> buf( msg );

#ifdef _DEBUG
    DEBUG1( "CWlanMgmtCommandHandler::SetUapsdSettings() - maxServicePeriodLength: 0x%02X",
        msg.maxServicePeriodLength );
    DEBUG1( "CWlanMgmtCommandHandler::SetUapsdSettings() - uapsdForVoice: %u",
        msg.uapsdForVoice );
    DEBUG1( "CWlanMgmtCommandHandler::SetUapsdSettings() - uapsdForVideo: %u",
        msg.uapsdForVideo );
    DEBUG1( "CWlanMgmtCommandHandler::SetUapsdSettings() - uapsdForBestEffort: %u",
        msg.uapsdForBestEffort );
    DEBUG1( "CWlanMgmtCommandHandler::SetUapsdSettings() - uapsdForBackground: %u",
        msg.uapsdForBackground );
#endif // _DEBUG            

    TInt err = iChannel.ManagementCommand( buf, NULL, &iStatus );
    if( err )
        {
        DEBUG1( "ERROR calling RWlanLogicalChannel::ManagementCommand(): %d", err );
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, err );
        }
    SetActive();  
    }

// ---------------------------------------------------------
// CWlanMgmtCommandHandler::SetPowerSaveSettings
// ---------------------------------------------------------
//
void CWlanMgmtCommandHandler::SetPowerSaveSettings(
    TBool aStayInUapsdPsModeForVoice,
    TBool aStayInUapsdPsModeForVideo,
    TBool aStayInUapsdPsModeForBestEffort,
    TBool aStayInUapsdPsModeForBackground,
    TBool aStayInLegacyPsModeForVoice,
    TBool aStayInLegacyPsModeForVideo,
    TBool aStayInLegacyPsModeForBestEffort,
    TBool aStayInLegacyPsModeForBackground )
    {
    DEBUG( "CWlanMgmtCommandHandler::SetPowerSaveSettings()" );

    TConfigurePwrModeMgmtTrafficOverrideMsg msg;
    msg.hdr.oid_id = E802_11_CONFIGURE_PWR_MODE_MGMT_TRAFFIC_OVERRIDE;
    msg.stayInPsDespiteUapsdVoiceTraffic = aStayInUapsdPsModeForVoice;
    msg.stayInPsDespiteUapsdVideoTraffic = aStayInUapsdPsModeForVideo;
    msg.stayInPsDespiteUapsdBestEffortTraffic = aStayInUapsdPsModeForBestEffort;
    msg.stayInPsDespiteUapsdBackgroundTraffic = aStayInUapsdPsModeForBackground;
    msg.stayInPsDespiteLegacyVoiceTraffic = aStayInLegacyPsModeForVoice;
    msg.stayInPsDespiteLegacyVideoTraffic = aStayInLegacyPsModeForVideo;
    msg.stayInPsDespiteLegacyBestEffortTraffic = aStayInLegacyPsModeForBestEffort;
    msg.stayInPsDespiteLegacyBackgroundTraffic = aStayInLegacyPsModeForBackground;
    TPckg<TConfigurePwrModeMgmtTrafficOverrideMsg> buf( msg );

#ifdef _DEBUG
    DEBUG1( "CWlanMgmtCommandHandler::SetPowerSaveSettings() - stayInPsDespiteUapsdVoiceTraffic: %u",
        msg.stayInPsDespiteUapsdVoiceTraffic );
    DEBUG1( "CWlanMgmtCommandHandler::SetPowerSaveSettings() - stayInPsDespiteUapsdVideoTraffic: %u",
        msg.stayInPsDespiteUapsdVideoTraffic );
    DEBUG1( "CWlanMgmtCommandHandler::SetPowerSaveSettings() - stayInPsDespiteUapsdBestEffortTraffic: %u",
        msg.stayInPsDespiteUapsdBestEffortTraffic );
    DEBUG1( "CWlanMgmtCommandHandler::SetPowerSaveSettings() - stayInPsDespiteUapsdBackgroundTraffic: %u",
        msg.stayInPsDespiteUapsdBackgroundTraffic );
    DEBUG1( "CWlanMgmtCommandHandler::SetPowerSaveSettings() - stayInPsDespiteLegacyVoiceTraffic: %u",
        msg.stayInPsDespiteLegacyVoiceTraffic );
    DEBUG1( "CWlanMgmtCommandHandler::SetPowerSaveSettings() - stayInPsDespiteLegacyVideoTraffic: %u",
        msg.stayInPsDespiteLegacyVideoTraffic );
    DEBUG1( "CWlanMgmtCommandHandler::SetPowerSaveSettings() - stayInPsDespiteLegacyBestEffortTraffic: %u",
        msg.stayInPsDespiteLegacyBestEffortTraffic );
    DEBUG1( "CWlanMgmtCommandHandler::SetPowerSaveSettings() - stayInPsDespiteLegacyBackgroundTraffic: %u",
        msg.stayInPsDespiteLegacyBackgroundTraffic );
#endif // _DEBUG            

    TInt err = iChannel.ManagementCommand( buf, NULL, &iStatus );
    if( err )
        {
        DEBUG1( "ERROR calling RWlanLogicalChannel::ManagementCommand(): %d", err );
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, err );
        }
    SetActive();    
    }

// ---------------------------------------------------------
// CWlanMgmtCommandHandler::SetTxQueueParameters
// ---------------------------------------------------------
//
void CWlanMgmtCommandHandler::SetTxQueueParameters(
    TQueueId aQueueId,
    TUint16 aMediumTime,
    TUint32 aMaxTxMSDULifetime )
    {
    DEBUG( "CWlanMgmtCommandHandler::SetTxQueueParameters()" );

    TConfigureTxQueueMsg msg;
    msg.hdr.oid_id = E802_11_CONFIGURE_TX_QUEUE;
    msg.queueId = aQueueId;
    msg.mediumTime = aMediumTime;
    msg.maxTxMSDULifetime = aMaxTxMSDULifetime;
    TPckg<TConfigureTxQueueMsg> buf( msg );

#ifdef _DEBUG
    DEBUG1( "CWlanMgmtCommandHandler::SetTxQueueParameters() - queueId: %u",
        msg.queueId );
    DEBUG1( "CWlanMgmtCommandHandler::SetTxQueueParameters() - mediumTime: %u",
        msg.mediumTime );
    DEBUG1( "CWlanMgmtCommandHandler::SetTxQueueParameters() - maxTxMSDULifetime: %u",
        msg.maxTxMSDULifetime );
#endif // _DEBUG            

    TInt err = iChannel.ManagementCommand( buf, NULL, &iStatus );
    if( err )
        {
        DEBUG1( "ERROR calling RWlanLogicalChannel::ManagementCommand(): %d", err );
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, err );
        }
    SetActive();    
    }

// ---------------------------------------------------------
// CWlanMgmtCommandHandler::SetBlockAckUsage
// ---------------------------------------------------------
//
void CWlanMgmtCommandHandler::SetBlockAckUsage(
    TUint8 aTxUsage,
    TUint8 aRxUsage )
    {
    DEBUG( "CWlanMgmtCommandHandler::SetBlockAckUsage()" );
    
    TConfigureHtBlockAckMsg msg;
    msg.hdr.oid_id = E802_11_CONFIGURE_HT_BLOCK_ACK;
    msg.iTxBlockAckUsage = aTxUsage;
    msg.iRxBlockAckUsage = aRxUsage;
    TPckg<TConfigureHtBlockAckMsg> buf( msg );

    DEBUG1( "CWlanMgmtCommandHandler::SetRcpiTriggerLevel() - aTxUsage is 0x%02X",
        aTxUsage );
    DEBUG1( "CWlanMgmtCommandHandler::SetRcpiTriggerLevel() - aRxUsage is 0x%02X",
        aRxUsage );

    TInt err = iChannel.ManagementCommand( buf, NULL, &iStatus );
    if( err )
        {
        DEBUG1( "ERROR calling RWlanLogicalChannel::ManagementCommand(): %d", err );
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, err );
        }
    SetActive();    
    }

// ---------------------------------------------------------
// CWlanMgmtCommandHandler::SetSnapHeaderFilter
// ---------------------------------------------------------
//
void CWlanMgmtCommandHandler::SetSnapHeaderFilter(
    TSnapHeader aHeader )
    {
    DEBUG( "CWlanMgmtCommandHandler::SetSnapHeaderFilter()" );
    
    TConfigureProprietarySnapHdrMsg msg;
    msg.hdr.oid_id = E802_11_CONFIGURE_PROPRIETARY_SNAP_HDR;
    msg.snapHdr = aHeader;
    TPckg<TConfigureProprietarySnapHdrMsg> buf( msg );

    TInt err = iChannel.ManagementCommand( buf, NULL, &iStatus );
    if( err )
        {
        DEBUG1( "ERROR calling RWlanLogicalChannel::ManagementCommand(): %d", err );
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, err );
        }
    SetActive();    
    }

// ---------------------------------------------------------
// CWlanMgmtCommandHandler::SetRcpiTriggerLevel
// ---------------------------------------------------------
//
void CWlanMgmtCommandHandler::SetRcpiTriggerLevel( 
    TUint8 aTriggerLevel )
    {
    DEBUG( "CWlanMgmtCommandHandler::SetRcpiTriggerLevel()" );

    TSetRcpiTriggerLevelMsg msg;
    msg.hdr.oid_id = E802_11_SET_RCPI_TRIGGER_LEVEL;
    msg.RcpiTrigger = aTriggerLevel;
    TPckg<TSetRcpiTriggerLevelMsg> buf( msg );

    DEBUG1( "CWlanMgmtCommandHandler::SetRcpiTriggerLevel() - aTriggerLevel is %u",
        aTriggerLevel );

    TInt err = iChannel.ManagementCommand( buf, NULL, &iStatus );
    if( err )
        {
        DEBUG1( "ERROR calling RWlanLogicalChannel::ManagementCommand(): %d", err );
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, err );
        }
    SetActive();
    }

// ---------------------------------------------------------
// CWlanMgmtCommandHandler::DisableUserData
// Block user data.
// ---------------------------------------------------------
//
void CWlanMgmtCommandHandler::DisableUserData()
    {
    DEBUG( "CWlanMgmtCommandHandler::DisableUserData()" );

    TDisableUserDataMsg msg;
    msg.hdr.oid_id = E802_11_DISABLE_USER_DATA;

    TPckg<TDisableUserDataMsg> buf( msg );

    TInt err = iChannel.ManagementCommand( buf, NULL, &iStatus );
    if( err )
        {
        DEBUG1( "ERROR calling RWlanLogicalChannel::ManagementCommand(): %d", err );
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, err );
        }
    SetActive();
    }

// ---------------------------------------------------------
// CWlanMgmtCommandHandler::EnableUserData
// Pass user data through.
// ---------------------------------------------------------
//
void CWlanMgmtCommandHandler::EnableUserData()
    {
    DEBUG( "CWlanMgmtCommandHandler::EnableUserData()" );

    TEnableUserDataMsg msg;
    msg.hdr.oid_id = E802_11_ENABLE_USER_DATA;
    TPckg<TEnableUserDataMsg> buf( msg );

    TInt err = iChannel.ManagementCommand( buf, NULL, &iStatus );
    if( err )
        {
        DEBUG1( "ERROR calling RWlanLogicalChannel::ManagementCommand(): %d", err );
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, err );
        }
    SetActive();
    }

// ---------------------------------------------------------
// CWlanMgmtCommandHandler::AddCipherKey
// ---------------------------------------------------------
//
void CWlanMgmtCommandHandler::AddCipherKey(
    TWlanCipherSuite aCipherSuite,
    TUint8 aKeyIndex,
    TUint32 aLength,
    const TUint8* aData,
    const TMacAddress& aMacAddr,
    TBool aUseAsDefault )
    {
    DEBUG( "CWlanMgmtCommandHandler::AddCipherKey()" );

    DEBUG1( "CWlanMgmtCommandHandler::AddCipherKey() - aCipherSuite = %u",
        aCipherSuite );       
    DEBUG1( "CWlanMgmtCommandHandler::AddCipherKey() - aKeyIndex = %u",
        aKeyIndex );
    DEBUG1( "CWlanMgmtCommandHandler::AddCipherKey() - aUseAsDefault = %u",
        aUseAsDefault );
    DEBUG6( "CWlanMgmtCommandHandler::AddCipherKey() - aMacAddr = %02X:%02X:%02X:%02X:%02X:%02X",
        aMacAddr.iMacAddress[0], aMacAddr.iMacAddress[1], aMacAddr.iMacAddress[2], 
        aMacAddr.iMacAddress[3], aMacAddr.iMacAddress[4], aMacAddr.iMacAddress[5] );        
    DEBUG1( "CWlanMgmtCommandHandler::AddCipherKey() - aLength = %u",
        aLength );
    DEBUG_BUFFER( aLength, aData );

    TAddCipherKeyMsg msg;
    msg.hdr.oid_id = E802_11_ADD_CIPHER_KEY;
    msg.cipherSuite = aCipherSuite;
    Mem::Copy( msg.data, aData, aLength );
    msg.length = aLength;
    msg.keyIndex = aKeyIndex;
    msg.macAddress = aMacAddr;
    msg.useAsDefaultKey = aUseAsDefault;    
    TPckg<TAddCipherKeyMsg> buf( msg );

    TInt err = iChannel.ManagementCommand( buf, NULL );
    if( err )
        {
        DEBUG1("ERROR calling RWlanLogicalChannel::ManagementCommand(): %d", err );
        }
    }

// ---------------------------------------------------------
// CWlanMgmtCommandHandler::Connect
// ---------------------------------------------------------
//
void CWlanMgmtCommandHandler::Connect(
    const TSSID& aSSID,                 
    const TMacAddress& aBSSID,          
    TUint16 aAuthAlgorithm,      
    TEncryptionStatus aEncryptionStatus,
    TWlanCipherSuite aPairwiseKeyType,
    TBool aIsInfra,
    TUint32 aIeDataLength,
    const TUint8* aIeData,        
    TUint32 aScanFrameLength,
    const TUint8* aScanFrame,
    TBool aIsPairwiseKeyInvalidated,
    TBool aIsGroupKeyInvalidated,
    TBool aIsRadioMeasurementSupported,
    const TPairwiseKeyData& aPairwiseKey )
    {
    DEBUG( "CWlanMgmtCommandHandler::Connect()" );
    
    DEBUG1( "CWlanMgmtCommandHandler::Connect() - aPairwiseKeyType = %u",
        aPairwiseKeyType );
    DEBUG1( "CWlanMgmtCommandHandler::Connect() - aPairwiseKey.length = %u",
        aPairwiseKey.length );

    TConnectMsg msg;
    msg.hdr.oid_id = E802_11_CONNECT;
    msg.SSID = aSSID;
    msg.BSSID = aBSSID;
    msg.authAlgorithmNbr = aAuthAlgorithm;
    msg.encryptionStatus = aEncryptionStatus;
    msg.pairwiseCipher = aPairwiseKeyType;
    msg.isInfra = aIsInfra;
    msg.scanResponseFrameBodyLength = aScanFrameLength;
    msg.scanResponseFrameBody = const_cast<TUint8*>( aScanFrame );
    msg.ieDataLength = aIeDataLength;    
    msg.ieData = const_cast<TUint8*>( aIeData );
    msg.invalidatePairwiseKey = aIsPairwiseKeyInvalidated;
    msg.invalidateGroupKey = aIsGroupKeyInvalidated;
    msg.radioMeasurement = aIsRadioMeasurementSupported;
    msg.pairwiseKey = aPairwiseKey;
    TPckg<TConnectMsg> buf( msg );

    TInt err = iChannel.ManagementCommand( buf, NULL, &iStatus );
    if( err )
        {
        DEBUG1( "ERROR calling RWlanLogicalChannel::ManagementCommand(): %d", err );
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, err );
        }
    SetActive();
    }

// ---------------------------------------------------------
// CWlanMgmtCommandHandler::DoCancel
// ---------------------------------------------------------
//    
void CWlanMgmtCommandHandler::DoCancel()
    {
    DEBUG( "CWlanMgmtCommandHandler::DoCancel()" );
    // RWlanLogicalChannel does not have cancel for ManagementCommand()
    }

// ---------------------------------------------------------
// CWlanMgmtCommandHandler::RunL
// ---------------------------------------------------------
//    
void CWlanMgmtCommandHandler::RunL()
    {
    DEBUG1( "CWlanMgmtCommandHandler::RunL, status == %d", iStatus.Int() );
    iClient.OnRequestComplete( iStatus.Int() );
    }

// End of file
