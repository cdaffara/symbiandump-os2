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
* Description:  Class for parsing 802.11e QBSS Load IE.
*
*/


#include "core_frame_qbss_load_ie.h"
#include "core_tools.h"
#include "core_tools_parser.h"
#include "am_debug.h"

const u8_t CORE_FRAME_QBSS_LOAD_IE_LENGTH = 7;
const u8_t CORE_FRAME_QBSS_LOAD_IE_STATION_COUNT_OFFSET = 2;
const u8_t CORE_FRAME_QBSS_LOAD_IE_CHANNEL_UTILIZATION_OFFSET = 4;
const u8_t CORE_FRAME_QBSS_LOAD_IE_ADMISSION_CAPACITY_OFFSET = 5;

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_qbss_load_ie_c* core_frame_qbss_load_ie_c::instance(
    const core_frame_dot11_ie_c& ie )
    {
    if ( ie.data_length() != CORE_FRAME_QBSS_LOAD_IE_LENGTH )
        {
        DEBUG( "core_frame_qbss_load_ie_c::instance() - not a valid IE, invalid length" );

        return NULL;
        }

    core_frame_qbss_load_ie_c* instance = new core_frame_qbss_load_ie_c(
        ie.data_length(),
        ie.data(),
        0 );
    if ( !instance )
        {
        DEBUG( "core_frame_qbss_load_ie_c::instance() - unable to create an instance" );

        return NULL;
        }

    return instance;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//       
core_frame_qbss_load_ie_c::~core_frame_qbss_load_ie_c()
    {
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u16_t core_frame_qbss_load_ie_c::station_count() const
    {
    return core_tools_c::get_u16(
        data_m,
        CORE_FRAME_QBSS_LOAD_IE_STATION_COUNT_OFFSET );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u8_t core_frame_qbss_load_ie_c::channel_utilitization() const
    {
    return data_m[CORE_FRAME_QBSS_LOAD_IE_CHANNEL_UTILIZATION_OFFSET];
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u16_t core_frame_qbss_load_ie_c::admission_capacity() const
    {
    return core_tools_c::get_u16(
        data_m,
        CORE_FRAME_QBSS_LOAD_IE_ADMISSION_CAPACITY_OFFSET );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_qbss_load_ie_c::core_frame_qbss_load_ie_c(
    u16_t data_length,
    const u8_t* data,
    u16_t max_data_length ) :
    core_frame_dot11_ie_c( data_length, data, max_data_length )
    {
    }
