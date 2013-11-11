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
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "BCOpenCHeaders.h"

//Headers exported from Open C headers API
#include <cstdlib>
#include <stdlib.h>
#include <cctype>
#include <csignal>
#include <cwctype>
#include <cstdio>
#include <stddef.h>
#include <cstddef>
#include <setjmp.h>
#include <string.h>
#include <wchar.h>
#include <cwchar>
#include <clocale>
#include <cstdarg>
#include <cctype>
#include <csetjmp>
#include <math.h>
#include <cstdlib>
#include <climits>
#include <csignal>
#include <cwctype>
#include <ctime>
#include <cstddef>
#include <locale.h>
#include <ctime>
#include <time.h>
#include <cerrno>
#include <cstdarg>
#include <csetjmp>
#include <cstring>
#include <stdio.h>
#include <wctype.h>
#include <cmath>
#include <cassert>
#include <ctype.h>
#include <stdarg.h>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <clocale>
#include <cwchar>
#include <cfloat>

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
// CBCOpenCHeaders::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CBCOpenCHeaders::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// CBCOpenCHeaders::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CBCOpenCHeaders::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "TestAPI", CBCOpenCHeaders::TestAPI ),
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// CBCOpenCHeaders::TestAPI
// TestAPI test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CBCOpenCHeaders::TestAPI( CStifItemParser& aItem )
    {

    // Print to UI
    _LIT( KBCOpenCHeaders, "BCOpenCHeaders" );
    _LIT( KTestAPI, "In TestAPI" );
    TestModuleIf().Printf( 0, KBCOpenCHeaders, KTestAPI );
    // Print to log file
    iLog->Log( KTestAPI );

		_LIT( KResult, "No functions to be tested");
		_LIT( KComment, "Open C Headers API has passed");
    TestModuleIf().Printf( 0, KResult, KComment);

    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CBCOpenCHeaders::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
TInt CBCOpenCHeaders::?member_function(
   CItemParser& aItem )
   {

   ?code

   }
*/

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove
