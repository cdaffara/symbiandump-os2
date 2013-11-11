/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Part of WLAN HAL API
*
*/

/*
* %version: 28 %
*/

#ifndef WHA_TYPES_H
#define WHA_TYPES_H

#include <whanamespace.h>

NAMESPACE_BEGIN_WHA

typedef TUint16 TMib;
const TMib KMibDot11StationId                   = 0x1001;
const TMib KMibDot11MaxReceiveLifetime          = 0x1002;
const TMib KMibDot11SlotTime                    = 0x1003;
const TMib KMibDot11GroupAddressesTable         = 0x1004;
const TMib KMibDot11WepDefaultKeyId             = 0x1005;
const TMib KMibDot11CurrentTxPowerLevel         = 0x1006;
const TMib KMibDot11RTSThreshold                = 0x1007;
const TMib KMibCtsToSelf                        = 0x1101;
const TMib KMibArpIpAddressTable                = 0x1102;
const TMib KMibTemplateFrame                    = 0x1103;
const TMib KMibRxFilter                         = 0x1104;
const TMib KMibBeaconFilterIeTable              = 0x1105;
const TMib KMibBeaconFilterEnable               = 0x1106;
const TMib KMibSleepMode                        = 0x1107;
const TMib KMibWlanWakeUpInterval               = 0x1108;
const TMib KMibBeaconLostCount                  = 0x1109;
const TMib KMibRcpiThreshold                    = 0x110A;
const TMib KMibStatisticsTable                  = 0x110B;
const TMib KMibIbssPsConfig                     = 0x110C; /** deprecated */
const TMib KMibTxRatePolicy                     = 0x110D;
const TMib KMibCountersTable                    = 0x110E;
const TMib KMibDot11Preamble                    = 0x110F;
const TMib KMibEtherTypeFilter                  = 0x1110;
const TMib KMibBroadcastUdpFilter               = 0x1111;

const TMib KMibHtCapabilities                   = 0x2000;
const TMib KMibHtBssOperation                   = 0x2001;
const TMib KMibHtSecondaryBeacon                = 0x2002;
const TMib KMibHtBlockAckConfigure              = 0x2003;
const TMib KMibTxAutoRatePolicy                 = 0x2004;


typedef TInt32  TPowerLevel; 
typedef TUint8  TRcpi;
typedef TUint8  TBand;
typedef TUint32 TChannelNumber;
typedef TUint32 TPltType;
typedef TUint32 TRate; 
typedef TUint32 TPacketId;

typedef TUint8  TPsMode;
typedef TUint32 TIbssPsMode; /** deprecated */
typedef TUint8  TPrivacyKeyId;
typedef TUint32 TTemplateType;
typedef TUint32 TSleepMode;
typedef TUint8  TWlanWakeUpInterval;
typedef TUint32 TIpv4Address;
typedef TUint32 TStatus;
typedef TUint8  TSlotTime;

typedef TUint8  THtSecChannelOffset;
typedef TUint8  THtChannelWidth;
typedef TUint8  THtPcoPhase;

const   TUint8  KHtMcsSetLength = 10;
typedef TUint8  THtMcsSet[KHtMcsSetLength];

typedef TUint8  THtProtection;
typedef TUint8  THtPpduFormat;
typedef TUint8  THtRxStbc;
typedef TUint8  THtMaxAmsdu;
typedef TUint8  THtMaxAmpdu;
typedef TUint8  THtAmpduSpacing;
typedef TUint8  THtPcoTransTime;
typedef TUint8  THtSecBeacon;
typedef TUint8  THtMcsFeedback;
typedef TUint8  TPreambleType;
typedef TUint32 THtCapabilities;
typedef TUint8  TFilterMode;

const TBand KBand2dot4GHzMask   = ( 1 << 0 );
const TBand KBand4dot9GHzMask   = ( 1 << 1 );
const TBand KBand5GHzMask       = ( 1 << 2 );

/**
* 802.11 WEP key ID values
*/
const TPrivacyKeyId KPrivacyKeyId0      = 0;
const TPrivacyKeyId KPrivacyKeyId1      = 1;
const TPrivacyKeyId KPrivacyKeyId2      = 2;
const TPrivacyKeyId KPrivacyKeyId3      = 3;    
const TPrivacyKeyId KPrivacyKeyIdMax    = 4; // defined as an upper bound

enum TQueueId
    {
    ELegacy,
    EBackGround,
    EVideo,
    EVoice,
    EHcca,          /** deprecated */
    EQueueIdMax     // defined as upper bound
    };

const TUint32 KSlotTime20  = 20;
const TUint32 KSlotTime9   = 9;


const TUint KBeaconStorageSize = 256;
const TUint KProbeRequestStorageSize = KBeaconStorageSize;
const TUint KProbeResponseStorageSize = KBeaconStorageSize;

const TTemplateType KBeaconTemplate         = 0;
const TTemplateType KProbeRequestTemplate   = 1;
const TTemplateType KNullDataTemplate       = 2;
const TTemplateType KProbeResponseTemplate  = 3;
const TTemplateType KQosNullDataTemplate    = 4;
const TTemplateType KPSPollTemplate         = 5;

const TSleepMode KAwakeMode     = 0;
const TSleepMode KPowerDownMode = 1;
const TSleepMode KLowPowerMode  = 2;

const TWlanWakeUpInterval KWakeUpIntervalAllBeacons     = 0;
const TWlanWakeUpInterval KWakeUpIntervalAllDtims       = 1;
const TWlanWakeUpInterval KWakeUpIntervalEveryNthBeacon = 2;
const TWlanWakeUpInterval KWakeUpIntervalEveryNthDtim   = 3;

const TIbssPsMode KIbssStandardPsMode = 0; /** deprecated */
const TIbssPsMode KIbssEnhancedPsMode = 1; /** deprecated */

const THtSecChannelOffset KNoSecCh          = 0;
const THtSecChannelOffset KSecChAbovePrimCh = 1;
const THtSecChannelOffset KSecChBelowPrimCh = 3;

const THtChannelWidth KHtChWidthOnly20MHz  = 0;
const THtChannelWidth KHtChWidth40And20MHz = 1;

const THtPcoPhase KHtPcoPhase20MHz = 0;
const THtPcoPhase KHtPcoPhase40MHz = 1;

const THtProtection KHtOperationMode0 = 0;
const THtProtection KHtOperationMode1 = 1;
const THtProtection KHtOperationMode2 = 2;
const THtProtection KHtOperationMode3 = 3;

const THtPpduFormat KHtPpduFormatNonHt      = 0;
const THtPpduFormat KHtPpduFormatMixed      = 1;
const THtPpduFormat KHtPpduFormatGreenfield = 2;

const THtRxStbc KHtRxStbcNotSupported        = 0;
const THtRxStbc KHtRxStbcFor1Stream          = 1;
const THtRxStbc KHtRxStbcFor1And2Streams     = 2;
const THtRxStbc KHtRxStbcFor1And2And3Streams = 3;

const THtMaxAmsdu KHtMaxAmsdu3839Octets = 0;
const THtMaxAmsdu KHtMaxAmsdu7935Octets = 1;

const THtMaxAmpdu KHtMaxAmpdu8191Octets  = 0;
const THtMaxAmpdu KHtMaxAmpdu16383Octets = 1;
const THtMaxAmpdu KHtMaxAmpdu32767Octets = 2;
const THtMaxAmpdu KHtMaxAmpdu65535Octets = 3;

const THtAmpduSpacing KHtAmpduSpacingNoRestriction  = 0;
const THtAmpduSpacing KHtAmpduSpacingQuarter_us     = 1;
const THtAmpduSpacing KHtAmpduSpacingHalf_us        = 2;
const THtAmpduSpacing KHtAmpduSpacingOne_us         = 3;
const THtAmpduSpacing KHtAmpduSpacingTwo_us         = 4;
const THtAmpduSpacing KHtAmpduSpacingFour_us        = 5;
const THtAmpduSpacing KHtAmpduSpacingEight_us       = 6;
const THtAmpduSpacing KHtAmpduSpacingSixteen_us     = 7;

const THtPcoTransTime KHtPcoTransTimeNone  = 0;
const THtPcoTransTime KHtPcoTransTime400us = 1;
const THtPcoTransTime KHtPcoTransTime1_5ms = 2;
const THtPcoTransTime KHtPcoTransTime5ms   = 3;

const THtSecBeacon KHtSecBeaconRxPrimaryOnly   = 0;
const THtSecBeacon KHtSecBeaconRxEither        = 1;
const THtSecBeacon KHtSecBeaconRxSecondaryOnly = 2;

const THtMcsFeedback KHtMcsFeedbackNone                    = 0;
const THtMcsFeedback KHtMcsFeedbackUnsolicitedOnly         = 2;
const THtMcsFeedback KHtMcsFeedbackUnsolicitedAndSolicited = 3;

const TPreambleType KLongPreamble  = 0;
const TPreambleType KShortPreamble = 1;

const THtCapabilities KLdpcRx               = ( 1 <<  0 );
const THtCapabilities K40MhzChannel         = ( 1 <<  1 );
const THtCapabilities KGreenfieldFormat     = ( 1 <<  2 );
const THtCapabilities KShortGiFor20Mhz      = ( 1 <<  3 );
const THtCapabilities KShortGiFor40Mhz      = ( 1 <<  4 );
const THtCapabilities KStbcTx               = ( 1 <<  5 );
const THtCapabilities KDelayedBlockAck      = ( 1 <<  6 );
const THtCapabilities KDsssCckIn40Mhz       = ( 1 <<  7 );
const THtCapabilities KPsmp                 = ( 1 <<  8 );
const THtCapabilities KLsigTxopProtection   = ( 1 <<  9 );
const THtCapabilities KPco                  = ( 1 << 10 );
const THtCapabilities KHtcField             = ( 1 << 11 );
const THtCapabilities KReverseDirectionResp = ( 1 << 12 );

const TFilterMode KFilteringDisabled = 0;
const TFilterMode KFilterIn          = 1;
const TFilterMode KFilterOut         = 2;

typedef TIpv4Address TIpV4Addr;  /** deprecated */

const TIpv4Address KZeroIpV4Addr = 0x00000000;

/**
* rate definition masks
*/
const TRate KRate1Mbits       = 0x00000001;
const TRate KRate2Mbits       = 0x00000002;
const TRate KRate5_5Mbits     = 0x00000004;
const TRate KRate6Mbits       = 0x00000008;
const TRate KRate9Mbits       = 0x00000010;
const TRate KRate11Mbits      = 0x00000020;
const TRate KRate12Mbits      = 0x00000040;
const TRate KRate18Mbits      = 0x00000080;
const TRate KRate22Mbits      = 0x00000100;
const TRate KRate24Mbits      = 0x00000200;
const TRate KRate33Mbits      = 0x00000400;
const TRate KRate36Mbits      = 0x00000800;
const TRate KRate48Mbits      = 0x00001000;
const TRate KRate54Mbits      = 0x00002000;

const TRate KDot11gRates      =
    KRate1Mbits | KRate2Mbits | KRate5_5Mbits | KRate6Mbits | KRate9Mbits |
    KRate11Mbits | KRate12Mbits | KRate18Mbits | KRate22Mbits | KRate24Mbits |
    KRate33Mbits | KRate36Mbits | KRate48Mbits | KRate54Mbits;

const TRate KDot11bRates  =
    KRate1Mbits | KRate2Mbits | KRate5_5Mbits | KRate11Mbits;

const TRate KMaxDot11bRate = KRate11Mbits;

/**
* bitmasks for ReceivePacket aFlags member
*/
const TUint32 KEncryptNone      = 0;
const TUint32 KEncryptWapi      = (1 << 15);
const TUint32 KEncryptWep       = (1 << 16);
const TUint32 KEncryptTkip      = (1 << 17);
const TUint32 KEncryptAes       = (3 << 16);
const TUint32 KMoreFrames       = (1 << 18);
const TUint32 KDuringMeasure    = (1 << 19); /** deprecated */
const TUint32 KHtPacket         = (1 << 20);
const TUint32 KPartOfAmpdu      = (1 << 21);
const TUint32 KStbc             = (1 << 22);

struct STxPowerRange
    {
    TPowerLevel     iMinPowerLevel;
    TPowerLevel     iMaxPowerLevel;
    TUint32         iStepping;
    };

struct SHtCapabilities
    {
    THtCapabilities iHTCapabilitiesBitMask;
    TUint16         iRxMaxDataRate;
    THtChannelWidth iChannelWidth;
    THtRxStbc       iRxStbc;
    THtMaxAmsdu     iMaxAmsdu;
    THtMaxAmpdu     iMaxAmpdu;
    THtAmpduSpacing iAmpduSpacing;
    THtMcsSet       iRxMcs;
    THtMcsSet       iTxMcs;
    THtPcoTransTime iPcoTransTime;
    THtMcsFeedback  iMcsFeedback;
    };

struct SSettings
    {
    static const TUint KNumOfBandsSupported = 3;

    TUint8      iNumOfSSIDs;    
    TUint8      iRxBufAlignment; /** deprecated */
    TUint16     iNumOfBytesForSsid;
    TRate       iRates;
    TBand       iBand;
    TUint8      iRxoffset;
    TUint8      iNumOfGroupTableEntrys;
    TUint8      iNumOfTxRateClasses;
    TUint8      iTxFrameTrailerSpace;
    TUint8      iTxFrameHeaderSpace;
    TUint16     iFlagsMask; /** deprecated */
    STxPowerRange iTxPowerRange[KNumOfBandsSupported];

    enum { KMaxReceiveLifeTime  = ( 1 << 0 ) };
    enum { KIbssPsConfig        = ( 1 << 1 ) }; /** deprecated */
    enum { KTruncate            = ( 1 << 2 ) };
    enum { KPreambleOverride    = ( 1 << 3 ) };
    enum { KPowerLevelParam     = ( 1 << 4 ) };
    enum { KExpiryTimeParam     = ( 1 << 5 ) };
    enum { KProbe4Join          = ( 1 << 6 ) };
    enum { KMaxLifeTime         = ( 1 << 7 ) };
    enum { KTxNoAckPolicy       = ( 1 << 8 ) };
    enum { KBlockAckPolicy      = ( 1 << 9 ) }; /** deprecated */
    enum { KDot11SlotTime       = ( 1 << 10 ) };
    enum { KWmmSa               = ( 1 << 11 ) }; /** deprecated */
    enum { KSapsd               = ( 1 << 12 ) }; /** deprecated */
    enum { KRadioMeasurements   = ( 1 << 13 ) }; /** deprecated */
    enum { KLegacyPsPoll        = ( 1 << 14 ) }; /** deprecated */
    enum { KWep16ByteKey        = ( 1 << 15 ) }; /** deprecated */
    enum { KMoreDataAck         = ( 1 << 16 ) };
    enum { KScanChannelTimes    = ( 1 << 17 ) };
    enum { KAutonomousRateAdapt = ( 1 << 18 ) };
    enum { KNoSecHdrAndTrailer  = ( 1 << 19 ) };
    enum { KHtOperation         = ( 1 << 20 ) };
    enum { KWapi                = ( 1 << 21 ) };
    enum { KDsParamSetIeInProbe = ( 1 << 22 ) };
    enum { KEtherTypeFilter     = ( 1 << 23 ) };
    enum { KBroadcastUdpFilter  = ( 1 << 24 ) };
    enum { KMultipleRxBuffers   = ( 1 << 25 ) };

    TUint32         iCapability;
    SHtCapabilities iHtCapabilities;
    };

struct SConfigureDataBase
    {   
    // currently this is the only one defined
    TUint32 iDot11MaxTransmitMsduLifeTime;     
    };

// frequencies for 2.4 GHz band in units of KHz
const TUint32 K2dot4ChannelFrequencies[] = 
    {
    2412000, // Channel 1
    2417000, // Channel 2
    2422000, // Channel 3
    2427000, // Channel 4
    2432000, // Channel 5
    2437000, // Channel 6
    2442000, // Channel 7
    2447000, // Channel 8
    2452000, // Channel 9
    2457000, // Channel 10
    2462000, // Channel 11
    2467000, // Channel 12
    2472000, // Channel 13
    2484000  // Channel 14
    };
    
struct SChannelLoadParams /** deprecated */
    {
    };

struct SNoiseHistogramParams /** deprecated */
    {
    };

struct SBeaconParams /** deprecated */
    {
    enum { KPassive         = 0 };
    enum { KPassivePilot    = 1 };
    enum { KActive          = 2 };
    enum { KStaSelected     = 3 };
    enum { KBeaconTable     = 4 };

    TUint32                  iScanMode;
    };

union UMeasurementSpecific /** deprecated */
    {
    SBeaconParams           iBeaconParams;
    SChannelLoadParams      iChannelLoadParams;
    SNoiseHistogramParams   iNoiseHistogramParams;
    };

struct SParameterSet /** deprecated */
    {
    enum { KMeasTypeBasic           = 0 };
    enum { KMeasTypeChannelLoad     = 1 };
    enum { KMeasTypeNoiseHistogram  = 2 };
    enum { KMeasTypeBeacon          = 3 };
    enum { KMeasTypeFrame           = 4 };

    TUint32                 iType; 
    TUint32                 iDuration;
    TUint32                 iReserved;
    UMeasurementSpecific    iMeasurementSpecific;
    };

struct SChannels
    {
    TChannelNumber  iChannel;
    TUint32         iMinChannelTime;
    TUint32         iMaxChannelTime;
    TPowerLevel     iTxPowerLevel;
    };

struct SSSID
    {
    enum { KMaxSSIDLength = 32 };

    TUint32 iSSIDLength;
    TUint8  iSSID[KMaxSSIDLength];
    };

enum TScanType
    {
    EFgScan,
    EBgScan,
    EForcedBgScan
    };

enum TOperationMode
    {
    EIBSS,
    EBSS
    };

#pragma pack(1)
struct TMacAddress
    {
    enum { KMacAddressLength = 6 };

    /** the MAC address */
    TUint8 iMacAddress[KMacAddressLength];
    }; // 6 bytes
#pragma pack()    

/**
* Broadcast MAC Address.
*/
const TMacAddress KBroadcastMacAddr = 
    {
        { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }
    };

/**
* MAC address that is all zeros
*/
const TMacAddress KZeroMacAddr = 
    {
        { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
    };

struct SWepPairwiseKey
    {    
    enum { KHeaderSize = sizeof( TMacAddress ) + ( 2 * sizeof( TUint8 ) ) };

    TMacAddress     iMacAddr;
    TUint8          iReserved;
    TUint8          iKeyLengthInBytes;
    TUint8          iKey[1];
    };

struct SWepGroupKey
    {
    enum { KHeaderSize = sizeof( TPrivacyKeyId ) + ( 3 * sizeof( TUint8 ) ) };
    
    TPrivacyKeyId   iKeyId;
    TUint8          iKeyLengthInBytes;
    TUint8          iReserved[2];
    TUint8          iKey[1];
    };

/**
* Length of TKIP key in bytes;
*/
const TUint32 KTKIPKeyLength = 16;
/**
* Length of MIC in BYTEs
*/
const TUint32 KMicLength = 8;

struct STkipPairwiseKey
    {
    TMacAddress     iMacAddr;
    TUint8          iReserved[2];
    TUint8          iTkipKey[KTKIPKeyLength];
    TUint8          iRxMicKey[KMicLength];
    TUint8          iTxMicKey[KMicLength];
    TPrivacyKeyId   iKeyId;
    TUint8          iReserved2[3];
    };

/**
* Length of receive sequence counter in bytes
*/
const TUint32 KRxSequenceCounterLength = 8;

struct STkipGroupKey
    {
    TUint8          iTkipKey[KTKIPKeyLength];
    TUint8          iRxMicKey[KMicLength];
    TPrivacyKeyId   iKeyId;
    TUint8          iReserved[3];
    TUint8          iRxSequenceCounter[KRxSequenceCounterLength];
    };

/**
* Length of AES key in bytes;
*/
const TUint32 KAesKeyLength = 16;

struct SAesPairwiseKey
    {
    TMacAddress     iMacAddr;
    TUint8          iReserved[2];
    TUint8          iAesKey[KAesKeyLength];
    };

struct SAesGroupKey
    {
    TUint8          iAesKey[KAesKeyLength];
    TPrivacyKeyId   iKeyId;
    TUint8          iReserved[3];
    TUint8          iRxSequenceCounter[KRxSequenceCounterLength];
    };

/**
* Length of WAPI encryption key in bytes;
*/
const TUint32 KWapiKeyLength = 16;
/**
* Length of WAPI MIC key in bytes
*/
const TUint32 KWapiMicKeyLength = 16;

struct SWapiPairwiseKey
    {
    TMacAddress     iMacAddr;
    TPrivacyKeyId   iKeyId;
    TUint8          iReserved[1];
    TUint8          iWapiKey[KWapiKeyLength];
    TUint8          iMicKey[KWapiMicKeyLength];
    };

struct SWapiGroupKey
    {
    TUint8          iWapiKey[KWapiKeyLength];
    TUint8          iMicKey[KWapiMicKeyLength];
    TPrivacyKeyId   iKeyId;
    TUint8          iReserved[3];
    };

enum TPreamble
    {
    ELongPreamble,
    EShortPreamble
    };

const TPsMode KPsDisable = 0;
const TPsMode KPsEnable = 1;


enum TParameterSet
    {
    EParameterSetPlaceHolder
    };

enum TKeyType
    {
    EWepGroupKey,
    EWepPairWiseKey,
    ETkipGroupKey,    
    ETkipPairWiseKey,      
    EAesGroupKey,    
    EAesPairWiseKey,      
    EWapiGroupKey,
    EWapiPairWiseKey,
    EKeyNone        // defined as an empty type
    };

enum TPsScheme
    {
    ERegularPs,
    EUapsd,
    ELegacyPs, /** deprecated */
    ESapsd     /** deprecated */
    };

struct SSAPSDConfig /** deprecated */
    {
    TUint32 iServiceStartTime;  // Lower order of TSF
    TUint32 iServiceInterval;   // In microseconds
    };

enum TAckPolicy
    {
    ENormal,
    ENoaAck,
    EBlockAck /** deprecated */
    };

const TStatus KSuccess                  = 0;
const TStatus KFailed                   = 1;
const TStatus KDecryptFailure           = 2;
const TStatus KMicFailure               = 3;
const TStatus KSuccessXfer              = 4;
const TStatus KPending                  = 5;
const TStatus KQueueFull                = 6;
const TStatus KErrorRetryExceeded       = 7;
const TStatus KErrorLifetimeExceeded    = 8;
const TStatus KErrorNoLink              = 9;
const TStatus KErrorMacNotResponding    = 10;
const TStatus KSuccessQueueFull         = 11;


enum TCommandId
    {
    EInitializeResponse,
    EConfigureResponse,
    EReleaseResponse,
    EPLtResponse,
    EResetResponse,
    EMeasureCommandResponse,   /** deprecated */
    EStopMeasureResponse,      /** deprecated */
    EScanCommandResponse,
    EStopScanResponse,
    EJoinResponse,
    ESetPsModeCommandResponse,
    ESetBssParametersResponse,
    EReadMIBResponse,
    EWriteMIBResponse,
    EAddKeyResponse,
    ERemoveKeyResponse,
    EConfigureQueueResponse,
    EConfigureACResponse,
    };

enum TCompleteCommandId
    {
    EMeasureComplete, /** deprecated */
    EScanComplete,
    ESetPsModeComplete
    };

struct SJoinResponse
    {
    TPowerLevel iMinPowerLevel;
    TPowerLevel iMaxPowerLevel;
    };

struct SReadMibResponse
    {
    TMib        iMib;
    TUint16     iLength;
    const void* iData;
    };

union UCommandResponseParams
    {
    SJoinResponse           iJoinResponse;
    SReadMibResponse        iReadMibResponse;
    };

struct SChannelLoadResult /** deprecated */
    {
    TUint32 iCCABusyFraction;  
    };

struct SNoiseHistogramResult /** deprecated */
    {
    enum { KResultSize = 8 };

    TUint8 iRPIDensity[KResultSize];
    };

struct SFrameReport /** deprecated */
    {
    TMacAddress iTransmitAddress;
    TUint8      iReserved1[2];
    TMacAddress iBssId;
    TUint8      iReserved2[2];
    TPowerLevel iRxSignalPower;
    TUint32     iNumOfFrames;
    };

struct SFrameResultReport /** deprecated */
    {
    TUint32         iNumberOfResults;
    SFrameReport    iFrameReport[1];
    };

union UMeasurementSpecificResult /** deprecated */
    {
    SChannelLoadResult      iChannelLoadResult;
    SNoiseHistogramResult   iNoiseHistogramResult;
    SFrameResultReport      iFrameResultReport;
    };

struct SResultSet /** deprecated */
    {
    TUint32 iType;
    TStatus iStatus;

    UMeasurementSpecificResult  iMeasurementSpecificResult;
    };

struct SMeasureComplete /** deprecated */
    {
    TPsMode     iDot11PowerManagementMode;
    TUint8      iNumberOfMeasurementTypes;
    TUint8      iReserved[2];
    SResultSet  iResultSet[1];
    };

struct SScanComplete
    {
    TPsMode iDot11PowerManagementMode;
    TUint8  iReserved[3];
    };

struct SSetPsModeComplete
    {
    TPsMode iDot11PowerManagementMode;
    TUint8  iReserved[3];
    };

union UCommandCompletionParams
    {
    SMeasureComplete        iMeasureComplete;
    SScanComplete           iScanComplete;
    SSetPsModeComplete      iSetPsModeComplete;
    };

enum TIndicationId
    {
    EError,
    EBssLost,
    EBSSRegained,
    ERadar, /** deprecated */
    ERcpi,
    EPsModeError
    };

struct SError
    {
    TStatus iStatus;
    };

enum TMicKeyType
    {
    EPairWise,
    EGroup
    };

struct SRcpi
    {
    TRcpi   iRcpi;
    TUint8  iReserved[3];
    };

union UIndicationParams
    {
    SError          iError;
    SRcpi           iRcpi;
    };

NAMESPACE_END_WHA

#endif // WHA_TYPES_H

