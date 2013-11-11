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

#include "ut_symbianunittestsuite.h"
#include <symbianunittestsuite.h>
#include <symbianunittestmacros.h>

_LIT( KTestSuiteName, "TestSuite" );

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
UT_CSymbianUnitTestSuite* UT_CSymbianUnitTestSuite::NewL()
    {
    UT_CSymbianUnitTestSuite* self = UT_CSymbianUnitTestSuite::NewLC(); 
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
UT_CSymbianUnitTestSuite* UT_CSymbianUnitTestSuite::NewLC()
    {
    UT_CSymbianUnitTestSuite* self = 
        new( ELeave )UT_CSymbianUnitTestSuite();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
UT_CSymbianUnitTestSuite::UT_CSymbianUnitTestSuite()
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void UT_CSymbianUnitTestSuite::ConstructL()
    {
    BASE_CONSTRUCT
    
    DEFINE_TEST_CLASS( UT_CSymbianUnitTestSuite )
    
    ADD_SUT( UT_NameL )
    ADD_SUT( UT_AddL )
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
UT_CSymbianUnitTestSuite::~UT_CSymbianUnitTestSuite()
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void UT_CSymbianUnitTestSuite::SetupL()
    {
    iUnitTestSuite = CSymbianUnitTestSuite::NewL( KTestSuiteName );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void UT_CSymbianUnitTestSuite::Teardown()
    {
    delete iUnitTestSuite;
    iUnitTestSuite = NULL;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void UT_CSymbianUnitTestSuite::UT_NameL()
    {
    SUT_ASSERT_EQUALS( KTestSuiteName, iUnitTestSuite->Name() )
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void UT_CSymbianUnitTestSuite::UT_AddL()
    {
    SUT_ASSERT_EQUALS( 0, iUnitTestSuite->TestCaseCount() )

    UT_CSymbianUnitTestSuite* thisTest = UT_CSymbianUnitTestSuite::NewLC();
    iUnitTestSuite->AddL( thisTest );
    CleanupStack::Pop( thisTest );
    SUT_ASSERT_EQUALS( 
        thisTest->TestCaseCount(), iUnitTestSuite->TestCaseCount() )
    }
