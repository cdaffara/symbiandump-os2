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
* Description:  Class for parsing WMM QoS IEs.
*
*/


#include "core_frame_wmm_ie.h"
#include "core_tools.h"
#include "core_tools_parser.h"
#include "am_debug.h"

const u8_t CORE_FRAME_WMM_IE_PARAMETER_LENGTH = 26;
const u8_t CORE_FRAME_WMM_IE_PARAMETER_OUI_OFFSET = 2;
const u8_t CORE_FRAME_WMM_IE_PARAMETER_OUI_LENGTH = 5;
const u8_t CORE_FRAME_WMM_IE_PARAMETER_OUI[] = { 0x00, 0x50, 0xF2, 0x02, 0x1 };
const u8_t CORE_FRAME_WMM_IE_PARAMETER_VERSION_OFFSET = 7;
const u8_t CORE_FRAME_WMM_IE_PARAMETER_QOS_INFO_OFFSET = 8;
const u8_t CORE_FRAME_WMM_IE_PARAMETER_ACI_BE_OFFSET = 10;
const u8_t CORE_FRAME_WMM_IE_PARAMETER_ACI_BK_OFFSET = 14;
const u8_t CORE_FRAME_WMM_IE_PARAMETER_ACI_VI_OFFSET = 18;
const u8_t CORE_FRAME_WMM_IE_PARAMETER_ACI_VO_OFFSET = 22;
const u8_t CORE_FRAME_WMM_IE_PARAMETER_UAPSD_MASK = 0x80;
const u8_t CORE_FRAME_WMM_IE_PARAMETER_ACM_MASK = 0x10;

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_wmm_ie_parameter_c* core_frame_wmm_ie_parameter_c::instance(
    const core_frame_dot11_ie_c& ie )
    {
    if ( ie.data_length() != CORE_FRAME_WMM_IE_PARAMETER_LENGTH )
        {
        DEBUG( "core_frame_wmm_ie_parameter_c::instance() - not a valid IE, invalid length" );

        return NULL;
        }

    if ( core_tools_c::compare(
        &ie.data()[CORE_FRAME_WMM_IE_PARAMETER_OUI_OFFSET],
        CORE_FRAME_WMM_IE_PARAMETER_OUI_LENGTH,
        &CORE_FRAME_WMM_IE_PARAMETER_OUI[0],
        CORE_FRAME_WMM_IE_PARAMETER_OUI_LENGTH ) )
        {
        DEBUG( "core_frame_wpa_ie_c::instance() - not a valid IE, WMM OUI missing" );

        return NULL;       
        }

    core_frame_wmm_ie_parameter_c* instance = new core_frame_wmm_ie_parameter_c(
        ie.data_length(),
        ie.data(),
        0 );
    if ( !instance )
        {
        DEBUG( "core_frame_wmm_ie_parameter_c::instance() - unable to create an instance" );

        return NULL;
        }
    
    return instance;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//       
core_frame_wmm_ie_parameter_c::~core_frame_wmm_ie_parameter_c()
    {
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u8_t core_frame_wmm_ie_parameter_c::version() const
    {
    return data_m[CORE_FRAME_WMM_IE_PARAMETER_VERSION_OFFSET];
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_frame_wmm_ie_parameter_c::is_uapsd_supported() const
    {
    if ( data_m[CORE_FRAME_WMM_IE_PARAMETER_QOS_INFO_OFFSET] & CORE_FRAME_WMM_IE_PARAMETER_UAPSD_MASK )
        {
        return true_t;
        }

    return false_t;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_frame_wmm_ie_parameter_c::is_admission_control_required(
    core_access_class_e access_class ) const
    {
    u8_t offset( 0 );
    
    switch ( access_class )
        {
        case core_access_class_best_effort:
            offset = CORE_FRAME_WMM_IE_PARAMETER_ACI_BE_OFFSET;
            break;
        case core_access_class_background:
            offset = CORE_FRAME_WMM_IE_PARAMETER_ACI_BK_OFFSET;
            break;
        case core_access_class_video:
            offset = CORE_FRAME_WMM_IE_PARAMETER_ACI_VI_OFFSET;
            break;
        case core_access_class_voice:
            /** Falls through on purpose. */
        default:
            offset = CORE_FRAME_WMM_IE_PARAMETER_ACI_VO_OFFSET;
            break;
        }

    if ( data_m[offset] & CORE_FRAME_WMM_IE_PARAMETER_ACM_MASK )
        {
        return true_t;
        }

    return false_t;    
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_wmm_ie_parameter_c::core_frame_wmm_ie_parameter_c(
    u16_t data_length,
    const u8_t* data,
    u16_t max_data_length ) :
    core_frame_dot11_ie_c( data_length, data, max_data_length )
    {
    }
