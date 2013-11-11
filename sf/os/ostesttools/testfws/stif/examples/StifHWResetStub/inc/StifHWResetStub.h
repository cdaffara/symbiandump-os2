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
* Description: This file contains the header file of 
* CStifHWResetStub of STIF TestFramework.
*
*/

#ifndef STIFHWRESETSTUB_H
#define STIFHWRESETSTUB_H

// INCLUDES
#include <StifHWReset.h>
#include <StifLogger.h>

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
// None

// DESCRIPTION
// Implements common reset module for STIF TestFramework. According to this DLL
// template can be implemented licensee specific reset module.
NONSHARABLE_CLASS(CStifHWResetStub) 
        :public CStifHWReset
    {
    public:     // Enumerations
    	// None

    private:    // Enumerations
    	// None

    public:     // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CStifHWResetStub* NewL();
        
        /**
        * Destructor.
        */
        ~CStifHWResetStub();

    public:     // New functions
        
        /**
        * Does reset according to reset type.
        */
        TInt DoReset( CTestModuleIf::TRebootType aResetType );
        
    public:     // Functions from base classes
    	// None
       
    protected:  // New functions
    	// None
    	
    protected:  // Functions from base classes
        // None
    	
    private:

        /**
        * C++ default constructor.
        */
        CStifHWResetStub();

        /**
        * Symbian OS constructor.
        */
        void ConstructL();        

        /**
        * Kills process. Example method.
        */
        TInt KillProcess();

        /**
        * General reset. Example method.
        */
        TInt GeneralReset();

	public:     // Data
	    // None

	protected:  // Data
        // None

    private:    // Data

        // Logger instance
        CStifLogger*            iLogger;

    public:     // Friend classes
        // None

    protected:  // Friend classes
        // None

    private:    // Friend classes
        // None
    	
    };

#endif // STIFHWRESETSTUB_H

// End of File
