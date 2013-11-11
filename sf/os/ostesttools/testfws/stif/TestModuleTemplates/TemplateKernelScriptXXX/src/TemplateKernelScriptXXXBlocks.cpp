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
* Description: This file contains kernel testclass implementation.
*
*/

// INCLUDE FILES
#include "TemplateKernelScriptXXX.h"

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
// DTemplateKernelScriptXXX::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void DTemplateKernelScriptXXX::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// DTemplateKernelScriptXXX::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// 
// Note that in EKA2 methods cannot leave and therefore RunMethodL isn't leaving 
// function dispite of its name!    
// -----------------------------------------------------------------------------
//
TInt DTemplateKernelScriptXXX::RunMethodL( 
    const TDesC& aMethod, 
    const TDesC& aParams )  
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "Example", DTemplateKernelScriptXXX::ExampleL ),

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aMethod, aParams );

    }

// -----------------------------------------------------------------------------
// DTemplateKernelScriptXXX::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt DTemplateKernelScriptXXX::ExampleL( 
    const TDesC& aParams )
    {
    Kern::Printf( ("DTemplateKernelScriptXXX::ExampleL(%S)"), &aParams );             

    return KErrNone;

    }

// -----------------------------------------------------------------------------
// DTemplateKernelScriptXXX::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
TInt DTemplateKernelScriptXXX::?member_function(
   const TDesC& aParams )
   {

   ?code

   }
*/

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

// End of File
