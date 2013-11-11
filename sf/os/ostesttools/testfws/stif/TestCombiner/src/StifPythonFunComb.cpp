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
* Description: This file contains StifPythonFunComb implementation.
*
*/


// INCLUDE FILES
#include "StifPythonFunComb.h"
#include <e32svr.h>

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

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// GetNewIndexForThreadL
// Function countss all threads named like pythonscripter* and returns index for
// the new thread for pythonscripter.
// -----------------------------------------------------------------------------
TInt GetNewIndexForThreadL(void)
	{
	TInt max_index = 0;
	// * as a wildcard for the name search
	_LIT(KFindAll, "pythonscripter*");

	// default RThread object, has a handle of the current thread
	RThread thread;
	TFullName fullName;
	TFindThread finder(KFindAll);

	while(finder.Next(fullName) == KErrNone)
		{
		max_index++;
		}
	return max_index;
	}
//
// ============================ MEMBER FUNCTIONS ===============================

// End of File
