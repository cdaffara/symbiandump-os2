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

#include "symbianunittestcommandlineparser.h"
#include "symbianunittestdllnameparser.h"
#include "symbianunittestrunner.h"

_LIT( KHelpKey, "-help" );
_LIT( KHelpKeyShort, "-h" );
_LIT( KTestsKey, "-tests=" );
_LIT( KTestsKeyShort, "-t=" );
_LIT( KTestCasesKey, "-cases=" );
_LIT( KTestCasesKeyShort, "-c=" );
_LIT( KAllocKey, "-alloc" );
_LIT( KAllocKeyShort, "-a" );
_LIT( KBackgroundKey, "-background" );
_LIT( KBackgroundKeyShort, "-b" );
_LIT( KOutputKey, "-output=" );
_LIT( KOutputKeyShort, "-o=" );
_LIT( KNoPromptKey, "-noprompt" );
_LIT( KTimeoutKey, "-timeout=" );
_LIT( KTimeoutKeyShort, "-to=" );


_LIT( KOutputFileName, "SymbianUnitTestResults" );
_LIT( KDefaultOutputFormat, "html" );

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CSymbianUnitTestCommandLineParser* CSymbianUnitTestCommandLineParser::NewLC()
    {
    CSymbianUnitTestCommandLineParser* self = 
        new( ELeave )CSymbianUnitTestCommandLineParser();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CSymbianUnitTestCommandLineParser* CSymbianUnitTestCommandLineParser::NewL()
    {
    CSymbianUnitTestCommandLineParser* self = 
        CSymbianUnitTestCommandLineParser::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CSymbianUnitTestCommandLineParser::CSymbianUnitTestCommandLineParser()
	: iBackground( EFalse )
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CSymbianUnitTestCommandLineParser::ConstructL()
    {
    iArguments = CCommandLineArguments::NewL();   
    TPtrC dummy;
    iShowHelp =  FindArgument( KHelpKey, KHelpKeyShort, dummy );
    iAllocFailureSimulation = FindArgument( KAllocKey, KAllocKeyShort, dummy );
    iNoPrompt = FindArgument( KNoPromptKey, KNoPromptKey, dummy );
    iBackground = FindArgument( KBackgroundKey, KBackgroundKeyShort, dummy );
    SetOutputFormatL();
    SetTimeoutL();
    SetTestDllNamesL();
    SetTestCaseNamesL();
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CSymbianUnitTestCommandLineParser::~CSymbianUnitTestCommandLineParser()
    {
    delete iTestDllNames;
    delete iTestCaseNames;
    delete iOutputFormat;
    delete iArguments;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TBool CSymbianUnitTestCommandLineParser::ShowHelp() const
    {
    return iShowHelp;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
const MDesCArray& CSymbianUnitTestCommandLineParser::TestDllNames() const
    {
    return *iTestDllNames;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
const CDesCArray& CSymbianUnitTestCommandLineParser::TestCaseNames() const
    {
    return *iTestCaseNames;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TBool CSymbianUnitTestCommandLineParser::MemoryAllocationFailureSimulation() const
    {
    return iAllocFailureSimulation;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TBool CSymbianUnitTestCommandLineParser::Background() const
    {
    return iBackground;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
const TDesC& CSymbianUnitTestCommandLineParser::OutputFileName() const
    {
    return KOutputFileName;
    }
    
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
const TDesC& CSymbianUnitTestCommandLineParser::OutputFormat() const
    {
    return *iOutputFormat;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TInt CSymbianUnitTestCommandLineParser::Timeout() const
    {
    return iTimeout;
    }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TBool CSymbianUnitTestCommandLineParser::PromptUser() const
    {
    return !iNoPrompt;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CSymbianUnitTestCommandLineParser::SetOutputFormatL()
    {
    HBufC* outputFormat = NULL;
    TPtrC outputFormatPtr;
    TBool found = EFalse;
    found = FindArgument( KOutputKey, KOutputKeyShort, outputFormatPtr );
    if ( found )
        {
        outputFormat = outputFormatPtr.AllocL();
        }
    else
        {
        outputFormat = KDefaultOutputFormat().AllocL();
        }
    delete iOutputFormat;
    iOutputFormat = outputFormat;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CSymbianUnitTestCommandLineParser::SetTimeoutL()
    {
    iTimeout = KSymbianUnitTestDefaultTimeout; //use default
    TPtrC timeoutPtr;
    TBool hasTimeout = FindArgument( KTimeoutKey, KTimeoutKeyShort, timeoutPtr );
    if ( hasTimeout)
        {
        TLex timeoutLex(timeoutPtr);
		TInt timeout;
        TInt ret = timeoutLex.Val(timeout);
        hasTimeout = ( ret == KErrNone && timeout >= 0 );
		if ( hasTimeout ) 
	    	{
		    iTimeout = timeout;
		    }
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CSymbianUnitTestCommandLineParser::SetTestDllNamesL()
    {
    CDesCArray* testDllNames = new( ELeave )CDesCArrayFlat( 1 );
    CleanupStack::PushL( testDllNames );
    TPtrC testDllNamesPtr;
    TBool found = EFalse;
    found = FindArgument( KTestsKey, KTestsKeyShort, testDllNamesPtr );
    if ( found )
        {
        TSymbianUnitTestDllNameParser parser;
        parser.Parse( testDllNamesPtr );
        TPtrC testDllNamePtr;
        TBool hasNext = EFalse;
        hasNext = ( parser.GetNext( testDllNamePtr ) == KErrNone );
        while ( hasNext )
            {
            testDllNames->AppendL( testDllNamePtr );
            hasNext = ( parser.GetNext( testDllNamePtr ) == KErrNone );
            }
        }
    iShowHelp = ( testDllNames->Count() == 0 );
    CleanupStack::Pop( testDllNames );
    delete iTestDllNames;
    iTestDllNames = testDllNames;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CSymbianUnitTestCommandLineParser::SetTestCaseNamesL()
    {
    CDesCArray* testCaseNames = new( ELeave )CDesCArrayFlat( 1 );
    CleanupStack::PushL( testCaseNames );
    TPtrC testCaseNamesPtr;
    TBool found = EFalse;
    found = FindArgument( KTestCasesKey, KTestCasesKeyShort, testCaseNamesPtr );
    if ( found )
        {
	//reuse the testdll parser here for case names
        TSymbianUnitTestDllNameParser parser;
        parser.Parse( testCaseNamesPtr );
        TPtrC testCaseNamePtr;
        TBool hasNext = EFalse;
        hasNext = ( parser.GetNext( testCaseNamePtr ) == KErrNone );
        while ( hasNext )
            {
            testCaseNames->AppendL( testCaseNamePtr );
            hasNext = ( parser.GetNext( testCaseNamePtr ) == KErrNone );
            }
        }
    CleanupStack::Pop( testCaseNames );
    delete iTestCaseNames;
    iTestCaseNames = testCaseNames;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TBool CSymbianUnitTestCommandLineParser::FindArgument( 
    const TDesC& aKey,
    const TDesC& aShortKey,
    TPtrC& aValue )
    {
    TBool found( EFalse );
    for ( TInt i = 0; i < iArguments->Count(); i++ )
        {
        TPtrC arg( iArguments->Arg( i ) );
        if ( arg.FindF( aKey ) == 0 || arg.FindF( aShortKey ) == 0 )
            {
            found = ETrue;
            TInt equalsPos = arg.Locate( '=' );
            if ( equalsPos > 0 & equalsPos < arg.Length() - 1 ) 
                {
                aValue.Set( arg.Mid( equalsPos + 1 ) );
                }
            }
        }
    return found;
    }
