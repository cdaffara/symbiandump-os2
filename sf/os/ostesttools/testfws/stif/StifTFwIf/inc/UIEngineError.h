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
* Description: This file contains the header file of the 
* CUIEngineErrorPrinter.
*
*/

#ifndef UI_ENGINE_ERROR_H
#define UI_ENGINE_ERROR_H


//  INCLUDES
#include <e32std.h>
#include <e32base.h>
#include "TestEngineClient.h"

// CONSTANTS
// None

// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
// None


// FORWARD DECLARATIONS
class CUIEngine;

// CLASS DECLARATION


// DESCRIPTION

// CUIEngineErrorPrinter is an active object which handles the error notifications
// from the test framework.
class CUIEngineErrorPrinter
        :public CActive
    {
    public: // Enumerations

    private: // Enumerations

    public:  // Constructors and destructor

        /**
        * NewL is first phase of two-phased constructor.
        */
        static CUIEngineErrorPrinter* NewL( CUIEngine* aUIEngine );

        /**
        * Destructor of CTestCaseRunner.
        */
        virtual ~CUIEngineErrorPrinter();

        /**
        * Start
        */
        void StartL( RTestEngine& aTestCase );

    public: // New functions

    public: // Functions from base classes

        /**
        * RunL derived from CActive handles the completed requests.
        */
        void RunL();

        /**
        * DoCancel derived from CActive handles the Cancel
        */
        void DoCancel();

    protected:  // New functions
        // None

    protected:  // Functions from base classes
        // None

        /** 
        * C++ default constructor.
        */
        CUIEngineErrorPrinter( CUIEngine* aUIEngine );

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL( );

    private:   // Functions from base classes
        // None

    public:   //Data
        // None
    
    protected:  // Data
        // None

    private:    // Data

        // Pointer to test case container
        CUIEngine*                      iUIEngine;

        // Testcase object handle
        RTestEngine                     iTestEngine;
        
        // Test result package
        TErrorNotification              iError;
        TErrorNotificationPckg          iErrorPckg;

    public:     // Friend classes
        // None

    protected:  // Friend classes
        // None

    private:    // Friend classes
        // None

    };

#endif // UI_ENGINE_ERROR_H

// End of File
