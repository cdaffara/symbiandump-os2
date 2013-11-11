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
* Description:  Handles type conversion between adaptation and core layers
*
*/

/*
* %version: 35 %
*/

#ifndef WLANCONVERSIONUTIL_H
#define WLANCONVERSIONUTIL_H

#include <wdbifwlansettings.h>  // Used in SIAPSettings.
#include <wdbiflansettings.h>   // Used in SLanSettings.
#include <wlancontrolinterface.h>
#include <wlantrafficstreamparameters.h>
#include "FrameXferBlock.h"
#include "wlmclientserver.h"
#include "core_types.h"
#include "core_type_list.h"
#include "wlandevicesettings.h"
#include "umac_types.h"
#include "wlanpowersaveinterface.h"
#include "wlanhwinit.h"

/**
* Utility class for making type conversion between Symbian and "Wlan Core" types
* @lib wlmserversrv.lib
* @since Series 60 3.0
*/
class TWlanConversionUtil
    {
    public:  // New functions

        /**
        * ConvertSSID
        * @since Series 60 3.0
        * @param aCoreSSID (OUT)
        * @param aSSID (IN)
        */    
        static void ConvertSSID(
            core_ssid_s& aCoreSSID,
            const TSSID& aSSID );

        /**
        * ConvertSSID
        * @since Series 60 3.0
        * @param aCoreSSID (OUT)
        * @param aSSID (IN)
        */    
        static void ConvertSSID(
            core_ssid_s& aCoreSSID,
            const TDesC& aSSID );

        /**
        * ConvertSSID
        * @since Series 60 3.0
        * @param aCoreSSID (OUT)
        * @param aSSID (IN)
        */    
        static void ConvertSSID(
            core_ssid_s& aCoreSSID,
            const TDesC8& aSSID );

        /**
        * ConvertSSID
        * @since Series 60 3.0
        * @param aCoreSSID
        * @
        */    
        static void ConvertSSID(
            TSSID& aAmSSID,
            const core_ssid_s& aCoreSSID );
        
        /**
        * ConvertWpaPreSharedKey
        * @since Series 60 3.0
        * @param aWpaPreSharedKey (OUT)
        * @param aWspPin (IN)
        */    
        static void ConvertWpaPreSharedKey(
            core_wpa_preshared_key_s& aWpaPreSharedKey,
            const TDesC8& aWspPin );

        /**
        * ConvertScanRate
        * @since Series 60 3.0
        * @param aCoreScanRate
        * @return TRate
        */    
        static TRate ConvertScanRate( int_t aCoreScanRate );

        /**
        * ConvertSecurityMode
        * @since Series 60 3.0
        * @param aCoreMode
        * @return TWlanSecurity
        */    
        static TWlanSecurity ConvertSecurityMode(
            const core_connection_security_mode_e& aCoreMode );

        /**
        * ConvertEncryptionMode
        * @since Series 60 3.0
        * @param aCoreEncryptionMode
        * @return TWEPStatus
        */    
        static TEncryptionStatus ConvertEncryptionMode(
            const core_encryption_mode_e& aCoreEncryptionMode );

        /**
        * ConvertConnectionState
        * @since Series 60 3.0
        * @param aCoreState
        * @return TWlanConnectionState 
        */   
        static TWlanConnectionState ConvertConnectionState(
            const core_connection_state_e& aCoreState );

        /**
        * ConvertScanMode
        * @since Series 60 3.0
        * @param aAmScanMode
        * @return core_scan_mode_e 
        */   
        static core_scan_mode_e ConvertScanMode(
            const TScanMode& aAmScanMode );

        /**
        * ConvertScanMode
        * @since Series 60 3.0
        * @param aCoreScanMode
        * @return TScanMode 
        */   
        static TScanMode ConvertScanMode(
            const core_scan_mode_e& aCoreScanMode );

        /**
        * ConvertConvertCipherKeyType
        * @since Series 60 3.0
        * @param aCoreCipherKeyType
        * @return TWlanCipherSuite
        */   
        static TWlanCipherSuite ConvertCipherKeyType(
            const core_cipher_key_type_e& aCoreCipherKeyType );

        /**
        * ConvertNotificationType
        * @since Series 60 3.0
        * @param aCoreNotificationType
        * @return TWlmNotify 
        */   
        static TWlmNotify ConvertNotificationType(
            const core_notification_e& aCoreNotificationType );

        /**
        * ConvertConnectStatus
        * @since Series 60 3.0
        * @param aCoreConnectStatus The core connect status being converted.
        * @param security_mode Security mode of the IAP.
        * @return error code either Symbian generic or WLAN specific 
        */   
        static TInt ConvertConnectStatus(
            const core_connect_status_e& aCoreConnectStatus,
            core_security_mode_e security_mode );
        
        /**
        * ConvertRegion
        * @since Series 60 3.0
        * @param aAmRegion
        * @return core_wlan_region_e 
        */   
        static core_wlan_region_e ConvertRegion( const TWlanRegion& aAmRegion );

        /**
         * ConvertRegion
         * @since Series 60 5.0
         * @param aCoreRegion
         * @return TWlanRegion 
         */   
         static TWlanRegion ConvertRegion( const core_wlan_region_e& aCoreRegion );
        
        /**
        * ConvertMacAddress
        * Function assumes both mac address types have the same length
        * @since Series 60 3.0
        * @param aAmMacAddress (OUT)
        * @param aCoreMacAddress (IN)
        */   
        static void ConvertMacAddress(
            TMacAddress& aAmMacAddress,
            const core_mac_address_s& aCoreMacAddress );

        /**
        * ConvertMacAddress
        * Function assumes both mac address types have the same length
        * @since Series 60 3.0
        * @param aCoreMacAddress (OUT)
        * @param aAmMacAddress (IN)
        */   
        static void ConvertMacAddress(
            core_mac_address_s& aCoreMacAddress,
            const TMacAddress& aAmMacAddress );

        /**
        * ConvertMacAddress
        * Function assumes both mac address types have the same length
        * @since Series 60 3.0
        * @param aCoreMacAddress (OUT)
        * @param aAmMacAddress (IN)
        */   
        static void ConvertMacAddress(
            core_mac_address_s& aCoreMacAddress,
            const TMacAddr& aAmMacAddress );

        /**
        * ConvertMacAddress
        * Function assumes both mac address types have the same length
        * @since Series 60 3.0
        * @param aCoreMacAddress (OUT)
        * @param aAmMacAddress (IN)
        */   
        static void ConvertMacAddress(
            TMacAddress& aCoreMacAddress,
            const TMacAddr& aAmMacAddress );

        /**
        * ConvertPowerMode
        * @since Series 60 3.0
        * @param aCorePowerMode (IN)
        * @return TPowerMode
        */        
        static TPowerMode TWlanConversionUtil::ConvertPowerMode(
            const core_power_mode_e& aCorePowerMode );

        /**
        * ConvertWakeUpInterval
        * @since Series 60 3.0
        * @param aAmWakeUpMode (OUT)
        * @param aAmWakeUpInterval (OUT)
        * @param aCoreWakeUpMode (OUT)
        * @param aCoreWakeUpInterval (IN)
        */        
        static void TWlanConversionUtil::ConvertWakeUpInterval(
            TWlanWakeUpInterval& aAmWakeUpMode,
            TUint8& aAmWakeUpInterval,
            const core_power_save_wakeup_mode_e& aCoreWakeUpMode,
            const u8_t& aCoreWakeUpInterval );

        /**
        * ConvertIndication
        * @since Series 60 3.0
        * @param aAmIndication
        * @return core_am_indication_e 
        */   
        static core_am_indication_e ConvertIndication(
            const TIndication& aAmIndication );

        /**
        * ConvertDeviceSettings
        * @since Series 60 3.0
        * @param aCoreSettings (OUT)
        * @param aAmSettings (IN)
        */    
        static void ConvertDeviceSettings(
            core_device_settings_s& aCoreSettings,
            const CWlanDeviceSettings::SWlanDeviceSettings& aAmSettings );
            
        /**
        * ConvertIapSettings
        * @since Series 60 3.0
        * @param aCoreSettings (OUT)
        * @param aAmSettings (IN)
        * @param aDhcpInUse (IN)
        * @param aOverrideSettings (IN)
        */    
        static void ConvertIapSettings(
            core_iap_data_s& aCoreSettings,
            const SWLANSettings& aAmSettings,
            const TBool aDhcpInUse,
            const TWLMOverrideSettings& aOverrideSettings );

        /**
        * ConvertIapSettings
        * @since Series 60 3.2
        * @param aCoreSettings (OUT)
        * @param aAmSettings (IN)
        */    
        static void ConvertIapSettings(
            core_iap_data_s& aCoreSettings,
            const TWlanLimitedIapData& aAmSettings );

        /**
        * ConvertErrorCode
        * @since Series 60 3.0
        * @param aError
        * @return Corresponding Adaptation module error code.
        */    
        static TInt ConvertErrorCode( const core_error_e aError );

        /**
        * ConvertErrorCode
        * @since Series 60 3.0
        * @param aError
        * @return Corresponding Core error code.
        */    
        static core_error_e ConvertErrorCode( const TInt aError );
    
        /**
        * Convert mW to dBm.
        * @since Series 60 3.0
        * @param aValue mW value to convert.
        * @return Corresponding value in dBm.
        * @note If the method is unable to perform conversion,
        * it will return 0dBm as the conversion result.
        */
        static u32_t ConvertMilliwattsToDecibel( const TUint32 aValue );

        /**
        * Convert dBm to mW.
        * @since Series 60 3.0
        * @param aValue dBm value to convert.
        * @return Corresponding value in mW.
        * @note If the method is unable to perform conversion,
        * it will return 1mW as the conversion result.
        */
        static TUint32 ConvertDecibelToMilliWatts( const u32_t aValue );

        /**
        * Convert RSS class
        * @since Series 60 3.0
        * @param aCoreRssClass.
        * @return Corresponding Adaptation module value.
        */
        static TWlanRCPLevel ConvertRcpClass( const core_rcp_class_e aValue );

        /**
        * Convert scan channels
        * @since Series 60 3.0
        * @param aAmChannels (OUT)
        * @param aCoreChannels (IN)
        */
        static void ConvertScanChannels(
            SChannels& aAmChannels,
            const core_scan_channels_s& aCoreChannels );
            
        /**
        * Convert scan channels
        * @since Series 60 3.0
        * @param aCoreChannels (OUT)
        * @param aAmChannels (IN)
        */
        static void ConvertScanChannels(            
            core_scan_channels_s& aCoreChannels,
            RArray<TUint>& aAmChannels );
            
        /**
        * Convert frame type
        * @since Series 60 3.1
        * @param core frame type
        * @return adaptation frame type
        */
        static TDataBuffer::TFrameType ConvertFrameType(
            core_frame_type_e aFrameType );

        /**
        * Convert frame type
        * @since Series 60 3.1
        * @param adaptation frame type
        * @return core frame type
        */
        static core_frame_type_e ConvertFrameType(
            TDataBuffer::TFrameType aFrameType );

        /**
         * Convert packet statistics.
         * 
         * @since Series 60 3.2
         * @param aAmStatistics (OUT)
         * @param aCoreStatistics (IN)         
         */
        static void ConvertPacketStatistics(
            TWlanPacketStatistics& aAmStatistics,
            const core_packet_statistics_s& aCoreStatistics );

        /**
         * Convert U-APSD settings.
         * 
         * @since Series 60 3.2
         * @param aCoreSettings (OUT)
         * @param aAmSettings (IN)         
         */
        static void ConvertUapsdSettings(
            core_uapsd_settings_s& aCoreSettings,
            const TWlanUapsdSettings& aAmSettings );

        /**
         * Convert U-APSD settings.
         * 
         * @since Series 60 3.2
         * @param aAmSettings (OUT)
         * @param aCoreSettings (IN)         
         */
        static void ConvertUapsdSettings(
            TWlanUapsdSettings& aAmSettings,
            const core_uapsd_settings_s& aCoreSettings );

        /**
         * Convert TX rate policies.
         * 
         * @since Series 60 3.2
         * @param aAmPolicies (OUT)
         * @param aAmMcsPolicies (OUT)
         * @param aAmAutoRatePolicies (OUT)
         * @param aCorePolicies (IN)         
         */
        static void ConvertTxRatePolicies(
            TTxRatePolicy& aAmPolicies,
            THtMcsPolicy& aAmMcsPolicies,
            TTxAutoRatePolicy& aAmAutoRatePolicies,
            const core_tx_rate_policies_s& aCorePolicies );

        /**
         * Convert TX rate policy mappings.
         * 
         * @since Series 60 3.2
         * @param aAmMappings (OUT)
         * @param aCoreMappings (IN)         
         */
        static void ConvertTxRatePolicyMappings(
            TQueue2RateClass& aAmMappings,
            const core_tx_rate_policy_mappings_s& aCoreMappings );

        /**
         * Convert TX rate policy initial rates.
         * 
         * @since Series 60 3.2
         * @param aInitialRates (OUT)
         * @param aCorePolicies (IN)         
         */
        static void ConvertTxRatePolicyInitialRates(
            TInitialMaxTxRate4RateClass& aInitialRates,
            const core_tx_rate_policies_s& aCorePolicies );

        /**
         * Convert power save settings.
         * 
         * @since Series 60 3.2
         * @param aCoreSettings (OUT)
         * @param aAmSettings (IN)         
         */
        static void ConvertPowerSaveSettings(
            core_power_save_settings_s& aCoreSettings,
            const TWlanPowerSaveSettings& aAmSettings );

        /**
         * Convert power save settings.
         * 
         * @since Series 60 3.2
         * @param aAmSettings (OUT)
         * @param aCoreSettings (IN)         
         */
        static void ConvertPowerSaveSettings(
            TWlanPowerSaveSettings& aAmSettings,
            const core_power_save_settings_s& aCoreSettings );

        /**
         * Convert U-APSD maximum service period length
         * @since Series 60 3.2
         * @param aServicePeriodLength (IN)
         * @return Corresponding Adaptation module maximum service period length
         */
        static TMaxServicePeriodLength ConvertUapsdMaxServicePeriod(
            core_max_service_period_length_e aServicePeriodLength );
            
        /**
         * Convert traffic stream status.
         * @since Series 60 3.2
         * @param aStatus (IN)
         * @return Corresponding Adaptation module traffic stream status.
         */
        static TWlanTrafficStreamStatus ConvertTrafficStreamStatus(
            core_traffic_stream_status_e aStatus );        

        /**
         * Convert access point information.
         * @since Series 60 3.2
         * @param aAmInfo (OUT)
         * @param aCoreInfo (IN)
         */
        static void ConvertApInformation(
            TWlanAccessPointInfo& aAmInfo,
            const core_ap_information_s& aCoreInfo );

        /**
         * Convert roam metrics of the current connection.
         * @since Series 60 3.2
         * @param aAmRoamMetrics (OUT)
         * @param aCoreRoamMetrics (IN)
         */
        static void ConvertRoamMetrics(
            TWlanRoamMetrics& aAmRoamMetrics,
            const core_roam_metrics_s& aCoreRoamMetrics );

        /**
         * Convert list of BSSIDs on the rogue list.
         * @since Series 60 3.2
         * @param aAmRogueList (OUT)
         * @param aCoreRogueList (IN)
         */ 
        static void ConvertRogueList(
            TWlmRogueList& aAmRogueList,
            core_type_list_c<core_mac_address_s>& aCoreRogueList );

        /**
         * Convert tx queue id.
         * @since Series 60 3.2
         * @param queue_id (IN)
         * @return Corresponding Adaptation module tx queue id.
         */
        static TQueueId ConvertTxQueueId(
            core_access_class_e queue_id );

        /**
         * Convert a Protected Setup credential attribute.
         * @since S60 3.2
         * @param aAmAttribute (OUT)
         * @param aCoreAttribute (IN)
         */
        static void ConvertProtectedSetupCredentialAttribute(
            TWlanProtectedSetupCredentialAttribute& aAmAttribute,
            const core_iap_data_s& aCoreAttribute );

        /**
         * Convert a Protected Setup status code to a WLAN error code.
         * @since Series 60 3.2
         * @param aCoreStatus The core Protected Setup status being converted.
         * @return WLAN specific error code.
         */
        static TInt ConvertProtectedSetupStatus(
            core_protected_setup_status_e aCoreStatus );

        /**
         * Convert SNAP header.
         * @since Series 60 3.2
         * @param aAmHeader (OUT)
         * @param aCoreHeader (IN)
         */
        static void ConvertSnapHeader(
            TSnapHeader& aAmHeader,
            const core_snap_header_s& aCoreHeader );        

        /**
         * Convert power save mode.
         * 
         * @since Series 60 5.0.1
         * @param aCoreMode (OUT)
         * @param aAmMode (IN)         
         */
        static void ConvertPowerSaveMode(
            core_power_save_mode_s& aCoreMode,
            TWlanPowerSaveMode aAmMode );        

        /**
         * Convert feature flags.
         *
         * @since S60 v5.1
         * @param aStaticFeatures Bitmask of statically enabled features.
         * @param aDynamicFeatures Bitmask of dynamically enabled features.
         * @return Corresponding core features.
         */
        static u32_t ConvertFeatureFlags(
            TUint aStaticFeatures,
            TUint aDynamicFeatures );

        /**
         * Convert feature flags.
         *
         * @since S60 v5.1
         * @param aCoreFlags Bitmask enabled features.
         * @return Corresponding UMAC features.
         */        
        static TUint ConvertFeatureFlags(
            u32_t aCoreFlags );

        /**
         * Convert access class.
         * 
         * @param aAccessClass (IN)
         * @return Corresponding Adaptation module access class.
         */
        static TWlmAccessClass ConvertAccessClass(
            core_access_class_e aAccessClass );

        /**
         * Convert traffic mode.
         * 
         * @param aAccessClass (IN)
         * @return Corresponding Adaptation module traffic mode.
         */
        static TWlmAcTrafficMode ConvertTrafficMode(
            core_access_class_traffic_mode_e aMode );

        /**
         * Convert traffic status.
         * 
         * @param aAccessClass (IN)
         * @return Corresponding Adaptation module traffic status
         */
        static TWlmAcTrafficStatus ConvertTrafficStatus(
            core_access_class_traffic_status_e aStatus );

        /**
         * Convert traffic stream parameters.
         * 
         * @param aCoreTid (OUT)
         * @param aCoreUserPriority (OUT)
         * @param coreParams (OUT)
         * @param aAmParams (IN)         
         */
        static void ConvertTrafficStreamParameters(
            u8_t& aCoreTid,
            u8_t& aCoreUserPriority,
            core_traffic_stream_params_s& aCoreParams,
            const TWlanTrafficStreamParameters& aAmParams );

        /**
         * Convert TX rate enumeration to corresponding rate as bits per second.
         *
         * @since aRate (IN)
         * @return Corresponding rate as bits per second.
         */
        static u32_t ConvertTxRate(
            TWlanRate aRate );

        /**
         * Convert cipher key data.
         */
        static void ConvertCipherKey(
            TPairwiseKeyData& aAmKey,
            const core_cipher_key_s& aCoreKey );

    };

#endif // WLANCONVERSIONUTIL_H
