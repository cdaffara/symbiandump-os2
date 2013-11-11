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
* Description: CUIEngineErrorPrinter handles error prints from 
* test framework.
*
*/

// INCLUDE FILES
#include <e32base.h>
#include <e32svr.h>
#include <stifinternal/UIEngineContainer.h>
#include "UIEngineError.h"
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

    Class: CUIEngineErrorPrinter

    Method: NewL

    Description: Create a testcase runner.

    Parameters: CUIEngineContainer* aContainer: in: Pointer to testcase container
    
    Return Values: CUIEngineErrorPrinter* : pointer to created object

    Errors/Exceptions: Leaves if memory allocation for object fails
                       Leaves if ConstructL leaves

    Status: Draft
    
-------------------------------------------------------------------------------
*/
CUIEngineErrorPrinter* CUIEngineErrorPrinter::NewL( 
                                CUIEngine* aUIEngine )
    {

    CUIEngineErrorPrinter* self = 
        new ( ELeave ) CUIEngineErrorPrinter( aUIEngine );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;

    }

/*
-------------------------------------------------------------------------------

    Class: CUIEngineErrorPrinter

    Method: ConstructL

    Description: Second phase constructor.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft
    
-------------------------------------------------------------------------------
*/
void CUIEngineErrorPrinter::ConstructL()
    {

    }

/*
-------------------------------------------------------------------------------

    Class: CUIEngineErrorPrinter

    Method: CUIEngineErrorPrinter

    Description: Constructor.
    
    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft
    
-------------------------------------------------------------------------------
*/
CUIEngineErrorPrinter::CUIEngineErrorPrinter( CUIEngine* aUIEngine ): 
    CActive( EPriorityStandard ),
    iUIEngine( aUIEngine ),
    iErrorPckg( iError )
    {
    
     __TRACE( KPrint, ( _L( "CUIEngineErrorPrinter::CUIEngineErrorPrinter") ) );
    __ASSERT_ALWAYS( aUIEngine, User::Panic( _L("Null pointer"), KErrGeneral ) );
    
    CActiveScheduler::Add( this );

    }

/*
-------------------------------------------------------------------------------

    Class: CUIEngineErrorPrinter

    Method: ~CUIEngineErrorPrinter

    Description: Destructor.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft
    
-------------------------------------------------------------------------------
*/
CUIEngineErrorPrinter::~CUIEngineErrorPrinter( )
    {
    
    __TRACE( KPrint, ( _L( "CUIEngineErrorPrinter::~CUIEngineErrorPrinter") ) );
    Cancel();
    
    }

/*
-------------------------------------------------------------------------------

    Class: CUIEngineErrorPrinter

    Method: StartL

    Description: Starts a test case and sets the active object to active.

    Parameters: RTestCase& aTestCase: in: Reference to the testcase object

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
void CUIEngineErrorPrinter::StartL( RTestEngine& aEngine )
    {
    
    iTestEngine = aEngine;
 
    if ( IsActive() )
        User::Leave( KErrInUse );

    SetActive();   
    iTestEngine.ErrorNotification ( iErrorPckg, iStatus );
    
    }



/*
-------------------------------------------------------------------------------

    Class: CUIEngineErrorPrinter

    Method: RunL

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
void CUIEngineErrorPrinter::RunL()
    {

    if( iStatus != KErrNone )
        {
        return;
        }
        
    iUIEngine->ErrorPrint( iError );
    
    // Set new request
    SetActive();   
    iTestEngine.ErrorNotification ( iErrorPckg, iStatus );

    }



/*
-------------------------------------------------------------------------------

    Class: CUIEngineErrorPrinter

    Method: DoCancel

    Description: Cancels the asynchronous request

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft
    
-------------------------------------------------------------------------------
*/
void CUIEngineErrorPrinter::DoCancel()
    {

    iTestEngine.CancelAsyncRequest ( ETestEngineErrorNotification );

    }

// ================= OTHER EXPORTED FUNCTIONS ================================= 
// None

// End of File
