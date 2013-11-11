/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Contains common data structures used by WLAN management service.
*
*/

/*
* %version: 12 %
*/

#ifndef WLANMGMTCOMMON_H
#define WLANMGMTCOMMON_H

// INCLUDES
#include <e32std.h>

// LOCAL CONSTANTS
/** The maximum SSID length. */
const TUint KWlanMaxSsidLength = 32;

/** The maximum WPS PIN length. */
const TUint KWlanMaxWpsPinLength = 8;

/** The maximum BSSID length. */
const TUint KWlanMaxBssidLength = 6;

/** The maximum length of WPA preshared key data. */
const TUint KWlanWpaPskMaxLength = 64;

/** The maximum length of WEP key data. */
const TUint KWlanWepKeyMaxLength = 29;

/** Infinite max delay to be used in GetScanResults and GetAvailableIaps */
const TUint KWlanInfiniteScanDelay = 0xFFFFFFFF;

/** Current version of WLAN callback interface. */
const TUint KWlanCallbackInterfaceVersion = 2;

// DATA TYPES
/** Data structure for storing the SSID of a WLAN network. */
typedef TBuf8<KWlanMaxSsidLength> TWlanSsid;

/** Data structure for storing the WPS Pin. */
typedef TBuf8<KWlanMaxWpsPinLength> TWlanWpsPin;

/** Data structure for storing the BSSID of a BSS. */
typedef TBuf8<KWlanMaxBssidLength> TWlanBssid;

/** Data structure for storing a WEP key. */
typedef TBuf8<KWlanWepKeyMaxLength> TWlanWepKey;

/** 
 * Data structure for storing either a WPA preshared key or passphrase.
 *
 * The WPA-PSK key can be represented either as a passphrase or as the
 * actual preshared key.
 *
 * A passphrase can contain from 8 to 63 ASCII characters where each
 * character MUST have a decimal encoding in the range of 32 to 126,
 * inclusive.
 *
 * A preshared key is stored as 64 character hex string.
 */
typedef TBuf8<KWlanWpaPskMaxLength> TWlanWpaPresharedKey;

/** Values for possible WLAN connection states. */
enum TWlanConnectionMode
    {
    /** No connection is active. */
    EWlanConnectionModeNotConnected,
    /** Connection to an infrastructure network is active. */
    EWlanConnectionModeInfrastructure,    
    /** Connection to an ad-hoc network is active. */
    EWlanConnectionModeAdhoc,
    /** Connection to a secure infrastructure network is active. */
    EWlanConnectionModeSecureInfra,
    // Searching for an access point. No data flow.
    EWlanConnectionModeSearching
    };

/** Values for possible WLAN operating modes. */
enum TWlanOperatingMode
    {
    /** Ad-hoc network. */
    EWlanOperatingModeAdhoc,
    /** Infrastructure network. */
    EWlanOperatingModeInfrastructure
    };

/** Values for possible WLAN connection security modes. */
enum TWlanConnectionSecurityMode
    {
    /** Security mode open, i.e. no security. */
    EWlanConnectionSecurityOpen,
    /** Security mode WEP. */
    EWlanConnectionSecurityWep,
    /** Security mode 802d1x. */
    EWlanConnectionSecurity802d1x,
    /** Security mode WPA. */
    EWlanConnectionSecurityWpa,
    /** Security mode WPA PSK. */
    EWlanConnectionSecurityWpaPsk
    };

/**
 * Values for possible WLAN connection security modes.
 * These are more detailed than TWlanConnectionSecurityMode.
 */
enum TWlanConnectionExtentedSecurityMode
    {
    /** Security mode open, i.e. no security. */
    EWlanConnectionExtentedSecurityModeOpen,
    /** Security mode Open WEP. */
    EWlanConnectionExtentedSecurityModeWepOpen,
    /** Security mode Shared WEP. */
    EWlanConnectionExtentedSecurityModeWepShared,
    /** Security mode 802d1x. */
    EWlanConnectionExtentedSecurityMode802d1x,
    /** Security mode WPA. */
    EWlanConnectionExtentedSecurityModeWpa,
    /** Security mode WPA PSK. */
    EWlanConnectionExtentedSecurityModeWpaPsk,
    /** Security mode WPA2. */
    EWlanConnectionExtentedSecurityModeWpa2,
    /** Security mode WPA2 PSK. */
    EWlanConnectionExtentedSecurityModeWpa2Psk,
    /** Security mode WAPI. */
    EWlanConnectionExtentedSecurityModeWapi,
    /** Security mode WAPI PSK. */
    EWlanConnectionExtentedSecurityModeWapiPsk
    };

/** Defines the possible values for IAP security mode. */
enum TWlanIapSecurityMode
    {
    /** No encryption used. */
    EWlanIapSecurityModeAllowUnsecure,
    /** Use WEP encryption with static keys. */
    EWlanIapSecurityModeWep,
    /** Use WEP/TKIP/CCMP encryption, keys are negotiated by EAPOL. */
    EWlanIapSecurityMode802d1x,
    /** Use TKIP/CCMP encryption, keys are negotiated by EAPOL. */
    EWlanIapSecurityModeWpa,
    /** Use CCMP encryption, keys are negotiated by EAPOL. */
    EWlanIapSecurityModeWpa2Only,
    };

/** Data values for RSS classes. */
enum TWlanRssClass
    {
    /** Received signal level is 'normal'. */
    EWlanRssClassNormal,
    /** Received signal level is 'weak'. */
    EWlanRssClassWeak
    };

/** Enumeration of the possible default WEP keys. */
enum TWlanDefaultWepKey
    {
    EWlanDefaultWepKey1,            ///< Key number 1
    EWlanDefaultWepKey2,            ///< Key number 2
    EWlanDefaultWepKey3,            ///< Key number 3
    EWlanDefaultWepKey4             ///< Key number 4
    };

/** Enumeration of the possible authentication modes. */
enum TWlanAuthenticationMode
    {
    EWlanAuthenticationModeOpen,    ///< Open authentication
    EWlanAuthenticationModeShared   ///< Shared authentication
    };

/** Enumaration for possible traffic stream statuses. */
enum TWlanTrafficStreamStatus
    {
    /**
     * The traffic stream is active in the current WLAN access point.
     */
    EWlanTrafficStreamStatusActive,
    /**
     * The traffic stream is not active in the current WLAN access point
     * because the AP doesn't require admission control.
     */
    EWlanTrafficStreamStatusInactiveNotRequired,
    /**
     * The traffic stream is not active in the current WLAN access point
     * because the AP has deleted the traffic stream.
     */    
    EWlanTrafficStreamStatusInactiveDeletedByAp,
    /**
     * The traffic stream is not active in the current WLAN access point
     * because the AP has refused the traffic stream request due to
     * insufficient over-the-air bandwidth.
     */
    EWlanTrafficStreamStatusInactiveNoBandwidth,
    /**
     * The traffic stream is not active in the current WLAN access point
     * because the AP has refused the traffic stream request due to
     * invalid traffic stream parameters.
     */
    EWlanTrafficStreamStatusInactiveInvalidParameters,
    /**
     * The traffic stream is not active in the current WLAN access point
     * because the AP has refused the traffic stream request due to
     * other reasons.
     */
    EWlanTrafficStreamStatusInactiveOther
    };

/** Enumeration for traffic stream direction. */
enum TWlanTrafficStreamDirection
    {
    /** Admission is requested for uplink traffic. */ 
    EWlanTrafficStreamDirectionUplink,
    /** Admission is requested for downlink traffic. */ 
    EWlanTrafficStreamDirectionDownlink,
    /** Admission is requested for both uplink and downlink traffic. */ 
    EWlanTrafficStreamDirectionBidirectional
    };

/** Enumeration for traffic stream traffic type. */
enum TWlanTrafficStreamTrafficType
    {
    /** Traffic pattern is periodic,. */
    EWlanTrafficStreamTrafficTypePeriodic,
    /** Traffic pattern is aperiodic or unspecified. */
    EWlanTrafficStreamTrafficTypeAperiodic
    };

/** Defines the possible TX rate values. */
enum TWlanRate
	{
	TWlanRateNone       = 0x00000000,
	TWlanRate1mbit      = 0x00000001,
	TWlanRate2mbit      = 0x00000002,
	TWlanRate5p5mbit    = 0x00000004,
	TWlanRate6mbit      = 0x00000008,
	TWlanRate9mbit      = 0x00000010,
	TWlanRate11mbit     = 0x00000020,
	TWlanRate12mbit     = 0x00000040,
	TWlanRate18mbit     = 0x00000080,
	TWlanRate22mbit     = 0x00000100,
	TWlanRate24mbit     = 0x00000200,
	TWlanRate33mbit     = 0x00000400,
	TWlanRate36mbit     = 0x00000800,
	TWlanRate48mbit     = 0x00001000,
	TWlanRate54mbit     = 0x00002000
	};

/** Data structure for storing a Protected Setup credential attribute. */
struct TWlanProtectedSetupCredentialAttribute
    {
    /** Operating mode of the network. */
    TWlanOperatingMode iOperatingMode;
    /** Authentication mode of the network. */
    TWlanAuthenticationMode iAuthenticationMode;
    /** Security mode of the network. */
    TWlanIapSecurityMode iSecurityMode;
    /** Name of the network. */
    TWlanSsid iSsid;
    /** WEP key number 1. */
    TWlanWepKey iWepKey1;
    /** WEP key number 2. */
    TWlanWepKey iWepKey2;
    /** WEP key number 3. */
    TWlanWepKey iWepKey3;
    /** WEP key number 4. */
    TWlanWepKey iWepKey4;
    /** The WEP key used by default. */
    TWlanDefaultWepKey iWepDefaultKey;
    /** WPA preshared key. */
    TWlanWpaPresharedKey iWpaPreSharedKey;
    };

/**
 * Data structure for storing IAP availability data.
 */
struct TWlanIapAvailabilityData
    {
    /**
     * ID of the IAP.
     */
    TUint iIapId;
    /** 
     * Received Signal Strength Indicator (RSSI) of the strongest
     * AP in the network.
     */
    TUint iRssi;
    };

// CLASS DECLARATION
/** 
 * Callback interface for WLAN management notifications.
 *
 * These virtual methods should be inherited and implemented by the
 * client wanting to observe WLAN management events.
 *
 * The client has to enable notifications by calling the appropriate
 * method from the management interface.
 * @see MWlanMgmtInterface::ActivateNotificationsL.
 * @see MWlanMgmtInterface::ActivateExtendedNotificationsL.
 * @since S60 3.0
 */
class MWlanMgmtNotifications
    {
    public:

        /**
         * Connection state has changed.
         *
         * @since Callback interface v1
         * @param aNewState New connection state.
         */
        virtual void ConnectionStateChanged(
            TWlanConnectionMode /* aNewState */ ) {};

        /**
         * BSSID has changed (i.e. AP handover).
         *
         * @param aNewBSSID BSSID of the new access point.
         * @since Callback interface v1
         */
        virtual void BssidChanged(
            TWlanBssid& /* aNewBSSID */ ) {};

        /**
         * Connection has been lost.
         *
         * @since Callback interface v1
         */
        virtual void BssLost() {};

        /**
         * Connection has been regained.
         *
         * @since Callback interface v1
         */
        virtual void BssRegained() {};

        /**
         * New networks have been detected during scan.
         *
         * @since Callback interface v1
         */
        virtual void NewNetworksDetected() {};

        /**
         * One or more networks have been lost since the last scan.
         *
         * @since Callback interface v1
         */
        virtual void OldNetworksLost() {};

        /**
         * The used transmit power has been changed.
         *
         * @since Callback interface v1
         * @param aPower The transmit power in mW.
         */
        virtual void TransmitPowerChanged(
            TUint /* aPower */ ) {};
        
        /**
         * Received signal strength level has been changed.
         *
         * @since Callback interface v1
         * @param aRssClass specifies the current class of the received signal
         * @param aRss RSS level in absolute dBm values.
         */
        virtual void RssChanged(
            TWlanRssClass /* aRssClass */,
            TUint /* aRss */ ) {};

        /**
         * The status of a virtual traffic stream has changed.
         *
         * @since Callback interface v2
         * @param aStreamId ID of the traffic stream.
         * @param aStreamStatus Status of the traffic stream.
         */
        virtual void TrafficStreamStatusChanged(
            TUint /* aStreamId */,
            TWlanTrafficStreamStatus /* aStreamStatus */ ) {};

    };

#endif // WLANMGMTCOMMON_H
            
// End of File
