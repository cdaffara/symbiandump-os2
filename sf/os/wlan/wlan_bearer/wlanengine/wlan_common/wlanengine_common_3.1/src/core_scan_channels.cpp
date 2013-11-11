/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class for manipulating scan channels masks.
*
*/


#include "core_scan_channels.h"
#include "core_tools.h"
#include "am_debug.h"

const u16_t SCAN_BAND_2DOT4GHZ_MASK = 0x1FFF;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_scan_channels_c::core_scan_channels_c() :
    channels2dot4ghz_m( 0 )
    {
    DEBUG( "core_scan_channels_c::core_scan_channels_c()" );

    core_tools_c::fillz(
        reinterpret_cast<u8_t*>( &channel_mask_m ),
        sizeof( channel_mask_m ) );       
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_scan_channels_c::core_scan_channels_c(
    const core_scan_channels_s& channels ) :
    channels2dot4ghz_m( 0 )
    {
    DEBUG( "core_scan_channels_c::core_scan_channels_c() (channels)" );

    set( channels );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_scan_channels_c::core_scan_channels_c(
    const core_scan_channels_c& channels ) :
    channels2dot4ghz_m( 0 )
    {
    DEBUG( "core_scan_channels_c::core_scan_channels_c() (channels)" );

    set( channels );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_scan_channels_c::~core_scan_channels_c()
    {
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
const core_scan_channels_s& core_scan_channels_c::channels()
    {
    if ( channel_mask_m.band & SCAN_BAND_2DOT4GHZ )
        {
        core_tools_c::copy(
            &channel_mask_m.channels2dot4ghz[0],
            reinterpret_cast<u8_t*>( &channels2dot4ghz_m ),
            sizeof( channels2dot4ghz_m ) );
        }

    return channel_mask_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_scan_channels_c::set(
    const core_scan_channels_s& channels )
    {
    channel_mask_m = channels;
    channels2dot4ghz_m = 0;

    if ( channel_mask_m.band & SCAN_BAND_2DOT4GHZ )
        {
        core_tools_c::copy(
            reinterpret_cast<u8_t*>( &channels2dot4ghz_m ),
            &channels.channels2dot4ghz[0],            
            sizeof( channels2dot4ghz_m ) );         
        }        
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_scan_channels_c::set(
    const core_scan_channels_c& channels )
    {
    channel_mask_m.band = channels.channel_mask_m.band;
    channels2dot4ghz_m = 0;

    if ( channel_mask_m.band & SCAN_BAND_2DOT4GHZ )
        {
        channels2dot4ghz_m = channels.channels2dot4ghz_m;
        }    
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_scan_channels_c::merge(
    const core_scan_channels_c& channels )
    {
    channel_mask_m.band |= channels.channel_mask_m.band;

    if ( channels.channel_mask_m.band & SCAN_BAND_2DOT4GHZ )
        {
        channels2dot4ghz_m |= channels.channels2dot4ghz_m;
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_scan_channels_c::add(
    u8_t band,
    u8_t channel )
    {
    if ( band & SCAN_BAND_2DOT4GHZ &&
         channel &&
         channel <= SCAN_BAND_2DOT4GHZ_MAX_CHANNEL_EURO )
        {
        channel_mask_m.band |= band;

        channels2dot4ghz_m |= static_cast<u16_t>( 1 << ( channel - 1 ) );
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_scan_channels_c::invert_channels()
    {
    channels2dot4ghz_m ^= SCAN_BAND_2DOT4GHZ_MASK;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_scan_channels_c::remove(
	    u8_t band,
	    u8_t channel )
	    {
	    if ( band & SCAN_BAND_2DOT4GHZ &&
	         channel &&
	         channel <= SCAN_BAND_2DOT4GHZ_MAX_CHANNEL_EURO )
	        {
	        u16_t mask = static_cast<u16_t>( 1 << ( channel - 1 ) );
	        mask ^= SCAN_BAND_2DOT4GHZ_MASK;
	        channels2dot4ghz_m &= mask;
	        
	        if( channels2dot4ghz_m == 0 )
	            {
	            channel_mask_m.band &= ( ~SCAN_BAND_2DOT4GHZ );
	            }        	
	        }
	    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_scan_channels_c::is_empty(
    u8_t band )
    {
    if ( ( band & SCAN_BAND_2DOT4GHZ ) &&
         ( channel_mask_m.band & SCAN_BAND_2DOT4GHZ ) &&
         ( channels2dot4ghz_m & SCAN_BAND_2DOT4GHZ_MASK ) )
        {
        return false_t;
        }

    return true_t;
    }
