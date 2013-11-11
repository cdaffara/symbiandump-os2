/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   UMAC internal types and constants
*
*/

/*
* %version: 11 %
*/

#ifndef WLANINTERNALDEFINITIONS_H
#define WLANINTERNALDEFINITIONS_H

#include "am_platform_libraries.h" // basic types
#include "umacoidmsg.h"
#include "802dot11.h"

#ifndef RD_WLAN_DDK
#include <wha.h>
#else
#include <wlanwha.h>
#endif


typedef TUint16 TCwMinVector[WHA::Wha::KNumOfEdcaQueues];
typedef TUint16 TCwMaxVector[WHA::Wha::KNumOfEdcaQueues];
typedef TUint8  TAifsVector[WHA::Wha::KNumOfEdcaQueues];
typedef TUint16 TTxOplimitVector[WHA::Wha::KNumOfEdcaQueues];    
typedef TBool   TAcmVector[WHA::Wha::KNumOfEdcaQueues];

// MPDU SNAP header validation status codes
enum TSnapStatus
    {
    ESnapUnknown,       // unknown SNAP encountered
    ESnapDot11Ok,       // SNAP used by generic data MPDUs encountered
    ESnapProprietaryOk  // vendor specific SNAP encountered
    };

const TUint8 KWmmParamSetNotDefined = 255;                

// DA type
enum TDaType
    {
    EUnicastAddress,
    EMulticastAddress,
    EBroadcastAddress
    };

// Type used to instruct Dynamic 802.11 Pwr Mode Mgt regarding switching from
// PS to CAM mode after frame Tx
enum TDynamicCamSwitch
    {
    ECamSwitchNotForced,
    // if in PS, switch to CAM
    ESwitchToCam,
    // if in PS, stay in PS
    EDontSwitchToCam
    };

enum TPowerMgmtMode
    {
    EActive,
    ELightPs,
    EDeepPs
    };

enum TPowerMgmtModeChange
    {
    ENoChange,
    EToActive,
    EToLightPs,
    EToDeepPs
    };

struct TDot11PsModeWakeupSetting
    {
    /** 
    * WLAN wake-up mode in 802.11 PS mode. 
    */
    TWlanWakeUpInterval iWakeupMode;
    /** 
    * Specifies the value of N for wake-up modes 2 and 3. So, is relevant
    * only for wake-up modes 2 & 3 
    */
    TUint8 iListenInterval;
    };

typedef WHA::TRate TWhaRateMasks[KMaxNbrOfRateClasses];

// Value to denote an undefined SNAP header
const SSnapHeader KUndefinedSnapHeader 
    = { 0x00, 0x00, 0x00, { 0x00, 0x00, 0x00 } };

#endif // WLANINTERNALDEFINITIONS_H
