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
* Description:   This file contains definitions for OID messages, which are 
*                used for issuing management commands to UMAC.
*
*/

/*
* %version: 44 %
*/

#ifndef WLAN_OID_MSG_H
#define WLAN_OID_MSG_H

#include "umac_types.h"


                                                        //    Query   
                                                        //    | Set
                                                        //    | | 
enum TWlanCommandId                                     //    | | 
    {                                                   //    | | 
    E802_11_CONNECT = 0x0C000000,                       //      X
    E802_11_START_IBSS,                                 //      X
    E802_11_SCAN,                                       //      X    
    E802_11_STOP_SCAN,                                  //      X
    E802_11_DISCONNECT,                                 //      X
    E802_11_SET_POWER_MODE,                             //      X
    E802_11_SET_RCPI_TRIGGER_LEVEL,                     //      X     
    E802_11_SET_TX_POWER_LEVEL,                         //      X
    E802_11_CONFIGURE,                                  //      X
    E802_11_GET_LAST_RCPI,                              //    X       
    E802_11_DISABLE_USER_DATA,                          //      X
    E802_11_ENABLE_USER_DATA,                           //      X
    E802_11_ADD_CIPHER_KEY,                             //      X
    E802_11_ADD_MULTICAST_ADDR,                         //      X
    E802_11_REMOVE_MULTICAST_ADDR,                      //      X
    E802_11_CONFIGURE_BSS_LOST,                         //      X
    E802_11_SET_TX_RATE_ADAPT_PARAMS,                   //      X
    E802_11_CONFIGURE_TX_RATE_POLICIES,                 //      X
    E802_11_SET_POWER_MODE_MGMT_PARAMS,                 //      X
    E802_11_CONFIGURE_PWR_MODE_MGMT_TRAFFIC_OVERRIDE,   //      X
    E802_11_GET_FRAME_STATISTICS,                       //    X
    E802_11_CONFIGURE_UAPSD,                            //      X
    E802_11_CONFIGURE_TX_QUEUE,                         //      X
    E802_11_GET_MAC_ADDRESS,                            //    X
    E802_11_CONFIGURE_ARP_IP_ADDRESS_FILTERING,         //      X
    E802_11_CONFIGURE_HT_BLOCK_ACK,                     //      X
    E802_11_CONFIGURE_PROPRIETARY_SNAP_HDR              //      X
    };

const TUint KMaxCipherKeyLength = 32; // 256 bits
   
// -------------------------------------------------------------------------

/** 802.11 Power management modes */
enum TPowerMode
    {
    /** Continuous Awake Mode (CAM). */
    EPowerModeCam,
    /** Power Save (PS) Mode */
    EPowerModePs
    };

/** WLAN wake-up modes in 802.11 PS mode */
enum TWlanWakeUpInterval
    {
    EWakeUpIntervalAllBeacons     = 0,
    EWakeUpIntervalAllDtims       = 1,
    EWakeUpIntervalEveryNthBeacon = 2,
    EWakeUpIntervalEveryNthDtim   = 3,
    };
   
// -------------------------------------------------------------------------
enum TAuthenticationMode
    {
    /**
    * IEEE 802.11 open authentication mode. 
    * No checks when accepting clients in this mode.
    */
    EAuthModeOpen,
    /** IEEE 802.11 shared authentication that uses pre-shared WEP-key. */
    EAuthModeShared,
    /** Vendor specific authentication mode */
    EAuthModeLeap,
    /** Not a real mode, defined as an upper bound. */
    EAuthModeMax
    };

// -------------------------------------------------------------------------
/** WLAN rates, units of 500 kbit/sec */
enum TRate
    {
    ENoRate = 0,
    E1Mbps = 2,
    E2Mbps = 4,
    E5_5Mbps = 11,
    E11Mbps = 22,
    E22Mbps = 44,
    EBASIC_1Mbps = 0x82,
    EBASIC_2Mbps = 0x84,
    EBASIC_5_5Mbps = 0x8b,
    EBASIC_11Mbps = 0x96,
    EBASIC_22Mbps = 0xac
    };
    
// -------------------------------------------------------------------------

typedef enum _TEncryptionStatus
    {
    /** Security is disabled. */
    EEncryptionDisabled,
    /** Use WEP security */
    EEncryptionWep,
    /** Use 802.1x security */
    EEncryption802dot1x,
    /** Use WPA security */
    EEncryptionWpa,
    /** WEP mixed cell */
    EEncryptionWepMixed,
    /** 802.1x mixed cell */
    EEncryption802dot1xMixed,
    /** Use WAPI security */
    EEncryptionWAPI
    } TEncryptionStatus;

// -------------------------------------------------------------------------
/**
* The possible cipher suites.
*/
enum TWlanCipherSuite
    {
    EWlanCipherSuiteNone,
    EWlanCipherSuiteTkip,
    EWlanCipherSuiteCcmp,
    EWlanCipherSuiteWep,
    EWlanCipherSuiteWapi
    };

// -------------------------------------------------------------------------
//
// PRAGMA PACK 4 BEGINS
//
#pragma pack(4)

typedef struct _TSSID
    {
    /** 
    * Length of ssid-field in octets. This can be zero. 
    * If this is set to zero in the Scan command (message) when doing an
    * active scan, a broadcast scan is performed.
    */
    TUint32 ssidLength;
    /**
    * SSID information field.
    */
    TUint8 ssid[KMaxSSIDLength];
    } TSSID;

#pragma pack()
//
// PRAGMA PACK 4 ENDS
//
// -------------------------------------------------------------------------


enum TScanMode
    {
    /** Send probe requests to specified channels. */
    EActiveScan,
    /** Listen beacons from specified channels. */
    EPassiveScan
    };

typedef struct _SChannels
    {
    TUint8 iBand; // 1 = 2.4 GHz, 2 = 4.9 GHz, 4 = 5 GHz
    TUint8 iChannels2dot4GHz[2];
    TUint8 iChannels4dot9GHz[3];
    TUint8 iChannels5GHz[26];
    } SChannels;

// -------------------------------------------------------------------------

typedef struct _TTxRateClass
    {
    /** Number of attempts to tx at 54 Mbits/s */
    TUint8  txPolicy54;
    /** Number of attempts to tx at 48 Mbits/s */
    TUint8  txPolicy48;
    /** Number of attempts to tx at 36 Mbits/s */
    TUint8  txPolicy36;
    /** Number of attempts to tx at 33 Mbits/s */
    TUint8  txPolicy33;
    /** Number of attempts to tx at 24 Mbits/s */
    TUint8  txPolicy24;
    /** Number of attempts to tx at 22 Mbits/s */
    TUint8  txPolicy22;
    /** Number of attempts to tx at 18 Mbits/s */
    TUint8  txPolicy18;
    /** Number of attempts to tx at 12 Mbits/s */
    TUint8  txPolicy12;
    /** Number of attempts to tx at 11 Mbits/s */
    TUint8  txPolicy11;
    /** Number of attempts to tx at 9 Mbits/s */
    TUint8  txPolicy9;
    /** Number of attempts to tx at 6 Mbits/s */
    TUint8  txPolicy6;
    /** Number of attempts to tx at 5.5 Mbits/s */
    TUint8  txPolicy5_5;
    /** Number of attempts to tx at 2 Mbits/s */
    TUint8  txPolicy2;
    /** Number of attempts to tx at 1 Mbits/s */
    TUint8  txPolicy1;

    /** 802.11 ShortRetryLimit used in the rate class */    
    TUint8  shortRetryLimit;
    /** 802.11 LongRetryLimit used in the rate class */    
    TUint8  longRetryLimit;
    
    /** 
    * Transmit flags. 
    * Bit 0 - truncate. If this bit is set, then attempts to send a frame
    * stop when the total valid per-rate attempts have been exhausted, 
    * otherwise, and also in the case the feature is not supported
    * by WHA layer, transmissions will continue at the lowest available 
    * rate until the appropriate one of the iShortRetryLimit, iLongRetryLimit,
    * dot11MaxTransmitMsduLifetime, is exhausted. [optional]
    * Bit 1 - indicates if the preamble override from the rate class should 
    * be used in transmit. [optional]
    * Bit 2 - defines the type of preamble to be used by the rate class. 
    * 0 means long preamble and 1 means short preamble. [optional]    
    */    
    TUint32 flags;
    } TTxRateClass;

const TUint8 KMaxNbrOfRateClasses = 4;

typedef struct _TTxRatePolicy 
    {    
    TUint32         numOfPolicyObjects;
    TTxRateClass    txRateClass[KMaxNbrOfRateClasses];
    } TTxRatePolicy;

// -------------------------------------------------------------------------

typedef TUint8 TRateClassIndex; 

/** Tx Queue to Rate Class mapping */

typedef TRateClassIndex TQueue2RateClass[EQueueIdMax];

// -------------------------------------------------------------------------

typedef TUint32 TRateMask; 

/** Initial Max Tx Rate for Rate Class */
typedef TRateMask TInitialMaxTxRate4RateClass[KMaxNbrOfRateClasses];

/** Tx rate masks */

const TRateMask KRate1Mbits       = 0x00000001;
const TRateMask KRate2Mbits       = 0x00000002;
const TRateMask KRate5_5Mbits     = 0x00000004;
const TRateMask KRate6Mbits       = 0x00000008;
const TRateMask KRate9Mbits       = 0x00000010;
const TRateMask KRate11Mbits      = 0x00000020;
const TRateMask KRate12Mbits      = 0x00000040;
const TRateMask KRate18Mbits      = 0x00000080;
const TRateMask KRate22Mbits      = 0x00000100;
const TRateMask KRate24Mbits      = 0x00000200;
const TRateMask KRate33Mbits      = 0x00000400;
const TRateMask KRate36Mbits      = 0x00000800;
const TRateMask KRate48Mbits      = 0x00001000;
const TRateMask KRate54Mbits      = 0x00002000;

/** 
* Rate policy to use when the WLAN vendor implementation handles 
* Tx rate adaptation
*/
typedef TRateMask TTxAutoRatePolicy[KMaxNbrOfRateClasses];

// -------------------------------------------------------------------------

const   TUint8  KHtMcsSetLength = 10;
typedef TUint8  THtMcsSet[KHtMcsSetLength];
typedef THtMcsSet THtMcsPolicy[KMaxNbrOfRateClasses];

// -------------------------------------------------------------------------

/** the possible values for maximum service period length */
enum TMaxServicePeriodLength
    {
    EMaxServicePeriodLengthAll  = 0x00,
    EMaxServicePeriodLengthTwo  = 0x20,
    EMaxServicePeriodLengthFour = 0x40,
    EMaxServicePeriodLengthSix  = 0x60,
    };

// -------------------------------------------------------------------------

/** data frame statistics per Access Category */
typedef struct _TAccessCategoryStatistics
    {
    /** nbr of received unicast data frames */
    TUint rxUnicastDataFrameCount;
    /** nbr of successfully transmitted unicast data frames */
    TUint txUnicastDataFrameCount;
    /** nbr of received multicast data frames */
    TUint rxMulticastDataFrameCount;
    /** nbr of successfully transmitted multicast data frames */
    TUint txMulticastDataFrameCount;
    /** nbr of data frame transmit retries */
    TUint txRetryCount;
    /** nbr of data frames that could not be delivered to the WLAN AP/STA */
    TUint txErrorCount;
    /** 
    * average data frame Transmit / Media Delay in microseconds.
    * Zero if no frames were transmitted.
    */
    TUint txMediaDelay;
    /** 
    * average data frame Total Transmit Delay in microseconds. 
    * Zero if no frames were transmitted.
    */
    TUint totalTxDelay;
    /** nbr of data frames whose total transmit delay was <= 10ms */
    TUint totalTxDelayBin0;
    /** nbr of data frames whose total transmit delay was ]10,20]ms */
    TUint totalTxDelayBin1;
    /** nbr of data frames whose total transmit delay was ]20,40]ms */
    TUint totalTxDelayBin2;
    /** nbr of data frames whose total transmit delay was > 40ms */
    TUint totalTxDelayBin3;
    } TAccessCategoryStatistics;

// -------------------------------------------------------------------------

typedef struct _TPairwiseKeyData
    {
    /** 
    * length of the data block. 
    * If zero, the other fields in this struct are not relevant. 
    */
    TUint32 length;
    /** data block that holds the cipher key */
    TUint8 data[KMaxCipherKeyLength];
    /** index of the key */
    TUint8 keyIndex;
    } TPairwiseKeyData;
        
// -------------------------------------------------------------------------

    
/**
* Common header for all messages.
*/
#pragma pack(4)
typedef struct _TOIDHeader
    {
    /** OID identification (TWlanCommandId). */
    TUint32 oid_id;
    } TOIDHeader;
#pragma pack()


// -------------------------------------------------------------------------

typedef struct _TConnectMsg
    {
    /** Common message header */
    TOIDHeader hdr;
    /** Name of the network. */
    TSSID SSID;
    /** BSSID of the access point / IBSS network. */
    TMacAddress BSSID;
    /** Authentication algorithm number to be used */
    TUint16 authAlgorithmNbr;
    /** Used encryption mode */
    TEncryptionStatus encryptionStatus;
    /** Pairwise cipher to use; if any */
    TWlanCipherSuite pairwiseCipher;
    /** 
    * ETrue when connecting to an infrastructure network; 
    * EFalse otherwise. 
    */
    TBool isInfra;
    /** 
    * Pointer to a Beacon or Probe Response frame body from the network
    * to connect to
    */
    const TUint8* scanResponseFrameBody;
    /** Length of the Beacon or Probe Response frame body */
    TUint16 scanResponseFrameBodyLength;
    /** 
    * Pointer to the IE(s) to be included into the (re-)association request.
    * NULL if there are no IE(s) to be included.
    */
    const TUint8* ieData;
    /** Length of the IEs */
    TUint16 ieDataLength;
    /** 
    * ETrue if the current pairwise cipher key should be marked as invalid
    * EFalse if the current pairwise cipher key should not be marked as invalid
    * If a pairwise key doesn't exist, this value has no effect
    **/
    TBool invalidatePairwiseKey;
    /** 
    * ETrue if the current group cipher key should be marked as invalid
    * EFalse if the current group cipher key should not be marked as invalid
    * If a group key doesn't exist, this value has no effect
    **/
    TBool invalidateGroupKey;
    /**
     * ETrue if Radio Measurements are on
     */
    TBool radioMeasurement;
    /** pairwise key data; if relevant */
    TPairwiseKeyData pairwiseKey;
    } TConnectMsg;

// -------------------------------------------------------------------------

typedef struct _TStartIBSSMsg
    {
    /** Common message header */
    TOIDHeader hdr;
    /** Name of the network. */
    TSSID SSID;
    /** Beacon period in TUs (kusec). */
    TUint32 beaconInterval;
    /** ATIM window. */
    TUint32 ATIM;
    /** Used channel (1-14). Has to be legal in the current region. */
    TUint32 channel;
    /**
    * Used encryption mode.
    * [EEncryptionWep|EEncryptionDisabled]
    */
    TEncryptionStatus encryptionStatus;
    } TStartIBSSMsg;

// -------------------------------------------------------------------------

typedef struct _TScanMsg
    {
    /** Common message header */
    TOIDHeader hdr;
    /** Scan mode [active|passive]. */
    TScanMode mode;
    /** Network whose APs are scanned. Can be broadcast SSID. */
    TSSID SSID;
    /** Rate that is used in active scanning. */
    TRate scanRate;
    /** Channel set for scanning */
    SChannels channels;
    /** Min. time to listen beacons/probe responses on a channel. */
    TUint32 minChannelTime;
    /** Max. time to listen beacons/probe responses on a channel. */
    TUint32 maxChannelTime;
    /** ETrue if split scan shall be used; EFalse otherwise */
    TBool splitScan;
    } TScanMsg;

// -------------------------------------------------------------------------

typedef struct _TStopScanMsg
    {
    /** Common message header */
    TOIDHeader hdr;
    } TStopScanMsg;

// -------------------------------------------------------------------------

typedef struct _TDisconnectMsg
    {
    /** Common message header */
    TOIDHeader hdr;
    } TDisconnectMsg;

// -------------------------------------------------------------------------

typedef struct _TSetPowerModeMsg
    {
    /** Common message header */
    TOIDHeader hdr;
    /** 
    * 802.11 Power management mode. If the mode is EPowerModePs, we start
    * with Light PS 
    */
    TPowerMode powerMode;
    /** 
    * This setting is relevant only if powerMode is EPowerModePs.
    * If ETrue, disables the dynamic power mode management handling in UMAC.
    * If EFalse, allows the dynamic power mode management handling in UMAC.
    */
    TBool disableDynamicPowerModeManagement;
    
    /** 
    * WLAN wake-up mode in Light PS mode
    */
    TWlanWakeUpInterval wakeupModeInLightPs;
    /** 
    * Specifies the value of N for wake-up modes 2 and 3 in Light PS mode. 
    * So, is relevant only for wake-up modes 2 & 3 
    */
    TUint8 listenIntervalInLightPs;
    
    /** 
    * WLAN wake-up mode in Deep PS mode
    */
    TWlanWakeUpInterval wakeupModeInDeepPs;
    /** 
    * Specifies the value of N for wake-up modes 2 and 3 in Deep PS mode. 
    * So, is relevant only for wake-up modes 2 & 3 
    */
    TUint8 listenIntervalInDeepPs;
    } TSetPowerModeMsg;

// -------------------------------------------------------------------------
typedef struct _TSetRcpiTriggerLevelMsg
    {
    /** Common message header */
    TOIDHeader hdr;
    /** RCPI trigger level. */
    TInt32 RcpiTrigger;
    } TSetRcpiTriggerLevelMsg;

// -------------------------------------------------------------------------

typedef struct _TSetTxPowerLevelMsg
    {
    /** Common message header */
    TOIDHeader hdr;
    /**
    * Transmission power level in dBm.
    */
    TUint32 level;
    } TSetTxPowerLevelMsg;

// -------------------------------------------------------------------------

typedef struct _TConfigureMsg
    {
    /** Common message header */
    TOIDHeader hdr;
    /** 
    * If a bit in this mask is set, use of the corresponding WLAN 
    * feature is allowed; otherwise it is not allowed.
    * Note that this mask doesn't contain all supported WLAN features
    * but only the ones which we allow to be be configured on or off. 
    */
    TWlanFeatures allowedWlanFeatures;    
    /** Limit for packet size when to use RTS/CTS protocol. */
    TUint16 RTSThreshold;
    /** Max. time to (re-)send whole MSDU packet (in TUs) */
    TUint32 maxTxMSDULifetime;
    /**
    * When U-APSD power save mode is used for Voice AC, we are not in Voice
    * over WLAN Call state and we transmit at least voiceCallEntryTxThreshold
    * Voice priority frames during the time period (microseconds) denoted by
    * this parameter, we enter Voice over WLAN Call state
    */
    TUint32 voiceCallEntryTimeout;
    /** 
    * Threshold value for the number of Voice priority frames to enter
    * Voice over WLAN Call state. (See also the voiceCallEntryTimeout 
    * parameter)
    */
    TUint32 voiceCallEntryTxThreshold;
    /** 
    * The time interval (in microseconds) between QoS Null Data frames,
    * which we send during a Voice over WLAN Call in U-APSD power save
    * mode, if there are no other frames to be transmitted 
    */
    TUint32 voiceNullTimeout;
    /** 
    * If there are no Voice priority frames transmitted during the duration
    * of this timer (microseconds), we assume that the Voice over WLAN
    * Call has ended and we will stop sending the QoS Null Data frames
    * in U-APSD power save mode 
    */
    TUint32 noVoiceTimeout;
    /** 
    * The time interval (in microseconds) between Null Data frames,
    * which we send to the AP in infrastructure mode to keep the WLAN 
    * connection alive, if there are no other frames to be transmitted 
    */
    TUint32 keepAliveTimeout;
    /** 
    * If this RCPI level is predicted to be reached within the time
    * specified by spTimeToCountPrediction, a signal loss prediction
    * indication is sent. 
    */
    TUint32 spRcpiIndicationLevel;
    /** 
    * Specifies the time (in microseconds) how far into the future signal
    * prediction is done.
    */
    TUint32 spTimeToCountPrediction;
    /** 
    * The minimum time difference (in microseconds) between two signal
    * loss prediction indications.
    */
    TUint32 spMinIndicationInterval;
    } TConfigureMsg;

// -------------------------------------------------------------------------

typedef struct _TGetLastRcpiMsg
    {
    /** Common message header */
    TOIDHeader hdr;
    } TGetLastRcpiMsg;

// -------------------------------------------------------------------------

typedef struct _TDisableUserDataMsg
    {
    /** Common message header */
    TOIDHeader hdr;
    } TDisableUserDataMsg;

// -------------------------------------------------------------------------

typedef struct _TEnableUserDataMsg
    {
    /** Common message header */
    TOIDHeader hdr;
    } TEnableUserDataMsg;

// -------------------------------------------------------------------------

typedef struct _TAddCipherKeyMsg
    {
    /** Common message header */
    TOIDHeader hdr;
    /** cipher suite */
    TWlanCipherSuite cipherSuite;
    /** index of the key */
    TUint8 keyIndex;
    /** data blob that holds the cipher key */
    TUint8 data[KMaxCipherKeyLength];
    /** length of the data blob */
    TUint32 length;
    /** defines the MAC address the key is used for */
    TMacAddress macAddress;
    /** Is the key used as a default key. Applies to broadcast wep keys */
    TBool useAsDefaultKey;
    } TAddCipherKeyMsg;

// -------------------------------------------------------------------------

typedef struct _TAddMulticastAddrMsg
    {
    /** Common message header */
    TOIDHeader hdr;
    /** 
    * Defines the multicast MAC address to be added. 
    * Note that this will also automatically enable multicast filtering,
    * if it hasn't already been enabled by an earlier TAddMulticastAddrMsg.
    */
    TMacAddress macAddress;
    } TAddMulticastAddrMsg;

// -------------------------------------------------------------------------

typedef struct _TRemoveMulticastAddrMsg
    {
    /** Common message header */
    TOIDHeader hdr;
    /** 
    * If ETrue, remove all the currently specified multicast addresses.
    * Otherwise remove only macAddress (included in this message).
    * Note that if there are no multicast addresses left after this removal,
    * the multicast filtering will be disabled and all multicast packets will
    * again be received and accepted.
    */
    TBool removeAll;
    /** Defines the multicast MAC address to be removed */
    TMacAddress macAddress;
    } TRemoveMulticastAddrMsg;

// -------------------------------------------------------------------------

typedef struct _TConfigureBssLostMsg
    {
    /** Common message header */
    TOIDHeader hdr;
    /**
    * The number of consecutive beacons that can be lost in infrastructure 
    * mode before BSS Lost is indicated
    */
    TUint32 beaconLostCount;
    /**  
    * The number of consecutive transmissions that can fail totally before 
    * BSS lost is indicated
    */
    TUint8 failedTxPacketCount;
    } TConfigureBssLostMsg;

// -------------------------------------------------------------------------

typedef struct _TSetTxRateAdaptationParamsMsg
    {
    /** Common message header */
    TOIDHeader hdr;
    /*
    * minimum and initial rate increase checkpoint in units of frames
    * Range: [stepDownCheckpoint,maxStepUpCheckpoint]
    */
    TUint8  minStepUpCheckpoint;
    /*
    * maximum rate increase checkpoint in units of frames
    * Range: [stepDownCheckpoint,UCHAR_MAX]
    */
    TUint8  maxStepUpCheckpoint;
    /*
    * rate increase checkpoint is multiplied with this value if sending a 
    * probe frame fails
    * Range: [1,maxStepUpCheckpoint]
    */
    TUint8  stepUpCheckpointFactor;
    /*
    * after this many frames the need to decrease the rate is checked
    * Range: [2,UCHAR_MAX]
    */
    TUint8  stepDownCheckpoint;
    /*
    * minimum and initial rate increase threshold percentage
    * Range: [1,maxStepUpThreshold]
    */
    TUint8  minStepUpThreshold;
    /*
    * maximum rate increase threshold percentage value
    * Range: [1,100]
    */
    TUint8  maxStepUpThreshold;
    /*
    * rate increase threshold is incremented by this value if sending a probe 
    * frame fails
    * Range: [0,maxStepUpThreshold]
    */
    TUint8  stepUpThresholdIncrement;        
    /*
    * rate decrease threshold percentage
    * Range: [1,100]
    */
    TUint8  stepDownThreshold;
    /*
    * if EFalse, the rate adaptation algorithm handles the first frame 
    * transmitted after a rate increase in a special way. Otherwise the
    * special handling is disabled
    */
    TBool   disableProbeHandling;
    } TSetTxRateAdaptationParamsMsg;

// -------------------------------------------------------------------------

typedef struct _TConfigureTxRatePoliciesMsg
    {
    /** Common message header */
    TOIDHeader hdr;
    /*
    * defines the rate class(es) to use for frame Tx.
    * If WLAN PDD does not support as many rate classes as are specified
    * in this command message, only the rate class specified for ELegacy 
    * queue, which shall be the first class in this array, will be used.
    */
    TTxRatePolicy ratePolicy;
    /*
    * for every Tx queue / QoS Access Category, defines the rate class to
    * be used. 
    * TQueueId is used to index this array.
    */
    TQueue2RateClass queue2RateClass;
    /*
    * Max Tx rate which will be initially used to transmit using the 
    * rate class in question. If the specified rate is not supported, 
    * the next lower supported rate from the rate class will be used 
    * instead.
    * Only the first numOfPolicyObjects values (see TTxRatePolicy) from 
    * the beginning of the array are relevant.
    */
    TInitialMaxTxRate4RateClass initialMaxTxRate4RateClass;
    /** 
    * defines the rate class(es) to use for frame Tx when the WLAN vendor
    * implementation handles Tx rate adaptation.
    * If WLAN PDD does not support as many rate classes as are specified
    * in this command message, only the rate class specified for ELegacy 
    * queue, which shall be the first class in this array, will be used.
    * The mapping defined in queue2RateClass applies also to these classes.
    * The shortRetryLimit and the longRetryLimit values defined as part of 
    * the ratePolicy are relevant also with the auto rate policy. 
    * Only the first numOfPolicyObjects values (see TTxRatePolicy) from 
    * the beginning of the array are relevant.
    */
    TTxAutoRatePolicy autoRatePolicy;
    /** 
    * defines the HT MCS sets to use for frame Tx when communicating
    * with a HT network.
    * If WLAN PDD does not support as many MCS sets as are specified
    * in this command message, only the MCS set specified for ELegacy 
    * queue, which shall be the first MCS set in this array, will be used.
    * The mapping defined in queue2RateClass applies also to these MCS sets.
    * The shortRetryLimit and the longRetryLimit values defined as part of 
    * the ratePolicy are relevant also with the MCS sets. 
    * Only the first numOfPolicyObjects values (see TTxRatePolicy) from 
    * the beginning of the array are relevant.
    */
    THtMcsPolicy htMcsPolicy;
    } TConfigureTxRatePoliciesMsg;

// -------------------------------------------------------------------------

typedef struct _TSetPowerModeMgmtParamsMsg
    {
    /** Common message header */
    TOIDHeader hdr;
    /*
    * time interval in microseconds after which transition from Active mode 
    * to Light PS mode is considered
    */
    TUint32 toLightPsTimeout;
    /*
    * frame count threshold used when considering transition from Active
    * to Light PS mode
    */ 
    TUint16 toLightPsFrameThreshold;
    /*
    * time interval in microseconds after which the frame counter used when 
    * considering transition from Light PS to Active mode is reset
    */
    TUint32 toActiveTimeout;
    /*
    * frame count threshold used when considering transition from Light PS
    * to Active mode
    */
    TUint16 toActiveFrameThreshold;
    /*
    * time interval in microseconds after which transition from Light PS mode 
    * to Deep PS mode is considered
    */
    TUint32 toDeepPsTimeout;
    /*
    * frame count threshold used when considering transition from Light PS
    * to Deep PS mode
    */
    TUint16 toDeepPsFrameThreshold;
    /*
    * received frame payload length (in bytes) threshold in U-APSD network for
    * Best Effort Access Category
    */
    TUint16 uapsdRxFrameLengthThreshold;    
    } TSetPowerModeMgmtParamsMsg;

// -------------------------------------------------------------------------

typedef struct _TConfigurePwrModeMgmtTrafficOverrideMsg
    {
    /** Common message header */
    TOIDHeader hdr;
    /** 
    * The settings here become effective once using the PS mode has been 
    * allowed by WLAN Mgmt Client.
    * When a setting below is ETrue, any amount of Rx or Tx traffic via
    * the AC in question won't cause a change from PS to CAM mode once PS
    * mode has been entered, and traffic via that AC won't make us to 
    * stay in CAM either.
    * Every AC has a separate setting for U-APSD and legacy PS.
    * The U-APSD setting is used if U-APSD is used for the AC in question.
    * Otherwise the corresponding legacy setting is used.
    */
    /** 
    * U-APSD Voice AC setting
    */
    TBool stayInPsDespiteUapsdVoiceTraffic;
    /** 
    * U-APSD Video AC setting
    */
    TBool stayInPsDespiteUapsdVideoTraffic;
    /** 
    * U-APSD Best Effort AC setting
    */
    TBool stayInPsDespiteUapsdBestEffortTraffic;
    /** 
    * U-APSD Background AC setting
    */
    TBool stayInPsDespiteUapsdBackgroundTraffic;
    /** 
    * legacy Voice AC setting
    */
    TBool stayInPsDespiteLegacyVoiceTraffic;
    /** 
    * legacy Video AC setting
    */
    TBool stayInPsDespiteLegacyVideoTraffic;
    /** 
    * legacy Best Effort AC setting
    */
    TBool stayInPsDespiteLegacyBestEffortTraffic;
    /** 
    * legacy Background AC setting
    */
    TBool stayInPsDespiteLegacyBackgroundTraffic;
    } TConfigurePwrModeMgmtTrafficOverrideMsg;

// -------------------------------------------------------------------------

typedef struct _TGetFrameStatisticsMsg
    {
    /** Common message header */
    TOIDHeader hdr;
    } TGetFrameStatisticsMsg;


typedef struct _TStatisticsResponse
    {
    /** data frame statistics per Access Category */    
    TAccessCategoryStatistics acSpecific[EQueueIdMax];
    /** nbr of FCS errors in received MPDUs */
    TUint fcsErrorCount;
    } TStatisticsResponse;

// -------------------------------------------------------------------------

typedef struct _TConfigureUapsdMsg
    {
    /** Common message header */
    TOIDHeader hdr;
    /** 
    * the maximum number of buffered MSDUs and MMPDUs the AP may send during
    * a service period
    */
    TMaxServicePeriodLength maxServicePeriodLength;
    /** 
    * if ETrue the Voice AC is made both trigger and delivery enabled 
    * when connecting to a QoS AP supporting U-APSD 
    */
    TBool uapsdForVoice;
    /** 
    * if ETrue the Video AC is made both trigger and delivery enabled 
    * when connecting to a QoS AP supporting U-APSD 
    */
    TBool uapsdForVideo;
    /** 
    * if ETrue the Best Effort AC is made both trigger and delivery enabled 
    * when connecting to a QoS AP supporting U-APSD 
    */
    TBool uapsdForBestEffort;
    /** 
    * if ETrue the Background AC is made both trigger and delivery enabled 
    * when connecting to a QoS AP supporting U-APSD 
    */
    TBool uapsdForBackground;
    } TConfigureUapsdMsg;

// -------------------------------------------------------------------------

typedef struct _TConfigureTxQueueMsg
    {
    /** Common message header */
    TOIDHeader hdr;
    /* ID of the transmit queue to configure */
    TQueueId queueId;
    /** 
    * The amount of time the queue is allowed to access the WLAN air 
    * interface during one second interval. The unit of the parameter is 
    * 32 microseconds. Value 0 means that the medium time is unlimited.
    */
    TUint16 mediumTime;
    /** 
    * Maximum Transmit MSDU Lifetime to be used for the specified queue. 
    * Overrides the global maxTxMSDULifetime value specified in TConfigureMsg.
    * Unit: TUs
    */
    TUint32 maxTxMSDULifetime;
    } TConfigureTxQueueMsg;

// -------------------------------------------------------------------------

typedef struct _TGetMacAddressMsg
    {
    /** Common message header */
    TOIDHeader hdr;
    } TGetMacAddressMsg;

// -------------------------------------------------------------------------

typedef struct _TConfigureArpIpAddressFilteringMsg
    {
    /** Common message header */
    TOIDHeader hdr;
    /** 
    * if ETrue the filtering will be enabled 
    * if EFalse the filtering will be disabled
    */
    TBool enableFiltering;
    /** 
    * When filtering is enabled and the WLAN device receives an ARP packet 
    * where the target protocol address field does not match this address
    * the packet is dropped. 
    * Note that the address needs to be in big-endian byte order.
    */
    TIpv4Address ipV4Addr;
    } TConfigureArpIpAddressFilteringMsg;
    
// -------------------------------------------------------------------------

typedef struct _TConfigureHtBlockAckMsg
    {
    /** Common message header */
    TOIDHeader hdr;

    static const TUint8 KTid0 = ( 1 << 0 );
    static const TUint8 KTid1 = ( 1 << 1 );
    static const TUint8 KTid2 = ( 1 << 2 );
    static const TUint8 KTid3 = ( 1 << 3 );
    static const TUint8 KTid4 = ( 1 << 4 );
    static const TUint8 KTid5 = ( 1 << 5 );
    static const TUint8 KTid6 = ( 1 << 6 );
    static const TUint8 KTid7 = ( 1 << 7 );

    /**
    * A bit-map containing the block ACK usage status for the tx direction. 
    * Each bit corresponds to a TID, bit 0 corresponding to TID 0.
    * If a bit is set, block ACK can be used for the corresponding TID. The
    * WLAN device may initiate block ACK session with the AP for the TID in 
    * question.
    * If a bit is not set and there already is a block ACK session with the 
    * AP for this TID, the WLAN device should terminate the session.
    */ 
    TUint8 iTxBlockAckUsage;
    /*
    * A bit-map containing the block ACK usage status the rx direction.
    * Each bit corresponds to a TID, bit 0 corresponding to TID 0.
    * If a bit is set, block ACK can be is used for the corresponding TID. The
    * WLAN device may accept block ACK session requests from the AP for this
    * TID.
    * If a bit is not set, block ACK should not be used for the corresponding
    * TID. The WLAN device should reject block ACK session requests from the
    * AP for this TID. If there already is a block ACK session with the AP for
    * this TID, the WLAN device should terminate the session.
    */
    TUint8 iRxBlockAckUsage;
    } TConfigureHtBlockAckMsg;

// -------------------------------------------------------------------------

typedef struct _TConfigureProprietarySnapHdrMsg
    {
    /** Common message header */
    TOIDHeader hdr;
    /** 
    * Otherwise valid received 802.11 Data frames containing this SNAP header
    * are accepted and forwarded to the WLAN Management Client. 
    * Currently only a single SNAP header can be configured. So if this 
    * command is issued several times, the provided SNAP header replaces the
    * previous one. 
    */
    TSnapHeader snapHdr;
    } TConfigureProprietarySnapHdrMsg;
        
    
#endif      // WLAN_OID_MSG_H
