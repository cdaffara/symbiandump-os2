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

#include "ut_symbianunittestresult.h"
#include "symbianunittestresult.h"
#include "symbianunittestfailure.h"
#include <symbianunittestmacros.h>


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
UT_CSymbianUnitTestResult* UT_CSymbianUnitTestResult::NewL()
    {
    UT_CSymbianUnitTestResult* self = UT_CSymbianUnitTestResult::NewLC(); 
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
UT_CSymbianUnitTestResult* UT_CSymbianUnitTestResult::NewLC()
    {
    UT_CSymbianUnitTestResult* self = 
        new( ELeave )UT_CSymbianUnitTestResult();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
UT_CSymbianUnitTestResult::UT_CSymbianUnitTestResult()
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void UT_CSymbianUnitTestResult::ConstructL()
    {
    BASE_CONSTRUCT
    
    DEFINE_TEST_CLASS( UT_CSymbianUnitTestResult )
    
    ADD_SUT( UT_TestCountL )
    ADD_SUT( UT_StartTestL )
    ADD_SUT( UT_AddFailureL )
    ADD_SUT( UT_AddSetupErrorL )
    ADD_SUT( UT_AddLeaveFromTestL )
    ADD_SUT( UT_AddAssertFailureL )
    ADD_SUT( UT_AddMemoryLeakInfoL )
    ADD_SUT( UT_AddPanicInfoL )   
    ADD_SUT( UT_AddTimeOutErrorL )
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
UT_CSymbianUnitTestResult::~UT_CSymbianUnitTestResult()
    {  
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void UT_CSymbianUnitTestResult::SetupL()
    {
    iUnitTestResult = CSymbianUnitTestResult::NewL();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void UT_CSymbianUnitTestResult::Teardown()
    {
    delete iUnitTestResult;
    iUnitTestResult = NULL;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void UT_CSymbianUnitTestResult::UT_TestCountL()
    {
    // No tests
    SUT_ASSERT_EQUALS( 0, iUnitTestResult->TestCount() )
    
    // Increment the test count
    iUnitTestResult->iTestCount++;
    SUT_ASSERT_EQUALS( 1, iUnitTestResult->TestCount() )
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void UT_CSymbianUnitTestResult::UT_StartTestL()
    {
    SUT_ASSERT_EQUALS( KNullDesC, *iUnitTestResult->iCurrentTestName )
    SUT_ASSERT_EQUALS( 0, iUnitTestResult->TestCount() )    
    
    _LIT( KTestName, "FailedTest" );
    iUnitTestResult->StartTestL( KTestName );
    SUT_ASSERT_EQUALS( KTestName, *iUnitTestResult->iCurrentTestName )
    SUT_ASSERT_EQUALS( 1, iUnitTestResult->TestCount() )
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void UT_CSymbianUnitTestResult::UT_AddFailureL()
    {
    SUT_ASSERT_EQUALS( 0, iUnitTestResult->Failures().Count() )
    
    _LIT8( KFailureText, "FailureText" );
    
    // Memory allocation failure rate 0
    iUnitTestResult->AddFailureL( KFailureText, 0 );
    SUT_ASSERT_EQUALS( 1, iUnitTestResult->Failures().Count() )
    SUT_ASSERT_EQUALS( 
        KFailureText, iUnitTestResult->Failures()[ 0 ]->FailureMessage() )        
    SUT_ASSERT_EQUALS( 
        KErrNotFound, iUnitTestResult->Failures()[ 0 ]->LineNumber() )
    SUT_ASSERT_EQUALS( 
        KNullDesC8, iUnitTestResult->Failures()[ 0 ]->FileName() )        
    
    // Memory allocation failure rate > 0
    iUnitTestResult->AddFailureL( KFailureText, 1 );
    SUT_ASSERT_EQUALS( 2, iUnitTestResult->Failures().Count() )
    SUT_ASSERT( 
        iUnitTestResult->Failures()[ 1 ]->FailureMessage() != KFailureText )        
    SUT_ASSERT_EQUALS( 
        KErrNotFound, iUnitTestResult->Failures()[ 1 ]->LineNumber() )
    SUT_ASSERT_EQUALS( 
        KNullDesC8, iUnitTestResult->Failures()[ 1 ]->FileName() )
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void UT_CSymbianUnitTestResult::UT_AddSetupErrorL()
    {
    SUT_ASSERT_EQUALS( 0, iUnitTestResult->Failures().Count() )

    const TInt KErrorCode = KErrGeneral;
    iUnitTestResult->AddSetupErrorL( KErrorCode );
    SUT_ASSERT_EQUALS( 1, iUnitTestResult->Failures().Count() )
    SUT_ASSERT( 
        iUnitTestResult->Failures()[ 0 ]->FailureMessage().Length () > 0 )
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void UT_CSymbianUnitTestResult::UT_AddLeaveFromTestL()
    {
    SUT_ASSERT_EQUALS( 0, iUnitTestResult->Failures().Count() )

    iUnitTestResult->AddLeaveFromTestL( KErrGeneral, 0 );
    SUT_ASSERT_EQUALS( 1, iUnitTestResult->Failures().Count() )
    SUT_ASSERT( 
        iUnitTestResult->Failures()[ 0 ]->FailureMessage().Length () > 0 )        
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void UT_CSymbianUnitTestResult::UT_AddAssertFailureL()
    {
    SUT_ASSERT_EQUALS( 0, iUnitTestResult->Failures().Count() )
    
    _LIT8( KFailureMessage, "assert failure" );
    const TInt KLineNumber = 123;
    _LIT8( KFileName, "myfilename.cpp" );
    iUnitTestResult->AddAssertFailureL( 
        KFailureMessage, KLineNumber, KFileName );
    SUT_ASSERT_EQUALS( 1, iUnitTestResult->Failures().Count() )
    SUT_ASSERT_EQUALS( 
        KFailureMessage, iUnitTestResult->Failures()[ 0 ]->FailureMessage() )        
    SUT_ASSERT_EQUALS( 
        KLineNumber, iUnitTestResult->Failures()[ 0 ]->LineNumber() )
    SUT_ASSERT_EQUALS( 
        KFileName, iUnitTestResult->Failures()[ 0 ]->FileName() )    
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void UT_CSymbianUnitTestResult::UT_AddMemoryLeakInfoL()
    {
    SUT_ASSERT_EQUALS( 0, iUnitTestResult->Failures().Count() )

    iUnitTestResult->AddMemoryLeakInfoL( 1, 0 );
    SUT_ASSERT_EQUALS( 1, iUnitTestResult->Failures().Count() )
    SUT_ASSERT( 
        iUnitTestResult->Failures()[ 0 ]->FailureMessage().Length () > 0 )    
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void UT_CSymbianUnitTestResult::UT_AddPanicInfoL()
    {
    SUT_ASSERT_EQUALS( 0, iUnitTestResult->Failures().Count() )

    TExitCategoryName dummy;
    iUnitTestResult->AddPanicInfoL( dummy, KErrGeneral, 0 );
    SUT_ASSERT_EQUALS( 1, iUnitTestResult->Failures().Count() )
    SUT_ASSERT( 
        iUnitTestResult->Failures()[ 0 ]->FailureMessage().Length () > 0 )
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void UT_CSymbianUnitTestResult::UT_AddTimeOutErrorL()
    {
    SUT_ASSERT_EQUALS( 0, iUnitTestResult->Failures().Count() )

    const TInt KTimeOut = 10;
    iUnitTestResult->AddTimeOutErrorL( KTimeOut );
    SUT_ASSERT_EQUALS( 1, iUnitTestResult->Failures().Count() )
    SUT_ASSERT( 
        iUnitTestResult->Failures()[ 0 ]->FailureMessage().Length () > 0 )
    }

