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
* Description:  Class parsing 802.11k Beacon Request Reporting Detail IEs.
*
*/


#include "genscaninfo.h"
#include "core_frame_rm_ie_beacon_request_detail_ie.h"
#include "core_tools.h"
#include "core_tools_parser.h"
#include "am_debug.h"

const u8_t CORE_FRAME_RM_IE_BEACON_REQUEST_DETAIL_IE_MIN_LENGTH = 3;
const u8_t CORE_FRAME_RM_IE_BEACON_REQUEST_DETAIL_IE_REPORTING_DETAIL_OFFSET = 2;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_rm_ie_beacon_request_detail_ie_c* core_frame_rm_ie_beacon_request_detail_ie_c::instance(
    const core_frame_dot11_ie_c& ie )
    {
    if ( ie.data_length() < CORE_FRAME_RM_IE_BEACON_REQUEST_DETAIL_IE_MIN_LENGTH )
        {
        DEBUG( "core_frame_rm_ie_beacon_request_detail_ie_c::instance() - not a valid IE, invalid length" );

        return NULL;
        }

    core_frame_rm_ie_beacon_request_detail_ie_c* instance = new core_frame_rm_ie_beacon_request_detail_ie_c(
        ie.data_length(),
        ie.data(),
        0 );
    
    if ( !instance )
        {
        DEBUG( "core_frame_rm_ie_beacon_request_detail_ie_c::instance() - unable to create an instance" );

        return NULL;
        }

    DEBUG1( "core_frame_rm_ie_beacon_request_detail_ie_c::instance() - Reporting Detail: 0x%02X",
            instance->reporting_detail() );
    
    return instance;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//    
u8_t core_frame_rm_ie_beacon_request_detail_ie_c::reporting_detail() const
    {
    return data_m[CORE_FRAME_RM_IE_BEACON_REQUEST_DETAIL_IE_REPORTING_DETAIL_OFFSET];
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_rm_ie_beacon_request_detail_ie_c::core_frame_rm_ie_beacon_request_detail_ie_c(
    u16_t data_length,
    const u8_t* data,
    u16_t max_data_length ) :
    core_frame_dot11_ie_c( data_length, data, max_data_length )
    {
    }
