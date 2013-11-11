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

#ifndef SYMBIANUNITTESTCOMMANDLINEPARSER_H
#define SYMBIANUNITTESTCOMMANDLINEPARSER_H

// INCLUDES
#include <symbianunittestmacros.h>
#include <e32base.h>
#include <badesca.h>
#include <bacline.h>

// CLASS DECLARATION
class CSymbianUnitTestCommandLineParser : public CBase
    {
    public: // Constructors and destructor
    
        static CSymbianUnitTestCommandLineParser* NewLC();
        static CSymbianUnitTestCommandLineParser* NewL();
        ~CSymbianUnitTestCommandLineParser();
    
    public: // New functions
        
        TBool ShowHelp() const;
        const MDesCArray& TestDllNames() const;
        const CDesCArray& TestCaseNames() const;
        TBool MemoryAllocationFailureSimulation() const;
        TBool Background() const;
        const TDesC& OutputFileName() const;
        const TDesC& OutputFormat() const;
        TInt  Timeout() const;
        TBool PromptUser() const;
        
    private: // Constructors
    
        void ConstructL();
        CSymbianUnitTestCommandLineParser();
        
    private: // New functions
        
        void SetOutputFormatL();
        void SetTimeoutL();
        void SetTestDllNamesL();
        void SetTestCaseNamesL();
        TBool FindArgument( 
            const TDesC& aKey,
            const TDesC& aShortKey,
            TPtrC& aValue );
        
    private: // Data
        
        CCommandLineArguments* iArguments;
        TBool iShowHelp;
        CDesCArray* iTestDllNames;
        CDesCArray* iTestCaseNames;
        TBool iAllocFailureSimulation;
        HBufC* iOutputFormat;
        TBool iNoPrompt;
        TBool iBackground;
	TInt iTimeout;
        
    private: // Test
        
        SYMBIAN_UNIT_TEST_CLASS( UT_CSymbianUnitTestCommandLineParser )       
    };

#endif // SYMBIANUNITTESTCOMMANDLINEPARSER_H
