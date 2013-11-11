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
* Description: This file contains the header file of CStifHWReset 
* of STIF TestFramework.
*
*/

#ifndef STIFHWRESET_H
#define STIFHWRESET_H

// INCLUDES
#include <e32base.h>
#include <e32std.h>

#include "StifTestModule.h"

// CONSTANTS
// None

// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// CLASS DECLARATION

// DESCRIPTION
// This is class which can be inherited from own code to perform a
// reset to the system.
// This is used when calling dynamic loaded dll from STIF TestFramework.
class CStifHWReset
        :public CBase
    {
    public:     // Enumerations

        // Different reset types. Possibility to do different king of reset.
/*        enum TResetType
            {
            EResetType0,
            EResetType1,
            EResetType2,
            EResetType3,
            EResetType4,
            EResetType5
            };
*/
    public:     // Enumerations
    	// None

    private:    // Enumerations
    	// None

    public:     // Constructors and destructor
    	// None

    public:     // New functions
        
        /**
        * Defines reset type and forwards information to reset module.
        * This method is pure virtual and it is mandatory to implement it
        * to specific reset module.
        */
        virtual TInt DoReset( CTestModuleIf::TRebootType aResetType ) = 0;
        
    public:     // Functions from base classes
        // None

    protected:  // New functions
    	// None

    protected:  // Functions from base classes
        // None

    private:
        // None

	public:     // Data
	    // None

	protected:  // Data
        // None

    private:    // Data
        // None

    public:     // Friend classes
        // None
    	
    protected:  // Friend classes
        // None
    	
    private:    // Friend classes
        // None
    	
    };

#endif // STIFHWRESET_H

// End of File
