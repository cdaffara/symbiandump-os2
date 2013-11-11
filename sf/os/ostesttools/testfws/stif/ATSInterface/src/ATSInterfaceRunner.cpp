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
* Description: CATSInterfaceRunner: This object executes test 
* cases from Test Framework.
*
*/

// INCLUDE FILES
#include <e32base.h>
#include <e32cons.h>
#include <e32svr.h>
#include "ATSInterface.h"
#include "ATSInterfaceRunner.h"


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

    DESCRIPTION

    CATSInterfaceRunner: This object executes test cases from Test Framework.

-------------------------------------------------------------------------------
*/

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

    Class: CATSInterfaceRunner

    Method: CATSInterfaceRunner

    Description: Default constructor

    C++ default constructor can NOT contain any code, that
    might leave.
    
    Parameters: None
    
    Return Values: None

    Errors/Exceptions: None

    Status: Approved
    
-------------------------------------------------------------------------------
*/
CATSInterfaceRunner::CATSInterfaceRunner( CATSInterface* aATSInterface,
                                   TTestInfo& aTestInfo ) : 
    CActive( CActive::EPriorityStandard ),
    iATSInterface( aATSInterface ),
    iTestInfo( aTestInfo ),
    iTestInfoPckg( iTestInfo ),
    iFullTestResultPckg( iFullTestResult )
    {
    __ASSERT_ALWAYS( aATSInterface, User::Panic( _L("Null pointer"), KErrGeneral ) );

    CActiveScheduler::Add( this );

    }

/*
-------------------------------------------------------------------------------

    Class: CATSInterfaceRunner

    Method: ConstructL

    Description: Symbian OS second phase constructor

    Symbian OS default constructor can leave.
    
    Parameters: None

    Return Values: None

    Errors/Exceptions: Leaves if called Open method returns error

    Status: Approved
    
-------------------------------------------------------------------------------
*/
void CATSInterfaceRunner::ConstructL()
    {
    iTestEngineServ = iATSInterface->TestEngineServer();
    iTestEngine = iATSInterface->TestEngine();

    User::LeaveIfError( iTestCase.Open( iTestEngineServ, iTestInfoPckg ) );

    }

/*
-------------------------------------------------------------------------------

    Class: CATSInterfaceRunner

    Method: NewL

    Description: Two-phased constructor.
    
    Parameters: CATSInterface* aATSInterface: in: pointer to ATS Interface
                TTestInfo& aTestInfo: in: Test info

    Return Values: CATSInterfaceRunner* : pointer to created runner object

    Errors/Exceptions: Leaves if memory allocation for CATSInterfaceRunner fails
                       Leaves if ConstructL leaves

    Status: Approved
    
-------------------------------------------------------------------------------
*/
CATSInterfaceRunner* CATSInterfaceRunner::NewL( CATSInterface* aATSInterface,
                                         TTestInfo& aTestInfo )
    {
    CATSInterfaceRunner* self =  
        new ( ELeave ) CATSInterfaceRunner( aATSInterface, aTestInfo );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

/*
-------------------------------------------------------------------------------

    Class: CATSInterfaceRunner

    Method: ~CATSInterfaceRunner

    Description: Destructor
    
    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved
    
-------------------------------------------------------------------------------
*/    
CATSInterfaceRunner::~CATSInterfaceRunner()
    {
    Cancel();
    iTestCase.Close();
    }

/*
-------------------------------------------------------------------------------

    Class: CATSInterfaceRunner

    Method: StartTestL

    Description: Starts testing

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved
    
-------------------------------------------------------------------------------
*/
void CATSInterfaceRunner::StartTestL()
    {
#ifdef _DEBUG
    RDebug::Print(_L("Start test case %d: "), iTestInfo.iTestCaseInfo.iCaseNumber );
#endif

    if ( IsActive() )
        Cancel();

    iTestCase.RunTestCase( iFullTestResultPckg, iStatus );

    SetActive();
    }

/*
-------------------------------------------------------------------------------

    Class: CATSInterfaceRunner

    Method: RunL

    Description: RunL handles completed requests.

    Parameters: None

    Return Values: None

    Errors/Exceptions: Leaves if iStatus is not KErrNone, error is handled in
                       RunError called by CActiveObject

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
void CATSInterfaceRunner::RunL()
    {
#ifdef _DEBUG
    RDebug::Print(_L("RunTestCase completed: [%d] "), iStatus.Int() );
#endif
    // Check if the test case could not be executed, error handled in RunError
    User::LeaveIfError ( iStatus.Int() );

    // Check if the test case crashed, error handler in RunError
    if ( iFullTestResult.iCaseExecutionResultType !=
         TFullTestResult::ECaseExecuted )
        {
#ifdef _DEBUG
        RDebug::Print(_L("Test case execution failed: [%d] "), 
            iFullTestResult.iCaseExecutionResultCode );
#endif
        // Complete with the result of the test case
        iATSInterface->TestCompleted( iFullTestResult.iCaseExecutionResultCode );

        }
    else
        {
#ifdef _DEBUG
        // Debug test result
        RDebug::Print( _L("Test case execution completed[%d]: %S"), 
                            iFullTestResult.iTestResult.iResult,  
                            &iFullTestResult.iTestResult.iResultDes);
#endif

        // Complete with the result of the test case
        iATSInterface->TestCompleted( iFullTestResult.iTestResult.iResult );
        }

    }

/*
-------------------------------------------------------------------------------

    Class: CATSInterfaceRunner

    Method: DoCancel

    Description: Cancel active request.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved
    
-------------------------------------------------------------------------------
*/
void CATSInterfaceRunner::DoCancel()
    {
    // Cancel the active request
    iTestCase.CancelAsyncRequest( ETestCaseRunTestCase );

    }

/*
-------------------------------------------------------------------------------

    Class: CATSInterfaceRunner

    Method: RunError

    Description: Handle errors from TestFramework

    Parameters: TInt aError: in: Symbian OS error: Error code
    
    Return Values: TInt KErrNone: Always returned KErrNone

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
TInt CATSInterfaceRunner::RunError( TInt aError )
    {
#ifdef _DEBUG
    RDebug::Print(_L("Test case execution failed: [%d] "), 
            aError );
#endif
    iATSInterface->TestCompleted ( aError );

    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    DESCRIPTION

    CActiveTimer: This object prints running seconds to console screen.

-------------------------------------------------------------------------------
*/

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

    Class: CActiveTimer

    Method: CActiveTimer

    Description: Default constructor

    C++ default constructor can NOT contain any code, that
    might leave.
    
    Parameters: CConsoleBase* aConsole: in: Pointer to CConsoleBase
    
    Return Values: None

    Errors/Exceptions: None

    Status: Approved
    
-------------------------------------------------------------------------------
*/
CActiveTimer::CActiveTimer( CConsoleBase* aConsole ) : 
    CTimer( CActive::EPriorityStandard ),
    iConsole( aConsole )
    {
    CActiveScheduler::Add( this );
    }

/*
-------------------------------------------------------------------------------

    Class: CActiveTimer

    Method: ConstructL

    Description: Symbian OS second phase constructor

    Symbian OS default constructor can leave.
    
    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved
    
-------------------------------------------------------------------------------
*/
void CActiveTimer::ConstructL()
    {
    //Base class 2nd phase constructor
    CTimer::ConstructL();

    // Set console positions
    iXPos = 0;
    iYPos = 1;

    }

/*
-------------------------------------------------------------------------------

    Class: CActiveTimer

    Method: NewL

    Description: Two-phased constructor.
    
    Parameters: CConsoleBase* aConsole: in: Pointer to CConsoleBase

    Return Values: CActiveTimer* : pointer to created CActiveTimer object

    Errors/Exceptions: Leaves if memory allocation for CATSInterface fails
                       Leaves if ConstructL leaves

    Status: Approved
    
-------------------------------------------------------------------------------
*/
CActiveTimer* CActiveTimer::NewL( CConsoleBase* aConsole )
    {
    CActiveTimer* self =  
        new ( ELeave ) CActiveTimer( aConsole );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

/*
-------------------------------------------------------------------------------

    Class: CActiveTimer

    Method: ~CActiveTimer

    Description: Destructor
    
    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved
    
-------------------------------------------------------------------------------
*/    
CActiveTimer::~CActiveTimer()
    {
    Cancel();
    }

/*
-------------------------------------------------------------------------------

    Class: CActiveTimer

    Method: StartTestL

    Description: Starts testing

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved
    
-------------------------------------------------------------------------------
*/
void CActiveTimer::StartL()
    {
    iStartTime.HomeTime();
    // Wait a moment
    CTimer::After( KPrintInterval );
    }

/*
-------------------------------------------------------------------------------

    Class: CActiveTimer

    Method: RunL

    Description: RunL handles completed requests.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved
    
-------------------------------------------------------------------------------
*/
void CActiveTimer::RunL()
    {
    // Print time
    TTimeIntervalSeconds seconds;
    TTime time;
    time.HomeTime();
    time.SecondsFrom( iStartTime, seconds );

    TInt x = iConsole->WhereX();
    TInt y = iConsole->WhereY();

    // Print time to screen
    iConsole->SetPos( iXPos, iYPos );
    iConsole->ClearToEndOfLine();
    iConsole->Printf( _L("[Time: %d] "), seconds.Int() );

    iConsole->SetPos( x, y );
    // Wait a moment
    CTimer::After( KPrintInterval );

    }

/*
-------------------------------------------------------------------------------

    Class: CActiveTimer

    Method: DoCancel

    Description: Cancel

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved
    
-------------------------------------------------------------------------------
*/
void CActiveTimer::DoCancel()
    {
    // Print time
    TTimeIntervalSeconds seconds;
    TTime time;
    time.HomeTime();
    time.SecondsFrom( iStartTime, seconds );

    // Print time to screen
    iConsole->Printf( _L("Total Time: [%d] "), seconds.Int() );
    // Cancel the active request
    CTimer::DoCancel();

    }

// ================= OTHER EXPORTED FUNCTIONS ================================= 
// None

// End of File
