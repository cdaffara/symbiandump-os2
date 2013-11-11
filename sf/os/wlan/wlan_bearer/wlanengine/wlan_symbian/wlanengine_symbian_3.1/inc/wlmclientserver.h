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
* Description:  Data structures for client server communication in WLMServer.
*
*/

/*
* %version: 43 %
*/

#ifndef WLMCLIENTSERVER_H
#define WLMCLIENTSERVER_H

#include "umacoidmsg.h"
#include <wlanmgmtcommon.h>
#include <wlanerrorcodes.h>
#include <wdbifwlansettings.h>

_LIT(KWLMServerExe,         "Z:\\system\\libs\\WLMSERVEREXE");
_LIT(KWLMServerSemaphore,   "WLMServerSemaphore");
_LIT(KWLMDataServerName,    "WlanServer");

/** Version number */
const TUint KWLMServMajorVersionNumber = 1;
/** Release number */
const TUint KWLMServMinorVersionNumber = 0;
/** Build number */
const TUint KWLMServBuildVersionNumber = 1;

const TUint KMaxWepKeyStrLength     = 32;    // WEP max 128 bits -> 16 bytes -> 32 chars
const TUint KMaxDBFieldLength       = 50;
const TUint KMaxNotificationLength  = 128;   // Max data length for notification data.

/**
 * The maximum number of available IAPs returned.
 */
const TUint KWlmMaxAvailableIaps    = 64;

/**
 * The maximum number of association failures before the AP is blacklisted.
 */
const TUint KWlmMaxApFailureCount   = 5;

/**
 * The maximum number of Protected Setup credentials.
 */
const TUint KWlmProtectedSetupMaxCount = 8;

/**
 * The maximum number of BSSIDs in the rogue list.
 */
const TUint KWlmRogueListMaxCount = 10;

/**
 * The maximum size of the scan list is 50kB.
 */
const TUint KWlmScanListMaxSize = 51200;

// DATA TYPES

/**
* Data values for connection states.
*/
enum TWlanConnectionState
    {
    /** Connection is not active. No data flow. */
    EWlanStateNotConnected,
    /** Connection to access point is active. */
    EWlanStateInfrastructure,
    /** Searching access point. No data flow. */
    EWlanStateSearchingAP,
    /** AdHoc network is active. */
    EWlanStateIBSS,
    /** HW/SW/PC testing mode active. Testing only. */
    EWlanStateSecureInfra,
    /** Unknown state. */
    EConnectionStateUnknown
    };

/**
* Data values for security modes.
*/
enum TWlanSecurity
    {
    EWlanSecurityOpen,
    EWlanSecurityWepOpen,
    EWlanSecurityWepShared,
    EWlanSecurity802d1x,
    EWlanSecurityWpa,
    EWlanSecurityWpaPsk,
    EWlanSecurityWpa2,
    EWlanSecurityWpa2Psk,
    EWlanSecurityWapi,
    EWlanSecurityWapiPsk,    
    };

/**
* Data values for system modes.
*/
enum TWlanSystemMode
    {
    EWlanSystemNormal,
    EWlanSystemFlight,
    EWlanSystemDisabled,
    EWlanSystemStartupInProgress
    };

/**
* Data values for RSS classes.
* These has to be in synch with TWlanRssClass and 
* core_rss_level
*/
enum TWlanRCPLevel
    {
    EWlanRcpNormal,
    EWlanRcpWeak
    };

/**
* Commands from client to server.
*/
enum TWLMCommands
    {
    /** Ask server to send notifications to this session. */
    EOrderNotifications,
    /** Cancel server to stop sending notifications to this session. */
    ECancelNotifications,
    /** Get results of last access point scan. */
    EGetScanResults,
    /** Cancel an outstanding scan request. */
    ECancelGetScanResults,
    /** Join to a access point / ad hoc network by Profile id. */
    EJoinByProfileId,
    /** Cancel an outstanding join request. */
    ECancelJoin,
    /** Start an ad hoc nw / join to an ad hoc nw. */
    EStartIBSS,
    /** Reset connection. */
    EReset,
    /** Get available WLAN IAPs. */
    EGetAvailableIaps,
    /** Cancel an outstanding IAP availability request. */
    ECancelGetAvailableIaps,
    /** Get current state of the connection. */
    EGetCurrentState,
    /** Get current signal strength, 0 if no connection. */
    EGetCurrentRSSI,
    /** Get the MAC address of AP/STA that is being connected to. */
    EGetMacAddress,
    /** Get the name of the current network. */
    EGetNetworkName,
    /** Get the current security mode. */
    EGetSecurityMode,
    /** Get the current system mode. */
    EGetSystemMode,
    /** Notify the server about changed settings. */
    ENotifyChangedSettings,
    /** Add BSSID to roguelist (blacklist) */
    EAddBssidToRoguelist,
    /** Update RCP notification boundaries */
    EUpdateRcpNotificationBoundaries,
    /** Configure multicast address */
    EConfigureMulticastGroup,
    /** Get packet statistics of the current connection. */
    EGetPacketStatistics,
    /** Clear packet statistics of the current connection. */
    EClearPacketStatistics,
    /** Get the current U-APSD settings. */
    EGetUapsdSettings,
    /** Set the U-APSD settings. */
    ESetUapsdSettings,
    /** Get the current power save settings. */
    EGetPowerSaveSettings,
    /** Set the power save settings. */
    ESetPowerSaveSettings,
    /** Run Protected setup */
    ERunProtectedSetup,
    /** Cancel Protected setup */
    ECancelProtectedSetup,
    /** Request creation of a traffic stream. */
    ECreateTrafficStream,
    /** Cancel a pending traffic stream creation request. */
    ECancelCreateTrafficStream,
    /** Request deletion of a traffic stream. */
    EDeleteTrafficStream,
    /** Cancel a pending traffic stream deletion request. */
    ECancelDeleteTrafficStream,
    /** Get information about the current AP. */
    EGetAccessPointInfo,
    /** Get roam metrics of the current connection. */
    EGetRoamMetrics,
    /** Get a list of BSSIDs on the rogue list. */
    EGetRogueList,
    /** Get the current regulatory domain. */
    EGetRegulatoryDomain,
    /** Get the current power save mode. */
    EGetPowerSaveMode,
    /** Add a list of SSIDs to an IAP. */
    EAddIapSsidList,
    /** Remove any list of SSIDs attached to an IAP. */ 
    ERemoveIapSsidList,
    /** Set the power save mode. */
    ESetPowerSaveMode,
    /** Notify the server about changed PSM server mode. */
    ENotifyChangedPsmSrvMode,
    /** Get the current traffic status for access classes. */
    EGetAcTrafficStatus,
    /** Initiate a roam to the given BSSID. */
    EDirectedRoam,
    /** Cancel a pending directed roam request. */
    ECancelDirectedRoam,
    /** Request entry to aggressive background scan mode. */
    EStartAggressiveBgScan
    };

/**
* Nofications from server to client.
*/
enum TWlmNotify
    {
    EWlmNotifyNone                          = 0x0000,
    EWlmNotifyConnectionStateChanged        = 0x0001,
    EWlmNotifyBssidChanged                  = 0x0002,
    EWlmNotifyBssLost                       = 0x0004,
    EWlmNotifyBssRegained                   = 0x0008,
    EWlmNotifyNewNetworksDetected           = 0x0010,
    EWlmNotifyOldNetworksLost               = 0x0020,
    EWlmNotifyTransmitPowerChanged          = 0x0040,
    EWlmNotifyNotificationsCancelled        = 0x0080,
    EWlmNotifyRcpChanged                    = 0x0100,
    EWlmNotifyTrafficStreamStatusChanged    = 0x0200,
    EWlmNotifyAccessPointInfoChanged        = 0x0400,
    EWlmNotifyRcpiRoamAttemptStarted        = 0x0800,
    EWlmNotifyRcpiRoamAttemptCompleted      = 0x1000,
    EWlmNotifyAcTrafficModeChanged          = 0x2000,
    EWlmNotifyAcTrafficStatusChanged        = 0x4000
    };

/**
 * Default notifications for callback API v1.
 */
const TUint32 KWlmDefaultNotificationsV1 =
    EWlmNotifyConnectionStateChanged |
    EWlmNotifyBssidChanged |
    EWlmNotifyBssLost |
    EWlmNotifyBssRegained |
    EWlmNotifyNewNetworksDetected |
    EWlmNotifyOldNetworksLost |
    EWlmNotifyTransmitPowerChanged |
    EWlmNotifyNotificationsCancelled |
    EWlmNotifyRcpChanged;

/**
 * Default notifications for callback API v2.
 */
const TUint32 KWlmDefaultNotificationsV2 =
    KWlmDefaultNotificationsV1 |
    EWlmNotifyTrafficStreamStatusChanged;

/**
* Notifications' data.
*/
struct TWlmNotifyData
    {
    TBuf8<KMaxNotificationLength> data;
    };

/**
* Bit mask for overrided settings.
*/
enum TOverrideSettingsMask
    {
    EOverrideNoneMask   = 0x00000000,
    EOverrideSsidMask   = 0x00000001,
    EOverrideBssidMask  = 0x00000002,
	EOverrideWepMask	= 0x00000004,
	EOverrideWpaPskMask = 0x00000008,
	EOverrideWpaMask	= 0x00000010, /** Use WPA handshake, if bit defined. */
	EOverrideIbssMask	= 0x00000020  /** Use IBSS mode, if bit defined. */
    };

/**
 * The possible statuses of a traffic stream.
 */
enum TWlmTrafficStreamStatus
    {
    /**
     * The traffic stream has been successfully created.
     */
    EWlmTrafficStreamStatusActive,
    /**
     * The traffic stream has either been deleted by the network or
     * the new AP after roaming no longer requires admission control.
     */      
    EWlmTrafficStreamStatusNotActive,
    /**
     * The current AP has rejected our request to create a traffic
     * stream. Stream creation must not be retried until we have
     * roamed to a new AP.
     */
    EWlmTrafficStreamStatusRejected
    };

/**
 * The possible statuses of a PSM mode.
 */
enum TWlanPsmMode
    {
    /**
     * Normal PSM power save mode.
     */
    EWlmPsmModeNormal = 0,
    /**
     * Full PSM power save mode.
     */      
    EWlmPsmModePowerSave,
    /**
     * Partial PSM power save mode.
     */
    EWlmPsmPartialMode
    };

/**
 * The possible access classes.
 */
enum TWlmAccessClass
    {
    EWlmAccessClassBestEffort = 0,
    EWlmAccessClassBackground = 1,
    EWlmAccessClassVideo = 2,
    EWlmAccessClassVoice = 3,
    EWlmAccessClassMax = 4
    };

/**
 * Definitions for possible traffic modes for an access class.
 */
enum TWlmAcTrafficMode
    {
    /** Automatic traffic stream creation is allowed. */    
    EWlmAcTrafficModeAutomatic,
    /** Automatic traffic stream creation is NOT allowed. */
    EWlmAcTrafficModeManual
    };

/**
 * Definitions for possible traffic statuses for an access class.
 */
enum TWlmAcTrafficStatus
    {
    /**
     * Traffic for this access class has been admitted.
     */
    EWlmAcTrafficStatusAdmitted,
    /** 
     * Traffic for this access class has NOT been admitted,
     * traffic needs to be downgraded to a lower access class.
     */
    EWlmAcTrafficStatusNotAdmitted
    };

typedef TFixedArray<TWlmAcTrafficStatus, EWlmAccessClassMax> TWlmAcTrafficStatusArray;

/**
* Data structure for join overrides.
*/
struct TWLMOverrideSettings
    {
    /** See TOverrideSettingsMask */
    TUint32 settingsMask;
    /** Override SSID. Used in easy connection. */
    TSSID ssid;
    /** Override BSSID. Used in test cases only. */
    TMacAddress bssid;
	/** Override WEP key. Used in easy connection. */
	TWep wep;
	/** Override WPA Pre-Shared Key. Used in easy connection. */
	TWpaPsk wpaPsk;
    };

/**
* Data structure sending TMacAddress as a TPckg.
*/
struct TMacPckg
    {
    TMacAddress data;
    };

/**
 * Data structure for storing IAP availability data.
 */
struct TWlmAvailabilityData
    {
    TUint iapId;
    TUint rcpi;
    };

/**
* Data structure for storing available IAP IDs.
*/  
struct TWlmAvailableIaps
    {
    /** The number of IAPs available. */
    TUint32 count;

    /** The available IAP IDs. */
    TFixedArray<TWlmAvailabilityData,KWlmMaxAvailableIaps> iaps;
    };

/**
 * Structure for storing traffic stream parameters.
 */
struct TWlmTrafficStreamParams
    {
    /**
     * Whether the traffic pattern is periodic or aperiodic.
     */
    TBool isPeriodicTraffic;     
    /**
     * The current value of Nominal MSDU Size.
     */
    TUint16 nomimalMsduSize;
    /**
     * The current value of Maximum MSDU Size.
     */
    TUint16 maximumMsduSize;
    /**
     * The current value of Minimum Service Interval.
     */        
    TUint32 minimumServiceInterval;
    /**
     * The current value of Maximum Service Interval.
     */   
    TUint32 maximumServiceInterval;
    /**
     * The current value of Inactivity Interval.
     */    
    TUint32 inactivityInterval;
    /**
     * The current value of Suspension Interval.
     */    
    TUint32 suspensionInterval;
    /**
     * The current value of Service Start Time.
     */    
    TUint32 serviceStartTime;
    /**
     * The current value of Minimum Data Rate.
     */
    TUint32 minimumDataRate;
    /**
     * The current value of Mean Data Rate.
     */    
    TUint32 meanDataRate;
    /**
     * The current value of Peak Data Rate.
     */    
    TUint32 peakDataRate;
    /**
     * The current value of Maximum Burst Size.
     */    
    TUint32 maximumBurstSize;
    /**
     * The current value of Delay Bound
     */    
    TUint32 delayBound;
    /**
     * The current value of Minimum PHY Rate.
     */    
    TUint32 minimumPhyRate;
    /**
     * The current value of Bandwidth Allowance.
     */    
    TUint16 surplusBandwithAllowance;
    /**
     * The current value of Medium Time.
     */    
    TUint16 mediumTime;
    };


/**
 * Data structure for storing available Protected setup credentials.
 */  
struct TWlmProtectedSetupCredentials
    {
    /** The number of IAPs available. */
    TUint32 count;

    /** The available IAP IDs. */
    TFixedArray<TWlanProtectedSetupCredentialAttribute, KWlmProtectedSetupMaxCount> credentials;
    };

/**
 * Data structure for storing a list of BSSIDs on the rogue list.
 */
struct TWlmRogueList
    {
    /** The number of BSSIDs on the rogue list. */
    TUint32 count;

    /** BSSID list. */
    TFixedArray<TWlanBssid, KWlmRogueListMaxCount> list;
    };


/**
 * Structure for storing limited information about an IAP.
 */
struct TWlanLimitedIapData
    {
    TUint iapId;
    TUint serviceId;
    TBuf8<KMaxSSIDLength> ssid;
    TBuf8<KMaxSSIDLength> usedSsid;
    EConnectionMode networkType;
    EWlanSecurityMode securityMode;
    TBool isPskEnabled;
    TBool isHidden;
    };    

/**
 * Data structure for storing dynamic scan list parameters.
 */
struct TDynamicScanList
    {
    TUint32 count;
    TUint32 size;
    };

/**
 * Data structure for storing scan scheduling parameters.
 */
struct TScanScheduling
    {
    TInt cacheLifetime;
    TUint maxDelay;
    };

#endif // WLMCLIENTSERVER_H
