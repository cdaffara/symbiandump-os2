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

#include "ut_symbianunittestfailure.h"
#include "ut_symbianunittestresult.h"
#include "ut_symbianunittest.h"
#include "ut_symbianunittestsuite.h"
#include <symbianunittestsuite.h>


/**
 * Test suite factory function.
 */
EXPORT_C MSymbianUnitTestInterface* CreateTestSuiteL()
    {
    CSymbianUnitTestSuite* testSuite = 
        CSymbianUnitTestSuite::NewLC( _L("ut_sutfwframework") );

    testSuite->AddL( UT_CSymbianUnitTestFailure::NewLC() );
    CleanupStack::Pop();

    testSuite->AddL( UT_CSymbianUnitTestResult::NewLC() );
    CleanupStack::Pop();

    testSuite->AddL( UT_CSymbianUnitTest::NewLC() );
    CleanupStack::Pop();

    testSuite->AddL( UT_CSymbianUnitTestSuite::NewLC() );
    CleanupStack::Pop();
    
    CleanupStack::Pop( testSuite );
    return testSuite;    
    }

//  END OF FILE
