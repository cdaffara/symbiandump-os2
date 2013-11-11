/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  THookCleaner implementation
*
*/

// INCLUDES
#include "analyzetoolcleaner.h"
#include <e32debug.h>
#include "atlog.h"

// Global cleaner object
static THookCleaner cleaner;

// ordinal 1
EXPORT_C ATCLEANERTABLE* GetCallBackAddress()
    {
    LOGSTR1( "ATC GetCallBackAddress()" );
    
    return &( cleaner.iTable );
    }

// -----------------------------------------------------------------------------
// THookCleaner::THookCleaner()
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
THookCleaner::THookCleaner()
    {
    LOGSTR1( "ATC THookCleaner::THookCleaner()" );
    
    iTable.Reset();
    }

// -----------------------------------------------------------------------------
// THookCleaner::~THookCleaner()
// Destructor.
// -----------------------------------------------------------------------------
//
THookCleaner::~THookCleaner()
    {
    LOGSTR1( "ATC THookCleaner::~THookCleaner()" );
    
    for ( TUint i = 0; i < ATCLEANERTABLESIZE; i++ )
        {
        if ( iTable.At( i ) )
            {
            TAnalyzeToolCleanerBase* obj =
                    ( TAnalyzeToolCleanerBase* ) iTable.At( i );
            obj->Cleanup();
            }
        }
    }

// End of File
