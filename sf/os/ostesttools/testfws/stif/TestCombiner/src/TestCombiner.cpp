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
* Description: This module contains the implementation of 
* CTestCombiner class member functions.
*
*/

// INCLUDE FILES
#include <StifTestEventInterface.h>
#include <StifLogger.h>
#include "TestCombiner.h"
#include "TestKeywords.h"
#include "TestCase.h"
#include "TestCombinerEvent.h"
#include "Logging.h"
//--PYTHON-- begin
#include "StifPython.h"
#include "StifPythonFunComb.h"
//--PYTHON-- end
#include "SettingServerClient.h"

// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES  
// None

// CONSTANTS
_LIT( KTestRunner, "CTestRunner" );
_LIT( KRemoteTimer, "CRemoteTimer" );
_LIT( KRemoteReceiver, "CRemoteReceiver" );

// MACROS
#ifdef LOGGER
#undef LOGGER
#endif
#define LOGGER iLog

// LOCAL CONSTANTS AND MACROS
_LIT(KTitle, "title");
_LIT(KTimeout, "timeout");
_LIT(KPriority, "priority");

//printing macros
 _LIT( KExecute, "Execute");
 
// MODULE DATA STRUCTURES
// None

// LOCAL FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// ==================== LOCAL FUNCTIONS =======================================
// None

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

     Class: CTestCombiner

     Method: CTestCombiner

     Description: Default constructor

     C++ default constructor can NOT contain any code, that
     might leave.
     
     Parameters:    None

     Return Values: None

     Errors/Exceptions: None

     Status: Approved
    
-------------------------------------------------------------------------------
*/
CTestCombiner::CTestCombiner(): 
    iSchedulerActive( EFalse ),
    iRunningTests( 0 ), 
    iResult( KErrNone ),
    iEventPckg( iEvent ),
    iRemoteTimeout( KRemoteProtocolTimeout ),
    iCancelIfError( EFalse ),
    iFailedTestCase( 0 ),
    iLoopIsUsed( EFalse )
    {

    }

/*
-------------------------------------------------------------------------------

     Class: CTestCombiner

     Method: ConstructL

     Description: Symbian OS second phase constructor

     Symbian OS default constructor can leave.

     Parameters:    None

     Return Values: None

     Errors/Exceptions: None.

     Status: Approved

-------------------------------------------------------------------------------
*/
void CTestCombiner::ConstructL()
    {
    // TRAPed to avoid crashing if logfile creation fails,
    // so we can run test without logging to file
    TRAPD( err, iStdLog = CStifLogger::NewL( KTestCombinerLogDir, 
                                      KTestCombinerLogFile );
        );
    if( err != KErrNone )
        {
        iStdLog = NULL;
        __RDEBUG( (_L("Creating logfile failed") ) );
        }  
    iLog = iStdLog;

    //Read logger settings to check whether test case name is to be
    //appended to log file name.
    RSettingServer settingServer;
    TInt ret = settingServer.Connect();
    if(ret != KErrNone)
        {
        User::Leave(ret);
        }
    // Struct to StifLogger settigs.
    TLoggerSettings loggerSettings; 
    // Parse StifLogger defaults from STIF initialization file.
    ret = settingServer.GetLoggerSettings(loggerSettings);
    if(ret != KErrNone)
        {
        User::Leave(ret);
        }

    // Get engine settings
    TEngineSettings engineSettings;
    ret = settingServer.GetEngineSettings(engineSettings);
    if(ret != KErrNone)
        {
        __TRACE(KError, (_L("Could not retrieve engine settings from SettingServer [%d]"), ret));
        iUITestingSupport = EFalse;
        iSeparateProcesses = EFalse;
        }
        else
        {
        iUITestingSupport = engineSettings.iUITestingSupport;
        iSeparateProcesses = engineSettings.iSeparateProcesses;
        __TRACE(KMessage, (_L("Engine settings retrieved from SettingServer. UITestingSupport [%d] SeparateProcesses [%d]"), iUITestingSupport, iSeparateProcesses));
        }

    // Close Setting server session
    settingServer.Close();
    iAddTestCaseTitleToLogName = loggerSettings.iAddTestCaseTitle;

    iIndexTestModuleControllers = 1;
    
    __TRACE( KPrint, ( _L("New TestCombiner") ) );

    }

/*
-------------------------------------------------------------------------------

     Class: CTestCombiner

     Method: NewL

     Description: Two-phased constructor.
          
     Parameters:    None

     Return Values: CTestCombiner*: new object

     Errors/Exceptions: Leaves if new or ConstructL leaves.

     Status: Approved
    
-------------------------------------------------------------------------------
*/
CTestCombiner* CTestCombiner::NewL()
    {
     
    CTestCombiner* self = new (ELeave) CTestCombiner();
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
     
    }

/*
-------------------------------------------------------------------------------

     Class: CTestCombiner

     Method: ~CTestCombiner

     Description: Destructor
     
     Parameters:    None

     Return Values: None

     Errors/Exceptions: None

     Status: Approved
    
-------------------------------------------------------------------------------
*/
CTestCombiner::~CTestCombiner()
    {
    iLoopAllocationArray.Reset();
    iLoopAllocationArray.Close();
    iLoopIsUsed = EFalse;

    iTestCases.ResetAndDestroy();
    iTestModules.ResetAndDestroy();
    iEventArray.ResetAndDestroy();
    iSlaveArray.ResetAndDestroy();
    iSendReceive.ResetAndDestroy();

    iTestCases.Close();
    iTestModules.Close();
    iEventArray.Close();
    iSlaveArray.Close();
    iSendReceive.Close();

    // Stop all remaining measurement modules
    const TInt count_meas = iTestMeasurementArray.Count();
    for( TInt b = 0; b < count_meas; b++ )
        {
        iTestMeasurementArray[b]->iMeasurement->Stop();
        }
    iTestMeasurementArray.ResetAndDestroy();
    iTestMeasurementArray.Close();

    delete iSectionParser;
    delete iRemoteReceiver;
    delete iTestRunner;

    iLog = NULL;
    delete iStdLog;
    iStdLog = NULL;
    delete iTCLog;
    iTCLog = NULL;
    }

/*
-------------------------------------------------------------------------------

     Class: CTestCombiner

     Method: InitL

     Description: InitL is used to initialize the Test Module.

     Parameters: const TFileName& aIniFile: in: Initialization file
                 TBool aFirstTime: in: First time flag 

     Return Values: Symbian OS error code

     Errors/Exceptions: Leaves if ReadInitializationL leaves

     Status: Draft

-------------------------------------------------------------------------------
*/
TInt CTestCombiner::InitL( TFileName& aIniFile, 
                           TBool /*aFirstTime*/ )
    {

    __TRACEFUNC();

    if( aIniFile.Length() > 0 )
        {
        // Read initialization from test case file
        ReadInitializationL( aIniFile );
        }

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

     Class: CTestCombiner

     Method: GetTestCases

     Description: GetTestCases is used to inquired test cases

     Parameters:    const TFileName& aConfigFile: in: Config file name
                    RPointerArray<RTestEngine::TTestCaseInfo>& aTestCases: out: 
                          Array of TestCases 

     Return Values: KErrNone: Success
                    KErrNotFound: Testcases not found

     Errors/Exceptions: Leaves if CStifParser::NewL leaves
                        Leaves if CStifParser::SectionL leaves
                        Leaves if CStifParser::NextSectionL leaves
                        Leaves if memory allocation fails

     Status: Approved

-------------------------------------------------------------------------------
*/
TInt CTestCombiner::GetTestCasesL( const TFileName& aConfigFile, 
                                   RPointerArray<TTestCaseInfo>& aTestCases )
    {
    __TRACEFUNC();

    if( aConfigFile.Length() == 0 )
        {
        __TRACE( KError, (_L("No test case script file given") ) );
        __RDEBUG( (_L("No test case script file given") ) );
        return KErrNotFound;
        }

    CStifParser* parser = NULL;
    TRAPD( err, parser = 
        CStifParser::NewL( _L(""), aConfigFile, CStifParser::ECStyleComments ) );
    if( err != KErrNone )
        {
        __TRACE( KError, (_L("Test case script file not found") ) );
        __RDEBUG( (_L("Test case script file not found") ) );
        return KErrNotFound;
        }
    CleanupStack::PushL( parser );
    CStifSectionParser* section;
    TPtrC tmp;
    TInt index = 0;
    TInt ret = KErrNone;

    section = parser->SectionL( KTestStartTag, KTestEndTag );
    if( section == NULL )
        {
        ret = KErrNotFound;
        }
    else
        {    
        while( section )
            {
            CleanupStack::PushL( section );
             
            if( section->GetLine( KTitle,  tmp, ENoTag ) == KErrNone )
                 {
                 if( tmp.Length() > KMaxName )
                    {
                    tmp.Set( tmp.Left( KMaxName ) );
                    }                    
                 TTestCaseInfo* tc = new ( ELeave ) TTestCaseInfo();
                 CleanupStack::PushL( tc );
                 __TRACE( KVerbose, (_L("TestCase: %S"), &tmp));
                 tc->iTitle.Copy( tmp );
                 tc->iCaseNumber = ++index;
                 CStifItemParser* item = section->GetItemLineL( KTimeout );
                 if( item )
                    {
                    TInt timeout; // In milliseconds
                    ret = item->GetInt( KTimeout, timeout ); 
                    if( ret != KErrNone )
                        {
                        __TRACE( KError, (_L("Illegal timeout")));
                        User::Leave( ret ); 
                        }
                    tc->iTimeout = (TInt64)timeout*1000;
                    __TRACE( KMessage, (_L("Timeout: %i"), tc->iTimeout.Int64() ));
                    }
                 item = section->GetItemLineL( KPriority );
                 if( item )
                    {
                    ret = item->GetInt( KPriority, tc->iPriority ); 
                    if( ret != KErrNone )
                        {
                        TPtrC priority;
                        ret = item->GetString( KPriority, priority );
                        if( ret != KErrNone )
                            {
                            __TRACE( KError, (_L("Illegal priority")));
                            User::Leave( ret ); 
                            }
                        switch( TTCKeywords::Parse( priority, 
                                                    TTCKeywords::Priority ) )
                            {
                            case TTCKeywords::EPriHigh:
                                tc->iPriority = TTestCaseInfo::EPriorityHigh;
                                break;
                            case TTCKeywords::EPriNormal:
                                tc->iPriority = TTestCaseInfo::EPriorityNormal;
                                break;
                            case TTCKeywords::EPriLow:
                                tc->iPriority = TTestCaseInfo::EPriorityLow;
                                break;
                            default:
                                __TRACE( KError, (_L("Illegal priority")));
                                User::Leave( KErrArgument ); 
                            }
                        }
                    __TRACE( KMessage, (_L("Priority: %i"), tc->iPriority ));
                    }

                 aTestCases.Append(tc);
                 CleanupStack::Pop( tc );
                 }
            CleanupStack::PopAndDestroy( section );
            section = parser->NextSectionL( KTestStartTag, KTestEndTag );
            }
        }

    CleanupStack::PopAndDestroy( parser );

    __TRACE( KPrint, (  _L( "Configfile '%S', testcases %d" ),
        &aConfigFile, index ));

    return ret;

    }

/*
-------------------------------------------------------------------------------

     Class: CTestCombiner

     Method: RunTestCaseL

     Description: Run a specified testcase.

     RunTestCaseL is used to run an individual test case specified 
     by aTestCase. 

     Parameters:    const TInt aCaseNumber: in: Testcase number 
                    const TFileName& aConfig: in: Configuration file name
                    TTestResult& aResult: out; test case result

     Return Values: KErrNone: Test case started succesfully.
                    KErrNotFound: Testcase not found
                    KErrUnknown: Unknown TestCombiner error
                    Any other SymbianOS error

     Errors/Exceptions: Leaves if GetTestCaseL leaves
                        Leaves if RunTestL leaves
                        Leaves if memory allocation fails

     Status: Approved

-------------------------------------------------------------------------------
*/
TInt CTestCombiner::RunTestCaseL( const TInt aCaseNumber, 
                                  const TFileName& aConfig, 
                                  TTestResult& aResult )
    {
    __TRACEFUNC();

    __TRACE( KMessage, (_L("***Testcase started***")));

    //Open new log file with test case title in file name
    if(iAddTestCaseTitleToLogName)
        {
        //Close test case log if exists
        if(iTCLog)
            {
            delete iTCLog;
            iTCLog = NULL;
            }
        TFileName logFileName;
        TName title;
        TestModuleIf().GetTestCaseTitleL(title);
        
        logFileName.Format(KTestCombinerLogFileWithTitle, &title);
        iTCLog = CStifLogger::NewL(KTestCombinerLogDir, logFileName);
        iLog = iTCLog;
        }

    /* 
    * Allow memory leaks, because Defines are leaved untouched 
    * after test case execution. Real memory leaks are detected with 
    * UHEAP macros.
    */
    TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksMem );

    // Read initialization from test case file
    ReadInitializationL( aConfig );

    __UHEAP_MARK;
        
    iSectionParser = GetTestCaseL( aCaseNumber, aConfig );

    // Check parsing result
    if( iSectionParser == NULL )
        {
        __TRACE( KError, (_L("***Parsing testcase failed***")));
        __UHEAP_MARKEND;
        return KErrNotFound;
        }

    CActiveScheduler* activeScheduler = 
        new ( ELeave ) CActiveScheduler();
    CleanupStack::PushL( activeScheduler );
    CActiveScheduler::Install( activeScheduler );

    // Resetting these variables to normal state @js             
    iFailedTestCase = 0;    
    iCancelIfError = EFalse;
    iScriptFailed = KErrNone;
    iScriptFailedDescription.Copy(KNullDesC);

    // Run the given testcase described in iSectionParser section
    RunTestL();

    CleanupStack::PopAndDestroy( activeScheduler );

    delete iSectionParser;
    iSectionParser = NULL;

    TInt ret = KErrNone;
    // Check if test case starting failed
    if( iResult != KErrNone )
        {
        TestModuleIf().Printf( KPrintPriHigh, _L("Result"), _L("Starting FAILED"));
         __TRACE( KError, (_L("***Starting testcase FAILED***")));
        ret = iResult;
        aResult.iResult = -2;
        aResult.iResultDes = _L("Testcase script execution failed");
        }
    else if(iScriptFailed != KErrNone)
        {
        TestModuleIf().Printf( KPrintPriHigh, _L("Result"), _L("Script execution FAILED"));
        __TRACE( KError, (_L("***Sscript execution FAILED***")));
        aResult.iResult = iScriptFailed;
        if(iScriptFailedDescription != KNullDesC)
            {
            aResult.iResultDes.Copy(iScriptFailedDescription);
            }
        else
            {
            aResult.iResultDes = _L("Testcase script execution failed");
            }
        }
    else
        {
        __TRACE( KPrint, (_L("***Testcase completed***")));
              
        // Testcombiner succeeded to start all testcases, 
        // check individual testcase results
        TInt count = iTestCases.Count();
        TInt i = iFailedTestCase;
        // iFailedTestCase is either 0 or the number of failed test case if 
        // canceliferror was given and a test case has failed

        for(; i < count; i++ )
            {
            // All cases should be completed now
            if( iTestCases[i]->State() != CTCTestCase::ETestCaseCompleted )
                {
                // This is some unknown internal TestCombiner error
                // Should not happen
                __TRACE( KError, (_L("TestCase (%S) not completed"), 
                    &iTestCases[i]->TestId() ));
                ret = KErrUnknown;
                break;
                }

            // Interpret execution result type from returned result
            TInt executionResult = TFullTestResult::ECaseExecuted; // Ok
            if( iTestCases[i]->iResult.iCaseExecutionResultType >= 
                TFullTestResult::ECaseLeave )
                {
                // Some abnormal execution result type
                executionResult = iTestCases[i]->iResult.iCaseExecutionResultType;
                }

            // Check expected execution result type
            if( executionResult != iTestCases[i]->ExpectedResultCategory() ) 
                {
                // expected and returned result types differ
                aResult.iResult = KErrGeneral;
                aResult.iResultDes.Copy( _L("Test case completed with ") );
                aResult.iResultDes.Append( 
                    TTCKeywords::ResultCategory( 
                    iTestCases[i]->iResult.iCaseExecutionResultType ));
                aResult.iResultDes.Append( _L(" and expected was ") );
                aResult.iResultDes.Append( 
                    TTCKeywords::ResultCategory( 
                    iTestCases[i]->ExpectedResultCategory() ));
                AppendTestResultToResultDes(aResult.iResultDes, iTestCases[i]->iResult.iTestResult.iResultDes);
                __TRACE( KPrint, ( _L( "%S"), &aResult.iResultDes ) );
                break;
                }

            // Check normal test result
            if( iTestCases[i]->ExpectedResultCategory() == 
                TFullTestResult:: ECaseExecuted )
                {
                // Normal completion, check result
                if( iTestCases[i]->iResult.iTestResult.iResult != 
                      iTestCases[i]->ExpectedResult() )
                    {
                     __TRACE( KPrint, ( _L( "Test failed, expect(%d) != result(%d)"), 
                         iTestCases[i]->ExpectedResult(),
                         iTestCases[i]->iResult.iTestResult.iResult ));
                    // We return the first error result as aResult
                    if( iTestCases[i]->iResult.iTestResult.iResult != KErrNone )
                        {
                        aResult = iTestCases[i]->iResult.iTestResult;
                        }
                    else
                        {
                        aResult.iResult = KErrGeneral;
                        aResult.iResultDes.Copy( _L("Test case completed with KErrNone and expected "));
                        aResult.iResultDes.AppendNum( iTestCases[i]->ExpectedResult() );
                        AppendTestResultToResultDes(aResult.iResultDes, iTestCases[i]->iResult.iTestResult.iResultDes);
                        }
                    break;
                    }
                }
            else 
                {
                // Abnormal completion, i.e. panic, leave, exception or timeout
                if( iTestCases[i]->iResult.iCaseExecutionResultCode != 
                      iTestCases[i]->ExpectedResult() )
                    {
                    __TRACE( KPrint, ( _L( "Test failed, expect errorcode(%d) != result(%d)"), 
                         iTestCases[i]->ExpectedResult(),
                         iTestCases[i]->iResult.iCaseExecutionResultCode ) );
                    // We return the first error result as aResult
                    aResult = iTestCases[i]->iResult.iTestResult;
                    // override result with real error code
                    aResult.iResult = iTestCases[i]->iResult.iCaseExecutionResultCode;
                    break;
                    }
                }
            }
        if( i == count )
            {
            TestModuleIf().Printf( KPrintPriHigh, _L("Result"), _L("PASSED"));
            __TRACE( KPrint, (_L("***Test case result: PASSED***")));
            aResult.iResult = KErrNone;
            aResult.iResultDes.Copy( _L("Test case succeeded") );
            }
        else
            {
            TestModuleIf().Printf( KPrintPriHigh, _L("Result"), _L("FAILED"));        
            __TRACE( KPrint, (_L("***Test case result: FAILED***")));
            }
        }

    // Release all pending event requests
    TEventIf event( TEventIf::ERelEvent );
    TInt eventCount = iEventArray.Count();
    for( TInt ind=0; ind < eventCount; ind++ )
        {
        event.SetName( iEventArray[ind]->Name() );
        TestModuleIf().Event( event );
        }

    iLoopAllocationArray.Reset();
    iLoopAllocationArray.Close();
    iLoopIsUsed = EFalse;

    // Delete all completed and checked testcases
    // Do not change the deletion order of events and test cases!!!
    iEventArray.ResetAndDestroy();
    iTestCases.ResetAndDestroy();
    iTestModules.ResetAndDestroy();
    iSlaveArray.ResetAndDestroy();

    iSendReceive.ResetAndDestroy();

    // Stop all remaining measurement modules
    const TInt count_meas = iTestMeasurementArray.Count();
    for( TInt b = 0; b < count_meas; b++ )
        {
        iTestMeasurementArray[b]->iMeasurement->Stop();
        }
    iTestMeasurementArray.ResetAndDestroy();
    iTestMeasurementArray.Close();

    __UHEAP_MARKEND;

    //If log was replaced then restore it
    if(iAddTestCaseTitleToLogName)
        {
        iLog = iStdLog;
        delete iTCLog;
        iTCLog = NULL;
        }

    return ret;

    }

/*
-------------------------------------------------------------------------------

     Class: CTestCombiner

     Method: GetTestCaseL

     Description: Get specified test case section from configfile.

     Parameters:    const TInt aCaseNumber: in: Test case number
                    const TFileName& aConfig: in: Configfile name 

     Return Values: CSectionParser*: pointer to test case section

     Errors/Exceptions: Leaves if CStifParser::NewL leaves
                        Leaves if CStifParser::SectionL leaves
                        Leaves if memory allocation fails

     Status: Approved

-------------------------------------------------------------------------------
*/
CStifSectionParser* CTestCombiner::GetTestCaseL( const TInt aCaseNumber, 
                                             const TFileName& aConfig )
    {
    __TRACEFUNC();
    CStifParser* parser = 
        CStifParser::NewL( _L(""), aConfig, CStifParser::ECStyleComments );
    CleanupStack::PushL( parser );

    CStifSectionParser* section = 
        parser->SectionL( KTestStartTag, KTestEndTag, aCaseNumber );

    CleanupStack::PopAndDestroy( parser );
    return section;
    }

/*
-------------------------------------------------------------------------------

     Class: CTestCombiner

     Method: ReadInitializationL

     Description: Read initialization from file.

     Parameters:    const TDesC& aIniFile: in: File that contains initialization

     Return Values: None

     Errors/Exceptions: None

     Status: Draft

-------------------------------------------------------------------------------
*/
void CTestCombiner::ReadInitializationL( const TDesC& aIniFile )
    {
    CStifParser* parser = NULL; 

    // Open file
    TRAPD( err, parser = 
        CStifParser::NewL( _L(""), aIniFile, CStifParser::ECStyleComments ) );
    if( ( err == KErrNone ) && parser )
        {
        CleanupStack::PushL( parser );
        __TRACE( KMessage, (_L("Read initialization from [%S]"),
            &aIniFile ) );
        
        // Read initialization parameters 
        CStifSectionParser* section = parser->SectionL( KInitStartTag,
                                                        KInitEndTag );
        if( section  )
            {
            CleanupStack::PushL( section );
            __TRACE( KMessage, (_L("Read initializations")) );

            // Read RCP timeout            
            CStifItemParser* item = section->GetItemLineL( KInitRcpTimeout );
            if( item != NULL )
                {
                __TRACE( KMessage, (_L("Got RCP timeout definition")) );
                CleanupStack::PushL( item );
                
                TInt value;
                if( item->GetInt( KInitRcpTimeout, value ) 
                        != KErrNone )
                    {
                    __TRACE( KError, (_L("No RCP timeout value given")) );
                    User::Leave( KErrGeneral );                    
                    } 
                // Change from seconds to milliseconds
                iRemoteTimeout = value*1000*1000;
                CleanupStack::PopAndDestroy( item );
                }

            CleanupStack::PopAndDestroy( section );
            }
           
        // Read defines
        section = parser->SectionL( KDefineStartTag, KDefineEndTag );
        while(section)
            {
            CleanupStack::PushL( section );
            __TRACE( KMessage, (_L("Read defines")) );

            TPtrC name;
            TPtrC value;
            CStifItemParser* item = section->GetItemLineL( _L("") );
            while( item )
                {
                CleanupStack::PushL( item );
                
                if( item->GetString( _L(""), name ) != KErrNone )
                    {
                    __TRACE( KError, (_L("No define name given")) );
                    User::Leave( KErrGeneral );
                    } 
                if( item->Remainder( value ) != KErrNone )
                    {
                    __TRACE( KError, (_L("No define value given")) );
                    User::Leave( KErrGeneral );
                    } 
                TInt count = iDefined.Count();
                TInt i = 0;
                for( ; i < count; i++ )
                    {
                    if( iDefined[i]->Name() == name )
                        {
                        __TRACE( KMessage, (_L("Update define %S:%S"), &name, &value ) );
                        // Update existing
                        iDefined[i]->SetValueL( value );
                        break;
                        }
                    }
                if( i == count) 
                    {
                    // New define, store it
                    CDefinedValue* define = CDefinedValue::NewL( name, value );
                    CleanupStack::PushL( define );
                    User::LeaveIfError( iDefined.Append( define ) );
                    CleanupStack::Pop( define );
                    }

                CleanupStack::PopAndDestroy( item );
                item = section->GetNextItemLineL();
                }
            CleanupStack::PopAndDestroy( section );
            section = parser->NextSectionL(KDefineStartTag, KDefineEndTag);
            }
        CleanupStack::PopAndDestroy( parser );
        }
    }

/*
-------------------------------------------------------------------------------

     Class: CTestCombiner

     Method: RunTestL

     Description: Run a testcase specified by iSectionParser.

     Parameters:    None
     
     Return Values: None.

     Errors/Exceptions: Leaves if CSectionParser::GetItemLineL leaves
                        Leaves if CTestRunner::NewL leaves
                        Leaves if memory allocation fails                                

     Status: Approved
    
-------------------------------------------------------------------------------
*/
void CTestCombiner::RunTestL()
    {
    __TRACEFUNC();
    iResult = KErrNone;

    TPtrC line;
    // "title" keyword must be in the first line
    User::LeaveIfError( iSectionParser->GetLine( KTitle, line, ENoTag ) );
     
    iTestRunner = CTestRunner::NewL( this ); 
    iRemoteReceiver = CRemoteReceiver::NewL( this );
    iRemoteReceiver->Start();

    __TRACE( KMessage, (_L("Run: %S"), &line));
    
    TestModuleIf().Printf( KPrintPriNorm, _L("Run"), _L("%S"), &line);
    // Rest of the job is done by test runner
    iTestRunner->SetRunnerActive();

    // Start activeScheduler looping testcase lines
    iSchedulerActive = ETrue;
    __TRACE( KMessage, (_L("Start CActiveScheduler")));
    CActiveScheduler::Current()->Start();

    TestModuleIf().Printf( KPrintPriNorm, _L("Executed"), _L("%S"), &line);
    __TRACE( KMessage, (_L("Executed: %S"), &line));
        
    delete iRemoteReceiver;
    iRemoteReceiver = NULL;
    delete iTestRunner;
    iTestRunner = NULL;

    }     

/*
-------------------------------------------------------------------------------

     Class: CTestCombiner

     Method: GetTestModuleL

     Description: Load testmodule if not already loaded, otherwise return
                  description of the loaded testmodule.

     Parameters:    TDesC& aModule: in: Module name.
                    TDesC& aIniFile: in: Ini file name.
                    const TDesC& aConfigFile: in: Test case(config) file name.
     
     Return Values: CTCTestModule*: pointer to testmodules description
     
     Errors/Exceptions: Leaves if CTCTestModule::NewL leaves
                        Leaves if RPointerArray::Append fails  

     Status: Proposal
    
-------------------------------------------------------------------------------
*/
CTCTestModule* CTestCombiner::GetTestModuleL( TDesC& aModule,
                                              TDesC& aIniFile,
                                              const TDesC& aConfigFile )
    {
    __TRACEFUNC();
    TInt count = iTestModules.Count();
    TInt i, j;
    TInt counttc;
    TInt running;
    TBool newWay = EFalse;
    
    __TRACE(KMessage, (_L("Find test module for [%S]"), &aModule));

    //Name for new module controller
    TName newModuleName = aModule;

    //Check if it is python case or UITestingSupport or SeparateProcesses mode
    newWay = (aModule.Find(KPythonScripter) == 0)
             || (iUITestingSupport && aModule.Find(KTestScripterName) == 0)
             || (iSeparateProcesses); 

    if(newWay)
        {
        __TRACE(KMessage, (_L("TCTestModule operating mode: exclusive")));

        //Find module of given name
        __TRACE(KMessage, (_L("Looking for module [%S]"), &aModule));
        if(iUITestingSupport && aModule.Find(KTestScripterName) == 0)
            {
            __TRACE(KMessage, (_L("UITestingSupport option is on. New module controller will be created always")));
            }
        else
            {
            for(i = 0; i < count; i++)
                {
                TPtrC modName = iTestModules[i]->ModuleName();
                if(modName.Find(aModule) == 0)
                    {
                    //Check if there is running test case
                    counttc = iTestCases.Count();
                    running = 0;
                    for(j = 0; j < counttc; j++)
                        {
                        if(iTestCases[j]->TestModule() == iTestModules[i] && iTestCases[j]->State() != CTestCase::ETestCaseCompleted)
                            {
                            //We have found at least one running test case. There is no reason to find any more,
                            //because there shouldn't be because test module may run only one python scripter test case.
                            running++;
                            __TRACE(KMessage, (_L("Module controller found [%S], but it has running test cases"), &modName));
                            break;
                            }
                        }
                    if(running == 0)
                        {
                        __TRACE(KMessage, (_L("Free module controller has been found [%S]"), &modName));
                        return iTestModules[i];
                        }
                    }
                }
            }
        //Update name of new module to be created
        newModuleName.AppendFormat(_L("@%d_"), GetIndexForNewTestModuleController());
        newModuleName.LowerCase();
        }
    else
        {
        __TRACE(KMessage, (_L("TCTestModule operating mode: normal")));
        for(i = 0; i < count; i++)
            {
            if(iTestModules[i]->ModuleName() == aModule)
                {
                // Found test module, return description
                __TRACE(KMessage, (_L("Module controller [%S] found"), &aModule));
                return iTestModules[i];
                }
            }
        }

    __TRACE(KMessage, (_L("Creating new test module controller [%S] with ini [%S]"), &newModuleName, &aIniFile));
    CTCTestModule* module = CTCTestModule::NewL(this, newModuleName, aIniFile, aConfigFile);
    CleanupStack::PushL(module);

    //Enumerate test cases
    module->GetTestCasesForCombiner(aConfigFile);

    //Append new module to list
    User::LeaveIfError(iTestModules.Append(module));
    CleanupStack::Pop(module);

    //Log some info
    if(newWay)
        {    
        __TRACE(KMessage, (_L("Module [%S] added to list. Currently there are following controllers of [%S] type:"), &newModuleName, &aModule));
        j = 1;
        for(i = 0; i < iTestModules.Count(); i++)
            {
            if(iTestModules[i]->ModuleName().Find(aModule) == 0)
                {
                __TRACE(KMessage, (_L("    %d. [%S]"), j, &iTestModules[i]->ModuleName()));
                j++;
                }
            }
        }

    if( iLoopIsUsed )
        {
        User::LeaveIfError( iLoopAllocationArray.Append( module ) );
        }

    return module;
    }

/*
-------------------------------------------------------------------------------

     Class: CTestCombiner

     Method: GetTest

     Description: Get test case from testcase array.

     Parameters:    TDesC& aTestId: in: TestId for testcase
     
     Return Values: CTCTestCase*: running/runned testcase
                    NULL: Testcase with aTestId not running/runned

     Errors/Exceptions: None

     Status: Approved
    
-------------------------------------------------------------------------------
*/
CTestCase* CTestCombiner::GetTest( TDesC& aTestId )
    {
    __TRACEFUNC();
    // TestId is not mandatory, so length may be zero
    if( aTestId.Length() == 0 )
        {
        return NULL;
        }

    TInt count = iTestCases.Count();
    for( TInt i=0; i < count; i++ )
        {
        if( iTestCases[i]->TestId() == aTestId )
            {
            // Found testcase with specified TestId
            return iTestCases[i];
            }
        }
    // Test case with aTestId not found
    return NULL;
    
    }
    
/*
-------------------------------------------------------------------------------

     Class: CTestCombiner

     Method: GetRunningTest

     Description: Get running test case.

     Parameters: TDesC& aTestId: in: TestId for testcase
     
     Return Values: CTCTestCase*: running testcase
                    NULL: Testcase with aTestId not running 

     Errors/Exceptions: None

     Status: Approved

-------------------------------------------------------------------------------
*/
CTestCase* CTestCombiner::GetRunningTest( TDesC& aTestId )
    {
    __TRACEFUNC();
     
    CTestCase* test = GetTest( aTestId );
    if( test ) 
        {
        if( test->State() == CTestCase::ETestCaseRunning )
            {
            return test;
            }
        else
            {
             __TRACE( KMessage, (_L("GetTest: Searched task (%S) not running (%i)"), 
                &aTestId, test->State() ));
            }
        }

    return NULL;
    
    }
    
/*
-------------------------------------------------------------------------------

     Class: CTestCombiner

     Method: GetLocalTest

     Description: Get local test case from testcase array.

     Parameters:    TDesC& aTestId: in: TestId for testcase
     
     Return Values: CTCTestCase*: running/runned testcase
                    NULL: Testcase with aTestId not running/runned 

     Errors/Exceptions: None

     Status: Proposal
    
-------------------------------------------------------------------------------
*/
CTCTestCase* CTestCombiner::GetLocalTest( TDesC& aTestId )
    {
    __TRACEFUNC();
    CTestCase* testCase = GetTest( aTestId );
    if( testCase )
        {    
        if( testCase->Type() != CTestCase::ECaseLocal )
            {
            __TRACE( KMessage, (_L("GetLocalTest: Searched task (%S) not local"), 
                      &aTestId ));
            return NULL;
            }
        }
    return ( CTCTestCase* ) testCase;
    
    }
    
/*
-------------------------------------------------------------------------------

     Class: CTestCombiner

     Method: GetLocalRunningTest

     Description:  Get local running test case.

     Parameters:    TDesC& aTestId: in: TestId for testcase
     
     Return Values: CTCTestCase*: running/runned testcase
                    NULL: Testcase with aTestId not running/runned 

     Errors/Exceptions: None

     Status: Proposal
    
-------------------------------------------------------------------------------
*/
CTCTestCase* CTestCombiner::GetLocalRunningTest( TDesC& aTestId )
    {
    __TRACEFUNC();
    CTestCase* testCase = GetRunningTest( aTestId );
    if( testCase )
        {    
        if( testCase->Type() != CTestCase::ECaseLocal )
            {
            __TRACE( KMessage, (_L("GetLocalRunningTest: Searched task (%S) not local"), 
                      &aTestId ));
            return NULL;
            }
        }
        
    return ( CTCTestCase* ) testCase;
    
    }
    
/*
-------------------------------------------------------------------------------

     Class: CTestCombiner

     Method: GetRemoteTest

     Description: Get local test case from testcase array.

     Parameters:    TDesC& aTestId: in: TestId for testcase
     
     Return Values: CTCTestCase*: running/runned testcase
                    NULL: Testcase with aTestId not running/runned 

     Errors/Exceptions: None

     Status: Proposal
    
-------------------------------------------------------------------------------
*/
CRemoteTestCase* CTestCombiner::GetRemoteTest( TDesC& aTestId )
    {
    __TRACEFUNC();
    CTestCase* testCase = GetTest( aTestId );
    if( testCase )
        {    
        if( testCase->Type() != CTestCase::ECaseRemote )
            {
            __TRACE( KMessage, (_L("GetRemoteTest: Searched task (%S) not remote"), 
                      &aTestId ));
            return NULL;
            }
        }
        
    return ( CRemoteTestCase* ) testCase;
    
    }
        
/*
-------------------------------------------------------------------------------

     Class: CTestCombiner

     Method: GetRemoteTest

     Description: Get remote test case from slave array.

     Parameters:    TUint32 aSlaveId: in: Slave id for testcase
     
     Return Values: TCaseInfo*: running/runned testcase
                    NULL: Testcase with aTestId not running/runned 

     Errors/Exceptions: None

     Status: Proposal
    
-------------------------------------------------------------------------------
*/
CRemoteTestCase* CTestCombiner::GetRemoteTest( TUint32 aSlaveId )
    {
    
    __TRACEFUNC();
    
    // Check all remote testcases
    TInt caseCount = iTestCases.Count(); 
    CRemoteTestCase* testCase = NULL;
    for( TInt caseInd = 0; caseInd < caseCount; caseInd++ )
        {
        if( iTestCases[caseInd]->Type() == CTestCase::ECaseRemote )
            {
            testCase = ( CRemoteTestCase* )iTestCases[caseInd];
            if( testCase->iSlaveId == aSlaveId )
                {
                return testCase;
                }
            }
        }
    
    return NULL;
        
    }   

/*
-------------------------------------------------------------------------------

     Class: CTestCombiner

     Method: GetRemoteTestRunSent

     Description: Get remote test case from slave array

     Parameters:    TUint32 aSlaveId: in: Slave id for testcase

     Return Values: TCaseInfo*: running/runned testcase
                    NULL: Testcase with aTestId not running/runned 

     Errors/Exceptions: None

     Status: Draft @js
    
-------------------------------------------------------------------------------
*/
    
CRemoteTestCase* CTestCombiner::GetRemoteTestRunSent( TUint32 aSlaveId )
    {
    
    __TRACEFUNC();
    
    // Check all remote testcases
    TInt caseCount = iTestCases.Count(); 
    CRemoteTestCase* testCase = NULL;
    for( TInt caseInd = 0; caseInd < caseCount; caseInd++ )
        {
        if( iTestCases[caseInd]->Type() == CTestCase::ECaseRemote )
            {
            testCase = ( CRemoteTestCase* )iTestCases[caseInd];
            
            if( testCase->iRemoteState != CRemoteTestCase::ECaseRunSent )
                {
                continue;
                }
            if( testCase->iSlaveId == aSlaveId )
                {
                return testCase;
                }
            }
        }
    
    return NULL;
        
    }       
  
/*
-------------------------------------------------------------------------------

     Class: CTestCombiner

     Method: GetRemoteTest

     Description: Get remote test case from slave array.

     Parameters:    TUint32 aSlaveId: in: Slave id for testcase
     
     Return Values: TCaseInfo*: running/runned testcase
                    NULL: Testcase with aTestId not running/runned 

     Errors/Exceptions: None

     Status: Proposal
    
-------------------------------------------------------------------------------
*/
CRemoteTestCase* CTestCombiner::GetRunningRemoteTest( TUint32 aSlaveId )
    {
    
    __TRACEFUNC();
    
    // Check all remote testcases
    TInt caseCount = iTestCases.Count(); 
    CRemoteTestCase* testCase = NULL;
    for( TInt caseInd = 0; caseInd < caseCount; caseInd++ )
        {
        if( ( iTestCases[caseInd]->Type() == CTestCase::ECaseRemote ) &&
            ( iTestCases[caseInd]->State() == CTestCase::ETestCaseRunning ) )
            {
            testCase = ( CRemoteTestCase* )iTestCases[caseInd];
            
            if( ( testCase->iRemoteState != CRemoteTestCase::ECaseRunSent ) &&
                ( testCase->iRemoteState != CRemoteTestCase::ECaseRunning ) && 
                ( testCase->iRemoteState != CRemoteTestCase::ECaseCancelled ))
                {
                //User::Leave( KErrGeneral ); 
                continue;                       
                }
                
            if( testCase->iSlaveId == aSlaveId )
                {
                return testCase;
                }
            }
        }
    
    return NULL;
        
    }   

    
/*
-------------------------------------------------------------------------------

     Class: CTestCombiner

     Method: GetRemoteTestSlave

     Description: Get remote test case running on slave

     Parameters:    TUint32 aSlaveDevId: in: Slave id for testcase
     
     Return Values: CRemoteTestCase*: running/runned testcase
                    NULL: Testcase with aTestId not running/runned 

     Errors/Exceptions: None

     Status: Proposal
    
-------------------------------------------------------------------------------
*/
CRemoteTestCase* CTestCombiner::GetRemoteRunningTestOnSlave( 
                                                    TUint32 aSlaveDevId )
    {
    
    __TRACEFUNC();
    
    // Check all remote testcases
    TInt caseCount = iTestCases.Count(); 
    CRemoteTestCase* testCase = NULL;
    for( TInt caseInd = 0; caseInd < caseCount; caseInd++ )
        {
        if( ( iTestCases[caseInd]->Type() == CTestCase::ECaseRemote ) &&
            ( iTestCases[caseInd]->State() == CTestCase::ETestCaseRunning ) )
            {
            testCase = ( CRemoteTestCase* )iTestCases[caseInd];
            if( DEVID( testCase->iSlaveId ) == DEVID( aSlaveDevId ) )
                {
                return testCase;
                }
            }
        }
    
    return NULL;
        
    }   

/*
-------------------------------------------------------------------------------

     Class: CTestCombiner

     Method: GetRemoteTestSlave

     Description: Gets a correct CRemoteSendReceive object on slave with
                  aSlaveId.

     Parameters:    TUint32 aSlaveId: in: Slave id CRemoteSendReceive object
     
     Return Values: CRemoteSendReceive*: Current CRemoteSendReceive object.
                    NULL: CRemoteSendReceive object do not exist or found.

     Errors/Exceptions: None

     Status: Proposal
    
-------------------------------------------------------------------------------
*/
CRemoteSendReceive* CTestCombiner::GetRemoteSendReceive( TUint32 aSlaveId )
    {
    __TRACEFUNC();
    
    for( TInt sr = 0; sr < iSendReceive.Count(); sr++ )
        {
        if( iSendReceive[sr]->iSlaveId == aSlaveId )
            {
            return iSendReceive[sr];
            }
        }

    return NULL;

    }

/*
-------------------------------------------------------------------------------

     Class: CTestCombiner

     Method: GetSlave

     Description: Get remote slave.

     Parameters:    TUint32 aSlaveId: in: Slave id 
     
     Return Values: CSlaveInfo*: reserved slave
                    NULL: slave with aSlaveId not reserved 

     Errors/Exceptions: None

     Status: Proposal
    
-------------------------------------------------------------------------------
*/
CSlaveInfo* CTestCombiner::GetSlave( TUint32 aSlaveId )
    {
    
    __TRACEFUNC();
    
    TInt count = iSlaveArray.Count();
    for( TInt index = 0; index < count; index++ )
        {
        if( DEVID( iSlaveArray[index]->iSlaveDevId ) == DEVID( aSlaveId) )
            {
            return iSlaveArray[index];
            }
        }
    
    return NULL;
    
    }

/*
-------------------------------------------------------------------------------

     Class: CTestCombiner

     Method: GetSlave

     Description: Get remote slave.

     Parameters:    TDesC& aSlaveName: in: Slave name 
     
     Return Values: CSlaveInfo*: reserved slave
                    NULL: slave with aSlaveId not reserved 

     Errors/Exceptions: None

     Status: Proposal
    
-------------------------------------------------------------------------------
*/
CSlaveInfo* CTestCombiner::GetSlave( TDesC& aSlaveName )
    {
    
    __TRACEFUNC();

    TInt count = iSlaveArray.Count();
    for( TInt index = 0; index < count; index++ )
        {
        if( iSlaveArray[index]->iName == aSlaveName )
            {
            return iSlaveArray[index];
            }
        }
    
    return NULL;
    
    }        

/*
-------------------------------------------------------------------------------

     Class: CTestCombiner

     Method: StartTestL

     Description: Start specified test case from testmodule.

     Parameters:    CStartInfo& aStartInfo: in: Test case information
     
     Return Values: KErrNone: Testcase started
                    KErrAlreadyExists: testcase with same aTestId is already 
                                       running
                    Any other SymbianOS errorcode

     Errors/Exceptions: Leaves if CTCTestCase::NewL leaves
                        Leaves if arguments are illegal
                        Leaves if GetTestModuleL leaves
                        Leaves if CTestExecution::Open fails
                        Leaves if CTCTestCase::StartL leaves
                        Leaves if RPointerArray::Append fails

     Status: Proposal

-------------------------------------------------------------------------------
*/
TInt CTestCombiner::StartTestL(  CStartInfo& aStartInfo )
    {
    __TRACEFUNC();
    // Check that TestId is unique.
    if( GetTest( aStartInfo.iTestId ) != NULL )
        {
        // If loop testing is ongoing, allow already defined testid.
        if( iLoopIsUsed )
            {
            __TRACE( KVerbose, ( _L("StartTestL: TestId (%S) already in use. Loop allows already defined TestId"), 
             &aStartInfo.iTestId ));
            }
        else
            {
            __TRACE( KError, ( _L("StartTestL: TestId (%S) already in use"), 
                 &aStartInfo.iTestId ));
            return KErrAlreadyExists;
            }
        }

    __ASSERT_ALWAYS( aStartInfo.iModule.Length() < KMaxFileName, 
        User::Leave( KErrArgument ) );
    __ASSERT_ALWAYS( aStartInfo.iIniFile.Length() < KMaxFileName, 
        User::Leave( KErrArgument ) );
    __ASSERT_ALWAYS( aStartInfo.iConfig.Length() < KMaxFileName, 
        User::Leave( KErrArgument ) );
    __ASSERT_ALWAYS( aStartInfo.iTestId.Length() < KMaxName, 
        User::Leave( KErrArgument ) );

    __TRACE( KMessage, ( _L("Call GetTestModuleL") ));

    // Load Test Module
    CTCTestModule* module = NULL;
    if( (aStartInfo.iModule.Find( KTestScripterName ) != KErrNotFound) 
        || (aStartInfo.iModule.Find( KPythonScripter ) != KErrNotFound)
        || (aStartInfo.iModule.Find( KTestCombinerName ) != KErrNotFound) 
        )
        {
        // TestScripter in use. Give config file for parsing STIF Settings.
        module = GetTestModuleL( aStartInfo.iModule,
                                 aStartInfo.iIniFile,
                                 aStartInfo.iConfig );
        }
    else
        {
        module = GetTestModuleL( aStartInfo.iModule,
                                 aStartInfo.iIniFile,
                                 KNullDesC );
        }

    __TRACE( KMessage, ( _L("Create CTCTestCase") ));

    CTCTestCase* tc = 
        CTCTestCase::NewL( this, 
                           aStartInfo.iModule, 
                           aStartInfo.iTestId, 
                           aStartInfo.iExpectedResult, 
                           aStartInfo.iCategory,
                           aStartInfo.iTestCaseArguments,
                           module ); //--PYTHON--

    CleanupStack::PushL( tc );
    
    //If name of testcase was given, find testcase number
    if(aStartInfo.iTitle != KNullDesC)
        {
        __TRACE(KMessage, (_L("Trying to find test case entitled \"%S\""), &aStartInfo.iTitle));
        aStartInfo.iCaseNum = -1;
        TInt ret = module->GetCaseNumByTitle(aStartInfo.iTitle, aStartInfo.iCaseNum);
        if(ret != KErrNone)
            {
            __TRACE(KError, (_L("Couldn't find test case entitled \"%S\". Error %d"), &aStartInfo.iTitle, ret));
            }
        else
            {
            __TRACE(KMessage, (_L("Found test case entitled \"%S\". Case num %d"), &aStartInfo.iTitle, aStartInfo.iCaseNum));
            }
        }

    __TRACE( KMessage, ( _L("Open TestExecution") ));
    
    User::LeaveIfError( tc->TestExecution().Open( module->TestServer(), 
                                                  aStartInfo.iCaseNum, 
                                                  aStartInfo.iConfig ) );

    __TRACE( KMessage, ( _L("Start testcase runner") ));

      // Enable testcase control before calling RunTestCase
    tc->StartL();

    __TRACE( KMessage, 
        ( _L("Start: testid(%S), module(%S), ini(%S), config(%S), case(%d), expect(%d/%d), timeout(%d)"), 
        &aStartInfo.iTestId, &aStartInfo.iModule, &aStartInfo.iIniFile, 
        &aStartInfo.iConfig, aStartInfo.iCaseNum, aStartInfo.iExpectedResult, 
        aStartInfo.iCategory, aStartInfo.iTimeout ));

    TestModuleIf().Printf( KPrintPriLow, _L("Start"),
        _L("testid(%S), module(%S), ini(%S), config(%S), case(%d), expect(%d)"), 
         &aStartInfo.iTestId, &aStartInfo.iModule, &aStartInfo.iIniFile, 
         &aStartInfo.iConfig, aStartInfo.iCaseNum, aStartInfo.iExpectedResult );

    if ( tc->TestCaseArguments().Length() > 0 )
        {
        tc->TestExecution().RunTestCase( tc->iResultPckg, tc->TestCaseArguments(), tc->iStatus );
        }
    else
        {
        tc->TestExecution().RunTestCase( tc->iResultPckg, tc->iStatus );
        }

    iRunningTests++;

    User::LeaveIfError( iTestCases.Append( tc ) );
    if( iLoopIsUsed )
        {
        User::LeaveIfError( iLoopAllocationArray.Append( tc ) );
        }
    CleanupStack::Pop( tc );

    return KErrNone;

}

/*
-------------------------------------------------------------------------------

     Class: CTestCombiner

     Method: Complete

     Description: Handle completed test case.

     Parameters:    CTCTestCase* aTestCase: in: Test case to complete

     Return Values: None.

     Errors/Exceptions: None

     Status: Approved

-------------------------------------------------------------------------------
*/
void CTestCombiner::Complete( CTestCase* aTestCase, TInt aError )
    {
    __TRACEFUNC();
    
    if( aError != KErrNone )
        {
        iResult = aError;
        }

    TInt count = iTestCases.Count();
    TInt i = 0;
    for(; i < count; i++ )
        {
        if( iTestCases[i] == aTestCase )
            {
            // Test Case completed
            __TRACE( KPrint, (  _L( "Complete: %S result: %d, execution result: %d, expected: %d"), 
                 &aTestCase->TestId(), aTestCase->iResult.iTestResult.iResult, 
                 aTestCase->iResult.iCaseExecutionResultCode, aTestCase->ExpectedResult()  ));
            TestModuleIf().Printf( KPrintPriLow, _L("Complete"),
                _L( "%S results: test(%d) exe(%d) expect(%d)"), 
                 &aTestCase->TestId(), aTestCase->iResult.iTestResult.iResult,
                 aTestCase->iResult.iCaseExecutionResultCode, aTestCase->ExpectedResult() );
            iRunningTests--;
            break;
            }
        }
    if( i == count )
        {
        __TRACE( KError, (_L("CTestCombiner::Complete: Test case not found!!")));
        }

    // Check if we were waiting this case to complete
    if( ( iWaitTestCase.Length() > 0 ) && 
        ( iWaitTestCase == aTestCase->TestId() ) )
        {
        // We were waiting this case to complete 
        // Now we can proceed executing the testcase
        __TRACE( KMessage, (_L("TestCase was waiting, set runner active")));
        iTestRunner->SetRunnerActive();
        iWaitTestCase.Zero();
        
     //return; - return removed due to STIF-509 CancelIfError won't work when used together with complete command
     
        }
    else if( aTestCase->Type() == CTestCase::ECaseRemote )
        {
        __TRACE( KMessage, (_L("CTestCombiner::Complete: Remote case complete")));
        // Completed testcase was remote case,
        // check if slave should be freed
        CRemoteTestCase* remote = ( CRemoteTestCase* )aTestCase;
        if( remote->iFreeSlave )
            {
            CSlaveInfo* slave = GetSlave( remote->iSlaveId );
            if( slave )
                {
                // Free slave now
                TRAPD( err, iTestRunner->ExecuteFreeL( slave ) );
                // Continue if freeing fails
                if( err == KErrNone )
                    {
                    __TRACE( KMessage, (_L("Complete: Freeing slave")));
                    return;
                    }
                else
                    {
                    __TRACE( KError, (_L("Complete: Freeing slave failed")));
                    }
                }
            else 
                {
                __TRACE( KError, (_L("Complete: Slave not found")));
                }
            }
        }

    // If running test is 0, active scheduler is active and CTestRunner is
    // ready then stop active scheduler.(Operations continue from 
    // CTestCombiner::RunTestL() after active scheduler start).
    if( ( iRunningTests == 0 ) &&
          iSchedulerActive &&
        ( iTestRunner->iState == CTestRunner::ERunnerReady ) )
        {
        // This was last running testcase, so we can stop 
        // activescheduler
        __TRACE( KMessage, (_L("All TestCases completed, stop CActiveScheduler")));
        CActiveScheduler::Current()->Stop();
        iSchedulerActive = EFalse;
        }
    else 
        {
        __TRACE( KMessage, 
            (_L("CTestCombiner::Complete: %d test cases running"), iRunningTests ));
        __TRACE( KMessage, 
            (_L("CTestCombiner::Complete: active %d"), iSchedulerActive ));
        if(iSchedulerActive)    
            {             
            __TRACE( KMessage, 
                (_L("CTestCombiner::Complete: state %d"), iTestRunner->iState ));
            }
        }

    // Checking if user wants to skip the rest of the execution in case of error @js
    if(iCancelIfError && iSchedulerActive)    
        {
        // Cancel event if it was waiting event
        if(iTestRunner->iEvent.Name() != KNullDesC && iTestRunner->iEvent.Type() == TEventIf::EWaitEvent)
            {
            TestModuleIf().CancelEvent(iTestRunner->iEvent, &iTestRunner->iStatus);
            }

        // Interpret execution result type from returned result
        TInt executionResult = TFullTestResult::ECaseExecuted; // Ok
        if( (aTestCase->iResult.iCaseExecutionResultType >= 
            TFullTestResult::ECaseLeave ))
            {
            __TRACE( KMessage, (_L("The test case ended with error!")));

            // Some abnormal execution result type
            executionResult = iTestCases[i]->iResult.iCaseExecutionResultType;
            }

         // Check expected execution result type
        if( executionResult != aTestCase->ExpectedResultCategory() ) 
            {
            // expected and returned result types differ
            __TRACE( KMessage, (_L("The test case has wrong result category!")));
            // There was an error and we must stop test case execution
            iFailedTestCase = i;
            iRunningTests--;
            __TRACE( KMessage, (_L("Stopping the CActiveScheduler.")));
            CActiveScheduler::Current()->Stop();
            iSchedulerActive = EFalse;
            }

        // Check normal test result if activescheduler is still up & running
        if( iSchedulerActive )
            {
            if( aTestCase->ExpectedResultCategory() == 
            TFullTestResult:: ECaseExecuted)
                {
                // Normal completion, check result
                if( iTestCases[i]->iResult.iTestResult.iResult != 
                    iTestCases[i]->ExpectedResult() )
                    { 
                    __TRACE( KMessage, (_L("Result category is not what was expected!")));
                    // There was an error and we must stop test case execution
                    iFailedTestCase = i;
                    iRunningTests = 0; //The whole test is stopped. Reset value of running tests.
                    //This line caused that variable value to be -1. Test could not finish. //iRunningTests--;
                    __TRACE( KMessage, (_L("Stopping the CActiveScheduler.")));
                    CActiveScheduler::Current()->Stop();
                    iSchedulerActive = EFalse;
                    }
                }
            else 
                {
                // Abnormal completion, i.e. panic, leave, exception or timeout
                if( aTestCase->iResult.iCaseExecutionResultCode != 
                    aTestCase->ExpectedResult())
                    {
                    __TRACE( KMessage, (_L("The test case has abnormal completion!")));
                    // There was an error and we must stop test case execution
                    iFailedTestCase = i;
                    iRunningTests--;
                    __TRACE( KMessage, (_L("Stopping the CActiveScheduler.")));
                    CActiveScheduler::Current()->Stop();
                    iSchedulerActive = EFalse;
                    }
                }
            }
        }
    }

/*
-------------------------------------------------------------------------------

     Class: CTestCombiner

     Method: NotifyEvent

     Description: Asynchronous event command interface
     
     Check requested events and send unset to first requested
     
     Parameters: TEventIf& aEvent: in: Event command 
                TRequestStatus& aStatus: in: TRequestStatus used in 
                                                          asynchronous command 
     
     Return Values: ETrue: asynchronous command given
                          EFalse: asyncronous command not given

     Errors/Exceptions: None
     
     Status: Proposal
     
-------------------------------------------------------------------------------
*/
TBool CTestCombiner::UnsetEvent( TEventIf& aEvent, 
                                 TRequestStatus& aStatus )
    {
    
    __TRACE( KMessage, ( _L("CTestCombiner::NotifyEvent")));    
    
    // First check TestCombiner events
    TInt eventCount = iEventArray.Count();
    for( TInt i = 0; i < eventCount; i++ )
        {
        if( aEvent.Name() == iEventArray[i]->Name() )
            {
            __TRACE( KMessage, (  
                _L( "Set unset pending for testcombiner's %S event"),  
                    &aEvent.Name()  ));
            iEventArray[i]->SetRequestStatus( &aStatus );
            return ETrue;
            }
        }
    
    // Check all local testcases
    TInt caseCount = iTestCases.Count(); 
    TInt eventInd;
    CTCTestCase* testCase = NULL;
    for( TInt caseInd = 0; caseInd < caseCount; caseInd++ )
        {
        if( iTestCases[caseInd]->Type() == CTestCase::ECaseLocal )
            {
            testCase = ( CTCTestCase* )iTestCases[caseInd];
            eventCount = testCase->EventArray().Count();
             
             // Check all requested events
            for( eventInd = 0; eventInd < eventCount; eventInd++)
                {
                const TName& eventName = 
                    testCase->EventArray()[eventInd]->Event().Name();
                if( eventName == aEvent.Name() )
                    {
                      // Event request is pending, send control command
                    iEvent.Copy( aEvent );
                    __TRACE( KMessage, (  
                        _L( "Set unset pending for client's %S event"),  
                        &aEvent.Name()  ));
                    testCase->TestExecution().NotifyEvent( iEventPckg, 
                                                             aStatus );
                    return ETrue;
                    }
                }
            }
        }
     
    return EFalse;
     
    }
     
/*
-------------------------------------------------------------------------------

     Class: CTestCombiner

     Method: ReceiveResponse

     Description: Handles responce received from slave
  
     Parameters:    None
     
     Return Values: None

     Errors/Exceptions: None

     Status: Draft
    
-------------------------------------------------------------------------------
*/
void CTestCombiner::ReceiveResponse( TDesC& aMsg )
    {
    
    __TRACEFUNC();
    
    TBool continueTask = ETrue;
    TRAPD( err, continueTask = iTestRunner->ReceiveResponseL( aMsg ) );
        
    // We start receiver again, even in error situation
    iRemoteReceiver->Start();

    if( err != KErrNone )
        {
        __TRACE( KError, ( _L("CTestCombiner::ReceiveResponse ERROR")));
        iResult = err;
        if( iTestRunner->IsActive() )
            {
            iTestRunner->Cancel();
            }
        else
            {
            iTestRunner->CancelTestCases();
            }
        return;
        }
        
    if( continueTask && !iTestRunner->IsActive() )
        {
        iTestRunner->SetRunnerActive();
        }
    
    }
    
/*
-------------------------------------------------------------------------------

     Class: CTestCombiner

     Method: RemoteTimeout

     Description: Handles timeouts.
  
     Parameters:    None
     
     Return Values: None

     Errors/Exceptions: None

     Status: Draft
    
-------------------------------------------------------------------------------
*/
void CTestCombiner::RemoteTimeout()
    {
    
    __TRACEFUNC();
    
    iResult = KErrTimedOut;
    if( iTestRunner->IsActive() )
        {
        __TRACE( KError, (_L("Remote timeout, Cancel runner")));
        iTestRunner->Cancel();
        }
    else
        {
        __TRACE( KError, (_L("Remote timeout, Cancel test cases")));
        iTestRunner->CancelTestCases();
        }
    
    }
    
/*
-------------------------------------------------------------------------------

     Class: CTestCombiner

     Method: ExecuteMeasurementL

     Description: Executes measurement script line.
  
     Parameters: CStifItemParser* aItem: in: parsed line
     
     Return Values: TBool: in no error ETrue returned

     Errors/Exceptions: None

     Status: Approved
    
-------------------------------------------------------------------------------
*/
void CTestCombiner::ExecuteMeasurementL( CStifItemParser* aItem )
    {
    __TRACEFUNC();
    
     __TRACEFUNC();
    TPtrC type;
    TPtrC command;

    // Get command
    if( aItem->GetNextString( command ) != KErrNone )
        {
        __TRACE( KError, ( 
            _L( "Unknown argument for 'measurement' command" ) ) );
        User::Leave( KErrArgument ); // Error in parsing => Leave
        }
    // Get name
    if( aItem->GetNextString( type ) != KErrNone )
        {
        __TRACE( KError, ( 
            _L( "Unknown argument for 'measurement' type" ) ) );
        User::Leave( KErrArgument ); // Error in parsing => Leave
        }

    // Verify measurement type
    if( !( type == KParamMeasurement01 ||
                type == KParamMeasurement02 ||
                type == KParamMeasurement03 ||
                type == KParamMeasurement04 ||
                type == KParamMeasurement05 ||
                type == KParamMeasurementBappea ) )

        {
        __TRACE( KError, ( 
            _L( "Unknown measurement type:[%S]" ), &type ) );
        User::Leave( KErrArgument ); // Error in types => Leave
        }

    // Verify command
   if( command == _L( "start" ) )
        {
        // START measurement's process
        __TRACE( KMessage, ( _L( "Start 'measurement' with '%S'"), &type ) );
        StartMeasurementL( type, aItem );
        }
    else if( command == _L( "stop" ) )
        {
        // STOP measurement's process
        __TRACE( KMessage, ( _L( "'Stop 'measurement' with '%S'"), &type ) );
        StopMeasurementL( type );
        }
    else
        {
        __TRACE( KError, ( 
            _L( "Unknown command for 'measurement' command:[%S] or type:[%S]" ), &command, &type ) );
        User::Leave( KErrArgument ); // Error in commands => Leave
        }

    }

/*
-------------------------------------------------------------------------------

     Class: CTestCombiner

     Method: StartMeasurementL

     Description: 
  
     Parameters: const TDesC& aType: in: Plugin type.
                 CStifItemParser* aItem: in: Item object for parsing.

     Return Values: None.

     Errors/Exceptions: Leaves is bappea start or timed operation fails.

     Status: Proposal
    
-------------------------------------------------------------------------------
*/
void CTestCombiner::StartMeasurementL( const TDesC& aType,
                                           CStifItemParser* aItem )
    {
    __TRACEFUNC();

    CSTIFTestMeasurement* testMeasurement = NULL;

    // Get Measurement configuration info
    TPtrC configurationInfo( KNullDesC() );
    if( aItem->Remainder( configurationInfo ) != KErrNone )
        {
        __TRACE( KInit, ( 
            _L( "Using default path and file name for measurement configure" ) ) );
        } 

    if( aType == KParamMeasurement01 )
        {
        testMeasurement = CSTIFTestMeasurement::NewL( 
                    this, 
                    CSTIFTestMeasurement::KStifMeasurementPlugin01,
                    configurationInfo );
        }
    else if( aType == KParamMeasurement02 )
        {
        testMeasurement = CSTIFTestMeasurement::NewL( 
                    this, 
                    CSTIFTestMeasurement::KStifMeasurementPlugin02,
                    configurationInfo );
        }
    else if( aType == KParamMeasurement03 )
        {
        testMeasurement = CSTIFTestMeasurement::NewL( 
                    this, 
                    CSTIFTestMeasurement::KStifMeasurementPlugin03,
                    configurationInfo );
        }
    else if( aType == KParamMeasurement04 )
        {
        testMeasurement = CSTIFTestMeasurement::NewL( 
                    this, 
                    CSTIFTestMeasurement::KStifMeasurementPlugin04,
                    configurationInfo );
        }
    else if( aType == KParamMeasurement05 )
        {
        testMeasurement = CSTIFTestMeasurement::NewL( 
                    this, 
                    CSTIFTestMeasurement::KStifMeasurementPlugin05,
                    configurationInfo );
        }
    else if( aType == KParamMeasurementBappea )
        {
        testMeasurement = CSTIFTestMeasurement::NewL( 
                    this, 
                    CSTIFTestMeasurement::KStifMeasurementBappeaProfiler,
                    configurationInfo );
        }
    else
        {
        __TRACE( KError, ( _L( "Unknown plugin[%S] for 'measurement'" ), &aType ) );
        User::Leave( KErrArgument );
        }

   // Start test measurement
    TInt start_ret( KErrNone );
    start_ret = testMeasurement->Start();
    if( start_ret != KErrNone )
        {
        delete testMeasurement;
        __TRACE( KError, ( 
            _L( "CTestCombiner::StartMeasurementL(): Measurement Start() fails:[%d]" ), start_ret ) );
        User::Leave( start_ret );
        }

    TTestMeasurement* object = new (ELeave) TTestMeasurement();
    object->iName = aType;
    object->iMeasurement = testMeasurement;

    // Array for handling test measurement between different objects
    TInt ret = iTestMeasurementArray.Append( object );
    if( ret != KErrNone )
        {
        delete object;
        __TRACE( KError, ( 
            _L( "CTestCombiner::StartMeasurementL(): iTestMeasurementArray.Append fails:[%d]" ), ret ) );
        User::Leave( ret );
        }

    }

/*
-------------------------------------------------------------------------------

     Class: CTestCombiner

     Method: StopMeasurementL

     Description: Stops test measurement.
  
     Parameters: None.

     Return Values: None.

     Errors/Exceptions: None.

     Status: Proposal
    
-------------------------------------------------------------------------------
*/
void CTestCombiner::StopMeasurementL( const TDesC& aType )
    {
    __TRACEFUNC();

    TInt count = iTestMeasurementArray.Count();
    for( TInt i = 0; i < count; i++ )
        {
        if( iTestMeasurementArray[i]->iName == aType )
            {
            // Found measurement module, stop
            iTestMeasurementArray[i]->iMeasurement->Stop();
            // Delete data
            delete iTestMeasurementArray[i];
            // Remove pointer to deleted data(Append())
            iTestMeasurementArray.Remove( i );
            // iTestMeasurementArray can contain only one type of measurement
            // so we can break when type is removed.
            break;
            }
        }

    }

/*
-------------------------------------------------------------------------------

     Class: CTestCombiner

     Method: AppendTestResultToResultDes

     Description: Append to TC's result description (if possible due to length) 
                  limitation provided text in [] brackets.
  
     Parameters: None.

     Return Values: None.

     Errors/Exceptions: None.

     Status: Proposal
    
-------------------------------------------------------------------------------
*/
void CTestCombiner::AppendTestResultToResultDes(TDes& aResultDescr, const TDesC& aTestCaseResultDescr)
    {
    if(aTestCaseResultDescr != KNullDesC)
        {
        _LIT(KAdditionalInfo, " [%S]");
        TInt len = aResultDescr.Length() + KAdditionalInfo().Length() + aTestCaseResultDescr.Length();

        if(len > KStifMaxResultDes)
            {
            len = KStifMaxResultDes - aResultDescr.Length() - KAdditionalInfo().Length();
            if(len > 0)
                {
                TPtrC descr = aTestCaseResultDescr.Mid(0, len); 
                aResultDescr.AppendFormat(KAdditionalInfo, &descr);
                }
            }
        else
            {
            aResultDescr.AppendFormat(KAdditionalInfo, &aTestCaseResultDescr);
            }
        }
    }

/*
-------------------------------------------------------------------------------

    Class: CTestCombiner

    Method: GetIndexForNewTestModuleController

    Description: Returns new index for test module controller.
                 This number is appended to module controller name.
                 This method is used when option to run every test case in 
                 separate process is set to on.

    Parameters: None
    
    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CTestCombiner::GetIndexForNewTestModuleController(void)
    {
    return iIndexTestModuleControllers++;
    }
    
/*
-------------------------------------------------------------------------------

    DESCRIPTION

    This module contains the implementation of CTestRunner class 
    member functions. CTestRunner is used to execute TestCombiner testcase by 
    CTestCombiner.

-------------------------------------------------------------------------------
*/
// MACROS
#ifdef LOGGER
#undef LOGGER
#endif
#define LOGGER iTestCombiner->iLog

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

     Class: CTestRunner

     Method: CTestRunner

     Description: Default constructor

     C++ default constructor can NOT contain any code, that
     might leave.
     
     Parameters: CTestCombiner* aTestCombiner: in: Backpointer to CTestCombiner

     Return Values: None

     Errors/Exceptions: None

     Status: Approved
    
-------------------------------------------------------------------------------
*/
CTestRunner::CTestRunner( CTestCombiner* aTestCombiner ): 
    CActive(  CActive::EPriorityLow ), // Executed with lowest priority 
    iState( ERunnerIdle ),
    iTestCombiner( aTestCombiner )
    {
    CActiveScheduler::Add( this );
    __TRACEFUNC();
    
    }

/*
-------------------------------------------------------------------------------

     Class: CTestRunner

     Method: ConstructL

     Description: Symbian OS second phase constructor

     Symbian OS default constructor can leave.

     Parameters:    None

     Return Values: None

     Errors/Exceptions: None

     Status: Approved
    
-------------------------------------------------------------------------------
*/
void CTestRunner::ConstructL()
    {
    TInt ret;
    
    ret = iPauseTimer.CreateLocal();
    if(ret != KErrNone)
        {
        __TRACE( KError, (_L("Unable to create RTimer: iPauseTimer [%d] "), ret));
        User::Leave(ret);
        }
        
    ret = iPauseCombTimer.CreateLocal();
    if(ret != KErrNone)
        {
        __TRACE( KError, (_L("Unable to create RTimer: iPauseCombTimer [%d] "), ret));
        User::Leave(ret);
        }
    
    iRemoteTimer = CRemoteTimer::NewL( iTestCombiner );
    
    }

/*
-------------------------------------------------------------------------------

     Class: CTestRunner

     Method: NewL

     Description: Two-phased constructor.
          
     Parameters: CTestCombiner* aTestCombiner: in: Backpointer to CTestCombiner

     Return Values: CTestRunner*: new object

     Errors/Exceptions: Leaves if new or ConstructL leaves

     Status: Approved
    
-------------------------------------------------------------------------------
*/

CTestRunner* CTestRunner::NewL( CTestCombiner* aTestCombiner )
    {
    CTestRunner* self = new (ELeave) CTestRunner( aTestCombiner );
     
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

/*
-------------------------------------------------------------------------------

     Class: CTestRunner

     Method: ~CTestRunner

     Description: Destructor
     
     Parameters:    None

     Return Values: None

     Errors/Exceptions: None

     Status: Approved
    
-------------------------------------------------------------------------------
*/     

CTestRunner::~CTestRunner()
    {
    __TRACEFUNC();
    Cancel();
    
    delete iRemoteTimer;
    iRemoteTimer = 0;
    
    delete iLine;
    iLine = 0;
    
    iPauseTimer.Close();
    
    iPauseCombTimer.Close();
         
    }

/*
-------------------------------------------------------------------------------

     Class: CTestRunner

     Method: RunL

     Description: Derived from CActive, handles testcase execution.

     Parameters:    None.

     Return Values: None.

     Errors/Exceptions: Leaves on error situations.

     Status: Approved
    
-------------------------------------------------------------------------------
*/
void CTestRunner::RunL()
    {
    __TRACEFUNC();
    __TRACE( KMessage, (_L("CTestRunner::RunL: [%d] "), iStatus.Int() ));
     
    User::LeaveIfError( iStatus.Int() );

    if( ( iTestCombiner == NULL ) || 
        ( iTestCombiner->iSectionParser == NULL ) )
        {
        User::Leave( KErrGeneral );
        }
        
    TBool continueTask = EFalse;
    
    // Check if there is still some time for combiner pause and we need to 
    // continue pausing
    if(iPauseCombRemainingTime > 0)
        {           
        // Maximum time for one RTimer::After request                   
        TInt maximumTime = KMaxTInt / 1000;                       
        
        __TRACE( KMessage, (_L("CTestRunner::RunL: Going to reissue PauseCombiner request ") ) );           
        __TRACE( KMessage, (_L("CTestRunner::RunL: iRemainingTimeValue = %d"), iPauseCombRemainingTime ) );        
        
        if( iPauseCombRemainingTime < maximumTime )
            {                           
            iPauseCombTimer.After(iStatus, (iPauseCombRemainingTime * 1000));
            iPauseCombRemainingTime = 0;
            }
        else
            {            
            iPauseCombRemainingTime -= maximumTime;
            iPauseCombTimer.After(iStatus, (maximumTime * 1000));        
            }     
            
        SetActive();
        return;
        }     
 
    // Handling runner states
    switch( iState )
        {
        case ERunnerWaitTimeout:
            {
            __TRACE( KMessage, (_L("Resume %S"), &iPausedTestCase));
            iTestCombiner->TestModuleIf().Printf( KPrintPriExec, 
                KExecute, _L("Resume %S"), &iPausedTestCase);
             // Get running testcase identified with testid
            CTestCase* testCase = iTestCombiner->GetRunningTest( iPausedTestCase ); 
            if( testCase == NULL ) User::Leave( KErrNotFound );
            iPausedTestCase.Zero();
            if( testCase->Type() == CTestCase::ECaseLocal )
                {
                CTCTestCase* localTestCase = ( CTCTestCase* )testCase;
                // Resume execution
                User::LeaveIfError( localTestCase->TestExecution().Resume() );
                continueTask = ETrue;
                }
            else // ECaseRemote
                {
                CRemoteTestCase* remoteTestCase = ( CRemoteTestCase* )testCase;
                // Resume execution
                if( ExecuteRemoteTestCtlL( NULL, 
                                           remoteTestCase, 
                                           TTCKeywords::EResume ) )
                    {
                    continueTask = ETrue;
                    }
                }               
            }
            break;
        case ERunnerWaitUnset:
            iState = ERunnerIdle;

            // Check Unset event 
            if( !CheckUnsetEvent() )
                {
                // Got event and unset has not completed 
                // Should never come here
                User::Panic( KTestRunner, KErrGeneral );
                 }
            break;
                         
        case ERunnerRunning:
            {
            iState = ERunnerIdle;
            
            // Get next execution line from configuration section
            iEndLoopStartPos = iTestCombiner->iSectionParser->GetPosition();
            TPtrC line;
            if( iTestCombiner->iSectionParser->GetNextLine( line ) == KErrNone )
                {
                // Got new execution line 
                __TRACE( KMessage, (_L("CTestRunner got line")));
        
                CStifItemParser* item = PreprocessLineL( line );
            
                if( item )
                    {     
                     // Got new execution line 
                    CleanupStack::PushL( item ); 
                      
                    // Execute script line 
                    if( ExecuteLineL( item ) )
                        {
                          __TRACE( KMessage, (_L("RunL: continueTask")));
                         // Set CTestRunner active again to perform 
                         // next execution line
                         // from testcase section 
                        continueTask = ETrue;
                        }
                    CleanupStack::PopAndDestroy( item );
                    }
                }
            else 
                {
                // No more execution lines in testcase section
                __TRACE( KMessage, 
                    (_L("CTestRunner::RunL: Testcase script done (%d running)"), 
                        iTestCombiner->iRunningTests));
 
                if( ( iTestCombiner->iRunningTests == 0 ) &&
                    iTestCombiner->iSchedulerActive )
                    {
                    __TRACE( KMessage, 
                        (_L("RunL: All TestCases done, stop CActiveScheduler")));                     
                    CActiveScheduler::Current()->Stop();
                    iTestCombiner->iSchedulerActive = EFalse;
                    }
                // Now testcase section is executed, 
                // so CTestRunner has done its job and stops
                iState = ERunnerReady;

                //If we're inside loop, then we have error
                if(iTestCombiner->iLoopIsUsed)
                    {
                    __TRACE(KError, (_L("Endloop keyword not found. Cannot finish test case properly.")));
                    iTestCombiner->iResult = KErrGeneral;
                    }
                }
            }
            break;
        case ERunnerAllocate:
        case ERunnerFree:
        case ERunnerRemote:
        default:
            __TRACE( KError, 
                (_L("CTestRunner::RunL: Entered in illegal state(%d)"), iState ));
            User::Panic( KTestRunner, KErrGeneral );  
            break;
        }   
    if( continueTask )
        {
        SetRunnerActive();
        }
           
    }
     
/*
-------------------------------------------------------------------------------

     Class: CTestRunner

     Method: DoCancel

     Description: Derived from CActive handles the Cancel

     Parameters:    None.

     Return Values: None.

     Errors/Exceptions: None.

     Status: Approved
    
-------------------------------------------------------------------------------
*/
void CTestRunner::DoCancel()
    {
    __TRACEFUNC();
    __TRACE( KMessage, (_L("CTestRunner::DoCancel")));
    iTestCombiner->TestModuleIf().Printf( KPrintPriLow, _L("Runner"), _L("DoCancel"));
    
    iPauseCombTimer.Cancel();
    
    switch( iState )
        {
        case ERunnerWaitTimeout:
            iPauseTimer.Cancel();
            break;
        case ERunnerWaitUnset:
            break;             
        case ERunnerRunning:
            break;
        case ERunnerAllocate:
        case ERunnerFree:
        case ERunnerRemote:
            // Cancel remote test cases
            break;
        default:
            __TRACE( KError, 
                (_L("CTestRunner::DoCancel: Entered in illegal state(%d)"), iState ));
            User::Panic( KTestRunner, KErrGeneral );  
            break;
        }                  
    
    // Cancel all testcases
    CancelTestCases();
         
    iState = ERunnerCancel;
      
    }

/*
-------------------------------------------------------------------------------

     Class: CTestRunner

     Method: RunError

     Description: Derived from CActive handles errors from active handler.
  
     Parameters:    TInt aError: in: error from CActive
     
     Return Values: KErrNone: success

     Errors/Exceptions: None.

     Status: Approved
    
-------------------------------------------------------------------------------
*/
TInt CTestRunner::RunError( TInt aError )
    {
    __TRACEFUNC();
    __TRACE( KMessage, (_L("CTestRunner::RunError %d"), aError));
        
    if ( iRunErrorMessage.Length() != 0 )
    	{
    	iTestCombiner->TestModuleIf().Printf( KPrintPriLow, _L("Runner"), 
	            _L("RunError : %S"), &iRunErrorMessage );  
	    iRunErrorMessage = KNullDesC;
    	}
    else
    	{
    	iTestCombiner->TestModuleIf().Printf( KPrintPriLow, _L("Runner"), 
	            _L("RunError"));        	
    	}    
    
    iState = ERunnerError;
        
    // Return error from here
    iTestCombiner->iResult = aError;

    CancelTestCases();
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

     Class: CTestRunner

     Method: PreprocessLineL

     Description: Preprocesses script line
  
     Parameters:  TPtrC& line: in: script line
                  CStifItemParser*& aItem: out: New CStifItemParser for script
                  line.
                    
     Return Values: HBufC* pointer if new memory that has been allocated

     Errors/Exceptions: Leaves on error situations.

     Status: Draft
    
-------------------------------------------------------------------------------
*/        
CStifItemParser* CTestRunner::PreprocessLineL( TDesC& line )
    {
    
    CStifItemParser* item = NULL;
    TPtrC tmp;
    TInt len = 0;
        
    // Decide how long buffer should be allocated
    if( line.Length() < KMaxName/2 )
        {
        len = KMaxName;
        }
    else 
        {
        len = line.Length() + KMaxName;
        }
    delete iLine;
    iLine = 0;
    iLine = HBufC::NewL( len );
    TPtr parsedLine( iLine->Des() );
    len = 0;
    
    item = CStifItemParser::NewL( line, 0, line.Length() );
    CleanupStack::PushL( item);
    
    TInt ret = item->GetString( _L(""), tmp );
    while( ret == KErrNone )
        {
        len += CheckDefined( tmp );
        if( ( parsedLine.Length() + tmp.Length() + 1 ) > parsedLine.MaxLength() )
            {
            // Allocate bigger buffer
            HBufC* tmpBuf = HBufC::NewL( parsedLine.MaxLength() + KMaxName );
            CleanupStack::PushL( tmpBuf );
            TPtrC ptr( iLine->Des() );
            parsedLine.Set( tmpBuf->Des() );
            parsedLine.Copy( ptr );
            delete iLine;
            iLine = tmpBuf;
            CleanupStack::Pop( tmpBuf );
            } 
        parsedLine.Append( tmp );        
        parsedLine.Append( _L(" ") );        
        ret = item->GetNextString( tmp );
        }
        
    CleanupStack::PopAndDestroy( item );
    
    item = CStifItemParser::NewL( parsedLine, 0, parsedLine.Length() );
    
    return item;
    
    }

/*
-------------------------------------------------------------------------------

     Class: CTestRunner

     Method: CheckDefined

     Description: Check if aWord is some defined word
  
     Parameters:  TPtrC& aWord: inout: Parsed word, defined or original returned

     Return Values: TInt: Length difference between new and old word

     Errors/Exceptions: None.

     Status: Approved

-------------------------------------------------------------------------------
*/
TInt CTestRunner::CheckDefined( TPtrC& aWord )
    {
    TInt len = 0;

    // KLoopCounter word changing to current loop count value.
    if( aWord == KLoopCounter )
        {
        iLoopCounterDes.Zero();
        iLoopCounterDes.AppendNum( iLoopCounter );
        len = iLoopCounterDes.Length() - aWord.Length();
        aWord.Set( iLoopCounterDes );
        return len;
        }

    TInt count = iTestCombiner->iDefined.Count();
    for( TInt i = 0; i < count; i++ )
        {
        if( iTestCombiner->iDefined[i]->Name() == aWord )
            { 
            len = iTestCombiner->iDefined[i]->Value().Length() - aWord.Length();
            aWord.Set( iTestCombiner->iDefined[i]->Value() );
            break;
            }
        }
    return len;

    }

/*
-------------------------------------------------------------------------------

     Class: CTestRunner

     Method: ExecuteLineL

     Description: Executes script line
  
     Parameters:    CStifItemParser* aItem: in: script line
                    TTCKeywords::TKeywords aKeyword: in: keyword index

     Return Values: ETrue: continue script file execution
                    EFalse: stop script file execution

     Errors/Exceptions: Leaves on error situations.

     Status: Approved

-------------------------------------------------------------------------------
*/
TBool CTestRunner::ExecuteLineL( CStifItemParser* aItem )
    {
    _LIT( KErrMsgUnknownKeyword, "Unknown or illegal keyword %S" );
    _LIT( KErrMsgMeasurementInvalidArgument, "Measurement : Invalid argument" );
    TBool continueTask = ETrue;
    TPtrC tmp;

    TPtrC keywordItem;
    // Get first word from line, i.e. keyword
    User::LeaveIfError( aItem->GetString( _L(""), keywordItem ) );
    // Parse keyword
    TInt keyword = TTCKeywords::Parse( keywordItem, TTCKeywords::Keyword );
        
    switch( keyword )
        {
        // Test case execution control cases
        case TTCKeywords::EPauseCombiner:
        	continueTask = ExecuteCombinerPauseL( aItem );
        	break;
        case TTCKeywords::ERun:
            continueTask = ExecuteRunL( aItem );
            break;
        case TTCKeywords::EPause:
        case TTCKeywords::EComplete:
        case TTCKeywords::ECancel:
        case TTCKeywords::EResume:
            continueTask = ExecuteTestCtlL( aItem, (TTCKeywords::TKeywords)keyword );
            break;

        // Event control cases
        case TTCKeywords::ESet:
              continueTask = ExecuteEventSetL( aItem );
            break;
        case TTCKeywords::EUnset:
              continueTask = ExecuteEventUnsetL( aItem );
            break;
        case TTCKeywords::ERequest:
        case TTCKeywords::EWait:
        case TTCKeywords::ERelease:
            continueTask = ExecuteEventCtlL( aItem, (TTCKeywords::TKeywords)keyword );
            break;
        case TTCKeywords::EPrint:
            {
            TName buf;
            while( aItem->GetNextString( tmp ) == KErrNone )
                {
                if( buf.Length() + tmp.Length() >= buf.MaxLength() )
                    {
                    break;
                    }
                buf.Append( tmp );
                buf.Append( _L(" ") );
                }

            __TRACE( KMessage, (_L("Test: %S"), &buf ));
            iTestCombiner->TestModuleIf().Printf( KPrintPriHigh, 
                _L("Test"), 
                _L("%S"), &buf);
           }
           break;
        case TTCKeywords::EAllocate:
            continueTask = ExecuteAllocateL( aItem );
            break;
        case TTCKeywords::EFree:
            continueTask = ExecuteFreeL( aItem );
            break;
        case TTCKeywords::ERemote:
            continueTask = ExecuteRemoteL( aItem );
            break;
        case TTCKeywords::ETimeout:
        case TTCKeywords::EPriority:
            // not used here
            break;
        case TTCKeywords::ECancelIfError:
            // @js
            iTestCombiner->iCancelIfError = ETrue;
            break;
        case TTCKeywords::EMeasurement:
            TRAPD( retErr, iTestCombiner->ExecuteMeasurementL( aItem ) );
        	if ( retErr == KErrArgument )
        		{
        		iRunErrorMessage = KErrMsgMeasurementInvalidArgument;
        		}
        	if ( retErr != KErrNone )
        		{
        		User::Leave( retErr );
        		}
            break;
        case TTCKeywords::ELoop:
            ExecuteLoopL( aItem );
            iTestCombiner->iLoopIsUsed = ETrue;
            break;
        case TTCKeywords::EEndLoop:
            continueTask = ExecuteEndLoopL();
            break;
        case TTCKeywords::ETitle:
            // title has been handled already, this is duplicate
        default:
            {
              __TRACE( KError, (_L("Unknown or illegal keyword") ) );
            // Unknown or illegal keyword
            iRunErrorMessage.Format( KErrMsgUnknownKeyword, &keywordItem );
            User::Leave( KErrGeneral );
            }
            break;
        }

    __TRACE( KMessage, (_L("RunL: TestCase line executed")));

    return continueTask;

    }

/*
-------------------------------------------------------------------------------

     Class: CTestRunner

     Method: ExecuteRunL

     Description: Executes run line
  
     Parameters:    CStifItemParser* aItem: in: script line
     
     Return Values: ETrue: continue script file execution
                    EFalse: stop script file execution

     Errors/Exceptions: Leaves on error situations.

     Status: Proposal
    
-------------------------------------------------------------------------------
*/
TBool CTestRunner::ExecuteRunL( CStifItemParser* aItem )
    {
    _LIT( KErrMsgCaseRunError, "Run : %S[case=%d] run error" );
    __TRACE( KMessage, (_L("Run")));
    iTestCombiner->TestModuleIf().Printf( KPrintPriExec, KExecute, _L("Run"));
    
    CStartInfo* startInfo = CStartInfo::NewL();
    CleanupStack::PushL( startInfo );

    ParseRunParamsL( aItem, *startInfo );
             
    // Start new case with configurations parsed above                
    
    iRunErrorMessage.Format( KErrMsgCaseRunError, &startInfo->iModule, startInfo->iCaseNum );
    User::LeaveIfError( 
        iTestCombiner->StartTestL( *startInfo ) );
    iRunErrorMessage = KNullDesC;
    
    CleanupStack::PopAndDestroy( startInfo );
        
    return ETrue;
    
}

 
/*
-------------------------------------------------------------------------------

     Class: CTestRunner

     Method: ParseRunParamsL

     Description: Parses run parameters
  
     Parameters:    CStifItemParser* aItem: in: script line
                    CStartInfo& aStartInfo: out: Parsed information
     
     Return Values: None
     
     Errors/Exceptions: Leaves on error situations.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
void CTestRunner::ParseRunParamsL( CStifItemParser* aItem,
                                    CStartInfo& aStartInfo )
    {
    _LIT( KErrMsgRunTestmoduleNameNotDefined, "Run : Testmodule name is not defined " );
    _LIT( KErrMsgRunCfgFileNotDefined, "Run : Testmodule configuration file is not defined" );
    _LIT( KErrMsgRunTestcaseNumberNotDefined, "Run : Testcase number is not defined or has invalid value" );
    _LIT( KErrMsgRunCfgFileNameToLong, "Run : TestScripter test case file(config)'s name is too long. Current length[%d], allowed max length[%d]. Cannot continue" );
    _LIT( KErrMsgRunInvalidExpectValue, "Run : Invalid expected result value" );
    _LIT( KErrMsgRunUnknownOrIllegalCategory, "Run : Unknown or illegal result category" );
    _LIT( KErrMsgRunInvalidTimeoutValue, "Run: Invalid testcase timeout value" );
    _LIT( KErrMsgRunUnknowOrIllegalKeyword, "Run: Unknown or illegal keyword %S" );
    
    TPtrC tmp;
    TInt ret = KErrNone;

    // Get mandatory run arguments
    // Testmodule name
    ret = aItem->GetNextString( tmp );
    if ( ret != KErrNone )
    	{
    	iRunErrorMessage = KErrMsgRunTestmoduleNameNotDefined;
    	User::Leave( ret );
    	}
    
    aStartInfo.SetModuleNameL( tmp );
    __TRACE( KMessage, (_L("module: %S"), &aStartInfo.iModule ));
    
    // Configuration file
    ret = aItem->GetNextString( tmp );
    if ( ret != KErrNone )
    	{
    	iRunErrorMessage = KErrMsgRunCfgFileNotDefined;
    	User::Leave( ret );
    	}
    
    TFileName cfgFileName( tmp );
    TStifUtil::CorrectFilePathL( cfgFileName );    
    aStartInfo.SetConfigL( cfgFileName );
    
    __TRACE( KMessage, (_L("config: %S"), &aStartInfo.iConfig ));

    // Check is TestScripter
    if( aStartInfo.iModule.Find( KTestScripterName ) != KErrNotFound )
        {
        // TestScripter name is format: 'testscripter_testcasefilename'

        TParse parse;
        parse.Set( aStartInfo.iConfig, NULL, NULL );

        // Maximum length of TestScripter's name(Max limitation from
        // CTestModuleController creation)
        TInt maximumLength = KMaxName - ( KTestScripterNameLength + 1 );

        TFileName testScripterAndTestCaseFile; // InitL() takes TFileName
        testScripterAndTestCaseFile.Copy( KTestScripterName );
        testScripterAndTestCaseFile.Append( _L( "_" ) );
        if( parse.Name().Length() < maximumLength )
            {
            testScripterAndTestCaseFile.Append( parse.Name() );
            }
        else
            {
            __TRACE( KInit, ( CStifLogger::ERed,
                _L( "TestScripter test case file(config)'s name is too long. Current length[%d], allowed max length[%d]. Cannot continue" ),
                parse.Name().Length(), maximumLength ) );
            iRunErrorMessage.Format( KErrMsgRunCfgFileNameToLong, parse.Name().Length(), maximumLength );
            User::Leave( KErrArgument );
            }
// ----
        aStartInfo.DeleteModuleName(); // Delete old name buffer for new one
        aStartInfo.SetModuleNameL( testScripterAndTestCaseFile );
        }

    // Testcase number
    ret = aItem->GetInt( tmp, aStartInfo.iCaseNum );
    if ( ret != KErrNone )
    	{
    	iRunErrorMessage = KErrMsgRunTestcaseNumberNotDefined;
    	User::Leave( ret );
    	}
    
    __TRACE( KMessage, (_L("testcasenum: %d"), aStartInfo.iCaseNum ) );
     
    // Set mode of item parser to be able to read titles with spaces inside
    aItem->SetParsingType(CStifItemParser::EQuoteStyleParsing);

    // Get optional run arguments
    while( aItem->GetNextString( tmp ) == KErrNone )
        {
        TPtrC val;   
        TPtrC arg;
        ParseOptArgL( tmp, arg, val );
        CheckDefined( val );
              
        // Parse optional argument
        switch( TTCKeywords::Parse( arg, TTCKeywords::RunOptArg ) )
            {
            case TTCKeywords::EExpect:
                {
                TLex ptr( val );
                ret = ptr.Val( aStartInfo.iExpectedResult );
                if ( ret != KErrNone )
                	{
                	iRunErrorMessage = KErrMsgRunInvalidExpectValue;
                    User::Leave( ret );
                	}
                __TRACE( KMessage, (_L("expect=%d"), aStartInfo.iExpectedResult));
                }
                break;
            case TTCKeywords::ETestid:
				{
                aStartInfo.SetTestIdL( val );
                __TRACE( KMessage, (_L("TestId=%S"), &val));
				}
                break;
            case TTCKeywords::EIni:
				{
                __TRACE( KMessage, (_L("ini=%S"), &val));
	        	TFileName iniFileName( val );
	        	TStifUtil::CorrectFilePathL( iniFileName );
	            aStartInfo.SetIniFileL( iniFileName );
				}
                break;
            case TTCKeywords::ECategory:
				{
                __TRACE( KMessage, (_L("category=%S"), &val));
                aStartInfo.iCategory = TTCKeywords::GetResultCategory( val );
                if( aStartInfo.iCategory == TFullTestResult::ECaseOngoing )
                    {
                    __TRACE( KError, (_L("Unknown or illegal result category")));
                    //Unknown or illegal category
                    iRunErrorMessage = KErrMsgRunUnknownOrIllegalCategory;
                    User::Leave( KErrGeneral );
                    }
				}
                break;
            case TTCKeywords::ECaseTimeout:
                {
                TLex ptr( val );
                ret = ptr.Val( aStartInfo.iTimeout );
                if ( ret != KErrNone )
                	{
                	iRunErrorMessage = KErrMsgRunInvalidTimeoutValue;
                    User::Leave( ret );
                	}
                __TRACE( KMessage, (_L("timeout=%d"), aStartInfo.iTimeout ) );
                }
                break;
           	case TTCKeywords::ECaseTitle:
           	    {
                __TRACE( KMessage, (_L("case title=%S"), &val));
                aStartInfo.SetTitleL(val);
                break;
           	    }
            case TTCKeywords::EArgs:
                {
                __TRACE( KMessage, (_L("case arguments=%S"), &val));
                aStartInfo.SetTestCaseArgumentsL( val );
                }
                break;				
            default:
				{
                __TRACE( KError, (_L("Unknown or illegal keyword")));
                //Unknown or illegal keyword
                iRunErrorMessage.Format( KErrMsgRunUnknowOrIllegalKeyword, &arg );
                User::Leave( KErrGeneral );
				}
            }
        }             
    } 

/*
-------------------------------------------------------------------------------

     Class: CTestRunner

     Method: ExecuteTestCtlL

     Description: Executes script line
  
     Parameters:    CStifItemParser* aItem: in: script line
                    TTCKeywords::TKeywords aKeyword: in: keyword index
     
     Return Values: ETrue: continue script file execution
                    EFalse: stop script file execution

     Errors/Exceptions: Leaves on error situations.

     Status: Approved
    
-------------------------------------------------------------------------------
*/
TBool CTestRunner::ExecuteTestCtlL( CStifItemParser* aItem, 
                                    TTCKeywords::TKeywords aKeyword )
    {
    _LIT( KErrMsgTestIdNotDefined, "%S : testid is not defined" );
    _LIT( KErrMsgTestCaseNotFound, "%S : Test case %S not found" );
    TBool continueTask = ETrue;
    TPtrC tmp;
    
    TInt ret = KErrNone;

	TPtrC keywordStr = TTCKeywords::Keyword( aKeyword );

    // Parse testid
    ret = aItem->GetNextString( tmp );
    if( ret != KErrNone )
    	{
    	iRunErrorMessage.Format( KErrMsgTestIdNotDefined, &keywordStr );
        User::Leave( ret );            	
    	}

    // Get testcase identified with testid
    CTestCase* testCase = iTestCombiner->GetTest( tmp ); 
    if( testCase == NULL )
        {
        __TRACE( KError, (_L("ExecuteTestCtlL: Test case %S not found"), 
            &tmp));
        iRunErrorMessage.Format( KErrMsgTestCaseNotFound, &keywordStr, &tmp );
        User::Leave( KErrNotFound );
        }  
                              
    switch( aKeyword )
        {
        // Test case execution control cases
        case TTCKeywords::EPause:
            continueTask = ExecutePauseL( aItem, testCase );
            break;
        case TTCKeywords::EComplete:
            continueTask = ExecuteCompleteL( aItem, testCase );
            break;
        case TTCKeywords::ECancel:
            __TRACE( KMessage, (_L("Cancel %S"), &tmp));
            iTestCombiner->TestModuleIf().Printf( KPrintPriExec, 
                KExecute, _L("Cancel %S"), &tmp);
            if( testCase->Type() == CTestCase::ECaseRemote )
                {
                continueTask = 
                    ExecuteRemoteTestCtlL( aItem, testCase, aKeyword ); 
                }
            else 
                {
                if( testCase->State() != CTestCase::ETestCaseRunning )
                    {
                    __TRACE( KMessage, (_L("Cancelled task (%S) not running (%i)"), 
                        &tmp, testCase->State() )); 
                    User::Leave( KErrNotFound );                   
                    }
                CTCTestCase* test = ( CTCTestCase* )testCase;
                // Cancel local testcase
                test->TestExecution().CancelAsyncRequest( ETestExecutionRunTestCase );
                }
            break;
        case TTCKeywords::EResume:
            {
            __TRACE( KMessage, (_L("Resume %S"), &tmp));
            iTestCombiner->TestModuleIf().Printf( KPrintPriExec, 
                KExecute, _L("Resume %S"), &tmp);
            if( testCase->Type() == CTestCase::ECaseRemote )
                {
                continueTask = 
                    ExecuteRemoteTestCtlL( aItem, testCase, aKeyword ); 
                }
            else 
                {
                if( testCase->State() != CTestCase::ETestCaseRunning )
                    {
                    __TRACE( KMessage, (_L("Resumed task (%S) not running (%i)"), 
                        &tmp, testCase->State() )); 
                    User::Leave( KErrNotFound );                   
                    }
                CTCTestCase* test = ( CTCTestCase* )testCase;
                // Resume execution
                User::LeaveIfError( test->TestExecution().Resume() );
                }
            }
            break;
            
        default:
            // Should never come here
            User::Leave( KErrGeneral );
            break;
        }      
    
    return continueTask;
    
    }


/*
-------------------------------------------------------------------------------

     Class: CTestRunner

     Method: ExecuteCombinerPauseL

     Description: Executes causes pause in TestCombiner
  
     Parameters:    CStifItemParser* aItem: in: script line
                         
     Return Values: ETrue: continue script file execution
                    EFalse: stop script file execution

     Errors/Exceptions: Leaves on error situations.

     Status: Proposal
    
-------------------------------------------------------------------------------
*/
TBool CTestRunner::ExecuteCombinerPauseL( CStifItemParser* aItem )
{
	_LIT( KErrMsgPauseTimeoutNotDefined, "PauseCombiner : No timeout value given or value has invalid format" );
	_LIT( KErrMsgPauseTimeoutNotPositive, "PauseCombiner : Timeout value can't be <0" );

    TBool continueTask = EFalse;
    TInt pauseTime;
    TInt ret = KErrNone;
    
    // Parse testid
    ret = aItem->GetNextInt( pauseTime );
    if ( ret != KErrNone )
    	{
    	iRunErrorMessage = KErrMsgPauseTimeoutNotDefined;
        User::Leave( ret );    	
    	}

    if( pauseTime < 0 )
        {
        __TRACE( KError, (_L("CTestRunner::ExecuteCombinerPauseL: Given pause value < 0")));
        iRunErrorMessage = KErrMsgPauseTimeoutNotPositive;
        User::Leave( KErrArgument );
        }    
    
    
    // Maximum time for one RTimer::After request
    TInt maximumTime = KMaxTInt / 1000;

    // Check if pause value is suitable for RTimer::After
    if(pauseTime < maximumTime)
        {
        iPauseCombTimer.After(iStatus, pauseTime * 1000);
        iPauseCombRemainingTime = 0;
        }
    else
        {
        // Given pause value after multiplication with 1000 is
        // larger than KMaxTInt, so we need to split it and 
        // re-request After with remaining value from RunL

        iPauseCombRemainingTime = pauseTime - maximumTime;
        iPauseCombTimer.After(iStatus, maximumTime * 1000);
        }

    SetActive();

    __TRACE(KMessage, (_L("Executing pause, time=[%d]"), pauseTime));
    
    iState = ERunnerRunning;
    
    return continueTask;
}
     
/*
-------------------------------------------------------------------------------

     Class: CTestRunner

     Method: ExecutePauseL

     Description: Executes pause line
  
     Parameters:    CStifItemParser* aItem: in: script line
                    CTestCase* aTestcase: in: test case
     
     Return Values: ETrue: continue script file execution
                    EFalse: stop script file execution

     Errors/Exceptions: Leaves on error situations.

     Status: Approved
    
-------------------------------------------------------------------------------
*/
TBool CTestRunner::ExecutePauseL( CStifItemParser* aItem,
                                    CTestCase* aTestcase )
    {
    _LIT( KErrMsgPauseUnknownKeyword, "Pause : Unknown or illegal keyword %S" );
    _LIT( KErrMsgPauseTimeInvalidValue, "Pause : Pause time is not defined or has invalid value" );
    _LIT( KErrMsgPauseTimeNotPositive, "Pause : Pause time can't be <0" );
    TBool continueTask = ETrue;

    // Get optional pause arguments
    TPtrC tmp;
    iPauseTime = 0;
    while( aItem->GetNextString( tmp ) == KErrNone )
        {
        TPtrC val;   
        TPtrC arg;
        ParseOptArgL( tmp, arg, val );
        CheckDefined( val );
              
        // Parse optional argument
        switch( TTCKeywords::Parse( arg, TTCKeywords::PauseOptArg ) )
            {
            case TTCKeywords::ETime:
                {
                TLex ptr( val );
                TInt ret = KErrNone;
                ret = ptr.Val( iPauseTime );
                if ( ret != KErrNone )
                	{
                	iRunErrorMessage = KErrMsgPauseTimeInvalidValue;
                	User::Leave( ret );
                	}                
                if ( iPauseTime < 0 )
                	{
                	iRunErrorMessage = KErrMsgPauseTimeNotPositive;
                	User::Leave( KErrArgument );                	
                	}
                __TRACE( KMessage, (_L("time=%d"), iPauseTime ));
                }
                break;
            default:
                __TRACE( KError, (_L("Unknown or illegal keyword")));
                //Unknown or illegal keyword
                iRunErrorMessage.Format( KErrMsgPauseUnknownKeyword, &arg );
                User::Leave( KErrGeneral );
            }
        }             
    
    // Store paused testcase id if timeout was given as pause argument 
    if( iPauseTime != 0 )
        {
        iPausedTestCase.Copy( aTestcase->TestId() );
        }
    
    if( aTestcase->Type() == CTestCase::ECaseRemote )
        {
        return ExecuteRemoteTestCtlL( aItem, aTestcase, TTCKeywords::EPause );
        }
    if( aTestcase->State() != CTestCase::ETestCaseRunning )
        {
        __TRACE( KMessage, (_L("Paused task (%S) not running (%i)"), 
            &aTestcase->TestId(), aTestcase->State() )); 
        User::Leave( KErrNotFound );                   
        }
    CTCTestCase* test = ( CTCTestCase* )aTestcase;
    
    // Pause execution
    User::LeaveIfError( test->TestExecution().Pause() );
    
    // Resume paused case if timeout was given 
    if( iPauseTime != 0 )
        {
        continueTask = EFalse;
        iState = ERunnerWaitTimeout;
        iPauseTimer.After( iStatus, iPauseTime*1000 );
        SetActive();         
        }
        
    return continueTask;
    }

/*
-------------------------------------------------------------------------------

     Class: CTestRunner

     Method: ExecuteCompleteL

     Description: Executes complete line
  
     Parameters:    CStifItemParser* aItem: in: script line
                    CTestCase* aTestcase: in: test case
     
     Return Values: ETrue: continue script file execution
                    EFalse: stop script file execution

     Errors/Exceptions: Leaves on error situations.

     Status: Approved
    
-------------------------------------------------------------------------------
*/
TBool CTestRunner::ExecuteCompleteL( CStifItemParser* /* aItem */, 
                                     CTestCase* aTestcase )
    {
    TBool ret = ETrue;
    
    if( aTestcase->State() == CTestCase::ETestCaseCompleted )
         {
         // Requested testcase is completed already,
         // proceed testcase execution
          __TRACE( KMessage, (_L("Already completed")));
         }
    else if( aTestcase->State() == CTCTestCase::ETestCaseRunning )
         {
         // Wait testcase to complete                    
         iTestCombiner->iWaitTestCase.Copy( aTestcase->TestId() );
         // Stop testcase execution until testcase completed 
         ret = EFalse;
         iState = ERunnerWaitTestCase;
         }
    else
        {
        // This should newer happen
        User::Leave( KErrGeneral );
        }
        
    return ret;
    
    }
    
/*
-------------------------------------------------------------------------------

     Class: CTestRunner

     Method: ExecuteEventSetL

     Description: Executes event set line
  
     Parameters:    CStifItemParser* aItem: in: script line
     
     Return Values: ETrue: continue script file execution
                    EFalse: stop script file execution

     Errors/Exceptions: Leaves on error situations.

     Status: Approved
    
-------------------------------------------------------------------------------
*/
TBool CTestRunner::ExecuteEventSetL( CStifItemParser* aItem )
    {
    _LIT( KErrMsgSetEventNameNotDefined, "Set : event name is not defined" );
    _LIT( KErrMsgSetUnknownOrIllegalKeyword, "Set :Unknown or illegal keyword %S" );
    _LIT( KErrMsgSetStateInvalidValue, "Set : State value is not defined or has invalid format" );
    TPtrC tmp;
    TPtrC eventName;
    TInt ret = KErrNone;
    
    // Get event name
    ret = aItem->GetNextString( eventName );
    if( ret != KErrNone )
    	{
    	iRunErrorMessage = KErrMsgSetEventNameNotDefined;
        User::Leave( ret );        
    	}
    __TRACE( KMessage, (_L("Set %S"), &eventName));
    iTestCombiner->TestModuleIf().Printf( KPrintPriExec, KExecute, 
        _L("Set %S"), &eventName);
    iEvent.SetName( eventName );
    iEvent.SetType( TEventIf::ESetEvent );

    // Get optional set arguments
    while( aItem->GetNextString( tmp ) == KErrNone )
        {
        TPtrC val;   
        TPtrC arg;
        ParseOptArgL( tmp, arg, val );
        CheckDefined( val );
              
        // Parse optional set argument
        switch( TTCKeywords::Parse( arg, TTCKeywords::EventOptArg ) )
            {
            case TTCKeywords::EState:
                {
                TLex ptr( val );
                TInt tmpVal = 0;
                ret = ptr.Val( tmpVal );
                if ( ret != KErrNone )
                	{
                	iRunErrorMessage = KErrMsgSetStateInvalidValue;
                	User::Leave( ret );
                	}
                
                // Only value 1 has special meaning, others are ignored
                if( tmpVal == 1 )
                    {
                     __TRACE( KMessage, (_L("State event")));
                     iEvent.SetEventType( TEventIf::EState );
                    }
                }
                break;
            default:
                __TRACE( KError, (_L("Unknown or illegal keyword")));
                //Unknown or illegal keyword
                iRunErrorMessage.Format( KErrMsgSetUnknownOrIllegalKeyword, &arg );
                User::Leave( KErrGeneral );
            }
        }      

    // Set event
    iTestCombiner->TestModuleIf().Event( iEvent, iStatus );
    iState = ERunnerRunning;
    SetActive();
            
    return EFalse;
    
    }
    
/*
-------------------------------------------------------------------------------

     Class: CTestRunner

     Method: ExecuteEventUnsetL

     Description: Executes event unset line
  
     Parameters:    CStifItemParser* aItem: in: script line
     
     Return Values: ETrue: continue script file execution
                    EFalse: stop script file execution

     Errors/Exceptions: Leaves on error situations.

     Status: Approved
    
-------------------------------------------------------------------------------
*/
TBool CTestRunner::ExecuteEventUnsetL( CStifItemParser* aItem )
    {
    _LIT( KErrMsgUnsetEventNameNotDefined, "Unset : Event name is not defined" );
    TPtrC eventName;
    TInt ret = KErrNone;
    // Get event name
    ret = aItem->GetNextString( eventName );
    if ( ret != KErrNone )
    	{
    	iRunErrorMessage = KErrMsgUnsetEventNameNotDefined;
    	User::Leave( ret );        
    	}
    
    __TRACE( KMessage, (_L("Unset %S"), &eventName));
    iTestCombiner->TestModuleIf().Printf( KPrintPriExec, KExecute, 
        _L("Unset %S"), &eventName);
    iEvent.Set( TEventIf::EUnsetEvent, eventName, TEventIf::EState );
    
    // Check if trying to unset an event that is requested 
    // by testcombiner (otherwise testcombiner would deadlock)
    TInt count = iTestCombiner->iEventArray.Count();
    TInt ind = 0;
    for(; ind < count; ind++ )
        {
        if( eventName == iTestCombiner->iEventArray[ind]->Name() )
            {
            User::Leave( KErrInUse );
            }
        }

    // Check if some testmodule below 
    // has event request pending
    if( iTestCombiner->UnsetEvent( iEvent, 
                                   iStatus ) == EFalse )
        {
        // If they haven't requested event,
        // then check others above
        iTestCombiner->TestModuleIf().Event( iEvent, iStatus );
        iState = ERunnerRunning;
        SetActive();
            
        }
    else
        {
        // Some testmodule below has requested the event
        // Wait unset to complete
        SetActive();
        __TRACE( KPrint, ( _L("Unset: Start" ) ) );
        iState = ERunnerWaitUnset;
        // Stop execution until unset has completed
        }
    return EFalse;
    }

/*
-------------------------------------------------------------------------------

     Class: CTestRunner

     Method: ExecuteLineL

     Description: Executes script line
  
     Parameters:    CStifItemParser* aItem: in: script line
                    TTCKeywords::TKeywords aKeyword: in: keyword index
     
     Return Values: ETrue: continue script file execution
                    EFalse: stop script file execution

     Errors/Exceptions: Leaves on error situations.

     Status: Approved
    
-------------------------------------------------------------------------------
*/
TBool CTestRunner::ExecuteEventCtlL( CStifItemParser* aItem, 
                                     TTCKeywords::TKeywords aKeyword )
    {
    _LIT( KErrMsgEventNameNotDefined, "%S : Event name is not defined" );
    _LIT( KErrMsgEequestEventAlreadyExist, "Request : Requested event %S already exists" );
    _LIT( KErrMsgWaitEventNotRequested, "Wait :Waited event %S is not requested" );
    _LIT( KErrMsgReleaseEventNotRequested, "Release : Released event %S is not requested" );
    
    TBool continueTask = ETrue;
    TPtrC eventName;
    TInt ret = KErrNone;
	TPtrC keywordStr = TTCKeywords::Keyword( aKeyword );
    // Get event name
    ret = aItem->GetNextString( eventName );
    if ( ret != KErrNone )
    	{
    	iRunErrorMessage.Format( KErrMsgEventNameNotDefined, &keywordStr );
    	User::Leave( ret );
    	}
    
    TInt count = iTestCombiner->iEventArray.Count();
    TInt ind = 0;
    for(; ind < count; ind++ )
        {
        if( eventName == iTestCombiner->iEventArray[ind]->Name() )
            {
            break;
            }
        }
                    
    switch( aKeyword )
        {
        case TTCKeywords::ERequest:
            {
            __TRACE( KMessage, (_L("Request %S"), &eventName));
            iTestCombiner->TestModuleIf().Printf( KPrintPriExec, 
                KExecute, _L("Request %S"), &eventName);
                
            // Check that event is not already requested           
            if( ind < count )
                {
                __TRACE( KError, (_L("Requested event %S already exists"), 
                    &eventName));
                iRunErrorMessage.Format( KErrMsgEequestEventAlreadyExist, &eventName );
                User::Leave( KErrAlreadyExists );
                }
                
            // Add event to event array
            iEvent.SetName( eventName );
            iEvent.SetType( TEventIf::EReqEvent );
            TEventTc* event = new (ELeave) TEventTc( iTestCombiner->iLog );
            CleanupStack::PushL( event );
            event->Copy( iEvent );
            User::LeaveIfError( iTestCombiner->iEventArray.Append( event ));
            if( iTestCombiner->iLoopIsUsed )
                {
                User::LeaveIfError( iTestCombiner->iLoopAllocationArray.Append( event ) );
                }
            CleanupStack::Pop( event );
            
            // Request event
            iTestCombiner->TestModuleIf().Event( iEvent, iStatus );
            iState = ERunnerRunning;
            SetActive();
            continueTask = EFalse;
            }
            break;
        case TTCKeywords::EWait:
              {
            __TRACE( KMessage, (_L("Wait %S"), &eventName));
            iTestCombiner->TestModuleIf().Printf( KPrintPriExec, KExecute, 
                _L("Wait %S"), &eventName);

            // Check that event is requested           
            if( ind == count )
                {
                __TRACE( KError, (_L("Waited event %S is not requested"), 
                    &eventName));
                iRunErrorMessage.Format( KErrMsgWaitEventNotRequested, &eventName );
                User::Leave( KErrNotFound );
                }            
            iEvent.SetName( eventName );    
            iEvent.SetType( TEventIf::EWaitEvent );     
            // Wait event
            iTestCombiner->TestModuleIf().Event( iEvent, iStatus );
            iState = ERunnerRunning;
            SetActive();
            continueTask = EFalse;
            }
            break;
        case TTCKeywords::ERelease:
            {
            __TRACE( KMessage, (_L("Release %S"), &eventName));
            iTestCombiner->TestModuleIf().Printf( KPrintPriExec, KExecute, 
                _L("Release %S"), &eventName);
            // Check that event is requested           
            if( ind == count )
                {
                __TRACE( KError, (_L("Released event %S is not requested"), 
                    &eventName));
                iRunErrorMessage.Format( KErrMsgReleaseEventNotRequested, &eventName );
                User::Leave( KErrNotFound );
                }            
            // Remove event from array
            TEventTc* event = iTestCombiner->iEventArray[ind];
            iTestCombiner->iEventArray.Remove( ind );
            delete event;
            iEvent.SetName( eventName );
            iEvent.SetType( TEventIf::ERelEvent );
            
            // Release event
            iTestCombiner->TestModuleIf().Event( iEvent, iStatus );
            iState = ERunnerRunning;
            SetActive();
            continueTask = EFalse;
            }
            break;
        default:
            {
              __TRACE( KError, (_L("Illegal keyword") ) );
            
            // Unknown or illegal keyword
            User::Leave( KErrGeneral );
            }
            break;
     
        }
    return continueTask;
    }

/*
-------------------------------------------------------------------------------

     Class: CTestRunner

     Method: ExecuteAllocateL

     Description: Executes allocate line
  
     Parameters:    CStifItemParser* aItem: in: script line
     
     Return Values: ETrue: continue script file execution
                    EFalse: stop script file execution

     Errors/Exceptions: Leaves on error situations.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
TBool CTestRunner::ExecuteAllocateL( CStifItemParser* aItem )
    {
    _LIT( KErrMsgAllocateSlaveTypeNotDefined, "Allocate : Slave type was not given for allocate" );
    _LIT( KErrMsgAllocateSlaveNameNotDefined, "Allocate : Slave name is not defined" );
    _LIT( KErrMsgAllocateSlaveAlreadyAllocated, "Allocate : Slave with name %S already allocated" );
    __TRACE( KMessage, (_L("Allocate")));
       
    TPtrC type;
    TPtrC name;
    // Get slave type
    TInt ret = aItem->GetNextString( type );
    if( ret != KErrNone )
        {
        __TRACE( KError, (_L("Slave type was not given for allocate")));
        iRunErrorMessage = KErrMsgAllocateSlaveTypeNotDefined;
        User::Leave( KErrArgument );
        }
    
    // Get slave name
    ret = aItem->GetNextString( name );
    if( ret != KErrNone )
        {
        __TRACE( KError, (_L("Slave name was not given for allocate")));
        iRunErrorMessage = KErrMsgAllocateSlaveNameNotDefined;
        User::Leave( KErrArgument );
        }

    iTestCombiner->TestModuleIf().Printf( KPrintPriExec, KExecute, 
        _L("Allocate %S"), &name );
        
    __TRACE( KMessage, (_L("Allocate %S [name: %S]"), &type, &name));
    
    if( iTestCombiner->GetSlave( name ) )
        {
        __TRACE( KError, (_L("Slave with name %S already allocated"), 
            &name ) );
        iRunErrorMessage.Format( KErrMsgAllocateSlaveAlreadyAllocated, &name );
        User::Leave( KErrAlreadyExists );
        } 
        
    CSlaveInfo* slave = CSlaveInfo::NewL( name, KRemoteProtocolMasterId );
    CleanupStack::PushL( slave );
    User::LeaveIfError( iTestCombiner->iSlaveArray.Append( slave ) );
    if( iTestCombiner->iLoopIsUsed )
        {
        User::LeaveIfError( iTestCombiner->iLoopAllocationArray.Append( slave ) );
        }
    CleanupStack::Pop( slave );

    CStifTFwIfProt* req = CStifTFwIfProt::NewL();
    CleanupStack::PushL( req );
    req->CreateL();
    
    // Reserve message
    User::LeaveIfError(
        req->Append( CStifTFwIfProt::MsgType, CStifTFwIfProt::EMsgReserve ) );
    // Srcid. i.e. master id
    User::LeaveIfError(
        req->AppendId( slave->iMasterId ) );
    // DstId, broacast id
    User::LeaveIfError(
        req->AppendId( slave->iSlaveDevId ) );
    // Slave type
    User::LeaveIfError( req->Append( type ) );

    User::LeaveIfError( 
        iTestCombiner->TestModuleIf().RemoteSend( req->Message() ) );
        
    iState = ERunnerAllocate;
    slave->iState = CSlaveInfo::ESlaveReserveSent;

    // Start timer    
    iRemoteTimer->SetTimerActive( iTestCombiner->iRemoteTimeout );

    CleanupStack::PopAndDestroy( req );

    return EFalse;
    
    }
    
/*
-------------------------------------------------------------------------------

     Class: CTestRunner

     Method: ExecuteFreeL

     Description: Executes free line
  
     Parameters:    CStifItemParser* aItem: in: script line
     
     Return Values: ETrue: continue script file execution
                    EFalse: stop script file execution

     Errors/Exceptions: Leaves on error situations.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
TBool CTestRunner::ExecuteFreeL( CStifItemParser* aItem )
    {
    _LIT( KErrMsgFreeSlaveNameNotDefined, "Free : Slave name is not defined" );
    _LIT( KErrMsgFreeSlaveNotFound, "Free : Slave %S not found" );
    _LIT( KErrMsgFreeSlaveReserved, "Free : Slave %S in illegal state %d, cannot be released" );
    __TRACE( KMessage, (_L("Free")));
    
    TPtrC name;    
    // Get slave name
    TInt ret = aItem->GetNextString( name );
    if( ret != KErrNone )
        {
        __TRACE( KError, (_L("Slave name was not given for free")));
        iRunErrorMessage = KErrMsgFreeSlaveNameNotDefined;
        User::Leave( KErrArgument );
        }

    iTestCombiner->TestModuleIf().Printf( KPrintPriExec, KExecute, 
        _L("Free %S"), &name );
        
    __TRACE( KMessage, (_L("Free %S"), &name ) );
    
    CSlaveInfo* slave = iTestCombiner->GetSlave( name );
    if( slave == NULL )
        {
        __TRACE( KError, (_L("Slave %S not found"), &name ));
        iRunErrorMessage.Format( KErrMsgFreeSlaveNotFound, &name );
        User::Leave( KErrNotFound );
        }
    if( slave->iState != CSlaveInfo::ESlaveReserved )
        {
        __TRACE( KError, (_L("Slave %S in illegal state %d, cannot be released"), 
            &name, slave->iState ));
        iRunErrorMessage.Format( KErrMsgFreeSlaveReserved, &name, slave->iState );
        User::Leave( KErrGeneral );
        } 
        
    ExecuteFreeL( slave );
    
    return EFalse;
    
    }

/*
-------------------------------------------------------------------------------

     Class: CTestRunner

     Method: ExecuteFreeL

     Description: Executes free line
  
     Parameters:    CSlaveInfo* aSlave: in: slave info
     
     Return Values: None

     Errors/Exceptions: Leaves on error situations.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
void CTestRunner::ExecuteFreeL( CSlaveInfo* aSlave )
    {    
    
    CRemoteTestCase* testCase = 
        iTestCombiner->GetRemoteRunningTestOnSlave( aSlave->iSlaveDevId );
          
    if( testCase )
        {
        __TRACE( KMessage, 
            (_L("Postpone free until testcases completed")));
        // Test cases still running on slave,
        // Free slave after test case has completed
        testCase->iFreeSlave = ETrue;
        return;
        }
    
    CStifTFwIfProt* req = CStifTFwIfProt::NewL();
    CleanupStack::PushL( req );
    req->CreateL();
    
    // Release message
    User::LeaveIfError( 
        req->Append( CStifTFwIfProt::MsgType, CStifTFwIfProt::EMsgRelease ) );
    // Srcid. i.e. master id
    User::LeaveIfError( 
        req->AppendId( aSlave->iMasterId ) );
    // DstId is device broadcast
    User::LeaveIfError( 
        req->AppendId( SETID( DEVID( aSlave->iSlaveDevId ), 0 ) ) );
    
    User::LeaveIfError( 
        iTestCombiner->TestModuleIf().RemoteSend( req->Message() ) );
        
    iState = ERunnerFree;
    aSlave->iState = CSlaveInfo::ESlaveReleaseSent;

    // Start timer    
    iRemoteTimer->SetTimerActive( iTestCombiner->iRemoteTimeout );
   
    CleanupStack::PopAndDestroy( req );
    
    }
    
/*
-------------------------------------------------------------------------------

     Class: CTestRunner

     Method: ExecuteRemoteL

     Description: Executes remote line
  
     Parameters:    CStifItemParser* aItem: in: script line
     
     Return Values: ETrue: continue script file execution
                    EFalse: stop script file execution

     Errors/Exceptions: Leaves on error situations.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
TBool CTestRunner::ExecuteRemoteL( CStifItemParser* aItem )
    {
    _LIT( KErrMsgRemoteSlaveNameNotDefined, "Remote : Slave name is not defined" );
    _LIT( KErrMsgRemoteSlaveNotFound, "Remore : Slave %S not found" );
    _LIT( KErrMsgRemoteIllegalState, "Remote : Slave %S in illegal state %d, cannot send remote call" );
    _LIT( KErrMsgRemoteCommandNotDefined, "Slave command name was not given for remote" );
    TPtrC name;    
    TPtrC command;    
    // Get slave name
    TInt ret = aItem->GetNextString( name );
    if( ret != KErrNone )
        {
        __TRACE( KError, (_L("Slave name was not given for remote")));
        iRunErrorMessage = KErrMsgRemoteSlaveNameNotDefined;
        User::Leave( KErrArgument );
        }
        
    __TRACE( KMessage, (_L("Remote command to %S"), &name));
    
    CSlaveInfo* slave = iTestCombiner->GetSlave( name );
    if( slave == NULL )
        {
        __TRACE( KError, (_L("Slave %S not found"), &name ));
        iRunErrorMessage.Format( KErrMsgRemoteSlaveNotFound, &name );
        User::Leave( KErrArgument );
        }
    if( slave->iState != CSlaveInfo::ESlaveReserved )
        {
        __TRACE( KError, (_L("Slave %S in illegal state %d, cannot send remote call"), 
            &name, slave->iState ));
        iRunErrorMessage.Format( KErrMsgRemoteIllegalState, &name, slave->iState );
        User::Leave( KErrNotReady );
        } 
        
    // Get remote command name
    ret = aItem->GetNextString( command );
    if( ret != KErrNone )
        {
        __TRACE( KError, (_L("Slave command name was not given for remote")));
        iRunErrorMessage = KErrMsgRemoteCommandNotDefined;
        User::Leave( KErrArgument );
        }

    iTestCombiner->TestModuleIf().Printf( KPrintPriExec, KExecute, 
        _L("remote %S %S"), &name, &command );

    __TRACE( KPrint, (_L("remote %S %S"), &name, &command ) );

    // Parse command name
    TInt key = TTCKeywords::Parse( command, TTCKeywords::Keyword );    
    TBool continueTask = ETrue;
    
    switch( key )
        {
        // Test case starting
        case TTCKeywords::ERun:
            continueTask = ExecuteRemoteRunL( aItem, slave );
            break;
                   
        // Event control cases
        case TTCKeywords::ERequest:
        case TTCKeywords::EWait:
        case TTCKeywords::ERelease:
            continueTask = ExecuteRemoteEventCtlL( aItem, slave, key );
            break;

        case TTCKeywords::ESet:
        case TTCKeywords::EUnset:
            continueTask = ExecuteRemoteSetUnsetEventL(aItem, slave, key);
			break;
        // asynchronous 'sendreceive'
        case TTCKeywords::ESendReceive:
            continueTask = ExecuteRemoteSendReceiveL( aItem, slave );
            break;

        default:
            // Some unknown remote command, forward as such 
            continueTask = ExecuteRemoteUnknownL( aItem, slave, command );
            break;
        }
    
    return continueTask;
    
    }

/*
-------------------------------------------------------------------------------

     Class: CTestRunner

     Method: ExecuteRemoteRunL

     Description: Handles remote run 
  
     Parameters:    CStifItemParser* aItem: in: script line
                    CSlaveInfo* aSlave: in: slave info
                    HBufC *aSetUnsetEvent: in: data needed for startInfo
                    TInt aCaseNumber: in: data needed for startInfo
     
     Return Values: ETrue: continue script file execution
                    EFalse: stop script file execution and wait response

     Errors/Exceptions: Leaves on error situations.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
TBool CTestRunner::ExecuteRemoteRunL( CStifItemParser* aItem,
                                        CSlaveInfo* aSlave,
                                        HBufC *aSetUnsetEvent,
                                        TInt aCaseNumber )
    {
    
    CStifTFwIfProt* req = CStifTFwIfProt::NewL();
    CleanupStack::PushL( req );
    req->CreateL();
    
    CStartInfo* startInfo = CStartInfo::NewL();
    CleanupStack::PushL( startInfo );
    //if aSetUnsetEvent is given, then get start info from this argument
    if(aSetUnsetEvent != NULL)
    	{
    	TBuf<10> tmpModuleName;
    	tmpModuleName.Copy(_L("suevent"));
    	startInfo->SetModuleNameL(tmpModuleName);
    	startInfo->SetConfigL(*aSetUnsetEvent);
    	startInfo->iCaseNum = aCaseNumber;
    	}
    else
    	{
    ParseRunParamsL( aItem, *startInfo );   
}
    
    if( iTestCombiner->GetTest( startInfo->iTestId ) )
        {
        /*        
        __TRACE( KError, (_L("´Slave test running already with testid %S"), 
            &startInfo->iTestId  ) );
        */
        User::Leave( KErrAlreadyExists );
        }
        
    CRemoteTestCase* remote = 
        CRemoteTestCase::NewL( iTestCombiner, 
                               startInfo->iTestId,
                               startInfo->iExpectedResult,
                               startInfo->iCategory ); 
    
    CleanupStack::PushL( remote );
        
    // Remote message
    User::LeaveIfError( 
        req->Append( CStifTFwIfProt::MsgType, CStifTFwIfProt::EMsgRemote ) );
    // Srcid. i.e. master id
    User::LeaveIfError( req->AppendId( aSlave->iMasterId ) );
    // DstId is device broadcast
    User::LeaveIfError( 
        req->AppendId( SETID( DEVID( aSlave->iSlaveDevId ), 0 ) ) );
    // Run command
    User::LeaveIfError( 
        req->Append( CStifTFwIfProt::CmdType, CStifTFwIfProt::ECmdRun ) );
    // Run parameters
    User::LeaveIfError(
        req->Append( CStifTFwIfProt::RunParams, 
                CStifTFwIfProt::ERunModule, 
                startInfo->iModule ) );
    User::LeaveIfError(
        req->Append( CStifTFwIfProt::RunParams, 
                CStifTFwIfProt::ERunTestcasenum, 
                startInfo->iCaseNum ));
    if( startInfo->iIniFile.Length() > 0 )
        {
        // Initialization file
        __TRACE( KMessage, (_L("ini: %S"), &startInfo->iIniFile ));     
        User::LeaveIfError(
            req->Append( CStifTFwIfProt::RunParams, 
                    CStifTFwIfProt::ERunInifile, 
                    startInfo->iIniFile ) );
        }
    if( startInfo->iConfig.Length() > 0 )
        {
        // Initialization file
        __TRACE( KMessage, (_L("config: %S"), &startInfo->iConfig ));     
        User::LeaveIfError(
            req->Append( CStifTFwIfProt::RunParams, 
                    CStifTFwIfProt::ERunTestcasefile, 
                    startInfo->iConfig ));
        } 
    //Title (must be given between quotation marks in case of any spaces inside
    if( startInfo->iTitle.Length() > 0 )
        {
        __TRACE(KMessage, (_L("title: %S"), &startInfo->iTitle));
        TName tit;
        tit.Format(_L("\"title=%S\""), &startInfo->iTitle);
        User::LeaveIfError(req->Append(tit));
        }
    
    User::LeaveIfError(
        iTestCombiner->TestModuleIf().RemoteSend( req->Message() ) );
        
    remote->iRemoteState = CRemoteTestCase::ECaseRunSent;
    remote->iSlaveId = aSlave->iSlaveDevId;
    remote->StartL();
    User::LeaveIfError( iTestCombiner->iTestCases.Append( remote ) );
    if( iTestCombiner->iLoopIsUsed )
        {
        User::LeaveIfError( iTestCombiner->iLoopAllocationArray.Append( remote ) );
        }
    CleanupStack::Pop( remote );
    
    CleanupStack::PopAndDestroy( startInfo );
    
    iTestCombiner->iRunningTests++;
    
    // Start timer    
    iRemoteTimer->SetTimerActive( iTestCombiner->iRemoteTimeout );

    iState = ERunnerRemote;

    CleanupStack::PopAndDestroy( req );

    return EFalse;
         
    }
    
    
/*
-------------------------------------------------------------------------------

     Class: CTestRunner

     Method: ExecuteRemoteTestCtlL

     Description: Handles remote testcase controlling
  
     Parameters:    CStifItemParser* aItem: in: script line
                    CTestCase* aTestCase: in: test case
                    TInt aCmd: in: remote command 
     
     Return Values: ETrue: continue script file execution
                    EFalse: stop script file execution and wait response

     Errors/Exceptions: Leaves on error situations.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
TBool CTestRunner::ExecuteRemoteTestCtlL( CStifItemParser* /* aItem */, 
                                          CTestCase* aTestCase, 
                                          TInt aCmd )
    {
    
    CRemoteTestCase* caseInfo = ( CRemoteTestCase* ) aTestCase;
    
    CStifTFwIfProt* req = CStifTFwIfProt::NewL();
    CleanupStack::PushL( req );
    req->CreateL();
    // Remote message
    User::LeaveIfError( 
        req->Append( CStifTFwIfProt::MsgType, CStifTFwIfProt::EMsgRemote ) );
    // Srcid. i.e. master id
    //req->AppendId( ( TUint32 ) this );
    User::LeaveIfError(
        req->AppendId( KRemoteProtocolMasterId ));
    // DstId, i.e.slave id
    User::LeaveIfError(
        req->AppendId( caseInfo->iSlaveId ));
    
    switch( aCmd )
        {
        case TTCKeywords::EPause:
            if( caseInfo->iRemoteState != CRemoteTestCase::ECaseRunning )
                {
                __TRACE( KError, (_L("Test case with testid %S not running"), 
                    &aTestCase->TestId() ));
                User::Leave( KErrGeneral );
                }
            // Pause command
            User::LeaveIfError(
                req->Append( CStifTFwIfProt::CmdType, CStifTFwIfProt::ECmdPause ) );
            caseInfo->iRemoteState = CRemoteTestCase::ECasePauseSent;
            break;
        case TTCKeywords::EResume:
            if( caseInfo->iRemoteState != CRemoteTestCase::ECasePaused )
                {
                __TRACE( KError, (_L("Test case with testid %S not paused"), 
                    &aTestCase->TestId() ));
                User::Leave( KErrGeneral );
                }
            // Resume command
            User::LeaveIfError( 
                req->Append( CStifTFwIfProt::CmdType, CStifTFwIfProt::ECmdResume ));
            caseInfo->iRemoteState = CRemoteTestCase::ECaseResumeSent;
            break;            
        case TTCKeywords::ECancel:
            if( ( caseInfo->iRemoteState != CRemoteTestCase::ECaseRunning ) &&
                ( caseInfo->iRemoteState != CRemoteTestCase::ECasePaused ) )
                {
                __TRACE( KError, (_L("Test case with testid %S not running"),
                    &aTestCase->TestId() ));
                User::Leave( KErrGeneral );
                }
            // Cancel command
            User::LeaveIfError( 
                req->Append( CStifTFwIfProt::CmdType, CStifTFwIfProt::ECmdCancel ));    
            caseInfo->iRemoteState = CRemoteTestCase::ECaseCancelSent;
            break;      
        case TTCKeywords::EComplete:
            if( caseInfo->iRemoteState == CRemoteTestCase::ECaseCompleted )
                {
                __TRACE( KError, (_L("Test case with testid %S already completed"), 
                    &aTestCase->TestId() ));
                CleanupStack::PopAndDestroy( req );
                return ETrue;
                }
            else
                {
                iTestCombiner->iWaitTestCase = aTestCase->TestId();
                CleanupStack::PopAndDestroy( req );
                return EFalse;
                }    
        default:
            // Should never come here
            User::Leave( KErrGeneral );            
        }
    
    User::LeaveIfError( 
        iTestCombiner->TestModuleIf().RemoteSend( req->Message() ) );

    // Start timer    
    iRemoteTimer->SetTimerActive( iTestCombiner->iRemoteTimeout );
            
    iState = ERunnerRemote;
    
    CleanupStack::PopAndDestroy( req );
    
    return EFalse;
   
    }

/*
-------------------------------------------------------------------------------

     Class: CTestRunner

     Method: ExecuteRemoteEventCtlL

     Description: Handles remote event controlling
  
     Parameters:    CStifItemParser* aItem: in: script line
                    CSlaveInfo* aSlave: in: slave info
                    TInt aCmd: in: remote command 
     
     Return Values: ETrue: continue script file execution
                    EFalse: stop script file execution and wait response

     Errors/Exceptions: Leaves on error situations.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
TBool CTestRunner::ExecuteRemoteEventCtlL( CStifItemParser* aItem, 
                                           CSlaveInfo* aSlave, 
                                           TInt aCmd )
    {
    
    TPtrC eventName;
    // Get event name
    TInt ret = aItem->GetNextString( eventName );
    if( ret != KErrNone )
        {
        __TRACE( KError, (_L("Event name was not given for remote")));
        User::Leave( KErrArgument );
        }
    
    if( aCmd == TTCKeywords::EWait )
        {
        TEventTc* event = aSlave->GetEvent( eventName );
        if( event == NULL )
            {
            __TRACE( KError, (_L("Waited event %S not requested"), 
                &eventName  ) );
            User::Leave( KErrNotFound );
            }
        iState = ERunnerRunning;
        SetActive();
        event->WaitEvent( iStatus );     
        // Execution continue if waited event is set or after it is set       
        return EFalse;
        }
    
    CStifTFwIfProt* req = CStifTFwIfProt::NewL();
    CleanupStack::PushL( req );
    
    req->CreateL();
    // Remote message
    User::LeaveIfError( 
        req->Append( CStifTFwIfProt::MsgType, CStifTFwIfProt::EMsgRemote ));
    // Srcid. i.e. master id
    User::LeaveIfError( req->AppendId( aSlave->iMasterId ) );
    // DstId, i.e.slave device id
    User::LeaveIfError( req->AppendId( aSlave->iSlaveDevId ) );
           
    switch( aCmd )
        {       
        // Event control cases
        case TTCKeywords::ERequest:
            {
            TEventTc* event = aSlave->GetEvent( eventName );
            if( event != NULL )
                {
                __TRACE( KError, (_L("Event %S already requested"), 
                    &eventName  ) );
                User::Leave( KErrNotFound );
                }
            event = new( ELeave ) TEventTc( (TName&)eventName, 
                                             iTestCombiner->iLog );
            CleanupStack::PushL( event );
            User::LeaveIfError( aSlave->iEvents.Append( event ) );
            CleanupStack::Pop( event );
            // Request event
            User::LeaveIfError( 
                req->Append( CStifTFwIfProt::CmdType, CStifTFwIfProt::ECmdRequest ));  
            }
            break;
                        
        case TTCKeywords::ERelease:
            {
            TEventIf* event = NULL;
            TInt count = aSlave->iEvents.Count();
            TInt i = 0;
            for( ; i < count; i++ )
                {
                if( aSlave->iEvents[i]->Name() == eventName )
                    {
                    event = aSlave->iEvents[i];
                    break;
                    }
                }
            if( event == NULL )
                {
                __TRACE( KError, (_L("Event not found %S"), 
                    &eventName  ) );
                User::Leave( KErrNotFound );
                }
                                                  
            aSlave->iEvents.Remove(i);
            delete event;
            // Release event
            User::LeaveIfError(
                req->Append( CStifTFwIfProt::CmdType, CStifTFwIfProt::ECmdRelease ));  
            }
            break;
        
        case TTCKeywords::ESet:
        	{
        		__TRACE( KPrint, (_L("JIRA51 CTestRunner::ExecuteRemoteEventCtlL case ESet")));
	            User::LeaveIfError(req->Append(CStifTFwIfProt::CmdType, CStifTFwIfProt::ECmdSetEvent));
        		break;
        	}
       	case TTCKeywords::EUnset:
        	{
        		__TRACE( KPrint, (_L("JIRA51 CTestRunner::ExecuteRemoteEventCtlL case EUnset")));
	            User::LeaveIfError(req->Append(CStifTFwIfProt::CmdType, CStifTFwIfProt::ECmdUnsetEvent));
        		break;
        	}
        default:
             // Should never come here
            User::Leave( KErrGeneral );            
        }
        
    // Event name
    User::LeaveIfError( req->Append( eventName ) );  
    
	if(aCmd == TTCKeywords::ESet)
		{
 		TPtrC stateEvent;
		TInt ret = aItem->GetNextString(stateEvent);
		if(ret == KErrNotFound) //indication event - add indicate keyword to message
			{
	            User::LeaveIfError(req->Append(CStifTFwIfProt::EventType, TEventIf::EIndication));
			}
		else if(ret == KErrNone) //possibly state event
			{
			if(stateEvent.Compare(_L("state")) == 0) //state event - add state keyword to message
				{
	            User::LeaveIfError(req->Append(CStifTFwIfProt::EventType, TEventIf::EState));
				}
			else //syntax error in the line
				{
		        __TRACE(KError, (_L("Unknown keyword %S"), &stateEvent));
				}
			}
		else //syntax error in the line
			{
	        __TRACE(KError, (_L("Unknown keyword (2) %S"), &stateEvent));
			}
		}
    User::LeaveIfError( 
        iTestCombiner->TestModuleIf().RemoteSend( req->Message() ) );
                
    // Start timer    
    iRemoteTimer->SetTimerActive( iTestCombiner->iRemoteTimeout );

    iState = ERunnerRemote;
    
    CleanupStack::PopAndDestroy( req );
    
    return EFalse;
    
    }    

TBool CTestRunner::ExecuteRemoteSetUnsetEventL(CStifItemParser* aItem,
                                               CSlaveInfo* aSlave,
                                               TInt aCmd)
	{
	TPtrC eventName;
	// Get event name
	TInt ret = aItem->GetNextString(eventName);
	if(ret != KErrNone)
		{
		__TRACE(KError, (_L("Event name was not given for remote")));
		User::Leave(KErrArgument);
		}
	// Check if this is a state set, indication set, or unset command
	TInt caseNumber = -1;
	if(aCmd == TTCKeywords::ESet)
		{
 		TPtrC stateEvent;
		TInt ret = aItem->GetNextString(stateEvent);
		if(ret == KErrNotFound) //indication event - add indicate keyword to message
			{
			caseNumber = 1;
			}
		else if(ret == KErrNone) //possibly state event
			{
			if(stateEvent.Compare(_L("state")) == 0) //state event - add state keyword to message
				{
				caseNumber = 0;
				}
			else //syntax error in the line
				{
				__TRACE(KError, (_L("Unknown keyword %S"), &stateEvent));
				}
			}
		else //syntax error in the line
			{
			}
		}
	else //TTCKeyword::EUnset
		{
		caseNumber = 2;
		}

	if(caseNumber == -1)
		{
			__TRACE(KError, _L("Should never occur"));
		}
	// Build new descriptor with command to run hardcoded suevent test case on remote phone
	HBufC* cmd = HBufC::NewL(100);
	CleanupStack::PushL(cmd);
	TPtr cmdPtr(cmd->Des());
	cmdPtr.Copy(eventName);
	// Run remotely test case
RDebug::Print(_L("CTestRunner::ExecuteRemoteSetUnsetEventL calling ExecuteRemoteRun"));
	TBool retval = ExecuteRemoteRunL(aItem, aSlave, cmd, caseNumber);
	// Clean data
	CleanupStack::PopAndDestroy(cmd);
RDebug::Print(_L("CTestRunner::ExecuteRemoteSetUnsetEventL end"));
	return retval;
    }
/*
-------------------------------------------------------------------------------

     Class: CTestRunner

     Method: ExecuteRemoteSendReceiveL

     Description: Handles asynchronous remote sendreceive controlling
  
     Parameters:    CStifItemParser* aItem: in: script line
                    CSlaveInfo* aSlave: in: slave info

     Return Values: ETrue: continue script file execution
                    EFalse: stop script file execution and wait response

     Errors/Exceptions: Leaves on error situations.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
TBool CTestRunner::ExecuteRemoteSendReceiveL( CStifItemParser* aItem, 
                                           CSlaveInfo* aSlave )
    {
    CStifTFwIfProt* req = CStifTFwIfProt::NewL();
    CleanupStack::PushL( req );
    
    // Create CRemoteSendReceive object for taking sendreceive information to
    // save for later use.
    CRemoteSendReceive* remoteSendReceive = 
        CRemoteSendReceive::NewL( iTestCombiner );
    
    CleanupStack::PushL( remoteSendReceive );

    req->CreateL();
    // Remote message
    User::LeaveIfError(
        req->Append( CStifTFwIfProt::MsgType, CStifTFwIfProt::EMsgRemote ) );
    // Srcid. i.e. master id
    User::LeaveIfError( req->AppendId( aSlave->iMasterId ) );
    // DstId, i.e.slave device id
    User::LeaveIfError( req->AppendId( aSlave->iSlaveDevId ) );
    // Run command
    User::LeaveIfError( req->Append( 
        CStifTFwIfProt::CmdType, CStifTFwIfProt::ECmdSendReceive ) );
    // asynchronous sendreceive's parameters    
    TPtrC tmp;    

    while( aItem->GetNextString( tmp ) == KErrNone )
        {        
        // Append parameters
        User::LeaveIfError( req->Append( tmp ) );  
        }
    
    User::LeaveIfError( 
        iTestCombiner->TestModuleIf().RemoteSend( req->Message() ) );

    remoteSendReceive->iRemoteState = CRemoteSendReceive::ECaseSend;
    remoteSendReceive->iSlaveId = aSlave->iSlaveDevId;

    // Take CRemoteSendReceive object to save in array. This can be used in
    // TestCombiner when handling responses.
    User::LeaveIfError( 
            iTestCombiner->iSendReceive.Append( remoteSendReceive ) );
    if( iTestCombiner->iLoopIsUsed )
        {
        User::LeaveIfError( 
            iTestCombiner->iLoopAllocationArray.Append( remoteSendReceive ) );
        }
    CleanupStack::Pop( remoteSendReceive );

    // Start timer    
    iRemoteTimer->SetTimerActive( iTestCombiner->iRemoteTimeout );

    iState = ERunnerRemote;
    
    CleanupStack::PopAndDestroy( req );
    
    // Return EFalse=>start waiting response...
    return EFalse;

    }

/*
-------------------------------------------------------------------------------

     Class: CTestRunner

     Method: ExecuteRemoteUnknownL

     Description: Forwards messages as such
  
     Parameters:    CStifItemParser* aItem: in: script line
                    CSlaveInfo* aSlave: in: slave info
                    TInt aCmd: in: remote command 
     
     Return Values: ETrue: continue script file execution
                    EFalse: stop script file execution and wait response

     Errors/Exceptions: Leaves on error situations.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
TBool CTestRunner::ExecuteRemoteUnknownL( CStifItemParser* aItem, 
                                          CSlaveInfo* aSlave,
                                          TDesC& aCommand )
    {
    
    CStifTFwIfProt* req = CStifTFwIfProt::NewL();
    CleanupStack::PushL( req );
    
    req->CreateL();
    // Remote message
    User::LeaveIfError(
        req->Append( CStifTFwIfProt::MsgType, CStifTFwIfProt::EMsgRemote ) );
    // Srcid. i.e. master id
    User::LeaveIfError( req->AppendId( aSlave->iMasterId ) );
    // DstId, i.e.slave device id
    User::LeaveIfError( req->AppendId( aSlave->iSlaveDevId ) );
    
    // Append command name
    User::LeaveIfError( req->Append( aCommand ) );
    
    TPtrC tmp;         
    while( aItem->GetNextString( tmp ) == KErrNone )
        {        
        // Append parameters
        User::LeaveIfError( req->Append( tmp ) );  
        }
    
    User::LeaveIfError( 
        iTestCombiner->TestModuleIf().RemoteSend( req->Message() ) );
                
    // Start timer    
    iRemoteTimer->SetTimerActive( iTestCombiner->iRemoteTimeout );

    iState = ERunnerRemote;
    
    CleanupStack::PopAndDestroy( req );
    
    return EFalse;
    
    }    
/*
-------------------------------------------------------------------------------

     Class: CTestRunner

     Method: ReceiveResponseL

     Description: Handles responce received from slave
  
     Parameters:    TDesC& aMsg: in: message
     
     Return Values: ETrue: continue script file execution
                    EFalse: stop script file execution

     Errors/Exceptions: Leaves on error situations.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
TBool CTestRunner::ReceiveResponseL( TDesC& aMsg )
    {
    __TRACE( KMessage, (_L("ReceiveResponse")));
    
    iRemoteTimer->Cancel();
    
    CStifTFwIfProt* msg = CStifTFwIfProt::NewL();
    CleanupStack::PushL( msg );
    TRAPD( err, msg->SetL( aMsg ); );
    if( err != KErrNone )
        {
        __TRACE( KError, (_L("Response parsing failed")));
        User::Leave( err );
        }
    
    // Check protocol identifiers
    if( ( msg->SrcDevId() == 0 ) ||
        ( msg->DstDevId() == 0 ) ||
        ( msg->DstTestId() == 0 ) )
        {
        __TRACE( KError, (_L("Illegal deviceid received")));
        User::Leave( KErrGeneral );
        }
    
    // This is master, cannot receive anything else but responses
    if( msg->iMsgType != CStifTFwIfProt::EMsgResponse )
        {
        __TRACE( KError, (_L("Illegal message received %d"), 
            msg->iMsgType ));
        User::Leave( KErrGeneral );        
        }
        
    TBool continueTask = ETrue;
    switch( msg->iRespType )
        {
        case CStifTFwIfProt::EMsgReserve:
            {
            __TRACE( KMessage, (_L("ReceiveResponse Reserve")));
            if( iState != ERunnerAllocate )
                {
                __TRACE( KError, (_L("Response reserve received in illegal state %d"), 
                    iState ));
                User::Leave( KErrGeneral );    
                }
            // Check protocol Src test id
            if( msg->SrcTestId() != 0 )
                {
                __TRACE( KError, (_L("Illegal deviceid received")));
                User::Leave( KErrGeneral );
                }
            if( msg->iResult != KErrNone )
                {
                __TRACE( KError, (_L("Response with error %d"), msg->iResult ));
                User::Leave( msg->iResult );
                }
            CSlaveInfo* slave = NULL;
            TInt count = iTestCombiner->iSlaveArray.Count();
            for( TInt index = 0; index < count; index++ )
                {
                slave = iTestCombiner->iSlaveArray[index];
                if( ( slave->iSlaveDevId == 0 ) &&
                    ( slave->iState ==  CSlaveInfo::ESlaveReserveSent ) )
                    {
                    break;
                    }
                slave = NULL;
                }
            if( slave == NULL )
                {
                User::Leave( KErrNotFound );
                }
            slave->iSlaveDevId = msg->SrcId();
            slave->iState = CSlaveInfo::ESlaveReserved;
            __TRACE( KMessage, (_L("Slave allocated succesfully, continue execution")));
            }
            break;
        case CStifTFwIfProt::EMsgRelease:
            {
            __TRACE( KMessage, (_L("ReceiveResponse Release")));
            if( iState != ERunnerFree )
                {
                __TRACE( KError, (_L("Response release received in illegal state %d"), 
                    iState ));
                User::Leave( KErrGeneral );    
                }
            // Check protocol Src test id
            if( msg->SrcTestId() != 0 )
                {
                __TRACE( KError, (_L("Illegal deviceid received")));
                User::Leave( KErrGeneral );
                }                
            if( msg->iResult != KErrNone )
                {
                __TRACE( KError, (_L("Response with error %d"), msg->iResult ));
                User::Leave( msg->iResult );
                }

            CSlaveInfo* slave = iTestCombiner->GetSlave( msg->SrcId() );
            if( slave == NULL )
                {
                User::Leave( KErrNotFound );
                }
            slave->iState = CSlaveInfo::ESlaveReleased;
            __TRACE( KMessage, (_L("Slave freed succesfully, continue execution")));
            }            
            break;
        case CStifTFwIfProt::EMsgRemote:
            {         
            __TRACE( KMessage, (_L("ReceiveResponse Remote")));
            switch( msg->iCmdType )
                {
                case CStifTFwIfProt::ECmdRun:
                    continueTask = ReceiveResponseRunL( *msg );
                    break;
                case CStifTFwIfProt::ECmdPause:
                case CStifTFwIfProt::ECmdResume:
                case CStifTFwIfProt::ECmdCancel:
                    continueTask = ReceiveResponseTestCtlL( *msg );
                    break;
                case CStifTFwIfProt::ECmdRequest:
                case CStifTFwIfProt::ECmdRelease:
                case CStifTFwIfProt::ECmdSetEvent:
                case CStifTFwIfProt::ECmdUnsetEvent:
                    continueTask = ReceiveResponseEventCtlL( *msg );                    
                    break;
                case CStifTFwIfProt::ECmdSendReceive:
                    continueTask = ReceiveResponseSendReceiveL( *msg );                    
                    break;
                default:
                    continueTask = ReceiveResponseUnknownL( *msg );                    
                    break;
                }
            }            
            break;
        default:
            User::Leave( KErrGeneral );
        } 
               
    
    CleanupStack::PopAndDestroy( msg );    
    return continueTask;
    
    }

/*
-------------------------------------------------------------------------------

     Class: CTestRunner

     Method: ReceiveResponseRunL

     Description: Handles response for run received from slave
  
     Parameters:    CStifTFwIfProt& aMsg: in: protocol message parser
     
     Return Values: ETrue: continue script file execution
                    EFalse: stop script file execution

     Errors/Exceptions: Leaves on error situations.

     Status: Draft
    
-------------------------------------------------------------------------------
*/    
TBool CTestRunner::ReceiveResponseRunL( CStifTFwIfProt& aMsg )
    {

    TPtrC tmp = CStifTFwIfProt::RunStatus(aMsg.iRunStatus); 
    __TRACE( KMessage, (_L("ReceiveResponse Remote Run %S"), &tmp ));

    TBool continueTask = ETrue; 
    switch( aMsg.iRunStatus )
        {
        case CStifTFwIfProt::ERunStarted:
            {
            // Locate testcase    
            CRemoteTestCase* tcase = 
                iTestCombiner->GetRemoteTestRunSent( GETDEVID( aMsg.SrcId() ) );
            if( tcase == NULL ) 
                {
                __TRACE( KError, (_L("Testcase not found")));
                User::Leave( KErrNotFound );        
                }
            tcase->iSlaveId = aMsg.SrcId();

            tcase->iRemoteState = CRemoteTestCase::ECaseRunning;
            }
            break;
        case CStifTFwIfProt::ERunError:
        case CStifTFwIfProt::ERunReady:
            {
            // Locate testcase    
            CRemoteTestCase* tcase = 
                iTestCombiner->GetRunningRemoteTest( aMsg.SrcId() );
            if( tcase == NULL ) 
                {
                __TRACE( KError, (_L("Testcase not found")));
                User::Leave( KErrNotFound );        
                }

            switch( aMsg.iResultCategory )
                {
                case CStifTFwIfProt::EResultNormal:
                    tcase->iResult.iCaseExecutionResultType = 
                        TFullTestResult::ECaseExecuted;
                    tcase->iResult.iTestResult.iResult = aMsg.iResult;
                    tcase->iResult.iCaseExecutionResultCode = 0;
                    break;
                case CStifTFwIfProt::EResultPanic:
                    tcase->iResult.iCaseExecutionResultType = 
                        TFullTestResult::ECasePanic;
                    tcase->iResult.iTestResult.iResult = KErrGeneral;
                    tcase->iResult.iCaseExecutionResultCode = aMsg.iResult;
                    break;
                case CStifTFwIfProt::EResultException:
                    tcase->iResult.iCaseExecutionResultType = 
                        TFullTestResult::ECaseException;
                    tcase->iResult.iTestResult.iResult = KErrGeneral;
                    tcase->iResult.iCaseExecutionResultCode = aMsg.iResult;
                    break;
                case CStifTFwIfProt::EResultTimeout:
                    tcase->iResult.iCaseExecutionResultType = 
                        TFullTestResult::ECaseTimeout;                
                    tcase->iResult.iTestResult.iResult = KErrGeneral;
                    tcase->iResult.iCaseExecutionResultCode = aMsg.iResult;
                    break;
                case CStifTFwIfProt::EResultLeave:
                    tcase->iResult.iCaseExecutionResultType = 
                        TFullTestResult::ECaseLeave;                
                    tcase->iResult.iTestResult.iResult = KErrGeneral;
                    tcase->iResult.iCaseExecutionResultCode = aMsg.iResult;
                    break;
                default:
                    User::Leave( KErrGeneral );
                }
                    
            if( ( tcase->iRemoteState == CRemoteTestCase::ECaseCancelled ) ||
                ( tcase->iRemoteState == CRemoteTestCase::ECaseRunSent ) )
                {
                // Complete for cancelled testcase or error for run request, 
                // set runner active again
                continueTask = ETrue; 
                }
            else
                {
                // Continued from Complete in state ECaseRunning
                continueTask = EFalse; 
                }
                
            tcase->iRemoteState = CRemoteTestCase::ECaseCompleted;

        __TRACE( KMessage, (_L("ReceiveResponse Remote Run rq comp")));
            
            TRequestStatus* rs = &tcase->iStatus;
            // Complete testcase
            User::RequestComplete( rs, KErrNone );
            
            }
            break;
        default:
            // Should never come here
            User::Leave( KErrGeneral );    
        }        
    
    return continueTask;
    
    }
    
/*
-------------------------------------------------------------------------------

     Class: CTestRunner

     Method: ReceiveResponseTestCtlL

     Description: Handles responses for test control commands 
        received from slave
  
     Parameters:    CStifTFwIfProt& aMsg: in: protocol message parser
     
     Return Values: ETrue: continue script file execution
                    EFalse: stop script file execution

     Errors/Exceptions: Leaves on error situations.

     Status: Draft
    
-------------------------------------------------------------------------------
*/    
TBool CTestRunner::ReceiveResponseTestCtlL( CStifTFwIfProt& aMsg )
    {

    if( aMsg.iResult != KErrNone )
        {
        __TRACE( KError, (_L("Response with error %d"), aMsg.iResult ));
        User::Leave( aMsg.iResult );
        }
    
    // Locate testcase    
    CRemoteTestCase* tcase = iTestCombiner->GetRemoteTest( aMsg.SrcId() );
    if( tcase == NULL ) 
        {
        __TRACE( KError, (_L("Testcase not found")));
        User::Leave( KErrNotFound );        
        }
    
    TBool continueTask = ETrue;
    
    switch( aMsg.iCmdType )
        {
        case CStifTFwIfProt::ECmdPause:
            __TRACE( KMessage, (_L("ReceiveResponse Remote Pause")));

            if( tcase->iRemoteState != CRemoteTestCase::ECasePauseSent )
                {
                __TRACE( KError, (_L("Pause response received in illegal state")));
                User::Leave( KErrGeneral );                        
                }
            tcase->iRemoteState = CRemoteTestCase::ECasePaused;
            
            // Start pause timer if timeout was given 
            if( ( iPausedTestCase.Length() > 0 ) &&
                ( iPauseTime != 0 ) )
                {
                continueTask = EFalse;
                iState = ERunnerWaitTimeout;
                iPauseTimer.After( iStatus, iPauseTime*1000 );
                SetActive();         
                }
            break;
        case CStifTFwIfProt::ECmdResume:
            __TRACE( KMessage, (_L("ReceiveResponse Remote Resume")));

            if( tcase->iRemoteState != CRemoteTestCase::ECaseResumeSent )
                {
                __TRACE( KError, (_L("Resume response received in illegal state")));
                User::Leave( KErrGeneral );                        
                }
            tcase->iRemoteState = CRemoteTestCase::ECaseRunning;
            break;
        case CStifTFwIfProt::ECmdCancel:
            __TRACE( KMessage, (_L("ReceiveResponse Remote Cancel")));

            if( tcase->iRemoteState != CRemoteTestCase::ECaseCancelSent )
                {
                __TRACE( KError, (_L("Cancel response received in illegal state")));
                User::Leave( KErrGeneral );                        
                }
            tcase->iRemoteState = CRemoteTestCase::ECaseCancelled;
            // Need to wait Run response with KErrCancel
            continueTask = EFalse;
            // Start timer    
            iRemoteTimer->SetTimerActive( iTestCombiner->iRemoteTimeout );
            break;
        default:
            // Should never come here
            User::Leave( KErrGeneral );
        }
    
    return continueTask;
    }

/*
-------------------------------------------------------------------------------

     Class: CTestRunner

     Method: ReceiveResponseEventCtlL

     Description: Handles responses for event system control commands 
        received from slave
  
     Parameters:    CStifTFwIfProt& aMsg: in: protocol message parser
     
     Return Values: ETrue: continue script file execution
                    EFalse: stop script file execution 

     Errors/Exceptions: Leaves on error situations.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
TBool CTestRunner::ReceiveResponseEventCtlL( CStifTFwIfProt& aMsg )
    {
        
    CSlaveInfo* slave = iTestCombiner->GetSlave( aMsg.SrcId() );
    if( slave == NULL )
        {
        User::Leave( KErrNotFound );
        }
    
    TBool continueTask = ETrue;
    switch( aMsg.iCmdType )
        {
        case CStifTFwIfProt::ECmdRequest:
            {
            __TRACE( KMessage, (_L("ReceiveResponse Request")));
 
            TEventTc* event = slave->GetEvent( aMsg.iEventName );
            if( event == NULL )
                {
                User::Leave( KErrNotFound );
                }
            switch( aMsg.iEventStatus )
                {
                case CStifTFwIfProt::EEventActive:
                    __TRACE( KMessage, (_L("Event %S active"), &aMsg.iEventName ));
                    break;
                case CStifTFwIfProt::EEventSet:
                    __TRACE( KMessage, (_L("Event %S set"), &aMsg.iEventName ));
                    // Set event
                    event->SetEvent( aMsg.iEventType );
                    continueTask = EFalse;
                    break;
                case CStifTFwIfProt::EEventError:
                    __TRACE( KMessage, (_L("Event %S error %d"), 
                        &aMsg.iEventName, aMsg.iResult ));
                    User::Leave( aMsg.iResult );
                default:
                    User::Leave( KErrGeneral );
                }
            }
            break;
        case CStifTFwIfProt::ECmdRelease:
            __TRACE( KMessage, (_L("ReceiveResponse Release")));
            if( aMsg.iResult != KErrNone )
                {
                __TRACE( KError, (_L("Response with error %d"), aMsg.iResult ));
                User::Leave( aMsg.iResult );
                }

            // Everything ok, no need to do anything
            break;
        case CStifTFwIfProt::ECmdSetEvent:
            __TRACE( KMessage, (_L("ReceiveResponse SetEvent")));
            if( aMsg.iResult != KErrNone )
                {
                __TRACE( KError, (_L("Response with error %d"), aMsg.iResult ));
                User::Leave( aMsg.iResult );
                }

            // Everything ok, no need to do anything
            break;
        case CStifTFwIfProt::ECmdUnsetEvent:
            __TRACE( KMessage, (_L("ReceiveResponse Unset")));
            if( aMsg.iResult != KErrNone )
                {
                __TRACE( KError, (_L("Response with error %d"), aMsg.iResult ));
                User::Leave( aMsg.iResult );
                }
            // Everything ok, no need to do anything
            break;
        default:
            // Should never come here
            User::Leave( KErrGeneral );
        }

    return continueTask;
    
    }

/*
-------------------------------------------------------------------------------

     Class: CTestRunner

     Method: ReceiveResponseSendReceiveL

     Description: Handles responses for asynchronous sendreceive commands 
                  received from slave
  
     Parameters: CStifTFwIfProt& aMsg: in: protocol message parser
     
     Return Values: ETrue: continue script file execution
                    EFalse: stop script file execution 

     Errors/Exceptions: Leaves on error situations.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
TBool CTestRunner::ReceiveResponseSendReceiveL( CStifTFwIfProt& aMsg )
    {
        
    TPtrC tmp = CStifTFwIfProt::RunStatus(aMsg.iRunStatus); 
    __TRACE( KMessage, (
        _L("ReceiveResponseSendReceiveL asynchronous Remote SendReceive %S"),
        &tmp ) );

    TBool continueTask = EFalse; 
    switch( aMsg.iRunStatus )
        {
        case CStifTFwIfProt::ERunStarted:
            {
            // Locate CRemoteSendReceive object
            CRemoteSendReceive* sendreceive = 
                            iTestCombiner->GetRemoteSendReceive( GETDEVID(
                            aMsg.SrcId() ) );
            if( sendreceive == NULL ) 
                {
                __TRACE( KError, (_L("CRemoteSendReceive object not found")));
                User::Leave( KErrNotFound );        
                }
            sendreceive->iRemoteState = CRemoteSendReceive::ECaseSend;

            // continueTask is EFalse=>stop script file execution
            break;
            }
        case CStifTFwIfProt::ERunError:
        case CStifTFwIfProt::ERunReady:
            {
            if( aMsg.iResult != KErrNone )
                    {
                    __TRACE( KError, (_L("sendreceive response with error %d"), aMsg.iResult ));
                    User::Leave( aMsg.iResult );
                    }

            // Locate CRemoteSendReceive object
            CRemoteSendReceive* sendreceive = 
                iTestCombiner->GetRemoteSendReceive( aMsg.SrcId() );
            if( sendreceive == NULL ) 
                {
                __TRACE( KError, (_L("CRemoteSendReceive object not found")));
                User::Leave( KErrNotFound );        
                }

            // continueTask is ETrue=>continue script file execution
            continueTask = ETrue;
              
            sendreceive->iRemoteState = CRemoteSendReceive::ECaseCompleted;

            __TRACE( KMessage, (
                _L( "ReceiveResponseSendReceiveL asynchronous Remote SendReceive rq comp" ) ) );
            break;
            }
        default:
            {
            // Should never come here
            User::Leave( KErrGeneral );    
            }
        }
    return continueTask;

    }

/*
-------------------------------------------------------------------------------

     Class: CTestRunner

     Method: ReceiveResponseUnknownL

     Description: Handles responses for unspecified commands
  
     Parameters:    CStifTFwIfProt& aMsg: in: protocol message parser
     
     Return Values: ETrue: continue script file execution
                    EFalse: stop script file execution 

     Errors/Exceptions: Leaves on error situations.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
TBool CTestRunner::ReceiveResponseUnknownL( CStifTFwIfProt& aMsg )
    {
        
    CSlaveInfo* slave = iTestCombiner->GetSlave( aMsg.SrcId() );
    if( slave == NULL )
        {
        User::Leave( KErrNotFound );
        }
    if( aMsg.iResult != KErrNone )
        {
        __TRACE( KError, (_L("Response with error %d"), aMsg.iResult ));
        User::Leave( aMsg.iResult );
        }
    
    return ETrue;
    
    }

/*
-------------------------------------------------------------------------------

     Class: CTestRunner

     Method: ParseOptArgL

     Description: Parses optional argument 
  
     Parameters: const TDesC& aOptArg: in: 
                    argument-value pair (format arg=value)
                 TPtrC& aArg: out: parsed argument  
                 TPtrC& aVal: out: parsed value
     
     Return Values: None

     Errors/Exceptions: Leaves if parsing fails.

     Status: Approved
    
-------------------------------------------------------------------------------
*/
void CTestRunner::ParseOptArgL( const TDesC& aOptArg, TPtrC& aArg, TPtrC& aVal)
    { 
    _LIT( KErrMsgUnknownOrIllegalKeyword, "Unknown or illegal argument %S" );
    _LIT( KErrMsgValueNotDefined, "Value of optional argument %S is not defined" );
    TInt length = aOptArg.Length();
    for( TInt i=0; i < length; i++) 
        {
        // find the '=' sign 
        if( aOptArg[i] == '=' )
            {
            if( i+1 >= length )
                {
                __TRACE( KError, 
                    (_L("Illegal optional argument(%S), no value"), 
                    &aOptArg ));    
				TPtrC tmp = aOptArg.Left( i );
                iRunErrorMessage.Format( KErrMsgValueNotDefined, &tmp );
                User::Leave( KErrArgument );
                }
            aArg.Set( aOptArg.Left( i ) );
            aVal.Set( aOptArg.Mid( i+1 ) );
            __TRACE( KMessage, (  _L( "arg '%S', val '%S'" ),
                &aArg, &aVal ));        
            return;
            }
        }
    __TRACE( KError, (_L("Illegal optional argument(%S)"), &aOptArg ));    
    iRunErrorMessage.Format( KErrMsgUnknownOrIllegalKeyword, &aOptArg );
    User::Leave( KErrArgument );
    
    }     
     
/*
-------------------------------------------------------------------------------

     Class: CTestRunner

     Method: CancelTestCases

     Description: Cancels all running testcases
  
     Parameters:    None
     
     Return Values: None

     Errors/Exceptions: None.

     Status: Approved
    
-------------------------------------------------------------------------------
*/
void CTestRunner::CancelTestCases()
    {
    __TRACEFUNC();

    TInt count = iTestCombiner->iTestCases.Count();
    for( TInt i=0; i < count; i++ )
        {
        if( iTestCombiner->iTestCases[i]->State() == 
            CTCTestCase::ETestCaseRunning )
            {
            iTestCombiner->iTestCases[i]->Cancel();
            }
        }
        
    if( ( iTestCombiner->iRunningTests == 0 ) &&
        iTestCombiner->iSchedulerActive ) 
        {
        // Stop execution
        CActiveScheduler::Current()->Stop();
        iTestCombiner->iSchedulerActive = EFalse;
        return;
        }
        
    }

/*
-------------------------------------------------------------------------------

     Class: CTestRunner

     Method: SetRunnerActive

     Description: Set CTestRunner active and complete.
  
     Parameters:    None.

     Return Values: None.

     Errors/Exceptions: None.

     Status: Approved
    
-------------------------------------------------------------------------------
*/
void CTestRunner::SetRunnerActive()
     {
     __TRACEFUNC();
     
     if( IsActive() )
        {
        __TRACE( KError, ( _L("Runner already active %d"), iState ));
        User::Panic( KTestRunner, KErrInUse );
        }
 
     // Update state
     iState = ERunnerRunning;
 
     iStatus = KRequestPending;
     TRequestStatus* rs = &iStatus;
     SetActive();
     User::RequestComplete( rs, KErrNone );
     
     }
     
/*
-------------------------------------------------------------------------------

     Class: CTestRunner

     Method: CheckUnsetEvent

     Description: Check unset event.
     
     Parameters: None
         
     Return Values: ETrue: Unset event completed
                    EFalse: No unset event pending

     Errors/Exceptions: None

     Status: Approved
    
-------------------------------------------------------------------------------
*/
TBool CTestRunner::CheckUnsetEvent()
    {
    if( iEvent.Name().Length() == 0 )
        {
        return EFalse;
        }
        
    __TRACE( KMessage, (_L("Unset event completed") ));
    // Check if some testmodule below has still event request pending
    if( iTestCombiner->UnsetEvent( iEvent, iStatus ) == EFalse )
        {
         // No event request pending
        // then check other testmodules (may block)
        TInt res = iTestCombiner->TestModuleIf().Event( iEvent );
        if( res != KErrNone )
            {
            iTestCombiner->iResult = res;
            }
            
        __TRACE( KPrint, (_L("Unset: Complete") ) );
        iEvent.SetName( _L("") );
        // Proceed testcase section execution
        SetRunnerActive();
        }    
    else
        {
        iState = ERunnerWaitUnset;
        // Wait for unset to complete 
        SetActive();
        }
         
    return ETrue;
    }


/*
-------------------------------------------------------------------------------

     Class: CTestRunner

     Method: ExecuteLoopL

     Description: Handle the loop keyword operations.

     Parameters: CStifItemParser* aItem: in: Pointer to parsed item object.

     Return Values: None.

     Errors/Exceptions: None.

     Status: Proposal

-------------------------------------------------------------------------------
*/
void CTestRunner::ExecuteLoopL( CStifItemParser* aItem )
    {
    _LIT( KErrMsgLoopNestedLoop, "Loop: Nested loops are not supported " );
    _LIT( KErrMsgLoopInvalidLoopCountParam, "Loop: No loop count value given for loop or value has invalid format" );
    _LIT( KErrMsgLoopUnknownUnexpectedOption, "Loop: Unknown or unexpected loop option");
    _LIT( KErrMsgLoopPasslimitInvalidValue, "Loop: No passlimit value given for loop or value has invalid format" );
    _LIT( KErrMsgLoopPasslimitNotInRange, "Loop: Passlimit value is lower than 0 or higher than loop count" );
    __TRACEFUNC();

    if( iLoopTimes != 0 )
        {
        __TRACE( KError, (_L("ExecuteLoopL: Nested loop are not supported")));
        iRunErrorMessage = KErrMsgLoopNestedLoop;
        User::Leave( KErrNotSupported );
        }

    iLoopTimes = 0;
    iLoopCounter = 0;
    iPasslimitEnabled = EFalse;
    iTimedLoop = EFalse;
    
    if( aItem->GetNextInt( iLoopTimes ) != KErrNone )
        {
        __TRACE( KError, (_L("ExecuteLoopL: No loop count value given for loop")));
        iRunErrorMessage = KErrMsgLoopInvalidLoopCountParam;
        User::Leave( KErrArgument );
        }
    __TRACE( KMessage, (_L("ExecuteLoopL: Loop for %d times" ), iLoopTimes ) );

    //Check loop options
    TPtrC option;
    TInt ret = aItem->GetNextString(option); 
    if(ret == KErrNone)
        {
        if(option.Compare(_L("msec")) == 0) //time loop option
            {
            iTimedLoop = ETrue;
            iStartTime.HomeTime();
            iExpectedLoopTime = TInt64(iLoopTimes) * TInt64(1000); //convert to micro seconds
            __TRACE(KMessage, (_L("ExecuteLoopL: Timed loop for %d msec" ), iLoopTimes));
            
            ret = aItem->GetNextString(option); //Get next option
            }
        }
        
    if(ret == KErrNone)
        {
        if(option.Compare(_L("passlimit")) == 0) //passlimit option
            {
            iPasslimit = 0;
            if( aItem->GetNextInt( iPasslimit ) != KErrNone )
                {
                __TRACE( KError, ( _L( "ExecuteLoopL: No passlimit value given for loop." ) ) );
                iRunErrorMessage = KErrMsgLoopPasslimitInvalidValue;
                User::Leave( KErrArgument );
                }
            __TRACE( KMessage, ( _L( "ExecuteLoopL: Passlimit set on %d" ), iPasslimit ) );
            //Check if passlimit has valid value
            if(iPasslimit < 0 || (iPasslimit > iLoopTimes && !iTimedLoop))
                {
                __TRACE( KError, ( _L( "ExecuteLoopL: Passlimit value is lower than 0 or higher than loop count." ) ) );
                iRunErrorMessage = KErrMsgLoopPasslimitNotInRange;
                User::Leave( KErrArgument );
                }
            iPasslimitEnabled = ETrue;

            ret = aItem->GetNextString(option); //Get next option
            }
        }
        
    if(ret == KErrNone)
        {
        __TRACE( KError, ( _L( "ExecuteLoopL: Unknown or unexpected loop option [%S]" ), &option ) );
        iRunErrorMessage = KErrMsgLoopUnknownUnexpectedOption;
        User::Leave( KErrNotSupported );
        }

	iPassedIterationCnt = 0;

    iLoopStartPos = iTestCombiner->iSectionParser->GetPosition();
    }

/*
-------------------------------------------------------------------------------

     Class: CTestRunner

     Method: ExecuteEndLoopL

     Description: Handle the endloop keyword operations.

     Parameters: None.

     Return Values: TBool: Boolean value for indicate can testing continue.

     Errors/Exceptions: None.

     Status: Proposal

-------------------------------------------------------------------------------
*/
TBool CTestRunner::ExecuteEndLoopL()
    {
    __TRACEFUNC();

    // Fail test case if there was no loop started
    if(iTestCombiner->iLoopIsUsed == EFalse)
        {
        __TRACE(KError, (_L("Encountered \'endloop\' without \'loop\'. Aborting test case.")));
        iTestCombiner->iResult = KErrGeneral;
        iState = ERunnerError;
        CancelTestCases();
        return ETrue; // Test case file parsing can be continue
        }

	TBool iterationFailed = EFalse; //Has last iteration failed (at least one of test cases has failed)

    // First we check is all test cases that is set to run inside the loop
    // completed. If not completed we wait until test case completes.
    // Is some loop executions fails that is not allowed then loop execution
    // will be stopped and error result is given to TestCombiner

    for( TInt s = 0; s < iTestCombiner->iTestCases.Count(); s++ )
        {
        CTestCase* testCase = (CTestCase*)iTestCombiner->iTestCases[s]; 
        if( testCase == NULL )
            {
            __TRACE( KError, (_L("ExecuteEndLoopL: CTestCase object not found") ) );
            return ETrue;
            }
        // Check that testCase object is allocated inside the loop
        TBool isLoopTestCase( EFalse );
        for( TInt p = 0; p < iTestCombiner->iLoopAllocationArray.Count(); p++ )
            {
            if( iTestCombiner->iLoopAllocationArray[p] == testCase )
                {
                isLoopTestCase = ETrue;
                break;
                }
            }
        // testCase object is allocated inside loop
        if( isLoopTestCase )
            {
            // Test case is completed
            //if( testCase->State() == CTCTestCase::ETestCaseCompleted )
            if(testCase->IsCompletelyFinished())
                {
                // Check normal test result
                if( testCase->iExpectedResultCategory == TFullTestResult:: ECaseExecuted )
                    {
                    // Normal completion, check result
                    if( testCase->iResult.iTestResult.iResult != testCase->iExpectedResult )
                        {
                        __TRACE( KPrint, ( _L( "Test failed, expect(%d) != result(%d)"), 
                             testCase->iExpectedResult,
                             testCase->iResult.iTestResult.iResult ));
                        //If no passlimit is provided behave in old way
                        if( !iPasslimitEnabled )
                             {
                            // We return the first error result as aResult
                            if( testCase->iResult.iTestResult.iResult != KErrNone )
                                {
                                iTestCombiner->iScriptFailed = testCase->iResult.iCaseExecutionResultCode;
                                iTestCombiner->iScriptFailedDescription.Copy(testCase->iResult.iTestResult.iResultDes);
                                }
                            else
                                {
                                iTestCombiner->iScriptFailed = testCase->iResult.iCaseExecutionResultCode;
                                iTestCombiner->iScriptFailedDescription.Copy(_L("Test case has not finished with expected result (in loop)."));
                                }

                            iState = ERunnerError;
                            CancelTestCases();
                            return ETrue; // Test case file parsing can be continue
                            }
                        else
                            {
                            // Set flag that one of test cases has failed, so whole iteration is failed
                            iterationFailed = ETrue;
                            }
                        }
                    }
                // Abnormal completion, i.e. panic, leave, exception or timeout
                else 
                    {
                    if( testCase->iResult.iCaseExecutionResultCode != testCase->iExpectedResult )
                        {
                        __TRACE( KPrint, ( _L( "Test failed, expect errorcode(%d) != result(%d)"), 
                             testCase->iExpectedResult,
                             testCase->iResult.iCaseExecutionResultCode ) );
                        //If no passlimit is provided behave in old way
                        if( !iPasslimitEnabled )
                            {
                            // We return the first error result as aResult
                            iTestCombiner->iScriptFailed = testCase->iResult.iCaseExecutionResultCode;
                            iTestCombiner->iScriptFailedDescription.Copy(_L("Test case has not finished with expected execution error (in loop)."));

                            iState = ERunnerError;
                            // Return error from here
                            CancelTestCases();
                            return ETrue; // Test case file parsing can be continue
                            }
                        else
                            {
                            // Set flag that one of test cases has failed, so whole iteration is failed
                            iterationFailed = ETrue;
                            }
                        }

                    // Requested testcase is completed already,
                    // proceed testcase execution
                    __TRACE( KMessage, (_L("Already completed")));
                    }
                } // End "Test case is completed"
            // Test case is running state, set to wait the test case complete
            else if( testCase->State() == CTCTestCase::ETestCaseRunning )
                 {
                 // Wait testcase to complete. If no id there should generate
                 // id that test case complete will be handled correctly
                 if( testCase->TestId().Length() == 0 )
                    {
                    TPtrC generatedTestId( _L( "stif" ) );
                    delete testCase->iTestId;
                    testCase->iTestId = generatedTestId.Alloc();
                    }
                iTestCombiner->iWaitTestCase.Copy( testCase->TestId() );
                 // Stop testcase execution until testcase completed 
                 iState = ERunnerWaitTestCase;
                // Go to beginning of the endloop
                User::LeaveIfError(
                    iTestCombiner->iSectionParser->SetPosition( iEndLoopStartPos ));

                // Testing is ongoing, test case file parsing cannot
                // be continue. Next line will be run when some AO
                // will be complete and allow parsing to continue
                return EFalse; 
                }
            else if(testCase->State() == CTCTestCase::ETestCaseCompleted)
                {
                // Go to beginning of the endloop
                User::LeaveIfError(iTestCombiner->iSectionParser->SetPosition(iEndLoopStartPos));

                // Testing is ongoing, test case file parsing cannot
                // be continue. Next line will be run when some AO
                // will be complete and allow parsing to continue
                return ETrue;
                }
            else
                {
                // This should newer happen
                __TRACE( KError, (_L("ExecuteEndLoopL: Illegal branch") ) );
                }
            }
        } // end for-loop

    iLoopCounter++;
    __TRACE( KMessage, (_L("ExecuteLineL: Loop executed for %d times" ), 
            iLoopCounter ) );

    //If passlimit (endurance) is enabled, we must check if any of test case in this iteration has failed
    if( iPasslimitEnabled && !iterationFailed )
    	{
    	iPassedIterationCnt++;
    	}

    TTime currTime;
    currTime.HomeTime();
    //if( iLoopCounter < iLoopTimes )
    if(((!iTimedLoop) && (iLoopCounter < iLoopTimes)) //Normal loop
       ||
       iTimedLoop && (currTime.MicroSecondsFrom(iStartTime) < iExpectedLoopTime)) //Timed loop
        {
        // Go to beginning of the loop
        User::LeaveIfError(
            iTestCombiner->iSectionParser->SetPosition( iLoopStartPos ));
        }
    else
        {
        // End looping
        if( iPasslimitEnabled )
        	{
	        __TRACE(KMessage, (_L("ExecuteLoopL: Loop executed. Iterations: %d, passed: %d, expected: %d"), iLoopCounter, iPassedIterationCnt, iPasslimit));
        	}

        iLoopCounter = 0;
        iLoopTimes = 0;
        iLoopStartPos = 0;
        // Loop related initializations
        iTestCombiner->iLoopIsUsed = EFalse;
        //--LOOPBUG-- Do not zero counter because there could be some test cases run before the loop (Stif-83)
        //--LOOPBUG-- iTestCombiner->iRunningTests = 0;

        //If passlimit was given and number of passed test is less then expected, stop execution of combiner's test case
        if( iPasslimitEnabled && iPassedIterationCnt < iPasslimit )
        	{
	        __TRACE( KMessage, ( _L( "ExecuteLoopL: Loop has failed (passlimit). Finishing with KErrCompletion." ) ) );
            iTestCombiner->iScriptFailed = KErrCompletion;
            iTestCombiner->iScriptFailedDescription = _L("Loop has not reached passlimit requirement.");
            iState = ERunnerError;
            CancelTestCases();
            return ETrue; // Test case file parsing can be continue
        	}
        else if( iPasslimitEnabled && iPassedIterationCnt >= iPasslimit )
        	{
	        __TRACE( KMessage, ( _L( "ExecuteLoopL: Loop has passed (passlimit)" ) ) );
        	}
        iPassedIterationCnt = 0;
        iPasslimit = 0;
        iPasslimitEnabled = EFalse;
        }

    // Loop time is executed, free allocations that is allocated during loop
    TInt a( 0 );
    TInt b( 0 );

    for( b = 0; b < iTestCombiner->iLoopAllocationArray.Count(); b++ )
        {
        //for( a = 0; a < iTestCombiner->iLoopAllocationArray.Count(); a++ )
        for( a = 0; a < iTestCombiner->iTestCases.Count(); a++ )
            {
            if( a < iTestCombiner->iTestCases.Count() && iTestCombiner->iTestCases[a] == iTestCombiner->iLoopAllocationArray[b] )
                {
                delete iTestCombiner->iTestCases[a];
                iTestCombiner->iTestCases.Remove( a );
                }
            }
        }

    for( b = 0; b < iTestCombiner->iLoopAllocationArray.Count(); b++ )
        {
        //for( a = 0; a < iTestCombiner->iLoopAllocationArray.Count(); a++ )
        for( a = 0; a < iTestCombiner->iTestModules.Count(); a++ )
            {
            if( a < iTestCombiner->iTestModules.Count() && iTestCombiner->iTestModules[a] == iTestCombiner->iLoopAllocationArray[b] )
                {
                delete iTestCombiner->iTestModules[a];
                iTestCombiner->iTestModules.Remove( a );
                }
            }
        }

    for( b = 0; b < iTestCombiner->iLoopAllocationArray.Count(); b++ )
        {
        //for( a = 0; a < iTestCombiner->iLoopAllocationArray.Count(); a++ )
        for( a = 0; a < iTestCombiner->iEventArray.Count(); a++ )
            {
            if( a < iTestCombiner->iEventArray.Count() && iTestCombiner->iEventArray[a] == iTestCombiner->iLoopAllocationArray[b] )
                {
                delete iTestCombiner->iEventArray[a];
                iTestCombiner->iEventArray.Remove( a );
                }
            }
        }
    for( b = 0; b < iTestCombiner->iLoopAllocationArray.Count(); b++ )
        {
        //for( a = 0; a < iTestCombiner->iLoopAllocationArray.Count(); a++ )
        for( a = 0; a < iTestCombiner->iSlaveArray.Count(); a++ )
            {
            if( a < iTestCombiner->iSlaveArray.Count() && iTestCombiner->iSlaveArray[a] == iTestCombiner->iLoopAllocationArray[b] )
                {
                delete iTestCombiner->iSlaveArray[a];
                iTestCombiner->iSlaveArray.Remove( a );
                }
            }
        }
    for( b = 0; b < iTestCombiner->iLoopAllocationArray.Count(); b++ )
        {
        //for( a = 0; a < iTestCombiner->iLoopAllocationArray.Count(); a++ )
        for( a = 0; a < iTestCombiner->iSendReceive.Count(); a++ )
            {
            if( a < iTestCombiner->iSendReceive.Count() && iTestCombiner->iSendReceive[a] == iTestCombiner->iLoopAllocationArray[b] )
                {
                delete iTestCombiner->iSendReceive[a];
                iTestCombiner->iSendReceive.Remove( a );
                }
            }
        }

    // Test operation can be continued
    return ETrue; // Test case file parsing can be continue

    }

/*
-------------------------------------------------------------------------------

    DESCRIPTION

    This module contains the implementation of CRemoteTimer class 
    member functions. 

-------------------------------------------------------------------------------
*/
// MACROS
#ifdef LOGGER
#undef LOGGER
#endif
#define LOGGER iTestRunner->iTestCombiner->iLog

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

     Class: CRemoteTimer

     Method: CRemoteTimer

     Description: Default constructor

     C++ default constructor can NOT contain any code, that
     might leave.
     
     Parameters: CTestRunner* aTestRunner: in: Backpointer to CTestRunner

     Return Values: None

     Errors/Exceptions: None

     Status: Proposal
    
-------------------------------------------------------------------------------
*/
CRemoteTimer::CRemoteTimer( CTestCombiner* aTestCombiner ): 
    CActive(  CActive::EPriorityLow ), // Executed with lowest priority 
    iState( ETimerIdle ),
    iTestCombiner( aTestCombiner )
    {
    CActiveScheduler::Add( this );
    
    }

/*
-------------------------------------------------------------------------------

     Class: CRemoteTimer

     Method: ConstructL

     Description: Symbian OS second phase constructor

     Symbian OS default constructor can leave.

     Parameters:    None

     Return Values: None

     Errors/Exceptions: None

     Status: Proposal
    
-------------------------------------------------------------------------------
*/
void CRemoteTimer::ConstructL()
    {
    
    iTimer.CreateLocal();
    
    }

/*
-------------------------------------------------------------------------------

     Class: CRemoteTimer

     Method: NewL

     Description: Two-phased constructor.
          
     Parameters: CTestCombiner* aTestCombiner: in: Backpointer to CTestCombiner

     Return Values: CRemoteTimer*: new object

     Errors/Exceptions: Leaves if new or ConstructL leaves

     Status: Proposal
    
-------------------------------------------------------------------------------
*/

CRemoteTimer* CRemoteTimer::NewL(  CTestCombiner* aTestCombiner )
    {
    
    CRemoteTimer* self = new (ELeave) CRemoteTimer( aTestCombiner );
     
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    
    }

/*
-------------------------------------------------------------------------------

     Class: CRemoteTimer

     Method: ~CRemoteTimer

     Description: Destructor
     
     Parameters:    None

     Return Values: None

     Errors/Exceptions: None

     Status: Proposal
    
-------------------------------------------------------------------------------
*/     

CRemoteTimer::~CRemoteTimer()
    {
    
    Cancel();
    
    iTimer.Close();
         
    }

/*
-------------------------------------------------------------------------------

     Class: CRemoteTimer

     Method: RunL

     Description: Derived from CActive, handles testcase execution.

     Parameters:    None.

     Return Values: None.

     Errors/Exceptions: Leaves on error situations.

     Status: Proposal
    
-------------------------------------------------------------------------------
*/
void CRemoteTimer::RunL()
    {
        
    if( iState != ETimerPending )
        {
        User::Panic( KRemoteTimer, KErrGeneral );
        }
    
    if( iStatus.Int() != KErrNone )
        {
        User::Panic( KRemoteTimer, KErrDied );
        }
    
    iState = ETimerIdle;
    
    iTestCombiner->RemoteTimeout();
    
    }
     
/*
-------------------------------------------------------------------------------

     Class: CRemoteTimer

     Method: DoCancel

     Description: Derived from CActive handles the Cancel

     Parameters:    None.

     Return Values: None.

     Errors/Exceptions: None.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
void CRemoteTimer::DoCancel()
    {
    iTimer.Cancel();
    iState = ETimerIdle;
      
    }

/*
-------------------------------------------------------------------------------

     Class: CRemoteTimer

     Method: SetTimerActive

     Description: Starts timer

     Parameters:   TTimeIntervalMicroSeconds32 anInterval: in: Timeout

     Return Values: None.

     Errors/Exceptions: None.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
void CRemoteTimer::SetTimerActive( TTimeIntervalMicroSeconds32 anInterval )
    {
    if( iState != ETimerIdle )
        {
        User::Panic( KRemoteTimer, KErrGeneral );
        }
        
    iState = ETimerPending;
    
    iTimer.After( iStatus, anInterval );
    SetActive();         
    
    }
    
/*
-------------------------------------------------------------------------------

    DESCRIPTION

    This module contains the implementation of CRemoteReceiver class 
    member functions. 

-------------------------------------------------------------------------------
*/
// MACROS
#ifdef LOGGER
#undef LOGGER
#endif
#define LOGGER iTestCombiner->iLog

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

     Class: CRemoteReceiver

     Method: CRemoteReceiver

     Description: Default constructor

     C++ default constructor can NOT contain any code, that
     might leave.
     
     Parameters: CTestCombiner* aTestCombiner: in: Backpointer to CTestCombiner

     Return Values: None

     Errors/Exceptions: None

     Status: Proposal
    
-------------------------------------------------------------------------------
*/
CRemoteReceiver::CRemoteReceiver( CTestCombiner* aTestCombiner ): 
    CActive(  CActive::EPriorityStandard ), 
    iState( EReceiverIdle ),
    iTestCombiner( aTestCombiner )
    {
    CActiveScheduler::Add( this );
    __TRACEFUNC();
    
    }

/*
-------------------------------------------------------------------------------

     Class: CRemoteReceiver

     Method: ConstructL

     Description: Symbian OS second phase constructor

     Symbian OS default constructor can leave.

     Parameters:    None

     Return Values: None

     Errors/Exceptions: None

     Status: Proposal
    
-------------------------------------------------------------------------------
*/
void CRemoteReceiver::ConstructL()
    {
    

    }

/*
-------------------------------------------------------------------------------

     Class: CRemoteReceiver

     Method: NewL

     Description: Two-phased constructor.
          
     Parameters: CTestCombiner* aTestCombiner: in: Backpointer to CTestCombiner

     Return Values: CRemoteReceiver*: new object

     Errors/Exceptions: Leaves if new or ConstructL leaves

     Status: Proposal
    
-------------------------------------------------------------------------------
*/

CRemoteReceiver* CRemoteReceiver::NewL( CTestCombiner* aTestCombiner )
    {
    CRemoteReceiver* self = new (ELeave) CRemoteReceiver( aTestCombiner );
     
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

/*
-------------------------------------------------------------------------------

     Class: CRemoteReceiver

     Method: ~CRemoteReceiver

     Description: Destructor
     
     Parameters:    None

     Return Values: None

     Errors/Exceptions: None

     Status: Proposal
    
-------------------------------------------------------------------------------
*/     

CRemoteReceiver::~CRemoteReceiver()
    {
    __TRACEFUNC();
    Cancel();

         
    }
    
/*
-------------------------------------------------------------------------------

     Class: CRemoteReceiver

     Method: StartL

     Description: Activates receiving.

     Parameters:    None.

     Return Values: None.

     Errors/Exceptions: None.

     Status: Proposal
    
-------------------------------------------------------------------------------
*/
void CRemoteReceiver::Start()
    {
    __TRACEFUNC();
    __ASSERT_ALWAYS( iState == EReceiverIdle, 
        User::Panic( KRemoteReceiver, KErrGeneral ) );
    iState = EReceiverPending;
    
    iTestCombiner->TestModuleIf().RemoteReceive( iRemoteMsg, iStatus );        
    SetActive();
                 
    }

/*
-------------------------------------------------------------------------------

     Class: CRemoteReceiver

     Method: RunL

     Description: Derived from CActive, handles testcase execution.

     Parameters:    None.

     Return Values: None.

     Errors/Exceptions: Leaves on error situations.

     Status: Proposal
    
-------------------------------------------------------------------------------
*/
void CRemoteReceiver::RunL()
    {
    __TRACEFUNC();
    
    __ASSERT_ALWAYS( iState == EReceiverPending, 
        User::Panic( KRemoteReceiver, KErrGeneral ) );
    iState = EReceiverIdle;
    
    iTestCombiner->ReceiveResponse( iRemoteMsg );
                 
    }
     
/*
-------------------------------------------------------------------------------

     Class: CRemoteReceiver

     Method: DoCancel

     Description: Derived from CActive handles the Cancel

     Parameters:    None.

     Return Values: None.

     Errors/Exceptions: None.

     Status: Proposal
    
-------------------------------------------------------------------------------
*/
void CRemoteReceiver::DoCancel()
    {
    __TRACEFUNC();
    
    iTestCombiner->TestModuleIf().RemoteReceiveCancel();
 
    }
    
// ================= OTHER EXPORTED FUNCTIONS =================================

/*
-------------------------------------------------------------------------------
    
     Function: LibEntryL

     Description: Polymorphic Dll Entry Point

     Parameters:    None.

     Return Values: CTestCombiner*: pointer to new CTestCombiner

     Errors/Exceptions: Leaves if NewL leaves.

     Status: Approved
     
-------------------------------------------------------------------------------
*/

EXPORT_C CTestCombiner* LibEntryL()
    {
    return CTestCombiner::NewL();
    }

// End of File
