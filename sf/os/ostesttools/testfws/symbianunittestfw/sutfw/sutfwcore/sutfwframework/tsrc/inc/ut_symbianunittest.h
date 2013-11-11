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

#ifndef UT_SYMBIANUNITTEST_H
#define UT_SYMBIANUNITTEST_H

// INCLUDES
#include <symbianunittest.h>
#include "symbianunittestobserver.h"
#include <badesca.h>

// FORWARD DECLARATIONS
class CSymbianUnitTestResult;

// CLASS DECLARATION
NONSHARABLE_CLASS(UT_CSymbianUnitTest) : 
    public CSymbianUnitTest, 
    public MSymbianUnitTestObserver
    {
    public: // Constructors and destructor
    
        static UT_CSymbianUnitTest* NewL();
        static UT_CSymbianUnitTest* NewLC();
        ~UT_CSymbianUnitTest();
    
    public: // From MSymbianUnitTestObserver
        
        void TestPass(const TDesC& aTestCaseName);
        void TestFailed(const TDesC& aTestCaseName, const TDesC8& aErrMsg);
        
    protected: // Test functions
    
        void SetupL();
        void Teardown();        
        void UT_NameL();
        void UT_IsMemoryAllocationFailureSimulationUsedL();
        void UT_PanicInTestL();
        void UT_ErrorInSetupL();
        void UT_LeaveInTestL();
        void UT_MemoryLeakInTestL();
        void UT_EqualsAssertsL();
        void UT_FailingAssertsL();
        void UT_PassingAssertsL();
        void UT_TestCaseNamesL();
        void UT_TimeoutL();
        void UT_ResourceLeakInTestL();
        void UT_RequestLeakInTestL();

    protected: // Failing functions to simulate failing tests
        
        void SimulatePanicL();
        void SimulateLeaveL();
        void SimulateMemoryLeakL();
        void SimulateFailingAssertL();
        void SimulateFailingAssertWithIntsL();
        void SimulateFailingAssertWithDesC8sL();
        void SimulateFailingAssertWithDesC16sL();
        void SimulateFailingAssertLeaveL();
        void SimulateFailingAssertLeaveWithSpecificValueL();
        void SimulateTimeoutL();
        void SimulateEqualsAssertWithStringL();
		void SimulateEqualsAssertWithStringFailL();
		void SimulateEqualsAssertWithString8L();
		void SimulateEqualsAssertWithString8FailL();
		void SimulateEqualsAssertWithIntL();
		void SimulateEqualsAssertWithIntFailL();
        void SimulateResourceLeakL();
        void SimulateRequestLeakL();
        
    protected: // Dummy function to simulate setup and teardown
        
        void Dummy();
        
    private: // Contructors
        
        UT_CSymbianUnitTest();
        void ConstructL();
        
    private: // Data
        
        CSymbianUnitTestResult* iTestResult;
        UT_CSymbianUnitTest* iUnitTest;
        TInt iExecutedTestCount;
        CDesCArray* iTestCaseNames;
    };

#endif // UT_SYMBIANUNITTEST_H
