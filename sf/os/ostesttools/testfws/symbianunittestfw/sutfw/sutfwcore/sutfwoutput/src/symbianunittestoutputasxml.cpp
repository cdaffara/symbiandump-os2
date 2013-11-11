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
#include "symbianunittestoutputasxml.h"


_LIT8( KXmlResultOpenTag, "<SYMBIANUNITTEST_RESULT testcount=\"%d\">\r\n" );
_LIT8( KXmlResultCloseTag, "</SYMBIANUNITTEST_RESULT>\r\n" );
_LIT8( KXmlPassedTestsOpenTag, "\t<SYMBIANUNITTEST_PASSED count=\"%d\">\r\n" );
_LIT8( KXmlPassedTestsCloseTag, "\t</SYMBIANUNITTEST_PASSED>\r\n" );
_LIT8( KXmlFailedTestsOpenTag, "\t<SYMBIANUNITTEST_FAILED count=\"%d\">\r\n" );
_LIT8( KXmlFailedTestsCloseTag, "\t</SYMBIANUNITTEST_FAILED>\r\n" );
_LIT8( KXmlFailureOpenTag, "\t\t<SYMBIANUNITTEST_FAILURE>\r\n" );
_LIT8( KXmlFailureCloseTag, "\t\t</SYMBIANUNITTEST_FAILURE>\r\n" );
_LIT8( KXmlTestNameOpenTag, "\t\t\t<SYMBIANUNITTEST_NAME>\r\n" );
_LIT8( KXmlTestNameCloseTag, "\r\n\t\t\t</SYMBIANUNITTEST_NAME>\r\n" );
_LIT8( KXmlFileNameOpenTag, "\t\t\t<SYMBIANUNITTEST_FILE>\r\n" );
_LIT8( KXmlFileNameCloseTag, "\r\n\t\t\t</SYMBIANUNITTEST_FILE>\r\n" );
_LIT8( KXmlLineNumberOpenTag, "\t\t\t<SYMBIANUNITTEST_LINE>%d\r\n" );
_LIT8( KXmlLineNumberCloseTag, "\r\n\t\t\t</SYMBIANUNITTEST_LINE>\r\n" );
_LIT8( KXmlReasonOpenTag, "\t\t\t<SYMBIANUNITTEST_REASON>\r\n" );
_LIT8( KXmlReasonCloseTag, "\r\n\t\t\t</SYMBIANUNITTEST_REASON>\r\n" );


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CSymbianUnitTestOutputAsXml* CSymbianUnitTestOutputAsXml::NewL(
    const TDesC& aFileName )
    {
    CSymbianUnitTestOutputAsXml* self = 
        new( ELeave )CSymbianUnitTestOutputAsXml;
    CleanupStack::PushL( self );
    self->ConstructL( aFileName );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CSymbianUnitTestOutputAsXml::CSymbianUnitTestOutputAsXml()
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CSymbianUnitTestOutputAsXml::~CSymbianUnitTestOutputAsXml()
    {
    }

// -----------------------------------------------------------------------------
// From CSymbianUnitTestOutputFormatter
// -----------------------------------------------------------------------------
//
const TDesC& CSymbianUnitTestOutputAsXml::FileExtension() const
    {
    return KXmlOutput;
    }

// -----------------------------------------------------------------------------
// From CSymbianUnitTestOutputFormatter
// -----------------------------------------------------------------------------
//
void CSymbianUnitTestOutputAsXml::PrintHeaderL( 
    CSymbianUnitTestResult& aResult )
    {
    iOutputWriter->WriteL( KXmlResultOpenTag, aResult.TestCount() );
    }

// -----------------------------------------------------------------------------
// From CSymbianUnitTestOutputFormatter
// -----------------------------------------------------------------------------
//
void CSymbianUnitTestOutputAsXml::PrintPassedTestsL( 
    CSymbianUnitTestResult& aResult ) 
    {
    iOutputWriter->WriteL( KXmlPassedTestsOpenTag, aResult.PassedTestCount() );
    const CDesCArray& testCaseNames = aResult.TestCaseNames();
    for ( TInt i=0; i < testCaseNames.Count(); i++ )
        {
        iOutputWriter->WriteL( KXmlTestNameOpenTag );
        iOutputWriter->WriteL( testCaseNames[i]);
        iOutputWriter->WriteL( KXmlTestNameCloseTag );
        }
    iOutputWriter->WriteL( KXmlPassedTestsCloseTag );
    }

// -----------------------------------------------------------------------------
// From CSymbianUnitTestOutputFormatter
// -----------------------------------------------------------------------------
//
void CSymbianUnitTestOutputAsXml::PrintFailedTestsL( 
    CSymbianUnitTestResult& aResult ) 
    {
    iOutputWriter->WriteL( KXmlFailedTestsOpenTag, aResult.Failures().Count() );
    for ( TInt i=0; i < aResult.Failures().Count(); i++ )
        {
        iOutputWriter->WriteL( KXmlFailureOpenTag );
        PrintFailureDetailsL( *( aResult.Failures()[ i ] ) );
        iOutputWriter->WriteL( KXmlFailureCloseTag );
        }
    iOutputWriter->WriteL( KXmlFailedTestsCloseTag );
    }

// -----------------------------------------------------------------------------
// From CSymbianUnitTestOutputFormatter
// -----------------------------------------------------------------------------
//
void CSymbianUnitTestOutputAsXml::PrintFooterL()
    {
    iOutputWriter->WriteL( KXmlResultCloseTag );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CSymbianUnitTestOutputAsXml::PrintFailureDetailsL( 
    CSymbianUnitTestFailure& aTestFailure )
    {
    iOutputWriter->WriteL( KXmlTestNameOpenTag );
    iOutputWriter->WriteL( aTestFailure.TestName() );
    iOutputWriter->WriteL( KXmlTestNameCloseTag );
    if ( aTestFailure.FileName().Length() > 0 )
        {
        iOutputWriter->WriteL( KXmlFileNameOpenTag );
        iOutputWriter->WriteL( aTestFailure.FileName() );
        iOutputWriter->WriteL( KXmlFileNameCloseTag );
        }
    TInt lineNumber( aTestFailure.LineNumber() );
    if ( lineNumber >= 0 )
        {
        iOutputWriter->WriteL( KXmlLineNumberOpenTag, lineNumber );
        iOutputWriter->WriteL( KXmlLineNumberCloseTag );
        }
    iOutputWriter->WriteL( KXmlReasonOpenTag );
    iOutputWriter->WriteL( aTestFailure.FailureMessage() );
    iOutputWriter->WriteL( KXmlReasonCloseTag );
    }
