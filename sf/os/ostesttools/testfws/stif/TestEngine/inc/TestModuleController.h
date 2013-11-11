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
* CTestModuleController.
*
*/

#ifndef TEST_MODULE_CONTROLLER_H
#define TEST_MODULE_CONTROLLER_H

//  INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <atslogger.h>
#include <StifTestInterface.h>
#include <stifinternal/TestServerClient.h>

// CONSTANTS
// None

// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
class CTestEngine;
class CBufferArray;
class CTestScripterController;
class CTestModuleController;

// CLASS DECLARATION
// None

// CErrorPrinter is an active object which handles the execute test case
// asynchronous request.
class CErrorPrinter : public CActive
    {
    public:     // Enumerations

    private:    // Enumerations

    public:     // Constructors and destructor

        /**
        * NewL is first phase of two-phased constructor.
        */
        static CErrorPrinter* NewL( CTestEngine* aTestEngine );

        /**
        * Destructor of CErrorPrinter.
        */
        virtual ~CErrorPrinter();

        /**
        * Start
        */
        TInt StartL( RTestModule& aEngine );

    public:     // New functions
        // None

    protected:  // New functions
        // None

    private:
        /** 
        * C++ default constructor.
        */
        CErrorPrinter();

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL( CTestEngine* aEngine );

    public:     // Functions from base classes

        /**
        * RunL derived from CActive handles the completed requests.
        */
        void RunL();

        /**
        * DoCancel derived from CActive handles the Cancel
        */
        void DoCancel();

        /**
        * RunError derived from CActive handles errors from active handler.
        */
        TInt RunError( TInt aError );

    protected:  // New functions
        // None

    protected:  // Functions from base classes
        // None

    private:    // Functions from base classes
       // None

    public:     // Data
        // None
    
    protected:  // Data
        // None

    private:    // Data
         CTestEngine*           iEngine;     // Pointer to main console

         TErrorNotification     iError;      // Error notification
         TErrorNotificationPckg iErrorPckg;  // Error package
         RTestModule            iServer;     // Handle to TestModule

    public:     // Friend classes
        // None

    protected:  // Friend classes
        // None

    private:    // Friend classes
        // None

    };
// CLASS DECLARATION

// DESCRIPTION
// CServerStateHandler class is an active object that handles server state
// e.g. KErrServerTerminated
NONSHARABLE_CLASS( CServerStateHandler )
    : public CActive
    {

    public:  // Enumerations

    private: // Enumerations

    public:  // Constructors and destructor

        /**
        * NewL is first phase of two-phased constructor.
        */
        static CServerStateHandler* NewL( CTestEngine* aTestEngine, CTestModuleController* aTestModuleController );

        /**
        * Destructor of CServerStateHandler.
        */
        virtual ~CServerStateHandler();

    public: // New functions

        /**
        * Start monitoring
        */
        TInt StartL( RTestServer& aServer );

    public: // Functions from base classes

        /**
        * RunL derived from CActive handles the completed requests.
        */
        void RunL();

        /**
        * DoCancel derived from CActive handles the Cancel
        */
        void DoCancel();

        /**
        * RunError derived from CActive handles errors from active handler.
        */
        TInt RunError( TInt aError );

    protected:  // New functions
        // None

    protected:  // Functions from base classes
        // None

    private:

        /** 
        * C++ default constructor.
        */
        CServerStateHandler( CTestEngine* aTestEngine, CTestModuleController* aTestModuleController );
       
        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

    public:   //Data
        // None
    
    protected:  // Data
        // None

    private:    // Data
        CTestEngine*           iEngine;          // Pointer to TestEngine
        RThread                iServerThread;   // Handle to TestServer thread
        CTestModuleController*  iTestModuleController;  //Pointer to TestModuleController
    public:     // Friend classes
        // None

    protected:  // Friend classes
        // None

    private:    // Friend classes
        // None

    };

// DESCRIPTION
// CTestModuleController is an active object used for controlling the test
// module using the RTestModule API.

class CTestModuleController
        : public CActive 
    {
    public:     // Enumerations
        // None

    private:    // Enumerations

    protected:    // Enumerations

        // TTestModuleStatus defines the status of test module
        enum TTestModuleStatus
            {
            ETestModuleIdle,
            ETestModuleEnumerateCases,
            ETestModuleEnumerateCasesCompleted,
            };

    public:     // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CTestModuleController* NewL( CTestEngine* aEngine,
                                            const TName& aName,
                                            TBool aAfterReboot,
                                            TBool aCreateTestScripterCont = EFalse,
                                            CTestScripterController* aTestScripterController = NULL );

        /**
        * Destructor.
        */
        virtual ~CTestModuleController();

    public:     // New functions

        /**
        * StartEnumerateL starts the CTestModuleController active object.
        */
        virtual void StartEnumerateL();

        /**
        * Init the test module
        */
        virtual void InitL( TFileName& aIniFile, const TDesC& aConfigFile );

        /**
        * Add config file
        */
        virtual void AddConfigFileL( TFileName& aConfigFile );

        /**
        * Remove config file
        */
        virtual void RemoveConfigFileL( TFileName& aConfigFile );

        /**
        * Get Test Cases
        */
        virtual CFixedFlatArray<TTestInfo>* TestCasesL();

        /**
        * Free used memory for test cases
        */
        virtual void FreeTestCases();

        /**
        * Return handle to Test Server
        */
        virtual RTestServer& Server( TTestInfo& aTestInfo );

        /**
        * Return the name of Test Module.
        */
        virtual const TDesC& ModuleName( const TDesC& aModuleName );

        /**
        * Return reference to AtsLogger
        */
        virtual CAtsLogger& AtsLogger(){ return *iAtsLogger; };
        
        /**
        * Is enumeration complete?
        */
        virtual TBool EnumerationComplete();
        
        /**
        * Returns pointer to the created clone of the TestModuleController
        */		
		virtual CTestModuleController* CloneL( CTestModuleController* aTestModuleController, TBool aAfterReset, CTestScripterController* aTestScripterController );  		
		
		/**
        * Increases the value of iTestCaseCounter
        */
		virtual void CaseCreated();

		/**
        * Decreases the value of iTestCaseCounter and checks
        * can old testmodulecontroller deleted or not.
        */		
		virtual void CaseFinished();
		
    public:     // Functions from base classes

        /**
        * RunL derived from CActive handles the completed requests.
        */
        virtual void RunL();

        /**
        * DoCancel derived from CActive handles the cancellation
        */
        virtual void DoCancel();

        /**
        * RunError derived from CActive handles errors from active handler.
        */
        virtual TInt RunError( TInt aError );

        /**
        * Finds free test module controller, if not possible, creates new one.
        */
        virtual CTestModuleController* GetFreeOrCreateModuleControllerL(TTestInfo& aTestInfo, TBool aUITestingSupport);
        
        /**
        * Deletes given module controller (used only in CTestScripterController).
        */
        virtual void DeleteModuleController(CTestModuleController* aRealModuleController);

        /**
        * Removes given module controller from child list (used only in CTestScripterController).
        */
        virtual void RemoveModuleController(CTestModuleController* aRealModuleController);

        /**
        * Enumerates test module controller synchronously.
        * This is used when new test module controller is created when stif
        * is about to run test case.                
        */        
        virtual void EnumerateSynchronously(void);

    protected:  // New functions
        // None

    protected:  // Functions from base classes
        // None

    private:

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL( const TName& aName,
                            TBool aAfterReboot,
                            CTestScripterController* aTestScripterController );

        /**
        * Generate error report to xml result file and leave with notify.
        */
        void LeaveWithNotifyL( TInt aCode, const TDesC& aText );

        /**
        * Construct ATS logger. Added this function due to compiler error with
        * CW 3.0 when there are multiple TRAPDs 
        */        
        TInt ConstructASTLoggerL( TDesC& atsName, TBool & aAfterReboot);

    protected:
        /**
        * Parametric constructor.
        */
        CTestModuleController( CTestEngine* aEngine );

    public:     // Data
        // None

        // Config file array
        RPointerArray<HBufC>            iConfigFiles;
        
        // Tells is TestModuleController cloned or not in -15 (KErrServerTerminated) cases.
        TBool iTestModuleCrashDetected;

    private:    // Data
    
    protected:  // Data

        // Test Engine owning us
        CTestEngine*                    iEngine;

        // Test case array
        typedef CFixedFlatArray<TTestCaseInfo> TTestCaseArray;
        RPointerArray<TTestCaseArray>   iTestCaseArray;

        // Handle to Test Server
        RTestServer                     iServer;

        // Handle to Test Module
        RTestModule                     iModule;

        // Module name
        HBufC*                          iName;

        // Internal state
        TTestModuleStatus               iState;

        // Enumeration result
        TCaseSize                       iEnumResultPackage;

        // Count of test cases
        TInt                            iTestCaseCount;

        // Count of enumerated config files
        TInt                            iEnumerateCount;

        // Count of failed enumerated config files(This is used to decrease
        // iEnumerateCount)
        TInt                            iFailedEnumerateCount;
        // For removing faulty config (test case) file(s)
        RPointerArray<HBufC>            iFailedEnumerateConfig;

        // Is enumeration complete
        TBool                           iEnumerateComplete;

        // Temporary config file for each Enumerate call
        TPtrC                           iEnumConfigFile;

        // Pointer to CErrorPrinter
        CErrorPrinter*                  iErrorPrinter;
        // Server state handler
        CServerStateHandler*    iServerStateHandler;

        // ATS Logger
        CAtsLogger*                     iAtsLogger;

        // Pointer to CTestScripterController
        CTestScripterController*        iTestScripterController;
        // Indication is CTestScripterController's services needed. Services
        // that is implemented in CTestScripterController class.
        TInt                            iTestScripterIndicator;

        // Initialization file of Test Module
        TFileName                       iInifile;
        
        // Number of running test cases
        TInt iTestCaseCounter;
        
        // Reboot indication (moved here from ScripterControlle)
        TBool                           iAfterReboot;
        
        // List of additional module controllers "owned" by current controller
        // Used only with UITestingSupport and in case when every test case
        // has to be run in separate process
        RPointerArray<CTestModuleController> iChildrenControllers;
        
    public:     // Friend classes
        // None

    protected:  // Friend classes
        // None

    private:    // Friend classes
        // None

        friend class CTestScripterController;
        //friend class CTestModuleController;
        friend class CTestEngine; //iTestScripterController used from CTestEngine::TestModuleCrash 

    };

// DESCRIPTION
// CTestScripterController is for handling TestScripter's server session
// creations and handling operations that concern to TestScripter.

class CTestScripterController
        : public CTestModuleController
    {
    public:     // Enumerations
        // None

    private:    // Enumerations
        // None

    public:     // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CTestScripterController* NewL( CTestEngine* aEngine,
                                            const TName& aName,
                                            TBool aAfterReboot );

        /**
        * Destructor.
        */
        virtual ~CTestScripterController();

    public:     // New functions

        /**
        * StartEnumerateL starts the CTestModuleController active object.
        */
        void StartEnumerateL();

        /**
        * Init the test module
        */
        void InitL( TFileName& aIniFile, const TDesC& aConfigFile );

        /**
        * Add config file
        */
        void AddConfigFileL( TFileName& aConfigFile );

        /**
        * Remove config file
        */
        void RemoveConfigFileL( TFileName& aConfigFile );

        /**
        * Get Test Cases
        */
        CFixedFlatArray<TTestInfo>* TestCasesL();

        /**
        * Free used memory for test cases
        */
        void FreeTestCases();

        /**
        * Return handle to Test Server
        */
        RTestServer& Server( TTestInfo& aTestInfo );

        /**
        * Return the name of Test Module.
        */
        const TDesC& ModuleName( const TDesC& aModuleName );

        /**
        * Return reference to AtsLogger
        */
        CAtsLogger& AtsLogger(){ return *iTestScripter[0]->iAtsLogger; };

        /**
        * Is enumeration complete?
        */
        TBool EnumerationComplete();

        /**
        * Handles completed requests (Emulates RunL() ).
        */
        void RunLEmulator( CTestModuleController* aTestModuleController );

        /**
        * Handle errors(Emulates RunError()).
        */
        TInt RunErrorEmulator( TInt aError,
                                CTestModuleController* aTestModuleController );

        /**
        * Cancel active request(Emulates DoCancel()).
        */
        void DoCancelEmulator( CTestModuleController* aTestModuleController );

        /**
        * Get current CTestScripterController.
        */
        TInt GetCurrentIndex( CTestModuleController* aTestModuleController );

        /**
        * Finds free test module controller, if not possible, creates new one.
        */
        CTestModuleController* GetFreeOrCreateModuleControllerL(TTestInfo& aTestInfo, TBool aUITestingSupport);

        /**
        * Deletes given module controller.
        */
        void DeleteModuleController(CTestModuleController *aRealModuleController);

        /**
        * Removes given module controller from children list.
        */
        void RemoveModuleController(CTestModuleController *aRealModuleController);

    public:     // Functions from base classes

        /**
        * RunL derived from CActive handles the completed requests.
        */
        void RunL();

        /**
        * DoCancel derived from CActive handles the cancellation
        */
        void DoCancel();

        /**
        * RunError derived from CActive handles errors from active handler.
        */
        TInt RunError( TInt aError );

    protected:  // New functions
        // None

    protected:  // Functions from base classes
        // None

    private:

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL( const TName& aName, TBool aAfterReboot );

        /**
        * Parametric constructor.
        */
        CTestScripterController( CTestEngine* aEngine );

        /**
        * Create name according to TestScripter and Test case file.
        */
        HBufC* CreateTestScripterNameL( TFileName& aTestCaseFile, HBufC* aCreatedName );

    public:     // Data

        // Array for created CTestScripterController
        RPointerArray<CTestModuleController> iTestScripter;

    protected:  // Data

    private:    // Data
        // None

    public:     // Friend classes
        // None

    protected:  // Friend classes
        // None

    private:    // Friend classes
        // None

    };

/**
* Check is module TestScripter. Does parsing and returns new module name and
* error codes(Needed operations when creating server sessions to TestScripter).
*/
TInt GenerateModuleName(const TDesC& aModuleName, TDes& aNewModuleName);

#endif // TEST_MODULE_CONTROLLER_H

// End of File
