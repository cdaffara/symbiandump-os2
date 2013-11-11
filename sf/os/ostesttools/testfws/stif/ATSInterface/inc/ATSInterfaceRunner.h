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
* CATSInterfaceRunner.
*
*/

#ifndef ATS_INTERFACE_RUNNER_H
#define ATS_INTERFACE_RUNNER_H


//  INCLUDES
#include <e32std.h>
#include <e32base.h>
#include "ATSInterface.h"

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
// CATSInterfaceRunner is a class that is used to run test cases using
// STIF Test Framework.

class CATSInterfaceRunner
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
        static CATSInterfaceRunner* NewL( CATSInterface* aATSInterface,
             TTestInfo& aTestInfo );

        /**
        * Destructor of CATSInterfaceRunner.
        */
        ~CATSInterfaceRunner();

    public: // New functions

        /**
        * Run Tests.
        */
        TInt RunTestsL();

        /**
        * Start testing.
        */
        void StartTestL();

    public: // Functions from base classes

        /**
        * From CActive RunL handles request completion.
        */
        void RunL();

        /**
        * From CActive DoCancel handles request cancellation.
        */
        void DoCancel();

        /**
        * From CActive RunError handles error situations.
        */
        TInt RunError( TInt aError );

    protected: // New functions
        // None

    protected: // Functions from base classes
        // None

    private: // New functions

        /** 
        * C++ default constructor.
        */
        CATSInterfaceRunner( CATSInterface* aATSInterface,
            TTestInfo& aTestInfo );

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();


    public: //Data
        // None
    
    protected: // Data
        // None

    private: // Data
        
        // Pointer to ATSInterface
        CATSInterface*                  iATSInterface;

        // Handle to Test Engine Server
        RTestEngineServer               iTestEngineServ;

        // Handle to Test Engine
        RTestEngine                     iTestEngine;

        // Handle to Test Case
        RTestCase                       iTestCase;

        // Test info and package
        TTestInfo                       iTestInfo;
        TTestInfoPckg                   iTestInfoPckg;

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


// DESCRIPTION
// CActiveTimer is used to print running seconds to Console window
class CActiveTimer
        : public CTimer
    {
    public: // Enumerations
        // None

    private: // Enumerations
        // None

    public: // Constructors and destructor

        /**
        * NewL is two-phased constructor.
        */
        static CActiveTimer* NewL( CConsoleBase* aConsole );

        /**
        * Destructor of CActiveTimer.
        */
        ~CActiveTimer();

    public: // New functions

        /**
        * Start timer.
        */
        void StartL();

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
        CActiveTimer( CConsoleBase* aConsole );

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();


    public: // Data
        // None
    
    protected: // Data
        // None

    private: // Data
        
        // Pointer to Console
        CConsoleBase*                   iConsole;

        // Start Time
        TTime                           iStartTime;

        // Console position
        TInt                            iXPos;
        TInt                            iYPos;

    public: // Friend classes
        // None

    protected: // Friend classes
        // None

    private: // Friend classes
        // None

    };

#endif // ATS_INTERFACE_RUNNER_H

// End of File
