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

#include "symbianunittestresult.h"
#include "symbianunittestfailure.h"
#include "sutlogger.h"


_LIT8( KSymbianUnitTestLeaveCodeMessageFormat, "Leave code: %d" );
_LIT8( KSymbianUnitTestMemoryLeakPhrase, "Leaked %d bytes of memory" );
_LIT8( KSymbianUnitTestResourceLeakPhrase, "Leaked %d handles" );
_LIT8( KSymbianUnitTestRequestLeakPhrase, "Leaked %d requests" );
_LIT8( KSymbianUnitTestPanicPhrase, "Panic: " );
_LIT8( KSymbianUnitTestPanicReasonFormat, ", reason: %d" );
_LIT8( KSymbianUnitTestAllocFailureRateFormat, ", alloc failure rate: %d" );
_LIT8( KSymbianUnitTestSetupFailed, "Leave from the setup function: %d" );
_LIT8( KSymbianUnitTestErrorFormat, "Test Failed File[%S], Line[%d], Msg[%S]");
_LIT8( KSymbianUnitTestFailed, "Test Failed Reason[%S]");
_LIT( KSymbianUnitTestPass, "pass" );
_LIT( KSymbianUnitTestFail, "fail" );
_LIT8( KSymbianUnitTestTimedOut, "Timed out: %d s" );

const TInt KMax64BitUintLengthAsDescriptor = 20;
const TInt KMaxLength(1024);

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CSymbianUnitTestResult* CSymbianUnitTestResult::NewLC()
    {
    CSymbianUnitTestResult* self = new( ELeave )CSymbianUnitTestResult;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CSymbianUnitTestResult* CSymbianUnitTestResult::NewL()
    {
    CSymbianUnitTestResult* self = CSymbianUnitTestResult::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CSymbianUnitTestResult::CSymbianUnitTestResult() 
    : iOriginalHeap( User::Heap() )
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CSymbianUnitTestResult::ConstructL()
    {
    iCurrentTestName = KNullDesC().AllocL();
    iTestCaseNames = new (ELeave)CDesCArrayFlat(10);
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CSymbianUnitTestResult::~CSymbianUnitTestResult()
    {
    delete iCurrentTestName;
    delete iTestCaseNames;
    iFailures.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TInt CSymbianUnitTestResult::TestCount() const
    {
    return iTestCount;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TInt CSymbianUnitTestResult::PassedTestCount() const
    {
    return ( iTestCount - iFailures.Count() );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
RPointerArray< CSymbianUnitTestFailure >& CSymbianUnitTestResult::Failures()
    {
    return iFailures; 
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CSymbianUnitTestResult::StartTestL( const TDesC& aTestName )
    {
    SUT_LOG_FORMAT(_L("StartCase[%S]"), &aTestName);
    HBufC* tmp = aTestName.AllocL();
    delete iCurrentTestName;
    iCurrentTestName = tmp;
    iTestCount++;
    iCurrentResult = ETrue; 
    iTime.UniversalTime();
    }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CSymbianUnitTestResult::EndTestL()
    {
    TTime now;
    now.UniversalTime();
    TTimeIntervalMicroSeconds microSeconds = now.MicroSecondsFrom(iTime);
    TReal64 ms= microSeconds.Int64();
    //log the test time in milliseconds
    SUT_LOG_FORMAT2(_L("EndCase Result[%S] Time[%d]ms"), 
        iCurrentResult?&KSymbianUnitTestPass:&KSymbianUnitTestFail,
	microSeconds.Int64()/1000);
    if (iCurrentResult)
        {
        //only add passed case to the testcasenames
        iTestCaseNames->AppendL(*iCurrentTestName);
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TBool CSymbianUnitTestResult::CurrentResult()
    {
    return iCurrentResult;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CSymbianUnitTestResult::AddSetupErrorL( TInt aError )
    {
    const TInt KLength = 
        KSymbianUnitTestSetupFailed().Length() + 
        KMax64BitUintLengthAsDescriptor;    
    HBufC8* errorAsText = HBufC8::NewLC( KLength );
    errorAsText->Des().Format( KSymbianUnitTestSetupFailed, aError );
    User::LeaveIfError( AddFailure( *errorAsText, 0 ) );
    CleanupStack::PopAndDestroy( errorAsText );    
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CSymbianUnitTestResult::AddLeaveFromTestL(
    TInt aLeaveCode,
    TInt aAllocFailureRate )
    {
    const TInt KLength = 
        KSymbianUnitTestLeaveCodeMessageFormat().Length() + 
        KMax64BitUintLengthAsDescriptor;    
    HBufC8* failureMsg = HBufC8::NewLC( KLength );
    failureMsg->Des().Format( 
        KSymbianUnitTestLeaveCodeMessageFormat, aLeaveCode );
    User::LeaveIfError( AddFailure( *failureMsg, aAllocFailureRate ) );
    CleanupStack::PopAndDestroy( failureMsg );
    }
    
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CSymbianUnitTestResult::AddMemoryLeakInfoL(
    TInt aLeakedMemory,
    TInt aAllocFailureRate )
    {
    const TInt KLength = 
        KSymbianUnitTestMemoryLeakPhrase().Length() + 
        KMax64BitUintLengthAsDescriptor;    
    HBufC8* failureMsg = HBufC8::NewLC( KLength );
    failureMsg->Des().Format( 
        KSymbianUnitTestMemoryLeakPhrase, aLeakedMemory );
    User::LeaveIfError( AddFailure( *failureMsg, aAllocFailureRate ) );
    CleanupStack::PopAndDestroy( failureMsg );  
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CSymbianUnitTestResult::AddResourceLeakInfoL(
    TInt aLeakedResource,
    TInt aAllocFailureRate )
    {
    const TInt KLength = 
        KSymbianUnitTestResourceLeakPhrase().Length() + 
        KMax64BitUintLengthAsDescriptor;    
    HBufC8* failureMsg = HBufC8::NewLC( KLength );
    failureMsg->Des().Format( 
        KSymbianUnitTestResourceLeakPhrase, aLeakedResource );
    User::LeaveIfError( AddFailure( *failureMsg, aAllocFailureRate ) );
    CleanupStack::PopAndDestroy( failureMsg );  
    }
    
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CSymbianUnitTestResult::AddRequestLeakInfoL(
    TInt aLeakedRequest,
    TInt aAllocFailureRate )
    {
    const TInt KLength = 
        KSymbianUnitTestRequestLeakPhrase().Length() + 
        KMax64BitUintLengthAsDescriptor;    
    HBufC8* failureMsg = HBufC8::NewLC( KLength );
    failureMsg->Des().Format( 
        KSymbianUnitTestRequestLeakPhrase, aLeakedRequest );
    User::LeaveIfError( AddFailure( *failureMsg, aAllocFailureRate ) );
    CleanupStack::PopAndDestroy( failureMsg );  
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CSymbianUnitTestResult::AddPanicInfoL( 
    const TExitCategoryName& aPanicCategory,
    TInt aPanicReason,
    TInt aAllocFailureRate )
    {
    const TInt KLength = 
        KSymbianUnitTestPanicPhrase().Length() + aPanicCategory.Length() + 
        KSymbianUnitTestPanicReasonFormat().Length() + 
        KMax64BitUintLengthAsDescriptor;
    HBufC8* panicAsText = HBufC8::NewLC( KLength );
    TPtr8 panicAsTextPtr( panicAsText->Des() );
    panicAsTextPtr.Append( KSymbianUnitTestPanicPhrase );
    panicAsTextPtr.Append( aPanicCategory );
    panicAsTextPtr.AppendFormat( 
        KSymbianUnitTestPanicReasonFormat, aPanicReason );
    User::LeaveIfError( AddFailure( panicAsTextPtr, aAllocFailureRate ) );
    CleanupStack::PopAndDestroy( panicAsText );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TInt CSymbianUnitTestResult::AddAssertFailure(
    const TDesC8& aFailureMessage,
    TInt aLineNumber,
    const TDesC8& aFileName )
    {
    RHeap* currentHeap = User::SwitchHeap( &iOriginalHeap );
    TRAPD( err, AddAssertFailureL( aFailureMessage, aLineNumber, aFileName ) );
    User::SwitchHeap( currentHeap );
    return err;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CSymbianUnitTestResult::AddAssertFailureL(
    const TDesC8& aFailureMessage,
    TInt aLineNumber,
    const TDesC8& aFileName )
    {
    CSymbianUnitTestFailure* failure = 
        CSymbianUnitTestFailure::NewLC( 
            *iCurrentTestName, aFailureMessage, aLineNumber, aFileName );
    TBuf8<KMaxLength> strLine;
    strLine.Format( KSymbianUnitTestErrorFormat, &aFileName, aLineNumber, &aFailureMessage );
    while(strLine.Length() > 120) 
        {
        TBuf8<KMaxLength> line = strLine.MidTPtr( 0, 120 );
        line.Append( '-' );
        SUT_LOG_FORMAT(_L8("%S"), &line );
        line = strLine.Mid( 120 );
        strLine = line;
        }
    SUT_LOG_FORMAT(_L8("%S"), &strLine );
    iCurrentResult = EFalse;
    iFailures.AppendL( failure );
    CleanupStack::Pop( failure );
    }	

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CSymbianUnitTestResult::AddTimeOutErrorL( TInt aTimeOut )
    {
    const TInt KLength = 
        KSymbianUnitTestTimedOut().Length() + 
        KMax64BitUintLengthAsDescriptor;    
    HBufC8* errorAsText = HBufC8::NewLC( KLength );
    errorAsText->Des().Format( KSymbianUnitTestTimedOut, aTimeOut );
    User::LeaveIfError( AddFailure( *errorAsText, 0 ) );
    CleanupStack::PopAndDestroy( errorAsText );    
    }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TInt CSymbianUnitTestResult::AddFailure( 
    const TDesC8& aFailureText,
    TInt aAllocFailureRate )
    {
    RHeap* currentHeap = User::SwitchHeap( &iOriginalHeap );
    TRAPD( err, AddFailureL( aFailureText, aAllocFailureRate ) )
    User::SwitchHeap( currentHeap );
    return err;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CSymbianUnitTestResult::AddFailureL(
    const TDesC8& aFailureText,
    TInt aAllocFailureRate )
    {
    CSymbianUnitTestFailure* failure = NULL;
    if ( aAllocFailureRate > 0 )
        {
        const TInt KLength = 
            aFailureText.Length() + 
            KSymbianUnitTestAllocFailureRateFormat().Length() + 
            KMax64BitUintLengthAsDescriptor;
        HBufC8* failureMsg = HBufC8::NewLC( KLength );
        failureMsg->Des() = aFailureText;
        failureMsg->Des().AppendFormat(
            KSymbianUnitTestAllocFailureRateFormat, aAllocFailureRate );
        failure = CSymbianUnitTestFailure::NewL( 
            *iCurrentTestName, *failureMsg, KErrNotFound, KNullDesC8 );
        CleanupStack::PopAndDestroy( failureMsg );
        CleanupStack::PushL( failure );
        }
    else
        {
        failure = CSymbianUnitTestFailure::NewLC( 
            *iCurrentTestName, aFailureText, KErrNotFound, KNullDesC8 );
        }
        
    TBuf8<KMaxLength> strLine;
    strLine.Format( KSymbianUnitTestFailed, &aFailureText );
    while(strLine.Length() > 120) 
        {
        TBuf8<KMaxLength> line = strLine.MidTPtr( 0, 120 );
        line.Append( '-' );
        SUT_LOG_FORMAT(_L8("%S"), &line );
        line = strLine.Mid( 120 );
        strLine = line;
        }
    SUT_LOG_FORMAT(_L8("%S"), &strLine );
    //SUT_LOG_FORMAT(KSymbianUnitTestFailed, &aFailureText);
    iCurrentResult = EFalse;
    iFailures.AppendL( failure );
    CleanupStack::Pop( failure ); 
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
const CDesCArray& CSymbianUnitTestResult::TestCaseNames() const
    {
    return *iTestCaseNames;
    }

