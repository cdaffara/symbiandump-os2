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
* Description:  Implementation of ScanList inline methods.
*
*/


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline u32_t ScanList::Count() const
    {
    return count_m;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline u32_t ScanList::Size() const
    {
    return size_m;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline u32_t ScanList::MaxSize() const
    {
    return max_size_m;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline u8_t* ScanList::Data() const
    {
    return data_m;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void ScanList::Update(
    u32_t count,
    u32_t size )
    {
    count_m = count;
    size_m = size;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline void ScanList::ClearAll()
    {
    delete data_m;
    data_m = NULL;
    count_m = 0;
    size_m = 0;
    current_max_size_m = 0;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline u32_t ScanList::AddPadding(
    u32_t data_length )
    {
    /** @note size & 0x3 has the same result as size % 4 */
    return ( data_length & 0x3 ) ? data_length + ( 4 - data_length & 0x3 ) : data_length;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline u32_t ScanList::Compress()
    {
    return SetCurrentMaxSize( size_m );
    }
