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
* Description: This module contains implementation of CUnderTaker 
* class member functions. CUnderTaker is a class, which listens 
* on Test Execution Thread death, and if that thread dies, then 
* UnderTaker aborts the ongoing test execution request.
*
*/

// INCLUDE FILES
#include <e32std.h>
#include <e32svr.h>
#include "TestEngineClient.h"
#include <StifTestModule.h>
#include <stifinternal/TestServerClient.h>
#include "TestServer.h"
#include "TestServerModuleIf.h"
#include "TestServerCommon.h"
#include "PrintQueue.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ==================== LOCAL FUNCTIONS =======================================

// ================= MEMBER FUNCTIONS =========================================


/*
-------------------------------------------------------------------------------

    Class: CUnderTaker

    Method: NewL

    Description: Constructs a new CUnderTaker object.

    Parameters: CTestModuleContainer* aContainer :in:   "Parent"

    Return Values: CUnderTaker*                         New undertaker

    Errors/Exceptions: Leaves if memory allocation or ConstructL leaves.

    Status: Proposal

-------------------------------------------------------------------------------
*/
CUnderTaker* CUnderTaker::NewL( CTestModuleContainer* aContainer )
    {

    CUnderTaker* self = new( ELeave ) CUnderTaker();
    CleanupStack::PushL( self );
    self->ConstructL( aContainer );
    CleanupStack::Pop( self );
    return self;

    }

/*
-------------------------------------------------------------------------------

    Class: CUnderTaker

    Method: ConstructL

    Description: Second level constructor.

    Parameters: CTestModuleContainer* aExecution :in:   "Parent"

    Return Values: None

    Errors/Exceptions: Panics if aContainer is NULL

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CUnderTaker::ConstructL( CTestModuleContainer* aContainer )
    {

    __ASSERT_ALWAYS( aContainer, 
                     CTestServer::PanicServer( ENullTestModuleContainer ) );

    iModuleContainer = aContainer;

    }

/*
-------------------------------------------------------------------------------

    Class: CUnderTaker

    Method: CUnderTaker

    Description: Constructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
CUnderTaker::CUnderTaker() :
    CActive( CActive::EPriorityHigh+1 )
    {

    // By default in asynchronous mode
    iSynchronousMode = EFalse;

    }

/*
-------------------------------------------------------------------------------

    Class: CUnderTaker

    Method: ~CUnderTaker

    Description: Destructor. 
    Cancels active request.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
CUnderTaker::~CUnderTaker()
    {

    Cancel();

    }




/*
-------------------------------------------------------------------------------

    Class: CUnderTaker

    Method: StartL

    Description: Starts to monitor thread.

    Parameters: None

    Return Values: TInt                             Always KErrNone

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
TInt CUnderTaker::StartL()
    {
    
    SetActive();
    iModuleContainer->iThread.Logon( iStatus );
    iCancelNeeded = ETrue;

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CUnderTaker

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
void CUnderTaker::RunL()
    {

    // Make sure that any of the test interference thread's won't stay to run.
    iModuleContainer->KillTestinterferenceThread();

    // Make sure that any of the test measurement's process won't stay to run.
    iModuleContainer->KillTestMeasurement();

    // Get the exit category
    TName exitCategory = _L( "Crash reason:" );
    exitCategory.Append( iModuleContainer->iThread.ExitCategory() );
    TInt exitReason = iModuleContainer->iThread.ExitReason();
    TFullTestResult::TCaseExecutionResult exitType = TFullTestResult::ECaseException;
    if( iModuleContainer->iThread.ExitType() == EExitPanic )
        {
        exitType = TFullTestResult::ECasePanic;
        }    

    // it is not running anymore..
    iModuleContainer->iUpAndRunning = EFalse;

    // Do not monitor anymore.
    iModuleContainer->iThread.LogonCancel( iStatus );
    iCancelNeeded = EFalse;

    __TRACE( KError,( CStifLogger::ERed, _L( "Execution thread crashed, exitReason = %d" ), exitReason ) );
    __TRACE( KError,( CStifLogger::ERed, exitCategory ) );    

    if ( iSynchronousMode )
        {        

        // Synchronous request was running. Complete that 
          // operation.
        __TRACE( KVerbose,( _L( "CUnderTaker::RunL Crash during synchronous operation" ) ) );
        iModuleContainer->iErrorResult = KErrDied;
        iModuleContainer->Cancel();
        iModuleContainer->iUpAndRunning = EFalse;

        // Print notification
        TName operationText = _L("Test module died when calling:");
        operationText.Append( iModuleContainer->iOperationText );

        // Print to log file        
        __TRACE( KError,( CStifLogger::ERed, operationText ) );
        // Print to UI
        iModuleContainer->iCTestModule->ErrorPrint ( 1, operationText );        

        // Stop active scheduler.
        CActiveScheduler::Stop();

        // Execution continues from CTestModuleContainer::StartAndWaitOperation.

        }
    else
        {// Test case was running, 

        __TRACE( KError,( _L( "Test case execution aborted" ) ) );
        __TRACE( KVerbose,( _L( "CUnderTaker::Run crash during test execution" ) ) );      

        TInt caseResult = KErrGeneral;
        
        // Check if case have any special panic or exception codes that are "passed"
        if ( iModuleContainer->iAllowedExitReason != 
             CTestModuleIf::ENormal )            
            {
                
            if ( iModuleContainer->iAllowedExitReason == 
                 CTestModuleIf::EPanic &&
                 exitType == TFullTestResult::ECasePanic &&
                 iModuleContainer->iAllowedExitCode == exitReason )
                {
                exitType = TFullTestResult::ECaseExecuted;
                caseResult = KErrNone;
                exitCategory = _L("Allowed case panic");
                
                __TRACE( KError,( _L( "Test case execution panic, setting to OK" ) ) );
                }

            if ( iModuleContainer->iAllowedExitReason == 
                 CTestModuleIf::EException &&
                 exitType == TFullTestResult::ECaseException &&
                 iModuleContainer->iAllowedExitCode == exitReason )
                {
                exitType = TFullTestResult::ECaseExecuted;
                caseResult = KErrNone;
                exitCategory = _L("Allowed case exception");

                __TRACE( KError,( _L( "Test case execution exception, setting to OK" ) ) );
                }
            
            
            }

        // This was asynchronous request, and nobody is waiting for it
        // completion in server, so all clean-up must be done here

        // Forget iTestThreadContainer pointer, memory is freed because test thread
        // is destroyed   
        // This is needed to prevent from accessing memory from thread that is destroyed.
        // Do this only if test is been executed.
        iModuleContainer->iThreadContainer = NULL;

        if (iModuleContainer->iCTestExecution != NULL)
        	{
			// Execution thread have been killed
			iModuleContainer->iCTestExecution->SetThreadState( CTestExecution::ECancelled );
	
			
			// Cancel the request
			iModuleContainer->iCTestExecution->CompleteTestExecution( KErrNone,
																	  exitType,
																	  exitReason,
																	  caseResult,
																	  exitCategory );
	
			// If the print queue is empty, and there is active print
			// request, then cancel that request. 
			// NOTE: Case queue not empty AND request waiting AND
			//       thread terminated can't happen due iPrintMutex
			iModuleContainer->iCTestExecution->CompletePrintRequestIfQueueEmpty();
        	}
        else
        	{
        	__TRACE( KError,( _L( "STIF internal error - iCTestExecution is NULL in CUnderTaker::RunL." ) ) );
        	User::Leave(KErrGeneral);
        	}
        // Free the memory
        // Remove undertaker from module container to make sure that
        // it does not delete this.
        iModuleContainer->iUnderTaker = NULL;
        delete iModuleContainer;
        iModuleContainer = NULL;

        // Suicide. "Parent", i.e iModuleContainer has already died
        // and no-one have pointer to this active object, so die.
        delete this;

        }
       
    }


/*
-------------------------------------------------------------------------------

    Class: CUnderTaker

    Method: DoCancel

    Description: Stops thread monitoring

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/

void CUnderTaker::DoCancel()
    {

    if( iCancelNeeded && iModuleContainer )
        {
        iModuleContainer->iThread.LogonCancel( iStatus );
        }
    else
        {
        // Note that iModuleContainer can be NULL if iCancelNeeded is EFalse
        __ASSERT_ALWAYS( iModuleContainer, 
                         CTestServer::PanicServer( ENullTestModuleContainer ) );
        }

    }

/*
-------------------------------------------------------------------------------

    Class: CUnderTaker

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
TInt CUnderTaker::RunError( TInt aError )
    {
    __TRACE( KError,( _L( "CUnderTaker::RunError" ) ) );

    return aError;

    }

/*
-------------------------------------------------------------------------------

    Class: CUnderTaker

    Method: Completed

    Description: Is request completed

    Parameters: None

    Return Values:  TBool                           Completed?

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
TBool CUnderTaker::Completed()
    {

    if( iStatus == KRequestPending )
        {
        return EFalse;
        }
    else
        {
        return ETrue;
        }

    }

/*
-------------------------------------------------------------------------------

    Class: CUnderTaker

    Method: SetSynchronousMode

    Description: Set or unsets local processing mode

    Parameters: const TBool aMode             :in:  Mode

    Return Values:  None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CUnderTaker::SetSynchronousMode( const TBool aMode )
    {

    iSynchronousMode = aMode;
   
    }

// End of File
