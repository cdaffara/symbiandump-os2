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
* Description: CUIEngineRunner: This object executes test cases 
* from STIF Test Framework.
*
*/

// INCLUDE FILES
#include <e32base.h>
#include <e32svr.h>
#include <stifinternal/UIEngineContainer.h>
#include "UIEngineRunner.h"
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
#define LOGGER iUIEngineContainer->iUIEngine->iLogger


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

    Class: CUIEngineRunner

    Method: CUIEngineRunner

    Description: Default constructor

    C++ default constructor can NOT contain any code, that
    might leave.

    Parameters: None
    
    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
CUIEngineRunner::CUIEngineRunner( CUIEngineContainer* aUIEngineContainer) :
    CActive( CActive::EPriorityStandard ),
    iUIEngineContainer( aUIEngineContainer ),
    iFullTestResultPckg( iFullTestResult )
    {
    __TRACE( KPrint, ( _L( "CUIEngineRunner::CUIEngineRunner") ) );
    __ASSERT_ALWAYS( aUIEngineContainer, User::Panic( _L("Null pointer"), KErrGeneral ) );

    CActiveScheduler::Add( this );
    }


/*
-------------------------------------------------------------------------------

    Class: CUIEngineRunner

    Method: ConstructL

    Description: Symbian OS second phase constructor

    Symbian OS default constructor can leave.

    Parameters: None

    Return Values: None

    Errors/Exceptions: Leaves if called Open method returns error

    Status: Approved

-------------------------------------------------------------------------------
*/
void CUIEngineRunner::ConstructL()
    {

    }


/*
-------------------------------------------------------------------------------

    Class: CUIEngineRunner

    Method: NewL

    Description: Two-phased constructor.
    
    Parameters: CUIEngineContainer* CUIEngineContainer: in: Pointer to CUIEngineContainer Interface
                TTestInfo& aTestInfo: in: Test info

    Return Values: CUIEngineRunner* : Pointer to created runner object

    Errors/Exceptions: Leaves if memory allocation for CUIEngineRunner fails
                       Leaves if ConstructL leaves

    Status: Draft

-------------------------------------------------------------------------------
*/
CUIEngineRunner* CUIEngineRunner::NewL( CUIEngineContainer* aUIEngineContainer )
    {
    CUIEngineRunner* self =  
        new ( ELeave ) CUIEngineRunner( aUIEngineContainer );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }


/*
-------------------------------------------------------------------------------

    Class: CUIEngineRunner

    Method: ~CUIEngineRunner

    Description: Destructor
    
    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
CUIEngineRunner::~CUIEngineRunner()
    {
    __TRACE( KPrint, ( _L( "CUIEngineRunner::~CUIEngineRunner()") ) );
    Cancel();
    
    }


/*
-------------------------------------------------------------------------------

    Class: CUIEngineRunner

    Method: StartTestL

    Description: Starts testing

    Parameters: RTestCase& aTestCase: in: Handle to test case
                TFullTestResultPckg& aFullTestResultPckg: in: Handle to TFullTestResultPckg

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
void CUIEngineRunner::StartL( RTestCase& aTestCase )
    {
    __TRACE( KPrint, ( _L( "CUIEngineRunner::StartTestL") ) );

    iTestCase = aTestCase;

    if ( IsActive() )
        User::Leave( KErrInUse );

    SetActive();
    iTestCase.RunTestCase( iFullTestResultPckg, iStatus );
    }

/*
-------------------------------------------------------------------------------

    Class: CUIEngineRunner

    Method: RunL

    Description: RunL handles completed requests.

    Parameters: None

    Return Values: None

    Errors/Exceptions: Leaves if iStatus is not KErrNone, error is handled in
                       RunError called by CActiveObject

    Status: Draft

-------------------------------------------------------------------------------
*/
void CUIEngineRunner::RunL()
    {
    __TRACE( KPrint, ( _L( "CUIEngineRunner::RunL") ) );

    // Complete with the result of the test case
    iUIEngineContainer->TestCaseExecuted( iFullTestResult, iStatus.Int() );

    }

/*
-------------------------------------------------------------------------------

    Class: CUIEngineRunner

    Method: DoCancel

    Description: Cancel active request.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
void CUIEngineRunner::DoCancel()
    {

    __TRACE( KPrint, ( _L( "CUIEngineRunner::DoCancel") ) );
    iTestCase.CancelAsyncRequest( ETestCaseRunTestCase );
    
    }

/*
-------------------------------------------------------------------------------

    DESCRIPTION

    CActiveTimer: This object prints running seconds to console screen.

-------------------------------------------------------------------------------
*/

// ================= MEMBER FUNCTIONS =========================================




// ================= OTHER EXPORTED FUNCTIONS ================================= 
// None

// End of File
