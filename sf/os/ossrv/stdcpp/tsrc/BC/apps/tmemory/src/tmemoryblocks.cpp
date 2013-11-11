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
#include <new>
//#include <memory>
#include "tmemory.h"

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
// Ctmemory::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void Ctmemory::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// Ctmemory::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt Ctmemory::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "New1", Ctmemory::New1 ),
        ENTRY( "New2", Ctmemory::New2 ),
        ENTRY( "New3", Ctmemory::New3 ),
        ENTRY( "New4", Ctmemory::New4 ),
        //ADD NEW ENTRY HERE

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// Ctmemory::New1
// New1 test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

TInt Ctmemory::New1( CStifItemParser& aItem )
    {

		__UHEAP_MARK;
		int testfail = 0;
		Integer* p;
		try 
 		{
  			p = (Integer *)::operator new (sizeof(Integer));
 		}
 		catch(const std::bad_alloc& ex) 
 		{
  			::operator delete(p);
  			throw ex;
  			return KErrGeneral;
 		}
 		::operator delete(p);
 		__UHEAP_MARKEND;
 		return KErrNone;  

    }
    
// -----------------------------------------------------------------------------
// Ctmemory::New2
// New2 test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//   
 
TInt Ctmemory::New2( CStifItemParser& aItem )
    {

	__UHEAP_MARK;
	int testfail = 0;
	Integer* p = NULL;
	std::nothrow_t nt;
	
	p = (Integer *)::operator new(sizeof(Integer), nt);

	if( p == NULL )
	 testfail++;
	 
	::operator delete(p , nt) ;	
	p = NULL ;
	 
	__UHEAP_MARKEND; 
	
	if(testfail == 0) 
       return KErrNone;
    else
       return KErrGeneral;   
       
    }    


// -----------------------------------------------------------------------------
// Ctmemory::New3
// New3 test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//   
 
TInt Ctmemory::New3( CStifItemParser& aItem )
    {

		__UHEAP_MARK;
		int testfail = 0;
		Integer* p;
		try 
 		{
  			p = (Integer *)::operator new[](sizeof(Integer));
 		}
 		catch(const std::bad_alloc& ex) 
 		{
  			::operator delete(p);
  			throw ex;
  			return KErrGeneral;
 		}
 		::operator delete(p);
 		__UHEAP_MARKEND;
 		return KErrNone;  
       
    }    

// -----------------------------------------------------------------------------
// Ctmemory::New4
// New4 test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//   
 
TInt Ctmemory::New4( CStifItemParser& aItem )
    {

	__UHEAP_MARK;
	int testfail = 0;
	Integer* p = NULL;
	std::nothrow_t nt;
	
	p = (Integer *)::operator new[](sizeof(Integer) , nt);

	if( p == NULL )
	 testfail++;
	 
	::operator delete[](p , nt) ;	
	p = NULL ;
	 
	__UHEAP_MARKEND; 
	
	if(testfail == 0) 
       return KErrNone;
    else
       return KErrGeneral;   
       
    }    


// -----------------------------------------------------------------------------
// Ctmemory::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*

//	STD nothrow_t nt;
TInt Ctmemory::?member_function(
   CItemParser& aItem )
   {

   ?code

   }
*/

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  End of File
