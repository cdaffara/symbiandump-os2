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
* Description:  Class parsing 802.11k Beacon Request IEs.
*
*/


#include "genscaninfo.h"
#include "core_frame_rm_ie_beacon_request_ie.h"
#include "core_tools.h"
#include "core_tools_parser.h"
#include "am_debug.h"

const u8_t CORE_FRAME_RM_IE_BEACON_REQUEST_IE_MIN_LENGTH = 4;
const u8_t CORE_FRAME_RM_IE_BEACON_REQUEST_IE_REPORTING_CONDITION_OFFSET = 2;
const u8_t CORE_FRAME_RM_IE_BEACON_REQUEST_IE_THRESHOLD_REFERENCE_VALUE_OFFSET = 3;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_rm_ie_beacon_request_ie_c* core_frame_rm_ie_beacon_request_ie_c::instance( const core_frame_dot11_ie_c& ie )
    {
    if ( ie.data_length() < CORE_FRAME_RM_IE_BEACON_REQUEST_IE_MIN_LENGTH )
        {
        DEBUG( "core_frame_rm_ie_beacon_request_ie_c::instance() - not a valid IE, invalid length" );

        return NULL;
        }

    core_frame_rm_ie_beacon_request_ie_c* instance = new core_frame_rm_ie_beacon_request_ie_c(
        ie.data_length(),
        ie.data(),
        0 );
    
    if ( !instance )
        {
        DEBUG( "core_frame_rm_ie_parameter_c::instance() - unable to create an instance" );

        return NULL;
        }
    
    DEBUG1( "core_frame_rm_ie_beacon_request_ie_c::instance() - Reporting Condition: 0x%02X",
            instance->reporting_condition() );
    DEBUG1( "core_frame_rm_ie_beacon_request_ie_c::instance() - Threshold/Offset Reference Value: 0x%02X",
            instance->threshold_reference_value() );
    
    return instance;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//    
u8_t core_frame_rm_ie_beacon_request_ie_c::reporting_condition() const
    {
    return data_m[CORE_FRAME_RM_IE_BEACON_REQUEST_IE_REPORTING_CONDITION_OFFSET];
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//    
u8_t core_frame_rm_ie_beacon_request_ie_c::threshold_reference_value() const
    {
    return data_m[CORE_FRAME_RM_IE_BEACON_REQUEST_IE_THRESHOLD_REFERENCE_VALUE_OFFSET];
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_rm_ie_beacon_request_ie_c::core_frame_rm_ie_beacon_request_ie_c(
    u16_t data_length,
    const u8_t* data,
    u16_t max_data_length ) :
    core_frame_dot11_ie_c( data_length, data, max_data_length )
    {
    }
