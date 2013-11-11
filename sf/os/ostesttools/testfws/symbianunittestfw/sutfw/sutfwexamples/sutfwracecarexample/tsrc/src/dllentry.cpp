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

#include "ut_racecar.h"
#include <symbianunittestsuite.h>


// Exactly one exported function returning 
// the pointer to the suite of tests for the SymbianUnit framework.
//
EXPORT_C MSymbianUnitTestInterface* CreateTestL()
    {
    CSymbianUnitTestSuite* testSuite = 
        CSymbianUnitTestSuite::NewLC( _L("ut_racecar") );

    testSuite->AddL( UT_CRaceCar::NewLC() );
    CleanupStack::Pop();

    // Add more tests to the test suite here when testing multiple classes

    CleanupStack::Pop( testSuite );
    return testSuite;
    }


