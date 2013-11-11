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

#ifndef SYMBIANUNITTESTFILEOUTPUTWRITER_H
#define SYMBIANUNITTESTFILEOUTPUTWRITER_H

// INCLUDES
#include <f32file.h>
#include <e32base.h>

// CLASS DECLARATION
class CSymbianUnitTestFileOutputWriter : public CBase
    {
    public: // Constructors and destructor
    
        static CSymbianUnitTestFileOutputWriter* NewLC( 
            const TDesC& aFileName );
        
        static CSymbianUnitTestFileOutputWriter* NewL( 
                const TDesC& aFileName );
        
        ~CSymbianUnitTestFileOutputWriter();
   
    public: // New functions
        
        void WriteDateAndTimeL();
        void WriteL( const TDesC8& aValue );
        void WriteL( const TDesC8& aFormat, TInt aValue );
        void WriteL( const TDesC& aValue);
    
    private: // Constructors
    
        CSymbianUnitTestFileOutputWriter();
        void ConstructL( const TDesC& aFileName ); 
   
    private: // Data
        
        RFile iFile;
        RFs iRFs;
    };

#endif // SYMBIANUNITTESTFILEOUTPUTWRITER_H
