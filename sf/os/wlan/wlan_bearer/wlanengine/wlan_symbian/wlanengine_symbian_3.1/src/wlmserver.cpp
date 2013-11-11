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
* Description:  Server class of wlan engine
*
*/

/*
* %version: 111 %
*/

#include <e32def.h>
#include <e32std.h>
#include <featmgr.h>
#include <in_sock.h>
#include <bldvariant.hrh> // for feature definitions
#include <hal.h>

#include "wlmserver.h"
#include "wlmdriverif.h"
#include "wlmsession.h"
#include "wlanconversionutil.h"
#include "core_server_factory.h"
#include "core_tools.h"
#include "core_type_list.h"
#include "wlaneapolclient.h"
#include "wlangenericplugin.h"
#include "wlanssidlistdb.h"
#include "wlandevicesettingsinternalcrkeys.h"
#include "wlanbgscan.h"
#include "wlantimerservices.h"
#include "wlanticktimer.h"
#include "am_debug.h"

/** Panic codes for WlanEngine */
const TInt KWlmPanicCleanupStackCreationFailed  = 1;
const TInt KWlmPanicOpenSemaforeFailed          = 2;
const TInt KWlmPanicCreationOfSchedulerFailed   = 3;
/** Panic category of WlanEngine */
_LIT( KWlmModuleName, "WLANSRV" );

/** Constants for sanity checks */
const i32_t MAX_RCP_BOUNDARY = 255;
const i32_t MAX_HYSTERESIS = 255;
const u8_t MAX_USER_PRIORITY = 7; 

/** The number of hours after which the domain information expires. */
const TInt KWlmDomainInfoExpire = 5;
/** Operator MCC codes for North America, Canada, Taiwan, Brazil, Argentina, Mexico and Colombia. */
const TUint KWlmOperatorMccNATable[] = { 302, 310, 311, 312, 313, 314, 315, 316, 332, 466, 724, 722, 334, 732 };
/** The amount of operator MCC codes for North America, Canada, Taiwan, Brazil, Argentina, Mexico and Colombia. */
const TUint KWlmOperatorMccNATableLength = 14;

/** AP country information codes for North America, Canada, Taiwan, Brazil, Argentina, Mexico and Colombia
    are defined in core_types.h. */

/** Timestamp is stored in CenRep in minutes, constant for converterting this to microseconds */
const TUint KWlmTimestampInCenrep = 60000000; 

/** WLAN region values in CenRep */
const TInt KWlmRegionUnknown   = 0;
const TInt KWlmRegionETSI      = 1;
const TInt KWlmRegionFCC       = 2;

/** Valid range for cache lifetime in scan requests (in seconds). */
const TInt KWlmMaxScanCacheLifetime      = 60;
const TInt KWlmMinScanCacheLifetime      = 0;
const TInt KWlmDefaultScanCacheLifetime  = -1;

/** Valid range for maximum scan delay in scan requests (in seconds). */
const TUint KWlmMaxScanDelay             = 1200;

/** Infinite scan delay in GetAvailableIaps and GetScanResult. */
const TUint KWlmInfiniteScanDelay        = 0xFFFFFFFF;

/** Granularity for WLAN SSID lists. */
const TUint KWlanSsidListGranularity = 100;

/** Bitmask of WLAN features enabled at compile-time. */
const TUint KWlanStaticFeatures =
    CWlmServer::EWlanStaticFeature802dot11k |
    CWlmServer::EWlanStaticFeature802dot11n;

/** Minimum value for aggressive background scan interval, in seconds */
const TUint KWlanAggressiveBgScanMinInterval( 1 );

/** Maximum value for aggressive background scan interval, in seconds */
const TUint KWlanAggressiveBgScanMaxInterval( 120 );

/** Multiplier for deciding aggressive scan mode duration from set interval */
const TUint KWlanAggressiveBgScanDurationMultiplier( 6 );

/** Interval (in seconds) used for aggressive background scanning after
    unintentional link break. */
const TUint32 KWlanLinkBreakRecoveryInterval( 10 );

/** How long (in microseconds) aggressive background scanning is performed
    after unintentional link break. */
const TUint32 KWlanLinkBreakRecoveryDuration( 55000000 );  // 55 seconds

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWlmServer::CWlmServer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CWlmServer::CWlmServer() :
    CServer2( CActive::EPriorityStandard ),
    iCoreServer( NULL ),
    iDriverIf( NULL ),
    iSupportedFeatures( KWlanStaticFeatures ),
    iScanSchedulingTimer( NULL ),
    iCache( NULL ),
    iConfiguredCacheLifetime( 0 ),
    iPlatform( NULL ),
    iConnectionState( EWlanStateNotConnected ),
    iPrevConnectionState( EWlanStateNotConnected ),
    iIsRoaming( EFalse ),
    iPrevRcpiValue( 0 ),
    iRegion( EFCC ),
    iTimeofDomainQuery( 0 ),
    iClientSessionCount( 0 ),
    iRequestIdCounter( KWlanExtCmdBase ),
    iSessionIdCounter( 0 ),
    iCoreAsynchCb( NULL ),
    iCoreAsynchCbId( 0 ),
    iCoreAsynchCbStatus( core_error_ok ),
    iIsStartupComplete( EFalse ),
    iEapolClient( NULL ),
    iEapolHandler( NULL ),
    iSystemTickPeriod( 0 ),
    iScanSchedulingTimerExpiration( 0 ),
    iRequestTriggeringScanning( 0 ),
    iCoreHandlingScanRequest( EFalse ),
    iPowerSaveMode( EWlanPowerSaveAutomatic ),
    iPowerSaveEnabled( EFalse ),
    iSsidListDb( NULL ),
    iBgScanProvider( NULL ),
    iTimerServices( NULL ),
    iAggressiveScanningAfterLinkLoss( EFalse )
    {
    }

// -----------------------------------------------------------------------------
// CWlmServer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CWlmServer::ConstructL()
    {
    DEBUG( "CWlmServer::ConstructL()" );
    User::LeaveIfError( User::RenameThread( KWLMDataServerName ) );
    StartL( KWLMDataServerName );    

    HAL::Get( HAL::ESystemTickPeriod, iSystemTickPeriod );
    DEBUG1( "CWlmServer::ConstructL() - system tick period is %d",
        iSystemTickPeriod );
    DEBUG1( "CWlmServer::ConstructL() - current system tick count is %u",
        User::TickCount( ) );

    // Consult FeatureManager whether startup is allowed
    FeatureManager::InitializeLibL();
    if( !FeatureManager::FeatureSupported( KFeatureIdProtocolWlan ) )
        {
        DEBUG("ERROR: FeatureManager forbids starting of WlanServer");
        FeatureManager::UnInitializeLib();        
        User::Leave( KErrNotSupported );
        }

    if( FeatureManager::FeatureSupported( KFeatureIdFfWlanWapi ) )
        {
        iSupportedFeatures |= EWlanStaticFeatureWapi;
        }

    DEBUG1( "CWlmServer::ConstructL() - supported WLAN features: 0x%08X",
        iSupportedFeatures );
    FeatureManager::UnInitializeLib();

    // Create Driver Interface
    iDriverIf = CWlmDriverIf::NewL();

    // Get Mac Address
    TMacAddress mac( KZeroMacAddr );
    iDriverIf->GetMacAddress( mac );
    
    // Test its authenticity
    // If failure -> not starting the server
    const TMacAddress KDeadBeefMacAddr = {{ 0x00, 0xE0, 0xDE, 0xAD, 0xBE, 0xEF }};
    if( mac == KZeroMacAddr || mac == KDeadBeefMacAddr )
        {
        DEBUG("ERROR: MAC Address not acceptable -> Forbidden to start WlanServer");
        User::Leave( KErrNotSupported );
        }
    core_mac_address_s core_mac( ZERO_MAC_ADDR );
    TWlanConversionUtil::ConvertMacAddress( core_mac, mac );

    // Get Device Settings
    GetWlanSettingsL( iDeviceSettings );
    SetCachedRegion(iDeviceSettings.region, iDeviceSettings.regionTimestamp);    
    core_device_settings_s coreSettings; // core needs the settings here
    TWlanConversionUtil::ConvertDeviceSettings( coreSettings, iDeviceSettings );

    // Create core server    
    User::LeaveIfNull( iCoreServer = core_server_factory_c::instance(
        *this,
        *iDriverIf,
        coreSettings,
        core_mac,
        TWlanConversionUtil::ConvertFeatureFlags(
            iSupportedFeatures,
            iDeviceSettings.enabledFeatures ) ) );
    iCoreServer->disable_wlan( KWlanIntCmdDisableWlan );

    // Create scan timer
    DEBUG( "CWlmServer::ConstructL() - create backgroundscan timer" );
    TCallBack expiredCb( ScanSchedulingTimerExpired, this );
    TCallBack canceledCb( ScanSchedulingTimerCanceled, this );
    TCallBack emptyCb( EmptyCb, NULL );
    iScanSchedulingTimer = CWlanTickTimer::NewL(
        expiredCb,
        canceledCb,
        emptyCb );

    // Create scan cache
    iCache = CWlanScanResultCache::NewL();
    iConfiguredCacheLifetime = iDeviceSettings.scanExpirationTimer;

    // Create a callback for asynchronous core requests
    TCallBack callback( HandleCoreAsynchCb, this );
    iCoreAsynchCb = new (ELeave) CAsyncCallBack(
        callback, CActive::EPriorityStandard );

    // Create SSID list storage.
    iSsidListDb = CWlanSsidListDb::NewL();
    
    iTimerServices = CWlanTimerServices::NewL();
    
    iBgScanProvider = CWlanBgScan::NewL( static_cast<MWlanScanResultProvider&>( *this ), *iTimerServices );
    
    // Initialize Platform interface and publish mac address
    iPlatform = CWlmPlatform::NewL( *this );
    iPlatform->Initialize();
    TInt err = iPlatform->PublishMacAddress( mac );
    if( err )
        {
        DEBUG1( "ERROR publishing MAC address: %d", err );
        }    

    iGenericWlanPlugin.StartPlugins();
    
    DEBUG( "CWlmServer::ConstructL() - done" );    
    }

// -----------------------------------------------------------------------------
// CWlmServer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CWlmServer* CWlmServer::NewL()
    {
    CWlmServer* self = new( ELeave ) CWlmServer;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// -----------------------------------------------------------------------------
// CWlmServer::~CWlmServer
// -----------------------------------------------------------------------------
//
CWlmServer::~CWlmServer()
    {
    // Close all open sessions
    CWlmSession* session = NULL;
    iSessionIter.SetToFirst();
    while( (session = static_cast<CWlmSession*>(iSessionIter++)) != NULL )
        {
        delete session;
        }

    // Stop all plugins
    iGenericWlanPlugin.StopPlugins();

    iIapWeakList.Close();
    delete iCoreServer;
    delete iDriverIf;
    iNotificationArray.Close();
    delete iCache;
    delete iPlatform;
    iRequestMap.Close();
    delete iCoreAsynchCb;
    delete iScanSchedulingTimer;
    delete iSsidListDb;
    delete iBgScanProvider;
    delete iTimerServices;
    delete iEapolClient;
    iEapolHandler = NULL;
    REComSession::FinalClose();    
    }

// -----------------------------------------------------------------------------
// CWlmServer::StartServerThread
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CWlmServer::StartServerThread()
    {
    DEBUG( "CWlmServer::StartServerThread()" );

    __UHEAP_MARK;

    // Create cleanup stack
    CTrapCleanup* cleanup = CTrapCleanup::New();
    __ASSERT_ALWAYS( 
        cleanup, 
        User::Panic( KWlmModuleName, KWlmPanicCleanupStackCreationFailed ) 
        );

    // Open handle to semaphore
    RSemaphore started;
    TInt err = started.OpenGlobal( KWLMServerSemaphore, EOwnerProcess );
    __ASSERT_ALWAYS(
        err == KErrNone, 
        User::Panic( KWlmModuleName, KWlmPanicOpenSemaforeFailed )
        );

    // Create active scheduler   
    CWlmServer* server = NULL;
    CActiveScheduler* scheduler = new CActiveScheduler();
    if ( scheduler )
        {
        // Install scheduler
        CActiveScheduler::Install( scheduler );

        // Create server
        TRAP( err, server = CWlmServer::NewL() );

        if ( err != KErrNone )
            {
            DEBUG1( "CWlmServer::StartServerThread() - NewL leaved with code %d", err );
            }
        }
    else
        {
        User::Panic( KWlmModuleName, KWlmPanicCreationOfSchedulerFailed );
        }

    // Signal calling process that we've started via semaphore
    started.Signal(); 
    started.Close();

    // Start the active Scheduler (if there are no errors)
    if ( err == KErrNone )
        {
        DEBUG( "CWlmServer::StartServerThread() - Starting scheduler..." );
        CActiveScheduler::Start();
        }
    
    // Note that CActiveScheduler::Start() will not return until the scheduler
    // is stopped in CWlmServer::SessionClosed(). This is because we don't
    // belong to an active object, so there's no RunL() to be called.

    // Delete all objects
    delete server;
    delete CActiveScheduler::Current();
    delete cleanup;

    // Clean up Ecom framework
    REComSession::FinalClose();
    
    __UHEAP_MARKEND;

    return err;
    }

// ---------------------------------------------------------
// CWlmServer::GetWlanSettingsL
// ---------------------------------------------------------
//
void CWlmServer::GetWlanSettingsL(
    CWlanDeviceSettings::SWlanDeviceSettings& aSettings )
    {
    DEBUG( "CWlmServer::GetWlanSettingsL()" );
    CWlanDeviceSettings* db = CWlanDeviceSettings::NewL();
    CleanupStack::PushL( db );    
    db->ReadL( aSettings );
    CleanupStack::PopAndDestroy( db );
    }

// ---------------------------------------------------------
// CWlmServer::GetIapSettingsL
// ---------------------------------------------------------
//
void CWlmServer::GetIapSettingsL(
    const TUint32 aLanServiceId,
    SWLANSettings& aWlanSettings,
    RArray<TWlanSecondarySsid>& aSecondaryList )
    {
    DEBUG( "CWlmServer::GetIapSettingsL()" );

    CWLanSettings* wlanset = new(ELeave) CWLanSettings;
    CleanupStack::PushL( wlanset );

    User::LeaveIfError( wlanset->Connect() );
    User::LeaveIfError(
        wlanset->GetWlanSettingsForService( aLanServiceId, aWlanSettings ) );
	User::LeaveIfError(
		wlanset->GetSecondarySsidsForService( aWlanSettings.Id, aSecondaryList ) );

    DEBUG3( "- connMode == %d, securityMode == %d, # of secondary SSIDs == %d",
        aWlanSettings.ConnectionMode,
        aWlanSettings.SecurityMode,
        aSecondaryList.Count() );

    wlanset->Disconnect();
    CleanupStack::PopAndDestroy( wlanset );
    }

// ---------------------------------------------------------
// CWlmServer::NewSessionL
// ---------------------------------------------------------
//
CSession2* CWlmServer::NewSessionL(
    const TVersion& /*aVersion*/,
    const RMessage2& /*aMessage*/ ) const
    {
    DEBUG( "CWlmServer::NewSessionL()" );
    CWlmSession* session =
        CWlmSession::NewL(
            const_cast<CWlmServer&>(*this) );
    return session;
    }

// ---------------------------------------------------------
// CWlmServer::NotifyAdd
// ---------------------------------------------------------
//
void CWlmServer::NotifyAdd(
    CNotificationBase& aNotification )
    {
    DEBUG( "CWlmServer::NotifyAdd()" );
    iNotificationArray.Insert( &aNotification, 0 );
    }

// ---------------------------------------------------------
// CWlmServer::NotifyRemove
// ---------------------------------------------------------
//
void CWlmServer::NotifyRemove(
    CNotificationBase& aNotification )
    {
    DEBUG( "CWlmServer::NotifyRemove()" );
    TInt index = iNotificationArray.Find( &aNotification );
    iNotificationArray.Remove( index );
    }

// ---------------------------------------------------------
// CWlmServer::Connect
// ---------------------------------------------------------
//
void CWlmServer::Connect(
    TUint aSessionId,
    const RMessage2& aMessage )
    {
    DEBUG( "CWlmServer::Connect()" );

    // Check that WLAN is ON
    TWlanOnOffState wlanState( iPlatform->GetWlanOnOffState() );
	if( wlanState != EWlanOn )
	    {
		// WLAN is OFF
		DEBUG1( "CWlmServer::Connect() - refused due to WLAN is OFF (%d)",
		    wlanState );
		// WLAN state enumerations map one to one to WLAN error code
		aMessage.Complete( wlanState );
		
		return;
		}

    // Get WlanSettings and secondarySSID list
    // (lanServiceId specifies the table row in wlansettings)
    SWLANSettings iapData;
    RArray<TWlanSecondarySsid> secondarySsidList;
    TInt lanServiceId = aMessage.Int0();
    TRAPD( err, GetIapSettingsL( lanServiceId, iapData, secondarySsidList ) ) ;
    if( err != KErrNone )
        {
        DEBUG1( "CWlmServer::Connect() - GetIapSettingsL leaved with %d",
            err );
        secondarySsidList.Close();        
        aMessage.Complete( err );

        return;
        }

    DEBUG5( "CWlmServer::Connect() - SecurityMode: %u, WPAKeyLength: %u, WPAPreSharedKey: %u, EnableWpaPsk: %u, PresharedKeyFormat: %u",
        iapData.SecurityMode, iapData.WPAKeyLength, iapData.WPAPreSharedKey.Length(),
        iapData.EnableWpaPsk, iapData.PresharedKeyFormat );

    // Check whether WAPI is supported
    if( iapData.SecurityMode == Wapi &&
        !( iSupportedFeatures & EWlanStaticFeatureWapi ) )
        {
        DEBUG( "CWlmServer::Connect() - WAPI is not supported" );

        secondarySsidList.Close();        
        aMessage.Complete( KErrNotSupported );

        return;        
        }

    // Get override setting from message parameter
    TWLMOverrideSettings override = { 0 };
    TPckg<TWLMOverrideSettings> inData( override );
    err = aMessage.Read( 1, inData );
    if( err )
        {
        secondarySsidList.Close();
        aMessage.Complete( err );

        return;
        }

    // Type conversion
    core_iap_data_s* coreIapData = new core_iap_data_s;
    if( !coreIapData )
        {
        secondarySsidList.Close();
        aMessage.Complete( KErrNoMemory );

        return;
        }

    // Find out whether IP address should be static or dhcp
    CLanSettings lanSettingsClient;
    err = lanSettingsClient.Connect();
    if( err )
        {
        secondarySsidList.Close();
        delete coreIapData;
        aMessage.Complete( err );

        return;
        }
    SLanSettings lanSettings;
    lanSettingsClient.GetLanSettings( lanServiceId, lanSettings );
    lanSettingsClient.Disconnect();

    TWlanConversionUtil::ConvertIapSettings(
        *coreIapData, 
        iapData, 
        lanSettings.AddrFromServer,
        override );

    //
    // Handle secondary SSID list
    //
    core_type_list_c<core_ssid_entry_s>* coreSsidList = NULL;

    if( iapData.SSID.Length() &&
        ( iapData.UsedSSID.Length() || secondarySsidList.Count() ) )
        {
        DEBUG( "CWlmServer::Connect() - secondary SSIDs defined" );

        coreSsidList = new core_type_list_c<core_ssid_entry_s>();
        if( !coreSsidList )
            {
            secondarySsidList.Close();
            delete coreIapData;
            aMessage.Complete( KErrNoMemory );

            return;
            }
        coreSsidList->clear();

        core_ssid_entry_s* entry = new core_ssid_entry_s;
        if ( !entry )
            {
            secondarySsidList.Close();
            delete coreIapData;
            delete coreSsidList;
            aMessage.Complete( KErrNoMemory );

            return;
            }

        entry->id = coreIapData->id;
        TWlanConversionUtil::ConvertSSID( entry->ssid, iapData.SSID );
        if( iapData.UsedSSID.Length() )
            {
            TWlanConversionUtil::ConvertSSID( entry->used_ssid, iapData.UsedSSID );
            }
        else
            {
            entry->used_ssid = entry->ssid;
            }
        coreSsidList->append( entry );

        for( TInt idx( 0 ); idx < secondarySsidList.Count(); ++idx )
            {
            entry = new core_ssid_entry_s;
            if ( !entry )
                {
                secondarySsidList.Close();
                delete coreIapData;
                delete coreSsidList;
                aMessage.Complete( KErrNoMemory );

                return;                    
                }

            TWlanConversionUtil::ConvertSSID( 
                entry->ssid, secondarySsidList[idx].ssid );
            if ( secondarySsidList[idx].usedSsid.Length() )
                {
                TWlanConversionUtil::ConvertSSID(
                    entry->used_ssid, secondarySsidList[idx].usedSsid );
                }
            else
                {
                TWlanConversionUtil::ConvertSSID(
                    entry->used_ssid, iapData.SSID );
                }
            coreSsidList->append( entry );
            }

        DEBUG1( "CWlmServer::Connect() - secondary SSID count %u",
            coreSsidList->count() );
        }
    else
        {
        DEBUG( "CWlmServer::Connect() - no secondary SSIDs defined" );
        }
    secondarySsidList.Close();

    // Connection status
    core_connect_status_e* connectionStatus = new core_connect_status_e;
    if( !connectionStatus )
        {
        secondarySsidList.Close();
        delete coreIapData;
        delete coreSsidList;
        aMessage.Complete( KErrNoMemory );
        return;
        }
    *connectionStatus = core_connect_undefined;
    
    TInt ret = GetCurrentIapId(
        static_cast<TUint>(lanServiceId), 
        *coreIapData );
    iConnectionIapId = coreIapData->iap_id;

    if (  ret != KErrNone )
        {
        secondarySsidList.Close();
        delete coreIapData;
        delete coreSsidList;
        aMessage.Complete( ret );
        return;    
        }
    
    // create mapping
    SRequestMapEntry mapEntry;
    mapEntry.iMessage = aMessage;
    mapEntry.iFunction = EJoinByProfileId;
    mapEntry.iRequestId = iRequestIdCounter++;
    mapEntry.iSessionId = aSessionId;
    mapEntry.iParam0 = coreIapData;
    mapEntry.iParam1 = coreSsidList;
    mapEntry.iParam2 = connectionStatus;
    iRequestMap.Append( mapEntry );

    iCoreServer->connect(
        mapEntry.iRequestId,
        *coreIapData,
        *connectionStatus,
        coreSsidList );
    }

// ---------------------------------------------------------
// CWlmServer::CancelConnect
// ---------------------------------------------------------
//
void CWlmServer::CancelConnect(
    TUint /* aSessionId */,
    const RMessage2& aMessage )
    {
    DEBUG( "CWlmServer::CancelConnect()" );

    iAggressiveScanningAfterLinkLoss = EFalse;

    aMessage.Complete( KErrNone );
    }

// ---------------------------------------------------------
// CWlmServer::GetCurrentBSSID
// ---------------------------------------------------------
//
void CWlmServer::GetCurrentBSSID(
    TUint /* aSessionId */,
    const RMessage2& aMessage )
    {
    DEBUG( "CWlmServer::GetCurrentBSSID()" );

    core_mac_address_s coreBssid( ZERO_MAC_ADDR );
    core_error_e ret = iCoreServer->get_current_bssid( coreBssid );
    if ( ret != core_error_ok )
        {
        DEBUG1( "CWlmServer::GetCurrentBSSID() - get_current_bssid() failed with %u",
            ret );
        }

    TMacPckg bssid;
    TWlanConversionUtil::ConvertMacAddress(
        bssid.data,
        coreBssid );
    TPckg<TMacPckg> outPckg( bssid );    
    aMessage.Write( 0, outPckg );
    aMessage.Complete(
        TWlanConversionUtil::ConvertErrorCode( ret ) );
    }

// ---------------------------------------------------------
// CWlmServer::GetCurrentRCPI
// ---------------------------------------------------------
//
void CWlmServer::GetCurrentRCPI(
    TUint aSessionId,
    const RMessage2& aMessage )
    {
    DEBUG( "CWlmServer::GetCurrentRCPI()" );    

    if ( IsRoaming() )
        {
        DEBUG1( "CWlmServer::GetCurrentRCPI() - roam in progress, returning a cached value (%u)",
            iPrevRcpiValue );

        TPckg<TUint32> outPckg( iPrevRcpiValue );
        aMessage.Write( 0, outPckg );
        aMessage.Complete( KErrNone );

        return;        
        }

    // create mapping
    SRequestMapEntry mapEntry;
    mapEntry.iMessage = aMessage;
    mapEntry.iFunction = EGetCurrentRSSI;
    mapEntry.iRequestId = iRequestIdCounter++;
    mapEntry.iSessionId = aSessionId;    
    TUint32* tmp = new TUint32;
    if( tmp == NULL )
        {
        aMessage.Complete( KErrNoMemory );
        return;
        }
    mapEntry.iParam0 = tmp;
    iRequestMap.Append( mapEntry );

    // pass request to core
    iCoreServer->get_current_rcpi(
        mapEntry.iRequestId,
        *tmp );
    }

// ---------------------------------------------------------
// CWlmServer::GetCurrentSSID
// ---------------------------------------------------------
//
void CWlmServer::GetCurrentSSID(
    TUint /* aSessionId */,
    const RMessage2& aMessage )
    {
    DEBUG( "CWlmServer::GetCurrentSSID()" );

    core_ssid_s coreSsid( BROADCAST_SSID );
    core_error_e ret = iCoreServer->get_current_ssid( coreSsid );
    if ( ret != core_error_ok )
        {
        DEBUG1( "CWlmServer::GetCurrentSSID() - get_current_ssid() failed with %u",
            ret );
        }

    TSSID ssid = { 0 };
    TWlanConversionUtil::ConvertSSID(
        ssid,
        coreSsid );
    TPckg<TSSID> outPckg( ssid );    
    aMessage.Write( 0, outPckg );
    aMessage.Complete(
        TWlanConversionUtil::ConvertErrorCode( ret ) );
    }

// ---------------------------------------------------------
// CWlmServer::GetCurrentSecurityMode
// ---------------------------------------------------------
//
void CWlmServer::GetCurrentSecurityMode(
    TUint /* aSessionId */,
    const RMessage2& aMessage )
    {
    DEBUG( "CWlmServer::GetCurrentSecurityMode()" );

    core_connection_security_mode_e coreMode( core_connection_security_mode_open );
    core_error_e ret = iCoreServer->get_current_security_mode( coreMode );
    if ( ret != core_error_ok )
        {
        DEBUG1( "CWlmServer::GetCurrentSecurityMode() - get_current_security_mode() failed with %u",
            ret );
        }

    TWlanSecurity mode(
        TWlanConversionUtil::ConvertSecurityMode( coreMode ) );
    TPckg<TWlanSecurity> outPckg( mode );
    aMessage.Write( 0, outPckg );
    aMessage.Complete(
        TWlanConversionUtil::ConvertErrorCode( ret ) );
    }

// ---------------------------------------------------------
// CWlmServer::AddBssidToRoguelist
// ---------------------------------------------------------
//
void CWlmServer::AddBssidToRoguelist(
    TUint /* aSessionId */,
    const RMessage2& aMessage )
    {
    DEBUG( "CWlmServer::AddBssidToRoguelist()" );

    TMacAddress bssid( KZeroMacAddr );
    TPckg<TMacAddress> inPckg( bssid );
    TInt err( aMessage.Read( 0, inPckg ) );
    if( err != KErrNone )
        {
        aMessage.Complete( err );
        return;
        }

    core_mac_address_s coreBssid( ZERO_MAC_ADDR );
    TWlanConversionUtil::ConvertMacAddress(
        coreBssid,
        bssid );

    core_error_e ret = iCoreServer->add_bssid_to_rogue_list( coreBssid );
    if ( ret != core_error_ok )
        {
        DEBUG1( "CWlmServer::AddBssidToRoguelist() - add_bssid_to_rogue_list() failed with %u",
            ret );
        }

    aMessage.Complete(
        TWlanConversionUtil::ConvertErrorCode( ret ) );
    }

// ---------------------------------------------------------
// CWlmServer::UpdateRcpNotificationBoundary
// ---------------------------------------------------------
//
void CWlmServer::UpdateRcpNotificationBoundary(
    TUint /* aSessionId */,
    const RMessage2& aMessage )
    {
    DEBUG( "CWlmServer::UpdateRcpNotificationBoundary()" );

    i32_t rcp_boundary = aMessage.Int0();
    i32_t hysteresis = aMessage.Int1();
    
    if ( rcp_boundary < 0 || rcp_boundary > MAX_RCP_BOUNDARY ||
         hysteresis < 0 || hysteresis > MAX_HYSTERESIS )
        {
        aMessage.Complete( KErrArgument );
        return;        
        }
        
    core_error_e ret = iCoreServer->set_rcp_level_notification_boundary(
        rcp_boundary,
        hysteresis );
    if ( ret != core_error_ok )
        {
        DEBUG1( "CWlmServer::UpdateRcpNotificationBoundary() - set_rcp_level_notification_boundary() failed with %u",
            ret );
        }

    aMessage.Complete(
        TWlanConversionUtil::ConvertErrorCode( ret ) );
    }

// ---------------------------------------------------------
// CWlmServer::ConfigureMulticastGroup
// ---------------------------------------------------------
//
void CWlmServer::ConfigureMulticastGroup(
    TUint /* aSessionId */,
    const RMessage2& aMessage )
    {
    DEBUG( "CWlmServer::ConfigureMulticastGroup()" );

    bool_t join;
    if( aMessage.Int1() == KSoIp6JoinGroup )
        {
        join = true_t;
        }
    else
        {
        join = false_t;
        }
    
    TMacAddress adaptMac( KZeroMacAddr );
    TPckg<TMacAddress> inPckg( adaptMac );
    TInt ret( aMessage.Read( 0, inPckg ) );
    if( ret != KErrNone )
        {
        aMessage.Complete( ret );
        return;
        }

    if( !IsGroupBitSet( adaptMac ) )
        {
        aMessage.Complete( KErrArgument );
        return;
        }
        
    if( iConnectionState == EWlanStateNotConnected )
        {
        aMessage.Complete( KErrNotReady );
        return;
        }

    core_mac_address_s core_mac;
    TWlanConversionUtil::ConvertMacAddress( core_mac, adaptMac );

    core_error_e err = iCoreServer->configure_multicast_group(
        join,
        core_mac );
    if ( err != core_error_ok )
        {
        DEBUG1( "CWlmServer::ConfigureMulticastGroup() - configure_multicast_group() failed with %u",
            err );
        }

    aMessage.Complete(
        TWlanConversionUtil::ConvertErrorCode( err ) );
    }

// ---------------------------------------------------------
// CWlmServer::GetCurrentSystemMode
// ---------------------------------------------------------
//
void CWlmServer::GetCurrentSystemMode(
    TUint /* aSessionId */,
    const RMessage2& aMessage )
    {
    DEBUG( "CWlmServer::GetCurrentSystemMode()" );

    TPckg<TWlanSystemMode> outPckg( iPlatform->GetCurrentSystemMode() );
    TInt ret( aMessage.Write( 0, outPckg ) );
    aMessage.Complete( ret );
    }

// ---------------------------------------------------------
// CWlmServer::GetCurrentState
// ---------------------------------------------------------
//
TWlanConnectionState CWlmServer::GetCurrentState()
    {
    DEBUG( "CWlmServer::GetCurrentState()" );

    return iConnectionState;
    }

// ---------------------------------------------------------
// CWlmServer::GetConnectionState
// ---------------------------------------------------------
//
void CWlmServer::GetConnectionState(
    TUint /* aSessionId */,
    const RMessage2& aMessage )
    {
    DEBUG( "CWlmServer::GetConnectionState()" );

    /**
     * Do not return EWlanStateSearchingAP to the clients.
     */
    TWlanConnectionState state( iConnectionState );
    if ( iConnectionState == EWlanStateSearchingAP )
        {
        state = iPrevConnectionState;
        }

    TPckg<TWlanConnectionState> outPckg( state );
    aMessage.Write( 0, outPckg );
    aMessage.Complete( KErrNone );
    }

// ---------------------------------------------------------
// CWlmServer::GetScanResult
// ---------------------------------------------------------
//
void CWlmServer::GetScanResult(
    TUint aSessionId,
    const RMessage2& aMessage )
    {
    DEBUG( "CWlmServer::GetScanResult()" );

    // Read SSID argument
    TSSID ssid = { 0 };
    TPckg<TSSID> inPckg( ssid );
    if( aMessage.Read( 1, inPckg ) != KErrNone )
        {
        DEBUG( "CWlmServer::GetScanResult() - unable to read SSID parameter" );

        aMessage.Complete( KErrArgument );

        return;
        }

    if( ssid.ssidLength > MAX_SSID_LEN )
        {
        DEBUG1( "CWlmServer::GetScanResult() - invalid SSID length (%u)",
            ssid.ssidLength );

        aMessage.Complete( KErrArgument );

        return;
        }

    // Read cacheLifetime and maxDelay arguments
    TScanScheduling scanScheduling = { KWlmDefaultScanCacheLifetime, 0 };
    TPckg<TScanScheduling> scanSchedulingBuf( scanScheduling );
    if( aMessage.Read( 3, scanSchedulingBuf ) != KErrNone )
        {
        DEBUG( "CWlmServer::GetScanResult() - unable to read lifetime/delay parameters" );

        aMessage.Complete( KErrArgument );

        return;
        }

    // Check that parameters have valid ranges
    CheckScanSchedulingParameters(
        scanScheduling.cacheLifetime,
        scanScheduling.maxDelay );

    // Update the scan scheduling parameters to values that are going be used
    aMessage.Write( 3, scanSchedulingBuf );

    // Only check for valid cache if maxDelay is zero 
    if( scanScheduling.maxDelay == 0 )
        {
        // If not direct scan, try getting cached results and settle for them
        // if they exist
        ScanList* cacheList = iCache->GetScanList( ( scanScheduling.cacheLifetime == 0 ? 1 : scanScheduling.cacheLifetime ) );
        if( !ssid.ssidLength && cacheList )
            {
            // Just return cached results
    
            TDynamicScanList dynamicScanList;
            dynamicScanList.count = cacheList->Count();
            dynamicScanList.size = cacheList->Size();
    
            TPckg<TDynamicScanList> pckgDynamicScanList( dynamicScanList );
            TPtr8 ptrScanList(
                cacheList->Data(),
                cacheList->Size(),
                cacheList->Size() );
    
            DEBUG2( "CWlmServer::GetScanResult() - returning cached results, scan list count is %u, size is %u",
            	cacheList->Count(), cacheList->Size() );
    
            aMessage.Write( 0, ptrScanList );
            aMessage.Write( 2, pckgDynamicScanList );
            aMessage.Complete( KErrNone );
    
            return;
            }
        }

    // If direct scan, or cached results were too old, perform scan 
    core_ssid_s* coreSsid = new core_ssid_s;
    if( !coreSsid )
        {
        DEBUG( "CWlmServer::GetScanResult() - unable to instantiate core_ssid_s" );

        aMessage.Complete( KErrNoMemory );

        return;
        }

    TWlanConversionUtil::ConvertSSID(
        *coreSsid,
        ssid );

    ScanList* scanList = new ScanList( KWlmScanListMaxSize );
    if( !scanList )
        {
        DEBUG( "CWlmServer::GetScanResult() - unable to instantiate ScanList" );

        delete coreSsid;
        aMessage.Complete( KErrNoMemory );

        return;
        }    

    TUint* scanTime( NULL );
    if( scanScheduling.maxDelay != KWlmInfiniteScanDelay )
        {
        scanTime = new TUint(
            CalculateScanStartTime( scanScheduling.maxDelay ) );
        if( !scanTime )
            {
            DEBUG( "CWlmServer::GetScanResult() - unable to instantiate TUint" );

            delete coreSsid;
            delete scanList;
            aMessage.Complete( KErrNoMemory );
    
            return;
            }
        }
#ifdef _DEBUG
    else
        {
        DEBUG( "CWlmServer::GetScanResult() - Infinite maxDelay, request will not be triggering any scans" );
        }

    if( coreSsid->length )
        {
        DEBUG1S( "CWlmServer::GetScanResult() - performing a direct scan with SSID: ",
            coreSsid->length, &coreSsid->ssid[0] );
        }
    else
        {
        DEBUG( "CWlmServer::GetScanResult() - performing a broadcast scan" );
        }
#endif // DEBUG

    // create mapping
    SRequestMapEntry mapEntry;
    mapEntry.iMessage = aMessage;
    mapEntry.iFunction = EGetScanResults;
    mapEntry.iRequestId = iRequestIdCounter++;
    mapEntry.iSessionId = aSessionId;
    mapEntry.iParam0 = scanList;
    mapEntry.iParam1 = coreSsid;
    mapEntry.iTime = scanTime;
    iRequestMap.Append( mapEntry );

    // Scan scheduling timer needs to be set again if this request needs the results earlier or
    // if this is the only timed pending request
    if( scanTime != NULL && ( IsOnlyTimedScanRequestInRequestMap( mapEntry ) || *scanTime < iScanSchedulingTimerExpiration ) )
    	{
        UpdateScanSchedulingTimer( *scanTime, mapEntry.iRequestId );
        }
    }

// ---------------------------------------------------------
// CWlmServer::IsOnlyTimedScanRequestInRequestMap
// ---------------------------------------------------------
//
TBool CWlmServer::IsOnlyTimedScanRequestInRequestMap(
    	const SRequestMapEntry& aMapEntry ) const
	{
    DEBUG( "CWlmServer::IsOnlyTimedScanRequestInRequestMap()" );

    // go through all the requests in the map
    for(TInt index = 0; index < iRequestMap.Count(); index++)
        {
        // look for timed scan requests
        if( IsPendingTimedScanRequest( index ) )
            {
            // if the request ids don't match there are other scan requests
            if( iRequestMap[index].iRequestId != aMapEntry.iRequestId )
                {
                DEBUG( "CWlmServer::IsOnlyTimedScanRequestInRequestMap() - other timed scan requests found" );
                return EFalse;
                }
            }			
        }
    
    DEBUG( "CWlmServer::IsOnlyTimedScanRequestInRequestMap() - no other timed scan requests found" );
    
    return ETrue;
	}

// ---------------------------------------------------------
// CWlmServer::CancelGetScanResult
// ---------------------------------------------------------
//
void CWlmServer::CancelGetScanResult(
    TUint aSessionId,
    const RMessage2& aMessage )
    {
    DEBUG( "CWlmServer::CancelGetScanResult()" );

    CancelExternalRequest(
        aSessionId,
        EGetScanResults );

    aMessage.Complete( KErrNone );
    }

// ---------------------------------------------------------
// CWlmServer::GetAvailableIaps
// ---------------------------------------------------------
//
void CWlmServer::GetAvailableIaps(
    TUint aSessionId,
    const RMessage2& aMessage )
    {
    DEBUG( "CWlmServer::GetAvailableIaps()" );

    // Read cacheLifetime and maxDelay arguments
    TPckgBuf<TInt> cacheLifetimePckg( KWlmDefaultScanCacheLifetime );
    TPckgBuf<TUint> maxDelayPckg( 0 );
    TBool isFiltered(
        aMessage.Int3() );
    DEBUG1( "CWlmServer::GetAvailableIaps() - isFiltered: %u",
        isFiltered );
    if( aMessage.Read( 1, cacheLifetimePckg ) != KErrNone )
        {
        DEBUG( "CWlmServer::GetAvailableIaps() - unable to read lifetime parameter" );

        aMessage.Complete( KErrArgument );

        return;
        }

    if( aMessage.Read( 2, maxDelayPckg ) != KErrNone )
        {
        DEBUG( "CWlmServer::GetAvailableIaps() - unable to read delay parameter" );

        aMessage.Complete( KErrArgument );

        return;
        }

    // Check that parameters have valid ranges
    CheckScanSchedulingParameters(
        cacheLifetimePckg(),
        maxDelayPckg() );

    //Update the scan scheduling parameters to values that are going be used
    aMessage.Write( 1, cacheLifetimePckg );
    aMessage.Write( 2, maxDelayPckg );

    /**
     * See if cached IAP availability information is available.
     */
    RArray<TWlanLimitedIapData> iapList;
    RArray<TWlmAvailabilityData>* list = iCache->AvailableIaps(
        iapList,
        ( cacheLifetimePckg() == 0 ? 1 : cacheLifetimePckg() ) );
    // Only complete with valid cache if maxDelay is zero
    if( list && maxDelayPckg() == 0 )
        {
        TWlmAvailableIaps tmp = { 0 };
        if( isFiltered )
            {            
            DEBUG1( "CWlmServer::GetAvailableIaps() - using filtered cached IAP list, list contains %d IAP(s)",
                list->Count() );

            for( TInt idx( 0 ); idx < list->Count() && tmp.count < KWlmMaxAvailableIaps; ++idx )
                {
                if( (*list)[idx].rcpi >= iDeviceSettings.minRcpiForIapAvailability &&
                    iIapWeakList.Find( (*list)[idx].iapId ) == KErrNotFound )
                    {
                    DEBUG2( "CWlmServer::GetAvailableIaps() - IAP %u is available, RCPI is %u",
                        (*list)[idx].iapId, (*list)[idx].rcpi );
                    tmp.iaps[tmp.count++] = (*list)[idx];
                    }
                else
                    {
                    DEBUG2( "CWlmServer::GetAvailableIaps() - IAP %u filtered, RCPI is %u",
                        (*list)[idx].iapId, (*list)[idx].rcpi );
                    }
                }
            }
        else
            {
            DEBUG1( "CWlmServer::GetAvailableIaps() - using cached IAP list, list contains %d IAP(s)",
                list->Count() );

            for( TInt idx( 0 ); idx < list->Count() && tmp.count < KWlmMaxAvailableIaps; ++idx )
                {
                DEBUG2( "CWlmServer::GetAvailableIaps() - IAP %u is available, RCPI is %u",
                    (*list)[idx].iapId, (*list)[idx].rcpi );
                tmp.iaps[tmp.count++] = (*list)[idx];                
                }
            }

        TPckg<TWlmAvailableIaps> outPckg( tmp );
        aMessage.Write( 0, outPckg );
        aMessage.Complete( KErrNone );
        iapList.Close();

        return;
        }

    // Create list for WLAN IAP data
    core_type_list_c<core_iap_data_s>* iapDataList = new core_type_list_c<core_iap_data_s>;
    if( iapDataList == NULL )
        {
        DEBUG( "CWlmServer::GetAvailableIaps() - unable to instance core_iap_data_s list" );

        aMessage.Complete( KErrNoMemory );
        iapList.Close();

        return;
        }

    // Create list for secondary SSID data
    core_type_list_c<core_ssid_entry_s>* iapSsidList = new core_type_list_c<core_ssid_entry_s>;
    if ( iapSsidList == NULL )
        {
        DEBUG( "CWlmServer::GetAvailableIaps() - unable to instance core_ssid_entry_s list" );

        aMessage.Complete( KErrNoMemory );
        delete iapDataList;
        iapList.Close();

        return;
        }

    // Convert IAP data
    TInt ret = GetIapDataList(
        *iapDataList,
        *iapSsidList,
        iapList );
    if ( ret != KErrNone )
        {
        DEBUG1( "CWlmServer::GetAvailableIaps() - unable to convert IAP data (%d)",
            ret );

        aMessage.Complete( ret );
        delete iapDataList;
        delete iapSsidList;
        iapList.Close();

        return;
        }

    iapList.Close();

    // Create output list
    core_type_list_c<core_iap_availability_data_s>* iapAvailabilityList =
        new core_type_list_c<core_iap_availability_data_s>;
    if( iapAvailabilityList == NULL )
        {
        aMessage.Complete( KErrNoMemory );
        delete iapDataList;
        delete iapSsidList;
        return;
        }
        
    // Create ScanList
    ScanList* scanList = new ScanList( KWlmScanListMaxSize );
    if( scanList == NULL )
        {
        aMessage.Complete( KErrNoMemory );        
        delete iapDataList;
        delete iapSsidList;
        delete iapAvailabilityList;
        return;
        }

    TUint* scanTime( NULL );
    if( maxDelayPckg() != KWlmInfiniteScanDelay )
        {
        scanTime = new TUint(
            CalculateScanStartTime( maxDelayPckg() ) );
        if( !scanTime )
            {
            DEBUG( "CWlmServer::GetAvailableIaps() - unable to instantiate TUint" );
    
            aMessage.Complete( KErrNoMemory );
            delete iapDataList;
            delete iapSsidList;
            delete iapAvailabilityList;
            delete scanList;
    
            return;
            }
        }
#ifdef _DEBUG
    else
        {
        DEBUG( "CWlmServer::GetAvailableIaps() - Infinite maxDelay, request will not be triggering any scans" );
        }
#endif

    // create mapping
    SRequestMapEntry mapEntry;
    mapEntry.iMessage = aMessage;
    mapEntry.iFunction = EGetAvailableIaps;
    mapEntry.iRequestId = iRequestIdCounter++;
    mapEntry.iSessionId = aSessionId;    
    mapEntry.iParam0 = iapDataList;
    mapEntry.iParam1 = iapAvailabilityList;
    mapEntry.iParam2 = scanList;
    mapEntry.iParam3 = iapSsidList;
    mapEntry.iParam4 = reinterpret_cast<TAny*>( isFiltered );
    mapEntry.iTime = scanTime;
    iRequestMap.Append( mapEntry );

    // Scan scheduling timer needs to be set again if this request needs the results earlier or 
    // if this is the only timed pending request
    if( scanTime != NULL && ( IsOnlyTimedScanRequestInRequestMap( mapEntry ) || *scanTime < iScanSchedulingTimerExpiration ) )
    	{
        UpdateScanSchedulingTimer( *scanTime, mapEntry.iRequestId );
    	}
    }

// ---------------------------------------------------------
// CWlmServer::CancelGetAvailableIaps
// ---------------------------------------------------------
//
void CWlmServer::CancelGetAvailableIaps(
    TUint aSessionId,
    const RMessage2& aMessage )
    {
    DEBUG( "CWlmServer::CancelGetAvailableIaps()" );

    CancelExternalRequest(
        aSessionId,
        EGetAvailableIaps );

    aMessage.Complete( KErrNone );
    }

// ---------------------------------------------------------
// CWlmServer::ReleaseComplete
// ---------------------------------------------------------
//
void CWlmServer::ReleaseComplete(
    TUint aSessionId,
    const RMessage2& aMessage )
    {
    DEBUG( "CWlmServer::ReleaseComplete()" );

    iAggressiveScanningAfterLinkLoss = EFalse;

    // Data pipe should now be disconnected
    // -> safe to disconnect the management pipe.

    // create mapping
    SRequestMapEntry mapEntry;
    mapEntry.iMessage = aMessage;
    mapEntry.iFunction = EReset;
    mapEntry.iRequestId = iRequestIdCounter++;
    mapEntry.iSessionId = aSessionId;    
    iRequestMap.Append( mapEntry );

    // pass request to core
    if( iPlatform->IsWlanDisabled() )
        {
        // Disable will release a possible connection
        iCoreServer->disable_wlan( mapEntry.iRequestId );       
        }
    else
        {
        iCoreServer->release( mapEntry.iRequestId );
        }
    }

// ---------------------------------------------------------
// CWlmServer::NotifySessionCreated
// ---------------------------------------------------------
//
TUint CWlmServer::NotifySessionCreated()
    {
    DEBUG( "CWlmServer::NotifySessionCreated()" );
    ++iClientSessionCount;

    return ++iSessionIdCounter;
    }

// ---------------------------------------------------------
// CWlmServer::NotifySessionClosed
// ---------------------------------------------------------
//
void CWlmServer::NotifySessionClosed(
    TUint aSessionId )
    {
    DEBUG( "CWlmServer::NotifySessionClosed()" );
    for( TInt i( 0 ); i < iRequestMap.Count(); i++ )
        {
        if( iRequestMap[i].iSessionId == aSessionId )
            {
            DEBUG( "CWlmServer::NotifySessionClosed() - corresponding request found" );
            iRequestMap[i].iSessionId = 0;
            }
        }

    --iClientSessionCount;
    }

// ---------------------------------------------------------
// CWlmServer::NotifyChangedSettings
// ---------------------------------------------------------
//
void CWlmServer::NotifyChangedSettings(
    TUint /* aSessionId */,
    const RMessage2& aMessage )
    {
    DEBUG( "CWlmServer::NotifyChangedSettings()" );

    aMessage.Complete(
        UpdateWlanSettings() );
    }

// ---------------------------------------------------------
// CWlmServer::UpdateWlanSettings
// ---------------------------------------------------------
//
TInt CWlmServer::UpdateWlanSettings()
    {
    DEBUG( "CWlmServer::UpdateWlanSettings()" );

    CWlanDeviceSettings::SWlanDeviceSettings settings = { 0 };
    TRAPD( err, GetWlanSettingsL( settings ) );
    if( err != KErrNone )
        {
        DEBUG1( "CWlmServer::UpdateWlanSettings() - GetWlanSettingsL leaved with code %d", err );

        return err;
        }
    iDeviceSettings = settings;

    // Only if startup is complete, inform current settings to BgScan
    if( iIsStartupComplete )
        {
        DEBUG( "CWlmServer::UpdateWlanSettings() - notifying settings to BgScan provider" );
        iBgScanProviderSettings = MWlanBgScanProvider::TWlanBgScanSettings( settings.backgroundScanInterval,
                                                                    settings.psmServerMode,
                                                                    settings.bgScanPeakPeriodStart,
                                                                    settings.bgScanPeakPeriodEnd,
                                                                    settings.bgScanIntervalPeak,
                                                                    settings.bgScanIntervalOffPeak );
        iBgScanProvider->NotifyChangedSettings( iBgScanProviderSettings );
        }
        
    // Keep cache lifetime for scan results up to date
    iConfiguredCacheLifetime = settings.scanExpirationTimer;

    // Store power save enabled value
    iPowerSaveEnabled = settings.powerMode;
 
    // Convert settings to core side struct
    core_device_settings_s coreSettings;
    TWlanConversionUtil::ConvertDeviceSettings(
        coreSettings,
        settings );

    core_error_e ret = iCoreServer->update_device_settings( coreSettings );
    if( ret != core_error_ok )
        {
        DEBUG1( "CWlmServer::UpdateWlanSettings() - update_device_settings() failed with %u",
            ret );

        return TWlanConversionUtil::ConvertErrorCode( ret );
        }
    
    u32_t coreFeatures = TWlanConversionUtil::ConvertFeatureFlags(
        iSupportedFeatures,
        settings.enabledFeatures );
    ret = iCoreServer->set_enabled_features( coreFeatures );
    if( ret != core_error_ok )
        {
        DEBUG1( "CWlmServer::UpdateWlanSettings() - set_enabled_features() failed with %u",
            ret );

        return TWlanConversionUtil::ConvertErrorCode( ret );
        }

    DEBUG( "CWlmServer::UpdateWlanSettings() - returning" );

    return KErrNone;
    }

// ---------------------------------------------------------
// CWlmServer::UpdateScanSchedulingTimer
// ---------------------------------------------------------
//
void CWlmServer::UpdateScanSchedulingTimer( 
    TUint aScanTime, 
	TUint aTriggeringRequestId )
    {
    DEBUG1( "CWlmServer::UpdateScanSchedulingTimer() - aTriggeringRequestId = %u ", aTriggeringRequestId );

    if( iCoreHandlingScanRequest )
        {
        // Core is currently handling scanning, do not set the timer
		DEBUG( "CWlmServer::UpdateScanSchedulingTimer() - core is currently handling scanning, do not set the timer" );

        return;
        }

    TUint currentTickCount(
        User::TickCount() );
    TUint difference( 0 );
    if( aScanTime > currentTickCount )
        {
        difference = aScanTime - currentTickCount;  
        }

    iScanSchedulingTimer->Cancel();
    iScanSchedulingTimerExpiration = aScanTime;
    iRequestTriggeringScanning = aTriggeringRequestId;
    
    DEBUG2( "CWlmServer::UpdateScanSchedulingTimer() - scheduling the timer to %u system ticks(s) [%u s]",
        difference, difference * iSystemTickPeriod / KWlanSecsToMicrosecsMultiplier );

    iScanSchedulingTimer->After( difference );
    }

// ---------------------------------------------------------
// CWlmServer::FindNextTimedScanSchedulingRequest
// ---------------------------------------------------------
//
TBool CWlmServer::FindNextTimedScanSchedulingRequest( 
	TUint& aTriggeringRequestIndex )
    {
    DEBUG( "CWlmServer::FindNextTimedScanSchedulingRequest()" );
    
    TBool pendingScanRequestsFound( EFalse );
    TUint closestTime( 0 );
    aTriggeringRequestIndex = 0;
    
    for(TInt i=0; i < iRequestMap.Count(); i++)
        {
        if( IsPendingTimedScanRequest( i ) )
            {
            TUint* checkedTime = reinterpret_cast<TUint*>( iRequestMap[i].iTime );
            if( pendingScanRequestsFound == EFalse || closestTime > *checkedTime )
                {
                closestTime = *checkedTime;
                aTriggeringRequestIndex = i;
                pendingScanRequestsFound = ETrue;
                }
            }
        }
    
    if( pendingScanRequestsFound == EFalse )
        {
        // clear the scan scheduling related variables
        iRequestTriggeringScanning = KWlanIntCmdNull;
        iScanSchedulingTimerExpiration = 0;
        }
    else
        {
        DEBUG1( "CWlmServer::FindNextTimedScanSchedulingRequest() - RequestIndex %u ", aTriggeringRequestIndex );
        }
    
    return pendingScanRequestsFound;
    }

// ---------------------------------------------------------
// CWlmServer::NotifyBackgroundScanDone
// ---------------------------------------------------------
//
void CWlmServer::NotifyBackgroundScanDone(
    ScanList* aScanList, 
    core_type_list_c<core_iap_availability_data_s>& aIapAvailabilityData )
    {
    DEBUG( "CWlmServer::NotifyBackgroundScanDone()" );
    ASSERT( aScanList != NULL );

    // Unload the drivers immediately to conserve power.
    if( !iPlatform->IsWlanDisabled() &&
        iConnectionState == EWlanStateNotConnected )
        {
        iCoreServer->unload_drivers();
        }

    RArray<TWlanAvailableNetwork> networkList;
    GetNetworkList( *aScanList, networkList );
        
    // Update Cached results
    iCache->UpdateScanList( aScanList );
    TBool newIaps( EFalse );
    TBool lostIaps( EFalse );
    iCache->UpdateAvailableNetworksList(
        aIapAvailabilityData,
        networkList,
        newIaps,
        lostIaps );
    
    networkList.Close();
    
    UpdateAvailabilityInfo(
        newIaps,
        lostIaps,
        aScanList->Count() != 0 );
    }

// ---------------------------------------------------------
//
// Methods implementing abs_core_server_callback_c interface
//
// ---------------------------------------------------------
    
// ---------------------------------------------------------
// CWlmServer::notify
// ---------------------------------------------------------
//
void CWlmServer::notify(
    core_notification_e notification,
    int_t data_len,
    const u8_t* data )
    {
    DEBUG2( "CWlmServer::notify (notif == %d, data_len == %u)", notification, data_len );

    // Convert notification type
    TWlmNotify amNotification
        = TWlanConversionUtil::ConvertNotificationType( notification );

    // Convert notification parameter
    TBuf8<KMaxNotificationLength> buf;
    if ( data_len )
        {
        buf.Copy( data, data_len );

        if ( amNotification == EWlmNotifyConnectionStateChanged )
            {
            // 1st byte of notification data corresponds to connection state,
            // which must also be converted.
            // -> Data changes -> it must be copied                
            core_connection_state_e state
                = static_cast<core_connection_state_e>( data[0] );
            buf[0] = static_cast<u8_t>(
                TWlanConversionUtil::ConvertConnectionState( state ) );
            }
        else if ( amNotification == EWlmNotifyTransmitPowerChanged )
            {
            // Core uses dBm values internally, the value must be converted
            // to mW.
            const u32_t* value
                = reinterpret_cast<const u32_t*>( &data[0] );
            TUint32 retValue
                = TWlanConversionUtil::ConvertDecibelToMilliWatts( *value );
            buf.Copy( reinterpret_cast<TUint8*>( &retValue ), sizeof ( retValue ) );
            }
        else if( amNotification == EWlmNotifyRcpChanged )
            {
            // Data contains:
            // 1st byte == RCP Class
            // 2nd byte == RCP value
            //   (value is conveyed over client/server IF as 1 byte value)
            core_rcp_class_e rcpClass = static_cast<core_rcp_class_e>( data[0] );
            buf[0] = static_cast<u8_t>(
                TWlanConversionUtil::ConvertRcpClass( rcpClass ) );            
            }
        else if ( amNotification == EWlmNotifyTrafficStreamStatusChanged )
            {
            /**
             * The first four bytes of notification data is the stream ID,
             * which can be passed unmodified. The fifth byte is the status
             * of the stream, which needs to be converted.
             */
            core_traffic_stream_status_e status
                = static_cast<core_traffic_stream_status_e>( data[4] );
            buf[4] = static_cast<u8_t>(
                TWlanConversionUtil::ConvertTrafficStreamStatus( status ) );
            }
        else if( amNotification == EWlmNotifyAccessPointInfoChanged )
            {
            /**
             * The core data structure has to be converted to a corresponding
             * adaptation type.
             */            
            const core_ap_information_s* info =
                reinterpret_cast<const core_ap_information_s*>( &data[0] );

            TWlanAccessPointInfo apInfo;
            TWlanConversionUtil::ConvertApInformation(
                apInfo,
                *info );
            buf.Copy(
                reinterpret_cast<TUint8*>( &apInfo ),
                sizeof ( apInfo ) );
            }
        else if( amNotification == EWlmNotifyAcTrafficModeChanged )
            {
            core_access_class_e accessClass
                = static_cast<core_access_class_e>( data[0] );
            core_access_class_traffic_mode_e trafficMode
                = static_cast<core_access_class_traffic_mode_e>( data[1] );
            buf[0] = static_cast<u8_t>(
                TWlanConversionUtil::ConvertAccessClass( accessClass ) );
            buf[1] = static_cast<u8_t>(
                TWlanConversionUtil::ConvertTrafficMode( trafficMode ) );
            }
        else if( amNotification == EWlmNotifyAcTrafficStatusChanged )
            {
            core_access_class_e accessClass
                = static_cast<core_access_class_e>( data[0] );
            core_access_class_traffic_status_e trafficStatus
                = static_cast<core_access_class_traffic_status_e>( data[1] );
            buf[0] = static_cast<u8_t>(
                TWlanConversionUtil::ConvertAccessClass( accessClass ) );
            buf[1] = static_cast<u8_t>(
                TWlanConversionUtil::ConvertTrafficStatus( trafficStatus ) );            
            }
        }

    // Notify subscribees
    for ( TInt i = 0; i < iNotificationArray.Count(); i++ )
        {
        iNotificationArray[i]->AddNotification( amNotification, buf );
        }

    // Handle internal state changes / reactions
    switch ( amNotification )
        {
        case EWlmNotifyConnectionStateChanged:
            {
            DEBUG( "CWlmServer::notify() - STATE: EWlmNotifyConnectionStateChanged<ind> ");
            // Cache the state to adaptation side member variable.
            if ( iConnectionState != static_cast<TWlanConnectionState>( buf[0] ) )
                {
                iPrevConnectionState = iConnectionState;
                }
            iConnectionState = static_cast<TWlanConnectionState>( buf[0] );

            switch ( iConnectionState )
                {
                case EWlanStateNotConnected:
                    DEBUG( "CWlmServer::notify() - STATE: EWlanStateNotConnected" );
                    iIsRoaming = EFalse;
                    iConnectionIapId = 0;

                    // set icon to "not available"
                    SetIconState( EWlmIconStatusNotAvailable );
                    
                    // if link was unintentionally lost, scan aggressively
                    // for a while in order to try to find it again
                    if( iAggressiveScanningAfterLinkLoss )
                        {
                        TUint32 interval( KWlanLinkBreakRecoveryInterval );
                        TUint32 duration( KWlanLinkBreakRecoveryDuration );
                        iBgScanProvider->StartAggressiveBgScan(
                            interval,
                            duration );
                        iAggressiveScanningAfterLinkLoss = EFalse;
                        }
                    
					// If WLAN is ON, enable background scanning
					if( iPlatform->GetWlanOnOffState() == EWlanOn )
					    {
                        iBgScanProvider->SetBgScanState( MWlanBgScanProvider::EWlanBgScanOn );
                        }
                    
                    break;
                case EWlanStateInfrastructure:
                    DEBUG( "CWlmServer::notify() - STATE: EWlanStateInfrastructure" );
                    iIsRoaming = EFalse;
                    SetIconState( EWlmIconStatusConnected );
                    break;
                case EWlanStateSearchingAP:
                    DEBUG( "CWlmServer::notify() - STATE: EWlanStateSearchingAP" );
                    iIsRoaming = ETrue;
                    break;
                case EWlanStateIBSS:
                    DEBUG( "CWlmServer::notify() - STATE: EWlanStateIBSS" );
                    iIsRoaming = EFalse;
                    // Core separates adhoc mode to secure(WEP) and non-secure.
                    // set icon according to this
                    if( static_cast<core_connection_state_e>( data[0] )
                        == core_connection_state_secureibss )
                        {
                        SetIconState( EWlmIconStatusConnectedSecure );
                        }
                    else
                        {
                        SetIconState( EWlmIconStatusConnected );
                        }
                    break;
                case EWlanStateSecureInfra:
                    DEBUG( "CWlmServer::notify() - STATE: EWlanStateSecureInfra" );
                    iIsRoaming = EFalse;
                    SetIconState( EWlmIconStatusConnectedSecure );
                    break;
                case EConnectionStateUnknown:
                    DEBUG( "CWlmServer::notify() - STATE: EConnectionStateUnknown" );
                    break;
                default:
                    break;                
                }
            break;
            }
        case EWlmNotifyBssidChanged:
            DEBUG( "CWlmServer::notify() - STATE: EWlmNotifyBssidChanged<ind>" );
            break;
        case EWlmNotifyBssLost:
            DEBUG( "CWlmServer::notify() - STATE: EWlmNotifyBssLost<ind>" );
            iIsRoaming = ETrue;
            break;
        case EWlmNotifyBssRegained:
            DEBUG( "CWlmServer::notify() - STATE: EWlmNotifyBssRegained<ind>" );
            break;
        case EWlmNotifyRcpiRoamAttemptStarted:
            DEBUG( "CWlmServer::notify() - STATE: EWlmNotifyRcpiRoamAttemptStarted<ind>" );
            iIsRoaming = ETrue;
            break;
        case EWlmNotifyRcpiRoamAttemptCompleted:
            DEBUG( "CWlmServer::notify() - STATE: EWlmNotifyRcpiRoamAttemptCompleted<ind>" );
            iIsRoaming = EFalse;
            break;            
        case EWlmNotifyAcTrafficModeChanged:
            DEBUG( "CWlmServer::notify() - STATE: EWlmNotifyAcTrafficModeChanged<ind>" );
            break;
        case EWlmNotifyAcTrafficStatusChanged:
            DEBUG( "CWlmServer::notify() - STATE: EWlmNotifyAcTrafficStatusChanged<ind>" );
            break;
        case EWlmNotifyRcpChanged:
            DEBUG( "CWlmServer::notify() - STATE: EWlmNotifyRcpChanged<ind>" );
            if( static_cast<core_rcp_class_e>( data[0] ) == core_rcp_normal )
                {
                TInt idx = iIapWeakList.Find( iConnectionIapId );
                if( idx >= 0 )
                    {
                    DEBUG1( "CWlmServer::notify() - removing IAP %u from weak list",
                        iConnectionIapId );
                    iIapWeakList.Remove( idx );
                    }
                }
            else
                {
                TInt idx = iIapWeakList.Find( iConnectionIapId );
                if( idx == KErrNotFound )
                    {
                    DEBUG1( "CWlmServer::notify() - adding IAP %u to weak list",
                        iConnectionIapId );
                    iIapWeakList.Append( iConnectionIapId );                    
                    }
                }
            break;
        default:
            break;
        }
    }

// ---------------------------------------------------------
// CWlmServer::NotifyChangedPsmSrvMode
// ---------------------------------------------------------
//
void CWlmServer::NotifyChangedPsmSrvMode(
    TUint /* aSessionId */,
    const RMessage2& aMessage )
    {
    DEBUG( "CWlmServer::NotifyChangedPsmSrvMode()" );
    
    TInt mode( aMessage.Int0() );
    TInt err( KErrNone );
    
    TRAP( err, StoreWlanCenRepKeyValueL( KWlanPsmSrvMode, mode ) );
    if ( err != KErrNone )
        {
        DEBUG1( "CWlmServer::NotifyChangedPsmSrvMode() - failed to update CenRep, error code %d", err );
        }
  
    DEBUG1( "CWlmServer::NotifyChangedPsmSrvMode() - mode %i",
            mode );

    aMessage.Complete( err );
    
    UpdateWlanSettings();

    }

// ---------------------------------------------------------
// CWlmServer::StoreWlanCenRepKeyValue
// ---------------------------------------------------------
//
void CWlmServer::StoreWlanCenRepKeyValueL( const TUint32 aKey, const TInt aValue ) const
    {
    DEBUG( "CWlmServer::StoreWlanCenRepKeyValueL()" );
    
    CWlanDeviceSettings* db = CWlanDeviceSettings::NewL();
    CleanupStack::PushL( db );    
    
    // Write setting
    db->WriteCenRepKeyL( aKey, aValue );

    CleanupStack::PopAndDestroy( db );
    db = NULL;

    }

// ---------------------------------------------------------
// CWlmServer::SetCachedRegion
// ---------------------------------------------------------
//
void CWlmServer::SetCachedRegion(
	TUint32 region,
	TInt32 timestamp )
    {
    DEBUG( "CWlmServer::SetCachedRegion()" );
    
    if ( region == 0 || timestamp == 0 )
        {
    	// cached results do not exist -> iRegion remains in its original value
        return;
        }

    TTime currentTime(0);
    TTimeIntervalHours difference(0);   
    TInt64 timestampInt64;

    currentTime.UniversalTime();
    timestampInt64 = static_cast<TInt64>(timestamp) * KWlmTimestampInCenrep; // timestamp is in minutes in CenRep
    
    TTime timestampMicroseconds(timestampInt64);   
    
    TInt overflow = currentTime.HoursFrom( timestampMicroseconds, difference );
    if ( Abs( difference.Int() ) >= KWlmDomainInfoExpire || overflow )
        {
        DEBUG( "CWlmServer::SetCachedRegion() - cached results are too old, region information is not changed" );
        }
    else if ( Abs( difference.Int()) < KWlmDomainInfoExpire )
    	{
    	DEBUG( "CWlmServer::SetCachedRegion() - cached results are valid, they are taken into use" );
    	if ( region ==  KWlmRegionETSI) 
    		{
    		iRegion = EETSI;
    		DEBUG( "CWlmServer::SetCachedRegion() - Region is ETSI" );
    		}
    	else
    		{
    		iRegion = EFCC;
    		DEBUG( "CWlmServer::SetCachedRegion() - Region is FCC" );
    		}
        iTimeofDomainQuery = timestampMicroseconds;
    	}
    }

// ---------------------------------------------------------
// CWlmServer::store_ap_country_info
// ---------------------------------------------------------
//
void CWlmServer::store_ap_country_info(
	u32_t request_id,
	const core_wlan_region_e& found_region,
    bool_t inconsistent_info )
    {
    DEBUG( "CWlmServer::store_ap_country_info()" );
    
    iRegion = TWlanConversionUtil::ConvertRegion( found_region );
    
    // Write the WLAN region and timestamp to CenRep if the country information from APs was not inconsistent
  	if ( !inconsistent_info )
   		{   		
        iTimeofDomainQuery.UniversalTime();

        TInt region( KWlmRegionFCC );
        if ( iRegion == EETSI )
            {
            region = KWlmRegionETSI;
            }
        
        StoreRegionAndTimestamp( region, ( iTimeofDomainQuery.Int64() / KWlmTimestampInCenrep ) );

   		}
  	// If the APs country information was inconsistent then region is selected to be ETSI
  	else
   		{
   		iRegion = EETSI;
        iTimeofDomainQuery.UniversalTime();
        
        DEBUG1( "CWlmServer::store_ap_country_info() - regulatory domain is 0x%02X", iRegion );
   		}

    // Set timers to notice if system time is changed
    iPlatform->InitializeSystemTimeHandler();
    
    iCoreAsynchCbId = request_id;
    iCoreAsynchCbStatus = core_error_ok;
    iCoreAsynchCb->CallBack();
    
    }

// ---------------------------------------------------------
// CWlmServer::StoreRegionAndTimestamp
// ---------------------------------------------------------
//
void CWlmServer::StoreRegionAndTimestamp( const TInt aRegion, const TInt aTimestamp )
    {
    TRAPD( ret, StoreWlanCenRepKeyValueL( KWlanRegion, aRegion ) );
    if ( ret == KErrNone )
        {
        TRAP( ret, StoreWlanCenRepKeyValueL( KWlanRegionTimestamp, aTimestamp ) );
        if ( ret == KErrNone )
            {
            DEBUG1( "CWlmServer::StoreRegionAndTimestamp() - stored regulatory domain is 0x%02X ", aRegion );
            }
        else
            {
            DEBUG1( "CWlmServer::StoreRegionAndTimestamp() - attempt to store region timestamp leaved with code %d,", ret );                    
            }
        }
    else
        {
        DEBUG1( "CWlmServer::StoreRegionAndTimestamp() - attempt to store region leaved with code %d,", ret );
        }
    }

// ---------------------------------------------------------
// CWlmServer::get_regulatory_domain
// ---------------------------------------------------------
//
void CWlmServer::get_regulatory_domain(
    u32_t request_id,
    core_wlan_region_e& region,
#ifndef __WINS__
    bool_t& mcc_known )
#else
    bool_t& /* mcc_known */ )
#endif // __WINS__    
    {
    DEBUG( "CWlmServer::regulatory_domain()" );

    ASSERT( !iCoreAsynchCbId );

#ifndef __WINS__
    TTime currentTime(0);
    TTimeIntervalHours difference(0);
    TUint mcc( 0 );

    currentTime.UniversalTime();
    mcc_known = false_t;

    TInt overflow = currentTime.HoursFrom( iTimeofDomainQuery, difference );
    if ( Abs( difference.Int() ) >= KWlmDomainInfoExpire || overflow )
        {
        DEBUG( "CWlmServer::regulatory_domain() - cache is not valid, querying MCC" );
        iRegion = EETSI;
        
        // Query the MCC of the currently used operator.
        TRAPD( ret, iPlatform->GetCurrentOperatorMccL( mcc ) );
        if ( ret == KErrNone &&
             mcc )
            {
            DEBUG1( "CWlmServer::regulatory_domain() - MCC is %u", mcc );
            
            mcc_known = true_t;
            iTimeofDomainQuery.UniversalTime();
            
            for ( TUint i(0); i < KWlmOperatorMccNATableLength; i++ )
                {
                if ( mcc == KWlmOperatorMccNATable[i] )
                    {
                    DEBUG( "CWlmServer::regulatory_domain() - MCC matches a US MCC" );
                    iRegion = EFCC;
                    break;
                    }
                }

            TInt wlanRegion( KWlmRegionFCC );
            if ( iRegion == EETSI )
            	{
            	wlanRegion = KWlmRegionETSI;
            	}
            
            // Store the WLAN region and timestamp to CenRep
            StoreRegionAndTimestamp( wlanRegion, ( iTimeofDomainQuery.Int64() / KWlmTimestampInCenrep ) );
            
            // Set timers to notice if system time is changed
            iPlatform->InitializeSystemTimeHandler();

            }
        else
            {
            DEBUG1( "CWlmServer::regulatory_domain() - GetCurrentOperatorMccL leaved with code %d,", ret );
            iRegion = EFCC;
            }
        }
    else if ( Abs( difference.Int()) < KWlmDomainInfoExpire )
        {
        DEBUG( "CWlmServer::regulatory_domain() - cache valid, querying MCC" );
        
        TWlanRegion currentRegion = EETSI;
        
        // Query the MCC of the currently used operator.
        TRAPD( ret, iPlatform->GetCurrentOperatorMccL( mcc ) );
        if ( ret == KErrNone &&
             mcc )
            {
            DEBUG1( "CWlmServer::regulatory_domain() - MCC is %u", mcc );
            
            mcc_known = true_t;
            iTimeofDomainQuery.UniversalTime();
            
            for ( TUint i(0); i < KWlmOperatorMccNATableLength; i++ )
                {
                if ( mcc == KWlmOperatorMccNATable[i] )
                    {
                    DEBUG( "CWlmServer::regulatory_domain() - MCC matches a US MCC" );
                    currentRegion = EFCC;
                    break;
                    }
                }
            
            /* WLAN region is stored to CenRep only if it has changed, because otherwise 
             * this would cause CenRep to be written always when the drivers are loaded. 
             */
            if ( iRegion != currentRegion )
                {
                iRegion = currentRegion;

                // Write the WLAN region and timestamp to CenRep
                TInt wlanRegion( KWlmRegionFCC );
                if ( iRegion == EETSI )
                    {
                    wlanRegion = KWlmRegionETSI;
                    }
                
                StoreRegionAndTimestamp( wlanRegion, ( iTimeofDomainQuery.Int64() / KWlmTimestampInCenrep ) );
                
                // Set timers to notice if system time is changed
                iPlatform->InitializeSystemTimeHandler();
            	
                }
            else
                {
            	iRegion = currentRegion;
                }
            
            }
        else
            {
            DEBUG1( "CWlmServer::regulatory_domain() - GetCurrentOperatorMccL leaved with code %d,", ret );
            mcc_known = true_t;
            }
        }

    DEBUG1( "CWlmServer::regulatory_domain() - regulatory domain is 0x%02X", iRegion );
    
    region = TWlanConversionUtil::ConvertRegion( iRegion );
#else // __WINS__
    region = core_wlan_region_fcc;
#endif // __WINS__

    iCoreAsynchCbId = request_id;
    iCoreAsynchCbStatus = core_error_ok;
    iCoreAsynchCb->CallBack();
    }
    
// ---------------------------------------------------------
// CWlmServer::request_complete
// ---------------------------------------------------------
//
void CWlmServer::request_complete(
    u32_t request_id,
    core_error_e status )
    {
    DEBUG2( "CWlmServer::request_complete(%u, %d)", request_id, status );

#ifdef _DEBUG
    TInt requestMapCount( iRequestMap.Count() );
    if( requestMapCount )
        {
        DEBUG( "CWlmServer::request_complete() - current requests:" );
        for ( TInt idx( 0 ); idx < requestMapCount; ++idx )
            {
            DEBUG2( "CWlmServer::request_complete() - ID %u, function %d", iRequestMap[idx].iRequestId, iRequestMap[idx].iFunction );
            }
        }
#endif
      
    // Find correct request from the map
    TInt idx = FindRequestIndex( request_id );

    if( idx >= iRequestMap.Count() )
        {
        DEBUG1("CWlmServer::request_complete() - completed request (ID %u) not in request map",
                request_id );
        return;
        }

    // create a copy of completed request's map entry
   	SRequestMapEntry completedMapEntry = iRequestMap[idx];

   	// If the operation was GetAvailableIaps, see if there are any SSID lists
   	// defined and update the availability results.
   	if( completedMapEntry.iFunction == EGetAvailableIaps )
   	    {
   	    TRAP_IGNORE( HandleSsidListAvailabilityL( completedMapEntry ) );
   	    }

    // Complete first all the other possible request that can be completed using the same results
    // and the actual triggering request is completed last
    // Background scan request should be updated after all requests have been completed
    TBool bgScanCompleted( request_id == KWlanIntCmdBackgroundScan );
    if( request_id == iRequestTriggeringScanning &&
    	status != core_error_cancel )
        {
        DEBUG( "CWlmServer::request_complete() - also additional requests can be completed" );

        /* 
         * Request can be completed using the results of another request if
         * this method is related to scan scheduling and results of the triggering request can
         * be used also to complete this request
         *
         * Following rules apply:
         *  - GetAvailableIaps results can be used to complete another GetAvailableIaps request, 
         *    background scan request or GetScanResults request without given SSID (broadcast scan) 
         *  - Background scan results can be used to complete GetAvailableIaps request or
         *    GetScanResults request without given SSID (broadcast scan)
         *  - GetScanResults request without given SSID (broadcast scan) results can be used to complete
         *    another GetScanResults request without given SSID (broadcast scan)
         *  - GetScanResults request with given SSID (direct scan) results can be used to complete
         *    another GetScanResults request with same given SSID (direct scan)
         */
       
        TInt i = 0;
        while( i < iRequestMap.Count() )
            {            
            if( IsPendingScanRequest(i) &&
            	iRequestMap[i].iRequestId != iRequestTriggeringScanning &&
                CanRequestBeCompleted( iRequestMap[i], completedMapEntry ) )
                {
                DEBUG1( "CWlmServer::request_complete() - completing additional request (ID %u)", iRequestMap[i].iRequestId );

                if( iRequestMap[i].iRequestId == KWlanIntCmdBackgroundScan )
                    {
                    CompleteInternalRequest( iRequestMap[i], status, EFalse );
                    bgScanCompleted = ETrue;
                    }
                else
                    {
                    CompleteExternalRequest( iRequestMap[i], status, &completedMapEntry );
                    if( IsSessionActive( iRequestMap[i] ) )
                        {
                        iRequestMap[i].iMessage.Complete( 
                            TWlanConversionUtil::ConvertErrorCode( status ) );
                        }
                    iRequestMap.Remove( i );
                    }                
                }
            else
                {
                ++i;
                }
            }
        }

    DEBUG1( "CWlmServer::request_complete() - completing triggering request (ID %u)", completedMapEntry.iRequestId );
    if( request_id < KWlanExtCmdBase )
        {
        CompleteInternalRequest( completedMapEntry, status );
        }
    else if( completedMapEntry.iFunction == EJoinByProfileId ) 
        {
        core_iap_data_s* coreIapData =
            reinterpret_cast<core_iap_data_s*>( completedMapEntry.iParam0 );
        core_type_list_c<core_ssid_entry_s>* coreSsidList =
            reinterpret_cast<core_type_list_c<core_ssid_entry_s>*>( completedMapEntry.iParam1 );
        core_connect_status_e* connectionStatus =
            reinterpret_cast<core_connect_status_e*>( completedMapEntry.iParam2 );

        if( status == core_error_ok && IsSessionActive( completedMapEntry ) )
            {
            DEBUG2("CONNECT COMPLETED WITH status == %u -> adapt == %d",
                *connectionStatus,
                TWlanConversionUtil::ConvertConnectStatus(
                    *connectionStatus,
                    coreIapData->security_mode ) );
            completedMapEntry.iMessage.Complete(
                TWlanConversionUtil::ConvertConnectStatus(
                    *connectionStatus,
                    coreIapData->security_mode ) );
            if( *connectionStatus == core_connect_ok )
                {
                // If connection succeeded, raise flag indicating that
                // aggressive background scanning has to be carried out
                // in case the connection drops
                iAggressiveScanningAfterLinkLoss = ETrue;
                
                // Inform BgScan provider about successful connection.
                // If background scan is currently on, background scan
                // will be disabled and it's request will be removed
                // from the request map.
                iBgScanProvider->SetBgScanState( MWlanBgScanProvider::EWlanBgScanOff );
                
                }
            }
        else if ( IsSessionActive( completedMapEntry ) )
            {
            DEBUG2("CONNECT COMPLETED WITH error == %u -> adapt == %d",
                status,
                TWlanConversionUtil::ConvertErrorCode( status ) );
            completedMapEntry.iMessage.Complete(
                TWlanConversionUtil::ConvertErrorCode( status ) );
            }
        else
            {
            DEBUG( "CWlmServer::request_complete() - session has already closed" );
            }
        delete coreIapData;
        delete coreSsidList;
        delete connectionStatus;
        // re-use idx variable
        idx = FindRequestIndex( completedMapEntry.iRequestId );
        if( idx < iRequestMap.Count() )
            {
            iRequestMap.Remove( idx );
            }
        }
    else if ( completedMapEntry.iFunction == ERunProtectedSetup )
        {
        core_iap_data_s* iapData =
            reinterpret_cast<core_iap_data_s*>( completedMapEntry.iParam0 );
        core_type_list_c<core_iap_data_s>* iapDataList =
            reinterpret_cast<core_type_list_c<core_iap_data_s>*>( completedMapEntry.iParam1 );
        core_protected_setup_status_e* protectedSetupStatus = 
            reinterpret_cast<core_protected_setup_status_e*>( completedMapEntry.iParam2 );
        
        // Convert the received credentials.
        TWlmProtectedSetupCredentials tmp;
        tmp.count = 0;
        core_iap_data_s* iter = iapDataList->first();           
        while ( iter && tmp.count < KWlmProtectedSetupMaxCount )
            {
            TWlanConversionUtil::ConvertProtectedSetupCredentialAttribute(                    
                tmp.credentials[tmp.count],
                *iter );
            ++tmp.count;
            iter = iapDataList->next();
            }

        DEBUG1( "CWlmServer::request_complete() - converted %u Protected Setup credential attributes",
            tmp.count );

        if( IsSessionActive( completedMapEntry ) )
            {
            TPckg<TWlmProtectedSetupCredentials> outPckg( tmp );
            completedMapEntry.iMessage.Write( 2, outPckg );
            }

        if( status == core_error_ok && IsSessionActive( completedMapEntry ) )
            {                    
            DEBUG2("PROTECTED SETUP COMPLETED WITH status == %u -> adapt == %d",
                *protectedSetupStatus,
                TWlanConversionUtil::ConvertProtectedSetupStatus( *protectedSetupStatus ) );
            completedMapEntry.iMessage.Complete( 
                TWlanConversionUtil::ConvertProtectedSetupStatus( *protectedSetupStatus ) );
            }
        else if ( IsSessionActive( completedMapEntry ) )
            {
            DEBUG2("PROTECTED SETUP COMPLETED WITH error == %u -> adapt == %d",
                status,
                TWlanConversionUtil::ConvertErrorCode( status ) );
            completedMapEntry.iMessage.Complete( 
                TWlanConversionUtil::ConvertErrorCode( status ) );                    
            }
        else
            {
            DEBUG( "CWlmServer::request_complete() - session has already closed" );
            }

        delete iapData;
        delete iapDataList;
        delete protectedSetupStatus;
        // re-use idx variable
        idx = FindRequestIndex( completedMapEntry.iRequestId );
        if( idx < iRequestMap.Count() )
            {
            iRequestMap.Remove( idx );
            }
        }
    else
        {
        CompleteExternalRequest( completedMapEntry, status );
        if( IsSessionActive( completedMapEntry ) )
            {
        	completedMapEntry.iMessage.Complete( 
                TWlanConversionUtil::ConvertErrorCode( status ) );
            }
        // re-use idx variable
        idx = FindRequestIndex( completedMapEntry.iRequestId );
        if( idx < iRequestMap.Count() )
            {
            iRequestMap.Remove( idx );
            }
        }

    // Background scan request needs to be updated only after all the other request have been completed
    // otherwise the new request will be appended to the pending list too early
    if( bgScanCompleted )
        {
        iBgScanProvider->ScanComplete();
        }
    
    TUint indexNextScan;
    if( request_id == iRequestTriggeringScanning )
    	{
        iCoreHandlingScanRequest = EFalse;

    	if( FindNextTimedScanSchedulingRequest( indexNextScan ) )
            {
            TUint* nextScanTime = reinterpret_cast<TUint*>( iRequestMap[indexNextScan].iTime );
            UpdateScanSchedulingTimer( *nextScanTime, iRequestMap[indexNextScan].iRequestId );
            }
        }

#ifdef _DEBUG
    requestMapCount = iRequestMap.Count();
    if( requestMapCount )
        {
        DEBUG( "CWlmServer::request_complete() - remaining requests:" );
        // re-use idx variable
        for ( idx = 0; idx < requestMapCount; ++idx )
            {
            DEBUG1( "CWlmServer::request_complete() - ID %u", iRequestMap[idx].iRequestId );
            DEBUG1( "CWlmServer::request_complete() - function %d", iRequestMap[idx].iFunction );
            }
        }
#endif
    
    return;

    }

// ---------------------------------------------------------
// CWlmServer::CanRequestBeCompleted
// ---------------------------------------------------------
//
TBool CWlmServer::CanRequestBeCompleted(
    const SRequestMapEntry& aCheckedMapEntry,
    const SRequestMapEntry& aCompletedMapEntry ) const
    {
    DEBUG2( "CWlmServer::CanRequestBeCompleted() - checked %u completed %u", 
    		 aCheckedMapEntry.iRequestId, aCompletedMapEntry.iRequestId );

    if( aCheckedMapEntry.iRequestId == KWlanIntCmdBackgroundScan )
        {
        DEBUG( "CWlmServer::CanRequestBeCompleted() - The request to be checked is background scan" );

        if( iRequestTriggeringScanning == KWlanIntCmdBackgroundScan )
            {
            DEBUG( "CWlmServer::CanRequestBeCompleted() - The triggering request is background scan" );

            return ETrue;
            }
        else if( aCompletedMapEntry.iRequestId >= KWlanExtCmdBase && 
                aCompletedMapEntry.iFunction == EGetAvailableIaps )
            {
            DEBUG( "CWlmServer::CanRequestBeCompleted() - The triggering request is GetAvailableIaps" );

            return ETrue;
            }
        else
            {
            return EFalse;
            }
        }
    else if( aCheckedMapEntry.iFunction == EGetAvailableIaps )
        {
        DEBUG( "CWlmServer::CanRequestBeCompleted() - The request to be checked is GetAvailableIaps" );
        
        if( iRequestTriggeringScanning == KWlanIntCmdBackgroundScan )
            {
            DEBUG( "CWlmServer::CanRequestBeCompleted() - The triggering request is background scan" );
            
            return ETrue;
            }
        else if( aCompletedMapEntry.iRequestId >= KWlanExtCmdBase &&
                aCompletedMapEntry.iFunction == EGetAvailableIaps )
            {
            DEBUG( "CWlmServer::CanRequestBeCompleted() - The triggering request is GetAvailableIap" );

            return ETrue;
            }
        else
            {
            DEBUG( "CWlmServer::CanRequestBeCompleted() - The triggering request is not scan related" );

            return EFalse;
            }
        }
    else if( aCheckedMapEntry.iFunction == EGetScanResults )
        {
        DEBUG( "CWlmServer::CanRequestBeCompleted() - The request to be checked is GetScanResults" );
        
        core_ssid_s* triggeringSsid = reinterpret_cast<core_ssid_s*>( aCompletedMapEntry.iParam1 );
        core_ssid_s* checkedSsid = reinterpret_cast<core_ssid_s*>( aCheckedMapEntry.iParam1 );

        if( checkedSsid->length == 0 &&
                aCompletedMapEntry.iRequestId == KWlanIntCmdBackgroundScan )
            {
            DEBUG( "CWlmServer::CanRequestBeCompleted() - The triggering request is background scan" );

            return ETrue;
            }
        else if( aCompletedMapEntry.iRequestId >= KWlanExtCmdBase &&
        		 checkedSsid->length == 0 && 
        		 aCompletedMapEntry.iFunction == EGetAvailableIaps )
            {
            DEBUG( "CWlmServer::CanRequestBeCompleted() - The triggering request is GetAvailableIaps" );
            
            return ETrue;
            }
        else if( aCompletedMapEntry.iRequestId >= KWlanExtCmdBase &&
        		 checkedSsid->length == 0 &&
        		 aCompletedMapEntry.iFunction == EGetScanResults &&
        		 triggeringSsid->length == 0 )
            {
            DEBUG( "CWlmServer::CanRequestBeCompleted() - The triggering request is GetScanResults (broadcast)" );

            return ETrue;
            }
        else if( aCompletedMapEntry.iRequestId >= KWlanExtCmdBase && 
        		 checkedSsid->length != 0 &&
        		 aCompletedMapEntry.iFunction == EGetScanResults &&
       	         triggeringSsid->length != 0 &&
       	         *checkedSsid == *triggeringSsid )
            {
            DEBUG( "CWlmServer::CanRequestBeCompleted() - The triggering request is GetScanResults (direct) and both have same SSID" );
            
            return ETrue;
            }
        else
            {
            DEBUG( "CWlmServer::CanRequestBeCompleted() - The triggering request is something else" );
            
            return EFalse;
            }
        
        }
    return EFalse;
    }

// ---------------------------------------------------------
// CWlmServer::CompleteInternalRequest
// ---------------------------------------------------------
//
void CWlmServer::CompleteInternalRequest(
    const SRequestMapEntry& aRequest, 
    core_error_e aStatus,
    TBool aCompletedWasTriggering )
    {
    TInt idx = FindRequestIndex( aRequest.iRequestId );
    
    if( idx >= iRequestMap.Count() )
        {
        DEBUG1("CWlmServer::CompleteInternalRequest() - request (ID %u) not in request map", idx );
        return;
        }
    
    DEBUG1( "CWlmServer::CompleteInternalRequest() - index (%d)", idx );

    switch( aRequest.iRequestId )
        {
        case KWlanIntCmdBackgroundScan:
            {
            core_type_list_c<core_ssid_entry_s>* iapSsidList =
                reinterpret_cast<core_type_list_c<core_ssid_entry_s>*>( aRequest.iParam3 );
            delete iapSsidList;
            iapSsidList = NULL;
            
            ScanList* scanList = 
                reinterpret_cast<ScanList*>( aRequest.iParam2 );
            core_type_list_c<core_iap_availability_data_s>* iapAvailabilityList = 
                reinterpret_cast<core_type_list_c<core_iap_availability_data_s>*>( aRequest.iParam1 );
            core_type_list_c<core_iap_data_s>* iapDataList =
                reinterpret_cast<core_type_list_c<core_iap_data_s>*>( aRequest.iParam0 );
            delete iapDataList;
            iapDataList = NULL;
            
            TUint* completedScanTime = reinterpret_cast<TUint*>( aRequest.iTime );
            delete completedScanTime;
            completedScanTime = NULL;

            if( aCompletedWasTriggering )
                {
                DEBUG( "CWlmServer::CompleteInternalRequest() - this request was handled by core" );

                if( aStatus == core_error_ok )
                    {
                    NotifyBackgroundScanDone( scanList, *iapAvailabilityList );
                    // cache takes the ownership of the scanList
                    scanList = NULL;
                    }
            	}
            else
                {
                DEBUG( "CWlmServer::CompleteInternalRequest() - this request was not handled by core" );
                }
            
            delete scanList;
            scanList = NULL;
            delete iapAvailabilityList;
            iapAvailabilityList = NULL;
                        
            break;
            }
        case KWlanIntCmdNull: // Fall through on purpose
        default:
            DEBUG1( "CWlmServer::CompleteInternalRequest() - request ID %u not handled", aRequest.iRequestId );
            break;
            // not interested in rest of the internal request completions
        }

    iRequestMap.Remove( idx );
    }

// ---------------------------------------------------------
// CWlmServer::CompleteExternalRequest
// ---------------------------------------------------------
//
void CWlmServer::CompleteExternalRequest(
    const SRequestMapEntry& aRequest,
    core_error_e aStatus,
    SRequestMapEntry* aTriggerRequest )
    {
    
    TInt idx = FindRequestIndex( aRequest.iRequestId );
    
    if( idx >= iRequestMap.Count() )
        {
        DEBUG1("CWlmServer::CompleteExternalRequest() - request (ID %u) not in request map", idx );
        return;
        }
    
    DEBUG1( "CWlmServer::CompleteExternalRequest() - index (%d)", idx );

    // Find out the request type
    // in order to handle possible return parameters
    switch( aRequest.iFunction )
        {
        case EGetScanResults:
            {
            ScanList* tmp( NULL );
            core_ssid_s* ssid = reinterpret_cast<core_ssid_s*>( aRequest.iParam1 );
            TUint* completedScanTime = reinterpret_cast<TUint*>( aRequest.iTime );
            ScanList* completedScanList = reinterpret_cast<ScanList*>( aRequest.iParam0 );

            if( aTriggerRequest == NULL )
                {
                DEBUG( "CWlmServer::CompleteExternalRequest() - GetScanResults request handled by core" );    

                tmp = reinterpret_cast<ScanList*>( aRequest.iParam0);
                }
            else
                {
                DEBUG( "CWlmServer::CompleteExternalRequest() - GetScanResults request not handled by core" );    
                // Use the results of the triggering request to complete also this other request
                // Check was the triggering request background scan or GetAvailableIaps or 
                // was it GetScanResults because the ScanList is stored to different parameter
                if( aTriggerRequest->iRequestId >= KWlanExtCmdBase && aTriggerRequest->iFunction == EGetScanResults )
                    {
                    tmp = reinterpret_cast<ScanList*>( aTriggerRequest->iParam0 );
                    }
                else
                    {
                    tmp = reinterpret_cast<ScanList*>( aTriggerRequest->iParam2 );
                    }
                }

            if( aStatus == core_error_ok )
                {
                TDynamicScanList dynamicScanList;
                dynamicScanList.count = tmp->Count();
                dynamicScanList.size = tmp->Size();

                TPckgBuf<TDynamicScanList> pckgDynamicScanList( dynamicScanList );
                TPtr8 ptrScanList(
                    tmp->Data(),
                    tmp->Size(),
                    tmp->Size() );				
                                                                
                DEBUG2( "CWlmServer::CompleteExternalRequest() - scan results count is %u, size is %u",
                    tmp->Count(), tmp->Size() );

                if( IsSessionActive( aRequest ) )
                    {
                    aRequest.iMessage.Write( 0, ptrScanList );
                    aRequest.iMessage.Write( 2, pckgDynamicScanList );
                    }

                // Check whether to cache the results or not
                if( ssid->length > 0 && aTriggerRequest == NULL )
                    {
                    // direct scan results are not cached
                    DEBUG( "CWlmServer::CompleteExternalRequest() - direct scan; not caching scan results" );    
                    
                    delete tmp;
                    }
                else if( aTriggerRequest == NULL )
                    {
                     // not deleting scanList, because cache takes the ownership
                    DEBUG( "CWlmServer::CompleteExternalRequest() - caching scan results" );
                    iCache->UpdateScanList( tmp );
                    }
                else
                    {
                    DEBUG( "CWlmServer::CompleteExternalRequest() - request not handled by core; not caching scan results" );    
                    
                    delete completedScanList;
                    }
                }
            else
                {
                // scan failed due to some reason: not caching anything
                if( aTriggerRequest == NULL ) 
                    {
                    delete tmp;
                    }
                else
                    {
                    delete completedScanList;
                    }
                }

            delete ssid; // ssid
            delete completedScanTime;
            break;
            }
        case EReset:
            {
            // no parameters to return
            break;
            }
        case EGetAvailableIaps:
            {
            // Create pointers to parameters
            core_type_list_c<core_ssid_entry_s>* iapSsidList;
            ScanList* scanList;
            core_type_list_c<core_iap_availability_data_s>* coreAvailabilityList;
            core_type_list_c<core_iap_data_s>* iapDataList;
            TBool isFiltered( reinterpret_cast<TBool>( aRequest.iParam4 ) );
            DEBUG1( "CWlmServer::CompleteExternalRequest() - isFiltered: %u",
                isFiltered );

            iapSsidList = reinterpret_cast<core_type_list_c<core_ssid_entry_s>*>( aRequest.iParam3 );
            iapDataList = reinterpret_cast<core_type_list_c<core_iap_data_s>*>( aRequest.iParam0 );
            
            TUint* completedScanTime = reinterpret_cast<TUint*>( aRequest.iTime );
            ScanList* completedScanList = reinterpret_cast<ScanList*>( aRequest.iParam2);
            core_type_list_c<core_iap_availability_data_s>* completedAvailabilityList =
                reinterpret_cast<core_type_list_c<core_iap_availability_data_s>*>( aRequest.iParam1 );
            
            if( aTriggerRequest == NULL )
                {
                DEBUG( "CWlmServer::CompleteExternalRequest() - GetAvailableIaps request handled by core" );    

                scanList = reinterpret_cast<ScanList*>( aRequest.iParam2);
                coreAvailabilityList = reinterpret_cast<core_type_list_c<core_iap_availability_data_s>*>( aRequest.iParam1 );
                }
            else
                {
                DEBUG( "CWlmServer::CompleteExternalRequest() - GetAvailableIaps request not handled by core" );    
                //Use the results of the triggering request to complete also this other request
                scanList = reinterpret_cast<ScanList*>( aTriggerRequest->iParam2);
                coreAvailabilityList = reinterpret_cast<core_type_list_c<core_iap_availability_data_s>*>( aTriggerRequest->iParam1 );                
                }

            delete iapSsidList;
            iapSsidList = NULL;
            delete iapDataList;
            iapDataList = NULL;

            if( aStatus == core_error_ok )
                {
                RArray<TWlanAvailableNetwork> networkList;
                GetNetworkList( *scanList, networkList );
        
                // Update Cached results
                TBool newIaps( EFalse );
                TBool lostIaps( EFalse );
                
                if( aTriggerRequest == NULL )
                    {
                    iCache->UpdateAvailableNetworksList(
                        *coreAvailabilityList,
                        networkList,
                        newIaps,
                        lostIaps );
                    }
                networkList.Close();                

                TWlmAvailableIaps tmp = { 0 };
                core_iap_availability_data_s* pInt = coreAvailabilityList->first();
                if( isFiltered )
                    {
                    while( pInt && tmp.count < KWlmMaxAvailableIaps )
                        {
                        if( pInt->rcpi < iDeviceSettings.minRcpiForIapAvailability )
                            {
                            DEBUG3( "CWlmServer::CompleteExternalRequest() - IAP %u filtered, RCPI is too weak (%u vs %u)",
                                pInt->id, pInt->rcpi, iDeviceSettings.minRcpiForIapAvailability );
                            }
                        else if( iIapWeakList.Find( pInt->id ) != KErrNotFound )
                            {
                            if( pInt->rcpi < iDeviceSettings.rcpiTrigger )
                                {
                                DEBUG3( "CWlmServer::CompleteExternalRequest() - IAP %u filtered, in weak list and RCPI is too weak (%u vs %u)",
                                    pInt->id, pInt->rcpi, iDeviceSettings.rcpiTrigger );                            
                                }
                            else
                                {
                                DEBUG2( "CWlmServer::CompleteExternalRequest() - IAP %u is available, RCPI is %u, removed from weak list",
                                    pInt->id, pInt->rcpi );
                                tmp.iaps[tmp.count].iapId = pInt->id;
                                tmp.iaps[tmp.count++].rcpi = pInt->rcpi;
                                iIapWeakList.Remove(
                                    iIapWeakList.Find( pInt->id ) );
                                }
                            }
                        else
                            {
                            DEBUG2( "CWlmServer::CompleteExternalRequest() - IAP %u is available, RCPI is %u",
                                pInt->id, pInt->rcpi );
                            tmp.iaps[tmp.count].iapId = pInt->id;
                            tmp.iaps[tmp.count++].rcpi = pInt->rcpi;
                            }
                        pInt = coreAvailabilityList->next();
                        }
                    }
                else
                    {
                    while( pInt && tmp.count < KWlmMaxAvailableIaps )
                        {
                        if( pInt->rcpi >= iDeviceSettings.rcpiTrigger &&
                            iIapWeakList.Find( pInt->id ) != KErrNotFound )
                            {                            
                            DEBUG2( "CWlmServer::CompleteExternalRequest() - IAP %u is available, RCPI is %u, removed from weak list",
                                pInt->id, pInt->rcpi );
                            iIapWeakList.Remove(
                                iIapWeakList.Find( pInt->id ) );
                            }
                        else
                            {
                            DEBUG2( "CWlmServer::CompleteExternalRequest() - IAP %u is available, RCPI is %u",
                                pInt->id, pInt->rcpi );
                            }

                        tmp.iaps[tmp.count].iapId = pInt->id;
                        tmp.iaps[tmp.count++].rcpi = pInt->rcpi;
                        pInt = coreAvailabilityList->next();                        
                        }
                    }

                if( IsSessionActive( aRequest ) )
                    {
                    TPckg<TWlmAvailableIaps> outPckg( tmp );
                    aRequest.iMessage.Write( 0, outPckg );
                    }
                if( aTriggerRequest == NULL )
                    {
                    DEBUG("CWlmServer::CompleteExternalRequest() - delete coreAvailabilityList" );
                    delete coreAvailabilityList;	
                    }
                else
                    {
                    // If this completed request was not the triggering request then there is no need
                    // to cache anything. The triggering request results will be cached.
                    delete completedAvailabilityList;
                    delete completedScanList;
                    }

                // handle scan list
                if( aTriggerRequest == NULL )
                    {                    
                    iCache->UpdateScanList( scanList );
                    // not deleting scanList, because cache takes the ownership                

                    UpdateAvailabilityInfo(
                        newIaps,
                        lostIaps,
                        scanList->Count() != 0 );
                    }
                }
            else
                {
                // scan failed due to some reason: not caching anything
                if( aTriggerRequest == NULL )
                    {
                    delete coreAvailabilityList;
                    delete scanList;
                    }
                else
                    {
                    // Delete only the lists of the completed request. Triggering request lists are
                    // deleted later on when that request is actually handled.
                    delete completedAvailabilityList;
                    delete completedScanList; 
                    }
                }
            delete completedScanTime;
            break;
            }
        case EGetCurrentRSSI:
            {
            TUint32 tmp = *( reinterpret_cast<TUint32*>( aRequest.iParam0 ) );
            if( IsSessionActive( aRequest ) )
                {
                TPckg<TUint32> outPckg( tmp );
                aRequest.iMessage.Write( 0, outPckg );
                }
            iPrevRcpiValue = tmp;
            delete reinterpret_cast<TUint32*>( aRequest.iParam0 );
            break;
            }
        case EGetSystemMode:
            {
            // not asynch request; never comes here
            break;
            }
        case EConfigureMulticastGroup:
            {
            // no parameters to return
            delete reinterpret_cast<TUint32*>( aRequest.iParam0 );
            break;
            }
        case EGetPacketStatistics:
            {
            core_packet_statistics_s* coreStatistics =
                reinterpret_cast<core_packet_statistics_s*>( aRequest.iParam0 );
            if( IsSessionActive( aRequest ) )                
                {
                TPckgBuf<TWlanPacketStatistics> statisticPckg;
                TWlanConversionUtil::ConvertPacketStatistics(
                    statisticPckg(),
                    *coreStatistics );
                aRequest.iMessage.Write( 0, statisticPckg );
                }
            delete coreStatistics;
            break;
            }
        case ECreateTrafficStream:
            {
            u32_t* coreStreamId = reinterpret_cast<u32_t*>( aRequest.iParam0 );
            core_traffic_stream_status_e* coreStreamStatus =
                reinterpret_cast<core_traffic_stream_status_e*>( aRequest.iParam1 );
            if( IsSessionActive( aRequest ) &&
                aStatus == core_error_ok )
                {                
                TPckgBuf<TUint> streamIdPckg(
                    *coreStreamId ); 
                TPckgBuf<TWlanTrafficStreamStatus> streamStatusPckg(
                    TWlanConversionUtil::ConvertTrafficStreamStatus( *coreStreamStatus ) );                
                aRequest.iMessage.Write( 2, streamIdPckg );
                aRequest.iMessage.Write( 3, streamStatusPckg );
                }
            delete coreStreamId;
            delete coreStreamStatus;
            break;
            }
        case EDeleteTrafficStream:
            {
            // no parameters to return
            break;
            }
        case EDirectedRoam:
            {
            // no parameters to return
            break;
            }            
        default:
            {
            DEBUG1( "CWlmServer::CompleteExternalRequest() - ERROR: unknown request type (%d)!",
                aRequest.iFunction );
            break;
            }
        }    
    }

// ---------------------------------------------------------
// CWlmServer::IsAvailableNetworkEqual
// ---------------------------------------------------------
//
TBool CWlmServer::IsAvailableNetworkEqual(
    const TWlanAvailableNetwork& aFirst,
    const TWlanAvailableNetwork& aSecond )
    {
    if( aFirst.ssid != aSecond.ssid )
        {
        return EFalse;
        }

    if( aFirst.networkType != aSecond.networkType )
        {
        return EFalse;        
        }

    if( aFirst.securityMode != aSecond.securityMode )
        {
        return EFalse;
        }

    return ETrue;
    }

// ---------------------------------------------------------
// CWlmServer::GetNetworkList
// ---------------------------------------------------------
//
TInt CWlmServer::GetNetworkList(
    const ScanList& aScanList,
    RArray<TWlanAvailableNetwork>& aNetworkList )
    {
    DEBUG( "CWlmServer::GetNetworkList()" );
    
    ScanInfo info( aScanList );
    TIdentityRelation<TWlanAvailableNetwork> isEqual( IsAvailableNetworkEqual );    
    for( info.First(); !info.IsDone(); info.Next() )
        {        
        TUint8 ieLength( 0 );
        const TUint8* ieData = NULL;

        if ( info.InformationElement( E802Dot11SsidIE, ieLength, &ieData ) == WlanScanError_Ok &&
             ieLength <= KMaxSSIDLength )
            {
            TWlanAvailableNetwork network;
            network.ssid.Copy( ieData, ieLength );
            network.securityMode = info.SecurityMode();
            network.rcpi = info.RXLevel();
            if( info.OperatingMode() == WlanOperatingModeInfra )
                {
                network.networkType = Infrastructure;                
                }
            else
                {
                network.networkType = Adhoc;
                }

            TInt idx = aNetworkList.Find( network, isEqual ); 
            if ( idx == KErrNotFound )
                {
                DEBUG1S( "CWlmServer::GetNetworkList() - appending SSID ",
                    ieLength, ieData );
                aNetworkList.Append( network );
                }
            else if( idx >= 0 &&
                     aNetworkList[idx].rcpi < network.rcpi )
                {
                aNetworkList[idx].rcpi = network.rcpi;            
                }
            }
        }

    return KErrNone;
    }

// ---------------------------------------------------------
// CWlmServer::UpdateAvailabilityInfo
// ---------------------------------------------------------
//
void CWlmServer::UpdateAvailabilityInfo(
    TBool aNewNetworksDetected,
    TBool aOldNetworksLost,
    TBool aAnyNetworksDetected )
    {
    DEBUG1( "CWlmServer::UpdateAvailabilityInfo() - aNewNetworksDetected: %u",
        aNewNetworksDetected );
    DEBUG1( "CWlmServer::UpdateAvailabilityInfo() - aOldNetworksLost:     %u",
        aOldNetworksLost );    
    DEBUG1( "CWlmServer::UpdateAvailabilityInfo() - aAnyNetworksDetected: %u",
        aAnyNetworksDetected );    

    /**
     * Make sure background scan is on before giving any indications.
     */
    if( !iBgScanProvider->IsBgScanEnabled() )
        {
        DEBUG( "CWlmServer::UpdateAvailabilityInfo() - background scan is not enabled, not sending any indications" );
        return;
        }

    /**
     * Notify clients about new networks/IAPs if necessary.
     */
    if( aNewNetworksDetected )
        {
        TBuf8<1> tmp;
        for( TInt i = 0; i < iNotificationArray.Count(); i++ )
            {
            iNotificationArray[i]->AddNotification( EWlmNotifyNewNetworksDetected, tmp );
            }
        }

    /**
     * Notify clients about lost networks/IAPs if necessary.
     */
    if( aOldNetworksLost )
        {
        TBuf8<1> tmp;
        for( TInt i = 0; i < iNotificationArray.Count(); i++ )
            {
            iNotificationArray[i]->AddNotification( EWlmNotifyOldNetworksLost, tmp );
            }
        }

    /**
     * Set icon state if not connected.
     */
    if( iConnectionState == EWlanStateNotConnected )
        {
        if( aAnyNetworksDetected )
            {
            SetIconState( EWlmIconStatusAvailable );
            }
        else
            {
            SetIconState( EWlmIconStatusNotAvailable );
            }
        }    
    }

// ---------------------------------------------------------
// CWlmServer::CancelExternalRequest
// ---------------------------------------------------------
//
void CWlmServer::CancelExternalRequest(
    TUint aSessionId,
    TWLMCommands aCommand )
    {
    DEBUG( "CWlmServer::CancelExternalRequest()" );
    DEBUG1( "CWlmServer::CancelExternalRequest() - aSessionId: %u",
        aSessionId );
    DEBUG1( "CWlmServer::CancelExternalRequest() - aCommand: %u",
        aCommand );

#ifdef _DEBUG
    DEBUG( "CWlmServer::CancelExternalRequest()" );
    DEBUG( "CWlmServer::CancelExternalRequest() - iRequestMap:" );

    for ( TInt idx( 0 ); idx < iRequestMap.Count(); ++idx )
        {
        DEBUG1( "CWlmServer::CancelExternalRequest() - iRequestId %u",
            iRequestMap[idx].iRequestId );
        DEBUG1( "CWlmServer::CancelExternalRequest() - iSessionId %u",
            iRequestMap[idx].iSessionId );            
        DEBUG1( "CWlmServer::CancelExternalRequest() - iFunction: %d",
            iRequestMap[idx].iFunction );
        }
#endif // _DEBUG        

    for( TInt i( 0 ); i < iRequestMap.Count(); i++ )
        {
        if( iRequestMap[i].iSessionId == aSessionId &&
            iRequestMap[i].iFunction == aCommand )
            {
            DEBUG( "CWlmServer::CancelExternalRequest() - corresponding request found, cancelling" );
            
            if( iRequestMap[i].iRequestId == iRequestTriggeringScanning && 
            	iCoreHandlingScanRequest )
                {
                // Core is already handling this scan request
                // request Core to cancel the handling
                DEBUG( "CWlmServer::CancelExternalRequest() - Core is already handling this request, send cancel request" );

                iCoreServer->cancel_request(
                    iRequestMap[i].iRequestId );
                }
            else if( iRequestMap[i].iRequestId == iRequestTriggeringScanning )
                {
                // Core is not handling currently this scan request
                // Complete the cancelled request and timer is set again
                DEBUG( "CWlmServer::CancelExternalRequest() - Core is not handling currently this request" );
                DEBUG( "CWlmServer::CancelExternalRequest() - Cancel timer, complete cancelled request and set timer" );

                iScanSchedulingTimer->Cancel();
                request_complete( iRequestMap[i].iRequestId, core_error_cancel );                
                }
            else
                {
                if( iRequestMap[i].iFunction == EGetScanResults || 
                	iRequestMap[i].iFunction == EGetAvailableIaps )
                    {
                    // Core is not handling currently this scan request
                    // Remove the cancelled request
                    DEBUG( "CWlmServer::CancelExternalRequest() - Core is not handling currently this scan request" );
                    DEBUG( "CWlmServer::CancelExternalRequest() - this request is not the scan scheduling triggering request" );
                    DEBUG( "CWlmServer::CancelExternalRequest() - remove the cancelled request" );

                    CompleteExternalRequest( iRequestMap[i], core_error_cancel );
                    if( IsSessionActive( iRequestMap[i] ) )
                        {
                        iRequestMap[i].iMessage.Complete( 
                            TWlanConversionUtil::ConvertErrorCode( core_error_cancel ) );
                        }

                    iRequestMap.Remove( i );
                    }
                else
                    {
                    // Cancelled request is not a scan scheduling related request
                    // make normal cancel handling and request Core to cancel the handling
                    DEBUG( "CWlmServer::CancelExternalRequest() - Cancelled request is not a scan scheduling related request" );

                    iCoreServer->cancel_request(
                        iRequestMap[i].iRequestId );                    
                    }
                }
            return;
            }
        }

    DEBUG( "CWlmServer::CancelExternalRequest() - no pending request found, ignoring" );   
    }

// ---------------------------------------------------------
// CWlmServer::CheckScanSchedulingParameters
// ---------------------------------------------------------
//
void CWlmServer::CheckScanSchedulingParameters(
    TInt& aCacheLifetime,
    TUint& aMaxDelay )
    {
    DEBUG1( "CWlmServer::CheckScanSchedulingParameters() - aCacheLifetime: %d", aCacheLifetime );
    DEBUG1( "CWlmServer::CheckScanSchedulingParameters() - aMaxDelay: %u", aMaxDelay);

    if( aMaxDelay == KWlmInfiniteScanDelay )
        {
        DEBUG( "CWlmServer::CheckScanSchedulingParameters() - infinite aMaxDelay" );        
        }
    else if( aMaxDelay > KWlmMaxScanDelay )
        {
        aMaxDelay = KWlmMaxScanDelay;
        DEBUG1( "CWlmServer::CheckScanSchedulingParameters() - new aMaxDelay: %u", aMaxDelay );
        }
    else
        {
        DEBUG( "CWlmServer::CheckScanSchedulingParameters() - no changes to aMaxDelay" );
        }

    // cacheLifetime parameter has meaning only when maxDelay is zero
    if( aMaxDelay != 0 )
        {
        aCacheLifetime = 0;
        DEBUG1( "CWlmServer::CheckScanSchedulingParameters() - aMaxDelay non-zero -> new aCacheLifetime: %d", aCacheLifetime );
        }
    else
        {
        if( aCacheLifetime > KWlmMaxScanCacheLifetime )
            {
            aCacheLifetime = KWlmMaxScanCacheLifetime;
            DEBUG1( "CWlmServer::CheckScanSchedulingParameters() - new aCacheLifetime: %d", aCacheLifetime );
            }
        else if( aCacheLifetime == KWlmDefaultScanCacheLifetime )
            {
            aCacheLifetime = iConfiguredCacheLifetime;        
            DEBUG1( "CWlmServer::CheckScanSchedulingParameters() - new aCacheLifetime: %d", aCacheLifetime );
            }
        else if( aCacheLifetime < KWlmMinScanCacheLifetime )
            {
            aCacheLifetime = KWlmMinScanCacheLifetime;
            DEBUG1( "CWlmServer::CheckScanSchedulingParameters() - new aCacheLifetime: %d", aCacheLifetime );
            }
        else
            {
            DEBUG( "CWlmServer::CheckScanSchedulingParameters() - no changes to aCacheLifetime" );
            }    
        }
    }

// ---------------------------------------------------------
// CWlmServer::cancel_request
// ---------------------------------------------------------
//
void CWlmServer::cancel_request(
    u32_t /* request_id */ )
    {
    DEBUG( "CWlmServer::cancel_request" );
    }

// ---------------------------------------------------------
// CWlmServer::load_eapol
// ---------------------------------------------------------
//
u32_t CWlmServer::load_eapol(
    core_eapol_operating_mode_e mode,
    abs_wlan_eapol_callback_c* const partner )
    {
    DEBUG( "CWlmServer::load_eapol" );
    
    if ( iEapolClient )
        {
        DEBUG( "CWlmServer::load_eapol - CWlanEapolClient already instantiated." );
        return wlan_eapol_if_error_ok;
        }
    
    TInt implementationUid( KCWlanEapolClientUid ); 
    if( mode == core_eapol_operating_mode_wapi )
        {
        implementationUid = KCWlanWapiClientUid;
        }
    
    TRAPD( ret, iEapolClient = CWlanEapolClient::NewL(
        implementationUid, this ) ); // "this" is instance providing SendData()
    if( ret != KErrNone )
        {
        DEBUG1( "ERROR: CWlanEapolClient::NewL leaved with %d.", ret );
        return wlan_eapol_if_error_allocation_error;
        }
    iEapolHandler = partner;
    return wlan_eapol_if_error_ok;
    }

// ---------------------------------------------------------
// CWlmServer::shutdown
// ---------------------------------------------------------
//
u32_t CWlmServer::shutdown()
    {
    DEBUG( "CWlmServer::shutdown" );
    ASSERT( iEapolClient );

    iEapolClient->Shutdown();
    delete iEapolClient;
    iEapolClient = NULL;

    return wlan_eapol_if_error_ok;
    }

// ---------------------------------------------------------
// CWlmServer::configure
// ---------------------------------------------------------
//
u32_t CWlmServer::configure(
    const u32_t header_offset,
    const u32_t MTU,
    const u32_t trailer_length )
    {
    DEBUG( "CWlmServer::configure" );
    ASSERT( iEapolClient );
    return iEapolClient->Configure( header_offset, MTU, trailer_length );
    }
    

// ---------------------------------------------------------
// CWlmServer::process_data
// ---------------------------------------------------------
//
u32_t CWlmServer::process_data(
    const void * const data,
    const u32_t length )
    {
    DEBUG( "CWlmServer::process_data" );
    ASSERT( iEapolClient );
    return iEapolClient->ProcessData( data, length );
    }


// ---------------------------------------------------------
// CWlmServer::HandleCoreAsynchCb
// ---------------------------------------------------------
//
TInt CWlmServer::HandleCoreAsynchCb( TAny* aThisPtr )
    {
    CWlmServer* self = static_cast<CWlmServer*>( aThisPtr );

    /**
     * This could be a problem if the core immediately sets
     * another pending callback request.
     */

    self->iCoreServer->request_complete(
        self->iCoreAsynchCbId,
        self->iCoreAsynchCbStatus );

    self->iCoreAsynchCbId = 0;
    self->iCoreAsynchCbStatus = core_error_ok;

	return KErrNone;    
    }

// ---------------------------------------------------------
// CWlmServer::BackgroundScanRequest
// ---------------------------------------------------------
//
TInt CWlmServer::BackgroundScanRequest(
	TUint aScanStartInterval )
    {
    DEBUG1( "CWlmServer::BackgroundScanRequest() - aScanStartInterval %u", aScanStartInterval );

    // Create list for WLAN IAP data
    core_type_list_c<core_iap_data_s>* iapDataList = new core_type_list_c<core_iap_data_s>;
    if( iapDataList == NULL )
        {
        // Do nothing and hope that on next expiry there is enough memory
        DEBUG( "CWlmServer::BackgroundScanRequest() - Out of memory" );
        return KErrNoMemory;
        }

    // Create list for secondary SSID data
    core_type_list_c<core_ssid_entry_s>* iapSsidList = new core_type_list_c<core_ssid_entry_s>;
    if ( !iapSsidList )
        {
        DEBUG( "CWlmServer::BackgroundScanRequest() - unable to instance core_ssid_entry_s list" );

        delete iapDataList;

        return KErrNoMemory;
        }

    // Read IAP data from CommDb
    RArray<TWlanLimitedIapData> iapList;
    TInt ret = iCache->GetIapDataList( iapList );
    if( ret )
        {
        DEBUG1( "CWlmServer::BackgroundScanRequest() - GetIapDataList failed (%d)", ret );

        delete iapDataList;
        delete iapSsidList;
        iapList.Close();

        return ret;
        }

    // Convert IAP data
    ret = GetIapDataList(
        *iapDataList,
        *iapSsidList,
        iapList );
    if ( ret != KErrNone )
        {
        DEBUG1( "CWlmServer::BackgroundScanRequest() - unable to convert IAP data (%d)", ret );

        delete iapDataList;
        delete iapSsidList;
        iapList.Close();

        return ret;
        }

    iapList.Close();

    // Create output list
    core_type_list_c<core_iap_availability_data_s>* iapAvailabilityList =
        new core_type_list_c<core_iap_availability_data_s>;
    if( iapAvailabilityList == NULL )
        {
        DEBUG( "ERROR: Out of memory" );
        delete iapDataList;
        delete iapSsidList;
        return KErrNoMemory;
        }
    
    // Create ScanList
    ScanList* scanList = new ScanList( KWlmScanListMaxSize );
    if( scanList == NULL )
        {
        DEBUG( "CWlmServer::BackgroundScanRequest() - Out of memory when instantiating ScanList" );
        delete iapDataList;
        delete iapSsidList;
        delete iapAvailabilityList;
        return KErrNoMemory;
        }

    TUint* scanTime = new TUint(
        CalculateScanStartTime( aScanStartInterval ) );
    if( !scanTime )
        {
        DEBUG( "CWlmServer::BackgroundScanRequest() - unable to instantiate TUint" );

        delete iapDataList;
        delete iapSsidList;
        delete iapAvailabilityList;
        delete scanList;
        return KErrNoMemory;
        }
    
    // create mapping
    SRequestMapEntry mapEntry;
    mapEntry.iFunction = EGetAvailableIaps;
    mapEntry.iRequestId = KWlanIntCmdBackgroundScan;
    mapEntry.iSessionId = 0;
    mapEntry.iParam0 = iapDataList;
    mapEntry.iParam1 = iapAvailabilityList;
    mapEntry.iParam2 = scanList;
    mapEntry.iParam3 = iapSsidList;
    mapEntry.iParam4 = reinterpret_cast<TAny*>( EFalse );
    mapEntry.iTime = scanTime;
    iRequestMap.Append( mapEntry );

    if( IsOnlyTimedScanRequestInRequestMap( mapEntry ) || *scanTime < iScanSchedulingTimerExpiration )
        {
        // Scan scheduling timer needs to be set again because this request needs the results earlier
        UpdateScanSchedulingTimer( *scanTime, mapEntry.iRequestId );
        }
    
    return KErrNone;
    }

// ---------------------------------------------------------
// CWlmServer::ScanSchedulingTimerExpired
// ---------------------------------------------------------
//
TInt CWlmServer::ScanSchedulingTimerExpired( TAny* aThisPtr )
    {
    DEBUG( "CWlmServer::ScanSchedulingTimerExpired()" );

    CWlmServer* self = static_cast<CWlmServer*>( aThisPtr );

    TInt index = self->FindRequestIndex( self->iRequestTriggeringScanning );
    
    if( index >= self->iRequestMap.Count() && self->iBgScanProvider->IsBgScanEnabled() )
        {
        /**
         * This is most probably caused by the periodic background scan. It can happen if background
         * scan is enabled but something fails when background scan request is tried to be added to
         * scan scheduling queue.
         * 
         * Add background scan request to scan scheduling queue and it should be handled immediately.
         * Check that there are no background scans pending in the queue. 
         */
        DEBUG( "CWlmServer::ScanSchedulingTimerExpired() - request is not in iRequestMap, but background scan enabled" );

        self->BackgroundScanRequest( 0 );
        }
    else if( index >= self->iRequestMap.Count() )
    	{
        DEBUG( "CWlmServer::ScanSchedulingTimerExpired() - request is not in the iRequestMap, do nothing" );
    	return KErrNone;
    	}
    
    DEBUG1( "CWlmServer::ScanSchedulingTimerExpired() - iRequestId %u", self->iRequestMap[index].iRequestId );
    
    TUint indexNextScan( 0 );
    // If roaming is ongoing or WLAN is OFF, scanning is not started for
    // GetScanResults but instead empty scan list is returned. 
    if ( ( self->iRequestMap[index].iRequestId >= KWlanExtCmdBase && 
           self->iRequestMap[index].iFunction == EGetScanResults ) && 
    	 ( self->IsRoaming() || self->iPlatform->GetWlanOnOffState() != EWlanOn ) )
        {
        DEBUG2( "CWlmServer::ScanSchedulingTimerExpired() - GetScanResults, returning empty list; roaming: %d, WLAN on/off: %d",
            self->IsRoaming(),
            self->iPlatform->GetWlanOnOffState() );

        ScanList* completedScanList = reinterpret_cast<ScanList*>( self->iRequestMap[index].iParam0 );
        core_ssid_s* completedSsid =  reinterpret_cast<core_ssid_s*>( self->iRequestMap[index].iParam1 );
        TUint* completedScanTime = reinterpret_cast<TUint*>( self->iRequestMap[index].iTime );
        
        // Only the triggering request is completed and then scan scheduling timer is set again 
        TPckgBuf<TUint32> pckgCount( 0 );
        TPckgBuf<TUint32> pckgSize( 0 );
        
        TPckgBuf<TDynamicScanList> pckgDynamicScanList;
        pckgDynamicScanList().count = 0;
        pckgDynamicScanList().size = 0;

        if( self->IsSessionActive( self->iRequestMap[index] ) )
            {
            self->iRequestMap[index].iMessage.Write( 2, pckgDynamicScanList );

            if( self->iPlatform->GetWlanOnOffState() != EWlanOn )
                {
                self->iRequestMap[index].iMessage.Complete( self->iPlatform->GetWlanOnOffState() );
                }
            else
                {
                self->iRequestMap[index].iMessage.Complete( KErrServerBusy );
                }
            }

        delete completedScanList;
        completedScanList = NULL;
        delete completedSsid;
        completedSsid = NULL;
        delete completedScanTime;
        completedScanTime = NULL;
        
        self->iRequestMap.Remove( index );

        if( self->FindNextTimedScanSchedulingRequest( indexNextScan ) )
            {
            TUint* nextScanTime = reinterpret_cast<TUint*>( self->iRequestMap[indexNextScan].iTime );
        	self->UpdateScanSchedulingTimer( *nextScanTime, self->iRequestMap[indexNextScan].iRequestId );
            }
        
        DEBUG( "CWlmServer::ScanSchedulingTimerExpired() - message completed with empty scan results" );
        return KErrNone;        
        }
    
    // If the command is GetAvailableIaps
    if ( self->iRequestMap[index].iRequestId >= KWlanExtCmdBase && 
         self->iRequestMap[index].iFunction == EGetAvailableIaps )
        {
        DEBUG( "CWlmServer::ScanSchedulingTimerExpired() - GetAvailableIaps" );
            
        core_type_list_c<core_iap_data_s>* iapDataList = reinterpret_cast<core_type_list_c<core_iap_data_s>*>( self->iRequestMap[index].iParam0 );
        core_type_list_c<core_iap_availability_data_s>* iapAvailabilityList =
            reinterpret_cast<core_type_list_c<core_iap_availability_data_s>*>( self->iRequestMap[index].iParam1 );
        ScanList* scanList =  reinterpret_cast<ScanList*>( self->iRequestMap[index].iParam2 );
        core_type_list_c<core_ssid_entry_s>* iapSsidList =  reinterpret_cast<core_type_list_c<core_ssid_entry_s>*>( self->iRequestMap[index].iParam3 );
        TUint* scanTime =  reinterpret_cast<TUint*>( self->iRequestMap[index].iTime );        

        // If the device is roaming OR
        // there are not WLAN IAPs defined in the device OR
        // WLAN is OFF
        // --> return empty list
        if( self->IsRoaming() ||
            iapDataList->count() == 0 ||
            self->iPlatform->GetWlanOnOffState() != EWlanOn )
            {
            DEBUG3( "CWlmServer::ScanSchedulingTimerExpired() - GetAvailableIaps, returning empty list; roaming: %d, iaps: %d, WLAN on/off: %d",
                self->IsRoaming(),
                iapDataList->count(),
                self->iPlatform->GetWlanOnOffState() );    
            
            // Only the triggering request is completed and then scan scheduling timer is set again 
            if( self->IsSessionActive( self->iRequestMap[index] ) )
                {
                TWlmAvailableIaps tmp = { 0 };
                TPckg<TWlmAvailableIaps> outPckg( tmp );
                self->iRequestMap[index].iMessage.Write( 0, outPckg );

                if( self->iPlatform->GetWlanOnOffState() != EWlanOn )
                    {
                    self->iRequestMap[index].iMessage.Complete( self->iPlatform->GetWlanOnOffState() );
                    }
                else if( iapDataList->count() == 0 )
                    {
                    self->iRequestMap[index].iMessage.Complete( KErrNone );
                    }
                else
                    {
                    self->iRequestMap[index].iMessage.Complete( KErrServerBusy );
                    }
                }

            delete iapDataList;
            iapDataList = NULL;
            delete iapAvailabilityList;
            iapAvailabilityList = NULL;
            delete scanList;
            scanList = NULL;
            delete iapSsidList;
            iapSsidList = NULL;
            delete scanTime;
            scanTime = NULL;
        
            self->iRequestMap.Remove( index );

            if( self->FindNextTimedScanSchedulingRequest( indexNextScan ) )
                {
                TUint* nextScanTime = reinterpret_cast<TUint*>( self->iRequestMap[indexNextScan].iTime );
                self->UpdateScanSchedulingTimer( *nextScanTime, self->iRequestMap[indexNextScan].iRequestId );
                }
        
            DEBUG( "CWlmServer::ScanSchedulingTimerExpired() - message completed with empty iap list" );
            return KErrNone;        
            }
        }

    // If triggering request is background scan and WLAN connection exist, background scan is skipped
    if( self->iRequestMap[index].iRequestId == KWlanIntCmdBackgroundScan && 
    	self->iConnectionState != EWlanStateNotConnected )
        {
        DEBUG("CWlmServer::ScanSchedulingTimerExpired() - active connection exists: skipping background scan");
 
        // notify bg scan provider so it will issue a new CWlmServer::Scan() call,
        // where the existing request map entry will be updated
        self->iBgScanProvider->ScanComplete();
        
        return KErrNone;
        }

    // Scan mode is either passive or active
    core_scan_mode_e mode = core_scan_mode_active;
    TBool isActiveScanAllowed( ETrue );
    if( self->iPlatform->GetScanType() == EWLMScanForcedPassive )
        {
        mode = core_scan_mode_passive;
        isActiveScanAllowed = EFalse;
        }

    if( self->iRequestMap[index].iRequestId == KWlanIntCmdBackgroundScan )
        {
        DEBUG( "CWlmServer::ScanSchedulingTimerExpired() - starting background scan, cancel timer" );
        // pass request to core
        core_type_list_c<core_iap_data_s>* iapDataList = reinterpret_cast<core_type_list_c<core_iap_data_s>*>( self->iRequestMap[index].iParam0 );
        core_type_list_c<core_ssid_entry_s>* iapSsidList = reinterpret_cast<core_type_list_c<core_ssid_entry_s>*>( self->iRequestMap[index].iParam3 );
        core_type_list_c<core_iap_availability_data_s>* iapAvailabilityList =
            reinterpret_cast<core_type_list_c<core_iap_availability_data_s>*>( self->iRequestMap[index].iParam1 );
        ScanList* scanList = reinterpret_cast<ScanList*>( self->iRequestMap[index].iParam2 );
        
        self->iCoreServer->get_available_iaps(
            self->iRequestMap[index].iRequestId,
            isActiveScanAllowed,
            *iapDataList,
            *iapAvailabilityList,
            iapSsidList,
            *scanList );
        
        self->iScanSchedulingTimer->Cancel();
        }
    else if( self->iRequestMap[index].iFunction == EGetScanResults )
        {
        DEBUG( "CWlmServer::ScanSchedulingTimerExpired() - starting GetScanResults, cancel timer" );
        
        // pass request to core
        ScanList* tmp = reinterpret_cast<ScanList*>( self->iRequestMap[index].iParam0);
        core_ssid_s* ssid2 = reinterpret_cast<core_ssid_s*>( self->iRequestMap[index].iParam1 );
        
        self->iCoreServer->get_scan_result(
            self->iRequestMap[index].iRequestId,
            mode,
            *ssid2,
            SCAN_CHANNELS_2DOT4GHZ_ETSI,
            0,
            *tmp,
            false_t );
        
        self->iScanSchedulingTimer->Cancel();
        }
    else if( self->iRequestMap[index].iFunction == EGetAvailableIaps )
        {
        DEBUG( "CWlmServer::ScanSchedulingTimerExpired() - starting GetAvailableIaps, cancel timer" );
            
        // pass request to core
        core_type_list_c<core_iap_data_s>* iapDataList = reinterpret_cast<core_type_list_c<core_iap_data_s>*>( self->iRequestMap[index].iParam0 );
        core_type_list_c<core_ssid_entry_s>* iapSsidList = reinterpret_cast<core_type_list_c<core_ssid_entry_s>*>( self->iRequestMap[index].iParam3 );
        core_type_list_c<core_iap_availability_data_s>* iapAvailabilityList =
            reinterpret_cast<core_type_list_c<core_iap_availability_data_s>*>( self->iRequestMap[index].iParam1 );
        ScanList* scanList = reinterpret_cast<ScanList*>( self->iRequestMap[index].iParam2 );
        
        self->iCoreServer->get_available_iaps(
            self->iRequestMap[index].iRequestId,
            isActiveScanAllowed,
            *iapDataList,
            *iapAvailabilityList,
            iapSsidList,
            *scanList );
        
        self->iScanSchedulingTimer->Cancel();
        }
    self->iCoreHandlingScanRequest = ETrue;
    return KErrNone;
   }

// ---------------------------------------------------------
// CWlmServer::ScanSchedulingTimerCanceled()
// ---------------------------------------------------------
//
TInt CWlmServer::ScanSchedulingTimerCanceled(
    TAny* aThisPtr )
    {
    DEBUG( "CWlmServer::ScanSchedulingTimerCanceled()" );

    CWlmServer* self = reinterpret_cast<CWlmServer*>( aThisPtr );

    /**
     * Scan scheduling timer was canceled, re-arm the timer. 
     */
    TUint indexNextScan( 0 );
    if( self->FindNextTimedScanSchedulingRequest( indexNextScan ) )
        {
        self->UpdateScanSchedulingTimer(
            *(self->iRequestMap[indexNextScan].iTime),
            self->iRequestMap[indexNextScan].iRequestId );
        }

    return 0;
    }

// ---------------------------------------------------------
// CWlmServer::EmptyCb()
// ---------------------------------------------------------
//
TInt CWlmServer::EmptyCb(
    TAny* /* aThisPtr */ )
    {
    return 0;
    }

    
// ---------------------------------------------------------
// CWlmServer::GetIapDataList()
// ---------------------------------------------------------
//
TInt CWlmServer::GetIapDataList(
    core_type_list_c<core_iap_data_s>& aCoreIapDataList,
    core_type_list_c<core_ssid_entry_s>& aCoreIapSsidList,
    const RArray<TWlanLimitedIapData>& aAmIapDataList )
    {
    DEBUG( "CWlmServer::GetIapDataList()" );
    DEBUG1( "CWlmServer::GetIapDataList() - converting %u entries",
        aAmIapDataList.Count() );

    /**
     * Create a commsdat session for retrieving possible secondary SSIDs.
     */
    CWLanSettings wlanSettings;
    TInt ret = wlanSettings.Connect();
    if ( ret != KErrNone )
        {
        DEBUG1( "CWlmServer::GetIapDataList() - CWLanSettings::Connect() failed (%d)",
            ret );

        return ret;
        }

    // Loop given list of IAPs and create respective list of iap data
    for( TInt idx( 0 ); idx < aAmIapDataList.Count(); idx++ )
        {
        if( aAmIapDataList[idx].ssid.Length() ) // filter out EasyWlan IAP
            {
            DEBUG1( "CWlmServer::GetIapDataList() - converting IAP %u",
                aAmIapDataList[idx].iapId );

            core_iap_data_s* coreData = new core_iap_data_s;
            if( !coreData )
                {
                DEBUG( "CWlmServer::GetIapDataList() - unable to instantiate core_iap_data_s" );
                wlanSettings.Disconnect();

                return KErrNoMemory;
                }

            TWlanConversionUtil::ConvertIapSettings(
                *coreData,
                aAmIapDataList[idx] );

            ret = aCoreIapDataList.append( coreData );
            if( ret )
                {
                DEBUG1( "CWlmServer::GetIapDataList() - unable to append core_iap_data_s entry (%u)",
                    ret );
                wlanSettings.Disconnect();
                delete coreData;

                return ret;
                }

            // Read possible secondary ssids
            RArray<TWlanSecondarySsid> secondarySsidList;
            wlanSettings.GetSecondarySsidsForService(
                aAmIapDataList[idx].serviceId,
                secondarySsidList );
                
            // Create secondary SSID list if needed
            if( aAmIapDataList[idx].usedSsid.Length() || secondarySsidList.Count() )
                {
                DEBUG1( "CWlmServer::GetIapDataList() - IAP %u has secondary SSIDs defined",
                    coreData->id );

                core_ssid_entry_s* entry = new core_ssid_entry_s;
                if ( !entry )
                    {
                    DEBUG( "CWlmServer::GetIapDataList() - unable to instantiate core_ssid_entry_s" );

                    secondarySsidList.Close();
                    wlanSettings.Disconnect();

                    return KErrNoMemory;
                    }

                entry->id = coreData->id;
                entry->ssid = coreData->ssid;
                if( aAmIapDataList[idx].usedSsid.Length() )
                    {
                    TWlanConversionUtil::ConvertSSID( entry->used_ssid, aAmIapDataList[idx].usedSsid );
                    }
                else
                    {
                    entry->used_ssid = entry->ssid;
                    }
                aCoreIapSsidList.append( entry );

                for( TInt idx( 0 ); idx < secondarySsidList.Count(); ++idx )
                    {
                    entry = new core_ssid_entry_s;
                    if ( !entry )
                        {
                        DEBUG( "CWlmServer::GetIapDataList() - unable to instantiate core_ssid_entry_s" );

                        secondarySsidList.Close();
                        wlanSettings.Disconnect();

                        return KErrNoMemory;
                        }

                    entry->id = coreData->id;
                    TWlanConversionUtil::ConvertSSID( 
                        entry->ssid, secondarySsidList[idx].ssid );
                    if ( secondarySsidList[idx].usedSsid.Length() )
                        {
                        TWlanConversionUtil::ConvertSSID(
                            entry->used_ssid, secondarySsidList[idx].usedSsid );
                        }
                    else
                        {
                        entry->used_ssid = coreData->ssid;
                        }
                    aCoreIapSsidList.append( entry );
                    }
                }

            secondarySsidList.Close();
            }
        } // for loop

    wlanSettings.Disconnect();

    return KErrNone;
    }

// ---------------------------------------------------------
// CWlmServer::GetLanSettings()
// ---------------------------------------------------------
//
TInt CWlmServer::GetLanSettings(
    TUint32 aLanServiceId, 
    SLanSettings& aLanSettings )
    {
    DEBUG("CWlmServer::GetLanSettings()");
    
    TInt ret = KErrNone;
    CLanSettings* lanSettings = new CLanSettings;
    if( lanSettings == NULL )
        {
        DEBUG( "ERROR creating CLanSettings" );
        return KErrNoMemory;
        }

    ret = lanSettings->Connect();
    if( ret != KErrNone )
        {
        DEBUG1( "ERROR - Connect() failed with %d", ret );
        lanSettings->Disconnect();
        delete lanSettings;
        return ret;
        }
        
    ret = lanSettings->GetLanSettings( aLanServiceId, aLanSettings );
    if( ret != KErrNone )
        {
        DEBUG1( "ERROR - GetLanSettings() failed with %d", ret );
        }
    lanSettings->Disconnect();
    delete lanSettings;    
    return ret;
    }

// ---------------------------------------------------------
// CWlmServer::BtConnectionEstablished
// ---------------------------------------------------------
//
void CWlmServer::BtConnectionEstablished()
    {        
    iDriverIf->Notify( core_am_indication_bt_connection_established );
    }

// ---------------------------------------------------------
// CWlmServer::BtConnectionDisabled
// ---------------------------------------------------------
//
void CWlmServer::BtConnectionDisabled()
    {
    iDriverIf->Notify( core_am_indication_bt_connection_disconnected );
    }

// ---------------------------------------------------------
// CWlmServer::SystemTimeChanged
// ---------------------------------------------------------
//
void CWlmServer::SystemTimeChanged()
    {
    DEBUG("CWlmServer::SystemTimeChanged()");
   
    if ( iIsStartupComplete )
    	{
    	// If the changed time is less than cached region timestamp,
    	// then cache is cleared.
    	TTime newTime;
    	newTime.UniversalTime();
    	
    	if ( newTime.Int64() < iTimeofDomainQuery.Int64() )
    	    {
    		// Region cache expires when user has changed the time
            TTime timestampClear(1);
            iTimeofDomainQuery = timestampClear;
            
            StoreRegionAndTimestamp( KWlmRegionUnknown, ( iTimeofDomainQuery.Int64() / KWlmTimestampInCenrep ) );
    	    }
    	
        DEBUG("CWlmServer::SystemTimeChanged() - inform timer services about system time change");
    	iTimerServices->HandleTimeout();
        DEBUG("CWlmServer::SystemTimeChanged() - refreshing settings to BgScan provider");
        iBgScanProvider->NotifyChangedSettings( iBgScanProviderSettings );
    	}
    }

// ---------------------------------------------------------
// CWlmServer::ClearRegionCache
// ---------------------------------------------------------
//
void CWlmServer::ClearRegionCache()
    {
    DEBUG("CWlmServer::ClearRegionCache()");
   
    // Region cache expires when user has changed the time
    TTime timestampClear(1);
    iTimeofDomainQuery = timestampClear;
    
    StoreRegionAndTimestamp( KWlmRegionUnknown, ( iTimeofDomainQuery.Int64() / KWlmTimestampInCenrep )  );

    }

// ---------------------------------------------------------
// CWlmServer::SendData
// ---------------------------------------------------------
//
TInt CWlmServer::SendData(
    const void * const aData, 
    const TInt aLength )
    {
    DEBUG2( "CWlmServer::SendData(data=0x%08X, length=%i)", aData, aLength );
    ASSERT( iEapolHandler );
    return iEapolHandler->send_data( aData, aLength );
    }

// ---------------------------------------------------------
// CWlmServer::Scan
// ---------------------------------------------------------
//
void CWlmServer::Scan(
    const TUint& aMaxDelay )
    {
    DEBUG1( "CWlmServer::Scan() - aMaxDelay %u", aMaxDelay );
    
    // - by design, this method is called only if background scan is enabled
    ASSERT( iBgScanProvider->IsBgScanEnabled() );
    // - by design, this method is only called if iIsStartupComplete is true
    ASSERT( iIsStartupComplete );
    // - by design, a background scan request can already be in the queue
    //   -> instead of creating a new request to the request map, modify the existing one
    // - by design, background scan request might be issued even if we're connected
    // - update scan scheduling timer if needed
        
    TInt index = FindRequestIndex( KWlanIntCmdBackgroundScan ); 

    if( index >= iRequestMap.Count() )
        {
        // there are no pending background scan requests
        DEBUG( "CWlmServer::Scan() - there are NO pending background scan requests" );
                
        BackgroundScanRequest( aMaxDelay ); 
        }
    else
        {
        DEBUG1( "CWlmServer::Scan() - there is a pending background scan request, index (%d)", index );
        
        ASSERT( iRequestMap[index].iTime );
        
        DEBUG( "CWlmServer::Scan() - cancel scan scheduling timer (might already be cancelled but it doesn't matter)" );
        iScanSchedulingTimer->Cancel();

        if( !iCoreHandlingScanRequest )
            {
            DEBUG( "CWlmServer::Scan() - core is not handling background scan request" );
            
            DEBUG( "CWlmServer::Scan() - set new scan start time for existing background scan request" );
            *iRequestMap[index].iTime = CalculateScanStartTime( aMaxDelay );
            
            DEBUG( "CWlmServer::Scan() - find the scan scheduling request which expires next" );
            TUint indexNextScan;
            if( FindNextTimedScanSchedulingRequest( indexNextScan ) )
                {
                UpdateScanSchedulingTimer( *iRequestMap[indexNextScan].iTime,
                                            iRequestMap[indexNextScan].iRequestId );
                }
#ifdef _DEBUG
            else
                {
                DEBUG( "CWlmServer::Scan() - no scan scheduling request found!!" );
                ASSERT( 0 );
                }
#endif
            }
        else
            {
            DEBUG( "CWlmServer::Scan() - core is currently handling some scan request" );
            
            if( iRequestTriggeringScanning == iRequestMap[index].iRequestId )
                {
                DEBUG( "CWlmServer::Scan() - core is handling background scanning currently" );
                DEBUG( "CWlmServer::Scan() - no need to update the existing background scan request" );
                }
            else
                {
                DEBUG( "CWlmServer::Scan() - core is NOT handling background scanning currently" );
                DEBUG( "CWlmServer::Scan() - just update the time to the background scan request" );
                *iRequestMap[index].iTime = CalculateScanStartTime( aMaxDelay );
                }
            }
        }
    
    DEBUG( "CWlmServer::Scan() - returning" );
    }

// ---------------------------------------------------------
// CWlmServer::CancelScan
// ---------------------------------------------------------
//
void CWlmServer::CancelScan()
    {
    DEBUG( "CWlmServer::CancelScan()" );
    
    // if background scan is not enabled anymore, set icon
    if( iConnectionState == EWlanStateNotConnected &&
        !iBgScanProvider->IsBgScanEnabled() )
        {
        SetIconState( EWlmIconStatusNotAvailable );
        }

    // look for the background scan request
    TUint index = FindRequestIndex( KWlanIntCmdBackgroundScan ); 

    if( index < iRequestMap.Count() )
        {
        // pending background scan request found
        if( !iCoreHandlingScanRequest )
            {
            DEBUG( "CWlmServer::CancelScan() - core is not yet handling the next scan request" );
            
            DEBUG( "CWlmServer::CancelScan() - cancel timer" );
            iScanSchedulingTimer->Cancel();

            DEBUG( "CWlmServer::CancelScan() - remove entry from request map" );
            SRequestMapEntry entry = iRequestMap[index];
            delete reinterpret_cast<core_type_list_c<core_iap_data_s>*>( entry.iParam0 );
            delete reinterpret_cast<core_type_list_c<core_iap_availability_data_s>*>( entry.iParam1 );
            delete reinterpret_cast<ScanList*>( entry.iParam2);
            delete reinterpret_cast<core_type_list_c<core_ssid_entry_s>*>( entry.iParam3 );
            delete reinterpret_cast<TUint*>( entry.iTime );
            iRequestMap.Remove( index );

            DEBUG( "CWlmServer::CancelScan() - find next possible timed scan scheduling request" );
            TUint indexNextScan;
            if( FindNextTimedScanSchedulingRequest( indexNextScan ) )
                {
                UpdateScanSchedulingTimer( *iRequestMap[indexNextScan].iTime, iRequestMap[indexNextScan].iRequestId );
                }
#ifdef _DEBUG
            else
                {
                DEBUG( "CWlmServer::CancelScan() - no next timed scan scheduling request found" );                
                }
#endif
            }
        else
            {
            // Core is currently handling scanning
            DEBUG( "CWlmServer::CancelScan() - core is currently handling scanning" );
            
            if( iRequestTriggeringScanning == iRequestMap[index].iRequestId )
                {
                DEBUG( "CWlmServer::CancelScan() - core is handling background scanning currently" );
                DEBUG( "CWlmServer::CancelScan() - no need to remove background scan request" );
                }
            else
                {
                DEBUG( "CWlmServer::CancelScan() - core is NOT handling background scanning currently" );
                DEBUG( "CWlmServer::CancelScan() - just remove the entry from the request map" );
                
                DEBUG( "CWlmServer::CancelScan() - remove entry from request map" );
                SRequestMapEntry entry = iRequestMap[index];
                delete reinterpret_cast<core_type_list_c<core_iap_data_s>*>( entry.iParam0 );
                delete reinterpret_cast<core_type_list_c<core_iap_availability_data_s>*>( entry.iParam1 );
                delete reinterpret_cast<ScanList*>( entry.iParam2);
                delete reinterpret_cast<core_type_list_c<core_ssid_entry_s>*>( entry.iParam3 );
                delete reinterpret_cast<TUint*>( entry.iTime );                
                iRequestMap.Remove( index );
                }
            }
        }
    DEBUG( "CWlmServer::CancelScan() - returning" );
    }

// ---------------------------------------------------------
// CWlmServer::StartAggressiveBgScan
// ---------------------------------------------------------
//
void CWlmServer::StartAggressiveBgScan(
    TUint /* aSessionId */,
    const RMessage2& aMessage )
    {
    DEBUG( "CWlmServer::StartAggressiveBgScan" );
    
    // Scan interval in seconds
    TUint32 ScanInterval( aMessage.Int0() );
    
    aMessage.Complete( KErrNone );
    
    // Check that requested interval is in valid range
    // and if it is not, adjust the value.
    if( ScanInterval < KWlanAggressiveBgScanMinInterval )
        {
        DEBUG1( "CWlmServer::StartAggressiveBgScan - Requested value (%u) is below minimum limit",
            ScanInterval );
        ScanInterval = KWlanAggressiveBgScanMinInterval;
        }
    else if( ScanInterval > KWlanAggressiveBgScanMaxInterval )
        {
        DEBUG1( "CWlmServer::StartAggressiveBgScan - Requested value (%u) is above maximum limit",
            ScanInterval );
        ScanInterval = KWlanAggressiveBgScanMaxInterval;
        }
    
    // Calculate from scan interval how long the aggressive
    // mode will be kept active
    TUint32 duration( KWlanAggressiveBgScanDurationMultiplier *
                     ScanInterval *
                     KWlanSecsToMicrosecsMultiplier );
    
    // Forward request to BG scan module
    iBgScanProvider->StartAggressiveBgScan(
        ScanInterval,
        duration );
    }

// ---------------------------------------------------------
// CWlmServer::GetPacketStatistics
// ---------------------------------------------------------
//
void CWlmServer::GetPacketStatistics(
    TUint aSessionId,
    const RMessage2& aMessage )
    {
    DEBUG( "CWlmServer::GetPacketStatistics()" );

    // create mapping
    SRequestMapEntry mapEntry;
    mapEntry.iMessage = aMessage;
    mapEntry.iFunction = EGetPacketStatistics;
    mapEntry.iRequestId = iRequestIdCounter++;
    mapEntry.iSessionId = aSessionId;    
    core_packet_statistics_s* tmp = new core_packet_statistics_s;
    if( tmp == NULL )
        {
        aMessage.Complete( KErrNoMemory );
        return;
        }
    mapEntry.iParam0 = tmp;
    iRequestMap.Append( mapEntry );
    
    // pass request to core
    iCoreServer->get_packet_statistics(
        mapEntry.iRequestId,
        *tmp );    
    }

// ---------------------------------------------------------
// CWlmServer::ClearPacketStatistics
// ---------------------------------------------------------
//       
void CWlmServer::ClearPacketStatistics(
    TUint /* aSessionId */,
    const RMessage2& aMessage )
    {        
    DEBUG( "CWlmServer::ClearPacketStatistics()" );
    
    core_error_e ret = iCoreServer->clear_packet_statistics();
    if ( ret != core_error_ok )
        {
        DEBUG1( "CWlmServer::ClearPacketStatistics() - clear_packet_statistics() failed with %u",
            ret );
        }

    aMessage.Complete(
        TWlanConversionUtil::ConvertErrorCode( ret ) );
    }

// ---------------------------------------------------------
// CWlmServer::GetUapsdSettings
// ---------------------------------------------------------
//
void CWlmServer::GetUapsdSettings(
    TUint /* aSessionId */,
    const RMessage2& aMessage )
    {
    DEBUG( "CWlmServer::GetUapsdSettings()" );    

    core_uapsd_settings_s coreSettings;
    core_error_e ret = iCoreServer->get_uapsd_settings( coreSettings );
    if ( ret != core_error_ok )
        {
        DEBUG1( "CWlmServer::GetUapsdSettings() - get_uapsd_settings() failed with %u",
            ret );
        }

    TWlanUapsdSettings settings;
    TWlanConversionUtil::ConvertUapsdSettings(
        settings,
        coreSettings );
    TPckg<TWlanUapsdSettings> outPckg( settings );
    aMessage.Write( 0, outPckg );
    aMessage.Complete(
        TWlanConversionUtil::ConvertErrorCode( ret ) );
    }

// ---------------------------------------------------------
// CWlmServer::SetUapsdSettings
// ---------------------------------------------------------
//
void CWlmServer::SetUapsdSettings(
    TUint /* aSessionId */,
    const RMessage2& aMessage )
    {
    DEBUG( "CWlmServer::SetUapsdSettings()" );

    TWlanUapsdSettings settings = 
        { EWlanMaxServicePeriodLengthAll, ETrue, ETrue, ETrue, ETrue };
    TPckg<TWlanUapsdSettings> inPckg( settings );
    TInt err( aMessage.Read( 0, inPckg ) );
    if( err != KErrNone )
        {
        aMessage.Complete( err );
        return;
        }

    core_uapsd_settings_s coreSettings;
    TWlanConversionUtil::ConvertUapsdSettings(
        coreSettings,
        settings );    
    
    core_error_e ret = iCoreServer->set_uapsd_settings( coreSettings );
    if ( ret != core_error_ok )
        {
        DEBUG1( "CWlmServer::SetUapsdSettings() - set_uapsd_settings() failed with %u",
            ret );
        }

    aMessage.Complete(
        TWlanConversionUtil::ConvertErrorCode( ret ) );
    }

// ---------------------------------------------------------
// CWlmServer::GetPowerSaveSettings
// ---------------------------------------------------------
//
void CWlmServer::GetPowerSaveSettings(
    TUint /* aSessionId */,
    const RMessage2& aMessage )
    {
    DEBUG( "CWlmServer::GetPowerSaveSettings()" );    

    core_power_save_settings_s coreSettings;
    core_error_e ret = iCoreServer->get_power_save_settings( coreSettings );
    if ( ret != core_error_ok )
        {
        DEBUG1( "CWlmServer::GetPowerSaveSettings() - get_power_save_settings() failed with %u",
            ret );
        }

    TWlanPowerSaveSettings settings;
    TWlanConversionUtil::ConvertPowerSaveSettings(
        settings,
        coreSettings );
    TPckg<TWlanPowerSaveSettings> outPckg( settings );
    aMessage.Write( 0, outPckg );
    aMessage.Complete(
        TWlanConversionUtil::ConvertErrorCode( ret ) );
    }

// ---------------------------------------------------------
// CWlmServer::SetPowerSaveSettings
// ---------------------------------------------------------
//
void CWlmServer::SetPowerSaveSettings(
    TUint /* aSessionId */,
    const RMessage2& aMessage )
    {
    DEBUG( "CWlmServer::SetPowerSaveSettings()" );

    TWlanPowerSaveSettings settings =
        { ETrue, EFalse, EFalse, EFalse, EFalse, EFalse, EFalse, EFalse };
    TPckg<TWlanPowerSaveSettings> inPckg( settings );
    TInt err( aMessage.Read( 0, inPckg ) );
    if( err != KErrNone )
        {
        aMessage.Complete( err );
        return;
        }

    core_power_save_settings_s coreSettings;
    TWlanConversionUtil::ConvertPowerSaveSettings(
        coreSettings,
        settings );

    core_error_e ret = iCoreServer->set_power_save_settings( coreSettings );
    if ( ret != core_error_ok )
        {
        DEBUG1( "CWlmServer::SetPowerSaveSettings() - set_power_save_settings() failed with %u",
            ret );
        }

    aMessage.Complete(
        TWlanConversionUtil::ConvertErrorCode( ret ) );
    }

// ---------------------------------------------------------
// CWlmServer::RunProtectedSetup
// ---------------------------------------------------------
//
void CWlmServer::RunProtectedSetup(
    TUint aSessionId,
    const RMessage2& aMessage )
    {
    DEBUG( "CWlmServer::RunProtectedSetup()" );
    
    // Check that WLAN is ON
	TWlanOnOffState wlanState( iPlatform->GetWlanOnOffState() );
	if( wlanState != EWlanOn )
	    {
		// WLAN is OFF and therefore request is not served.
		DEBUG1( "CWlmServer::RunProtectedSetup() - rejected due to WLAN is OFF (%d)",
		    wlanState );
		// WLAN states map one to one to WLAN error codes.
		aMessage.Complete( wlanState );
		return;
		}
        
    TPckgBuf<TWlanSsid> ssidPckg;
    TInt ret( aMessage.Read( 0, ssidPckg ) );
    if( ret != KErrNone )
        {
        aMessage.Complete( ret );
        return;
        }
    
    TPckgBuf<TWlanWpsPin> wpsPinPckg;
    ret = aMessage.Read( 1, wpsPinPckg );
    if( ret != KErrNone )
        {
        aMessage.Complete( ret );
        return;
        }
    
    core_iap_data_s* coreIapData = new core_iap_data_s;
    if( !coreIapData )
        {
        aMessage.Complete( KErrNoMemory );
        return;
        }
    
    coreIapData->id = 0;
    coreIapData->op_mode = core_operating_mode_infrastructure;
    coreIapData->security_mode = core_security_mode_protected_setup;
    coreIapData->wpa_preshared_key_in_use = ETrue; 
    coreIapData->is_wpa_overriden = ETrue;
    
    // Type conversion
    TWlanConversionUtil::ConvertSSID(
         coreIapData->ssid,
         ssidPckg() );
    
    // Type conversion
    TWlanConversionUtil::ConvertWpaPreSharedKey(
         coreIapData->wpa_preshared_key,
         wpsPinPckg() );
    
    // Create a list for the results.  
    core_type_list_c<core_iap_data_s>* iapDataList = new core_type_list_c<core_iap_data_s>;
    if( iapDataList == NULL )
        {
        DEBUG( "CWlmServer::RunProtectedSetup() - unable to create iapDataList" );
        delete coreIapData;

        aMessage.Complete( KErrNoMemory );
        return;
        }

    // Protected Setup status
    core_protected_setup_status_e* protectedSetupStatus = new core_protected_setup_status_e;
    if( !protectedSetupStatus )
        {
        delete coreIapData;
        delete iapDataList;

        aMessage.Complete( KErrNoMemory );
        return;
        }
    *protectedSetupStatus = core_protected_setup_status_undefined;

    // create mapping
    SRequestMapEntry mapEntry;
    mapEntry.iMessage = aMessage;
    mapEntry.iFunction = ERunProtectedSetup;
    mapEntry.iRequestId = iRequestIdCounter++;
    mapEntry.iSessionId = aSessionId;    
    mapEntry.iParam0 = coreIapData;
    mapEntry.iParam1 = iapDataList;
    mapEntry.iParam2 = protectedSetupStatus;
    iRequestMap.Append( mapEntry );

    // pass request to core
    iCoreServer->run_protected_setup(
        mapEntry.iRequestId, *coreIapData, *iapDataList, *protectedSetupStatus );
    }

// ---------------------------------------------------------
// CWlmServer::CancelProtectedSetup
// ---------------------------------------------------------
//
void CWlmServer::CancelProtectedSetup(
    TUint aSessionId,
    const RMessage2& aMessage )
    {
    DEBUG( "CWlmServer::CancelProtectedSetup()" );
    
    CancelExternalRequest(
        aSessionId,
        ERunProtectedSetup );
    
    aMessage.Complete( KErrNone );
    }

// ---------------------------------------------------------
// CWlmServer::CreateTrafficStream
// ---------------------------------------------------------
//
void CWlmServer::CreateTrafficStream(
    TUint aSessionId,
    const RMessage2& aMessage )
    {
    DEBUG( "CWlmServer::CreateTrafficStream()" );
    
    TPckgBuf<TWlanTrafficStreamParameters> paramsPckg( 0 );
    TInt ret( aMessage.Read( 0, paramsPckg ) );
    if( ret != KErrNone )
        {
        aMessage.Complete( ret );
        return;
        }
    if( paramsPckg().iUserPriority > MAX_USER_PRIORITY )
        {
        aMessage.Complete( KErrArgument );
        return;
        }

    TBool isAutomatic = aMessage.Int1();

    u32_t* coreStreamId = new u32_t;
    core_traffic_stream_status_e* coreStreamStatus = new core_traffic_stream_status_e;
    if( !coreStreamId ||
        !coreStreamStatus )
        {
        delete coreStreamId;
        delete coreStreamStatus;
        
        aMessage.Complete( KErrNoMemory );
        return;        
        }

    u8_t coreTid( TRAFFIC_STREAM_TID_NONE );
    u8_t coreUserPriority( 0 );
    core_traffic_stream_params_s coreParams = { 0 };
    TWlanConversionUtil::ConvertTrafficStreamParameters(
        coreTid,
        coreUserPriority,
        coreParams,
        paramsPckg() );

    // create mapping
    SRequestMapEntry mapEntry;
    mapEntry.iMessage = aMessage;
    mapEntry.iFunction = ECreateTrafficStream;
    mapEntry.iRequestId = iRequestIdCounter++;
    mapEntry.iSessionId = aSessionId;    
    mapEntry.iParam0 = coreStreamId;
    mapEntry.iParam1 = coreStreamStatus;
    iRequestMap.Append( mapEntry );

    // pass request to core
    iCoreServer->create_traffic_stream(
        mapEntry.iRequestId,
        coreTid,
        coreUserPriority,
        isAutomatic,
        coreParams,
        *coreStreamId,
        *coreStreamStatus );
    }

// ---------------------------------------------------------
// CWlmServer::CancelCreateTrafficStream
// ---------------------------------------------------------
//
void CWlmServer::CancelCreateTrafficStream(
    TUint aSessionId,
    const RMessage2& aMessage )
    {
    CancelExternalRequest(
        aSessionId,
        ECreateTrafficStream );

    aMessage.Complete( KErrNone ); 
    }

// ---------------------------------------------------------
// CWlmServer::DeleteTrafficStream
// ---------------------------------------------------------
//
void CWlmServer::DeleteTrafficStream(
    TUint aSessionId,
    const RMessage2& aMessage )
    {
    DEBUG( "CWlmServer::DeleteTrafficStream()" );

    u32_t streamId = aMessage.Int0();

    // create mapping
    SRequestMapEntry mapEntry;
    mapEntry.iMessage = aMessage;
    mapEntry.iFunction = EDeleteTrafficStream;
    mapEntry.iRequestId = iRequestIdCounter++;
    mapEntry.iSessionId = aSessionId;    
    iRequestMap.Append( mapEntry );

    // pass request to core
    iCoreServer->delete_traffic_stream(
        mapEntry.iRequestId,
        streamId ); 
    }

// ---------------------------------------------------------
// CWlmServer::CancelDeleteTrafficStream
// ---------------------------------------------------------
//
void CWlmServer::CancelDeleteTrafficStream(
    TUint aSessionId,
    const RMessage2& aMessage )
    {
    CancelExternalRequest(
        aSessionId,
        EDeleteTrafficStream );

    aMessage.Complete( KErrNone );   
    }

// ---------------------------------------------------------
// CWlmServer::GetAccessPointInfo
// ---------------------------------------------------------
//
void CWlmServer::GetAccessPointInfo(
    TUint /* aSessionId */,
    const RMessage2& aMessage )
    {
    DEBUG( "CWlmServer::GetAccessPointInfo()" );

    core_ap_information_s coreInfo;
    core_error_e ret = iCoreServer->get_current_ap_info( coreInfo );
    if ( ret != core_error_ok )
        {
        DEBUG1( "CWlmServer::GetAccessPointInfo() - get_current_ap_info() failed with %u",
            ret );
        }
    
    TWlanAccessPointInfo info;
    TWlanConversionUtil::ConvertApInformation(
        info,
        coreInfo );
    TPckg<TWlanAccessPointInfo> outPckg( info );
    aMessage.Write( 0, outPckg );
    aMessage.Complete(
        TWlanConversionUtil::ConvertErrorCode( ret ) );
    }

// ---------------------------------------------------------
// CWlmServer::GetRoamMetrics
// ---------------------------------------------------------
//
void CWlmServer::GetRoamMetrics(
    TUint /* aSessionId */,
    const RMessage2& aMessage )
    {
    DEBUG( "CWlmServer::GetRoamMetrics()" );

    core_roam_metrics_s coreRoamMetrics;
    iCoreServer->get_roam_metrics( coreRoamMetrics );
    
    TWlanRoamMetrics roamMetrics;
    TWlanConversionUtil::ConvertRoamMetrics(
        roamMetrics,
        coreRoamMetrics );

    TPckg<TWlanRoamMetrics> outPckg( roamMetrics );
    aMessage.Write( 0, outPckg );
    aMessage.Complete(
        TWlanConversionUtil::ConvertErrorCode( core_error_ok ) );
    }

// ---------------------------------------------------------
// CWlmServer::GetRogueList
// ---------------------------------------------------------
//
void CWlmServer::GetRogueList(
    TUint /* aSessionId */,
    const RMessage2& aMessage )
    {
    DEBUG( "CWlmServer::GetRogueList()" );

    core_type_list_c<core_mac_address_s> coreRogueList;
    core_error_e ret = iCoreServer->get_rogue_list( coreRogueList );
    if ( ret != core_error_ok )
        {
        DEBUG1( "CWlmServer::GetRogueList() - get_rogue_list() failed with %u",
            ret );
        }
    
    TWlmRogueList rogueList;
    TWlanConversionUtil::ConvertRogueList(
        rogueList,
        coreRogueList );

    TPckg<TWlmRogueList> outPckg( rogueList );
    aMessage.Write( 0, outPckg );
    aMessage.Complete(
        TWlanConversionUtil::ConvertErrorCode( ret ) );
    }

// ---------------------------------------------------------
// CWlmServer::GetRegulatoryDomain
// ---------------------------------------------------------
//
void CWlmServer::GetRegulatoryDomain(
    TUint /* aSessionId */,
    const RMessage2& aMessage )
    {
    DEBUG( "CWlmServer::GetRegulatoryDomain()" );

    TPckg<TWlanRegion> outPckg( iRegion );
    aMessage.Write( 0, outPckg );
    aMessage.Complete(
        TWlanConversionUtil::ConvertErrorCode( core_error_ok ) );
    }

// ---------------------------------------------------------
// CWlmServer::GetPowerSaveMode
// ---------------------------------------------------------
//
void CWlmServer::GetPowerSaveMode(
    TUint /* aSessionId */,
    const RMessage2& aMessage )
    {
    DEBUG( "CWlmServer::GetPowerSaveMode()" );

    TWlanPowerSave powerSaveMode( iPowerSaveMode );
    if ( !iPowerSaveEnabled )
        {
        powerSaveMode = EWlanPowerSaveNone;
        }
    
    TPckg<TWlanPowerSave> outPckg( powerSaveMode );
    aMessage.Write( 0, outPckg );
    aMessage.Complete(
        TWlanConversionUtil::ConvertErrorCode( core_error_ok ) );
    }

// ---------------------------------------------------------
// CWlmServer::AddIapSsidListL
// ---------------------------------------------------------
//
void CWlmServer::AddIapSsidListL(
    TUint /* aSessionId */,
    const RMessage2& aMessage )
    {
    DEBUG( "CWlmServer::AddIapSsidListL()" );

    TUint iapId(
        aMessage.Int0() );
    TUint ssidCount(
        aMessage.Int1() );
    TInt ssidListSize( 0 );
    if( aMessage.Ptr2() )
        {
        ssidListSize = aMessage.GetDesLengthL( 2 );
        }

    DEBUG3( "CWlmServer::AddIapSsidListL() - IAP ID: %u, SSID list count: %u, SSID list size: %u",
        iapId, ssidCount, ssidListSize );

    if( ssidCount &&
        ssidListSize )
        {
        /**
         * Make sure the IAP ID actually exists.
         */
        RArray<TWlanLimitedIapData> iapList;
        iCache->GetIapDataList( iapList );
        TBool isMatch( EFalse );
        for( TInt idx( 0 ); idx < iapList.Count() && !isMatch; ++idx )
            {
            if( iapList[idx].iapId == iapId )
                {
                isMatch = ETrue;
                }
            }
        iapList.Close();
        if( !isMatch )
            {
            DEBUG1( "CWlmServer::AddIapSsidListL() - IAP ID %u not found", iapId );

            User::Leave( KErrNotFound );
            }

        /**
         * Read the SSID list from the client side to a temporary buffer.
         */
        HBufC8* buffer = HBufC8::NewL( ssidListSize );
        CleanupStack::PushL( buffer );
    
        TPtr8 bufferPtr(
            buffer->Des() );
        aMessage.ReadL( 2, bufferPtr );

        /**
         * Copy the contents of the buffer to an CArrayFixFlat instance
         * so that they can be iterated.
         */    
        CArrayFixFlat<TWlanSsid>* ssidBuffer =
            new CArrayFixFlat<TWlanSsid>( ssidCount );
        if( !ssidBuffer )
            {
            DEBUG( "CWlmServer::AddIapSsidListL() - unable to instantiate CArrayFixFlat" );
    
            User::Leave( KErrNoMemory );
            }
        CleanupStack::PushL( ssidBuffer );
        ssidBuffer->AppendL(
            reinterpret_cast<const TWlanSsid*>( buffer->Ptr() ), ssidCount );

        CleanupStack::Pop( ssidBuffer );
        CleanupStack::PopAndDestroy( buffer );
        CleanupStack::PushL( ssidBuffer );

        /**
         * Initialize an SSID list and store SSIDs into it.
         */
        CWlanSsidList* ssidList = CWlanSsidList::NewL( ssidCount );
        CleanupStack::PushL( ssidList );
    
        for( TInt idx( 0 ); idx < ssidBuffer->Count(); ++idx )
            {
            if( ssidBuffer->At( idx ).Length() <= KWlanMaxSsidLength )
                {
                TInt ret = ssidList->AddSsid( ssidBuffer->At( idx ) );
                if( ret != KErrNone )
                    {
                    DEBUG1( "CWlmServer::AddIapSsidListL() - AddSsid failed with %d",
                        ret );
                    }

                User::LeaveIfError( ret );
                }
            }

        DEBUG2( "CWlmServer::AddIapSsidListL() - attaching a list of %u SSID(s) to IAP ID %u",
            ssidList->Count(), iapId );

        iSsidListDb->WriteListL(
            iapId,
            *ssidList );

        CleanupStack::PopAndDestroy( ssidList );
        CleanupStack::PopAndDestroy( ssidBuffer );
    
        DEBUG( "CWlmServer::AddIapSsidListL() - SSID list succesfully attached" );
        }
    else
        {
        iSsidListDb->DeleteListL(
            iapId );

        DEBUG( "CWlmServer::AddIapSsidListL() - SSID list succesfully deleted" );
        }
    
    /**
     * Invalidate cached IAP availability results since they might
     * not be valid anymore.
     */
    iCache->InvalidateAvailabilityCache();

    aMessage.Complete( KErrNone );
    }

// ---------------------------------------------------------
// CWlmServer::RemoveIapSsidListL
// ---------------------------------------------------------
//
void CWlmServer::RemoveIapSsidListL(
    TUint /* aSessionId */,
    const RMessage2& aMessage )
    {
    DEBUG( "CWlmServer::RemoveIapSsidListL()" );

    TUint iapId = aMessage.Int0();

    DEBUG1( "CWlmServer::RemoveIapSsidListL() - removing SSID list from IAP ID %u",
        iapId );

    iSsidListDb->DeleteListL(
        iapId );

    DEBUG( "CWlmServer::RemoveIapSsidListL() - SSID list succesfully removed" );

    /**
     * Invalidate cached IAP availability results since they might
     * not be valid anymore.
     */
    iCache->InvalidateAvailabilityCache();

    aMessage.Complete( KErrNone );
    }

// ---------------------------------------------------------
// CWlmServer::SetPowerSaveMode
// ---------------------------------------------------------
//
void CWlmServer::SetPowerSaveMode(
    TUint /* aSessionId */,
    const RMessage2& aMessage )
    {
    DEBUG( "CWlmServer::SetPowerSaveMode()" );

    TWlanPowerSaveMode mode = static_cast<TWlanPowerSaveMode>(
        aMessage.Int0() );
  
    DEBUG1( "CWlmServer::SetPowerSaveMode() - mode %u",
        mode );

    core_power_save_mode_s coreMode;
    TWlanConversionUtil::ConvertPowerSaveMode(
        coreMode,
        mode );

    iCoreServer->set_power_save_mode( coreMode );

    aMessage.Complete( KErrNone );
    }

// ---------------------------------------------------------
// CWlmServer::GetAcTrafficStatus
// ---------------------------------------------------------
//
void CWlmServer::GetAcTrafficStatus(
    TUint /* aSessionId */,
    const RMessage2& aMessage )
    {
    DEBUG( "CWlmServer::GetAcTrafficStatus()" );

    core_ac_traffic_information_s coreInfo;
    core_error_e ret = iCoreServer->get_current_ac_traffic_info( coreInfo );
    if( ret != core_error_ok )
        {
        DEBUG1( "CWlmServer::GetAcTrafficStatus() - get_current_ac_traffic_info() failed with %u",
            ret );

        aMessage.Complete( 
            TWlanConversionUtil::ConvertErrorCode( ret ) );        
        }

    TWlmAcTrafficStatusArray info;
    TPckg<TWlmAcTrafficStatusArray> outPckg( info );
    info[EWlmAccessClassBestEffort] = TWlanConversionUtil::ConvertTrafficStatus(
        coreInfo.status_for_best_effort );
    info[EWlmAccessClassBackground] = TWlanConversionUtil::ConvertTrafficStatus(
        coreInfo.status_for_background );
    info[EWlmAccessClassVideo] = TWlanConversionUtil::ConvertTrafficStatus(
        coreInfo.status_for_video );
    info[EWlmAccessClassVoice] = TWlanConversionUtil::ConvertTrafficStatus(
        coreInfo.status_for_voice );

    aMessage.Write( 0, outPckg );
    aMessage.Complete( KErrNone );
    }

// ---------------------------------------------------------
// CWlmServer::DirectedRoam
// ---------------------------------------------------------
//
void CWlmServer::DirectedRoam(
    TUint aSessionId,
    const RMessage2& aMessage )
    {
    DEBUG( "CWlmServer::DirectedRoam()" );

    TPckgBuf<TMacAddress> bssidPckg;
    TInt ret( aMessage.Read( 0, bssidPckg ) );
    if( ret != KErrNone )
        {
        aMessage.Complete( ret );
        return;
        }

    core_mac_address_s coreBssid( ZERO_MAC_ADDR );
    TWlanConversionUtil::ConvertMacAddress(
        coreBssid,
        bssidPckg() );

    // create mapping
    SRequestMapEntry mapEntry;
    mapEntry.iMessage = aMessage;
    mapEntry.iFunction = EDirectedRoam;
    mapEntry.iRequestId = iRequestIdCounter++;
    mapEntry.iSessionId = aSessionId;
    iRequestMap.Append( mapEntry );

    // pass request to core
    iCoreServer->directed_roam(
        mapEntry.iRequestId,
        coreBssid );
    }

// ---------------------------------------------------------
// CWlmServer::CancelDirectedRoam
// ---------------------------------------------------------
//
void CWlmServer::CancelDirectedRoam(
    TUint aSessionId,
    const RMessage2& aMessage )
    {
    DEBUG( "CWlmServer::CancelDirectedRoam()" );

    CancelExternalRequest(
        aSessionId,
        EDirectedRoam );

    aMessage.Complete( KErrNone );    
    }

// ---------------------------------------------------------
// CWlmServer::StartupComplete
// ---------------------------------------------------------
//
void CWlmServer::StartupComplete()
    {
    iIsStartupComplete = ETrue;
    
    SetIconState( EWlmIconStatusNotAvailable );
    
    iCoreServer->enable_wlan( KWlanIntCmdEnableWlan );

    /**
     * Read the CommsDat data again so that we can be sure
     * that it is up to date after reboot. It can happen that
     * when WLAN engine boots up commsdat is not up to date yet.
     */
    UpdateWlanSettings();

    // If WLAN is set ON, enable background scanning
    if( iPlatform->GetWlanOnOffState() == EWlanOn )
        {
        iBgScanProvider->SetBgScanState( MWlanBgScanProvider::EWlanBgScanOn );
        }

    iPlatform->InitializeSystemTimeHandler();
    }


// ---------------------------------------------------------
// CWlmServer::EmergencyCallComplete
// ---------------------------------------------------------
//
void CWlmServer::EmergencyCallComplete( TBool aStartupCompleted )
    {
    if ( !aStartupCompleted )
        {
        iCoreServer->disable_wlan( KWlanIntCmdDisableWlan );
        }
    else
        {
        if ( iIsStartupComplete == EFalse )
            {
            iIsStartupComplete = ETrue; 
            
            UpdateWlanSettings();
            
            iPlatform->InitializeSystemTimeHandler();
            }
        }              
    }

// ---------------------------------------------------------
// CWlmServer::HandleSsidListAvailabilityL
// ---------------------------------------------------------
//
void CWlmServer::HandleSsidListAvailabilityL(
    const SRequestMapEntry& aMapEntry )
    {   
    DEBUG( "CWlmServer::HandleSsidListAvailabilityL()" );

    ScanList* scanList =
        reinterpret_cast<ScanList*>( aMapEntry.iParam2 );
    core_type_list_c<core_iap_availability_data_s>* iapAvailabilityList = 
        reinterpret_cast<core_type_list_c<core_iap_availability_data_s>*>( aMapEntry.iParam1 );

    /**
     * Go through the IAP list and find IAPs that haven't been found
     * during GetAvailableIaps and have an SSID list attached.  
     */
    RArray<TWlanLimitedIapData> attachedIapList;
    CleanupClosePushL( attachedIapList );
    const RArray<TWlanLimitedIapData>& cachedIapList(
        iCache->CachedIapDataList() );
    core_type_list_iterator_c<core_iap_availability_data_s> iter( *iapAvailabilityList );    
    for( TInt idx( 0 ); idx < cachedIapList.Count(); ++idx )
        {
        TBool isFound( EFalse );
        for( core_iap_availability_data_s* item = iter.first(); !isFound && item; item = iter.next() )
            {
            if( item->id == cachedIapList[idx].iapId )
                {                
                isFound = ETrue;
                }
            }
        if( !isFound )
            {
            DEBUG1( "CWlmServer::HandleSsidListAvailabilityL() - IAP ID %u isn't available",
                cachedIapList[idx].iapId );
            if( iSsidListDb->IsListAttached(
                cachedIapList[idx].iapId ) )
                {
                DEBUG1( "CWlmServer::HandleSsidListAvailabilityL() - IAP %u has an SSID list attached",
                    cachedIapList[idx].iapId );
                attachedIapList.Append( cachedIapList[idx] ); 
                }
            }
        }

    if( !attachedIapList.Count() )
        {
        DEBUG( "CWlmServer::HandleSsidListAvailabilityL() - no IAPs with an list attached, nothing to do" );

        CleanupStack::PopAndDestroy( &attachedIapList );

        return;
        }

    /**
     * Generate a list of SSIDs detected during the scan and go through
     * the IAPs with lists attached again.
     */
    RArray<TWlanAvailableNetwork> networkList;
    CleanupClosePushL( networkList );
    GetNetworkList( *scanList, networkList );

    for( TInt idx( 0 ); idx < attachedIapList.Count(); ++idx )
        {
        DEBUG1( "CWlmServer::HandleSsidListAvailabilityL() - requesting SSID list for IAP %u",
            attachedIapList[idx].iapId );

        CWlanSsidList* ssidList = CWlanSsidList::NewL( KWlanSsidListGranularity );
        CleanupStack::PushL( ssidList );
        iSsidListDb->ReadListL(
            attachedIapList[idx].iapId,
            *ssidList );
        TBool isMatch( EFalse );
        TUint rcpi( 0 );
        for( TInt iidx( 0 ); !isMatch && iidx < networkList.Count(); ++iidx )
            {
            if( attachedIapList[idx].networkType == networkList[iidx].networkType &&
                ssidList->IsInList( networkList[iidx].ssid ) )
                {
                rcpi = networkList[iidx].rcpi;
                isMatch = ETrue;
                }
            }

        if( isMatch )
            {
            DEBUG( "CWlmServer::HandleSsidListAvailabilityL() - matching SSID found" );
                
            /**
             * A match has been found, mark the IAP as available.
             */
            core_iap_availability_data_s* data = new (ELeave) core_iap_availability_data_s;
            data->id = attachedIapList[idx].iapId;
            data->rcpi = rcpi;
            core_error_e ret = iapAvailabilityList->append(
                data );
            if( ret != core_error_ok )
                {
                delete data;
                }
            data = NULL;
            }
        else
            {
            DEBUG( "CWlmServer::HandleSsidListAvailabilityL() - no matching SSIDs found for IAP" );
            }

        CleanupStack::PopAndDestroy( ssidList );
        }

    CleanupStack::PopAndDestroy( &networkList );        
    CleanupStack::PopAndDestroy( &attachedIapList );

    DEBUG( "CWlmServer::HandleSsidListAvailabilityL() - all done" );
    }

// ---------------------------------------------------------
// CWlmServer::GetCurrentIapId
// ---------------------------------------------------------
//
TInt CWlmServer::GetCurrentIapId( 
    const TUint aLanServiceId,
    core_iap_data_s& aCoreIapData  )
    {
    DEBUG( "CWlmServer::GetCurrentIapId()" );        
    /**
     * Read all WLAN IAPs from commsdat.
     */
    CWLanSettings wlanSettings;    
    TInt ret = wlanSettings.Connect();
    if ( ret != KErrNone )
        {
        DEBUG1( "CWlmServer::GetCurrentIapId - CWLanSettings::Connect() failed (%d)",
                ret );

        return ret;
        }

    RArray<SWlanIAPId> wlanIapIds;
    TRAP( ret, wlanSettings.GetIAPWlanServicesL( wlanIapIds ) );
    if( ret != KErrNone )
        {
        DEBUG1( "CWlmServer::GetCurrentIapId - CWLanSettings::GetIAPWlanServicesL() failed (%d)",
                ret );
        wlanIapIds.Close();
        wlanSettings.Disconnect();

        return ret;
        }
    TInt IapsCount = wlanIapIds.Count();
    /**
     * Get the matching IAP id.
     */ 
    for ( TInt i = 0; i < IapsCount; i++ )
        {
        if ( wlanIapIds[i].iWLANRecordId == aLanServiceId )
            {
            aCoreIapData.iap_id = wlanIapIds[i].iIAPId;
            i = IapsCount; 
            }
        }
    
    wlanIapIds.Close();
    wlanSettings.Disconnect();
    
    DEBUG( "CWlmServer::GetCurrentIapId() - all done" );   
    
    return KErrNone;
    }
    
// ---------------------------------------------------------
// CWlmServer::PublishBgScanInterval
// ---------------------------------------------------------
//
void CWlmServer::PublishBgScanInterval( TUint32& aInterval )
    {
    DEBUG1( "CWlmServer::PublishWlanBgScanInterval( %u )", aInterval );
    
    iPlatform->PublishBgScanInterval( aInterval );
    }
    
// ---------------------------------------------------------
// CWlmServer::WlanOn
// ---------------------------------------------------------
//
void CWlmServer::WlanOn()
    {
	DEBUG( "CWlmServer::WlanOn()" );
	
	// BG scan can be set ON only after BG scan interval has been
	// set and that does not happen until startup has been
	// completed.
	if( iIsStartupComplete )
	    {
	    // Enable background scanning
	    iBgScanProvider->SetBgScanState( MWlanBgScanProvider::EWlanBgScanOn );
	    }
	}

// ---------------------------------------------------------
// CWlmServer::WlanOff
// ---------------------------------------------------------
//
void CWlmServer::WlanOff()
    {
	DEBUG1( "CWlmServer::WlanOff() - ConnectionState=%d",
	    iConnectionState );
	
	// Disable background scanning
	iBgScanProvider->SetBgScanState( MWlanBgScanProvider::EWlanBgScanOff );
	
	// Cancel all running operations that are forbidden in WLAN OFF
    CancelExternalRequestsByType( ERunProtectedSetup );
    CancelExternalRequestsByType( EJoinByProfileId );
    
	// If WLAN is connected...
    if( iConnectionState != EWlanStateNotConnected )
        {
        //...send out disconnected indication, which brings down
        // the connection gracefully
        TBuf8<KMaxNotificationLength> buf;
        buf.Append( static_cast<u8_t>( EWlanStateNotConnected ) );
        SendNotification( EWlmNotifyConnectionStateChanged, buf );
        }
	}
    
// ---------------------------------------------------------
// CWlmServer::CancelRequestsByType
// ---------------------------------------------------------
//
void CWlmServer::CancelExternalRequestsByType(
    const TWLMCommands aCommand )
    {
    DEBUG1( "CWlmServer::CancelExternalRequestsByType( aCommand=%u )", aCommand );

    // Go through request map and cancel the command given as a parameter
    for( TInt i( 0 ); i < iRequestMap.Count(); i++ )
        {
        if( iRequestMap[i].iFunction == aCommand )
            {
            DEBUG1( "CWlmServer::CancelExternalRequestsByType() - request found (id=%u), cancelling",
                iRequestMap[i].iRequestId );
            
            iCoreServer->cancel_request( iRequestMap[i].iRequestId );
            }
        }
    }

// ---------------------------------------------------------
// CWlmServer::SendNotification
// ---------------------------------------------------------
//
void CWlmServer::SendNotification(
    TWlmNotify amNotification,
    TBuf8<KMaxNotificationLength>& aParams )
    {
    DEBUG1( "CWlmServer::SendNotification( notification=%u )",
        amNotification );
    
    // Notify subscribees
    for ( TInt i = 0; i < iNotificationArray.Count(); i++ )
        {
        iNotificationArray[i]->AddNotification( amNotification, aParams );
        }
    }
 
