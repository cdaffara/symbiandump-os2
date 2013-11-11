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
* Description:  Default values for some of the settings.
*
*/


#ifndef CORE_SETTINGS_DEFAULT_H
#define CORE_SETTINGS_DEFAULT_H

#include "core_types.h"

const core_uapsd_settings_s DEFAULT_UAPSD_SETTINGS =
    {
    core_max_service_period_length_all, // allow AP to send all buffered frames during a service period
    true_t,     // U-APSD is enabled for Voice
    true_t,     // U-APSD is enabled for Video
    true_t,     // U-APSD is enabled for BestEffort
    true_t      // U-APSD is enabled for Background
    };

const core_power_save_settings_s DEFAULT_POWER_SAVE_SETTINGS =
    {
    true_t,     // stay in U-APSD power save when using Voice 
    false_t,    // allow CAM<>U-APSD power save transitions when using Video
    false_t,    // allow CAM<>U-APSD power save transitions when using BestEffort
    false_t,    // allow CAM<>U-APSD power save transitions when using Background
    false_t,    // allow CAM<>legacy power save transitions when using Voice    
    false_t,    // allow CAM<>legacy power save transitions when using Video    
    false_t,    // allow CAM<>legacy power save transitions when using BestEffort    
    false_t,    // allow CAM<>legacy power save transitions when using Background    
    };

const core_block_ack_usage_s DEFAULT_BLOCK_ACK_USAGE =
    {
    0xFF,       // enabled for all traffic streams in TX direction
    0xFF        // enabled for all traffic streams in RX direction
    };

#endif // CORE_SETTINGS_DEFAULT_H
