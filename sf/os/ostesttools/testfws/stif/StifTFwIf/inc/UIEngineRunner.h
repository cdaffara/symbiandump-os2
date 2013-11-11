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
* CUIEngineRunner.
*
*/


#ifndef STIF_TFW_IF_RUNNER_H
#define STIF_TFW_IF_RUNNER_H


//  INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <stifinternal/UIEngineContainer.h>

// CONSTANTS
// None

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

// CUIEngineRunner is a class that is used to run test cases using
// STIF Test Framework.
class CUIEngineRunner
        : public CActive
    {
    public: // Enumerations
        // None

    private: // Enumerations
        // None

    public: // Constructors and destructor

        /**
        * NewL is two-phased constructor.
        */
        static CUIEngineRunner* NewL( CUIEngineContainer* aUIEngineContainer );

        /**
        * Destructor of CUIEngineRunner.
        */
        ~CUIEngineRunner();

    public: // New functions

        /**
        * StartTestL starts testing.
        */
        void StartL( RTestCase& aTestCase );
        
        /**
        * Test result.
        */
        inline TFullTestResult& FullResult(){ return iFullTestResult; };

    public: // Functions from base classes

        /**
        * From CActive RunL handles request completion.
        */
        void RunL();

        /**
        * From CActive DoCancel handles request cancellation.
        */
        void DoCancel();

    protected: // New functions
        // None

    protected: // Functions from base classes
        // None

    private: // New functions

        /** 
        * C++ default constructor.
        */
        CUIEngineRunner( CUIEngineContainer* aUIEngineContainer);

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();


    public: //Data
        // None
    
    protected: // Data
        // None

    private: // Data

        // Pointer to UIEngine
        CUIEngineContainer*            iUIEngineContainer;

        // Handle to Test Case
        RTestCase                       iTestCase;
        
        // Test result and package
        TFullTestResult                 iFullTestResult;
        TFullTestResultPckg             iFullTestResultPckg;

    public: // Friend classes
        // None

    protected: // Friend classes
        // None

    private: // Friend classes
        // None

    };



#endif // STIF_TFW_IF_RUNNER_H

// End of File
