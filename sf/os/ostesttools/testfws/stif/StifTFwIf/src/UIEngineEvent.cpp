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
* Description: CUIEngineEvent: This object executes test cases 
* from STIF Test Framework.
*
*/

// INCLUDE FILES
#include <e32base.h>
#include <e32svr.h>
#include <stifinternal/UIEngineContainer.h>
#include "UIEngineEvent.h"
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

    Class: CUIEngineEvent

    Method: CUIEngineEvent

    Description: Default constructor

    C++ default constructor can NOT contain any code, that
    might leave.

    Parameters: None
    
    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
CUIEngineEvent::CUIEngineEvent( CUIEngine* aUIEngine) :
    CActive( CActive::EPriorityStandard ),
    iState( EEventIdle ),
    iUIEngine( aUIEngine ),
    iEventPckg( iEvent )
    {
    __TRACE( KPrint, ( _L( "CUIEngineEvent::CUIEngineEvent") ) );
    __ASSERT_ALWAYS( aUIEngine, User::Panic( _L("Null pointer"), KErrGeneral ) );

    CActiveScheduler::Add( this );
    }


/*
-------------------------------------------------------------------------------

    Class: CUIEngineEvent

    Method: ConstructL

    Description: Symbian OS second phase constructor

    Symbian OS default constructor can leave.

    Parameters: None

    Return Values: None

    Errors/Exceptions: Leaves if called Open method returns error

    Status: Approved

-------------------------------------------------------------------------------
*/
void CUIEngineEvent::ConstructL()
    {

    }


/*
-------------------------------------------------------------------------------

    Class: CUIEngineEvent

    Method: NewL

    Description: Two-phased constructor.
    
    Parameters: CUIEngineContainer* CUIEngineContainer: in: Pointer to CUIEngineContainer Interface
                TTestInfo& aTestInfo: in: Test info

    Return Values: CUIEngineEvent* : Pointer to created runner object

    Errors/Exceptions: Leaves if memory allocation for CUIEngineEvent fails
                       Leaves if ConstructL leaves

    Status: Draft

-------------------------------------------------------------------------------
*/
CUIEngineEvent* CUIEngineEvent::NewL( CUIEngine* aUIEngine )
    {
    CUIEngineEvent* self =  
        new ( ELeave ) CUIEngineEvent( aUIEngine );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }


/*
-------------------------------------------------------------------------------

    Class: CUIEngineEvent

    Method: ~CUIEngineEvent

    Description: Destructor
    
    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
CUIEngineEvent::~CUIEngineEvent()
    {
    
    __TRACE( KPrint, ( _L( "CUIEngineEvent::~CUIEngineEvent()") ) );
    Cancel();
    
    if( iState == EEventWaitCompleted )
        {
        // Release event
        Release();
        }
    
    }


/*
-------------------------------------------------------------------------------

    Class: CUIEngineEvent

    Method: Request

    Description: Request event.

    Parameters: RTestCase& aTestCase: in: Handle to test case
                TFullTestResultPckg& aFullTestResultPckg: in: Handle to TFullTestResultPckg

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
TInt CUIEngineEvent::Request( TDesC& aEventName, 
                               TUint32 aMaster, 
                               TUint32 aSlave )
    {
    
    __TRACE( KPrint, ( _L( "CUIEngineEvent::Request %S"), &aEventName ) );
    
    iState = EEventRequested;
    
    iMaster = aMaster;
    iSlave = aSlave; 
    iEvent.SetType( TEventIf::EReqEvent );
    iEvent.SetName( aEventName );
    
    TRequestStatus status;
    iUIEngine->iTestEngine.Event( iEventPckg, status ); 
    User::WaitForRequest( status );
    
    if( status.Int() == KErrNone )
        {
        // Enable event waiting
        iState = EEventWait;            
        iEvent.SetType( TEventIf::EWaitEvent );
        SetActive();
        iUIEngine->iTestEngine.Event( iEventPckg, iStatus ); 
        }
    return status.Int();
        
    }
    
/*
-------------------------------------------------------------------------------

    Class: CUIEngineEvent

    Method: Release

    Description: Release event.

    Parameters: RTestCase& aTestCase: in: Handle to test case
                TFullTestResultPckg& aFullTestResultPckg: in: Handle to TFullTestResultPckg

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
TInt CUIEngineEvent::Release()
    {
    
    __TRACE( KPrint, ( _L( "CUIEngineEvent::Release %S"), &iEvent.Name() ) );
    Cancel();
   
    TRequestStatus status;
    // Release event
    iState = EEventReleased;
    iEvent.SetType( TEventIf::ERelEvent );
    iUIEngine->iTestEngine.Event( iEventPckg, status ); 
    User::WaitForRequest( status ); 
    
    return status.Int();
    
    }
    

/*
-------------------------------------------------------------------------------

    Class: CUIEngineEvent

    Method: RunL

    Description: RunL handles completed requests.

    Parameters: None

    Return Values: None

    Errors/Exceptions: Leaves if iStatus is not KErrNone, error is handled in
                       RunError called by CActiveObject

    Status: Draft

-------------------------------------------------------------------------------
*/
void CUIEngineEvent::RunL()
    {
    
    __TRACE( KPrint, ( _L( "CUIEngineEvent::RunL") ) );

    // Error handled in RunError
    User::LeaveIfError ( iStatus.Int() );
    
    switch( iState )
        {
        case EEventWait:
            {
            // Create response 
            CStifTFwIfProt* resp = CStifTFwIfProt::NewL();
            CleanupStack::PushL( resp );    
            
            resp->CreateL();
            resp->Append( CStifTFwIfProt::MsgType, CStifTFwIfProt::EMsgResponse );
            resp->AppendId( iSlave );
            resp->AppendId( iMaster );
            resp->Append( CStifTFwIfProt::MsgType, CStifTFwIfProt::EMsgRemote );
            resp->Append( CStifTFwIfProt::CmdType, CStifTFwIfProt::ECmdRequest );
            resp->Append( CStifTFwIfProt::EventStatus, 
                         CStifTFwIfProt::EEventSet );
            resp->Append( iEvent.Name() );
            if( iEvent.EventType() == TEventIf::EState )
                {
                resp->Append( CStifTFwIfProt::EventStatusParams, 
                             CStifTFwIfProt::EEventType,
                             CStifTFwIfProt::EventType, 
                             TEventIf::EState );
                } 
            // Send response
            iUIEngine->RemoteMsg( NULL, resp->Message() );
            CleanupStack::PopAndDestroy( resp );    
            
            if( iEvent.EventType() == TEventIf::EIndication )
                {            
                // Enable indication event waiting again
                iEvent.SetType( TEventIf::EWaitEvent );
                SetActive();
                iUIEngine->iTestEngine.Event( iEventPckg, iStatus ); 
                }
            else  // state event, set only once for one request
                {                
                iState = EEventWaitCompleted;
                }
            }
            break;
        case EEventWaitCompleted:
        case EEventRequested:
        case EEventReleased:
        default:
            __TRACE( KError, ( _L( "CUIEngineEvent::RunL: Illegal state %d"), iState ) );
            User::Leave( KErrGeneral );
        }
    }

/*
-------------------------------------------------------------------------------

    Class: CUIEngineEvent

    Method: DoCancel

    Description: Cancel active request.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
void CUIEngineEvent::DoCancel()
    {
    
    __TRACE( KPrint, ( _L( "CUIEngineEvent::DoCancel") ) );
    TRequestStatus status;

    switch( iState )
        {
        case EEventWait:
            // First cancel waiting
            iEvent.SetType( TEventIf::ECancelWait );
            iUIEngine->iTestEngine.Event( iEventPckg, status );
            User::WaitForRequest( status ); 
            iState = EEventWaitCompleted;
            break;
        default:
            iUIEngine->iTestEngine.CancelAsyncRequest( ETestEngineEvent ); 
            iState = EEventIdle;
            break;
        }
    
    }


/*
-------------------------------------------------------------------------------

    Class: CUIEngineEvent

    Method: RunError

    Description: Handle errors from STIF TestFramework

    Parameters: TInt aError: in: Symbian OS error: Error code
    
    Return Values: TInt KErrNone: Always returned KErrNone

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
TInt CUIEngineEvent::RunError( TInt aError )
    {
    __TRACE( KPrint, ( _L( "CUIEngineEvent::RunError") ) );

    switch( iState )
        {
        case EEventRequested:
        case EEventWait:
        case EEventWaitCompleted:
        case EEventReleased:
            {
            // Create response
            CStifTFwIfProt* resp = NULL; 
            TRAPD( err, resp = CStifTFwIfProt::NewL(); );
            if( err != KErrNone )
                {
                return KErrNone;
                }

            resp->CreateL();
            resp->Append( CStifTFwIfProt::MsgType, CStifTFwIfProt::EMsgResponse );
            resp->AppendId( iSlave );
            resp->AppendId( iMaster );
            resp->Append( CStifTFwIfProt::MsgType, CStifTFwIfProt::EMsgRemote );
            resp->Append( CStifTFwIfProt::CmdType, CStifTFwIfProt::ECmdRequest );
            resp->Append( CStifTFwIfProt::EventStatus, 
                         CStifTFwIfProt::EEventError );
            resp->Append( iEvent.Name() );
            resp->Append( CStifTFwIfProt::EventStatusParams, 
                             CStifTFwIfProt::EEventResult,
                             aError );
            
            // Send response
            iUIEngine->RemoteMsg( NULL, resp->Message() ); 
            delete resp;
            }
            break;
        default:
            __TRACE( KError, ( _L( "CUIEngineEvent::RunError: Illegal state %d"), iState ) );
            return aError;
        }
    
    return KErrNone;
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
