/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Default values for TX rate policies.
*
*/


#ifndef CORE_TX_RATE_POLICIES_H
#define CORE_TX_RATE_POLICIES_H

#include "core_types.h"

/**
 * TX rate policies to use with 802.11bg access points.
 */

const core_tx_rate_policy_s TX_RATE_POLICY_BG =
    {
    1,                      // 54 Mbits/s
    0,                      // 48 Mbits/s
    0,                      // 36 Mbits/s
    0,                      // 33 Mbits/s
    1,                      // 24 Mbits/s
    0,                      // 22 Mbits/s
    1,                      // 18 Mbits/s
    1,                      // 12 Mbits/s
    0,                      // 11 Mbits/s
    1,                      // 9 Mbits/s
    3,                      // 6 Mbits/s
    0,                      // 5.5 Mbits/s
    0,                      // 2 Mbits/s
    1,                      // 1 Mbits/s
    0,                      // short retry limit will be filled later
    0,                      // long retry limit will be filled later
    0,                      // no flags
    core_tx_rate_6mbit,     // initial rate is 6 Mbit/s
    CORE_TX_RATES_802P11BG, // TX auto rate policy (all 802.11bg rates)
    { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF } 
    };

const core_tx_rate_policy_s TX_RATE_POLICY_VOICE_BG =
    {
    1,                      // 54 Mbits/s
    0,                      // 48 Mbits/s
    0,                      // 36 Mbits/s
    0,                      // 33 Mbits/s
    1,                      // 24 Mbits/s
    0,                      // 22 Mbits/s
    1,                      // 18 Mbits/s
    1,                      // 12 Mbits/s
    0,                      // 11 Mbits/s
    1,                      // 9 Mbits/s
    1,                      // 6 Mbits/s
    0,                      // 5.5 Mbits/s
    0,                      // 2 Mbits/s
    1,                      // 1 Mbits/s
    0,                      // short retry limit will be filled later
    0,                      // long retry limit will be filled later
    0,                      // no flags
    core_tx_rate_12mbit,    // initial rate is 12 Mbit/s
    CORE_TX_RATES_802P11BG, // TX auto rate policy (all 802.11bg rates)
    { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }
    };

const core_tx_rate_policy_s TX_RATE_POLICY_BLUETOOTH_BG =
    {
    2,                      // 54 Mbits/s
    2,                      // 48 Mbits/s
    1,                      // 36 Mbits/s
    0,                      // 33 Mbits/s
    1,                      // 24 Mbits/s
    0,                      // 22 Mbits/s
    1,                      // 18 Mbits/s
    1,                      // 12 Mbits/s
    0,                      // 11 Mbits/s
    1,                      // 9 Mbits/s
    1,                      // 6 Mbits/s
    0,                      // 5.5 Mbits/s
    0,                      // 2 Mbits/s
    0,                      // 1 Mbits/s
    0,                      // short retry limit will be filled later
    0,                      // long retry limit will be filled later
    0,                      // no flags
    core_tx_rate_24mbit,    // initial rate is 24 Mbit/s
    0x3FF8,                 // TX auto rate policy (6 Mbits/s and above)
    { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }
    };

const core_tx_rate_policy_s TX_RATE_POLICY_BLUETOOTH_VOICE_BG =
    {
    2,                      // 54 Mbits/s
    2,                      // 48 Mbits/s
    1,                      // 36 Mbits/s
    0,                      // 33 Mbits/s
    1,                      // 24 Mbits/s
    0,                      // 22 Mbits/s
    1,                      // 18 Mbits/s
    1,                      // 12 Mbits/s
    0,                      // 11 Mbits/s
    1,                      // 9 Mbits/s
    2,                      // 6 Mbits/s
    0,                      // 5.5 Mbits/s
    0,                      // 2 Mbits/s
    1,                      // 1 Mbits/s
    0,                      // short retry limit will be filled later
    0,                      // long retry limit will be filled later
    0,                      // no flags
    core_tx_rate_24mbit,    // initial rate is 24 Mbit/s
    0x3FF9,                 // TX auto rate policy (1 Mbit/s, 6 Mbit/s and above)
    { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }
    };

/**
 * TX rate policies to use with 802.11b-only access points.
 */

const core_tx_rate_policy_s TX_RATE_POLICY_B =
    {
    0,                      // 54 Mbits/s
    0,                      // 48 Mbits/s
    0,                      // 36 Mbits/s
    0,                      // 33 Mbits/s
    0,                      // 24 Mbits/s
    0,                      // 22 Mbits/s
    0,                      // 18 Mbits/s
    0,                      // 12 Mbits/s
    2,                      // 11 Mbits/s
    0,                      // 9 Mbits/s
    0,                      // 6 Mbits/s
    2,                      // 5.5 Mbits/s
    2,                      // 2 Mbits/s
    1,                      // 1 Mbits/s
    0,                      // short retry limit will be filled later
    0,                      // long retry limit will be filled later
    0,                      // no flags
    core_tx_rate_2mbit,     // initial rate is 2 Mbit/s,
    CORE_TX_RATES_802P11B,  // TX auto rate policy (all 802.11b rates)
    { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }
    };

const core_tx_rate_policy_s TX_RATE_POLICY_VOICE_B =
    {
    0,                      // 54 Mbits/s
    0,                      // 48 Mbits/s
    0,                      // 36 Mbits/s
    0,                      // 33 Mbits/s
    0,                      // 24 Mbits/s
    0,                      // 22 Mbits/s
    0,                      // 18 Mbits/s
    0,                      // 12 Mbits/s
    2,                      // 11 Mbits/s
    0,                      // 9 Mbits/s
    0,                      // 6 Mbits/s
    2,                      // 5.5 Mbits/s
    1,                      // 2 Mbits/s  
    1,                      // 1 Mbits/s
    0,                      // short retry limit will be filled later
    0,                      // long retry limit will be filled later
    0,                      // no flags
    core_tx_rate_5p5mbit,   // initial rate is 5.5 Mbit/s
    CORE_TX_RATES_802P11B,  // TX auto rate policy (all 802.11b rates)
    { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }
    };

const core_tx_rate_policy_s TX_RATE_POLICY_BLUETOOTH_B =
    {
    0,                      // 54 Mbits/s
    0,                      // 48 Mbits/s
    0,                      // 36 Mbits/s
    0,                      // 33 Mbits/s
    0,                      // 24 Mbits/s
    0,                      // 22 Mbits/s
    0,                      // 18 Mbits/s
    0,                      // 12 Mbits/s
    6,                      // 11 Mbits/s
    0,                      // 9 Mbits/s
    0,                      // 6 Mbits/s
    2,                      // 5.5 Mbits/s
    0,                      // 2 Mbits/s
    0,                      // 1 Mbits/s
    0,                      // short retry limit will be filled later
    0,                      // long retry limit will be filled later
    0,                      // no flags
    core_tx_rate_11mbit,    // initial rate is 11 Mbit/s
    0x24,                   // TX auto rate policy (5.5 Mbit/s, 11 Mbit/s)
    { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }
    };

const core_tx_rate_policy_s TX_RATE_POLICY_BLUETOOTH_VOICE_B =
    {
    0,                      // 54 Mbits/s
    0,                      // 48 Mbits/s
    0,                      // 36 Mbits/s
    0,                      // 33 Mbits/s
    0,                      // 24 Mbits/s
    0,                      // 22 Mbits/s
    0,                      // 18 Mbits/s
    0,                      // 12 Mbits/s
    6,                      // 11 Mbits/s
    0,                      // 9 Mbits/s
    0,                      // 6 Mbits/s
    2,                      // 5.5 Mbits/s
    0,                      // 2 Mbits/s
    1,                      // 1 Mbits/s
    0,                      // short retry limit will be filled later
    0,                      // long retry limit will be filled later
    0,                      // no flags
    core_tx_rate_11mbit,    // initial rate is 11 Mbit/s
    0x25,                   // TX auto rate policy (1 Mbit/s, 5.5 Mbit/s, 11 Mbit/s)
    { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }
    };

/**
 * TX rate policies to use with 802.11g-only access points.
 */

const core_tx_rate_policy_s TX_RATE_POLICY_G =
    {
    1,                      // 54 Mbits/s
    0,                      // 48 Mbits/s
    0,                      // 36 Mbits/s
    0,                      // 33 Mbits/s
    1,                      // 24 Mbits/s
    0,                      // 22 Mbits/s
    1,                      // 18 Mbits/s
    1,                      // 12 Mbits/s
    0,                      // 11 Mbits/s
    1,                      // 9 Mbits/s
    2,                      // 6 Mbits/s
    0,                      // 5.5 Mbits/s
    0,                      // 2 Mbits/s
    0,                      // 1 Mbits/s
    0,                      // short retry limit will be filled later
    0,                      // long retry limit will be filled later
    0,                      // no flags
    core_tx_rate_6mbit,     // initial rate is 6 Mbit/s,
    CORE_TX_RATES_802P11G,  // TX auto rate policy (all 802.11g rates)
    { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }
    };

const core_tx_rate_policy_s TX_RATE_POLICY_VOICE_G =
    {
    1,                      // 54 Mbits/s
    0,                      // 48 Mbits/s
    0,                      // 36 Mbits/s
    0,                      // 33 Mbits/s
    1,                      // 24 Mbits/s
    0,                      // 22 Mbits/s
    1,                      // 18 Mbits/s
    1,                      // 12 Mbits/s
    0,                      // 11 Mbits/s
    0,                      // 9 Mbits/s
    2,                      // 6 Mbits/s
    0,                      // 5.5 Mbits/s
    0,                      // 2 Mbits/s
    0,                      // 1 Mbits/s
    0,                      // short retry limit will be filled later
    0,                      // long retry limit will be filled later
    0,                      // no flags
    core_tx_rate_6mbit,     // initial rate is 6 Mbit/s
    CORE_TX_RATES_802P11G,  // TX auto rate policy (all 802.11g rates)
    { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }
    };

const core_tx_rate_policy_s TX_RATE_POLICY_BLUETOOTH_G =
    {
    2,                      // 54 Mbits/s
    2,                      // 48 Mbits/s
    1,                      // 36 Mbits/s
    0,                      // 33 Mbits/s
    1,                      // 24 Mbits/s
    0,                      // 22 Mbits/s
    1,                      // 18 Mbits/s
    1,                      // 12 Mbits/s
    0,                      // 11 Mbits/s
    1,                      // 9 Mbits/s
    1,                      // 6 Mbits/s
    0,                      // 5.5 Mbits/s
    0,                      // 2 Mbits/s
    0,                      // 1 Mbits/s
    0,                      // short retry limit will be filled later
    0,                      // long retry limit will be filled later
    0,                      // no flags
    core_tx_rate_24mbit,    // initial rate is 24 Mbit/s
    CORE_TX_RATES_802P11G,  // TX auto rate policy (all 802.11g rates)
    { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }
    };

const core_tx_rate_policy_s TX_RATE_POLICY_BLUETOOTH_VOICE_G =
    {
    2,                      // 54 Mbits/s
    2,                      // 48 Mbits/s
    1,                      // 36 Mbits/s
    0,                      // 33 Mbits/s
    1,                      // 24 Mbits/s
    0,                      // 22 Mbits/s
    1,                      // 18 Mbits/s
    1,                      // 12 Mbits/s
    0,                      // 11 Mbits/s
    1,                      // 9 Mbits/s
    1,                      // 6 Mbits/s
    0,                      // 5.5 Mbits/s
    0,                      // 2 Mbits/s
    0,                      // 1 Mbits/s
    0,                      // short retry limit will be filled later
    0,                      // long retry limit will be filled later
    0,                      // no flags
    core_tx_rate_24mbit,    // initial rate is 24 Mbit/s
    CORE_TX_RATES_802P11G,  // TX auto rate policy (all 802.11g rates)
    { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }
    };

#endif // CORE_TX_RATE_POLICIES_H
