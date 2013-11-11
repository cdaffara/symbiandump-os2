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
* Description: This module contains the implementation of 
* CTestModuleDemo class member functions that does the actual 
* tests.
*
*/

// INCLUDE FILES
#include <e32math.h>
#include "DemoModule.h"

// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES  
// None

// CONSTANTS
// None

// MACROS
// None

// LOCAL CONSTANTS AND MACROS
// None

// MODULE DATA STRUCTURES
// None

// LOCAL FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// ==================== LOCAL FUNCTIONS =======================================

// ============================ MEMBER FUNCTIONS ===============================

/*
-------------------------------------------------------------------------------

    Class: CDemoModule

    Method: Case

    Description: Returns a test case by number.

    This function contains an array of all available test cases 
    i.e pair of case name and test function. If case specified by parameter
    aCaseNumber is found from array, then that item is returned.

    The reason for this rather complicated function is to specify all the
    test cases only in one place. It is not necessary to understand how
    function pointers to class member functions works when adding new test
    cases. See function body for instructions how to add new test case.
    
    Parameters:    const TInt aCaseNumber :in:      Test case number

    Return Values: const TCaseInfo Struct containing case name & function

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/ 
const TCaseInfo CDemoModule::Case ( 
    const TInt aCaseNumber ) const 
     {

    /*
    * To add new test cases, implement new test case function and add new 
    * line to KCases array specify the name of the case and the function 
    * doing the test case
    * In practice, do following
    *
    * 1) Make copy of existing test case function and change its name
    *    and functionality. Note that the function must be added to 
    *    OOMHard.cpp file and to OOMHard.h 
    *    header file.
    *
    * 2) Add entry to following KCases array either by using:
    *
    * 2.1: FUNCENTRY or ENTRY macro
    * ENTRY macro takes two parameters: test case name and test case 
    * function name.
    *
    * FUNCENTRY macro takes only test case function name as a parameter and
    * uses that as a test case name and test case function name.
    *
    * Or
    *
    * 2.2: OOM_FUNCENTRY or OOM_ENTRY macro. Note that these macros are used
    * only with OOM (Out-Of-Memory) testing!
    *
    * OOM_ENTRY macro takes five parameters: test case name, test case 
    * function name, TBool which specifies is method supposed to be run using
    * OOM conditions, TInt value for first heap memory allocation failure and 
    * TInt value for last heap memory allocation failure.
    * 
    * OOM_FUNCENTRY macro takes test case function name as a parameter and uses
    * that as a test case name, TBool which specifies is method supposed to be
    * run using OOM conditions, TInt value for first heap memory allocation 
    * failure and TInt value for last heap memory allocation failure. 
    */ 

    static TCaseInfoInternal const KCases[] =
        {
        // To add new test cases, add new items to this array
        
        // NOTE: When compiled to GCCE, there must be Classname::
        // declaration in front of the method name, e.g. 
        // CDemoModule::PrintTest. Otherwise the compiler
        // gives errors.
                
        ENTRY( "Loop test", CDemoModule::LoopTest ),
        ENTRY( "Simple math test", CDemoModule::SimpleMathTest ),
        ENTRY( "Math test", CDemoModule::MathTest ),
        ENTRY( "Print test", CDemoModule::PrintTest ),
        // Example how to use OOM functionality
        OOM_ENTRY( "Heap memory allocation with OOM (aborts)", CDemoModule::HeapMemoryAllocation, ETrue, 1, 2 ),
        ENTRY( "Heap memory allocation (passes)", CDemoModule::HeapMemoryAllocation )
        };

    // Verify that case number is valid
    if( (TUint) aCaseNumber >= sizeof( KCases ) / 
                               sizeof( TCaseInfoInternal ) )
        {

        // Invalid case, construct empty object
        TCaseInfo null( (const TText*) L"" );
        null.iMethod = NULL;
        null.iIsOOMTest = EFalse;
        null.iFirstMemoryAllocation = 0;
        null.iLastMemoryAllocation = 0;
        return null;

        } 

    // Construct TCaseInfo object and return it
    TCaseInfo tmp ( KCases[ aCaseNumber ].iCaseName );
    tmp.iMethod = KCases[ aCaseNumber ].iMethod;
    tmp.iIsOOMTest = KCases[ aCaseNumber ].iIsOOMTest;
    tmp.iFirstMemoryAllocation = KCases[ aCaseNumber ].iFirstMemoryAllocation;
    tmp.iLastMemoryAllocation = KCases[ aCaseNumber ].iLastMemoryAllocation;
    return tmp;

    }

/*
-------------------------------------------------------------------------------

    Class: CDemoModule

    Method: SimpleMathTest

    Description: Simple Math testing.

    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CDemoModule::SimpleMathTest( TTestResult& aResult )
    {
    /* Test with mathematic calculations and printing */
    _LIT( KDefinion , "RunTestCase");
    _LIT( KData , "Simple Math calculations");
    TestModuleIf().Printf( 0, KDefinion, KData );

    // Random number.
    TReal num = Math::Random(); 
    // Intermediate result
    TReal sqrt = 0;
    // Result
    TReal result = 0;

    // Take square root of the number
    TInt ret = Math::Sqrt( sqrt, num );
    if( ret != KErrNone )
        {
        // Return error if Sqrt failed
        _LIT( KResult ,"Math::Sqrt failed");
        aResult.iResultDes.Copy( KResult );
        aResult.iResult = KErrGeneral;

        // Case was executed but failed
        return KErrNone;
        }

    // Take power of two from the previous result
    ret = Math::Pow( result, sqrt, 2 );
    if( ret != KErrNone )
        {
        // Return error if Pow failed
        _LIT( KResult ,"Math::Pow failed");
        aResult.iResultDes.Copy( KResult );
        aResult.iResult = KErrGeneral;
        
        // Case was executed but failed
        return KErrNone;
        }

    // Compare final result to the original value, 
    // rounded values should usually be same.
    TInt32 a1;
    TInt32 a2;
    TInt r = Math::Int(a1, result);

    if (r != KErrNone )
        {
        _LIT( KResult ,"1st conversion failed");
        aResult.iResultDes.Copy( KResult );
        aResult.iResult = r;
        return KErrNone;
        }

    r = Math::Int(a2, num);
    if (r != KErrNone )
        {
        _LIT( KResult ,"2nd conversion failed");
        aResult.iResultDes.Copy( KResult );
        aResult.iResult = r;
        return KErrNone;
        }

    if(  a1 != a2 )
        {
        _LIT( KResult ,"Calculation doesn't match");
        // Return error if comparison failed
        aResult.iResultDes.Copy( KResult );
        aResult.iResult = KErrGeneral;
        // Case was executed but failed
        return KErrNone;
        }


    // Test case passed

    // Sets test case result and description(Maximum size is KStifMaxResultDes)
    _LIT( KDescription , "Test case passed");
    aResult.SetResult( KErrNone, KDescription );
    //aResult.iResult = KErrNone;
    //aResult.iResultDes = KDescription;

    // Case was executed
    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CDemoModule

    Method: MathTest

    Description: Print loop test. Actually does not test anything, just
    demonstrate how to print. Test is always pased.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CDemoModule::LoopTest( TTestResult& aResult )
    {

    /* Simple print and wait loop */
    _LIT( KDefinion ,"State");
    _LIT( KData ,"Looping");
    TestModuleIf().Printf( 0, KDefinion, KData );
    for( TInt i=0; i<10; i++)
        {
        _LIT( KRunning ,"Running");
        _LIT( KLoop ,"%d");
        TestModuleIf().Printf( 0, KRunning, KLoop, i);
        User::After( 1000000 );
        }
    _LIT( KData2 ,"Finished");
    TestModuleIf().Printf( 0, KDefinion, KData2 );

    // Test case passed

    // Sets test case result and description(Maximum size is KStifMaxResultDes)
    _LIT( KDescription , "Test case passed");
    aResult.SetResult( KErrNone, KDescription );
    //aResult.iResult = KErrNone;
    //aResult.iResultDes = KDescription;

    // Case was executed
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CDemoModule

    Method: MathTest

    Description: Math testing.
  
    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CDemoModule::MathTest( TTestResult& aResult )
    {
    /* Test with mathematic calculations and printing */
    _LIT( KDefinion ,"RunTestCase");
    _LIT( KData ,"Math calculations");
    TestModuleIf().Printf( 0, KDefinion, KData );
    // Result
    TRealX res;
    // Random numbers
    TInt rand[6];
    TRealX coef[5];
    TInt i;
    TInt j;
    for( i=0; i<10; i++ )
        {
        for(j=0; j<6; j++)
            rand[j] = Math::Random(); 
        for(j=0; j<5; j++)
            coef[j] = rand[j];
        Math::PolyX(res, rand[5], 5, coef );

        _LIT( KResult ,"Result %d");
        TestModuleIf().Printf( 0, KDefinion, KResult, ( TInt )res);
        }

    // Test case passed

    // Sets test case result and description(Maximum size is KStifMaxResultDes)
    _LIT( KDescription , "Test case passed");
    aResult.SetResult( KErrNone, KDescription );
    //aResult.iResult = KErrNone;
    //aResult.iResultDes = KDescription;

    // Case was executed
    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CDemoModule

    Method: PrintTest

    Description: Printing testing.

    Parameters:  TTestResult& aErrorDescription: out:   
                    Test result and on error case a short description of error

    Return Values: TInt: Always KErrNone to indicate that test was valid

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CDemoModule::PrintTest( TTestResult& aResult )
    {
    /* Tests printing with high intencity and different priority */ 
    _LIT( KDefinion ,"CTestModuleDemo::RunTestCase");
    _LIT( KData ,"Heavy print looping");
    TestModuleIf().Printf( 0, KDefinion, KData );
    TInt j;
    for( TInt i=0; i<10; i++)
        {

        for(j=0; j<2; j++)
            {
            _LIT( KTmp ,"RunTestCase");
            TBuf<15> tmp;
            tmp.Copy( KTmp );
            tmp.AppendNum( j );
            _LIT( KNewData ,"p");
            TestModuleIf().Printf(3, tmp, KNewData );
            }

        for(j=0; j<500; j++)
            {
             _LIT( KNewDefinion ,"RunTestCase");
             _LIT( KNewData ,"%c");
            TestModuleIf().Printf(1+(Math::Random() % 12),
                KNewDefinion, 
                KNewData, 'a'+(j%('z'-'a')) );
            }

        _LIT( KNewDefinion ,"RunTestCase");
        _LIT( KNewData ,"Running %d");
        TestModuleIf().Printf( 2, KNewDefinion,KNewData, i);
        User::After( 1000000 );
        }

    // Test case passed

    // Sets test case result and description(Maximum size is KStifMaxResultDes)
    _LIT( KDescription , "Test case passed");
    aResult.SetResult( KErrNone, KDescription );
    //aResult.iResult = KErrNone;
    //aResult.iResultDes = KDescription;

    // Case was executed
    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CDemoModule

    Method: HeapMemoryAllocation

    Description: Allocates heap descriptor.
  
    Parameters: TTestResult& aError

    Return Values: TInt

    Errors/Exceptions: Leaves with KErrNoMemory if heap allocation fails

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
TInt CDemoModule::HeapMemoryAllocation( TTestResult& aResult )
    {
    _LIT( KLogInfo , "CDemoModule::HeapMemoryAllocation" );
    iLog->Log( KLogInfo );

    // Allocate heap descriptor
    HBufC * buffer = HBufC::New( 10 );
    if( buffer == NULL )
        {
        // Allocation failed
        _LIT( KNewLogInfo , "buffer was NULL");
        iLog->Log( KNewLogInfo );
        User::Leave( KErrNoMemory );
        }
    else
        {
        // Descriptor creation was ok
        _LIT( KNewLogInfo ,"buffer was allocated properly" );
        iLog->Log( KNewLogInfo );
        }

    delete buffer;

    _LIT( KDescription , "Test case passed");
    aResult.SetResult( KErrNone, KDescription );
    return KErrNone;
    }

// ================= OTHER EXPORTED FUNCTIONS =================================

// End of File
