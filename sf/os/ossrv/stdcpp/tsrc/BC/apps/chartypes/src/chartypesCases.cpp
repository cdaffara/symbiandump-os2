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



// [INCLUDE FILES] - do not remove
#include <e32math.h>
#include "chartypes.h"


#include <stl\char_traits.h>
#include <stl\_ctraits_fns.h>

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def

// LOCAL CONSTANTS AND MACROS
//const ?type ?constant_var = ?constant;
//#define ?macro_name ?macro_def

// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

// LOCAL FUNCTION PROTOTYPES
//?type ?function_name( ?arg_type, ?arg_type );

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ?function_name ?description.
// ?description
// Returns: ?value_1: ?description
//          ?value_n: ?description_line1
//                    ?description_line2
// -----------------------------------------------------------------------------
//
/*
?type ?function_name(
    ?arg_type arg,  // ?description
    ?arg_type arg)  // ?description
    {

    ?code  // ?comment

    // ?comment
    ?code
    }
*/

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Cchartypes::Case
// Returns a test case by number.
//
// This function contains an array of all available test cases 
// i.e pair of case name and test function. If case specified by parameter
// aCaseNumber is found from array, then that item is returned.
// 
// The reason for this rather complicated function is to specify all the
// test cases only in one place. It is not necessary to understand how
// function pointers to class member functions works when adding new test
// cases. See function body for instructions how to add new test case.
// -----------------------------------------------------------------------------
//
const TCaseInfo Cchartypes::Case ( 
    const TInt aCaseNumber ) const 
     {

    /**
    * To add new test cases, implement new test case function and add new 
    * line to KCases array specify the name of the case and the function 
    * doing the test case
    * In practice, do following
    * 1) Make copy of existing test case function and change its name
    *    and functionality. Note that the function must be added to 
    *    chartypes.cpp file and to chartypes.h 
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
        // [test cases entries] - do not remove
        
        // NOTE: When compiled to GCCE, there must be Classname::
        // declaration in front of the method name, e.g. 
        // Cchartypes::PrintTest. Otherwise the compiler
        // gives errors.
        
        ENTRY( "char_traits - Value Functions test", Cchartypes::ValueFunctionsTest ),
        ENTRY( "char_traits - Test Functions test", Cchartypes::TestFunctionsTest ),
        ENTRY( "char_traits - Conversion Functions test", Cchartypes::ConversionFunctionsTest ),
        // Example how to use OOM functionality
        //OOM_ENTRY( "Loop test with OOM", Cchartypes::LoopTest, ETrue, 2, 3),
        //OOM_FUNCENTRY( Cchartypes::PrintTest, ETrue, 1, 3 ),
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



TInt Cchartypes::ValueFunctionsTest( 
    TTestResult& aResult )
    {
 		 int failures =0;

		_LIT( KDescription, "ValueFunctionsTest passed" );
 	    _LIT( KDescriptionErr, "ValueFunctionsTest Failed" );
   
__UHEAP_MARK;

		try
		{
		 
		    std::char_traits<char>::char_type a = 'a', b = 'b', c = 'c';
		    std::char_traits<char>::assign(a,b);

			std::char_traits<char>::char_type aArray[20];
			std::char_traits<char>::assign(aArray,20,a);

			std::char_traits<char>::char_type bArray[20];
			std::char_traits<char>::char_type *bCharPtr = std::char_traits<char>::copy(aArray, bArray, 10);
			
			std::char_traits<char>::int_type eofVal = std::char_traits<char>::eof();
			
			const std::char_traits<char>::char_type * ptrCharArrayFind = std::char_traits<char>::find(aArray, 20, c);
			
			size_t lengthArray = std::char_traits<char>::length(aArray);
			
			std::char_traits<char>::char_type *bCharArrayMove = std::char_traits<char>::move(aArray, bArray, 20);
			
			std::char_traits<char>::int_type noneofVal = std::char_traits<char>::not_eof(eofVal);

		}
		catch(...)
		   {
		   	failures++;
		   	
		   }
   
	if(failures  )
	{
 		aResult.SetResult( KErrNone, KDescriptionErr );
		return KErrGeneral;
	}
    


    // Test case passed

    // Sets test case result and description(Maximum size is KStifMaxResultDes)
    aResult.SetResult( KErrNone, KDescription );

__UHEAP_MARK;
    // Case was executed
    return KErrNone;

    }




TInt Cchartypes::TestFunctionsTest( 
    TTestResult& aResult )
    {
 		 int failures =0;
	   
		_LIT( KDescription, "TestFunctionsTest passed" );
 	    _LIT( KDescriptionErr, "TestFunctionsTest Failed" );
   
__UHEAP_MARK;

		try
		{
		 
			std::char_traits<char>::char_type a = 'a', b = 'b';
			std::char_traits<char>::int_type ia = 'a', ib = 'b';

			std::char_traits<char>::char_type aArray[20];
			std::char_traits<char>::assign(aArray,20,a);

			std::char_traits<char>::char_type bArray[20];
			std::char_traits<char>::char_type *ptrChar = std::char_traits<char>::copy(aArray, bArray, 10);

			int cmpResult = std::char_traits<char>::compare(aArray,bArray,20);

			bool eqResult = std::char_traits<char>::eq(a,b);
			
			bool eqintResult = std::char_traits<char>::eq_int_type(ia,ib);

			bool ltResult = std::char_traits<char>::lt(a,b);

		}
		catch(...)
		   {
		   	failures++;
		   	
		   }
   
	if(failures  )
	{
 		aResult.SetResult( KErrNone, KDescriptionErr );
		return KErrGeneral;
	}
    


    // Test case passed

    // Sets test case result and description(Maximum size is KStifMaxResultDes)
    aResult.SetResult( KErrNone, KDescription );

__UHEAP_MARK;
    // Case was executed
    return KErrNone;

    }




TInt Cchartypes::ConversionFunctionsTest( 
    TTestResult& aResult )
    {
 		 int failures =0;
	   
		_LIT( KDescription, "ConversionFunctionsTest passed" );
 	    _LIT( KDescriptionErr, "ConversionFunctionsTest Failed" );
   
__UHEAP_MARK;

		try
		{
		 
			std::char_traits<char>::char_type a = 'a' , b;
			std::char_traits<char>::int_type ia = 'a' , ib;
			
			b = std::char_traits<char>::to_char_type(ia);
			ib = std::char_traits<char>::to_int_type(a);


		}
		catch(...)
		   {
		   	failures++;
		   	
		   }
   
	if(failures  )
	{
 		aResult.SetResult( KErrNone, KDescriptionErr );
		return KErrGeneral;
	}
    


    // Test case passed

    // Sets test case result and description(Maximum size is KStifMaxResultDes)
    aResult.SetResult( KErrNone, KDescription );

__UHEAP_MARK;
    // Case was executed
    return KErrNone;

    }





// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
?type ?classname::?member_function(
   ?arg_type arg,
   ?arg_type arg )
   {

   ?code

   }
*/

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// ?function_name implements...
// ?implementation_description.
// Returns: ?value_1: ?description
//          ?value_n: ?description
//                    ?description
// -----------------------------------------------------------------------------
//
/*
?type  ?function_name(
    ?arg_type arg,  // ?description
    ?arg_type arg )  // ?description
    {

    ?code

    }
*/
//  [End of File] - do not remove
