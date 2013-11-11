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

#ifndef UT_SYMBIANUNITTESTRESULT_H
#define UT_SYMBIANUNITTESTRESULT_H

// INCLUDES
#include <symbianunittest.h>

// FORWARD DECLARATIONS
class CSymbianUnitTestResult;

// CLASS DECLARATION
NONSHARABLE_CLASS( UT_CSymbianUnitTestResult ) : public CSymbianUnitTest
    {
    public: // Constructors and destructor
    
        static UT_CSymbianUnitTestResult* NewL();
        static UT_CSymbianUnitTestResult* NewLC();
        ~UT_CSymbianUnitTestResult();
    
    protected: // Test functions
    
        void SetupL();
        void Teardown();       
        void UT_TestCountL();
        void UT_StartTestL();
        void UT_AddFailureL();
        void UT_AddSetupErrorL();        
        void UT_AddLeaveFromTestL();       
        void UT_AddAssertFailureL();
        void UT_AddMemoryLeakInfoL();
        void UT_AddPanicInfoL();        
	void UT_AddTimeOutErrorL();

    private: // Contructors
        
        UT_CSymbianUnitTestResult();
        void ConstructL();        
        
    private: // Data
    
        CSymbianUnitTestResult* iUnitTestResult;
    };

#endif // UT_SYMBIANUNITTESTRESULT_H
