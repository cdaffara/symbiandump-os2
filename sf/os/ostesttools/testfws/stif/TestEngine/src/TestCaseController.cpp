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
* Description: This module contains implementation of 
* CTestCaseController class member functions.
*
*/


// INCLUDE FILES
#include <e32svr.h>
#include <e32uid.h>
#include <StifLogger.h>
#include "StifTFwIfProt.h"
#include "TestCaseController.h"
#include "TestModuleController.h"
#include "TestReport.h"
#include "Logging.h"
#include "StifHWReset.h"

// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES  
// None

// CONSTANTS
// None

// MACROS
// None

// LOCAL CONSTANTS AND MACROS
// None

// MODULE DATA STRUCTURES
// None

// LOCAL FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// ==================== LOCAL FUNCTIONS =======================================
// None


#define LOGGER iEngine->Logger()

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

    Class: CTestCaseController

    Method: CTestCaseController

    Description: Default constructor

    C++ default constructor can NOT contain any code, that
    might leave.

    Parameters: CTestEngine* aEngine: in: Pointer to CTestEngine
                RTestExecution aTestExecution: in: Handle to RTestExecution
                CAtsLogger& aAtsLogger: in: Reference to CAtsLogger
                RTestExecution aTestExecution: in: Handle to RTestExecution
                const TTestInfo& aTestInfo: in: Test Info

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CTestCaseController::CTestCaseController( CTestEngine* aEngine,
                                          CTestReport* aTestReport,
                                          CAtsLogger& aAtsLogger,
                                          RTestExecution aTestExecution,
                                          const TTestInfo& aTestInfo ) :
    CActive( CActive::EPriorityStandard ),
    iEngine( aEngine ),
    iTestReport( aTestReport ),
    iTestExecution( aTestExecution ),
    iTestInfo( aTestInfo ),
    iState( ETestCaseIdle ),
    iResultPckg( iResult ),
    iAtsLogger( aAtsLogger )
    {
    CActiveScheduler::Add( this );

    }

/*
-------------------------------------------------------------------------------

    Class: CTestCaseController

    Method: ConstructL

    Description: Symbian OS second phase constructor

    Symbian OS default constructor can leave.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CTestCaseController::ConstructL()
    {
    __TRACE( KVerbose, ( _L( "CTestCaseController::ConstructL, [%S]" ), &iTestInfo.iTestCaseInfo.iTitle ) );

    // Logger settings
    TLoggerSettings loggerSettings;
    loggerSettings.iCreateLogDirectories = EFalse;
    loggerSettings.iOverwrite = ETrue;
    loggerSettings.iTimeStamp = EFalse;
    loggerSettings.iLineBreak = EFalse;
    loggerSettings.iEventRanking = EFalse;
    loggerSettings.iThreadId = EFalse;
    loggerSettings.iHardwareFormat = CStifLogger::ETxt;
    loggerSettings.iHardwareOutput = CStifLogger::ERDebug;
    loggerSettings.iEmulatorFormat = CStifLogger::ETxt;
    loggerSettings.iEmulatorOutput = CStifLogger::ERDebug;
    loggerSettings.iUnicode = EFalse;
    loggerSettings.iAddTestCaseTitle = EFalse;

    iRDebugLogger = CStifLogger::NewL( _L( "" ), _L( "" ), loggerSettings );

    // If timeout is specified, then create timeout handler.
    if ( iTestInfo.iTestCaseInfo.iTimeout > TTimeIntervalMicroSeconds(0) )
        {
        iTimeout = CTestCaseTimeout::NewL ( this,
                                            iTestInfo.iTestCaseInfo.iTimeout );
        }

    }

/*
-------------------------------------------------------------------------------

    Class: CTestCaseController

    Method: NewL

    Description: Two-phased constructor.

    Parameters: CTestEngine* aEngine: in: Pointer to CTestEngine
                CTestReport* aTestReport: in: Pointer to CTestReport
                CAtsLogger& aAtsLogger: in: Reference to CAtsLogger
                RTestExecution aTestExecution: in: Handle to RTestExecution
                const TTestInfo& aTestInfo: in: Test Info

    Return Values: CTestCaseController* : pointer to created object

    Errors/Exceptions: Leaves if construction of CTestCaseController fails

    Status: Approved

-------------------------------------------------------------------------------
*/
CTestCaseController* CTestCaseController::NewL( CTestEngine* aEngine,
                                               CTestReport* aTestReport,
                                               CAtsLogger& aAtsLogger,
                                               RTestExecution aTestExecution,
                                               const TTestInfo& aTestInfo )
    {
    CTestCaseController* self = new ( ELeave ) CTestCaseController( aEngine,
        aTestReport, aAtsLogger, aTestExecution, aTestInfo );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestCaseController

    Method: ~CTestCaseController

    Description: Destructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
CTestCaseController::~CTestCaseController()
    {
    __TRACE( KVerbose, ( _L( "CTestCaseController::~CTestCaseController" ) ) );
    Cancel();

    delete iRDebugLogger;
    delete iTimeout;
    delete iTestCaseArguments;
    }

/*
-------------------------------------------------------------------------------

    Class: CTestCaseController

    Method: StartL

    Description: Start active object

    Parameters: const RMessage& aMessage: in: Server message

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CTestCaseController::StartL( const RMessage2& aMessage )
    {
    iRDebugLogger->Log( _L( "Starting testcase [%S]" ), &iTestInfo.iTestCaseInfo.iTitle );
    __TRACE( KInit, ( _L(" Starting testcase [%S]"), &iTestInfo.iTestCaseInfo.iTitle ) );

    // Check that this request is not pending!!
    __ASSERT_ALWAYS( iState != ETestCaseRunning,
                            iEngine->PanicClient( EReqPending, aMessage ) );
    iMessage = aMessage;

    iState = ETestCaseRunning;
    
    delete iTestCaseArguments;
    iTestCaseArguments = NULL;
    
    TInt testCaseArgumentsLength = iMessage.GetDesLength( 1 );
    if ( ( testCaseArgumentsLength != KErrArgument ) && ( testCaseArgumentsLength != KErrBadDescriptor ) )
        {
        iTestCaseArguments = HBufC::NewL( testCaseArgumentsLength );
        TPtr testCaseArgumentsPtr( iTestCaseArguments->Des() );
        User::LeaveIfError( iMessage.Read( 1, testCaseArgumentsPtr ) );
        iTestExecution.RunTestCase( iResultPckg, *iTestCaseArguments, iStatus );        
        }
    else
        {    
        iTestExecution.RunTestCase( iResultPckg, iStatus );
        }
    SetActive();

    // If testcase has timeout (handler), then start it
    if ( iTimeout )
        {
        iTimeout->Start();
        }

    }

/*
-------------------------------------------------------------------------------

    Class: CTestCaseController

    Method: Timeout

    Description: Timeouts active request.
    - Cancel request

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CTestCaseController::Timeout()
    {
    if ( iState == ETestCaseRunning )
        {
        iState = ETestCaseTimeout;
        Cancel();
        }

    }

/*
-------------------------------------------------------------------------------

    Class: CTestCaseController

    Method: RunL

    Description: RunL handles completed requests. Leaves are handled in
                 RunError method

    Parameters: None

    Return Values: None

    Errors/Exceptions: Leaves if AddTestCaseResultL leaves

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestCaseController::RunL()
    {
    iState = ETestCaseCompleted;


    // "iStatus.Int()" error code is received from system's framework not from
    // test case execution.
    if ( iStatus.Int() != KErrNone )
        {
            if ( iStatus.Int() == KErrServerTerminated )
            {
            // something went badly wrong!
            __TRACE( KInit, ( CStifLogger::ERed, 
                _L( "TestCase [%S] cannot be executed, STIF panics with[%d]" ),
                &iTestInfo.iTestCaseInfo.iTitle,
                iStatus.Int() ) );       
                
            __TRACE( KInit, ( CStifLogger::ERed, 
                _L( "Possible reason: Test case has paniced seriously" ) ) );

            // We don't leave here but we write information to 
            // forward(testreport etc.)
            _LIT( KLeaveInfo, "Test case execution fails" );
            // Sets test case to crash category
            iResult.iCaseExecutionResultCode = iStatus.Int();
            // Test case result
            iResult.iTestResult.iResult = iStatus.Int();
            iResult.iTestResult.iResultDes = KLeaveInfo;

            } 
        
            else
            {   
        // For example testmodule's NewL or testmodule's constructor has
        // leaved and STIF cannot connect to test module
        __TRACE( KInit, ( CStifLogger::ERed, 
            _L( "TestCase [%S] cannot execute, TestModule loading operations fails with[%d]" ),
            &iTestInfo.iTestCaseInfo.iTitle,
            iStatus.Int() ) );
        __TRACE( KInit, ( CStifLogger::ERed, 
            _L( "Possible reason: TestModule's NewL or Constructor has leaved and STIF cannot connect to test module" ) ) );

        // We don't leave here but we write information to 
        // forward(testreport etc.)
        _LIT( KLeaveInfo, "TestModule loading fails, cannot connect to the TestModule" );
        // Sets test case to crash category
        iResult.iCaseExecutionResultCode = iStatus.Int();
        // Test case result
        iResult.iTestResult.iResult = iStatus.Int();
        iResult.iTestResult.iResultDes = KLeaveInfo;
            }
        }
        
	else
	{
			
    // Cancel event request, because the testcase is completed
    iTestExecution.CancelAsyncRequest( ETestExecutionNotifyEvent );        
	}

    // Test case is executed
    if( iTestInfo.iTestCaseInfo.iTitle.Length() > 0 )
        {
        iRDebugLogger->Log( _L( "TestCase [%S] finished with verdict[%d]" ),
                            &iTestInfo.iTestCaseInfo.iTitle,
                            iResult.iTestResult.iResult );

        __TRACE( KInit, ( _L( " TestCase [%S] finished with verdict[%d]" ),
                            &iTestInfo.iTestCaseInfo.iTitle,
                            iResult.iTestResult.iResult ) );
        }
    else
        {
        iRDebugLogger->Log( _L( "TestCase [%d] finished with verdict[%d]" ),
                            iTestInfo.iTestCaseInfo.iCaseNumber,
                            iResult.iTestResult.iResult );

        __TRACE( KInit, ( _L( " TestCase [%d] finished with verdict[%d]" ),
                            iTestInfo.iTestCaseInfo.iCaseNumber,
                            iResult.iTestResult.iResult ) );
        }
    __TRACE( KVerbose, ( 
            _L( "CTestCaseController::RunL: iStatus=[%d]" ), iStatus.Int() ));

    GenerateXmlLogL();

    // Report test case result
    if( iTestReport )
        {
        iTestReport->AddTestCaseResultL( iTestInfo, iResult, iStatus.Int() );
        iTestReport->UpdateReportSummaryL();
        }

    TRAPD( ret, iMessage.WriteL( 0, iResultPckg ) );

    // Case done
    iMessage.Complete( ret );

    }

/*
-------------------------------------------------------------------------------

    Class: CTestCaseController

    Method: DoCancel

    Description: Cancel active request

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CTestCaseController::DoCancel()
    {

    switch ( iState )
        {
        case ETestCaseRunning:
            iTestExecution.CancelAsyncRequest( ETestExecutionRunTestCase );
            if( iTestInfo.iTestCaseInfo.iTitle.Length() > 0 )
                {
                iRDebugLogger->Log( _L( "TestCase [%S] execution aborted" ),
                                    &iTestInfo.iTestCaseInfo.iTitle );

                __TRACE( KInit, ( _L( "TestCase [%S] execution aborted" ),
                                    &iTestInfo.iTestCaseInfo.iTitle ) );
                }
            else
                {
                iRDebugLogger->Log( _L( "TestCase [%S] execution aborted" ),
                                    iTestInfo.iTestCaseInfo.iCaseNumber );

                __TRACE( KInit, ( _L( "TestCase [%S] execution aborted" ),
                                    iTestInfo.iTestCaseInfo.iCaseNumber ) );
                }
            break;
        case ETestCaseTimeout:
            iTestExecution.CancelAsyncRequest( ETestExecutionRunTestCase );
            // Update result to timeout
            iResult.iCaseExecutionResultType = TFullTestResult::ECaseTimeout;
            iResult.iTestResult.iResultDes = _L("Test case was timeouted and cancelled");

            if( iTestReport )
                {
                iTestReport->AddTestCaseResultL( iTestInfo, iResult, KErrTimedOut );
                }

            if( iTestInfo.iTestCaseInfo.iTitle.Length() > 0 )
                {
                iRDebugLogger->Log( _L( "TestCase [%S] finished with verdict[%d]" ),
                                    &iTestInfo.iTestCaseInfo.iTitle,
                                    iResult.iTestResult.iResult );

                __TRACE( KInit, ( _L( " TestCase [%S] finished with verdict[%d]" ),
                                    &iTestInfo.iTestCaseInfo.iTitle,
                                    iResult.iTestResult.iResult ) );
                }
            else
                {
                iRDebugLogger->Log( _L( "TestCase [%d] finished with verdict[%d]" ),
                                    iTestInfo.iTestCaseInfo.iCaseNumber,
                                    iResult.iTestResult.iResult );

                __TRACE( KInit, ( _L( " TestCase [%d] finished with verdict[%d]" ),
                                    iTestInfo.iTestCaseInfo.iCaseNumber,
                                    iResult.iTestResult.iResult ) );
                }            
            
            break;
        case ETestCaseSuicided:
            {
            //Store current results because cancel operation overwrites it
            TInt currentTestResult = iResult.iTestResult.iResult;
            TInt currentExecutionResult = iResult.iCaseExecutionResultCode;
            iTestExecution.CancelAsyncRequest(ETestExecutionRunTestCase);
            // Update result to suicide
            switch(iStopExecutionType)
                {
                case EOk:
                    iResult.iCaseExecutionResultType = TFullTestResult::ECaseExecuted;
                    iResult.iTestResult.iResultDes = _L("Test case passed (StopExecution())");
                    break;
                case EFail:
                    iResult.iCaseExecutionResultType = TFullTestResult::ECaseExecuted;
                    iResult.iTestResult.iResultDes = _L("Test case failed (StopExecution())");
                    break;
                default: //EAbort
                    iResult.iCaseExecutionResultType = TFullTestResult::ECaseSuicided;
                    iResult.iTestResult.iResultDes = _L("Test case killed (StopExecution())");
                }
            iResult.iTestResult.iResult = currentTestResult;
            iResult.iCaseExecutionResultCode = currentExecutionResult;
            }
            if(iTestReport)
                {
                iTestReport->AddTestCaseResultL(iTestInfo, iResult, KErrNone);
                }

            // Log some message
            if(iTestInfo.iTestCaseInfo.iTitle.Length() > 0)
                {
                _LIT(KTestCaseDesc, "TestCase [%S] finished with verdict [%d]");
                switch(iStopExecutionType)
                    {
                    case EOk:
                        {
                        _LIT(KTestCaseResult, "***Testcase PASSED***");

                        iRDebugLogger->Log(KTestCaseResult);
                        __TRACE(KInit, (KTestCaseResult));
                        iRDebugLogger->Log(KTestCaseDesc, &iTestInfo.iTestCaseInfo.iTitle, iResult.iTestResult.iResult);
                        __TRACE(KInit, (KTestCaseDesc, &iTestInfo.iTestCaseInfo.iTitle, iResult.iTestResult.iResult));
                        break;
                        }
                    case EFail:
                        {
                        _LIT(KTestCaseResult, "***Testcase FAILED***");

                        iRDebugLogger->Log(KTestCaseResult);
                        __TRACE(KInit, (KTestCaseResult));
                        iRDebugLogger->Log(KTestCaseDesc, &iTestInfo.iTestCaseInfo.iTitle, iResult.iTestResult.iResult);
                        __TRACE(KInit, (KTestCaseDesc, &iTestInfo.iTestCaseInfo.iTitle, iResult.iTestResult.iResult));
                        break;
                        }
                    default: //EAbort
                        {
                        _LIT(KTestCaseResult, "***Testcase KILLED***");

                        iRDebugLogger->Log(KTestCaseResult);
                        __TRACE(KInit, (KTestCaseResult));
                        iRDebugLogger->Log(KTestCaseDesc, &iTestInfo.iTestCaseInfo.iTitle, iResult.iTestResult.iResult);
                        __TRACE(KInit, (KTestCaseDesc, &iTestInfo.iTestCaseInfo.iTitle, iResult.iTestResult.iResult));
                        }
                    }
                }
            else
                {
                _LIT(KTestCaseDesc, "TestCase [%d] finished with verdict [%d]");
                switch(iStopExecutionType)
                    {
                    case EOk:
                        {
                        _LIT(KTestCaseResult, "***Testcase PASSED***");

                        iRDebugLogger->Log(KTestCaseResult);
                        __TRACE(KInit, (KTestCaseResult));
                        iRDebugLogger->Log(KTestCaseDesc, iTestInfo.iTestCaseInfo.iCaseNumber, iResult.iTestResult.iResult);
                        __TRACE(KInit, (KTestCaseDesc, iTestInfo.iTestCaseInfo.iCaseNumber, iResult.iTestResult.iResult));
                        break;
                        }
                    case EFail:
                        {
                        _LIT(KTestCaseResult, "***Testcase FAILED***");

                        iRDebugLogger->Log(KTestCaseResult);
                        __TRACE(KInit, (KTestCaseResult));
                        iRDebugLogger->Log(KTestCaseDesc, iTestInfo.iTestCaseInfo.iCaseNumber, iResult.iTestResult.iResult);
                        __TRACE(KInit, (KTestCaseDesc, iTestInfo.iTestCaseInfo.iCaseNumber, iResult.iTestResult.iResult));
                        break;
                        }
                    default: //EAbort
                        {
                        _LIT(KTestCaseResult, "***Testcase KILLED***");

                        iRDebugLogger->Log(KTestCaseResult);
                        __TRACE(KInit, (KTestCaseResult));
                        iRDebugLogger->Log(KTestCaseDesc, iTestInfo.iTestCaseInfo.iCaseNumber, iResult.iTestResult.iResult);
                        __TRACE(KInit, (KTestCaseDesc, iTestInfo.iTestCaseInfo.iCaseNumber, iResult.iTestResult.iResult));
                        }
                    }
                }
            break;
        case ETestCaseIdle:
        case ETestCaseCompleted:
        default:
            // DoCancel called in wrong state => Panic
            _LIT( KTestCaseController, "CTestCaseController" );
            User::Panic( KTestCaseController, EDoCancelDisorder );
            break;
        }

    // Write result and complete 
    TRAPD( ret, iMessage.WriteL( 0, iResultPckg ) );

    iMessage.Complete( ret );  
        
    iState = ETestCaseCompleted;
    
    // Generate xml result log
    GenerateXmlLogL();

    }

/*
-------------------------------------------------------------------------------

    Class: CTestCaseController

    Method: RunError

    Description: Handle errors.

    Parameters: TInt aError: in: Symbian OS error: Error code

    Return Values: TInt KErrNone: Always returned KErrNone

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CTestCaseController::RunError( TInt aError )
    {
    __TRACE( KError, ( CStifLogger::ERed, 
        _L( "CTestCaseController::RunError: Test case execution has failed! aError=[%d]" ), aError ) );
    TInt completionError = aError;

    // Write result, because it could include descriptive result for
    // failed case
    TRAPD( err, iMessage.WriteL( 0, iResultPckg ); );

    if ( err != KErrNone )
        {
        completionError = err;
        }

    // Complete message with error
    iMessage.Complete( completionError );

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestCaseController

    Method: GenerateXmlLogL

    Description: Generate XML log.

    Parameters: None

    Return Values: None
    
    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CTestCaseController::GenerateXmlLogL()
    {
    
    // Report result with AtsLogger
    iAtsLogger.BeginTestCaseReportL( iTestInfo.iTestCaseInfo.iTitle, 
                                     KErrNone,  // Expected result not supported
                                     iResult.iStartTime );
    
    if( iResult.iCaseExecutionResultCode != KErrNone )
        {
        iAtsLogger.SetTestCaseResultL( iResult.iCaseExecutionResultCode );
        iAtsLogger.TestCaseFailed();  
        if( iResult.iTestResult.iResultDes.Length() > 0 )
            {
            iAtsLogger.ErrorL( iResult.iTestResult.iResultDes );   
            } 
        }
     else
        {
        iAtsLogger.SetTestCaseResultL( iResult.iTestResult.iResult ); 
        if( iResult.iTestResult.iResult == KErrNone )
            {
            iAtsLogger.TestCasePassed();  
            }
        else
            {
            iAtsLogger.TestCaseFailed();  
            if( iResult.iTestResult.iResultDes.Length() > 0 )
                {
                iAtsLogger.ErrorL( iResult.iTestResult.iResultDes );    
                }
            }
        }
    // Set test case result to ATS Logger
    iAtsLogger.EndTestCaseL();

    }

/*
-------------------------------------------------------------------------------

    Class: CTestCaseController

    Method: GetTestCaseInfo

    Description: Get testcase info(test module, config file, test case, etc).

    Parameters: TTestInfo& aTestInfo: inout: Test information

    Return Values: None
    
    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CTestCaseController::GetTestCaseInfo( TTestInfo& aTestInfo )
    {
    aTestInfo = iTestInfo;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestCaseController

    Method: Suicide

    Description: Cancels active request.

    Parameters: aCode: the reason of suicide

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CTestCaseController::Suicide(TStopExecutionType aType, TInt aCode)
    {
    if(iState == ETestCaseRunning)
        {
        iStopExecutionType = aType;
        switch(iStopExecutionType)
            {
            case EOk:
                iResult.iTestResult.iResult = KErrNone;
                iResult.iCaseExecutionResultCode = KErrNone;
                break;
            case EFail:
                iResult.iTestResult.iResult = aCode;
                iResult.iCaseExecutionResultCode = KErrNone;
                break;
            default: //EAbort
                iResult.iTestResult.iResult = aCode;
                iResult.iCaseExecutionResultCode = aCode;
            }
        iState = ETestCaseSuicided;
        Cancel();
        }
    }

/*
-------------------------------------------------------------------------------

    DESCRIPTION

    This module contains implementation of CTestProgressNotifier class member
    functions.

-------------------------------------------------------------------------------
*/

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

    Class: CTestProgressNotifier

    Method: CTestProgressNotifier

    Description: Default constructor

    C++ default constructor can NOT contain any code, that
    might leave.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CTestProgressNotifier::CTestProgressNotifier( CTestEngine* aEngine,
                             RTestExecution aTestExecution ) :
    CActive( CActive::EPriorityStandard ),
    iEngine( aEngine ),
    iTestExecution( aTestExecution ),
    iState( ETestProgressIdle ),
    iProgressPckg( iProgress )
    {
    CActiveScheduler::Add( this );

    }

/*
-------------------------------------------------------------------------------

    Class: CTestProgressNotifier

    Method: ConstructL

    Description: Symbian OS second phase constructor

    Symbian OS default constructor can leave.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestProgressNotifier::ConstructL()
    {
    __TRACE( KVerbose, ( _L( "CTestProgressNotifier::ConstructL" ) ) );
    }

/*
-------------------------------------------------------------------------------

    Class: CTestProgressNotifier

    Method: NewL

    Description: Two-phased constructor.

    Parameters: CTestEngine* aEngine: in: Pointer to CTestEngine
                RTestExecution aTestExecution: in: Handle to RTestExecution

    Return Values: CTestProgressNotifier* : pointer to created object

    Errors/Exceptions: Leaves if construction of CTestProgressNotifier fails

    Status: Approved

-------------------------------------------------------------------------------
*/
CTestProgressNotifier* CTestProgressNotifier::NewL( CTestEngine* aEngine,
                             RTestExecution aTestExecution )
    {
    CTestProgressNotifier* self = new ( ELeave ) CTestProgressNotifier(
        aEngine, aTestExecution );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestProgressNotifier

    Method: ~CTestProgressNotifier

    Description: Destructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CTestProgressNotifier::~CTestProgressNotifier()
    {
    __TRACE( KVerbose, ( _L( "CTestProgressNotifier::~CTestProgressNotifier" ) ) );
    Cancel();

    }

/*
-------------------------------------------------------------------------------

    Class: CTestProgressNotifier

    Method: StartL

    Description: Start active object

    Parameters: const RMessage& aMessage: in: Server message

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestProgressNotifier::StartL( const RMessage2& aMessage )
    {
    __TRACE( KVerbose, ( _L( "CTestProgressNotifier::StartL" ) ) );
    // Check that this request is not pending!!
    __ASSERT_ALWAYS( iState != ETestProgressPending,
                        iEngine->PanicClient( EReqPending, aMessage ) );
    iMessage = aMessage;

    iState = ETestProgressPending;
    SetActive();

    iTestExecution.NotifyProgress( iProgressPckg, iStatus );

    }

/*
-------------------------------------------------------------------------------

    Class: CTestProgressNotifier

    Method: RunL

    Description: RunL handles completed requests.

    Parameters: None

    Return Values: None

    Errors/Exceptions: Leaves if WriteL leaves
                       Leaves if iStatus is not KErrNone or KErrEof
                       Leaves are handled in RunError method

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestProgressNotifier::RunL()
    {
    __TRACE( KVerbose, (_L("CTestProgressNotifier::RunL: iStatus=[%d]" ), iStatus.Int() ));

    iState = ETestProgressCompleted;

    // Check that request was successful or completed with acceptable error
    // KErrEof is acceptable error and it means that the test case will not
    // send progresses anymore (because it is closed)
    if ( KErrNone == iStatus.Int() )
        {
        iMessage.WriteL( 0, iProgressPckg );
        }
    else if ( KErrEof != iStatus.Int() )
        {
        // Leave, error will be handled in RunError
        User::Leave( iStatus.Int() );
        }

    // Complete message
    iMessage.Complete( iStatus.Int() );

    }

/*
-------------------------------------------------------------------------------

    Class: CTestProgressNotifier

    Method: DoCancel

    Description: Cancel active request

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestProgressNotifier::DoCancel()
    {
    switch ( iState )
        {
        case ETestProgressPending:
            iTestExecution.CancelAsyncRequest( ETestExecutionNotifyProgress );
            iMessage.Complete( KErrCancel );
            break;
        case ETestProgressIdle:
        case ETestProgressCompleted:
        default:
            // DoCancel called in wrong state => Panic
            _LIT( KTestProgressNotifier, "CTestProgressNotifier" );
            User::Panic( KTestProgressNotifier, EDoCancelDisorder );
            break;
        }
    iState = ETestProgressIdle;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestProgressNotifier

    Method: RunError

    Description: Handle errors.

    Parameters: TInt aError: in: Symbian OS error: Error code

    Return Values: TInt KErrNone: Always returned KErrNone

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CTestProgressNotifier::RunError( TInt aError )
    {
    // Complete message with error
    iMessage.Complete( aError );

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    DESCRIPTION

    This module contains implementation of CTestEventNotifier class member
    functions.

-------------------------------------------------------------------------------
*/

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

    Class: CTestEventNotifier

    Method: CTestEventNotifier

    Description: Default constructor

    C++ default constructor can NOT contain any code, that
    might leave.

    Parameters: CTestEngine* aEngine: in: Pointer to CTestEngine
                RTestExecution aTestExecution: in: Handle to RTestExecution

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CTestEventNotifier::CTestEventNotifier( CTestEngine* aEngine, 
                                        RTestExecution aTestExecution ) :
    CActive( CActive::EPriorityStandard ),
    iEngine( aEngine ),
    iTestExecution( aTestExecution ),
    iState( ETestEventIdle ),
    iEventPckg( iEvent ),
    iEventNotifyPckg( iEventNotify ),
    iController( NULL )
    {
    CActiveScheduler::Add( this );

    }

/*
-------------------------------------------------------------------------------

    Class: CTestEventNotifier

    Method: ConstructL

    Description: Symbian OS second phase constructor

    Symbian OS default constructor can leave.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestEventNotifier::ConstructL( )
    {
    __TRACE( KVerbose, ( _L( "CTestEventNotifier::ConstructL" ) ) );

    }

/*
-------------------------------------------------------------------------------

    Class: CTestEventNotifier

    Method: NewL

    Description: Two-phased constructor.

    Parameters: CTestEngine* aEngine: in: Pointer to CTestEngine
                RTestExecution aTestExecution: in: Handle to RTestExecution

    Return Values: CTestEventNotifier* : pointer to created object

    Errors/Exceptions: Leaves if construction of CTestEventNotifier fails

    Status: Approved

-------------------------------------------------------------------------------
*/
CTestEventNotifier* CTestEventNotifier::NewL( CTestEngine* aEngine,
                                              RTestExecution aTestExecution )
    {
    CTestEventNotifier* self = new ( ELeave ) CTestEventNotifier(
        aEngine, aTestExecution );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestEventNotifier

    Method: ~CTestEventNotifier

    Description: Destructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CTestEventNotifier::~CTestEventNotifier()
    {
    __TRACE( KVerbose, ( _L( "CTestEventNotifier::~CTestEventNotifier" ) ) );
    Cancel();
    iEventArray.ResetAndDestroy();
    iEventArray.Close();
    delete iController;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestEventNotifier

    Method: StartL

    Description: Start active object

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestEventNotifier::Start()
    {
    __TRACE( KVerbose, ( _L( "CTestEventNotifier::StartL" ) ) );
    // Check that this request is not pending!!
    __ASSERT_ALWAYS( iState != ETestEventPending,
        User::Leave( KErrAlreadyExists ) );

    iEvent.SetType( TEventIf::EEnable );
    iState = ETestEventPending;
    iTestExecution.NotifyEvent( iEventPckg, iStatus );
    SetActive();

    }

/*
-------------------------------------------------------------------------------

    Class: CTestEventNotifier

    Method: RunL

    Description: RunL handles completed requests.

    Parameters: None

    Return Values: None

    Errors/Exceptions: Leaves if iStatus is not KErrNone
                       Leaves if iState is not ETestEventPending
                       Leaves if some leaving method called here leaves

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestEventNotifier::RunL()
    {
    __TRACE( KVerbose, ( _L( "CTestEventNotifier::RunL: iStatus=[%d]" ), iStatus.Int() ) );

    User::LeaveIfError( iStatus.Int() );

    switch( iState )
        {
        case ETestEventPending:
            {
            iState = ETestEventCompleted;
            
            switch( iEvent.Type() )
                {
                case TEventIf::EReqEvent:
                    {
                    __RDEBUG( ( _L("CTestEventNotifier(ReqEvent) %S"), 
                        &iEvent.Name() ));
                    if( iEngine->IsStateEventAndSet( iEvent.Name() ) )
                        {
                        __TRACE( KVerbose, ( _L( "CTestEventNotifier::RunL: Requested Global event already set" ) ) );
                        TEventIf event( TEventIf::ESetEvent, iEvent.Name(),
                                        TEventIf::EState );
                        TEventIfPckg eventPckg( event );
                        TRequestStatus status;
                        iTestExecution.NotifyEvent( eventPckg, status );
                        User::WaitForRequest( status );
                        }
                    //add to iEventArray
                    HBufC* name = iEvent.Name().AllocLC();
                    if( iEventArray.Append( name ) != KErrNone )
                        {
                        User::Leave( KErrNoMemory );
                        }
                    CleanupStack::Pop( name );
                    }
                    break;                    
                case TEventIf::ERelEvent:
                    {
                    __TRACE( KVerbose, ( _L( "CTestEventNotifier(RelEvent) %S" ), &iEvent.Name() ) );
                    //remove from iEventArray
                    TInt count = iEventArray.Count();
                    const TDesC& eventName = iEvent.Name();
                    TInt i;
                    for( i = 0; i < count; i++ )
                        {
                        TPtrC name = iEventArray[i]->Des();
                        if( name == eventName )
                            {
                            HBufC* tmp = iEventArray[i];
                            iEventArray.Remove( i );
                            delete tmp;
                            break;
                            }
                        }
                    // Check that event was found
                    if( i == count )
                        {
                        User::Leave( KErrGeneral );
                        }         
                    }
                    break;                    
                case TEventIf::ESetEvent:
                    {
                    __RDEBUG( ( _L("CTestEventNotifier(SetEvent) %S"), 
                        &iEvent.Name() ));
                    iController = iEngine->CtlEventL( iEvent, iStatus );
                    SetActive();
                    return;
                    }
                case TEventIf::EUnsetEvent:
                    {
                    __RDEBUG( ( _L("CTestEventNotifier(UnsetEvent) %S"), 
                        &iEvent.Name() ));
                    iController = iEngine->CtlEventL( iEvent, iStatus );
                    SetActive();
                    return;
                    }
                default:
                    {
                    User::Leave( KErrGeneral );
                    }
                }
            // Set request again
            Start();                
            // Otherwise request is enabled again later
            }
            break;
        case ETestEventCompleted:
            __RDEBUG( ( _L("CTestEventNotifier(Complete)")));
            Start();
            break;
        default:
            User::Leave( KErrGeneral );
            break;
        }

    }

/*
-------------------------------------------------------------------------------

    Class: CTestEventNotifier

    Method: DoCancel

    Description: Cancel active request

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestEventNotifier::DoCancel()
    {
    __TRACE( KVerbose, ( _L( "CTestEventNotifier::DoCancel" ) ) );

    switch ( iState )
        {
        case ETestEventPending:
            iTestExecution.CancelAsyncRequest( ETestExecutionNotifyEvent );
            break;
        case ETestEventCompleted:
            delete iController;
            iController = NULL;
            break;
        default:
            // DoCancel called in wrong state => Panic
            _LIT( KTestEventNotifier, "CTestEventNotifier" );
            User::Panic( KTestEventNotifier, EDoCancelDisorder );
            break;
        }

    iState = ETestEventIdle;
    iEventArray.ResetAndDestroy();

    }

/*
-------------------------------------------------------------------------------

    Class: CTestEventNotifier

    Method: RunError

    Description: Handle errors.

    Parameters: TInt aError: in: Symbian OS error: Error code

    Return Values: TInt KErrNone: Always returned KErrNone

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CTestEventNotifier::RunError( TInt aError )
    {
    switch ( iState )
        {
        case ETestEventPending:
            if( aError != KErrCancel )
                {
                __TRACE( KError, ( CStifLogger::ERed, _L( "CTestEventNotifier::RunError %d"), aError) );
                }
            else
                {
                __TRACE( KVerbose, ( _L( "CTestEventNotifier stopped")) );
                }

            // We stop event notifier if we get error here
            // Clear requested event list
            iEventArray.ResetAndDestroy();
            break;
        case ETestEventCompleted:
            // Do not close here
            __TRACE( KError, ( CStifLogger::ERed, _L( "CTestEventNotifier::RunError %d"), aError) );
            delete iController;
            iController = NULL;
            // forward error to testcase
            iEvent.SetType( TEventIf::EEnable );
            iState = ETestEventPending;
            iTestExecution.NotifyEvent( iEventPckg, iStatus, aError );
            SetActive();
            break;
        default:
            __TRACE( KError, ( CStifLogger::ERed, _L( "CTestEventNotifier::RunError %d"), aError) );
            // DoCancel called in wrong state => Panic
            _LIT( KTestEventNotifier, "CTestEventNotifier" );
            User::Panic( KTestEventNotifier, EDoCancelDisorder );
            break;
        }

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestEventNotifier

    Method: CtlEventL

    Description: Controls events

    Parameters: const TEventIf& aEvent: in: Event
                TRequestStatus& aStatus: in: Request status

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestEventNotifier::CtlEvent( const TEventIf& aEvent, 
                                    TRequestStatus& aStatus )
    {
    __TRACE( KVerbose, ( _L( "CTestEventNotifier::CtlEventL" ) ) );
    const TDesC& eventName = aEvent.Name();
    TInt count = iEventArray.Count();
    for( TInt i = 0; i < count; i++ )
        {
        TPtrC name = iEventArray[i]->Des();
        if( name == eventName )
            {
            iEventNotify.Copy( aEvent );
            iTestExecution.NotifyEvent( iEventNotifyPckg, aStatus );
            return;
            }
        }
    }

/*
-------------------------------------------------------------------------------

    Class: CTestEventNotifier

    Method: CheckCtlEventL

    Description: Checks if CtlEvent should be called

    Parameters: const TEventIf& aEvent: in: Event

    Return Values: ETrue: If CtlEvent sould be called.

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TBool CTestEventNotifier::CheckCtlEvent( const TEventIf& aEvent )
    {
    const TDesC& eventName = aEvent.Name();
    TInt count = iEventArray.Count();
    for( TInt i = 0; i < count; i++ )
        {
        TPtrC name = iEventArray[i]->Des();
        if( name == eventName )
            {
            return ETrue;
            }
        }

    return EFalse;
    }

/*
-------------------------------------------------------------------------------

    DESCRIPTION

    This module contains implementation of CTestEventController class member
    functions.

-------------------------------------------------------------------------------
*/

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

    Class: CTestEventController

    Method: CTestEventController

    Description: Default constructor

    C++ default constructor can NOT contain any code, that
    might leave.

    Parameters: CTestEngine* aEngine: in: Pointer to CTestEngine
                const TEventIf& aEvent: in: Event

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CTestEventController::CTestEventController( CTestEngine* aEngine, 
                                            const TEventIf& aEvent ) :
    iEngine( aEngine ),
    iRequestStatus( NULL ),
    iEventPckg( iEvent ),
    iActiveEventCmds( 0 )
    {
    iEvent.Copy( aEvent );

    }

/*
-------------------------------------------------------------------------------

    Class: CTestEventController

    Method: ConstructL

    Description: Symbian OS second phase constructor

    Symbian OS default constructor can leave.

    Parameters: TRequestStatus* aStatus: in: Request status

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestEventController::ConstructL( TRequestStatus* aStatus )
    {
    iEngine->Logger()->Log( _L( "CTestEventController::ConstructL" ) );

    if( CheckEventsL() == 0 )
        {
        // No request was pending, complete immediately
        User::RequestComplete( aStatus, KErrNone );
        }
    else
        {
        iRequestStatus = aStatus;
        }

    }

/*
-------------------------------------------------------------------------------

    Class: CTestEventController

    Method: ConstructL

    Description: Symbian OS second phase constructor

    Symbian OS default constructor can leave.

    Parameters: RMessage& aMessage: inout: Message to be handled

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestEventController::ConstructL( RMessage2& aMessage )
    {
    iEngine->Logger()->Log( _L( "CTestEventController::ConstructL" ) );

    if( CheckEventsL() == 0 )
        {
        // No request was pending, complete immediately
        aMessage.Complete( KErrNone );
        }
    else
        {
        iMessage = aMessage;
        }

    }

/*
-------------------------------------------------------------------------------

    Class: CTestEventNotifier

    Method: NewL

    Description: Two-phased constructor.

    Parameters: CTestEngine* aEngine: in: Pointer to CTestEngine
                const TEventIf& aEvent: in: Event
                TRequestStatus* aStatus: in: Request status

    Return Values: CTestEventController* : pointer to created object

    Errors/Exceptions: Leaves if construction of CTestEventController fails

    Status: Approved

-------------------------------------------------------------------------------
*/

CTestEventController* CTestEventController::NewL( CTestEngine* aEngine,
                                                  const TEventIf& aEvent,
                                                  TRequestStatus* aStatus )
    {
    CTestEventController* self = 
        new ( ELeave )CTestEventController( aEngine, aEvent );
    CleanupStack::PushL( self );
    self->ConstructL( aStatus );
    CleanupStack::Pop( self );
    return self;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestEventNotifier

    Method: NewL

    Description: Two-phased constructor.

    Parameters: CTestEngine* aEngine: in: Pointer to CTestEngine
                const TEventIf& aEvent: in: Event
                RMessage& aMessage: inout: Message to be handled

    Return Values: CTestEventController* : pointer to created object

    Errors/Exceptions: Leaves if construction of CTestEventController fails

    Status: Approved

-------------------------------------------------------------------------------
*/
CTestEventController* CTestEventController::NewL( CTestEngine* aEngine,
                                                  const TEventIf& aEvent,
                                                  RMessage2& aMessage )
    {
    CTestEventController* self = 
        new ( ELeave )CTestEventController( aEngine, aEvent );
    CleanupStack::PushL( self );
    self->ConstructL( aMessage );
    CleanupStack::Pop( self );
    return self;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestEventController

    Method: ~CTestEventController

    Description: Destructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/

CTestEventController::~CTestEventController()
    {
	if( iEngine != NULL )
		{
		if( iEngine->Logger() != NULL )
			{
			iEngine->Logger()->Log( _L( "CTestEventController::~CTestEventController" ) );
			}
		}
    
    if( iRequestStatus )
        {
        // Must be completed if pending, otherwise  
        // CTestEventNotifier::DoCancel blocks
        User::RequestComplete( iRequestStatus, KErrCancel );
        }
            
    iEventCallBacks.ResetAndDestroy();
    iEventCallBacks.Close();

    }

/*
-------------------------------------------------------------------------------

    Class: CTestEventController

    Method: CheckEventsL

    Description: Check all events.

    Parameters: None

    Return Values: TInt: Active event commands

    Errors/Exceptions: Leaves if CtlEventL leaves
                       Leaves if memory allocation fails
                       Leaves if unset event not found from pending 
                            state event list 

    Status: Approved

-------------------------------------------------------------------------------
*/  
TInt CTestEventController::CheckEventsL()
    {
    iEngine->Logger()->Log( _L( "CTestEventController::CheckEventsL" ) );

    iActiveEventCmds += CheckClientEventsL();
    iActiveEventCmds += CheckTestcaseEventsL();

    return iActiveEventCmds;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestEventController

    Method: CheckClientEventsL

    Description: Check client events.

    Parameters: None

    Return Values: TInt: Request of pending

    Errors/Exceptions: Leaves if CtlEventL leaves
                       Leaves if memory allocation fails
                       Leaves if unset event not found from pending 
                            state event list 

    Status: Approved

-------------------------------------------------------------------------------
*/  
TInt CTestEventController::CheckClientEventsL()
    {
    iEngine->Logger()->Log( _L( "CTestEventController::CheckClientEventsL" ) );
    TInt reqPending = 0;

    // Check client event requests
    TInt count = iEngine->ClientEvents().Count();
    for( TInt index = 0; index < count; index++ )
        {
        if( iEngine->ClientEvents()[index]->Name() == iEvent.Name() )
            {
            TEventMsg* event = iEngine->ClientEvents()[index]; 
            if( iEvent.Type() == TEventIf::ESetEvent )
                {
                // Set found event 
                event->Set( iEvent.EventType() );
                }
            else if( iEvent.Type() == TEventIf::EUnsetEvent )
                {
                // Unset found event
                // Create callback for change notifier
                TCallBack callBack( EventCallBack, this );
                CCallBack* eventCallBack = new (ELeave)CCallBack( callBack,
                                                    CActive::EPriorityHigh );
                CleanupStack::PushL( eventCallBack );
                
                event->Unset( eventCallBack->Status() ); 
                reqPending++;
                eventCallBack->SetActive();
                User::LeaveIfError( iEventCallBacks.Append( eventCallBack ) );
                CleanupStack::Pop( eventCallBack );
                }       
            break;
            }
        }

    return reqPending;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestEventController

    Method: CheckTestcaseEventsL

    Description: Check testcase events.

    Parameters: None

    Return Values: TInt: Request of pending

    Errors/Exceptions: Leaves if CtlEventL leaves
                       Leaves if memory allocation fails
                       Leaves if unset event not found from pending 
                            state event list 

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CTestEventController::CheckTestcaseEventsL()
    {
    TInt reqPending = 0;
    TCallBack callBack( EventCallBack, this );
    CCallBack* eventCallBack = NULL;

    // Then check testcase event requests
    TInt count = iEngine->TestCaseArray().Count();
    eventCallBack = new (ELeave)CCallBack( callBack, 
                                           CActive::EPriorityHigh );
    CleanupStack::PushL( eventCallBack );
    for ( TInt index = 0; index < count; index++ )
        {

        if( iEngine->TestCaseArray()[index]->CheckCtlEvent( iEvent ))
            {
            reqPending++;
            eventCallBack->SetActive();
            iEngine->TestCaseArray()[index]->CtlEvent( iEvent, 
                                                          eventCallBack->Status() );                                                          
            User::LeaveIfError( iEventCallBacks.Append( eventCallBack ) );
            CleanupStack::Pop( eventCallBack );
            eventCallBack = new (ELeave)CCallBack( callBack, 
                                                   CActive::EPriorityHigh );
            CleanupStack::PushL( eventCallBack );    
            }
        }
    CleanupStack::PopAndDestroy( eventCallBack );
    return reqPending;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestEventController

    Method: EventComplete

    Description: EventComplete handles completed event requests.

    Parameters: None

    Return Values: None

    Errors/Exceptions: 

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestEventController::EventComplete()
    {
    iEngine->Logger()->Log( _L( "CTestEventController::EventComplete" ));

    iActiveEventCmds--;
    
    if( iActiveEventCmds == 0 )
        {
        TInt error = KErrNone;
        TInt count = iEventCallBacks.Count();
        for( TInt i=0; i<count; i++ )
            {
            if( iEventCallBacks[i]->iStatus.Int() != KErrNone )
                {
                error = iEventCallBacks[i]->iStatus.Int();
                break;
                }
            }
        iEventCallBacks.ResetAndDestroy();
            
        // All event commands are completed
        if( iRequestStatus )
            {
            User::RequestComplete( iRequestStatus, error );
            }
        else
            {
            // No request was pending, complete immediately
            iMessage.Complete( error );
            }
        }

    }

/*
-------------------------------------------------------------------------------

    Class: CTestEventController

    Method: EventCallBack

    Description: static EventCallBack handles completed event requests.

    Parameters: TAny* aTestEventController: in: Test event controller

    Return Values: TInt: returns KErrNone

    Errors/Exceptions:None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CTestEventController::EventCallBack( TAny* aTestEventController )
    {
    CTestEventController* controller =
                                (CTestEventController*) aTestEventController;
    controller->EventComplete();
    return KErrNone;

    }
    
    
/*
-------------------------------------------------------------------------------

    DESCRIPTION

    This module contains implementation of CTestRemoteCmdNotifier class member
    functions.

-------------------------------------------------------------------------------
*/

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

    Class: CTestProgressNotifier

    Method: CTestRemoteCmdNotifier

    Description: Default constructor

    C++ default constructor can NOT contain any code, that
    might leave.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
CTestRemoteCmdNotifier::CTestRemoteCmdNotifier( CTestEngine* aEngine,
                             RTestExecution aTestExecution,
                             CTestCaseController* aTestCaseController,
                             CAtsLogger& aAtsLogger ) :
    CActive( CActive::EPriorityStandard ),
    iEngine( aEngine ),
    iTestExecution( aTestExecution ),
    iState( ETestProgressIdle ),
    iRemoteTypePckg( iRemoteType ),
    iMsgSizePckg( iMsgSize ),
    iTestCaseController( aTestCaseController ),
    iAtsLogger( aAtsLogger )
    {
    CActiveScheduler::Add( this );

    }

/*
-------------------------------------------------------------------------------

    Class: CTestRemoteCmdNotifier

    Method: ConstructL

    Description: Symbian OS second phase constructor

    Symbian OS default constructor can leave.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
void CTestRemoteCmdNotifier::ConstructL( )
    {
    __TRACE( KVerbose, ( _L( "CTestRemoteCmdNotifier::ConstructL" ) ) );

    }

/*
-------------------------------------------------------------------------------

    Class: CTestRemoteCmdNotifier

    Method: NewL

    Description: Two-phased constructor.

    Parameters: CTestEngine* aEngine: in: Pointer to CTestEngine
                RTestExecution aTestExecution: in: Handle to RTestExecution

    Return Values: CTestRemoteCmdNotifier* : pointer to created object

    Errors/Exceptions: Leaves if construction of CTestRemoteCmdNotifier fails

    Status: Draft

-------------------------------------------------------------------------------
*/
CTestRemoteCmdNotifier* CTestRemoteCmdNotifier::NewL( CTestEngine* aEngine,
                             RTestExecution aTestExecution,
                             CTestCaseController* aTestCaseController,
                             CAtsLogger& aAtsLogger )
    {
    CTestRemoteCmdNotifier* self = new ( ELeave ) CTestRemoteCmdNotifier(
        aEngine, aTestExecution, aTestCaseController, aAtsLogger );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestRemoteCmdNotifier

    Method: ~CTestRemoteCmdNotifier

    Description: Destructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
CTestRemoteCmdNotifier::~CTestRemoteCmdNotifier()
    {
    __TRACE( KVerbose, ( _L( "CTestRemoteCmdNotifier::~CTestRemoteCmdNotifier" ) ) );
    Cancel();

    delete iReceivedMsg;
    iReceivedMsg = 0;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestRemoteCmdNotifier

    Method: EnableReceive

    Description: Prepare to start active object

    Parameters: const RMessage& aMessage: in: Server message

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
void CTestRemoteCmdNotifier::EnableReceive( const RMessage2& aMessage )
    {
    __TRACE( KVerbose, ( _L( "CTestRemoteCmdNotifier::EnableReceive" ) ) );
    
    iMessage = aMessage;
    iMessageAvail = ETrue;
    
    Start( aMessage );

    }
    
/*
-------------------------------------------------------------------------------

    Class: CTestRemoteCmdNotifier

    Method: GetReceivedMsg

    Description: Read received message

    Parameters: const RMessage& aMessage: in: Server message

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
void CTestRemoteCmdNotifier::GetReceivedMsg( const RMessage2& aMessage )
    {
    __TRACE( KVerbose, ( _L( "CTestRemoteCmdNotifier::GetReceivedMsg" ) ) );
    
    TInt ret = KErrNone;
    if( iReceivedMsg )
        {
        TRAP( ret, aMessage.WriteL( 0, iReceivedMsg->Des() ) );
        delete iReceivedMsg;
        iReceivedMsg = 0;
        }
    else
        {
        ret = KErrNotFound;
        }
        
    // Complete message
    aMessage.Complete( ret );
                
    }

/*
-------------------------------------------------------------------------------

    Class: CTestRemoteCmdNotifier

    Method: StartL

    Description: Start active object

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
void CTestRemoteCmdNotifier::Start( const RMessage2& aMessage )
    {
    // Check that this request is not pending!!
    __ASSERT_ALWAYS( iState != ETestProgressPending,
                    iEngine->PanicClient( EReqPending, aMessage ) );
    iState = ETestProgressPending;
    SetActive();
    // Start first phase of the remote command's operations
    iTestExecution.NotifyRemoteCmd( iRemoteTypePckg, 
                                    iMsgSizePckg, 
                                    iStatus );

    }

/*
-------------------------------------------------------------------------------

    Class: CTestRemoteCmdNotifier

    Method: CancelReq

    Description: Cancel the request.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
void CTestRemoteCmdNotifier::CancelReq()
    {
    if(iMessageAvail)
        {
        iMessageAvail = EFalse;
        iMessage.Complete( KErrCancel );
        }
    }

/*
-------------------------------------------------------------------------------

    Class: CTestRemoteCmdNotifier

    Method: RunL

    Description: RunL handles completed requests.

    Parameters: None

    Return Values: None

    Errors/Exceptions: Leaves if WriteL leaves
                       Leaves if iStatus is not KErrNone or KErrEof
                       Leaves are handled in RunError method

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestRemoteCmdNotifier::RunL()
    {
    __TRACE( KVerbose, ( _L( "CTestRemoteCmdNotifier::StartL: iStatus=[%d]" ), iStatus.Int() ) );

    User::LeaveIfError( iStatus.Int() );
    
    iState = ETestProgressCompleted;

    TInt ret( 0 );
    switch( iRemoteType )
        {
        case EStifCmdSend:             // "Sending"
            {
            if( ( iMessageAvail == EFalse ) ||
                ( iMsgSize <= 0 ) )
                { 
                User::Leave( KErrGeneral );
                }
            // Delete previous if exists
            delete iReceivedMsg;
            iReceivedMsg = 0;
            // Create new buffer 
            iReceivedMsg = HBufC8::NewL( iMsgSize );

            // Start second phase of the remote command's operations,
            // buffer is read with GetReceivedMsg
            TPtr8 tmp = iReceivedMsg->Des();
            ret = iTestExecution.ReadRemoteCmdInfo( tmp, iRemoteType );

            // Writing received info to UI
            iMessage.WriteL( 0, iRemoteTypePckg );
            iMessage.WriteL( 1, iMsgSizePckg );
            
            // Complete message
            iMessage.Complete( ret );
            iMessageAvail = EFalse;

            break;
            }
        case EStifCmdReboot:           // "Sending"
            {
            TRebootParams remoteType;
            TRebootParamsPckg remoteTypePckg( remoteType );
            // Start second phase of the remote command's operations
            ret = iTestExecution.ReadRemoteCmdInfo( remoteTypePckg, iRemoteType );
            __TRACE( KInit, ( CStifLogger::ERed, _L("REBOOT PHONE (type %d)" ), remoteType.aType ) );

            if( remoteType.aType == CTestModuleIf::EKernelReset )
                {
                __TRACE( KInit, ( _L("Rebooting with kernel reset" ) ) );
                __TRACE( KInit, ( _L("Kernel reset implementation is ongoing, trying different reset..." ) ) );
                }

#ifdef __WINS__
            __TRACE( KInit, ( _L("Rebooting with Process kill(WINS)" ) ) );
            RProcess thisProcess;
            //thisProcess.SetSystem( ETrue );
            thisProcess.Kill( KErrNone );
            thisProcess.Close();
#else // Hardware specific
            TInt r( KErrNone );
            __TRACE( KInit, ( _L("Rebooting with reset module(HW)" ) ) );
            r = DynamicResetModule( remoteType.aType );
            if( r != KErrNone )
                {
                __TRACE( KInit, ( CStifLogger::EError, _L("This reseting type is failed, trying different reset...")) );
                }
#endif // Hardware specific

            // if( !remoteType.aType == CTestModuleIf::EDefaultReset )
            // Note this change needs an error code tranceiver between reboot
            // module and engine. (If reboot fails return error code, if reboot
            // is default then kill process else error code returning)

            // Do process kill as a last option
            __TRACE( KInit, ( _L("Rebooting with Process kill" ) ) );
           
            RProcess thisProcess2;         
            thisProcess2.Kill( KErrNone );
            thisProcess2.Close();

            // If this text is shown in UI something is wrong and this needs some investigation.
            iEngine->ErrorPrint( 0, _L( "Reboot phone...E.g. disconnect battery!!!" ) );
            break;
            }
        case EStifCmdStoreState:       // "Sending"
            {
            if( iMessageAvail == EFalse )
                { 
                User::Leave( KErrGeneral );
                }
            TRebootStateParams remoteState;
            TRebootStateParamsPckg remoteStatePckg( remoteState );

            // Start second phase of the remote command's operations
            ret = iTestExecution.ReadRemoteCmdInfo( remoteStatePckg,
                                                    iRemoteType );

            // Get test case information(test module, test case file, etc.)
            TTestInfo testInfo;
            iTestCaseController->GetTestCaseInfo( testInfo );

            // Write state informations to the file
            iEngine->WriteRebootParams( testInfo, remoteState.aCode,
                                        remoteState.aName );
                                                        
            // Write ATS loggers buffers to drive
            iEngine->FlushAtsLogger();

            // Pause test cases that there cannot make e.g. new store state
            // calls. iCaseNumber is index type value so increment by one to
            // get current test case.
            iEngine->PauseAllTestCases();
            // Resume current test case
            iTestCaseController->iTestExecution.Resume();

            // Writing received info to UI
            iMessage.WriteL( 0, iRemoteTypePckg );

            // Complete message
            iMessage.Complete( ret );
            iMessageAvail = EFalse;
            break;
            }
        case EStifCmdGetStoredState:   // "Reading, this must be done with two phase"
            {
            // Get test case information(test module, test case file, etc.)
            TTestInfo testInfo;
            iTestCaseController->GetTestCaseInfo( testInfo );

            TGetRebootStoredParams remoteStoredState;
            // Read state informations from the file
            TInt errorCodeToClient = 
                iEngine->ReadRebootParams( testInfo, 
                                           remoteStoredState.aName,
                                           remoteStoredState.aCode );

            TGetRebootStoredParamsPckg remoteStoredPckg( remoteStoredState );

            // Start second phase of the remote command's operations
            ret = iTestExecution.ReadRemoteCmdInfo( remoteStoredPckg,
                                                    iRemoteType,
                                                    errorCodeToClient );
            
            Start( iMessage ); // Starts active object

            break;
            }
       case EStifCmdMeasurement:   // "Reading, this must be done with two phase"
            {
            TGetMeasurementOptions remoteMeasurementOptions;
            remoteMeasurementOptions.iOptions = iEngine->StifMeasurement();

            TGetMeasurementOptionsPckg remoteMeasurementPckg( remoteMeasurementOptions );

            // Start second phase of the remote command's operations
            ret = iTestExecution.ReadRemoteCmdInfo( remoteMeasurementPckg,
                                                    iRemoteType,
                                                    KErrNone );
            
            Start( iMessage ); // Starts active object

            break;
            }

        case EStifCmdReceive:          // "Reading"
        default:
            User::Leave( KErrNotFound );
            break;
        }
    
    }

/*
-------------------------------------------------------------------------------

    Class: CTestRemoteCmdNotifier

    Method: DoCancel

    Description: Cancel active request

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
void CTestRemoteCmdNotifier::DoCancel()
    {
    switch ( iState )
        {
        case ETestProgressPending:
            iTestExecution.CancelAsyncRequest( ETestExecutionNotifyRemoteCmd );
            //iMessage.Complete( KErrCancel );
            break;
        case ETestProgressIdle:
        case ETestProgressCompleted:
        default:
            // DoCancel called in wrong state => Panic
            _LIT( KTestRemoteCmdNotifier, "CTestRemoteCmdNotifier" );
            User::Panic( KTestRemoteCmdNotifier, EDoCancelDisorder );
            break;
        }
    iState = ETestProgressIdle;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestRemoteCmdNotifier

    Method: RunError

    Description: Handle errors.

    Parameters: TInt aError: in: Symbian OS error: Error code

    Return Values: TInt KErrNone: Always returned KErrNone

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
TInt CTestRemoteCmdNotifier::RunError( TInt aError )
    {
    // Complete message with error
    if(iMessageAvail)
        {
        iMessageAvail = EFalse;
        iMessage.Complete( aError );
        }
    
    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestRemoteCmdNotifier

    Method: ResetL

    Description: Reset HW/WINS. Loads dynamically reset module by name.

    Parameters: CTestModuleIf::TRebootType aResetType: in: Reset type

    Return Values: TInt: Symbian error code.

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
TInt CTestRemoteCmdNotifier::DynamicResetModule( 
                                        CTestModuleIf::TRebootType aResetType )
    {
    __TRACE( KInit, (  _L( "DynamicResetModule()" ) ) );
    RLibrary resetModule;
    // Load the module
    TPtrC dllName;
    dllName.Set( iEngine->GetDeviceResetDllName() );
    // Loading should work with and without '.dll' extension.
    TInt r = resetModule.Load( dllName );
    if ( r != KErrNone )
        {
        __TRACE( KError, ( CStifLogger::EError, _L("Can't initialize reset module[%S], code = %d"), &dllName, r ) );
        return KErrNotFound;
        }
    else
        {
        // Print reset module name
        __TRACE( KInit, (  _L("Loaded reset module[%S]"), &dllName ) );
        }

    

    // Get pointer to first exported function
    CTestInterfaceFactory libEntry;
    libEntry = (CTestInterfaceFactory) resetModule.Lookup( 1 );
    if ( libEntry == NULL )
        {
        // New instance can't be created
        __TRACE ( KError, ( CStifLogger::EError, _L("Can't initialize reset module, NULL libEntry" ) ) );
        return KErrNoMemory;
        }
    else
        {
        __TRACE ( KInit, ( _L("Pointer to 1st exported received")));
        }

    CStifHWReset* reset;
    reset = NULL;

    // initialize test module
    __TRACE ( KVerbose, (_L("Calling 1st exported at 0x%x"), (TUint32) libEntry ));
    TRAPD ( err, reset =  (*libEntry)() );

     // Handle leave from test module
    if ( err != KErrNone )
        {
        __TRACE (KError, ( CStifLogger::EError, _L("Leave when calling 1st exported function, code %d"), err));
        return err;
        }
    else if ( reset == NULL )     // Handle NULL from test module init
        {
        __TRACE (KError, ( CStifLogger::EError, _L("NULL pointer received when constructing test module")));
        delete reset;

        // Set error codes
        return KErrNoMemory;
        }
    else
        {
        __TRACE (KInit, (_L("Entrypoint successfully called, test module instance at 0x%x"), (TUint32)reset ) );
        }

    // Calls dynamically loaded reset module's method.
    TInt ret = reset->DoReset( aResetType );
    if( ret != KErrNone )
        {
        __TRACE (KInit, (_L("DynamicResetModule; DoReset fails with error: %d"), ret ) );
        return ret;
        }

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    DESCRIPTION

    This module contains implementation of CTestCommandNotifier class member
    functions.

-------------------------------------------------------------------------------
*/

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

    Class: CTestCommandNotifier

    Method: CTestCommandNotifier

    Description: Default constructor

    C++ default constructor can NOT contain any code, that
    might leave.

    Parameters: CTestEngine* aEngine: in: Pointer to CTestEngine
                RTestExecution aTestExecution: in: Handle to RTestExecution

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CTestCommandNotifier::CTestCommandNotifier(CTestEngine* aEngine,
                                           RTestExecution aTestExecution):
    CActive(CActive::EPriorityStandard),
    iEngine(aEngine),
    iTestExecution(aTestExecution),
    iCommandPckg(iCommand)
    {
    CActiveScheduler::Add(this);
    }

/*
-------------------------------------------------------------------------------

    Class: CTestCommandNotifier

    Method: ConstructL

    Description: Symbian OS second phase constructor

    Symbian OS default constructor can leave.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestCommandNotifier::ConstructL( )
    {
    __TRACE(KVerbose, (_L("CTestCommandNotifier::ConstructL")));
    }

/*
-------------------------------------------------------------------------------

    Class: CTestCommandNotifier

    Method: NewL

    Description: Two-phased constructor.

    Parameters: CTestEngine* aEngine: in: Pointer to CTestEngine
                RTestExecution aTestExecution: in: Handle to RTestExecution

    Return Values: CTestCommandNotifier* : pointer to created object

    Errors/Exceptions: Leaves if construction of CTestCommandNotifier fails

    Status: Approved

-------------------------------------------------------------------------------
*/
CTestCommandNotifier* CTestCommandNotifier::NewL(CTestEngine* aEngine,
                                                 RTestExecution aTestExecution)
    {
    CTestCommandNotifier* self = new (ELeave) CTestCommandNotifier(aEngine, aTestExecution);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

/*
-------------------------------------------------------------------------------

    Class: CTestCommandNotifier

    Method: ~CTestCommandNotifier

    Description: Destructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CTestCommandNotifier::~CTestCommandNotifier()
    {
    __TRACE(KVerbose, (_L("CTestEventNotifier::~CTestEventNotifier")));
    Cancel();
    }

/*
-------------------------------------------------------------------------------

    Class: CTestCommandNotifier

    Method: StartL

    Description: Start active object

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestCommandNotifier::Start()
    {
    __TRACE(KVerbose, (_L("CTestEventNotifier::StartL")));

    iTestExecution.NotifyCommand2(iCommandPckg, iParamsPckg, iStatus, KErrNone);
    SetActive();
    }

/*
-------------------------------------------------------------------------------

    Class: CTestCommandNotifier

    Method: RunL

    Description: RunL handles completed requests.

    Parameters: None

    Return Values: None

    Errors/Exceptions: Leaves if iStatus is not KErrNone
                       Leaves if iState is not ETestEventPending
                       Leaves if some leaving method called here leaves

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestCommandNotifier::RunL()
    {
    __TRACE(KVerbose, (_L("CTestCommandNotifier::RunL: iStatus=[%d]"), iStatus.Int()));

    User::LeaveIfError(iStatus.Int());

    iEngine->ExecuteCommandL(iCommand, iParamsPckg);

    // Set request again
    Start();
    }

/*
-------------------------------------------------------------------------------

    Class: CTestCommandNotifier

    Method: DoCancel

    Description: Cancel active request

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestCommandNotifier::DoCancel()
    {
    __TRACE(KVerbose, (_L( "CTestEventNotifier::DoCancel")));

    iTestExecution.CancelAsyncRequest(ETestExecutionNotifyCommand);
    }

/*
-------------------------------------------------------------------------------

    Class: CTestCommandNotifier

    Method: RunError

    Description: Handle errors.

    Parameters: TInt aError: in: Symbian OS error: Error code

    Return Values: TInt KErrNone: Always returned KErrNone

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CTestCommandNotifier::RunError(TInt aError)
    {
    __TRACE(KError, (CStifLogger::ERed, _L("CTestCommandNotifier::RunError %d"), aError));
    return KErrNone;
    }


// ================= OTHER EXPORTED FUNCTIONS =================================

// None

// End of File
