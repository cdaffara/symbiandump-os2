/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Default parameter sets for traffic streams.
*
*/

/*
* %version: 1 %
*/

#ifndef WLMTSPARAMS_H
#define WLMTSPARAMS_H

#include "core_types.h"

/**
 * Constant definitions.
 */
const u32_t KWlmTsParamUnspecified = 0;
const u32_t KWlmTsParamDisabled = 0xFFFFFFFF;
const u32_t KWlmTsParamBitNominalSizeFixed = 0x8000;

/**
 * The default traffic stream parameter sets per access class.
 */
const core_traffic_stream_params_s KWlmDefaultTsParams[] = 
    {
        // EWlmAccessClassBestEffort
        {
        false_t,                                    // Periodic traffic
        core_traffic_stream_direction_bidirectional,// Traffic stream direction
        KWlmTsParamUnspecified,                     // Nominal MSDU size (bytes)
        1500,                                       // Maximum MSDU size (bytes)
        KWlmTsParamUnspecified,                     // Minimum service interval (microseconds)
        KWlmTsParamUnspecified,                     // Maximum service interval (microseconds)
        KWlmTsParamUnspecified,                     // Inactivity interval (microseconds)
        KWlmTsParamDisabled,                        // Suspension interval (microseconds)
        KWlmTsParamUnspecified,                     // Service start time (microseconds)
        60000,                                      // Minimum data rate (bps)
        60000,                                      // Average data rate (bps)
        60000,                                      // Maximum data rate (bps)
        KWlmTsParamUnspecified,                     // Maximum burst size (bytes)
        KWlmTsParamUnspecified,                     // Maximum transport delay (microseconds)
        6000000,                                    // Minimum WLAN data rate (bps)
        core_tx_rate_6mbit,                         // Nominal WLAN data rate (bps)
        8192,                                       // SBA ratio        
        true_t,                                     // Re-creation of traffic streams
        0,                                          // ignored
        0,                                          // ignored
        0                                           // ignored
        },

        // EWlmAccessClassBackground
        {
        false_t,                                    // Traffic pattern
        core_traffic_stream_direction_bidirectional,// Traffic stream direction
        KWlmTsParamUnspecified,                     // Nominal MSDU size (bytes)
        1500,                                       // Maximum MSDU size (bytes)
        KWlmTsParamUnspecified,                     // Minimum service interval (microseconds)
        KWlmTsParamUnspecified,                     // Maximum service interval (microseconds)
        KWlmTsParamUnspecified,                     // Inactivity interval (microseconds)
        KWlmTsParamDisabled,                        // Suspension interval (microseconds)
        KWlmTsParamUnspecified,                     // Service start time (microseconds)
        60000,                                      // Minimum data rate (bps)
        60000,                                      // Average data rate (bps)
        60000,                                      // Maximum data rate (bps)
        KWlmTsParamUnspecified,                     // Maximum burst size (bytes)
        KWlmTsParamUnspecified,                     // Maximum transport delay (microseconds)
        6000000,                                    // Minimum WLAN data rate (bps)
        core_tx_rate_6mbit,                         // Nominal WLAN data rate (bps)
        8192,                                       // SBA ratio
        true_t,                                     // Re-creation of traffic streams
        0,                                          // ignored
        0,                                          // ignored
        0                                           // ignored
        },
        
        // EWlmAccessClassVideo
        {
        false_t,                                    // Traffic pattern
        core_traffic_stream_direction_bidirectional,// Traffic stream direction
        150,                                        // Nominal MSDU size (bytes)
        1500,                                       // Maximum MSDU size (bytes)
        KWlmTsParamUnspecified,                     // Minimum service interval (microseconds)
        KWlmTsParamUnspecified,                     // Maximum service interval (microseconds)
        KWlmTsParamUnspecified,                     // Inactivity interval (microseconds)
        KWlmTsParamDisabled,                        // Suspension interval (microseconds)
        KWlmTsParamUnspecified,                     // Service start time (microseconds)
        160,                                        // Minimum data rate (bps)
        160,                                        // Average data rate (bps)
        160,                                        // Maximum data rate (bps)
        KWlmTsParamUnspecified,                     // Maximum burst size (bytes)
        KWlmTsParamUnspecified,                     // Maximum transport delay (microseconds)
        6000000,                                    // Minimum WLAN data rate (bps)
        core_tx_rate_6mbit,                         // Nominal WLAN data rate (bps)
        8192,                                       // SBA ratio
        true_t,                                     // Re-creation of traffic streams
        0,                                          // ignored
        0,                                          // ignored
        0                                           // ignored
        },

        // EWlmAccessClassVoice
        {
        true_t,                                     // Traffic pattern
        core_traffic_stream_direction_bidirectional,// Traffic stream direction
        208 | KWlmTsParamBitNominalSizeFixed,       // Nominal MSDU size (bytes)
        208,                                        // Maximum MSDU size (bytes)
        20000,                                      // Minimum service interval (microseconds)
        20000,                                      // Maximum service interval (microseconds)
        10000000,                                   // Inactivity interval (microseconds)
        KWlmTsParamDisabled,                        // Suspension interval (microseconds)
        KWlmTsParamUnspecified,                     // Service start time (microseconds)
        83200,                                      // Minimum data rate (bps)
        83200,                                      // Average data rate (bps)
        83200,                                      // Maximum data rate (bps)
        KWlmTsParamUnspecified,                     // Maximum burst size (bytes)
        KWlmTsParamUnspecified,                     // Maximum transport delay (microseconds)
        6000000,                                    // Minimum WLAN data rate (bps)
        core_tx_rate_6mbit,                         // Nominal WLAN data rate (bps)
        8193,                                       // SBA ratio
        true_t,                                     // Re-creation of traffic streams
        0,                                          // ignored
        0,                                          // ignored
        0                                           // ignored
        }
    };

#endif // WLMTSPARAMS_H
