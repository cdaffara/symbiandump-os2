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

#ifndef SYMBIANUNITTESTFAILURE_H
#define SYMBIANUNITTESTFAILURE_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class MSymbianUnitTestInterface;

// CLASS DECLARATION
class CSymbianUnitTestFailure : public CBase
    {
    public: // Constructors and destructor
    
        static CSymbianUnitTestFailure* NewL(
            const TDesC& aTestName,
            const TDesC8& aFailureMessage,
            TInt aLineNumber,
            const TDesC8& aFileName );

        static CSymbianUnitTestFailure* NewLC(
            const TDesC& aTestName,
            const TDesC8& aFailureMessage,
            TInt aLineNumber,
            const TDesC8& aFileName ); 
        
        ~CSymbianUnitTestFailure();
    
    public: // New functions
        
        const TDesC8& TestName();
        const TDesC8& FailureMessage();
        TInt LineNumber();
        const TDesC8& FileName();
    
    private: // Constructors
        
        CSymbianUnitTestFailure( TInt aLineNumber );
        
        void ConstructL( 
            const TDesC& aTestName,
            const TDesC8& aFailureMessage,
            const TDesC8& aFileName );
        
    private: // Data
    
        TInt iLineNumber;
        HBufC8* iTestName;
        HBufC8* iFailureMessage;
        HBufC8* iFileName;  
    };

#endif // SYMBIANUNITTESTFAILURE_H
