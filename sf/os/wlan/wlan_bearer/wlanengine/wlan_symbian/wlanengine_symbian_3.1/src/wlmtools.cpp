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
* Description:  Small utilities implemented by the adaptation
*
*/


#include <e32std.h>
#include <e32math.h> 
#include "core_am_tools.h"
#include "am_debug.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
u64_t core_am_tools_c::timestamp()
    {
    TTime currentTime( 0 );
    currentTime.HomeTime();
    
    return currentTime.Int64();   
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
u32_t core_am_tools_c::random()
    {
    TTime currentTime( 0 );
    currentTime.HomeTime();
    
    TInt64 seed = currentTime.Int64();
    return Math::Rand( seed );

    }
