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
* Description: This file contains the header file of the 
* CSTIFTestFrameworkSettings.
*
*/

#ifndef STIFTESTFRAMEWORKSETTINGS_H
#define STIFTESTFRAMEWORKSETTINGS_H

//  INCLUDES
#include <e32base.h>
#include <StifLogger.h>
#include <StifParser.h>

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
// CSTIFTestFrameworkSettings is a STIF Test Framework Test Engine class.
// Class contains different operations of settings parsing from Test Framework
// initíalization file.

class CSTIFTestFrameworkSettings 
        :public CBase
    {
    public:     // Enumerations

    private:    // Enumerations

    public:     // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CSTIFTestFrameworkSettings* NewL();

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CSTIFTestFrameworkSettings();

    public:     // New functions

        /**
        * Get report mode settings from STIF TestFramework's ini file.
        */
        IMPORT_C TInt GetReportModeL( CStifSectionParser* aSectionParser,
                                                TPtrC aTag,
                                                TUint& aSetting );

        /**
        * Get file settings from STIF TestFramework's ini file.
        */
        IMPORT_C TInt GetFileSetting( CStifSectionParser* aSectionParser,
                                                TPtrC aTag,
                                                TPtrC& aSetting );

        /**
        * Get format setting from STIF TestFramework's ini file.
        */
        IMPORT_C TInt GetFormatL( CStifSectionParser* aSectionParser,
                                        TPtrC aTag,
                                        CStifLogger::TLoggerType& aLoggerType );

        /**
        * Get output setting from STIF TestFramework's ini file.
        */
        IMPORT_C TInt GetOutputL( CStifSectionParser* aSectionParser,
                                        TPtrC aTag,
                                        CStifLogger::TOutput& aOutput );

        /**
        * Get boolean type of settings from STIF TestFramework's ini file.
        */
        IMPORT_C TInt GetBooleanSettingsL( CStifSectionParser* aSectionParser,
                                            TPtrC aTag,
                                            TBool& aOverwrite );

        /**
        * Get file creation mode setting from STIF TestFramework's ini file.
        */
        IMPORT_C TInt GetOverwriteL( CStifSectionParser* aSectionParser,
                                                TPtrC aTag,
                                                TBool& aOverwrite );

        /**
        * Get format setting from STIF TestFramework's ini file.
        */
        IMPORT_C TInt GetFormatL( CStifSectionParser* aSectionParser,
                                        TPtrC aTag,
                                        CStifLogger::TLoggerType& aLoggerType,
                                        TBool &aXML );

    public:     // Functions from base classes

    protected:  // New functions

    protected:  // Functions from base classes

    private:

        /**
        * C++ default constructor.
        */
        CSTIFTestFrameworkSettings();

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

    public:     // Data

    protected:  // Data

    private:    // Data

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

    };

#endif      // STIFTESTFRAMEWORKSETTINGS_H

// End of File
