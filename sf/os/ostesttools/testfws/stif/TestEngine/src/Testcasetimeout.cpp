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
* CTestCaseTimeout class member functions.
*
*/

// INCLUDE FILES
#include <e32svr.h>
#include <hal.h>
#include <StifLogger.h>
#include "TestReport.h"
#include "TestEngine.h"
#include "TestEngineCommon.h"
#include "TestCaseController.h"

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

    Class: CTestCaseTimeout

    Method: CTestCaseTimeout

    Description: Default constructor

    C++ default constructor can NOT contain any code, that
    might leave.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CTestCaseTimeout::CTestCaseTimeout() : CActive (CActive::EPriorityStandard)
    {

    }

/*
-------------------------------------------------------------------------------

    Class: CTestCaseTimeout

    Method: ConstructL

    Description: Symbian OS second phase constructor

    Symbian OS default constructor can leave.

    Parameters: 

    Return Values: None

    Errors/Exceptions: 

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestCaseTimeout::ConstructL( CTestCaseController* aCase,
                                   TTimeIntervalMicroSeconds aTimeout )
    {
    iCase = aCase;
    iTimeout = aTimeout;  
    iTimer.CreateLocal();

	iTestCaseTimeout = 0;  // Initialize

    }

/*
-------------------------------------------------------------------------------

    Class: CTestCaseTimeout

    Method: NewL

    Description: Two-phased constructor.

    Parameters: const TTestReportOutput aReportOutput: in: Report output type

    Return Values: CTestCaseTimeout* : pointer to created object

    Errors/Exceptions: Leaves if memory allocation for object fails
                       Leaves if ConstructL leaves

    Status: Approved

-------------------------------------------------------------------------------
*/
CTestCaseTimeout* CTestCaseTimeout::NewL( CTestCaseController* aCase, 
                                          TTimeIntervalMicroSeconds aTimeout )
    {
    CTestCaseTimeout* self = new ( ELeave ) CTestCaseTimeout();
    CleanupStack::PushL( self );
    self->ConstructL( aCase, aTimeout );
    CleanupStack::Pop( self );
    return self;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestCaseTimeout

    Method: ~CTestCaseTimeout

    Description: Destructor.

    Cancel request

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CTestCaseTimeout::~CTestCaseTimeout()
    {
    Cancel();

    }

/*
-------------------------------------------------------------------------------

    Class: CTestCaseTimeout

    Method: Start

    Description: Start timeout counting

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestCaseTimeout::Start()
    {
    // Add to active scheduler
    CActiveScheduler::Add ( this );
    SetActive();

    // Request timer
    TTime timeout;
    timeout.HomeTime();
    timeout = timeout + iTimeout;
    
    // Store absolute timeout
    iTestCaseTimeout = timeout;

    // Note: iTimer.After() method cannot use because there needed
    // TTimeIntervalMicroSeconds32 and it is 32 bit. So then cannot create 
    // timeout time that is long enough. At() uses 64 bit value=>Long enough.
    iTimer.At( iStatus, timeout );

    }

/*
-------------------------------------------------------------------------------

    Class: CTestCaseTimeout

    Method: RunL

    Description: RunL handles completed timeouts.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestCaseTimeout::RunL()
    {
    // Timeout
	TTime timeout;
	timeout.HomeTime();
	// Handle the abort case when system time gets changed, but timeout is
    // still valid. All other cases should timeout since they invalidate the
    // logic of the timers.
	if ( iStatus == KErrAbort)
		{
		if ( iTestCaseTimeout > timeout )
			{  
			RDebug::Print( _L( "Absolute timer still valid. Restaring timer. iStatus: %d" ), iStatus.Int() );
			// Start new timer
			iStatus = KErrNone; // reset value
			iTimer.At ( iStatus, iTestCaseTimeout );  // restart timer
			SetActive();
			}
		else
			{
			// Absolute timer no longer valid. Must timeout.
			iCase->Timeout();
			}

		}
	else
		{
		// Status was not KErrAbort. Timing out!
		iCase->Timeout();
		}

    }

/*
-------------------------------------------------------------------------------

    Class: CTestCaseTimeout

    Method: DoCancel

    Description: Cancel active request

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestCaseTimeout::DoCancel()
    {
    iTimer.Cancel();

    }

/*
-------------------------------------------------------------------------------

    Class: CTestCaseTimeout

    Method: RunError

    Description: Handle errors. Just let framework handle errors because
    RunL does not leave.

    Parameters: TInt aError: in: Symbian OS error: Error code

    Return Values: TInt: Symbian OS error code

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CTestCaseTimeout::RunError( TInt aError )
    {
    return aError;

    }

// ================= OTHER EXPORTED FUNCTIONS =================================

// None

// End of File
