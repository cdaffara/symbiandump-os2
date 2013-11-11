/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of the ScanList class.
*
*/


#include "genscanlist.h"
#include "genscanoffsets.h"
#include "am_debug.h"

/** The size IE header. */
const u8_t IE_HEADER_SIZE = 2;
/** The offset for element ID field of an IE. */
const u8_t IE_OFFSET_ELEMENT_ID = 0;
/** The offset for length field of an IE. */
const u8_t IE_OFFSET_LENGTH = 1;
/** The number of mandatory IEs. */
const u8_t MANDATORY_IE_LIST_SIZE = 3;
/** Array of mandatory IEs and their minimum and maximum lengths. */
const u8_t MANDATORY_IE_LIST[][3] =
    { { 0, 0, 32 },     // SSID
      { 1, 1, 255 },    // Supported Rates
      { 3, 1, 255 } };  // DS
/** Indexes for the IE array. */
const u8_t MANDATORY_IE_ID = 0;
const u8_t MANDATORY_IE_MIN_LENGTH = 1;
const u8_t MANDATORY_IE_MAX_LENGTH = 2;

/** Defining this enables memory allocation related traces. */
//#define SCANLIST_DEEP_DEBUG 1

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
ScanList::ScanList() :
    data_m( NULL ),
    granularity_m( SCAN_LIST_DEFAULT_GRANULARITY ),
    count_m( 0 ),
    size_m( 0 ),
    current_max_size_m( 0 ),
    max_size_m( SCAN_LIST_DEFAULT_MAX_SIZE )
    {
    DEBUG( "ScanList::ScanList()" );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
ScanList::ScanList(
    u32_t max_size,
    u32_t granularity ) :
    data_m( NULL ),
    granularity_m( granularity ),
    count_m( 0 ),
    size_m( 0 ),
    current_max_size_m( 0 ),
    max_size_m( max_size )
    {
    DEBUG( "ScanList::ScanList()" );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
ScanList::~ScanList()
    {
    DEBUG( "ScanList::~ScanList()" );
    
    delete data_m;
    data_m = NULL;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u32_t ScanList::Append(
    u32_t data_length,
    const ScanFrame* data )
    {
    DEBUG( "ScanList::Append()" );

#ifdef SCANLIST_DEEP_DEBUG
    DEBUG1( "ScanList::Append() - data_length %u",
        data_length );
    DEBUG1( "ScanList::Append() - count_m %u",
        count_m );
    DEBUG1( "ScanList::Append() - current_max_size_m %u",
        current_max_size_m );
    DEBUG1( "ScanList::Append() - size_m %u",
        size_m );

    DEBUG1( "ScanList::Append() - data_length w/ padding %u",
        AddPadding( data_length  ) );
    DEBUG1( "ScanList::Append() - size left %u",
        current_max_size_m - size_m );
#endif // SCANLIST_DEEP_DEBUG
    
    if( !current_max_size_m ||
        AddPadding( data_length  ) >= ( current_max_size_m - size_m ) )
        {
        /**
         * Grow the container by granularity_m bytes.
         */
        const u32_t prev_max_size( current_max_size_m );
        SetCurrentMaxSize( current_max_size_m + granularity_m );
        if ( current_max_size_m == prev_max_size )
            {
            return APPEND_FAILED_NO_MEMORY;
            }
        }

    /**
     * Copy the frame to the container.
     */
    Copy(
        &data_m[size_m],
        data,
        data_length );

    /**
     * Make sure the frame is valid before accepting it.
     */
    if ( CheckData( &data_m[size_m] ) )
        {
        size_m += AddPadding( data_length );
        ++count_m;
        }

    return size_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u32_t ScanList::SetCurrentMaxSize(
    u32_t max_size )
    {
    DEBUG( "ScanList::SetCurrentMaxSize()" );
    
#ifdef SCANLIST_DEEP_DEBUG    
    DEBUG1( "ScanList::SetCurrentMaxSize() - max_size %u",
        max_size );
    DEBUG1( "ScanList::SetCurrentMaxSize() - current_max_size_m %u",
        current_max_size_m );
    DEBUG1( "ScanList::SetCurrentMaxSize() - max_size_m %u",
        max_size_m );        
    DEBUG1( "ScanList::SetCurrentMaxSize() - size_m %u",
        size_m );
#endif // SCANLIST_DEEP_DEBUG

    if ( !max_size )
        {
        ClearAll();

        DEBUG1( "ScanList::SetCurrentMaxSize() - setting current_max_size_m to %u",
            current_max_size_m );

        return current_max_size_m;
        }

    if ( max_size < size_m )
        {
        DEBUG1( "ScanList::SetCurrentMaxSize() - keeping current_max_size_m as %u",
            current_max_size_m );

        return current_max_size_m;
        }

    if ( max_size > max_size_m )
        {
        if ( current_max_size_m == max_size_m )
            {
            DEBUG1( "ScanList::SetCurrentMaxSize() - keeping current_max_size_m as %u",
                current_max_size_m );

            return current_max_size_m;
            }
        else
            {
            max_size = max_size_m;
            }
        }

    u8_t* buffer = new u8_t[max_size];
    if ( !buffer )
        {
        return current_max_size_m;
        }

    if ( data_m )
        {
        Copy(
            &buffer[0],
            &data_m[0],
            size_m );

        delete data_m;
        data_m = NULL;
        }

    data_m = buffer;
    current_max_size_m = max_size;

    DEBUG1( "ScanList::SetCurrentMaxSize() - setting current_max_size_m to %u",
        current_max_size_m );

    return current_max_size_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t ScanList::CheckData(
    const ScanFrame* data ) const
    {
    /**
     * Calculate the length of the data.
     */
    u16_t size = static_cast<u16_t>(
      *( reinterpret_cast<const u32_t*>( data + CNTRL_LENGTH_OFFSET ) ) + CNTRL_HEADER_LEN );

    const u8_t* current_ie = NULL;

    /**
     * Search mandatory IEs.
     */
    for ( u8_t i( 0 ); i < MANDATORY_IE_LIST_SIZE; ++i )
        {
        /*
         * Go through all IEs.
         */
        bool_t is_found( false_t );
        for ( current_ie = data + MGMT_BODY_OFFSET;
              !is_found;
              current_ie += current_ie[IE_OFFSET_LENGTH] + IE_HEADER_SIZE )
            {
            if ( current_ie < data + size )
                { /** Still searching... */
                if ( current_ie[IE_OFFSET_ELEMENT_ID] == MANDATORY_IE_LIST[i][MANDATORY_IE_ID] &&
                    current_ie[IE_OFFSET_LENGTH] >= MANDATORY_IE_LIST[i][MANDATORY_IE_MIN_LENGTH] &&
                    current_ie[IE_OFFSET_LENGTH] <= MANDATORY_IE_LIST[i][MANDATORY_IE_MAX_LENGTH] )
                    { /** We have a match! Lets find out the next one. */
                    is_found = true_t;
                    }
                }
            else
                { /** The IE was not found. */
                DEBUG1( "ScanList::CheckData() - Element ID %u not found, ignoring frame",
                    MANDATORY_IE_LIST[i][MANDATORY_IE_ID] );

                return false_t;
                }
            }
        }

    /**
     * Data is OK.
     */
    return true_t;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u8_t* ScanList::Copy(
    void* target,
    const void* source,
    u32_t data_length )
    {
    if ( data_length <= 0 )
        {
        return reinterpret_cast<u8_t*>( target );
        }

    if ( target == source )
        {
        return reinterpret_cast<u8_t*>( target ) + data_length;
        }

    /**
     * Ensure areas do not overlap.
     */
    if ( target > source )
        {
        ASSERT( reinterpret_cast<u8_t*>( target ) >= ( reinterpret_cast<const u8_t*>( source ) + data_length ) );
        }
    else
        {
        ASSERT( reinterpret_cast<const u8_t*>( source ) >= ( reinterpret_cast<u8_t*>( target ) + data_length ) );
        }

    for ( u32_t i( 0 ); i < data_length; ++i )
        {
        *( reinterpret_cast<u8_t*>( target ) + i ) = *( reinterpret_cast<const u8_t*>( source ) + i );
        }

    return reinterpret_cast<u8_t*>( target ) + data_length;
    }
