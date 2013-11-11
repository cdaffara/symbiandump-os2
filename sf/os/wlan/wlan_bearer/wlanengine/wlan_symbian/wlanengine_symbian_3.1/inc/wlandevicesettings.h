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
* Description:  Interface to read and write WLAN device specific settings.
*
*/

/*
* %version: 24 %
*/

#ifndef WLANDEVICESETTINGS_H
#define WLANDEVICESETTINGS_H

#include <e32base.h>
#include "umacoidmsg.h"

const TUint KMaxBssidStrLength  = 12;   // BSS ID = MAC address
const TUint KMaxSsidStrLength   = 64;   // SSID is 32 bytes -> 64 characters.
const TUint KMaxWepStrLength    = 50;

// Default values for some settings
const TUint32 KWlanDefaultBeacon = 100;                         // 100 kusec
const TUint32 KWlanDefaultBackgroundScanInterval = 0;           // No background scan
const TUint32 KWlanDefaultTxPowerLevel = 100;                   // 100mW
const TUint32 KWlanDefaultRcpiTrigger = 70;                     // 70 dB
const TUint32 KWlanDefaultSimultaneousBtUsageAllowed = 0;       // 0=Enabled 1=Not enabled
const TUint32 KWlanDefaultScanExpirationTimer = 10;             // 10 seconds
const TUint32 KWlanDefaultUnloadDriverTimer = 5;                // 5 seconds
const TUint32 KWlanDefaultRoamTimer = 7000000;                  // 7 seconds
const TUint32 KWlanDefaultRcpiDifference = 12;                  // 12 dB
const TUint32 KWlanDefaultConnRegainTimer = 10000000;           // 10 seconds.
const TUint32 KWlanDefaultMaxTriesToFindNw = 5;                 // 5 times
const TUint32 KWlanDefaultDelayBetweenFindNw = 1500000;         // 1,5 seconds
const TUint32 KWlanDefaultowerMode = 0;                         // 0 == enabled
const TUint32 KWlanDefaultLongRetryLimit = 4;
const TUint32 KWlanDefaultRTSThreshold = 2347;
const TUint32 KWlanDefaultShortRetryLimit = 7;
const TUint32 KWlanDefaultMinChanneltime = 10;
const TUint32 KWlanDefaultMaxChanneltime = 30;
const TUint32 KWlanDefaultMaxTransmitMSDULifetime = 512;
const TUint32 KWlanDefaultMinPassiveChannelTime = 110;
const TUint32 KWlanDefaultMaxPassiveChannelTime = 110;
const TUint32 KWlanDefaultMaxApFailureCount = 5;
const TUint32 KWlanDefaultMaxApAuthFailureCount = 1;
const TUint32 KWlanDefaultLongBeaconFindCount = 3;
const TUint32 KWlanDefaultQosNullFrameInterval = 20000;         // 20 ms
const TUint32 KWlanDefaultQosNullFrameTimeout = 1000000;        // 1 second
const TUint32 KWlanDefaultKeepAliveInterval = 200000000;        // 200 seconds         
const TUint32 KWlanDefaultScanStopRcpiThreshold = 80;      
const TUint32 KWlanDefaultMinRcpiForIapAvailability = 50;  
const TUint32 KWlanDefaultQoSNullFrameEntryTimeout = 300000;    // 300 ms   
const TUint32 KWlanDefaultMaxApDeauthenticationCount = 5; 
const TUint32 KWlanDefaultApDeauthenticationTimeout = 2000000;  // 2 seconds  
const TUint32 KWlanDefaultRegion = 0;                           // not available                
const TInt32  KWlanDefaultRegionTimestamp = 0;                  // not available       
const TUint32 KWlanDefaultMaxDtimSkipInterval = 0;              // wake on every DTIM
const TUint32 KWlanDefaultPsActiveToLightTimeout = 100000;      // 100ms
const TUint32 KWlanDefaultPsActiveToLightThreshold = 1;         // 1 frame
const TUint32 KWlanDefaultPsLightToActiveTimeout = 300000;      // 300ms
const TUint32 KWlanDefaultPsLightToActiveThreshold = 4;         // 4 frames
const TUint32 KWlanDefaultPsLightToDeepTimeout = 1000000;       // 1 second
const TUint32 KWlanDefaultPsLightToDeepThreshold = 1;           // 1 frame
const TUint32 KWlanDefaultPsUapsdRxFrameLengthThreshold = 400;  // 400 bytes
const TUint32 KWlanDefaultRcpiRoamMinInterval = 4000000;        // 4 seconds
const TUint32 KWlanDefaultRcpiRoamMaxInterval = 60000000;       // 1 minute
const TUint32 KWlanDefaultRcpiRoamAttemptsPerInterval = 2;
const TUint32 KWlanDefaultRcpiRoamNextIntervalFactor = 2;
const TUint32 KWlanDefaultRcpiRoamNextIntervalAddition = 0;
const TUint32 KWlanDefaultScanListExpirationTime = 120000000;   // 2 minutes
const TUint32 KWlanDefaultQoSNullFrameEntryTxCount = 5;
const TUint32 KWlanDefaultSpRcpiTarget = 65;
const TUint32 KWlanDefaultSpTimeTarget = 900000;                // 900 ms
const TUint32 KWlanDefaultSpMinIndicationInterval = 3000000;    // 3 seconds  
const TUint32 KWlanDefaultBssLostRoamMinInterval = 1000000;     // 1 second
const TUint32 KWlanDefaultBssLostRoamMaxInterval = 1500000;     // 1,5 seconds
const TUint32 KWlanDefaultBssLostRoamAttemptsPerInterval = 2;
const TUint32 KWlanDefaultBssLostRoamNextIntervalFactor = 1;
const TUint32 KWlanDefaultBssLostRoamNextIntervalAddition = 500000; // 500 ms
const TUint32 KWlanDefaultBssLostRoamMaxTriesToFindNw = 5;
const TUint32 KWlanDefaultTrafficStreamCreationTimeout = 1000000;   // 1 second
const TUint32 KWlanDefaultBeaconLostThreshold = 15;
const TUint32 KWlanDefaultBtBeaconLostThreshold = 20;
const TUint32 KWlanDefaultTxFailThreshold = 4;
const TUint32 KWlanDefaultBtTxFailThreshold = 4;
const TUint32 KWlanDefaultPowerSaveDelay = 7;                   // 7 seconds
const TUint32 KWlanDefaultRegionExpirationTime = 18000;         // 5 hours
const TUint32 KWlanDefaultRrmMinMeasurementInterval = 45000000; // 45 seconds 
const TUint32 KWlanDefaultPsmServerMode = 0;                    // PSM server is in normal mode
const TUint32 KWlanDefaultBgScanPeakPeriodStart = 600;          // Peak starts at 06:00 o'clock
const TUint32 KWlanDefaultBgScanPeakPeriodEnd = 100;            // Peak ends at 01:00 o'clock
const TUint32 KWlanDefaultBgScanIntervalPeakPeriod = 600;       // Background scan interval for peak hours is 600 s
const TUint32 KWlanDefaultBgScanIntervalOffPeakPeriod = 1200;   // Background scan interval for off-peak hours is 1200 s
const TUint32 KWlanDefaultMinActiveBroadcastChannelTime = 10;
const TUint32 KWlanDefaultMaxActiveBroadcastChannelTime = 110;
const TUint32 KWlanDefaultEnabledFeatures = 0xFFFFFFFF;

/**
 * Bit definitions for run-time WLAN features.
 */
enum TWlanRunTimeFeature
    {
    /** Initialisation value. */
    EWlanRunTimeFeatureNone                = 0x00000000,
    /** Whether 802.11n is enabled. */
    EWlanRunTimeFeature802dot11n           = 0x00000001,
    };

// The value of backgroundScanInterval to deny periodic scanning
const TUint KScanIntervalNever = 0;

class CCommsDatabase;
class CCommsDbTableView;

/**
*  Interface to read and write WLAN device specific settings.
*  The data is stored in CommsDB.
*  @since Series 60 3.0
*  @lib wlandevicesettings.lib
*/
NONSHARABLE_CLASS( CWlanDeviceSettings ) : public CBase
    {
public:

    struct SWlanDeviceSettings
        {
        TUint32 beacon;                             ///< In ad hoc beacon interval in Kusec.
        TUint32 longRetry;                          ///< How many times packets bigger than 'rts' are been resent.
        TUint32 rts;                                ///< Min. size of a packet that CTS/RTS handshake is been used.
        TUint32 shortRetry;                         ///< How many times packets smaller than 'rts' are been resent.
        TUint32 backgroundScanInterval;             ///< Is WLAN indication icon been showed in UI.
        TUint32 txPowerLevel;                       ///< Transmission power level in use. In mWs.
        TRate scanRate;                             ///< Data rate used in probe request.
        TUint32 rcpiTrigger;                        ///< Default value for RSSI trigger.
        TUint32 minActiveChannelTime;               ///< Min time (TUs) to listen a channel in active direct scanning / during an active connection.
        TUint32 maxActiveChannelTime;               ///< Max time (TUs) to listen a channel in active direct scanning / during an active connection.
        TUint32 maxTxMSDULifeTime;                  ///< Max time to send one (fragmented) packet.
        TBool useDefaultSettings;                   ///< If ETrue default values are being used, EFalse use the user defined values.
        TUint32 scanExpirationTimer;                ///< Time after a new scan is done if required. Otherwise the last scan result is returned.
        TUint32 unloadDriverTimer;                  ///< Time to wait before unload WLAN drivers if no WLAN connection.
        TUint32 roamTimer;                          ///< Time of how often the roaming conditions is been checked (in microseconds).
        TUint32 rcpiDifference;                     ///< Difference of current and the best connections.
        TUint32 connRegainTimer;                    ///< Time to wait connection regain before start roaming sequence (in microseconds).
        TUint32 maxTriesToFindNw;                   ///< How many times a nw is being tried to scan before give up.
        TUint32 delayBetweenFindNw;                 ///< Delay (in microseconds) how long is been waited between tries to find nw.
        TUint32 powerMode;                          ///< Status of WLAN power mode (enabled/disabled)
        TBool allowRadioMeasurements;               ///< Are radio measurements allowed
        TUint32 minPassiveChannelTime;              ///< Min time to listen channel in passive scanning.
        TUint32 maxPassiveChannelTime;              ///< Max time to listen channel in passive scanning.
        TUint32 maxApFailureCount;                  ///< Maximum amount of association failures allowed before blacklisting.
        TUint32 maxApAuthFailureCount;              ///< Maximum amount of authentication failures allowed before blacklisting.
        TUint32 longBeaconFindCount;                ///< Maximum number of tries for scanning APs with long beacon intervals.
        TUint32 qosNullFrameInterval;               ///< Defines how often a QoS NULL data frame is sent.
        TUint32 qosNullFrameTimeout;                ///< Defines how soon after the last Voice AC packet QoS NULL data frame sending is stopped.
        TUint32 keepAliveInterval;                  ///< Defines how often NULL data frames are sent when nothing is being transmitted (in microseconds).
        TUint32 scanStopRcpiThreshold;              ///< Defines the RCPI threshold for stopping a direct scan in a BSS lost roam.
        TUint32 minRcpiForIapAvailability;          ///< Defines minimum RCPI required before an IAP can be marked as available.
        TUint32 qosNullFrameEntryTimeout;           ///< If the time between two voice packets is less than this value, QoS NULL data frame sending is started (in microseconds).
        TUint32 maxApDeauthenticationCount;         ///< After a successful roam an AP can deauthenticate the terminal this many times before it's blacklisted.
        TUint32 apDeauthenticationTimeout;          ///< Deauthentications that happen within time defined by this value after a successful roam are calculated towards AP's deauthentication count (in microseconds).
        TUint32 maxDtimSkipInterval;                ///< Maximum amount of microseconds to sleep when waking up on DTIMs.       
        TUint32 psActiveToLightTimeout;             ///< Timeout for Active->Light transition.
        TUint32 psActiveToLightThreshold;           ///< Frame count threshold for Active->Light transition.
        TUint32 psLightToActiveTimeout;             ///< Timeout for Light->Active transition.
        TUint32 psLightToActiveThreshold;           ///< Frame count threshold for Light->Active transition.
        TUint32 psLightToDeepTimeout;               ///< Timeout for Light->Deep transition.
        TUint32 psLightToDeepThreshold;             ///< Frame count threshold for Light->Deep transition.
        TUint32 psUapsdRxThreshold;                 ///< Rx frame lenght threshold in U-APSD for Best Effort.        
        TUint32 rcpiRoamMinInterval;                ///< The minimum delay between RCPI roam checks (in microseconds).
        TUint32 rcpiRoamMaxInterval;                ///< The maximum delay between RCPI roam checks (in microseconds).
        TUint32 rcpiRoamAttemptsPerInterval;        ///< Defines how many times an RCPI roam check interval value is used until the next interval value is calculated.
        TUint32 rcpiRoamNextIntervalFactor;         ///< The factor the current interval value is multiplied by to get the next interval value.
        TUint32 rcpiRoamNextIntervalAddition;       ///< The value that is added to the interval value that has been multiplied by rcpiRoamNextIntervalFactor.
        TUint32 scanListExpirationTime;             ///< Defines how long beacons/probe responses are stored in the internal scan list (in microseconds).
        TUint32 qosNullFrameEntryTxCount;           ///< Defines how many Voice AC packets must be sent during a certain period before QoS NULL data frame sending is started.
        TUint32 spRcpiTarget;                       ///< The signal predictor algorithm target RCPI value for roam indication.
        TUint32 spTimeTarget;                       ///< The signal predictor algorithm target time for roam indication (in microseconds).
        TUint32 spMinIndicationInterval;            ///< The minimum time interval for consecutive roam indications from the signal predictor algorithm (in microseconds).
        TUint32 bssLostRoamMinInterval;             ///< The minimum delay between BSS lost roam attempts (in microseconds).
        TUint32 bssLostRoamMaxInterval;             ///< The maximum delay between BSS lost roam attempts (in microseconds).
        TUint32 bssLostRoamAttemptsPerInterval;     ///< Defines how many times a BSS lost roam interval value is used until the next interval value is calculated.
        TUint32 bssLostRoamNextIntervalFactor;      ///< The factor the current interval value is multiplied by to get the next interval value.
        TUint32 bssLostRoamNextIntervalAddition;    ///< The value that is added to the interval value that has been multiplied by bssLostRoamNextIntervalFactor.
        TUint32 bssLostRoamMaxTriesToFindNw;        ///< Defines how many attempts are made to find a suitable AP before giving up during BSS lost.
        TUint32 trafficStreamCreationTimeout;       ///< Defines how many microseconds to wait for traffic stream request response. 
        TUint32 beaconLostThreshold;                ///< The number of lost consecutive beacons after which beacon lost event is indicated. 
        TUint32 btBeaconLostThreshold;              ///< The number of lost consecutive beacons after which beacon lost event is indicated during an active Bluetooth connection. 
        TUint32 txFailThreshold;                    ///< The number of lost consecutive packets after which TX failure event is indicated.
        TUint32 btTxFailThreshold;                  ///< The number of lost consecutive packets after which TX failure event is indicated during an active Bluetooth connection.
        TUint32 powerSaveDelay;                     ///< The number of seconds after which power save is enabled during the initial association.
        TUint32 regionExpirationTime;               ///< The amount of seconds region information is valid.
        TUint32 rrmMinMeasurementInterval;          ///< Minimum interval for accepting subsequent RRM Beacon Requests (in microseconds).
        TUint32 psmServerMode;                      ///< PSM server mode (normal, power save, partial).
        TUint32 bgScanPeakPeriodStart;              ///< The time WLAN background scanning peak period starts.
        TUint32 bgScanPeakPeriodEnd;                ///< The time WLAN background scanning peak period ends.
        TUint32 bgScanIntervalPeak;                 ///< WLAN background scan interval for peak period.
        TUint32 bgScanIntervalOffPeak;              ///< WLAN background scan interval for off-peak period.
        TBool automaticTrafficStreamMgmt;           ///< Whether admission control traffic stream management is done automatically.        
        TUint32 minActiveBroadcastChannelTime;      ///< Min time (TUs) to listen a channel in active broadcast scanning when not connected.
        TUint32 maxActiveBroadcastChannelTime;      ///< Max time (TUs) to listen a channel in active broadcast scanning when not connected.
        TUint32 enabledFeatures;                    ///< Bitmask of enabled WLAN features.
        TUint32 region;                             ///< Last known WLAN region that is valid for 5 hours. This value is selected based on the information received from the APs or from cellular network (MCC).
        TInt32  regionTimestamp;                    ///< Timestamp for storing the latest WLAN region (region) to CenRep (minutes from 0AD nominal Gregorian).  
        };
 
    public:     // Methods

    // Constructors and destructor

        /**
        * Static constructor.
        */
        IMPORT_C static CWlanDeviceSettings* NewL();
        
        /**
        * Destructor.
        */
        IMPORT_C virtual ~CWlanDeviceSettings();

    // New methods

        /**
        * Read WLAN device settings from the database.
        * The read values is dependent of the useDefaultSettings -value.
        * If it is ETrue in user defineable table the default settings are
        * been returned, it is EFalse user defined values are returned.
        * @param aSettings Current WLAN device settings.
        * Leaves with general error code.
        */
        IMPORT_C void ReadL( SWlanDeviceSettings& aSettings );

        /**
        * Read WLAN device settings from the database.
        * @param aGetDefaultSettings    ETrue return default settings, 
        *                               EFalse return user defined values.
        * @param aSettings              Current WLAN device settings.
        * Leaves with general error code.
        */
        IMPORT_C void ReadL( TBool aGetDefaultSettings, SWlanDeviceSettings& aSettings );

        /**
        * Save user defineable WLAN device settings to the database.
        * @param aSettings New WLAN device settings.
        * Leaves with general error code.
        */
        IMPORT_C void WriteL( const SWlanDeviceSettings& aSettings );
        
        /**
        * Save user defineable WLAN device setting to the Central Repository.
        * @param aKey Id of the WLAN Central Repository key.
        * @param aValue Value to be written to the WLAN Central Repository key.
        * Leaves with general error code.
        */
        IMPORT_C void WriteCenRepKeyL( const TUint32 aKey, const TInt aValue ) const;

        /**
        * Returns the default WLAN device settings.
        *
        * @param aSettings Default WLAN device settings
        */
        IMPORT_C static void GetDefaultSettings( SWlanDeviceSettings& aSettings );

    // Methods from base classes

    protected:  // Methods

    // New Methods
        
    // Methods from base classes

    private:    //Methods

        /**
        * C++ default constructor.
        */
        CWlanDeviceSettings();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Check is WLAN device settings table correct in CommsDB.
        */
        void CheckWlanDeviceSettingsTableL();

        /**
        * Insert a new table to WLAN device settings and initialise it.
        * @param aTableType 0 for default settings, 1 for user settings.
        */
        void NewRecordL( TUint32 aTableType );

        /**
        * Open database table.
        */
        void OpenTableL();

        /**
        * Read data from CenRep
        * @param aSettings will contain the settings on completion
        */
        TInt ReadPrivateData( SWlanDeviceSettings& aSettings );

        /**
        * Write data to CenRep
        * @param aSettings contains the settings to write
        */
        TInt WritePrivateData( const SWlanDeviceSettings& aSettings );

#ifdef _DEBUG
        void LogSettings( const SWlanDeviceSettings& aSettings ) const;
#endif // _DEBUG

    private:    // Data
        // Comms database
        CCommsDatabase* iDb;

        //Default WLAN device settings table from Comms database
        CCommsDbTableView* iDefTable;
        
        // User defined WLAN device settings table from Comms database
        CCommsDbTableView* iUsrTable;
    };

#endif      // WLANDEVICESETTINGS_H

// End of File
