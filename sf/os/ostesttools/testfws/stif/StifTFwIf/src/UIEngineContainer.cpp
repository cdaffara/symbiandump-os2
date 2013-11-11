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
* Description: CUIEngine: This object executes test cases from STIF 
* Test Framework.
*
*/

// INCLUDE FILES
#include <e32base.h>
#include <e32svr.h>
#include <stifinternal/UIEngine.h>
#include <stifinternal/UIEngineContainer.h>
#include "UIEnginePrinter.h"
#include "UIEngineRunner.h"
#include "UIEngineRemote.h"
#include "Logging.h"


// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES  
// None

// CONSTANTS
// None

// MACROS
#ifdef LOGGER
#undef LOGGER
#endif
#define LOGGER iUIEngine->iLogger

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

    Class: CUIEngineContainer

    Method: CUIEngineContainer

    Description: Default constructor

    C++ default constructor can NOT contain any code, that
    might leave.

    Parameters:  CUIEngine* aUIEngine: in: Pointer to CUIEngine interface
                 TTestInfo& aTestInfo: in: Test information

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
CUIEngineContainer::CUIEngineContainer( CUIEngine* aUIEngine,
                                   const TTestInfo& aTestInfo,
                                   RTestEngineServer& aTestEngineServ,
                                   RTestEngine& aTestEngine ) :
    iUIEngine( aUIEngine ),
    iTestInfo( aTestInfo ),
    iTestInfoPckg( iTestInfo ),
    iState( ENotStarted )
    {
    __TRACE( KPrint, (  _L( "CUIEngineContainer::CUIEngineContainer.") ) );

    // Get handles to test engine
    iTestEngineServ = aTestEngineServ;
    iTestEngine = aTestEngine;

    __ASSERT_ALWAYS( aUIEngine, User::Panic( _L("Null pointer"), KErrGeneral ) );

    }

/*
-------------------------------------------------------------------------------

    Class: CUIEngineContainer

    Method: ConstructL

    Description: Symbian OS second phase constructor

    Symbian OS default constructor can leave

    Parameters: None

    Return Values: None

    Errors/Exceptions: Leaves if called Open method returns error

    Status: Draft

-------------------------------------------------------------------------------
*/
void CUIEngineContainer::ConstructL()
    {
    __TRACE( KPrint, (  _L( "CUIEngineContainer::ConstructL.") ) );

    // Open test case
    User::LeaveIfError( iTestCase.Open( iTestEngineServ, iTestInfoPckg ) );
    }

/*
-------------------------------------------------------------------------------

    Class: CUIEngineContainer

    Method: NewL

    Description: Two-phased constructor.

    Parameters: CUIEngine* aUIEngine: in: pointer to CUIEngine Interface
                TTestInfo& aTestInfo: in: Test info

    Return Values: CUIEngineRunner* : pointer to created runner object

    Errors/Exceptions: Leaves if memory allocation for CUIEngineRunner fails
                       Leaves if ConstructL leaves

    Status: Draft

-------------------------------------------------------------------------------
*/
CUIEngineContainer* CUIEngineContainer::NewL( CUIEngine* aUIEngine,
                                            const TTestInfo& aTestInfo,
                                            RTestEngineServer& aTestEngineServ,
                                            RTestEngine& aTestEngine )
    {
    CUIEngineContainer* self =  
        new ( ELeave ) CUIEngineContainer( aUIEngine,
                                            aTestInfo,
                                            aTestEngineServ,
                                            aTestEngine );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

/*
-------------------------------------------------------------------------------

    Class: CUIEngineContainer

    Method: ~CUIEngineContainer

    Description: Destructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/    
CUIEngineContainer::~CUIEngineContainer()
    {
    
    __TRACE( KPrint, (  _L( "CUIEngineContainer::~CUIEngineContainer.") ) );

    delete iPrinter;
    delete iRunner;
    delete iRemote;
    
    // Close test case
    iTestCase.Close();
    
    }


/*
-------------------------------------------------------------------------------

    Class: CUIEngineContainer

    Method: StartContinerL

    Description: StartContinerL call CUIEngineRunner to starting test case execution

    Parameters: None

    Return Values: TInt KErrNone: Always returned KErrNone

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
TInt CUIEngineContainer::StartContainerL()
    {
    
    __TRACE( KPrint, ( _L( "CUIEngineContainer::StartContainerL. Running test case: %S"), 
        &( iTestInfo.iTestCaseInfo.iTitle ) ) );
        
    // Start printer
    iPrinter = CUIEnginePrinter::NewL( this );
    iPrinter->StartL( iTestCase );

    // Start remote
    iRemote = CUIEngineRemote::NewL( this );
    iRemote->StartL( iTestCase );

    // Start runner
    iRunner = CUIEngineRunner::NewL( this );
    iRunner->StartL( iTestCase );

    // Update state of test case
    iState = ERunning;

    __TRACE( KPrint, ( _L( "CUIEngineContainer::StartContainerL: started") ) );

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CUIEngineContainer

    Method: CancelTest

    Description: CancelTest cancels test case execution

    Parameters: None

    Return Values: TInt KErrNone: Always returned KErrNone

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CUIEngineContainer::CancelTest()
    {

    if ( iState == ERunning || iState == EPaused )
        {
        __TRACE( KPrint, ( _L( "CUIEngineContainer::CancelTest") ) );

        // Cancel test case execution
        iTestCase.CancelAsyncRequest( ETestCaseRunTestCase );
        
        }

    return KErrNone;
    }


/*
-------------------------------------------------------------------------------

    Class: CUIEngineContainer

    Method: PauseTest

    Description: PauseTest pauses test case execution

    Parameters: None

    Return Values: Symbian OS error: Error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CUIEngineContainer::PauseTest()
    {
    TInt ret(KErrNone);

    if ( iState == ERunning )
        {
        ret = iTestCase.Pause();

        __TRACE( KPrint, ( _L( "CUIEngineContainer::PauseTest. Ret = %d"), ret ) );

        if ( ret == KErrNone)
            {
            iState = EPaused;
            }
        }

    return ret;
    }


/*
-------------------------------------------------------------------------------

    Class: CUIEngineContainer

    Method: ResumeTest

    Description: PauseTest resumes test case execution

    Parameters: None

    Return Values: Symbian OS error: Error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CUIEngineContainer::ResumeTest()
    {
    TInt ret(KErrNone);

    if ( iState == EPaused )
        {
        ret = iTestCase.Resume();

        __TRACE( KPrint, ( _L( "CUIEngineContainer::ResumeTest. Ret = %d"), ret ) );

        if ( ret == KErrNone)
            {
            iState = ERunning;
            }
        }

    return ret;
    }


/*
-------------------------------------------------------------------------------

    Class: CUIEngineContainer

    Method: TestCaseExecuted

    Description: TestCaseExecuted informs that test case is executed

    Parameters: TFullTestResult& aFullTestResult: in: Full information of 
                test case execution. Includes also test case results

    Return Values: None

    Errors/Exceptions: TInt KErrNone: Always returned KErrNone

    Status: Draft

-------------------------------------------------------------------------------
*/
void CUIEngineContainer::TestCaseExecuted( TFullTestResult& aFullTestResult, 
                                           TInt aStatus )
    {
        
    iState = EExecuted;

    // Set execution result
    if( ( KErrNone != aStatus ) &&
        ( aFullTestResult.iCaseExecutionResultCode == KErrNone ) &&
        ( aFullTestResult.iTestResult.iResult == KErrNone ) )
        {
        aFullTestResult.iTestResult.iResultDes.Copy( 
            _L("Completed with error") );
        aFullTestResult.iCaseExecutionResultType = 
            TFullTestResult::ECaseErrorFromModule;
        aFullTestResult.iCaseExecutionResultCode = aStatus;
        }

    __TRACE( KPrint, ( _L( "CUIEngineContainer::TestCaseExecuted.") ) );

    // Test execution is not completed before all prints are 
    // delivered to UI.
    if( !iPrinter->IsRunning() || 
        ( aStatus != KErrNone ) || 
        ( aFullTestResult.iCaseExecutionResultCode != KErrNone ) )
        {        
        // Call TestExecuted of CUIEngine interface
        iUIEngine->TestExecuted(this, aFullTestResult);
        // 1. This method is called from CUIEngineRunner::RunL() and it is 
        // last operation => no other call to CUIEngineContainer class.
        // 2. TestExecuted() method takes this pointer. Inside TestExecuted()
        // will be deleted the this pointer => iUIEngine not valid anymore.
        // 1 and 2 => Do not handle any CUIEngineContainer variable here
        // because memory is not valid anymore.
        //e.g. iState = EFinished; // illegal operation here->crash

        }
        
    }


/*
-------------------------------------------------------------------------------

    Class: CUIEngineContainer

    Method: PrintProgress

    Description: PrintProgress prints information from test case

    Parameters: TTestProgress& aProgress: in: Progress info

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
void CUIEngineContainer::PrintProgress( TTestProgress& aProgress )
    {
    if( iState == ERunning )
        {
        // Only running cases may print
        iUIEngine->PrintProg( this, aProgress );
        }
    else
        {
        __TRACE( KPrint, 
            ( _L("%S: %S"), &aProgress.iDescription, &aProgress.iText ) );
        }
    }
    
/*
-------------------------------------------------------------------------------

    Class: CUIEngineContainer

    Method: PrintsDone

    Description: Signals that printing is completed

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
void CUIEngineContainer::PrintsDone()
    {
    if( iState == EExecuted )
        {
        // Call TestExecuted of CUIEngine interface
        iUIEngine->TestExecuted(this, iRunner->FullResult() );
        }
    }
    
/*
-------------------------------------------------------------------------------

    Class: CUIEngineContainer

    Method: RemoteSend

    Description: RemoteSend forwards remote protocol messages from master

    Parameters: 
    
    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
void CUIEngineContainer::RemoteSend( const TDesC& aRemoteMsg, 
                                     TInt /* aStatus */ )
    {

    __TRACE( KPrint, ( _L( "CUIEngineContainer::RemoteSend.") ) );

    // Save master id if doesn't exists already
    if( iMasterId == 0 )
        {
        CStifTFwIfProt* msg = NULL;
        TRAPD( err, 
            msg = CStifTFwIfProt::NewL(); 
            msg->SetL( aRemoteMsg );
            );
        if( err != KErrNone )
            {
            delete msg;
            // oom
            return;
            }
        iMasterId = msg->SrcId();
        delete msg;
        }
            
    // Call AtsSend of CUIEngine interface
    iUIEngine->RemoteMsg( this, aRemoteMsg );

    }

/*
-------------------------------------------------------------------------------

    Class: CUIEngineContainer

    Method: GoingToReboot

    Description: Inform UI that phone is going to rebooted

    Parameters: 
    
    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
TInt CUIEngineContainer::GoingToReboot( TRequestStatus& aStatus )
    {

    __TRACE( KPrint, ( _L( "CUIEngineContainer::GoingToReboot.") ) );
    
    // Forward            
    return iUIEngine->GoingToReboot( this, aStatus );

    }
    
/*
-------------------------------------------------------------------------------

    Class: CUIEngineContainer

    Method: RemoteReceive

    Description: RemoteReceive forwards remote protocol messages to master

    Parameters: 
    
    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
TInt CUIEngineContainer::RemoteReceive( const TDesC& aRemoteMsg )
    {

    __TRACE( KPrint, ( _L( "CUIEngineContainer::RemoteReceive.") ) );
    
    // Check master test case status, FIX110
    if( iState != ERunning )
        {
        __TRACE( KError, ( _L( "Master case not running") ) );

        return KErrNotFound;
        }
    
    HBufC8* buf = HBufC8::New( aRemoteMsg.Length() );
    if( buf == NULL )
        { 
        return KErrNoMemory;
        }
      
    TPtr8 tmp = buf->Des();
    tmp.Copy( aRemoteMsg );     
    TInt ret = iTestCase.NotifyRemoteMsg( tmp, EStifCmdReceive );
    delete buf;
  
    return ret;
    
    }

// ================= OTHER EXPORTED FUNCTIONS ================================= 

// End of File
