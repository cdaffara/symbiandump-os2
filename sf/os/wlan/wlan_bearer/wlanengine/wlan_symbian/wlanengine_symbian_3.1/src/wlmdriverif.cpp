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
* Description:  Interface to drivers
*
*/

/*
* %version: 52 %
*/

#include <wlanhwinit.h>
#include "wlanlddcommon.h"

#include "am_debug.h"
#include "wlmdriverif.h"
#include "wlanmgmtcommandhandler.h"
#include "wlanmgmtframehandler.h"
#include "wlannotificationhandler.h"
#include "wlanconversionutil.h"
#include "abs_core_driverif_callback.h"

#define PDD_FILE_NAME _L("wlanpdd")
#define PDD_NAME _L("wlan.phys")

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWlmDriverIf::CWlmDriverIf
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CWlmDriverIf::CWlmDriverIf() :
    iCoreServer( NULL ),
    iPendingRequestId( 0 ),
    iNotificationHandler( NULL ),
    iManagementCommandHandler( NULL ),
    iManagementFrameHandler( NULL ),
    iAsynchCallback( NULL ),
    iErrorStatus( core_error_ok ),
    iHwInit( NULL ),
    iIsPDDLoaded( EFalse ),
    iIsLDDLoaded( EFalse ),
    iManagementStatus( NULL )
    {
    DEBUG( "CWlmDriverIf::CWlmDriverIf()" );
    }

// -----------------------------------------------------------------------------
// CWlmDriverIf::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CWlmDriverIf* CWlmDriverIf::NewL()
    {
    DEBUG( "CWlmDriverIf::NewL()" );
    CWlmDriverIf* self = new(ELeave) CWlmDriverIf();    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CWlmDriverIf::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CWlmDriverIf::ConstructL()
    {
    DEBUG( "CWlmDriverIf::ConstructL()" );
    
    // Instantiate handlers
    iManagementCommandHandler = CWlanMgmtCommandHandler::NewL( iChannel, *this );
    iManagementFrameHandler = CWlanMgmtFrameHandler::NewL( iChannel, *this );
    iNotificationHandler = CWlanNotificationHandler::NewL( iChannel, *this );
    // Instantiate asynch callback provider
    // (mainly for error situations)
    TCallBack callback( AsynchCallbackFunction, this );
    iAsynchCallback = new(ELeave) CAsyncCallBack(
            callback, CActive::EPriorityStandard );
    iHwInit = CWlanHwInit::NewL();
    }
    
// Destructor
CWlmDriverIf::~CWlmDriverIf()
    {
    DEBUG( "CWlmDriverIf::~CWlmDriverIf()" );
    delete iAsynchCallback;
    delete iHwInit;
    delete iManagementCommandHandler;
    delete iManagementFrameHandler;
    delete iNotificationHandler;
    iCoreServer = NULL;
    iManagementStatus = NULL;
    }

// -----------------------------------------------------------------------------
// CWlmDriverIf::init
// -----------------------------------------------------------------------------
//
void CWlmDriverIf::init(
    abs_core_driverif_callback_c* core_callback )
    {
    iCoreServer = core_callback;
    }

// -----------------------------------------------------------------------------
// CWlmDriverIf::load_drivers
// -----------------------------------------------------------------------------
//
void CWlmDriverIf::load_drivers(
    u32_t request_id,
    uint_t rts_threshold,
    u32_t max_tx_msdu_lifetime,
    u32_t qos_null_frame_entry_timeout,
    u32_t qos_null_frame_entry_tx_count,
    u32_t qos_null_frame_interval,
    u32_t qos_null_frame_exit_timeout,
    u32_t keep_alive_interval,
    u32_t sp_rcpi_target,
    u32_t sp_time_target,
    u32_t sp_min_indication_interval,
    u32_t enabled_features )
    {
    DEBUG( "CWlmDriverIf::load_drivers()" );

    // Set request id and ensure no prior requests are pending
    if( SetRequestId( request_id ) )
        {
        ASSERT( false_t );
        return;
        }
    
    // Buffer for initialization data    
    const TUint8* pdaData = NULL;
    TUint pdaDataLength( 0 );
    // Buffer for firmware data
    const TUint8* fwData = NULL;
    TUint fwDataLength( 0 );
    // Get the data from CMT
    iHwInit->GetHwInitData( &pdaData, pdaDataLength, &fwData, fwDataLength );

    // Load physical device
    DEBUG( "- loading PDD" );
    TInt err = KErrNone;
    err = User::LoadPhysicalDevice( PDD_FILE_NAME );
    if( ( err != KErrNone ) && ( err != KErrAlreadyExists ) )
        {
        DEBUG1("ERROR: PDD loading failed with %d", err );
        UnloadDrivers();
        iErrorStatus = core_error_drivers_not_loaded;
        iAsynchCallback->CallBack();
        return;
        }
    iIsPDDLoaded = ETrue;

    // Load logical device
    DEBUG( "- loading LDD" );
    err = User::LoadLogicalDevice( LDD_FILE_NAME );
    if( ( err != KErrNone ) && ( err != KErrAlreadyExists ) )
        {
        DEBUG1("ERROR: LDD loading failed with %d", err );
        UnloadDrivers();
        iErrorStatus = core_error_drivers_not_loaded;
        iAsynchCallback->CallBack();
        return;
        }
    iIsLDDLoaded = ETrue;

    // Open logical channel
    DEBUG( "- Open logical channel" );
    TOpenParam openParam;
    openParam.iPda = const_cast<TUint8*>( pdaData );
    openParam.iPdaLength = pdaDataLength;
    openParam.iFirmWare = const_cast<TUint8*>( fwData );
    openParam.iFirmWareLength = fwDataLength;
    err = iChannel.Open( KUnitWlan, openParam );
    if( err )
        {
        DEBUG1("ERROR: Opening channel failed with %d", err );
        UnloadDrivers();
        iErrorStatus = core_error_drivers_not_loaded;
        iAsynchCallback->CallBack();
        return;
        }

    // Activate handlers
    DEBUG( "- Activate handlers" );
    if( iNotificationHandler->IsActive() ||
        iNotificationHandler->Start() )
        {
        DEBUG("ERROR starting notification handler");
        UnloadDrivers();
        iErrorStatus = core_error_drivers_not_loaded;
        iAsynchCallback->CallBack();
        return;
        }
    if( iManagementFrameHandler->IsActive() ||
        iManagementFrameHandler->Start() )
        {
        DEBUG("ERROR starting management frame handler");
        UnloadDrivers();
        iErrorStatus = core_error_drivers_not_loaded;
        iAsynchCallback->CallBack();
        return;
        }

    // Configure drivers
    iManagementCommandHandler->Configure(
        static_cast<u16_t>( rts_threshold ),
        max_tx_msdu_lifetime,
        qos_null_frame_entry_timeout,
        qos_null_frame_entry_tx_count,
        qos_null_frame_interval,
        qos_null_frame_exit_timeout,
        keep_alive_interval,
        sp_rcpi_target,
        sp_time_target,
        sp_min_indication_interval,
        TWlanConversionUtil::ConvertFeatureFlags( enabled_features ) );
    }

// -----------------------------------------------------------------------------
// CWlmDriverIf::unload_drivers
// -----------------------------------------------------------------------------
//
void CWlmDriverIf::unload_drivers( u32_t request_id )
    {
    DEBUG( "CWlmDriverIf::unload_drivers()" );

    // Set request id and ensure no prior requests are pending
    if( SetRequestId( request_id ) )
        {
        ASSERT( false_t );
        return;
        }
    
    // Unloading is actually asynchronous
    UnloadDrivers();
    
    // Order a callback
    iErrorStatus = core_error_ok;
    iAsynchCallback->CallBack();
    }

// -----------------------------------------------------------------------------
// CWlmDriverIf::UnloadDrivers
// -----------------------------------------------------------------------------
//
void CWlmDriverIf::UnloadDrivers()
    {
    DEBUG( "CWlmDriverIf::UnloadDrivers()" );
    // Cancel notification and management frame observations
    if( iNotificationHandler->IsActive() )
        {
        DEBUG( "- cancelling iNotificationHandler" );
        iNotificationHandler->Stop();
        }
    if( iManagementFrameHandler->IsActive() )
        {
        DEBUG( "- cancelling iManagementFrameHandler" );
        iManagementFrameHandler->Stop();
        }

    TInt err( KErrNone );
    if( iIsLDDLoaded )
        {
        DEBUG( "- closing logical channel" );
        iChannel.CloseChannel();
        err = User::FreeLogicalDevice( LDD_NAME );
        iIsLDDLoaded = EFalse;
        if ( err != KErrNone )
            {
            DEBUG1( "ERROR: FreeLogicalDevice() failed with %d", err );
            }
        }

    if( iIsPDDLoaded )
        {
        err = User::FreePhysicalDevice( PDD_NAME );
        iIsPDDLoaded = EFalse;
        if ( err != KErrNone )
            {
            DEBUG1( "ERROR: FreePhysicalDevice() failed with %d", err );
            }
        }
    DEBUG( "- UnloadDrivers DONE" );
    }

// -----------------------------------------------------------------------------
// CWlmDriverIf::start_ibss
// -----------------------------------------------------------------------------
//
void CWlmDriverIf::start_ibss(
    u32_t request_id,
    const core_ssid_s& ssid,
    u32_t beacon_interval,
    u32_t channel,
    core_encryption_mode_e encrypt_level )
    {
    DEBUG( "CWlmDriverIf::start_ibss()" );

    // Set request id and ensure no prior requests are pending
    if( SetRequestId( request_id ) )
        {
        ASSERT( false_t );
        return;
        }

    TSSID id;
    TWlanConversionUtil::ConvertSSID( id, ssid );
    iManagementCommandHandler->StartIBSS(
            id,
            beacon_interval,
            channel,
            TWlanConversionUtil::ConvertEncryptionMode( encrypt_level ) );
    }
       
// -----------------------------------------------------------------------------
// CWlmDriverIf::scan
// -----------------------------------------------------------------------------
//   
void CWlmDriverIf::scan(
    u32_t request_id,
    core_scan_mode_e scan_mode,
    const core_ssid_s& scan_ssid,
    int_t scan_rate,
    const core_scan_channels_s& scan_channels,
    u32_t scan_min_ch_time,
    u32_t scan_max_ch_time,
    bool_t is_split_scan )
    {
    DEBUG( "CWlmDriverIf::scan()" );

    // Set request id and ensure no prior requests are pending
    if( SetRequestId( request_id ) )
        {
        ASSERT( false_t );
        return;
        }

    // Convert needed parameters
    TSSID id;
    TWlanConversionUtil::ConvertSSID( id, scan_ssid );
    TScanMode scanmode = TWlanConversionUtil::ConvertScanMode( scan_mode );
    TRate rate = TWlanConversionUtil::ConvertScanRate( scan_rate );
    SChannels channels;
    TWlanConversionUtil::ConvertScanChannels( channels, scan_channels );

    // Send command to commandhandler
    iManagementCommandHandler->Scan(
        scanmode,
        id,
        rate,
        channels,
        scan_min_ch_time,
        scan_max_ch_time,
        is_split_scan );
    }

// -----------------------------------------------------------------------------
// CWlmDriverIf::stop_scan
// -----------------------------------------------------------------------------
//
void CWlmDriverIf::stop_scan(
    u32_t request_id )
    {
    DEBUG( "CWlmDriverIf::stop_scan()" );

    // Set request id and ensure no prior requests are pending
    if( SetRequestId( request_id ) )
        {
        ASSERT( false_t );
        return;
        }

    // Send command to commandhandler
    iManagementCommandHandler->StopScan();
    }

// -----------------------------------------------------------------------------
// CWlmDriverIf::disconnect
// -----------------------------------------------------------------------------
//     
void CWlmDriverIf::disconnect( u32_t request_id )
    {
    DEBUG( "CWlmDriverIf::disconnect()" );

    // Set request id and ensure no prior requests are pending
    if( SetRequestId( request_id ) )
        {
        ASSERT( false_t );
        return;
        }

    iManagementCommandHandler->Disconnect();
    }

// -----------------------------------------------------------------------------
// CWlmDriverIf::set_power_mode
// -----------------------------------------------------------------------------
//    
void CWlmDriverIf::set_power_mode(
    u32_t request_id,
    const core_power_mode_s& mode )
    {
    DEBUG( "CWlmDriverIf::set_power_mode()" );

    // Set request id and ensure no prior requests are pending
    if( SetRequestId( request_id ) )
        {
        ASSERT( false_t );
        return;
        }

    TPowerMode amMode( EPowerModeCam );
    TWlanWakeUpInterval amWakeUpModeLight( EWakeUpIntervalAllBeacons );
    TUint8 amWakeUpIntervalLight( 0 );
    TWlanWakeUpInterval amWakeUpModeDeep( EWakeUpIntervalAllDtims );
    TUint8 amWakeUpIntervalDeep( 0 );

    amMode = TWlanConversionUtil::ConvertPowerMode(
        mode.mode );    
    TWlanConversionUtil::ConvertWakeUpInterval(
        amWakeUpModeLight,
        amWakeUpIntervalLight,
        mode.wakeup_mode_light,
        mode.wakeup_interval_light );
    TWlanConversionUtil::ConvertWakeUpInterval(
        amWakeUpModeDeep,
        amWakeUpIntervalDeep,
        mode.wakeup_mode_deep,
        mode.wakeup_interval_deep );

    iManagementCommandHandler->SetPowerMode(
        amMode,
        mode.disable_dynamic_ps,
        amWakeUpModeLight,
        amWakeUpIntervalLight,
        amWakeUpModeDeep,
        amWakeUpIntervalDeep );
    }

// -----------------------------------------------------------------------------
// CWlmDriverIf::set_tx_power_level
// -----------------------------------------------------------------------------
//
void CWlmDriverIf::set_tx_power_level(
    u32_t request_id,
    u32_t tx_level )
    {
    DEBUG( "CWlmDriverIf::set_tx_power_level()" );

    // Set request id and ensure no prior requests are pending
    if( SetRequestId( request_id ) )
        {
        ASSERT( false_t );
        return;
        }

    // Send command to commandhandler
    iManagementCommandHandler->SetTxPowerLevel( tx_level ); 
    }

// -----------------------------------------------------------------------------
// CWlmDriverIf::add_cipher_key
// -----------------------------------------------------------------------------
//
void CWlmDriverIf::add_cipher_key(
    core_cipher_key_type_e cipher_suite,
    u8_t key_index,
    u16_t key_length,
    const u8_t* key_data,
    const core_mac_address_s& mac,
    bool_t use_as_default )
    {
    DEBUG( "CWlmDriverIf::add_cipher_key()" );

    TMacAddress macAddr;
    TWlanConversionUtil::ConvertMacAddress( macAddr, mac );

    iManagementCommandHandler->AddCipherKey(
        TWlanConversionUtil::ConvertCipherKeyType( cipher_suite ),
        key_index,                             // no conversion needed
        static_cast<TUint32>( key_length ),    // u16_t -> TUint32
        key_data,                              // no conversion needed
        macAddr,
        use_as_default );
    }

// -----------------------------------------------------------------------------
// CWlmDriverIf::get_current_rcpi
// -----------------------------------------------------------------------------
//
void CWlmDriverIf::get_current_rcpi(
    u32_t request_id,
    u32_t& rcpi )
    {
    // Set request id and ensure no prior requests are pending
    if( SetRequestId( request_id ) )
        {
        ASSERT( false_t );
        return;
        }

    DEBUG( "CWlmDriverIf::get_last_rcpi()" );
    iManagementCommandHandler->GetLastRCPI(
        rcpi ); // no conversion needed 
    }

// -----------------------------------------------------------------------------
// CWlmDriverIf::configure_multicast_group
// -----------------------------------------------------------------------------
//
void CWlmDriverIf::configure_multicast_group(
    u32_t request_id,
    bool_t join_group,
    const core_mac_address_s& multicast_addr )
    {
    // Set request id and ensure no prior requests are pending
    if( SetRequestId( request_id ) )
        {
        ASSERT( false_t );
        return;
        }

    DEBUG( "CWlmDriverIf::configure_multicast_group()" );
    TMacAddress mac;
    TWlanConversionUtil::ConvertMacAddress( mac, multicast_addr );
    iManagementCommandHandler->ConfigureMulticastGroup(
        join_group,
        mac );
    }

// -----------------------------------------------------------------------------
// CWlmDriverIf::set_bss_lost_parameters
// -----------------------------------------------------------------------------
//
void CWlmDriverIf::set_bss_lost_parameters(
    u32_t request_id,
    const core_bss_lost_parameters_s& parameters )
    {
    DEBUG( "CWlmDriverIf::set_bss_lost_parameters()" ); 

    // Set request id and ensure no prior requests are pending
    if( SetRequestId( request_id ) )
        {
        ASSERT( false_t );
        return;
        }   
    
    iManagementCommandHandler->SetBssLostParameters(
        parameters.beacon_lost_count,
        parameters.failed_tx_packet_count );
    }

// -----------------------------------------------------------------------------
// CWlmDriverIf::set_tx_rate_adaptation_parameters
// -----------------------------------------------------------------------------
//
void CWlmDriverIf::set_tx_rate_adaptation_parameters(
    u32_t request_id,
    const core_tx_rate_adaptation_parameters_s& parameters )
    {
    DEBUG( "CWlmDriverIf::set_tx_rate_adaptation_parameters()" );    

    // Set request id and ensure no prior requests are pending
    if( SetRequestId( request_id ) )
        {
        ASSERT( false_t );
        return;
        }    
    
    iManagementCommandHandler->SetTxRateAdaptationParameters(
        parameters.min_stepup_checkpoint,
        parameters.max_stepup_checkpoint,
        parameters.stepup_checkpoint_factor,
        parameters.stepdown_checkpoint,
        parameters.min_stepup_threshold,
        parameters.max_stepup_threshold,
        parameters.stepup_threshold_increment,
        parameters.stepdown_threshold,
        parameters.disable_probe_handling );
    }

// -----------------------------------------------------------------------------
// CWlmDriverIf::set_power_mode_mgmt_parameters
// -----------------------------------------------------------------------------
//
void CWlmDriverIf::set_power_mode_mgmt_parameters(
    u32_t request_id,
    const core_power_mode_mgmt_parameters_s& parameters )
    {
    DEBUG( "CWlmDriverIf::set_power_mode_mgmt_parameters()" );    
    
    // Set request id and ensure no prior requests are pending
    if( SetRequestId( request_id ) )
        {
        ASSERT( false_t );
        return;
        }
    
    iManagementCommandHandler->SetPowerModeMgmtParameters(
        parameters.active_to_light_timeout,
        parameters.active_to_light_threshold,
        parameters.light_to_active_timeout,
        parameters.light_to_active_threshold,
        parameters.light_to_deep_timeout,
        parameters.light_to_deep_threshold,
        parameters.uapsd_rx_frame_length_threshold );
    }

// -----------------------------------------------------------------------------
// CWlmDriverIf::set_tx_rate_policies
// -----------------------------------------------------------------------------
//
void CWlmDriverIf::set_tx_rate_policies(
    u32_t request_id,
    const core_tx_rate_policies_s& policies,
    const core_tx_rate_policy_mappings_s& mappings )
    {
    DEBUG( "CWlmDriverIf::set_tx_rate_policies()" );    

    // Set request id and ensure no prior requests are pending
    if( SetRequestId( request_id ) )
        {
        ASSERT( false_t );
        return;
        } 

    TTxRatePolicy amPolicies;
    THtMcsPolicy amMcsPolicies;
    TTxAutoRatePolicy amAutoRatePolicies;
    TWlanConversionUtil::ConvertTxRatePolicies( amPolicies, amMcsPolicies, amAutoRatePolicies, policies  ); 
    TQueue2RateClass amMappings;
    TWlanConversionUtil::ConvertTxRatePolicyMappings( amMappings, mappings );
    TInitialMaxTxRate4RateClass amInitialRates;
    TWlanConversionUtil::ConvertTxRatePolicyInitialRates( amInitialRates, policies );

    iManagementCommandHandler->SetTxRatePolicies(
        amPolicies,
        amMcsPolicies,
        amMappings,
        amInitialRates,
        amAutoRatePolicies );
    }

// -----------------------------------------------------------------------------
// CWlmDriverIf::get_packet_statistics
// -----------------------------------------------------------------------------
//
void CWlmDriverIf::get_packet_statistics(
    u32_t request_id,
    core_packet_statistics_by_access_category_s& statistics )
    {
    DEBUG( "CWlmDriverIf::get_packet_statistics()" );

    // Set request id and ensure no prior requests are pending
    if( SetRequestId( request_id ) )
        {
        ASSERT( false_t );
        return;
        } 

    iManagementCommandHandler->GetPacketStatistics(
        reinterpret_cast<TStatisticsResponse&>( statistics ) );
    }

// -----------------------------------------------------------------------------
// CWlmDriverIf::set_uapsd_settings
// -----------------------------------------------------------------------------
//
void CWlmDriverIf::set_uapsd_settings(
    u32_t request_id,
    const core_uapsd_settings_s& settings )
    {
    DEBUG( "CWlmDriverIf::set_uapsd_settings()" );

    // Set request id and ensure no prior requests are pending
    if( SetRequestId( request_id ) )
        {
        ASSERT( false_t );
        return;
        }

    iManagementCommandHandler->SetUapsdSettings(
        TWlanConversionUtil::ConvertUapsdMaxServicePeriod( settings.max_service_period ),
        settings.uapsd_enabled_for_voice,
        settings.uapsd_enabled_for_video,
        settings.uapsd_enabled_for_best_effort,
        settings.uapsd_enabled_for_background );
    }

// -----------------------------------------------------------------------------
// CWlmDriverIf::set_power_save_settings
// -----------------------------------------------------------------------------
//
void CWlmDriverIf::set_power_save_settings(
    u32_t request_id,
    const core_power_save_settings_s& settings )
    {
    DEBUG( "CWlmDriverIf::set_power_save_settings()" );

    // Set request id and ensure no prior requests are pending
    if( SetRequestId( request_id ) )
        {
        ASSERT( false_t );
        return;
        }

    iManagementCommandHandler->SetPowerSaveSettings(
        settings.stay_in_uapsd_power_save_for_voice,
        settings.stay_in_uapsd_power_save_for_video,
        settings.stay_in_uapsd_power_save_for_best_effort,
        settings.stay_in_uapsd_power_save_for_background,
        settings.stay_in_legacy_power_save_for_voice,
        settings.stay_in_legacy_power_save_for_video,
        settings.stay_in_legacy_power_save_for_best_effort,
        settings.stay_in_legacy_power_save_for_background );
    }

// -----------------------------------------------------------------------------
// CWlmDriverIf::set_tx_queue_parameters
// -----------------------------------------------------------------------------
//
void CWlmDriverIf::set_tx_queue_parameters(
    u32_t request_id,
    core_access_class_e queue_id,
    u16_t medium_time,
    u32_t max_tx_msdu_lifetime )
    {
    DEBUG( "CWlmDriverIf::set_tx_queue_parameters()" );

    // Set request id and ensure no prior requests are pending
    if( SetRequestId( request_id ) )
        {
        ASSERT( false_t );
        return;
        }

    iManagementCommandHandler->SetTxQueueParameters(
        TWlanConversionUtil::ConvertTxQueueId( queue_id ),
        medium_time,
        max_tx_msdu_lifetime );
    }

// -----------------------------------------------------------------------------
// CWlmDriverIf::set_rcpi_trigger_level
// -----------------------------------------------------------------------------
//
void CWlmDriverIf::set_rcpi_trigger_level(
    u32_t request_id,
    u8_t rcpi_trigger )
    {
    // Set request id and ensure no prior requests are pending
    if( SetRequestId( request_id ) )
        {
        ASSERT( false_t );
        return;
        }

    iManagementCommandHandler->SetRcpiTriggerLevel( rcpi_trigger ); 
    }

// -----------------------------------------------------------------------------
// CWlmDriverIf::enable_user_data
// -----------------------------------------------------------------------------
//
void CWlmDriverIf::enable_user_data( u32_t request_id )
    {
    DEBUG( "CWlmDriverIf::enable_user_data()" );

    // Set request id and ensure no prior requests are pending
    if( SetRequestId( request_id ) )
        {
        ASSERT( false_t );
        return;
        }

    iManagementCommandHandler->EnableUserData(); 
    }

// -----------------------------------------------------------------------------
// CWlmDriverIf::disable_user_data
// -----------------------------------------------------------------------------
//
void CWlmDriverIf::disable_user_data( u32_t request_id )
    {
    DEBUG( "CWlmDriverIf::disable_user_data()" );

    // Set request id and ensure no prior requests are pending
    if( SetRequestId( request_id ) )
        {
        ASSERT( false_t );
        return;
        }

    iManagementCommandHandler->DisableUserData(); 
    }

// -----------------------------------------------------------------------------
// CWlmDriverIf::connect
// -----------------------------------------------------------------------------
//
void CWlmDriverIf::connect(
    u32_t request_id,
    core_management_status_e& status,
    const core_ssid_s& ssid,
    const core_mac_address_s& bssid,
    u16_t auth_algorithm,
    core_encryption_mode_e encryption_level,
    core_cipher_key_type_e pairwise_key_type,
    bool_t is_infra,
    u32_t ie_data_length,
    const u8_t* ie_data,
    u32_t scan_frame_length,
    const u8_t* scan_frame,
    bool_t is_pairwise_key_invalidated,
    bool_t is_group_key_invalidated,
    bool_t is_radio_measurement_supported,
    const core_cipher_key_s* pairwise_key )
    {
#ifdef _DEBUG
    if( is_infra == true_t )
        {
        DEBUG( "CWlmDriverIf::connect() - infrastructure" );        
        }
    else
        {
        DEBUG( "CWlmDriverIf::connect() - adhoc" );
        }
#endif // _DEBUG

    // Set request id and ensure no prior requests are pending
    if( SetRequestId( request_id ) )
        {
        ASSERT( false_t );
        return;
        }

    iManagementStatus = &status;

    TSSID tmpSsid;
    TWlanConversionUtil::ConvertSSID( tmpSsid, ssid );
    TMacAddress tmpBssid;
    TWlanConversionUtil::ConvertMacAddress( tmpBssid, bssid );
    TPairwiseKeyData tmpKey = { 0, 0 };
    if( pairwise_key )
        {
        TWlanConversionUtil::ConvertCipherKey( tmpKey, *pairwise_key );
        }

    iManagementCommandHandler->Connect(
        tmpSsid,
        tmpBssid,
        auth_algorithm,
        TWlanConversionUtil::ConvertEncryptionMode( encryption_level ),
        TWlanConversionUtil::ConvertCipherKeyType( pairwise_key_type ),
        is_infra,
        ie_data_length,
        ie_data,
        scan_frame_length,
        scan_frame,
        is_pairwise_key_invalidated,
        is_group_key_invalidated,
        is_radio_measurement_supported,
        tmpKey );
    }

// -----------------------------------------------------------------------------
// CWlmDriverIf::cancel_request
// -----------------------------------------------------------------------------
//
void CWlmDriverIf::cancel_request(
    u32_t /* request_id */ )
    {
    iManagementCommandHandler->Cancel();
    }

// -----------------------------------------------------------------------------
// CWlmDriverIf::send_frame
// -----------------------------------------------------------------------------
//
void CWlmDriverIf::send_frame(
    core_frame_type_e frame_type,
    const u16_t frame_length,
    const u8_t* const frame_data,
    u8_t frame_priority,
    const core_mac_address_s& destination,
    bool_t send_encrypted )
    {
    DEBUG( "CWlmDriverIf::send_frame()" );

    TMacAddress macAddr;
    TWlanConversionUtil::ConvertMacAddress( macAddr, destination );
    
    iManagementFrameHandler->SendFrame(
        TWlanConversionUtil::ConvertFrameType( frame_type ),
        static_cast<u16_t>( frame_length ),
        frame_data,
        frame_priority,
        macAddr,
        send_encrypted );
    }

// -----------------------------------------------------------------------------
// CWlmDriverIf::set_arp_filter
// -----------------------------------------------------------------------------
//
void CWlmDriverIf::set_arp_filter(
    u32_t /* request_id */,
    const core_arp_filter_s& /* filter */ )
    {
    // Not supported currently
    ASSERT( false_t );
    return;
    }

// -----------------------------------------------------------------------------
// CWlmDriverIf::set_block_ack_usage
// -----------------------------------------------------------------------------
//
void CWlmDriverIf::set_block_ack_usage(
    u32_t request_id, 
    const core_block_ack_usage_s& usage )
    {
    // Set request id and ensure no prior requests are pending
    if( SetRequestId( request_id ) )
        {
        ASSERT( false_t );
        return;
        }

    iManagementCommandHandler->SetBlockAckUsage(
        usage.tx_usage,
        usage.rx_usage ); 
    }

// -----------------------------------------------------------------------------
// CWlmDriverIf::set_snap_header_filter
// -----------------------------------------------------------------------------
//
void CWlmDriverIf::set_snap_header_filter(
    u32_t request_id,
    const core_snap_header_s& header )
    {
    DEBUG( "CWlmDriverIf::set_snap_header_filter()" );

    // Set request id and ensure no prior requests are pending
    if( SetRequestId( request_id ) )
        {
        ASSERT( false_t );
        return;
        }

    TSnapHeader amHeader;
    TWlanConversionUtil::ConvertSnapHeader( amHeader, header );

    iManagementCommandHandler->SetSnapHeaderFilter(
        amHeader );
    }

// -----------------------------------------------------------------------------
// CWlmDriverIf::OnNotify
// -----------------------------------------------------------------------------
//
void CWlmDriverIf::OnNotify( const TIndication& aIndication )
    {
    DEBUG( "CWlmDriverIf::OnNotify" );
    iCoreServer->notify( TWlanConversionUtil::ConvertIndication( aIndication ) );
    }

// -----------------------------------------------------------------------------
// CWlmDriverIf::OnFrameReceive
// -----------------------------------------------------------------------------
//
void CWlmDriverIf::OnFrameReceive(
    const TDataBuffer::TFrameType aFrameType,
    const TUint aLength, 
    const TUint8* const aData,
    TUint8 aRcpi )
    {
    DEBUG( "CWlmDriverIf::ReceivePacket" );
    ASSERT( iCoreServer );
    iCoreServer->receive_frame(
        TWlanConversionUtil::ConvertFrameType( aFrameType ),
        static_cast<u16_t>( aLength ),
        aData,
        aRcpi );
    }

// -----------------------------------------------------------------------------
// CWlmDriverIf::OnRequestComplete
// -----------------------------------------------------------------------------
//
void CWlmDriverIf::OnRequestComplete( TInt status )
    {
    DEBUG1( "CWlmDriverIf::OnRequestComplete (status == %d)", status );
    // Save pending reqId to a local stack variable,
    // and reset the corresponding member variable in order to guarantee
    // that driverIf is capable to accept new request
    TUint32 tmp = iPendingRequestId;
    iPendingRequestId = 0;
    if ( iManagementStatus &&
         status > KErrNone )
        {
        *iManagementStatus = static_cast<core_management_status_e>( status );
        iManagementStatus = NULL;
        }

    iCoreServer->request_complete(
        tmp,
        TWlanConversionUtil::ConvertErrorCode( status ) );
    }

// -----------------------------------------------------------------------------
// CWlmDriverIf::AsynchCallbackFunction
// -----------------------------------------------------------------------------
//
TInt CWlmDriverIf::AsynchCallbackFunction( TAny* aThisPtr )
    {
    CWlmDriverIf* self = static_cast<CWlmDriverIf*>( aThisPtr );
    DEBUG2( "CWlmDriverIf::AsynchCallbackFunction (reqId == %d, status == %d)",
        self->iPendingRequestId, 
        self->iErrorStatus );    

    // Save pending reqId/errorStatus to a local stack variable,
    // and reset the corresponding member variables in order to guarantee
    // that driverIf is capable to accept new request
    TUint32 tmpReqId = self->iPendingRequestId;
    self->iPendingRequestId = 0;
    core_error_e tmpError = self->iErrorStatus;
    self->iErrorStatus = core_error_ok;
    
    self->iCoreServer->request_complete( tmpReqId, tmpError );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CWlmDriverIf::SetRequestId
// -----------------------------------------------------------------------------
//
TInt CWlmDriverIf::SetRequestId( TUint32 aRequestId )
    {
    // Save the request_id
    if( iPendingRequestId != 0 )
        {
        DEBUG("ERROR: command already pending");
        ASSERT( EFalse );
        iErrorStatus = core_error_drivers_not_loaded;
        iAsynchCallback->CallBack();
        return KErrInUse;
        }
    else
        {
        iPendingRequestId = aRequestId;
        iManagementStatus = NULL;
        return KErrNone;
        }
    }

// -----------------------------------------------------------------------------
// CWlmDriverIf::GetMacAddress
// -----------------------------------------------------------------------------
//
TInt CWlmDriverIf::GetMacAddress( TMacAddress& aMac )
    {
    TMacAddr tmp;
    TInt ret = iHwInit->GetMacAddress( tmp );

    TWlanConversionUtil::ConvertMacAddress( aMac, tmp );

    return ret;
    }

// -----------------------------------------------------------------------------
// CWlmDriverIf::Notify
// -----------------------------------------------------------------------------
//
void CWlmDriverIf::Notify( core_am_indication_e aIndication )
    {
    iCoreServer->notify( aIndication );
    }
