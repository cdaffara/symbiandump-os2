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
* Description: This file contains the keywords for the 
* TestScripter module.
*
*/

#ifndef TESTKEYWORDS_H
#define TESTKEYWORDS_H

//  INCLUDES
#include <e32base.h>
#include <StifTestInterface.h>

// CONSTANTS
// None

// MACROS
// None

// DATA TYPES
typedef TPtrC (*KeywordFunc)( TInt aKeyword );

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// CLASS DECLARATION

// DESCRIPTION
// TestScripter Keyword class
class TTestKeywords 
    {
    public: // Enumerations
        // Keyword enum
        enum TKeywords
            {
            ETitle,
            ETimeout,
            EPriority,
            ECreate,
            EDelete,
            ERequest,
            EWait,
            ERelease,
            ESet,
            EUnset,
            EPrint,
            EAllowNextResult,
            EWaitTestClass,
            ECreateKernel,
            EPause,
            ELoop,
            EEndLoop,
            EOOMIgnoreFailure,
            EOOMHeapFailNext,
            EOOMHeapSetFail,
            EOOMHeapToNormal,
            ETestInterference,
            EMeasurement,
            EAllowErrorCodes,
            EBringToForeground,
            ESendToBackground,
            EPressKey,
            ETypeText,
            EVar,
            ECallSub,
            ECreateX,
            ESetResultDescription,
            ESendPointerEvent,
            };

        // priorities
        enum TPriority
            {
            EPriHigh,
            EPriNormal,
            EPriLow,
            };

    private: // Enumerations

    public: // Constructors and destructor

    public: // Constructors and destructor

    public: // New functions

        /**
        * Returns a string desrciptor corresponding to keyword number. 
        */
        static TPtrC Keyword( TInt aKeyword );

        /**
        * Returns a string desrciptor corresponding to priority 
        * enum value. 
        */
        static TPtrC Priority( TInt aArg );

        /**
        * Returns a keyword enum corresponding to keyword 
        * string descriptor. 
        */
        static TInt Parse( TDesC& aKeyword, KeywordFunc aFunc );

    public: // Functions from base classes

    protected:  // New functions

    protected:  // Functions from base classes

    private:    // New functions

    public:     // Data

    protected:  // Data

    private:    // Data

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

    };

#endif        // TESTKEYWORDS_H

// End of File
