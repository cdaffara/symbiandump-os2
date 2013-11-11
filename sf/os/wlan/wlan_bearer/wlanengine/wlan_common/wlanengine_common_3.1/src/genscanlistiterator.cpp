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
* Description:  Implementation of the ScanListIterator class.
*
*/


#include "genscanlist.h"
#include "genscanlistiterator.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
ScanListIterator::ScanListIterator(
    const ScanList& scan_list ) :
    current_m( &scan_list.data_m[0] ),
    scan_list_m( scan_list )
    {
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
const ScanFrame* ScanListIterator::First()
    {
    if ( !scan_list_m.size_m )
        {
        return NULL;
        }

    /**
     * Data exist, set the iterator to the beginning of the container.
     */
    current_m = &scan_list_m.data_m[0];
    return current_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
const ScanFrame* ScanListIterator::Next()
    {
    if ( !scan_list_m.size_m )
        {
        return NULL;
        }
    
    current_m += ScanList::AddPadding( Size() );
    return Current();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
const ScanFrame* ScanListIterator::Current() const
    {
    if( current_m >= scan_list_m.data_m + scan_list_m.size_m )
        {
        return NULL;
        }

    return current_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t ScanListIterator::IsDone() const
    {
    return ( current_m >= scan_list_m.data_m + scan_list_m.size_m );
    }
