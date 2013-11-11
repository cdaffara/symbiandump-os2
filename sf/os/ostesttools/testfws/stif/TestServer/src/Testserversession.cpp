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
* Description: This module contains implementation of CTestServer 
* class member functions.
*
*/

// INCLUDE FILES
#include <e32std.h>
#include <e32svr.h>
#include "TestEngineClient.h"
#include <StifTestModule.h>
#include <stifinternal/TestServerClient.h>
#include "TestServer.h"
#include "TestServerCommon.h"
#include "TestServerModuleIf.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES
// Struct to pass parameters to server thread
struct TThreadStartTestServerSession
    {
    RThread    iServerThread; // The server thread
    RSemaphore iStarted;      // Startup syncronisation semaphore   
    TInt       iStartupResult;// Start-up result
    };

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ==================== LOCAL FUNCTIONS =======================================

// None

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

    Class: CTestCasesList

    Method: NewL

    Description: Create new test cases list    

    Parameters: const TDesC& aConfigFileName :in:  Config file name

    Return Values: CTestCasesList* Pointer to new test cases list

    Errors/Exceptions: Leaves if memory allocation fails or ConstructL leaves.

    Status: Approved

-------------------------------------------------------------------------------
*/
CTestCasesList* CTestCasesList::NewL( const TDesC& aConfigFileName )
    {    
    CTestCasesList* self = new(ELeave)CTestCasesList;
    CleanupStack::PushL( self );
    self->ConstructL( aConfigFileName );
    CleanupStack::Pop( self );
    return self;
    }

/*
-------------------------------------------------------------------------------

    Class: CTestCasesList

    Method: ~CTestCasesList

    Description: Destructor    

    Parameters: 

    Return Values: 

    Errors/Exceptions: 

    Status: Approved

-------------------------------------------------------------------------------
*/
CTestCasesList::~CTestCasesList()
    {
    delete iConfigFileName;
    iConfigFileName = NULL;
    iTestCases.ResetAndDestroy();
    iTestCases.Close();
    }

/*
-------------------------------------------------------------------------------

    Class: CTestCasesList

    Method: AppendTestCaseL

    Description: Appends test case.

    Parameters: const TDesC& aTestCaseTitle  in: Test case title

    Return Values: 

    Errors/Exceptions: Leaves if memory allocation fails

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestCasesList::AppendTestCaseL( const TDesC& aTestCaseTitle )
    {
    HBufC* testCaseTitle = aTestCaseTitle.AllocL();
    CleanupStack::PushL( testCaseTitle );
    iTestCases.AppendL( testCaseTitle );
    CleanupStack::Pop( testCaseTitle );
    }

/*
-------------------------------------------------------------------------------

    Class: CTestCasesList

    Method: GetTestCaseTitleL

    Description: Returns specified test case title  

    Parameters: TInt aIndex: in: Requested test case index. 

    Return Values: Test case title.

    Errors/Exceptions: Leaves if test case index is invalid

    Status: Approved

-------------------------------------------------------------------------------
*/
const TDesC& CTestCasesList::GetTestCaseTitleL( TInt aIndex ) const
    {    
    if ( ( aIndex < 0 ) || ( aIndex >= iTestCases.Count() ) )
        {
        User::Leave( KErrNotFound );
        }
    return *iTestCases[ aIndex ];
    }

/*
-------------------------------------------------------------------------------

    Class: CTestCasesList

    Method: GetConfigFileName

    Description: Returns config file name  

    Parameters: 

    Return Values: Config file name.

    Errors/Exceptions: 

    Status: Approved

-------------------------------------------------------------------------------
*/
const TDesC& CTestCasesList::GetConfigFileName() const
    {    
    return *iConfigFileName;
    }

/*
-------------------------------------------------------------------------------

    Class: CTestCasesList

    Method: Count

    Description: Returns count of test cases.    

    Parameters: 

    Return Values: Test cases count.

    Errors/Exceptions: 

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CTestCasesList::Count() const
    {
    return iTestCases.Count();
    }

/*
-------------------------------------------------------------------------------

    Class: CTestCasesList

    Method: ResetAndDestroy

    Description: Resets list of test cases.    

    Parameters: 

    Return Values: 

    Errors/Exceptions: 

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestCasesList::ResetAndDestroy()
    {    
    iTestCases.ResetAndDestroy();
    }

/*
-------------------------------------------------------------------------------

    Class: CTestCasesList

    Method: ~CTestCasesList

    Description: Destructor    

    Parameters: 

    Return Values: 

    Errors/Exceptions: 

    Status: Approved

-------------------------------------------------------------------------------
*/
CTestCasesList::CTestCasesList()
    {
    }

/*
-------------------------------------------------------------------------------

    Class: CTestCasesList

    Method: ~CTestCasesList

    Description: Destructor    

    Parameters: 

    Return Values: 

    Errors/Exceptions: 

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestCasesList::ConstructL( const TDesC& aConfigFileName )
    {
    iConfigFileName = aConfigFileName.AllocL();
    }

/*
-------------------------------------------------------------------------------

    Class: CTestModule

    Method: NewL

    Description: Create new Test Module    

    Parameters: RThread& aClient              :in:  Handle to client
                CTestServer* aServer          :in:  Pointer to server

    Return Values: CTestModule*                     Pointer to new test module

    Errors/Exceptions: Leaves if memory allocation fails or ConstructL leaves.

    Status: Approved

-------------------------------------------------------------------------------
*/
CTestModule* CTestModule::NewL( CTestServer* aServer )
    {

    CTestModule* self=new( ELeave ) CTestModule();
    CleanupStack::PushL( self );
    self->ConstructL( aServer );
    CleanupStack::Pop();
    return self;
    }

/*
-------------------------------------------------------------------------------

    Class: CTestModule

    Method: CTestModule

    Description: Constructor.
    Initialise base class.

    Parameters: RThread& aClient              :in:  Handle to client

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CTestModule::CTestModule() :
    CSession2(),
    iIni(0, 0)
    {
    }

/*
-------------------------------------------------------------------------------

    Class: CTestModule

    Method: ~CTestModule

    Description: Destructor.
    Deallocate memory and close handles.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None.

    Status: Proposal

-------------------------------------------------------------------------------
*/
CTestModule::~CTestModule()
    {

    __TRACE( KVerbose, ( _L( "Closing test module" ) ) );
        
    // Free test case related data. No error checking here, because if freeing
    // fails, then nothing can be done in destructor.
    FreeCaseData();

    // Delete all test module instances.
    // Cleanup all RTestExecution has been opened by client but not properly
    // closed for example if timeout occurs and client died. This almost do
    // same than CloseSession() in "if( iTestExecutionHandle )" branch.
    if( iTestExecutionHandle )
        {
        TInt handle = 0;
        CObject* theObj = NULL;
        TInt count = iTestExecutionHandle->Count();

        for( TInt i = 0 ; i < count; i++ )
            {
            // Get pointer to CTestExecution
            theObj=iTestExecutionHandle->operator[]( i );
            if( theObj )
                {
                handle=iTestExecutionHandle->At( theObj );

                CTestExecution* testcase = (CTestExecution*) theObj;
                // Cancels test(s) execution(timeout, exit, etc)
                // For example TestCombiner is timeouted => TestCombiner's
                // TestServer+TestModule(s) should cancel also.
                testcase->CancelTestExecution();
                // CTestExecution will panic if test case is ongoing!!! This
                // should be stopped, client should handless this.
                iTestExecutionHandle->Remove( handle );
                }
            }

        delete iTestExecutionHandle;
        iTestExecutionHandle = NULL;

        }

    // Delete all FREE Test Module instances.
    if( iTestModuleInstances )
        {
        iTestModuleInstances->ResetAndDestroy();
        delete iTestModuleInstances;
        iTestModuleInstances = NULL;
        }

    // Delete ini file heap buffer
    delete iIniBuffer;
    iIniBuffer = NULL;

    // Delete array of test case titles
    iTestCaseTitlesMap.ResetAndDestroy();
    iTestCaseTitlesMap.Close();
    }

/*
-------------------------------------------------------------------------------

    Class: CTestModule

    Method: ConstructL

    Description: Second level constructor.
    
    Parameters: CTestServer* aServer          :in:  Server

    Return Values: None

    Errors/Exceptions: Leaves if:
                       base class CreateL leaves
                       Object index creation fails
                       Object container creation fails
                       Pointer array construction fails                       

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestModule::ConstructL( CTestServer* aServer )
    {

	__TRACE( KVerbose, ( _L( "CTestModule::ConstructL - constructing server session" ) ) );

    // second-phase construct base class
    //CSession2::CreateL( /* *aServer */ );
    iTestServer = aServer;

    // create new object index
    iTestExecutionHandle = CObjectIx::NewL();

    // Initialize the object container using the object
    // container index in the server.
    iContainer = iTestServer->NewContainerL();

    iTestModuleInstances = new( ELeave ) RPointerArray<CTestModuleContainer>;

	__TRACE( KError, ( _L( "CTestModule::ConstructL - constructing server session done" ) ) );

    }


/*
-------------------------------------------------------------------------------

    Class: CTestModule

    Method: CountResources

    Description: Resource counting

    Parameters: None

    Return Values: TInt                             Resource count

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CTestModule::CountResources()
    {

    return iResourceCount;

    }


/*
-------------------------------------------------------------------------------

    Class: CTestModule

    Method: NumResources

    Description: Get resources, writes to Message()

    Parameters: None

    Return Values: None

    Errors/Exceptions: Panic client if result can't be written to descriptor.

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestModule::NumResources( const RMessage2& aMessage )
    {

    TPckgBuf<TInt> countPckg( iResourceCount );

    TRAPD( r, aMessage.WriteL( 0, countPckg ) );
    if( r !=KErrNone )
        {
        PanicClient( EBadDescriptor, aMessage );
        }

    }

/*
-------------------------------------------------------------------------------

    Class: CTestModule

    Method: PanicClient

    Description: Panic clients.

    Parameters: TInt aPanic                   :in:  Panic code

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestModule::PanicClient( const TInt aPanic, const RMessage2& aMessage ) const
    {

    __TRACE( KError,( _L( "CTestModule::PanicClient code = %d" ), aPanic ) );

    _LIT( KTxtTestModule,"CTestModule" );

    aMessage.Panic( KTxtTestModule, aPanic );
    }


/*
-------------------------------------------------------------------------------

    Class: CTestModule

    Method: CloseSession

    Description: Close session

    Parameters: None

    Return Values: TInt: Symbian error code.

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CTestModule::CloseSession( const RMessage2& aMessage )
    {

    __TRACE( KMessage,( _L( "CTestModule::CloseSession in" ) ) );

    // Delete all unclosed subsession handle before closing session.
    // Remove iTestExecutionHandle contents. iTestExecutionHandle countains
    // CTestExecution that handles test case have been created.
    if( iTestExecutionHandle )
        {
        TInt handle = 0;
        CObject* theObj = NULL;
        TInt count = iTestExecutionHandle->Count();        
        for ( TInt i = count - 1; i >= 0; i-- )
            {
            theObj=iTestExecutionHandle->operator[]( i );
            if( theObj )
                {
                handle=iTestExecutionHandle->At( theObj );
                // CTestExecution will panic if test case is ongoing!!! This
                // should be stopped, client should handle this.
                iTestExecutionHandle->Remove( handle );
                }
            }

        delete iTestExecutionHandle;
        iTestExecutionHandle = NULL;
        }

    // Deletion must be done here, because the "CloseSession" message is 
    // completed before execution continues from CActiveScheduler::Start 
    // location, and the main thread can continue execution 
    // and therefore shutdown itself and all threads in that process.
    
    // Delete the object container
    iTestServer->DeleteContainer( iContainer );

    // Free test case related data
    TInt r = FreeCaseData();

    // Delete all FREE Test Module instances
    iTestModuleInstances->ResetAndDestroy();
    delete iTestModuleInstances;
    iTestModuleInstances = NULL;

     // Inform server that session is closed
    iTestServer->SessionClosed();

	// Check for avoiding multiple complete, see CTestModule::ServiceL()
    if( r != KErrNone )
        {
		// In error cases do not complete, just return
        __TRACE( KMessage,( _L( "CTestModule::CloseSession out (1)" ) ) );
        return r;
        }

    aMessage.Complete( KErrNone );

    // After active scheduler shutdown is done, execution continues from
    // CTestServer::ThreadFunction()

    __TRACE( KMessage,( _L( "CTestModule::CloseSession out (2)" ) ) );
    return r;
    }


/*
-------------------------------------------------------------------------------

    Class: CTestModule

    Method: ServiceL

    Description: Trap harness for dispatcher

    Parameters: const RMessage& aMessage  :inout:   Message

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CTestModule::ServiceL( const RMessage2& aMessage )
    {

    // NOTE! HW testing slows down dramatically if adds commants here

    //__TRACE( KMessage,( _L( "CTestModule::ServiceL in" ) ) );

    TInt r = KErrNone;
    TRAPD( ret, r = DispatchMessageL( aMessage ) );

    if ( ret != KErrNone )
        {
        // Complete message on leaving cases with leave code.
        __TRACE( KError, ( CStifLogger::ERed, _L( "CTestModule::DispatchMessageL leaved" ) ) );
        aMessage.Complete( ret );
        }

    // Complete message with error code originating from message handling
    // function.
    if ( r != KErrNone )
        {
        __TRACE( KError, ( CStifLogger::ERed, _L( "CTestModule::DispatchMessageL returned error" ) ) );
        aMessage.Complete( r );
        }

     // __TRACE( KMessage,( _L( "CTestModule::ServiceL out" ) ) );

    }


/*
-------------------------------------------------------------------------------

    Class: CTestModule

    Method: DispatchMessageL

    Description: Dispatch message, calls corresponding function to do it.

    Parameters: const RMessage& aMessage  :inout:   Message to be handled

    Return Values: TInt                             Error code

    Errors/Exceptions: Leaves if operation handling function leaves

    Status: Proposal

-------------------------------------------------------------------------------
*/
TInt CTestModule::DispatchMessageL( const RMessage2& aMessage )
    {

    __TRACE( KMessage,( _L( "CTestModule::DispatchMessageL %d" ),
        aMessage.Function()  ) );
    switch( aMessage.Function() )
    {
    // Session specific
    case ETestServerCloseSession:                // Close whole session
        {
		__TRACE( KError ,( _L( "Closing test module session" ) ) );
        return CloseSession( aMessage );
        }
    case ETestServerGetServerThreadId:           // Get Server ThreadId
        {
		__TRACE( KError ,( _L( "Return server thread id" ) ) );
        return GetServerThreadIdL( aMessage );
        }

    case ETestModuleCreateSubSession :           // Create new test module subsession
        {
		__TRACE( KError,( _L( "Creating test module session" ) ) );
        return CreateModuleSessionL( aMessage );
        }

    case ETestModuleCloseSubSession:             // Close module subsession
        {
        // Nothing to do.
        aMessage.Complete( KErrNone );
        return KErrNone;
        }

    case ETestExecutionCreateSubSession:         // Create new test execution subsession
        {
        return NewTestExecutionL( aMessage );
        }
 
    case ETestModuleEnumerateTestCases:          // Enumerate test cases
        {
		__TRACE( KError,( _L( "Enumerating test cases" ) ) );
        return EnumerateTestCasesL( aMessage );
        }
        
    case ETestModuleGetTestCases:                // Get test cases
        {
        return GetTestCasesL( aMessage );
        }

    case ETestModuleErrorNotification:           // Request error notification
        {
        return HandleErrorNotificationL( aMessage );
        }

    case ETestModuleCancelAsyncRequest:
        {
        return CancelAsyncRequestL( aMessage );
        }
  
    // Subsession specific
    case ETestExecutionRunTestCase:              // Run test case
        {
		__TRACE( KInit,( _L( "Running test case" ) ) );
        CTestExecution* testCase=CaseFromHandle( aMessage.Int3(), aMessage );
        return testCase->RunTestCase( aMessage );
        }
        
    case ETestExecutionNotifyProgress:           // Test case prints
        {
        CTestExecution* testCase=CaseFromHandle( aMessage.Int3(), aMessage );
        return testCase->NotifyPrint( aMessage );
        }

    case ETestExecutionNotifyEvent:              // Event notifications
        {
        CTestExecution* testCase=CaseFromHandle( aMessage.Int3(), aMessage );
        return testCase->NotifyEvent( aMessage );
        }
        
    case ETestExecutionNotifyRemoteCmd:          // RemoteCmd notifications
        {
        CTestExecution* testCase=CaseFromHandle( aMessage.Int3(), aMessage );
        return testCase->NotifyRemoteCmd( aMessage );
        }
    case ETestExecutionReadRemoteCmdInfo:          // RemoteCmd reading
        {
        //CTestExecution* testCase=CaseFromHandle( aMessage.Int3() );
        //return testCase->NotifyRemoteCmd( aMessage );
        CTestExecution* testCase=CaseFromHandle( aMessage.Int3(), aMessage );
        return testCase->ReadRemoteCmdInfo( aMessage );
        }


    case ETestExecutionResume:                   // Resume case execution
        {
		__TRACE( KVerbose,( _L( "Resuming test execution" ) ) );
        CTestExecution* testCase=CaseFromHandle( aMessage.Int3(), aMessage );
        return testCase->Resume( aMessage );
        }
        
    case ETestExecutionPause:                    // Pause case execution
        {
		__TRACE( KVerbose,( _L( "Pausing test execution" ) ) );
        CTestExecution* testCase=CaseFromHandle( aMessage.Int3(), aMessage );
        return testCase->Pause( aMessage );
        }
            
    case ETestExecutionCloseSubSession:          // Close execution subsession
        {
        CTestExecution* testCase=CaseFromHandle( aMessage.Int3(), aMessage );
        return testCase->CloseTestExecution( aMessage );
        }

    case ETestExecutionCancelAsyncRequest:       // Cancel async request
        {
        CTestExecution* testCase=CaseFromHandle( aMessage.Int3(), aMessage );
        return testCase->CancelRequestL( aMessage );
        }
    case ETestExecutionNotifyCommand:            // Command notifications
        {
        CTestExecution* testCase=CaseFromHandle( aMessage.Int3(), aMessage );
        return testCase->NotifyCommand( aMessage );
        }

    default:                                     // Invalid request
        {
        PanicClient( EBadRequest, aMessage );
        return KErrNotSupported;
        }
    }

    }


/*
-------------------------------------------------------------------------------

    Class: CTestModule

    Method: EnumerateTestCases

    Description: Enumerates test cases and returns test case count by writing
    result to client side package.

    Function obtains a TestModule and calls EnumerateTestCases from it.
    Test module will be released when calling GetTestCases function.

    Parameters: const RMessage& aMessage  :inout:   Message to be handled

    Return Values: TInt                             Operation result

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
TInt CTestModule::EnumerateTestCasesL( const RMessage2& aMessage )
    {

    __TRACE( KMessage,( _L( "CTestModule::EnumerateTestCasesL in" ) ) );
 
    // Get data from message
    TFileName config;
    TRAPD( res, aMessage.ReadL( 0, config ) );
    if(res == KErrDied)
        {
        RDebug::Print(_L("CTestModule::EnumerateTestCasesL() Reading from RMessage ended with KErrDied. Client is not alive anymore and this request will be ignored"));
        return res;
        }
    else if( res != KErrNone )
        {
        RDebug::Print(_L("CTestModule::EnumerateTestCasesL() #1 Panic client with [%d], res=[%d]"), EBadDescriptor, res);
        PanicClient( EBadDescriptor, aMessage );
        return res;
        }

    // Free old case data
    User::LeaveIfError( FreeCaseData() );

    // Get test cases from the test module
    User::LeaveIfError( GetTestModule( iEnumerateModule, config ) );

    // Enumerate test cases
    iEnumerateModule->EnumerateTestCases( config );

    // Something fatal?
    TInt r = iEnumerateModule->OperationErrorResult();
    if(  r != KErrNone )
        {
        // Enumeration module have crashed.
        delete iEnumerateModule;
        iEnumerateModule = NULL;

        return r;
        }

    // Error from module?
    if( iEnumerateModule->ModuleResult() != KErrNone )
        {
        return iEnumerateModule->ModuleResult();
        }

    // Write count to Ptr1()
    const RPointerArray<TTestCaseInfo>* testCases = iEnumerateModule->TestCases();
    
    if( testCases == NULL )
        {
        User::Leave( KErrGeneral );
        }
    
    CTestCasesList* testCasesList = NULL;
    for ( TInt i = 0; i < iTestCaseTitlesMap.Count(); i++ )
        {
        if ( iTestCaseTitlesMap[ i ]->GetConfigFileName() == config )
            {
            testCasesList = iTestCaseTitlesMap[ i ];
            break;
            }
        }
    if ( testCasesList == NULL )
        {
        testCasesList = CTestCasesList::NewL( config );
        CleanupStack::PushL( testCasesList );
        iTestCaseTitlesMap.AppendL( testCasesList );
        CleanupStack::Pop( testCasesList );
        }
    
    // Store titles (for further use, i.e. when asked for title from the interface via CTestModuleIf->CTestThreadContainer->CTestModuleContainer)
    testCasesList->ResetAndDestroy();
    TInt i;
    for(i = 0; i < testCases->Count(); i++)
        {
        //Handle situation when test cases are enumerated not as 0-based (testscripter, ...)
        if(i == 0 && (*testCases)[i]->iCaseNumber > 0)
            {
            testCasesList->AppendTestCaseL( KNullDesC );
            }
        testCasesList->AppendTestCaseL( (*testCases)[i]->iTitle );
        }
    
    TPckgBuf<TInt> countPckg( testCases->Count() );
    TRAP( res, aMessage.WriteL( 1, countPckg ) );
    if(res == KErrDied)
        {
        RDebug::Print(_L("CTestModule::EnumerateTestCasesL() Writing to RMessage ended with KErrDied. Client is not alive anymore and this request will be ignored"));
        return res;
        }
    else if( res != KErrNone )
        {
        RDebug::Print(_L("CTestModule::EnumerateTestCasesL() #2 Panic client with [%d], res=[%d], config=[%S]"), EBadDescriptor, res, &config);
        PanicClient( EBadDescriptor, aMessage );
        return res;
        }

    // All ok, complete message
    aMessage.Complete( KErrNone );

    __TRACE( KMessage,( _L( "CTestModule::EnumerateTestCasesL out" ) ) );
    
    return KErrNone;

    }


/*
-------------------------------------------------------------------------------

    Class: CTestModule

    Method: GetTestCasesL

    Description: Get test cases. Enumerate test cases must be called before
    calling this function.

    Function releases the test module reserved by EnumerateTestCase().

    Parameters: const RMessage& aMessage  :inout:   Message to be handled

    Return Values: TInt                             Operation result

    Errors/Exceptions: Leaves if cases have not been enumerated.

    Status: Proposal

-------------------------------------------------------------------------------
*/
TInt CTestModule::GetTestCasesL( const RMessage2& aMessage )
    {

    __TRACE( KMessage,( _L( "CTestModule::GetTestCasesL in" ) ) );

    // Leave if cases have not been enumerated.
    //User::LeaveIfNull(( TAny* ) iEnumerateModule->TestCases() );
    if( ( TAny* ) iEnumerateModule->TestCases() == NULL )
        {
        User::Leave( KErrGeneral );
        }

    const TInt len = sizeof( TTestCaseInfo );
    
    // Get cases
    const RPointerArray<TTestCaseInfo>& cases = *iEnumerateModule->TestCases();

    // Get number of cases
    const TInt Kcount = cases.Count();

    // Loop through case and copy then to client's descriptor.
    for( TInt i = 0; i < Kcount; i++ )
        {

        // Construct package for source data
        TTestCaseInfoPckg tmpPackage( *cases[i] );

        // Write to correct location
        aMessage.WriteL( 0, tmpPackage, i *len );

        }

    // Free case data and the test module
    User::LeaveIfError( FreeCaseData() );

    // Finished
    aMessage.Complete( KErrNone );

    __TRACE( KMessage,( _L( "CTestModule::GetTestCasesL out" ) ) );

    return KErrNone;

    }


/*
-------------------------------------------------------------------------------

    Class: CTestModule

    Method: HandleErrorNotification

    Description: Request error notification.

    Parameters: const RMessage& aMessage  :inout:   Message to be handled

    Return Values: TInt                             Operation result

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
TInt CTestModule::HandleErrorNotificationL( const RMessage2& aMessage )
    {
    
    iErrorMessage = aMessage;
    iErrorMessageAvailable = ETrue;

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestModule

    Method: GetServerThreadId

    Description: Request server state notification.

    Parameters: const RMessage& aMessage :inout: Message to be handled

    Return Values: TInt Operation result

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
TInt CTestModule::GetServerThreadIdL( const RMessage2& aMessage )
    {
      
    TInt id( iTestServer->GetServerThreadId() );
   
    TPckg<TThreadId> threadIdPckg( id );
   
    TRAPD( res, aMessage.WriteL( 0, threadIdPckg ) );
    
      // Finished
    aMessage.Complete( res );
    
    return KErrNone;

    }      

/*
-------------------------------------------------------------------------------

    Class: CTestModule

    Method: CancelAsyncRequest

    Description: Cancels asynchronous request

    Parameters: const RMessage& aMessage  :inout:   Message to be handled

    Return Values: TInt                             Operation result

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
TInt CTestModule::CancelAsyncRequestL( const RMessage2& aMessage )
    {
    
    switch ( aMessage.Int0() )
        {
        case ETestModuleErrorNotification:
            {
            if ( iErrorMessageAvailable )
                {
                iErrorMessage.Complete ( KErrCancel );
                iErrorMessageAvailable = EFalse;
                }
            aMessage.Complete ( KErrNone );
            break;
            }

        default:
            {
            PanicClient( EInvalidRequestCancel, aMessage );
            break;
            }
        }

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestModule

    Method: ErrorPrint

    Description: Prints error

    Parameters: const TInt aPriority :in: Priority
                TPtrC aError: in: Error

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CTestModule::ErrorPrint( const TInt aPriority, 
                              TPtrC aError )
    {

    if ( iErrorMessageAvailable )
        {        
        TErrorNotification error;
        TErrorNotificationPckg errorPckg ( error );

        error.iModule = _L("TestServer");
        error.iPriority = aPriority;
        error.iText = aError;

        TRAPD( r, iErrorMessage.WriteL( 0, errorPckg ) );
        
        // Do not handle errors
        iErrorMessageAvailable = EFalse;
        iErrorMessage.Complete( r );

        }
    else
        {
        RDebug::Print (_L("Error message lost %d [%S]"), aPriority, &aError );
        }
    
    }

/*
-------------------------------------------------------------------------------

    Class: CTestModule

    Method: FreeCaseData

    Description: Frees the test case data and test module that is used in
    enumeration.

    Parameters: None

    Return Values: TInt                             Error code

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
TInt CTestModule::FreeCaseData()
    {

    __TRACE( KMessage,( _L( "CTestModule::FreeCaseData in" ) ) );
 
    TInt r = KErrNone;

    if( iEnumerateModule )
        {
        
        // Deallocate testcase memory
        iEnumerateModule->FreeEnumerationData();

        // Free the module
        r = FreeTestModule( iEnumerateModule );

        iEnumerateModule = NULL;

        }

    __TRACE( KMessage,( _L( "CTestModule::FreeCaseData out" ) ) );

    return r;

    }


/*
-------------------------------------------------------------------------------

    Class: CTestModule

    Method: CreateModuleSessionL

    Description: Creates a new module session.
    Just take the initialisation file name from message.

    Parameters: const RMessage& aMessage  :inout:   Message to be handled

    Return Values: TInt                             Operation result

    Errors/Exceptions: Leaves if memory allocation fails

    Status: Proposal

-------------------------------------------------------------------------------
*/
TInt CTestModule::CreateModuleSessionL( const RMessage2& aMessage )
    {

    __TRACE( KMessage,( _L( "CTestModule::CreateModuleSession in" ) ) );

    // Take parameters
    TFileName ini;
    TRAPD( res, aMessage.ReadL( 0, ini ) );
    if( res != KErrNone )
        {
        PanicClient( EBadDescriptor, aMessage );
        return res;
        }

    // Construct heap buffer for initialization file name
    iIniBuffer = HBufC::NewL( ini.Length() );
    iIni.Set ( iIniBuffer->Des() );
    iIni.Copy ( ini );

    aMessage.Complete( KErrNone );

    __TRACE( KMessage,( _L( "CTestModule::CreateModuleSession out" ) ) );

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestModule

    Method: NewTestExecutionL

    Description: Create new test execution subsession
    
    Parameters: const RMessage& aMessage  :inout:   Message to be handled
    
    Return Values: TInt                             Operation result

    Errors/Exceptions: Function leaves if object can't be created or
                       it can't be added to container.
                       Function panics client if message contains invalid
                       descriptor.

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
TInt CTestModule::NewTestExecutionL( const RMessage2& aMessage )
    {

    __TRACE( KMessage,( _L( "CTestModule::NewTestExecutionL in" ) ) );

    // Get data from message
    TInt caseNumber = aMessage.Int0();
    TFileName config;
    
    TRAPD( res, aMessage.ReadL( 1, config ) );
    if( res != KErrNone )
        {
        PanicClient( EBadDescriptor, aMessage );
        return res;
        }

    // Make new object
    CTestExecution* execution=CTestExecution::NewL( this, caseNumber, config );

    // add object to object container to generate unique id
    iContainer->AddL( execution );

    // add object to object index; this returns a unique handle so we can get it again
    TInt handle=iTestExecutionHandle->AddL( execution );

    // write the handle to client
    TPckg<TInt> handlePckg( handle );
    TRAP( res, aMessage.WriteL( 3, handlePckg ) );
    if( res != KErrNone )
        {
        iTestExecutionHandle->Remove( handle );
        PanicClient( EBadDescriptor, aMessage );
        return res;
        }

    // notch up another resource
    iResourceCount++;

    // Complete message
    aMessage.Complete( KErrNone );

    __TRACE( KMessage,( _L( "CTestModule::NewTestExecutionL out" ) ) );

    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CTestModule

    Method: DeleteTestExecution

    Description: Deletes Test Execution by handle

    Parameters: const TUint aHandle           :in:  Handle

    Return Values: None

    Errors/Exceptions: Panics client if invalid handle

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CTestModule::DeleteTestExecution( const TUint aHandle, const RMessage2& aMessage )
    {

    // Verify that handle is valid
    CaseFromHandle( aHandle, aMessage );

    // Remove object
    iTestExecutionHandle->Remove( aHandle );

    // Decrement resource count.
    iResourceCount--;

    }


/*
-------------------------------------------------------------------------------

    Class: CTestModule

    Method: CaseFromHandle

    Description: Return subsession from handle

    Parameters: const TUint aHandle           :in:  Handle

    Return Values: CTestExecution*                  Test Execution object

    Errors/Exceptions: Function panics client if invalid handle.

    Status: Approved
    
-------------------------------------------------------------------------------
*/
CTestExecution* CTestModule::CaseFromHandle( const TUint aHandle, const RMessage2& aMessage ) const
    {

    CTestExecution* testcase =( CTestExecution* ) iTestExecutionHandle->At( aHandle );

    if( testcase == NULL )
        {
        PanicClient( EBadSubsessionHandle, aMessage );
        }

    return testcase;

    }


/*
-------------------------------------------------------------------------------

    Class: CTestModule

    Method: GetTestModule

    Description: Gets a CTestModuleBase*. If there is a free entry
    in the free list, then it is returned. Otherwise a new one is created.

    Parameters: CTestModuleContainer*& aContainer: out: container pointer.
                const TDesC& aConfig: in: Test case (config) file name.

    Return Values: Symbian OS error code
    
    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
TInt CTestModule::GetTestModule( CTestModuleContainer*& aContainer,
                                    const TDesC& aConfig )
    {
    TInt ret = KErrNone;

    if( iTestModuleInstances->Count() )
        {
        // Return an existing one        
        aContainer =( *iTestModuleInstances )[0];
        iTestModuleInstances->Remove( 0 );
		__TRACE( KInit,( _L( "Reusing old test module container instance at 0x%x" ), (TUint32) aContainer ) );
        }
    else
        {
        // Create a new one    
        __TRACE( KInit,( _L( "Creating new test module instance" ) ) );
        TRAPD( err, aContainer = CTestModuleContainer::NewL( 
                                                iTestServer->ModuleName(),
                                                this,
                                                aConfig ) );

        // If module can't be created, then return NULL.
        if( err )
            {
            __TRACE( KError,( _L( "Can't create new test module container instance" ) ) );
            aContainer = NULL;
            ret = err;
            }
		else
			{
			__TRACE( KInit,( _L( "Test module instance container created" ) ) );
			}

        if( aContainer )
            {

            // Initialise module
            aContainer->Initialize( iTestServer->ModuleName(), iTestServer->FirstTime() );

            if( aContainer->OperationErrorResult() != KErrNone )
                {
                // Can't initialise module, delete it
                __TRACE( KError,( CStifLogger::ERed, _L( "Operation error, can't initialize test module container instance" ) ) );
                ret = aContainer->OperationErrorResult();                    
                delete aContainer;
                aContainer = NULL;
                }
            else if( aContainer->ModuleResult() != KErrNone )
                {
                // Can't initialise module, delete it
                __TRACE( KError,( CStifLogger::ERed, _L( "Module error, can't initialize test module container instance" ) ) );
                ret = aContainer->ModuleResult();
                delete aContainer;
                aContainer = NULL;
                }
            else
                {
                // Module initialised properly, clear the first time flag.
                iTestServer->ClearFirstTime();
				__TRACE( KInit,( _L( "Test module container initialized at 0x%x" ), (TUint32) aContainer ) );
                }
            }
        }

    return ret;

    }


/*
-------------------------------------------------------------------------------

    Class: CTestModule

    Method: FreeTestModule

    Description:Frees a CTestModuleContainer. This function can be called
    from the context of the test execution thread.
    
    Parameters: CTestModuleContainer* aModule :in:  Module to be freed
    
    Return Values: TInt                             Error code

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
TInt CTestModule::FreeTestModule( CTestModuleContainer* aModule )
    {
  
    // Free the module
    TInt r = iTestModuleInstances->Append( aModule );
    if( r != KErrNone )
        {
        delete aModule;
        aModule = NULL;
        }

    return r;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestModule

    Method: IniName

    Returns the initialisation file name
    
    Parameters: None
    
    Return Values: const TDesC&                     Initialisation file name

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/

const TDesC& CTestModule::IniName() const
    {

    return iIni;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestModule

    Method: Name

    Returns the module name
    
    Parameters: None
    
    Return Values: const TDesC&                     Initialisation file name

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
const TDesC& CTestModule::Name() const
    {

    return iTestServer->ModuleName();

    }

/*
-------------------------------------------------------------------------------

    Class: CTestModule

    Method: GetTestCaseTitleL

    Gets title of currently running test case
    
    Parameters: None
    
    Return Values: TInt aTestCaseNumber: in: index of currently running test case
                   TDes& aTestCaseTitle: out: test case title

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
void CTestModule::GetTestCaseTitleL(TInt aTestCaseNumber, const TDesC& aConfigFile,TDes& aTestCaseTitle)
    {
    CTestCasesList* testCasesList = NULL;
    for ( TInt i = 0; i < iTestCaseTitlesMap.Count(); i++ )
        {
        if ( iTestCaseTitlesMap[ i ]->GetConfigFileName() == aConfigFile )
            {
            testCasesList = iTestCaseTitlesMap[ i ];
            break;
            }
        }
    if ( testCasesList == NULL )
        {
        User::Leave( KErrNotFound );
        }
    
    RDebug::Print(_L("Trying to get test case title from module. Index=%d, count=%d"), aTestCaseNumber, testCasesList->Count() );
    
    aTestCaseTitle.Copy( testCasesList->GetTestCaseTitleL( aTestCaseNumber ) );
    }


/*
-------------------------------------------------------------------------------

    Class: CTestModule

    Method: GetTestServer

    Gets pointer to TestServer
    
    Parameters: None
    
    Return Values: CTestServer : pointer to TestServer

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
CTestServer* CTestModule::GetTestServer()
    {
    return iTestServer;
    }

// End of File
