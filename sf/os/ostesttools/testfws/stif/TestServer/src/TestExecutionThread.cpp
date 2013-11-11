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
* CTestThreadContainer class implementation. These functions are 
* called from the context of the test execution thread.
*
*/

// INCLUDE FILES
#include <e32std.h>
#include <e32svr.h>
#include <e32uid.h>
#include <StifTestModule.h>
#include "ThreadLogging.h"
#include "TestEngineClient.h"
#include <stifinternal/TestServerClient.h>
#include "TestServer.h"
#include "TestThreadContainer.h"
#include "TestServerCommon.h"
#include "TestServerModuleIf.h"
#include "TestServerEvent.h"
#include "TestThreadContainerRunner.h"
#include <stifinternal/TestThreadContainerRunnerFactory.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS
#ifdef THREADLOGGER
#undef THREADLOGGER
#endif
#define THREADLOGGER iThreadLogger 

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ==================== LOCAL FUNCTIONS =======================================

// None

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

    Class: CTestThreadContainer

    Method: NewL

    Description: Returns new CTestThreadContainer instance.

    Parameters: None

    Return Values: CTestThreadContainer*            New instance

    Errors/Exceptions: Function leaves if memory allocation fails or
                       CTestThreadContainer ConstructL leaves.

    Status: Proposal

-------------------------------------------------------------------------------
*/
CTestThreadContainer* CTestThreadContainer::NewL( 
    CTestModuleContainer* aModuleContainer,
    TThreadId aServerThreadId )
    {

    CTestThreadContainer* self = 
        new ( ELeave ) CTestThreadContainer( aModuleContainer );
    CleanupStack::PushL( self );
    self->ConstructL( aServerThreadId );
    CleanupStack::Pop( self );
    return self;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestThreadContainer

    Method: ConstructL

    Description: Second level constructor.

    Parameters: None

    Return Values: CTestThreadContainer*            New instance

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
void CTestThreadContainer::ConstructL( TThreadId aServerThreadId )
    {
    
    User::LeaveIfError( iServerThread.Open( aServerThreadId ) );
    
    iErrorPrintSem.SetHandle( ModuleContainer().ErrorPrintSemHandle() );
    User::LeaveIfError( iErrorPrintSem.Duplicate( iServerThread ) );
    
    }

/*
-------------------------------------------------------------------------------

    Class: CTestThreadContainer

    Method: CTestThreadContainer

    Description: Constructor.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
CTestThreadContainer::CTestThreadContainer( 
    CTestModuleContainer* aModuleContainer ):
    iModuleContainer( aModuleContainer ),
    iCheckResourceFlags( 0 )
    {
   
    ModuleContainer().SetThreadContainer( this );
    
    StifMacroErrorInit(); // Initialization

    }

/*
-------------------------------------------------------------------------------

    Class: CTestThreadContainer

    Method: ~CTestThreadContainer

    Description: Destructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
CTestThreadContainer::~CTestThreadContainer()
    {

    // Close mutexes
    if ( iPrintMutex.Handle() != 0 ) iPrintMutex.Close();
    if ( iEventMutex.Handle() != 0 ) iEventMutex.Close();
    if ( iSndMutex.Handle() != 0 ) iSndMutex.Close();
    if ( iRcvMutex.Handle() != 0 ) iRcvMutex.Close();
    if ( iInterferenceMutex.Handle() != 0 ) iInterferenceMutex.Close();
    if ( iMeasurementMutex.Handle() != 0 ) iMeasurementMutex.Close();
    if ( iCommandMutex.Handle() != 0 ) iCommandMutex.Close();

    // Mutex for testcomplete and cancel operations. Close duplicate mutex
    if ( iTestThreadMutex.Handle() != 0 ) iTestThreadMutex.Close();

    // Close semaphores
    if ( iPrintSem.Handle() != 0 ) iPrintSem.Close();
    if ( iErrorPrintSem.Handle() != 0 ) iErrorPrintSem.Close();
    if ( iEventSem.Handle() != 0 ) iEventSem.Close();
    if ( iSndSem.Handle() != 0 ) iSndSem.Close();
    if ( iRcvSem.Handle() != 0 ) iRcvSem.Close();
    if ( iInterferenceSem.Handle() != 0 ) iInterferenceSem.Close();
    if ( iMeasurementSem.Handle() != 0 ) iMeasurementSem.Close();
    //if ( iReceiverSem.Handle() != 0 ) iReceiverSem.Close();
    if ( iCommandSem.Handle() != 0) iCommandSem.Close();
    
    iServerThread.Close();
    
    }

/*
-------------------------------------------------------------------------------

    Class: CTestThreadContainer

    Method: InitializeModule

    Description: Initialize test module.

    Function obtains pointer to the first exported function of the test module,
    and calls that function to obtain an instance of CTestModuleBase derived
    object. After that the "Init()"-method is called. If some operation fails,
    module will be deleted and error code is returned.

    This function is a static member function, which is intented to be called
    from the context of the test module thread.

    Parameters: RLibrary& aModule                 :in: Module to be loaded
    
    Return Values: TInt                                Error code from module
                                                       or memory allocation.

    Errors/Exceptions: None.

    Status: Proposal

-------------------------------------------------------------------------------
*/
TInt CTestThreadContainer::InitializeModuleInThread ( RLibrary& aModule )
    {        
        
    __TRACEI ( KInit, ( _L("Starting test module initialization") ) );
    __TRACEI ( KInit, ( CStifLogger::EBold,  _L("Module name \"%S\""), 
        &ModuleContainer().OperationName() ) );
    ModuleContainer().OperationText() = _L("E32DLL");

    TFileName moduleName;
    TFileName tmpBuffer;

    TInt r( KErrNone );
    TFileName newNameBuffer;
    TInt check = CheckModuleName( ModuleContainer().OperationName(), newNameBuffer );
    if( check == KErrNone )
        {
        // Load the module(TestScripter)
        r = aModule.Load( newNameBuffer );
        }
    else
        {
        // Load the module(Others)
        RemoveOptionalIndex(ModuleContainer().OperationName(), newNameBuffer);
        __TRACEI(KInit, (_L( "Valid module name is [%S] (extracted from [%S])"), &newNameBuffer, &ModuleContainer().OperationName()));
        r = aModule.Load(newNameBuffer);
        }
    
    if ( r != KErrNone )
        {
        __TRACEI (KError, ( CStifLogger::EError, _L("Can't initialize test module code = %d"), r));

        // Set error codes
        ModuleContainer().OperationErrorResult() = r;
        return r;
        }
    else
        {
        // Print module name
        moduleName = aModule.FileName();    
        __TRACEI (KInit, (  _L("Loaded test module[%S]"), &moduleName ) );
        }

    
    // Get pointer to first exported function
    ModuleContainer().OperationText() = _L("1st EXPORTED function");
    CTestInterfaceFactory libEntry;
    libEntry = (CTestInterfaceFactory) aModule.Lookup(1);
    if ( libEntry == NULL )
        {
        // New instance can't be created
        __TRACEI (KError, ( CStifLogger::EError, _L("Can't initialize test module, NULL libEntry")));

        // Set error codes
        ModuleContainer().OperationErrorResult() = KErrNoMemory;
        return KErrNoMemory;
        }
    else
        {
        __TRACEI ( KInit, ( _L("Pointer to 1st exported received")));
        }

    // initialize test module
    __TRACEI ( KVerbose, (_L("Calling 1st exported at 0x%x"), (TUint32) libEntry ));
    TRAPD ( err, iTestModule =  (*libEntry)() );

     // Handle leave from test module
    if ( err != KErrNone )
        {
        __TRACEI (KError, ( CStifLogger::EError, _L("Leave when calling 1st exported function, code %d"), err));
        tmpBuffer = _L("Leave from test module 1st EXPORTED function");        
        ErrorPrint( 1, tmpBuffer );        
        delete iTestModule;
        iTestModule = NULL;

        // Set error codes
        ModuleContainer().OperationErrorResult() = err;
        return err;
        }
    else if ( iTestModule == NULL )     // Handle NULL from test module init
        {
        __TRACEI (KError, ( CStifLogger::EError, _L("NULL pointer received when constructing test module")));
        tmpBuffer = _L("Test module 1st EXPORTED function returned NULL");        
        ErrorPrint( 1, tmpBuffer );        
        delete iTestModule;
        iTestModule = NULL;

        // Set error codes
        ModuleContainer().OperationErrorResult() = KErrNoMemory;
        return KErrNoMemory;
        }
    else
        {
        __TRACEI (KInit, (_L("Entrypoint successfully called, test module instance at 0x%x"), (TUint32)iTestModule ) );
        }

    // Verify version number.
    ModuleContainer().OperationText() = _L("Version");
    TVersion moduleAPIVersion(0,0,0);
    TVersion myOldAPIVersion( KOldTestModuleAPIMajor, KOldTestModuleAPIMinor, KOldTestModuleAPIBuild );
    TVersion myAPIVersion( KTestModuleAPIMajor, KTestModuleAPIMinor, KTestModuleAPIBuild );
    TRAP ( err,  moduleAPIVersion = iTestModule->Version() );

    if ( err != KErrNone ||  (( myOldAPIVersion.iMajor !=  moduleAPIVersion.iMajor ||
                                myOldAPIVersion.iMinor !=  moduleAPIVersion.iMinor  )
                                &&
                              ( myAPIVersion.iMajor != moduleAPIVersion.iMajor ||
                              	myAPIVersion.iMinor != moduleAPIVersion.iMinor ))
        )
        {
        tmpBuffer = moduleAPIVersion.Name();
        __TRACEI (KError, ( CStifLogger::EError, _L("Incorrect test module version. Module version %S"), &tmpBuffer ) );        
        tmpBuffer = myOldAPIVersion.Name();
        __TRACEI (KError, ( CStifLogger::EError, _L("Required version %S"), &tmpBuffer  ) );
                
        tmpBuffer.Format(_L("Invalid version in [%S]"), &moduleName );
        ErrorPrint( 1, tmpBuffer );        

         // Set error codes
        ModuleContainer().OperationErrorResult() = KErrNotSupported;
        return KErrNotSupported;
        }

    ModuleContainer().OperationText() = _L("InitL");
    // Initialize test module
    TInt initResult = KErrNone;
    TRAP ( err, 
        CTestModuleIf::NewL( NULL, iTestModule );
        TFileName tmp = ModuleContainer().TestModuleIniFile();
        initResult = iTestModule->InitL( tmp, ModuleContainer().OperationIntBuffer() );
    );

    // Handle leave from test module
    if ( err != KErrNone )
        {
        __TRACEI (KError, ( CStifLogger::EError, _L("Leave when initializing test module code %d"), err));
        tmpBuffer = _L("Leave from test module InitL");        
        ErrorPrint( 1, tmpBuffer );   
        ModuleContainer().OperationText() = _L("DESTRUCTOR");
        delete iTestModule;
        iTestModule = NULL;
        ModuleContainer().OperationText() = _L("");

        // Set error codes
        ModuleContainer().OperationErrorResult() = err;
        return err;
        }      
    else if ( initResult != KErrNone ) 
        {     // Handle failed initialisation of test module
        __TRACEI (KError, ( CStifLogger::EError, _L("Can't initialize test module, code %d"), initResult));
        ModuleContainer().OperationText() = _L("DESTRUCTOR");
        delete iTestModule;
        iTestModule = NULL;
        ModuleContainer().OperationText() = _L("");
        
        // Set error code
        ModuleContainer().ModuleResult() = initResult;
        return initResult;
        }
    ModuleContainer().OperationText() = _L("");

    __TRACEI (KInit, ( CStifLogger::EBold, _L("Test module initialization done")));     

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestThreadContainer

    Method: EnumerateInThread

    Description: Enumerate test cases. Function calls GetTestCases method
    from the test module.

    This function is a static member function, which is intented to be called
    from the context of the test module thread.
        
    Parameters: None
    
    Return Values: TInt                           Error code.

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
TInt CTestThreadContainer::EnumerateInThread()
    {

    TInt err = KErrNone;
    __TRACEI ( KInit, ( CStifLogger::EBold, _L("Calling GetTestCasesL") ) );

    if ( iCases == NULL )
        {
        iCases = new RPointerArray<TTestCaseInfo>;
        if ( iCases == NULL )
            {
            ModuleContainer().OperationErrorResult() = KErrNoMemory;
            __TRACEI ( KError, ( _L("Can't create pointer array for cases") ) );
            return ModuleContainer().OperationErrorResult();
            }
        }

    // Thread ID logging(For error situations) !!!!! ----------
    /*
    RThread t;
    RDebug::Print(_L("XXXXXXXXXXXXXXXXXXXXXX CurrentThread=[%d]"), t.Id() );
    t.Open( t.Id() );   
    RDebug::Print(_L("XXXXXXXXXXXXXXXXXXXXXX Real id=[%d]"), t.Id() );
    t.Close();
    */
    // --------------------------------------------------------

    ModuleContainer().OperationText() = _L("GetTestCasesL");
    TRAPD (r, err = iTestModule->GetTestCasesL( 
                        ModuleContainer().OperationName(),
                        *iCases ) );
    ModuleContainer().OperationText() = _L("");

    // Leave
    if ( r != KErrNone )
        {
        __TRACEI ( KError, ( CStifLogger::ERed, _L("GetTestCasesL leave code %d"), r ) );
        TName tmpBuffer = _L("Leave from test module GetTestCasesL");        
        ErrorPrint( 1, tmpBuffer );        
        FreeEnumerationDataInThread();

        ModuleContainer().OperationErrorResult() = r;
        return r;
        }
        
    // Error originating from test module
    if ( err != KErrNone )
        {
        __TRACEI ( KError, ( CStifLogger::ERed, _L("GetTestCasesL returned error %d"), err ) );
        FreeEnumerationDataInThread();

        ModuleContainer().ModuleResult() = err;
        return err;
        }

    __TRACEI ( KInit, ( _L("GetTestCasesL successfully called") ) );

    // All ok.
    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestThreadContainer

    Method: FreeEnumerationDataInThread

    Description: Frees the enumeration data. This function is called, when
    the enumeration data is read from execution thread heap to server thread
    heap. If cases have not been enumerated function does nothing.
    
    Function is intented to be called from the context of the test module thread.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CTestThreadContainer::FreeEnumerationDataInThread()
    {

    __TRACEI ( KInit, ( _L("Freeing test case array") ) );

    if ( iCases )
        {
        iCases->ResetAndDestroy();
        delete iCases;
        iCases = NULL;
        }
    
    __TRACEI ( KInit, ( _L("Freeing test case array done") ) );

    }

/*
-------------------------------------------------------------------------------

    Class: CTestThreadContainer

    Method: ExecuteTestCaseInThread

    Description: Execute test case. This function calls either RunTestCase or 
    ExecuteOOMTestCase to execute and report the results.

    This function is a static member function, which is intented to be called
    from the context of the test module thread.

    Parameters: None

    Return Values: TInt: Error code

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CTestThreadContainer::ExecuteTestCaseInThread()
    {        
    TVersion moduleAPIVersion;
    moduleAPIVersion = iTestModule->Version(); 

    __TRACEI ( KInit, 
        ( CStifLogger::EBold, _L("Executing test case file=[%S] case=%d"), 
            &ModuleContainer().OperationName(), 
            ModuleContainer().OperationIntBuffer() ) );

    TInt r = KErrNone;
    
    // Thread handle
    RThread thisRt;
    r = DuplicateMutexHandles( thisRt );
       
    // Result from RunTestCase       
    TTestResult caseResult; 
    
    // Execution result from RunTestCase
    TInt err = KErrNone;

    // Fill in initial values
    TestExecution().FullResult().iCaseExecutionResultType = 
        TFullTestResult::ECaseExecuted;
    TestExecution().FullResult().iCaseExecutionResultCode = KErrNone;            
    TestExecution().FullResult().iStartTime.HomeTime();
    TestExecution().TestThreadFailure() = CTestExecution::ETestThreadOk;

    // Set handle to test execution
    TRAP( r, CTestModuleIf::NewL( this,
                                  iTestModule ) );

    ModuleContainer().OperationText() =_L("RunTestCaseL");
    
    // Do resource checks before starting test case
    iCheckResourceFlags = 0;        
    
    TInt tmp;
    TInt threadHandleCountBeforeTest;

    // Request count check
    TInt requestCountBeforeTest = thisRt.RequestCount();
    // Handle count check, not checking process handles
    thisRt.HandleCount( tmp, threadHandleCountBeforeTest );

    // If handle ok, then execute test          
    if( r == KErrNone )
        {       
        TInt testCaseNumber = ModuleContainer().OperationIntBuffer();

        // Do the test
        __TRACEI ( KInit, ( _L("About to call RunTestCaseL. If nothing in log \
            after line \"Calling RunTestCaseL\", check testserver log file.") ) );        
                    
        TInt firstMemFailure = 0; 
        TInt lastMemFailure = 0;        
        // Store the OOM test type
        CTestModuleBase::TOOMFailureType failureType;
                
        // Check if the current test case is supposed to be run using OOM
        if( iTestModule->OOMTestQueryL( ModuleContainer().OperationName(), 
                                        testCaseNumber, 
                                        failureType,
                                        firstMemFailure, 
                                        lastMemFailure ) )
            {
            //  Run the test case in OOM conditions      
            r = ExecuteOOMTestCase( testCaseNumber, 
                                    firstMemFailure, 
                                    lastMemFailure, 
                                    err, 
                                    caseResult );
            }
        else
            {
            // Run the test case the old way, without OOM testing           
            __TRACEI ( KInit, ( _L("Calling RunTestCaseL - \
                OOM condition is not set") ) );
            TRAP( r, err = iTestModule->RunTestCaseL( 
                                            testCaseNumber,
                                            ModuleContainer().OperationName(),
                                            caseResult ) );
            }
        }        
     
    // Do resource checks after test case execution
    // Handle count check
    TInt threadHandleCountAfterTest;
    thisRt.HandleCount( tmp, threadHandleCountAfterTest );
    // Request count check
    TInt requestCountAfterTest = thisRt.RequestCount();           
          
    ModuleContainer().OperationText() =_L("");

    // Store end time
    TestExecution().FullResult().iEndTime.HomeTime();   
    // Remove handle to testexecution 
    TRAPD( rr, CTestModuleIf::NewL( NULL, iTestModule ) );
    

    if ( rr != KErrNone )
        {        
        __TRACEI ( KError, ( _L("Memory low in executionthread.") ) );        
        // Do not actually handle error
        }
  
    // Report test result. Parts of this will be overwritten if error
    // is detected
    TestExecution().FullResult().iTestResult = caseResult;

    // Get target exit reasons
    CTestModuleIf::TExitReason allowedExitReason;
    TInt allowedExitCode = KErrNone;
    ExitReason( allowedExitReason, allowedExitCode );
    
    TBool returnLeakCheckFail = EFalse;

    // Check are STIF macros used
    if( iTestMacroInfo.iIndication ) 
        {
        // STIF macros are used. Set description info, test case to
        // ECaseExecuted state and case execution result code to KErrNone
        // to get test case to failed category.
        TName tmpResultDes;
        __TRACEI ( KError, ( CStifLogger::ERed, _L("Leave from RunTestCaseL(STIF TF's macro is used)" ) ) );
        // Set result description
        tmpResultDes.Copy( _L( "FILE[") );
        tmpResultDes.Append( iTestMacroInfo.iFileDes );
        tmpResultDes.Append( _L( "] FUNCTION[" ) );
        tmpResultDes.Append( iTestMacroInfo.iFunctionDes );
        tmpResultDes.Append( _L( "] LINE[" ) );
        tmpResultDes.AppendNum( iTestMacroInfo.iLine );
        tmpResultDes.Append( _L( "]" ) );
        // Other result information
        TestExecution().FullResult().iTestResult.iResult =
                                            iTestMacroInfo.iReceivedError;
        TestExecution().FullResult().iTestResult.iResultDes = tmpResultDes;
        TestExecution().FullResult().iCaseExecutionResultType = 
                                            TFullTestResult::ECaseExecuted;
        // Set category to failed cases
        TestExecution().FullResult().iCaseExecutionResultCode = KErrNone;
        StifMacroErrorInit(); // Initialization back to default
        }
    else if( r != KErrNone )
        {   // Case has left, overwrite normal result description string
        __TRACEI ( KError, ( CStifLogger::ERed, _L("Leave from RunTestCaseL, code %d"), r ) );
        // Set result description
        TName tmpResultDes = _L("Leave during case:");
        // Check if there was already some description passed to result object
        if(caseResult.iResultDes.Length() > 0)
            {
            tmpResultDes.Format(_L("Leave during case [%S]:"), &caseResult.iResultDes);
            if(tmpResultDes.Length() > KStifMaxResultDes)
                {
                tmpResultDes.SetLength(KStifMaxResultDes);
                }
            }
        // Other result information
        TestExecution().FullResult().iTestResult.iResult = KErrGeneral;
        TestExecution().FullResult().iTestResult.iResultDes = tmpResultDes;
        TestExecution().FullResult().iCaseExecutionResultType = 
            TFullTestResult::ECaseLeave;
        TestExecution().FullResult().iCaseExecutionResultCode = r;
        }    
    else if ( err != KErrNone )
        {   
        // Case has returned error (e.g. case not found )       
        __TRACEI ( KError, ( CStifLogger::ERed, _L("RunTestCaseL returned error %d"), err ) ); 
        TestExecution().FullResult().iCaseExecutionResultType = 
            TFullTestResult::ECaseErrorFromModule;
        TestExecution().FullResult().iCaseExecutionResultCode = err;
        }
    else if ( allowedExitReason != CTestModuleIf::ENormal )
        {
        // Test is failed, because it should end to panic or exception.
        __TRACEI ( KInit, ( _L("Case ended normally even if it should end to panic/exception") ) ); 
        TestExecution().FullResult().iTestResult.iResult = KErrGeneral;
        TestExecution().FullResult().iTestResult.iResultDes = 
            _L("Case did not ended to panic/exception");
        }
    // If test case is passed, check memory leak, handles etc...    
    else if( caseResult.iResult == KErrNone )
        {
        returnLeakCheckFail = ETrue;
        }
   
   // Test case leak checks
    // In EKA2 heap size cannot be measured because THeapWalk is no longer supported    
    LeakChecksForTestCase( returnLeakCheckFail, 
                           threadHandleCountBeforeTest,
                           threadHandleCountAfterTest,
                           requestCountBeforeTest,
                           requestCountAfterTest );

    // Close execution specific handles

    iPrintMutex.Close();
    iEventMutex.Close();
    iSndMutex.Close();
    iRcvMutex.Close();
    iInterferenceMutex.Close();
    iMeasurementMutex.Close();
    iCommandMutex.Close();
    
    // The Wait operation is performed to let the message from TestServer 
    // to TestEngine achieve TestEngine or TestCombiner.   
    iCommandSem.Wait();
    
    // Note: iTestThreadMutex.iClose() mutex will be used later, close in destructor.
    
    iPrintSem.Close();
    iEventSem.Close();
    iSndSem.Close();
    iRcvSem.Close();
    iInterferenceSem.Close();
    iMeasurementSem.Close();
    iCommandSem.Close();

    // Close thread handle 
    thisRt.Close();

    __TRACEI ( KVerbose, ( _L("ExecuteTestCase out") ) );

    // continues from CTestModuleContainer::RunL
        
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CTestThreadContainer

    Method: DuplicateMutexHandles

    Description: Duplicates mutex handles

    Parameters: None

    Return Values: TInt

    Errors/Exceptions: Panic if duplication fails

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CTestThreadContainer::DuplicateMutexHandles( RThread& aThread )
    {
    // For duplicating mutexes
    iPrintMutex.SetHandle( TestExecution().PrintMutexHandle() );
    iEventMutex.SetHandle( TestExecution().EventMutexHandle() );
    iSndMutex.SetHandle( TestExecution().SndMutexHandle() );
    iRcvMutex.SetHandle( TestExecution().RcvMutexHandle() );
    iInterferenceMutex.SetHandle( TestExecution().InterferenceMutexHandle() );
    iMeasurementMutex.SetHandle( TestExecution().MeasurementMutexHandle() );
    iCommandMutex.SetHandle(TestExecution().CommandMutexHandle());

    // Mutex for testcomplete and cancel operations. For duplicating mutex
    iTestThreadMutex.SetHandle( TestExecution().TestThreadMutexHandle() );

    // For duplicating semaphores
    iPrintSem.SetHandle( TestExecution().PrintSemHandle() );
    iEventSem.SetHandle( TestExecution().EventSemHandle() );
    iSndSem.SetHandle( TestExecution().SndSemHandle() );
    iRcvSem.SetHandle( TestExecution().RcvSemHandle() );
    iInterferenceSem.SetHandle( TestExecution().InterferenceSemHandle() );
    iMeasurementSem.SetHandle( TestExecution().MeasurementSemHandle() );
    iCommandSem.SetHandle(TestExecution().CommandSemHandle());
    
    // Store thread id for later use
    TestExecution().SetTestThread( aThread.Id() );
    
    // Duplicate handles from server thread
    TRAPD( r,
        User::LeaveIfError( iPrintMutex.Duplicate( iServerThread ) );
        User::LeaveIfError( iEventMutex.Duplicate( iServerThread ) );
        User::LeaveIfError( iSndMutex.Duplicate( iServerThread ) );
        User::LeaveIfError( iRcvMutex.Duplicate( iServerThread ) );
        User::LeaveIfError( iInterferenceMutex.Duplicate( iServerThread ) );
        User::LeaveIfError( iMeasurementMutex.Duplicate( iServerThread ) );
        User::LeaveIfError( iCommandMutex.Duplicate( iServerThread ) );
        
        User::LeaveIfError( iTestThreadMutex.Duplicate( iServerThread ) );

        User::LeaveIfError( iPrintSem.Duplicate( iServerThread ) );
        User::LeaveIfError( iEventSem.Duplicate( iServerThread ) );
        User::LeaveIfError( iSndSem.Duplicate( iServerThread ) );
        User::LeaveIfError( iRcvSem.Duplicate( iServerThread ) );
        User::LeaveIfError( iInterferenceSem.Duplicate( iServerThread ) );
        User::LeaveIfError( iMeasurementSem.Duplicate( iServerThread ) );
        User::LeaveIfError( iCommandSem.Duplicate( iServerThread ) );
        );

    // Raise panic if duplications failed        
    if( r != KErrNone )
        {
        Panic( EDuplicateFail );
        }
    
    // Return the result, no error occurred
    return KErrNone;            
    }

/*
-------------------------------------------------------------------------------

    Class: CTestThreadContainer

    Method: ExecuteOOMTestCase

    Description: Executes OOM test case

    Parameters: None

    Return Values: TInt

    Errors/Exceptions: Panic if EOOMDisableLeakChecks is not set and test case
    leaks memory.                       

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CTestThreadContainer::ExecuteOOMTestCase( TInt aTestCaseNumber,
                                               TInt aFirst, 
                                               TInt aLast, 
                                               TInt& aResult, 
                                               TTestResult& caseResult )
    {
    TBool OOMwarning = EFalse;  
    __TRACEI ( KInit, ( _L("CTestThreadContainer::ExecuteOOMTestCase") ) );            
    __TRACEI ( KInit, ( _L("Executing test case #%d using OOM"), aTestCaseNumber ) );               
    
    // OOM test environment initialization
    TRAPD( r, iTestModule->OOMTestInitializeL( 
                                ModuleContainer().OperationName(),
                                aTestCaseNumber ); );               
    
    for( TInt i=aFirst; i<aLast; i++ )
        {   
        // Fail the i:nth heap allocation
        User::__DbgSetAllocFail( RHeap::EUser, RHeap::EFailNext, i  );
        
        //__TRACEI ( KInit, ( _L("Setting %d:nth heap allocation to fail"), i  ) );             
                
        // Intersection of iCheckResourceFlags and 
        // EDisableMemoryLeakChecksInOOM to check if memory leak checks are to 
        // be used with OOM testing.        
        if( !( iCheckResourceFlags & CTestModuleIf::EOOMDisableLeakChecks ) )
            {
            User::__DbgMarkStart( RHeap::EUser );
            }
            
        TRAP( r, aResult = iTestModule->RunTestCaseL( 
                        aTestCaseNumber,
                        ModuleContainer().OperationName(),
                        caseResult ) );
        
        // Raise panic if test case leaks memory and EOOMDisableLeakChecks is not
        // set        
        if( !( iCheckResourceFlags & CTestModuleIf::EOOMDisableLeakChecks ) )
            {
            User::__DbgMarkEnd( RHeap::EUser, 0 );
            }
        
        // If no error occurred, fake a memory error to make sure that this is
        // the last test. If this last allocation goes wrong, it proves that 
        // either the FAILNEXT() macro has reached its limit or that somewhere
        // in the code some object TRAPped the OOM exception and did not leave.     
        if( ( r != KErrNoMemory ) && ( aResult != KErrNoMemory  )
            && ( caseResult.iResult != KErrNoMemory ) )
            {
            TInt* dummy = new TInt;
            OOMwarning = ( dummy != NULL );
            delete dummy;
            }

        // Cancel the simulated heap allocation failure
        User::__DbgSetAllocFail( RHeap::EUser, RHeap::ENone, 1 );
    
        if( ( r != KErrNoMemory ) && !OOMwarning && ( aResult != KErrNoMemory )
            && ( caseResult.iResult != KErrNoMemory ) )
            {
            // If we get here test was executed properly (= no memory error
            // and no warning)
            break;
            }

        if( OOMwarning )
            {            
            // It is possible that during testing some components TRAP the OOM
            // exception and continue to run (do not leave) or they return an 
            // error other than KErrNoMemory. These situations are making the
            // OOM testing really difficult, so they should be detected and 
            // make the tester aware.
             
            // Since each test case might have a specific oppinion on handling
            // this situation, it is left up to the tester to handle it by 
            // implementing the OOMHandleWarningL method. STIF will log a
            // warning and call OOMHandleWarningL method.

            // Print the OOM error message
            __TRACEI ( KInit, ( _L("Possible trapped or non-leaving allocation in test case #%d"), i  ) );                          

            iTestModule->OOMHandleWarningL( ModuleContainer().OperationName(), aTestCaseNumber, i );

            // Clear the warning flag
            OOMwarning = EFalse;
            }
        }
        
    // OOM test environment finalization                    
    __TRACEI ( KInit, ( _L("Calling OOMTestFinalizeL") ) ); 
    TRAPD( fres, iTestModule->OOMTestFinalizeL( 
                                    ModuleContainer().OperationName(), 
                                    aTestCaseNumber ); );
    // Check the result
    if( fres != KErrNone )
        {
        __TRACEI ( KInit, ( _L("OOMTestFinalizeL execution failed with error %d"), fres ) );                            
        }
                
    return r;
    }

/*
-------------------------------------------------------------------------------

    Class: CTestThreadContainer

    Method: LeakChecksForTestCase

    Description: Checks test case for memory, handle and request leaks

    Parameters: None    

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CTestThreadContainer::LeakChecksForTestCase( TBool aReturnLeakCheckFail,
                                                  TInt aThreadHandleCountBeforeTest,
                                                  TInt aThreadHandleCountAfterTest,
                                                  TInt aRequestCountBeforeTest, 
                                                  TInt aRequestCountAfterTest )
                                                
    {      
    __TRACEI ( KInit, ( _L("CTestThreadContainer::LeakChecksForTestCase") ) );                          

    // Note: Request leaks detection is disabled in UI components testing
    if( !( iCheckResourceFlags & CTestModuleIf::ETestLeaksRequests ) && 
             ( aRequestCountBeforeTest != aRequestCountAfterTest ) &&
             ( !iModuleContainer->GetTestModule()->GetTestServer()->UiTesting()))
        {
        // Test is failed, because it should end to panic or exception.
        __TRACEI ( KError, ( CStifLogger::ERed, 
            _L("Asynchronous request leak from test module. Request count before:[%d] and after:[%d] test."),
            aRequestCountBeforeTest, aRequestCountAfterTest ) );
            
        // Set failure status    
        TestExecution().TestThreadFailure() |= CTestExecution::ETestRequestLeak;
        if( aReturnLeakCheckFail )
            {
            aReturnLeakCheckFail = EFalse;   // return first fail   
#ifndef STIF_DISABLE_LEAK_CHECK 
            // Testcase set to failed when request leak occurred
            TestExecution().FullResult().iTestResult.iResult = KErrGeneral;
#endif
            TestExecution().FullResult().iTestResult.iResultDes = 
                _L("Asynchronous request leak from testmodule");
            TestExecution().FullResult().iTestResult.iResultDes.
                AppendNum( aRequestCountAfterTest );
            }
        }
    // Note: Handle leaks detection is disabled in UI components testing
    if( !( iCheckResourceFlags & CTestModuleIf::ETestLeaksHandles ) && 
             ( aThreadHandleCountBeforeTest != aThreadHandleCountAfterTest ) &&
             ( !iModuleContainer->GetTestModule()->GetTestServer()->UiTesting()) )
        {
        // Test is failed, because it should end to panic or exception.
        __TRACEI ( KError, ( CStifLogger::ERed, 
            _L("Thread handle leak from test module. Handle count before:[%d] and after:[%d] test."),
            aThreadHandleCountBeforeTest, aThreadHandleCountAfterTest ) ); 
            
        // Set failure status
        TestExecution().TestThreadFailure() |= CTestExecution::ETestHandleLeak;    
        if( aReturnLeakCheckFail )
            {
            aReturnLeakCheckFail = EFalse;   // return first fail   
#ifndef STIF_DISABLE_LEAK_CHECK 
            // Testcase is set to failed yet when handle leak occurred
            TestExecution().FullResult().iTestResult.iResult = KErrGeneral;
#endif
            TestExecution().FullResult().iTestResult.iResultDes = 
                _L("Thread handle leak from testmodule");
            TestExecution().FullResult().iTestResult.iResultDes.
                AppendNum( aRequestCountAfterTest );
            }
        }       
    }

/*
-------------------------------------------------------------------------------

    Class: CTestThreadContainer

    Method: DeleteTestModule

    Description: Deletes a test module

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CTestThreadContainer::DeleteTestModule()
    {

    __TRACEI ( KInit, ( _L("Deleting test module instance at 0x%x"), iTestModule ) );
    // Delete the test module
    ModuleContainer().OperationText() = _L("DESTRUCTOR");
    TRAPD( r, delete iTestModule );
    ModuleContainer().OperationText() = _L("");
    iTestModule = NULL;

    if ( r )
        {
        __TRACEI ( KError, ( _L("Leave when deleting test module, code %d"), r ) );
        }

    __TRACEI ( KInit, ( _L("Test module instance deleted") ) );

    }
    
    /*
-------------------------------------------------------------------------------

    Class: CTestThreadContainer

    Method: TestCases

    Description: Returns constant pointer to test case array

    Parameters: None
    
    Return Values: const RPointerArray<TTestCaseInfo>*  Test cases

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
const RPointerArray<TTestCaseInfo>* CTestThreadContainer::TestCases() const
    {
    
    return iCases;

    }
    
/*
-------------------------------------------------------------------------------

    Class: CTestThreadContainer

    Method: ErrorPrint

    Description: Prints error

    Parameters: const TInt aPriority :in: Priority
                TPtrC aError: in: Error

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CTestThreadContainer::ErrorPrint( const TInt aPriority, 
                                       TPtrC aError )
    {
    
    // Get access to print stuff
    iErrorPrintSem.Wait();
    
    // Get status variable from server
    TRequestStatus* status = 
        ModuleContainer().GetRequest( CTestModuleContainer::ERqErrorPrint );
    
    if( status == NULL )
        {
        Panic( ENullRequest );
        return;
        }
    
    // Fill in progress
    TErrorNotification& progress = ModuleContainer().ErrorNotification();
    progress.iPriority = aPriority;
    progress.iText = aError;
    
    // Complete action to server
    iServerThread.RequestComplete( status, KErrNone );
    
    }
    
/*
-------------------------------------------------------------------------------

    Class: CTestThreadContainer

    Method: DoNotifyPrint

    Description: If print notification available, notification is copied to
                    client memory space and request is completed.
                 Else new print queue item is created and appended to print 
                    queue. If queue is full or memory can't be allocated,
                    then message will be discarded.
        
    Parameters: const TInt aPriority  :       :in:  Priority
                const TStifInfoName& aDes         :in:  Description
                const TName& aBuffer          :in:  Value
    
    Return Values: None

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
void CTestThreadContainer::DoNotifyPrint( const TInt aPriority,
                                    const TStifInfoName& aDes,
                                    const TName& aBuffer )
    {    
    // Get access to print stuff
    iPrintSem.Wait();
    
    iPrintMutex.Wait(); // Take mutex to get access to server thread.
                        // Between Wait and Signal is critical section and this
                        // verifies that iPrintSem and RequestComplete is done
                        // successfully.
    
    // Get status variable from server
    TRequestStatus* status = 
        TestExecution().GetRq( CTestExecution::ERqPrint );
    
    if( status == NULL )
        {
        iPrintMutex.Signal();
        Panic( ENullRequest );
        return;
        }

    if( *status != KRequestPending )
        {
        // CPrintHandler::DoCancel called before getting here, just return
        iPrintMutex.Signal();
        return;    
        }
    // Fill in progress
    TTestProgress& progress = TestExecution().TestProgress();
    progress.iPosition = aPriority;
    progress.iDescription = aDes;
    progress.iText = aBuffer;
    
    // Complete action to server
    iServerThread.RequestComplete( status, KErrNone );

    iPrintMutex.Signal();

    }

/*
-------------------------------------------------------------------------------

    Class: CTestThreadContainer

    Method: DoNotifyEvent

    Description: Forward event request.
        
    Parameters: const TEventIf: in: Event definition
                TRequestStatus* aStatus: in: TRequestStatus to complete 
    
    Return Values: None

    Errors/Exceptions: Panics if event array can't be created

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
TInt CTestThreadContainer::DoNotifyEvent( TEventIf& aEvent, 
                                          TRequestStatus* aStatus )
    {
    
    TInt ret = KErrNone;
    
    // Send event req
    SetEventReq( TEventDef::EEventCmd, aEvent, aStatus );
    
    if( aStatus == NULL )
        {
        // Synchronous Event command used ->
        // Block until completed with ECmdComplete from NotifyEvent
        // Cannot be done before ERelEvent, 
        // because Unset may be blocking the server  
        User::WaitForRequest( iReqStatus ); 
        
        User::After( 1 );// workaround found for STIF 347 
        
        // Return result from engine 
        ret = iReqStatus.Int();
        
        }    
    
    return ret;
    
    }
    
/*
-------------------------------------------------------------------------------

    Class: CTestThreadContainer

    Method: CancelEvent

    Description: Cancels pending event request.
        
    Parameters: None
    
    Return Values: None

    Errors/Exceptions: None
    
    Status: Proposal
    
-------------------------------------------------------------------------------
*/
void CTestThreadContainer::CancelEvent( TEventIf& aEvent, 
                                        TRequestStatus* aStatus )
    {

    __TRACEI( KMessage, ( _L( "CTestThreadContainer::CancelEvent(%d): %S [%p]" ), 
            aEvent.Type(), &aEvent.Name(), aStatus ) );
            
    // Send event req
    SetEventReq( TEventDef::EEventCmdCancel, aEvent, aStatus );
    
    }

/*
-------------------------------------------------------------------------------

    Class: CTestThreadContainer

    Method: SetExitReason

    Description: Set exit reason
        
    Parameters: const TExitReason aExitReason in: Exit reason
                const TInt aExitCode in: Exit code

    Return Values: None
    
    Errors/Exceptions: None

    
    Status: Proposal
    
-------------------------------------------------------------------------------
*/
void CTestThreadContainer::SetExitReason( const CTestModuleIf::TExitReason aExitReason, 
                                    const TInt aExitCode )
    {
    
    TInt exitCode = aExitCode;
    
    if( ( aExitReason == CTestModuleIf::ENormal ) &&
        ( aExitCode != KErrNone ) )
        {
        __TRACEI( KError, 
            ( _L( "SetExitReason: Exit type normal uses always exit code 0 (given %d is not used)" ), 
                exitCode ) );
        exitCode = KErrNone;
        }

    ModuleContainer().AllowedExitReason() = aExitReason;
    ModuleContainer().AllowedExitCode() = exitCode;              
        
    }

/*
-------------------------------------------------------------------------------

    Class: CTestThreadContainer

    Method: SetBehavior

    Description: Set test behaviour.
        
    Parameters:  const CTestModuleIf::TTestBehavior aType: in: behaviour type 
                 TAny* aPtr: in: data

    Return Values: Symbian OS error code.
    
    Errors/Exceptions: None
    
    Status: Proposal
    
-------------------------------------------------------------------------------
*/
TInt CTestThreadContainer::SetBehavior( const CTestModuleIf::TTestBehavior aType, 
                                        TAny* /*aPtr*/ )
    {
    
    if( aType & CTestModuleIf::ETestLeaksMem )
        {
        iCheckResourceFlags |= CTestModuleIf::ETestLeaksMem;
        }
    if( aType & CTestModuleIf::ETestLeaksRequests )
        {
        iCheckResourceFlags |= CTestModuleIf::ETestLeaksRequests;
        } 
    if( aType & CTestModuleIf::ETestLeaksHandles )
        {
        iCheckResourceFlags |= CTestModuleIf::ETestLeaksHandles;
        }
    // For OOM testing
    if( aType & CTestModuleIf::EOOMDisableLeakChecks )
        {
        iCheckResourceFlags |= CTestModuleIf::EOOMDisableLeakChecks;
        }                
    if( !( aType & iCheckResourceFlags ) )
        {
        return KErrNotFound;
        }
        
    return KErrNone;
    
    }

/*
-------------------------------------------------------------------------------

    Class: CTestThreadContainer

    Method: ExitReason

    Description: Gets exit reason
        
    Parameters: TExitReason& aExitReason out: Exit reason
                TInt& aExitCode out: Exit code

    Return Values: None
    
    Errors/Exceptions: None
    
    Status: Proposal
    
-------------------------------------------------------------------------------
*/
void CTestThreadContainer::ExitReason( CTestModuleIf::TExitReason& aExitReason, 
                                   TInt& aExitCode )
    {

    aExitReason = ModuleContainer().AllowedExitReason();
    aExitCode = ModuleContainer().AllowedExitCode();
          
        
    }
    
/*
-------------------------------------------------------------------------------

    Class: CTestThreadContainer

    Method: SetEventReq 

    Description: Sets asynchronous event request.
        
    Parameters: None
    
    Return Values: None

    Errors/Exceptions: None
    
    Status: Proposal
    
-------------------------------------------------------------------------------
*/
void CTestThreadContainer::SetEventReq( TEventDef::TEventCmdType aType, 
                                        TEventIf& aEvent, 
                                        TRequestStatus* aStatus )
    {
    // Get access to event stuff
    iEventSem.Wait();

    iEventMutex.Wait(); // Take mutex to get access to server thread.
                        // Between Wait and Signal is critical section and this
                        // verifies that iPrintSem and RequestComplete is done
                        // successfully.
    
    // Get status variable from server
    TRequestStatus* status = 
        TestExecution().GetRq( CTestExecution::ERqEvent );
    
    if( status == NULL )
        {
        iEventMutex.Signal();
        Panic( ENullRequest );
        return;
        }
    if( *status != KRequestPending )
        {
        // CEventHandler::DoCancel called before getting here, just return
        iEventMutex.Signal();
        return;    
        }
    
    // Fill in event on server thread
    TEventDef& event = TestExecution().EventDef();
    event.iType = aType;
    event.iEvent.Copy( aEvent );

    if( aStatus )
        {
        // Store TRequestStatus which is completed when next EEnable comes in
        event.iStatus = aStatus;
        } 
    else
        {
        iReqStatus = KRequestPending;
        event.iStatus = &iReqStatus;
        }   

    __TRACEI( KMessage ,(_L("SetReq Stat %d, %x"), this, 
        aStatus ));
    
    // Complete action to server
    iServerThread.RequestComplete( status, KErrNone );
    
    iEventMutex.Signal();

    }

/*
-------------------------------------------------------------------------------

    Class: CTestThreadContainer

    Method: DoRemoteReceive

    Description: Enable remote receive and send.
    
    Parameters: 
    
    Return Values: None

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
void CTestThreadContainer::DoRemoteReceive( TStifCommand aRemoteCommand,
                                            TParams aParams,
                                            TInt aLen,
                                            TRequestStatus& aStatus )
    {    
    
    switch( aRemoteCommand )
        {
        case EStifCmdSend:             // "Send"
        case EStifCmdReboot:           // "Send"
        case EStifCmdStoreState:       // "Send"
        case EStifCmdGetStoredState:   // "Receive, this must be done with two phase"
        case EStifCmdMeasurement:      // "Receive"
            {
            __TRACEI( KMessage, ( _L( "CTestThreadContainer::DoRemoteReceive Wait SndSem" ) ) );
            
            // Get access to sender 
            // (for receive, used for securing access to shared memory)
            iSndSem.Wait();

            iSndMutex.Wait();   // Take mutex to get access to server thread.
                                // Between Wait and Signal is critical section and this
                                // verifies that iPrintSem and RequestComplete is done
                                // successfully.

            // Get status variable from server
            TRequestStatus* status = 
                TestExecution().GetRq( CTestExecution::ERqSnd );

            if( status == NULL )
                {
                iSndMutex.Signal();
                Panic( ENullRequest );
                return;
                }
            if( *status != KRequestPending )
                {
                // CSndHandler::DoCancel called before getting here, just return
                iSndMutex.Signal();
                return;    
                }                

            // Fill in information
            TCmdDef& aDef = TestExecution().SndInfo();
            aDef.iCommand = aRemoteCommand;
            aDef.iParam = aParams;
            aDef.iLen = aLen;
            aDef.iStatus = &aStatus;
            
            __TRACEI( KMessage ,
                (_L("CTestThreadContainer::DoRemoteReceive Complete request 0x%x"),
                    status ));
            // Complete action to server
            iServerThread.RequestComplete( status, KErrNone );  

            iSndMutex.Signal();
            }
            break;
        case EStifCmdReceive:          // "Receive"
             {
            __TRACEI( KMessage, ( _L( "CTestThreadContainer::DoRemoteReceive Wait RcvSem" ) ) );

            // Get access to receive handler 
            iRcvSem.Wait();

            iRcvMutex.Wait();   // Take mutex to get access to server thread.
                                // Between Wait and Signal is critical section and this
                                // verifies that iPrintSem and RequestComplete is done
                                // successfully.

            // Get status variable from server
            TRequestStatus* status = 
                TestExecution().GetRq( CTestExecution::ERqRcv );

            if( status == NULL )
                {
                iRcvMutex.Signal();
                Panic( ENullRequest );
                return;
                }
            if( *status != KRequestPending )
                {
                // CRcvHandler::DoCancel called before getting here, just return
                iRcvMutex.Signal();
                return;    
                }                

            // Fill in information
            TCmdDef& aDef = TestExecution().RcvInfo();
            aDef.iCommand = aRemoteCommand;
            aDef.iParam = aParams;
            aDef.iLen = aLen;
            aDef.iStatus = &aStatus;
            
            __TRACEI( KMessage ,
                (_L("CTestThreadContainer::DoRemoteReceive Complete request 0x%x"),
                    status ));
            __TRACEI( KMessage, ( _L( "CTestThreadContainer::DoRemoteReceive signal RcvSem" ) ) );
            //iReceiverSem.Signal();

            // Complete action to server
            iServerThread.RequestComplete( status, KErrNone );  

            iRcvMutex.Signal();
            
            }
            break;

        default:
            TRequestStatus* rs = &aStatus;
            User::RequestComplete( rs, KErrNotSupported );
            break;
        }
        
    }
    
/*
-------------------------------------------------------------------------------

    Class: CTestThreadContainer

    Method: DoRemoteReceiveCancel

    Description: Cancel DoRemoteReceive
    
    Parameters: None
    
    Return Values: None

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
TInt CTestThreadContainer::DoRemoteReceiveCancel()
    {   
           
    // Get access to receive handler
    iRcvSem.Wait();

    iRcvMutex.Wait();   // Take mutex to get access to server thread.
                        // Between Wait and Signal is critical section and this
                        // verifies that iPrintSem and RequestComplete is done
                        // successfully. 

    // Get status variable from server
    TRequestStatus* status = 
        TestExecution().GetRq( CTestExecution::ERqRcv );

    if( status == NULL )
        {
        iRcvMutex.Signal();
        return KErrNotFound;
        }
        
    if( *status != KRequestPending )
        {
        // CRcvHandler::DoCancel called before getting here, just return
        iRcvMutex.Signal();
        Panic( ENullRequest );
        return KErrNone;    
        }        

    // Fill in information
    TCmdDef& aDef = TestExecution().RcvInfo();
    aDef.iCommand = EStifCmdReceiveCancel;
    // Complete action to server
    iServerThread.RequestComplete( status, KErrNone );  
    
    iRcvMutex.Signal();

    return KErrNone;
    
    }

/*
-------------------------------------------------------------------------------

    Class: CTestThreadContainer

    Method: TestComplete

    Description: Complete test operation: Get test case, run test case,
                 complete test case, etc.
        
    Parameters: TInt aCompletionCode: in: completion code.
    
    Return Values: None

    Errors/Exceptions: None

    Status: Approved
    
-------------------------------------------------------------------------------
*/
void CTestThreadContainer::TestComplete( TInt aCompletionCode )
    {    
    
    // Get status variable from server
    TRequestStatus* status = 
        ModuleContainer().GetRequest( CTestModuleContainer::ERqTestCase );
    
    if( status == NULL )
        {
        Panic( ENullRequest );
        return;
        }
        
    // Complete action to server
    if( iTestThreadMutex.Handle() == 0 )
        {
        // Actual test case is not started yet. Inititialization phase is ongoing.
        // Before the completion check if the status was not already completed
        // from other thread in CTestModuleContainer::DoCancel().
        // For details see Jira STIF-564
        if(*status == KRequestPending)
            iServerThread.RequestComplete( status, aCompletionCode );
        }
    else
        {
        // Test case execution is started. Test is ongoing.
        // Before the completion check if the status was not already completed
        // from other thread in CTestModuleContainer::DoCancel().
        // For details see Jira STIF-564
        if(*status == KRequestPending)
            {
            iTestThreadMutex.Wait(); // Block that complete and cancel do not
                                     // executed at the same time.
            iServerThread.RequestComplete( status, aCompletionCode );
            iTestThreadMutex.Signal();
            }            
        }
    
    }

/*
-------------------------------------------------------------------------------

    Class: CTestThreadContainer

    Method: UIExecutionThread

    Description: This is the test module execution thread "main" function".
    All test module function calls are executed in context of this execution
    thread.

    When the thread is resumed first time, function goes to wait a semaphore.
    Operations are initiated by setting operation and signaling the semaphore.
    If operation is synchronous, then end of operation is signaled by using
    OperationCompleted -Semaphore. When operation is done, function (and thread)
    are going to wait OperationSemaphore.

    Function exist either when operation does fatal error, or operation type
    is "Exit". The thread function exist from it's main loop and the thread
    will be terminated.

    Parameters: TAny* aParams:                :in:  Pointer to CTestModuleContainer
    
    Return Values: TInt                        KErrNone

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
TInt CTestThreadContainer::UIExecutionThread( TAny* aParams )
	{
	
    CTestModuleContainer* moduleContainer = 
    	(CTestModuleContainer*) aParams;
    
    CTestModule* module = moduleContainer->GetTestModule();
    CTestServer* testServer = module->GetTestServer();
    CTestThreadContainerRunnerFactory* factory = testServer->GetTestThreadContainerRunnerFactory();    
    
    RThread server;
    // Duplicate handles from server thread
    TInt ret = server.Open( moduleContainer->ServerThreadId() );
    if( ret != KErrNone )
    	{
    	Panic( EThreadHandleOpenFail );
    	}
    RSemaphore OperationStartSemaphore;
    OperationStartSemaphore.SetHandle( 
    		moduleContainer->OperationStartSemHandle() );
    if( OperationStartSemaphore.Duplicate( server ) != KErrNone )
        {
        Panic( EDuplicateFail );
        }        
    RSemaphore OperationChangeSemaphore;
    OperationChangeSemaphore.SetHandle( 
    		moduleContainer->OperationChangeSemHandle() );
    if( OperationChangeSemaphore.Duplicate( server ) != KErrNone )
        {
        Panic( EDuplicateFail );
        }
    server.Close();
    
    CTestThreadContainerRunner* runner = factory->CreateL();
    
    runner->Setup( moduleContainer );
    
    while ( runner->IsReusable() )
        {
        // Thread is going to suspend        
        runner->CheckSignalFromSuspend();
        
        // Wait next operation
        OperationStartSemaphore.Wait();

        // Get operation semaphore
        OperationChangeSemaphore.Wait();    
    
        // Run and wait active object
        runner->RunOneIteration();
        
        OperationChangeSemaphore.Signal();        
        }
        
    OperationStartSemaphore.Close();
    OperationChangeSemaphore.Close();        
        
    runner->TeareDown();
    
    runner->Deque();
    
    factory->DeleteL( runner );    
    
    return KErrNone;    
	}


/*
-------------------------------------------------------------------------------

    Class: CTestThreadContainer

    Method: ExecutionThread

    Description: This is the test module execution thread "main" function".
    All test module function calls are executed in context of this execution
    thread.

    When the thread is resumed first time, function goes to wait a semaphore.
    Operations are initiated by setting operation and signaling the semaphore.
    If operation is synchronous, then end of operation is signaled by using
    OperationCompleted -Semaphore. When operation is done, function (and thread)
    are going to wait OperationSemaphore.

    Function exist either when operation does fatal error, or operation type
    is "Exit". The thread function exist from it's main loop and the thread
    will be terminated.

    Parameters: TAny* aParams:                :in:  Pointer to CTestModuleContainer
    
    Return Values: TInt                        KErrNone

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
TInt CTestThreadContainer::ExecutionThread( TAny* aParams )
    {
    TInt error( KErrNone );
    
    const TUint32 KAll = 0xFFFFFFFF;
#ifndef __HIDE_IPC_V1__ // e.g. 7.0s, 8.0a
    RThread currentThread;
    currentThread.SetExceptionHandler( ExceptionHandler, KAll );
#else // PlatSec used. Thread exception management is part of the User class.
    User::SetExceptionHandler( ExceptionHandler, KAll );
#endif // __HIDE_IPC_V1__

     // Check parameters
    __ASSERT_ALWAYS( aParams, Panic( EInvalidCTestThreadContainer ) );

    CTestModuleContainer* moduleContainer = 
        (CTestModuleContainer*) aParams;
       
    // Create cleanup stack
    CTrapCleanup* tc = CTrapCleanup::New();
    __ASSERT_ALWAYS( tc, Panic( ECreateTrapCleanup ) );

    CTestThreadContainer* exec = NULL;    
    TRAPD( err,
        exec = CTestThreadContainer::NewL( moduleContainer, 
                                           moduleContainer->ServerThreadId() );
        );    
    if( err != KErrNone )
        {
        Panic( ENullTestThreadContainer );
        }

    // Construct the logger
    TName path = _L("C:\\logs\\testframework\\testserver\\");
    TFileName name = _L("testserver_thread_");  
    name.Append( moduleContainer->TestModuleName() );

    // Create logger, in Wins use HTML in HW default logger
    TLoggerSettings loggerSettings;

    // Directory must create by hand if test server log wanted
    loggerSettings.iCreateLogDirectories = EFalse;

    loggerSettings.iOverwrite = ETrue;
    loggerSettings.iTimeStamp = ETrue;
    loggerSettings.iLineBreak = ETrue;
    loggerSettings.iEventRanking = EFalse;
    loggerSettings.iThreadId = EFalse;
    loggerSettings.iHardwareFormat = CStifLogger::ETxt;
#ifndef FORCE_STIF_INTERNAL_LOGGING_TO_RDEBUG
    loggerSettings.iEmulatorFormat = CStifLogger::EHtml;
    loggerSettings.iHardwareOutput = CStifLogger::EFile;
    loggerSettings.iEmulatorOutput = CStifLogger::EFile;
#else
    RDebug::Print( _L( "STIF Test Server's thread logging forced to RDebug" ) );
    loggerSettings.iEmulatorFormat = CStifLogger::ETxt;
    loggerSettings.iHardwareOutput = CStifLogger::ERDebug;
    loggerSettings.iEmulatorOutput = CStifLogger::ERDebug;
#endif
    loggerSettings.iUnicode = EFalse;
    loggerSettings.iAddTestCaseTitle = EFalse;

    TRAP ( error, exec->iThreadLogger = CStifLogger::NewL( path, name,
                                                            loggerSettings ) );

    RLibrary module;                    // Handle to test module library
    TBool reusable = ETrue;             // Is test module reusable?
    TBool initialized = EFalse;         // Is module initialized?
    TBool signalFromSuspend = EFalse;   // Send signal from suspend state?

    RThread server;
    // Duplicate handles from server thread
    TInt ret = server.Open( moduleContainer->ServerThreadId() );
    if( ret != KErrNone )
    	{
    	Panic( EThreadHandleOpenFail );
    	}
    RSemaphore OperationStartSemaphore;
    OperationStartSemaphore.SetHandle( 
        exec->ModuleContainer().OperationStartSemHandle() );
    if( OperationStartSemaphore.Duplicate( server ) != KErrNone )
        {
        Panic( EDuplicateFail );
        }        
    RSemaphore OperationChangeSemaphore;
    OperationChangeSemaphore.SetHandle( 
        exec->ModuleContainer().OperationChangeSemHandle() );
    if( OperationChangeSemaphore.Duplicate( server ) != KErrNone )
        {
        Panic( EDuplicateFail );
        }
    server.Close();
   
    
    ret = KErrNone;

    // The test module thread will stay in this loop until it either
    // dies or is exited nicely.
    while ( reusable )
        {
        // Thread is going to suspend
        
        if ( signalFromSuspend )
            {
            signalFromSuspend = EFalse;
            exec->TestComplete( ret );
            }
        ret = KErrNone;
        
        // Wait next operation
        OperationStartSemaphore.Wait();

        // Get operation semaphore
        OperationChangeSemaphore.Wait();
        switch ( moduleContainer->OperationType() )
            {

            // Test module initialisation
            case CTestModuleContainer::EInitializeModule:
                {
                __ASSERT_ALWAYS ( !initialized,
                                  Panic( EReInitializingTestModule ) );

                // Initialize module
                if ( exec->InitializeModuleInThread( module ) == KErrNone )
                    {
                    initialized = ETrue;
                    }

                signalFromSuspend = ETrue;
                break;
                }

            // Test case enumeration
            case CTestModuleContainer::EEnumerateInThread:
                {
                __ASSERT_ALWAYS ( initialized,
                                  Panic( ETestModuleNotInitialized ) );
                ret = exec->EnumerateInThread();

                signalFromSuspend = ETrue;
                break;
                }

            // Free test case enumeration data
            case CTestModuleContainer::EFreeEnumerationData:
                {
                __ASSERT_ALWAYS ( initialized,
                                  Panic( ETestModuleNotInitialized ) );
                exec->FreeEnumerationDataInThread ();
                
                signalFromSuspend = ETrue;
                break;
                }

            // Execute test case
            case CTestModuleContainer::EExecuteTestInThread:
                {
                __ASSERT_ALWAYS ( initialized,
                                  Panic( ETestModuleNotInitialized ) );
                ret = exec->ExecuteTestCaseInThread ();

                signalFromSuspend = ETrue;
                break;
                }

            // Exiting (i.e test server is unloading)
            case CTestModuleContainer::EExit:
                {
                reusable = EFalse;
                break;
                }

            // Illegal state
            default:
                {
                Panic( EInvalidTestModuleOperation );
                }
            }
            OperationChangeSemaphore.Signal();
        
        }
        
    OperationStartSemaphore.Close();
    OperationChangeSemaphore.Close();

    exec->DeleteTestModule();

    // Close handle to module. No function calls to test
    // module are possible after this line.
    module.Close();

    // Delete logger    
    delete exec->iThreadLogger;
    exec->iThreadLogger = NULL;

    // Delete clean-up stack.
    delete tc;
    tc = NULL;

    // Operation completed ( = Exit completed )
    exec->TestComplete( KErrNone );
    
    delete exec;
    
    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestThreadContainer

    Method: Panic

    Description: Panicing function for test thread.

    Parameters: TPanicReason aReason: in: Reason code
    
    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CTestThreadContainer::Panic( TPanicReason aReason )
    {
    
    RDebug::Print( _L("CTestThreadContainer::Panic %d"), aReason );
    
    User::Panic( _L("CTestThreadContainer::Panic"), aReason );
    
    }
/*
-------------------------------------------------------------------------------

    Class: CTestThreadContainer

    Method: ServerAlive

    Description: Check that server is alive.

    Parameters: None
    
    Return Values: None

    Errors/Exceptions: Panics thread if server has died.

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CTestThreadContainer::IsServerAlive() const
    {
        
    if( iServerThread.ExitType() != EExitPending ) 
        {
        // Server thread has died
        __RDEBUG( ( _L( "Server died" ) ) );
        Panic( EServerDied );
        }
        
    }
    
/*
-------------------------------------------------------------------------------

    Class: CTestThreadContainer

    Method: TestExecution

    Description: Return CTestExecution handle to "parent" i.e. server.

    Parameters: None
    
    Return Values: CTestExecution&

    Errors/Exceptions: Panics thread if server has died.

    Status: Proposal

-------------------------------------------------------------------------------
*/
CTestExecution& CTestThreadContainer::TestExecution() const
    { 
    
    IsServerAlive();
    CTestExecution* execution = iModuleContainer->TestExecution();
    if( execution == NULL )
        {
        Panic( ENullExecution );
        }
    return *execution;
    
    };

/*
-------------------------------------------------------------------------------

    Class: CTestThreadContainer

    Method: GetTestCaseArguments

    Description: Get test case arguments

    Parameters: None
    
    Return Values: test case arguments

    Errors/Exceptions: 

    Status: Proposal

-------------------------------------------------------------------------------
*/
const TDesC& CTestThreadContainer::GetTestCaseArguments() const
    {
    return TestExecution().GetTestCaseArguments();
    }


/*
-------------------------------------------------------------------------------

    Class: CTestThreadContainer

    Method: TestExecution

    Description: Return CTestExecution handle to "parent" i.e. server.

    Parameters: None
    
    Return Values: CTestExecution&

    Errors/Exceptions: Panics thread if server has died.

    Status: Proposal

-------------------------------------------------------------------------------
*/
CTestModuleContainer& CTestThreadContainer::ModuleContainer()
    { 
    
    IsServerAlive();
    return *iModuleContainer; 
    
    };
 
/*
-------------------------------------------------------------------------------

    Class: CTestThreadContainer

    Method: ExceptionHandler

    Description: Test execution thread exception handler

    Just kill thread. Undertaker handles rest.

    Parameters: TExcType: in: Exception type
    
    Return Values: None

    Errors/Exceptions: This function kills the thread where it is executed in.

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
void CTestThreadContainer::ExceptionHandler ( TExcType aType )
    {

    // Kill the current thread, undertaker handles rest
    RThread current;
    current.Kill( aType );

    // This line is never executed, because thread has been killed.
    }

/*
-------------------------------------------------------------------------------

    Class: CTestThreadContainer

    Method: StifMacroErrorInit

    Description: STIF TF's macro. Initialized TTestMacro.

    Parameters: None
    
    Return Values: None

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
void CTestThreadContainer::StifMacroErrorInit()
    {
    iTestMacroInfo.iIndication = EFalse;
    iTestMacroInfo.iFileDes = KNullDesC;
    iTestMacroInfo.iFunctionDes = KNullDesC;
    iTestMacroInfo.iLine = 0;
    iTestMacroInfo.iReceivedError = 0;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestThreadContainer

    Method: StifMacroError

    Description: STIF TF's macros. Saves information for later use.

    Parameters: TInt aMacroType: in: Macro type(0:TL, 1:T1L, 2:T2L, etc.)
                TDesC& aFile: in: Modified file information.
                TDesC& aFunction: in: Modified function information.
                TInt aLine: in: Line information.
                TInt aResult: in: Received result.
                TInt aExpected1: in: Expected result from user.
                TInt aExpected2: in: Expected result from user.
                TInt aExpected3: in: Expected result from user.
                TInt aExpected4: in: Expected result from user.
                TInt aExpected5: in: Expected result from user.
    
    Return Values: Symbian OS error code

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
TInt CTestThreadContainer::StifMacroError( TInt aMacroType,
                                           const TText8* aFile,
                                           const char* aFunction,
                                           TInt aLine,
                                           TInt aResult,
                                           TInt aExpected1,
                                           TInt aExpected2,
                                           TInt aExpected3,
                                           TInt aExpected4,
                                           TInt aExpected5 )
    {
    TStifMacroDes file;
    TStifMacroDes function;

    // Modifies aFile and aFunction lengths if nesessarily.
    // File and function maximun length is KStifMacroMax.
    SetMacroInformation( KStifMacroMax, KStifMacroMax, 
                            aFile, aFunction, file, function );

    // Log more information to file and rdebug
    switch( aMacroType )
        {
        case 0: // TL macro
            {
            __TRACEI( KError, ( CStifLogger::ERed, 
                _L( "FAIL: STIF TF's macro. FILE[%S], FUNCTION[%S], LINE[%d]" ),
                &file, &function, aLine ) );
            RDebug::Print( 
                _L( "FAIL: STIF TF's macro. FILE[%S], FUNCTION[%S], LINE[%d]" ),
                &file, &function, aLine );
            break;
            }
       case 1: // T1L macro
            {
           __TRACEI( KError, ( CStifLogger::ERed,
                _L( "FAIL: STIF TF's macro. RECEIVED[%d], EXPECTED[%d], FILE[%S], FUNCTION[%S], LINE[%d]" ),
                aResult, aExpected1, &file, &function, aLine ) );
            RDebug::Print( 
                _L( "FAIL: STIF TF's macro. RECEIVED[%d], EXPECTED[%d], FILE[%S], FUNCTION[%S], LINE[%d]" ),
                aResult, aExpected1, &file, &function, aLine );
            break;
            }
       case 2: // T2L macro
            {
            __TRACEI( KError, ( CStifLogger::ERed,
                _L( "FAIL: STIF TF's macro. RECEIVED[%d], EXPECTED[%d], EXPECTED[%d], FILE[%S], FUNCTION[%S], LINE[%d]" ),
                aResult, aExpected1, aExpected2, &file, &function, aLine ) );
            RDebug::Print( 
                _L( "FAIL: STIF TF's macro. RECEIVED[%d], EXPECTED[%d], EXPECTED[%d], FILE[%S], FUNCTION[%S], LINE[%d]" ),
                aResult, aExpected1, aExpected2, &file, &function, aLine );
            break;
            }
       case 3: // T3L macro
            {
            __TRACEI( KError, ( CStifLogger::ERed,
                _L( "FAIL: STIF TF's macro. RECEIVED[%d], EXPECTED[%d], EXPECTED[%d], EXPECTED[%d], FILE[%S], FUNCTION[%S], LINE[%d]" ),
                aResult, aExpected1, aExpected2, aExpected3, &file, &function, aLine ) );
            RDebug::Print( 
                _L( "FAIL: STIF TF's macro. RECEIVED[%d], EXPECTED[%d], EXPECTED[%d], EXPECTED[%d], FILE[%S], FUNCTION[%S], LINE[%d]" ),
                aResult, aExpected1, aExpected2, aExpected3, &file, &function, aLine );
            break;
            }
       case 4: // T4L macro
            {
            __TRACEI( KError, ( CStifLogger::ERed,
                _L( "FAIL: STIF TF's macro. RECEIVED[%d], EXPECTED[%d], EXPECTED[%d], EXPECTED[%d], EXPECTED[%d], FILE[%S], FUNCTION[%S], LINE[%d]" ),
                aResult, aExpected1, aExpected2, aExpected3, aExpected4, &file, &function, aLine ) );
            RDebug::Print( 
                _L( "FAIL: STIF TF's macro. RECEIVED[%d], EXPECTED[%d], EXPECTED[%d], EXPECTED[%d], EXPECTED[%d], FILE[%S], FUNCTION[%S], LINE[%d]" ),
                aResult, aExpected1, aExpected2, aExpected3, aExpected4, &file, &function, aLine );
            break;
            }
       case 5: // T5L macro
            {
            __TRACEI( KError, ( CStifLogger::ERed,
                _L( "FAIL: STIF TF's macro. RECEIVED[%d], EXPECTED[%d], EXPECTED[%d], EXPECTED[%d], EXPECTED[%d], EXPECTED[%d], FILE[%S], FUNCTION[%S], LINE[%d]" ),
                aResult, aExpected1, aExpected2, aExpected3, aExpected4, aExpected5, &file, &function, aLine ) );
            RDebug::Print( 
                _L( "FAIL: STIF TF's macro. RECEIVED[%d], EXPECTED[%d], EXPECTED[%d], EXPECTED[%d], EXPECTED[%d], EXPECTED[%d], FILE[%S], FUNCTION[%S], LINE[%d]" ),
                aResult, aExpected1, aExpected2, aExpected3, aExpected4, aExpected5, &file, &function, aLine );
            break;
            }
        default: // default, faulty handling
            {
            __TRACEI( KError, ( CStifLogger::EError,
                _L( "CTestThreadContainer::StifMacroError(): Macro faulty handling(Macro type is incorrect)" ) ) );
            RDebug::Print( 
                _L( "ERROR: CTestThreadContainer::StifMacroError(): Macro faulty handling(Macro type is incorrect)" ) );
            return KErrArgument; // Test case goes to crashed category
            }        
        }

    // Modifies aFile and aFunction lengths if nesessarily.
    // File maximun length is KStifMacroMaxFile.
    // Function maximun length is KStifMacroMaxFunction.
    SetMacroInformation( KStifMacroMaxFile, KStifMacroMaxFunction, 
                            aFile, aFunction, file, function );

    // Set information for later use(this information is
    // limited and can be seen in UI)
    iTestMacroInfo.iIndication = ETrue;
    iTestMacroInfo.iFileDes = file;
    iTestMacroInfo.iFunctionDes = function;
    iTestMacroInfo.iLine = aLine;
    if( aResult == KErrNone )
        {
        // aResult is KErrNone. TL macro is used or expected result(s) are/is
        // negative value(s). Received error code is mapped to KErrArgument
        // because this is erronous case.
        iTestMacroInfo.iReceivedError = KErrArgument;
        }
    else
        {
        iTestMacroInfo.iReceivedError = aResult;
        }

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestThreadContainer

    Method: SetMacroInformation

    Description: Modifies aRecFile and aRecFunction lengths if nesessarily.

    Parameters: TInt aMaxLength: in: Maximum length of file information.
                TInt aMaxLength: in: Maximum length of function information.
                const TText8* aRecFile: in: Received file information.
                char* aRecFunction: in: Received function information.
                TDes& aFile: inout: Modified file.
                TDes& aFunction: inout: Modified function.

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CTestThreadContainer::SetMacroInformation( TInt aFileMaxLength,
                                                TInt aFuntionMaxLength,
                                                const TText8* aRecFile,
                                                const char* aRecFunction,
                                                TDes& aFile,
                                                TDes& aFunction )
    {
    // Create 8 to 16
    TPtrC8 buf_file;
    buf_file.Set( aRecFile );
    // File description length is limited. Extracts the rightmost part of the
    // data.
    aFile.Copy( buf_file.Right( aFileMaxLength ) );
    aFile.LowerCase();

    if( aRecFunction )
        {
        // Create 8 to 16
        TPtrC8 buf_func;
        buf_func.Set( (const unsigned char*)aRecFunction );
        // Function description length is limited. Extracts the leftmost part
        // of the data.
        aFunction.Copy( buf_func.Left( aFuntionMaxLength ) );
        aFunction.LowerCase();
        }
    else
        {
        // Function is not given(WINS)
        aFunction.Copy( _L( "-" ) );
        }

    }

/*
-------------------------------------------------------------------------------

    Class: CTestThreadContainer

    Method: AddInterferenceThread

    Description: With this can be store information about test interference
                 thread to client space.

    Parameters: RThread aSTIFTestInterference: in: Thread information to store

    Return Values: TInt: Symbian OS error code.

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
TInt CTestThreadContainer::AddInterferenceThread( 
                                RThread aSTIFTestInterference )
    {
    // Get access to test interference stuff
    iInterferenceSem.Wait();
    
    iInterferenceMutex.Wait();  // Take mutex to get access to server thread.
                                // Between Wait and Signal is critical section
                                // and this verifies that iInterferenceSem and
                                // RequestComplete is done successfully.
    
    // Get status variable from server
    TRequestStatus* status = 
        TestExecution().GetRq( CTestExecution::ERqInterference );
    
    if( status == NULL )
        {
        iInterferenceMutex.Signal();
        Panic( ENullRequest );
        return KErrNone;
        }

    if( *status != KRequestPending )
        {
        // CInterferenceHandler::DoCancel called before getting here,
        // just return
        iInterferenceMutex.Signal();
        return KErrNone;    
        }

    // Add thread to Array. Via array can handle test interference thread's
    // kill in panic etc. cases
    TTestInterference& testInterface = TestExecution().TestInterference();
    testInterface.iThreadId = aSTIFTestInterference.Id();    
    testInterface.iOperation = TTestInterference::EAppend;

    // Complete action to server
    iServerThread.RequestComplete( status, KErrNone );
    // Goes to CInterferenceHandler::RunL()

    iInterferenceMutex.Signal();

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestThreadContainer

    Method: RemoveInterferenceThread

    Description: With this can be remove information about test interference
                 thread from client space.

    Parameters:  RThread aSTIFTestInterference: in: Thread information to store

    Return Values: TInt: Symbian OS error code.

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
TInt CTestThreadContainer::RemoveInterferenceThread( 
                                RThread aSTIFTestInterference )
    {
    // Get access to test interference stuff
    iInterferenceSem.Wait();
    
    iInterferenceMutex.Wait();  // Take mutex to get access to server thread.
                                // Between Wait and Signal is critical section
                                // and this verifies that iInterferenceSem and
                                // RequestComplete is done successfully.
    
    // Get status variable from server
    TRequestStatus* status = 
        TestExecution().GetRq( CTestExecution::ERqInterference );
    
    if( status == NULL )
        {
        iInterferenceMutex.Signal();
        Panic( ENullRequest );
        return KErrNone;
        }

    if( *status != KRequestPending )
        {
        // CInterferenceHandler::DoCancel called before getting here, just return
        iInterferenceMutex.Signal();
        return KErrNone;    
        }

    // Add thread to Array. Via array can handle test interference thread's
    // kill in panic etc. cases
    TTestInterference& testInterface = TestExecution().TestInterference();
    testInterface.iThreadId = aSTIFTestInterference.Id();    
    testInterface.iOperation = TTestInterference::ERemove;

    // Complete action to server
    iServerThread.RequestComplete( status, KErrNone );
    // Goes to CInterferenceHandler::RunL()

    iInterferenceMutex.Signal();

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestThreadContainer

    Method: HandleMeasurementProcess

    Description: With this can be stored information about test measurement
                 to TestServer space.

    Parameters: CSTIFTestMeasurement::TMeasurement aSTIFMeasurementInfo: in:
                Struct for measurement information.

    Return Values: TInt: Symbian OS error code.

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CTestThreadContainer::HandleMeasurementProcess( 
            const CSTIFTestMeasurement::TStifMeasurementStruct& aSTIFMeasurementInfo )
    {
    // Get access to test measurement stuff

    // This is syncronous operation and other request cannot executed at the
    // same time. In this case iMeasurementSem is not signaled in StarL().
    // So iMeasurementSem.Wait(); is not needed in this case.

    iMeasurementMutex.Wait();   // Take mutex to get access to server thread.
                                // Between Wait and Signal is critical section
                                // and this verifies that iMeasurementSem and
                                // RequestComplete is done successfully.

    // Get status variable from server
    TRequestStatus* status = 
        TestExecution().GetRq( CTestExecution::ERqMeasurement );

    if( status == NULL )
        {
        iMeasurementMutex.Signal();
        Panic( ENullRequest );
        return KErrNone;
        }

    if( *status != KRequestPending )
        {
        // CMeasurementHandler::DoCancel called before getting here,
        // just return
        iMeasurementMutex.Signal();
        return KErrNone;
        }

    TTestMeasurement& testmeasurement = TestExecution().TestMeasurement();
    testmeasurement.iMeasurementStruct = aSTIFMeasurementInfo;

    // Complete action to server
    iServerThread.RequestComplete( status, KErrNone );
    // Goes to CMeasurementHandler::RunL()

    // Make this synchronous and block until needed operations are done.
    iMeasurementSem.Wait();
    // This continue here when iMeasurementSem.Signal is said in 
    // CMeasurementHandler::RunL(). So when measurement operations are done.

    // Error code from measurement related operations
    TInt ret( testmeasurement.iMeasurementStruct.iOperationResult );

    iMeasurementMutex.Signal();

    return ret;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestThreadContainer

    Method: SetEventReq

    Description: Sets asynchronous event request.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CTestThreadContainer::DoNotifyCommand(TCommand aCommand, const TDesC8& aParamsPckg)
    {
    // Get access to command stuff
    iCommandSem.Wait();

    iCommandMutex.Wait(); // Take mutex to get access to server thread.
                          // Between Wait and Signal is critical section and this
                          // verifies that iCommandSem and RequestComplete is done
                          // successfully.

    // Get status variable from server
    TRequestStatus* status = TestExecution().GetRq(CTestExecution::ERqCommand);

    if(status == NULL)
        {
        iCommandMutex.Signal();
        Panic(ENullRequest);
        return;
        }
    if(*status != KRequestPending)
        {
        iCommandMutex.Signal();
        return;
        }

    // Fill in information
    CCommandDef& aDef = TestExecution().CommandDef();
    aDef.iCommand = aCommand;
    aDef.iParamsPckg.Copy(aParamsPckg);

    // Complete action to server
    iServerThread.RequestComplete(status, KErrNone);

    iCommandMutex.Signal();

    }

/*
-------------------------------------------------------------------------------

    Class: CTestThreadContainer

    Method: GetTestCaseTitleL

    Description: Gets title of currently running test.

    Parameters: aTestCaseTitle: OUT: test case title descriptor

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CTestThreadContainer::GetTestCaseTitleL(TDes& aTestCaseTitle)
    {
    ModuleContainer().GetTestCaseTitleL(aTestCaseTitle); 
    }

/*
-------------------------------------------------------------------------------

    Class: CTestThreadContainer

    Method: SetThreadLogger

    Description: Sets thread logger.

    Parameters: CStifLogger* aThreadLogger: in: Pointer to thread logger.

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CTestThreadContainer::SetThreadLogger( CStifLogger* aThreadLogger )
	{
	
	iThreadLogger = aThreadLogger;
	}

/*
-------------------------------------------------------------------------------

    Class: CTestThreadContainer

    Method: SetThreadLogger

    Description: Gets thread logger.

    Parameters: None

    Return Values: Pointer to CStifLogger. 

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
CStifLogger* CTestThreadContainer::GetThreadLogger()
	{
	
	return iThreadLogger;
	}

/*
-------------------------------------------------------------------------------

    Class: CTestThreadContainer

    Method: UITesting

    Description: Gets information if testserver supports UI testing.

    Parameters: None

    Return Values: True if testserver supports UI testing, False if testserver
    			   doesn't support UI testing. 

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
EXPORT_C TBool CTestThreadContainer::UITesting()
	{
	
	return iModuleContainer->GetTestModule()->GetTestServer()->UiTesting();	
	}

/*
-------------------------------------------------------------------------------

    Class: CTestThreadContainer

    Method: GetUiEnvProxy

    Description: Gets UIEnvProxy.

    Parameters: None

    Return Values: Pointer to UIEnvProxy 

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
EXPORT_C CUiEnvProxy* CTestThreadContainer::GetUiEnvProxy()
	{
	
	return iModuleContainer->GetTestModule()->GetTestServer()->GetUiEnvProxy();
	}

// End of File
