/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Declaration of the SWlanMib structure.
*
*/

/*
* %version: 17 %
*/

#ifndef UMACMIB_H
#define UMACMIB_H

#include "802dot11DefaultValues.h"
#include "wha_mibDefaultvalues.h"

#ifndef RD_WLAN_DDK
#include <wha_types.h>
#else
#include <wlanwha_types.h>
#endif

/**
* This attribute shall specify the number of TUs that a
* responding STA should wait for the next frame in the
* authentication sequence
*/
const TUint32 dot11AuthenticationResponseTimeOut 
    = EDot11AuthenticateResponseTimeoutDefault;

/**
* This attribute shall specify the number of TUs that a
* requesting STA should wait for a response to a
* transmitted association-request MMPDU
*/
const TUint32 dot11AssociateResponseTimeout 
    = EDot11AssociationResponseTimeOutDefault;

/**
*  This structure holds 802.11 MIB values
*/
struct SWlanMib
    {
    /** 
    * After exceeding this number of consecutive failed transmissions indicate 
    * BSS lost; by default 
    */
    enum { KFailedTxPacketCountThresholdDefault = 4 };
    
    /** Ctor */
    inline SWlanMib();

    /**
    * The MAC address assigned to this STA
    */
    TMacAddress dot11StationId;
    /**
    * The MaxTransmitMSDULifetime shall be the elapsed time in TU, after the
    * initial transmission of an MSDU, after which further attempts to transmit
    * the MSDU shall be terminated.
    * The Lifetime may be specified separately for every WMM Access Category,
    * i.e. Tx queue.
    * Indexed with WHA::TQueueId
    */
    TUint32 dot11MaxTransmitMSDULifetime[WHA::EQueueIdMax];
    /** MaxTransmitMSDULifetime default value */ 
    TUint32 dot11MaxTransmitMSDULifetimeDefault;
    /**
    * This attribute shall indicate the number of octets in an MPDU, below
    * which an RTS/CTS handshake shall not be performed. An RTS/CTS 
    * handshake shall be performed at the beginning of any frame exchange
    * sequence where the MPDU is of type Data or Management, the MPDU has an
    * individual address in the Address1 field, and the length of the MPDU is
    * greater than this threshold. Setting this attribute to be larger than 
    * the maximum MSDU size shall have the effect of turning off the RTS/CTS 
    * handshake for frames of Data or Management type transmitted by this STA.
    * Setting this attribute to zero shall have the effect of turning on the 
    * RTS/CTS handshake for all frames of Data or Management type transmitted 
    * by this STA. 
    */
    TUint32 dot11RTSThreshold;
    /** The Tx power level used to transmit data */
    WHA::TPowerLevel    dot11CurrentTxPowerLevel;
    /** The wlan wake-up interval in BSS mode */
    WHA::TWlanWakeUpInterval iWlanWakeupInterval;
    /** 
    * Specifies the value of N for WLAN wake-up interval modes 2 and 3. So, 
    * is relevant only for wake-up interval modes 2 & 3  
    */
    TUint8 iWlanListenInterval;
    /**  
    * The number of consecutive beacons that can be lost in infrastructure 
    * mode before the WLAN device should send BSSLost event to us
    */
    TUint32 iBeaconLostCount;    
    /**  
    * The number of consecutive transmissions that can fail totally before 
    * we indicate BSS lost 
    */
    TUint8 iFailedTxPacketCountThreshold;
    /**
    * The amount of time a Tx queue is allowed to access the WLAN air 
    * interface during one second interval. The unit of the parameter is 32 
    * microseconds. Value 0 means that the Medium Time is unlimited.
    * Indexed with WHA::TQueueId
    */
    TUint16 iMediumTime[WHA::EQueueIdMax];
    
private:
    /** Prohibit copy constructor */
    SWlanMib( const SWlanMib& );
    /** Prohibit assigment operator */
    SWlanMib& operator= ( const SWlanMib& );
    };


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline SWlanMib::SWlanMib() :
    dot11StationId( KZeroMacAddr ),
    // dot11MaxTransmitMSDULifetimeDefault is properly initialized in 
    // WlanDot11State::OnConfigureUmacMib        
    dot11MaxTransmitMSDULifetimeDefault( 0 ),
    dot11RTSThreshold( EDot11RTSThresholdDefault ),
    dot11CurrentTxPowerLevel( 0 ),
    iWlanWakeupInterval( WHA::KWlanWakeUpIntervalMibDefault.iMode ),
    iWlanListenInterval( WHA::KListenIntervalDefault ),
    iBeaconLostCount( WHA::KBeaconLostCountDefault ),
    iFailedTxPacketCountThreshold( KFailedTxPacketCountThresholdDefault )
    {
    // dot11MaxTransmitMSDULifetime is properly initialized in 
    // WlanDot11State::OnConfigureUmacMib        
    os_memset( 
        dot11MaxTransmitMSDULifetime, 
        0, 
        sizeof( dot11MaxTransmitMSDULifetime) );

    // iMediumTime is properly initialized in
    // WlanDot11State::OnConfigureUmacMib
    os_memset( iMediumTime, 0, sizeof( iMediumTime) );
    }

#endif      // UMACMIB_H
