/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ECom interface definition for WLAN Control API.
*
*/

/*
* %version: 11 %
*/

#ifndef WLANCONTROLINTERFACE_H
#define WLANCONTROLINTERFACE_H

#include <wlanpowersaveinterface.h> // TWlanPowerSave
#include <wlanmgmtcommon.h>         // TWlanBssid, TWlanSsid

// INCLUDES
#include <e32std.h>


/** Defines the possible values of maximum service period length. */
enum TWlanMaxServicePeriodLength
    {
    EWlanMaxServicePeriodLengthAll,
    EWlanMaxServicePeriodLengthTwo,
    EWlanMaxServicePeriodLengthFour,
    EWlanMaxServicePeriodLengthSix,
    };

/** Defines the possible values of last cause for roaming. */
enum TWlanRoamReason
    {
    EWlanRoamReasonLowRssi,
    EWlanRoamReasonApLost,
    };

/**
 * Possible WLAN regions.
 */
enum TWlanRegion
    {
    /** Channels 1-11, USA */
    EFCC    = 0x10,
    /** Channels 1-13, Europe */
    EETSI   = 0x30,
    };

/** Possible WLAN power save modes. */
enum TWlanPowerSaveMode
    {
    /** Automatic mode, wake-up mode and interval adjusted dynamically. */
    EWlanPowerSaveModeAutomatic,
    /** Power save is disabled. */
    EWlanPowerSaveModeNone,
    /** Power save is enabled, wake-up at every beacon. */
    EWlanPowerSaveModeBeacon,
    /** Power save is enabled, wake-up at every DTIM. */
    EWlanPowerSaveModeDtim,
    /** Power save is enabled, wake-up at every Nth DTIM, where N is defined by DTIM skipping interval. */
    EWlanPowerSaveModeDtimSkipping
    };

/** Defines a structure for storing WLAN packet statistics. */
struct TWlanPacketStatistics
    {
    /** Number of sent data frames. */
    TUint32 txFrames;
    /** Number of successfully received data frames. */
    TUint32 rxFrames;
    /** Number of sent multicast data frames. */
    TUint32 txMulticastFrames;
    /** Number of successfully received multicast data frames. */
    TUint32 rxMulticastFrames;
    /** Number of received frames with FCS errors. */
    TUint32 fcsErrors;
    /** Total number of retransmissions done. */
    TUint32 txRetries;
    /** Number of data frames that could not be to delivered. */
    TUint32 txErrors;
    };

/** Defines the U-APSD settings for the access categories. */
struct TWlanUapsdSettings
    {
    /** Defines the maximum number of frames to send during a service period. */
    TWlanMaxServicePeriodLength maxServicePeriodLength;
    /** Whether U-APSD is trigger and delivery-enabled for Voice. */
    TBool uapsdForVoice;
    /** Whether U-APSD is trigger and delivery-enabled for Video. */
    TBool uapsdForVideo;
    /** Whether U-APSD is trigger and delivery-enabled for BestEffort. */
    TBool uapsdForBestEffort;
    /** Whether U-APSD is trigger and delivery-enabled for Background. */
    TBool uapsdForBackground;
    };

/** Defines the power save settings for the access categories. */
struct TWlanPowerSaveSettings
    {
    /** Whether the terminal stays in U-APSD power save when using Voice. */
    TBool stayInUapsdPsModeForVoice;
    /** Whether the terminal stays in U-APSD power save when using Video. */
    TBool stayInUapsdPsModeForVideo;
    /** Whether the terminal stays in U-APSD power save when using BestEffort. */
    TBool stayInUapsdPsModeForBestEffort;
    /** Whether the terminal stays in U-APSD power save when using Background. */
    TBool stayInUapsdPsModeForBackground;
    /** Whether the terminal stays in legacy power save when using Voice. */
    TBool stayInLegacyPsModeForVoice;
    /** Whether the terminal stays in legacy power save when using Video. */
    TBool stayInLegacyPsModeForVideo;
    /** Whether the terminal stays in legacy power save when using BestEffort. */
    TBool stayInLegacyPsModeForBestEffort;
    /** Whether the terminal stays in legacy power save when using Background. */
    TBool stayInLegacyPsModeForBackground;
    };

/**
 * Data structure for storing information about an AP.
 */
struct TWlanAccessPointInfo
    {
    /** The SSID of the AP. */
    TWlanSsid ssid;
    /** The BSSID of the AP. */
    TWlanBssid bssid;
    /** Capabilities of the AP. */
    TUint16 capabilities;
    /** The channel AP is on. */
    TUint8 channel;
    /** Received Signal Strength Indicator (RSSI). */
    TUint8 rssi;
    /** Basic rates bitmap of the AP. Rates are defined in TWlanRate. */
    TUint32 basicRates;
    /** Supported rates bitmap of the AP. Rates are defined in TWlanRate. */
    TUint32 supportedRates;
    /** The security mode of the AP. */
    TWlanConnectionExtentedSecurityMode securityMode;
    /** Whether Admission Control must be used with Voice. */
    TBool isAcRequiredForVoice;
    /** Whether Admission Control must be used with Video. */
    TBool isAcRequiredForVideo;
    /** Whether Admission Control must be used with BestEffort. */
    TBool isAcRequiredForBestEffort;
    /** Whether Admission Control must be used with Background. */
    TBool isAcRequiredForBackground;
    /** Whether the AP supports WPX. */
    TBool isWpx;
    };


/**
 * Data structure for storing roaming metrics information.
 */
struct TWlanRoamMetrics
    {
    /** Connection attempt total counter. */
    TUint32 connectionAttemptTotalCount;
    /** Unsuccesfull connection attempt counter. */
    TUint32 unsuccesfullConnectionAttemptCount;
    /** Roaming counter. */
    TUint32 roamingCounter;
    /** Coverage loss counter. */
    TUint32 coverageLossCount;

    /** The total duration (ms) of the last roaming (= data path broken time + scanning time). */
    TUint32 lastRoamTotalDuration;
    /** The duration (ms) how long the data path was broken during the last roaming. */
    TUint32 lastRoamDataPathBrokenDuration;
    /** The cause for the last roaming. */
    TWlanRoamReason lastRoamReason;
    };


/**
 * @brief ECom interface definition for WLAN Control API.
 *
 * This class defines the methods used for...
 *
 * @since S60 v3.2
 */
class MWlanControlInterface
    {

public:

    /**
     * Get packet statistics of the current connection.
     *
     * @since S60 v3.2
     * @param aStatistics Packet statistics of the current connection.
     * @return KErrNone if statistics were read successfully, an error otherwise.
     */
    virtual TInt GetPacketStatistics(
        TWlanPacketStatistics& aStatistics ) = 0;

    /**
     * Clear packet statistics of the current connection.
     *
     * @since S60 v3.2
     */        
    virtual void ClearPacketStatistics() = 0;

    /**
     * Get the current U-APSD settings.
     *
     * @since S60 v3.2
     * @param aSettings Current U-APSD settings.
     * @return KErrNone if settings were read successfully, an error otherwise.
     */
    virtual TInt GetUapsdSettings(
        TWlanUapsdSettings& aSettings ) = 0;

    /**
     * Set the U-APSD settings.
     *
     * @since S60 v3.2
     * @param aSettings Current U-APSD settings to be set.
     * @return KErrNone if settings were set successfully, an error otherwise.
     */
    virtual TInt SetUapsdSettings(
        const TWlanUapsdSettings& aSettings ) = 0;

    /**
     * Get the current power save settings.
     *
     * @since S60 v3.2
     * @param aSettings power save settings.
     * @return KErrNone if settings were read successfully, an error otherwise.
     */
    virtual TInt GetPowerSaveSettings(
        TWlanPowerSaveSettings& aSettings ) = 0;

    /**
     * Set the power save settings.
     *
     * @since S60 v3.2
     * @param aSettings Current power save settings to be set.
     * @return KErrNone if settings were set successfully, an error otherwise.
     */      
    virtual TInt SetPowerSaveSettings(
        const TWlanPowerSaveSettings& aSettings ) = 0;


    /**
     * Get information about the current AP.
     *
     * @since S60 v3.2
     * @param aInfo Information about the current AP.
     * @return KErrNone if information is available, an error otherwise.
     */
    virtual TInt GetAccessPointInfo(
        TWlanAccessPointInfo& aInfo ) = 0;

    /**
     * Get roam metrics of the current connection.
     *
     * @since S60 v3.2
     * @param aRoamMetrics Roam metrics of the current connection.
     * @return KErrNone if settings were read successfully, an error otherwise.
     */
    virtual TInt GetRoamMetrics(
        TWlanRoamMetrics& aRoamMetrics ) = 0;

    /**
     * Return a list of BSSIDs on the rogue list.
     *
     * @since S60 v3.2
     * @param aRogueList List of BSSIDs on the rogue list.
     * @return KErrNone if list were read successfully, an error otherwise.
     */
    virtual TInt GetRogueList(
        CArrayFixSeg<TWlanBssid>& aRogueList ) = 0;
        
    /**
     * Get the current regulatory domain.
     *
     * @since S60 v3.2
     * @param aRegion current region.
     * @return KErrNone if value was read successfully, an error otherwise.
     */
    virtual TInt GetRegulatoryDomain(
        TWlanRegion& aRegion ) = 0;
        
    /**
     * Get the current power save mode.
     *
     * @since S60 v3.2
     * @param aPowerSaveMode current power save mode.
     * @return KErrNone if value was read successfully, an error otherwise.
     */
    virtual TInt GetPowerSaveMode(
        TWlanPowerSave& aPowerSaveMode ) = 0;

    /**
     * Set the power save mode.
     *
     * @note This method is meant for overriding the dynamic power save mode selection
     *       for testing purposes. Overriding the power save mode may have an adverse
     *       effect on throughput and/or power consumption. Dynamic selection can be
     *       re-enabled by setting the mode to EWlanPowerSaveModeAutomatic.
     *
     * @since S60 v5.0.1
     * @param aMode Power save mode to be set.
     * @return KErrNone if the mode was set successfully, an error otherwise.
     */
    virtual TInt SetPowerSaveMode(
        TWlanPowerSaveMode aMode ) = 0;

    };

#endif // WLANCONTROLINTERFACE_H
