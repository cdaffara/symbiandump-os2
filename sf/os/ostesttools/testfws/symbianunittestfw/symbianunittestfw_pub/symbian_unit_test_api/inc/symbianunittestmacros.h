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

#ifndef SYMBIANUNITTESTMACROS_H
#define SYMBIANUNITTESTMACROS_H


#ifndef __KERNEL_MODE__
// INCLUDES
#include <e32std.h>
#endif


// MACROS
#ifdef SYMBIAN_UNIT_TEST

const TInt KMaxPathLength( 255 );

#ifndef __KERNEL_MODE__

    inline TInt operator==( TTrue, TInt aInt )
        {
        return aInt;
        }

    inline TInt operator==( TInt aInt, TTrue )
        {
        return aInt;
        }

    inline TInt operator!=( TTrue, TInt aInt )
        {
        return !aInt;
        }

    inline TInt operator!=( TInt aInt, TTrue )
        {
        return !aInt;
        }

    
    /** 
    * Calls the base class constructor that sets the name of unit test.
    */
    #define BASE_CONSTRUCT\
        CSymbianUnitTest::ConstructL( _L8( __PRETTY_FUNCTION__ ) );

    /**
    * Adds a new unit test case to this unit test.
    * The default setup and teardown functions will be used.
    * @param aTestPtr a function pointer to the unit test case
    */
    #ifndef __GCCE__
        #define ADD_SUT( aTestPtr )\
            AddTestCaseL(\
                _L( #aTestPtr ),\
                CSymbianUnitTest::FunctionPtr( SetupL ),\
                CSymbianUnitTest::FunctionPtr( aTestPtr ),\
                CSymbianUnitTest::FunctionPtr( Teardown ) );
            
    #else
        #define ADD_SUT( aTestPtr )\
            AddTestCaseL(\
                _L( #aTestPtr ),\
                CSymbianUnitTest::FunctionPtr( &CSymbianUnitTest::SetupL ),\
                CSymbianUnitTest::FunctionPtr( &TYPE::aTestPtr ),\
                CSymbianUnitTest::FunctionPtr( &CSymbianUnitTest::Teardown ) );
    #endif
            

    /**
    * Adds a new unit test case to this unit test.
    * The user can specify
    * @param aSetupPtr a function pointer to the setup function 
    *        that will be executed before the actual unit test case
    * @param aTestPtr a function pointer to the unit test case
    * @param aTeardownPtr a function pointer to the teardown function
    *        that will be executed after the actual unit test case
    */
    #ifndef __GCCE__
        #define ADD_SUT_WITH_SETUP_AND_TEARDOWN( aSetupPtr, aTestPtr, aTeardownPtr )\
            AddTestCaseL(\
                _L( #aTestPtr ),\
                CSymbianUnitTest::FunctionPtr( aSetupPtr ),\
                CSymbianUnitTest::FunctionPtr( aTestPtr ),\
                CSymbianUnitTest::FunctionPtr( aTeardownPtr ) );
     #else
         #define ADD_SUT_WITH_SETUP_AND_TEARDOWN( aSetupPtr, aTestPtr, aTeardownPtr )\
            AddTestCaseL(\
                _L( #aTestPtr ),\
                CSymbianUnitTest::FunctionPtr( &TYPE::aSetupPtr ),\
                CSymbianUnitTest::FunctionPtr( &TYPE::aTestPtr ),\
                CSymbianUnitTest::FunctionPtr( &TYPE::aTeardownPtr ) );
     
     #endif

    /**
    * Asserts a condition in a unit test case.
    * Leaves with a Symbian unit test framework specific error code
    * if the condition evaluates to EFalse.
    * In case of a failed assertion, the framework records 
    * the failure reason, line number and file name to the test results.
    * @param aCondition the asserted condition.
    */
    
       
    #define SUT_ASSERT( aCondition )\
        if ( aCondition ) {} else\
            {\
            TBuf8<KMaxPathLength> fullFileName( _L8( __FILE__ ) );\
            TChar ch1 = '\\';\
            TChar ch2 = '/';\
            TInt idx1 = fullFileName.LocateReverseF( ch1 );\
            TInt idx2 = fullFileName.LocateReverseF( ch2 );\
            idx1 = idx1>idx2?idx1:idx2;\
            TPtrC8 ptr = fullFileName.Mid( idx1 + 1 );\
            AssertionFailedL( _L8( #aCondition ), __LINE__, ptr );\
            }

   /**
    * Asserts a condition in a unit test case.
    * Leaves with a Symbian unit test framework specific error code
    * if the condition evaluates to EFalse.
    * In case of a failed assertion, the framework records 
    * the failure reason, line number and file name to the test results.
    * @param aCondition the asserted condition.
    * @param aDescription the error description
    */
    
       
    #define SUT_ASSERT_DESC( aCondition, aDescription )\
        if ( aCondition ) {} else\
            {\
            TBuf8<KMaxPathLength> fullFileName( _L8( __FILE__ ) );\
            TChar ch1 = '\\';\
            TChar ch2 = '/';\
            TInt idx1 = fullFileName.LocateReverseF( ch1 );\
            TInt idx2 = fullFileName.LocateReverseF( ch2 );\
            idx1 = idx1>idx2?idx1:idx2;\
            TPtrC8 ptr = fullFileName.Mid( idx1 + 1);\
            AssertionFailedL( _L8( aDescription ), __LINE__, ptr );\
            }


    /**
    * Asserts that two values are equal.
    * Leaves with a Symbian unit test framework specific error code
    * if the values are not equal.
    * In case of a failed assertion, the framework records 
    * the failure reason, line number and file name to the test results.
    * @param aExpectedValue the expected value
    * @param aActualValue the actual value
    */
    #define SUT_ASSERT_EQUALS( aExpected, aActual )\
        {\
        TBuf8<KMaxPathLength> fullFileName( _L8( __FILE__ ) );\
        TChar ch1 = '\\';\
        TChar ch2 = '/';\
        TInt idx1 = fullFileName.LocateReverseF( ch1 );\
        TInt idx2 = fullFileName.LocateReverseF( ch2 );\
        idx1 = idx1>idx2?idx1:idx2;\
        TPtrC8 ptr = fullFileName.Mid( idx1 + 1);\
        AssertEqualsL( aExpected, aActual, __LINE__, ptr );\
        }

    /**
    * Asserts that two values are equal.
    * Leaves with a Symbian unit test framework specific error code
    * if the values are not equal.
    * In case of a failed assertion, the framework records 
    * the failure reason, line number and file name to the test results.
    * @param aExpectedValue the expected value
    * @param aActualValue the actual value
    * @param aDescription the error description
    */
    #define SUT_ASSERT_EQUALS_DESC( aExpected, aActual, aDescription )\
        {\
        TBuf8<KMaxPathLength> fullFileName( _L8( __FILE__ ) );\
        TChar ch1 = '\\';\
        TChar ch2 = '/';\
        TInt idx1 = fullFileName.LocateReverseF( ch1 );\
        TInt idx2 = fullFileName.LocateReverseF( ch2 );\
        idx1 = idx1>idx2?idx1:idx2;\
        TPtrC8 ptr = fullFileName.Mid( idx1 + 1);\
        AssertEqualsL( aExpected, aActual, __LINE__, ptr, _L8(aDescription) );\
        }

    /**
    * Asserts that a statement leaves an expected value.
    * Leaves with a Symbian unit test framework specific error code
    * if the leave code is not the expected one.
    * In case of a failed assertion, the framework records 
    * the failure reason, line number and file name to the test results.
    * @param aStatement the statement
    * @param aError the expected leave code
    */
    #define SUT_ASSERT_LEAVE_WITH( aStatement, aError )\
        {\
        TInt KLine( __LINE__ );\
        TRAPD( err, aStatement )\
        if ( aError!=err )\
            {\
            TBuf8<KMaxPathLength> fullFileName( _L8( __FILE__ ) );\
            TChar ch1 = '\\';\
            TChar ch2 = '/';\
            TInt idx1 = fullFileName.LocateReverseF( ch1 );\
            TInt idx2 = fullFileName.LocateReverseF( ch2 );\
            idx1 = idx1>idx2?idx1:idx2;\
            TPtrC8 ptr = fullFileName.Mid( idx1 + 1);\
            AssertLeaveL( _L8( #aStatement ), err, aError, KLine, ptr );\
            }\
        }

    /**
    * Asserts that a statement leaves.
    * The macro itself leaves with a Symbian unit test framework 
    * specific error code if the statement leaves.
    * In case of a failed assertion, the framework records 
    * the failure reason, the line number and file name to the test results.
    *
    * Note: SUT_ASSERT_LEAVE_WITH should be used instead 
    * whenever possible, because the implementation of 
    * SYMBIAN_UT_ASSERT_LEAVE TRAPs also KErrNoMemory.
    * This means that all the memory allocations are not looped through 
    * during the memory allocation failure simulation.
    * @param aStatement the statement
    */
    #define SUT_ASSERT_LEAVE( aStatement )\
        {\
        TInt KLine( __LINE__ );\
        TRAPD( err, aStatement)\
        if ( err==KErrNone )\
            {\
            TBuf8<KMaxPathLength> fullFileName( _L8( __FILE__ ) );\
            TChar ch1 = '\\';\
            TChar ch2 = '/';\
            TInt idx1 = fullFileName.LocateReverseF( ch1 );\
            TInt idx2 = fullFileName.LocateReverseF( ch2 );\
            idx1 = idx1>idx2?idx1:idx2;\
            TPtrC8 ptr = fullFileName.Mid( idx1 + 1);\
            RecordNoLeaveFromStatementL( _L8( #aStatement ), KLine, ptr );\
            }\
        }
           
 
#else   // macro defined for kernel testing
    
    #ifndef __GCCE__
        #define ADD_SUT( aTestPtr )\
            AddTestCase(\
                _L( #aTestPtr ),\
                DSymbianUnitTest::FunctionPtr( Setup ),\
                DSymbianUnitTest::FunctionPtr(aTestPtr),\
                DSymbianUnitTest::FunctionPtr( Teardown ) );
            
    #else
        #define ADD_SUT( aTestPtr )\
            AddTestCase(\
                _L( #aTestPtr ),\
                DSymbianUnitTest::FunctionPtr( &DSymbianUnitTest::Setup ),\
                DSymbianUnitTest::FunctionPtr(&TYPE::aTestPtr),\
                DSymbianUnitTest::FunctionPtr( &DSymbianUnitTest::Teardown ) );
    #endif
    
    #ifndef __GCCE__
        #define ADD_SUT_WITH_SETUP_AND_TEARDOWN( aSetupPtr, aTestPtr, aTeardownPtr )\
            AddTestCase(\
                _L( #aTestPtr ),\
                DSymbianUnitTest::FunctionPtr( aSetupPtr ),\
                DSymbianUnitTest::FunctionPtr( aTestPtr ),\
                DSymbianUnitTest::FunctionPtr( aTeardownPtr ) );
     #else
         #define ADD_SUT_WITH_SETUP_AND_TEARDOWN( aSetupPtr, aTestPtr, aTeardownPtr )\
            AddTestCase(\
                _L( #aTestPtr ),\
                DSymbianUnitTest::FunctionPtr( &TYPE::aSetupPtr ),\
                DSymbianUnitTest::FunctionPtr( &TYPE::aTestPtr ),\
                DSymbianUnitTest::FunctionPtr( &TYPE::aTeardownPtr ) );
     
     #endif
    
    #define SUT_ASSERT( aCondition )\
        if ( aCondition ) {} else\
            {\
            return AssertionFailed( _L8( #aCondition ), __LINE__, _L8( __FILE__ ) );\
            }

    #define SUT_ASSERT_DESC( aCondition, aDescription )\
        if ( aCondition ) {} else\
            {\
            return AssertionFailed( _L8( aDescription ), __LINE__, _L8( __FILE__ ) );\
            }


    #define SUT_ASSERT_EQUALS( aExpected, aActual )\
        if ( AssertEquals(aExpected, aActual, __LINE__, _L8( __FILE__ )) == KErrNone ) {} else\
        {\
        return KErrGeneral;\
        }

    #define SUT_ASSERT_EQUALS_DESC( aExpected, aActual, aDescription )\
        if ( AssertEquals( aExpected, aActual, __LINE__, _L8( __FILE__ ), _L8(aDescription)) == KErrNone ) {} else\
        {\
        return KErrGeneral;\
        }


    
#endif // __IN_KERNEL__
    
    /**
    * Can be used to hide the friend declaration for a unit test class.
    */
    #define SYMBIAN_UNIT_TEST_CLASS( ClassName ) friend class ClassName;
            
    #define DEFINE_TEST_CLASS( aClassName )\
        typedef aClassName TYPE;
    
#else
    #define SYMBIAN_UNIT_TEST_CLASS( ClassName )
#endif //SYMBIAN_UNIT_TEST
    
#endif // SYMBIANUNITTESTMACROS_H
