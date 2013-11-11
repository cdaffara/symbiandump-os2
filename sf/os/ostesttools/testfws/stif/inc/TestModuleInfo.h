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
* CTestModuleInfo class. The class was created to keep module names, 
* ini files and test case configuration files.
*
*/

#ifndef TESTMODULEINFO_H
#define TESTMODULEINFO_H

//  INCLUDES
#include <e32base.h>
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

// DESCRIPTION
// Class with information about configuration (test case) file.
class CTestCaseFileInfo
    :public CBase
    {
    public: // Enumerations

    private: // Enumerations

    public:  // Constructors and destructor
        /**
        * NewL.
        */
        static CTestCaseFileInfo* NewL(TDesC& aCfgFileName, CStifLogger* aLogger);

        /**
        * C++ destructor.
        */
        virtual ~CTestCaseFileInfo();

    private: // Constructors and destructor
        /**
        * C++ constructor.
        */
        CTestCaseFileInfo();

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL(TDesC& aCfgFileName, CStifLogger* aLogger);

    public: // New functions
        /**
        * Set configuration file as already checked.
        */
        IMPORT_C void SetChecked(void);

        /**
        * Return if config file has been checked.
        */
        TBool IsChecked(void);

        /**
        * Get module name.
        */
        IMPORT_C TInt GetCfgFileName(TDes& aCfgFileName);

    protected:  // New functions

    private:    // New functions

    public:     // Data

    protected:  // Data

    private:    // Data
        //Config file name
        HBufC* iCfgFileName;

        //Was checked
        TBool iChecked;

        // Logger instance
        CStifLogger* iLogger;

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes
    };

// DESCRIPTION
// Class with information about test modules.
class CTestModuleInfo
    :public CBase
    {
    public: // Enumerations

    private: // Enumerations

    public:  // Constructors and destructor
        /**
        * NewL.
        */
        static CTestModuleInfo* NewL(TDesC& aModuleName, CStifLogger* aLogger);

        /**
        * C++ destructor.
        */
        virtual ~CTestModuleInfo();

    private: // Constructors and destructor
        /**
        * C++ constructor.
        */
        CTestModuleInfo();

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL(TDesC& aModuleName, CStifLogger* aLogger);

    public: // New functions
        /**
        * Set initialization file name.
        */
        IMPORT_C TInt SetIniFile(TDesC& aIniFileName);

        /**
        * Add configuration (test case) file name to array.
        */
        IMPORT_C TInt AddCfgFile(TDesC& aCfgFileName);

        /**
        * Get number of test case files.
        */
        IMPORT_C TInt CountCfgFiles(void);

        /**
        * Get module name.
        */
        IMPORT_C TInt GetModuleName(TDes& aModuleName);

        /**
        * Get ini file.
        */
        IMPORT_C TInt GetIniFileName(TDes& aIniFileName);

        /**
        * Get cfg file.
        */
        IMPORT_C TInt GetCfgFileName(TInt aIndex, TDes& aCfgFileName);

        /**
        * Get first unchecked config file.
        */
        CTestCaseFileInfo* GetUncheckedCfgFile(void);

    protected:  // New functions

    private:    // New functions

    public:     // Data

    protected:  // Data

    private:    // Data
        //Module name
        HBufC* iModuleName;

        //Initialization file
        HBufC* iIniFileName;

        //Array of cfg files
        RPointerArray<CTestCaseFileInfo> iCfgFiles;

        // Logger instance
        CStifLogger* iLogger;

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes
    };

// DESCRIPTION
// Class with list of test modules.
class CTestModuleList
    :public CBase
    {
    public: // Enumerations

    private: // Enumerations

    public:  // Constructors and destructor
        /**
        * NewL.
        */
        IMPORT_C static CTestModuleList* NewL(CStifLogger* aLogger);

        /**
        * C++ destructor.
        */
        IMPORT_C virtual ~CTestModuleList();

    private: // Constructors and destructor
        /**
        * C++ constructor.
        */
        CTestModuleList();

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL(CStifLogger* aLogger);

    public: // New functions
        /**
        * Add test module to list.
        */
        IMPORT_C TInt AddTestModule(TDesC& aModuleName);

        /**
        * Get number of test modules.
        */
        IMPORT_C TInt Count(void);

        /**
        * Get module.
        */
        IMPORT_C CTestModuleInfo* GetModule(TDesC& aModuleName);

        /**
        * Get module.
        */
        IMPORT_C CTestModuleInfo* GetModule(TInt aIndex);

        /**
        * Get first unchecked test case file (module: testscripter and testcombiner).
        */
        IMPORT_C CTestCaseFileInfo* GetUncheckedCfgFile(void);

    protected:  // New functions

    private:    // New functions

    public:     // Data

    protected:  // Data

    private:    // Data
        //Array of test modules
        RPointerArray<CTestModuleInfo> iTestModules;

        // Logger instance
        CStifLogger* iLogger;

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes
    };

#endif      // TESTMODULEINFO_H

// End of File
