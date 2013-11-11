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
#ifndef SYMBIANUNITTESTOUTPUTASTEXT_H
#define SYMBIANUNITTESTOUTPUTASTEXT_H

// INCLUDES
#include "symbianunittestoutputformatter.h"

// FORWARD DECLARATIONS
class CSymbianUnitTestFailure;

// CONSTANTS
_LIT( KTextOutput, "txt" );

// CLASS DECLARATION
class CSymbianUnitTestOutputAsText : public CSymbianUnitTestOutputFormatter
    {
    public: // Constructors and destructor
    
        static CSymbianUnitTestOutputAsText* NewL( const TDesC& aFileName );
        ~CSymbianUnitTestOutputAsText();
    
    protected: // From CSymbianUnitTestOutputFormatter
        
        const TDesC& FileExtension() const;       
        void PrintHeaderL( CSymbianUnitTestResult& aResult );
        void PrintPassedTestsL( CSymbianUnitTestResult& aResult );
        void PrintFailedTestsL( CSymbianUnitTestResult& aResult );
        void PrintFooterL();
        
    private: // Constructors
        
        CSymbianUnitTestOutputAsText();
    
    private: // New functions
        
        void PrintFailureDetailsL( CSymbianUnitTestFailure& aTestFailure );
    };

#endif // SYMBIANUNITTESTOUTPUTASTEXT_H
