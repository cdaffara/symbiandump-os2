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
* Description: STIFUnit module declaration
*
*/

#ifndef StifUnitGeneric_H
#define StifUnitGeneric_H

// INCLUDES
#include <e32svr.h>
//#include "StifTestModule.h"
#include <StifTestInterface.h>
#include <StifLogger.h>
#include <StifUnitMacros.h>

// INCLUDES FROM CASES FILE
#define STIF_UNIT_INCLUDE_SECTION
#include STIF_UNIT_TEST_CASES
#undef STIF_UNIT_INCLUDE_SECTION

// FORWARD DECLARATIONS
class STIF_UNIT_MODULE_CLASS_NAME;

// DATA TYPES
// A typedef for function that does the actual testing,
// function is a type 
// TInt STIF_UNIT_MODULE_CLASS_NAME::<NameOfFunction> ( TTestResult& aResult )
typedef TInt (STIF_UNIT_MODULE_CLASS_NAME::* TestFunction)(TTestResult&);

// CLASS DECLARATION
/**
*  An internal structure containing a test case name and
*  the pointer to function doing the test
*
*  @lib ?library
*  @since ?Series60_version
*/
class TCaseInfoInternal
    {
    public:
        const TText*    iCaseName;
        TestFunction    iMethod;
        TBool           iIsOOMTest;
        TInt            iFirstMemoryAllocation;
        TInt            iLastMemoryAllocation;
    };

// CLASS DECLARATION
/**
*  A structure containing a test case name and
*  the pointer to function doing the test
*
*  @lib ?library
*  @since ?Series60_version
*/
class TCaseInfo
    {
    public:
        TPtrC iCaseName;
        TestFunction iMethod;
        TBool           iIsOOMTest;
        TInt            iFirstMemoryAllocation;
        TInt            iLastMemoryAllocation;

    TCaseInfo( const TText* a ) : iCaseName( (TText*) a )
        {
        };

    };

// CLASS DECLARATION
/**
*  STIFUnit class (name of the class is changed by real module name).
*/
NONSHARABLE_CLASS(STIF_UNIT_MODULE_CLASS_NAME) : public CTestModuleBase
    {
    public:  //Enums
        
    public:  // Constructors and destructor


        /**
        * Two-phased constructor.
        */
        static STIF_UNIT_MODULE_CLASS_NAME* NewL()
            {
            STIF_UNIT_MODULE_CLASS_NAME* self = new (ELeave) STIF_UNIT_MODULE_CLASS_NAME;

            CleanupStack::PushL( self );
            self->ConstructL();
            CleanupStack::Pop();

            return self;
            }

        /**
        * Destructor.
        */
        virtual ~STIF_UNIT_MODULE_CLASS_NAME()
            {
            delete iLog;
            }


    public: // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    public: // Functions from base classes

        /**
        * From CTestModuleBase InitL is used to initialize the 
        *       STIFUnit module. It is called once for every instance of 
        *       TestModuleUnit after its creation.
        * @param aIniFile Initialization file for the test module (optional)
        * @param aFirstTime Flag is true when InitL is executed for first 
        *               created instance of STIFUnit module.
        * @return Symbian OS error code
        */
        TInt InitL( TFileName& /*aIniFile*/, TBool /*aFirstTime*/ )
            {
            return KErrNone;
            }

        /**
        * From CTestModuleBase GetTestCasesL is used to inquiry test cases 
        *   from STIFUnit module. 
        * @since ?Series60_version
        * @param aTestCaseFile Test case file (optional)
        * @param aTestCases  Array of TestCases returned to test framework
        * @return Symbian OS error code
        */
        TInt GetTestCasesL( const TFileName& /*aTestCaseFile*/, 
                            RPointerArray<TTestCaseInfo>& aTestCases )
            {
            TTestResult dummyResult;
            return MainTestL(EEnumerateTestCases, -1, aTestCases, dummyResult); 
            }

        /**
        * From CTestModuleBase RunTestCaseL is used to run an individual 
        *   test case. 
        * @since ?Series60_version
        * @param aCaseNumber Test case number
        * @param aTestCaseFile Test case file (optional)
        * @param aResult Test case result returned to test framework (PASS/FAIL)
        * @return Symbian OS error code (test case execution error, which is 
        *           not reported in aResult parameter as test case failure).
        */   
        TInt RunTestCaseL( const TInt aCaseNumber, 
                           const TFileName& /*aTestCaseFile*/,
                           TTestResult& aResult )
        {
        if(!iVersionLogged)
            {
            SendTestModuleVersion();
            iVersionLogged = ETrue;
            }
    
        RPointerArray<TTestCaseInfo> aTestCases; //temporary
    
        // iNumberOfTestCases should contain valid number of test cases present in test module. This info
        // is gathered in the enumeration procedure.
        // However if test case crashes, test module is loaded again and enumeration is not performed.
        // In such case counting procedure must be done.
        if(iNumberOfTestCases == 0)
            {
            TRAPD(errCount, MainTestL(ECountTestCases, aCaseNumber, aTestCases, aResult));
            if(errCount != KErrNone)
                {
        		    iLog->Log(_L("Error during counting test cases [%d]"), errCount);
        		    return errCount;
        		    }
        	  }
        
        /**
         * SetupL is responsible for inicialization of all fields (etc.) common for all testcases
         * MainTestL starts required testcase
         * TeardownL destroys all data that was created by SetupL
         */
        TInt errSetup = KErrNone;
        TInt errTestcase = KErrNone;
        
        if(aCaseNumber > iNumberOfTestCases)
            return KErrNotFound;
        	
        __UHEAP_MARK;
        
        TRAP(errSetup, MainTestL(ERunTestCase, STIF_RUN_SETUP, aTestCases, aResult); TRAP(errTestcase, MainTestL(ERunTestCase, aCaseNumber, aTestCases, aResult))\
        		MainTestL(ERunTestCase, STIF_RUN_TEARDOWN, aTestCases, aResult));
        
        __UHEAP_MARKEND;
        
        if(errTestcase != KErrNone)
            {
            aResult.SetResult(errTestcase, _L("Testcase failed"));
            }
        if(errSetup != KErrNone)
            {
            aResult.SetResult(errSetup, _L("Setup or teardown failed"));
            }
       
        return KErrNone;
        }

        /**
        * From CTestModuleBase; OOMTestQueryL is used to specify is particular
        * test case going to be executed using OOM conditions
        * @param aTestCaseFile Test case file (optional)
        * @param aCaseNumber Test case number (optional)
        * @param aFailureType OOM failure type (optional)
        * @param aFirstMemFailure The first heap memory allocation failure value (optional)
        * @param aLastMemFailure The last heap memory allocation failure value (optional)
        * @return TBool
        */
        #ifdef STIFUNIT_OOMTESTQUERYL
        virtual TBool OOMTestQueryL( const TFileName& aTestCaseFile, 
                                     const TInt aCaseNumber, 
                                     TOOMFailureType& aFailureType, 
                                     TInt& aFirstMemFailure, 
                                     TInt& aLastMemFailure);
        #else
        virtual TBool OOMTestQueryL( const TFileName& /* aTestCaseFile */, 
                                     const TInt /* aCaseNumber */, 
                                     TOOMFailureType& /* aFailureType */, 
                                     TInt& /* aFirstMemFailure */, 
                                     TInt& /* aLastMemFailure */ )
            {
            return EFalse;
            }
        #endif

        /**
        * From CTestModuleBase; OOMTestInitializeL may be used to initialize OOM
        * test environment
        * @param aTestCaseFile Test case file (optional)
        * @param aCaseNumber Test case number (optional)
        * @return None
        */
        #ifdef STIFUNIT_OOMTESTINITIALIZEL
        virtual void OOMTestInitializeL( const TFileName& aTestCaseFile, 
                                         const TInt aCaseNumber);
        #else
        virtual void OOMTestInitializeL( const TFileName& /* aTestCaseFile */, 
                                         const TInt /* aCaseNumber */ )
            {
            }
        #endif 

        /**
        * From CTestModuleBase; OOMHandleWarningL
        * @param aTestCaseFile Test case file (optional)
        * @param aCaseNumber Test case number (optional)
        * @param aFailNextValue FailNextValue for OOM test execution (optional)
        * @return None
        *
        * User may add implementation for OOM test warning handling. Usually no
        * implementation is required.           
        */
        #ifdef STIFUNIT_OOMHANDLEWARNINGL
        virtual void OOMHandleWarningL( const TFileName& aTestCaseFile,
                                        const TInt aCaseNumber, 
                                        TInt& aFailNextValue);
        #else
        virtual void OOMHandleWarningL( const TFileName& /* aTestCaseFile */,
                                        const TInt /* aCaseNumber */, 
                                        TInt& /* aFailNextValue */)
            {
            }
        #endif 

        /**
        * From CTestModuleBase; OOMTestFinalizeL may be used to finalize OOM
        * test environment
        * @param aTestCaseFile Test case file (optional)
        * @param aCaseNumber Test case number (optional)
        * @return None
        *
        */
        #ifdef STIFUNIT_OOMTESTFINALIZEL
        virtual void OOMTestFinalizeL( const TFileName& aTestCaseFile, 
                                       const TInt aCaseNumber);
        #else
        virtual void OOMTestFinalizeL( const TFileName& /* aTestCaseFile */, 
                                       const TInt /* aCaseNumber */ )
            {
            }
        #endif

        /**
         * Method used to log version of test module
         */
        void SendTestModuleVersion()
            {
            TVersion moduleVersion;
            moduleVersion.iMajor = TEST_MODULE_VERSION_MAJOR;
            moduleVersion.iMinor = TEST_MODULE_VERSION_MINOR;
            moduleVersion.iBuild = TEST_MODULE_VERSION_BUILD;
            	
            TFileName moduleName;
            moduleName = STIF_UNIT_MODULE_NAME;
            
            TBool newVersionOfMethod = ETrue;
            TestModuleIf().SendTestModuleVersion(moduleVersion, moduleName, newVersionOfMethod);
            }

    protected:  // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    protected:  // Functions from base classes

        /**
        * From ?base_class ?member_description
        */
        //?type ?member_function();

    private:

        /**
        * C++ default constructor.
        */
        STIF_UNIT_MODULE_CLASS_NAME()
            {
            }

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL()
            {
            iLog = CStifLogger::NewL( KUnitLogPath, KUnitLogFile);

            // Sample how to use logging
            iLog->Log( KLogStart );

            iVersionLogged = EFalse;
            iNumberOfTestCases = 0;
            }

        /**
        * Method containing all test cases, setup and teardown sections.
        */
        TInt MainTestL(TCallReason aRunReason, TInt aTestToRun, RPointerArray<TTestCaseInfo>& aTestCases, TTestResult& aResult)
            {
            aResult.iResult = aResult.iResult; //for avoiding compiler warning when unit test does not use any assert macro  
            if(aRunReason == ERunTestCase)
                {
                if(aTestToRun < 0)
                    {
                    iLog->Log(_L("Running setup or teardown"));
                    }
                else
                    {
                    iLog->Log(_L("Running test case #%d"), aTestToRun);
                    }
                }
            else if(aRunReason == EEnumerateTestCases)
                {
                iLog->Log(_L("Enumerating test cases."));
                }
            else if(aRunReason == ECountTestCases)
                {
                iLog->Log(_L("Counting test cases."));
                }
            else
                {
                iLog->Log(_L("Unknown run reason [%d]."), aRunReason);
                return KErrNotSupported;
                }
            
            TInt _test_case_no = -1;
            
            //test cases, setup and teardown include
            #define TEST_CASES
            #include STIF_UNIT_TEST_CASES
            #undef TEST_CASES
            
            if(aRunReason == EEnumerateTestCases)
                {
                iNumberOfTestCases = _test_case_no;
                iLog->Log(_L("Enumeration completed."));
                }
            else if(aRunReason == ECountTestCases)
                {
                iNumberOfTestCases = _test_case_no;
                iLog->Log(_L("Counting completed."));
                }
            
            // Test case was executed
            return KErrNone;
            }

    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    private:    // Data
        // Pointer to test (function) to be executed
        TestFunction iMethod;

        // Pointer to logger
        CStifLogger * iLog; 

        // Flag saying if version of test module was already sent
        TBool iVersionLogged;
        // Total number of test cases
        TInt iNumberOfTestCases;

        // ?one_line_short_description_of_data
        //?data_declaration;

        // Reserved pointer for future extension
        //TAny* iReserved;
        
		#define TEST_VAR_DECLARATIONS
        /**
         * all testmodule-global variables declarations are inserted here
         */
        #include STIF_UNIT_TEST_CASES
		#undef TEST_VAR_DECLARATIONS

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

EXPORT_C CTestModuleBase* LibEntryL()
    {
    return STIF_UNIT_MODULE_CLASS_NAME::NewL();
    }
#ifndef STIFUNIT_SETHEAPANDSTACKSIZE
EXPORT_C TInt SetRequirements( CTestModuleParam*& /*aTestModuleParam*/, 
                               TUint32& /*aParameterValid*/ )
    {
    return KErrNone;
    }

#endif
#endif      // StifUnitGeneric_H

// End of File
