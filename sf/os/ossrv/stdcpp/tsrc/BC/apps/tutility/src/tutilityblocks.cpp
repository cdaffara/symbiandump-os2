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



// INCLUDE FILES
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include <memory>

#include "tutility.h"


using namespace std;

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
// Ctutility::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void Ctutility::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// Ctutility::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt Ctutility::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "Uninitialized_Copy", Ctutility::Uninitialized_Copy ),
        ENTRY( "Uninitialized_Copy_n", Ctutility::Uninitialized_Copy_n ),
        ENTRY( "Uninitialized_Fill", Ctutility::Uninitialized_Fill ),
        ENTRY( "Uninitialized_Fill_n", Ctutility::Uninitialized_Fill_n ),
        
        //ADD NEW ENTRY HERE

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// Ctutility::Uninitialized_Copy
// Uninitialized_Copy test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

TInt Ctutility::Uninitialized_Copy( CStifItemParser& aItem )
    {

	__UHEAP_MARK;
	 int Array1[] = { 10, 20, 30, 40 };
     const int N = sizeof( Array1 ) / sizeof( int );

     int i, testfail = 0;

     Integer* ArrayPtr = ( Integer* )::operator new( N * sizeof( int ) );
     Integer* LArrayPtr = uninitialized_copy(Array1, Array1 + N, ArrayPtr);  
	 
     if ( ( &Array1[0] + N ) == ( void* )LArrayPtr )
     {
      testfail++;
     }
     
     if ( ( void* )LArrayPtr != ( void* )( ArrayPtr + N ) )
     {
      testfail++;
     }

     delete ArrayPtr ;
     
     __UHEAP_MARKEND;

     if( testfail == 0)
       return KErrNone;
     else
       return KErrGeneral;  

    }

// -----------------------------------------------------------------------------
// Ctutility::Uninitialized_Copy_n
// Uninitialized_Copy_n test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

TInt Ctutility::Uninitialized_Copy_n( CStifItemParser& aItem )
    {

	__UHEAP_MARK;
	 int Array1[] = { 10, 20, 30, 40 };
     const int N = sizeof( Array1 ) / sizeof( int );

     int i;

     Integer* ArrayPtr = ( Integer* )::operator new( N * sizeof( int ) );
     std::pair<int*,Integer*> LArrayPtr = uninitialized_copy_n(Array1, N, ArrayPtr);  
	 
     delete ArrayPtr ;
     __UHEAP_MARKEND;

     return KErrNone;  

    }


// -----------------------------------------------------------------------------
// Ctutility::Uninitialized_Fill
// Uninitialized_Fill test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

TInt Ctutility::Uninitialized_Fill( CStifItemParser& aItem )
    {

	__UHEAP_MARK;
     int testfail = 0 , p = 0;
     const int N = 10;
     Integer val ( 25 );
     Integer* Array1 = ( Integer* )::operator new( N * sizeof( int ) );
     uninitialized_fill( Array1, Array1 + N, val );

     for ( int i = 0 ; i < N; i++ )
      {
         p = Array1[ i ].get( );    
         if(  p != 25 )
           testfail++; 
      }
 
     delete Array1 ;
     __UHEAP_MARKEND;

     if( testfail == 0)
       return KErrNone;
     else
       return KErrGeneral;  
    }


// -----------------------------------------------------------------------------
// Ctutility::Uninitialized_Fill_n
// Uninitialized_Fill_n test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

TInt Ctutility::Uninitialized_Fill_n( CStifItemParser& aItem )
    {

	__UHEAP_MARK;
     int testfail = 0 , p = 0;
     const int N = 10;
     Integer val ( 60 );
     Integer* Array1 = ( Integer* )::operator new( N * sizeof( int ) );
     uninitialized_fill_n( Array1, N, val );

     for ( int i = 0 ; i < N; i++ )
      {
         p = Array1[ i ].get( );    
         if(  p != 60 )
           testfail++; 
      }
 
     delete Array1 ;
     __UHEAP_MARKEND;

     if( testfail == 0)
       return KErrNone;
     else
       return KErrGeneral;  
    }


// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  End of File
