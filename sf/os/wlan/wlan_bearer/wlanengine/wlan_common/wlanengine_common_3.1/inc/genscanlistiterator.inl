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
* Description:  Implementation of ScanListIterator inline methods.
*
*/


#include "genscanoffsets.h"

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline u16_t ScanListIterator::Size() const
    {
    if ( !scan_list_m.size_m )
        {
        return 0;
        }

    return static_cast<u16_t>( CNTRL_HEADER_LEN + 
        *( reinterpret_cast<const u32_t*>( current_m + CNTRL_LENGTH_OFFSET ) ) );
    }
