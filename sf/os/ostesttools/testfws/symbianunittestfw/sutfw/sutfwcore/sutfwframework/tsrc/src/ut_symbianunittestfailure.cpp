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
#include "symbianunittestfailure.h"
#include <symbianunittestmacros.h>

_LIT8( KFailedTestName8, "FailedTest" );
_LIT( KFailedTestName, "FailedTest" );
_LIT8( KFailureMessage, "FailureMessage" );
const TInt KFailureLineNumber = 123;
_LIT8( KFailedTestFileName, "UT_Failure.cpp" );

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
UT_CSymbianUnitTestFailure* UT_CSymbianUnitTestFailure::NewL()
    {
    UT_CSymbianUnitTestFailure* self = UT_CSymbianUnitTestFailure::NewLC(); 
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
UT_CSymbianUnitTestFailure* UT_CSymbianUnitTestFailure::NewLC()
    {
    UT_CSymbianUnitTestFailure* self = 
        new( ELeave )UT_CSymbianUnitTestFailure();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
UT_CSymbianUnitTestFailure::UT_CSymbianUnitTestFailure()
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void UT_CSymbianUnitTestFailure::ConstructL()
    {
    BASE_CONSTRUCT
    
    DEFINE_TEST_CLASS( UT_CSymbianUnitTestFailure )
    
    ADD_SUT( UT_GettersL )
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
UT_CSymbianUnitTestFailure::~UT_CSymbianUnitTestFailure()
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void UT_CSymbianUnitTestFailure::SetupL()
    {
    iUnitTestFailure = 
        CSymbianUnitTestFailure::NewL(
            KFailedTestName,
            KFailureMessage,
            KFailureLineNumber,
            KFailedTestFileName );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void UT_CSymbianUnitTestFailure::Teardown()
    {
    delete iUnitTestFailure;
    iUnitTestFailure = NULL;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void UT_CSymbianUnitTestFailure::UT_GettersL()
    {
    SUT_ASSERT_EQUALS( 
        KFailedTestName8, iUnitTestFailure->TestName() )
    SUT_ASSERT_EQUALS( 
        KFailureMessage, iUnitTestFailure->FailureMessage() )
    SUT_ASSERT_EQUALS( 
        KFailureLineNumber, iUnitTestFailure->LineNumber() )
    SUT_ASSERT_EQUALS( 
        KFailedTestFileName, iUnitTestFailure->FileName() )
    }
