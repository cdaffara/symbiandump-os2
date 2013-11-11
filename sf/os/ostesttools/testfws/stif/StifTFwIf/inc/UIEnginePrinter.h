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
* CUIEnginePrinter.
*
*/

#ifndef STIF_TFW_IF_PRINTER_H
#define STIF_TFW_IF_PRINTER_H


//  INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <stifinternal/UIEngineContainer.h>

// CONSTANTS

// Interval for timer printing
const TInt KPrintInterval = 1000000;

// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
// None


// FORWARD DECLARATIONS
// None


// CLASS DECLARATION


// DESCRIPTION

// CUIEnginePrinter is an active object which handles the print notifications
// from the test case.
class CUIEnginePrinter
        : public CActive
    {
    public: // Enumerations

    private: // Enumerations

    public:  // Constructors and destructor

        /**
        * NewL is first phase of two-phased constructor.
        */
        static CUIEnginePrinter* NewL( CUIEngineContainer* aUIEngineContainer );

        /**
        * Destructor of CTestCaseRunner.
        */
        virtual ~CUIEnginePrinter();

        /**
        * Start
        */
        void StartL( RTestCase& aTestCase );

    public: // New functions
    
        inline TBool IsRunning(){ return iRunning; };

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
        CUIEnginePrinter( CUIEngineContainer* aUIEngineContainer );

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL( );

    private:   // Functions from base classes
        // None

    public:   //Data

    protected:  // Data
        // None

    private:    // Data

        // Pointer to test case container
        CUIEngineContainer*             iUIEngineContainer;

        // Testcase object handle
        RTestCase                       iTestCase;
        
        // Test result package
        TTestProgress                   iProgress;
        TTestProgressPckg               iProgressPckg;
        
        TBool                           iRunning;

    public:     // Friend classes
        // None

    protected:  // Friend classes
        // None

    private:    // Friend classes
        // None

    };



#endif // STIF_TFW_IF_PRINTER_H

// End of File
