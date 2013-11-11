/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Defines default constant MIB structures 
*                for default WHA MIB configuration
*
*/

/*
* %version: 19 %
*/

#ifndef WHA_MIBDEFAULTVALUES_H
#define WHA_MIBDEFAULTVALUES_H

#ifndef RD_WLAN_DDK
#include <wha_mib.h>
#else
#include <wlanwha_mib.h>
#endif

NAMESPACE_BEGIN_WHA

// dot11MaxReceiveLifeTime
//
const TUint32 KDot11MaxReceiveLifeTimeDefault = 512;

const Sdot11MaxReceiveLifeTime KDot11MaxReceiveLifeTimeMibDefault 
    = { KDot11MaxReceiveLifeTimeDefault };


// dot11SlotTime
//
const TUint32 KDot11SlotTimeDefault = KSlotTime20;

const Sdot11SlotTime KDot11SlotTimeMibDefault
    = { KDot11SlotTimeDefault };


// dot11GroupAddressesTable
//
const Sdot11GroupAddressesTable KDot11GroupAddressesTableMibDefault = 
    { 
        EFalse,                     // disable     
        0,                          // NumOfAddrs
        0
    }; 


// dot11WepDefaultKeyId
//
const TPrivacyKeyId KDot11WepDefaultKeyId = KPrivacyKeyId0;

const Sdot11WepDefaultKeyId KDot11WepDefaultKeyIdMib
    = { KDot11WepDefaultKeyId };


// dot11CurrentTxPowerLevel
//
const TPowerLevel KDot11CurrentTxPowerLevelDefault = 20;

const Sdot11CurrentTxPowerLevel KDot11CurrentTxPowerLevelMibDefault
    = { KDot11CurrentTxPowerLevelDefault };


// ctsToSelf
//
const TBool KCtsToSelfDefault = EFalse;

const SctsToSelf KCtsToSelfMibDefault
    = { KCtsToSelfDefault };


// arpIpAddressTable
//
const WHA::SarpIpAddressTable KArpIpAddressTableMibDefault = 
    { 
        EFalse, 0
    };    

// rxFilter
//
const TUint32 KRxFilterDefault = 0;

const SrxFilter KRxFilterMibDefault 
    = { KRxFilterDefault };


// beaconFilterIeTable
//
const SbeaconFilterIeTable KBeaconFilterIeTableMibDefault = 
    { 
        0,  // numofElems
        0   // empty IE table
    }; 


// beaconFilterEnable
//
const SbeaconFilterEnable KBeaconFilterEnableMibDefault = 
    { 
        ETrue,  // filtering enabled
        0
    };


// wlanWakeUpInterval
//
const TUint8 KListenIntervalDefault = 1;
const SwlanWakeUpInterval KWlanWakeUpIntervalMibDefault
    = { KWakeUpIntervalAllBeacons,
        KListenIntervalDefault,
        0       // iReserved
      };


// beaconLostCount
//
const TUint32 KBeaconLostCountDefault = 5;

const SbeaconLostCount KBeaconLostCountMibDefault
    = { KBeaconLostCountDefault };


// rcpiThreshold
//
const TRcpi KRcpiThresholdDefault = 0;

const SrcpiThreshold KRcpiThresholdMibDefault
    = { KRcpiThresholdDefault };


// txRatePolicy
//

const TUint8 KDefaultTxRatePolicyId = 1;

const TUint8 KTxPolicy54Default     = 0;
const TUint8 KTxPolicy48Default     = 0;
const TUint8 KTxPolicy36Default     = 0;
const TUint8 KTxPolicy33Default     = 0;
const TUint8 KTxPolicy24Default     = 0;
const TUint8 KTxPolicy22Default     = 0;
const TUint8 KTxPolicy18Default     = 0;
const TUint8 KTxPolicy12Default     = 0;
const TUint8 KTxPolicy11Default     = 0;
const TUint8 KTxPolicy9Default      = 0;
const TUint8 KTxPolicy6Default      = 0;
const TUint8 KTxPolicy5_5Default    = 0;
const TUint8 KTxPolicy2Default      = 0;
const TUint8 KTxPolicy1Default      = 1;

const TUint8 KShortRetryLimitDefault = 7;
const TUint8 KLongRetryLimitDefault  = 4;

const TUint32 KFlagsDefault          = 0;

const TUint32 KNumOfPolicyObjectsDefault = 1;

const StxRatePolicy KTxRatePolicyMibDefault =
    {   // rate policy struct
        KNumOfPolicyObjectsDefault,
        {   // rate class array             
            {   // rate class struct
                KTxPolicy54Default,
                KTxPolicy48Default,
                KTxPolicy36Default,
                KTxPolicy33Default,
                KTxPolicy24Default,
                KTxPolicy22Default,
                KTxPolicy18Default,
                KTxPolicy12Default,
                KTxPolicy11Default,
                KTxPolicy9Default,
                KTxPolicy6Default,
                KTxPolicy5_5Default,
                KTxPolicy2Default,
                KTxPolicy1Default,
                KShortRetryLimitDefault,
                KLongRetryLimitDefault,
                KFlagsDefault
            }
        }
    };


// htCapabilities
//
const TBool KHtSupportDefault = EFalse;

const ShtCapabilities KHtCapabilitiesMibDefault = 
    { 
        KHtSupportDefault,                  // HT support
        { { 0, 0, 0, 0, 0, 0 } },           // MAC
        0,                                  // STBC Tx
        0,                                  // A-MPDU
        0,                                  // capabilities
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },   // Rx MCS set
        0,                                  // min MPDU spacing in A-MPDUs
        0,                                  // MCS feedback
        0,                                  // transmit beamforming
        0,                                  // ASEL
        { 0, 0, 0 }                         // padding
    };


// htBssOperation
//
const ShtBssOperation KHtBssOperationMibDefault = 
    {
        0,                                  // HT information
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },   // basic MCS set
        0,                                  // HT protection mode
        0,                                  // secondary ch offset
        0,                                  // AP ch width
        { 0, 0, 0 }                         // padding
    };

// htSecondaryBeacon
//
const ShtSecondaryBeacon KHtSecondaryBeaconMibDefault = 
    {
        KHtSecBeaconRxPrimaryOnly,
        0
    };

// htBlockAckConfigure
//
const TUint8 KHtBlockAckDefault = 0x00; // Block Ack disabled for all TIDs 

const ShtBlockAckConfigure KHtBlockAckConfigureMibDefault = 
    {
        KHtBlockAckDefault,              
        KHtBlockAckDefault,
        0                       // padding
    };

NAMESPACE_END_WHA

#endif      // WHA_MIBDEFAULTVALUES_H  
