/*
* Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  WLAN event enumerations and uid:s for Publish And Subscribe.
*                PubSub clients can include this file and listen to these events.
*                These events will be routed through Publish And Subscribe.
*
*/

/*
* %version: 8 %
*/

#ifndef WLANINTERNALPSKEYS_H
#define WLANINTERNALPSKEYS_H

#include "wlansdkpskeys.h"

// LOCAL CONSTANTS
const TInt KPSWlanEnumerationFirstValue = 0;

/**
* P&S category WLAN information defined in wlansdkpskeys.h
*/


/**
* WLAN indicator
*/
const TUint KPSWlanIndicator = 0x00000002;
const RProperty::TType KPSWlanIndicatorType = RProperty::EInt;

/**
* The possible WLAN indicator values
* These values correspond to the WLAN icon statuses
*/
enum TPSWlanIndicator
    {
    /** No WLAN connection or WLAN availability, WLAN not in use */ 
    EPSWlanIndicatorNone = KPSWlanEnumerationFirstValue,
    /** 
    * The device has been set to scan for WLANs, and a WLAN is available. 
    * That is, the device has been set to show WLAN availability from
    * Tools->Settings->Connection->Wireless LAN->Show WLAN availability and if
    * WLAN is available, this indicator value is used.
    */
    EPSWlanIndicatorAvailable,
    /** A WLAN connection is active in a network that does not have encryption. */
    EPSWlanIndicatorActive,
    /** A WLAN connection is active in a network that has encryption. */
    EPSWlanIndicatorActiveSecure
    };

/**
* WLAN background scan interval (in seconds)
*/
const TUint KPSWlanBgScanInterval = 0x00000003;
const RProperty::TType KPSWlanBgScanIntervalType = RProperty::EInt;

/**
 * WLAN on/off state
 */
const TUint KPSWlanOnOffState = 0x00000004;
const RProperty::TType KPSWlanOnOffStateType = RProperty::EInt;

/**
 * Possible values for WLAN on/off state
 */
enum TPSWlanOnOff
    {
    /** WLAN is OFF */
    EPSWlanOff = 0,
    /** WLAN is ON */
    EPSWlanOn
    };

#endif // WLANINTERNALPSKEYS_H
           
//  End of File
