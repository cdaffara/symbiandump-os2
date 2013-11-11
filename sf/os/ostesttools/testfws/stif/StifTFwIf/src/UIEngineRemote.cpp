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
* Description: CUIEngineRemote: This object executes test cases 
* from STIF Test Framework.
*
*/

// INCLUDE FILES
#include <e32base.h>
#include <e32svr.h>
#include <stifinternal/UIEngineContainer.h>
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

    Class: CUIEngineRemote

    Method: CUIEngineRemote

    Description: Default constructor

    C++ default constructor can NOT contain any code, that
    might leave.

    Parameters: None
    
    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
CUIEngineRemote::CUIEngineRemote( CUIEngineContainer* aUIEngineContainer) :
    CActive( CActive::EPriorityStandard ),
    iUIEngineContainer( aUIEngineContainer ),
    iRemoteTypePckg ( iRemoteType ),
    iMsgLenPckg ( iMsgLen ),
    iState( EIdle )
    {
    __TRACE( KPrint, ( _L( "CUIEngineRemote::CUIEngineRemote") ) );
    __ASSERT_ALWAYS( aUIEngineContainer, User::Panic( _L("Null pointer"), KErrGeneral ) );

    CActiveScheduler::Add( this );
    }


/*
-------------------------------------------------------------------------------

    Class: CUIEngineRemote

    Method: ConstructL

    Description: Symbian OS second phase constructor

    Symbian OS default constructor can leave.

    Parameters: None

    Return Values: None

    Errors/Exceptions: Leaves if called Open method returns error

    Status: Approved

-------------------------------------------------------------------------------
*/
void CUIEngineRemote::ConstructL()
    {

    }


/*
-------------------------------------------------------------------------------

    Class: CUIEngineRemote

    Method: NewL

    Description: Two-phased constructor.
    
    Parameters: CUIEngineContainer* CUIEngineContainer: in: Pointer to CUIEngineContainer Interface
                TTestInfo& aTestInfo: in: Test info

    Return Values: CUIEngineRemote* : Pointer to created runner object

    Errors/Exceptions: Leaves if memory allocation for CUIEngineRemote fails
                       Leaves if ConstructL leaves

    Status: Draft

-------------------------------------------------------------------------------
*/
CUIEngineRemote* CUIEngineRemote::NewL( CUIEngineContainer* aUIEngineContainer )
    {
    
    CUIEngineRemote* self =  
        new ( ELeave ) CUIEngineRemote( aUIEngineContainer );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    
    }


/*
-------------------------------------------------------------------------------

    Class: CUIEngineRemote

    Method: ~CUIEngineRemote

    Description: Destructor
    
    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
CUIEngineRemote::~CUIEngineRemote()
    {
    __TRACE( KPrint, ( _L( "CUIEngineRemote::~CUIEngineRemote()") ) );
    Cancel();
    
    }


/*
-------------------------------------------------------------------------------

    Class: CUIEngineRemote

    Method: StartTestL

    Description: Starts testing

    Parameters: RTestCase& aTestCase: in: Handle to test case
                TFullTestResultPckg& aFullTestResultPckg: in: Handle to TFullTestResultPckg

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
void CUIEngineRemote::StartL( RTestCase& aTestCase )
    {
    __TRACE( KPrint, ( _L( "CUIEngineRemote::StartL") ) );

    iTestCase = aTestCase;

    if ( IsActive() )
        User::Leave( KErrInUse );

    Start();
        
    }
    
/*
-------------------------------------------------------------------------------

    Class: CUIEngineRemote

    Method: Start

    Description: Start request

    Parameters: None
    
    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
void CUIEngineRemote::Start()
    {
    __TRACE( KPrint, ( _L( "CUIEngineRemote::Start") ) );

    iState = EPending;
    SetActive();
    iTestCase.NotifyRemoteType( iRemoteTypePckg, iMsgLenPckg, iStatus );
    
    }

/*
-------------------------------------------------------------------------------

    Class: CUIEngineRemote

    Method: RunL

    Description: RunL handles completed requests.

    Parameters: None

    Return Values: None

    Errors/Exceptions: Leaves if iStatus is not KErrNone, error is handled in
                       RunError called by CActiveObject

    Status: Draft

-------------------------------------------------------------------------------
*/
void CUIEngineRemote::RunL()
    {
    __TRACE( KPrint, ( _L( "CUIEngineRemote::RunL") ) );
    
    switch( iState )
        {
        case EPending:
            iState = EIdle;
            
            switch( iRemoteType )
                {
                case EStifCmdSend: // Sending message
                    {
                    if( iMsgLen <= 0 ) 
                        {
                        User::Leave( KErrGeneral );
                        }
                    HBufC8* buf = HBufC8::NewLC( iMsgLen );
                    TPtr8 tmp = buf->Des(); 
                    if( iTestCase.NotifyRemoteMsg( tmp, iRemoteType ) == KErrNone )
                        {
                        HBufC* buf2 = HBufC::NewLC( iMsgLen );
                        TPtr tmp2 = buf2->Des();
                        tmp2.Copy( tmp );
                        
                        // Forward message
                        iUIEngineContainer->RemoteSend( tmp2, iStatus.Int() );
                        CleanupStack::PopAndDestroy( buf2 );
                        }
                    CleanupStack::PopAndDestroy( buf );
                    }        
                    break;
                case EStifCmdStoreState: // Got indication that reboot will follow
                    {
                    // System will reboot, inform UI
                    // Forward message
                    SetActive();
                    TInt ret = iUIEngineContainer->GoingToReboot( iStatus );
                    if( ret != KErrNone )
                        {
                        User::Leave( ret );
                        }
                    iState = EUIMsgPending;
                    return;
                    }
                // The rest should never come here
                case EStifCmdReboot:
                case EStifCmdReceive:
                case EStifCmdGetStoredState:
                default:
                    {
                    if( iStatus.Int() != KErrServerTerminated )
                        {   
                        RDebug::Print( _L("CUIEngineRemote::RunL() -- Leaved with [%d] "), iStatus.Int() );  
                        User::Leave( KErrNotFound );  
                        }
                    break;
                    }
                }
            break;
        case EUIMsgPending:
            {
            TInt result = iStatus.Int();
            if( result == KErrCancel )
                {
                result = KErrNone;
                }
            // Let the system to reboot..
            TPckg<TInt> tmp(  result );
            iTestCase.NotifyRemoteMsg( tmp, EStifCmdRebootProceed );
            Start();
            return;
            }
        default:
            User::Leave( KErrGeneral );
        }
        
    if( iStatus.Int() == KErrNone )
        {   
        Start();    
        }
    
    }

/*
-------------------------------------------------------------------------------

    Class: CUIEngineRemote

    Method: DoCancel

    Description: Cancel active request.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
void CUIEngineRemote::DoCancel()
    {
    __TRACE( KPrint, ( _L( "CUIEngineRemote::DoCancel") ) );
    iTestCase.CancelAsyncRequest( ETestCaseNotifyRemoteType );
    
    }
/*
-------------------------------------------------------------------------------

    Class: CUIEngineRemote

    Method: RunError

    Description: None

    Parameters: TInt aError: in: Symbian error code

    Return Values: Error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/    
TInt CUIEngineRemote::RunError(TInt aError)
    {
    return aError;
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
