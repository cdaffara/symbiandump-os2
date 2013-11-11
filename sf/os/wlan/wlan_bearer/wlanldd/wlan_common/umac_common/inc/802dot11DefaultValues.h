/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Defines values for some 802.11 attributes.
*
*/

/*
* %version: 16 %
*/

#ifndef _802DOT11DOT11DEFAULTVALUES_H
#define _802DOT11DOT11DEFAULTVALUES_H

/**
* The time (in TUs) how long an MSDU can stay in 
* a transmit queue
*/
enum TDot11MaxTransmitMSDULifetime
    {
    EDot11MaxTransmitMSDULifetimeMin        = 1,
    EDot11MaxTransmitMSDULifetimeMax        = 0xFFFFFFFF,
    EDot11MaxTransmitMSDULifetimeDefault    = 512
    };

/**
* This attribute shall indicate the number of octets in an MPDU,
* below which an RTS/CTS handshake shall not be performed. An
* RTS/CTS handshake shall be performed at the beginning of any
* frame exchange sequence where the MPDU is of type Data or
* Management, the MPDU has an individual address in the Address1
* field, and the length of the MPDU is greater than
* this threshold. (For additional details, refer to Table 21 in
* 9.7.) Setting this attribute to be larger than the maximum
* MSDU size shall have the effect of turning off the RTS/CTS
* handshake for frames of Data or Management type transmitted by
* this STA. Setting this attribute to zero shall have the effect
* of turning on the RTS/CTS handshake for all frames of Data or
* Management type transmitted by this STA. 
*/

enum TDot11RTSThreshold
    {
    EDot11RTSThresholdMin      = 0,
    EDot11RTSThresholdMax      = 2347,
    EDot11RTSThresholdDefault  = EDot11RTSThresholdMax
    };

/**
* This attribute shall specify the number of TUs that a
* responding STA should wait for the next frame in the
* authentication sequence
*/
enum TDot11AuthenticationResponseTimeout
    {
    EDot11AuthenticateResponseTimeoutMin    = 1,
    EDot11AuthenticateResponseTimeoutMax    = 0xFFFFFFFF,
    EDot11AuthenticateResponseTimeoutDefault = 250
    };

/**
* This attribute shall specify the number of TUs that a
* requesting STA should wait for a response to a
* transmitted association-request MMPDU
*/
enum TDot11AssociationResponseTimeOut
    {
    EDot11AssociationResponseTimeOutMin    = 1,
    EDot11AssociationResponseTimeOutMax    = 0xFFFFFFFF,
    EDot11AssociationResponseTimeOutDefault = 250
    };

/**
* This attribute specifies the value for Listen Interval (in milliseconds),
* which is used in (re-)association request when associating to a network.
* The value here is at least the max duration of a passive scan (as an AP 
* may use the listen interval in determining the lifetime of frames that it 
* buffers for a STA)
*/
const TUint16 KDot11ListenIntervalInMs( 2000 ); // 2 s


/**
* CwMin & CwMax values per 802.11 standard
*/

/** for 802.11b */
const TUint16 KDot11CwMinB     = 31;
/** for 802.11a/g */
const TUint16 KDot11CwMinAandG = 15;
/** for 802.11a/b/g */
const TUint16 KDot11CwMax      = 1023;


/**
* Default WMM parameter values for a WMM station
* per WiFi WMM specification v1.1
*/

/** Background */
const TUint16 KDot11BgCwMinB         = KDot11CwMinB;
const TUint16 KDot11BgCwMinAandG     = KDot11CwMinAandG;    
const TUint16 KDot11BgCwMax          = KDot11CwMax;
const TUint8  KDot11BgAifsn          = 7;
const TUint16 KDot11BgTxopLimit      = 0;

/** Best Effort (Legacy) */
const TUint16 KDot11BeCwMinB         = KDot11CwMinB;
const TUint16 KDot11BeCwMinAandG     = KDot11CwMinAandG;
const TUint16 KDot11BeCwMax          = KDot11CwMax;
const TUint8  KDot11BeAifsn          = 3;
const TUint16 KDot11BeTxopLimit      = 0;

/** Video */
const TUint16 KDot11ViCwMinB         = ( KDot11CwMinB + 1 ) / 2 - 1;
const TUint16 KDot11ViCwMinAandG     = ( KDot11CwMinAandG + 1 ) / 2 - 1;
const TUint16 KDot11ViCwMaxB         = KDot11CwMinB;
const TUint16 KDot11ViCwMaxAandG     = KDot11CwMinAandG;
const TUint8  KDot11ViAifsn          = 2;
const TUint16 KDot11ViTxopLimitB     = 6016;
const TUint16 KDot11ViTxopLimitAandG = 3008;

/** Voice */
const TUint16 KDot11VoCwMinB         = ( KDot11CwMinB + 1 ) / 4 - 1;
const TUint16 KDot11VoCwMinAandG     = ( KDot11CwMinAandG + 1 ) / 4 - 1;
const TUint16 KDot11VoCwMaxB         = ( KDot11CwMinB + 1 ) / 2 - 1;
const TUint16 KDot11VoCwMaxAandG     = ( KDot11CwMinAandG + 1 ) / 2 - 1;    
const TUint8  KDot11VoAifsn          = 2;
const TUint16 KDot11VoTxopLimitB     = 3264;
const TUint16 KDot11VoTxopLimitAandG = 1504;


/** The minimum value of AIFSN per WiFi WMM specification v1.1 */
const TUint8  KDot11AifsnMin = 2;

/**
* Medium Time specifies the amount of time a Transmit queue is allowed to 
* access the WLAN air interface during one second interval. 
* Value 0 means that the medium time is unlimited.
*/
const TUint16 KDot11MediumTimeDefault = 0;

#endif      // _802DOT11DOT11DEFAULTVALUES_H
