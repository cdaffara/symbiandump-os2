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
#ifndef SYMBIANUNITTESTOUTPUTFORMATTER_H
#define SYMBIANUNITTESTOUTPUTFORMATTER_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CSymbianUnitTestResult;
class CSymbianUnitTestFileOutputWriter;

// CLASS DECLARATION
class CSymbianUnitTestOutputFormatter : public CBase 
    {
    public: // Destructor        

        ~CSymbianUnitTestOutputFormatter();
    
    public: // New functions
        
        void PrintL( CSymbianUnitTestResult& aResult );
    
    protected: // Constructors
    
        CSymbianUnitTestOutputFormatter();
        void ConstructL( const TDesC& aFileName );

    protected: // Pure virtual functions
    
        virtual const TDesC& FileExtension() const = 0;
        virtual void PrintHeaderL( CSymbianUnitTestResult& aResult ) = 0;
        virtual void PrintPassedTestsL( CSymbianUnitTestResult& aResult ) = 0;
        virtual void PrintFailedTestsL( CSymbianUnitTestResult& aResult ) = 0;
        virtual void PrintFooterL() = 0;
    
    protected: // Data
        
        CSymbianUnitTestFileOutputWriter* iOutputWriter; // Owned
    };

#endif // SYMBIANUNITTESTOUTPUTFORMATTER_H
