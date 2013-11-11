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
* Description:   UMAC type definitions. 
*
*/

/*
* %version: 30 %
*/

#ifndef UMACTYPES_H
#define UMACTYPES_H

#include "am_platform_libraries.h"
#include "pack.h"
#include <wlanosaplatform.h>


/** WLAN feature bit mask */
typedef TUint32 TWlanFeatures;

/** HT operation (per 802.11n) */
const TWlanFeatures KWlanHtOperation = ( 1 << 0 );


/** Indication data type */
typedef enum _TIndication
    {
	EMediaDisconnect,
    EOsPowerStandby,
    EHWFailed,
    EConsecutiveBeaconsLost,
    EConsecutiveTxFailures,
    EConsecutivePwrModeSetFailures,
    EBSSRegained,
    EWepDecryptFailure,
    EPairwiseKeyMicFailure,
    EGroupKeyMicFailure,
    ERcpiTrigger,
    EScanCompleted,
    ESignalLossPrediction,
    EApTestOpportunity,
    EVoiceCallOn,
    EVoiceCallOff,
    EPsModeError
    } TIndication;

/** Connect failed because of unsupported network configuration */
const TInt KWlanErrUnsupportedNwConf = -333001;
    
/**
* Maximum length of an SSID in BYTES
*/
const TUint8 KMaxSSIDLength = 32;

/**
* Length of a MAC address
*/
const TUint8 KMacAddressLength = 6;

/**
* 802.11 WEP key ID values
*/
enum T802Dot11WepKeyId
    {
    E802Dot11WepKeyId0 = 0,
    E802Dot11WepKeyId1 = 1,
    E802Dot11WepKeyId2 = 2,
    E802Dot11WepKeyId3 = 3,
    // defined as an upper bound
    E802Dot11WepKeyIdMax = 4
    };

/** Tx queues. They have a 1:1 mapping with QoS Access Categories */

enum TQueueId
    {
    ELegacy,
    EBackGround,
    EVideo,
    EVoice,
    EQueueIdMax     // defined as upper bound
    };

/**
 * WHA transmit queue state
 */
enum TTxQueueState
    {
    ETxQueueFull,
    ETxQueueNotFull
    };

/**
* MAC address
*/
#pragma pack( 1 )
struct TMacAddress
    {
    /** the MAC address */
    TUint8 iMacAddress[KMacAddressLength];
    } __PACKED; // 6 bytes

// pop the pragma pack stack to return to normal alignment of structures
#pragma pack( ) 

/**
* Broadcast MAC Address.
*/
const TMacAddress KBroadcastMacAddr = {{ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }};

/**
* MAC address that is all zeros
*/
const TMacAddress KZeroMacAddr = {{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }};

/** IP v4 address */
typedef TUint32 TIpv4Address;

/** Length of OUI field in SNAP header */
const TUint8 KOuiLength = 3;    
    
/** SNAP header */
#pragma pack( 1 )
typedef struct _TSnapHeader
    {
    /** destination service access point */
    TUint8 iDSAP;
    /** source service access point */
    TUint8 iSSAP;
    /** control field */
    TUint8 iControl;
    /** organizationally unique identifier */
    TUint8 iOUI[KOuiLength];    // 3
    } TSnapHeader; // 6 bytes
#pragma pack()

/**
 * state of all WHA transmit queues
 */
typedef TTxQueueState TWhaTxQueueState[EQueueIdMax];


// returns aDest
extern TAny* os_memcpy( 
    TAny* aDest, 
    const TAny* aSrc, 
    TUint32 aLengthinBytes );

// return 0 if equal
extern TInt os_memcmp( 
    const TAny* aLhs, 
    const TAny* aRhs, 
    TUint aNumOfBytes );

extern TAny* os_memset( 
    void* aDest, 
    TInt aValue, 
    TUint aCount );


/** internal UMAC events bitmask type */
typedef TUint32 TInternalEvent;

/** internal UMAC event requiring us to update the AC parameters to 
 *  WHA layer 
 */
const TUint32 KAcParamUpdate = ( 1 << 0 );
/** internal UMAC event requiring us to change the dot11 power mgmt mode */
const TUint32 KPowerMgmtTransition = ( 1 << 1 );
/** internal UMAC default timer timeout event */
const TUint32 KTimeout = ( 1 << 2 );
/** internal UMAC Voice Call Entry timer timeout event */
const TUint32 KVoiceCallEntryTimerTimeout = ( 1 << 3 );
/** internal UMAC Null timer timeout event */
const TUint32 KNullTimerTimeout = ( 1 << 4 );
/** internal UMAC No Voice timer timeout event */
const TUint32 KNoVoiceTimerTimeout = ( 1 << 5 );
/** internal UMAC Keep Alive timer timeout event */
const TUint32 KKeepAliveTimerTimeout = ( 1 << 6 );
/** internal UMAC Active to Light PS timer timeout event */
const TUint32 KActiveToLightPsTimerTimeout = ( 1 << 7 );
/** internal UMAC Light PS to Active timer timeout event */
const TUint32 KLightPsToActiveTimerTimeout = ( 1 << 8 );
/** internal UMAC Light PS to Deep PS timer timeout event */
const TUint32 KLightPsToDeepPsTimerTimeout = ( 1 << 9 );
/** internal UMAC event requiring us to set the cts to self MIB */
const TUint32 KSetCtsToSelf = ( 1 << 10 );
/** internal UMAC event requiring us to set the RCPI trigger level MIB */
const TUint32 KSetRcpiTriggerLevel = ( 1 << 11 );
/** internal UMAC event requiring us to set the HT BSS Operation MIB */
const TUint32 KSetHtBssOperation = ( 1 << 12 );


/** internal WLAN LDD timer type */
enum TWlanTimer
    {
    EWlanDefaultTimer,
    EWlanVoiceCallEntryTimer,
    EWlanNullTimer,
    EWlanNoVoiceTimer,
    EWlanKeepAliveTimer,    
    EWlanActiveToLightPsTimer,
    EWlanLightPsToActiveTimer,
    EWlanLightPsToDeepPsTimer
    };

#endif // UMACTYPES_H
