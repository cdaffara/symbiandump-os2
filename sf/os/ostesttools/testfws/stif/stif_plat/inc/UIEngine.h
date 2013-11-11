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
* Description: This file contains the header file of the UIEngine.
*
*/

#ifndef UI_ENGINE_H
#define UI_ENGINE_H

//  INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <StifLogger.h>
#include <StifTFwIf.h>

#include <TestEngineClient.h>
#include <StifTFwIfProt.h>


// CONSTANTS
// Log dir and file
_LIT( KUiLogDir, "\\Logs\\TestFramework\\ui\\" );
_LIT( KUiLogFile, "ui.txt" );

// TestScripter related informations
const TInt KTestScripterNameLength = 12;    
_LIT( KTestScripterName, "testscripter" );  
_LIT( KTestCombinerName, "testcombiner" ); 
                                            

// MACROS
// None

// DATA TYPES

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
class CUIEngineContainer;
class CUIEngineEvent;
class CUIEngineErrorPrinter;

// CLASS DECLARATION

// DESCRIPTION
    
// CUIIf is the interface class of STIF Test Framework UI engine 
class CUIIf 
    :public CBase
    {
    public: // Functions that the derived class may implement.

        /**
        * Test case executed.
        *
        * Returns Symbian OS error code.
        */
        virtual void TestExecuted ( CUIEngineContainer* aContainer,
                                    TFullTestResult& aFullTestResult ) = 0;

        /**
        * Progress information from Test case execution.
        *
        * Returns Symbian OS error code.
        */
        virtual TInt PrintProg ( CUIEngineContainer* /* aContainer */,
                                 TTestProgress& /* aProgress */ )
                                 { return KErrNotSupported; };
                                 
        /**
        * Error information from Test Framework.
        *
        * Returns Symbian OS error code.
        */
        virtual TInt ErrorPrint ( TErrorNotification& /* aError */ )
                                  { return KErrNotSupported; };
    
        /**
        * Forward remote messages.
        */
        virtual TInt RemoteMsg( CUIEngineContainer* /* aContainer */,
                                const TDesC& /* aMessage */ )
                                 { return KErrNotSupported; };

        /**
        * Forward reboot indication.
        */
        virtual TInt GoingToReboot( CUIEngineContainer* /* aContainer */,
                                    TRequestStatus& /* aStatus */ )
                                 { return KErrNotSupported; };

        /**
        * Initialize logger.
        */
        virtual void InitializeLogger( CStifLogger* /*aLogger*/ ){};
                             
    };


// CUIEngine is the main class of STIF Test Framework UI engine 
class CUIEngine 
    :public CBase
    {
    public: // Enumerations

    protected: // Enumerations
        
    private: // Enumerations

    public:  // Constructors and destructor
        
        static CUIEngine* NewL( CUIIf* aUi );
        
        /**
        * Destructor.
        */
        virtual ~CUIEngine();

    public: // New functions

        RTestEngine& TestEngine(){ return iTestEngine; }

        /**
        * Used to open TestFramework.
        *
        * Returns Symbian OS error code.
        */
        TInt Open( const TDesC& aTestFrameworkIni );

        /**
        * Used to close TestFramework.
        *
        * Returns Symbian OS error code.
        */
        TInt Close();

        /**
        * Used to add test module to test framework.
        *
        * Returns Symbian OS error code.
        */
        TInt AddTestModule( const TDesC& aModuleName, 
                            const TDesC& aIniFile );

        /**
        * Used to remove test module to test framework.
        *
        * Returns Symbian OS error code.
        */
        TInt RemoveTestModule( const TDesC& aModuleName );

        /**
        * Used to add test case file for test module.
        *
        * Returns Symbian OS error code.
        */
        TInt AddTestCaseFile( const TDesC& aModuleName,
                              const TDesC& aCaseFile );

        /**
        * Used to remove test case file from test module.
        *
        * Returns Symbian OS error code.
        */
        TInt RemoveTestCaseFile( const TDesC& aModuleName,
                                 const TDesC& aCaseFile );

        /**
        * Get testcases from test modules. Returns test case information 
        * in aTestinfo.
        * Test module name and test case file can be used to limit the 
        * set of test cases returned.
        *
        * Returns Symbian OS error code.
        */
        TInt GetTestCasesL( RPointerArray<CTestInfo>& aTestInfo,
                            const TDesC& aTestModule = (TDesC&) KNullDesC(),
                            const TDesC& aTestCaseFile = (TDesC&) KNullDesC() );

        /**
        * Start selected test case identified with CTestInfo.
        * Return TestId, which is handle used in subsequent 
        * test case execution control.
        *
        * Returns Symbian OS error code.
        */
        TInt StartTestCase( CUIEngineContainer*& aContainer,
                            const CTestInfo& aTestInfo );
    
        /**
        * Abort started test case.
        */
        void AbortStartedTestCase( CUIEngineContainer* aContainer );

    protected: // Functions that the derived class may implement.

    private:
    
        /**
        * C++ default constructor.
        */
        CUIEngine( CUIIf* aUi );

        /**
        * Second phase constructor.
        */
        void ConstructL();

        /**
        * Test case executed.
        *
        * Returns Symbian OS error code.
        */
        void TestExecuted ( CUIEngineContainer* aContainer,
                            TFullTestResult& aFullTestResult );

        /**
        * Progress information from Test case execution.
        *
        * Returns Symbian OS error code.
        */
        TInt PrintProg ( CUIEngineContainer* aContainer,
                         TTestProgress& aProgress );
                         
        /**
        * Error information from Test framework.
        *
        * Returns Symbian OS error code.
        */
        TInt ErrorPrint ( TErrorNotification& aError );

        /**
        * Forward remote messages.
        */
        TInt RemoteMsg( CUIEngineContainer* aContainer,
                        const TDesC& aMessage );

        /**
        * Forward reboot indication..
        */
        TInt GoingToReboot( CUIEngineContainer* aContainer, TRequestStatus& aStatus );

        
    public: // Data

        // Logger 
        CStifLogger*                            iLogger;

    protected: // Data
        // None
        
    private: // Data
        
        /**
        * Handle to UI.
        */
        CUIIf*                                  iUi; 

        // Handle to Test Engine Server
        RTestEngineServer                       iTestEngineServ;

        // Handle to Test Engine
        RTestEngine                             iTestEngine;
        
        // Container array
        RPointerArray<CUIEngineContainer>       iContainerArray;
        
        // Error printer
        CUIEngineErrorPrinter*                  iError;
                
    public: // Friend classes
        // None
        
    protected: // Friend classes
        // None

    private: // Friend classes
        friend class CUIEngineContainer;
        friend class CUIEngineEvent;
        friend class CUIEngineErrorPrinter;
    };

#endif      // UI_ENGINE_H 

// End of File
