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
* Description: This file contains the header file of the CATSInterface.
*
*/

#ifndef ATS_INTERFACE_H
#define ATS_INTERFACE_H


//  INCLUDES
#include <e32std.h>
#include <e32base.h>
#include "TestEngineClient.h"
#include "TestModuleInfo.h"

// CONSTANTS
// None

// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
// None


// FORWARD DECLARATIONS
class CATSInterfaceRunner;

// CLASS DECLARATION


// DESCRIPTION
// CATSInterface is the main class of ATS Interface

class CATSInterface
        : public CBase
    {
    public: // Enumerations
        // None

    private: // Enumerations
        // None    

    public: // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CATSInterface* NewL();
        
        /**
        * Destructor.
        */
        ~CATSInterface();

    public: // New functions
        
        /**
        * Run test cases
        */
        void RunTestsL();

        /**
        * Test case is completed
        */
        void TestCompleted( TInt aError );

        /**
        * Return handle to Test Engine Server.
        */
        RTestEngineServer& TestEngineServer();

        /**
        * Return handle to Test Engine.
        */
        RTestEngine& TestEngine();

    public: // Functions from base classes
        // None

    protected: // New functions
        // None

    protected: // Functions from base classes
        // None

    private: // New functions

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

        /**
        * C++ default constructor.
        */
        CATSInterface();

        /**
        * Parse command line
        */
        void ParseCommandLineL();

        /**
        * Run all test cases
        */
        void RunAllTestCasesL();

        /**
        * Run the test case specified by aTestInfo.
        */
        void RunTestCaseL( TTestInfo& aTestInfo );

        /**
        * Run a test case
        */
        TInt RunATestCaseL( CATSInterfaceRunner* aTestCase );

        /**
        * Log errors and leave.
        */
        void LogErrorAndLeaveL( const TDesC& aFunction, 
                                const TDesC& aDescription,
                                const TInt aError );

        /**
        * Parse and search for module info and fill list of modules.
        */
        void ParseTestModulesL(CStifParser* aParser, CTestModuleList* aModuleList, const TDesC& aSectionStart, const TDesC& aSectionEnd);

    protected: // Data
        // None

    private: // Data

        // Console window
        CConsoleBase*           iConsole;

        // Handle to Test Engine Server
        RTestEngineServer       iTestEngineServ;

        // Handle to Test Engine
        RTestEngine             iTestEngine;

        // Complete error for test case
        TInt                    iTestCompletedError;

        // Test Module
        TName                   iTestModule;

        // Name of the Test Module's config file
        TFileName               iConfigFile;

        // Name of the Test Engine's ini file
        TFileName               iEngineIniFile;

        // Name of the Test Module's ini file
        TFileName               iModuleIniFile;

        // List of found modules (included from module given in parameters)
        CTestModuleList*        iModuleList;

    public: // Friend classes
        // None

    protected: // Friend classes
        // None

    private: // Friend classes
        // None

    };


#endif // ATS_INTERFACE_H

// End of File
