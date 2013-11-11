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
* Description:   Implementation of the WlanDot11MacError class.
*
*/

/*
* %version: 23 %
*/

#include "config.h"
#include "UmacDot11MacError.h"
#include "UmacContextImpl.h"
#include "umacwharelease.h"

#ifndef NDEBUG 
const TInt8 WlanDot11MacError::iName[] = "dot11-macerror";
#endif // !NDEBUG

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11MacError::Entry( 
    WlanContextImpl& aCtxImpl )
    {
    if ( !(iFlags & KEntryExecuted) )
        {
        iFlags |= KEntryExecuted;

        // cancel posssible pending timeouts...
        aCtxImpl.CancelTimer();
        aCtxImpl.iUmac.CancelTimeout( EWlanVoiceCallEntryTimer );
        aCtxImpl.iUmac.CancelTimeout( EWlanNullTimer );
        aCtxImpl.iUmac.CancelTimeout( EWlanNoVoiceTimer );            
        aCtxImpl.iUmac.CancelTimeout( EWlanKeepAliveTimer );            
        aCtxImpl.iUmac.CancelTimeout( EWlanActiveToLightPsTimer );
        aCtxImpl.iUmac.CancelTimeout( EWlanLightPsToActiveTimer );
        aCtxImpl.iUmac.CancelTimeout( EWlanLightPsToDeepPsTimer );
        
        // ... and indicate mac error
        OnInDicationEvent( aCtxImpl, EHWFailed );

        // we also
        // mark WHA cmd inactive
        aCtxImpl.DeActivateWsaCmd();
        // ... and enable oid requests
        aCtxImpl.Enable( WlanEventDispatcher::KOidChannel );
        // ... to give some slack to the mgmt client to try to complete 
        // whatever it was doing when the error occurred, and then finally 
        // reset the system
        
        // complete possibly pending request with an error code
        OnOidComplete( aCtxImpl, KErrGeneral );
        // complete possibly pending mgmt client Tx request 
        OnMgmtPathWriteComplete( aCtxImpl );
        }
    else if ( iFlags & KCompleteOid )
        {
        iFlags &= ~KCompleteOid;
        OnOidComplete( aCtxImpl );
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11MacError::Exit( 
    WlanContextImpl& /*aCtxImpl*/ )
    {
    // nothing to do here
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11MacError::FinitSystem( 
    WlanContextImpl& aCtxImpl )
    {
    // mark oid completion 
    iFlags |= KCompleteOid;
    // execute transition
    ChangeState( aCtxImpl, 
        *this,                      // prev state
        aCtxImpl.WhaRelease()       // next state
        );      
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void WlanDot11MacError::Indication( 
    WlanContextImpl& /*aCtxImpl*/, 
    WHA::TIndicationId /*aIndicationId*/,
    const WHA::UIndicationParams& /*aIndicationParams*/ )
    {
    // silently discard WHA indications as we are in error state
    // there is no point of forwarding them to the layer above
    }

// we have defined handlers for the methods below as it is possible that they
// still get called after we have sent the HW Failure indication 

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11MacError::Configure(
    WlanContextImpl& aCtxImpl,
    TUint32 /*aRTSThreshold*/,              
    TUint32 /*aMaxTxMSDULifetime*/,
    TUint32 /*aVoiceCallEntryTimeout*/,
    TUint32 /*aVoiceCallEntryTxThreshold*/,
    TUint32 /*aVoiceNullTimeout*/,
    TUint32 /*aNoVoiceTimeout*/,
    TUint32 /*aKeepAliveTimeout*/,
    TUint32 /*aSpRcpiIndicationLevel*/,
    TUint32 /*aSpTimeToCountPrediction*/,
    TUint32 /*aSpMinIndicationInterval*/ )
    {
    return CompleteMgmtCommand( aCtxImpl, KErrGeneral );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11MacError::Connect(
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
    return CompleteMgmtCommand( aCtxImpl, KErrGeneral );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11MacError::StartIBSS(
    WlanContextImpl& aCtxImpl,
    const TSSID& /*aSSID*/,                 
    TUint32 /*aBeaconInterval*/,            
    TUint32 /*aAtim*/,                      
    TUint32 /*aChannel*/,                   
    TEncryptionStatus /*aEncryptionStatus*/ )
    {
    return CompleteMgmtCommand( aCtxImpl, KErrGeneral );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11MacError::Scan(
    WlanContextImpl& aCtxImpl,
    TScanMode /*aMode*/,
    const TSSID& /*aSSID*/,                
    TRate /*aScanRate*/,                   
    SChannels& /*aChannels*/,
    TUint32 /*aMinChannelTime*/,            
    TUint32 /*aMaxChannelTime*/,
    TBool /*aSplitScan*/ )
    {
    return CompleteMgmtCommand( aCtxImpl, KErrGeneral );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11MacError::StopScan( WlanContextImpl& aCtxImpl )
    {
    return CompleteMgmtCommand( aCtxImpl, KErrNone );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11MacError::Disconnect( WlanContextImpl& aCtxImpl )
    {
    return CompleteMgmtCommand( aCtxImpl, KErrNone );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11MacError::SetPowerMode(
    WlanContextImpl& aCtxImpl,
    TPowerMode /*aPowerMode*/,
    TBool /*aDisableDynamicPowerModeManagement*/,
    TWlanWakeUpInterval /*aWakeupModeInLightPs*/, 
    TUint8 /*aListenIntervalInLightPs*/,
    TWlanWakeUpInterval /*aWakeupModeInDeepPs*/,
    TUint8 /*aListenIntervalInDeepPs*/ )
    {
    return CompleteMgmtCommand( aCtxImpl, KErrNone );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11MacError::SetRcpiTriggerLevel(
    WlanContextImpl& aCtxImpl,
    TUint32 /*aRcpiTrigger*/ )          
    {
    return CompleteMgmtCommand( aCtxImpl, KErrNone );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11MacError::GetLastRcpi(
    WlanContextImpl& aCtxImpl )
    {
    return CompleteMgmtCommand( aCtxImpl, KErrGeneral );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11MacError::AddTkIPKey( 
    WlanContextImpl& aCtxImpl,
    const TUint8* /*aData*/, 
    TUint32 /*aLength*/,
    T802Dot11WepKeyId /*aKeyIndex*/,
    const TMacAddress& /*aMacAddr*/ )
    {
    return CompleteMgmtCommand( aCtxImpl, KErrNone );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11MacError::AddMulticastTKIPKey( 
    WlanContextImpl& aCtxImpl,
    T802Dot11WepKeyId /*aKeyIndex*/,
    TUint32 /*aLength*/,
    const TUint8* /*aData*/ )
    {
    return CompleteMgmtCommand( aCtxImpl, KErrNone );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11MacError::AddAesKey( 
    WlanContextImpl& aCtxImpl,
    const TUint8* /*aData*/, 
    TUint32 /*aLength*/,
    const TMacAddress& /*aMacAddr*/ )
    {
    return CompleteMgmtCommand( aCtxImpl, KErrNone );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11MacError::AddMulticastAesKey( 
    WlanContextImpl& aCtxImpl,
    T802Dot11WepKeyId /*aKeyIndex*/,
    TUint32 /*aLength*/,
    const TUint8* /*aData*/ )
    {
    return CompleteMgmtCommand( aCtxImpl, KErrNone );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11MacError::AddBroadcastWepKey(
    WlanContextImpl& aCtxImpl,
    TUint32 /*aKeyIndex*/,             
    TBool /*aUseAsDefaulKey*/,                
    TUint32 /*aKeyLength*/,                      
    const TUint8 /*aKey*/[KMaxWEPKeyLength],
    const TMacAddress& /*aMac*/ )
    {
    return CompleteMgmtCommand( aCtxImpl, KErrNone );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11MacError::AddUnicastWepKey(
    WlanContextImpl& aCtxImpl,
    const TMacAddress& /*aMacAddr*/,
    TUint32 /*aKeyLength*/,                      
    const TUint8 /*aKey*/[KMaxWEPKeyLength] )
    {
    return CompleteMgmtCommand( aCtxImpl, KErrNone );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11MacError::AddMulticastWapiKey( 
    WlanContextImpl& aCtxImpl,
    T802Dot11WepKeyId /*aKeyIndex*/,
    TUint32 /*aLength*/,
    const TUint8* /*aData*/ )
    {
    return CompleteMgmtCommand( aCtxImpl, KErrNone );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11MacError::AddUnicastWapiKey( 
    WlanContextImpl& aCtxImpl,
    const TUint8* /*aData*/, 
    TUint32 /*aLength*/,
    T802Dot11WepKeyId /*aKeyIndex*/,
    const TMacAddress& /*aMacAddr*/ )
    {
    return CompleteMgmtCommand( aCtxImpl, KErrNone );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11MacError::SetTxPowerLevel(
    WlanContextImpl& aCtxImpl,
    TUint32 /*aLevel*/ )
    {
    return CompleteMgmtCommand( aCtxImpl, KErrNone );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11MacError::AddMulticastAddr(
    WlanContextImpl& aCtxImpl,
    const TMacAddress& /*aMacAddr*/ )
    {
    return CompleteMgmtCommand( aCtxImpl, KErrNone );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11MacError::RemoveMulticastAddr(
    WlanContextImpl& aCtxImpl,
    TBool /*aRemoveAll*/,
    const TMacAddress& /*aMacAddr*/ )
    {
    return CompleteMgmtCommand( aCtxImpl, KErrNone );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11MacError::ConfigureBssLost( 
    WlanContextImpl& aCtxImpl,
    TUint32 /*aBeaconLostCount*/,
    TUint8 /*aFailedTxPacketCount*/ )
    {
    return CompleteMgmtCommand( aCtxImpl, KErrNone );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11MacError::SetTxRateAdaptParams( 
    WlanContextImpl& aCtxImpl,
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
    return CompleteMgmtCommand( aCtxImpl, KErrNone );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11MacError::ConfigureTxRatePolicies( 
    WlanContextImpl& aCtxImpl,
    const TTxRatePolicy& /*aRatePolicy*/,
    const TQueue2RateClass& /*aQueue2RateClass*/,
    const TInitialMaxTxRate4RateClass& /*aInitialMaxTxRate4RateClass*/,
    const TTxAutoRatePolicy& /*aAutoRatePolicy*/,
    const THtMcsPolicy& /*aHtMcsPolicy*/ )
    {
    return CompleteMgmtCommand( aCtxImpl, KErrNone );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11MacError::GetFrameStatistics( WlanContextImpl& aCtxImpl )
    {
    return CompleteMgmtCommand( aCtxImpl, KErrGeneral );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11MacError::ConfigureTxQueueIfNecessary( 
        WlanContextImpl& aCtxImpl,
        TQueueId /*aQueueId*/,
        TUint16 /*aMediumTime*/,
        TUint32 /*aMaxTxMSDULifetime*/ )
    {
    return CompleteMgmtCommand( aCtxImpl, KErrNone );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11MacError::GetMacAddress( WlanContextImpl& aCtxImpl )
    {
    return CompleteMgmtCommand( aCtxImpl, KErrGeneral );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11MacError::ConfigureArpIpAddressFiltering(
    WlanContextImpl& aCtxImpl,
    TBool /*aEnableFiltering*/,
    TIpv4Address /*aIpv4Address*/ )
    {
    return CompleteMgmtCommand( aCtxImpl, KErrNone );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool WlanDot11MacError::ConfigureHtBlockAck(
    WlanContextImpl& aCtxImpl, 
    TUint8 /*aTxBlockAckUsage*/,
    TUint8 /*aRxBlockAckUsage*/ )
    {
    return CompleteMgmtCommand( aCtxImpl, KErrNone );
    }

#ifndef NDEBUG 
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
const TInt8* WlanDot11MacError::GetStateName( 
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
TBool WlanDot11MacError::CompleteMgmtCommand(
    WlanContextImpl& aCtxImpl,
    TInt aStatus )
    {
    OnOidComplete( aCtxImpl, aStatus );
    // signal caller that no state transition ocurred
    return EFalse;        
    }
