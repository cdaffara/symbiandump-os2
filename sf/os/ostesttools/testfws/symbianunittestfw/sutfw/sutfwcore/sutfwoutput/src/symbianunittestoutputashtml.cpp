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
#include "symbianunittestoutputashtml.h"
#include <utf.h>

// CONSTANTS
_LIT8( KHtmlHeader, "<html><head><title>Unit test results</title></head>\n" );
_LIT8( KHtmlFooter, "</html>\n" );
_LIT8( KHtmlBodyStartTag, "<body bgcolor=\"#DDDDDD\" text=\"black\">\n" );
_LIT8( KHtmlBodyEndTag, "</body>\n");
_LIT8( KHtmlMainHeaderStartTag, "<h2> %d unit tests executed " );
_LIT8( KHtmlMainHeaderEndTag, "</h2>\n" );
_LIT8( KHtmlPassedTests, 
       "<h3><font color=\"green\">Passed tests: %d</b></font></h3>\n" );
_LIT8( KHtmlFailedTests, 
       "<h3><font color=\"red\">Failed tests: %d</font></h3>\n" );
_LIT8( KHtmlParagraphBreak, "<p></p>\n" );
_LIT8( KHtmlTableStartTag, "<table border=\"0\">\n" );
_LIT8( KHtmlTableEndTag, "</table>\n" );
_LIT8( KHtmlTRStartTag, "<tr>\n" );
_LIT8( KHtmlTREndTag, "</tr>\n" );
_LIT8( KHtmlTitleColumnStart, "<td width=\"100\" bgcolor=\"#888888\"><b>" );
_LIT8( KHtmlTitleColumnEnd,   "</b></td>\n" );
_LIT8( KHtmlTestNameTitle,   "Test name" );
_LIT8( KHtmlFileNameTitle,   "File name" );
_LIT8( KHtmlLineNumberTitle, "Line number" );
_LIT8( KHtmlReasonTitle,     "Reason" );
_LIT8( KHtmlValueColumnStart, "<td bgcolor=\"#BBBBBB\">" );
_LIT8( KHtmlValueColumnEnd,   "</td>\n" );


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CSymbianUnitTestOutputAsHtml* CSymbianUnitTestOutputAsHtml::NewL(
    const TDesC& aFileName )
    {
    CSymbianUnitTestOutputAsHtml* self = 
        new( ELeave )CSymbianUnitTestOutputAsHtml;
    CleanupStack::PushL( self );
    self->ConstructL( aFileName );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CSymbianUnitTestOutputAsHtml::CSymbianUnitTestOutputAsHtml()
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CSymbianUnitTestOutputAsHtml::~CSymbianUnitTestOutputAsHtml()
    {
    }

// -----------------------------------------------------------------------------
// From CSymbianUnitTestOutputFormatter
// -----------------------------------------------------------------------------
//
const TDesC& CSymbianUnitTestOutputAsHtml::FileExtension() const
    {
    return KHtmlOutput;
    }

// -----------------------------------------------------------------------------
// From CSymbianUnitTestOutputFormatter
// -----------------------------------------------------------------------------
//
void CSymbianUnitTestOutputAsHtml::PrintHeaderL( 
    CSymbianUnitTestResult& aResult )
    {
    iOutputWriter->WriteL( KHtmlHeader );
    iOutputWriter->WriteL( KHtmlBodyStartTag );
    iOutputWriter->WriteL( KHtmlMainHeaderStartTag, aResult.TestCount() );
    iOutputWriter->WriteDateAndTimeL();
    iOutputWriter->WriteL( KHtmlMainHeaderEndTag );
    }

// -----------------------------------------------------------------------------
// From CSymbianUnitTestOutputFormatter
// -----------------------------------------------------------------------------
//
void CSymbianUnitTestOutputAsHtml::PrintPassedTestsL( 
    CSymbianUnitTestResult& aResult ) 
    {
    iOutputWriter->WriteL( KHtmlPassedTests, aResult.PassedTestCount() );
    iOutputWriter->WriteL( KHtmlTableStartTag );
    const CDesCArray& testCaseNames = aResult.TestCaseNames();
    for ( TInt i=0; i < testCaseNames.Count(); i++ )
        {
        HBufC8* testName = CnvUtfConverter::ConvertFromUnicodeToUtf8L( testCaseNames[i] );
        PrintTableRowL( KHtmlTestNameTitle, *testName );
        delete testName;
        }
    iOutputWriter->WriteL( KHtmlTableEndTag );
    }

// -----------------------------------------------------------------------------
// From CSymbianUnitTestOutputFormatter
// -----------------------------------------------------------------------------
//
void CSymbianUnitTestOutputAsHtml::PrintFailedTestsL( 
    CSymbianUnitTestResult& aResult ) 
    {
    iOutputWriter->WriteL( KHtmlFailedTests, aResult.Failures().Count() );
    for ( TInt i=0; i < aResult.Failures().Count(); i++ )
        {
        CSymbianUnitTestFailure* failure = aResult.Failures()[ i ];
        iOutputWriter->WriteL( KHtmlTableStartTag );
        PrintTableRowL( KHtmlTestNameTitle, failure->TestName() );
        PrintTableRowL( KHtmlReasonTitle, failure->FailureMessage() );        
        if ( failure->LineNumber() >= 0 )
            {
            TBuf8< 20 > value;
            value.AppendNum( failure->LineNumber() );
            PrintTableRowL( KHtmlLineNumberTitle, value );
            }
        if ( failure->FileName().Length() > 0 )
            {
            PrintTableRowL( KHtmlFileNameTitle, failure->FileName() );
            }
        iOutputWriter->WriteL( KHtmlTableEndTag );
        iOutputWriter->WriteL( KHtmlParagraphBreak );
        }
    }

// -----------------------------------------------------------------------------
// From CSymbianUnitTestOutputFormatter
// -----------------------------------------------------------------------------
//
void CSymbianUnitTestOutputAsHtml::PrintFooterL()
    {
    iOutputWriter->WriteL( KHtmlBodyEndTag );
    iOutputWriter->WriteL( KHtmlFooter );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CSymbianUnitTestOutputAsHtml::PrintTableRowL(
    const TDesC8& aTitle, 
    const TDesC8& aValue )
    {
    iOutputWriter->WriteL( KHtmlTRStartTag );
    iOutputWriter->WriteL( KHtmlTitleColumnStart );
    iOutputWriter->WriteL( aTitle );
    iOutputWriter->WriteL( KHtmlTitleColumnEnd );
    iOutputWriter->WriteL( KHtmlValueColumnStart );
    iOutputWriter->WriteL( aValue );
    iOutputWriter->WriteL( KHtmlValueColumnEnd );
    iOutputWriter->WriteL( KHtmlTREndTag );
    }
