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
* Description:  Implementations for the scan list iterators
*
*/


#include "core_scan_list_iterator.h"
#include "core_scan_list.h"
#include "core_tools.h"
#include "core_am_tools.h"
#include "am_debug.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_scan_list_iterator_by_tag_c::core_scan_list_iterator_by_tag_c(
    core_scan_list_c& scan_list,
    u8_t tag ) :
    scan_list_m( scan_list ),
    tag_m( tag )
    {
    DEBUG( "core_scan_list_iterator_by_tag_c::core_scan_list_iterator_by_tag_c" );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_scan_list_iterator_by_tag_c::~core_scan_list_iterator_by_tag_c()
    {
    DEBUG( "core_scan_list_iterator_by_tag_c::~core_scan_list_iterator_by_tag_c" );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_ap_data_c* core_scan_list_iterator_by_tag_c::first()
    {
    for( core_scan_list_c::core_scan_list_entry_s* iter = scan_list_m.scan_list_m.first(); iter; iter = scan_list_m.scan_list_m.next() )
        {
        if ( !( iter->tags & tag_m ) )
            {
            return iter->ap_data;
            }
        }

    return NULL;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_ap_data_c* core_scan_list_iterator_by_tag_c::next()
    {
    for( core_scan_list_c::core_scan_list_entry_s* iter = scan_list_m.scan_list_m.next(); iter; iter = scan_list_m.scan_list_m.next() )
        {
        if ( !( iter->tags & tag_m ) )
            {
            return iter->ap_data;
            }
        }

    return NULL;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_ap_data_c* core_scan_list_iterator_by_tag_c::current()
    {
    if ( scan_list_m.scan_list_m.current() )
        {
        return scan_list_m.scan_list_m.current()->ap_data;
        }

    return NULL;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_scan_list_iterator_by_age_c::core_scan_list_iterator_by_age_c(
    core_scan_list_c& scan_list,
    u32_t maximum_age ) :
    scan_list_m( scan_list ),
    maximum_age_m( maximum_age )
    {
    DEBUG( "core_scan_list_iterator_by_age_c::core_scan_list_iterator_by_age_c" );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_scan_list_iterator_by_age_c::~core_scan_list_iterator_by_age_c()
    {
    DEBUG( "core_scan_list_iterator_by_age_c::~core_scan_list_iterator_by_age_c" );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_ap_data_c* core_scan_list_iterator_by_age_c::first()
    {
    for( core_scan_list_c::core_scan_list_entry_s* iter = scan_list_m.scan_list_m.first(); iter; iter = scan_list_m.scan_list_m.next() )
        {
        u64_t difference = ( core_am_tools_c::timestamp() - iter->timestamp ) / SECONDS_FROM_MICROSECONDS;
            
        if ( maximum_age_m == SCAN_LIST_AGE_ANY ||
             difference <= maximum_age_m )
            {
            return iter->ap_data;
            }
        }

    return NULL;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_ap_data_c* core_scan_list_iterator_by_age_c::next()
    {
    for( core_scan_list_c::core_scan_list_entry_s* iter = scan_list_m.scan_list_m.next(); iter; iter = scan_list_m.scan_list_m.next() )
        {
        u64_t difference = ( core_am_tools_c::timestamp() - iter->timestamp ) / SECONDS_FROM_MICROSECONDS;
            
        if ( maximum_age_m == SCAN_LIST_AGE_ANY ||
             difference <= maximum_age_m )
            {
            return iter->ap_data;
            }
        }

    return NULL;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_ap_data_c* core_scan_list_iterator_by_age_c::current()
    {
    if ( scan_list_m.scan_list_m.current() )
        {
        return scan_list_m.scan_list_m.current()->ap_data;
        }

    return NULL;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_scan_list_iterator_by_tag_and_ssid_c::core_scan_list_iterator_by_tag_and_ssid_c(
    core_scan_list_c& scan_list,
    u8_t tag,
    const core_ssid_s& ssid ) :
    core_scan_list_iterator_by_tag_c( scan_list, tag ),
    ssid_m( ssid )
    {
    DEBUG( "core_scan_list_iterator_by_tag_and_ssid_c::core_scan_list_iterator_by_tag_and_ssid_c" );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_scan_list_iterator_by_tag_and_ssid_c::~core_scan_list_iterator_by_tag_and_ssid_c()
    {
    DEBUG( "core_scan_list_iterator_by_tag_and_ssid_c::~core_scan_list_iterator_by_tag_and_ssid_c" );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_ap_data_c* core_scan_list_iterator_by_tag_and_ssid_c::first()
    {
    core_ap_data_c* iter = core_scan_list_iterator_by_tag_c::first();
    while ( iter )
        {
        if ( iter->ssid() == ssid_m )
            {
            return iter;
            }
            
        iter = core_scan_list_iterator_by_tag_c::next();
        }

    return NULL;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_ap_data_c* core_scan_list_iterator_by_tag_and_ssid_c::next()
    {
    core_ap_data_c* iter = core_scan_list_iterator_by_tag_c::next();
    while ( iter )
        {
        if ( iter->ssid() == ssid_m )
            {
            return iter;
            }
            
        iter = core_scan_list_iterator_by_tag_c::next();
        }

    return NULL;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_scan_list_iterator_by_tag_and_ssid_and_bssid_c::core_scan_list_iterator_by_tag_and_ssid_and_bssid_c(
    core_scan_list_c& scan_list,
    u8_t tag,
    const core_ssid_s& ssid,
    const core_mac_address_s& bssid ) :
    core_scan_list_iterator_by_tag_and_ssid_c( scan_list, tag, ssid ),
    bssid_m( bssid )
    {
    DEBUG( "core_scan_list_iterator_by_tag_and_ssid_and_bssid_c::core_scan_list_iterator_by_tag_and_ssid_and_bssid_c" );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_scan_list_iterator_by_tag_and_ssid_and_bssid_c::~core_scan_list_iterator_by_tag_and_ssid_and_bssid_c()
    {
    DEBUG( "core_scan_list_iterator_by_tag_and_ssid_and_bssid_c::~core_scan_list_iterator_by_tag_and_ssid_and_bssid_c" );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_ap_data_c* core_scan_list_iterator_by_tag_and_ssid_and_bssid_c::first()
    {
    core_ap_data_c* iter = core_scan_list_iterator_by_tag_and_ssid_c::first();
    while( iter )
        {
        if( bssid_m == BROADCAST_MAC_ADDR ||
            iter->bssid() == bssid_m )
            {
            return iter;
            }

        iter = core_scan_list_iterator_by_tag_and_ssid_c::next();
        }

    return NULL;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_ap_data_c* core_scan_list_iterator_by_tag_and_ssid_and_bssid_c::next()
    {
    core_ap_data_c* iter = core_scan_list_iterator_by_tag_and_ssid_c::next();
    while( iter )
        {
        if( bssid_m == BROADCAST_MAC_ADDR ||
            iter->bssid() == bssid_m )
            {
            return iter;
            }
            
        iter = core_scan_list_iterator_by_tag_and_ssid_c::next();
        }

    return NULL;
    }
