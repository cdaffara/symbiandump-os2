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

#include <e32debug.h>

#include "symbianunittestconsoleui.h"
#include "symbianunittestcommandlineparser.h"
#include "symbianunittestrunner.h"
#include "symbianunittestversion.h"

_LIT( KTitleTxt, "\n==== SymbianUnit v%d.%d.%d ====\n\n" );
_LIT( KAllocOnTxt, "  Alloc: ON\n\n" );
_LIT( KAllocOffTxt, "  Alloc: OFF\n\n" );
_LIT( KExecutingTestsTxt, "  Executing...\n" );
_LIT( KTestRunnerFailedTxt, "  Test run failed!\n  Reason: %d\n" );
_LIT( KNoTestsFoundTxt, "  No tests found!\n" );
_LIT( KTestsExecutedTxt, "  Executed: %d / %d\n" );
_LIT( KPassedTestsTxt, "  Passed: %d\n" );
//_LIT( KFailedTestsTxt, "  Failed: %d\n" );
_LIT( KFailedTestsTxt2, "  Failed: %S\n" );
_LIT( KWindowName, "SymbianUnit" );
_LIT( KSymbianUnitTestPanic, "SymbianUnit creation" );
_LIT( KPressAnyKeyTxt, "\n==[ press any key ]==\n    " );

_LIT( KHelpTxt, 
"Allowed arguments:\n\n\
-tests|t=<dll,dll,...>\n\
-cases|c=<case,case,...>\n\
-alloc|a\n\
-background|b\n\
-help|h\n\
-output|o=<html|xml|txt>\n\
-timeout|to\n\
-noprompt\n" );


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
GLDEF_C TInt E32Main() 
    {
    //__UHEAP_MARK;
    CTrapCleanup* cleanup = CTrapCleanup::New();
    TRAPD( err, MainL() );
    __ASSERT_ALWAYS( 
        err == KErrNone, User::Panic( KSymbianUnitTestPanic, err ) );
    delete cleanup;
    //__UHEAP_MARKEND;
//    User::Heap().Reset();
    return err;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
LOCAL_C void MainL() 
    {
    // install an active scheduler
    CActiveScheduler* scheduler = new( ELeave )CActiveScheduler;
    CActiveScheduler::Install( scheduler );
    CleanupStack::PushL( scheduler );


    CSymbianUnitTestConsoleUi* main = NULL;

    TRAPD( err, main = CSymbianUnitTestConsoleUi::NewL() );
    if ( err == KErrNone ) 
        {
        CActiveScheduler::Start();
        }
    
    CleanupStack::Pop( scheduler );
    delete main;
    delete scheduler;
    // Do not delete console. It will check for memory leaks.
    // This is not what is wanted if running tests without 
    // memory leak detection.
    //delete console;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CSymbianUnitTestConsoleUi* CSymbianUnitTestConsoleUi::NewLC()
    {
    CSymbianUnitTestConsoleUi* self = 
        new( ELeave )CSymbianUnitTestConsoleUi();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CSymbianUnitTestConsoleUi* CSymbianUnitTestConsoleUi::NewL()
    {
    CSymbianUnitTestConsoleUi* self = 
        CSymbianUnitTestConsoleUi::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CSymbianUnitTestConsoleUi::CSymbianUnitTestConsoleUi()
 : CActive( EPriorityStandard )
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CSymbianUnitTestConsoleUi::ConstructL()
    {
    User::SetJustInTime( EFalse ); // Do not stop on test case panics
    iCommandLineParser = CSymbianUnitTestCommandLineParser::NewL();
    iTestRunner = CSymbianUnitTestRunner::NewL( *this );
    if( !iCommandLineParser->Background() ) 
        {
        TSize size( KConsFullScreen, KConsFullScreen );
        iConsole = Console::NewL( KWindowName, size );
        RThread().SetPriority( EPriorityAbsoluteForeground );    
        }
    else 
        {
        RThread().SetPriority( EPriorityAbsoluteBackground );    
        }

    CActiveScheduler::Add( this );
    // Set ready to run immediately
    TRequestStatus *status = &iStatus;
    User::RequestComplete( status, KErrNone );
    SetActive ();
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CSymbianUnitTestConsoleUi::~CSymbianUnitTestConsoleUi()
    {
    Cancel();
    delete iTestRunner;
    delete iCommandLineParser;
    delete iConsole;
    }

// -----------------------------------------------------------------------------
// From CActive
// -----------------------------------------------------------------------------
//
void CSymbianUnitTestConsoleUi::RunL()
    {
    if (iCommandLineParser->Background()) 
        {
        //run in background
        TRAPD( err, 
        iTestRunner->ExecuteTestsL( 
            iCommandLineParser->TestDllNames(),
            iCommandLineParser->MemoryAllocationFailureSimulation(),
            iCommandLineParser->OutputFileName(),
            iCommandLineParser->OutputFormat(),
            iCommandLineParser->TestCaseNames(),
            iCommandLineParser->Timeout()) )
	if ( err != KErrNone)
	    {
		    RDebug::Print( KTestRunnerFailedTxt, err );
	    }
        }
    else 
        {
        iConsole->Printf( KTitleTxt, SUT_MAJOR_VERSION, SUT_MINOR_VERSION, SUT_BUILD_VERSION);
        if ( iCommandLineParser->ShowHelp() )
            {
            InfoMsg( KHelpTxt );
            }
        else
           {
            PrintAllocFailureSimulationText();
            InfoMsg( KExecutingTestsTxt );
            TRAPD(err,
                 iTestCount = iTestRunner->TestCountL(
                         iCommandLineParser->TestDllNames(),
                         iCommandLineParser->TestCaseNames()));
            TRAP( err, 
                iTestRunner->ExecuteTestsL( 
                    iCommandLineParser->TestDllNames(),
                    iCommandLineParser->MemoryAllocationFailureSimulation(),
                    iCommandLineParser->OutputFileName(),
                    iCommandLineParser->OutputFormat(),
    	        iCommandLineParser->TestCaseNames(),
    		iCommandLineParser->Timeout()) )
            if ( err != KErrNone )
                {
                    iConsole->Printf( KTestRunnerFailedTxt, err );
                }
            else 
                {
                if ( iExecutedTestCount == 0 )
                    {
                    InfoMsg( KNoTestsFoundTxt );
                    }
                }
            }
        if ( iCommandLineParser->PromptUser() )
            {
            InfoMsg( KPressAnyKeyTxt );
            iConsole->Getch(); // get and ignore character        
            }
        }
    CActiveScheduler::Stop();
    }

// -----------------------------------------------------------------------------
// From CActive
// -----------------------------------------------------------------------------
//
void CSymbianUnitTestConsoleUi::DoCancel()
    {
    }

// -----------------------------------------------------------------------------
// From MSymbianUnitTestUiCallBack
// -----------------------------------------------------------------------------
//
void CSymbianUnitTestConsoleUi::InfoMsg( const TDesC& aMessage )
    {
    iConsole->Printf( aMessage );
    }

// -----------------------------------------------------------------------------
// From MSymbianUnitTestUiCallBack
// -----------------------------------------------------------------------------
//
void CSymbianUnitTestConsoleUi::InfoMsg( 
    const TDesC& aFormat, 
    const TDesC& aMessage )
    {
    iConsole->Printf( aFormat, &aMessage );
    }

// -----------------------------------------------------------------------------
// From MSymbianUnitTestUiCallBack
// -----------------------------------------------------------------------------
//
void CSymbianUnitTestConsoleUi::TestPass(const TDesC& /*aTestCaseName*/)
    {
    IncrementExecutedTestsCount();
    }

void CSymbianUnitTestConsoleUi::TestFailed(const TDesC& /*aTestCaseName*/, const TDesC8& /*aErrMsg*/)
    {
    IncrementExecutedTestsCount();
    }


void CSymbianUnitTestConsoleUi::IncrementExecutedTestsCount()
    {
    if (iCommandLineParser->Background()) 
        {
        return;
        }
    TPoint pos = iConsole->CursorPos();
    pos.iY -= 1;
    if ( iExecutedTestCount > 0 )
        {
        pos.iY -= 2;
        }
    iConsole->SetCursorPosAbs( pos );
    
    iExecutedTestCount++;
    iConsole->Printf( 
        KTestsExecutedTxt, iExecutedTestCount, iTestCount);
    
    TInt passedTestCount = iExecutedTestCount - iTestRunner->FailedTestCount();
        iConsole->Printf( KPassedTestsTxt, passedTestCount );
    
    TInt count = iTestRunner->FailedTestCount();
    TBuf<10> txtCount;
    txtCount.AppendFormat( _L("%d"), count );
    InfoMsg( KFailedTestsTxt2, txtCount );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CSymbianUnitTestConsoleUi::PrintAllocFailureSimulationText()
    {
    if ( iCommandLineParser->MemoryAllocationFailureSimulation() )
        {
        InfoMsg( KAllocOnTxt );
        }
    else
        {
        InfoMsg( KAllocOffTxt );
        }
    }
