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
* Description: CUIEnginePrinter handles prints from test cases.
*
*/

// INCLUDE FILES
#include <e32base.h>
#include <e32svr.h>
#include <stifinternal/UIEngineContainer.h>
#include "UIEnginePrinter.h"
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

    Class: CUIEnginePrinter

    Method: NewL

    Description: Two-phased constructor.

    Parameters: CUIEngineContainer* CUIEngineContainer: in: Pointer to CUIEngineContainer Interface
                TTestInfo& aTestInfo: in: Test info

    Return Values: CUIEnginePrinter* : Pointer to created runner object

    Errors/Exceptions: Leaves if memory allocation for CUIEnginePrinter fails
                       Leaves if ConstructL leaves

    Status: Approved

-------------------------------------------------------------------------------
*/
CUIEnginePrinter* CUIEnginePrinter::NewL( 
    CUIEngineContainer* aUIEngineContainer )
    {
    CUIEnginePrinter* self =
        new ( ELeave ) CUIEnginePrinter( aUIEngineContainer );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

/*
-------------------------------------------------------------------------------

    Class: CUIEnginePrinter

    Method: ConstructL

    Description: Symbian OS second phase constructor

    Symbian OS default constructor can leave.

    Parameters: None

    Return Values: None

    Errors/Exceptions: Leaves if called Open method returns error

    Status: Draft

-------------------------------------------------------------------------------
*/
void CUIEnginePrinter::ConstructL()
    {

    }

/*
-------------------------------------------------------------------------------

    Class: CUIEnginePrinter

    Method: CUIEnginePrinter

    Description: Default constructor

    C++ default constructor can NOT contain any code, that
    might leave.

    Parameters: None
    
    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
CUIEnginePrinter::CUIEnginePrinter( 
    CUIEngineContainer* aUIEngineContainer ) :
        CActive( CActive::EPriorityStandard ),
        iUIEngineContainer( aUIEngineContainer ),
        iProgressPckg( iProgress ),
        iRunning( EFalse )
    {
    __TRACE( KPrint, ( _L( "CUIEnginePrinter::CUIEnginePrinter") ) );
    __ASSERT_ALWAYS( aUIEngineContainer, User::Panic( _L("Null pointer"), KErrGeneral ) );

    CActiveScheduler::Add( this );

    }

/*
-------------------------------------------------------------------------------

    Class: CUIEnginePrinter

    Method: ~CUIEnginePrinter

    Description: Destructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
CUIEnginePrinter::~CUIEnginePrinter()
    {
    __TRACE( KPrint, ( _L( "CUIEnginePrinter::~CUIEnginePrinter") ) );

    Cancel();
    
    }

/*
-------------------------------------------------------------------------------

    Class: CUIEnginePrinter

    Method: StartProgressL

    Description: Starts testing

    Parameters: RTestCase& aTestCase: in: Handle to test case
                TTestProgressPckg& aProgressPckg: in: Handle to TTestProgressPckg

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
void CUIEnginePrinter::StartL( RTestCase& aTestCase )
    {
    __TRACE( KPrint, ( _L( "CUIEnginePrinter::StartProgressL.") ) );

    iTestCase = aTestCase;

    if ( IsActive() )
        User::Leave( KErrInUse );

    iRunning = ETrue;

    SetActive();
    iTestCase.NotifyProgress( iProgressPckg, iStatus );
    }


/*
-------------------------------------------------------------------------------

    Class: CUIEnginePrinter

    Method: RunL

    Description: RunL handles completed requests

    Parameters: None

    Return Values: None

    Errors/Exceptions: Leaves if iStatus is not KErrNone, error is handled in
                       RunError called by CActiveObject

    Status: Draft

-------------------------------------------------------------------------------
*/
void CUIEnginePrinter::RunL()
    {
    
    if( iStatus != KErrNone )
        {
        // KErrEof and other errors
        iRunning = EFalse;
        // Signal to container
        iUIEngineContainer->PrintsDone();
        return;
        }
        
    iUIEngineContainer->PrintProgress( iProgress );
    
    // Start a new request
    SetActive();
    iTestCase.NotifyProgress( iProgressPckg, iStatus );    
    
    }

/*
-------------------------------------------------------------------------------

    Class: CUIEnginePrinter

    Method: DoCancel

    Description: Cancel active request.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
void CUIEnginePrinter::DoCancel()
    {
    __TRACE( KPrint, ( _L( "CUIEnginePrinter::DoCancel") ) );
    
    iRunning = EFalse;

    iTestCase.CancelAsyncRequest( ETestCaseNotifyProgress );
    
    }


// ================= OTHER EXPORTED FUNCTIONS ================================= 
// None

// End of File
