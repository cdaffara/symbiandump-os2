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
* Description:  Implementation of core_scan_list_c class.
*
*/

/*
* %version: 14 %
*/

#include "core_scan_list.h"
#include "core_tools.h"
#include "core_am_tools.h"
#include "am_debug.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_scan_list_c::core_scan_list_c() :
    scan_list_m( )
    {
    DEBUG( "core_scan_list_c::core_scan_list_c()" );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_scan_list_c::~core_scan_list_c()
    {
    DEBUG( "core_scan_list_c::~core_scan_list_c()" );

    for ( core_scan_list_entry_s* iter = scan_list_m.first(); iter; iter = scan_list_m.next() )
        {
        delete iter->ap_data;
        }

    scan_list_m.clear();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u32_t core_scan_list_c::count() const
    {
    return scan_list_m.count();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_scan_list_c::update_entry(
    core_ap_data_c& ap_data )
    {
    bool_t is_match_found( false_t );
    const core_mac_address_s bssid(
        ap_data.bssid() );
    core_ssid_s ssid(
        ap_data.ssid() );    
    u64_t timestamp(
        core_am_tools_c::timestamp() );

    DEBUG6( "core_scan_list_c::update_entry() - searching for entries with BSSID %02X:%02X:%02X:%02X:%02X:%02X",
        bssid.addr[0], bssid.addr[1], bssid.addr[2],
        bssid.addr[3], bssid.addr[4], bssid.addr[5] );                
    DEBUG1S( "core_scan_list_c::update_entry() - and with SSID ",
        ssid.length, &ssid.ssid[0] );

    core_scan_list_entry_s* iter = scan_list_m.first();
    while( iter )
        {
        bool_t is_goto_next_entry( true_t );

        if ( iter->ap_data->bssid() == bssid )
            {
            if ( iter->ap_data->ssid() == ssid )
                {
                if ( !is_match_found )
                    {
                    if ( !iter->tags && iter->ap_data->rcpi() >= ap_data.rcpi() )
                        {                      
                        if ( ap_data.frame()->type() == core_frame_dot11_c::core_dot11_type_beacon )
                            {
                            DEBUG2( "core_scan_list_c::update_entry() - an entry with matching BSSID and SSID (age %u, RCPI %u) already received, ignoring beacon entry",
                                static_cast<u32_t>( timestamp - iter->timestamp ) / SECONDS_FROM_MICROSECONDS, iter->ap_data->rcpi() );
                            }
                        else
                            {
                            DEBUG2( "core_scan_list_c::update_entry() - an entry with matching BSSID and SSID (age %u, RCPI %u) already received, ignoring probe entry",
                                static_cast<u32_t>( timestamp - iter->timestamp ) / SECONDS_FROM_MICROSECONDS, iter->ap_data->rcpi() );
                            }

                        is_match_found = true_t;
                        }
                    else
                        {
                        if ( ap_data.frame()->type() == core_frame_dot11_c::core_dot11_type_beacon )
                            {
                            DEBUG2( "core_scan_list_c::update_entry() - an entry with matching BSSID and SSID (age %u, RCPI %u) found, replacing entry with a beacon entry",
                                static_cast<u32_t>( timestamp - iter->timestamp ) / SECONDS_FROM_MICROSECONDS, iter->ap_data->rcpi() );
                            }
                        else
                            {
                            DEBUG2( "core_scan_list_c::update_entry() - an entry with matching BSSID and SSID (age %u, RCPI %u) found, replacing entry with a probe entry",
                                static_cast<u32_t>( timestamp - iter->timestamp ) / SECONDS_FROM_MICROSECONDS, iter->ap_data->rcpi() );
                            }

                        is_match_found = true_t;

                        *(iter->ap_data) = ap_data;
                        iter->timestamp = timestamp;
                        iter->tags = 0;
                        }
                    }
                else
                    {
                    DEBUG( "core_scan_list_c::update_entry() - an entry with matching BSSID and SSID found (duplicate), removing entry" );

                    core_scan_list_entry_s* temp = iter;
                    iter = scan_list_m.next();
                    scan_list_m.remove( temp );

                    is_goto_next_entry = false_t;
                    }
                }
            else if ( iter->ap_data->frame()->type() == core_frame_dot11_c::core_dot11_type_beacon &&
                      ap_data.frame()->type() == core_frame_dot11_c::core_dot11_type_beacon )
                {
                /**
                 * If both the new entry and the old entry are beacons with different SSIDs,
                 * it means the SSID has changed and the old entry needs to be replaced or
                 * removed.
                 */
                if ( !is_match_found )
                    {
                    DEBUG( "core_scan_list_c::update_entry() - two beacons with the same BSSID but different SSID found, replacing entry" );
                    is_match_found = true_t;

                    *(iter->ap_data) = ap_data;
                    iter->timestamp = timestamp;
                    iter->tags = 0;
                    }
                else
                    {
                    DEBUG( "core_scan_list_c::update_entry() - two beacons with the same BSSID but different SSID found (duplicate), removing entry" );

                    core_scan_list_entry_s* temp = iter;
                    iter = scan_list_m.next();
                    scan_list_m.remove( temp );

                    is_goto_next_entry = false_t;
                    }
                }
#if 0                
            else
                {            
                DEBUG( "core_scan_list_c::update_entry() - an entry with matching BSSID found but SSID doesn't match, refreshing entry timestamp" );
                iter->timestamp = timestamp;
                }
#endif // 0                
            }

        if ( is_goto_next_entry )
            {
            iter = scan_list_m.next();
            }
        }

    if ( !is_match_found )
        {
        if ( ap_data.frame()->type() == core_frame_dot11_c::core_dot11_type_beacon )
            {
            DEBUG( "core_scan_list_c::update_entry() - no entry matching BSSID and SSID found, adding a new beacon entry" );
            }
        else
            {
            DEBUG( "core_scan_list_c::update_entry() - no entry matching BSSID and SSID found, adding a new probe entry" );
            }

        core_scan_list_entry_s* entry = new core_scan_list_entry_s;
        if ( entry )
            {
            entry->ap_data = core_ap_data_c::instance( ap_data );
            entry->timestamp = timestamp;
            entry->tags = 0;

            scan_list_m.append( entry );
            }
        }

    return core_error_ok;         
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_scan_list_c::remove_entries_by_bssid(
    const core_mac_address_s& bssid )
    {
    DEBUG6( "core_scan_list_c::remove_entries_by_bssid() - removing entries with BSSID %02X:%02X:%02X:%02X:%02X:%02X",
        bssid.addr[0], bssid.addr[1], bssid.addr[2],
        bssid.addr[3], bssid.addr[4], bssid.addr[5] );                

    core_type_list_iterator_c<core_scan_list_entry_s> iter( scan_list_m );
    for( core_scan_list_entry_s* current = iter.first(); current; current = iter.next() )
        {
        if ( current->ap_data->bssid() == bssid )
            {
            DEBUG( "core_scan_list_c::remove_entries_by_bssid() - matching entry found" );
            
            iter.remove();
            
            delete current->ap_data;
            delete current;
            current = NULL;
            }
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_scan_list_c::remove_entries_by_age(
    u32_t age )
    {
    DEBUG1( "core_scan_list_c::remove_entries_by_age() - removing entries older than %u microsecond(s)",
        age );

    u64_t timestamp(
        core_am_tools_c::timestamp() );

    core_type_list_iterator_c<core_scan_list_entry_s> iter( scan_list_m );
    for( core_scan_list_entry_s* current = iter.first(); current; current = iter.next() )
        {
        if ( static_cast<u32_t>( timestamp - current->timestamp ) > age )
            {
#ifdef _DEBUG
            core_mac_address_s bssid(
                current->ap_data->bssid() );

            DEBUG7( "core_scan_list_c::remove_entries_by_age() - BSSID %02X:%02X:%02X:%02X:%02X:%02X [%u second(s)] removed",
                bssid.addr[0], bssid.addr[1], bssid.addr[2],
                bssid.addr[3], bssid.addr[4], bssid.addr[5],
                static_cast<u32_t>( timestamp - current->timestamp ) / SECONDS_FROM_MICROSECONDS );
#endif // _DEBUG

            iter.remove();

            delete current->ap_data;
            delete current;
            current = NULL;
            }
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_scan_list_c::set_tag(
    u8_t tag )
    {
    for( core_scan_list_entry_s* iter = scan_list_m.first(); iter; iter = scan_list_m.next() )
        {
        iter->tags |= tag;
        }    
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//       
void core_scan_list_c::clear_tag(
    u8_t tag )
    {
    for( core_scan_list_entry_s* iter = scan_list_m.first(); iter; iter = scan_list_m.next() )
        {
        iter->tags &= ~tag;
        }    
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//               
void core_scan_list_c::clear_all_tags()
    {
    for( core_scan_list_entry_s* iter = scan_list_m.first(); iter; iter = scan_list_m.next() )
        {
        iter->tags = 0;
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_scan_list_c::print_contents()
    {
    u32_t size( sizeof( *this ) );
    u64_t timestamp = core_am_tools_c::timestamp();

    for( core_scan_list_entry_s* iter = scan_list_m.first(); iter; iter = scan_list_m.next() )
        {
        const core_mac_address_s bssid(
            iter->ap_data->bssid() );
        core_ssid_s ssid(
            iter->ap_data->ssid() );    

        DEBUG6( "core_scan_list_c::print_contents() - BSSID %02X:%02X:%02X:%02X:%02X:%02X",
            bssid.addr[0], bssid.addr[1], bssid.addr[2],
            bssid.addr[3], bssid.addr[4], bssid.addr[5] );                
        DEBUG1S( "core_scan_list_c::print_contents() - SSID ",
            ssid.length, &ssid.ssid[0] );
        DEBUG1( "core_scan_list_c::print_contents() - age %u second(s)",
            static_cast<u32_t>( timestamp - iter->timestamp ) / SECONDS_FROM_MICROSECONDS );
        DEBUG1( "core_scan_list_c::print_contents() - tags 0x%02X",
            iter->tags );

        DEBUG( "core_scan_list_c::print_contents()" );
        
        size += sizeof( *iter ) + sizeof( *iter->ap_data ) +
                sizeof( *iter->ap_data->frame() ) + iter->ap_data->frame()->data_length();
        }

    DEBUG2( "core_scan_list_c::print_contents() - total size for scan list is %u bytes with %u AP(s)",
        size, count() );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_scan_list_c::get_channels_by_ssid(
    core_scan_channels_c& channels,
    const core_ssid_s& ssid )
    {
    DEBUG( "core_scan_list_c::get_channels_by_ssid()" );
    
    core_type_list_iterator_c<core_scan_list_entry_s> iter( scan_list_m );
    for( core_scan_list_entry_s* current = iter.first(); current; current = iter.next() )
        {
        if ( current->ap_data->ssid() == ssid )
            {
            channels.add(
                current->ap_data->band(),
                current->ap_data->channel() );
            }
        }

    DEBUG2( "core_scan_list_c::get_channels_by_ssid() - channels 0x%02X%02X",
        channels.channels().channels2dot4ghz[1],
        channels.channels().channels2dot4ghz[0] );
    }
