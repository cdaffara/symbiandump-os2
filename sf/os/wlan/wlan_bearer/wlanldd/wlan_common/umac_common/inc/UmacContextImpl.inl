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
* Description:   Implementation of WlanContextImpl inline methods.
*
*/

/*
* %version: 79 %
*/

#include "umacconnectcontext.h"

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline WlanMacState& WlanContextImpl::CurrentState()
    {
    return *iCurrentMacState;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline WHA::SSettings& WlanContextImpl::WHASettings()
    {
    return iWHASettings;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline const WHA::SSettings& WlanContextImpl::WHASettings() const
    {
    return iWHASettings;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::WHASettings( 
    const WHA::SSettings& aSSettings )
    {
    iWHASettings = aSSettings;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline SAuthenticationFrame& WlanContextImpl::GetAuthenticationFrame()
    {
    return iManagementFrameTemplates.iAuthenticationFrame;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline SHtAuthenticationFrame& WlanContextImpl::GetHtAuthenticationFrame()
    {
    return iManagementFrameTemplates.iHtAuthenticationFrame;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline SAssociationRequestFrame& 
WlanContextImpl::GetAssociationRequestFrame() 
    {
    return  iManagementFrameTemplates.iAssociationRequestFrame;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline SHtAssociationRequestFrame& 
WlanContextImpl::GetHtAssociationRequestFrame() 
    {
    return  iManagementFrameTemplates.iHtAssociationRequestFrame;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline SDeauthenticateFrame& WlanContextImpl::GetDeauthenticateFrame() 
    {
    return iManagementFrameTemplates.iDeauthenticateFrame;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline SHtDeauthenticateFrame& WlanContextImpl::GetHtDeauthenticateFrame() 
    {
    return iManagementFrameTemplates.iHtDeauthenticateFrame;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline SDisassociateFrame& WlanContextImpl::GetDisassociationFrame()
    {
    return iManagementFrameTemplates.iDisassociationFrame;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline SHtDisassociateFrame& WlanContextImpl::GetHtDisassociationFrame()
    {
    return iManagementFrameTemplates.iHtDisassociationFrame;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline SReassociationRequestFrame& WlanContextImpl::GetReassociationRequestFrame()
    {
    return iManagementFrameTemplates.iReassociationRequestFrame;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline SHtReassociationRequestFrame& 
WlanContextImpl::GetHtReassociationRequestFrame()
    {
    return iManagementFrameTemplates.iHtReassociationRequestFrame;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline SNullDataFrame& WlanContextImpl::NullDataFrame()
    {
    return iNullDataFrame;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline SHtQosNullDataFrame& WlanContextImpl::QosNullDataFrame()
    {
    return iQosNullDataFrame;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TUint16 WlanContextImpl::QosNullDataFrameLength() const
    {
    return ( HtSupportedByNw() ? 
                sizeof( SHtQosNullDataFrame ) :
                sizeof( SQosNullDataFrame ) );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline SDataFrameHeader& WlanContextImpl::GetDataFrameHeader() 
    {
    return iDataFrameHeader;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TWlanUserTxDataCntx& WlanContextImpl::GetTxDataCntx()
    {
    return iTxDataCntx;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline const TWlanUserTxDataCntx& WlanContextImpl::GetTxDataCntx() const
    {
    return iTxDataCntx;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TWlanUserTxDataCntx& WlanContextImpl::GetMgmtTxDataCntx()
    {
    return iMgmtTxDataCntx;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::ResetAuthSeqNmbrExpected()
    {
    iAuthSeqNmbrExpected.Reset();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::TxRatePolicy(
    WHA::TQueueId aQueueId,
    TBool aUseSpecialRatePolicy,
    WHA::TRate& aRate, 
    TUint8& aPolicyId ) const
    {
    return iTxRateAdaptation.RatePolicy( 
        *this, 
        aQueueId, 
        aUseSpecialRatePolicy, 
        aRate, 
        aPolicyId );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TMacAddress& WlanContextImpl::GetBssId() 
    {
    return iConnectContext.iBSSID;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::FeaturesAllowed( TWlanFeatures aFeaturesAllowed )
    {
    iAllowedWlanFeatures = aFeaturesAllowed;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TWlanFeatures WlanContextImpl::FeaturesAllowed() const
    {
    return iAllowedWlanFeatures;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::Aid( const TUint32 aAid )
    {
    iConnectContext.iAid = aAid;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TUint32 WlanContextImpl::Aid() const
    {
    return iConnectContext.iAid;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::AtimWindow( const TUint32 aAtimWindow )
    {
    iConnectContext.iAtim = aAtimWindow;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TUint32 WlanContextImpl::AtimWindow() const
    {
    return iConnectContext.iAtim;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TSSID& WlanContextImpl::GetSsId()
    {
    return iConnectContext.iSSID;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TEncryptionStatus& WlanContextImpl::EncryptionStatus() 
    {
    return iConnectContext.iEncryptionStatus;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TUint16& WlanContextImpl::AuthenticationAlgorithmNumber()
    {
    return iConnectContext.iAuthAlgorithmNbr;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::OnTxCompleted( 
    const TUint32 aRate, 
    const TBool aSuccess,
    WHA::TQueueId aQueueId,
    WHA::TRate aRequestedRate )
    {
    if ( !( WHASettings().iCapability & 
         WHA::SSettings::KAutonomousRateAdapt ) )
        {
        // as autonomous rate adaptation is not being used, inform our own
        // rate adaptation object about Tx completion
        iTxRateAdaptation.OnTxCompleted( aRate, aSuccess, aQueueId, 
            aRequestedRate );        
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::CancelTimer()
    {
    iUmac.CancelTimeout();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TUint8 WlanContextImpl::GetAuthSeqNmbrExpected() const
    {
    return iAuthSeqNmbrExpected.GetAuthSeqNmbrExpected();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline SSupportedRatesIE& WlanContextImpl::GetOurSupportedRatesIE()
    {
    return iConnectContext.iOurSupportedRates;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline SExtendedSupportedRatesIE& WlanContextImpl::GetOurExtendedSupportedRatesIE()
    {
    return iConnectContext.iOurExtendedSupportedRates;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::SetScanResponseFrameBody( const TUint8* aBody )
    {
    iConnectContext.iScanResponseFrameBody = aBody;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline const TUint8* WlanContextImpl::ScanResponseFrameBody() const
    {
    return iConnectContext.iScanResponseFrameBody;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::SetScanResponseFrameBodyLength( TUint16 aLength )
    {
    iConnectContext.iScanResponseFrameBodyLength = aLength;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TUint16 WlanContextImpl::ScanResponseFrameBodyLength() const
    {
    return iConnectContext.iScanResponseFrameBodyLength;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::IeData( const TUint8* aIeData )
    {
    iConnectContext.iIeData = aIeData;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline const TUint8* WlanContextImpl::IeData() const
    {
    return iConnectContext.iIeData;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::IeDataLength( TUint16 aIeDataLength )
    {
    iConnectContext.iIeDataLength = aIeDataLength;    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TBool WlanContextImpl::RadioMeasurement() const
    {
    return iConnectContext.iRadioMeasurement;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::RadioMeasurement( TBool aRadioMeasurement )
    {
    iConnectContext.iRadioMeasurement = aRadioMeasurement;    
    }
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TUint16 WlanContextImpl::IeDataLength() const
    {
    return iConnectContext.iIeDataLength;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::SetActivePrivacyModeFilter(
    WlanContextImpl& /*aCtxImpl*/,                                                            
    TEncryptionStatus aEncryptionStatus )
    {
    iPrivacyModeFilters.SetActiveFilter( aEncryptionStatus );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TBool WlanContextImpl::ExecuteActivePrivacyModeFilter(
    const SDataFrameHeader& aFrameheader, 
    TBool aUserDataEnabled, 
    TUint16 aEthernetType,
    TBool aUnicastKeyExists, 
    TBool aAesOrTkipOrWapiEncrypted ) const
    {
    return iPrivacyModeFilters.ExecuteFilter(
        aFrameheader, 
        aUserDataEnabled, 
        aEthernetType,
        aUnicastKeyExists, 
        aAesOrTkipOrWapiEncrypted );    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline SCapabilityInformationField& 
WlanContextImpl::GetCapabilityInformation() 
    {
    return iConnectContext.iCapabilityInformation;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TBool WlanContextImpl::OnConsecutiveBeaconsLost()
    {
    TBool ret( EFalse );

    if ( !iConsecutiveBeaconsLostIndicated )
        {
        iConsecutiveBeaconsLostIndicated = ETrue;
        ret = ETrue;        
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TBool WlanContextImpl::OnConsecutiveTxFailures()
    {
    TBool ret( EFalse );

    if ( !iConsecutiveTxFailuresIndicated )
        {
        iConsecutiveTxFailuresIndicated = ETrue;
        ret = ETrue;        
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TBool WlanContextImpl::OnConsecutivePwrModeSetFailures()
    {
    TBool ret( EFalse );

    if ( !iConsecutivePwrModeSetFailuresIndicated )
        {
        iConsecutivePwrModeSetFailuresIndicated = ETrue;
        ret = ETrue;        
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::ResetBssLossIndications()
    {
    iConsecutiveBeaconsLostIndicated = EFalse;
    iConsecutiveTxFailuresIndicated = EFalse;
    iConsecutivePwrModeSetFailuresIndicated = EFalse;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline SSupportedRatesIE& WlanContextImpl::GetApSupportedRatesIE() 
    {
    return iConnectContext.iApSupportedRates;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline SExtendedSupportedRatesIE& WlanContextImpl::GetApExtendedSupportedRatesIE() 
    {
    return iConnectContext.iApExtendedSupportedRates;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::ResetTxRateAdaptation()
    {
    iTxRateAdaptation.Reset();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TUint32& WlanContextImpl::GetMinBasicRate()
    {
    return iConnectContext.iNwsaMinBasicRate;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TUint32& WlanContextImpl::GetMaxBasicRate()
    {
    return iConnectContext.iNwsaMaxBasicRate;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::ClearBasicRateSet()
    {
    iConnectContext.iNwsaBasicRateSet = 0;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::BasicRateSetBitSet( 
    const TUint32 aRateBitToSet )
    {
    iConnectContext.iNwsaBasicRateSet |= aRateBitToSet;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TUint32 WlanContextImpl::BasicRateSet() const
    {
    return iConnectContext.iNwsaBasicRateSet;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline WHA::TOperationMode WlanContextImpl::NetworkOperationMode() const
    {
    return iConnectContext.iOperationMode;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::NetworkOperationMode( 
    WHA::TOperationMode aOperationMode )
    {
    iConnectContext.iOperationMode = aOperationMode;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline const TPairwiseKeyData* WlanContextImpl::RoamingPairwiseKey() const
    {
    return iConnectContext.iRoamingPairwiseKey;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::RoamingPairwiseKey( 
    const TPairwiseKeyData* aKeyData )
    {
    iConnectContext.iRoamingPairwiseKey = aKeyData;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::GroupKeyType( 
    WHA::TKeyType aKeyType )
    {
    iConnectContext.iGroupKeyType = aKeyType;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline WHA::TKeyType WlanContextImpl::GroupKeyType() const
    {
    return iConnectContext.iGroupKeyType;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::PairWiseKeyType( 
    WHA::TKeyType aKeyType )
    {
    iConnectContext.iPairWiseKeyType = aKeyType;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline WHA::TKeyType WlanContextImpl::PairWiseKeyType() const
    {
    return iConnectContext.iPairWiseKeyType;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::PairwiseCipher( TWlanCipherSuite aCipherSuite )
    {
    iConnectContext.iPairwiseCipher = aCipherSuite;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TWlanCipherSuite WlanContextImpl::PairwiseCipher() const
    {
    return iConnectContext.iPairwiseCipher;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline WHA::TChannelNumber WlanContextImpl::NetworkChannelNumeber() const
    {
    return iConnectContext.iChannelNumber;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::NetworkChannelNumeber( 
    WHA::TChannelNumber aChannelNumber )
    {
    iConnectContext.iChannelNumber = aChannelNumber;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TUint32 WlanContextImpl::NetworkBeaconInterval() const
    {
    return iConnectContext.iBeaconInterval;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::NetworkBeaconInterval( 
    TUint32 aBeaconInterval )
    {
    iConnectContext.iBeaconInterval = aBeaconInterval;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline WHA::TPsMode WlanContextImpl::DesiredDot11PwrMgmtMode() const
    {
    return iConnectContext.iDesiredDot11PwrMgmtMode;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::DesiredDot11PwrMgmtMode( 
    WHA::TPsMode aPsMode )
    {
    iConnectContext.iDesiredDot11PwrMgmtMode = aPsMode;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline WHA::TPsMode WlanContextImpl::CurrentDot11PwrMgmtMode() const
    {
    return iConnectContext.iCurrentDot11PwrMgmtMode;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::CurrentDot11PwrMgmtMode( 
    WHA::TPsMode aPsMode )
    {
    iConnectContext.iCurrentDot11PwrMgmtMode = aPsMode;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline const TDot11PsModeWakeupSetting& 
    WlanContextImpl::DesiredPsModeConfig() const
    {
    return iConnectContext.iDesiredPsModeConfig;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::SetDesiredPsModeConfig(
    const TDot11PsModeWakeupSetting& aPsModeWakeupSetting )
    {
    iConnectContext.iDesiredPsModeConfig = aPsModeWakeupSetting;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline WHA::TPsMode WlanContextImpl::ClientDot11PwrMgmtMode() const
    {
    return iConnectContext.iClientDesiredDot11PwrMgtMode;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::ClientDot11PwrMgmtMode( 
    TPowerMode aPsMode )
    {
    iConnectContext.iClientDesiredDot11PwrMgtMode = 
        static_cast<WHA::TPsMode>(aPsMode);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline const TDot11PsModeWakeupSetting& 
    WlanContextImpl::ClientLightPsModeConfig() const
    {
    return iConnectContext.iClientLightPsModeConfig;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::SetClientLightPsModeConfig( 
    TWlanWakeUpInterval aWakeupMode, 
    TUint8 aListenInterval )
    {
    iConnectContext.iClientLightPsModeConfig.iWakeupMode = aWakeupMode;
    iConnectContext.iClientLightPsModeConfig.iListenInterval = aListenInterval;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline const TDot11PsModeWakeupSetting&
    WlanContextImpl::ClientDeepPsModeConfig() const
    {
    return iConnectContext.iClientDeepPsModeConfig;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::SetClientDeepPsModeConfig( 
    TWlanWakeUpInterval aWakeupMode, 
    TUint8 aListenInterval )
    {
    iConnectContext.iClientDeepPsModeConfig.iWakeupMode = aWakeupMode;
    iConnectContext.iClientDeepPsModeConfig.iListenInterval = aListenInterval;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TBool WlanContextImpl::DynamicPwrModeMgtDisabled() const
    {
    return ( iConnectContext.iFlags & 
             WlanConnectContext::KDynamicPwrModeMgmtDisabled );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TBool WlanContextImpl::UseShortPreamble() const
    {
    return (iConnectContext.iFlags & WlanConnectContext::KUseShortPreamble);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TBool WlanContextImpl::Reassociate() const
    {
    return (iConnectContext.iFlags & WlanConnectContext::KReassociate);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TBool WlanContextImpl::UseShortSlotTime() const
    {
    return (iConnectContext.iFlags & WlanConnectContext::KUseShortSlotTime);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TBool WlanContextImpl::ProtectionBitSet() const
    {
    return (iConnectContext.iFlags & WlanConnectContext::KProtectionBitSet);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TBool WlanContextImpl::QosEnabled() const
    {
    return (iConnectContext.iFlags & WlanConnectContext::KQosEnabled);
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TBool WlanContextImpl::UapsdEnabled() const
    {
    return (iConnectContext.iFlags & WlanConnectContext::KUapsdEnabled);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TBool WlanContextImpl::MulticastFilteringDisAllowed() const
    {
    return (iConnectContext.iFlags & WlanConnectContext::KMulticastFilteringDisAllowed);    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TBool WlanContextImpl::ErpIePresent() const
    {
    return (iConnectContext.iFlags & WlanConnectContext::KErpIePresent);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TBool WlanContextImpl::DisassociatedByAp() const
    {
    return (iConnectContext.iFlags & WlanConnectContext::KDisassociatedByAp);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TBool WlanContextImpl::UapsdRequestedForVoice() const
    {
    return ( iConnectContext.iFlags & 
             WlanConnectContext::KUapsdRequestedForVoice );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TBool WlanContextImpl::UapsdRequestedForVideo() const
    {
    return ( iConnectContext.iFlags & 
             WlanConnectContext::KUapsdRequestedForVideo );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TBool WlanContextImpl::UapsdRequestedForBestEffort() const
    {
    return ( iConnectContext.iFlags & 
             WlanConnectContext::KUapsdRequestedForBestEffort );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TBool WlanContextImpl::UapsdRequestedForBackground() const
    {
    return ( iConnectContext.iFlags & 
             WlanConnectContext::KUapsdRequestedForBackground );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TBool WlanContextImpl::UapsdUsedForVoice() const
    {
    return ( iConnectContext.iFlags & 
             WlanConnectContext::KUapsdUsedForVoice );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TBool WlanContextImpl::UapsdUsedForVideo() const
    {
    return ( iConnectContext.iFlags & 
             WlanConnectContext::KUapsdUsedForVideo );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TBool WlanContextImpl::UapsdUsedForBestEffort() const
    {
    return ( iConnectContext.iFlags & 
             WlanConnectContext::KUapsdUsedForBestEffort );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TBool WlanContextImpl::UapsdUsedForBackground() const
    {
    return ( iConnectContext.iFlags & 
             WlanConnectContext::KUapsdUsedForBackground );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TBool WlanContextImpl::ApTestOpportunitySeekStarted() const
    {
    return ( iConnectContext.iFlags & 
             WlanConnectContext::KApTestOpportunitySeekStarted );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TBool WlanContextImpl::ApTestOpportunityIndicated() const
    {
    return ( iConnectContext.iFlags & 
             WlanConnectContext::KApTestOpportunityIndicated );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TBool WlanContextImpl::HtSupportedByNw() const
    {
    return ( iConnectContext.iFlags & 
             WlanConnectContext::KHtSupportedByNw );    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TUint8 WlanContextImpl::WmmParameterSetCount() const
    {
    return iConnectContext.iWmmParamSetCount;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::WmmParameterSetCount( TUint8 aValue )
    {
    iConnectContext.iWmmParamSetCount = aValue;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TUint32 WlanContextImpl::RateBitMask() const
    {
    return iConnectContext.iRateBitMask;
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::RateBitMask( TUint32 aValue )
    {
    iConnectContext.iRateBitMask = aValue;
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TCwMinVector& WlanContextImpl::CwMinVector()
    {
    return iConnectContext.iCwMin;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TCwMaxVector& WlanContextImpl::CwMaxVector()
    {
    return iConnectContext.iCwMax;    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TAifsVector& WlanContextImpl::AifsVector()
    {
    return iConnectContext.iAIFS;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TTxOplimitVector& WlanContextImpl::TxOplimitVector()
    {
    return iConnectContext.iTxOplimit;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TAcmVector& WlanContextImpl::AcmVector()
    {
    return iConnectContext.iAdmCtrlMandatory;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline STxWmmIE& WlanContextImpl::OurWmmIe()
    {
    return iConnectContext.iOurWmmIe;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TTxRatePolicy& WlanContextImpl::RatePolicy()
    {
    return iConnectContext.iRatePolicy;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TQueue2RateClass& WlanContextImpl::Queue2RateClass()
    {
    return iConnectContext.iQueue2RateClass;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TInitialMaxTxRate4RateClass& 
WlanContextImpl::InitialMaxTxRate4RateClass()
    {
    return iConnectContext.iInitialMaxTxRate4RateClass;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TTxAutoRatePolicy& WlanContextImpl::AutoRatePolicy()
    {
    return iConnectContext.iAutoRatePolicy;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TUint8 WlanContextImpl::SpecialTxAutoRatePolicy() const
    {
    return iConnectContext.iSpecialTxRatePolicyId;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::SpecialTxAutoRatePolicy( TUint8 aPolicyId )
    {
    iConnectContext.iSpecialTxRatePolicyId = aPolicyId;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline THtMcsPolicy& WlanContextImpl::HtMcsPolicy()
    {
    return iConnectContext.iHtMcsPolicy;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TQosInfoUapsdMaxSpLen& WlanContextImpl::UapsdMaxSpLen()
    {
    return iConnectContext.iUapsdMaxSpLen;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TBool WlanContextImpl::WsaCmdActive() const
    {
    return iWsaCmdActive;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::ActivateWsaCmd()
    {
    iWsaCmdActive = ETrue;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::DeActivateWsaCmd()
    {
    iWsaCmdActive = EFalse;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::IncrementAuthSeqNmbrExpected()
    {
    ++iAuthSeqNmbrExpected;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TBool WlanContextImpl::SetTxRateAdaptationRates( 
    TUint8 aPolicyId,
    WHA::TRate aRateBitmask )
    {
    return iTxRateAdaptation.SetRates( aPolicyId, aRateBitmask );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::SetTxRatePolicy( WHA::TQueueId aQueueId, TUint8 aPolicyId )
    {
    iTxRateAdaptation.SetPolicy( aQueueId, aPolicyId );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::SetCurrentMaxTxRate( 
    TUint8 aPolicyId, 
    WHA::TRate aRate )
    {
    iTxRateAdaptation.SetCurrentMaxTxRate( aPolicyId, aRate );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::SetTxRateAdaptationAlgorithmParams( 
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
    iTxRateAdaptation.SetAlgorithmParameters(
        aMinStepUpCheckpoint,
        aMaxStepUpCheckpoint,
        aStepUpCheckpointFactor,
        aStepDownCheckpoint,
        aMinStepUpThreshold,
        aMaxStepUpThreshold,
        aStepUpThresholdIncrement,
        aStepDownThreshold,
        aDisableProbeHandling );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TUint32 WlanContextImpl::Random()
    {
    return iPrnGenerator.Generate();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TUint32 WlanContextImpl::WhaCommandAct() const
    {
    return iWhaCommandAct;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::WhaCommandAct( TUint32 aAct )
    {
    iWhaCommandAct = aAct;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::RegisterEvent( const TOIDHeader& aOid )
    {
    iEventDispatcher.Register( aOid );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::RegisterEvent( TInternalEvent aInternalEvent )
    {
    iEventDispatcher.Register( aInternalEvent );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TBool WlanContextImpl::ChannelEnabled( 
    WlanEventDispatcher::TChannel aChannel ) const
    {
    return iEventDispatcher.ChannelEnabled( aChannel );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::Enable( 
    WlanEventDispatcher::TChannel aChannelMask )
    {
    iEventDispatcher.Enable( aChannelMask );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::Disable( 
    WlanEventDispatcher::TChannel aChannelMask )
    {
    iEventDispatcher.Disable( aChannelMask );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TBool WlanContextImpl::DispatchEvent()
    {
    return iEventDispatcher.Dispatch();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TBool WlanContextImpl::CommandCompletionEventRegistered( 
    WHA::TCompleteCommandId aCommandId ) const
    {
    return iEventDispatcher.CommandCompletionRegistered( aCommandId );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TBool WlanContextImpl::DispatchCommandCompletionEvent()
    {
    return iEventDispatcher.DispatchCommandCompletionEvent();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TBool WlanContextImpl::ProtocolStackTxDataAllowed() const
    {
    return iEnableUserData;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TBool WlanContextImpl::PushPacketToPacketScheduler( 
    const TAny* aPacket, 
    TUint32 aLength, 
    WHA::TQueueId aQueueId,
    TUint32 aPacketId,
    const TDataBuffer* aMetaHeader,
    TBool aMore,
    TBool aMulticastData,
    TBool aUseSpecialRatePolicy )
    {
    return iPacketScheduler.Push( 
        *this, 
        aPacket, 
        aLength,
        aQueueId,
        aPacketId,
        aMetaHeader,
        aMore,
        aMulticastData,
        aUseSpecialRatePolicy );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::FlushPacketScheduler()
    {
    iPacketScheduler.Flush( *this );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::SchedulePackets( TBool aMore )
    {
    iPacketScheduler.SchedulePackets( *this, aMore );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TBool WlanContextImpl::UnsentTxPackets() const
    {
    return iPacketScheduler.UnsentPackets();
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::IncrementFailedTxPacketCount()
    {
    ++iFailedTxPacketCount;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TUint8 WlanContextImpl::FailedTxPacketCount() const
    {
    return iFailedTxPacketCount;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::ResetFailedTxPacketCount()
    {
    iFailedTxPacketCount = 0;
    }

// ---------------------------------------------------------------------------
// We use the failed Tx packet count threshold as the threshold value
// in this case, too - as informing AP about a power mgmt mode change
// also involves a frame Tx
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::InitializeSetPsModeCount()
    {
    iSetPsModeCount = iWlanMib.iFailedTxPacketCountThreshold;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::DecrementSetPsModeCount()
    {
    --iSetPsModeCount;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TUint WlanContextImpl::SetPsModeCount() const
    {
    return iSetPsModeCount;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::StartPowerModeManagement()
    {
    iDynamicPowerModeCntx.StartPowerModeManagement();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::StopPowerModeManagement()
    {
    iDynamicPowerModeCntx.StopPowerModeManagement();
    }
                
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TPowerMgmtModeChange WlanContextImpl::OnFrameTx( 
    WHA::TQueueId aQueueId,
    TUint16 aEtherType,
    T802Dot11FrameControlTypeMask aDot11FrameType )
    {
    return iDynamicPowerModeCntx.OnFrameTx( 
        aQueueId, 
        aEtherType, 
        aDot11FrameType );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TPowerMgmtModeChange WlanContextImpl::OnFrameRx(
    WHA::TQueueId aAccessCategory,
    TUint16 aEtherType,
    TUint aPayloadLength,
    TDaType aDaType )
    {
    return iDynamicPowerModeCntx.OnFrameRx( 
        aAccessCategory, 
        aEtherType,
        aPayloadLength, 
        aDaType );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TPowerMgmtModeChange WlanContextImpl::OnPsModeErrorIndication()
    {
    return iDynamicPowerModeCntx.OnPsModeErrorIndication();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::SetPowerModeManagementParameters(
    TUint32 aToLightPsTimeout,
    TUint16 aToLightPsFrameThreshold,
    TUint32 aToActiveTimeout,
    TUint16 aToActiveFrameThreshold,
    TUint32 aToDeepPsTimeout,
    TUint16 aToDeepPsFrameThreshold,
    TUint16 aUapsdRxFrameLengthThreshold )
    {
    iDynamicPowerModeCntx.SetParameters(
        aToLightPsTimeout,
        aToLightPsFrameThreshold,
        aToActiveTimeout,
        aToActiveFrameThreshold,
        aToDeepPsTimeout,
        aToDeepPsFrameThreshold,
        aUapsdRxFrameLengthThreshold );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::ConfigurePwrModeMgmtTrafficOverride( 
    TBool aStayInPsDespiteUapsdVoiceTraffic,
    TBool aStayInPsDespiteUapsdVideoTraffic,
    TBool aStayInPsDespiteUapsdBestEffortTraffic, 
    TBool aStayInPsDespiteUapsdBackgroundTraffic,
    TBool aStayInPsDespiteLegacyVoiceTraffic,
    TBool aStayInPsDespiteLegacyVideoTraffic,
    TBool aStayInPsDespiteLegacyBestEffortTraffic,
    TBool aStayInPsDespiteLegacyBackgroundTraffic )
    {
    iDynamicPowerModeCntx.ConfigureTrafficOverride( 
        aStayInPsDespiteUapsdVoiceTraffic,
        aStayInPsDespiteUapsdVideoTraffic,
        aStayInPsDespiteUapsdBestEffortTraffic, 
        aStayInPsDespiteUapsdBackgroundTraffic,
        aStayInPsDespiteLegacyVoiceTraffic,
        aStayInPsDespiteLegacyVideoTraffic,
        aStayInPsDespiteLegacyBestEffortTraffic,
        aStayInPsDespiteLegacyBackgroundTraffic );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::FreezePwrModeMgmtTrafficOverride()
    {
    iDynamicPowerModeCntx.FreezeTrafficOverride();
    }
                    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::RestorePwrModeMgmtParameters()
    {
    iDynamicPowerModeCntx.RestoreActiveModeParameters();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TBool WlanContextImpl::OnActiveToLightPsTimerTimeout()
    {
    return iDynamicPowerModeCntx.OnActiveToLightPsTimerTimeout();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TBool WlanContextImpl::OnLightPsToActiveTimerTimeout()
    {
    return iDynamicPowerModeCntx.OnLightPsToActiveTimerTimeout();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TBool WlanContextImpl::OnLightPsToDeepPsTimerTimeout()
    {
    return iDynamicPowerModeCntx.OnLightPsToDeepPsTimerTimeout();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline WlanContextImpl::TGroupAddStatus WlanContextImpl::AddMulticastAddress( 
        const TMacAddress& aMacAddress )
    {
    return iJoinedMulticastGroups.AddGroup( aMacAddress );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TBool WlanContextImpl::RemoveMulticastAddress( const TMacAddress& aMacAddress )
    {
    return iJoinedMulticastGroups.RemoveGroup( aMacAddress );    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TUint8 WlanContextImpl::MulticastAddressCount() const
    {
    return iJoinedMulticastGroups.Count();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TUint8 WlanContextImpl::GetMulticastAddresses( 
    const TMacAddress*& aMacAddresses ) const
    {
    return iJoinedMulticastGroups.GetGroups( aMacAddresses );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::ResetMulticastAddresses()
    {
    iJoinedMulticastGroups.Reset();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::MarkInternalTxBufFree()
    {
    iInternalTxBufFree = ETrue;

    OsTracePrint( KUmacDetails, (TUint8*)
        ("UMAC: WlanContextImpl::MarkInternalTxBufFree: buffer free again"));
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TBool WlanContextImpl::InternalTxBufBeingWaited() const
    {
    return iInternalTxBufBeingWaited;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::ClearInternalTxBufBeingWaitedFlag()
    {
    iInternalTxBufBeingWaited = EFalse;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::ResetFrameStatistics()
    {
    os_memset( &iFrameStatistics, 0, sizeof( iFrameStatistics ) );    
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::IncrementRxUnicastDataFrameCount( 
    WHA::TQueueId aAccessCategory )
    {
    ++iFrameStatistics.acSpecific[aAccessCategory].rxUnicastDataFrameCount;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::IncrementTxUnicastDataFrameCount(
    WHA::TQueueId aAccessCategory )
    {
    ++iFrameStatistics.acSpecific[aAccessCategory].txUnicastDataFrameCount;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::IncrementRxMulticastDataFrameCount(
    WHA::TQueueId aAccessCategory )
    {
    ++iFrameStatistics.acSpecific[aAccessCategory].rxMulticastDataFrameCount;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::IncrementTxMulticastDataFrameCount(
    WHA::TQueueId aAccessCategory)
    {
    ++iFrameStatistics.acSpecific[aAccessCategory].txMulticastDataFrameCount;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::IncrementTxRetryCount( 
    WHA::TQueueId aAccessCategory,
    TUint aCount )
    {
    iFrameStatistics.acSpecific[aAccessCategory].txRetryCount += aCount;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::IncrementTxErrorCount( 
    WHA::TQueueId aAccessCategory )
    {
    ++iFrameStatistics.acSpecific[aAccessCategory].txErrorCount;
    }

// ---------------------------------------------------------------------------
// Note! We use the Tx Media Delay field first to collect the
// cumulative Media Delay. The average Media Delay is then calculated at the 
// point of reporting the frame statistics results to WLAN Mgmt client
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::IncrementTxMediaDelay( 
    WHA::TQueueId aAccessCategory,
    TUint aDelay )
    {
    iFrameStatistics.acSpecific[aAccessCategory].txMediaDelay += aDelay;
    }

// ---------------------------------------------------------------------------
// Note! We use the Total Tx Delay field first to collect the
// cumulative Total Tx Delay. The average Total Tx Delay is then calculated 
// at the point of reporting the frame statistics results to WLAN Mgmt client
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::IncrementTotalTxDelay( 
    WHA::TQueueId aAccessCategory,
    TUint aDelay )
    {
    iFrameStatistics.acSpecific[aAccessCategory].totalTxDelay += aDelay;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::StoreFcsErrorCount( TUint aCount )
    {
    iFrameStatistics.fcsErrorCount = aCount;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline const TStatisticsResponse& WlanContextImpl::FrameStatistics() const
    {
    return iFrameStatistics;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::SetNullSendControllerParameters(
    TUint32 aVoiceCallEntryTimeout,
    TUint32 aVoiceCallEntryTxThreshold,
    TUint32 aNullTimeout,
    TUint32 aNoVoiceTimeout,
    TUint32 aKeepAliveTimeout )
    {
    iNullSendController.SetParameters( 
        aVoiceCallEntryTimeout,
        aVoiceCallEntryTxThreshold,
        aNullTimeout, 
        aNoVoiceTimeout, 
        aKeepAliveTimeout );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::StartVoiceOverWlanCallMaintenance()
    {
    iNullSendController.StartVoiceOverWlanCallMaintenance();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::StopVoiceOverWlanCallMaintenance()
    {
    iNullSendController.StopVoiceOverWlanCallMaintenance();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::TerminateVoiceOverWlanCallMaintenance()
    {
    iNullSendController.TerminateVoiceOverWlanCallMaintenance();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::ResumeQosNullSending()
    {
    iNullSendController.ResumeQosNullSending();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::StartKeepAlive()
    {
    iNullSendController.StartKeepAlive();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::StopKeepAlive()
    {
    iNullSendController.StopKeepAlive();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::OnDataRxCompleted( 
    WHA::TQueueId aQueueId,
    TUint aPayloadLength )
    {
    iNullSendController.OnFrameRx( aQueueId, aPayloadLength );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::OnDataTxCompleted( WHA::TQueueId aQueueId )
    {
    iNullSendController.OnFrameTx( aQueueId );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::OnQosNullDataTxCompleted()
    {
    iNullSendController.OnQosNullDataTxCompleted();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::OnNullDataTxCompleted()
    {
    iNullSendController.OnNullDataTxCompleted();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::OnVoiceCallEntryTimerTimeout()
    {
    iNullSendController.OnVoiceCallEntryTimerTimeout();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::OnNullTimerTimeout()
    {
    iNullSendController.OnNullTimerTimeout();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::OnNoVoiceTimerTimeout()
    {
    iNullSendController.OnNoVoiceTimerTimeout();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::OnKeepAliveTimerTimeout()
    {
    iNullSendController.OnKeepAliveTimerTimeout();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TBool WlanContextImpl::InVoiceCallState() const
    {
    return iNullSendController.InVoiceCallState();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TBool WlanContextImpl::InsertNewRcpiIntoPredictor( 
    TInt64 aTimestamp, 
    WHA::TRcpi aRcpi )
    {
    return iWlanSignalPredictor.InsertNewRcpi( 
        // this cast is ok as the predictor is prepared for the possibility 
        // of the lower (TUint32) part of the timestamp rolling around
        static_cast<TUint32>(aTimestamp), 
        aRcpi );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TBool WlanContextImpl::GetLatestMedianRcpiFromPredictor( 
    TInt64 aTimestamp,
    WHA::TRcpi& aLatestMedian ) const
    {
    return iWlanSignalPredictor.GetLatestMedian( 
        // this cast is ok as the predictor is prepared for the possibility 
        // of the lower (TUint32) part of the timestamp rolling around
        static_cast<TUint32>(aTimestamp),
        aLatestMedian );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void WlanContextImpl::ConfigureWlanSignalPredictor(
    TUint32 aTimeToWarnLevel,
    TUint32 aTimeToNextInd,
    WHA::TRcpi aRcpiWarnLevel)
    {
    iWlanSignalPredictor.ConfigureSignalPredictor( 
        aTimeToWarnLevel,
        aTimeToNextInd,
        aRcpiWarnLevel );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline void WlanContextImpl::ClearBssMembershipFeatureList()
    {
    os_memset( 
        &iOurBssMembershipFeatureArray, 
        KUnallocated, 
        sizeof( iOurBssMembershipFeatureArray ) );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline SHtCapabilitiesIE& WlanContextImpl::GetOurHtCapabilitiesIe()
    {
    return iOurHtCapabilitiesIe;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline SHtCapabilitiesIE& WlanContextImpl::GetNwHtCapabilitiesIe()
    {
    return iConnectContext.iNwHtCapabilitiesIe;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline SHtOperationIE& WlanContextImpl::GetNwHtOperationIe()
    {
    return iConnectContext.iNwHtOperationIe;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline WHA::ShtBlockAckConfigure& WlanContextImpl::GetHtBlockAckConfigure()
    {
    return iHtBlockAckConfigure;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline SSnapHeader& WlanContextImpl::GetProprietarySnapHeader()
    {
    return iProprietarySnapHeader;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TBool WlanContextImpl::GetWhaTxStatus( 
    TWhaTxQueueState& aTxQueueState ) const
    {
    return iPacketScheduler.GetWhaTxStatus( *this, aTxQueueState );
    }
