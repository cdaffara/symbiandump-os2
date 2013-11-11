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

#ifndef SYMBIANUNITTESTOUTPUTASXML_H
#define SYMBIANUNITTESTOUTPUTASXML_H

// INCLUDES
#include "symbianunittestoutputformatter.h"

// FORWARD DECLARATIONS
class CSymbianUnitTestFailure;

// CONTANTS
_LIT( KXmlOutput, "xml" );

// CLASS DECLARATION
class CSymbianUnitTestOutputAsXml : public CSymbianUnitTestOutputFormatter
    {
    public: // Constructors and destructor
    
        static CSymbianUnitTestOutputAsXml* NewL( const TDesC& aFileName );
        ~CSymbianUnitTestOutputAsXml();
    
    protected: // From CSymbianUnitTestOutputFormatter
        
        const TDesC& FileExtension() const;        
        void PrintHeaderL( CSymbianUnitTestResult& aResult );
        void PrintPassedTestsL( CSymbianUnitTestResult& aResult );
        void PrintFailedTestsL( CSymbianUnitTestResult& aResult );
        void PrintFooterL();
        
    private: // Constructors
        
        CSymbianUnitTestOutputAsXml();
    
    private: // New functions
    
        void PrintFailureDetailsL( CSymbianUnitTestFailure& aTestFailure );
    };

#endif // SYMBIANUNITTESTOUTPUTASXML_H
