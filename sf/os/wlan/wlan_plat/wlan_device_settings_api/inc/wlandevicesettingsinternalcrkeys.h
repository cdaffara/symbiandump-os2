/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Internal CenRep keys
*
*/

/*
* %version: 16 %
*/

#ifndef WLANDEVICESETTINGSINTERNALCRKEYS_H
#define WLANDEVICESETTINGSINTERNALCRKEYS_H

/**
 * Repository Id
 */
const TUid KCRUidWlanDeviceSettingsRegistryId = {0x101f8e44};

/**
 * The beacon interval in milliseconds to be used when creating an IBSS network.
 */
const TUint32 KWlanBeacon =                              0x00000002;

/**
 * The data rate used for sending probe requests. The value is determined by
 * multiplying the data rate by two; 1MB/s = 2, 2MB/s = 4 and so forth.
 */
const TUint32 KWlanScanRate =                            0x00000004;

/**
 * The RCPI threshold after which roaming to a better AP is attempted.
 */
const TUint32 KWlanRcpiTrigger =                         0x00000005;

/**
 * The minimum channel time for active scan in milliseconds.
 */
const TUint32 KWlanMinActiveChannelTime =                0x00000006;

/**
 * The maximum channel time for active scan in milliseconds.
 */
const TUint32 KWlanMaxActiveChannelTime =                0x00000007;

/**
 * The maximum amount of time allowed to transmit a packet before
 * it is discarded. The value is in milliseconds.
 */
const TUint32 KWlanMaxTxMSDULifeTime =                   0x00000008;

/**
 * Defines how many seconds the cached broadcast scan results are valid.
 */
const TUint32 KWlanScanExpirationTimer =                 0x00000009;

/**
 * The activity timeout value after which the WLAN drivers are unloaded.
 * Note that this setting applies only when the terminal is not connected
 * to a network. The value is in seconds.
 */
const TUint32 KWlanUnloadDriverTimer =                   0x0000000A;

/**
 * Defines how often roaming to better AP is attempted when the threshold
 * set in KWlanRcpiTrigger has been crossed. The value is in microseconds.
 */
const TUint32 KWlanRoamTimer =                           0x0000000B;

/**
 * Defines the how much better an AP has to be than the current AP
 * before roaming is attempted.
 */
const TUint32 KWlanRcpiDifference =                      0x0000000C;

/**
 * The amount of time to wait after a BSS lost indication before
 * attempting to roam. The value is in microseconds.
 */
const TUint32 KWlanConnRegainTimer =                     0x0000000D;

/**
 * The number of attempts to find suitable network during initial connect.
 */
const TUint32 KWlanMaxTriesToFindNw =                    0x0000000E;

/**
* KWlanDelayBetweenFindNw 
* Defines the delay between attempts to find network.
* (microseconds)
*/
const TUint32 KWlanDelayBetweenFindNw =                  0x0000000F;

/**
 * Maximum transfer unit (MTU) size
 */
const TUint32 KWlanMTU =                                 0x00000010;

/**
* KWlanMinPassiveChannelTime
* Minimum channel time for passive scan.
*/
const TUint32 KWlanMinPassiveChannelTime =               0x00000011;

/**
* KWlanMaxPassiveChannelTime
* Maximum channel time for passive scan.
*/
const TUint32 KWlanMaxPassiveChannelTime =               0x00000012;

/**
* KWlanMaxApFailureCount
* Maximum amount of association failures.
*/
const TUint32 KWlanMaxApFailureCount =                   0x00000013;

/**
* KWlanLongBeaconFindCount
* Long beacon interval AP detection.
*/
const TUint32 KWlanLongBeaconFindCount =                 0x00000014;

/**
* KWlanQosNullFrameInterval
* QoS NULL data frame send interval.
*/
const TUint32 KWlanQosNullFrameInterval =                0x00000015;

/**
* KWlanQosNullFrameTimeout
* QoS NULL data frame send timeout.
*/
const TUint32 KWlanQosNullFrameTimeout =                 0x00000016;

/**
 * NULL data frame sending interval when no transmission in microseconds.
 */
const TUint32 KWlanKeepAliveInterval =                   0x00000017;

/**
 * RCPI threshold for direct scan stop in BSS lost.
 */
const TUint32 KWlanScanStopRcpiThreshold =               0x00000018;

/**
 * Minimum RCPI for available IAP.
 */
const TUint32 KWlanMinRcpiForIapAvailability =           0x00000019;

/**
 * Entry timeout for QoS NULL frame sending in microseconds.
 */
const TUint32 KWlanQoSNullFrameEntryTimeout =            0x0000001A;

/**
 * Deauthentications per AP before blacklisting.
 */
const TUint32 KWlanMaxApDeauthenticationCount =          0x0000001B;

/**
 * Deauthentication timeout for calculating AP's deauthentication count
 * (in microseconds).
 */
const TUint32 KWlanApDeauthenticationTimeout =           0x0000001C;

/**
 * Default value in seconds for BackGround Scan Interval
 */
const TUint32 KWlanDefaultBGScanInterval =               0x0000001D;

/**
 * Maximum amount of microseconds to sleep when waking up on DTIMs.
 */
const TUint32 KWlanMaxDtimSkipInterval =                 0x0000001F;

/**
 * Time interval in microseconds after which transition from Active mode 
 * to Light PS mode is considered.
 */
const TUint32 KWlanPsActiveToLightTimeout =              0x00000020;

/**
 * Frame count threshold used when considering transition from Active
 * to Light PS mode.
 */
const TUint32 KWlanPsActiveToLightThreshold =            0x00000021;

/**
 * Time interval in microseconds after which the frame counter used when 
 * considering transition from Light PS to Active mode is reset.
 */
const TUint32 KWlanPsLightToActiveTimeout =              0x00000022;

/**
 * Frame count threshold used when considering transition from Light PS
 * to Active mode.
 */
const TUint32 KWlanPsLightToActiveThreshold =            0x00000023;

/**
 * Time interval in microseconds after which transition from Light PS mode 
 * to Deep PS mode is considered.
 */
const TUint32 KWlanPsLightToDeepTimeout =                0x00000024;

/**
 * Frame count threshold used when considering transition from Light PS
 * to Deep PS mode.
 */
const TUint32 KWlanPsLightToDeepThreshold =              0x00000025;

/**
 * Received frame payload length (in bytes) threshold in U-APSD network for
 * Best Effort Access Category.
 */
const TUint32 KWlanPsUapsdRxFrameLengthThreshold =       0x00000026;

/**
 * The minimum delay between RCPI roam checks (in microseconds).
 */
const TUint32 KWlanRcpiRoamMinInterval =                 0x00000027;

/**
 * The maximum delay between RCPI roam checks (in microseconds).
 */
const TUint32 KWlanRcpiRoamMaxInterval =                 0x00000028;

/**
 * Defines how many times an RCPI roam check interval value is used
 * until the next interval value is calculated.
 */
const TUint32 KWlanRcpiRoamAttemptsPerInterval =         0x00000029;

/**
 * The factor the current interval value is multiplied by to
 * get the next interval value.
 */
const TUint32 KWlanRcpiRoamNextIntervalFactor =          0x0000002A;

/**
 * The value that is added to the interval value that has been
 * multiplied by KWlanRcpiRoamNextIntervalFactor.
 */
const TUint32 KWlanRcpiRoamNextIntervalAddition =        0x0000002B;

/**
 * Defines how long beacons/probe responses are stored in
 * the internal scan list (in microseconds).
 */
const TUint32 KWlanScanListExpirationTime =              0x0000002C;

/**
 * Defines how many Voice AC packets must be sent during a certain period
 * before QoS NULL data frame sending is started.
 */
const TUint32 KWlanQoSNullFrameEntryTxCount =            0x0000002D;

/**
 * The signal predictor algorithm target RCPI value for roam indication.
 */
const TUint32 KWlanSpRcpiTarget =                        0x0000002E;

/**
 * The signal predictor algorithm target time for roam indication (in microseconds).
 */
const TUint32 KWlanSpTimeTarget =                        0x0000002F;

/**
 * The minimum time interval for consecutive roam indications
 * from the signal predictor algorithm (in microseconds).
 */ 
const TUint32 KWlanSpMinIndicationInterval =             0x00000030;

/**
 * The minimum delay between BSS lost roam attempts (in microseconds).
 */
const TUint32 KWlanBssLostRoamMinInterval =              0x00000031;

/**
 * The maximum delay between BSS lost roam attempts (in microseconds).
 */
const TUint32 KWlanBssLostRoamMaxInterval =              0x00000032;

/**
 * Defines how many times a BSS lost roam interval value is used
 * until the next interval value is calculated.
 */
const TUint32 KWlanBssLostRoamAttemptsPerInterval =      0x00000033;

/**
 * The factor the current interval value is multiplied by to
 * get the next interval value.
 */
const TUint32 KWlanBssLostRoamNextIntervalFactor =       0x00000034;

/**
 * The value that is added to the interval value that has been
 * multiplied by KWlanBssLostRoamNextIntervalFactor.
 */
const TUint32 KWlanBssLostRoamNextIntervalAddition =     0x00000035;

/**
 * Defines how many attempts are made to find a suitable AP before
 * giving up during BSS lost.
 */
const TUint32 KWlanBssLostRoamMaxTriesToFindNw =         0x00000036;

/**
 * Defines how many microseconds to wait for traffic stream request response.
 */
const TUint32 KWlanTrafficStreamCreationTimeout =        0x00000037;

/**
 * The number of lost consecutive beacons after which beacon lost event
 * is indicated.
 */
const TUint32 KWlanBeaconLostThreshold =                 0x00000038;

/**
 * The number of lost consecutive beacons after which beacon lost event
 * is indicated during an active Bluetooth connection.
 */
const TUint32 KWlanBtBeaconLostThreshold =               0x00000039;

/**
 * The number of lost consecutive packets after which TX failure event
 * is indicated.
 */
const TUint32 KWlanTxFailThreshold =                     0x0000003A;

/**
 * The number of lost consecutive packets after which TX failure event
 * is indicated during an active Bluetooth connection.
 */
const TUint32 KWlanBtTxFailThreshold =                   0x0000003B;

/**
 * The number of seconds after which power save is enabled during
 * the initial association.
 */
const TUint32 KWlanPowerSaveDelay =                      0x0000003C;

/**
 * The amount of seconds region information is valid.
 */
const TUint32 KWlanRegionExpirationTime =                0x0000003D;

/**
 * Minimum interval for accepting subsequent RRM Beacon Requests
 * (in microseconds).
 */
const TUint32 KWlanRrmMinMeasurementInterval =           0x0000003E;

/**
 * Current PSM server mode where possible values are:
 * 0 = Normal mode
 * 1 = Power save
 * 2 = Partial power save
 */
const TUint32 KWlanPsmSrvMode =                          0x0000003F;

/**
 * Peak start time for WLAN background scan specifying hours and minutes.
 * Valid range is 0 to 2359 meaning 00:00 o'clock to 23:59 o'clock.
 */
const TUint32 KWlanBgScanPeakPeriodStart =               0x00000040;

/**
 * Peak end time for WLAN background scan specifying hours and minutes.
 * Valid range is 0 to 2359 meaning 00:00 o'clock to 23:59 o'clock.
 */
const TUint32 KWlanBgScanPeakPeriodEnd =                 0x00000041;

/**
 * WLAN background scan interval in seconds for peak period.
 */
const TUint32 KWlanBgScanIntervalPeakPeriod =            0x00000042;

/**
 * WLAN background scan interval in seconds for off-peak period.
 */
const TUint32 KWlanBgScanIntervalOffPeakPeriod =         0x00000043;

/**
 * Whether admission control traffic stream management is done automatically.
 */
const TUint32 KWlanAutomaticTrafficStreamMgmt =          0x00000044;

/**
 * The minimum channel time for active broadcast scan in milliseconds.
 */
const TUint32 KWlanMinActiveBroadcastChannelTime =       0x00000045;

/**
 * The maximum channel time for active broadcast scan in milliseconds.
 */
const TUint32 KWlanMaxActiveBroadcastChannelTime =       0x00000046;

/**
 * Bitmask of enabled WLAN features.
 */
const TUint32 KWlanEnabledFeatures =                     0x00000047;

/**
 * Cached WLAN region where possible values are: 
 * 0 = Region is unknown 
 * 1 = Region is ETSI 
 * 2 = Region is FCC.
 */
const TUint32 KWlanRegion =                              0x00000050;

/**
 * Timestamp for cached WLAN region (minutes from 0AD nominal Gregorian).
 */
const TUint32 KWlanRegionTimestamp =                     0x00000051;

/**
 * Master switch for WLAN.
 */
const TUint32 KWlanOnOff =                               0x00000052;

/**
 * Setting for overriding WLAN on/off, forcing it to off.
 */
const TUint32 KWlanForceDisable =                        0x00000053;

#endif      // WLANDEVICESETTINGSINTERNALCRKEYS_H
