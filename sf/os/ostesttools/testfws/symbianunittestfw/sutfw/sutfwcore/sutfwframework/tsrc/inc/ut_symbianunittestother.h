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

#ifndef UT_SYMBIANUNITTESTOTHER_H
#define UT_SYMBIANUNITTESTOTHER_H

// INCLUDES
#include <symbianunittest.h>

// FORWARD DECLARATIONS
class CSymbianUnitTestFailure;

// CLASS DECLARATION
NONSHARABLE_CLASS( UT_CSymbianUnitTestOther ) : public CSymbianUnitTest
    {
    public: // Constructors and destructor
    
        static UT_CSymbianUnitTestOther* NewL();
        static UT_CSymbianUnitTestOther* NewLC();
        ~UT_CSymbianUnitTestOther();
    
    protected: // Test functions
    
        /*void SetupL();
        void Teardown();        */
        void UT_GettersL();

    private: // Contructors
        
        UT_CSymbianUnitTestOther();
        void ConstructL();        
        
    private: // Data
    
        CSymbianUnitTestFailure* iUnitTestFailure;
    };

#endif // UT_SYMBIANUNITTESTOTHER_H
