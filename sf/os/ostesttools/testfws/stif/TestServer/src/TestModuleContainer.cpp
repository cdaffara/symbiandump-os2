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
* CTestModuleContainer class member functions. CTestModuleContainer 
* class contains interface * to execute various functions in context 
* of test execution thread.
*
* CTestModuleContainer is the owner of the test execution thread 
* object and the owner of the test module instance.
*
*/

// INCLUDE FILES
#include <e32std.h>
#include <e32svr.h>
#include <e32uid.h>
#include "TestEngineClient.h"
#include <StifTestModule.h>
#include <stifinternal/TestServerClient.h>
#include "TestServer.h"
#include "TestServerModuleIf.h"
#include "TestServerCommon.h"
#include "PrintQueue.h"
#include "TestThreadContainer.h"
//--PYTHON-- begin
#include "StifPython.h"
//--PYTHON-- end

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// The test module execution thread function
TInt ExecutionThread( TAny* aParams );

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES
typedef TInt( *CTestInterfaceFactoryTestModule )( CTestModuleParam*&, TUint32& );

// FORWARD DECLARATIONS

// ==================== LOCAL FUNCTIONS =======================================

// ================= MEMBER FUNCTIONS =========================================


/*
-------------------------------------------------------------------------------

    Class: CTestModuleContainer

    Method: NewL

    Description: Returns new CTestModuleContainer instance.

    Parameters: const TDesC& aName: in: Module name
                CTestModule* aSession: in: "Parent"
                const TDesC& aConfig: in: Test case (config) file name.

    Return Values: CTestModuleContainer* New instance

    Errors/Exceptions: Function leaves if memory allocation fails or
                       CTestModuleContainer ConstructL leaves.
                       Panics if aSession is NULL.

    Status: Proposal

-------------------------------------------------------------------------------
*/
CTestModuleContainer* CTestModuleContainer::NewL( const TDesC& aName,
                                                  CTestModule* aSession,
                                                  const TDesC& aConfig )
    {

    __ASSERT_ALWAYS ( aSession, 
                      CTestServer::PanicServer( ENullTestModule ) );

    CTestModuleContainer* self = new ( ELeave ) CTestModuleContainer();
    CleanupStack::PushL( self );
    self->ConstructL( aName, aSession, aConfig );
    CleanupStack::Pop( self );
    return self;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleContainer

    Method: ConstructL

    Description: Second level constructor.

    Function creates the execution thread, creates exception
    handler and the thread undertaker for it and then resumes the thread.

    Parameters: const TDesC& aName: in: Module name
                CTestModule* aSession: in: "Parent"
                const TDesC& aConfig: in: Test case (config) file name.

    Return Values: CTestModuleContainer* New instance

    Errors/Exceptions: Function leaves if thread creation fails,
                       memory allocation fails or undertaker creation leaves.

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
void CTestModuleContainer::ConstructL( const TDesC& aName,
                                       CTestModule* aSession,
                                       const TDesC& aConfig )
    {

    RDebug::Print( aName );
    
    iNestedActiveScheduler = new (ELeave) CActiveSchedulerWait;
    
    iCTestModule = aSession;

    // Create error print handler
    iErrorPrintHandler = CErrorPrintHandler::NewL( *this );
    User::LeaveIfError( iErrorPrintSem.CreateLocal( 0 ) );

    // Construct unique thread name
    const TInt KAddressLen = 8;
    const TInt KMaxName = 50;  // Something smaller than max thread name
    TInt len = aName.Length();
    if ( KAddressLen + len > KMaxName )
        {
        len = KMaxName - KAddressLen;
        }
    
    // Default heap and stack sizes that may configure by user.
    TInt heapMinSize( KTestThreadMinHeap );
    TInt heapMaxSize( KTestThreadMaxHeap );
    TInt stackSize( KStackSize );

    // Check is user give heap or stack sizes and verify sizes.
    GetTestModuleParams( aName, aConfig, stackSize, heapMinSize, heapMaxSize );

    // Create a new thread
    TInt ret = KErrAlreadyExists;
    for( TInt i = 0; ret == KErrAlreadyExists; i++ )
        {
        TName threadName = aName.Left( len );
        threadName.AppendFormat( _L( "%x" ), ( TUint32 ) this );
        threadName.AppendFormat( _L( "%x" ), i );
        //RDebug::Print( threadName );

		RDebug::Print(_L("CTestModuleContainer::ConstructL create thread=[%S] stackSize=(%d)"), &threadName, stackSize);

		if ( iCTestModule->GetTestServer()->UiTesting() == false )
			{
	        ret = iThread.Create(
	                    threadName,                 // name of thread
	                    CTestThreadContainer::ExecutionThread,// thread function
	                    stackSize,                  // Stack size
	                    heapMinSize,                // Heap sizes
	                    heapMaxSize,
	                    this                        // Parameter to thread function
	                    );
			}
		else
			{
	        ret = iThread.Create(
                    threadName,                 // name of thread
                    CTestThreadContainer::UIExecutionThread,// thread function
                    stackSize,                  // Stack size
                    heapMinSize,                // Heap sizes
                    heapMaxSize,
                    this                        // Parameter to thread function
                    );
			}

        // If test execution thread exist try to create new one
        if( ret == KErrAlreadyExists )
            {
            RDebug::Print( _L( "Thread[%S] allready exist, creating new one" ), &threadName );
            __TRACE ( KInit, ( CStifLogger::ERed, _L( "Thread[%S] allready exist, creating new one" ), &threadName ) );
            }

        }

    // If thread creation fails, leave here.
    if( ret != KErrNone )
        {
        RDebug::Print( _L( "Execution thread creation fails with: [%d]" ), ret );
        __TRACE ( KError, ( CStifLogger::ERed, _L( "Execution thread creation fails with: [%d]" ), ret ) );
        User::Leave( ret );
        }

    // Doing nothing
    // ChangeOperationNoError ( ESuspend, 0 );
    // Thread suspends always first by default
    
    // Construct the undertaker to get notifications about the 
    // thread death. 
    // This must be trapped to allow to kill just created thread
    // nicely without a mess in destructor.
    TRAPD ( r, 
        iUnderTaker = CUnderTaker::NewL ( this );
        CActiveScheduler::Add( iUnderTaker );
        iUnderTaker->StartL();
        );    

    // Can't create or start the undertaker. Kill the thread. 
    // Undertaker will be deleted in destructor if needed.
    if ( r != KErrNone )
        {
        iThread.Kill ( r );
        iThread.Close();
        User::Leave( r );
        }

    // Get server thread id ( i.e current thread id )
    RThread tmpThread;
    iServerThreadId = tmpThread.Id();
    
    // Resume the thread
    iThread.Resume();                // Start the thread
    iUpAndRunning = ETrue;           // Thread is created 

    // Add "this" container to active scheduler
    CActiveScheduler::Add ( this );

    // Start error print handler
    iErrorPrintHandler->StartL();

    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleContainer

    Method: CTestModuleContainer

    Description: Constructor.

    Initialise semaphores.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
CTestModuleContainer::CTestModuleContainer() :
    CActive( CActive::EPriorityStandard ),
    iOperationName ( 0, 0 ),
    iIsPaused( EFalse )
    {

    // Create operation start semaphore
    iOperationStartSemaphore.CreateLocal( 0 );

    // Operation can be changed without first waiting.
    iOperationChangeSemaphore.CreateLocal( 1 );

    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleContainer

    Method: ~CTestModuleContainer

    Description: Destructor

    Delete the test execution thread, if it is up and running. Delete
    undertaker and close handles.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
CTestModuleContainer::~CTestModuleContainer()
    {

    __TRACE ( KThreadOperation, ( _L( "CTestModuleContainer::~CTestModuleContainer in" ) ) );

    if ( iUpAndRunning )
        {
		if( iUnderTaker != NULL )
			{
			iUnderTaker->Cancel();
			}

        // Set the operation
        ChangeOperationNoError ( EExit, 0 );

        // Do operation
        StartAndWaitOperation();

        }
    
    delete iErrorPrintHandler;
	iErrorPrintHandler = NULL;

    // Delete heap descriptor
    delete iModuleNameBuffer;
    iModuleNameBuffer = NULL;

    // Delete undertaker
    delete iUnderTaker;
    iUnderTaker = NULL;

    // Close error semaphore
    if ( iErrorPrintSem.Handle() != 0 ) iErrorPrintSem.Close();

    // Close the semaphores
    iOperationStartSemaphore.Close();
    iOperationChangeSemaphore.Close();

    // Close handle to thread
    iThread.Close();

    Cancel();

    delete iNestedActiveScheduler;
    iNestedActiveScheduler = NULL;

    __TRACE ( KThreadOperation, ( _L( "CTestModuleContainer::~CTestModuleContainer out" ) ) );

    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleContainer

    Method: StartAndWaitOperation

    Description: Signals operation and waits until operation is completed.

    Function executes active scheduler locally to handle both request
    completion from execution thread and thread undertaker operations.    

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
TInt CTestModuleContainer::StartAndWaitOperation()
    {

    // Change undertaker mode to synchronous    
    iUnderTaker->SetSynchronousMode( ETrue );

    iStatus = KRequestPending;
    // Get ready to handle operation completion
    SetActive();

    // Start operation
    iOperationStartSemaphore.Signal();

    // Start "nested" active scheduler "locally"
    //CActiveScheduler::Start();
    iNestedActiveScheduler->Start();

    // Request completed or operation crashed, everything 
    // is done in ModuleContainer's RunL function or in UnderTaker's RunL.

    // Cancel the original request in case than it was the undertaker
    // that stopped the active scheduler. Does not do anything if original
    // request is already finished.
    Cancel();
    
    // Undertaker back to "normal" mode
    iUnderTaker->SetSynchronousMode ( EFalse );

    return iModuleResult;

    }



/*
-------------------------------------------------------------------------------

    Class: CTestModuleContainer

    Method: Initialise

    Description: Initialise the test module

    Initialisation is done in context of test execution thread.

    Parameters: const TDesC& aName            :in:  Module name
                TBool aFirstTime              :in:  First init?


    Return Values: TInt                             Result from init

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
TInt CTestModuleContainer::Initialize( const TFileName& aName,
                                       TBool aFirstTime
                                     )
    {

    __TRACE ( KInit, ( _L( "Initializing test module" ) ) );

    // Set the operation
    iErrorResult = ChangeOperation ( EInitializeModule, aName, aFirstTime );
    iModuleResult = KErrNone;
    
    if ( iErrorResult == KErrNone )
        {
        // Wait until operation is completed
        StartAndWaitOperation();
        }
    
    if ( iErrorResult )
        {
        __TRACE ( KError, ( CStifLogger::ERed,  _L( "Initializing test module failed %d" ), iErrorResult ) );
        }
    else if ( iModuleResult )
        {
        __TRACE ( KError, ( CStifLogger::ERed, _L( "Initializing test module failed code from module = %d" ), iModuleResult ) );
        } 
    else
        {
        __TRACE ( KInit, ( CStifLogger::EBold, _L( "Test module initialization done" ) ) );
        }

    // Return result
    return iModuleResult;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleContainer

    Method: EnumerateTestCases

    Description: Enumerate test cases of the test module

    Enumeration is done in context of test execution thread.

    Parameters: const TDesC& aName            :in:  Configuration file

    Return Values: TInt                             Result from enumeration

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
TInt CTestModuleContainer::EnumerateTestCases( const TFileName& aName )
    {

    __TRACE ( KInit, ( CStifLogger::EBold, _L( "Enumerating test cases from [%S] (might be empty)" ), &aName ) );

    // Set the operation
    iErrorResult = ChangeOperation( EEnumerateInThread, aName, 0 );
    iModuleResult = KErrNone;

    if ( iErrorResult == KErrNone )
        {
        // Wait until operation is completed
        StartAndWaitOperation();
        }

    if ( iErrorResult )
        {
        __TRACE ( KError, ( CStifLogger::ERed, _L( "Enumerating test cases failed %d" ), iErrorResult ) );
        }
    else if ( iModuleResult )
        {
        __TRACE ( KError, ( CStifLogger::ERed, _L( "Enumerating test cases failed, code from module = %d" ), iModuleResult ) );
        } 
    else
        {
        __TRACE ( KInit, ( _L( "Enumerating test cases done" ) ) );
        }

    // Return result
    return iModuleResult;

    }



/*
-------------------------------------------------------------------------------

    Class: CTestModuleContainer

    Method: FreeEnumerationData

    Description: Frees the enumeration data

    Memory deallocation is done in context of test execution thread.

    Parameters: None

    Return Values: TInt                             Result from operation

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/

TInt CTestModuleContainer::FreeEnumerationData()
    {

    // Set the operation
    ChangeOperationNoError( EFreeEnumerationData, 0 );
    
    // Do the operation
    StartAndWaitOperation();

    // Return result
    return iModuleResult;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleContainer

    Method: RunTestCase

    Description: Run a test case

    Running a test case is done in context of test execution thread. This
    function is asynchronous ( i.e test is not finished when this function
    returns ).

    Parameters: const TFileName& aName        :in:  Configuration file
                TInt aCaseNumber              :in:  Case number
                const RMessage                :in:  Handle to test msg.

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CTestModuleContainer::RunTestCase ( const TFileName& aName,
                                         const TInt aCaseNumber,
                                         const RMessage2& aMessage
                                       )
    {

    __TRACE ( KInit, ( CStifLogger::EBold, _L( "Running test case [%S], case [%d]" ), &aName, aCaseNumber ) );

    // Set the operation    
    iErrorResult = ChangeOperation( EExecuteTestInThread, aName, aCaseNumber );

    if ( iErrorResult != KErrNone )
        {
        aMessage.Complete ( iErrorResult );
        return;
        }    

    // Set data
    iMessage = aMessage;

    iStatus = KRequestPending;
    // Get ready to handle operation completion
    SetActive();

    // Do operation
    iOperationStartSemaphore.Signal();    

    // This is asynchronous operation, 
    // do not wait until completed.    

    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleContainer

    Method: SetExecutionSubSession

    Description: Set execution subsession

    Sets module container test execution subsession. Execution subsession
    is changed when a module container is reused to run another 
    test case.

    Parameters: CTestExecution* aExecution    :in:  Subsession pointer
    
    Return Values: None

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
void CTestModuleContainer::SetExecutionSubSession( CTestExecution* aExecution )
    {

    iCTestExecution = aExecution;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleContainer

    Method: PauseThread

    Description: Pauses the thread

    Parameters: None
    
    Return Values: None

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
TInt CTestModuleContainer::PauseThread()
    {
    
    if( iThread.ExitType() != EExitPending )
        {
        // Thread is not alive anymore
        return KErrDied;
        }

    if(iCTestExecution != NULL)
    	{
		RMutex printMutex;
		printMutex.SetHandle( iCTestExecution->PrintMutexHandle() );
		RMutex eventMutex;
		eventMutex.SetHandle( iCTestExecution->EventMutexHandle() );
		RMutex sndMutex;
		sndMutex.SetHandle( iCTestExecution->SndMutexHandle() );
		RMutex rcvMutex;
		rcvMutex.SetHandle( iCTestExecution->RcvMutexHandle() );
		
		printMutex.Wait(); // wait that print operation is done
		eventMutex.Wait(); // wait that event operation is done        
		sndMutex.Wait(); // wait that and operation is done
		rcvMutex.Wait(); // wait that event operation is done        
			
		iThread.Suspend();
		
		printMutex.Signal();
		eventMutex.Signal();
		sndMutex.Signal();
		rcvMutex.Signal();
    	}
    else
		{
		__TRACE( KError,( _L( "STIF internal error - iCTestExecution is NULL in CTestModuleContainer::PauseThread" ) ) );
		User::Panic(_L("NULL pointer exception"), KErrGeneral);
		}
    
    iIsPaused = ETrue;

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleContainer

    Method: ResumeThread

    Description: Resumes the thread

    Parameters: None
    
    Return Values: None

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
TInt CTestModuleContainer::ResumeThread()
    {

    if( iThread.ExitType() != EExitPending )
        {
        // Thread is not alive anymore
        return KErrDied;
        }
    
    iThread.Resume();

    // Pause() - Resume() operations done successfully. 
    iIsPaused = EFalse;

    return KErrNone;
    
    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleContainer

    Method: KillThread

    Description: Kills the thread

    Parameters: const TInt aCode              :in:   Kill code

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
void CTestModuleContainer::KillThread( const TInt aCode )
    {

    iUpAndRunning = EFalse;
    iUnderTaker->Cancel();
    
    if(iCTestExecution != NULL)
    	{
		RMutex printMutex;
		printMutex.SetHandle( iCTestExecution->PrintMutexHandle() );
		RMutex eventMutex;
		eventMutex.SetHandle( iCTestExecution->EventMutexHandle() );
		RMutex sndMutex;
		sndMutex.SetHandle( iCTestExecution->SndMutexHandle() );
		RMutex rcvMutex;
		rcvMutex.SetHandle( iCTestExecution->RcvMutexHandle() );
		
		printMutex.Wait(); // wait that print operation is done
		eventMutex.Wait(); // wait that event operation is done        
		sndMutex.Wait(); // wait that snd operation is done
		rcvMutex.Wait(); // wait that event operation is done        
			
		iThread.Kill ( aCode );
		
		printMutex.Signal();
		eventMutex.Signal();
		sndMutex.Signal();
		rcvMutex.Signal();
    	}
    else
		{
		__TRACE( KError,( _L( "STIF internal error - iCTestExecution is NULL in CTestModuleContainer::KillThread" ) ) );
		User::Panic(_L("NULL pointer exception"), KErrGeneral);
		}

    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleContainer

    Method: ChangeOperation

    Description: Multithread safe way to change operation

    Parameters: const TOperation aOperation   :in:  Operation type
                const TFileName* aNameBuffer  :in:  Filename ( or NULL )
                const TInt aInt               :in:  Operation specific int
    
    Return Values: TInt                             Error code

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
TInt CTestModuleContainer::ChangeOperation( const TOperation aOperation,
                                            const TFileName& aNameBuffer,
                                            const TInt aInt )
    {

    TInt ret = KErrNone;

    iOperationChangeSemaphore.Wait();

    iErrorResult  = KErrNone;
    iModuleResult = KErrNone;

    TFileName newNameBuffer;
    // Check is TestScripter
    TInt check = CheckModuleName( aNameBuffer, newNameBuffer );
    delete iModuleNameBuffer;
	iModuleNameBuffer = NULL;
    if( check == KErrNone )
        {
        // Construct heap buffer for configuration file
        TRAP( ret, iModuleNameBuffer = HBufC::NewL( newNameBuffer.Length() ) );

        if( ret == KErrNone )
            {
            iOperationName.Set ( iModuleNameBuffer->Des() );
            iOperationName.Copy ( newNameBuffer );
        
            iOperationIntBuffer = aInt;
            iOperationType = aOperation;
            }
        else
            {
            __TRACE ( KError, ( CStifLogger::ERed, _L( "CTestModuleContainer::ChangeOperation NoMemory" ) ) );
            }   
        }
    else
        {
        // Construct heap buffer for configuration file
        TRAP( ret, iModuleNameBuffer = HBufC::NewL( aNameBuffer.Length() ) );

        if( ret == KErrNone )
            {
            iOperationName.Set ( iModuleNameBuffer->Des() );
            iOperationName.Copy ( aNameBuffer );
        
            iOperationIntBuffer = aInt;
            iOperationType = aOperation;
            }
        else
            {
            __TRACE ( KError, ( CStifLogger::ERed, _L( "CTestModuleContainer::ChangeOperation NoMemory" ) ) );
            }
        }

    iOperationChangeSemaphore.Signal();

    return ret;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleContainer

    Method: ChangeOperationNoError

    Description: Multithread safe way to change operation. This version
    of function can't fail.

    Parameters: const TOperation aOperation   :in:  Operation type
                const TInt aInt               :in:  Operation specific int
    
    Return Values: None

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
void CTestModuleContainer::ChangeOperationNoError( const TOperation aOperation,
                                                   const TInt aInt
                                                 )
    {

    iOperationChangeSemaphore.Wait();

    iErrorResult  = KErrNone;
    iModuleResult = KErrNone;
    iOperationIntBuffer = aInt;
    iOperationType = aOperation;
    
    iOperationChangeSemaphore.Signal();

    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleContainer

    Method: Operation

    Description: Multithread safe way to obtain current operation.

    Parameters: None

    Return Values: TBool                            Operation

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
//@spe const CTestModuleContainer::TOperation CTestModuleContainer::Operation()
CTestModuleContainer::TOperation CTestModuleContainer::Operation()
    {
    TOperation operation;
    iOperationChangeSemaphore.Wait();
    operation = iOperationType;
    iOperationChangeSemaphore.Signal();

    return operation;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleContainer

    Method: ModuleResult

    Description: Returns error code from test module

    Parameters: None

    Return Values: TInt                             Result code from module

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
TInt& CTestModuleContainer::ModuleResult()
    {

    return iModuleResult;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleContainer

    Method: OperationErrorResult

    Description: Returns error code from test module

    Parameters: None

    Return Values: TInt                             Result code from module

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
TInt& CTestModuleContainer::OperationErrorResult()
    {

    return iModuleResult;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleContainer

    Method: TestModuleName

    Description: Returns test module name.

    Parameters: None

    Return Values: const TDesC&: test module name

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
const TDesC& CTestModuleContainer::TestModuleName()
    {
    
    return iCTestModule->Name();
    
    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleContainer

    Method: TestModuleName

    Description: Returns test module name.

    Parameters: None

    Return Values: const TDesC&: test module name

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
const TDesC& CTestModuleContainer::TestModuleIniFile()
    {
    
    return iCTestModule->IniName();
    
    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleContainer

    Method: ModuleResult

    Description: Returns constant pointer to test case array

    Parameters: None
    
    Return Values: const RPointerArray<TTestCaseInfo>*  Test cases

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
const RPointerArray<TTestCaseInfo>* CTestModuleContainer::TestCases() const
    {
    
    if( !iUpAndRunning )
        {
        return NULL;
        }
        
    if( iThreadContainer == NULL )
        {
        CTestServer::PanicServer( ENullTestThreadContainer );
        }
    return iThreadContainer->TestCases();

    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleContainer

    Method: Complete

    Description: Completes operation from any thread

    Parameters: const TInt aCompletionCode :in:     Completion code
    
    Return Values: None

    Errors/Exceptions: Function panics if server's thread can't be opened.

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CTestModuleContainer::Complete( const TInt aCompletionCode )
    {

    TRequestStatus* req = &iStatus;
    User::RequestComplete( req, aCompletionCode );
    
    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleContainer

    Method: DoErrorPrint

    Description: Handle error prints.
        
    Parameters: None
    
    Return Values: None

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
void CTestModuleContainer::DoErrorPrint()
    {    
    
    iCTestModule->ErrorPrint( iErrorPrint.iPriority, iErrorPrint.iText );
    
    }
 
/*
-------------------------------------------------------------------------------

    Class: CTestModuleContainer

    Method: KillTestinterferenceThread

    Description: Make sure that any of the test interference thread's won't
                 stay to run if test case is crashed of test interference
                 object is not deleted.

    Parameters: None
    
    Return Values: None

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
void CTestModuleContainer::KillTestinterferenceThread()
    {
    if (iCTestExecution != NULL)
    	{
    	iCTestExecution->KillTestinterferenceThread();
    	}
    else
    	{
    	__TRACE( KError,( _L( "STIF internal error - iCTestExecution is NULL in CTestModuleContainer::KillTestinterferenceThread" ) ) );
    	User::Panic(_L("NULL pointer exception"), KErrGeneral);
    	}
    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleContainer

    Method: KillTestMeasurement

    Description: Make sure that any of the test measurement process's won't
                 stay to run if test case is crashed of test measurement object
                 is not deleted.

    Parameters: None
    
    Return Values: None

    Errors/Exceptions: None

    Status: Approved
    
-------------------------------------------------------------------------------
*/
void CTestModuleContainer::KillTestMeasurement()
    {    
    if (iCTestExecution != NULL)
    	{
    	iCTestExecution->KillTestMeasurement();
    	}
    else
    	{
    	__TRACE( KError,( _L( "STIF internal error - iCTestExecution is NULL in CTestModuleContainer::KillTestMeasurement" ) ) );
    	User::Panic(_L("NULL pointer exception"), KErrGeneral);
    	}
    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleContainer

    Method: GetRequest

    Description: Return status flags.
        
    Parameters: TRequestType aType: in: request type
    
    Return Values: TRequestStatus*: pointer to TRequestStatus

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
TRequestStatus* CTestModuleContainer::GetRequest( TRequestType aType )
    { 
    
    TRequestStatus* status = NULL;
    
    switch( aType )
        {
        case ERqTestCase:
            status = &iStatus;
            break;
        case ERqErrorPrint:
            status = &iErrorPrintHandler->iStatus;
            break;
        default: 
            break;
        }
    
    return status;

    }
    
/*
-------------------------------------------------------------------------------

    Class: CTestModuleContainer

    Method: RunL

    Description: Stops active scheduler when operation in another thread
                 is completed.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CTestModuleContainer::RunL()
    {
    
    if( iOperationType == CTestModuleContainer::EExecuteTestInThread )
        {
        // Make sure that any of the test interference thread's won't stay
        // to run.
        KillTestinterferenceThread();

        // Make sure that any of the test measurement process's won't stay
        // to run.
        KillTestMeasurement();

        if(iCTestExecution == NULL)
        	{
        	__TRACE( KError,( _L( "STIF internal error - iCTestExecution is NULL in CTestModuleContainer::RunL" ) ) );
        	User::Leave(KErrGeneral);
        	}
        else
        	{
			// Set the thread state
			iCTestExecution->SetThreadState( CTestExecution::EFinished );
	
			 // If test print queue is empty, then complete request with KEof. If Queue
			// is not empty, then do not complete. Queue will be emptied by UI.   
			iCTestExecution->CompletePrintRequestIfQueueEmpty();
	
			// Event queue clean-up
			iCTestExecution->CleanupEvents();
			
			// Complete the test request
			iCTestExecution->CompleteTestExecution( iStatus.Int() );
			
			// Test case execution OK and test module thread's pause operation
			// is not ongoing. Re-use old test module execution thread
			if( iCTestExecution->TestThreadFailure() == CTestExecution::ETestThreadOk && !iIsPaused )
				{
				// Return this module to pool
				__TRACE( KInit, ( _L("Freeing test module container at 0x%x"),  
					(TUint32) this ) );
				iCTestModule->FreeTestModule( this );
				}
			// Problems in test case execution. Delete this test module thread and
			// start next test case in new test module thread.
			else
				{
				if( iIsPaused )
					{
					// Test case is paused by user and this RunL is executed also.
					// This RunL will make Resume() operation to fail because test
					// case is finished. Next test case cannot complete and that is
					// why next test case will start at the "clean table".
					__TRACE( KInit, ( _L( "Delete test module container at 0x%x because test module thread is paused while test case is finished(Cannot Resume())" ), (TUint32) this ) );  
					}
				else
					{
					// delete test module thread if some thread problem has occurred 
					__TRACE( KInit, ( _L("Delete test module container at 0x%x because of thread leak (0x%x)"),  
						(TUint32) this,  iCTestExecution->TestThreadFailure() ) );
					}
				delete this;            
				}
        	}
        }
    else
        {
        // Synchronous operation is completed, stop "nested" active scheduler.
        //CActiveScheduler::Stop();
        iNestedActiveScheduler->AsyncStop();
        // Execution continues from CTestModuleContainer::WaitOperation().
        }
    }


/*
-------------------------------------------------------------------------------

    Class: CTestModuleContainer

    Method: DoCancel

    Description: Cancel asyncronous request.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CTestModuleContainer::DoCancel()
    {

    if ( iUpAndRunning == EFalse )
        {
        
        // Before the completion check if the status was not already completed
        // from other thread in CTestThreadContainer::TestComplete().
        // For details see Jira STIF-564
        if(iStatus == KRequestPending)
            Complete ( KErrCancel ); 

        }

    }



/*
-------------------------------------------------------------------------------

    Class: CTestModuleContainer

    Method: RunError

    Description: Handle errors. 

    Because RunL does not leave, one should never come here. Just forward
    error code and let framework panic the server in case of error.

    Parameters: TInt aError:                  :in:  Error code

    Return Values:  TInt                            Error code

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
TInt CTestModuleContainer::RunError( TInt aError )
    {

    __TRACE( KError,( _L( "CTestModuleContainer::RunError %d" ), aError ) );

    return aError;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleContainer

    Method: ReadParametersFromScriptFileL

    Description: Read test class parameters from script file if parameter(s)
                 are/is set(TestScripter and TestCombiner).

    Parameters: const TDesC& aConfig: in: Test case (config) file name.
                TUint32& aStackSize: inout: Stack size.
                TUint32& aHeapMinSize: inout: Heap's minimum size.
                TUint32& aHeapMaxSize: inout: Heap's maximum size.

    Return Values: Symbian error code.

    Errors/Exceptions: Leaves if User::LeaveIfError() leaves
                       Leaves if User::LeaveIfNull() leaves

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CTestModuleContainer::ReadParametersFromScriptFileL( 
                                                const TDesC& aConfig,
                                                TInt& aStackSize,
                                                TInt& aHeapMinSize,
                                                TInt& aHeapMaxSize )
    {
    // __UHEAP_MARK;

    RFs fileServer;
    RFile file;
    TInt ret( KErrNone );

    User::LeaveIfError( fileServer.Connect() );
    CleanupClosePushL( fileServer );
    
    __TRACE( KInit, ( _L( "ReadParametersFromScriptFile(): Open configfile [%S]" ), &aConfig ) );

    TParse parse;
    parse.Set( aConfig, NULL, NULL );

    User::LeaveIfError( fileServer.SetSessionPath( parse.DriveAndPath() ) );

    User::LeaveIfError( file.Open( fileServer, aConfig, EFileRead | EFileShareAny ) );
    CleanupClosePushL( file );

    TInt size( 0 );
    User::LeaveIfError( file.Size( size ) );

    const TInt tmpSize = KMaxName; // 128
    TInt offset( 0 ); // Offset value to parts reading

    // Indications for tags
    TBool start_tag_found( EFalse );
    TBool end_tag_found( EFalse );
    // Offset values for start and end position parsing
    TInt offset_start( 0 );
    TInt offset_end( 0 );

    /* Search is combined to section that include tags. After this create new
    combined section that include second buffer data and new buffer data.
    Section:     1) 1+2
                 2) 2+1
                 2) 1+2 etc. This should ensure that all data is search.
    */

    // Construct modifiable heap-based descriptor. tmp1 to CleanupStack
    // This for first data buffer
    HBufC8* tmp1 = HBufC8::NewLC( tmpSize );            // 128
    TPtr8 buf1 = tmp1->Des();

    // Construct modifiable heap-based descriptor. tmp2 to CleanupStack
    // This for second data buffer
    HBufC8* tmp2 = HBufC8::NewLC( tmpSize );            // 128
    TPtr8 buf2 = tmp2->Des();

    // Construct modifiable heap-based descriptor. tmp3 to CleanupStack
    // This includes both first and second data buffers
    HBufC* tmp3 = HBufC::NewLC( tmpSize + tmpSize );    // 256
    TPtr currentSection = tmp3->Des();

    // Construct modifiable heap-based descriptor. tmp4 to CleanupStack
    // This is for changing 8 bit to 16 bit
    HBufC* tmp4 = HBufC::NewLC( tmpSize );              // 128
    TPtr to16bit = tmp4->Des();

    ret = KErrNone;

    // Read data to buffer 1
    // tmp4, tmp3, tmp2 and tmp1 are in CleanupStack => Leave OK
    // CleanupClosePushL is used to fileServer and file => Leave OK
    User::LeaveIfError( file.Read( offset, buf1, tmpSize ) );

    // For next buffer reading
    offset += tmpSize;

    // Read data to buffer 2
    // tmp4, tmp3, tmp2 and tmp1 are in CleanupStack => Leave OK
    // CleanupClosePushL is used to fileServer and file => Leave OK
    User::LeaveIfError( file.Read( offset, buf2, tmpSize ) );

    // 8 bit to 16. Create first combined buffer 1 and buffer 2 that
    // is used for searching.
    to16bit.Copy( buf1 );
    currentSection.Copy( to16bit );
    to16bit.Copy( buf2 );
    currentSection.Append( to16bit );

    // Check if small files to be parsed(e.g. settings + one short case).
    TInt bigsize( 0 );
    // Check if parsed section is smaller than 256
    if( size <  ( tmpSize + tmpSize ) )
        {
        bigsize = size;
        }
    else
        {
        bigsize = tmpSize + tmpSize;    // 256
        }

    do
        {
        // Start the next buffer reading
        offset += tmpSize;

        // Search the start tag
        ret = currentSection.Find( KStifSettingsStartTag );
        if( ret != KErrNotFound )
            {
            // Succesfully search, take the start offset
            ret = file.Seek( ESeekCurrent, offset_start );
            if( ret == KErrNone )
                {
                start_tag_found = ETrue;
                // Current section is end so offset to beging of the 
                // combined section.
                offset_start = ( offset_start - ( bigsize ) );
                }
            }
        // Search the end tag
        ret = currentSection.Find( KStifSettingsEndTag );
        if( ret != KErrNotFound )
            {
            // Succesfully search, take the end offset
            ret = file.Seek( ESeekCurrent, offset_end );
            if( ret == KErrNone )
                {
                end_tag_found = ETrue;
                }
            }

        // Both start and end tags are founded, start parsing sizes.
        if( start_tag_found && end_tag_found )
            {
            TInt length = ( offset_end - offset_start );

            // Construct modifiable heap-based descriptor.
            // setting_buf to CleanupStack
            HBufC8* setting_buf = HBufC8::NewLC( length );// 8 bit for reading
            TPtr8 setting8 = setting_buf->Des();

            // Construct modifiable heap-based descriptor.
            // setting_buf2 to CleanupStack
            HBufC* setting_buf2 = HBufC::NewLC( length );// 16 bit for parsing
            TPtr setting16 = setting_buf2->Des();

            // Read data from the founded STIF settings sections
            ret = KErrNone;
            // HBufCs are in CleanupStack => Leave OK
            // CleanupClosePushL is used to fileServer and file => Leave OK
            User::LeaveIfError( file.Read( offset_start, setting8, length ) );

            // Section 8 bit to 16.
            setting16.Copy( setting8 );

            // Start create parser for parsing heap and stack sizes.
            // NOTE: Comments are parsed away from the section.
            CStifParser* parser = NULL;
            User::LeaveIfNull( parser = CStifParser::NewL( setting16,
                                        CStifParser::ECStyleComments  ) );
            CleanupStack::PushL(parser);

            CStifSectionParser* section = NULL;
            User::LeaveIfNull( section = parser->SectionL( KStifSettingsStartTag,
                                                   KStifSettingsEndTag ) );
            CleanupStack::PushL(section);
            TInt lineRet( KErrNone );
            TInt integer( 0 );

            // Try to get stack size
            CStifItemParser* itemLine = NULL;
            TRAPD( parse_ret, itemLine = section->GetItemLineL(
                                                        KUserDefStackSize ) );
            if ( parse_ret == KErrNone && itemLine != NULL )
                {
                lineRet = itemLine->GetInt( KUserDefStackSize, integer );
                if ( lineRet == KErrNone )
                    {
                    aStackSize = integer;
                    }
                }
            delete itemLine;
            itemLine = NULL;

            // Try to get minimum heap size
            TRAP( parse_ret, itemLine = section->GetItemLineL(
                                                        KUserDefMinHeap ) );
            if ( parse_ret == KErrNone && itemLine != NULL )
                {
                lineRet = itemLine->GetInt( KUserDefMinHeap, integer );
                if ( lineRet == KErrNone )
                    {
                    aHeapMinSize = integer;
                    }
                }
            delete itemLine;
            itemLine = NULL;

            // Try to get maximum heap size
            TRAP( parse_ret, itemLine = section->GetItemLineL(
                                                        KUserDefMaxHeap ) );
            if ( parse_ret == KErrNone && itemLine != NULL )
                {
                lineRet = itemLine->GetInt( KUserDefMaxHeap, integer );
                if ( lineRet == KErrNone )
                    {
                    aHeapMaxSize = integer;
                    }
                }
            delete itemLine;

            CleanupStack::Pop(section);
            CleanupStack::Pop(parser);

            CleanupStack::PopAndDestroy( setting_buf2 );
            CleanupStack::PopAndDestroy( setting_buf );

            delete section;
            delete parser;

            break; // Stop the do branch
            }

        // Start to create new section for search

        // 8 bit to 16. Last buffer to first
        to16bit.Copy( buf2 );
        currentSection.Copy( to16bit );

        // Read new data to buffer 1
        ret = KErrNone;
        User::LeaveIfError( file.Read( offset, buf1, tmpSize ) );

        // 8 bit to 16. Append
        to16bit.Copy( buf1 );
        currentSection.Append( to16bit );

        // Copy first buffer data to buffer 2. This is added to first
        // in section in next run
        buf2.Copy( buf1 );

        } while( offset < size );

    CleanupStack::PopAndDestroy( tmp4 );
    CleanupStack::PopAndDestroy( tmp3 );
    CleanupStack::PopAndDestroy( tmp2 );
    CleanupStack::PopAndDestroy( tmp1 );

    CleanupStack::PopAndDestroy( &file );
    CleanupStack::PopAndDestroy( &fileServer );
    file.Close();
    fileServer.Close();

    //__UHEAP_MARKEND;

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleContainer

    Method: ReadParametersFromTestModule

    Description: Loads dynamically testmodule and calls SetRequirements()-
                 method for test module parameter handling.

    Parameters: const TDesC& aModuleName: in: Test module name
                CTestModuleParam*& aTestModuleParam: inout: Object for handling
                test module parameters.

    Return Values: TInt: Symbian error code.

    Errors/Exceptions: None.

    Status: Proposal

-------------------------------------------------------------------------------
*/
TInt CTestModuleContainer::ReadParametersFromTestModule(
                                const TDesC& aModuleName,
                                CTestModuleParam*& aTestModuleParam )
    {
    __TRACE( KInit, (  _L( "ReadParametersFromTestModule() [%S]" ), &aModuleName ) );
    RLibrary testModule;
    // Load the module
    TPtrC dllName;
    
    // Remove optional index appended to module name
    TFileName validModuleName;
    RemoveOptionalIndex(aModuleName, validModuleName);
    __TRACE(KInit, (_L( "Valid module name is [%S] (extracted from [%S])"), &validModuleName, &aModuleName));
    dllName.Set(validModuleName);
    
    // Loading should work with and without '.dll' extension.
    TInt r = testModule.Load( dllName );
    if ( r != KErrNone )
        {
        __TRACE( KError, ( CStifLogger::EError, _L("Can't initialize test module[%S], code = %d"), &dllName, r ) );
        return KErrNotFound;
        }
    else
        {
        // Print reset module name
        __TRACE( KInit, (  _L("Loaded test module[%S]"), &dllName ) );
        }

    

    CTestInterfaceFactoryTestModule libEntry = NULL;
    
#if defined( __ARMCC__ )
        {
        // In this environment the heap and stack feature may crash if
        // test module not include a workaround for ARM RVCT(ARMv5) compiler
        // error. For more information see STIF user guide.
        __TRACE ( KInit, ( _L( "Workaround for ARM RVCT(ARMv5) compiler error should be included to TestModule." ) ) );
        }
#endif // #if defined(__ARMCC__)

    // Get pointer to second exported function
    // Verify that there is function
    //CTestInterfaceFactoryTestModule libEntry;
    libEntry = (CTestInterfaceFactoryTestModule) testModule.Lookup( 2 );
    if ( libEntry == NULL )
        {
        // New instance can't be created
        __TRACE( KInit, (_L( "Test module is old version and has not SetRequirements() feature." ) ) );
        testModule.Close();
        return KErrNotSupported;
        }
    else
        {
        __TRACE ( KInit, ( _L("Pointer to 2st exported received")));
        }

    // Calls dynamically loaded module's second method.
    __TRACE ( KVerbose, (_L("Calling 2st exported at 0x%x"), (TUint32) libEntry ));
    TUint32 check = 0;
    TInt ret =  (*libEntry)( aTestModuleParam, check  );
    if( check != KStifTestModuleParameterChanged  )
        {
        // Problems in def-file definitions. Test module is old wersion
        // and has not SetRequirements feature.
        RDebug::Print( ( _L( "STIF TF: Test module is old version and has not SetRequirements() feature." ) ) );
        __TRACE( KInit, (_L( "Test module is old version and has not SetRequirements() feature." ) ) );
        testModule.Close();
        return KErrNotSupported;
        }
    if( ret != KErrNone )
        {
        __TRACE (KInit, (_L("ReadParametersFromTestModule; SetRequirements fails with error: %d"), ret ) );
        testModule.Close();
        return ret;
        }

    libEntry = NULL; 
    testModule.Close(); // After close test module's pointer is not
                        // valid anymore.

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleContainer

    Method: GetTestModuleParams

    Description: Load test module dynamically. If loading is done succesfully
                 then set test module's parameters according to version.
                 Verify received parameters.

    Parameters: const TDesC& aModuleName: in: Test module name.
                const TDesC& aConfig: in: Test case (config) file name.
                TInt& aStackSize: inout: Stack size.
                TInt& aHeapMinSize: inout: Heap's minimum size.
                TInt& aHeapMaxSize: inout: Heap's maximum size.

    Return Values: TInt: Symbian error code.

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
TInt CTestModuleContainer::GetTestModuleParams( const TDesC& aModuleName,
                                                const TDesC& aConfig,
                                                TInt& aStackSize,
                                                TInt& aHeapMinSize,
                                                TInt& aHeapMaxSize )
    {
    TInt r( KErrNone );

    // There is lower case despite what is initialization file given.
    // Module name is without dll extension.
    // Check is TestScripter or TestCombiner and test case (config) file
    // name is given.
    if( ( aModuleName.Find( KTestScripterName ) != KErrNotFound ||
          aModuleName == _L( "testcombiner" ) ) && aConfig != KNullDesC )
        {
        TRAPD( ret, r = ReadParametersFromScriptFileL( aConfig, aStackSize,
                                          aHeapMinSize, aHeapMaxSize ) );
        if( r != KErrNone )
            {
            __TRACE( KInit, ( CStifLogger::ERed, _L( "Cannot set Test Class's stack or heap sizes, fail with error: %d" ), r ) );
            __TRACE( KInit, ( _L( "Default sizes will be use(See KTestThreadMinHeap, KTestThreadMinHeap and KStackSize)" ) ) );
            return r;
            }
        if( ret != KErrNone )
            {
            __TRACE( KInit, ( CStifLogger::ERed, _L( "Cannot set Test Class's stack or heap sizes, leaves with error: %d" ), r ) );
            __TRACE( KInit, ( _L( "Default sizes will be use(See KTestThreadMinHeap, KTestThreadMinHeap and KStackSize)" ) ) );
            return ret;
            }
        }
    else
        {
        CTestModuleParam* testModuleParam = NULL;
        //--PYTHON-- begin
        if(aModuleName.Find(KPythonScripter) != KErrNotFound)
            {
			RDebug::Print(_L("CTestModuleContainer::GetTestModuleParams reading params for PythonScripter, aModuleName=[%S]"), &aModuleName);
            TName n;
            n.Copy(KPythonScripter);
            r = ReadParametersFromTestModule( n, testModuleParam );
            }
        else
        //--PYTHON-- end
            r = ReadParametersFromTestModule( aModuleName, testModuleParam );

        if( r != KErrNone )
            {
            __TRACE( KInit, ( _L( "Cannot set Test Module's stack or heap sizes, fails with: %d" ), r ) );
            __TRACE( KInit, ( _L( "Default sizes will be use(See KTestThreadMinHeap, KTestThreadMinHeap and KStackSize)" ) ) );
            delete testModuleParam;
            return r;
            }

        if ( testModuleParam->Version() == 1 )
            {
            // Casting
            CTestModuleParamVer01* paramVer01 = ( CTestModuleParamVer01* ) testModuleParam;
            aStackSize = paramVer01->iTestThreadStackSize;
            aHeapMinSize = paramVer01->iTestThreadMinHeap;
            aHeapMaxSize = paramVer01->iTestThreadMaxHeap;
            }

        delete testModuleParam;
        }

    // Verify that sizes are valid. If problems are notices then use default 
    // value

    // 1) "The panic occurs when the value of the stack size is negative."
    if( aStackSize < 0 )
        {
        RDebug::Print( ( _L("STIF TF: GetTestModuleParams() fails because value of the stack size is negative, default value is taken into use") ) );
        __TRACE( KInit, ( CStifLogger::ERed, _L("STIF TF: GetTestModuleParams() fails because value of the stack size is negative, default value is taken into use" ) ) );
        // Use default value
        aStackSize = KStackSize;
        }
    
    // 2) "The panic occurs if the minimum heap size specified is less
    // than KMinHeapSize".
    // KMinHeapSize: "Functions that require a new heap to be allocated will
    // either panic, or will reset the required heap size to this value if a
    // smaller heap size is specified".
    if( aHeapMinSize < KMinHeapSize )
        {
        RDebug::Print( _L( "STIF TF: GetTestModuleParams() fails because test module minimum heap size is less than KMinHeapSize:[%d], default value is taken into use" ), KMinHeapSize );
        __TRACE( KInit, ( CStifLogger::ERed, _L("STIF TF: GetTestModuleParams() fails because test module minimum heap size is less than KMinHeapSize:[%d], default value is taken into use"), KMinHeapSize ) );
        // Use default value(Note: This is used for now on)
        aHeapMinSize = KTestThreadMinHeap;
        }

    // 3) "The panic occurs if the minimum heap size specified is greater than
    // the maximum size to which the heap can grow".
    // Check this last !
    if( aHeapMinSize > aHeapMaxSize )
        {
        RDebug::Print( ( _L("STIF TF: GetTestModuleParams() fails because the maximum heap size is specified less than minimum heap size, default values is taken into use" ) ) );
        __TRACE( KInit, ( CStifLogger::ERed, _L( "STIF TF: GetTestModuleParams() fails because the maximum heap size is specified less than minimum heap size, default values is taken into use" ) ) );
        // Use default values
        aHeapMinSize = KTestThreadMinHeap;
        aHeapMaxSize = KTestThreadMaxHeap;
        }

    __TRACE( KInit, ( _L( "[%S] uses:" ), &aModuleName ) );
    __TRACE( KInit, ( _L( "Stack size: [%d]" ), aStackSize ) );
    __TRACE( KInit, ( _L( "Minimum heap size: [%d]" ), aHeapMinSize ) );
    __TRACE( KInit, ( _L( "Maximum heap size: [%d]" ), aHeapMaxSize ) );

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleContainer

    Method: GetTestCaseTitleL

    Description: Gets title of currently running test case.

    Parameters: TDes& aTestCaseTitle: out: Test case title.

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CTestModuleContainer::GetTestCaseTitleL(TDes& aTestCaseTitle)
    {
    //currently run test case stored in the iOperationIntBuffer variable
    iCTestModule->GetTestCaseTitleL( iOperationIntBuffer, 
                                     iCTestExecution->GetConfigFileName(),
                                    aTestCaseTitle);  
    }


/*
-------------------------------------------------------------------------------

    Class: CTestModuleContainer

    Method: GetTestModule

    Description: Gets pointer to test module

    Parameters: none

    Return Values: CTestModule* : Pointer to test module

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
CTestModule* CTestModuleContainer::GetTestModule()
    {
    
    return iCTestModule;
    }


/*
-------------------------------------------------------------------------------

    DESCRIPTION

    This module contains implementation of CPrintHandler class member functions.
    CErrorPrintHandler listens print notifications from test thread.

-------------------------------------------------------------------------------
*/

// ================= MEMBER FUNCTIONS =========================================


/*
-------------------------------------------------------------------------------

    Class: CErrorPrintHandler

    Method: NewL

    Description: Constructs a new CErrorPrintHandler object.

    Parameters: CTestExecution& aExecution: in: "Parent"

    Return Values: CErrorPrintHandler*: New undertaker

    Errors/Exceptions: Leaves if memory allocation or ConstructL leaves.

    Status: Proposal

-------------------------------------------------------------------------------
*/
CErrorPrintHandler* CErrorPrintHandler::NewL( CTestModuleContainer& aContainer )
    {

    CErrorPrintHandler* self = new( ELeave ) CErrorPrintHandler( aContainer );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;

    }

/*
-------------------------------------------------------------------------------

    Class: CErrorPrintHandler

    Method: ConstructL

    Description: Second level constructor.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CErrorPrintHandler::ConstructL()
    {

    }

/*
-------------------------------------------------------------------------------

    Class: CErrorPrintHandler

    Method: CErrorPrintHandler

    Description: Constructor

    Parameters: CTestModuleContainer& aExecution :in:   "Parent"

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
CErrorPrintHandler::CErrorPrintHandler( CTestModuleContainer& aContainer ) :
    CActive( CActive::EPriorityStandard ),
    iContainer( aContainer )
    {
    
    CActiveScheduler::Add ( this );
        
    }

/*
-------------------------------------------------------------------------------

    Class: CErrorPrintHandler

    Method: ~CErrorPrintHandler

    Description: Destructor. 
    Cancels active request.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
CErrorPrintHandler::~CErrorPrintHandler()
    {

    Cancel();

    }

/*
-------------------------------------------------------------------------------

    Class: CErrorPrintHandler

    Method: StartL

    Description: Starts to monitor thread.

    Parameters: None

    Return Values: TInt                             Always KErrNone

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
void CErrorPrintHandler::StartL()
    {
    
    __TRACE( KPrint, ( _L( "CErrorPrintHandler::StartL" ) ) );
    
    iStatus = KRequestPending;
    SetActive();
    
    // Signal test thread
    iContainer.iErrorPrintSem.Signal();

    }

/*
-------------------------------------------------------------------------------

    Class: CErrorPrintHandler

    Method: RunL

    Description: Handles thread death.
    Function does:
    1 ) Stops monitoring thread
    1 ) Marks thread death
    2 ) Completes ongoing requests
    3 ) Cleans the memory

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CErrorPrintHandler::RunL()
    {

    __TRACE( KPrint, ( _L( "CErrorPrintHandler::RunL [%d]" ), iStatus.Int() ) );
    
    iContainer.DoErrorPrint();    
    
    // enable error print request
    iContainer.iErrorPrintHandler->StartL();
       
    }

/*
-------------------------------------------------------------------------------

    Class: CErrorPrintHandler

    Method: DoCancel

    Description: Stops print notification listening.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/

void CErrorPrintHandler::DoCancel()
    {

    __TRACE( KPrint, ( _L( "CErrorPrintHandler::DoCancel" ) ) );

    // Signal test thread
    iContainer.iErrorPrintSem.Wait();

    TRequestStatus* status = &iStatus;    
    User::RequestComplete( status, KErrCancel );

    }

/*
-------------------------------------------------------------------------------

    Class: CErrorPrintHandler

    Method: RunError

    Description: Handle errors. RunL function does not leave, so one should
    never come here. 

    Print trace and let framework handle error( i.e to do Panic )

    Parameters: TInt aError:                  :in:  Error code

    Return Values:  TInt                            Error code

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
TInt CErrorPrintHandler::RunError( TInt aError )
    {
    
    __TRACE( KError,( _L( "CErrorPrintHandler::RunError" ) ) );

    return aError;
    
    }

/*
-------------------------------------------------------------------------------

    Class: -

    Method: CheckModuleName

    Description: Check is module TestScripter. Does parsing and returns new
                 module name and error codes(Needed operations when creating
                 server sessions to TestScripter).

    Parameters: TFileName aModuleName: in: Module name for checking.
                TFileName& aNewModuleName: inout: Parsed module name.

    Return Values: KErrNone if TestScripter releated module.
                   KErrNotFound if not TestScripter releated module.

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
TInt CheckModuleName( const TFileName& aModuleName, TFileName& aNewModuleName )
    {
//--PYTHON-- old code has been replaced with the new one
/*
    // Check that length is greated than KTestScripterNameLength
    if( aModuleName.Length() < KTestScripterNameLength )
        {
        return KErrNotFound;
        }
    // Check is TestScripter
    TPtrC check( aModuleName.Mid( 0, KTestScripterNameLength ) );
    TInt ret = check.Compare( KTestScripterName );
    if( ret == KErrNone )
        {
        aNewModuleName.Copy( aModuleName.Mid( 0, KTestScripterNameLength ) );
        }
    else
        {
        return KErrNotFound;
        }

    return KErrNone;
*/

	// Check that length is greated than KTestScripterNameLength
	if( aModuleName.Length() >= KTestScripterNameLength )
		{
		TPtrC check( aModuleName.Mid( 0, KTestScripterNameLength ) );
		TInt ret = check.Compare( KTestScripterName );
		if( ret == KErrNone )
			{
			aNewModuleName.Copy( aModuleName.Mid( 0, KTestScripterNameLength ) );
			return KErrNone;
			}
		}

	// Check that length is greated than KTestScripterNameLength
	if( aModuleName.Length() >= KPythonScripterLength )
		{
		TPtrC check( aModuleName.Mid( 0, KPythonScripterLength ) );
		TInt ret = check.Compare( KPythonScripter );
		if( ret == KErrNone )
			{
			aNewModuleName.Copy( aModuleName.Mid( 0, KPythonScripterLength ) );
			return KErrNone;
			}
		}

    return KErrNotFound;
    }

/*
-------------------------------------------------------------------------------

    Class: -

    Method: RemoveOptionalIndex

    Description: Remove optional index appended to module name.
                 If it is found (@ char) then set new module name without it.
                 This feature is used when iSeparateProcesses is set in
                 TestEngine.

    Parameters: TFileName aModuleName: in: Module name for checking.
                TFileName& aNewModuleName: inout: Parsed module name.

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
void RemoveOptionalIndex(const TDesC& aModuleName, TDes& aNewModuleName)
    {
    //Copy module name to destination buffer
    aNewModuleName.Copy(aModuleName);
    
    //Search for @ char (it means that some index has been appended)
    TInt index = aNewModuleName.Find(_L("@"));
    
    //Remove index form name (if found)
    if(index != KErrNotFound)
        {
        aNewModuleName.Delete(index, aNewModuleName.Length() - index);
        }
    }

// End of File
