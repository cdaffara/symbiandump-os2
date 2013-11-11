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
* Description:  Definitions for the class TAnalyzeToolCleanerBase.
*
*/

#ifndef __ANALYZETOOLCLEANER_H__
#define __ANALYZETOOLCLEANER_H__

// INCLUDES
#include <e32std.h>

// CONSTANTS
#define ATCLEANERTABLESIZE 10
#define ATCLEANERTABLE TFixedArray<TUint32, ATCLEANERTABLESIZE>
_LIT( KATCleanerDllName, "atoolcleaner.dll" );

// CLASS DECLARATION

/**
*  Cleaner base class
*/
class TAnalyzeToolCleanerBase
    {
public:
    /**
    * Cleanup function which uninstall allocator
    */
    virtual void Cleanup() = 0;
    };

// CLASS DECLARATION

/**
*  Cleaner class
*/
class THookCleaner
    {
public:
    /**
    * C++ default constructor.
    */
    THookCleaner();
    
    /**
    * Destructor.
    */
    ~THookCleaner();
    
    ATCLEANERTABLE iTable;
    };
#endif // __ANALYZETOOLCLEANER_H__
