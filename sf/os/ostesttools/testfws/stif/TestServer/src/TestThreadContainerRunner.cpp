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
* CTestThreadContainerRunner class member functions.
*
*/

// INCLUDE FILES
#include "TestThreadContainerRunner.h"
#include "ThreadLogging.h"
#include "TestThreadContainer.h"
#include "TestServer.h"

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


// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

     Class: CTestThreadContainerRunner

     Method: NewL

     Description: NewL is first phase of two-phased constructor.

     NewL is first phase of two-phased constructor.

     Parameters: TThreadId aMainThreadId: in: Main thread id.
     			 CActiveScheduler* aMainThreadActiveScheduler: in: Pointer to main thread active scheduler.
     
     Return Values: Pointer to new CTestThreadContainerRunner object.

     Errors/Exceptions: None

     Status: Draft
    
-------------------------------------------------------------------------------
*/
EXPORT_C CTestThreadContainerRunner* CTestThreadContainerRunner::NewL( TThreadId aMainThreadId, 
		CActiveScheduler* aMainThreadActiveScheduler )
	{	
	CTestThreadContainerRunner* self = new(ELeave) CTestThreadContainerRunner();
	CleanupStack::PushL( self );
	self->ConstructL( aMainThreadId, aMainThreadActiveScheduler );
	CleanupStack::Pop( self );
	
	return self;
	}

/*
-------------------------------------------------------------------------------

     Class: CTestThreadContainerRunner

     Method: CTestThreadContainerRunner

     Description: Default constructor.

	 Default constructor.

     Parameters: None.
     
     Return Values: None.

     Errors/Exceptions: None.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
CTestThreadContainerRunner::CTestThreadContainerRunner()
:CActive( EPriorityNormal )
	{
	}

/*
-------------------------------------------------------------------------------

     Class: CTestThreadContainerRunner

     Method: ~CTestThreadContainerRunner

     Description: Default destructor.

	 Default destructor.

     Parameters: None.
     
     Return Values: None.

     Errors/Exceptions: None.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
CTestThreadContainerRunner::~CTestThreadContainerRunner()
	{
	Cancel();
	}

/*
-------------------------------------------------------------------------------

     Class: CTestThreadContainerRunner

     Method: ConstructL

     Description: Second phase of two-phased constructor.

	 Second phase of two-phased constructor.

     Parameters: TThreadId aMainThreadId: in: Main thread id.
     			 CActiveScheduler* aMainThreadActiveScheduler: in: Pointer to main thread active scheduler.
     
     Return Values: None.

     Errors/Exceptions: None.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
void CTestThreadContainerRunner::ConstructL( TThreadId aMainThreadId, 
		CActiveScheduler* aMainThreadActiveScheduler )
	{
	User::LeaveIfNull( aMainThreadActiveScheduler );
	
	iMainThreadId = aMainThreadId;

	iMainThreadActiveScheduler = aMainThreadActiveScheduler;	
	aMainThreadActiveScheduler->Add( this );
	
	iOperationOngoing.CreateLocal( 0 );
	
	iTestThreadContainer = NULL;
	iCurrentOperation = ENone;
	}

/*
-------------------------------------------------------------------------------

     Class: CTestThreadContainerRunner

     Method: Setup

     Description: Emulates setup part of CTestThreadContainer::ExecutionThread method

	 Setup is performed in main thread.

     Parameters: CTestModuleContainer* aTestModuleContainer: in: Pointer to test CTestModuleContainer
     
     Return Values: None.

     Errors/Exceptions: None.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
void CTestThreadContainerRunner::Setup( CTestModuleContainer* aTestModuleContainer )
	{	
	iTestModuleContainer = aTestModuleContainer;
	iCurrentOperation = ESetup;
	SetActive();
	CompleteRequest();
	iOperationOngoing.Wait();
	}

/*
-------------------------------------------------------------------------------

     Class: CTestThreadContainerRunner

     Method: RunOneIteration

     Description: Emulates one iteration of while loop from CTestThreadContainer::ExecutionThread method

	 Iterations is performed in main thread.

     Parameters: None
     
     Return Values: None.

     Errors/Exceptions: None.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
void CTestThreadContainerRunner::RunOneIteration()
	{	
	iCurrentOperation = ERunOneIteration;
	SetActive();
	CompleteRequest();
	iOperationOngoing.Wait();
	}

/*
-------------------------------------------------------------------------------

     Class: CTestThreadContainerRunner

     Method: TeareDown

     Description: Emulates teare down part of CTestThreadContainer::ExecutionThread method

	 Teare down is performed in main thread.

     Parameters: None
     
     Return Values: None

     Errors/Exceptions: None

     Status: Draft
    
-------------------------------------------------------------------------------
*/
void CTestThreadContainerRunner::TeareDown()
	{	
	iCurrentOperation = ETearDown;
	SetActive();
	CompleteRequest();
	iOperationOngoing.Wait();
	}

/*
-------------------------------------------------------------------------------

     Class: CTestThreadContainerRunner

     Method: CheckSignalFromSuspend

     Description: Checks if operation change signal was signaled from suspend state.

	 Main part of code is executed in main thread.

     Parameters: None
     
     Return Values: None

     Errors/Exceptions: None

     Status: Draft
    
-------------------------------------------------------------------------------
*/
void CTestThreadContainerRunner::CheckSignalFromSuspend()
	{
    if ( iSignalFromSuspend )
        {
        iSignalFromSuspend = EFalse;
        iTestThreadContainer->TestComplete( iReturnCode );
        }
	}

/*
-------------------------------------------------------------------------------

     Class: CTestThreadContainerRunner

     Method: IsReusable

     Description: Checks if test thread is reusable.

	 Checks if test thread is reusable.

     Parameters: None
     
     Return Values: True if TestThreadContainer is reusable.

     Errors/Exceptions: None

     Status: Draft
    
-------------------------------------------------------------------------------
*/
TBool CTestThreadContainerRunner::IsReusable()
	{
	return iReusable;
	}

/*
-------------------------------------------------------------------------------

     Class: CTestThreadContainerRunner

     Method: ExceptionHandler

     Description: Test thread exception handler.

	 Test thread exception handler.

     Parameters: TExcType aType: in: Exception type.
     
     Return Values: None

     Errors/Exceptions: None

     Status: Draft
    
-------------------------------------------------------------------------------
*/
void CTestThreadContainerRunner::ExceptionHandler ( TExcType aType )
	{
	// Kill the current thread, undertaker handles rest
	RThread current;
	current.Kill( aType );
	
	// This line is never executed, because thread has been killed.
	}

/*
-------------------------------------------------------------------------------

     Class: CTestThreadContainerRunner

     Method: RunL

     Description: RunL derived from CActive handles the completed requests.

	 RunL derived from CActive handles the completed requests.

     Parameters: None.
     
     Return Values: None.

     Errors/Exceptions: Leaves if one of the called method leavs.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
void CTestThreadContainerRunner::RunL()
	{
	switch( iCurrentOperation )
		{
		case ESetup:
			{
		    TInt error( KErrNone );
		    
		    const TUint32 KAll = 0xFFFFFFFF;
		#ifndef __HIDE_IPC_V1__ // e.g. 7.0s, 8.0a
		    RThread currentThread;
		    currentThread.SetExceptionHandler( ExceptionHandler, KAll );
		#else // PlatSec used. Thread exception management is part of the User class.
		    User::SetExceptionHandler( ExceptionHandler, KAll );
		#endif // __HIDE_IPC_V1__

		    // Create cleanup stack
//		    CTrapCleanup* tc = CTrapCleanup::New();
//		    __ASSERT_ALWAYS( tc, Panic( ECreateTrapCleanup ) );

		    iTestThreadContainer = NULL;    
		    TRAPD( err,
		    	iTestThreadContainer = CTestThreadContainer::NewL( iTestModuleContainer, 
		    			iTestModuleContainer->ServerThreadId() );
		        );    
		    if( err != KErrNone )
		        {
		        Panic( ENullTestThreadContainer );
		        }

		    // Construct the logger
		    TName path = _L("C:\\logs\\testframework\\testserver\\");
		    TFileName name = _L("testserver_thread_");  
		    name.Append( iTestModuleContainer->TestModuleName() );

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

		    TRAP ( error, iTestThreadContainer->SetThreadLogger( CStifLogger::NewL( path, name,
		                                                            loggerSettings ) ) );

		    iReusable = ETrue;             // Is test module reusable?
		    iInitialized = EFalse;         // Is module initialized?
		    iSignalFromSuspend = EFalse;   // Send signal from suspend state?
		    
		    iReturnCode = KErrNone;			
			}
			break;
		case ERunOneIteration:
			{			
	        iReturnCode = KErrNone;
	        
	        switch ( iTestModuleContainer->OperationType() )
	            {
	            // Test module initialisation
	            case CTestModuleContainer::EInitializeModule:
	                {
	                __ASSERT_ALWAYS ( !iInitialized,
	                                  Panic( EReInitializingTestModule ) );

	                // Initialize module
	                if ( iTestThreadContainer->InitializeModuleInThread( iModule ) == KErrNone )
	                    {
	                    iInitialized = ETrue;
	                    }

	                iSignalFromSuspend = ETrue;
	                break;
	                }

	            // Test case enumeration
	            case CTestModuleContainer::EEnumerateInThread:
	                {
	                __ASSERT_ALWAYS ( iInitialized,
	                                  Panic( ETestModuleNotInitialized ) );
	                iReturnCode = iTestThreadContainer->EnumerateInThread();

	                iSignalFromSuspend = ETrue;
	                break;
	                }

	            // Free test case enumeration data
	            case CTestModuleContainer::EFreeEnumerationData:
	                {
	                __ASSERT_ALWAYS ( iInitialized,
	                                  Panic( ETestModuleNotInitialized ) );
	                iTestThreadContainer->FreeEnumerationDataInThread ();
	                
	                iSignalFromSuspend = ETrue;
	                break;
	                }

	            // Execute test case
	            case CTestModuleContainer::EExecuteTestInThread:
	                {
	                __ASSERT_ALWAYS ( iInitialized,
	                                  Panic( ETestModuleNotInitialized ) );
	                iReturnCode = iTestThreadContainer->ExecuteTestCaseInThread ();

	                iSignalFromSuspend = ETrue;
	                break;
	                }

	            // Exiting (i.e test server is unloading)
	            case CTestModuleContainer::EExit:
	                {
	                iReusable = EFalse;
	                break;
	                }

	            // Illegal state
	            default:
	                {
	                Panic( EInvalidTestModuleOperation );
	                }
	            }
			}
			break;
		case ETearDown:
			{
			iTestThreadContainer->DeleteTestModule();

		    // Close handle to module. No function calls to test
		    // module are possible after this line.
			iModule.Close();

		    // Delete logger    
			CStifLogger* threadLogger = iTestThreadContainer->GetThreadLogger();
			iTestThreadContainer->SetThreadLogger( NULL );
			delete threadLogger;
			threadLogger = NULL;

		    // Delete clean-up stack.
//		    delete tc;
//		    tc = NULL;
			
		    // Operation completed ( = Exit completed )
		    iTestThreadContainer->TestComplete( KErrNone );
		    
		    delete iTestThreadContainer;
			}
			break;
		case ENone:
			break;
		}
		
	iCurrentOperation = ENone;
	iOperationOngoing.Signal();
	}

/*
-------------------------------------------------------------------------------

     Class: CTestThreadContainerRunner

     Method: Panic

     Description: Raises panic.

	 Raises panic.

     Parameters: TInt aReason: in: Panic reason.
     
     Return Values: None.

     Errors/Exceptions: None.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
void CTestThreadContainerRunner::Panic( TInt aReason )
    {    
    RDebug::Print( _L("CTestThreadContainer::Panic %d"), aReason );    
    User::Panic( _L("CTestThreadContainer::Panic"), aReason );    
    }

/*
-------------------------------------------------------------------------------

     Class: CTestThreadContainerRunner

     Method: DoCancel

     Description: DoCancel derived from CActive handles the Cancel.

	 DoCancel derived from CActive handles the Cancel.

     Parameters: None.
     
     Return Values: None.

     Errors/Exceptions: None.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
void CTestThreadContainerRunner::DoCancel()
	{
	}

/*
-------------------------------------------------------------------------------

     Class: CTestThreadContainerRunner

     Method: CompleteRequest

     Description: Complets current operation request.

     Complets current operation request, what causes execution of RunL method 
     in main thread of UITestServerStarter.


     Parameters: None.
     
     Return Values: None.

     Errors/Exceptions: None.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
void CTestThreadContainerRunner::CompleteRequest()
	{
	iStatus = KRequestPending;
	TRequestStatus* statusPtr = &iStatus;		            	
	RThread mainThread;
	User::LeaveIfError( mainThread.Open( iMainThreadId ) );
	mainThread.RequestComplete( statusPtr, KErrNone );
	mainThread.Close();	
	}

// End of File
