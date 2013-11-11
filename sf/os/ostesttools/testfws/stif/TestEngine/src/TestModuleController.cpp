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
* CTestModuleController class member functions.
*
*/

// INCLUDE FILES
#include <e32svr.h>
#include <stifinternal/TestServerClient.h>
#include <StifLogger.h>
#include "TestEngine.h"
#include "TestModuleController.h"
#include "Logging.h"

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

    Class: CTestModuleController

    Method: CTestModuleController

    Description: Default constructor

    C++ default constructor can NOT contain any code, that
    might leave.

    Parameters: CTestEngine* aEngine: in: Pointer to CTestEngine

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CTestModuleController::CTestModuleController( CTestEngine* aEngine ) :
    CActive( CActive::EPriorityStandard ),
    iEngine( aEngine ),
    iState( ETestModuleIdle )
    {
    CActiveScheduler::Add( this );
    iTestScripterController = NULL;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleController

    Method: ConstructL

    Description: Symbian OS second phase constructor

    Symbian OS default constructor can leave.

    Parameters: const TName& aName: in: Test module name

    Return Values: None

    Errors/Exceptions: Leaves if memory allocation for iConfigFiles fails
                       Leaves if memory allocation for iTestCaseArray fails

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestModuleController::ConstructL( 
                            const TName& aName,
                            TBool aAfterReboot,
                            CTestScripterController* aTestScripterController )
    {
    iTestScripterController = aTestScripterController;
    iTestScripterIndicator = 0;
    
    iAfterReboot = aAfterReboot;

    __TRACE ( KInit, ( _L( "Creating CTestModuleController [%S]" ), &aName ) );

    iTestCaseArray = RPointerArray<TTestCaseArray>();

    // Check aName length
    if( aName.Length() <= 0 )
        {
        iEngine->LeaveWithNotifyL( KErrArgument );
        }

    // Remove optional '.DLL' from file name
    TName name = aName;
    name.LowerCase();
    TParse parse;
    parse.Set( name, NULL, NULL );

    if ( parse.Ext() == _L(".dll") )
        {
        const TInt len = parse.Ext().Length();
        name.Delete ( name.Length()-len, len );
        }

    iName = name.AllocL();

    // HBufC to TPtrC
    TPtrC atsName;

    TFileName newNameBuffer;
    TInt check = GenerateModuleName( iName->Des(), newNameBuffer );
    if( check == KErrNone )
        {
        // Load the module(TestScripter)
        atsName.Set( newNameBuffer );
        }
    else
        {
        // Load the module(Others)
        atsName.Set( iName->Des() );
        }

    // Moved the implementation to a new function, ConstructASTLoggerL, due to
    // CW 3.0 compiler error with multiple TRAPDs
    TRAPD(err, ConstructASTLoggerL( atsName, aAfterReboot));

    if( err != KErrNone )
        {
         __TRACE ( KError, ( _L( "CTestModuleController[%S]::ConstructL: Creation of ATS logger failed" ), iName ) );
        iEngine->LeaveWithNotifyL( err, _L("Creation of ATS logger failed") );
        }

    }
/*
-------------------------------------------------------------------------------

    Class: CTestModuleController

    Method: ConstructASTLoggerL

    Description: Construct ATS logger

    Parameters: TDesC& atsName, TBool& aAfterReboot

    Return Values: TInt

    Errors/Exceptions: Leaves if error got from ATS logger NewL

    Status: Proposal

-------------------------------------------------------------------------------
*/

TInt CTestModuleController::ConstructASTLoggerL( TDesC& atsName, TBool& aAfterReboot )
    {
    TRAPD( err, iAtsLogger = CAtsLogger::NewL( atsName, aAfterReboot ); );
    if( err == KErrNotFound && aAfterReboot )
        {
        // If file does not exist, create new 
        aAfterReboot = EFalse;
        iAtsLogger = CAtsLogger::NewL( atsName, aAfterReboot );
        }
    User::LeaveIfError( err );
    iAtsLogger->SetFullReporting( ETrue );
    if( aAfterReboot )
        {
        iAtsLogger->ContinueAfterRebootL();
        }
    else
        {
        // Begin reporting
        iAtsLogger->BeginTestReportL();
          
        // Begin test set
        iAtsLogger->BeginTestSetL();
        }
    return KErrNone;
    }
/*
-------------------------------------------------------------------------------

    Class: CTestModuleController

    Method: InitL

    Description: Initialize test module.

    Parameters: TFileName& aIniFile: in: Initialization file of Test Module
                const TDesC& aConfigFile: in: Test case(config) file name(Used 
                in TestScripter case).

    Return Values: None

    Errors/Exceptions: Leaves if error got from Test Server

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CTestModuleController::InitL( TFileName& aIniFile, 
                                    const TDesC& aConfigFile )
    {
    __TRACE( KInit, (_L( "Initialising test module [%S] with initialization file [%S]" ),
        iName, &aIniFile ) );

    // HBufC to TPtrC
    TPtrC name( iName->Des() );

    // Connect to server
    TInt r = iServer.Connect( name, aConfigFile );

    if ( r != KErrNone )
        {
        if ( r == KErrBadName )
            {
            __TRACE( KError, ( CStifLogger::ERed, _L( "Test Module name or Configuration File name is too long: [%S]" ), &name) );
            LeaveWithNotifyL( r,  _L( "Test Module name or Configuration File name is too long." ) );
            }
        __TRACE( KError, ( CStifLogger::ERed, _L( "Can't connect to test module [%S], IniFile [%S]" ), &name, &aIniFile ) );
        LeaveWithNotifyL( r,  _L( "Can't connect to test module" ) );
        }

    // Open session 
    r = iModule.Open( iServer, aIniFile );
    if ( r != KErrNone )
        {
        __TRACE( KError, ( CStifLogger::ERed, _L( "Can't open session to test module [%S], IniFile=[%S]" ), &name, &aIniFile ) );
        LeaveWithNotifyL( r, _L( "Can't open session to test module" ) );
        }

    iErrorPrinter = CErrorPrinter::NewL( iEngine );
    iErrorPrinter->StartL( iModule );

    iServerStateHandler = CServerStateHandler::NewL( iEngine, this );
    iServerStateHandler->StartL( iServer );
    __TRACE( KInit, (_L( "Initialising test module [%S] with initialization file [%S] done" ),
        iName, &aIniFile ) );

    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleController

    Method: NewL

    Description: Two-phased constructor.

    Parameters: CTestEngine* aEngine: in: CTestEngine object.
                const TName& aName: in: module name.
                TBool aAfterReboot: in: reboot indicator.
                TBool aCreateTestScripterCont: in: Indications to
                TestModule or TestScripter creation
                CTestScripterController* aTestScripterController: in:
                CTestEngine object.


    Return Values: CTestModuleController* : pointer to created object

    Errors/Exceptions: Leaves if called ConstructL method leaves

    Status: Proposal

-------------------------------------------------------------------------------
*/
CTestModuleController* CTestModuleController::NewL( 
                            CTestEngine* aEngine,
                            const TName& aName,
                            TBool aAfterReboot,
                            TBool aCreateTestScripterCont,
                            CTestScripterController* aTestScripterController )
    {
    // Create CTestScripterController
    if( aCreateTestScripterCont )
        {
        CTestScripterController* testScripterController = NULL;
        testScripterController = CTestScripterController::NewL(
            aEngine, aName, aAfterReboot );
        return testScripterController;  
        }


    CTestModuleController* self =
        new ( ELeave ) CTestModuleController( aEngine );
    CleanupStack::PushL( self );
    self->ConstructL( aName, aAfterReboot, aTestScripterController );
    CleanupStack::Pop( self );
    return self;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleController

    Method: ~CTestModuleController

    Description: Destructor

    Deallocate all allocated resources

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
CTestModuleController::~CTestModuleController()
    {

    // If CTestScripterController is created there is deleted base class(
    // CTestModuleController) also and this has no iName.
    if( iName )
        {
        __TRACE(KVerbose, (_L( "Deleting CTestModuleController [%S]" ), iName ) );
        }

    Cancel();

    // If CTestScripterController is created there is deleted base class(
    // CTestModuleController) also and this has no iAtsLogger.
    if( iAtsLogger )
        {
        // End test set
        TRAPD( err, iAtsLogger->EndTestSetL() );
        // End AtsLogger reporting, has to be trapped in destructor
        TRAPD( err2, iAtsLogger->EndTestReportL() );
        if( ( err != KErrNone ) || ( err2 != KErrNone ) )
            {
            // Print error if error got from trap
            __TRACE( KError, ( 
                _L( "Destructor of CTestModuleController [%S]: XML log closing failed" ),
                iName ) );
            }
        }

    // Delete Atslogger
    delete iAtsLogger;

    // Stop error printer
    delete iErrorPrinter;
    iErrorPrinter = NULL;
    // Delete server state handler
    delete iServerStateHandler;
    iServerStateHandler = NULL;

    // Close RTestModule session
    iModule.Close();
    // Close RTestServer session
    iServer.Close();

    // Release the test case array
    iTestCaseArray.Close();

    // Release the config file array
    iConfigFiles.ResetAndDestroy();
    iConfigFiles.Close();

    // Release the config file array
    iFailedEnumerateConfig.ResetAndDestroy();
    iFailedEnumerateConfig.Close();
    
    // Release the children array
    iChildrenControllers.ResetAndDestroy();
    iChildrenControllers.Close();

    delete iName;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleController

    Method: AddConfigFileL

    Description: Add config file

    Parameters: TFileName& aConfigFile: in: Config file for Test Module

    Return Values: None

    Errors/Exceptions: Leaves if AppendL leaves

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestModuleController::AddConfigFileL( TFileName& aConfigFile )
    {
    __TRACE( KInit,
        ( _L( "CTestModuleController::AddConfigFileL [%S] aConfigFile=[%S]" ),
        iName, &aConfigFile ) );
    // Check that this config file does not already exists
    for ( TInt i = 0; i < iConfigFiles.Count(); i++ )
        {
        // HBufC to TPtrC
        TPtrC name( iConfigFiles[i]->Des() );
        if ( KErrNone == aConfigFile.CompareF( name ) )
            {
            LeaveWithNotifyL( KErrAlreadyExists,  
                _L( "Adding config file failed: Config file already exists" ) );
            }
        }
    HBufC* configFile = aConfigFile.AllocLC();

    User::LeaveIfError( iConfigFiles.Append( configFile ) );
    CleanupStack::Pop( configFile );

    }
    
/*
-------------------------------------------------------------------------------

    Class: CTestModuleController

    Method: RemoveConfigFileL

    Description: Remove config file

    Parameters: TFileName& aConfigFile: in: Config file name

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestModuleController::RemoveConfigFileL( TFileName& aConfigFile )
    {
    __TRACE( KInit, ( 
        _L( "CTestModuleController::RemoveConfigFileL [%S] aConfigFile=[%S]" ),
        iName, &aConfigFile ) );
    // Find config file
    for ( TInt i = 0; i < iConfigFiles.Count(); i++ )
        {
        // HBufC to TPtrC
        TPtrC name( iConfigFiles[i]->Des() );
        if ( KErrNone == aConfigFile.CompareF( name ) )
            {
            HBufC* configFile = iConfigFiles[i];
            iConfigFiles.Remove(i);
            delete configFile;
            // Cancel enumerate and free test cases
            Cancel();
            FreeTestCases();
            return;
            }
        }
        
    LeaveWithNotifyL( KErrNotFound, 
        _L( "Removing config file failed: Config file not found" ) );
            
    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleController

    Method: StartEnumerateL

    Description: Start test case enumeration

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestModuleController::StartEnumerateL()
    {
    __TRACE( KVerbose, ( 
        _L( "CTestModuleController::StartEnumerateL [%S]" ), iName ) ); 
    iTestCaseCount = 0;
    iEnumerateCount = 0;
    iFailedEnumerateCount = 0;
    iFailedEnumerateConfig.Reset();
    iEnumerateComplete = EFalse;

    iState = ETestModuleEnumerateCases;

    if ( iConfigFiles.Count() > 0 )
        {
        // HBufC to TPtrC
        iEnumConfigFile.Set( iConfigFiles[0]->Des() );
        }

    __TRACE( KInit, ( 
        _L( "Getting testcases from module [%S], test case file [%S]" ),
        iName, &iEnumConfigFile ) );

    SetActive();
    iModule.EnumerateTestCases( iEnumConfigFile, iEnumResultPackage, iStatus );

    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleController

    Method: RunL

    Description: RunL handles completed requests.

    Parameters: None

    Return Values: None

    Errors/Exceptions: Leaves if iStatus is not KErrNone
                       Leaves if some of called leaving methods leaves

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestModuleController::RunL()
    {

    // If CTestScripterController is used then move execution to the
    // CTestScripterController size
    if( iTestScripterController )
        {
        iTestScripterController->RunLEmulator( this );
        return;
        }

    __TRACE( KVerbose, ( _L( "CTestModuleController::RunL [%S] [iStatus = %d]"),
        iName, iStatus.Int() ) );

    // Note:
    // If test case not found there may be existing cases from previous
    // enumerations, so those cases are valid. e.g. case: "add test case file",
    // "add test case file that not exist" and "get test cases".

    TInt ret( KErrNone );
    // Check that request was successful
    if( iStatus.Int() == KErrNone )
        {
        // Get enumerated test cases and append them to array
        CFixedFlatArray<TTestCaseInfo>* testCases = 
            CFixedFlatArray<TTestCaseInfo>::NewL( iEnumResultPackage() );
        CleanupStack::PushL( testCases );
        ret = iModule.GetTestCases( *testCases );
        __TRACE( KInit, ( _L( "RunL()'s GetTestCases method returns: %d" ), ret ) );

        iTestCaseCount += testCases->Count();

        if ( testCases->Count() == 0 )
            {

            if (iConfigFiles.Count() > 0)
                {
                __TRACE( KInit, ( CStifLogger::EBold, 
                    _L( "Module [%S], test case file [%S] returned 0 cases" ),
                    iName,  iConfigFiles[iEnumerateCount] ) );
                }
            else
                {
                __TRACE( KInit, ( CStifLogger::EBold, 
                    _L( "Module [%S] without test case file, returned 0 cases" ),
                    iName ) );
                }
            iEngine->ErrorPrint( 1, _L("Someone returned 0 test cases. Check testengine log"));
            iAtsLogger->CommentL( _L("Test module returned 0 test cases") );   
            }

        // Store test cases for later use
        User::LeaveIfError( iTestCaseArray.Append( testCases ) );
        CleanupStack::Pop( testCases );
        
        }
    else
        {
        // Calculate failed enumeration count
        iFailedEnumerateCount++;
        // Add failed config(test case) file to array for later removing
        if( iConfigFiles.Count() != NULL )
            {
            __TRACE( KError, (
                CStifLogger::ERed, 
                _L( "Test case[%S] enumeration fails with error: %d" ),
                iConfigFiles[iEnumerateCount], iStatus.Int() ) );
            // Append
            iFailedEnumerateConfig.Append( iConfigFiles[iEnumerateCount] );
            }
        }

    iEnumerateCount++;

    if ( iEnumerateCount < iConfigFiles.Count() )
        {
        // Continue enumeration
        __TRACE( KInit, ( 
            _L( "Getting testcases from module [%S], test case file [%S]" ),
            iName, iConfigFiles[iEnumerateCount] ) );
        SetActive();
        iModule.EnumerateTestCases( *iConfigFiles[iEnumerateCount],
                                    iEnumResultPackage, iStatus );
        }
    else if( iTestCaseCount == 0 )
        {
        // Total count of succesfully enumerations
        iEnumerateCount -= iFailedEnumerateCount;
        User::Leave( KErrNotFound );
        }
    else
        {
        // Total count of succesfully enumerations
        iEnumerateCount -= iFailedEnumerateCount;
        // Remove faulty config (test case) file(s)
        for( TInt a= 0; a < iFailedEnumerateConfig.Count(); a++ )
            {
            TInt index( 0 );
            // Find removed config(test case) file
            index = iConfigFiles.Find( iFailedEnumerateConfig[a] );
            if( index != KErrNotFound )
                {
                __TRACE( KInit, ( _L( "Removing test case file[%S]" ),
                    iConfigFiles[index] ) );
                // Remove
                iConfigFiles.Remove( index );
                }
            }

        // All test cases enumerated
        iState = ETestModuleEnumerateCasesCompleted;
        iEnumerateComplete = ETrue;

        iEngine->EnumerationCompleted( iTestCaseCount );
        }

    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleController

    Method: TestCasesL

    Description: Return Test Cases

    Parameters: None

    Return Values: CFixedFlatArray<TTestInfo>* :Pointer to created array

    Errors/Exceptions: Leaves if NewL leaves

    Status: Approved

-------------------------------------------------------------------------------
*/
CFixedFlatArray<TTestInfo>* CTestModuleController::TestCasesL()
    {
    CFixedFlatArray<TTestInfo>* testCases = 
        CFixedFlatArray<TTestInfo>::NewL( iTestCaseCount );

    CleanupStack::PushL( testCases );

    // Loop through all test cases from all config files
    TInt totalCount = 0;
    // HBufC to TPtrC
    TPtrC name( iName->Des() );
    for ( TInt i = 0; i < iEnumerateCount; i++ )
        {
        TTestInfo tmpInfo;
        tmpInfo.iModuleName = name;
        if ( iConfigFiles.Count() > 0 )
            {
            tmpInfo.iConfig = *iConfigFiles[i];
            }

        // Get test cases from iTestCaseArray at [i]
        //if( (iTestCaseArray)[i] )
		if ( iTestCaseArray.Count() > 0) 
            {
            CFixedFlatArray<TTestCaseInfo>* tempTestCases = (iTestCaseArray)[i];
            for ( TInt j = 0; j < tempTestCases->Count(); j++ )
                {
                tmpInfo.iTestCaseInfo = (*tempTestCases)[j];
                // Set TestCaseInfo to testCases array
                testCases->Set( totalCount, tmpInfo );
                totalCount++;
                }
            }
        }

    CleanupStack::Pop( testCases );

    return testCases;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleController

    Method: FreeTestCases

    Description: Free memory used for test cases

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestModuleController::FreeTestCases()
    {
    // Reset and destroy test cases
    iTestCaseArray.ResetAndDestroy();
    iTestCaseArray.Close();

    iTestCaseCount = 0;
    iEnumerateComplete = EFalse;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleController

    Method: DoCancel

    Description: Cancel active request

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestModuleController::DoCancel()
    {

    // If CTestScripterController is used then move execution to the
    // CTestScripterController size
    if( iTestScripterController )
        {
        iTestScripterController->DoCancelEmulator( this );
        return;
        }

    __TRACE( KVerbose, ( _L( "CTestModuleController::DoCancel [%S]" ), iName ) );

    switch ( iState )
        {
        case ETestModuleEnumerateCases:
            iModule.CancelAsyncRequest( ETestModuleEnumerateTestCases );
            iEnumerateComplete = ETrue;
            // Enumeration canceled, complete with KErrCancel
            iEngine->EnumerationCompleted( 0, KErrCancel );

            // Free allocated test cases because EnumerateTestCases was
            // canceled
            FreeTestCases();
            break;
        case ETestModuleIdle:
        case ETestModuleEnumerateCasesCompleted:
        default:
            // DoCancel called in wrong state => Panic
            _LIT( KTestModuleController, "CTestModuleController" );
            User::Panic( KTestModuleController, EDoCancelDisorder );
            break;
        }

    iAtsLogger->ErrorL( _L("Test case enumeration cancelled") );   

    iState = ETestModuleIdle;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleController

    Method: RunError

    Description: Handle errors.

    Parameters: TInt aError: in: Symbian OS error: Error code

    Return Values: TInt KErrNone: Always returned KErrNone

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
TInt CTestModuleController::RunError( TInt aError )
    {

    // If CTestScripterController is used then move execution to the
    // CTestScripterController size
    if( iTestScripterController )
        {
        return iTestScripterController->RunErrorEmulator( aError, this );
        }

    __TRACE( KError, ( CStifLogger::ERed, 
        _L( "CTestModuleController::RunError [%S] aError=[%d]" ),
        iName, aError ) );
    if( aError == KErrNoMemory )
        {
        __TRACE( KError, ( CStifLogger::ERed, 
            _L( "No memory available. Test case file's size might be too big." ) ) );
        }

    iEnumerateComplete = ETrue;

    iAtsLogger->ErrorL( _L("Test module did not return any test cases") );
    
    _LIT( KErrorText, " did not return any test cases [error: ");
    if( KErrorText().Length() + iName->Length() + 1 < KMaxName )
        {
        // Enumeration failed, print warning and complete with KErrNone
        TName error( iName->Des() );
        error.Append( KErrorText );  
        error.AppendNum( aError );  
        error.Append( _L("]") );  
        iEngine->ErrorPrint ( 0, error );
    
        iEngine->EnumerationCompleted( 0, KErrNone );
        }
    else 
        {
        // Cannot only print warning, complete with error
        iEngine->EnumerationCompleted( 0, aError );
        }        

    // Free allocated test cases because EnumerateTestCases failed
    FreeTestCases();
    iEnumerateComplete = ETrue;

    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleController

    Method: Server

    Description: Return handle to Test Server

    Parameters: TTestInfo& aTestInfo: in: Test info for this test case

    Return Values: RTestServer& : Reference to RTestServer

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
RTestServer& CTestModuleController::Server( TTestInfo& /*aTestInfo*/ )
    {
    return iServer;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleController

    Method: GetFreeOrCreateModuleControllerL

    Description: Return pointer to module controller.
                 Find module controller which does not run test case
                 and if this is not possible then create new one.
                 (Only for test scripter).

    Parameters: TTestInfo& aTestInfo: in: Test info for this test case
                TBool aUITestingSupport: in: Is UI testing mode enabled

    Return Values: CTestModuleController* : pointer to module controller

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CTestModuleController* CTestModuleController::GetFreeOrCreateModuleControllerL(TTestInfo& /*aTestInfo*/, TBool /*aUItestingSupport*/)
    {
    return NULL;
    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleController

    Method: ModuleName

    Description: Return the name of Test Module

    Parameters: const TDesC& aModuleName: in Modulename

    Return Values: const TDesC : Name of Test Module

    Errors/Exceptions: None

    Status: proposal

-------------------------------------------------------------------------------
*/
const TDesC& CTestModuleController::ModuleName( const TDesC& /*aModuleName*/ )
    {
    return *iName;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleController

    Method: EnumerationComplete

    Description: Is enumeration of test cases complete.

    Parameters: None
    
    Return Values: TBool ETrue: Enumeration of test cases is complete
                         EFalse: Enumeration is not complete

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TBool CTestModuleController::EnumerationComplete()
    {
    return iEnumerateComplete;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleController

    Method: LeaveWithNotifyL

    Description: Print out info and leave.

    Parameters: TInt aCode: in: Error code 
                const TDesC& aText: in: Test info
    
    Return Values: None
    
    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CTestModuleController::LeaveWithNotifyL( TInt aCode, const TDesC& aText )
    {

    __TRACE( KError, ( CStifLogger::ERed, aText ) );
    if( iAtsLogger )
        {
        iAtsLogger->ErrorL( aText );
        }
    iEngine->LeaveWithNotifyL( aCode );

    }
    
/*
-------------------------------------------------------------------------------

    Class: CTestModuleController

    Method: CaseCreated

    Description: Increases the value of iTestCaseCounter

    Parameters: None
    
    Return Values: None
    
    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/  
void CTestModuleController::CaseCreated()
    {
    // Number of ongoing testcases
    iTestCaseCounter++;
    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleController

    Method: CaseFinished

    Description: Decreases the value of iTestCaseCounter and deletes this pointer
                 only in test module crash situations (KErrServerTerminated -15)

    Parameters: None:
    
    Return Values: None
    
    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/  
void CTestModuleController::CaseFinished()
    {
    // Number of ongoing testcases
    iTestCaseCounter--;
    
    // iTestModuleCrashDetected tells is TestModuleController cloned or not
    // TestModuleController is cloned only when testmodule is crashed with KErrServerTerminated -15
    if (iTestCaseCounter == 0 && iTestModuleCrashDetected)
       {
       // Delete this in case where it has been replaced with it clone
       // and no one have pointer to this. This happens only when test module
       // crashes    
  	   delete this;
  	   // Mem::Fill(this, sizeof(CTestModuleController), 0xa1a1a1a1);
       }
       
    }
    
/*
-------------------------------------------------------------------------------

    Class: CTestModuleController

    Method: CloneL

    Description: 

    Parameters: 
    
    Return Values: 
    
    Errors/Exceptions: 

    Status: 

-------------------------------------------------------------------------------
*/    
CTestModuleController* CTestModuleController::CloneL( 
                            CTestModuleController* aTestModuleController,
                            TBool aAfterReset,
                            CTestScripterController* aTestScripterController )
    {
    __TRACE( KInit, ( _L( "Cloning CTestModuleController" ) ) );
   
    
    TName crashModuleName;
    crashModuleName = aTestModuleController->ModuleName( crashModuleName );
    
    CTestModuleController* clone = CTestModuleController::NewL( iEngine, crashModuleName, aAfterReset, EFalse, aTestScripterController );
    
    clone->InitL( aTestModuleController->iInifile, KNullDesC );

	// Give ATS logger to clone
	clone->iAtsLogger = iAtsLogger;
	iAtsLogger = NULL; 
	
    for ( TInt i = 0; i < aTestModuleController->iConfigFiles.Count(); i++ )
	    {
		TPtrC configFile = aTestModuleController->iConfigFiles[i]->Des();
		TFileName config = configFile;
   	    clone->AddConfigFileL( config );
   	    }   
	
	iTestModuleCrashDetected = ETrue;       
	
    __TRACE( KVerbose, ( _L( "Cloning of CTestModuleController finished " ) ) );
    return clone; 
    }    

/*
-------------------------------------------------------------------------------

    Class: CTestModuleController

    Method: DeleteModuleController

    Description: In that class this method does nothing. It is implemented
                 in CTestScripterController.

    Parameters: CTestModuleController* aRealModuleController: not used
    
    Return Values: None
    
    Errors/Exceptions: None

    Status: 

-------------------------------------------------------------------------------
*/    
void CTestModuleController::DeleteModuleController(CTestModuleController* /*aRealModuleController*/)
    {
    } 

/*
-------------------------------------------------------------------------------

    Class: CTestModuleController

    Method: RemoveModuleController

    Description: In that class this method does nothing. It is implemented
                 in CTestScripterController.

    Parameters: CTestModuleController* aRealModuleController: not used
    
    Return Values: None
    
    Errors/Exceptions: None

    Status: 

-------------------------------------------------------------------------------
*/    
void CTestModuleController::RemoveModuleController(CTestModuleController* /*aRealModuleController*/)
    {
    } 

/*
-------------------------------------------------------------------------------

    Class: CTestModuleController

    Method: EnumerateSynchronously

    Description: Enumerates test module controller. Used only when new
                 test module controller is created during test case
                 execution.

    Parameters: None
    
    Return Values: None
    
    Errors/Exceptions: None

    Status: 

-------------------------------------------------------------------------------
*/    
void CTestModuleController::EnumerateSynchronously(void)
    {
    TInt i;
    TInt cfgfiles = iConfigFiles.Count();
    __TRACE(KInit, (_L("Module controller will enumerate synchronously [%d] config files"), cfgfiles));
    
    for(i = 0; i < cfgfiles; i++)
        {
        TRequestStatus status; 
        iModule.EnumerateTestCases(*iConfigFiles[i],
                                   iEnumResultPackage, 
                                   status);
        User::WaitForRequest(status);
        }
    }

/*
-------------------------------------------------------------------------------

    DESCRIPTION

    This module contains implementation of CTestScripterController class member
    functions.

-------------------------------------------------------------------------------
*/
#define LOGGER iEngine->Logger()

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

    Class: CTestScripterController

    Method: CTestScripterController

    Description: Default constructor

    C++ default constructor can NOT contain any code, that
    might leave.

    Parameters: CTestEngine* aEngine: in: Pointer to CTestEngine

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CTestScripterController::CTestScripterController( CTestEngine* aEngine ) :
    CTestModuleController( aEngine )
    {
    // None

    }

/*
-------------------------------------------------------------------------------

    Class: CTestScripterController

    Method: ConstructL

    Description: Symbian OS second phase constructor

    Symbian OS default constructor can leave.

    Parameters: const TName& aName: in: Test module name
                TBool aAfterReboot: in: Reboot indication

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestScripterController::ConstructL( const TName& aName,
                                          TBool aAfterReboot )
    {
    __TRACE( KInit, ( 
        _L( "NOTE: Test module is TestScripter and each test case(config) file(s) will have own server(s)" ) ) );

    // Remove optional '.DLL' from file name
    TName name = aName;
    name.LowerCase();
    TParse parse;
    parse.Set( name, NULL, NULL );

    if ( parse.Ext() == _L(".dll") )
        {
        const TInt len = parse.Ext().Length();
        name.Delete ( name.Length()-len, len );
        }

    iName = name.AllocL();

    iAfterReboot = aAfterReboot;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestScripterController

    Method: InitL

    Description: Initialize test module.

    Parameters: TFileName& aIniFile: in: Initialization file of Test Module
                const TDesC& aConfigFile: in: Test case(config) file name(Used 
                in TestScripter case).

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestScripterController::InitL( TFileName& aIniFile, 
                                     const TDesC& /*aConfigFile*/ )
    {
    // Take initialization file of Test Module
    iInifile = aIniFile;

    // Just empty, don't create TestServer operations. TestServer
    // creation will be do in AddTestCaseFile()-method.

    }

/*
-------------------------------------------------------------------------------

    Class: CTestScripterController

    Method: NewL

    Description: Two-phased constructor.

    Parameters: CTestEngine* aEngine: in: CTestEngine object
                const TName& aName: in: Test module name
                TBool aAfterReboot: in: Reboot indication

    Return Values: CTestScripterController* : pointer to created object

    Errors/Exceptions: Leaves if called ConstructL method leaves

    Status: Approved

-------------------------------------------------------------------------------
*/
CTestScripterController* CTestScripterController::NewL( CTestEngine* aEngine,
                                                        const TName& aName,
                                                        TBool aAfterReboot )
    {
    CTestScripterController* self =
        new ( ELeave ) CTestScripterController( aEngine );
    CleanupStack::PushL( self );
    self->ConstructL( aName, aAfterReboot );
    CleanupStack::Pop( self );
    return self;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestScripterController

    Method: ~CTestScripterController

    Description: Destructor

    Deallocate all allocated resources. Delete CTestScripterController's
    CTestModuleController(s). After this is deleted base class also, see:
    CTestModuleController::~CTestModuleController(). 

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CTestScripterController::~CTestScripterController()
    {
    // Delete CTestScripterController(s).
    iTestScripter.ResetAndDestroy();
    iTestScripter.Close();

    // After this is deleted base class also, see:
    // CTestModuleController::~CTestModuleController(). 

    }

/*
-------------------------------------------------------------------------------

    Class: CTestScripterController

    Method: AddConfigFileL

    Description: Add config file.
                 - Creates CTestModuleController(Gives test case file name)
                 - Initializes CTestModuleController( creates a server session)
                 - Adds config file

    Parameters: TFileName& aConfigFile: in: Config file for TestScripter

    Return Values: None

    Errors/Exceptions: Leaves if AppendL leaves

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestScripterController::AddConfigFileL( TFileName& aConfigFile )
    {
    // Check that this config file does not already exists
    for(  TInt a = 0; a < iTestScripter.Count(); a++ )
        {
        for ( TInt i = 0; i < iTestScripter[a]->iConfigFiles.Count(); i++ )
            {
            // HBufC to TPtrC
            TPtrC name( iTestScripter[a]->iConfigFiles[i]->Des() );
            if ( KErrNone == aConfigFile.CompareF( name ) )
                {
                LeaveWithNotifyL( KErrAlreadyExists,  
                    _L( "Adding config file failed: Config file already exists" ) );
                }
            }
        }

    __TRACE( KInit, ( 
        _L( "CTestScripterController::AddConfigFileL aConfigFile=[%S]" ) ,
        &aConfigFile ) );

    HBufC* testScripterAndTestCaseFile = NULL; // InitL() takes TFileName
    testScripterAndTestCaseFile = CreateTestScripterNameL( 
                                            aConfigFile,
                                            testScripterAndTestCaseFile );
    // Add to cleanup stack here, because CreateTestScripterNameL needs to be
    // trapped in other methods.
    CleanupStack::PushL( testScripterAndTestCaseFile );

    //Create server and active object(This uses CTestModuleController::InitL())
    CTestModuleController* module = CTestModuleController::NewL( 
                                        iEngine,
                                        testScripterAndTestCaseFile->Des(),
                                        iAfterReboot, EFalse, this );
    CleanupStack::PopAndDestroy( testScripterAndTestCaseFile );
    CleanupStack::PushL( module );

    User::LeaveIfError( iTestScripter.Append( module ) );

    // Now is used TestScripter so give test case file also(used
    // in caps modifier cases). 
    TRAPD ( err, module->InitL( iInifile, aConfigFile ) );
    if( err != KErrNone )
        {
        __TRACE( KInit, ( _L( "InitL fails with error: %d" ), err ) );
        User::Leave( err );
        }

    module->AddConfigFileL( aConfigFile );

    CleanupStack::Pop( module );

    }

/*
-------------------------------------------------------------------------------

    Class: CTestScripterController

    Method: RemoveConfigFileL

    Description: Remove config file

    Parameters: TFileName& aConfigFile: in: Config file name

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestScripterController::RemoveConfigFileL( TFileName& aConfigFile )
    {
     __TRACE( KInit, ( _L( "CTestScripterController::RemoveConfigFileL" ) ) );

    HBufC* testScripterAndTestCaseFile = NULL;
    testScripterAndTestCaseFile = CreateTestScripterNameL( aConfigFile,
                                         testScripterAndTestCaseFile );
    // Add to cleanup stack here, because CreateTestScripterNameL needs to be
    // trapped in other methods.
    CleanupStack::PushL( testScripterAndTestCaseFile );

    // Get correct iTestScripter
    TInt index( -1 );
    for( TInt a = 0; a < iTestScripter.Count(); a++ )
        {
        if( ( iTestScripter[a]->iName->CompareF( 
                        testScripterAndTestCaseFile->Des() ) == KErrNone ) )
            {
            index = a;
            break;
            }
        }

    CleanupStack::PopAndDestroy( testScripterAndTestCaseFile );

    if( index == -1 )
        {
        LeaveWithNotifyL( KErrNotFound, 
            _L( "Removing config file failed: Config file not found" ) );
        }

    __TRACE( KInit, ( 
        _L( "CTestScripterController[%S]::RemoveConfigFileL aConfigFile=[%S]"),
        iTestScripter[index]->iName, &aConfigFile ) );

    // Find config file
    for ( TInt i = 0; i < iTestScripter[index]->iConfigFiles.Count(); i++ )
        {
        // HBufC to TPtrC
        TPtrC name( iTestScripter[index]->iConfigFiles[i]->Des() );
        if ( KErrNone == aConfigFile.CompareF( name ) )
            {
            HBufC* configFile = iTestScripter[index]->iConfigFiles[i];
            iTestScripter[index]->iConfigFiles.Remove(i);
            delete configFile;
            // Cancel enumerate and free test cases
            Cancel();
            FreeTestCases();
            // Testcasefile(config) is found, so we can return
            return;
            }
        }

    LeaveWithNotifyL( KErrNotFound, 
        _L( "Removing config file failed: Config file not found" ) );

    }

/*
-------------------------------------------------------------------------------

    Class: CTestScripterController

    Method: StartEnumerateL

    Description: Start test case enumeration

    Parameters: None

    Return Values: None

    Errors/Exceptions: LeaveWithNotifyL if no test case added. 

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestScripterController::StartEnumerateL()
    {
    if( iTestScripter.Count() <= 0 )
        {
        RDebug::Print( 
            _L( "There is no test case file for TestScripter registered yet. Enumeraton aborted." ) );
        LeaveWithNotifyL( KErrNotFound,
            _L( "There is no test case file for TestScripter registered yet. Enumeraton aborted." ) );
        return;
        }

    for( TInt a = 0; a < iTestScripter.Count(); a++ )
        {
        __TRACE( KVerbose, ( 
                _L( "CTestScripterController[%S]::StartEnumerateL" ),
                iTestScripter[a]->iName ) );
        iTestScripter[a]->iTestCaseCount = 0;
        iTestScripter[a]->iEnumerateCount = 0;
        iTestScripter[a]->iFailedEnumerateCount = 0;
        iTestScripter[a]->iFailedEnumerateConfig.Reset();
        iTestScripter[a]->iEnumerateComplete = EFalse;

        iTestScripter[a]->iState = ETestModuleEnumerateCases;
        
        if ( iTestScripter[a]->iConfigFiles.Count() > 0 )
            {
            // HBufC to TPtrC
            iTestScripter[a]->iEnumConfigFile.Set( 
                                iTestScripter[a]->iConfigFiles[0]->Des() );
            }

        __TRACE( KInit, ( 
            _L( "Getting testcases from module [%S], test case file[%S]" ),
            iTestScripter[a]->iName, &iTestScripter[a]->iEnumConfigFile ) );
        
        iTestScripter[a]->SetActive();
        iTestScripter[a]->iModule.EnumerateTestCases( 
                                    iTestScripter[a]->iEnumConfigFile,
                                    iTestScripter[a]->iEnumResultPackage,
                                    iTestScripter[a]->iStatus );
        } // End of for-loop

    }

/*
-------------------------------------------------------------------------------

    Class: CTestScripterController

    Method: GetCurrentIndex

    Description: Get current CTestScripterController.

    Parameters: CTestModuleController* aTestModuleController: in:
                Pointer current to CTestModuleController

    Return Values: KErrNotFound returned if CTestModuleController not found
                   else current index returned.

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CTestScripterController::GetCurrentIndex( CTestModuleController*
                                                    aTestModuleController )
    {
    // Get correct iTestScripter
    return iTestScripter.Find( aTestModuleController );

    }

/*
-------------------------------------------------------------------------------

    Class: CTestScripterController

    Method: RunL

    Description: RunL handles completed requests.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestScripterController::RunL()
    {
    // Should never come here because one TestScripter have one test case
    // file per server session.
    // CTestScripterController's base class is an active object but this class
    // not used as an active object. So there not used RunL => Panic.

    User::Panic( _L( "CTestScripterController::RunL()" ), KErrCorrupt );

    }

/*
-------------------------------------------------------------------------------

    Class: CTestScripterController

    Method: RunLEmulator

    Description: RunLEmulator handles completed requests(Emulates RunL()).
                 This is called from CTestModuleController::RunL.

    Parameters: CTestModuleController* aTestModuleController: in: pointer to
                CTestModuleController.

    Return Values: None

    Errors/Exceptions: Leaves if some of called leaving methods leaves

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestScripterController::RunLEmulator( CTestModuleController*
                                                    aTestModuleController )
    {
    iTestScripterIndicator++;
    TInt index = GetCurrentIndex( aTestModuleController );
    if( index < KErrNone )
        {
        User::Leave( KErrNotFound );
        }

    __TRACE( KVerbose, ( 
        _L( "CTestScripterController[%S]::RunLEmulator [iStatus = %d]"),
        iTestScripter[index]->iName, iTestScripter[index]->iStatus.Int() ) );

    // Note:
    // If test case not found there may be existing cases from previous
    // enumerations, so those cases are valid. e.g. case: "add test case file",
    // "add test case file that not exist" and "get test cases".

    TInt ret( KErrNone );

    // Check that request was successful
    if( iTestScripter[index]->iStatus.Int() == KErrNone )
        {
        // Get enumerated test cases and append them to array
        CFixedFlatArray<TTestCaseInfo>* testCases = 
                        CFixedFlatArray<TTestCaseInfo>::NewL( 
                        iTestScripter[index]->iEnumResultPackage() );
        CleanupStack::PushL( testCases );
        ret = iTestScripter[index]->iModule.GetTestCases( *testCases );
        __TRACE( KInit, ( 
                _L( "RunL()'s GetTestCases method returns: %d" ), ret ) );

        iTestScripter[index]->iTestCaseCount += testCases->Count();

        if ( testCases->Count() == 0 )
            {

            if (iTestScripter[index]->iConfigFiles.Count() > 0)
                {
                __TRACE( KInit, ( 
                    CStifLogger::EBold, 
                    _L( "Module [%S], test case file[%S] returned 0 cases" ),
                    iTestScripter[index]->iName,
                    iTestScripter[index]->iConfigFiles[iTestScripter[index]->iEnumerateCount] ) );
                }
            else
                {
                __TRACE( KInit, ( CStifLogger::EBold,
                    _L("Module [%S] without test case file, returned 0 cases"),
                    iTestScripter[index]->iName ) );
                }
            iTestScripter[index]->iEngine->ErrorPrint( 1,
                _L("Someone returned 0 test cases. Check testengine log"));
            iTestScripter[index]->iAtsLogger->CommentL( 
                _L("Test module returned 0 test cases") );   
            }

        // Store test cases for later use
        User::LeaveIfError( iTestScripter[index]->iTestCaseArray.Append( 
                                                                testCases ) );
        CleanupStack::Pop( testCases );
        
        }
    else
        {
        // Calculate failed enumeration count
        iTestScripter[index]->iFailedEnumerateCount++;
        // Add failed config(test case) file to array for later removing
        if( iTestScripter[index]->iConfigFiles.Count() != NULL )
            {
            __TRACE( KError, (
                CStifLogger::ERed, 
                _L( "Test case[%S] enumeration fails with error: %d" ),
                iTestScripter[index]->iConfigFiles[iTestScripter[index]->iEnumerateCount],
                iTestScripter[index]->iStatus.Int() ) );
            // Append
            iTestScripter[index]->iFailedEnumerateConfig.Append(
                iTestScripter[index]->iConfigFiles[iTestScripter[index]->iEnumerateCount] );
            }
        }

    TInt count = iTestScripter.Count();

    // All TestScripter enumerations is finished for cleaning if necessarily
    // and complete request up to engine.
    if( count == iTestScripterIndicator )
        {
        TInt testCaseCount( 0 );
        for( TInt a = 0; a < count; a++  )
            {
            // Remove faulty config (test case) file(s) and 
            // iEnumerateCount not increased.
            if( iTestScripter[a]->iTestCaseCount == 0 )
                {
                // Check that test case file is not allready removed.
                if( iTestScripter[a]->iConfigFiles.Count() != 0 )
                    {
                    iTestScripter[a]->iConfigFiles.Remove( 0 );
                    }
                }
            else
                {
                iTestScripter[a]->iEnumerateCount++;
                testCaseCount += iTestScripter[a]->iTestCaseCount;
                }

            // All test cases enumerated
            iTestScripter[a]->iState = ETestModuleEnumerateCasesCompleted;
            iTestScripter[a]->iEnumerateComplete = ETrue;
            }

        iTestScripterIndicator = 0; // Initialization back to 0, enumerations
                                    // are done at this run.
        iEngine->EnumerationCompleted( testCaseCount );
        }

    }

/*
-------------------------------------------------------------------------------

    Class: CTestScripterController

    Method: TestCasesL

    Description: Return Test Cases

    Parameters: None

    Return Values: CFixedFlatArray<TTestInfo>*: Pointer to created array

    Errors/Exceptions: Leaves if NewL leaves

    Status: Approved

-------------------------------------------------------------------------------
*/
CFixedFlatArray<TTestInfo>* CTestScripterController::TestCasesL()
    {
    TInt testCaseCount( 0 );
    for( TInt i = 0; i < iTestScripter.Count(); i++  )
        {
        testCaseCount += iTestScripter[i]->iTestCaseCount;
        }

    CFixedFlatArray<TTestInfo>* testCases = 
        CFixedFlatArray<TTestInfo>::NewL( testCaseCount );

    CleanupStack::PushL( testCases );

    // Loop through all test cases from all config files
    TInt totalCount = 0;
    // HBufC to TPtrC
    for( TInt a = 0; a < iTestScripter.Count(); a++ )
        {
        TPtrC name( iTestScripter[a]->iName->Des() );
        for ( TInt i = 0; i < iTestScripter[a]->iEnumerateCount; i++ )
            {
            TTestInfo tmpInfo;
            tmpInfo.iModuleName = name;
            if ( iTestScripter[a]->iConfigFiles.Count() > 0 )
                {
                tmpInfo.iConfig = *iTestScripter[a]->iConfigFiles[i];
                }

            // Get test cases from iTestCaseArray at [i]
            if( (iTestScripter[a]->iTestCaseArray)[i] )
                {
                CFixedFlatArray<TTestCaseInfo>* tempTestCases = 
                                    (iTestScripter[a]->iTestCaseArray)[i];
                for ( TInt j = 0; j < tempTestCases->Count(); j++ )
                    {
                    tmpInfo.iTestCaseInfo = (*tempTestCases)[j];
                    // Set TestCaseInfo to testCases array
                    testCases->Set( totalCount, tmpInfo );
                    totalCount++;
                    }
                }
            }
        }

    CleanupStack::Pop( testCases );

    return testCases;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestScripterController

    Method: FreeTestCases

    Description: Free memory used for test cases

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestScripterController::FreeTestCases()
    {
    for( TInt a = 0; a < iTestScripter.Count(); a++ )
        {
        // Reset and destroy test cases
        iTestScripter[a]->iTestCaseArray.ResetAndDestroy();
        iTestScripter[a]->iTestCaseArray.Close();

        iTestScripter[a]->iTestCaseCount = 0;
        iTestScripter[a]->iEnumerateComplete = EFalse;
        }

    }

/*
-------------------------------------------------------------------------------

    Class: CTestScripterController

    Method: DoCancel

    Description: Cancel active request

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestScripterController::DoCancel()
    {
    // Should never come here because one TestScripter have one test case
    // file per server session.
    // CTestScripterController's base class is an active object but this class
    // not used as an active object. So there not used DoCancel => Panic.

    User::Panic( _L( "CTestScripterController::DoCancel()" ), KErrCorrupt );

    }

/*
-------------------------------------------------------------------------------

    Class: CTestScripterController

    Method: DoCancelEmulator

    Description: Cancel active request(Emulates DoCancel)
                 This is called from CTestModuleController::DoCancel.

    Parameters: CTestModuleController* aTestModuleController: in: pointer to
                CTestModuleController.

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestScripterController::DoCancelEmulator(
                                CTestModuleController* aTestModuleController )
    {
    TInt index = GetCurrentIndex( aTestModuleController );
    if( index < KErrNone )
        {
        User::Leave( KErrNotFound );
        }

    __TRACE( KVerbose, ( 
        _L( "CTestScripterController[%S]::RunDoCancelEmulator" ),
        iTestScripter[index]->iName ) );

    switch ( iTestScripter[index]->iState )
        {
        case ETestModuleEnumerateCases:
            iTestScripter[index]->iModule.CancelAsyncRequest(
                                            ETestModuleEnumerateTestCases );
            iTestScripter[index]->iEnumerateComplete = ETrue;
            // Enumeration canceled, complete with KErrCancel
            iTestScripter[index]->iEngine->EnumerationCompleted(
                                                            0, KErrCancel );

            // Free allocated test cases because EnumerateTestCases was
            // canceled
            FreeTestCases();
            break;
        case ETestModuleIdle:
        case ETestModuleEnumerateCasesCompleted:
        default:
            // DoCancel called in wrong state => Panic
            _LIT( KTestModuleController, "CTestModuleController" );
            User::Panic( KTestModuleController, EDoCancelDisorder );
            break;
        }

    iTestScripter[index]->iAtsLogger->ErrorL( 
                        _L("Test case enumeration cancelled") );
    iTestScripter[index]->iState = ETestModuleIdle;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestScripterController

    Method: RunError

    Description: Handle errors.

    Parameters: TInt aError: in: Symbian OS error: Error code

    Return Values: TInt KErrNone: Always returned KErrNone

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CTestScripterController::RunError( TInt /*aError*/ )
    {
    // Should never come here because one TestScripter have one test case
    // file per server session.
    // CTestScripterController's base class is an active object but this class
    // not used as an active object. So there not used RunError => Panic.

    User::Panic( _L( "CTestScripterController::RunError()" ), KErrCorrupt );
    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestScripterController

    Method: RunErrorEmulator

    Description: Handle errors(Emulates RunError).
                 This is called from CTestModuleController::RunError.

    Parameters: TInt aError: in: Symbian OS error: Error code.
                CTestModuleController* aTestModuleController: in: pointer to
                CTestModuleController.

    Return Values: TInt KErrNone: Always returned KErrNone

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CTestScripterController::RunErrorEmulator( TInt aError,
                                CTestModuleController* aTestModuleController )
    {
    TInt index = GetCurrentIndex( aTestModuleController );
    if( index < KErrNone )
        {
        User::Leave( KErrNotFound );
        }
    
    __TRACE( KError, ( CStifLogger::ERed,
        _L( "CTestScripterController[%S]::RunErrorEmulator aError=[%d]" ),
        iTestScripter[index]->iName, aError ) );
    if( aError == KErrNoMemory )
        {
        __TRACE( KError, ( CStifLogger::ERed, 
            _L( "No memory available. Test case file's size might be too big." ) ) );
        }

    iTestScripter[index]->iEnumerateComplete = ETrue;

    iTestScripter[index]->iAtsLogger->ErrorL( 
                            _L("Test module did not return any test cases") );
    
    _LIT( KErrorText, " did not return any test cases [error: ");
    if( KErrorText().Length() +
            iTestScripter[index]->iName->Length() +
            1 < KMaxName )
        {
        // Enumeration failed, print warning and complete with KErrNone
        TName error( iTestScripter[index]->iName->Des() );
        error.Append( KErrorText );  
        error.AppendNum( aError );  
        error.Append( _L("]") );  
        iEngine->ErrorPrint ( 0, error );
    
        iTestScripter[index]->iEngine->EnumerationCompleted( 0, KErrNone );
        }
    else 
        {
        // Cannot only print warning, complete with error
        iTestScripter[index]->iEngine->EnumerationCompleted( 0, aError );
        }

    // Free allocated test cases because EnumerateTestCases failed
    FreeTestCases();
    iTestScripter[index]->iEnumerateComplete = ETrue;

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestScripterController

    Method: EnumerationComplete

    Description: Is enumeration of test cases complete.

    Parameters: None
    
    Return Values: TBool ETrue: Enumeration of test cases is complete
                         EFalse: Enumeration is not complete

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TBool CTestScripterController::EnumerationComplete()
    {
    for( TInt a = 0; a < iTestScripter.Count(); a++ )
        {
        if( !iTestScripter[a]->iEnumerateComplete )
            {
            return EFalse;
            }
        }
    return ETrue;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestScripterController

    Method: Server

    Description: Return handle to Test Server

    Parameters: TTestInfo& aTestInfo: in: Test info for this test case

    Return Values: RTestServer& : Reference to RTestServer

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
RTestServer& CTestScripterController::Server( TTestInfo& aTestInfo )
    {
    HBufC* testScripterAndTestCaseFile = NULL;
    TRAPD( ret, testScripterAndTestCaseFile = CreateTestScripterNameL( 
                                            aTestInfo.iConfig,
                                            testScripterAndTestCaseFile ) );
    // Add to cleanup stack here, because CreateTestScripterNameL needs to be
    // trapped in other methods.
    CleanupStack::PushL( testScripterAndTestCaseFile );
    if( ret != KErrNone )
        {
        User::Panic( 
            _L( "CTestScripterController::Server(): CreateTestScripterNameL" ),
            ret );
        }

    // Get correct handle
    TInt index( KErrNotFound );
    for( TInt a = 0; a < iTestScripter.Count(); a++ )
        {
        if( testScripterAndTestCaseFile->Des() == 
                                            iTestScripter[a]->iName->Des() )
            {
            index = a;
            break;
            }
        }

    CleanupStack::PopAndDestroy( testScripterAndTestCaseFile );

    if( index == KErrNotFound )
        {
        User::Panic(
            _L( "CTestScripterController::Server(): Index not found" ),
            KErrNotFound );
        }

    // Return handle
    return iTestScripter[index]->iServer;
    }

/*
-------------------------------------------------------------------------------

    Class: CTestScripterController

    Method: GetFreeOrCreateModuleControllerL

    Description: Return pointer to test module controller.
                 Find controller which does not run test case
                 and if this is not possible then create new one.

    Parameters: TTestInfo& aTestInfo: in: Test info for this test case
                TBool aUITestingSupport: in: Is UI testing mode enabled

    Return Values: CTestModuleController* : pointer to controller

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CTestModuleController* CTestScripterController::GetFreeOrCreateModuleControllerL(TTestInfo& aTestInfo, TBool aUITestingSupport)
    {
    HBufC* testScripterAndTestCaseFile = NULL;
    TRAPD(ret, testScripterAndTestCaseFile = CreateTestScripterNameL(aTestInfo.iConfig, testScripterAndTestCaseFile));

    // Add to cleanup stack here, because CreateTestScripterNameL needs to be trapped in other methods.
    CleanupStack::PushL(testScripterAndTestCaseFile);
    if(ret != KErrNone)
        {
        User::Panic(_L("CTestScripterController::GetFreeOrCreateModuleControllerL(): CreateTestScripterNameL"), ret);
        }

    CTestModuleController* resultController = NULL;
    CTestModuleController* parentController = NULL;
    TInt j;
    
    __TRACE(KInit, (_L("Find free real module controller (or create new one)")));

    // Get handle to correct "parent" module controller
    __TRACE(KInit, (_L("Searching for parent module controller named [%S]"), testScripterAndTestCaseFile));
    for(TInt a = 0; a < iTestScripter.Count(); a++)
        {
        //Check if module name matches to given name
        if(iTestScripter[a]->iName->Des() == testScripterAndTestCaseFile->Des())
            {
            parentController = iTestScripter[a]; 
            __TRACE(KInit, (_L("Parent module controller [%S] has been found. Checking its %d children"), parentController->iName, parentController->iChildrenControllers.Count()));
            //Now check all its children and find free one
            //In UI testing mode always create new module controller
            if(!aUITestingSupport)
                {
                for(j = 0; j < parentController->iChildrenControllers.Count(); j++)
                    {
                    if(parentController->iChildrenControllers[j]->iTestCaseCounter == 0)
                        {
                        resultController = parentController->iChildrenControllers[j];
                        __TRACE(KInit, (_L("Free real module controller found [%S]"), resultController->iName));
                        break;
                        }
                    else
                        {
                        __TRACE(KInit, (_L("Module controller found [%S] but is not free (it runs %d test cases)"), parentController->iChildrenControllers[j]->iName, parentController->iChildrenControllers[j]->iTestCaseCounter));
                        }
                    }
                }
            else
                {
                __TRACE(KInit, (_L("In UITestingSupport mode new module controller will be always created")));
                }
            }
        }

    //Append underscore to name
    TPtr ptr = testScripterAndTestCaseFile->Des();
    ptr.Append(_L("@"));

    //Create new module controller if free one has not been found
    if(!resultController)
        {
        TBuf<10> ind;
        ind.Format(_L("%d"), iEngine->GetIndexForNewTestModuleController());
        TPtr ptr = testScripterAndTestCaseFile->Des();
        ptr.Append(ind);
        __TRACE(KInit, (_L("Free real module controller not found. Creating new one [%S]."), testScripterAndTestCaseFile));
            
        //Create server and active object (This uses CTestModuleController::InitL())
        CTestModuleController* module = CTestModuleController::NewL( 
                                            iEngine,
                                            testScripterAndTestCaseFile->Des(),
                                            iAfterReboot, EFalse, this);
        CleanupStack::PushL(module);
        parentController->iChildrenControllers.AppendL(module);
        __TRACE(KInit, (_L("Child added to [%S] controller. Currently it has %d children:"), parentController->iName, parentController->iChildrenControllers.Count()));
        for(j = 0; j < parentController->iChildrenControllers.Count(); j++)
            {
            __TRACE(KInit, (_L("    %d. [%S]"), j + 1, parentController->iChildrenControllers[j]->iName));
            }

        // Now is used TestScripter so give test case file also(used
        // in caps modifier cases). 
        TRAPD(err, module->InitL(iInifile, aTestInfo.iConfig));
        if(err != KErrNone)
            {
            __TRACE(KVerbose, (_L("InitL fails with error: %d" ), err));
            User::Leave(err);
            }

        module->AddConfigFileL(aTestInfo.iConfig);

        __TRACE(KInit, (_L("New module controller created [%S]."), testScripterAndTestCaseFile));

        //Enumerate test cases
        module->EnumerateSynchronously();

        CleanupStack::Pop(module);
        resultController = module;
        }

    CleanupStack::PopAndDestroy(testScripterAndTestCaseFile);

    // Return handle
    return resultController;
    }

/*
-------------------------------------------------------------------------------

    Class: CTestScripterController

    Method: ModuleName

    Description: Return the name of Test Scripter

    Parameters: const TDesC& aModuleName: in: Module name

    Return Values: const TDesC : Name of Test Scripter

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
const TDesC& CTestScripterController::ModuleName( const TDesC& aModuleName )
    {
    // If test case file not added yet.
    if( iTestScripter.Count() == 0 || aModuleName == KTestScripterName )
        {
        return *iName;
        }

    // Test case(s) is(are) added. Scan the name from corrent TestScripter
    // session
    for( TInt a = 0; a < iTestScripter.Count(); a++ )
        {
        if( aModuleName == iTestScripter[a]->iName->Des() )
            {
            return *iTestScripter[a]->iName;
            }
        }

    return KNullDesC;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestScripterController

    Method: CreateTestScripterNameL

    Description: Create name according to TestScripter and Test case file.

    Parameters: TFileName& aTestCaseFile: in: Test case file with path and name
                TFileName& aCreatedName: inout: Created name

    Return Values: None

    Errors/Exceptions: Leaves is test case file is too long

    Status: Approved

-------------------------------------------------------------------------------
*/
HBufC* CTestScripterController::CreateTestScripterNameL( 
                                                    TFileName& aTestCaseFile,
                                                    HBufC* aCreatedName )
    {
    TParse parse;
    parse.Set( aTestCaseFile, NULL, NULL );

    TInt length( 0 );
    length = parse.Name().Length();
    length += ( KTestScripterNameLength + 1 );
    length += 10; //this will be used to add unique identifier (when run test case in separate process is on)

    // aCreatedName to CleanupStack
    aCreatedName = HBufC::NewLC( length );
    TPtr ptr = aCreatedName->Des();

    // Maximum length of TestScripter's name(Max limitation from
    // CTestModuleController creation)
    TInt maximumLength = KMaxName - ( KTestScripterNameLength + 1 );

    // Start create name. Format is testscripter_testcasefile
    ptr.Copy( KTestScripterName );
    ptr.Append( _L( "_" ) );
    if( parse.Name().Length() < maximumLength )
        {
        ptr.Append( parse.Name() );
        ptr.LowerCase();
        }
    else
        {
        __TRACE( KInit, ( CStifLogger::ERed,
            _L( "TestScripter test case file(config)'s name is too long. Current length[%d], allowed max length[%d]. Cannot continue" ),
            parse.Name().Length(), maximumLength ) );
        User::Leave( KErrArgument );
        }

    // Pop here because this method can be trapped and trap panics with
    // E32USER-CBase if cleap up stack is not empty.
    CleanupStack::Pop( aCreatedName );

    return aCreatedName;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestScripterController

    Method: DeleteModuleController

    Description: Finds specified module controller and deletes it.

    Parameters: CTestModuleController* aRealModuleController: module controller
                   to be deleted.
    
    Return Values: None
    
    Errors/Exceptions: None

    Status: 

-------------------------------------------------------------------------------
*/    
void CTestScripterController::DeleteModuleController(CTestModuleController* aRealModuleController)
    {
    __TRACE(KInit, (_L("Attempting to delete real module controller [%S]"), aRealModuleController->iName));

    TInt i, j, k;
    TInt children;
    TInt subcontrollers = iTestScripter.Count();
    
    for(k = 0; k < subcontrollers; k++)
        {
        children = iTestScripter[k]->iChildrenControllers.Count();
        __TRACE(KInit, (_L("...checking controller [%S] which has %d children"), iTestScripter[k]->iName, children));

        for(i = 0; i < children; i++)
            {
            if(iTestScripter[k]->iChildrenControllers[i] == aRealModuleController)
                {
                __TRACE(KInit, (_L("Real module controller found... deleting")));
                delete iTestScripter[k]->iChildrenControllers[i];
                iTestScripter[k]->iChildrenControllers.Remove(i);
    
                __TRACE(KInit, (_L("Child removed from [%S] controller. Currently it has %d children:"), iTestScripter[k]->iName, iTestScripter[k]->iChildrenControllers.Count()));
                for(j = 0; j < iTestScripter[k]->iChildrenControllers.Count(); j++)
                    {
                    __TRACE(KInit, (_L("    %d. [%S]"), j + 1, iTestScripter[k]->iChildrenControllers[j]->iName));
                    }
    
                return;
                }
            }
        }
    __TRACE(KInit, (_L("Real module controller NOT found... NOT deleting")));
    }

/*
-------------------------------------------------------------------------------

    Class: CTestScripterController

    Method: RemoveModuleController

    Description: Finds specified module controller and removes it from children list.

    Parameters: CTestModuleController* aRealModuleController: module controller
                   to be removed.
    
    Return Values: None
    
    Errors/Exceptions: None

    Status: 

-------------------------------------------------------------------------------
*/    
void CTestScripterController::RemoveModuleController(CTestModuleController* aRealModuleController)
    {
    __TRACE(KInit, (_L("Attempting to remove real module controller [%x]"), aRealModuleController));

    TInt i, j, k;
    TInt children;
    TInt subcontrollers = iTestScripter.Count();
    
    for(k = 0; k < subcontrollers; k++)
        {
        children = iTestScripter[k]->iChildrenControllers.Count();
        __TRACE(KInit, (_L("...checking controller [%S] which has %d children"), iTestScripter[k]->iName, children));

        for(i = 0; i < children; i++)
            {
            if(iTestScripter[k]->iChildrenControllers[i] == aRealModuleController)
                {
                __TRACE(KInit, (_L("Real module controller found... removing")));
                iTestScripter[k]->iChildrenControllers.Remove(i);
    
                __TRACE(KInit, (_L("Child removed from [%S] controller. Currently it has %d children:"), iTestScripter[k]->iName, iTestScripter[k]->iChildrenControllers.Count()));
                for(j = 0; j < iTestScripter[k]->iChildrenControllers.Count(); j++)
                    {
                    __TRACE(KInit, (_L("    %d. [%S]"), j + 1, iTestScripter[k]->iChildrenControllers[j]->iName));
                    }
    
                return;
                }
            }
        }
    __TRACE(KInit, (_L("Real module controller NOT found... NOT removing")));
    }

/*
-------------------------------------------------------------------------------

    DESCRIPTION

    This module contains implementation of CErrorPrinter class member
    functions.

-------------------------------------------------------------------------------
*/

/*
-------------------------------------------------------------------------------

    Class: CErrorPrinter

    Method: NewL

    Description: Create a testcase runner.

    Parameters: CTestEngine* aMain: in: Pointer to console main

    Return Values: CErrorPrinter* : pointer to created object

    Errors/Exceptions: Leaves if memory allocation for object fails
                       Leaves if ConstructL leaves

    Status: Approved

-------------------------------------------------------------------------------
*/
CErrorPrinter* CErrorPrinter::NewL( CTestEngine* aTestEngine )
    {
    CErrorPrinter* self = new ( ELeave ) CErrorPrinter();
    CleanupStack::PushL( self );
    self->ConstructL( aTestEngine );
    CleanupStack::Pop( self );
    return self;

    }

/*
-------------------------------------------------------------------------------

    Class: CErrorPrinter

    Method: ConstructL

    Description: Second level constructor.

    Parameters: CTestEngine* aEngine: in: Pointer to Engine

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CErrorPrinter::ConstructL( CTestEngine* aEngine )
    {
    iEngine = aEngine;

    }

/*
-------------------------------------------------------------------------------

    Class: CErrorPrinter

    Method: CErrorPrinter

    Description: Constructor.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CErrorPrinter::CErrorPrinter( ) : CActive( EPriorityStandard ),
                                      iErrorPckg( iError )
    {

    }

/*
-------------------------------------------------------------------------------

    Class: CErrorPrinter

    Method: ~CErrorPrinter

    Description: Destructor.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CErrorPrinter::~CErrorPrinter( )
    {
    Cancel();

    }

/*
-------------------------------------------------------------------------------

    Class: CErrorPrinter

    Method: StartL

    Description: Starts a test case and sets the active object to active.

    Parameters: RTestModule& aServer: in: Reference to the server object

    Return Values: None

    Errors/Exceptions: TInt: Return KErrNone

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CErrorPrinter::StartL( RTestModule& aServer )
    {
    iServer = aServer;

    CActiveScheduler::Add ( this );

    SetActive();
    aServer.ErrorNotification ( iErrorPckg, iStatus );

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CErrorPrinter

    Method: RunL

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CErrorPrinter::RunL()
    {

    if ( iStatus.Int() != KErrNone )
        {
        __TRACE( KVerbose, ( _L( "In CErrorPrinter::RunL [iStatus = %d]" ), iStatus.Int() ) );
        }
    else
       {
        // Forward error print to UI and set request again active.
        iEngine->ErrorPrint( iErrorPckg );
        SetActive();
        iServer.ErrorNotification ( iErrorPckg, iStatus );
       }
    }

/*
-------------------------------------------------------------------------------

    Class: CErrorPrinter

    Method: DoCancel

    Description: Cancels the asynchronous request

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CErrorPrinter::DoCancel()
    {
    iServer.CancelAsyncRequest ( ETestModuleErrorNotification );

    }

/*
-------------------------------------------------------------------------------

    Class: CErrorPrinter

    Method: RunError

    Description: Handles errors. RunL can't leave so just forward error
    and let framework handle error.

    Parameters: TInt aError: in: Error code

    Return Values: TInt: Error code

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CErrorPrinter::RunError( TInt aError )
    {
    return aError;

    }
/*
-------------------------------------------------------------------------------

    Class: CServerStateHandler

    Method: NewL

    Description: Constructs a new CServerStateHandler object.

    Parameters: CTestEngine* aMain

    Return Values: CServerStateHandler*: New undertaker

    Errors/Exceptions: Leaves if memory allocation or ConstructL leaves.

    Status: Proposal

-------------------------------------------------------------------------------
*/
CServerStateHandler* CServerStateHandler::NewL( CTestEngine* aTestEngine, 
                                                CTestModuleController* aTestModuleController )
    {

    CServerStateHandler* self = 
                        new( ELeave ) CServerStateHandler( aTestEngine, aTestModuleController );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;

    }
/*
-------------------------------------------------------------------------------

    Class: CServerStateHandler

    Method: ConstructL

    Description: Second level constructor.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CServerStateHandler::ConstructL()
    {

    }

/*
-------------------------------------------------------------------------------

    Class: CServerStateHandler

    Method: CServerStateHandler

    Description: Constructor

    Parameters: CTestEngine* aMain

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
CServerStateHandler::CServerStateHandler( CTestEngine* aTestEngine, 
                                          CTestModuleController* aTestModuleController ) :
    CActive( CActive::EPriorityStandard ),
    iEngine( aTestEngine ),
    iTestModuleController( aTestModuleController )
    {
    
    }

/*
-------------------------------------------------------------------------------

    Class: CServerStateHandler

    Method: ~CServerStateHandler

    Description: Destructor. 
    Cancels active request.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
CServerStateHandler::~CServerStateHandler()
    {
    
    Cancel();

    iServerThread.Close();
    
    }
/*
-------------------------------------------------------------------------------

    Class: CServerStateHandler

    Method: StartL

    Description: Starts to monitor server thread.

    Parameters: RTestServer& aServer

    Return Values: TInt: 

    Errors/Exceptions: 

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
TInt CServerStateHandler::StartL( RTestServer& aServer )
    {
    
    __TRACE( KVerbose, ( _L( "CServerStateHandler::StartL" ) ) );
     
    TThreadId serverThreadId; 
     
    iStatus = KRequestPending;
  
    // Asks from server its thread ID value
    User::LeaveIfError( aServer.GetServerThreadId ( serverThreadId ) ); 
      
    // Opens handle to thread
    User::LeaveIfError( iServerThread.Open( serverThreadId ) );
    
    CActiveScheduler::Add( this );
        
    // Requests notification when this thread dies, normally or otherwise   
    iServerThread.Logon( iStatus ); // Miten RThread hanska ko serveriin..
   
    SetActive();

    return KErrNone;

    }
/*
-------------------------------------------------------------------------------

    Class: CServerStateHandler

    Method: RunL

    Description: Handles thread death.
  
    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CServerStateHandler::RunL()
    {
    
    // something went badly wrong!
    __TRACE( KInit, ( CStifLogger::ERed, 
        _L( "Test case execution fails. Possible reason: KErrServerTerminated" ) ) );
 
    RDebug::Print( _L("Test case execution fails. Possible reason: KErrServerTerminated") );     
             
    // Note: 
    // More Info about STIF panic with KErrServerTerminated 
    // will be informed to the user via testengine log and testreport    
    // in CTestCaseController::RunL() method
   
    // TestModuleCrash is called for doing all needed recovering operations for enabling STIF 
    // to continue test case execution
    iEngine->TestModuleCrash( iTestModuleController );
      
    }
/*
-------------------------------------------------------------------------------

    Class: CServerStateHandler

    Method: DoCancel

    Description: Stops listening TestServer status.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
void CServerStateHandler::DoCancel()
    {

    __TRACE( KVerbose, ( _L( "CServerStateHandler::DoCancel" ) ) );

    // Cancels an outstanding request for notification of the death of this thread.
    iServerThread.LogonCancel( iStatus );
    
    }
/*
-------------------------------------------------------------------------------

    Class: CServerStateHandler

    Method: RunError

    Description: Handle errors. RunL function does not leave, so one should
    never come here. 

    Print trace and let framework handle error( i.e to do Panic )

    Parameters: TInt aError: in: Error code

    Return Values:  TInt: Error code

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
TInt CServerStateHandler::RunError( TInt aError )
    {
    __TRACE( KError,( _L( "CServerStateHandler::RunError" ) ) );
    return aError;

    }

/*
-------------------------------------------------------------------------------

    Class: -

    Method: GenerateModuleName

    Description: Check is module TestScripter. Does parsing and returns new
                 module name and error codes(Needed operations when creating
                 server sessions to TestScripter). 

    Parameters: const TFileName& aModuleName: in: Module name for checking.
                TFileName& aNewModuleName: inout: Parsed module name.

    Return Values: KErrNone if TestScripter releated module.
                   KErrNotFound if not TestScripter releated module.

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/

TInt GenerateModuleName(const TDesC& aModuleName,
                        TDes& aNewModuleName)
    {
    // Check that length is greated than KTestScripterNameLength
    if( aModuleName.Length() < KTestScripterNameLength )
        {
        return KErrNotFound;
        }
    // Check is TestScripter
    TPtrC check( aModuleName.Mid( 0, KTestScripterNameLength ) );
    TInt ret = check.CompareF( KTestScripterName );
    if( ret == KErrNone )
        {
        aNewModuleName.Copy( aModuleName.Mid( 0, KTestScripterNameLength ) );
        aNewModuleName.LowerCase();
        }
    else
        {
        return KErrNotFound;
        }

    return KErrNone;

    }

// ================= OTHER EXPORTED FUNCTIONS =================================

// None

// End of File
