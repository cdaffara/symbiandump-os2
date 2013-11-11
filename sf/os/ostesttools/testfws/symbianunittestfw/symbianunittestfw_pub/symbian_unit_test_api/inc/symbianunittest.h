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
* Description:  
*
*/
#ifndef SYMBIANUNITTEST_H
#define SYMBIANUNITTEST_H

// INCLUDES
#include <symbianunittestinterface.h>
#include <symbianunittestmacros.h>
#include <e32base.h>

// FORWARD DECLARATIONS
class CSymbianUnitTestResult;

// CLASS DECLARATION
/**
* The base class for unit test classes.
* The classes inheriting from this class should call 
* CSymbianUnitTest::ConstructL from their ConstructL.
* Although the functions implemented in this class 
* for unit test assertions and adding test cases 
* can be called directly from the subclasses,
* the macros defined in symbianunittestmacros.h should be used instead.
*
* @lib symbianunittestfw.lib
*/
class CSymbianUnitTest : public CBase, public MSymbianUnitTestInterface
    {
    public: // Typedefs
    
        typedef void ( CSymbianUnitTest::*FunctionPtr )();      
    
    public: // Destructor

        /**
        * Destructor
        */        
        IMPORT_C ~CSymbianUnitTest();
    
    public: // From MSymbianUnitTestInterface

        /**
        * Function called by the framework to execute 
        * the unit tests in the subclass.
        * There is no need to call this function directly 
        * from the unit test classes.
        * 
        * @param aObserver an observer for the test progress 
        * @param aResult the test result
        * @param aFailureSimulation the type of failure simulation to be used
	* @param aTestCaseNames the test cases to run
	* @param aTimeout the time out value for test execution
        */        
        IMPORT_C void ExecuteL( 
            MSymbianUnitTestObserver& aObserver,
            CSymbianUnitTestResult& aResult,
            MSymbianUnitTestInterface::TFailureSimulation aFailureSimulation,
	    const CDesCArray& aTestCaseNamesi,
	    TInt aTimeout );

        /**
        * Returns the number of unit test cases contained in this object. 
        * @return The number of unit test cases in this object
        */        
        IMPORT_C TInt TestCaseCount();

        /**
        * @return The name of this unit test class
        */        
        IMPORT_C const TDesC& Name() const;

        /**
         * @return the array of test case names
         */
        IMPORT_C void TestCaseNamesL(CDesCArray& aTestCaseNames);
        
    protected: // Constructors

        /**
        * Constructor
        */        
        IMPORT_C CSymbianUnitTest();
        
        /**
        * This function should be called 
        * from the ConstructL of the subclass
        * 
        * @param aName the name of the unit test class
        */        
        IMPORT_C void ConstructL( const TDesC8& aName );
    
    public: // New functions    

        /**
        * This function should be overidden in the actual unit test classes.
        * It is the default setup function that gets executed 
        * by the framework before each unit test case.
        */        
        IMPORT_C virtual void SetupL();
        
        /**
        * This function should be overidden in the actual unit test classes.
        * It is the default teardown function that gets executed 
        * by the framework after each unit test case.
        */        
        IMPORT_C virtual void Teardown();

    protected:    // New functions    
        /**
        * Checks whteher the test case is being run using memory 
        * allocation failure simulation.
        * @return ETrue if the test case is being run using memory 
        *         allocation failure simulation, EFalse if not
        */        
        IMPORT_C TBool IsMemoryAllocationFailureSimulationUsed() const;
        
        /**
        * Adds a new unit test case to this unit test.
        * Use macro ADD_SYMBIAN_UT in symbianunittestmacros.h 
        * instead to avoid the need to pass 
        * the name of the unit test case as a parameter.
        * 
        * @param aName the name of the unit test case
        * @param aSetupFunction a function pointer to the setup function 
        *        that will be executed before the actual unit test case
        * @param aTestFunction a function pointer to the unit test case
        * @param aTeardownFunction a function pointer to the teardown function
        *        that will be executed after the actual unit test case
        */        
        IMPORT_C void AddTestCaseL( 
            const TDesC& aName,
            CSymbianUnitTest::FunctionPtr aSetupFunction,
            CSymbianUnitTest::FunctionPtr aTestFunction,
            CSymbianUnitTest::FunctionPtr aTeardownFunction );

        /**
        * Asserts that two TInt values are equal.
        * Leaves with a Symbian unit test framework specific error code
        * if the values are not equal.
        * In case of a failed assertion, the framework records 
        * the failure reason, line number and file name to the test results.
        * Use macro SYMBIAN_UT_ASSERT_EQUALS in symbianunittestmacros.h 
        * instead to avoid the need to pass the other parameters 
        * than the actual asserted values.  
        * 
        * @param aExpectedValue the expected value
        * @param aActualValue the actual value
        * @param aLineNumber the line number of the assertion
        * @param aFileName the name of the file where the assertion is located
        */        
        IMPORT_C void AssertEqualsL( 
            TInt aExpectedValue,
            TInt aActualValue,
            TInt aLineNumber,
            const TDesC8& aFileName );        

        /**
        * Asserts that two TDesC8 values are equal.
        * Leaves with a Symbian unit test framework specific error code
        * if the values are not equal.
        * In case of a failed assertion, the framework records 
        * the failure reason, line number and file name to the test results.
        * Use macro SYMBIAN_UT_ASSERT_EQUALS in symbianunittestmacros.h 
        * instead to avoid the need to pass the other parameters 
        * than the actual asserted values.  
        * 
        * @param aExpectedValue the expected value
        * @param aActualValue the actual value
        * @param aLineNumber the line number of the assertion
        * @param aFileName the name of the file where the assertion is located
        */ 
        IMPORT_C void AssertEqualsL(
            const TDesC8& aExpectedValue,
            const TDesC8& aActualValue, 
            TInt aLineNumber, 
            const TDesC8& aFileName );

        /**
        * Asserts that two TDesC16 values are equal.
        * Leaves with a Symbian unit test framework specific error code
        * if the values are not equal.
        * In case of a failed assertion, the framework records 
        * the failure reason, line number and file name to the test results.
        * Use macro SYMBIAN_UT_ASSERT_EQUALS in symbianunittestmacros.h 
        * instead to avoid the need to pass the other parameters 
        * than the actual asserted values.  
        * 
        * @param aExpectedValue the expected value
        * @param aActualValue the actual value
        * @param aLineNumber the line number of the assertion
        * @param aFileName the name of the file where the assertion is located
        */        
        IMPORT_C void AssertEqualsL(
            const TDesC16& aExpectedValue,
            const TDesC16& aActualValue,
            TInt aLineNumber,
            const TDesC8& aFileName );       

        /**
        * Asserts that a statement leaves an expected value.
        * Leaves with a Symbian unit test framework specific error code
        * if the leave code is not the expected one.
        * In case of a failed assertion, the framework records 
        * the failure reason, line number and file name to the test results.
        * Use macro SYMBIAN_UT_ASSERT_LEAVE in symbianunittestmacros.h 
        * instead to avoid the need to pass the other parameters 
        * than the actual statement that is checked for the leave.  
        * 
        * @param aStatement textual presentation of the statement
        * @param aActualLeaveCode the actual leave code from the statement
        * @param aExpectedLeaveCode the expected leave code
        * @param aLineNumber the line number of the assertion
        * @param aFileName the name of the file where the assertion is located
        */        
        IMPORT_C void AssertLeaveL(
            const TDesC8& aStatement,
            TInt aActualLeaveCode,
            TInt aExpectedLeaveCode,
            TInt aLineNumber,
            const TDesC8& aFileName );        
       
        /**
        * Records a failed assertion to the test results and
        * leaves with a Symbian unit test framework specific error code
        * to quit the execution of the test function.
        * Use macro SYMBIAN_UT_ASSERT in symbianunittestmacros.h 
        * to perform the actual assertion.  
        * 
        * @param aFailureMessage the failed assertion as text
        * @param aLineNumber the line number of the assertion
        * @param aFileName the name of the file where the assertion is located
        */        
        IMPORT_C void AssertionFailedL( 
            const TDesC8& aFailureMessage,
            TInt aLineNumber,
            const TDesC8& aFileName );        

        /**
        * Records a non-leaving statement to the test results and
        * leaves with a Symbian unit test framework specific error code
        * to quit the execution of the test function.
        * This function is called from macro SYMBIAN_UT_ASSERT_LEAVE 
        * in symbianunittestmacros.h to record the result of a failed assertion.
        * There is usually no need to call this function directly.
        * 
        * @param aStatement textual presentation of the statement
        * @param aLineNumber the line number of the assertion
        * @param aFileName the name of the file where the assertion is located
        */        
        IMPORT_C void RecordNoLeaveFromStatementL(
            const TDesC8& aStatement,
            TInt aLineNumber,
            const TDesC8& aFileName ); 



	 /**
        * Asserts that two TInt values are equal.
        * Leaves with a Symbian unit test framework specific error code
        * if the values are not equal.
        * In case of a failed assertion, the framework records 
        * the failure reason, line number and file name to the test results.
        * Use macro SYMBIAN_UT_ASSERT_EQUALS in symbianunittestmacros.h 
        * instead to avoid the need to pass the other parameters 
        * than the actual asserted values.  
        * 
        * @param aExpectedValue the expected value
        * @param aActualValue the actual value
        * @param aLineNumber the line number of the assertion
        * @param aFileName the name of the file where the assertion is located
        * @param aFailureMessage the error message
        */        
        IMPORT_C void AssertEqualsL( 
            TInt aExpectedValue,
            TInt aActualValue,
            TInt aLineNumber,
            const TDesC8& aFileName,
	    const TDesC8& aFailureMessage );        

        /**
        * Asserts that two TDesC8 values are equal.
        * Leaves with a Symbian unit test framework specific error code
        * if the values are not equal.
        * In case of a failed assertion, the framework records 
        * the failure reason, line number and file name to the test results.
        * Use macro SYMBIAN_UT_ASSERT_EQUALS in symbianunittestmacros.h 
        * instead to avoid the need to pass the other parameters 
        * than the actual asserted values.  
        * 
        * @param aExpectedValue the expected value
        * @param aActualValue the actual value
        * @param aLineNumber the line number of the assertion
        * @param aFileName the name of the file where the assertion is located
        * @param aFailureMessage the error message
        */ 
        IMPORT_C void AssertEqualsL(
            const TDesC8& aExpectedValue,
            const TDesC8& aActualValue, 
            TInt aLineNumber, 
            const TDesC8& aFileName,
	    const TDesC8& aFailureMessage );

        /**
        * Asserts that two TDesC16 values are equal.
        * Leaves with a Symbian unit test framework specific error code
        * if the values are not equal.
        * In case of a failed assertion, the framework records 
        * the failure reason, line number and file name to the test results.
        * Use macro SYMBIAN_UT_ASSERT_EQUALS in symbianunittestmacros.h 
        * instead to avoid the need to pass the other parameters 
        * than the actual asserted values.  
        * 
        * @param aExpectedValue the expected value
        * @param aActualValue the actual value
        * @param aLineNumber the line number of the assertion
        * @param aFileName the name of the file where the assertion is located
        * @param aFailureMessage the error message
        */        
        IMPORT_C void AssertEqualsL(
            const TDesC16& aExpectedValue,
            const TDesC16& aActualValue,
            TInt aLineNumber,
            const TDesC8& aFileName,
	    const TDesC8& aFailureMessage );      
        
    private: // Internal class for a unit test function's data
        
        class CSymbianUnitTestCase : public CBase
            {            
            public: // Constructors and destructor
                
                static CSymbianUnitTestCase* NewL( 
                    const TDesC& aName,
                    CSymbianUnitTest::FunctionPtr aSetupFunction,
                    CSymbianUnitTest::FunctionPtr aTestFunction,
                    CSymbianUnitTest::FunctionPtr aTeardownFunction );
                
                ~CSymbianUnitTestCase();
            
            public: // New functions
                
                const TDesC& Name() const;
                
            public: // Data
                
                CSymbianUnitTest::FunctionPtr iSetupFunction;
                CSymbianUnitTest::FunctionPtr iTestFunction;
                CSymbianUnitTest::FunctionPtr iTeardownFunction;
            
            private: // Constructors
                
                CSymbianUnitTestCase( 
                    CSymbianUnitTest::FunctionPtr aSetupFunction,
                    CSymbianUnitTest::FunctionPtr aTestFunction,
                    CSymbianUnitTest::FunctionPtr aTeardownFunction );
                void ConstructL( const TDesC& aName );
                
            private: // Data
                
                HBufC* iName;
            };
        
        private: // New functions
        
            HBufC8* NotEqualsMessageLC( const TDesC16& aExpected,
                                        const TDesC16& aActual );
        
            void ExecuteTestCaseInThreadL( 
                CSymbianUnitTestCase& aTestCase,
                CSymbianUnitTestResult& aResult,
	        TInt aTimeout);
            
            static TInt TestThreadEntryFunction( TAny* aPtr );
            
            void ExecuteTestCaseL();
            
            void DoExecuteTestCaseL( TInt& aLeaveCodeFromTest );
            
            void StartAllocFailureSimulation();
            
            void StopAllocFailureSimulation();

	    static TInt TimerThreadEntryFunction( TAny* aPtr);
        
    private: // Data
    
        HBufC* iName;
        RHeap::TAllocFail iAllocFailureType;
        TUint iAllocFailureRate;
        TInt iLeakedMemory;
        TInt iLeakedResource;
        TInt iLeakedRequest;
        TInt iHeapCellsReservedByAssertFailure;
        RPointerArray< CSymbianUnitTestCase > iTestCases;
        CSymbianUnitTestResult* iTestResult; // Not owned
        CSymbianUnitTestCase* iCurrentTestCase; // Not owned

    private: // Test
        
        SYMBIAN_UNIT_TEST_CLASS( UT_CSymbianUnitTest )
    };

#endif // SYMBIANUNITTEST_H
