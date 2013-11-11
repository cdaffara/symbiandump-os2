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
#include "symbianunittestfileoutputwriter.h"
#include "symbianunittestoutputastext.h"

_LIT8( KTxtExecutedTests, "\r\n %d unit tests executed " );
_LIT8( KTxtPassedTests, " Passed tests: %d\r\n\r\n" );
_LIT8( KTxtPassedTest,  "   Test: " );
_LIT8( KTxtFailedTests, " Failed tests: %d\r\n\r\n" );
_LIT8( KTxtFailureTest, "   Test: " );
_LIT8( KTxtFailureFile, "   File: " );
_LIT8( KTxtFailureLine, "   Line: %d\r\n" );
_LIT8( KTxtFailureReason, "   Reason: " );
_LIT8( KLineEnd, "\r\n" );

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CSymbianUnitTestOutputAsText* CSymbianUnitTestOutputAsText::NewL(
    const TDesC& aFileName )
    {
    CSymbianUnitTestOutputAsText* self = 
        new( ELeave )CSymbianUnitTestOutputAsText;
    CleanupStack::PushL( self );
    self->ConstructL( aFileName );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CSymbianUnitTestOutputAsText::CSymbianUnitTestOutputAsText()
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CSymbianUnitTestOutputAsText::~CSymbianUnitTestOutputAsText()
    {
    }

// -----------------------------------------------------------------------------
// From CSymbianUnitTestOutputFormatter
// -----------------------------------------------------------------------------
//
const TDesC& CSymbianUnitTestOutputAsText::FileExtension() const
    {
    return KTextOutput;
    }

// -----------------------------------------------------------------------------
// From CSymbianUnitTestOutputFormatter
// -----------------------------------------------------------------------------
//
void CSymbianUnitTestOutputAsText::PrintHeaderL( 
    CSymbianUnitTestResult& aResult )
    {
    iOutputWriter->WriteL( KTxtExecutedTests, aResult.TestCount() );
    iOutputWriter->WriteDateAndTimeL();
    iOutputWriter->WriteL( KLineEnd );
    iOutputWriter->WriteL( KLineEnd );
    }

// -----------------------------------------------------------------------------
// From CSymbianUnitTestOutputFormatter
// -----------------------------------------------------------------------------
//
void CSymbianUnitTestOutputAsText::PrintPassedTestsL( 
    CSymbianUnitTestResult& aResult )
    {
    iOutputWriter->WriteL( KTxtPassedTests, aResult.PassedTestCount() );
    const CDesCArray& testCaseNames = aResult.TestCaseNames();
    for ( TInt i=0; i < testCaseNames.Count(); i++ )
        {
        iOutputWriter->WriteL( KTxtPassedTest);
        iOutputWriter->WriteL( testCaseNames[i]);
        iOutputWriter->WriteL( KLineEnd );
        }
    }

// -----------------------------------------------------------------------------
// From CSymbianUnitTestOutputFormatter
// -----------------------------------------------------------------------------
//
void CSymbianUnitTestOutputAsText::PrintFailedTestsL( 
    CSymbianUnitTestResult& aResult ) 
    {
    iOutputWriter->WriteL( KTxtFailedTests, aResult.Failures().Count() );
    for ( TInt i=0; i < aResult.Failures().Count(); i++ )
        {
        PrintFailureDetailsL( *( aResult.Failures()[ i ] ) );
        }
    }

// -----------------------------------------------------------------------------
// From CSymbianUnitTestOutputFormatter
// -----------------------------------------------------------------------------
//
void CSymbianUnitTestOutputAsText::PrintFooterL()
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CSymbianUnitTestOutputAsText::PrintFailureDetailsL( 
    CSymbianUnitTestFailure& aTestFailure )
    {
    iOutputWriter->WriteL( KTxtFailureTest );
    iOutputWriter->WriteL( aTestFailure.TestName() );
    iOutputWriter->WriteL( KLineEnd );
    if ( aTestFailure.FileName().Length() > 0 )
        {
        iOutputWriter->WriteL( KTxtFailureFile );
        iOutputWriter->WriteL( aTestFailure.FileName() );
        iOutputWriter->WriteL( KLineEnd );
        }
    if ( aTestFailure.LineNumber() >= 0 )
        {
        iOutputWriter->WriteL( KTxtFailureLine, aTestFailure.LineNumber() );
        }
    iOutputWriter->WriteL( KTxtFailureReason );
    iOutputWriter->WriteL( aTestFailure.FailureMessage() );
    iOutputWriter->WriteL( KLineEnd );
    iOutputWriter->WriteL( KLineEnd );
    }
