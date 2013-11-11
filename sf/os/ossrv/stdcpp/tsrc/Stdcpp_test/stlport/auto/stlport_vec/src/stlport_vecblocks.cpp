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
* Description:   ?Description
*
*/




// INCLUDE FILES
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "stlport_vec.h"

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
// Cstlport_vec::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void Cstlport_vec::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// Cstlport_vec::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt Cstlport_vec::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "vec1_test", Cstlport_vec::vec1_testL ),
        ENTRY( "vec2_test", Cstlport_vec::vec2_testL ),
        ENTRY( "vec3_test", Cstlport_vec::vec3_testL ),
        ENTRY( "vec4_test", Cstlport_vec::vec4_testL ),
        ENTRY( "vec5_test", Cstlport_vec::vec5_testL ),
        ENTRY( "vec6_test", Cstlport_vec::vec6_testL ),
        ENTRY( "vec7_test", Cstlport_vec::vec7_testL ),
        ENTRY( "vec8_test", Cstlport_vec::vec8_testL ),
        

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// Cstlport_vec::vec1_testL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cstlport_vec::vec1_testL( CStifItemParser& /*aItem*/ )
    {

      int ret=0;
    ret = vec1_test(0, NULL);
    
     if(ret==0)
		return KErrNone;
	else	
		return KErrGeneral;

    }
 // -----------------------------------------------------------------------------
// Cstlport_vec::vec2_testL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cstlport_vec::vec2_testL( CStifItemParser& /*aItem*/ )
    {

      int ret=0;
    ret = vec2_test(0, NULL);
    
     if(ret==0)
		return KErrNone;
	else	
		return KErrGeneral;

    }   
// -----------------------------------------------------------------------------
// Cstlport_vec::vec3_testL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cstlport_vec::vec3_testL( CStifItemParser& /*aItem*/ )
    {

      int ret=0;
    ret = vec3_test(0, NULL);
    
     if(ret==0)
		return KErrNone;
	else	
		return KErrGeneral;

    }

// -----------------------------------------------------------------------------
// Cstlport_vec::vec4_testL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cstlport_vec::vec4_testL( CStifItemParser& /*aItem*/ )
    {

      int ret=0;
    ret = vec4_test(0, NULL);
    
     if(ret==0)
		return KErrNone;
	else	
		return KErrGeneral;

    }

// -----------------------------------------------------------------------------
// Cstlport_vec::vec5_testL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cstlport_vec::vec5_testL( CStifItemParser& /*aItem*/ )
    {

      int ret=0;
    ret = vec5_test(0, NULL);
    
     if(ret==0)
		return KErrNone;
	else	
		return KErrGeneral;

    }
// -----------------------------------------------------------------------------
// Cstlport_vec::vec6_testL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cstlport_vec::vec6_testL( CStifItemParser& /*aItem*/ )
    {

      int ret=0;
    ret = vec6_test(0, NULL);
    
     if(ret==0)
		return KErrNone;
	else	
		return KErrGeneral;

    }    

// -----------------------------------------------------------------------------
// Cstlport_vec::vec7_testL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cstlport_vec::vec7_testL( CStifItemParser& /*aItem*/ )
    {

      int ret=0;
    ret = vec7_test(0, NULL);
    
     if(ret==0)
		return KErrNone;
	else	
		return KErrGeneral;

    }
    
// -----------------------------------------------------------------------------
// Cstlport_vec::vec8_testL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cstlport_vec::vec8_testL( CStifItemParser& /*aItem*/ )
    {

      int ret=0;
    ret = vec8_test(0, NULL);
    
     if(ret==0)
		return KErrNone;
	else	
		return KErrGeneral;

    }    
// -----------------------------------------------------------------------------
// Cstlport_vec::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
TInt Cstlport_vec::?member_function(
   CItemParser& aItem )
   {

   ?code

   }
*/

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  End of File
