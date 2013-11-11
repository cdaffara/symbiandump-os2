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

#ifndef UT_SYMBIANUNITTESTCOMMANDLINEPARSER_H
#define UT_SYMBIANUNITTESTCOMMANDLINEPARSER_H

// INCLUDES
#include <symbianunittest.h>

// FORWARD DECLARATIONS
class CSymbianUnitTestCommandLineParser;

// CLASS DECLARATION
class UT_CSymbianUnitTestCommandLineParser  : public CSymbianUnitTest
    {
    public: // Constructors and destructor
    
        static UT_CSymbianUnitTestCommandLineParser* NewL();
        static UT_CSymbianUnitTestCommandLineParser* NewLC();
        ~UT_CSymbianUnitTestCommandLineParser();
    
    protected: // Test functions
    
        void SetupL();
        void Teardown();
        void UT_SetOutputFormatL();
        void UT_SetTestDllNamesL();
        void UT_FindArgumentL();
        void UT_SetTimeoutL();
        void UT_SetTestCaseNamesL();
        
        // new added. 
        void UT_TestCommandLineL();
        void LaunchExample( TInt aType );
    
    private: // Contructors
        
        UT_CSymbianUnitTestCommandLineParser();
        void ConstructL();  

    private: // Data
    
        // The object to be tested as a member variable:
        CSymbianUnitTestCommandLineParser* iParser;
    };

#endif // UT_SYMBIANUNITTESTCOMMANDLINEPARSER_H
