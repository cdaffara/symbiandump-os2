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
* Description:  Definitions for the class RAnalyzeToolMemoryAllocator.
*
*/


#include "analyzetoolmemoryallocator.h"
#include "analyzetoolpanics.pan"
#include "atlog.h"

// -----------------------------------------------------------------------------
// RAnalyzeToolMemoryAllocator::RAnalyzeToolMemoryAllocator()
// C++ default constructor.
// -----------------------------------------------------------------------------
//
RAnalyzeToolMemoryAllocator::RAnalyzeToolMemoryAllocator( TBool aNotFirst ) :
    RAllocator(),
    iNotFirst( aNotFirst )
    {
    LOGSTR1( "ATMH RAnalyzeToolMemoryAllocator::RAnalyzeToolMemoryAllocator()" );
    
    // Acquire the original allocator
    iAllocator = &User::Allocator(); 
    
    // Set memory model zero
    iMemoryModel = 0;
    }

// -----------------------------------------------------------------------------
// RAnalyzeToolMemoryAllocator::~RAnalyzeToolMemoryAllocator()
// Destructor.
// -----------------------------------------------------------------------------
//
RAnalyzeToolMemoryAllocator::~RAnalyzeToolMemoryAllocator()
    {
    LOGSTR1( "ATMH RAnalyzeToolMemoryAllocator::~RAnalyzeToolMemoryAllocator()" );
    
    // Call switching orinigal allocator back.
    // Just in case, if uninstall was not called.
    SwitchOriginalAllocator();
    }
    
// -----------------------------------------------------------------------------
// RAnalyzeToolMemoryAllocator::SwitchOriginalAllocator()
// Switches original allocator in use if not already.
// -----------------------------------------------------------------------------
//
void RAnalyzeToolMemoryAllocator::SwitchOriginalAllocator()
    {
    LOGSTR1( "ATMH RAnalyzeToolMemoryAllocator::SwitchOriginalAllocator()" );
    
    if ( iAllocator != &User::Allocator() )
        {
        User::SwitchAllocator( iAllocator );
        }
    }

// End of File
