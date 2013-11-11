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
#ifndef SYMBIANUNITTESTOUTPUTASHTML_H
#define SYMBIANUNITTESTOUTPUTASHTML_H

// INCLUDES
#include "symbianunittestoutputformatter.h"

// FORWARD DECLARATIONS
class CSymbianUnitTestFailure;

// CONTANTS
_LIT( KHtmlOutput, "html" );

// CLASS DECLARATION
class CSymbianUnitTestOutputAsHtml : public CSymbianUnitTestOutputFormatter
    {
    public: // Constructors and destructor
    
        static CSymbianUnitTestOutputAsHtml* NewL( const TDesC& aFileName );
        ~CSymbianUnitTestOutputAsHtml();
    
    protected: // From CSymbianUnitTestOutputFormatter
        
        const TDesC& FileExtension() const;      
        void PrintHeaderL( CSymbianUnitTestResult& aResult );
        void PrintPassedTestsL( CSymbianUnitTestResult& aResult );
        void PrintFailedTestsL( CSymbianUnitTestResult& aResult );
        void PrintFooterL();
        
    private: // Constructors
        
        CSymbianUnitTestOutputAsHtml();
        
    private: // New functions
    
        void PrintTableRowL( const TDesC8& aTitle, const TDesC8& aValue );
    };

#endif // SYMBIANUNITTESTOUTPUTASHTML_H
