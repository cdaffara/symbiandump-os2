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
* RTestEngineServer class member functions.
*
*/

// INCLUDE FILES
#include <e32svr.h>
#include "TestEngineClient.h"
#include "TestEngineCommon.h"

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

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

    Class: RTestEngineServer

    Method: Connect

    Description: Connect method creates new RTestEngineServer session.

    RTestEngineServer session is used to manage the test case execution.

    Parameters: None

    Return Values: TInt KErrNone : TestEngineServer created successfully

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TInt RTestEngineServer::Connect()
    {
    TInt ret = StartEngine();
    if ( ret == KErrNone)
        {
        ret = CreateSession( KTestEngineName, Version() );
        }
    return ret;

    }

/*
-------------------------------------------------------------------------------

    Class: RTestEngineServer

    Method: Version

    Description: Return client side version number from RTestEngineServer.

    Parameters: None

    Return Values: TVersion : Version number from RTestEngineServer

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TVersion RTestEngineServer::Version() const
    {
    return ( TVersion( KTestEngineMajorVersionNumber,
        KTestEngineMinorVersionNumber, KTestEngineBuildVersionNumber ) );

    }

/*
-------------------------------------------------------------------------------

    Class: RTestEngineServer

    Method: Close

    Description: Closes the RTestEngineServer session.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C void RTestEngineServer::Close()
    {
    // Check that server is connected
    if ( Handle() != 0 )
        {
        TIpcArgs args( TIpcArgs::ENothing, TIpcArgs::ENothing, TIpcArgs::ENothing );
        SendReceive( ETestEngineServerCloseSession, args );
        }
    RSessionBase::Close();

    }

/*
-------------------------------------------------------------------------------

    DESCRIPTION

    This module contains implementation of RTestEngine class member functions.

-------------------------------------------------------------------------------
*/

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

    Class: RTestEngine

    Method: Open

    Description: Open creates a subsession to TestEngine.

    Parameters: RTestEngineServer& aServer : Handle to Test Engine Server
                const TFileName& aIniFile : Ini file to be used for 
                 initialization of the Test Engine

    Return Values: TInt: Symbian OS error code

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TInt RTestEngine::Open( RTestEngineServer& aServer,
                                 const TFileName& aIniFile )
    {
    TIpcArgs args( &aIniFile, TIpcArgs::ENothing, TIpcArgs::ENothing );
    return CreateSubSession( aServer, ETestEngineCreateSubSession, args );
    }

/*
-------------------------------------------------------------------------------

    Class: RTestEngine

    Method: Close

    Description: Close the RTestEngine subsession.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C void RTestEngine::Close()
    {
    RSubSessionBase::CloseSubSession( ETestEngineCloseSubSession );

    }

/*
-------------------------------------------------------------------------------

    Class: RTestEngine

    Method: SetAttribute

    Description: Sets attributes to Test Framework

    Parameters: TAttribute aAttribute: in: Attribute type
                const TDesC& aValue: in: Attribute value

    Return Values: TInt: Symbian OS error code

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
EXPORT_C TInt RTestEngine::SetAttribute( TAttribute aAttribute,
                                          const TDesC& aValue )
    {
    TIpcArgs args( aAttribute, &aValue, TIpcArgs::ENothing );
    return SendReceive( ETestEngineSetAttribute, args );
    }

/*
-------------------------------------------------------------------------------

    Class: RTestEngine

    Method: AddTestModule

    Description: Load dynamically a new Test Module to the Test Framework.

    Parameters: const TName& aTestModule : Name of the Test Module to be added
                const TFileName& aIniFile : Initialization file of Test Module

    Return Values: TInt: Symbian OS error code

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TInt RTestEngine::AddTestModule( const TName& aTestModule,
                                          const TFileName& aIniFile )
    {
    TIpcArgs args( &aTestModule, &aIniFile, TIpcArgs::ENothing );
    return SendReceive( ETestEngineAddTestModule, args );
    }

/*
-------------------------------------------------------------------------------

    Class: RTestEngine

    Method: RemoveTestModule

    Description: Remove the existing Test Module from the Test Framework.

    Parameters: const TName& aTestModule : Name of the Test Module to be 
                                           removed

    Return Values: TInt: Symbian OS error code

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TInt RTestEngine::RemoveTestModule( const TName& aTestModule )
    {
    TIpcArgs args( &aTestModule, TIpcArgs::ENothing, TIpcArgs::ENothing );
    return SendReceive( ETestEngineRemoveTestModule, args );

    }

/*
-------------------------------------------------------------------------------

    Class: RTestEngine

    Method: AddConfigFile

    Description: Add new config file to Test Module.

    Parameters: const TName& aTestModule : Name of Test Module where the config
                 file will be added
                const TFileName& aConfigFile : Name of config file to be added

    Return Values: TInt: Symbian OS error code

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TInt RTestEngine::AddConfigFile( const TName& aTestModule,
                                          const TFileName& aConfigFile )
    {
    TIpcArgs args( &aTestModule, &aConfigFile, TIpcArgs::ENothing );
    return SendReceive( ETestEngineAddConfigFile, args );

    }

/*
-------------------------------------------------------------------------------

    Class: RTestEngine

    Method: RemoveConfigFile

    Description: Remove config file from Test Module.

    Parameters: const TName& aTestModule : Test Module name
                const TFileName& aConfigFile : Name of config file to be removed

    Return Values: TInt: Symbian OS error code

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
EXPORT_C TInt RTestEngine::RemoveConfigFile( const TName& aTestModule,
                                             const TFileName& aConfigFile )
    {
    TIpcArgs args( &aTestModule, &aConfigFile, TIpcArgs::ENothing );
    return SendReceive( ETestEngineRemoveConfigFile, args );
    }

/*
-------------------------------------------------------------------------------

    Class: RTestEngine

    Method: EnumerateTestCases

    Description: First phase of two-phased test case query operation.

    Test cases are inquired by two-phased operation calling first the 
    EnumerateTestCases method and then the GetTestCases. 

    Parameters: TInt& aCount : Test case count
                TRequestStatus& aStatus : Request status

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C void RTestEngine::EnumerateTestCases( TCaseCount& aCount,
                                              TRequestStatus& aStatus )
    {
    TIpcArgs args( &aCount, TIpcArgs::ENothing, TIpcArgs::ENothing );
    SendReceive( ETestEngineEnumerateTestCases, args, aStatus );
    }

/*
-------------------------------------------------------------------------------

    Class: RTestEngine

    Method: GetTestCases

    Description: Second phase of two-phased test case query operation.

    GetTestCases retrieves test cases from Test Modules to aTestCases that is
    a list consisting of several TTestInfo objects.

    Parameters: CArrayFixFlat<TTestInfo>& aTestCaseBuffer : Test case array

    Return Values: TInt: Symbian OS error code

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TInt RTestEngine::GetTestCases( 
                     CFixedFlatArray<TTestInfo>& aTestCaseBuffer )
    {
    // Construct and send message
    TIpcArgs args( &aTestCaseBuffer.Des(), TIpcArgs::ENothing, TIpcArgs::ENothing );
    return SendReceive( ETestEngineGetTestCases, args );

    }

/*
-------------------------------------------------------------------------------

    Class: RTestEngine

    Method: ErrorNotification

    Description: Obtains error notifications from Test Engine and Test
    Servers via Test Engine.

    Parameters: TTestProgressPckg& aError : Error package
                TRequestStatus& aStatus : Request status

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/ 
EXPORT_C void RTestEngine::ErrorNotification( TErrorNotificationPckg& aError,
                                              TRequestStatus& aStatus )
    {
    TIpcArgs args( &aError, TIpcArgs::ENothing, TIpcArgs::ENothing );
    SendReceive( ETestEngineErrorNotification, args, aStatus );

    }

/*
-------------------------------------------------------------------------------

    Class: RTestEngine

    Method: CancelAsyncRequest

    Description: Asynchronous requests can be canceled by this function.

    Parameters: TInt aReqToCancel : Request to be cancelled

    Return Values: TInt: Symbian OS error code

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TInt RTestEngine::CancelAsyncRequest( TInt aReqToCancel )
    {
    TIpcArgs args( aReqToCancel, TIpcArgs::ENothing, TIpcArgs::ENothing );
    return SendReceive( ETestEngineCancelAsyncRequest, args );

    }

/*
-------------------------------------------------------------------------------

    Class: RTestEngine

    Method: Event

    Description: Used to control STIF Test Framework event system.

    Parameters: TEventIfPckg& aEvent : in : Event information
                 TRequestStatus& aStatus : in :  Request status

    Return Values: TInt KErrNone : No errors occurred

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C void RTestEngine::Event( TEventIfPckg& aEvent, 
                                  TRequestStatus& aStatus )
    {
    TIpcArgs args( &aEvent, TIpcArgs::ENothing, TIpcArgs::ENothing );
    SendReceive( ETestEngineEvent, args, aStatus );

    }

/*
-------------------------------------------------------------------------------

    Class: RTestEngine

    Method: OpenLoggerSession

    Description: Open created subsession to TestEngine.

    Parameters: RTestEngineServer& aServer : Handle to Test Engine Server
                TLoggerSettings& aLoggerSettings: in: Logger's overwrite struct

    Return Values: TInt KErrNone: TestEngine opened and initialized
                                  succesfully

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TInt RTestEngine::OpenLoggerSession( RTestEngineServer& aServer, 
                                            TLoggerSettings& aLoggerSettings )
    {
    // Package
    TPckg<TLoggerSettings> loggerSettingsPckg( aLoggerSettings );

    TIpcArgs args( &loggerSettingsPckg, TIpcArgs::ENothing, TIpcArgs::ENothing );
    // Goes to CTestEngine's DispatchMessageL() method
    CreateSubSession( aServer, ETestEngineLoggerSettings, args );
    RSubSessionBase::CloseSubSession( ETestEngineCloseLoggerSettings );

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: RTestEngine

    Method: AddTestCaseResultToTestReport

    Description: Sends info about executed test cases to test engine, which
                 will forward it to test report.
                 This is used when after reboot UIStore reads info about 
                 executed test cases before reboot.

    Parameters: TTestIngo& aTestInfo : test info structure
                TFullTestResult& aTestResult: test result structure
                TInt aError: error

    Return Values: TInt error code returned by test engine

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TInt RTestEngine::AddTestCaseResultToTestReport(const TTestInfo& aTestInfo,
                                                         const TFullTestResult& aTestResult,
                                                         const TInt aError)
    {
    // Create packages
    TTestInfoPckg testInfoPckg(aTestInfo);
    TFullTestResultPckg fullTestResultPckg(aTestResult);

    // Create argument list
    TIpcArgs args(&testInfoPckg, &fullTestResultPckg, aError);

    // Send it to test engine
    TInt res = SendReceive(ETestEngineReportTestCase, args);

    return res;
    }

/*
-------------------------------------------------------------------------------

    DESCRIPTION

    This module contains implementation of RTestCase class member functions.

-------------------------------------------------------------------------------
*/

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

    Class: RTestCase

    Method: Open

    Description: Open creates a subsession to TestCase.

    Parameters: RTestEngineServer& aServer : Handle to Test Engine Server
                const TTestInfoPckg& aTestCaseInfo : TTestInfoPckg : Test info
                                                                     package

    Return Values: TInt: Symbian OS error code

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TInt RTestCase::Open( RTestEngineServer& aServer,
                              const TTestInfoPckg& aTestCaseInfo )
    {
    TIpcArgs args( &aTestCaseInfo, TIpcArgs::ENothing, TIpcArgs::ENothing );
    return CreateSubSession( aServer, ETestCaseCreateSubSession, args );
    }

/*
-------------------------------------------------------------------------------

    Class: RTestCase

    Method: Close

    Description: Close the RTestCase subsession.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C void RTestCase::Close()
    {
    RSubSessionBase::CloseSubSession( ETestCaseCloseSubSession );

    }

/*
-------------------------------------------------------------------------------

    Class: RTestCase

    Method: RunTestCase

    Description: Run a test case asynchronously.

    Parameters: TFullTestResultPckg& aTestResult : TFullTestResult :
                 Test Result package
                TRequestStatus& aStatus : Request status

    Return Values: TInt KErrNone : Test case could be run
                        Other error code : Reason the test case couldn't be run

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C void RTestCase::RunTestCase( TFullTestResultPckg& aTestResult,
                                     TRequestStatus& aStatus )
    {
    TIpcArgs args( &aTestResult, TIpcArgs::ENothing, TIpcArgs::ENothing );
    SendReceive( ETestCaseRunTestCase, args, aStatus );
    }

/*
-------------------------------------------------------------------------------

    Class: RTestCase

    Method: RunTestCase

    Description: Run a test case asynchronously.

    Parameters: TFullTestResultPckg& aTestResult : TFullTestResult :
                 Test Result package
                const TDesC& aTestCaseArgs: Test case arguments
                TRequestStatus& aStatus : Request status

    Return Values: TInt KErrNone : Test case could be run
                        Other error code : Reason the test case couldn't be run

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C void RTestCase::RunTestCase( TFullTestResultPckg& aTestResult,
                                      const TDesC& aTestCaseArgs,
                                      TRequestStatus& aStatus )
    {
    TIpcArgs args( &aTestResult, &aTestCaseArgs, TIpcArgs::ENothing );
    SendReceive( ETestCaseRunTestCase, args, aStatus );
    }

/*
-------------------------------------------------------------------------------

    Class: RTestCase

    Method: Pause

    Description: Pause suspends the execution of the test case.

    Parameters: None

    Return Values: TInt: Symbian OS error code

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TInt RTestCase::Pause()
    {
    TIpcArgs args( TIpcArgs::ENothing, TIpcArgs::ENothing, TIpcArgs::ENothing );
    return SendReceive( ETestCasePause, args );
    }

/*
-------------------------------------------------------------------------------

    Class: RTestCase

    Method: Resume

    Description: Resume the test case suspended previously.

    Parameters: None

    Return Values: TInt: Symbian OS error code

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TInt RTestCase::Resume()
    {
    TIpcArgs args( TIpcArgs::ENothing, TIpcArgs::ENothing, TIpcArgs::ENothing );
    return SendReceive( ETestCaseResume, args );
    }

/*
-------------------------------------------------------------------------------

    Class: RTestCase

    Method: NotifyProgress

    Description: Request different progress information from the test case.

    Parameters: TTestProgressPckg& aProgress : TTestProgress : Test Progress
                                                               package
                TRequestStatus& aStatus : Request status

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C void RTestCase::NotifyProgress( TTestProgressPckg& aProgress,
                                         TRequestStatus& aStatus )
    {
    TIpcArgs args( &aProgress, TIpcArgs::ENothing, TIpcArgs::ENothing );
    SendReceive( ETestCaseNotifyProgress, args, aStatus );
    }
    
/*
-------------------------------------------------------------------------------

    Class: RTestCase

    Method: NotifyRemoteType

    Description: Request remote commands information from the test case.

    Parameters: TRemoteCommandPckg& aType: in: Request type
                TRequestStatus& aStatus : Request status

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
EXPORT_C void RTestCase::NotifyRemoteType( TStifCommandPckg& aType,
                                           TPckg<TInt>& aMsgSize,
                                           TRequestStatus& aStatus )
    {
    TIpcArgs args( &aType, &aMsgSize, TIpcArgs::ENothing );
    SendReceive( ETestCaseNotifyRemoteType, args, aStatus );
    }

/*
-------------------------------------------------------------------------------

    Class: RTestCase

    Method: NotifyRemoteMsg

    Description: Request remote commands information from the test case.

    Parameters: TDesC& aMessage: in: Message buffer
                TRemoteCmdType aType: in: Message direction

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
EXPORT_C TInt RTestCase::NotifyRemoteMsg( TDes8& aMessage,
                                          TStifCommand aType )
    {
    TIpcArgs args( &aMessage, aType, aMessage.Length() );
    return SendReceive( ETestCaseNotifyRemoteMsg, args );
    }

/*
-------------------------------------------------------------------------------

    Class: RTestCase

    Method: CancelAsyncRequest

    Description: Asynchronous requests can be canceled by this function.

    Parameters: TInt aReqToCancel : Request to be cancelled

    Return Values: TInt: Symbian OS error code

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TInt RTestCase::CancelAsyncRequest( TInt aReqToCancel )
    {
    TIpcArgs args( aReqToCancel, TIpcArgs::ENothing, TIpcArgs::ENothing );
    return SendReceive( ETestCaseCancelAsyncRequest, args );
    }

// ================= OTHER EXPORTED FUNCTIONS =================================

// None

// End of File
