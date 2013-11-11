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
#include "symbianunittestobserver.h"
#include "symbianunittestfailure.h"
#include "sutlogger.h"
#include <symbianunittest.h>
#include <e32math.h>
#include <utf.h>
#include <e32debug.h>

// Assertion failure message formats:
_LIT8( KIntsNotEqualFormat, "Asserted: expected=%d, actual=%d" );
_LIT8( KDesCsNotEqualFormat, "Asserted: expected='%S', actual='%S'" );
_LIT8( KAssertLeaveFormat1, "'%S' expected to leave: expected=%d, actual=%d" );
_LIT8( KAssertLeaveFormat2, "'%S' expected to leave but did not leave" );

const TInt KMaxSizeOfTwoIntsAsText = 80;
const TInt KErrSymbianUnitTestAssertionFailed = -99999999;
_LIT( KTestThreadName, "SymbianUnitTestThread" );
const TInt KTestThreadMaxHeapSize = 0x400000; // 4 MB
_LIT8( KDoubleColon8, "::" );
_LIT( KDoubleColon16, "::" );

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CSymbianUnitTest::ConstructL( const TDesC8& aName )
    {    
    TInt doubleColonPos( aName.FindF( KDoubleColon8 ) );
    TPtrC8 classNamePtr( aName );
    if ( doubleColonPos > 0 )
        {
        classNamePtr.Set( aName.Left( doubleColonPos ) );
        }
    
    iName = CnvUtfConverter::ConvertToUnicodeFromUtf8L( classNamePtr ); 
    
    SUT_LOG_FORMAT(_L("enter CSymbianUnitTest::ConstructL -- aName : [%S]"), iName );
    
    // unify the same name for armv5 & winscw. 
    // on winscw, the function name is abc, 
    // on armv5,  the function name is void abc. 
    // unify to abc. 
    TChar chSpace( ' ' );
    TChar chTab( '\t' );
    TChar chAsterisk( '*' );
    TChar chAnd( '&' );
    TBuf<100> strTmp( *iName ); 
    strTmp.Trim();
    TInt pos;
    
    // "char * & abc" => abc
    pos = strTmp.LocateReverse( chAnd );
    if( KErrNotFound != pos ) 
    	{
    	TPtrC ptr = strTmp.Right( strTmp.Length() - pos - 1 );
    	strTmp = ptr;
    	strTmp.Trim();
    	}
    // "void * abc" => abc
    pos = strTmp.LocateReverse( chAsterisk );
    if( KErrNotFound != pos ) 
    	{
    	TPtrC ptr = strTmp.Right( strTmp.Length() - pos - 1 );
    	strTmp = ptr;
    	strTmp.Trim();
    	}
    // "void abc"
    pos = strTmp.LocateReverse( chSpace );
    if( KErrNotFound != pos ) 
    	{
    	TPtrC ptr = strTmp.Right( strTmp.Length() - pos - 1 );
    	strTmp = ptr;
    	strTmp.Trim();
    	}
    // "void 	abc"
    pos = strTmp.LocateReverse( chTab );
    if( KErrNotFound != pos ) 
    	{
    	TPtrC ptr = strTmp.Right( strTmp.Length() - pos - 1 );
    	strTmp = ptr;
    	strTmp.Trim();
    	}
    	
    *iName = strTmp;
    
    SUT_LOG_FORMAT(_L("exit CSymbianUnitTest::ConstructL -- aName : [%S]"), iName );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C CSymbianUnitTest::CSymbianUnitTest()
 : iAllocFailureType( RHeap::ENone ),
   iAllocFailureRate( 0 )
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C CSymbianUnitTest::~CSymbianUnitTest() 
    { 
    delete iName;
    iTestCases.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// From MSymbianUnitTestInterface
// -----------------------------------------------------------------------------
//
EXPORT_C void CSymbianUnitTest::ExecuteL(
    MSymbianUnitTestObserver& aObserver,
    CSymbianUnitTestResult& aResult,
    MSymbianUnitTestInterface::TFailureSimulation aFailureSimulation,
    const CDesCArray& aTestCaseNames,
    TInt aTimeout )
    {
    if ( aFailureSimulation == EMemAllocFailureSimulation )
        {
        iAllocFailureType = RHeap::EDeterministic;
        }
    else
        {
        iAllocFailureType = RHeap::ENone;
        }      
    SUT_LOG_FORMAT(_L("start testing, total test cases[%d]"), iTestCases.Count());
    //print the test cases name in the log
    for ( TInt i = iTestCases.Count() -1; i >=0 ; i-- )
        {
        CSymbianUnitTestCase& testCase = *( iTestCases[ i ] );
	//check the specified test case list if any
	if ( aTestCaseNames.Count() > 0 )
	    {
	    TInt index=0;
	    TInt ret = aTestCaseNames.Find(testCase.Name(), index );
	    if (ret != 0)
	        {
		    //the case isn't in the specified test case names, skip it
            SUT_LOG_FORMAT(_L("skip test case[%S]"), &testCase.Name());
		    delete iTestCases[i];
            iTestCases.Remove(i);
            continue;
	        }
	    }
	SUT_LOG_FORMAT(_L("TestCase[%S]"), &testCase.Name());
        }

    for ( TInt i = 0; i < iTestCases.Count(); i++ )
        {
        CSymbianUnitTestCase& testCase = *( iTestCases[ i ] );

        aResult.StartTestL( testCase.Name() );
        ExecuteTestCaseInThreadL( testCase, aResult, aTimeout );
        aResult.EndTestL();

        if (aResult.CurrentResult())
            {
            aObserver.TestPass(iCurrentTestCase->Name());
            }
        else
            {
            CSymbianUnitTestFailure* failure = aResult.Failures()[
                aResult.Failures().Count()-1];
            aObserver.TestFailed(iCurrentTestCase->Name(), failure->FailureMessage());
            }
        }
    SUT_LOG_FORMAT(_L("testing finished, total passed test cases[%d]"), aResult.PassedTestCount());
    }

// -----------------------------------------------------------------------------
// From MSymbianUnitTestInterface
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSymbianUnitTest::TestCaseCount()
    {
    return iTestCases.Count();
    }

// -----------------------------------------------------------------------------
// From MSymbianUnitTestInterface
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CSymbianUnitTest::Name() const
    {
    if ( iName )
        {
        return *iName;
        }
    return KNullDesC;
    }

// -----------------------------------------------------------------------------
// From MSymbianUnitTestInterface
// -----------------------------------------------------------------------------
//
EXPORT_C void CSymbianUnitTest::TestCaseNamesL(CDesCArray& aTestCaseNames)
    {
    for ( TInt i = iTestCases.Count() -1; i >=0 ; i-- )
        {
        CSymbianUnitTestCase& testCase = *( iTestCases[ i ] );
        aTestCaseNames.AppendL(testCase.Name());
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CSymbianUnitTest::SetupL()
    {
    // The default implementation is no operation
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CSymbianUnitTest::Teardown()
    {
    // The default implementation is no operation
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CSymbianUnitTest::IsMemoryAllocationFailureSimulationUsed() const
    {
    return ( iAllocFailureType == RHeap::EDeterministic );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CSymbianUnitTest::AddTestCaseL( 
    const TDesC& aName,
    FunctionPtr aSetupFunction,
    FunctionPtr aTestFunction,
    FunctionPtr aTeardownFunction )
    {
    const TInt KTestCaseNameLength = 
        Name().Length() + KDoubleColon16().Length() + aName.Length(); 
    HBufC* name = HBufC::NewLC( KTestCaseNameLength );
    name->Des().Append( Name() );
    name->Des().Append( KDoubleColon16 );
    name->Des().Append( aName ); 
    CSymbianUnitTestCase* testCase = 
        CSymbianUnitTestCase::NewL( 
            *name, aSetupFunction, aTestFunction, aTeardownFunction );
    CleanupStack::PopAndDestroy( name );
    CleanupStack::PushL( testCase );
    iTestCases.AppendL( testCase );
    CleanupStack::Pop( testCase );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CSymbianUnitTest::AssertEqualsL(
    TInt aExpectedValue, 
    TInt aActualValue, 
    TInt aLineNumber,
    const TDesC8& aFileName )
    {
    if ( aExpectedValue != aActualValue )
        {
        StopAllocFailureSimulation();
        HBufC8* msg = HBufC8::NewLC( 
            KIntsNotEqualFormat().Size() + KMaxSizeOfTwoIntsAsText );
        msg->Des().Format( KIntsNotEqualFormat, aExpectedValue, aActualValue );
        AssertionFailedL( *msg, aLineNumber, aFileName );
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CSymbianUnitTest::AssertEqualsL(
    const TDesC8& aExpectedValue,
    const TDesC8& aActualValue,
    TInt aLineNumber,
    const TDesC8& aFileName )
    {
    if ( aExpectedValue.Compare( aActualValue ) != 0 )
        {
        StopAllocFailureSimulation();
        TInt size = 
            KDesCsNotEqualFormat().Size() + 
            aExpectedValue.Size() + 
            aActualValue.Size();
        HBufC8 *msg = HBufC8::NewLC( size );
        msg->Des().Format( KDesCsNotEqualFormat, 
                           &aExpectedValue, 
                           &aActualValue );
        AssertionFailedL( *msg, aLineNumber, aFileName );
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CSymbianUnitTest::AssertEqualsL(
    const TDesC16& aExpectedValue,
    const TDesC16& aActualValue,
    TInt aLineNumber,
    const TDesC8& aFileName )
    {
    if ( aExpectedValue.Compare( aActualValue ) != 0 )
        {
        StopAllocFailureSimulation();
        HBufC8* msg = NotEqualsMessageLC( aExpectedValue, aActualValue );
        AssertionFailedL( *msg, aLineNumber, aFileName );
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CSymbianUnitTest::AssertLeaveL(
    const TDesC8& aStatement,
    TInt aActualLeaveCode,
    TInt aExpectedLeaveCode,
    TInt aLineNumber,
    const TDesC8& aFileName )
    {
    if ( aActualLeaveCode == KErrNoMemory &&
         aExpectedLeaveCode != KErrNoMemory &&
         iAllocFailureType == RHeap::EDeterministic )
        {
        User::Leave( KErrNoMemory );
        }   
    if ( aActualLeaveCode != aExpectedLeaveCode )
        {
        StopAllocFailureSimulation();
        HBufC8* msg = HBufC8::NewLC( 
            KAssertLeaveFormat1().Size() + 
            aStatement.Size() + 
            KMaxSizeOfTwoIntsAsText );
        msg->Des().Format( KAssertLeaveFormat2, &aStatement, 
                           aExpectedLeaveCode, aActualLeaveCode );        
        AssertionFailedL( *msg, aLineNumber, aFileName );
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CSymbianUnitTest::RecordNoLeaveFromStatementL(
    const TDesC8& aStatement,
    TInt aLineNumber,
    const TDesC8& aFileName )
    {
    StopAllocFailureSimulation();
    HBufC8* msg = 
        HBufC8::NewLC( KAssertLeaveFormat1().Size() + aStatement.Size() );
    msg->Des().Format( KAssertLeaveFormat1, &aStatement );        
    AssertionFailedL( *msg, aLineNumber, aFileName );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CSymbianUnitTest::AssertionFailedL(
    const TDesC8& aFailureMessage,
    TInt aLineNumber,
    const TDesC8& aFileName )
    {
    TInt dummy( 0 );
    TInt heapCellsBeforeAddingTheFailure( User::Heap().AllocSize( dummy ) );
    
    if ( iTestResult )
        {
        User::LeaveIfError( iTestResult->AddAssertFailure( 
            aFailureMessage, aLineNumber, aFileName ) );
        }
        
    TInt heapCellsAfterAddingTheFailure( User::Heap().AllocSize( dummy ) );
    iHeapCellsReservedByAssertFailure = 
        heapCellsAfterAddingTheFailure - heapCellsBeforeAddingTheFailure;
    User::Leave( KErrSymbianUnitTestAssertionFailed );
    }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CSymbianUnitTest::AssertEqualsL(
    TInt aExpectedValue, 
    TInt aActualValue, 
    TInt aLineNumber,
    const TDesC8& aFileName,
    const TDesC8& aFailureMessage )
    {
    if ( aExpectedValue != aActualValue )
        {
        StopAllocFailureSimulation();
        AssertionFailedL( aFailureMessage, aLineNumber, aFileName );
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CSymbianUnitTest::AssertEqualsL(
    const TDesC8& aExpectedValue,
    const TDesC8& aActualValue,
    TInt aLineNumber,
    const TDesC8& aFileName,
    const TDesC8& aFailureMessage )
    {
    if ( aExpectedValue.Compare( aActualValue ) != 0 )
        {
        StopAllocFailureSimulation();
        AssertionFailedL( aFailureMessage, aLineNumber, aFileName );
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CSymbianUnitTest::AssertEqualsL(
    const TDesC16& aExpectedValue,
    const TDesC16& aActualValue,
    TInt aLineNumber,
    const TDesC8& aFileName,
    const TDesC8& aFailureMessage )
    {
    if ( aExpectedValue.Compare( aActualValue ) != 0 )
        {
        StopAllocFailureSimulation();
        AssertionFailedL( aFailureMessage, aLineNumber, aFileName );
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CSymbianUnitTest::CSymbianUnitTestCase* 
CSymbianUnitTest::CSymbianUnitTestCase::NewL( 
    const TDesC& aName,
    FunctionPtr aSetupFunction,
    FunctionPtr aTestFunction,
    FunctionPtr aTeardownFunction )
    {
    CSymbianUnitTestCase* self = 
        new( ELeave )CSymbianUnitTestCase( 
            aSetupFunction, aTestFunction, aTeardownFunction );
    CleanupStack::PushL( self );
    self->ConstructL( aName );
    CleanupStack::Pop( self );
    return self;    
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CSymbianUnitTest::CSymbianUnitTestCase::CSymbianUnitTestCase( 
    FunctionPtr aSetupFunction,
    FunctionPtr aTestFunction,
    FunctionPtr aTeardownFunction ) :
    iSetupFunction( aSetupFunction ),
    iTestFunction( aTestFunction ),
    iTeardownFunction( aTeardownFunction )
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CSymbianUnitTest::CSymbianUnitTestCase::ConstructL( const TDesC& aName )
    {
    iName = aName.AllocL();
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CSymbianUnitTest::CSymbianUnitTestCase::~CSymbianUnitTestCase()
    {
    delete iName;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
const TDesC& CSymbianUnitTest::CSymbianUnitTestCase::Name() const
    {
    return *iName;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
HBufC8* CSymbianUnitTest::NotEqualsMessageLC(
    const TDesC16& aExpectedValue,
    const TDesC16& aActualValue )
    {
    TInt length = 
        KDesCsNotEqualFormat().Length() + 
        aExpectedValue.Length() + 
        aActualValue.Length();
    HBufC8* msg = HBufC8::NewLC( length );

    HBufC8* expected = 
        CnvUtfConverter::ConvertFromUnicodeToUtf8L( aExpectedValue );
    CleanupStack::PushL( expected );

    HBufC8* actual = CnvUtfConverter::ConvertFromUnicodeToUtf8L( aActualValue );
    CleanupStack::PushL( actual );

    msg->Des().Format( KDesCsNotEqualFormat, expected, actual );

    CleanupStack::PopAndDestroy( actual );
    CleanupStack::PopAndDestroy( expected );
    
    return msg;
    }
    
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CSymbianUnitTest::ExecuteTestCaseInThreadL(
    CSymbianUnitTestCase& aTestCase,
    CSymbianUnitTestResult& aResult,
    TInt aTimeout )
    {
    iCurrentTestCase = &aTestCase;
    iTestResult = &aResult;

    //create exec thread
    TName threadName( KTestThreadName );
    // Append a random number to make the name unique
    const TInt KThreadIdWidth = 10;
    threadName.AppendNumFixedWidthUC( Math::Random(), EHex, KThreadIdWidth );
    RThread execThread;
    TInt err = execThread.Create( threadName,
                              TestThreadEntryFunction,
                              KDefaultStackSize,
                              KMinHeapSize,
                              KTestThreadMaxHeapSize,
                              this );
    User::LeaveIfError( err );
    CleanupClosePushL( execThread );
    

    //start exec thread
    TRequestStatus status = KRequestPending;
    execThread.Logon( status );
    execThread.Resume();
    
    TBool timedOut = EFalse;
    if (aTimeout > 0)
        {    
	    SUT_LOG_DEBUGF(_L("run test case with timeout %d"), aTimeout);
	    //run test case with timeout control
	    TRequestStatus waitStatus = KRequestPending;
        RTimer timer;
        User::LeaveIfError(timer.CreateLocal());
        CleanupClosePushL(timer);
        timer.After(waitStatus, aTimeout*1000000);
        User::WaitForRequest( status, waitStatus );
        if (waitStatus.Int() == KRequestPending)
            {
            timer.Cancel();
            timer.Close();
            User::WaitForRequest( waitStatus );
            }

        if (status.Int() == KRequestPending)
            {
    	    //test case did not complete in time
    	    //terminate the exec thread
    	    SUT_LOG_DEBUG(" test case timed out, kill the exec thread");
    	    timedOut = ETrue;
    	    
    	    execThread.LogonCancel( status );
    	    User::WaitForRequest(status);
    	    
            execThread.Kill(KErrTimedOut);
    	    aResult.AddTimeOutErrorL( aTimeout );
            } 
        CleanupStack::PopAndDestroy( &timer );
	}
    else 
        {
    	SUT_LOG_DEBUG(" run test case without timeout");
    	//exec test case without timeout control
        User::WaitForRequest(status);
        }
    if (status.Int() != KErrNone && !timedOut)
        {
        SUT_LOG_DEBUG("testcase exec thread panic");
        aResult.AddPanicInfoL( 
            execThread.ExitCategory(), 
            execThread.ExitReason(),
            iAllocFailureRate );
        }
    CleanupStack::PopAndDestroy( &execThread );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TInt CSymbianUnitTest::TestThreadEntryFunction( TAny* aPtr )
    {
    CSymbianUnitTest* self = reinterpret_cast< CSymbianUnitTest* >( aPtr );
    TInt err = KErrNoMemory;
    CTrapCleanup* cleanupStack = CTrapCleanup::New();
    if ( cleanupStack )
        {
        // Operator new used without ELeave on purpose to avoid using TRAP.
        CActiveScheduler* scheduler = new CActiveScheduler;
        if ( scheduler )
            {
            CActiveScheduler::Install( scheduler );
            TRAP( err, self->ExecuteTestCaseL() )
            delete scheduler;
            }
        }
    delete cleanupStack;
    return err;
    }    

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CSymbianUnitTest::ExecuteTestCaseL()
    {    
    __ASSERT_ALWAYS( iTestResult, User::Leave( KErrNotFound ) );
    __ASSERT_ALWAYS( iCurrentTestCase, User::Leave( KErrNotFound ) );
    iAllocFailureRate = 0;
    iLeakedMemory = 0;
    iLeakedResource = 0;
    iLeakedRequest = 0;
    TInt leaveCodeFromTest( KErrNoMemory );
    if ( iAllocFailureType == RHeap::EDeterministic )
        {
        TUint counter( 1 );
        while ( leaveCodeFromTest == KErrNoMemory )
            {
            iAllocFailureRate = counter;
            DoExecuteTestCaseL( leaveCodeFromTest );
            counter++;
            }
        }
    else
        {
        DoExecuteTestCaseL( leaveCodeFromTest );
        }
    // Add the possible failure or memory leak to the results
    if ( leaveCodeFromTest == KErrNone )
        {
        if ( iLeakedMemory > 0 )
            {
            iTestResult->AddMemoryLeakInfoL( iLeakedMemory, iAllocFailureRate );
            }
        if ( iLeakedResource > 0 )
            {
            iTestResult->AddResourceLeakInfoL( iLeakedResource, iAllocFailureRate );
            }
        if ( iLeakedRequest > 0 )
            {
            iTestResult->AddRequestLeakInfoL( iLeakedRequest, iAllocFailureRate );
            }
        }
    else if ( leaveCodeFromTest != KErrSymbianUnitTestAssertionFailed )
        {
        iTestResult->AddLeaveFromTestL( leaveCodeFromTest, iAllocFailureRate );
        }
    else
        {
        // No operation here. 
        // Assertion failure has happened and it has been added to the results.
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CSymbianUnitTest::DoExecuteTestCaseL( TInt& aLeaveCodeFromTest )
    {
    __ASSERT_ALWAYS( iTestResult, User::Leave( KErrNotFound ) );
    __ASSERT_ALWAYS( iCurrentTestCase, User::Leave( KErrNotFound ) );
    aLeaveCodeFromTest = KErrNone;
    iHeapCellsReservedByAssertFailure = 0;
    TInt memoryBeforeTest( 0 ); 
    TInt heapCellsBeforeTest( User::Heap().AllocSize( memoryBeforeTest ) );
    TInt resourceInThreadBeforeTest, resourceInProcessBeforeTest;
    RThread().HandleCount( resourceInProcessBeforeTest, resourceInThreadBeforeTest );
    TInt requestBeforeTest = RThread().RequestCount();
    TRAPD( err, ( this->*iCurrentTestCase->iSetupFunction )() )
    if ( err != KErrNone )
        {       
        ( this->*iCurrentTestCase->iTeardownFunction )();
        iTestResult->AddSetupErrorL( err );
        return;
        }
    StartAllocFailureSimulation();
    TRAP( aLeaveCodeFromTest, ( this->*iCurrentTestCase->iTestFunction )() );
    StopAllocFailureSimulation();
       
    ( this->*iCurrentTestCase->iTeardownFunction )();

    TInt memoryAfterTest( 0 );
    TInt heapCellsAfterTest( User::Heap().AllocSize( memoryAfterTest ) );
    TInt leakedHeapCells = 
        heapCellsAfterTest - 
            ( heapCellsBeforeTest + iHeapCellsReservedByAssertFailure );
    if ( leakedHeapCells > 0 )
        {
        iLeakedMemory = memoryAfterTest - memoryBeforeTest;
        }
        
    TInt resourceInThreadAfterTest, resourceInProcessAfterTest;
    RThread().HandleCount( resourceInProcessAfterTest, resourceInThreadAfterTest );
    iLeakedResource = resourceInThreadAfterTest - resourceInThreadBeforeTest;
    TInt requestAfterTest = RThread().RequestCount();
    iLeakedRequest = requestAfterTest - requestBeforeTest;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CSymbianUnitTest::StartAllocFailureSimulation()
    {
    __UHEAP_SETFAIL( iAllocFailureType, iAllocFailureRate );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CSymbianUnitTest::StopAllocFailureSimulation()
    {
    __UHEAP_RESET;
    }
