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
* Description: This module contains implementation of 
* CTestModuleParamVer01 class and CTestModuleParamVer01 class 
* member functions.
*
*/

// INCLUDE FILES
#include <stifinternal/TestServerClient.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ==================== LOCAL FUNCTIONS =======================================

// None

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

    Class: CTestModuleParamVer01

    Method: NewL

    Description: Creates CTestModuleParamVer01
      
    Parameters: None

    Return Values: CTestModuleParamVer01: CTestModuleParamVer01 object

    Errors/Exceptions: Leaves if memory allocation fails.

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
EXPORT_C CTestModuleParamVer01* CTestModuleParamVer01::NewL()
    {
    CTestModuleParamVer01* self = new( ELeave ) CTestModuleParamVer01();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleParamVer01

    Method: CTestModuleParamVer01

    Description: Constructor.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
CTestModuleParamVer01::CTestModuleParamVer01()
    {
    iVersio = EVersio1;

    // Default test module thread heap and stack sizes.
    iTestThreadStackSize = KStackSize;
	iTestThreadMinHeap = KTestThreadMinHeap;
	iTestThreadMaxHeap = KTestThreadMaxHeap;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleParamVer01

    Method: ~CTestModuleParamVer01

    Description: Destructor.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
EXPORT_C CTestModuleParamVer01::~CTestModuleParamVer01()
    {
    // None

    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleParamVer01

    Method: ConstructL

    Description: Second level constructor.
    
    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CTestModuleParamVer01::ConstructL()
    {
	// None

	}

// End of File
