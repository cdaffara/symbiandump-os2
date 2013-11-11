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

#ifndef UT_SYMBIANUNITTESTSUITE_H
#define UT_SYMBIANUNITTESTSUITE_H

// INCLUDES
#include <symbianunittest.h>

// FORWARD DECLARATIONS
class CSymbianUnitTestSuite;

// CLASS DECLARATION
NONSHARABLE_CLASS( UT_CSymbianUnitTestSuite ) : public CSymbianUnitTest
    {
    public: // Constructors and destructor
    
        static UT_CSymbianUnitTestSuite* NewL();
        static UT_CSymbianUnitTestSuite* NewLC();
        ~UT_CSymbianUnitTestSuite();
    
    protected: // Test functions
    
        void SetupL();
        void Teardown();
        void UT_NameL();
        void UT_AddL();  
        
    private: // Contructors
        
        UT_CSymbianUnitTestSuite();
        void ConstructL();        
        
    private: // Data
    
        CSymbianUnitTestSuite* iUnitTestSuite;
    };

#endif // UT_SYMBIANUNITTESTSUITE_H
