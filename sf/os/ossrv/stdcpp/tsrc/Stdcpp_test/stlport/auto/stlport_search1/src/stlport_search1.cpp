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
#include <Stiftestinterface.h>
#include "stlport_search1.h"
#ifdef LIBC_OLD_BACKEND
#include <mrtlib.h>
#endif
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
// Cstlport_search1::Cstlport_search1
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
Cstlport_search1::Cstlport_search1( 
    CTestModuleIf& aTestModuleIf ):
        CScriptBase( aTestModuleIf )
    {
    }

// -----------------------------------------------------------------------------
// Cstlport_search1::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void Cstlport_search1::ConstructL()
    {
    iLog = CStifLogger::NewL( Kstlport_search1LogPath, 
                          Kstlport_search1LogFile,
                          CStifLogger::ETxt,
                          CStifLogger::EFile,
                          EFalse );

    }

// -----------------------------------------------------------------------------
// Cstlport_search1::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
Cstlport_search1* Cstlport_search1::NewL( 
    CTestModuleIf& aTestModuleIf )
    {
#ifdef LIBC_OLD_BACKEND
    MrtInit();
#endif
 #ifdef __ARMCC__
    #pragma diag_suppress 830
 #endif    
    Cstlport_search1* self = new (ELeave) Cstlport_search1( aTestModuleIf );

 
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;

    }

// Destructor
Cstlport_search1::~Cstlport_search1()
    { 
#ifdef LIBC_OLD_BACKEND
    MrtClose();
#endif
    // Delete resources allocated from test methods
    Delete();

    // Delete logger
    delete iLog; 

    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// LibEntryL is a polymorphic Dll entry point.
// Returns: CScriptBase: New CScriptBase derived object
// -----------------------------------------------------------------------------
//
EXPORT_C CScriptBase* LibEntryL( 
    CTestModuleIf& aTestModuleIf ) // Backpointer to STIF Test Framework
    {

    return ( CScriptBase* ) Cstlport_search1::NewL( aTestModuleIf );

    }

// -----------------------------------------------------------------------------
// E32Dll is a DLL entry point function.
// Returns: KErrNone
// -----------------------------------------------------------------------------
//
#ifndef EKA2 // Hide Dll entry point to EKA2
GLDEF_C TInt E32Dll(
    TDllReason /*aReason*/) // Reason code
    {
    return(KErrNone);

    }
#endif // EKA2

//  End of File