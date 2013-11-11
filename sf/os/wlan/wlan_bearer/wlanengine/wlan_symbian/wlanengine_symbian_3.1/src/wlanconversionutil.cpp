/*
* Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* %version: 89 %
*/

#include <in_sock.h>
#include <e32math.h>
#include <in_iface.h>
#include "wlanconversionutil.h"
#include "wlmserver.h"
#include "wlmtsparams.h"
#include "am_debug.h"

/**
 * Mappings from user priority to access class.
 */
const TWlmAccessClass KWlmUserPriorityToAccessClass[] =
    {
    EWlmAccessClassBestEffort,
    EWlmAccessClassBackground,
    EWlmAccessClassBackground,
    EWlmAccessClassBestEffort,
    EWlmAccessClassVideo,
    EWlmAccessClassVideo,
    EWlmAccessClassVoice,
    EWlmAccessClassVoice    
    };

// Compile time assert. Used to make sure that some core constants are matching with Symbian constants.
#define COMPILE_ASSERT(e) typedef char __C_ASSERT__[(e)?1:-1] //lint --e{58,92,751,761,30,452,545}
// 58  = Error -- Bad type
// 92  = Negative array dimension or bit field length (-1)
// 751 = local typedef '__C_ASSERT__' not referenced
// 761 = Redundant typedef '__C_ASSERT__' previously declared at line x
// 30  = Expected an integer constant, obtained an expression that could not be evaluated
// 452 = typedef Symbol '__C_ASSERT__' redeclared (size) conflicts with line x
// 545 = Suspicious use of &

// ---------------------------------------------------------
// TWlanConversionUtil::ConvertSSID()
// ---------------------------------------------------------
//
void TWlanConversionUtil::ConvertSSID(
    core_ssid_s& aCoreSSID,
    const TSSID& aSSID )
    {
    Mem::Copy( aCoreSSID.ssid, aSSID.ssid, aSSID.ssidLength );
    aCoreSSID.length = aSSID.ssidLength;
    }

// ---------------------------------------------------------
// TWlanConversionUtil::ConvertSSID()
// ---------------------------------------------------------
//
void TWlanConversionUtil::ConvertSSID(
    core_ssid_s& aCoreSSID,
    const TDesC& aSSID )
    {
    TBuf8<KMaxSSIDLength> ssidBuf;
    ssidBuf.Copy( aSSID );
    
    Mem::Copy( aCoreSSID.ssid, ssidBuf.Ptr(), ssidBuf.Length() );
    aCoreSSID.length = ssidBuf.Length();
    }

// ---------------------------------------------------------
// TWlanConversionUtil::ConvertSSID()
// ---------------------------------------------------------
//
void TWlanConversionUtil::ConvertSSID(
    core_ssid_s& aCoreSSID,
    const TDesC8& aSSID )
    {
    Mem::Copy( aCoreSSID.ssid, aSSID.Ptr(), aSSID.Length() );
    aCoreSSID.length = aSSID.Length();
    }

// ---------------------------------------------------------
// TWlanConversionUtil::ConvertSSID()
// ---------------------------------------------------------
//
void TWlanConversionUtil::ConvertSSID(
    TSSID& aAmSSID,
    const core_ssid_s& aCoreSSID )
    {
    Mem::Copy( aAmSSID.ssid, aCoreSSID.ssid, aCoreSSID.length );
    aAmSSID.ssidLength = aCoreSSID.length;
    }

// ---------------------------------------------------------
// TWlanConversionUtil::ConvertWpaPreSharedKey()
// ---------------------------------------------------------
//
void TWlanConversionUtil::ConvertWpaPreSharedKey(
    core_wpa_preshared_key_s& aWpaPreSharedKey,
    const TDesC8& aWspPin )
    {        
    Mem::Copy(
        aWpaPreSharedKey.key_data,
        aWspPin.Ptr(),
        aWspPin.Length() );
    aWpaPreSharedKey.key_length = aWspPin.Length();
    }

// ---------------------------------------------------------
// TWlanConversionUtil::ConvertScanRate()
// ---------------------------------------------------------
//
TRate TWlanConversionUtil::ConvertScanRate( int_t aCoreScanRate )
    {
    return static_cast<TRate>( aCoreScanRate );
    }

// ---------------------------------------------------------
// TWlanConversionUtil::ConvertSecurityMode()
// ---------------------------------------------------------
//
TWlanSecurity TWlanConversionUtil::ConvertSecurityMode(
    const core_connection_security_mode_e& aCoreMode )
    {
    switch( aCoreMode )
        {
        case core_connection_security_mode_open:
            return EWlanSecurityOpen;
        case core_connection_security_mode_wep_open:
            return EWlanSecurityWepOpen;
        case core_connection_security_mode_wep_shared:
            return EWlanSecurityWepShared;
        case core_connection_security_mode_802d1x:
            return EWlanSecurity802d1x;
        case core_connection_security_mode_wpa:
            return EWlanSecurityWpa;
        case core_connection_security_mode_wpa_psk:
            return EWlanSecurityWpaPsk;
        case core_connection_security_mode_wpa2:
            return EWlanSecurityWpa2;
        case core_connection_security_mode_wpa2_psk:
            return EWlanSecurityWpa2Psk;
        case core_connection_security_mode_wapi:
            return EWlanSecurityWapi;
        case core_connection_security_mode_wapi_psk:
            return EWlanSecurityWapiPsk;
        default:
            return EWlanSecurityOpen;
        }
    }

// ---------------------------------------------------------
// TWlanConversionUtil::ConvertEncryptionMode()
// ---------------------------------------------------------
//
TEncryptionStatus TWlanConversionUtil::ConvertEncryptionMode(
    const core_encryption_mode_e& aCoreEncryptionMode )
    {
    switch( aCoreEncryptionMode )
        {
        case core_encryption_mode_disabled:
            return EEncryptionDisabled;
        case core_encryption_mode_wep:
            return EEncryptionWep;
        case core_encryption_mode_802dot1x:
            return EEncryption802dot1x;
        case core_encryption_mode_wep_mixed_cell:
            return EEncryptionWepMixed;
        case core_encryption_mode_802dot1x_mixed_cell:
            return EEncryption802dot1xMixed;
        case core_encryption_mode_wpi:
            return EEncryptionWAPI;
        default: // instead of case core_encryption_mode_wpa:
            return EEncryptionWpa;
        }
    }

// ---------------------------------------------------------
// TWlanConversionUtil::ConvertConnectionState()
// ---------------------------------------------------------
//
TWlanConnectionState TWlanConversionUtil::ConvertConnectionState(
    const core_connection_state_e& aCoreState )
    {
    switch( aCoreState )
        {
        case core_connection_state_notconnected:
            return EWlanStateNotConnected;
        case core_connection_state_infrastructure:
            return EWlanStateInfrastructure;
        case core_connection_state_searching:
            return EWlanStateSearchingAP;
        case core_connection_state_ibss: // fall through on purpose
        case core_connection_state_secureibss:
            return EWlanStateIBSS;
        case core_connection_state_secureinfra:
            return EWlanStateSecureInfra;
        default:
            return EConnectionStateUnknown;
        }
    }

// ---------------------------------------------------------
// TWlanConversionUtil::ConvertScanMode()
// ---------------------------------------------------------
//
core_scan_mode_e TWlanConversionUtil::ConvertScanMode(
    const TScanMode& aAmScanMode )
    {
    switch( aAmScanMode )
        {
        case EActiveScan:
            return core_scan_mode_active;
        case EPassiveScan:
            return core_scan_mode_passive;
        default:
            return core_scan_mode_passive;
        }
    }

// ---------------------------------------------------------
// TWlanConversionUtil::ConvertScanMode()
// ---------------------------------------------------------
//
TScanMode TWlanConversionUtil::ConvertScanMode(
    const core_scan_mode_e& aCoreScanMode )
    {
    switch( aCoreScanMode )
        {
        case core_scan_mode_active:
            return EActiveScan;
        case core_scan_mode_passive:
            return EPassiveScan;
        default:
            return EPassiveScan;
        }
    }

// ---------------------------------------------------------
// TWlanConversionUtil::ConvertConvertCipherKeyType()
// ---------------------------------------------------------
//
TWlanCipherSuite TWlanConversionUtil::ConvertCipherKeyType(
    const core_cipher_key_type_e& aCoreCipherKeyType )
    {
    switch( aCoreCipherKeyType )
        {
        case core_cipher_key_type_none:
            return EWlanCipherSuiteNone;
        case core_cipher_key_type_tkip:
            return EWlanCipherSuiteTkip;
        case core_cipher_key_type_ccmp:
            return EWlanCipherSuiteCcmp;
        case core_cipher_key_type_wpi:
            return EWlanCipherSuiteWapi;
        default:
            return EWlanCipherSuiteWep;
        }
    }

// ---------------------------------------------------------
// TWlanConversionUtil::ConvertNotificationType()
// ---------------------------------------------------------
//
TWlmNotify TWlanConversionUtil::ConvertNotificationType(
    const core_notification_e& aCoreNotificationType )
    {
    switch( aCoreNotificationType )
        {
        case core_notification_connection_state_changed:
            return EWlmNotifyConnectionStateChanged;
        case core_notification_bssid_changed:
            return EWlmNotifyBssidChanged;
        case core_notification_bss_lost:
            return EWlmNotifyBssLost;
        case core_notification_bss_regained:
            return EWlmNotifyBssRegained;
        case core_notification_tx_power_level_changed:
            return EWlmNotifyTransmitPowerChanged;
        case core_notification_rcp_changed:
            return EWlmNotifyRcpChanged;
        case core_notification_ts_status_changed:
            return EWlmNotifyTrafficStreamStatusChanged;
        case core_notification_ap_info_changed:
            return EWlmNotifyAccessPointInfoChanged;
        case core_notification_rcpi_roam_attempt_started:
            return EWlmNotifyRcpiRoamAttemptStarted;
        case core_notification_rcpi_roam_attempt_completed:
            return EWlmNotifyRcpiRoamAttemptCompleted;
        case core_notification_ac_traffic_mode_changed:
            return EWlmNotifyAcTrafficModeChanged;
        case core_notification_ac_traffic_status_changed:
            return EWlmNotifyAcTrafficStatusChanged;
        default: // this should never happen
            return EWlmNotifyNone;
        }
    }

// ---------------------------------------------------------
// TWlanConversionUtil::ConvertConnectStatus()
// ---------------------------------------------------------
//
TInt TWlanConversionUtil::ConvertConnectStatus(
    const core_connect_status_e& aCoreConnectStatus,
    core_security_mode_e security_mode )
    {
    switch( aCoreConnectStatus )
        {
        case core_connect_ok:
            return KErrNone;
        case core_connect_network_not_found:
        case core_connect_mode_infra_required_but_ibss_found:
        case core_connect_mode_ibss_required_but_infra_found:
        case core_connect_ap_full:
        case core_connect_ap_not_whitelisted:
        case core_connect_ap_permanently_blacklisted:
        case core_connect_ap_temporarily_blacklisted:
        case core_connect_ap_outside_defined_region:
        case core_connect_iap_open_but_ap_requires_encryption:
        case core_connect_iap_wep_but_ap_has_no_privacy:
        case core_connect_iap_wpa_but_ap_has_no_privacy:
        case core_connect_eapol_auth_start_timeout:
        case core_connect_wep_open_authentication_unsupported:
        case core_connect_wep_shared_authentication_unsupported:
            return KErrWlanNetworkNotFound;
        case core_connect_iap_wep_but_ap_has_wpa_ie:
            return KErrWlanWpaAuthRequired;
        case core_connect_wpa_ap_has_no_valid_ciphers:
        case core_connect_wpa_ie_required_but_ap_has_none:
            {
            if ( security_mode == core_security_mode_wpa2only )
                {
                return KErrWlanWpa2OnlyModeNotSupported;
                }

            return KErrWlanNetworkNotFound;
            }
        case core_connect_wep_open_echo_test_failed:
            return KErrWlanIllegalEncryptionKeys;
        case core_connect_wep_shared_authentication_failed:
            return KErrWlanSharedKeyAuthFailed;
        case core_connect_wpa_psk_required_but_ap_has_no_support:
            return KErrWlanEapModeRequired;
        case core_connect_wpa_eap_required_but_ap_has_no_support:
            return KErrWlanPskModeRequired;
        case core_connect_wpa_eap_failure:
            return KErrWlanWpaAuthFailed;
        case core_connect_wpa_psk_failure:
            return KErrWlanIllegalWpaPskKey;
        case core_connect_802_1x_failure:
        case core_connect_802_1x_authentication_algorithm_not_supported:
            return KErrWlan802dot1xAuthFailed;
        case core_connect_wpa_authentication_canceled_by_user:
        case core_connect_802_1x_authentication_canceled_by_user:
            return KErrCancel;
        case core_connect_eap_gtc_failure:
            return KErrWlanEapGtcFailed;
        case core_connect_eap_tls_failure:
            return KErrWlanEapTlsFailed;
        case core_connect_eap_tls_server_certificate_expired:
        case core_connect_eap_ttls_server_certificate_expired:
        case core_connect_eap_peap_server_certificate_expired:
            return KErrWlanServerCertificateExpired;
        case core_connect_eap_tls_server_certificate_unknown:
        case core_connect_eap_ttls_server_certificate_unknown:
        case core_connect_eap_peap_server_certificate_unknown:
            return KErrWlanCerficateVerifyFailed;
        case core_connect_eap_tls_user_certificate_expired:
        case core_connect_eap_ttls_user_certificate_expired:
        case core_connect_eap_peap_user_certificate_expired:
            return KErrWlanUserCertificateExpired;
        case core_connect_eap_tls_user_certificate_unknown:
        case core_connect_eap_ttls_user_certificate_unknown:
        case core_connect_eap_peap_user_certificate_unknown:
            return KErrWlanNoUserCertificate;
        case core_connect_eap_tls_illegal_cipher_suite:
        case core_connect_eap_ttls_illegal_cipher_suite:
        case core_connect_eap_peap_illegal_cipher_suite:
            return KErrWlanNoCipherSuite;
        case core_connect_eap_tls_user_rejected:
        case core_connect_eap_ttls_user_rejected:
        case core_connect_eap_peap_user_rejected:
            return KErrWlanUserRejected;
        case core_connect_eap_leap_failure:
            return KErrWlanLeapFailed;
        case core_connect_eap_sim_failure:
            return KErrWlanEapSimFailed;
        case core_connect_eap_sim_identity_query_failed:
        case core_connect_eap_aka_identity_query_failed:
            return KErrWlanSimNotInstalled;
        case core_connect_eap_sim_user_has_not_subscribed_to_the_requested_service:
        case core_connect_eap_aka_user_has_not_subscribed_to_the_requested_service:
            return KErrWlanNotSubscribed;
        case core_connect_eap_sim_users_calls_are_barred:
        case core_connect_eap_aka_users_calls_are_barred:
            return KErrWlanAccessBarred;
        case core_connect_eap_ttls_failure:
            return KErrWlanEapTtlsFailed;
        case core_connect_eap_aka_failure:
            return KErrWlanEapAkaFailed;
        case core_connect_eap_peap_failure:
            return KErrWlanEapPeapFailed;
        case core_connect_eap_mschapv2_failure:
            return KErrWlanEapMsChapv2;
        case core_connect_eap_mschapv2_restricted_logon_hours:
            return KErrWlanRestrictedLogonHours;
        case core_connect_eap_mschapv2_account_disabled:
            return KErrWlanAccountDisabled;
        case core_connect_eap_mschapv2_no_dialin_permission:
            return KErrWlanNoDialinPermissions;
        case core_connect_eap_mschapv2_password_expired:
            return KErrWlanPasswordExpired;
        case core_connect_eap_fast_tunnel_compromise_error:
            return KErrWlanEapFastTunnelCompromiseError;
        case core_connect_eap_fast_unexpected_tlv_exhanged:
            return KErrWlanEapFastUnexpextedTlvExhanged;
        case core_connect_eap_fast_no_pac_nor_certs_to_authenticate_with_provision_disabled:
            return KErrWlanEapFastNoPacNorCertsToAuthenticateWithProvDisabled;
        case core_connect_eap_fast_no_matching_pac_for_aid:
            return KErrWlanEapFastNoMatchingPacForAid;
        case core_connect_eap_fast_authentication_failed:
            return KErrWlanEapFastAuthFailed;
        case core_connect_eap_fast_pac_store_corrupted:
            return KErrWlanEapFastPacStoreCorrupted;
        case core_connect_wapi_ie_required_but_ap_has_none:
        case core_connect_wapi_certificate_required_but_ap_has_no_support:
        case core_connect_wapi_psk_required_but_ap_has_no_support:
        case core_connect_wapi_ap_has_no_valid_ciphers:
            return KErrWlanNetworkNotFound;
        case core_connect_wapi_psk_failure:
            return KErrWlanInternalError;
        case core_connect_wapi_certificate_failure:
            return KErrWlanInternalError;
        case core_connect_ap_unsupported_configuration:
            return KErrIfAuthenticationFailure;
        default:
            return KErrUnknown;
        }
    }

// ---------------------------------------------------------
// TWlanConversionUtil::ConvertRegion()
// ---------------------------------------------------------
//
core_wlan_region_e TWlanConversionUtil::ConvertRegion(
    const TWlanRegion& aAmRegion )
    {
    switch( aAmRegion )
        {
        case EETSI:
            return core_wlan_region_etsi;
        default:
            return core_wlan_region_fcc;
        }
    }

// ---------------------------------------------------------
// TWlanConversionUtil::ConvertRegion()
// ---------------------------------------------------------
//
TWlanRegion TWlanConversionUtil::ConvertRegion(
    const core_wlan_region_e& aCoreRegion )
    {
    switch( aCoreRegion )
        {
        case core_wlan_region_etsi:
            return EETSI;
        default:
            return EFCC;
        }
    }

// ---------------------------------------------------------
// TWlanConversionUtil::ConvertMacAddress()
// ---------------------------------------------------------
//
void TWlanConversionUtil::ConvertMacAddress(
    TMacAddress& aAmMacAddress,
    const core_mac_address_s& aCoreMacAddress )
    {
    // Function assumes both mac address types have the same length
    Mem::Copy( 
        aAmMacAddress.iMacAddress, 
        aCoreMacAddress.addr, 
        MAC_ADDR_LEN );
    }

// ---------------------------------------------------------
// TWlanConversionUtil::ConvertMacAddress()
// ---------------------------------------------------------
//
void TWlanConversionUtil::ConvertMacAddress(
    core_mac_address_s& aCoreAddress,
    const TMacAddress& aAmMacAddress )
    {
    // Function assumes both mac address types have the same length
    Mem::Copy( 
        aCoreAddress.addr, 
        aAmMacAddress.iMacAddress, 
        MAC_ADDR_LEN );
    }

// ---------------------------------------------------------
// TWlanConversionUtil::ConvertMacAddress()
// ---------------------------------------------------------
//
void TWlanConversionUtil::ConvertMacAddress(
    core_mac_address_s& aCoreAddress,
    const TMacAddr& aAmMacAddress )
    {
    // Function assumes both mac address types have the same length
    Mem::Copy( 
        aCoreAddress.addr, 
        aAmMacAddress.iMacAddress, 
        MAC_ADDR_LEN );
    }

// ---------------------------------------------------------
// TWlanConversionUtil::ConvertMacAddress()
// ---------------------------------------------------------
//
void TWlanConversionUtil::ConvertMacAddress(
    TMacAddress& aCoreAddress,
    const TMacAddr& aAmMacAddress )
    {
    // Function assumes both mac address types have the same length
    Mem::Copy( 
        aCoreAddress.iMacAddress, 
        aAmMacAddress.iMacAddress, 
        MAC_ADDR_LEN );
    }

// ---------------------------------------------------------
// TWlanConversionUtil::ConvertPowerMode()
// ---------------------------------------------------------
//
TPowerMode TWlanConversionUtil::ConvertPowerMode(
    const core_power_mode_e& aCorePowerMode )
    {
    switch( aCorePowerMode )
        {
        case core_power_mode_cam:
            return EPowerModeCam;
        default:
            return EPowerModePs;
        }
    }

// ---------------------------------------------------------
// TWlanConversionUtil::ConvertWakeUpInterval()
// ---------------------------------------------------------
//
void TWlanConversionUtil::ConvertWakeUpInterval(
    TWlanWakeUpInterval& aAmWakeUpMode,
    TUint8& aAmWakeUpInterval,
    const core_power_save_wakeup_mode_e& aCoreWakeUpMode,
    const u8_t& aCoreWakeUpInterval )
    {
    switch( aCoreWakeUpMode )
        {
        case core_power_save_wakeup_mode_dtim:
            {
            if( aCoreWakeUpInterval == 1 )
                {
                aAmWakeUpMode = EWakeUpIntervalAllDtims;
                aAmWakeUpInterval = 0;
                }
            else
                {
                aAmWakeUpMode = EWakeUpIntervalEveryNthDtim;
                aAmWakeUpInterval = aCoreWakeUpInterval;
                }
            break;
            }       
        default:
            {
            if( aCoreWakeUpInterval == 1 )
                {
                aAmWakeUpMode = EWakeUpIntervalAllBeacons;
                aAmWakeUpInterval = 0;
                }
            else
                {
                aAmWakeUpMode = EWakeUpIntervalEveryNthBeacon;
                aAmWakeUpInterval = aCoreWakeUpInterval;
                }
            break;
            }
        }
    }

// ---------------------------------------------------------
// TWlanConversionUtil::ConvertIndication()
// ---------------------------------------------------------
//
core_am_indication_e TWlanConversionUtil::ConvertIndication(
    const TIndication& aAmIndication )
    {
    switch( aAmIndication )
        {
        case EMediaDisconnect:        
            return core_am_indication_wlan_media_disconnect;
        case EOsPowerStandby:
            return core_am_indication_os_power_standby;
        case EHWFailed:
            return core_am_indication_wlan_hw_failed;
        case EConsecutiveBeaconsLost:
            return core_am_indication_wlan_beacon_lost;
        case EConsecutiveTxFailures:
            return core_am_indication_wlan_tx_fail;
        case EConsecutivePwrModeSetFailures:
            return core_am_indication_wlan_power_mode_failure;
        case EBSSRegained:
            return core_am_indication_wlan_bss_regained;
        case EWepDecryptFailure:
            return core_am_indication_wlan_wep_decrypt_failure;
        case EPairwiseKeyMicFailure:
            return core_am_indication_wlan_pairwise_key_mic_failure;
        case EGroupKeyMicFailure:
            return core_am_indication_wlan_group_key_mic_failure;
        case ERcpiTrigger:
            return core_am_indication_wlan_rcpi_trigger;
        case EScanCompleted:
            return core_am_indication_wlan_scan_complete;
        case ESignalLossPrediction:
            return core_am_indication_wlan_signal_loss_prediction;
        case EApTestOpportunity:
            return core_am_indication_wlan_power_save_test_trigger;
        case EVoiceCallOn:
            return core_am_indication_voice_call_on;
        case EVoiceCallOff:
            return core_am_indication_voice_call_off;
        case EPsModeError:
            return core_am_indication_wlan_ap_ps_mode_error;
        default:
            return core_am_indication_wlan_hw_failed;
        }
    }

// ---------------------------------------------------------
// TWlanConversionUtil::ConvertDeviceSettings()
// ---------------------------------------------------------
//
void TWlanConversionUtil::ConvertDeviceSettings(
    core_device_settings_s& aCoreSettings,
    const CWlanDeviceSettings::SWlanDeviceSettings& aAmSettings )
    {
    aCoreSettings.beacon = static_cast<u32_t>( aAmSettings.beacon );
    aCoreSettings.long_retry = static_cast<u32_t>( aAmSettings.longRetry );
    aCoreSettings.short_retry = static_cast<u32_t>( aAmSettings.shortRetry );
    aCoreSettings.rts = static_cast<u32_t>( aAmSettings.rts );
    aCoreSettings.tx_power_level = TWlanConversionUtil::ConvertMilliwattsToDecibel( aAmSettings.txPowerLevel );    
	aCoreSettings.scan_rate = static_cast<u32_t>( aAmSettings.scanRate ); // TRate -> u32_t
	aCoreSettings.rcpi_trigger = static_cast<u32_t>( aAmSettings.rcpiTrigger );
	aCoreSettings.active_scan_min_ch_time = static_cast<u32_t>( aAmSettings.minActiveChannelTime );
	aCoreSettings.active_scan_max_ch_time = static_cast<u32_t>( aAmSettings.maxActiveChannelTime );
	aCoreSettings.active_broadcast_scan_min_ch_time = static_cast<u32_t>( aAmSettings.minActiveBroadcastChannelTime );
	aCoreSettings.active_broadcast_scan_max_ch_time = static_cast<u32_t>( aAmSettings.maxActiveBroadcastChannelTime );	
	aCoreSettings.passive_scan_min_ch_time = static_cast<u32_t>( aAmSettings.minPassiveChannelTime );
	aCoreSettings.passive_scan_max_ch_time = static_cast<u32_t>( aAmSettings.maxPassiveChannelTime );
    aCoreSettings.max_tx_msdu_life_time = static_cast<u32_t>( aAmSettings.maxTxMSDULifeTime );
    aCoreSettings.unload_driver_timer = static_cast<u32_t>( aAmSettings.unloadDriverTimer );
    aCoreSettings.roam_timer = static_cast<u32_t>( aAmSettings.roamTimer );
    aCoreSettings.rcpi_difference = static_cast<u32_t>( aAmSettings.rcpiDifference );
    aCoreSettings.max_tries_to_find_nw = static_cast<u32_t>( aAmSettings.maxTriesToFindNw );
    aCoreSettings.delay_between_find_nw = static_cast<u32_t>( aAmSettings.delayBetweenFindNw );
    aCoreSettings.power_save_enabled = static_cast<u32_t>( aAmSettings.powerMode );
    aCoreSettings.allow_radio_measurements = static_cast<bool_t>( aAmSettings.allowRadioMeasurements );    
    aCoreSettings.max_ap_association_failure_count = static_cast<u32_t>( aAmSettings.maxApFailureCount );
    aCoreSettings.max_ap_authentication_failure_count = static_cast<u32_t>( aAmSettings.maxApAuthFailureCount );
    aCoreSettings.long_beacon_find_count = static_cast<u32_t>( aAmSettings.longBeaconFindCount );
    aCoreSettings.qos_null_frame_interval = static_cast<u32_t>( aAmSettings.qosNullFrameInterval );
    aCoreSettings.qos_null_frame_exit_timeout = static_cast<u32_t>( aAmSettings.qosNullFrameTimeout );
    aCoreSettings.qos_null_frame_entry_timeout = static_cast<u32_t>( aAmSettings.qosNullFrameEntryTimeout );
    aCoreSettings.qos_null_frame_entry_tx_count = static_cast<u32_t>( aAmSettings.qosNullFrameEntryTxCount );
    aCoreSettings.keep_alive_interval = static_cast<u32_t>( aAmSettings.keepAliveInterval );
    aCoreSettings.scan_stop_rcpi_threshold = static_cast<u32_t>( aAmSettings.scanStopRcpiThreshold );
    aCoreSettings.iap_availability_rcpi_threshold = static_cast<u32_t>( aAmSettings.minRcpiForIapAvailability );
    aCoreSettings.max_ap_deauthentication_count = static_cast<u32_t>( aAmSettings.maxApDeauthenticationCount );
    aCoreSettings.ap_deauthentication_timeout = static_cast<u32_t>( aAmSettings.apDeauthenticationTimeout );    
    aCoreSettings.sp_rcpi_target = static_cast<u32_t>( aAmSettings.spRcpiTarget );
    aCoreSettings.sp_time_target = static_cast<u32_t>( aAmSettings.spTimeTarget );
    aCoreSettings.sp_min_indication_interval = static_cast<u32_t>( aAmSettings.spMinIndicationInterval );
    aCoreSettings.scan_list_expiration_time = static_cast<u32_t>( aAmSettings.scanListExpirationTime );
    aCoreSettings.rcpi_roam_min_interval = static_cast<u32_t>( aAmSettings.rcpiRoamMinInterval );
    aCoreSettings.rcpi_roam_max_interval = static_cast<u32_t>( aAmSettings.rcpiRoamMaxInterval );
    aCoreSettings.rcpi_roam_attempts_per_interval = static_cast<u32_t>( aAmSettings.rcpiRoamAttemptsPerInterval );
    aCoreSettings.rcpi_roam_next_interval_factor = static_cast<u32_t>( aAmSettings.rcpiRoamNextIntervalFactor );
    aCoreSettings.rcpi_roam_next_interval_addition = static_cast<u32_t>( aAmSettings.rcpiRoamNextIntervalAddition );    
    aCoreSettings.bss_lost_roam_min_interval = static_cast<u32_t>( aAmSettings.bssLostRoamMinInterval );
    aCoreSettings.bss_lost_roam_max_interval = static_cast<u32_t>( aAmSettings.bssLostRoamMaxInterval );
    aCoreSettings.bss_lost_roam_attempts_per_interval = static_cast<u32_t>( aAmSettings.bssLostRoamAttemptsPerInterval );
    aCoreSettings.bss_lost_roam_next_interval_factor = static_cast<u32_t>( aAmSettings.bssLostRoamNextIntervalFactor );
    aCoreSettings.bss_lost_roam_next_interval_addition = static_cast<u32_t>( aAmSettings.bssLostRoamNextIntervalAddition );
    aCoreSettings.bss_lost_roam_max_tries_to_find_nw = static_cast<u32_t>( aAmSettings.bssLostRoamMaxTriesToFindNw );        
    aCoreSettings.max_dtim_skip_interval = static_cast<u32_t>( aAmSettings.maxDtimSkipInterval );
    aCoreSettings.ps_active_to_light_timeout = static_cast<u32_t>( aAmSettings.psActiveToLightTimeout );
    aCoreSettings.ps_active_to_light_threshold = static_cast<u32_t>( aAmSettings.psActiveToLightThreshold );
    aCoreSettings.ps_light_to_active_timeout = static_cast<u32_t>( aAmSettings.psLightToActiveTimeout );
    aCoreSettings.ps_light_to_active_threshold = static_cast<u32_t>( aAmSettings.psLightToActiveThreshold );
    aCoreSettings.ps_light_to_deep_timeout = static_cast<u32_t>( aAmSettings.psLightToDeepTimeout );
    aCoreSettings.ps_light_to_deep_threshold = static_cast<u32_t>( aAmSettings.psLightToDeepThreshold );
    aCoreSettings.ps_uapsd_rx_frame_length = static_cast<u32_t>( aAmSettings.psUapsdRxThreshold );
    aCoreSettings.rrm_min_measurement_interval = static_cast<u32_t>( aAmSettings.rrmMinMeasurementInterval );
    }

// ---------------------------------------------------------
// TWlanConversionUtil::ConvertIapSettings()
// ---------------------------------------------------------
//
void TWlanConversionUtil::ConvertIapSettings(
    core_iap_data_s& aCoreSettings,
    const SWLANSettings& aAmSettings,
    const TBool aDhcpInUse,
    const TWLMOverrideSettings& aOverrideSettings )
    {
    Mem::FillZ( &aCoreSettings, sizeof( aCoreSettings ) );

    aCoreSettings.id = aAmSettings.ServiceID; // LanService table row-id

    // Handle
    // Connection mode
    if ( aAmSettings.ConnectionMode == Adhoc ||
         aOverrideSettings.settingsMask & EOverrideIbssMask )
        {
        aCoreSettings.op_mode = core_operating_mode_ibss;
        }
    else
        {
        aCoreSettings.op_mode = core_operating_mode_infrastructure;
        }
    
    // Handle
    // SSID
    if( aOverrideSettings.settingsMask & EOverrideSsidMask )
        {
        Mem::Copy( 
            aCoreSettings.ssid.ssid,
            aOverrideSettings.ssid.ssid, 
            aOverrideSettings.ssid.ssidLength );
        aCoreSettings.ssid.length = aOverrideSettings.ssid.ssidLength;
        }
    else
        {
        TBuf8<MAX_SSID_LEN> buf8;
        buf8.Copy( aAmSettings.SSID );
        Mem::Copy( 
            aCoreSettings.ssid.ssid,
            buf8.Ptr(),
            aAmSettings.SSID.Length() );
        aCoreSettings.ssid.length = aAmSettings.SSID.Length();        
        }

    // Handle
    // ScanSSID / is_hidden
    if( aAmSettings.ScanSSID )
        {
        aCoreSettings.is_hidden = true_t;
        }
    else
        {
        aCoreSettings.is_hidden = false_t;
        }
    
    // Handle
    // Security mode
    switch( aAmSettings.SecurityMode )
        {
        case AllowUnsecure:
            aCoreSettings.security_mode = core_security_mode_allow_unsecure;
            break;
        case Wep:
            aCoreSettings.security_mode = core_security_mode_wep;
            break;
        case Wlan8021x:
            if( aAmSettings.WPAKeyLength == 1 )
                {
                aCoreSettings.security_mode = core_security_mode_802dot1x_unencrypted;
                }
            else
                {
                aCoreSettings.security_mode = core_security_mode_802dot1x;
                }
            break;
        case Wpa:
            aCoreSettings.security_mode = core_security_mode_wpa;
            break;            
        case Wpa2Only:
            aCoreSettings.security_mode = core_security_mode_wpa2only;
            break;
        case Wapi:
            aCoreSettings.security_mode = core_security_mode_wapi;
            break;            
        default:
            aCoreSettings.security_mode = core_security_mode_allow_unsecure;
        }

    // Handle
    // WEP Keys
    if( aOverrideSettings.settingsMask & EOverrideWepMask )
        {
        aCoreSettings.wep_key1.key_index = aOverrideSettings.wep.KeyIndex;
        aCoreSettings.wep_key1.key_length = aOverrideSettings.wep.KeyLength;
        Mem::Copy(
            aCoreSettings.wep_key1.key_data, 
            aOverrideSettings.wep.KeyMaterial, 
            MAX_WEP_KEY_LENGTH );
        aCoreSettings.default_wep_key = aOverrideSettings.wep.KeyIndex;
        aCoreSettings.security_mode = core_security_mode_wep;        

        aCoreSettings.wep_key2.key_length = 0;
        aCoreSettings.wep_key3.key_length = 0;
        aCoreSettings.wep_key4.key_length = 0;
        }
    else
        {
        aCoreSettings.wep_key1.key_index = aAmSettings.WepKey1.KeyIndex;
        aCoreSettings.wep_key1.key_length = aAmSettings.WepKey1.KeyLength;
        Mem::Copy( 
            aCoreSettings.wep_key1.key_data, 
            aAmSettings.WepKey1.KeyMaterial, 
            MAX_WEP_KEY_LENGTH );

        aCoreSettings.wep_key2.key_index = aAmSettings.WepKey2.KeyIndex;
        aCoreSettings.wep_key2.key_length = aAmSettings.WepKey2.KeyLength;
        Mem::Copy( 
            aCoreSettings.wep_key2.key_data, 
            aAmSettings.WepKey2.KeyMaterial, 
            MAX_WEP_KEY_LENGTH );

        aCoreSettings.wep_key3.key_index = aAmSettings.WepKey3.KeyIndex;
        aCoreSettings.wep_key3.key_length = aAmSettings.WepKey3.KeyLength;
        Mem::Copy( 
            aCoreSettings.wep_key3.key_data, 
            aAmSettings.WepKey3.KeyMaterial, 
            MAX_WEP_KEY_LENGTH );

        aCoreSettings.wep_key4.key_index = aAmSettings.WepKey4.KeyIndex;
        aCoreSettings.wep_key4.key_length = aAmSettings.WepKey4.KeyLength;
        Mem::Copy( 
            aCoreSettings.wep_key4.key_data, 
            aAmSettings.WepKey4.KeyMaterial, 
            MAX_WEP_KEY_LENGTH );

        aCoreSettings.default_wep_key = aAmSettings.WepIndex;        
        }

    // Handle
    // Authentication mode
    switch( aAmSettings.AuthenticationMode )
        {
        case EWepAuthModeShared:
            aCoreSettings.authentication_mode = core_authentication_mode_shared;
            break;
        case EWepAuthModeOpen:
            /** Falls through on purpose. */
        default:
            aCoreSettings.authentication_mode = core_authentication_mode_open;
            break;
        }

    // Handle
    // Preshared key
    aCoreSettings.wpa_preshared_key_in_use = aAmSettings.EnableWpaPsk;
    if ( aAmSettings.EnableWpaPsk )
        {
        Mem::Copy(
            aCoreSettings.wpa_preshared_key.key_data,
            aAmSettings.WPAPreSharedKey.Ptr(),
            aAmSettings.WPAPreSharedKey.Length() );
        aCoreSettings.wpa_preshared_key.key_length = aAmSettings.WPAPreSharedKey.Length();
        }

    // Handle
    // EOverrideWpaPskMask
    if ( aOverrideSettings.settingsMask & EOverrideWpaPskMask )
        {
        Mem::Copy(
            aCoreSettings.wpa_preshared_key.key_data,
            aOverrideSettings.wpaPsk.KeyMaterial,
            aOverrideSettings.wpaPsk.KeyLength );
        aCoreSettings.wpa_preshared_key.key_length = aOverrideSettings.wpaPsk.KeyLength;
        aCoreSettings.security_mode = core_security_mode_wpa;
        aCoreSettings.wpa_preshared_key_in_use = true_t;
        aCoreSettings.is_wpa_overriden = true_t;
        }

    // Handle
    // EOverrideWpaMask
    if ( aOverrideSettings.settingsMask & EOverrideWpaMask )
        {
        aCoreSettings.wpa_preshared_key.key_length = 0;
        aCoreSettings.security_mode = core_security_mode_wpa;
        aCoreSettings.wpa_preshared_key_in_use = false_t;
        aCoreSettings.is_wpa_overriden = true_t;
        }

    // Handle
    // ChannelId (Adhoc feature)
    aCoreSettings.used_adhoc_channel = aAmSettings.ChannelID;

    // Handle DHCP vs static IP
    aCoreSettings.is_dynamic_ip_addr = aDhcpInUse;    

    // Roaming is always allowed.
    aCoreSettings.is_roaming_allowed = true_t;
    }

// ---------------------------------------------------------
// TWlanConversionUtil::ConvertIapSettings()
// ---------------------------------------------------------
//
void TWlanConversionUtil::ConvertIapSettings(
    core_iap_data_s& aCoreSettings,
    const TWlanLimitedIapData& aAmSettings )
    {
    Mem::FillZ( &aCoreSettings, sizeof( aCoreSettings ) );

    aCoreSettings.id = aAmSettings.iapId;

    // Handle
    // Connection mode
    if ( aAmSettings.networkType == Adhoc )
        {
        aCoreSettings.op_mode = core_operating_mode_ibss;
        }
    else
        {
        aCoreSettings.op_mode = core_operating_mode_infrastructure;
        }
    
    // Handle
    // SSID
    ConvertSSID( aCoreSettings.ssid, aAmSettings.ssid );

    // Handle
    // ScanSSID / is_hidden
    if( aAmSettings.isHidden )
        {
        aCoreSettings.is_hidden = true_t;
        }
    else
        {
        aCoreSettings.is_hidden = false_t;
        }
    
    // Handle
    // Security mode
    switch( aAmSettings.securityMode )
        {
        case AllowUnsecure:
            aCoreSettings.security_mode = core_security_mode_allow_unsecure;
            break;
        case Wep:
            aCoreSettings.security_mode = core_security_mode_wep;
            break;
        case Wlan8021x:
            aCoreSettings.security_mode = core_security_mode_802dot1x;
            break;
        case Wpa:
            aCoreSettings.security_mode = core_security_mode_wpa;
            break;            
        case Wpa2Only:
            aCoreSettings.security_mode = core_security_mode_wpa2only;
            break;
        case Wapi:
            aCoreSettings.security_mode = core_security_mode_wapi;
            break;
        default:
            aCoreSettings.security_mode = core_security_mode_allow_unsecure;
        }

    // Handle
    // Preshared key
    aCoreSettings.wpa_preshared_key_in_use = aAmSettings.isPskEnabled;
    }

// ---------------------------------------------------------
// TWlanConversionUtil::ConvertErrorCode()
// ---------------------------------------------------------
//
TInt TWlanConversionUtil::ConvertErrorCode( const core_error_e aError )
    {
    switch ( aError )
        {
        case core_error_ok:
            return KErrNone;
        case core_error_not_found:
            return KErrNotFound;
        case core_error_no_memory:
            return KErrNoMemory;
        case core_error_illegal_argument:
            return KErrArgument;
        case core_error_not_supported:
            return KErrNotSupported;
        case core_error_in_use:
            return KErrInUse;
        case core_error_timeout:
            return KErrTimedOut;
        case core_error_connection_already_active:
            return KErrWlanConnAlreadyActive;
        case core_error_wlan_disabled:
            return KErrNotReady;
        case core_error_cancel:
            return KErrCancel;
        case core_error_already_exists:
            return KErrAlreadyExists;
        case core_error_illegal_rate:
        case core_error_failed:
        case core_error_drivers_not_loaded:
        case core_error_in_power_save:
        case core_error_challenge_failure:
        case core_error_not_connected:
        case core_error_general:
        case core_error_unsupported_config:
        default:
            return KErrGeneral;
        }
    }

// ---------------------------------------------------------
// TWlanConversionUtil::ConvertErrorCode()
// ---------------------------------------------------------
//
core_error_e TWlanConversionUtil::ConvertErrorCode( const TInt aError )
    {
    switch ( aError )
        {
        case KErrNone:
            return core_error_ok;
        case KErrNotFound:
            return core_error_not_found;
        case KErrNoMemory:
            return core_error_no_memory;
        case KErrNotSupported:
            return core_error_not_supported;
        case KErrArgument:
            return core_error_illegal_argument;
        case KErrInUse:
            return core_error_in_use;
        case KErrTimedOut:
            return core_error_timeout;
        case KErrCancel:
            return core_error_cancel;
        case KErrAlreadyExists:
            return core_error_already_exists;
        case KWlanErrUnsupportedNwConf:
            return core_error_unsupported_config;
        default:
            return core_error_general;    
        }
    }

// ---------------------------------------------------------
// TWlanConversionUtil::ConvertMilliwattsToDecibel()
// ---------------------------------------------------------
//
u32_t TWlanConversionUtil::ConvertMilliwattsToDecibel(
    const TUint32 aValue )
    {
    TReal realResult( 0 );
    TInt32 intResult( 0 );
    TUint32 retValue( 0 );

    DEBUG1( "TWlanConversionUtil::ConvertMilliwattsToDecibel() - aValue = %u",
        aValue );
    
    if ( ( Math::Log( realResult, aValue ) == KErrNone ) &&
         ( Math::Int( intResult, realResult * 10 ) == KErrNone ) )
        {
        if ( intResult )
            {
            retValue = intResult;
            }        
        }
    
    DEBUG1( "TWlanConversionUtil::ConvertMilliwattsToDecibel() - retValue = %u",
        retValue );
    return static_cast<u32_t>( retValue );
    }

// ---------------------------------------------------------
// TWlanConversionUtil::ConvertDecibelToMilliWatts()
// ---------------------------------------------------------
//
TUint32 TWlanConversionUtil::ConvertDecibelToMilliWatts(
    const u32_t aValue )
    {
    TReal realResult( 0 );
    TReal realRound( 0 );
    const TReal realValue( 10 );
    const TReal realPower( static_cast<TUint32>( aValue ) );
    TInt32 intResult( 0 );
    TInt32 intDecimalPlaces( 0 );
    TUint32 retValue( 1 );

    DEBUG1( "TWlanConversionUtil::ConvertDecibelToMilliWatts() - aValue = %u",
        aValue );

    if ( ( Math::Pow( realResult, realValue, realPower / 10 ) == KErrNone ) &&
         ( Math::Round( realRound, realResult, intDecimalPlaces ) == KErrNone ) &&
         ( Math::Int( intResult, realRound ) == KErrNone ) )
        {
        if ( intResult )
            {
            retValue = intResult;
            }
        }

    DEBUG1( "TWlanConversionUtil::ConvertDecibelToMilliWatts() - retValue = %u",
        retValue );
    return retValue;
    }

// ---------------------------------------------------------
// TWlanConversionUtil::ConvertRcpClass()
// ---------------------------------------------------------
//
TWlanRCPLevel TWlanConversionUtil::ConvertRcpClass(
    const core_rcp_class_e aValue )
    {
    switch( aValue )
        {
        case core_rcp_weak:
            return EWlanRcpWeak;
        default:
            return EWlanRcpNormal;
        }
    }

// ---------------------------------------------------------
// TWlanConversionUtil::ConvertScanChannels()
// ---------------------------------------------------------
//
void TWlanConversionUtil::ConvertScanChannels(
    SChannels& aAmChannels,
    const core_scan_channels_s& aCoreChannels )
    {
    aAmChannels.iBand = aCoreChannels.band;
    Mem::Copy(
        &aAmChannels.iChannels2dot4GHz[0],
        &aCoreChannels.channels2dot4ghz[0],
        sizeof( aCoreChannels.channels2dot4ghz ) );
    Mem::Copy(
        &aAmChannels.iChannels4dot9GHz[0],
        &aCoreChannels.channels4dot9ghz[0],
        sizeof( aCoreChannels.channels4dot9ghz ) );
    Mem::Copy(
        &aAmChannels.iChannels5GHz[0],
        &aCoreChannels.channels5ghz[0],
        sizeof( aCoreChannels.channels5ghz ) );                
    }

// ---------------------------------------------------------
// TWlanConversionUtil::ConvertScanChannels()
// ---------------------------------------------------------
//
void TWlanConversionUtil::ConvertScanChannels(            
    core_scan_channels_s& aCoreChannels,
    RArray<TUint>& aAmChannels )
    {
    aCoreChannels.band = SCAN_BAND_2DOT4GHZ;
    u16_t channels( 0 );
    
    for ( TInt idx( 0 ); idx < aAmChannels.Count(); ++idx )
        {
        channels |= static_cast<u16_t>( 1 << ( aAmChannels[idx] - 1 ) );
        }

    Mem::Copy(
        &aCoreChannels.channels2dot4ghz[0],
        reinterpret_cast<u8_t*>( &channels ),
        sizeof( channels ) );
    }

// ---------------------------------------------------------
// TWlanConversionUtil::ConvertFrameType()
// ---------------------------------------------------------
//
core_frame_type_e TWlanConversionUtil::ConvertFrameType(
    TDataBuffer::TFrameType aFrameType )
    {
    switch( aFrameType )
        {
        case TDataBuffer::KEthernetFrame:
            {
            return core_frame_type_ethernet;
            }
        case TDataBuffer::KDot11Frame:
            {
            return core_frame_type_dot11;
            }
        case TDataBuffer::KEthernetTestFrame:
            {
            return core_frame_type_test;
            }
        default:
            return core_frame_type_snap;
        }
    }

// ---------------------------------------------------------
// TWlanConversionUtil::ConvertFrameType()
// ---------------------------------------------------------
//
TDataBuffer::TFrameType TWlanConversionUtil::ConvertFrameType(
    core_frame_type_e aFrameType )
    {
    switch( aFrameType )
        {
        case core_frame_type_ethernet:
            {
            return TDataBuffer::KEthernetFrame;
            }
        case core_frame_type_dot11:
            {
            return TDataBuffer::KDot11Frame;
            }
        case core_frame_type_test:
            {
            return TDataBuffer::KEthernetTestFrame;
            }
        default:
            return TDataBuffer::KSnapFrame;
        }
    }
    
// ---------------------------------------------------------
// TWlanConversionUtil::ConvertPacketStatistics()
// ---------------------------------------------------------
//
void TWlanConversionUtil::ConvertPacketStatistics(
    TWlanPacketStatistics& aAmStatistics,
    const core_packet_statistics_s& aCoreStatistics )
    {
    aAmStatistics.txFrames = aCoreStatistics.tx_frames;
    aAmStatistics.rxFrames = aCoreStatistics.rx_frames;
    aAmStatistics.txMulticastFrames = aCoreStatistics.tx_multicast_frames;
    aAmStatistics.rxMulticastFrames = aCoreStatistics.rx_multicast_frames;
    aAmStatistics.fcsErrors = aCoreStatistics.fcs_errors;
    aAmStatistics.txRetries = aCoreStatistics.tx_retries;
    aAmStatistics.txErrors = aCoreStatistics.tx_errors;
    }

// ---------------------------------------------------------
// TWlanConversionUtil::ConvertUapsdSettings()
// ---------------------------------------------------------
//
void TWlanConversionUtil::ConvertUapsdSettings(
    core_uapsd_settings_s& aCoreSettings,
    const TWlanUapsdSettings& aAmSettings )
    {
    switch( aAmSettings.maxServicePeriodLength )
        {
        case EWlanMaxServicePeriodLengthTwo:
            aCoreSettings.max_service_period = core_max_service_period_length_two;
            break;
        case EWlanMaxServicePeriodLengthFour:
            aCoreSettings.max_service_period = core_max_service_period_length_four;
            break;
        case EWlanMaxServicePeriodLengthSix:
            aCoreSettings.max_service_period = core_max_service_period_length_six;
            break;
        case EWlanMaxServicePeriodLengthAll:
            /** Falls through on purpose. */
        default:
            aCoreSettings.max_service_period = core_max_service_period_length_all;
            break;
        }

    aCoreSettings.uapsd_enabled_for_voice = aAmSettings.uapsdForVoice;
    aCoreSettings.uapsd_enabled_for_video = aAmSettings.uapsdForVideo;
    aCoreSettings.uapsd_enabled_for_best_effort = aAmSettings.uapsdForBestEffort;
    aCoreSettings.uapsd_enabled_for_background = aAmSettings.uapsdForBackground;
    }

// ---------------------------------------------------------
// TWlanConversionUtil::ConvertUapsdSettings()
// ---------------------------------------------------------
//
void TWlanConversionUtil::ConvertUapsdSettings(
    TWlanUapsdSettings& aAmSettings,
    const core_uapsd_settings_s& aCoreSettings )
    {
    switch( aCoreSettings.max_service_period )
        {
        case core_max_service_period_length_two:
            aAmSettings.maxServicePeriodLength = EWlanMaxServicePeriodLengthTwo;
            break;
        case core_max_service_period_length_four:
            aAmSettings.maxServicePeriodLength = EWlanMaxServicePeriodLengthFour;
            break;
        case core_max_service_period_length_six:
            aAmSettings.maxServicePeriodLength = EWlanMaxServicePeriodLengthSix;
            break;
        case core_max_service_period_length_all:
            /** Falls through on purpose. */
        default:
            aAmSettings.maxServicePeriodLength = EWlanMaxServicePeriodLengthAll;
            break;
        }    

    aAmSettings.uapsdForVoice = aCoreSettings.uapsd_enabled_for_voice;
    aAmSettings.uapsdForVideo = aCoreSettings.uapsd_enabled_for_video;
    aAmSettings.uapsdForBestEffort = aCoreSettings.uapsd_enabled_for_best_effort;
    aAmSettings.uapsdForBackground = aCoreSettings.uapsd_enabled_for_background;
    }

// ---------------------------------------------------------
// TWlanConversionUtil::ConvertTxRatePolicies()
// ---------------------------------------------------------
//
void TWlanConversionUtil::ConvertTxRatePolicies(
    TTxRatePolicy& aAmPolicies,
    THtMcsPolicy& aAmMcsPolicies,
    TTxAutoRatePolicy& aAmAutoRatePolicies,
    const core_tx_rate_policies_s& aCorePolicies )
    {
    aAmPolicies.numOfPolicyObjects = aCorePolicies.policy_count;
    for ( TUint idx( 0 ); idx < aCorePolicies.policy_count; ++idx )
        {
        aAmPolicies.txRateClass[idx].txPolicy54 = aCorePolicies.policy[idx].tx_policy_54;
        aAmPolicies.txRateClass[idx].txPolicy48 = aCorePolicies.policy[idx].tx_policy_48;
        aAmPolicies.txRateClass[idx].txPolicy36 = aCorePolicies.policy[idx].tx_policy_36;
        aAmPolicies.txRateClass[idx].txPolicy33 = aCorePolicies.policy[idx].tx_policy_33;
        aAmPolicies.txRateClass[idx].txPolicy24 = aCorePolicies.policy[idx].tx_policy_24;
        aAmPolicies.txRateClass[idx].txPolicy22 = aCorePolicies.policy[idx].tx_policy_22;
        aAmPolicies.txRateClass[idx].txPolicy18 = aCorePolicies.policy[idx].tx_policy_18;
        aAmPolicies.txRateClass[idx].txPolicy12 = aCorePolicies.policy[idx].tx_policy_12;
        aAmPolicies.txRateClass[idx].txPolicy11 = aCorePolicies.policy[idx].tx_policy_11;
        aAmPolicies.txRateClass[idx].txPolicy9 = aCorePolicies.policy[idx].tx_policy_9;
        aAmPolicies.txRateClass[idx].txPolicy6 = aCorePolicies.policy[idx].tx_policy_6;
        aAmPolicies.txRateClass[idx].txPolicy5_5 = aCorePolicies.policy[idx].tx_policy_5p5;
        aAmPolicies.txRateClass[idx].txPolicy2 = aCorePolicies.policy[idx].tx_policy_2;
        aAmPolicies.txRateClass[idx].txPolicy1 = aCorePolicies.policy[idx].tx_policy_1;
        aAmPolicies.txRateClass[idx].shortRetryLimit  = aCorePolicies.policy[idx].short_retry_limit;
        aAmPolicies.txRateClass[idx].longRetryLimit = aCorePolicies.policy[idx].long_retry_limit;
        aAmPolicies.txRateClass[idx].flags = aCorePolicies.policy[idx].flags;
        aAmAutoRatePolicies[idx] = aCorePolicies.policy[idx].tx_auto_rate_policy;
        Mem::Copy(
            &aAmMcsPolicies[idx][0],
            &aCorePolicies.policy[idx].mcs_set[0],
            sizeof( THtMcsSet ) );
        }
    }

// ---------------------------------------------------------
// TWlanConversionUtil::ConvertTxRatePolicyMappings()
// ---------------------------------------------------------
//
void TWlanConversionUtil::ConvertTxRatePolicyMappings(
    TQueue2RateClass& aAmMappings,
    const core_tx_rate_policy_mappings_s& aCoreMappings )
    {
    aAmMappings[ELegacy] = aCoreMappings.policy_for_background;
    aAmMappings[EBackGround] = aCoreMappings.policy_for_background;
    aAmMappings[EVideo] = aCoreMappings.policy_for_video;
    aAmMappings[EVoice] = aCoreMappings.policy_for_voice;
    }

// ---------------------------------------------------------
// TWlanConversionUtil::ConvertTxRatePolicyInitialRates()
// ---------------------------------------------------------
//
void TWlanConversionUtil::ConvertTxRatePolicyInitialRates(
    TInitialMaxTxRate4RateClass& aInitialRates,
    const core_tx_rate_policies_s& aCorePolicies )
    {
    Mem::FillZ(
        &aInitialRates[0],
        KMaxNbrOfRateClasses );

    for ( TUint idx( 0 ); idx < aCorePolicies.policy_count; ++idx )
        {
        aInitialRates[idx] = aCorePolicies.policy[idx].initial_tx_rate;
        }
    }

// ---------------------------------------------------------
// TWlanConversionUtil::ConvertPowerSaveSettings()
// ---------------------------------------------------------
//
void TWlanConversionUtil::ConvertPowerSaveSettings(
    core_power_save_settings_s& aCoreSettings,
    const TWlanPowerSaveSettings& aAmSettings )
    {
    aCoreSettings.stay_in_uapsd_power_save_for_voice = aAmSettings.stayInUapsdPsModeForVoice;
    aCoreSettings.stay_in_uapsd_power_save_for_video = aAmSettings.stayInUapsdPsModeForVideo;
    aCoreSettings.stay_in_uapsd_power_save_for_best_effort = aAmSettings.stayInUapsdPsModeForBestEffort;
    aCoreSettings.stay_in_uapsd_power_save_for_background = aAmSettings.stayInUapsdPsModeForBackground;

    aCoreSettings.stay_in_legacy_power_save_for_voice = aAmSettings.stayInLegacyPsModeForVoice;
    aCoreSettings.stay_in_legacy_power_save_for_video = aAmSettings.stayInLegacyPsModeForVideo;
    aCoreSettings.stay_in_legacy_power_save_for_best_effort = aAmSettings.stayInLegacyPsModeForBestEffort;
    aCoreSettings.stay_in_legacy_power_save_for_background = aAmSettings.stayInLegacyPsModeForBackground;
    }

// ---------------------------------------------------------
// TWlanConversionUtil::ConvertPowerSaveSettings()
// ---------------------------------------------------------
//
void TWlanConversionUtil::ConvertPowerSaveSettings(
    TWlanPowerSaveSettings& aAmSettings,
    const core_power_save_settings_s& aCoreSettings )
    {
    aAmSettings.stayInUapsdPsModeForVoice = aCoreSettings.stay_in_uapsd_power_save_for_voice;
    aAmSettings.stayInUapsdPsModeForVideo = aCoreSettings.stay_in_uapsd_power_save_for_video;
    aAmSettings.stayInUapsdPsModeForBestEffort = aCoreSettings.stay_in_uapsd_power_save_for_best_effort;
    aAmSettings.stayInUapsdPsModeForBackground = aCoreSettings.stay_in_uapsd_power_save_for_background;    

    aAmSettings.stayInLegacyPsModeForVoice = aCoreSettings.stay_in_legacy_power_save_for_voice;
    aAmSettings.stayInLegacyPsModeForVideo = aCoreSettings.stay_in_legacy_power_save_for_video;
    aAmSettings.stayInLegacyPsModeForBestEffort = aCoreSettings.stay_in_legacy_power_save_for_best_effort;
    aAmSettings.stayInLegacyPsModeForBackground = aCoreSettings.stay_in_legacy_power_save_for_background;    
    }

// ---------------------------------------------------------
// TWlanConversionUtil::ConvertUapsdMaxServicePeriod()
// ---------------------------------------------------------
//
TMaxServicePeriodLength TWlanConversionUtil::ConvertUapsdMaxServicePeriod(
    core_max_service_period_length_e aServicePeriodLength )
    {
    switch( aServicePeriodLength )
        {
        case core_max_service_period_length_two:
            return EMaxServicePeriodLengthTwo;
        case core_max_service_period_length_four:
            return EMaxServicePeriodLengthFour;
        case core_max_service_period_length_six:
            return EMaxServicePeriodLengthSix;
        case core_max_service_period_length_all:
            /** Falls through on purpose. */
        default:
            return EMaxServicePeriodLengthAll;
        }  
    }

// ---------------------------------------------------------
// TWlanConversionUtil::ConvertTrafficStreamStatus()
// ---------------------------------------------------------
//
TWlanTrafficStreamStatus TWlanConversionUtil::ConvertTrafficStreamStatus(
    core_traffic_stream_status_e aStatus )
    {
    switch( aStatus )
        {
        case core_traffic_stream_status_active:
            return EWlanTrafficStreamStatusActive;
        case core_traffic_stream_status_inactive_not_required:
            return EWlanTrafficStreamStatusInactiveNotRequired;
        case core_traffic_stream_status_inactive_deleted_by_ap:
            return EWlanTrafficStreamStatusInactiveDeletedByAp;
        case core_traffic_stream_status_inactive_no_bandwidth:
            return EWlanTrafficStreamStatusInactiveNoBandwidth;
        case core_traffic_stream_status_inactive_invalid_parameters:
            return EWlanTrafficStreamStatusInactiveInvalidParameters;
        case core_traffic_stream_status_inactive_other:
            /** Falls through on purpose. */
        default:
            return EWlanTrafficStreamStatusInactiveOther;
        }   
    }

/* Compile time assertions are used to make sure that rate bits are same in core and in Symbian. */
COMPILE_ASSERT( TWlanRateNone    == core_tx_rate_none   );
COMPILE_ASSERT( TWlanRate1mbit   == core_tx_rate_1mbit  );
COMPILE_ASSERT( TWlanRate2mbit   == core_tx_rate_2mbit  );
COMPILE_ASSERT( TWlanRate5p5mbit == core_tx_rate_5p5mbit);
COMPILE_ASSERT( TWlanRate6mbit   == core_tx_rate_6mbit  );
COMPILE_ASSERT( TWlanRate9mbit   == core_tx_rate_9mbit  );
COMPILE_ASSERT( TWlanRate11mbit  == core_tx_rate_11mbit );
COMPILE_ASSERT( TWlanRate12mbit  == core_tx_rate_12mbit );
COMPILE_ASSERT( TWlanRate18mbit  == core_tx_rate_18mbit );
COMPILE_ASSERT( TWlanRate22mbit  == core_tx_rate_22mbit );
COMPILE_ASSERT( TWlanRate24mbit  == core_tx_rate_24mbit );
COMPILE_ASSERT( TWlanRate33mbit  == core_tx_rate_33mbit );
COMPILE_ASSERT( TWlanRate36mbit  == core_tx_rate_36mbit );
COMPILE_ASSERT( TWlanRate48mbit  == core_tx_rate_48mbit );
COMPILE_ASSERT( TWlanRate54mbit  == core_tx_rate_54mbit );

// ---------------------------------------------------------
// TWlanConversionUtil::ConvertApInformation()
// ---------------------------------------------------------
//
void TWlanConversionUtil::ConvertApInformation(
    TWlanAccessPointInfo& aAmInfo,
    const core_ap_information_s& aCoreInfo )
    {
    aAmInfo.ssid.Copy( aCoreInfo.ssid.ssid, Min(aCoreInfo.ssid.length, KWlanMaxSsidLength ) );
    aAmInfo.bssid.Copy( aCoreInfo.bssid.addr, KWlanMaxBssidLength );
    aAmInfo.capabilities = aCoreInfo.capabilities;
    aAmInfo.channel = aCoreInfo.channel;
    
    // RCPI -> RSSI CONVERSION
    // Note: conversion may round the result by 0.5 units
    aAmInfo.rssi = ( 110 - ( aCoreInfo.rcpi / 2 ) );
    aAmInfo.basicRates = aCoreInfo.basic_rates;
    aAmInfo.supportedRates = aCoreInfo.supported_rates;

    switch ( aCoreInfo.security_mode )
        {
        case core_connection_security_mode_open:
            aAmInfo.securityMode = EWlanConnectionExtentedSecurityModeOpen;
            break;
        case core_connection_security_mode_wep_open:
            aAmInfo.securityMode = EWlanConnectionExtentedSecurityModeWepOpen;
            break;
        case core_connection_security_mode_wep_shared:
            aAmInfo.securityMode = EWlanConnectionExtentedSecurityModeWepShared;
            break;
        case core_connection_security_mode_802d1x:
            aAmInfo.securityMode = EWlanConnectionExtentedSecurityMode802d1x;
            break;
        case core_connection_security_mode_wpa:
            aAmInfo.securityMode = EWlanConnectionExtentedSecurityModeWpa;
            break;
        case core_connection_security_mode_wpa2:
            aAmInfo.securityMode = EWlanConnectionExtentedSecurityModeWpa2;
            break;
        case core_connection_security_mode_wpa_psk:
            aAmInfo.securityMode = EWlanConnectionExtentedSecurityModeWpaPsk;
            break;
        case core_connection_security_mode_wpa2_psk:
            aAmInfo.securityMode = EWlanConnectionExtentedSecurityModeWpa2Psk;
            break;
        case core_connection_security_mode_wapi:
            aAmInfo.securityMode = EWlanConnectionExtentedSecurityModeWapi;
            break;
        case core_connection_security_mode_wapi_psk:
            aAmInfo.securityMode = EWlanConnectionExtentedSecurityModeWapiPsk;
            break;            
        default:
            aAmInfo.securityMode = EWlanConnectionExtentedSecurityModeOpen;
            break;
        }
    
    aAmInfo.isAcRequiredForVoice = aCoreInfo.is_ac_required_for_voice;
    aAmInfo.isAcRequiredForVideo = aCoreInfo.is_ac_required_for_video;
    aAmInfo.isAcRequiredForBestEffort = aCoreInfo.is_ac_required_for_best_effort;
    aAmInfo.isAcRequiredForBackground = aCoreInfo.is_ac_required_for_background;
    aAmInfo.isWpx = aCoreInfo.is_wpx;
    }

// ---------------------------------------------------------
// TWlanConversionUtil::ConvertRoamMetrics()
// ---------------------------------------------------------
//
void TWlanConversionUtil::ConvertRoamMetrics(
    TWlanRoamMetrics& aAmRoamMetrics,
    const core_roam_metrics_s& aCoreRoamMetrics )
    {
    aAmRoamMetrics.connectionAttemptTotalCount = aCoreRoamMetrics.connection_attempt_total_count;
    aAmRoamMetrics.unsuccesfullConnectionAttemptCount = aCoreRoamMetrics.unsuccesfull_connection_attempt_count;
    aAmRoamMetrics.roamingCounter = aCoreRoamMetrics.roaming_counter;
    aAmRoamMetrics.coverageLossCount = aCoreRoamMetrics.coverage_loss_count;
    aAmRoamMetrics.lastRoamTotalDuration = aCoreRoamMetrics.last_roam_total_duration;
    aAmRoamMetrics.lastRoamDataPathBrokenDuration = aCoreRoamMetrics.last_roam_data_path_broken_duration;

    switch ( aCoreRoamMetrics.last_roam_reason )
        {
        case core_roam_reason_none:
        case core_roam_reason_initial_connect:
        case core_roam_reason_signal_strength:
        case core_roam_reason_signal_loss_prediction:
        case core_roam_reason_directed_roam:
            aAmRoamMetrics.lastRoamReason = EWlanRoamReasonLowRssi;
            break;
        case core_roam_reason_failed_reauthentication:
        case core_roam_reason_media_disconnect:
        case core_roam_reason_bss_lost:
            aAmRoamMetrics.lastRoamReason = EWlanRoamReasonApLost;
            break;
        }
    }

// ---------------------------------------------------------
// TWlanConversionUtil::ConvertRogueList()
// ---------------------------------------------------------
//
void TWlanConversionUtil::ConvertRogueList(
    TWlmRogueList& aAmRogueList,
    core_type_list_c<core_mac_address_s>& aCoreRogueList )
    {
    aAmRogueList.count = 0;
    core_type_list_iterator_c<core_mac_address_s> iter( aCoreRogueList );
    iter.first();
    while ( iter.current() && aAmRogueList.count < KWlmRogueListMaxCount )
        {
        TWlanBssid bssid;
        bssid.Copy( iter.current()->addr, KWlanMaxBssidLength );
        aAmRogueList.list[aAmRogueList.count] = bssid;
        
        ++aAmRogueList.count;
        iter.next();
        }
    }

// ---------------------------------------------------------
// TWlanConversionUtil::ConvertTxQueueId()
// ---------------------------------------------------------
//
TQueueId TWlanConversionUtil::ConvertTxQueueId(
    core_access_class_e queue_id )
    {
    switch ( queue_id )
        {
        case core_access_class_voice:
            return EVoice;
        case core_access_class_video:
            return EVideo;
        case core_access_class_background:
            return EBackGround;
        case core_access_class_best_effort:
            /** Falls through on purpose. */
        default:
            return ELegacy;
        }
    }

// ---------------------------------------------------------
// TWlanConversionUtil::ConvertProtectedSetupCredentialAttribute()
// ---------------------------------------------------------
//
void TWlanConversionUtil::ConvertProtectedSetupCredentialAttribute(
    TWlanProtectedSetupCredentialAttribute& aAmAttribute,
    const core_iap_data_s& aCoreAttribute )
    {
    // Handle operating mode.
    if ( aCoreAttribute.op_mode == core_operating_mode_ibss )
        {
        aAmAttribute.iOperatingMode = EWlanOperatingModeAdhoc;
        }
    else
        {
        aAmAttribute.iOperatingMode = EWlanOperatingModeInfrastructure;        
        }

    // Handle authentication mode
    if ( aCoreAttribute.authentication_mode == core_authentication_mode_shared )
        {
        aAmAttribute.iAuthenticationMode = EWlanAuthenticationModeShared;
        }
    else
        {
        aAmAttribute.iAuthenticationMode = EWlanAuthenticationModeOpen;
        }

    // Handle security mode.
    switch( aCoreAttribute.security_mode )
        {        
        case core_security_mode_wep:
            aAmAttribute.iSecurityMode = EWlanIapSecurityModeWep;
            break;
        case core_security_mode_802dot1x:
            aAmAttribute.iSecurityMode = EWlanIapSecurityMode802d1x;
            break;
        case core_security_mode_wpa:
            aAmAttribute.iSecurityMode = EWlanIapSecurityModeWpa;
            break;
        case core_security_mode_wpa2only:
            aAmAttribute.iSecurityMode = EWlanIapSecurityModeWpa2Only;
            break;
        case core_security_mode_allow_unsecure:
            /** Falls through on purpose. */
        default:
            aAmAttribute.iSecurityMode = EWlanIapSecurityModeAllowUnsecure;
            break;
        }

    // Handle SSID
    aAmAttribute.iSsid.Copy(
        &aCoreAttribute.ssid.ssid[0],
        aCoreAttribute.ssid.length );

    // Handle WEP keys
    aAmAttribute.iWepDefaultKey = static_cast<TWlanDefaultWepKey>(
        aCoreAttribute.default_wep_key );

    aAmAttribute.iWepKey1.Copy(
        &aCoreAttribute.wep_key1.key_data[0],
        aCoreAttribute.wep_key1.key_length );

    aAmAttribute.iWepKey2.Copy(
        &aCoreAttribute.wep_key2.key_data[0],
        aCoreAttribute.wep_key2.key_length );

    aAmAttribute.iWepKey3.Copy(
        &aCoreAttribute.wep_key3.key_data[0],
        aCoreAttribute.wep_key3.key_length );

    aAmAttribute.iWepKey4.Copy(
        &aCoreAttribute.wep_key4.key_data[0],
        aCoreAttribute.wep_key4.key_length );

    // Handle WPA preshared key
    aAmAttribute.iWpaPreSharedKey.Copy(
        &aCoreAttribute.wpa_preshared_key.key_data[0],
        aCoreAttribute.wpa_preshared_key.key_length );
    }

// ---------------------------------------------------------
// TWlanConversionUtil::ConvertProtectedSetupStatus()
// ---------------------------------------------------------
//
TInt TWlanConversionUtil::ConvertProtectedSetupStatus(
    core_protected_setup_status_e aCoreStatus )
    {
    switch( aCoreStatus )
        {
        case core_protected_setup_status_ok:
            return KErrNone;
        case core_protected_setup_status_session_overlap: // fall through on purpose
        case core_protected_setup_status_multiple_PBC_sessions_detected:
            return KErrWlanProtectedSetupMultiplePBCSessionsDetected;
        case core_protected_setup_status_walktime_expired: // fall through on purpose
        case core_protected_setup_status_registration_session_timeout:
            return KErrWlanProtectedSetupRegistrationSessionTimeout;
        case core_protected_setup_status_network_auth_failure:
            return KErrWlanProtectedSetupNetworkAuthFailure;
        case core_protected_setup_status_network_assoc_failure:
            return KErrWlanProtectedSetupNetworkAssociationFailure;
        case core_protected_setup_status_OOB_interface_read_error:
            return KErrWlanProtectedSetupOOBInterfaceReadError;
        case core_protected_setup_status_decryption_CRC_failure:
            return KErrWlanProtectedSetupDecryptionCRCFailure;
        case core_protected_setup_status_RF_band_2_4_ghz_not_supported:
            return KErrWlanProtectedSetup2_4ChannelNotSupported;
        case core_protected_setup_status_RF_band_5_0_ghz_not_supported:
            return KErrWlanProtectedSetup5_0ChannelNotSupported;
        case core_protected_setup_status_signal_too_weak:
            return KErrWlanSignalTooWeak;
        case core_protected_setup_status_no_DHCP_response:
            return KErrWlanProtectedSetupNoDHCPResponse;
        case core_protected_setup_status_failed_DHCP_configure:
            return KErrWlanProtectedSetupFailedDHCPConfig;
        case core_protected_setup_status_ip_address_conflict:
            return KErrWlanProtectedSetupIPAddressConflict;
        case core_protected_setup_status_could_not_connect_to_registrar:
            return KErrWlanProtectedSetupCouldNotConnectToRegistrar;
        case core_protected_setup_status_rogue_activity_suspected:
            return KErrWlanProtectedSetupRogueActivitySuspected;
        case core_protected_setup_status_device_busy:
            return KErrWlanProtectedSetupDeviceBusy;
        case core_protected_setup_status_setup_locked:
            return KErrWlanProtectedSetupSetupLocked;
        case core_protected_setup_status_message_timeout:
            return KErrWlanProtectedSetupMessageTimeout;
        case core_protected_setup_status_device_password_authentication_failure:
            return KErrWlanProtectedSetupDevicePasswordAuthFailure;
        case core_protected_setup_status_pin_code_authentication_not_supported:
            return KErrWlanProtectedSetupPINMethodNotSupported;
        case core_protected_setup_status_push_button_authentication_not_supported:
            return KErrWlanProtectedSetupPBMethodNotSupported;
        default:
            return KErrGeneral;
        }
    }

// ---------------------------------------------------------
// TWlanConversionUtil::ConvertSnapHeader()
// ---------------------------------------------------------
//
void TWlanConversionUtil::ConvertSnapHeader(
    TSnapHeader& aAmHeader,
    const core_snap_header_s& aCoreHeader )
    {
    aAmHeader.iDSAP = aCoreHeader.dsap; 
    aAmHeader.iSSAP = aCoreHeader.ssap;
    aAmHeader.iControl = aCoreHeader.control;
    Mem::Copy(
        &aAmHeader.iOUI[0],
        &aCoreHeader.oui[0],
        KOuiLength );
    }

// ---------------------------------------------------------
// TWlanConversionUtil::ConvertPowerSaveMode()
// ---------------------------------------------------------
//
void TWlanConversionUtil::ConvertPowerSaveMode(
    core_power_save_mode_s& aCoreMode,
    TWlanPowerSaveMode aAmMode )
    {
    switch( aAmMode )
        {
        case EWlanPowerSaveModeNone:
            aCoreMode = CORE_POWER_SAVE_MODE_NONE;
            break;
        case EWlanPowerSaveModeBeacon:
            aCoreMode = CORE_POWER_SAVE_MODE_EVERY_BEACON;
            break;
        case EWlanPowerSaveModeDtim:
            aCoreMode = CORE_POWER_SAVE_MODE_EVERY_DTIM;
            break;
        case EWlanPowerSaveModeDtimSkipping:
            aCoreMode = CORE_POWER_SAVE_MODE_DTIM_SKIPPING;
            break;
        case EWlanPowerSaveModeAutomatic:
            /** Falls through purpose. */
        default:
            aCoreMode = CORE_POWER_SAVE_MODE_AUTOMATIC;
            break;
        }
    }

// ---------------------------------------------------------
// TWlanConversionUtil::ConvertFeatureFlags()
// ---------------------------------------------------------
//
u32_t TWlanConversionUtil::ConvertFeatureFlags(
    TUint aStaticFeatures,
    TUint aDynamicFeatures )
    {
    u32_t coreFeatures( core_feature_none );
    if( aStaticFeatures & CWlmServer::EWlanStaticFeaturePowerSaveTest )
        {
        coreFeatures |= core_feature_power_save_test;
        }
    if( aStaticFeatures & CWlmServer::EWlanStaticFeature802dot11k )
        {
        coreFeatures |= core_feature_802dot11k;
        }
    if( ( aStaticFeatures & CWlmServer::EWlanStaticFeature802dot11n ) &&
        ( aDynamicFeatures & EWlanRunTimeFeature802dot11n ) ) 
        {
        coreFeatures |= core_feature_802dot11n;
        }

    return coreFeatures;
    }

// ---------------------------------------------------------
// TWlanConversionUtil::ConvertFeatureFlags()
// ---------------------------------------------------------
//
TUint TWlanConversionUtil::ConvertFeatureFlags(
    u32_t aCoreFlags )
    {
    TUint ret( 0 );

    if( aCoreFlags & core_feature_802dot11n )
        {
        ret |= KWlanHtOperation;
        }

    return ret;
    }

// ---------------------------------------------------------
// TWlanConversionUtil::ConvertAccessClass()
// ---------------------------------------------------------
//
TWlmAccessClass TWlanConversionUtil::ConvertAccessClass(
    core_access_class_e aAccessClass )
    {
    switch( aAccessClass )
        {
        case core_access_class_voice:
            return EWlmAccessClassVoice;
        case core_access_class_video:
            return EWlmAccessClassVideo;
        case core_access_class_background:
            return EWlmAccessClassBackground;
        case core_access_class_best_effort:
            /** Falls through on purpose. */
        default:
            return EWlmAccessClassBestEffort;            
        }
    }

// ---------------------------------------------------------
// TWlanConversionUtil::ConvertTrafficMode()
// ---------------------------------------------------------
//
TWlmAcTrafficMode TWlanConversionUtil::ConvertTrafficMode(
    core_access_class_traffic_mode_e aMode )
    {
    if( aMode == core_access_class_traffic_mode_automatic )
        {
        return EWlmAcTrafficModeAutomatic;
        }
    
    return EWlmAcTrafficModeManual;    
    }

// ---------------------------------------------------------
// TWlanConversionUtil::ConvertTrafficStatus()
// ---------------------------------------------------------
//
TWlmAcTrafficStatus TWlanConversionUtil::ConvertTrafficStatus(
    core_access_class_traffic_status_e aStatus )
    {
    if( aStatus == core_access_class_traffic_status_admitted )
        {
        return EWlmAcTrafficStatusAdmitted;
        }
    
    return EWlmAcTrafficStatusNotAdmitted;
    }

// ---------------------------------------------------------
// TWlanConversionUtil::ConvertTrafficStreamParameters()
// ---------------------------------------------------------
//
void TWlanConversionUtil::ConvertTrafficStreamParameters(
    u8_t& aCoreTid,
    u8_t& aCoreUserPriority,
    core_traffic_stream_params_s& aCoreParams,
    const TWlanTrafficStreamParameters& aAmParams )
    {
    aCoreUserPriority =
        aAmParams.iUserPriority;
    aCoreParams =
        KWlmDefaultTsParams[KWlmUserPriorityToAccessClass[aCoreUserPriority]];

    if( aAmParams.iParameterBitmap & TWlanTrafficStreamParameters::EWlanParameterId )
        {
        aCoreTid = aAmParams.iId;
        }
    else
        {
        aCoreTid = TRAFFIC_STREAM_TID_NONE;
        }

    if( aAmParams.iParameterBitmap & TWlanTrafficStreamParameters::EWlanParameterType )
        {
        if( aAmParams.iType == EWlanTrafficStreamTrafficTypePeriodic )
            {
            aCoreParams.is_periodic_traffic = true_t;            
            }
        else
            {
            aCoreParams.is_periodic_traffic = false_t;
            }        
        }
    if( aAmParams.iParameterBitmap & TWlanTrafficStreamParameters::EWlanParameterDirection )
        {
        if( aAmParams.iDirection == EWlanTrafficStreamDirectionUplink )
            {
            aCoreParams.direction = core_traffic_stream_direction_uplink;
            }
        else if( aAmParams.iDirection == EWlanTrafficStreamDirectionDownlink )
            {
            aCoreParams.direction = core_traffic_stream_direction_downlink;
            }
        else
            {
            aCoreParams.direction = core_traffic_stream_direction_bidirectional;
            }
        }
    if( aAmParams.iParameterBitmap & TWlanTrafficStreamParameters::EWlanParameterNominalMsduSize )
        {
        aCoreParams.nominal_msdu_size = aAmParams.iNominalMsduSize;
        if( aAmParams.iIsMsduSizeFixed )
            {
            aCoreParams.nominal_msdu_size |= KWlmTsParamBitNominalSizeFixed;            
            }
        }
    if( aAmParams.iParameterBitmap & TWlanTrafficStreamParameters::EWlanParameterMaximumMsduSize )
        {
        aCoreParams.maximum_msdu_size = aAmParams.iMaximumMsduSize;
        }
    if( aAmParams.iParameterBitmap & TWlanTrafficStreamParameters::EWlanParameterMinimumServiceInterval )
        {
        aCoreParams.minimum_service_interval = aAmParams.iMinimumServiceInterval;
        }
    if( aAmParams.iParameterBitmap & TWlanTrafficStreamParameters::EWlanParameterMaximumServiceInterval )
        {
        aCoreParams.maximum_service_interval = aAmParams.iMaximumServiceInterval;
        }
    if( aAmParams.iParameterBitmap & TWlanTrafficStreamParameters::EWlanParameterInactivityInterval )
        {
        aCoreParams.inactivity_interval = aAmParams.iInactivityInterval;
        }
    if( aAmParams.iParameterBitmap & TWlanTrafficStreamParameters::EWlanParameterSuspensionInterval )
        {
        aCoreParams.suspension_interval = aAmParams.iSuspensionInterval;
        }
    if( aAmParams.iParameterBitmap & TWlanTrafficStreamParameters::EWlanParameterServiceStartTime )
        {
        aCoreParams.service_start_time = aAmParams.iServiceStartTime;
        }
    if( aAmParams.iParameterBitmap & TWlanTrafficStreamParameters::EWlanParameterMinimumDataRate )
        {
        aCoreParams.minimum_data_rate = aAmParams.iMinimumDataRate;
        }
    if( aAmParams.iParameterBitmap & TWlanTrafficStreamParameters::EWlanParameterMeanDataRate )
        {
        aCoreParams.mean_data_rate = aAmParams.iMeanDataRate;
        }
    if( aAmParams.iParameterBitmap & TWlanTrafficStreamParameters::EWlanParameterPeakDataRate )
        {
        aCoreParams.peak_data_rate = aAmParams.iPeakDataRate;
        }
    if( aAmParams.iParameterBitmap & TWlanTrafficStreamParameters::EWlanParameterMaximumBurstSize )
        {
        aCoreParams.maximum_burst_size = aAmParams.iMaximumBurstSize;
        }  
    if( aAmParams.iParameterBitmap & TWlanTrafficStreamParameters::EWlanParameterDelayBound )
        {
        aCoreParams.delay_bound = aAmParams.iDelayBound;
        }
    if( aAmParams.iParameterBitmap & TWlanTrafficStreamParameters::EWlanParameterMinimumPhyRate )
        {
        aCoreParams.minimum_phy_rate = ConvertTxRate( aAmParams.iMinimumPhyRate );
        }
    if( aAmParams.iParameterBitmap & TWlanTrafficStreamParameters::EWlanParameterNominalPhyRate )
        {
        aCoreParams.nominal_phy_rate =
            static_cast<core_tx_rate_e>( aAmParams.iNominalPhyRate );
        }
    if( aAmParams.iParameterBitmap & TWlanTrafficStreamParameters::EWlanParameterSba )
        {
        aCoreParams.surplus_bandwidth_allowance = aAmParams.iSba;
        }
    if( aAmParams.iParameterBitmap & TWlanTrafficStreamParameters::EWlanParameterIsRetryAllowed )
        {
        aCoreParams.is_retry_allowed = aAmParams.iIsAutomaticRetryAllowed;
        }
    }

// ---------------------------------------------------------
// TWlanConversionUtil::ConvertTxRate()
// ---------------------------------------------------------
//
u32_t TWlanConversionUtil::ConvertTxRate(
    TWlanRate aRate )
    {
    switch( aRate )
        {
        case TWlanRate54mbit:
            return 54000000;
        case TWlanRate48mbit:
            return 48000000;
        case TWlanRate36mbit:
            return 36000000;
        case TWlanRate33mbit:
            return 33000000;
        case TWlanRate24mbit:
            return 24000000;
        case TWlanRate22mbit:
            return 22000000;
        case TWlanRate18mbit:
            return 18000000;
        case TWlanRate12mbit:
            return 12000000;
        case TWlanRate11mbit:
            return 11000000;
        case TWlanRate9mbit:
            return 9000000;
        case TWlanRate6mbit:
            return 6000000;
        case TWlanRate5p5mbit:
            return 5500000;
        case TWlanRate2mbit:
            return 2000000;
        case TWlanRate1mbit:
            return 1000000;
        case TWlanRateNone:
            /** Falls through on purpose. */
        default:
            return 0;
        }
    }

// ---------------------------------------------------------
// TWlanConversionUtil::ConvertCipherKey()
// ---------------------------------------------------------
//
void TWlanConversionUtil::ConvertCipherKey(
    TPairwiseKeyData& aAmKey,
    const core_cipher_key_s& aCoreKey )
    {
    aAmKey.keyIndex = aCoreKey.key_index;
    aAmKey.length = aCoreKey.key_length;
    Mem::Copy(
        &aAmKey.data[0],
        &aCoreKey.key_data[0],
        MAX_CIPHER_KEY_LENGTH );
    }
